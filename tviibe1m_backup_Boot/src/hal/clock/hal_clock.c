/***************************************************************************
*\file hal_clock.c
****************************************************************************/ 

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_clock_def.h"
#include "hal_clock.h"

static uint8_t HAL_Clock_Source_Init(clock_config_t *tsConfig)
{
    uint8_t u8HalResult;
    uint8_t u8ErrorCounter = 0;

    /***  Setting Clock Path2 source            ***/
    if(DRIVER_TRUE == HAL_Clock_Input_Path_Setting( SYSCLK_CLKPATH_IN_IMO, CLOCK_PATH2))
    {
        /***  Set HFx Path Source                       ***/
        u8HalResult = HAL_Clock_Output_Path_Setting( CLOCK_PATH2 , CLOCK_HF0 );
        
        /***  Set HFx Divider and Enable                ***/
        Cy_SysClk_HfClockSetDivider( (cy_en_hfclk_t)CLOCK_HF0 ,CY_SYSCLK_HFCLK_NO_DIVIDE);
    }
    else
    {
        u8ErrorCounter++;
    }
    
    /***  ECO Enable/Disable                    ***/
    if(DRIVER_TRUE == HAL_Clock_High_External_Clock_Active(tsConfig->u8EcoEnable, tsConfig->u32Timeout))
    {
        ;/* Do nothing */
    }
    else
    {
        u8ErrorCounter++;
    }
    
    /***  ILO0 Enable/Disable                   ***/
    if(DRIVER_TRUE == HAL_Clock_Low_Internal_Clock_0_Active(tsConfig->u8Ilo0Enable, tsConfig->u32Timeout))
    {
        ;/* Do nothing */
    }
    else
    {
        u8ErrorCounter++;
    }
    
    /***  ILO1 Enable/Disable                   ***/
    if(DRIVER_TRUE == HAL_Clock_Low_Internal_Clock_1_Active(tsConfig->u8Ilo1Enable, tsConfig->u32Timeout))
    {
        ;/* Do nothing */
    }
    else
    {
        u8ErrorCounter++;
    }
    
    /***  WCO Enable/Disable                    ***/
    if(DRIVER_TRUE == HAL_Clock_Low_External_Clock_Active(tsConfig->u8WcoEnable, tsConfig->u32Timeout))
    {
        ;/* Do nothing */
    }
    else
    {
        u8ErrorCounter++;
    }
    
    /***  PLL setting and enabling              ***/
    if(DRIVER_TRUE == HAL_Clock_PLL_Configuration (&tsConfig->tsPllConfig))
    {
        ;/* Do nothing */
    }
    else
    {
        u8ErrorCounter++;
    }
    
    /***  FLL setting and enabling              ***/
    if(DRIVER_TRUE == HAL_Clock_FLL_Configuration(&tsConfig->tsFllConfig))
    {
        ;/* Do nothing */
    }
    else
    {
        u8ErrorCounter++;
    }

    if(0 == u8ErrorCounter)
    {
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_FALSE;
    }

    return u8HalResult;
}

