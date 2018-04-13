/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2007-2008; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      	APPLICATION CODE
*
*                                     	hal_ProcessACC_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : hal_ProcessACC_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  	HAL_PROCESSACC_GLOBLAS
#include  <math.h>
#include	<string.h>
#include	"bsp_h.h"
#include	"bsp_acc_h.h"
#include	"hal_h.h"
#include	"cfg_h.h"
#include	"main_h.h"
#include	"hal_protocol_h.h"
#include	"hal_processGPS_h.h"
#include	"hal_processACC_h.h"
#include	"hal_processSYS_h.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

static 	void 	HAL_Check_Car_Change 			(s16 X,	s16 Y,	s16 Z);
static	void 	HAL_Calculate_Car_Angle 	(s16 X,	s16 Y,	s16 Z,  u8 ucFilterNum);


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*************************************************���ٶ����߼�*************************************************/

__align(8) float   Angle_Horizon_X=0, Angle_Horizon_Y=0, Angle_Horizon_Z=0;	// ��������ڵ���Ƕ�
__align(8) float   Now_Acc=0,  Max_Acc=0, Static_ACC = 0;;
static  s16 S_X = 0, S_Y = 0, S_Z = 0;
static  u8  S_MaxAxis = 0u;

#define  MAX(a,b)  						                (a > b ? a : b)
#ifndef  PI
#define  PI                                   3.14
#endif
#define  PI_ANGLE                             180

#define  MAX_X                                1
#define  MAX_Y                                2
#define  MAX_Z                                3

#define  ACC_JUDGE_NUM                        10

#define  POW2(n)                              (n * n)

#define  ACC_UP_TMR                           250
#define  ACC_BRAKE_TMR                        250
#define  ACC_CRASH_TMR                        100

#define  STATIC_ACC_SUM_NUM                   20u

#define  CHANGE_MID_VAL_FILTER_EN             1
#define  KALMANFILTER_EN                      1

#define  KALMANFILTER_Q                       12
#define  KALMANFILTER_R                       1
/*
******************************************************************************
* Function Name  : my_abs16()
* Description    : ȡ����ֵ����
* Input          : 
*								 : 
* Output         : None
* Return         : None
******************************************************************************
*/
u16 my_abs16 (s16 number)
{
   return (number>= 0 ? number : -number);
}	
	
/*        
        Q:����������Q���󣬶�̬��Ӧ��죬�����ȶ��Ա仵
        R:����������R���󣬶�̬��Ӧ�����������ȶ��Ա��        
*/
static double KalmanFilter(const double ResrcData, double ProcessNiose_Q,double MeasureNoise_R,double InitialPrediction)
{
    double R = MeasureNoise_R;
    double Q = ProcessNiose_Q;
    static        double x_last;
    double x_mid = x_last;
    double x_now;
    static        double p_last;
    double p_mid ;
    double p_now;
    double kg;        

    x_mid  = x_last; //x_last=x(k-1|k-1),x_mid=x(k|k-1)
    p_mid  = p_last+Q; //p_mid=p(k|k-1),p_last=p(k-1|k-1),Q=����
    kg     = p_mid/(p_mid+R); //kgΪkalman filter��RΪ����
    x_now  = x_mid+kg*(ResrcData-x_mid);//���Ƴ�������ֵ
            
    p_now  = (1-kg)*p_mid;//����ֵ��Ӧ��covariance        

    p_last = p_now; //����covarianceֵ
    x_last = x_now; //����ϵͳ״ֵ̬

    return x_now;                
}

