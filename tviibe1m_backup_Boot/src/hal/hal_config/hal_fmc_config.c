/***************************************************************************
*\file hal_fmc_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_fmc_def.h"

/*FMC configutaion*/
fmc_config_t gtdFMC_Config =
{
    .u8ReadWaitStates = 1u,
};

 