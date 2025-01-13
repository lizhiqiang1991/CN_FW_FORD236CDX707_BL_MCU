/***************************************************************************
*\file hal_systick.h 
****************************************************************************/

#ifndef HAL_SYSTICK_H
#define HAL_SYSTICK_H

uint8_t HAL_Systick_Init(systick_config_t* tsConfig);
uint8_t HAL_Systick_Active(uint8_t u8Enable);

uint8_t HAL_Systick_Interrupt_Set(uint8_t u8Enable, uint8_t u8Priority);
uint8_t HAL_Systick_Counter_Set( uint32_t u32Value);
uint8_t HAL_Systick_Counter_Get( uint32_t *pReturn);
uint8_t HAL_Systick_Reload_Set ( uint32_t u32Value);
uint8_t HAL_Systick_Reload_Get ( uint32_t *pReturn);
uint8_t HAL_Systick_Counter_Clear(void);            
uint8_t HAL_Systick_Reload_Clear(void);
uint8_t HAL_Systick_DelayUs(uint32_t u32Delay);

uint8_t HAL_Systick_Callback_Register (Systick_ISR pSystickCallback, void* pContext);
uint8_t HAL_Systick_Flag_Get(uint32_t * pReturn);

/* Leo Add it */
uint8_t HAL_Systick_Init_1msISR(void);

/*obsolete hal_systick functions*/
//uint8_t HAL_Systick_Start(uint8_t u8Enable);
//uint8_t HAL_Systick_Flag_Clear(void);

#endif

  