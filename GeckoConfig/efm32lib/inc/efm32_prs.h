/***************************************************************************//**
 * @file
 * @brief Peripheral Reflex System (PRS) peripheral API for EFM32.
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
#ifndef __EFM32_PRS_H
#define __EFM32_PRS_H

#include "efm32.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup EFM32_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup PRS
 * @{
 ******************************************************************************/

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** Edge detection type. */
typedef enum
{
  prsEdgeOff  = PRS_CH_CTRL_EDSEL_OFF,      /**< Leave signal as is. */
  prsEdgePos  = PRS_CH_CTRL_EDSEL_POSEDGE,  /**< Generate pules on positive edge. */
  prsEdgeNeg  = PRS_CH_CTRL_EDSEL_NEGEDGE,  /**< Generate pules on negative edge. */
  prsEdgeBoth = PRS_CH_CTRL_EDSEL_BOTHEDGES /**< Generate pules on both edges. */
} PRS_Edge_TypeDef;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Set level control bit for one or more channels.
 *
 * @details
 *   The level value for a channel is XORed with both the pulse possible issued
 *   by PRS_PulseTrigger() and the PRS input signal selected for the channel(s).
 *
 * @param[in] level
 *   Level to use for channels indicated by @p mask. Use logical OR combination
 *   of PRS_SWLEVEL_CHnLEVEL defines for channels to set high level, otherwise 0.
 *
 * @param[in] mask
 *   Mask indicating which channels to set level for. Use logical OR combination
 *   of PRS_SWLEVEL_CHnLEVEL defines.
 ******************************************************************************/
static __INLINE void PRS_LevelSet(uint32_t level, uint32_t mask)
{
  PRS->SWLEVEL = (PRS->SWLEVEL & ~mask) | (level & mask);
}


/***************************************************************************//**
 * @brief
 *   Trigger a high pulse (one HFPERCLK) for one or more channels.
 *
 * @details
 *   Setting a bit for a channel causes the bit in the register to remain high
 *   for one HFPERCLK cycle. The pulse is XORed with both the corresponding bit
 *   in PRS SWLEVEL register and the PRS input signal selected for the
 *   channel(s).
 *
 * @param[in] channels
 *   Logical ORed combination of channels to trigger a pulse for. Use
 *   PRS_SWPULSE_CHnPULSE defines.
 ******************************************************************************/
static __INLINE void PRS_PulseTrigger(uint32_t channels)
{
  PRS->SWPULSE = channels & _PRS_SWPULSE_MASK;
}

void PRS_SourceSignalSet(unsigned int ch,
                         uint32_t source,
                         uint32_t signal,
                         PRS_Edge_TypeDef edge);

#if ((defined _EFM32_TINY_FAMILY) || (defined _EFM32_GIANT_FAMILY))
void PRS_SourceAsyncSignalSet(unsigned int ch,
                              uint32_t source,
                              uint32_t signal);
#endif

/** @} (end addtogroup PRS) */
/** @} (end addtogroup EFM32_Library) */

#ifdef __cplusplus
}
#endif

#endif /* __EFM32_PRS_H */
