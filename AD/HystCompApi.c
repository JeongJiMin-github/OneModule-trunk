/**
********************************************************************************
* Copyright (c) 2006 CAS
* @brief   AD Hysteresis Compensation API Source File\n
* @file    FilterApi.c
* @version 1.0
* @date    2006/12/05
* @author  Yi Phyo Hong (HYP)
********************************************************************************
* @remark  [Version History]\n
*          v1.0 2006/12/05 created by HYP \n
********************************************************************************
*/

/*
********************************************************************************
*                       INCLUDES
********************************************************************************
*/
/* Insert include files here */
#include "../Common/Globals.h"
#include "../Common/CommonLib.h"
#include "../DB/StorageApi.h"
#include "../DB/ParamApi.h"
#include "../Communication/DebugApi.h"
#include "HystCompApi.h"
#include "AdApi.h"

#ifdef USE_EXP_HYST_COMP
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Hysteresis structure를 사용하기 위한 변수*/
HYST_PARAM_STRUCT HystParam;

////////////////////////
// Hysteresis Variable
////////////////////////
float HystErr=0.0;			// 현재 Hysteresis Error
long HystZeroRawAd;		// hysteresis error function의 시작 지점(영점키를 누른 상태의 보상전Ad, Hystersis가 0인 지점)
float HystMaxErrUp;		// 최대 hysteresis error
float HystMaxErrDn;		// 최대 hysteresis error
long PrevHystAd;		// 전 사이클의 보상전Ad
INT8U HystDirection;		// 무게 변화 방향(1=증가, 0=감소)
float HystConstantsUp[5];
float HystConstantsDn[5];

float HystAd[10];
////////////////////////
double CalcDeterminant( float **mat, int order);
bool MatrixInversion(float **A, int order, float **Y);

void CalcHystCurve(void);

//TM capa 500kg, 1t, 3t, 5t, 10t, 15t, 25t, 50t
INT16U hystRatioTableCapa[8] = {500, 1000, 3000, 5000, 10000, 15000, 25000, 50000};
INT16U hystRatioTableOrder[8] = {5,  4,       3,    5,     4,     3,     5,     5};
INT16U hystRatioTableMid[8][4] = {
        {100, 200, 300, 400},
        {250, 500, 750,   0},
        {1000, 2000, 0,   0},
        {1000, 2000, 3000, 4000},
        {2500, 5000, 7500,    0},
        {5000, 10000,    0,    0},
        {5000, 10000, 15000, 20000},
        {10000, 15000, 20000, 25000}//??????
};

INT8U HystCompSetRatio4Capa(INT16U capa)
{
        INT8U i,j;
        
        if (HystParam.hystOrder == 0) return ERR_NONE;
            
        for (i = 0;  i < 8; i++) 
        {
                if (hystRatioTableCapa[i] == capa) break;
        }
        if (i == 8) return ERR_CAL_RANGE;
        
        HystParam.hystOrder = hystRatioTableOrder[i];
//kkh0225 code optimaization
	for(j = 0; j < 4; j++)
	{
        HystParam.hystRatio[j].word.low = CalParam.capa;
		HystParam.hystRatio[j].word.high = hystRatioTableMid[i][j]; 		
	}
/*        HystParam.hystRatio[0].word.low = CalParam.capa;	//kkh0225 code optimaization
	HystParam.hystRatio[0].word.high = hystRatioTableMid[i][0];       
	HystParam.hystRatio[1].word.low = CalParam.capa;
	HystParam.hystRatio[1].word.high = hystRatioTableMid[i][1];        
	HystParam.hystRatio[2].word.low = CalParam.capa;
	HystParam.hystRatio[2].word.high = hystRatioTableMid[i][2];        
	HystParam.hystRatio[3].word.low = CalParam.capa;
	HystParam.hystRatio[3].word.high = hystRatioTableMid[i][3]; 
*/
        return ERR_NONE;
}

