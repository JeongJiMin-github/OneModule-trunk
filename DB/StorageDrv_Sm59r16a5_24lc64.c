/**
********************************************************************************
* Copyright (c) 2014 CAS
* @brief   Storage Drvier Source File for 24lc64\n
           Chip : 24LC64
* @file    StorageDrv_Sm59r16a5_24lc64.c
* @version 1.0
* @date    2014/02/24
* @author  Keum Young-Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2014/02/24 created by KYK \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "StorageDrv.h"

#include "../Hardware/HardwareConfig.h"
#include "../Hardware/HardwareDrv.h"	//TODO confirm

#ifdef STORAGE_DRV_SM59R16A5_M24LC64T
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

/** @brief EEPROM_DEVICE WRITE ADDRESS ( A2 : 0, A1 : 0, A0 : 1) */
#define M24LC64_DEVICE_WRITE_ADDR 	0xA2
/** @brief EEPROM_DEVICE READ ADDRESS ( A2 : 0, A1 : 0, A0 : 1) */
#define M24LC64_DEVICE_READ_ADDR 		0xA3
/** @brief External STORAGE_PLU_AREA_SIZE*/
#define EXT_PLU_BASE_ADDR				0x0


/*
********************************************************************************
*                       LOCAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert local typedef & structure here */
/** @brief EEPROM에서 ACK가 정상 동작함을 나타내는 Flag */
static BOOLEAN ackOk;


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
//Two wire Serial Interface
static INT8U eepromReadByte(INT16U addr);
static void eepromWriteByte(INT16U addr, INT8U byte);

static void tsiSendStartBit(void);
static void tsiSendStopBit(void);
static void tsiSendAck(void);
static void tsiSendNak(void);
static void tsiWriteByte(INT8U wbuf);
static void tsiAckPollFromSlave(void);
static INT8U tsiReadByte(void);
static void m24lc64tWriteByte(INT8U pageAddr, INT8U lowAddr, INT8U src);
static void m24lc64tReadByte(INT8U pageAddr, INT8U lowAddr, INT8U *dst);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    외부 EEPROM에 1Byte write하는 함수\n
* @param   addr : Write하려는 주소, src : Write하려는 값
* @return   none
* @remark  
********************************************************************************
*/
void StorageDrvWriteData(INT16U addr, INT8U *src, INT16U size)
{
	INT8U highAddr;
	INT8U lowAddr;
	INT16U dstAddr;
	INT16U i;

	if(addr < EXT_PLU_BASE_ADDR)
	{			
		dstAddr = addr;
		for (i = 0; i < size; i++)
		{
			eepromWriteByte(dstAddr, src[i]);
			dstAddr++;
		}
	}
	else
	{
		dstAddr = addr - EXT_PLU_BASE_ADDR;
		for (i = 0; i < size; i++)
		{
			//write byte
			highAddr = (INT8U)((dstAddr & 0x1f00) >> 8);
			lowAddr = (INT8U)(dstAddr & 0x00ff);		
				
			while (I2cPortUseFlag == ON);
			I2cPortUseFlag = ON;
			
			m24lc64tWriteByte(highAddr, lowAddr, src[i]);
			
			I2cPortUseFlag = OFF;
			//
			dstAddr++;
		}
	}
}

