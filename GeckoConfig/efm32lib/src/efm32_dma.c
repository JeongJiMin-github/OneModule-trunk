/***************************************************************************//**
 * @file
 * @brief Direct memory access (DMA) module peripheral API for EFM32
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

#include "efm32_dma.h"
#include "efm32_cmu.h"
#include "efm32_assert.h"
#include "efm32_bitband.h"

/***************************************************************************//**
 * @addtogroup EFM32_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup DMA
 * @brief Direct Memory Access (DMA) Peripheral API for EFM32
 * @details
 *  These DMA access functions provide basic support for the following
 *  types of DMA cycles:
 *
 *  @li @b Basic, used for transferring data between memory and peripherals.
 *  @li @b Auto-request, used for transferring data between memory locations.
 *  @li @b Ping-pong, used for for continuous transfer of data between memory
 *    and peripherals, automatically toggling between primary and alternate
 *    descriptors.
 *  @li @b Memory @b scatter-gather, used for transferring a number of buffers
 *    between memory locations.
 *  @li @b Peripheral @b scatter-gather, used for transferring a number of
 *    buffers between memory and peripherals.
 *
 *  A basic understanding of the DMA controller is assumed. Please refer to
 *  the EFM32 reference manual for further details.
 *
 *  The term 'descriptor' is used as a synonym to the 'channel control data
 *  structure' term.
 *
 *  In order to use the DMA controller, the initialization function must have
 *  been executed once (normally during system init):
 * @verbatim
 * DMA_Init();
 * @endverbatim
 *
 *  Then, normally a user of a DMA channel configures the channel:
 * @verbatim
 * DMA_CfgChannel();
 * @endverbatim
 *
 *  The channel configuration only has to be done once, if reusing the channel
 *  for the same purpose later.
 *
 *  In order to set up a DMA cycle, the primary and/or alternate descriptor
 *  has to be set up as indicated below.
 *
 *  For basic or auto-request cycles, use once on either primary or alternate
 *  descriptor:
 * @verbatim
 * DMA_CfgDescr();
 * @endverbatim
 *
 * For ping-pong cycles, configure both primary or alternate descriptors:
 * @verbatim
 * DMA_CfgDescr(); // Primary descriptor config
 * DMA_CfgDescr(); // Alternate descriptor config
 * @endverbatim
 *
 * For scatter-gather cycles, the alternate descriptor array must be programmed:
 * @verbatim
 * // 'n' is the number of scattered buffers
 * // 'descr' points to the start of the alternate descriptor array
 *
 * // Fill in 'cfg'
 * DMA_CfgDescrScatterGather(descr, 0, cfg);
 * // Fill in 'cfg'
 * DMA_CfgDescrScatterGather(descr, 1, cfg);
 * :
 * // Fill in 'cfg'
 * DMA_CfgDescrScatterGather(descr, n - 1, cfg);
 * @endverbatim
 *
 * In many cases, the descriptor configuration only has to be done once, if
 * re-using the channel for the same type of DMA cycles later.
 *
 * In order to activate the DMA cycle, use the respective DMA_Activate...()
 * function.
 *
 * For ping-pong DMA cycles, use DMA_RefreshPingPong() from the callback to
 * prepare the completed descriptor for reuse. Notice that the refresh must
 * be done prior to the other active descriptor completes, otherwise the
 * ping-pong DMA cycle will halt.
 * @{
 ******************************************************************************/

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/***************************************************************************//**
 * @brief
 *   Prepare descriptor for DMA cycle.
 *
 * @details
 *   This function prepares the last pieces of configuration required to start a
 *   DMA cycle. Since the DMA controller itself modifies some parts of the
 *   descriptor during use, those parts need to be refreshed if reusing a
 *   descriptor configuration.
 *
 * @note
 *   If using this function on a descriptor already activated and in use by the
 *   DMA controller, the behaviour is undefined.
 *
 * @param[in] channel
 *   DMA channel to prepare for DMA cycle.
 *
 * @param[in] cycleCtrl
 *   DMA cycle type to prepare for.
 *
 * @param[in] primary
 *   @li true - prepare primary descriptor
 *   @li false - prepare alternate descriptor
 *
 * @param[in] useBurst
 *   The burst feature is only used on peripherals supporting DMA bursts.
 *   Bursts must not be used if the total length (as given by nMinus1) is
 *   less than the arbitration rate configured for the descriptor. Please
 *   refer to the reference manual for further details on burst usage.
 *
 * @param[in] dst
 *   Address to start location to transfer data to. If NULL, leave setting in
 *   descriptor as is.
 *
 * @param[in] src
 *   Address to start location to transfer data from. If NULL, leave setting in
 *   descriptor as is.
 *
 * @param[in] nMinus1
 *   Number of elements (minus 1) to transfer (<= 1023).
 ******************************************************************************/