/**
********************************************************************************
* @brief    히스테리시스 보상 함수\n
* @param    raw filtered ad data
* @return   히스테리시스 보상 ad data
* @remark   
********************************************************************************
*/
INT32S HystCompProc(INT32S ad)
{
	long ZeroOffset;
        static float WidthIncr=0.0;
        static float WidthDecr=0.0;
        static float X0Incr=0.0;
        static float X0Decr=0.0;
        float temp,ad2,Span,AdProduct;
        //long l_dbgsize;
        int i,j;

	if (HystParam.hystEnable == 0)
	{
		return ad;
	}
        
        Span = (float)HystParam.hystAd[HystParam.hystOrder] - (float)HystParam.hystAd[0];
	
	if(HystParam.hystAd[0] == 0 || HystParam.hystAd[1] == 0 || HystParam.hystAd[2] == 0 || HystParam.hystAd[3] == 0
		|| HystParam.hystAd[1] == HystParam.hystAd[2] || HystParam.hystAd[2] == HystParam.hystAd[0] || HystParam.hystAd[3] == HystParam.hystAd[2]) {
		HystErr = 0.0;
		return ad;
	}
	ZeroOffset = HystZeroRawAd - HystParam.hystAd[0];

	if(ad <= HystParam.hystAd[0] + ZeroOffset) {
		HystErr = 0.0;
	} else if(ad >= HystParam.hystAd[HystParam.hystOrder] + ZeroOffset) {
		HystErr = 0.0;
	} else {
		if(ad > PrevHystAd) {			// 현재 ad가 이전 값 보다 클 때(ad 증가)
			if(HystDirection == 0) {	// 이전 ad 변화 방향이 '-'였을 때(ad 변화 방향이 바뀔 때)
                                ad2=(float)PrevHystAd-(float)HystParam.hystAd[0];
                                temp=0.0;
                                for (i=0;i<HystParam.hystOrder;i++)
                                {   AdProduct=1.0;
                                    for (j=HystParam.hystOrder;j>i;j--)
                                        AdProduct *= ad2;
                                    temp += AdProduct*HystConstantsUp[i];
                                }
                                //ad2=((float)PrevHystAd-(float)HystParam.hystAd[0]) - Span/2.0;
                                //temp = -4.0*(float)HystMaxErrUp/Span/Span*ad2*ad2 + HystMaxErrUp;
                                WidthIncr = temp - HystErr;
                                X0Incr=(float)PrevHystAd - (float)HystParam.hystAd[0];
			}
			HystDirection = 1;
                        ad2=(float)ad-(float)HystParam.hystAd[0];
                        temp=0.0;
                        for (i=0;i<HystParam.hystOrder;i++)
                        {   AdProduct=1.0;
                            for (j=HystParam.hystOrder;j>i;j--)
                                AdProduct *= ad2;
                            temp += AdProduct*HystConstantsUp[i];
                        }
                        //ad2=((float)ad-(float)HystParam.hystAd[0]) - Span/2.0;
                        //temp = -4.0*(float)HystMaxErrUp/Span/Span*ad2*ad2 + HystMaxErrUp;
                        if (X0Incr > Span || X0Incr < Span)
                            //HystErr = temp - WidthIncr/(Span-X0Incr)*(Span-(float)(ad-HystParam.hystAd[0]));
                            HystErr = temp - WidthIncr/(Span-X0Incr)/(Span-X0Incr)*(Span-(float)(ad-HystParam.hystAd[0]))*(Span-(float)(ad-HystParam.hystAd[0])); //20121224 YJH love you
		} else if(ad < PrevHystAd) {	// 현재 ad가 이전 값 보다 작을 때(ad 감소)
			if(HystDirection == 1) {	// 이전 ad 변화 방향이 '+'였을 때(ad 변화 방향이 바뀔 때)
                                ad2=(float)PrevHystAd-(float)HystParam.hystAd[0];
                                temp=0.0;
                                for (i=0;i<HystParam.hystOrder;i++)
                                {   AdProduct=1.0;
                                    for (j=HystParam.hystOrder;j>i;j--)
                                        AdProduct *= ad2;
                                    temp += AdProduct*HystConstantsDn[i];
                                }
                                //ad2=((float)PrevHystAd-(float)HystParam.hystAd[0]) - Span/2.0;
                                //temp = -4.0*(float)HystMaxErrDn/Span/Span*ad2*ad2 + HystMaxErrDn;
                                WidthDecr = temp - HystErr;
                                X0Decr=(float)PrevHystAd - (float)HystParam.hystAd[0];
			}
			HystDirection = 0;
                        ad2=(float)ad-(float)HystParam.hystAd[0];
                        temp=0.0;
                        for (i=0;i<HystParam.hystOrder;i++)
                        {   AdProduct=1.0;
                            for (j=HystParam.hystOrder;j>i;j--)
                                AdProduct *= ad2;
                            temp += AdProduct*HystConstantsDn[i];
                        }
                        //ad2=((float)ad-(float)HystParam.hystAd[0]) - Span/2.0;
                        //temp = -4.0*(float)HystMaxErrDn/Span/Span*ad2*ad2 + HystMaxErrDn;
                        if (X0Decr > 0.0 || X0Decr < 0.0)
                            //HystErr = temp - WidthDecr/(X0Decr)*(float)(ad-HystParam.hystAd[0]);
                            HystErr = temp - WidthDecr/(X0Decr)/(X0Decr)*(float)(ad-HystParam.hystAd[0])*(float)(ad-HystParam.hystAd[0]);
		}
	}	
	//l_dbgsize=sprintf (dbgStr, "%ld, %.2f, %.3f, %.3f\r\n",ad, HystErr, HystMaxErrUp,HystMaxErrDn);
	//SerialDebug(dbgStr,l_dbgsize);
        
	PrevHystAd = ad;
	ad = ad - (long)HystErr;
	
	return ad;  
}