/**
********************************************************************************
* @brief   외부 EEPROM에 1Byte Read하는 함수\n
* @param   addr : Read하려는 주소, dst : Read할 위치 
* @return   none
* @remark  
********************************************************************************
*/
void StorageDrvReadData(INT16U addr, INT8U *dst, INT16U size)
{
	INT8U highAddr;
	INT8U lowAddr;	
	INT16U srcAddr;
	INT16U i;
	
	if(addr < EXT_PLU_BASE_ADDR)
	{		
		srcAddr = addr;
		for (i = 0; i < size; i++)
		{
			dst[i] = eepromReadByte(srcAddr);
			srcAddr++;
		}
	}
	else
	{
		srcAddr = addr - EXT_PLU_BASE_ADDR;
		for (i = 0; i < size; i++)
		{
			//read byte
			highAddr = (INT8U)((srcAddr & 0x1f00) >> 8);
			lowAddr = (INT8U)(srcAddr & 0x00ff);
			
			while (I2cPortUseFlag == ON);
			I2cPortUseFlag = ON;
			
			m24lc64tReadByte(highAddr, lowAddr, dst+i);
			
			I2cPortUseFlag = OFF;
			//
			srcAddr++;
		}
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
* @brief    Read Storage Byte Data\n
* @param    addr : Storage Address
* @return   byte data
* @remark   
********************************************************************************
*/
INT8U eepromReadByte(INT16U addr)
{
	return 0;
}

/**
********************************************************************************
* @brief    Write Storage Byte Data\n
* @param    addr : Storage Address
*           byte : data for writing
* @return   none
* @remark   
********************************************************************************
*/
void eepromWriteByte(INT16U addr, INT8U byte)
{
}


/**
********************************************************************************
* @brief    24LC64에 Write 신호를 보내는 함수\n
* @param  pageAddr : 주소상위 값, lowAddr : 주소 하위 값, src : Write하려는 값
* @return   none
* @remark   
********************************************************************************
*/
void m24lc64tWriteByte(INT8U pageAddr, INT8U lowAddr, INT8U src)
{
Rewrite1:
	ackOk = 1;
	tsiSendStartBit(); 
	tsiWriteByte(M24LC64_DEVICE_WRITE_ADDR);
	tsiAckPollFromSlave();
	if(!ackOk)
	{
		goto Rewrite1;
	}
	tsiWriteByte(pageAddr);
	tsiAckPollFromSlave();
	tsiWriteByte(lowAddr);
	tsiAckPollFromSlave();
	tsiWriteByte(src);
	tsiAckPollFromSlave();
	tsiSendStopBit();
}

/**
********************************************************************************
* @brief    24LC64에 Read 신호를 보내는 함수\n
* @param  pageAddr : 주소상위 값, lowAddr : 주소 하위 값, src : Read하려는 위치
* @return   none
* @remark   
********************************************************************************
*/
void m24lc64tReadByte(INT8U pageAddr, INT8U lowAddr, INT8U *dst)
{
Rewrite2:
	ackOk = 1;	
	tsiSendStartBit();   	
	tsiWriteByte(M24LC64_DEVICE_WRITE_ADDR);
	tsiAckPollFromSlave();
	if(!ackOk)
	{
		goto Rewrite2;
	}
	tsiWriteByte(pageAddr);
	tsiAckPollFromSlave();
	tsiWriteByte(lowAddr);
	tsiAckPollFromSlave();
	tsiSendStartBit();
	tsiWriteByte(M24LC64_DEVICE_READ_ADDR);   
	tsiAckPollFromSlave();
	*dst = tsiReadByte();
	tsiSendAck();	
	tsiSendNak();
	tsiSendStopBit();
}

/**
********************************************************************************
* @brief    1 Byte 신호를 보내는 함수\n
* @param    wbuf : 보내려는 1 byte값 
* @return   none
* @remark  
********************************************************************************
*/
void tsiWriteByte(INT8U wbuf)
{ 
  	INT8U i;	
	
	for (i = 0; i < 8; i++) 
	{
		if (wbuf & (0x80 >> i)) 
		{
			I2C_SDA = 1;
		}
		else
		{
			I2C_SDA = 0;
		}
		delay1Clock();
		delay1Clock();	//for test
		delay1Clock();	//for test
		delay1Clock();	//for test
		I2C_SCLK = 1;	
		delay4Clock();
		delay4Clock();	//for test
		delay4Clock();	//for test
		delay4Clock();	//for test
		I2C_SCLK = 0;
  	}

}

/**
********************************************************************************
* @brief    ACK 신호를 기다리는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void tsiAckPollFromSlave(void)
{	
	INT8U ErrorCount = 0;
	
	I2C_SDA = 1;		
	delay2Clock();
	delay2Clock();	//for test
	delay2Clock();	//for test
	delay2Clock();	//for test
	I2C_SCLK = 1;		
	delay4Clock();
	delay4Clock();	//for test
	delay4Clock();	//for test
	delay4Clock();	//for test

  	while (I2C_SDA) 
	{
		if (ErrorCount > 10) 
		{
	    		ackOk = 0;
		    	break;
		}
		ErrorCount++;
   	}
	I2C_SCLK = 0;
}

/**
********************************************************************************
* @brief    Start 신호를 보내는 함수\n
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void tsiSendStartBit(void)
{    
	I2C_SCLK = 1;
	I2C_SDA = 1;
	delay1Clock();
	delay1Clock();	//for test
	delay1Clock();	//for test
	delay1Clock();	//for test
	I2C_SDA = 0;  
	delay5Clock();  
	delay5Clock();  	//for test
	delay5Clock();  	//for test
	delay5Clock();  	//for test
	I2C_SCLK = 0;
} 

/**
********************************************************************************
* @brief    Stop 신호를 보내는 함수\n
* @param    none
* @return   none
* @remark  
********************************************************************************
*/
void tsiSendStopBit(void)
{
  	I2C_SDA = 0;		
	delay2Clock();
	delay2Clock(); //for test
	delay2Clock(); //for test
	delay2Clock(); //for test
  	I2C_SCLK = 1;		
	delay5Clock();
	delay5Clock();	//for test
	delay5Clock();	//for test
	delay5Clock();	//for test
  	I2C_SDA = 1;
}

/**
********************************************************************************
* @brief    ACK 신호를 보내는 함수\n
* @param    none
* @return   none
* @remark   Run Initail Time
********************************************************************************
*/
void tsiSendAck(void)
{
  	I2C_SDA = 0;		
	delay2Clock();
	delay2Clock();	//for test
	delay2Clock();	//for test
	delay2Clock();	//for test
  	I2C_SCLK = 1;		
	delay4Clock();
	delay4Clock();	//for test
	delay4Clock();	//for test
	delay4Clock();	//for test
  	I2C_SCLK = 0;
}

/**
********************************************************************************
* @brief    NAK 신호를 보내는 함수\n
* @param    none
* @return   none
* @remark   Run Initail Time
********************************************************************************
*/
void tsiSendNak(void)
{
  	I2C_SDA = 1;		
	delay2Clock();
	delay2Clock();	//for test
	delay2Clock();	//for test
	delay2Clock();	//for test
  	I2C_SCLK = 1;		
	delay4Clock();
	delay4Clock();	//for test
	delay4Clock();	//for test
	delay4Clock();	//for test
  	I2C_SCLK = 0;
}


/**
********************************************************************************
* @brief    1 Byte를 읽어오는 신호를 받는 함수\n
* @param    none
* @return   none
* @remark 
********************************************************************************
*/
INT8U tsiReadByte(void)
{
  	INT8U byteData;
  	INT8U i;
	
  	byteData = 0;
  	I2C_SDA = 1;

  	for (i = 0; i < 8; i++) 
	{
		delay3Clock();
		delay3Clock();	//for test
		delay3Clock();	//for test
		delay3Clock();	//for test
		I2C_SCLK = 1;	
		delay2Clock();
		delay2Clock();	//for test
		delay2Clock();	//for test
		delay2Clock();	//for test
		if (I2C_SDA == 1)   
		{
			byteData |= (0x80 >> i);
		}
		I2C_SCLK = 0;
  	}
  	return byteData;
}
#endif//#ifdef STORAGE_DRV_M24LC64T
