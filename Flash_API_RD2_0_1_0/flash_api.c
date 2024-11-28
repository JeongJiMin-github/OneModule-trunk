/*****************************************************************************
*
*           (c) ATMEL-Wireless and Microcontrollers 2000 
*
*
******************************************************************************/

/*C***************************************************************************
* NAME: flash_api.c  
*----------------------------------------------------------------------------
* CREATED_BY:    Jean-sebastien Berthy && Raphaël L'etendu
* CREATION_DATE: 1/03/2001 
* AUTHOR:        $Author: jberthy $
* REVISION       $Revision: 1.1.1.1 $
* DATE:          $Date: 2001/03/02 10:11:24 $
*----------------------------------------------------------------------------*/



/*_____ I N C L U D E - F I L E S ____________________________________________*/
#include <reg51xd2.h>//inserted for One Module CPU
#include "config.h"


/*_____ G L O B A L S ________________________________________________________*/

#ifdef LARGE_MEMORY_MODEL
  Uint16 data data_addr_xram;
  Uint16 data data_addr_flash;
  Uchar data data_nb_data;
#endif

Uchar data  data_value;

/*_____ P R I V A T E - F U N C T I O N S - D E C L A R A T I O N ____________*/


/*_____ L O C A L S __________________________________________________________*/

/*_____ EXTERNAL - F U N C T I O N S - D E C L A R A T I O N ____________*/

extern void ASM_MOV_R1_A(void);
extern void __API_FLASH_ENTRY_POINT(void);

/*F**************************************************************************
* NAME: __api_wr_data_byte 
*----------------------------------------------------------------------------
* AUTHOR: Jean-sebastien Berthy && Raphaël L'etendu
*----------------------------------------------------------------------------
* PARAMS:  
*     Uint16 address : address to program   
*     Uchar value : data to write
*     Uchar return : 
*             return  = 0x00 -> pass                            
*             return != 0x00 -> fail
*----------------------------------------------------------------------------
* PURPOSE: 
*  Program data byte in Flash memory 
*
*****************************************************************************/
#ifdef __API_WR_CODE_BYTE
Uchar __api_wr_code_byte (Uint16 address, Uchar value)
{
  #ifdef LARGE_MEMORY_MODEL
    data_addr_flash = address;
    data_value = value;
    DPTR = data_addr_flash;
    ACC = 0x02;
    ASM_MOV_R1_A();
    ACC = data_value;   
    __API_FLASH_ENTRY_POINT();
    return (ACC); 
  #else
    DPTR = address;    
    ACC = 0x02;
    ASM_MOV_R1_A();
    ACC = value;  
    __API_FLASH_ENTRY_POINT();
    return (ACC); 
#endif


}
#endif

/*F**************************************************************************
* NAME: __api_wr_code_page 
*----------------------------------------------------------------------------
* AUTHOR: Jean-sebastien Berthy && Raphaël L'etendu
*----------------------------------------------------------------------------
* PARAMS:  
*     Uint16 add_flash : address of the first byte to program in the Flash
*     Uint16 add_xram  : address in XRAM of the first data to program
*     Uchar nb_data : number of bytes to program
*     Uchar return : 
*                   return = 0x00 -> pass                           
*                   return != 0x00 -> fail 
*----------------------------------------------------------------------------
* PURPOSE: 
*  Program until 128 Datas in Flash memory.
* Number of bytes to program is limited such as the Flash write remains in a
* single 128 bytes page.               
*
*****************************************************************************/
#ifdef __API_WR_CODE_PAGE
Uchar __api_wr_code_page (Uint16 add_flash, Uint16 add_xram, Uchar nb_data)
{
#ifdef LARGE_MEMORY_MODEL
  data_addr_flash = add_flash;
  data_addr_xram = add_xram;
  data_nb_data = nb_data;
  AUXR1 &= ~0x01;        /* Set DPTR=DPTR0 */
  DPTR = data_addr_flash;
  AUXR1++;        /* DPTR=DPTR1 */
  DPTR = data_addr_xram;
  ACC = 0x09;
  ASM_MOV_R1_A();
  ACC = data_nb_data;     /* Number of bytes to program */  
    __API_FLASH_ENTRY_POINT();
  AUXR1++;        /* Set DPTR=DPTR0 */
  return (ACC);
#else
  AUXR1 &= ~0x01;        /* Set DPTR=DPTR0 */
  DPTR = add_flash;
  AUXR1++;        /* DPTR=DPTR1 */
  DPTR = add_xram;
  ACC = 0x09;
  ASM_MOV_R1_A();
  ACC = nb_data;
  __API_FLASH_ENTRY_POINT();
  AUXR1++;        /* Set DPTR=DPTR0 */
return (ACC);
#endif

}
#endif


/*F**************************************************************************
* NAME: __api_fct_set_1 
*----------------------------------------------------------------------------
* AUTHOR: Jean-sebastien Berthy && Raphaël L'etendu 
*----------------------------------------------------------------------------
* PARAMS:  
*     Uchar _R1 : 
*     Uint16 _DPTR : 
*     Uchar return : 
*****************************************************************************/
#ifdef __API_FCT_SET_1
Uchar __api_fct_set_1 (Uchar _R1, Uint16 _DPTR)
{
#ifdef LARGE_MEMORY_MODEL
  data_value = _R1;
  data_addr_flash = _DPTR;
  DPTR = data_addr_flash;
  ACC = data_value;
  ASM_MOV_R1_A();
  __API_FLASH_ENTRY_POINT();
  return (ACC);
#else
  DPTR = _DPTR;
  ACC = _R1;
  ASM_MOV_R1_A();
  __API_FLASH_ENTRY_POINT();
  return (ACC);
#endif
}
#endif


