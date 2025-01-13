/******************************************************************************
;       Program		: I2c0SlaveApp.c
;       Function	: Declear SERCOM0 I2C Slave Application Function
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
;       Describe	: Use SERCOM0 for I2C Slave
;			  I2C_SDA_PA08 = SERCOM0/PAD[0] 
;			  I2C_SCL_PA09 = SERCOM0/PAD[1]
;			  [Function Requirements]					  
;			  1. The ECU is disabled program flash in the SM_UPDATE state when battery voltage is under 9V for SGM.
;			  2. The ECU don't into SM_UPDATE state when battery voltage is under 9V for SGM.
;			  3. CRC32 calculate address range are 0x0000 ~ 0xFFDF. (APP=64KB, deduct CustomerID, MCU VER, CRC32)
;			  4. I2C slave frequency set 1MHz.
;			  5. I2C protocol packets is LSB (Low Byte) first.
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "InitApp.h"
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include "CYTVII_B_E_1M_KIT_PIN.h"
/*---------------------------- Declare External Var -------------------------*/
static volatile st_queue tsI2cSlaveRx;

static uint8_t u8AppMemRawDataBuff[SIZE_APP_MEM_BLOCK] = {0};
static uint8_t u8AppCrc32Buff[LEN_APP_CRC32] = {0};
static uint32_t u32ComputeCrc32 = (uint32_t)(INIT_CRC32);
#if BACK_UP
static uint32_t u32AppUpdateAddress;
#endif

static uint8_t rxBuffer[I2CS_WBUFFER_SIZE] = {NUM_ZERO}; /* Declaration of the Slave LPI2C RX buffer for Master Write*/
static uint8_t txBuffer[I2CS_RBUFFER_SIZE] = {NUM_ZERO}; /* Declaration of the Slave LPI2C TX buffer for Master Read*/

static uint8_t u8retMsg[2];
static uint16_t u16CalculateChecksum = 0U;
static uint16_t u16DataChecksum = 0U;
static uint8_t u8checksum_reset = 0U;
static uint16_t u16SerialID;

static uint8_t u8BootloaderStatusFlag = (uint8_t)(DEFAULT_BL_STATUS); /* default is BL mode */
static bool gbResetFlag = false;

#if BACK_UP
extern uint8_t u8APP1BinOffsetAddr[];
extern uint8_t u8APP2BinOffsetAddr[];
#endif
/*------------------- u8BootloaderStatusFlag ----------------
Bit7 : Bootloader whether receive check update status event, 0 = not receive, 1 = event trigger.
Bit6 : Bootloader whether receive program event, 0 = not receive, 1 = event trigger.
Bit5 : Bootloader whether receive erase event, 0 = not receive, 1 = event trigger.
Bit4 : 0 = Bootloader execute update area APP1, 1 = Bootloader execute update area APP2.
Bit3 : 0 = Bootloader not finish update process, 1 = Bootloader update APP memory success.
Bit2 : 0 = Bootloader not program APP memory yet, 1 = Bootloader program APP memory and receive checksum_all is correct.
Bit1 : 0 = Bootloader not receive erase command, 1 = Bootloader erase APP region ready.
Bit0 : 0 = Bootloader not receive handshake command, 1 = Bootloader agree handshake.
------------------------------------------------------------*/

/*---------------------------- Start Program --------------------------------*/
/******************************************************************************
;       Function Name			:	uint8_t I2c0SlaveApp_BL_Status_Set(uint8_t u8Status)
;       Function Description	:	Set bootloader status 1 flags information and trigger IRQ signal.
;       Parameters				:	[u8Status] - Set bootloader flags. 
;       Return Values			:	Return bootloader status 1 information.
;		Source ID				:
******************************************************************************/
uint8_t I2c0SlaveApp_BL_Status_Set(uint8_t u8Status)
{
    u8BootloaderStatusFlag |= u8Status;

    return u8BootloaderStatusFlag;
}

