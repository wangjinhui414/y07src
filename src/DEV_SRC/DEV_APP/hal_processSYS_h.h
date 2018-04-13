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
*                                     	  hal_ProcessSYS_h.h
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessSYS_h.h
* Version       : V1.00
* Programmer(s) : BAN
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

#ifndef  HAL_PROCESSSYS_H_H
#define  HAL_PROCESSSYS_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   HAL_PROCESSSYS_GLOBLAS
#define  HAL_PROCESSSYS_EXT
#else
#define  HAL_PROCESSSYS_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include 	"stm32f10x.h"



/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// ����״̬��������glob_igSta 
#define	DEF_IG_RPM_START							((u8)0x11)					// RPM��⿪ʼ
#define	DEF_IG_RPM_0									((u16)0x0000)					// RPM<100
#define	DEF_IG_RPM_100								((u16)100)					// RPM>=100
#define	DEF_IG_ECU_TIMEOUT						((u16)0xFFFF)					// ����æ/��ʱ


//////////////////////////////////////////////////////////////////////
// ϵͳ�ڲ���-ϵͳ״̬λ��������s_Sys.sysState 

#define	BIT_SYSTEM_POW					      ((u32)0x00000001)		// 0:��ع���;1:�ⲿ��Դ����
#define	BIT_SYSTEM_POW24					    ((u32)0x00000002)		// 0:12Vϵͳ����;1:24Vϵͳ����
#define	BIT_SYSTEM_ON				      		((u32)0x00000004)		// 0:ON��;1:ON��
#define	BIT_SYSTEM_IG					        ((u32)0x00000008)		// 0:Ϩ��;1:���
#define	BIT_SYSTEM_MOVE_ACC			      ((u32)0x00000010)		// 0:ֹͣ״̬1:�ƶ�״̬(�ɼ��ٶ��ж�)
#define	BIT_SYSTEM_IDLE					      ((u32)0x00000020)		// 0:����ƶ�:1:���ͣ��(IDLE)
#define	BIT_SYSTEM_LOCK					      ((u32)0x00000040)		// 0:����;1:����
#define	BIT_SYSTEM_DOOR					      ((u32)0x00000080)		// 0:�Ź�:1:�ſ�

#define	BIT_SYSTEM_ARM					      ((u32)0x00000100)		// 0:����;l:���
#define	BIT_SYSTEM_A0					        ((u32)0x00000200)		// 0:����1:�Զ�����
#define	BIT_SYSTEM_A9					        ((u32)0x00000400)		// 0:����1:��������״̬
#define	BIT_SYSTEM_CATCH					    ((u32)0x00000800)		// 0:��λ״̬1:����״̬
#define	BIT_SYSTEM_MUTE					      ((u32)0x00001000)		// 0:�����ر�1:����ģʽ
#define	BIT_SYSTEM_BELT					      ((u32)0x00002000)		// 0:��ȫ��δϵ1:��ȫ��ϵ��
#define	BIT_SYSTEM_OBDOK					    ((u32)0x00004000)		// 0:OBD��Ч1:OBD��Ч
#define	BIT_SYSTEM_LIBAT					    ((u32)0x00008000)		// 0:﮵����Ч1:﮵����Ч

#define	BIT_SYSTEM_TJSTA					    ((u32)0x00010000)		// 0:δ��죬1���
#define	BIT_SYSTEM_TJSOUND_END				((u32)0x00020000)		// 0:���貥����������1��Ҫ��������������
#define	BIT_SYSTEM_POWDOWN					  ((u32)0x00040000)		// 0:��ѹ����1:��ѹ����0.5V
#define	BIT_SYSTEM_MOVE_GPS					  ((u32)0x00080000)		// 0:ֹͣ״̬1:�ƶ�״̬(��GPS�ж�)
#define	BIT_SYSTEM_NULL11					    ((u32)0x00100000)		// ����
#define	BIT_SYSTEM_NULL12					    ((u32)0x00200000)		// ����
#define	BIT_SYSTEM_NULL13					    ((u32)0x00400000)		// ����
#define	BIT_SYSTEM_NULL14					    ((u32)0x00800000)		// ����

#define	BIT_SYSTEM_NULL15					    ((u32)0x01000000)		// ����
#define	BIT_SYSTEM_NULL16					    ((u32)0x02000000)		// ����
#define	BIT_SYSTEM_NULL17					    ((u32)0x04000000)		// ����
#define	BIT_SYSTEM_NULL18					    ((u32)0x08000000)		// ����
#define	BIT_SYSTEM_NULL19					    ((u32)0x10000000)		// ����
#define	BIT_SYSTEM_NULL20					    ((u32)0x20000000)		// ����
#define	BIT_SYSTEM_NULL21				    	((u32)0x40000000)		// ����
#define	BIT_SYSTEM_NULL22					    ((u32)0x80000000)		// ����

