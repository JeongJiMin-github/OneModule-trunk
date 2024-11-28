/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   UI define File\n
* @file    UiDefine.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _UI_DEFINE_H
#define _UI_DEFINE_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
///////////////////////Key Define///////////////////////////

/** 
* @brief Key Mode : Max Key Function Table Number
* @remark Key Mode는 물리적으로 같은 Key를 눌렀을 경우\n
*         모드에 따라 다른 key function code를 반환하여 처리하기 위함
*/
#define KEY_MODE_MAX_NUM	4
/** @brief Key Mode : Normal -> use Sale, Program, Calibration Mode */
#define KEY_MODE_NORMAL		0
/** @brief Key Mode : Alphabet Mode */
#define KEY_MODE_ALPHABET	1
/** @brief Key Mode : Hexa Mode */
#define KEY_MODE_HEX		2
/** @brief Key Mode : Key Type Set Mode */
#define KEY_MODE_KEY_TYPE	3


/** @brief KEY_NUM_0*/
#define KEY_NUM_0	0
/** @brief KEY_NUM_1*/
#define KEY_NUM_1	1
/** @brief KEY_NUM_2*/
#define KEY_NUM_2	2
/** @brief KEY_NUM_3*/
#define KEY_NUM_3	3
/** @brief KEY_NUM_4*/
#define KEY_NUM_4	4
/** @brief KEY_NUM_5*/
#define KEY_NUM_5	5
/** @brief KEY_NUM_6*/
#define KEY_NUM_6	6
/** @brief KEY_NUM_7*/
#define KEY_NUM_7	7
/** @brief KEY_NUM_8*/
#define KEY_NUM_8	8
/** @brief KEY_NUM_9*/
#define KEY_NUM_9	9

   
/** @brief KEY_C, 10*/
#define KEY_C			0x0A
/** @brief KEY_NUM_00, 11*/
#define KEY_NUM_00 		0x0B
/** @brief KEY_ON_OFF 12*/
#define KEY_ON_OFF		0x0C
/** @brief KEY_ZERO, 13*/
#define KEY_ZERO		0x0D
/** @brief KEY_TARE, 14*/
#define KEY_TARE		0x0E
/** @brief KEY_DECIMAL_POINT, 15*/  //csh 20120726
#define KEY_POINT		0x0F
/** @brief KEY_ADD, 16*/
#define KEY_ADD				0x10
/** @brief KEY_FIXED_ADD, 17*/
#define KEY_FIXED_ADD		0x11
/** @brief KEY_SUM, 18*/
#define KEY_SUM				0x12
/** @brief KEY_PAY, 19*/
#define KEY_PAY				0x13
/** @brief KEY_PLU_CALL, 20*/
#define KEY_PLU_CALL		0x14
/** @brief KEY_PLU_SAVE, 21*/
#define KEY_PLU_SAVE		0x15
/** @brief KEY_CANCEL 22*/
#define KEY_CANCEL			0x16
/** @brief KEY_MODE, 23*/
#define KEY_MODE			0x17
/** @brief KEY_HALF, 24*/
#define KEY_HALF			0x18
/** @brief KEY_QUARTER, 25 move --> 61*/
//#define KEY_QUARTER			0x19
/** @brief KEY_PLU_DIRECT_SAVE, 25*/
#define KEY_PLU_DIRECT_SAVE		0x19
/** @brief KEY_PREPACK, 26*/
#define KEY_PREPACK     		0x1A
/** @brief KEY_UNIT_CHANGE, 27*/
#define KEY_UNIT_CHANGE	0x1B
/** @brief KEY_DISPLAY_TEST, 28*/
#define KEY_DISPLAY_TEST	0x1C
/** @brief KEY_HOLD, 29*/
#define KEY_HOLD			0x1D
/** @brief KEY_PRINT_ENABLE, 30*/
#define KEY_PRINT_ENABLE	0x1E
/** @brief KEY_NO_FUNC, 31*/
#define KEY_NO_FUNC		0x1F

