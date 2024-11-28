/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Global Define Header File\n
           for One Module
* @file    Globals.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _GLOBALS_H
#define _GLOBALS_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief use Debug API */
//#define USE_DEBUG_API
/** @brief Debug temperature sensor*/
//#define DEBUG_TEMP
/** @brief Debug AD Converter*/
//#define DEBUG_AD_API
/** @brief Debug AD test by rsForOne*/
//#define DEBUG_AD_TEST

/** @brief Compiler option : use Keil */
#define KEIL

/** @brief model type number(JIG)*/
#define MODEL_JIG		 			0x0000
/** @brief model type number(V3 JIG)*/
#define MODEL_OMV3_JIG		 			0x0010
/** @brief model type number(V4 JIG)*/
#define MODEL_OMV4_JIG		 		0x0020
/** @brief model type number(EB)*/
#define MODEL_EB_LCD	 			0x1000
/** @brief Model type number(EB_Percent tare ver.)*/ 
#define MODEL_EB_PT_LCD			0x1001
/** @brief Model type number(EB Total price 7 digit ver.)*/ 
#define MODEL_EB_LCD_7DIGIT		0x1002
/** @brief model type number(SW1S)*/
#define MODEL_SW1SCU_LCD			0x1010
/** @brief model type number(SW1S Unit change ver.)*/
//#define MODEL_SW1S_UC_LCD	 	0x1011
/** @brief model type number(SW1SU ECR ver.)*/
#define MODEL_SW1SU_LCD_ECR			0x1012
/** @brief model type number(SW-LR)*/
#define MODEL_SWLRWR_LED	 		0x1020
/** @brief model type number(SW-LR ECR ver.)*/
#define MODEL_SWLRWR_LED_ECR	 	0x1021
/** @brief model type number(ER-PLUS LCD )*/
#define MODEL_ER_LCD	 			0x1030
/** @brief model type number(ER-PLUS LCD ECR version)*/
#define MODEL_ER_LCD_ECR 			0x1031
/** @brief model type number(ER-PLUS-M LCD)*/
#define MODEL_ER_PRO_LCD			0x1032
/** @brief model type number(ER-PLUS LCD Clerk send version)*/
#define MODEL_ER_LCD_CLERK			0x1033
/** @brief model type number(ER-PLUS LCD Barcode print version)*/
#define MODEL_ER_LCD_BARCODE		0x1034
/** @brief model type number(S2000JR)*/
#define MODEL_S2000JR_LCD		 	0x1035
/** @brief model type number(S2000JR-M)*/
#define MODEL_S2000JR_M_LCD		 	0x1036
/** @brief model type number(ER_LCD_ARGOX)*/
#define MODEL_ER_LCD_ARGOX		 	0x1037
/** @brief model type number(ER-PLUS-M LCD)*/
#define MODEL_ER_PRO_LCD_ECR		0x1038
/** @brief model type number(ER-PLUS-M LCD 6digit)*/
#define MODEL_ER_PRO_6LCD			0x1039
/** @brief model type number(S2000JR-M 6digit)*/
#define MODEL_S2000JR_M_6LCD	 	0x103A
/** @brief model type number(ER-PLUS-M LCD)*/
#define MODEL_ER_PRO_6LCD_ECR		0x103B
/** @brief model type number(ER-PLUS LED)*/
#define MODEL_ER_LED	 			0x1040
/** @brief model type number(ER-PLUS-M LED)*/
#define MODEL_ER_PRO_LED	 		0x1041
/** @brief model type number(ER-PLUS VFD)*/
#define MODEL_ER_VFD	 			0x1050
/** @brief model type number(ER-PLUS VFD)*/
#define MODEL_S2000JR_VFD	 		0x1051
/** @brief model type number(ER2-LCD)*/
#define MODEL_ER2_LCD				0x1060
/** @brief model type number(ER2-LCD ECR version)*/
#define MODEL_ER2_LCD_ECR			0x1061
/** @brief model type number(ER2-LCD Compare version)*/
#define MODEL_ER2_LCD_COMPARE		0x1062
/** @brief model type number(CASTON3)*/
#define MODEL_CASTON3_LED			0x1070
/** @brief model type number(DBII LCD ECR Version)*/
#define MODEL_DB2_LCD	 			0x1080
/** @brief model type number(DBII)*/
#define MODEL_DB2_LCD_ECR			0x1081
/** @brief model type number(DBII)*/
#define MODEL_DB1H_VFD	 			0x1090
/** @brief model type number(DBII)*/
#define MODEL_DB2_VFD	 			0x10A0
/** @brief model type number(DDI)*/
#define MODEL_DDI_LCD	 			0x10B0
/** @brief model type number(SW1C)*/
//#define MODEL_SW1C_LCD	 	0x10B0
/** @brief model type number(SW-WR)*/
//#define MODEL_SWWR_LED 		0x10C0
/** @brief model type number(SW-WR)*/
//#define MODEL_SWWR_UC_LED 	0x10D0
/** @brief Model type number(PW-II)*/ 
#define MODEL_PW2_LCD				0x10E0
/** @brief Model type number(ED)*/ 
#define MODEL_ED_LCD				0x10F0
/** @brief Model type number(ED ECR Version)*/ 
#define MODEL_ED_LCD_ECR			0x10F1
/** @brief model type number(CASTON2)*/
#define MODEL_CASTON2_LED			0x1100
/** @brief model type number(AP)*/
#define MODEL_AP_VFD				0x1110
/** @brief model type number(SW-1S(US ver))*/
#define MODEL_SW1S_LCD_LB 			0x1120
/** @brief model type number(PW-2(US ver))*/
#define MODEL_PW2_LCD_LB 			0x1130
/** @brief model type number(AD)*/
#define MODEL_AD_VFD				0x1140
/** @brief model type number(DBII)*/
#define MODEL_DB2_LED_6DIGIT		0x1150
/** @brief model type number(DBII)*/
#define MODEL_DB2_LED_5DIGIT	 	0x1151
/** @brief model type number(PB)*/
#define MODEL_PB_LCD				0x1160
/** @brief model type number(DP)*/
#define MODEL_DP_LCD				0x1161
/** @brief model type number(PB HEALTH)*/
#define MODEL_PB_LCD_HEALTH			0x1162
/** @brief model type number(PB BLUETOOTH)*/
#define MODEL_PB_LCD_BLUETOOTH		0x1163
/** @brief model type number(HD)*/
#define MODEL_HD_LED				0x1170

/** @brief model type number(AP-LED)*/
#define MODEL_AP_MLED		0x2000
/** @brief model type number(ER-M)*/
#define MODEL_ER_MLED		0x2010
/** @brief model type number(IDP, IDU)*/
#define MODEL_IDP_LED		0x2020



/** @brief model type number(SW) OnemoduleV3*/
#define MODEL_OMV3_SW1SCU_LCD			0x3000
/** @brief model type number(SW) OnemoduleV3*/
#define MODEL_OMV3_SWLRWR_LED			0x3010

/** @brief model type number(FW) OnemoduleV3*/

#define MODEL_OMV3_FW500S_LCD			0x3030
/** @brief model type number(FW) OnemoduleV3*/
#define MODEL_OMV3_FW500S_LED			0x3040
/** @brief model type number(FW LCD US) OnemoduleV3*/
#define MODEL_OMV3_FW500S_LCD_LB    	0x3050
/** @brief model type number(FW LCD_RS232 Version) OnemoduleV3*/
#define MODEL_OMV3_FW500S_LCD_RS232		0x3060



/** @brief model type number(TLF) OnemoduleV3*/
#define MODEL_OMV3_TM_LCD			0x3700		// 0x3700 ~ 0x4000  Industrial model Number
/** @brief model type number(PR) PR PLUS LED*/
#define MODEL_PR_PLUS_LED			    0x4000

/** @brief model type number(SW) SM59R16A5*/
#define MODEL_SM59R16A5_SW1SCU_LCD		0x5000

/** @brief model type number(SWLR) SM59R16A5*/
#define MODEL_SM59R16A5_SWLRWR_LED		0x5001

/** @brief model type number(EB)*/
#define MODEL_OMV4_EB_LCD           0x6000

/** @brief model type number(SW LCD)*/
#define MODEL_OMV4_SW1SCU_LCD    	0x6010

/** @brief model type number(SW LCD_ECR)*/
#define MODEL_OMV4_SW1SU_LCD_ECR	0x6012

/** @brief model type number(SW-LR)*/
#define MODEL_OMV4_SWLRWR_LED       0x6020

/** @brief model type number(ER LCD)*/
#define MODEL_OMV4_ER_LCD			0x6030

/** @brief model type number(ER LCD_ECR)*/
#define MODEL_OMV4_ER_LCD_ECR		0x6031

/** @brief model type number(ER-PLUS-M LCD)*/
#define MODEL_OMV4_ER_PRO_LCD		0x6032

/** @brief model type number(ER-PLUS LCD Clerk send version)*/
#define MODEL_OMV4_ER_LCD_CLERK		0x6033

/** @brief model type number(ER LCD_SATO)*/
#define MODEL_OMV4_ER_LCD_SATO		0x6034

/** @brief model type number(ER-PLUS-M LCD ECR)*/
#define MODEL_OMV4_ER_PRO_LCD_ECR	0x6038

/** @brief model type number(ER-PLUS LED)*/
#define MODEL_OMV4_ER_LED	 		0x6040

/** @brief model type number(ER-PLUS-M LED)*/
#define MODEL_OMV4_ER_PRO_LED       0x6041

/** @brief model type number(ER JR_LCD)*/
#define MODEL_OMV4_ER2_LCD          0x6060

/** @brief model type number(ER JR-LCD ECR version)*/
#define MODEL_OMV4_ER2_LCD_ECR		0x6061

/** @brief model type number(ER JR-LCD SATO)*/
#define MODEL_OMV4_ER2_LCD_SATO		0x6062

/** @brief model type number(DB2 LCD)*/
#define MODEL_OMV4_DB2_LCD			0x6080

/** @brief model type number(DB1H VFD)*/
#define MODEL_OMV4_DB1H_VFD			0x6090

/** @brief model type number(PW-II)*/
#define MODEL_OMV4_PW2_LCD			0x60E0

/** @brief Model type number(ED)*/
#define MODEL_OMV4_ED_LCD			0x60F0

/** @brief model type number(DB2 LED)*/
#define MODEL_OMV4_DB2_LED_5DIGIT   0x6151

/** @brief model type number(PB LCD)*/
#define MODEL_OMV4_PB_LCD            0x6200

/** @brief model type number(SW LCD)*/
#define MODEL_OMV4_SW2W_LCD    	0x6210

/** @brief model type number(SW LCD)*/
#define MODEL_OMV4_SW2W_LED    	0x6220

/** @brief model type number(PDN LED)*/
#define MODEL_OMV4_PDN_LED    	0x6230

/** @brief Model Number(JIG)*/ 
//#define MODEL_NUM MODEL_JIG

/** @brief Model Number(JIG)*/ 
//#define MODEL_NUM MODEL_OMV3_JIG

/** @brief Model Number(OMV4_JIG)*/ 
//#define MODEL_NUM MODEL_OMV4_JIG

/** @brief Model Number(EB) data=133.0/256, xdata=1783/1792, code=58931/63488, 2011.04.11, Portugal: data=133.1/256, xdata=1782/1792, code=59059/63488, 2013.01.17*/
/*Austrailia: data=133.1/256, xdata=1785/1792, code=59374/63488, 2013.01.17*/
//#define MODEL_NUM MODEL_EB_LCD

/** @brief Model Number(EB_Percent tare ver.) data=129.6/256, xdata=1760/1792, code=45325/63488, 2013.01.15*/ //45498
//#define MODEL_NUM MODEL_EB_PT_LCD

/** @brief Model Number(EB_total price 7digit ver.) data=xxx.x/256, xdata=1779/1792, code=58970/63488, 2010.09.07*/ //59345
//#define MODEL_NUM MODEL_EB_LCD_7DIGIT

/** @brief Model Number(SW1S, SW1C, SW1U) data= 95.3/256, xdata= 1690/2048, code= 46736/65023, 2014.02.18*/ 
//#define MODEL_NUM MODEL_SM59R16A5_SW1SCU_LCD

/** @brief Model Number(SW1S, SW1C, SW1U) data=101.2/256, xdata=1693/1792, code=46554/63488, 2014.05.13*/
//#define MODEL_NUM MODEL_SW1SCU_LCD

/** @brief Model Number(SW1S, SW1U ECR) data=101.4/256, xdata=1697/1792, code=50711/63488, 2016.06.16*/
//#define MODEL_NUM MODEL_SW1SU_LCD_ECR

/** @brief Model Number(SWLR) data= , xdata= , code= , 2014.02.18*/ 
//#define MODEL_NUM MODEL_SM59R16A5_SWLRWR_LED

/** @brief Model Number(SWLR) data=xxx.x/256, xdata=1661/1792, code=45717/63488, 2010.09.07*/
//#define MODEL_NUM	MODEL_SWLRWR_LED

/** @brief model type number(SW-LR ECR ver.) xdata=xxx.x/256, xdata=1661/1792, code=45717/63488, 2010.09.07*/
//#define MODEL_NUM	MODEL_SWLRWR_LED_ECR
 
/** @brief Model Number(SW-1S(US ver)) data=xxx.x/256, xdata=1689/1792, code=45878/63488, 2010.09.07*/ 
//#define MODEL_NUM MODEL_SW1S_LCD_LB

/** @brief Model Number(MODEL_PB_LCD)) data=121.5/256, xdata=1691/1792, code=47863/63488, 2013.12.17*/ 
//#define MODEL_NUM MODEL_PB_LCD

/** @brief Model Number(MODEL_PB_LCD_HEALTH)) data=121.3/256, xdata=1762/1792, code=46556/63488, 2011.07.07*/ 
//#define MODEL_NUM MODEL_PB_LCD_HEALTH

/** @brief Model Number(MODEL_PB_LCD_BLUETOOTH)) data=121.6/256, xdata=1714/1792, code=48566/63488, 2016.01.29*/ 
//#define MODEL_NUM MODEL_PB_LCD_BLUETOOTH

/** @brief Model Number(MODEL_DP_LCD)) data=121.2/256, xdata=1671/1792, code=46684/63488, 2011.04.08*/ 
//#define MODEL_NUM MODEL_DP_LCD

/** @brief Model Number(ER_PLUS_LCD) data=170.4/256, xdata=1753/1792, code=63300/63488, 2013.05.07*/ // decimal point No USE
/*Australia: data=164.4/256, xdata=1743/1792, code=63752/63488, 2013.01.15*/  
/*Portugal: data=170.4/256, xdata=1752/1792, code=63503/63488, 2013.01.15*/  
/*Turkey: data=170.4/256, xdata=1752/1792, code=63492/63488, 2013.01.15*/
/*Newzealand: data=170.6/256, xdata=1755/1792, code=63876/63488, 2013.01.15*/
/*Peru: data=170.4/256, xdata=1752/1792, code=63729/63488, 2013.01.15*/
/*Russia: data=170.4/256, xdata=1752/1792, code=63501/63488, 2013.01.15*/
//#define MODEL_NUM	MODEL_ER_LCD


/** @brief Model Number(ER_PLUS_LCD_ECR) data=170.4/256, xdata=1787/1792, code=63364/63488, 2015.09.10*/
//#define MODEL_NUM	MODEL_ER_LCD_ECR

/** @brief Model Number(ER_PLUS_M_LCD) data=167.3/256, xdata=1792/1792, code=63215/63488, 2016.03.08*/ // decimal point No USE
/*Germany: data=167.3/256, xdata=1792/1792, code=63215/63488, 2016.03.08*/	// decimal point No USE
/*Turkey: data=164.4/256, xdata=1780/1792, code=63290/63488, 2016.03.10*/	// decimal point No USE
//#define MODEL_NUM	MODEL_ER_PRO_LCD

/** @brief Model Number(ER_PLUS_M_LCD) data=167.4/256, xdata=1790/1792, code=63218/63488, 2013.02.28*/ // Quick store function, decimal point  No USE
//#define MODEL_NUM	MODEL_ER_PRO_LCD_ECR

/** @brief model type number(ER-PLUS LCD Clerk send version) data=170.2/256, xdata=1752/1792, code=63393/63488, 2012.08.24*/ // decimal point No USE
//#define MODEL_NUM	MODEL_ER_LCD_CLERK

/** @brief model type number(ER-PLUS LCD Barcode print version) data=170.2/256, xdata=1751/1792, code=63451/63488, 2012.08.24*/// decimal point No USE
//#define MODEL_NUM	MODEL_ER_LCD_BARCODE

/** @brief Model Number(S2000JR) data=170.2/256, xdata=1668/1792, code=62692/63488, 2021.05.24*/ // decimal point No USE
//#define MODEL_NUM MODEL_S2000JR_LCD

/** @brief model type number(S2000JR-M) data=167.2/256, xdata=1706/1792, code=62818/63488, 2013.01.18*/ //decimal point No USE
//#define MODEL_NUM MODEL_S2000JR_M_LCD		 	

/** @brief Model Number(MODEL_ER_LCD_ARGOX) data=170.2/256, xdata=1751/1792, code=63359/63488, 2012.08.24*/ //decimal point No USE
//#define MODEL_NUM MODEL_ER_LCD_ARGOX

/** @brief Model Number(ER_PLUS_M_6LCD) data=179.4/256, xdata=1783/1792, code=63425/63488, 2012.08.24*/ //no test 
//#define MODEL_NUM	MODEL_ER_PRO_6LCD

/** @brief model type number(S2000JR-M 6DIGIT) data=170.2/256, xdata=1714/1792, code=62646/63488, 2020.03.31*/  //no test 
//#define MODEL_NUM MODEL_S2000JR_M_6LCD		 	

/** @brief Model Number(ER_PLUS_M_LCD 6DIGIT) data=179.6/256, xdata=1784/1792, code=62981/63488, 2012.08.24*/  //no test
//#define MODEL_NUM	MODEL_ER_PRO_6LCD_ECR

/** @brief Model Number(ER_PLUS_LED) data=152.0/256, xdata=1758/1792, code=62162/63488, 2012.08.24*/
//#define MODEL_NUM	MODEL_ER_LED

/** @brief Model Number(ER_PLUS_M_LED) data=147.0/256, xdata=1792/1792, code=62191/63488, 2015.03.09*/
/*Turkey: data=147.0/256, xdata=1780/1792, code=62266/63488, 2016.03.11*/	// decimal point No USE
//#define MODEL_NUM MODEL_ER_PRO_LED

/** @brief Model Number(ER_PLUS_VFD) data=159.0/256, xdata=1783/1792, code=62866/63488, 2012.08.24*/
//#define MODEL_NUM	MODEL_ER_VFD

/** @brief Model Number(ER_S2000JR_VFD) data=158.6/256, xdata=1783/1792, code=62567/63488, 2013.01.17*/
//#define MODEL_NUM MODEL_S2000JR_VFD

/** @brief Model Number(ER2_LCD) data=148.3/256, xdata=1755/1792, code=62425/63488, 2013.08.23, Russia: data=148.5/256, xdata=1755/1792, code=60743/63488, 2013.01.15*/
//#define MODEL_NUM	MODEL_ER2_LCD

/** @brief Model Number(ER2_LCD_ECR) data=148.3/256, xdata=1719/1792, code=60655/63488, 2013.02.26*/
//#define MODEL_NUM	MODEL_ER2_LCD_ECR

/** @brief Model Number(ER2_LCD_ECR) data=148./256,xdata=1776/1792, code=58717/63488, 2012.08.24*/ // decimal point No USE
//#define MODEL_NUM	MODEL_ER2_LCD_COMPARE

/** @brief Model Number(ER MLED) data=131.7/256, xdata=1775/1792, code=60039/63488, 2010.09.07*/  //no test
//#define MODEL_NUM MODEL_ER_MLED			   



/** @brief Model Number(CASTON2_LED) data=xxx.x/256, xdata=1721/1792, code=47414/63488, 2010.09.07*/
//#define MODEL_NUM	MODEL_CASTON2_LED

/** @brief Model Number(CASTON3_LED) data=xxx.x/256, xdata=1721/1792, code=47407/63488, 2010.09.07*/
//#define MODEL_NUM	MODEL_CASTON3_LED

/** @brief Model Number(HD_LED) data=xxx.x/256, xdata=1721/1792, code=47414/63488, 2010.09.07*/
//#define MODEL_NUM MODEL_HD_LED			
/** @brief Model Number(AP) data=190.0/256, xdata=1772/1792, code=62379/63488, 2013.01.17*/ 
//#define MODEL_NUM MODEL_AP_VFD

/** @brief Model Number(AD) data=xxx.x/256, xdata=1732/1792, code=48660/63488, 2010.09.07*/ 
//#define MODEL_NUM MODEL_AD_VFD	  

/** @brief Model Number(DB2_LCD) data=119.7/256, xdata=1720/1792, code=52854/63488, 2011.04.29*/
//#define MODEL_NUM MODEL_DB2_LCD

/** @brief Model Number(DB2_LCD) data=120.2/256, xdata=1708/1792, code=55001/63488, 2020.03.24*/
//#define MODEL_NUM MODEL_DB2_LCD_ECR

/** @brief Model Number(DDI_LCD) data=xxx.x/256, xdata=1720/1792, code=52784/63488, 2010.09.07*/
//#define MODEL_NUM MODEL_DDI_LCD

/** @brief Model Number(DB1_1H_VFD) data=88.5/256, xdata=1771/1792, code=49181/63488, 2014.12.26*/
//#define MODEL_NUM MODEL_DB1H_VFD

/** @brief Model Number(DB2_VFD) data=84.4/256, xdata=1718/1792, code=52498/63488, 2011.04.29*/
//#define MODEL_NUM	MODEL_DB2_VFD

/** @brief Model Number(DB2_LED_6DIGIT)_NOT FOR SALE, data=xxx.x/256, xdata=0/1792, code=0/63488, Compile Error 2010.09.07*/ 
//#define MODEL_NUM MODEL_DB2_LED_6DIGIT

/** @brief Model Number(DB2_LED_5DIGIT) data=69.6/256, xdata=1767/1792, code=50713/63488, 2016.03.23*/
/*Netherland: data=69.6/256, xdata=1777/1792, code=51263/63488, 2014.02.14*/ 
//#define MODEL_NUM MODEL_DB2_LED_5DIGIT


							  							 
/** @brief Model Number(PW-II) data=101.1/256, xdata=1685/1792, code=45392/63488, 2013.01.17*/ 
//#define MODEL_NUM MODEL_PW2_LCD

/** @brief Model Number(PW-2(US ver)) data=113.1/256, xdata=1710/1792, code=46278/63488, 2013.01.17*/ 
//#define MODEL_NUM MODEL_PW2_LCD_LB



/** @brief Model Number(ED) data=120.0/256, xdata=1744/1792, code=52953/63488, 2020.03.24, USA: data=120.0/256, xdata=1719/1792, code=53152/63488, 2013.01.17*/
//#define MODEL_NUM MODEL_ED_LCD

/** @brief Model Number(ED) data=120.2/256, xdata=1708/1792, code=54993/63488 2020.03.24*/
//#define MODEL_NUM MODEL_ED_LCD_ECR

/** @brief Model Number(IDP_LED) data=79.7/256, xdata=1685/1792, code=46923/63488, 2013.01.17*/ 
//#define MODEL_NUM MODEL_IDP_LED
  

/** @brief Model Number(SW) data=    , xdata=         , code=           , 2011.08.22*/ 
//#define MODEL_NUM MODEL_OMV3_SW1SCU_LCD

/** @brief Model Number(SW) data=    , xdata=         , code=           , 2012.01.03*/ 
//#define MODEL_NUM MODEL_PR_PLUS_LED

/** @brief Model Number(SW) data=    , xdata=         , code=           , 2011.08.22*/ 
//#define MODEL_NUM MODEL_OMV3_TM_LCD