uint8_t HAL_Clock_Init(clock_config_t *tsConfig)
{
    /*ARM M0 pluse*/
#if (__CORTEX_M == 0x0u)
    uint8_t u8HalResult = DRIVER_FALSE;
    
    if (NULL == tsConfig)
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        /* disable WDT */
        Cy_WDT_Disable();

        /***  Setting wait state for ROM                ***/
        CPUSS->unROM_CTL.stcField.u2SLOW_WS = ROM_SLOW_WAIT_STATE;
        CPUSS->unROM_CTL.stcField.u2FAST_WS = ROM_FAST_WAIT_STATE;

        /***  Setting wait state for RAM                ***/
        CPUSS->unRAM0_CTL0.stcField.u2SLOW_WS = RAM0_SLOW_WAIT_STATE;
        CPUSS->unRAM0_CTL0.stcField.u2FAST_WS = RAM0_FAST_WAIT_STATE;

        CPUSS->unRAM1_CTL0.stcField.u2SLOW_WS = RAM1_SLOW_WAIT_STATE;
        CPUSS->unRAM1_CTL0.stcField.u2FAST_WS = RAM1_FAST_WAIT_STATE;

        /****  Setting wait state for FLASH             ***/
        FLASHC->unFLASH_CTL.stcField.u4MAIN_WS = FLASH_WAIT_STATE;

        /***    Set clock LF source  (WDT)      ***/
        SRSS->unCLK_SELECT.stcField.u3LFCLK_SEL = CY_SYSCLK_LFCLK_IN_ILO0;

        /***  Set CPUSS dividrs as required             ***/
        /***  FAST = PERI = CLK_SLOW = 80,000,000Hz     ***/
        Cy_SysClk_FastClkSetDivider (CLK_FAST_DIVIDER);
        Cy_SysClk_PeriClkSetDivider (CLK_PERI_DIVIDER);
        Cy_SysClk_SlowClkSetDivider (CLK_SLOW_DIVIDER);
        
        /*Reset ECO Configuration Register to default value*/
        volatile uint32_t* pEco_config = (volatile uint32_t*) 0x4026151Cu;
        *pEco_config = 0x00000002u;
        
        u8HalResult = HAL_Clock_Source_Init(tsConfig);
        
        if(DRIVER_TRUE == u8HalResult)
        {
            Cy_WDT_Unlock();
            Cy_SysClk_Ilo0HibernateOn((bool)tsConfig->u8Ilo0Enable);
            Cy_WDT_Lock();
            
            /* Check the IO status. If current status is frozen, unfreeze the system. */
            if( true == Cy_SysPm_GetIoFreezeStatus())
            {
                /* Unfreeze the system */
                Cy_SysPm_IoUnfreeze();
            }
            else
            {
                ;/* Do nothing */
            }
            SystemCoreClockUpdate();
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            ;/* Do nothing */
        }
    }

    return u8HalResult;
#else
    /*ARM M4 : ARM M0 has initialized the system clock*/
    UNUSED(tsConfig);
    SystemInit();  /*M4 system init*/
    /* Check the IO status. If current status is frozen, unfreeze the system. */
    if(Cy_SysPm_GetIoFreezeStatus())
    {
        /* Unfreeze the system */
        Cy_SysPm_IoUnfreeze();
    }
    else
    {
        ;/* Do nothing */
    }

    return DRIVER_TRUE;
#endif
}

static uint8_t HAL_Clock_HF_Source_Get(uint32_t u32ClkPath)
{
    return SRSS->unCLK_ROOT_SELECT[u32ClkPath].stcField.u4ROOT_MUX;
}

