/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   Common Library Source File\n
           for One Module
* @file    CommonLib.c
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
#include "Globals.h"
#include "CommonLib.h"

/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Decimal 자리수를 확인하기 위한 Table */
const INT32U code DecPowerConvTable[DEC_POWER_CONV_TABLE_NUM] = {1L, 10L, 100L, 1000L, 10000L, 100000L, 1000000L, 10000000L, 100000000L, 1000000000L};
/** @brief Hexa 자리수를 확인하기 위한 Table */
const INT32U code HexPowerConvTable[HEX_POWER_CONV_TABLE_NUM] = {0x01L, 0x10L, 0x100L, 0x1000L, 0x10000L, 0x100000L, 0x1000000L, 0x10000000L};


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
* @brief    INT32S 절대값 반환\n
* @param    num : 입력값
* @return   절대값
* @remark   
********************************************************************************
*/
INT32S AbsInt32s(INT32S num)
{
	if (num > 0)
	{
		return (num);
	}
	else
	{
		return (-num);
	}	
}

/**
********************************************************************************
* @brief    a * b / c 를 구하는 함수\n
* @param    상수 3개 a, b, c
* @return   a * b / c의 결과 값
* @remark   
********************************************************************************
*/
INT32S multiplyDivide(INT32S a, INT32S b, INT32S c)
{
	INT32S ret;
	
	if (b == 0) 
	{
		return 0;
	}
	
	ret = a;
	ret *= b;
	ret *= 10l;
	ret /= c;
	if (ret > 0) 
	{
		ret += 5;
	}
	else	    
	{
		ret -= 5;
	}
	ret /= 10l;
	
	return ret;
}

/**
********************************************************************************
* @brief    Decimal Digit Length 반환\n
* @param    decNum : 입력값
* @return   Decimal Digit Length (INT8U)
* @remark   
********************************************************************************
*/
INT8U CalcDecDigitLen(INT32S decNum)
{
	INT8U i;
	INT32S absNum;

	absNum = AbsInt32s(decNum);

	for (i = (DEC_POWER_CONV_TABLE_NUM - 1); i > 0; i--)
	{
		if (absNum >= DecPowerConvTable[i])
		{
			return i+1;
		}
	}
	return 1;
}

/**
********************************************************************************
* @brief    Hexa Digit Length 반환\n
* @param    hexNum : 입력값
* @return   Hexa Digit Length (INT8U)
* @remark   
********************************************************************************
*/
INT8U CalcHexDigitLen(INT32U hexNum)
{
	INT8U i;

	for (i = (HEX_POWER_CONV_TABLE_NUM - 1); i > 0; i--)
	{
		if (hexNum >= HexPowerConvTable[i])
		{
			return i+1;
		}
	}
	return 1;
}

/**
********************************************************************************
* @brief    하위 nibble을 Hexa char로 반환\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
INT8U NibbleToHexChar(INT8U nibble)
{
	nibble = nibble & 0x0f;

	if (nibble > 9)
	{
		return (nibble - 10 + 'A');
	}
	else
	{
		return (nibble + '0');
	}
}
*/
/**
********************************************************************************
* @brief    byte을 Hexa string으로 반환\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
/*
void ByteToHexStr(INT8U byte, char *str)
{
	str[0] = NibbleToHexChar(byte >> 4);
	str[1] = NibbleToHexChar(byte & 0x0f);
}
*/

#ifdef USE_CTS_MODE
/**
********************************************************************************
* @brief    메모리를 특정값으로 변환\n
* @param    메모리 시작주소, 변경 값, 메모리 사이즈
* @return   none
* @remark   
********************************************************************************
*/	
void CLMemSet(INT8U *startAddr, INT8U changeData, INT16U size)
{
	INT16U i;	
	//INT8U *tmp;

	//tmp = startAddr;
	
	for (i = 0; i < size; i++)
	{
		*startAddr = changeData;
		startAddr++;
	}
}
/**
********************************************************************************
* @brief    memory copy\n
* @param    dst : 저장될 data Pointer\n
*           src : 원본 data Pointer\n
*           size : dst String size
* @return   none
* @remark   
********************************************************************************
*/	

void CLMemCopy(INT8U *dst, INT8U *src, INT16U size)
{
	INT16U i;

	for (i = 0; i < size; i++)
	{
		*dst = *src;
		src++;
		dst++;
	}
}
#endif
/**
********************************************************************************
* @brief    const String Data뒤에 숫자 Data를 Decimal String으로 만들어 붙임
* @param    dst : 저장될 String Pointer\n
*           src : 원본 const String Pointer\n
*           num : String으로 변환할 Data\n
*           size : dst String size
* @return   none
* @remark   ex) "err  1"
********************************************************************************
*/
void MakeStrWithNum(char *dst, const char *src, INT32S num, INT8U size)
{
	INT8U i;

	i = 0;
	while (src[i] != 0x00)				
	{
		dst[i] = src[i];
		i++;
	}

	if (i < size)
	{
		Int32sToDecStr(&dst[i], num, size - i, 0x20, 0, 0, 0);
		dst[size] = 0x00;
	}
}