/** @brief Model Number(SW500) data=    , xdata=         , code=           , 2016.08.26*/ 
//#define MODEL_NUM MODEL_OMV3_FW500S_LCD

//#define MODEL_NUM MODEL_OMV3_FW500S_LCD_LB


/** @brief Model Number(FW500) data=    , xdata=         , code=           , 2016.08.26*/ 
//#define MODEL_NUM MODEL_OMV3_FW500S_LED

/** @brief Model Number(FW500_RS232 Version) data=    , xdata=         , code=           , 2015.12.14*/ 
//#define MODEL_NUM MODEL_OMV3_FW500S_LCD_RS232

//[start] OMV4 Model List
/** @brief Model Number(SW LCD) data=    , xdata=         , code=           , 2011.08.22*/
//#define MODEL_NUM MODEL_OMV4_SW1SCU_LCD

/** @brief Model Number(SW LCD_ECR) data=    , xdata=         , code=           , 2017.07.13*/
//#define MODEL_NUM MODEL_OMV4_SW1SU_LCD_ECR

/** @brief Model Number(SW LED) data=xxx.x/256, xdata=1661/1792, code=45717/63488, 2010.09.07*/
//#define MODEL_NUM MODEL_OMV4_SWLRWR_LED

/** @brief Model Number(DB2 LED) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_DB2_LED_5DIGIT

/** @brief Model Number(DB2 LCD) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_DB2_LCD

/** @brief Model Number(DB1H VFD) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_DB1H_VFD

/** @brief Model Number(ER_PLUS LCD) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_ER_LCD

/** @brief Model Number(ER_PLUS LCD_ECR) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_ER_LCD_ECR

/** @brief Model Number(ER_PLUS LCD_ECR) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_ER_LCD_SATO

/** @brief Model Number(ER_PLUS_LED) data=152.0/256, xdata=1758/1792, code=62162/63488, 2012.08.24*/
//#define MODEL_NUM MODEL_OMV4_ER_LED

/** @brief Model Number(ER_PLUS_M LCD) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_ER_PRO_LCD

/** @brief Model Number(ER_PLUS_M LCD_ECR) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_ER_PRO_LCD_ECR

/** @brief Model Number(ER_PLUS_M_LED) data=147.0/256, xdata=1792/1792, code=62191/63488, 2015.03.09*/
/*Turkey: data=147.0/256, xdata=1780/1792, code=62266/63488, 2016.03.11*/	// decimal point No USE
//#define MODEL_NUM MODEL_OMV4_ER_PRO_LED

/** @brief Model Number(ED) data=xxx.x/256, xdata=1716/1792, code=52724/63488, 2010.09.03, USA: data=120.0/256, xdata=1719/1792, code=53152/63488, 2013.01.17*/
//#define MODEL_NUM MODEL_OMV4_ED_LCD

/** @brief Model Number(ER JR_LCD) data=148.3/256, xdata=1755/1792, code=62425/63488, 2013.08.23, Russia: data=148.5/256, xdata=1755/1792, code=60743/63488, 2013.01.15*/
//#define MODEL_NUM MODEL_OMV4_ER2_LCD

/** @brief Model Number(ER JR_LCD ECR) data=148.3/256, xdata=1755/1792, code=62425/63488, 2013.08.23, Russia: data=148.5/256, xdata=1755/1792, code=60743/63488, 2013.01.15*/
//#define MODEL_NUM MODEL_OMV4_ER2_LCD_ECR

/** @brief Model Number(ER JR_LCD SATO) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_ER2_LCD_SATO

/** @brief Model Number(EB) data=133.0/256, xdata=1783/1792, code=58931/63488, 2011.04.11, Portugal: data=133.1/256, xdata=1782/1792, code=59059/63488, 2013.01.17*/
/*Austrailia: data=133.1/256, xdata=1785/1792, code=59374/63488, 2013.01.17*/
//#define MODEL_NUM MODEL_OMV4_EB_LCD

/** @brief Model Number(PW-II) data=    , xdata=         , code=           , 2017.01.20*/
//#define MODEL_NUM MODEL_OMV4_PW2_LCD

/** @brief Model Number(PB LCD(OMV4)) data=    , xdata=         , code=           , 2018.12.19*/
//#define MODEL_NUM MODEL_OMV4_PB_LCD

/** @brief Model Number(SW LCD_ECR) data=    , xdata=         , code=           , 2018.09.13*/
//#define MODEL_NUM MODEL_OMV4_SW2W_LCD

/** @brief model type number(ER-PLUS LCD Clerk send version)  data=    , xdata=         , code=           , 2018.09.13*/
//#define MODEL_NUM MODEL_OMV4_ER_LCD_CLERK

/** @brief Model Number(SW LCD_ECR) data=    , xdata=         , code=           , 2018.09.13*/
//#define MODEL_NUM MODEL_OMV4_SW2W_LED

/** @brief Model Number(PDN) data=    , xdata=         , code=           , 2019.07.24*/
#define MODEL_NUM MODEL_OMV4_PDN_LED

//[end] OMV4 Model List

#if MODEL_NUM == MODEL_OMV4_PB_LCD
    /** @brief Software Version(SW)*/
    #define VERSION_NUM 412
    /** @brief Software Version(RUSSIA, SW)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA, SW)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 2479
    /** @brief Scale use all weight unit(kg, lb, oz, gram)*/
    #define USE_ALL_WEIGHT_UNIT
    /** @brief USE SW's Program mode*/
    #define USE_PGM_PB
    //#define USE_PGM_SW
    /** @brief USE Count & Compare mode*/
    #define USE_COUNT_COMPARE
    /** @brief Filter default value*/
    #define AD_FILT_DEFAULT_VALUE 		21
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE simple weighing sale mode */
    #define USE_SALE_SIMPLE
    /** @brief USE Non Numeric editor */
    #define USE_NON_NUMERIC_EDITOR
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE CAL6 Change Key Type */
    #define USE_CAL6_KEY_TYPE
    /** @brief USE BAcklight Menu */
    #define USE_BL_MENU
	/** @brief USE Smart BT protocol */
	#define USE_SMART_BT_PROTOCOL
    /** @brief for Use Uart1*/
    #define USE_UART1
    /** @brief for Use REVERSE DISPLAY*/
//    #define REVERSE_DISPLAY
    /** @brief USE Certification when printing*/
    #define USE_PRINT_CERTIFICATION

    /** @brief AD (in CPU) OPERATING VOLTAGE VALUE */
    #define AD_OPERATING_VOLTAGE_3_3V

//    /** @brief USE RS232 Option */
    //#define USE_RS232

/**	@brief USE Not Print for unstable mode */
    //#define USE_NOT_UNSTABLE  //for OIML Certification
    /** @brief USE Zero tracking range setting */
    #define USE_ZERO_TRACKING_RANGE_SETTING
    /** @brief USE Change initial zero range setting */
    #define USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Tare Auto Clear at Zero point  */
    #define USE_TARE_AUTO_CLEAR_SET
    /** @brief USE Auto Tare Function  */
    #define USE_AUTO_TARE_FUNCTION 
    /** @brief USE Simple Scale's DLP Printer mode */
    #define USE_SIMPLE_PRT_SELECT
    /** @brief USE CANADA Ceritfication */
    #define USE_KEY_ZERO_FOR_CERTIFICATION
	/** @brief USE CTS MODE */
	//#define USE_CTS_MODE 
#ifdef USE_CTS_MODE
    /** @brief Software Version(PB_CTS)*/
	#undef VERSION_NUM
    #define VERSION_NUM 461 //CTS 사용 시 해당 버전으로 적용
    /** @brief USE CTS_ONE_DP*/
    #define CTS_ONE_DP
    /** @brief Model name string*/
	#define MODEL_NAME_STR  "PB          "   // 공백 부분 스페이스 입력 필요
	/** @brief USE rtc mode */
    #define USE_RTC
#endif
    #ifdef COUNTRY_THAILAND
    /** @brief USE not Print Weight Unit*/
    #define NO_USE_PRT_WEIGHT_UNIT
    /** @brief USE Print Stable Zero Weight*/
    #define USE_STABLE_ZERO_WEIGHT_PRT
    
    #define PRT_PUT_NUM_FILL_CHAR '0'
    /** @brief Not USE Hold Key */
    #define NO_USE_HOLD_KEY
    #endif

	/** @brief Nation Define(SW1SCU_LCD)*/
//    #define COUNTRY_GUATEMALA
    
    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
    /** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
    /** @brief for Use Uart1*/
    //#define USE_UART1
    /* @brief This macro disable Low-Voltage-Reset function. */
    #define USE_DISABLE_LVR
    /** @brief USE Display reset in Display drive procedure*/
    #define USE_DISP_RESET
    
    #ifdef COUNTRY_GUATEMALA
        #define USE_LIBRA_ESPANOLA_UNIT_CHANGE
    #endif
#endif//#if MODEL_NUM ==MODEL_NUM_MODEL_OMV4_PB_LCD

#if MODEL_NUM == MODEL_OMV4_SW1SCU_LCD
    /** @brief Software Version(SW)*/
    #define VERSION_NUM 412
    /** @brief Software Version(RUSSIA, SW)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA, SW)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 2475
    /** @brief USE Russia certificated version display */
    #define USE_RUSSIA_VERSION_DISPLAY 217
    /** @brief Scale use all weight unit(kg, lb, oz, gram)*/
    #define USE_ALL_WEIGHT_UNIT
    /** @brief USE SW's Program mode*/
    #define USE_PGM_SW
    /** @brief USE Count & Compare mode*/
    #define USE_COUNT_COMPARE
    /** @brief Filter default value*/
    #define AD_FILT_DEFAULT_VALUE 		21
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE simple weighing sale mode */
    #define USE_SALE_SIMPLE
    /** @brief USE Non Numeric editor */
    #define USE_NON_NUMERIC_EDITOR
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE CAL6 Change Key Type */
    #define USE_CAL6_KEY_TYPE
    /** @brief USE RS232 Option */
    #define	USE_RS232
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Underload Limit Change (-10Digit) (for Japan) */
    #define USE_UNDERLOAD_10DIGIT
    /** @brief USE Temp compensation */
    //#define USE_TEMP_COMP	//for CANADA
    /** @brief USE Creep compensation */
    //#define USE_CREEP_COMP	//for CANADA
    /** @brief USE 0.1kg capa table */
    //#define USE_UNIT_0_1KG_CAPA_TABLE //for China special
    #ifdef USE_UNIT_0_1KG_CAPA_TABLE
    /** @brief Model Edition view */
    #define EDITION_STR " LC"
	/** @brief USE CTS MODE */
	//#define USE_CTS_MODE 
#ifdef USE_CTS_MODE
    /** @brief Software Version(SW_CTS)*/
	#undef VERSION_NUM
    #define VERSION_NUM 461 //CTS 사용 시 해당 버전으로 적용
    /** @brief USE CTS_ONE_DP_UNDER6*/
    #define CTS_ONE_DP_UNDER6
    /** @brief Model name string*/
	#define MODEL_NAME_STR  "SW          "   // 공백 부분 스페이스 입력 필요
#endif
    #endif
	/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
	//#define USE_VERSION_STR_DOT

	/** @brief Nation Define(SW1SCU_LCD)*/
	//#define COUNTRY_GUATEMALA
    
    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
    /** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
    /** @brief USE rtc mode */
    #define USE_RTC
    /** @brief for Use Uart1*/
    //#define USE_UART1
    /* @brief This macro disable Low-Voltage-Reset function. */
    #define USE_DISABLE_LVR
    /** @brief USE Display reset in Display drive procedure*/
    #define USE_DISP_RESET
    
    #ifdef COUNTRY_GUATEMALA
        #define USE_LIBRA_ESPANOLA_UNIT_CHANGE
    #endif
#endif//#if MODEL_NUM == MODEL_OMV4_SW1SCU_LCD

#if MODEL_NUM == MODEL_OMV4_SW1SU_LCD_ECR
	/** @brief Software Version(SW)*/
	#define VERSION_NUM 412
	/** @brief Software Version(RUSSIA, SW)*/
	#define VERSION_NUM_RUSSIA 217
	/** @brief Software Version(SOUTH AFRICA, SW)*/
	#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
	/** @brief Software Revision*/
	#define REVISION_NUM 2475
	/** @brief Model Edition view */
	#define EDITION_STR " ECR"
	/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
	#define USE_ALL_WEIGHT_UNIT
	/** @brief USE SW's Program mode*/
	#define USE_PGM_SW
	/** @brief USE Count & Compare mode*/
	#define USE_COUNT_COMPARE
	/** @brief Filter default value*/
	#define AD_FILT_DEFAULT_VALUE 		21
	/** @brief USE ECR communication API */
	#define USE_ECR_COMM_API
	/** @brief USE simple weighing sale mode */
	#define USE_SALE_SIMPLE
	/** @brief USE RS232 Option */
	#define	USE_RS232
	/** @brief USE Printer */
	#define USE_PRINTER
	/** @brief USE Non Numeric editor */
	#define USE_NON_NUMERIC_EDITOR
	/** @brief USE CAL6 Change Key Type */
	#define USE_CAL6_KEY_TYPE
	/**	@brief USE SW ECR Option */
	#define USE_WEIGHT_ONLY_ECR
	/** @brief USE Change initial zero range setting */
	#define	USE_CHANGE_ZERO_RANGE_SETTING
	/** @brief USE fifteen percent Initial zero range  */
	#define	USE_FIFTEEN_ZERO_RANGE
	/** @brief USE DIALOG6 */
	#define DIALOG6
	/** @brief USE Tare Auto Clear at Zero point  */
	#define USE_TARE_AUTO_CLEAR_SET
	/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
	//#define USE_VERSION_STR_DOT
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
	/** @brief USE CTS MODE */
	//#define USE_CTS_MODE 
#ifdef USE_CTS_MODE
    /** @brief Software Version(SW_CTS)*/
	#undef VERSION_NUM
    #define VERSION_NUM 460 //CTS 사용 시 해당 버전으로 적용
    /** @brief USE CTS_ONE_DP_UNDER6*/
    #define CTS_ONE_DP_UNDER6
    /** @brief Model name string*/
	#define MODEL_NAME_STR  "SW          "   // 공백 부분 스페이스 입력 필요
	/** @brief USE rtc mode */
	#define USE_RTC
#endif
	/** @brief Compiler option : use IAR */
	#define IAR
	/** @brief USE global typedef for 32bit define */
	#define USE_32BIT_GLOBAL_TYPE
	/** @brief USE 32bit packed for iar define */
	#define USE_32BIT_PACKED_IAR
	/** @brief USE M0516LDN MCU */
	#define USE_M0516LDN_MCU
    /** @brief USE Display reset in Display drive procedure*/
    #define USE_DISP_RESET
#endif


#if MODEL_NUM == MODEL_OMV4_SWLRWR_LED
    /** @brief Nation Define(SWLR)*/
    //#define COUNTRY_RUSSIA
    //#define COUNTRY_CHINA
    //#define COUNTRY_GUATEMALA
    
    /** @brief Software Version(SWLR)*/
    #define VERSION_NUM 410
    /** @brief Software Version(RUSSIA, SWLR)*/
    #define VERSION_NUM_RUSSIA 217
    /** @brief Software Version(SOUTH AFRICA, SWLR)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 1867
    /** @brief Scale use all weight unit(kg, lb, oz, gram)*/
    #define USE_ALL_WEIGHT_UNIT
    /** @brief USE SW's Program mode*/
    #define USE_PGM_SW
    /** @brief USE Count & Compare mode*/
    #define USE_COUNT_COMPARE
    /** @brief USE Suspend mode*/
    #define USE_SUSPEND_MODE
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE simple weighing sale mode */
    #define USE_SALE_SIMPLE
    /** @brief USE Non Numeric editor */
    #define USE_NON_NUMERIC_EDITOR
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE CAL6 Change Key Type */
    #define USE_CAL6_KEY_TYPE
    /** @brief USE RS232 Option */
    #define	USE_RS232
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    
    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
    
    #ifdef COUNTRY_CHINA
        /** @brief USE Fast update weight display value (China) */
        #define USE_FAST_DISPLAYING_WEIGHT
        /** @brief for Fast update weight display value change ad rate (China)*/
        #define AD_RAW_PROC_RATE	16L
        /** @brief for Fast shock filter for hish speed display weight (China)*/
        #define FAST_BREAK_SHOCK_FILTER
        /** @brief for Fast average filter for hish speed display weight (China)*/
        #define FAST_AVERAGE_IN_LOW_AD
    #endif
    #ifdef COUNTRY_GUATEMALA
        #define USE_LIBRA_ESPANOLA_UNIT_CHANGE
    #endif
#endif//#if MODEL_NUM == MODEL_OMV4_SWLRWR_LED

#if MODEL_NUM == MODEL_OMV4_DB1H_VFD
//#define COUNTRY_RUSSIA
//#define COUNTRY_TURKEY
//#define COUNTRY_NETHERLAND

    #define VERSION_NUM 410
	/** @brief Software Version(RUSSIA, SW)*/
	#define VERSION_NUM_RUSSIA VERSION_NUM
	/** @brief Software Version(SOUTH AFRICA, SW)*/
	#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
	/** @brief Software Revision*/
	#define REVISION_NUM 1845

	/** @brief Compiler option : use IAR */
	#define IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
	/** @brief USE 32bit packed for iar define */
	#define USE_32BIT_PACKED_IAR
	
	/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
	#define USE_ALL_WEIGHT_UNIT
	/** @brief USE DB's Program mode*/
	#define USE_PGM_DB1H
	/** @brief USE Print mode*/
	//define USE_PRINT_KEY
	/** @brief For Print mode*/
	#define USE_COUNT_COMPARE_PERCENT
	/** @brief USE Standard communication API */
	#define USE_STD_COMM_API
	/** @brief USE bench scale sale mode */
	#define USE_SALE_BENCH
	/** @brief USE Non Numeric editor */
	#define USE_NON_NUMERIC_EDITOR
	/** @brief USE Printer */
	#define USE_PRINTER
	/** @brief USE CAL6 Change Key Type */
	#define USE_CAL6_KEY_TYPE
	/** @brief USE Change initial zero range setting */
	#define	USE_CHANGE_ZERO_RANGE_SETTING
	/** @brief USE fifteen percent Initial zero range  */
	#define	USE_FIFTEEN_ZERO_RANGE
	/** @brief USE MOON's Gravity  */
	//#define	USE_MOON_GRAVITY	
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020

#endif	//#if MODEL_NUM == MODEL_OMV4_DB1H_VFD

#if MODEL_NUM == MODEL_OMV4_ER_LCD

    /** @brief Nation Define(ER_LCD)*/
    #define COUNTRY_UN			//영공사양, Key type을 국거별로 정의하기 위해 정의함
    //#define COUNTRY_TURKEY
    //#define COUNTRY_AUSTRALIA
    //#define COUNTRY_PORTUGAL
    //#define COUNTRY_NEWZEALAND
    //#define COUNTRY_PERU
    //#define COUNTRY_RUSSIA
    //#define COUNTRY_GUATEMALA
    //#define COUNTRY_GERMANY
    
    /** @brief Compiler option : use IAR */
    #define IAR
    
    /** @brief Software Version(ER_LCD)*/
    #define VERSION_NUM 413
    /** @brief Software Version(RUSSIA, ER_LCD)*/
    #define VERSION_NUM_RUSSIA 214
    /** @brief Software Version(SOUTH AFRICA, ER_LCD)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 2150
    /** @brief USE ER's Program mode*/
    #define USE_PGM_ER
    /** @brief Don't USE Count mode*/
    #define NO_USE_COUNT
    /** @brief USE CTS MODE */
    //#define USE_CTS_MODE
#ifdef  USE_CTS_MODE
    /** @brief USE_CTS_TRIANGLE */
    //#define USE_CTS_TRIANGLE
    /** @brief Software Version(ER_LCD_CTS)*/
	#undef VERSION_NUM
    #define VERSION_NUM 460 //CTS 사용 시 해당 버전으로 적용
    /** @brief Model name string*/
    #define MODEL_NAME_STR "ER PLUS     "   // 공백 부분 스페이스 입력 필요
#endif
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE price computing sale mode */
    #define USE_SALE_PRICE_EXTERNAL_PLU
    /** @brief USE Numeric editor */
    #define USE_NUMERIC_EDITOR
    /** @brief USE 6 digit total price window */
    #define USE_TOTAL_6DIGIT
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE RTC */
    #define USE_RTC
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Creep compensation */
    //#define USE_CREEP_COMP	//for CANADA
    //#define USE_CITIZEN_PRINTER	                                        //only use citizen printer
    /** @brief USE Price Auto Clear User set  */
    #define	USE_PRICE_AUTO_CLEAR_SET
    /** @brief USE Tare Auto Clear User set  */
    #define	USE_TARE_AUTO_CLEAR_SET
    /** @brief USE Decimal Point for ER Series */
    //#define USE_DECIMAL_POINT // csh 20120731
    /** @brief USE PLU Quick Store Function */
    #define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
    
    #ifdef COUNTRY_UN
        /** @brief USE Direct PLU key type(for UN) */
        #define USE_DIRECT_PLUKEY_TYPE_UN
    #endif
    
    #ifdef COUNTRY_AUSTRALIA
        /** @brief USE Add key for certification */
        #define USE_ADD_KEY_FOR_CERTIFICATION							   	// dont' use with USE_PESO_PRINT
        /** @brief USE Direct PLU key type(for UN) */
        #define USE_DIRECT_PLUKEY_TYPE_UN
    #endif
    
    #ifdef COUNTRY_PORTUGAL
        /** @brief USE PESO DEP print(for Portugal) */
        #define USE_PESO_PRINT											// dont' use with USE_ADD_KEY_FOR_CERTIFICATION
        /** @brief USE Direct PLU key type(for UN) */
        #define USE_DIRECT_PLUKEY_TYPE_UN
    #endif
    
    #ifdef COUNTRY_TURKEY	//딜러롬 기능 주석처리, 딜러롬 컴파일 시 주석 해제
        /** @brief USE Add key & Tare key for continual sale (for Turkey) */
        //#define USE_TURKISH_ADD
        /** @brief USE Direct PLU key type(for turkey) */
        #define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
        /** @brief USE Contineous send weight data to ECR(for turkey) */
	    #define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
	    /** @brief USE ER communication API */
        #define USE_ER_COMM_API //Dealer
        #undef USE_STD_COMM_API //Dealer
        /** @brief USE Turkey POS Connection Protocol  */ 
        #define USE_TURKEY_POS_PROTOCOL //Dealer
        /** @brief USE key block for ER PLUS Turkey */ 
        #define USE_KEY_BLOCK	//Dealer
    #endif
    
    #ifdef COUNTRY_PERU
        /** @brief USE Cancel key for Daily Total Print (for Peru) */
        #define DAILY_TOTAL_PRINT
        /** @brief USE Direct PLU key type(for UN) */
        #define USE_DIRECT_PLUKEY_TYPE_UN
    #endif
    
    #ifdef COUNTRY_GERMANY
        /** @brief USE Direct PLU key type(for Germany) */
        #define USE_DIRECT_PLUKEY_TYPE_M1_M9
		/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
		//#define USE_VERSION_STR_DOT
		/** @brief PLU not cleared after setting tare */
		#define USE_PLU_AFTER_TARE_SET
        /** @brief Check AD read completed */
		#define USE_AD_READ_COMPLETE_CHECK
    #endif
    
    /** @brief USE Contineous send weight data to ECR(for turkey) */
    //#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
    /** @brief USE Command tare key function(for turkey) */
    //#define USE_COMMAND_TARE_KEY	//turkey ECR company
    
    #ifdef COUNTRY_NEWZEALAND
        /** @brief USE Multiple print function (for NewZealand) */
        #define USE_MULTI_PRINT_FUNC
        /** @brief USE Direct PLU key type(for UN) */
        #define USE_DIRECT_PLUKEY_TYPE_UN
    #endif
    
    #ifdef COUNTRY_GUATEMALA
        #define USE_LIBRA_ESPANOLA_UNIT_CHANGE
        /** @brief USE Direct PLU key type(for UN) */
        #define USE_DIRECT_PLUKEY_TYPE_UN
    #endif
    
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief Fast display define */