//32 ~ 59 구 ER 버전에서 PLU key임
/** @brief KEY_C, 32 move --> 10*/
//#define KEY_C				0x20
/** @brief KEY_ZERO, 33 move --> 13 */
//#define KEY_ZERO			0x21
/** @brief KEY_TARE, 34 move --> 14 */
//#define KEY_TARE			0x22
/** @brief KEY_STAR, 35 move --> 80 */
//#define KEY_STAR			0x23
/** @brief SET KEY, 36 move --> 81 */
//#define KEY_SET				0x24
/** @brief SAMPLE KEY, 37 move --> 82 */
//#define KEY_SAMPLE			0x25
/** @brief PRINT KEY, 38 move --> 83 */
//#define KEY_PRINT			0x26

/** @brief KEY_EURO, 60*/
#define KEY_EURO			0x3C		// 60
/** @brief KEY_QUARTER, 61*/
#define KEY_QUARTER			0x3D
/** @brief KEY_PERCENT_TARE, 62*/
#define KEY_PERCENT_TARE 	0x3E		// 62
/** @brief KEY_TARE_SAVE, 63*/
#define KEY_TARE_SAVE		0x3F		// 63
/** @brief KEY_BOTH_SAVE, 64*/
#define KEY_BOTH_SAVE		0x40		// 64
/** @brief KEY_PERCENT_TARE_SET, 65*/
#define KEY_PERCENT_TARE_SET	0x41	// 65
/** @brief KEY_X, 66*/
#define KEY_X				0x42		// 66
/** @brief 100g KEY, 67 */
#define KEY_HUNDREDGRAM	0x43		// 67

/** @brief KEY_BAT_CHECK, 70*/
#define KEY_BAT_CHECK 		0x46		// 70
/** @brief KEY_BACK_LIGHT, 71*/
#define KEY_BACK_LIGHT		0x47		// 71
/** @brief KEY_GROSS, 72*/
#define KEY_GROSS		0x48		// 72

/** @brief KEY_SEND_PROTOCOL FOR CHINAMARKET, 73*/
#define KEY_SEND_PROTOCOL	0x49		// 73
/** @brief KEY_SET_PROTOCOL FOR CHINAMARKET, 74*/
#define KEY_SET_PROTOCOL	0x4A		// 74
					  
/** @brief KEY_STAR, 80*/
#define KEY_STAR			0x50		// 80
/** @brief SET KEY, 81*/
#define KEY_SET				0x51
/** @brief SAMPLE KEY, 82*/
#define KEY_SAMPLE			0x52
/** @brief PRINT KEY, 83*/
#define KEY_PRINT			0x53

/** @brief CLERK 1 KEY, 84*/
#define KEY_CLERK_1ST		0x54
/** @brief CLERK 2 KEY, 85*/
#define KEY_CLERK_2ND		0x55
/** @brief CLERK 3 KEY, 86*/
#define KEY_CLERK_3RD		0x56
/** @brief CLERK 4 KEY, 87*/
#define KEY_CLERK_4TH		0x57
/** @brief CLERK 5 KEY, 88*/
#define KEY_CLERK_5TH		0x58
/** @brief OPEN CLERK KEY, 89*/
#define KEY_OPEN_CLERK		0x59
/** @brief CLERK 5 KEY, 90*/
#define KEY_SEND_CLERK		0x5A
/** @brief CLERK 5 KEY, 91*/
#define KEY_SEND_BARCODE	0x5B
/** @brief SEND PROTOCOL, 92*/
#define KEY_SEND_SK_PROTOCOL	0x5C


/** @brief KEY_NUM_A*/
#define KEY_NUM_A	0x6A
/** @brief KEY_NUM_B*/
#define KEY_NUM_B	0x6B
/** @brief KEY_NUM_C*/
#define KEY_NUM_C	0x6C
/** @brief KEY_NUM_D*/
#define KEY_NUM_D	0x6D
/** @brief KEY_NUM_E*/
#define KEY_NUM_E	0x6E
/** @brief KEY_NUM_F*/
#define KEY_NUM_F	0x6F
/** @brief KEY_ON_OFF  move --> 12*/
//#define KEY_ON_OFF	0x70

