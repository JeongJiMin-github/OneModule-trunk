/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   Hardware Driver Source File\n
           Chip : M0516LDN
* @file    HardwareDrv_M0516ldn.c
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
#include "M051Series.h"
#include "../Common/Globals.h"
#include "../DB/StorageDrv.h"
#include "HardwareConfig.h"

#ifdef CPU_M0516LDN
#include "clk.h"
#include "gpio.h"

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief I2C Port 중복 사용을 제한하기 위한 Flag */
INT8U I2cPortUseFlag;
/** @brief Key output port 상태, Display output port 와 공용 사용시 사용 */
INT8U  KeyOutputPortStatus;

 /*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000

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
* @brief    Hardware Dirver Init 함수\n
*           CPU Init\n
*           Hardware variable Init\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void GpioInit(void)
{
	
	  #if (MODEL_NUM == MODEL_OMV4_PDN_LED)
        //ad
		GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);//AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);//AD_SDA

        //power
        GPIO_SetMode(P3, BIT4, GPIO_PMD_OUTPUT);	//POWER_CON //
        
        //key
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);		//TARE
        GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);		//MODE
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);		//ZERO
        GPIO_SetMode(P0, BIT3, GPIO_PMD_QUASI);		//SW_CAL

        //lcd
		GPIO_SetMode(P0, BIT2, GPIO_PMD_OUTPUT);	//DISP_SDA
        GPIO_SetMode(P0, BIT1, GPIO_PMD_OUTPUT);	//DISP_SCL

        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);	//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);		//EEPROM_SDA
		
		//temperature
#ifdef TEMP_DRV_NTC10K
		GPIO_SetMode(P1, BIT6, GPIO_PMD_INPUT);		//NTC Temperature sensor
#endif
        //etc
        GPIO_SetMode(P2, BIT4, GPIO_PMD_OUTPUT);	//BUZZER

		BUZZER_CLR;
        GPIO_DISABLE_DIGITAL_PATH(P0, 0x01);
    #endif

    #if (MODEL_NUM == MODEL_OMV4_SW1SCU_LCD) || (MODEL_NUM == MODEL_OMV4_SW1SU_LCD_ECR)
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);//AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);//AD_SDA
        
        //key
        GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);//POWER_KEY_CHK
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//ZERO
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//TARE
        GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);//HOLD or MODE

        //lcd
        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);//LCD_DATA

        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA

        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA

        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT4, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P2, BIT3, GPIO_PMD_QUASI);//POWER_CON
               
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);
        
    #endif
        
#if MODEL_NUM == MODEL_OMV4_SW2W_LCD
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);//AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);//AD_SDA
        
        //ad check
        GPIO_SetMode(P2, BIT7, GPIO_PMD_INPUT);//ad check
        
        //key
        GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);//POWER_KEY_CHK
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//ZERO
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//TARE
        GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);//HOLD or MODE

        //lcd
        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);//LCD_DATA
        
        GPIO_SetMode(P3, BIT5, GPIO_PMD_OUTPUT);//BACKLIGHT

        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA

        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA

        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT4, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P2, BIT3, GPIO_PMD_QUASI);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);
        
    #endif
        
    #if MODEL_NUM == MODEL_OMV4_SWLRWR_LED
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);//AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);//AD_SDA

        //key
        GPIO_SetMode(P1, BIT6, GPIO_PMD_QUASI);//POWER_KEY_CHK
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//ZERO
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//TARE
        GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);//HOLD or MODE

        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA

        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA

        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT4, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);
    #endif

    #if MODEL_NUM ==(MODEL_NUM == MODEL_OMV4_SW2W_LED)
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);//AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);//AD_SDA
        
        //ad check
        GPIO_SetMode(P2, BIT7, GPIO_PMD_INPUT);//ad check
        
        //key
        GPIO_SetMode(P1, BIT6, GPIO_PMD_QUASI);//POWER_KEY_CHK
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//ZERO
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//TARE
        GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);//HOLD or MODE
        
        //led
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);//CS5817_CLK
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);//CS5817_SDA

        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA

        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA

        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT4, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);
    #endif
        
    #if MODEL_NUM == MODEL_OMV4_DB2_LCD

        //ADC
		GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);
		GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);

		//Buzzer
		GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT);

		//Power Con
		GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);

		//Key
		GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);		//SW_CAL
		GPIO_SetMode(P1, BIT6, GPIO_PMD_QUASI);		//POWER_KEY_CHK
		GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);		//KEY_PORT_PRINT
		GPIO_SetMode(P3, BIT3, GPIO_PMD_QUASI);		//KEY_PORT_HOLD
		GPIO_SetMode(P3, BIT4, GPIO_PMD_QUASI);		//KEY_PORT_SAMPLE
		GPIO_SetMode(P3, BIT5, GPIO_PMD_QUASI);		//KEY_PORT_SET
		GPIO_SetMode(P3, BIT6, GPIO_PMD_QUASI);		//KEY_PORT_MODE
		GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);		//KEY_PORT_TARE
		GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);		//KEY_PORT_ZERO

		//LCD
		GPIO_SetMode(P0, BIT0, GPIO_PMD_OUTPUT);	//HT1621_CS
		GPIO_SetMode(P0, BIT2, GPIO_PMD_OUTPUT);	//HT1621_WR
		GPIO_SetMode(P0, BIT1, GPIO_PMD_OUTPUT);	//HT1621_DATA
		GPIO_SetMode(P2, BIT4, GPIO_PMD_OUTPUT);	//BACK_LIGHT

		//EEPROM
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);	//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);		//EEPROM_SDA

        //RTC
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);	//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);		//RTC_SDA

		//Bat check
		//GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);		//BAT_CHK
		//GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);

    #endif

    #if MODEL_NUM == MODEL_OMV4_DB2_LED_5DIGIT
        //ADC
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);
        
        //Buzzer
        GPIO_SetMode(P2, BIT4, GPIO_PMD_OUTPUT);
        
        //Power Con
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);
        
        //Key
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);		//SW_CAL
		GPIO_SetMode(P1, BIT6, GPIO_PMD_QUASI);		//POWER_KEY_CHK
		GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);		//KEY_PORT_PRINT
		GPIO_SetMode(P3, BIT3, GPIO_PMD_QUASI);		//KEY_PORT_HOLD
		GPIO_SetMode(P3, BIT4, GPIO_PMD_QUASI);		//KEY_PORT_SAMPLE
		GPIO_SetMode(P3, BIT5, GPIO_PMD_QUASI);		//KEY_PORT_SET	
		GPIO_SetMode(P3, BIT6, GPIO_PMD_QUASI);		//KEY_PORT_MODE
		GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);		//KEY_PORT_TARE
		GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);		//KEY_PORT_ZERO
        
        //EEPROM
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);	//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);		//EEPROM_SDA

        //RTC
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);	//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);		//RTC_SDA

        GPIO_SetMode(P0, BIT0, GPIO_PMD_OUTPUT);    //LED_INDICATOR
        GPIO_SetMode(P0, BIT1, GPIO_PMD_OUTPUT);    //LED_DIGIT_1
        GPIO_SetMode(P0, BIT2, GPIO_PMD_OUTPUT);    //LED_DIGIT_2
        GPIO_SetMode(P0, BIT3, GPIO_PMD_OUTPUT);    //LED_DIGIT_3
        GPIO_SetMode(P0, BIT4, GPIO_PMD_OUTPUT);    //LED_DIGIT_4
        GPIO_SetMode(P0, BIT5, GPIO_PMD_OUTPUT);    //LED_DIGIT_5
        GPIO_SetMode(P0, BIT6, GPIO_PMD_OUTPUT);    //LED_DIGIT_6
        GPIO_SetMode(P2, BIT1, GPIO_PMD_OUTPUT);    //COM_CON
        GPIO_SetMode(P2, BIT2, GPIO_PMD_OUTPUT);    //DIGIT_CON
    #endif
		
	#if MODEL_NUM == MODEL_OMV4_DB1H_VFD
        //ADC
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);
		
		//POWER CON
		GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);	//POWER_CON
		
		//KEY
		GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);		//KEY_PORT_ZERO
		GPIO_SetMode(P3, BIT3, GPIO_PMD_QUASI);		//KEY_PORT_TARE
		GPIO_SetMode(P3, BIT4, GPIO_PMD_QUASI);		//KEY_PORT_HOLD
		GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);		//SW_CAL
		GPIO_SetMode(P1, BIT6, GPIO_PMD_INPUT);		//POWER_KEY_CHK
		
		//BUZZER
		GPIO_SetMode(P3, BIT5, GPIO_PMD_OUTPUT);	//BUZZER

		//EEPROM
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);	//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);		//EEPROM_SDA
		
		//VFD
		GPIO_SetMode(P2, BIT1, GPIO_PMD_OUTPUT);	//PT6311_STROBE
        GPIO_SetMode(P2, BIT2, GPIO_PMD_OUTPUT);	//PT6311_DATA
		GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT);	//PT6311_CLK
				
		//RS232
        GPIO_SetMode(P1, BIT6, GPIO_PMD_QUASI);
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);

		//RTC
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);	//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);		//RTC_SDA
		
    #endif  
        
    #if (MODEL_NUM == MODEL_OMV4_ER_LCD) || (MODEL_NUM == MODEL_OMV4_ER_LCD_ECR) || (MODEL_NUM == MODEL_OMV4_ER_LCD_CLERK) ||  (MODEL_NUM == MODEL_OMV4_ER_LCD_SATO)
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);    //AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);     //AD_SDA

        //key
        GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);     //Key_IN_1
        GPIO_SetMode(P3, BIT3, GPIO_PMD_QUASI);     //Key_IN_2
        GPIO_SetMode(P3, BIT4, GPIO_PMD_QUASI);     //Key_IN_3
        GPIO_SetMode(P3, BIT5, GPIO_PMD_QUASI);     //Key_IN_4

        
        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);     //Key_OUT_1
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);     //Key_OUT_2
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);     //Key_OUT_3
        GPIO_SetMode(P0, BIT3, GPIO_PMD_QUASI);     //Key_OUT_4
        GPIO_SetMode(P0, BIT4, GPIO_PMD_QUASI);     //Key_OUT_5
        GPIO_SetMode(P0, BIT5, GPIO_PMD_QUASI);     //Key_OUT_6

        
        GPIO_SetMode(P1, BIT6, GPIO_PMD_INPUT);	    //POWER_KEY_CHK
        
        //Power Con
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);
        
        //lcd
        GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//LCD_DATA
        GPIO_SetMode(P2, BIT4, GPIO_PMD_QUASI);//BACK_LIGHT

        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA

        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA

        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);
    #endif
        
    #if MODEL_NUM == MODEL_OMV4_ER_LED   
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);    //AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);     //AD_SDA

        //key
        GPIO_SetMode(P3, BIT4, GPIO_PMD_QUASI);     //Key_IN_1
        GPIO_SetMode(P3, BIT5, GPIO_PMD_QUASI);     //Key_IN_2
        GPIO_SetMode(P3, BIT6, GPIO_PMD_QUASI);     //Key_IN_3
        GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);     //Key_IN_4
        
        GPIO_SetMode(P1, BIT6, GPIO_PMD_INPUT);	    //POWER_KEY_CHK
        
        //led     
        GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);    //LED_SEG_LATCH
        GPIO_SetMode(P3, BIT3, GPIO_PMD_QUASI);    //LED_SEG_OUT_ENABLE
        GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);    //LED Digit1~8 Latch
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);    //LED Digit9~16 Latch
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);    //LED Digit17~20 Latch
        GPIO_SetMode(P2, BIT3, GPIO_PMD_QUASI);    //LED Digit Output Enable
        
        //key scan
        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);
        GPIO_SetMode(P0, BIT3, GPIO_PMD_QUASI);
        GPIO_SetMode(P0, BIT4, GPIO_PMD_QUASI);
        GPIO_SetMode(P0, BIT5, GPIO_PMD_QUASI);
        GPIO_SetMode(P0, BIT6, GPIO_PMD_QUASI);
        GPIO_SetMode(P0, BIT7, GPIO_PMD_QUASI);
        
        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA

        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA

        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT4, GPIO_PMD_QUASI);//BUZZER
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);
    #endif
        
    #if (MODEL_NUM == MODEL_OMV4_ER_PRO_LCD) || (MODEL_NUM == MODEL_OMV4_ER_PRO_LCD_ECR)
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);    //AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);     //AD_SDA
        
        //key
        GPIO_SetMode(P3, BIT2, GPIO_PMD_INPUT);     //Key_IN_1
        GPIO_SetMode(P3, BIT3, GPIO_PMD_INPUT);     //Key_IN_2
        GPIO_SetMode(P3, BIT4, GPIO_PMD_INPUT);     //Key_IN_3
        GPIO_SetMode(P3, BIT5, GPIO_PMD_INPUT);     //Key_IN_4
        GPIO_SetMode(P3, BIT6, GPIO_PMD_INPUT);     //Key_IN_5
        GPIO_SetMode(P3, BIT7, GPIO_PMD_INPUT);     //Key_IN_5
        GPIO_SetMode(P2, BIT0, GPIO_PMD_INPUT);     //Key_IN_6
        GPIO_SetMode(P0, BIT7, GPIO_PMD_INPUT);     //Key_IN_7
        
//        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);     //Key_OUT_1
//        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);     //Key_OUT_2
//        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);     //Key_OUT_3
        
        GPIO_SetMode(P0, BIT0, GPIO_PMD_OUTPUT);     //Key_OUT_1
        GPIO_SetMode(P0, BIT1, GPIO_PMD_OUTPUT);     //Key_OUT_2
        GPIO_SetMode(P0, BIT2, GPIO_PMD_OUTPUT);     //Key_OUT_3
        
        
        GPIO_SetMode(P1, BIT6, GPIO_PMD_QUASI);	    //POWER_KEY_CHK
        
        //Power Con
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);
        
        //lcd
        GPIO_SetMode(P2, BIT3, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//LCD_DATA
        GPIO_SetMode(P2, BIT4, GPIO_PMD_QUASI);//BACK_LIGHT
        
        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA
        
        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA
        
        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);

    #endif

    #if MODEL_NUM == MODEL_OMV4_ER_PRO_LED
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);    //AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);     //AD_SDA
        
        //key
        GPIO_SetMode(P3, BIT2, GPIO_PMD_INPUT);     //Key_IN_1
        GPIO_SetMode(P3, BIT3, GPIO_PMD_INPUT);     //Key_IN_2
        GPIO_SetMode(P3, BIT4, GPIO_PMD_INPUT);     //Key_IN_3
        GPIO_SetMode(P3, BIT5, GPIO_PMD_INPUT);     //Key_IN_4
        GPIO_SetMode(P3, BIT6, GPIO_PMD_INPUT);     //Key_IN_5
        GPIO_SetMode(P3, BIT7, GPIO_PMD_INPUT);     //Key_IN_5
        GPIO_SetMode(P2, BIT0, GPIO_PMD_INPUT);     //Key_IN_6
        GPIO_SetMode(P0, BIT7, GPIO_PMD_INPUT);     //Key_IN_7
        
        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);     //Key_OUT_1
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);     //Key_OUT_2
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);     //Key_OUT_3

        GPIO_SetMode(P1, BIT6, GPIO_PMD_QUASI);	    //POWER_KEY_CHK
        
        //Power Con
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);
        
        //lcd
        GPIO_SetMode(P2, BIT3, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//LCD_DATA
        GPIO_SetMode(P2, BIT4, GPIO_PMD_QUASI);//BACK_LIGHT
        
        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA
        
        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA
        
        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
//        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);//POWER_CON
        
//        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);

    #endif

        
    #if MODEL_NUM == MODEL_OMV4_ED_LCD    
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);    //AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);     //AD_SDA
        
        //key
        GPIO_SetMode(P3, BIT5, GPIO_PMD_QUASI);     //KEY_PORT_POS0
        GPIO_SetMode(P3, BIT4, GPIO_PMD_QUASI);     //KEY_PORT_POS1
        GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);     //KEY_PORT_POS2
        GPIO_SetMode(P3, BIT6, GPIO_PMD_QUASI);     //KEY_PORT_POS3
        GPIO_SetMode(P3, BIT3, GPIO_PMD_QUASI);     //KEY_PORT_POS4
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);     //KEY_PORT_POS5
        GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);     //KEY_PORT_POS6
        GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);     //KEY_PORT_POS7
        
        GPIO_SetMode(P1, BIT6, GPIO_PMD_QUASI);	    //POWER_KEY_CHK
       
        //lcd
        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);//LCD_DATA
        GPIO_SetMode(P2, BIT4, GPIO_PMD_QUASI);//BACK_LIGHT
        
        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA
        
        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA
        
        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P1, BIT7, GPIO_PMD_QUASI);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);

    #endif
        
    #if (MODEL_NUM == MODEL_OMV4_ER2_LCD) || (MODEL_NUM == MODEL_OMV4_ER2_LCD_ECR) || (MODEL_NUM == MODEL_OMV4_ER2_LCD_SATO)
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);    //AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);     //AD_SDA
        
        //key
        GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);     //Key_IN_1
        GPIO_SetMode(P3, BIT3, GPIO_PMD_QUASI);     //Key_IN_2
        GPIO_SetMode(P3, BIT4, GPIO_PMD_QUASI);     //Key_IN_3
        GPIO_SetMode(P3, BIT5, GPIO_PMD_QUASI);     //Key_IN_4

        
        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);     //Key_OUT_1
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);     //Key_OUT_2
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);     //Key_OUT_3
        GPIO_SetMode(P0, BIT3, GPIO_PMD_QUASI);     //Key_OUT_4
        GPIO_SetMode(P0, BIT4, GPIO_PMD_QUASI);     //Key_OUT_5
       
        //lcd
        GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//LCD_DATA
        GPIO_SetMode(P2, BIT4, GPIO_PMD_QUASI);//BACK_LIGHT
        
        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA
        
        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA
        
        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);

    #endif
        
    #if MODEL_NUM == MODEL_OMV4_EB_LCD   
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);    //AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);     //AD_SDA

        //key
        GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);     //Key_IN_1
        GPIO_SetMode(P3, BIT3, GPIO_PMD_QUASI);     //Key_IN_2
        GPIO_SetMode(P3, BIT4, GPIO_PMD_QUASI);     //Key_IN_3
        GPIO_SetMode(P3, BIT5, GPIO_PMD_QUASI);     //Key_IN_4

        
        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);     //Key_OUT_1
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);     //Key_OUT_2
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);     //Key_OUT_3
        GPIO_SetMode(P0, BIT3, GPIO_PMD_QUASI);     //Key_OUT_4
        GPIO_SetMode(P0, BIT4, GPIO_PMD_QUASI);     //Key_OUT_5
        GPIO_SetMode(P0, BIT5, GPIO_PMD_QUASI);     //Key_OUT_6

        
        GPIO_SetMode(P1, BIT6, GPIO_PMD_INPUT);	    //POWER_KEY_CHK
        
        //Power Con
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);
        
        //lcd
        GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);//LCD_DATA
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P2, BIT4, GPIO_PMD_QUASI);//BACK_LIGHT

        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA

        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA

        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P1, BIT7, GPIO_PMD_OUTPUT);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);
    #endif
        
    #if MODEL_NUM == MODEL_OMV4_PW2_LCD
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);//AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);//AD_SDA
        
        //key
        GPIO_SetMode(P2, BIT0, GPIO_PMD_QUASI);//POWER
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//ZERO
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//TARE
        GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);//HOLD or MODE
        
        //lcd
        GPIO_SetMode(P3, BIT2, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P3, BIT3, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P3, BIT4, GPIO_PMD_QUASI);//LCD_DATA
        
        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA
        
        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA
        
        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        GPIO_SetMode(P2, BIT3, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P1, BIT1, GPIO_PMD_OUTPUT);//POWER_CON
    #endif
        
    #if  MODEL_NUM == MODEL_NUM_MODEL_OMV4_PB_LCD
        //ad
        GPIO_SetMode(P1, BIT4, GPIO_PMD_OUTPUT);//AD_SCL
        GPIO_SetMode(P4, BIT2, GPIO_PMD_QUASI);//AD_SDA

        //key
        GPIO_SetMode(P2, BIT0, GPIO_PMD_INPUT);//POWER_KEY_CHK
        GPIO_SetMode(P2, BIT2, GPIO_PMD_QUASI);//ZERO
        GPIO_SetMode(P2, BIT1, GPIO_PMD_QUASI);//TARE
        GPIO_SetMode(P3, BIT7, GPIO_PMD_QUASI);//HOLD or MODE

        //lcd
        GPIO_SetMode(P0, BIT0, GPIO_PMD_QUASI);//LCD_CS
        GPIO_SetMode(P0, BIT1, GPIO_PMD_QUASI);//LCD_WR
        GPIO_SetMode(P0, BIT2, GPIO_PMD_QUASI);//LCD_DATA
        //Backlight
        GPIO_SetMode(P3, BIT6, GPIO_PMD_QUASI);//LCD_CS
        //eeprom
        GPIO_SetMode(P4, BIT4, GPIO_PMD_OUTPUT);//EEPROM_SCL
        GPIO_SetMode(P4, BIT5, GPIO_PMD_QUASI);//EEPROM_SDA

        //rtc
        GPIO_SetMode(P2, BIT5, GPIO_PMD_OUTPUT);//RTC_SCL
        GPIO_SetMode(P2, BIT6, GPIO_PMD_QUASI);//RTC_SDA

        //etc
        GPIO_SetMode(P1, BIT0, GPIO_PMD_INPUT);//SW_CAL
        GPIO_SetMode(P1, BIT5, GPIO_PMD_INPUT);//BAT_CHK
        
        GPIO_SetMode(P1, BIT6, GPIO_PMD_INPUT);//DRY BAT_CHK
        
        GPIO_SetMode(P2, BIT4, GPIO_PMD_OUTPUT);//BUZZER
        GPIO_SetMode(P2, BIT3, GPIO_PMD_QUASI);//POWER_CON
        
        GPIO_DISABLE_DIGITAL_PATH(P1, 0x20);
        
    #endif
} 
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable IRC22M clock */
    CLK->PWRCON |= CLK_PWRCON_IRC22M_EN_Msk;

    /* Waiting for IRC22M clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_IRC22M_STB_Msk);

    /* Switch HCLK clock source to HIRC */
    CLK->CLKSEL0 = CLK_CLKSEL0_HCLK_S_HIRC;

