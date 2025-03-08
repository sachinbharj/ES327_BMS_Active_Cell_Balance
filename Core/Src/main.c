/**
  ******************************************************************************
  * @file           : main.c
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */


/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "main.h" // Main header file
#include "adc.h" // ADC peripheral configuration
#include "dma.h" // DMA peripheral configuration (for ADC processing)
#include "usart.h" // UART peripheral configuration
#include "tim.h" // Timer configuration
#include "gpio.h" // GPIO configuration
#include "pl455.h" // BQ76PL455-A monitor IC header file for cell monitoring
#include "stm32g4xx_hal.h" // standard HAL library for STM32G4 series
#include <stdio.h> // Standard input/output functions
#include <stdlib.h> // Standard library functions (e.g. printf)
#include "molicel_soc_lookup.h" // Lookup table for Molicel P45B SOC vs OCV values
#include <string.h> // String manipulation functions
#include <math.h> // Maths library (for sqrt function in std dev determination)
#include "active_balancing.h" // Active balancing algorithm
#include"pack_config.h" // Battery pack configuration


/* ***** DEFINE CONSTANT ***** */

#define STD_DEV_SOC_THRESH 5.0 // Standard deviation threshold for triggering balancing (in percentage), changed based on balancing reqs


/* ***** DEFINE GLOBAL VARIABLES ***** */

BYTE  recFrame[132]; // Buffer for receiving data from cell monitor IC
uint16_t pack_current_ADC[1]; // Array to store ADC reading for pack current
uint8_t buffer[100];  // Buffer for transmitting data via UART
uint8_t recvBuf[1]; // Buffer for receiving data via UART

// Fault status variables
int fault_status = 0; // Fault status flag, initially set to 0 for no error
const float undervolt_thresh = 2.5; // Cell undervoltage threshold
const float overvolt_thresh = 4.2; // Cell overvoltage threshold
const float current_thresh = 1.0; // Pack max current threshold

// First reading
int first_reading = 1; // Flag to ignore first set of readings from cell monitor IC
int check_first_reading(int *first_reading); // Function prototype for checking first readings

// User button
int button_press = 0; // Flag for user button press

// Voltage readings
float volt[6]; // Array to store voltage readings for each cell

// Pack current
float pack_ADC_voltage; // Variable to store pack current ADC voltage
float pack_current; // Variable to store pack current in Amps

// State of Charge (SOC)
float soc_values[NOC]; // Array to store SOC values for each cell
float mean_soc = 0.0; // Mean SOC value
float std_dev_soc = 0.0; // Standard deviation of SOC


/* ***** SYSTEM CLOCK ***** */

void SystemClock_Config(void); // Function to configure system clock


/** @brief  Function to initialise the cell monitor IC
  *         - Configures fault registers, voltage thresholds, pack configuration and auto-addressing
  */
