/******************************************************************************
;       Program		: main.h
;       Function	: Declare Main Function & Variable
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
******************************************************************************/
#ifndef MAIN_H
#define MAIN_H
/*---------------------------- Include Library ------------------------------*/
//#include "definitions.h"/* SYS function prototypes */
#include <stdlib.h>		/* Defines EXIT_FAILURE */
//#include "device.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>	/* Defines true */
#include <stddef.h>		/* Defines NULL */
/*---------------------------- Switch Support Function ----------------------*/
#define		DEBUG_UART			1
#define		WDT_RST				false
#define		BACK_UP				0
/*---------------------------- Define Constant ------------------------------*/
#define 	NOTHING				(uint8_t)(0U)
#define 	YES					0xFFU
#define 	NO					0x00U
#define 	TRUE				0x01U
#define 	FALSE				0x00U
/*---------------------------- Declare Enum ---------------------------------*/
void app_main(void);
enum Status
{
	STATUS_FINISH = 0U,
	STATUS_FAIL = 1U,
	STATUS_WAIT = 2U,
	STATUS_BUSY = 3U
};

enum StateMachine
{
    BL_STARTUP = 0U,
    BL_INIT = 1U, 
    BL_UPDATE = 2U
};

enum Switch{
	DISABLE = 0U,
	ENABLE = 1U
};
/*---------------------------- Declare Function -----------------------------*/
#endif