/******************************************************************************
;       Function Name			:	bool I2c0SlaveApp_Crc32_Check(uint32_t u32Size)			
;       Function Description	:	Compute and check MCU flash CRC32 values.
;       Parameters				:	[u32Size] - Input App memory size.
;       Return Values			:	Return CRC32 check status.
;									(1) true - CRC32 check is correct.
;									(2) false - CRC32 check is failure.
;		Source ID				:
******************************************************************************/
bool I2c0SlaveApp_Crc32_Check(uint32_t u32Size)
{
    bool bCheck = false;
    uint32_t u32Index, u32AppCrc32;
    uint16_t u16CrcH, u16CrcL;

#if BACK_UP
    uint32_t u32OffsetAddr;

    u32AppUpdateAddress = FunApp_App_Update_Address_Get();
    u32OffsetAddr = u32AppUpdateAddress + 0x80U;
    for (u32Index = 0U; u32Index < u32Size; u32Index++) /* Calculate APP memory size CRC32,  APP_SIZE/A_PAGE = timers */
    {
        (void)FlashRwApp_Read((uint32_t)(u32OffsetAddr) + (u32Index * (uint32_t)(LEN_READ_MEM)), (uint32_t)(LEN_READ_MEM), &u8AppMemRawDataBuff[0]);
        if (u32Index == (u32Size - 1U)) /* Reserved for CustomerId, MCU Version and MCU CRC32 */
        {
            u32ComputeCrc32 = CrcApp_Crc32_Calculation(&u8AppMemRawDataBuff[0], (uint32_t)(LEN_READ_MEM - LEN_RESERVED), u32ComputeCrc32);
        }
        else
        {
            u32ComputeCrc32 = CrcApp_Crc32_Calculation(&u8AppMemRawDataBuff[0], (uint32_t)(LEN_READ_MEM), u32ComputeCrc32);
        }
    }

    u32ComputeCrc32 = (~u32ComputeCrc32);

    /* Get APP CRC32 Value */
    (void)FlashRwApp_Read(u32AppUpdateAddress + 0xFFFC, LEN_APP_CRC32, u8AppCrc32Buff);

#else
    for (u32Index = 0U; u32Index < u32Size; u32Index++) /* Calculate APP memory size CRC32,  APP_SIZE/A_PAGE = timers */
    {
        (void)FlashRwApp_Read((uint32_t)(ADDR_APP_START) + (u32Index * (uint32_t)(LEN_READ_MEM)), (uint32_t)(LEN_READ_MEM), &u8AppMemRawDataBuff[0]);
        if (u32Index == (u32Size - 1U)) /* Reserved for CustomerId, MCU Version and MCU CRC32 */
        {
            u32ComputeCrc32 = CrcApp_Crc32_Calculation(&u8AppMemRawDataBuff[0], (uint32_t)(LEN_READ_MEM - LEN_RESERVED), u32ComputeCrc32);
        }
        else
        {
            u32ComputeCrc32 = CrcApp_Crc32_Calculation(&u8AppMemRawDataBuff[0], (uint32_t)(LEN_READ_MEM), u32ComputeCrc32);
        }
    }

    u32ComputeCrc32 = (~u32ComputeCrc32);

    /* Get APP CRC32 Value */
    (void)FlashRwApp_Read(ADDR_APP_CRC32, LEN_APP_CRC32, u8AppCrc32Buff);
#endif

    u16CrcH = ((uint16_t)(u8AppCrc32Buff[0]) << (uint16_t)(SHIFT_BIT_8)) | (uint16_t)(u8AppCrc32Buff[1]);
    u16CrcL = ((uint16_t)(u8AppCrc32Buff[2]) << (uint16_t)(SHIFT_BIT_8)) | (uint16_t)(u8AppCrc32Buff[3]);
    u32AppCrc32 = ((uint32_t)(u16CrcH) << (uint32_t)(SHIFT_BIT_16)) | (uint32_t)(u16CrcL);

#if DEBUG_UART
    DEBUG_PF("u32ComputeCrc32 = 0x%x ,u32AppCrc32 = 0x%x\r\n",u32ComputeCrc32,u32AppCrc32);
#endif

    /* Compare APP Crc32 */
    if (u32ComputeCrc32 != u32AppCrc32)
    {
        bCheck = false; /* CRC32 fail */
    }
    else
    {
        if (u32ComputeCrc32 != 0xffffffff)
        {
            bCheck = true; /* CRC32 pass */
        }
        else
        {
            bCheck = false; /* CRC32 fail */
        }
    }

    (void)(u32Size);
    return bCheck;
}

