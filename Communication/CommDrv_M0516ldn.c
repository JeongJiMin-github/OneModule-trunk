/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   Communication Driver Source File\n
           Chip : M0516LDN
* @file    CommDrv_M0516ldn.c
* @version 1.0
* @date    2016/12/06
* @author  Keum Young Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2016/12/06 created by KYK \n
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
#include "../Communication/PrtApi.h"

#ifdef COMM_DRV_M0516LDN
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

#define USART1_NUM  1
#define USART2_NUM  2


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
#ifdef USE_UART2 
static BOOLEAN serialTxing1;
#endif

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
void UART_setupUart(uint8_t uartNumber, uint8_t baud)
{
    INT32U  baudrate;
    
    switch(baud)
    {
        case BAUD_RATE_9600:
            baudrate     = 9600;
                break;
        case BAUD_RATE_19200:
            baudrate     = 19200;
                break;
        case BAUD_RATE_38400:
            baudrate     = 38400;
                break;
        case BAUD_RATE_57600:
            baudrate     = 57600;
                break;
        case BAUD_RATE_115200:
            baudrate     = 115200;
                break;
        case BAUD_RATE_2400:
            baudrate     = 2400;
                break;
        default:
            baudrate     = 9600;
            break;
    }
    
   /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
	if (uartNumber == 1)
	{
		SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
		UART_Open(UART0, baudrate);
	}
    
#ifdef USE_UART1
	if (uartNumber == 2)
	{
    /* Reset IP */
		SYS_ResetModule(UART1_RST);
    /* Configure UART1 and set UART1 Baudrate */
		UART_Open(UART1, baudrate);
	}
	//UART_Open(UART1, BAUD_RATE_115200);
#endif
}

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */

/**************************************************************************//**
 * @brief Setup a USART to be used in asynchronous (UART) mode
 * @param usartNumber is the number of the USART to use (e.g. 1 for USART1)
 * @param location is the IO location to use
 *****************************************************************************/


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
#ifdef USE_UART1_BAUDRATE_SET
void CommDrvInit(INT8U uart0_baud, INT8U uart1_baud)
{
	RingBufInit(&SerialRxRingBuf, &rxData[0], SERIAL_RX_BUF_SIZE);
	RingBufInit(&SerialTxRingBuf, &txData[0], SERIAL_TX_BUF_SIZE);
	
#ifdef USE_UART1
	RingBufInit(&SerialRx1RingBuf, &rx1Data[0], SERIAL_RX1_BUF_SIZE);
	RingBufInit(&SerialTx1RingBuf, &tx1Data[0], SERIAL_TX1_BUF_SIZE);
#endif
	serialTxing = OFF;
	UART_setupUart(1, uart0_baud);

    /* Enable Interrupt and install the call back function */
    UART_ENABLE_INT(UART0, (UART_IER_RDA_IEN_Msk | UART_IER_RTO_IEN_Msk)); //rx
    NVIC_EnableIRQ(UART0_IRQn);
#ifdef USE_UART1 
    serialTxing = OFF;
	UART_setupUart(2, uart1_baud);

    /* Enable Interrupt and install the call back function */
    UART_ENABLE_INT(UART1, (UART_IER_RDA_IEN_Msk | UART_IER_RTO_IEN_Msk)); //rx
    NVIC_EnableIRQ(UART1_IRQn);
#endif
}

