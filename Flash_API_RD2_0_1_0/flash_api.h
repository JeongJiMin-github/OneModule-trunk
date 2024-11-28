/****************************************************************************
*
*           (c) ATMEL-Wireless and Microcontrollers 2001
*
*
*****************************************************************************/

/*H***************************************************************************
* NAME: flash_api.h          
*----------------------------------------------------------------------------
* CREATED_BY:    Jean-sebastien Berthy && Raphaël L'etendu
* CREATION_DATE: 1/03/2000  
* AUTHOR:        $Author: jberthy $
* REVISION       $Revision: 1.1.1.1 $
* DATE:          $Date: 2001/03/02 10:11:24 $
*----------------------------------------------------------------------------
*****************************************************************************/


#ifndef _FLASH_API_H_
#define _FLASH_API_H_

/*_____ C O N F I G U R A T I O N ____________________________________________*/

/* define here the needed functions */
//#define  __API_FCT_SET_1
#define  __API_FCT_SET_2

//#define  __API_WR_CODE_BYTE
//#define  __API_WR_CODE_PAGE

//#define  __API_RD_EEPROM_BYTE
//#define  __API_WR_EEPROM_BYTE
//#define  __API_WR_EEPROM_PAGE


/******************************************************************************/
/******************************************************************************/
/***                                                                        ***/
/***       WARNING :TAKE CARE OF THESE TWO DEFINES                          ***/
/***                                                                        ***/
/******************************************************************************/
/******************************************************************************/

#define EETIM_VALUE     0x37  /* For exact EETIM value : see T8951RD2 Data Sheet
*/
                // Example :value of EETIM  mhz*5=0x37 @ 11 mhz = 0x3C @ 12 mhz 

/* IF NEEDED ( LARGE MEMORY MODEL USED ) uncomment this line : */

#define LARGE_MEMORY_MODEL


/*_____ END C O N F I G U R A T I O N
____________________________________________*/


#ifdef __API_FCT_SET_1
  Uchar __api_fct_set_1 (Uchar _R1, Uint16 _DPTR);

  #define __api_rd_manufacturer()         __api_fct_set_1(0, 0)
  #define __api_rd_device_id1()           __api_fct_set_1(0, 1)
  #define __api_rd_device_id2()           __api_fct_set_1(0, 2)
  #define __api_rd_device_id3()           __api_fct_set_1(0, 3)
  #define __api_erase_SBV()               __api_fct_set_1(4, 0)

  #define SSB_ALLOW_WR    ((Uchar)0xFE)       
  #define SSB_SECURITY_WR ((Uchar)0x00)   
  #define SSB_SECURITY_RD ((Uchar)0x01)

  #define __api_wr_SSB(SSB)               __api_fct_set_1(5, SSB)
                                
  #define __api_wr_SSB_RD_WR_SECURITY()   __api_wr_SSB(SSB_SECURITY_RD)
  #define __api_wr_SSB_WR_SECURITY()      __api_wr_SSB(SSB_SECURITY_WR)
  #define __api_wr_SSB_NO_SECURITY()      __api_wr_SSB(SSB_ALLOW_WR)

  #define __api_rd_SSB()                  __api_fct_set_1(7, 0)
  #define __api_rd_BSB()                  __api_fct_set_1(7, 1)
  #define __api_rd_SBV()                  __api_fct_set_1(7, 2)
  #define __api_rd_HSB()                  __api_fct_set_1(7, 3)
  #define __api_rd_bootloader_version()   __api_fct_set_1(8, 0)

#endif


#ifdef __API_FCT_SET_2
  Uchar __api_fct_set_2 (Uchar _ACC, Uchar _DPL);
  
  #define __api_wr_BSB(BSB)               __api_fct_set_2(BSB, 0)
  #define __api_wr_SBV(SBV)               __api_fct_set_2(SBV, 1)
            
#endif

#define __api_rd_code_byte(address)       (*((Uchar code*) (address)))

#ifdef  __API_WR_CODE_BYTE
  Uchar __api_wr_code_byte  (Uint16 , Uchar);
#endif

#ifdef  __API_WR_CODE_PAGE
  Uchar __api_wr_code_page  (Uint16 , Uint16, Uchar);
#endif


/*------------------------------- EEPROM -------------------------------*/

#define EEPROM_NOT_BUSY ((Uchar) 0)
#define EEPROM_BUSY     ((Uchar) 1)
  

#define  __api_eeprom_busy()  (EECON&1) 

#ifdef   __API_RD_EEPROM_BYTE
  Uchar    __api_rd_eeprom_byte     (Uint16);
#endif

#ifdef   __API_WR_EEPROM_BYTE
  Uchar  __api_wr_eeprom_byte  (Uint16, Uchar);
#endif

#ifdef  __API_WR_EEPROM_PAGE
  Uchar __api_wr_eeprom_page  (Uint16 , Uint16, Uchar);
#endif


#endif /* _FLASH_API_H_ */

