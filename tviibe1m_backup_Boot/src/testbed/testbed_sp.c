/*

File    : testbed_sp.c
Purpose : Check main stack usgae testbed

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

static void Init_EVB_Uart(void)
{
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
    
    /*Clear VT100 terminal screen */
    DEBUG_PF ("\033[1J" "\033[0;0H");
}

void testbed_sp(void)
{
    __enable_irq();
    
    Init_EVB_Uart();
    
    /*Dummy data alloc in main stack*/
    uint8_t dummy[768];
    for (int i = 0; i < sizeof(dummy) ; i++)
        dummy[i] = 0;
    
    /* Get stack usage*/
    uint8_t u8Persent = 0;
    HAL_SP_Init(&gtdSPConfig);
    HAL_SP_Usage_Get(&gtdSPConfig,&u8Persent);
#if CY_CPU_CORTEX_M0P
    DEBUG_PF("CPU_M0+ Main Stack Usage: %d%%\n",u8Persent);
#else
    DEBUG_PF("CPU_M4 Main Stack Usage: %d%%\n",u8Persent);
#endif
    
    UNUSED(dummy);
    for (;;);
}