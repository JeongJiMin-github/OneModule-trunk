/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Temperature Sensor Driver Source File\n
           Chip : TCN75A
* @file    TempDrv.c
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
#include "TempDrv.h"

#include "../Hardware/HardwareConfig.h"
#ifdef TEMP_DRV_TCN75A
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
/** @brief Temp. senosr buffer size */
#define TEMP_DRV_BUF_SIZE	10
/** @brief Temp. senosr �д� �ð� ���� (���� : 100ms), 1 min*/
//#define AD_TEMP_READ_100MS_TIME	600
#define AD_TEMP_READ_100MS_TIME	100 //100 msec * 100 = 10 sec

/** @brief I2C_WRITE_BIT*/
#define I2C_WRITE_BIT		0x00
/** @brief I2C_READ_BIT*/
#define I2C_READ_BIT		0x01

/** @brief I2C ��� ���� EEPROM Device ���� ID, for future use*/
#define EEPROM_ADDR			0xA0

/** @brief I2C ��� ���� TEMPERATURE Sensor Device ���� ID*/
#define TCN75A_DEV_ADDR		0x90
/** @brief TCN75A_SLAVE_ADDR, 3 addr pin is ground (see schematic)*/
#define TCN75A_SLAVE_ADDR	0x00
/** @brief �µ������͸� �����ϴ� �������� */
#define TCN75A_DATA_ADDR	0x00
/** @brief �µ������� �����ϴ� �������� */
#define TCN75A_CONFIG_ADDR	0x01

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
/** @brief Temp. senosr buffer */
static INT8S tempDrvBuf[TEMP_DRV_BUF_SIZE];
/** @brief Temp. senosr buffer Index (Input) */
static INT8U tempDrvBufIndex;

/** @brief �µ��������� ACK�� ���� �������� ��Ÿ���� Flag */
static BOOLEAN ackOk;
/** @brief �µ������� (���� : Deg) */
static INT8S tempValue;

#ifdef DEBUG_TEMP
/** @brief tempDebugFlag*/
static BOOLEAN tempDebugFlag;
/** @brief tempDebugData*/
static INT8S tempDebugData;
#endif

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
#ifdef USE_TEMP_COMP
static INT8S tempDrvReadCurrData(void);
#endif
//Below is not static, for use I2C in other files
void I2cWriteByte(INT8U deviceAddr, INT8U slaveAddr, INT8U addr, INT8U writeData);
INT8U I2cReadByte(INT8U deviceAddr, INT8U slaveAddr, INT8U addr);
INT16U I2cReadInt16u(INT8U deviceAddr, INT8U slaveAddr, INT8U address);

static void i2cStartCondition(void);
static void i2cStopCondition(void);
static void i2cAckPollFromSlave(void);
static void i2cSendAck(void);
static void i2cSendNak(void);
static void i2cSendByteToSlave(INT8U sendData);
static INT8U i2cReceiveByteFromSlave(void);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
#ifdef USE_TEMP_COMP
/*
********************************************************************************
* @brief    Temperature Sensor Driver Procedure\n
* @param    none
* @return   none
* @remark   ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�
*           tempValue�� ������Ʈ��
********************************************************************************
*/
void TempDrvProc(INT16U currTimer100ms)
{
	static INT16U prevTempReadTimer100ms = 0;
	INT8U i;
	INT16S sum;

	if ((currTimer100ms - prevTempReadTimer100ms) > AD_TEMP_READ_100MS_TIME)
	{
		prevTempReadTimer100ms = currTimer100ms;

		//put buffer
		tempDrvBuf[tempDrvBufIndex] = tempDrvReadCurrData();
		tempDrvBufIndex++;
		tempDrvBufIndex = tempDrvBufIndex % TEMP_DRV_BUF_SIZE;

		//averaging
		sum = TEMP_DRV_BUF_SIZE / 2;
		for (i = 0; i < TEMP_DRV_BUF_SIZE; i++)
		{
			sum = sum + tempDrvBuf[i];
		}
		tempValue = (INT8S)(sum / TEMP_DRV_BUF_SIZE); 
	}

#ifdef DEBUG_TEMP
	if (tempDebugFlag) 
	{
		tempValue = tempDebugData;
	}
#endif

}

