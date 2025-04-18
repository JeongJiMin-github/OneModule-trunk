/**
 * @section LICENSE
 ******************************************************************************
 *           <b>(C) Copyright 2009 Energy Micro AS, www.energymicro.com</b>
 ******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source/compiled
 * code may not be used, redistributed or modified without a written consent
 * from Energy Micro AS.
 *
 * This copyright notice may not be removed from the source code nor changed.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no
 * obligation to support this Software. Energy Micro AS is providing the
 * Software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Energy Micro AS will not be liable for any consequential, incidental, or
 * special damages, or any other relief, or for any claim by any third party,
 * arising from your use of this Software.
 *
 ******************************************************************************
 * @file dmdif_ssd2119_ebi.c
 * @brief Dot matrix display SSD2119 interface using EBI
 * @author Energy Micro AS
 ******************************************************************************
 */

#include <stdint.h>
#include "dmd_ssd2119_registers.h"
#include "dmd_ssd2119.h"
#include "dmdif_ssd2119_spi.h"
#include "dvk.h"

/* Local function prototypes */
static EMSTATUS setNextReg(uint8_t reg);

static uint32_t command_register;
static uint32_t data_register;

/**************************************************************************//**
*  @brief
*  Initializes the data interface to the LCD controller SSD2119
*
*
*  @param cmdRegAddr
*  The address in memory where data to the command register in the display
*  controller are written
*  @param dataRegAddr
*  The address in memory where data to the data register in the display
*  controller are written
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_init(uint32_t cmdRegAddr, uint32_t dataRegAddr)
{
  command_register = cmdRegAddr;
  data_register    = dataRegAddr;

  DVK_SPI_writeRegister( BC_BUS_CFG, BC_BUS_CFG_SPI );
  
  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Writes a value to a control register in the LCD controller
*
*  @param reg
*  The register that will be written to
*  @param data
*  The value to write to the register
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_writeReg(uint8_t reg, uint16_t data)
{
  setNextReg(reg);

  /* Write bits [15:8] of the data to bits [8:1] of the output lines */
  DVK_SPI_writeRegister( (uint16_t *) data_register, ((data & 0xff00) >> 8) << 1);

  /* Write bits [7:0] of the data to bits [8:1] of the output lines */
  DVK_SPI_writeRegister( (uint16_t *) data_register, (data & 0x00ff) << 1);

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Reads the device code of the LCD controller
*  DOESN'T WORK
*
*  @return
*  The device code of the LCD controller
******************************************************************************/
uint16_t DMDIF_readDeviceCode(void)
{
  uint16_t readData;
  uint16_t deviceCode;

  /* Reading from the oscillation control register gives the device code */
  setNextReg(DMD_SSD2119_DEVICE_CODE_READ);

  readData    = DVK_SPI_readRegister( (uint16_t *) data_register );
  deviceCode  = ((readData >> 1) & 0x00ff) << 8;
  readData    = DVK_SPI_readRegister( (uint16_t *) data_register );
  deviceCode |= ((readData >> 1) & 0x00ff);

  return deviceCode;
}

/**************************************************************************//**
*  @brief
*  Sends the data access command to the LCD controller to prepare for one or more
*  writes or reads using the DMDIF_writeData() and DMDIF_readData()
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_prepareDataAccess(void)
{
  setNextReg(DMD_SSD2119_ACCESS_DATA);

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Writes one pixel to the LCD controller. DMDIF_prepareDataAccess() needs to be
*  called before writing data using this function.
*
*  @param data
*  The color value of the pixel to be written in 18bpp format
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_writeData(uint32_t data)
{
  /* Write bits [17:9] of the pixel data to bits [8:0] on the output lines */
  DVK_SPI_writeRegister( (uint16_t *) data_register, (data & 0x0003FE00) >> 9);
  /* Write bits [8:0] of the pixel data to bits [8:0] on the output lines */
  DVK_SPI_writeRegister( (uint16_t *) data_register, (data & 0x000001FF));

  return DMD_OK;
}

/**************************************************************************//**
*  @brief
*  Reads a byte of data from the memory of the LCD controller.
*  DMDIF_prepareDataAccess() needs to be called before using this function.
*  DOESN'T WORK
*
*  @return
*  18bpp value of pixel
******************************************************************************/
uint32_t DMDIF_readData(void)
{
  uint32_t data;

  /* Read bits [17:9] of the pixel */
  data = DVK_SPI_readRegister( (uint16_t *) data_register ) << 9;
  /* Read bits [8:0] of the pixel */
  data |= DVK_SPI_readRegister( (uint16_t *) data_register );

  return data;
}

/**************************************************************************//**
*  @brief
*  Delays the program a given number of milliseconds
*
*  @param reg
*  Number of milliseconds to delay.
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
EMSTATUS DMDIF_delay(uint32_t ms)
{
  extern void Delay(uint32_t dlyTicks);

  Delay(ms);

  return DMD_OK;
}


/**************************************************************************//**
*  \brief
*  Sets the register in the LCD controller to write commands to
*
*  \param reg
*  The next register in the LCD controller to write to
*
*  @return
*  DMD_OK on success, otherwise error code
******************************************************************************/
static EMSTATUS setNextReg(uint8_t reg)
{
  uint16_t data;

  data = ((uint16_t) reg) << 1;

  /* First 9 bits is 0 */
  DVK_SPI_writeRegister( (uint16_t *) command_register, 0 );

  /* Write the register address to bits [8:1] in the index register */
  DVK_SPI_writeRegister( (uint16_t *) command_register, data );

  return DMD_OK;
}


