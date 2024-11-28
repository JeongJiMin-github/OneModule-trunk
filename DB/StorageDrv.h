/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Storage Drvier Header File\n
           Chip : AT24C16A
* @file    StorageDrv.h
* @version 1.0
* @date    2006/10/02
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/10/02 created by HYP \n
********************************************************************************
*/

#ifndef _STORAGE_DRV_H
#define _STORAGE_DRV_H

/*
********************************************************************************
*                       GLOBAL DEFINITIONS & MACROS
********************************************************************************
*/
/* Insert #define here */


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
extern void StorageDrvReadData(INT16U addr, INT8U *dst, INT16U size);
extern void StorageDrvWriteData(INT16U addr, INT8U *src, INT16U size);
extern void StorageDrvCheckDevice(void);

//extern void ExtStorageDrvReadByte(INT16U addr, INT8U *dst);
//extern void ExtStorageDrvWriteByte(INT16U addr, INT8U src);

#endif /* _STORAGE_DRV_H */
