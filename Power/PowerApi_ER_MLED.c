/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Power API Source File\n
* @file    PowerApi.c
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
#include "../Common/AdData.h"
#include "../Common/UiDefine.h"
#include "../SystemTimer/TimerApi.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../UI/DispApi.h"
#include "../UI/KeyApi.h"
#include "../UI/BuzzerApi.h"
#include "../Common/SaleData.h"
#include "../Hardware/HardwareConfig.h"
#include "../Hardware/HardwareDrv.h"

#include "PowerDrv.h"
#include "PowerApi.h"
#include "BattDrv.h"

#ifdef POWER_DRV_ER_MLED
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
/** @brief BATT_CHECK_COUNT_MAX, ���� ms*/
#define BATT_CHECK_COUNT_MAX	10
/** @brief BATT_CHECK_CUT_OFF_COUNT_MAX, ���� sec*/
#define BATT_CHECK_CUT_OFF_COUNT_MAX	5

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
/** @brief Battery ���� check�� �ʱ⿡ ���� ��Ű�� Count (100ms)*/
static INT8U battCheckCount;
/** @brief Battery cut off voltage check Count */
static INT8U battCheckCutOffCount;
/** @brief Low battery�� ��� ���� ���� display�� ���� ���� time Delay check flag */
static BOOLEAN powerOffAfter2SecFlag;
/** @brief Power Saver�� Init�� PowerProc() ó�� ���� �ÿ� �ϱ����� Check flag */
static BOOLEAN powerSaverInitFlag;
/** @brief Low battery�� ��� Backlight�� off�ϴ� flag */
static BOOLEAN powerLowBattBLoffFlag;

extern INT8U displaySleep;

/** @brief Auto Power Off Mode�� ���� �ð� (���� : min), ��, 0 = Always on(=not use auto power off) */
INT8U	 PowerSaverAutoOffTime1min;
/** @brief LCD BL Off�� ���� �ð� (���� : sec), ��, 0 = initial off, 1 = always on */
INT8U	 PowerSaverBlOffTime1s;
/** @brief LED Suspend Mode�� ���� �ð� (���� : sec) 0 = always on(=not use standby mode)*/
INT8U	 PowerSaverStandByTime1s;
/** 

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
* @brief    Power API Parameter�� default ������ �ʱ�ȭ\n
* @param    type : defualt type
* @return   none
* @remark   Parameter ������ �ʱⰪ�� �ִ´�.\n
*           Cal Mode�� Program ��忡�� ��ü �ʱ�ȭ�� �� �����Ѵ�.
********************************************************************************
*/
void PowerSetDefaultParam(void)
{
	INT16U battAdOffset;

 	ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME, 0);
	ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BL_OFF_TIME, 0);
#ifdef USE_SUSPEND_MODE
	ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME, 60);
#else
	ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME, 0);
#endif
//	ParamWriteByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_LOWBAT_BL_OFF, 0);

	battAdOffset = 	ParamReadWord(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BATT_AD_OFFSET);
	if (!BattDrvCheckAdOffsetRange(battAdOffset))
	{
		ParamWriteWord(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BATT_AD_OFFSET, BattDrvGetDefaultAdOffset());
	}
}

/**
********************************************************************************
* @brief    Power API Init\n
* @param    none
* @return   none
* @remark   ������ �о ������ �ִ´�.\n
*           �ʱ⿡ ���� �Ǵ� �Լ�\n
********************************************************************************
*/
void PowerInit(void)
{
	PowerDrvPowerOn();
	PowerDrvInit();
	BattDrvInit(ParamReadWord(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BATT_AD_OFFSET));

//	while (POWER_KEY_CHK==0) {
//		POWER_KEY_CHK=1;
//	};	//wait Pwr key off

	battCheckCount = BATT_CHECK_COUNT_MAX;//���� PowerProc�� 100ms�� �ѹ��� �����Ѵٸ� 1��
	battCheckCutOffCount = 0;
	powerOffAfter2SecFlag = 0;
	powerSaverInitFlag = 0;	
	powerLowBattBLoffFlag = (BOOLEAN)ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_LOWBAT_BL_OFF);
}

/**
********************************************************************************
* @brief    Power API Procedure �Լ�\n
* @param    none
* @return   none
* @remark   ���� �ð��� �ѹ��� ��� ���� �Ǵ� �Լ�\n
********************************************************************************
*/
extern BOOLEAN BattChargeFlag;
extern BOOLEAN PowerCheckOnlyBattery();

