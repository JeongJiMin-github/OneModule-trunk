/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Communication API Source File\n
* @file    CommApi.c
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
#include "../Common/RingBuf.h"
#include "../Common/AdData.h"
#include "../Common/SaleData.h"
#include "../Common/TimeData.h"
#include "../Common/UiDefine.h"
#include "../Application/ScaleModeApp.h"
#include "../Time/TimeApi.h"
#include "../Power/PowerApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../Ad/AdApi.h"
#include "../Ad/TempCompApi.h"
#include "../Ad/HystCompApi.h"
#include "../Ad/LinearCompApi.h"
#include "../Ad/CreepCompApi.h"
#include "CommDrv.h"
#include "CommApi.h"
#include "DebugApi.h"
#ifdef USE_CTS_MODE
#include "../application/CtsModeApp.h"
#endif


#include "../Hardware/HardwareDrv.h"//for HwDrvSetFactoryBoot
#ifdef DEBUG_TEMP
#include "../AD/TempDrv.h"
#endif

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
#define NONE_ERROR_CODE					0
#define READ_ERROR_CODE					1
#define WRITE_ERROR_CODE				2


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
/** @brief Factory Communication Interpreter의 상태 변수 */
static INT8U commStateF;

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
void commOmFunc(RING_BUF *txRingBufPtr, INT8U type, INT8U rw, INT32U addr, INT32S data32s);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/**
********************************************************************************
* @brief    
* @param    
* @return   
* @remark   TODO : 채워넣기
********************************************************************************
*/
void CommOmInit(void)
{
	commStateF = 0;
}
/**
********************************************************************************
* @brief    
* @param    
* @return   
* @remark   TODO : 채워넣기
********************************************************************************
*/
BOOLEAN CommOmInterpreter(RING_BUF *txRingBufPtr, char byte)
{
	static INT8U commDataSize;
	static INT8U commDataRw;
	static INT8U commDataType;
	static UNION_INT32U commDataAddr;
	static UNION_INT32U commData32s;
	static INT8U bcc;

	switch (commStateF)
	{
		case 0:
			switch (byte)
			{
				case 'D':		//for debugging
				case 'E':		//send byte   (1 byte)
				case 'S':		//send word   (2 bytes)
				case 'L':		//send Dword (4 bytes)
#ifdef USE_RTC					
				case 'C':		//send Time data (1 bytes)												
#endif
					commDataType = byte;
					bcc = byte;
					commDataRw = 0;
					commStateF++;
					break;
				default:
					commStateF = 0;
			}
			break;
		case 1:
			switch (byte)
			{
				case 'R':	
				case 'W':	
					commDataRw = byte;
					bcc ^= byte;
					commDataSize = 0;
					commDataAddr.dword = 0;
					commStateF++;
					break;
				default:
					commStateF = 0;
				}
			break;
		case 2:
			commDataAddr.byteArray.byte[commDataSize] = byte;
			bcc ^= byte;
			commDataSize++;
			if (commDataSize >= 4) 
			{
				commDataSize = 0;
				commData32s.dword = 0;
				commStateF++;
				if (commDataRw == 'R')
				{
					commStateF++;
				}
			}
			break;
		case 3:
			commData32s.byteArray.byte[commDataSize] = byte;
			bcc ^= byte;
			commDataSize++;
			if (commDataSize >= 4) 
			{
				commStateF++;
			}
			break;
		case 4:
			if (byte == bcc)
			{
				commStateF++;
			}
			else
			{
				commStateF = 0;
			}
			break;
		case 5:
			if (byte == 0x0D)
			{
				commOmFunc(txRingBufPtr, commDataType, commDataRw, (INT32S)commDataAddr.dword, (INT32S)commData32s.dword);
			}
		default:
			commStateF = 0;
			break;
	}

	if (commStateF == 0) 
	{
		return 0;
	}
	else
	{
		return 1;
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
* @brief    
* @param    
* @return   
* @remark   TODO : 채워넣기
********************************************************************************
*/
void commOmFunc(RING_BUF *txRingBufPtr, INT8U type, INT8U rw, INT32U addr, INT32S data32s)
{
	INT8U error;
	INT32S sendData;
	INT8U bcc;
	INT8U i;
	INT8U *bPtr;
	VERSION version;
#ifdef USE_CTS_MODE    
	INT8U count, offset;
	INT16U i16Buf;
	INT8U strBuf[17];
	INT32U cmdBuf;
#ifdef USE_CTS_OMV2	
	INT8U endian_convert = 0;
#endif		
#endif
	error = 0;
	if (type == 'D') 
	{
		if (rw == 'R') 
		{
			switch (addr)
			{
#ifdef DEBUG_AD_TEST
				case 0:
					sendData = (INT32S)AdData.temp;
					break;
				case 1:
					sendData = (INT32S)TempParam.highTemp;
					break;
				case 2:
					sendData = (INT32S)TempParam.midTemp;
					break;
				case 3:
					sendData = (INT32S)TempParam.lowTemp;
					break;
				case 4:
					sendData = (INT32S)TempParam.reCalTemp;
					break;
				case 5:
					sendData = TempParam.deltaZeroValueAtHighTemp;
					break;
				case 6:
					sendData = TempParam.reserved1;
					break;
				case 7:
					sendData = TempParam.deltaZeroValueAtLowTemp;
					break;
				case 8:
					sendData = CalParam.calZeroValue;
					break;
				case 9:
					sendData = TempParam.deltaSpanValueAtHighTemp;
					break;
				case 10:
					sendData = TempParam.reserved2;
					break;
				case 11:
					sendData = TempParam.deltaSpanValueAtLowTemp;
					break;
				case 12:
					sendData = CalParam.calSpanValue;
					break;
				case 13:
					sendData = (INT32S)TempParam.tempCompEnable;
					break;
				case 14:
					sendData = (INT32S)TempParam.calAvailable;
					break;
				case 15:
					sendData = (INT32S)HystParam.hystEnable;
					break;
				case 16:
					sendData = (INT32S)HystParam.hystZeroValue;
					break;
				case 17:
					sendData = (INT32S)HystParam.hystMidUpValue;
					break;
				case 18:
					sendData = (INT32S)HystParam.hystMidDownValue;
					break;
				case 19:
					sendData = (INT32S)HystParam.hystSpanValue;
					break;
				case 20:
					sendData = (INT32S)CreepParam.creepTime;
					break;
				case 21:
					sendData = (INT32S)CreepParam.creep;
					break;
				case 22:
					sendData = AdData.rawFilteredData;
					break;
				case 23:
					sendData = (INT32S)CalParam.initZeroResetEnable;
					break;
				case 101:
					AdSetDefaultParam(OIML);
					break;
#endif//#ifdef DEBUG_AD_TEST
				case 100:
					ParamReadData(PARAM_VERSION_AREA_NUM, 0, (INT8U *)&version, sizeof(version));
					if(KeyCheckCalPort() || (version.fwErasePwd == 0xaa))
					{
						HwDrvSetFactoryBoot();
						PowerOff();
					}
					break;
#ifdef USE_CTS_MODE
				// 모델 추가되면 case 추가할 것
				case CTS_ADDR_MODEL1:
				case CTS_ADDR_MODEL2:
				case CTS_ADDR_MODEL3:
				case CTS_ADDR_PROGRAM_VER:
				case CTS_ADDR_SERIALNUM1:
				case CTS_ADDR_SERIALNUM2:
				case CTS_ADDR_SERIALNUM3:
				case CTS_ADDR_SERIALNUM4:
				case CTS_ADDR_YEAR:
				case CTS_ADDR_MONTH:
				case CTS_ADDR_DATE:
				case CTS_ADDR_ZERO:
				case CTS_ADDR_MIDUP:
				case CTS_ADDR_MIDDOWN:
			    case CTS_ADDR_SPAN:
				case CTS_ADDR_CAPA:
				case CTS_ADDR_EVALUE:
				case CTS_ADDR_DVALUE:
				case CTS_ADDR_WDP:
				case CTS_ADDR_UNIT:
				case CTS_ADDR_CALG:
				case CTS_ADDR_USINGG:
				case CTS_ADDR_CTSENABLE:
				case CTS_ADDR_CTSPERIOD:
				case CTS_ADDR_CAL_STATE:
				case CTS_ADDR_CTS_VERSION:
				case CTS_ADDR_PRODUCT_CODE1:
				case CTS_ADDR_PRODUCT_CODE2:
				case CTS_ADDR_PRODUCT_CODE3:
				case CTS_ADDR_PRODUCT_CODE4:
				case CTS_ADDR_BOARDID:
				case CTS_ADDR_BCC_ERR:
					error = CtsCommFunc(rw, addr, data32s, &sendData);
#ifdef USE_CTS_OMV2	
					endian_convert = 1;
#endif
					break;
#endif
				default:
					error = 1;
					break;
			}
		} 
		else if (rw == 'W')
		{
			switch (addr)
			{
#ifdef DEBUG_AD_TEST
#ifdef DEBUG_TEMP
				case 0:
					TempDrvWriteDataForDebug((INT8S)data32s);
					AdData.temp = (INT8S)data32s;
					sendData = (INT32S)AdData.temp;
					break;
#endif//#ifdef DEBUG_TEMP
				case 1:
					TempParam.highTemp = (INT8S)data32s;
					sendData = (INT32S)TempParam.highTemp;
					break;
				case 2:
					TempParam.midTemp = (INT8S)data32s;
					sendData = (INT32S)TempParam.midTemp;
					break;
				case 3:
					TempParam.lowTemp = (INT8S)data32s;
					sendData = (INT32S)TempParam.lowTemp;
					break;
				case 4:
					TempParam.reCalTemp = (INT8S)data32s;
					sendData = (INT32S)TempParam.reCalTemp;
					break;
				case 5:
					TempParam.deltaZeroValueAtHighTemp = data32s;
					sendData = TempParam.deltaZeroValueAtHighTemp;
					break;
				case 6:
					TempParam.reserved1 = data32s;
					sendData = TempParam.reserved1;
					break;
				case 7:
					TempParam.deltaZeroValueAtLowTemp = data32s;
					sendData = TempParam.deltaZeroValueAtLowTemp;
					break;
				case 8:
					CalParam.calZeroValue = data32s;
					sendData = CalParam.calZeroValue;
					break;
				case 9:
					TempParam.deltaSpanValueAtHighTemp = data32s;
					sendData = TempParam.deltaSpanValueAtHighTemp;
					break;
				case 10:
					TempParam.reserved2 = data32s;
					sendData = TempParam.reserved2;
					break;
				case 11:
					TempParam.deltaSpanValueAtLowTemp = data32s;
					sendData = TempParam.deltaSpanValueAtLowTemp;
					break;
				case 12:
					CalParam.calSpanValue = data32s;
					sendData = CalParam.calSpanValue;
					break;
				case 13:
					TempParam.tempCompEnable = (INT8U)data32s;
					sendData = (INT32S)TempParam.tempCompEnable;
					break;
				case 14:
					TempParam.calAvailable = (INT8U)data32s;
					sendData = (INT32S)TempParam.calAvailable;
					break;
				case 15:
					HystParam.hystEnable = (INT8U)data32s;
					sendData = (INT32S)HystParam.hystEnable;
					break;
				case 16:
					HystParam.hystZeroValue = data32s;
					sendData = (INT32S)HystParam.hystZeroValue;
					LinearParam.linearZeroValue = HystParam.hystZeroValue;
					break;
				case 17:
					HystParam.hystMidUpValue = data32s;
					sendData = (INT32S)HystParam.hystMidUpValue;
					break;
				case 18:
					HystParam.hystMidDownValue = data32s;
					sendData = (INT32S)HystParam.hystMidDownValue;
					LinearParam.linearMidValue = HystParam.hystMidDownValue;
					break;
				case 19:
					HystParam.hystSpanValue = data32s;
					sendData = (INT32S)HystParam.hystSpanValue;
					LinearParam.linearSpanValue = HystParam.hystSpanValue;
					break;
				case 20:
					CreepParam.creepTime = (INT8U)data32s;
					sendData = (INT32S)CreepParam.creepTime;
					break;
				case 21:
					CreepParam.creep = (INT16S)data32s;
					sendData = (INT32S)CreepParam.creep;
					break;
#ifdef DEBUG_AD_API
				case 22:
					AdWriteDataForDebug(data32s);
					sendData = AdData.rawFilteredData;
					break;
#endif//#ifdef DEBUG_AD_API
				case 23:
					CalParam.initZeroResetEnable = (INT8U)data32s;
					sendData = (INT32S)CalParam.initZeroResetEnable;
					break;
#endif
#ifdef USE_CTS_MODE
				case CTS_ADDR_SERIALNUM1:
				case CTS_ADDR_SERIALNUM2:
				case CTS_ADDR_SERIALNUM3:
				case CTS_ADDR_SERIALNUM4:
#ifdef USE_CTS_OMV2
					reverse_array((INT8U *)&data32s,sizeof(INT32U));
					endian_convert = 1;
#endif					
					error = CtsCommFunc(rw, addr, data32s, &sendData);
					switch(addr)
					{
						case CTS_ADDR_SERIALNUM1:
							offset= 0;
							break;
						case CTS_ADDR_SERIALNUM2:
							offset= 4;
							break;
						case CTS_ADDR_SERIALNUM3:
							offset= 8;
							break;
						case CTS_ADDR_SERIALNUM4:
							offset= 12;
							break;
					}
					StorageWriteData(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_SERIAL_NUM + offset, &CtsStructBuf[CTS_STORAGE_OFFSET_SERIAL_NUM + offset], 4);
					StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_BCC, CtsStructBuf[CTS_STORAGE_OFFSET_BCC]);
					sendData = data32s;
					break;
				case CTS_ADDR_PRODUCT_CODE1:
				case CTS_ADDR_PRODUCT_CODE2:
				case CTS_ADDR_PRODUCT_CODE3:
				case CTS_ADDR_PRODUCT_CODE4:
#ifdef USE_CTS_OMV2
					reverse_array((INT8U *)&data32s,sizeof(INT32U));
					endian_convert = 1;
#endif					
					error = CtsCommFunc(rw, addr, data32s, &sendData);
					switch(addr)
					{
						case CTS_ADDR_PRODUCT_CODE1:
							offset= 0;
							break;
						case CTS_ADDR_PRODUCT_CODE2:
							offset= 4;
							break;
						case CTS_ADDR_PRODUCT_CODE3:
							offset= 8;
							break;
						case CTS_ADDR_PRODUCT_CODE4:
							offset= 12;
							break;
					}
					StorageWriteData(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_PRODUCT_CODE + offset, &CtsStructBuf[CTS_STORAGE_OFFSET_PRODUCT_CODE + offset], 4);
					StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_BCC, CtsStructBuf[CTS_STORAGE_OFFSET_BCC]);
					sendData = data32s;
					break;
				case CTS_ADDR_BOARDID:
#ifdef USE_CTS_OMV2
					reverse_array((INT8U *)&data32s,sizeof(INT32U));
					endian_convert = 1;
#endif					
					error = CtsCommFunc(rw, addr, data32s, &sendData);
					StorageWriteData(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_BOADRD_ID, &CtsStructBuf[CTS_STORAGE_OFFSET_BOADRD_ID], 4);
					StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_BCC, CtsStructBuf[CTS_STORAGE_OFFSET_BCC]);
					sendData = data32s;
					break;			
				case CTS_ADDR_ENCRYPTKEY:
#ifdef USE_CTS_OMV2
					reverse_array((INT8U *)&data32s,sizeof(INT32U));
					endian_convert = 1;
#endif					
					error = CtsCommFunc(rw, addr, data32s, &sendData);
					StorageWriteData(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_ENCRYPT_KEY, &CtsStructBuf[CTS_STORAGE_OFFSET_ENCRYPT_KEY], 4);
					StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_BCC, CtsStructBuf[CTS_STORAGE_OFFSET_BCC]);
					sendData = data32s;
					break;
				case CTS_ADDR_CTSCMD:
#ifdef USE_CTS_OMV2
					reverse_array((INT8U *)&data32s,sizeof(INT32U));
					endian_convert = 1;
#endif					
					error = CtsCommFunc(rw, addr, data32s, &sendData);
					if (!sendData)
					{
						StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_ENABLE, CtsStructBuf[CTS_STORAGE_OFFSET_ENABLE]);
						StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_CALYEAR, CtsStructBuf[CTS_STORAGE_OFFSET_CALYEAR]);
						StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_CALMONTH, CtsStructBuf[CTS_STORAGE_OFFSET_CALMONTH]);
						StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_CALDATE, CtsStructBuf[CTS_STORAGE_OFFSET_CALDATE]);
						StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_EXPIRED, CtsStructBuf[CTS_STORAGE_OFFSET_EXPIRED]);
						StorageWriteData(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_PERIOD, &CtsStructBuf[CTS_STORAGE_OFFSET_PERIOD], 2);
						StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_BCC, CtsStructBuf[CTS_STORAGE_OFFSET_BCC]);
					}
					break;
				case CTS_ADDR_RESET:
