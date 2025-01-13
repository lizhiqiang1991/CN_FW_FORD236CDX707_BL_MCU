/***************************************************************************
*\file hal_adc_calibration.c 
****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_adc_def.h"
#include "hal_adc.h"

#define DOUBLE_CHECK_VREF_H_VREEF_L   
/* #define DISPLAY_DETAIL_LOG */


/** 
 * Macro definition for SFLASH trim
 */

/** Select the analaog power rails as per your hardware */
#define USE_TRIM_FOR_3P3V_VDDA                  0
#define USE_TRIM_FOR_5P0V_VDDA                  1
#define USE_TEMPERATURE_TRIM_VALUES             USE_TRIM_FOR_3P3V_VDDA

#if (USE_TEMPERATURE_TRIM_VALUES == USE_TRIM_FOR_3P3V_VDDA)
    #pragma message("hal_adc_calc_die_temp.c \"USE_TRIM_FOR_3P3V_VDDA\" ")
#else
    #pragma message("hal_adc_calc_die_temp.c \"USE_TRIM_FOR_5P0V_VDDA\" ")
#endif

/** Read from the SFLASH table */
#if (USE_TEMPERATURE_TRIM_VALUES == USE_TRIM_FOR_3P3V_VDDA)
    /** SFLASH table for VDDA - 3.3V */
    #define EPASS_TEMP_TRIM_TEMP_COLDSORT       0x17000654ul
    #define EPASS_TEMP_TRIM_TEMP_ROOMSORT       0x1700064Eul
    #define EPASS_TEMP_TRIM_TEMP_HOTCLASS       0x1700065Aul
    #define EPASS_TEMP_TRIM_DIODE_COLDSORT      0x17000656ul
    #define EPASS_TEMP_TRIM_DIODE_ROOMSORT      0x17000650ul
    #define EPASS_TEMP_TRIM_DIODE_HOTCLASS      0x1700065Cul
    #define EPASS_TEMP_TRIM_VBG_COLDSORT        0x17000658ul
    #define EPASS_TEMP_TRIM_VBG_ROOMSORT        0x17000652ul
    #define EPASS_TEMP_TRIM_VBG_HOTCLASS        0x1700065Eul
#else
    /** SFLASH table for VDDA - 5V */
    #define EPASS_TEMP_TRIM_TEMP_COLDSORT       0x17000654ul
    #define EPASS_TEMP_TRIM_TEMP_ROOMSORT       0x1700064Eul
    #define EPASS_TEMP_TRIM_TEMP_HOTCLASS       0x1700065Aul
    #define EPASS_TEMP_TRIM_DIODE_COLDSORT      0x1700066Eul
    #define EPASS_TEMP_TRIM_DIODE_ROOMSORT      0x1700066Aul
    #define EPASS_TEMP_TRIM_DIODE_HOTCLASS      0x17000672ul
    #define EPASS_TEMP_TRIM_VBG_COLDSORT        0x17000670ul
    #define EPASS_TEMP_TRIM_VBG_ROOMSORT        0x1700066Cul
    #define EPASS_TEMP_TRIM_VBG_HOTCLASS        0x17000674ul
#endif  /* (USE_TEMPERATURE_TRIM_VALUES == USE_TRIM_FOR_3P3V_VDDA) */

/** SFLASH table read macro */
#define GET_SFLASH_VALUE(x)                     CY_GET_REG16(x)
#define GET_TRIM_VALUE(x)                       (GET_SFLASH_VALUE(x) & 0xFFFFu)

/** Temperature trim values read from SFLASH */
#define TEMP_TRIM_TEMP_ROOMSORT                 GET_TRIM_VALUE(EPASS_TEMP_TRIM_TEMP_ROOMSORT)
#define TEMP_TRIM_TEMP_COLDSORT                 GET_TRIM_VALUE(EPASS_TEMP_TRIM_TEMP_COLDSORT)
#define TEMP_TRIM_TEMP_HOTCLASS                 GET_TRIM_VALUE(EPASS_TEMP_TRIM_TEMP_HOTCLASS)

#define TEMP_TRIM_DIODE_ROOMSORT                GET_TRIM_VALUE(EPASS_TEMP_TRIM_DIODE_ROOMSORT)
#define TEMP_TRIM_DIODE_COLDSORT                GET_TRIM_VALUE(EPASS_TEMP_TRIM_DIODE_COLDSORT)
#define TEMP_TRIM_DIODE_HOTCLASS                GET_TRIM_VALUE(EPASS_TEMP_TRIM_DIODE_HOTCLASS)