/** @brief KEY_PLU_1*/
#define KEY_PLU_1	0x80
/** @brief KEY_PLU_2*/
#define KEY_PLU_2	0x81
/** @brief KEY_PLU_3*/
#define KEY_PLU_3	0x82
/** @brief KEY_PLU_4*/
#define KEY_PLU_4	0x83
/** @brief KEY_PLU_5*/
#define KEY_PLU_5	0x84
/** @brief KEY_PLU_6*/
#define KEY_PLU_6	0x85
/** @brief KEY_PLU_7*/
#define KEY_PLU_7	0x86
/** @brief KEY_PLU_8*/
#define KEY_PLU_8	0x87
/** @brief KEY_PLU_9*/
#define KEY_PLU_9	0x88
/** @brief KEY_PLU_10*/
#define KEY_PLU_10	0x89
/** @brief KEY_PLU_11*/
#define KEY_PLU_11	0x8A
/** @brief KEY_PLU_12*/
#define KEY_PLU_12	0x8B
/** @brief KEY_PLU_13*/
#define KEY_PLU_13	0x8C
/** @brief KEY_PLU_14*/
#define KEY_PLU_14	0x8D
/** @brief KEY_PLU_15*/
#define KEY_PLU_15	0x8E
/** @brief KEY_PLU_16*/
#define KEY_PLU_16	0x8F
/** @brief KEY_PLU_17*/
#define KEY_PLU_17	0x90
/** @brief KEY_PLU_18*/
#define KEY_PLU_18	0x91
/** @brief KEY_PLU_19*/
#define KEY_PLU_19	0x92
/** @brief KEY_PLU_20*/
#define KEY_PLU_20	0x93
/** @brief KEY_PLU_21*/
#define KEY_PLU_21	0x94
/** @brief KEY_PLU_22*/
#define KEY_PLU_22	0x95
/** @brief KEY_PLU_23*/
#define KEY_PLU_23	0x96
/** @brief KEY_PLU_24*/
#define KEY_PLU_24	0x97
/** @brief KEY_PLU_25*/
#define KEY_PLU_25	0x98
/** @brief KEY_PLU_26*/
#define KEY_PLU_26	0x99
/** @brief KEY_PLU_27*/
#define KEY_PLU_27	0x9A
/** @brief KEY_PLU_28*/
#define KEY_PLU_28	0x9B
/** @brief KEY_PLU_29*/
#define KEY_PLU_29	0x9C
/** @brief KEY_PLU_30*/
#define KEY_PLU_30	0x9D
/** @brief KEY_PLU_31*/
#define KEY_PLU_31	0x9E
/** @brief KEY_PLU_32*/
#define KEY_PLU_32	0x9F
/** @brief KEY_PLU_33*/
#define KEY_PLU_33	0xA0
/** @brief KEY_PLU_34*/
#define KEY_PLU_34	0xA1
/** @brief KEY_PLU_35*/
#define KEY_PLU_35	0xA2
/** @brief KEY_PLU_36*/
#define KEY_PLU_36	0xA3
/** @brief KEY_PLU_37*/
#define KEY_PLU_37	0xA4
/** @brief KEY_PLU_38*/
#define KEY_PLU_38	0xA5
/** @brief KEY_PLU_39*/
#define KEY_PLU_39	0xA6
/** @brief KEY_PLU_40*/
#define KEY_PLU_40	0xA7
/** @brief KEY_PLU_41*/
#define KEY_PLU_41	0xA8
/** @brief KEY_PLU_42*/
#define KEY_PLU_42	0xA9
/** @brief KEY_PLU_43*/
#define KEY_PLU_43	0xAA
/** @brief KEY_PLU_44*/
#define KEY_PLU_44	0xAB
/** @brief KEY_PLU_45*/
#define KEY_PLU_45	0xAC





/** @brief summy Key*/
#define KEY_DUMMY	0xfc
/** @brief CAL Key*/
#define KEY_CAL		0xfd
/** @brief 존재하지 않는 Key*/
#define KEY_NOT_EXISTED 0xfe
/** @brief key가 눌려지지 않음을 나타냄*/
#define KEY_NOT_PRESSED	0xff

