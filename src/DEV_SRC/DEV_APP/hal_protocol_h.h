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
*                                     		hal_protocol_h.h
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_protocol.h
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

#ifndef  HAL_PROTOCOL_H_H
#define  HAL_PROTOCOL_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   HAL_PROTOCOL_GLOBLAS
#define  HAL_PROTOCOL_EXT
#else
#define  HAL_PROTOCOL_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include 	"stm32f10x.h"
#include	"stddef.h"
#include	"bsp_acc_h.h"
#include	"hal_processGPS_h.h"

/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// �������Ŷ���

#define	USC2_CHECK_BALANCE						("8BF760A853CA65F668C067E58F668F8600530049004D53614F59989D3002")	// ������ʱ��鳵��SIM�����
#define	USC2_MONEY_CODE1							("4E0D8DB3")	// ����
#define	USC2_MONEY_CODE2							("4F59989D")	// ���
#define	USC2_MONEY_CODE3							("8BDD8D39")	// ����

#define	GSM_MONEY_CODE1								(0x0DBB)	// ����
#define	GSM_MONEY_CODE2								(0x599D)	// ���
#define	GSM_MONEY_CODE3								(0xDD39)	// ����


// ���ſ��ƶ���
#define	GSM_SMS_CONTROL_H							("WL-CTL-CMD:")						// ����������Ϣ��ʼ
#define	GSM_SMS_NUMCHECK_H						("WL-NUM-CMD:")						// SIM���������������ʼ



/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ϵͳЭ����ض���
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

#define	DEF_ZXT_PRO_START							((u8)0x7E)					// Э��ͷ
#define	DEF_ZXT_PRO_PROFLAG						((u8)0x10)					// ��Ϣ��ʶ


//////////////////////////////////////////////////////////////////////
// ͨ��Ӧ�����Ͷ���
#define	DEF_ACKTYPE_OK								((u8)0x00)					// �ɹ�/ȷ��
#define	DEF_ACKTYPE_ERR								((u8)0x01)					// ʧ��
#define	DEF_ACKTYPE_BUSY							((u8)0x02)					// æ

//////////////////////////////////////////////////////////////////////
// ����ID����-���в���(�������·�)

#define	DEF_CMDID_NAVI_PLA						((u16)0x0E01)				// �������·�-����IP��ַ����Ӧ��(������IP��ַ��APN)
#define	DEF_CMDID_COMACK_PLA					((u16)0x6001)       // �������·�-ͨ��Ӧ��
#define	DEF_CMDID_UPDATE_PLA					((u16)0x0124)       // �������·�-�豸���� 
#define	DEF_CMDID_UPDATEINFO_PLA			((u16)0x0125)       // �������·�-������Ϣ
#define	DEF_CMDID_UPDATEDATA_PLA			((u16)0x0126)       // �������·�-�����ļ�
#define	DEF_CMDID_UPDATEQUE_PLA				((u16)0x0131)       // �������·�-�����ɹ���ѯ
#define	DEF_CMDID_SETIP_PLA						((u16)0x0127)       // �������·�-����IP��ַ
#define	DEF_CMDID_SETODO_PLA					((u16)0x0132)       // �������·�-��ʼ�����
#define	DEF_CMDID_DEVRESET_PLA				((u16)0x0133)       // �������·�-�豸����
#define	DEF_CMDID_GPSUPTIME_PLA				((u16)0x0134)     	// �������·�-�豸GPS�ɼ����
#define	DEF_CMDID_MODULESWT_PLA				((u16)0x0135)     	// �������·�-�����豸ģ�鿪��ָ��
#define	DEF_CMDID_ENGINEBRAND_PLA			((u16)0x0137)     	// �������·�-������Ʒ��
#define	DEF_CMDID_POWOFFDELAY_PLA			((u16)0x0140)     	// �������·�-��ʱ���ػ�(����˯��)
#define	DEF_CMDID_SETTIME_PLA					((u16)0x0141)     	// �������·�-ʱ���·�ָ��
#define	DEF_CMDID_SETPAILIANG_PLA			((u16)0x0145)     	// �������·�-����������
#define	DEF_CMDID_SETTHRESHOLD_PLA		((u16)0x0147)     	// �������·�-�豸��ֵ����
#define	DEF_CMDID_QUEPARA_PLA					((u16)0x0148)     	// �������·�-�豸������ѯ
#define	DEF_CMDID_SMSCENTERPHO_PLA		((u16)0x0151)     	// �������·�-�������ĺ���(��Ҫ�豸�ظ����ŵ��ú���)
#define	DEF_CMDID_SIMCARDPHO_PLA			((u16)0x0152)     	// �������·�-�豸SIM������
#define DEF_CMDID_SETPHONUM_PLA				((u16)0x0153)     	// �������·�-���úϷ����ſ��ƺ���
#define	DEF_CMDID_QUEPHONUM_PLA				((u16)0x0154)     	// �������·�-��ѯ�Ϸ����ſ��ƺ���
#define	DEF_CMDID_SETSLEEPTIME_PLA		((u16)0x0156)       // �������·�-�����豸˯�߼�������ʱ����
#define	DEF_CMDID_RTSTART_PLA					((u16)0x0287)       // �������·�-ר����ϯʵʱ����������
#define	DEF_CMDID_RTSTOP_PLA					((u16)0x0288)       // �������·�-ר����ϯʵʱ������ֹͣ
#define	DEF_CMDID_STARTTJ_PLA					((u16)0x0293)       // �������·�-��ʼ�ֶ����
#define	DEF_CMDID_READFUEL_PLA				((u16)0x0296)       // �������·�-ʣ��������ѯ
#define	DEF_CMDID_CONCAR_PLA					((u16)0x0A01)       // �������·�-��������(ͨ��OBD���ⲿ����ģ��)
#define	DEF_CMDID_CONTTS_PLA					((u16)0x0A03)       // �������·�-TTS����
#define	DEF_CMDID_CLRCODE_PLA					((u16)0x0A04)       // �������·�-����ָ��
#define	DEF_CMDID_QUECAR_PLA					((u16)0x0A06)       // �������·�-��ѯ����״̬(ͨ��OBD���ⲿ����ģ��)


