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
*                                               cfg_h.h
*                                              with the
*                                             Y05D Board
*
* Filename      : cfg_h.h
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

#ifndef  CFG_PRESENT
#define  CFG_PRESENT

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   CFG_MODULE
#define  CFG_EXT
#else
#define  CFG_EXT  extern
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
*                                               SYS DEFINES
*********************************************************************************************************
*/
///////////////////////////////////////////////////////////////////////////////////////
// ���̻�������

// ϵͳ�汾��
#define	DEF_SOFTWARE_VER							("05.00")				// ͨѶ���ְ汾��(������汾��)

// ��������
#define	DEF_BT_NAME										("ZXT-Y06")			// ����ģ������(��󳤶�18�ֽ�)
#define	DEF_BT_PASS										("1234")				// �������ӷ�ʽʱ�ṩ������(����4~16�ֽ�)

// ����ID��Ϣ
#define	DEF_COMPANY_ID								((u8)0x01)			// �豸��½/�������ݰ�����ID����ͨΪ:0x01

///////////////////////////////////////////////////////////////////////////////////////
// ϵͳ-���Ź���˯�߿���																 
#define	OS_SYSWDG_EN									(1)							// ���Ϊ1��ʹ�ÿ��Ź�
#define	OS_FEEDWDG_TIME								(2)							// n*s ϵͳι��ʱ����
#define	OS_SYSWDG_TIME								(SYSWDG_TIMEOUT)	// n*s ����IWG���Ź����ʱ��
#define	OS_RTC_ALM_SECS								(10)						// n*s RTC ALM����˯��ʱ��������(��ʱ��Ӧ�ñȿ��Ź����ʱ��С)
#define	OS_RTC_REFRESH								(1)							// n*s RTC ����ж�ˢ��ʱ��
	
#define WOK_SLEEPMODE_EN							(1)							// ���Ϊ1��ʹ�ܽ���˯��ģʽ
#define	OS_INSLEEP_IDLE_TIME					((u32)10*1000)	// n*ms �����������������и�ʱ�����ʽ����˯��״̬
#define	OS_INSLEEP_READCARSTA_TIMEOUT	((u32)30*60*1000)	// n*ms �����ʱ�����ղ���OBD�ĳ���״̬����Ҳǿ�ƽ���˯��
#define	OS_INSLEEP_SENDDATA_TIMEOUT		((u32)5*60*1000)	// n*ms �����ʱ���ڻ��ǲ��ܷ���������ݻ��������ɹ������˯��
#define	OS_RST_CHECKSLEEP_TIME				((u32)5*60*1000) //n*ms ϵͳ��λ�����˯�߼����ӳ�ʱ��
//#define	OS_RST_CHECKSLEEP_TIME				((u32)60*1000) //n*ms ϵͳ��λ�����˯�߼����ӳ�ʱ��
#define	OS_OBDOFF_CHECKINTERVAL_TIME	((u32)30*1000) 	//n*ms ϵͳͨ��OBD��鳵��״̬ʱ����
#define OS_TJSOUND_MAX_TIMEOUT				((u32)30*1000) 	//n*ms �����������������ʱʱ��
#define OS_TJ_MAX_TIMEOUT							((u32)300*1000)	//n*ms ������ʱʱ��

// Э�����
#define	OS_NAVIREQ_EN									(1)							// ���ʹ�ܺ�ȡ���������ݰ����ͼ�ȡ��IPʧЧ����
#define	OS_SENDACK_EN									(1)							// �������ݺ��Ƿ���Ҫ������Ӧ��
#define	OS_HEARTBEAT_EN								(1)							// �Ƿ�������������

//////////////////////////////////////////////////////////////////////////////////////		
// ϵͳӦ�ö���		
#define	OS_GPSREFIX_WAIT_TIMEOUT			((u16)1200)			// n*100ms �豸Ϩ���Զ��ϴ����ݰ��ȴ�GPS(Ĭ��120s)
#define	OS_HEART_ACK_ERR_TIMES				(3) 						// �����������ݰ�����Ӧ��������(Ĭ��5����)

#define	OS_ACK_ERR_TIMES							(10) 						// Ӧ�����ݰ�����Ӧ��������(Ĭ��10����)

