/*

File    : main_cm0_testbed.c
Purpose : Testbed application start

*/
#include "cy_project.h"
#include "cy_device_headers.h"
#include <stdio.h>
#include "hal_config.h"

/* GPIO */
extern void testbed_gpio      (void);
extern void testbed_gpio_intr (void);
/* CLOCK */
extern void testbed_clock (void);
/* TIMER */
extern void testbed_timer (void);
extern void testbed_timer_count_sec(void);
/* PWM */
extern void testbed_pwm   (void);
extern void testbed_pwm_period(void);
extern void testbed_dynamic_pwm_duty(void);
extern void testbed_pwm_duty_interrput(void);
/* I2C */
extern void testbed_i2c_master (void);
extern void testbed_i2c_mem_master(void);
extern void testbed_i2c_slave (void);
extern void testbed_i2c_led_slave(void);
/* UART */
extern void testbed_uart_polling (void);
extern void testbed_uart_interrupt(void);
/* SPI */
extern void testbed_spi_master(void);
/* POWER MODE */
extern void testbed_power_mode(void);
/* ADC */
extern void testbed_adc_interrupt(void);
extern void testbed_adc_normal_read(void);
extern void testbed_adc_range_detect(void);
extern void testbed_adc_die_temp(void);
/*Systick */
extern void testbed_systick(void);
/*Main/Wroking Flash*/
extern void testbed_fmc(void);
/*Simple bootloader*/
extern void testbed_simple_bootloader(void);
/*System reset cause*/
extern void testbed_sys_reset_cause(void);
/* check main stack usage*/ 
extern void testbed_sp    (void); 
/*Watch dog */
extern void testbed_wdt   (void);
extern void testbed_wdt_interrupt(void);
/*Crypto CRC*/
extern void testbed_crypto_crc(void);
/*Reading silicon ID*/
extern void testbed_silicon_id(void);

#define GPIO_TESTBED               0
#define GPIO_INTERRUPT_TESTBED     1
#define CLK_TESTBED                10
#define ADC_READ_TESTBED           13
#define ADC_INTERRUPT_TESTBED      14
#define ADC_RANGE_DETECT_TESTBED   15
#define ADC_DIE_TEMP_TESTBED       16
#define TIMER_TESTBED              20
#define TIMER_COUNT_SEC_TESTBED    21
#define PWM_TESTBED                30
#define PWM_PERIOD_TESTBED         40
#define PWM_DYNAMIC_DUTY_TESTBED   41
#define PWM_DUTY_INTERRUPT         42
#define I2C_MASTER_TESTBED         50
#define I2C_MEM_MASTER_TESTBED     51
#define I2C_SLAVE_TESTBED          60
#define I2C_LED_SLAVE_TESTBED      61
#define UART_POLLING_TESTBED       65
#define UART_INTERRUPT_TESTBED     66
#define SPI_MASTER_TESTBED         67
#define SP_TESTBED                 70
#define POWER_MODE_TESTBED         78
#define WDT_TESTBED                80
#define WDT_WARN_INTR_TESTBED      81
#define SYSTICK_TESTBED            83
#define CHIPID_TESTBED             90
#define FMC_TESTBED                200
#define SIMPLE_BOOTLOADER_TESTBED  300
#define SYSTEM_RESET_CAUSE_TESTBED 400
#define CRYPTO_CRC_TESTBED         500
#define SILICON_ID_TESTBED         600   
#define M4_TESTBED                 0xffff

#define CURRENT_TESTBED  (ADC_DIE_TEMP_TESTBED) 

