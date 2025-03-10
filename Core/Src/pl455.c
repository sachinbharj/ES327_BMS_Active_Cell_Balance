/**
  ******************************************************************************
  * @file           : pl455.c
  * @project        : ES327_BMS_Active_Balance
  ******************************************************************************
  */


/* ***** HEADER FILES ***** */
// Include necessary header files for program to run
#include "string.h" // String manipulation functions
#include "pl455.h" // header file for PL455 cell monitor IC
#include "datatypes.h" // Include custom datatype definitions
#include "stm32g4xx_hal.h" // STM32 HAL library for hardware abstraction
#include "stdint.h" // Standard integer type definitions
#include <stdio.h> // Include standard I/O functions


// Function prototype for CRC16 checksum calculation
uint16_t CRC16(BYTE *pBuf, int nLen);


/**
 * @brief  Power down the PL455 monitoring IC
 */
void powerDown(void)
{
	HAL_Delay(2000); // 2 second delay
	WriteReg(0,12,0x40,1,FRMWRT_SGL_NR); // Write to register to power down chip
}


/**
 * @brief Wake up the PL455 monitoring IC
 */
void WakePL455(void)
{
    printf("Waking IC...\n"); // Print wakeup message to serial monitor

	// Toggle wake signal using GPIO pin PA0
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // Assert wakeup pulse (set low)
	HAL_Delay(0.01); // 10 microsecond delay to meet wakeup timing requirements
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET); // Drive wakeup signal high

	GPIO_InitTypeDef GPIO_InitStruct = {0}; // Initialise GPIO structure for wakeup functionality

	/*Configure GPIO pin : PA0 */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; // Set pin as push-pull output
	GPIO_InitStruct.Pull = GPIO_PULLUP; // Enable internal pull-up resistor
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; // Set speed to low (reduce processing power)
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // Apply GPIO configuration
}


/**
 * @brief Write a value to a register on the PL455 monitoring IC
 *        - bID -> IC address (board ID)
 *        - wAddr -> Register address to write data to
 *        - dwData -> Data value to write
 *        - bLen -> Number of bytes to write
 *        - bWriteType -> Write mode (single / group write, read only, write & only)
 */
int  WriteReg(BYTE bID, uint16_t wAddr, uint64_t dwData, BYTE bLen, BYTE bWriteType)
{
	int bRes = 0;  // Result variable
	BYTE bBuf[4] = {0, 0, 0, 0}; // Buffer to store data before transmission

	// Write data based on length (1 to 8)
	switch(bLen)
	{
	case 1: // write 1 byte of data to IC
		bBuf[0] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 1, bWriteType);
		break;
	case 2: // write 2 byte of data to IC
		bBuf[0] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[1] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 2, bWriteType);
		break;
	case 3: // write 3 byte of data to IC
		bBuf[0] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[1] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[2] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 3, bWriteType);
		break;
	case 4: // write 4 byte of data to IC
		bBuf[0] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[1] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[2] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[3] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 4, bWriteType);
		break;
	case 5: // write 5 byte of data to IC
		bBuf[0] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[1] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[2] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[3] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[4] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 5, bWriteType);
		break;
	case 6: // write 6 byte of data to IC
		bBuf[0] = (dwData & 0x0000FF0000000000) >> 40;
		bBuf[1] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[2] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[3] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[4] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[5] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 6, bWriteType);
		break;
	case 7: // write 7 byte of data to IC
		bBuf[0] = (dwData & 0x00FF000000000000) >> 48;
		bBuf[1] = (dwData & 0x0000FF0000000000) >> 40;
		bBuf[2] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[3] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[4] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[5] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[6] =  dwData & 0x00000000000000FF;;
		bRes = WriteFrame(bID, wAddr, bBuf, 7, bWriteType);
		break;
	case 8: // write 8 byte of data to IC
		bBuf[0] = (dwData & 0xFF00000000000000) >> 56;
		bBuf[1] = (dwData & 0x00FF000000000000) >> 48;
		bBuf[2] = (dwData & 0x0000FF0000000000) >> 40;
		bBuf[3] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[4] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[5] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[6] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[7] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 8, bWriteType);
		break;
	default:
		break;
	}
	return bRes; // Return success or failure status
}


