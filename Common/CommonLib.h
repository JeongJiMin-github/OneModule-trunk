/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Common Library Header File\n
           for One Module
* @file    CommmonLib.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _COMMON_LIB_H
#define _COMMON_LIB_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief DecPowerConvTable 크기 */
#define DEC_POWER_CONV_TABLE_NUM 10
/** @brief HexPowerConvTable 크기 */
#define HEX_POWER_CONV_TABLE_NUM 8

/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
extern const INT32U code DecPowerConvTable[DEC_POWER_CONV_TABLE_NUM];
extern const INT32U code HexPowerConvTable[HEX_POWER_CONV_TABLE_NUM];

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
extern INT32S AbsInt32s(INT32S num);
extern INT32S multiplyDivide(INT32S A, INT32S B, INT32S C);
extern INT8U CalcDecDigitLen(INT32S decNum);
extern INT8U CalcHexDigitLen(INT32U hexNum);
//extern INT8U NibbleToHexChar(INT8U nibble);	//not use
//extern void ByteToHexStr(INT8U byte, char *str);	//not use
extern void MakeStrWithNum(char *dst, const char *src, INT32S num, INT8U size);
extern void MakeStrWithDecNum(char *dst, const char *src, INT32S num, INT8U size, INT8U digitSize);
extern void MakeStrWithHexNum(char *dst, const char *src, INT32U num, INT8U size, INT8U digitSize);
extern void MakeStrCopy(char *dst, const char *src, INT8U size);
extern INT8U Int32sToDecStr(char *dst, INT32S numData, INT8U size, char fillChar, INT8U decPos, char decChar, INT8U useThousandPt);
extern INT8U Int32uToHexStr(char *dst, INT32S numData, INT8U size, INT8U digitSize);
extern INT32U PriceRoundOff(INT32U data1, INT32U data2);
extern INT8U hex2bcd(INT8U cc);
extern INT8U bcd2hex(INT8U cc);
#ifdef USE_ARGOX_PRINTER
extern void DelStrSpace(INT8U *msg, INT16U size);
#endif
extern INT8U isNum(INT8U cc);
#ifdef USE_CTS_MODE
extern INT8U CalcBcc(INT8U *bccData, INT16U len);
extern void CLMemSet(INT8U *startAddr, INT8U changeData, INT16U size);
extern void CLMemCopy(INT8U *dst, INT8U *src, INT16U size);	//not use
#endif
extern void *Memset(void *dest, int c, unsigned int count);

#endif /* _COMMON_LIB_H */