#define TEMP_TRIM_VBG_ROOMSORT                  GET_TRIM_VALUE(EPASS_TEMP_TRIM_VBG_ROOMSORT)
#define TEMP_TRIM_VBG_COLDSORT                  GET_TRIM_VALUE(EPASS_TEMP_TRIM_VBG_COLDSORT)
#define TEMP_TRIM_VBG_HOTCLASS                  GET_TRIM_VALUE(EPASS_TEMP_TRIM_VBG_HOTCLASS)

/*MAX Log String length */
#define MAX_CALIBRATIN_LOG_LEN   (96u)

/** ADC calibration iteration macro */
#define ADC_CAL_GAIN_MAX_VALUE                  (15)
#define ADC_CAL_GAIN_MIN_VALUE                  (-16)
#define ADC_CAL_OFFSET_MAX_VALUE                (127)
#define ADC_CAL_OFFSET_MIN_VALUE                (-128)

/** ADC calibration conversion limiting macro */
#define ADC_VFERL_OFFSET_TRANSIT_VALUE          (1)               // initial value --> 0 
#define ADC_VFERH_OFFSET_TRANSIT_VALUE          (4094)            // initial value --> 4095
#define ADC_VFERL_GAIN_TRANSIT_VALUE            (1)               // initial value --> 0

/** ADC logical channel to be used */
#define ADC_LOGICAL_CHANNEL                     (0)

/** Number of iteration to get the ADC readings */
#define ADC_CAL_NUM_OF_ITERATION                (8)

/** 
 * \var static bool isConversionComplete
 * \brief ADC conversion complete flag
 */
static bool isConversionComplete = false;

/** 
 * \var static uint16_t resultBuff
 * \brief ADC conversion result buffer place holder
 */
static uint16_t resultBuff[ADC_CAL_NUM_OF_ITERATION];

/** 
 * \var static cy_stc_adc_ch_status_t statusBuff
 * \brief ADC conversion status buffer place holder
 */
static cy_stc_adc_ch_status_t  statusBuff[ADC_CAL_NUM_OF_ITERATION];


/** ADC Calibration ********************************************************************************/
static HAL_ADC_Dbg_Calibration pCalibrationDbg;
 
static cy_stc_adc_channel_config_t tsAdcCalibrationChannelConfig = 
{
    .triggerSelection          = CY_ADC_TRIGGER_OFF,
    .channelPriority           = 0u,
    .preenptionType            = CY_ADC_PREEMPTION_FINISH_RESUME,
    .isGroupEnd                = true, //
    .doneLevel                 = CY_ADC_DONE_LEVEL_PULSE,
    .pinAddress                = CY_ADC_PIN_ADDRESS_VREF_L,
    .portAddress               = CY_ADC_PORT_ADDRESS_SARMUX0,
    .extMuxSelect              = 0u,
    .extMuxEnable              = true,
    .preconditionMode          = CY_ADC_PRECONDITION_MODE_OFF,
    .overlapDiagMode           = CY_ADC_OVERLAP_DIAG_MODE_OFF,
    .sampleTime                = 120u,
    .calibrationValueSelect    = CY_ADC_CALIBRATION_VALUE_ALTERNATE,//
    .postProcessingMode        = CY_ADC_POST_PROCESSING_MODE_NONE,
    .resultAlignment           = CY_ADC_RESULT_ALIGNMENT_RIGHT,
    .signExtention             = CY_ADC_SIGN_EXTENTION_UNSIGNED,
    .averageCount              = 16u,
    .rightShift                = 0u,
    .rangeDetectionMode        = CY_ADC_RANGE_DETECTION_MODE_INSIDE_RANGE,
    .rangeDetectionLoThreshold = 0x0000u,
    .rangeDetectionHiThreshold = 0x0FFFu,
    .mask.grpDone              = true,
    .mask.grpCancelled         = false,
    .mask.grpOverflow          = false,
    .mask.chRange              = false,
    .mask.chPulse              = false,
    .mask.chOverflow           = false,
};

static char StrBuf[MAX_CALIBRATIN_LOG_LEN];
 
