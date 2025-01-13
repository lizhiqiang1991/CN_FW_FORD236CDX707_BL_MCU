/***************************************************************************
*\file hal_timer_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_timer_def.h"

/*Timer Configuration*/
timer_config_t gts_1mSec_Timer_Continuous_Config = 
{
    .u32Period = 1000u,                     /* 1~ 65535 uSec*/
    .u8RunType = TIMER_CONTINUOUS,          /* Continuous 、One Shot*/
    .u8PeriodUnit = TIMER_PERIOD_MICRO_SEC, /* uSec*/
    .u8InterruptEnable = FUNC_ENABLE,
    
};

timer_config_t gts_1mSec_Timer_OneShot_Config = 
{
    .u32Period = 1000u,                     /*1~ 65535 uSec*/
    .u8RunType = TIMER_ONESHOT,             /* One Shot*/
    .u8PeriodUnit = TIMER_PERIOD_MICRO_SEC, /* uSec*/
    .u8InterruptEnable = FUNC_ENABLE,   
};

timer_config_t gts_1Sec_Timer_Config = 
{
    .u32Period = 1000u,                     /* 1000 mSec = 1 Sec*/
    .u8RunType = TIMER_CONTINUOUS,          /* Continuous*/
    .u8PeriodUnit = TIMER_PERIOD_MILLI_SEC, /* mSec*/
    .u8InterruptEnable = FUNC_ENABLE,   
};

/*** End of file ***/

  