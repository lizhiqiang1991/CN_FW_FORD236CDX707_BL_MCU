/******************************************************************************
;       Program		: main.c 
;       Function	: The Main Program and State Machine
;       Chip		: Cypress CYT2B6
;       Clock		:
;       Date		: 2021 / 10 / 08
;       Author		: Joel Wang
;       Describe 	: 
******************************************************************************/
/*---------------------------- Include File ---------------------------------*/
#include "app_main.h"
#include "InitApp.h"
//__no_init volatile uint16_t u16UpdateKey @0x08001500;
/*---------------------------- Declare Global Variable ----------------------*/
//static un_RES_CAUSE_t Res_Cause;
/*---------------------------- Start Program --------------------------------*/
/******************************************************************************  
;       Function Name			:	int main(void)
;       Function Description	:	Main function.
;       Parameters				:	void
;       Return Values			:	NA
;		Source ID				:
******************************************************************************/ 
void app_main(void)
{
	(void)InitApp_Startup(NOTHING);
#if DEBUG_UART                         
	DEBUG_PF("u16UpdateKey=%x\r\n",u16UpdateKey);
	DEBUG_PF("********reset mode=%x************\r\n",CYREG_RES_CAUSE->stcField);
#endif	
	while(true)
	{
		switch(InitApp_StateMachine_Get())
		{
			case BL_STARTUP:
				//(void)InitApp_Startup(NOTHING);
				(void)FunApp_Startup_Check(NOTHING);
			break;
                        case BL_INIT:
                           (void)InitApp_All(NOTHING);
                            break;
			case BL_UPDATE:
                            I2c0SlaveApp_Flash_Operate_Process();
                            break;
			default:
				;
			break;
		}
	}
}
/*---------------------------------------------------------------------------*/ 
