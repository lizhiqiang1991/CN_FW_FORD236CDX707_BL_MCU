/******************************************************************************
;       Program		: PinIOApp.c
;       Function	: GPIO Configure and Setting 
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
;       Describe	: HW Version (2 Bytes) = 0xA3A2A1A0
;					  Every pins size are 4 bits				  
;					  PCBA_A3_PA27 = High Nibble of High Byte
;					  PCBA_A2_PA28 = Low Nibble of High Byte
;					  PCBA_A1_PB01 = High Nibble of Low Byte
;					  PCBA_A0_PB00 = Low Nibble of Low Byte
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "InitApp.h"
/*---------------------------- Declare External Var -------------------------*/
#define COOLING_FAN_PWM_GROUP   PWM_GROUP0_CNT13
/*---------------------------- Start Program --------------------------------*/

/******************************************************************************
;       Function Name			:	bool PinIOApp_System_Power(uint8_t u8Switch)
;       Function Description	:	Switch system power on or off.
;       Parameters				:	[u8Switch] - Set system power on or off.
;       Return Values			:	true
;		Source ID				:
******************************************************************************/
bool PinIOApp_System_Power(uint8_t u8Switch)
{
	switch(u8Switch)
	{
		case SYS_PWR_ON:
		    /* ----- LM61460 -----*/
		    (void)HAL_GPIO_PinWrite(PIN_LM61460_EN,PIN_HIGH);     // P3V3_EN
            Cy_SysLib_Delay(7);      // Delay 7 msec
            
            if(MPSC_Check_Power_Alive(PIN_LM61460_PG, 30U) == (uint8_t)NUM_ZERO)
            {
                ;/*for 3V3 not good*/
            }
            else
            {
                ;
            }
            
            /* ----- MAX20419 -----*/
            (void)HAL_GPIO_PinWrite(PIN_20419_EN1,PIN_HIGH);    // P5V_EN
            (void)HAL_GPIO_PinWrite(PIN_20419_EN2,PIN_HIGH);    // P1V8_EN
            (void)HAL_GPIO_PinWrite(PIN_20419_EN3,PIN_HIGH);    // P1V1_EN
    
            Cy_SysLib_Delay(20);  // Delay 20 msec,for 5V,1V8,1V1 out
            
            if(MPSC_Check_Power_Alive(PIN_20419_RESET1, 30U) == (uint8_t)NUM_ZERO)
            {
                ;/*for 5V not good*/
            }
            else
            {
                ;
            }
            if(MPSC_Check_Power_Alive(PIN_20419_RESET2, 30U) == (uint8_t)NUM_ZERO)
            {
                ;/*for 1V8 not good*/
            }
            else
            {
                ;
            }
            
            if(MPSC_Check_Power_Alive(PIN_20419_RESET3, 30U) == (uint8_t)NUM_ZERO)
            {
                ;/*for 1V1 not good*/
            }
            else
            {
                ;
            }
            
            Cy_SysLib_Delay(2);  // Delay 2 msec
            
		     /* ----- DS90UH984 -----*/
		    (void)HAL_GPIO_PinWrite(PIN_984_PDB,PIN_HIGH);
          
		     /* ----- INTB initial set high -----*/
//		    (void)HAL_GPIO_PinWrite(PIN_984_INTB_IN,PIN_HIGH);     // PIN_984_INTB_IN =1
//		    (void)HAL_GPIO_PinWrite(PIN_MCU_INT,PIN_HIGH);        //  PIN_MCU_INT =1
		break;
        
		case SYS_PWR_OFF:
            /* ----- MAX25240 -----*/
            (void)HAL_GPIO_PinWrite(PIN_25240_EN_L,PIN_LOW); // Buck-Boost turn off
            (void)HAL_GPIO_PinWrite(PIN_25240_EN_R,PIN_LOW); // Buck-Boost turn off  
            
            /* ----- MAX25210  -----*/
            (void)HAL_GPIO_PinWrite(PIN_25210_EN_L,PIN_LOW);  // HV LDO BL off
            (void)HAL_GPIO_PinWrite(PIN_25210_EN_R,PIN_LOW);  // HV LDO BL off   
            
            /* ----- HX82105 -----*/
            (void)HAL_GPIO_PinWrite(PIN_PANEL_RESET,PIN_LOW); // Panel_RESET  =0
            
            /* ----- HX8880 -----*/
            (void)HAL_GPIO_PinWrite(PIN_TCON_RESET,PIN_LOW); // TCON_RESET = 0
            
            /* ----- MAX25221 -----*/
            (void)HAL_GPIO_PinWrite(PIN_MAX25221_EN,PIN_LOW); // MAX25221_DIS   
            
            /* ----- MAX25210  -----*/
            (void)HAL_GPIO_PinWrite(PIN_25210_EN_L,PIN_LOW);  // HV LDO BL off
            (void)HAL_GPIO_PinWrite(PIN_25210_EN_R,PIN_LOW);  // HV LDO BL off     
            
            /* ----- MAX20419 -----*/
            (void)HAL_GPIO_PinWrite(PIN_MAX20419_XWDI,PIN_LOW);    
            
            /* ----- DS90UH984 -----*/
            
            Cy_SysLib_Delay(3);  // Delay 3 msec

			/* ----- DS90UH984 -----*/
			(void)HAL_GPIO_PinWrite(PIN_984_PDB,PIN_LOW);
			(void)HAL_GPIO_PinWrite(PIN_984_INTB_IN,PIN_LOW);     // PIN_984_INTB_IN =0
			(void)HAL_GPIO_PinWrite(PIN_MCU_INT,PIN_LOW);        //  PIN_MCU_INT =0
			/* ----- MAX20419 ----*/
            (void)HAL_GPIO_PinWrite(PIN_20419_EN1,PIN_HIGH);   // 5V off
            (void)HAL_GPIO_PinWrite(PIN_20419_EN2,PIN_HIGH);   // 1V8 off
            (void)HAL_GPIO_PinWrite(PIN_20419_EN3,PIN_HIGH);   // 1V1 off
			/* ----- LM61460 -----*/
			(void)HAL_GPIO_PinWrite(PIN_LM61460_EN,PIN_LOW);     // P3V3_DIS

			(void)HAL_GPIO_PinWrite(PIN_PANEL_FPCA_DET_OUT_R,PIN_LOW);  // FPC ouput off
			(void)HAL_GPIO_PinWrite(PIN_PANEL_FPCA_DET_OUT_L,PIN_LOW);  // FPC ouput off
			(void)HAL_GPIO_PinWrite(PIN_TCON_BIST,PIN_LOW);     // PIN_TCON_BIST ,BIST disable
			(void)HAL_GPIO_PinWrite(PIN_TCON_BIST_STOP,PIN_LOW);     // pattern stop
			(void)HAL_GPIO_PinWrite(PIN_MAX20419_XWDI,PIN_LOW);
		    break;
		default:
			/* NOTHING */
		break;
	}

	(void)(u8Switch);
	return true;
}

