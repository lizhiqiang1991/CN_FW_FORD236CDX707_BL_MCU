/******************************************************************************
;       Program		: CrcApp.h
;       Function	: CRC Calculation Functions 
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 09 / 29
;       Author		: Joel Wang
******************************************************************************/
#ifndef CrcApp_H
#define CrcApp_H 
/*---------------------------- Define Constant ------------------------------*/
#define		INIT_CRC32		0xFFFFFFFFU
#define		INIT_CRC16		0x0000U
#define		SIZE_CRC32		256U
#define		SIZE_CRC16		256U
/*---------------------------- Declare Function -----------------------------*/ 
uint32_t CrcApp_Crc32_Calculation(uint8_t *pData, uint32_t u32Len, uint32_t u32Crc32);
uint16_t CrcApp_Crc16_Calculation(uint8_t *pData, uint32_t u32Len);
uint8_t CrcApp_CalculateSum8(uint8_t *Buffer, uint8_t Length);
uint16_t CrcApp_CalculateSum16(uint8_t *Buffer, uint8_t Length);
uint8_t CrcApp_CalculateCheckSum8(uint8_t *Buffer, uint8_t Length);
void CrcApp_CalculateAllDataCheckSum16(uint8_t *Buffer, uint8_t Length, uint8_t reset);
uint16_t CrcApp_GetCheckSum16(void);
#endif
