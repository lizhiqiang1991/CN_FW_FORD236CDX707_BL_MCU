/***************************************************************************
*\file hal_wdt_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_wdt_def.h"

wdt_config_t gts_WDT_Config =
{
    .u32Timeout     = HAL_WDT_TIMEOUT,
    .u8UpperLimit   = 40000ul,
    .u8LowerLimit   = 0ul,
    .u8WarnLimit    = 0ul,
    .u8UpperAction  = WDT_UPP_ACTION_RESET,
    .u8LowerAction  = WDT_LOW_ACTION_NONE,
    .u8WarnAction   = WDT_WARN_ACTION_NONE,
    .u8InterruptEnable = FUNC_DISABLE,
    .u8NvicPriority    = 3u,
};

/*** End of file ***/

  