static void DMA_Prepare(unsigned int channel,
                        DMA_CycleCtrl_TypeDef cycleCtrl,
                        bool primary,
                        bool useBurst,
                        void *dst,
                        void *src,
                        unsigned int nMinus1)
{
  DMA_DESCRIPTOR_TypeDef *descr;
  DMA_DESCRIPTOR_TypeDef *primDescr;
  DMA_CB_TypeDef         *cb;
  uint32_t               inc;
  uint32_t               chBit;
  uint32_t               tmp;

  primDescr = ((DMA_DESCRIPTOR_TypeDef *)(DMA->CTRLBASE)) + channel;

  /* Find descriptor to configure */
  if (primary)
  {
    descr = primDescr;
  }
  else
  {
    descr = ((DMA_DESCRIPTOR_TypeDef *)(DMA->ALTCTRLBASE)) + channel;
  }

  /* If callback defined, update info on whether callback is issued */
  /* for primary or alternate descriptor. Mainly needed for ping-pong */
  /* cycles. */
  cb = (DMA_CB_TypeDef *)(primDescr->USER);
  if (cb)
  {
    cb->primary = (uint8_t)primary;
  }

  if (src)
  {
    inc = (descr->CTRL & _DMA_CTRL_SRC_INC_MASK) >> _DMA_CTRL_SRC_INC_SHIFT;
    if (inc == _DMA_CTRL_SRC_INC_NONE)
    {
      descr->SRCEND = src;
    }
    else
    {
      descr->SRCEND = (void *)((uint32_t)src + (nMinus1 << inc));
    }
  }

  if (dst)
  {
    inc = (descr->CTRL & _DMA_CTRL_DST_INC_MASK) >> _DMA_CTRL_DST_INC_SHIFT;
    if (inc == _DMA_CTRL_DST_INC_NONE)
    {
      descr->DSTEND = dst;
    }
    else
    {
      descr->DSTEND = (void *)((uint32_t)dst + (nMinus1 << inc));
    }
  }

  chBit = 1 << channel;
  if (useBurst)
  {
    DMA->CHUSEBURSTS = chBit;
  }
  else
  {
    DMA->CHUSEBURSTC = chBit;
  }

  if (primary)
  {
    DMA->CHALTC = chBit;
  }
  else
  {
    DMA->CHALTS = chBit;
  }

  /* Set cycle control */
  tmp         = descr->CTRL & ~(_DMA_CTRL_CYCLE_CTRL_MASK | _DMA_CTRL_N_MINUS_1_MASK);
  tmp        |= nMinus1 << _DMA_CTRL_N_MINUS_1_SHIFT;
  tmp        |= (uint32_t)cycleCtrl << _DMA_CTRL_CYCLE_CTRL_SHIFT;
  descr->CTRL = tmp;
}

/** @endcond */

/*******************************************************************************
 ************************   INTERRUPT FUNCTIONS   ******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Interrupt handler for DMA cycle completion handling.
 *
 * @details
 *   Clears any pending flags and calls registered callback (if any).
 *
 *   If using the default interrupt vector table setup provided, this function
 *   is automatically placed in the IRQ table due to weak linking. If taking
 *   control over the interrupt vector table in some other way, this interrupt
 *   handler must be installed in order to be able to support callback actions.
 ******************************************************************************/
void DMA_IRQHandler(void)
{
  DMA_DESCRIPTOR_TypeDef *descr = (DMA_DESCRIPTOR_TypeDef *)(DMA->CTRLBASE);
  int                    channel;
  DMA_CB_TypeDef         *cb;
  uint32_t               pending;
  uint32_t               pendingPrio;
  uint32_t               prio;
  uint32_t               primaryCpy;
  int                    i;

  /* Get all pending interrupts */
  pending = DMA->IF;

  /* Check for bus error */
  if (pending & DMA_IF_ERR)
  {
    /* Loop here to enable the debugger to see what has happened */
    while (1)
      ;
  }

  /* Process all pending channel interrupts. First process channels */
  /* defined with high priority, then those with default priority. */
  prio        = DMA->CHPRIS;
  pendingPrio = pending & prio;
  for (i = 0; i < 2; i++)
  {
    channel = 0;
    /* Process pending interrupts within high/default priority group */
    /* honouring priority within group. */
    while (pendingPrio)
    {
      if (pendingPrio & 1)
      {
        /* Clear pending interrupt prior to invoking callback, in case it */
        /* sets up another DMA cycle. */
        DMA->IFC = 1 << channel;

        /* Normally, no point in enabling interrupt without callback, but */
        /* check if callback is defined anyway. Callback info is always */
        /* located in primary descriptor. */
        cb = (DMA_CB_TypeDef *)(descr[channel].USER);
        if (cb)
        {
          /* Toggle next-descriptor indicator always prior to invoking */
          /* callback (in case callback reconfigurs something) */
          primaryCpy   = cb->primary;
          cb->primary ^= 1;
          if (cb->cbFunc)
          {
            cb->cbFunc(channel, (bool)primaryCpy, cb->userPtr);
          }
        }
      }

      pendingPrio >>= 1;
      channel++;
    }

    /* On second iteration, process default priority channels */
    pendingPrio = pending & ~prio;
  }
}


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Activate DMA auto-request cycle (used for memory-memory transfers).
 *
 * @details
 *   Prior to activating the DMA cycle, the channel and descriptor to be used
 *   must have been properly configured.
 *
 * @note
 *   If using this function on a channel already activated and in use by the
 *   DMA controller, the behaviour is undefined.
 *
 * @param[in] channel
 *   DMA channel to activate DMA cycle for.
 *
 * @param[in] primary
 *   @li true - activate using primary descriptor
 *   @li false - activate using alternate descriptor
 *
 * @param[in] dst
 *   Address to start location to transfer data to. If NULL, leave setting in
 *   descriptor as is from a previous activation.
 *
 * @param[in] src
 *   Address to start location to transfer data from. If NULL, leave setting in
 *   descriptor as is from a previous activation.
 *
 * @param[in] nMinus1
 *   Number of DMA transfer elements (minus 1) to transfer (<= 1023). The
 *   size of the DMA transfer element (1, 2 or 4 bytes) is configured with
 *   DMA_CfgDescr().
 ******************************************************************************/