//////////////////////////////////////////////////////////////////////
// ����ID����-���в���(�豸�ϴ�)

#define	DEF_CMDID_NAVI_DEV						((u16)0x0E01)				// �豸�ϴ�-����IP��ַ����
#define	DEF_CMDID_COMACK_DEV					((u16)0x6001)       // �豸�ϴ�-ͨ��Ӧ��
#define	DEF_CMDID_UPDATEREQ_DEV				((u16)0x01AD)       // �豸�ϴ�-��������ָ��
#define	DEF_CMDID_UPDATEACK_DEV				((u16)0x01AF)       // �豸�ϴ�-Ӧ���������
#define	DEF_CMDID_NETBRAKREQ_DEV			((u16)0x01AA)       // �豸�ϴ�-����Ͽ�ָ��
#define	DEF_CMDID_SLEEPREQ_DEV				((u16)0x01BB)       // �豸�ϴ�-����˯��ָ��
#define	DEF_CMDID_HEALTH_DEV					((u16)0x0146)       // �豸�ϴ�-�豸״̬�ϴ�(24H������)
#define	DEF_CMDID_PARAACK_DEV					((u16)0x0149)       // �豸�ϴ�-������ѯӦ��
#define	DEF_CMDID_ERRORLOG_DEV				((u16)0x0150)       // �豸�ϴ�-���ϱ����ϴ�
#define	DEF_CMDID_PHONUMACK_DEV				((u16)0x0155)       // �豸�ϴ�-�Ϸ����ſ��ƺ����ѯӦ��
#define	DEF_CMDID_POWON_DEV						((u16)0x0202)       // �豸�ϴ�-�豸��һ���ϵ籨��
#define	DEF_CMDID_LOGIN_DEV						((u16)0x0286)       // �豸�ϴ�-�豸��½/����$$$$$$$$
#define	DEF_CMDID_HEARD_DEV						((u16)0x0282)       // �豸�ϴ�-�豸����
#define	DEF_CMDID_GPS_DEV							((u16)0x0283)       // �豸�ϴ�-����GPS�ϴ�����
#define	DEF_CMDID_ZITAI_DEV						((u16)0x0284)       // �豸�ϴ�-��̬�ϴ�����
#define	DEF_CMDID_IG_DEV							((u16)0x0285)       // �豸�ϴ�-�豸���Ϩ������
#define	DEF_CMDID_RTDATA_DEV					((u16)0x0289)       // �豸�ϴ�-ʵʱ����������
#define	DEF_CMDID_RTDATAEND_DEV				((u16)0x0290)       // �豸�ϴ�-ʵʱ���������ݽ���
#define	DEF_CMDID_FLASH_DEV						((u16)0x0291)       // �豸�ϴ�-FLASH���������ϴ�
#define	DEF_CMDID_VIN_DEV							((u16)0x0292)       // �豸�ϴ�-VIN���ϴ� 
#define	DEF_CMDID_TJDATA_DEV					((u16)0x0294)       // �豸�ϴ�-��������ϴ�
#define	DEF_CMDID_SMSCON_DEV					((u16)0x0295)       // �豸�ϴ�-���ų���������Ӧ
#define	DEF_CMDID_READFUEL_DEV				((u16)0x0297)       // �豸�ϴ�-ʣ��������ѯ��Ӧ
#define	DEF_CMDID_CONCARACK_DEV				((u16)0x0A02)       // �豸�ϴ�-���ƽ��Ӧ��
#define	DEF_CMDID_CLRCODEACK_DEV			((u16)0x0A05)       // �豸�ϴ�-����Ӧ��
#define	DEF_CMDID_QUECARACK_DEV				((u16)0x0A07)       // �豸�ϴ�-��ѯ����״̬Ӧ��
#define	DEF_CMDID_ALARM_DEV						((u16)0x0A08)       // �豸�ϴ�-���������ϴ�


