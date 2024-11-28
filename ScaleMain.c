/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Scale Main Source File\n
* @file    ScaleMain.c
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
*          v1.1 2012/09/19 created by KDH \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "./Common/Globals.h"
#include "./Common/SaleData.h"
#include "./Common/AdData.h"
#include "./Common/PluData.h"
#include "./Common/UiDefine.h"
#include "./Common/RingBuf.h"
#include "./DB/StorageApi.h"
#include "./DB/ParamApi.h"
#include "./DB/PluApi.h"
#include "./SystemTimer/TimerApi.h"
#include "./Time/TimeApi.h"
#include "./UI/DispApi.h"
#include "./UI/KeyApi.h"
#include "./UI/BuzzerApi.h"
#include "./UI/EditorApi.h"
#include "./AD/AdApi.h"
#include "./Power/PowerApi.h"
#include "./Communication/CommApi.h"
#include "./Communication/PrtApi.h"
#include "./Communication/DebugApi.h"
#include "./Application/ScaleModeApp.h"

#include "./Hardware/HardwareDrv.h"

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

//=====================================================================================  
//                           MAIN  ROUTINE          
//
//=====================================================================================   
/**
********************************************************************************
* @brief    Scale Main 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/

int main(void)
{
    HwDrvInit();//init CPU
RESTART:
#if defined (USE_EFM32G_MCU)
    AdInit();       // ADinit 순서를 바꿈 power check시 A/D를 하기 때문
    PowerInit();//power on
    TimerInit();        // ad power check때문에 Timer를 먼저 살림 
    KeyInit();//waiting power key is unpressed
    DispInit();
    BuzzerInit();
    CommInit();
    #ifdef USE_PRINTER
        PrtInit();
    #endif
    EditorInit();
    TimeInit();			// end main init for efm32g (OMV3)
#elif defined (USE_STM8L151C8_MCU)
    AdInit();			// ADinit 순서를 바꿈 power check시 A/D를 하기 때문
    TimerInit();		// ad power check때문에 Timer를 먼저 살림	  
    DispInit();
    PowerInit();		//power on
    KeyInit();			//waiting power key is unpressed
    BuzzerInit();
    CommInit();
    #ifdef USE_PRINTER
        PrtInit();    
    #endif
    EditorInit();
    TimeInit();			// end main init for STM8L151C8
#elif defined (USE_M0516LDN_MCU)
    PowerInit();//power on
    KeyInit();
    AdInit();
#ifdef USE_AUTO_POWER_SETTING
	AutoPowerInit();
#endif
    DispInit();
    BuzzerInit();
    TimerInit();
    CommInit();
#ifdef USE_PRINTER
    PrtInit();
#endif
    EditorInit();
    TimeInit();			//  end main init for M0516LDN (OMV4)
#else //OMv2      
    PowerInit();//power on
#ifdef USE_AUTO_POWER_SETTING
	AutoPowerInit();
#endif
    KeyInit();//waiting power key is unpressed
    
    AdInit();
    DispInit();
    //KeyInit();//move up
    BuzzerInit();
    
    TimerInit();
    CommInit();
        
    #ifdef USE_PRINTER
        PrtInit();
    #endif
    #ifdef USE_DEBUG_API
        DebugInit();
    #endif
    EditorInit();
    TimeInit();
#endif      // end main init for 8051 (OMV2) 
	if (ScaleModeCheckVersion()) 
	{
		goto RESTART;
	}
#ifdef USE_CTS_MODE_OLD
    CtsModeInit();
#endif 
    ScaleModeInit();//last // ScaleModeInit is ScaleModeApp. 
    
#ifdef USE_BT_CONNECT 	
	Btreset();
#endif

    while (1)
    {
#ifdef USE_CTS_MODE_OLD
        CtsModeProc();
#elif defined(USE_CTS_MODE)
        ScaleModeCtsProc();
#endif
        PowerProc();
#if defined(USE_AC_POWER) || defined(USE_AC_BATT_POWER) || defined(USE_CTS_MODE)
        if(PowerRestartFlag)
          {
            PowerRestartFlag = 0;
            goto RESTART;   
          }
#endif
            
        CommProc();  // CommProc is ScaleModeApp. 
#ifdef USE_PRINTER
		PrtProc();
#endif
        AdProc(0, 0);
        EditorProc();
        ScaleModeProc();    // ScaleModeProc is ScaleModeApp. 
        DispProc();
#ifdef USE_POWER_SAVING_MODE
        PowerSavingMode(0);
#endif

    }
}


