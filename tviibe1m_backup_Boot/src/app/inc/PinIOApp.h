/******************************************************************************
;       Program		: PinIOApp.h
;       Function	: Define IO Pin Function
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
******************************************************************************/
#ifndef PinIOAPP_H
#define PinIOAPP_H
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
/*---------------------------- Delcare Content  ----------------------------*/
#define	PARA_ODD_EVEN_NUM	2U

enum PinSet
{
	SET_PIN_BIT0 = 0x01U,
	SET_PIN_BIT1 = 0x02U,
	SET_PIN_BIT2 = 0x04U,
	SET_PIN_BIT3 = 0x08U,
	SET_PIN_BIT4 = 0x10U,
	SET_PIN_BIT5 = 0x20U,
	SET_PIN_BIT6 = 0x40U,
	SET_PIN_BIT7 = 0x80U,
	SET_PIN_BIT8 = 0x0100U,
	SET_PIN_BIT9 = 0x0200U,
	SET_PIN_BIT10 = 0x0400U,
	SET_PIN_BIT11 = 0x0800U,
	SET_PIN_BIT12 = 0x1000U,
	SET_PIN_BIT13 = 0x2000U,
	SET_PIN_BIT14 = 0x4000U,
	SET_PIN_BIT15 = 0x8000U,
	SET_PIN_BIT16 = 0x010000U,
	SET_PIN_BIT17 = 0x020000U,
	SET_PIN_BIT18 = 0x040000U,
	SET_PIN_BIT19 = 0x080000U,
	SET_PIN_BIT20 = 0x100000U,
	SET_PIN_BIT21 = 0x200000U,
	SET_PIN_BIT22 = 0x400000U,
	SET_PIN_BIT23 = 0x800000U,
	SET_PIN_BIT24 = 0x01000000U,
	SET_PIN_BIT25 = 0x02000000U,
	SET_PIN_BIT26 = 0x04000000U,
	SET_PIN_BIT27 = 0x08000000U,
	SET_PIN_BIT28 = 0x10000000U,
	SET_PIN_BIT29 = 0x20000000U,
	SET_PIN_BIT30 = 0x40000000U,
	SET_PIN_BIT31 = 0x80000000U
};

enum ShiftBit{
    SHIFT_BIT_0, 		
    SHIFT_BIT_1,		
    SHIFT_BIT_2,			
    SHIFT_BIT_3,			
    SHIFT_BIT_4,			
    SHIFT_BIT_5,			
    SHIFT_BIT_6,			
    SHIFT_BIT_7,			
    SHIFT_BIT_8,			
    SHIFT_BIT_9,			
    SHIFT_BIT_10,		
    SHIFT_BIT_11,		
    SHIFT_BIT_12,		
    SHIFT_BIT_13,		
    SHIFT_BIT_14,		
    SHIFT_BIT_15,		
    SHIFT_BIT_16,		
    SHIFT_BIT_17,		
    SHIFT_BIT_18,		
    SHIFT_BIT_19,		
    SHIFT_BIT_20,		
    SHIFT_BIT_21,		
    SHIFT_BIT_22,		
    SHIFT_BIT_23,		
    SHIFT_BIT_24,		
    SHIFT_BIT_25,		
    SHIFT_BIT_26,		
    SHIFT_BIT_27,		
    SHIFT_BIT_28,		
    SHIFT_BIT_29,		
    SHIFT_BIT_30,		
    SHIFT_BIT_31,		
    SHIFT_BIT_32		
};

enum GpioPort
{
	PORT_A = 0U, 
	PORT_B = 1U,
	PORT_C = 2U
};

enum GpioPin
{
	BIT0 = 0U,
	BIT1 = 1U,
	BIT2,
	BIT3,
	BIT4,
	BIT5,
	BIT6,
	BIT7,
	BIT8,
	BIT9,
	BIT10,
	BIT11,
	BIT12,
	BIT13,
	BIT14,
	BIT15,
	BIT16, 
	BIT17,
	BIT18,
	BIT19,
	BIT20, 
	BIT21,
	BIT22,
	BIT23,
	BIT24, 
	BIT25,
	BIT26,
	BIT27,
	BIT28,
	BIT29,
	BIT30,
	BIT31
};

