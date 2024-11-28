/**
********************************************************************************
* Copyright (c) 2016 CAS
* @brief   AD Driver Source File\n
           Chip : RX8010
* @file    TimeDrv_Rx8010.c
* @version 1.0
* @date    2016/12/06
* @author  Park Young Min (FDC954)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2016/12/06 created by FDC954 \n
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
#include "../Common/TimeData.h"
#include "TimeDrv.h"
#include "../Hardware/HardwareConfig.h"

#ifdef TIME_DRV_RX8010
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

/** @brief RX8010_SEC_WRITE*/
#define	RX8010_SEC_ADD	  0x10
#define	RX8010_MIN_ADD	  0x11
#define	RX8010_HOUR_ADD	  0x12
#define	RX8010_WEEK_ADD	  0x13
#define	RX8010_DAY_ADD	  0x14
#define	RX8010_MONTH_ADD  0x15
#define	RX8010_YEAR_ADD	  0x16

#define	RX8010_READ	    0x65
#define	RX8010_WRITE    0x64
//#define RX8010_CON_ADD  0x0E




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
void rtcSendStartBit(void);
void rtcSendStopBit(void);
void rtcAckPollFromSlave(void);
void rtcSendAck(void);
void rtcSendNak(void);
INT8U rtcReadByte(void);
void rtcWriteByte(INT8U wbuf);
void rx8010ReadTimedata(void);
void rx8010WriteTimeData(void);

INT8U ackOk;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Time Drvier Init 함수\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/
void TimeDrvInit(void)
{

}

/**
********************************************************************************
* @brief    Time Driver Write Time Struct 함수\n
* @param    none
* @return   none
* @remark   TimeData 구조체 내용을 Time IC에 쓴다.\n
********************************************************************************
*/
void TimeDrvWriteAllClkData(void)//Burst Mode
{
    rx8010WriteTimeData();
}

/**
********************************************************************************
* @brief    Time Driver Read Time Struct 함수\n
* @param    none
* @return   none
* @remark   Time IC에서 Time값을 일어 TimeData 구조체에 쓴다.\n
********************************************************************************
*/
void TimeDrvReadAllClkData(void)
{
    rx8010ReadTimedata();
}

void rx8010WriteTimeData(void)
{
    INT8U ackcount = 0;     // duckspg cal
    INT8U week;     
    
Rertcwrite1:
	ackOk = 1;	
	rtcSendStartBit();
	rtcWriteByte(RX8010_WRITE);
	rtcAckPollFromSlave();
	if(!ackOk)
	{
        ackcount++;
        if (ackcount < 20)
        {
            goto Rertcwrite1;
        }
	}
    rtcWriteByte(RX8010_SEC_ADD);
    rtcAckPollFromSlave();
    rtcWriteByte(hex2bcd(TimeData.sec));
    rtcAckPollFromSlave();
    rtcWriteByte(hex2bcd(TimeData.min));
    rtcAckPollFromSlave();
    rtcWriteByte(hex2bcd(TimeData.hour));
    rtcAckPollFromSlave();   
    week = TimeData.day;    
    switch (week) // weekday
	{
		default:		
		case 1: week = 0x02; break; // Monday
		case 2: week = 0x04; break; // Tuesday
		case 3: week = 0x08; break; // Wednesday
		case 4: week = 0x10; break; // Thursday
		case 5: week = 0x20; break; // Friday
		case 6: week = 0x40; break; // Saturday
        case 7: week = 0x01; break; // Sunday
	}    
    rtcWriteByte(week);
    rtcAckPollFromSlave();
    rtcWriteByte(hex2bcd(TimeData.date));
    rtcAckPollFromSlave();
    rtcWriteByte(hex2bcd(TimeData.month));
    rtcAckPollFromSlave();
    rtcWriteByte(hex2bcd(TimeData.year));
    rtcAckPollFromSlave();
    rtcSendStopBit();
}