/**
********************************************************************************
* @brief    Initialize Temperature sensor.\n
*           1. ONE shot mode : disable
*			2. ADC resolution : 12 bit
*			3. Fault queue : 1 bit
*			4. Alert polarity : Active low
*			5. mode : comparator
*			6. shut down : disable
* @param    none
* @return   Temperature Sensor Initial (1 : Succeed, 0 : Fail)
* @remark   
********************************************************************************
*/
BOOLEAN TempDrvInit(void)
{
	INT8U i;

	tempDrvBufIndex = 0;

	ackOk = 1;
	I2cWriteByte(TCN75A_DEV_ADDR, TCN75A_SLAVE_ADDR, TCN75A_CONFIG_ADDR, 0x60); //config register setting

	if (ackOk) 
	{
		tempValue = tempDrvReadCurrData();
		for (i = 0; i < TEMP_DRV_BUF_SIZE; i++)
		{
			tempDrvBuf[i] = tempValue;
		}
	}
	else
	{
		tempValue = 20;
	}

#ifdef DEBUG_TEMP
	tempDebugFlag = 0;
#endif

	return ackOk;
}

/**
********************************************************************************
* @brief    Filtering �� �µ������� ���� �д� �Լ�\n
* @param    none
* @return   �µ��� (���� : 1 degree C)
* @remark   
********************************************************************************
*/
INT8S TempDrvReadData(void)
{
	return tempValue;
}
#endif
#ifdef DEBUG_TEMP
/**
********************************************************************************
* @brief    �µ������� �� �����ϴ� �Լ� (for Debug)\n
* @param    temp : �µ��� (���� : 1 degree C)
* @return   none
* @remark   
********************************************************************************
*/
void TempDrvWriteDataForDebug(INT8S temp)
{
	if (temp == -100)
	{
		tempDebugFlag = 0;
		tempValue = tempDrvReadCurrData();
	}
	else
	{
		tempDebugFlag = 1;
		tempDebugData = temp;
		tempValue = tempDebugData;
	}
}
#endif
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    ���� �µ������� ���� �д� �Լ�\n
* @param    none
* @return   �µ��� (���� : 1 degree C)
* @remark   Debug �뵵 �ܿ��� ���� ������� ���ÿ�.(TempDrvReadData() ����� ��)
********************************************************************************
*/
INT8S tempDrvReadCurrData(void)
{
	INT16U value;

	value = I2cReadInt16u(TCN75A_DEV_ADDR, TCN75A_SLAVE_ADDR, TCN75A_DATA_ADDR);
	return (INT8S)(value >> 8);
}

/**
********************************************************************************
* @brief    1 Byte data�� Slave�� Ư�� �ּҿ� Writing�ϴ� �Լ�\n
* @param    ������, �ּ�, �Էµ�����, Slave device ID
* @return   none
* @remark   
********************************************************************************
*/
void I2cWriteByte(INT8U deviceAddr, INT8U slaveAddr, INT8U addr, INT8U writeData)
{
  	i2cStartCondition();

	i2cSendByteToSlave(deviceAddr|I2C_WRITE_BIT + slaveAddr);		// Slave Address /w     Command write
	
  	i2cAckPollFromSlave();

  	i2cSendByteToSlave(addr);							// Address to Begin
  	i2cAckPollFromSlave();

  	i2cSendByteToSlave(writeData);								// data to save
  	i2cAckPollFromSlave();

  	i2cStopCondition();
}


/**
********************************************************************************
* @brief    1 Byte data�� Slave�� Ư�� �ּҿ� Reading�ϴ� �Լ�\n
* @param    ������, �ּ�, Slave device ID
* @return   �о�� ������(INT8U)
* @remark   
********************************************************************************
*/

INT8U I2cReadByte(INT8U deviceAddr, INT8U slaveAddr, INT8U addr)
{
  	INT8U readData;

  	i2cStartCondition();

	i2cSendByteToSlave(deviceAddr|I2C_WRITE_BIT + slaveAddr);	// Slave Address /w     Command
	
	i2cAckPollFromSlave();

  	i2cSendByteToSlave(addr);						// Address to read
	i2cAckPollFromSlave();

  	i2cStartCondition();
	
	i2cSendByteToSlave(deviceAddr|I2C_READ_BIT + slaveAddr);	// Slave Address /w     Read Command

	i2cAckPollFromSlave();

  	readData = i2cReceiveByteFromSlave();

  	i2cSendNak();
  	i2cStopCondition();
	
  	return readData;
}