//////////////////////////////////////////////////////////////////////
// ϵͳ�ڲ���-ϵͳ����״̬λ����s_Sys.alarmState

#define	BIT_ALARM_SOS					      	((u32)0x00000001)		// 1:������Ƴ�����������غ󴥷�(�յ�Ӧ�������)
#define	BIT_ALARM_OVERSPEED				  	((u32)0x00000002)		// 1:���ٱ���(��־ά���������������)
#define	BIT_ALARM_TIRDDRIVE				  	((u32)0x00000004)		// 1:ƣ�ͼ�ʻ(��־ά���������������)
#define	BIT_ALARM_ILLALARM				  	((u32)0x00000008)		// 1:Ԥ��(�յ�Ӧ�������)
#define	BIT_ALARM_GPSERR					  	((u32)0x00000010)		// 1:GNSSģ�鷢������(��־ά���������������)
#define	BIT_ALARM_ANTBREAK				  	((u32)0x00000020)		// 1:GNSS����δ�ӻ򱻼���(��־ά���������������)
#define	BIT_ALARM_ANTSHORT				  	((u32)0x00000040)		// 1:GNSS���߶�·(��־ά���������������)
#define	BIT_ALARM_POWLOW					  	((u32)0x00000080)		// 1:�ն�����ԴǷѹ(��־ά���������������)

#define	BIT_ALARM_POWREMOVE				  	((u32)0x00000100)		// 1:�ն�����Դ����(��־ά���������������)
#define	BIT_ALARM_DIRVEOVT				  	((u32)0x00000200)		// 1:�����ۼƼ�ʻ��ʱ(��־ά���������������
#define	BIT_ALARM_PARKOVT					  	((u32)0x00000400)		// 1:��ʱͣ��(��־ά���������������)
#define	BIT_ALARM_ILLIG					    	((u32)0x00000800)		// 1:�����Ƿ����(�յ�Ӧ�������)
#define	BIT_ALARM_ILLMOVE					  	((u32)0x00001000)		// 1:�����Ƿ�λ��
#define	BIT_ALARM_OILERR					  	((u32)0x00002000)		// 1:���������쳣(��־ά���������������)
#define	BIT_ALARM_ILLOPEN				    	((u32)0x00004000)		// 1:�����Ƿ�����(ͨ������������)(��־ά���������������)
#define	BIT_ALARM_SIMERR					  	((u32)0x00008000)		// 1:SIM������(�յ�Ӧ�������)

#define	BIT_ALARM_CRASH					    	((u32)0x00010000)		// 1:���Ʒ�����ײ����(�յ�Ӧ�������)
#define	BIT_ALARM_NULL2		  	  	    ((u32)0x00020000)		// ����
#define	BIT_ALARM_NULL3				  			((u32)0x00040000)		// ����
#define	BIT_ALARM_NULL4					  		((u32)0x00080000)		// ����
#define	BIT_ALARM_NULL5					    	((u32)0x00100000)		// ����
#define	BIT_ALARM_NULL6					    	((u32)0x00200000)		// ����
#define	BIT_ALARM_NULL7					  		((u32)0x00400000)		// ����
#define	BIT_ALARM_NULL8					  		((u32)0x00800000)		// ����

#define	BIT_ALARM_NULL9					  		((u32)0x01000000)		// ����
#define	BIT_ALARM_NULL10					  	((u32)0x02000000)		// ����
#define	BIT_ALARM_NULL11				    	((u32)0x04000000)		// ����
#define	BIT_ALARM_NULL12					  	((u32)0x08000000)		// ����
#define	BIT_ALARM_NULL13					  	((u32)0x10000000)		// ����
#define	BIT_ALARM_NULL14					  	((u32)0x20000000)		// ����
#define	BIT_ALARM_NULL15					  	((u32)0x40000000)		// ����
#define	BIT_ALARM_NULL16					  	((u32)0x80000000)		// ����

//////////////////////////////////////////////////////////////////////
// ϵͳ״̬�������Ͷ���
typedef struct	tag_SYSState_Def
{						
		vu32		sysState;																			// ϵͳȫ��״̬
		vu32		alarmState;																		// ϵͳȫ�ֱ���״̬
								
}SYSState_TypeDef;

