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
*                                        	APPLICATION CODE
*
*                                     				hal_h.h
*                                              with the
*                                   				Y05D Board
*
* Filename      : hal_h.h
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

#ifndef  HAL_PRESENT
#define  HAL_PRESENT

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   HAL_MODULE
#define  HAL_EXT
#else
#define  HAL_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "stm32f10x.h"

/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

// ���˯��ģʽ����																 
#define	OS_DEEPSLEEP_MODE1						(1)						// �ر����������Դ���жϣ���PVD,RTC,�ⲿ��Դ����ɻ���
#define	OS_DEEPSLEEP_MODE2						(2)						// �ر����������Դ���жϣ���PVD,RTC,�ɻ���

// PVD�����־
#define	OS_INITPVD_FLAG								((u16)0xA56A)	// OS����ǰPVD����ʱ����

// IAP �����ⲿFLASH�洢λ�ö���
#define	DEF_IAP_BACKUP_POSI1					(u16)(0xA5A5)	// ��ǰ����洢��BACK_UP1λ��
#define DEF_IAP_BACKUP_POSI2					(u16)(0xB5B5)	// ��ǰ����洢��BACK_UP2λ��

// IAP ����״̬�ڲ�CPU FLASH��־���嶨��//////////

#define	DEF_IAP_LOCATIONKEY_FLAG			((u16)0xA1A5)	// ��������״̬(����iapFlag��ֵ)		
#define	DEF_IAP_LOCATIONCMD_FLAG			((u16)0xA2A5)	// ���ش�����������״̬(����iapFlag��ֵ)		
#define	DEF_IAP_FTP_FLAG							((u16)0xA3A5)	// Զ��FTP����״̬(����iapFlag��ֵ)
#define	DEF_IAP_NETPRO_FLAG						((u16)0xA4A5)	// Զ���û��Զ���Э������״̬(����iapFlag��ֵ)
#define	DEF_IAP_LOCATIONBKP_FLAG			((u16)0xA5A5)		// ���ش�����������״̬,��BKP�ؼ��ִ���(����iapFlag��ֵ)	
#define	DEF_IAP_PRO1MID_FLAG					((u16)0xC1C5)	// �Ѿ��������������ص�����FLASH1����λ��(����iapFlag��ֵ)
#define	DEF_IAP_PRO2MID_FLAG					((u16)0xC2C5)	// �Ѿ��������������ص�����FLASH1����λ��(����iapFlag��ֵ)
#define	DEF_IAP_CLEAN_FLAG						((u16)0x0000)	// ���������־		

// ��������״̬�ؼ���
#define	DEF_UPDATE_WAIT								((u16)0xA4A6)	// ���������ȴ�״̬(����iapSta��ֵ)
#define	DEF_UPDATE_KEYWORD						((u16)0xA5A7)	// ��������״̬�йؼ���(����iapSta��ֵ)

// CPU�ڲ�FLASH��ַ���ֶ���////////////////////////
#define	FLASH_CPU_ALL_SIZE						(0x40000)			// CPU FLASH������С��STM32F105R8T6 256K��
#define	FLASH_CPU_IAP_START						(0x08000000)	// IAP����ʼ��ַ
#define	FLASH_CPU_IAP_SIZE						(0x7800)			// IAP�����С30K
#define	FLASH_CPU_COMMON_START				(0x08007800)	// ϵͳͨ�ò����洢��ʼ��ַ
#define	FLASH_CPU_COMMON_SIZE					(0x800)				// ϵͳͨ�ò�����С2K
#define	FLASH_CPU_SYSCFG_START				(0x08008000)	// ϵͳ���ò����洢��ʼ��ַ
#define	FLASH_CPU_SYSCFG_SIZE					(0x800)				// ϵͳ���ò�����С2K
#define	FLASH_CPU_SYSOBD_START				(0x08008800)	// ϵͳOBD�����洢��ʼ��ַ
#define	FLASH_CPU_SYSOBD_SIZE					(0x800)				// ϵͳOBD������С2K
#define	FLASH_CPU_APPNAME_START				(0x08009000)	// Ӧ�ó������ִ洢��ʼ��ַ
#define	FLASH_CPU_APPNAME_SIZE				(0x800)				// Ӧ�ó������ִ�С2K
#define	FLASH_CPU_APPSIZE_START				(0x08009800)	// Ӧ�ó����С�洢��ʼ��ַ
#define	FLASH_CPU_APPSIZE_SIZE				(0x800)				// Ӧ�ó����С��С2K
#define	FLASH_CPU_APP_START						(0x0800A000)	// Ӧ�ó���洢��ʼ��ַ
#define	FLASH_CPU_APP_SIZE						(0x36000)			// Ӧ�ó����С256K-30K-10K=216K
#define	DEF_MAXOF_VERB_LEN						(20)					// ����汾����󳤶�
 
/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ϵͳ���Ĭ�ϲ�������
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