enum SysPower
{	
	SYS_PWR_OFF = 0U,
	SYS_PWR_ON = 1U
};

enum ProcessStatus
{
	PINIO_FINISH = 0U,
	PINIO_FAIL = 1U
};

enum HwVerPins{
	SET_PCBA_A3 = 0x1000U,
	SET_PCBA_A2 = 0x0100U,	
	SET_PCBA_A1 = 0x0010U,
	SET_PCBA_A0 = 0x0001U
};

enum PinLevel{
	PIN_LEVEL_HIGH = 1U,
	PIN_LEVEL_LOW = 0U
};
/*---------------------------- I/O Pin Definition ---------------------------*/	
#define	portXRES_TP_R		PORT_A
#define	pinXRES_TP_R		BIT1

#define	portBATT_SENSE_EN	PORT_A
#define	pinBATT_SENSE_EN	BIT2

#define	portBATT_SENSE_ADC	PORT_A
#define	pinBATT_SENSE_ADC	BIT3

#define	portI2C_SDA_DES_SLAVE	PORT_A
#define	pinI2C_SDA_DES_SLAVE	BIT8

#define	portI2C_SCL_DES_SLAVE	PORT_A
#define	pinI2C_SCL_DES_SLAVE	BIT9

#define	portMAX20048_EN1	PORT_A
#define	pinMAX20048_EN1		BIT10

#define	portMAX20048_EN2	PORT_A
#define	pinMAX20048_EN2		BIT18

#define	portTCON_RESET		PORT_A
#define	pinTCON_RESET		BIT20

#define	portTCON_PWMI		PORT_A
#define	pinTCON_PWMI		BIT21

#define	portPCBA_A3		PORT_A
#define	pinPCBA_A3		BIT27

#define	portPCBA_A2		PORT_A
#define	pinPCBA_A2		BIT28

#define	portPCBA_A0		PORT_B
#define	pinPCBA_A0		BIT0

#define	portPCBA_A1		PORT_B
#define	pinPCBA_A1		BIT1

#define	portPDB			PORT_B
#define	pinPDB			BIT4

#define	portMCU_5V_EN		PORT_B
#define	pinMCU_5V_EN		BIT6

#define	portXON			PORT_B
#define	pinXON			BIT19

#define	portTCON_BIST		PORT_B
#define	pinTCON_BIST		BIT20

#define	portDRIVER_RESET	PORT_B
#define	pinDRIVER_RESET		BIT22

#define	portDUAL_NRST		PORT_B
#define	pinDUAL_NRST		BIT24

#define	portDUAL_CLKIN		PORT_B
#define	pinDUAL_CLKIN		BIT25

#define	portDEBUG_TX		PORT_B
#define	pinDEBUG_TX		BIT30

#define	portDEBUG_RX		PORT_B
#define	pinDEBUG_RX		BIT31

#define	portMCU_INT		PORT_C
#define	pinMCU_INT		BIT0

#define	portMCU_1V8_EN		PORT_C
#define	pinMCU_1V8_EN		BIT19

#define	portMCU_WD_RESET	PORT_C
#define	pinMCU_WD_RESET		BIT24

#define	portMCU_WDI		PORT_C
#define	pinMCU_WDI		BIT25

#define	portMCU_3V3_1V1_EN	PORT_C
#define	pinMCU_3V3_1V1_EN	BIT28
/*---------------------------- Declare Function Replace Name -----------------*/
/*---------------------------- Declare Function -----------------------------*/
bool PinIOApp_System_Power(uint8_t u8Switch);
uint8_t PinIOApp_Init(uint8_t u8Para);
uint8_t MPSC_Check_Power_Alive(uint8_t status, uint8_t times);
#endif