void rx8010ReadTimedata(void)
{
    INT8U ackcount = 0;     // duckspg cal
    INT8U week;     
    
Rertcwrite2:
	ackOk = 1;
	rtcSendStartBit();
	rtcWriteByte(RX8010_WRITE);
	rtcAckPollFromSlave();
	if(!ackOk)
	{
          ackcount++;
          if (ackcount < 20)
          {
            goto Rertcwrite2;
          }
	}
    
    rtcWriteByte(RX8010_SEC_ADD);
    rtcAckPollFromSlave();
    rtcSendStartBit();  
    rtcWriteByte(RX8010_READ);
    rtcAckPollFromSlave();
    TimeData.sec = bcd2hex(rtcReadByte());
    rtcSendAck();
    TimeData.min = bcd2hex(rtcReadByte());
    rtcSendAck();
    TimeData.hour = bcd2hex(rtcReadByte());
    rtcSendAck();
    week= rtcReadByte();    
    switch (week) // weekday
	{
		default:
		case 0x01: week = 7; break; // Sunday
		case 0x02: week = 1; break; // Monday
		case 0x04: week = 2; break; // Tuesday
		case 0x08: week = 3; break; // Wednesday
		case 0x10: week = 4; break; // Thursday
		case 0x20: week = 5; break; // Friday
		case 0x40: week = 6; break; // Saturday
	}
    TimeData.day = week;
    rtcSendAck();    
    TimeData.date = bcd2hex(rtcReadByte());
    rtcSendAck();
    TimeData.month = bcd2hex(rtcReadByte());
    rtcSendAck();
    TimeData.year = bcd2hex(rtcReadByte());
    rtcSendNak();
    rtcSendStopBit();
}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Read Storage Byte Data\n
* @param    addr : Storage Address
* @return   byte data
* @remark   
********************************************************************************
*/
INT8U rx8010ReadStrByte(void)
{
    INT8U rtcData = 0;
	rtcData = rtcReadByte();
    rtcSendAck();
    return rtcData;
}
/**
********************************************************************************
* @brief    Start 신호를 보내는 함수\n
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void rtcSendStartBit(void)
{    
	RTC_CLK_HIGH;
	RTC_SDA_HIGH;
	delay1Clock();
	RTC_SDA_LOW;
	delay5Clock();  
	RTC_CLK_LOW;
} 
/**
********************************************************************************
* @brief    Stop 신호를 보내는 함수\n
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void rtcSendStopBit(void)
{
  	RTC_SDA_LOW;
	delay2Clock();
  	RTC_CLK_HIGH;
	delay5Clock();
  	RTC_SDA_HIGH;
}

/**
********************************************************************************
* @brief    ACK 신호를 기다리는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void rtcAckPollFromSlave(void)
{	
	INT8U ErrorCount = 0;


//	RTC_SDA_HIGH;
//	delay2Clock();
    RTC_SDA_LOW;
    
	RTC_SDA_INCHANGE;
	RTC_CLK_HIGH;
	delay4Clock();

   
  	while (RTC_SDA_DATA) 
	{
		if (ErrorCount > 10) 
		{
	    		ackOk = 0;
		    	break;
		}
		ErrorCount++;
   	}
	RTC_CLK_LOW;
   RTC_SDA_OUTCHANGE;
}

/**
********************************************************************************
* @brief    ACK 신호를 보내는 함수\n
* @param    none
* @return   none
* @remark   Run Initail Time
********************************************************************************
*/
void rtcSendAck(void)
{
  	RTC_SDA_LOW;
	delay2Clock();
  	RTC_CLK_HIGH;
	delay4Clock(); 
  	RTC_CLK_LOW;
}
/**
********************************************************************************
* @brief    NAK 신호를 보내는 함수\n
* @param    none
* @return   none
* @remark   Run Initail Time
********************************************************************************
*/
void rtcSendNak(void)
{
  	RTC_SDA_HIGH;
	delay2Clock();
  	RTC_CLK_HIGH;
	delay4Clock();
  	RTC_CLK_LOW;
}
/**
********************************************************************************
* @brief    1 Byte를 읽어오는 신호를 받는 함수\n
* @param    none
* @return   none
* @remark 
********************************************************************************
*/
INT8U rtcReadByte(void)
{
  	INT8U byteData;
  	INT8U i;
	
  	byteData = 0;
  	RTC_SDA_HIGH;
    delay3Clock();
    RTC_SDA_INCHANGE;
    
  	for (i = 0; i < 8; i++) 
	{
		delay3Clock();
		RTC_CLK_HIGH;
		delay2Clock();
		if (RTC_SDA_DATA == 1)
		{
			byteData |= (0x80 >> i);
		}
		RTC_CLK_LOW;
  	}
    RTC_SDA_OUTCHANGE;
//    delay15Clock(); // for test
  	return byteData;
}


/**
********************************************************************************
* @brief    1 Byte 신호를 보내는 함수\n
* @param    wbuf : 보내려는 1 byte값 
* @return   none
* @remark  
********************************************************************************
*/
void rtcWriteByte(INT8U wbuf)
{ 
  	INT8U i;
	
	for (i = 0; i < 8; i++) 
	{
		if (wbuf & (0x80 >> i)) 
		{
			RTC_SDA_HIGH;
		}
		else
		{
			RTC_SDA_LOW;
		}
		delay1Clock();
		RTC_CLK_HIGH;
		delay4Clock();
		RTC_CLK_LOW;
  	}
}

#endif//#ifdef TIME_DRV_RX8010
