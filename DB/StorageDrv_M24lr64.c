/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
           Chip : M24LR64
* @file    StorageDrv_M24lr64.c
* @version 1.0
* @date    2012/09/19
* @author  Deok Hyun Kim (KDH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/19 created by KDH \n
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


#ifdef STORAGE_DRV_M24LR64
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
// TODO: check EEPROM Version. 
// M24LR64-R 0xA2
// M24LR64-E 0xA6
#define M24LR64R_DEVICE_WRITE_ADDR 	0xA2
#define M24LR64E_R_DEVICE_WRITE_ADDR 	0xA6
/** @brief EEPROM_DEVICE READ ADDRESS ( A2 : 0, A1 : 0, A0 : 1) */
// TODO: check EEPROM Version. 
// M24LR64-R 0xA3
// M24LR64-E 0xA7
#define M24LR64R_DEVICE_READ_ADDR 	0xA3
#define M24LR64E_R_DEVICE_READ_ADDR 	0xA7
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
INT8U eeprom_write_addr;
INT8U eeprom_read_addr;


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
static INT8U eepromCheckdevice(void);

static void tsiSendStartBit(void);
static void tsiSendStopBit(void);
static void tsiSendAck(void);
static void tsiSendNak(void);
static void tsiWriteByte(INT8U wbuf);
static void tsiAckPollFromSlave(void);
static INT8U tsiReadByte(void);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Check 외부 EEPROM Device\n
* @param   none
* @return   none
* @remark  
********************************************************************************
*/
void StorageDrvCheckDevice()
{
	INT8U ret;	

	// Check eeprom device
	// M24LR64R
	eeprom_write_addr = M24LR64R_DEVICE_WRITE_ADDR;
	eeprom_read_addr = M24LR64R_DEVICE_READ_ADDR;;
	ret = eepromCheckdevice();
	if(ret) return;

	// M24LR64E-R
	eeprom_write_addr = M24LR64E_R_DEVICE_WRITE_ADDR;
	eeprom_read_addr = M24LR64E_R_DEVICE_READ_ADDR;;
	ret = eepromCheckdevice();
	if(ret) return;
}

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
		dstAddr = addr;
		for (i = 0; i < size; i++)
		{
			eepromWriteByte(dstAddr, src[i]);
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
		srcAddr = addr;
		for (i = 0; i < size; i++)
		{
			dst[i] = eepromReadByte(srcAddr);
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
* @brief    check EEPROM response\n
* @param    none
* @return   byte data
* @remark   
********************************************************************************
*/
INT8U eepromCheckdevice()
{
    INT8U ackcount = 0;     // duckspg cal

  	while(1)
  	{
		ackOk = 1;	
		tsiSendStartBit();
		tsiWriteByte(eeprom_write_addr);
		tsiAckPollFromSlave();
		if(!ackOk)
		{
	        ackcount++;
	        if(ackcount > 20)
	        {
				return 0;// EEPROM error
	        }
		}
		else return 1; // EEPROM OK
  	}
}

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
    INT8U ackcount = 0;     // duckspg cal
 
    INT8U pageAddr;
    INT8U lowAddr;
    INT8U dataValue=0;
    pageAddr = (addr>>8) & 0x00ff;
    lowAddr = addr & 0x00ff;  
  
Rewrite2:
	ackOk = 1;	
	tsiSendStartBit();
	tsiWriteByte(eeprom_write_addr);
	tsiAckPollFromSlave();
	if(!ackOk)
	{
        ackcount++;
        if(ackcount > 20)
        {
            EEPROM_POWEROFF;
            delay6Clock();
            EEPROM_POWERON;
        }
        goto Rewrite2;            
	}
	tsiWriteByte(pageAddr);
	tsiAckPollFromSlave();
	tsiWriteByte(lowAddr);
	tsiAckPollFromSlave();
	tsiSendStartBit();
	tsiWriteByte(eeprom_read_addr);
	tsiAckPollFromSlave();
	dataValue = tsiReadByte();
	tsiSendNak();
	tsiSendStopBit();
  
    return dataValue;
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
    
    INT8U ackcount = 0;     // duckspg cal
    INT8U pageAddr;
    INT8U lowAddr;
    
    pageAddr = (addr>>8) & 0x00ff;
    lowAddr = addr & 0x00ff;
    
Rewrite1:
	ackOk = 1;
	tsiSendStartBit(); 
	tsiWriteByte(eeprom_write_addr);
	tsiAckPollFromSlave();
	if(!ackOk)
	{
        ackcount++;
        if(ackcount > 20)
        {
           EEPROM_POWEROFF;
           delay6Clock();
           EEPROM_POWERON;
        }
        goto Rewrite1;            
	}

	tsiWriteByte(pageAddr);
	tsiAckPollFromSlave();
	tsiWriteByte(lowAddr);
	tsiAckPollFromSlave();
	tsiWriteByte(byte);
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
		EEPROM_CLK_HIGH;	
		delay4Clock();
		EEPROM_CLK_LOW;
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

    
//	EEPROM_SDA_HIGH;
//	delay2Clock();

    EEPROM_SDA_INCHANGE;
    
    delay2Clock();

	EEPROM_CLK_HIGH;		
	delay4Clock();

    
  	while (EEPROM_SDA_DATA) 
	{
		if (ErrorCount > 10) 
		{
	    		ackOk = 0;
		    	break;
		}
		ErrorCount++;
   	}
	EEPROM_CLK_LOW;
   EEPROM_SDA_OUTCHANGE;
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
	EEPROM_SDA_LOW;
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
  	EEPROM_CLK_HIGH;
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
  	EEPROM_CLK_HIGH;
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
  	EEPROM_CLK_HIGH;
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
    delay3Clock();
    EEPROM_SDA_INCHANGE;
    
  	for (i = 0; i < 8; i++) 
	{
		delay3Clock();
		EEPROM_CLK_HIGH;
		delay2Clock();
		if (EEPROM_SDA_DATA == 1)   
		{
			byteData |= (0x80 >> i);
		}
		EEPROM_CLK_LOW;
  	}
    EEPROM_SDA_OUTCHANGE;
  	return byteData;
}
#endif//#ifdef STORAGE_DRV_M24LC128T
