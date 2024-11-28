/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Communication Driver Source File\n
* @file    CommDrv_Efm32g840.c
* @version 1.0
* @date    2012/09/19
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/19 created by KDH \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "../Common/RingBuf.h"
#include "CommDrv.h"

#include "../Hardware/HardwareConfig.h"
#include "efm32_cmu.h"
#include "efm32_gpio.h"
#include "efm32_usart.h"
#include "../Communication/PrtApi.h"

#ifdef COMM_DRV_EFM32G840
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Communication Driver Rx Ring Buffer */
RING_BUF SerialRxRingBuf;
/** @brief Communication Driver Tx Ring Buffer */
RING_BUF SerialTxRingBuf;
#ifdef USE_UART1 
/** @brief Communication Driver Rx Ring Buffer */
RING_BUF SerialRx1RingBuf;
/** @brief Communication Driver Tx Ring Buffer */
RING_BUF SerialTx1RingBuf;
#endif

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */


/** @brief Communication Driver Rx Ring Buffer Size */
#define SERIAL_RX_BUF_SIZE	32
/** @brief Communication Driver Tx Ring Buffer Size */
#define SERIAL_TX_BUF_SIZE	72
#ifdef USE_UART1 
/** @brief Communication Driver Rx Ring Buffer Size */
#define SERIAL_RX1_BUF_SIZE	32
/** @brief Communication Driver Tx Ring Buffer Size */
#define SERIAL_TX1_BUF_SIZE	72
#endif
#define RXBUFSIZE       8

#define UART_PERCLK_FREQUENCY 1000000

#define USART0_NUM  0
#define USART1_NUM  1
#define USART2_NUM  2

#define GPIO_POS0 0
#define GPIO_POS1 1
#define GPIO_POS2 2

#define UART_IF_TXBL                         (0x1UL << 1)                    /**< TX Buffer Level Interrupt Flag */


/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */


/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief Communication Driver Rx Ring Buffer에서 사용할 byte buffer arrary */
static char rxData[SERIAL_RX_BUF_SIZE];

/** @brief Communication Driver Tx Ring Buffer에서 사용할 byte buffer arrary */
static char txData[SERIAL_TX_BUF_SIZE];

#ifdef USE_UART1 
/** @brief Communication Driver Rx Ring Buffer에서 사용할 byte buffer arrary */
static char rx1Data[SERIAL_RX1_BUF_SIZE];
/** @brief Communication Driver Tx Ring Buffer에서 사용할 byte buffer arrary */
static char tx1Data[SERIAL_TX1_BUF_SIZE];
#endif


/** @brief Tx Data Transmit 중임을 알리는 Flag, 전송중 전송 방지  */
static BOOLEAN serialTxing;
#ifdef USE_UART1 
static BOOLEAN serialTxing1;
#endif

static volatile INT8S	rxReadIndex  = 0;
static volatile INT8S	rxWriteIndex = 0;
static volatile INT8S	rxCount      = 0;
static volatile INT8U	rxBuffer[RXBUFSIZE];

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */

#include "efm32_chip.h"
/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
#define USART_CHANGE_DEFAULT                                                              \
  { usartEnable,      /* Enable RX/TX when init completed. */                                \
    0,                /* Use current configured reference clock for configuring baudrate. */ \
    57600,           /* 115200 bits/s. */                                                   \
    usartOVS8,       /* 16x oversampling. */                                                \
    usartDatabits8,   /* 8 databits. */                                                      \
    usartNoParity,    /* No parity. */                                                       \
    usartStopbits1    /* 1 stopbit. */                                                       \
  }
