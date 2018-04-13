/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2007-2008; Micrium, Inc.; Weston, FL
*																									ed by international copyright laws.
*                   Knowledge of the source code
*                   All rights reserved.  Protect may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        			MAIN CODE
*
*                                               main_h.h
*                                              with the
*                                             Y05D Board
*
* Filename      : main_h.h
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

#ifndef  MAIN_PRESENT
#define  MAIN_PRESENT

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   MAIN_MODULE
#define  MAIN_EXT
#else
#define  MAIN_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "stm32f10x.h"


/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define  APP_TASK_START_PRIO							(6)	//
#define	 APP_TASK_GSM_PRIO								(7)	//
#define	 APP_TASK_GPRS_PRIO								(8) //
#define	 APP_TASK_RXD_PRIO								(9)
#define	 APP_TASK_SEND_PRIO								(10)
#define	 APP_TASK_PROTOCOL_PRIO						(11)
#define	 APP_TASK_BASE_PRIO								(12)
//#define	 APP_TASK_OBD1_PRIO								(13)	// TASK_PRIO 13~18 �����OBD����
//#define	 APP_TASK_OBD2_PRIO								(14)
//#define	 APP_TASK_OBD3_PRIO								(15)
//#define	 APP_TASK_OBD4_PRIO								(16)
//#define	 APP_TASK_OBD5_PRIO								(17)
//#define	 APP_TASK_OBD6_PRIO								(18)
#define	 APP_TASK_BT_PRIO									(19)
#define	 APP_TASK_GPS_PRIO								(20)
#define	 APP_TASK_MUTUAL_PRIO							(21)
#define	 APP_TASK_DBG_PRIO								(23)
#define	 APP_TASK_CON_PRIO								(22)


/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE					(100)
#define  APP_TASK_GSM_STK_SIZE						(100)	
#define  APP_TASK_GPRS_STK_SIZE						(200)	
#define	 APP_TASK_RXD_STK_SIZE						(1200)
#define	 APP_TASK_SEND_STK_SIZE						(1200)
#define  APP_TASK_PROTOCOL_STK_SIZE				(1300)
#define  APP_TASK_BASE_STK_SIZE						(1300)
#define	 APP_TASK_BT_STK_SIZE							(80)	
#define	 APP_TASK_GPS_STK_SIZE						(600)
#define	 APP_TASK_MUTUAL_STK_SIZE					(600)
#define	 APP_TASK_DBG_STK_SIZE						(600)
#define	 APP_TASK_CON_STK_SIZE						(80)
/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               OTHER DEFINES
*********************************************************************************************************
*/

						
/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

// ������Ź�������ʱ�ж�

#define	TASK_GSM_TIMOUT								((u32)30*1000)
#define	TASK_GPRS_TIMOUT							((u32)6*60*1000)
#define	TASK_RXD_TIMOUT								((u32)3*60*1000)
#define	TASK_SEND_TIMOUT							((u32)5*60*1000)
#define	TASK_PROTOCOL_TIMOUT					((u32)5*60*1000)
#define	TASK_BASE_TIMOUT							((u32)40*60*1000)			// ���������ڸ��������ɸ�ֵ����������ʱʱ��
#define	TASK_BT_TIMEOUT								((u32)2*60*1000)
#define	TASK_GPS_TIMOUT								((u32)3*60*1000)
#define	TASK_MUTUAL_TIMOUT						((u32)30*1000)
#define	TASK_DBG_TIMOUT								((u32)3*60*1000)
#define	TASK_CON_TIMOUT								((u32)60*1000)
#define	TASK_OBD1_TIMEOUT							((u32)3*60*1000)
#define	TASK_OBD2_TIMEOUT							((u32)3*60*1000)
#define	TASK_OBD3_TIMEOUT							((u32)3*60*1000)


// ������Ź����ݽṹ
typedef struct	tag_SoftIWG_Def
{
		vu32		gsmCounter;	
		vu32		gprsCounter;
		vu32		rxdCounter;
		vu32		sendCounter;
		vu32		protocolCounter;
		vu32		baseCounter;
		vu32		btCounter;				
		vu32		gpsCounter;		
		vu32		mutualCounter;	
		vu32		dbgCounter;
		vu32		conCounter;
		vu32		obd1Counter;
		vu32		obd2Counter;
		vu32		obd3Counter;	
							
}SoftIWG_TypeDef;

// ϵͳ������Ϣ���ݽṹ
typedef struct	tag_SYSError_Def
{			
		u8			removeTimes;																	// 24Сʱ�Ƴ�����	
		u8			gpsResetTimes;																// GPS 24Сʱ��λ����	
		u8			gsmResetTimes;																// GSM 24Сʱ��λ����		
		u8			gsmSimTimes;																	// GSM SIM��24Сʱ�Ƴ�����
		u8			accTimes;																			// ���ٶ�оƬ��д24Сʱ���ϴ���
		u8			obdBusRWTimes;																// OBD���߶�д���ϴ���
		u8			obdParaRWTimes;																// OBD������д���ϴ���(��ָ���ͳ�ϵ����)
		u8			flashTimes;																		// ����Flash��д����(ָ���л��������ݶ�д)
		u8			cpuFlashTimes;																// CPU Flash��д����(��Ҫָϵͳ������д)
		u8			conTimes;																			// �ⲿ����ģ��ͨ�Ź��ϴ���
		u8			spiFlashSumTimes;															// SPI FLASH�ܴ�������ۼ�					
																																																						
}SYSError_Typedef;


