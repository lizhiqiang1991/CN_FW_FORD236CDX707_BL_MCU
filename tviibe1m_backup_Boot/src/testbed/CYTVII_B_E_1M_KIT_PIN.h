#pragma once 
/*CYTVII-B-E-1M TRAVEOII STARTER KIT*/

/*USER LED*/
#define KIT_LED_1_BLUE   IO_P19_0
#define KIT_LED_4_BLUE   IO_P12_2

/*BUTTON */
#define KIT_BUTTON_PIN   IO_P7_0

/*I2C*/
#define KIT_I2C_SDA_PIN  IO_P7_1  /*SCB5_SDA , J16_3*/ 
#define KIT_I2C_SCL_PIN  IO_P7_2  /*SCB5_SCL , J15_3*/
#define KIT_I2C_SCB      (5)

/*UART*/
#define KIT_UART_RX_PIN  IO_P0_0  /*SCB0_RX*/ 
#define KIT_UART_TX_PIN  IO_P0_1  /*SCB0_TX*/ 
#define KIT_UART_SCB     (0)                         

/*SPI*/
#define KIT_SPI_MISO     IO_P22_0  /*SCB6_MISO J11_12*/
#define KIT_SPI_MOSI     IO_P22_1  /*SCB6_MOSI J11_11*/
#define KIT_SPI_CLK      IO_P22_2  /*SCB6_CLK  J11_13*/
#define KIT_SPI_SEL0     IO_P22_3  /*SCB6_SEL0 J11_10*/
#define KIT_SPI_SCB      (6)    


