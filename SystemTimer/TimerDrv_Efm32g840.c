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
#ifdef TIMER_DRV_EFM32G840_STD
#include "../AD/AdDrv_Efm32g840.h"



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

#if defined (USE_CLK_11MHZ)
// Timer 0         : 1ms
// 171875 Hz -> 11Mhz (clock frequency) / 64 (prescaler)
//  Setting TOP to 172 results in an overflow each 1ms 
#define TOP 172
// Timer 1      : 10ms
// 10742 Hz -> 11Mhz (clock frequency) / 1024 (prescaler)
//  Setting TOP to 107 results in an overflow each 1ms 
#define TIEMR1TOP   107

#elif defined (USE_CLK_7MHZ)
// Timer 0         : 1ms
// 109375 Hz -> 7Mhz (clock frequency) / 64 (prescaler)
//  Setting TOP to 109 results in an overflow each 1ms 
// Rev D 
// Updated the HFRCO 7 MHz band typical value to 6.6 MHz.
#define TOP 109
// Timer 1      : 10ms
// 6835 Hz -> 7Mhz (clock frequency) / 1024 (prescaler)
//  Setting TOP to 68 results in an overflow each 1ms 
// Rev D 
// Updated the HFRCO 7 MHz band typical value to 6.6 MHz.
#define TIEMR1TOP   68

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
  TIMER_Init_TypeDef timerInit =
  {
    .enable     = true, 
    .debugRun   = true, 
    .prescale   = timerPrescale64, 
    .clkSel     = timerClkSelHFPerClk, 
    .fallAction = timerInputActionNone, 
    .riseAction = timerInputActionNone, 
    .mode       = timerModeUp, 
    .dmaClrAct  = false,
    .quadModeX4 = false, 
    .oneShot    = false, 
    .sync       = false, 
  };

  TIMER_Init_TypeDef timer1Init =
  {
    .enable     = true, 
    .debugRun   = true, 
    .prescale   = timerPrescale1024, 
    .clkSel     = timerClkSelHFPerClk, 
    .fallAction = timerInputActionNone, 
    .riseAction = timerInputActionNone, 
    .mode       = timerModeUp, 
    .dmaClrAct  = false,
    .quadModeX4 = false, 
    .oneShot    = false, 
    .sync       = false, 
  };
  
  /* Enable overflow interrupt */
  TIMER_IntEnable(TIMER0, TIMER_IF_OF);
  
  /* Enable TIMER0 interrupt vector in NVIC */
  NVIC_EnableIRQ(TIMER0_IRQn);
  
  /* Set TIMER Top value */
  TIMER_TopSet(TIMER0, TOP + Add_Top);
  
  /* Configure TIMER */
  TIMER_Init(TIMER0, &timerInit);



  
  /* Enable overflow interrupt */
  TIMER_IntEnable(TIMER1, TIMER_IF_OF);
  
  /* Enable TIMER0 interrupt vector in NVIC */
  NVIC_EnableIRQ(TIMER1_IRQn);
  
  /* Set TIMER Top value */
  TIMER_TopSet(TIMER1, TIEMR1TOP + Add_TIEMR1TOP);
  
  /* Configure TIMER */
  TIMER_Init(TIMER1, &timer1Init);
  
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
* @brief    Timer0 overflow Interrupt 함수(per TIMER0_FREQ)\n
* @param    none
* @return   none
* @remark   sysTimer 증가, KeyProc 실행
********************************************************************************
*/
// using option for high speed
// using option은 반환값이 없는 경우 사용 가능함
// 내부에서 호출되는 함수에서 다시 호출되는 공용함수가 없어야함(주의!!!)
void TIMER0_IRQHandler(void)
{ 
  
/* Clear flag for TIMER0 overflow interrupt */
  TIMER_IntClear(TIMER0, TIMER_IF_OF);  

#ifdef EFM32G840_AD
    AdEfm32g840Proc();
#endif
#ifdef USE_BACK_LIGHT
	BlDrvProc();
#endif//#ifdef USE_BACK_LIGHT
#ifdef USE_DISP_DRV_PROC_FOR_INTERRUPT
	DispDrvProcForInterrupt();
#endif//#ifdef USE_DISP_DRV_PROC_FOR_INTERRUPT
	KeyDrvProc();
  
}

/*
void timerDrvT0Interrupt (void) interrupt 1 using 1
{
	TH0 = TIMER0_RELOAD_VALUE_TH0;
	TL0 = TIMER0_RELOAD_VALUE_TL0;

#ifdef USE_BACK_LIGHT
	BlDrvProc();
#endif//#ifdef USE_BACK_LIGHT
#ifdef USE_DISP_DRV_PROC_FOR_INTERRUPT
	DispDrvProcForInterrupt();
#endif//#ifdef USE_DISP_DRV_PROC_FOR_INTERRUPT
	KeyDrvProc();
}
*/
/**
********************************************************************************
* @brief    Timer1 Overflosw Interrupt 함수(per TIMER1_FREQ = 10 ms)\n
* @param    none
* @return   none
* @remark   sysTimer10ms, sysTimer100ms 증가, BuzzerDrvProc 실행
********************************************************************************
*/

void TIMER1_IRQHandler(void)
{ 
  /* Clear flag for TIMER0 overflow interrupt */
  TIMER_IntClear(TIMER1, TIMER_IF_OF);
	sysTimer10ms++;

	if (!(sysTimer10ms%5)) 
	{	// 50 ms
		BuzzerDrvProc();
	}
	if (!(sysTimer10ms%10)) 
	{	// 100 ms
		sysTimer100ms++;
	}
}
/*
void timerDrvT1Interrupt (void) interrupt 3
{
	TH1 = TIMER1_RELOAD_VALUE_TH1;
	TL1 = TIMER1_RELOAD_VALUE_TL1;  // 10 ms

	sysTimer10ms++;

	if (!(sysTimer10ms%5)) 
	{	// 50 ms
		BuzzerDrvProc();
	}
	if (!(sysTimer10ms%10)) 
	{	// 100 ms
		sysTimer100ms++;
	}
}
*/
#endif//#ifdef TIMER_DRV_AT89C51ED2_STD
