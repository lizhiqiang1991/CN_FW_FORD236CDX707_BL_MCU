/*

File    : testbed_crypto_crc.c
Purpose : CRC testbed

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

 
void testbed_crypto_crc(void) 
{
    HAL_All_IRQ_Enable();
    
    Init_EVB_Uart();
    DEBUG_PF("Testbed (HAL CRC Driver) \n");
    
    HAL_Crypto_Server_Enable(FUNC_ENABLE);

    HAL_CRC_Init(&gtdCRC_Config);
       
    uint8_t  CRC_src[]          = {0x12, 0x34, 0x56, 0x78 , 0x9a};
    uint32_t CRC_dataSize = sizeof(CRC_src) / sizeof(CRC_src[0]);
    uint32_t CRC_result;
    uint32_t* pSRC = (uint32_t*) CRC_src;
    
    /*CRC32 */
    HAL_CRC32_RUN(CRC_src,CRC_dataSize,(uint8_t*)&CRC_result);
    DEBUG_PF("CRC32 SRC(UINT32)=0x%x Result=0x%x\n",(*pSRC),CRC_result);
    
    /*CRC16-CCITT*/
    HAL_CRC16_RUN(CRC_src,CRC_dataSize,(uint8_t*)&CRC_result);
    DEBUG_PF("CRC16 SRC(UINT32)=0x%x Result=0x%x\n",(*pSRC),CRC_result);
    
    /*CRC8-CCITT*/
    HAL_CRC8_RUN(CRC_src,CRC_dataSize,(uint8_t*)&CRC_result);
    DEBUG_PF("CRC8 SRC(UINT32)=0x%x Result=0x%x\n",(*pSRC),CRC_result);
    
    for(;;);
}

