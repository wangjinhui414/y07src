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
*                                        APPLICATION CODE
*
*                                     	 hal_ProcessGPS_c.c
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessGPS_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  HAL_PROCESSGPS_GLOBLAS
#include	<string.h>
#include	<stdio.h>
#include  <stdlib.h>
#include	<math.h>
#include  "cfg_h.h"
#include	"main_h.h"
#include	"bsp_h.h"
#include 	"bsp_rtc_h.h"
#include  "bsp_gps_h.h"
#include	"hal_h.h"
#include 	"hal_protocol_h.h"
#include	"hal_ProcessGPS_h.h"
#include	"hal_ProcessSYS_h.h"
#include 	"hal_processDBG_h.h"
#include 	"hal_processQUE_h.h"

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

static								vu8										adjtimes =0;
static								vu8										firstFixFlag=0;		// GPS�ϵ���һ�ζ�λFLAG�������λ����Ϊ1����Ϊ0

static								vu8										gpsMoveStep =0,	gpsStopStep =0;					// GPS�ƶ����׶α���
static								vu32									gpsMoveCounter=0,	gpsStopCounter =0;		// GSP�ƶ�����ʱ��

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static		void			DssQue_init 		(DSSQue_TypeDef	*que,	u16	size);
static		u16	 			DssQue_size 		(DSSQue_TypeDef	*que);
static		s8				DssQue_in 			(DSSQue_TypeDef	*que,	GPSProData_Def *gpsPro);
static		s8				DssQue_out			(DSSQue_TypeDef	*que,	GPSProData_Def *gpsPro);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : DssQue_init()
* Description    : 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
static 	void	DssQue_init (DSSQue_TypeDef	*que,	u16	size)
{
		que->OSQStart        = 0; 	
		que->OSQEnd          = size;	
		que->OSQIn           = 0;	
		que->OSQOut          = 0;	
		que->OSQSize         = size;	
		que->OSQEntries      = 0;
}
/*
******************************************************************************
* Function Name  : HAL_DssQue_size()
* Description    : 
* Input          : None
* Output         : None
* Return         : len
******************************************************************************
*/
static	u16	 DssQue_size (DSSQue_TypeDef	*que)
{
		return	(que->OSQEntries);
}
/*
******************************************************************************
* Function Name  : HAL_DssQue_in()
* Description    : 
* Input          : None
* Output         : None
* Return         : DEF_QUE_OK;DEF_QUE_FULL;DEF_QUE_EMPTY;DEF_QUE_ERROR;DEF_QUE_OVERFLOW
******************************************************************************
*/
static	s8	DssQue_in (DSSQue_TypeDef	*que,	GPSProData_Def *gpsPro)
{		
		if (que->OSQEntries >= que->OSQSize) 
		{ 
				memcpy((u8 *)&s_dss[que->OSQIn],	(u8 *)gpsPro,	sizeof(GPSProData_Def));
				que->OSQIn++;
				if (que->OSQIn == que->OSQEnd) 
				{
		    		que->OSQIn = que->OSQStart;
		    }
				que->OSQOut++;
				if (que->OSQOut == que->OSQEnd) 
				{
			  		que->OSQOut = que->OSQStart;
			  }		
				return	DEF_PROQUE_QUE_OK;
	  } 
		else 
		{
				memcpy((u8 *)&s_dss[que->OSQIn],	(u8 *)gpsPro,	sizeof(GPSProData_Def));
				que->OSQIn++;
		    que->OSQEntries++;
		    if (que->OSQIn == que->OSQEnd) 
				{
		       	que->OSQIn = que->OSQStart;
		    }	 	
	  }
		return	DEF_PROQUE_QUE_OK;
}

