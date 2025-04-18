/***************************************************************************//**
 * @file
 * @brief Inter-integrated Circuit (I2C) Peripheral API for EFM32.
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
#include "efm32.h"
#include "efm32_i2c.h"
#include "efm32_cmu.h"
#include "efm32_bitband.h"
#include "efm32_assert.h"

/***************************************************************************//**
 * @addtogroup EFM32_Library
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup I2C
 * @brief Inter-integrated Circuit (I2C) Peripheral API for EFM32
 * @{
 ******************************************************************************/

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Validation of I2C register block pointer reference for assert statements. */
#if defined(_EFM32_GECKO_FAMILY) || defined(_EFM32_TINY_FAMILY)
#define I2C_REF_VALID(ref)    ((ref) == I2C0)
#endif

#if defined(_EFM32_GIANT_FAMILY)
#define I2C_REF_VALID(ref)    ((ref == I2C0) || (ref == I2C1))
#endif

/** Error flags indicating I2C transfer has failed somehow. */
/* Notice that I2C_IF_TXOF (transmit overflow) is not really possible with */
/* this SW supporting master mode. Likewise for I2C_IF_RXUF (receive underflow) */
/* RXUF is only likely to occur with this SW if using a debugger peeking into */
/* RXDATA register. Thus, we ignore those types of fault. */
#define I2C_IF_ERRORS    (I2C_IF_BUSERR | I2C_IF_ARBLOST)

/** @endcond */

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Master mode transfer states. */
typedef enum
{
  i2cStateStartAddrSend,       /**< Send start + (first part of) address. */
  i2cStateAddrWFAckNack,       /**< Wait for ACK/NACK on (first part of) address. */
  i2cStateAddrWF2ndAckNack,    /**< Wait for ACK/NACK on second part of 10 bit address. */
  i2cStateRStartAddrSend,      /**< Send repeated start + (first part of) address. */
  i2cStateRAddrWFAckNack,      /**< Wait for ACK/NACK on address sent after repeated start. */
  i2cStateDataSend,            /**< Send data. */
  i2cStateDataWFAckNack,       /**< Wait for ACK/NACK on data sent. */
  i2cStateWFData,              /**< Wait for data. */
  i2cStateWFStopSent,          /**< Wait for STOP to have been transmitted. */
  i2cStateDone                 /**< Transfer completed successfully. */
} I2C_TransferState_TypeDef;

/** @endcond */

/*******************************************************************************
 *******************************   STRUCTS   ***********************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/** Structure used to store state information on an ongoing master mode transfer. */
typedef struct
{
  /** Current state. */
  I2C_TransferState_TypeDef  state;

  /** Result return code. */
  I2C_TransferReturn_TypeDef result;

  /** Offset in current sequence buffer. */
  uint16_t                   offset;

  /* Index to current sequence buffer in use. */
  uint8_t                    bufIndx;

  /** Reference to I2C transfer sequence definition provided by user. */
  I2C_TransferSeq_TypeDef    *seq;
} I2C_Transfer_TypeDef;

/** @endcond */

/*******************************************************************************
 *****************************   LOCAL DATA   *******^**************************
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/**
 * Lookup table for Nlow + Nhigh setting defined by CLHR. Set undefined
 * index (0x3) to reflect default setting just in case.
 */
static const uint8_t i2cNSum[] = { 4 + 4, 6 + 3, 11 + 3, 4 + 4 };

/** Transfer state info for ongoing master mode transfer */
static I2C_Transfer_TypeDef i2cTransfer[I2C_COUNT];

/** @endcond */

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Get current configured I2C bus frequency.
 *
 * @details
 *   This frequency is only of relevance when acting as master.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @return
 *   Current I2C frequency in Hz.
 ******************************************************************************/
