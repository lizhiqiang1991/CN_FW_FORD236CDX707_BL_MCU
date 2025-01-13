/***************************************************************************
*\file hal_sysreset.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_sysreset_def.h"
#include "hal_sysreset.h"

uint8_t HAL_RES_CAUSE_GET(uint32_t *pu32RCause)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    if (pu32RCause != NULL)
    {
        *pu32RCause = (SRSS->unRES_CAUSE.u32Register);
		Cy_SysReset_ClearAllResetReasons();
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

/*** End of file ***/

  