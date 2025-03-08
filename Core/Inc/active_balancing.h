/**
  ******************************************************************************
  * @file           : active_balancing.h
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */

#ifndef INC_ACTIVE_BALANCING_H_
#define INC_ACTIVE_BALANCING_H_

/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "main.h" // Main application header
#include "pack_config.h" // Configuration settings for pack configuration

/* ***** EXTERNAL VARIABLES ***** */
extern float soc_values[NOC]; // Array storing SOC values for all cells

/* ***** FUNCTION PROTOTYPES ***** */
void active_balance_trigger(); // Triggers active balancing process
void detect_imbalanced_cell(); // Identifies the most imbalanced cell
void balance_undercharged_cell(int cell_index); // Balances an undercharged cell
void balance_overcharged_cell(int cell_index); // Balances an overcharged cell

#endif