/********************************************************
0xEn/Fn Cmd packet:
				   |-------------- Len --------------|
		+--------------------------------------------+
		|  CMD_ID  |   Len   |  Data[n]  | Checksum  |
		+--------------------------------------------+
		|    1 B   |    1B   |    n B    |    1 B    |
		+--------------------------------------------+
		|---------   checksum   ---------|

0xE7 Data packet:
				   |--------------------- Len ---------------------|
		+----------------------------------------------------------+
		|  CMD_ID  |   Len   |  serial id  |  Data[n]  | Checksum  |
		+----------------------------------------------------------+
		|    1 B   |    1B   |     2 B     |    n B    |    1 B    |
		+----------------------------------------------------------+
		|----------------   checksum   ----------------|		
		CMD_ID 		: 	0xEn / 0xFn
						0xE7	: update date packet 
						0xF7	: Response od 0xE7
						0xE8	: Whole date packet checksum 
						0xF8	: Response od 0xE8

		Checksum 	:	(8 bit add sum)[from CMD_ID to Data] +1 
		Len 		:	Byte count from Len to Checksum
		Data[n]		:	Up to 128 bytes

 		
********************************************************/
static bool I2c0SlaveApp_Rx_Checksum_Check(void)
{
    bool bCheck = true;
    uint8_t u8checksum = 0;

    u8checksum = CrcApp_CalculateCheckSum8((uint8_t *)rxBuffer, tsI2cSlaveRx.length - 1);
    if (u8checksum == tsI2cSlaveRx.buf[tsI2cSlaveRx.length - 1])
    {
        bCheck = true;
    }
    else
    {
        bCheck = false;
    }

    return bCheck;
}

static bool I2c0SlaveApp_Prepare_Response_Buf(uint8_t u8cmd, uint8_t u8datalen, uint8_t *u8buf)
{
    uint8_t u8ret;
    uint8_t u8checksum_pos = u8datalen;

    if (u8datalen > BUFFER_RES_MAX)
    {
        u8ret = false;
    }
    else
    {
        txBuffer[OFFSET_CMD] = u8cmd;
        txBuffer[OFFSET_LEN] = u8datalen;

        for (int i = 0; i < (u8datalen - 2); i++)
        {
            txBuffer[OFFSET_DAT + i] = u8buf[i];
        }

        txBuffer[u8checksum_pos] = CrcApp_CalculateCheckSum8((uint8_t *)txBuffer, u8datalen);
        u8ret = true;
    }
    return u8ret;
}

