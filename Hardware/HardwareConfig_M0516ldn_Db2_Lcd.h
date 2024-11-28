/**
********************************************************************************
* Copyright (c) 2017 CAS
* @brief   Hardware Configuraiton Header File\n
           for OMV4 DBII LCD
* @file    HardwareConfig_M0516ldn_Db2_Lcd.h
* @version 1.0
* @date    2017/01/23
* @author  Jae Joon Hong (HJJ)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2017/01/23 created by HJJ \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_M0516LDN_DB2_LCD_H
#define _HARDWARE_CONFIG_M0516LDN_DB2_LCD_H

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
    #define AD_DRV_XM24L
#endif
/** @brief Device Option : use M0516LDN AD IC for Battery Voltage */
#define BATT_DRV_M0516LDN
/** @brief Device Option : use HT1621 Driver IC for M0516LDN SW LCD */
#define DISP_DRV_HT1621_M0516LDN_DB2
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
/** @brief Device Option : use Key Driver */
#define KEY_DRV_OMV4_DBII_LCD
/** @brief Device Option : use M0516LDN MCU AD (POWER CHECK, BATT CHECK, TEMP CHECK) */
#define AD_M0516LDN
/** @brief Device Option : use M0516LDN Buzzer Driver */
#define BUZZER_DRV_M0516LDN
/** @brief Device Option : Use BACK_LGIHT Driver for Std */
#define USE_BACK_LIGHT
/** @brief Device Option : Back light is Active high */
#define BACK_LIGHT_ON_ACTIVE_HIGH
/** @brief Device Option : Battery type */
#define BATT_TYPE_PB_6V
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : use Power is Active low */
#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : Use POWER DRV for Std*/
#define POWER_DRV_STD
/** @brief Device Option : AD Driver use non default AD Freq (use when X2 mode is off) */
#define AD_DRV_FREQ				10L         	//duckspg 8L -> 10L

/** OMV4 PORT */
#include "M051Series.h"
#include "gpio.h"

/* - a/d module*/
#define OB1020_CLK					P14
#define OB1020_CLK_HIGH				{OB1020_CLK = 1;}
#define OB1020_CLK_LOW				{OB1020_CLK = 0;}
//
#define OB1020_DOUT					P42
#define OB1020_DOUT_HIGH			{OB1020_DOUT = 1;}
#define OB1020_DOUT_LOW				{OB1020_DOUT = 0;}
#define OB1020_DOUT_INCHANGE		{GPIO_SetMode(P4, BIT2, GPIO_PMD_INPUT);}
#define OB1020_DOUT_OUTCHANGE		{GPIO_SetMode(P4, BIT2, GPIO_PMD_OUTPUT);}

#ifdef USE_CHECK_AD_VENDOR
/* PCB_AD_CHK */
#define PCB_AD_CHK      P27
#endif

#ifdef AD_DRV_XM24L
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
#define BUZZER						P23
#define BUZZER_SET					{BUZZER = 1;}
#define BUZZER_CLR					{BUZZER = 0;}

/* - power module*/
#define POWER_CON					P17
#define POWER_CON_ON				{POWER_CON = 1;}
#define POWER_CON_OFF				{POWER_CON = 0;}

/* - key module*/
#define SW_CAL						P10
#define POWER_KEY_CHK				P16
#define KEY_PORT_PRINT				P32
#define KEY_PORT_HOLD				P33
#define KEY_PORT_SAMPLE				P34
#define KEY_PORT_SET				P35
#define KEY_PORT_MODE				P36
#define KEY_PORT_TARE				P37
#define KEY_PORT_ZERO				P20

/** @brief BACK_LIGHT*/
#define BACK_LIGHT					P24

/* - display module*/
#define LCD_CS						P00
#define HT1621_CS_HIGH				{LCD_CS = 1;}
#define HT1621_CS_LOW				{LCD_CS = 0;}

#define LCD_WR						P02
#define HT1621_WR_HIGH				{LCD_WR = 1;}
#define HT1621_WR_LOW				{LCD_WR = 0;}

#define LCD_DATA					P01
#define HT1621_SDA_HIGH				{LCD_DATA = 1;}
#define HT1621_SDA_LOW				{LCD_DATA = 0;}
#define HT1621_SDA_INCHANGE			{GPIO_SetMode(P0, BIT1, GPIO_PMD_INPUT);}
#define HT1621_SDA_OUTCHANGE		{GPIO_SetMode(P0, BIT1, GPIO_PMD_OUTPUT);}

/* - Enable eeprom module*/
#define EEPROM_CLK					P44
#define EEPROM_CLK_HIGH				{EEPROM_CLK = 1;}
#define EEPROM_CLK_LOW				{EEPROM_CLK = 0;}

#define EEPROM_SDA_DATA				P45
#define EEPROM_SDA_HIGH				{EEPROM_SDA_DATA = 1;}
#define EEPROM_SDA_LOW				{EEPROM_SDA_DATA = 0;}
#define EEPROM_SDA_INCHANGE			{GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);}
#define EEPROM_SDA_OUTCHANGE		{GPIO_SetMode(P4, BIT5, GPIO_PMD_OUTPUT);}

/* - Enable eeprom module*/
#define RTC_CLK						P25
#define RTC_CLK_HIGH				{RTC_CLK = 1;}
#define RTC_CLK_LOW					{RTC_CLK = 0;}

#define RTC_SDA_DATA				P26
#define RTC_SDA_HIGH				{RTC_SDA_DATA = 1;}
#define RTC_SDA_LOW					{RTC_SDA_DATA = 0;}
#define RTC_SDA_INCHANGE			{GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);}
#define RTC_SDA_OUTCHANGE			{GPIO_SetMode(P4, BIT5, GPIO_PMD_OUTPUT);}


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

#endif //_HARDWARE_CONFIG_M0516LDN_DB2_LCD_H
