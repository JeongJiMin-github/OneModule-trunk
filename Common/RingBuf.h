/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Ring Buffer Header File\n
* @file    RingBuf.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _RING_BUF_H
#define _RING_BUF_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Ring Buffer struct*/
typedef struct {
	/** @brief Byte Data Array */
	char *Buf;
	/** @brief Byte Data Array Size*/
	INT16U Size;
	/** @brief Array Position for Input */
	INT16U InPtr;
	/** @brief Array Position for output */
	INT16U OutPtr;
} RING_BUF;


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

extern void RingBufInit(RING_BUF *rBuf, char *buf, INT8U size);
extern void RingBufFlushData(RING_BUF *rBuf);
extern void RingBufPutChar(RING_BUF *rBuf, char byte);
extern char RingBufGetChar(RING_BUF *rBuf);
extern char	RingBufGetCharWithParity(RING_BUF *rBuf, INT8U parityTypeChk, INT8U *error);
extern void RingBufRewindChar(RING_BUF *rBuf);

extern void RingBufPutInt16u(RING_BUF *rBuf, INT16U word);
extern INT16U RingBufGetInt16u(RING_BUF *rBuf);
extern void RingBufPutInt32sForAdDrv(RING_BUF *rBuf, INT32S dword);
extern INT32S RingBufGetInt32s(RING_BUF *rBuf);

extern void RingBufPutData(RING_BUF *rBuf, char *src, INT16U size);
extern void RingBufPutStr(RING_BUF *rBuf, char *src);
//extern void RingBufPutConstStr(RING_BUF *rBuf, const char *src);	//not use
//extern void RingBufGetData(RING_BUF *rBuf, char *dst, INT16U size);	//not use
extern INT16U RingBufCountSize(RING_BUF *rBuf);
extern INT8U RingBufCheckBuf(RING_BUF *rBuf);
extern INT16U RingBufCheckMaxBufSize(RING_BUF *rBuf);

//for interrput routine (for preventing reentrant)
extern void RingBufPutCharForKeyInterrupt(RING_BUF *rBuf, char byte);
extern void RingBufPutCharForCommInterrupt(RING_BUF *rBuf, char byte);
extern char RingBufGetCharForCommInterrupt(RING_BUF *rBuf);
extern INT8U RingBufCheckBufForCommInterrupt(RING_BUF *rBuf);

#endif /* _RING_BUF_H */