//////////////////////////////////////////////////////////////////////
// ϵͳͨ�ò�����-Ĭ�ϲ�������
#define	DEF_DFT_COMMON_SAVED_MARK			((u16)0x66A5)				// ϵͳͨ�ò�������ʼ����ɱ�־(Ĭ��0x66A5,2015-5-29�Ժ󽫲��ı��ֵ��ֹ������λ)
#define	DEF_DFT_COMMON_SUM						((u16)0x0000)				// ��Ҫ���ݾ����������!!!!
#define	DEF_DFT_COMMON_IAPFLAG				((u16)0x0000)				// IAP������־
#define	DEF_DFT_COMMON_IAPSTA					((u16)0x0000)				// IAP����״̬
#define	DEF_DFT_COMMON_ID							((u8)0x00)					// ϵͳIDĬ��ֵ
#define	DEF_DFT_COMMON_CARINFO				((u8)0x00)					// ������Ϣ(��ϵ+���ʹ���+���ͻ�Ʒ��)		
#define	DEF_DFT_COMMON_AADC						((u16)2671)					// ϵͳĬ��AD��׼ (����ADCĬ��ֵ)��Ҫ���ݾ��������д!!!!
#define	DEF_DFT_COMMON_SWDG						((u8)0)							// ������Ź�Ĭ��ֵ
#define	DEF_DFT_COMMON_HVERB					("00.00")						// ϵͳӲ���汾��Ĭ��ֵ
#define	DEF_DFT_COMMON_GPSDIS					((double)0)					// GPS�ۼӹ�����Ĭ��ֵ
#define	DEF_DFT_COMMON_OTAPAPN				("CMNET")						// Զ������APN
#define	DEF_DFT_COMMON_OTAPAPNUSER		("")								// Զ������APN�û���
#define	DEF_DFT_COMMON_OTAPAPNPASS		("")								// Զ������APN���� 
#define	DEF_DFT_COMMON_OTAPIP					("")								// Զ������IP	
#define	DEF_DFT_COMMON_OTAPPORT				("")								// Զ������PORT
#define	DEF_DFT_COMMON_OTAPFTPUSER		("")								// FTP�������û���	
#define	DEF_DFT_COMMON_OTAPFTPPASS		("")								// FTP����������
#define	DEF_DFT_COMMON_OTAPFTPFILE		("")								// FTP����������·�����ļ���
#define	DEF_DFT_COMMON_OTAPFILESIZE		((u32)0)						// Զ�����������С
#define	DEF_DFT_COMMON_OTAPERRCODE		((u8)0)							// Զ������������


//////////////////////////////////////////////////////////////////////
// ϵͳӦ�ò�����-Ĭ�ϲ�������
#define	DEF_DFT_SYSCFG_SAVED_MARK			((u16)0x0034)				// ϵͳӦ�ò�������ʼ����ɱ�־(Ĭ��0x0034,2015-5-29�Ժ󽫲��ı��ֵ��ֹ������λ)
#define	DEF_DFT_SYSCFG_SUM						((u16)0x0000)				// ��Ҫ���ݾ����������!!!!
#define	DEF_DFT_SYSCFG_APN						("CMNET")						// ϵͳAPN	
#define	DEF_DFT_SYSCFG_APNUSER				("")								// ϵͳAPN��½�û�	
#define	DEF_DFT_SYSCFG_APNPASS				("")								// ϵͳAPN��½����	
#define	DEF_DFT_SYSCFG_DNSIP					("202.106.0.20")		// ϵͳ����������IP��ַ
#define	DEF_DFT_SYSCFG_DOMAIN1				("zxs3.wiselink.net.cn")						// ϵͳ����1
#define	DEF_DFT_SYSCFG_DOMAIN2				("zxs3.wiselink.net.cn")						// ϵͳ����2
#define	DEF_DFT_SYSCFG_IP1						("221.123.179.91")	// ϵͳ��������IP��ַ(����IP)
#define	DEF_DFT_SYSCFG_PORT1					("7005")						// ϵͳ���������˿ں�
#define	DEF_DFT_SYSCFG_IP2						("")								// ϵͳ����������IP��ַ(Ӧ��IP)
#define	DEF_DFT_SYSCFG_PORT2					("")								// ϵͳ�����������˿ں�
#define	DEF_DFT_SYSCFG_DOMAINEN				((u8)0)							// ��������ʹ�����Ϊ1��ʹ����������Ϊ0���ֹ�������Ӽ�IP����
#define	DEF_DFT_SYSCFG_GPSCOLLECTTIME	((u8)30)						// GPS�ɼ�ʱ��������λΪ��(s)
#define	DEF_DFT_SYSCFG_ONREPORTTIME		((u32)30)						// �����Ϣ�㱨�������λΪ��(s)	
#define	DEF_DFT_SYSCFG_OFFREPORTTIME	((u32)0)						// Ϩ����Ϣ�㱨�������λΪ��(s)	
#define	DEF_DFT_SYSCFG_HEARTBEATTTIME	((u32)32)						// �ն��������ͼ������λΪ��(s)	
#define	DEF_DFT_SYSCFG_TCPACKTIMEOUT	((u32)30)						// TCP��ϢӦ��ʱʱ�䣬��λΪ��(s)
#define	DEF_DFT_SYSCFG_HEALTHREPORTTIME	((u32)10800)			// �������ϱ�ʱ��������λΪ��(s)��Ĭ��4Сʱ 2015-6-30 ��Ϊ3Сʱ
#define	DEF_DFT_SYSCFG_SMSCENTER			("")								// ���ƽ̨SMSè�绰����	
#define	DEF_DFT_SYSCFG_EVENTSWT				((u32)0xffffffff)		// �¼��ϱ������֣���ӦλΪ1���ϱ���Ӧ�¼�0���ϱ�
#define	DEF_DFT_SYSCFG_OVERSPEED			((u16)120)					// ���ٱ������ޣ���λΪ����ÿСʱ(km/h)
#define	DEF_DFT_SYSCFG_OVERSPEEDTIME	((u16)10)						// ���ٳ���ʱ�䣬��λΪ��(s)
#define	DEF_DFT_SYSCFG_TIREDDIRVETIME	((u32)10800)				// ƣ�ͼ�ʻʱ�����ޣ���λΪ��(s)Ĭ��3Сʱ		
#define	DEF_DFT_SYSCFG_PHO1						("")								// ���������绰����1
#define	DEF_DFT_SYSCFG_PHO2						("")								// ���������绰����2
#define	DEF_DFT_SYSCFG_PHO3						("")								// ���������绰����3
#define	DEF_DFT_SYSCFG_PHO4						("")								// ���������绰����4
#define	DEF_DFT_SYSCFG_PHO5						("")								// ���������绰����5
#define	DEF_DFT_SYSCFG_PHO6						("")								// ���������绰����6
#define	DEF_DFT_SYSCFG_AESKEY					((u8)0x00)					// ��Կ
#define	DEF_DFT_SYSCFG_AESTYPE				((u8)0x00)					// ��������	
#define	DEF_DFT_SYSCFG_DEVEN					((u8)0x0f)					// �豸ʹ�ܿ���(Ĭ�Ϲر��񶯱�����ʾ��������ƹ���)
#define	DEF_DFT_SYSCFG_WMODE					((u8)0)							// �豸������ʽ
#define	DEF_DFT_SYSCFG_NATIONCODE			("86")							// ���Ҵ���(�й�Ĭ��"86")ʹ��PDU��ʽ���Ͷ���ʱʹ��		
#define	DEF_DFT_SYSCFG_SLEEPSWT				((u8)0xff)					// ˯�߻��ѿ���
#define	DEF_DFT_SYSCFG_DSSATH					((u16)350)					// DSS����������	
#define	DEF_DFT_SYSCFG_DSSDTH					((u16)550)					// DSS����������
#define	DEF_DFT_SYSCFG_DSSRTH					((u16)0)						// DSS��ת������	
#define	DEF_DFT_SYSCFG_DSSPTH					((u16)1100)					// DSS��ײ����	
#define	DEF_DFT_SYSCFG_ACCMT					((u16)8)						// ���ٶ�оƬ�ƶ�INT1�������ޣ���λΪ��G(mg)	
#define	DEF_DFT_SYSCFG_ACCMD					((u16)1)						// ���ٶ�оƬ�ƶ�INT1����ʱ������λ����(ms)
#define	DEF_DFT_SYSCFG_SYSMOVE				((u8)15)						// ϵͳ�ƶ��ٶ�����,��λ:����/Сʱ(ʹ��GPS�ٶ��ж��ƶ�ʱ����)
#define	DEF_DFT_SYSCFG_SYSMOVET				((u8)60)						// ϵͳ�ƶ�ʱ������,��λ:��(ʹ��GPS�ٶ��ж��ƶ�ʱ����)
#define	DEF_DFT_SYSCFG_LOWPOWER				((u16)1150)					// ��ѹ��������
#define	DEF_DFT_SYSCFG_TJDELAYTIME		((u8)5)							// �����ʱʱ��Ĭ��5,��λ:��
#define	DEF_DFT_SYSCFG_IGOFFDELAY			((u16)60)						// Ϩ������ʱʱ��Ĭ��1min,��λ:��
#define	DEF_DFT_SYSCFG_NAVISTEP				((u8)0)							// IP�����׶α���
#define	DEF_DFT_SYSCFG_GLOBSLEEPTIME	((u32)1800)				// �豸ȫ��˯�߽���ʱ��Ĭ��30����,��λ:��2015-8-3 �޸�Ϊ23:54����,2015-9-28 �޸�Ϊ30����
#define	DEF_DFT_SYSCFG_SOOUND1				("������ϵͳ��ʼ�����İ���������죬���Ժ�")											// ���ǰ������Ϣ
#define	DEF_DFT_SYSCFG_SOOUND2				("�������ֻ��ͻ��˵����֪ͨ")		// �����ɲ�����Ϣ

