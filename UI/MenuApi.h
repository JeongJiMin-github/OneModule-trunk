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
/** @brief Menu의 최대 진입 Level 
*   @remark 0x0000 ~ 0xffff 4자리를 뜻함\n
*	<MENU_MODE_MENU 사용 예>\n
*	{0x1320, "PGM32", MENU_MODE_MENU, 0},	//Enter 입력하면 0x1321 menu display, 순수 메뉴 모드, 함수 없음\n
*	{0x1321, "PGM321", MENU_MODE_EDIT, pgm1321Func}, //Enter입력 시 function 실행\n
*													//함수 완료 시 0x1321 Menu Display\n
*													//C key 입력시 0x1320으로 올라감\n
*													//Menu Up 입력시 다음 메뉴(0x1322) 실행\n
*	{0x1322, "PGM322", MENU_MODE_EDIT, pgm1322Func},//Enter입력 시 function 실행 \n
*													//함수 완료 시 0x1322 Menu Display\n
*													//C key 입력시 0x1320으로 올라감\n
*													//Menu Down 입력시 이전 메뉴(0x1321) 실행\n
*   \n
*	<MENU_MODE_CONTI_EDIT 사용 예>\n
*	{0x1320, "PGM32", MENU_MODE_MENU, 0},	//Enter 입력하면 0x1321 실행, 순수 메뉴 모드, 함수 없음\n
*	{0x1321, "PGM321", MENU_MODE_CONTI_EDIT_BLINK, pgm1321Func}, //editor 끝나면 아래 실행\n
*	{0x1322, "PGM322", MENU_MODE_CONTI_EDIT_BLINK, pgm1322Func}, //editor 끝나면 아래 실행\n
*	{0x1323, "PGM322", MENU_MODE_CONTI_EDIT_BLINK, pgm1323Func}  //editor 끝나면 0x1320 menu display\n
*/
#define MENU_MAX_LEVEL	4

/** @brief Menu Mode : Editor를 사용하는 모드 (MENU_EDITOR_INIT_STATE, MENU_EDITOR_INIT_STATE두가지 state만 사용) */
#define MENU_MODE_EDIT		0
/** @brief Menu Mode : 순수 Menu 모드 */
#define MENU_MODE_MENU		1
/** @brief Menu Mode : 일반 모드 : 함수에 진입하면 계속 실행됨, state를 사용하여 순차적으로 처리 해야함 */
#define MENU_MODE_NORMAL	2
/** @brief Menu Mode : 연속 Edit 모드 : Editor가 끝나면 다음 메뉴 바로 실행 (window가 1개이면 menu blink)*/
#define MENU_MODE_CONTI_EDIT_BLINK	3
/** @brief Menu Mode : 연속 Edit 모드 : Editor가 끝나면 다음 메뉴 바로 실행 (window가 1개여도 menu blink 하지 않음)*/
#define MENU_MODE_CONTI_EDIT_NO_BLINK	4
/** @brief Menu Mode : 계속 실행 모드 : Exit 함수 실행 위해 사용 */
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

/** @brief Menu function return value : 미완료 */
#define MENU_RET_INCOMPLETE	0
/** @brief Menu function return value : 완료 */
#define MENU_RET_COMPLETE	1
/** @brief Menu function return value : Sale Mode로 전환 명령 */
#define MENU_RET_EXIT		2
/** @brief Menu function return value : Cal Mode로 전환 명령 */
#define MENU_RET_CAL_MODE	3

/** @brief Menu State : Menu를 Display하고 key 대기하는 중 */
#define MENU_STATE_MENU	0
/** @brief Menu State : Menu의 function을 실행 중 */
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
	/** @brief menuNum 4자리 Hex값 (0x0000 ~ 0xffff) */	
	INT16U menuNum;

	/** @brief menu 이름, display 할 string */	
	char menuStr[7];

	/** @brief menu mode (see MENU_MODE_EDIT...)*/	
	INT8U mode;

	/** @brief menu에서 실행할 function */
	INT8U (*func) (KEY_TYPE key);

} MENU;

/** @brief Direct Menu Struct */
typedef struct
{
	/** @brief Direct Menu Key */	
	KEY_TYPE menuKey;

	/** @brief menuNum 4자리 Hex값 (0x0000 ~ 0xffff) */	
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