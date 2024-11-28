/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Relay API Header File\n
* @file    RelayApi.h
* @version 1.0
* @date    2012/10/10
* @author  TAE WOO KOO (KTW)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2012/10/10 created by KTW \n
********************************************************************************
*/

#ifndef _RELAY_API_H
#define _RELAY_API_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */
/** @brief Relay 최대 개수 */
#define RELAY_MAX_NUM			3

/** @brief RELAY HIGH*/
#define RELAY_HIGH		0
/** @brief RELAY OK*/
#define RELAY_OK		1
/** @brief RELAY LOW*/
#define RELAY_LOW		2

/*
********************************************************************************
*                       GLOBAL DATA TYPES & STRUCTURES
********************************************************************************
*/
/* Insert global typedef & struct here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLE EXTERNALS
********************************************************************************
*/
/* Insert global variable externals here */


/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert global function prototypes here */
extern void RelayInit(void);
extern void RelaySet(INT8U num, INT8U on);

#endif /* _RELAY_API_H */
