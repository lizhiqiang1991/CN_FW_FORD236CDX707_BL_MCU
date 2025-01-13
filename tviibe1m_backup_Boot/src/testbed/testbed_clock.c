/*

File    : testbed_clock.c
Purpose : CLOCK testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"

#define USER_LED IO_P19_0
uint8_t Clock_Test_Case0(void)
{
    uint8_t u8HalResult;
    u8HalResult = HAL_Clock_Init(&gtsClockConfig);
    if(DRIVER_TRUE == u8HalResult && (PLL_TARGET_FREQ >> 1)== cy_delayFreqHz )
    {
        u8HalResult = HAL_Clock_Init(&gtsClockConfig);
        if(DRIVER_TRUE == u8HalResult && (PLL_TARGET_FREQ >> 1)== cy_delayFreqHz )
        {
            ;
        }
        else
        {
            u8HalResult = DRIVER_FALSE;
        }
    }
    else
    {
        u8HalResult = DRIVER_FALSE;
    }
    return u8HalResult;
}

uint8_t Clock_Test_Case1(void)
{
    uint8_t u8HalResult;
    uint32_t u32Freq = 100000000;
    u8HalResult = HAL_Clock_Init(&gtsClockConfig);
    if(DRIVER_TRUE == u8HalResult && (PLL_TARGET_FREQ >> 1)== cy_delayFreqHz )
    {
        gtsClockConfig.tsPllConfig.u32OutClkFreq = u32Freq;
        u8HalResult = HAL_Clock_Init(&gtsClockConfig);
        if(DRIVER_TRUE == u8HalResult && (u32Freq >> 1)== cy_delayFreqHz )
        {
            ;
        }
        else
        {
            u8HalResult = DRIVER_FALSE;
        }
    }
    else
    {
        u8HalResult = DRIVER_FALSE;
    }
    return u8HalResult;
}

uint8_t Clock_Test_Case2(void)
{
    uint8_t u8HalResult;

    gtsClockConfig.tsPllConfig.u32OutClkFreq = PLL_TARGET_FREQ;
    u8HalResult = HAL_Clock_Init(&gtsClockConfig);
    if(DRIVER_TRUE == u8HalResult && (PLL_TARGET_FREQ >> 1)== cy_delayFreqHz )
    {
        gtsClockConfig.tsPllConfig.u8Enable = FUNC_DISABLE;
        u8HalResult = HAL_Clock_Init(&gtsClockConfig);
        if(DRIVER_TRUE == u8HalResult && (CLK_FREQ_IMO >> 1)== cy_delayFreqHz )
        {
            ;
        }
        else
        {
            u8HalResult = DRIVER_FALSE;
        }
    }
    else
    {
        u8HalResult = DRIVER_FALSE;
    }
    return u8HalResult;
}

uint8_t Clock_Test_Case3(void)
{
    uint8_t u8HalResult;

    gtsClockConfig.tsPllConfig.u8Enable = FUNC_DISABLE;
    gtsClockConfig.tsFllConfig.u8ToClockHF = CLOCK_HF0;
    gtsClockConfig.tsFllConfig.u8Enable = FUNC_ENABLE;
    
    u8HalResult = HAL_Clock_Init(&gtsClockConfig);
    if(DRIVER_TRUE == u8HalResult && (FLL_TARGET_FREQ >> 1)== cy_delayFreqHz )
    {
        gtsClockConfig.tsFllConfig.u8Enable = FUNC_DISABLE;
        u8HalResult = HAL_Clock_Init(&gtsClockConfig);
        if(DRIVER_TRUE == u8HalResult && (CLK_FREQ_IMO >> 1)== cy_delayFreqHz )
        {
            ;
        }
        else
        {
            u8HalResult = DRIVER_FALSE;
        }
    }
    else
    {
        u8HalResult = DRIVER_FALSE;
    }
    return u8HalResult;
}

void testbed_clock(void) 
{
    uint8_t u8ErrCounter = 0;
    uint8_t u8Case = 0;
    uint8_t u8HalResult;

    for (;;)
    {
        switch(u8Case++)
        {
            case 0:
                u8HalResult = Clock_Test_Case0();
                if(DRIVER_TRUE == u8HalResult)
                {
                    ;
                }
                else
                {
                    u8ErrCounter++;
                }
                break;
            case 1:
                u8HalResult = Clock_Test_Case1();
                if(DRIVER_TRUE == u8HalResult)
                {
                    ;
                }
                else
                {
                    u8ErrCounter++;
                }
                break;
            case 2:
                u8HalResult = Clock_Test_Case2();
                if(DRIVER_TRUE == u8HalResult)
                {
                    ;
                }
                else
                {
                    u8ErrCounter++;
                }
                break;
            case 3:
                u8HalResult = Clock_Test_Case3();
                if(DRIVER_TRUE == u8HalResult)
                {
                    ;
                }
                else
                {
                    u8ErrCounter++;
                }
                break;
            default:
                HAL_GPIO_Pin_Init(USER_LED,&gtdPinOutputConfig);
                HAL_GPIO_PinWrite(USER_LED,PIN_LOW);
                for (;;)
                {
                    if(0 != u8ErrCounter)
                    {
                        HAL_GPIO_PinToggle(USER_LED);
                    }
                    else
                    {
                        ;
                    }
                    DELAY(1600000U);
                }
                break;
        }
    }
}