/*
******************************************************************************
* Function Name  : DssQue_out()
* Description    : len <= PRA_MAXOF_DSSPACK
* Input          : None
* Output         : None
* Return         : DEF_QUE_IS_EMPTY, DEF_QUE_IS_SUCCESS, DEF_FRAM_ERROR
******************************************************************************
*/
static	s8	DssQue_out(DSSQue_TypeDef	*que,	GPSProData_Def *gpsPro)
{
		if (que->OSQEntries != 0) 
		{
				memcpy((u8 *)gpsPro,	(u8 *)&s_dss[que->OSQOut],	sizeof(GPSProData_Def));
				que->OSQOut++;
			  que->OSQEntries--;
			  if (que->OSQOut == que->OSQEnd) 
				{
			      que->OSQOut = que->OSQStart;
			  }	 				 	
				return	DEF_PROQUE_QUE_OK;
	  } 
		else 
			 	return  DEF_PROQUE_QUE_EMPTY;		
}

/*
*********************************************************************************************************
*                                     GPS APP FUNCTION
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : rad
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
static	float	rad(float d)
{
		return	(d * PI / 180.0);
}
/*
******************************************************************************
* Function Name  : getDistance
* Description    : 
* Input          : None
* Output         : None
* Return         : ���������뾫ȷ����
******************************************************************************
*/
static float getDistance(float lng1, float lat1, float lng2, float lat2)
{
		float radLat1 = rad(lat1);
		float radLat2 = rad(lat2);
		float a = radLat1 - radLat2;
		float b = rad(lng1) - rad(lng2);
		float s = 2 * sin(sqrt(pow(sin(a/2),2) + cos(radLat1)*cos(radLat2)*pow(sin(b/2),2)));
		s = s * EARTH_RADIUS;
		return	(s * 1000);
}
/*
******************************************************************************
* Function Name  : HAL_GpsModuleSleep
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_GpsModuleSleep	(void)
{
		BSP_GPSPOW_CON(DEF_DISABLE);	// ֹͣģ���Դ��Ӧ
		s_gps.sta = 'V';							// �޸�GPSΪδ��λ״̬
		s_gps.fs	= '1';	
		s_gps.noSV	= 0;
	
		s_gpsBuf.sta = 'V';
		s_gpsBuf.fs = '1';
		s_gpsBuf.noSV = 0;
	
		s_gpsGL.sta = 'V';
		s_gpsGL.fs = '1';
		s_gpsGL.noSV = 0;
	
		s_gpsPro.fs ='1';
		s_gpsPro.noSV = 0;
}
/*
******************************************************************************
* Function Name  : HAL_GpsModuleWork
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_GpsModuleWork	(void)
{
		BSP_GPSPOW_CON(DEF_ENABLE);		// ʹ��ģ���Դ��Ӧ
}
/*
******************************************************************************
* Function Name  : HAL_GpsModuleReset
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_GpsModuleReset	(u32	delayTime)
{
		GPS_GPRMC.Sta[0] = 'V';		// ���GPS���ջ������ؼ�״̬����
		GPS_GPGGA.FS[0]	 = '1';							
		s_gps.sta = 'V';
		s_gps.fs = '1';
		s_gps.noSV = 0;
	
		s_gpsBuf.sta = 'V';
		s_gpsBuf.fs = '1';
		s_gpsBuf.noSV = 0;
	
		s_gpsGL.sta = 'V';
		s_gpsGL.fs = '1';
		s_gpsGL.noSV = 0;
	
		s_gpsPro.fs ='1';
		s_gpsPro.noSV = 0;
		
		GPS_DEVICE.HardWareSta 	= DEF_GPSHARD_CHEKING;   // �ü��״̬
		BSP_GPS_POW_RESET(delayTime);
}
/*
******************************************************************************
* Function Name  : HAL_ConvertGpsData
* Description    : ����ģ�������GPS���ݰ���ʱ��Լ�����㷨�浽��ʱ�洢�����Ա�
*									 ��������ʱ�����ݿ���
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_ConvertGpsData	(void)
{
		SYS_DATETIME		rtcTime,gpsTime;
		u32			uint_G=0,uint_R=0;
		u8   		chaTmpBuf[10]="",	p0=0,p1=0,p2=0,p3=0; 		

		//double	douTmpDu=0,douTmpFen=0,douTmp=0;	
		float	floTmp=0;
		s32 	 	sintTmp=0;	// ��γ�ȼ�����ʱ�����и�ֵ���	
		/////////////////////////////////////////////////////  		
		memset(chaTmpBuf,'\0',10); 
  
    /////////////////////////////////////////////////////
		// ת��FIX״̬
		/////////////////////////////////////////////////////	
		s_gps.fs	=	GPS_GPGSA.FS[0];		// ע��GGA��FS������GSA�����в�ͬ,GGA����3D��λ״̬
		
		/////////////////////////////////////////////////////
		// ת����������
		/////////////////////////////////////////////////////
		s_gps.noSV = atoi((const char *)GPS_GPGGA.NoSV);		
		
		if(GPS_GPRMC.Sta[0] == 'A')		// 2015-9-21 ֻ�ж�λ�Ÿ���GPSʱ��
		{		
        /////////////////////////////////////////////////////
        // ����gpsʱ�� (��λ���ʱ�䶼��������)
        /////////////////////////////////////////////////////
				chaTmpBuf[0]= GPS_GPRMC.Date[4];
				chaTmpBuf[1]= GPS_GPRMC.Date[5];  
				gpsTime.year= atoi((const char *)chaTmpBuf) + 2000;
				chaTmpBuf[0]= GPS_GPRMC.Date[2];
				chaTmpBuf[1]= GPS_GPRMC.Date[3];
				gpsTime.month = atoi((const char *)chaTmpBuf);
				chaTmpBuf[0]= GPS_GPRMC.Date[0];
				chaTmpBuf[1]= GPS_GPRMC.Date[1];
				gpsTime.day = atoi((const char *)chaTmpBuf);
			
				chaTmpBuf[0]= GPS_GPRMC.Time[0];
				chaTmpBuf[1]= GPS_GPRMC.Time[1];  
				gpsTime.hour = atoi((const char *)chaTmpBuf);
				chaTmpBuf[0]= GPS_GPRMC.Time[2];
				chaTmpBuf[1]= GPS_GPRMC.Time[3];  
				gpsTime.minute = atoi((const char *)chaTmpBuf);
				chaTmpBuf[0]= GPS_GPRMC.Time[4];
				chaTmpBuf[1]= GPS_GPRMC.Time[5];  
				gpsTime.second = atoi((const char *)chaTmpBuf);
		
				uint_G	=	Mktime(gpsTime.year, gpsTime.month, gpsTime.day, gpsTime.hour, gpsTime.minute, gpsTime.second);	

				if(gpsTime.year >= 2015u)
				{
						s_gps.gmtTime 	= uint_G;		// �ϵ��ֻ��GPS��λ��һ�βŸ���ʱ��,������ܵõ���ʱ�䲻��   
						s_gpsGL.gmtTime = uint_G; 	// ʱ��û���˲�����ÿ�붼����
          
            BSP_RTC_Get_Current(&rtcTime);	
            uint_R	=	Mktime(rtcTime.year, rtcTime.month, rtcTime.day, rtcTime.hour, rtcTime.minute, rtcTime.second);	

            if(((uint_G > uint_R) && ((uint_G - uint_R) > 10)) || ((uint_R > uint_G) && ((uint_R - uint_G) > 10)))
            {
                adjtimes++;
                if(adjtimes >= RTC_ADJ_TIMES)
                {
                    adjtimes =0;																			

                    BSP_RTC_Set_Current(&gpsTime);
                    #if(DEF_EVENTINFO_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_EVENT)
                        myPrintf("[EVENT]: RTC-READJUST!\r\n");
                    #endif		
                }									
            }
            else
            {
                adjtimes =0;
            }  	 
		
            /////////////////////////////////////////////////////
            // ת������ (���δ��λ�򾭶Ⱦ������һ�ζ�λ�ľ���)
            /////////////////////////////////////////////////////
            // 13233.9159 dddmm.mmmm 	
            sintTmp	= (s32)(atof((const char *)GPS_GPRMC.Longitude) * 10000);			
            p3	= (u8)(sintTmp % 100);					
            sintTmp =sintTmp/100;
            p2  = (u8)(sintTmp % 100);
            sintTmp =sintTmp/100;
            p1  = (u8)(sintTmp % 100);
            sintTmp =sintTmp/100;
            p0  = sintTmp;
            s_gps.longitude = ((p0 << 24) | (p1 << 16) | (p2 << 8) | p3);
            if((GPS_GPRMC.E_W[0] == 'W') || (GPS_GPRMC.E_W[0] == 'w'))
                s_gps.longitude = s_gps.longitude * -1;		//���ȸ�ֵ
      
            /////////////////////////////////////////////////////
            // ת��γ��	(���δ��λ��ά�Ⱦ������һ�ζ�λ��γ��)
            /////////////////////////////////////////////////////
            // 4717.11399  dddmm.mmmm 	
            sintTmp	= (s32)(atof((const char *)GPS_GPRMC.Latitude) * 10000);			
            p3	= (u8)(sintTmp % 100);					
            sintTmp =sintTmp/100;
            p2  = (u8)(sintTmp % 100);
            sintTmp =sintTmp/100;
            p1  = (u8)(sintTmp % 100);
            sintTmp =sintTmp/100;
            p0  = sintTmp;
            s_gps.latitude = ((p0 << 24) | (p1 << 16) | (p2 << 8) | p3);
            if((GPS_GPRMC.N_S[0] == 'S') || (GPS_GPRMC.N_S[0] == 's'))
                s_gps.latitude = s_gps.latitude * -1;		//γ�ȸ�ֵ
      
            /////////////////////////////////////////////////////
            // ת���ٶ�
            /////////////////////////////////////////////////////	
            floTmp = atof((const char *)GPS_GPRMC.Speed);							//���ַ���ת�ɵ�������ֵ
            s_gps.speed	=	(u16)(floTmp * 1.852 * 10);									//ת�ɡ�����/Сʱ��������10����ֵ
            s_TmpGps.cunSpeed	=(u16)(floTmp * 1.852 + 0.5);						//���µ�ǰ�ٶ���Ϣ
        
            /////////////////////////////////////////////////////
            // ת���߶�
            /////////////////////////////////////////////////////	
            floTmp 	= (s32)atof((const char *)GPS_GPGGA.MSLHigh);				//���ַ���ת�ɵ�������ֵ 			
            s_gps.high  = (s16)(floTmp + 0.5);													//��������

            /////////////////////////////////////////////////////
            // ת������
            /////////////////////////////////////////////////////	
            floTmp 	= (s32)atof((const char *)GPS_GPRMC.Azimuth);
            s_gps.heading = (u16)(floTmp + 0.5);												//��������
            s_TmpGps.cunAngle = (u16)(floTmp + 0.5);											//���µ�ǰ�Ƕ�
            
            /////////////////////////////////////////////////////
            // ���¶�λ״̬
            /////////////////////////////////////////////////////
            s_gps.sta = 'A';	
        }
        else
        {
            /////////////////////////////////////////////////////
            // ���¶�λ״̬
            /////////////////////////////////////////////////////
            s_gps.sta = 'V';	
        } 
		}
    else
    {
        /////////////////////////////////////////////////////
        // ���¶�λ״̬
        /////////////////////////////////////////////////////
        s_gps.sta = 'V';
    }
}
/*
******************************************************************************
* Function Name  : HAL_ConvertGpsProData
* Description    : ��GPS�ײ�����ת����Э������(���GPS����λ��ʹ��ϵͳRTCʱ�����GPSʱ��)
* Input          : ���GPS����λ���ʱ�估FS�ⶼ����ֵΪ0
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_ConvertGpsProData	(GPSData_TypeDef *gpdData,	GPSProData_Def *gpsPro)
{
			SYS_DATETIME	tmpRtc;
			memset((u8 *)&tmpRtc,	0, sizeof(tmpRtc));
	
			// ʱ�䣬����λ��rtcʱ���ϴ�
			if(gpdData->sta == 'A')
					gpsPro->gmtTime	=gpdData->gmtTime;	// GPS��λʹ��GPSʱ��
			else
			{
					BSP_RTC_Get_Current(&tmpRtc);	
					gpsPro->gmtTime =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);	
			}
			
			// ��γ�ȣ�����λ��ֵ0
			if(gpdData->sta == 'A')
			{
					gpsPro->longitude	=gpdData->longitude;
					gpsPro->latitude	=gpdData->latitude;
			}
			else
			{
					gpsPro->longitude	=0;
					gpsPro->latitude	=0;
			}
			
			// �ٶȹ��˴���(2015-8-5 Ϩ��״̬���ٶȲ���0�������豸Ϩ���ƶ������ٶȽ���Ϊ0)
			if(gpdData->sta != 'A')
					gpsPro->speed	=0;
			else
			{
					// 2015-9-21 GPS�ٶ���ֵ�쳣�ж�
					if((gpdData->speed / 10) < 250)
							gpsPro->speed	=gpdData->speed;	// 250 ����������Ϊ�ٶȺϷ�
					else
					{
							;	// �ٶȴ���250��ά���ϴ��ٶ���Ϣ����
					}
			}

					
			// �߶ȼ�����
			if(gpdData->sta == 'A')
			{
					gpsPro->heading		=(u8)(gpdData->heading/2 + 0.5);
					gpsPro->high			=gpdData->high;	
			}
			else
			{
					gpsPro->heading		=0;
					gpsPro->high			=0;	
			}
			
			// ASCII��ת��ֵ
			if(gpdData->fs >= 0x30)
					gpsPro->fs = gpdData->fs-0x30;		// ��ASCII��ת����ֵ
			else
					gpsPro->fs = 1;
			
			gpsPro->noSV =gpdData->noSV;

}
/*
******************************************************************************
* Function Name  : HAL_GpsProDataOut
* Description    : ��GPSЭ������ͨ�����ģʽ������ڴ���(STM32 CM3Ĭ��ΪС��ģʽ)
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
u16	HAL_GpsProDataOut	(u8 *outBuf,	GPSProData_Def *gpsPro)
{
			u32 tmpData32=0;
			u16 tmpLen=0,tmpData16=0;
			
			tmpData32 =REV32(gpsPro->gmtTime); 
			memcpy((outBuf + tmpLen),(u8 *)&tmpData32,	4);		
			tmpLen +=4;
			tmpData32 =REV32(gpsPro->longitude); 
			memcpy((outBuf + tmpLen),(u8 *)&tmpData32,	4);
			tmpLen +=4;
			tmpData32 =REV32(gpsPro->latitude); 
			memcpy((outBuf + tmpLen),(u8 *)&tmpData32,	4);
			tmpLen +=4;	
			tmpData16 =REV16(gpsPro->speed); 
			memcpy((outBuf + tmpLen),(u8 *)&tmpData16,	2);
			tmpLen +=2;
			memcpy((outBuf + tmpLen),&gpsPro->heading,	1);
			tmpLen +=1;
			tmpData16 =REV16(gpsPro->high); 
			memcpy((outBuf + tmpLen),(u8 *)&tmpData16,	2);
			tmpLen +=2;
			memcpy((outBuf + tmpLen),&gpsPro->fs,	1);
			tmpLen +=1;
			memcpy((outBuf + tmpLen),&gpsPro->noSV,	1);
			tmpLen +=1;
			return tmpLen;
}
/*
******************************************************************************
* Function Name  : HAL_SaveDssProData
* Description    : ��GPS�ײ�����ת����Э�����ݲ��洢��DSS������
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
u16	HAL_SaveDssProData	(GPSData_TypeDef *gpdData)
{
			u16 ret=0;
			GPSProData_Def gpsPro;
	
			memset((u8 *)&gpsPro,	0,	sizeof(gpsPro));
	
			gpsPro.gmtTime		=gpdData->gmtTime;
			gpsPro.longitude	=gpdData->longitude;
			gpsPro.latitude		=gpdData->latitude;
			gpsPro.speed			=gpdData->speed;
			gpsPro.heading		=(u8)(gpdData->heading/2 + 0.5);
			gpsPro.high				=gpdData->high;	
			if(gpdData->fs >= 0x30)
					gpsPro.fs = gpdData->fs-0x30;		// ��ASCII��ת����ֵ
			else
					gpsPro.fs = 1;
			gpsPro.noSV =gpdData->noSV;
			
			// ������Ӵ洢
			DssQue_in (&s_qDss,	&gpsPro);
			ret = DssQue_size(&s_qDss);
			return ret;
}
/*
******************************************************************************
* Function Name  : HAL_GpsMoveCheckProcess
* Description    : GSP�ƶ�״̬�б��� (GPS��λʱ����)
* Input          : 
*								 : enFlag			: ʹ�ܼ���־DEF_DISABLE����ֹ��⣬DEF_ENABLE��ʹ�ܼ��
*								 : cunSpeed		: ��ǰ�ٶ�ֵ(km/h)
*								 : sMThreshold: ��ʼ�ƶ��ٶ�����(km/h)
*								 : sMDuration	: ��ʼ�ƶ��ٶ�ʱ��(s)
*								 : eMThreshold: �����ƶ��ٶ�����(km/h)
*								 : eMDuration	: �����ƶ��ٶ�ʱ��(s)
* Output         : None
* Return         : �ƶ�״̬����1, ֹͣ״̬����0, ����з���2
******************************************************************************
*/
u8	HAL_GpsMoveCheckProcess	(u8	enFlag,	u8 cunSpeed,	u8 sMThreshold,	u8 sMDuration, u8 eMThreshold, u8 eMDuration)
{
		if(enFlag == DEF_DISABLE)
		{
				// ��ֹ��⸴λ���׶α���
				gpsMoveStep =0;
				gpsStopStep =0;
		}
		else
		{
				// GPS �ƶ�����߼�//////////////////////////////////////////////////////////////
				if(cunSpeed >= sMThreshold)
				{
						if(gpsMoveStep == 0)
						{
								gpsMoveStep =1;
								OSTmr_Count_Start(&gpsMoveCounter);	//��ʼ�ƶ���ʱ
						}
						else if(gpsMoveStep == 1)
						{
								if(OSTmr_Count_Get(&gpsMoveCounter) >= (sMDuration * 1000))		
								{
										gpsMoveStep =0;	// �´����¼��
										if(HAL_SYS_GetState(BIT_SYSTEM_MOVE_GPS) == 1)
										{								
												HAL_SYS_SetState(BIT_SYSTEM_MOVE_GPS,	DEF_DISABLE);	// ���ƶ�״̬
											
												#if(DEF_EVENTINFO_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_EVENT)
														myPrintf("[EVENT]: Stop move(GPS,%d)!\r\n",	cunSpeed);
												#endif			
										}
										return 1;	
								}
						}
						else
								gpsMoveStep =0;		
				}
				else
						gpsMoveStep =0;		

				// GPS ֹͣ����߼�//////////////////////////////////////////////////////////////
				if(cunSpeed <= eMThreshold)
				{
						if(gpsStopStep == 0)
						{
								gpsStopStep =1;
								OSTmr_Count_Start(&gpsStopCounter);	//��ʼ�ƶ���ʱ
						}
						else if(gpsStopStep == 1)
						{
								if(OSTmr_Count_Get(&gpsStopCounter) >= (eMDuration * 1000))		
								{
										gpsStopStep =2;
										if(HAL_SYS_GetState(BIT_SYSTEM_MOVE_GPS) == 0)
										{
												HAL_SYS_SetState(BIT_SYSTEM_MOVE_GPS,	DEF_ENABLE);	// �����ƶ�״̬
											
												#if(DEF_EVENTINFO_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_EVENT)
														myPrintf("[EVENT]: Start move(GPS,%d)!\r\n",	cunSpeed);
												#endif									
										}
										return 0;
								}
						}
				}
				else
						gpsStopStep =0;		
		}
		return	2;
}
///*
//******************************************************************************
//* Function Name  : HAL_GPSdisProcess
//* Description    : �ۼ�GPS����������
//* Input          : 
//										upLong	: �ϴξ���(��λ�ȳ�10��7�η�;�з����Ͷ���Ϊ��+������Ϊ��-��;)
//										upLat		���ϴ�γ��(��λ�ȳ�10��7�η�;�з����ͱ�γΪ��+����γΪ��-��;)	
//										cunLong	:	���ξ���
//										cunLat	:	����γ��
//* Output         : None
//* Return         : �����Ϸ����ء�0���Ƿ����ء�-1��
//******************************************************************************
//*/
//s8	HAL_GPSdisProcess	(s32 upLong,	s32	upLat,	s32	cunLong,	s32	cunLat)
//{						
//		if(HAL_getBspState(DEF_BSPSTA_GPSSTA) == 1)	
//		{
//				if((upLong != 0) && (upLat != 0))
//				{							
//						s_Comon.gpsDis += getDistance((upLong / 10000000.0), (upLat / 10000000.0), (cunLong / 10000000.0), (cunLat / 10000000.0)) / 1000.0;										  
//						return	0;
//				}
//				else
//						return	-1;
//		}	
//		else	
//				return	-1;		
//}		
///*
//******************************************************************************
//* Function Name  : HAL_SpeedAlarmProcess
//* Description    : ���ٱ������
//* Input          : 
//										swt			:	��������
//										upSpeed	: �ϴξ���(��λ����/Сʱ;)
//										upLat		���ϴ�γ��
//										cunLong	:	���ξ���
//										cunLat	:	����γ��
//* Output         : None
//* Return         : �����򱨾����ء�1��û�з��ء�0��
//******************************************************************************
//*/
//u8	HAL_SpeedAlarmProcess	(u32 swt,	u16 upSpeed,	u16	cunSpeed,	u16	speedLevel)
//{
//		if(HAL_getBspState(DEF_BSPSTA_GPSSTA) == 1)	
//    {
//        if((upSpeed < speedLevel) && (cunSpeed >= speedLevel))
//    		{
//            HAL_setAlarmState(1,BIT_ALARM_OVERSPEED); // ���ٱ����¼�
//    				if(swt & DEF_BIT_SWT_OVERSPEED)
//    				{
//    						s_sysFlag.do_overSpeed	=	TRUE;	
//    				}
//    		}
//    		else
//    				HAL_setAlarmState(0,BIT_ALARM_OVERSPEED); 
//    
//    		//  �жϱ���������
//    		if(s_sysFlag.do_overSpeed	==	TRUE)
//    		{
//    				s_sysFlag.do_overSpeed = FALSE;
//    
//    				// �����Զ��ϴ�ϵͳ����״̬��������
//            HAL_CommonMakePackProcess(DEF_TYPE_EVENT);
//    				return	1;	
//    		}
//    		else
//    				return	0;
//     }
//     return 0;
//}

