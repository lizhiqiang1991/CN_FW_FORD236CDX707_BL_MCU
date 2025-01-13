/***************************************************************************
*\file hal_spi_config.c 
****************************************************************************/

#include "cy_project.h"
#include "cy_device_headers.h"
#include "hal_common.h"
#include "hal_spi_def.h"
#include "hal_gpio_def.h"

#define SPI_TRANSER_TIMEOUT   (1000U)

spi_config_t gtdSPI_Master_Mode = 
{
    .u8MISO_pin = IO_NC,       /**< SPI MISO pin*/
    .u8MOSI_pin = IO_NC,       /**< SPI MOSI pin*/
    .u8CLK_Pin  = IO_NC,       /**< SPI CLK pin*/
    .u8CS_Pin   = IO_NC,       /**< SPI CS pin*/
    .u16TransferTimeout = SPI_TRANSER_TIMEOUT,       /** (Transfer timeout) */
    .u32Mode      = SPI_MASTER,       /**< SPI master/ slave*/
    .u32SubMode   = SPI_MOTOROLA,     /**< Specifies the sub mode of SPI operation    ***/
    .u32SclkMode  = SPI_CPHA0_CPOL0,  /**< Clock is active low, data is changed on first edge ***/
    .u32DataWidth = 8u,               /**< The width of RX/TX data (valid range 4-32).*/
    .u8EnableMsbFirst = SPI_MSB_FIRST,/*** Enables the hardware to shift out the data element MSB first, otherwise, LSB first ***/
    
    .u32BaudRate = 0u,  /** Please set baudrate value of SPI you want */
};

/*** End of file ***/

  