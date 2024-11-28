/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Time(RTC) Driver Source File\n
*          IC : HT1381, DS1302호환 (단, HT1381은 Tricle Charge, RAM 없음)
* @file    TimeDrv.c
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
#include "../Common/CommonLib.h"
#include "../Common/TimeData.h"
#include "TimeDrv.h"
#include "../Hardware/HardwareConfig.h"

#ifdef TIME_DRV_HT1381
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
/** @brief HT1381_SEC_WRITE*/
#define	HT1381_SEC_WRITE	0x80
/** @brief HT1381_SEC_READ*/
#define HT1381_SEC_READ		0x81
/** @brief HT1381_MIN_WRITE*/
#define	HT1381_MIN_WRITE	0x82
/** @brief HT1381_MIN_READ*/
#define HT1381_MIN_READ		0x83
/** @brief HT1381_HR_WRITE*/
#define	HT1381_HR_WRITE		0x84
/** @brief HT1381_HR_READ*/
#define HT1381_HR_READ		0x85
/** @brief HT1381_DATE_WRITE*/
#define	HT1381_DATE_WRITE	0x86
/** @brief HT1381_DATE_READ*/
#define HT1381_DATE_READ	0x87
/** @brief HT1381_MONTH_WRITE*/
#define	HT1381_MONTH_WRITE	0x88
/** @brief HT1381_MONTH_READ*/
#define HT1381_MONTH_READ	0x89
/** @brief HT1381_DAY_WRITE*/
#define	HT1381_DAY_WRITE	0x8A
/** @brief HT1381_DAY_READ*/
#define HT1381_DAY_READ		0x8B
/** @brief HT1381_YEAR_WRITE*/
#define	HT1381_YEAR_WRITE		0x8C
/** @brief HT1381_YEAR_READ*/
#define HT1381_YEAR_READ		0x8D
/** @brief HT1381_CTRL_WRITE*/
#define	HT1381_CTRL_WRITE		0x8E
/** @brief HT1381_CTRL_READ*/
#define HT1381_CTRL_READ		0x8F
/*
#define	DS1302_TRCHA_WRITE	0x90
#define DS1302_TRCHA_READ	0x91
*/
/** @brief HT1381_CLK_BUR_WRITE*/
#define	HT1381_CLK_BUR_WRITE	0xBE
/** @brief HT1381_CLK_BUR_READ*/
#define HT1381_CLK_BUR_READ		0xBF
/*
#define	DS1302_RAM_BUR_WRITE	0xFE
#define DS1302_RAM_BUR_READ		0xFF
*/

/** @brief HT1381_SEC_ADDR*/
#define HT1381_SEC_ADDR		0
/** @brief HT1381_MIN_ADDR*/
#define HT1381_MIN_ADDR		1
/** @brief HT1381_HOUR_ADDR*/
#define HT1381_HOUR_ADDR	2
/** @brief HT1381_DATE_ADDR*/
#define HT1381_DATE_ADDR	3
/** @brief HT1381_MONTH_ADDR*/
#define HT1381_MONTH_ADDR	4
/** @brief HT1381_DAY_ADDR*/
#define HT1381_DAY_ADDR		5
/** @brief HT1381_YEAR_ADDR*/
#define HT1381_YEAR_ADDR	6

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
static void ht1381WriteByte(INT8U bData);
static INT8U ht1381ReadByte(void);
static void ht1381WriteData(INT8U addr, INT8U bData);
static INT8U ht1381ReadData(INT8U addr);

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
//RTC Port Init	
	HT1381_RST = 0;
   	HT1381_SCLK = 1;

	ht1381WriteData(HT1381_CTRL_WRITE,0x00);	// control register 
												// disable write protect 
/* only DS1302
	ht1381WriteData(HT1381_TRCHA_WRITE,0x00);	// trickle charger register 0000(OFF):00(none):00(none)
*/
	ht1381WriteData(HT1381_CTRL_WRITE,0x80);	// control register 
												// enable write protect 
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
	ht1381WriteData(HT1381_CTRL_WRITE,0x00);	// control register
									// disable write protect 

	HT1381_SCLK = 0;
	HT1381_RST = 1;///1 us	// TODO : Check this time

	ht1381WriteByte(HT1381_CLK_BUR_WRITE);	// clock burst write (eight registers) 
	ht1381WriteByte(hex2bcd(TimeData.sec));
	ht1381WriteByte(hex2bcd(TimeData.min));
	ht1381WriteByte(hex2bcd(TimeData.hour));
	ht1381WriteByte(hex2bcd(TimeData.date));
	ht1381WriteByte(hex2bcd(TimeData.month));
	ht1381WriteByte(hex2bcd(TimeData.day));
	ht1381WriteByte(hex2bcd(TimeData.year));
	ht1381WriteByte(0x00);		// must write control register in burst mode 
//	ht1381WriteByte(0x80);		

	HT1381_RST = 0;///1us
	HT1381_SCLK = 1;

	ht1381WriteData(HT1381_CTRL_WRITE,0x80);	// control register 
									// enable write protect 
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
	HT1381_SCLK = 0;
	HT1381_RST = 1;///1 us
	ht1381WriteByte(HT1381_CLK_BUR_READ);	// clock burst 
	TimeData.sec = bcd2hex(ht1381ReadByte());
	TimeData.min = bcd2hex(ht1381ReadByte());
	TimeData.hour = bcd2hex(ht1381ReadByte());	
	TimeData.date = bcd2hex(ht1381ReadByte());
	TimeData.month = bcd2hex(ht1381ReadByte());
	TimeData.day = bcd2hex(ht1381ReadByte());
	TimeData.year = bcd2hex(ht1381ReadByte());
	HT1381_RST = 0;///1us
	HT1381_SCLK = 1;
}