#endif

#if MODEL_NUM == MODEL_OMV4_ER_LCD_ECR
	/** @brief Nation Define(OMV4_ER_LCD_ECR)*/
	//#define COUNTRY_NETHERLAND
	//#define COUNTRY_REPUBLIC_OF_SOUTH_AFRICA //남아공 기능이 Dislog6와 String Mode 영공 기능에 합병 - 남아공사양 사용 안함
	//#define COUNTRY_MEXICO //멕시코 선언시 ECR type #2,3,4 의 프로토콜이 'I'에서 동작 하도록 수정, RS-232 config(8 databits, parity none, 1 stop bits, 9600bps)
	//#define COUNTRY_TURKEY
	//#define COUNTRY_GERMANY
	//#define COUNTRY_CZECH
	#define COUNTRY_UN
	//#define COUNTRY_SLOVAKIA
 
    /** @brief Compiler option : use IAR */
	#define IAR
	/** @brief USE global typedef for 32bit define */
	#define USE_32BIT_GLOBAL_TYPE
	/** @brief USE 32bit packed for iar define */
	#define USE_32BIT_PACKED_IAR
	/** @brief USE M0516LDN MCU */
	#define USE_M0516LDN_MCU
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
	/** @brief Model Edition view */
	#define EDITION_STR " ECR"
	
	/** @brief Software Version(ER_LCD)*/
	#define VERSION_NUM 410
	/** @brief Software Version(RUSSIA, PR_LED)*/
	#define VERSION_NUM_RUSSIA VERSION_NUM
	/** @brief Software Version(SOUTH AFRICA, PR_LED)*/
	#define VERSION_NUM_SOUTH_AFRICA 201
	/** @brief Software Revision*/
	#define REVISION_NUM 2305
	
	
	/** @brief USE ER's Program mode*/
	#define USE_PGM_ER
	/** @brief Don't USE Count mode*/
	#define NO_USE_COUNT
	/** @brief USE Standard communication API */
	#define USE_ECR_COMM_API
	/** @brief USE price computing sale mode */
	#define USE_SALE_PRICE_EXTERNAL_PLU
	/** @brief USE Numeric editor */
	#define USE_NUMERIC_EDITOR
	/** @brief USE 6 digit total price window */
	#define USE_TOTAL_6DIGIT
	/** @brief USE Change initial zero range setting */
	#define	USE_CHANGE_ZERO_RANGE_SETTING
	/** @brief USE fifteen percent Initial zero range  */
	#define	USE_FIFTEEN_ZERO_RANGE
	/** @brief USE Printer */
	//#define USE_PRINTER
	/** @brief USE DIALOG6 */
	#define DIALOG6 //기존 TYPE 11과 TYPE 12가 중복되어 TYPE 11 제거 TYPE 14 : string mode
					// DIALOG6 기능 사용 시 USE_PRICE_AUTO_CLEAR_SET, USE_TARE_AUTO_CLEAR_SET, USE_QUICK_STORE_FUNCTION 기능 Disable
	/** @brief USE ANKER PROTOCL TYPE17 */
	#define USE_ECR_ANKER_PROTOCOL
	#ifdef COUNTRY_UN
	/** @brief USE Direct PLU key type(for UN) */
	#define USE_DIRECT_PLUKEY_TYPE_UN
	#endif
	
	#ifdef COUNTRY_TURKEY
	/** @brief USE Direct PLU key type(for turkey) */
	#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
	#endif
	
	#ifdef COUNTRY_GERMANY
	/** @brief USE Direct PLU key type(for Germany) */
	#define USE_DIRECT_PLUKEY_TYPE_M1_M9
	/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
	//#define USE_VERSION_STR_DOT		// 버전표시 : V4.xx.0 (버전이 404이면 V4.04.0 으로 표시)
	#endif
	
	#ifdef COUNTRY_CZECH
	/** @brief USE Direct PLU key type(for UN) */
	#define USE_DIRECT_PLUKEY_TYPE_UN
	#define USE_2BYPE_CHECKSUM_PROTOCOL
	#endif

	#ifdef COUNTRY_NETHERLAND
	 /** @brief USE Direct PLU key type(for UN) */
	 #define USE_DIRECT_PLUKEY_TYPE_UN
	 //#define USE_DIALOG2_5DIGIT_PRICE //dealer test ver.
	#endif
	
	#ifdef COUNTRY_SLOVAKIA
	/** @brief USE Direct PLU key type(for UN) */
	#define USE_DIRECT_PLUKEY_TYPE_UN
	#define USE_SLOVAKIA_PROTOCOL
	#endif
#endif

#if MODEL_NUM == MODEL_OMV4_ER_LCD_SATO
 
    /** @brief Compiler option : use IAR */
	#define IAR
	/** @brief USE global typedef for 32bit define */
	#define USE_32BIT_GLOBAL_TYPE
	/** @brief USE 32bit packed for iar define */
	#define USE_32BIT_PACKED_IAR
	/** @brief USE M0516LDN MCU */
	#define USE_M0516LDN_MCU
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
	/** @brief Model Edition view */
	#define EDITION_STR "SATO"
	
	/** @brief Software Version(ER_LCD)*/
	#define VERSION_NUM 413
    /** @brief Software Version(RUSSIA, ER_LED)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA, ER_LED)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
	/** @brief Software Revision*/
	#define REVISION_NUM 2305
	
	
	/** @brief USE ER's Program mode*/
	#define USE_PGM_ER
	/** @brief Don't USE Count mode*/
	#define NO_USE_COUNT
	/** @brief USE Standard communication API */
	#define USE_ECR_COMM_API
	/** @brief USE price computing sale mode */
	#define USE_SALE_PRICE_EXTERNAL_PLU
	/** @brief USE Numeric editor */
	#define USE_NUMERIC_EDITOR
	/** @brief USE 6 digit total price window */
	#define USE_TOTAL_6DIGIT
	/** @brief USE Change initial zero range setting */
	#define	USE_CHANGE_ZERO_RANGE_SETTING
	/** @brief USE fifteen percent Initial zero range  */
	#define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Price Auto Clear User set  */
    #define	USE_PRICE_AUTO_CLEAR_SET
    /** @brief USE Tare Auto Clear User set  */
    #define	USE_TARE_AUTO_CLEAR_SET
	/** @brief USE Printer */
	//#define USE_PRINTER
	/** @brief USE DIALOG6 */
	//#define DIALOG6 //기존 TYPE 11과 TYPE 12가 중복되어 TYPE 11 제거 TYPE 14 : string mode
					// DIALOG6 기능 사용 시 USE_PRICE_AUTO_CLEAR_SET, USE_TARE_AUTO_CLEAR_SET, USE_QUICK_STORE_FUNCTION 기능 Disable
    /** @brief USE SATO Printer protocol */
    #define USE_SATO_PRINTER_PROTOCOL
    /** @brief PLU not cleared after setting tare */
    #define USE_PLU_AFTER_TARE_SET
    /** @brief Check AD read completed */
	#define USE_AD_READ_COMPLETE_CHECK

    #define	USE_FIFTEEN_ZERO_RANGE   
	/** @brief USE Direct PLU key type(for Germany) */
	#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif


#if MODEL_NUM == MODEL_OMV4_ER_LED
/** @brief Compiler option : use IAR */
#define IAR
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR
/** @brief USE M0516LDN MCU */
#define USE_M0516LDN_MCU
/** @brief Software Version(ER_LED)*/
#define VERSION_NUM 410
/** @brief Software Version(RUSSIA, ER_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, ER_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1872

/** @brief Nation Define(ER_LED)*/
#define COUNTRY_UN
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY
//#define COUNTRY_NEWZEALAND
//#define COUNTRY_GUATEMALA

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
/* @brief This macro disable Low-Voltage-Reset function. */
//#define USE_DISABLE_LVR
/** @brief use check ad vendor: ob1020, xm24l */
#define USE_CHECK_AD_VENDOR //if not select, default is OB1020

#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7	//LED, VFD전용 키 사양 (Backlight ON/OFF Key 제외)
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#ifdef COUNTRY_GUATEMALA
#define USE_UNIT_SPENISH_OZ //ER PLUS LED에서 온즈 기능 사용시 단가표시 LED 설정.
#define USE_LIBRA_ESPANOLA_UNIT_CHANGE
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_NEWZEALAND
/** @brief USE Multiple print function (for NewZealand) */
#define USE_MULTI_PRINT_FUNC
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif
#endif

#if MODEL_NUM == MODEL_OMV4_ER_PRO_LCD
    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief Software Version(ER_PLUS_M_LCD)*/
    #define VERSION_NUM 410
    /** @brief Software Version(RUSSIA)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 1927
    
    /** @brief Nation Define(ER_PLUS_M_LCD)*/
    #define COUNTRY_UN
    //#define COUNTRY_TURKEY
    //#define COUNTRY_GERMANY
    
    /** @brief USE ER's Program mode*/
    #define USE_PGM_ER
    /** @brief Don't USE Count mode*/
    #define NO_USE_COUNT
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE price computing sale mode */
    #define USE_SALE_PRICE_EXTERNAL_PLU
    /** @brief USE Numeric editor */
    #define USE_NUMERIC_EDITOR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU	
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
    
    #if MODEL_NUM == MODEL_OMV4_ER_PRO_LCD
        /** @brief USE 8 digit total price window */
        #define USE_TOTAL_8DIGIT
        /** @brief USE Decimal Point for ER Series */
        //#define USE_DECIMAL_POINT // csh 20120731
        #else//MODEL_NUM == MODEL_ER_PRO_6LCD
        /** @brief USE 6 digit total price window */
        #define USE_TOTAL_6DIGIT
        /** @brief USE Decimal Point for ER Series */
        //#define USE_DECIMAL_POINT // csh 20120731
    #endif
    
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE RTC */
    #define USE_RTC
    /** @brief USE Creep compensation */
    //#define USE_CREEP_COMP	//for CANADA
    /** @brief USE HT1622 Buzzer Driver */
    //#define USE_HT1622_BUZZER_DRV
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Price Auto Clear User set  */
    //#define	USE_PRICE_AUTO_CLEAR_SET			// Code size 오버로 인해 부가 기능 삭제
    /** @brief USE Tare Auto Clear User set  */
    //#define	USE_TARE_AUTO_CLEAR_SET			// Code size 오버로 인해 부가 기능 삭제
    /** @brief USE PLU Quick Store Function */
    #define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
    /** @brief USE ER_PLUS_M_TURKEY_KEY DEFINE */
    //#define USE_ER_PLUS_M_TURKEY_KEY_DEFINE
    
    
    #ifdef COUNTRY_UN
        /** @brief USE Direct PLU key type(for UN) */
        #define USE_DIRECT_PLUKEY_TYPE_UN
    #endif
    
    #ifdef COUNTRY_TURKEY	//딜러롬 기능 주석처리-> 터키 요청으로 딜러기능 생산적용
        /** @brief USE Add key & Tare key for continual sale (for Turkey) */
        #define USE_TURKISH_ADD
        /** @brief USE Contineous send weight data to ECR(for turkey) */
        #define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
        /** @brief USE Command tare key function(for turkey) */
        #define USE_COMMAND_TARE_KEY	//turkey ECR company
        /** @brief USE Direct PLU key type(for turkey) */
        #define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
    #endif
    
    #ifdef COUNTRY_GERMANY
        /** @brief USE Direct PLU key type(for Germany) */
        #define USE_DIRECT_PLUKEY_TYPE_M1_M9
    #endif
    
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief Fast display define */

#endif

#if MODEL_NUM == MODEL_OMV4_ER_PRO_LCD_ECR
	/** @brief Software Version(ER_PLUS_M_LCD)*/
	#define VERSION_NUM 410
	/** @brief Software Version(RUSSIA)*/
	#define VERSION_NUM_RUSSIA VERSION_NUM
	/** @brief Software Version(SOUTH AFRICA)*/
	#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
	/** @brief Software Revision*/
	#define REVISION_NUM 2158

	/** @brief Nation Define(ER_PLUS_M_LCD)*/
	#define COUNTRY_UN
	//#define COUNTRY_TURKEY
	//#define COUNTRY_GERMANY
	//#define COUNTRY_HUNGARY
	

	/** @brief Compiler option : use IAR */
	#define IAR
	/** @brief USE global typedef for 32bit define */
	#define USE_32BIT_GLOBAL_TYPE
	/** @brief USE 32bit packed for iar define */
	#define USE_32BIT_PACKED_IAR
	/** @brief USE M0516LDN MCU */
	#define USE_M0516LDN_MCU
	/** @brief USE ER's Program mode*/
	#define USE_PGM_ER
	/** @brief Don't USE Count mode*/
	#define NO_USE_COUNT
	/** @brief USE Standard communication API */
	#define USE_ECR_COMM_API
	/** @brief USE price computing sale mode */
	#define USE_SALE_PRICE_EXTERNAL_PLU
	/** @brief USE Numeric editor */
	#define USE_NUMERIC_EDITOR
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020

	#if MODEL_NUM == MODEL_OMV4_ER_PRO_LCD_ECR
	/** @brief USE 8 digit total price window */
	#define USE_TOTAL_8DIGIT
	#else//MODEL_NUM == MODEL_ER_PRO_6LCD_ECR
	/** @brief USE 6 digit total price window */
	#define USE_TOTAL_6DIGIT
	#endif
	/** @brief USE DIALOG6 */
	#define DIALOG6 //기존 TYPE 11과 TYPE 12가 중복되어 TYPE 11 제거 TYPE 14 : string mode

	/** @brief USE ANKER PROTOCL TYPE17 */
	#define USE_ECR_ANKER_PROTOCOL

	/** @brief USE Printer */
	//#define USE_PRINTER
	/** @brief USE RTC */
	#define USE_RTC
	/** @brief USE Creep compensation */
	//#define USE_CREEP_COMP	//for CANADA
	/** @brief USE HT1622 Buzzer Driver */
	//#define USE_HT1622_BUZZER_DRV
	/** @brief USE Change initial zero range setting */
	#define	USE_CHANGE_ZERO_RANGE_SETTING
	/** @brief USE fifteen percent Initial zero range  */
	#define	USE_FIFTEEN_ZERO_RANGE
	/** @brief USE Price Auto Clear User set  */
	//#define	USE_PRICE_AUTO_CLEAR_SET		// Code size 오버로 인해 부가 기능 삭제
	/** @brief USE Tare Auto Clear User set  */
	//#define	USE_TARE_AUTO_CLEAR_SET			// Code size 오버로 인해 부가 기능 삭제
	/** @brief USE PLU Quick Store Function */
	//#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장 (ER-M ECR: Code size overflow)
	/** @brief USE Decimal Point for ER Series */
	//#define USE_DECIMAL_POINT // csh 20120731

	#ifdef COUNTRY_UN
	/** @brief USE Direct PLU key type(for UN) */
	#define USE_DIRECT_PLUKEY_TYPE_UN
	#endif
	#ifdef COUNTRY_TURKEY	//딜러롬 기능 주석 처리
	/** @brief USE Add key & Tare key for continual sale (for Turkey) */
	//#define USE_TURKISH_ADD
	/** @brief USE Contineous send weight data to ECR(for turkey) */
	//#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
	/** @brief USE Command tare key function(for turkey) */
	//#define USE_COMMAND_TARE_KEY	//turkey ECR company
	/** @brief USE Direct PLU key type(for turkey) */
	#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
	#endif
	#ifdef COUNTRY_GERMANY
	/** @brief USE Direct PLU key type(for Germany) */
	#define USE_DIRECT_PLUKEY_TYPE_M1_M9
	#endif
	#ifdef COUNTRY_HUNGARY
	/** @brief USE Direct PLU key type(for UN) */
	#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD
	#define USE_HUNGARY_CURRENCY	//헝가리 화폐기호 사용
	#endif
#endif


#if MODEL_NUM == MODEL_OMV4_ER_PRO_LED

    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief Software Version(ER_LED)*/
    #define VERSION_NUM 410
    /** @brief Software Version(RUSSIA, ER_LED)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA, ER_LED)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 1936
    
    /** @brief Nation Define(ER_PLUS_M_LCD)*/
    #define COUNTRY_UN
    //#define COUNTRY_TURKEY
    //#define COUNTRY_GERMANY
    
    /** @brief USE ER's Program mode*/
    #define USE_PGM_ER
    /** @brief Don't USE Count mode*/
    #define NO_USE_COUNT
    /** @brief USE Suspend mode*/
    #define USE_SUSPEND_MODE
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE price computing sale mode */
    #define USE_SALE_PRICE_EXTERNAL_PLU
    /** @brief USE Numeric editor */
    #define USE_NUMERIC_EDITOR
    /** @brief USE 8 digit total price window */
    #define USE_TOTAL_8DIGIT
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE RTC */
    #define USE_RTC
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Price Auto Clear User set  */
    //#define	USE_PRICE_AUTO_CLEAR_SET	// Code size 오버로 인해 부가 기능 삭제
    /** @brief USE Tare Auto Clear User set  */
    //#define	USE_TARE_AUTO_CLEAR_SET	// Code size 오버로 인해 부가 기능 삭제
    /** @brief USE PLU Quick Store Function */
    #define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
    /** @brief USE Decimal Point for ER Series */
    //#define USE_DECIMAL_POINT // csh 20120731
    /** @brief USE ER_PLUS_M_TURKEY_DEFINE */
    //#define USE_ER_PLUS_M_TURKEY_KEY_DEFINE
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
    
    #ifdef COUNTRY_UN
        /** @brief USE Direct PLU key type(for UN) */
        #define USE_DIRECT_PLUKEY_TYPE_UN
    #endif
    
    #ifdef COUNTRY_GERMANY
        /** @brief USE Direct PLU key type(for Germany) */
        #define USE_DIRECT_PLUKEY_TYPE_M1_M9
    #endif
    
    #ifdef COUNTRY_TURKEY	//딜러 롬 기능 주석처리
        /** @brief USE Add key & Tare key for continual sale (for Turkey) */
        #define USE_TURKISH_ADD
        /** @brief USE Contineous send weight data to ECR(for turkey) */
        #define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
        /** @brief USE Command tare key function(for turkey) */
        #define USE_COMMAND_TARE_KEY	//turkey ECR company
        /** @brief USE Direct PLU key type(for turkey) */
        #define USE_DIRECT_PLUKEY_TYPE_M1_M7	//LED, VFD전용 키 사양 (Backlight ON/OFF Key 제외)
    #endif

#endif

#if MODEL_NUM == MODEL_OMV4_ED_LCD
    /** @brief Compiler option : use IAR */
    #define IAR
    
    /** @brief Nation Define(ED_LCD)*/
    //#define COUNTRY_UNITEDSTATES
    //#define COUNTRY_NETHERLAND
    //#define COUNTRY_POLAND
    //#define COUNTRY_TURKEY
    //#define COUNTRY_HUNGARY
    
    /** @brief Software Version(ED_LCD)*/
    #define VERSION_NUM 410
    /** @brief Software Version(RUSSIA, SW)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA, SW)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 2276
    /** @brief Scale use all weight unit(kg, lb, oz, gram)*/
    #define USE_ALL_WEIGHT_UNIT
    /** @brief USE DB's Program mode*/
    #define USE_PGM_DB
    /** @brief USE Count & Compare & Percent mode*/
    #define USE_COUNT_COMPARE_PERCENT
    /** @brief USE bench scale sale mode */
    #define USE_SALE_BENCH
    /** @brief USE Non Numeric editor */
    #define USE_NON_NUMERIC_EDITOR
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE RTC */
    #define USE_RTC
    /** @brief USE BAcklight Menu */
    #define USE_BL_MENU
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Real STREAM function and PC_TARE (for NETHERLAND) */
    //#define REAL_STREAM //real stream mode default print setting OFF, KEY, AUT, Real STR
    //#define PC_TARE //only use real stream mode    
	/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
	//#define USE_VERSION_STR_DOT
    
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
    
    #ifdef COUNTRY_TURKEY
        /** @brief USE ED communication API */
        #define USE_ED_COMM_API
        /** @brief USE Turkey POS Connection Protocol  */
        #define USE_TURKEY_POS_PROTOCOL
        /** @brief Block the Function Key(for Turkey) */
		#define TURKEY_BLOCK_FUNCTION_KEY
		/** @brief USE Contineous send weight data to ECR(for turkey) */
        #define USE_ED_CONTI_SEND_WEIGHT_DATA	//only turkey ECR dealer ver.
        #define USE_TARE_AUTO_CLEAR_SET
    #else
        /** @brief USE Standard communication API */
        #define USE_STD_COMM_API
    #endif
    
    #ifdef COUNTRY_UNITEDSTATES
        /** @brief NO USE Hold Key(for USA) */
        #define NO_USE_HOLD_KEY	
    #endif
    
    #ifdef COUNTRY_NETHERLAND
        /** @brief Extend the Hold Time(for Netherlands) */
        #define USE_EXTEND_HOLD_TIME
    #endif
    
    #ifdef COUNTRY_POLAND
        /** @brief Block the Function Key(for Polands) */
        #define BLOCK_FUNCTION_KEY
    #endif
    
    #ifdef COUNTRY_HUNGARY
		/** @brief USE Real STREAM function */
		#define REAL_STREAM //real stream mode default print setting OFF, KEY, AUT, Real STR
        /** @brief Use Add the Status Flag(for Hungary) */
        #define USE_ADD_STATUS_FLAG
    #endif

#endif

#if MODEL_NUM == MODEL_OMV4_ER2_LCD

    /** @brief Compiler option : use IAR */
    #define IAR
    
    /** @brief Nation Define(ER2_LCD)*/
    //#define COUNTRY_UNITEDSTATES
    //#define COUNTRY_TURKEY
    //#define COUNTRY_RUSSIA
    //#define COUNTRY_AUSTRALIA
	//#define COUNTRY_SPAIN
    
    /** @brief Software Version(ER2_LCD)*/
    #define VERSION_NUM 413
    /** @brief Software Version(RUSSIA, ER2_LCD)*/
    #define VERSION_NUM_RUSSIA 212
    /** @brief Software Version(SOUTH AFRICA, ER2_LCD)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 2306
    /** @brief USE EB's Program mode*/
    #define USE_PGM_EB
    /** @brief Don't USE Count mode*/
    #define NO_USE_COUNT
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE price computing sale mode */
    #define USE_SALE_PRICE
    /** @brief USE Numeric editor */
    #define USE_NUMERIC_EDITOR
    /** @brief USE 6 digit total price window */
    #define USE_TOTAL_6DIGIT
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
#if defined (COUNTRY_TURKEY) || defined (COUNTRY_SPAIN)
#else
    /** @brief USE Real STREAM function for ER Jr */
    #define USE_REAL_STREAM
#endif
    /** @brief USE Price Auto Clear User set  */
    #define	USE_PRICE_AUTO_CLEAR_SET
    /** @brief USE Tare Auto Clear User set  */
    #define	USE_TARE_AUTO_CLEAR_SET
    /** @brief USE PLU Quick Store Function */
    #define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
    /** @brief Use to dry battery and Pb battery(for TURKEY) */
    #define USE_DRY_AND_PB_BATT_6V
#ifdef USE_DRY_AND_PB_BATT_6V
    /** @brief Use to auto power setting function  */
    #define USE_AUTO_POWER_SETTING