/******************************************************************************
;       Function Name			:	uint8_t PinIOApp_Init(uint8_t u8Para)
;       Function Description	:	Initialize all pin.
;       Parameters				:	[u8Para] - NOTHING
;       Return Values			:	NOTHING
;		Source ID				:
******************************************************************************/
uint8_t PinIOApp_Init(uint8_t u8Para)
{
   /**< GPIO as Input */
    HAL_GPIO_Pin_Init(PIN_MCU_DETECT0,&gtdPinInputConfig);          /**< P0.0 */
    HAL_GPIO_Pin_Init(PIN_MCU_DETECT1,&gtdPinInputConfig);          /**< P0.1 */
    HAL_GPIO_Pin_Init(PIN_25240_PG_L,&gtdPinInputConfig);           /**< P3.0 */
    HAL_GPIO_Pin_Init(PIN_25210_RESET_L,&gtdPinInputConfig);        /**< P5.0 */
    HAL_GPIO_Pin_Init(PIN_25501_FLTB_L,&gtdPinInputConfig);         /**< P5.2 */
    HAL_GPIO_Pin_Init(PIN_FPCA_DET_IN_R,&gtdPinInputConfig);        /**< P7.4 */
    HAL_GPIO_Pin_Init(PIN_FPCA_DET_IN_L,&gtdPinInputConfig);        /**< P7.5 */
    HAL_GPIO_Pin_Init(PIN_PANEL_ABD,&gtdPinInputConfig);            /**< P8.1 */
    HAL_GPIO_Pin_Init(PIN_TCON_ASIL_O,&gtdPinInputConfig);          /**< P12.2 */
    HAL_GPIO_Pin_Init(PIN_984_LOCK,&gtdPinInputConfig);             /**< P13.4 */
    HAL_GPIO_Pin_Init(PIN_MAX25221_FLTB,&gtdPinInputConfig);        /**< P13.7 */
    HAL_GPIO_Pin_Init(PIN_MAX20419_PG,&gtdPinInputConfig);          /**< P14.1 */
    HAL_GPIO_Pin_Init(PIN_LM61460_PG,&gtdPinInputConfig);           /**< P14.3 */
    HAL_GPIO_Pin_Init(PIN_MAX25501_FLTB_R,&gtdPinInputConfig);      /**< P17.2 */
    HAL_GPIO_Pin_Init(PIN_25210_RESET_R,&gtdPinInputConfig);        /**< P18.3 */
#if 0
    /* PCBA2.0 */
    HAL_GPIO_Pin_Init(PIN_25240_PG_R,&gtdPinInputConfig);           /**< P18.5 */  
#else
    /* PCBA3.0 pin */
    HAL_GPIO_Pin_Init(PIN_25240_PG_R,&gtdPinInputConfig);           /**< P19.1 */
#endif 
    HAL_GPIO_Pin_Init(PIN_20419_RESET1,&gtdPinInputConfig);         /**< P21.5 */
    HAL_GPIO_Pin_Init(PIN_20419_RESET2,&gtdPinInputConfig);         /**< P22.0 */
    HAL_GPIO_Pin_Init(PIN_20419_RESET3,&gtdPinInputConfig);         /**< P22.1 */


            
    /**< GPIO as Output */
    HAL_GPIO_Pin_Init(PIN_MCU_INT,&gtdPinOutputConfig);             /**< P0.2 */
    HAL_GPIO_Pin_Init(PIN_25240_EN_L,&gtdPinOutputConfig);          /**< P3.1 */
    HAL_GPIO_Pin_Init(PIN_25210_EN_L,&gtdPinOutputConfig);          /**< P5.1 */
    HAL_GPIO_Pin_Init(PIN_U301_VBATT_SENSE,&gtdPinOutputConfig);    /**< P5.3 */
    HAL_GPIO_Pin_Init(PIN_W25X20_WP,&gtdPinOutputConfig);           /**< P6.5 */
    HAL_GPIO_Pin_Init(PIN_PANEL_FPCA_DET_OUT_R,&gtdPinOutputConfig);/**< P7.3 */
    HAL_GPIO_Pin_Init(PIN_PANEL_FPCA_DET_OUT_L,&gtdPinOutputConfig);/**< P8.0 */
    HAL_GPIO_Pin_Init(PIN_PANEL_RESET,&gtdPinOutputConfig);         /**< P8.2 */
#if 0
    /* PCBA2.0 */
    HAL_GPIO_Pin_Init(PIN_TCON_BIST_STOP,&gtdPinOutputConfig);      /**< P12.0 */
#else
    /* PCBA3.0 */
    HAL_GPIO_Pin_Init(PIN_TCON_BIST_STOP,&gtdPinOutputConfig);      /**< P11.1 */
#endif   
    HAL_GPIO_Pin_Init(PIN_TCON_BIST,&gtdPinOutputConfig);           /**< P12.3 */
    HAL_GPIO_Pin_Init(PIN_TCON_RESET,&gtdPinOutputConfig);          /**< P12.4 */
    HAL_GPIO_Pin_Init(PIN_984_PDB,&gtdPinOutputConfig);             /**< P13.0 */
    HAL_GPIO_Pin_Init(PIN_984_INTB_IN,&gtdPinOutputConfig);         /**< P13.3 */
    HAL_GPIO_Pin_Init(PIN_MAX25221_EN,&gtdPinOutputConfig);         /**< P13.6 */
    HAL_GPIO_Pin_Init(PIN_MAX20419_XWDI,&gtdPinOutputConfig);       /**< P14.0 */
    HAL_GPIO_Pin_Init(PIN_LM61460_EN,&gtdPinOutputConfig);          /**< P14.2 */
    HAL_GPIO_Pin_Init(PIN_25210_EN_R,&gtdPinOutputConfig);          /**< P18.0 */

    HAL_GPIO_Pin_Init(PIN_25240_EN_R,&gtdPinOutputConfig);           /**< P19.0 */

    HAL_GPIO_Pin_Init(PIN_MCU_PIN64_P17_0,&gtdPinOutputConfig);     /**< P17.0 */
    HAL_GPIO_Pin_Init(PIN_U301_EN_VBATT_SENSE,&gtdPinOutputConfig);     /**< P5.3*/
    HAL_GPIO_Pin_Init(PIN_20419_EN3,&gtdPinOutputConfig);           /**< P19.2 */
    HAL_GPIO_Pin_Init(PIN_20419_EN2,&gtdPinOutputConfig);           /**< P19.3 */
    HAL_GPIO_Pin_Init(PIN_20419_EN1,&gtdPinOutputConfig);           /**< P21.0 */

	(void)(u8Para);
	return  (u8Para);
}