static uint8_t HAL_Clock_PllStatus_Get(uint32_t u32ClkPath, uint32_t* pReturn)
{
    uint8_t  u8HalResult;
    
    if ((u32ClkPath != 0UL) && (u32ClkPath <= SRSS_NUM_PLL))
    {
        u32ClkPath--;
        * pReturn = SRSS->unCLK_PLL_CONFIG[u32ClkPath].stcField.u1ENABLE ;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    return u8HalResult;
}

static uint8_t HAL_Clock_FllStatus_Get(void)
{
    return SRSS->unCLK_FLL_CONFIG4.stcField.u1CCO_ENABLE;
}

static uint8_t HAL_Clock_FLL_PreConfiguration(pll_fll_config_t *tsFllConfig)
{
    uint8_t u8HalResult;

    if (NULL != tsFllConfig)
    {
        if(FUNC_ENABLE  == HAL_Clock_FllStatus_Get())
        {
            if(CLOCK_PATH0 == HAL_Clock_HF_Source_Get(CLOCK_HF0))
            {
                /***  Set HFx Path Source                       ***/
                (void)HAL_Clock_Output_Path_Setting( CLOCK_PATH2 , CLOCK_HF0 );
            }
            else
            {
                ;/* Do nothing */
            }
        }
        else
        {
            ;/* Do nothing */
        }
        (void)HAL_Clock_FLL_Clock_Active(FUNC_DISABLE, WAIT_FOR_STABILIZATION);
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    
    return u8HalResult;
}

static uint8_t HAL_Clock_PLL_PreConfiguration(pll_fll_config_t *tsPllConfig)
{
    uint8_t u8HalResult;
    uint32_t u32PllEnable;
    
    if (NULL != tsPllConfig)
    {
        u8HalResult = HAL_Clock_PllStatus_Get(CLOCK_PATH1, &u32PllEnable);
        if(DRIVER_TRUE == u8HalResult)
        {
            if(FUNC_ENABLE  == u32PllEnable)
            {
                if(CLOCK_PATH1 == HAL_Clock_HF_Source_Get(CLOCK_HF0))
                {
                    /***  Set HFx Path Source                       ***/
                    (void)HAL_Clock_Output_Path_Setting( CLOCK_PATH2 , CLOCK_HF0 );
                }
                else
                {
                    ;/* Do nothing */
                }
            }
            else
            {
                ;/* Do nothing */
            }
            (void)HAL_Clock_PLL_Clock_Active(FUNC_DISABLE, WAIT_FOR_STABILIZATION);
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            ;/* Do nothing */
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }

    (void)u32PllEnable;
    return u8HalResult;
}

uint8_t HAL_Clock_PLL_Configuration(pll_fll_config_t *tsPllConfig)
{
    uint8_t u8HalResult;
    cy_stc_pll_config_t tsPllConfigTemp;
    cy_en_sysclk_status_t eResult;

    u8HalResult = HAL_Clock_PLL_PreConfiguration(tsPllConfig);
    if(DRIVER_TRUE == u8HalResult && FUNC_ENABLE == tsPllConfig->u8Enable )
    {
        memset(&tsPllConfigTemp,0,sizeof(tsPllConfigTemp));
        u8HalResult = HAL_Clock_Input_Path_Setting( tsPllConfig->u8InputClockPath, CLOCK_PATH1);
        if (DRIVER_TRUE == u8HalResult)
        {
            tsPllConfigTemp.inputFreq  = tsPllConfig->u32InClkFreq;
            tsPllConfigTemp.outputFreq = tsPllConfig->u32OutClkFreq;
            tsPllConfigTemp.lfMode     = (0U);  /**< CLK_PLL_CONFIG register, PLL_LF_MODE bit */
            tsPllConfigTemp.outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO;

            eResult = Cy_SysClk_PllConfigure(CLOCK_PATH1 , &tsPllConfigTemp);

            if (CY_SYSCLK_SUCCESS == eResult)
            {
                u8HalResult = HAL_Clock_PLL_Clock_Active(FUNC_ENABLE, WAIT_FOR_STABILIZATION);
                if (DRIVER_TRUE == u8HalResult)
                {
                    (void)HAL_Clock_Output_Path_Setting( CLOCK_PATH1 , tsPllConfig->u8ToClockHF );
                    Cy_SysClk_HfClkEnable( (cy_en_hfclk_t)tsPllConfig->u8ToClockHF );
                    Cy_SysClk_HfClockSetDivider( (cy_en_hfclk_t)tsPllConfig->u8ToClockHF  ,CY_SYSCLK_HFCLK_NO_DIVIDE);
                }
                else
                {
                    ;/* Do nothing */
                }
            }
            else if(CY_SYSCLK_BAD_PARAM == eResult)
            {
                u8HalResult = DRIVER_PARAMETER_ERROR;
            }
            else
            {
                u8HalResult = DRIVER_FALSE;
            }
        }
        else
        {
            UNUSED(eResult);
        }
    }
    else
    {
        UNUSED(eResult);
    }

    return u8HalResult;
}

uint8_t HAL_Clock_FLL_Configuration(pll_fll_config_t *tsFllConfig)
{
    uint8_t u8HalResult;

    u8HalResult = HAL_Clock_FLL_PreConfiguration(tsFllConfig);
    if (DRIVER_TRUE == u8HalResult)
    {
        if( FUNC_ENABLE == tsFllConfig->u8Enable)
        {
            u8HalResult = HAL_Clock_Input_Path_Setting( tsFllConfig->u8InputClockPath, CLOCK_PATH0);
            if ( DRIVER_TRUE == u8HalResult)
            {
                if (CY_SYSCLK_SUCCESS == Cy_SysClk_FllConfigureStandard(tsFllConfig->u32InClkFreq, tsFllConfig->u32OutClkFreq, CY_SYSCLK_FLLPLL_OUTPUT_AUTO))
                {
                    u8HalResult = HAL_Clock_FLL_Clock_Active(FUNC_ENABLE, WAIT_FOR_STABILIZATION);
                    if (DRIVER_TRUE == u8HalResult)
                    {
                        (void) HAL_Clock_Output_Path_Setting( CLOCK_PATH0 , tsFllConfig->u8ToClockHF );
                        Cy_SysClk_HfClockSetDivider( (cy_en_hfclk_t)tsFllConfig->u8ToClockHF  ,CY_SYSCLK_HFCLK_NO_DIVIDE);
                        Cy_SysClk_HfClkEnable( (cy_en_hfclk_t)tsFllConfig->u8ToClockHF );
                    }
                    else
                    {
                        ;/* Do nothing */
                    }
                }
                else
                {
                    u8HalResult = DRIVER_FALSE;
                }
            }
            else
            {
                ;/* Do nothing */
            }
        }
        else
        {
            ;/* Do nothing */
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }

    return u8HalResult;
}

/*ECO Enable/Disable*/
uint8_t HAL_Clock_High_External_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
    uint8_t u8HalResult;

    if (FUNC_ENABLE == u8Enable)
    {
        if (CY_SYSCLK_SUCCESS == Cy_SysClk_EcoConfigure(CLK_FREQ_ECO, LOAD_CAP_IN_PF, ESR_IN_OHME, MAX_DRIVE_LEVEL_IN_UW))
        {
            if (CY_SYSCLK_SUCCESS == Cy_SysClk_EcoEnable(u32Timeout))
            {
                u8HalResult = DRIVER_TRUE;
            }
            else
            {
                u8HalResult = DRIVER_TIMEOUT;
            }
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
    }
    else
    {
        Cy_SysClk_EcoDisable();
        u8HalResult = DRIVER_TRUE;
    }

    return u8HalResult;
}

/*WCO Enable/Disable*/
uint8_t HAL_Clock_Low_External_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
    uint8_t u8HalResult;

    if (FUNC_ENABLE == u8Enable)
    {
        if (CY_SYSCLK_SUCCESS == Cy_SysClk_WcoEnable(u32Timeout))
        {
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_TIMEOUT;
        }
    }
    else
    {
        Cy_SysClk_WcoDisable();
        u8HalResult = DRIVER_TRUE;
    }

    return u8HalResult;
}

/*IMO Enable/Disable*/
uint8_t HAL_Clock_High_Internal_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
    uint8_t u8HalResult;
    UNUSED(u8Enable);
    UNUSED(u32Timeout);
    /*
    Disabling internal clock sources that are not generating
    a system clock. All clock sources are initially disabled,
    except the IMO. Note that some clock sources, such as
    the crystal oscillators (WCO and ECO) have relatively
    long startup times. Switching these circuits off and on
    may result in more overall current if the system must idle
    while they start up.
    */
    /*Currently SDL 6.5.0 does not implement IMO enable/disable*/
    
    u8HalResult = DRIVER_PARAMETER_ERROR;

    return u8HalResult;
}

uint8_t HAL_Clock_Low_Internal_Clock_0_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
    uint8_t u8HalResult;
    
    Cy_WDT_Unlock();
    if (FUNC_ENABLE == u8Enable)
    {
        if (CY_SYSCLK_SUCCESS == Cy_SysClk_Ilo0Enable())
        {
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_WRITE_ERROR;
        }
    }
    else
    {
        if (CY_SYSCLK_SUCCESS == Cy_SysClk_Ilo0Disable())
        {
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_WRITE_ERROR;
        }
    }
    Cy_WDT_Lock();

    return u8HalResult;
}

uint8_t HAL_Clock_Low_Internal_Clock_1_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
    uint8_t u8HalResult;

    Cy_WDT_Unlock();
    if (FUNC_ENABLE == u8Enable)
    {
        Cy_SysClk_Ilo1Enable();
        u8HalResult = DRIVER_TRUE;
    }
    else
    {
        if (CY_SYSCLK_SUCCESS == Cy_SysClk_Ilo1Disable())
        {
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_TIMEOUT;
        }
    }
    Cy_WDT_Lock();

    return u8HalResult;
}

