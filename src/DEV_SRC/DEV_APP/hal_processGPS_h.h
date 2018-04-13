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
// GPS�˲�����������
#define	GPS_LB_NUM										(3)	 		// GSP�ϴ���γ���˲��������

//////////////////////////////////////////////////////////////////////
// GPSУʱ����
#define	RTC_ADJ_TIMES									(3)	 		// ϵͳУʱȷ�ϴ���

//////////////////////////////////////////////////////////////////////
// DSS���ܶ���
#define	DSS_POINT_NUM									(5)	 		// GPS DSS��������
#define	DSS_ATH_VALUE									(12)		// ��������12km/h
#define	DSS_DTH_VALUE									(12)		// ��������12km/h
#define	DSS_RTH_VALUE									(40)		// ת��������4.0
#define	DSS_WTH_VALUE									(90)		// ��ת��������9.0

// DSS�жϷ���ֵ���Ͷ���
#define	DEF_GPSDSS_ATH								((u8)0xA0)		// ����
#define	DEF_GPSDSS_DTH								((u8)0xA1)		// ����
#define	DEF_GPSDSS_RTH								((u8)0xA2)		// ת��
#define	DEF_GPSDSS_WTH								((u8)0xA3)		// ��ת��

//////////////////////////////////////////////////////////////////////
// ��������

#define	EARTH_RADIUS 									((double)6378.137) // ��λΪ����
#define	PI														((double)3.14)

//////////////////////////////////////////////////////////////////////
// ���򱨾����Ͷ���

#define	DEF_ZONE_NULL									('N')
#define	DEF_ZONE_ENTER								('I')
#define	DEF_ZONE_EXIT									('O')
#define	DEF_ZONE_BOTH									('B')

#define	DEF_SHAPE_RECTANGLE						('R')		// ����
#define	DEF_SHAPE_CIRCULAR						('C')		// Բ��
#define	DEF_SHAPE_POLYGON							('P')		// �����


/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

// GPS����ʱ���ݽṹ����
typedef struct	tag_GPSData_Def
{  
    u8			sta;																					// ��λ״̬'A'��λ��'V'����λ
		u8			fs;																						// ��״̬'1'����λ��'2'2D��λ��'3'3D��λ
		u8			noSV;																					// ���ڶ�λ����������
		u32			gmtTime;																			// ��1970�������
		s32     latitude;																    	// �Զ�Ϊ��λ��γ��ֵ����10��7�η�����ȷ�������֮һ��
		s32     longitude;																	  // �Զ�Ϊ��λ�ľ���ֵ����10��7�η�����ȷ�������֮һ��
    s16     high;                                         // ���θ߶ȣ���mΪ��λ
    u16     speed;                                        // ��km/hΪ��λ����10����ֵ 
    u16     heading;                                      // 0-359,����Ϊ0
						
}GPSData_TypeDef;


// �����ϴ���GPS���ݽṹ����(����ͨЭ���ʽ)
#pragma pack(1)	// ��ֹ�洢�������������Ķ����ڴ����ã��������ݰ�Э���ϴ�ʱ�ഫһ���򼸸�0x00
typedef struct	tag_GPSProData_Def
{ 
		u32			gmtTime;																			// ��1970�������
		s32     longitude;																	  // ��+������+������+С���ֵ�ʮλ+С���ֵ�
		s32     latitude;																    	// ��+������+������+С���ֵ�ʮλ+С���ֵ�
		u16     speed;                                        // ��λkm/h����10���洢
		u8     	heading;                                      // 0-359,����Ϊ0��������ֵ��2�洢 
		s16     high;                                         // ���θ߶ȣ���mΪ��λ(�з�����)
		u8			fs;																						// ��״̬1����λ��2,2D��λ��3,3D��λ		
		u8			noSV;																					// ���ڶ�λ����������
   					
}GPSProData_Def;
#pragma pack()


// DSS���нṹ
typedef struct	tag_DSSQue_Def
{
		u16			OSQStart;																			// ���п�ʼλ��
		u16			OSQEnd;																				// ���н���λ��
		u16			OSQIn;																				// ����ͷָ��
		u16			OSQOut;																				// ����βָ��
		u16			OSQSize;																			// �����ܴ�С
		u16			OSQEntries;																		// ���е�ǰ�洢���ݰ���
							
}DSSQue_TypeDef;



// ϵͳ���ڼ���ĵ���ʱ���� ////
typedef struct	tag_TmpGPS_TypeDef
{   		
		u16			cunSpeed;																			// ��ǰ�ٶ���Ϣ(����/Сʱ)
		u16			cunAngle;																			// ��ǰ�Ƕ�	
		u16			upSpeed;																			// �ϴ��ٶ�����(����/Сʱ)
		u16			upAngle;																			// �ϴνǶ�
							
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
HAL_PROCESSGPS_EXT				TmpGPS_TypeDef			s_TmpGps;				// ���ڼ������ʱ����
HAL_PROCESSGPS_EXT				GPSData_TypeDef			s_gps;					// ת�����GPS����(δ���˵�ԭʼ����)
HAL_PROCESSGPS_EXT				GPSData_TypeDef			s_gpsBuf;				// ת�����GPS����(GPS�����м仺����)
HAL_PROCESSGPS_EXT				GPSData_TypeDef			s_gpsGL;				// ת�����GPS����(���˺������)
HAL_PROCESSGPS_EXT				GPSProData_Def			s_gpsPro;				// ��Ҫ�ϴ���GPSЭ������

HAL_PROCESSGPS_EXT				DSSQue_TypeDef			s_qDss;
HAL_PROCESSGPS_EXT				GPSProData_Def			s_dss[DSS_POINT_NUM];	// ��Ҫ�ϴ���GPSЭ������



			
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