static USART_TypeDef           * uart   = USART1;
static USART_TypeDef           * uart0   = USART0;
static USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;
static USART_InitAsync_TypeDef uartChangeInit = USART_CHANGE_DEFAULT;
void cmuSetup(void)
{
        CHIP_Init();
  /* Start HFXO and wait until it is stable */
  /* CMU_OscillatorEnable( cmuOsc_HFXO, true, true); */

  /* Select HFXO as clock source for HFCLK */
  /* CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO ); */

  /* Disable HFRCO */
  /* CMU_OscillatorEnable( cmuOsc_HFRCO, false, false ); */

  /* Enable clock for HF peripherals */
  CMU_ClockEnable(cmuClock_HFPER, true);

  /* Enable clock for USART module */
  CMU_ClockEnable(cmuClock_USART1, true);
 
  /* Enable clock for USART module */
  CMU_ClockEnable(cmuClock_USART0, true);
  
  
    /* Enable clock for GPIO module */
  CMU_ClockEnable(cmuClock_GPIO, true);
  
  /* Enable clock for TIMER0 module */
  CMU_ClockEnable(cmuClock_TIMER0, true);
  
  
  GPIO_PinModeSet(gpioPortD, 5, gpioModePushPull, 1);\
  GPIO_PinModeSet(gpioPortC, 13, gpioModePushPull, 0);

  
}


void uartSetup(void)
{
  /* Enable clock for GPIO module (required for pin configuration) */
  CMU_ClockEnable(cmuClock_GPIO, true);
  /* Configure GPIO pins */
  GPIO_PinModeSet(gpioPortD, 0, gpioModePushPull, 1);
  GPIO_PinModeSet(gpioPortD, 1, gpioModeInputPull, 0);


  /* Prepare struct for initializing UART in asynchronous mode*/
  uartInit.enable       = usartDisable;   /* Don't enable UART upon intialization */
  uartInit.refFreq      = 0;              /* Provide information on reference frequency. When set to 0, the reference frequency is */
  uartInit.baudrate     = 115200;         /* Baud rate */
  uartInit.oversampling = usartOVS4;     /* Oversampling. Range is 4x, 6x, 8x or 16x */
  uartInit.databits     = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
  uartInit.parity       = usartNoParity;  /* Parity mode */
  uartInit.stopbits     = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */

  /* Initialize USART with uartInit struct */
  USART_InitAsync(uart, &uartInit);

  /* Prepare UART Rx and Tx interrupts */
  USART_IntClear(uart, _USART_IF_MASK);
  USART_IntEnable(uart, USART_IF_RXDATAV);
    
  NVIC_ClearPendingIRQ(USART1_RX_IRQn);
  NVIC_ClearPendingIRQ(USART1_TX_IRQn);
  NVIC_EnableIRQ(USART1_RX_IRQn);
  NVIC_EnableIRQ(USART1_TX_IRQn);

  /* Enable I/O pins at UART1 location #2 */
  uart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC1;

  /* Enable UART */
  USART_Enable(uart, usartEnable);
}



/* Insert global functions here */
/**************************************************************************//**
 * @brief Setup a USART to be used in asynchronous (UART) mode
 * @param usartNumber is the number of the USART to use (e.g. 1 for USART1)
 * @param location is the IO location to use
 *****************************************************************************/