uint32_t I2C_BusFreqGet(I2C_TypeDef *i2c)
{
  uint32_t hfperclk;
  uint32_t n;

  /* Max frequency is given by fSCL = fHFPERCLK/((Nlow + Nhigh)(DIV + 1) + 4) */
  hfperclk = CMU_ClockFreqGet(cmuClock_HFPER);
  n        = (uint32_t)(i2cNSum[(i2c->CTRL & _I2C_CTRL_CLHR_MASK) >> _I2C_CTRL_CLHR_SHIFT]);

  return(hfperclk / ((n * (i2c->CLKDIV + 1)) + 4));
}


/***************************************************************************//**
 * @brief
 *   Set I2C bus frequency.
 *
 * @details
 *   The bus frequency is only of relevance when acting as a master. The bus
 *   frequency should not be set higher than the max frequency accepted by the
 *   slowest device on the bus.
 *
 *   Notice that due to asymmetric requirements on low and high I2C clock
 *   cycles by the I2C specification, the actual max frequency allowed in order
 *   to comply with the specification may be somewhat lower than expected.
 *
 *   Please refer to the reference manual, details on I2C clock generation,
 *   for max allowed theoretical frequencies for different modes.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] refFreq
 *   I2C reference clock frequency in Hz that will be used. If set to 0,
 *   the currently configured reference clock is assumed. Setting it to a higher
 *   than actual configured value only has the consequence of reducing the real
 *   I2C frequency.
 *
 * @param[in] freq
 *   Bus frequency to set (actual bus speed may be lower due to integer
 *   prescaling). Safe (according to I2C specification) max frequencies for
 *   standard, fast and fast+ modes are available using I2C_FREQ_ defines.
 *   (Using I2C_FREQ_ defines requires corresponding setting of @p type.)
 *   Slowest slave device on bus must always be considered.
 *
 * @param[in] type
 *   Clock low to high ratio type to use. If not using i2cClockHLRStandard,
 *   make sure all devices on the bus support the specified mode. Using a
 *   non-standard ratio is useful to achieve higher bus clock in fast and
 *   fast+ modes.
 ******************************************************************************/
void I2C_BusFreqSet(I2C_TypeDef *i2c,
                    uint32_t refFreq,
                    uint32_t freq,
                    I2C_ClockHLR_TypeDef type)
{
  uint32_t n;
  uint32_t div;

  /* Avoid divide by 0 */
  EFM_ASSERT(freq);
  if (!freq)
  {
    return;
  }

  /* Frequency is given by fSCL = fHFPERCLK/((Nlow + Nhigh)(DIV + 1) + 4), thus */
  /* DIV = ((fHFPERCLK - 4fSCL)/((Nlow + Nhigh)fSCL)) - 1 */

  if (!refFreq)
  {
    refFreq = CMU_ClockFreqGet(cmuClock_HFPER);
  }
  n = (uint32_t)(i2cNSum[(i2c->CTRL & _I2C_CTRL_CLHR_MASK) >> _I2C_CTRL_CLHR_SHIFT]);

  div = (refFreq - (4 * freq)) / (n * freq);
  EFM_ASSERT(div);
  if (div)
  {
    div--;
  }

  /* Clock divisor must be at least 1 in slave mode according to reference */
  /* manual (in which case there is normally no need to set bus frequency). */
  if ((i2c->CTRL & I2C_CTRL_SLAVE) && !div)
  {
    div = 1;
  }

  EFM_ASSERT(div <= _I2C_CLKDIV_DIV_MASK);
  i2c->CLKDIV = div;
}


/***************************************************************************//**
 * @brief
 *   Enable/disable I2C.
 *
 * @note
 *   After enabling the I2C (from being disabled), the I2C is in BUSY state.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] enable
 *   true to enable counting, false to disable.
 ******************************************************************************/
void I2C_Enable(I2C_TypeDef *i2c, bool enable)
{
  EFM_ASSERT(I2C_REF_VALID(i2c));

  BITBAND_Peripheral(&(i2c->CTRL), _I2C_CTRL_EN_SHIFT, (unsigned int)enable);
}