/* 개별 Time Data 읽고 쓰는 부분은 현재 사용하지 않음 
//(address값이 아닌 Number로 변환 후 추후 필요시 사용)
void TimeDrvWriteClkData(INT8U clk_addr, INT8U bData)
{
	ht1381WriteData(HT1381_CTRL_WRITE,0x00);	// control register 
									// disable write protect 

	clk_addr = clk_addr<<1;
	clk_addr = (clk_addr|0x80)&0xfe;
	ht1381WriteData(clk_addr,hex2bcd(bData));

	ht1381WriteData(HT1381_CTRL_WRITE,0x80);	// control register 
									// enable write protect 
}

INT8U TimeDrvReadClkData(INT8U clk_addr)
{
	clk_addr = clk_addr << 1;
	clk_addr = clk_addr | 0x81;
	return bcd2hex(ht1381ReadData(clk_addr));
}
*/
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    ht1381 1Byte를 write하는 함수
* @param    write data (1byte)
* @return   none
* @remark   
********************************************************************************
*/
void ht1381WriteByte(INT8U bData) 
{
	INT8U i;

	for (i = 0; i < 8; i++)
	{
	   	if (bData & 0x01)	
    		{
		    HT1381_DIO = 1;//70 ns   
//			_nop();  
		}
		else 
		{
			HT1381_DIO = 0;//70 ns
//			_nop();
		}
		HT1381_SCLK = 1;//250 ns
//  		_nop();
	        bData >>= 1;
		HT1381_SCLK = 0;//250 ns 
        }        
        HT1381_DIO = 1;//for reading bi-directional port
}

/**
********************************************************************************
* @brief    ht1381 1Byte를 read하는 함수
* @param    none
* @return   read data(1 byte)
* @remark   
********************************************************************************
*/
INT8U ht1381ReadByte(void)
{
	INT8U i;
	INT8U bData;

	HT1381_DIO = 1;//for reading bi-directional port

	bData = 0x00;
	for (i = 0; i < 8; i++)
	{
       	bData >>= 1;
        	if (HT1381_DIO) 
		{
			bData |= 0x80;
		}
		else
		{
			bData &= 0x7f;
		}
        	HT1381_SCLK = 1;//250 ns
        	delay1Clock();
//		_nop();
//		_nop();
	        HT1381_SCLK = 0;//250 ns
//		_nop();  
	}
	return bData;
}

/**
********************************************************************************
* @brief    특정 주소에 1 byte data를 write하는 함수
* @param    address, byte data
* @return   none
* @remark   
********************************************************************************
*/
void ht1381WriteData(INT8U addr, INT8U bData)
{
	HT1381_SCLK = 0;	
	HT1381_RST = 1;///1 us
	ht1381WriteByte(addr);
	ht1381WriteByte(bData);
	HT1381_RST = 0;///1 us
	HT1381_SCLK = 1;	
}
/*
INT8U ht1381ReadData(INT8U addr)
{
	INT8U bData;

	HT1381_RST = 1;//1 us
	ht1381WriteByte(addr);
	bData = ht1381ReadByte();
	HT1381_RST = 0;//1 us
    return bData;
}
*/
//RAM은 DS1302만 있음 (추후 DS1302 사용시 필요한 함수)
/*
void ds1302WriteRAMData(INT8U ram_addr, INT8U bData)
{
	ht1381WriteData(HT1381_CTRL_WRITE,0x00);//	 control register 
									// disable write protect 

	if (ram_addr >= 0 && ram_addr < 31)
	{
		ram_addr = ram_addr << 1;
		ram_addr = (ram_addr | 0xc0) & 0xfe;
		ht1381WriteData(ram_addr,bData);
	}

	ht1381WriteData(HT1381_CTRL_WRITE,0x80);	// control register 
									// enable write protect 
}

INT8U ds1302ReadRAMData(INT8U ram_addr)
{
	if (ram_addr >= 0 && ram_addr < 31)
	{
		ram_addr = ram_addr << 1;
		ram_addr = ram_addr | 0xc1;
		return ht1381ReadData(ram_addr);
	}
}

void ds1302ReadAllRAMData(HUGEDATA INT8U *p_data)//Burst Mode
{
	INT8U i;
	
	HT1381_RST = 1;//1 us
	ht1381WriteByte(HT1381_RAM_BUR_READ);	// RAM burst read 

	for (i = 0; i < 31; i++)
	{
		*(p_data + i) = ht1381ReadByte();
	}

	HT1381_RST = 0;	//1 us
}

void ds1302WriteAllRAMData(HUGEDATA INT8U * p_data)
{
	INT8U i;
	ht1381WriteData(HT1381_CTRL_WRITE,0x00);	// control register 
									// disable write protect 
	
	HT1381_RST = 1;	//1 us
	ht1381WriteByte(HT1381_RAM_BUR_WRITE);	// RAM burst write 

	for (i = 0; i < 31; i++)
	{
		ht1381WriteByte(*(p_data+i));
	}
	HT1381_RST = 0;	//1 us
	ht1381WriteData(HT1381_CTRL_WRITE,0x80);	// control register 
									// enable write protect 
}
*/
#endif//#ifdef TIME_DRV_HT1381