void UART_setupUart(uint8_t uartNumber, uint8_t location, uint8_t baudrate)
{
  USART_TypeDef* uart;
  INT8U overSample = 0;
  USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;

  /* Determining USART */
  switch (uartNumber)
  {
	  case 0:
		uart = USART0;
		break;
	  case 1:
		uart = USART1;
		break;
	  case 2:
		uart = USART2;
		break;
	  default:
		return;
  }
  
      /* Prepare struct for initializing UART in asynchronous mode*/
    uartInit.enable       = usartDisable;   /* Don't enable UART upon intialization */
    uartInit.refFreq      = 0;              /* Provide information on reference frequency. When set to 0, the reference frequency is */
    uartInit.oversampling = usartOVS8;     /* Oversampling. Range is 4x, 6x, 8x or 16x */
    uartInit.databits     = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
    uartInit.parity       = usartNoParity;  /* Parity mode */
    uartInit.stopbits     = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
  
  
  
    switch(baudrate)
    {
        case BAUD_RATE_9600:
            uartInit.baudrate     = 9600;
                break;
        case BAUD_RATE_19200:
            uartInit.baudrate     = 19200;
                break;
        case BAUD_RATE_38400:
            uartInit.baudrate     = 38400;
                break;
        case BAUD_RATE_57600:
            uartInit.baudrate     = 57600;
                break;
        case BAUD_RATE_115200:
            uartInit.baudrate     = 115200;
                break;
        case BAUD_RATE_2400:
            uartInit.baudrate     = 2400;
                break;
        default:
            uartInit.baudrate     = 9600;
            break;
    }
    USART_InitAsync(uart, &uartInit);
    /* Prepare UART Rx and Tx interrupts */
    USART_IntClear(uart, _USART_IF_MASK);
    USART_IntEnable(uart, USART_IF_RXDATAV);

    switch (uartNumber)
    {
      case 0:
        NVIC_ClearPendingIRQ(USART0_RX_IRQn);
        NVIC_ClearPendingIRQ(USART0_TX_IRQn);
        NVIC_EnableIRQ(USART0_RX_IRQn);
        NVIC_EnableIRQ(USART0_TX_IRQn);
        break;
      case 1:
        NVIC_ClearPendingIRQ(USART1_RX_IRQn);
        NVIC_ClearPendingIRQ(USART1_TX_IRQn);
        NVIC_EnableIRQ(USART1_RX_IRQn);
        NVIC_EnableIRQ(USART1_TX_IRQn);
        break;
      default:
        NVIC_ClearPendingIRQ(USART0_RX_IRQn);
        NVIC_ClearPendingIRQ(USART0_TX_IRQn);
        NVIC_EnableIRQ(USART0_RX_IRQn);
        NVIC_EnableIRQ(USART0_TX_IRQn);
        break;
    }

        
  /* Enable I/O pins at UART1 location #2 */
    switch (location)
    {
      case 0:
        uart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC0;
        break;
      case 1:
        uart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC1;
        break;
      case 2:
        uart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC2;
        break;
      default:
        uart->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | USART_ROUTE_LOCATION_LOC0;
        break;
    }
    
  /* Enable UART */
  USART_Enable(uart, usartEnable);
}

/**
********************************************************************************
* @brief    Communication Driver Init 함수\n
*           Serial Communication Ring Buffer Init\n
*           Serial Communication Speed Setting\n
* @param    none
* @return   none
* @remark   일정 시간에 한번씩 계속 실행 되는 함수\n
********************************************************************************
*/
void CommDrvInit(INT8U baud)
{
    baud = 0;// crystal 선정후 삭제 


	RingBufInit(&SerialRxRingBuf, &rxData[0], SERIAL_RX_BUF_SIZE);
	RingBufInit(&SerialTxRingBuf, &txData[0], SERIAL_TX_BUF_SIZE);
	
	#ifdef USE_UART1 
	RingBufInit(&SerialRx1RingBuf, &rx1Data[0], SERIAL_RX1_BUF_SIZE);
	RingBufInit(&SerialTx1RingBuf, &tx1Data[0], SERIAL_TX1_BUF_SIZE);
	#endif
	serialTxing = OFF;

	UART_setupUart(USART0_NUM, GPIO_POS0, baud);
	#ifdef USE_UART1 
	UART_setupUart(USART1_NUM, GPIO_POS1, baud);
	#endif


    #ifdef USE_UART1 
	serialTxing = OFF;
    #endif
}

/**
********************************************************************************
* @brief    Communication Driver Procedure 함수\n
*           Communication Driver Tx Ring Buffer 확인 후 Data 보냄
*           (Serial Interrupt Flag On 시킴)
* @param    none
* @return   none
* @remark   일정 시간에 한번씩 계속 실행 되는 함수\n
********************************************************************************
*/
void  CommDrvProc(void)
{
  USART_TypeDef *uart0 = USART0;
  static INT8U comTemp = 1;
  if (comTemp == 1)
  {
      uart0->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
      comTemp = 0;
  }
  if (RingBufCheckBuf(&SerialTxRingBuf))
  {
	serialTxing = ON;
    USART_IntEnable(uart0, UART_IF_TXBL);
  }
}