void DMA_ActivateAuto(unsigned int channel,
                      bool primary,
                      void *dst,
                      void *src,
                      unsigned int nMinus1)
{
  uint32_t chBit;

  EFM_ASSERT(channel < DMA_CHAN_COUNT);
  EFM_ASSERT(nMinus1 <= (_DMA_CTRL_N_MINUS_1_MASK >> _DMA_CTRL_N_MINUS_1_SHIFT));

  DMA_Prepare(channel,
              dmaCycleCtrlAuto,
              primary,
              false,
              dst,
              src,
              nMinus1);

  chBit        = 1 << channel;
  DMA->CHENS   = chBit; /* Enable channel */
  DMA->CHSWREQ = chBit; /* Activate with SW request */
}


/***************************************************************************//**
 * @brief
 *   Activate DMA basic cycle (used for memory-peripheral transfers).
 *
 * @details
 *   Prior to activating the DMA cycle, the channel and descriptor to be used
 *   must have been properly configured.
 *
 * @note
 *   If using this function on a channel already activated and in use by the
 *   DMA controller, the behaviour is undefined.
 *
 * @param[in] channel
 *   DMA channel to activate DMA cycle for.
 *
 * @param[in] primary
 *   @li true - activate using primary descriptor
 *   @li false - activate using alternate descriptor
 *
 * @param[in] useBurst
 *   The burst feature is only used on peripherals supporting DMA bursts.
 *   Bursts must not be used if the total length (as given by nMinus1) is
 *   less than the arbitration rate configured for the descriptor. Please
 *   refer to the reference manual for further details on burst usage.
 *
 * @param[in] dst
 *   Address to start location to transfer data to. If NULL, leave setting in
 *   descriptor as is from a previous activation.
 *
 * @param[in] src
 *   Address to start location to transfer data from. If NULL, leave setting in
 *   descriptor as is from a previous activation.
 *
 * @param[in] nMinus1
 *   Number of DMA transfer elements (minus 1) to transfer (<= 1023). The
 *   size of the DMA transfer element (1, 2 or 4 bytes) is configured with
 *   DMA_CfgDescr().
 ******************************************************************************/
void DMA_ActivateBasic(unsigned int channel,
                       bool primary,
                       bool useBurst,
                       void *dst,
                       void *src,
                       unsigned int nMinus1)
{
  EFM_ASSERT(channel < DMA_CHAN_COUNT);
  EFM_ASSERT(nMinus1 <= (_DMA_CTRL_N_MINUS_1_MASK >> _DMA_CTRL_N_MINUS_1_SHIFT));

  DMA_Prepare(channel,
              dmaCycleCtrlBasic,
              primary,
              useBurst,
              dst,
              src,
              nMinus1);

  /* Enable channel, request signal is provided by peripheral device */
  DMA->CHENS = 1 << channel;
}


