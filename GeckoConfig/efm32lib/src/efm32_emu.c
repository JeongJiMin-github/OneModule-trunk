/***************************************************************************//**
 * @file
 * @brief Energy Management Unit (EMU) Peripheral API for EFM32
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

#include "efm32_emu.h"
#include "efm32_cmu.h"
#include "efm32_assert.h"

/***************************************************************************//**
 * @addtogroup EFM32_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup EMU
 * @brief Energy Management Unit (EMU) Peripheral API for EFM32
 * @{
 ******************************************************************************/

/* Consistency check, since restoring assumes similar bitpositions in */
/* CMU OSCENCMD and STATUS regs */
#if (CMU_STATUS_AUXHFRCOENS != CMU_OSCENCMD_AUXHFRCOEN)
#error Conflict in AUXHFRCOENS and AUXHFRCOEN bitpositions
#endif
#if (CMU_STATUS_HFXCOENS != CMU_OSCENCMD_HFXCOEN)
#error Conflict in HFXOENS and HFXOEN bitpositions
#endif
#if (CMU_STATUS_LFRCOENS != CMU_OSCENCMD_LFRCOEN)
#error Conflict in LFRCOENS and LFRCOEN bitpositions
#endif
#if (CMU_STATUS_LFXCOENS != CMU_OSCENCMD_LFXCOEN)
#error Conflict in LFXOENS and LFXOEN bitpositions
#endif

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/**
 * CMU configured oscillator selection and oscillator enable status. When a
 * user configures oscillators, this varaiable shall shadow the configuration.
 * It is used by the EMU module in order to be able to restore the oscillator
 * config after having been in certain energy modes (since HW may automatically
 * alter config when going into an energy mode). It is the responsibility of
 * the CMU module to keep it up-to-date (or a user if not using the CMU API
 * for oscillator control).
 */
static uint16_t cmuStatus;
/** @endcond */


/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Restore oscillators and core clock after having been in EM2 or EM3.
 ******************************************************************************/
static void EMU_Restore(void)
{
  uint32_t cmuLocked;

  /* Although we could use the CMU API for most of the below handling, we */
  /* would like this function to be as efficient as possible. */

  /* CMU registers may be locked */
  cmuLocked = CMU->LOCK & CMU_LOCK_LOCKKEY_LOCKED;
  CMU_Unlock();

  /* AUXHFRCO was automatically disabled (except if using debugger). */
  /* HFXO was automatically disabled. */
  /* LFRCO/LFXO were possibly disabled by SW in EM3. */
  /* Restore according to status prior to entering EM. */
  CMU->OSCENCMD = cmuStatus & (CMU_STATUS_AUXHFRCOENS |
                               CMU_STATUS_HFXOENS |
                               CMU_STATUS_LFRCOENS |
                               CMU_STATUS_LFXOENS);

  /* Restore oscillator used for clocking core */
  switch (cmuStatus & (CMU_STATUS_HFXOSEL | CMU_STATUS_HFRCOSEL |
                       CMU_STATUS_LFXOSEL | CMU_STATUS_LFRCOSEL))
  {
  case CMU_STATUS_LFRCOSEL:
    /* Wait for LFRCO to stabilize */
    while (!(CMU->STATUS & CMU_STATUS_LFRCORDY))
      ;
    CMU->CMD = CMU_CMD_HFCLKSEL_LFRCO;
    break;

  case CMU_STATUS_LFXOSEL:
    /* Wait for LFXO to stabilize */
    while (!(CMU->STATUS & CMU_STATUS_LFXORDY))
      ;
    CMU->CMD = CMU_CMD_HFCLKSEL_LFXO;
    break;

  case CMU_STATUS_HFXOSEL:
    /* Wait for HFXO to stabilize */
    while (!(CMU->STATUS & CMU_STATUS_HFXORDY))
      ;
    CMU->CMD = CMU_CMD_HFCLKSEL_HFXO;
    break;

  default: /* CMU_STATUS_HFRCOSEL */
    /* If core clock was HFRCO core clock, it is automatically restored to */
    /* state prior to entering energy mode. No need for further action. */
    break;
  }

  /* Restore CMU register locking */
  if (cmuLocked)
  {
    CMU_Lock();
  }
}

