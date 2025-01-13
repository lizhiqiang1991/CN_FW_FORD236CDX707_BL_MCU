/******************************************************************************
;       Program		: I2c0SlaveApp.h
;       Function	: Declare and Define I2C Slave Function
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 21
;       Author		: Joel Wang
******************************************************************************/
#ifndef I2c0SlaveApp_H
#define I2c0SlaveApp_H
/*---------------------------- Include Library -------------------------------*/
/*---------------------------- Define Constant -------------------------------*/
#define     PACKAGE_1                   true

#define     NUM_ZERO                    0U
#define     NUM_ONE                     1U
#define		ADDR_SLAVE					0x71U
#define     QUEUE_MAX                   256U

/** Please Refer to datasheet to check the SCB5 SDA/SCL pin*/
#define 	I2C_PIN_SDA					PIN_I2C_SCB1_SDA
#define 	I2C_PIN_SCL     			PIN_I2C_SCB1_SCL
#define 	I2C_SLAVE_GROUP   			I2C_GROPU1
#define     I2CS_WBUFFER_SIZE           256U
#define     I2CS_RBUFFER_SIZE           32U
#define		ECU_VER						"F1.00"

#define		ADDR_APP_START					0x10010000U


#define		UNIT_KB						1024U
#define		SIZE_64B					64U
#define		SIZE_256B					256U
#define		SIZE_1KB					(1U * UNIT_KB)
#define		SIZE_4KB					(4U * UNIT_KB)
#define		SIZE_64KB					(64U * UNIT_KB)
#define		SIZE_128KB					(128U * UNIT_KB)
#define		SIZE_256KB					(256U * UNIT_KB)
#define		SIZE_APP_BASE				SIZE_64KB	/* APP memory size */

#define		SET_APP_BLOCK				SIZE_64B
#define		SIZE_APP_MEM_BLOCK			SET_APP_BLOCK		/* APP memory block */

#define		SIZE_APP_CRC32				4U

#define		LEN_ERASE					2U //(uint32_t)(SIZE_APP_BASE/SIZE_256B)			/* ERASE unit are 256B. APP size is 64K. */


#define		LEN_APP_CRC32			    SIZE_APP_CRC32
#define		LEN_READ_MEM				SET_APP_BLOCK	/* Read unit is 64B */
#define		LEN_RESERVED				4U				/* Reserved 4 bytes for MCU_CRC32 */
#define		ADDR_APP_CRC32		        0x1001FFFCU

/* UART Packets Length */
#define		LEN_CRC16					2U
#define		LEN_CRC16_L					1U
#define		LEN_CRC16_H					1U
#define		LEN_IRQ_STATUS				1U

#define		TIMES_APP_MEM				(uint32_t)(SIZE_APP_BASE / LEN_READ_MEM)		/* SIZE_APP_MEM / SIZE_APP_MEM_BLOCK = 64KB/64B = 1024 times, read APP memory 64KB memory */
#define     TIMES_OFFSET_ADDR_RESERVED  2U

#define 	LEN_ID  					1U

/* Ford CDX707 Bootloader Command */
#define     UPDATE_CMD                  0U

#define		CMD_UPDATE_REQUEST			((uint8_t)0xE4U)
#define		CMD_BOOTLOADER_HS			((uint8_t)0xE5U)
#define		CMD_ERASE_REQUEST			((uint8_t)0xE6U)
#define		CMD_APP_PROGRAM				((uint8_t)0xE7U)
#define		CMD_APP_CHECKSUM			((uint8_t)0xE8U)
#define		CMD_UPDATE_STATUS			((uint8_t)0xE9U)
#define		USER_CMD_STATUS			    ((uint8_t)0xEAU)


#define		CMD_UPDATE_ACK				((uint8_t)0xF4U)
#define		CMD_BOOTLOADER_HS_ACK		((uint8_t)0xF5U)
#define		CMD_ERASE_REQUEST_ACK		((uint8_t)0xF6U)
#define		CMD_APP_PROGRAM_ACK			((uint8_t)0xF7U)
#define		CMD_APP_CHECKSUM_ACK		((uint8_t)0xF8U)
#define		CMD_UPDATE_STATUS_ACK	    ((uint8_t)0xF9U)
#define		CMD_USER_CMD_STATUS_ACK	    ((uint8_t)0xFAU)

#define		LEN_UPDATE					((uint8_t)0x03U)
#define		LEN_BOOTLOADER_HS			((uint8_t)0x03U)
#define		LEN_ERASE_REQUEST			((uint8_t)0x03U)
#define		LEN_APP_PROGRAM			    ((uint8_t)0x84U)
#define		LEN_APP_CHECKSUM			((uint8_t)0x04U)
#define		LEN_UPDATE_STATUS			((uint8_t)0x02U)