static void ADC_CalibrationLog(void *fmt, ...)
{   
    va_list arg;
    if (NULL != pCalibrationDbg)
    {
        va_start(arg, fmt);
        vsnprintf (StrBuf, MAX_CALIBRATIN_LOG_LEN, (const char *) fmt, arg );
        va_end(arg);
        (*pCalibrationDbg) (StrBuf);
    }
}

static void ADCx_Ch0_Callback (bool u8GroupDone)
{
    if (true == u8GroupDone)
    {
        /* Get the result(s) */
        Cy_Adc_Channel_GetResult(&PASS0_SAR0->CH[ADC_LOGICAL_CHANNEL], &resultBuff[0], &statusBuff[0]);
        isConversionComplete = true;
    }
}

static void ADC_ConfigureChannel(cy_en_adc_pin_address_t channelAddress)
{
    /* Configure the ADC channel to requested address */
    tsAdcCalibrationChannelConfig.pinAddress = channelAddress;
    Cy_Adc_Channel_Init(&PASS0_SAR0->CH[ADC_LOGICAL_CHANNEL], &tsAdcCalibrationChannelConfig);

    /* Enable ADC ch. */
    Cy_Adc_Channel_Enable(&PASS0_SAR0->CH[ADC_LOGICAL_CHANNEL]);
}

static uint16_t ADC_ReadChannelData(cy_en_adc_pin_address_t channelAddress)
{
    uint32_t avgAdcValue = 0ul;

    ADC_ConfigureChannel(channelAddress);
    for(uint8_t i = 0u; i < ADC_CAL_NUM_OF_ITERATION; i++)
    {
        Cy_Adc_Channel_SoftwareTrigger(&PASS0_SAR0->CH[ADC_LOGICAL_CHANNEL]);
        while(isConversionComplete != true);
        isConversionComplete = false;
        avgAdcValue += resultBuff[0];
        /*ADC_CalibrationLog(" Read ADC value : %d \n\r", resultBuff[0]);*/
    }

    return (avgAdcValue/ADC_CAL_NUM_OF_ITERATION);
}

static int16_t ADC_Calibration_Vref_L(volatile stc_PASS_SAR_t * base)
{
    bool isCalibrationDone = false;
    int32_t currentAdcAvgValue=0xFF, previousAdcAvgValue=0xFF;
    cy_stc_adc_analog_calibration_conifg_t tempCalValue={0};
    int16_t vreflOffset=0;
    
    /* Configure the ADC channel to VREFL */
    ADC_CalibrationLog("\n Configuring VREF_L channel \n\r");
    
    int8_t offset = ADC_CAL_OFFSET_MAX_VALUE;
    /* Perform offset calibration operation on Vref_L */
    for(uint16_t i = 0 ; i < (256u) ; i++)
    {
        /* Update the offset value in ALT_CAL */
        tempCalValue.offset = offset;
        Cy_Adc_SetAltAnalogCalibrationValue(base, &tempCalValue);
        
        isConversionComplete = false;
        currentAdcAvgValue = 0;
        
        /* take the average value from adc */
        currentAdcAvgValue = ADC_ReadChannelData(CY_ADC_PIN_ADDRESS_VREF_L);
        
#ifdef DISPLAY_DETAIL_LOG        
        ADC_CalibrationLog(" Set offset value : %d ", tempCalValue.offset);
        ADC_CalibrationLog(" Current Avg Value : %d \n\r", currentAdcAvgValue);
#endif
        /* check for the ADC reading changes from 0x001 -> 0x000 */
        if((currentAdcAvgValue < ADC_VFERL_OFFSET_TRANSIT_VALUE) && 
           (previousAdcAvgValue == ADC_VFERL_OFFSET_TRANSIT_VALUE) && 
           (previousAdcAvgValue != 0x00))
        {
            isCalibrationDone = true;
            break;
        }
        else
        {
            previousAdcAvgValue = currentAdcAvgValue;
        }
        offset--;
    }
    
    /* If calibration is done update the vreflOffset value */
    if(isCalibrationDone)
    {
        vreflOffset = tempCalValue.offset;
        ADC_CalibrationLog(" First step calibration done.");
        ADC_CalibrationLog(" vreflOffset : %d \n\r", vreflOffset);
    }
    else
    {
        if ((currentAdcAvgValue == 0) && (previousAdcAvgValue == 0))
        {
            vreflOffset = 0x00u;
            ADC_CalibrationLog(" First step calibration done.");
            ADC_CalibrationLog(" vreflOffset : %d \n\r", vreflOffset);
        }
        else
        {
            vreflOffset = 0xFFu;
            ADC_CalibrationLog(" Error : Not able to find the calibration value for vreflOffset. \n\r");
        }
    }
    return vreflOffset;
}

