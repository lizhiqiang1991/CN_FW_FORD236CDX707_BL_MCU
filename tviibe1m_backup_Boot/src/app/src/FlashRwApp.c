/******************************************************************************
;       Program		: FlashRwApp.c
;       Function	: Declear Flash R/W Function
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
;       Describe	: Use Microchip NVMCTRL (Non-Volatile Memory Controller)
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include "InitApp.h"
/*---------------------------- Declare External Var -------------------------*/
static uint8_t u8AppAreaBuff = 1U;
static uint8_t u8UpdateRequest = 0U;
static uint8_t u8Area_A = 1U;
static uint8_t u8Area_B = 2U;
//static uint8_t u8BootVersionBuff[8];
/*---------------------------- Start Program --------------------------------*/
/******************************************************************************
;       Function Name			:	uint8_t FlashRwApp_Initial(void)
;       Function Description	:	Initialize FMC driver setting.
;       Parameters				:	
;       Return Values			:	Initial result
;		Source ID				:	
******************************************************************************/
uint8_t FlashRwApp_Initial(void)
{
    return HAL_FMC_Initial(&gtdFMC_Config);
}

/******************************************************************************
;       Function Name			:	uint8_t FlashRwApp_128B_Page_Compare(uint32_t u32StartAddr, uint8_t *pu8SrcBuff)
;       Function Description	:
;       Parameters				:
;       Return Values			:
;		Source ID				:
******************************************************************************/
uint8_t FlashRwApp_128B_Page_Compare(uint32_t u32StartAddr, uint8_t *pu8SrcBuff)
{
    uint8_t u8Status;
    u8Status = (uint8_t)(STATUS_FINISH);

    uint8_t u8TempData[SIZE_128B_PAGE];
    if (HAL_FMC_FlashRead(u8TempData, u32StartAddr, SIZE_128B_PAGE) == DRIVER_TRUE)
    {
        for (uint32_t i = 0; i < SIZE_128B_PAGE; i++)
        {
            if (u8TempData[i] != pu8SrcBuff[i])
            {
                u8Status = (uint8_t)(STATUS_FAIL); /* Data are not match */
                break;
            }
        }
    }
    (void)(u32StartAddr);
    (void)(*pu8SrcBuff);
    return u8Status;
}

/******************************************************************************
;       Function Name			:
;       Function Description	:
;       Parameters				:
;       Return Values			:
;		Source ID				:
******************************************************************************/
uint8_t FlashRwApp_128B_Page_Write(uint32_t u32StartAddr, uint8_t *pu8DataBuff)
{
    uint8_t u8Status;

    if (HAL_FMC_FlashWrite(pu8DataBuff, u32StartAddr, SIZE_128B_PAGE) == DRIVER_TRUE)
    {
        u8Status = FlashRwApp_128B_Page_Compare(u32StartAddr, pu8DataBuff);
    }
    else
    {
        u8Status = (uint8_t)(STATUS_FAIL);
    }

    return u8Status;
}

/******************************************************************************
;       Function Name			:	uint8_t FlashRwApp_Read(uint32_t u32DestAddr, uint32_t u32ReadByteLen, uint8_t *pu8DataBuff)
;       Function Description	:	Read flash memory, , the maximum range are 256 bytes.
;       Parameters				:	[u32DestAddr] - Set destination address.
;									[u32ReadByteLen] - Set read's length.
;									[*pu8DataBuff] - Input data.
;       Return Values			:	Return status.
;									(1) NVC_FINISH
;									(2) NVC_FAIL
;		Source ID				:
******************************************************************************/
uint8_t FlashRwApp_Read(uint32_t u32DestAddr, uint32_t u32ReadByteLen, uint8_t *pu8DataBuff)
{
    uint8_t u8Status;

    if (HAL_FMC_FlashRead(pu8DataBuff, u32DestAddr, u32ReadByteLen) == DRIVER_TRUE)
    {
        u8Status = NVC_FINISH;
    }
    else
    {
        u8Status = NVC_FAIL;
    }

    return u8Status;
}

/******************************************************************************
;       Function Name			:	uint8_t FlashRwApp_256B_Erase(uint32_t u32StartAddr, uint32_t u32PageShift)
;       Function Description	:	Erase 256 bytes flash memory.
;       Parameters				:	[u32StartAddr] - Set start address.
;									[u32PageShift] - Set erase page.
;       Return Values			:	Return status.
;									(1) NVC_FINISH
;									(2) NVC_FAIL
;		Source ID				:
******************************************************************************/
uint8_t FlashRwApp_Sector_Erase(uint32_t u32StartAddr)
{
    uint8_t u8Status;

    if (HAL_FMC_FlashEraseSector(u32StartAddr, FUNC_ENABLE) == DRIVER_TRUE)
    {
        u8Status = (uint8_t)(NVC_FINISH);
    }
    else
    {
        u8Status = (uint8_t)(NVC_FAIL);
    }

    return u8Status;
}