/**
********************************************************************************
* @brief    const String Data뒤에 숫자 Data를 Decimal String으로 만들어 붙임
* @param    dst : 저장될 String Pointer\n
*           src : 원본 const String Pointer\n
*           num : String으로 변환할 Data\n
*           size : dst String size \n
*           digitSize : numData digit size
* @return   none
* @remark  src pointer 값이 0이면 const string 출력 안함
*           ex) "err 01"
********************************************************************************
*/
void MakeStrWithDecNum(char *dst, const char *src, INT32S num, INT8U size, INT8U digitSize)
{
	INT8U i;

	i = 0;
	if (src != 0)
	{
		while (src[i] != 0x00)				
		{
			dst[i] = src[i];
			i++;
		}
	}

	if (i < size)
	{
		Int32sToDecStr(&dst[i], num, size - i, 0x20, digitSize - 1, 0, 0);
		dst[size] = 0x00;
	}
}

/**
********************************************************************************
* @brief    const String Data뒤에 숫자 Data를 Hex String으로 만들어 붙임
* @param    dst : 저장될 String Pointer\n
*           src : 원본 const String Pointer\n
*           num : String으로 변환할 Data\n
*           size : dst String size \n
*           digitSize : numData digit size
* @return   none
* @remark  ex) "err 0A"
********************************************************************************
*/
void MakeStrWithHexNum(char *dst, const char *src, INT32U num, INT8U size, INT8U digitSize)
{
	INT8U i;

	i = 0;
	if (src != 0)
	{
		while (src[i] != 0x00)				
		{
			dst[i] = src[i];
			i++;
		}
	}

	if (i < size)
	{
		Int32uToHexStr(&dst[i], num, size - i, digitSize);
		dst[size] = 0x00;
	}
}

/**
********************************************************************************
* @brief    const 문자열을 복사하는 함수
* @param    dst : 저장될 String Pointer\n
*           src : 원본 const String Pointer\n
*           size : dst String size
* @return   none
* @remark   
********************************************************************************
*/
void MakeStrCopy(char *dst, const char *src, INT8U size)
{
	INT8U i;

	i = 0;
	for (i = 0 ; i < size; i++)
	{
		dst[i] = src[i];
	}
}

/**
********************************************************************************
* @brief    INT32S를 Decimal String으로 변환
* @param    dst : 출력할 String Pointer\n
*           numData : 변환할 number data\n
*           size : dst String size\n
*           decPos : Decimal Position
*           decChar : Decimal Point Char (0x00 이면 삽입하지 않음), Print시(String 변환시) 사용
*           useThousandPt : use flag for thousnad point, Print시(String 변환시) 사용
* @return   String size내에서 '-'sign이 처리가 안될 때 추가로 '-'sign flag 
*           필요하다는 의미로 '1'반환, 그외 '0' 반환
* @remark   Decimal Position 까지는 '0' 채움 (예 0,00)
*           남는 자리는 space로 채움 (오른쪽 정렬)
********************************************************************************
*/
INT8U Int32sToDecStr(char *dst, INT32S numData, INT8U size, char fillChar, INT8U decPos, char decChar, INT8U useThousandPt)
{
	INT8U pos;
	INT8U i;
	INT8U byte;
	INT8U endFlag;
	INT8U auxMinusSignFlag;
	INT8U sign;
	INT8U strSize;
	INT32S	absNumData;
	INT8U use1000Pt;

	endFlag = 0;
	strSize = 0;
	pos = size - 1;
	auxMinusSignFlag = OFF;

	if (fillChar == 0) 
	{
		fillChar = 0x20; //space
	}

	if (numData < 0) 
	{
		sign = 1;
		absNumData = -numData;
	}
	else
	{
		sign = 0;
		absNumData = numData;
	}

	dst[size] = 0x00;	//null

	if (((absNumData / DecPowerConvTable[decPos]) >= 1000) && (useThousandPt == 1)) 
	{
		use1000Pt = 1;
	}
	else
	{
		use1000Pt = 0;
	}

	for (i = 0; i < size; i++)
	{
		if ((endFlag == 0) || (i <= decPos))
		{
			if (pos > 0)
			{
				if ((decChar != 0x00) && (decPos != 0) && (decPos == i))
				{
					dst[pos] = decChar;
					pos--;
					strSize++;
				} 
				else if ((use1000Pt == 1) && ((decPos + 3) == i))
				{
					dst[pos] = ',';	//COMMA
					pos--;
					strSize++;
				}
			}
			byte = (INT8U)(absNumData % 10);
			dst[pos] = byte + '0';
			strSize++;
			absNumData = absNumData / 10;
			if (absNumData == 0) 
			{
				endFlag = 1;
			}
		}
		else
		{
			dst[pos] = fillChar;//Space Character
		}
		if (pos == 0) 
		{
			break;
		}
		pos--;
	}

	if (sign == 1)
	{
		if (strSize < size)
		{
			dst[size - strSize - 1] = '-';
		}
		else
		{
			auxMinusSignFlag = ON;
		}
	}

	return auxMinusSignFlag;
}

