/***************************************************************************//**
 * @file
 * @brief Flash controller module (MSC) peripheral API for EFM32
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
#ifndef __EFM32_MSC_H
#define __EFM32_MSC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "efm32.h"
#include "efm32_bitband.h"



/***************************************************************************//**
 * @addtogroup EFM32_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup MSC
 * @brief Flash controller (MSC) peripheral API for EFM32
 * @{
 ******************************************************************************/

/*******************************************************************************
 *************************   DEFINES   *****************************************
 ******************************************************************************/

/**
 * @brief
 *    The timeout used while waiting for the flash to become ready after
 *    a write. This number indicates the number of iterations to perform before
 *    issuing a timeout.
 * @note
 *    This timeout is set very large (in the order of 100x longer than
 *    necessary). This is to avoid any corner cases.
 *
 */
#define MSC_PROGRAM_TIMEOUT    10000000ul

/*******************************************************************************
 *************************   TYPEDEFS   ****************************************
 ******************************************************************************/

/** Return codes for writing/erasing the flash */
typedef enum
{
  mscReturnOk          = 0,  /**< Flash write/erase successful. */
  mscReturnInvalidAddr = -1, /**< Invalid address. Write to an address that is not flash. */
  mscReturnLocked      = -2, /**< Flash address is locked. */
  mscReturnTimeOut     = -3, /**< Timeout while writing to flash. */
  mscReturnUnaligned   = -4  /**< Unaligned access to flash. */
} msc_Return_TypeDef;


/*******************************************************************************
 *************************   PROTOTYPES   **************************************
 ******************************************************************************/

void MSC_Deinit(void);
void MSC_Init(void);

/***************************************************************************//**
 * @brief
 *    Clear one or more pending MSC interrupts.
 *
 * @param[in] flags
 *    Pending MSC intterupt source to clear. Use a bitwise logic OR combination
 *   of valid interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
static __INLINE void MSC_IntClear(uint32_t flags)
{
  MSC->IFC = flags;
}

/***************************************************************************//**
 * @brief
 *   Disable one or more MSC interrupts.
 *
 * @param[in] flags
 *   MSC interrupt sources to disable. Use a bitwise logic OR combination of
 *   valid interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
static __INLINE void MSC_IntDisable(uint32_t flags)
{
  MSC->IEN &= ~(flags);
}


/***************************************************************************//**
 * @brief
 *   Enable one or more MSC interrupts.
 *
 * @note
 *   Depending on the use, a pending interrupt may already be set prior to
 *   enabling the interrupt. Consider using MSC_IntClear() prior to enabling
 *   if such a pending interrupt should be ignored.
 *
 * @param[in] flags
 *   MSC interrupt sources to enable. Use a bitwise logic OR combination of
 *   valid interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
static __INLINE void MSC_IntEnable(uint32_t flags)
{
  MSC->IEN |= flags;
}


/***************************************************************************//**
 * @brief
 *   Get pending MSV interrupt flags.
 *
 * @note
 *   The event bits are not cleared by the use of this function.
 *
 * @return
 *   MSC interrupt sources pending. A bitwise logic OR combination of valid
 *   interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
static __INLINE uint32_t MSC_IntGet(void)
{
  return(MSC->IF);
}


/***************************************************************************//**
 * @brief
 *   Set one or more pending MSC interrupts from SW.
 *
 * @param[in] flags
 *   MSC interrupt sources to set to pending. Use a bitwise logic OR combination of
 *   valid interrupt flags for the MSC module (MSC_IF_nnn).
 ******************************************************************************/
static __INLINE void MSC_IntSet(uint32_t flags)
{
  MSC->IFS = flags;
}


#if _EFM32_TINY_FAMILY == 1
/***************************************************************************//**
 * @brief
 *   Starts measuring cache hit ratio.
 * @details
 *   This function starts the performance counters. It is defined inline to
 *   minimize the impact of this code on the measurement itself.
 ******************************************************************************/
static __INLINE void MSC_StartCacheMeasurement(void)
{
  /* Clear CMOF and CHOF to catch these later */
  MSC->IFC = MSC_IF_CHOF | MSC_IF_CMOF;

  /* Start performance counters */
  MSC->CMD = MSC_CMD_STARTPC;
}