#define	OS_LP_SHUTNET_TINES						((u8)5)					// �͹���ģʽ�¶������Դ���
#define	OS_LP_GSMSLEEP_TINES					((u8)5)					// �͹���ģʽ��GSMģ��˯�߳��Դ���
#define	OS_PROCOMSMS_STAMPTIMEOUT			((u32)600)				// Э����ƶ���ʱ�����ʱʱ�䵥λ��(Ĭ�ϣ�10����)
#define	OS_PROCONSMS_RECTIMEOUT				((u32)600)			// Э����ƶ��Ž��ճ�ʱʱ�䵥λ��(Ĭ�ϣ�10����)

#define	OS_IGON_CHECK_TIMES						((u8)2)					// IG ON ״̬ȷ�ϴ���
#define	OS_IGOFF_CHECK_TIMES					((u8)50)				// IG OFF ״̬ȷ�ϴ���
#define	OS_IG_CHECK_TIME							((u32)100)			// IG ״̬ȷ��ʱ����

#define	OS_LOWPOWIN_CHECK_TIMES				((u8)20)				// LOW POW IN ״̬ȷ�ϴ���
#define	OS_LOWPOWOFF_CHECK_TIMES			((u8)20)				// LOW POW OUT ״̬ȷ�ϴ���
#define	OS_LOWPOW_CHECK_TIME					((u32)1000)			// LOW POW ״̬ȷ��ʱ����

#define	OS_GSM_MAXERR_TIME						((u16)60)				// GSM�ײ����������
#define	OS_SIM_MAXERR_TIME						((u16)60)			  // SIM��������������10���Ӻ�λ
#define	OS_GPS_ERRCHECK_TIME					((u32)8000)			// GPSӦ�ò����ȷ��ʱ��

#define	OS_LOGUP_INTERVAL_TIME				((u32)10*60*1000)	// �豸����LOG�ϴ�ʱ����Ĭ��10����
#define	OS_APPIP_VALID_TIME						((u32)86000)		// n*ms ����RTC��ʱ(Ĭ��23Сʱ54�ֵײ�RTC���񳬹�24Сʱ�Զ���0��ʼ)
#define	OS_APPIP_CNNERROR_TIMES				((u8)3)					// Ӧ��IP��������ʧ��������(�����ô���Ӧ��IP����ʧЧ����)						


///////////////////////////////////////////////////////////////////////////////////////
// ������Ϣ-������Ϣ�������
#define	DEF_INITINFO_OUTPUTEN					(1)							// ��ʼ����Ϣ���ʹ��
#define	DEF_MEMINFO_OUTPUTEN			  	(1)							// �洢��ȫ�ֵ�����Ϣ���ʹ��
#define	DEF_ACCINFO_OUTPUTEN					(1)							// ACC��Ϣ���ʹ��
#define	DEF_GPSINFO_OUTPUTEN					(1)							// GPS��Ϣ���ʹ��
#define	DEF_GSMINFO_OUTPUTEN					(1)							// GSM��Ϣ���ʹ��
#define	DEF_EXTIINFO_OUTPUTEN					(0)							// �ж���Ϣ���ʹ��
#define	DEF_EVENTINFO_OUTPUTEN				(1)							// ʹ���¼���Ϣ���
#define	DEF_ACCINFO_OUTPUTEN					(1)							// ʹ�ܺ����ACC��Ϣ
#define	DEF_OSINFO_OUTPUTEN						(1)							// ϵͳOS��Ϣ���
#define	DEF_OBDINFO_OUTPUTEN					(1)							// ʹ�����OBD������Ϣ
#define	DEF_GPRSINFO_OUTPUTEN					(1)							// GPRS��Ϣ���ʹ
#define	DEF_BTINFO_OUTPUTEN						(1)							// BT��Ϣ���ʹ��
#define	DEF_SMSINFO_OUTPUTEN					(1)							// SMS��Ϣ���ʹ��
#define	DEF_ADCINFO_OUTPUTEN					(1)							// AD�ɼ���Ϣ���
#define	DEF_IGCHECKLOGIC_OUTPUTEN			(1)							// IG���Ϩ����Ӧ���ٶ�״̬���ʹ��
						
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

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