/***************************************************************************//**
 * @brief
 *   Activate DMA ping-pong cycle (used for memory-peripheral transfers).
 *
 * @details
 *   Prior to activating the DMA cycle, the channel and both descriptors must
 *   have been properly configured. The primary descriptor is always the first
 *   descriptor to be used by the DMA controller.
 *
 * @note
 *   If using this function on a channel already activated and in use by the
 *   DMA controller, the behaviour is undefined.
 *
 * @param[in] channel
 *   DMA channel to activate DMA cycle for.
 *
 * @param[in] useBurst
 *   The burst feature is only used on peripherals supporting DMA bursts.
 *   Bursts must not be used if the total length (as given by nMinus1) is
 *   less than the arbitration rate configured for the descriptors. Please
 *   refer to the reference manual for further details on burst usage. Notice
 *   that this setting is used for both the primary and alternate descriptors.
 *
 * @param[in] primDst
 *   Address to start location to transfer data to, for primary descriptor.
 *   If NULL, leave setting in descriptor as is from a previous activation.
 *
 * @param[in] primSrc
 *   Address to start location to transfer data from, for primary descriptor.
 *   If NULL, leave setting in descriptor as is from a previous activation.
 *
 * @param[in] primNMinus1
 *   Number of DMA transfer elements (minus 1) to transfer (<= 1023), for
 *   primary descriptor. The size of the DMA transfer element (1, 2 or 4 bytes)
 *   is configured with DMA_CfgDescr().
 *
 * @param[in] altDst
 *   Address to start location to transfer data to, for alternate descriptor.
 *   If NULL, leave setting in descriptor as is from a previous activation.
 *
 * @param[in] altSrc
 *   Address to start location to transfer data from, for alternate descriptor.
 *   If NULL, leave setting in descriptor as is from a previous activation.
 *
 * @param[in] altNMinus1
 *   Number of DMA transfer elements (minus 1) to transfer (<= 1023), for
 *   alternate descriptor. The size of the DMA transfer element (1, 2 or 4 bytes)
 *   is configured with DMA_CfgDescr().
 ******************************************************************************/
void DMA_ActivatePingPong(unsigned int channel,
                          bool useBurst,
                          void *primDst,
                          void *primSrc,
                          unsigned int primNMinus1,
                          void *altDst,
                          void *altSrc,
                          unsigned int altNMinus1)
{
  EFM_ASSERT(channel < DMA_CHAN_COUNT);
  EFM_ASSERT(primNMinus1 <= (_DMA_CTRL_N_MINUS_1_MASK >> _DMA_CTRL_N_MINUS_1_SHIFT));
  EFM_ASSERT(altNMinus1 <= (_DMA_CTRL_N_MINUS_1_MASK >> _DMA_CTRL_N_MINUS_1_SHIFT));

  /* Prepare alternate descriptor first */
  DMA_Prepare(channel,
              dmaCycleCtrlPingPong,
              false,
              useBurst,
              altDst,
              altSrc,
              altNMinus1);

  /* Prepare primary descriptor last in order to start cycle using it */
  DMA_Prepare(channel,
              dmaCycleCtrlPingPong,
              true,
              useBurst,
              primDst,
              primSrc,
              primNMinus1);

  /* Enable channel, request signal is provided by peripheral device */
  DMA->CHENS = 1 << channel;
}


/***************************************************************************//**
 * @brief
 *   Activate DMA scatter-gather cycle (used for either memory-peripheral
 *   or memory-memory transfers).
 *
 * @details
 *   Prior to activating the DMA cycle, the array with alternate descriptors
 *   must have been properly configured. This function can be reused without
 *   reconfiguring the alternate descriptors, as long as @p count is the same.
 *
 * @note
 *   If using this function on a channel already activated and in use by the
 *   DMA controller, the behaviour is undefined.
 *
 * @param[in] channel
 *   DMA channel to activate DMA cycle for.
 *
 * @param[in] useBurst
 *   The burst feature is only used on peripherals supporting DMA bursts
 *   (and thus this parameter is ignored for memory scatter-gather cycles).
 *   This parameter determines if bursts should be enabled during DMA transfers
 *   using the alternate descriptors. Bursts must not be used if the total
 *   length (as given by nMinus1 for the alternate descriptor) is
 *   less than the arbitration rate configured for the descriptor. Please
 *   refer to the reference manual for further details on burst usage.
 *
 * @param[in,out] altDescr
 *   Pointer to start of array with prepared alternate descriptors. The last
 *   descriptor will have its cycle control type reprogrammed to basic type.
 *
 * @param[in] count
 *   Number of alternate descriptors in @p altDescr array. Maximum number of
 *   alternate descriptors is 256.
 ******************************************************************************/