void testbed_main(void) 
{
  /* Clock init*/
  HAL_Clock_Init(&gtsClockConfig);

  #ifndef CY_SEMIHOSTING_DISABLED
    #if (UART_POLLING_TESTBED == CURRENT_TESTBED) || (UART_INTERRUPT_TESTBED == CURRENT_TESTBED)
       /*Don't use uart semihosting */
    #else
      /*redirect printf to UART (SCB3) , P13.0 (RX) / P13.1 (TX)*/
      Cy_Semihosting_InitAll(CY_USB_SCB_TYPE,115200,NULL,false);
    #endif
  #endif
  
#if (GPIO_TESTBED == CURRENT_TESTBED)
  testbed_gpio();
#endif

#if (GPIO_INTERRUPT_TESTBED == CURRENT_TESTBED)
  testbed_gpio_intr();
#endif
  
#if (ADC_READ_TESTBED == CURRENT_TESTBED)
  testbed_adc_normal_read();
#endif
 
#if (ADC_INTERRUPT_TESTBED == CURRENT_TESTBED)
  testbed_adc_interrupt();
#endif
 
#if (ADC_RANGE_DETECT_TESTBED == CURRENT_TESTBED)
  testbed_adc_range_detect();
#endif
  
#if (ADC_DIE_TEMP_TESTBED == CURRENT_TESTBED)
  testbed_adc_die_temp();
#endif
  
#if (CLK_TESTBED == CURRENT_TESTBED)
  testbed_clock(); 
#endif

#if (TIMER_TESTBED == CURRENT_TESTBED)
  testbed_timer();
#endif

#if (TIMER_COUNT_SEC_TESTBED == CURRENT_TESTBED)
  testbed_timer_count_sec();
#endif
  
#if (PWM_TESTBED == CURRENT_TESTBED)
  testbed_pwm();
#endif

#if (PWM_PERIOD_TESTBED == CURRENT_TESTBED)
  testbed_pwm_period();
#endif  
  
#if (PWM_DYNAMIC_DUTY_TESTBED == CURRENT_TESTBED)
  testbed_dynamic_pwm_duty();
#endif  
 
#if (PWM_DUTY_INTERRUPT ==   CURRENT_TESTBED)
  testbed_pwm_duty_interrput();
#endif
  
#if (SP_TESTBED == CURRENT_TESTBED)
  testbed_sp();
#endif

#if (I2C_MASTER_TESTBED == CURRENT_TESTBED)
  testbed_i2c_master();
#endif

#if (I2C_MEM_MASTER_TESTBED == CURRENT_TESTBED)
  testbed_i2c_mem_master();
#endif
  
#if (I2C_SLAVE_TESTBED == CURRENT_TESTBED)
  testbed_i2c_slave();
#endif

#if (I2C_LED_SLAVE_TESTBED == CURRENT_TESTBED)
  testbed_i2c_led_slave();
#endif

#if (UART_POLLING_TESTBED == CURRENT_TESTBED)
  testbed_uart_polling();
#endif

#if (UART_INTERRUPT_TESTBED == CURRENT_TESTBED)
  testbed_uart_interrupt();
#endif
  
#if (SPI_MASTER_TESTBED == CURRENT_TESTBED)
  testbed_spi_master();
#endif
  
#if (POWER_MODE_TESTBED == CURRENT_TESTBED)
  testbed_power_mode();
#endif
  
#if (SYSTICK_TESTBED == CURRENT_TESTBED)
  testbed_systick();
#endif  
  
#if (WDT_TESTBED == CURRENT_TESTBED)
  testbed_wdt();
#endif

#if (WDT_WARN_INTR_TESTBED == CURRENT_TESTBED)
  testbed_wdt_interrupt();
#endif  
  
#if (CHIPID_TESTBED == CURRENT_TESTBED)
  //testbed_chipID_sdl(); /* has not been implemented yet.*/
#endif
 
#if (FMC_TESTBED == CURRENT_TESTBED)
  testbed_fmc();
#endif
  
#if (SIMPLE_BOOTLOADER_TESTBED == CURRENT_TESTBED)
  testbed_simple_bootloader();
#endif  
  
#if (SYSTEM_RESET_CAUSE_TESTBED == CURRENT_TESTBED)
  testbed_sys_reset_cause();
#endif
  
#if (CRYPTO_CRC_TESTBED  == CURRENT_TESTBED)
  testbed_crypto_crc();
#endif  
  
#if (SILICON_ID_TESTBED == CURRENT_TESTBED)
  testbed_silicon_id();
#endif
  
#if (M4_TESTBED == CURRENT_TESTBED)
  /* Enable CM4. CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
  Cy_SysEnableApplCore(CY_CORTEX_M4_APPL_ADDR); 
  for (;;)
  {
     Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
  }
#endif
  
  printf("NO TESTBED SELECTED\n");
  for (;;);
}

 
/*************************** End of file ****************************/