//////////////////////////////////////////////////////////////////////
// ������ݰ���ض���

#define	DEF_PRO_MKPACK_ERRPACK				((s8)-2)
#define	DEF_PRO_MKPACK_ERRTYPE				((s8)-1)
#define	DEF_PRO_MKPACK_NONE						((s8)0)

//////////////////////////////////////////////////////////////////////
// �������ݰ���ض���

#define DEF_PRO_UNPACK_UNKNOW         ((s8)-100)
#define	DEF_PRO_UNPACK_ERRLEN					((s8)-5)
#define	DEF_PRO_UNPACK_ERRCRC					((s8)-4)
#define DEF_PRO_UNPACK_ERRPARA        ((s8)-3)
#define	DEF_PRO_UNPACK_ERRCMDID				((s8)-2)
#define	DEF_PRO_UNPACK_ERRSTART				((s8)-1)
#define	DEF_PRO_UNPACK_NONE						((s8)0)

//////////////////////////////////////////////////////////////////////
// �豸״̬λ����(���ڵ�½/������Ϣ�豸״̬�ϴ�)
//#define	DEF_DEVSTA_BIT_TTS						((u8)0x01)					// TTS�Ƿ�֧��,0֧��,1��֧��
//#define	DEF_DEVSTA_BIT_TTSEN					((u8)0x02)					// TTS�Ƿ�ʹ��,1ʹ��,0��ֹ
//#define	DEF_DEVSTA_BIT_GPS						((u8)0x04)					// GPS�Ƿ�֧��,0֧��,1��֧��
//#define	DEF_DEVSTA_BIT_GPSEN					((u8)0x08)					// GPS�Ƿ�ʹ��,1ʹ��,0��ֹ
//#define	DEF_DEVSTA_BIT_BT							((u8)0x10)					// BT�Ƿ�֧��,0֧��,1��֧��
//#define	DEF_DEVSTA_BIT_BTEN						((u8)0x20)					// BT�Ƿ�ʹ��,1ʹ��,0��ֹ
//#define	DEF_DEVSTA_BIT_CON						((u8)0x40)					// CON�Ƿ�֧��,0֧��,1��֧��
//#define	DEF_DEVSTA_BIT_CONEN					((u8)0x80)					// CON�Ƿ�ʹ��,1ʹ��,0��ֹ

		
/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/
//////////////////////////////////////////////////////////////////////
// ���ſ��Ʒ����붨��(��Э�鷵����)
			