/*
******************************************************************************
* Function Name  : HAL_GpsDssProcess
* Description    : DSS�¼����(ֻ�ж�λ�ż��)
* Input          : 
										upAngle	:	�ϴη����
										upSpeed	: �ϴ��ٶ�
										cunAngle:	��ǰ�����
										cunSpeed: ��ǰ�ٶ�
* Output         :  
* Return         : TYPE_DSS_ATH:�������¼�,TYPE_DSS_DTH:�������¼�,TYPE_DSS_RTH:��ת���¼�,0:���¼�
******************************************************************************
*/
u8	HAL_GpsDssProcess	(u16 upAngle,	u16 upSpeed, u16 cunAngle,	u16 cunSpeed)
{
		float floTmp=0;
		u16 tmpAngle=0;
		if(HAL_SYS_GetState (BIT_SYSTEM_IG) == 1)
		{
				// �������¼�/////////////
				if((cunSpeed > upSpeed) && ((cunSpeed - upSpeed) <= 25) && ((cunSpeed - upSpeed) >= DSS_ATH_VALUE)) 
				{
						if(s_event.dssATHSta.up == FALSE)
						{
							 	s_event.dssATHSta.up	= TRUE;
								return	DEF_GPSDSS_ATH;
						}		
				}
				else
				{
						if(s_event.dssATHSta.up == TRUE)
								s_event.dssATHSta.up	= FALSE;
				}	
				// �������¼�/////////////
				if((cunSpeed < upSpeed) && ((upSpeed - cunSpeed) >= DSS_DTH_VALUE))
				{
						if(s_event.dssDTHSta.up == FALSE)
						{
							 	s_event.dssDTHSta.up	= TRUE;
								return	DEF_GPSDSS_DTH;
						}		
				}
				else
				{
						if(s_event.dssDTHSta.up == TRUE)
								s_event.dssDTHSta.up	= FALSE;
				}	

				// ��ת���¼�/////////////
				tmpAngle = abs(cunAngle - upAngle);	
				if(tmpAngle > 180)
						tmpAngle = 360 - tmpAngle;		

				//////////////////////////				
				if((upSpeed > 10) && (cunSpeed > 10))
				{
						floTmp = fabs(tan(tmpAngle * 3.14 / 180) * cunSpeed * 100 / 36);
						if((floTmp >= DSS_WTH_VALUE) && (abs(cunAngle - upAngle) > 5))		// ��ת��
						{
								if(s_event.dssRTHSta.up == FALSE)
								{
									 	s_event.dssRTHSta.up	= TRUE;
										return	DEF_GPSDSS_WTH;
								}			
						}
						else if((floTmp >= DSS_RTH_VALUE) && (abs(cunAngle - upAngle) > 5))	// ת��
						{
								if(s_event.dssRTHSta.up == FALSE)
								{
									 	s_event.dssRTHSta.up	= TRUE;
										return	DEF_GPSDSS_RTH;
								}								
						}
						else
						{
								if(s_event.dssRTHSta.up == TRUE)
										s_event.dssRTHSta.up	= FALSE;
						}	
				}	
		}
		return	0;
}
/*
******************************************************************************
* Function Name  : HAL_GpsGetDssData
* Description    : DSS GPS��λ�����(��GPS DSS���ݵ������ݳ��Ӵ���)
* Input          : 
* Output         :  
* Return         :
******************************************************************************
*/
s8	HAL_GpsGetDssData	(GPSProData_Def *gpsPro)
{
		s8	ret =0;
		ret =DssQue_out(&s_qDss,	gpsPro);
		return	ret;
}
/*
******************************************************************************
* Function Name  : HAL_GpsAppInit
* Description    : GPSӦ�ò����ݳ�ʼ��
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_GpsAppInit (void)
{
		DssQue_init(&s_qDss,	DSS_POINT_NUM);
		memset((u8 *)&s_TmpGps,	0,	sizeof(s_TmpGps));	// ���ڼ������ʱ����
		memset((u8 *)&s_gps,		0,	sizeof(s_gps));			// ԭʼ���ݻ���
		memset((u8 *)&s_gpsBuf,	0,	sizeof(s_gpsBuf));	// �м���˴洢
		memset((u8 *)&s_gpsGL,	0,	sizeof(s_gpsGL));		// ���չ�������
		memset((u8 *)&s_gpsPro,	0,	sizeof(s_gpsPro));	// �����ϴ������ݽṹ
}

/*****************************************end*********************************************************/



