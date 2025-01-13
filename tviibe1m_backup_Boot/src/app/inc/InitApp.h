/******************************************************************************
;       Program		: InitApp.h
;       Function	: Declare Init and SM Function & Variable
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
******************************************************************************/
#ifndef INITAPP_H
#define INITAPP_H
/*---------------------------- Include File ---------------------------------*/
#include "..\..\app_main.h"
#include "ClockApp.h"
#include "CrcApp.h"
#include "FlashRwApp.h"
#include "FunApp.h"
#include "I2c0SlaveApp.h"
#include "InitApp.h"
#include "PinIOApp.h"
#if DEBUG_UART
#include "Uart1App.h"
#endif
#include "WatchDogApp.h"

__no_init volatile uint16_t u16UpdateKey @0x08001500;
/*---------------------------- Define Constant ------------------------------*/
/*---------------------------- Support Function -----------------------------*/
/*---------------------------- Support Function -----------------------------*/
/*---------------------------- Declare Enum ---------------------------------*/
/*---------------------------- Declare Function -----------------------------*/ 
uint8_t InitApp_Startup(uint8_t u8Para);
uint8_t InitApp_All(uint8_t u8Para);
void InitApp_Startup_DeInit(void);
uint8_t InitApp_StateMachine_Get(void);
uint8_t InitApp_StateMachine_Set(uint8_t u8State);
#endif
