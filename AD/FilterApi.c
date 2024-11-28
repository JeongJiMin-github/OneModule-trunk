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
/** @brief FIR Filter ���� ������*/
#define FIR_FILT_SIZE	20
/** @brief Medain Filter ���� ������*/
#define MEDIAN_FILT_BUF_SIZE	20
/** @brief MAF ���� ������*/
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
/** @brief FIR Filter ����*/
//static FP32 FirFiltBuf[FIR_FILT_SIZE];	//not use

/** @brief Median Filter ����*/
//static INT32S MedianFiltBuf[MEDIAN_FILT_BUF_SIZE];
/** @brief Median Filter ���� �ε���*/
//static INT8U MedianFiltBufIndex;
/** @brief Median Filter ���� ������*/
//static INT8U MedianFiltSize;

/** @brief MAF ����*/
static INT32S MAFBuf[MAF_BUF_SIZE];
/** @brief MAF ���� �ε���*/
static INT8U MAFBufIndex;
/** @brief MAF ������ ������*/
static INT8U MAFWinSize;
/** @brief MAF ������ max ������*/
static INT8U MAFWinMaxSize;
/** @brief MAF ������ min ������*/
static INT8U MAFWinMinSize;
/** @brief MAF �����츦 Break�ϴ� ��� ��ġ*/
static INT32S MAFWinBreakThres;
/** @brief MAF �����츦 ������Ű�� ��� ��ġ*/
static INT32S MAFWinIncThres;
/** @brief MAF output ��*/
static INT32S MAFfilteredData;
/** @brief MAF ������ ����, Break ���� ��*/
//static INT32S MAFfilterRefData;
/** @brief MAF shock ���� ���ذ� */
static INT32S MAFshockRefData;
/** @brief MAF shock ���� ũ�� ���ذ� */
static INT32S MAFshockThres;
/** @brief MAF shock ���� ũ��, MAF window break ���ذ� ������ ���ذ�*/
static INT32S MAFThresIncThres;
/** @brief MAF shock ���� ������ ũ�� */
static INT8S MAFshockWinSize;
/** @brief MAF shock ���� ���� */
static INT8S MAFshockCount;


/** @brief FIR Filter ��� �迭 */
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
* @brief    FIR Filter Init �Լ�\n
* @param    none
* @return   none
* @remark   �ʱ⿡ �ѹ� ����
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
* @brief    FIR Filter�� �����͸� �Է��ϴ� �Լ�\n
* @param    none
* @return   Filering�� ��
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
* @brief    Median Filter Init �Լ�\n
* @param    filtSize : �߰����� ���ϱ� ���� buffer ���� <= MEDIAN_FILT_BUF_SIZE 
* @return   none
* @remark   �ʱ⿡ �ѹ� ����
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
* @brief    Median Filter�� �����͸� �Է��ϴ� �Լ�\n
* @param    inData : AD �Է°�
* @return   Filering�� ��
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
* @brief    Moving Average Filter Init �Լ�\n
* @param    winMinSize : window �ּ� ũ��
*           winMaxSize : window �ִ� ũ�� <= MAF_BUF_SIZE
*           winIncThres : window�� ���� ��ų ����
*           winBreakThres : window size�� �ּ�ȭ�� ����
* @return   none
* @remark   �ʱ⿡ �ѹ� ����
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
* @brief    Moving Average Filter�� �����͸� �Է��ϴ� �Լ�\n
* @param    inData : AD �Է°�
* @return   Filering�� ��
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
* @brief    Moving Average Filter with removing Shock Init �Լ�\n
* @param    winMinSize : window �ּ� ũ��
*           winMaxSize : window �ִ� ũ�� <= MAF_BUF_SIZE
*           winIncThres : window�� ���� ��ų ����
*           winBreakThres : window size�� �ּ�ȭ�� ����
*           shockWinSize : shock ���� window ũ��
*           shockThres : shock ���� ũ��
*			thresIncThres : winIncThres, winBreakThres, shockThres�� 2��� Ű�� ���
* @return   none
* @remark   �ʱ⿡ �ѹ� ����
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
* @brief    Moving Average Filter with removing Shock �� �����͸� �Է��ϴ� �Լ�\n
* @param    inData : AD �Է°�
* @return   Filering�� ��
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
	INT32S shockBreakThres;//shock filter�� �������� ���� ���ġ(�� ��� �ݿ���)
#endif

	MAFBufIndex++;
	MAFBufIndex = MAFBufIndex % MAF_BUF_SIZE;

	diffData = inData - MAFshockRefData; 

	if (inData < MAFThresIncThres) //3���� ��谪 ���� ���ġ ���̸� �״��
	{
		shockThres = MAFshockThres;
		winBreakThres = MAFWinBreakThres;
		winIncThres = MAFWinIncThres;
	}
	else //���̸� 3���� ���ġ ���� (������ Ŭ���� ����� ���� ���)
	{
		shockThres = MAFshockThres + MAFshockThres;
		winBreakThres = MAFWinBreakThres + MAFWinBreakThres;
		winIncThres = MAFWinIncThres + MAFWinIncThres;
	}