//////////////////////////////////////////////////////////////////////
// ϵͳOBD������-Ĭ�ϲ�������(��OBD���Ŷ���)
#define	DEF_DFT_SYSOBD_SAVED_MARK			((u16)0x0002)				// ϵͳOBD��������ʼ����ɱ�־
//#define	DEF_DFT_SYSOBD_SUM						((u16)0x0000)				// ��Ҫ���ݾ����������!!!!
//#define	DEF_DFT_SYSOBD_ODO						((double)0)					// OBD������
//#define	DEF_DFT_SYSOBD_FUEL						((double)0)					// OBD�ۼ��ͺ�

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ϵͳ��ز�����С����
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

//////////////////////////////////////////////////////////////////////
// ϵͳͨ�ò�����-������С����
#define PRA_SIZEOF_COMMON_SAVED_MARK  (sizeof(s_common.savedMark)) 	    // 
#define PRA_SIZEOF_COMMON_SUM  				(sizeof(s_common.sum)) 	        	// 
#define PRA_SIZEOF_COMMON_IAPFLAG  		(sizeof(s_common.iapFalg)) 	      // 	
#define	PRA_SIZEOF_COMMON_IAPSTA			(sizeof(s_common.iapSta))					//
#define PRA_SIZEOF_COMMON_ID  				(6) 	        										// 
#define PRA_SIZEOF_COMMON_CARINFO  		(3) 	        										// 
#define PRA_SIZEOF_COMMON_AADC  			(sizeof(s_common.aadc)) 	       	// 
#define PRA_SIZEOF_COMMON_SWDG  			(sizeof(s_common.swdg))           // 
#define PRA_SIZEOF_COMMON_HVERB			  (5)      													// 		
#define PRA_SIZEOF_COMMON_GPSDIS  		(sizeof(s_common.aadc)) 	       	// 
#define PRA_SIZEOF_COMMON_OTAPAPN  		(20) 	        										// 
#define PRA_SIZEOF_COMMON_OTAPAPNUSER (20) 	        										// 
#define PRA_SIZEOF_COMMON_OTAPAPNPASS (20) 	        										// 
#define PRA_SIZEOF_COMMON_OTAPIP  		(50) 	        										// 
#define PRA_SIZEOF_COMMON_OTAPPORT  	(5) 	        										// 
#define	PRA_SIZEOF_COMMON_OTAPFTPUSER	(20)															//
#define	PRA_SIZEOF_COMMON_OTAPFTPPASS	(20)															// 
#define	PRA_SIZEOF_COMMON_OTAPFTPFILE	(50)															//
#define PRA_SIZEOF_COMMON_OTAPFILESIZE (sizeof(s_common.otapFileSize)) 	// 
#define PRA_SIZEOF_COMMON_OTAPERRCODE (sizeof(s_common.errCode)) 	      // 

