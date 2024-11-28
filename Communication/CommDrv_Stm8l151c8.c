/**
********************************************************************************
* Copyright (c) 2012 CAS
* @brief   Communication Driver Source File\n
* @file    CommDrv_Stm8l151c8.c
* @version 1.0
* @date    2012/09/24
* @author  Young Hoon Lee (LYH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/24 created by LYH \n
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

#ifdef COMM_DRV_STM8L151C8
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

#define RXBUFSIZE       8

#define UART_PERCLK_FREQUENCY 1000000
/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
static char rxData[SERIAL_RX_BUF_SIZE];

/** @brief Communication Driver Tx Ring Buffer에서 사용할 byte buffer arrary */
static char txData[SERIAL_TX_BUF_SIZE];

/** @brief Tx Data Transmit 중임을 알리는 Flag, 전송중 전송 방지  */
BOOLEAN serialTxing;
/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/

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
/**
  * @brief	Configures COM port.
  * @param	COM: Specifies the COM port to be configured.
  *   This parameter should be COM1.
  * @param	USART_InitStruct: pointer to a USART_InitTypeDef structure that
  *   contains the configuration information for the specified USART peripheral.
  * @retval None
  */
  
void STM_COMInit(USART_TypeDef* USARTx, uint32_t USART_BaudRate,
					  USART_WordLength_TypeDef USART_WordLength,
					  USART_StopBits_TypeDef USART_StopBits,
					  USART_Parity_TypeDef USART_Parity,
					  USART_Mode_TypeDef USART_Mode)
{
  /* Enable USART clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);

  /* Configure USART Tx as alternate function push-pull  (software pull up)*/
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, ENABLE);

  /* Configure USART Rx as alternate function push-pull  (software pull up)*/
  //GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, ENABLE); // GPIO Pin mode : input pull-up, no external interrupt

  /* USART configuration */
  USART_Init(USARTx, USART_BaudRate,
			 USART_WordLength,
			 USART_StopBits,
			 USART_Parity,
			 USART_Mode);
}

static void USART_Config(void)
{
  /* USART configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - Odd parity
        - Receive and transmit enabled
        - USART Clock disabled
  */
  STM_COMInit(USART1, (uint32_t)9600, USART_WordLength_8b, USART_StopBits_1,
                   USART_Parity_No, (USART_Mode_Tx | USART_Mode_Rx));

//  USART_ClockInit(USART1, USART_Clock_Enable, USART_CPOL_High, USART_CPHA_2Edge,USART_LastBit_Enable);
  /* Enable general interrupts */
  enableInterrupts();

  /* Enable the USART Receive interrupt: this interrupt is generated when the USART
    receive data register is not empty */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  /* Enable the USART Transmit complete interrupt: this interrupt is generated when the USART
    transmit Shift Register is empty */
  USART_ITConfig(USART1, USART_IT_TC, ENABLE);

  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}



/**
********************************************************************************
* @brief    Communication Driver Init 함수\n
*           Serial Communication Ring Buffer Init\n
*           Serial Communication Speed Setting\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void CommDrvInit(INT8U baud)
{
    baud = 0;// crystal 선정후 삭제 


	RingBufInit(&SerialRxRingBuf, &rxData[0], SERIAL_RX_BUF_SIZE);
	RingBufInit(&SerialTxRingBuf, &txData[0], SERIAL_TX_BUF_SIZE);
	
	serialTxing = OFF;
	
	GPIO_Init(GPIOC, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Slow);
	
	GPIO_Init(GPIOC, GPIO_Pin_2, GPIO_Mode_In_PU_No_IT);

	USART_Config();

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
	char temp;
	if (RingBufCheckBuf(&SerialTxRingBuf))
	{
		serialTxing = ON;
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	}
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */


#endif//#ifdef COMM_DRV_STM8L151C8
