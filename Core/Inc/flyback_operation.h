/**
  ******************************************************************************
  * @file           : flyback_operation.h
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */

#ifndef INC_FLYBACK_OPERATION_H_
#define INC_FLYBACK_OPERATION_H_

/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "main.h"  // Include main application definitions
#include <stdio.h> // Include standard I/O functions for debugging

/* ***** FUNCTION PROTOTYPES ***** */
void flyback_1A_balance(); // Activate flyback converter for 1A balancing
void flyback_4A_balance(); // Activate flyback converter for 4A balancing
float read_balancing_current(); // Read balancing current using ADC measurement
void terminate_flyback(); // Terminate flyback converter operation

#endif