//////////////////////////////////////////////////////////////////////
// ϵͳӦ�ò�����-������С����
#define PRA_SIZEOF_SYSCFG_SAVED_MARK  (sizeof(s_cfg.savedMark)) 	    	// 
#define PRA_SIZEOF_SYSCFG_SUM  				(sizeof(s_cfg.sum)) 	        		// 
#define PRA_SIZEOF_SYSCFG_APN  				(20) 	        										// 
#define PRA_SIZEOF_SYSCFG_APNUSER  		(20) 	        										// 
#define PRA_SIZEOF_SYSCFG_APNPASS  		(20) 	        										// 
#define PRA_SIZEOF_SYSCFG_DNSIP  			(30) 	        										// 
#define PRA_SIZEOF_SYSCFG_DOMAIN1 		(50) 	        										// 
#define PRA_SIZEOF_SYSCFG_DOMAIN2  		(50) 	        										// 
#define PRA_SIZEOF_SYSCFG_IP1  				(30) 	        										// 
#define PRA_SIZEOF_SYSCFG_PORT1  			(5) 	        										// 
#define PRA_SIZEOF_SYSCFG_IP2  				(30) 	        										// 
#define PRA_SIZEOF_SYSCFG_PORT2  			(5) 	        										// 
#define PRA_SIZEOF_SYSCFG_DOMAINEN  			(sizeof(s_cfg.domainEn)) 	    // 
#define PRA_SIZEOF_SYSCFG_GPSCOLLECTTIME  (sizeof(s_cfg.gpsCollectTime)) 	// 	
#define PRA_SIZEOF_SYSCFG_ONREPORTTIME  	(sizeof(s_cfg.onReportTime)) 	// 
#define PRA_SIZEOF_SYSCFG_OFFREPORTTIME  	(sizeof(s_cfg.offReportTime))	// 
#define PRA_SIZEOF_SYSCFG_HEARTBEATTTIME  (sizeof(s_cfg.heartbeatTime)) // 
#define PRA_SIZEOF_SYSCFG_TCPACKTIMEOUT  	(sizeof(s_cfg.tcpAckTimeout)) // 
#define PRA_SIZEOF_SYSCFG_HEALTHREPORTTIME  (sizeof(s_cfg.healthReportTime)) // 	
#define PRA_SIZEOF_SYSCFG_SMSCENTER  	(20) 	        										// 
#define PRA_SIZEOF_SYSCFG_EVENTSWT  			(sizeof(s_cfg.eventSwt)) 	    // 
#define PRA_SIZEOF_SYSCFG_OVERSPEED  			(sizeof(s_cfg.overSpeed)) 	  // 
#define PRA_SIZEOF_SYSCFG_OVERSPEEDTIME  	(sizeof(s_cfg.overSpeedTime)) // 
#define PRA_SIZEOF_SYSCFG_TIREDDIRVETIME  (sizeof(s_cfg.tiredDirveTime))	// 
#define PRA_SIZEOF_SYSCFG_PHO1  			(20) 	        										// 
#define PRA_SIZEOF_SYSCFG_PHO2  			(20) 	        										// 
#define PRA_SIZEOF_SYSCFG_PHO3  			(20) 	        										//
#define PRA_SIZEOF_SYSCFG_PHO4  			(20) 	        										// 
#define PRA_SIZEOF_SYSCFG_PHO5  			(20) 	        										// 
#define PRA_SIZEOF_SYSCFG_PHO6  			(20) 	        										// 
#define PRA_SIZEOF_SYSCFG_AESKEY  		(20) 	    												// 
#define PRA_SIZEOF_SYSCFG_AESTYPE  		(sizeof(s_cfg.aesType)) 	    		// 
#define PRA_SIZEOF_SYSCFG_DEVEN  			(sizeof(s_cfg.devEn)) 	    			// 
#define PRA_SIZEOF_SYSCFG_WMODE  			(sizeof(s_cfg.wMode)) 	    			// 
#define PRA_SIZEOF_SYSCFG_NATIONCODE  (5) 	        										// 
#define PRA_SIZEOF_SYSCFG_SLEEPSWT  	(sizeof(s_cfg.sleepSwt)) 	    		// 
#define PRA_SIZEOF_SYSCFG_DSSATH  		(sizeof(s_cfg.dssATH)) 	    			// 
#define PRA_SIZEOF_SYSCFG_DSSDTH 			(sizeof(s_cfg.dssDTH)) 	    			// 
#define PRA_SIZEOF_SYSCFG_DSSRTH	 		(sizeof(s_cfg.dssRTH)) 	    			// 
#define PRA_SIZEOF_SYSCFG_DSSPTH	 		(sizeof(s_cfg.dssPTH)) 	    			// 
#define PRA_SIZEOF_SYSCFG_ACCMT  			(sizeof(s_cfg.accMT)) 	    			// 
#define PRA_SIZEOF_SYSCFG_ACCMD  			(sizeof(s_cfg.accMD)) 	    			// 
#define PRA_SIZEOF_SYSCFG_SYSMOVE  		(sizeof(s_cfg.sysMove)) 	    		// 
#define PRA_SIZEOF_SYSCFG_SYSMOVET  	(sizeof(s_cfg.sysMoveT)) 	    		// 
#define PRA_SIZEOF_SYSCFG_LOWPOWER  	(sizeof(s_cfg.lowPower)) 	    		// 
#define	PRA_SIZEOF_SYSCFG_TJDELAYTIME (sizeof(s_cfg.tjDelayTime)) 	    // 
#define	PRA_SIZEOF_SYSCFG_IGOFFDELAY 	(sizeof(s_cfg.igOffDelay)) 	    	// 
#define	PRA_SIZEOF_SYSCFG_NAVISTEP 		(sizeof(s_cfg.naviStep)) 	    		// 	
#define PRA_SIZEOF_SYSCFG_SOUND1  		(70) 	    												// 
#define PRA_SIZEOF_SYSCFG_SOUND2  		(70) 	    												// 



//////////////////////////////////////////////////////////////////////
// ϵͳOBD������-������С����(��OBD���Ŷ���)
//#define PRA_SIZEOF_SYSOBD_SAVED_MARK  (sizeof(s_obd.savedMark)) 	  		// 
//#define PRA_SIZEOF_SYSOBD_SUM  				(sizeof(s_obd.sum)) 	        		// 
//#define PRA_SIZEOF_SYSOBD_ODO  				(sizeof(s_obd.odometer)) 	    		// 
//#define PRA_SIZEOF_SYSOBD_FUEL  			(sizeof(s_obd.fuel)) 	        		// 

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ϵͳ��ز�����󳤶ȶ���
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