void DMA_ActivateScatterGather(unsigned int channel,
                               bool useBurst,
                               DMA_DESCRIPTOR_TypeDef *altDescr,
                               unsigned int count)
{
  DMA_DESCRIPTOR_TypeDef *descr;
  DMA_CB_TypeDef         *cb;
  uint32_t               cycleCtrl;
  uint32_t               chBit;

  EFM_ASSERT(channel < DMA_CHAN_COUNT);
  EFM_ASSERT(altDescr);
  EFM_ASSERT(count && (count <= 256));

  /* We have to configure the primary descriptor properly in order to */
  /* transfer one complete alternate descriptor from the alternate */
  /* descriptor table into the actual alternate descriptor. */
  descr = (DMA_DESCRIPTOR_TypeDef *)(DMA->CTRLBASE) + channel;

  /* Set source end address to point to alternate descriptor array */
  descr->SRCEND = (uint32_t *)altDescr + (count * 4) - 1;

  /* The destination end address in the primary descriptor MUST point */
  /* to the corresponding alternate descriptor in scatter-gather mode. */
  descr->DSTEND = (uint32_t *)((DMA_DESCRIPTOR_TypeDef *)(DMA->ALTCTRLBASE) +
                               channel + 1) - 1;

  /* The user field of the descriptor is used for callback configuration, */
  /* and already configured when channel is configured. Do not modify it. */

  /* Determine from alternate configuration whether this is a memory or */
  /* peripheral scatter-gather, by looking at the first alternate descriptor. */
  cycleCtrl  = altDescr->CTRL & _DMA_CTRL_CYCLE_CTRL_MASK;
  cycleCtrl &= ~(1 << _DMA_CTRL_CYCLE_CTRL_SHIFT);

  EFM_ASSERT((cycleCtrl == dmaCycleCtrlMemScatterGather) ||
             (cycleCtrl == dmaCycleCtrlPerScatterGather));

  /* Set last alternate descriptor to basic or auto-request cycle type in */
  /* order to have dma_done signal asserted when complete. Otherwise interrupt */
  /* will not be triggered when done. */
  altDescr[count - 1].CTRL &= ~_DMA_CTRL_CYCLE_CTRL_MASK;
  if (cycleCtrl == dmaCycleCtrlMemScatterGather)
  {
    altDescr[count - 1].CTRL |= (uint32_t)dmaCycleCtrlAuto << _DMA_CTRL_CYCLE_CTRL_SHIFT;
  }
  else
  {
    altDescr[count - 1].CTRL |= (uint32_t)dmaCycleCtrlBasic << _DMA_CTRL_CYCLE_CTRL_SHIFT;
  }

  /* If callback defined, update info on whether callback is issued for */
  /* primary or alternate descriptor. Not really useful for scatter-gather, */
  /* but do for consistency. Always set to alternate, since that is the last */
  /* descriptor actually used. */
  cb = (DMA_CB_TypeDef *)(descr->USER);
  if (cb)
  {
    cb->primary = false;
  }

  /* Configure primary descriptor control word */
  descr->CTRL =
    ((uint32_t)dmaDataInc4 << _DMA_CTRL_DST_INC_SHIFT) |
    ((uint32_t)dmaDataSize4 << _DMA_CTRL_DST_SIZE_SHIFT) |
    ((uint32_t)dmaDataInc4 << _DMA_CTRL_SRC_INC_SHIFT) |
    ((uint32_t)dmaDataSize4 << _DMA_CTRL_SRC_SIZE_SHIFT) |
    /* Use same protection scheme as for alternate descriptors */
    (altDescr->CTRL & _DMA_CTRL_SRC_PROT_CTRL_MASK) |
    ((uint32_t)dmaArbitrate4 << _DMA_CTRL_R_POWER_SHIFT) |
    (((count * 4) - 1) << _DMA_CTRL_N_MINUS_1_SHIFT) |
    (((uint32_t)useBurst & 1) << _DMA_CTRL_NEXT_USEBURST_SHIFT) |
    cycleCtrl;

  chBit = 1 << channel;

  /* Start with primary descriptor */
  DMA->CHALTC = chBit;

  /* Enable channel */
  DMA->CHENS = chBit;

  /* Send request if memory scatter-gather, otherwise request signal is */
  /* provided by peripheral. */
  if (cycleCtrl == dmaCycleCtrlMemScatterGather)
  {
    DMA->CHSWREQ = chBit;
  }
}


/***************************************************************************//**
 * @brief
 *   Configure a DMA channel.
 *
 * @details
 *   Configure miscellaneous issues for a DMA channel. This function is typically
 *   used once to setup a channel for a certain type of use.
 *
 * @note
 *   If using this function on a channel already in use by the DMA controller,
 *   the behaviour is undefined.
 *
 * @param[in] channel
 *   DMA channel to configure.
 *
 * @param[in] cfg
 *   Configuration to use.
 ******************************************************************************/
void DMA_CfgChannel(unsigned int channel, DMA_CfgChannel_TypeDef *cfg)
{
  DMA_DESCRIPTOR_TypeDef *descr;

  EFM_ASSERT(channel < DMA_CHAN_COUNT);
  EFM_ASSERT(cfg);

  /* Always keep callback configuration reference in primary descriptor */
  descr               = (DMA_DESCRIPTOR_TypeDef *)(DMA->CTRLBASE);
  descr[channel].USER = (uint32_t)(cfg->cb);

  /* Set to specified priority for channel */
  if (cfg->highPri)
  {
    DMA->CHPRIS = 1 << channel;
  }
  else
  {
    DMA->CHPRIC = 1 << channel;
  }

  /* Set DMA signal source select */
  DMA->CH[channel].CTRL = cfg->select;

  /* Enable/disable interrupt as specified */
  if (cfg->enableInt)
  {
    DMA->IFC = (1 << channel);
    BITBAND_Peripheral(&(DMA->IEN), channel, 1);
  }
  else
  {
    BITBAND_Peripheral(&(DMA->IEN), channel, 0);
  }
}