#endif
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
	/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
	//#define USE_VERSION_STR_DOT
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
    /** @brief PLU not cleared after setting tare */
    #define USE_PLU_AFTER_TARE_SET
    /** @brief Check AD read completed */
	#define USE_AD_READ_COMPLETE_CHECK
    
    #ifdef COUNTRY_UNITEDSTATES
        /** @brief USE Command USA Key Table(for USA) */
        #define USE_KEY_USA	
        /** @brief Use to unit change setting (KG/LB/OZ) (for USA) */
        #define USE_USER_SET_UNIT
        /** @brief USE User set PLU tare finction for NTEP certification */
        #define USE_USER_SET_PLU_TARE
    #endif
    
    #ifdef COUNTRY_TURKEY
        /** @brief USE to turn alwyas On/Off of Backlight when Press backlight key(for TURKEY) */
        #define USE_BACKLIGHT_KEY_ONOFF_ALWAYS
        /** @brief USE User set PLU tare finction for NTEP certification */
        #define USE_USER_SET_PLU_TARE
	    /** @brief USE Contineous send weight data to ECR(for turkey) */
	    #define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
	    /** @brief USE Command tare key function(for turkey) */
	    #define USE_COMMAND_TARE_KEY	//turkey ECR company
	    
		#define USE_EB_STREAM
		/**	@brief USE (CAS 22Byte Protocol) Stream mode  */
		#define USE_STREAM_CAS_22BYTE // Multi print와 동시에 사용 할 수 없음.
		#define USE_ECR_TYPE_7	//ECR TYPE 7 프로토콜 사용

		//#define USE_DEALER_POS_DEALER_FUNC // Tureky Dearler ver 적용 디파인 Fucntion #1,2 둘 다 적용 필요.
		#ifdef USE_DEALER_POS_DEALER_FUNC		// Dealer ver.
			/** @brief USE ER communication API */
      		#define USE_ER_COMM_API //Dealer Function #1 ED POS Fucntion
        	#undef USE_STD_COMM_API //Dealer Function #1 ED POS Fucntion
     		/** @brief USE Turkey POS Connection Protocol  */ 
        	#define USE_TURKEY_POS_PROTOCOL //Dealer Function #2 Turkey stream Function
        	#undef USE_ECR_TYPE_7	//ECR TYPE 7 프로토콜 사용 (Tureky Dearler ver 기능 적용 안 함)
        #endif
    #endif
    
    #ifdef COUNTRY_RUSSIA
        /** @brief USE Decimal Point for ER Series */
        #define USE_DECIMAL_POINT // csh 20120731
        /** @brief USE Command Russia Key Table(for Russia) */
        #define USE_KEY_RUSSIA
        /** @brief USE User set PLU tare finction for NTEP certification */
        #define USE_USER_SET_PLU_TARE
    #endif
    
    #ifdef COUNTRY_AUSTRALIA
        /** @brief USE Add key for certification */
        #define USE_ADD_KEY_FOR_CERTIFICATION							   	// dont' use with USE_PESO_PRINT
    #endif

	#ifdef COUNTRY_SPAIN
        /** @brief USE PRII_SPAIN $-protocol(RS-PC) */
        #define USE_PRII_SPAIN_$_PROTOCOL

		/** 
		 * @brief 	USE PRII_SPAIN RS-CO protocol
		 * @remark	USE_REAL_STREAM(U-SND)과 다른 형식 
		 */ 
        #define USE_PRII_SPAIN_CO_PROTOCOL

		/** 
		 * @brief 	USE PRII_SPAIN RS-ST protocol
		 * @remark	USE_MULTI_PRINT_FUNC 디파인과 CAL4-5 2bit 영역을 겹치게 사용하므로 같이 사용하지 말 것
		 * 			CAL4-5의 0,1,2 bit 영역 사용
		 */
        #define USE_PRII_SPAIN_ST_PROTOCOL
	#endif

#endif

#if MODEL_NUM == MODEL_OMV4_ER2_LCD_ECR
	/** @brief Nation Define(OMV4_ER_JR_LCD_ECR)*/
	//#define COUNTRY_REPUBLIC_OF_SOUTH_AFRICA //남아공 기능이 Dislog6와 String Mode 영공 기능에 합병 - 남아공사양 사용 안함-> 남아공 요청으로 ECR 버전에 DLP/DEP 추가
	//#define COUNTRY_TURKEY
    //#define COUNTRY_FRANCE

	/** @brief Compiler option : use IAR */
    #define IAR
	/** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
	/** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
	/** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU

	/** @brief Software Version(ER_LCD)*/
	#define VERSION_NUM 410
	/** @brief Software Version(RUSSIA, ER_LCD)*/
	#define VERSION_NUM_RUSSIA 212
	/** @brief Software Version(SOUTH AFRICA, ER_LCD)*/
	#define VERSION_NUM_SOUTH_AFRICA 211
	/** @brief Software Revision*/
	#define REVISION_NUM 2305

	/** @brief Model Edition view */
	#define EDITION_STR " ECR"

	/** @brief USE ER's Program mode*/
	#define USE_PGM_EB
	/** @brief Don't USE Count mode*/
	#define NO_USE_COUNT
	/** @brief USE Standard communication API */
	#define USE_ECR_COMM_API
	/** @brief USE price computing sale mode */
	#define USE_SALE_PRICE
	/** @brief USE Numeric editor */
	#define USE_NUMERIC_EDITOR
	/** @brief USE 6 digit total price window */
	#define USE_TOTAL_6DIGIT
	/** @brief USE Change initial zero range setting */
	#define	USE_CHANGE_ZERO_RANGE_SETTING
	/** @brief USE fifteen percent Initial zero range  */
	#define	USE_FIFTEEN_ZERO_RANGE
	/** @brief USE Real STREAM function for ER Jr */
	//#define USE_REAL_STREAM
	/** @brief USE Printer */
	//#define USE_PRINTER
	/** @brief USE DIALOG6 */
	#define DIALOG6 //기존 TYPE 11과 TYPE 12가 중복되어 TYPE 11 제거 TYPE 14 : string mode
	/** @brief USE ANKER PROTOCL TYPE17 */
	#define USE_ECR_ANKER_PROTOCOL
	/** @brief USE Price Auto Clear User set  */
	#define	USE_PRICE_AUTO_CLEAR_SET
	/** @brief USE Tare Auto Clear User set  */
	#define	USE_TARE_AUTO_CLEAR_SET
	/** @brief USE PLU Quick Store Function */
	#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
	/** @brief USE Decimal Point for ER Series */
	//#define USE_DECIMAL_POINT // csh 20120731
	/** @brief Use to dry battery and Pb battery(for TURKEY) */
	#define USE_DRY_AND_PB_BATT_6V
#ifdef USE_DRY_AND_PB_BATT_6V
	/** @brief Use to auto power setting function  */
    #define USE_AUTO_POWER_SETTING
#endif
	/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
	//#define USE_VERSION_STR_DOT		// 버전표시 : V4.xx.0 (버전이 404이면 V4.04.0 으로 표시)
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
	
#ifdef COUNTRY_TURKEY
    /** @brief USE to turn alwyas On/Off of Backlight when Press backlight key(for TURKEY) */
    #define USE_BACKLIGHT_KEY_ONOFF_ALWAYS
    /** @brief USE User set PLU tare finction for NTEP certification */
    #define USE_USER_SET_PLU_TARE
#endif
#ifdef COUNTRY_REPUBLIC_OF_SOUTH_AFRICA
	/** @brief USE Printer mode in ECR(Use DLP, DEP)  */
	#define USE_PRINTER_IN_ECR
	/** @brief USE Printer */
	#define USE_PRINTER
#endif
#ifdef COUNTRY_FRANCE
    /** @brief Use sending the same weight only once in the ECR protocol */
    #define USE_SEND_SAME_WEIGHT_ONCE
#endif

#endif


#if MODEL_NUM == MODEL_OMV4_ER2_LCD_SATO
	/** @brief Nation Define(OMV4_ER_JR_LCD_ECR)*/

	/** @brief Compiler option : use IAR */
    #define IAR
	/** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
	/** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
	/** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU

	/** @brief Software Version(ER_LCD)*/
	#define VERSION_NUM 413
	/** @brief Software Version(RUSSIA, ER_LCD)*/
	#define VERSION_NUM_RUSSIA 212
	/** @brief Software Version(SOUTH AFRICA, ER_LCD)*/
	#define VERSION_NUM_SOUTH_AFRICA 211
	/** @brief Software Revision*/
	#define REVISION_NUM 2305

	/** @brief Model Edition view */
	#define EDITION_STR "SATO"

	/** @brief USE ER's Program mode*/
	#define USE_PGM_EB
	/** @brief Don't USE Count mode*/
	#define NO_USE_COUNT
	/** @brief USE Standard communication API */
	#define USE_ECR_COMM_API
	/** @brief USE price computing sale mode */
	#define USE_SALE_PRICE
	/** @brief USE Numeric editor */
	#define USE_NUMERIC_EDITOR
	/** @brief USE 6 digit total price window */
	#define USE_TOTAL_6DIGIT
	/** @brief USE Change initial zero range setting */
	#define	USE_CHANGE_ZERO_RANGE_SETTING
	/** @brief USE fifteen percent Initial zero range  */
	#define	USE_FIFTEEN_ZERO_RANGE
	/** @brief USE Real STREAM function for ER Jr */
	//#define USE_REAL_STREAM
	/** @brief USE Printer */
	//#define USE_PRINTER
	/** @brief USE DIALOG6 */
	//#define DIALOG6 //기존 TYPE 11과 TYPE 12가 중복되어 TYPE 11 제거 TYPE 14 : string mode
	/** @brief USE ANKER PROTOCL TYPE17 */
	//#define USE_ECR_ANKER_PROTOCOL
	/** @brief USE Price Auto Clear User set  */
	#define	USE_PRICE_AUTO_CLEAR_SET
	/** @brief USE Tare Auto Clear User set  */
	#define	USE_TARE_AUTO_CLEAR_SET
	/** @brief USE PLU Quick Store Function */
	#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
	/** @brief USE Decimal Point for ER Series */
	//#define USE_DECIMAL_POINT // csh 20120731
	/** @brief Use to dry battery and Pb battery(for TURKEY) */
	#define USE_DRY_AND_PB_BATT_6V
#ifdef USE_DRY_AND_PB_BATT_6V
	/** @brief Use to auto power setting function  */
    #define USE_AUTO_POWER_SETTING
#endif
	/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
	//#define USE_VERSION_STR_DOT		// 버전표시 : V4.xx.0 (버전이 404이면 V4.04.0 으로 표시)
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020

    /** @brief USE SATO Printer protocol */
    #define USE_SATO_PRINTER_PROTOCOL	
    /** @brief PLU not cleared after setting tare */
    #define USE_PLU_AFTER_TARE_SET
    /** @brief Check AD read completed */
    #define USE_AD_READ_COMPLETE_CHECK
#endif


/** @brief Software Version Number (INT16U xx.xx)*/
#if MODEL_NUM == MODEL_OMV4_EB_LCD/** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
    /** @brief Software Version(EB)*/
    #define VERSION_NUM 410
    /** @brief Software Version(RUSSIA, EB)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA, EB)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 2150
    
    /** @brief Nation Define(EB_LCD)*/
    //#define COUNTRY_AUSTRALIA
    //#define COUNTRY_PORTUGAL
    //#define COUNTRY_POLAND
    //#define COUNTRY_GEORGIA
    
    /** @brief USE EB's Program mode*/
    #define USE_PGM_EB
    /** @brief Don't USE Count mode*/
    #define NO_USE_COUNT
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE price computing sale mode */
    #define USE_SALE_PRICE
    /** @brief USE Numeric editor */
    #define USE_NUMERIC_EDITOR
    /** @brief USE 6 digit total price window */
    #define USE_TOTAL_6DIGIT
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Decimal Point for ER Series */
    //#define USE_DECIMAL_POINT // csh 20120731    
	/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
	//#define USE_VERSION_STR_DOT
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
    /** @brief USE CTS MODE */
    //#define USE_CTS_MODE 
#ifdef  USE_CTS_MODE
    /** @brief USE_CTS_TRIANGLE */
    //#define USE_CTS_TRIANGLE
    /** @brief Software Version(EB_CTS)*/
    #define VERSION_NUM 460 //CTS 사용 시 해당 버전으로 적용
    /** @brief Model name string*/
    #define MODEL_NAME_STR  "EB          "   // 공백 부분 스페이스 입력 필요
    /** @brief USE RTC */
    #define USE_RTC
#endif
    
    #ifdef COUNTRY_AUSTRALIA
        /** @brief USE Add key for certification */
        #define USE_ADD_KEY_FOR_CERTIFICATION		  						// dont' use with USE_PESO_PRINT
    #endif
    
    #ifdef COUNTRY_PORTUGAL
        /** @brief USE PESO DEP print(for Portugal) */
        #define USE_PESO_PRINT											// dont' use with USE_ADD_KEY_FOR_CERTIFICATION
    #endif
    
    #ifdef COUNTRY_POLAND
        #define USE_MULTI_LABEL
        #define USE_MULTI_PRINT_FUNC
    #endif

	#ifdef COUNTRY_GEORGIA
	    /** @brief USE Name Size 16 byte */
	    #define USE_PLU_NAME_SIZE_16
	#endif
    
    /** @brief USE User set PLU tare finction for NTEP certification */
    //#define USE_USER_SET_PLU_TARE
    //#define USE_USER_SET_UNIT
    /** @brief USE Contineous send weight data to ECR(for turkey) */
    //#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
    /** @brief USE Gross Key(for turkey) */
    //#define USE_GROSS_KEY
    /** @brief USE Contineous send weight data to ECR(for turkey) */
    //#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
    /** @brief USE Command tare key function(for turkey) */
    //#define USE_COMMAND_TARE_KEY	//turkey ECR company
#endif

#if MODEL_NUM == MODEL_OMV4_DB2_LED_5DIGIT
    //#define COUNTRY_RUSSIA
    //#define COUNTRY_TURKEY
    //#define COUNTRY_NETHERLAND
    
    /** @brief Software Version(DB2_LED)*/
    #define VERSION_NUM 410
    /** @brief Software Version(RUSSIA)*/
    #define VERSION_NUM_RUSSIA 217
    /** @brief Software Version(SOUTH AFRICA)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 1845

    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020

    /** @brief Scale use all weight unit(kg, lb, oz, gram)*/
    #define USE_ALL_WEIGHT_UNIT
    /** @brief Don't USE Count mode*/
    #define NO_USE_COUNT
    /** @brief USE DB's Program mode*/
    #define USE_PGM_DB
    /** @brief USE Count & Compare & Percent mode*/
    //#define USE_COUNT_COMPARE_PERCENT
    /** @brief USE DB2 LED Func for 5 digit display */
    #define USE_DB2_LED_5DIGIT_FUNC
    /** @brief No USE CAL43 parameter edit*/
    #define NO_USE_CAL43
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE bench scale sale mode */
    #define USE_SALE_BENCH
    /** @brief USE Non Numeric editor */
    #define USE_NON_NUMERIC_EDITOR
    /** @brief USE_5DIGIT of weight display */
    #define USE_WEIGHT_5DIGIT
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE RTC */
    #define USE_RTC
    /** @brief USE_DISP_BRIGHT */
    #define USE_DISP_BRIGHT
    /** @brief USE_SUSPEND_MODE */
    #define USE_SUSPEND_MODE
    ///** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    ///** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE

    #ifdef COUNTRY_TURKEY
    /** @brief USE Bluetooth COMM API (DBI-LED) */
    #define	USE_BT_CONNECT
    /**	@brief USE (CAS 22Byte Protocol) Stream mode  */
    #define USE_STREAM_CAS_22BYTE			//Apply Model : DBI(LED) & DP
    #endif

    /** @brief USE Real STREAM function and PC_TARE (for NETHERLAND) */
    #ifdef COUNTRY_NETHERLAND
        #define REAL_STREAM //real stream mode default print setting OFF, KEY, AUT, Real STR
        #define PC_TARE //only use real stream mode
    #endif
#endif

#if MODEL_NUM == MODEL_OMV4_DB2_LCD
	//#define COUNTRY_NETHERLAND
	//#define COUNTRY_GUATEMALA
	//#define COUNTRY_UZBEKISTAN
	
    #define VERSION_NUM 412
	/** @brief Software Version(RUSSIA, PR_LED)*/
	#define VERSION_NUM_RUSSIA 217
	/** @brief Software Version(SOUTH AFRICA, PR_LED)*/
	#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
	/** @brief Software Revision*/
	#define REVISION_NUM 2322

    /** @brief Compiler option : use IAR */
	#define IAR
    /** @brief USE global typedef for 32bit define */
	#define USE_32BIT_GLOBAL_TYPE
	/** @brief USE 32bit packed for iar define */
	#define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
	/** @brief Software Version(DB2_LCD)*/
    /** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
	
	/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
	#define USE_ALL_WEIGHT_UNIT
	/** @brief USE DB's Program mode*/
	#define USE_PGM_DB
	/** @brief USE Count & Compare & Percent mode*/
	#define USE_COUNT_COMPARE_PERCENT
	/** @brief USE Standard communication API */
	#define USE_STD_COMM_API
	/** @brief USE bench scale sale mode */
	#define USE_SALE_BENCH
	/** @brief USE Non Numeric editor */
	#define USE_NON_NUMERIC_EDITOR
	/** @brief USE Printer */
	#define USE_PRINTER
	/** @brief USE RTC */
	#define USE_RTC
	/** @brief USE BAcklight Menu */
	#define USE_BL_MENU
	/** @brief USE Change initial zero range setting */
	#define	USE_CHANGE_ZERO_RANGE_SETTING
	/** @brief USE fifteen percent Initial zero range  */
	#define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Display reset in Display drive procedure*/
    #define USE_DISP_RESET
	/** @brief USE CTS MODE */
	//#define USE_CTS_MODE 
#ifdef USE_CTS_MODE
    /** @brief USE CTS_ONE_DP*/
    #define CTS_ONE_DP
    /** @brief Software Version(DBII_CTS)*/
	#undef VERSION_NUM
    #define VERSION_NUM 461 //CTS 사용 시 해당 버전으로 적용
	/** @brief Model name string*/
	#define MODEL_NAME_STR  "DBII        "   // 공백 부분 스페이스 입력 필요
#endif
	/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
	//#define USE_VERSION_STR_DOT

	#ifdef COUNTRY_NETHERLAND
		/** @brief USE Multiple print function (for NewZealand) */
		#define USE_NETHERLAND_STREAM		// NETHERLAND 전용 STREAM
		//define USE_COMPARE_AUTOTARE		// Auto Tare 기능
	#endif
	
	#ifdef COUNTRY_GUATEMALA
		#define USE_LIBRA_ESPANOLA_UNIT_CHANGE
	#endif
	
	#ifdef COUNTRY_UZBEKISTAN
		#define USE_STREAM_CAS_22BYTE			//Apply Model : DB-2(LCD), DBI(LED), DP
	#endif

	/** @brief Fast display define */
	#define FASTLCDDISPLAY

	#ifdef FASTLCDDISPLAY
		//#define USE_FAST_DISPLAYING_WEIGHT
		/** @brief for Fast update weight display value change ad rate (China)*/
		#define AD_RAW_PROC_RATE	20L
		/** @brief for Fast shock filter for hish speed display weight (China)*/
		#define FAST_BREAK_SHOCK_FILTER
		/** @brief for Fast average filter for hish speed display weight (China)*/
		#define FAST_AVERAGE_IN_LOW_AD
	#endif
#endif	//#if MODEL_NUM == MODEL_NUM MODEL_OMV4_DB2_LCD

#if MODEL_NUM == MODEL_OMV4_PW2_LCD
/** @brief Compiler option : use IAR */
#define IAR
//#define COUNTRY_RUSSIA

/** @brief Software Version(PW II)*/
#define VERSION_NUM 410
/** @brief Software Version(RUSSIA, PW II)*/
#define VERSION_NUM_RUSSIA 212
/** @brief Software Version(SOUTH AFRICA, PW II)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM

/** @brief Software Revision*/
#define REVISION_NUM 2150
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief Filter default value */
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Auto Power Set Change Mode */
#define USE_AUTO_POWER_SET_CHANGE
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR
/** @brief USE M0516LDN MCU */
#define USE_M0516LDN_MCU
/** @brief use check ad vendor: ob1020, xm24l */
#define USE_CHECK_AD_VENDOR //if not select, default is OB1020

/* @brief This macro disable Low-Voltage-Reset function. */
#define USE_DISABLE_LVR


/** @brief USE rtc mode */
//#define USE_RTC
/** @brief USE Unit change key */
//#define USE_HOLD_KEY
/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
//#define USE_VERSION_STR_DOT
#endif

#if MODEL_NUM == MODEL_OMV4_SW2W_LCD
    /** @brief Software Version(SW)*/
    #define VERSION_NUM 405
    /** @brief Software Version(RUSSIA, SW)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA, SW)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 2345
    /** @brief USE Russia certificated version display */
    #define USE_RUSSIA_VERSION_DISPLAY 217
    /** @brief Scale use all weight unit(kg, lb, oz, gram)*/
    #define USE_ALL_WEIGHT_UNIT
    /** @brief USE SW's Program mode*/
    #define USE_PGM_SW
    /** @brief USE Count & Compare mode*/
    #define USE_COUNT_COMPARE
    /** @brief Filter default value*/
    #define AD_FILT_DEFAULT_VALUE 		21
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE simple weighing sale mode */
    #define USE_SALE_SIMPLE
    /** @brief USE Non Numeric editor */
    #define USE_NON_NUMERIC_EDITOR
    /** @brief USE Printer */
    //#define USE_PRINTER
	/** @brief USE Zero tracking range setting */
	#define USE_ZERO_TRACKING_RANGE_SETTING

    /** @brief USE BAcklight Menu */
    #define USE_BL_MENU
    /** @brief USE Long key */
    #define USE_LONG_KEY

    /** @brief USE CAL6 Change Key Type */
    #define USE_CAL6_KEY_TYPE
    /** @brief USE RS232 Option */
    //#define	USE_RS232
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Underload Limit Change (-10Digit) (for Japan) */
    #define USE_UNDERLOAD_10DIGIT
    /** @brief USE Temp compensation */
    //#define USE_TEMP_COMP	//for CANADA
    /** @brief USE Creep compensation */
    //#define USE_CREEP_COMP	//for CANADA
    /** @brief USE 0.1kg capa table */
    //#define USE_UNIT_0_1KG_CAPA_TABLE //for China special
    #ifdef USE_UNIT_0_1KG_CAPA_TABLE
    /** @brief Model Edition view */
    #define EDITION_STR " LC"
    #endif

    /** @brief USE Zero tracking range setting */
    #define USE_ZERO_TRACKING_RANGE_SETTING

	/** @brief Nation Define(SW1SCU_LCD)*/
	//#define COUNTRY_GUATEMALA
	//#define COUNTRY_AUSTRALIA
    
    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
    /** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020

	/** @brief Use to Pb battery */
    //#define USE_PB_BATT_6V
	
	#ifdef USE_PB_BATT_6V
		#define EDITION_STR "PB BAT"
	#endif
    /** @brief AD (in CPU) OPERATING VOLTAGE VALUE */
    #define AD_OPERATING_VOLTAGE_3_3V

    /** @brief for Use Uart1*/
    //#define USE_UART1
    /* @brief This macro disable Low-Voltage-Reset function. */
    #define USE_DISABLE_LVR
    
    #ifdef COUNTRY_GUATEMALA
        #define USE_LIBRA_ESPANOLA_UNIT_CHANGE
    #endif
#endif//#if MODEL_NUM == MODEL_OMV4_SW2W_LCD


