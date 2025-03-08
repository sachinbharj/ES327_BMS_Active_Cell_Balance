/**
  ******************************************************************************
  * @file           : pl455.h
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */

#ifndef PL455_H_
#define PL455_H_

/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "datatypes.h" // Include custom datatype definitions
#include "stdint.h" // Include standard integer types
#include "main.h" // Include main application header
#include "usart.h" // Include UART communication functions
#include "gpio.h" // Include GPIO control functions
#include "stm32g4xx_hal.h" // Include HAL library for STM32 hardware abstraction


// ========================== USER DEFINED MACROS =========================== //

// Write Command Types for cell monitor IC
#define FRMWRT_SGL_R	0x00 // single device write with response
#define FRMWRT_SGL_NR	0x10 // single device write without response
#define FRMWRT_GRP_R	0x20 // group broadcast with response
#define FRMWRT_GRP_NR	0x30 // group broadcast without response
#define FRMWRT_ALL_R	0x60 // general broadcast with response
#define FRMWRT_ALL_NR	0x70 // general broadcast without response

#define TOTALBOARDS	1 // Number of ICs in the stack
#define BAUDRATE 250000 // Defined IC baudrate (recommended)


// ========================== FUNCTION PROTOTYPES =========================== //

// System control functions
void ResetPL455(); // Reset the PL455 monitoring IC
void WakePL455(); // Wake up the PL455 IC from sleep mode
void powerDown(void); // Power down the PL455 IC

// Cell voltage measurement functions
void req_cell_volt(); // Request voltage readings from IC
void getcellVoltages(uint8_t *data,uint8_t NOC,float *volt); // Extract cell voltages from received data

// Register Communication Functions
int  WriteReg(BYTE bID, uint16_t wAddr, uint64_t dwData, BYTE bLen, BYTE bWriteType); // Write data to a specific register
int ReadResp(BYTE * pData, uint16_t bLen); // Read response data from IC
int  WriteFrame(BYTE bID, uint16_t wAddr, BYTE * pData, BYTE bLen, BYTE bWriteType); // Construct and transmit frame to IC

// Delay functions
void delayms(uint16_t ms); // Millisecond delay function
void delayus(uint16_t us); // Microsecond delay function

#endif