static int16_t ADC_Calibration_Vref_H(volatile stc_PASS_SAR_t * base)
{
    bool isCalibrationDone = false;
    int32_t currentAdcAvgValue=0xFF, previousAdcAvgValue=0xFF;
    cy_stc_adc_analog_calibration_conifg_t tempCalValue={0};
    int16_t vrefhOffset=0;
    
    /* Configure the ADC channel to VREFH */
    ADC_CalibrationLog("\n Configuring VREF_H channel \n\r");
    
    int8_t offset = ADC_CAL_OFFSET_MIN_VALUE;
    /* Perform offset calibration operation on Vref_H */
    for (uint16_t i = 0 ; i < (256u) ; i++)
    {
        /* Update the offset value in ALT_CAL */
        tempCalValue.offset = offset;
        Cy_Adc_SetAltAnalogCalibrationValue(base, &tempCalValue);
        
        isConversionComplete = false;
        currentAdcAvgValue = 0;
        
        /* take the average value from adc */
        currentAdcAvgValue = ADC_ReadChannelData(CY_ADC_PIN_ADDRESS_VREF_H);

#ifdef DISPLAY_DETAIL_LOG         
        ADC_CalibrationLog(" Set offset value : %d ", tempCalValue.offset);
        ADC_CalibrationLog(" Current Avg Value : %d \n\r", currentAdcAvgValue);
#endif
        
        /* check for the ADC reading changes from 0xFFE -> 0xFFF */
        if((currentAdcAvgValue > ADC_VFERH_OFFSET_TRANSIT_VALUE) && (previousAdcAvgValue == ADC_VFERH_OFFSET_TRANSIT_VALUE))
        {         
            isCalibrationDone = true;
            break;
        }
        else
        {
            previousAdcAvgValue = currentAdcAvgValue;
        }
        offset ++;
    }
    
    /* If calibration is done update the vrefhOffset value */
    if(isCalibrationDone)
    {
        vrefhOffset = tempCalValue.offset;
        ADC_CalibrationLog(" Second step calibration done.");
        ADC_CalibrationLog(" vrefhOffset : %d \n\r", vrefhOffset);
    }
    else
    {
        if ((currentAdcAvgValue == 0xFFF) && (previousAdcAvgValue == 0xFFF))
        {
            vrefhOffset = 0x00;
            ADC_CalibrationLog(" Second step calibration done.");
            ADC_CalibrationLog(" vrefhOffset : %d \n\r", vrefhOffset);
        }
        else
        {
            vrefhOffset = 0xFF;
            ADC_CalibrationLog(" Error : Not able to find the calibration value for vrefhOffset. \n\r");
        }
    }
    return vrefhOffset;
}


