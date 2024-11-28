/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Relay Driver Source File\n
* @file    RelayDrv.c
* @version 1.0
* @date    2012/10/10
* @author  TAE WOO KOO (KTW)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/10/10 created by KTW \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "RelayDrv.h"
#include "RelayApi.h"

#include "../Hardware/HardwareConfig.h"
#ifdef RELAY_DRV_STD
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
**
********************************************************************************
* @brief    Relayon/off setting 함수 (editting Page)
* @param    num : Relayon 번호\n
*           on : 1 -> 켜기, 0 -> 끄기
* @return   none
* @remark   Relayon 번호 정의되어 있음 (RELAY_HIGH...)
********************************************************************************
*/
void RelayDrvSet(INT8U num, INT8U on)
{
    switch(num)
    {
        case RELAY_HIGH:
          if (on)
          {
            RELAY_HIGH_ON;
          }
          else
          {
            RELAY_HIGH_OFF;
          }
        break;
        
        case RELAY_OK:
          if (on)
          {
            RELAY_OK_ON;
          }
          else
          {
            RELAY_OK_OFF;
          }
        break;
        
        case RELAY_LOW:
          if (on)
          {
            RELAY_LOW_ON;
          }
          else
          {
            RELAY_LOW_OFF;
          }
        break;
        
        default:
        break;  
    }
}
/*
********************************************************************************
* @brief    KR : Relay Drive 초기화 함수\n
*                 US : \n
*                 CN : \n
* @param    KR : \n
*                 US : \n
*                 CN : \n
* @return   KR : \n
*                 US : \n
*                 CN : \n
* @remark   KR : \n
*                 US : \n
*                 CN : \n
********************************************************************************
*/
void RelayDrvInit(void)
{
    RELAY_HIGH_OFF;
    RELAY_OK_OFF;
    RELAY_LOW_OFF;
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
#endif  // #endif RELAY_DRV_STD