#ifdef USE_CTS_OMV2
					reverse_array((INT8U *)&data32s,sizeof(INT32U));
					endian_convert = 1;
#endif					
					error = CtsCommFunc(rw, addr, data32s, &sendData);
					if (!error)
					{
	                    PowerRestartFlag = 1; 
						sendData = data32s;
					}
					break;
				case CTS_ADDR_CTS_DATA_INIT:	// Test용 프로토콜. Trunk 머지 시 해당 프로토콜 삭제
#ifdef USE_CTS_OMV2
					reverse_array((INT8U *)&data32s,sizeof(INT32U));
					endian_convert = 1;
#endif					
					error = CtsCommFunc(rw, addr, data32s, &sendData);
					StorageWriteByte(STORAGE_CTS_DATA_BASE_ADDR + CTS_STORAGE_OFFSET_INIT, CtsStructBuf[CTS_STORAGE_OFFSET_INIT]);
                    PowerRestartFlag = 1; 
					sendData = data32s;
					break;
#endif
				default:
					error = 1;
					break;
			}
		}
		else
		{
			error = 1;
		}
#ifdef DEBUG_AD_TEST
		if (error == 0)
		{
			if (rw == 'W')
			{
				if (addr == 0 || addr == 22)
				{
					//none
				}
				else if (addr == 8 || addr == 12 || addr == 23)
				{
					CalWriteParam();
				}
				else if (addr >= 15 && addr <= 19)
				{
					HystWriteParam();
					LinearWriteParam();
				}
				else if (addr == 20 || addr == 21)
				{
#ifdef USE_CREEP_COMP
					CreepWriteParam();
#endif
				}
				else
				{
#if defined(USE_TEMP_COMP) || defined(USE_TEMP_COMP_MWIIH)
					TempWriteParam();
#endif
				}
			}
		}
