/******************************************************************************
;       Program		: Uart1App.c 
;       Function	: Declare and Define Debug SERCOM1 UART Function
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
;       Describe 	: Code Gen Debug UART function. 
;					  Include SERCOM1_UART and STDIO_UART components.
;					  UART_TX = PB30
;					  UART_RX = PB31
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
/*---------------------------- Include Library -------------------------------*/
#include "cy_project.h"
#include "cy_device_headers.h"
#include <stdio.h>
#include "hal_config.h"
#include "InitApp.h"
/*---------------------------- Declare Global Variable ----------------------*/	
#if 0
//static uint8_t gu8StrShowBuff[SIZE_SHOW_STR] = {0U};
//static uint8_t gu8ValShowBuff[SIZE_SHOW_VAL] = {0U}; 
//static uint16_t gu16StrShowLen; 
#endif
/*---------------------------- Start Program --------------------------------*/
/******************************************************************************
;	Function Name			:	uint8_t Uart1App_Init(uint8_t u8Para)
;	Function Description	:	Initialize debug UART function.
;	Parameters				:	[u8Para] - NOTHING
;	Return Values			:	NOTHING
;	Source ID				:
******************************************************************************/
#if DEBUG_UART
uint8_t Uart1App_Init(uint8_t u8Para)
{
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);

	(void)(u8Para);
	return u8Para;
}
#endif
/*---------------------------------------------------------------------------*/ 
