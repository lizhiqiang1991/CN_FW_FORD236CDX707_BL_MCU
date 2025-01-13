/******************************************************************************
;       Program		: FunApp.h
;       Function	: Declare Scenario Functions
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
******************************************************************************/
#ifndef FunApp_H
#define FunApp_H
/*---------------------------- Inculde Library ------------------------------*/
/*---------------------------- Define Constant ------------------------------*/
#define 	APP_START_ADDR		ADDR_APP_START

#define		UPDATE_REQUEST	    ((void*)0x08002000)		/* Update request SRAM Address */

/* Boot Access Key */
//#define		CHK_UPDATE_KEY	0x5AU
#define		CHK_UPDATE_KEY	0x55AAU
//#define		CHK_UPDATE_KEY	0xf25bU

#define		CLR_KEYS				0x0000U

#define		DELAY_CLK_48MHZ			9500U	/* Based on 1ms, system clock 48MHz */

#define		ASM_NOP()			__NOP()
#define		ASM_SYS_RESET()     (__NVIC_SystemReset())
#define		ASM_SET_MSP(u32Msp)	(__set_MSP(u32Msp))
#define		ASM_VECTOR_RESET(u32ResetVector)	asm("bx %0"::"r" (u32ResetVector))

#define		CNT_DONE			0U
#define		CNT_REDUCE			1U

#define		SIZE_ACCESS_KEY		6U
#define     BOOTSWVersion_LEN   10U
/*---------------------------- Support Function -----------------------------*/
#define		FunApp_PAC_Irq_Disable(u8Irq)	(NVIC_DisableIRQ(u8Irq))
#define 	FunApp_Nvic_Init(u8Para)        (__enable_irq())
/*---------------------------- Declare Function -----------------------------*/
__no_init volatile uint16_t u16UpdateKey @0x08001500;
//__no_init volatile uint16_t u16UpdateKey @0x08001500;
uint8_t FunApp_Startup_Check(uint8_t u8Para);
uint32_t FunApp_App_Update_Address_Get(void);
#endif
