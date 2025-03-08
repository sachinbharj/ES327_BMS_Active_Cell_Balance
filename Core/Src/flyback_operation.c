/**
  ******************************************************************************
  * @file           : flyback_operation.c
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */

/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "flyback_operation.h" // Include header file for flyback operation functions
#include "tim.h" // Include timer library for PWM control
#include <stdio.h> // Include standard input/output functions for debugging
#include "adc.h" // Include ADC library for current sensing
#include "usart.h" // Include UART library for serial communication
#include <string.h> // Include string manipulation functions

/* ***** GLOBAL VARIABLES ***** */
float balancing_current_ADC_voltage; // Stores ADC voltage reading for balancing current measurement
uint16_t balancing_current_ADC[1]; // Stores ADC raw value for balancing current measurement
uint8_t buffer_ADC[32]; // Buffer for transmitting data via UART


/**
 * @brief  Activates the flyback converter for 1A balancing current
 *
 * This function sets an initial PWM duty cycle for the flyback converter
 * and includes a feedback loop to adjust the duty cycle to maintain a
 * balancing current of 1A
 */
void flyback_1A_balance()
{
	int duty_cycle_1A = 28; // Initial PWM duty cycle for 1A current (28%)

	printf("\n********** Flyback Converter Activated **********\n"); // Print flyback operation message
	TIM1->CCR1 = duty_cycle_1A; // Set PWM duty cycle
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // Start PWM on TIM1, Channel 1

	HAL_Delay(1000); // 1 second delay for balancing current stabilisation

	// Feedback loop to maintain 1A balancing current
	for (int i = 0; i < 5; i++)
	{
		float balancing_current = read_balancing_current(); // Read measured output current

		printf("Balancing Current: %.2f A | PWM Duty Cycle: %d%%\n", balancing_current, duty_cycle_1A); // Print balancing current and duty cycle

		// Adjust duty cycle based on current measurement (feedback loop)
		if (balancing_current < 0.95) // If balancing current is smaller than 0.95A
		{
			duty_cycle_1A += 1; // Increase duty cycle by 1% to increase output current
		} else if (balancing_current > 1.05) // Else if balancing current is larger than 1.05A
		{
			duty_cycle_1A -=1; // Decrease duty cycle by 1% to decrease output current
		}

		HAL_Delay(1000); // 1 second delay for next feedback cycle
	}

}


/**
 * @brief  Activates the flyback converter for 4A balancing current
 *
 * Same as 1A function - this can be used as alternative for high balancing current flow
 */
void flyback_4A_balance()
{
	int duty_cycle_4A = 0.76; // Initial duty cycle for 4A current

	printf("***Flyback Converter Activated***\n"); // Print flyback operation message
	TIM1->CCR1 = duty_cycle_4A; // Set PWM duty cycle
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // Start PWM on TIM1, Channel 1

	HAL_Delay(1000); // 1 second delay for balancing current stabilisation

	// Feedback loop to maintain 4A balancing current
	for (int i = 0; i < 10; i++)
	{
		float balancing_current = read_balancing_current(); // Read measured output current

		printf("\nBalancing Current: %.2f A | PWM Duty Cycle: %d%%\n", balancing_current, duty_cycle_4A);  // Print balancing current and duty cycle

		// Adjust duty cycle based on current measurement (feedback loop)
		if (balancing_current < 3.8) // If balancing current is smaller than 3.8A
		{
			duty_cycle_4A += 1; // Increase duty cycle by 1% to increase output current
		} else if (balancing_current > 4.2)  // Else if balancing current is larger than 4.2A
		{
			duty_cycle_4A -=1; // Decrease duty cycle by 1% to decrease output current
		}

		HAL_Delay(1000); // 1 second delay for next feedback cycle
	}

}


/**
 * @brief Read the balancing current using ADC measurement
 */
float read_balancing_current()
{
	uint16_t balancing_current_ADC[1]; // Variable to store ADC raw reading
	float current; // Variable to store computed current value

	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED); // Calibrate ADC before use

	HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&balancing_current_ADC, 1); // Start ADC conversion via direct memory allocation (DMA)

	balancing_current_ADC_voltage = ((float)(*balancing_current_ADC) / 4096.0) * 3.3; // Calculate voltage: 4096 for 12-bit, 3.3 for STM32 input range

	current = (balancing_current_ADC_voltage / 20) / 0.005; // Convert voltage to current (20V/V gain, 5mΩ shunt resistor)

	return current; // Return calculated current value
}


/**
 * @brief  Stop the flyback converter operation
 */
void terminate_flyback()
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1); // Stop PWM output on TIM1, Channel 1
	printf("PWM Terminated!\n"); // Print termination message

	HAL_Delay(500); // 500ms delay before next voltage readings
}

