/**************************************************************************//**
 * @file
 * @brief LCD Controller header file
 * @author Energy Micro AS
 * @version 1.0.4
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2010 Energy Micro AS, http://www.energymicro.com</b>
 ******************************************************************************
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
 *****************************************************************************/

/** @addtogroup STK_DRIVERS
 *  @{
 */

/** @addtogroup LCDCONTROLLER
 *  @{
 */

#ifndef __LCDCONTROLLER_H
#define __LCDCONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

#include "efm32.h"
/* Range of symbols available on display */
typedef enum
{
  LCD_SYMBOL_GECKO,
  LCD_SYMBOL_ANT,
  LCD_SYMBOL_PAD0,
  LCD_SYMBOL_PAD1,
  LCD_SYMBOL_AM,
  LCD_SYMBOL_PM,
  LCD_SYMBOL_EFM32,
  LCD_SYMBOL_MINUS,
  LCD_SYMBOL_COL3,
  LCD_SYMBOL_COL5,
  LCD_SYMBOL_COL10,
  LCD_SYMBOL_DEGC,
  LCD_SYMBOL_DEGF,
  LCD_SYMBOL_MICROAMP,
  LCD_SYMBOL_MILLIAMP,
  LCD_SYMBOL_DP6,
  LCD_SYMBOL_DP10,
} lcdSymbol;

/* Regular functions */
void LCD_Init(bool useBoost);
void LCD_UseBoost(bool useBoost);
void LCD_IRQHandler(void);
void LCD_Disable(void);

void LCD_AllOff(void);
void LCD_AllOn(void);

void LCD_ARing(int anum, int on);
void LCD_Battery(int batteryLevel);
void LCD_EnergyMode(int em, int on);
void LCD_Number(int value);
void LCD_NumberOff(void);
void LCD_AlphaNumberOff(void);
void LCD_Symbol(lcdSymbol s, int on);
void LCD_Write(char *string);

#endif

/** @}
 */

/** @}
 */