void I2c0SlaveApp_Update_Cmd_Process(void)
{
    uint8_t u8Cmd;
    u8Cmd = rxBuffer[UPDATE_CMD];

    if (tsI2cSlaveRx.length > 0)
    {
        switch (u8Cmd)
        {
        case CMD_UPDATE_REQUEST:
            /*Now is already in bootloader, should not receive this cmd*/
            if (I2c0SlaveApp_Rx_Checksum_Check() == true)
            {
                u8retMsg[0] = 0x43;
            }
            else
            {
                u8retMsg[0] = 0x7F;
            }
            I2c0SlaveApp_Prepare_Response_Buf(CMD_UPDATE_ACK, LEN_UPDATE_ACK, u8retMsg);
            break;

        case CMD_BOOTLOADER_HS:
            if (I2c0SlaveApp_Rx_Checksum_Check() == true)
            {
                u8retMsg[0] = 0x43;
                u8BootloaderStatusFlag |= (uint8_t)SET_BL_HANDSHAKE;
            }
            else
            {
                u8retMsg[0] = 0x7F;
            }

            I2c0SlaveApp_Prepare_Response_Buf(CMD_BOOTLOADER_HS_ACK, LEN_BOOTLOADER_HS_ACK, u8retMsg);
            break;

        case CMD_ERASE_REQUEST:
            if ((I2c0SlaveApp_Rx_Checksum_Check() == true) && ((u8BootloaderStatusFlag & 0x01U) == (uint8_t)SET_BL_HANDSHAKE))
            {
                u8retMsg[0] = 0x43;
                u8BootloaderStatusFlag |= (uint8_t)SET_APP_ERASE_EVENT;
                // reset checksum value caculate
                u8checksum_reset = 1;
            }
            else
            {
                u8retMsg[0] = 0x7F;
            }
            I2c0SlaveApp_Prepare_Response_Buf(CMD_ERASE_REQUEST_ACK, LEN_ERASE_REQUEST_ACK, u8retMsg);
            break;

        case CMD_APP_PROGRAM:
            if ((I2c0SlaveApp_Rx_Checksum_Check() == true) && ((u8BootloaderStatusFlag & 0x03U) == ((uint8_t)SET_BL_HANDSHAKE | (uint8_t)SET_APP_ERASE)))
            {
                u16SerialID = ((tsI2cSlaveRx.buf[OFFSET_DAT] << 8U) | tsI2cSlaveRx.buf[OFFSET_DAT + 1]);
#if PACKAGE_1
                if ((u16SerialID > 0x0000U) && (u16SerialID < 0x0201U))
#else
                if ((u16SerialID >= 0x0000U) && (u16SerialID < 0x0200U))
#endif
                {
                    u8retMsg[0] = tsI2cSlaveRx.buf[OFFSET_DAT];
                    u8retMsg[1] = tsI2cSlaveRx.buf[OFFSET_DAT + 1U];
                    u8BootloaderStatusFlag |= (uint8_t)SET_APP_PROGRAM_EVENT;

                    CrcApp_CalculateAllDataCheckSum16((uint8_t *)tsI2cSlaveRx.buf, tsI2cSlaveRx.length - 5, u8checksum_reset);
                    u8checksum_reset = 0;
                }
                else
                {
                    u8retMsg[0] = 0xFFU;
                    u8retMsg[1] = 0xFFU;
                }
            }
            else
            {
                u8retMsg[0] = 0xFFU;
                u8retMsg[1] = 0xFFU;
            }
            I2c0SlaveApp_Prepare_Response_Buf(CMD_APP_PROGRAM_ACK, LEN_APP_PROGRAM_ACK, u8retMsg);
            break;

        case CMD_APP_CHECKSUM:
            if ((I2c0SlaveApp_Rx_Checksum_Check() == true) && (u16SerialID > 2U))
            {
                u16CalculateChecksum = CrcApp_GetCheckSum16();
                u16DataChecksum = (tsI2cSlaveRx.buf[OFFSET_DAT] << 8U) | (tsI2cSlaveRx.buf[OFFSET_DAT + 1U]);
                if (u16CalculateChecksum == u16DataChecksum)
                {
                    u8retMsg[0] = 0U; // check pass
                    u8BootloaderStatusFlag |= (uint8_t)SET_APP_CHECKSUM_SUCCESS;
                }
                else
                {
                    u8retMsg[0] = 1U; // check fail
                }
            }
            else
            {
                u8retMsg[0] = 1U; // check fail
            }
            I2c0SlaveApp_Prepare_Response_Buf(CMD_APP_CHECKSUM_ACK, LEN_APP_CHECKSUM_ACK, u8retMsg);
            break;

        case CMD_UPDATE_STATUS:
            if (I2c0SlaveApp_Rx_Checksum_Check() == true)
            {
                u8BootloaderStatusFlag |= (uint8_t)SET_UPDATE_STATUS_EVENT;
                if ((u8BootloaderStatusFlag & 0x07U) == ((uint8_t)SET_BL_HANDSHAKE | (uint8_t)SET_APP_ERASE | (uint8_t)SET_APP_CHECKSUM_SUCCESS))
                {
#if BACK_UP
                    if (I2c0SlaveApp_Crc32_Check(TIMES_APP_MEM - TIMES_OFFSET_ADDR_RESERVED) == true) /* Check APP CRC */
                    {
                        u8retMsg[0] = 0U;
                        u8BootloaderStatusFlag |= (uint8_t)SET_UPDATE_SUCCESS;
                        FlashRwApp_Change_App_Update_Area();
                    }
#else
                    if (I2c0SlaveApp_Crc32_Check(TIMES_APP_MEM) == true) /* Check APP CRC */
                    {
                        u8retMsg[0] = 0U;
                        u8BootloaderStatusFlag |= (uint8_t)SET_UPDATE_SUCCESS;
                    }
#endif
                    else
                    {
                        u8retMsg[0] = 1U;
                    }
                }
                else
                {
                    u8retMsg[0] = 1U;
                }
            }
            else
            {
                u8retMsg[0] = 1U;
            }
            I2c0SlaveApp_Prepare_Response_Buf(CMD_UPDATE_STATUS_ACK, LEN_UPDATE_STATUS_ACK, u8retMsg);
            break;

        case USER_CMD_STATUS:
            if (I2c0SlaveApp_Rx_Checksum_Check() == true)
            {
                u8retMsg[0] = u8BootloaderStatusFlag;
            }
            else
            {
                u8retMsg[0] = 0xFFU;
            }
            I2c0SlaveApp_Prepare_Response_Buf(CMD_USER_CMD_STATUS_ACK, LEN_UPDATE_STATUS_ACK, u8retMsg);
            break;

        default:;
            break;
        }
        tsI2cSlaveRx.length = 0U;
    }
    else
    {
        ;
    }
}

