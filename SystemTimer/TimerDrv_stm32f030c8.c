/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
* @file    TimerDrv_Efm32g840.c
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
#include "../Common/UiDefine.h"
#include "../UI/KeyApi.h"
#include "../UI/DispDrv.h"
#include "../UI/KeyDrv.h"
#include "../UI/BuzzerDrv.h"
#include "../UI/BackLightDrv.h"
#include "TimerDrv.h"
#include "../Hardware/HardwareConfig.h"
#include "../Hardware/HardwareDrv.h"
#ifdef TIMER_DRV_STM32F0XX_STD
#include "stm32f0xx_tim.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_misc.h"



/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/

#define COMPMAX 32      // Letimer를 위한 maxtop설정 Letimer는 32.768 로 종작 

#ifdef USE_CLK_11MHZ
// Timer 0         : 1ms
// 171875 Hz -> 11Mhz (clock frequency) / 64 (prescaler)
//  Setting TOP to 172 results in an overflow each 1ms 
#define TOP 172
// Timer 1      : 10ms
// 10742 Hz -> 11Mhz (clock frequency) / 1024 (prescaler)
//  Setting TOP to 107 results in an overflow each 1ms 
#define TIEMR1TOP   107

#else
/* Insert #define here */
// Timer 0         : 1ms
// 15625 Hz -> 1Mhz (clock frequency) / 64 (prescaler)
//  Setting TOP to 16 results in an overflow each 1ms 
// Rev D 
// Updated the HFRCO 1 MHz band typical value to 1.2 MHz.
#define TOP 16
// Timer 0      : 10ms
// 977 Hz -> 1Mhz (clock frequency) / 1024 (prescaler)
//  Setting TOP to 10 results in an overflow each 1ms 
// Rev D 
// Updated the HFRCO 1 MHz band typical value to 1.2 MHz.
#define TIEMR1TOP   10
#endif


/*
// Timer 0
// 109375 Hz -> 7Mhz (clock frequency) / 64 (prescaler)
//  Setting TOP to 16 results in an overflow each 1ms 
#define TOP 110
#define TIEMR1TOP   110
*/




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
/** @brief 10 ms Timer Value*/
static INT16U	sysTimer10ms;
/** @brief 100 ms Timer Value*/
static INT16U	sysTimer100ms;

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
/* Insert global functions here */
/**
********************************************************************************
* @brief    Timer Driver Init 함수\n
*           Timer Driver 초기화\n
* @param    none
* @return   none
* @remark   CPU Timer Setting (Timer0, Timer1)
*           초기에 한번 실행 되는 함수\n
********************************************************************************
*/


void TimerDrvInit(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  
  /* TIM2,TIM3 Clock Enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

   /* Enable TIM2 Global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    /* TIM2 Initialize */    
    TIM_TimeBaseStructure.TIM_Period=1; // 100 ms FDC954 1ms 
    TIM_TimeBaseStructure.TIM_Prescaler=24000-1; // 1 kHz
    TIM_TimeBaseStructure.TIM_ClockDivision=0;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
  
      /* TIM1 Enale */
    TIM_Cmd(TIM2,ENABLE);
    TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE); // interrupt enable
  
  /* Enable TIM3 Global Interrupt */  
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 4;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
    /* TIM3 Initialize */    
    TIM_TimeBaseStructure.TIM_Period=10; // 10 ms
    TIM_TimeBaseStructure.TIM_Prescaler=24000-1; // 1 kHz
    TIM_TimeBaseStructure.TIM_ClockDivision=0;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
  
      /* TIM3 Enale */
    TIM_Cmd(TIM3,ENABLE);
    TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE); // interrupt enable
}

/**
********************************************************************************
* @brief    10ms 단위로 돌아가는 System Timer 값을 읽는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
INT16U TimerDrvGetSysTimer10ms(void)
{
	return sysTimer10ms;
}

/**
********************************************************************************
* @brief    100ms 단위로 돌아가는 System Timer 값을 읽는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
INT16U TimerDrvGetSysTimer100ms(void)
{
	return sysTimer100ms;
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */



/**
********************************************************************************
* @brief    Timer2 overflow Interrupt 함수(per TIMER1_FREQ)\n
* @param    none
* @return   none
* @remark   sysTimer 증가, KeyProc 실행
********************************************************************************
*/
void TIM2_IRQHandler(void)
{ 
    if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
    {
      TIM_ClearITPendingBit(TIM2, TIM_IT_Update); // Clear the interrupt flag
      //sysTimer100ms++;
    
    
    //FDC954

#ifdef USE_BACK_LIGHT
    BlDrvProc();
#endif//#ifdef USE_BACK_LIGHT
#ifdef USE_DISP_DRV_PROC_FOR_INTERRUPT
    DispDrvProcForInterrupt();
#endif//#ifdef USE_DISP_DRV_PROC_FOR_INTERRUPT
    KeyDrvProc();
    //FDC954  
    }
}


/**
********************************************************************************
* @brief    Timer3 Overflosw Interrupt 함수(per TIMER1_FREQ = 10 ms)\n
* @param    none
* @return   none
* @remark   sysTimer10ms, sysTimer100ms 증가, BuzzerDrvProc 실행
********************************************************************************
*/

void TIM3_IRQHandler(void)
{ 
    if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
    {
      TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // Clear the interrupt flag
      sysTimer10ms++;

#ifdef STM32F0XX_AD 
      AdStm32f0xxProc();
#endif
    }    
      //FDC954
      if (!(sysTimer10ms%5))  
      {	// 50 ms
        BuzzerDrvProc();
      }
      if (!(sysTimer10ms%10))  
      {	// 100 ms
        sysTimer100ms++;
      }
      //FDC954
    
}

#endif//#ifdef TIMER_DRV_STM32F0XX_STD
