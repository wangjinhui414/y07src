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
*                                     	 hal_ProcessGPS_h.h
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessGPS_h.h
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  HAL_PROCESSGPS_H_H
#define  HAL_PROCESSGPS_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   HAL_PROCESSGPS_GLOBLAS
#define  HAL_PROCESSGPS_EXT
#else
#define  HAL_PROCESSGPS_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include 	"stm32f10x.h"
#include	"bsp_gps_h.h"


/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/
//////////////////////////////////////////////////////////////////////
// GPS滤波点数量定义
#define	GPS_LB_NUM										(3)	 		// GSP上传经纬度滤波处理点数

//////////////////////////////////////////////////////////////////////
// GPS校时定义
#define	RTC_ADJ_TIMES									(3)	 		// 系统校时确认次数

//////////////////////////////////////////////////////////////////////
// DSS功能定义
#define	DSS_POINT_NUM									(5)	 		// GPS DSS保存点个数
#define	DSS_ATH_VALUE									(12)		// 加速门限12km/h
#define	DSS_DTH_VALUE									(12)		// 减速门限12km/h
#define	DSS_RTH_VALUE									(40)		// 转弯速门限4.0
#define	DSS_WTH_VALUE									(90)		// 急转弯速门限9.0

// DSS判断返回值类型定义
#define	DEF_GPSDSS_ATH								((u8)0xA0)		// 加速
#define	DEF_GPSDSS_DTH								((u8)0xA1)		// 减速
#define	DEF_GPSDSS_RTH								((u8)0xA2)		// 转弯
#define	DEF_GPSDSS_WTH								((u8)0xA3)		// 急转弯

//////////////////////////////////////////////////////////////////////
// 基本定义

#define	EARTH_RADIUS 									((double)6378.137) // 单位为公里
#define	PI														((double)3.14)

//////////////////////////////////////////////////////////////////////
// 区域报警类型定义

#define	DEF_ZONE_NULL									('N')
#define	DEF_ZONE_ENTER								('I')
#define	DEF_ZONE_EXIT									('O')
#define	DEF_ZONE_BOTH									('B')

#define	DEF_SHAPE_RECTANGLE						('R')		// 矩形
#define	DEF_SHAPE_CIRCULAR						('C')		// 圆形
#define	DEF_SHAPE_POLYGON							('P')		// 多边形


/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

// GPS传临时数据结构定义
typedef struct	tag_GPSData_Def
{  
    u8			sta;																					// 定位状态'A'定位，'V'不定位
		u8			fs;																						// 绑定状态'1'不定位，'2'2D定位，'3'3D定位
		u8			noSV;																					// 用于定位的卫星数量
		u32			gmtTime;																			// 从1970年的秒数
		s32     latitude;																    	// 以度为单位的纬度值乘以10的7次方，精确到百万分之一度
		s32     longitude;																	  // 以度为单位的经度值乘以10的7次方，精确到百万分之一度
    s16     high;                                         // 海拔高度，以m为单位
    u16     speed;                                        // 以km/h为单位扩大10倍赋值 
    u16     heading;                                      // 0-359,正北为0
						
}GPSData_TypeDef;


// 用于上传的GPS数据结构定义(智信通协议格式)
#pragma pack(1)	// 防止存储器对齐所产生的额外内存暂用，导致数据按协议上传时多传一个或几个0x00
typedef struct	tag_GPSProData_Def
{ 
		u32			gmtTime;																			// 从1970年的秒数
		s32     longitude;																	  // 度+整数分+整数分+小数分的十位+小数分的
		s32     latitude;																    	// 度+整数分+整数分+小数分的十位+小数分的
		u16     speed;                                        // 单位km/h扩大10倍存储
		u8     	heading;                                      // 0-359,正北为0，方向数值除2存储 
		s16     high;                                         // 海拔高度，以m为单位(有符号数)
		u8			fs;																						// 绑定状态1不定位，2,2D定位，3,3D定位		
		u8			noSV;																					// 用于定位的卫星数量
   					
}GPSProData_Def;
#pragma pack()


// DSS队列结构
typedef struct	tag_DSSQue_Def
{
		u16			OSQStart;																			// 队列开始位置
		u16			OSQEnd;																				// 队列结束位置
		u16			OSQIn;																				// 队列头指针
		u16			OSQOut;																				// 队列尾指针
		u16			OSQSize;																			// 队列总大小
		u16			OSQEntries;																		// 队列当前存储数据包数
							
}DSSQue_TypeDef;



// 系统用于计算的的临时变量 ////
typedef struct	tag_TmpGPS_TypeDef
{   		
		u16			cunSpeed;																			// 当前速度信息(公里/小时)
		u16			cunAngle;																			// 当前角度	
		u16			upSpeed;																			// 上次速度数据(公里/小时)
		u16			upAngle;																			// 上次角度
							
}TmpGPS_TypeDef;

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

	
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/
HAL_PROCESSGPS_EXT				TmpGPS_TypeDef			s_TmpGps;				// 用于计算的临时变量
HAL_PROCESSGPS_EXT				GPSData_TypeDef			s_gps;					// 转换后的GPS数据(未过滤的原始数据)
HAL_PROCESSGPS_EXT				GPSData_TypeDef			s_gpsBuf;				// 转换后的GPS数据(GPS过滤中间缓冲区)
HAL_PROCESSGPS_EXT				GPSData_TypeDef			s_gpsGL;				// 转换后的GPS数据(过滤后的数据)
HAL_PROCESSGPS_EXT				GPSProData_Def			s_gpsPro;				// 需要上传的GPS协议数据

HAL_PROCESSGPS_EXT				DSSQue_TypeDef			s_qDss;
HAL_PROCESSGPS_EXT				GPSProData_Def			s_dss[DSS_POINT_NUM];	// 需要上传的GPS协议数据



			
/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void			HAL_GpsAppInit									(void);
void			HAL_GpsModuleSleep							(void);
void			HAL_GpsModuleWork								(void);
void			HAL_GpsModuleReset							(u32	delayTime);
void			HAL_ConvertGpsData							(void);
void			HAL_ConvertGpsProData						(GPSData_TypeDef *gpdData,	GPSProData_Def *gpsPro);
u16				HAL_GpsProDataOut								(u8 *outBuf,	GPSProData_Def *gpsPro);
u16				HAL_SaveDssProData							(GPSData_TypeDef *gpdData);
u8				HAL_GpsDssProcess								(u16 upAngle,	u16 upSpeed, u16 cunAngle,	u16 cunSpeed);
s8				HAL_GpsGetDssData								(GPSProData_Def *gpsPro);
u8				HAL_GpsMoveCheckProcess					(u8	enFlag,	u8 cunSpeed,	u8 sMThreshold,	u8 sMDuration, u8 eMThreshold, u8 eMDuration);

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