/**
********************************************************************************
* @brief    Hysteresis compensation parameter들의 default 값을 세팅 해주는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void HystSetDefaultParam(void)
{
        INT8U i;
        
        HystParam.hystEnable = 0;	
        HystParam.hystOrder = 5;
        
        for (i = 0; i < 10; i++)
        {
        	HystParam.hystAd[i] = 0;
        }

        for (i = 0; i < 4; i++)
        {
        	HystParam.hystRatio[i].dword = 0;
        }
        HystWriteParam();
}

/**
********************************************************************************
* @brief    Hysteresis compensation parameter들을 EEPROM으로 Write하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void HystWriteParam(void)
{
//	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HYST, (INT8U *)&HystParam, sizeof(HystParam));
        HystCompSetRatio4Capa(CalParam.capa);
	ParamWriteData(PARAM_HEAD_AREA_NUM, 0, (INT8U *)&HystParam, sizeof(HystParam));
}

/**
********************************************************************************
* @brief    EEPROM의 Hysteresis compensation parameter들을 RAM으로 Read하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void HystReadParam(void)
{
	//ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HYST, (INT8U *)&HystParam, sizeof(HystParam));
	ParamReadData(PARAM_HEAD_AREA_NUM, 0, (INT8U *)&HystParam, sizeof(HystParam));        
}

/**
********************************************************************************
* @brief    히스테리시스 보상에서 사용하는 변수를 초기화 하는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/        
void HystInitVariable(void)
{
	HystReadParam();
/*        HystParam.hystEnable = 1;

        HystParam.hystOrder = 5;
        HystParam.hystAd[0] = 29510;//0   10=1803
        HystParam.hystAd[1] = 47565+1803;//100
        HystParam.hystAd[2] = 65622;//+1803;//200
        HystParam.hystAd[3] = 83680;//300
        HystParam.hystAd[4] = 101730;//400
        HystParam.hystAd[5] = 119780;//500 span
        HystParam.hystAd[6] = 101730;//400
        HystParam.hystAd[7] = 83680;//300
        HystParam.hystAd[8] = 65621;//200
        HystParam.hystAd[9] = 47566;//100
        
	HystParam.hystRatio[0].word.low = 500;
	HystParam.hystRatio[0].word.high = 100;        
	HystParam.hystRatio[1].word.low = 500;
	HystParam.hystRatio[1].word.high = 200;        
	HystParam.hystRatio[2].word.low = 500;
	HystParam.hystRatio[2].word.high = 300;        
	HystParam.hystRatio[3].word.low = 500;
	HystParam.hystRatio[3].word.high = 400;        
*/  
        /////////////////////////
        
	HystErr			= 0.0;
	HystZeroRawAd 	= HystParam.hystAd[0];
	PrevHystAd 	= HystParam.hystAd[0];
	HystDirection 	= 0;
	HystMaxErrUp 		= 0.0;
	HystMaxErrDn 		= 0.0;
        
        CalcHystCurve();
}

