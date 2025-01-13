/***************************************************************************
*\file hal_sp.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_sp_def.h"
#include "hal_sp.h"

#define HAL_STACK_OVERFLOW       (0xffU)
#define HAL_ONE_HUNDRED_PERCENT  (100U)

static uint32_t MSP_Get(void)
{
  register uint32_t u32Result;
  __ASM volatile ("MRS %0, msp" : "=r" (u32Result) );
  return(u32Result);
}
 
uint8_t HAL_SP_Init(sp_config_t *tsConfig)
{
     uint8_t u8HalResult;
    if (NULL != tsConfig)
    {
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

uint8_t HAL_SP_Overflow_Check(sp_config_t *tsSp)
{
    uint8_t u8HalResult;
    if (NULL != tsSp)
    {
        uint8_t u8Percent;
        HAL_SP_Usage_Get(tsSp,&u8Percent);
        if (HAL_STACK_OVERFLOW == u8Percent)
        {
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

uint8_t HAL_SP_Usage_Get(sp_config_t *tsSp,uint8_t* pReturn)
{
    uint8_t u8HalResult;
    uint8_t u8Percent;
    
    if ((NULL != tsSp) || (NULL != pReturn))
    {
        uint32_t u32CurSp      = MSP_Get(); /* uint32_t __get_MSP(void) Return current value of the Main Stack Pointer (MSP)*/
        uint32_t u32StackSize  = (uint32_t) ( ((uint32_t)(tsSp->pvStackEnd)) - ((uint32_t)(tsSp->pvStackStart)) ); //(tsSp->pvMinStackSize);
        uint32_t u32StackEnd   = (uint32_t) (tsSp->pvStackEnd);
        uint32_t u32UseBytes = u32StackEnd - u32CurSp;
        
        if (u32UseBytes >= u32StackSize)
            u8Percent = HAL_STACK_OVERFLOW;
        else
        {
            u8Percent = (uint8_t)((float) ( (float)u32UseBytes/(float)u32StackSize ) * HAL_ONE_HUNDRED_PERCENT);
        }
        *pReturn = u8Percent;
        
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

/*** End of file ***/

  