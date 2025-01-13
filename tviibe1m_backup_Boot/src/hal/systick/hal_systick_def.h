/***************************************************************************
*\file hal_systick_def.h 
****************************************************************************/

#ifndef HAL_SYSTICK_DEF_H
#define HAL_SYSTICK_DEF_H

typedef void(*Systick_ISR) (void* pContext);

typedef struct
{
    uint32_t u32ReloadCounter;
    uint32_t u32Counter;
    uint8_t u8ClockSource;
    uint8_t u8InterruptEnable;
    uint8_t u8NvicPriority;
    
#if 0    
    uint32_t u32Timeout;
#endif
    
}systick_config_t;

/* uint8_t u8ClockSource */
 /** The low frequency clock clk_lf is selected. The precision of this clock depends on whether the low frequency
    clock source is a SRSS internal RC oscillator (imprecise) or a device external crystal oscillator (precise) */
#define SYSTICK_CLOCK_SOURCE_CLK_LF     CY_SYSTICK_CLOCK_SOURCE_CLK_LF

    /** The internal main oscillator (IMO) clock clk_imo is selected. */
#define SYSTICK_CLOCK_SOURCE_CLK_IMO    CY_SYSTICK_CLOCK_SOURCE_CLK_IMO

    /** The external crystal oscillator (ECO) clock clk_eco is selected. */
#define SYSTICK_CLOCK_SOURCE_CLK_ECO    CY_SYSTICK_CLOCK_SOURCE_CLK_ECO

    /** The SRSS clk_timer is selected. The clk_timer is a divided/gated version of clk_hf or clk_imo. */
#define SYSTICK_CLOCK_SOURCE_CLK_TIMER  CY_SYSTICK_CLOCK_SOURCE_CLK_TIMER

    /** The CPU clock is selected. */
#define SYSTICK_CLOCK_SOURCE_CLK_CPU    CY_SYSTICK_CLOCK_SOURCE_CLK_CPU

extern systick_config_t gtdSystickConfig;

#endif

  