/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Hardware Driver Source File\n
               chip:STM8L151C8
* @file    HardwareDrv_Stm8l151c8.c
* @version 1.0
* @date    2012/09/24
* @author  Young Hoon Lee (LYH)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/09/24 created by LYH \n
********************************************************************************
*/


/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "../STM8L151C8/stm8l15x_conf.h"
#include "../DB/StorageDrv.h"
#include "HardwareConfig.h"
#ifdef CPU_STM8L151C8
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

/**
********************************************************************************
* @brief    Hardware Dirver Init 함수\n
*           CPU Init\n
*           Hardware variable Init\n
* @param    none
* @return   none
* @remark   초기에 한번 실행 되는 함수\n
********************************************************************************
*/


void HwDrvInit(void)
{
	uint8_t FLASHTEMP;

#ifdef CLK1MHz
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_16); // SYSTEM CLOCK 1MHz
#endif
#ifdef CLK2MHz
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_8); // SYSTEM CLOCK 2MHz
#endif
#ifdef CLK16MHz
	CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1); // SYSTEM CLOCK 16MHz	
#endif	
	while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI)
	{}
	MODEL_PORT_INIT;
	GPIO_SetBits(GPIOA, GPIO_Pin_5);

	//Internal eeprom Init
	/* Define flash programming Time*/
	FLASH_SetProgrammingTime(FLASH_ProgramTime_Standard);
	/* Unlock flash data eeprom memory */
	FLASH_Unlock(FLASH_MemType_Data);
	/* Wait until Data EEPROM area unlocked flag is set*/
	while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
	{}		
	
	//Able to download firmware by rs232
	FLASHTEMP=FLASH_ReadByte(0x0000480B);
	if(FLASHTEMP != 0x55) 	FLASH_ProgramOptionByte(0x0000480B, 0x55);
	FLASHTEMP=FLASH_ReadByte(0x0000480C);	
	if(FLASHTEMP != 0xAA) 	FLASH_ProgramOptionByte(0x0000480C, 0xAA);
	
}

/**
********************************************************************************
* @brief    다음에 켤 때 Factory Boot가 무조건 실행하도록 하는 함수\n
* @param    none
* @return   none
* @remark   ENBOOT = 1, BSB = 0xFF, SBV = 0xFC 로 만든다.\n
********************************************************************************
*/
void HwDrvSetFactoryBoot(void)
{

}

/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
 
#endif //#ifdef CPU_STM8L151C8