/***************************************************************************//**
 * @brief
 *   Initialize I2C.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] init
 *   Pointer to I2C initialization structure.
 ******************************************************************************/
void I2C_Init(I2C_TypeDef *i2c, const I2C_Init_TypeDef *init)
{
  EFM_ASSERT(I2C_REF_VALID(i2c));

  i2c->IEN = 0;
  i2c->IFC = _I2C_IFC_MASK;

  I2C_BusFreqSet(i2c, init->refFreq, init->freq, init->clhr);

  BITBAND_Peripheral(&(i2c->CTRL),
                     _I2C_CTRL_SLAVE_SHIFT,
                     ~((unsigned int)(init->master)));

  BITBAND_Peripheral(&(i2c->CTRL),
                     _I2C_CTRL_EN_SHIFT,
                     (unsigned int)(init->enable));
}


/***************************************************************************//**
 * @brief
 *   Reset I2C to same state as after a HW reset.
 *
 * @note
 *   The ROUTE register is NOT reset by this function, in order to allow for
 *   centralized setup of this feature.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 ******************************************************************************/
void I2C_Reset(I2C_TypeDef *i2c)
{
  i2c->CTRL      = _I2C_CTRL_RESETVALUE;
  i2c->CLKDIV    = _I2C_CLKDIV_RESETVALUE;
  i2c->SADDR     = _I2C_SADDR_RESETVALUE;
  i2c->SADDRMASK = _I2C_SADDRMASK_RESETVALUE;
  i2c->IEN       = _I2C_IEN_RESETVALUE;
  i2c->IFC       = _I2C_IFC_MASK;
  /* Do not reset route register, setting should be done independently */
}


/***************************************************************************//**
 * @brief
 *   Continue an initiated I2C transfer (single master mode only).
 *
 * @details
 *   This function is used repeatedly after a I2C_TransferInit() in order to
 *   complete a transfer. It may be used in polled mode as the below example
 *   shows:
 * @verbatim
 * I2C_TransferReturn_TypeDef ret;
 *
 * // Do a polled transfer
 * ret = I2C_TransferInit(I2C0, seq);
 * while (ret == i2cTransferInProgress)
 * {
 *   ret = I2C_Transfer(I2C0);
 * }
 * @endverbatim
 *  It may also be used in interrupt driven mode, where this function is invoked
 *  from the interrupt handler. Notice that if used in interrupt mode, NVIC
 *  interrupts must be configured and enabled for the I2C bus used. I2C
 *  peripheral specific interrupts are managed by this SW.
 *
 * @note
 *   Only single master mode is supported.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @return
 *   Returns status for ongoing transfer.
 *   @li #i2cTransferInProgress - indicates that transfer not finished.
 *   @li #i2cTransferDone - transfer completed successfully.
 *   @li otherwise some sort of error has occurred.
 *
 ******************************************************************************/
