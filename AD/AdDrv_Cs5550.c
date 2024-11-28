/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
           Chip : ADS1100
* @file    AdDrv.c
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
#include "AdDrv.h"

#include "../Hardware/HardwareConfig.h"
#ifdef AD_DRV_CS5550

#ifdef KEIL
#pragma OPTIMIZE (0, SIZE)
#endif
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief KR : Loadcell AD 값을 넣을 Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF AdDrvRingBuf;

/** @brief KR : Battery Voltage AD 값을 넣을 Ring Buffer\n
		  US : \n
		  CN :									*/
RING_BUF AdDrvRingBufForBatt;

/*
********************************************************************************
*                       LOCAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief KR : Channel 1(L/C) Ad 값을 넣을 buffer size\n
		  US : \n
		  CN :									*/
#define AD_BUF_SIZE		10

/** @brief KR : Channel 2(Battery) Ad 값을 넣을 buffer size\n
		  US : \n
		  CN :									*/
#define AD_BUF_SIZE_FOR_BATT	10

/** @brief KR : CS5550 Clock Freq. (Hz)\n
		  US : CS5550 Clock Freq. (Hz)\n
		  CN :									*/
#define CS5550_MCLK	4000000L

/** @brief KR : CS5550 AD sampling Freq. (Hz)\n
		  US : CS5550 AD sampling Freq. (Hz)\n
		  CN :									*/
#ifdef AD_DRV_FREQ
#define CS5550_AD_FREQ	AD_DRV_FREQ
#else
#define CS5550_AD_FREQ	16L
#endif

// CS5550 Command
/** @brief KR : CS5550 Command\n
		  US : CS5550 Command\n
		  CN :									*/
/** @brief KR : Perform a single computation cycle\n
		  US : Perform a single computation cycle\n
		  CN :									*/
#define CS5550_START_SINGLE_CON	0xe0

/** @brief KR : Perform continuous computation cycles\n
		  US : Perform continuous computation cycles\n
		  CN :									*/
#define CS5550_START_CONTI_CON	0xe8

/** @brief KR : SYNC0\n
		  US : SYNC0\n
		  CN :									*/
#define CS5550_SYNC0			0xfe

/** @brief KR : SYNC1\n
		  US : SYNC1\n
		  CN :									*/
#define CS5550_SYNC1			0xff

/** @brief KR : Power-Up/Halt\n
		  US : Power-Up/Halt\n
		  CN :									*/
#define CS5550_POWER_UP_HALT	0xa0

/** @brief KR : Software reset\n
		  US : Software reset\n
		  CN :									*/
#define CS5550_SOFT_RESET		0x80

/** @brief KR : CS5550 Register Address\n
		  US : CS5550 Register Address\n
		  CN :									*/
/** @brief KR : Configuration Register\n
		  US : Configuration Register\n
		  CN :									*/
#define CS5550_CONFIG_REG_ADDR		0

/** @brief KR : CH1 Offset Register\n
		  US : CH1 Offset Register\n
		  CN :									*/
#define CS5550_OFFSET1_REG_ADDR		1

/** @brief KR : CH1 Gain Register\n
		  US : CH1 Gain Register\n
		  CN :									*/
#define CS5550_GAIN1_REG_ADDR			2

/** @brief KR : CH2 Offset Register\n
		  US : CH2 Offset Register\n
		  CN :									*/
#define CS5550_OFFSET2_REG_ADDR		3

/** @brief KR : CH2 Gain Register\n
		  US : CH2 Gain Register\n
		  CN :									*/
#define CS5550_GAIN2_REG_ADDR			4

/** @brief KR : Cycle Count Register\n
		  US : Cycle Count Register\n
		  CN :									*/
#define CS5550_CYCLE_COUNT_REG_ADDR	5

/** @brief KR : CH1 Raw AD data register\n
		  US : CH1 Raw AD data register\n
		  CN :									*/
#define CS5550_OUT1_REG_ADDR			7

