/***************************************************************************
*\file hal_jump.c
****************************************************************************/ 

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_jump.h"

static void Cy_SI_UserApp(uint32_t stackPtr, uint32_t rstAddr)
{
    __asm volatile("MSR msp, R0\n"
                   "BX  R1     \n" );
}

uint8_t HAL_Jump_to_Address(uint32_t u32Address)
{
    uint8_t u8HalResult;
    if (Cy_Flash_MainBoundsCheck(u32Address) == CY_FLASH_IN_BOUNDS)
    {
        uint32_t *pMSP = (uint32_t *)u32Address;
        uint32_t *pResetVector = (uint32_t *)u32Address;
         
        /* Start by disabling interrupts, before changing interrupt vectors*/
        __disable_irq();
          
        /*Set vector table offset*/
        SCB->VTOR = u32Address;
        
        /*Set address for RESET*/
        pResetVector++;
        Cy_SI_UserApp( *pMSP, *pResetVector);
       
        /*Won't reach here */
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}


