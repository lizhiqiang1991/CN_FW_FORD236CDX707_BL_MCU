#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_crypto.h"
#include "hal_crc_def.h"
#include "hal_crc.h"

#if (HW_CRYPTO_CRC == 0) 

#pragma message("HAL_CRYPTO : Crypto engine SOFTWARE ")


uint8_t HAL_CRC_Init(crc_config_t* tsConfig)
{
    UNUSED(tsConfig);
    return DRIVER_FALSE;
}

uint8_t HAL_CRC8_RUN (uint8_t *pu8Data, uint32_t u32DataSize, uint8_t *u8CrcResult)
{
    UNUSED(pu8Data);
    UNUSED(u32DataSize);
    UNUSED(u8CrcResult);
    
    return DRIVER_FALSE;
}

uint8_t HAL_CRC16_RUN (uint8_t *pu8Data, uint32_t u32DataSize, uint8_t *u8CrcResult)
{
    UNUSED(pu8Data);
    UNUSED(u32DataSize);
    UNUSED(u8CrcResult);
    return DRIVER_FALSE;
}

uint8_t HAL_CRC32_RUN (uint8_t *pu8Data, uint32_t u32DataSize, uint8_t *u8CrcResult)
{
    UNUSED(pu8Data);
    UNUSED(u32DataSize);
    UNUSED(u8CrcResult);
    return DRIVER_FALSE;
}

#endif /* HW_CRYPTO_CRC == 0 */

