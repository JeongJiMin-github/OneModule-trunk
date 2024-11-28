/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Configuraiton Header File\n
           for OMV3 jig
* @file    HardwareConfig_Efm32g840f128_jig.h
* @version 1.0
* @date    2011/08/22
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2011/08/22 created by KDH \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_EFM32G840F128_JIG_H
#define _HARDWARE_CONFIG_EFM32G840F128_JIG_H

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
/** @brief Device Option : use HT1621 Display Driver IC for SW LCD */
#define DISP_DRV_NONE
/** @brief Device Option : use EFM32G840F128 CPU */
#define MCU_EFM32G840F128
/** @brief Device Option : use EFM32G840F128 Timer Mode */
#define TIMER_DRV_EFM32G840_STD
/** @brief Device Option : use M24LR64 eeprom */
#define STORAGE_DRV_M24LR64
/** @brief Device Option : use EFM32G840 Serial */
#define COMM_DRV_EFM32G840
/** @brief Device Option : use TIME_DRV_DS1337 RTC IC */
#define TIME_DRV_DS1337
/** @brief Device Option : use SW(LCD) Key Driver */
#define KEY_DRV_NONE
/** @brief Device Option : use EFM32G Buzzer Driver */
#define BUZZER_DRV_EFM32G

/** @brief Device Option : Use POWER DRV for LP2985*/
#define POWER_DRV_LP2985            // duckspg 

/** @brief Device Option : use Power is Active low */
#define POWER_ON_ACTIVE_LOW
/** @brief Device Option : Battery type */
#define BATT_TYPE_PB_9V
/** @brief Device Option : Buzzer is Active low */
#define BUZZER_ON_ACTIVE_LOW
/** @brief Device Option : Use POWER DRV for Std */
#define POWER_DRV_STD
/** @brief Device Option : AD Driver use non default AD Freq (use when X2 mode is off) */
#define AD_DRV_FREQ		10L         //duckspg 8L -> 10L
/** @brief lcd contrast controll value **/
#define CONTRAST_SET_VALUE		30         //duckspg lcd contrast 조절 값 


#define AD_BATT_CH          0x00000003UL
#define AD_TEMP_CH          0x00000008UL
#define AD_POWERCHECK_CH    0x00000006UL        // duckspg power check을 연결하는 AD채널로 변경해야함 
/** @brief Device Option : EFM32G840*/
#define TEMP_DRV_NONE           // duckspg efm32내부에 있는 온도로 교체 

/** @brief Device Option : use EFM32G840 MCU AD (POWER CHECK, BATT CHECK, TEMP CHECK) */
#define EFM32G840_AD




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
#define ADS1230_POWERDOWN   (GPIO->P[gpioPortB].DOUTSET = 1 << 13)

/* - Enable RTC module*/
#define RTC_CLK_HIGH            (GPIO->P[gpioPortC].DOUTSET = 1 << 14)
#define RTC_CLK_LOW             (GPIO->P[gpioPortC].DOUTCLR = 1 << 14)
#define RTC_SDA_HIGH            (GPIO->P[gpioPortC].DOUTSET = 1 << 13)
#define RTC_SDA_LOW             (GPIO->P[gpioPortC].DOUTCLR = 1 << 13)
#define RTC_SDA_DATA         ((GPIO->P[gpioPortC].DIN >> 13) & 0x1)
#define RTC_SDA_INCHANGE     GPIO_PinModeSet(gpioPortC, 13, gpioModeInput, 0)
#define RTC_SDA_OUTCHANGE    GPIO_PinModeSet(gpioPortC, 13, gpioModeWiredAnd, 1)


#define SW_CAL              0
#define BUZZER_SET          0       
#define BUZZER_CLR          0
#define POWER_KEY_CHK       0       // duckspg  KEY CHECK는 A/D로 할꺼기 때문에 0으로 함 
#define POWER_CON_ON        0          
#define POWER_CON_OFF       0
#define KEY_PORT_ZERO       0
#define KEY_PORT_TARE       0
#define KEY_PORT_MODE_HOLD  0


/* - Enable clock to LCD module*/
#define MODEL_PORT_INIT           CMU_ClockEnable(cmuClock_LCD, true);\
/*Divide clock to LCD module*/    CMU_ClockDivSet(cmuClock_LCDpre, cmuClkDiv_128);\
/*Enable clock for uart module*/  CMU_ClockEnable(cmuClock_USART0, true);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortB, 13, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortD, 5, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortB, 4, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortD, 4, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortD, 6, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortD, 8, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortD, 7, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortC, 6, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 4, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 6, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 5, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 7, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortF, 2, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortF, 4, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortF, 3, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortF, 5, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 8, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 12, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 9, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 13, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 14, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortE, 15, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 15, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 1, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 0, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 2, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 3, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 5, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 4, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 6, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortB, 3, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortB, 5, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortB, 4, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortB, 6, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortD, 0, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 12, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortC, 4, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortB, 7, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortD, 1, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortA, 13, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortC, 5, gpioModeInputPull, 0);\
/*Port Test init*/            GPIO_PinModeSet(gpioPortB, 8, gpioModeInputPull, 0);\



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
