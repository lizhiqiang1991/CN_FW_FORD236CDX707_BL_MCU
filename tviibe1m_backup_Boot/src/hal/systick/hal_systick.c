/***************************************************************************
*\file hal_systick.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_systick_def.h"
#include "hal_systick.h"

typedef struct
{
    Systick_ISR pIsr;
    void* pContext;
}systick_callback_t;

static systick_callback_t tsSysTick_Callback = {0};
static volatile uint8_t u8Tick_ISR_Enable = false;

#define VALID_TICK_VALUE(x) (((x) & 0xFF000000UL) == 0UL )  /* Systick reload value has only 24bit width.*/
 
static void HAL_SysTick_ISR(void)
{
     /* Verify that tick timer flag was set */
    if ( (0u != Cy_SysTick_GetCountFlag()) && (NULL != tsSysTick_Callback.pIsr) )
    {
        (*tsSysTick_Callback.pIsr)(tsSysTick_Callback.pContext);
    }
}

uint8_t HAL_Systick_Init(systick_config_t* tsConfig)
{
    uint8_t u8HalResult;
    
    if (NULL != tsConfig)
    {
        /*Check config*/
        if (
            ( (tsConfig->u8ClockSource == SYSTICK_CLOCK_SOURCE_CLK_LF)    ||\
              (tsConfig->u8ClockSource == SYSTICK_CLOCK_SOURCE_CLK_IMO)   ||\
              (tsConfig->u8ClockSource == SYSTICK_CLOCK_SOURCE_CLK_ECO)   ||\
              (tsConfig->u8ClockSource == SYSTICK_CLOCK_SOURCE_CLK_TIMER) ||\
              (tsConfig->u8ClockSource == SYSTICK_CLOCK_SOURCE_CLK_CPU) ) &&\
            ( (tsConfig->u8NvicPriority >= 2u) ) && /* !!! PRIORITY 0 and 1 for SYSTEM Call don't use*/  
            ( (tsConfig->u8InterruptEnable == FUNC_ENABLE) ||\
              (tsConfig->u8InterruptEnable == FUNC_DISABLE) ) &&\
              (VALID_TICK_VALUE(tsConfig->u32ReloadCounter)) &&\
              (VALID_TICK_VALUE(tsConfig->u32Counter))\
           )
        {
#if 0
            Cy_SysTick_Init((cy_en_systick_clock_source_t)tsConfig->u8ClockSource, tsConfig->u32ReloadCounter);
#else            
            __ramVectors[CY_SYSTICK_IRQ_NUM] = &HAL_SysTick_ISR;
            NVIC_SetPriority(SysTick_IRQn, tsConfig->u8NvicPriority);
            Cy_SysTick_SetClockSource((cy_en_systick_clock_source_t)tsConfig->u8ClockSource);
 
            Cy_SysTick_SetReload(tsConfig->u32ReloadCounter);
             
            SysTick->VAL = tsConfig->u32Counter;
            SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
#endif            
            if (FUNC_ENABLE == tsConfig->u8InterruptEnable )
                Cy_SysTick_EnableInterrupt();
            else
                Cy_SysTick_DisableInterrupt();
            
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

uint8_t HAL_Systick_Active(uint8_t u8Enable)
{
    uint8_t u8HalResult;
    
    if ((FUNC_ENABLE == u8Enable) || (FUNC_DISABLE == u8Enable))
    {
        if (FUNC_ENABLE == u8Enable)
             SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  
        else
             SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_Systick_Interrupt_Set(uint8_t u8Enable, uint8_t u8Priority)
{
    uint8_t u8HalResult;
    if (
        ((FUNC_ENABLE == u8Enable) || (FUNC_DISABLE == u8Enable))&&
        (u8Priority >= 2u )
       )
    {
        NVIC_SetPriority(SysTick_IRQn, u8Priority);
        
        if(FUNC_ENABLE == u8Enable)
        {
            Cy_SysTick_EnableInterrupt();
        }
        else
        {
            Cy_SysTick_DisableInterrupt();
        }
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_Systick_Counter_Set( uint32_t u32Value)
{
    uint8_t u8HalResult;
    if ( VALID_TICK_VALUE(u32Value) )
    {
        SysTick->VAL = u32Value;
        u8HalResult = DRIVER_TRUE;
    }
    else
        u8HalResult = DRIVER_PARAMETER_ERROR;
    
    return u8HalResult;
}

uint8_t  HAL_Systick_Counter_Get( uint32_t *pReturn)
{
    uint8_t u8HalResult;
    if (NULL != pReturn)
    {
        *pReturn = (SysTick->VAL);
        u8HalResult = DRIVER_TRUE;
    }
    else
        u8HalResult = DRIVER_PARAMETER_ERROR;
    
    return u8HalResult;
}

uint8_t HAL_Systick_Reload_Clear(void)
{
    return HAL_Systick_Reload_Set(0u);
}

uint8_t HAL_Systick_Reload_Set( uint32_t u32Value)
{
    uint8_t u8HalResult;
    if ( VALID_TICK_VALUE(u32Value) )
    {
        SysTick->LOAD = (u32Value & SysTick_LOAD_RELOAD_Msk);  
        u8HalResult = DRIVER_TRUE;
    }
    else
        u8HalResult = DRIVER_PARAMETER_ERROR;
    
    return u8HalResult;
}

uint8_t  HAL_Systick_Reload_Get( uint32_t *pReturn)
{
    uint8_t u8HalResult;
    if (NULL != pReturn)
    {
        *pReturn = (SysTick->LOAD); 
        u8HalResult = DRIVER_TRUE;
    }
    else
        u8HalResult = DRIVER_PARAMETER_ERROR;
    
    return u8HalResult;
}

uint8_t HAL_Systick_Counter_Clear(void)
{
    SysTick->VAL = 0u;
    return DRIVER_TRUE;
}

uint8_t HAL_Systick_DelayUs(uint32_t u32Delay)
{
    Cy_SysTick_DelayInUs(u32Delay);
    return DRIVER_TRUE;
}

uint8_t HAL_Systick_Callback_Register (Systick_ISR pSystickCallback, void* pContext)
{
    uint8_t u8HalResult;
    if (NULL == pSystickCallback)
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        tsSysTick_Callback.pIsr = pSystickCallback;
        tsSysTick_Callback.pContext = pContext;
        u8HalResult = DRIVER_TRUE;
    }
    return u8HalResult;
}

uint8_t HAL_Systick_Flag_Get(uint32_t * pReturn)
{
    uint8_t u8HalResult;
    if (NULL == pReturn)
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        /*
        Returns a non-zero value if a flag is set, otherwise a zero is returned.
        */
        *pReturn = Cy_SysTick_GetCountFlag();
        u8HalResult = DRIVER_TRUE;
    }
    return u8HalResult;
}

uint8_t HAL_Systick_Init_1msISR(void)
{
    /*uint8_t u8HalResult;*/
             
    __ramVectors[CY_SYSTICK_IRQ_NUM] = &HAL_SysTick_ISR;
    NVIC_SetPriority(SysTick_IRQn, 3u);
    Cy_SysTick_SetClockSource((cy_en_systick_clock_source_t)SYSTICK_CLOCK_SOURCE_CLK_CPU);
 
    Cy_SysTick_SetReload(80000u);
             
    SysTick->VAL = 80000u;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
         
    Cy_SysTick_EnableInterrupt();
    
   
    return DRIVER_TRUE;
}

#if 0
uint8_t HAL_Systick_Flag_Clear(void)
{
    SysTick->CTRL = 0x0000u;
    return DRIVER_TRUE;
}
#endif

/*** End of file ***/

  