uint8_t HAL_Clock_PLL_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
    cy_en_sysclk_status_t eResult;
    uint8_t u8HalResult;
    
    if (FUNC_ENABLE == u8Enable)
    {
        eResult = Cy_SysClk_PllEnable(CLOCK_PATH1, u32Timeout);
        if (CY_SYSCLK_SUCCESS == eResult)
        {
            u8HalResult = DRIVER_TRUE;
        }
        else if(CY_SYSCLK_TIMEOUT == eResult)
        {
            u8HalResult = DRIVER_TIMEOUT;
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
    }
    else
    {
        eResult = Cy_SysClk_PllDisable(CLOCK_PATH1);
        if (CY_SYSCLK_SUCCESS == eResult)
        {
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_PARAMETER_ERROR;
        }
    }

    return u8HalResult;
}

uint8_t HAL_Clock_FLL_Clock_Active(uint8_t u8Enable, uint32_t u32Timeout)
{
    uint8_t u8HalResult;

    if (FUNC_ENABLE == u8Enable)
    {
        if (CY_SYSCLK_SUCCESS == Cy_SysClk_FllEnable(u32Timeout))
        {
            SRSS->unCLK_FLL_STATUS.stcField.u1UNLOCK_OCCURRED = 1ul;
            /* Set the FLL bypass mode to 3 */
            SRSS->unCLK_FLL_CONFIG3.stcField.u2BYPASS_SEL = (uint32_t)CY_SYSCLK_FLLPLL_OUTPUT_OUTPUT;
            u8HalResult = DRIVER_TRUE;
        }
        else
        {
            u8HalResult = DRIVER_TIMEOUT;
        }
    }
    else
    {
        (void)Cy_SysClk_FllDisableSequence(u32Timeout);
        u8HalResult = DRIVER_TRUE;
    }

    return u8HalResult; 
}