/////////////////////////////////////////////////////////////////////
// ϵͳͨ�ò�����-��󳤶ȶ���
#define PRA_MAXOF_COMMON_SAVED_MARK  	(PRA_SIZEOF_COMMON_SAVED_MARK) 	  // 
#define PRA_MAXOF_COMMON_SUM  				(PRA_SIZEOF_COMMON_SUM) 	        // 
#define PRA_MAXOF_COMMON_IAPFLAG  		(PRA_SIZEOF_COMMON_IAPFLAG) 	    //
#define	PRA_MAXOF_COMMON_IAPSTA				(PRA_SIZEOF_COMMOM_IAPSTA)				//
#define PRA_MAXOF_COMMON_ID  					(PRA_SIZEOF_COMMON_ID) 	        	// 
#define PRA_MAXOF_COMMON_CARINFO  		(PRA_SIZEOF_COMMON_CARINFO) 	    // 
#define PRA_MAXOF_COMMON_AADC  				(PRA_SIZEOF_COMMON_AADC) 	        // 
#define PRA_MAXOF_COMMON_SWDG  				(PRA_SIZEOF_COMMON_SWDG)          // 
#define PRA_MAXOF_COMMON_HVERB			  (PRA_SIZEOF_COMMON_HVERB+1)      	// 		
#define PRA_MAXOF_COMMON_GPSDIS  			(PRA_SIZEOF_COMMON_GPSDIS) 	      // 
#define PRA_MAXOF_COMMON_OTAPAPN  		(PRA_SIZEOF_COMMON_OTAPAPN+1) 	  // 
#define PRA_MAXOF_COMMON_OTAPAPNUSER 	(PRA_SIZEOF_COMMON_OTAPAPNUSER+1) // 
#define PRA_MAXOF_COMMON_OTAPAPNPASS 	(PRA_SIZEOF_COMMON_OTAPAPNPASS+1) // 
#define PRA_MAXOF_COMMON_OTAPIP  			(PRA_SIZEOF_COMMON_OTAPIP+1) 	    // 
#define PRA_MAXOF_COMMON_OTAPPORT  		(PRA_SIZEOF_COMMON_OTAPPORT+1) 	  // 
#define	PRA_MAXOF_COMMON_OTAPFTPUSER	(PRA_SIZEOF_COMMON_OTAPFTPUSER+1)	//
#define	PRA_MAXOF_COMMON_OTAPFTPPASS	(PRA_SIZEOF_COMMON_OTAPFTPPASS+1)	//
#define	PRA_MAXOF_COMMON_OTAPFTPFILE	(PRA_SIZEOF_COMMON_OTAPFTPFILE+1)	//
#define	PRA_MAXOF_COMMON_OTAPFILESIZE	(PRA_SIZEOF_COMMON_OTAPFILESIZE)	//
#define PRA_MAXOF_COMMON_OTAPERRCODE 	(PRA_SIZEOF_COMMON_OTAPERRCODE) 	// 
#define	PRA_MAXOF_COMMON							(sizeof(SYSCommon_Typedef))

