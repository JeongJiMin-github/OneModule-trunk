/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Timer(in CPU) Driver Source File\n
           Chip : C52
* @file    TimerDrv.c
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
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
#ifdef TIMER_DRV_AT89C51ED2_STD
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
/* Insert #define here */
//	 PRESCALE_VALUE = 12; //Standard 89C52
//   TIMER_FREQ = Osc/(PRESCALE_VALUE*(65536-TIMER_RELOAD_VALUE)
//   TIMER_RELOAD_VALUE = 65536 - Osc/(PRESCALE_VALUE*TIMER_FREQ)

/** @brief Timer 0 Frequency */
//#define TIMER0_FREQ		1250//= 0.8 ms
#if defined (USE_TIMER0_3000Hz)
#define TIMER0_FREQ		3000//= 0.5 ms
#elif defined (USE_TIMER0_2000Hz)
#define TIMER0_FREQ		2000//= 0.5 ms
#elif defined (USE_TIMER0_1500Hz)
#define TIMER0_FREQ		1500//= 0.75 ms
#else
#define TIMER0_FREQ		1000//= 1 ms
#endif

/** @brief Timer 1 Frequency */
//#define TIMER0_FREQ		200//= 5 ms
#define TIMER1_FREQ		100//= 10 ms

#ifdef X2_MODE
/** @brief PRESCALE_VALUE*/
#define PRESCALE_VALUE		6L
#else
/** @brief PRESCALE_VALUE*/
#define PRESCALE_VALUE		12L
#endif

/** @brief Timer 0 TL0 register init value */
#define TIMER0_RELOAD_VALUE_TL0	(-(OSC/(PRESCALE_VALUE*TIMER0_FREQ)) & 0xff)
/** @brief Timer 0 TH0 register init value */
#define TIMER0_RELOAD_VALUE_TH0	(-(OSC/(PRESCALE_VALUE*TIMER0_FREQ)) >> 8)
/** @brief Timer 1 TL1 register init value */
#define TIMER1_RELOAD_VALUE_TL1	(-(OSC/(PRESCALE_VALUE*TIMER1_FREQ)) & 0xff)
/** @brief Timer 1 TH1 register init value */
#define TIMER1_RELOAD_VALUE_TH1	(-(OSC/(PRESCALE_VALUE*TIMER1_FREQ)) >> 8)

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
    TH1 = TIMER1_RELOAD_VALUE_TH1;
	TL1 = TIMER1_RELOAD_VALUE_TL1;
	
	TH0 = TIMER0_RELOAD_VALUE_TH0;
	TL0 = TIMER0_RELOAD_VALUE_TL0;
	// Timer/counter Mode Control Register
	// TMOD = [GATE|C/T|M1|M0|GATE|C/T|M1|M0] 7..0
	// GATE = When GATE = 0, Timer will run only while TRx = 1(software control)
	// C/(/T) = Timer or Counter Selector
	// M1 M0 = Mode
	// 0  0  = 13-bit Timer
	// 0  1  = 16-bit Timer
	// 1  0  = 8-bit Auto-reload Timer/Counter
	// 1  1  = see Datasheet
    TMOD = 0x11;      // Timer 0, Timer 1 is 16 bit
	// Timer/counter Control Register
	// TCON = [TF1|TR1|TF0|TR0|IE1|IT1|IE0|IT0] 7..0
	// TF1 = Timer1 Overflow Flag
	// TR1 = Timer1 Run Control Flag
	// TF0 = Timer0 Overflow Flag
	// TR0 = Timer0 Run Control Flag
	// TE1 = External Interrupt 1 Edge Flag
	// IT1 = Interrupt 1 type Control Bit
	// TE0 = External Interrupt 0 Edge Flag
	// IT0 = Interrupt 0 type Control Bit
    TCON = 0x50;      // run Timer 0,1 
	// Interrupt Enable Register
	// IE = [EA|-|ET2|ES|ET1|EX1|ET0|EX0] 7..0
	// EA = Disable all Interrupt
	// ET2 = Enable Timer2 Overflow
	// ES = Enable Serial Port Interrupt
	// ET1 = Enable Timer1 Overflow
	// EX1 = Enable External Interrupt 1
	// ET0 = Enable Timer0 Overflow
	// EX0 = Enable External Interrupt 0
//	IE = 0x8a;        // Enable all, Enable T1,0 Timer overflow
	IEN0 = 0x8a;        // Enable all, Enable T1,0 Timer overflow
	// Interrupt Priority Register
	// IP = [-|-|PT2|PS|PT1|PX1|PT0|PX0] 7..0
	// PT2 = Define Timer 2 Interrupt Priority Level
	// PS = Define Serial Port Interrupt Priority Level
	// PT1 = Define Timer 1 Interrupt Priority Level
	// PX1 = Define External Interrupt 1 Priority Level
	// PT0 = Define Timer 0 Interrupt Priority Level
	// PX0 = Define External Interrupt 0 Priority Level
	//IP = 0x02;        // Set Timer 0 to priorty no. 1
//	IP = 0x12;        // Set Timer 0 to priorty no. 1//HYP 20060922
	IPL0 = 0x12;        // Set Timer 0 to priorty no. 1//HYP 20060922
	
	sysTimer10ms = 0;
	sysTimer100ms = 0;
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

/**
********************************************************************************
* @brief    Timer1 Overflosw Interrupt 함수(per TIMER1_FREQ = 10 ms)\n
* @param    none
* @return   none
* @remark   sysTimer10ms, sysTimer100ms 증가, BuzzerDrvProc 실행
********************************************************************************
*/
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
#endif//#ifdef TIMER_DRV_AT89C51ED2_STD