//////////////////////////////////////////////////////////////////////
// AD�ɼ��������Ͷ���
#define DEF_ADC_MAXNUM								5										// ADת������ƽ��ֵ���õ�Դ���ݸ���
typedef struct	tag_ADInfo_Def
{	
		u16			VBatVol;																			// �ⲿ��Դ��ѹֵ(�㷨���������100��)	
		u16			ADCVBatBK;																		// �ⲿ��Դ12VADC��׼ֵ
		u16			ADCVBat[DEF_ADC_MAXNUM];											// �ⲿ��ԴADԭʼ���ݻ���

}ADCInfo_Typedef;

//////////////////////////////////////////////////////////////////////
// ϵͳ����Ӳ��������Ͷ���(HAL_BSP_SetState()HAL_BSP_GetState()���)
typedef enum {		
		DEF_BSPSTA_ON = 0,								// ON�����״̬(����Y06��Ʒ�޸��ź�)	
		DEF_BSPSTA_IG,										// IG�����״̬(����Y06��ƷΪ�������ź��߼�wakeup�ź���)	
		DEF_BSPSTA_GPSSTA,								// GPS�Ƿ�λ��Ч'A'or'V'
		DEF_BSPSTA_GPS3D,									// GPS�Ƿ�Ϊ3D��λ
		DEF_BSPSTA_GPSSPEED,							// GPS�ٶ�

}ENUM_BSP_STA;


//////////////////////////////////////////////////////////////////////
// ���������ݽṹ
typedef struct	tag_Trigger_Def
{						
		bool		up;
		bool		upup;
										
}Trigger_TypeDef;

//////////////////////////////////////////////////////////////////////
// �¼�״̬�ṹ(���ڴ������Ƶ��ϴ�״̬�뱾��״̬�ĶԱ�)
typedef struct	tag_EventTriType_Def
{						
		Trigger_TypeDef		on;					// ON����״̬��Y06��Ʒ�޸��ź��ߣ�
		Trigger_TypeDef		ig;					// ��𰴼�״̬
		Trigger_TypeDef		igOffMov;		// Ϩ���ƶ�����״̬
		Trigger_TypeDef		lowPow;			// ��ѹ�Ͱ���״̬
		Trigger_TypeDef		gpsFixSta;	// GPS��λ��Ч����״̬
		Trigger_TypeDef		conErrSta;	// CON����ģ�����״̬		
		Trigger_TypeDef		healtySta;	// ����������
		Trigger_TypeDef		dssATHSta;	// DSS������
		Trigger_TypeDef		dssDTHSta;	// DSS������
		Trigger_TypeDef		dssRTHSta;	// DSS��ת��
								
}EventTri_TypeDef;

//////////////////////////////////////////////////////////////////////
// �ͺ�ͳ�ƽṹ

typedef struct	tag_FuelValType_Def
{			
		vu8		step;										// �׶�״̬
		u32		up;											// �ϴ�����
		u32		cun;										// ��ǰ����
		u8		type;										// ����(0-��֧�֣�1-�ٷֱȣ�2-����)
		u32		sum;										// �ۼ�ֵ		
								
}FuelVal_TypeDef;

	
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

HAL_PROCESSSYS_EXT		ADCInfo_Typedef				s_adInfo;			// AD�ɼ�ȫ�ֽṹ����
HAL_PROCESSSYS_EXT		SYSState_TypeDef			s_sys;				// ϵͳȫ��״̬�ṹ����
HAL_PROCESSSYS_EXT		EventTri_TypeDef			s_event;			// ���������¼��ṹ�����
HAL_PROCESSSYS_EXT		vu16									glob_igSta,glob_igStaExt,glob_igStaLast;		// ��ʵϨ��״̬����
HAL_PROCESSSYS_EXT		vu8										glob_accOnRalStep;	//  ACC�жϳ������׶α���
HAL_PROCESSSYS_EXT		vu8										glob_accAdjustFlag;	//  ���ٶȽǶȵ���ȫ�ֱ�־
HAL_PROCESSSYS_EXT		FuelVal_TypeDef				s_fuel;				// �ͺļ���ṹ
			
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
void 					HAL_IG_IRQHandlers							(void);
void					HAL_EventAlarmProcess 			    (u32 	eventSwt);
void			    HAL_BSP_SetState 								(ENUM_BSP_STA type,	u8 newSta);
u32				    HAL_BSP_GetState 								(ENUM_BSP_STA type);
void					HAL_SYS_SetState 								(u32 staBit,	u8 newSta);
u8						HAL_SYS_GetState 								(u32 staBit);
void					HAL_SYSALM_SetState 						(u32 almBit,	u8 newSta);
u8						HAL_SYSALM_GetState 						(u32 almBit);

u8            HAL_GetLastPowerRemoveSt        (void);
void          HAL_SetLastPowerRemoveSt        (u8 ucSt);

u8 HAL_GetIgonAccJudgeIgoffFlag(void);

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
