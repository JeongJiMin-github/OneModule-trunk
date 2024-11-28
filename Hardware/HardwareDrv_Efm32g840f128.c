/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Driver Source File\n
* @file    HardWareDrv_Efm32g840f128.c
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
//#include "../Flash_API_RD2_0_1_0/config.h"
#include "efm32_gpio.h"
#include "efm32_cmu.h"
#include "efm32_chip.h"
#include "efm32_system.h"
#include "HardwareConfig.h"

#ifdef MCU_EFM32G840F128

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief I2C Port 중복 사용을 제한하기 위한 Flag */
INT8U I2cPortUseFlag;

/** @brief 온도 보상에 따른 cpu 클럭을 변경할때 튜닝하는 값 */
unsigned int EfmTuning = 0;

/** @brief Key output port 상태, Display output port 와 공용 사용시 사용 */
INT8U  KeyOutputPortStatus;

INT8S Add_Top = 0;
INT8S Add_TIEMR1TOP = 0;

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

void mainClkSetting(INT8U mainClk, INT8U PeripheralClk);
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
void mainClkSetting(INT8U mainClk, INT8U PeripheralClk);

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
	SYSTEM_ChipRevision_TypeDef chipRev;

    CHIP_Init();
    /* Ensure Main clock 1Mhz */
    MODEL_CLK_INIT
    mainClkSetting(EFM32G840F128_MAINCLK,EFM32G840F128_PERCLK);
    /* Ensure LE modules are accessible */
    CMU_ClockEnable(cmuClock_CORELE, true);
    /* Enable clock for GPIO module */
    CMU_ClockEnable(cmuClock_GPIO, true);
    /* Enable clock for TIMER0 module */
    CMU_ClockEnable(cmuClock_TIMER0, true);
    /* Enable clock for TIMER1 module */
    CMU_ClockEnable(cmuClock_TIMER1, true);
    /* Enable clock for AD module */    
    CMU_ClockEnable(cmuClock_ADC0, true);

    
    /** @brief Init for only Module V3 */
    GPIO_PinModeSet(gpioPortC, 12, gpioModeWiredAnd, 0);   // eeprom sclk
    GPIO_PinModeSet(gpioPortC, 7, gpioModeWiredAnd, 1);   // eeprom data
    GPIO_PinModeSet(gpioPortB, 14, gpioModePushPull, 1);       //eeprom Power  
    GPIO_PinModeSet(gpioPortB, 12, gpioModeWiredAnd, 1);   // ads230 gain
    GPIO_PinModeSet(gpioPortA, 14, gpioModeWiredAnd, 0);   // ads230 speed
    GPIO_PinModeSet(gpioPortB, 11, gpioModeWiredAnd, 0);   // ads230 clk
    GPIO_PinModeSet(gpioPortD, 2, gpioModeInput, 0);   // ads230 tout
    GPIO_PinModeSet(gpioPortB, 13, gpioModeWiredAnd, 0);    //ads230 powerdown
    GPIO_PinModeSet(gpioPortD, 3, gpioModeInput, 0);       // batt check    
    GPIO_PinModeSet(gpioPortC, 13, gpioModeWiredAndPullUp, 1);   // RTC DIO
    GPIO_PinModeSet(gpioPortC, 14, gpioModeWiredAndPullUp, 1);       //RTC CLK 
   	EEPROM_POWERON;

    MODEL_PORT_INIT         // 외부에 붙는 포트설정은 각 모델별로 틀리기 때문에  hardwareConfig.h에서 define 

/*
Rev D & E May 28th, 2021
Updated the HFRCO 1 MHz band typical value to 1.2 MHz.
Updated the HFRCO 7 MHz band typical value to 6.6 MHz.
Other minor corrections. 
*/
    SYSTEM_ChipRevisionGet(&chipRev);

	if (chipRev.major == 0x01)
	{
		if ((chipRev.minor == 0x03) || (chipRev.minor == 0x04))
		{
            #if defined(USE_CLK_11MHZ)
                Add_Top = 0;
                Add_TIEMR1TOP = 0;
            #elif defined(USE_CLK_7MHZ)
                Add_Top = -6;
                Add_TIEMR1TOP = -4;
            #else
                Add_Top = 3;
                Add_TIEMR1TOP = 2;
            #endif
		}
	}    
