/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Configuraiton Header File\n
           for All models
* @file    HardwareConfig.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _HARDWARE_CONFIG_H
#define _HARDWARE_CONFIG_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
#if MODEL_NUM == MODEL_JIG
#include "HardwareConfig_At89c51ed2_Jig.h"
#endif

#if MODEL_NUM == MODEL_OMV3_JIG
#include "HardwareConfig_Efm32g840f128_jig.h"
#endif

#if MODEL_NUM == MODEL_OMV4_JIG
#include "HardwareConfig_M0516ldn_Jig.h"
#endif

#if MODEL_NUM == MODEL_SW1SCU_LCD
#include "HardwareConfig_At89c51ed2_Sw_lcd.h"
#endif

#if MODEL_NUM == MODEL_SW1SU_LCD_ECR
#include "HardwareConfig_At89c51ed2_Sw_lcd.h"
#endif

#if MODEL_NUM == MODEL_EB_LCD
#include "HardwareConfig_At89c51ed2_Eb_lcd.h"
#endif

#if MODEL_NUM == MODEL_EB_LCD_7DIGIT
#include "HardwareConfig_At89c51ed2_Eb_lcd.h"
#endif

#if MODEL_NUM == MODEL_SWLRWR_LED
#include "HardwareConfig_At89c51ed2_Sw_Led.h"
#endif

#if MODEL_NUM == MODEL_SWLRWR_LED_ECR
#include "HardwareConfig_At89c51ed2_Sw_Led.h"
#endif

#if MODEL_NUM == MODEL_ER_LCD
#include "HardwareConfig_At89c51ed2_Er_Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER_LCD_ECR
#include "HardwareConfig_At89c51ed2_Er_Lcd.h"
#endif

#if MODEL_NUM == MODEL_S2000JR_LCD
#include "HardwareConfig_At89c51ed2_S2000jr_Lcd.h"
#endif

#if MODEL_NUM == MODEL_S2000JR_M_LCD
#include "HardwareConfig_At89c51ed2_S2000jr_M_Lcd.h"
#endif

#if MODEL_NUM == MODEL_S2000JR_M_6LCD
#include "HardwareConfig_At89c51ed2_S2000jr_M_6Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER_LCD_ARGOX
#include "HardwareConfig_At89c51ed2_Er_Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER_LCD_CLERK
#include "HardwareConfig_At89c51ed2_Er_Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER_LCD_BARCODE
#include "HardwareConfig_At89c51ed2_Er_Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER_PRO_LCD
#include "HardwareConfig_At89c51ed2_Er_Plus_Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER_PRO_LCD_ECR
#include "HardwareConfig_At89c51ed2_Er_Plus_Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER_PRO_6LCD
#include "HardwareConfig_At89c51ed2_Er_Plus_6Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER_PRO_6LCD_ECR
#include "HardwareConfig_At89c51ed2_Er_Plus_6Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER_LED
#include "HardwareConfig_At89c51ed2_Er_Led.h"
#endif

#if MODEL_NUM == MODEL_ER_PRO_LED
#include "HardwareConfig_At89c51ed2_Er_Plus_Led.h"
#endif

#if MODEL_NUM == MODEL_ER_VFD
#include "HardwareConfig_At89c51ed2_Er_Vfd.h"
#endif

#if MODEL_NUM == MODEL_S2000JR_VFD
#include "HardwareConfig_At89c51ed2_S2000jr_Vfd.h"
#endif

#if MODEL_NUM == MODEL_ER2_LCD
#ifdef USE_DRY_AND_PB_BATT_6V
#include "HardwareConfig_At89c51ed2_Er2_Lcd_Dry_Batt.h"
#else
#include "HardwareConfig_At89c51ed2_Er2_Lcd.h"
#endif
#endif

#if MODEL_NUM == MODEL_ER2_LCD_ECR
#include "HardwareConfig_At89c51ed2_Er2_Lcd.h"
#endif

#if MODEL_NUM == MODEL_ER2_LCD_COMPARE
#include "HardwareConfig_At89c51ed2_Er2_Lcd.h"
#endif

#if MODEL_NUM == MODEL_CASTON2_LED
#include "HardwareConfig_At89c51ed2_Caston2_Led.h"
#endif

