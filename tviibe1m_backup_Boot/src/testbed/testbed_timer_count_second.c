/*

File    : testbed_timer_count_second.c
Purpose : Timer testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"
 
 
#define USER_LED        KIT_LED_1_BLUE
#define USER_LED2       KIT_LED_4_BLUE

#define LED_TIMER        TIMER_GROUP0_CNT0
#define TEN_SEC_TIMER    TIMER_GROUP0_CNT1 

 
static void LED_Timer_Handler(void* context)
{
    UNUSED (context);
    HAL_GPIO_PinToggle(USER_LED);  
}

static void LED_10Sec_Timer_Handler(void* context)
{
    UNUSED (context);
    HAL_GPIO_PinToggle(USER_LED2);
}

void testbed_timer_count_sec(void) 
{
  __enable_irq();
  timer_config_t gts_10Sec_Timer_Config = 
  {
      .u32Period = 10000u,                    /* 10000 mSec = 10 Sec*/
      .u8RunType = TIMER_CONTINUOUS,          /* Continuous*/
      .u8PeriodUnit = TIMER_PERIOD_MILLI_SEC, /* mSec*/
      .u8InterruptEnable = FUNC_ENABLE,   
  };
   
  /*LED gpio init config as output*/
  HAL_GPIO_Pin_Init(USER_LED ,&gtdPinOutputConfig);
  HAL_GPIO_Pin_Init(USER_LED2,&gtdPinOutputConfig);
  
  /*Timer init*/
  CY_ASSERT (HAL_Timer_Init(LED_TIMER,&gts_1Sec_Timer_Config) == DRIVER_TRUE); /*Timer Continuous*/
  CY_ASSERT (HAL_Timer_Init(TEN_SEC_TIMER,&gts_10Sec_Timer_Config) == DRIVER_TRUE); /*Timer Continuous*/
   
  /*Timer register callback*/
  HAL_Timer_Callback_Register (LED_TIMER,LED_Timer_Handler,NULL);
  HAL_Timer_Callback_Register (TEN_SEC_TIMER,LED_10Sec_Timer_Handler,NULL);
  
  /*Timer Enable*/
  HAL_Timer_Active(LED_TIMER,FUNC_ENABLE);
  HAL_Timer_Active(TEN_SEC_TIMER,FUNC_ENABLE);
  
  /*LED_TIMER & TEN_SEC_TIMER timer start*/ 
  HAL_Timer_Start(LED_TIMER,FUNC_ENABLE);
  HAL_Timer_Start(TEN_SEC_TIMER,FUNC_ENABLE);
   
  for (;;)
  {
    ;
  }
}