/*
******************************************************************************
* Function Name  : Cal_Angle_Horison_XYZ()
* Description    : ��������ڵ���Ƕ�(����ȫ�ֱ���Angle_Horizon_X,Angle_Horizon_Y,Angle_Horizon_Z)
* Input          : 
*								 : 
* Output         : None
* Return         : None
******************************************************************************
*/
static	void  Cal_Angle_Horison_XYZ(s16	T_X,	s16 T_Y,	s16 T_Z)
{
    float temp_xyz = 0;

    //������˼���ǣ������Ǹ����ٶȱ仯����100����������X��Y��Z��ı任����100����
    temp_xyz        = T_Y * T_Y + T_Z * T_Z; //����ƽ����
    temp_xyz        = sqrt( temp_xyz ); //����
    if (0 != temp_xyz)
    {
        temp_xyz        = T_X / temp_xyz;
        Angle_Horizon_X = atan( temp_xyz ); //����X����ˮƽ�淽��X��֮�нǣ�ˮƽX��Ϊ���裩//����
        Angle_Horizon_X = ( ( ( float )Angle_Horizon_X ) / PI ) * PI_ANGLE;	//��ɽǶ�
    }
    else
    {
        Angle_Horizon_X = 90;
    }

    temp_xyz        = T_X * T_X + T_Z * T_Z; //����ƽ����
    temp_xyz        = sqrt( temp_xyz ); //����
    if (0 != temp_xyz)
    {
        temp_xyz        = T_Y / temp_xyz;
        Angle_Horizon_Y = atan( temp_xyz ); //����Y����ˮƽ�淽��Y��֮�нǣ�ˮƽY��Ϊ���裩
        Angle_Horizon_Y = ( ( ( float )Angle_Horizon_Y ) / PI ) * PI_ANGLE;	//��ɽǶ�
    }
    else
    {
        Angle_Horizon_Y = 90;
    }

    temp_xyz        = T_X * T_X + T_Y * T_Y; //����ƽ����
    temp_xyz        = sqrt( temp_xyz ); //����
    if (0 != temp_xyz)
    {
        temp_xyz        = T_Z / temp_xyz;
        Angle_Horizon_Z = atan( temp_xyz ); //����Z����ˮƽ�淽��Z��֮�нǣ�ˮƽZ��Ϊ���裩
        Angle_Horizon_Z = ( ( ( float )Angle_Horizon_Z ) / PI ) * PI_ANGLE;	//��ɽǶ�
    }
    else
    {
        Angle_Horizon_Z = 90;
    }
}

static float AdjustMidValFilter(float fVal, float *fpData, u8 *pucCnt)
{
    float fRet = 0;
  
    if(*pucCnt != 3u)
    {
        fpData[*pucCnt] = fVal;
        (*pucCnt)++;
    }
    else
    {
        fpData[1u] = (fpData[0u] + fpData[2u]) / 2.0f;
        fRet       = fpData[0u];
        fpData[0u] = fpData[1u];
        fpData[1u] = fpData[2u];
        fpData[2u] = fVal;
    }
    
    return fRet;
}

static void AccCalculate(s16 X,	s16 Y,	s16 Z, float *pAccVal)
{
#if CHANGE_MID_VAL_FILTER_EN
    static  float s_fBuf[3] = {0};
    static  u8    s_ucCnt   = 0u;
#endif
		float  fMaxAngle        = MAX(MAX(Angle_Horizon_X, Angle_Horizon_Y), Angle_Horizon_Z);
		s16    sMaxChangeAcc    = MAX(my_abs16(MAX(X - S_X, Y - S_Y)), my_abs16(Z - S_Z));
    s16    sMidChangeAcc    = 0;
    float  fMidAngle        = 0;
		float  fMoveAngle       = 0;
		
    if (fMaxAngle == Angle_Horizon_X)
    {
        S_MaxAxis = MAX_X;
        fMidAngle = MAX(Angle_Horizon_Y,Angle_Horizon_Z);
    }
    else if (fMaxAngle == Angle_Horizon_Y)
    {
        S_MaxAxis = MAX_Y;
        fMidAngle = MAX(Angle_Horizon_X,Angle_Horizon_Z);
    }
    else
    {
        S_MaxAxis = MAX_Z;
        fMidAngle = MAX(Angle_Horizon_X,Angle_Horizon_Y);
    }
    
		if ( sMaxChangeAcc == my_abs16(X - S_X))
		{
				fMoveAngle    = Angle_Horizon_X;
        sMidChangeAcc = MAX(my_abs16(Y - S_Y),my_abs16(Z - S_Z));
		}
		else if (sMaxChangeAcc == my_abs16(Y - S_Y))
		{
				fMoveAngle    = Angle_Horizon_Y;
        sMidChangeAcc = MAX(my_abs16(X - S_X),my_abs16(Z - S_Z));
		}
		else
		{
				fMoveAngle    = Angle_Horizon_Z;
        sMidChangeAcc = MAX(my_abs16(X - S_X),my_abs16(Y - S_Y));
		}
		
		if (fMaxAngle != fMoveAngle)
		{
				*pAccVal = ( float )sMaxChangeAcc / cos( ( ( float )fMoveAngle / PI_ANGLE ) * PI );
		}
		else
		{
        *pAccVal = ( float )sMidChangeAcc / cos( ( ( float )fMidAngle / PI_ANGLE ) * PI );
		}
//     *pAccVal = my_abs16(sqrt( POW2( my_abs16(X - S_X) ) + POW2( my_abs16(Y - S_Y) ) +  POW2(my_abs16(Z - S_Z))));
#if CHANGE_MID_VAL_FILTER_EN    
    *pAccVal = AdjustMidValFilter(*pAccVal, s_fBuf, &s_ucCnt);
#endif
}

