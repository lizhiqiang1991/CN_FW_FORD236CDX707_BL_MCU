/*

File    : testbed_sysreset_cause.c
Purpose : get reset reason

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
#define TX_PIN_UART     IO_P0_1 /*EVB on board USB to UART*/ 
#define RX_PIN_UART     IO_P0_0                             
#define UART_SCB        UART_GROPU0                         

#define USER_BUTTON         KIT_BUTTON_PIN  /*IO_P7_0*/

#ifdef NDEBUG
#define DEBUG_PF(...)
#else
#define DEBUG_PF(...) HAL_UartPrintf(UART_SCB,##__VA_ARGS__)
// or 
//#define DEBUG_PF(f_, ...) printf((f_), ##__VA_ARGS__)
#endif

static void Init_EVB_Uart(void)
{
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
    
    /*Clear VT100 terminal screen */
    DEBUG_PF ("\033[1J" "\033[0;0H");
}

void testbed_sys_reset_cause(void)
{
    HAL_All_IRQ_Enable();
    
    Init_EVB_Uart();
    /*Button GPIO config as input*/
    HAL_GPIO_Pin_Init(USER_BUTTON,&gtdPinInputConfig);
  
    DEBUG_PF("testbed_sys_reset_cause\n");
    
    uint32_t u32Reason;
    uint8_t  u8Button = 0;
    /* Read the RESET reason */
    CY_ASSERT( HAL_RES_CAUSE_GET(&u32Reason) == DRIVER_TRUE );
    
    DEBUG_PF("Cause Register = 0x%x\n{\n", u32Reason);
    
    if (( u32Reason & SYSRESET_PORVDDD) == SYSRESET_PORVDDD)
    {
        DEBUG_PF("POWER ON RESET\n");
    }
    
    if (( u32Reason & SYSRESET_SOFT ) == SYSRESET_SOFT )
    {
        DEBUG_PF("SOFTWARE RESET\n");
    }
    
    if( ( u32Reason & SYSRESET_XRES ) == SYSRESET_XRES )
    {
        DEBUG_PF("XRES\n");
    }
    
    DEBUG_PF("}\n");
    
    for (;;)
    {  
        HAL_GPIO_PinRead(USER_BUTTON,&u8Button); 
        if (0U == u8Button)
        {
            Cy_SysLib_Delay(100); /*Delay 100 msec , simple button debouncing*/
            HAL_GPIO_PinRead(USER_BUTTON,&u8Button);
        }
        else
        {
            Cy_SysLib_Delay(10);
            continue;
        }
        
        if (0U == u8Button /* Button Pressed*/) 
        {
            /*Trigger software reset*/
            NVIC_SystemReset();
        }
    }
}