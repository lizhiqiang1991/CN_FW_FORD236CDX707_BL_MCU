/******************************************************************************
;       Program		: Uart1App.h
;       Function	: Declare and Define SERCOM1 UART Function
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
******************************************************************************/
#ifndef Uart1App_H
#define Uart1App_H

/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
#define TX_PIN_UART     PIN_UART_SCB7_TX /*EVB on board USB to UART*/ 
#define RX_PIN_UART     PIN_UART_SCB7_RX                             
#define UART_SCB        UART_GROPU7                  

#ifdef NDEBUG
#define DEBUG_PF(...)
#else
#define DEBUG_PF(...) HAL_UartPrintf(UART_SCB,##__VA_ARGS__)
#endif

#define     SHOW_PARA_HEX           10U
#define     SHOW_PARA_DEC           6U
#define     SHOW_ASCII_ZERO         0x30U
#define     SHOW_ASCII_X            0x78U
#define     SHOW_ASCII_NUM          0x30U
#define     SHOW_ASCII_CAPS         0x37U
#define     SHOW_STR_A              0x0AU

#define     SIZE_SHOW_VAL           20U
#define     SIZE_SHOW_STR           50U
/*---------------------------- Declare Constant ------------------------------*/
enum ShowMode{
	SHOW_HEX,
	SHOW_DEC
};

enum HexNum{
	HEX_HEAD_1,
	HEX_HEAD_2,
	HEX_VAL_H,
	HEX_VAL_M0,
	HEX_VAL_M1,
	HEX_VAL_M2,
	HEX_VAL_M3,
	HEX_VAL_M4,
	HEX_VAL_M5,
	HEX_VAL_L
};

enum DecNum{
	DEC_VAL_H,
	DEC_VAL_M0,
	DEC_VAL_M1,
	DEC_VAL_M2,
	DEC_VAL_M3,
	DEC_VAL_L
};
	
/*---------------------------- Declare Function ------------------------------*/
uint8_t Uart1App_Init(uint8_t u8Para);
#endif
