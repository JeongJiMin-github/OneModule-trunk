/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Menu Api Source File\n
* @file    MenuApi.c
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
#include "../Common/UiDefine.h"
#include "../Communication/DebugApi.h"
#include "KeyApi.h"
#include "DispApi.h"
#include "EditorApi.h"
#include "MenuApi.h"


/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Menu function ������ ���Ǵ� state */
INT8U  MenuFuncState;

/** @brief Menu function ������ ���Ǵ� Enter key code */
KEY_TYPE MenuKeyEnter;
/** @brief Menu function ������ ���Ǵ� C key code */
KEY_TYPE MenuKeyC;
/** @brief Menu function ������ ���Ǵ� Menu Up key code */
KEY_TYPE MenuKeyMenuUp;
/** @brief Menu function ������ ���Ǵ� Menu Down key code */
KEY_TYPE MenuKeyMenuDown;

/** @brief String ����� Window ��ȣ */
INT8U  MenuStrWinNum;	
/** @brief Data ����� Window ��ȣ */
INT8U  MenuDataWinNum;
/** @brief �߰� ���� ����� Window ��ȣ */
INT8U  MenuAuxWinNum;
/** @brief Menu�� ǥ���� window�� 1������ ��Ÿ���� Flag (ex, SW = 1, RB = 0)*/
BOOLEAN  MenuWinIsOne;

/** @brief Menu���� ����� Key mode type */
INT8U MenuKeyMode;

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
/** @brief menu level�� ���� Mask table */
const INT16U code menuLevelMaskTable[MENU_MAX_LEVEL] =
{
	0xF000, 0xFF00, 0xFFF0, 0xFFFF
};

/** @brief menu level�� ���� Digit Mask table */
const INT16U code menuLevelDigitMaskTable[MENU_MAX_LEVEL] =
{
	0xF000, 0x0F00, 0x00F0, 0x000F
};

/** @brief menu level�� ���� �ּ� Digit Unit table */
const INT16U code menuLevelUnitTable[MENU_MAX_LEVEL] =
{
	0x1000, 0x0100, 0x0010, 0x0001
};

/** @brief ���� menu number */
static INT16U menuNum;
/** @brief ���� menu state */
static INT8U menuState;
/** @brief ���� menu table�� menu ���� */
static INT8U menuMaxPos;
/** @brief ���� menu table Pointer */
static MENU code *menuTablePtr;
/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static INT8U menuCheckLevel(void);
static INT8U menuSearchPos(void);
static INT8U menuIncNum(INT8U enableRot);
static INT8U menuDecNum(void);
static INT8U menuEnterNum(void);
static INT8U menuEscNum(void);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */
/**
********************************************************************************
* @brief    Menu Api Init �Լ�\n
*           Menu table, Key, Window �ʱ�ȭ\n
*           Menu mode �ʱ�ȭ\n
* @param    smode : Menu ������ ���
* @return   none
* @remark   Menu Application Procedure���� Menu�� ���� ����
*           �ʱ⿡ �ѹ� ���� �Ǵ� �Լ�\n
********************************************************************************
*/
void MenuInit(void)
{
	MenuKeyEnter = KeyGetPgmKeyByNum(KEY_MENU_ENTER_NUM);
	MenuKeyC = KeyGetPgmKeyByNum(KEY_MENU_C_NUM);
	MenuKeyMenuUp = KeyGetPgmKeyByNum(KEY_MENU_UP_NUM);
	MenuKeyMenuDown = KeyGetPgmKeyByNum(KEY_MENU_DOWN_NUM);

	MenuStrWinNum = DispGetMenuWinByNum(DISP_MENU_STR_WIN_NUM);
	MenuAuxWinNum = DispGetMenuWinByNum(DISP_MENU_AUX_WIN_NUM);
	MenuDataWinNum = DispGetMenuWinByNum(DISP_MENU_DATA_WIN_NUM);

	if (MenuStrWinNum == MenuDataWinNum)
	{
		MenuWinIsOne = 1;
	}
	else
	{
		MenuWinIsOne = 0;
	}
	MenuSetKeyMode(KEY_MODE_NORMAL);
}

/**
********************************************************************************
* @brief    Menu Set Mode �Լ�\n
*           Menu table �ʱ�ȭ\n
*           calibration, Program mode �ʱ�ȭ\n
* @param    smode : ������ ���
* @return   none
* @remark   Menu Application Procedure���� Menu�� ���� ����
********************************************************************************
*/
void MenuSetMode(INT16U mNum, INT8U mState, MENU code *mTablePtr, INT8U mMaxPos)
{
	MenuFuncState = 0;	

	menuNum = mNum;
	menuState = mState;
	menuTablePtr = mTablePtr;
	menuMaxPos = mMaxPos;
}

/**
********************************************************************************
* @brief    Menu Set Key Mode �Լ�\n
* @param    smode : ������ ���
* @return   none
* @remark   
********************************************************************************
*/
void MenuSetKeyMode(INT8U keyMode)
{
	MenuKeyMode = keyMode;
}