/***************************************************************************//**
 * @brief
 *   Stops measuring the hit rate.
 * @note
 *   This function is defined inline to minimize the impact of this
 *   code on the measurement itself.
 *   This code only works for relatively short sections of code. If you wish
 *   to measure longer sections of code you need to implement a IRQ Handler for
 *   The CHOF and CMOF overflow interrupts. Theses overflows needs to be
 *   counted and included in the total.
 *   The functions can then be implemented as follows:
 * @verbatim
 * volatile uint32_t hitOverflows
 * volatile uint32_t missOverflows
 *
 * void MSC_IRQHandler(void)
 * {
 *   uint32_t flags;
 *   flags = MSC->IF;
 *   if (flags & MSC_IF_CHOF)
 *   {
 *      MSC->IFC = MSC_IF_CHOF;
 *      hitOverflows++;
 *   }
 *   if (flags & MSC_IF_CMOF)
 *   {
 *     MSC->IFC = MSC_IF_CMOF;
 *     missOverflows++;
 *   }
 * }
 *
 * void startPerformanceCounters(void)
 * {
 *   hitOverflows = 0;
 *   missOverflows = 0;
 *
 *   MSC_IntEnable(MSC_IF_CHOF | MSC_IF_CMOF);
 *   NVIC_EnableIRQ(MSC_IRQn);
 *
 *   MSC_StartCacheMeasurement();
 * }
 * @endverbatim
 * @return
 *   Returns -1 if there has been no cache accesses.
 *   Returns -2 if there has been an overflow in the performance counters.
 *   If not, it will return the percentage of hits versus misses.
 ******************************************************************************/
static __INLINE int32_t MSC_GetCacheMeasurement(void)
{
  int32_t total;
  /* Stop the counter before computing the hit-rate */
  MSC->CMD = MSC_CMD_STOPPC;

  /* Check for overflows in performance counters */
  if (MSC->IF & (MSC_IF_CHOF | MSC_IF_CMOF))
    return -2;

  /* Because the hits and misses are volatile, we need to split this up into
   * two statements to avoid a compiler warning regarding the order of volatile
   * accesses. */
  total  = MSC->CACHEHITS;
  total += MSC->CACHEMISSES;

  /* To avoid a division by zero. */
  if (total == 0)
    return -1;

  return (MSC->CACHEHITS * 100) / total;
}


/***************************************************************************//**
 * @brief
 *   Flush the contents of the instruction cache.
 ******************************************************************************/
static __INLINE void MSC_FlushCache(void)
{
  MSC->CMD = MSC_CMD_INVCACHE;
}


/***************************************************************************//**
 * @brief
 *   Enable or disable instruction cache functionality
 * @param[in] enable
 *   Enable instruction cache. Default is on.
 ******************************************************************************/
static __INLINE void MSC_EnableCache(bool enable)
{
  BITBAND_Peripheral(&(MSC->READCTRL), _MSC_READCTRL_IFCDIS_SHIFT, ~enable);
}


/***************************************************************************//**
 * @brief
 *   Enable or disable instruction cache functionality in IRQs
 * @param[in] enable
 *   Enable instruction cache. Default is on.
 ******************************************************************************/
static __INLINE void MSC_EnableCacheIRQs(bool enable)
{
  BITBAND_Peripheral(&(MSC->READCTRL), _MSC_READCTRL_ICCDIS_SHIFT, ~enable);
}


/***************************************************************************//**
 * @brief
 *   Enable or disable instruction cache flushing when writing to flash
 * @param[in] enable
 *   Enable automatic cache flushing. Default is on.
 ******************************************************************************/
static __INLINE void MSC_EnableAutoCacheFlush(bool enable)
{
  BITBAND_Peripheral(&(MSC->READCTRL), _MSC_READCTRL_AIDIS_SHIFT, ~enable);
}
#endif

#ifdef __CC_ARM  /* MDK-ARM compiler */
msc_Return_TypeDef MSC_WriteWord(uint32_t *address, void const *data, int numBytes);
msc_Return_TypeDef MSC_ErasePage(uint32_t *startAddress);
#endif /* __CC_ARM */

#ifdef __ICCARM__ /* IAR compiler */
__ramfunc msc_Return_TypeDef MSC_WriteWord(uint32_t *address, void const *data, int numBytes);
__ramfunc msc_Return_TypeDef MSC_ErasePage(uint32_t *startAddress);
#endif /* __ICCARM__ */

#ifdef __GNUC__  /* GCC based compilers */
#ifdef __CROSSWORKS_ARM  /* Rowley Crossworks */
msc_Return_TypeDef MSC_WriteWord(uint32_t *address, void const *data, int numBytes) __attribute__ ((section(".fast")));
msc_Return_TypeDef MSC_ErasePage(uint32_t *startAddress) __attribute__ ((section(".fast")));
#else /* Sourcery G++ */
msc_Return_TypeDef MSC_WriteWord(uint32_t *address, void const *data, int numBytes) __attribute__ ((section(".ram")));
msc_Return_TypeDef MSC_ErasePage(uint32_t *startAddress) __attribute__ ((section(".ram")));
#endif /* __GNUC__ */
#endif /* __CROSSWORKS_ARM */

/** @} (end addtogroup MSC) */
/** @} (end addtogroup EFM32_Library) */

#ifdef __cplusplus
}
#endif

#endif /* __EFM32_MSC_H */