#ifdef USE_UART1
void  Comm1DrvProc(void)
{
	USART_TypeDef *uart = USART1;	
	static INT8U com1Temp = 1;
	
	if (com1Temp == 1)
	{
		uart->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
		com1Temp = 0;
	}
	
	if (RingBufCheckBuf(&SerialTx1RingBuf))
	{
		serialTxing1 = ON;
		USART_IntEnable(uart, UART_IF_TXBL);
	}
}
#endif

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Communication Driver Interrupt 함수\n
* @param    none
* @return   none
* @remark   8051계열의 Serial Interrpt 처리 (Tx, Rx)\n
*           받은 Data가 있으면 Rx Ring Buffer에 저장\n
*           보낼 Data가 있으면 Tx Ring Buffer에서 가져와 전송
********************************************************************************
*/
//interrupt [0x23] void serialInt (void)    /* Serial Port */ 
/*
 void serialInt (void) interrupt 4
{
	INT8U temp;

	if (RI) 
	{
		temp = SBUF;
		RingBufPutCharForCommInterrupt(&SerialRxRingBuf, temp);
		RI = 0;
	}

	if (TI)
	{
		if (RingBufCheckBufForCommInterrupt(&SerialTxRingBuf))
		{
			SBUF = RingBufGetCharForCommInterrupt(&SerialTxRingBuf);
		}
		else
		{
			serialTxing = OFF;
		}
		TI = 0;
	}
}
*/

/**
********************************************************************************
* @brief    Communication Driver Interrupt 함수\n
* @param    none
* @return   none
* @remark   8051계열의 Serial Interrpt 처리 (Tx, Rx)\n
*           받은 Data가 있으면 Rx Ring Buffer에 저장\n
*           보낼 Data가 있으면 Tx Ring Buffer에서 가져와 전송
********************************************************************************
*/
/**************************************************************************//**
 * @brief USART2 TX IRQ Handler
 * Sending until all data has been sent.
 *****************************************************************************/
void USART0_TX_IRQHandler(void)
{
  USART_TypeDef *uart0 = USART0;

   /* Clear TX interrupts */
   uart0->IFC |= USART_IFC_TXC;
   
  /* Uart checking the data*/
  if (uart0->STATUS & USART_STATUS_TXBL)
  {
    /* Waiting for the usart to be ready */
    if (RingBufCheckBufForCommInterrupt(&SerialTxRingBuf))
    {
      uart0->TXDATA = RingBufGetCharForCommInterrupt(&SerialTxRingBuf);
    }
    else
    {
      serialTxing = OFF;
        USART_IntDisable(uart0, UART_IF_TXBL);

    }
    //TI = 0;
  }
#ifdef	USE_RS485
    /*Waiting for transmission of last byte */
    while (!(uart0->STATUS & USART_STATUS_TXC)) ;
#else
    /*Waiting for transmission of last byte */
    //while (!(uart->STATUS & USART_STATUS_TXC)) ;
#endif
}


/**
********************************************************************************
* @brief    Communication Driver Interrupt 함수\n
* @param    none
* @return   none
* @remark   8051계열의 Serial Interrpt 처리 (Tx, Rx)\n
*           받은 Data가 있으면 Rx Ring Buffer에 저장\n
*           보낼 Data가 있으면 Tx Ring Buffer에서 가져와 전송
********************************************************************************
*/
/**************************************************************************//**
 * @brief USART1 IRQ Handler
 * set up the interrupt prior to use
 *****************************************************************************/
void USART0_RX_IRQHandler(void)
{
  USART_TypeDef *uart0 = USART0;
  uint8_t		rxdata;

  /* Checking that RX-flag is set*/
  if (uart0->STATUS & USART_STATUS_RXDATAV)
  {
	    rxdata = uart0->RXDATA;
        RingBufPutCharForCommInterrupt(&SerialRxRingBuf, rxdata);
        //RI = 0;
        
        /* Disable interrupt */
        //uart->IEN &= ~USART_IEN_RXDATAV;
        //NVIC_ClearPendingIRQ(USART1_RX_IRQn);
        //NVIC_DisableIRQ(USART1_RX_IRQn);
  }
}

/**
********************************************************************************
* @brief    Communication Driver Interrupt 함수\n
* @param    none
* @return   none
* @remark   8051계열의 Serial Interrpt 처리 (Tx, Rx)\n
*           받은 Data가 있으면 Rx Ring Buffer에 저장\n
*           보낼 Data가 있으면 Tx Ring Buffer에서 가져와 전송
********************************************************************************
*/
#ifdef USE_UART1
/**************************************************************************//**
 * @brief USART2 TX IRQ Handler
 * Sending until all data has been sent.
 *****************************************************************************/
