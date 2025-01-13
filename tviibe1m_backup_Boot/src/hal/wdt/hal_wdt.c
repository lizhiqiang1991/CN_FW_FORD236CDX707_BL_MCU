/***************************************************************************
*\file hal_wdt.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_wdt_def.h"
#include "hal_wdt.h"

#define WDT_DEBUG  (0)

static uint32_t u32EnableTimeout = 0u;
static HAL_WDT_WARN_CALLBACK pWdt_Warn_Callback = NULL;

static void HAL_WDT_Warm_ISR(void)
{
    if (NULL != pWdt_Warn_Callback)
    {
        (*pWdt_Warn_Callback)();
    }
    
    Cy_WDT_ClearInterrupt();
}

uint8_t HAL_WDT_Init (wdt_config_t* tsConfig)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    uint32_t interruptState;
    
    if (NULL != tsConfig)
    {
      if (
         ((tsConfig->u8LowerAction == WDT_LOW_ACTION_NONE )  
         || (tsConfig->u8LowerAction == WDT_LOW_ACTION_RESET))
      &&    
        ((tsConfig->u8UpperAction == WDT_UPP_ACTION_NONE )   
         || (tsConfig->u8UpperAction == WDT_UPP_ACTION_RESET))
      &&   
        ((tsConfig->u8WarnAction == WDT_WARN_ACTION_NONE )      
         || (tsConfig->u8WarnAction == WDT_WARN_ACTION_INT  ))
      &&
        ((tsConfig->u8InterruptEnable == FUNC_DISABLE)
         || (tsConfig->u8InterruptEnable == FUNC_ENABLE)
        )
      &&((tsConfig->u8NvicPriority > 1u))      
       )
      {
          u32EnableTimeout = tsConfig->u32Timeout;
          
          Cy_WDT_Disable();
          interruptState = Cy_SysLib_EnterCriticalSection();
          /* Unlock the WDT registers by two writes */
          Cy_WDT_Unlock();
        
          Cy_WDT_SetLowerLimit  (tsConfig->u8LowerLimit );
          Cy_WDT_SetUpperLimit  (tsConfig->u8UpperLimit );
          Cy_WDT_SetWarnLimit   (tsConfig->u8WarnLimit  );
          Cy_WDT_SetLowerAction ((cy_en_wdt_lower_upper_action_t) tsConfig->u8LowerAction);
          Cy_WDT_SetUpperAction ((cy_en_wdt_lower_upper_action_t) tsConfig->u8UpperAction);
          Cy_WDT_SetWarnAction  ((cy_en_wdt_warn_action_t) tsConfig->u8WarnAction );
          /*
            Set automatically service when the count value reaches WARN_LIMIT.
            This function is used for WARN_LIMIT.
            The default value is off (CY_WDT_DISABLE) to let the user to clear the WDT
          */
          Cy_WDT_SetAutoService (CY_WDT_DISABLE);
/* 
    It needs when using debugger but MCU WILL NOT BE RESET BY WDT 
*/ 
#if WDT_DEBUG == 1
          Cy_WDT_SetDebugRun(CY_WDT_ENABLE);  
#endif  

          /* Lock the WDT registers */
          Cy_WDT_Lock();
          Cy_SysLib_ExitCriticalSection(interruptState);
          u8HalResult = HAL_WDT_Interrupt_Set(tsConfig->u8InterruptEnable , tsConfig->u8NvicPriority);
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

uint8_t HAL_WDT_DeInit(void)
{
    cy_stc_sysint_irq_t stc_sysint_irq_cfg_wdt =
    {
        .sysIntSrc = srss_interrupt_wdt_IRQn,
        .intIdx    = CYHAL_CPU_WDT_IRQn,
        .isEnabled = true,
    };
    
    Cy_WDT_UnmaskInterrupt();
    NVIC_DisableIRQ(stc_sysint_irq_cfg_wdt.intIdx);
    Cy_WDT_ClearWatchdog();
    Cy_WDT_Disable();
    u32EnableTimeout = 0u;
    pWdt_Warn_Callback = NULL;
    return DRIVER_TRUE;
}

uint8_t HAL_WDT_Interrupt_Set(uint8_t u8Enable, uint8_t u8Priority)
{
    cy_stc_sysint_irq_t stc_sysint_irq_cfg_wdt =
    {
        .sysIntSrc = srss_interrupt_wdt_IRQn,
        .intIdx    = CYHAL_CPU_WDT_IRQn,
        .isEnabled = true,
    };

    uint8_t u8HalResult = DRIVER_FALSE;
    if ( ((u8Enable == FUNC_DISABLE) || (u8Enable == FUNC_ENABLE))
       &&
         (u8Priority > 1u)
       )
    {
        if (FUNC_ENABLE == u8Enable)
        {
            Cy_WDT_MaskInterrupt();
            /*----------------------------------*/
            /* Interrupt Configuration for WDT  */
            /*----------------------------------*/
            Cy_SysInt_InitIRQ(&stc_sysint_irq_cfg_wdt);
            Cy_SysInt_SetSystemIrqVector(stc_sysint_irq_cfg_wdt.sysIntSrc, HAL_WDT_Warm_ISR);
            NVIC_SetPriority(CYHAL_CPU_WDT_IRQn,u8Priority);
            NVIC_ClearPendingIRQ(stc_sysint_irq_cfg_wdt.intIdx);
            NVIC_EnableIRQ(stc_sysint_irq_cfg_wdt.intIdx);
        }
        else
        {
            Cy_WDT_UnmaskInterrupt();
            NVIC_DisableIRQ(stc_sysint_irq_cfg_wdt.intIdx);
        }
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_WDT_Active(uint8_t u8Enable)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    
    if ( (u8Enable == FUNC_DISABLE) ||(u8Enable == FUNC_ENABLE) )
    {
        if (FUNC_ENABLE == u8Enable)
        {
            Cy_WDT_Enable();
            for (uint32_t i = 0 ; i < u32EnableTimeout ; i++)
            {
                if (Cy_WDT_GetCount() > 0)
                {
                    u8HalResult = DRIVER_TRUE;
                    break;
                }
                Cy_SysLib_DelayUs(1u);
            }
            if (DRIVER_TRUE == u8HalResult)
                u8HalResult = DRIVER_TRUE;
            else
                u8HalResult = DRIVER_TIMEOUT;
        }
        else
        {
            Cy_WDT_Disable();
            u8HalResult = DRIVER_TRUE;
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
           
    return u8HalResult;
}

uint8_t HAL_WDT_Clear(void)
{
    Cy_WDT_ClearWatchdog();
    return DRIVER_TRUE;
}

uint8_t HAL_WDT_Warn_Callback_Register(HAL_WDT_WARN_CALLBACK pCallback)
{
     uint8_t u8HalResult = DRIVER_FALSE;
     if (NULL != pCallback)
     {
         pWdt_Warn_Callback = pCallback;
         u8HalResult = DRIVER_TRUE;
     }
     else
     {
         u8HalResult = DRIVER_PARAMETER_ERROR;
     }
     return u8HalResult;
}
/*** End of file ***/

 