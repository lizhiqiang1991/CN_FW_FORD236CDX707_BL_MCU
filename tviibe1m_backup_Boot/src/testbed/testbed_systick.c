/*

File    : testbed_sp.c
Purpose : Check main stack usgae testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

#define USER_LED        KIT_LED_1_BLUE

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

static uint8_t u8Count = 0;
static uint8_t u8StopPrint = false;
static void tick_isr (void* pContext)
{   
    uint32_t* u32Data = (uint32_t*)pContext;
    if (u8Count >= 10)
    {
        HAL_GPIO_PinToggle(USER_LED);
        if (false == u8StopPrint)
        {
            uint32_t u32Tick = 0;
            HAL_Systick_Counter_Get(&u32Tick);
            DEBUG_PF("(1)tick isr 0x%x , tick count = %d \n",*u32Data,u32Tick);
            HAL_Systick_Counter_Get(&u32Tick);
            DEBUG_PF("(2)tick isr 0x%x , tick count = %d \n",*u32Data,u32Tick);
        }
        u8Count = 0;
    }
    else
    {
       
        u8Count++;
    }
}

void testbed_systick(void)
{
    static uint32_t u32MyDummyData = 0x1234;
    
    systick_config_t tdTickConfig = 
    {
        .u32ReloadCounter  = 8000000u,  /*m0 , cpu clock 80M , 100msec = 8000000u*/
        .u32Counter        = 8000000u,
        .u8ClockSource     = SYSTICK_CLOCK_SOURCE_CLK_CPU,
        .u8InterruptEnable = FUNC_ENABLE, /*FUNC_DISABLE,*/
        .u8NvicPriority    = 3u,
    };
    
    __enable_irq();
    Init_EVB_Uart();
    /*LED GPIO config as output*/
    HAL_GPIO_Pin_Init(USER_LED,&gtdPinOutputConfig);
  
    /** 測試 (1) 正常操作 Enable interrupt 註冊 Callback LED 閃 6 秒*/
    CY_ASSERT (HAL_Systick_Init(&tdTickConfig) == DRIVER_TRUE);
    HAL_Systick_Callback_Register (tick_isr, &u32MyDummyData);
    HAL_Systick_Active(FUNC_ENABLE);
    
    /* Delay 6 second */
    Cy_SysLib_Delay(6000); 
    
    /** 測試 (2) 重新 Reload 加快 LED 速度 閃 6 秒*/ 
    DEBUG_PF("LED flashing fast\n");
    HAL_Systick_Active(FUNC_DISABLE); /*Stop tick*/
    u8StopPrint = true;
    u8Count = 0;
    HAL_Systick_Counter_Clear();
    HAL_Systick_Reload_Set(800000u);
    /*Restart tick*/
    HAL_Systick_Active(FUNC_ENABLE);
    
    /* Delay 6 second */
    Cy_SysLib_Delay(6000);
    
    /** 測試 (3) 關閉中斷直接讀取 TICK COUNT*/   
    uint32_t u32CurTickCount = 0;
    HAL_Systick_Active(FUNC_DISABLE);
    HAL_Systick_Interrupt_Set(FUNC_DISABLE,3u); /*Stop SysTick interrupt*/
    HAL_GPIO_PinWrite(USER_LED,PIN_LOW);
    HAL_Systick_Counter_Clear();
    HAL_Systick_Reload_Set(800000u);
    HAL_Systick_Active(FUNC_ENABLE);
    Cy_SysLib_DelayCycles(100000);
    HAL_Systick_Counter_Get(&u32CurTickCount);
    DEBUG_PF("Cy_SysLib_DelayCycles(100000) = %d \n",800000u - u32CurTickCount);
    
    /** 測試 (4) 關閉中斷 重新 RELOAD 等SYSTICK FLAG */
    /** !! 注意當 Systick 中斷打開時 HAL_Systick_Flag_Get 讀不到狀態, 狀態被 systick interrupt 清掉 */
    /**  The count flag is set, once the SysTick counter reaches zero.
	  The flag is cleared on read. */
    /* Wait for systick flag (systick interrupt must disable)*/
    uint32_t u32Flag;
    HAL_Systick_Active(FUNC_DISABLE);
    HAL_Systick_Counter_Clear();
    HAL_Systick_Reload_Set(66666u);
    HAL_Systick_Active(FUNC_ENABLE);
    do
    {
        HAL_Systick_Flag_Get(&u32Flag);
    } while(u32Flag == 0);
    DEBUG_PF("Flag get\n");
    
    for (;;)
    {
        HAL_Systick_DelayUs(100u);
    }
}

