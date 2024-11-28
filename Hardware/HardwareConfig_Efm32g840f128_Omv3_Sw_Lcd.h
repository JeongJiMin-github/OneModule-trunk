/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Configuraiton Header File\n
           for OMV3 SW LCD
* @file    HardwareConfig_Efm32g840f128_Omv3_Sw_Lcd.h
* @version 1.0
* @date    2011/08/22
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2011/08/22 created by KDH \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_EFM32G840F128_OMV3_SW_LCD_H
#define _HARDWARE_CONFIG_EFM32G840F128_OMV3_SW_LCD_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Device Option : use ADS1230 AD IC for L/C */
#define AD_DRV_ADS1230
/** @brief Device Option : use EFM32G AD IC for Battery Voltage */
#define BATT_DRV_EFM32G
/** @brief Device Option : use EFM32G Display Driver IC for SW LCD */
#define DISP_DRV_EFM32G840_OMV3_SW
/** @brief Device Option : use EFM32G840F128 CPU */
#define MCU_EFM32G840F128
/** @brief Device Option : use EFM32G840F128 Timer Mode */
#define TIMER_DRV_EFM32G840_STD
/** @brief Device Option : use M24LR64 eeprom */
#define STORAGE_DRV_M24LR64
/** @brief Device Option : use EFM32G840F128 Serial */
#define COMM_DRV_EFM32G840
/** @brief Device Option : use Temperature Sensor TCN75A or NONE*/
#define TEMP_DRV_NONE
/** @brief Device Option : use DS1337 RTC IC */
#define TIME_DRV_DS1337
/** @brief Device Option : use EFM32G840F128 Key Driver */
#define KEY_DRV_OMV3_SW_LCD
/** @brief Device Option : use EFM32G840 MCU AD (POWER CHECK, BATT CHECK, TEMP CHECK) */
#define EFM32G840_AD
/** @brief Device Option : use EFM32G Buzzer Driver */
#define BUZZER_DRV_EFM32G

/** @brief Device Option : Battery type */
#define BATT_TYPE_DRY_9V
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Use POWER API for Std */
#define POWER_API_STD
/** @brief Device Option : Use POWER DRV for LP2985*/
#define POWER_DRV_LP2985

/** @brief Device Option : AD Driver use non default AD Freq (use when X2 mode is off) */
#define AD_DRV_FREQ		10L         //duckspg 8L -> 10L

/** @brief lcd contrast controll value **/
#define CONTRAST_SET_VALUE		30         //duckspg lcd contrast 조절 값 


#define AD_BATT_CH          0x00000003UL
#define AD_POWERCHECK_CH    0x00000006UL        // duckspg power check을 연결하는 AD채널로 변경해야함 


/** @brief Device Option : use EFM32G840 MAIN CLK SETTING */
#ifdef USE_CLK_11MHZ
#define EFM32G840F128_MAINCLK       11       // cpu main clk 예 1Mhz설정 = 1, 7Mhz설정 = 7
#else
#define EFM32G840F128_MAINCLK       1       // cpu main clk 예 1Mhz설정 = 1, 7Mhz설정 = 7
#endif
#define EFM32G840F128_PERCLK        0       // 2^x X승의 값 divide 예 3이면 8로 나눔 예 : cpu를 7Mhz로 설정한후 3을 설정하면 Peripheral clk은 0.875Mhz로 설정됨
#define EFM32G840F128_MODIFY        0       // 초기 시작할때 클럭 보정 값 



/** @brief 설정을 위한 include 아래의 포트를 설정하기 위해 gpioPortA등등을 써야함 */
#include "efm32_gpio.h"
#include "efm32_adc.h"
#include "efm32_cmu.h"
#include "efm32_gpio.h"
#include "efm32_timer.h"
#include "efm32_letimer.h"
#include "efm32_emu.h"
#include "efm32_acmp.h"



/* - Enable eeprom module*/
#define EEPROM_CLK_HIGH         (GPIO->P[gpioPortC].DOUTSET = 1 << 12)
#define EEPROM_CLK_LOW          (GPIO->P[gpioPortC].DOUTCLR = 1 << 12)
#define EEPROM_SDA_HIGH         (GPIO->P[gpioPortC].DOUTSET = 1 << 7)
#define EEPROM_SDA_LOW          (GPIO->P[gpioPortC].DOUTCLR = 1 << 7)
#define EEPROM_SDA_DATA         ((GPIO->P[gpioPortC].DIN >> 7) & 0x1)
#define EEPROM_SDA_INCHANGE     GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 0)
#define EEPROM_SDA_OUTCHANGE    GPIO_PinModeSet(gpioPortC, 7, gpioModeWiredAnd, 1)
#define EEPROM_POWEROFF         (GPIO->P[gpioPortB].DOUTCLR = 1 << 14)
#define EEPROM_POWERON          (GPIO->P[gpioPortB].DOUTSET = 1 << 14)


/* - Enable a/d module*/
#define ADS1230_CLK_HIGH    (GPIO->P[gpioPortB].DOUTSET = 1 << 11)
#define ADS1230_CLK_LOW     (GPIO->P[gpioPortB].DOUTCLR = 1 << 11)
#define ADS1230_SDK         ((GPIO->P[gpioPortD].DIN >> 2) & 0x1)
#define ADS1230_POWERON     (GPIO->P[gpioPortB].DOUTSET = 1 << 13)
#define ADS1230_POWERDOWN   (GPIO->P[gpioPortB].DOUTCLR = 1 << 13)

