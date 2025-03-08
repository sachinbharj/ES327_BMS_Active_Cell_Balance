/**
  ******************************************************************************
  * @file           : molicel_soc_lookup.c
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */

/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "molicel_soc_lookup.h" // Include the header file

/**
 * Function to determine the SOC based on voltage reading using predefined lookup table (see molicel_soc_lookup.h)
 */
int volt_to_soc(float voltage)
{
    // If voltage is below the lowest threshold in lookup table, return 0% SOC
    if (voltage <= soc_table[0].voltage)
    {
        return soc_table[0].soc;
    }

    // If voltage is above the highest threshold in lookup table, return 100% SOC
    if (voltage >= soc_table[SOC_TABLE_SIZE - 1].voltage)
    {
        return soc_table[SOC_TABLE_SIZE - 1].soc;
    }

    // Iterate through the SOC table to find two voltage points for interpolation
    for (int i = 0; i < SOC_TABLE_SIZE - 1; i++)
    {
        if (voltage >= soc_table[i].voltage && voltage <= soc_table[i + 1].voltage) // If voltage falls between two points in the table
        {
            float v1 = soc_table[i].voltage; // Lower bound voltage
            float v2 = soc_table[i + 1].voltage; // Upper bound voltage
            int soc1 = soc_table[i].soc; // Lower bound SOC value
            int soc2 = soc_table[i + 1].soc; // Upper bound SOC value

            // Perform linear interpolation between the two points to estimate the SOC
            return soc1 + (int)((voltage - v1) / (v2 - v1) * (soc2 - soc1));
        }
    }

    // Default case if no valid SOC is found (should never be reached)
    return -1;
}


