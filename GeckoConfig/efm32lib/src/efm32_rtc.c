/***************************************************************************//**
 * @file
 * @brief Real Time Counter (RTC) Peripheral API for EFM32
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

#include "efm32_rtc.h"
#include "efm32_assert.h"
#include "efm32_bitband.h"

/***************************************************************************//**
 * @addtogroup EFM32_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup RTC
 * @brief Real Time Counter (RTC) Peripheral API for EFM32
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Validation of valid comparator register for assert statements. */
#define RTC_COMP_REG_VALID(reg)    (((reg) <= 1))

/** @endcond */


/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

#if defined(_EFM32_GECKO_FAMILY)
/***************************************************************************//**
 * @brief
 *   Wait for ongoing sync of register(s) to low frequency domain to complete.
 *
 * @note
 *   This only applies to the Gecko Family, see the reference manual
 *   chapter about Access to Low Energy Peripherals (Asynchronos Registers)
 *   for details. For Tiny Gecko and Giant Gecko, the RTC supports immediate
 *   updates of registers, and will automatically hold the bus until the
 *   register has been updated.
 *
 * @param[in] mask
 *   Bitmask corresponding to SYNCBUSY register defined bits, indicating
 *   registers that must complete any ongoing synchronization.
 ******************************************************************************/
static __INLINE void RTC_Sync(uint32_t mask)
{
  /* Avoid deadlock if modifying the same register twice when freeze mode is */
  /* activated. */
  if (RTC->FREEZE & RTC_FREEZE_REGFREEZE)
    return;

  /* Wait for any pending previous write operation to have been completed */
  /* in low frequency domain. This is only required for the Gecko Family */
  while (RTC->SYNCBUSY & mask)
    ;
}
#endif

/** @endcond */

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Get RTC compare register value.
 *
 * @param[in] comp
 *   Compare register to get, either 0 or 1
 *
 * @return
 *   Compare register value, 0 if invalid register selected.
 ******************************************************************************/
uint32_t RTC_CompareGet(unsigned int comp)
{
  uint32_t ret;

  EFM_ASSERT(RTC_COMP_REG_VALID(comp));

  /* Initialize selected compare value */
  switch (comp)
  {
  case 0:
    ret = RTC->COMP0;
    break;

  case 1:
    ret = RTC->COMP1;
    break;

  default:
    /* Unknown compare register selected */
    ret = 0;
    break;
  }

  return ret;
}


/***************************************************************************//**
 * @brief
 *   Set RTC compare register value.
 *
 * @note
 *   The setting of a compare register requires synchronization into the
 *   low frequency domain. If the same register is modified before a previous
 *   update has completed, this function will stall until the previous
 *   synchronization has completed. This only applies to the Gecko Family, see
 *   comment in the RTC_Sync() internal function call.
 *
 * @param[in] comp
 *   Compare register to set, either 0 or 1
 *
 * @param[in] value
 *   Initialization value (<= 0x00ffffff)
 ******************************************************************************/
void RTC_CompareSet(unsigned int comp, uint32_t value)
{
  volatile uint32_t *compReg;
#if defined(_EFM32_GECKO_FAMILY)
  uint32_t          syncbusy;
#endif

  EFM_ASSERT(RTC_COMP_REG_VALID(comp) &&
             ((value & ~(_RTC_COMP0_COMP0_MASK >> _RTC_COMP0_COMP0_SHIFT)) == 0));

  /* Initialize selected compare value */
  switch (comp)
  {
  case 0:
    compReg = &(RTC->COMP0);
#if defined(_EFM32_GECKO_FAMILY)
    syncbusy = RTC_SYNCBUSY_COMP0;
#endif
    break;

  case 1:
    compReg = &(RTC->COMP1);
#if defined(_EFM32_GECKO_FAMILY)
    syncbusy = RTC_SYNCBUSY_COMP1;
#endif
    break;

  default:
    /* Unknown compare register selected, abort */
    return;
  }
#if defined(_EFM32_GECKO_FAMILY)
  /* LF register about to be modified require sync. busy check */
  RTC_Sync(syncbusy);
#endif

  *compReg = value;
}


/***************************************************************************//**
 * @brief
 *   Enable/disable RTC.
 *
 * @note
 *   The enabling/disabling of the RTC modifies the RTC CTRL register which
 *   requires synchronization into the low frequency domain. If this register is
 *   modified before a previous update to the same register has completed, this
 *   function will stall until the previous synchronization has completed. This
 *   only applies to the Gecko Family, see comment in the RTC_Sync() internal
 *   function call.
 *
 * @param[in] enable
 *   true to enable counting, false to disable.
 ******************************************************************************/
