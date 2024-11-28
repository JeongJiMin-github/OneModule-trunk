/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Ring Buffer Source File\n
* @file    RingBuf.c
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "../Common/RingBuf.h"

#if defined(STVD) || defined(IAR)

#else
#ifdef KEIL
#pragma OPTIMIZE(0, SIZE)
#endif
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */


/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */


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
* @brief    Ring Buffer Init 함수\n
* @param    rBuf : 초기화할 Ring Buffer pointer
*           buf : buffer로 사용할 byte array pointer
*           size : buffer size
* @return   none
* @remark   Buffer 단위 size는 8,16,32 가능함 \n
*           넣고 빼는 함수를 단위에 맞게 사용하면 됨\n
*           예1) int8u buffer[5]를 ring buffer로 사용한다면\n
*                size = 5, buf = buffer\n
*                넣을때 RingBufPutChar(...) 사용\n
*                빼낼때 RingBufGetChar(...) 사용\n
*           예2) int32u buffer[5]를 ring buffer로 사용한다면\n
*                size = 5, buf = (INT8U *)buffer\n
*                넣을때 RingBufPutInt32s(...) 사용\n
*                빼낼때 RingBufGetInt32s(...) 사용\n
********************************************************************************
*/
void RingBufInit(RING_BUF *rBuf, char *buf, INT8U size)
{
	(*rBuf).OutPtr = 0;
	(*rBuf).InPtr = 0;
	(*rBuf).Size = size;
	(*rBuf).Buf = buf;
}

/**
********************************************************************************
* @brief    Ring Buffer data를 모두 버리는 함수\n
* @param    rBuf : Ring Buffer pointer
* @return   none
* @remark   
********************************************************************************
*/
/*
void RingBufFlushData(RING_BUF *rBuf)
{
	(*rBuf).OutPtr = 0;
	(*rBuf).InPtr = 0;
}
*/

/**
********************************************************************************
* @brief    Write byte to Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
*           byte : source data
* @return   none
* @remark   
********************************************************************************
*/
void RingBufPutChar(RING_BUF *rBuf, char byte)
{
	(*rBuf).Buf[(*rBuf).InPtr] = byte;

	(*rBuf).InPtr++;
	if ((*rBuf).InPtr >= (*rBuf).Size) 
	{
		(*rBuf).InPtr = 0;
	}
}

/**
********************************************************************************
* @brief    Read byte from Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
* @return   byte
* @remark   
********************************************************************************
*/
char RingBufGetChar(RING_BUF *rBuf)
{
	char byte;

	byte = (*rBuf).Buf[(*rBuf).OutPtr];

	(*rBuf).OutPtr++;
	if ((*rBuf).OutPtr >= (*rBuf).Size)
	{
		(*rBuf).OutPtr = 0;
	}

	return byte;
}

/**
********************************************************************************
* @brief    Read byte from Ring Buffer for Communication\n
* @param    rBuf : Ring Buffer pointer
* @return   byte
* @remark   
********************************************************************************
*/
char RingBufGetCharWithParity(RING_BUF *rBuf, INT8U parityType, INT8U *error)
{
	char byte;
	INT8U i, numOfOne;

	byte = (*rBuf).Buf[(*rBuf).OutPtr];

	(*rBuf).OutPtr++;
	if ((*rBuf).OutPtr >= (*rBuf).Size)
	{
		(*rBuf).OutPtr = 0;
	}
	*error = 0;
	numOfOne = 0;
	if(parityType != PARITY_TYPE_NONE)
	{
		for (i =0; i < 8; i++)
		{
			if((byte << i) & 0x80) numOfOne++;
		}
		numOfOne %= 2;
		if (parityType == numOfOne) *error = 0;
		else *error = 1;
		byte &= 0x7f;
	}	
	
	return byte;
}

