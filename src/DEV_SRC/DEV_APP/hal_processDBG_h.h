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
*                                     	 hal_ProcessDBG_h.h
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessDBG_h.h
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

#ifndef  HAL_PROCESSDBG_H_H
#define  HAL_PROCESSDBG_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   HAL_PROCESSDBG_GLOBLAS
#define  HAL_PROCESSDBG_EXT
#else
#define  HAL_PROCESSDBG_EXT  extern
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

																													
/**********Interface Define***********/
#define	DEF_DBG_BUFSIZE			(200)


/**********Cmd Define***********/
//format="$S_ID:123456\r\n"

#define	DBG_CMD_HEAD_START							('$')								//
#define	DBG_CMD_HEAD_END								(':')								//
#define	DBG_CMD_CODE_SIZE								(20)

//////////////////////////////////////////////////////////////////////////////////
// �������ò�ѯָ��
#define	DBG_CMD_CODE_QCOMON							("Q_COMON")					//	��ѯϵͳͨ�ò���
#define	DBG_CMD_CODE_QSYSCFG						("Q_SYSCFG")				//	��ѯϵͳӦ�ò���
#define	DBG_CMD_CODE_QBAK   						("Q_BAK")				    //	��ѯ����������
#define	DBG_CMD_CODE_QCOMONEXT					("Q_COMONEXT")			//	��ѯRAMϵͳͨ�ò���
#define	DBG_CMD_CODE_QSYSCFGEXT 				("Q_SYSCFGEXT")			//	��ѯRAMϵͳӦ�ò���
#define	DBG_CMD_CODE_QSYSOBD						("Q_SYSOBD")				//	��ѯϵͳOBD����
#define	DBG_CMD_CODE_QVERB							("Q_VERB")					//  ��ѯͨѶϵͳ����汾��
#define	DBG_CMD_CODE_SVERB							("S_VERB")					//  ���ð汾��(����ֻ���������汾���޷��޸��ڲ��汾��)