void RTC_Enable(bool enable)
{
#if defined(_EFM32_GECKO_FAMILY)
  /* LF register about to be modified require sync. busy check */
  RTC_Sync(RTC_SYNCBUSY_CTRL);
#endif

  BITBAND_Peripheral(&(RTC->CTRL), _RTC_CTRL_EN_SHIFT, (unsigned int) enable);
}


/***************************************************************************//**
 * @brief
 *   RTC register synchronization freeze control.
 *
 * @details
 *   Some RTC registers require synchronization into the low frequency (LF)
 *   domain. The freeze feature allows for several such registers to be
 *   modified before passing them to the LF domain simultaneously (which
 *   takes place when the freeze mode is disabled).
 *
 * @note
 *   When enabling freeze mode, this function will wait for all current
 *   ongoing RTC synchronization to LF domain to complete (Normally
 *   synchronization will not be in progress.) However for this reason, when
 *   using freeze mode, modifications of registers requiring LF synchronization
 *   should be done within one freeze enable/disable block to avoid unecessary
 *   stalling. This only applies to the Gecko Family, see the reference manual
 *   chapter about Access to Low Energy Peripherals (Asynchronos Registers)
 *   for details.
 *
 * @param[in] enable
 *   @li true - enable freeze, modified registers are not propagated to the
 *       LF domain
 *   @li false - disables freeze, modified registers are propagated to LF
 *       domain
 ******************************************************************************/
void RTC_FreezeEnable(bool enable)
{
  if (enable)
  {
#if defined(_EFM32_GECKO_FAMILY)
    /* Wait for any ongoing LF synchronization to complete. This is just to */
    /* protect against the rare case when a user                            */
    /* - modifies a register requiring LF sync                              */
    /* - then enables freeze before LF sync completed                       */
    /* - then modifies the same register again                              */
    /* since modifying a register while it is in sync progress should be    */
    /* avoided.                                                             */
    while (RTC->SYNCBUSY)
      ;
#endif
    RTC->FREEZE = RTC_FREEZE_REGFREEZE;
  }
  else
  {
    RTC->FREEZE = 0;
  }
}


/***************************************************************************//**
 * @brief
 *   Initialize RTC.
 *
 * @details
 *   Note that the compare values must be set separately with RTC_CompareSet().
 *   That should probably be done prior to the use of this function if
 *   configuring the RTC to start when initialization is completed.
 *
 * @note
 *   The initialization of the RTC modifies the RTC CTRL register which requires
 *   synchronization into the low frequency domain. If this register is
 *   modified before a previous update to the same register has completed, this
 *   function will stall until the previous synchronization has completed. This
 *   only applies to the Gecko Family, see comment in the RTC_Sync() internal
 *   function call.
 *
 * @param[in] init
 *   Pointer to RTC initialization structure.
 ******************************************************************************/
void RTC_Init(const RTC_Init_TypeDef *init)
{
  uint32_t tmp;

  if (init->enable)
  {
    tmp = RTC_CTRL_EN;
  }
  else
  {
    tmp = 0;
  }

  /* Configure DEBUGRUN flag, sets whether or not counter should be
   * updated when debugger is active */
  if (init->debugRun)
  {
    tmp |= RTC_CTRL_DEBUGRUN;
  }

  /* Configure COMP0TOP, this will use the COMP0 compare value as an
   * overflow value, instead of default 24-bit 0x00ffffff */
  if (init->comp0Top)
  {
    tmp |= RTC_CTRL_COMP0TOP;
  }

#if defined(_EFM32_GECKO_FAMILY)
  /* LF register about to be modified require sync. busy check */
  RTC_Sync(RTC_SYNCBUSY_CTRL);
#endif

  RTC->CTRL = tmp;
}



/***************************************************************************//**
 * @brief
 *   Restore RTC to reset state
 ******************************************************************************/
void RTC_Reset(void)
{
  /* Restore all essential RTC register to default config */
  RTC->FREEZE = _RTC_FREEZE_RESETVALUE;
  RTC->CTRL   = _RTC_CTRL_RESETVALUE;
  RTC->COMP0  = _RTC_COMP0_RESETVALUE;
  RTC->COMP1  = _RTC_COMP1_RESETVALUE;
  RTC->IEN    = _RTC_IEN_RESETVALUE;
  RTC->IFC    = _RTC_IFC_RESETVALUE;
}



/***************************************************************************//**
 * @brief
 *   Restart RTC counter from zero
 ******************************************************************************/
void RTC_CounterReset(void)
{
  /* A disable/enable sequnce will start the counter at zero */
  RTC_Enable(false);
  RTC_Enable(true);
}


/** @} (end addtogroup RTC) */
/** @} (end addtogroup EFM32_Library) */