#endif//#ifdef DEBUG_AD_TEST
	}
	else if (type == 'E') 
	{
		if (rw == 'R') 
		{
			sendData = (INT32S)StorageReadByte((INT16U)addr);
		}
		else if (rw == 'W')
		{
			 if (!(addr == PARAM_LOG_BASE_ADDR + LOG_PARAM_OFFSET_CAL_PASSWORD_ENABLE_KEY))
	    			StorageWriteByte((INT16U)addr, (INT8U)data32s);
			sendData = (INT32S)StorageReadByte((INT16U)addr);
			if ((addr >= PARAM_KEY_BASE_ADDR) && (addr < (PARAM_KEY_BASE_ADDR + PARAM_KEY_AREA_SIZE)))
			{
				KeySaveChecksum();//save checksum
			}
		}
		else
		{
			error = 1;
		}
	}
	else if (type == 'S') 
	{
		if (rw == 'R') 
		{
			sendData = (INT32S)StorageReadWord((INT16U)addr);
		}
		else if (rw == 'W')
		{
			StorageWriteWord((INT16U)addr, (INT16U)data32s);
			sendData = (INT32S)StorageReadWord((INT16U)addr);
		}
		else
		{
			error = 1;
		}
	}
	else if (type == 'L') 
	{
		if (rw == 'R') 
		{
			sendData = (INT32S)StorageReadDword((INT16U)addr);
		}
		else if (rw == 'W')
		{
			StorageWriteDword((INT16U)addr, (INT32U)data32s);
			sendData = (INT32S)StorageReadDword((INT16U)addr);
		}
		else
		{
			error = 1;
		}
	}
