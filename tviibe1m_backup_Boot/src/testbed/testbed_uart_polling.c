/*

File    : testbed_uart_polling.c
Purpose : uart testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
#define TX_PIN_UART     IO_P0_1 /*EVB on board USB to UART*/  //IO_P13_1 
#define RX_PIN_UART     IO_P0_0                               //IO_P13_0
#define UART_SCB        UART_GROPU0                           //(3u)

#ifdef NDEBUG
#define DEBUG_PF(...)
#else
#define DEBUG_PF(...) HAL_UartPrintf(UART_SCB,##__VA_ARGS__)
#endif

 /* vt100 command */
#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"

static const char* pString = "\n\nUART Polling testbed\n\n";

void testbed_uart_polling(void)
{
    bool bIsTxComplete;
    __enable_irq();
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
    
    /*Clear VT100 terminal screen */
    DEBUG_PF ("\033[1J" "\033[0;0H");
    
    /*Demo uart write : HAL_Uart_Write need to wait TxComplete*/
    HAL_Uart_Write(UART_SCB,(void*)pString,strlen(pString));
    do {   
        HAL_Uart_IsTxComplete(UART_SCB,&bIsTxComplete);
    } while(bIsTxComplete != true);
    
    /*Demo uart printf : No need to wait TxComplete*/
    DEBUG_PF ("<Printf Demo>\n\tInteger %d\n\tfloat %.3f\n\tstring %s\n\tchar %c\n\n",23 , 66.666,"Test String",'Q');
    
    /*Demo colored string*/
    DEBUG_PF("<Colored string Demo>\n");
    DEBUG_PF(YELLOW "\tShow yellow string Integer:%d float %f\n" NONE,123,88.8888);
    DEBUG_PF(RED "\tShow red string\n" NONE);
    DEBUG_PF("\tShow normal string\n");
    DEBUG_PF("Key in >\n");
    uint16_t u16NumInRx = 0u;
    //uint32_t u32TxStatus;
    
    for (;;)
    {
        uint8_t cTemp = 0x00u;
        HAL_Uart_GetNumInRxFifo(UART_SCB,&u16NumInRx);
        if (u16NumInRx != 0u)
        {
            HAL_Uart_Getc(UART_SCB,&cTemp);
            //HAL_Uart_GetTxStatus(UART_SCB,&u32TxStatus);
            //DEBUG_PF("Tx Status 0x%x\n",u32TxStatus & CY_SCB_UART_TX_UNDERFLOW);
            HAL_Uart_Putc(UART_SCB,cTemp);
            //HAL_Uart_GetTxStatus(UART_SCB,&u32TxStatus);
            //DEBUG_PF("Tx Status 0x%x\n",u32TxStatus & CY_SCB_UART_TX_DONE); 
        }
        Cy_SysLib_Delay(1u);
    }
}