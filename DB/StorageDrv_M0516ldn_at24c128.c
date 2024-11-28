/**
********************************************************************************
* Copyright (c) 2017 CAS
* @brief   Storage Drvier Source File\n
           Chip : AT24C128C
* @file    StorageDrv_M0516ldn_at24c128.c
* @version 1.0
* @date    2017/04/20
* @author  Keum Young Kwang (KYK)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2017/04/20 created by KYK \n
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
#include "../Hardware/HardwareDrv.h"

#ifdef STORAGE_DRV_AT24C128C
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
#define AT24C128C_DEVICE_WRITE_ADDR 	0xA2
/** @brief EEPROM_DEVICE READ ADDRESS ( A2 : 0, A1 : 0, A0 : 1) */
#define AT24C128C_DEVICE_READ_ADDR      0xA3
/** @brief External STORAGE_PLU_AREA_SIZE*/
#define EXT_PLU_BASE_ADDR				0x1000

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
static void at24c128cWriteByte(INT8U pageAddr, INT8U lowAddr, INT8U src);
static void at24c128cReadByte(INT8U pageAddr, INT8U lowAddr, INT8U *dst);

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
    dstAddr = addr;
    
    for (i = 0; i < size; i++)
    {
        //write byte
        highAddr = (INT8U)((dstAddr & 0x3f00) >> 8);
        lowAddr = (INT8U)(dstAddr & 0x00ff);		
        
        while (I2cPortUseFlag == ON);
        I2cPortUseFlag = ON;
        
        at24c128cWriteByte(highAddr, lowAddr, src[i]);
        
        I2cPortUseFlag = OFF;
        //
        dstAddr++;
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

    srcAddr = addr;
    for (i = 0; i < size; i++)
    {
      //read byte
      highAddr = (INT8U)((srcAddr & 0x3f00) >> 8);
      lowAddr = (INT8U)(srcAddr & 0x00ff);
      
      while (I2cPortUseFlag == ON);
      I2cPortUseFlag = ON;
      
      at24c128cReadByte(highAddr, lowAddr, dst+i);
      
      I2cPortUseFlag = OFF;
      //
      srcAddr++;
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
* @brief    AT24C128C에 Write 신호를 보내는 함수\n
* @param  pageAddr : 주소상위 값, lowAddr : 주소 하위 값, src : Write하려는 값
* @return   none
* @remark
********************************************************************************
*/
void at24c128cWriteByte(INT8U pageAddr, INT8U lowAddr, INT8U src)
{
Rewrite1:
	ackOk = 1;
	tsiSendStartBit();
	tsiWriteByte(AT24C128C_DEVICE_WRITE_ADDR);
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
* @brief    AT24C128C에 Read 신호를 보내는 함수\n
* @param  pageAddr : 주소상위 값, lowAddr : 주소 하위 값, src : Read하려는 위치
* @return   none
* @remark
********************************************************************************
*/
void at24c128cReadByte(INT8U pageAddr, INT8U lowAddr, INT8U *dst)
{
Rewrite2:
	ackOk = 1;	
	tsiSendStartBit();   	
	tsiWriteByte(AT24C128C_DEVICE_WRITE_ADDR);
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
	tsiWriteByte(AT24C128C_DEVICE_READ_ADDR);
	tsiAckPollFromSlave();
	*dst = tsiReadByte();
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
			EEPROM_SDA_HIGH;
		}
		else
		{
			EEPROM_SDA_LOW;
		}
		delay1Clock();
		delay1Clock();

		EEPROM_CLK_HIGH;
		delay4Clock();
		delay4Clock();

		EEPROM_CLK_LOW;
		delay4Clock();
        delay4Clock();
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
	
	EEPROM_SDA_HIGH;		
	delay2Clock();
	delay2Clock();

	EEPROM_CLK_HIGH;		
	delay4Clock();
	delay4Clock();

    EEPROM_SDA_INCHANGE;

  	while (EEPROM_SDA_DATA)
	{
		if (ErrorCount > 10)
		{
	    		ackOk = 0;
		    	break;
		}
		ErrorCount++;
   	}
	EEPROM_SDA_OUTCHANGE;

	EEPROM_CLK_LOW;
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
	EEPROM_CLK_HIGH;
	EEPROM_SDA_HIGH;
	delay5Clock();
	delay5Clock();

	EEPROM_SDA_LOW;
	delay5Clock();
	delay5Clock();

	EEPROM_CLK_LOW;
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
  	EEPROM_SDA_LOW;		
	delay2Clock();
	delay2Clock();

  	EEPROM_CLK_HIGH;		
	delay5Clock();
	delay5Clock();

  	EEPROM_SDA_HIGH;
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
  	EEPROM_SDA_LOW;		
	delay2Clock();
	delay2Clock();

  	EEPROM_CLK_HIGH;		
	delay4Clock();
	delay4Clock();
	
  	EEPROM_CLK_LOW;
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
  	EEPROM_SDA_HIGH;		
	delay2Clock();
	delay2Clock();

  	EEPROM_CLK_HIGH;		
	delay4Clock();
	delay4Clock();

  	EEPROM_CLK_LOW;
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
  	EEPROM_SDA_HIGH;

  	for (i = 0; i < 8; i++)
	{
		delay3Clock();
		delay3Clock();

		EEPROM_CLK_HIGH;	
		delay2Clock();
		delay2Clock();

		EEPROM_SDA_INCHANGE;

		if (EEPROM_SDA_DATA == 1)
		{
			byteData |= (0x80 >> i);
		}
		EEPROM_SDA_OUTCHANGE;
		EEPROM_CLK_LOW;
  	}
  	return byteData;
}
#endif//#ifdef STORAGE_DRV_AT24C128C
