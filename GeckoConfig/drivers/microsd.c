/**************************************************************************//**
 * @file
 * @brief Micro SD card driver for EFM32_G2xx_DK and EFM32_G8xx_DK
 *        This file provides the interface layer between the DVK and the 
 *        fat filesystem provided.
 * @author Energy Micro AS
 * @version 1.6.1
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
#include "diskio.h"
#include "microsd.h"
#include "efm32_cmu.h"
#include "efm32_usart.h"
#include "segmentlcd.h"

#define CS_LOW()                 GPIO->P[2].DOUTCLR = 0x100;
#define CS_HIGH()                GPIO->P[2].DOUTSET = 0x100;

extern DSTATUS Stat;    /* Disk status */
extern UINT Timer1;     /* 1000Hz decrement timer - this is updated in the 
                         * systick handler. */

uint32_t MICROSD_refFreq;       /* HFPERClk frequency, used for setting the baudrates
                                 * correctly. */

/******************************************************************************
 * @brief 
 *   Initialize SPI setting. 
 *   The MicroSD card is connected to USART0, location #2. 
 *   The FatFS system controls the SPI CS directly through the macros
 *   CS_HIGH() and CS_LOW().
 * @param[in] refFreq
 *   The frequency of the input clock.
 *****************************************************************************/
