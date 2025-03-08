/**
  ******************************************************************************
  * @file           : switch_matrix.c
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */

/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "switch_matrix.h" // Include header file for switch matrix operation
#include <stdio.h> // Include standard I/O functions



/**
 * @brief  Enable the switch matrix path to the specified target cell
 */
void enable_cell_path(int target_cell)
{
	switch (target_cell) // Execution based on target cell
	{
		case 1: // Activate MOSFETs for Cell 1 balancing path
			HAL_GPIO_WritePin(MCU_SW_MOS2_GPIO_Port, MCU_SW_MOS2_Pin, GPIO_PIN_SET); // Activate MOSFET 2
			HAL_GPIO_WritePin(MCU_SW_MOS4_GPIO_Port, MCU_SW_MOS4_Pin, GPIO_PIN_SET); // Activate MOSFET 4
			HAL_GPIO_WritePin(MCU_SW_MOS10_GPIO_Port, MCU_SW_MOS10_Pin, GPIO_PIN_SET); // Activate MOSFET 10
			HAL_GPIO_WritePin(MCU_SW_MOS11_GPIO_Port, MCU_SW_MOS11_Pin, GPIO_PIN_SET); // Activate MOSFET 11

			HAL_Delay(500);
			printf("Cell 1 target path enabled...\n"); // Print message
			break;

		case 2: // Activate MOSFETs for Cell 2 balancing path
			HAL_GPIO_WritePin(MCU_SW_MOS1_GPIO_Port, MCU_SW_MOS1_Pin, GPIO_PIN_SET); // Activate MOSFET 1
			HAL_GPIO_WritePin(MCU_SW_MOS3_GPIO_Port, MCU_SW_MOS3_Pin, GPIO_PIN_SET); // Activate MOSFET 3
			HAL_GPIO_WritePin(MCU_SW_MOS9_GPIO_Port, MCU_SW_MOS9_Pin, GPIO_PIN_SET); // Activate MOSFET 9
			HAL_GPIO_WritePin(MCU_SW_MOS10_GPIO_Port, MCU_SW_MOS10_Pin, GPIO_PIN_SET); // Activate MOSFET 10

			HAL_Delay(500);
			printf("Cell 2 target path enabled...\n"); // Print message
			break;

		case 3: // Activate MOSFETs for Cell 3 balancing path
			HAL_GPIO_WritePin(MCU_SW_MOS2_GPIO_Port, MCU_SW_MOS2_Pin, GPIO_PIN_SET); // Activate MOSFET 2
			HAL_GPIO_WritePin(MCU_SW_MOS4_GPIO_Port, MCU_SW_MOS4_Pin, GPIO_PIN_SET); // Activate MOSFET 4
			HAL_GPIO_WritePin(MCU_SW_MOS8_GPIO_Port, MCU_SW_MOS8_Pin, GPIO_PIN_SET); // Activate MOSFET 8
			HAL_GPIO_WritePin(MCU_SW_MOS9_GPIO_Port, MCU_SW_MOS9_Pin, GPIO_PIN_SET); // Activate MOSFET 9

			HAL_Delay(500);
			printf("Cell 3 target path enabled...\n"); // Print message
			break;

		case 4: // Activate MOSFETs for Cell 4 balancing path
			HAL_GPIO_WritePin(MCU_SW_MOS1_GPIO_Port, MCU_SW_MOS1_Pin, GPIO_PIN_SET); // Activate MOSFET 1
			HAL_GPIO_WritePin(MCU_SW_MOS3_GPIO_Port, MCU_SW_MOS3_Pin, GPIO_PIN_SET); // Activate MOSFET 3
			HAL_GPIO_WritePin(MCU_SW_MOS7_GPIO_Port, MCU_SW_MOS7_Pin, GPIO_PIN_SET); // Activate MOSFET 7
			HAL_GPIO_WritePin(MCU_SW_MOS8_GPIO_Port, MCU_SW_MOS8_Pin, GPIO_PIN_SET); // Activate MOSFET 8

			HAL_Delay(500);
			printf("Cell 4 target path enabled...\n"); // Print message
			break;

		case 5: // Activate MOSFETs for Cell 5 balancing path
			HAL_GPIO_WritePin(MCU_SW_MOS2_GPIO_Port, MCU_SW_MOS2_Pin, GPIO_PIN_SET); // Activate MOSFET 2
			HAL_GPIO_WritePin(MCU_SW_MOS4_GPIO_Port, MCU_SW_MOS4_Pin, GPIO_PIN_SET); // Activate MOSFET 4
			HAL_GPIO_WritePin(MCU_SW_MOS6_GPIO_Port, MCU_SW_MOS6_Pin, GPIO_PIN_SET); // Activate MOSFET 6
			HAL_GPIO_WritePin(MCU_SW_MOS7_GPIO_Port, MCU_SW_MOS7_Pin, GPIO_PIN_SET); // Activate MOSFET 7

			HAL_Delay(500);
			printf("Cell 5 target path enabled...\n"); // Print message
			break;

		case 6: // Activate MOSFETs for Cell 6 balancing path
			HAL_GPIO_WritePin(MCU_SW_MOS1_GPIO_Port, MCU_SW_MOS1_Pin, GPIO_PIN_SET); // Activate MOSFET 1
			HAL_GPIO_WritePin(MCU_SW_MOS3_GPIO_Port, MCU_SW_MOS3_Pin, GPIO_PIN_SET); // Activate MOSFET 3
			HAL_GPIO_WritePin(MCU_SW_MOS5_GPIO_Port, MCU_SW_MOS5_Pin, GPIO_PIN_SET); // Activate MOSFET 5
			HAL_GPIO_WritePin(MCU_SW_MOS6_GPIO_Port, MCU_SW_MOS6_Pin, GPIO_PIN_SET); // Activate MOSFET 6

			HAL_Delay(500);
			printf("Cell 6 target path enabled...\n"); // Print message
			break;
	}
}


/**
 * @brief  Reset switch matrix by disabling all MOSFETs
 */
void switch_matrix_reset()
{
	HAL_Delay(500);

	HAL_GPIO_WritePin(MCU_SW_MOS1_GPIO_Port, MCU_SW_MOS1_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 1
	HAL_GPIO_WritePin(MCU_SW_MOS2_GPIO_Port, MCU_SW_MOS2_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 2
	HAL_GPIO_WritePin(MCU_SW_MOS3_GPIO_Port, MCU_SW_MOS3_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 3
	HAL_GPIO_WritePin(MCU_SW_MOS4_GPIO_Port, MCU_SW_MOS4_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 4
	HAL_GPIO_WritePin(MCU_SW_MOS5_GPIO_Port, MCU_SW_MOS5_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 5
	HAL_GPIO_WritePin(MCU_SW_MOS6_GPIO_Port, MCU_SW_MOS6_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 6
	HAL_GPIO_WritePin(MCU_SW_MOS7_GPIO_Port, MCU_SW_MOS7_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 7
	HAL_GPIO_WritePin(MCU_SW_MOS8_GPIO_Port, MCU_SW_MOS8_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 8
	HAL_GPIO_WritePin(MCU_SW_MOS9_GPIO_Port, MCU_SW_MOS9_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 9
	HAL_GPIO_WritePin(MCU_SW_MOS10_GPIO_Port, MCU_SW_MOS10_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 10
	HAL_GPIO_WritePin(MCU_SW_MOS11_GPIO_Port, MCU_SW_MOS11_Pin, GPIO_PIN_RESET); // Deactivate MOSFET 11
}