#define	DBG_CMD_CODE_SETDFT							("S_DFT")						//  ���ò���ΪĬ��ֵ
#define	DBG_CMD_CODE_SETID							("S_ID")						//	�����豸ID��(6λ)
#define	DBG_CMD_CODE_QUEID							("Q_ID")						//  ��ѯ�豸ID��
#define	DBG_CMD_CODE_CV									("S_CV")						//	�Զ�У���ѹ
#define	DBG_CMD_CODE_SETAADC						("S_AADC")					//	�����豸AD��׼
#define	DBG_CMD_CODE_QUEAADC						("Q_AADC")					//  ��ѯ�豸AD��׼
#define	DBG_CMD_CODE_SETTIME						("S_TIME")					//	�����豸RTCʱ��(GMTʱ�伴Ϊ��׼ʱ��-8Сʱ)	
#define	DBG_CMD_CODE_QUETIME						("Q_TIME")					//	��ѯ�豸RTCʱ��
#define	DBG_CMD_CODE_SETAPN							("S_APN")						//	����APN
#define	DBG_CMD_CODE_QUEAPN							("Q_APN")						//	��ѯAPN
#define	DBG_CMD_CODE_SETIP							("S_IP")						//	����IP��ַ
#define	DBG_CMD_CODE_QUEIP							("Q_IP")						//	��ѯIP��ַ
#define	DBG_CMD_CODE_SETUPDATE					("S_UPDATE")				//  ���ÿ�ʼ��������
#define	DBG_CMD_CODE_SETUPDATE1					("S_UPDATE1")				//  ���ô���BKP����
#define	DBG_CMD_CODE_SETOTAP						("S_OTAP")					//  ���ÿ�ʼԶ������
#define	DBG_CMD_CODE_QUEQUEUE						("Q_QUE")						//	��ѯ����״̬
#define	DBG_CMD_CODE_SETQUEUE						("S_QUE")						//  ��ն�����Ϣ
#define	DBG_CMD_CODE_QUECARINFO					("Q_CARINFO")				//  ��ѯ��ϵ���ͷ�����Ʒ��
#define	DBG_CMD_CODE_SETCARINFO					("S_CARINFO")				//  ���ó�ϵ���ͷ�����Ʒ��
#define	DBG_CMD_CODE_QGPSRPTTIME				("Q_GPSRPT")				//	��ѯGPS�ɼ����ϱ�ʱ������λs
#define	DBG_CMD_CODE_SGPSRPTTIME				("S_GPSRPT")				//	����GPS�ɼ����ϱ�ʱ������λs
#define	DBG_CMD_CODE_QUEHEALTHRPT				("Q_HEALTHRPT")			//  ��ѯ�豸����������ʱ������λs
#define	DBG_CMD_CODE_SETHEALTHRPT				("S_HEALTHRPT")			//  �����豸����������ʱ������λs
#define	DBG_CMD_CODE_QUEIGOFFDELAY			("Q_IGOFFDELAY")		//  ��ѯ�豸Ϩ��״̬�ж���ʱʱ�䵥λs
#define	DBG_CMD_CODE_SETIGOFFDELAY			("S_IGOFFDELAY")		//  �����豸Ϩ��״̬�ж���ʱʱ�䵥λs
#define	DBG_CMD_CODE_QUEDSS							("Q_DSS")						//	��ѯ��ʻϰ�߲���
#define	DBG_CMD_CODE_SETDSS							("S_DSS")						//  ���ü�ʻϰ�߲���
#define	DBG_CMD_CODE_QUEACC							("Q_ACC")						//	��ѯ���ٶ��ж���ز���
#define	DBG_CMD_CODE_SETACC							("S_ACC")						//	���ü��ٶ��ж���ز���
#define	DBG_CMD_CODE_QUESLEEPTIME				("Q_SLEEPTIME")			//	��ѯ�豸˯��ʱ�䵥λs
#define	DBG_CMD_CODE_SETSLEEPTIME				("S_SLEEPTIME")			//	�����豸˯��ʱ�䵥λs
#define	DBG_CMD_CODE_QUEIGOFFMOVE				("Q_IGOFFMOVE")			//	��ѯϵͳϨ���ƶ���������
#define	DBG_CMD_CODE_SETIGOFFMOVE				("S_IGOFFMOVE")			//	����ϵͳϨ���ƶ���������
#define	DBG_CMD_CODE_SETDEVEN						("S_DEVEN")					//	����ģ��ʹ�ܿ���
#define	DBG_CMD_CODE_QUEDEVEN						("Q_DEVEN")					// 	��ѯģ��ʹ�ܿ���
#define	DBG_CMD_CHANGE_SETAPPIP					("S_APP2")					//	����Ӧ��IP
#define	DBG_CMD_CHANGE_QUEAPPIP					("Q_APP2")					// 	��ѯӦ��IP


#define	DBG_CMD_CODE_JUMP1						  ("JUMP1")					// 	��ת��1Ӧ��
//////////////////////////////////////////////////////////////////////////////////
// ����ָ��
#define	DBG_CMD_CODE_RESET							("C_RESET")					//	�豸�����λ
#define	DBG_CMD_CODE_SHUTNET						("C_SHUTNET")				//  ǿ�ƶϿ�����
#define	DBG_CMD_CODE_GSMBYPASS					("C_GSMBYPASS")			// 	GSM͸��ָ��
#define	DBG_CMD_CODE_BTBYPASS						("C_BTBYPASS")			// 	BT͸��ָ��
#define	DBG_CMD_CODE_TTSBYPASS					("C_TTSBYPASS")			// 	TTS͸��ָ��

//////////////////////////////////////////////////////////////////////////////////
// ��������Ϊ��ʱ�������״̬�洢��backup�Ĵ�����
#define	DBG_CMD_SHOW										("SHOW")						//  ��ʾ��Ϣͷ
#define	DBG_CMD_HIDE										("HIDE")						//  ������Ϣͷ
#define	DBG_CMD_SHOW_ALL								("SHOW_ALL")				//  ��ʾALL��Ϣ
#define	DBG_CMD_HIDE_ALL								("HIDE_ALL")				//	����ALL��Ϣ
#define	DBG_CMD_SHOW_RTC								("SHOW_RTC")				//  ��ʾRTC��Ϣ
#define	DBG_CMD_HIDE_RTC								("HIDE_RTC")				//  ����RTC��Ϣ
#define	DBG_CMD_SHOW_SYS								("SHOW_SYS")				//  ��ʾSYS��Ϣ
#define	DBG_CMD_HIDE_SYS								("HIDE_SYS")				//  ����SYS��Ϣ
#define	DBG_CMD_SHOW_GSM								("SHOW_GSM")				//  ��ʾGSM��Ϣ
#define	DBG_CMD_HIDE_GSM								("HIDE_GSM")				//  ����GSM��Ϣ
#define	DBG_CMD_SHOW_GSMSTA							("SHOW_GSMSTA")			//  ��ʾGSMSTA��Ϣ
#define	DBG_CMD_HIDE_GSMSTA							("HIDE_GSMSTA")			//  ����GSMSTA��Ϣ
#define	DBG_CMD_SHOW_GPS								("SHOW_GPS")				//  ��ʾGPS��Ϣ
#define	DBG_CMD_HIDE_GPS								("HIDE_GPS")				//  ����GPS��Ϣ
#define	DBG_CMD_SHOW_GPSSTA							("SHOW_GPSSTA")			//  ��ʾGPSSTA��Ϣ
#define	DBG_CMD_HIDE_GPSSTA							("HIDE_GPSSTA")			//  ����GPSSTA��Ϣ
#define	DBG_CMD_SHOW_CON								("SHOW_CON")				//  ��ʾCON��Ϣ
#define	DBG_CMD_HIDE_CON								("HIDE_CON")				//  ����CON��Ϣ
#define	DBG_CMD_SHOW_OS									("SHOW_OS")					//  ��ʾOS��Ϣ
#define	DBG_CMD_HIDE_OS									("HIDE_OS")					//  ����OS��Ϣ
#define	DBG_CMD_SHOW_OBD								("SHOW_OBD")				//  ��ʾOBD��Ϣ
#define	DBG_CMD_HIDE_OBD								("HIDE_OBD")				//  ����OBD��Ϣ
#define	DBG_CMD_SHOW_ADC								("SHOW_ADC")				//  ��ʾADC��Ϣ
#define	DBG_CMD_HIDE_ADC								("HIDE_ADC")				//  ����ADC��Ϣ
#define	DBG_CMD_SHOW_ACC								("SHOW_ACC")				//  ��ʾACC��Ϣ
#define	DBG_CMD_HIDE_ACC								("HIDE_ACC")				//  ����ACC��Ϣ
#define	DBG_CMD_SHOW_GPRS								("SHOW_GPRS")				//  ��ʾGPRS��Ϣ
#define	DBG_CMD_HIDE_GPRS								("HIDE_GPRS")				//  ����GPRS��Ϣ
#define	DBG_CMD_SHOW_SMS								("SHOW_SMS")				//  ��ʾSMS��Ϣ
#define	DBG_CMD_HIDE_SMS								("HIDE_SMS")				//  ����SMS��Ϣ
#define	DBG_CMD_SHOW_BT									("SHOW_BT")					//  ��ʾBT��Ϣ
#define	DBG_CMD_HIDE_BT									("HIDE_BT")					//  ����BT��Ϣ
#define	DBG_CMD_SHOW_EVENT							("SHOW_EVENT")			//  ��ʾEVENT��Ϣ
#define	DBG_CMD_HIDE_EVENT							("HIDE_EVENT")			//  ����EVENT��Ϣ
#define	DBG_CMD_SHOW_OTHER							("SHOW_OTHER")			//  ��ʾOTHER��Ϣ
#define	DBG_CMD_HIDE_OTHER							("HIDE_OTHER")			//  ����OTHER��Ϣ

