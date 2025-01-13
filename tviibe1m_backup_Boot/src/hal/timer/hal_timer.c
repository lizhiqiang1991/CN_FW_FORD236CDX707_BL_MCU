/***************************************************************************
*\file hal_timer.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "cyhal_hwmgr.h"
#include "hal_timer_def.h"
#include "hal_timer.h"

#define ONE_MEG_HZ     (1000000U)
#define ONE_K_HZ       (1000U)

static cy_stc_tcpwm_counter_config_t const td_Default_Config =
{
    .period             = 1000U - 1U,                          
    .clockPrescaler     = CY_TCPWM_COUNTER_PRESCALER_DIVBY_1,
    .runMode            = CY_TCPWM_PWM_CONTINUOUS,
    .countDirection     = CY_TCPWM_COUNTER_COUNT_UP,
    .debug_pause        = 0U,
    .CompareOrCapture   = CY_TCPWM_COUNTER_MODE_COMPARE,
    .compare0           = 0U,
    .compare0_buff      = 0U,
    .compare1           = 0U,
    .compare1_buff      = 0U,
    .enableCompare0Swap = false,
    .enableCompare1Swap = false,
    .interruptSources   = 0U,
    .capture0InputMode  = 3U,
    .capture0Input      = 0U,
    .reloadInputMode    = 3U,
    .reloadInput        = 0U,
    .startInputMode     = 3U,
    .startInput         = 0U,
    .stopInputMode      = 3U, /* CY_TCPWM_INPUT_RISING_EDGE  =0 ,CY_TCPWM_INPUT_FALLING_EDGE = 1, CY_TCPWM_INPUT_BOTH_EDGES = 2, CY_TCPWM_INPUT_LEVEL = 3 */
    .stopInput          = 0U,
    .capture1InputMode  = 3U,
    .capture1Input      = 0U,
    .countInputMode     = 3U,
    .countInput         = 1U,
    .trigger1           = CY_TCPWM_COUNTER_OVERFLOW,
};

static uint8_t u8InitDivider  = false;
 
