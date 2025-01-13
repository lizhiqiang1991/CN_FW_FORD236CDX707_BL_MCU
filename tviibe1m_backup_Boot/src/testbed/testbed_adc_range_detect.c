/*

File    : testbed_adc_range_detect.c
Purpose : ADC testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>

/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
#define TX_PIN_UART     IO_P0_1 /*EVB on board USB to UART*/ 
#define RX_PIN_UART     IO_P0_0                             
#define UART_SCB        UART_GROPU0                         

#ifdef NDEBUG
#define DEBUG_PF(...)
#else
#define DEBUG_PF(...) HAL_UartPrintf(UART_SCB,##__VA_ARGS__)
#endif


const char* strDetectMode[] = 
{
    "DETECTION MODE : BELOW_LO     ",  /**< Below Low threshold (result < Lo) */
    "DETECTION MODE : INSIDE_RANGE ",  /**< Inside range (Lo <= result < Hi) */
    "DETECTION MODE : ABOVE_HI     ",  /**< Above high threshold (Hi <= result) */
    "DETECTION MODE : OUTSIDE_RANGE",  /**< Outside range (result < Lo || Hi <= result) */
};

/*
ADC_RANGE_DETECTION_MODE_BELOW_LO         IF below    , rangeIntr = 1
ADC_RANGE_DETECTION_MODE_INSIDE_RANGE     IF inside   , rangeIntr = 1
ADC_RANGE_DETECTION_MODE_ABOVE_HI         IF above_hi , rangeIntr = 1

ADC_RANGE_DETECTION_MODE_OUTSIDE_RANGE    IF ADC_VALUE > HI_THRESHOLD , aboveHi = 1 , rangeIntr = 1 
                                          IF ADC_VALUE < LO_THRESHOLD , aboveHi = 0 , rangeIntr = 1
                                          OTHERWISE aboveHi = 0 , rangeIntr = 0
*/
#define DET_MODE    ADC_RANGE_DETECTION_MODE_INSIDE_RANGE

#define LO_THRESHOLD  1000u
#define HI_THRESHOLD  2000u

static void ADC0_Callback (bool u8GroupDone)
{
    static uint32_t  u32Index = 0;
    uint16_t         u16Result = 0u;
    adc_ch_stattus_t statusBuff ;
    if (true == u8GroupDone)
    {
        /* Get the result(s) */
        HAL_ADC_Channel_Result_Get (ADC0_LOGIC_CH0,&u16Result, &statusBuff);
        
        /*
         ADC_RANGE_DETECTION_MODE_BELOW_LO         IF below    , rangeIntr = 1
         ADC_RANGE_DETECTION_MODE_INSIDE_RANGE     IF inside   , rangeIntr = 1
         ADC_RANGE_DETECTION_MODE_ABOVE_HI         IF above_hi , rangeIntr = 1
         
         ADC_RANGE_DETECTION_MODE_OUTSIDE_RANGE    
                                                   IF ADC_VALUE > HI_THRESHOLD , aboveHi = 1 , rangeIntr = 1 
                                                   IF ADC_VALUE < LO_THRESHOLD , aboveHi = 0 , rangeIntr = 1
                                                   OTHERWISE aboveHi = 0 , rangeIntr = 0
        */
        
        if (true == statusBuff.valid)
        {
            float v = (5.0 *((float)u16Result / 4095.0));
            
            /* Display ADC result */
            DEBUG_PF("(%04d) %04d %.3fV (aboveHi %d ,rangeIntr %d)\n",
                     u32Index, 
                     u16Result,
                     v,
                     statusBuff.aboveHi,
                     statusBuff.rangeIntr);
            /* Move cursor to previous line */
            DEBUG_PF("\x1b[1F");
        }
        u32Index++;
    }
}

/**
    注意!!!
    如果要使用 Range detection 功能必需使用 ADC 中斷
*/

