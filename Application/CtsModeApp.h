/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Cts Mode Applicaion Source File\n
* @file    CtsModeApp.c
* @version 1.0
* @date    2013/01/25
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _CTS_MODE_APP_H
#define _CTS_MODE_APP_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
#ifdef USE_CTS_OMV2
/*OMV2 전용 CTS Addr define, 기존 cts addr에서 엔디안 변환된 값임. 해당 디파인 파생하였기 때문에, 충돌을 방지하여 2byte 이상의 addr 사용을 금함*/
    #define CTS_ADDR_MODEL1         0xC8000000
    #define CTS_ADDR_MODEL2         0xC9000000
    #define CTS_ADDR_MODEL3         0xCA000000
    #define CTS_ADDR_PROGRAM_VER    0xCC000000
    #define CTS_ADDR_SERIALNUM1     0xCD000000
    #define CTS_ADDR_SERIALNUM2     0xCE000000
    #define CTS_ADDR_SERIALNUM3     0xCF000000
    #define CTS_ADDR_SERIALNUM4     0xD0000000
    #define CTS_ADDR_YEAR           0xD2000000
    #define CTS_ADDR_MONTH          0xD3000000
    #define CTS_ADDR_DATE           0xD4000000
    #define CTS_ADDR_ZERO           0xD5000000
    #define CTS_ADDR_MIDUP          0xD6000000
    #define CTS_ADDR_MIDDOWN        0xD7000000
    #define CTS_ADDR_SPAN           0xD8000000
    #define CTS_ADDR_CAPA           0xD9000000
    #define CTS_ADDR_EVALUE         0xDA000000
    #define CTS_ADDR_DVALUE         0xDB000000
    #define CTS_ADDR_WDP            0xDC000000
    #define CTS_ADDR_UNIT           0xDD000000
    #define CTS_ADDR_CALG           0xDE000000
    #define CTS_ADDR_USINGG         0xDF000000
    #define CTS_ADDR_CTSENABLE      0xE2000000
    #define CTS_ADDR_CTSPERIOD      0xE3000000
    #define CTS_ADDR_CAL_STATE      0xE4000000
    #define CTS_ADDR_CTS_VERSION    0xE5000000
    #define CTS_ADDR_PRODUCT_CODE1  0xE6000000
    #define CTS_ADDR_PRODUCT_CODE2  0xE7000000
    #define CTS_ADDR_PRODUCT_CODE3  0xE8000000
    #define CTS_ADDR_PRODUCT_CODE4  0xE9000000
    #define CTS_ADDR_BOARDID        0xEA000000
    #define CTS_ADDR_ENCRYPTKEY     0xEB000000
    #define CTS_ADDR_BCC_ERR		0xEC000000
    #define CTS_ADDR_CTSCMD         0x2C010000
    #define CTS_ADDR_RESET          0x2D010000
    #define CTS_ADDR_CTS_DATA_INIT  0x3B010000
    #define CTS_ADDR_CTS_TIME_SEC  	0x00000000
    #define CTS_ADDR_CTS_TIME_MIN  	0x01000000
    #define CTS_ADDR_CTS_DATA_HOUR  0x02000000
    #define CTS_ADDR_CTS_DATA_DATE  0x03000000
    #define CTS_ADDR_CTS_DATA_MONTH 0x04000000
    #define CTS_ADDR_CTS_DATA_DAY  	0x05000000
    #define CTS_ADDR_CTS_DATA_YEAR  0x06000000
#else
    #define CTS_ADDR_MODEL1         200
    #define CTS_ADDR_MODEL2         201
    #define CTS_ADDR_MODEL3         202
    #define CTS_ADDR_PROGRAM_VER    204
    #define CTS_ADDR_SERIALNUM1     205
    #define CTS_ADDR_SERIALNUM2     206
    #define CTS_ADDR_SERIALNUM3     207
    #define CTS_ADDR_SERIALNUM4     208
    #define CTS_ADDR_YEAR           210
    #define CTS_ADDR_MONTH          211
    #define CTS_ADDR_DATE           212
    #define CTS_ADDR_ZERO           213
    #define CTS_ADDR_MIDUP          214
    #define CTS_ADDR_MIDDOWN        215
    #define CTS_ADDR_SPAN           216
    #define CTS_ADDR_CAPA           217
    #define CTS_ADDR_EVALUE         218
    #define CTS_ADDR_DVALUE         219
    #define CTS_ADDR_WDP            220
    #define CTS_ADDR_UNIT           221
    #define CTS_ADDR_CALG           222
    #define CTS_ADDR_USINGG         223
    #define CTS_ADDR_CTSENABLE      226
    #define CTS_ADDR_CTSPERIOD      227
    #define CTS_ADDR_CAL_STATE      228
    #define CTS_ADDR_CTS_VERSION    229
    #define CTS_ADDR_PRODUCT_CODE1  230
    #define CTS_ADDR_PRODUCT_CODE2  231
    #define CTS_ADDR_PRODUCT_CODE3  232
    #define CTS_ADDR_PRODUCT_CODE4  233
    #define CTS_ADDR_BOARDID        234
    #define CTS_ADDR_ENCRYPTKEY     235
    #define CTS_ADDR_BCC_ERR		236
    #define CTS_ADDR_CTSCMD         300
    #define CTS_ADDR_RESET          301
    #define CTS_ADDR_CTS_DATA_INIT  315