/**
********************************************************************************
* @brief    히스테리시스 보상에서 사용하는 Zero 변수를 Setting하는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void HystSetZero(INT32S lAd)
{
        if(AbsInt32s(lAd - HystParam.hystAd[0]) < (HystParam.hystAd[HystParam.hystOrder] - HystParam.hystAd[0]) / 10) {
                HystZeroRawAd = lAd;
        }        
}

void CalcHystCurve(void)
{
        //float E0, A, X0;
        //long l_dbgsize;
        float tempx[25];
        float tempi[25];
	float *XX[5];
	float *XI[5];
	float YY[5];
        float HystW2CRatio[4];
        float HAd[10];
        int i;
        
       
	if(HystParam.hystAd[0] == HystParam.hystAd[1] || HystParam.hystAd[1] == HystParam.hystAd[2] || HystParam.hystAd[1] == HystParam.hystAd[3] || HystParam.hystAd[2] == HystParam.hystAd[3]) return;
	//if(HystParam.CurveRatio.byte.low == 0) return;

        for(i = 0; i < 4; i++)  
	{        
            HystW2CRatio[i] = (float)HystParam.hystRatio[i].word.high / (float)HystParam.hystRatio[i].word.low;
        }
        
        for(i=0;i<5;i++)  
	{
            XX[i] = tempx+(i*(5));  
            XI[i] = tempi+(i*(5));  
	}
        
        for (i=0;i<10;i++)
        {
            HAd[i]=HystParam.hystAd[i]-HystParam.hystAd[0];
        }
        
        if (HystParam.hystOrder==2)
        {
            XX[0][0]=HAd[1]*HAd[1]; XX[0][1]=HAd[1];
            XX[1][0]=HAd[2]*HAd[2]; XX[1][1]=HAd[2];
            YY[0]=(float)HAd[1]-(float)HAd[2]*HystW2CRatio[0];
            YY[1]=0.0;
        }
        
        if (HystParam.hystOrder==3)
        {
            XX[0][0]=HAd[1]*HAd[1]*HAd[1]; XX[0][1]=HAd[1]*HAd[1]; XX[0][2]=HAd[1];
            XX[1][0]=HAd[2]*HAd[2]*HAd[2]; XX[1][1]=HAd[2]*HAd[2]; XX[1][2]=HAd[2];
            XX[2][0]=HAd[3]*HAd[3]*HAd[3]; XX[2][1]=HAd[3]*HAd[3]; XX[2][2]=HAd[3];
            YY[0]=(float)HAd[1]-(float)HAd[3]*HystW2CRatio[0];
            YY[1]=(float)HAd[2]-(float)HAd[3]*HystW2CRatio[1];
            YY[2]=0.0;
        }
        
        if (HystParam.hystOrder==4)
        {
            XX[0][0]=HAd[1]*HAd[1]*HAd[1]*HAd[1]; XX[0][1]=HAd[1]*HAd[1]*HAd[1]; XX[0][2]=HAd[1]*HAd[1]; XX[0][3]=HAd[1];
            XX[1][0]=HAd[2]*HAd[2]*HAd[2]*HAd[2]; XX[1][1]=HAd[2]*HAd[2]*HAd[2]; XX[1][2]=HAd[2]*HAd[2]; XX[1][3]=HAd[2];
            XX[2][0]=HAd[3]*HAd[3]*HAd[3]*HAd[3]; XX[2][1]=HAd[3]*HAd[3]*HAd[3]; XX[2][2]=HAd[3]*HAd[3]; XX[2][3]=HAd[3];
            XX[3][0]=HAd[4]*HAd[4]*HAd[4]*HAd[4]; XX[3][1]=HAd[4]*HAd[4]*HAd[4]; XX[3][2]=HAd[4]*HAd[4]; XX[3][3]=HAd[4];
            YY[0]=(float)HAd[1]-(float)HAd[4]*HystW2CRatio[0];
            YY[1]=(float)HAd[2]-(float)HAd[4]*HystW2CRatio[1];
            YY[2]=(float)HAd[3]-(float)HAd[4]*HystW2CRatio[2];
            YY[3]=0.0;
        }
        
        if (HystParam.hystOrder==5)
        {
            XX[0][0]=HAd[1]*HAd[1]*HAd[1]*HAd[1]*HAd[1]; XX[0][1]=HAd[1]*HAd[1]*HAd[1]*HAd[1]; XX[0][2]=HAd[1]*HAd[1]*HAd[1]; XX[0][3]=HAd[1]*HAd[1]; XX[0][4]=HAd[1];
            XX[1][0]=HAd[2]*HAd[2]*HAd[2]*HAd[2]*HAd[2]; XX[1][1]=HAd[2]*HAd[2]*HAd[2]*HAd[2]; XX[1][2]=HAd[2]*HAd[2]*HAd[2]; XX[1][3]=HAd[2]*HAd[2]; XX[1][4]=HAd[2];
            XX[2][0]=HAd[3]*HAd[3]*HAd[3]*HAd[3]*HAd[3]; XX[2][1]=HAd[3]*HAd[3]*HAd[3]*HAd[3]; XX[2][2]=HAd[3]*HAd[3]*HAd[3]; XX[2][3]=HAd[3]*HAd[3]; XX[2][4]=HAd[3];
            XX[3][0]=HAd[4]*HAd[4]*HAd[4]*HAd[4]*HAd[4]; XX[3][1]=HAd[4]*HAd[4]*HAd[4]*HAd[4]; XX[3][2]=HAd[4]*HAd[4]*HAd[4]; XX[3][3]=HAd[4]*HAd[4]; XX[3][4]=HAd[4];
            XX[4][0]=HAd[5]*HAd[5]*HAd[5]*HAd[5]*HAd[5]; XX[4][1]=HAd[5]*HAd[5]*HAd[5]*HAd[5]; XX[4][2]=HAd[5]*HAd[5]*HAd[5]; XX[4][3]=HAd[5]*HAd[5]; XX[4][4]=HAd[5];
            YY[0]=(float)HAd[1]-(float)HAd[5]*HystW2CRatio[0];
            YY[1]=(float)HAd[2]-(float)HAd[5]*HystW2CRatio[1];
            YY[2]=(float)HAd[3]-(float)HAd[5]*HystW2CRatio[2];
            YY[3]=(float)HAd[4]-(float)HAd[5]*HystW2CRatio[3];
            YY[4]=0.0;
        }
        
        if (MatrixInversion(XX,HystParam.hystOrder,XI))
        {
            for (i=0;i<HystParam.hystOrder;i++)
            {
                HystConstantsUp[i]=0.0;
                for (int j=0;j<HystParam.hystOrder;j++)
                    HystConstantsUp[i] += XI[i][j]*YY[j];
            }
        }
        else
        {
            return;
        }
    
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////// DOWN
        if (HystParam.hystOrder==2)
        {
            XX[0][0]=HAd[3]*HAd[3]; XX[0][1]=HAd[3];
            XX[1][0]=HAd[2]*HAd[2]; XX[1][1]=HAd[2];
            YY[0]=(float)HAd[3]-(float)HAd[2]*HystW2CRatio[0];
            YY[1]=0.0;
        }
        
        if (HystParam.hystOrder==3)
        {
            XX[0][0]=HAd[5]*HAd[5]*HAd[5]; XX[0][1]=HAd[5]*HAd[5]; XX[0][2]=HAd[5];
            XX[1][0]=HAd[4]*HAd[4]*HAd[4]; XX[1][1]=HAd[4]*HAd[4]; XX[1][2]=HAd[4];
            XX[2][0]=HAd[3]*HAd[3]*HAd[3]; XX[2][1]=HAd[3]*HAd[3]; XX[2][2]=HAd[3];
            YY[0]=(float)HAd[5]-(float)HAd[3]*HystW2CRatio[0];
            YY[1]=(float)HAd[4]-(float)HAd[3]*HystW2CRatio[1];
            YY[2]=0.0;
        }
        
        if (HystParam.hystOrder==4)
        {
            XX[0][0]=HAd[7]*HAd[7]*HAd[7]*HAd[7]; XX[0][1]=HAd[7]*HAd[7]*HAd[7]; XX[0][2]=HAd[7]*HAd[7]; XX[0][3]=HAd[7];
            XX[1][0]=HAd[6]*HAd[6]*HAd[6]*HAd[6]; XX[1][1]=HAd[6]*HAd[6]*HAd[6]; XX[1][2]=HAd[6]*HAd[6]; XX[1][3]=HAd[6];
            XX[2][0]=HAd[5]*HAd[5]*HAd[5]*HAd[5]; XX[2][1]=HAd[5]*HAd[5]*HAd[5]; XX[2][2]=HAd[5]*HAd[5]; XX[2][3]=HAd[5];
            XX[3][0]=HAd[4]*HAd[4]*HAd[4]*HAd[4]; XX[3][1]=HAd[4]*HAd[4]*HAd[4]; XX[3][2]=HAd[4]*HAd[4]; XX[3][3]=HAd[4];
            YY[0]=(float)HAd[7]-(float)HAd[4]*HystW2CRatio[0];
            YY[1]=(float)HAd[6]-(float)HAd[4]*HystW2CRatio[1];
            YY[2]=(float)HAd[5]-(float)HAd[4]*HystW2CRatio[2];
            YY[3]=0.0;
        }
        
        if (HystParam.hystOrder==5)
        {
            XX[0][0]=HAd[9]*HAd[9]*HAd[9]*HAd[9]*HAd[9]; XX[0][1]=HAd[9]*HAd[9]*HAd[9]*HAd[9]; XX[0][2]=HAd[9]*HAd[9]*HAd[9]; XX[0][3]=HAd[9]*HAd[9]; XX[0][4]=HAd[9];
            XX[1][0]=HAd[8]*HAd[8]*HAd[8]*HAd[8]*HAd[8]; XX[1][1]=HAd[8]*HAd[8]*HAd[8]*HAd[8]; XX[1][2]=HAd[8]*HAd[8]*HAd[8]; XX[1][3]=HAd[8]*HAd[8]; XX[1][4]=HAd[8];
            XX[2][0]=HAd[7]*HAd[7]*HAd[7]*HAd[7]*HAd[7]; XX[2][1]=HAd[7]*HAd[7]*HAd[7]*HAd[7]; XX[2][2]=HAd[7]*HAd[7]*HAd[7]; XX[2][3]=HAd[7]*HAd[7]; XX[2][4]=HAd[7];
            XX[3][0]=HAd[6]*HAd[6]*HAd[6]*HAd[6]*HAd[6]; XX[3][1]=HAd[6]*HAd[6]*HAd[6]*HAd[6]; XX[3][2]=HAd[6]*HAd[6]*HAd[6]; XX[3][3]=HAd[6]*HAd[6]; XX[3][4]=HAd[6];
            XX[4][0]=HAd[5]*HAd[5]*HAd[5]*HAd[5]*HAd[5]; XX[4][1]=HAd[5]*HAd[5]*HAd[5]*HAd[5]; XX[4][2]=HAd[5]*HAd[5]*HAd[5]; XX[4][3]=HAd[5]*HAd[5]; XX[4][4]=HAd[5];
            YY[0]=(float)HAd[9]-(float)HAd[5]*HystW2CRatio[0];
            YY[1]=(float)HAd[8]-(float)HAd[5]*HystW2CRatio[1];
            YY[2]=(float)HAd[7]-(float)HAd[5]*HystW2CRatio[2];
            YY[3]=(float)HAd[6]-(float)HAd[5]*HystW2CRatio[3];
            YY[4]=0.0;
        }
        
        if (MatrixInversion(XX,HystParam.hystOrder,XI))
        {
            for (i=0;i<HystParam.hystOrder;i++)
            {
                HystConstantsDn[i]=0.0;
                for (int j=0;j<HystParam.hystOrder;j++)
                    HystConstantsDn[i] += XI[i][j]*YY[j];
            }
        }
        else
        {
            return;
        }
}        


///////////////////////////////////////////////////////////////////////////////
// matrix inversioon  

// the result is put in Y  
int GetMinor(float **src, float **dest, int row, int col, int order);

bool MatrixInversion(float **A, int order, float **Y)  
{  
    double det ;  

    // memory allocation  
    //float *temp = new float[(order-1)*(order-1)];  
    //float **minor = new float*[order-1];  
    float temp[16];
   // float minor[4];
    float *minor[4]; 
    
    if (order<2 || order>5)
       return FALSE;
    
    det = CalcDeterminant(A,order);
    
    if (det==0.0)
       return FALSE;
    det=1.0/det;

    for(int i=0;i<order-1;i++)  
        minor[i] = temp+(i*(order-1));  

    for(int j=0;j<order;j++)  
    {  
        for(int i=0;i<order;i++)  
        {  
            // get the co-factor (matrix) of A(j,i)  
            GetMinor(A,minor,j,i,order);  
            Y[i][j] = det*CalcDeterminant(minor,order-1);  
            if( (i+j)%2 == 1)  
                Y[i][j] = -Y[i][j];  
        }  
    }  


    return 1;
}  
 

// calculate the cofactor of element (row,col)  

int GetMinor(float **src, float **dest, int row, int col, int order)  
{  
    // indicate which col and row is being copied to dest  
    int colCount=0,rowCount=0;  
   
    for(int i = 0; i < order; i++ )  
    {  
        if( i != row )  
        {  
            colCount = 0;  
            for(int j = 0; j < order; j++ )  
            {  
                // when j is not the element  
                if( j != col )  
                {  
                    dest[rowCount][colCount] = src[i][j];  
                    colCount++;  
                }  
            }  
            rowCount++;  
        }  
    }  
    return 1;  
}  


// Calculate the determinant recursively.  
double CalcDeterminant( float **mat, int order)  
{  
    // order must be >= 0  
    // stop the recursion when matrix is a single element  
    if( order == 1 )  
        return mat[0][0];  
   
    // the determinant value  
    double det = 0.0;  


    // allocate the cofactor matrix  
    //float **minor;  
    //minor = new float*[order-1];  
    //for(int i=0;i<order-1;i++)  
    //    minor[i] = new float[order-1];  
    float temp[16];
    float *minor[4];
    for(int i=0;i<order-1;i++)  
        minor[i] = temp+(i*(order-1));  


    for(int i = 0; i < order; i++ )  
    {  
        // get minor of element (0,i)  
        GetMinor( mat, minor, 0, i , order);  
        // the recusion is here!  
   
        det += (i%2==1?-1.0:1.0) * mat[0][i] * CalcDeterminant(minor,order-1);  
        //det += pow( -1.0, i ) * mat[0][i] * CalcDeterminant( minor,order-1 );  
    }  
   

    
    return det;  
} 

#else
/*
********************************************************************************
*                       GLOBAL(EXPORTED) VARIABLES & TABLES
********************************************************************************
*/
/* Insert global variables & tables here */
/** @brief Hysteresis structure를 사용하기 위한 변수*/
HYST_PARAM_STRUCT HystParam;

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
/** @brief 무게 변화 방향(1=증가, 0=감소)*/
INT8U hystDirection;
/** @brief 현재 Hysteresis Error*/
INT32S hystErr;			
/** @brief hystMaxAd에서의 Hysteresis Error*/
INT32S hystErrAtMaxAd;	
/** @brief hystMinAd에서의 Hysteresis Error*/
INT32S hystErrAtMinAd;	
/** @brief 무게가 증가에서 감소로 바뀐 지점의 보상전Ad*/
INT32S hystMaxAd;
/** @brief 최대 hysteresis error(=Setup.HystAd[2]-Setup.HystAd[1])*/
INT32S hystMaxErr;		
/** @brief 무게가 감소에서 증가로 바뀐 지점의 보상전Ad*/
INT32S hystMinAd;		
/** @brief hysteresis error function의 시작 지점(영점키를 누른 상태의 보상전Ad, Hystersis가 0인 지점)*/
INT32S hystZeroRawAd;	
/** @brief 이전 사이클의 보상전Ad*/
INT32S prevHystAd;	