/* - Enable RTC module*/
#define RTC_CLK_HIGH            (GPIO->P[gpioPortC].DOUTSET = 1 << 14)
#define RTC_CLK_LOW             (GPIO->P[gpioPortC].DOUTCLR = 1 << 14)
#define RTC_SDA_HIGH            (GPIO->P[gpioPortC].DOUTSET = 1 << 13)
#define RTC_SDA_LOW             (GPIO->P[gpioPortC].DOUTCLR = 1 << 13)
#define RTC_SDA_DATA         ((GPIO->P[gpioPortC].DIN >> 13) & 0x1)
#ifdef USE_CTS_MODE
#define RTC_SDA_INCHANGE     GPIO_PinModeSet(gpioPortC, 13, gpioModeInputPull, 1)
#else
#define RTC_SDA_INCHANGE     GPIO_PinModeSet(gpioPortC, 13, gpioModeInputPull, 0)
#endif
#define RTC_SDA_OUTCHANGE    GPIO_PinModeSet(gpioPortC, 13, gpioModeWiredAndPullUp, 1)


/* SW-1S port */

#define BUZZER_SET          (GPIO->P[gpioPortD].DOUTSET = 1 << 4)           
#define BUZZER_CLR          (GPIO->P[gpioPortD].DOUTCLR = 1 << 4)
#define POWER_CON_ON        (GPIO->P[gpioPortD].DOUTSET = 1 << 5)           
#define POWER_CON_OFF       (GPIO->P[gpioPortD].DOUTCLR = 1 << 5) 
#define KEY_PORT_ZERO       ((GPIO->P[gpioPortD].DIN >> 7) & 0x1)
#define KEY_PORT_TARE       ((GPIO->P[gpioPortD].DIN >> 8) & 0x1)
#define KEY_PORT_MODE_HOLD  ((GPIO->P[gpioPortC].DIN >> 6) & 0x1)
#define SW_CAL              ((GPIO->P[gpioPortB].DIN >> 7) & 0x1)


#define MODEL_CLK_INIT             CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);\
                                   CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);\
                                   CMU_OscillatorEnable(cmuOsc_HFRCO, true, false);\

/* - Enable clock to LCD module*/
#define MODEL_PORT_INIT           CMU_ClockEnable(cmuClock_LCD, true);\
/*Divide clock to LCD module*/    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);\
/*Divide clock to LCD module*/    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);\
/*Divide clock to LCD module*/    CMU_ClockEnable(cmuClock_CORELE, true);\
/*Divide clock to LCD module*/    CMU_ClockDivSet(cmuClock_LCDpre, cmuClkDiv_128);\
/*Enable clock for uart module*/  CMU_ClockEnable(cmuClock_USART0, true);\
/*ZERO key port init*/            GPIO_PinModeSet(gpioPortD, 7, gpioModeInputPull, 1);\
/*HOLD key port init*/            GPIO_PinModeSet(gpioPortC, 6, gpioModeInputPull, 1);\
/*TARE key port init*/            GPIO_PinModeSet(gpioPortD, 8, gpioModeInputPull, 1);\
/*CAL key port init*/             GPIO_PinModeSet(gpioPortB, 7, gpioModeInputPull, 1);\
/*BUZZER key port init*/          GPIO_PinModeSet(gpioPortD, 4, gpioModePushPull, 1);\
/*POWER ON OFF key port init*/    GPIO_PinModeSet(gpioPortD, 5, gpioModePushPull, 1);\
/*POWER CHECK key port int*/      GPIO_PinModeSet(gpioPortD, 6, gpioModeInput, 0);\
/*UART0_TX port int*/             GPIO_PinModeSet(gpioPortE, 10, gpioModePushPull, 1);\
/*UART0_RX port int*/      		  GPIO_PinModeSet(gpioPortE, 11, gpioModeInputPull, 0);\



#ifndef USE_CLK_11MHZ // 11MHz setting 

/** @brief delay1Clock*/
#define delay1Clock()	{__NOP();}
/** @brief delay2Clock*/
#define	delay2Clock()	{__NOP();__NOP();}
/** @brief delay3Clock*/
#define	delay3Clock()	{__NOP();__NOP();__NOP();}
/** @brief delay4Clock*/
#define	delay4Clock()	{__NOP();__NOP();__NOP();__NOP();}
/** @brief delay5Clock*/
#define	delay5Clock()	{__NOP();__NOP();__NOP();__NOP();__NOP();}
/** @brief delay6Clock*/
#define	delay6Clock()	{__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();}
/** @brief delay50Clock*/
#define delay50Clock()	{__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
						__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
						__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
						__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
						__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();}
#else

#define	delay10Clock()  {__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();}
/** @brief delay1Clock*/
#define delay1Clock()	{delay10Clock();}
/** @brief delay2Clock*/
#define	delay2Clock()	{delay10Clock();delay10Clock();}
/** @brief delay3Clock*/
#define	delay3Clock()	{delay10Clock();delay10Clock();delay10Clock();}
/** @brief delay4Clock*/
#define	delay4Clock()	{delay10Clock();delay10Clock();delay10Clock();delay10Clock();}
/** @brief delay5Clock*/
#define	delay5Clock()	{delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();}
/** @brief delay6Clock*/
#define	delay6Clock()	{delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();}
/** @brief delay50Clock*/
#define delay50Clock()	{delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();\
						delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();\
						delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();\
						delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();\
						delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();delay10Clock();}

#endif



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