I2C_TransferReturn_TypeDef I2C_Transfer(I2C_TypeDef *i2c)
{
  uint32_t                tmp;
  uint32_t                pending;
  I2C_Transfer_TypeDef    *transfer;
  I2C_TransferSeq_TypeDef *seq;

  EFM_ASSERT(I2C_REF_VALID(i2c));

  /* Support up to 2 I2C buses */
  if (i2c == I2C0)
  {
    transfer = i2cTransfer;
  }
#if (I2C_COUNT > 1)
  else if (i2c == I2C1)
  {
    transfer = i2cTransfer + 1;
  }
#endif
  else
  {
    return(i2cTransferUsageFault);
  }

  seq = transfer->seq;
  for (;; )
  {
    pending = i2c->IF;

    /* If some sort of fault, abort transfer. */
    if (pending & I2C_IF_ERRORS)
    {
      if (pending & I2C_IF_ARBLOST)
      {
        /* If arbitration fault, it indicates either a slave device */
        /* not responding as expected, or other master which is not */
        /* supported by this SW. */
        transfer->result = i2cTransferArbLost;
      }
      else if (pending & I2C_IF_BUSERR)
      {
        /* A bus error indicates a misplaced start or stop, which should */
        /* not occur in master mode controlled by this SW. */
        transfer->result = i2cTransferBusErr;
      }

      /* If error situation occurred, it is difficult to know */
      /* exact cause and how to resolve. It will be up to a wrapper */
      /* to determine how to handle a fault/recovery if possible. */
      transfer->state = i2cStateDone;
      goto done;
    }

    switch (transfer->state)
    {
    /***************************************************/
    /* Send first start+address (first byte if 10 bit) */
    /***************************************************/
    case i2cStateStartAddrSend:
      if (seq->flags & I2C_FLAG_10BIT_ADDR)
      {
        tmp = (((uint32_t)(seq->addr) >> 8) & 0x06) | 0xf0;

        /* In 10 bit address mode, the address following the first */
        /* start always indicate write. */
      }
      else
      {
        tmp = (uint32_t)(seq->addr) & 0xfe;

        if (seq->flags & I2C_FLAG_READ)
        {
          /* Indicate read request */
          tmp |= 1;
        }
      }

      transfer->state = i2cStateAddrWFAckNack;
      i2c->TXDATA     = tmp; /* Data not transmitted until START sent */
      i2c->CMD        = I2C_CMD_START;
      goto done;

    /*******************************************************/
    /* Wait for ACK/NACK on address (first byte if 10 bit) */
    /*******************************************************/
    case i2cStateAddrWFAckNack:
      if (pending & I2C_IF_NACK)
      {
        i2c->IFC         = I2C_IFC_NACK;
        transfer->result = i2cTransferNack;
        transfer->state  = i2cStateWFStopSent;
        i2c->CMD         = I2C_CMD_STOP;
      }
      else if (pending & I2C_IF_ACK)
      {
        i2c->IFC = I2C_IFC_ACK;

        /* If 10 bit address, send 2nd byte of address. */
        if (seq->flags & I2C_FLAG_10BIT_ADDR)
        {
          transfer->state = i2cStateAddrWF2ndAckNack;
          i2c->TXDATA     = (uint32_t)(seq->addr) & 0xff;
        }
        else
        {
          /* Determine whether receiving or sending data */
          if (seq->flags & I2C_FLAG_READ)
          {
            transfer->state = i2cStateWFData;
          }
          else
          {
            transfer->state = i2cStateDataSend;
            continue;
          }
        }
      }
      goto done;

    /******************************************************/
    /* Wait for ACK/NACK on second byte of 10 bit address */
    /******************************************************/
    case i2cStateAddrWF2ndAckNack:
      if (pending & I2C_IF_NACK)
      {
        i2c->IFC         = I2C_IFC_NACK;
        transfer->result = i2cTransferNack;
        transfer->state  = i2cStateWFStopSent;
        i2c->CMD         = I2C_CMD_STOP;
      }
      else if (pending & I2C_IF_ACK)
      {
        i2c->IFC = I2C_IFC_ACK;

        /* If using plain read sequence with 10 bit address, switch to send */
        /* repeated start. */
        if (seq->flags & I2C_FLAG_READ)
        {
          transfer->state = i2cStateRStartAddrSend;
        }
        /* Otherwise expected to write 0 or more bytes */
        else
        {
          transfer->state = i2cStateDataSend;
        }
        continue;
      }
      goto done;

    /*******************************/
    /* Send repeated start+address */
    /*******************************/
    case i2cStateRStartAddrSend:
      if (seq->flags & I2C_FLAG_10BIT_ADDR)
      {
        tmp = ((seq->addr >> 8) & 0x06) | 0xf0;
      }
      else
      {
        tmp = seq->addr & 0xfe;
      }

      /* If this is a write+read combined sequence, then read is about to start */
      if (seq->flags & I2C_FLAG_WRITE_READ)
      {
        /* Indicate read request */
        tmp |= 1;
      }

      transfer->state = i2cStateRAddrWFAckNack;
      /* We have to write START cmd first since repeated start, otherwise */
      /* data would be sent first. */
      i2c->CMD    = I2C_CMD_START;
      i2c->TXDATA = tmp;
      goto done;

    /**********************************************************************/
    /* Wait for ACK/NACK on repeated start+address (first byte if 10 bit) */
    /**********************************************************************/
    case i2cStateRAddrWFAckNack:
      if (pending & I2C_IF_NACK)
      {
        i2c->IFC         = I2C_IFC_NACK;
        transfer->result = i2cTransferNack;
        transfer->state  = i2cStateWFStopSent;
        i2c->CMD         = I2C_CMD_STOP;
      }
      else if (pending & I2C_IF_ACK)
      {
        i2c->IFC = I2C_IFC_ACK;

        /* Determine whether receiving or sending data */
        if (seq->flags & I2C_FLAG_WRITE_READ)
        {
          transfer->state = i2cStateWFData;
        }
        else
        {
          transfer->state = i2cStateDataSend;
          continue;
        }
      }
      goto done;

    /*****************************/
    /* Send a data byte to slave */
    /*****************************/
    case i2cStateDataSend:
      /* Reached end of data buffer? */
      if (transfer->offset >= seq->buf[transfer->bufIndx].len)
      {
        /* Move to next message part */
        transfer->offset = 0;
        transfer->bufIndx++;

        /* Send repeated start when switching to read mode on 2nd buffer */
        if (seq->flags & I2C_FLAG_WRITE_READ)
        {
          transfer->state = i2cStateRStartAddrSend;
          continue;
        }

        /* Only writing from one buffer, or finished both buffers */
        if ((seq->flags & I2C_FLAG_WRITE) || (transfer->bufIndx > 1))
        {
          transfer->state = i2cStateWFStopSent;
          i2c->CMD        = I2C_CMD_STOP;
          goto done;
        }

        /* Reprocess in case next buffer is empty */
        continue;
      }

      /* Send byte */
      i2c->TXDATA     = (uint32_t)(seq->buf[transfer->bufIndx].data[transfer->offset++]);
      transfer->state = i2cStateDataWFAckNack;
      goto done;

    /*********************************************************/
    /* Wait for ACK/NACK from slave after sending data to it */
    /*********************************************************/
    case i2cStateDataWFAckNack:
      if (pending & I2C_IF_NACK)
      {
        i2c->IFC         = I2C_IFC_NACK;
        transfer->result = i2cTransferNack;
        transfer->state  = i2cStateWFStopSent;
        i2c->CMD         = I2C_CMD_STOP;
      }
      else if (pending & I2C_IF_ACK)
      {
        i2c->IFC        = I2C_IFC_ACK;
        transfer->state = i2cStateDataSend;
        continue;
      }
      goto done;

    /****************************/
    /* Wait for data from slave */
    /****************************/
    case i2cStateWFData:
      if (pending & I2C_IF_RXDATAV)
      {
        uint8_t data;

        /* Must read out data in order to not block further progress */
        data = (uint8_t)(i2c->RXDATA);

        /* Make sure not storing beyond end of buffer just in case */
        if (transfer->offset < seq->buf[transfer->bufIndx].len)
        {
          seq->buf[transfer->bufIndx].data[transfer->offset++] = data;
        }

        /* If we have read all requested data, then the sequence should end */
        if (transfer->offset >= seq->buf[transfer->bufIndx].len)
        {
          transfer->state = i2cStateWFStopSent;
          i2c->CMD        = I2C_CMD_NACK;
          i2c->CMD        = I2C_CMD_STOP;
        }
        else
        {
          /* Send ACK and wait for next byte */
          i2c->CMD = I2C_CMD_ACK;
        }
      }
      goto done;

    /***********************************/
    /* Wait for STOP to have been sent */
    /***********************************/
    case i2cStateWFStopSent:
      if (pending & I2C_IF_MSTOP)
      {
        i2c->IFC        = I2C_IFC_MSTOP;
        transfer->state = i2cStateDone;
      }
      goto done;

    /******************************/
    /* Unexpected state, SW fault */
    /******************************/
    default:
      transfer->result = i2cTransferSwFault;
      transfer->state  = i2cStateDone;
      goto done;
    }
  }

 done:

  if (transfer->state == i2cStateDone)
  {
    /* Disable interrupt sources when done */
    i2c->IEN = 0;

    /* Update result unless some fault already occurred */
    if (transfer->result == i2cTransferInProgress)
    {
      transfer->result = i2cTransferDone;
    }
  }

  return transfer->result;
}