#if MODEL_NUM == MODEL_OMV4_SW2W_LED
    /** @brief Nation Define(SWLR)*/
    //#define COUNTRY_RUSSIA
    //#define COUNTRY_CHINA
    //#define COUNTRY_GUATEMALA
    //#define COUNTRY_MYANMAR
    //#define COUNTRY_AUSTRALIA
    
    /** @brief Software Version(SWLR)*/
    #define VERSION_NUM 405
    /** @brief Software Version(RUSSIA, SWLR)*/
    #define VERSION_NUM_RUSSIA 217
    /** @brief Software Version(SOUTH AFRICA, SWLR)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 2345
    /** @brief Scale use all weight unit(kg, lb, oz, gram)*/
    #define USE_ALL_WEIGHT_UNIT
    /** @brief USE SW's Program mode*/
    #define USE_PGM_SW
    /** @brief USE Count & Compare mode*/
    #define USE_COUNT_COMPARE
    /** @brief Filter default value*/
    #define AD_FILT_DEFAULT_VALUE 		21
    /** @brief USE Suspend mode*/
    #define USE_SUSPEND_MODE
    /** @brief USE Standard communication API */
    #define USE_STD_COMM_API
    /** @brief USE simple weighing sale mode */
    #define USE_SALE_SIMPLE
    /** @brief USE Non Numeric editor */
    #define USE_NON_NUMERIC_EDITOR
    /** @brief USE Printer */
    //#define USE_PRINTER

    /** @brief USE Long key */
    #define USE_LONG_KEY

    /** @brief USE CAL6 Change Key Type */
    #define USE_CAL6_KEY_TYPE
    /** @brief USE RS232 Option */
    //#define	USE_RS232
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    
    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
    /** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020

    /** @brief AD (in CPU) OPERATING VOLTAGE VALUE */
    #define AD_OPERATING_VOLTAGE_3_3V
    /** @brief USE Zero tracking range setting */
	#define USE_ZERO_TRACKING_RANGE_SETTING
	
    #ifdef COUNTRY_CHINA
        /** @brief USE Fast update weight display value (China) */
        #define USE_FAST_DISPLAYING_WEIGHT
        /** @brief for Fast update weight display value change ad rate (China)*/
        #define AD_RAW_PROC_RATE	16L
        /** @brief for Fast shock filter for hish speed display weight (China)*/
        #define FAST_BREAK_SHOCK_FILTER
        /** @brief for Fast average filter for hish speed display weight (China)*/
        #define FAST_AVERAGE_IN_LOW_AD
    #endif
    #ifdef COUNTRY_GUATEMALA
        #define USE_LIBRA_ESPANOLA_UNIT_CHANGE
    #endif

    #ifdef COUNTRY_MYANMAR
        /** @brief USE Viss unit */
        #define USE_VISS_UNIT_CHANGE
    #endif
#endif//#if MODEL_NUM == MODEL_OMV4_SW2W_LED

#if MODEL_NUM == MODEL_OMV4_PDN_LED
    /** @brief Nation Define(PDN_LED)*/
    //#define COUNTRY_UNITEDSTATES
	//#define COUNTRY_CANADA
    #define COUNTRY_TURKEY
    //#define USE_CERTIFICATION_FUNCTION

    /** @brief Software Version(PDN)*/
    #define VERSION_NUM 415
    /** @brief Software Version(RUSSIA, PDN)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA, PDN)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 2346
	/** @brief Model Edition view */
	#define EDITION_STR " ECR"

    /** @brief USE PDN's Program mode*/
	#define USE_PGM_PDN

    /** @brief Filter default value*/
    #define AD_FILT_DEFAULT_VALUE 		21
	/** @brief USE PDN ECR communication API */
	#define USE_PDN_ECR_COMM_API
    /** @brief USE simple weighing sale mode */
    #define USE_SALE_SIMPLE
    /** @brief USE Non Numeric editor */
    #define USE_NON_NUMERIC_EDITOR
	/** @brief USE Printer */
    #define USE_PRINTER
	/** @brief USE Simple Scale's DLP Printer mode */
	#define USE_SIMPLE_PRT_SELECT
    /** @brief USE CAL6 Change Key Type */
    #define USE_CAL6_KEY_TYPE
    /** @brief USE RS232 Option */
    #define	USE_RS232
	/**	@brief USE PDN ECR Option */
	#define USE_PDN_ECR
	/**	@brief USE PD-II ECR Option */
	//#define USE_PD2_ECR
	/** @brief USE DIALOG6 */
	#define DIALOG6
    /** @brief USE Change initial zero range setting */
    #define	USE_CHANGE_ZERO_RANGE_SETTING
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE Underload Limit Change (-10Digit) (for Japan) */
    #define USE_UNDERLOAD_10DIGIT

    /* @brief This macro disable Low-Voltage-Reset function. */
    #define USE_DISABLE_LVR
    
    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
	/** @brief use check ad vendor: ob1020, xm24l */
    #define USE_CHECK_AD_VENDOR //if not select, default is OB1020
	/** @brief USE Smart BT protocol */
	#define USE_SMART_BT_PROTOCOL
    /** @brief for Use Uart1*/
    #define USE_UART1
	/**	@brief USE (CAS 22Byte Protocol) Stream mode  */
	#define USE_STREAM_CAS_22BYTE			//Apply Model : DBI(LED) & DP & PB & PDN
	/**	@brief USE SEND PERIOD 100ms Stream mode  */
	#define USE_FSEND_PERIOD_100MS
	/**	@brief USE UART1 for Printer */
	#define USE_UART1_PRINTER
	/** @brief USE 32 kg Capacity*/
	#define USE_32KG_CAPA
	/** @brief USE URAT1 Baudrate Setting*/
	#define USE_UART1_BAUDRATE_SET
	/** @brief NO USE Weight Total*/
	#define NO_USE_PRT_WEIGHT_TOTAL
	/** @brief USE Command tare key function */
	#define USE_COMMAND_TARE_KEY
	/** @brief USE Time SETTING*/
	//#define USE_TIME_SET
	/** @brief USE Creep compensation */
	/** @brief USE Zero tracking range setting */
	//#define USE_ZERO_TRACKING_RANGE_SETTING
#ifdef COUNTRY_UNITEDSTATES
    #define USE_PDN_USA_SETTING
	#define NOT_USE_TARE_KEY
	#undef USE_PRINTER
	#undef USE_STREAM_CAS_22BYTE
#endif
#ifdef COUNTRY_CANADA
	#define USE_PDN_CANADA_SETTING
	#undef USE_PRINTER
	#undef USE_STREAM_CAS_22BYTE
#endif
#ifdef COUNTRY_TURKEY
    #define USE_TURKEY_ECR_15_PROTOCOL     // 터키 요청에 의해 ECR Type#15(실시간 무게 전송)에 줄바꿈 기능 추가
#endif
#ifdef USE_CERTIFICATION_FUNCTION // 온도보상 및 크립보상 Menu On
	/** @brief USE MWII-H Temp compensation */
	#define USE_TEMP_COMP_MWIIH
	#define USE_CREEP_COMP
	/** @brief Scale use CREEP_MENU*/
	#define USE_CREEP_MENU
	/** @brief Scale use CREEP_MENU Time Setting*/
	#define USE_CREEP_TIME
#endif
#endif

#if MODEL_NUM == MODEL_OMV4_ER_LCD_CLERK
/** @brief Software Version(ER_LCD)*/
#define VERSION_NUM 410
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2029

#define COUNTRY_UN
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY

/** @brief Compiler option : use IAR */
#define IAR
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR
/** @brief USE M0516LDN MCU */
#define USE_M0516LDN_MCU
/** @brief use check ad vendor: ob1020, xm24l */
#define USE_CHECK_AD_VENDOR //if not select, default is OB1020
/** @brief USE rtc mode */
#define USE_RTC
/** @brief for Use Uart1*/
//#define USE_UART1
/* @brief This macro disable Low-Voltage-Reset function. */
#define USE_DISABLE_LVR


/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE RTC */
#define USE_CLERK_KEY
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA

#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#endif


#if MODEL_NUM == MODEL_PR_PLUS_LED
/** @brief Compiler option : use STVD */
#define STVD
#include "../STM8L151C8/stm8l15x_conf.h"
/** @brief Nation Define*/
//#define COUNTRY_CHINA		// Function For China market
//#define COUNTRY_NICARAGUA
//#define COUNTRY_SRILANKA

/** @brief Software Version(PR_LED)*/
#define VERSION_NUM 407
/** @brief Software Version(RUSSIA, PR_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, PR_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2397
/** @brief Device Option : dont use ad data shift*/
#define AD_DATA_NOTSHIFT
/** @brief USE default AD SPAN LIMIT */
#define USE_AD_SPAN_LIMIT_PR_PLUS
/** @brief USE ER's Program mode*/
#define USE_PGM_PR_PLUS
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PR
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE 5 digit unit price window */
#define USE_UNITPRICE_5DIGIT
/** @brief USE AC BATT POWER */
#define USE_AC_BATT_POWER
/** @brief USE STM8L151C8 MCU */
#define USE_STM8L151C8_MCU
/** @brief USE STM8L151C8 Buzzer Driver */
#define USE_BUZZER_DRV_STM8L151C8
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for PR PLUS */
#define USE_DECIMAL_POINT // csh 20120731
/** @brief USE Add User setting doublezero,Battled  for PR PLUS */
#define USE_USER_SETTING_FOR_DOUBLEZERO_BATTLED	
// User Setting에서  Decimal 키를 Decimal/NUM00 키 사용 선택 기능 추가됨 
// User Setting에서  BATT  indicator 를 On/Off 사용 가능 
/** @brief UNLOCK_ADD_SUM_PAY_FUNCTION  for PR PLUS */
#define UNLOCK_ADD_SUM_PAY_FUNCTION
// Allow to use ADD, SUM, PAY function C-6 setting on Calibration 
/** @brief USE CAL SPAN BIG LIMIT */
//#define USE_CAL_SPAN_EXT_LIMIT_PR_PLUS	//calibration span 값 limit 변경(not use_)

#ifdef COUNTRY_NICARAGUA
#define USE_UNIT_CHANGE_FUNCTION	// Use 'UNIT CHANGE(KG/LB)' function(M4 -> Kg/Lb)
#endif
#ifdef COUNTRY_CHINA
#define USE_PROTOCOL_FOR_CHINAMARKET	//Protocol for China supermarket
#define USE_LONG_KEY_FOR_CHINAMARKET	//Long key for Sending protocol and setting protocol types.
#define USE_SEND_WEIGHT_DATA			//Protocol for China supermarket
#endif
#ifdef COUNTRY_SRILANKA
#define USE_THREE_ZERO_RANGE	//Use three percent initial zero range based on calibration data
#define USE_SRILANKA_TARE	// Not allow PLU tare, Sucessive Tare
#define USE_SRILANKA_ZERO_RANGE	// ZERO range 3% base on Calibration
#define USE_NOT_UNDERLOAD_ERROR	// Use not underload error display
#define USE_CONTINUE_INIT_ZERO_ERROR // not cleared intit zero range until turn off power.
#else
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
#endif


/****************************************************************/
/* dealer fucntion */
/** @brief USE_PRINTER_FOR_CHINA for PR PLUS */
//#define USE_PRINTER_FOR_CHINA	//SAMPLE Price computing print
/****************************************************************/

#endif				


#if MODEL_NUM == MODEL_OMV3_SWLRWR_LED
/** @brief Compiler option : use IAR */
#define IAR

/** @brief Software Version(SW)*/
#define VERSION_NUM 303
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999
/** @brief Device Option : dont use ad data shift*/
#define AD_DATA_NOTSHIFT
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE OMV3 Program mode*/
#define USE_PGM_SW
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE    // duckspg define 확인해야함 
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE power saving mode*/
#define USE_POWER_SAVING_MODE
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE Change initial zero range setting *///
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE EFM32G840 MCU */
#define USE_EFM32G_MCU
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE

/* duckspg EFM관련 define */

/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21      // 기존 MODEL_SWLRWR_LED는 추가가 안되어있다. 

/** @brief Fast display define */
#define FASTLCDDISPLAY       

#ifdef FASTLCDDISPLAY

#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	20L     
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
#endif
#endif



#if MODEL_NUM == MODEL_OMV3_JIG
/** @brief Compiler option : use IAR */
#define IAR
/** @brief Device Option : dont use ad data shift*/
#define AD_DATA_NOTSHIFT
/** @brief USE EFM32G840 MCU */
#define USE_EFM32G_MCU
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR

/** @brief USE EFM32G840 LITTLEENDIAN MCU */
#define USE_LITTLEENDIAN_MCU

/** @brief Software Version(JIG)*/
#define VERSION_NUM 301
/** @brief Software Version(RUSSIA, JIG)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, JIG)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE JIG communication API */
#define USE_JIG_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE RTC */
#define USE_RTC

#endif


#if MODEL_NUM == MODEL_JIG
/** @brief Software Version(JIG)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, JIG)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, JIG)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1414
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE JIG communication API */
#define USE_JIG_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE RTC */
#define USE_RTC
#endif

#if MODEL_NUM == MODEL_OMV4_JIG
    /** @brief Software Version(JIG)*/
    #define VERSION_NUM 402
    /** @brief Software Version(RUSSIA, JIG)*/
    #define VERSION_NUM_RUSSIA VERSION_NUM
    /** @brief Software Version(SOUTH AFRICA, JIG)*/
    #define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
    /** @brief Software Revision*/
    #define REVISION_NUM 9999
    /** @brief USE SW's Program mode*/
    #define USE_PGM_SW
    /** @brief Don't USE Count mode*/
    #define NO_USE_COUNT
    /** @brief USE JIG communication API */
    #define USE_JIG_COMM_API_OMV4
    /** @brief USE simple weighing sale mode */
    #define USE_SALE_SIMPLE
    /** @brief USE Non Numeric editor */
    #define USE_NON_NUMERIC_EDITOR
    /** @brief USE Printer */
    #define USE_PRINTER
    /** @brief USE fifteen percent Initial zero range  */
    #define	USE_FIFTEEN_ZERO_RANGE
    /** @brief USE RTC */
    #define USE_RTC

    /** @brief Compiler option : use IAR */
    #define IAR
    /** @brief USE M0516LDN MCU */
    #define USE_M0516LDN_MCU
    /** @brief USE global typedef for 32bit define */
    #define USE_32BIT_GLOBAL_TYPE
    /** @brief USE 32bit packed for iar define */
    #define USE_32BIT_PACKED_IAR
    /** @brief Software Version(ER_LED)*/
#endif//#if MODEL_NUM == MODEL_OMV4_JIG

/** @brief Software Version Number (INT16U xx.xx)*/
#if MODEL_NUM == MODEL_EB_LCD
/** @brief Software Version(EB)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, EB)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, EB)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1689

/** @brief Nation Define(EB_LCD)*/
//#define COUNTRY_AUSTRALIA
//#define COUNTRY_PORTUGAL
//#define COUNTRY_POLAND
//#define COUNTRY_KOREA

/** @brief USE EB's Program mode*/
#define USE_PGM_EB
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731

#ifdef COUNTRY_AUSTRALIA
/** @brief USE Add key for certification */
#define USE_ADD_KEY_FOR_CERTIFICATION		  						// dont' use with USE_PESO_PRINT
#endif

#ifdef COUNTRY_PORTUGAL
/** @brief USE PESO DEP print(for Portugal) */
#define USE_PESO_PRINT											// dont' use with USE_ADD_KEY_FOR_CERTIFICATION
#endif

#ifdef COUNTRY_POLAND
#define USE_MULTI_LABEL
#define USE_MULTI_PRINT_FUNC
#endif

#ifdef COUNTRY_KOREA
/**	@brief USE (CAS 22Byte Protocol) Stream mode  */
#define USE_STREAM_CAS_22BYTE // Multi print와 동시에 사용 할 수 없음.
#define USE_EB_STREAM
#endif

/** @brief USE User set PLU tare finction for NTEP certification */
//#define USE_USER_SET_PLU_TARE
//#define USE_USER_SET_UNIT
/** @brief USE Contineous send weight data to ECR(for turkey) */
//#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
/** @brief USE Gross Key(for turkey) */
//#define USE_GROSS_KEY
/** @brief USE Contineous send weight data to ECR(for turkey) */
//#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
/** @brief USE Command tare key function(for turkey) */
//#define USE_COMMAND_TARE_KEY	//turkey ECR company
#endif

/** @brief Software Version Number (INT16U xx.xx)*/
#if MODEL_NUM == MODEL_EB_LCD_7DIGIT
/** @brief Software Version(EB)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, EB)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, EB)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999
/** @brief USE EB's Program mode*/
#define USE_PGM_EB
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 7 digit total price window */
#define USE_TOTAL_7DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
#endif

#if MODEL_NUM == MODEL_SW1SCU_LCD
//#define COUNTRY_GUATEMALA

/** @brief Software Version(SW)*/
#define VERSION_NUM 223
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2483
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Underload Limit Change (-10Digit) (for Japan) */
#define USE_UNDERLOAD_10DIGIT
/** @brief USE Calibration Enter Flag (for Japan) */
#define USE_CAL_ENTER_FLAG
/** @brief USE Display reset in Display drive procedure*/
#define USE_DISP_RESET

/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
//#define USE_VERSION_STR_DOT
/** @brief USE Temp compensation */
//#define USE_TEMP_COMP	//for CANADA
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
/** @brief USE 0.1kg capa table */
//#define USE_UNIT_0_1KG_CAPA_TABLE //for China special
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
/** @brief Model Edition view */
#define EDITION_STR " LC"
#endif
#ifdef COUNTRY_GUATEMALA
#define USE_LIBRA_ESPANOLA_UNIT_CHANGE
#endif
#endif//#if MODEL_NUM == MODEL_SW1SCU_LCD

#if MODEL_NUM == MODEL_SM59R16A5_SW1SCU_LCD
//#define COUNTRY_RUSSIA

/** @brief Software Version(SW)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM

/** @brief Software Revision*/
#define REVISION_NUM 1497
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Underload Limit Change (-10Digit) (for Japan) */
#define USE_UNDERLOAD_10DIGIT
/** @brief USE Temp compensation */
//#define USE_TEMP_COMP	//for CANADA
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
/** @brief USE 0.1kg capa table */
//#define USE_UNIT_0_1KG_CAPA_TABLE //for China special
#ifdef USE_UNIT_0_1KG_CAPA_TABLE
/** @brief Model Edition view */
#define EDITION_STR " LC"
#endif
#endif //#if MODEL_NUM == MODEL_SM59R16A5_SW1SCU_LCD

#if MODEL_NUM == MODEL_SW1SU_LCD_ECR
/** @brief Nation Define(SW1SU_LCD_ECR)*/
//    #define COUNTRY_UNITEDSTATES

/** @brief Software Version(SW)*/
#define VERSION_NUM 223
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2483
/** @brief Model Edition view */
#define EDITION_STR " ECR"
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief USE Count & Compare mode*/
//#define USE_COUNT_COMPARE
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE ECR communication API */
#define USE_ECR_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/**	@brief USE SW ECR Option */
#define USE_WEIGHT_ONLY_ECR
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE DIALOG6 */
#define DIALOG6
/** @brief USE Tare Auto Clear at Zero point  */
#define USE_TARE_AUTO_CLEAR_SET
/** @brief USE Display reset in Display drive procedure*/
#define USE_DISP_RESET

#ifdef COUNTRY_UNITEDSTATES
  /** @brief UNITEDSTATES DEFAULT  */
  #define USE_UNITEDSTATES_DEFAULT
  /** @brief Disable TARE key  */
  #define NOT_USE_TARE_KEY
  /** @brief In'ECR' TYPE(4,5), weight Unit is set to upper case  */
  #define USE_ECR_WEIGHT_UNIT_CAPITAL
#endif
#endif

#if MODEL_NUM == MODEL_OMV3_FW500S_LED

//#define COUNTRY_THAILAND

/** @brief Compiler option : use IAR */
#define IAR

/** @brief Software Version(FW)*/
#define VERSION_NUM 309
/** @brief Software Version(RUSSIA, FW)*/
#define VERSION_NUM_RUSSIA 305
/** @brief Software Version(SOUTH AFRICA, FW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Device Option : dont use ad data shift*/
#define AD_DATA_NOTSHIFT
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE OMV3 Program mode*/
#define USE_PGM_FW
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE    // duckspg define 확인해야함 
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE power saving mode*/
#define USE_POWER_SAVING_MODE
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE default cal gravity korea */
#define USE_DEFAULT_KOREA_GRAVITY
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
//#define USE_PRINTER     //duckspg define 확인해야함  (프린터가 있나?)
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE HOLD DISPLAY 빠르게 찍힘 사용 영업요청사항 FW500 hold display 빠르게 찍기 */
#define	USE_HOLD_FAST_DISPLAY
/** @brief USE Change initial zero range setting *///
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE EFM32G840 MCU */
#define USE_EFM32G_MCU
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP      // QC test
/** @brief USE cts mode */
//#define USE_CTS_MODE_OLD
/** @brief USE rtc mode */
#define USE_RTC
/** @brief USE autozero mode */
#define AUTO_ZERO

/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21      // 기존 MODEL_SWLRWR_LED는 추가가 안되어있다. 

/** @brief Fast display define */
#define FASTLCDDISPLAY       

#ifdef FASTLCDDISPLAY

#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	10L     // 전원 ON 시 ERROR 1 표시되는 현상 방어. 20L -> 10L 로 수정
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
#endif

#ifdef COUNTRY_THAILAND
  #define USE_PRINTER
/** @brief USE RS232 Option -> OMv3 use 11MHz CLK*/
  #define USE_CLK_11MHZ

  /** @brief USE not Print Weight Unit*/
  #define NO_USE_PRT_WEIGHT_UNIT
  /** @brief USE Print Stable Zero Weight*/
  #define USE_STABLE_ZERO_WEIGHT_PRT
    
  #define PRT_PUT_NUM_FILL_CHAR '0'
  /** @brief Not USE Hold Key */
  #define NO_USE_HOLD_KEY
#endif
#endif


#if MODEL_NUM == MODEL_OMV3_FW500S_LCD_LB           // duckspg lb
/** @brief Compiler option : use IAR */
#define IAR
/** @brief Software Version(FW)*/
#define VERSION_NUM 307
/** @brief Software Version(RUSSIA, FW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, FW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Device Option : dont use ad data shift*/
#define AD_DATA_NOTSHIFT
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE OMV3 Program mode*/
#define USE_PGM_SW
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE power saving mode*/
#define USE_POWER_SAVING_MODE
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE 1/16 oz unit */
#define USE_UNIT_1_16OZ
/** @brief USE UL display */
#define USE_UL_DISPLAY
/** @brief Device Option : use LB oz clear */
#define USE_UNIT_OZ_LCD_CLEAR
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE HOLD DISPLAY 빠르게 찍힘 사용 영업요청사항 FW500 hold display 빠르게 찍기 */
#define	USE_HOLD_FAST_DISPLAY
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP      // QC test
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief USE EFM32G840 MCU */
#define USE_EFM32G_MCU
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR

/** @brief USE Creep compensation */
//#define USE_CREEP_COMP      // QC test
/** @brief USE cts on indicator */
//#define USE_CTS_ON_INDICATOR_OLD
/** @brief USE cts mode */
//#define USE_CTS_MODE_OLD
/** @brief USE rtc mode */
#define USE_RTC

/** @brief Fast display define */
#define FASTLCDDISPLAY       

#ifdef FASTLCDDISPLAY

#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	10L     
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
#endif

#endif



#if MODEL_NUM == MODEL_OMV3_FW500S_LCD
/** @brief Nation Define(FW500_LCD)*/
//#define COUNTRY_THAILAND