void PowerProc(void)
{
	static INT16U prevPowerOffCmdTimer100ms = 0;
	static INT16U prevPowerTimer100ms = 0;
	static INT8U  prevBlStatus = 0;
	static INT8U  prevSuspendStatus = 0;
	static INT8U  prevDispOnOffStatus = 0;
	INT8U currBlStatus;
	INT8U currSuspendStatus;
	INT8U currDispOnOffStatus;
	INT16U currTimer100ms;
	INT8U tempVolt;
	INT16U batt_volt;

	currTimer100ms = TimerGetSysTimer100ms();

	//Check Power Key
//	if (KeyCheckPowerKey())
//	{
//		PowerOff();
//	}

	//Check BL status for checking Batt %
	currBlStatus = DispBlGetStatus();
	currSuspendStatus = DispGetSuspendStatus();
	currDispOnOffStatus = DispGetOnOffStatus();
	if ((prevBlStatus != currBlStatus) || 			   
	    (prevSuspendStatus != currSuspendStatus) ||    
		(prevDispOnOffStatus != currDispOnOffStatus))
	{
		BattDrvResetChargingRefVolt();
		prevBlStatus = currBlStatus;
		prevSuspendStatus = currSuspendStatus;
		prevDispOnOffStatus = currDispOnOffStatus;
	}

	BattDrvProc(currTimer100ms);

	if (prevPowerTimer100ms != currTimer100ms)
	{
		prevPowerTimer100ms = currTimer100ms;
		//100 ms�� �ѹ��� ����
		if (powerOffAfter2SecFlag)
		{
			if ((INT16U)(currTimer100ms - prevPowerOffCmdTimer100ms) > 20)
			{
				PowerOff();
			}
		}

		if (battCheckCount == 0)
		{
			battCheckCount = BATT_CHECK_COUNT_MAX;
			if (BattDrvCheckCutOff())
			{
				battCheckCutOffCount++;
			}
			else
			{
				battCheckCutOffCount = 0;
			}

//		if (!POWER_CHECK) {
			if (PowerCheckOnlyBattery()) {
				if ((powerOffAfter2SecFlag == 0) && (battCheckCutOffCount > BATT_CHECK_CUT_OFF_COUNT_MAX))
				{
					DispSetConstStrMsg(WIN_WEIGHT, "BATT");//TODO : change string
					DispSetRunMsgPage(10, 2);
					BuzzerSetOnTimes(2);
					prevPowerOffCmdTimer100ms = TimerGetSysTimer100ms();
					powerOffAfter2SecFlag = 1;
				}
			}
//			tempVolt=PowerGetUseablePercent();
			batt_volt=BattDrvReadVolt();
			if (PowerCheckOnlyBattery()) {
				tempVolt=(batt_volt - 560) * 100 / (655 - 560);
			} else {
				tempVolt=(batt_volt - 560) * 100 / (700 - 560);
			}
//			tempVolt=(batt_volt - 560) * 100 / (655 - 560);
			if (tempVolt<90) {
				if (!PowerCheckOnlyBattery()) {
					DispSetIndicator(INDICATOR_LOW_BATT, ON);
				} else {
					DispSetIndicator(INDICATOR_LOW_BATT, OFF);
				}
			} else {
				DispSetIndicator(INDICATOR_LOW_BATT,OFF);
			}

//			if (BattDrvCheckLow())
//			{
//				DispSetIndicator(INDICATOR_LOW_BATT, ON);
//				if (powerLowBattBLoffFlag) 
//				{
//					DispBlSetEnableFlag(DISABLE);
//				}
//				else
//				{
//					DispBlSetEnableFlag(ENABLE);
//				}
//			} else {
//				DispSetIndicator(20,OFF);
//				DispSetIndicator(INDICATOR_LOW_BATT, OFF);
//				DispBlSetEnableFlag(ENABLE);
//			}
			DispSetBattEnergy(BattDrvGetUseablePercent());
		}
		else
		{
			battCheckCount--;
		}

		if (powerSaverInitFlag == 0)
		{
			powerSaverInitFlag = 1;
			PowerSaverInit();
		}

		//Check Power Saver Reset Condition
		if (KeyCheckPressedFlag()) 
		{
			KeyResetPressedFlag();
			PowerSaverResetTimer(0xff);
		}
		else if (!AdData.stableFlag)
		{
			PowerSaverResetTimer(0xff);
		}
		else if (AdData.stableFlag && (!AdData.grossZeroFlag && !AdData.netZeroFlag))
		{
		 	PowerSaverResetTimer(TIMER_DELAY_100MS_STAND_BY);
		}

		//Check Power Saver Timer
		PowerSaverCheckPowerOffTimer();
		PowerSaverCheckBLOffTimer();
		PowerSaverCheckStandByTimer();
	}
}

/**
********************************************************************************
* @brief    Power Off �Լ�\n
* @param    none
* @return   none
* @remark   ������ ����.
********************************************************************************
*/
void PowerOff(void)
{
	BuzzerSetOnTimes(1);
	while (1)
	{
		PowerDrvPowerOff();
	}
}

/**
********************************************************************************
* @brief    Init Power Saver\n
* @param    none
* @return   none
* @remark   ������ �о ������ �ִ´�. \n
*           �ʱ⿡ ���� �Ǵ� �Լ�\n
********************************************************************************
*/
void PowerSaverInit(void)
{
	PowerSaverAutoOffTime1min = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_AUTO_OFF_TIME);
	PowerSaverSetBlOffTime1s(ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BL_OFF_TIME));
	PowerSaverStandByTime1s = ParamReadByte(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_STANDBY_TIME);
	PowerSaverResetTimer(0xff);

	//PowerSaverStandByTime1s = 3;//for test
}