/*F**************************************************************************
* NAME: __api_fct_set_2
*----------------------------------------------------------------------------
* AUTHOR: Jean-sebastien Berthy && Raphaël L'etendu 
*----------------------------------------------------------------------------
* PARAMS:  
*     Uchar _ACC : 
*     Uchar _DPL : 
*     Uchar return : 
*
*****************************************************************************/
#ifdef __API_FCT_SET_2
Uchar __api_fct_set_2 (Uchar _ACC, Uchar _DPL)
{
#ifdef LARGE_MEMORY_MODEL
  data_value = _ACC;
  data_nb_data = _DPL;
  DPH = 0x00;
  DPL = data_nb_data;
  ACC = 0x06;
  ASM_MOV_R1_A();
  ACC = data_value;
  __API_FLASH_ENTRY_POINT();
#else
  DPH = 0x00;
  DPL = _DPL;
  ACC = 0x06;
  ASM_MOV_R1_A();
  ACC = _ACC;
  __API_FLASH_ENTRY_POINT();
#endif
  return 1;
}
#endif


/*F**************************************************************************
* NAME: api_rd_eeprom_byte
*----------------------------------------------------------------------------
* AUTHOR: Jean-sebastien Berthy && Raphaël L'etendu 
*----------------------------------------------------------------------------
* PARAMS:  
      Uint16 address :
*     Uchar return : 
*----------------------------------------------------------------------------
* PURPOSE: Read a byte in Eeprom
* 
*****************************************************************************/
#ifdef __API_RD_EEPROM_BYTE
Uchar __api_rd_eeprom_byte(Uint16 address)
{  
  register Uchar data_value;
  bit ea_save;
  ea_save = EA;
  EA = 0;
  EECON |= 0x02;
  data_value = *((Uchar xdata*) address);  
  EECON--;        // &= 0xFD;
  EA = ea_save;    // restore interrupt state
  return data_value;
}
#endif

/*F**************************************************************************
* NAME: api_wr_eeprom_byte
*----------------------------------------------------------------------------
* AUTHOR: Jean-sebastien Berthy && Raphaël L'etendu 
*----------------------------------------------------------------------------
* PARAMS:  
*     Uint16 address :
*     Uchar value :
*     Uchar return : 
*----------------------------------------------------------------------------
* PURPOSE: Program a byte in Eeprom
*
*****************************************************************************/
#ifdef __API_WR_EEPROM_BYTE
Uchar __api_wr_eeprom_byte(Uint16 address, Uchar value)
{
  bit ea_save;
  ea_save = EA;
  EETIM = EETIM_VALUE;
  EA = 0;
  EECON |= 0x02;
  *((Uchar xdata*)address) = value;/* addr is a pointer to external data mem */
  EECON =  0x50;  // write 5x, clear 2nd LSb
  EECON ^= 0x50;  // write Ax         
  EA = ea_save;     // restore interrupt state
  return 1;
}
#endif


/*F**************************************************************************
* NAME: __api_wr_eeprom_page
*----------------------------------------------------------------------------
* AUTHOR: Jean-sebastien Berthy && Raphaël L'etendu 
*----------------------------------------------------------------------------
* PARAMS:  
*     Uint16 add_eeprom : address of the first byte to program in the Eeprom
*     Uint16 add_xram  : address in XRAM of the first data to program
*     Uchar nb_data : number of bytes to program
*     Uchar return :
*----------------------------------------------------------------------------
* PURPOSE: Program until 64 bytes in Eeprom memory.
* Number of bytes to program is limited such as the Eeprom write remains in a
* single 64 bytes page. 
*
*****************************************************************************/
#ifdef __API_WR_EEPROM_PAGE
Uchar __api_wr_eeprom_page (Uint16 add_eeprom, Uint16 add_xram, Uchar nb_data)
{
bit ea_save;

#ifdef LARGE_MEMORY_MODEL
  //init
  data_addr_flash = add_eeprom;
  data_addr_xram  = add_xram;
  data_nb_data = nb_data;
  ea_save = EA;
  EA = 0;
  
  // write column latch
  DPTR = data_addr_xram;     
  AUXR1++;        /* DPTR=DPTR1 */
  DPTR = data_addr_flash;
  AUXR1++;
  for ( ; data_nb_data; data_nb_data--)
  {
    ACC = *((Uchar xdata*)DPTR);
    AUXR1++;
    EECON |= 0x02;
    *((Uchar xdata*)DPTR) = ACC;
    DPL++;
    EECON--;
    AUXR1++;
    DPTR++;
  }
  // start programmation
  EETIM = EETIM_VALUE;
  EECON = 0x50; // write 5x, clear 2nd LSb
  EECON ^= 0x50; // write Ax         
  // restore
  
  EA = ea_save;  // restore interrupt state
#else
  ea_save = EA;
  EA = 0;
  // write column latch
  DPTR = add_xram;     
  AUXR1++;        /* DPTR=DPTR1 */
  DPTR = add_eeprom;
  AUXR1++;
  for ( ; nb_data; nb_data--)
  {
    ACC = *((Uchar xdata*)DPTR);
    AUXR1++;
    EECON |= 0x02;
    *((Uchar xdata*)DPTR) = ACC;
    DPL++;
    EECON--;
    AUXR1++;
    DPTR++;
  }
  // start programmation
  EETIM = EETIM_VALUE;
  EECON = 0x50; // write 5x, clear 2nd LSb
  EECON ^= 0x50; // write Ax         
  // restore
  EA = ea_save;  // restore interrupt state
#endif
  return 1;
}
#endif




































































































