/******************************************************************************
;       Program  : FunApp.c
;       Function : Implement Project Scenario Functions 
;       Chip     : Cypress CYT2B6
;       Clock    :
;       Date     : 2021 / 10 / 08
;       Author   : Joel Wang
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "InitApp.h"
uint16_t uCHK_UPDATE_KEY;
/*---------------------------- Declare External Var --------------------------*/
uint8_t u8BootSWVersion[BOOTSWVersion_LEN] = {"T-04.00.00"};
static uint32_t u32App1Address = APP1_START_ADDR;
static uint32_t u32App2Address = APP2_START_ADDR;
//__no_init volatile uint16_t u16UpdateKey @0x08001500;
/*---------------------------- Start Program ---------------------------------*/
/******************************************************************************
;	Function Name			:	static uint8_t FunApp_Jump_to_App(uint8_t u8Para)
;	Function Description	:	Jump to application mode from bootloader mode.
;	Parameters				:	[u8Para] - NOTHING
;	Return Values			:	NOTHING
;	Source ID				:
******************************************************************************/
static uint8_t FunApp_Jump_to_App(uint8_t u8Para)
{
#if BACK_UP
    if (FlashRwApp_App_Area_Get() == 1U)
    {
        HAL_Jump_to_Address(u32App1Address);
    }
    else if (FlashRwApp_App_Area_Get() == 2U)
    {
        HAL_Jump_to_Address(u32App2Address);
    }
    else
    {
        ;
    }
#else
    HAL_Jump_to_Address(u32App1Address);
#endif

    (void)(u8Para);
    return u8Para;
}

/******************************************************************************
;	Function Name			:	static uint8_t FunApp_App_Crc_Check(uint8_t u8Para)
;	Function Description	:	App memory CRC32 check.
;	Parameters				:	[u8Para] - NOTHING
;	Return Values			:	NOTHING
;	Source ID				:
******************************************************************************/
static uint8_t FunApp_App_Crc_Check(uint8_t u8Para)
{
    if (I2c0SlaveApp_Crc32_Check(TIMES_APP_MEM) == true) /* Check APP CRC */
    {
#if DEBUG_UART
        DEBUG_PF("Crc PASS\r\n");
#endif

        (void)FunApp_Jump_to_App(NOTHING); /* Jump to APP */
    }
    else
    {
#if DEBUG_UART
        DEBUG_PF("Crc ERROR\r\n");
#endif

        HAL_Jump_to_Address(u32App2Address);
    }
    (void)(u8Para);
    return u8Para;
}

/******************************************************************************
;	Function Name			:	uint8_t FunApp_Startup_Check(uint8_t u8Para)
;	Function Description	:	Check jump to App mode or bootloader mode.
;	Parameters				:	[u8Para] - NOTHING
;	Return Values			:	NOTHING
;	Source ID				:
******************************************************************************/
uint8_t FunApp_Startup_Check(uint8_t u8Para)
{
    FlashRwApp_BootVersion_Check(u8BootSWVersion);
#if BACK_UP
	DEBUG_PF("Jump to back\r\n");
    FlashRwApp_App_Infor_Read();

   if ((u16UpdateKey == (uint16_t)(CHK_UPDATE_KEY)) && ((FlashRwApp_App_Area_Get() == 1U) || (FlashRwApp_App_Area_Get() == 2U)))
   // if ((u16UpdateKey <=0xffff)&&(u16UpdateKey >=0xf000) && ((FlashRwApp_App_Area_Get() == 1U) || (FlashRwApp_App_Area_Get() == 2U)))
    {
        u16UpdateKey = (uint16_t)CLR_KEYS;
        (void)InitApp_StateMachine_Set(BL_INIT);
#if DEBUG_UART
        DEBUG_PF("Jump to update process\n");
#endif
    }
    else if ((u16UpdateKey != (uint16_t)(CHK_UPDATE_KEY)) && ((FlashRwApp_App_Area_Get() == 1U) || (FlashRwApp_App_Area_Get() == 2U)))
    {
#if DEBUG_UART
        DEBUG_PF("Jump to app!\n");
        Cy_SysLib_Delay(10);
#endif
        (void)FunApp_Jump_to_App(NOTHING); /* Jump to APP */
    }
    else
    {
        (void)InitApp_StateMachine_Set(BL_INIT); /* Error, Stay in bootloader to update */
    }
#else
    if (u16UpdateKey == (uint16_t)(CHK_UPDATE_KEY) && (CY_SYSRESET_SOFT== Cy_SysReset_GetResetReason())/* && uCHK_UPDATE_KEY!= (uint16_t)(CHK_UPDATE_KEY)*/)
    //if(u16UpdateKey >=0xf000 && CY_SYSRESET_SOFT== Cy_SysReset_GetResetReason())  //startup_customize.h  line 36 to be annotated
    {
        u16UpdateKey = (uint16_t)CLR_KEYS;
        (void)InitApp_StateMachine_Set(BL_INIT);
#if DEBUG_UART
        DEBUG_PF("Jump to update process\n");
#endif
    }
    else
    {
        //uCHK_UPDATE_KEY=u16UpdateKey;
         DEBUG_PF("1:u16UpdateKey=%x\r\n", *(volatile unsigned int *)0x08001500);
        //*(volatile unsigned int *)0x08001500=uCHK_UPDATE_KEY+1;
       // DEBUG_PF("2:u16UpdateKey=%x\r\n", *(volatile unsigned int *)0x08001500);
        // Cy_SysLib_Delay(3);
       //  NVIC_SystemReset();
        (void)FunApp_App_Crc_Check(NOTHING);
       
    }
#endif

    (void)(u8Para);
    return u8Para;
}

/******************************************************************************
;       Function Name			:	uint32_t FunApp_App_Update_Address_Get(void)
;       Function Description	:	
;       Parameters				:	
;       Return Values			:	
;		Source ID				:
******************************************************************************/
uint32_t FunApp_App_Update_Address_Get(void)
{
    if (FlashRwApp_App_Area_Get() == 1U)
    {
        return u32App2Address; /* Config is APP1, so update APP2 */
    }
    else if (FlashRwApp_App_Area_Get() == 2U)
    {
        return u32App1Address;
    }
    else
    {
        return u32App1Address; /* TBD */
    }
}

/*----------------------------------------------------------------------------*/