/** @brief menu 모드에서 Menu 전환에 사용하는 Key 최대 개수 */
#define KEY_MAX_KEY_NUM_FOR_PGM	9
/** @brief menu 모드 Enter Key를 가져오기 위한 KeyDrvFuncTableForPgm에서의 위치 */
#define KEY_MENU_ENTER_NUM		0
/** @brief menu 모드 C Key를 가져오기 위한 KeyDrvFuncTableForPgm에서의 위치 */
#define KEY_MENU_C_NUM			1
/** @brief menu 모드 Menu Up Key를 가져오기 위한 KeyDrvFuncTableForPgm에서의 위치 */
#define KEY_MENU_UP_NUM			2
/** @brief menu 모드 Menu Down Key를 가져오기 위한 KeyDrvFuncTableForPgm에서의 위치 */
#define KEY_MENU_DOWN_NUM		3
/** @brief menu 모드 INCrease Key를 가져오기 위한 KeyDrvFuncTableForPgm에서의 위치 */
#define KEY_INC_NUM					4
/** @brief menu 모드 ROTate Key를 가져오기 위한 KeyDrvFuncTableForPgm에서의 위치 */
#define KEY_ROT_NUM				5
/** @brief menu 모드 DECrease Key를 가져오기 위한 KeyDrvFuncTableForPgm에서의 위치 */
#define KEY_DEC_NUM				6
/** @brief menu 모드 ROTate Left Key를 가져오기 위한 KeyDrvFuncTableForPgm에서의 위치 */
#define KEY_ROTL_NUM				7
/** @brief menu 모드 Exit Key를 가져오기 위한 KeyDrvFuncTableForPgm에서의 위치 */
#define KEY_EXIT_NUM				8


/** @brief Window 최대 개수 */
#define WIN_MAX_NUM	4
/** @brief Window Number : Weight */
#define WIN_WEIGHT	0
/** @brief Window Number : Unit Price */
#define WIN_UNIT_PRICE	1
/** @brief Window Number : Total Price */
#define WIN_TOTAL_PRICE	2
/** @brief Window Number : Tare */
#define WIN_TARE	3

/** @brief Decimal Point Type : dot */
#define DEC_TYPE_DOT	0
/** @brief Decimal Point Type : comma */
#define DEC_TYPE_COMMA	 1

#define NO_USE_THOUSAND_PT	0
#define USE_THOUSAND_PT		1

///////////////////////Display Define///////////////////////////

/** @brief Indicator 최대 개수 */
#ifdef USE_CTS_MODE     
#ifdef USE_EXPANDED_INDICATOR
#define INDICATOR_MAX_NUM       	57//expanded indicator TM   // cts on 추가
#else
#define INDICATOR_MAX_NUM			36//standard   // cts on 추가 
#endif
#else
#ifdef USE_EXPANDED_INDICATOR
#define INDICATOR_MAX_NUM       	56//expanded indicator TM
#else
#define INDICATOR_MAX_NUM			35//standard
#endif

#endif

/** @brief Incidator Number */
/** @brief INDICATOR_WEIGHT_MINUS */
#define INDICATOR_WEIGHT_MINUS		WIN_WEIGHT
/** @brief INDICATOR_UNIT_PRICE_MINUS*/
#define INDICATOR_UNIT_PRICE_MINUS	WIN_UNIT_PRICE
/** @brief INDICATOR_TOTAL_PRICE_MINUS*/
#define INDICATOR_TOTAL_PRICE_MINUS	WIN_TOTAL_PRICE
/** @brief INDICATOR_WIN_TARE_MINUS*/
#define INDICATOR_WIN_TARE_MINUS	WIN_TARE
/** @brief INDICATOR_ZERO*/
#define INDICATOR_ZERO				4
/** @brief INDICATOR_STABLE*/
#define INDICATOR_STABLE			5
/** @brief INDICATOR_NET*/
#define INDICATOR_NET				6
/** @brief INDICATOR_LOW_BATT*/
#define INDICATOR_LOW_BATT			7
/** @brief INDICATOR_OZ and VISS*/
#define INDICATOR_OZ				8
#define INDICATOR_VISS              8
/** @brief INDICATOR_LB*/
#define INDICATOR_LB				9	
/** @brief INDICATOR_KG*/
#define INDICATOR_KG				10	
/** @brief INDICATOR_GRAM*/
#define INDICATOR_GRAM				11	
/** @brief INDICATOR_PCS*/
#define INDICATOR_PCS				12
/** @brief INDICATOR_PREPACK*/
#define INDICATOR_PREPACK			13
/** @brief INDICATOR_HIGH*/
#define INDICATOR_HIGH				14
/** @brief INDICATOR_LOW*/
#define INDICATOR_LOW				15
/** @brief INDICATOR_OK*/
#define INDICATOR_OK				16
/** @brief INDICATOR_EURO*/
#define INDICATOR_EURO				17
/** @brief INDICATOR_HOLD*/
#define INDICATOR_HOLD   			18
/** @brief INDICATOR_BACKLIGHT*/
#define INDICATOR_BACKLIGHT 		19
/** @brief INDICATOR_LACK*/
#define INDICATOR_LACK 				20
/** @brief INDICATOR_UNITPRICE_100G*/
#define INDICATOR_UNITPRICE_100G	21
/** @brief INDICATOR_TARE_SAVE*/
#define INDICATOR_TARE_SAVE	 		22
/** @brief INDICATOR_BOTH_SAVE*/
#define INDICATOR_BOTH_SAVE	 		23
/** @brief INDICATOR_UNITPRICE_KG*/
#define INDICATOR_UNITPRICE_KG		24
/** @brief INDICATOR_UNITPRICE_LB*/
#define INDICATOR_UNITPRICE_LB		25
/** @brief INDICATOR_PERCENT*/
#define INDICATOR_PERCENT			26
/** @brief INDICATOR_SUM*/
#define INDICATOR_SUM				27
/** @brief INDICATOR_FULL_BATT*/
#define INDICATOR_FULL_BATT			28
/** @brief INDICATOR_CHG_BATT*/
#define INDICATOR_CHG_BATT			29
/** @brief INDICATOR_EURO_KG*/
#define INDICATOR_EURO_KG			30
/** @brief INDICATOR_USD*/
#define INDICATOR_USD				31//for AP
/** @brief INDICATOR_USD_KG*/
#define INDICATOR_USD_KG			32 //for AP
/** @brief INDICATOR_USD_KG*/
#define INDICATOR_UNITPRICE_OZ		33 //for S2000jr
/** @brief INDICATOR_AC*/
#define INDICATOR_AC				34 //for PR_PLUS
/** @brief INDICATOR_CTS*/
#define INDICATOR_CTS				35 //for FW500, SW CTS DISPLAY