/**
********************************************************************************
* @brief    Ring Buffer에서 읽은 1 byte를 되돌림\n
* @param    rBuf : Ring Buffer pointer
* @return   none
* @remark   
********************************************************************************
*/
void RingBufRewindChar(RING_BUF *rBuf)
{
	if ((*rBuf).OutPtr <= 0)
	{
		(*rBuf).OutPtr = (*rBuf).Size - 1;
	}
	else
	{
		(*rBuf).OutPtr--;
	}
}

/**
********************************************************************************
* @brief    Write int16u to Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
*           word : source data
* @return   none
* @remark   
********************************************************************************
*/
#ifdef USE_M0516LDN_MCU
void RingBufPutInt16u(RING_BUF *rBuf, INT16U word)
{
	INT16U *pBuf;

	pBuf = (INT16U *)((*rBuf).Buf);

	pBuf[(*rBuf).InPtr] = word;
	(*rBuf).InPtr++;
	if ((*rBuf).InPtr >= (*rBuf).Size)
	{
		(*rBuf).InPtr = 0;
	}
}

/**
********************************************************************************
* @brief    Read int16u from Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
* @return   word
* @remark   
********************************************************************************
*/
INT16U RingBufGetInt16u(RING_BUF *rBuf)
{
	INT16U *pBuf;
	INT16U word;

	pBuf = (INT16U *)((*rBuf).Buf);

	word = pBuf[(*rBuf).OutPtr];
	(*rBuf).OutPtr++;
	if ((*rBuf).OutPtr >= (*rBuf).Size)
	{
		(*rBuf).OutPtr = 0;
	}

	return word;
}
#endif

/**
********************************************************************************
* @brief    Write int32u to Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
*           word : source data
* @return   none
* @remark   
********************************************************************************
*/
void RingBufPutInt32sForAdDrv(RING_BUF *rBuf, INT32S dword)
{
	INT32S *pBuf;

	pBuf = (INT32S *)((*rBuf).Buf);

	pBuf[(*rBuf).InPtr] = dword;
	(*rBuf).InPtr++;
	if ((*rBuf).InPtr >= (*rBuf).Size)
	{
		(*rBuf).InPtr = 0;
	}
}

/**
********************************************************************************
* @brief    Read int32u from Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
* @return   dword
* @remark   
********************************************************************************
*/
INT32S RingBufGetInt32s(RING_BUF *rBuf)
{
	INT32S *pBuf;
	INT32S dword;

	pBuf = (INT32S *)((*rBuf).Buf);

	dword = pBuf[(*rBuf).OutPtr];
	(*rBuf).OutPtr++;
	if ((*rBuf).OutPtr >= (*rBuf).Size)
	{
		(*rBuf).OutPtr = 0;
	}

	return dword;
}

/**
********************************************************************************
* @brief    Write data to Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
*           src : source data pointer
*           size : data size
* @return   none
* @remark   
********************************************************************************
*/
void RingBufPutData(RING_BUF *rBuf, char *src, INT16U size)
{
    while (size > 0) 
	{
		RingBufPutChar(rBuf, *src);
		size--;
		src++;
    }
}

/**
********************************************************************************
* @brief    Write string to Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
*           src : source string pointer
* @return   none
* @remark   
********************************************************************************
*/
void RingBufPutStr(RING_BUF *rBuf, char *src)
{
    while (*src != '\0') 
	{
		RingBufPutChar(rBuf, *src);
		src++;
    }
}

/**
********************************************************************************
* @brief    Write const string to Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
*           src : source string pointer
* @return   none
* @remark   
********************************************************************************
*/
/*
void RingBufPutConstStr(RING_BUF *rBuf, const char *src)
{
    while (*src != '\0') 
	{
		RingBufPutChar(rBuf, *src);
		src++;
    }
}
*/