/*
********************************************************************************
*                       LOCAL FUNCTION PROTOTYPES
********************************************************************************
*/
/* Insert static function prototypes here */
static INT32S hystErrCalc(INT32S ad, INT32S zeroOffset);
static INT32S hystDownErrCalc(INT32S ad, INT32S adOffset, INT32S maxAd, INT32S hystMaxAd);
static INT32S hystUpErrCalc(INT32S ad, INT32S adOffset);

/*
********************************************************************************
*                       GLOBAL(EXPORTED) FUNCTIONS
********************************************************************************
*/
/* Insert global functions here */


/*
********************************************************************************
*                       LOCAL FUNCTIONS
********************************************************************************
*/
/* Insert local functions here */
/**
********************************************************************************
* @brief    히스테리시스 보상 함수\n
* @param    raw filtered ad data
* @return   히스테리시스 보상 ad data
* @remark   
********************************************************************************
*/
INT32S HystCompProc(INT32S ad)
{
	INT32S ZeroOffset;

	if (HystParam.hystEnable == 0)
	{
		hystErr = 0;
		return ad;
	}

//move to HystInitVariable()
//	if (HystParam.hystZeroValue == 0 || HystParam.hystMidUpValue == 0 || HystParam.hystMidDownValue == 0 || HystParam.hystSpanValue == 0
//		|| HystParam.hystMidUpValue == HystParam.hystMidDownValue || HystParam.hystMidDownValue == HystParam.hystZeroValue || HystParam.hystSpanValue == HystParam.hystMidDownValue) 
//	{
//		hystErr = 0;
//		return ad;
//	}

	hystMaxErr = HystParam.hystMidUpValue - HystParam.hystMidDownValue;
	ZeroOffset = hystZeroRawAd - HystParam.hystZeroValue;

	if (ad < HystParam.hystZeroValue + ZeroOffset)
	{
		hystMinAd = HystParam.hystZeroValue + ZeroOffset;
		hystErr = 0;
		hystErrAtMinAd = 0;
	} 
	else if (ad > HystParam.hystSpanValue + ZeroOffset)	
	{
		hystMaxAd = HystParam.hystSpanValue + ZeroOffset;
		hystErr = 0;
		hystErrAtMaxAd = 0;
	} 
	else 
	{
		if (ad > prevHystAd) 
		{								// 현재 ad가 이전 값 보다 클 때(ad 증가)
			if (hystDirection == 0) 
			{							// 이전 ad 변화 방향이 '-'였을 때(ad 변화 방향이 바뀔 때)
				hystMinAd = prevHystAd;
				hystErrAtMinAd = hystErr;
			}
			hystMaxAd = prevHystAd;
			hystErrAtMaxAd = hystErr;
			hystDirection = 1;
		} 
		else if (ad < prevHystAd) 
		{								// 현재 ad가 이전 값 보다 작을 때(ad 감소)
			if (hystDirection == 1) 
			{							// 이전 ad 변화 방향이 '+'였을 때(ad 변화 방향이 바뀔 때)
				hystMaxAd = prevHystAd;
				hystErrAtMaxAd = hystErr;
			}
			hystMinAd = prevHystAd;
			hystErrAtMinAd = hystErr;
			hystDirection = 0;
		}
		hystErr = hystErrCalc(ad, ZeroOffset);	// Calculation Hysteresis Error
	}	
	prevHystAd = ad;
	ad = ad - hystErr;
	
	return ad;
}