/******************************************************************************
;       Function Name			:	void FlashRwApp_App_Infor_Read(void)
;       Function Description	:	
;       Parameters				:	
;       Return Values			:	
;		Source ID				:
******************************************************************************/
void FlashRwApp_App_Infor_Read(void)
{
    HAL_FMC_EEPROM_Safety(FUNC_ENABLE);
    HAL_FMC_EEPROMRead((uint8_t *)&u8AppAreaBuff, (uint32_t)APP_AREA_Config_Addr, sizeof(u8AppAreaBuff));
    //    HAL_FMC_EEPROMRead ((uint8_t*)&u8UpdateRequest,(uint32_t)UPDATE_REQUEST_Addr,sizeof(u8UpdateRequest));
    HAL_FMC_EEPROM_Safety(FUNC_DISABLE);

#if DEBUG_UART
    if (u8AppAreaBuff == 1U)
    {
        DEBUG_PF("Application memory is APP1 ,start in (0x%x)\n", APP1_START_ADDR);
    }
    else if (u8AppAreaBuff == 2U)
    {
        DEBUG_PF("Application memory is APP2 ,start in (0x%x)\n", APP2_START_ADDR);
    }
    else
    {
        DEBUG_PF("Application area config is error!\n");
    }
#endif
}

/******************************************************************************
;       Function Name			:	uint8_t FlashRwApp_App_Area_Get(void)
;       Function Description	:	
;       Parameters				:	
;       Return Values			:	
;		Source ID				:
******************************************************************************/
uint8_t FlashRwApp_App_Area_Get(void)
{
    return u8AppAreaBuff;
}

/******************************************************************************
;       Function Name			:	uint8_t FlashRwApp_Whether_Update_Get(void)
;       Function Description	:	
;       Parameters				:	
;       Return Values			:	
;		Source ID				:
******************************************************************************/
uint8_t FlashRwApp_Whether_Update_Get(void)
{
    return u8UpdateRequest;
}

/******************************************************************************
;       Function Name			:	void FlashRwApp_Clear_Update_Key(void)
;       Function Description	:
;       Parameters				:
;       Return Values			:
;		Source ID				:
******************************************************************************/
void FlashRwApp_Clear_Update_Key(void)
{
    HAL_FMC_EEPROMEraseSector((uint32_t)UPDATE_REQUEST_Addr, FUNC_ENABLE);
}
/******************************************************************************
;       Function Name			:	void FlashRwApp_Change_App_Update_Area(void)
;       Function Description	:
;       Parameters				:
;       Return Values			:
;		Source ID				:
******************************************************************************/
void FlashRwApp_Change_App_Update_Area(void)
{
    if (FlashRwApp_App_Area_Get() == 1U)
    {
        HAL_FMC_EEPROMEraseSector((uint32_t)APP_AREA_Config_Addr, FUNC_ENABLE);
        HAL_FMC_EEPROMWrite((uint8_t *)&u8Area_B, (uint32_t)APP_AREA_Config_Addr, sizeof(u8Area_B));
    }
    else if (FlashRwApp_App_Area_Get() == 2U)
    {
        HAL_FMC_EEPROMEraseSector((uint32_t)APP_AREA_Config_Addr, FUNC_ENABLE);
        HAL_FMC_EEPROMWrite((uint8_t *)&u8Area_A, (uint32_t)APP_AREA_Config_Addr, sizeof(u8Area_A));
    }
    else
    {
        ;
    }
}
/******************************************************************************
;       Function Name			:	void FlashRwApp_Version_Read(void)
;       Function Description	:	
;       Parameters				:	
;       Return Values			:	
;		Source ID				:
******************************************************************************/
void FlashRwApp_BootVersion_Check(uint8_t *pu8BLVerBuff)
{
    uint8_t u8BootVersionBuff[10];
    uint8_t u8FlashBuff[128];
    uint8_t i = 0;
    bool bVerCheck = true;
    HAL_FMC_EEPROM_Safety(FUNC_ENABLE);
    HAL_FMC_EEPROMRead((uint8_t *)&u8BootVersionBuff, (uint32_t)BL_Version_Addr, sizeof(u8BootVersionBuff));
    HAL_FMC_EEPROM_Safety(FUNC_DISABLE);
    #if DEBUG_UART
    DEBUG_PF("EE Ver : %s\r\n",u8BootVersionBuff);
    #endif
    for (i = 0; i < sizeof(u8BootVersionBuff); i++)
    {
        if (pu8BLVerBuff[i] != u8BootVersionBuff[i])
        {
            bVerCheck = false;
            #if DEBUG_UART
            DEBUG_PF("Ver error\r\n");
            #endif
            break;
        }
    }

    if (!bVerCheck)
    { 
        #if DEBUG_UART
        DEBUG_PF("Write Ver\r\n");
        #endif
        (void)memset(u8FlashBuff, 0xFFU, sizeof(u8FlashBuff));
        memcpy(u8FlashBuff,u8BootVersionBuff,10);
        HAL_FMC_EEPROMEraseSector((uint32_t)BL_Version_Addr, FUNC_ENABLE);
        HAL_FMC_EEPROMWrite((uint8_t *)pu8BLVerBuff, (uint32_t)BL_Version_Addr, sizeof(u8BootVersionBuff));
    }
}
/*---------------------------------------------------------------------------*/
