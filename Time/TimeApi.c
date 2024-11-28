/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Time(RTC) API Source File\n
* @file    TimeApi.c
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
#include "../Common/CommonLib.h"
#include "../Common/TimeData.h"
#include "TimeApi.h"
#include "TimeDrv.h"

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
/** @brief 파일내에서 time data를 사용하기 위한 변수*/
TIME_DATA TimeData;

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


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Time API Parameter를 default 값으로 초기화\n
* @param    type : defualt type
* @return   none
* @remark   Parameter 영역에 초기값을 넣는다.\n
*           Cal Mode나 Program 모드에서 전체 초기화할 때 실행한다.
********************************************************************************
*/
void TimeSetDefaultParam(void)
{

}

/**
********************************************************************************
* @brief    Time API Init 함수\n
*           Time Driver 초기화\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void TimeInit(void)
{
	TimeDrvInit();
}

/**
********************************************************************************
* @brief    Time API Write Time Struct 함수\n
* @param    none
* @return   none
* @remark   TimeData 구조체 내용을 Time IC에 쓴다.\n
********************************************************************************
*/
void TimeWriteData(void)
{
	TimeDrvWriteAllClkData();
}

/**
********************************************************************************
* @brief    Time API Read Time Struct 함수\n
* @param    none
* @return   none
* @remark   Time IC에서 Time값을 일어 TimeData 구조체에 쓴다.\n
********************************************************************************
*/
void TimeReadData(void)
{
	TimeDrvReadAllClkData();
}

/**
********************************************************************************
* @brief    Time API test 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
#include "../Communication/CommDrv.h"

void TimeTest(void)
{
	char str[10];

	TimeDrvReadAllClkData();

	MakeStrWithNum(str, "d:", TimeData.day, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "y:", TimeData.year, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "m:", TimeData.month, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "d:", TimeData.date, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "h:", TimeData.hour, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "m:", TimeData.min, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "s:", TimeData.sec, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
}
*/
/**
********************************************************************************
* @brief    Time API test 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
void TimeWriteTest(void)
{
	char str[10];

	TimeData.year = 1;
	TimeData.month = 2;
	TimeData.date = 3;
	TimeData.hour = 4;
	TimeData.min = 5;
	TimeData.sec = 6;
	//TimeWriteData();TimeWriteData();TimeWriteData();
	TimeReadData();

	MakeStrWithNum(str, "1:", TimeData.year, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "2:", TimeData.month, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "3:", TimeData.date, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "4:", TimeData.hour, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "5:", TimeData.min, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
	MakeStrWithNum(str, "6:", TimeData.sec, 4);
	RingBufPutStr(&SerialTxRingBuf, str);
}
*/
/**
********************************************************************************
* @brief    Time  함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
INT8U TimeCheckDay(INT16U year, INT16U month, INT16U day)
{
	INT16U mday;
	
	if (month > 12)
	{
		return 0;
	}
	mday = TimeGetMonthDays(year, month);
	if (day <= 0)
	{
		return 0;
	}
	if (day > mday)
	{
		return 0;
	}
	return 1;
}
*/
/**
********************************************************************************
* @brief    Time  함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
INT8U TimeGetWeek(INT16U year, INT16U month, INT16U day)
{	// year=2001,month=1..12 day=1..
        INT16U c, y, week;

	if (month < 3)
	{
		month = month + 12;
		year--;
	}
       c = year / 100;
       y = year % 100;

	week = (c >> 2) - (c << 1) + y + (y >> 2) + (26 * (month + 1) / 10) + day - 1;
	c = week % 7;
	if (c < 0)
	{
		c = 7 + c;
	}
	return (INT8U)c;
}
*/
/**
********************************************************************************
* @brief    Time  함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
INT16U TimeCheckLeap(INT16U year)
{
	INT16U a,c,y,test;

	a = year / 4000;
	c = (year - (4000 * a)) / 100;
	y = year - (4000 * a) - (100 * c);
	test = year % 4;
	
	if (y == 0) 
	{
		if (( c > 0) && (test == 0) )
		{
			return 1;	/* Leap year */
		}
	}
	if (y > 0) 
	{
		if (test == 0)
		{
			return 1;	/* Leap year */
		}
	}
	return 0;
}

/**
********************************************************************************
* @brief    Time  함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
INT16U TimeGetDays(INT16U year, INT16U month, INT16U day)	// 1..365
{
	INT16U total, i;

	month -= 1;
	total = 0;
	
	for (i = 0; i < month; i++)
	{
		total += TimeGetMonthDays(year, i+1);
	}
	
	total += day;
	return (total);
}

/**
********************************************************************************
* @brief    Time  함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void TimeGetNextDay(INT16U *year, INT16U *month, INT16U *day, INT16U ds)
{
	INT16U qd,rm;

	qd = *day + ds;
	rm = TimeGetMonthDays(*year, *month);
	if (qd > rm) 
	{
M1:
		qd = qd - rm;
		(*month)++;		
		if ((*month) > 12) 
		{
			*month = 1;
			(*year)++; 
		}
		rm = TimeGetMonthDays(*year, *month);
		if (qd > rm)
		{
			goto M1;
		}
	}
	*day = qd;
}

/**
********************************************************************************
* @brief    Time  함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
void TimeGetBeforeDay(INT16U *year, INT16U *month, INT16U *day, INT16U ds)
{
	INT16U qd, rm;

	qd = *day - ds;
	if (qd < 1) 
	{
M1:
		(*month)--;
		if ((*month) < 1) 
		{
			(*month) = 12;
			(*year)--; 
		}
		rm = TimeGetMonthDays(*year, *month);
		qd = qd + rm;
		if (qd < 1)
		{
			goto M1;
		}
	}
	*day = qd;
}
*/
/**
********************************************************************************
* @brief    Time  함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
INT16U TimeGetYearDays(INT16U year)
{
        return (TimeCheckLeap(year) + 365);
}
*/
/**
********************************************************************************
* @brief    Time  함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
INT16U TimeGetMonthDays(INT16U year, INT16U month)
{
	INT16U day;

	month--;
	if (month <= 6)
	{
		day = (month + 1) % 2;
	} 
	else
	{
		day = month % 2;
	}
	if (month == 1)
	{
		month = 28 + TimeCheckLeap(year);
	}
	else
	{
		month = 30 + day;
	}
	return month;
}
#ifdef USE_CTS_MODE 
/**
********************************************************************************
* @brief    날짜의 유효성을 확인하는 함수\n
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void TimeVerify()
{ 
	int addr;
	if(TimeData.year>99 || TimeData.month>12 || TimeData.date>31 )
	{
		for(addr = 3; addr<7; addr++)
		{
			*((INT8U *)&TimeData + addr) = 1;
		}
		TimeWriteData();
		TimeReadData();
	}
}
#endif