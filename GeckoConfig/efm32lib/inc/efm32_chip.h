/***************************************************************************//**
 * @file
 * @brief Chip Initialization API for EFM32
 * @author Energy Micro AS
 * @version 2.0.0
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
#ifndef __EFM32_CHIP_H
#define __EFM32_CHIP_H

#include "efm32.h"
#include "efm32_system.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup EFM32_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup CHIP
 * @brief Chip Initialization API for EFM32
 * @{
 ******************************************************************************/

/**************************************************************************//**
 * @brief
 *   Chip initialization routine for revision errata workarounds
 *
 * This init function will configure the EFM32 device to a state where it is
 * as similar as later revisions as possible, to improve software compatibility
 * with newer parts. See the device specific errata for details.
 *****************************************************************************/
static __INLINE void CHIP_Init(void)
{
  uint32_t                    rev;
  SYSTEM_ChipRevision_TypeDef chipRev;
  volatile uint32_t           *reg;

  rev = *(volatile uint32_t *)(0x0FE081FC);
  /* Engineering Sample calibration setup */
  if ((rev >> 24) == 0)
  {
    reg   = (volatile uint32_t *)0x400CA00C;
    *reg &= ~(0x70UL);
    /* DREG */
    reg   = (volatile uint32_t *)0x400C6020;
    *reg &= ~(0xE0000000UL);
    *reg |= ~(7UL << 25);
  }
  if ((rev >> 24) <= 3)
  {
    /* DREG */
    reg   = (volatile uint32_t *)0x400C6020;
    *reg &= ~(0x00001F80UL);
    /* Update CMU reset values */
    reg  = (volatile uint32_t *)0x400C8040;
    *reg = 0;
    reg  = (volatile uint32_t *)0x400C8044;
    *reg = 0;
    reg  = (volatile uint32_t *)0x400C8058;
    *reg = 0;
    reg  = (volatile uint32_t *)0x400C8060;
    *reg = 0;
    reg  = (volatile uint32_t *)0x400C8078;
    *reg = 0;
  }

  SYSTEM_ChipRevisionGet(&chipRev);
  if (chipRev.major == 0x01)
  {
    /* Rev A errata handling for EM2/3. Must enable DMA clock in order for EM2/3 */
    /* to work. This will be fixed in later chip revisions, so only do for rev A. */
    if (chipRev.minor == 00)
    {
      reg   = (volatile uint32_t *)0x400C8040;
      *reg |= 0x2;
    }

    /* Rev A+B errata handling for I2C when using EM2/3. USART0 clock must be enabled */
    /* after waking up from EM2/EM3 in order for I2C to work. This will be fixed in */
    /* later chip revisions, so only do for rev A+B. */
    if (chipRev.minor <= 0x01)
    {
      reg   = (volatile uint32_t *)0x400C8044;
      *reg |= 0x1;
    }
  }
  /* Ensure correct ADC/DAC calibration value */
  rev = *(volatile uint32_t *)0x0FE081F0;
  if (rev < 0x4C8ABA00)
  {
    uint32_t cal;

    /* Enable ADC/DAC clocks */
    reg   = (volatile uint32_t *)0x400C8044UL;
    *reg |= (1 << 14 | 1 << 11);

    /* Retrive calibration values */
    cal = ((*(volatile uint32_t *)(0x0FE081B4UL) & 0x00007F00UL) >>
           8) << 24;

    cal |= ((*(volatile uint32_t *)(0x0FE081B4UL) & 0x0000007FUL) >>
            0) << 16;

    cal |= ((*(volatile uint32_t *)(0x0FE081B4UL) & 0x00007F00UL) >>
            8) << 8;

    cal |= ((*(volatile uint32_t *)(0x0FE081B4UL) & 0x0000007FUL) >>
            0) << 0;

    /* ADC0->CAL = 1.25 reference */
    reg  = (volatile uint32_t *)0x40002034UL;
    *reg = cal;

    /* DAC0->CAL = 1.25 reference */
    reg  = (volatile uint32_t *)(0x4000402CUL);
    cal  = *(volatile uint32_t *)0x0FE081C8UL;
    *reg = cal;

    /* Turn off ADC/DAC clocks */
    reg   = (volatile uint32_t *)0x400C8044UL;
    *reg &= ~(1 << 14 | 1 << 11);
  }
}

/** @} (end addtogroup SYSTEM) */
/** @} (end addtogroup EFM32_Library) */

#ifdef __cplusplus
}
#endif

#endif /* __EFM32_CHIP_H */