#define DEF_SMSCONTROL_NONE       		((s8)0)							// �ɹ�
#define	DEF_SMSCONTROL_FORMAT					((s8)-1)						// ��ʽ����
#define	DEF_SMSCONTROL_DES						((s8)-2)						// ����ʧ��
#define DEF_SMSCONTROL_BUSY      			((s8)-3)						// ����æ
#define DEF_SMSCONTROL_NOSUPORT   		((s8)-4)						// ���߲�֧��
#define DEF_SMSCONTROL_NUM    				((s8)-5)						// �Ƿ����ƺ���
#define DEF_SMSCONTROL_TIMEOUT    		((s8)-6)						// ��Ϣ���Ƴ�ʱ
#define DEF_SMSCONTROL_TIMESTAMP    	((s8)-7)						// ����ʱ����Ƿ�
#define DEF_SMSCONTROL_INVALID    		((s8)-8)						// ��Ч��Ϣ

// ����(GPRS)���ƣ����ķǷ�Ӧ�𷵻ض���
typedef enum
{
		SMS_ACK_BUSY						=0x01,      // �豸æ
		SMS_ACK_NOSUPORT				=0x02,			// ��֧��
		SMS_ACK_ILLNUM					=0x03,      // �Ƿ�����
		SMS_ACK_TIMEOUT         =0x04,			// ���Ž��ճ�ʱ
		SMS_ACK_TIMESTAMP       =0x05,			// ��������ʱ�����ʱ
	
}SMS_ACK_TYPE;


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// Э������ṹ����
#define	DEF_PARABUF_SIZE					    (1060)             	// �������ݰ�������������С(���ǵ����������ļ���С)
#define	DEF_PARAPACK_SIZE					    (240)             	// ��������ݰ�������������С
typedef	struct tag_PROTOCOL_Def
{
		u8		start;							// ��ʼ�ֽ�
		u8		proFlag;						// Э���ʶ
		u16		proLen;							// Э�鳤��
		u16		bodySn;							// �������к�	
		u16		bodycmdId;					// ���������ʶ
		u16		paraLen;						// ��������
		u8		paraData[DEF_PARABUF_SIZE];			// ������
		u16		crcCode;						// CRCУ����

}Protocol_Typedef;

//////////////////////////////////////////////////////////////////////
// ������Ӧ�����ݰ��ṹ
typedef struct	tag_AckData_Def
{
		u16		msgId;																					// �̶�0x6001��0x0E01
		u16		ackSn;																					// Ӧ����ˮ��
		u8		result;																					// DEF_ACKTYPE_OK or DEF_ACKTYPE_ERR
	
}AckData_TypeDef;

//////////////////////////////////////////////////////////////////////
// �������̽������ݰ��ṹ

// ���������붨��

#define	DEF_IAP_XUCHUAN										((u8)0xAA)			// �ϵ�����
#define	DEF_IAP_NONE_ERR									((u8)0x00)
#define	DEF_IAP_NET_ERR										((u8)0xC0)
#define	DEF_IAP_FILE_SIZE_ERR							((u8)0xD0)
#define	DEF_IAP_START_SIZE_ERR						((u8)0xD1)
#define	DEF_IAP_PACK_SIZE_ERR							((u8)0xD2)
#define	DEF_IAP_WFLASH_ERASE_ERR					((u8)0xE0)
#define	DEF_IAP_WFLASH_READ_ERR						((u8)0xE1)
#define	DEF_IAP_WFLASH_WRITE_ERR					((u8)0xE2)
#define	DEF_IAP_CFLASH_ERASE_ERR					((u8)0xF0)
#define	DEF_IAP_CFLASH_READ_ERR						((u8)0xF1)
#define	DEF_IAP_CFLASH_WRITE_ERR					((u8)0xF2)
#define	DEF_IAP_UNKNOW_ERR								((u8)0xF3)	

#define	DEF_UPDATEBUF_SIZE					      (1040)          // Ӧ��ṹ���ݻ�������С(���ǵ������������ݰ����(һ��1K))
typedef struct	tag_UpdateData_Def
{
		u16		msgId;																					// �̶�0x6001��0x0E01
		u16		sn;																							// �յ���Ϣ��ˮ��
		vu8		writeLock;																			// ������������־(0:���п�д�룬1:ռ����)
		u16		dataLen;																				// ��������������															
		u8		dataBuf[DEF_UPDATEBUF_SIZE];										// ��������������
	
}UpdateData_TypeDef;