//
// reserved for standard indicator option (35~49)
// Insert new indicator define here for standard indicator option(not define USE_EXPANDED_INDICATOR)
// 
/** @brief INDICATOR_TON*/
#define INDICATOR_TON		        50 //for TM
/** @brief INDICATOR_SHORT_TON*/
#define INDICATOR_SHORT_TON	        51 //for TM
/** @brief INDICATOR_LONG_TON*/
#define INDICATOR_LONG_TON		    52 //for TM
/** @brief INDICATOR_NEWTON*/
#define INDICATOR_NEWTON	        53 //for TM
/** @brief INDICATOR_KN*/
#define INDICATOR_KN		        54 //for TM
/** @brief INDICATOR_DAN*/
#define INDICATOR_DAN		        55 //for TM


/** @brief menu 모드 window 최대 개수 */
#define DISP_MAX_WIN_NUM_FOR_MENU	3
/** @brief menu 모드 String window 번호를 가져오기 위한 dispDrvWinTableForMenu에서의 위치*/
#define DISP_MENU_STR_WIN_NUM		0
/** @brief menu 모드 추가 정보 window 번호를 가져오기 위한 dispDrvWinTableForMenu에서의 위치*/
#define DISP_MENU_AUX_WIN_NUM		1
/** @brief menu 모드 Data window 번호를 가져오기 위한 dispDrvWinTableForMenu에서의 위치*/
#define DISP_MENU_DATA_WIN_NUM		2
/** @brief oz mode disable*/
#define OZ_OFF_MODE		0
/** @brief oz mode enble*/
#define OZ_ON_MODE		1
/** @brief oz Decimal Point mode enable*/
#define OZ_DP_MODE		2

/** @brief DLP Barcode type NONE*/
#define TYPE_NONE 						0
/** @brief DLP Barcode type WEIGHT*/
#define TYPE_WEIGHT 					1
/** @brief DLP Barcode type PRICE*/
#define TYPE_PRICE 						2
/** @brief DLP Barcode type NONE*/
#define TYPE_WEIGHT_WITH_BARCODE 	3
/** @brief DLP Barcode type NONE*/
#define TYPE_WEIGHT_COMPARE 			4
/** @brief DLP Barcode type NONE*/
#define TYPE_COUNT_COMPARE 			5

/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
/** @brief Key Function Code Typedef */
typedef INT8U KEY_TYPE;


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


#endif /* _UI_DEFINE_H */