/***************************************************************************//**
 * @brief
 *   Configure DMA descriptor for auto-request, basic or ping-pong DMA cycles.
 *
 * @details
 *   This function is used for configuration of a descriptor for the following
 *   DMA cycle types:
 *
 *   @li auto-request - used for memory/memory transfer
 *   @li basic - used for a peripheral/memory transfer
 *   @li ping-pong - used for a ping-pong based peripheral/memory transfer
 *     style providing time to refresh one descriptor while the other is
 *     in use.
 *
 *   The DMA cycle is not activated, please see DMA_ActivateAuto(),
 *   DMA_ActivateBasic() or DMA_ActivatePingPong() to activate the DMA cycle.
 *   In many cases, the configuration only has to be done once, and all
 *   subsequent cycles may be activated with the activate function.
 *
 *   For ping-pong DMA cycles, this function must be used both on the primary
 *   and the alternate descriptor prior to activating the DMA cycle.
 *
 *   Notice that the DMA channel must also be configured, see DMA_CfgChannel().
 *
 * @note
 *   If using this function on a descriptor already activated and in use by
 *   the DMA controller, the behaviour is undefined.
 *
 * @param[in] channel
 *   DMA channel to configure for.
 *
 * @param[in] primary
 *   @li true - configure primary descriptor
 *   @li false - configure alternate descriptor
 *
 * @param[in] cfg
 *   Configuration to use.
 ******************************************************************************/
void DMA_CfgDescr(unsigned int channel,
                  bool primary,
                  DMA_CfgDescr_TypeDef *cfg)
{
  DMA_DESCRIPTOR_TypeDef *descr;

  EFM_ASSERT(channel < DMA_CHAN_COUNT);
  EFM_ASSERT(cfg);

  /* Find descriptor to configure */
  if (primary)
  {
    descr = (DMA_DESCRIPTOR_TypeDef *)DMA->CTRLBASE;
  }
  else
  {
    descr = (DMA_DESCRIPTOR_TypeDef *)DMA->ALTCTRLBASE;
  }
  descr += channel;

  /* Prepare the descriptor */
  /* Source/destination end addresses set when started */
  descr->CTRL =
    (cfg->dstInc << _DMA_CTRL_DST_INC_SHIFT) |
    (cfg->size << _DMA_CTRL_DST_SIZE_SHIFT) |
    (cfg->srcInc << _DMA_CTRL_SRC_INC_SHIFT) |
    (cfg->size << _DMA_CTRL_SRC_SIZE_SHIFT) |
    ((uint32_t)(cfg->hprot) << _DMA_CTRL_SRC_PROT_CTRL_SHIFT) |
    (cfg->arbRate << _DMA_CTRL_R_POWER_SHIFT) |
    (0 << _DMA_CTRL_N_MINUS_1_SHIFT) |         /* Set when activated */
    (0 << _DMA_CTRL_NEXT_USEBURST_SHIFT) |     /* Set when activated */
    DMA_CTRL_CYCLE_CTRL_INVALID;               /* Set when activated */
}


/***************************************************************************//**
 * @brief
 *   Configure an alternate DMA descriptor for use with scatter-gather DMA
 *   cycles.
 *
 * @details
 *   In scatter-gather mode, the alternate descriptors are located in one
 *   contiguous memory area. Each of the alternate descriptor must be fully
 *   configured prior to starting the scatter-gather DMA cycle.
 *
 *   The DMA cycle is not activated by this function, please see
 *   DMA_ActivateScatterGather() to activate the DMA cycle. In some cases, the
 *   alternate configuration only has to be done once, and all subsequent
 *   transfers may be activated with the activate function.
 *
 *   Notice that the DMA channel must also be configured, see DMA_CfgChannel().
 *
 * @param[in] descr
 *   Points to start of memory area holding the alternate descriptors.
 *
 * @param[in] indx
 *   Alternate descriptor index number to configure (numbered from 0).
 *
 * @param[in] cfg
 *   Configuration to use.
 ******************************************************************************/