//////////////////////////////////////////////////////////////////////
// ϵͳӦ�ò�����-��󳤶ȶ���
#define PRA_MAXOF_SYSCFG_SAVED_MARK  	(PRA_SIZEOF_SYSCFG_SAVED_MARK) 	  // 
#define PRA_MAXOF_SYSCFG_SUM  				(PRA_SIZEOF_SYSCFG_SUM) 	        // 
#define PRA_MAXOF_SYSCFG_APN  				(PRA_SIZEOF_SYSCFG_APN+1) 	      // 
#define PRA_MAXOF_SYSCFG_APNUSER  		(PRA_SIZEOF_SYSCFG_APNUSER+1) 	  // 
#define PRA_MAXOF_SYSCFG_APNPASS  		(PRA_SIZEOF_SYSCFG_APNPASS+1) 	  // 
#define PRA_MAXOF_SYSCFG_DNSIP  			(PRA_SIZEOF_SYSCFG_DNSIP+1) 	    // 
#define PRA_MAXOF_SYSCFG_DOMAIN1 			(PRA_SIZEOF_SYSCFG_DOMAIN1+1) 	  // 
#define PRA_MAXOF_SYSCFG_DOMAIN2  		(PRA_SIZEOF_SYSCFG_DOMAIN2+1) 	  // 
#define PRA_MAXOF_SYSCFG_IP1  				(PRA_SIZEOF_SYSCFG_IP1+1) 	      // 
#define PRA_MAXOF_SYSCFG_PORT1  			(PRA_SIZEOF_SYSCFG_PORT1+1) 	    // 
#define PRA_MAXOF_SYSCFG_IP2  				(PRA_SIZEOF_SYSCFG_IP2+1) 	      // 
#define PRA_MAXOF_SYSCFG_PORT2  			(PRA_SIZEOF_SYSCFG_PORT2+1) 	    // 
#define PRA_MAXOF_SYSCFG_DOMAINEN  				(PRA_SIZEOF_SYSCFG_DOMAINEN) 	    	// 
#define PRA_MAXOF_SYSCFG_GPSCOLLECTTIME  	(PRA_SIZEOF_SYSCFG_GPSCOLLECTTIME) 	// 
#define PRA_MAXOF_SYSCFG_ONREPORTTIME  		(PRA_SIZEOF_SYSCFG_ONREPORTTIME) 		// 
#define PRA_MAXOF_SYSCFG_OFFREPORTTIME  	(PRA_SIZEOF_SYSCFG_OFFREPORTTIME)		// 
#define PRA_MAXOF_SYSCFG_HEARTBEATTTIME  	(PRA_SIZEOF_SYSCFG_HEARTBEATTTIME)	// 
#define PRA_MAXOF_SYSCFG_TCPACKTIMEOUT  	(PRA_SIZEOF_SYSCFG_TCPACKTIMEOUT) 	// 
#define PRA_MAXOF_SYSCFG_HEALTHREPORTTIME (PRA_SIZEOF_SYSCFG_HEALTHREPORTTIME) 	// 
#define PRA_MAXOF_SYSCFG_SMSCENTER  			(PRA_SIZEOF_SYSCFG_SMSCENTER+1) 	  // 
#define PRA_MAXOF_SYSCFG_EVENTSWT  				(PRA_SIZEOF_SYSCFG_EVENTSWT) 	    	// 
#define PRA_MAXOF_SYSCFG_OVERSPEED  			(PRA_SIZEOF_SYSCFG_OVERSPEED) 	  	// 
#define PRA_MAXOF_SYSCFG_OVERSPEEDTIME  	(PRA_SIZEOF_SYSCFG_OVERSPEEDTIME) 	// 
#define PRA_MAXOF_SYSCFG_TIREDDIRVETIME  	(PRA_SIZEOF_SYSCFG_TIREDDIRVETIME)	// 
#define PRA_MAXOF_SYSCFG_PHO1  				(PRA_SIZEOF_SYSCFG_PHO1+1) 	      // 
#define PRA_MAXOF_SYSCFG_PHO2  				(PRA_SIZEOF_SYSCFG_PHO2+1) 	      // 
#define PRA_MAXOF_SYSCFG_PHO3  				(PRA_SIZEOF_SYSCFG_PHO3+1) 	      // 
#define PRA_MAXOF_SYSCFG_PHO4  				(PRA_SIZEOF_SYSCFG_PHO4+1) 	      // 
#define PRA_MAXOF_SYSCFG_PHO5  				(PRA_SIZEOF_SYSCFG_PHO5+1) 	      // 
#define PRA_MAXOF_SYSCFG_PHO6  				(PRA_SIZEOF_SYSCFG_PHO6+1) 	      // 
#define PRA_MAXOF_SYSCFG_AESKEY  			(PRA_SIZEOF_SYSCFG_AESKEY) 	    	// 
#define PRA_MAXOF_SYSCFG_AESTYPE  		(PRA_SIZEOF_SYSCFG_AESTYPE) 	    // 
#define PRA_MAXOF_SYSCFG_DEVEN  			(PRA_SIZEOF_SYSCFG_DEVEN) 	    	// 
#define PRA_MAXOF_SYSCFG_WMODE  			(PRA_SIZEOF_SYSCFG_WMODE) 	    	// 
#define PRA_MAXOF_SYSCFG_NATIONCODE  	(PRA_SIZEOF_SYSCFG_NATIONCODE+1) 	// 
#define PRA_MAXOF_SYSCFG_SLEEPSWT  		(PRA_SIZEOF_SYSCFG_SLEEPSWT) 	    // 
#define PRA_MAXOF_SYSCFG_DSSATH  			(PRA_SIZEOF_SYSCFG_DSSATH) 	    	// 
#define PRA_MAXOF_SYSCFG_DSSDTH 			(PRA_SIZEOF_SYSCFG_DSSDTH) 	    	// 
#define PRA_MAXOF_SYSCFG_DSSRTH	 			(PRA_SIZEOF_SYSCFG_DSSRTH) 	    	// 
#define PRA_MAXOF_SYSCFG_DSSPTH	 			(PRA_SIZEOF_SYSCFG_DSSPTH) 	    	// 
#define PRA_MAXOF_SYSCFG_ACCMT  			(PRA_SIZEOF_SYSCFG_ACCMT) 	    	// 
#define PRA_MAXOF_SYSCFG_ACCMD  			(PRA_SIZEOF_SYSCFG_ACCMD) 	    	// 
#define PRA_MAXOF_SYSCFG_SYSMOVE  		(PRA_SIZEOF_SYSCFG_SYSMOVE) 	    // 
#define PRA_MAXOF_SYSCFG_SYSMOVET  		(PRA_SIZEOF_SYSCFG_SYSMOVET) 	    // 
#define PRA_MAXOF_SYSCFG_LOWPOWER  		(PRA_SIZEOF_SYSCFG_LOWPOWER) 	    // 
#define	PRA_MAXOF_SYSCFG_TJDELAYTIME	(PRA_SIZEOF_SYSCFG_TJDELAYTIME)		//
#define	PRA_MAXOF_SYSCFG_IGOFFDELAY		(PRA_SIZEOF_SYSCFG_IGOFFDELAY)		//
#define	PRA_MAXOF_SYSCFG_NAVISTEP			(PRA_SIZEOF_SYSCFG_NAVISTEP)			//
#define	PRA_MAXOF_SYSCFG_SOUND1				(PRA_SIZEOF_SYSCFG_SOUND1+1)			//
#define	PRA_MAXOF_SYSCFG_SOUND2				(PRA_SIZEOF_SYSCFG_SOUND2+1)			//
#define	PRA_MAXOF_SYSCFG							(sizeof(SYSConfig_Typedef))

//////////////////////////////////////////////////////////////////////
// ϵͳOBD������-��󳤶ȶ���(��OBD���Ŷ���)

//#define PRA_MAXOF_SYSOBD_SAVED_MARK  	(PRA_SIZEOF_SYSOBD_SAVED_MARK) 	  // 
//#define PRA_MAXOF_SYSOBD_SUM  				(PRA_SIZEOF_SYSOBD_SUM) 	        // 
//#define PRA_MAXOF_SYSOBD_ODO  				(PRA_SIZEOF_SYSOBD_ODO) 	    		// 
//#define PRA_MAXOF_SYSOBD_FUEL  				(PRA_SIZEOF_SYSOBD_FUEL) 	        // 
//#define	PRA_MAXOF_SYSOBD							(sizeof(SYSObd_Typedef))

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ϵͳ��ز���CPU�ڲ�FLASH��ʼ��ַ����
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

/////////////////////////////////////////////////////////////////////
// ϵͳͨ�ò�����-��ʼ��ַ����
#define	ADDR_START_COMMON							(FLASH_CPU_COMMON_START)					// ϵͳͨ�ô洢����ʼ��ַ

//////////////////////////////////////////////////////////////////////
// ϵͳӦ�ò�����-��ʼ��ַ����
#define	ADDR_START_SYSCFG							(FLASH_CPU_SYSCFG_START)					// ϵͳӦ�ò����洢����ʼ��ַ