void I2c0SlaveApp_Flash_Operate_Process(void)
{
    if ((u8BootloaderStatusFlag & 0x20U) == (uint8_t)SET_APP_ERASE_EVENT)
    {
#if BACK_UP
        u8BootloaderStatusFlag &= ~(uint8_t)SET_APP_ERASE_EVENT;
        u32AppUpdateAddress = FunApp_App_Update_Address_Get();
        for (uint32_t u32Index = 0U; u32Index < LEN_ERASE; u32Index++)
        {
            (void)FlashRwApp_Sector_Erase(u32AppUpdateAddress + (u32Index * 0x8000U));
        }
        u8BootloaderStatusFlag |= (uint8_t)SET_APP_ERASE;
#if DEBUG_UART
        DEBUG_PF("Erase command success!\r\n");
#endif
#else
        u8BootloaderStatusFlag &= ~(uint8_t)SET_APP_ERASE_EVENT;
        for (uint32_t u32Index = 0U; u32Index < LEN_ERASE; u32Index++)
        {
            (void)FlashRwApp_Sector_Erase((uint32_t)ADDR_APP_START + (u32Index * 0x8000U));
        }
        u8BootloaderStatusFlag |= (uint8_t)SET_APP_ERASE;
#if DEBUG_UART
        DEBUG_PF("Erase command success!\r\n");
#endif
#endif
    }

    if ((u8BootloaderStatusFlag & 0x40U) == (uint8_t)SET_APP_PROGRAM_EVENT)
    {
        uint32_t u32FlashWriteAddr;

#if BACK_UP
        u8BootloaderStatusFlag &= ~(uint8_t)SET_APP_PROGRAM_EVENT;
        u32AppUpdateAddress = FunApp_App_Update_Address_Get();
        u32FlashWriteAddr = u32AppUpdateAddress + (u16SerialID << 7U);
        if ((u16SerialID == 0x0001U) && (u32AppUpdateAddress == APP1_START_ADDR)) /* Ignore package 0 because 128 bytes is for offset start address information */
        {
            FlashRwApp_128B_Page_Write(u32FlashWriteAddr, (uint8_t *)&u8APP1BinOffsetAddr[0]);
        }
        else if ((u16SerialID == 0x0001U) && (u32AppUpdateAddress == APP2_START_ADDR))
        {
            FlashRwApp_128B_Page_Write(u32FlashWriteAddr, (uint8_t *)&u8APP2BinOffsetAddr[0]);
        }
        else
        {
            FlashRwApp_128B_Page_Write(u32FlashWriteAddr, (uint8_t *)&tsI2cSlaveRx.buf[OFFSET_DAT + 2]);
        }
#else
        u8BootloaderStatusFlag &= ~(uint8_t)SET_APP_PROGRAM_EVENT;

#if PACKAGE_1
        u32FlashWriteAddr = (uint32_t)ADDR_APP_START + ((u16SerialID - 1U) << 7U);
#else
        u32FlashWriteAddr = (uint32_t)ADDR_APP_START + (u16SerialID << 7U);
#endif

        FlashRwApp_128B_Page_Write(u32FlashWriteAddr, (uint8_t *)&tsI2cSlaveRx.buf[OFFSET_DAT + 2]);
#endif

#if DEBUG_UART
        DEBUG_PF("Package = %d\r\n", u16SerialID);
#endif
    }

    if (((u8BootloaderStatusFlag & 0x80U) == (uint8_t)SET_UPDATE_STATUS_EVENT) && (gbResetFlag == true))
    {
        u8BootloaderStatusFlag = (uint8_t)(DEFAULT_BL_STATUS);
        Cy_SysLib_Delay(20); // Delay 50 msec(250)
        InitApp_Startup_DeInit();
    }
}