void init_chip()
{
    printf("IC initialisation\n"); // Print initialisation message to serial monitor

	// Mask and clear IC fault registers
	WriteReg(0, 107, 0x8000, 2, FRMWRT_SGL_NR); // Mask chip FAULT
	WriteReg(0, 82, 0xFFC0, 2, FRMWRT_SGL_NR); // Clear fault summary flags
	WriteReg(0, 81, 0x38, 1, FRMWRT_SGL_NR); // Clear system status fault flags

	// Set cell over-voltage and cell under-voltage thresholds (also later done in print_cell_voltages() function)
	WriteReg(0, 144, 0xD1EC, 2, FRMWRT_SGL_NR); // set OV threshold = 4.1000V
	WriteReg(0, 142, 0x6148, 2, FRMWRT_SGL_NR); // set UV threshold = 1.9000V

	// Auto-address all boards
	WriteReg(0, 14, 0x38, 1, FRMWRT_ALL_NR); // Set auto-address mode, internal regulator NPN disabled
	WriteReg(0, 12, 0x08, 1, FRMWRT_ALL_NR); // Enter auto-address mode

	WriteReg(0, 16, 0x1080, 2, FRMWRT_SGL_NR);	// Enable single-end communication

	delayms(10); // 10ms delay for settings to take effect

	WriteReg(0, 60, 0x00, 1, FRMWRT_SGL_NR); // Set 0 mux delay
	WriteReg(0, 61, 0x00, 1, FRMWRT_SGL_NR); // Set 0 initial delay
	WriteReg(0, 62, 0xCC, 1, FRMWRT_SGL_NR); // Set 99.92us ADC sampling period
	WriteReg(0, 7, 0x00, 1, FRMWRT_SGL_NR); // Set no oversampling period

	// Select number of cells to sample
	WriteReg(0, 13, 0x06, 1, FRMWRT_SGL_NR); // Set number of cells to 6
	WriteReg(0, 3, 0x003F0000, 4, FRMWRT_SGL_NR); // Enable 6 cell voltage measurements only

	WriteReg(0, 107, 0x8000, 2, FRMWRT_SGL_NR); // Mask chip FAULT
	WriteReg(0, 82, 0xFFC0, 2, FRMWRT_ALL_NR); // clear all fault summary flags
	WriteReg(0, 81, 0x38, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register
}


/**
  * @brief  Function to redirect printf() output to serial monitor via UART 1
  */
int _write(int file, char *ptr, int len)
{
    HAL_UART_Transmit(&hlpuart1, (uint8_t*) ptr, len, 100); // transmit over UART 1 (USB port)
    return len;
}


/**
  * @brief  Function to request cell voltage readings from the monitoring IC
  */
void req_cell_volt()
{
	WriteReg(0, 2, 0x01, 1, FRMWRT_SGL_R); // Send command to IC to request voltage readings
}


/**
 * @brief  Callback function for handling external GPIO interrupts
 * 		   - Function triggered for external GPIO interrupt (use button press)
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_13) // If user blue button is pressed
	{
		button_press = 1; // Set button press flag
		HAL_GPIO_WritePin(GPIOA, PACK_ENABLE_Pin, GPIO_PIN_SET); // Close pack relay
		printf("Pack relay closed...\n"); // Print message to serial monitor
		printf("---------------------\n"); // Print line break for readability
	}
}


/**
 * @brief  Callback function for handling UART reception completion
 *         - Function triggered to process data once UART receives a complete frame
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart3, recFrame, 15); // Re-enable UART receive interrupt for continuous data reception
}


/**
 * @brief  Checks if a critical fault has been detected and takes necessary action
 */
void fault_flag_status()
{
	if (fault_status) // Check fault status flag
	{
		printf("CRITICAL Fault Detected! Relay Opened ... Program Terminated\n"); // Print error message
		HAL_GPIO_WritePin(GPIOA, PACK_ENABLE_Pin, GPIO_PIN_RESET); // Open pack relay (disable relay GPIO)
		exit(1); // Terminate program execution, microcontroller must be reset to restart
	}
}


/**
 * @brief  Print cell voltages and SOC values to the serial monitor
 * 		   - Also check for under/overvoltage and update fault status flag
 */
void print_cell_voltages()
{
	printf("\n**************** MONITORING STATUS ****************\n"); // Print message for readability

	for (int i = 0; i < NOC; i++) // Iterate through all monitored cells
	{
		soc_values[i] = volt_to_soc(volt[i]); // Convert voltage reading to SOC (see molicel_soc_lookup.c)

		printf( "Cell %d Voltage: %.3fV | SOC = %.1f%%\n", NOC - i, volt[i], soc_values[i]); // Print cell voltages 1 to 6

		if (volt[i] > overvolt_thresh) // If cell voltage is greater than overvoltage threshold
		{
			printf("Cell %d OVERVOLTAGE ERROR: %.3fV (Threshold: %.1fV)\n", NOC - i, volt[i], overvolt_thresh); // Print error message
			fault_status = 1; // Update fault status flag
		} else if (volt[i] < undervolt_thresh) {
			printf("Cell %d UNDERVOLTAGE ERROR: %.3fV (Threshold: %.1fV)\n", NOC - i, volt[i], undervolt_thresh); // Print error message
			fault_status = 1; // Update fault status flag
		}
	}
}


/**
 * @brief  Measure pack current using an ADC reading
 */
void measure_pack_current()
{
	if (button_press) // If button_press flag is set to 1 (from interrupt)
	{
		HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED); // Calibrate ADC1

		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&pack_current_ADC, 1); // Start ADC conversion via direct memory allocation (DMA)

		pack_ADC_voltage = ((float)(*pack_current_ADC) / 4096.0) * 3.3; // Calculate voltage: 4096 for 12-bit, 3.3 for STM32 input range

		pack_current = pack_ADC_voltage / 0.5; // Calculate pack current in Amps (20V/V gain from current sense amplifier)

		sprintf((char*)buffer, "\n************* Pack current: %.3fA\r *************\n", pack_current); // Store pack_current in UART buffer
		HAL_UART_Transmit(&hlpuart1, buffer, strlen((char*)buffer), HAL_MAX_DELAY); // Transmit to serial monitor via UART 1
	}
}


/**
 * @brief  Check if the pack current exceeds the predefined threshold
 */
void pack_overcurrent_check()
{
	if (pack_current > current_thresh) // If measured current is greater than threshold (set to 1A)
	{
		printf("PACK OVERCURRENT ERROR: %.3fA (Threshold: %.1fA)\n", pack_current, current_thresh); // Print error message
		fault_status = 1; // Set fault flag to 1
	}
}

/**
 * @brief  Skips the first reading from the cell monitor IC
 *         - First reading is always invalid (returns 0)
 *         - Triggers undervoltage fault even when not present
 */
int check_first_reading(int *first_reading)
{
	if (*first_reading) // Check if first reading flag is set
	{
		*first_reading = 0; // Clear flag to indicate first reading has been handled
		printf("Skipping initial invalid readings...\n"); // Print status message
		HAL_Delay(500); // 500ms delay for system stabilisation
		return 1; // Indicate to skip this iteration of while loop
	}
	return 0; // Proceed normally with the next while loop iteration
}