#else
void CommDrvInit(INT8U baud)
{
	RingBufInit(&SerialRxRingBuf, &rxData[0], SERIAL_RX_BUF_SIZE);
	RingBufInit(&SerialTxRingBuf, &txData[0], SERIAL_TX_BUF_SIZE);
	
#ifdef USE_UART1 
	RingBufInit(&SerialRx1RingBuf, &rx1Data[0], SERIAL_RX1_BUF_SIZE);
	RingBufInit(&SerialTx1RingBuf, &tx1Data[0], SERIAL_TX1_BUF_SIZE);
#endif
	serialTxing = OFF;
	UART_setupUart(1, baud);

    /* Enable Interrupt and install the call back function */
    UART_ENABLE_INT(UART0, (UART_IER_RDA_IEN_Msk | UART_IER_RTO_IEN_Msk)); //rx
    NVIC_EnableIRQ(UART0_IRQn);
#ifdef USE_UART1 
    serialTxing = OFF;
	UART_setupUart(2, baud);

    /* Enable Interrupt and install the call back function */
    UART_ENABLE_INT(UART1, (UART_IER_RDA_IEN_Msk | UART_IER_RTO_IEN_Msk)); //rx
    NVIC_EnableIRQ(UART1_IRQn);
#endif

}
#endif

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
    if (RingBufCheckBuf(&SerialTxRingBuf))
    {
        serialTxing = ON;
        
        /* Enable Interrupt and install the call back function */
        UART_ENABLE_INT(UART0, UART_IER_THRE_IEN_Msk);
        NVIC_EnableIRQ(UART0_IRQn);
    }
}
#ifdef USE_UART1
void  Comm1DrvProc(void)
{
    if (RingBufCheckBuf(&SerialTx1RingBuf))
    {
        serialTxing = ON;
        
        /* Enable Interrupt and install the call back function */
        UART_ENABLE_INT(UART1, UART_IER_THRE_IEN_Msk);
        NVIC_EnableIRQ(UART1_IRQn);
    }
}
#endif


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

/**************************************************************************//**
 * @brief USART1 IRQ Handler
 * Sending until all data has been sent.
 *****************************************************************************/

/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 0 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_IRQHandler(void)
{
    INT8U trxdata;
    INT32U u32IntSts = UART0->ISR;

    //Rx Int
    if(u32IntSts & UART_ISR_RDA_INT_Msk)
    {
        /* Get all the input characters */
        while(UART_IS_RX_READY(UART0))
        {
            /* Get the character from UART Buffer */
            trxdata = UART_READ(UART0);
            RingBufPutCharForCommInterrupt(&SerialRxRingBuf, trxdata);
        }
    }
    
    //Tx Int
    if(u32IntSts & UART_ISR_THRE_INT_Msk)
    {
        if (RingBufCheckBufForCommInterrupt(&SerialTxRingBuf))
        {
            trxdata = RingBufGetCharForCommInterrupt(&SerialTxRingBuf);
            UART_WRITE(UART0, trxdata); // Transmit the character
        }
        else
        {
            serialTxing = OFF;
            UART_DISABLE_INT(UART0, (UART_IER_THRE_IEN_Msk));
            //NVIC_DisableIRQ(UART0_IRQn);
        }
#ifdef	USE_RS485
        /*Waiting for transmission of last byte */
#else
        /*Waiting for transmission of last byte */
        //while (!(uart->STATUS & USART_STATUS_TXC)) ;
#endif
    }
}
/**************************************************************************//**
 * @brief USART1 IRQ Handler
 * Sending until all data has been sent.
 *****************************************************************************/
#ifdef USE_UART1
void UART1_IRQHandler(void)
{
    uint8_t		trxdata;
    uint32_t u32IntSts = UART1->ISR;

    //Rx Int
    if(u32IntSts & UART_ISR_RDA_INT_Msk)
    {
        /* Get all the input characters */
        while(UART_IS_RX_READY(UART1))
        {
            /* Get the character from UART Buffer */
            trxdata = UART_READ(UART1);
            RingBufPutCharForCommInterrupt(&SerialRx1RingBuf, trxdata);
        }
    }
    
    //Tx Int
    if(u32IntSts & UART_ISR_THRE_INT_Msk)
    {
        if (RingBufCheckBufForCommInterrupt(&SerialTx1RingBuf))
        {
            trxdata = RingBufGetCharForCommInterrupt(&SerialTx1RingBuf);
            UART_WRITE(UART1, trxdata); // Transmit the character
        }
        else
        {
            serialTxing = OFF;
            UART_DISABLE_INT(UART1, (UART_IER_THRE_IEN_Msk));
            //NVIC_DisableIRQ(UART1_IRQn);
        }
    }
}
#endif // USE_UART1

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