/** @brief Compiler option : use IAR */
#define IAR
/** @brief Software Version(FW)*/
#define VERSION_NUM 308
/** @brief Software Version(RUSSIA, FW)*/
#define VERSION_NUM_RUSSIA 304
/** @brief Software Version(SOUTH AFRICA, FW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Device Option : dont use ad data shift*/
#define AD_DATA_NOTSHIFT
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE OMV3 Program mode*/
#define USE_PGM_FW
/** @brief USE power saving mode*/
#define USE_POWER_SAVING_MODE
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Printer */
//#define USE_PRINTER
/** @brief USE default cal gravity korea */
#define USE_DEFAULT_KOREA_GRAVITY
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE HOLD DISPLAY 빠르게 찍힘 사용 영업요청사항 FW500 hold display 빠르게 찍기 */
#define	USE_HOLD_FAST_DISPLAY
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP      // QC test
/** @brief USE EFM32G840 MCU */
#define USE_EFM32G_MCU
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR
/** @brief USE cts on indicator */
//#define USE_CTS_ON_INDICATOR_OLD
/** @brief USE cts mode */
//#define USE_CTS_MODE_OLD
/** @brief USE rtc mode */
#define USE_RTC
/** @brief USE autozero mode */
#define AUTO_ZERO


/** @brief Fast display define */
#define FASTLCDDISPLAY       

#ifdef FASTLCDDISPLAY

#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	10L     
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
#endif

#ifdef COUNTRY_THAILAND
  #define USE_PRINTER
/** @brief USE RS232 Option -> OMv3 use 11MHz CLK*/
  #define USE_CLK_11MHZ

  /** @brief USE not Print Weight Unit*/
  #define NO_USE_PRT_WEIGHT_UNIT
  /** @brief USE Print Stable Zero Weight*/
  #define USE_STABLE_ZERO_WEIGHT_PRT
    
  #define PRT_PUT_NUM_FILL_CHAR '0'
  /** @brief Not USE Hold Key */
  #define NO_USE_HOLD_KEY
#endif

#endif

#if MODEL_NUM == MODEL_OMV3_FW500S_LCD_RS232
/** @brief Compiler option : use IAR */
#define IAR
/** @brief Software Version(FW)*/
#define VERSION_NUM 307
/** @brief Software Version(RUSSIA, FW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, FW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Device Option : dont use ad data shift*/
#define AD_DATA_NOTSHIFT
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE OMV3 Program mode*/
#define USE_PGM_FW
/** @brief USE power saving mode*/
#define USE_POWER_SAVING_MODE
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE default cal gravity korea */
#define USE_DEFAULT_KOREA_GRAVITY
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE RS232 Option -> OMv3 use 11MHz CLK*/
#define USE_CLK_11MHZ
/** @brief USE HOLD DISPLAY 빠르게 찍힘 사용 영업요청사항 FW500 hold display 빠르게 찍기 */
#define	USE_HOLD_FAST_DISPLAY
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP      // QC test
/** @brief USE EFM32G840 MCU */
#define USE_EFM32G_MCU
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR
/** @brief USE cts on indicator */
//#define USE_CTS_ON_INDICATOR_OLD
/** @brief USE cts mode */
//#define USE_CTS_MODE_OLD
/** @brief USE rtc mode */
#define USE_RTC
/** @brief USE autozero mode */
#define AUTO_ZERO
/**	@brief USE (CAS 22Byte Protocol) Stream mode  */
#define USE_STREAM_CAS_22BYTE
/**	@brief USE SEND PERIOD 100ms Stream mode  */
#define USE_FSEND_PERIOD_100MS
/**	@brief TURKET REMOTE DISPLAY TEMP DEFINE mode  */
#define TURKEY_REMOTE_DISP_DEMO
/** @brief Fast display define */
#define FASTLCDDISPLAY

#ifdef FASTLCDDISPLAY

#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	10L
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
#endif

/** @brief Model Edition view */
#define EDITION_STR "RS232"

#endif

#if MODEL_NUM == MODEL_OMV3_SW1SCU_LCD
/** @brief Compiler option : use IAR */
#define IAR
/** @brief Software Version(SW)*/
#define VERSION_NUM 303
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999
/** @brief Device Option : dont use ad data shift*/
#define AD_DATA_NOTSHIFT
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE power saving mode*/
#define USE_POWER_SAVING_MODE
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API

/** @brief USE SW's Program mode*/
#define USE_PGM_SW

/** @brief USE EFM32G840 MCU */
#define USE_EFM32G_MCU
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR

/** @brief Fast display define */
#define FASTLCDDISPLAY       

#ifdef FASTLCDDISPLAY

#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	20L     
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
#endif

#endif

#if MODEL_NUM == MODEL_SWLRWR_LED
/** @brief Nation Define(SWLR)*/
//#define COUNTRY_RUSSIA
//#define COUNTRY_CHINA
//#define COUNTRY_GUATEMALA
//#define COUNTRY_SRILANKA
//#define COUNTRY_INDIA

/** @brief Software Version(SWLR)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SWLR)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA, SWLR)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2738

/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE RS232 Option */
#define	USE_RS232
#ifdef COUNTRY_SRILANKA
/** @brief USE three percent Initial zero range based on Calibration data  */
#define	USE_THREE_ZERO_RANGE
#else
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
#endif

#ifdef COUNTRY_CHINA
/** @brief USE Fast update weight display value (China) */
#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	16L
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
#endif
#ifdef COUNTRY_GUATEMALA
#define USE_LIBRA_ESPANOLA_UNIT_CHANGE
#endif
#ifdef COUNTRY_SRILANKA
//#define USE_SRILANKA_ZERO_RANGE		// ZERO range 3% base on Calibration
#define USE_SRILANKA_TARE			// Not allow PLU tare, Sucessive Tare
#endif
#ifdef COUNTRY_INDIA
  /** @brief USE not Print Weight Unit EXCEPT for K_SEND */
  #define NO_USE_PRT_WEIGHT_UNIT_EXCEPT_FOR_K_SEND   /* NO_USE_PRT_WEIGHT_UNIT define 처럼 무게 단위 전송하지 않으나 K-SEND는 보냄 */
  #define USE_CAL_ENTER
#endif
#endif//#if MODEL_NUM == MODEL_SWLRWR_LED

#if MODEL_NUM == MODEL_SWLRWR_LED_ECR
/** @brief Nation Define(SWLR)*/
//#define COUNTRY_RUSSIA
//#define COUNTRY_CHINA
//#define COUNTRY_GUATEMALA
//#define COUNTRY_SRILANKA

/** @brief Software Version(SWLR)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SWLR)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA, SWLR)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2069
/** @brief Model Edition view */
#define EDITION_STR " ECR"
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE ECR communication API */
#define USE_ECR_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/**	@brief USE Weight Only ECR Option */
#define USE_WEIGHT_ONLY_ECR
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE DIALOG6 */
#define DIALOG6
/** @brief USE RS232 Option */
#define	USE_RS232
#ifdef COUNTRY_SRILANKA
/** @brief USE three percent Initial zero range based on Calibration data  */
#define	USE_THREE_ZERO_RANGE
#else
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
#endif
#ifdef COUNTRY_CHINA
/** @brief USE Fast update weight display value (China) */
#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	16L
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
#endif
#ifdef COUNTRY_GUATEMALA
#define USE_LIBRA_ESPANOLA_UNIT_CHANGE
#endif
#ifdef COUNTRY_SRILANKA
//#define USE_SRILANKA_ZERO_RANGE		// ZERO range 3% base on Calibration
#define USE_SRILANKA_TARE			// Not allow PLU tare, Sucessive Tare
#endif
#endif//#if MODEL_NUM == MODEL_SWLRWR_LED_ECR


#if MODEL_NUM == MODEL_SM59R16A5_SWLRWR_LED
/** @brief Nation Define(SWLR)*/
//#define COUNTRY_RUSSIA
//#define COUNTRY_CHINA

/** @brief Software Version(SWLR)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SWLR)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA, SWLR)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999

/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE

#ifdef COUNTRY_CHINA
/** @brief USE Fast update weight display value (China) */
#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	16L
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
#endif
#endif//#if MODEL_NUM == MODEL_SM59R16A5_SWLRWR_LED

#if MODEL_NUM == MODEL_ER_LED
/** @brief Software Version(ER_LED)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, ER_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, ER_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1626

/** @brief Nation Define(ER_LED)*/
#define COUNTRY_UN
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY
//#define COUNTRY_NEWZEALAND
//#define COUNTRY_GUATEMALA

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731

#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7	//LED, VFD전용 키 사양 (Backlight ON/OFF Key 제외)
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#ifdef COUNTRY_GUATEMALA
#define USE_UNIT_SPENISH_OZ //ER PLUS LED에서 온즈 기능 사용시 단가표시 LED 설정.
#define USE_LIBRA_ESPANOLA_UNIT_CHANGE
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_NEWZEALAND
/** @brief USE Multiple print function (for NewZealand) */
#define USE_MULTI_PRINT_FUNC
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#endif				

#if MODEL_NUM == MODEL_ER_PRO_LED
/** @brief Software Version(ER_LED)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, ER_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, ER_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1663

/** @brief Nation Define(ER_PLUS_M_LCD)*/
#define COUNTRY_UN
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 8 digit total price window */
#define USE_TOTAL_8DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Price Auto Clear User set  */
//#define	USE_PRICE_AUTO_CLEAR_SET	// Code size 오버로 인해 부가 기능 삭제
/** @brief USE Tare Auto Clear User set  */
//#define	USE_TARE_AUTO_CLEAR_SET	// Code size 오버로 인해 부가 기능 삭제
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
/** @brief USE ER_PLUS_M_TURKEY_DEFINE */
//#define USE_ER_PLUS_M_TURKEY_KEY_DEFINE

#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#ifdef COUNTRY_TURKEY	//딜러 롬 기능 주석처리
/** @brief USE Add key & Tare key for continual sale (for Turkey) */
//#define USE_TURKISH_ADD
/** @brief USE Contineous send weight data to ECR(for turkey) */
//#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
/** @brief USE Command tare key function(for turkey) */
//#define USE_COMMAND_TARE_KEY	//turkey ECR company
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7	//LED, VFD전용 키 사양 (Backlight ON/OFF Key 제외)
#endif

#endif

#if MODEL_NUM == MODEL_ER_LCD

/** @brief Nation Define(ER_LCD)*/
#define COUNTRY_UN			//영공사양, Key type을 국거별로 정의하기 위해 정의함
//#define COUNTRY_TURKEY
//#define COUNTRY_AUSTRALIA
//#define COUNTRY_PORTUGAL
//#define COUNTRY_NEWZEALAND
//#define COUNTRY_PERU
//#define COUNTRY_RUSSIA
//#define COUNTRY_GUATEMALA
//#define COUNTRY_GERMANY


/** @brief Software Version(ER_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, ER_LCD)*/
#define VERSION_NUM_RUSSIA 214
/** @brief Software Version(SOUTH AFRICA, ER_LCD)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2150
/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
//#define USE_CITIZEN_PRINTER	                                        //only use citizen printer
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장

#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif 

#ifdef COUNTRY_AUSTRALIA
/** @brief USE Add key for certification */
#define USE_ADD_KEY_FOR_CERTIFICATION							   	// dont' use with USE_PESO_PRINT
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif 

#ifdef COUNTRY_PORTUGAL
/** @brief USE PESO DEP print(for Portugal) */
#define USE_PESO_PRINT											// dont' use with USE_ADD_KEY_FOR_CERTIFICATION
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY	//딜러롬 기능 주석처리
/** @brief USE Add key & Tare key for continual sale (for Turkey) */
//#define USE_TURKISH_ADD
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
#endif

#ifdef COUNTRY_PERU
/** @brief USE Cancel key for Daily Total Print (for Peru) */
#define DAILY_TOTAL_PRINT
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
//#define USE_VERSION_STR_DOT
#endif

/** @brief USE Contineous send weight data to ECR(for turkey) */
//#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
/** @brief USE Command tare key function(for turkey) */
//#define USE_COMMAND_TARE_KEY	//turkey ECR company

#ifdef COUNTRY_NEWZEALAND
/** @brief USE Multiple print function (for NewZealand) */
#define USE_MULTI_PRINT_FUNC
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_GUATEMALA
#define USE_LIBRA_ESPANOLA_UNIT_CHANGE
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#endif

#if MODEL_NUM == MODEL_ER_LCD_ARGOX 								//Connetion Argox Printer
/** @brief Software Version(ER_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, ER_LCD)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, ER_LCD)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999

/** @brief Nation Define(ER_LCD_ARGOX)*/
//#define COUNTRY_CANADA //STANDARD

/** @brief Model Edition view */
#define EDITION_STR "ARGOX"

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE Printer */
#define USE_ARGOX_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
/** @brief USE Add key for certification */
//#define USE_ADD_KEY_FOR_CERTIFICATION							   	// dont' use with USE_PESO_PRINT
/** @brief USE Add key & Tare key for continual sale (for Turkey) */
//#define USE_TURKISH_ADD											
/** @brief USE PESO DEP print(for Portugal) */
//#define USE_PESO_PRINT
#endif

#if MODEL_NUM == MODEL_ER_VFD
/** @brief Software Version(ER_VFD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, ER_VFD)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, ER_VFD)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1390

/** @brief Nation Define(ER_LED)*/
#define COUNTRY_UN
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY
//#define COUNTRY_NEWZEALAND


/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE 5 digit weight window */
#define USE_WEIGHT_5DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731

#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7	//LED, VFD전용 키 사양 (Backlight ON/OFF Key 제외)
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#ifdef COUNTRY_NEWZEALAND
/** @brief USE Multiple print function (for NewZealand) */
#define USE_MULTI_PRINT_FUNC
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#endif

#if MODEL_NUM == MODEL_ER2_LCD

/** @brief Nation Define(ER2_LCD)*/
//#define COUNTRY_UNITEDSTATES
//#define COUNTRY_TURKEY 
//#define COUNTRY_RUSSIA
//#define COUNTRY_AUSTRALIA

/** @brief Software Version(ER2_LCD)*/
#define VERSION_NUM 221
/** @brief Software Version(RUSSIA, ER2_LCD)*/
#define VERSION_NUM_RUSSIA 212
/** @brief Software Version(SOUTH AFRICA, ER2_LCD)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2150
/** @brief USE EB's Program mode*/
#define USE_PGM_EB
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Real STREAM function for ER Jr */
#define USE_REAL_STREAM
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief Use to dry battery and Pb battery(for TURKEY) */
#define USE_DRY_AND_PB_BATT_6V
/** @brief Use to auto power setting function  */
#define USE_AUTO_POWER_SETTING

/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
//#define USE_VERSION_STR_DOT

/** @brief USE Contineous send weight data to ECR(for turkey) */
//#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
/** @brief USE Command tare key function(for turkey) */
//#define USE_COMMAND_TARE_KEY	//turkey ECR company

#ifdef COUNTRY_UNITEDSTATES
/** @brief USE Command USA Key Table(for USA) */
#define USE_KEY_USA	
/** @brief Use to unit change setting (KG/LB/OZ) (for USA) */
#define USE_USER_SET_UNIT
/** @brief USE User set PLU tare finction for NTEP certification */
#define USE_USER_SET_PLU_TARE
#endif

#ifdef COUNTRY_TURKEY
/** @brief USE to turn alwyas On/Off of Backlight when Press backlight key(for TURKEY) */
#define USE_BACKLIGHT_KEY_ONOFF_ALWAYS
/** @brief USE User set PLU tare finction for NTEP certification */
#define USE_USER_SET_PLU_TARE
#endif

#ifdef COUNTRY_RUSSIA
/** @brief USE Decimal Point for ER Series */
#define USE_DECIMAL_POINT // csh 20120731
/** @brief USE Command Russia Key Table(for Russia) */
#define USE_KEY_RUSSIA
/** @brief USE User set PLU tare finction for NTEP certification */
#define USE_USER_SET_PLU_TARE
#endif

#ifdef COUNTRY_AUSTRALIA
/** @brief USE Add key for certification */
#define USE_ADD_KEY_FOR_CERTIFICATION							   	// dont' use with USE_PESO_PRINT
#endif 

#endif

#if MODEL_NUM == MODEL_ER2_LCD_COMPARE
/** @brief Software Version(ER2_LCD_COMPARE)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, ER2_LCD_COMPARE)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, ER2_LCD_COMPARE)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999

/** @brief Nation Define(ER_LCD_ARGOX)*/
//#define COUNTRY_TURKEY //STANDARD

/** @brief Model Edition view */
#define EDITION_STR "COMP"

/** @brief USE EB's Program mode*/
#define USE_PGM_EB
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief Don't USE DLP Printer*/
#define NO_USE_DLP
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_COMPARE
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE key of turkey */
#define USE_ER2_TURKEY_COMP_KEY
/** @brief USE price computing sale mode and compare sale mode*/
#define USE_SALE_PRICE_COMPARE_PLU //plu structure 
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Decimal Point for ER Series *///사용불가 버그 존재 
//#define USE_DECIMAL_POINT // csh 20120731 
#endif


#if MODEL_NUM == MODEL_ER2_LCD_ECR
/** @brief Software Version(ER_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, ER_LCD)*/
#define VERSION_NUM_RUSSIA 212
/** @brief Software Version(SOUTH AFRICA, ER_LCD)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1391

/** @brief Model Edition view */
#define EDITION_STR " ECR"

/** @brief USE ER's Program mode*/
#define USE_PGM_EB
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_ECR_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Real STREAM function for ER Jr */
#define USE_REAL_STREAM
/** @brief USE Printer */
//#define USE_PRINTER
/** @brief USE DIALOG6 */
#define DIALOG6 //기존 TYPE 11과 TYPE 12가 중복되어 TYPE 11 제거 TYPE 14 : string mode
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
/** @brief Use to dry battery and Pb battery(for TURKEY) */
#define USE_DRY_AND_PB_BATT_6V
#endif

#if MODEL_NUM == MODEL_CASTON2_LED
//#define COUNTRY_USA

/** @brief Software Version(CASTON_LED)*/
#define VERSION_NUM 218
/** @brief Software Version(RUSSIA, CASTON_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, CASTON_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM

#ifdef COUNTRY_USA
	#define REAL_VERSION_NUM 100
#else
	#define REAL_VERSION_NUM 227
#endif

/** @brief Software Revision*/
#define REVISION_NUM 1711
#define USE_ALL_WEIGHT_UNIT
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE CASTON's Program mode*/
#define USE_PGM_CASTON
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Bluetooth COMM API (TW100,TWN) */
#define USE_BT_COMM_API
/** @brief USE CAL7 menu -> Weight calibration */
#define USE_CAL7_WEIGHT_CAL
/** @brief USE simple hanging sale mode */
#define USE_SALE_HANGING
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
//#define USE_RTC
/** @brief USE Zero tracking range setting */
#define USE_ZERO_TRACKING_RANGE_SETTING
/** @brief USE weight backup */
#define USE_WEIGHT_BACKUP
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE TWN  */
#define	USE_TWN_FUNCS
/** @brief USE CAL ENTER  */
#define USE_CAL_ENTER
/** @brief USE UNIT KG LB DEC CHANGE CAPA Table */	//For CASTON Special
#define USE_UNIT_KG_LB_DEC_CHANGE_CAPA_TABLE
#endif

#if MODEL_NUM == MODEL_CASTON3_LED
//#define COUNTRY_USA

/** @brief Software Version(CASTON_LED)*/
#define VERSION_NUM 218
/** @brief Software Version(RUSSIA, CASTON_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, CASTON_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM

#ifdef COUNTRY_USA
	#define REAL_VERSION_NUM 100
#else
	#define REAL_VERSION_NUM 227
#endif

/** @brief Software Revision*/
#define REVISION_NUM 1711
#define USE_ALL_WEIGHT_UNIT
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE CASTON's Program mode*/
#define USE_PGM_CASTON
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Bluetooth COMM API (TW100,TWN) */
#define USE_BT_COMM_API
/** @brief USE CAL7 menu -> Weight calibration */
#define USE_CAL7_WEIGHT_CAL
/** @brief USE simple hanging sale mode */
#define USE_SALE_HANGING
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
//#define USE_RTC
/** @brief USE Zero tracking range setting */
#define USE_ZERO_TRACKING_RANGE_SETTING
/** @brief USE weight backup */
#define USE_WEIGHT_BACKUP
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE TWN  */
#define	USE_TWN_FUNCS
/** @brief USE CAL ENTER  */
#define USE_CAL_ENTER
/** @brief USE UNIT KG LB DEC CHANGE CAPA Table */	//For CASTON Special
#define USE_UNIT_KG_LB_DEC_CHANGE_CAPA_TABLE
#endif


#if MODEL_NUM == MODEL_OMV3_TM_LCD

/** @brief Software Version(TM)*/
#define VERSION_NUM 308
/** @brief Software Version(RUSSIA, TM)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, TM)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2358
#define IAR

/** @brief Scale use CREEP_MENU*/
//#define USE_CREEP_MENU
/** @brief Scale use CREEP_COMP*/
//#define USE_CREEP_COMP
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
/** @brief for 7Mhz CPU_clk */
#define USE_CLK_7MHZ
/** @brief for 11Mhz CPU_clk */
//#define USE_CLK_11MHZ
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief Device Option : dont use ad data shift*/
#define AD_DATA_NOTSHIFT
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
//#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE TM300_LCD's Program mode*/
#define USE_PGM_TM
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Bluetooth & XBEE COMM API (TW100,TWN) */
#define USE_TM_COMM_API
/** @brief USE simple hanging sale mode */
#define USE_SALE_HANGING
/** @brief USE EFM32G840 MCU */
#define USE_EFM32G_MCU
/** @brief USE expanded indicator option */
#define USE_EXPANDED_INDICATOR
/** @brief USE weight backup */
#define USE_WEIGHT_BACKUP
/** @brief USE expanded hysteresis compensation option */
//#define USE_EXP_HYST_COMP
/** @brief for Use KeyLock*/
#define USE_KEY_LOCK
/** @brief USE CAL ENTER  */
#define USE_CAL_ENTER

/** @brief Fast display define */
#define FASTLCDDISPLAY       
#ifdef FASTLCDDISPLAY
#define USE_FAST_DISPLAYING_WEIGHT
/** @brief for Fast update weight display value change ad rate (China)*/
#define AD_RAW_PROC_RATE	20L     
/** @brief for Fast shock filter for hish speed display weight (China)*/
#define FAST_BREAK_SHOCK_FILTER
/** @brief for Fast average filter for hish speed display weight (China)*/
#define FAST_AVERAGE_IN_LOW_AD
/** @brief USE EFM32G840 MCU */
#define USE_EFM32G_MCU
/** @brief USE 32bit packed for iar define */
#define USE_32BIT_PACKED_IAR
/** @brief USE global typedef for 32bit define */
#define USE_32BIT_GLOBAL_TYPE
#endif

/** @brief USE TWN  */
#define	USE_TWN_FUNCS
/** @brief for Use PowerOff for TWN Module*/
#define CASTON_POWEROFF
/** @brief for TM Relay*/
#define	USE_RELAY
/** @brief for TM RS-485*/
//#define	USE_RS485
/** @brief for Use Uart1 for Omv3*/
#define USE_UART1 
/** @brief for Use XBEE_CHANNEL_CHANGE for Omv3*/
#define USE_XBEE
/** @brief for TM Unit_UN(short/long ton, nuton, daN, X1000)*/
#define USE_UNIT_TON_NEWTON_DAN_KN
/** @brief for TM Unit_Korea (short/long ton, nuton, daN, X1000)*/
//#define USE_UNIT_KG_ONLY
#endif


#if MODEL_NUM == MODEL_HD_LED
/** @brief Software Version(CASTON_LED)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, CASTON_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, CASTON_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999
#define USE_ALL_WEIGHT_UNIT
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE HD's Program mode*/
#define USE_PGM_HD
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Bluetooth COMM API (TW100,TWN) */
#define USE_BT_COMM_API
/** @brief USE CAL7 menu -> Weight calibration */
#define USE_CAL7_WEIGHT_CAL
/** @brief USE simple hanging sale mode */
#define USE_SALE_HANGING
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
//#define USE_RTC
/** @brief USE Zero tracking range setting */
#define USE_ZERO_TRACKING_RANGE_SETTING
/** @brief USE weight backup */
#define USE_WEIGHT_BACKUP
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE TWN  */
#define	USE_TWN_FUNCS
#endif