#endif


#define CTS_STRUCT_SIZE    80

/** @brief CTS_STORAGE_OFFSET_ENABLE    */
#define CTS_STORAGE_OFFSET_ENABLE                 0    //55
/** @brief CTS_STORAGE_OFFSET_VERSION     */
#define CTS_STORAGE_OFFSET_VERSION                1    //58 
/** @brief CTS_STORAGE_OFFSET_INIT     */
#define CTS_STORAGE_OFFSET_INIT                   2    //59
/** @brief CTS_STORAGE_OFFSET_EXPIRED     */
#define CTS_STORAGE_OFFSET_EXPIRED                3    //60 reseved 61~78
/** @brief CTS_STORAGE_OFFSET_CALYEAR     */
#define CTS_STORAGE_OFFSET_CALYEAR                4    //52~54
/** @brief CTS_STORAGE_OFFSET_CALMONTH     */
#define CTS_STORAGE_OFFSET_CALMONTH               5    //52~54
/** @brief CTS_STORAGE_OFFSET_CALDATE     */
#define CTS_STORAGE_OFFSET_CALDATE                6    //52~54
/** @brief CTS_STORAGE_OFFSET_PERIOD     */
#define CTS_STORAGE_OFFSET_PERIOD                 8    //56~57
/** @brief CTS_STORAGE_OFFSET_SERIAL_NUM    */
#define CTS_STORAGE_OFFSET_SERIAL_NUM             12    //24~39
/** @brief CTS_STORAGE_OFFSET_PRODUCT_CODE  */
#define CTS_STORAGE_OFFSET_PRODUCT_CODE           28     //0~15
/** @brief CTS_STORAGE_OFFSET_BOADRD_ID   */
#define CTS_STORAGE_OFFSET_BOADRD_ID              44    //16 ~ 19
/** @brief CTS_STORAGE_OFFSET_ENCRYPT_KEY   */
#define CTS_STORAGE_OFFSET_ENCRYPT_KEY            48    //20 ~ 23
/** @brief CTS_STORAGE_OFFSET_CRC    */
#define CTS_STORAGE_OFFSET_BCC                    79    //79~89


#define CTS_OFF					0
#define CTS_ON					1

#define CTS_NO_ERROR			0
#define CTS_ERROR				1

#define CTS_NOT_NEED_UPDATE		0

#define CTS_NOT_EXPIRED			0
#define CTS_EXPIRED				1
#define CTS_NEED_UPDATE			0xFF

#define CTS_DISABLED 			0
#define CTS_ENABLED				1

#define CTS_COMM_CMD_NO_ERROR	0
#define CTS_COMM_CMD_ERROR		2
#define CTS_COMM_WRONG_REQ		1


/*
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
#ifdef USE_CTS_MODE
extern INT8U CtsUpdateScaleData(INT8U id, INT8U *src);
extern INT8U CtsModeInit(INT8U *ctsBuf);
extern INT8U CtsCheckExpired(INT8U *TimeDataPtr);
extern INT8U CtsCheckEnabled(void);
extern INT8U CtsCheckCalEnterEnabled(void);
extern INT8U CtsCommFunc(INT8U rw, INT32U addr, INT32S data32s, INT32S *sendDataPtr);
extern void CtsTimeSync(INT8U *TimeDataPtr);
#ifdef USE_CTS_OMV2
extern void reverse_array(char *src,int size);
#endif
#elif defined (USE_CTS_MODE_OLD)
extern void CtsCalEnter(void);
extern void CtsModeInit(void);
extern void CtsSetModelSerialParam(INT8U type);
extern void CtsSetDefaultParam(INT8U type);
extern void CtsModeProc(void);
extern void CtsModeCalProc(void);
#endif

#endif /* _CTS_MODE_APP_H */