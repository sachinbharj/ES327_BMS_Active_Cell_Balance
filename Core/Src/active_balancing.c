/**
  ******************************************************************************
  * @file           : active_balancing.c
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */

/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "active_balancing.h" // Include header file for active balancing functions
#include "switch_matrix.h" // Include header for switch matrix control
#include <stdio.h> // Include standard I/O functions
#include <math.h> // Include math functions (for fabs())
#include "tim.h" // Include STM32 HAL Timer library for timing operations
#include "flyback_operation.h" // Include flyback converter control functions

extern float mean_soc; // Access the calculated mean SOC from main.c


/**
 * @brief  Trigger the active balancing process
 */
void active_balance_trigger()
{
	printf("\n             ----------------------\n"); // Print for readability
	printf("             Active Balancing Triggered!\n"); // Print active balancing messsage

	detect_imbalanced_cell(); // Identify the most imbalanced cell and determine if undercharged or overcharged

	printf("------------------------------------------------------\n"); // Print for readability

	HAL_Delay(2000); // 2 second delay for system stability
}



/**
 * @brief  Identifies the most imbalanced cell and determines whether it is overcharged or undercharged
 */
void detect_imbalanced_cell()
{
	int most_imbalanced_index = 0; // Initialise variable for index of most imbalanced cell
	float max_deviation = 0.0; // Initialise variable for max deviation from mean SOC

	// Iterate through all cells to find the most imbalanced one
	for (int i = 0; i < NOC; i++)
	{
		float deviation = fabs(soc_values[i] - mean_soc); // Calculate absolute deviation from mean SOC
		if (deviation > max_deviation)
		{
			max_deviation = deviation;
			most_imbalanced_index = i;
		}
	}

	// Determine if the most imbalanced cell is overcharged or undercharged
	if (soc_values[most_imbalanced_index] > mean_soc) // If cell SOC is greater than mean SOC
	{
		printf("Cell %d is OVERCHARGED (%.1f%% SOC, Mean: %.1f%%)\n", NOC - most_imbalanced_index, soc_values[most_imbalanced_index], mean_soc); // Print overcharged message
		balance_overcharged_cell(most_imbalanced_index); // Call function to balance overcharged cell
	} else
	{
		printf("Cell %d is UNDERCHARGED (SOC: %.1f%%, Mean: %.1f%%)\n", NOC - most_imbalanced_index, soc_values[most_imbalanced_index], mean_soc); // Print undercharged message
		balance_undercharged_cell(most_imbalanced_index); // Call function to balance undercharged cell
	}

	HAL_Delay(1000); // 1 second delay before continuing
}


/**
 * @brief  Algorithm for balancing undercharged cell
 */
void balance_undercharged_cell(int cell_index)
{
	int cell_number = NOC - cell_index; // Convert index to cell number

	printf("\n********** BALANCING UNDERCHARGED CELL %d **********\n", cell_number); // Print undercharged cell message

	enable_cell_path(cell_number); // Activate switch matrix path to the undercharged cell (see switch_matrix.c)

	flyback_1A_balance(); // Initiate flyback converter energy transfer by generating PWM signal (see flyback_operation.c)

	terminate_flyback(); // Stop flyback converter operation (see flyback_operation.c)

	switch_matrix_reset(); // Reset switch matrix (see switch_matrix.c)

	printf("\n********** UNDERCHARGE BALANCING COMPLETED **********\n"); // Print balancing completion message
}


/**
 * @brief  Algorithm for balancing overcharged cell
 */
void balance_overcharged_cell(int cell_index)
{
	int cell_number = NOC - cell_index; // Convert index to cell number

	printf("\n********** BALANCING OVERCHARGED CELL %d **********\n", cell_number); // Print overcharged cell message

	// Iterate through all cells except the overcharged cell
	for (int i = 1; i <= NOC; i++)
	{
		if (i == cell_number) // Skip the overcharged cell
			continue;

		printf("Balancing Cell %d...\n", i); // Print message for cell balanced

		enable_cell_path(i); // Activate switch matrix path to the current cell (see switch_matrix.c)

		flyback_1A_balance(); // Initiate flyback converter energy transfer by generating PWM signal (see flyback_operation.c)

		terminate_flyback(); // Stop flyback converter operation (see flyback_operation.c)

		switch_matrix_reset(); // Reset switch matrix (see switch_matrix.c)
	}

	printf("\n********** OVERCHARGE BALANCING COMPLETED **********\n"); // Print balancing completion message
}