//////////////////////////////////////////////////////////////////////
// ��Ҫ�豸����Ŀ��ƽṹ(�豸����)

// �豸�������Ͷ���
typedef enum
{
		TTS_CMD								=0xA0,				// TTS��ʱ��������		
		CAR_CMD_IG_ON					=0xC0,        // �������
		CAR_CMD_IG_OFF				=0xC1,        // ����Ϩ��
		CAR_CMD_ARM_SET				=0xC2,				// �������
		CAR_CMD_ARM_RST				=0xC3,				// ��������
		CAR_CMD_FIND_CAR			=0xC4,				// Ѱ��
		CAR_CMD_CATCH_CAR			=0xC5,				// ���س���
		CAR_CMD_FREE_CAR			=0xC6,				// �ָ�����
		CAR_CMD_TRUNK					=0xC7,				// ����
		CAR_CMD_READSTA				=0xC8,				// ��ȡ����״̬
		CAR_CMD_WINUP					=0xC9,				// ����
		CAR_CMD_WINDOWN				=0xCA,				// ����

}DEV_CMDCODE_TYPE;

#define	DEF_CONDEVBUF_SIZE					  (120)             	// ��������С(TTSʵʱ������󳤶��ֽ�����60)
typedef struct	tag_ConDevData_Def
{
		DEV_CMDCODE_TYPE		type;															// �������
		vu8		dataLock;										 									 	// ������(���ͷ���λ,���շ����)
		u16		ackSn;																					// ��Ҫ��Ӧ�����ˮ��
		u16		paraLen;																				// ��������
		u8		paraBuf[DEF_CONDEVBUF_SIZE];										// ������
							
}ConDevData_TypeDef;

//////////////////////////////////////////////////////////////////////
// ��ҪOBD����Ŀ��ƽṹ(OBD����)

// OBD�����ӿ����ݽṹ
typedef enum
{
		CMD_IG_ON					=0,               // �豸����¼�
		CMD_IG_OFF,                 			  // �豸Ϩ���¼�
		CMD_SYS_WAKEUP,            				  // �豸����
		CMD_SYS_SLEEP,											// �豸׼������˯��
		CMD_AUTO_TJ,											  // �Զ����(���豸������ɻ���ʱʹ��)
		CMD_READ_ALARMSTA,									// ��ȡ��������״̬��Ϣ(ÿ��Ϩ����·�OBD���ر���״̬(�п���δ����δ�س��ţ�δ�ش�))
		CMD_READ_BOXSTA,										// ��ȡ����ת�Ӱ�״̬��Ϣ(�Ƿ���Ч)
		CMD_CAR_INFO,												// ��ϵ������Ϣ�·�(�豸ÿ���ϵ����͸�OBDһ��)
		CMD_PROTOCOL,											  // Э�����
		CMD_SMS,														// ���ſ���
		CMD_TEST,														// �������߲���
		CMD_BT,															// ��������ͨ��
		CMD_GET_CARDATA,										// ��ȡ����������Ϣ(RPM״̬)
		CMD_READ_FUEL,											// ��ȡ��ǰ������Ϣ
		CMD_READ_VIN,												// ������VIN������Ϣ

}OBD_CMDCODE_TYPE;

#define	DEF_OBDBUF_SIZE					  	(500)             	// ��������С(����OBD�ظ�ʹ��)
typedef struct	tag_OBDMbox_Def
{
		OBD_CMDCODE_TYPE	cmdCode;				  // �������(��ö�ٶ���)
		vu8		dataLock;										  // ������(���ͷ���λ,���շ����)
		vu16	sn;													  // ������ˮ��(��Э����ˮ��)			
		vu16	proSn;											  // Э����ˮ��
		vu16	proCmdId;										  // Э������ID
		vu16	proParaLen;									  // ��������
		vu8		proParaBuf[DEF_OBDBUF_SIZE];		// ������
		vu8		retCode;
							
}OBDMbox_TypeDef;

