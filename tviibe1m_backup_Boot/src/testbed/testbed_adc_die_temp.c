/*

File    : testbed_adc_die_temp.c
Purpose : ADC Die temp calculation testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>

/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
#define TX_PIN_UART     IO_P0_1 /*EVB on board USB to UART*/  //IO_P13_1 
#define RX_PIN_UART     IO_P0_0                               //IO_P13_0
#define UART_SCB        UART_GROPU0                           //(3u)

#ifdef NDEBUG
#define DEBUG_PF(...)
#else
#define DEBUG_PF(...) HAL_UartPrintf(UART_SCB,##__VA_ARGS__)
#endif

void ADC_Calibration_Log_Callback (char* pDbgString)
{
    DEBUG_PF( pDbgString );
}

void testbed_adc_die_temp(void) 
{
    double temperatureData = 0.0;
    __enable_irq();
    
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
    
    /*Clear VT100 terminal screen */
    DEBUG_PF ("\033[1J" "\033[0;0H");

    DEBUG_PF(" < HAL ADC Caliibration testing >\n");
    
    /*開始 ADC Calibration , Calibration 結果自動寫入 offset / gain register*/
    CY_ASSERT ( HAL_ADC_Calibration ( ADC_Calibration_Log_Callback ) == DRIVER_TRUE );
    
    /*讀取 Die Tempterature 前一定要執行 HAL_ADC_Calibration*/
    CY_ASSERT ( HAL_ADC_DieTemperature_Get( &temperatureData ) == DRIVER_TRUE ) ;
    
    DEBUG_PF(" => Current temperature Data : %.3f degC\n",temperatureData);
    for (;;)
    {
        Cy_SysLib_Delay(20); 
    }
}