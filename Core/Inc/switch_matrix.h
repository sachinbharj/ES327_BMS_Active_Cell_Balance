/**
  ******************************************************************************
  * @file           : switch_matrix.h
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */
#ifndef INC_SWITCH_MATRIX_H_
#define INC_SWITCH_MATRIX_H_

/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "main.h" // Include main application definitions

/* ***** FUNCTION PROTOTYPES ***** */
void enable_cell_path(int target_cell); // Enable switch matrix path to specified target cell
void switch_matrix_reset(); // Reset switch matrix by disabling all MOSFETs

#endif