uint8_t MPSC_Check_Power_Alive(uint8_t status, uint8_t times)
{
    uint8_t isAlive = NUM_ZERO;

    if (times == NUM_ZERO)
    {   /* times can not be zero */
        return false;
    }

    for( ; times>(uint8_t)NUM_ZERO; times--)
    {
        switch(status)
        {
            case PIN_LM61460_PG:
                (void)HAL_GPIO_PinRead(PIN_LM61460_PG,&isAlive);
                /* ucErrorPower &= ~(uint8_t)ERROR_PGOOD_LM61460; */
                break;
            case PIN_MAX20419_PG:
                (void)HAL_GPIO_PinRead(PIN_MAX20419_PG,&isAlive);
                /* ucErrorPower &= ~(uint8_t)ERROR_PGOOD_MAX20419; */
                break;
            case PIN_MAX25221_FLTB:
                (void)HAL_GPIO_PinRead(PIN_MAX25221_FLTB,&isAlive);
                /* ucErrorPower &= ~(uint8_t)ERROR_PGOOD_MAX25221_FLTB; */
                break;
            case PIN_PANEL_ABD:
                (void)HAL_GPIO_PinRead(PIN_PANEL_ABD,&isAlive);
                /* ucErrorPower &= ~(uint8_t)ERROR_PIN_PANEL_ABD; */
                break;
            case PIN_984_LOCK:
                (void)HAL_GPIO_PinRead(PIN_984_LOCK,&isAlive);
                /* ucErrorPower &= ~(uint8_t)ERROR_PIN_984_LOCK; */
                break;
            case PIN_25210_RESET_L:
                (void)HAL_GPIO_PinRead(PIN_25210_RESET_L,&isAlive);
                /* ucErrorPower &= ~(uint8_t)ERROR_PIN_25210_RESET_L; */
                break;
            case PIN_25210_RESET_R:
                (void)HAL_GPIO_PinRead(PIN_25210_RESET_R,&isAlive);
                /* ucErrorPower &= ~(uint8_t)ERROR_PIN_25210_RESET_R; */
                break;
            default:
                break;
        }

        if (isAlive == NUM_ONE)
        {   /* pin good! */
            return true;
        }
        else
        {
            Cy_SysLib_Delay(1); // 1ms
        }

    }

    return false;
}
/*---------------------------------------------------------------------------*/ 