/*
******************************************************************************
* Function Name  : HAL_Calculate_Car_Angle()
* Description    : ���㳵�������֮��ĽǶ�
* Input          : X-x���С,Y-y���С,Z-z���С(ƽ��������ֵ) (����ȫ�ֱ���Car_Direction_XYZ)
*								 : 
* Output         : None
* Return         : None
******************************************************************************
*/
static	void HAL_Calculate_Car_Angle (s16 X,	s16 Y,	s16 Z, u8 ucFilterNum)
{
    static u8    s_ucCnt = 0u;
    static s32   s_X     = 0u;
    static s32   s_Y     = 0u;
    static s32   s_Z     = 0u;
    
    s_X += X;
		s_Y += Y;
		s_Z += Z;
    
    s_ucCnt ++;
    
    if (s_ucCnt == ucFilterNum)
    {
        S_X        = s_X / ucFilterNum;
        S_Y        = s_Y / ucFilterNum;
        S_Z        = s_Z / ucFilterNum;
        Static_ACC = my_abs16(sqrt( POW2( my_abs16(S_X) ) + POW2( my_abs16(S_Y) ) +  POW2(my_abs16(S_Z))));
      
        Cal_Angle_Horison_XYZ(S_X,S_Y,S_Z);          // ������Ե���Ƕ�
    
        #if(DEF_ACCINFO_OUTPUTEN > 0)
        if(dbgInfoSwt & DBG_INFO_ACC)
            myPrintf("[MEMS-ACC]:ACC_Car-Angle:X:%.1f,Y:%.1f,Z:%.1f\r\n",	Angle_Horizon_X,Angle_Horizon_Y,Angle_Horizon_Z);
        #endif
        
        Angle_Horizon_X = my_abs16( Angle_Horizon_X );
        Angle_Horizon_Y = my_abs16( Angle_Horizon_Y );
        Angle_Horizon_Z = my_abs16( Angle_Horizon_Z );
      
        s_ucCnt         = 0u;
        s_X             = 0;
        s_Y             = 0;
        s_Z             = 0;
    }
}
/*
******************************************************************************
* Function Name  : HAL_Check_Car_Change()
* Description    : �жϳ�����̬�Ƿ�ı�
* Input          : 
*								 : 
* Output         : None
* Return         : 
******************************************************************************
*/
static void HAL_Check_Car_Change (s16 X,	s16 Y,	s16 Z)
{
    static u8    s_ucgChangeFlag  = 0u;
    static u8    s_ucGoChangeFlag = 0u;
    static u32   s_ucgKeepTmr     = 0u;
    static u32   s_ucGoKeepTmr    = 0u;
    static float s_fLastAcc       = 0.0f;
    u16          usChangeVal      = 0u;
    u16          usNowAcc         = 0u;
  

    if (S_MaxAxis == MAX_X)
    {
        usChangeVal = my_abs16(X - S_X);
    }
    else if (S_MaxAxis == MAX_Y)
    {
        usChangeVal = my_abs16(Y - S_Y);
    }
    else
    {
        usChangeVal = my_abs16(Z - S_Z);
    }
  
    if (usChangeVal > 15)
  	{
        if (0u == s_ucgChangeFlag)
        {
            OSTmr_Count_Start(&s_ucgKeepTmr);
            s_ucgChangeFlag = 1u;
        }
    }
    else
    {
        s_ucgChangeFlag = 0u;
    }
    
    if ( (Now_Acc > 15) && (my_abs16(Now_Acc - s_fLastAcc) < 50))
    {
        if (0u == s_ucGoChangeFlag)
        {
            OSTmr_Count_Start(&s_ucGoKeepTmr);
            s_ucGoChangeFlag = 1u;
        }
    }
    else
    {
        s_ucGoChangeFlag = 0u;
        OSTmr_Count_Start(&s_ucGoKeepTmr);
    }
    
    if (s_ucgChangeFlag || s_ucGoChangeFlag)
    {
        if (( s_ucgChangeFlag && (OSTmr_Count_Get(&s_ucgKeepTmr) >= 300u)) || ( s_ucGoChangeFlag && (OSTmr_Count_Get(&s_ucGoKeepTmr) >= 300u)))
        {
            usNowAcc = my_abs16(sqrt( POW2( my_abs16(X) ) + POW2( my_abs16(Y) ) +  POW2(my_abs16(Z))));
            if ((usNowAcc > (Static_ACC - 10) ) && (usNowAcc < (Static_ACC + 10)))
            {
                if(dbgInfoSwt & DBG_INFO_ACC)
                {
                    myPrintf("**************Attitude Adjust**************\r\n");
                    myPrintf("[LAST_STATIC-ACC]:S_X:%-d,S_Y:%-d,S_Z:%-d, XYZ:%-d\r\n", S_X, S_Y, S_Z, (u16)Static_ACC);
                    myPrintf("[NOW_STATIC-ACC]:X:%-d,Y:%-d,Z:%-d, XYZ:%-d\r\n", X, Y, Z, usNowAcc);
                }
                S_X = X;
                S_Y = Y;
                S_Z = Z;
                s_ucgChangeFlag  = 0u;
                s_ucGoChangeFlag = 0u;
            }
            else
            {
            }
        }
        else
        {
        }
    }
    else
    {
    }
    s_fLastAcc = Now_Acc;
}



