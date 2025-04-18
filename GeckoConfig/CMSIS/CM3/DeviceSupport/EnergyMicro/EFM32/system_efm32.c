/***************************************************************************//**
 * @file
 * @brief CMSIS Cortex-M3 Peripheral Access Layer for EFM32 devices.
 * @author Energy Micro AS
 * @version 2.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2011 Energy Micro AS, http://www.energymicro.com</b>
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

#include <stdint.h>
#include "efm32.h"
#include "efm32_chip.h"

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

/** LFRCO frequency, tuned to below frequency during manufacturing. */
#define EFM32_LFRCO_FREQ  (32768UL)
#define EFM32_ULFRCO_FREQ (1000UL)

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/

/* System oscillator frequencies. These frequencies are normally constant */
/* for a target, but they are made configurable in order to allow run-time */
/* handling of different boards. The crystal oscillator clocks can be set */
/* compile time to a non-default value by defining respective EFM32_nFXO_FREQ */
/* values according to board design. By defining the EFM32_nFXO_FREQ to 0, */
/* one indicates that the oscillator is not present, in order to save some */
/* SW footprint. */

#ifndef EFM32_HFXO_FREQ
#define EFM32_HFXO_FREQ (32000000UL)
#endif
/* Do not define variable if HF crystal oscillator not present */
#if (EFM32_HFXO_FREQ > 0)
/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/** System HFXO clock. */ 
static uint32_t SystemHFXOClock = EFM32_HFXO_FREQ;
/** @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
#endif

#ifndef EFM32_LFXO_FREQ 
#define EFM32_LFXO_FREQ (EFM32_LFRCO_FREQ)
#endif
/* Do not define variable if LF crystal oscillator not present */
#if (EFM32_LFXO_FREQ > 0)
/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */
/** System LFXO clock. */ 
static uint32_t SystemLFXOClock = EFM32_LFXO_FREQ;
/** @endcond (DO_NOT_INCLUDE_WITH_DOXYGEN) */
#endif


/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

/**
 * @brief
 *   System System Clock Frequency (Core Clock).
 *
 * @details
 *   Required CMSIS global variable that must be kept up-to-date.
 */
uint32_t SystemCoreClock;

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Get the current core clock frequency.
 *
 * @details
 *   Calculate and get the current core clock frequency based on the current
 *   configuration. Assuming that the SystemCoreClock global variable is
 *   maintained, the core clock frequency is stored in that variable as well.
 *   This function will however calculate the core clock based on actual HW
 *   configuration. It will also update the SystemCoreClock global variable.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   The current core clock frequency in Hz.
 ******************************************************************************/
uint32_t SystemCoreClockGet(void)
{
  uint32_t ret;
  
  ret = SystemHFClockGet();
  ret >>= (CMU->HFCORECLKDIV & _CMU_HFCORECLKDIV_HFCORECLKDIV_MASK) >> 
          _CMU_HFCORECLKDIV_HFCORECLKDIV_SHIFT;

  /* Keep CMSIS variable up-to-date just in case */
  SystemCoreClock = ret;

  return ret;
}


/***************************************************************************//**
 * @brief
 *   Get the current HFCLK frequency.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   The current HFCLK frequency in Hz.
 ******************************************************************************/
uint32_t SystemHFClockGet(void)
{
  SYSTEM_ChipRevision_TypeDef chipRev;
  uint32_t ret;
  
  SYSTEM_ChipRevisionGet(&chipRev);
  
  switch (CMU->STATUS & (CMU_STATUS_HFRCOSEL | CMU_STATUS_HFXOSEL |
                         CMU_STATUS_LFRCOSEL | CMU_STATUS_LFXOSEL))
  {
    case CMU_STATUS_LFXOSEL:
#if (EFM32_LFXO_FREQ > 0)
      ret = SystemLFXOClock;
#else
      /* We should not get here, since core should not be clocked. May */
      /* be caused by a misconfiguration though. */
      ret = 0;
#endif
      break;
      
    case CMU_STATUS_LFRCOSEL:
      ret = EFM32_LFRCO_FREQ;
      break;
      
    case CMU_STATUS_HFXOSEL:
#if (EFM32_HFXO_FREQ > 0)
      ret = SystemHFXOClock;
#else
      /* We should not get here, since core should not be clocked. May */
      /* be caused by a misconfiguration though. */
      ret = 0;
#endif
      break;
      
    default: /* CMU_STATUS_HFRCOSEL */
      switch (CMU->HFRCOCTRL & _CMU_HFRCOCTRL_BAND_MASK)
      {
          case CMU_HFRCOCTRL_BAND_28MHZ:
            ret = 28000000;
            break;
    
          case CMU_HFRCOCTRL_BAND_21MHZ:
            ret = 21000000;
            break;
    
          case CMU_HFRCOCTRL_BAND_14MHZ:
            ret = 14000000;
            break;
    
          case CMU_HFRCOCTRL_BAND_11MHZ:
            ret = 11000000;
            break;
    
          case CMU_HFRCOCTRL_BAND_7MHZ:
            //cpu revision D & E : 7Mhz -> 6.6Mhz   
			//7Mhz로 설정 시 실질적으로 6.6Mhz로 동작  
            if ((chipRev.minor == 0x03) || (chipRev.minor == 0x04))
                ret = 6600000;
            else
                ret = 7000000;
            break;
    
          case CMU_HFRCOCTRL_BAND_1MHZ:
            //cpu revision D & E : 1Mhz -> 1.2Mhz             
            if ((chipRev.minor == 0x03) || (chipRev.minor == 0x04))
                ret = 1200000;
            else
                ret = 1000000;
            break;
    
          default:
            ret = 0;
            break;
      }
      break;
  }

  return ret;
}