#if MODEL_NUM == MODEL_DB2_LCD

/** @brief Nation Define(ER_LCD)*/
//#define COUNTRY_NETHERLAND
//#define COUNTRY_GUATEMALA
//#define COUNTRY_UZBEKISTAN
//#define COUNTRY_KOREA

/** @brief Software Version(DB2_LCD)*/
#define VERSION_NUM 223
//#define VERSION_NUM 110		//DBI OIML 인증버전(V1.1X)
/** @brief Software Version(RUSSIA, PR_LED)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA, PR_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2568

/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE DB's Program mode*/
#define USE_PGM_DB
/** @brief USE Count & Compare & Percent mode*/
#define USE_COUNT_COMPARE_PERCENT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE bench scale sale mode */
#define USE_SALE_BENCH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE BAcklight Menu */
#define USE_BL_MENU
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
//#define USE_VERSION_STR_DOT
/** @brief USE Version Number 3 digits(for Old Certi, not Use New Certi) */
//#define USE_VERSION_NUM_3_DIGIT
/** @brief USE Display reset in Display drive procedure*/
#define USE_DISP_RESET

#ifdef COUNTRY_NETHERLAND
/** @brief USE Multiple print function (for NewZealand) */
#define USE_NETHERLAND_STREAM			// NETHERLAND 전용 STREAM
#endif

#ifdef COUNTRY_GUATEMALA
#define USE_LIBRA_ESPANOLA_UNIT_CHANGE
#endif

#ifdef COUNTRY_UZBEKISTAN
#define USE_STREAM_CAS_22BYTE			//Apply Model : DB-2(LCD), DBI(LED), DP
#endif
#ifdef COUNTRY_KOREA
/** @brief Use Display count unit weight */
#define USE_DISP_COUNT_UNIT_WEIGHT
#endif

#endif

#if MODEL_NUM == MODEL_DB2_LCD_ECR

/** @brief Software Version(DB2_LCD)*/
#define VERSION_NUM 220
//#define VERSION_NUM 110		//DBI OIML 인증버전(V1.1X)
/** @brief Software Version(RUSSIA, PR_LED)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA, PR_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2198
/** @brief Model Edition view */
#define EDITION_STR " ECR"
/** @brief USE ECR communication API */
#define USE_ECR_COMM_API
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE DB's Program mode*/
#define USE_PGM_DB
/** @brief USE Count & Compare & Percent mode*/
#define USE_COUNT_COMPARE_PERCENT
/** @brief USE ECR communication API */
#define USE_ECR_COMM_API
/** @brief USE bench scale sale mode */
#define USE_SALE_BENCH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
//#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE BAcklight Menu */
#define USE_BL_MENU
/**	@brief USE Weight Only ECR Option */
#define USE_WEIGHT_ONLY_ECR
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE DIALOG6 */
#define DIALOG6

#endif

#if MODEL_NUM == MODEL_DDI_LCD
/** @brief Software Version(DB2_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE DB's Program mode*/
#define USE_PGM_DB
/** @brief USE Count & Compare & Percent mode*/
#define USE_COUNT_COMPARE_PERCENT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE bench scale sale mode */
#define USE_SALE_BENCH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE BAcklight Menu */
#define USE_BL_MENU
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
#endif

#if MODEL_NUM == MODEL_DB1H_VFD
/** @brief Software Version(DB1H)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1560
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE DB's Program mode*/
#define USE_PGM_DB1H
/** @brief USE Print mode*/
//define USE_PRINT_KEY
/** @brief For Print mode*/
#define USE_COUNT_COMPARE_PERCENT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE bench scale sale mode */
#define USE_SALE_BENCH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE MOON's Gravity  */
//#define	USE_MOON_GRAVITY
#endif

#if MODEL_NUM == MODEL_DB2_VFD
//#define COUNTRY_RUSSIA
//#define USE_CTS_MODE
#ifdef USE_CTS_MODE
	/** @brief Software Version(DB_CTS)*/
	#define VERSION_NUM 250 //CTS 사용 시 해당 버전으로 적용
	/** @brief USE CTS_ONE_DP*/
	#define CTS_ONE_DP
	/** @brief Model name string*/
	#define MODEL_NAME_STR  "DB-IIF      "   // 공백 부분 스페이스 입력 필요
	#define USE_CTS_OMV2
#else
/** @brief Software Version(DB2)*/
#define VERSION_NUM 220
#endif
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM

/** @brief Software Revision*/
#define REVISION_NUM 2359
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
//#define USE_ALL_WEIGHT_UNIT
/** @brief USE DB's Program mode*/
#define USE_PGM_DB
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE_PERCENT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE bench scale sale mode */
#define USE_SALE_BENCH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE EMS PROTOCOL (for China)*/
//#define USE_EMS_PROTOCOL
/** @brief USE 6 EA BAUDRATE, EMS use 2400 (for China)*/
//#define USE_6_BAUDRATE
/** @brief USE ZERO TRACKING RANGE 2D, EMS use (for China)*/
//#define USE_ZERO_TRACKING_RANGE_2D
#endif

/** @brief Software Version Number (INT16U xx.xx)*/
#if MODEL_NUM == MODEL_EB_PT_LCD
/** @brief Software Version(EB Percent tare ver.)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1321
/** @brief USE EB's Program mode*/
#define USE_PGM_EB
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE Percent tare mode */
#define USE_PERCENT_TARE_MODE
/** @brief USE EB Percent tare ver. sale mode */
#define USE_SALE_EBPT
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
#endif

#if MODEL_NUM == MODEL_PW2_LCD
//#define COUNTRY_RUSSIA

/** @brief Software Version(SW)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA 212
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM

/** @brief Software Revision*/
#define REVISION_NUM 2150
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief Filter default value */
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Auto Power Set Change Mode */
#define USE_AUTO_POWER_SET_CHANGE
/** @brief USE Unit change key */
//#define USE_HOLD_KEY
/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
//#define USE_VERSION_STR_DOT
#endif


#if MODEL_NUM == MODEL_ED_LCD
/** @brief Software Version(DB2_LCD)*/

/** @brief Nation Define(ED_LCD)*/
//#define COUNTRY_UNITEDSTATES
//#define COUNTRY_NETHERLAND
//#define COUNTRY_POLAND
//#define COUNTRY_TURKEY
//#define COUNTRY_HUNGARY

#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2150
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE DB's Program mode*/
#define USE_PGM_DB
/** @brief USE Count & Compare & Percent mode*/
#define USE_COUNT_COMPARE_PERCENT
/** @brief USE bench scale sale mode */
#define USE_SALE_BENCH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE BAcklight Menu */
#define USE_BL_MENU
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Real STREAM function and PC_TARE (for NETHERLAND) */
//#define REAL_STREAM //real stream mode default print setting OFF, KEY, AUT, Real STR
//#define PC_TARE //only use real stream mode
/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
//#define USE_VERSION_STR_DOT

#ifdef COUNTRY_TURKEY
/** @brief USE ED communication API */
#define USE_ED_COMM_API
/** @brief USE Turkey POS Connection Protocol  */
#define USE_TURKEY_POS_PROTOCOL
/** @brief Block the Function Key(for Turkey) */
#define BLOCK_FUNCTION_KEY
#else
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
#endif

#ifdef COUNTRY_UNITEDSTATES
/** @brief NO USE Hold Key(for USA) */
#define NO_USE_HOLD_KEY	
#endif

#ifdef COUNTRY_NETHERLAND
/** @brief Extend the Hold Time(for Netherlands) */
#define USE_EXTEND_HOLD_TIME
#endif

#ifdef COUNTRY_POLAND
/** @brief Block the Function Key(for Polands) */
#define BLOCK_FUNCTION_KEY
#endif

#ifdef COUNTRY_HUNGARY
/** @brief Use Add the Status Flag(for Hungary) */
#define USE_ADD_STATUS_FLAG
#endif

#endif

#if MODEL_NUM == MODEL_ED_LCD_ECR
/** @brief Software Version(ED_LCD_ECR)*/

#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2198
/** @brief Model Edition view */
#define EDITION_STR " ECR"
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE DB's Program mode*/
#define USE_PGM_DB
/** @brief USE Count & Compare & Percent mode*/
#define USE_COUNT_COMPARE_PERCENT
/** @brief USE ECR communication API */
#define USE_ECR_COMM_API
/** @brief USE bench scale sale mode */
#define USE_SALE_BENCH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
//#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE BAcklight Menu */
#define USE_BL_MENU
/**	@brief USE Weight Only ECR Option */
#define USE_WEIGHT_ONLY_ECR
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE DIALOG6 */
#define DIALOG6

#endif

#if MODEL_NUM == MODEL_SW1S_LCD_LB
/** @brief Software Version(SW)*/
#define VERSION_NUM 223
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2483
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief Filter default value */
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE 1/16 oz unit */
#define USE_UNIT_1_16OZ
/** @brief USE Unit change key */
//#define USE_UNITCHANGE_KEY
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE start Unit 1/16 OZ  */
#define	USE_START_UNIT_1_16OZ
/** @brief USE Display reset in Display drive procedure*/
#define USE_DISP_RESET

#endif

#if MODEL_NUM == MODEL_PB_LCD

//#define COUNTRY_THAILAND
//#define COUNTRY_GERMANY
//#define COUNTRY_COSTARICA

/** @brief Software Version(SW)*/
#define VERSION_NUM 225
/** @brief Software Version(RUSSIA, PR_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, PR_LED)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2789
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE PB's Program mode*/
#define USE_PGM_PB  
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
//#define NO_USE_COUNT
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */ 
#define USE_CAL6_KEY_TYPE
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
#define USE_BL_MENU		
/**	@brief USE Not Print for unstable mode */
//#define USE_NOT_UNSTABLE  //for OIML Certification
/** @brief USE Zero tracking range setting */
#define USE_ZERO_TRACKING_RANGE_SETTING
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Tare Auto Clear at Zero point  */
#define USE_TARE_AUTO_CLEAR_SET
/** @brief USE Auto Tare Function  */
#define USE_AUTO_TARE_FUNCTION
/** @brief USE Simple Scale's DLP Printer mode */
#define USE_SIMPLE_PRT_SELECT
/** @brief USE CANADA Ceritfication */
#define USE_KEY_ZERO_FOR_CERTIFICATION
/** @brief USE Calibration Enter Flag (for Japan) */
#define USE_CAL_ENTER_FLAG
/** @brief USE V4 certification version Display, do not use */
//#define USE_V4_CERTIFICATION_DISPLAY
/** @brief USE Display reset in Display drive procedure*/
#define USE_DISP_RESET
/** @brief USE Certification when printing*/
#define USE_PRINT_CERTIFICATION


#ifdef USE_V4_CERTIFICATION_DISPLAY
#undef VERSION_NUM
#define VERSION_NUM 400
#endif

#ifdef COUNTRY_THAILAND
/** @brief USE not Print Weight Unit*/
#define NO_USE_PRT_WEIGHT_UNIT
/** @brief USE Print Stable Zero Weight*/
#define USE_STABLE_ZERO_WEIGHT_PRT
/** @brief USE Display Weight Only When Stable */
#define USE_DISPLAY_WEIGHT_ONLY_WHEN_STABLE

#define PRT_PUT_NUM_FILL_CHAR '0'
/** @brief Not USE Hold Key */
#define NO_USE_HOLD_KEY
#endif
#ifdef COUNTRY_GERMANY
/** @brief USE Print Stable Weight(Without returning to zero) */
#define USE_PRT_STABLE_WEIGHT
#endif
#ifdef COUNTRY_COSTARICA
/** @brief USE Total SUM(Weight & Count) display*/
#define USE_DISPLAY_SUM_TOTAL
#endif
#endif

#if MODEL_NUM == MODEL_PB_LCD_HEALTH

/** @brief Software Version(SW)*/
#define VERSION_NUM 220  
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999
/** @brief Model Edition view(Auto Hold for Japan) */
#define EDITION_STR "HEALTH"
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE PB's Program mode*/
#define USE_PGM_PB  
/** @brief USE Count & Compare mode*/
#define NO_USE_COUNT
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE_HEALTH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */ 
#define USE_CAL6_KEY_TYPE
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
#define USE_BL_MENU		
/**	@brief USE Not Print for unstable mode */
#define USE_NOT_UNSTABLE  //for OIML Certification
/** @brief USE Zero tracking range setting */
#define USE_ZERO_TRACKING_RANGE_SETTING
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE

#endif

#if MODEL_NUM == MODEL_PB_LCD_BLUETOOTH

/** @brief Software Version(SW)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, PR_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1648
/** @brief Model Edition view */
#define EDITION_STR "BLTH"
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE PB's Program mode*/
#define USE_PGM_PB  
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
//#define NO_USE_COUNT
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */ 
#define USE_CAL6_KEY_TYPE
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
#define USE_BL_MENU		
/** @brief USE Zero tracking range setting */
#define USE_ZERO_TRACKING_RANGE_SETTING
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Tare Auto Clear at Zero point  */
#define USE_TARE_AUTO_CLEAR_SET
/** @brief USE Auto Tare Function  */
#define USE_AUTO_TARE_FUNCTION
/** @brief USE Simple Scale's DLP Printer mode */
#define USE_SIMPLE_PRT_SELECT
/** @brief USE Bluetooth COMM API (DBI-LED) */
#define	USE_BT_CONNECT
/**	@brief USE (CAS 22Byte Protocol) Stream mode  */
#define USE_STREAM_CAS_22BYTE			//Apply Model : DBI(LED) & DP & PB
/** @brief USE Command key function(for Korea DP Model) */
#define USE_COMMAND_KEY					//Apply Model : DP,PB
#endif

#if MODEL_NUM == MODEL_DP_LCD

/** @brief Software Version(SW)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, SW)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, SW)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE PB's Program mode*/
#define USE_PGM_PB  
/** @brief USE Count & Compare mode*/
#define NO_USE_COUNT
/** @brief Filter default value*/
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE DP model communication API */
#define USE_DP_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */ 
#define USE_CAL6_KEY_TYPE
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
#define USE_BL_MENU		
/**	@brief USE Not Print for unstable mode */
#define USE_NOT_UNSTABLE  //for OIML Certification
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE Zero tracking range setting */
#define USE_ZERO_TRACKING_RANGE_SETTING
/**	@brief USE (CAS 22Byte Protocol) Stream mode  */
#define USE_STREAM_CAS_22BYTE			//Apply Model : DP 
/** @brief USE weight backup */
#define USE_WEIGHT_BACKUP				//Apply Model : DP
/** @brief USE Command key function(for Korea DP Model) */
#define USE_COMMAND_KEY					//Apply Model : DP (Only use Simple weighing scale)
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
#endif

#if MODEL_NUM == MODEL_ER_LCD_ECR
/** @brief Nation Define(ER_LCD_ECR)*/
//#define COUNTRY_NETHERLAND
//#define COUNTRY_REPUBLIC_OF_SOUTH_AFRICA //남아공 기능이 Dislog6와 String Mode 영공 기능에 합병 - 남아공사양 사용 안함
//#define COUNTRY_MEXICO //멕시코 선언시 ECR type #2,3,4 의 프로토콜이 'I'에서 동작 하도록 수정, RS-232 config(8 databits, parity none, 1 stop bits, 9600bps)
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY
//#define COUNTRY_CZECH
//#define COUNTRY_UN
//#define COUNTRY_SLOVAKIA

/** @brief Model Edition view */
#define EDITION_STR " ECR"

/** @brief Software Version(ER_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA, PR_LED)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA, PR_LED)*/
#define VERSION_NUM_SOUTH_AFRICA 201

/** @brief Software Revision*/
#define REVISION_NUM 2182


/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_ECR_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Printer */
//#define USE_PRINTER
/** @brief USE DIALOG6 */
#define DIALOG6 //기존 TYPE 11과 TYPE 12가 중복되어 TYPE 11 제거 TYPE 14 : string mode
				// DIALOG6 기능 사용 시 USE_PRICE_AUTO_CLEAR_SET, USE_TARE_AUTO_CLEAR_SET, USE_QUICK_STORE_FUNCTION 기능 Disable
/** @brief USE Price Auto Clear User set  */
//#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
//#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
//#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#ifdef COUNTRY_CZECH
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#define USE_2BYPE_CHECKSUM_PROTOCOL
/** @brief Version redefine */
#undef VERSION_NUM
#define VERSION_NUM 420
/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
#define USE_VERSION_STR_DOT
#endif

#ifdef COUNTRY_SLOVAKIA
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#define USE_SLOVAKIA_PROTOCOL
#endif

#endif

#if MODEL_NUM == MODEL_PW2_LCD_LB
/** @brief Software Version(SW)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1413
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief Filter default value */
#define AD_FILT_DEFAULT_VALUE 		21
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE 1/16 oz unit */
#define USE_UNIT_1_16OZ
/** @brief USE Unit change key */
//#define USE_UNITCHANGE_KEY
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
#endif

#if MODEL_NUM == MODEL_ER_MLED
/** @brief Software Version(ER2_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999
/** @brief USE EB's Program mode*/
#define USE_PGM_ER_MLED
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_ER_MLED
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE UNIT PRICE 5DIGIT */
#define PRICE_SIZE 	5
/** @brief USE BATTERY FULL SIGN */
#define USE_BATT_FULL_SIGN
/** @brief Combine  PAY and SUM */
#define USE_KEY_COMBINE_PAY_SUM
/** @brief check connected or not connected Adaptor */
#define USE_POWER_ADAPTOR_CHECK
/** @brief USE RTC */
#define USE_RTC
/** @brief USE China Limit Calibration */
#define USE_CHINA_LIMIT_CAL
/** @brief USE Power Display Switch */
#define USE_POWER_DISPLAY_SW
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
#endif

#if MODEL_NUM == MODEL_IDP_LED
//#define COUNTRY_RUSSIA
//#define COUNTRY_CHINA

/** @brief Software Version(IDP)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA 211
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1512
/** @brief Nation Define(IDP)*/

/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE SW's Program mode*/
#define USE_PGM_SW
/** @brief USE Count & Compare mode*/
#define USE_COUNT_COMPARE
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple weighing sale mode */
#define USE_SALE_SIMPLE
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief Combine  PAY and SUM */
#define USE_KEY_COMBINE_PAY_SUM
/** @brief check connected or not connected Adaptor */
#define USE_POWER_ADAPTOR_CHECK
/** @brief USE RS232 Option */
#define	USE_RS232
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE DS-252 Protocol */
//#define	USE_DS252_PROTOCOL	//Digi DS-252 Model Protocol(Demand Mode) Request from India
/** @brief USE RTC */
//#define USE_RTC
/** @brief USE China Limit Calibration */
//#define USE_CHINA_LIMIT_CAL
/** @brief USE Power Display Switch */
//#define USE_POWER_DISPLAY_SW

#ifdef COUNTRY_CHINA  //디야텐텐(한국의 웨밀리 마트 같은곳)
#define NO_USE_PRT_WEIGHT_UNIT
#define PRT_PUT_NUM_FILL_CHAR '0'
#endif

#endif

#if MODEL_NUM == MODEL_AP_VFD
/** @brief Software Version(AP_VFD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1573
/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU_AP
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 7 digit total price window */
#define USE_TOTAL_7DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC //TO DO : check use or not
/** @brief USE AC POWER */
#define USE_AC_POWER
/** @brief USE 12digit over load */
#define USE_OVER_LOAD_LIMIT_BIT
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
#endif

#if MODEL_NUM == MODEL_AD_VFD
/** @brief Software Version(AD_VFD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1500
/** @brief USE ER's Program mode*/
//#define USE_PGM_AD
#define USE_PGM_DB1H
/** @brief Don't USE Count mode*/
//#define NO_USE_COUNT
#define USE_COUNT_COMPARE_PERCENT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE simple sale mode */
#define USE_SALE_BENCH
/** @brief USE NON Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE CAL6 Change Key Type */
#define USE_CAL6_KEY_TYPE
/** @brief USE AC POWER */
#define USE_AC_POWER
/** @brief USE capaTable for 2.5kg */
#define USE_UNIT_0_1KG_CAPA_TABLE
/** @brief Step 6 Use Baudrate */
#define USE_6_BAUDRATE
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
#endif

#if (MODEL_NUM == MODEL_ER_PRO_LCD) || (MODEL_NUM == MODEL_ER_PRO_6LCD)
/** @brief Software Version(ER_PLUS_M_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1663

/** @brief Nation Define(ER_PLUS_M_LCD)*/
#define COUNTRY_UN
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR

#if MODEL_NUM == MODEL_ER_PRO_LCD
/** @brief USE 8 digit total price window */
#define USE_TOTAL_8DIGIT
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
#else//MODEL_NUM == MODEL_ER_PRO_6LCD
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
#endif

/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
/** @brief USE HT1622 Buzzer Driver */
//#define USE_HT1622_BUZZER_DRV
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Price Auto Clear User set  */
//#define	USE_PRICE_AUTO_CLEAR_SET			// Code size 오버로 인해 부가 기능 삭제 
/** @brief USE Tare Auto Clear User set  */
//#define	USE_TARE_AUTO_CLEAR_SET			// Code size 오버로 인해 부가 기능 삭제 
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE ER_PLUS_M_TURKEY_KEY DEFINE */
//#define USE_ER_PLUS_M_TURKEY_KEY_DEFINE
/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
#define USE_VERSION_STR_DOT


#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY	//딜러롬 기능 주석처리
/** @brief USE Add key & Tare key for continual sale (for Turkey) */
//#define USE_TURKISH_ADD
/** @brief USE Contineous send weight data to ECR(for turkey) */
//#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
/** @brief USE Command tare key function(for turkey) */
//#define USE_COMMAND_TARE_KEY	//turkey ECR company
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#endif

#if (MODEL_NUM == MODEL_ER_PRO_LCD_ECR) || (MODEL_NUM == MODEL_ER_PRO_6LCD_ECR)
/** @brief Software Version(ER_PLUS_M_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2279
/** @brief Model Edition view */
#define EDITION_STR " ECR"

/** @brief Nation Define(ER_PLUS_M_LCD)*/
#define COUNTRY_UN
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_ECR_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR

#if MODEL_NUM == MODEL_ER_PRO_LCD_ECR
/** @brief USE 8 digit total price window */
#define USE_TOTAL_8DIGIT
#else//MODEL_NUM == MODEL_ER_PRO_6LCD_ECR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
#endif
/** @brief USE DIALOG6 */
#define DIALOG6 //기존 TYPE 11과 TYPE 12가 중복되어 TYPE 11 제거 TYPE 14 : string mode
/** @brief 13 14 15 delete  */
#define USE_ECR_12_ONLY	// code size 오버로 13,14,15 기능 삭제(독일만 테스트 롬으로 나감)

/** @brief USE Printer */
//#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
/** @brief USE HT1622 Buzzer Driver */
//#define USE_HT1622_BUZZER_DRV
/** @brief USE Change initial zero range setting */
//#define	USE_CHANGE_ZERO_RANGE_SETTING	//xdata over로 인해 기능 삭제	
/** @brief USE fifteen percent Initial zero range  */
//#define	USE_FIFTEEN_ZERO_RANGE				//xdata over로 인해 기능 삭제	
/** @brief USE Price Auto Clear User set  */
//#define	USE_PRICE_AUTO_CLEAR_SET		// Code size 오버로 인해 부가 기능 삭제
/** @brief USE Tare Auto Clear User set  */
//#define	USE_TARE_AUTO_CLEAR_SET			// Code size 오버로 인해 부가 기능 삭제
/** @brief USE PLU Quick Store Function */
//#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장 (ER-M ECR: Code size overflow)
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731