/*
******************************************************************************
* Function Name  : HAL_Calculate_Main()
* Description    : ��ʻ��Ϊ�ж�
* Input          : X-x���С,Y-y���С,Z-z���С(ƽ��������ֵ) (����ȫ�ֱ���Car_Direction_XYZ)
*								 : 
* Output         : ����������ͼ��ϳɼ��ٶ�ֵ
* Return         : None
******************************************************************************
*/
u8 HAL_Calculate_Main (s16 X,	s16 Y, s16 Z, u8 *pucCarAngleFlag)
{
		u8 retCode                      = 0;
    u8 tmpCrashCode                 = 0;
    // ������ײ��ʱ�洢����
    static GPSProData_Def	tmpGPSInfo;	
    static vu32	 s_accPackCounter   = 0;			// ���ٶȴ�����˶�ʱ��
    static vu32	 s_accCrashCounter1 = 0;
    static vu32	 s_accCrashCounter2 = 0;		// ��ײ���˶�ʱ��
    static s16	 s_tmpX             = 0;
    static s16	 s_tmpY             = 0;
    static s16	 s_tmpZ             = 0;
    static s16	 s_tmpXYZ           = 0;
    static u8		 s_first_flag       = 0;					// �״��жϱ�־
    static u32   s_uiUpTmr          = 0u;
    static u32   s_uiBrakeTme       = 0u;
    static u32   s_uiCreashTmr      = 0u;
    static u8    s_ucBegainUp       = 0u;
    static u8    s_ucBegainBreak    = 0u;
    static u8    s_ucBegainCreash   = 0u;
    static u8    s_ucCreashFlag     = 0u;
#if CHANGE_MID_VAL_FILTER_EN
    static float s_fXBuf[3]         = {0};    
    static float s_fYBuf[3]         = {0};   
    static float s_fZBuf[3]         = {0};
    static u8    s_ucXCnt           = 0u;
    static u8    s_ucYCnt           = 0u;
    static u8    s_ucZCnt           = 0u;
    static u8    s_ucFilterCnt      = 0u;
#endif 

		if ( s_first_flag < STATIC_ACC_SUM_NUM )
		{
        s_first_flag++;
				HAL_Calculate_Car_Angle(X, Y, Z, STATIC_ACC_SUM_NUM);
        if (STATIC_ACC_SUM_NUM == s_first_flag)
        {
            OSTmr_Count_Start(&s_accPackCounter);	// ��ʼ��̬������˶�ʱ��
        }
		}
    else if (1u == *pucCarAngleFlag)
    {
        s_first_flag = 0u;
        *pucCarAngleFlag = 0u;
    }
    else
    {
#if CHANGE_MID_VAL_FILTER_EN
        X = AdjustMidValFilter(X, s_fXBuf, &s_ucXCnt);
        Y = AdjustMidValFilter(Y, s_fYBuf, &s_ucYCnt);
        Z = AdjustMidValFilter(Z, s_fZBuf, &s_ucZCnt);
#endif
        HAL_Check_Car_Change(X, Y, Z);
      
        AccCalculate(X, Y, Z, &Now_Acc);
#if KALMANFILTER_EN
        Now_Acc = KalmanFilter(Now_Acc, KALMANFILTER_Q, KALMANFILTER_R, 0);
#endif

        if((OSTmr_Count_Get(&s_accPackCounter) >= 10000) && (0u == s_ucCreashFlag))	// 10��֮��ֻ�ϴ�һ���¼�
        {
             if(dbgInfoSwt & DBG_INFO_ACC)
             {
                myPrintf("[Now_Acc]:%-d\r\n",	my_abs16(Now_Acc));
//                 myPrintf("[ACC]:%-d\r\n",	my_abs16(sqrt( POW2( my_abs16(X) ) + POW2( my_abs16(Y) ) +  POW2(my_abs16(Z)))));
             }
          
            if (Now_Acc >= s_cfg.dssPTH)
            {
                s_ucFilterCnt = 0u;
                if (0u == s_ucBegainCreash)
                {
                    s_ucBegainCreash = 1u;
                    s_ucBegainBreak  = 0u;
                    s_ucBegainUp     = 0u;
                    OSTmr_Count_Start(&s_uiCreashTmr);
                }
                else
                {
                }
            }
            else if (Now_Acc >= s_cfg.dssDTH)
            {
                s_ucFilterCnt = 0u;
                if ((0u == s_ucBegainBreak) && (0u == s_ucBegainCreash))
                {
                    s_ucBegainBreak = 1u;
                    s_ucBegainUp    = 0u;
                    OSTmr_Count_Start(&s_uiBrakeTme);
                }
                else
                {
                }
            }
            else if (Now_Acc >= s_cfg.dssATH)
            {
                s_ucFilterCnt = 0u;
                if ((0u == s_ucBegainUp) && (0u == s_ucBegainBreak) && (0u == s_ucBegainCreash))
                {
                    s_ucBegainUp = 1u;
                    OSTmr_Count_Start(&s_uiUpTmr);
                }
                else
                {
                }
            }
            else
            {
                if (s_ucBegainUp || s_ucBegainBreak || s_ucBegainCreash)
                {
                    if (Now_Acc >= (s_cfg.dssATH - 50))
                    {
                        if (s_ucFilterCnt >= 5u)
                        {
                            Max_Acc          = 0;
                            s_ucFilterCnt    = 0u;
                            s_ucBegainUp     = 0u;
                            s_ucBegainBreak  = 0u;
                            s_ucBegainCreash = 0u;
                        }
                        else
                        {
                            s_ucFilterCnt++;
                        }
                    }
                    else
                    {
                        Max_Acc          = 0;
                        s_ucFilterCnt    = 0u;
                        s_ucBegainUp     = 0u;
                        s_ucBegainBreak  = 0u;
                        s_ucBegainCreash = 0u;
                    }
                }
                else
                {
                    Max_Acc          = 0;
                    s_ucFilterCnt    = 0u;
                    s_ucBegainUp     = 0u;
                    s_ucBegainBreak  = 0u;
                    s_ucBegainCreash = 0u;
                }
            }
            
            if (s_ucBegainUp || s_ucBegainBreak || s_ucBegainCreash)
            {
                if (Now_Acc > Max_Acc)
                {
                    Max_Acc  = Now_Acc;
                    s_tmpX   = X;
                    s_tmpY   = Y;
                    s_tmpZ   = Z;
                    s_tmpXYZ = Max_Acc;
                }
              
                if ((OSTmr_Count_Get(&s_uiCreashTmr) >= ACC_CRASH_TMR) && s_ucBegainCreash)
                {
                    memset((u8 *)&tmpGPSInfo,	0,	sizeof(tmpGPSInfo));
										memcpy((u8 *)&tmpGPSInfo,	(u8 *)&s_gpsPro, sizeof(s_gpsPro));
                    s_ucCreashFlag = 1u;
                    #if(DEF_EVENTINFO_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_EVENT)
                        myPrintf("[EVENT]: RAPID_CRASH Check start!!\r\n\r\n");
                    #endif
                    OSTmr_Count_Start(&s_accCrashCounter1);	// ��ײ���˼�ʱ����ʼ��ʱ
                    OSTmr_Count_Start(&s_accCrashCounter2);	// ��ײ���˼�ʱ����ʼ��ʱ	
                }
                else if ((OSTmr_Count_Get(&s_uiBrakeTme) >= ACC_BRAKE_TMR) && s_ucBegainBreak)
                {
                    memset((u8 *)&tmpGPSInfo,	0,	sizeof(tmpGPSInfo));
										memcpy((u8 *)&tmpGPSInfo,	(u8 *)&s_gpsPro, sizeof(s_gpsPro));
                    HAL_ZiTaiInQueueProcess(TYPE_ZITAI_DTH,&tmpGPSInfo,s_tmpX,s_tmpY,s_tmpZ,s_tmpXYZ);	
                    #if(DEF_EVENTINFO_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_EVENT)
                        myPrintf("[EVENT]: RAPID_BRAKE!! (X:%-d,Y:%-d,Z:%-d,XYZ:%-d)\r\n\r\n",	s_tmpX,s_tmpY,s_tmpZ,s_tmpXYZ);
                    #endif
                    OSTmr_Count_Start(&s_accPackCounter);	// ��ʼ��̬������˶�ʱ��
                    Max_Acc         = 0;
                    s_ucFilterCnt   = 0u;
                    s_ucBegainBreak = 0u;
                    retCode         = DEF_ACC_RAPID_BRAKE;
                }
                else if ((OSTmr_Count_Get(&s_uiUpTmr) >= ACC_UP_TMR) && s_ucBegainUp)
                {
                    memset((u8 *)&tmpGPSInfo,	0,	sizeof(tmpGPSInfo));
										memcpy((u8 *)&tmpGPSInfo,	(u8 *)&s_gpsPro, sizeof(s_gpsPro));
                    HAL_ZiTaiInQueueProcess(TYPE_ZITAI_ATH,&tmpGPSInfo,s_tmpX,s_tmpY,s_tmpZ,s_tmpXYZ);																	
                    #if(DEF_EVENTINFO_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_EVENT)
                        myPrintf("[EVENT]: RAPID_UP!! (X:%-d,Y:%-d,Z:%-d,XYZ:%-d)\r\n\r\n",	s_tmpX,s_tmpY,s_tmpZ,s_tmpXYZ);
                    #endif
                    OSTmr_Count_Start(&s_accPackCounter);	// ��ʼ��̬������˶�ʱ��
                    Max_Acc       = 0;
                    s_ucBegainUp  = 0u;
                    s_ucFilterCnt = 0u;
                    retCode       = DEF_ACC_RAPID_CRASH;
                }
                else
                {
                }
            }
            else
            {
            }
        }
        else
        {
        }
        
        if(s_ucCreashFlag)
        {
            if(OSTmr_Count_Get(&s_accCrashCounter1) <= 60000)
            {
                if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
                {
                    if(HAL_BSP_GetState(DEF_BSPSTA_GPSSPEED) <= 5)
                    {
                        if(OSTmr_Count_Get(&s_accCrashCounter2) >= 10000)
                        {
                            tmpCrashCode =DEF_ACC_RAPID_CRASH;	// ȷ��Ϊ��ײ�¼�
                        }
                    }
                    else
                    {
                        OSTmr_Count_Start(&s_accCrashCounter2);	// ��ײ���˼�ʱ����ʼ��ʱ	
                    }
                }
                else
                {
                    OSTmr_Count_Start(&s_accCrashCounter2);	// ��ײ���˼�ʱ����ʼ��ʱ	
                }
            }
            else
            {
                tmpCrashCode =DEF_ACC_RAPID_BRAKE;		// ��Ϊ�Ǽ�����
            }
            
            // ���˺��¼����
            if(tmpCrashCode != 0)
            {
                if(tmpCrashCode == DEF_ACC_RAPID_BRAKE)
                {
                    HAL_ZiTaiInQueueProcess(TYPE_ZITAI_DTH,&tmpGPSInfo,s_tmpX,s_tmpY,s_tmpZ,s_tmpXYZ);	
                    #if(DEF_EVENTINFO_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_EVENT)
                        myPrintf("[EVENT]: RAPID_BRAKE!! (X:%-d,Y:%-d,Z:%-d,XYZ:%-d)\r\n\r\n",	s_tmpX,s_tmpY,s_tmpZ,s_tmpXYZ);
                    #endif
                    retCode =DEF_ACC_RAPID_BRAKE;
                    
                }
                else
                {
                    HAL_ZiTaiInQueueProcess(TYPE_ZITAI_PTH,&tmpGPSInfo,s_tmpX,s_tmpY,s_tmpZ,s_tmpXYZ);	
                    #if(DEF_EVENTINFO_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_EVENT)
                        myPrintf("[EVENT]: RAPID_CRASH!! (X:%-d,Y:%-d,Z:%-d,XYZ:%-d)\r\n\r\n",	s_tmpX,s_tmpY,s_tmpZ,s_tmpXYZ);
                    #endif
                    retCode =DEF_ACC_RAPID_CRASH;
                }
                
                OSTmr_Count_Start(&s_accPackCounter);	// ��ʼ��̬������˶�ʱ��
                Max_Acc          = 0;
                s_ucCreashFlag   = 0u;
                s_ucFilterCnt    = 0u;
                s_ucBegainCreash = 0u;
            }
        }
        else
        { 
        }
    }
    return	retCode;
}  
/*
******************************************************************************
* Function Name  : HAL_ACCMoveCheckProcess
* Description    : ���ڼ��ٶȵ��ƶ�״̬�б���
* Input          : 
*								 : *intTimes	: �жϴ���
*								 : sMDuration	: ��ʼ�ƶ��ٶ�ʱ��(s)
*								 : eMDuration	: �����ƶ��ٶ�ʱ��(s)
* Output         : None
* Return         : �ƶ�״̬����1, ֹͣ״̬����0, ����з���2
******************************************************************************
*/
u8	HAL_AccMoveCheckProcess	(vu16 *intTimes,	u16 sMDuration,	u16 eMDuration)
{
    // ���ڼ��ٶȵ��ƶ��ж�
    static	vu8		accMoveStep =0;		// ���ٶ��ƶ��жϽ׶α���
    static	vu32	accMoveCounter	=0;	// ��ʼ�ƶ���ʱ��
    static	vu32	accStopCounter	=0;	// �����ƶ���ʱ��
  
		// ACC �ƶ�����߼�//////////////////////////////////////////////////////////////
		if(accMoveStep == 0)
		{
				accMoveStep =1;				
				OSTmr_Count_Start(&accMoveCounter);	//��ʼ�ƶ���ʱ��	
				OSTmr_Count_Start(&accStopCounter);	//��ʼ�ƶ���ʱ��	
		}
		else if(accMoveStep == 1)
		{
				if(OSTmr_Count_Get(&accMoveCounter) >= (sMDuration * 1000))
				{		
						OSTmr_Count_Start(&accMoveCounter);	
						if(*intTimes >= 1)
						{
								accMoveStep =0;	// �´����¼��
								if(HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0)
								{
										HAL_SYS_SetState(BIT_SYSTEM_MOVE_ACC,	DEF_ENABLE);	// �����ƶ�״̬
									
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[EVENT]: Start move(ACC,%d)!\r\n",	*intTimes);
										#endif									
								}
								*intTimes 	=0;		// ���жϴ���׼������ͳ��
								return 1;	
						}
				}
				if(OSTmr_Count_Get(&accStopCounter) >= (eMDuration * 1000))
				{
						OSTmr_Count_Start(&accStopCounter);	
						if(*intTimes == 0)
						{
								accMoveStep =0;	// �´����¼��
								if(HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 1)
								{								
										HAL_SYS_SetState(BIT_SYSTEM_MOVE_ACC,	DEF_DISABLE);	// ���ƶ�״̬
									
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[EVENT]: Stop move(ACC,%d)!\r\n",	*intTimes);
										#endif			
								}
								return 0;	
						}						
				}			
		}
		else
    {
				accMoveStep =0;				
    }
		return	2;
}

/*****************************************end*********************************************************/



