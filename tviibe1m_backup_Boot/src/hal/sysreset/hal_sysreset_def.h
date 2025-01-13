/***************************************************************************
*\file hal_sysreset_def.h 
****************************************************************************/

#ifndef HAL_SYSRESET_DEF_H
#define HAL_SYSRESET_DEF_H

/*******************************************************************************
*
*  \ 32-bit Contents of RES_CAUSE register
*  | Name			        | Value
*  |------------------------------------|--------------------	
*  | CY_SYSRESET_WDT			| 0x00000001 (bit0)
*  | CY_SYSRESET_ACT_FAULT		| 0x00000002 (bit1)
*  | CY_SYSRESET_DPSLP_FAULT		| 0x00000004 (bit2)
*  | CY_SYSRESET_TC_DBGRESET		| 0x00000008 (bit3)
*  | CY_SYSRESET_SOFT			| 0x00000010 (bit4)
*  | CY_SYSRESET_MCWDT0			| 0x00000020 (bit5)
*  | CY_SYSRESET_MCWDT1			| 0x00000040 (bit6)
*  | CY_SYSRESET_MCWDT2			| 0x00000080 (bit7)
*  | CY_SYSRESET_MCWDT3			| 0x00000100 (bit8)
*  | CY_SYSRESET_XRES			| 0x00010000 (bit16)
*  | CY_SYSRESET_BODVDDD		| 0x00020000 (bit17)
*  | CY_SYSRESET_BODVDDA		| 0x00040000 (bit18)
*  | CY_SYSRESET_BODVCCD		| 0x00080000 (bit19)
*  | CY_SYSRESET_OVDVDDD		| 0x00100000 (bit20)
*  | CY_SYSRESET_OVDVDDA		| 0x00200000 (bit21)
*  | CY_SYSRESET_OVDVCCD		| 0x00400000 (bit22)
*  | CY_SYSRESET_OCD_ACT_LINREG		| 0x00800000 (bit23)
*  | CY_SYSRESET_OCD_DPSLP_LINREG	| 0x01000000 (bit24)
*  | CY_SYSRESET_OCD_REGHC	        | 0x02000000 (bit25)
*  | CY_SYSRESET_PMIC	                | 0x04000000 (bit26)
*  | CY_SYSRESET_PXRES	                | 0x08000000 (bit27)
*  | CY_SYSRESET_STRUCT_XRES		| 0x20000000 (bit29)
*  | CY_SYSRESET_PORVDDD		| 0x40000000 (bit30)
*
*******************************************************************************/

/** A basic WatchDog Timer (WDT) reset has occurred since the last power cycle. */
#define SYSRESET_WDT                 (0x00000001u)
/** The fault logging system requested a reset from its Active logic. */
#define SYSRESET_ACT_FAULT           (0x00000002u)
/** The fault logging system requested a reset from its DeepSleep logic. */
#define SYSRESET_DPSLP_FAULT         (0x00000004u)
/** Test controller or debugger asserted reset. */
#define SYSRESET_TC_DBGRESET         (0x00000008u)
/** The CPU requested a system reset through it's SYSRESETREQ. This can be done via a debugger probe or in firmware. */
#define SYSRESET_SOFT                (0x00000010u)
/** The Multi-Counter Watchdog timer #0 reset has occurred since last power cycle. */
#define SYSRESET_MCWDT0              (0x00000020u)
/** The Multi-Counter Watchdog timer #1 reset has occurred since last power cycle. */
#define SYSRESET_MCWDT1              (0x00000040u)
/** The Multi-Counter Watchdog timer #2 reset has occurred since last power cycle. */
#define SYSRESET_MCWDT2              (0x00000080u)
/** The Multi-Counter Watchdog timer #3 reset has occurred since last power cycle. */
#define SYSRESET_MCWDT3              (0x00000100u)
/** External XRES pin assertion. */
#define SYSRESET_XRES                (0x00010000u)
/** Reset due to external VDDD supply crossing brown-out limit. */
#define SYSRESET_BODVDDD             (0x00020000u)
/** Reset due to external VDDA supply crossed the brown-out limit. */
#define SYSRESET_BODVDDA             (0x00040000u)
/** Reset due to internal VCCD core supply crossed the brown-out limit. */
#define SYSRESET_BODVCCD             (0x00080000u)
/** Reset due to over voltage detection on the external VDDD supply. */
#define SYSRESET_OVDVDDD             (0x00100000u)
/** Reset due to over voltage detection on the external VDDA supply.. */
#define SYSRESET_OVDVDDA             (0x00200000u)
/** Reset due to over voltage detection on the internal core VCCD supply. */
#define SYSRESET_OVDVCCD             (0x00400000u)
/** Reset due to over current detection on the internal VCCD supply when supplied by the ACTIVE power mode linear regulator. */
#define SYSRESET_OCD_ACT_LINREG      (0x00800000u)
/** Reset due to over current detection on the internal VCCD supply when supplied by the DEEPSLEEP power mode linear regulator. */
#define SYSRESET_OCD_DPSLP_LINREG    (0x01000000u)
/** Reset due to over current detection from REGHC (if present). */
#define SYSRESET_OCD_REGHC           (0x02000000u)
/** PMIC status triggered a reset. */
#define SYSRESET_PMIC                (0x04000000u)
/** Programmable XRES triggered. */
#define SYSRESET_PXRES               (0x08000000u)
/** Structural reset was asserted. */
#define SYSRESET_STRUCT_XRES         (0x20000000u)
/** Indicator that a POR occurred. */
#define SYSRESET_PORVDDD             (0x40000000u)

#endif

  