#if MODEL_NUM == MODEL_CASTON3_LED
#include "HardwareConfig_At89c51ed2_Caston3_Led.h"
#endif

#if MODEL_NUM == MODEL_HD_LED
#include "HardwareConfig_At89c51ed2_HD_Led.h"
#endif

#if MODEL_NUM == MODEL_DB2_LCD
#include "HardwareConfig_At89c51ed2_Db2_Lcd.h"
#endif

#if MODEL_NUM == MODEL_DB2_LCD_ECR
#include "HardwareConfig_At89c51ed2_Db2_Lcd.h"
#endif

#if MODEL_NUM == MODEL_DDI_LCD
#include "HardwareConfig_At89c51ed2_DDI_Lcd.h"
#endif

#if MODEL_NUM == MODEL_DB1H_VFD
#include "HardwareConfig_At89c51ed2_Db1h_Vfd.h"
#endif

#if MODEL_NUM == MODEL_DB2_VFD
#include "HardwareConfig_At89c51ed2_Db2_Vfd.h"
#endif

#if MODEL_NUM == MODEL_EB_PT_LCD
#include "HardwareConfig_At89c51ed2_Eb_lcd.h"
#endif

#if MODEL_NUM == MODEL_PW2_LCD
#include "HardwareConfig_At89c51ed2_Sw_lcd.h"
#endif

#if MODEL_NUM == MODEL_PB_LCD
#include "HardwareConfig_At89c51ed2_Pb_lcd.h"
#endif

#if MODEL_NUM == MODEL_DP_LCD
#include "HardwareConfig_At89c51ed2_Pb_lcd.h"
#endif

#if MODEL_NUM == MODEL_PB_LCD_HEALTH
#include "HardwareConfig_At89c51ed2_Pb_lcd.h"
#endif

#if MODEL_NUM == MODEL_PB_LCD_BLUETOOTH
#include "HardwareConfig_At89c51ed2_Pb_lcd.h"
#endif

#if MODEL_NUM == MODEL_ED_LCD
#include "HardwareConfig_At89c51ed2_ED_Lcd.h"
#endif

#if MODEL_NUM == MODEL_ED_LCD_ECR
#include "HardwareConfig_At89c51ed2_ED_Lcd.h"
#endif

#if MODEL_NUM == MODEL_SW1S_LCD_LB
#include "HardwareConfig_At89c51ed2_Sw_lcd_lb.h"
#endif

#if MODEL_NUM == MODEL_PW2_LCD_LB
#include "HardwareConfig_At89c51ed2_Sw_lcd_lb.h"
#endif

#if MODEL_NUM == MODEL_ER_MLED
#include "HardwareConfig_At89c51ed2_Er_MLed.h"
#endif

#if MODEL_NUM == MODEL_AP_VFD
#include "HardwareConfig_At89c51ed2_Ap_Vfd.h"
#endif

#if MODEL_NUM == MODEL_AD_VFD
#include "HardwareConfig_At89c51ed2_Ad_Vfd.h"
#endif

#if MODEL_NUM == MODEL_IDP_LED
#include "HardwareConfig_At89c51ed2_IDP_LED.h"
#endif

#if MODEL_NUM == MODEL_DB2_LED_6DIGIT
#include "HardwareConfig_At89c51ed2_Db2_Led.h"
#endif

#if MODEL_NUM == MODEL_DB2_LED_5DIGIT
#include "HardwareConfig_At89c51ed2_Db2_Led.h"
#endif

#if MODEL_NUM == MODEL_OMV3_SW1SCU_LCD
#include "HardwareConfig_Efm32g840f128_Omv3_Sw_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV3_SWLRWR_LED
#include "HardwareConfig_Efm32g840f128_Omv3_Sw_Led.h"
#endif

#if MODEL_NUM == MODEL_PR_PLUS_LED
#include "HardwareConfig_Stm8l151c8_Pr_Plus_Led.h"
#endif

#if MODEL_NUM == MODEL_OMV3_TM_LCD
#include "HardwareConfig_Efm32g840f128_Omv3_Tm_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV3_FW500S_LCD
#include "HardwareConfig_Efm32g840f128_Omv3_Fw500s_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV3_FW500S_LED
#include "HardwareConfig_Efm32g840f128_Omv3_Fw500s_Led.h"
#endif

