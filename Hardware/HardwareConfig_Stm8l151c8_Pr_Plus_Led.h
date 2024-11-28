/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Configuraiton Header File\n
           for PR PLUS LED
* @file    HardwareConfig_Stm8l151c8_Pr_Plus_Led.h
* @version 1.0
* @date    2012/09/24
* @author  Young Hoon Lee (LYH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/24 created by LYH \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_STM8L151C8_PR_LED
#define _HARDWARE_CONFIG_STM8L151C8_PR_LED
/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Device Option : use ADS1230 AD IC for L/C */
#define AD_DRV_ADS1230
/** @brief Device Option : use STM8L151C8 AD for Battery Voltage */
#define AD_DRV_STM8L151C8
#define BATT_DRV_STM8L151C8
/** @brief Device Option : use CS5817 Display Driver IC for PR PLUS LED*/
#define DISP_DRV_CS5817_PR_PLUS_LED
/** @brief Device Option : use STM8L151C8 CPU */
#define CPU_STM8L151C8
/** @brief Device Option : use AT89C51ED2 CPU X2 Mode */
#define TIMER_DRV_STM8L151C8
/** @brief Device Option : use STM8L151C8 Internal eeprom */
#define STORAGE_DRV_STM8L151C8
/** @brief Device Option : use STM8L151C8 Serial */
#define COMM_DRV_STM8L151C8
/** @brief Device Option : use Temperature Sensor NONE*/
#define TEMP_DRV_NONE
/** @brief TIME_DRV_NONE*/
#define TIME_DRV_NONE
/** @brief Device Option : use PR PLUS LED Key Driver */
#define KEY_DRV_PR_PLUS_LED
/** @brief Device Option : Battery type */
#define BATT_TYPE_PB_4V
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Use POWER DRV for PR PLUs LED */
#define POWER_DRV_PR_PLUS_LED


/** @brief Device Option : AD Driver use non default AD Freq (use when X2 mode is off) */
#define AD_DRV_FREQ			10L         //duckspg 8L -> 10L

/** @brief Xtal frequency */
#define OSC            11059200L

/** @brief Clock Config */
//#define CLK1MHz
//#define CLK2MHz
#define CLK16MHz


#include "../STM8L151C8/stm8l15x_conf.h"
/** @brief KEY_OUT_PORT for key scan*/

/* - a/d module*/
#define ADS1230_CLK_HIGH    	{GPIOA->ODR |= GPIO_Pin_6;}
#define ADS1230_CLK_LOW     	{GPIOA->ODR &= (uint8_t)(~GPIO_Pin_6);}
#define ADS1230_SDK         	((BitStatus)(GPIOA->IDR & (uint8_t)GPIO_Pin_7))
#define ADS1230_POWERON		
#define ADS1230_POWERDOWN

/* - buzzer module*/
#define BUZZER_SET          	{GPIO_SetBits(GPIOC, GPIO_Pin_4);}
#define BUZZER_CLR          	{GPIO_ResetBits(GPIOC, GPIO_Pin_4);}

/* - key module*/
#define KEY_OUT_PORT			(GPIOB->ODR)
#define KEY_IN_PORT				(GPIO_ReadInputData(GPIOD))
#define KEY_IN_MASK				0x0f
#define SW_CAL          	    (GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_5))

/* - display module*/
#define CS5817_CLK_HIGH			{GPIO_SetBits(GPIOE, GPIO_Pin_3);}
#define CS5817_CLK_LOW      	{GPIO_ResetBits(GPIOE, GPIO_Pin_3);}
#define CS5817_SDA_HIGH			{GPIOE->ODR |= GPIO_Pin_2;}
#define CS5817_SDA_LOW      	{GPIOE->ODR &= (uint8_t)(~GPIO_Pin_2);}
#define CS5817_SDA_INPUT    	((BitStatus)(GPIOE->IDR & (uint8_t)GPIO_Pin_2))
#define CS5817_SDA_INCHANGE		{GPIO_Init(GPIOE, GPIO_Pin_2,GPIO_Mode_In_PU_No_IT);}
#define CS5817_SDA_OUTCHANGE	{GPIO_Init(GPIOE, GPIO_Pin_2,GPIO_Mode_Out_PP_High_Fast);}

/* - power module*/
#define POWER_CON_ON        	{GPIO_SetBits(GPIOD, GPIO_Pin_7);}
#define POWER_CON_OFF       	{GPIO_ResetBits(GPIOD, GPIO_Pin_7);}
#define ACCHK					(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4))
#define OnOffCHK				(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6))
#define POWER_KEY_CHK       	(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6))

#define MODEL_PORT_INIT		\
/*DISP_SDA port init*/			GPIO_Init(GPIOE, GPIO_Pin_2,GPIO_Mode_Out_PP_High_Fast);\
/*DISP_SCL port init*/			GPIO_Init(GPIOE, GPIO_Pin_3,GPIO_Mode_Out_PP_High_Fast);\
/*AD_PDWN port init*/			GPIO_Init(GPIOA, GPIO_Pin_5,GPIO_Mode_Out_PP_Low_Slow);\
/*AD_SCLK  port init*/			GPIO_Init(GPIOA, GPIO_Pin_6,GPIO_Mode_Out_PP_Low_Fast);\
/*AD_DATA port init*/			GPIO_Init(GPIOA, GPIO_Pin_7,GPIO_Mode_In_PU_No_IT);\
/*PWRCL port init*/			GPIO_Init(GPIOD, GPIO_Pin_7,GPIO_Mode_Out_PP_High_Slow);\
/*OnOffCHK port init*/ 		GPIO_Init(GPIOD, GPIO_Pin_6,GPIO_Mode_In_FL_No_IT);\
/*Buzzer port int*/			GPIO_Init(GPIOC, GPIO_Pin_4,GPIO_Mode_Out_PP_High_Slow);\
/*CAL port int*/				GPIO_Init(GPIOC, GPIO_Pin_5,GPIO_Mode_Out_PP_High_Slow);\
/*ACCHK port int*/			GPIO_Init(GPIOE, GPIO_Pin_4,GPIO_Mode_In_FL_No_IT);\

/** @brief delay1Clock*/
#define delay1Clock()	{_asm("nop\n");}
/** @brief delay2Clock*/
#define	delay2Clock()	{_asm("nop\n");_asm("nop\n");}
/** @brief delay3Clock*/
#define	delay3Clock()	{_asm("nop\n");_asm("nop\n");_asm("nop\n");}
/** @brief delay4Clock*/
#define	delay4Clock()	{_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");}
/** @brief delay5Clock*/
#define	delay5Clock()	{_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");}
/** @brief delay6Clock*/
#define	delay6Clock()	{_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");}
/** @brief delay50Clock*/
#define delay50Clock()	{_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");\
						_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");\
						_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");\
						_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");\
						_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");\
						_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");\
						_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");\
						_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");_asm("nop\n");\
						_asm("nop\n");_asm("nop\n");}
/*
*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */

#endif