/**
********************************************************************************
* @brief    히스테리시스 보상중 히스테리시스 에러를 구하는 함수\n
* @param    raw filtered ad data, zero offset
* @return   히스테리시스 보상에러
* @remark   
********************************************************************************
*/
INT32S hystErrCalc(INT32S ad, INT32S zeroOffset)
{
	INT32S h1, h2, shiftOffset;
	
	shiftOffset = hystMinAd - HystParam.hystZeroValue;
	shiftOffset -= multiplyDivide(hystErrAtMinAd, (HystParam.hystMidDownValue - HystParam.hystZeroValue), hystMaxErr);  

	if (shiftOffset < 0 || shiftOffset > HystParam.hystSpanValue) 
	{
		shiftOffset = 0;	
	}	
	
	if (hystDirection) 
	{
		h1 = hystUpErrCalc(ad, shiftOffset);	// Shifted Hyst.Err. Function(Up)
		h2 = hystUpErrCalc(ad, zeroOffset);	// Basic Hyst.Err. Function(Up)
		if (hystMaxErr > 0) 
		{										// Positive Hysteresis
			if (h1 > h2) 
			{	
				h1 = h2;		// Select Minimum
			}
		} 
		else 
		{					// Negative Hysteresis
			if (h1 < h2) 
			{
				h1 = h2;		// Select Maximum
			}
		}
	}
	else
	{
		h1 = hystDownErrCalc(ad, zeroOffset, hystMaxAd, hystErrAtMaxAd);
	}
	
	if (hystMaxErr > 0)
	{			// Boundary Check : Positive Hysteresis
		if (h1 > hystMaxErr) 
		{
			h1 = hystMaxErr;
		}
		else if (h1 < 0) 
		{
			h1 = 0;
		}
	} 
	else 
	{						// Boundary Check : Negative Hysteresis
		if (h1 < hystMaxErr) 
		{
			h1 = hystMaxErr;
		}
		else if (h1 > 0) 
		{
			h1 = 0;
		}
	}
	return h1;
}

