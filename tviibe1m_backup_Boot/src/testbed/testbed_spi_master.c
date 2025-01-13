/*

File    : testbed_spi_master.c
Purpose : SPI testbed

*/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_config.h"
#include <stdio.h>
#include "CYTVII_B_E_1M_KIT_PIN.h"

/** Please Refer to datasheet to check the SCB0 UART TX/RX pin*/
#define TX_PIN_UART     KIT_UART_TX_PIN /*EVB on board USB to UART*/ 
#define RX_PIN_UART     KIT_UART_RX_PIN                             
#define UART_SCB        KIT_UART_SCB                         

#define USER_BUTTON         KIT_BUTTON_PIN  /*IO_P7_0*/

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

#define SCB_SPI_BAUDRATE 125000ul  /** <---(125 KHz) Please set baudrate value of SPI you want */

#define TX_RX_BUF_SIZE   (2u)
/*Helper function*/
#define HAL_SPI_START(X) HAL_SPI_Active((X),FUNC_ENABLE)
#define HAL_SPI_STOP(X)  HAL_SPI_Active((X),FUNC_DISABLE)

/* Commands */
#define CMD_SET_OFF     (0u)
#define CMD_SET_RED     (1u)
#define CMD_SET_RG      (2u)
#define CMD_SET_GREEN   (3u)
#define CMD_SET_GB      (4u)
#define CMD_SET_BLUE    (5u)
#define CMD_SET_UNKNOWN (0xFFu)

/* Start and end of the packet markers */
#define PACKET_SOP      (0x01u)
#define PACKET_EOP      (0x17u)

/**
    SPI COMMAND TO LED SPI SLAVE
    PACKET_SOP 

*/

__packed typedef struct 
{
    const uint8_t u8SOF;
    const uint8_t u8Cmd;
    const uint8_t u8EOP;
}SPI_LED_CMD;

SPI_LED_CMD cmd_R_LED_ON = 
{
    .u8SOF = PACKET_SOP,
    .u8Cmd = CMD_SET_RED,
    .u8EOP = PACKET_EOP, 
};

SPI_LED_CMD cmd_G_LED_ON = 
{
    .u8SOF = PACKET_SOP,
    .u8Cmd = CMD_SET_GREEN,
    .u8EOP = PACKET_EOP, 
};

SPI_LED_CMD cmd_B_LED_ON = 
{
    .u8SOF = PACKET_SOP,
    .u8Cmd = CMD_SET_BLUE,
    .u8EOP = PACKET_EOP, 
};

SPI_LED_CMD cmd_LED_OFF = 
{
    .u8SOF = PACKET_SOP,
    .u8Cmd = CMD_SET_OFF,
    .u8EOP = PACKET_EOP, 
};

/* Packet size */
#define PACKET_SIZE      (3u)


void testbed_spi_master(void) 
{
    HAL_All_IRQ_Enable();
    
    /*Button GPIO config as input*/
    HAL_GPIO_Pin_Init(USER_BUTTON,&gtdPinInputConfig);
    Init_EVB_Uart();
    DEBUG_PF("Testbed SPI master mode\n");
    
    spi_config_t tsSPI_Master_Mode = 
    {
        .u8MISO_pin = KIT_SPI_MISO,       /**< SPI MISO pin*/
        .u8MOSI_pin = KIT_SPI_MOSI,       /**< SPI MOSI pin*/
        .u8CLK_Pin  = KIT_SPI_CLK,        /**< SPI CLK pin*/
        .u8CS_Pin   = KIT_SPI_SEL0,       /**< SPI CS pin*/
        .u16TransferTimeout = 100u,       /** (Transfer timeout) */
        .u32Mode      = SPI_MASTER,       /**< SPI master/ slave*/
        .u32SubMode   = SPI_MOTOROLA,     /**< Specifies the sub mode of SPI operation    ***/
        .u32SclkMode  = SPI_CPHA0_CPOL0,  /**< Clock is active low, data is changed on first edge ***/
        .u32DataWidth = 8u,               /**< The width of RX/TX data (valid range 4-16).*/
        .u8EnableMsbFirst = SPI_MSB_FIRST,/*** Enables the hardware to shift out the data element MSB first, otherwise, LSB first ***/
        
        .u32BaudRate = SCB_SPI_BAUDRATE,  /** Please set baudrate value of SPI you want */
    };
        
    uint8_t u8RXBuf[PACKET_SIZE];
    uint8_t u8Button;
    
    uint32_t u32Idx = 0;
    
    CY_ASSERT ( HAL_SPI_Init (KIT_SPI_SCB,&tsSPI_Master_Mode) == DRIVER_TRUE);
    CY_ASSERT ( HAL_SPI_START(KIT_SPI_SCB) == DRIVER_TRUE);
    
    for(;;)
    {
        HAL_GPIO_PinRead(USER_BUTTON,&u8Button); 
        if (0U == u8Button)
        {
            Cy_SysLib_Delay(120); /*Delay 100 msec , simple button debouncing*/
            HAL_GPIO_PinRead(USER_BUTTON,&u8Button);
        }
        else
        {
            Cy_SysLib_Delay(10);
            continue;
        }
        
        if (0U == u8Button /* Button Pressed*/) 
        {
            SPI_LED_CMD* pCurCmd;
            switch (u32Idx)
            {
              case 0:  pCurCmd = &cmd_R_LED_ON; break;
              case 1:  pCurCmd = &cmd_G_LED_ON; break;
              case 2:  pCurCmd = &cmd_B_LED_ON; break;
              case 3:  pCurCmd = &cmd_LED_OFF ; break;
              default: pCurCmd = &cmd_LED_OFF ; break;
            }
            
            DEBUG_PF("SPI Send Cmd %d\n",pCurCmd->u8Cmd);
            HAL_SPI_Transfer(KIT_SPI_SCB,(uint8_t*)pCurCmd,u8RXBuf,PACKET_SIZE);
              
            u32Idx++;
            if (u32Idx >= 4) u32Idx = 0;
        } 
    }
}

