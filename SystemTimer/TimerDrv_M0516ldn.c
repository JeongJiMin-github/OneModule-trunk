/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   Timer(in CPU) Driver Source File\n
           Chip : M0516LDN
* @file    TimerDrv_M0516ldn.c
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
#include "../Common/UiDefine.h"
#include "../UI/KeyApi.h"
#include "../UI/DispDrv.h"
#include "../UI/KeyDrv.h"
#include "../UI/BuzzerDrv.h"
#include "../UI/BackLightDrv.h"
#include "TimerDrv.h"
#include "../Hardware/HardwareConfig.h"
#include "../Hardware/HardwareDrv.h"

#ifdef TIMER_DRV_M0516LDN
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


/*
********************************************************************************
*                       GLOBAL(FILE SCOPE) VARIABLES & TABLES
********************************************************************************
*/
/* Insert file scope variable & tables here */
/** @brief 1 ms Timer Value*/
#ifdef USE_BACK_LIGHT
static INT16U	sysTimer1ms;
#endif
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
    /* Open Timer0 frequency to 1 Hz in periodic mode, and enable interrupt */
//    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1);
//    TIMER_EnableInt(TIMER0);

    /* Open Timer1 frequency to 10 Hz in periodic mode, and enable interrupt */
//    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 10);
//    TIMER_EnableInt(TIMER1);

    /* Open Timer2 frequency to 1000 Hz in periodic mode, and enable interrupt */
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 1000); //1 msec
    TIMER_EnableInt(TIMER2);

    /* Open Timer3 frequency to 100 Hz in periodic mode, and enable interrupt */
#ifndef USE_BACK_LIGHT
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 100); //10 msec
    TIMER_EnableInt(TIMER3);
#endif
    /* Enable Timer0 ~ Timer3 NVIC */
//    NVIC_EnableIRQ(TMR0_IRQn);
//    NVIC_EnableIRQ(TMR1_IRQn);
    NVIC_EnableIRQ(TMR2_IRQn);
#ifndef USE_BACK_LIGHT
    NVIC_EnableIRQ(TMR3_IRQn);
#endif
    /* Start Timer0 ~ Timer3 counting */
//    TIMER_Start(TIMER0);
//    TIMER_Start(TIMER1);
    TIMER_Start(TIMER2);
#ifndef USE_BACK_LIGHT
    TIMER_Start(TIMER3);
#endif
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
void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
    }
}

/**
********************************************************************************
* @brief    Timer1 overflow Interrupt 함수(per TIMER1_FREQ)\n
* @param    none
* @return   none
* @remark   sysTimer 증가, KeyProc 실행
********************************************************************************
*/
void TMR1_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);
    }
}

/**
********************************************************************************
* @brief    Timer2 overflow Interrupt 함수(per TIMER2_FREQ)\n
* @param    none
* @return   none
* @remark   sysTimer 증가, KeyProc 실행
********************************************************************************
*/
void TMR2_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER2) == 1)
    {
        /* Clear Timer2 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER2);
#ifdef USE_BACK_LIGHT
        sysTimer1ms++;

        if (!(sysTimer1ms%10))
        {	// 10 ms
            sysTimer10ms++;
        }
        if (!(sysTimer1ms%50))
        {	// 50 ms
            BuzzerDrvProc();
        }
        if (!(sysTimer1ms%100))
        {	// 100 ms
            sysTimer100ms++;
        }
        BlDrvProc();
#endif
#ifdef USE_DISP_DRV_PROC_FOR_INTERRUPT
        DispDrvProcForInterrupt();
#endif
        KeyDrvProc();
    }
}

/**
********************************************************************************
* @brief    Timer3 Overflosw Interrupt 함수(per TIMER3_FREQ = 10 ms)\n
* @param    none
* @return   none
* @remark   sysTimer10ms, sysTimer100ms 증가, BuzzerDrvProc 실행
********************************************************************************
*/
void TMR3_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER3) == 1)
    {
        /* Clear Timer3 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER3);
        sysTimer10ms++;
    }
    if (!(sysTimer10ms%5))
    {	// 50 ms
        BuzzerDrvProc();
    }
    if (!(sysTimer10ms%10))
    {	// 100 ms
        sysTimer100ms++;
    }
}
#endif//#ifdef TIMER_DRV_M0516LDN