/** @brief KR : CH2 Raw AD data register\n
		  US : CH2 Raw AD data register\n
		  CN :									*/
#define CS5550_OUT2_REG_ADDR			8

/** @brief KR : CH1 Filtered AD data register\n
		  US : CH1 Filtered AD data register\n
		  CN :									*/
#define CS5550_FILT1_REG_ADDR			11

/** @brief KR : CH2 Filtered AD data register\n
		  US : CH2 Filtered AD data register\n
		  CN :									*/
#define CS5550_FILT2_REG_ADDR			12

/** @brief KR : Chip Status register\n
		  US : Chip Status register\n
		  CN :									*/
#define CS5550_STATUS_REG_ADDR		15

/** @brief KR : INT mask register\n
		  US : INT mask register\n
		  CN :									*/
#define CS5550_MASK_REG_ADDR			26

/** @brief KR : Chip Control register\n
		  US : Chip Control register\n
		  CN :									*/
#define CS5550_CONTROL_REG_ADDR		28

/** @brief KR : CS5550 Register Mask\n
		  US : CS5550 Register Mask\n
		  CN :									*/
/** @brief KR : data ready\n
		  US : data ready\n
		  CN :									*/
#define CS5550_STATUS_MASK_DRDY			0x800000

/** @brief KR : 0 = gain 10, 1 = gain 50\n
		  US : 0 = gain 10, 1 = gain 50\n
		  CN :									*/
#define CS5550_CONFIG_MASK_GAIN			0x010000

/** @brief KR : 00 ~ 11 Soft Interrupt Config bits, see Datasheet\n
		  US : 00 ~ 11 Soft Interrupt Config bits, see Datasheet\n
		  CN :									*/
#define CS5550_CONFIG_MASK_INT_CONFIG	0x001800

/** @brief KR : High Pass Filt AIN1, 0 = disable, 1 = enable\n
		  US : High Pass Filt AIN1, 0 = disable, 1 = enable\n
		  CN :									*/
#define CS5550_CONFIG_MASK_1HPF			0x000020

/** @brief KR : High Pass Filt AIN2, 0 = disable, 1 = enable\n
		  US : High Pass Filt AIN2, 0 = disable, 1 = enable\n
		  CN :									*/
#define CS5550_CONFIG_MASK_2HPF			0x000040

/** @brief KR : Invert CPU Clk, 0 = normal, 1 = min noise\n
		  US : Invert CPU Clk, 0 = normal, 1 = min noise\n
		  CN :									*/
#define CS5550_CONFIG_MASK_ICPU			0x000010

/** @brief KR : Clock Divider 0~16\n
		  US : Clock Divider 0~16\n
		  CN :									*/
#define CS5550_CONFIG_MASK_CLK_DIV		0x00000f

/** @brief KR : CS5550 Register Initial Value\n
		  US : CS5550 Register Initial Value\n
		  CN :									*/
/** @brief KR : Clock divide value\n
		  US : Clock divide value\n
		  CN :									*/
#define CS5550_CONFIG_CLK_DIV_VALUE		0x000001

/** @brief KR : Config register value\n
		  US : Config register value\n
		  CN :									*/
#define CS5550_CONFIG_REG_VALUE			(CS5550_CONFIG_MASK_GAIN|CS5550_CONFIG_MASK_ICPU|CS5550_CONFIG_CLK_DIV_VALUE)

/** @brief KR : Cycle count register value\n
		  US : Cycle count register value\n
		  CN :									*/
#define CS5550_CYCLE_COUNT_REG_VALUE		((CS5550_MCLK / CS5550_CONFIG_CLK_DIV_VALUE) / (1024L * CS5550_AD_FREQ))

/** @brief KR : Status Register initializing Value\n
		  US : Status Register initializing Value\n
		  CN :									*/
#define CS5550_STATUS_REG_VALUE			0xffffff

/** @brief KR : Mask register value\n
		  US : Mask register value\n
		  CN :									*/