/** @endcond */


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Enter energy mode 2 (EM2).
 *
 * @details
 *   When entering EM2, the high frequency clocks are disabled, ie HFXO, HFRCO
 *   and AUXHFRCO (for AUXHFRCO, see exception note below). When re-entering
 *   EM0, HFRCO is re-enabled and the core will be clocked by the configured
 *   HFRCO band. This ensures a quick wakeup from EM2.
 *
 *   However, prior to entering EM2, the core may have been using another
 *   oscillator than HFRCO. The @p restore parameter gives the user the option
 *   to restore all HF oscillators according to state prior to entering EM2,
 *   as well as the clock used to clock the core. This restore procedure is
 *   handled by SW. However, since handled by SW, it will not be restored
 *   before completing the interrupt function(s) waking up the core!
 *
 * @note
 *   If restoring core clock to use the HFXO oscillator, which has been
 *   disabled during EM2 mode, this function will stall until the oscillator
 *   has stabilized. Stalling time can be reduced by adding interrupt
 *   support detecting stable oscillator, and an asynchronous switch to the
 *   original oscillator. See CMU documentation. Such a feature is however
 *   outside the scope of the implementation in this function.
 * @par
 *   If HFXO is re-enabled by this function, and NOT used to clock the core,
 *   this function will not wait for HFXO to stabilize. This must be considered
 *   by the application if trying to use features relying on that oscillator
 *   upon return.
 * @par
 *   If a debugger is attached, the AUXHFRCO will not be disabled if enabled
 *   upon entering EM2. It will thus remain enabled when returning to EM0
 *   regardless of the @p restore parameter.
 *
 * @param[in] restore
 *   @li true - restore oscillators and clocks, see function details.
 *   @li false - do not restore oscillators and clocks, see function details.
 * @par
 *   The @p restore option should only be used if all clock control is done
 *   via the CMU API.
 ******************************************************************************/
void EMU_EnterEM2(bool restore)
{
  /* Auto-update CMU status just in case before entering energy mode. */
  /* This variable is normally kept up-to-date by the CMU API. */
  cmuStatus = (uint16_t)(CMU->STATUS);

  /* Enter Cortex-M3 deep sleep mode */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();

  /* Restore oscillators/clocks if specified */
  if (restore)
  {
    EMU_Restore();
  }
  /* If not restoring, and original clock was not HFRCO, we have to */
  /* update CMSIS core clock variable since core clock has changed */
  /* to using HFRCO. */
  else if (!(cmuStatus & CMU_STATUS_HFRCOSEL))
  {
    SystemCoreClockUpdate();
  }
}


/***************************************************************************//**
 * @brief
 *   Enter energy mode 3 (EM3).
 *
 * @details
 *   When entering EM3, the high frequency clocks are disabled by HW, ie HFXO,
 *   HFRCO and AUXHFRCO (for AUXHFRCO, see exception note below). In addition,
 *   the low frequency clocks, ie LFXO and LFRCO are disabled by SW. When
 *   re-entering EM0, HFRCO is re-enabled and the core will be clocked by the
 *   configured HFRCO band. This ensures a quick wakeup from EM3.
 *
 *   However, prior to entering EM3, the core may have been using another
 *   oscillator than HFRCO. The @p restore parameter gives the user the option
 *   to restore all HF/LF oscillators according to state prior to entering EM3,
 *   as well as the clock used to clock the core. This restore procedure is
 *   handled by SW. However, since handled by SW, it will not be restored
 *   before completing the interrupt function(s) waking up the core!
 *
 * @note
 *   If restoring core clock to use an oscillator other than HFRCO, this
 *   function will stall until the oscillator has stabilized. Stalling time
 *   can be reduced by adding interrupt support detecting stable oscillator,
 *   and an asynchronous switch to the original oscillator. See CMU
 *   documentation. Such a feature is however outside the scope of the
 *   implementation in this function.
 * @par
 *   If HFXO/LFXO/LFRCO are re-enabled by this function, and NOT used to clock
 *   the core, this function will not wait for those oscillators to stabilize.
 *   This must be considered by the application if trying to use features
 *   relying on those oscillators upon return.
 * @par
 *   If a debugger is attached, the AUXHFRCO will not be disabled if enabled
 *   upon entering EM3. It will thus remain enabled when returning to EM0
 *   regardless of the @p restore parameter.
 *
 * @param[in] restore
 *   @li true - restore oscillators and clocks, see function details.
 *   @li false - do not restore oscillators and clocks, see function details.
 * @par
 *   The @p restore option should only be used if all clock control is done
 *   via the CMU API.
 ******************************************************************************/