/**
********************************************************************************
* @brief    Read data to Ring Buffer\n
* @param    rBuf : Ring Buffer pointer
*           dst : destination data pointer
*           size : data size
* @return   none
* @remark   
********************************************************************************
*/
/*
void RingBufGetData(RING_BUF *rBuf, char *dst, INT16U size)
{
    while (size > 0) 
	{
		*dst = RingBufGetChar(rBuf);
		size--;
		dst++;
    }
}
*/
/**
********************************************************************************
* @brief    Check exist data size for reading\n
* @param    rBuf : Ring Buffer pointer
* @return   data size
* @remark   
********************************************************************************
*/
INT16U RingBufCountSize(RING_BUF *rBuf)
{
	INT16U size;

	if ((*rBuf).InPtr >= (*rBuf).OutPtr)
	{
		size = (*rBuf).InPtr - (*rBuf).OutPtr;
	}
	else if ((*rBuf).InPtr < (*rBuf).OutPtr)
	{
		size = (*rBuf).InPtr + (*rBuf).Size - (*rBuf).OutPtr;
	}

	return size;
}

/**
********************************************************************************
* @brief    Check exist data\n
* @param    rBuf : Ring Buffer pointer
* @return   0 -> none, 1-> exist
* @remark   
********************************************************************************
*/
INT8U RingBufCheckBuf(RING_BUF *rBuf)
{
	if ((*rBuf).InPtr != (*rBuf).OutPtr)
	{
		return TRUE;
	}
	return FALSE;
}

/**
********************************************************************************
* @brief    Check Max Buffer Size\n
* @param    rBuf : Ring Buffer pointer
* @return   Max Buffer Size
* @remark   
********************************************************************************
*/
INT16U RingBufCheckMaxBufSize(RING_BUF *rBuf)
{
	return (*rBuf).Size;
}

/**
********************************************************************************
* @brief    Write byte to Ring Buffer for Key Driver interrupt routine\n
* @param    rBuf : Ring Buffer pointer
*           byte : source data
* @return   none
* @remark   for preventing reentrant   
********************************************************************************
*/
void RingBufPutCharForKeyInterrupt(RING_BUF *rBuf, char byte)
{
	(*rBuf).Buf[(*rBuf).InPtr] = byte;

	(*rBuf).InPtr++;
	if ((*rBuf).InPtr >= (*rBuf).Size)
	{
		(*rBuf).InPtr = 0;
	}
}


/**
********************************************************************************
* @brief    Write byte to Ring Buffer for Communidation Driver interrupt routine\n
* @param    rBuf : Ring Buffer pointer
*           byte : source data
* @return   none
* @remark   for preventing reentrant   
********************************************************************************
*/
void RingBufPutCharForCommInterrupt(RING_BUF *rBuf, char byte)
{
	(*rBuf).Buf[(*rBuf).InPtr] = byte;

	(*rBuf).InPtr++;
	if ((*rBuf).InPtr >= (*rBuf).Size)
	{
		(*rBuf).InPtr = 0;
	}
}

/**
********************************************************************************
* @brief    Read byte from Ring Buffer for Communidation Driver interrupt routine\n
* @param    rBuf : Ring Buffer pointer
* @return   byte
* @remark   for preventing reentrant  
********************************************************************************
*/
char RingBufGetCharForCommInterrupt(RING_BUF *rBuf)
{
	char byte;

	byte = (*rBuf).Buf[(*rBuf).OutPtr];

	(*rBuf).OutPtr++;
	if ((*rBuf).OutPtr >= (*rBuf).Size)
	{
		(*rBuf).OutPtr = 0;
	}

	return byte;
}

/**
********************************************************************************
* @brief    Check exist data for Communidation Driver interrupt routine\n
* @param    rBuf : Ring Buffer pointer
* @return   0 -> none, 1-> exist
* @remark   for preventing reentrant
********************************************************************************
*/
INT8U RingBufCheckBufForCommInterrupt(RING_BUF *rBuf)
{
	if ((*rBuf).InPtr != (*rBuf).OutPtr)
	{
		return TRUE;
	}
	return FALSE;
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */

