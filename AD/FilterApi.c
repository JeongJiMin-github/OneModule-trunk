/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Filter API Source File\n
* @file    FilterApi.c
* @version 1.0
* @date    2006/12/05
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "../Common/CommonLib.h"
#include "../Common/RingBuf.h"
#include "../Communication/DebugApi.h"

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
/** @brief FIR Filter 버퍼 사이즈*/
#define FIR_FILT_SIZE	20
/** @brief Medain Filter 버퍼 사이즈*/
#define MEDIAN_FILT_BUF_SIZE	20
/** @brief MAF 버퍼 사이즈*/
#define MAF_BUF_SIZE	60

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
/** @brief FIR Filter 버퍼*/
//static FP32 FirFiltBuf[FIR_FILT_SIZE];	//not use

/** @brief Median Filter 버퍼*/
//static INT32S MedianFiltBuf[MEDIAN_FILT_BUF_SIZE];
/** @brief Median Filter 버퍼 인덱스*/
//static INT8U MedianFiltBufIndex;
/** @brief Median Filter 버퍼 사이즈*/
//static INT8U MedianFiltSize;

/** @brief MAF 버퍼*/
static INT32S MAFBuf[MAF_BUF_SIZE];
/** @brief MAF 버퍼 인덱스*/
static INT8U MAFBufIndex;
/** @brief MAF 윈도우 사이즈*/
static INT8U MAFWinSize;
/** @brief MAF 윈도우 max 사이즈*/
static INT8U MAFWinMaxSize;
/** @brief MAF 윈도우 min 사이즈*/
static INT8U MAFWinMinSize;
/** @brief MAF 윈도우를 Break하는 경계 수치*/
static INT32S MAFWinBreakThres;
/** @brief MAF 윈도우를 증가시키는 경계 수치*/
static INT32S MAFWinIncThres;
/** @brief MAF output 값*/
static INT32S MAFfilteredData;
/** @brief MAF 원도우 증가, Break 기준 값*/
//static INT32S MAFfilterRefData;
/** @brief MAF shock 판정 기준값 */
static INT32S MAFshockRefData;
/** @brief MAF shock 판정 크기 기준값 */
static INT32S MAFshockThres;
/** @brief MAF shock 판정 크기, MAF window break 기준값 증가의 기준값*/
static INT32S MAFThresIncThres;
/** @brief MAF shock 판정 윈도우 크기 */
static INT8S MAFshockWinSize;
/** @brief MAF shock 개수 변수 */
static INT8S MAFshockCount;


/** @brief FIR Filter 상수 배열 */
/*
const FP32 code FirFiltCoff[FIR_FILT_SIZE] = 
{
    0.004812806171,
    0.005358659764,
   -0.003091713149,
   -0.023101784546,
   -0.043404896952,
   -0.040649528878,
    0.006153060933,
    0.096293645935,
    0.199250219752,
    0.268178350018,
    0.268178350018,
    0.199250219752,
    0.096293645935,
    0.006153060933,
   -0.040649528878,
   -0.043404896952,
   -0.023101784546,
   -0.003091713149,
    0.005358659764,
    0.004812806171
};
*/


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
* @brief    FIR Filter Init 함수\n
* @param    none
* @return   none
* @remark   초기에 한번 실행
********************************************************************************
*/
/*
void FIRFiltInit(void)
{
	INT8U i;

	for (i = 0; i < FIR_FILT_SIZE; i++)
	{
		FirFiltBuf[i] = 0;
	}
}
*/
/**
********************************************************************************
* @brief    FIR Filter에 데이터를 입력하는 함수\n
* @param    none
* @return   Filering된 값
* @remark   
********************************************************************************
*/
/*
INT32S FIRFiltProc(INT32S inData)
{
	FP32 filteredData;
	INT8U i;

	for (i = FIR_FILT_SIZE - 1; i != 0; i--)
	{
		FirFiltBuf[i] = FirFiltBuf[i - 1];
	}
	FirFiltBuf[0] = (FP32)inData;

	filteredData = 0;
	for (i = 0; i < FIR_FILT_SIZE; i++)
	{
		filteredData += FirFiltCoff[i] * FirFiltBuf[i];
	}

	return (INT32S)filteredData;
}
*/
/**
********************************************************************************
* @brief    Median Filter Init 함수\n
* @param    filtSize : 중간값을 구하기 위한 buffer 범위 <= MEDIAN_FILT_BUF_SIZE 
* @return   none
* @remark   초기에 한번 실행
********************************************************************************
*/
/*
void MedianFiltInit(INT8U filtSize)
{
	INT8U	i;

	MedianFiltBufIndex = 0;
	MedianFiltSize = filtSize;

	for (i = 0; i < MEDIAN_FILT_BUF_SIZE; i++)
	{
		MedianFiltBuf[i] = 0;
	}
}
*/
/**
********************************************************************************
* @brief    Median Filter에 데이터를 입력하는 함수\n
* @param    inData : AD 입력값
* @return   Filering된 값
* @remark   
********************************************************************************
*/
/*
INT32S MedianFiltProc(INT32S inData)
{
	INT8U i;
	INT32S bufData;
	INT8U j;
	INT8U centerIndex;
	INT32S sortedBuf[MEDIAN_FILT_BUF_SIZE];
	INT32S filteredData;

	MedianFiltBufIndex++;
	MedianFiltBufIndex = MedianFiltBufIndex % MEDIAN_FILT_BUF_SIZE;
	MedianFiltBuf[MedianFiltBufIndex] = inData;

	for (i = 0; i < MedianFiltSize; i++)
	{
		sortedBuf[i] = MedianFiltBuf[(MedianFiltBufIndex + MEDIAN_FILT_BUF_SIZE - i) % MEDIAN_FILT_BUF_SIZE];
	}

	for (i = 0; i < (MedianFiltSize - 1); i++)
	{
		for (j = 0; j < (MedianFiltSize - i - 1); j++)
		{
			if (sortedBuf[j] > sortedBuf[j + 1]) 
			{
				bufData = sortedBuf[j];
				sortedBuf[j] = sortedBuf[j + 1];
				sortedBuf[j + 1] = bufData;
			}
		}
	}

	centerIndex = MedianFiltSize/2;
	filteredData = sortedBuf[centerIndex];

	return filteredData;
}
*/
/**
********************************************************************************
* @brief    Moving Average Filter Init 함수\n
* @param    winMinSize : window 최소 크기
*           winMaxSize : window 최대 크기 <= MAF_BUF_SIZE
*           winIncThres : window를 증가 시킬 조건
*           winBreakThres : window size를 최소화할 조건
* @return   none
* @remark   초기에 한번 실행
********************************************************************************
*/
/*
void MAFInit(INT8U winMinSize, INT8U winMaxSize, INT32S winIncThres, INT32S winBreakThres)
{
	INT8U i;

	MAFWinMinSize = winMinSize;
	MAFWinMaxSize = winMaxSize;
	MAFWinIncThres = winIncThres;
	MAFWinBreakThres = winBreakThres;

	MAFBufIndex = 0;
	MAFWinSize = MAFWinMinSize;
	MAFfilteredData = 0;
	MAFfilterRefData = 0;

	for (i = 0; i < MAF_BUF_SIZE; i++)
	{
		MAFBuf[i] = 0;
	}
}
*/
/**
********************************************************************************
* @brief    Moving Average Filter에 데이터를 입력하는 함수\n
* @param    inData : AD 입력값
* @return   Filering된 값
* @remark   
********************************************************************************
*/
/*
//#include <stdio.h>//for debug
INT32S MAFProc(INT32S inData)
{
	INT8U i;
	INT32S sum;
	INT8U bufIndex;
	INT32S minFilteredData;
//	char dbgStr[20];

	MAFBufIndex++;
	MAFBufIndex = MAFBufIndex % MAF_BUF_SIZE;
	MAFBuf[MAFBufIndex] = inData;

//Average Min. Size
	sum = MAFWinMinSize / 2;
	for (i = 0; i < MAFWinMinSize; i++)
	{
		bufIndex = (MAFBufIndex + MAF_BUF_SIZE - i) % MAF_BUF_SIZE;
		sum = sum + MAFBuf[bufIndex];
	}
	minFilteredData = sum / MAFWinMinSize;

//Debug
//	sprintf(dbgStr, "%ld	", (INT32S)MAFWinSize);
//	DebugStrOut(dbgStr);

	if (AbsInt32s(minFilteredData - MAFfilterRefData) < MAFWinBreakThres) 
	{
		if (MAFWinSize == MAFWinMinSize) 
		{
			MAFfilterRefData = minFilteredData;
		} 
		else
		{
			MAFfilterRefData = (MAFfilterRefData + MAFfilteredData) / 2;
		}

		if (AbsInt32s(minFilteredData - MAFfilterRefData) < MAFWinIncThres)
		{
			MAFWinSize++;
			if (MAFWinSize > MAFWinMaxSize)
			{
				MAFWinSize = MAFWinMaxSize;
			}
		}
	}
	else
	{
		MAFWinSize = MAFWinMinSize;
		MAFfilterRefData = minFilteredData;
	}

	sum = MAFWinSize / 2;
	for (i = 0; i < MAFWinSize; i++)
	{
		bufIndex = (MAFBufIndex + MAF_BUF_SIZE - i) % MAF_BUF_SIZE;
		sum = sum + MAFBuf[bufIndex];
	}
	MAFfilteredData = sum / MAFWinSize;

	return MAFfilteredData;
}
*/
/**
********************************************************************************
* @brief    Moving Average Filter with removing Shock Init 함수\n
* @param    winMinSize : window 최소 크기
*           winMaxSize : window 최대 크기 <= MAF_BUF_SIZE
*           winIncThres : window를 증가 시킬 조건
*           winBreakThres : window size를 최소화할 조건
*           shockWinSize : shock 판정 window 크기
*           shockThres : shock 판정 크기
*			thresIncThres : winIncThres, winBreakThres, shockThres를 2배로 키울 경계
* @return   none
* @remark   초기에 한번 실행
********************************************************************************
*/
void MAFSInit(INT8U winMinSize, INT8U winMaxSize, INT32S winIncThres, INT32S winBreakThres, INT8U shockWinSize, INT32S shockThres, INT32S thresIncThres)
{
	INT8U i;

	MAFWinMinSize = winMinSize;
	MAFWinMaxSize = winMaxSize;
	MAFWinIncThres = winIncThres;
	MAFWinBreakThres = winBreakThres;

	MAFBufIndex = 0;
	MAFWinSize = MAFWinMinSize;
	MAFfilteredData = 0;
	//MAFfilterRefData = 0;
	MAFshockRefData = 0;
	MAFshockCount = 0;
	MAFshockWinSize = shockWinSize;
	MAFshockThres = shockThres;
	MAFThresIncThres = thresIncThres;

	for (i = 0; i < MAF_BUF_SIZE; i++)
	{
		MAFBuf[i] = 0;
	}
}

/**
********************************************************************************
* @brief    Moving Average Filter with removing Shock 에 데이터를 입력하는 함수\n
* @param    inData : AD 입력값
* @return   Filering된 값
* @remark   
********************************************************************************
*/
INT32S MAFSProc(INT32S inData)
{
	INT8U i;
	INT32S sum;
	INT8U bufIndex, currIndex;
	INT32S minFilteredData;
	INT32S prevMinFilteredData;
	INT32S diffData;
	INT8S sign;
	static INT8S oldSign;
	INT32S shockThres;
	INT32S winBreakThres;
	INT32S winIncThres;
#ifdef FAST_BREAK_SHOCK_FILTER
	INT32S shockBreakThres;//shock filter를 적용하지 않을 경계치(값 즉시 반영됨)
#endif

	MAFBufIndex++;
	MAFBufIndex = MAFBufIndex % MAF_BUF_SIZE;

	diffData = inData - MAFshockRefData; 

	if (inData < MAFThresIncThres) //3가지 경계값 증가 경계치 내이면 그대로
	{
		shockThres = MAFshockThres;
		winBreakThres = MAFWinBreakThres;
		winIncThres = MAFWinIncThres;
	}
	else //밖이면 3가지 경계치 증가 (질량이 클수록 노이즈도 증가 방어)
	{
		shockThres = MAFshockThres + MAFshockThres;
		winBreakThres = MAFWinBreakThres + MAFWinBreakThres;
		winIncThres = MAFWinIncThres + MAFWinIncThres;
	}

//--------------------------------------//
// Shock Filter
//--------------------------------------//
#ifdef FAST_BREAK_SHOCK_FILTER
	shockBreakThres = 128 * shockThres;//about 200d	-> Magic Number 변경할 것
#endif

	if (AbsInt32s(diffData) > shockThres)//AD 변화량이 Shock 경계치 밖이면(Shock 이면)
	{
#ifdef FAST_BREAK_SHOCK_FILTER
		if (AbsInt32s(diffData) > shockBreakThres)//AD 변화량이 상당히 크면(무게 변화로 감지)
		{
			MAFshockCount = MAFshockWinSize;//Shock 필터 중지
		}
#endif
		if (diffData > 0) sign = 1;
		else if (diffData < 0) sign = -1;
		else sign = 0;

		MAFshockCount++;

		if (MAFshockCount > MAFshockWinSize)//Shock 개수가 shock window보다 크면(실제 질량이면)
		{
			MAFshockCount = MAFshockWinSize;
			currIndex = MAFBufIndex;//Shock 필터 중지
		}
		else
		{
			if (MAFshockCount != 1 && oldSign != sign) //Shock 필터 동작 중 AD 변화량의 부호가 바뀌면 Shock 필터 계속 동작
			{
				MAFBufIndex = (MAFBufIndex + MAF_BUF_SIZE - (MAFshockCount - 1)) % MAF_BUF_SIZE;
				MAFshockCount = 1;
				currIndex = (MAFBufIndex + MAF_BUF_SIZE - 1) % MAF_BUF_SIZE;
			}
			else //AD 변화량의 부호 변경이 없고
			{
				if (MAFshockCount == MAFshockWinSize) //Shock 개수가 shock window에 도달하면 Shock 필터 중지
				{
					MAFBufIndex = (MAFBufIndex + MAF_BUF_SIZE - MAFshockCount) % MAF_BUF_SIZE;
					currIndex = MAFBufIndex;
				}
				else //Shock 개수가 shock window보다 작으면 Shock 필터 동작
				{
					currIndex = (MAFBufIndex + MAF_BUF_SIZE - MAFshockCount) % MAF_BUF_SIZE;
				}
			}
		}
		oldSign = sign;
	}
	else //AD 변화량이 Shock 경계치 안이면(Shock이 아니면)
	{
		if (MAFshockCount > 0 && MAFshockCount < MAFshockWinSize) //바로전 shock AD 값이 있으면 skip
		{
		 	MAFBufIndex = (MAFBufIndex + MAF_BUF_SIZE - MAFshockCount) % MAF_BUF_SIZE;
		}
		MAFshockCount = 0;
		currIndex = MAFBufIndex;
	}
	MAFBuf[MAFBufIndex] = inData;

//--------------------------------------//
// Moving Average Filter
//--------------------------------------//
	sum = MAFWinMinSize / 2;
	for (i = 0; i < MAFWinMinSize; i++)
	{
		bufIndex = (currIndex + MAF_BUF_SIZE - i) % MAF_BUF_SIZE;
		sum = sum + MAFBuf[bufIndex];
	}
	minFilteredData = sum / MAFWinMinSize; //현재 최소 범위 평균값

	sum = MAFWinMinSize / 2;
	for (i = 0; i < MAFWinMinSize; i++)
	{
		bufIndex = (currIndex + MAF_BUF_SIZE - MAFWinMinSize - i) % MAF_BUF_SIZE;
		sum = sum + MAFBuf[bufIndex];
	}
	prevMinFilteredData = sum / MAFWinMinSize; //전 최소 범위 평균값

	diffData = AbsInt32s(minFilteredData - prevMinFilteredData);

	if (diffData < winBreakThres) //AD 변화량이 break 범위 안이면 평균 버퍼 크기 유지
	{
		if (diffData < winIncThres)	//AD 변화량이 increase 범위 안이면 평균 버퍼 크기 증가
		{
			if (MAFshockCount == 0) 
			{
				MAFWinSize++;
			}
			if (MAFWinSize > MAFWinMaxSize)
			{
				MAFWinSize = MAFWinMaxSize;
			}
			MAFshockRefData = MAFfilteredData; //Shock 판정 기준값 설정
		}
	}
	else //AD 변화량이 break 범위 밖이면 평균 버퍼 크기 최소화
	{
		MAFWinSize = MAFWinMinSize/2 + 1;
#ifdef FAST_AVERAGE_IN_LOW_AD //빠른 응답을 위햐 버퍼 크기 더 줄임(무게에 따라)
		if (inData < MAFThresIncThres)
		{
			if (inData < (MAFThresIncThres/2))
			{
				MAFWinSize = MAFWinSize - 2;
			}
			else
			{
				MAFWinSize = MAFWinSize - 1;
			}
			if ((MAFWinSize == 0) || (MAFWinSize > MAF_BUF_SIZE)) MAFWinSize = 1;
		}
#endif
		MAFshockRefData = 0; //Shock 판정 기준값 리셋
	}

	sum = MAFWinSize / 2;
	for (i = 0; i < MAFWinSize; i++)
	{
		bufIndex = (currIndex + MAF_BUF_SIZE - i) % MAF_BUF_SIZE;
		sum = sum + MAFBuf[bufIndex];
	}
	MAFfilteredData = sum / MAFWinSize;  //MAF 에서 정해전 버퍼 크기 만큼 평균한 값 반영

	return MAFfilteredData;
}


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
