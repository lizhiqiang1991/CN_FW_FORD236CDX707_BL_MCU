/***************************************************************************
*\file hal_crypto_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_crc_def.h"
 
#define CRC_INIT_TIMEOUT   (100U) 

crc_config_t gtdCRC_Config =
{
    .u32TimeoutMS = CRC_INIT_TIMEOUT,
};


/*** End of file ***/

  