/*Rev D */

// EEPROM CHECK DEVICE
	StorageDrvCheckDevice();
}

/**
********************************************************************************
* @brief    다음에 켤 때 Factory Boot가 무조건 실행하도록 하는 함수\n
* @param    none
* @return   nonec
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
/**
********************************************************************************
* @brief    설정하려고 하는 main clk 과 peripheral clk을 설정한다 
* @param    mainClk 사용하고자 하는 클럭을 설정 , PeripheralClk peripheral 클럭을 설정 
* @return   nonec
* @remark   main clk을 정하고 peripheral clk을 설정하면 그것에 따라서 클럭이 설정된다.
            EFM32G840F128_MODIFY : 초기 클럭중 클럭 오차율을 조정할때 쓴다. 
********************************************************************************
*/


void mainClkSetting(INT8U mainClk, INT8U PeripheralClk)
{
    CMU->HFPERCLKDIV = CMU_HFPERCLKDIV_HFPERCLKEN;
    CMU->OSCENCMD = CMU_OSCENCMD_HFRCOEN;

    switch (mainClk)
    {
        case 1:
            EfmTuning = (DEVINFO->HFRCOCAL0 & _DEVINFO_HFRCOCAL0_BAND1_MASK)
                   >> _DEVINFO_HFRCOCAL0_BAND1_SHIFT;
            EfmTuning = EfmTuning + EFM32G840F128_MODIFY;
            CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_1MHZ | EfmTuning;
            break;
        case 7:
            EfmTuning = (DEVINFO->HFRCOCAL0 & _DEVINFO_HFRCOCAL0_BAND7_MASK)
                   >> _DEVINFO_HFRCOCAL0_BAND7_SHIFT;
            EfmTuning = EfmTuning + EFM32G840F128_MODIFY;
            CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_7MHZ | EfmTuning;
            break;
        case 11:
            EfmTuning = (DEVINFO->HFRCOCAL0 & _DEVINFO_HFRCOCAL0_BAND11_MASK)
                   >> _DEVINFO_HFRCOCAL0_BAND11_SHIFT;
            EfmTuning = EfmTuning + EFM32G840F128_MODIFY;
            CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_11MHZ | EfmTuning;
            break;
        case 14:
            EfmTuning = (DEVINFO->HFRCOCAL0 & _DEVINFO_HFRCOCAL0_BAND14_MASK)
                   >> _DEVINFO_HFRCOCAL0_BAND14_SHIFT;
            EfmTuning = EfmTuning + EFM32G840F128_MODIFY;
            CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_14MHZ | EfmTuning;
            break;
        case 21:
            EfmTuning = (DEVINFO->HFRCOCAL0 & _DEVINFO_HFRCOCAL1_BAND21_MASK)
                   >> _DEVINFO_HFRCOCAL1_BAND21_SHIFT;
            EfmTuning = EfmTuning + EFM32G840F128_MODIFY;
            CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_21MHZ | EfmTuning;
            break;
        case 28:
            EfmTuning = (DEVINFO->HFRCOCAL0 & _DEVINFO_HFRCOCAL1_BAND28_MASK)
                   >> _DEVINFO_HFRCOCAL1_BAND28_SHIFT;
            EfmTuning = EfmTuning + EFM32G840F128_MODIFY;
            CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_28MHZ | EfmTuning;
            break;
        default:
            EfmTuning = (DEVINFO->HFRCOCAL0 & _DEVINFO_HFRCOCAL0_BAND1_MASK)
                   >> _DEVINFO_HFRCOCAL0_BAND1_SHIFT;
            EfmTuning = EfmTuning + EFM32G840F128_MODIFY;
            CMU->HFRCOCTRL = CMU_HFRCOCTRL_BAND_1MHZ | EfmTuning;
            break;
    }
    
	while (!(CMU->STATUS & CMU_STATUS_HFRCORDY)) ;
    CMU_ClockDivSet(cmuClock_HFPER, PeripheralClk);
}


/* Insert local functions here */


#endif //#ifdef CPU_AT89C51ED2