//////////////////////////////////////////////////////////////////////////////////
// ��������
#define	DBG_CMD_TEST_TEST1							("T_TEST1")					//	����24Сʱ�������ݰ�
#define	DBG_CMD_TEST_TEST2							("T_TEST2")					//	�������ϱ������ݰ�
#define	DBG_CMD_TEST_TEST3							("T_TEST3")					//	����GPS�������ݰ�
#define	DBG_CMD_TEST_TEST4							("T_TEST4")					//	������̬�������ݰ�
#define	DBG_CMD_TEST_TEST5							("T_TEST5")					//	����GPS�յ㲹���������ݰ�
#define	DBG_CMD_TEST_TEST6							("T_TEST6")					//	������ѹ����
#define	DBG_CMD_TEST_TEST7							("T_TEST7")					//	������ֵ��ѯӦ�����ݰ�
#define	DBG_CMD_TEST_TJ									("T_TJ")						//	�ֶ�����OBD���
#define	DBG_CMD_TEST_TJ1								("T_TJ1")						//	�ֶ������������OBD���

#define	DBG_CMD_TEST_IG_ON							("T_IGON")					//	�ֶ��������
#define	DBG_CMD_TEST_IG_OFF							("T_IGOFF")					//	�ֶ�����Ϩ��
#define	DBG_CMD_TEST_ARM_SET						("T_ARMSET")				//	�ֶ��������
#define	DBG_CMD_TEST_ARM_RST						("T_ARMRST")				//	�ֶ���������
#define	DBG_CMD_TEST_FIND_CAR						("T_FINDCAR")				//	�ֶ�����Ѱ��
#define	DBG_CMD_TEST_READ_STA						("T_READSTA")				//	�ֶ�����״̬��ȡ
#define	DBG_CMD_TEST_TRUNK							("T_TRUNK")					//	�ֶ�������������

#define	DBG_CMD_TEST_FLASH							("T_FLASH")					//	FLASHоƬ����	
#define	DBG_CMD_TEST_ACC								("T_ACC")						//	ACC���ٶ�оƬ����
#define	DBG_CMD_TEST_GPS								("T_GPS")						//	GPSģ�����
#define	DBG_CMD_TEST_GSM								("T_GSM")						//	GSMģ�����
#define	DBG_CMD_TEST_CAN								("T_CAN")						//	GAN���߲���
#define	DBG_CMD_TEST_K									("T_K")							//	K���߲���
#define	DBG_CMD_TEST_UART1							("T_UART1")					//	�ⲿ���ڲ���
#define	DBG_CMD_TEST_CSQ								("T_CSQ")						//	GSM�ź�ǿ�Ȳ���

#define	DBG_CMD_PARA_SIZE								(200)	

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


// DBG�ӿ����ݽṹ����

typedef struct	tag_DBGPort_Def
{
		u8			rxFlag;
		u16			txlen;
		u16			rxlen;
		u8			rxdBuf[DEF_DBG_BUFSIZE];
			 
}DBGPort_TypeDef;	


typedef struct	tag_TESTStr_Def
{
		u8			busTestCode;			// ���߲����룬0-can���ԣ�1-k���ԣ�2-���ƴ��ڲ���
			 
}TESTStr_TypeDef;
	
	
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

HAL_PROCESSDBG_EXT		DBGPort_TypeDef				s_dbgPort;
HAL_PROCESSDBG_EXT		TESTStr_TypeDef				s_test;

			
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

void			HAL_DBGRES_IT_CON								(u8	newSta);
void			HAL_DBGTXD_IT_CON								(u8	newSta);
void			HAL_DBGUART_RxTxISRHandler 			(void);
void			HAL_DBG_Init 										(void);
void			HAL_DBGCmdProcess								(void);	

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