/**
********************************************************************************
* @brief    Menu Api Procedure �Լ�\n
* @param    void
* @return   MENU_RET_INCOMPLETE, MENU_RET_COMPLETE, MENU_RET_EXIT, MENU_RET_CAL_MODE 
* @remark   menu table�� ���� �����
*           ���� �ð��� �ѹ� ��� ����Ǵ� �Լ�\n
********************************************************************************
*/
INT8U MenuProc(void)
{
	//TODO : check optimize error
	static INT8U menuPos;//static is for prevent optimize error (don't erase if optimize is on)
	static INT8U menuMode;//static is for prevent optimize error (don't erase if optimize is on)
	static INT8U menuWinRefrash;//static is for prevent optimize error (don't erase if optimize is on)
	KEY_TYPE key;
	INT8U funcRetValue;
	INT16U prevMenuNum;

	menuWinRefrash = 0;
	menuPos = menuSearchPos();
	menuMode = menuTablePtr[menuPos].mode;

	funcRetValue = MENU_RET_INCOMPLETE;

	if (KeyCheck())
	{
		key = KeyGetFuncCode(MenuKeyMode);
	}
	else
	{
		key = KEY_NOT_PRESSED;
	}

	if (menuState == MENU_STATE_MENU)
	{
		DispSetConstStr(MenuStrWinNum, menuTablePtr[menuPos].menuStr);

		switch (menuMode)
		{
			case MENU_MODE_CONTI_EDIT_BLINK:
				menuWinRefrash = 2;
			case MENU_MODE_CONTI_EDIT_NO_BLINK:
			case MENU_MODE_RUN:
				menuState = MENU_STATE_RUN;
				break;
			default:					
//			case MENU_MODE_MENU:
//			case MENU_MODE_NORMAL:
//			case MENU_MODE_EDIT:
				if (key == MenuKeyEnter)
				{
					menuState = menuEnterNum();
				}
				else if (key == MenuKeyMenuUp)
				{
					menuState = menuIncNum(1);
				}
				else if (key == MenuKeyMenuDown)
				{
					menuState = menuDecNum();
				}
				else if (key == MenuKeyC)
				{
					menuState = menuEscNum();
				}
				break;
		}
		key = KEY_NOT_PRESSED;

		if (menuState == MENU_STATE_RUN)
		{
			menuPos = menuSearchPos();						
			funcRetValue = menuTablePtr[menuPos].func(key);			
		}
	}
	else 	//if (menuState == MENU_STATE_RUN)
	{		
		switch (menuMode)
		{
			case MENU_MODE_CONTI_EDIT_BLINK:
			case MENU_MODE_CONTI_EDIT_NO_BLINK:
				if (key == MenuKeyEnter )
				{
					funcRetValue = menuTablePtr[menuPos].func(key);
				}
				else if (key == MenuKeyMenuUp)
				{
					menuState = menuIncNum(1);
				}
				else if (key == MenuKeyMenuDown)
				{
					menuState = menuDecNum();
				}
				else if (key == MenuKeyC)
				{
					menuState = menuEscNum();
				}
				break;
			case MENU_MODE_EDIT:
				if (key == MenuKeyEnter)
				{
					funcRetValue = menuTablePtr[menuPos].func(key);
				}
				else if (key == MenuKeyMenuUp)
				{
					menuState = menuIncNum(1);
				}
				else if (key == MenuKeyMenuDown)
				{
					menuState = menuDecNum();
				}
				else if (key == MenuKeyC)
				{
					menuState = MENU_STATE_MENU;
				}
				break;
			default:
//			case MENU_MODE_RUN:
//			case MENU_MODE_NORMAL:
				funcRetValue = menuTablePtr[menuPos].func(key);
				break;
		}
		if (menuState == MENU_STATE_MENU)
		{
			menuWinRefrash = 1;
			MenuFuncState = MENU_RET_INCOMPLETE;
		}
	}

	switch (funcRetValue)
	{
		case MENU_RET_COMPLETE:
			menuState = MENU_STATE_MENU;
			MenuFuncState = 0;
			menuWinRefrash = 1;
			if (menuMode == MENU_MODE_CONTI_EDIT_BLINK || menuMode == MENU_MODE_CONTI_EDIT_NO_BLINK || menuMode == MENU_MODE_RUN)
			{
				prevMenuNum = menuNum;
				menuState = menuIncNum(0);
				if (prevMenuNum == menuNum)
				{
					menuState = menuEscNum();
				}
				else
				{
					if (menuMode == MENU_MODE_CONTI_EDIT_BLINK)
					{
						menuWinRefrash = 2;
					}
				}
			}
			break;
		case MENU_RET_EXIT:
		case MENU_RET_CAL_MODE:
			menuState = MENU_STATE_MENU;
			MenuFuncState = 0;
			break;
	}

	if (menuWinRefrash)
	{
		//Clear Data, Aux String
		DispClearAllDigits();
		menuPos = menuSearchPos();
		DispSetConstStr(MenuStrWinNum, menuTablePtr[menuPos].menuStr);
		if (menuWinRefrash == 2 && MenuWinIsOne == 1)
		{
			DispSetConstStrMsg(MenuStrWinNum, menuTablePtr[menuPos].menuStr);
			DispSetRunMsgPage(5, 1);
		}
	}

	return funcRetValue;
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    Menu level�� �˻��ϴ� �Լ�\n
* @param    none
* @return   level : 0x1000 -> 0, 0x1111 -> 3
* @remark   
********************************************************************************
*/
INT8U menuCheckLevel(void)
{
	INT8U i;
	
	for (i = 0; i < MENU_MAX_LEVEL; i++)
	{
		if ((menuNum & menuLevelDigitMaskTable[i]) == 0)
		{
			return (i - 1);
		}
	}
	return (i - 1);
}

/**
********************************************************************************
* @brief    menu table������ ��ġ�� �˻��ϴ� �Լ�\n
* @param    none
* @return   table offset number by menu number
* @remark   
********************************************************************************
*/
INT8U menuSearchPos(void)
{
	INT16U i;
	
	for (i = 0; i < menuMaxPos; i++)
	{
		if (menuNum == menuTablePtr[i].menuNum)
		{
			return i;
		}
	}
	return 0;
}

/**
********************************************************************************
* @brief    ���� level���� �޴� ����(Option : Rotation) \n
* @param    enableRot : 1 -> ������ �޴������� 1�� �޴��� ����(Rotation)
* @return   MENU_STATE_MENU, MENU_STATE_RUN
* @remark   
********************************************************************************
*/
INT8U menuIncNum(INT8U enableRot)
{
	INT16U i;
	INT16U retMenuNum;
	INT8U menuLevel;

	retMenuNum = menuNum;
	menuLevel = menuCheckLevel();
	retMenuNum = retMenuNum + menuLevelUnitTable[menuLevel];
	retMenuNum = retMenuNum & menuLevelMaskTable[menuLevel];
	for (i = 0; i < menuMaxPos; i++)
	{
		if (retMenuNum == menuTablePtr[i].menuNum)
		{
			menuNum = retMenuNum;						
			return MENU_STATE_MENU;
		}
	}
//rotate
	if (enableRot)
	{
		if (menuLevel > 0)
		{
			menuNum = (retMenuNum & menuLevelMaskTable[menuLevel - 1]) + menuLevelUnitTable[menuLevel];
		}
	}
	return MENU_STATE_MENU;
}

/**
********************************************************************************
* @brief    ���� level���� �޴� ����\n
* @param    none
* @return   MENU_STATE_MENU, MENU_STATE_RUN
* @remark   1 �� �޴������� Rotate ��
********************************************************************************
*/
INT8U menuDecNum(void)
{
	INT16U i;
	INT16U j;
	INT16U retMenuNum;
	INT8U menuLevel;

	retMenuNum = menuNum;
	menuLevel = menuCheckLevel();
	retMenuNum = retMenuNum - menuLevelUnitTable[menuLevel];
	retMenuNum = retMenuNum & menuLevelMaskTable[menuLevel];

	for (i = 0; i < menuMaxPos; i++)
	{
		if (retMenuNum == menuTablePtr[i].menuNum)
		{
			if (retMenuNum & menuLevelDigitMaskTable[menuLevel]) 	//!= 0
			{	
				menuNum = retMenuNum;
				return MENU_STATE_MENU;
			}
			else
			{
				break;
			}
		}
	}

	retMenuNum = retMenuNum + menuLevelDigitMaskTable[menuLevel];
	for (i = 0x0F; i > 0; i--)
	{
		for (j = 0; j < menuMaxPos; j++)
		{
			if (retMenuNum == menuTablePtr[j].menuNum)
			{
				menuNum = retMenuNum;
				return MENU_STATE_MENU;
			}
		}
		retMenuNum = retMenuNum - menuLevelUnitTable[menuLevel];
	}

	return MENU_STATE_MENU;
}

/**
********************************************************************************
* @brief    ���� menu ����\n
* @param    none
* @return   MENU_STATE_MENU, MENU_STATE_RUN
* @remark   
********************************************************************************
*/
INT8U menuEnterNum(void)
{
	INT8U menuPos;
	INT8U menuLevel;

	menuPos = menuSearchPos();
	menuLevel = menuCheckLevel();

	if (menuLevel < (MENU_MAX_LEVEL - 1))
	{
		if (menuTablePtr[menuPos].mode == MENU_MODE_MENU)
		{
			menuNum = menuNum + menuLevelUnitTable[menuLevel + 1];
			return MENU_STATE_MENU;
		}
		else
		{
			return MENU_STATE_RUN;
		}
	}
	else
	{
		return MENU_STATE_RUN;
	}
}

/**
********************************************************************************
* @brief    ���� �޴��� ���� ������\n
* @param    none
* @return   MENU_STATE_MENU
* @remark   
********************************************************************************
*/
INT8U menuEscNum(void)
{
	INT8U menuLevel;

	menuLevel = menuCheckLevel();

	if (menuLevel > 0)
	{
		menuLevel = menuLevel - 1;
		menuNum = menuNum & menuLevelMaskTable[menuLevel];
	}
	
	return MENU_STATE_MENU;
}