/**
********************************************************************************
* @brief    하향 측정시 히스테리시스 에러를 구하는 함수\n
* @param    raw filtered ad data, ad offset, 최대 ad data, 히스테리시스 최대 오차
* @return   하향 히스테리시스 보상에러
* @remark   
********************************************************************************
*/
INT32S hystDownErrCalc(INT32S ad, INT32S adOffset, INT32S maxAd, INT32S hystMaxAd)
{
	INT32S h;
	
	if (maxAd == HystParam.hystZeroValue + adOffset)
	{
		return 0;
	}
	
	h = multiplyDivide(hystMaxAd, (ad - HystParam.hystZeroValue- adOffset), (maxAd - HystParam.hystZeroValue- adOffset));

	return h;
}

/**
********************************************************************************
* @brief    상향 측정시 히스테리시스 에러를 구하는 함수\n
* @param    raw filtered ad data, ad offset, 최대 ad data, 히스테리시스 최대 오차
* @return   상향 히스테리시스 보상에러
* @remark   
********************************************************************************
*/
INT32S hystUpErrCalc(INT32S ad, INT32S adOffset)
{
	INT32S h;
	
	if (ad - adOffset < HystParam.hystMidDownValue) 
	{
		h = multiplyDivide(hystMaxErr, (ad - HystParam.hystZeroValue - adOffset), (HystParam.hystMidDownValue - HystParam.hystZeroValue));
	} 
	else 
	{
		h = multiplyDivide(hystMaxErr, (ad - HystParam.hystMidDownValue - adOffset), (HystParam.hystMidDownValue - HystParam.hystSpanValue));
		h += hystMaxErr;
	}
	return h;
}

