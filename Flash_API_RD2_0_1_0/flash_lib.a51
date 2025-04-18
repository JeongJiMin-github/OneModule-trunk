;****************************************************************************
;*
;*            (c) ATMEL-Wireless and Microcontrollers 2001 
;*
;*
;*****************************************************************************/

 NAME FLASH_LIB; 
;*A51**************************************************************************
; FILE_NAME         : FLASH_LIB.a51                                            
;------------------------------------------------------------------------------
; FILE_CREATED_BY   : Jean-sebastien Berthy && Rapha�l L'etendu
; FILE_CREATION_DATE: 14/01/00                                                 
;------------------------------------------------------------------------------
; FILE_PURPOSE: low level function for API  
;******************************************************************************

USING 0

PUBLIC ASM_MOV_R1_A
PUBLIC __API_FLASH_ENTRY_POINT


AUXR1 EQU 0A2h 

START SEGMENT CODE
RSEG START

;*****************************************************************************
; FUNCTION_NAME: ASM_MOV_A_R1
;*****************************************************************************
ASM_MOV_R1_A:
  Mov R1, A
RET

;*****************************************************************************
; FUNCTION_NAME: __API_FLASH_ENTRY_POINT
;*****************************************************************************
__API_FLASH_ENTRY_POINT:
  PUSH  AR2
  PUSH  AR4
  PUSH  AR6
  LCALL 0FFF0h
  POP AR6
  POP AR4
  POP AR2
RET

END