#if MODEL_NUM == MODEL_OMV3_FW500S_LCD_LB
#include "HardwareConfig_Efm32g840f128_Omv3_Fw500s_Lcd_Lb.h"
#endif

#if MODEL_NUM == MODEL_OMV3_FW500S_LCD_RS232
#include "HardwareConfig_Efm32g840f128_Omv3_Fw500s_Lcd_Rs232.h"
#endif

#if MODEL_NUM == MODEL_SM59R16A5_SW1SCU_LCD
#include "HardwareConfig_Sm59r16a5_Sw_Lcd.h"
#endif

#if MODEL_NUM == MODEL_SM59R16A5_SWLRWR_LED
#include "HardwareConfig_Sm59r16a5_Sw_Led.h"
#endif

#if MODEL_NUM == MODEL_OMV4_SW1SCU_LCD
#include "HardwareConfig_M0516ldn_Sw_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_PB_LCD //pys
#include "HardwareConfig_M0516ldn_Pb_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_SW1SU_LCD_ECR
#include "HardwareConfig_M0516ldn_Sw_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_DB2_LCD
#include "HardwareConfig_M0516ldn_Db2_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_DB2_LED_5DIGIT
#include "HardwareConfig_M0516ldn_Db2_Led.h"
#endif

#if MODEL_NUM == MODEL_OMV4_DB1H_VFD
#include "HardwareConfig_M0516ldn_Db1h_Vfd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ER_LCD
#include "HardwareConfig_M0516ldn_Er_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ER_LCD_ECR
#include "HardwareConfig_M0516ldn_Er_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ER_LCD_CLERK
#include "HardwareConfig_M0516ldn_Er_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ER_LCD_SATO
#include "HardwareConfig_M0516ldn_Er_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ER_LED
#include "HardwareConfig_M0516ldn_Er_Led.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ER_PRO_LCD
#include "HardwareConfig_M0516ldn_Er_Plus_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ER_PRO_LCD_ECR
#include "HardwareConfig_M0516ldn_Er_Plus_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ED_LCD
#include "HardwareConfig_M0516ldn_Ed_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ER_PRO_LED
#include "HardwareConfig_M0516ldn_Er_Plus_Led.h"
#endif

#if MODEL_NUM == MODEL_OMV4_ER2_LCD
    #ifdef USE_DRY_AND_PB_BATT_6V
    #include "HardwareConfig_M0516ldn_Er2_Lcd_Dry_Batt.h"
    #else
    #include "HardwareConfig_M0516ldn_Er2_Lcd.h"
    #endif
#endif

#if MODEL_NUM == MODEL_OMV4_ER2_LCD_ECR
	#ifdef USE_DRY_AND_PB_BATT_6V
	#include "HardwareConfig_M0516ldn_Er2_Lcd_Dry_Batt.h"
	#else
	#include "HardwareConfig_M0516ldn_Er2_Lcd.h"
	#endif
#endif

#if MODEL_NUM == MODEL_OMV4_ER2_LCD_SATO
	#ifdef USE_DRY_AND_PB_BATT_6V
	#include "HardwareConfig_M0516ldn_Er2_Lcd_Dry_Batt.h"
	#else
	#include "HardwareConfig_M0516ldn_Er2_Lcd.h"
	#endif
#endif


#if MODEL_NUM == MODEL_OMV4_SWLRWR_LED
#include "HardwareConfig_M0516ldn_Sw_Led.h"
#endif

#if MODEL_NUM == MODEL_OMV4_EB_LCD
#include "HardwareConfig_M0516ldn_Eb_lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_PW2_LCD
#include "HardwareConfig_M0516ldn_Sw_Lcd.h"
#endif

#if MODEL_NUM == MODEL_OMV4_SW2W_LCD
	#ifdef USE_PB_BATT_6V
	#include "HardwareConfig_M0516ldn_Sw2w_Lcd_Pb_Batt.h"
	#else
	#include "HardwareConfig_M0516ldn_Sw2w_Lcd.h"
	#endif
#endif

#if MODEL_NUM == MODEL_OMV4_SW2W_LED
#include "HardwareConfig_M0516ldn_Sw2w_Led.h"
#endif

#if MODEL_NUM == MODEL_OMV4_PDN_LED
#include "HardwareConfig_M0516ldn_Pdn_Led.h"
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

#endif /* _HARDWARE_CONFIG_H */
