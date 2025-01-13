/******************************************************************************
;       Program		: WatchDogApp.c
;       Function	: Monitor MCU System
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
;       Describe 	: Watchdog reset period is WDT_8192_CLK, WDT_1KHz * 8192 = 8 second.
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "InitApp.h"
#include "hal_common.h"
#define WDT_COUNT_VALUE 32767ul /* 1000ms/30.518us=32767, Unit : 1/32768Hz=30.518us, Upper Limit: 1sec (override) */
/*---------------------------- Declare Global Variable ----------------------*/
/*---------------------------- Start Program --------------------------------*/
/******************************************************************************
;       Function Name			:	void WatchDogApp_Initialize(void)
;       Function Description	:	WDG initial
;       Parameters				:	void
;       Return Values			:
;		Source ID				:
******************************************************************************/
void WatchDogApp_Initialize(void)
{
    /* Clear the PORVDDD Reset reason flag */
    /* This is necessary because PORVDDD flag blocks the WDT flag to rise */
    Cy_SysReset_ClearAllResetReasons();

    /*-----------------------*/
    /* Configuration for WDT */
    /*-----------------------*/
    Cy_WDT_Init();                      /* Upper Limit: 1sec and reset */
    Cy_WDT_Unlock();
    Cy_WDT_SetUpperLimit(WDT_COUNT_VALUE);      /* Unit : 1/32768Hz=30.518us, Upper Limit: 1sec (override) */
    Cy_WDT_SetDebugRun(CY_WDT_ENABLE);  /* This is necessary when using debugger */
    Cy_WDT_Lock();
    Cy_WDT_Enable();
}

/******************************************************************************
;       Function Name			:	void WatchDogApp_Clean(void)
;       Function Description	:	Clear WDG counter
;       Parameters				:	u8Nothing
;       Return Values			:	reutrn clean success or not
;		Source ID				:
******************************************************************************/
void WatchDogApp_Clean(void)
{
    /* When testing WDT reset, this code needs to be commented out. */
    Cy_WDT_ClearWatchdog();
}
/*---------------------------------------------------------------------------*/