/**
********************************************************************************
* @brief    INT32S를 Hexa String으로 변환
* @param    dst : 출력할 String Pointer\n
*           numData : 변환할 number data\n
*           size : dst String size\n
*           digitSize : data의 digit size (digit size까지는 '0'을 채운다.)
* @return   dst String size
* @remark   남는 자리는 space로 채움 (오른쪽 정렬)\n
********************************************************************************
*/
INT8U Int32uToHexStr(char *dst, INT32S numData, INT8U size, INT8U digitSize)
{
	INT8U pos;
	INT8U i;
	INT8U byte;
	INT8U endFlag;

	endFlag = 0;
	pos = size - 1;

	dst[size] = 0x00;//null

	for (i = 0; i < size; i++)
	{
		if (endFlag == 0)
		{
			byte = (INT8U)(numData % 0x10);
			if (byte < 0x0A) 
			{
				dst[pos] = byte + '0';
			}
			else
			{
				dst[pos] = (byte - 0x0A) + 'A';
			}
			numData = numData / 0x10;
			if (numData == 0)
			{
				if (i < (digitSize - 1))
				{
					endFlag = 0;
				}
				else
				{
					endFlag = 1;
				}
			}
		}
		else
		{
			dst[pos] = 0x20;//Space Character
		}
		pos--;
	}

	return size;
}

/**
********************************************************************************
* @brief    data1의 값을 data2로  반올림한후 최종 반올림 된 값을 반환하는 함수
* @param    data1 : 반올림 하려는값 \n
*           data2 : 반올림 기준이 되는 값 \n
* @return   반올림 된 값 
* @remark   
********************************************************************************
*/
INT32U PriceRoundOff(INT32U data1, INT32U data2)
{
	INT32U	result;
	INT32U	remainder;

	if (data2 == 0)
	{
		return data1;
	}
	else
	{	
		result = data1 / data2;
		remainder = data1 % data2;
	    	if ((remainder * 2) >= data2)
		{
		    	result++;
		}
		result = result * data2;
		return result;
	}    	
}

/**
********************************************************************************
* @brief    HEX data를 BCD값으로 변환하는 함수
* @param    cc : HEX data
* @return   BCD data
* @remark   ex) 0x0d -> 0x13
********************************************************************************
*/
INT8U hex2bcd(INT8U cc)		
{
	INT8U c1,c2;

	c1 = cc / 10;
	c2 = cc % 10;
	c1 <<= 4;
	c1 |= c2;
	return c1;
}

/**
********************************************************************************
* @brief    BCD data를 HEX값으로 변환하는 함수
* @param    cc : BCD data
* @return   HEX data
* @remark   ex) 0x13 -> 0x0d
********************************************************************************
*/
INT8U bcd2hex(INT8U cc)		
{
	INT8U c1,c2;

	c1 = cc & 0x0f;
	c2 = cc >> 4;
	c2 = c2 * 10 + c1;
	return c2;
}

/**
********************************************************************************
* @brief    Str Space 제거       
* @param    none
* @return   none
* @remark    	 		\n
*********************************************************************************
*/
#ifdef USE_ARGOX_PRINTER
void DelStrSpace(INT8U *msg, INT16U size)
{
	INT8S i;
	for(i = size -1; i > 0;  i--)
	{
		if(msg[i] == 0x20)msg[i] = '\0';
		else return;
	}
}
#endif

/**
********************************************************************************
* @brief    ASCII문자중 숫자 판별 함수
* @param    cc : ASCII data
* @return   KR : 판별결과(1:숫자, 0:숫자아님)\n
* @remark   ex) '0'~'9'-> 숫자, 그외 -> 숫자아님
********************************************************************************
*/
INT8U isNum(INT8U cc)
{
	if(cc >= '0' && cc <= '9') return 1;
	else return 0;
}
#ifdef USE_CTS_MODE
/**
********************************************************************************
* @brief   BCC 계산 함수\n
* @param    data,data size
* @return   bcc
* @remark   오류 검출을 하기 위하여 BCC 값을 계산하는 함수
********************************************************************************
*/  
INT8U CalcBcc(INT8U *bccData, INT16U len)
{
	INT8U i;
	INT8U bcc = 0; 
	for(i = 0; i<len; i++)
		bcc ^= *(bccData+i); 
	return bcc;
}
#endif

void *Memset(void *dest, int c, unsigned int count)
{
	unsigned char *p = (unsigned char *)dest;

	while (p && count--) {
		*p++ = (unsigned char)c;
	}
	return p;
}
/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
