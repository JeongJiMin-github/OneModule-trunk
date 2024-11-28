/**
********************************************************************************
* Copyright (c) 2018 CAS
* @brief   Hardware Configuraiton Header File\n
           for OMV4 SW2W LED
* @file    HardwareConfig_M0516ldn_Sw2w_Led.h
* @version 1.0
* @date    2018/09/18
* @author  Park Young Min (FDC954)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2018/09/18 created by FDC954 \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_M0516LDN_SW2W_LED_H
#define _HARDWARE_CONFIG_M0516LDN_SW2W_LED_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Device Option : use OB1020 AD IC for L/C */
#define AD_DRV_OB1020
#ifdef USE_CHECK_AD_VENDOR
/** @brief Device Option : use XM24L AD IC for L/C */
    #define AD_DRV_XM24L_3_3V //AD_DRV_XM24L
#endif
#define BATT_DRV_M0516LDN
/** @brief Device Option : use TD62783 Display Driver IC for SW LED */
//#define DISP_DRV_XME17_M0516LDN_SWLR
#define DISP_DRV_XME17_OMV4_SW2W_LED
/** @brief Device Option : use M0516LDN CPU */
#define CPU_M0516LDN
/** @brief Device Option : use DRV_M0516LDN Timer Mode */
#define TIMER_DRV_M0516LDN
/** @brief Device Option : use AT24C64D eeprom */
#define STORAGE_DRV_AT24C64D
/** @brief Device Option : use M0516LDN Serial */
#define COMM_DRV_M0516LDN
/** @brief Device Option : use Temperature Sensor TCN75A or NONE*/
#define TEMP_DRV_NONE
/** @brief Device Option : use RX8010 RTC IC */
#define TIME_DRV_RX8010
///** @brief Device Option : use  Key Driver */
#define KEY_DRV_OMV4_SW2W_LED
/** @brief Device Option : use M0516LDN MCU AD (POWER CHECK, BATT CHECK, TEMP CHECK) */
#define AD_M0516LDN
/** @brief Device Option : use M0516LDN Buzzer Driver */
#define BUZZER_DRV_M0516LDN
/** @brief Device Option : Battery type */
#define BATT_TYPE_PB_6V
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Back Light is Active low */
#define BACK_LIGHT_ON_ACTIVE_HIGH
/** @brief Device Option : Use DispDrvProcForInterrupt() */
#define USE_DISP_DRV_PROC_FOR_INTERRUPT
/** @brief Device Option : Use counting, compare func. */
#define COUNTING_SCALE
/** @brief Device Option : Power is Active low */
#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : Use POWER DRV for Std */
#define POWER_DRV_STD
/** @brief Device Option : AD Driver use non default AD Freq (use when X2 mode is off) */
#define AD_DRV_FREQ		10L         //duckspg 8L -> 10L



/** OMV4 PORT */
#include "M051Series.h"
#include "gpio.h"

/* - a/d module*/
#define OB1020_CLK                      P14
#define OB1020_CLK_HIGH                 {OB1020_CLK = 1;}
#define OB1020_CLK_LOW                  {OB1020_CLK = 0;}

#define OB1020_DOUT                     P42
#define OB1020_DOUT_HIGH                {OB1020_DOUT = 1;}
#define OB1020_DOUT_LOW                 {OB1020_DOUT = 0;}
#define OB1020_DOUT_INCHANGE            {GPIO_SetMode(P4, BIT2, GPIO_PMD_INPUT);}
#define OB1020_DOUT_OUTCHANGE           {GPIO_SetMode(P4, BIT2, GPIO_PMD_OUTPUT);}       

#ifdef USE_CHECK_AD_VENDOR
/* PCB_AD_CHK */
#define PCB_AD_CHK       P27
#endif

#ifdef AD_DRV_XM24L_3_3V //AD_DRV_XM24L
/* - XM24L a/d module*/
#define XM24L_CLK       P14
#define XM24L_CLK_HIGH  {XM24L_CLK = 1;}
#define XM24L_CLK_LOW   {XM24L_CLK = 0;}