static void SlaveErrorEvent(uint32_t u32ErrStatus)
{
#if DEBUG_UART
    DEBUG_PF("I2CER(0x%x)\r\n", u32ErrStatus);
    if (u32ErrStatus & I2C_SLAVE_STATUS_RD_BUSY)
    {
        DEBUG_PF("\tI2C_SLAVE_STATUS_RD_BUSY\r\n");
    }
    if (u32ErrStatus & I2C_SLAVE_STATUS_RD_UNDRFL)
    {
        DEBUG_PF("\tI2C_SLAVE_STATUS_RD_UNDRFL\r\n");
    }
    if (u32ErrStatus & I2C_SLAVE_STATUS_WR_BUSY)
    {
        DEBUG_PF("\tI2C_SLAVE_STATUS_WR_BUSY\r\n");
    }
    if (u32ErrStatus & I2C_SLAVE_STATUS_WR_OVRFL)
    {
        DEBUG_PF("\tI2C_SLAVE_STATUS_WR_OVRFL\r\n");
    }
    if (u32ErrStatus & I2C_SLAVE_STATUS_ARB_LOST)
    {
        DEBUG_PF("\tI2C_SLAVE_STATUS_ARB_LOST\r\n");
    }
    if (u32ErrStatus & I2C_SLAVE_STATUS_BUS_ERR)
    {
        DEBUG_PF("\tI2C_SLAVE_STATUS_BUS_ERR\r\n");
    }
#endif
}

static void Scb_I2C_Slave_Event(uint32_t u32Events)
{
    uint32_t u32ErrStatus;
    switch (u32Events)
    {
    case I2C_SLAVE_READ_EVENT:
        HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_GROUP, txBuffer, I2CS_RBUFFER_SIZE);
        if (txBuffer[0] == 0xF9)
        {
            gbResetFlag = true;
        }
        break;

    case I2C_SLAVE_WRITE_EVENT:
        HAL_I2C_Slave_WriteBuf_Config(I2C_SLAVE_GROUP, &rxBuffer[0], I2CS_WBUFFER_SIZE);
        break;

    case I2C_SLAVE_RD_IN_FIFO_EVENT:
        break;

    case I2C_SLAVE_RD_BUF_EMPTY_EVENT:
        break;

    case I2C_SLAVE_RD_CMPLT_EVENT:
        break;

    case I2C_SLAVE_WR_CMPLT_EVENT:
        HAL_I2C_Slave_WriteTransferCount_Get(I2C_SLAVE_GROUP, (uint32_t *)&tsI2cSlaveRx.cnt);
        (void)memcpy((uint8_t *)&tsI2cSlaveRx.buf[0], &rxBuffer[0], tsI2cSlaveRx.cnt);
        tsI2cSlaveRx.length = tsI2cSlaveRx.cnt;

        I2c0SlaveApp_Update_Cmd_Process();
        break;

    case I2C_SLAVE_ERR_EVENT:
        HAL_I2C_Slave_Status_Get(I2C_SLAVE_GROUP, &u32ErrStatus);
        SlaveErrorEvent(u32ErrStatus);
        break;
    default:;
        break;
    }
}

/******************************************************************************
;       Function Name			:	uint8_t I2c0SlaveApp_Init(uint8_t u8Para)
;       Function Description	:	Initialize I2C slave function.
;       Parameters				:	[u8Para] - NOTHING
;       Return Values			:	NOTHING
;		Source ID				:
******************************************************************************/
uint8_t I2c0SlaveApp_Init(uint8_t u8Para)
{
    /** I2C Standard mode 400k bps*/
    gtsI2C_Slave_Fast_Mode.u8PinSDA = I2C_PIN_SDA;
    gtsI2C_Slave_Fast_Mode.u8PinSCL = I2C_PIN_SCL;
    gtsI2C_Slave_Fast_Mode.u8SlaveAddress = ADDR_SLAVE;

    HAL_I2C_Init(I2C_SLAVE_GROUP, &gtsI2C_Slave_Fast_Mode);

    /* Master read : Slave output data from g_i2c_tx_buf*/
    HAL_I2C_Slave_ReadBuf_Config(I2C_SLAVE_GROUP, &txBuffer[0], I2CS_RBUFFER_SIZE);

    /* Master write : Slave receive data put in to g_i2c_rx_buf*/
    HAL_I2C_Slave_WriteBuf_Config(I2C_SLAVE_GROUP, &rxBuffer[0], I2CS_WBUFFER_SIZE);

    HAL_I2C_Slave_Interrupt_Set(I2C_SLAVE_GROUP, FUNC_ENABLE, 0x00U);
    HAL_I2C_Callback_Register(I2C_SLAVE_GROUP, Scb_I2C_Slave_Event);
    HAL_I2C_Active(I2C_SLAVE_GROUP, FUNC_ENABLE);

    (void)(u8Para);
    return u8Para;
}