//////////////////////////////////////////////////////////////////////
// ϵͳӦ�ò�����-��ʼ��ַ����
#define	ADDR_START_SYSOBD							(FLASH_CPU_SYSOBD_START)					// ϵͳOBD�����洢����ʼ��ַ

/////////////////////////////////////////////////////////////////////
// ����ʧ�����Դ�������
#define	DEF_TRYTIMES_CPUREAD				(3)						// CPU flash�����Դ���
#define	DEF_TRYTIMES_CPUWRITE				(3)						// CPU flashд���Դ���

/*
*********************************************************************************************************
*                                               OTHER DEFINES
*********************************************************************************************************
*/

// ϵͳ�����������
typedef enum {		
		DEF_TYPE_COMMON = 0,							// ͨ�ò���																	
		DEF_TYPE_SYSCFG,									// Ӧ�ò���		
		DEF_TYPE_SYSOBD,									// OBD����	
}ENUM_PARA_TYPE;

#define	DEF_PARA_BUSY_ERR							((s8)-10)						// ������ȡæ
#define	DEF_PARA_UNWRITE_ERR					((s8)-9)						// ����δ��д��
#define	DEF_PARA_UNKNOW_ERR						((s8)-8)						// ����δ֪����
#define	DEF_PARA_ERASE_ERR						((s8)-7)						// ������������
#define	DEF_PARA_WRITECMP_ERR					((s8)-6)						// ����д��Աȴ���
#define	DEF_PARA_WRITE_ERR						((s8)-5)						// ����д�����
#define	DEF_PARA_READ_ERR							((s8)-4)						// ������ȡ����
#define	DEF_PARA_TYPE_ERR							((s8)-3)						// �������ʹ���
#define	DEF_PARA_SUM_ERR							((s8)-2)						// ����У��ʹ���
#define	DEF_PARA_INIT_ERR							((s8)-1)						// ����δ��ʼ������
#define	DEF_PARA_NONE_ERR							((s8)0)							// ������ȷ

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// ͨ����Ϣ���ݽṹ
typedef struct	tag_SYSCommon_Def
{
		u16			savedMark;																		// ������ʼ������־
		u16			sum;																					// �����ۼӺͱ�־
		u16			iapFlag;																			// IAP������־(����BootLoader��ʹ��)
		u16			iapSta;																				// IAP����״̬(����Ӧ����ʹ��)
		u8			id[PRA_MAXOF_COMMON_ID];											// �豸ID��
		u8			carInfo[PRA_MAXOF_COMMON_CARINFO];						// ������Ϣ(��ϵ+���ʹ���+���ͻ�Ʒ��)				
		u16			aadc;																					// �豸AD��׼			
		u8			swdg[13];																			// �豸������Ź���Ϣ
		u8			hverb[PRA_MAXOF_COMMON_HVERB];								// �豸Ӳ���汾��
		double	gpsDis;																				// GPS�ۼӹ�����					
		u8			otapApn[PRA_MAXOF_COMMON_OTAPAPN]; 						// OTAP APN	
		u8			otapApnUser[PRA_MAXOF_COMMON_OTAPAPNUSER];		// OTAP APN��½�û���	
		u8			otapApnPass[PRA_MAXOF_COMMON_OTAPAPNPASS];		// OTAP APN��½����	
		u8			otapIp[PRA_MAXOF_COMMON_OTAPIP];							// OTAP������IP��ַ
		u8			otapPort[PRA_MAXOF_COMMON_OTAPPORT];					// OTAP�������˿ں�	
		u8			otapFtpUser[PRA_MAXOF_COMMON_OTAPFTPUSER];		// FTP��½�û���	
		u8			otapFtpPass[PRA_MAXOF_COMMON_OTAPFTPPASS];		// FTP��¼����
		u8			otapFtpFile[PRA_MAXOF_COMMON_OTAPFTPFILE];		// FTP�ļ�Ŀ¼���ļ�
		u32			otapFileSize;																	// OTAP�ļ���С
		u8			otapErrCode;																	// OTAP���´�����																													

}SYSCommon_Typedef;

// �豸����λ����(devEn)//////////////////////////////////////////////
#define	DEF_DEVEN_BIT_TTS							((u8)0x01)					// TTSʹ�ܿ���(1:����,0:�ر�)
#define	DEF_DEVEN_BIT_GPS							((u8)0x02)					// GPSʹ�ܿ���(1:����,0:�ر�)
#define	DEF_DEVEN_BIT_BT							((u8)0x04)					// BTʹ�ܿ���(1:����,0:�ر�)
#define	DEF_DEVEN_BIT_GPRS						((u8)0x08)					// GPRSʹ�ܿ���(1:����,0:�ر�)
#define	DEF_DEVEN_BIT_CON							((u8)0x10)					// Զ�̿���ģ��ʹ�ܿ���(1:����,0:�ر�)
#define	DEF_DEVEN_BIT_SHAKE						((u8)0x20)					// �񶯱�������(1:����,0:�ر�)