#define CS5550_MASK_REG_VALUE				(CS5550_STATUS_MASK_DRDY)

/** @brief KR : Control register value\n
		  US : Control register value\n
		  CN :									*/
#define CS5550_CONTROL_REG_VALUE			0x000004


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
/** @brief KR : Loadcell AD 값을 넣을 Ring Buffer에서 사용할 byte array\n
		  US : \n
		  CN :									*/
INT32U adDrvData[AD_BUF_SIZE];

/** @brief KR : Battery Voltage AD 값을 넣을 Ring Buffer에서 사용할 byte array\n
		  US : \n
		  CN :									*/
INT32U adDrvDataForBatt[AD_BUF_SIZE_FOR_BATT];

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static void cs5550Init(void);
static void cs5550WriteByte(INT8U tran_data);
static void cs5550WriteCmd(INT8U command);
static INT32U cs5550ReadReg(INT8U address);
static void cs5550WriteReg(INT8U address, INT32U reg_value);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief       KR : AD Driver Init 함수, AD Ring Buffer Init, AD Chip Init\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 초기에 실행 되는 함수\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void AdDrvInit(void)
{
	RingBufInit(&AdDrvRingBuf, (INT8U *)(&adDrvData[0]), AD_BUF_SIZE);
	RingBufInit(&AdDrvRingBufForBatt, (INT8U *)(&adDrvDataForBatt[0]), AD_BUF_SIZE_FOR_BATT);

	cs5550Init();
}
/**
********************************************************************************
* @brief       KR : AD Sampling Rate 반환 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : Sampling rate\n
*                 US : Sampling rate\n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT16U AdDrvGetSamplingRate(void)
{
	return (INT16U)CS5550_AD_FREQ;
}

/**
********************************************************************************
* @brief       KR : AD Driver Procedure, AD Chip에서 읽은 값을 Ring Buffer에 저장\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 일정 시간에 한번씩 계속 실행 되는 함수\n
			     AD Chip의 Sampling 시간 보다는 빨리 실행 되어야 함\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void AdDrvProc(void)
{
	INT32S adData;

	adData = cs5550ReadReg(CS5550_STATUS_REG_ADDR);
	adData = adData & CS5550_STATUS_MASK_DRDY;

	if (adData)
	{  			
		adData = cs5550ReadReg(CS5550_OUT1_REG_ADDR);
		adData = adData & 0x800000;//minus
	
		if (adData)
		{  			
			adData = 0;
		}
		else
		{	
			adData = cs5550ReadReg(CS5550_FILT1_REG_ADDR);
		}
		RingBufPutInt32sForAdDrv(&AdDrvRingBuf, adData);

		adData = cs5550ReadReg(CS5550_FILT2_REG_ADDR);
		RingBufPutInt32sForAdDrv(&AdDrvRingBufForBatt, adData);
	
		cs5550WriteReg(CS5550_STATUS_REG_ADDR,CS5550_STATUS_MASK_DRDY);
	}
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief       KR : CS5550 AD Chip Init\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : 초기에 한번 실행하거나 Chip 초기화 필요시 실행\n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void cs5550Init(void)
{
	AD_CS = 0;
	AD_CS = 1;
	AD_CS = 0;
	AD_SCLK = 0;
	cs5550WriteCmd(CS5550_POWER_UP_HALT);
	cs5550WriteCmd(CS5550_SOFT_RESET);		
	cs5550WriteCmd(CS5550_SYNC1);		
	cs5550WriteCmd(CS5550_SYNC1);		
	cs5550WriteCmd(CS5550_SYNC1);		
	cs5550WriteCmd(CS5550_SYNC0);
	cs5550WriteCmd(CS5550_SYNC1);		
	cs5550WriteCmd(CS5550_SYNC1);		
	cs5550WriteCmd(CS5550_SYNC1);
	cs5550WriteReg(CS5550_STATUS_REG_ADDR,CS5550_STATUS_REG_VALUE);		
	cs5550WriteReg(CS5550_CONFIG_REG_ADDR,CS5550_CONFIG_REG_VALUE);		
	cs5550WriteReg(CS5550_CYCLE_COUNT_REG_ADDR,CS5550_CYCLE_COUNT_REG_VALUE);  		
	cs5550WriteReg(CS5550_MASK_REG_ADDR,CS5550_MASK_REG_VALUE);		 
	cs5550WriteReg(CS5550_CONTROL_REG_ADDR,CS5550_CONTROL_REG_VALUE); 

	//cs5550WriteReg(CS5550_GAIN1_REG_ADDR,0x440000);//1.1 배
	//cs5550WriteReg(CS5550_GAIN1_REG_ADDR,0x020000);//1/32 배

	cs5550WriteCmd(CS5550_START_CONTI_CON);			
}

/**
********************************************************************************
* @brief       KR : CS5550 AD Chip에 8 bit 보내는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void cs5550WriteByte(INT8U tran_data)
{
	INT8U i;

	for (i = 0; i < 8; i++)
	{
		if ((tran_data & 0x80)!=0)
		{
			AD_DIN = 1;
		}
		else
		{
			AD_DIN = 0;
		}
		AD_SCLK = 1;
		tran_data <<= 1; 
		AD_SCLK = 0;
	}  
}

/**
********************************************************************************
* @brief       KR : CS5550 AD Chip에 8 bit Command 보내는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void cs5550WriteCmd(INT8U command)
{
	//	EA = 0;	//TODO : 필요한지 확인 필요
	AD_CS = 0;
	cs5550WriteByte(command);
	AD_CS = 1;
	//	EA = 1;	
}

/**
********************************************************************************
* @brief       KR : CS5550 AD Chip의 Registor값을 읽는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : address : registor 주소\n
*                 US : \n
*                 CN : \n
* @return     KR : registor 값(4 byte)\n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
INT32U cs5550ReadReg(INT8U addr)
{
	INT8U i;
	INT8U j;
	INT8U sync_0;
	INT32U reg_value = 0;

	//make Read Register command
	addr <<= 1;//bit7 = 0, bit6 = 0(Read), bit5~1:addr, bit0 = 0

	//    EA = 0;
	AD_CS = 0;
	cs5550WriteByte(addr);				//write command

	for (j = 0; j < 3; j++)
	{
		sync_0 = CS5550_SYNC0;

		for (i = 0; i < 8; i++)
		{	       
			if ((sync_0 & 0x80)!=0)	   //write sync command
			{
				AD_DIN = 1;
			}
			else
			{
				AD_DIN = 0;
			}

			AD_SCLK  = 1;

			reg_value <<= 1;		  //read register
			if (AD_DOUT)
			{
				reg_value++;
			}
			sync_0 <<= 1;
			AD_SCLK  = 0;
		}
	}
	AD_CS = 1;
	//	EA = 1;
	return(reg_value);
} 

/**
********************************************************************************
* @brief       KR : CS5550 AD Chip의 Registor에 값을 쓰는 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : address : registor 주소(1 byte), reg_value : registor 값(4 byte)\n
*                 US : \n
*                 CN : \n
* @return     KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void cs5550WriteReg(INT8U addr, INT32U reg_value)
{
	INT8U i;

	//make Write Register command
	reg_value <<= 8;
	addr <<= 1;
	addr |= 0x40;//bit7 = 0, bit6 = 1(Write), bit5~1:addr, bit0 = 0

	//	EA = 0;
	AD_CS = 0;
	cs5550WriteByte(addr);

	for (i = 0; i < 24; i++)
	{
		if (reg_value & 0x80000000)
		{
			AD_DIN = 1;
		}
		else
		{
			AD_DIN = 0;
		}
		AD_SCLK = 1;	    
		reg_value <<= 1;
		AD_SCLK = 0;
	}
	AD_CS = 1;
	//	EA = 1;
}

#endif//#ifdef AD_DRV_CS5550