void DMA_CfgDescrScatterGather(DMA_DESCRIPTOR_TypeDef *descr,
                               unsigned int indx,
                               DMA_CfgDescrSGAlt_TypeDef *cfg)
{
  uint32_t cycleCtrl;

  EFM_ASSERT(descr);
  EFM_ASSERT(cfg);

  /* Point to selected entry in alternate descriptor table */
  descr += indx;

  if (cfg->srcInc == dmaDataIncNone)
  {
    descr->SRCEND = cfg->src;
  }
  else
  {
    descr->SRCEND = (void *)((uint32_t)(cfg->src) + ((uint32_t)(cfg->nMinus1) << cfg->srcInc));
  }

  if (cfg->dstInc == dmaDataIncNone)
  {
    descr->DSTEND = cfg->dst;
  }
  else
  {
    descr->DSTEND = (void *)((uint32_t)(cfg->dst) + ((uint32_t)(cfg->nMinus1) << cfg->dstInc));
  }

  /* User definable part not used */
  descr->USER = 0;

  if (cfg->peripheral)
  {
    cycleCtrl = (uint32_t)dmaCycleCtrlPerScatterGather + 1;
  }
  else
  {
    cycleCtrl = (uint32_t)dmaCycleCtrlMemScatterGather + 1;
  }

  descr->CTRL =
    (cfg->dstInc << _DMA_CTRL_DST_INC_SHIFT) |
    (cfg->size << _DMA_CTRL_DST_SIZE_SHIFT) |
    (cfg->srcInc << _DMA_CTRL_SRC_INC_SHIFT) |
    (cfg->size << _DMA_CTRL_SRC_SIZE_SHIFT) |
    ((uint32_t)(cfg->hprot) << _DMA_CTRL_SRC_PROT_CTRL_SHIFT) |
    (cfg->arbRate << _DMA_CTRL_R_POWER_SHIFT) |
    ((uint32_t)(cfg->nMinus1) << _DMA_CTRL_N_MINUS_1_SHIFT) |
    /* Never set next useburst bit, since the descriptor used after the */
    /* alternate descriptor is the primary descriptor which operates on */
    /* memory. If the alternate descriptors need to have useBurst set, this */
    /* done when setting up the primary descriptor, ie when activating. */
    (0 << _DMA_CTRL_NEXT_USEBURST_SHIFT) |
    (cycleCtrl << _DMA_CTRL_CYCLE_CTRL_SHIFT);
}


/***************************************************************************//**
 * @brief
 *   Check if DMA channel is enabled.
 *
 * @details
 *   The DMA channel is disabled when the DMA controller has finished a DMA
 *   cycle.
 *
 * @param[in] channel
 *   DMA channel to check.
 *
 * @return
 *   true if channel is enabled, false if not.
 ******************************************************************************/
bool DMA_ChannelEnabled(unsigned int channel)
{
  EFM_ASSERT(channel < DMA_CHAN_COUNT);

  return (bool)((DMA->CHENS >> channel) & 1);
}


/***************************************************************************//**
 * @brief
 *   Initializes DMA controller.
 *
 * @details
 *   This function will reset and prepare the DMA controller for use. Although
 *   it may be used several times, it is normally only used during system
 *   init. If reused during normal operation, notice that any ongoing DMA
 *   transfers will be aborted. When completed, the DMA controller is in
 *   an enabled state.
 *
 * @note
 *   Must be invoked before using the DMA controller.
 *
 * @param[in] init
 *   Pointer to a structure containing DMA init information.
 ******************************************************************************/
void DMA_Init(DMA_Init_TypeDef *init)
{
  EFM_ASSERT(init);

  /* Make sure control block is properly aligned */
  EFM_ASSERT(!((uint32_t)(init->controlBlock) & ((16 * DMA_CHAN_COUNT * 2) - 1)));

  /* Make sure DMA clock is enabled prior to accessing DMA module */
  CMU_ClockEnable(cmuClock_DMA, true);

  /* Make sure DMA controller is set to a known reset state */
  DMA_Reset();

  /* Clear/enable DMA interrupts */
  NVIC_ClearPendingIRQ(DMA_IRQn);
  NVIC_EnableIRQ(DMA_IRQn);

  /* Enable bus error interrupt */
  DMA->IEN = DMA_IEN_ERR;

  /* Set pointer to control block, notice that this ptr must have been */
  /* properly aligned, according to requirements defined in the reference */
  /* manual. */
  DMA->CTRLBASE = (uint32_t)(init->controlBlock);

  /* Configure and enable the DMA controller */
  DMA->CONFIG = ((uint32_t)(init->hprot) << _DMA_CONFIG_CHPROT_SHIFT) | DMA_CONFIG_EN;
}


/***************************************************************************//**
 * @brief
 *   Refresh a descriptor used in a DMA ping-pong cycle.
 *
 * @details
 *   During a ping-pong DMA cycle, the DMA controller automatically alternates
 *   between primary and alternate descriptors, when completing use of a
 *   descriptor. While the other descriptor is in use by the DMA controller,
 *   the SW should refresh the completed descriptor. This is typically done from
 *   the callback defined for the ping-pong cycle.
 *
 * @param[in] channel
 *   DMA channel to refresh ping-pong descriptor for.
 *
 * @param[in] primary
 *   @li true - refresh primary descriptor
 *   @li false - refresh alternate descriptor
 *
 * @param[in] useBurst
 *   The burst feature is only used on peripherals supporting DMA bursts.
 *   Bursts must not be used if the total length (as given by nMinus1) is
 *   less than the arbitration rate configured for the descriptor. Please
 *   refer to the reference manual for further details on burst usage.
 *
 * @param[in] dst
 *   Address to start location to transfer data to. If NULL, leave setting in
 *   descriptor as is.
 *
 * @param[in] src
 *   Address to start location to transfer data from. If NULL, leave setting in
 *   descriptor as is.
 *
 * @param[in] nMinus1
 *   Number of DMA transfer elements (minus 1) to transfer (<= 1023). The
 *   size of the DMA transfer element (1, 2 or 4 bytes) is configured with
 *   DMA_CfgDescr().
 *
 * @param[in] stop
 *   Indicate that the DMA ping-pong cycle shall stop @b after completing use
 *   of this descriptor.
 ******************************************************************************/