/**
********************************************************************************
* @brief    2 Byte data�� Slave�� Ư�� �ּҿ� Reading�ϴ� �Լ�\n
* @param    ������, �ּ�, Slave device ID
* @return   �о�� ������ (INT16U)
* @remark   
********************************************************************************
*/
INT16U I2cReadInt16u(INT8U deviceAddr, INT8U slaveAddr, INT8U addr)
{
	UNION_INT16U readData;

  	i2cStartCondition();

	i2cSendByteToSlave(deviceAddr|I2C_WRITE_BIT + slaveAddr);	// Slave Address /w     Command
	
	i2cAckPollFromSlave();

  	i2cSendByteToSlave(addr);						// Address to read

	i2cAckPollFromSlave();

  	i2cStartCondition();
	
	i2cSendByteToSlave(deviceAddr|I2C_READ_BIT + slaveAddr);	// Slave Address /w     Read Command

	i2cAckPollFromSlave();

  	readData.byte.high = i2cReceiveByteFromSlave();
	i2cSendAck();
	readData.byte.low = i2cReceiveByteFromSlave();

  	i2cSendNak();
  	i2cStopCondition();

	return readData.word;
}

/**
********************************************************************************
* @brief    I2C ��� ���� ��������\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void i2cStartCondition(void)
{
	I2C_SCLK = 1;
	I2C_SDA = 1;
	delay1Clock();
	I2C_SDA = 0;
	delay5Clock();
	I2C_SCLK = 0;
}

/**
********************************************************************************
* @brief    I2C ��� ���� ��������\n
* @param    none
* @return   none
* @remark   Run Initail Time
********************************************************************************
*/
void i2cStopCondition(void)
{
  	I2C_SDA = 0;		
	delay2Clock();
  	I2C_SCLK = 1;		
	delay5Clock();
  	I2C_SDA = 1;
}

/**
********************************************************************************
* @brief    ACK��ȣ�� ��ٸ��� �Լ�\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void i2cAckPollFromSlave(void)
{
	INT8U ErrorCount = 0;
  	
	I2C_SDA = 1;		
	delay2Clock();
  	I2C_SCLK = 1;		
	delay4Clock();

  	while (I2C_SDA) 
	{	// time out check & error display
		if (ErrorCount > 50) 
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
* @brief    ACK ��ȣ�� ������ �Լ�\n
* @param    none
* @return   none
* @remark   Run Initail Time
********************************************************************************
*/
void i2cSendAck(void)
{
  	I2C_SDA = 0;		
	delay2Clock();
  	I2C_SCLK = 1;		
	delay4Clock();
  	I2C_SCLK = 0;
}

/**
********************************************************************************
* @brief    NAK ��ȣ�� ������ �Լ�\n
* @param    none
* @return   none
* @remark   Run Initail Time
********************************************************************************
*/
void i2cSendNak(void)
{
  	I2C_SDA = 1;		
	delay2Clock();
  	I2C_SCLK = 1;		
	delay4Clock();
  	I2C_SCLK = 0;
}

/**
********************************************************************************
* @brief    1 Byte data�� Slave device�� ������ �Լ�.\n
* @param    1 byte send data
* @return   none
* @remark   
********************************************************************************
*/
void i2cSendByteToSlave(INT8U sendData)
{				
  	INT8U i;
  	for (i = 0; i < 8; i++) 
	{ // MSB First 
		if (sendData & (0x80 >> i)) 
		{
			I2C_SDA = 1;	// data bit     high
		}
		else
		{
			I2C_SDA = 0;	// data bit     low
		}
		delay1Clock();
		I2C_SCLK = 1;	
		delay4Clock();
		I2C_SCLK = 0;
  	}
}

/**
********************************************************************************
* @brief    1 Byte data�� Slave device�� ���� �޴� �Լ�.\n
* @param    none
* @return   1 byte receive data
* @remark   
********************************************************************************
*/
INT8U i2cReceiveByteFromSlave(void)	// MSB First
{
  	INT8U byteData;
  	INT8U i;
	
  	byteData = 0;
  	I2C_SDA = 1;

  	for (i = 0; i < 8; i++) 
	{
		delay3Clock();
		I2C_SCLK = 1;	
		delay2Clock();
		if (I2C_SDA == 1)   
		{
			byteData |= (0x80 >> i);	// if data bit high then set high
		}
		I2C_SCLK = 0;
  	}
  	return byteData;
}


#endif//#ifdef TEMP_DRV_TCN75A
