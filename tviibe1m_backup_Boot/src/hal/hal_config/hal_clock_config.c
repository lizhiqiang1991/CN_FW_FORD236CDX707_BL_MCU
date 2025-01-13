/***************************************************************************
*\file hal_clock_config.c
****************************************************************************/ 

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_clock_def.h"

/* 
        CYT2B6 Clocking System  
                
                               CLK_PATH0
       ----> PATH_MUX0 --> FLL ------------      --> ROOT_MUX0 --> CLK_HF0 (cm0/cm4/peri/flash ...)
  IMO  |                                  |      |
  ECO  |                                  |====> |
  EXT  |                       CLK_PATH1  |      | 
       ----> PATH_MUX1 --> PLL ------------      --> ROOT_MUX1 --> CLK_HF1 (event generator)

*/

/** example **/
#if 0
clock_config_t gtsClockConfig =
{
    .tsPllConfig.u8InputClockPath = SYSCLK_CLKPATH_IN_ECO,  /* Input Clock from ECO */
    .tsPllConfig.u32InClkFreq = CLK_FREQ_ECO,               /* ECO: 16MHz*/
    .tsPllConfig.u32OutClkFreq = PLL_TARGET_FREQ,           /* target PLL output*/
    .tsPllConfig.u8ToClockHF = CLOCK_HF1,
    .tsPllConfig.u8Enable = FUNC_DISABLE,

    .tsFllConfig.u8InputClockPath = SYSCLK_CLKPATH_IN_ECO,  /* Input Clock from ECO */
    .tsFllConfig.u32InClkFreq = CLK_FREQ_ECO,               /* ECO: 16MHz*/
    .tsFllConfig.u32OutClkFreq = PLL_TARGET_FREQ,           /* target PLL output*/
    .tsFllConfig.u8ToClockHF = CLOCK_HF1,
    .tsFllConfig.u8Enable = FUNC_DISABLE,
    
    .u32EcoFreq   = CLK_FREQ_ECO,
    .u8EcoEnable  = FUNC_ENABLE,
    .u8WcoEnable  = FUNC_DISABLE,
    .u8Ilo0Enable = FUNC_DISABLE,
    .u8Ilo1Enable = FUNC_DISABLE,
    .u32Timeout   = WAIT_FOR_STABILIZATION,
};
#endif

#if 1
clock_config_t gtsClockConfig = 
{
    .tsPllConfig.u8InputClockPath = SYSCLK_CLKPATH_IN_IMO,  /* Input Clock from IMO */
    .tsPllConfig.u32InClkFreq = CLK_FREQ_IMO,               /* IMO: 8MHz */
    .tsPllConfig.u32OutClkFreq = PLL_TARGET_FREQ,           /* target PLL output*/
    .tsPllConfig.u8ToClockHF = CLOCK_HF0,
    .tsPllConfig.u8Enable = FUNC_ENABLE,
    
    .tsFllConfig.u8InputClockPath = SYSCLK_CLKPATH_IN_IMO,  /* Input Clock from IMO */
    .tsFllConfig.u32InClkFreq = CLK_FREQ_IMO,               /* IMO: 8MHz */
    .tsFllConfig.u32OutClkFreq = FLL_TARGET_FREQ,           /* target FLL output*/
    .tsFllConfig.u8ToClockHF = CLOCK_HF1,
    .tsFllConfig.u8Enable = FUNC_DISABLE,
    
    .u32EcoFreq   = CLK_FREQ_ECO,
    .u8EcoEnable  = FUNC_DISABLE,
    .u8WcoEnable  = FUNC_DISABLE,
    .u8Ilo0Enable = FUNC_ENABLE,
    .u8Ilo1Enable = FUNC_ENABLE,
    .u32Timeout   = WAIT_FOR_STABILIZATION,
};
#endif

/*** End of file ***/

