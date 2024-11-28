/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Menu Api Header File\n
* @file    MenuApi.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _MENU_API_H
#define _MENU_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Menu�� �ִ� ���� Level 
*   @remark 0x0000 ~ 0xffff 4�ڸ��� ����\n
*	<MENU_MODE_MENU ��� ��>\n
*	{0x1320, "PGM32", MENU_MODE_MENU, 0},	//Enter �Է��ϸ� 0x1321 menu display, ���� �޴� ���, �Լ� ����\n
*	{0x1321, "PGM321", MENU_MODE_EDIT, pgm1321Func}, //Enter�Է� �� function ����\n
*													//�Լ� �Ϸ� �� 0x1321 Menu Display\n
*													//C key �Է½� 0x1320���� �ö�\n
*													//Menu Up �Է½� ���� �޴�(0x1322) ����\n
*	{0x1322, "PGM322", MENU_MODE_EDIT, pgm1322Func},//Enter�Է� �� function ���� \n
*													//�Լ� �Ϸ� �� 0x1322 Menu Display\n
*													//C key �Է½� 0x1320���� �ö�\n
*													//Menu Down �Է½� ���� �޴�(0x1321) ����\n
*   \n
*	<MENU_MODE_CONTI_EDIT ��� ��>\n
*	{0x1320, "PGM32", MENU_MODE_MENU, 0},	//Enter �Է��ϸ� 0x1321 ����, ���� �޴� ���, �Լ� ����\n
*	{0x1321, "PGM321", MENU_MODE_CONTI_EDIT_BLINK, pgm1321Func}, //editor ������ �Ʒ� ����\n
*	{0x1322, "PGM322", MENU_MODE_CONTI_EDIT_BLINK, pgm1322Func}, //editor ������ �Ʒ� ����\n
*	{0x1323, "PGM322", MENU_MODE_CONTI_EDIT_BLINK, pgm1323Func}  //editor ������ 0x1320 menu display\n
*/
#define MENU_MAX_LEVEL	4

/** @brief Menu Mode : Editor�� ����ϴ� ��� (MENU_EDITOR_INIT_STATE, MENU_EDITOR_INIT_STATE�ΰ��� state�� ���) */
#define MENU_MODE_EDIT		0
/** @brief Menu Mode : ���� Menu ��� */
#define MENU_MODE_MENU		1
/** @brief Menu Mode : �Ϲ� ��� : �Լ��� �����ϸ� ��� �����, state�� ����Ͽ� ���������� ó�� �ؾ��� */
#define MENU_MODE_NORMAL	2
/** @brief Menu Mode : ���� Edit ��� : Editor�� ������ ���� �޴� �ٷ� ���� (window�� 1���̸� menu blink)*/
#define MENU_MODE_CONTI_EDIT_BLINK	3
/** @brief Menu Mode : ���� Edit ��� : Editor�� ������ ���� �޴� �ٷ� ���� (window�� 1������ menu blink ���� ����)*/
#define MENU_MODE_CONTI_EDIT_NO_BLINK	4
/** @brief Menu Mode : ��� ���� ��� : Exit �Լ� ���� ���� ��� */
#define MENU_MODE_RUN		5


/** @brief Program mode root menu Number */
#define MENU_PGM_ROOT_NUM	0x1100
/** @brief Calibration mode root menu Number */
#define MENU_CAL_ROOT_NUM	0x8100
/** @brief Calibration mode check password menu Number */
#define MENU_CAL_CHECK_PWD_NUM	0xF100

/** @brief MENU_MODE_EDIT menu function state : Init&run editor */
#define MENU_EDITOR_INIT_STATE	0
/** @brief MENU_MODE_EDIT menu function state : editor complete and process result */
#define MENU_EDITOR_COMPLETE_STATE	1

/** @brief Menu function return value : �̿Ϸ� */
#define MENU_RET_INCOMPLETE	0
/** @brief Menu function return value : �Ϸ� */
#define MENU_RET_COMPLETE	1
/** @brief Menu function return value : Sale Mode�� ��ȯ ��� */
#define MENU_RET_EXIT		2
/** @brief Menu function return value : Cal Mode�� ��ȯ ��� */
#define MENU_RET_CAL_MODE	3

/** @brief Menu State : Menu�� Display�ϰ� key ����ϴ� �� */
#define MENU_STATE_MENU	0
/** @brief Menu State : Menu�� function�� ���� �� */
#define MENU_STATE_RUN	1
/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */
/** @brief Menu Struct */
typedef struct
{
	/** @brief menuNum 4�ڸ� Hex�� (0x0000 ~ 0xffff) */	
	INT16U menuNum;

	/** @brief menu �̸�, display �� string */	
	char menuStr[7];

	/** @brief menu mode (see MENU_MODE_EDIT...)*/	
	INT8U mode;

	/** @brief menu���� ������ function */
	INT8U (*func) (KEY_TYPE key);

} MENU;

/** @brief Direct Menu Struct */
typedef struct
{
	/** @brief Direct Menu Key */	
	KEY_TYPE menuKey;

	/** @brief menuNum 4�ڸ� Hex�� (0x0000 ~ 0xffff) */	
	INT16U menuNum;
} DIRECT_MENU;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */
extern INT8U MenuFuncState;

extern KEY_TYPE MenuKeyEnter;
extern KEY_TYPE MenuKeyC;
extern KEY_TYPE MenuKeyMenuUp;
extern KEY_TYPE MenuKeyMenuDown;

extern INT8U MenuStrWinNum;
extern INT8U MenuDataWinNum;
extern INT8U MenuAuxWinNum;
extern BOOLEAN MenuWinIsOne;

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void MenuInit(void);
extern void MenuSetMode(INT16U menuNum, INT8U menuState, MENU code *menuTablePtr, INT8U menuMaxPos);
extern void MenuSetKeyMode(INT8U keyMode);
extern INT8U MenuProc(void);

#endif /* _MENU_API_H */