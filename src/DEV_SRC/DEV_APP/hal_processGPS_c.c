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
static								vu8										firstFixFlag=0;		// GPS上电后第一次定位FLAG，如果定位过则为1否则为0

static								vu8										gpsMoveStep =0,	gpsStopStep =0;					// GPS移动检测阶段变量
static								vu32									gpsMoveCounter=0,	gpsStopCounter =0;		// GSP移动检测计时器

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
* Return         : 两坐标间距离精确到米
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
		BSP_GPSPOW_CON(DEF_DISABLE);	// 停止模块电源供应
		s_gps.sta = 'V';							// 修改GPS为未定位状态
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
		BSP_GPSPOW_CON(DEF_ENABLE);		// 使能模块电源供应
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
		GPS_GPRMC.Sta[0] = 'V';		// 清空GPS接收缓冲区关键状态数据
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
		
		GPS_DEVICE.HardWareSta 	= DEF_GPSHARD_CHEKING;   // 置检测状态
		BSP_GPS_POW_RESET(delayTime);
}
/*
******************************************************************************
* Function Name  : HAL_ConvertGpsData
* Description    : 将从模块读到的GPS数据按照时间约定的算法存到临时存储区中以便
*									 发送数据时的数据拷贝
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
		s32 	 	sintTmp=0;	// 经纬度计算临时变量有负值情况	
		/////////////////////////////////////////////////////  		
		memset(chaTmpBuf,'\0',10); 
  
    /////////////////////////////////////////////////////
		// 转换FIX状态
		/////////////////////////////////////////////////////	
		s_gps.fs	=	GPS_GPGSA.FS[0];		// 注意GGA中FS定义与GSA中略有不同,GGA中无3D定位状态
		
		/////////////////////////////////////////////////////
		// 转换卫星数量
		/////////////////////////////////////////////////////
		s_gps.noSV = atoi((const char *)GPS_GPGGA.NoSV);		
		
		if(GPS_GPRMC.Sta[0] == 'A')		// 2015-9-21 只有定位才更新GPS时间
		{		
        /////////////////////////////////////////////////////
        // 更新gps时间 (定位与否时间都正常更新)
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
						s_gps.gmtTime 	= uint_G;		// 上电后只有GPS定位过一次才更新时间,否则可能得到的时间不对   
						s_gpsGL.gmtTime = uint_G; 	// 时间没有滤波处理每秒都更新
          
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
            // 转换经度 (如果未定位则经度就是最后一次定位的经度)
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
                s_gps.longitude = s_gps.longitude * -1;		//经度负值
      
            /////////////////////////////////////////////////////
            // 转换纬度	(如果未定位则维度就是最后一次定位的纬度)
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
                s_gps.latitude = s_gps.latitude * -1;		//纬度负值
      
            /////////////////////////////////////////////////////
            // 转换速度
            /////////////////////////////////////////////////////	
            floTmp = atof((const char *)GPS_GPRMC.Speed);							//将字符串转成单精度数值
            s_gps.speed	=	(u16)(floTmp * 1.852 * 10);									//转成“公里/小时”并扩大10倍赋值
            s_TmpGps.cunSpeed	=(u16)(floTmp * 1.852 + 0.5);						//更新当前速度信息
        
            /////////////////////////////////////////////////////
            // 转换高度
            /////////////////////////////////////////////////////	
            floTmp 	= (s32)atof((const char *)GPS_GPGGA.MSLHigh);				//将字符串转成单精度数值 			
            s_gps.high  = (s16)(floTmp + 0.5);													//四舍五入

            /////////////////////////////////////////////////////
            // 转换航向
            /////////////////////////////////////////////////////	
            floTmp 	= (s32)atof((const char *)GPS_GPRMC.Azimuth);
            s_gps.heading = (u16)(floTmp + 0.5);												//四舍五入
            s_TmpGps.cunAngle = (u16)(floTmp + 0.5);											//更新当前角度
            
            /////////////////////////////////////////////////////
            // 更新定位状态
            /////////////////////////////////////////////////////
            s_gps.sta = 'A';	
        }
        else
        {
            /////////////////////////////////////////////////////
            // 更新定位状态
            /////////////////////////////////////////////////////
            s_gps.sta = 'V';	
        } 
		}
    else
    {
        /////////////////////////////////////////////////////
        // 更新定位状态
        /////////////////////////////////////////////////////
        s_gps.sta = 'V';
    }
}
/*
******************************************************************************
* Function Name  : HAL_ConvertGpsProData
* Description    : 将GPS底层数据转换成协议数据(如果GPS不定位则将使用系统RTC时间代替GPS时间)
* Input          : 如果GPS不定位则除时间及FS外都将赋值为0
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_ConvertGpsProData	(GPSData_TypeDef *gpdData,	GPSProData_Def *gpsPro)
{
			SYS_DATETIME	tmpRtc;
			memset((u8 *)&tmpRtc,	0, sizeof(tmpRtc));
	
			// 时间，不定位以rtc时间上传
			if(gpdData->sta == 'A')
					gpsPro->gmtTime	=gpdData->gmtTime;	// GPS定位使用GPS时间
			else
			{
					BSP_RTC_Get_Current(&tmpRtc);	
					gpsPro->gmtTime =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);	
			}
			
			// 经纬度，不定位赋值0
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
			
			// 速度过滤处理(2015-8-5 熄火状态下速度不清0，否则设备熄火移动报警速度将都为0)
			if(gpdData->sta != 'A')
					gpsPro->speed	=0;
			else
			{
					// 2015-9-21 GPS速度数值异常判断
					if((gpdData->speed / 10) < 250)
							gpsPro->speed	=gpdData->speed;	// 250 公里以内认为速度合法
					else
					{
							;	// 速度大于250则维持上次速度信息不变
					}
			}

					
			// 高度及方向
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
			
			// ASCII码转数值
			if(gpdData->fs >= 0x30)
					gpsPro->fs = gpdData->fs-0x30;		// 将ASCII码转成数值
			else
					gpsPro->fs = 1;
			
			gpsPro->noSV =gpdData->noSV;

}
/*
******************************************************************************
* Function Name  : HAL_GpsProDataOut
* Description    : 将GPS协议数据通过大端模式输出到内存中(STM32 CM3默认为小端模式)
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
* Description    : 将GPS底层数据转换成协议数据并存储到DSS队列中
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
					gpsPro.fs = gpdData->fs-0x30;		// 将ASCII码转成数值
			else
					gpsPro.fs = 1;
			gpsPro.noSV =gpdData->noSV;
			
			// 数据入队存储
			DssQue_in (&s_qDss,	&gpsPro);
			ret = DssQue_size(&s_qDss);
			return ret;
}
/*
******************************************************************************
* Function Name  : HAL_GpsMoveCheckProcess
* Description    : GSP移动状态判别处理 (GPS定位时调用)
* Input          : 
*								 : enFlag			: 使能检测标志DEF_DISABLE：禁止检测，DEF_ENABLE：使能检测
*								 : cunSpeed		: 当前速度值(km/h)
*								 : sMThreshold: 开始移动速度门限(km/h)
*								 : sMDuration	: 开始移动速度时长(s)
*								 : eMThreshold: 结束移动速度门限(km/h)
*								 : eMDuration	: 结束移动速度时长(s)
* Output         : None
* Return         : 移动状态返回1, 停止状态返回0, 检测中返回2
******************************************************************************
*/
u8	HAL_GpsMoveCheckProcess	(u8	enFlag,	u8 cunSpeed,	u8 sMThreshold,	u8 sMDuration, u8 eMThreshold, u8 eMDuration)
{
		if(enFlag == DEF_DISABLE)
		{
				// 禁止检测复位检测阶段变量
				gpsMoveStep =0;
				gpsStopStep =0;
		}
		else
		{
				// GPS 移动检测逻辑//////////////////////////////////////////////////////////////
				if(cunSpeed >= sMThreshold)
				{
						if(gpsMoveStep == 0)
						{
								gpsMoveStep =1;
								OSTmr_Count_Start(&gpsMoveCounter);	//开始移动计时
						}
						else if(gpsMoveStep == 1)
						{
								if(OSTmr_Count_Get(&gpsMoveCounter) >= (sMDuration * 1000))		
								{
										gpsMoveStep =0;	// 下次重新检测
										if(HAL_SYS_GetState(BIT_SYSTEM_MOVE_GPS) == 1)
										{								
												HAL_SYS_SetState(BIT_SYSTEM_MOVE_GPS,	DEF_DISABLE);	// 清移动状态
											
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

				// GPS 停止检测逻辑//////////////////////////////////////////////////////////////
				if(cunSpeed <= eMThreshold)
				{
						if(gpsStopStep == 0)
						{
								gpsStopStep =1;
								OSTmr_Count_Start(&gpsStopCounter);	//开始移动计时
						}
						else if(gpsStopStep == 1)
						{
								if(OSTmr_Count_Get(&gpsStopCounter) >= (eMDuration * 1000))		
								{
										gpsStopStep =2;
										if(HAL_SYS_GetState(BIT_SYSTEM_MOVE_GPS) == 0)
										{
												HAL_SYS_SetState(BIT_SYSTEM_MOVE_GPS,	DEF_ENABLE);	// 设置移动状态
											
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
//* Description    : 累加GPS公里数处理
//* Input          : 
//										upLong	: 上次经度(单位度乘10的7次方;有符号型东经为“+”西经为“-”;)
//										upLat		：上次纬度(单位度乘10的7次方;有符号型北纬为“+”南纬为“-”;)	
//										cunLong	:	本次经度
//										cunLat	:	本次纬度
//* Output         : None
//* Return         : 参数合法返回‘0’非法返回‘-1’
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
//* Description    : 超速报警检测
//* Input          : 
//										swt			:	报警开关
//										upSpeed	: 上次经度(单位公里/小时;)
//										upLat		：上次纬度
//										cunLong	:	本次经度
//										cunLat	:	本次纬度
//* Output         : None
//* Return         : 有区域报警返回‘1’没有返回‘0’
//******************************************************************************
//*/
//u8	HAL_SpeedAlarmProcess	(u32 swt,	u16 upSpeed,	u16	cunSpeed,	u16	speedLevel)
//{
//		if(HAL_getBspState(DEF_BSPSTA_GPSSTA) == 1)	
//    {
//        if((upSpeed < speedLevel) && (cunSpeed >= speedLevel))
//    		{
//            HAL_setAlarmState(1,BIT_ALARM_OVERSPEED); // 超速报警事件
//    				if(swt & DEF_BIT_SWT_OVERSPEED)
//    				{
//    						s_sysFlag.do_overSpeed	=	TRUE;	
//    				}
//    		}
//    		else
//    				HAL_setAlarmState(0,BIT_ALARM_OVERSPEED); 
//    
//    		//  判断报警并发送
//    		if(s_sysFlag.do_overSpeed	==	TRUE)
//    		{
//    				s_sysFlag.do_overSpeed = FALSE;
//    
//    				// 更新自动上传系统报警状态参数部分
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
* Description    : DSS事件检测(只有定位才检测)
* Input          : 
										upAngle	:	上次方向角
										upSpeed	: 上次速度
										cunAngle:	当前方向角
										cunSpeed: 当前速度
* Output         :  
* Return         : TYPE_DSS_ATH:急加速事件,TYPE_DSS_DTH:急加速事件,TYPE_DSS_RTH:急转弯事件,0:无事件
******************************************************************************
*/
u8	HAL_GpsDssProcess	(u16 upAngle,	u16 upSpeed, u16 cunAngle,	u16 cunSpeed)
{
		float floTmp=0;
		u16 tmpAngle=0;
		if(HAL_SYS_GetState (BIT_SYSTEM_IG) == 1)
		{
				// 急加速事件/////////////
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
				// 急减速事件/////////////
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

				// 急转弯事件/////////////
				tmpAngle = abs(cunAngle - upAngle);	
				if(tmpAngle > 180)
						tmpAngle = 360 - tmpAngle;		

				//////////////////////////				
				if((upSpeed > 10) && (cunSpeed > 10))
				{
						floTmp = fabs(tan(tmpAngle * 3.14 / 180) * cunSpeed * 100 / 36);
						if((floTmp >= DSS_WTH_VALUE) && (abs(cunAngle - upAngle) > 5))		// 急转弯
						{
								if(s_event.dssRTHSta.up == FALSE)
								{
									 	s_event.dssRTHSta.up	= TRUE;
										return	DEF_GPSDSS_WTH;
								}			
						}
						else if((floTmp >= DSS_RTH_VALUE) && (abs(cunAngle - upAngle) > 5))	// 转弯
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
* Description    : DSS GPS定位点出队(将GPS DSS数据单点数据出队处理)
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
* Description    : GPS应用层数据初始化
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_GpsAppInit (void)
{
		DssQue_init(&s_qDss,	DSS_POINT_NUM);
		memset((u8 *)&s_TmpGps,	0,	sizeof(s_TmpGps));	// 用于计算的临时变量
		memset((u8 *)&s_gps,		0,	sizeof(s_gps));			// 原始数据缓冲
		memset((u8 *)&s_gpsBuf,	0,	sizeof(s_gpsBuf));	// 中间过滤存储
		memset((u8 *)&s_gpsGL,	0,	sizeof(s_gpsGL));		// 最终过滤数据
		memset((u8 *)&s_gpsPro,	0,	sizeof(s_gpsPro));	// 用于上传的数据结构
}

/*****************************************end*********************************************************/