/**
 * @brief  Constructs and transmits a frame over UART to the PL455 IC.
 *         - bID -> IC address (board ID)
 *         - wAddr -> Register address
 *         - pData -> Pointer to data to be written
 *         - bLen -> Number of bytes to write
 * 		   - bWriteType -> Write mode (single / group write, read only, write & only)
 */
int  WriteFrame(BYTE bID, uint16_t wAddr, BYTE * pData, BYTE bLen, BYTE bWriteType)
{
	int	   bPktLen = 0; // Packet length
	BYTE   pFrame[32]; // Frame buffer of 32 bytes
	BYTE * pBuf = pFrame; // Pointer to frame buffer
	uint16_t   wCRC; // CRC checksum variable

	if (bLen == 7 || bLen > 8) // Return 0 if invalid length
		return 0;

	memset(pFrame, 0x7F, sizeof(pFrame)); // Initialise frame with default values

	// Deduce whether to use 8-bit or 16-bit address mode
	if (wAddr > 255)	{
		*pBuf++ = 0x88 | bWriteType | bLen;	// Use 16-bit address mode
		if (bWriteType == FRMWRT_SGL_R || bWriteType == FRMWRT_SGL_NR || bWriteType == FRMWRT_GRP_R || bWriteType == FRMWRT_GRP_NR)
		{
			*pBuf++ = (bID & 0x00FF);
		}
		*pBuf++ = (wAddr & 0xFF00) >> 8;
		*pBuf++ =  wAddr & 0x00FF;
	}
	else {
		*pBuf++ = 0x80 | bWriteType | bLen;	// use 8-bit address mode
		if (bWriteType == FRMWRT_SGL_R || bWriteType == FRMWRT_SGL_NR || bWriteType == FRMWRT_GRP_R || bWriteType == FRMWRT_GRP_NR)
		{
			*pBuf++ = (bID & 0x00FF);
		}
		*pBuf++ = wAddr & 0x00FF;
	}

	// Copy data to frame buffer
	while(bLen--)
		*pBuf++ = *pData++;

	// Compute and append CRC checksum to frame
	bPktLen = pBuf - pFrame;

	wCRC = CRC16(pFrame, bPktLen);
	*pBuf++ = wCRC & 0x00FF;
	*pBuf++ = (wCRC & 0xFF00) >> 8;
	bPktLen += 2;

	// Transmit frame over UART 3
	HAL_UART_Transmit(&huart3, pFrame, (uint16_t)bPktLen, 100);

	return bPktLen;
}


/**
 * @brief  Triggers UART receive interrupt for PL455 response.
 */
int ReadResp(BYTE * pData, uint16_t bLen)
{
	HAL_UART_Receive_IT(&huart3, pData, bLen); // Trigger UART interrupt-based receive
	return 0;
}


/**
 * @brief  Extracts cell voltages from received data.
 * 		   - data -> pointer to received data buffer
 * 		   - NOC -> number of cells (6)
 * 		   - volt -> pointer to array where extracted voltages are stored
 */
void getcellVoltages(uint8_t *data, uint8_t NOC, float *volt)
{
	for(int i=1;i<=NOC;i++) // Iterate through each cell
	{
	*volt ++= (data[2*i-1] << 8 | data[i*2]) * 0.00007666; // Extract and convert raw 16-bit ADC to floating voltage value
	}
}


// CRC16 lookup for PL455
// ITU_T polynomial: x^16 + x^15 + x^2 + 1
const uint16_t crc16_table[256] = {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};


/**
 * @brief  Compute CRC16 checksum for a given buffer
 *         - pBuf -> Pointer to buffer
 *         - nLen -> Length of buffer
 */
uint16_t CRC16(BYTE *pBuf, int nLen)
{
	uint16_t wCRC = 0;
	int i;

	for (i = 0; i < nLen; i++)
	{
		wCRC ^= (*pBuf++) & 0x00FF;
		wCRC = crc16_table[wCRC & 0x00FF] ^ (wCRC >> 8);
	}
	return wCRC;
}


/**
 * @brief  Software function for delay in milliseconds
 */
void delayms(uint16_t ms) {
	  volatile unsigned int delayval;
	  delayval = ms * 8400;   // 8400 are about 1ms
	  while(delayval--);
}


/**
 * @brief  Software function for delay in microseconds
 */
void delayus(uint16_t us) {
	  static volatile unsigned int delayval;
	  delayval = us * 9;
	  while(delayval--);
}