static int16_t ADC_Calibration_Gain(volatile stc_PASS_SAR_t * base, int16_t finalOffset)
{
    bool isCalibrationDone = false;
    int32_t currentAdcAvgValue=0xFF, previousAdcAvgValue=0xFF;
    cy_stc_adc_analog_calibration_conifg_t tempCalValue={0};
    int16_t vreflGain=0;
    
    ADC_CalibrationLog("\n\r ------- ADC gain calibration -------\n\r");
    
    /* Configure the ADC channel to VREFL */
    ADC_CalibrationLog("\n Configuring VREF_L channel \n\r");
    
    /* Perform gain calibration operation on Vref_L */
    isConversionComplete = false;
    tempCalValue.offset = finalOffset;
    ADC_CalibrationLog(" Set offset value : %d \n\r", tempCalValue.offset);
    
    int8_t gain = ADC_CAL_GAIN_MAX_VALUE;
    
    for(int8_t i=0 ; i < (32u) ; i++)
    {
        /* Update the gain value in ALT_CAL */
        tempCalValue.gain = gain;
        Cy_Adc_SetAltAnalogCalibrationValue(base, &tempCalValue);
        
        isConversionComplete = false;
        currentAdcAvgValue = 0;
        
        /* take the average value from adc */
        currentAdcAvgValue = ADC_ReadChannelData(CY_ADC_PIN_ADDRESS_VREF_L);

#ifdef DISPLAY_DETAIL_LOG         
        ADC_CalibrationLog(" Set gain value : %d ", tempCalValue.gain);
        ADC_CalibrationLog(" Current Avg Value : %d \n\r", currentAdcAvgValue);
#endif
        
        /* check for the ADC reading changes from 0x001 -> 0x000 */
        if((currentAdcAvgValue < ADC_VFERL_GAIN_TRANSIT_VALUE) && (previousAdcAvgValue == ADC_VFERL_GAIN_TRANSIT_VALUE))
        {
            isCalibrationDone = true;
            break;
        }
        else
        {
            previousAdcAvgValue = currentAdcAvgValue;
        }
        gain--;
    }
    
    /* If calibration is done update the vreflGain value */
    if(isCalibrationDone)
    {
        vreflGain = tempCalValue.gain & 0x001F;
        ADC_CalibrationLog(" Third step calibration done.");
        ADC_CalibrationLog(" vreflGain : %d \n\r", vreflGain);
    }
    else
    {
        if (currentAdcAvgValue == 0)
        {
            vreflGain = 1;
            ADC_CalibrationLog(" Third step calibration done.");
            ADC_CalibrationLog(" vreflGain : %d \n\r", vreflGain);
        }
        else
        {
            vreflGain = 0xFF;
            ADC_CalibrationLog(" Error : Not able to find the calibration value for vreflGain. \n\r");
        }
    }
    
    return vreflGain;
}

static cy_en_adc_status_t ADC_CalculateSoftTrim(volatile stc_PASS_SAR_t * base)
{
    cy_en_adc_status_t eCalStatus;
    bool overallCalibration=true;
    int16_t vreflGain=0, finalGain=0;
    int16_t vreflOffset=0, vrefhOffset=0, finalOffset=0;
    
    cy_stc_adc_analog_calibration_conifg_t analogCalValue={0};
    
    /* Get the ADC calibration value from trim */ 
    ADC_CalibrationLog("\n\r ------- ADC offset calibration -------\n\r");
    
    analogCalValue.offset = ADC_CAL_OFFSET_MAX_VALUE;
    analogCalValue.gain = ADC_CAL_GAIN_MAX_VALUE;
    
    ADC_CalibrationLog(" Starting Offset value\t: %d \n\r", analogCalValue.offset);
    ADC_CalibrationLog(" Starting Gain value\t: %d \n\r", analogCalValue.gain);
    
    /***************************************************************************
    * Calibrate the offset value for SAR
    **************************************************************************/
    /* Perform offset calibration operation on Vref_L */
    vreflOffset = ADC_Calibration_Vref_L ( base );
    if (vreflOffset == 0xFF) overallCalibration = false;
    
    /* Perform offset calibration operation on Vref_H */
    vrefhOffset = ADC_Calibration_Vref_H ( base );
    if (vrefhOffset == 0xFF) overallCalibration = false;
    
    /* Calculate overall offset value */
    if(overallCalibration == true)
    {
        finalOffset = (int8_t)((vreflOffset + vrefhOffset) >> 1) + 2;
        ADC_CalibrationLog(" final offset value A : %d \n\r", finalOffset);
    }
    
    /***************************************************************************
    * Calibrate the Gain value for SAR
    **************************************************************************/
    vreflGain = ADC_Calibration_Gain ( base , finalOffset);
    if (vreflGain == 0xFF) overallCalibration = false;
    
    /* Calculate overall gain value */
    if(overallCalibration == true)
    {
        finalGain = (vreflGain - 1) & 0x001F;
        ADC_CalibrationLog(" final gain value B : %d \n\r", finalGain);
    }
    
    /***************************************************************************
    * Update the calibration register for SAR
    **************************************************************************/
    if(overallCalibration == true)
    {
        ADC_CalibrationLog("\n\r ------- ADC calibration update -------\n\r");
        
        /* Update final gain and offset to the alternate calibration register */
        analogCalValue.offset = finalOffset;
        analogCalValue.gain = finalGain;
        Cy_Adc_SetAltAnalogCalibrationValue(base, &analogCalValue);
        
        ADC_CalibrationLog("\n Updating ADC calibration register...\n\r");
        
        /* Trigger calibration for coherent update */
        Cy_Adc_TriggerCalibrationUpdate(base);
        
        /* Check if update done */
        if(Cy_Adc_IsCalibrationUpdateDone(base))
        {
            /* Get and check for the proper update to calibration register */
            Cy_Adc_GetAnalogCalibrationValue(base, &analogCalValue);
            CY_ASSERT_L1(analogCalValue.offset == finalOffset);
            CY_ASSERT_L1(analogCalValue.gain == finalGain);
            
            ADC_CalibrationLog("\n ADC calibration offset\t: %d \n\r", analogCalValue.offset);
            ADC_CalibrationLog(" ADC calibration gain\t: %d \n\r", analogCalValue.gain);
            ADC_CalibrationLog("\n*************** ADC Calibration Done ***************");
            
            eCalStatus = CY_ADC_SUCCESS;
        }
        else
        {
            ADC_CalibrationLog("\n Error : Not able to update calibration register. \n\r");
            eCalStatus =  CY_ADC_BAD_PARAM;
        }
    }
    else
    {
        ADC_CalibrationLog("\n Error : Failed to find calibration values. \n\r");
        ADC_CalibrationLog("\n*************** ADC Calibration Failed ***************\n");
        eCalStatus =  CY_ADC_BAD_PARAM;
    }
    return eCalStatus;
}