static uint8_t HAL_Timer_Config_Check(cy_tcpwm_t* pCounter,timer_config_t* tsConfig)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    if ((NULL != tsConfig) && (NULL != pCounter))
    {
        if ((tsConfig->u32Period >= 1u )             &&\
            (tsConfig->u32Period <= 65535u)          &&\
             (tsConfig->u8RunType == TIMER_CONTINUOUS || 
              tsConfig->u8RunType == TIMER_ONESHOT)   &&\
             (tsConfig->u8PeriodUnit == TIMER_PERIOD_MICRO_SEC ||\
              tsConfig->u8PeriodUnit == TIMER_PERIOD_MILLI_SEC)\
           )
        {
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

static void HAL_Timer_Source_Clock_Init(cy_tcpwm_t* pCounter,uint8_t u8PeriodUnit)
{
    cy_en_divider_types_t eDividerType;
    uint32_t u32DivNum = 0;
    
    if (TIMER_PERIOD_MICRO_SEC == u8PeriodUnit)
    {
        eDividerType = CY_SYSCLK_DIV_8_BIT;
        u32DivNum = CYHAL_DIV_NUM_TCPWM_COUNTER_8_BIT;
    }
    else
    {
        eDividerType = CY_SYSCLK_DIV_24_5_BIT;
        u32DivNum = CYHAL_DIV_NUM_TCPWM_COUNTER_24_BIT;
    }
    
    /* Assign a programmable divider for TCPWM0_GRP0_CNTx */
    Cy_SysClk_PeriphAssignDivider(pCounter->eClkSrc, eDividerType, u32DivNum);
    
    if (u8InitDivider == false)
    {
        uint32_t u32PeriFreq = 0;
        uint32_t u32Div = 0;
        Cy_SysClk_GetClkPeriFrequency(&u32PeriFreq);
        
        /** TIMER_PERIOD_MICRO_SEC 8 bits Divider*/
        u32Div = (u32PeriFreq / ONE_MEG_HZ) - (1U);
        /* Divider 79 --> 80MHz / (79+1) = 1MHz*/
        Cy_SysClk_PeriphSetDivider   (CY_SYSCLK_DIV_8_BIT, CYHAL_DIV_NUM_TCPWM_COUNTER_8_BIT, u32Div); 
        Cy_SysClk_PeriphEnableDivider(CY_SYSCLK_DIV_8_BIT, CYHAL_DIV_NUM_TCPWM_COUNTER_8_BIT);
        
        /** TIMER_PERIOD_MILLI_SEC 24.5 bits Divider*/
        u32Div = (u32PeriFreq / ONE_K_HZ) - (1U);
        /* Divider 79999 --> 80MHz / (79999+1) = 1KHz*/
        Cy_SysClk_PeriphSetFracDivider(CY_SYSCLK_DIV_24_5_BIT, CYHAL_DIV_NUM_TCPWM_COUNTER_24_BIT, u32Div,0u);
        Cy_SysClk_PeriphEnableDivider (CY_SYSCLK_DIV_24_5_BIT, CYHAL_DIV_NUM_TCPWM_COUNTER_24_BIT);
        
        u8InitDivider = true;
    }
    else
    {
        ;/* do nothing*/ 
    }
}

uint8_t HAL_Timer_Init ( uint8_t u8TimerGroup, timer_config_t* tsConfig)
{
  cy_stc_tcpwm_counter_config_t tdCounterConfig;
  uint8_t u8HalResult = DRIVER_REGISTER_ERROR;
  uint32_t u32Status = CY_RET_BAD_PARAM;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  
  if (DRIVER_TRUE == HAL_Timer_Config_Check(pCounter,tsConfig))
  {
    HAL_Timer_Source_Clock_Init(pCounter,tsConfig->u8PeriodUnit);
    
    /* Initialize TCPWM0_GPR0_CNTx as Timer/Counter */
    memcpy(&tdCounterConfig,&td_Default_Config,sizeof(cy_stc_tcpwm_counter_config_t));
    
    tdCounterConfig.period  = tsConfig->u32Period - (1U);
    tdCounterConfig.runMode = tsConfig->u8RunType;
    
    u32Status = Cy_Tcpwm_Counter_Init(pCounter->pBase, &tdCounterConfig);
    if (CY_RET_SUCCESS == u32Status)
    {
      pCounter->u8IsISREnable = tsConfig->u8InterruptEnable;
      u8HalResult = DRIVER_TRUE;
    }
    else
    {
      u8HalResult = DRIVER_FALSE;
    }
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
    
  return u8HalResult;
}

uint8_t HAL_Timer_DeInit (uint8_t u8TimerGroup)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
   
  if (NULL != pCounter)
  {
    Cy_SysInt_DisableIRQ(pCounter->eIntr);
    Cy_Tcpwm_TriggerStopOrKill(pCounter->pBase);
    Cy_Tcpwm_Counter_Disable  (pCounter->pBase);
    Cy_Tcpwm_Counter_DeInit   (pCounter->pBase);
    pCounter->pCallback = NULL;
    pCounter->pContext  = NULL;
    pCounter->u8IsISREnable = FUNC_DISABLE;
    u8HalResult = DRIVER_TRUE;
  }
  else
    u8HalResult = DRIVER_PARAMETER_ERROR;

  return u8HalResult;
}

uint8_t HAL_Timer_Start(uint8_t u8TimerGroup, uint8_t u8Enable)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  
  if (NULL != pCounter && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
  {
    if (FUNC_ENABLE == u8Enable)
      Cy_Tcpwm_TriggerStart(pCounter->pBase);
    else
      Cy_Tcpwm_TriggerStopOrKill(pCounter->pBase);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Active(uint8_t u8TimerGroup, uint8_t u8Enable)
{
  uint8_t u8HalResult  = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  
  if (NULL != pCounter && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
  {
    if (FUNC_ENABLE == u8Enable)
    {
      /* Enable TC Interrupt mask*/
      Cy_Tcpwm_Counter_SetTC_IntrMask(pCounter->pBase);
      Cy_Tcpwm_Counter_Enable(pCounter->pBase);
    }
    else
      Cy_Tcpwm_Counter_Disable(pCounter->pBase);
    
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Counter_Clear(uint8_t u8TimerGroup)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
   
  if (NULL != pCounter)
  {
    Cy_Tcpwm_Counter_SetCounter(pCounter->pBase,0U);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Read(uint8_t u8TimerGroup, uint32_t * pReturn)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  if ( (NULL != pCounter) && (NULL != pReturn) )
  {
    *pReturn = Cy_Tcpwm_Counter_GetCounter(pCounter->pBase);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Callback_Register (uint8_t u8TimerGroup, Timer_ISR pTimerCallback, void* pContext)
{
  return  CYHAL_TCPWM_IRQ_Callback_Set(u8TimerGroup, pTimerCallback, pContext);
}

uint8_t HAL_Timer_Flag_Clear(uint8_t u8TimerGroup)
{
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
  
  if (NULL != pCounter)
  {
    Cy_Tcpwm_Counter_ClearTC_Intr(pCounter->pBase);
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}

uint8_t HAL_Timer_Flag_Get(uint8_t u8TimerGroup)
{
  uint8_t u8HalResult = DRIVER_REGISTER_ERROR;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
   
  if (NULL != pCounter)
  {
    if (Cy_Tcpwm_Counter_GetTC_IntrMasked(pCounter->pBase))
      u8HalResult = DRIVER_TRUE;
    else
      u8HalResult = DRIVER_FALSE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
  return u8HalResult;
}


uint8_t HAL_Timer_Interrupt_Set(uint8_t u8TimerGroup, uint8_t u8Enable, uint8_t u8Priority)
{
  UNUSED(u8Priority);
  
  uint8_t u8HalResult = DRIVER_FALSE;
  cy_tcpwm_t* pCounter = CYHAL_TCPWM_Get(u8TimerGroup);
   
  if (NULL != pCounter  && (FUNC_ENABLE ==  u8Enable|| FUNC_DISABLE ==  u8Enable))
  {
    pCounter->u8IsISREnable = u8Enable;
    if (FUNC_ENABLE == u8Enable)
    {
      Cy_SysInt_EnableIRQ(pCounter->eIntr);
      Cy_Tcpwm_Counter_SetTC_IntrMask(pCounter->pBase);
    }
    else
    {
      Cy_Tcpwm_Counter_ClearTC_Intr(pCounter->pBase);
      Cy_SysInt_DisableIRQ(pCounter->eIntr);
    }
    u8HalResult = DRIVER_TRUE;
  }
  else
  {
    u8HalResult = DRIVER_PARAMETER_ERROR;
  }
     
  return u8HalResult;
}


/*** End of file ***/

  