/**************************************************************************//**
 * @brief
 *   Get high frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   HFXO frequency in Hz.
 *****************************************************************************/
uint32_t SystemHFXOClockGet(void)
{
  /* External crystal oscillator present? */
#if (EFM32_HFXO_FREQ > 0)
  return SystemHFXOClock;
#else
  return 0;
#endif
}


/**************************************************************************//**
 * @brief
 *   Set high frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This function is mainly provided for being able to handle target systems
 *   with different HF crystal oscillator frequencies run-time. If used, it
 *   should probably only be used once during system startup.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @param[in] freq
 *   HFXO frequency in Hz used for target.
 *****************************************************************************/
void SystemHFXOClockSet(uint32_t freq)
{
  /* External crystal oscillator present? */
#if (EFM32_HFXO_FREQ > 0)
  SystemHFXOClock = freq;

  /* Update core clock frequency if HFXO is used to clock core */
  if (CMU->STATUS & CMU_STATUS_HFXOSEL)
  {
    /* The function will update the global variable */
    SystemCoreClockGet();
  }
#else
  (void)freq; /* Unused parameter */
#endif
}


/**************************************************************************//**
 * @brief
 *   Initialize the system.
 *
 * @details
 *   Do required generic HW system init.
 *
 * @note
 *   This function is invoked during system init, before the main() routine
 *   and any data has been initialized. For this reason, it cannot do any
 *   initialization of variables etc.
 *****************************************************************************/
void SystemInit(void)
{
}


/**************************************************************************//**
 * @brief
 *   Get low frequency RC oscillator clock frequency for target system.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   LFRCO frequency in Hz.
 *****************************************************************************/
uint32_t SystemLFRCOClockGet(void)
{
  /* Currently we assume that this frequency is properly tuned during */
  /* manufacturing and is not changed after reset. If future requirements */
  /* for re-tuning by user, we can add support for that. */
  return EFM32_LFRCO_FREQ;
}


/**************************************************************************//**
 * @brief
 *   Get ultra low frequency RC oscillator clock frequency for target system.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   ULFRCO frequency in Hz.
 *****************************************************************************/
uint32_t SystemULFRCOClockGet(void)
{
  /* The ULFRCO frequency is not tuned, and can be very inaccurate */
  return EFM32_ULFRCO_FREQ;
}


/**************************************************************************//**
 * @brief
 *   Get low frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @return
 *   LFXO frequency in Hz.
 *****************************************************************************/
uint32_t SystemLFXOClockGet(void)
{
  /* External crystal oscillator present? */
#if (EFM32_LFXO_FREQ > 0)
  return SystemLFXOClock;
#else
  return 0;
#endif
}


/**************************************************************************//**
 * @brief
 *   Set low frequency crystal oscillator clock frequency for target system.
 *
 * @note
 *   This function is mainly provided for being able to handle target systems
 *   with different HF crystal oscillator frequencies run-time. If used, it
 *   should probably only be used once during system startup.
 *
 * @note
 *   This is an EFM32 proprietary function, not part of the CMSIS definition.
 *
 * @param[in] freq
 *   LFXO frequency in Hz used for target.
 *****************************************************************************/
void SystemLFXOClockSet(uint32_t freq)
{
  /* External crystal oscillator present? */
#if (EFM32_LFXO_FREQ > 0)
  SystemLFXOClock = freq;

  /* Update core clock frequency if LFXO is used to clock core */
  if (CMU->STATUS & CMU_STATUS_LFXOSEL)
  {
    /* The function will update the global variable */
    SystemCoreClockGet();
  }
#else
  (void)freq; /* Unused parameter */
#endif
}