#define	DEF_CMDBUF_SIZE					  (30)             		// ��������С(�����·�OBD��������)
typedef struct	tag_OBDMboxCmd_Def
{
		OBD_CMDCODE_TYPE	cmdCode;				  // �������(��ö�ٶ���)
		vu8		dataLock;										  // ������(���ͷ���λ,���շ����)
		vu16	sn;													  // ������ˮ��(��Э����ˮ��)			
		vu16	proSn;											  // Э����ˮ��
		vu16	proCmdId;										  // Э������ID
		vu16	proParaLen;									  // ��������
		vu8		proParaBuf[DEF_CMDBUF_SIZE];		// ������
		vu8		retCode;
							
}OBDMboxCmd_TypeDef;

//////////////////////////////////////////////////////////////////////
// ��Ϩ������
typedef enum
{
		TYPE_IG_ON		=0x00,             		// ���
		TYPE_IG_OFF		=0x01,                // Ϩ��
		TYPE_IG_OFFEXT	=0x02,              // Ϩ�𲹱�(����������ʱ��ƫ����)
		TYPE_IG_OFFNEW	=0x03,							// �µ�Ϩ���ĸ�ʽ(�����˵����Ϣ)

}IG_TYPE;

//////////////////////////////////////////////////////////////////////
// ��̬����
typedef enum
{
		TYPE_ACC_ATH	=0x00,             		// ������
		TYPE_ACC_DTH	=0x01,                // ������
		TYPE_ACC_ROLL	=0x02,                // ����
		TYPE_ACC_CRASH=0x03,                // ��ײ
		TYPE_ACC_RTH	=0x04,                // ��ת��

}ACC_TYPE;

//////////////////////////////////////////////////////////////////////
// ��������
typedef enum
{
		ALARM_BIT_POWER		=0x01,            // ��ѹ����
		ALARM_BIT_SHAKE		=0x02,            // �𶯱���
		ALARM_BIT_MOVE		=0x04,            // Ϩ��λ�Ʊ���
		ALARM_BIT_WIN			=0x08,            // ����δ�ر���
		ALARM_BIT_DOOR		=0x10,            // ����δ�ر���
		ALARM_BIT_LOCK		=0x20,            // �п���δ�ر���
		ALARM_BIT_LDOOR		=0x40,            // ��������δ�ر���
		ALARM_BIT_LTRUNK	=0x80,            // ���������δ�ر���
	
		ALARM_EXT_BIT_ILLOPEN	=0x0100,			// ��չ����λ�Ƿ����Ż�δ������ʾ
  
    ALARM_EXT_BIT_REMOVE	=0x010000,	  // ��չ����λ�����ʾ

}ALARM_BIT_TYPE;

//////////////////////////////////////////////////////////////////////
// ����LOG����
typedef enum
{
		TYPE_LOG_FLASH		=0xE0,            // FLASH��д����
		TYPE_LOG_SIM			=0xE1,            // SIM������
		TYPE_LOG_GSM			=0xE2,            // GSMģ�����
		TYPE_LOG_GSMNET		=0xE3,            // GSM��������
		TYPE_LOG_GPS			=0xE4,            // GPSģ�����
		TYPE_LOG_OBDBUS		=0xE5,            // OBD����ͨ�Ź���
		TYPE_LOG_OBDFLAG	=0xE6,            // OBD���ʹ������
		TYPE_LOG_ACC			=0xE7,            // ���ٶ�оƬ����
		TYPE_LOG_CPUFLASH	=0xE8,						// CPU FLASH��д����
		TYPE_LOG_CON			=0xE9,						// �ⲿ����ģ��ͨ�Ź���					

}LOG_TYPE;

//////////////////////////////////////////////////////////////////////
// DSS����(��ʻϰ��)
typedef enum
{
		TYPE_DSS_ATH	=0x00,            		// ����
		TYPE_DSS_DTH	=0x01,            		// ����
		TYPE_DSS_RTH	=0x02,            		// ת��

}DSS_TYPE;

//////////////////////////////////////////////////////////////////////
// ��̬����(��ʻϰ��)
typedef enum
{
		TYPE_ZITAI_ATH	=0x00,            	// ������
		TYPE_ZITAI_DTH	=0x01,            	// ������
		TYPE_ZITAI_OTH	=0x02,            	// ����
		TYPE_ZITAI_PTH	=0x03,            	// ��ײ
		TYPE_ZITAI_WTH	=0x04,            	// ��ת��

}ZITAI_TYPE;

