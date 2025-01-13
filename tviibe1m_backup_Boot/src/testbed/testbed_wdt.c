/*

File    : testbed_wdt.c
Purpose : Watch dog testbed

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

#ifdef NDEBUG
#define DEBUG_PF(...)
#else
#define DEBUG_PF(...) HAL_UartPrintf(UART_SCB,##__VA_ARGS__)
// or 
//#define DEBUG_PF(f_, ...) printf((f_), ##__VA_ARGS__)
#endif

/*Helper function*/
#define HAL_WDT_START() HAL_WDT_Active(FUNC_ENABLE)
#define HAL_WDT_STOP()  HAL_WDT_Active(FUNC_DISABLE)

static void Init_EVB_Uart(void)
{
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
    
    /*Clear VT100 terminal screen */
    DEBUG_PF ("\033[1J" "\033[0;0H");
}

/***
!!! 注意 WDT 範例不能進 DEBUG , 
如果要進 DEBUG 需 呼叫 Cy_WDT_SetDebugRun(CY_WDT_ENABLE); 
但是 timeout 後 mcu 不會 reset 
*/
void testbed_wdt(void)
{
    uint32_t u32ResetReason;
    
    HAL_All_IRQ_Enable();
    
    Init_EVB_Uart();
    DEBUG_PF("testbed_wdt\n");
    
    /* Read the RESET reason */
    CY_ASSERT( HAL_RES_CAUSE_GET(&u32ResetReason) == DRIVER_TRUE );
    if ((SYSRESET_WDT & u32ResetReason) == 1u)
    {
        DEBUG_PF(">>>Reset by WDT\n");
    }
    
    DEBUG_PF("WDT Start\n");    
    CY_ASSERT( HAL_WDT_Init(&gts_WDT_Config) == DRIVER_TRUE );
    /*
    如果要進 DEBUG 需 呼叫 Cy_WDT_SetDebugRun(CY_WDT_ENABLE); 
    但是 timeout 後 mcu 不會 reset 
    */
    // Cy_WDT_SetDebugRun(CY_WDT_ENABLE);
    
    CY_ASSERT( HAL_WDT_START() == DRIVER_TRUE);
 
#define CLEAR_WDT    (1)
    
    for (;;)
    {
        DEBUG_PF("WDT Count %d\n",Cy_WDT_GetCount());
        /* Move cursor to previous line */
        DEBUG_PF("\x1b[1F");
        Cy_SysLib_Delay(100u);
#if CLEAR_WDT == 1u    
        /* When testing WDT reset, this CLEAR_WDT needs to be 0. */
        HAL_WDT_Clear();
#endif        
    }
}