//////////////////////////////////////////////////////////////////////
// ϵͳ������Ϣ���ݽṹ
typedef struct	tag_SYSConfig_Def
{			
		u16			savedMark;																		// ������ʼ������־															
		u16			sum;																					// �����ۼӺͱ�־									
		///////////////////////////////////////////////////////////////////////////
		u8			apn[PRA_MAXOF_SYSCFG_APN]; 										// ϵͳAPN	
		u8			apnUser[PRA_MAXOF_SYSCFG_APNUSER];						// ϵͳAPN��½�û�	
		u8			apnPass[PRA_MAXOF_SYSCFG_APNPASS];						// ϵͳAPN��½����	
		u8			dnsIp[PRA_MAXOF_SYSCFG_DNSIP];								// ϵͳ����������IP��ַ
		u8			domain1[PRA_MAXOF_SYSCFG_DOMAIN1];						// ϵͳ����1 
		u8			domain2[PRA_MAXOF_SYSCFG_DOMAIN2];						// ϵͳ����2
		u8			ip1[PRA_MAXOF_SYSCFG_IP1];										// ϵͳ��������IP��ַ(����IP)
		u8			port1[PRA_MAXOF_SYSCFG_PORT1];								// ϵͳ���������˿ں�
		u8			ip2[PRA_MAXOF_SYSCFG_IP2];										// ϵͳ����������IP��ַ(Ӧ��IP)
		u8			port2[PRA_MAXOF_SYSCFG_PORT2];								// ϵͳ�����������˿ں�
		u8			domainEn;																			// ��������ʹ�����Ϊ1��ʹ����������Ϊ0���ֹ�������Ӽ�IP����
		///////////////////////////////////////////////////////////////////////////
		u8			gpsCollectTime;																// GPS�ɼ�ʱ��������λΪ��(s)
		u32			onReportTime;																	// �����Ϣ�㱨�������λΪ��(s)	
		u32			offReportTime;																// Ϩ����Ϣ�㱨�������λΪ��(s)	
		u32     heartbeatTime;                                // �ն��������ͼ������λΪ��(s)	
		u32     tcpAckTimeout;                                // TCP��ϢӦ��ʱʱ�䣬��λΪ��(s)
		u32			healthReportTime;															// �������ϱ�ʱ��������λΪ��(s)
		u8			smsCenter[PRA_MAXOF_SYSCFG_SMSCENTER];				// ���ƽ̨SMSè�绰����	
		u32     eventSwt;                              				// �¼��ϱ������֡���ӦλΪ1���ϱ���Ӧ�¼�0���ϱ�
		u16     overSpeed;                                		// ���ٱ������ޣ���λΪ����ÿСʱ(km/h)
		u16     overSpeedTime;                                // ���ٳ���ʱ�䣬��λΪ��(s)
		u32     tiredDirveTime;                               // ƣ�ͼ�ʻʱ�����ޣ���λΪ��(s)																																																		
		u8			pho1[PRA_MAXOF_SYSCFG_PHO1];									// ���������绰����1
		u8			pho2[PRA_MAXOF_SYSCFG_PHO2];									// ���������绰����2
		u8			pho3[PRA_MAXOF_SYSCFG_PHO3];									// ���������绰����3
		u8			pho4[PRA_MAXOF_SYSCFG_PHO4];									// ���������绰����4
		u8			pho5[PRA_MAXOF_SYSCFG_PHO5];									// ���������绰����5
		u8			pho6[PRA_MAXOF_SYSCFG_PHO6];									// ���������绰����6
		u8			aeskey[PRA_MAXOF_SYSCFG_AESKEY];							// �ն�AES��Կ
		u8			aesType;																			// ��������	
		///////////////////////////////////////////////////////////////////////////
    u8			devEn;											                  // �豸ʹ�ܿ���(�������豸ʹ��λ����)
    u8      wMode;                                        // �豸������ʽ
    u8			nationCode[PRA_MAXOF_SYSCFG_NATIONCODE];			// ���Ҵ���(�й�Ĭ��086)		 
		u8			sleepSwt;																			// ˯�߿���
		u16			dssATH;																				// DSS����������	
		u16			dssDTH;																				// DSS����������
		u16			dssRTH;																				// DSS��ת������	
		u16			dssPTH;																				// DSS��ײ����			
		u16			accMT;																				// ���ٶ�оƬ�ƶ�INT1�������ޣ���λΪ��G(mg)	
		u16			accMD;																				// ���ٶ�оƬ�ƶ�INT1����ʱ������λ����(ms)		
		u8			sysMove;																			// ϵͳ�ƶ��ٶ�����,��λ:����/Сʱ(ʹ��GPS�ٶ��ж��ƶ�ʱ����)
		u8			sysMoveT;																			// ϵͳ�ƶ�ʱ������,��λ:��(ʹ��GPS�ٶ��ж��ƶ�ʱ����)
		u16			lowPower;																			// ��ѹ��������,��λ:V(����100���洢)
		u8			tjDelayTime;																	// �����ʱʱ����λ��
		u16			igOffDelay;																		// Ϩ������ʱʱ�䵥λ��
		vu8			naviStep;																			// IP�����׶α���(0-��Ҫ��������IP;1-��ȡIP�ɹ�;2-IP�Ѿ�ʧЧ)
		u32			globSleepTime;																// �豸˯��ȫ��ʱ��(��λ:s)
		u8			sound1[PRA_MAXOF_SYSCFG_SOUND1];							// ��쿪ʼ��������
		u8			sound2[PRA_MAXOF_SYSCFG_SOUND2];							// ������
		//u8      testapp2;
}SYSConfig_Typedef;

////////////////////////////////////////////////////////////////////////
//// OBD�洢���ݽṹ
//typedef struct	tag_SYSObd_Def
//{			
//		u16			savedMark;																		// ������ʼ������־														
//		u16			sum;																					// �����ۼӺͱ�־
//		double	odometer;																			// OBD������
//		double	fuel;																					// OBD�ۼ��ͺ�
//																																																						
//}SYSObd_Typedef;


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/
HAL_EXT		u16										globInitPVDFlag;			// ��ʼ����PVD���ϱ�־(OS����ǰ���PVD��Ч�ñ�־����)
HAL_EXT		SYSCommon_Typedef			s_common;
HAL_EXT		SYSConfig_Typedef			s_cfg;  
//HAL_EXT		SYSObd_Typedef				s_obd;  

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

// ϵͳ��ʼ������
void 			HAL_CPU_ResetPro 					(void);
void			HAL_INIT_ALL							(void);
u8				HAL_SysParaLoadProcess 		(void);
void			HAL_ParaSetDefault 				(ENUM_PARA_TYPE	type);
void			HAL_ParaSetDefaultExt 		(ENUM_PARA_TYPE	type);

// ϵͳ������д�ӿں���
s8				HAL_LoadParaPro 					(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);
s8				HAL_SaveParaPro 					(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);

// ϵͳ˯�߽ӿں���
s8				HAL_PerpheralInPowerMode	(void);
s8				HAL_PerpheralOutPowerMode	(void);

// ϵͳ�����ź�����ȡ
s8				HAL_SYSParaSemPend 				(u32	timeout);
void			HAL_SYSParaSemPost 				(void);

s8 HAL_CommonParaReload(void);
void HAL_SysParaReload(void);
void HAL_CfgChk(void);
/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