#ifdef USE_RTC
	else if (type == 'C') 
	{
		if (rw == 'R') 
		{
			TimeReadData();
#ifdef USE_CTS_MODE
			TimeVerify();
#endif
#ifdef USE_CTS_OMV2
			switch(addr)
			{
				case CTS_ADDR_CTS_TIME_SEC:
				case CTS_ADDR_CTS_TIME_MIN:
				case CTS_ADDR_CTS_DATA_HOUR:
				case CTS_ADDR_CTS_DATA_DATE:
				case CTS_ADDR_CTS_DATA_MONTH:
				case CTS_ADDR_CTS_DATA_DAY:
				case CTS_ADDR_CTS_DATA_YEAR:
					reverse_array((INT8U *)&addr,sizeof(INT32U));
					endian_convert = 1;
					break;
			}
#endif	
			sendData = (INT32S)(*((INT8U *)&TimeData + addr));
#ifdef USE_CTS_OMV2
			if(endian_convert)
				reverse_array((INT8U *)&addr,sizeof(INT32U));
#endif			
		}
		else if (rw == 'W')
		{
#ifdef USE_CTS_OMV2
			switch(addr)
			{
				case CTS_ADDR_CTS_TIME_SEC:
				case CTS_ADDR_CTS_TIME_MIN:
				case CTS_ADDR_CTS_DATA_HOUR:
				case CTS_ADDR_CTS_DATA_DATE:
				case CTS_ADDR_CTS_DATA_MONTH:
				case CTS_ADDR_CTS_DATA_DAY:
				case CTS_ADDR_CTS_DATA_YEAR:
					reverse_array((INT8U *)&addr,sizeof(INT32U));
					reverse_array((INT8U *)&data32s,sizeof(INT32U));
					endian_convert = 1;
					break;
			}
#endif			
			*((INT8U *)&TimeData + addr) = (INT8U)data32s;
			sendData = (INT32S)(*((INT8U *)&TimeData + addr));
#ifdef USE_CTS_OMV2
			if(endian_convert)
				reverse_array((INT8U *)&addr,sizeof(INT32U));
#endif				
			TimeWriteData();
		}
		else
		{
			error = 1;
		}
#ifdef USE_CTS_MODE
		if (rw == 'R' || rw == 'W') 
			CtsTimeSync((INT8U *)(&TimeData));
#endif
	}