void USART1_TX_IRQHandler(void)
{
  USART_TypeDef *uart = USART1;

   /* Clear TX interrupts */
   uart->IFC |= USART_IFC_TXC;
   
  /* Uart checking the data*/
  if (uart->STATUS & USART_STATUS_TXBL)
  {
    /* Waiting for the usart to be ready */
    if (RingBufCheckBufForCommInterrupt(&SerialTx1RingBuf))
    {
        #ifdef	USE_RS485   
        //Rs485CsSet(ON);     //RS-485 CS Enable
        RS485_CS_ON;
        #endif
		uart->TXDATA = RingBufGetCharForCommInterrupt(&SerialTx1RingBuf);
    }
    else
    {
        USART_IntDisable(uart, UART_IF_TXBL);
        #ifdef	USE_RS485  
        //Rs485CsSet(OFF);    //RS-485 CS Disable
        RS485_CS_OFF;
        #endif
        serialTxing1 = OFF;
    }
  }  
#ifdef	USE_RS485
    /*Waiting for transmission of last byte */
    while (!(uart->STATUS & USART_STATUS_TXC)) ;
#else
    /*Waiting for transmission of last byte */
    //while (!(uart->STATUS & USART_STATUS_TXC)) ;
#endif
  if (uart->CTRL & USART_CTRL_ERRSRX || uart->CTRL & USART_CTRL_ERRSTX)
  {
	uart->CTRL = _USART_CTRL_ERRSRX_DEFAULT;
  }
}

/**
********************************************************************************
* @brief    Communication Driver Interrupt 함수\n
* @param    none
* @return   none
* @remark   8051계열의 Serial Interrpt 처리 (Tx, Rx)\n
*           받은 Data가 있으면 Rx Ring Buffer에 저장\n
*           보낼 Data가 있으면 Tx Ring Buffer에서 가져와 전송
********************************************************************************
*/
/**************************************************************************//**
 * @brief USART1 IRQ Handler
 * set up the interrupt prior to use
 *****************************************************************************/
void USART1_RX_IRQHandler(void)
{
    USART_TypeDef *uart = USART1;
    uint8_t		rxdata;
    
    #ifdef	USE_RS485 
    RS485_CS_OFF;
    delay5Clock();
    #endif
/*
	if (uart->CTRL & USART_CTRL_ERRSRX || uart->CTRL & USART_CTRL_ERRSTX)
    {
		uart->CTRL = _USART_CTRL_ERRSRX_DEFAULT;
    }
	if(uart->IF & USART_IF_RXOF)
	{
		uart->IFS &= ~_USART_IF_RXOF_MASK;
		//rxdata = uart->RXDATA; 
	}
	if(uart->IF & USART_IF_RXUF)
	{
		uart->IFS &= ~_USART_IF_RXUF_MASK;
	}
	if(uart->IF & USART_IF_FERR)
	{
  		uart->IFS &= ~_USART_IF_FERR_MASK;
	}
*/
    /* Checking that RX-flag is set*/
    if (uart->STATUS & USART_STATUS_RXDATAV)
    {
        rxdata = uart->RXDATA;
        RingBufPutCharForCommInterrupt(&SerialRx1RingBuf, rxdata);
    }
}
#endif//#ifdef USE_UART1


#ifdef	USE_RS485  
/*
**
********************************************************************************
* @brief    Relayon/off setting 함수 (editting Page)
* @param    num : Relayon 번호\n
*           on : 1 -> 켜기, 0 -> 끄기
* @return   none
* @remark   Relayon 번호 정의되어 있음 (RELAY_HIGH...)
********************************************************************************
*/
void Rs485DrvSet(INT8U on)
{
    if (on)
    {
        RS485_CS_ON;
    }
    else
    {
        RS485_CS_OFF;
    }
}

#endif//#ifdef USE_RS485
#endif//#ifdef COMM_DRV_C51


