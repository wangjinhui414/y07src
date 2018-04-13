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
*                                           dev_include_h.h
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

#ifndef  DEV_INCLUDE_PRESENT
#define  DEV_INCLUDE_PRESENT

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   DEV_INCLUDE_MODULE
#define  DEV_INCLUDE_EXT
#else
#define  DEV_INCLUDE_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  "stm32f10x.h"
#include 	"ucos_ii.h"
#include  "hal_h.h"
#include	"bsp_h.h"
#include	"bsp_rtc_h.h"
#include	"bsp_kline_h.h"	 
#include  "bsp_can_h.h"
#include 	"hal_protocol_h.h"

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

// CAN ID����
#define  DEF_CAN_1										(1)							
#define  DEF_CAN_2										(2)	

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
// ϵͳ�����洢������ζ���///////////////////////////////////////////////
//typedef enum {		
//		DEF_TYPE_COMMON = 0,			// ͨ�ò���																	
//		DEF_TYPE_SYSCFG,					// Ӧ�ò���		
//		DEF_TYPE_SYSOBD,					// OBD����		
//}ENUM_PARA_TYPE;

// CAN�����ʶ���//////////////////////////////////////////////////////////
//typedef enum {		
//		BAUD_C_100K = 0,
//		BAUD_C_125K,
//		BAUD_C_250K,
//		BAUD_C_500K,
//		BAUD_C_1000K
//}ENUM_CAN_BAUD;


// RTCʱ�����ڽṹ����////////////////////////////////////////////////////
//typedef	struct tag_Date_Time
//{
//	u16		year;									/* year */	
//	u8		month;								/* month */
//	u8		day;									/* day */
//	u8		hour;			 						/* hour */
//	u8		minute;								/* minute */
//	u8		second;								/* second */

//}SYS_DATETIME;

// OBD�����ӿ����ݽṹ////////////////////////////////////////////////////
//typedef enum
//{
//		CMD_IG_ON					=0,             // �豸����¼�
//		CMD_IG_OFF,                 			// �豸Ϩ���¼�
//		CMD_SYS_WAKEUP,            				// �豸����
//		CMD_AUTO_TJ,											// �Զ����(���豸������ɻ���ʱʹ��)
//		CMD_READ_STA,											// ��ȡ����״̬��Ϣ
//		CMD_PROTOCOL,											// Э�����

//}OBD_CMDCODE_TYPE;

//#define	DEF_PROPARA_SIZE					  	(200)             	// ��������С(ʵʱ�����������ֳ����ֽ����200)
//typedef struct	tag_OBDMbox_Def
//{
//		OBD_CMDCODE_TYPE	cmdCode;														// �������(��ö�ٶ���)
//		vu16	sn;																							// ������ˮ��(��Э����ˮ��)			
//		vu16	proSn;																					// Э����ˮ��
//		vu16	proCmdId;																				// Э������ID
//		vu16	proParaLen;																			// ��������
//		vu8		proParaBuf[DEF_PROPARA_SIZE];										// ������
//		vu8		retCode;
//							
//}OBDMbox_TypeDef;

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

DEV_INCLUDE_EXT		OS_EVENT		*OS_OBDReqMbox;							// OBD��������(ͨѶ->OBD)
DEV_INCLUDE_EXT		OS_EVENT		*OS_OBDAckMbox;							// OBDӦ������(OBD->ͨѶ)

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

// ʱ��Ӧ��API
void					OSTmr_Count_Start				(vu32	*pCounter);
u32						OSTmr_Count_Get					(vu32	*pCounter);
void					RTCTmr_Count_Start			(vu32	*pSecond);
u32						RTCTmr_Count_Get				(vu32	*pSecond);
void 					BSP_RTC_Get_Current			(SYS_DATETIME *pDTime);		// RTCʱ���ȡ
u32 					Mktime 									(u32 year, u32 mon,u32 day, u32 hour,u32 min, u32 sec);		// ��������ʱ����ת�ɴ�1970�굽���ڵ�����
void 					Gettime 								(u32 count,	SYS_DATETIME *cnt);		// ����1970�굽���ڵ�����ת��������ʱ����

// ϵͳ������д�ӿ�API
s8						HAL_LoadParaPro 				(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);
s8						HAL_SaveParaPro 				(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);

// CAN����Ӧ��API
void					BSP_CanWork							(u8	CAN_ID);	// CPU CAN���ܹ���
void					BSP_CanSleep 						(u8	CAN_ID);	// CPU CAN����˯��
void					BSP_CanTransWork				(u8	CAN_ID);	// CAN �շ�������
void					BSP_CanTransSleep 			(u8	CAN_ID);	// CAN �շ���˯��
s8  					BSP_CAN_Device_Init			(u8 CAN_ID,	u8	mode,	ENUM_CAN_BAUD	baudRate);
s8 						BSP_CAN_Setup_Baud			(u8 CAN_ID,	u8	mode,	ENUM_CAN_BAUD	baudRate);
void 					BSP_CAN_Setup_Filter32	(u8	CAN_ID,	u8	filterNum,	u8 filterMode,	u32 id,	 u32 maskId,
												 		 					 u8 assignmen,	FunctionalState enable);
void 					BSP_CAN_Setup_Filter16	(u8	CAN_ID,	u8	filterNum,	u8 filterMode,	u32 id1,	u32 maskId1,	
																			 u32	id2,	u32	maskId2,	u8 assignmen,	FunctionalState enable);																				 
s8						BSP_CAN_SendPack				(u8 CAN_ID,	CanTxMsg *Message);

// K-lineӦ��API
void					BSP_K_SWL_EN						(void);
void					BSP_K_SWL_DIS						(void);
void 					BSP_K_Sleep						  (void);
void 					BSP_K_Work						  (void);
void 					BSP_K_Init						  (u32	baudRate);
void  				BSP_K_SendData				 	(u8 *buf,	u32	len);

// ���ڵ������API
s8						myPrintf 								(const char * format, ...);	// ��������ʵ��ַ�����ӡ�������


/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