//////////////////////////////////////////////////////////////////////
// ����״̬�������(֧�ֶ�״̬����)
typedef enum
{
		CAR_STATE_TYPE_LIGHT		=0x01,			// ����
		CAR_STATE_TYPE_DOOR			=0x02,      // ���ż�����
		CAR_STATE_TYPE_IG_A			=0xA1,      // ����״̬(�¶�)
		CAR_STATE_TYPE_ARM_A		=0xA2,    	// ���״̬(�¶�)
		CAR_STATE_TYPE_DOOR_A		=0xA3,      // ���ż�����(�¶�)

}CAR_STATE_TYPE;


//////////////////////////////////////////////////////////////////////
// �������ƶ�Ϣ�ṹ

typedef	struct tag_SMSCon_Def
{
		u8		dataLock;											//  ������
		u8		numLen;												//	�绰���볤��
		u8		numBuf[20];										//	�����͵Ķ��ź���(���ܺ�ȥ����+��)
		u16		dataLen;											//	���յ��������ݳ���(���ܺ�Դ����)
		u8		dataBuf[160];									//	����������(���ܺ�Դ����)		
		u8		cmdLen;												//  �����볤��
		u8		cmdCode[8];										//  ������
		u32		smsTimStamp;									//  ����������ʱ���(HEX��ֵ��ʽ,���������ʱ�����ͬ)
		u8		finishFlag;										//  ������ɱ�־
		u8		resultCode;										//	����ָ��ִ�н����
					    
}SMSCon_Typedef;


//////////////////////////////////////////////////////////////////////
// OBDͨ�ŵȴ����ݽṹ

//////////////////////////////////////////////////////////////////////
// ��������������ݽṹ

typedef struct	tag_CONReqMbox_Def
{
		DEV_CMDCODE_TYPE	cmdCode;				  // �������(��ö�ٶ���)
		vu8		dataLock;										  // ������(���ͷ���λ,���շ����)
		vu16	sn;													  // ������ˮ��(��Э����ˮ��)			
//		vu16	ackSn;											  // Э��Ӧ��SN(���Ӧ�����ݰ�ʱ����)
//		u32		timeStamp;										// ����ʱ���(���Ӧ�����ݰ�ʱ����)
							
}CONReqMbox_TypeDef;

// �������Ӧ�����ݽṹ(0x01,0x03~0x05����Ϊ���ƶ������ݺϷ���Ӧ��)
typedef enum
{
		CAR_ACK_SUCCESS					=0x00,      // ���Ƴɹ�
		CAR_ACK_UNSUPS					=0x02,      // ��֧��
		CAR_ACK_AUTH            =0x06,			// Ȩ������
		CAR_ACK_ILLCMD          =0x07,			// �޷�ʶ���ָ�������
		CAR_ACK_ILLMODE					=0x08,			// ��ǰģʽ��֧�ִ˲���
		CAR_ACK_BRAKE						=0x09,			// ��ɲ��δ���²�֧�ִ˲���
		CAR_ACK_HBRAKE					=0x0A,			// ��ɲ��δ����֧�ִ˲���
		CAR_ACK_DRIVEING				=0x0B,			// ��ʻ��
		CAR_ACK_TIMEOUT					=0x0C,			// ���Ƴ�ʱ
	
}CAR_ACK_TYPE;

typedef struct	tag_CONAckMbox_Def
{
		vu8		finishFlag;										// Ӧ����ɱ�־
		vu16	sn;													  // ������ˮ��(��Э����ˮ��)		
		DEV_CMDCODE_TYPE	cmdCode;				  // �������(��ö�ٶ���)
		CAR_ACK_TYPE		retCode;						// ���ƽ��
							
}CONAckMbox_TypeDef;


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

HAL_PROTOCOL_EXT			AckData_TypeDef				s_ack;								// ͨ��Ӧ������ṹ
HAL_PROTOCOL_EXT			ConDevData_TypeDef		s_conDev1,s_conDev2;	// ͨ���豸���Ʊ����ṹ(2��������ʱ����)
HAL_PROTOCOL_EXT			UpdateData_TypeDef		s_update;							// ����������ʹ�õĽ������ݽṹ
HAL_PROTOCOL_EXT			CONReqMbox_TypeDef		s_devReq;							// ���������������

HAL_PROTOCOL_EXT			vu16									glob_readCarSn;				//  ����״̬��ȡ����sn
HAL_PROTOCOL_EXT			vu32									glob_readTimeStamp;		//  ����״̬��ȡ����ʱ���
HAL_PROTOCOL_EXT			vu8										glob_odoBuf[3];				//	OBD��칫��������

HAL_PROTOCOL_EXT			OBDMboxCmd_TypeDef		s_obdCmd;							//  ��OBD�������ݽṹ



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

// Э����غ���
s8				HAL_ProUnpackMain								(u8	*pBuf,	u16	len);

// OBD/ϵͳ���ƽ���
s8				HAL_SendOBDProProcess						(OBD_CMDCODE_TYPE	cmdCode,	u16 proSn,	u16 cmdId,	u8 *pPara,	u16 len);
s8				HAL_SendCONProProcess						(DEV_CMDCODE_TYPE	cmdtype,	u16 proSn,	u8 *pPara,	u16 len);

// �豸�ϴ�����Ӵ���(������д洢)
s8				HAL_GPSInQueueProcess						(void);
s8				HAL_ZiTaiInQueueProcess					(ZITAI_TYPE type,	GPSProData_Def *pGps,	s16 X_Axis, s16 Y_Axis, s16 Z_Axis, s16 XYZ_Axis);
s8				HAL_24RptInQueueProcess					(void);
s8				HAL_IGInQueueProcess						(IG_TYPE type,	u32 offSetSec);
s8				HAL_AlarmInQueueProcess					(ALARM_BIT_TYPE alarmBit);
s8				HAL_LogInQueueProcess						(LOG_TYPE type);
s8				HAL_DssInQueueProcess						(DSS_TYPE type);
s8				HAL_SMSConRetInQueueProcess			(SMSCon_Typedef* conSms);
s8				HAL_ReadCarStaInQueueProcess		(u16 ackSn,	u32 reqTimeStmp);
s8				HAL_PowOnInQueueProcess					(u8 staByte,	u32 offSetTick,	u32 powOnRtc);
s8				HAL_SMSInQueueProcess						(u8 smsMode, u8 smsType,	u8 *num,	u16 numLen,	u8 *pData,	u16 len);


// ���ʹ������///////////////////////////////////////////////////////////////////
s8				HAL_PackComAckProcess						(u8 *pPara,	u16 paraLen,	u8 *outBuf,	u16 *outLen);
s8				HAL_PackOBDDataProcess					(u16 cmdId,	u8 *pPara,	u16 paraLen,	u8 *outBuf,	u16 *outLen, u16 *retSn);
s8				HAL_PackRealTimeDataProcess			(u8 *dataType,	u8 *outBuf,	u16 *outLen, u16 *retSn);
s8				HAL_PackFlashDataProcess				(u16	maxLen,	u8 *outBuf,	u16 *outLen, u16 *retSn,	u16 *lastOut,	u16 *outNum);

// ����/��½/���ִ������(ʵʱ��������洢)
s8 				HAL_BuildNavi_Process 					(u8 *pOutBuf,	u16 *outLen);
s8 				HAL_BulidLogin_Process 					(u8 *pOutBuf,	u16 *outLen, u16 *retSn);

// ����/����/˯�߽���������(ʵʱ��������洢)
s8 				HAL_BulidComPack_Process 				(u16	cmdId,	u8 *pOutBuf,	u16 *outLen, u16 *retSn);

// �����ļ����ش���///////////////////////////////////////////////////////////////
u8				HAL_DownLoadFileProcess					(u8 times);

// ���ſ���Э�鴦��///////////////////////////////////////////////////////////////
s8				HAl_SMSTimeOut									(u8 *pSrcBuf, u16 srcLen,	u8 *pRetBuf,	u16 *retLen);
s8				HAl_SMSCarControl								(u8 *pCmdCode,	u16 cmdCodeLen);

// ����Э�鴦��///////////////////////////////////////////////////////////////
u8				HAL_UpdataCarState							(u8 *outBuf,	u16	*len);


/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