/******************************************************************************
;       Function Name			:	uint8_t I2c0SlaveApp_DeInit(uint8_t u8Para)
;       Function Description	:	
;       Parameters				:
;       Return Values			:
;		Source ID				:
******************************************************************************/
uint8_t I2c0SlaveApp_DeInit(uint8_t u8Para)
{
    HAL_I2C_DeInit(I2C_SLAVE_GROUP);
    (void)(u8Para);
    return u8Para;
}

#if BACK_UP
uint8_t u8APP1BinOffsetAddr[128] =
    {
        0x00,
        0x00,
        0x01,
        0x08,
        0x0D,
        0xC3,
        0x01,
        0x10,
        0xF1,
        0xC8,
        0x01,
        0x10,
        0xA9,
        0xC6,
        0x01,
        0x10,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xC5,
        0xC6,
        0x01,
        0x10,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xF5,
        0xC8,
        0x01,
        0x10,
        0xF9,
        0xC8,
        0x01,
        0x10,
        0xCD,
        0xB8,
        0x01,
        0x10,
        0xF3,
        0xB8,
        0x01,
        0x10,
        0x19,
        0xB9,
        0x01,
        0x10,
        0x3F,
        0xB9,
        0x01,
        0x10,
        0x65,
        0xB9,
        0x01,
        0x10,
        0x8B,
        0xB9,
        0x01,
        0x10,
        0xB1,
        0xB9,
        0x01,
        0x10,
        0xD7,
        0xB9,
        0x01,
        0x10,
        0xFD,
        0xC8,
        0x01,
        0x10,
        0x01,
        0xC9,
        0x01,
        0x10,
        0x05,
        0xC9,
        0x01,
        0x10,
        0x09,
        0xC9,
        0x01,
        0x10,
        0x0D,
        0xC9,
        0x01,
        0x10,
        0x11,
        0xC9,
        0x01,
        0x10,
        0x15,
        0xC9,
        0x01,
        0x10,
        0x19,
        0xC9,
        0x01,
        0x10,
};

uint8_t u8APP2BinOffsetAddr[128] =
    {
        0x00,
        0x00,
        0x01,
        0x08,
        0x0D,
        0x43,
        0x03,
        0x10,
        0xF1,
        0x48,
        0x03,
        0x10,
        0xA9,
        0x46,
        0x03,
        0x10,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xC5,
        0x46,
        0x03,
        0x10,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xF5,
        0x48,
        0x03,
        0x10,
        0xF9,
        0x48,
        0x03,
        0x10,
        0xCD,
        0x38,
        0x03,
        0x10,
        0xF3,
        0x38,
        0x03,
        0x10,
        0x19,
        0x39,
        0x03,
        0x10,
        0x3F,
        0x39,
        0x03,
        0x10,
        0x65,
        0x39,
        0x03,
        0x10,
        0x8B,
        0x39,
        0x03,
        0x10,
        0xB1,
        0x39,
        0x03,
        0x10,
        0xD7,
        0x39,
        0x03,
        0x10,
        0xFD,
        0x48,
        0x03,
        0x10,
        0x01,
        0x49,
        0x03,
        0x10,
        0x05,
        0x49,
        0x03,
        0x10,
        0x09,
        0x49,
        0x03,
        0x10,
        0x0D,
        0x49,
        0x03,
        0x10,
        0x11,
        0x49,
        0x03,
        0x10,
        0x15,
        0x49,
        0x03,
        0x10,
        0x19,
        0x49,
        0x03,
        0x10,
};
#endif
/*------------------------------------------------------------------------*/