static void ADC_CheckSoftTrim(void)
{
    float currentAdcAvgValue=0;
    
    ADC_CalibrationLog("\n\n\r ------- ADC calibration test -------\n\r");
    
    /* Use updated calibration parameter */
    tsAdcCalibrationChannelConfig.calibrationValueSelect = CY_ADC_CALIBRATION_VALUE_REGULAR;
    
    /* Configure VREF_L channel and measure */
    ADC_CalibrationLog("\n Configuring VREF_L channel \n\r");
    isConversionComplete = false;
    
    /* Check for measurement result */
    currentAdcAvgValue = ADC_ReadChannelData(CY_ADC_PIN_ADDRESS_VREF_L);
    ADC_CalibrationLog("\n\r Current Avg Value L : %f \n\r", currentAdcAvgValue);
    ADC_CalibrationLog("\n\r --------------------------------\n\r");
    
    /* Configure VREF_H channel and measure */
    ADC_CalibrationLog("\n Configuring VREF_H channel \n\r");
    isConversionComplete = false;
    currentAdcAvgValue = 0;
      
    /* Check for measurement result */
    currentAdcAvgValue = ADC_ReadChannelData(CY_ADC_PIN_ADDRESS_VREF_H);
    ADC_CalibrationLog("\n\r Current Avg Value H : %f \n\r", currentAdcAvgValue);
    ADC_CalibrationLog("\n\r --------------------------------\n\r");
    
    ADC_CalibrationLog("\n\r************ ADC Calibration Test Done *************\n\r");
}

uint8_t HAL_ADC_Calibration (HAL_ADC_Dbg_Calibration pDbgCallback)
{
    uint8_t u8HalResult = DRIVER_FALSE;
    cy_en_adc_status_t eCalStatus;
    /*ADC Group configutaion*/
    adc_group_config_t tdADC_Config =
    {
        .u8PreconditionTime = 0u,
        .u8PowerupTime = 0u,
        .u8EnableIdlePowerDown = FUNC_DISABLE,
    };
    
    if (DRIVER_TRUE == HAL_ADC_DeInit(ADC_GROPU0))
    {
        pCalibrationDbg = pDbgCallback;
        HAL_ADC_Init(ADC_GROPU0, &tdADC_Config);
        
        /*Register adc0 channel 0 callback*/
        HAL_ADC_Callback_Register (ADC0_LOGIC_CH0, ADCx_Ch0_Callback );
        
        eCalStatus = ADC_CalculateSoftTrim (PASS0_SAR0);
        if (CY_ADC_SUCCESS == eCalStatus)
        {
#ifdef DOUBLE_CHECK_VREF_H_VREEF_L     
            ADC_CheckSoftTrim();
#endif            
            u8HalResult = DRIVER_TRUE;
        }
        else
            u8HalResult = DRIVER_FALSE; 
    }
    
    return u8HalResult;
}