/**
********************************************************************************
* @brief    Back Light Off time setting (���� : 1s)\n
* @param    blOffTime1s : �ڵ����� B/L �� �ð�
* @return   none
* @remark   Setting�� ���� BackLight ����
********************************************************************************
*/
void PowerSaverSetBlOffTime1s(INT8U blOffTime1s)
{
/*
PowerSaverBlOffTime1s = blOffTime1s;
	if (blOffTime1s == 0)
	{
		DispBlSetOnOff(OFF);
	}
	else
	{
		DispBlSetOnOff(ON);
	}
*/
}

/**
********************************************************************************
* @brief    init Power Saver Timer\n
* @param    timerNum : timer number, 0xff = all
* @return   none
* @remark   power save timer�� �ʱⰪ���� ����
********************************************************************************
*/
void PowerSaverResetTimer(INT8U timerNum)
{
	INT8U flag;

	switch (timerNum)
	{
		case 0xff:
			flag = 0x07;
			break;
		case TIMER_DELAY_100MS_POWER_OFF:
			flag = 0x01;
			break;
		case TIMER_DELAY_100MS_BL_OFF:
			flag = 0x02;
			break;
		case TIMER_DELAY_100MS_STAND_BY:
			flag = 0x04;
			break;
		default:
			flag = 0x00;
			break;
	}
//	if ((PowerSaverAutoOffTime1min != 0) && (flag&0x01))
//	{
//		TimerSetDelay100ms(TIMER_DELAY_100MS_POWER_OFF, (INT16U)PowerSaverAutoOffTime1min*600);
//	}
//	if ((PowerSaverBlOffTime1s != 0 || PowerSaverBlOffTime1s != 1) && (flag&0x02))
//	{
//		TimerSetDelay100ms(TIMER_DELAY_100MS_BL_OFF, (INT16U)PowerSaverBlOffTime1s*10);
//	}
	if ((PowerSaverStandByTime1s != 0) && (flag&0x04))
	{
		TimerSetDelay100ms(TIMER_DELAY_100MS_STAND_BY, (INT16U)PowerSaverStandByTime1s*10);
	}
}

/**
********************************************************************************
* @brief    check Power off Timer\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void PowerSaverCheckPowerOffTimer(void)
{
//	if (PowerSaverAutoOffTime1min == 0)
//	{
//		return;
//	}
//	if (TimerCheckDelay100ms(TIMER_DELAY_100MS_POWER_OFF))
//	{
//		PowerOff();
//	}
}

/**
********************************************************************************
* @brief    check Back Light Off Timer\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void PowerSaverCheckBLOffTimer(void)
{
/*
if (PowerSaverBlOffTime1s == 0)
	{
		return;
	}
	if (PowerSaverBlOffTime1s == 1)
	{
		return;
	}

	if (TimerCheckDelay100ms(TIMER_DELAY_100MS_BL_OFF))
	{
		DispBlSetOnOff(OFF);
	}
	else
	{
		DispBlSetOnOff(ON);
	}
*/
}

/**
********************************************************************************
* @brief    check Standby mode Timer\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void PowerSaverCheckStandByTimer(void)
{
	if (PowerSaverStandByTime1s == 0)
	{
		return;
	}

	if (TimerCheckDelay100ms(TIMER_DELAY_100MS_STAND_BY))
	{
//		DispSuspendOnOff(ON);
		displaySleep=1;
	}
	else
	{
//		DispSuspendOnOff(OFF);		
		displaySleep=0;
	}
}

/**
********************************************************************************
* @brief    Battery Read Voltage �Լ�\n
* @param    none
* @return   Battery Voltage �� (2 byte)
*           Battery Voltage �� x.xx �Ҽ����� 2�ڸ� (2 byte)
* @remark   
********************************************************************************
*/
INT16U PowerReadBattVolt(void)
{
	return BattDrvReadVolt();
}

/**
********************************************************************************
* @brief    Battery ���� �뷮 Check �Լ�\n
* @param   none
* @return   ���� �뷮 (0 ~ 100 %)
* @remark   
********************************************************************************
*/
INT8U PowerGetUseablePercent(void)
{
	return BattDrvGetUseablePercent();
}

/**
********************************************************************************
* @brief    setting current battery ad value is 5V\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void PowerCalBattOffset(void)
{
	INT32S tempInt32s;

	tempInt32s = BattDrvReadAd();
	ParamWriteWord(PARAM_POWER_AREA_NUM, POWER_PARAM_OFFSET_BATT_AD_OFFSET,(INT16U)tempInt32s);
	BattDrvSetAdOffset(tempInt32s);
}

/**
********************************************************************************
* @brief	battery cut off check timer\n
* @param	none
* @return	none
* @remark	battery cal �� ���
********************************************************************************
*/
void PowerResetBattCutOffTimer(void)
{
	battCheckCutOffCount = 0;
}

#endif//#ifdef POWER_DRV_ER_MLED
