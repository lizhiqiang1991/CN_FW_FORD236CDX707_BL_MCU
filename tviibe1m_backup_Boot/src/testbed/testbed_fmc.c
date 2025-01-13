/*

File    : testbed_fmc.c
Purpose : Code/Working flash testbed

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

static void code_flash_testing(void)
{
    /** !!! Code flash 使用上注意 !!! 
        因為Code flash 最少只能寫 8 Bytes , 
        如果資料不滿 8 bytes 一樣以 8 Bytes 寫入, 剩餘補 '0'
        如果分批寫入要小心不要蓋到別的位址資料
     */
    #define CODE_DUMMY_SIZE_WORD  557  //任意長度不要超過 32K bytes --> 1, 2 ,73 , 517 , 557 , ... max (8192 WORDS = 32K bytes) 
    
    /** 
      此範例只有 Erase 1 個 (位址 0x10010000)  Large sector 所以最大只能寫入 32K bytes
      如果 Erase 2 個 Large sector 最大寫入64K bytes ... 依此類推 
    */
   
    /**(1) Fill dummy data*/
    static uint32_t u32FlashDummyData[CODE_DUMMY_SIZE_WORD];
    for (int i = 0; i < sizeof(u32FlashDummyData)/sizeof(u32FlashDummyData[0]) ; i++)
    {
        u32FlashDummyData[i] = i;
       // DEBUG_PF("u32FlashDummyData[%d] = %d; \n",i,i);
    }
    
    /**(2) Eease sector 0x10010000 (large sector #2) and enable blank verify */ 
    uint32_t u32SectorAddr = 0x10010000u;
    DEBUG_PF ("Erase sector (0x%x) ... ",u32SectorAddr);
    CY_ASSERT ( HAL_FMC_FlashEraseSector(u32SectorAddr,FUNC_ENABLE) == DRIVER_TRUE);
    DEBUG_PF ("Done \n");
    
    /**(3) Write Dummy data to main flash */
    DEBUG_PF ("Wrtie data to (0x%x) (%d bytes)... ",u32SectorAddr,sizeof(u32FlashDummyData));
    CY_ASSERT( HAL_FMC_FlashWrite((uint8_t*)u32FlashDummyData,u32SectorAddr, sizeof(u32FlashDummyData) /*Data length byte*/) == DRIVER_TRUE);
    DEBUG_PF ("Done \n");
     
    /**(4_1) Use POINTER to verify whether the data is correct */
    uint32_t* p_u32TestFlsTop = (uint32_t*)u32SectorAddr;
    DEBUG_PF ("(4_1) DATA Verify ...\n");
    for(uint32_t i = 0; i < CODE_DUMMY_SIZE_WORD; i++)
    {
        DEBUG_PF("Array[%d] = %d \n",i,p_u32TestFlsTop[i]); 
        CY_ASSERT( p_u32TestFlsTop[i] == i);
    }
    DEBUG_PF ("(4_1) Pass \n");
    
    /**(4_2) Use HAL_FMC_FlashRead to read */
    static uint32_t u32TempData[CODE_DUMMY_SIZE_WORD];
    CY_ASSERT( HAL_FMC_FlashRead((uint8_t*) u32TempData ,u32SectorAddr, sizeof(u32TempData) ) == DRIVER_TRUE);
    DEBUG_PF ("(4_2) DATA Verify ...\n");
    for(uint32_t i = 0; i < CODE_DUMMY_SIZE_WORD; i++)
    {
        DEBUG_PF("Array[%d] = %d \n",i,u32TempData[i]); 
        CY_ASSERT( u32TempData[i] == i);
    }
    DEBUG_PF ("(4_2) Pass \n");
}

static void eeprom_flash_testing(void)
{
    /** !!! Working flash 使用上注意 !!! 
    因為Working flash 最少只能寫 4 Bytes , 
    如果資料不滿 4 bytes 一樣以 4 Bytes 寫入, 剩餘補 '0'
    如果分批寫入要小心不要蓋到別的位址資料
    */
    typedef struct __attribute__(( packed ))
    {
        uint8_t u8Data;
        uint16_t u16Data;
        char strVerNum[ 16 ];
    } MyData;
    
    /** 範例寫入一塊資料結構*/
    MyData tdData;
    tdData.u8Data = 0xaa;
    tdData.u16Data = 0x4567;
    strcpy(tdData.strVerNum,"VER_0_12");
    
    /**(1) Eease sector 0x14000000 and enable blank verify */
    uint32_t u32SectorAddr = 0x14000000;
    DEBUG_PF ("Erase sector (0x%x) ... ",u32SectorAddr);
    CY_ASSERT ( HAL_FMC_EEPROMEraseSector(u32SectorAddr,FUNC_ENABLE) == DRIVER_TRUE);
    DEBUG_PF ("Done \n");
    
    /**(2) Write data to working flash*/
    DEBUG_PF ("Wrtie data to (0x%x) (%d bytes)... ",u32SectorAddr,sizeof(tdData));
    CY_ASSERT( HAL_FMC_EEPROMWrite((uint8_t*)&tdData,u32SectorAddr, sizeof(tdData) /*Data length byte*/) == DRIVER_TRUE);
    DEBUG_PF ("Done \n");
    
    /**(3_1) Use POINTER to verify whether the data is correct*/
    DEBUG_PF ("(3_1) DATA Verify ...\n");
    MyData* ptd = (MyData*) u32SectorAddr;
    DEBUG_PF ("MyData { u8Data = 0x%x , u16Data = 0x%x , strVerNum = %s}\n",ptd->u8Data,ptd->u16Data,ptd->strVerNum);
 
    /**(3_2) Use HAL_FMC_EEPROMRead to read */
    DEBUG_PF ("(3_1) DATA Verify ...\n");
    MyData tdTempData;
    CY_ASSERT( HAL_FMC_EEPROMRead ((uint8_t *) &tdTempData,u32SectorAddr,sizeof(tdTempData)) == DRIVER_TRUE);
    DEBUG_PF ("MyData { u8Data = 0x%x , u16Data = 0x%x , strVerNum = %s}\n",tdTempData.u8Data,tdTempData.u16Data,tdTempData.strVerNum);

    // /**(4) Erase working flash All*/
    // CY_ASSERT (HAL_FMC_EEPROMEraseAll(FUNC_ENABLE) == DRIVER_TRUE);
}

void testbed_fmc(void) 
{
    __enable_irq();
    
    Init_EVB_Uart();
    DEBUG_PF("Testbed (HAL FMC Driver) \n");
   
    /** Init Flash Driver */
    CY_ASSERT ( HAL_FMC_Initial(&gtdFMC_Config) == DRIVER_TRUE);
    
    DEBUG_PF("<Code flash testing> \n");
    code_flash_testing(); 
  
    DEBUG_PF("<EEPROM (Working flash) testing> \n"); 
    eeprom_flash_testing();
    
    DEBUG_PF("FMC Driver Deinit ... "); 
    Hal_FMC_Deinit();
    DEBUG_PF("Done \n"); 
    
    for(;;);
}

