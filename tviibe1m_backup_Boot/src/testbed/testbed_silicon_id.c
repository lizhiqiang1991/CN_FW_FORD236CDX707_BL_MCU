/*

File    : testbed_silicon_id.c
Purpose : Reading silicon ID

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
#define TX_PIN_UART     KIT_UART_TX_PIN /*EVB on board USB to UART*/ 
#define RX_PIN_UART     KIT_UART_RX_PIN                             
#define UART_SCB        KIT_UART_SCB                          

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

void testbed_silicon_id(void)
{
    HAL_All_IRQ_Enable();
    
    Init_EVB_Uart();
    
    DEBUG_PF("Testbed Reading Silicon ID\n\n");
     
    silicon_info_t tsSiliconInfo;
    HAL_SiliconID_Get(&tsSiliconInfo);
    
    DEBUG_PF("======================================================\n");
     
    DEBUG_PF("Silicon: 0x%.4X, Family: 0x%.3x, Rev.: 0x%.2x\n", 
                tsSiliconInfo.u32SiliconID,
                tsSiliconInfo.u32Family,
                tsSiliconInfo.u32Rev);
    
    DEBUG_PF("SFlash version: %d\n",tsSiliconInfo.u32SFlash_Version);
    
    DEBUG_PF("Flash boot version %d.%d\n", 
                tsSiliconInfo.u32FlashBootMajorVersion,
                tsSiliconInfo.u32FlashBootMinorVersion);
    
    DEBUG_PF("SROM version %d.%d\n",
              tsSiliconInfo.u32SROM_FW_MajorVersion,
              tsSiliconInfo.u32SROM_FW_MinorVersion);
    DEBUG_PF("UUID ID0 0x%X\n", (uint32_t)((tsSiliconInfo.u64UUID>>32) & 0x00000000ffffffffu));
    DEBUG_PF("UUID ID1 0x%X\n", (uint32_t)(tsSiliconInfo.u64UUID & 0x00000000ffffffffu));
    
    DEBUG_PF("======================================================\n");
    
    for (;;);
}