// ��λ���ȼ�����//////////////////////////////////
#define	DEF_RESET_PRI0								((vu8)0xAA)		//���������������ȴ�10���ֱ�Ӹ�λ
#define	DEF_RESET_PRI1								((vu8)0xBB)		//���������������ȴ�180���ֱ�Ӹ�λ	
#define	DEF_RESET_WAIT								((vu8)0xCC)		//�������ݷ�����ɺ������ȴ�״̬��־



// ϵͳ��ʱӦ�ñ���
typedef struct	tag_APPTmp_Def
{			
		vu32		report24Counter;															// 24Сʱ�Ƴ�����	
		vu32		igOnMsgCounter;																// Ϩ���ϴ����ݼ����ʱ��
		vu32		igOffMsgCounter;															// ����ϴ����ݼ����ʱ��
		vu32		igOnChiXuCounter;															// �豸����һ�ε��ʼ����ʱ�䶨ʱ��
		vu8			cnnErrTimes;																	// GSM���������������
		vu8			ackErrTimes;																	// GSM����Ӧ�����ݴ���
		vu16		simErrTimes;																	// SIM��Ӧ�����쳣����
		vu8			gsmConSleep;																	// GSM˯�߿���
		vu32		obdSn;																				// OBD������ˮ��
		vu16		sysSn;																				// ϵͳ�����ϴ���ˮ��
		vu8			resetReq;																			// ϵͳ��λ�����־
		vu16		maxSpeed;																			// ���ʱ��(���ڵ��Ϩ�����ϴ�)
		vu32		sendDataToCounter;														// �������ݳ�ʱ��ʱ��
		vu8			obdReadCarStaFlag;														// ��ȡ����״̬�����б�־
		vu32		obdReadCarStaCounter;													// ��ȡ����״̬��ʱ��
		vu8			rxdDataLock;																	// ������ģʽ���豸����������
		vu32		igStartTicks;																	// ���ʼϵͳ�δ�洢(���ڵ��Ϩ��ʱ��ƫ��������)	
		vu8			naviStep;																			// IP�����׶α���
		vu32		appIPWorkCounter;															// Ӧ��IP��������ʱ��
		vu8			appIPCnnErrTimes;															// Ӧ��IP�����������
		vu32		igEnDelayCounter;															// IG��ʱ��ⶨʱ��
		vu32		upWaitCounter;																// �����ȴ���ʱ��
		vu8			upFlag;																				// �����ȴ���־
		vu8			sleepWaitFlag;																// ȫ��˯�ߵȴ���־
		vu32		globSleepDelayCounter;												// ȫ��˯�ߵȴ�ʱ��
		vu8			tjSendFlag;																		// ��챨�ķ���־
																																																						
}APPTmp_Typedef;

// GSMӦ�ýṹ����
typedef struct	tag_GSMAppVar_Def
{		
		u8			simNumWriteFlag;	// SIM����д���־
		u8			pSim[30];					// SIM���������
		u8			simNum[20];				// SIM������
		u8			IMEI[30];					// IMEI����
		u8			MCC[4];						// �ƶ����Ҵ���3char
		u8			MNC[4];						// �ƶ��������2��3char
		u8			LAC[5];						// С������4hexchar
		u8			CELL[5];					// ��վID4hexchar
		u8			sLeve;						// GSM��ǰ�ź�ǿ��
		u8			rssi;							// GSM��ǰ������
								
}GSMAppVar_TypeDef;

///////////////////////////////////////////////////
// ���ڷ��ͽṹ�ĸ��������ݽṹ����

#define	DEF_ACKTYPE_ACK								((u8)0)				// ���ݰ�Ӧ������-��ҪӦ��
#define	DEF_ACKTYPE_NOACK							((u8)1)				// ���ݰ�Ӧ������-����Ҫ

// ��¼��Ϣ���ͽṹ
#define	DEF_LOGINSENDBUF_SIZE					(100)					// ���ڵ�½����ʱ��������С
typedef struct	tag_TmpLoginSend_Def
{	
		vu8			handShake;															// �豸��½��½/����״̬0-δ��½1-��½/�������
		vu16		sn;		
		u16			len;
		u8			buf[DEF_LOGINSENDBUF_SIZE];		
	
}TmpLoginSend_TypeDef;