//--------------------------------------//
// Shock Filter
//--------------------------------------//
#ifdef FAST_BREAK_SHOCK_FILTER
	shockBreakThres = 128 * shockThres;//about 200d	-> Magic Number ������ ��
#endif

	if (AbsInt32s(diffData) > shockThres)//AD ��ȭ���� Shock ���ġ ���̸�(Shock �̸�)
	{
#ifdef FAST_BREAK_SHOCK_FILTER
		if (AbsInt32s(diffData) > shockBreakThres)//AD ��ȭ���� ����� ũ��(���� ��ȭ�� ����)
		{
			MAFshockCount = MAFshockWinSize;//Shock ���� ����
		}
#endif
		if (diffData > 0) sign = 1;
		else if (diffData < 0) sign = -1;
		else sign = 0;

		MAFshockCount++;

		if (MAFshockCount > MAFshockWinSize)//Shock ������ shock window���� ũ��(���� �����̸�)
		{
			MAFshockCount = MAFshockWinSize;
			currIndex = MAFBufIndex;//Shock ���� ����
		}
		else
		{
			if (MAFshockCount != 1 && oldSign != sign) //Shock ���� ���� �� AD ��ȭ���� ��ȣ�� �ٲ�� Shock ���� ��� ����
			{
				MAFBufIndex = (MAFBufIndex + MAF_BUF_SIZE - (MAFshockCount - 1)) % MAF_BUF_SIZE;
				MAFshockCount = 1;
				currIndex = (MAFBufIndex + MAF_BUF_SIZE - 1) % MAF_BUF_SIZE;
			}
			else //AD ��ȭ���� ��ȣ ������ ����
			{
				if (MAFshockCount == MAFshockWinSize) //Shock ������ shock window�� �����ϸ� Shock ���� ����
				{
					MAFBufIndex = (MAFBufIndex + MAF_BUF_SIZE - MAFshockCount) % MAF_BUF_SIZE;
					currIndex = MAFBufIndex;
				}
				else //Shock ������ shock window���� ������ Shock ���� ����
				{
					currIndex = (MAFBufIndex + MAF_BUF_SIZE - MAFshockCount) % MAF_BUF_SIZE;
				}
			}
		}
		oldSign = sign;
	}
	else //AD ��ȭ���� Shock ���ġ ���̸�(Shock�� �ƴϸ�)
	{
		if (MAFshockCount > 0 && MAFshockCount < MAFshockWinSize) //�ٷ��� shock AD ���� ������ skip
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
	minFilteredData = sum / MAFWinMinSize; //���� �ּ� ���� ��հ�

	sum = MAFWinMinSize / 2;
	for (i = 0; i < MAFWinMinSize; i++)
	{
		bufIndex = (currIndex + MAF_BUF_SIZE - MAFWinMinSize - i) % MAF_BUF_SIZE;
		sum = sum + MAFBuf[bufIndex];
	}
	prevMinFilteredData = sum / MAFWinMinSize; //�� �ּ� ���� ��հ�

	diffData = AbsInt32s(minFilteredData - prevMinFilteredData);

	if (diffData < winBreakThres) //AD ��ȭ���� break ���� ���̸� ��� ���� ũ�� ����
	{
		if (diffData < winIncThres)	//AD ��ȭ���� increase ���� ���̸� ��� ���� ũ�� ����
		{
			if (MAFshockCount == 0) 
			{
				MAFWinSize++;
			}
			if (MAFWinSize > MAFWinMaxSize)
			{
				MAFWinSize = MAFWinMaxSize;
			}
			MAFshockRefData = MAFfilteredData; //Shock ���� ���ذ� ����
		}
	}
	else //AD ��ȭ���� break ���� ���̸� ��� ���� ũ�� �ּ�ȭ
	{
		MAFWinSize = MAFWinMinSize/2 + 1;
#ifdef FAST_AVERAGE_IN_LOW_AD //���� ������ ���� ���� ũ�� �� ����(���Կ� ����)
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
		MAFshockRefData = 0; //Shock ���� ���ذ� ����
	}

	sum = MAFWinSize / 2;
	for (i = 0; i < MAFWinSize; i++)
	{
		bufIndex = (currIndex + MAF_BUF_SIZE - i) % MAF_BUF_SIZE;
		sum = sum + MAFBuf[bufIndex];
	}
	MAFfilteredData = sum / MAFWinSize;  //MAF ���� ������ ���� ũ�� ��ŭ ����� �� �ݿ�

	return MAFfilteredData;
}


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
