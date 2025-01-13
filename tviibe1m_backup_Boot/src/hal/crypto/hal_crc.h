#ifndef HAL_CRYPTO_CRC_H
#define HAL_CRYPTO_CRC_H

uint8_t HAL_CRC_Init  (crc_config_t* tsConfig);
uint8_t HAL_CRC8_RUN  (uint8_t *pu8Data, uint32_t u32DataSize, uint8_t *u8CrcResult);
uint8_t HAL_CRC16_RUN (uint8_t *pu8Data, uint32_t u32DataSize, uint8_t *u8CrcResult);
uint8_t HAL_CRC32_RUN (uint8_t *pu8Data, uint32_t u32DataSize, uint8_t *u8CrcResult);

#endif

