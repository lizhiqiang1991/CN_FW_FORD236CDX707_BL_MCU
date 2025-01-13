#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_crypto.h"

#if (HW_CRYPTO_CRC == 1) 

/* Include default crypto configuration.
 * You can use other configuration instead follows.
 */
#include "crypto/cy_crypto_config.h"

/* For CRYPTO server that runs on the CM0+ */
static cy_stc_crypto_server_context_t tdCryptoServerCtx;

uint8_t HAL_Crypto_Server_Enable(uint8_t u8Enable)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    
#if (CY_CPU_CORTEX_M0P)
    
    if ((FUNC_ENABLE == u8Enable) || (FUNC_DISABLE == u8Enable))
    {
        if (FUNC_ENABLE == u8Enable)
        {
            cy_en_crypto_status_t eStatus = CY_CRYPTO_HW_NOT_ENABLED;
            /* Start crypto server */
            eStatus = Cy_Crypto_Server_Start(&cryptoConfig, &tdCryptoServerCtx);
            
            /* Enable CRYPTO regarding IRQn */
            /* In this examle 2 of crypto regarding interrupt uses same number,
                    so just one invoking NVIC setting is needed.
                    CY_CRYPTO_NOTIFY_INTR_NR    == 
                    CY_CRYPTO_ERROR_INTR_NR
            */
            if (CY_CRYPTO_SUCCESS == eStatus)
            {
                NVIC_SetPriority(CY_CRYPTO_NOTIFY_CPU_INT_IDX, 0);
                NVIC_ClearPendingIRQ(CY_CRYPTO_NOTIFY_CPU_INT_IDX);
                NVIC_EnableIRQ(CY_CRYPTO_NOTIFY_CPU_INT_IDX);
                u8HalResult = DRIVER_TRUE;
            }
            else
            {
                u8HalResult = DRIVER_FALSE;
            }
            
        }
        else
        {
            Cy_Crypto_Server_Stop();
            u8HalResult = DRIVER_TRUE;
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
#else
    
    /*Can't run Crypto server on CM4*/
    #error Can't run Crypto server on CM4
#endif    
    
    return u8HalResult;
}

#else /*(HW_CRYPTO_CRC == 1)*/

uint8_t HAL_Crypto_Server_Enable(uint8_t u8Enable)
{
    /*If using sowfwate CRC , you don't need to use Crypto server*/
    return DRIVER_TRUE;
}
#endif