#endif
	else
	{
		error = 1;
	}

	if (error == 0)
	{
		RingBufPutChar(txRingBufPtr, ESC);
		RingBufPutChar(txRingBufPtr, type);
		bcc = type;
		RingBufPutChar(txRingBufPtr, 'W');
		bcc ^= 'W';
		bPtr = (INT8U *)&addr;
		RingBufPutData(txRingBufPtr, bPtr, 4);
		for (i = 0; i < 4; i++)
		{
			bcc ^= bPtr[i];
		}
#ifdef USE_CTS_OMV2
		if(endian_convert)
		{
			reverse_array((INT8U *)&sendData,sizeof(INT32U));
		}
#endif
		bPtr = (INT8U *)&sendData;
		RingBufPutData(txRingBufPtr, bPtr, 4);
		for (i = 0; i < 4; i++)
		{
			bcc ^= bPtr[i];
		}
		RingBufPutChar(txRingBufPtr, bcc);
		RingBufPutChar(txRingBufPtr, 0x0D);
#ifdef USE_CTS_MODE
		if ( type == 'D' && (rw == 'W' || rw == 'R') &&  (addr >= CTS_ADDR_MODEL1 && addr <= CTS_ADDR_CTS_DATA_INIT))
		{
			while(RingBufCheckBuf(txRingBufPtr)) CommDrvProc();
		}
#endif
	}
}




