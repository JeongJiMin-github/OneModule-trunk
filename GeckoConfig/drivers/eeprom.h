/***************************************************************************//**
 * @file
 * @brief EEPROM driver for 24AA024 (2Kbit) EEPROM device on the DVK.
 * @author Energy Micro AS
 * @version 1.6.1
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2010 Energy Micro AS, http://www.energymicro.com</b>
 *******************************************************************************
 *
 * This source code is the property of Energy Micro AS. The source and compiled
 * code may only be used on Energy Micro "EFM32" microcontrollers.
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
 ******************************************************************************/

#ifndef __EEPROM_H
#define __EEPROM_H

#include "efm32.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** I2C device address for EEPROM on DVK */
#define EEPROM_DVK_ADDR      0xA0


/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

int EEPROM_Read(I2C_TypeDef *i2c,
                uint8_t addr,
                unsigned int offset,
                uint8_t *data,
                unsigned int len);
int EEPROM_Write(I2C_TypeDef *i2c,
                 uint8_t addr,
                 unsigned int offset,
                 uint8_t *data,
                 unsigned int len);

#ifdef __cplusplus
}
#endif

#endif /* __EEPROM_H */