void DMA_RefreshPingPong(unsigned int channel,
                         bool primary,
                         bool useBurst,
                         void *dst,
                         void *src,
                         unsigned int nMinus1,
                         bool stop)
{
  DMA_CycleCtrl_TypeDef  cycleCtrl;
  DMA_DESCRIPTOR_TypeDef *descr;
  uint32_t               inc;
  uint32_t               chBit;
  uint32_t               tmp;

  EFM_ASSERT(channel < DMA_CHAN_COUNT);
  EFM_ASSERT(nMinus1 <= (_DMA_CTRL_N_MINUS_1_MASK >> _DMA_CTRL_N_MINUS_1_SHIFT));

  /* The ping-pong DMA cycle may be stopped by issuing a basic cycle type */
  if (stop)
  {
    cycleCtrl = dmaCycleCtrlBasic;
  }
  else
  {
    cycleCtrl = dmaCycleCtrlPingPong;
  }

  /* Find descriptor to configure */
  if (primary)
  {
    descr = ((DMA_DESCRIPTOR_TypeDef *)(DMA->CTRLBASE)) + channel;
  }
  else
  {
    descr = ((DMA_DESCRIPTOR_TypeDef *)(DMA->ALTCTRLBASE)) + channel;
  }

  if (src)
  {
    inc = (descr->CTRL & _DMA_CTRL_SRC_INC_MASK) >> _DMA_CTRL_SRC_INC_SHIFT;
    if (inc == _DMA_CTRL_SRC_INC_NONE)
    {
      descr->SRCEND = src;
    }
    else
    {
      descr->SRCEND = (void *)((uint32_t)src + (nMinus1 << inc));
    }
  }

  if (dst)
  {
    inc = (descr->CTRL & _DMA_CTRL_DST_INC_MASK) >> _DMA_CTRL_DST_INC_SHIFT;
    if (inc == _DMA_CTRL_DST_INC_NONE)
    {
      descr->DSTEND = dst;
    }
    else
    {
      descr->DSTEND = (void *)((uint32_t)dst + (nMinus1 << inc));
    }
  }

  chBit = 1 << channel;
  if (useBurst)
  {
    DMA->CHUSEBURSTS = chBit;
  }
  else
  {
    DMA->CHUSEBURSTC = chBit;
  }

  /* Set cycle control */
  tmp         = descr->CTRL & ~(_DMA_CTRL_CYCLE_CTRL_MASK | _DMA_CTRL_N_MINUS_1_MASK);
  tmp        |= nMinus1 << _DMA_CTRL_N_MINUS_1_SHIFT;
  tmp        |= cycleCtrl << _DMA_CTRL_CYCLE_CTRL_SHIFT;
  descr->CTRL = tmp;
}


/***************************************************************************//**
 * @brief
 *   Reset the DMA controller.
 *
 * @details
 *   This functions will disable the DMA controller and set it to a reset
 *   state.
 *
 * @note
 *   Notice that any ongoing transfers will be aborted.
 ******************************************************************************/
void DMA_Reset(void)
{
  int i;

  /* Disable DMA interrupts */
  NVIC_DisableIRQ(DMA_IRQn);

  /* Put the DMA controller into a known state, first disabling it. */
  DMA->CONFIG      = _DMA_CONFIG_RESETVALUE;
  DMA->CHUSEBURSTC = _DMA_CHUSEBURSTC_MASK;
  DMA->CHREQMASKC  = _DMA_CHREQMASKC_MASK;
  DMA->CHENC       = _DMA_CHENC_MASK;
  DMA->CHALTC      = _DMA_CHALTC_MASK;
  DMA->CHPRIC      = _DMA_CHPRIC_MASK;
  DMA->ERRORC      = DMA_ERRORC_ERRORC;
  DMA->IEN         = _DMA_IEN_RESETVALUE;
  DMA->IFC         = _DMA_IFC_MASK;

  /* Clear channel control flags */
  for (i = 0; i < DMA_CHAN_COUNT; i++)
  {
    DMA->CH[i].CTRL = _DMA_CH_CTRL_RESETVALUE;
  }
}


/** @} (end addtogroup DMA) */
/** @} (end addtogroup EFM32_Library) */