uint8_t HAL_ADC_DieTemperature_Get(double* pDegC)
{
    double temperatureData;
    uint8_t u8HalResult;
    cy_stc_adc_temp_ref_t tempRefValue;
    cy_stc_adc_temp_raw_t tempRawValue;
     /* Update the sort temperature value matrix A */
    tempRefValue.adcTempValues.coldValue = -(TEMP_TRIM_TEMP_COLDSORT / 10.0);       // Note: Temperature data read from sFLASH is multiple of 10.
    tempRefValue.adcTempValues.roomValue = (TEMP_TRIM_TEMP_ROOMSORT / 10.0);        // Note: Temperature data read from sFLASH is multiple of 10.
    tempRefValue.adcTempValues.hotValue = (TEMP_TRIM_TEMP_HOTCLASS / 10.0);         // Note: Temperature data read from sFLASH is multiple of 10.

    /* Update the sort temperature adc value matrix b */
    tempRefValue.adcDiodeValues.coldValue = (uint16_t)TEMP_TRIM_DIODE_COLDSORT;
    tempRefValue.adcDiodeValues.roomValue = (uint16_t)TEMP_TRIM_DIODE_ROOMSORT;
    tempRefValue.adcDiodeValues.hotValue = (uint16_t)TEMP_TRIM_DIODE_HOTCLASS;

    /* Update the reference Vbg values */
    tempRefValue.adcVbgValues.coldValue = (uint16_t)TEMP_TRIM_VBG_COLDSORT;         // Note: All the Vbg trim values are almost same.
    tempRefValue.adcVbgValues.roomValue = (uint16_t)TEMP_TRIM_VBG_ROOMSORT;
    tempRefValue.adcVbgValues.hotValue = (uint16_t)TEMP_TRIM_VBG_HOTCLASS;

#ifdef DISPLAY_DETAIL_LOG
    ADC_CalibrationLog(" \n\r Type\t|\tTemp\t|\tVbe\t|\tVbg\n\r");
    ADC_CalibrationLog(" -------------------------------------------------------\n\r");
    ADC_CalibrationLog(" Cold\t| %f\t|", tempRefValue.adcTempValues.coldValue);
    ADC_CalibrationLog(" %f\t|", tempRefValue.adcDiodeValues.coldValue);
    ADC_CalibrationLog(" %f\n\r", tempRefValue.adcVbgValues.coldValue);
    ADC_CalibrationLog(" Room\t| %f\t|", tempRefValue.adcTempValues.roomValue);
    ADC_CalibrationLog(" %f\t|", tempRefValue.adcDiodeValues.roomValue);
    ADC_CalibrationLog(" %f\n\r", tempRefValue.adcVbgValues.roomValue);
    ADC_CalibrationLog(" Hot\t| %f\t|", tempRefValue.adcTempValues.hotValue);
    ADC_CalibrationLog(" %f\t|", tempRefValue.adcDiodeValues.hotValue);
    ADC_CalibrationLog(" %f\n\r", tempRefValue.adcVbgValues.hotValue);
#endif
     
    /* Set reference buffered mode on - to pump Vbg from SRSS */
    Cy_Adc_SetReferenceBufferMode(PASS0_EPASS_MMIO, CY_ADC_REF_BUF_MODE_ON);
    
    /* Read and update the raw values for VBG and Temp Sensor */
    tsAdcCalibrationChannelConfig.calibrationValueSelect = CY_ADC_CALIBRATION_VALUE_ALTERNATE;
    ADC_CalibrationLog("\n\r");
    tempRawValue.adcVbgRawValue   = ADC_ReadChannelData(CY_ADC_PIN_ADDRESS_VBG);
    tempRawValue.adcVtempRawValue = ADC_ReadChannelData(CY_ADC_PIN_ADDRESS_VTEMP);
    ADC_CalibrationLog("\n\r adcVbgRawValue\t\t: %d. \n\r", tempRawValue.adcVbgRawValue);
    ADC_CalibrationLog(" adcDiodeRawValue\t: %d. \n\r", tempRawValue.adcVtempRawValue);
    
    /* Calculate the internal die temperature */
    ADC_CalibrationLog("\n\r Calculating die-temperature...");
    temperatureData = Cy_Adc_CalculateDieTemperature(&tempRefValue, &tempRawValue);
    ADC_CalibrationLog("\n\r dieTemperature\t: %f degC\n\r", temperatureData);
    ADC_CalibrationLog(" -------------------------------------------------------\n\r");
    
    if (NULL != pDegC)
    {
        *pDegC = temperatureData;
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        temperatureData = 0.0;
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}


/*** End of file ***/

  