void testbed_adc_range_detect(void) 
{
    __enable_irq();
    
    gts_UART_115200_bps_Config.u8TxPin = TX_PIN_UART;
    gts_UART_115200_bps_Config.u8RxPin = RX_PIN_UART;
    
    CY_ASSERT( HAL_Uart_Init(UART_SCB,&gts_UART_115200_bps_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_Uart_Active(UART_SCB,FUNC_ENABLE) == DRIVER_TRUE);
    
    /*Clear VT100 terminal screen */
    DEBUG_PF ("\033[1J" "\033[0;0H");
    
    DEBUG_PF("HAL ADC Range detect (ADC0) testing\n");
     
    adc_channel_config_t tsAN0_Config = 
    {
        .u8PinAddress         = ADC_PIN_AN0, /** channel 0*/
        .u8IsGroupEnd         = true,        /**< This value indicates that this channel is the last channel of a group or not */
        .u8TriggerSelection   = ADC_TRIGGER_OFF,
        .u8SignExtention      = ADC_SIGN_EXTENTION_UNSIGNED, /** Select whether result data is signed or unsigned. */
        .u8RangeDetectionMode = DET_MODE, /**< Select range detection mode*/
        .u8PreconditionMode   = ADC_PRECONDITION_MODE_OFF,           /**< Select preconditioning mode.*/
        .u8PostProcessingMode = ADC_POST_PROCESSING_MODE_RANGE,     /**< 打開 POST PROCESSING MODE 預設關閉 */
        .u8AverageCount       = 0u, 
        .u8RightShift         = 0u,
        .u16RangeDetectionLoThreshold = LO_THRESHOLD, /**< Range detect low threshold (Lo) */
        .u16RangeDetectionHiThreshold = HI_THRESHOLD, /**< Range detect high threshold (Hi) */
     };
 
    /* Initialize ADC0 Group*/
    CY_ASSERT( HAL_ADC_Init(ADC_GROPU0,&gtdADC_Config) == DRIVER_TRUE);
    
    /* Initialize ADC Ch (ADC0_LOGIC_CH0) and Enable */
    /* 指定 ADC0 腳位 ADC_PIN_AN0 到 ADC0 [LOGIC CHANNEL 0 ]*/
    CY_ASSERT( HAL_ADC_Channel_Init   (ADC0_LOGIC_CH0, &tsAN0_Config) == DRIVER_TRUE);
    CY_ASSERT( HAL_ADC_Channel_Active (ADC0_LOGIC_CH0, FUNC_ENABLE) == DRIVER_TRUE);
  
     /* Register adc interrupt*/
    CY_ASSERT( HAL_ADC_Callback_Register (ADC0_LOGIC_CH0, ADC0_Callback) == DRIVER_TRUE);
    
    DEBUG_PF("%s\n", strDetectMode[tsAN0_Config.u8RangeDetectionMode]);
    DEBUG_PF("Lo Threshold = %d\nHi Threshold = %d\n",
             tsAN0_Config.u16RangeDetectionLoThreshold,
             tsAN0_Config.u16RangeDetectionHiThreshold);
    
    /* Issue first SW trigger A/D conversion */
    //CY_ASSERT( HAL_ADC_Channel_SW_Trigger(ADC0_LOGIC_CH0) == DRIVER_TRUE);  
     
    for (;;)
    { 
        /* Trigger next conversion */
        /** 如果設定 ADC_TRIGGER_CONTINUOUS 不需要每次執行 HAL_ADC_Channel_SW_Trigger*/
        //if (ADC_TRIGGER_OFF == tsAN0_Config.u8TriggerSelection)
            HAL_ADC_Channel_SW_Trigger(ADC0_LOGIC_CH0);
        
        /*如果設定 ADC_TRIGGER_OFF 每 1.2 秒 SW Trigger 一次進行 ADC 取樣*/
        Cy_SysLib_Delay(1200);
    }
}

