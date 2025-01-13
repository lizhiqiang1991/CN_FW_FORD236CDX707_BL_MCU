/***************************************************************************
*\file hal_wdt.h 
****************************************************************************/

#ifndef HAL_WDT_H
#define HAL_WDT_H

uint8_t HAL_WDT_Init  (wdt_config_t* tsConfig);
uint8_t HAL_WDT_DeInit(void);
uint8_t HAL_WDT_Active(uint8_t u8Enable);
uint8_t HAL_WDT_Interrupt_Set(uint8_t u8Enable, uint8_t u8Priority);
uint8_t HAL_WDT_Clear(void);
uint8_t HAL_WDT_Warn_Callback_Register(HAL_WDT_WARN_CALLBACK pCallback);

#endif

  