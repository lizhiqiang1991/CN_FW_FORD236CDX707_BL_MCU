/******************************************************************************
;       Program		: ClockApp.c 
;       Function	: The Main Program
;       Chip		: Cypress CYT2B6
;       Clock		: 
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
;       Describe 	:
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "InitApp.h"
/*---------------------------- Declare Global Variable ----------------------*/
//static NvmCtrl_Config_ts NvmCtrlInit;
/*---------------------------- Start Program --------------------------------*/
/******************************************************************************
;       Function Name			:	uint8_t ClockApp_Init(uint8_t u8Para)
;       Function Description	:	Initialize system clock.
;       Parameters				:	NOTHING
;       Return Values			:	NOTHING
;		Source ID				:
******************************************************************************/
uint8_t ClockApp_Init(uint8_t u8Para)
{
    HAL_Clock_Init(&gtsClockConfig);
    
	(void)(u8Para);
	return u8Para;
}
/*---------------------------------------------------------------------------*/ 