/***************************************************************************//**
 * @brief
 *   Prepare and start an I2C transfer (single master mode only).
 *
 * @details
 *   This function must be invoked in order to start an I2C transfer
 *   sequence. In order to actually complete the transfer, I2C_Transfer() must
 *   be used either in polled mode or by adding a small driver wrapper utilizing
 *   interrupts.
 *
 * @note
 *   Only single master mode is supported.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] seq
 *   Pointer to sequence structure defining the I2C transfer to take place. The
 *   referenced structure must exist until the transfer has fully completed.
 *
 * @return
 *   Returns status for ongoing transfer:
 *   @li #i2cTransferInProgress - indicates that transfer not finished.
 *   @li otherwise some sort of error has occurred.
 ******************************************************************************/
I2C_TransferReturn_TypeDef I2C_TransferInit(I2C_TypeDef *i2c,
                                            I2C_TransferSeq_TypeDef *seq)
{
  I2C_Transfer_TypeDef *transfer;
  volatile uint32_t    tmp;

  EFM_ASSERT(I2C_REF_VALID(i2c));
  EFM_ASSERT(seq);

  /* Support up to 2 I2C buses */
  if (i2c == I2C0)
  {
    transfer = i2cTransfer;
  }
#if (I2C_COUNT > 1)
  else if (i2c == I2C1)
  {
    transfer = i2cTransfer + 1;
  }
#endif
  else
  {
    return(i2cTransferUsageFault);
  }

  /* Check if in busy state. Since this SW assumes single master, we can */
  /* just issue an abort. The BUSY state is normal after a reset. */
  if (i2c->STATE & I2C_STATE_BUSY)
  {
    i2c->CMD = I2C_CMD_ABORT;
  }

  /* Make sure user is not trying to read 0 bytes, it is not */
  /* possible according to I2C spec, since slave will always start */
  /* sending first byte ACK on address. The read operation can */
  /* only be stopped by NACKing a received byte, ie minimum 1 byte. */
  if (((seq->flags & I2C_FLAG_READ) && !(seq->buf[0].len)) ||
      ((seq->flags & I2C_FLAG_WRITE_READ) && !(seq->buf[1].len))
      )
  {
    return(i2cTransferUsageFault);
  }

  /* Prepare for a transfer */
  transfer->state   = i2cStateStartAddrSend;
  transfer->result  = i2cTransferInProgress;
  transfer->offset  = 0;
  transfer->bufIndx = 0;
  transfer->seq     = seq;

  /* Ensure buffers are empty */
  i2c->CMD = I2C_CMD_CLEARPC | I2C_CMD_CLEARTX;
  if (i2c->IF & I2C_IF_RXDATAV)
  {
    tmp = i2c->RXDATA;
  }

  /* Clear all pending interrupts prior to starting transfer. */
  i2c->IFC = _I2C_IFC_MASK;

  /* Enable those interrupts we are interested in throughout transfer. */
  /* Notice that the I2C interrupt must also be enabled in the NVIC, but */
  /* that is left for an additional driver wrapper. */
  i2c->IEN = I2C_IF_NACK | I2C_IF_ACK | I2C_IF_MSTOP |
             I2C_IF_RXDATAV | I2C_IF_ERRORS;

  /* Start transfer */
  return(I2C_Transfer(i2c));
}

/** @} (end addtogroup I2C) */
/** @} (end addtogroup EFM32_Library) */