uint8_t HAL_Clock_Output_Path_Setting(uint8_t u8ClockSource, uint8_t u8ClockTarget)
{
    uint8_t u8HalResult = DRIVER_FALSE;

    if (BYPASSED_PATH_NO < u8ClockSource || CLOCK_HF2 < u8ClockTarget)
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }
    else
    {
        Cy_SysClk_HfClkEnable((cy_en_hfclk_t)u8ClockTarget);
        Cy_SysClk_HfClockSetSource((cy_en_hfclk_t)u8ClockTarget, (cy_en_hf_clk_sources_t)u8ClockSource);
        u8HalResult = DRIVER_TRUE;
    }

    return u8HalResult; 
}

uint8_t HAL_Clock_Input_Path_Setting(uint32_t u32ClockSource, uint8_t u8ClockTarget)
{
    uint8_t u8HalResult;

    if(BYPASSED_PATH_NO >= u8ClockTarget)
    {
        if(SYSCLK_CLKPATH_IN_ECO >= u32ClockSource)
        {
            if(CY_SYSCLK_SUCCESS == Cy_SysClk_ClkPathSetSource(u8ClockTarget, (cy_en_clkpath_in_sources_t)u32ClockSource))
            {
                u8HalResult = DRIVER_TRUE;
            }
            else
            {
                u8HalResult = DRIVER_PARAMETER_ERROR;
            }
        }
        else
        {
            if(((SYSCLK_CLKPATH_IN_DSI0 <= u32ClockSource) && (SYSCLK_CLKPATH_IN_WCO >= u32ClockSource)) ||\
                (SYSCLK_CLKPATH_IN_ILO1 == u32ClockSource))
            {
                if(CY_SYSCLK_SUCCESS == Cy_SysClk_ClkPathSetSource(u8ClockTarget, (cy_en_clkpath_in_sources_t)u32ClockSource))
                {
                    u8HalResult = DRIVER_TRUE;
                }
                else
                {
                    u8HalResult = DRIVER_PARAMETER_ERROR;
                }
            }
            else
            {
                u8HalResult = DRIVER_PARAMETER_ERROR;
            }
        }
    }
    else
    {
        u8HalResult = DRIVER_PARAMETER_ERROR;
    }

    return u8HalResult;
}


/*** End of file ***/