/** @brief USE Version string with dot (for Old Certi, not Use New Certi) */
//#define USE_VERSION_STR_DOT


#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY	//딜러롬 기능 주석 처리
/** @brief USE Add key & Tare key for continual sale (for Turkey) */
//#define USE_TURKISH_ADD
/** @brief USE Contineous send weight data to ECR(for turkey) */
//#define USE_CONTI_SEND_WEIGHT_DATA	//turkey ECR company
/** @brief USE Command tare key function(for turkey) */
//#define USE_COMMAND_TARE_KEY	//turkey ECR company
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#endif

#if MODEL_NUM == MODEL_DB2_LED_6DIGIT
//#define COUNTRY_RUSSIA

/** @brief Software Version(DB2_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM

/** @brief Software Revision*/
#define REVISION_NUM 9999
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief USE DB's Program mode*/
#define USE_PGM_DB
/** @brief USE Count & Compare & Percent mode*/
#define USE_COUNT_COMPARE_PERCENT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE bench scale sale mode */
#define USE_SALE_BENCH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE_DISP_BRIGHT */
#define USE_DISP_BRIGHT
/** @brief USE_SUSPEND_MODE */
#define USE_SUSPEND_MODE
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
#endif

#if MODEL_NUM == MODEL_DB2_LED_5DIGIT
//#define COUNTRY_RUSSIA
//#define COUNTRY_TURKEY
//#define COUNTRY_NETHERLAND
//#define COUNTRY_CHINA

/** @brief Software Version(DB2_LCD)*/
#define VERSION_NUM 221
//#define VERSION_NUM 110		//DBI OIML 인증버전(V1.1X)
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA 217
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM

/** @brief Software Revision*/
#define REVISION_NUM 2550
/** @brief Nation Define(DB2_LED)*/
//#define COUNTRY_NETHERLAND 
/** @brief Scale use all weight unit(kg, lb, oz, gram)*/
#define USE_ALL_WEIGHT_UNIT
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE DB's Program mode*/
#define USE_PGM_DB
/** @brief USE Count & Compare & Percent mode*/
//#define USE_COUNT_COMPARE_PERCENT
/** @brief USE DB2 LED Func for 5 digit display */
#define USE_DB2_LED_5DIGIT_FUNC
/** @brief No USE CAL43 parameter edit*/
#define NO_USE_CAL43
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE bench scale sale mode */
#define USE_SALE_BENCH
/** @brief USE Non Numeric editor */
#define USE_NON_NUMERIC_EDITOR
/** @brief USE_5DIGIT of weight display */
#define USE_WEIGHT_5DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE_DISP_BRIGHT */
#define USE_DISP_BRIGHT
/** @brief USE_SUSPEND_MODE */
#define USE_SUSPEND_MODE
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE

#ifdef COUNTRY_TURKEY
/** @brief USE Bluetooth COMM API (DBI-LED) */
#define	USE_BT_CONNECT
/**	@brief USE (CAS 22Byte Protocol) Stream mode  */
#define USE_STREAM_CAS_22BYTE			//Apply Model : DBI(LED) & DP
#endif

/** @brief USE Real STREAM function and PC_TARE (for NETHERLAND) */
#ifdef COUNTRY_NETHERLAND
#define REAL_STREAM //real stream mode default print setting OFF, KEY, AUT, Real STR
#define PC_TARE //only use real stream mode
#endif

#ifdef COUNTRY_CHINA
/** @brief DLP, DEP Printer  */
#define USE_DLP_DEP_PRINTER
#endif
#endif

#if MODEL_NUM == MODEL_ER_LCD_CLERK
/** @brief Software Version(ER_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999

#define COUNTRY_UN
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE RTC */
#define USE_CLERK_KEY
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA

#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#endif

#if MODEL_NUM == MODEL_ER_LCD_BARCODE
/** @brief Software Version(ER_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 9999


#define COUNTRY_UN
//#define COUNTRY_TURKEY
//#define COUNTRY_GERMANY

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE RTC */
#define USE_BARCODE_PRINT
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Price Auto Clear User set  */
#define	USE_PRICE_AUTO_CLEAR_SET
/** @brief USE Tare Auto Clear User set  */
#define	USE_TARE_AUTO_CLEAR_SET
/** @brief USE PLU Quick Store Function */
#define USE_QUICK_STORE_FUNCTION	//PLU Direct key를 길게 눌러 단가 저장
/** @brief USE Decimal Point for ER Jr */
//#define USE_DECIMAL_POINT // csh 20120731
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA

/** @brief NO USE Display Test key */
//#define NO_USE_DISPLAY_TEST_KEY
/** @brief NO USE Hold key */
//#define NO_USE_HOLD_KEY
#ifdef COUNTRY_UN
/** @brief USE Direct PLU key type(for UN) */
#define USE_DIRECT_PLUKEY_TYPE_UN
#endif

#ifdef COUNTRY_TURKEY
/** @brief USE Direct PLU key type(for turkey) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M7_LCD	//LCD전용 키 사양 (Backlight ON/OFF Key 포함)
#endif

#ifdef COUNTRY_GERMANY
/** @brief USE Direct PLU key type(for Germany) */
#define USE_DIRECT_PLUKEY_TYPE_M1_M9
#endif

#endif

#if MODEL_NUM == MODEL_S2000JR_LCD
/** @brief Software Version(ER_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2355

/** @brief Nation Define(S2000JR_LCD)*/
#define COUNTRY_UNITEDSTATES //STANDARD
//#define COUNTRY_AUSTRALIA

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731

#ifdef COUNTRY_AUSTRALIA
/** @brief USE Add key for certification */
#define USE_ADD_KEY_FOR_CERTIFICATION
#endif

/** @brief USE User set PLU tare finction for NTEP certification */
#define USE_USER_SET_PLU_TARE
#define USE_USER_SET_UNIT

#ifdef COUNTRY_UNITEDSTATES
/** @brief USE Date Type: 2 (mmddyy) */
#define USE_DATE_TYPE_MMDDYY
/** @brief USE Date Separator '/' instead of '.' */
#define USE_DATE_SEPARATOR_SLASH
/** @brief USE Printer DLP Protocol Variable V15: Tare (Tested: Godex DT2X) */
#define USE_PRT_DLP50_V15_TARE
#endif

#endif

#if (MODEL_NUM == MODEL_S2000JR_M_LCD) || (MODEL_NUM == MODEL_S2000JR_M_6LCD)
/** @brief Software Version(ER_PLUS_LCD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 2449

/** @brief Nation Define(S2000JR_M_LCD)*/
#define COUNTRY_UNITEDSTATES //STANDARD

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR

#if MODEL_NUM == MODEL_S2000JR_M_LCD
/** @brief USE 8 digit total price window */
#define USE_TOTAL_8DIGIT
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
#elif MODEL_NUM == MODEL_S2000JR_M_6LCD
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731
#endif

/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE Creep compensation */
//#define USE_CREEP_COMP	//for CANADA
/** @brief USE HT1622 Buzzer Driver */
//#define USE_HT1622_BUZZER_DRV
/** @brief USE User set PLU tare finction for NTEP certification */
#define USE_USER_SET_PLU_TARE
#define USE_USER_SET_UNIT
/** @brief NO USE EURO conversion key */
#define NO_USE_EURO_KEY
/** @brief USE Change initial zero range setting */
#define USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief NO USE Printer Enable key */
//#define NO_USE_PRT_ENABLE_KEY
/** @brief NO USE Display Test key */
//#define NO_USE_DISPLAY_TEST_KEY
/** @brief NO USE Hold key */
//#define NO_USE_HOLD_KEY

#ifdef COUNTRY_UNITEDSTATES
/** @brief USE Date Type: 2 (mmddyy) */
#define USE_DATE_TYPE_MMDDYY
/** @brief USE Date Separator '/' */
#define USE_DATE_SEPARATOR_SLASH
/** @brief USE Printer DLP Protocol Variable V15: Tare (Tested: Godex DT2X) */
#define USE_PRT_DLP50_V15_TARE
/** @brief USE Back Light Bright Default set 7 */
#define USE_BL_BRIGHT_DEFAULT_7
#endif

#endif

#if MODEL_NUM == MODEL_S2000JR_VFD
/** @brief Software Version(ER_VFD)*/
#define VERSION_NUM 220
/** @brief Software Version(RUSSIA)*/
#define VERSION_NUM_RUSSIA VERSION_NUM
/** @brief Software Version(SOUTH AFRICA)*/
#define VERSION_NUM_SOUTH_AFRICA VERSION_NUM
/** @brief Software Revision*/
#define REVISION_NUM 1348

/** @brief Nation Define*/
#define COUNTRY_UNITEDSTATES //STANDARD
//#define COUNTRY_AUSTRALIA

/** @brief USE ER's Program mode*/
#define USE_PGM_ER
/** @brief Don't USE Count mode*/
#define NO_USE_COUNT
/** @brief USE Suspend mode*/
#define USE_SUSPEND_MODE
/** @brief USE Standard communication API */
#define USE_STD_COMM_API
/** @brief USE price computing sale mode */
#define USE_SALE_PRICE_EXTERNAL_PLU
/** @brief USE Numeric editor */
#define USE_NUMERIC_EDITOR
/** @brief USE 6 digit total price window */
#define USE_TOTAL_6DIGIT
/** @brief USE 5 digit weight window */
#define USE_WEIGHT_5DIGIT
/** @brief USE Printer */
#define USE_PRINTER
/** @brief USE RTC */
#define USE_RTC
/** @brief USE USA KEY */
#define USE_S2000JR_USA_KEY
/** @brief USE Change initial zero range setting */
#define	USE_CHANGE_ZERO_RANGE_SETTING
/** @brief USE fifteen percent Initial zero range  */
#define	USE_FIFTEEN_ZERO_RANGE
/** @brief USE Decimal Point for ER Series */
//#define USE_DECIMAL_POINT // csh 20120731

#ifdef COUNTRY_AUSTRALIA
/** @brief USE Add key for certification */
#define USE_ADD_KEY_FOR_CERTIFICATION
#endif

#ifdef COUNTRY_UNITEDSTATES
/** @brief USE User set PLU tare finction for NTEP certification */
#define USE_USER_SET_PLU_TARE
#define USE_USER_SET_UNIT
#endif

#endif

//Country Code use ISO 3166-1 alpha-2
//ex) Korea is KR
#ifdef COUNTRY_UNITEDSTATES
#define VERSION_STR "US"
#elif defined COUNTRY_NETHERLAND
#define VERSION_STR "NL"
#elif defined COUNTRY_GERMANY
#define VERSION_STR "DE"
#elif defined COUNTRY_TURKEY
#define VERSION_STR "tr"
#elif defined COUNTRY_AUSTRALIA
#define VERSION_STR "AU"
#elif defined COUNTRY_PORTUGAL
#define VERSION_STR "PT"
#elif defined COUNTRY_NEWZEALAND
#define VERSION_STR "NZ"
#elif defined COUNTRY_CANADA
#define VERSION_STR "CA"
#elif defined COUNTRY_POLAND
#define VERSION_STR "PL"
#elif defined COUNTRY_INDIA
#define VERSION_STR "IN"
#elif defined COUNTRY_CHINA
#define VERSION_STR "CN"
#elif defined COUNTRY_KOREA
#define VERSION_STR "KR"
#elif defined COUNTRY_REPUBLIC_OF_SOUTH_AFRICA
#define VERSION_STR "ZA"
#elif defined COUNTRY_GUATEMALA
#define VERSION_STR "GT"
#elif defined COUNTRY_UZBEKISTAN
#define VERSION_STR "UZ"
#elif defined COUNTRY_NICARAGUA
#define VERSION_STR "NI"
#elif defined COUNTRY_INDONESIA
#define VERSION_STR "ID"
#elif defined COUNTRY_THAILAND
#define VERSION_STR "TH"
#elif defined COUNTRY_RUSSIA
#define VERSION_STR "v "	//Russia 인증 버전은 버전표시 동일해야함
#elif defined COUNTRY_SRILANKA
#define VERSION_STR "LK"	//Russia 인증 버전은 버전표시 동일해야함
#elif defined COUNTRY_GEORGIA
#define VERSION_STR "GE" 
#elif defined COUNTRY_HUNGARY
#define VERSION_STR "HU" 
#elif defined COUNTRY_MYANMAR
#define VERSION_STR "MM"
#elif defined COUNTRY_FRANCE
#define VERSION_STR "FR"
#elif defined COUNTRY_CZECH
#define VERSION_STR "CZ"
#elif defined COUNTRY_COSTARICA
#define VERSION_STR "CR"
#elif defined COUNTRY_SPAIN
#define VERSION_STR "ES"
#else
#define VERSION_STR "v "
#endif

/** @brief country type number (인증, 나라, 딜러 종류별) */
/** @brief OIML국가세팅*/
#define OIML			0
/** @brief NTEP 국가세팅*/
#define NTEP			1
/** @brief KOREA 국가세팅*/
#define KOREA		2
#ifdef USE_WEIGHT_ONLY_ECR
/** @brief AUSTRALIA 국가세팅*/
#define AUSTRALIA		3
#endif
/** @brief JAPAN 국가세팅*/
#define JAPAN		4
/** @brief RUSSIA 국가세팅*/
#define RUSSIA		5
/** @brief RUSSIA 국가세팅*/
#define TURKEY		6
/** @brief SOUTH AFRICA 국가세팅*/
#define SOUTH_AFRICA	7
/** @brief GUATEMALA 국가세팅*/
#define GUATEMALA	8	//cal 11에서 국가 세팅을 8번으로 설정해야 과테말라 국가표시 적용
/** @brief Dafault Key seting*/
#define DEFAULT_KEY_SET 	98
/** @brief Dafault AD seting*/
#define DEFAULT_AD_SET 	99


/** @brief 8051 compiler code */
#if defined(STVD)
#include "../STM8L151C8/stm8l15x_conf.h"
#define code
#define data
#define idata
#elif defined(IAR)
#define code //const
#define data
#define idata
#elif defined(KEIL)
#endif


//#define NEARDATA	_near
//#define ROMDATA	_rom
//#define HUGEDATA	_huge

/** @brief ON*/
#define ON		1
/** @brief OFF*/
#define OFF		0
/** @brief IDLE*/
#define IDLE		2

/** @brief DISABLE*/
#define	DISABLE		0
/** @brief ENABLE*/
#define	ENABLE		1

#ifdef USE_TURKEY_POS_PROTOCOL
/** @brief NOT GOOD*/
#define NG		0
/** @brief GOOD(OKAY)*/
#define OK		1
#endif

#if !defined(TRUE)
/** @brief TRUE*/
#define	TRUE	(~0)
/** @brief FALSE*/
#define	FALSE	0
#endif

/** @brief Parity type NONE*/
#define PARITY_TYPE_NONE		2
/** @brief Parity type ODD*/
#define PARITY_TYPE_ODD		1
/** @brief Parity type EVEN*/
#define PARITY_TYPE_EVEN		0

#ifndef NULL
/** @brief NULL for communication*/
#define NULL	0x00	
#endif
/** @brief SOH for communication*/
#define SOH		0x01
/** @brief STX for communication*/
#define STX		0x02
/** @brief ETX for communication*/
#define ETX	   	0x03
/** @brief EOT for communication*/
#define EOT		0x04
/** @brief ENQ for communication*/
#define ENQ		0x05
/** @brief ACK for communication*/
#define ACK		0x06
/** @brief BEL for communication*/
#define BEL		0x07
/** @brief BS for communication*/
#define BS 		0x08
/** @brief HT for communication*/
#define HT 	   	0x09
/** @brief LF for communication*/
#define LF 		0x0A
/** @brief VT for communication*/
#define VT 		0x0B
/** @brief FF for communication*/
#define FF 		0x0C
/** @brief CR for communication*/
//#define CR 		0x0D //CR = define sfr in keil
#define ASCII_CR 	0x0D
/** @brief SO for communication*/
#define SO		0x0E
/** @brief SI for communication*/
#define SI		0x0F
/** @brief DLE for communication*/
#define DLE		0x10
/** @brief DC1 for communication*/
#define DC1		0x11
/** @brief DC2 for communication*/
#define DC2		0x12
/** @brief DC3 for communication*/
#define DC3		0x13
/** @brief DC4 for communication*/
#define DC4		0x14
/** @brief NAK for communication*/
#define NAK		0x15
/** @brief SYN for communication*/
#define SYN		0x16
/** @brief ETB for communication*/
#define ETB		0x17
/** @brief CAN for communication*/
#define CAN		0x18
/** @brief EM for communication*/
#define EM		0x19
/** @brief SUB for communication*/
#define SUB		0x1A
/** @brief ESC for communication*/
#define ESC		0x1B
/** @brief FC for communication*/
#define FC 		0x1C
/** @brief GS for communication*/
#define GS 		0x1D
/** @brief RS for communication*/
#define RS 		0x1E
/** @brief US for communication*/
#define US 		0x1F
/** @brief DEL for communication*/
#define DEL 		0x7F

//TODO : 모델 마다 다름, help도 고려
/** @brief Initial zero error number*/
#define ERROR_NUM_UNSTABLE				0
/** @brief Unstable error number*/
#define ERROR_NUM_INIT_ZERO				1
/** @brief Unstable error number*/
//#define ERROR_PAY_LACK					2
/** @brief Unstable error number*/
#define ERROR_NUM_OVERLOAD				3
/** @brief No weight error number*/
#define ERROR_NUM_LACK_PAYMENT			4
/** @brief Overload error number*/
#define ERROR_NUM_ALREADY_TARE			5
/** @brief Calibration range error number*/
#define ERROR_NUM_TOTAL_ADDPRICE_OVER	6
/** @brief Calibration range error number*/
//#define ERROR_NUM_TOTAL_PRICE_OVER		7
/** @brief Initial zero error number*/
#define ERROR_NUM_CHANGE_PRICE_OVER		8
/** @brief Unstable error number*/
#define ERROR_NUM_NO_WEIGHT				9
/** @brief Unstable error number*/
//#define ERROR_PAY_LACK					10
/** @brief Unstable error number*/
//#define ERROR_ALREADY_TARE				11
/** @brief No weight error number*/
//#define ERROR_NUM_NO_WEIGHT				12
/** @brief Overload error number*/
#define ERROR_NUM_KEYCODE					13
/** @brief Calibration range error number*/
#define ERROR_NUM_CAL_RANGE 				14
/** @brief Calibration range error number*/
#define ERROR_NUM_HALF_QUARTER_OVER 		15

/** @brief help error number*/
//#define HELP_NUM_							1
/** @brief help error number*/
//#define HELP_NUM_						 	2
/** @brief Total price over help error number*/
//#define HELP_NUM_TOTAL_PRICE_OVER			3
/** @brief Change price is over help error number*/
//#define HELP_NUM_CHANGE_PRICE_OVER		4
/** @brief PLU tare is not correct help error number*/
//#define HELP_NUM_PLU_TARE_NOT_CORRECT	5

#ifdef USE_32BIT_PACKED_IAR
#define _PACKED_DEF   __packed        // 2011/08/09  KDH  onemoduleV3에서는 무조건 추가해야함 
#else
#define _PACKED_DEF
#endif

#ifdef USE_SEND_SAME_WEIGHT_ONCE
  #define DIFFERENCE_WEIGHT     0
  #define SAME_WEIGHT           1
  #define UNKNOWN_WEIGHT        2
#endif

// Revision 번호 자동추가 사용 시 #define DISPLAY_REVISION_INFO 풀기
#ifdef USE_M0516LDN_MCU // OMV4 사용시 자동추가 기능 사용
  #define DISPLAY_REVISION_INFO
#endif
#ifdef DISPLAY_REVISION_INFO
  #undef REVISION_NUM
  #include "revision_info.h"
  #define REVISION_NUM GetRevision()
#endif


/*
********************************************************************************
*                       		GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
/** @brief Logical data type (TRUE or FALSE)*/
#ifdef USE_32BIT_GLOBAL_TYPE
#include <stdbool.h>
typedef bool BOOLEAN;
/** @brief Unsigned 8 bit value*/
typedef unsigned char INT8U;
/** @brief Signed 8 bit value*/
typedef signed char INT8S;
/** @brief Unsigned 16 bit value*/
typedef unsigned short int INT16U;
/** @brief Signed 16 bit value*/
typedef signed short int INT16S;
/** @brief Unsigned 32 bit value*/
typedef unsigned int INT32U;
/** @brief Signed 32 bit value*/
typedef signed int INT32S;
/** @brief Unsigned 64 bit value (if available)*/
//typedef unsigned xxxxx INT64U;
/** @brief Signed 64 bit value (if available)*/
//typedef signed xxxxx INT64S;
/** @brief 32 bit, single prec. floating-point*/
typedef float FP32;
/** @brief 64 bit, double prec. floating-point*/
typedef double FP64;
#else
#ifdef STVD
typedef bool BOOLEAN;
#else
typedef bit BOOLEAN;
#endif
/** @brief Unsigned 8 bit value*/
typedef unsigned char INT8U;
/** @brief Signed 8 bit value*/
typedef signed char INT8S;
/** @brief Unsigned 16 bit value*/
typedef unsigned short INT16U;
/** @brief Signed 16 bit value*/
typedef signed short INT16S;
/** @brief Unsigned 32 bit value*/
typedef unsigned long INT32U;
/** @brief Signed 32 bit value*/
typedef signed long INT32S;
/** @brief Unsigned 64 bit value (if available)*/
//typedef unsigned xxxxx INT64U;
/** @brief Signed 64 bit value (if available)*/
//typedef signed xxxxx INT64S;
/** @brief 32 bit, single prec. floating-point*/
typedef float FP32;
/** @brief 64 bit, double prec. floating-point*/
typedef double FP64;
#endif

/** @brief INT32U Union data structure*/
typedef union{
	/** @brief INT32U Union data structure*/
	INT32U dword;
	/** @brief INT32U --> 4 byte Union data structure*/
	struct{
		/** @brief INT32U --> 4 byte Union data structure*/
		INT8U byte[4]; // byte[0] = 0xff, byte[1] = 0xee, byte[2] = 0xdd, byte[3] = 0xcc - 0xffeeddcc
	}byteArray;
	/** @brief INT32U --> 2 word Union data structure*/
	struct{
		/** @brief INT32U --> 2 word(word[0], word[1]) Union data structure*/
		INT16U word[2];
	}wordArray;
	/** @brief INT32U --> 2 word(high, low) Union data structure*/
	struct{
		/** @brief INT32U중에 상위 워드 (Big endian) */
		INT16U high;
		/** @brief INT32U중에 하위 워드 (Big endian) */
		INT16U low;
	}word;
} UNION_INT32U;

/** @brief INT16U 사이즈의 변수를 상하위 바이트로 나누어 쓸때 사용하는 UNION */
typedef union{
	/** @brief INT16U 사이즈의 변수전체를 쓸때 사용*/
	INT16U word;
	/** @brief byte[0]:상위 바이트, byte[1]:하위 바이트 (Big endian)*/
	struct{
		INT8U byte[2];
	}byteArray;
	/** @brief high:상위 바이트, low:하위 바이트 (Big endian)*/
	struct 
	{
		/** @brief INT16U중에 상위 바이트 (Big endian) */
		INT8U	high;
		/** @brief INT16U중에 하위 바이트 (Big endian) */		
		INT8U	low;
	} byte;
} UNION_INT16U;

/** @brief version Struct */
typedef _PACKED_DEF struct
{
	/** @brief model number */
	INT16U modelNum;
	/** @brief country number */
	INT8U countryNum;
	/** @brief version number */
	INT16U verNum;
	/** @brief version number */
	INT8U fwErasePwd;
	/** @brief 예약 영역 */
	INT8U reserved[2];
} VERSION;	 //8 bytes

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

#endif /* _GLOBALS_H */
