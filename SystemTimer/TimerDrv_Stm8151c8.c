/**
********************************************************************************
* Copyright (c) 2012 CAS
* @brief   Timer(in CPU) Driver Source File\n
           Chip : STM8L151C8
* @file    TimerDrv_Stm8l151c8.c
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
#include "../Common/UiDefine.h"
#include "../UI/KeyApi.h"
#include "../UI/DispDrv.h"
#include "../UI/KeyDrv.h"
#include "../UI/BuzzerDrv.h"
#include "../UI/BackLightDrv.h"
#include "TimerDrv.h"
#include "../Hardware/HardwareConfig.h"
#ifdef TIMER_DRV_STM8L151C8

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

/* Insert #define here */
// Timer 0         : 1ms
// 15625 Hz -> 1Mhz (clock frequency) / 64 (prescaler)
//  Setting TOP to 16 results in an overflow each 1ms 
#define TOP 16
// Timer 0      : 10ms
// 977 Hz -> 1Mhz (clock frequency) / 1024 (prescaler)
//  Setting TOP to 10 results in an overflow each 1ms 
#define TIEMR1TOP   10
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
INT16U	sysTimer10ms;
/** @brief 100 ms Timer Value*/
INT16U	sysTimer100ms;

#ifdef CLK1MHz
#define TIM4_PERIOD                   124// 1ms
#define TIM2_PERIOD                   624// 10ms
#endif
#ifdef CLK2MHz
#define TIM4_PERIOD                   124// 1ms
#define TIM2_PERIOD                   624// 10ms
#endif
#ifdef CLK16MHz
#define TIM4_PERIOD                   124// 1ms
#define TIM2_PERIOD                  1249// 10ms
#endif

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
#ifdef CLK1MHz
// CLK 1MHz
void TimerDrvInit(void)
{
	/* TIM2 configuration:
	 - TIM4CLK is set to 1 MHz, the TIM4 Prescaler is equal to 16 so the TIM1 counter
	 clock used is 1 MHz / 16 = 62 500 Hz
	- With 62 500 Hz we can generate time base:
		max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
		min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
	- In this example we need to generate a time base equal to 10 ms
	 so TIM4_PERIOD = (0.01 * 62500 - 1) = 624 */

	/* Enable TIM2 clock *///10ms
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);	
	/* TIM2 Peripheral Configuration */
	/* Time Base configuration */
	TIM2_TimeBaseInit(TIM2_Prescaler_16,TIM2_CounterMode_Up ,TIM2_PERIOD);
	/* Clear TIM4 update flag */
	TIM2_ClearFlag(TIM2_FLAG_Update);
	/* Enable update interrupt */
	TIM2_ITConfig(TIM2_IT_Update, ENABLE);

	/* Enable TIM4 clock *///1ms
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
	/* TIM4 configuration:
	 - TIM4CLK is set to 1 MHz, the TIM4 Prescaler is equal to 8 so the TIM1 counter
	 clock used is 1 MHz / 8 = 125 000 Hz
	- With 125 000 Hz we can generate time base:
		max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
		min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
	- In this example we need to generate a time base equal to 1 ms
	 so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */
	/* Time base configuration */
	TIM4_TimeBaseInit(TIM4_Prescaler_8, TIM4_PERIOD);
	/* Clear TIM4 update flag */
	TIM4_ClearFlag(TIM4_FLAG_Update);
	/* Enable update interrupt */
	TIM4_ITConfig(TIM4_IT_Update, ENABLE);	

	/* enable interrupts */
	enableInterrupts();

	/* Enable TIM2 */
	TIM2_Cmd(ENABLE);
	/* Enable TIM4 */
	TIM4_Cmd(ENABLE);
}
#endif
#ifdef CLK2MHz
// CLK 2MHz
void TimerDrvInit(void)
{
	/* TIM2 configuration:
	 - TIM4CLK is set to 2 MHz, the TIM4 Prescaler is equal to 32 so the TIM1 counter
	 clock used is 2 MHz / 32 = 62 500 Hz
	- With 62 500 Hz we can generate time base:
		max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
		min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
	- In this example we need to generate a time base equal to 10 ms
	 so TIM4_PERIOD = (0.01 * 62500 - 1) = 624 */

	/* Enable TIM2 clock *///10ms
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);	
	/* TIM2 Peripheral Configuration */
	/* Time Base configuration */
	TIM2_TimeBaseInit(TIM2_Prescaler_32,TIM2_CounterMode_Up ,TIM2_PERIOD);
	/* Clear TIM4 update flag */
	TIM2_ClearFlag(TIM2_FLAG_Update);
	/* Enable update interrupt */
	TIM2_ITConfig(TIM2_IT_Update, ENABLE);

	/* Enable TIM4 clock *///1ms
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
	/* TIM4 configuration:
	 - TIM4CLK is set to 2 MHz, the TIM4 Prescaler is equal to 16 so the TIM1 counter
	 clock used is 2 MHz / 16 = 125 000 Hz
	- With 125 000 Hz we can generate time base:
		max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
		min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
	- In this example we need to generate a time base equal to 1 ms
	 so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */
	/* Time base configuration */
	TIM4_TimeBaseInit(TIM4_Prescaler_16, TIM4_PERIOD);
	/* Clear TIM4 update flag */
	TIM4_ClearFlag(TIM4_FLAG_Update);
	/* Enable update interrupt */
	TIM4_ITConfig(TIM4_IT_Update, ENABLE);	

	/* enable interrupts */
	enableInterrupts();

	/* Enable TIM2 */
	TIM2_Cmd(ENABLE);
	/* Enable TIM4 */
	TIM4_Cmd(ENABLE);
}
#endif
#ifdef CLK16MHz
// CLK 16MHz
void TimerDrvInit(void)
{
	/* TIM2 configuration:
	 - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 32 so the TIM1 counter
	 clock used is 16 MHz /128 = 125 000 Hz
	- With 62 500 Hz we can generate time base:
		max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
		min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
	- In this example we need to generate a time base equal to 10 ms
	 so TIM4_PERIOD = (0.01 * 125000 - 1) = 1249 */

	/* Enable TIM2 clock *///10ms
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);	
	/* TIM2 Peripheral Configuration */
	/* Time Base configuration */
	TIM2_TimeBaseInit(TIM2_Prescaler_128,TIM2_CounterMode_Up ,TIM2_PERIOD);
	/* Clear TIM4 update flag */
	TIM2_ClearFlag(TIM2_FLAG_Update);
	/* Enable update interrupt */
	TIM2_ITConfig(TIM2_IT_Update, ENABLE);

	/* Enable TIM4 clock *///1ms
	CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
	/* TIM4 configuration:
	 - TIM4CLK is set to 16 MHz, the TIM4 Prescaler is equal to 16 so the TIM1 counter
	 clock used is 16 MHz / 128 = 125 000 Hz
	- With 125 000 Hz we can generate time base:
		max time base is 2.048 ms if TIM4_PERIOD = 255 --> (255 + 1) / 125000 = 2.048 ms
		min time base is 0.016 ms if TIM4_PERIOD = 1   --> (  1 + 1) / 125000 = 0.016 ms
	- In this example we need to generate a time base equal to 1 ms
	 so TIM4_PERIOD = (0.001 * 125000 - 1) = 124 */
	/* Time base configuration */
	TIM4_TimeBaseInit(TIM4_Prescaler_128, TIM4_PERIOD);
	/* Clear TIM4 update flag */
	TIM4_ClearFlag(TIM4_FLAG_Update);
	/* Enable update interrupt */
	TIM4_ITConfig(TIM4_IT_Update, ENABLE);	

	/* enable interrupts */
	enableInterrupts();

	/* Enable TIM2 */
	TIM2_Cmd(ENABLE);
	/* Enable TIM4 */
	TIM4_Cmd(ENABLE);
}
#endif

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
 
#endif //#ifdef TIMER_DRV_STM8L151C8