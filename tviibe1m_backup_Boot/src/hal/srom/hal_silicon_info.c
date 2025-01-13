/***************************************************************************
*\file hal_silicon_info.c 
****************************************************************************/
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_silicon_info.h"

uint8_t HAL_SiliconID_Get(silicon_info_t* ptsInfo)
{
    uint8_t u8HalResult;
    
    un_srom_api_args_t  tsParams = {0};
    un_srom_api_resps_t tsResp   = {0};
    
    if (NULL != ptsInfo)
    {
        memset(ptsInfo, 0u , sizeof(silicon_info_t));
        
        /*Reading Sflash version*/
        volatile uint32_t* p_reg = (volatile uint32_t*) 0x170000A8u;
        ptsInfo->u32SFlash_Version = *p_reg;
        
        /* CPUSS_PRODUCT_ID : Reading Silicon ID TYPE 0 */
        p_reg = (volatile uint32_t*) 0x40201400u;
        volatile uint32_t u32cpuss_product_id = *p_reg;
        ptsInfo->u32Family           =  u32cpuss_product_id & 0x00000FFFu;
        uint16_t u16MajorRevisionId  = (u32cpuss_product_id >> 16u) & 0x000Fu; 
        uint16_t u16MinorRevisionId  = (u32cpuss_product_id >> 20u) & 0x000Fu;
        ptsInfo->u32Rev = u16MajorRevisionId << 4u | u16MinorRevisionId;
        
        /* Reading Silicon ID TYPE 1*/
        {
        memset(&tsParams, 0, sizeof(tsParams));
        tsParams.SiId.arg0.Opcode = CY_SROM_OP_SILICON_ID;
        tsParams.SiId.arg0.IdType = 1u;

        Cy_Srom_CallApi(&tsParams, &tsResp);
        
        ptsInfo->u32SiliconID = tsResp.SiId.resp0.IdType1.SiliconIdHigh;
        ptsInfo->u32SiliconID <<= 8u;
        ptsInfo->u32SiliconID |= tsResp.SiId.resp0.IdType1.SiliconIdLow;
        }
        
        /* Reading Silicon ID TYPE 2 */
        {
        memset(&tsParams, 0u, sizeof(tsParams));
        tsParams.SiId.arg0.Opcode = CY_SROM_OP_SILICON_ID;
        tsParams.SiId.arg0.IdType = 2u;

        Cy_Srom_CallApi(&tsParams, &tsResp);

        ptsInfo->u32FlashBootMajorVersion = tsResp.SiId.resp0.IdType2.FlashBootMajorVersion;
        ptsInfo->u32FlashBootMinorVersion = tsResp.SiId.resp0.IdType2.FlashBootMinorVersion;
        ptsInfo->u32SROM_FW_MajorVersion = tsResp.SiId.resp0.IdType2.SROM_FW_MajorVersion;
        ptsInfo->u32SROM_FW_MinorVersion = tsResp.SiId.resp0.IdType2.SROM_FW_MinorVersion;
        }
        
        /* Reading Unique ID (Zero was read)*/
        {
        tsParams.RdUnId.arg0.Opcode = CY_SROM_OP_READ_UNIQUE_ID;
        Cy_Srom_CallApi(&tsParams, &tsResp);
        ptsInfo->u64UUID = tsResp.RdUnId.resp1.DIE_ID0;
        ptsInfo->u64UUID <<= 32u;
        ptsInfo->u64UUID |= tsResp.RdUnId.resp2.DIE_ID1;
        }
        
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

