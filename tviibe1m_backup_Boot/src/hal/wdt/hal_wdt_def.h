/***************************************************************************
*\file hal_wdt_def.h 
****************************************************************************/

#ifndef HAL_WDT_DEF_H
#define HAL_WDT_DEF_H

typedef struct
{
     uint32_t u32Timeout; /*Micro seconds*/
     uint32_t u8UpperLimit;
     uint32_t u8LowerLimit;
     uint32_t u8WarnLimit;
     uint8_t u8UpperAction;
     uint8_t u8LowerAction;
     uint8_t u8WarnAction;
     uint8_t u8InterruptEnable;
     uint8_t u8NvicPriority;
}wdt_config_t;

typedef void(*HAL_WDT_WARN_CALLBACK) (void);

#define HAL_WDT_TIMEOUT  (1000u)

/** The wdt lower limit actions. */
#define WDT_LOW_ACTION_NONE   CY_WDT_LOW_UPP_ACTION_NONE
#define WDT_LOW_ACTION_RESET  CY_WDT_LOW_UPP_ACTION_RESET

/** The wdt upper limit actions. */
#define WDT_UPP_ACTION_NONE   CY_WDT_LOW_UPP_ACTION_NONE
#define WDT_UPP_ACTION_RESET  CY_WDT_LOW_UPP_ACTION_RESET

/** The wdt warn limit actions. */
#define WDT_WARN_ACTION_NONE  CY_WDT_WARN_ACTION_NONE
#define WDT_WARN_ACTION_INT   CY_WDT_WARN_ACTION_INT

extern wdt_config_t gts_WDT_Config;

#endif

  