/**
 * @brief  Callback function required for ADC conversion
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) // callback for ADC DMA
{
}


/**
 * @brief  Compute mean and standard deviation of the values across all cells
] */
void compute_soc_stats()
{
	float soc_sum = 0.0; // Initialise variable for sum of SOCs and set to 0
	float sum_sq = 0.0; // Initialise variable for sum of squared differences and set to 0

	// Compute the sum of all SOC values
	for (int i = 0; i < NOC; i++)
	{
		soc_sum += soc_values[i];
	}

	mean_soc = soc_sum / NOC; // Calculate mean SOC

	// Compute sum of squared differences from the mean
	for (int i = 0; i < NOC; i++)
	{
		sum_sq += (soc_values[i] - mean_soc) * (soc_values[i] - mean_soc);
	}

	// Compute standard deviation
	std_dev_soc = sqrt(sum_sq / NOC); // Square root of sum of squared differences divided by number of cells (6)

	printf("\n***** SOC Mean: %.2f%% | Standard Deviation: %.2f%% *****\n", mean_soc, std_dev_soc); // Print calculated statistics
}


/**
 * @brief  Evaluate whether balancing is required based on SOC standard deviation
 */
void assess_equalisation()
{
	printf("\n              ---------------------\n"); // Print line break for readability
	printf("\n**************** BALANCING STATUS ****************\n");
	if (std_dev_soc > STD_DEV_SOC_THRESH) // If calculated std dev is greater than predefined threshold (5%)
	{
		printf("Balancing Needed - SOC Std Dev: %.2f%% (Threshold: %.2f%%)\n", std_dev_soc, STD_DEV_SOC_THRESH); // Balancing required message

		active_balance_trigger(); // Trigger active balancing mechanisms (see active_balancing.c)
	} else {
		printf("No Balancing Needed - SOC Std Dev: %.2f%% (Threshold: %.2f%%)\n", std_dev_soc, STD_DEV_SOC_THRESH); // Print no balancing required message
	}
}


/**
  * @brief  Main function where program execution begins
  */
int main(void)
{
	HAL_Init(); // Initialise HAL library

	SystemClock_Config(); // Configure system clock

	// Initialise peripherals
	MX_GPIO_Init(); // GPIOs for IC wakeup & MOSFET switching
	MX_DMA_Init(); // DMA to process ADC readings
	MX_LPUART1_UART_Init(); // LPUART1 for Serial Monitor
	MX_USART3_UART_Init(); // UART 3 for cell monitor IC communication
	MX_ADC1_Init(); // ADC1 for pack current readings
	MX_TIM1_Init(); // Timer for PWM generation
	MX_ADC2_Init(); // ADC2 for flyback balancing current output readings

	// Initialise local variable
	BYTE  bFrame[132]; // Buffer for UART receive from cell monitor IC of length 132 bytes

	powerDown(); // Power down IC initially for soft reset (see pl455.c)

	WakePL455(); // Wakeup sequence for IC (see pl455.c)

	delayms(100); // Small 100ms delay after wakeup

	init_chip(); // Initialise IC registers

	HAL_Delay(1000); // Additional 1 second delay for stability


	// Infinite loop for continuous monitoring and balancing
	while (1)
	{
		req_cell_volt(); // Request cell voltage readings

		ReadResp(bFrame, 15); // Trigger UART receive from IC, 15 bytes of data expected (see pl455.c)

		HAL_Delay(10); // Small 10ms delay after receiving data

		getcellVoltages(recFrame, NOC, volt); // Extract voltage readings from message and store to volt array (see pl455.c)

		// Skip processing of first reading as it always returns invalid
		if (check_first_reading(&first_reading)) {
			continue;  // Skip this iteration of the loop
		}

		print_cell_voltages(); // Print cell voltage readings and calculate SOCs, detect under/overvoltage faults

		compute_soc_stats(); // Compute mean and standard deviation of SOCs

		measure_pack_current(); // Measure and print pack current

		pack_overcurrent_check(); // Check for pack overcurrent condition

		fault_flag_status(); // Open relay and terminate program if a fault is detected

		assess_equalisation(); // Determine if balancing is needed, trigger algorithm if needed

		// Print separator for readability before next reading
		printf("\n----------------------------------------------------------------------------------------------------------------------------\n");

		HAL_Delay(2000); // 2 second delay before next iteration
	}
}



/* ****** AUTO-GENERATED CODE ******
 *  The following code is autogenerated by STM32CubeIDE which is required for program compilation
 * */


/**
 * @brief Configures the system clock settings
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  //Configure the main internal regulator output voltage

  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initialise the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 25;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initialises the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler(); // Call error handler if configuration fails
  }
}


void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq(); // Disable all interrupts
  while (1) // Infinite loop to indicate error state
  {
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
}

#endif