void MICROSD_init(uint32_t refFreq)
{ 
  MICROSD_refFreq = refFreq; 
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;
  
  /* Enabling clock to USART 0 */
  CMU_ClockEnable(cmuClock_USART0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize USART in SPI master mode. */  
  init.refFreq = refFreq;            
  init.baudrate = LOW_SPEED_CLOCK;
  init.msbf = true;
  USART_InitSync(MICROSD_USART, &init);
  
  /* Enabling pins and setting location, SPI CS not enable */
  MICROSD_USART->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN 
                       | USART_ROUTE_CLKPEN | USART_ROUTE_LOCATION_LOC2;

  /* IO configuration (USART 0, Location #2) */
  GPIO_PinModeSet(gpioPortC, 11, gpioModePushPull, 0); /* MOSI */
  GPIO_PinModeSet(gpioPortC, 10, gpioModeInput, 0);    /* MISO */
  GPIO_PinModeSet(gpioPortC, 8,  gpioModePushPull, 0); /* CS */
  GPIO_PinModeSet(gpioPortC, 9,  gpioModePushPull, 0); /* Clock */
}


/**************************************************************************//**
 * @brief Sets the SPI speed to low. This function is used by the FatFS driver.
 *****************************************************************************/  
void FCLK_SLOW()
{
  USART_BaudrateSyncSet(MICROSD_USART, MICROSD_refFreq, LOW_SPEED_CLOCK);
}
/**************************************************************************//**
 * @brief Sets the SPI speed to high. This function is used by the FatFS driver.
 *****************************************************************************/  
void FCLK_FAST()
{
  USART_BaudrateSyncSet(MICROSD_USART, MICROSD_refFreq, HIGH_SPEED_CLOCK);
}

/**************************************************************************//**
 * @brief Receive one byte from SD to EFM via SPI 
 * @return Byte to receive
 *****************************************************************************/
uint8_t rcvr_spi(void)
{
  while (!(MICROSD_USART->STATUS & USART_STATUS_TXBL)) ;
  MICROSD_USART->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

  MICROSD_USART->TXDATA = 0xff;
  return USART_Rx(MICROSD_USART);
}

/**************************************************************************//**
 * @brief Wait for card ready
 * @return 0xff: card ready, other value: card not ready
 *****************************************************************************/
uint8_t wait_ready(void)
{
  uint8_t res;
  /* Wait for ready in timeout of 500ms */  
  Timer1 = 500;
  do
    res = rcvr_spi();
  while ((res != 0xFF) && Timer1);
  return res;
}

/**************************************************************************//**
 * @brief Deselect the card and release SPI bus 
 *****************************************************************************/
void deselect(void)
{
  CS_HIGH();
  rcvr_spi();
}

/**************************************************************************//**
 * @brief Select the card and wait ready 
 * @return 1:Successful, 0:Timeout
 *****************************************************************************/
int select(void)
{
  CS_LOW();
  if (wait_ready() != 0xFF)
  {
    deselect();
    return 0;
  }
  return 1;
}

/**************************************************************************//**
 * @brief Turn on SD card power  
 *        DVK doesn't support socket power control, only enable the SPI clock
 *****************************************************************************/
void power_on(void)
{ 
  /* Enable SPI */
  CMU_ClockEnable(cmuClock_USART0, true);
}

/**************************************************************************//**
 * @brief Turn off SD card power  
 *        DVK doesn't support socket power control, only disable the SPI clock
 *****************************************************************************/
void power_off(void)
{
  /* Wait for card ready */
  select();                 
  deselect();
  /* Disable USART0 */
  CMU_ClockEnable(cmuClock_USART0, false);
  /* Set STA_NOINIT */
  Stat |= STA_NOINIT;                    
}

/**************************************************************************//**
 * @brief Receive a data packet from MMC 
 * @param[out] buff
 *        Data buffer to store received data
 * @param btr
 *        Byte count (must be multiple of 4)
 * @return 
 *        1:OK, 0:Failed
 *****************************************************************************/
int rcvr_datablock(BYTE *buff, UINT btr)
{
  uint8_t token;
  uint16_t *buff_16 = (uint16_t *) buff;
  uint32_t framectrl;
  uint32_t ctrl;
  
  /* Wait for data packet in timeout of 100ms */
  Timer1 = 100;
  do  
    token = rcvr_spi();
  while ((token == 0xFF) && (Timer1 > 0));
  
  if(token != 0xFE)
    /* Invalid data token */
    return 0;
  
  /* Clear send and receive buffer */
  MICROSD_USART->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
  /* Store old configuration. */
  framectrl = MICROSD_USART->FRAME;
  ctrl = MICROSD_USART->CTRL;
  /* Set up frames to 16 bit on each frame. This will increase the 
     data rate and make the maximum use of the buffers available. */
  MICROSD_USART->FRAME = (MICROSD_USART->FRAME & (~_USART_FRAME_DATABITS_MASK))
                       | USART_FRAME_DATABITS_SIXTEEN;
  MICROSD_USART->CTRL = (MICROSD_USART->CTRL & (~_USART_CTRL_BYTESWAP_MASK))
                       | USART_CTRL_BYTESWAP;
  while (!(MICROSD_USART->STATUS & USART_STATUS_TXBL)) ;
  /* Pipelining - The USART has two buffers of 16 bit in both 
   * directions. Make sure that at least one is in the pipe at all
   * times to maximize throughput. */
  MICROSD_USART->TXDOUBLE = 0xffff; 
  /* Receive the data block into buffer */
  do
  { 
    MICROSD_USART->TXDOUBLE = 0xffff;
    while (!(MICROSD_USART->STATUS & USART_STATUS_RXFULL)) ;
    *buff_16++ = (uint16_t) MICROSD_USART->RXDOUBLE;
  } while (btr -= 2);
  
  /* Next two bytes is the CRC which we discard. */  
  while (!(MICROSD_USART->STATUS & USART_STATUS_RXFULL)) ;
  MICROSD_USART->CMD = USART_CMD_CLEARRX;
  
  /* Restore 8-bit operation */
  MICROSD_USART->FRAME = framectrl;
  MICROSD_USART->CTRL = ctrl;
  
  return 1;		/* Return with success */
}

/**************************************************************************//**
 * @brief Send a data packet to MMC  
 * @param[in] buff 512 bytes data block to be transmitted
 * @param token Data token
 * @return 1:OK, 0:Failed
 *****************************************************************************/
#if _READONLY == 0
int xmit_datablock(const BYTE *buff, BYTE token)
{
  uint8_t resp;
  UINT bc = 512;

  if (wait_ready() != 0xFF)
    return 0;

  USART_Tx(MICROSD_USART, token);	        /* Xmit a token */
  if (token != 0xFD)
  { /* Not StopTran token */
    do
    { /* Xmit the 512 byte data block to the MMC */
      USART_Tx(MICROSD_USART, *buff++);
      USART_Tx(MICROSD_USART, *buff++);
    } while (bc -= 2);
    
    USART_Tx(MICROSD_USART, 0xFF);  /* CRC (Dummy) */
    USART_Tx(MICROSD_USART, 0xFF);
    resp = rcvr_spi();	        /* Receive a data response */
    if ((resp & 0x1F) != 0x05)	/* If not accepted, return with error */
      return 0;
  }
  return 1;
}
#endif	/* _READONLY */

/**************************************************************************//**
 * @brief Send a command packet to MMC    
 * @param Command byte
 * @param Argument
 * @return Response value
 *****************************************************************************/
BYTE send_cmd(BYTE cmd,	DWORD arg)
{
  BYTE n, res;
  volatile int timeOut;

  if (cmd & 0x80)
  { /* ACMD<n> is the command sequense of CMD55-CMD<n> */
    cmd &= 0x7F;
    res = send_cmd(CMD55, 0);
    if (res > 1) return res;
  }

  /* Select the card and wait for ready */
  deselect();
  if (!select())
    return 0xFF;

  /* Send command packet */
  USART_Tx(MICROSD_USART, 0x40 | cmd);		  /* Start + Command index */
  USART_Tx(MICROSD_USART, (BYTE)(arg >> 24));    /* Argument[31..24] */
  USART_Tx(MICROSD_USART, (BYTE)(arg >> 16));	  /* Argument[23..16] */
  USART_Tx(MICROSD_USART, (BYTE)(arg >> 8));	  /* Argument[15..8] */
  USART_Tx(MICROSD_USART, (BYTE)arg);		  /* Argument[7..0] */
  n = 0x01;			  /* Dummy CRC + Stop */
  if (cmd == CMD0)
    n = 0x95;		          /* Valid CRC for CMD0(0) */
  if (cmd == CMD8)
    n = 0x87;		          /* Valid CRC for CMD8(0x1AA) */
  USART_Tx(MICROSD_USART, n);

  /* Receive command response */
  if (cmd == CMD12)
    rcvr_spi();	                  /* Skip a stuff byte when stop reading */
  timeOut = 10;			  /* Wait for a valid response in timeout of 10 attempts */
  do
    res = rcvr_spi();
  while ((res & 0x80) && --timeOut);

  return res;			  /* Return with the response value */
}
