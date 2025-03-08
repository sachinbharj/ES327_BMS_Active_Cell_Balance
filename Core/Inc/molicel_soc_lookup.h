/**
  ******************************************************************************
  * @file           : molicel_soc_lookup.h
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */

#ifndef MOLICEL_SOC_LOOKUP_H_
#define MOLICEL_SOC_LOOKUP_H_

#include <stdint.h> // Standard integer type definitions


/**
 * @brief Structure to map voltage levels to corresponding SOC values
 * Each entry in the table represents a specific voltage value and its associated SOC
 */
typedef struct {
    float voltage;
    int soc;
} VoltageSOCMap;


// Define the number of entries in the SOC lookup table
#define SOC_TABLE_SIZE 23


/**
 * @brief Lookup table for voltage-to-SOC mapping
 * From WMG characterisation tests ************ CONFIDENTIAL ************
 */
static const VoltageSOCMap soc_table[SOC_TABLE_SIZE] = {
    {2.6, 0}, // Minimum voltage corresponding to 0% SOC
    {2.8, 0.8},
    {3.0, 3},
    {3.2, 8.5},
    {3.25, 10},
    {3.3, 13},
    {3.4, 18},
    {3.42, 20},
    {3.5, 25},
    {3.54, 30},
    {3.6, 35},
    {3.63, 40},
    {3.73, 50}, // Mid-range voltage corresponding to 50% SOC
    {3.8, 58},
    {3.81, 60},
    {3.86, 65},
    {3.92, 70},
    {3.96, 75},
    {4.01, 80},
    {4.06, 85},
    {4.09, 90},
    {4.11, 95},
    {4.19, 100}, // Fully charged voltage corresponding to 100% SOC
};

// Function prototype for SOC lookup
int volt_to_soc(float voltage);


#endif