/**
********************************************************************************
* @brief    Hysteresis compensation parameter들의 default 값을 세팅 해주는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void HystSetDefaultParam(void)
{
	HystParam.hystEnable = 0;	
	HystParam.hystZeroValue = 0;
	HystParam.hystMidUpValue = 0;
	HystParam.hystMidDownValue = 0;
	HystParam.hystSpanValue = 0;
	HystWriteParam();
}

/**
********************************************************************************
* @brief    Hysteresis compensation parameter들을 EEPROM으로 Write하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void HystWriteParam(void)
{
	ParamWriteData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HYST, (INT8U *)&HystParam, sizeof(HystParam));
}

/**
********************************************************************************
* @brief    EEPROM의 Hysteresis compensation parameter들을 RAM으로 Read하는 함수\n
* @param    NONE
* @return   NONE
* @remark   
********************************************************************************
*/
void HystReadParam(void)
{
	ParamReadData(PARAM_AD_AREA_NUM, AD_PARAM_OFFSET_HYST, (INT8U *)&HystParam, sizeof(HystParam));
}

/**
********************************************************************************
* @brief    히스테리시스 보상에서 사용하는 변수를 초기화 하는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void HystInitVariable(void)
{
	HystReadParam();

	if (HystParam.hystZeroValue == 0 || HystParam.hystMidUpValue == 0 || HystParam.hystMidDownValue == 0 || HystParam.hystSpanValue == 0
		|| HystParam.hystMidUpValue == HystParam.hystMidDownValue || HystParam.hystMidDownValue == HystParam.hystZeroValue || HystParam.hystSpanValue == HystParam.hystMidDownValue) 
	{
		HystParam.hystEnable = 0;
	}

	hystErrAtMinAd = 0;
	hystErrAtMaxAd = 0;
	hystErr = 0;
	hystZeroRawAd = HystParam.hystZeroValue;
	hystMaxAd = HystParam.hystZeroValue;
	hystMinAd = HystParam.hystZeroValue;
	prevHystAd = HystParam.hystZeroValue;
	hystDirection = 0;
	hystMaxErr = 0;

}

/**
********************************************************************************
* @brief    히스테리시스 보상에서 사용하는 Zero 변수를 Setting하는 함수\n
* @param    none
* @return   none
* @remark   
********************************************************************************
*/
void HystSetZero(INT32S lAd)
{
	if (AbsInt32s(lAd - HystParam.hystZeroValue) < (HystParam.hystSpanValue - HystParam.hystZeroValue) / 10)
	{
		hystZeroRawAd = lAd;
	}
}
#endif