void EMU_EnterEM3(bool restore)
{
  uint32_t cmuLocked;

  /* Auto-update CMU status just in case before entering energy mode. */
  /* This variable is normally kept up-to-date by the CMU API. */
  cmuStatus = (uint16_t)(CMU->STATUS);

  /* CMU registers may be locked */
  cmuLocked = CMU->LOCK & CMU_LOCK_LOCKKEY_LOCKED;
  CMU_Unlock();

  /* Disable LF oscillators */
  CMU->OSCENCMD = CMU_OSCENCMD_LFXODIS | CMU_OSCENCMD_LFRCODIS;

  /* Restore CMU register locking */
  if (cmuLocked)
  {
    CMU_Lock();
  }

  /* Enter Cortex-M3 deep sleep mode */
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
  __WFI();

  /* Restore oscillators/clocks if specified */
  if (restore)
  {
    EMU_Restore();
  }
  /* If not restoring, and original clock was not HFRCO, we have to */
  /* update CMSIS core clock variable since core clock has changed */
  /* to using HFRCO. */
  else if (!(cmuStatus & CMU_STATUS_HFRCOSEL))
  {
    SystemCoreClockUpdate();
  }
}


/***************************************************************************//**
 * @brief
 *   Enter energy mode 4 (EM4).
 *
 * @note
 *   Only a power on reset or external reset pin can wake the device from EM4.
 ******************************************************************************/
void EMU_EnterEM4(void)
{
  int i;

  /* Make sure register write lock is disabled */
  EMU->LOCK = EMU_LOCK_LOCKKEY_UNLOCK;

  for (i = 0; i < 4; i++)
  {
    EMU->CTRL = (2 << _EMU_CTRL_EM4CTRL_SHIFT);
    EMU->CTRL = (3 << _EMU_CTRL_EM4CTRL_SHIFT);
  }
  EMU->CTRL = (2 << _EMU_CTRL_EM4CTRL_SHIFT);
}


/***************************************************************************//**
 * @brief
 *   Power down memory block.
 *
 * @param[in] blocks
 *   Specifies a logical OR of bits indicating memory blocks to power down.
 *   Bit 0 selects block 1, bit 1 selects block 2, etc. Memory block 0 cannot
 *   be disabled. Please refer to the EFM32 reference manual for available
 *   memory blocks for a device.
 *
 * @note
 *   Only a reset can make the specified memory block(s) available for use
 *   after having been powered down. Function will be void for devices not
 *   supporting this feature.
 ******************************************************************************/
void EMU_MemPwrDown(uint32_t blocks)
{
#if defined(_EMU_MEMCTRL_RESETVALUE)
  EFM_ASSERT(blocks <= _EMU_MEMCTRL_MASK);

  EMU->MEMCTRL = blocks;
#else
  (void)blocks;
#endif
}


/***************************************************************************//**
 * @brief
 *   Update EMU module with CMU oscillator selection/enable status.
 *
 * @details
 *   When entering EM2 and EM3, the HW may change the core clock oscillator
 *   used, as well as disabling some oscillators. The user may optionally select
 *   to restore the oscillators after waking up from EM2 and EM3 through the
 *   SW API.
 *
 *   However, in order to support this in a safe way, the EMU module must
 *   be kept up-to-date on the actual selected configuration. The CMU
 *   module must keep the EMU module up-to-date.
 *
 *   This function is mainly intended for internal use by the CMU module,
 *   but if the applications changes oscillator configurations without
 *   using the CMU API, this function can be used to keep the EMU module
 *   up-to-date.
 ******************************************************************************/
void EMU_UpdateOscConfig(void)
{
  /* Fetch current configuration */
  cmuStatus = (uint16_t)(CMU->STATUS);
}


/** @} (end addtogroup EMU) */
/** @} (end addtogroup EFM32_Library) */