//    /* Enable external 12 MHz XTAL, IRC10K */
//    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk | CLK_PWRCON_OSC10K_EN_Msk;

//    /* Waiting for clock ready */
//    CLK_WaitClockReady(CLK_CLKSTATUS_PLL_STB_Msk | CLK_CLKSTATUS_XTL12M_STB_Msk | CLK_CLKSTATUS_IRC10K_STB_Msk);

    /* Enable peripheral clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_UART1_EN_Msk|
                  CLK_APBCLK_ADC_EN_Msk |
                  CLK_APBCLK_TMR0_EN_Msk | CLK_APBCLK_TMR1_EN_Msk | CLK_APBCLK_TMR2_EN_Msk | CLK_APBCLK_TMR3_EN_Msk;

    /* Peripheral clock source */
    CLK->CLKSEL1 = CLK_CLKSEL1_UART_S_HIRC |
                   CLK_CLKSEL1_ADC_S_HIRC |
                   CLK_CLKSEL1_TMR0_S_HIRC | CLK_CLKSEL1_TMR1_S_HIRC | CLK_CLKSEL1_TMR2_S_HIRC | CLK_CLKSEL1_TMR3_S_HIRC;

    /* Update System Core Clock */
#ifdef USE_BACK_LIGHT
    //for debugging backlight flicker
    //22.1184MHz/4 = 5.52 MHz
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(4));
#else
    //22.1184MHz/8 = 2.76 MHz
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(8));
#endif

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set P3 multi-function pins for UART0 RXD and TXD */
    SYS->P3_MFP &= ~(SYS_MFP_P30_Msk | SYS_MFP_P31_Msk);
    SYS->P3_MFP |= (SYS_MFP_P30_RXD0 | SYS_MFP_P31_TXD0);

    /* Set P1 multi-function pins for UART1 RXD and TXD */
    SYS->P1_MFP &= ~(SYS_MFP_P12_Msk | SYS_MFP_P13_Msk);
    SYS->P1_MFP |= (SYS_MFP_P12_RXD1 | SYS_MFP_P13_TXD1);

    /* Set P1 multi-function pins for AN5 (BAT_CHK) */
    SYS->P1_MFP &= ~SYS_MFP_P15_Msk;
    SYS->P1_MFP |= SYS_MFP_P15_AIN5;
#ifdef TEMP_DRV_NTC10K	
	/* Set P1 multi-function pins for AN4 (Thermisotor) */
	SYS->P1_MFP &= ~SYS_MFP_P16_Msk;
	SYS->P1_MFP |= SYS_MFP_P16_AIN6;
#endif
}

void HwDrvInit(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    GpioInit();
    
#ifdef USE_DISABLE_LVR  //for emc debugging
    /* This macro disable Low-Voltage-Reset function. */
    SYS_DISABLE_LVR();
#endif
    /* Lock protected registers */
    SYS_LockReg();
    
    I2cPortUseFlag = OFF;
}

/**
********************************************************************************
* @brief    다음에 켤 때 Factory Boot가 무조건 실행하도록 하는 함수\n
* @param    none
* @return   none
* @remark   ENBOOT = 1, BSB = 0xFF, SBV = 0xFC 로 만든다.\n
********************************************************************************
*/
void HwDrvSetFactoryBoot(void)
{

}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

#endif //#ifdef CPU_M0516LDN
