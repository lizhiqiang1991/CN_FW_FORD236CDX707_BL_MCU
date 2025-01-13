#ifndef HAL_CLOCK_DEF_H
#define HAL_CLOCK_DEF_H

typedef struct
{
    uint8_t  u8InputClockPath; /*CY_SYSCLK_CLKPATH_IN_IMO / CY_SYSCLK_CLKPATH_IN_ECO / CY_SYSCLK_CLKPATH_IN_EXT*/
    uint32_t u32InClkFreq;
    uint32_t u32OutClkFreq;
    uint8_t  u8ToClockHF;
    uint8_t u8Enable;
} pll_fll_config_t;

typedef struct
{
    pll_fll_config_t tsPllConfig;
    pll_fll_config_t tsFllConfig;
    uint32_t u32EcoFreq;
    uint8_t u8EcoEnable;
    uint8_t u8WcoEnable;
    uint8_t u8Ilo0Enable;
    uint8_t u8Ilo1Enable;
    uint32_t u32Timeout;
} clock_config_t;

/** Wait time definition **/
#define WAIT_FOR_STABILIZATION      (50000U)

/** System clock input source **/
#define SYSCLK_CLKPATH_IN_IMO       0u      /**< Select the IMO as the output of the path mux */
#define SYSCLK_CLKPATH_IN_EXT       1u      /**< Select the EXT as the output of the path mux */
#define SYSCLK_CLKPATH_IN_ECO       2u      /**< Select the ECO as the output of the path mux */
#define SYSCLK_CLKPATH_IN_DSIMUX    4u      /**< Select the DSI MUX output as the output of the path mux */
#define SYSCLK_CLKPATH_IN_DSI0      0x100u  /**< Select a DSI signal 0 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI1      0x101u  /**< Select a DSI signal 1 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI2      0x102u  /**< Select a DSI signal 2 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI3      0x103u  /**< Select a DSI signal 3 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI4      0x104u  /**< Select a DSI signal 4 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI5      0x105u  /**< Select a DSI signal 5 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI6      0x106u  /**< Select a DSI signal 6 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI7      0x107u  /**< Select a DSI signal 7 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI8      0x108u  /**< Select a DSI signal 8 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI9      0x109u  /**< Select a DSI signal 9 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI10     0x10Au  /**< Select a DSI signal 10 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI11     0x10Bu  /**< Select a DSI signal 11 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI12     0x10Cu  /**< Select a DSI signal 12 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI13     0x10Du  /**< Select a DSI signal 13 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI14     0x10Eu  /**< Select a DSI signal 14 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_DSI15     0x10Fu  /**< Select a DSI signal 15 as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_ILO0      0x110u  /**< Select the ILO0 (16) as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_WCO       0x111u  /**< Select the WCO (17) as the output of the DSI mux and path mux */
#define SYSCLK_CLKPATH_IN_ILO1      0x114u  /**< Select the ILO1 (20) as the output of the DSI mux and path mux */

/** System clock input source path **/
#define CLOCK_PATH0                 FLL_PATH_NO
#define CLOCK_PATH1                 PLL_PATH_NO
#define CLOCK_PATH2                 BYPASSED_PATH_NO

/** System clock output path **/
#define CLOCK_HF0                   (0U)
#define CLOCK_HF1                   (1U)
#define CLOCK_HF2                   (2U)

/** ECO parameters **/
#define CLK_FREQ_ECO                (16000000UL)
#define LOAD_CAP_IN_PF              (17UL)
#define ESR_IN_OHME                 (250UL)
#define MAX_DRIVE_LEVEL_IN_UW       (100UL)

/** IMO parameters **/
#define CLK_FREQ_IMO                ( 8000000ul)

/** FLL parameters **/
#define FLL_TARGET_FREQ             (100000000ul)

/** PLL parameters **/
#define PLL_TARGET_FREQ             (160000000ul)

#define FLASH_WAIT_STATE            (1ul) /* divided by 2 */
#define RAM0_FAST_WAIT_STATE        (0ul) /* divided by 1 */
#define RAM0_SLOW_WAIT_STATE        (1ul) /* divided by 2 */
#define RAM1_FAST_WAIT_STATE        (0ul) /* divided by 1 */
#define RAM1_SLOW_WAIT_STATE        (1ul) /* divided by 2 */
#define ROM_FAST_WAIT_STATE         (0ul) /* divided by 1 */
#define ROM_SLOW_WAIT_STATE         (1ul) /* divided by 2 */

#define CLK_FAST_DIVIDER            (1ul)  /* divided by 2 */
#define CLK_SLOW_DIVIDER            (0ul)  /* no division  */
#define CLK_PERI_DIVIDER            (1ul)  /* divided by 2 */

extern clock_config_t gtsClockConfig;

#endif