// ���ڷ���OBD���ݵķ��ͽṹ
#define	DEF_OBDSENDBUF_SIZE						(600)					// ���ڷ��͵���ʱ��������С(Ӧ���������Ϣ��С)
typedef struct	tag_TmpObdSend_Def
{
		vu8			obdSendFlag;														// OBD���ݷ��ͱ�־1Ϊ����
		vu16		sn;
		u8			ackType;																// ����Ӧ�����ͼ�����(DEF_ACKTYPE_ACK or DEF_ACKTYPE_NOACK)		
		u16			len;
		u8			buf[DEF_OBDSENDBUF_SIZE];
	
}TmpObdSend_TypeDef;

// ���ڷ�������RAM���з��ͽṹ
#define	DEF_RAMSENDBUF_SIZE						(200)					// ���ڷ��͵���ʱ��������С(Ӧ���������Ϣ��С)
typedef struct	tag_TmpRamSend_Def
{		
		vu16		sn;
		u8			ackType;																// ����Ӧ�����ͼ�����(DEF_ACKTYPE_ACK or DEF_ACKTYPE_NOACK)		
		u8			tryTimes;																// ���Է��ʹ���
		u8			type;																		// �������������
		u16			len;
		u8			buf[DEF_RAMSENDBUF_SIZE];
	
}TmpRamSend_TypeDef;

// ���ڷ�������FLASH������ݵķ��ͽṹ
#define	DEF_FLASHSENDBUF_SIZE					(600)					// ���ڷ��͵���ʱ��������С(Ӧ���������Ϣ��С)
typedef struct	tag_TmpFlashSend_Def
{		
		vu16		sn;
		u8			ackType;																// ����Ӧ�����ͼ�����(DEF_ACKTYPE_ACK or DEF_ACKTYPE_NOACK)	
		u16			queOutNum;															// ���ݳ��Ӹ��� 
		u16			queLastOut;															// ���֮ǰ��OUTָ��ֵ
		u8			tryTimes;																// ���ݷ��ͳ��Դ���
		u16			len;
		u8			buf[DEF_FLASHSENDBUF_SIZE];
	
}TmpFlashSend_TypeDef;

// ���ڷ���ͨ��Ӧ��������������˯�����ݰ��ķ��ͽṹ
#define	DEF_ACKSENDBUF_SIZE						(30)					// ���ڷ��͵���ʱ��������С
typedef struct	tag_TmpAckSend_Def
{		
		vu16		sn;		
		u16			len;
		u8			buf[DEF_ACKSENDBUF_SIZE];
	
}TmpAckSend_TypeDef;

// ͨ�÷��ͽṹ����������Ͷ���
typedef enum
{
		PACK_TYPE_HEART					=0xAA,              		// ������
		PACK_TYPE_SLEEP					=0xBB,		              // ˯�����ݰ�
		PACK_TYPE_SHUTNET				=0xCC,            			// �������ݰ�
	
}PACK_SEND_TYPE;
#define	DEF_COMSENDBUF_SIZE						(30)					// ���ڷ��͵���ʱ��������С
typedef struct	tag_TmpComSend_Def
{		
		PACK_SEND_TYPE	packType;												// ����������;�������ݰ���������ö�ٶ���
		vu16		sn;		
		u16			len;
		u8			buf[DEF_COMSENDBUF_SIZE];
	
}TmpComSend_TypeDef;

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

MAIN_EXT		SYSError_Typedef	s_err;			// ϵͳ�������ݽṹ
MAIN_EXT		SoftIWG_TypeDef		s_swg;			// ������Ź��ṹ����
MAIN_EXT		APPTmp_Typedef		s_osTmp;		// ϵͳӦ����ʱ����(�������Ҫ�����ȫ�ֱ���)
MAIN_EXT		GSMAppVar_TypeDef	s_gsmApp;		// GSMӦ�����ݽṹ

MAIN_EXT		TmpLoginSend_TypeDef	s_login;	// ��¼��Ϣ������ʱ�ṹ����
MAIN_EXT		TmpObdSend_TypeDef		s_obd;		// OBDʵʱ��Ϣ������ʱ�ṹ����
MAIN_EXT		TmpRamSend_TypeDef		s_ram;		// RAM������Ϣ������ʱ�ṹ����
MAIN_EXT		TmpFlashSend_TypeDef	s_flash;	// FLASH������Ϣ������ʱ�ṹ����	
MAIN_EXT		TmpAckSend_TypeDef		s_comAck;	// ͨ��Ӧ����Ϣ������ʱ�ṹ����
MAIN_EXT		TmpComSend_TypeDef		s_comPack;	// ����������Ͽ���˯����Ϣ������ʱ�ṹ����


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

void					APP_ErrClr 						(vu8 *Val);
void					APP_ErrAdd 						(vu8 *Val);
u8						APP_ErrQue 						(vu8 *Val);
void					OSTmr_Count_Start			(vu32	*pCounter);
u32						OSTmr_Count_Get				(vu32	*pCounter);
void					sendPrintProcess 			(u8 type,	u8 *pHexBuf,	u16 len);
void					OS_SuspendAllTaskProcess 	(void);
void					OS_ResumeAllTaskProcess 	(void);

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/
void JumpToApp1(void);
#endif                                                          /* End of module include.                               */