#define XM24L_DOUT      P42 
#define XM24L_DOUT_HIGH {XM24L_DOUT = 1;}
#define XM24L_DOUT_LOW  {XM24L_DOUT = 0;}
#define XM24L_DOUT_INCHANGE     {GPIO_SetMode(P4, BIT2, GPIO_PMD_INPUT);}
#define XM24L_DOUT_OUTCHANGE    {GPIO_SetMode(P4, BIT2, GPIO_PMD_OUTPUT);}
#endif

/* - buzzer module*/
#define BUZZER                          P24
#define BUZZER_SET                      {BUZZER = 1;}
#define BUZZER_CLR                      {BUZZER = 0;}

/* - power module*/
#define POWER_CON       P23
#define POWER_CON_ON    {POWER_CON = 1;}
#define POWER_CON_OFF   {POWER_CON = 0;}
#define POWER_KEY_CHK   P20

/* - key module*/
#define SW_CAL                          P10
#define KEY_PORT_ZERO                   P22
#define KEY_PORT_TARE                   P21
#define KEY_PORT_MODE_HOLD              P37

/* - display module*/

#define XME17_CLK       P01
#define XME17_CLK_HIGH  {XME17_CLK = 1;}
#define XME17_CLK_LOW   {XME17_CLK = 0;}

#define XME17_SDA     P02
#define XME17_SDA_HIGH	    {XME17_SDA = 1;}
#define XME17_SDA_LOW      {XME17_SDA = 0;}
#define XME17_SDA_INPUT  P02      
#define XME17_SDA_INCHANGE     {GPIO_SetMode(P0, BIT2, GPIO_PMD_INPUT);}
#define XME17_SDA_OUTCHANGE    {GPIO_SetMode(P0, BIT2, GPIO_PMD_OUTPUT);}


/* - Enable eeprom module*/
#define EEPROM_CLK                      P44
#define EEPROM_CLK_HIGH                 {EEPROM_CLK = 1;}
#define EEPROM_CLK_LOW                  {EEPROM_CLK = 0;}

#define EEPROM_SDA_DATA                 P45
#define EEPROM_SDA_HIGH                 {EEPROM_SDA_DATA = 1;}
#define EEPROM_SDA_LOW                  {EEPROM_SDA_DATA = 0;}
#define EEPROM_SDA_INCHANGE             {GPIO_SetMode(P4, BIT5, GPIO_PMD_INPUT);}
#define EEPROM_SDA_OUTCHANGE            {GPIO_SetMode(P4, BIT5, GPIO_PMD_OUTPUT);}

/* - Enable eeprom module*/
#define RTC_CLK                         P25
#define RTC_CLK_HIGH                    {RTC_CLK = 1;}
#define RTC_CLK_LOW                     {RTC_CLK = 0;}

#define RTC_SDA_DATA                    P26
#define RTC_SDA_HIGH                    {RTC_SDA_DATA = 1;}
#define RTC_SDA_LOW                     {RTC_SDA_DATA = 0;}
#define RTC_SDA_INCHANGE                {GPIO_SetMode(P2, BIT6, GPIO_PMD_INPUT);}
#define RTC_SDA_OUTCHANGE               {GPIO_SetMode(P2, BIT6, GPIO_PMD_OUTPUT);}

/** @brief delay1Clock*/
#define delay1Clock()	{asm("nop\n");}
/** @brief delay2Clock*/
#define	delay2Clock()	{asm("nop\n");asm("nop\n");}
/** @brief delay3Clock*/
#define	delay3Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay4Clock*/
#define	delay4Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay5Clock*/
#define	delay5Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay6Clock*/
#define	delay6Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay50Clock*/
#define delay15Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
			asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
                          asm("nop\n");asm("nop\n");asm("nop\n");}
/** @brief delay50Clock*/
#define delay50Clock()	{asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");asm("nop\n");\
						asm("nop\n");asm("nop\n");}


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

#endif //_HARDWARE_CONFIG_M0516LDN_SW2W_LED_H
