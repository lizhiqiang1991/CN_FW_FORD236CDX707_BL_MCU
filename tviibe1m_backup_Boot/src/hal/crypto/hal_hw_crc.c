#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_crypto.h"
#include "hal_crc_def.h"
#include "hal_crc.h"

#if (HW_CRYPTO_CRC == 1) 

#pragma message("HAL_CRYPTO : Crypto engine H/W ")

#include "crypto/cy_crypto_config.h"

/*******************************************************************************
* CRC32
********************************************************************************/
static const uint32_t CRC32_polynomial     = 0x04c11db7uL;
static const uint32_t CRC32_dataReverse    = 1uL;
static const uint32_t CRC32_dataXor        = 0uL;
static const uint32_t CRC32_lfsrInitState  = 0xffffffffuL;
static const uint32_t CRC32_remReverse     = 1uL;
static const uint32_t CRC32_remXor         = 0xffffffffuL;

/*******************************************************************************
* CRC16CCITT
********************************************************************************/
static const uint32_t CRC16CCITT_polynomial     = 0x10210000;
static const uint32_t CRC16CCITT_dataReverse    = 0;
static const uint32_t CRC16CCITT_dataXor        = 0x00;
static const uint32_t CRC16CCITT_lfsrInitState  = 0xffff0000;
static const uint32_t CRC16CCITT_remReverse     = 0;
static const uint32_t CRC16CCITT_remXor         = 0x00000000;

/*******************************************************************************
* CRC8-CCITT
********************************************************************************/
static const uint32_t CRC8CCITT_polynomial     = 0x8d000000;
static const uint32_t CRC8CCITT_dataReverse    = 0;
static const uint32_t CRC8CCITT_dataXor        = 0x00;
static const uint32_t CRC8CCITT_lfsrInitState  = 0xff000000;
static const uint32_t CRC8CCITT_remReverse     = 0;
static const uint32_t CRC8CCITT_remXor         = 0x00000000;

 
/* For CRYPTO client that can run on the both CPUs */
static cy_stc_crypto_context_t        tdCryptoCtx;
static bool bInitCrypto = false;
uint8_t HAL_CRC_Init(crc_config_t* tsConfig)
{
    uint8_t u8HalResult = DRIVER_FALSE;  
   
    if (NULL != tsConfig)
    {
        cy_en_crypto_status_t eStatus;
        for (uint32_t i = 0 ; i < tsConfig->u32TimeoutMS ; i++)
        {
            eStatus = Cy_Crypto_Init(&cryptoConfig, &tdCryptoCtx);
            if (CY_CRYPTO_SUCCESS == eStatus)
            {
                u8HalResult = DRIVER_TRUE;
                break;
            }
            else
                Cy_SysLib_Delay(1);
        }  
        
        if (DRIVER_TRUE == u8HalResult) 
        {
            bInitCrypto = true;
            Cy_Crypto_Enable(&tdCryptoCtx);
        }
        else
           u8HalResult =  DRIVER_FALSE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
 
    return u8HalResult;
}

#define CY_CRC_RUN(POLY,DATA_REVER,DATA_XOR,REM_REVER,REM_XOR,LFSR_INIT_STATE) \
 cy_stc_crypto_context_crc_t tdCryptoCrcContext;\
/* At first initialize CRC with CRC parameteres */\
    Cy_Crypto_Crc_Init\
    (\
        &tdCryptoCtx,\
        &tdCryptoCrcContext,\
        (POLY),\
        (DATA_REVER),\
        (DATA_XOR),\
        (REM_REVER),\
        (REM_XOR)\
    );\
    /* Wait for completion */\
    Cy_Crypto_Sync(true, &tdCryptoCtx);\
    \
     /* Then calculate CRC */\
    Cy_Crypto_Crc_Run\
    (\
        &tdCryptoCtx,\
        &tdCryptoCrcContext,\
        (void const *)pu8Data,\
        u32DataSize,\
        (uint32_t *)u8CrcResult,\
        (LFSR_INIT_STATE)\
    );\
    /* Wait for completion */\
    Cy_Crypto_Sync(true, &tdCryptoCtx)\

/*******************************************************************************
* CRC8CCITT
********************************************************************************/
uint8_t HAL_CRC8_RUN (uint8_t *pu8Data, uint32_t u32DataSize, uint8_t *u8CrcResult)
{
    uint8_t u8HalResult;
    if (true == bInitCrypto)
    {
        CY_CRC_RUN(CRC8CCITT_polynomial,
                   CRC8CCITT_dataReverse,
                   CRC8CCITT_dataXor,
                   CRC8CCITT_remReverse,
                   CRC8CCITT_remXor,
                   CRC8CCITT_lfsrInitState);
        u8HalResult = DRIVER_TRUE;
     }
     else
     {
         u8HalResult = DRIVER_FALSE;
     }
    
    return u8HalResult;
}

/*******************************************************************************
* CRC16CCITT
********************************************************************************/
uint8_t HAL_CRC16_RUN(uint8_t *pu8Data, uint32_t u32DataSize, uint8_t *u8CrcResult)
{
    uint8_t u8HalResult;
    if (true == bInitCrypto)
    {
        CY_CRC_RUN(CRC16CCITT_polynomial,
                   CRC16CCITT_dataReverse,
                   CRC16CCITT_dataXor,
                   CRC16CCITT_remReverse,
                   CRC16CCITT_remXor,
                   CRC16CCITT_lfsrInitState);  
        u8HalResult = DRIVER_TRUE;
     }
     else
     {
         u8HalResult = DRIVER_FALSE;
     }
    
    return u8HalResult;
}

 
uint8_t HAL_CRC32_RUN (uint8_t *pu8Data, uint32_t u32DataSize, uint8_t *u8CrcResult)
{
    uint8_t u8HalResult;
    if (true == bInitCrypto)
    {
        CY_CRC_RUN(CRC32_polynomial,
                   CRC32_dataReverse,
                   CRC32_dataXor,
                   CRC32_remReverse,
                   CRC32_remXor,CRC32_lfsrInitState);
        u8HalResult = DRIVER_TRUE;
     }
     else
     {
         u8HalResult = DRIVER_FALSE;
     }
    
    return u8HalResult;
}

#endif  /* HW_CRYPTO_CRC == 1 */