#define		LEN_UPDATE_ACK			    ((uint8_t)0x03U)
#define		LEN_BOOTLOADER_HS_ACK		((uint8_t)0x03U)
#define		LEN_ERASE_REQUEST_ACK		((uint8_t)0x03U)
#define		LEN_APP_PROGRAM_ACK			((uint8_t)0x04U)
#define		LEN_APP_CHECKSUM_ACK		((uint8_t)0x03U)
#define		LEN_UPDATE_STATUS_ACK		((uint8_t)0x03U)

#define		LEN_USER_CMD_STATUS_ACK		((uint8_t)0x03U)

#define 	BUFFER_RES_MAX		        4U
#define 	OFFSET_CMD			        0U
#define 	OFFSET_LEN			        1U
#define 	OFFSET_DAT			        2U

#define     DEFAULT_BL_STATUS           0x00U
/* Bootloader Status Flags Packets */
#define		SET_BL_HANDSHAKE			0x01U
#define		SET_APP_ERASE				0x02U
#define		SET_APP_CHECKSUM_SUCCESS	0x04U
#define		SET_UPDATE_SUCCESS		    0x08U
#define		SET_UPDATE_AREA			    0x10U
#define		SET_APP_ERASE_EVENT			0x20U
#define		SET_APP_PROGRAM_EVENT		0x40U
#define		SET_UPDATE_STATUS_EVENT		0x80U

/*---------------------------- Declare Enum ---------------------------------*/
enum NvcFunStatus
{
	NVC_FINISH = 0U,
	NVC_FAIL = 1U,
	NVC_WAIT = 2U, 
	NVC_BUSY = 3U,
};

/*------------- Read Packets --------------*/

/* Request Bootloader Handshake Packet(0xE5) */
enum HandshakePacket{
	HANDSHAKE_CMD,
	HANDSHAKE_LENGTH,
	HANDSHAKE_TYPE,
	HANDSHAKE_CHECKSUM
};

/* Request Erase APP Memory Packet(0xE6) */
enum ErasePacket{
	Erase_CMD,
	ERASE_LENGTH,
	ERASE_TYPE,
	ERASE_CHECKSUM
};

/* Request Program APP Memory Packet(0xE7) */
enum ProgramPacket{
	PROGRAM_CMD,
	PROGRAM_LENGTH,
	PROGRAM_NUMBER_H,
	PROGRAM_NUMBER_L,
    PROGRAM_DATA,
    PROGRAM_CHECKSUM = 132
};

/* APP Memory Checksum Packet(0xE8) */
enum APPChecksumPacket{
	APPCHECKSUM_CMD,
	APPCHECKSUM_LENGTH,
	APPCHECKSUM_H,
    APPCHECKSUM_L,
	APPCHECKSUM_CHECKSUM
};

/* Update Status Packet(0xE9) */
enum UpdateStatusPacket{
	UPDATESTATUS_CMD,
	UPDATESTATUS_LENGTH,
	UPDATESTATUS,
	UPDATESTATUS_CHECKSUM
};

/*------------- Read Packets End --------------*/

/*------------- Packets Data --------------*/
enum HandshakeDevice{
	HS_RH850 = 0x01U,
	HS_TOUCH = 0x02U,
    HS_MCU   = 0x03U    
};

enum HandshakeDeviceAck{
	HS_TOUCH_ACK = 0x42U,
	HS_MCU_ACK   = 0x43U,
    HS_FAIL_ACK  = 0x7FU    
};

enum EraseDevice{
	ERASE_RH850 = 0x01U,
	ERASE_TOUCH = 0x02U,
    ERASE_MCU   = 0x03U    
};

enum EraseDeviceAck{
	ERASE_TOUCH_ACK = 0x42U,
	ERASE_MCU_ACK   = 0x43U,
    ERASE_FAIL_ACK  = 0x7FU    
};

enum ChecksumAck{
	CHECKSUM_SUCCESS_ACK = 0x00U,
	CHECKSUM_FAIL_ACK    = 0x01U 
};

enum UpdateStatusAck{
	UPDATE_SUCCESS_ACK = 0x00U,
	UPDATE_FAIL_ACK    = 0x01U,
    UPDATE_DURING_ACK  = 0x02U
};
/*------------- Packets Data End --------------*/

typedef struct
{
    uint8_t buf[QUEUE_MAX];
    uint8_t cnt;
    uint8_t length;
}st_queue;

/*---------------------------- Support Function ------------------------------*/
/*---------------------------- Data Structure --------------------------------*/
/*---------------------------- Declare Function ------------------------------*/
uint8_t I2c0SlaveApp_BL_Status_Set(uint8_t u8Status);
bool I2c0SlaveApp_Crc32_Check(uint32_t u32Size);
void I2c0SlaveApp_Update_Cmd_Process(void);
void I2c0SlaveApp_Flash_Operate_Process(void);
uint8_t I2c0SlaveApp_Init(uint8_t u8Para);
uint8_t I2c0SlaveApp_DeInit(uint8_t u8Para);
//bool I2c0SlaveApp_Callback(SERCOM_I2C_SLAVE_TRANSFER_EVENT event, uintptr_t contextHandle);
void I2c0SlaveApp_APPAddress_Config(void);
#endif
