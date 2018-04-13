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
*                                      BOARD SUPPORT PACKAGE
*
*                                     			bsp_gsm_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : bsp_gsm_c.c
* Version       : V1.07
* Programmer(s) : Felix
*********************************************************************************************************
*/

// ����˵����
// ��������֧��SIMCOM��˾��SIM800ģ�飨��Ҫע�⣺32M�汾TTS������ֻ��ȡһ��ֻ��64M�汾��������TTS��
// ���粿��֧�ֶ��TCP���ӣ���Ҫע���������һ��BSP_GPRSResetPro�����������б�������TCP���Ӷ����Ͽ���
// BT����ֻ֧�ֵ�һ���ӣ��ҵ�һ�������յ�"SIMCOMSPPFORAPP"���������ܲųɹ��������Ͽ�������

// �汾˵����
// V1.00:	�װ潨��
// V1.01: Ϊ��������������Ч�ʣ���ֹ������©�ؽ�GPRS����������ʾ��ʽ��Ϊ�ź���Ͷ�ݣ�����֮ǰ������Ͷ�ݷ�ʽ
// V1.02: �޸���Ϊ����hex�����а���ƥ���ַ������µ���ǰ�˳����Ӷ����ݽ��ղ�����
// V1.03: ���ӱ��������ȡATָ��弰��������
// V1.04: ������TTS����������������ʹ���忨Ҳ�ɲ���
// V1.05: �Ż���ʼ������(ȡ����SIM����⼰��صĳ�ʼ��ָ��)����������һ������SIM���ĳ�ʼ����������������ǰ����SIM״̬�ж�
//        ���ŷ���ǰ����SIM״̬�жϣ���������Ӧ������һ��"CONNECT FAIL"ƥ����
// V1.06: CNUM��ѯSIM����ʱ����+86����
// V1.07: ����GSMǿ�ƶ�������BSP_GPRSFroceBreakSet()
// V1.08:	��TCP�������ݻ���λ��������1������Ӧ�ù����н��գ�һ��Ӧ�������������ļ�������
// V1.09: �Ż�������ģʽ��TCP���պ���
// V1.10: �޸�TCP���ͺ����ڲ�ƥ��Ӧ��ATΪ����ģʽֻ���յ��ڴ��ַ������˳�������TTS�Զ��ϱ���URC����Ӱ�췢���жϣ�������Ϊ����;
// V1.11: ����������������CGATT�Ĳ�ѯ����
// V1.12: ��ʼ������������URC�ϴ�RING�ж�ʹ��(�ֲ�GPRS����������©����)--���ܵ��²�ͣ��ȡ��������,���߼���ȡ��!!!
// V1.13: �޸���������������ݺ�������ȡǰ�жϽ��ճ��ȴӶ�������ʱȷ���յ�����������
// V1.14: ����CPIN״̬��ѯ�Ĵ����ַ�ƥ��(�忨: CPIN: READY, ���忨: +CPIN: NOT INSERTED(��һ���ϵ�) or ERROR)
// V1.15: �޸�SIM״̬��ѯ���������ֻ�г�ʱ�Ž��д����ۼ�
// V1.16: �޸Ķ��Ŷ�ȡ��������Ӫ�̷���ʱ���Ϊ��������ʱ�䣻
// V1.17: ��IP��ַ��ȡ����ϲ���ͨ�ô�����
// V1.18: �޸�GSMģ�鿪��I/O�жϴ����жϿ�����ʱʱ��5s->10s����SIM800C���ܻ���ּ�ⳬʱ��


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GSM_GLOBLAS
#include  <stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	"cfg_h.h"
#include	"bsp_h.h"
#include	"bsp_gsm_h.h"
#include 	"ucos_ii.h"
#include	"main_h.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/********** �ڲ� GSM I/O ���ƽӿ� *************/

/* Enable the GSM device pow */
#define	IO_GSM_POW_EN()							(GPIO_SetBits		(DEF_GSM_POWEN_PORTS, DEF_GSM_POWEN_PINS))	
#define	IO_GSM_POW_DIS()						(GPIO_ResetBits	(DEF_GSM_POWEN_PORTS, DEF_GSM_POWEN_PINS))
														 	
/* Power on the GSM device */
#define	IO_GSM_POW_ON()							(GPIO_SetBits		(DEF_GSM_POWON_PORTS, DEF_GSM_POWON_PINS))
#define	IO_GSM_POW_OFF()						(GPIO_ResetBits	(DEF_GSM_POWON_PORTS, DEF_GSM_POWON_PINS))

/* Reset the GSM device  */
#define	IO_GSM_RES_ON()							(GPIO_SetBits		(DEF_GSM_POWRST_PORTS, DEF_GSM_POWRST_PINS))	
#define	IO_GSM_RES_OFF()						(GPIO_ResetBits	(DEF_GSM_POWRST_PORTS, DEF_GSM_POWRST_PINS))

/* GSM DTR control */
#define	IO_GSM_DTR_EN()							(GPIO_SetBits		(DEF_GSM_POWSLEEP_PORTS, DEF_GSM_POWSLEEP_PINS))	
#define	IO_GSM_DTR_DIS()						(GPIO_ResetBits	(DEF_GSM_POWSLEEP_PORTS, DEF_GSM_POWSLEEP_PINS))			

/* Read ring/power pin state */
#define	IO_GSM_RING_DET()						(GPIO_ReadInputDataBit (DEF_GSM_RING_PORTS, DEF_GSM_RING_PINS))
#define	IO_GSM_POW_DET()						(GPIO_ReadInputDataBit (DEF_GSM_POWDET_PORTS, DEF_GSM_POWDET_PINS))

/* Enable/Disable GSM rxd interrupt  */
#define	GSM_ENABLE_RES_IT()					(BSP_RS232_RxIntEn	(DEF_GSM_UART))		// include at bsp.h
#define GSM_DISABLE_RES_IT()				(BSP_RS232_RxIntDis	(DEF_GSM_UART))		// include at bsp.h

/* Enable/Disable GSM txd interrupt  */
#define	GSM_ENABLE_SEND_IT()				(BSP_RS232_TxIntEn	(DEF_GSM_UART))		// include at bsp.h
#define	GSM_DISABLE_SEND_IT()				(BSP_RS232_TxIntDis	(DEF_GSM_UART))		// include at bsp.h

/********** �ڲ�ʹ�ú����ӿ� *************/

//(BSP_USART_Init(x,y,z))		// include at	bsp.h
//(StringSearch(x,y,z,k))		// include at	bsp.h					
//(OSTimeDly(x))						// include at	ucos_ii.h	
	

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

/* SIM800 Init cmd */
																									/* BASE INIT CMD" */
uc8 			AT_ATE0[] 				= "ATE0\r";           // Disable ECHO
uc8				AT_IPR[]					=	"AT+IPR=115200\r";	// Set 115200
uc8				AT_IFC[]					=	"AT+IFC=0,0\r";			// Disable the flow control
uc8				AT_CFUN1[]				=	"AT+CFUN=1\r";			// All function mode disable low power mode
uc8				AT_CFUN0[]				=	"AT+CFUN=0\r";			// Enable low power mode
uc8				AT_CFUN11[]				=	"AT+CFUN=1,1\r";		// Module softrest

																									/* SMS Init CMD */
uc8 			AT_CMGF[] 				= "AT+CMGF=1\r";      // Send SMS in text mode. 
uc8				AT_CSCS_GSM[]			=	"AT+CSCS=\"GSM\"\r";	// Set encode is GSM
uc8				AT_CSCS_UCS2[]		=	"AT+CSCS=\"UCS2\"\r";	// Set encode is UCS2
uc8				AT_CNMI[]					= "AT+CNMI=2,1\r";		// Auto remind on receive the SMS(Save at sim card)
uc8				AT_CPMS[]					= "AT+CPMS=\"SM\",\"SM\",\"SM\"\r";	// Set sms save at sim card
uc8				AT_CSCLK_1[]			= "AT+CSCLK=1\r";			// Enable sleep mode(DTR:1 goto sleep mode/DTR:0 exit sleep mode)
uc8				AT_CSCLK_0[]			= "AT+CSCLK=0\r";			// Disable sleep mode
uc8				AT_CTTSPARAM[]		=	"AT+CTTSPARAM=50,0,50,60,0\r";	// ����TTS��������ʽ��AT+CTTSPARAM=<Vol>,<Mode>,<Pitch>,<Spped>,<Channel>��
uc8				AT_CFGRI_1[]			=	"AT+CFGRI=1\r";			// ����URC�ϴ�ʱ�����жϲ���
uc8				AT_W[]						= "AT&W\r";						// Save parameter
																						

/*------------------------------------------------------------------------*/
uc8 *pATcommand[] =
{
		AT_ATE0,
    AT_IPR,
    AT_IFC,   	
		AT_CFUN1,
		AT_CSCLK_0,
		AT_CTTSPARAM,
		//AT_CFGRI_1,
		AT_W,			
};
/*------------------------------------------------------------------------*/
uc16 cmdTimeout[] =
{
    GSMTime2s,
    GSMTime2s,    
    GSMTime2s,
		GSMTime2s,
		GSMTime2s,
		//GSMTime2s,
		GSMTime2s,
};

/******************************************************************************************************/
/* AT Query cmd */
/******************************************************************************************************/
uc8				AT_CIMI[] 				= "AT+CIMI\r";				// Query IMSI code(ȫ���ƶ��豸ǩ��ʶ����)
uc8				AT_CGSN[] 				= "AT+CGSN\r";				// Query IMEI code(ȫ���ƶ��豸ʶ����,��ͬ��AT+GSN)
uc8				AT_GSN[] 					= "AT+GSN\r";					// Query IMEI code(��ͬ��AT+CGSN)

uc8				AT_CSQ[] 					= "AT+CSQ\r";					// Query GSM signal strength
uc8				AT_CPIN[] 				= "AT+CPIN?\r";				// Query SIM card state
uc8				AT_COPS[] 				= "AT+COPS?\r";				// Query Operator Selection(��ǰ����Ӫ��)
uc8				AT_CREG[] 				= "AT+CREG?\r";				// Query GSM regist state
uc8				AT_CGREG[] 				= "AT+CGREG?\r";			// Query GPRS regist state(������AT+CGATT=1��,����0,1)
uc8				AT_CGATT[]				=	"AT+CGATT?\r";			// Query attach or detach from GPRS(��ѯ�Ƿ��Ż���뵽GPRS����)
uc8				AT_CLCC[]					=	"AT+CLCC\r";				// Query Call state
uc8				AT_CNUM[]					=	"AT+CNUM\r";				// ��ѯ��������(ֻ��SIM��д��ú����ſɶ���,���򷵻ؿ��ַ���)

/******************************************************************************************************/
/* GPRS Init And Connect cmd */
/******************************************************************************************************/
uc8				AT_CGATT_0[] 			= "AT+CGATT=0\r";			// Mode attch the gprs net
uc8				AT_CGATT_1[] 			= "AT+CGATT=1\r";			// Mode attch the gprs net

uc8				AT_CIPMUX_1[]			= "AT+CIPMUX=1\r";		// ʹ�ܶ�·����ģʽ��0~5ͨ��,���6·����,��·ģʽ��ֹ͸��ģʽ��
																									// ��ֻ�������1���������ˣ�5���ͻ��ˡ���·ģʽʹ�ܺ�͸������ָ��CIPMODEָ����ӦΪERROR��
uc8				AT_CIPMUX_0[]			= "AT+CIPMUX=0\r";		// ��ֹ��·����ģʽ

uc8				AT_CIPMODE_0[]		= "AT+CIPMODE=1\r";		// ���������շ�͸��ģʽ(ע�⣺���ʹ�ܶ�����ģʽ���ָ���κ����ö�����"ERROR")
uc8				AT_CIPMODE_1[]		= "AT+CIPMODE=0\r";		// ���������շ�����ģʽ(���ʹ�ܶ�����ģʽ����ʹ�ø�ָ��)

uc8				AT_CSTT_H[]				=	"AT+CSTT=\"";				// ����APN,USER,PASS
uc8				AT_CIICR[]				=	"AT+CIICR\r";				// ����������·��GPRS��
uc8				AT_CIFSR[]				= "AT+CIFSR\r";				// ��ñ���IP��ַ
		
/******************************************************************************************************/
/* TCP Init And Connect cmd */
/******************************************************************************************************/

uc8				AT_CIPRXGET_1[]		=	"AT+CIPRXGET=1\r";	// �����������ݶ�ȡ��ʽΪ�ֶ���ʽ,�������ã�AT+CIPRXGET=2,id,1000����ȡ
uc8				AT_CIPHEAD_1[]		=	"AT+CIPHEAD=1\r";		// �����յ���������ͷ��ʽ(+CIPRXGET: )
uc8				AT_CIPSTART_H[]		=	"AT+CIPSTART=";			// ����TCP���ӣ���ʽ��AT+CIPSTART=0,"TCP","218.249.201.35","13000"��
uc8				AT_CIPCLOSE_H[]		=	"AT+CIPCLOSE=";			// �ر�TCP����
uc8				AT_CIPSHUT[]			=	"AT+CIPSHUT\r";			// �رյ�ǰGPRS�����Ļ���
uc8				AT_CIPSEND_H[]		=	"AT+CIPSEND=";			// �������ݣ���ʽ��AT+CIPSEND=1,8��
uc8				AT_CIPRXGET_H[]		=	"AT+CIPRXGET=2,";		// �ֶ���ȡ���յ����������ݣ���ʽ��AT+CIPRXGET=2,1,1000��
uc8				AT_CIPRXGET4_H[]	=	"AT+CIPRXGET=4,";		// �ֶ���ѯ���յ������ݳ���
uc8				AT_CIPSENDQ[]			=	"AT+CIPSEND?\r";		// ���ڲ�ѯ��ǰ�����ڷ��͵����ݳ���


/******************************************************************************************************/
/* Call and SMS cmd */
/******************************************************************************************************/

uc8				AT_ATD_H[]	 			= "ATD";							// Call dail
uc8				AT_ATA[]		 			= "ATA\r";						// Call ack
uc8				AT_ATH[]		 			= "ATH\r";						// Call hung up

uc8				AT_CMGR_H[] 			= "AT+CMGR=";					// Read SMS
uc8				AT_CMGD_H[]				= "AT+CMGD=";					// Delete SMS
uc8				AT_CMGF_0[]				=	"AT+CMGF=0\r";			// Set PDU mode
uc8				AT_CMGF_1[]				=	"AT+CMGF=1\r";			// Set TEXT mode
uc8				AT_CSCA_H[] 			= "AT+CSCA=\"";				// Set SMS center code
uc8				AT_QCSCA[]				=	"AT+CSCA?\r\n";			// Queue SMS center code
uc8				AT_CMGS_H[] 			= "AT+CMGS=";					// Send SMS

uc8				AT_CPBS_ON[] 			= "AT+CPBS=\"ON\"\r";	// ʹ�ܿ��Դ�SIM���绰���ж�ȡ��������
uc8				AT_CPBW_H[] 			= "AT+CPBW=1,\"";			// д�뱾������(д����ʹ��CNUM��ȡ��������)		

uc8				PDUfixP[]					=	"1100";							// PDU�ֻ��Ź̶��ֽ�	
uc8				PDUfixD[]					=	"000800";						// PDU�������ݹ̶��ֽ�
	
uc8				PDUinter[]				=	"91";								// ���ʻ���־
uc8				PDUbphone[]				=	"91";								// ���ֻ����ʹ���
uc8				PDUsphone[]				=	"81";								// ��С��ͨ���ʹ���



/******************************************************************************************************/
/* BT Init And Connect cmd */
/******************************************************************************************************/

// ģ����Ϊ��������ʱ���ͻ������ӳɹ���һ�����ݱ����ǡ�SIMCOMSPPFORAPP������ʹģ���л���APPģʽ
uc8				AT_BTPOWER_1[]		=	"AT+BTPOWER=1\r";		// ������ģ���Դ
uc8				AT_BTPOWER_0[]		=	"AT+BTPOWER=0\r";		// �ر�����ģ���Դ
uc8				AT_BTSTATUS[]			=	"AT+BTSTATUS?\r";		// ��ѯ����״̬��Ϣ
uc8				AT_BTHOST_H[]			=	"AT+BTHOST=";				// ����������
uc8				AT_BTUNPAIR[]			=	"AT+BTUNPAIR=0\r";	// ɾ������������Ϣ
uc8				AT_BTVIS_1[]			=	"AT+BTVIS=1\r";			// �����ɼ�ʹ��
uc8				AT_BTVIS_0[]			=	"AT+BTVIS=0\r";			// �����ɼ���ֹ
uc8				AT_BTPAIR_1[]			=	"AT+BTPAIR=1,1\r";	// ��Ӧ�����豸����ƥ�䷽ʽ
uc8				AT_BTPAIR_2_H[]		=	"AT+BTPAIR=2,";			// ��Ӧ�����豸Passkeyƥ�䷽ʽ
uc8				AT_BTACPT_1[]			=	"AT+BTACPT=1\r";		// ���������豸����������
uc8				AT_BTACPT_0[]			=	"AT+BTACPT=0\r";		// �ܾ������豸����������
uc8				AT_BTSPPGET_0[]		= "AT+BTSPPGET=0\r";	// ��������ģʽ-�Զ�
uc8				AT_BTSPPGET_1[]		= "AT+BTSPPGET=1\r";	// ��������ģʽ-�ֶ�
uc8				AT_BTSPPGET_2[]		=	"AT+BTSPPGET=2\r";	// �ֶ�ģʽ�½����յ������ݳ���
uc8				AT_BTSPPGET_3_H[] = "AT+BTSPPGET=3,";		// �ֶ�ģʽ�»�ȡ���յ������ݣ���ʽ��AT+BTSPPGET=3,17 17:���ݳ��ȣ�
uc8				AT_BTSPPCFG[]			=	"AT+BTSPPCFG?\r";		// ��ȡ��ǰ�շ�����״̬����ʽ��+BTSPPCFG: S,1,0 S:������,C:�ͻ���;1:����ID;0:ATģʽ,1:APPģʽ��
uc8				AT_BTSPPSEND_H[]	= "AT+BTSPPSEND=";		// �����ȷ�������


/******************************************************************************************************/
/* FS Init And APP cmd */
/******************************************************************************************************/

uc8				AT_FSDRIVE_0[]		=	"AT+FSDRIVE=0\r";		// ��ȡ���ش����̷���+FSDRIVE: C��
uc8				AT_FSDRIVE_1[]		= "AT+FSDRIVE=1\r";		// ��ȡSC�������̷���+FSDRIVE: D~G��
uc8				AT_FSLS_H[]				=	"AT+FSLS=";					// ��ȡ�����ļ���Ϣ
uc8				AT_FSLS_C[]				=	"AT+FSLS=C:\\r";		// ��ȡ����C�����ļ�����Ϣ
uc8				AT_FSLS_USER[]		=	"AT+FSLS=C:\\User\\r";		// ��ȡ����User���ļ�����Ϣ
uc8				AT_FSMKDIR_H[]		=	"AT+FSMKDIR=";			// ��Ŀ¼�´����ļ��У���ʽ��AT+FSMKDIR=C:\User\Log��
uc8				AT_FSCREATE_H[]		=	"AT+FSCREATE=";			// ��Ŀ¼�´����ļ�����ʽ��AT+FSCREATE=C:\User\Log\Log.txt��
uc8				AT_FSFLSIZE_H[]		=	"AT+FSFLSIZE=";			// ��ѯ�ļ���С����ʽ��AT+FSFLSIZE=C:\User\Log\Log.txt��
uc8				AT_FSWRITE_H[]		=	"AT+FSWRITE=";			// д�ļ�����ʽ��AT+FSWRITE=C:\User\Log\Log.txt,0,23,10  
                                                  // 0:���ļ���ͷ���룬�����Ǻ�������� 1�����ļ���βд��;23:����;10:д�볬ʱ�룩
uc8				AT_FSREAD_H[]			= "AT+FSREAD=";				// ���ļ�����ʽ��AT+FSREAD=C:\User\Log\Log.txt,0,100,0                                                  
																									// 0:���ļ�ͷ���� 1:��posi��; 100:����; 0:���ļ���ʼ��ַposition��
uc8				AT_FSMEM[]				=	"AT+FSMEM\r";				// ��ѯ�����̷��ռ�ռ���������ʽ��+FSMEM: C:76288byte��	
uc8				AT_FSRENAME_H[]		=	"AT+FSRENAME=";			// �������ļ�����ʽ��AT+FSRENAME=C:\User\Log\Log.txt,C:\User\Log\Log1.txt��		
uc8				AT_FSDEL_H[]			=	"AT+FSDEL=";				// ɾ���ļ��У���ʽ��AT_FSDEL=C:\User\Log\Log.txt��


/******************************************************************************************************/
/* TTS Init And APP cmd */
/******************************************************************************************************/

uc8				AT_CTTS_0[]				=	"AT+CTTS=0\r";			// ֹͣ��ǰ��������
uc8				AT_CTTS_1_H[]			=	"AT+CTTS=1,";				// UCS2��ʽ���ŵ�ǰ����
uc8				AT_CTTS_2_H[]			= "AT+CTTS=2,";				// ASCII��ʽ���ŵ�ǰ��������ʽ��AT+CTTS=1,��ӭʹ�ã�
uc8				AT_CTTSPARAM_H[]	=	"AT+CTTSPARAM=";		// ����TTS��������ʽ��AT+CTTSPARAM=<Vol>,<Mode>,<Pitch>,<Spped>,<Channel>��
uc8				AT_CTTSRING_0[]		= "AT+CTTSRING=0\r";	// ����ʱ��ֹ��������
uc8				AT_CTTSRING_1[]		= "AT+CTTSRING=1\r";	// ����ʱ���Բ�������

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
extern		OS_EVENT					*GSMMutexSem;					// �ײ㻥���ź���
extern		OS_EVENT					*GSMSmsSem;						// �յ������ź���
extern		OS_EVENT					*GSMGprsDataSem;			// GPRS���������ź���
extern		OS_EVENT					*GSMBtDataMbox;				// ����������������
extern		OS_EVENT					*GSMBtAppMbox;				// ������ȡҵ��״̬����

static		vu8								GSMMutexFlag	=1;			// �����������OS״̬���ɸñ���ʵ�ֻ������
static		vu32							ringCheckConuter =0;	// ring�¼���ʱ��ʱ��
static		vu8								ringWUpFlag	=0;				// �ı�־��˯�������á�1����Ring�ж�����Ϊ��2�������豸��Ring����
static		vu8								tcpReadMuxtex	=0;			// Ϊ��ʵ�������ȡ��ģ���Զ��ϴ���URC�ַ���ʵ�ֻ��
																									//(�����ȡ��Ϊ���������ļ������ܻ����һЩ�̻��ڳ����е�URC��
																									// ʹģ��������Ϊ�ǵ�ǰģ���յ���URC�������ϱ�״̬�Ĵ���)

BTMbox_Typedef							s_MboxBT;							// ��������������������ڴ����������ݼ�ҵ��

static		vu8								ringCheckStep=0;			// Ring���׶α���
static		vu32							ringCheckCounter=0;		// Ring��ⶨʱ��



/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static		void			gsmDelayMS								(u16	nms);

static		void			GSMGotoSleep		 					(void);
static		void			GSMStartWork							(void);

/* Interrupt server */
static		void  		GSMRS232_RxHandler 				(u8 rx_data);
static		void			GSMRS232_TxHandler 				(void);

static		void			Start2doSendAT						(void);
static		void	 		DoSendProcess 						(void);

static		u8				CheckResultCode						(u8	checkMode,	u32 TimeOut);
static 		u16				PDUMakeUpData							(u8	 mode,	u8	*pSmsCenter,	u8 *pPhone,	u8	*pData,		u16	 DataLen, u8	*pDst);
static		u16 			GSMInvertNumbers					(u8* pSrc, u8* pDst, u16 nSrcLength);
static		void			GSMRingPinInit 						(void);

static		u16				ChangeUSC22ASCII 					(u8 *pUSC2,		u16 USC2len,	u8 *pASCII);
static		u16				ChangeASCII2USC2 					(u8 *pASCII,	u16	ASCIIlen,	u8 *pUSC2);

static		void			GSMFeedWDGProcess					(void);		// ��ʼ����������Ҫ���õ�ι������


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                         GSM FEED WDG
*********************************************************************************************************
*********************************************************************************************************
*/
static	void	GSMFeedWDGProcess (void)
{
		#if(OS_SYSWDG_EN > 0)
		IWDG_ReloadCounter();		// ι��
		#endif
}


/*
*********************************************************************************************************
*                            					GSMģ�鹤�ߺ���
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : ChangeUSC22ASCII
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	u16	ChangeUSC22ASCII (u8 *pUSC2,	u16 USC2len,	u8 *pASCII)
{
		u16	i=0,j=0;
		u32 val=0;
		u8 tmpBuf[5]="";

		if((USC2len % 4) != 0)
				return	0xffff;
		for(i=0,j=0;	i<USC2len;	i +=4,j++)
		{
				val =0;
				memset(tmpBuf, '\0', sizeof(tmpBuf));
				memcpy(tmpBuf,	(pUSC2 + i),	4);
				sscanf((const char *)&tmpBuf,	"%x",	&val);
				*(pASCII + j) = (u8)(val & 0x000000ff);	// ȡ����ֽ�
		}
		*(pASCII + j) = '\0';
		return	j;
}
/*
*********************************************************************************************************
* Function Name  : ChangeASCII2USC2
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	u16	ChangeASCII2USC2 (u8 *pASCII,	u16	ASCIIlen,	u8 *pUSC2)
{
		u16	i=0;
		u8 tmpBuf[5]="0000",	valBuf[5]="";
		for(i=0;	i<ASCIIlen;	i++)
		{
				memset(valBuf,	'\0',		sizeof(valBuf));
				memset(tmpBuf,	'0',		strlen((const char *)tmpBuf));
				sprintf((char *)valBuf,	"%02X",	*(pASCII + i));
				memcpy(&tmpBuf[2],	valBuf,	2);
				memcpy((pUSC2 + i * 4),	tmpBuf,	4);
		}
		*(pUSC2 + i * 4) = '\0';
		return	(i * 4);
}

/*
*********************************************************************************************************
*                            					GSMģ��I/O������������(�ṩ��/�ػ�/��λ��)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : gsmDelayMS
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	gsmDelayMS	(u16	nms)
{
		if(OSRunning > 0)
				OSTimeDly(nms);
		else
				SoftDelayMS(nms);
}
/*
*********************************************************************************************************
* Function Name  : BSP_GSM_EXIT_SLEEPMODE
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_EXIT_SLEEPMODE	(void)
{
		// �˳�˯��ģʽ
		IO_GSM_DTR_DIS();
		gsmDelayMS(100);
}
/*
*********************************************************************************************************
* Function Name  : BSP_GSM_MODULE_DOWN
* Description    : power down the GSM module power supply
* Input          : newsta = ON or OFF 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSMMODULE_POWDOWN	(void)
{
		IO_GSM_POW_DIS();
		IO_GSM_POW_OFF();
}
/*
*********************************************************************************************************
* Function Name  : BSP_GSM_POWER_CON
* Description    : power on or down the GSM module 
* Input          : newsta = ON or OFF 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_POWER_CON	(void)
{
		IO_GSM_POW_ON();
		/* Delay 2S */
		gsmDelayMS(2000);
		IO_GSM_POW_OFF();
		/* Delay 1S */
		gsmDelayMS(1000);
}
/*
*********************************************************************************************************
* Function Name  : BSP_GSM_SOFT_RESET
* Description    : reset the gsm module    
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void 	BSP_GSM_SOFT_RESET	(void)
{
		IO_GSM_RES_ON();
		// Delay 2S 
		gsmDelayMS(2000);
		IO_GSM_RES_OFF();
		// Delay 1S 
		gsmDelayMS(1000);
}

/*
*********************************************************************************************************
* Function Name  : BSP_GSM_POW_RESET
* Description    : Hard reset the gsm module				      
* Input          : �豸��λʱ����
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_POW_RESET	(u32	dlayTime)
{	
		//IO_GSM_RES_ON();		// RESET �ܽ��п��ܻ�©�絼�·ŵ粻�ɾ
													//(�����ֹRST�ܽſ���,���������������ʼ���ַ������³�ʼ��������ճ�ʼ���ַ�������ʧ��)		
		IO_GSM_POW_OFF();
		IO_GSM_POW_DIS();		
    if (dlayTime <= 6000u)
    {
        dlayTime = 6000u;   //��ֹ�ϵ�ʱ�����GSMģ���޷���λ 20160104
    }
    else
    {
    }
		gsmDelayMS(dlayTime);	
		IO_GSM_RES_OFF();		
		IO_GSM_POW_EN();
		gsmDelayMS(1000);
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMHardWarePowShut() 
* Description    : GSMģ��Ӳ���ϵ紦��        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSMHardWarePowShut(void)
{
		memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	// ��������		
		s_GSMcomm.SetupPhase 		= PowerOff;
		s_GSMcomm.GSMComStatus	=	GSM_POWOFF;
		BSP_GSM_ReleaseFlag(DEF_ALL_FLAG);
		IO_GSM_POW_DIS();															// ģ��ϵ� 
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMHardWareReset() 
* Description    : �������λ��Чʱִ��Ӳ����λ        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMHardWareReset(u32 dlayTime)
{
		memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	// ��������		
		s_GSMcomm.SetupPhase 		= PowerOff;
		s_GSMcomm.GSMComStatus	=	GSM_POWOFF;
		BSP_GSM_ReleaseFlag(DEF_ALL_FLAG);
		BSP_GSM_POW_RESET(dlayTime);
		return	0;
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ReleaseFlag()	
* Description    :  ������λ�����ģ��״̬         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_ReleaseFlag (u8	flagType)
{
		if(flagType == DEF_ALL_FLAG)
				s_GSMcomm.CommandFlag &= ~AllF;	
		else if(flagType == DEF_RING_FLAG)
				s_GSMcomm.CommandFlag &= ~RingComeF;	
		else if(flagType == DEF_RINGCHECK_FLAG)	
				s_GSMcomm.CommandFlag &= ~RingCheckF;
		else if(flagType == DEF_CALLDOING_FLAG)	
				s_GSMcomm.CommandFlag &= ~CallDoingF;
		else if(flagType == DEF_SMSDOING_FLAG)	
				s_GSMcomm.CommandFlag &= ~SmsDoingF;
		else if(flagType == DEF_GDATA_FLAG)	
				s_GSMcomm.CommandFlag &= ~GprsDoingF;				
		else if(flagType == DEF_TTSPLAY_FLAG)	
				s_GSMcomm.CommandFlag &= ~TTSPlayF;
		else if(flagType == DEF_BTDING_FLAG)	
				s_GSMcomm.CommandFlag &= ~BTDoingF;
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_SetFlag()	
* Description    :  ������λ������ģ��״̬          
* Input          : 
* Output         : 	 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_SetFlag (u8	flagType)
{
		if(flagType == DEF_ALL_FLAG)
		{
				;		// ��������
		}
		else if(flagType == DEF_RING_FLAG)
				s_GSMcomm.CommandFlag |= RingComeF;	
		else if(flagType == DEF_RINGCHECK_FLAG)	
				s_GSMcomm.CommandFlag |= RingCheckF;
		else if(flagType == DEF_CALLDOING_FLAG)	
				s_GSMcomm.CommandFlag |= CallDoingF;
		else if(flagType == DEF_SMSDOING_FLAG)	
				s_GSMcomm.CommandFlag |= SmsDoingF;
		else if(flagType == DEF_GDATA_FLAG)	
				s_GSMcomm.CommandFlag |= GprsDoingF;
		else if(flagType == DEF_TTSPLAY_FLAG)	
				s_GSMcomm.CommandFlag |= TTSPlayF;
		else if(flagType == DEF_BTDING_FLAG)	
				s_GSMcomm.CommandFlag |= BTDoingF;
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_GetFlag()	
* Description    :  ������λ���ж�ģ��״̬       
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
u8	BSP_GSM_GetFlag (u8	flagType)
{
		if(flagType	==	DEF_RING_FLAG)
		{
				if(s_GSMcomm.CommandFlag & RingComeF)
						return	1;
		}
		else	if(flagType	==	DEF_RINGCHECK_FLAG)
		{
				if(s_GSMcomm.CommandFlag & RingCheckF)
						return	1;
		}
		else	if(flagType	==	DEF_CALLDOING_FLAG)
		{
				if(s_GSMcomm.CommandFlag & CallDoingF)
						return	1;	
		}
		else	if(flagType	==	DEF_SMSDOING_FLAG)
		{
				if(s_GSMcomm.CommandFlag & SmsDoingF)
						return	1;	
		}
		else	if(flagType	==	DEF_GDATA_FLAG)
		{
				if(s_GSMcomm.CommandFlag & GprsDoingF)
						return	1;	
		}
		else	if(flagType	==	DEF_TTSPLAY_FLAG)
		{
				if(s_GSMcomm.CommandFlag & TTSPlayF)
						return	1;	
		}
		else	if(flagType	==	DEF_BTDING_FLAG)
		{
				if(s_GSMcomm.CommandFlag & BTDoingF)
						return	1;	
		}
		return	0;		
}
/*
*********************************************************************************************************
*                            					GSM�ַ��������ߺ���
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : GSMInvertNumbers
* Description    : ����˳����ַ���ת��Ϊ�����ߵ����ַ�����������Ϊ��������'F'�ճ�ż�� 
*									 �磺"8613851872468" --> "683158812764F8"
*									 ����: pSrc - Դ�ַ���ָ��
*									       nSrcLength - Դ�ַ�������
*									 ���: pDst - Ŀ���ַ���ָ��
*									 ����: Ŀ���ַ�������
* Input          :  
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	u16 GSMInvertNumbers (u8* pSrc, u8* pDst, u16 nSrcLength)
{
		u16 nDstLength,i;   			// Ŀ���ַ�������
		u8  ch;    								// ������ʱ����һ���ַ�
	  u8* ptSrc = pSrc;
		u8* ptDst = pDst;
	
		nDstLength = nSrcLength;
		for(i=0; i<nSrcLength;	i+=2)
		{
				ch = *ptSrc++;   			// �����ȳ��ֵ��ַ�
				*ptDst++ = *ptSrc++; 	// ���ƺ���ֵ��ַ�
				*ptDst++ = ch;   			// �����ȳ��ֵ���
		}
		if(nSrcLength & 1)
		{
				*(ptDst-2) = 'F'; 		// ��'F'
				nDstLength++;  	 			// Ŀ�괮�����1
		}
		*ptDst = '\0';
		return nDstLength;
}
/*
*********************************************************************************************************
* Function Name  : PDUMakeUpData
* Description    : ��������PDUģʽ��������س�����ֵ 
*									 ����: mode 			- DEF_COM_PHONE / DEF_PHS_PHONE
*												 pSmsCenter - �������ĺ���ָ��
*												 pPhone			-	Ŀ���ֻ�����ָ��
*												 pData			-	��Ҫ���͵�����ָ��
*												 pDst				-	Ŀ�Ĵ洢��ָ��			
* Return         : �������Ϣ�ܳ���(�ĳ��Ȳ������������ĵ�ַ�����ֽ�) ������0x1a 
*********************************************************************************************************
*/
static  u16 PDUMakeUpData (u8	mode,	u8	*pSmsCenter,	u8 *pPhone,	u8	*pData,	u16	 DataLen, u8	*pDst)
{
		u8	tmpBuf[50]="",tmpDuf[50]="",tmpPhone[50]="";
		u16 j=0,i=0,len=0,tmplen=0;
		
		memset(tmpBuf,	'\0',	sizeof(tmpBuf));
		memset(tmpDuf,	'\0',	sizeof(tmpDuf));
		memset(tmpPhone,	'\0',	sizeof(tmpPhone));		
		
		// �������Ĳ��ִ��
		if(strlen((const char *)pSmsCenter) == 0)
		{
				*(pDst+j) = '0';	j++;			   		// copy "00"
				*(pDst+j) = '0';	j++;
		}
		else
		{
				strcpy((char	*)tmpPhone,	(char	*)pSmsCenter);				
				tmplen = GSMInvertNumbers(tmpPhone,	tmpBuf,	 strlen((const char *)tmpPhone));	
				tmplen = (strlen((const char *)PDUinter)+ tmplen) /2;
				sprintf((char *)tmpDuf,"%02X",	tmplen);
				for(i=0;	tmpDuf[i]!='\0';	i++,j++)
				{
						*(pDst+j) = tmpDuf[i];			 	// copy sms center len
				}
				for(i=0;	PDUinter[i]!='\0';	i++,j++)
				{
						*(pDst+j) = PDUinter[i];		 	// copy "91"
				}
				for(i=0;	tmpBuf[i]!='\0';	i++,j++)
				{
						*(pDst+j)	=	tmpBuf[i];		   	// copy smscenter number
				}
		}
		// �ֻ����벿�ִ��
		for(i=0;	PDUfixP[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j)	=	PDUfixP[i];				 		// copy "1100"
		}
		strcpy((char	*)tmpPhone,	(char	*)pPhone);
		GSMInvertNumbers(tmpPhone,	tmpBuf,	 strlen((const char *)tmpPhone));	
		tmplen = strlen((const char *)tmpPhone);
		sprintf((char *)tmpDuf,	"%02X",	tmplen);
		for(i=0;	tmpDuf[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j) = tmpDuf[i];				 		// copy phone number len
		}
		if(mode == DEF_COM_PHONE)				 
		{
				//	��ͨ�ֻ�
				for(i=0;	PDUbphone[i]!='\0';	i++,j++,len++)
				{
						*(pDst+j) = PDUbphone[i];		 	// copy "91"
				}
		}
		else
		{
				//	С��ͨ
				for(i=0;	PDUsphone[i]!='\0';	i++,j++,len++)
				{
						*(pDst+j) = PDUsphone[i];		 	// copy "81"
				}
		}
		for(i=0;	tmpBuf[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j)	=	tmpBuf[i];				 		// copy "phone number"
		}
		for(i=0;	PDUfixD[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j)	=	PDUfixD[i];				 		// copy "000800"
		}
		// �ֻ���Ϣ���ִ��
		tmplen = strlen((const char *)pData)/2;
		sprintf((char *)tmpDuf,	"%02X",	tmplen);
		for(i=0;	tmpDuf[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j)	=	tmpDuf[i];				 		// copy data len
		}
		for(i=0;	i<DataLen;	i++,j++,len++)
		{
				*(pDst+j)	=	pData[i];				   		// copy data
		}
		*(pDst+j) = 0x1a;								 			// sms data end
		*(pDst+j+1) = '\0';
	
		return len/2;
}
/*
*********************************************************************************************************
*                                           CheckResultCode()
*
* Description : ����ƥ�����������ATָ���������ģʽ��DEF_NULL_MODE	DEF_ONLYOK_MODE	DEF_SPECIAL_MODE DEF_COMMON_MODE
*
* Argument(s) : none.			 
*
* Return(s)   : none.	
*
* Caller(s)   : 
*
* Note(s)     : 
*********************************************************************************************************
*/
static	u8 CheckResultCode(u8	checkMode,	u32 TimeOut)
{
		if(checkMode ==  DEF_NULL_MODE)
		{
        ;											 				// �ȴ���ʱ�˳�(����ָ��͸��)
		}
		if((checkMode	==	DEF_ONLYOK_MODE) || (checkMode	==	DEF_COMMON_MODE))
		{    
				if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\nOK",s_GSMcomm.ps_commu->RCNT,4)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = CommandRecOk;
						s_GSMcomm.Timer = 0;
		        return CommandRecOk;
		    }
				else
				{
		        // �ȴ���ʱ
		        ;													// ������Ҫȡ�����벢������AT��AT+CSQ	AT+CLCC��
		    }
				
		}
		if((checkMode	==	DEF_SPECIAL_MODE) || (checkMode	==	DEF_COMMON_MODE))
		{
				// ����ģʽ�����Ŀ
				if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\nERROR\r\n",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = CommandRecErr;
						s_GSMcomm.Timer = 0;
		        return CommandRecErr;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\nNO CARRIER\r\n",s_GSMcomm.ps_commu->RCNT,14)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecNoCarrier;
						s_GSMcomm.Timer = 0;
		        return RecNoCarrier;														 
		    }
				/* 
				// ���Ž��������Զ��ϱ����ٷ���AT���������ﴦ����URC�Զ��ϱ����ô���
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CMTI: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecSms;
						s_GSMcomm.Timer = 0;
		        return RecSms;
		    }			
				*/
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n>",s_GSMcomm.ps_commu->RCNT,3)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCMGSAck;
						s_GSMcomm.Timer = 0;
		        return RecCMGSAck;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CSQ: ",s_GSMcomm.ps_commu->RCNT,8)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCSQcpl;
						s_GSMcomm.Timer = 0;
		        return RecCSQcpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CLCC: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCSCCcpl;
						s_GSMcomm.Timer = 0;
		        return RecCSCCcpl;
		    }		
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CPIN: READY",s_GSMcomm.ps_commu->RCNT,14)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCPINcpl;
						s_GSMcomm.Timer = 0;
		        return RecCPINcpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CPIN: NOT INSERTED",s_GSMcomm.ps_commu->RCNT,21)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCPINcplErr;
						s_GSMcomm.Timer = 0;
		        return RecCPINcplErr;
		    }
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CREG: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCREGcpl;
						s_GSMcomm.Timer = 0;
		        return RecCREGcpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CGREG: ",s_GSMcomm.ps_commu->RCNT,10)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCGREGcpl;
						s_GSMcomm.Timer = 0;
		        return RecCGREGcpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CMGS: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCMGScpl;
						s_GSMcomm.Timer = 0;
		        return RecCMGScpl;
		    }	    
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CMGR: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCMGRAck;
						s_GSMcomm.Timer = 0;
		        return RecCMGRAck;
		    }
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"SEND OK",s_GSMcomm.ps_commu->RCNT,7)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecSENDOKcpl;
						s_GSMcomm.Timer = 0;
		        return RecSENDOKcpl;
		    }
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"SHUT OK",s_GSMcomm.ps_commu->RCNT,7)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecShutCpl;
						s_GSMcomm.Timer = 0;
		        return RecShutCpl;
		    }
		    else if((StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"CONNECT OK",s_GSMcomm.ps_commu->RCNT,10)!=0xffff) ||
		    				(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"ALREADY CONNECT",s_GSMcomm.ps_commu->RCNT,15)!=0xffff))
				{
		        s_GSMcomm.CmdPhase = RecConnectCpl;
						s_GSMcomm.Timer = 0;
		        return RecConnectCpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"CONNECT FAIL",s_GSMcomm.ps_commu->RCNT,12)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecConnectFail;
						s_GSMcomm.Timer = 0;
		        return RecConnectFail;
		    }
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"CLOSE OK",s_GSMcomm.ps_commu->RCNT,8)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCloseCpl;
						s_GSMcomm.Timer = 0;
		        return RecCloseCpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)".",s_GSMcomm.ps_commu->RCNT,1)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCIFSRAck;
						s_GSMcomm.Timer = 0;
		        return RecCIFSRAck;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CNUM: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCNUMAck;
						s_GSMcomm.Timer = 0;
		        return RecCNUMAck;
		    }
		}
		
    if(s_GSMcomm.Timer > TimeOut)
		{
	      /* ����жϳ�ʱ,�����������һѭ��,�ٵ�����Ѿ���ʱ��.��ʵ�����յ�Ӧ��. */
	      s_GSMcomm.CmdPhase = CommandTimeout;
	      s_GSMcomm.Timer = 0;
				//s_GSMcomm.ps_commu->RCNT = 0;
        return CommandTimeout;
    }
		else
		{
        /* wait for timeout */
        return 0;
    }
}
/*
*********************************************************************************************************
*                            							GSM�ײ���/���жϺ���(stm32f10x_it.c�ӿ�)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                     BSP_GSMRING_IT_CON()
*
* Description: Ring pin interrupts cfg.
* Argument(s): none.
* Return(s)  : none.
* Note(s)    : 
*********************************************************************************************************
*/
void	BSP_GSMRING_IT_CON (u8	newSta)
{
		EXTI_InitTypeDef EXTI_InitStructure;
		
		// Ring ����I/O
		// Clear the Key Button EXTI line pending bit
	  EXTI_ClearITPendingBit(DEF_GSM_RING_EXTI_LINE);		
	  EXTI_InitStructure.EXTI_Line 		= DEF_GSM_RING_EXTI_LINE;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = DEF_GSM_RING_EXTI_TRIGGER;
	  EXTI_InitStructure.EXTI_LineCmd = (FunctionalState)newSta;
	  EXTI_Init(&EXTI_InitStructure);
}

/*
*********************************************************************************************************
*                                     GSMRingPinInit()
*
* Description: Ring pin interrupts.
* Argument(s): none.
* Return(s)  : none.
* Note(s)    : 
*********************************************************************************************************
*/
static	void	GSMRingPinInit (void)
{
	  EXTI_InitTypeDef EXTI_InitStructure;
		
		// Enbale	exti clock 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	  // Connect Key Button EXTI Line to Key Button GPIO Pin 
	  GPIO_EXTILineConfig(DEF_GSM_RING_EXTI_PORTS, DEF_GSM_RING_EXTI_PINS);
	 
		// Clear the Key Button EXTI line pending bit
	  EXTI_ClearITPendingBit(DEF_GSM_RING_EXTI_LINE);
	
		// Configure Key Button EXTI Line to generate an interrupt on falling edge  
		
		// Ring ����I/O 
	  EXTI_InitStructure.EXTI_Line 		= DEF_GSM_RING_EXTI_LINE;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = DEF_GSM_RING_EXTI_TRIGGER;
	  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	  EXTI_Init(&EXTI_InitStructure);
}
/*
*********************************************************************************************************
*                                     BSP_GSMRing_ISRHandler()
*
* Description: Ring pin interrupts.	 ���ringWUpFlag�ѱ����á�1�����޸�ringWUpFlag=2����ʾ�豸˯����Ring�¼�����
* Argument(s): none.
* Return(s)  : none.
* Note(s)    : 
*********************************************************************************************************
*/
void  BSP_GSMRing_ISRHandler (void)
{	
		if(ringWUpFlag == 1)
				ringWUpFlag =2;	// ����GSM��Ring���ѱ�־
		
		// ����Ring������־
		BSP_GSM_SetFlag(DEF_RING_FLAG);	// ֪ͨRing�жϲ���
}
/*
*********************************************************************************************************
*                                     BSP_GSMRS232_RxTxISRHandler()
*
* Description: Handle Rx and Tx interrupts.
*
* Argument(s): none.
*
* Return(s)  : none.
*
* Note(s)    : (1) This ISR handler handles the interrupt entrance/exit as expected by
*                  by uC/OS-II v2.85.  If you are using a different RTOS (or no RTOS), then this
*                  procedure may need to be modified or eliminated.  However, the logic in the handler
*                  need not be changed.
*********************************************************************************************************
*/
void  BSP_GSMRS232_RxTxISRHandler (void)
{
		vu8  rx_data;
		USART_TypeDef  *usart;

#if   (DEF_GSM_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_GSM_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_GSM_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_GSM_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_GSM_UART == DEF_UART_5)
		usart = UART5;
#else
		return;
#endif

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
        rx_data = USART_ReceiveData(usart) & 0xFF;              /* Read one byte from the receive data register         */
        GSMRS232_RxHandler(rx_data);
        USART_ClearITPendingBit(usart, USART_IT_RXNE);          /* Clear the GSM_UART Receive interrupt                   */
    }

    else if (USART_GetITStatus(usart, USART_IT_TXE) != RESET) {
        GSMRS232_TxHandler();
        USART_ClearITPendingBit(usart, USART_IT_TXE);           /* Clear the GSM_UART transmit interrupt  */
    }  
		else {
				USART_ReceiveData(usart);
				USART_ClearITPendingBit(usart, USART_IT_RXNE);
		}   
}
/*
*********************************************************************************************************
* Function Name  : 												GSMRS232_RxHandler()
* Description    : Handle a received byte.   
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void  GSMRS232_RxHandler (u8 rx_data)
{
		// ATָ������.
		if(ComGSM.RCNT	<	GSM_GPSRSBUF_RXD_SIZE)
		{				
				RBufGPRS[ComGSM.RCNT] = rx_data;  // ���ݶ����û�������.
				ComGSM.RCNT++;
		}
}
/*
*********************************************************************************************************
* Function Name  : 												GSMRS232_TxHandler()
* Description    : Handle a byte transmission.     
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void  GSMRS232_TxHandler (void)
{
		vu16 state;
		USART_TypeDef  *usart;

#if   (DEF_GSM_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_GSM_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_GSM_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_GSM_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_GSM_UART == DEF_UART_5)
		usart = UART5;
#else
		return;
#endif
	
		

		state = ComGSM.State;
		
		if((state & MOD) == MOD_S)
		{
		    /* ���ڷ���ģʽ */
		    if(ComGSM.SCNT < ComGSM.SLEN)
				{
						USART_SendData(usart, SBufGPRS[ComGSM.SCNT]);
						while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
						ComGSM.SCNT++;
		    }
				else
				{
			      state=(state&(~SS))|SS_O; 	  		// �÷�����ɱ�־. 
			      state = state&(~MOD);         		// ��Ϊ����ģʽ. 
			
						ComGSM.State	=	state;		
						GSM_DISABLE_SEND_IT();						// ֹͣGSM������.
		    }
		}
}
/*
*********************************************************************************************************
* Function Name  : 												StartSend_GPRS()
* Description    : use to start the send        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	StartSend_GPRS (void)
{  
		u16	i=0;
		//u16 tmpRCNT=0;
		USART_TypeDef  *usart;

#if   (DEF_GSM_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_GSM_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_GSM_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_GSM_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_GSM_UART == DEF_UART_5)
		usart = UART5;
#else
    return;
#endif
	
		// 2015-1-16 by��gaofei ��������ͬ�����ƣ���ֹ�������ݽضϽ����ַ���
		/*
		for(i=0;	i<1000;	i++)
		{
				tmpRCNT = ComGSM.RCNT;
				if(tmpRCNT == 0)
						break;
				else
				{
						tmpRCNT = tmpRCNT - 1;
						if((RBufGPRS[tmpRCNT]  == '\r') || (RBufGPRS[tmpRCNT]  == '\n'))
						{
								gsmDelayMS(10);
								if((tmpRCNT + 1) == ComGSM.RCNT)
										break;
						}
				}	
				gsmDelayMS(1);
		}
		*/

	 	ComGSM.SCNT =0;
    ComGSM.RCNT =0;
    ComGSM.RLEN =GSM_GPSRSBUF_RXD_SIZE;
		ComGSM.State =MOD_S | SS_I; //�÷���ģʽ�����ͽ����б�־ 
		ComGSM.State &=~SS_O;				//2015-11-4 �巢����ɱ�־
    
		GSM_ENABLE_SEND_IT();				//Enable	send interrupt
																	
		if(ComGSM.SCNT < ComGSM.SLEN)
		{
				USART_SendData(usart, SBufGPRS[ComGSM.SCNT]);
	  		while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);	  // ������Ӹ����������ݲ�������ȱ�ֽڣ�
				ComGSM.SCNT++;
		}
}

/*
*********************************************************************************************************
*                            							GSM��������   
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ERR_Add()
* Description    : �����ۼ�        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void		BSP_GSM_ERR_Add (u16 *Var,	u16	AddTimes)
{
		if(*Var + AddTimes >= 0xffff)
				*Var = 0xffff;
		else
				*Var +=AddTimes;		
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ERR_Clr()
* Description    : �����ۼӸ�λ        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void		BSP_GSM_ERR_Clr (u16 *Var)
{
		*Var = 0;
}

/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ERR_Que()
* Description    : �����ۼӴ�����ѯ        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
u16		BSP_GSM_ERR_Que (u16 *Var)
{
		return	*Var;
}
/*
*********************************************************************************************************
*                            							GSM�ײ㷢������Ӧ�ú���     
*********************************************************************************************************
*/


/*
*********************************************************************************************************
* Function Name  : 													Start2doSendAT()
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void	Start2doSendAT (void)
{
		u16	m=0;
		u8	*pInit8u;
		
    if(s_GSMcomm.SetupPhase == CommandInitOK)
		{}
		else
        return;
    if(s_GSMcomm.ActiveCmd == NoCmdActive)
		{}
    else
				return;
    if(s_GSMcomm.CommandFlag & ATCommandF)
		{}
    else
				return;
    if(s_GSMcomm.CmdPhase == CommandIdle)
		{}
    else																		
        return;

		if(1)
		{
			  s_GSMcomm.ActiveCmd = ATCmdActive;			//	����ռ�ݴ���
				pInit8u = (u8 *)s_GSMcomm.pSendBuf;
				
				for(m=0;	m<s_ATcomm.Slen;	m++)
				{
			  		*pInit8u = *(s_ATcomm.pSendBuf + m);
			  		pInit8u++;
		    }
				memset((u8 *)(s_ATcomm.pSendBuf),	'\0',	GSM_ATSBUF_TXD_SIZE);	 // ��֮ǰ��ջ�����
		    /* call sendstart. */
		    s_GSMcomm.Timer = 0;
		    s_GSMcomm.CmdPhase = CommandSend;
		    s_GSMcomm.ps_commu->SLEN = m;
		    s_GSMcomm.sendstart();			//���÷���
		}
}
/*
*********************************************************************************************************
* Function Name  : 													DoSendProcess()	
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void	 DoSendProcess (void)
{
		u8	i=0;
		u16	m=0;
		if(s_GSMcomm.SetupPhase == CommandInitOK)
		{}
		else
				return;
    if(s_GSMcomm.ActiveCmd == ATCmdActive)
		{}
		else
        return;
    if(s_GSMcomm.CmdPhase == CommandSend)
		{
        if(s_GSMcomm.ps_commu->State & SS_O)
				{
            /* The whole Command fram has been send out. */
            s_GSMcomm.CmdPhase = CommandSdCpl;
            s_GSMcomm.Timer = 0;
        }
    }
		else if(s_GSMcomm.CmdPhase == CommandSdCpl)
		{
				i = CheckResultCode(s_ATcomm.CheckMode,	s_ATcomm.timeout);			
        if(i)
				{ 
						s_ATcomm.ReturnCode	=	i;
						s_ATcomm.Rlen = s_GSMcomm.ps_commu->RCNT;		
								
						for(m=0; ((m<s_GSMcomm.ps_commu->RCNT) && (m<GSM_ATSBUF_RXD_SIZE));	m++)
						{
								*(s_ATcomm.pRecBuf + m) = *(s_GSMcomm.pRecBuf + m);	//�������븳ֵ��AT����BUF��
						}
							
            s_GSMcomm.CmdPhase = CommandIdle;							//����ΪAT����Ŀ���ģʽ
            s_GSMcomm.ActiveCmd = NoCmdActive;
						s_GSMcomm.CommandFlag	&= ~ATCommandF;					//��AT���ͱ�־
        }
				else
				{}/* wait for receive complete */
    }
		else
		{}/* wait for send complete */
}
/*
*********************************************************************************************************
* Function Name  : 										BSP_SendATcmdPro()
* Description    :          
* Input          : 
* Output         : 
* Return         : ����:	DEF_GSM_NONE_ERR �ɹ�, DEF_GSM_BUSY_ERRռ����, 
*													����������(��ֵΪ1~255)������ DEF_GSM_NONE_ERR��DEF_GSM_BUSY_ERR��ͻ
*********************************************************************************************************
*/
s8		BSP_SendATcmdPro (u8	checkMode,	u8	*pSendString,	u16	Slen,	u16	PendCode,	u32	timeout)
{
		u16	i=0;
		u32	loop=0;
		s8	retCode=DEF_GSM_NONE_ERR;
		if(s_GSMcomm.ActiveCmd == NoCmdActive)
		{
				for(i=0;	i<Slen;	i++)
				{
					 *(s_ATcomm.pSendBuf + i)	=	*(pSendString + i);	
				}
				s_ATcomm.Slen				=	Slen;					
				s_ATcomm.Rlen				=	0;
				s_ATcomm.timeout		=	timeout;
				s_ATcomm.ReturnCode	=	0;
				s_ATcomm.CheckMode	=	checkMode;
				memset((u8	*)s_ATcomm.pRecBuf,	'\0',	GSM_ATSBUF_RXD_SIZE);
				
				// ��������ַ���
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				//myPrintf("%s",	s_ATcomm.pSendBuf);
				BSP_OSPutChar (DEF_DBG_UART,	s_ATcomm.pSendBuf,	s_ATcomm.Slen);
				#endif

				s_GSMcomm.CommandFlag	|=	ATCommandF;	// �ñ�־��ʼ����;
				
				loop=0;
				while(s_ATcomm.ReturnCode == 0)
				{
						gsmDelayMS(1);		// 2015-1-2 by:felix ���ƥ��Ч��		
						loop++;
						if(loop >= (timeout*100*2))		// 2015-11-4 by:felix ���ӳ�ʱ��ط�ֹһ�����ʵ��µ�AT���Ͷ�����������λ
								break;
				}	
				
				// ��������ַ���
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				myPrintf("%s",	s_ATcomm.pRecBuf);
				#endif
																	
				if(s_ATcomm.ReturnCode	!=	PendCode)
						retCode	=	(s8)(s_ATcomm.ReturnCode);
				else
						retCode	=	DEF_GSM_NONE_ERR;
		}
		else
				retCode	=	DEF_GSM_BUSY_ERR;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  : 										BSP_SendATcmdProExt()
* Description    : �ֶ����ʹ�����ֹ�Զ�������ǰ��buf���µĽ���gprs URC��©         
* Input          : 
* Output         : 
* Return         : ����:	DEF_GSM_NONE_ERR �ɹ�, DEF_GSM_BUSY_ERRռ����, 
*													����������(��ֵΪ1~255)������ DEF_GSM_NONE_ERR��DEF_GSM_BUSY_ERR��ͻ
*********************************************************************************************************
*/
s8		BSP_SendATcmdProExt (u8	checkMode,	u8	*pSendString,	u16	Slen,	u16	PendCode,	u32	timeout)
{
		u16	i=0, p=0;
		s8	retCode=DEF_GSM_NONE_ERR;
    u16 usCnt = s_GSMcomm.ps_commu->RCNT;
		if(s_GSMcomm.ActiveCmd == NoCmdActive)
		{		
				// ��������ַ���
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				//myPrintf("%s",	s_ATcomm.pSendBuf);
				BSP_OSPutChar (DEF_DBG_UART,	s_ATcomm.pSendBuf,	s_ATcomm.Slen);
				#endif

        BSP_PutChar(DEF_USART_1,pSendString,Slen);

        i=0;
        usCnt = s_GSMcomm.ps_commu->RCNT;
				while(1)
				{
						if(s_GSMcomm.ps_commu->RCNT >= usCnt)
						{
								p = StringSearch((u8 *)&s_GSMcomm.pRecBuf[usCnt],(u8 *)"\r\nOK",(s_GSMcomm.ps_commu->RCNT - usCnt),4);		
								if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
								{
										break;
								}		
								OSTimeDly(1);   
								if(i++  >= timeout * 100u)
								{
										retCode	=	DEF_GSM_BUSY_ERR;
										break;
								}
								else
								{
								}
						}
						else
						{
								retCode	=	DEF_GSM_BUSY_ERR;
								break;
						}
				}				
		}
		else
				retCode	=	DEF_GSM_BUSY_ERR;

		return	retCode;	
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMSoftReset() 
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMSoftReset(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// �����λ
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CFUN11,	strlen((const char *)AT_CFUN11),	CommandRecOk,	CFUN11Timeout) !=	DEF_GSM_NONE_ERR)
						{																
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_CFUN11_ERR;		
						}	
						else
						{
								BSP_GSM_ReleaseFlag(DEF_ALL_FLAG);						//  �������״̬��־
								memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	//  ��������
								s_GSMcomm.GSMComStatus 	= GSM_POWOK;					//  �ó�ʼ�����ʼ����״̬
								s_GSMcomm.SetupPhase 		= CheckSimCardSend;		//  �����λ����Ҫ�˻ص�sim����ⲽ��
								s_GSMcomm.Timer					=	0;
								s_GSMcomm.HardWareSta		=	DEF_GSMHARD_NONE_ERR;	
								retCode = DEF_GSM_NONE_ERR;
						}					
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}

/*
*********************************************************************************************************
*                            					GSM TTSӦ�ú���(���ϲ�Ӧ�ú�������)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSPlayPro() 
* Description    : ����TTS����(��ʼ���ź�����TTSPlayF,ɨ�躯����⵽����URC��λ�ñ�־)        
* Input          : mode			:DEF_TTS_PLAY_ASCII or  DEF_TTS_PLAY_UCS2 
*                : * pString: UCS2���ASCII���ַ���
*                : len			: �ַ�������
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TTSPlayPro	(u8 mode,	u8* pString, u16 len,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8  tmpBuf[100]={0};	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase >= CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))	
				{
						//	Send "AT+CTTS=1/2,string\r" AT_CTTS_2_H
						memset(tmpBuf,	0,	sizeof(tmpBuf));
						
						if(mode == DEF_TTS_PLAY_ASCII)
								strcat((char *)tmpBuf,	(char *)AT_CTTS_2_H);
						else 
								strcat((char *)tmpBuf,	(char *)AT_CTTS_1_H);
						
						if(len < (sizeof(tmpBuf) - strlen((const char *)tmpBuf) - 1))
						{
								strcat((char *)tmpBuf,	(char *)pString);
								strcat((char *)tmpBuf,	(char *)"\r");
																
								IO_SPAKER_PA_EN();	// ����ʹ�ܹ���
								if(BSP_SendATcmdProExt(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CTTSTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_CTTS_ERR;			
								}
								else
								{
										BSP_GSM_SetFlag (DEF_TTSPLAY_FLAG);		// ����TTS�����б�־����ɨ�赽URC�������Զ����
										retCode = DEF_GSM_NONE_ERR;			
								}							
						}
						else
								retCode	=	DEF_GSM_DATA_OVERFLOW;	// �������ݳ����������								
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSStopPro() 
* Description    : ֹͣ����TTS����        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TTSStopPro	(u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						//	Send "AT+CTTS=0" AT_CTTS_0
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CTTS_0,	strlen((const char *)AT_CTTS_0),	CommandRecOk,	CTTSTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_CTTS_ERR;			
						}
						else
						{					
								IO_SPAKER_PA_DIS();	// �رչ���
								retCode = DEF_GSM_NONE_ERR;					
						}
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSCfgPro() 
* Description    : ����TTS���Ų���        
* Input          : vol		:������С��0~100,dft:50��
*                : mode		:����ģʽ��0~3,dft:0��
*                : pitch	:���������������ϸ�⣨1~100,dft:50��
*                : speed	:���٣�1~100,dft:50��
*                : channel:ͨ���ţ�����SIM800ֻ��һ��ͨ����ͨ��0������ֵ�����ش���0~1,dft:0��
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TTSCfgPro	(u8 vol, u8 mode,	u8 pitch,	u8 speed,	u8 channel,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8  tmpBuf[50]="",tmpBuf1[25]="";	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((vol > 100) || (mode > 4) || (pitch == 0) || (pitch > 100) || (speed == 0) || (speed > 100) || (channel != 0))
						retCode = DEF_GSM_FAIL_ERR; 
				else
				{
    				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
    				{
    						//	Send "AT+CTTSPARAM=<vol>,<mode>,<pitch>,<speed>,<channel>\r" AT_CTTSPARAM_H     						 						
    						memset(tmpBuf,	0,	sizeof(tmpBuf));
    						memset(tmpBuf1,	0,	sizeof(tmpBuf1));   						
    						strcat((char *)tmpBuf,	(char *)AT_CTTSPARAM_H);
    						sprintf((char *)tmpBuf1,	"%d,%d,%d,%d,%d\r",	vol,mode,pitch,speed,channel);
    						strcat((char *)tmpBuf,	(char *)tmpBuf1);
    													
    						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CTTSPARAMTimeout) !=	DEF_GSM_NONE_ERR)
    						{
    								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
    								retCode	=	DEF_GSM_CTTSPARAM_ERR;
    						}
    						else
    								retCode = DEF_GSM_NONE_ERR;
    				}
    				else
    						retCode	=	DEF_GSM_BUSY_ERR;
    		}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSCfgRingPro() 
* Description    : ��������ʱʹ�ܻ��ֹTTS����        
* Input          : en:DEF_ENABLE or  DEF_DISENABLE 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TTSCfgRingPro	(u8 en,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[20]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						//	Send "AT+CTTSRING=0/1\r" AT_CTTSRING_
						memset(tmpBuf,	0,	sizeof(tmpBuf));
						if(en == DEF_ENABLE)
								strcat((char *)tmpBuf,	(char *)AT_CTTSRING_1);
						else 
								strcat((char *)tmpBuf,	(char *)AT_CTTSRING_0);
													
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CTTSRINGTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_CTTSRING_ERR;			
						}
						else
								retCode = DEF_GSM_NONE_ERR;			
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}

/*
*********************************************************************************************************
*                            					GSM FSӦ�ú���(���ϲ�Ӧ�ú�������)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSCreateFilePro() 
* Description    : �����ļ�������и��ļ���ֱ�ӷ��سɹ���ֻ֧���ڸ�Ŀ¼UserĿ¼�½����ļ���      
* Input          : *pFile:�ļ��������磺log.txt��
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_FSCreateFilePro	(u8 *pFile,	u32 timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="";
		u16	j=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// ��ѯUserĿ¼�Ƿ����	
						//	Send "AT+FSLS=C:\\r" AT_FSLS_C	
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_FSLS_C,	strlen((const char *)AT_FSLS_C),	CommandRecOk,	FSLSTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_FSLS_ERR;	
						}
						else						
						{
								j = 0;
								j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"User\\",s_GSMcomm.ps_commu->RCNT,5);
								if(j == 0xffff)		
								{
										// UserĿ¼������,����Ŀ¼
										//	Send "AT+FSMKDIR=C:\\User\\r" AT_FSMKDIR_H
		    						memset(tmpBuf,	0,	sizeof(tmpBuf));
		    						strcat((char *)tmpBuf,	(char *)AT_FSMKDIR_H);
		    						strcat((char *)tmpBuf,	"C:\\User\\r");
		
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSMKDIRTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												retCode	=	DEF_GSM_FSMKDIR_ERR;	
        						}	
        						else
        						{       										       								
        								// �����ļ�
            						//	Send "AT+FSCREATE=C:\\User\file.txt\r" AT_FSCREATE_H
            						memset(tmpBuf,	0,	sizeof(tmpBuf));
            						strcat((char *)tmpBuf,	(char *)AT_FSCREATE_H);
            						strcat((char *)tmpBuf,	"C:\\User\\");
            						strcat((char *)tmpBuf,	(char *)pFile);
            						strcat((char *)tmpBuf,	"\r");
            													
            						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSCREATETimeout) !=	DEF_GSM_NONE_ERR)
            						{
            								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
            								retCode	=	DEF_GSM_FSCREATE_ERR;			
            						}	
            						else
            								retCode = DEF_GSM_NONE_ERR;		// �ļ������ɹ�!!										
        						} 																								
								}
								else
								{
										// UserĿ¼�Ѿ����ڣ���ѯ�ļ��Ƿ����	
										//	Send "AT+FSLS=C:\\User\\r" AT_FSLS_USER											
										if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_FSLS_USER,	strlen((const char *)AT_FSLS_USER),	CommandRecOk,	FSLSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												retCode	=	DEF_GSM_FSLS_ERR;	
										}
										else
										{
												j = 0;
												j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)pFile, s_GSMcomm.ps_commu->RCNT, strlen((const char *)pFile));
												if(j == 0xffff)		
												{
														// �����ļ�
                						//	Send "AT+FSCREATE=C:\\User\file.txt\r" AT_FSCREATE_H
                						memset(tmpBuf,	0,	sizeof(tmpBuf));
                						strcat((char *)tmpBuf,	(char *)AT_FSCREATE_H);
                						strcat((char *)tmpBuf,	"C:\\User\\");
                						strcat((char *)tmpBuf,	(char *)pFile);
                						strcat((char *)tmpBuf,	"\r");
                													
                						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSCREATETimeout) !=	DEF_GSM_NONE_ERR)
                						{
                								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
                								retCode	=	DEF_GSM_FSCREATE_ERR;			
                						}	
                						else
                								retCode = DEF_GSM_NONE_ERR;		// �ļ������ɹ�!!		
												}
												else
														retCode = DEF_GSM_NONE_ERR;		// �ļ��Ѿ�����ֱ�ӷ���
										}
								}									
						}
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSDelFilePro() 
* Description    : ɾ���ļ���ֻ֧���ڸ�Ŀ¼UserĿ¼��ɾ���ļ���
* Input          : *pFile:�ļ��������磺log.txt��
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_FSDelFilePro	(u8 *pFile,	u32 timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// ����ѯ�ļ�ֱ��ɾ��
						//	Send "AT+FSDEL=C:\\User\file.txt\r" AT_FSDEL_H												
						memset(tmpBuf,	0,	sizeof(tmpBuf));
						strcat((char *)tmpBuf,	(char *)AT_FSDEL_H);
						strcat((char *)tmpBuf,	(char *)pFile);
						strcat((char *)tmpBuf,	"\r");													
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSDELTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_FSDEL_ERR;			
						}	
						else
								retCode = DEF_GSM_NONE_ERR;		// �ļ�ɾ���ɹ�			
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSReadFilePro() 
* Description    : ��ȡ�ļ���ֻ֧���ڸ�Ŀ¼UserĿ¼�¶�ȡ�ļ���
* Input          : *pFile	:�ļ��������磺log.txt��
*								 : posi		:��ȡƫ��������λ���ֽڣ�
*								 : *rData	:��ȡ��������ָ��
*								 : reqLen	:�����ȡ���ļ����ȣ����Ȳ��ܳ���GSM�ײ��ȡ��������С��
*								 : *retLen:ʵ�ʶ�ȡ���ļ�����
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_FSReadFilePro	(u8 *pFile,	u16 posi, u8 *rData, u16 reqLen, u16 *retLen,	u32 timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="",tmpBuf1[20]="";
		u16 j=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{							
				if(reqLen > (GSM_ATSBUF_RXD_SIZE - 100))
						retCode	=	DEF_GSM_DATA_OVERFLOW;		// ����GSM���ջ�������С���账��
				else
				{						
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{						
								// ���
								//	Send "AT+FSREAD=C:\\User\file,0/1,len,posi\r" AT_FSREAD_H												
								memset(tmpBuf,	0,	sizeof(tmpBuf));
								memset(tmpBuf1,	0,	sizeof(tmpBuf1));
								strcat((char *)tmpBuf,	(char *)AT_FSREAD_H);
								strcat((char *)tmpBuf,	"C:\\User\\");
								strcat((char *)tmpBuf,	(char *)pFile);
								
								if(posi == 0) 
										sprintf((char *)tmpBuf1,	",0,%d,0\r",	reqLen);				// ���ļ���ʼλ�ö�ȡ
								else
										sprintf((char *)tmpBuf1,	",1,%d,%d\r",	reqLen,posi);	// ���ļ�posiƫ��λ�ö�ȡ
								
								strcat((char *)tmpBuf,	(char *)tmpBuf1);												
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSREADTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_FSREAD_ERR;			
								}	
								else
								{
										// ������������(�����жϻس�������Ϊ����������д�Ĺ����л��Զ���ÿ����¼���ӻس�����)
										for(j=0;	(*(s_GSMcomm.pRecBuf + j) != 'O') && (*(s_GSMcomm.pRecBuf + j) != 'K') && (j < s_GSMcomm.ps_commu->RCNT);	j++)
										{
												*(rData + j) = *(s_GSMcomm.pRecBuf + j);
										}
										*retLen = j;	// ��ȡʵ�ʳ���
										retCode = DEF_GSM_NONE_ERR;		// �ļ���ȡ�ɹ�	
								}		
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSWriteFilePro() 
* Description    : д�ļ���ֻ֧���ڸ�Ŀ¼UserĿ¼��д�ļ����Զ�д���ļ���β��������ռ䲻�����Զ������ļ�ͷд����������ļ����֣�
*                : дÿ����¼ʱ���Զ�Ϊÿ����¼���ӻس����У��´��ٴ�д��ʱ���µ�һ�в��������ֶ���ӻس�����
*								 : �ļ��Ŀ��ÿռ������ģ��̼��汾�����ΪTTS�汾�����һЩ�����Ϊ�����汾��СһЩ����С��Լ60kByte
* Input          : *pFile	:�ļ��������磺log.txt��
*								 : *wData	:д���ݻ�����ָ��
*								 : wLen		:д�����ݳ���
*								 : wTimeOut:д��ʱ����λs����С�ڡ�д��ʱ������Ч�����������ûд��ǰ�ͳ�ʱ�����ˣ�
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_FSWriteFilePro	(u8 *pFile, u8 *wData, u16 wLen, u8 wTimeOut,	u32 timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="",tmpBuf1[20]="";
		u16 j=0,k=0;
		u32	avaLen=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{							
				if(wLen > (GSM_ATSBUF_TXD_SIZE - 100))
						retCode	=	DEF_GSM_DATA_OVERFLOW;		// ����GSM���ͻ�������С���账��
				else
				{					
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{						
								// �����жϿ��ÿռ��С
								//	Send "AT+FSMEM\r"	AT_FSMEM
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_FSMEM,	strlen((const char *)AT_FSMEM),	CommandRecOk,	FSMEMTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_FSMEM_ERR;			
								}	
								else
								{
										j = 0;
										j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+FSMEM: ", s_GSMcomm.ps_commu->RCNT, 8);
										if(j == 0xffff)	
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												retCode = DEF_GSM_NONE_ERR;		// �Ѿ������ļ�ֱ�ӷ���	
										}
										else
										{
												// ��ȡ������Ϣ
												j = j + 10; // ָ��ռ��С
												memset(tmpBuf1,'\0',sizeof(tmpBuf1));
												for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != 'b') && (k < sizeof(tmpBuf1)) && (k < s_GSMcomm.ps_commu->RCNT);	k++)
												{
														tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
												}
												tmpBuf1[k] = '\0';
												avaLen = atoi((const char *)tmpBuf1);
												
												// ���
												//	Send "AT+FSWRITE=C:\\User\file.txt,0/1,len,timeout\r" AT_FSWRITE_H
												memset(tmpBuf,	0,	sizeof(tmpBuf));
												memset(tmpBuf1,	0,	sizeof(tmpBuf1));
												strcat((char *)tmpBuf,	(char *)AT_FSWRITE_H);
												strcat((char *)tmpBuf,	"C:\\User\\");
												strcat((char *)tmpBuf,	(char *)pFile);
												// �ж�����д�볤���Ƿ�Ϸ�
												if((wLen + 1000) >= avaLen)	
														strcat((char *)tmpBuf,	",0");	// ����ļ����������ͷд�븲���ʼ����Ϣ
												else
														strcat((char *)tmpBuf,	",1");	// ���ļ���βд��
												
												sprintf((char *)tmpBuf1,	",%d,%d\r",	wLen,wTimeOut);	
												strcat((char *)tmpBuf,	(char *)tmpBuf1);								
												
												// Pend">"							
												if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecFSWriteAck,	FSWRITETimeout) !=	DEF_GSM_NONE_ERR)	//��ʱ�̶���ʱΪFSWRITETimeout
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
														retCode	=	DEF_GSM_FSWRITE_ERR;			
												}	
												else
												{
														//	Send data
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)wData,	wLen,	CommandRecOk,	FSWRITECPLTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);	// �����ۼ�
																retCode	=	DEF_GSM_FSWRITECPL_ERR;
														}	
														else
																retCode = DEF_GSM_NONE_ERR;		// �ļ�д��ɹ�!!		
												}															
										}										
								}									
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
*                            					GSM BTӦ�ú���(���ϲ�Ӧ�ú�������)     
*********************************************************************************************************
*/


/*
*********************************************************************************************************
* Function Name  :									  BSP_BTInitPro() 
* Description    : ������ʼ�����������õ�
* Input          : *pName	:�ļ��������磺Y05D-105���18char
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTInitPro	(u8 *pName, u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8	tmpBuf[50]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ��������Դ(����ָ���ⷵ���룬���֮ǰ������ԴΪ����״̬��᷵��"ERROR"Ҳ��Ϊ��Դ�Ѿ�ʹ����)
						// Send "AT+BTPOWER=1\r"
						BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTPOWER_1,	strlen((const char *)AT_BTPOWER_1),	CommandRecOk,	BTPOWERTimeout);
						gsmDelayMS(1000);	
					
						// ����������������
						// Send "AT+BTHOST=name\r"	AT_BTHOST_H
						memset(tmpBuf,	0,	sizeof(tmpBuf));
        		strcat((char *)tmpBuf,	(char *)AT_BTHOST_H);
        		strcat((char *)tmpBuf,	(char *)pName);
						strcat((char *)tmpBuf,	"\r");
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	BTHOSTTimeout)	!=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	= DEF_GSM_BTHOST_ERR;	
						}	
						else
						{
								gsmDelayMS(500);	
								// ���������ɼ���	
								// Send "AT+BTVIS=1\r"	AT_BTVIS_1
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTVIS_1,	strlen((const char *)AT_BTVIS_1),	CommandRecOk,	BTVISTimeout)	!=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	= DEF_GSM_BTVIS_ERR;	
								}	
								else
								{
										// ����������������ģʽΪ�ֶ�ģʽ
										// Send "AT+BTSPPGET=1\r"	AT_BTSPPGET_1	
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTSPPGET_1,	strlen((const char *)AT_BTSPPGET_1),	CommandRecOk,	BTSPPGET1Timeout)	!=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												retCode	= DEF_GSM_BTSPPGET1_ERR;	
										}	
										else
												retCode = DEF_GSM_NONE_ERR;
								}							
						}
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;				
		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTPairAckPro() 
* Description    : ���������ͻ��˷�����ƥ������
* Input          : mode		:0-������֤��1-Passkey ��֤
*                : *pPass	:���ģʽΪ1����Ҫ�ṩ���루����4~16�ֽڣ�
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTPairAckPro	(u8 mode,	u8 *pPass, u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8  tmpBuf[50]="";
		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ����ƥ��
						if(mode == 0)
						{
								// ����ƥ��
								// Send "AT+BTPAIR=1,1\r" AT_BTPAIR_1
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTPAIR_1,	strlen((const char *)AT_BTPAIR_1),	CommandRecOk,	BTPAIRTimeout)	!=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	= DEF_GSM_BTPAIR_ERR;	
								}	
								else
										retCode = DEF_GSM_NONE_ERR;
						}
						else
						{
								// Passkeyƥ��
								// Send "AT+BTPAIR=2,Passkey\r" AT_BTPAIR_2_H
								memset(tmpBuf,	0,	sizeof(tmpBuf));
        				strcat((char *)tmpBuf,	(char *)AT_BTPAIR_2_H);
        				strcat((char *)tmpBuf,	(char *)pPass);
        				strcat((char *)tmpBuf,	"\r");
								
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	BTPAIRTimeout)	!=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	= DEF_GSM_BTPAIR_ERR;	
								}	
								else
										retCode = DEF_GSM_NONE_ERR;
						}
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;				
		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTConnectAckPro() 
* Description    : ������������
* Input          : en	: DEF_DISABLE-��ֹ����;DEF_ENABLE-�������� 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTConnectAckPro	(u8 en,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ������������
						// Send "AT+BTACPT=1\r" AT_BTACPT_1
						if(en == DEF_DISABLE)
						{
    						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTACPT_0,	strlen((const char *)AT_BTACPT_0),	CommandRecOk,	BTACPTTimeout)	!=	DEF_GSM_NONE_ERR)
    						{
    								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
    								retCode	= DEF_GSM_BTACPT_ERR;	
    						}	
    						else
								{
    								BSP_GSM_ReleaseFlag(DEF_BTDING_FLAG);					// �����������߱�־
										retCode = DEF_GSM_NONE_ERR;		
								}
    				}		
    				else
    				{
    						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTACPT_1,	strlen((const char *)AT_BTACPT_1),	CommandRecOk,	BTACPTTimeout)	!=	DEF_GSM_NONE_ERR)
    						{
    								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
    								retCode	= DEF_GSM_BTACPT_ERR;	
    						}	
    						else
								{
    								BSP_GSM_SetFlag(DEF_BTDING_FLAG);							// �����������߱�־
										retCode = DEF_GSM_NONE_ERR;	
								}
    				}		
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;				
		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTRxdDataPro() 
* Description    : ��ȡBT����
* Input          : *pBuf	:���յ������ݻ�����ָ��
*								 : *len		:���յ������ݳ���
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTRxdDataPro	(u8	*pBuf,	u16	*len,	u32	timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="",tmpBuf1[20]="";
		u16 j=0,k=0,getLen=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{											
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// ��ȡ���ݳ�����Ϣ
						//	Send "AT+BTSPPGET=2\r" AT_BTSPPGET_2		
						tcpReadMuxtex =1;	// �û����־,ʹURC������
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_BTSPPGET_2,	strlen((const char *)AT_BTSPPGET_2),	CommandRecOk,	BTSPPGET2Timeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_BTSPPGET2_ERR;			
						}	
						else
						{
								gsmDelayMS(30);		// ��30ms�����������(��ֹ����HEX�����к���OK���ַ�������ƥ��ɹ���ǰ�˳�)
								//+BTSPPGET: 1,30
								j = 0;
								j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+BTSPPGET: ", s_GSMcomm.ps_commu->RCNT, 11);
								if(j == 0xffff)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode = DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										// ��ȡ������Ϣ
										j = j + 13; // ָ�򳤶���Ϣ
										memset(tmpBuf1,'\0',sizeof(tmpBuf1));
										for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < sizeof(tmpBuf1)) && (k < s_GSMcomm.ps_commu->RCNT);	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k] = '\0';
										getLen = atoi((const char *)tmpBuf1);
										
										// ��ȡ�������
										//	Send "AT+BTSPPGET=3,len\r" AT_BTSPPGET_3_H
										memset(tmpBuf,	'\0',	sizeof(tmpBuf));
        						strcat((char *)tmpBuf,	(char *)AT_BTSPPGET_3_H);
        						strcat((char *)tmpBuf,	(char *)tmpBuf1);
        						strcat((char *)tmpBuf,	"\r");
        						
										// Read							
        						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	BTSPPGET3Timeout) !=	DEF_GSM_NONE_ERR)	
        						{
        								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
        								retCode	=	DEF_GSM_BTSPPGET3_ERR;			
        						}	
        						else
        						{
        								//+BTSPPGET: 1,30,SIMCOMSPPFORAPPSIMCOMSPPFORAPP
        								j = 0;
												j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+BTSPPGET: ", s_GSMcomm.ps_commu->RCNT, 11);
												if(j == 0xffff)	
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
														retCode = DEF_GSM_UNKNOW_ERR;
												}
												else
												{
														// ��ȡ������Ϣ
														j = j + 16; // ָ��������Ϣ
														for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n')  && (k < s_GSMcomm.ps_commu->RCNT);	k++)
														{
																*(pBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
														}
														*(pBuf + k) = '\0';	
														*len =getLen;
														retCode = DEF_GSM_NONE_ERR;
				        				}
										}
								}
						}
						// ��ȡ��Ϻ����������������,��ֹURC����
						memset(RBufAT,	0,	sizeof(RBufAT));
						memset(RBufGPRS,	0,	sizeof(RBufGPRS));
						s_GSMcomm.ps_commu->RCNT =0;
						tcpReadMuxtex =0;	// ���־,ʹ������URC���									
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTSendDataPro() 
* Description    : ����BT����
* Input          : *pBuf	:��Ҫ���͵����ݻ�����ָ��
*								 : len		:�������ݳ���
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTSendDataPro	(u8	*pBuf,	u16	len,	u32	timeout)
{	
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="",tmpBuf1[20]="";
		u16 j=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{											
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// ��ѯ��ǰ����ģʽ���ͻ���ģʽ/������ģʽ��
						//	Send "AT+BTSPPCFG?\r" AT_BTSPPCFG																						
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_BTSPPCFG,	strlen((const char *)AT_BTSPPCFG),	CommandRecOk,	BTSPPCFGTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_BTSPPCFG_ERR;			
						}	
						else
						{
								j = 0;
								j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+BTSPPCFG:", s_GSMcomm.ps_commu->RCNT, 10);
								if(j == 0xffff)		
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode = DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										// ��ȡģʽ��Ϣ
										j = j + 15; // ָ�򳤶���Ϣ
										if(*(s_GSMcomm.pRecBuf + j) != '1')
												retCode	=	DEF_GSM_FAIL_ERR;		// ģʽ����ȷ���ش��󣬲��������ۼ�
										else
										{
												// ���
												//	Send "AT+BTSPPSEND=len" AT_BTSPPSEND_H												
												memset(tmpBuf,	'\0',	sizeof(tmpBuf));
												memset(tmpBuf1,	'\0',	sizeof(tmpBuf1));
		        						strcat((char *)tmpBuf,	(char *)AT_BTSPPSEND_H);		        						
		        						sprintf((char *)tmpBuf1,	"%d\r",	len);	
		        						strcat((char *)tmpBuf,	(char *)tmpBuf1);								
		        						
		        						// Pend">"							
		        						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecBTSendAck,	BTSPPSENDTimeout) !=	DEF_GSM_NONE_ERR)	
		        						{
		        								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
		        								retCode	=	DEF_GSM_BTSPPSEND_ERR;			
		        						}	
		        						else
		        						{
		        								//	Send data
														if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)pBuf,	len,	RecBTSendCpl,	BTSPPSENDCPLTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);	// �����ۼ�
																retCode	=	DEF_GSM_BTSPPSENDCPL_ERR;
														}	
		        								else
		        										retCode = DEF_GSM_NONE_ERR;		// �ļ�д��ɹ�!!		
		        						}																					
										}
								}
						}		
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
*                            					GSM�ϲ�Ӧ�ú���-״̬��ѯ����(��OS Ӧ�ú�������)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_QCSQCodePro() 
* Description    : ��ѯ�ź�ǿ�ȼ�������        
* Input          : ����ź�ǿ��Ϊ99(��ʾ�޷���ȡ)ֱ��д0
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QCSQCodePro	(u8 *pSLevel, u8*pRssi,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8	tmpBuf[20]="";	
		u16	k=0,j=0;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// Send "AT+CSQ"	���ȼ���ź�ǿ��
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CSQ,	strlen((const char *)AT_CSQ),	CommandRecOk,	CSQTimeout)	!=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	= DEF_GSM_CSQ_ERR;	
						}	
						else
						{
								j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CSQ: ",s_GSMcomm.ps_commu->RCNT,6);
								if(j == 0xffff)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_UNKNOW_ERR;	
								}
								else
								{
										j = j + 6;
										// ȡ�ź�ǿ��
										memset(tmpBuf,'\0',sizeof(tmpBuf));
										for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != ',') && (k <= 1);	k++)
										{
												tmpBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										*pSLevel = atoi((const char *)tmpBuf);
										if(*pSLevel == 99)
												*pSLevel = 0;
										
										// ȡ������	
										j = j + k + 1;
										memset(tmpBuf,'\0',sizeof(tmpBuf));	
										for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != '\r') && (k <= 1);	k++)
										{
												tmpBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										*pRssi = atoi((const char *)tmpBuf);	// ȡ������	
										retCode = DEF_GSM_NONE_ERR;
								}											
						}
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QCellGPSPosiPro() 
* Description    : ��ѯ�豸��վ��λ��Ϣ(���ʱʱ��60s)       
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QCellGPSPosiPro	(u8 *pLonBuf, u8 *pLatBuf, u8 *pDTime,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8	tmpBuf1[20]="";
		u16	k=0,j=0;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// ��Ҫ����ָ��
						/*
						//	Send "AT^SMOND"
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_SMOND,	strlen((const char *)AT_SMOND),	CommandRecOk,	SMONDTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_SMOND_ERR;			
						}
						else
						{
								j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"^SMOND:",s_GSMcomm.ps_commu->RCNT,7);
								if(j == 0xffff)	
								{
										retCode	=	DEF_GSM_UNKNOW_ERR;	
								}
								else
								{
										memset((u8 *)tmpBuf1,	'\0',	sizeof(tmpBuf1));

										// ȡ�ƶ����Ҵ���(MCC)
										j = j+7;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 3));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';										
										strcpy((char *)pMCCBuf,	(const char *)tmpBuf1);

										// ȡ�ƶ���Ӫ�̴���(MNC)
										j = j+k+1;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 3));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';
										strcpy((char *)pMNCBuf,	(const char *)tmpBuf1);

										// ȡС������(LAC)
										j = j+k+1;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 4));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';
										strcpy((char *)pLACBuf,	(const char *)tmpBuf1);
										
										// ȡ��վ��(CELL)
										j = j+k+1;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 4));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';
										strcpy((char *)pCELLBuf,	(const char *)tmpBuf1);
										
										
										// ȡ�ź�ǿ�ȼ�������
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT - j));	k++,j++)
										{
										 		;
										}
										j = j+k-1;
										// �ƶ����ź�ǿ�Ȳ�������λ��
										for(;	(*(s_GSMcomm.pRecBuf + j) != ',');	j--)
										{
												;
										}
										j = j+1;
										// ����
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 2));	k++)
										{
												*(pRssiBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
										}
										*(pRssiBuf + k) = '\0';
										j = j-2;
										for(;	(*(s_GSMcomm.pRecBuf + j) != ',');	j--)
										{
												;
										}
										j = j+1;
										// ����
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 2));	k++)
										{
												*(pSLevelBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
										}
										*(pSLevelBuf + k) = '\0';
										if((*pSLevelBuf == '9') && (*(pSLevelBuf + 1) == '9'))
										{
												*pSLevelBuf 		 	= '\0';		// �ź�ǿ�����
												*(pSLevelBuf + 1) = '\0';						
										}	
										retCode	=	DEF_GSM_NONE_ERR;	
								}	
						}		
						*/			
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QSimCardPrintPro() 
* Description    : ��ѯSIM��ӡˢ��(һ��Ϊ20λ����Щ����λ����ͬ)         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QSimCardPrintPro	(u8 *pCnumBuf,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u16	k=0,j=0;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// SIM800 ��֧�ָ�����
						/*
						//	Send "AT^SCID"
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_SCID,	strlen((const char *)AT_SCID),	CommandRecOk,	SCIDTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_SCID_ERR;			
						}
						else
						{
								j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"^SCID: ",s_GSMcomm.ps_commu->RCNT,7);
								if(j == 0xffff)	
								{
										retCode	=	DEF_GSM_UNKNOW_ERR;		
								}
								else
								{
										j= j + 7;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
										{
												*(pCnumBuf +k) = *(s_GSMcomm.pRecBuf + j + k);
										}
										*(pCnumBuf +k) = '\0';
										if((k < 4) || (k > 30))		// �����жϳ��ȷǷ�����ս���������
										{
												*pCnumBuf = '\0';
												retCode	=	DEF_GSM_FAIL_ERR;
										}
										else
												retCode	=	DEF_GSM_NONE_ERR;		
								}	
						}	
						*/				
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	

}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QIMSICodePro() 
* Description    : ��ѯ�����ƶ��豸ǩ��(IMSI)����(һ��Ϊ15λ)         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QIMSICodePro	(u8 *pImsiBuf,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u16	k=0;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						//	Send "AT+CIMI"
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CIMI,	strlen((const char *)AT_CIMI),	CommandRecOk,	CIMITimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_CIMI_ERR;			
						}
						else
						{								
								for(k = 0;	((*(s_GSMcomm.pRecBuf + k + 2) != '\r') && (*(s_GSMcomm.pRecBuf + k + 2) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
								{
										*(pImsiBuf +k) = *(s_GSMcomm.pRecBuf + k + 2);
								}
								*(pImsiBuf +k) = '\0';		
								if((k < 8) || (k > 30))		// IMSI�����жϳ��ȷǷ�����ս���������
								{
										*pImsiBuf = '\0';
										retCode	=	DEF_GSM_CIMI_ERR;
								}
								else
										retCode	=	DEF_GSM_NONE_ERR;		
						}					
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QIMEICodePro() 
* Description    : ��ѯ�����ƶ�̨�豸ʶ����(IMEI)(һ��Ϊ15λ)        
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QIMEICodePro(u8 *pImeiBuf,	u32 timeout)
{
		u16	k=0;
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_GSN,	strlen((const char *)AT_GSN),	CommandRecOk,	GSNTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_GSN_ERR;		
						}	
						else
						{								
								for(k = 0;	((*(s_GSMcomm.pRecBuf + k + 2) != '\r') && (*(s_GSMcomm.pRecBuf + k + 2) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
								{
										*(pImeiBuf +k) = *(s_GSMcomm.pRecBuf + k + 2);
								}
								*(pImeiBuf +k) = '\0';
								if((k < 8) || (k > 30))		// IMEI�����жϳ��ȷǷ�����ս���������
								{
										*pImeiBuf = '\0';
										retCode	=	DEF_GSM_GSN_ERR;
								}
								else
										retCode	=	DEF_GSM_NONE_ERR;
						}	
				}
				else
						retCode	=	DEF_GSM_GSN_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QCPINStatePro() 
* Description    :  ��ѯSIM��״̬       
* Input          : 
* Output         : =0 ready
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QCPINStatePro(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR,ret=0;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ��ѯ��״̬
						ret =BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
						if(ret !=	DEF_GSM_NONE_ERR)
						{
								if(ret == CommandTimeout)
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ֻ�г�ʱ����Ϊ�Ǵ���
								retCode	=	DEF_GSM_CPIN_ERR;
						}
						else
								retCode = DEF_GSM_NONE_ERR;
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QSMSCenterPro() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QSMSCenterPro(u8 *pSmsCenter,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		
		u16 p=0,i=0;
		u8	tmpCSCA[20]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_QCSCA,	strlen((const char *)AT_QCSCA),	CommandRecOk,	CSCATimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_CSCA_ERR;		
						}	
						else
						{
								p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CSCA: \"",s_GSMcomm.ps_commu->RCNT,8);
								if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										p = p + 11;
										memset(tmpCSCA,	'\0',	sizeof(tmpCSCA));
										for(i=0;	*(s_GSMcomm.pRecBuf + p + i) != '\"';	i++)
										{
												tmpCSCA[i]	=		*(s_GSMcomm.pRecBuf + p + i);
										}
										tmpCSCA[i]	=	'\0';

										// д�������ĺ���
										strcpy((char *)pSmsCenter,	(const char *)tmpCSCA);
										retCode = DEF_GSM_NONE_ERR;	
								}
						}	
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QSimCardNumPro() 
* Description    : ��ѯ��������(SIM������֮ǰд��SIM��������ܶ�������Ϊ���ַ�)    
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QSimCardNumPro(u8	*pSimNum,		u32	timeout)
{
		u16	p=0,k=0,i=0;
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CNUM,	strlen((const char *)AT_CNUM),	CommandRecOk,	CNUMTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_CNUM_ERR;		
						}	
						else
						{	
								// +CNUM: "","+18501191946",145,7,4
								p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)",\"",s_GSMcomm.ps_commu->RCNT,2);
								if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
								{
										// ���ؿ��ַ���
										*pSimNum ='\0';
										retCode	=	DEF_GSM_NONE_ERR;										
								}
								else
								{
										p +=2;	// ָ�����
										for(i=0;	(i<19) && (*(s_GSMcomm.pRecBuf + p) != '"');	i++)
										{
												if(*(s_GSMcomm.pRecBuf + p) != '1')		
														p +=1;	// �������ֻ�������ʼ�ֶ���"+86"
												else
														break;	
										}
										for(k = 0;	((k < 19) && (*(s_GSMcomm.pRecBuf + p + k) != '\"') && (*(s_GSMcomm.pRecBuf + p + k) != '\r') && (*(s_GSMcomm.pRecBuf + p + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
										{
												*(pSimNum + k) = *(s_GSMcomm.pRecBuf + p + k);	// ������'+'��							
										}
										*(pSimNum +k) = '\0';
										retCode	=	DEF_GSM_NONE_ERR;
								}
						}	
				}
				else
						retCode =	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_WriteSimCardNumPro() 
* Description    : ���ñ�������,����������д��SIM���绰���У�֮��ͨ��AT+CNUM�ɲ�ѯ��������
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_WriteSimCardNumPro(u8	*pSimNum,		u32	timeout)
{
		u16	p=0,k=0;
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;	
		u8	tmpBuf[30]={0};

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ����ʹ��CNUM��ѯ��������
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CPBS_ON,	strlen((const char *)AT_CPBS_ON),	CommandRecOk,	CPBSTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_CPBS_ERR;		
						}	
						else
						{	
								// ����������д��SIM���绰��
								// ���
								//	Send "AT+CPBW=1,\"13145678901\",145"												
								memset(tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CPBW_H);		        						
								strcat((char *)tmpBuf,	(char *)pSimNum);	
								strcat((char *)tmpBuf,	(char *)"\",145\r");
														
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CPBWTimeout) !=	DEF_GSM_NONE_ERR)	
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_CPBW_ERR;			
								}	
								else
										retCode	=	DEF_GSM_NONE_ERR;
						}	
				}
				else
						retCode =	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
*                            					GSM�ϲ�Ӧ�ú���-SIM����ʼ������(��OS Ӧ�ú�������)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMSimCardInitPro() 
* Description    :  SIM����ع��ܳ�ʼ������     
* Input          : 
*								 : 
*								 : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMSimCardInitPro (u8	*pSimNum,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;		
		s8	retCode=DEF_GSM_NONE_ERR,ret=0;
		u16 p=0,k=0,i=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ��ѯ��ǰSIM��״̬
						// Send "AT+CPIN?"
						ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
						if(ret !=	DEF_GSM_NONE_ERR)
						{
								if(ret == CommandTimeout)
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ֻ�г�ʱ����Ϊ�Ǵ���
								retCode	=	DEF_GSM_CPIN_ERR;
						}
						else
						{
								// SIM����ʼ������
							  //AT_CMGF,			// ��ҪSIM��֧��								
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF,	strlen((const char *)AT_CMGF),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_CMGF_ERR;
								}
								else
								{
										//AT_CSCS_GSM,	// ��ҪSIM��֧��								
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												retCode	=	DEF_GSM_CSCS_ERR;
										}
										else
										{
												//AT_CNMI,			// ��ҪSIM��֧��								
												if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CNMI,	strlen((const char *)AT_CNMI),	CommandRecOk,	CNMITimeout) !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
														retCode	=	DEF_GSM_CNMI_ERR;
												}
												else
												{
														//AT_CPMS,			// ��ҪSIM��֧��
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CPMS,	strlen((const char *)AT_CPMS),	CommandRecOk,	CPMSTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																retCode	=	DEF_GSM_CPMS_ERR;
														}
														else
														{
																// ��ȡSIM������
																if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CNUM,	strlen((const char *)AT_CNUM),	CommandRecOk,	CNUMTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CNUM_ERR;		
																}	
																else
																{	
																		// +CNUM: "","+18501191946",145,7,4
																		p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)",\"",s_GSMcomm.ps_commu->RCNT,2);
																		if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																		{
																				// ���ؿ��ַ���
																				*pSimNum ='\0';
																				retCode	=	DEF_GSM_NONE_ERR;										
																		}
																		else
																		{
																				p +=2;	// ָ�����		
																				if(*(s_GSMcomm.pRecBuf + p) == '+')		
																						p +=1;	// �������ֻ�������ʼ�ֶ���"+"																			
																				for(k = 0;	((k < 19) && (*(s_GSMcomm.pRecBuf + p + k) != '\"') && (*(s_GSMcomm.pRecBuf + p + k) != '\r') && (*(s_GSMcomm.pRecBuf + p + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
																				{
																						*(pSimNum + k) = *(s_GSMcomm.pRecBuf + p + k);	// ������'+'��							
																				}
																				*(pSimNum +k) = '\0';
																				retCode	=	DEF_GSM_NONE_ERR;
																		}
																}	
														}
												}
										}
								}
						}											
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;
		}
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
*                            					GSM�ϲ�Ӧ�ú���-���粿��(��OS Ӧ�ú�������)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  :									  BSP_GPRSFroceBreakSet() 
* Description    :  �ֶ�ǿ������Ϊ����Ͽ�״̬�����������Զ���������      
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GPRSFroceBreakSet(void)
{
		s_GSMcomm.GSMComStatus	=	GSM_ONCMD;
		BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);		// ������Ͽ�״̬
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GPRSResetPro() 
* Description    :  ��λGPRS����ʹ�����¸��ŵ�GPRS������(��������������ʧ����Ҫ��λGPRS����)        
* Input          : *pApn	: APN�ַ���������ָ�룬������Ϊ��
*								 : *pUser : ��¼�û����ַ���ָ�룬����Ϊ��
*								 : *pPass : ��¼�����ַ���ָ�룬����Ϊ��
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GPRSResetPro(u8 *pApn,	u8	*pUser,	 u8  *pPass,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;		
		u8 i=0,tmpBuf[80]={0},	cregOKFlag=0,	cgattOKFlag=0;
		s8	tmpRen=0,retCode=DEF_GSM_NONE_ERR,ret=0;
		u16 j=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// �����κϷ���
				if(*pApn == '\0')
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{			
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								// ��ѯ��ǰSIM��״̬
								// Send "AT+CPIN?"
								ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
								if(ret !=	DEF_GSM_NONE_ERR)
								{
										if(ret == CommandTimeout)
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ֻ�г�ʱ����Ϊ�Ǵ���
										retCode	=	DEF_GSM_CPIN_ERR;
								}
								else
								{
										// ��ѯ��ǰ����ע��״̬�Ƿ����	
										//	Send "AT+CREG?"
										if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CREG,	strlen((const char *)AT_CREG),	CommandRecOk,	CREGTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ��ѯ����ע�����
												retCode	=	DEF_GSM_CREG_ERR;
										}
										else
										{
												j= 0;
												j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CREG: *,*",s_GSMcomm.ps_commu->RCNT,10);
												if(j != 0xffff)	
												{
														j =j+9; // ָ��ע��״̬
														if((*(s_GSMcomm.pRecBuf + j) == '1') || (*(s_GSMcomm.pRecBuf + j) == '5'))
																cregOKFlag =1;	// ע��ɹ�����			
														else
																cregOKFlag =0;
												}
												else
														cregOKFlag =0;
										}
										
										// ��ѯ��ǰGPRS�������
										if(cregOKFlag == 1)
										{												
												if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CGATT,	strlen((const char *)AT_CGATT),	CommandRecOk,	CGATTTimeout) !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// GPRS��ѯ����
														retCode	=	DEF_GSM_CGATT_ERR;
												}
												else
												{
														j= 0;
														j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CGATT: *",s_GSMcomm.ps_commu->RCNT,9);
														if(j != 0xffff)	
														{
																j =j+8; // ָ����״̬
																if(*(s_GSMcomm.pRecBuf + j) == '1')
																		cgattOKFlag =1;	// ע��ɹ�����	
																else
																		cgattOKFlag =0;	
														}
														else
																cgattOKFlag =0;
												}
										}
										
										// ��ʼ����GPRS
										if((cregOKFlag == 0) || (cgattOKFlag == 0))
										{		
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�(2015-3-10�����Է�����ʱCGATT��ʱ�䷵��0״̬���ı�)
												if(cregOKFlag == 0)
														retCode	=	DEF_GSM_CREG_ERR;			
												else
														retCode	=	DEF_GSM_CGATT_ERR;	
										}
										else
										{											
												// ���ȶϿ���ǰPDP������(����3��)
												//	Send "AT+CIPSHUT\r"
												tmpRen =0;
												for(i=0;	i<3;	i++)
												{
														tmpRen	=	 BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CIPSHUT,	strlen((const char *)AT_CIPSHUT),	RecShutCpl,	CIPSHUTTimeout);
														if(tmpRen == DEF_GSM_NONE_ERR)	
																break;
												}
												if(i >=	3)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
														retCode	=	DEF_GSM_CIPSHUT_ERR;
												}
												else
												{																											
														s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ��GSM�˿�״̬
														BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	// ���µ�ǰ����״̬Ϊ�Ͽ�����
																																							
														// ��ʼGPRS��������
														// ����ʹ�ܶ���·ģʽ
														//	Send "AT+CIPMUX=1\r"
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIPMUX_1,	strlen((const char *)AT_CIPMUX_1),	CommandRecOk,	CIPMUXTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																retCode	=	DEF_GSM_CIPMUX_ERR;
														}		
														else
														{
																// ���ý�������ģʽ�ֶ�ģʽ����ָ�������GPRS�����ļ���ǰ���ã�
																//	Send "AT+CIPRXGET=1\r"
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIPRXGET_1,	strlen((const char *)AT_CIPRXGET_1),	CommandRecOk,	CIPRXGET1Timeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CIPRXGET1_ERR;
																}		
																else
																{
																		// ���ý���URC��ʽ
																		//	Send "AT+CIPHEAD=1\r"
																		if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIPHEAD_1,	strlen((const char *)AT_CIPHEAD_1),	CommandRecOk,	CIPHEADTimeout) !=	DEF_GSM_NONE_ERR)
																		{
																				BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																				retCode	=	DEF_GSM_CIPHEAD_ERR;
																		}
																		else
																		{																			
																				// ����APN,USER,PASS
																				//	Send "AT+CSTT="CMNET","",""\r"
																				memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																				strcat((char *)tmpBuf,	(char *)AT_CSTT_H);
																				strcat((char *)tmpBuf,	(char *)pApn);																		
																				strcat((char *)tmpBuf,	(char *)"\",\"");
																				strcat((char *)tmpBuf,	(char *)pUser);																		
																				strcat((char *)tmpBuf,	(char *)"\",\"");
																				strcat((char *)tmpBuf,	(char *)pPass);																		
																				strcat((char *)tmpBuf,	(char *)"\"\r");
																				
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CSTTTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																						retCode	=	DEF_GSM_CSTT_ERR;
																				}
																				else
																				{
																						// ����PDP������
																						//	Send "AT+CIICR\r"
																						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIICR,	strlen((const char *)AT_CIICR),	CommandRecOk,	CIICRTimeout) !=	DEF_GSM_NONE_ERR)
																						{
																								//BSP_GSM_ERR_Add(&err_Gsm.cnnErrorTimes,	1);		// ��������ۼ�
																								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	20);		// 2015-11-19�ô���ϲ���ͨ�ô�����
																								retCode	=	DEF_GSM_CIICR_ERR;
																						}
																						else
																						{	
																								// ��ȡ����IP����ָ�ȫ�ֽ�ƥ�����룩
																								//	Send "AT+CIFSR\r"
																								if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CIFSR,	strlen((const char *)AT_CIFSR),	RecCIFSRAck,	CIFSRTimeout) !=	DEF_GSM_NONE_ERR)
																								{
																										//BSP_GSM_ERR_Add(&err_Gsm.cnnErrorTimes,	1);		// ��������ۼ�
																										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 2015-11-19�ô���ϲ���ͨ�ô�����
																										retCode	= DEF_GSM_CIFSR_ERR;	
																								}
																								else
																										retCode = DEF_GSM_NONE_ERR;																							
																						}							
																				}																																		
																		}		
																}																						
														}	
												}
										}
								}
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPConnectPro() 
* Description    : sCnn		: ͨ���ţ���0��~��5����6·���ӣ��ַ����ͣ�
*								 : *pIp		: IP��ַ�ַ���ָ��
*								 : *pPort	: �˿ں��ַ���ָ��         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPConnectPro	(u8	sCnn,	u8 *pIp, u8 *pPort,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR,tmpRen=0;
		u8	tmpBuf[60]="";		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				// �����κϷ���
				if(((sCnn - 0x30) > DEF_GPRS_CNN_MAX) || (*pIp == '\0') || (*pPort == '\0'))
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{				
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{																																	
								// ���
								//	Send "AT+CIPSTART=0,\"TCP\",\"218.249.201.35\",\"13000\"\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPSTART_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);										
								strcat((char *)tmpBuf,	(char *)",\"TCP\",\"");
								strcat((char *)tmpBuf,	(char *)pIp);
								strcat((char *)tmpBuf,	(char *)"\",\"");
								strcat((char *)tmpBuf,	(char *)pPort);
								strcat((char *)tmpBuf,	(char *)"\"\r");
												
								tmpRen = BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecConnectCpl,	CIPSTARTTimeout); 
								if(tmpRen !=	DEF_GSM_NONE_ERR)
								{
										// ����������	
										BSP_GSM_ERR_Add(&err_Gsm.cnnErrorTimes,	1);		// �����ۼ�
										if(BSP_GSM_ERR_Que(&err_Gsm.cnnErrorTimes) >= DEF_GSM_CNN_TIMES)
										{
												BSP_GSM_ERR_Clr(&err_Gsm.cnnErrorTimes);		// �������	
												
												// ��Ҫ���¸���GPRS����(AT+CGATT=0��AT+CGATT=1)
												tmpRen = BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CGATT_0,	strlen((const char *)AT_CGATT_0),	CommandRecOk,	CGATT0Timeout); 
												if(tmpRen !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
														retCode	=	DEF_GSM_CGATT_ERR;				
												}
												else
												{
														tmpRen = BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CGATT_1,	strlen((const char *)AT_CGATT_1),	CommandRecOk,	CGATT1Timeout); 
														if(tmpRen !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																retCode	=	DEF_GSM_CGATT_ERR;				
														}
												}
										}	

										s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ��GSM�˿�״̬
										BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);
										retCode	=	DEF_GSM_CIPSTART_ERR;
								}
								else
								{										
										BSP_GSM_ERR_Clr(&err_Gsm.cnnErrorTimes);		// �������				
										s_GSMcomm.GSMComStatus	=	GSM_ONLINE;	// ��GSM�˿�״̬
										BSP_GSM_SetFlag(DEF_GDATA_FLAG);																									
										retCode	=	DEF_GSM_NONE_ERR;
								}
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPCloseNetPro() 
* Description    : sCnn		: ͨ���ţ���0��~��5����6·���ӣ��ַ����ͣ�      
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPCloseNetPro	(u8	sCnn,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8	tmpBuf[20]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				// �����κϷ���
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{																							
								// ���
								//	Send "AT+CIPCLOSE=n\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPCLOSE_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);	
								strcat((char *)tmpBuf,	(char *)"\r");
												
								if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCloseCpl,	CIPCLOSETimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_CIPCLOSE_ERR;
										
										// ���Կ���������ζ������ɹ�ֱ�ӿ��ǶϿ�PDP����
								}
								else
								{
										s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ��GSM�˿�״̬
										BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	
										retCode	=	DEF_GSM_NONE_ERR;		
								}				
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPShutNetPro() 
* Description    :      
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPShutNetPro	(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{		
				if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CIPSHUT,	strlen((const char *)AT_CIPSHUT),	RecShutCpl,	CIPSHUTTimeout) != DEF_GSM_NONE_ERR)
				{
						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
						retCode	=	DEF_GSM_CIPSHUT_ERR;
				}
				else
				{
						s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ��GSM�˿�״̬
						BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	
						retCode	=	DEF_GSM_NONE_ERR;		
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPSendData() 
* Description    : sCnn		: ͨ���ţ���0��~��5����6·���ӣ��ַ����ͣ�
*								 : *pBuf	: �������ݻ�����ָ��     
*								 : len		: ���ͳ���    
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPSendData(u8	sCnn,	u8	*pBuf,	u16	len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	i=0,	tmpBuf[30]="",	lenBuf[8]="",	ready2Send=0;
		s8	tmpRen=0,	retCode=DEF_GSM_NONE_ERR;	
		u16 j=0,k=0,avaLen=0,posi=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// �����κϷ���
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{					
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	==	GSM_ONLINE))
						{								
								// ��ѯ�ɷ��͵����ݳ���
								// Send "AT+CIPSEND?\r"
								// ���ظ�ʽΪ����·ģʽ����
								// +CIPSEND: 0,1460
								// +CIPSEND: 1,1460
								// +CIPSEND: 2,0
								// +CIPSEND: 3,0
								// +CIPSEND: 4,0
								// +CIPSEND: 5,0
								// 
								// OK
								/*
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CIPSENDQ,	strlen((const char *)AT_CIPSENDQ),	CommandRecOk,	CIPSENDQTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	= DEF_GSM_CIPSENDQ_ERR;			
								}
								else
								{										
										for(i=0;	i<=DEF_GPRS_CNN_MAX;	i++)
										{
												j=0;
												j= StringSearch((u8 *)(s_GSMcomm.pRecBuf + posi),(u8 *)"+CIPSEND: *,*",s_GSMcomm.ps_commu->RCNT,13);
												if(j != 0xffff)		
											  {
											  		j =j+10;	// ȡͨ����
											  		if(sCnn == *(s_GSMcomm.pRecBuf + j))
											  		{
											  				j =j+2;	// ȡ�ɷ������ݴ�С
											  				memset((u8 *)lenBuf,	'\0',	sizeof(lenBuf));
											  				for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < 5));	k++)
																{
																		lenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
																}
																lenBuf[k]='\0';	
											  			 	avaLen = atoi((const char *)lenBuf);
											  			 	if(len < avaLen)
																		ready2Send =1;
											  			 	break;
											  		}
											  		else
											  		{
											  				// ƫ�Ƽ���ָ�뵽��һ��
											  				for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
																{
																		;
																}
																posi = posi + k + 2; // �ƶ����ݽ�����bufָ��											  			
											  		}
											  }
											  else
											  		break;
										}
										if(i > DEF_GPRS_CNN_MAX)
										{
												;		// �쳣����ͨ������ƥ�䣡��������														
										}
								}
								*/
								
								ready2Send =1;	// ȡ�����ͻ�������ѯ	
								// ������������//////////////////////////////////////////////
								if(ready2Send != 1)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �쳣����ͨ������ƥ�䣡�������
										s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	
										BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);					// �Ͽ�����,��״̬�������ӷ�����										�				
										retCode =	DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										// ������Ҫ����ȷ�Ͽ��Է��͵����ݳ����Ƕ���
										// ���								
										//	Send "AT+CIPSEND=n,len\r"									
										memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
										memset((u8 *)lenBuf,	'\0',	sizeof(lenBuf));
										strcat((char *)tmpBuf,	(char *)AT_CIPSEND_H);
										strcat((char *)tmpBuf,	(char *)&sCnn);
										strcat((char *)tmpBuf,	(char *)",");
										sprintf((char *)lenBuf,"%d",	len);
										strcat((char *)tmpBuf,	(char *)lenBuf);
										strcat((char *)tmpBuf,	(char *)"\r");
										
										tmpRen = BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecTCPSendAck,	CIPSENDATimeout); 
										if(tmpRen !=	DEF_GSM_NONE_ERR)
										{
												// �������Ͽ�,��״̬�������ӷ�����
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												s_GSMcomm.GSMComStatus	=	GSM_ONCMD;
												BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);
												retCode	= DEF_GSM_CIPSENDA_ERR;			
										}
										else
										{
												// SEND DATA
												tmpRen = BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)pBuf,	len,	RecTCPSendCpl,	CIPSENDFTimeout);	
												if(tmpRen	!=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�														
														s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	
														BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	// �������Ͽ�,��״̬�������ӷ�����
														retCode	= DEF_GSM_CIPSENDF_ERR;
												}
												else
												{
														// �������ݳɹ�����λͨ�ô����ۼӼ�����
														BSP_GSM_ERR_Clr(&err_Gsm.gsmErrorTimes);
														retCode = DEF_GSM_NONE_ERR;	
												}						
										}
								}
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPRxdData() 
* Description    : sCnn		: ͨ���ţ���0��~��5����6·���ӣ��ַ����ͣ���дtcpReadMuxtex����״̬
*								 : *pBuf	: �������ݻ�����ָ��     
*								 : len		: ���ճ���             
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPRxdData(u8 sCnn,	u8	*pBuf,	u16	*len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8  tmpBuf[30]="",tmpLenBuf[8]="";
		s8	retCode=DEF_GSM_NONE_ERR;	
		u16	j=0,k=0,p=0,tmpLen=DEF_GPRS_RXDONE_SIZE,	nrLen=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// �����κϷ���
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{												
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	== GSM_ONLINE))
						{						
								// ���ֱ�Ӷ�ȡ����
								//	Send "AT+CIPRXGET=2,1,1000\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPRXGET_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);	
								strcat((char *)tmpBuf,	(char *)",");	
								sprintf((char *)tmpLenBuf,	"%d",	tmpLen);	// ��ͷ�ļ��궨�����DEF_GPRS_RXDONE_SIZE						
								strcat((char *)tmpBuf,	(char *)tmpLenBuf);
								strcat((char *)tmpBuf,	(char *)"\r");		
																				
								tcpReadMuxtex =1;	// �û����־,ʹURC������
								
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CIPRXGET2Timeout) != DEF_GSM_NONE_ERR) 
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_CIPGET2_ERR;	
								}
								else
								{
										j = 0;
										j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 2,*",s_GSMcomm.ps_commu->RCNT,14);
										if(j == 0xffff)		
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												retCode	= DEF_GSM_UNKNOW_ERR;											
										}
										else
										{
												// ȡ���������ݳ���(+CIPRXGET: 2,0,6,0)
												memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
												j = j + 15;
												for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
												{
														tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
												}
												tmpLenBuf[k] = '\0';
												tmpLen = atoi((const char *)tmpLenBuf);
												
												// ȡʣ���ȡ���ֽڳ���
												j = j + k + 1;	// ����','
												for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
												{
														tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);	
												}
												tmpLenBuf[k] = '\0';
												nrLen = atoi((const char *)tmpLenBuf);
												if(nrLen != 0)
												{
														#if(DEF_GSMINFO_OUTPUTEN > 0)
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("[GPRS]: Read abnormal, %d byte need to be read!\r\n",	nrLen);
														#endif 											
												}
												
												// ��������
												j = j + k + 2;	// �����س�����ָ��������
												// �жϽ��������в���Ϊ0d 0a���Ϊhex���ݸ�ʽ���������п�����0d 0a���Գ������ݸ��Ʋ�ȫ���
												for(k =0;	((k < tmpLen) && (k < DEF_GPRS_RXDONE_SIZE));	k++)
												{
														*(pBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
												}
												*(pBuf + k) = '\0';	
												*len = tmpLen;	
												retCode	= DEF_GSM_NONE_ERR;
										}					
								}	
								// ��ȡ��Ϻ����������������,��ֹURC����				
								tcpReadMuxtex =0;	// ���־,ʹ������URC���						
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPRxdDataUpdata() 
* Description    : sCnn		: ͨ���ţ���0��~��5����6·���ӣ��ַ����ͣ���дtcpReadMuxtex����״̬,�������������д������ݵĽ���
*								 : *pBuf	: �������ݻ�����ָ��     
*								 : len		: ���ճ���             
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPRxdDataUpdata(u8 sCnn,	u8	*pBuf,	u16	*len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8  tmpBuf[30]="",tmpLenBuf[8]="";
		s8	retCode=DEF_GSM_NONE_ERR;	
		u16	j=0,k=0,p=0,tmpLen=DEF_GPRS_RXDONE_SIZE,	nrLen=0,	tmpRxdCnt=0,	u16Times=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// �����κϷ���
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{												
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	== GSM_ONLINE))
						{	
								// �Ȳ�ѯ�յ������ݳ���
								//	Send "AT+CIPRXGET=4,id\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPRXGET4_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);	
								strcat((char *)tmpBuf,	(char *)"\r");	
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CIPRXGET2Timeout) != DEF_GSM_NONE_ERR) 
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_CIPGET2_ERR;	
								}
								else
								{										
										j = 0;
										j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 4,*",s_GSMcomm.ps_commu->RCNT,14);
										if(j == 0xffff)		
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												retCode	= DEF_GSM_UNKNOW_ERR;													
										}
										else
										{
												// ȡ���������ݳ���(+CIPRXGET: 4,0,len)
												tmpLen =0;
												memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));												
												j = j + 15;
												for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
												{
														tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
												}
												tmpLenBuf[k] = '\0';
												tmpLen = atoi((const char *)tmpLenBuf);
												if(tmpLen < 1040)
														gsmDelayMS(200);
												
												// ���ֱ�Ӷ�ȡ����
												//	Send "AT+CIPRXGET=2,1,1000\r"
												tmpLen=DEF_GPRS_RXDONE_SIZE;	// д�������ȡ����
												memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
												memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
												strcat((char *)tmpBuf,	(char *)AT_CIPRXGET_H);
												strcat((char *)tmpBuf,	(char *)&sCnn);	
												strcat((char *)tmpBuf,	(char *)",");	
												sprintf((char *)tmpLenBuf,	"%d",	tmpLen);	// ��ͷ�ļ��궨�����DEF_GPRS_RXDONE_SIZE						
												strcat((char *)tmpBuf,	(char *)tmpLenBuf);
												strcat((char *)tmpBuf,	(char *)"\r");		
																									
												tcpReadMuxtex =1;	// �û����־,ʹURC������												
												if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CIPRXGET2Timeout) != DEF_GSM_NONE_ERR) 
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
														retCode	=	DEF_GSM_CIPGET2_ERR;	
												}
												else
												{
														// ���ս����жϴ���(��ֹ����HEX�����к���OK���ַ�������ƥ��ɹ���ǰ�˳�)
														tmpRxdCnt =s_GSMcomm.ps_commu->RCNT;	// ��¼��ǰ���յ��ֽڳ���
														u16Times =250;	// ���ȴ���ʱ500ms
														while(u16Times--)
														{
																gsmDelayMS(2);
																if(tmpRxdCnt == s_GSMcomm.ps_commu->RCNT)
																		break;
																else
																		tmpRxdCnt = s_GSMcomm.ps_commu->RCNT;	// ���¼�¼����
														}
														j = 0;
														j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 2,*",s_GSMcomm.ps_commu->RCNT,14);
														if(j == 0xffff)		
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																retCode	= DEF_GSM_UNKNOW_ERR;											
														}
														else
														{
																// ȡ���������ݳ���(+CIPRXGET: 2,0,6,0)
																tmpLen =0;
																memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
																j = j + 15;
																for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
																{
																		tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
																}
																tmpLenBuf[k] = '\0';
																tmpLen = atoi((const char *)tmpLenBuf);
																
																// ȡʣ���ȡ���ֽڳ���
																j = j + k + 1;	// ����','
																for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
																{
																		tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);	
																}
																tmpLenBuf[k] = '\0';
																nrLen = atoi((const char *)tmpLenBuf);
																if(nrLen != 0)
																{
																		#if(DEF_GSMINFO_OUTPUTEN > 0)
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				myPrintf("[GPRS]: Read abnormal, %d byte need to be read!\r\n",	nrLen);
																		#endif 											
																}
																
																// ��������
																j = j + k + 2;	// �����س�����ָ��������
																// �жϽ��������в���Ϊ0d 0a���Ϊhex���ݸ�ʽ���������п�����0d 0a���Գ������ݸ��Ʋ�ȫ���
																for(k =0;	((k < tmpLen) && (k < DEF_GPRS_RXDONE_SIZE));	k++)
																{
																		*(pBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
																}
																*(pBuf + k) = '\0';	
																*len = tmpLen;	
																
																// �ж��Ƿ�������δ��ȡ
																if(nrLen != 0)
																		retCode = DEF_GSM_READ_AGAIN;
																else
																{
																		// �ٴ��ж��Ƿ������ݽ��յ�
																		j = j + k;	// ����������
																		if((s_GSMcomm.ps_commu->RCNT -j) > 0)
																		{
																				p = StringSearch((u8 *)(s_GSMcomm.pRecBuf+j),(u8 *)"+CIPRXGET: 1,*",(s_GSMcomm.ps_commu->RCNT-j),14);		
																				if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
																				{
																						*(s_GSMcomm.pRecBuf + p) = '\0';										// ɾ���ַ�����ֹͬһ�ַ����ظ����
																						#if(DEF_GSMINFO_OUTPUTEN > 0)
																						if(dbgInfoSwt & DBG_INFO_GPRS)
																								myPrintf("[GPRS]: Rxd data end with \"+CIPRXGET: 1,n\"!\r\n",	nrLen);
																						#endif 																			
																						retCode = DEF_GSM_READ_AGAIN;
																				}
																				else
																						retCode	= DEF_GSM_NONE_ERR;
																		}
																		else
																				retCode	= DEF_GSM_NONE_ERR;
																}
														}					
												}	
												// ��ȡ��Ϻ����������������,��ֹURC����
												s_GSMcomm.ps_commu->RCNT =0;
												tcpReadMuxtex =0;	// ���־,ʹ������URC���		
										}
								}
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
*                            					GSM�ϲ�Ӧ�ú���-���Ų���(��OS Ӧ�ú�������)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSSend() 
* Description    : ���ж��Ž���USC2���뷽ʽ���ͣ������ڲ��Ὣ���͵�ASCII��ת��USC2��(Ŀ�ı���GSM�ַ�����һЩ����
*									 �ַ��磺@,$�������޷�ʶ��) [ע]Ĭ�ϲ����Ͷ������ĺ�����ǲ����ֶ����������ĺ���       
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_SMSSend(u8	smsMode,	u8	*pSmsCenter,	u8	*pPhoneCode,	u8	*pData,	u16	len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[200]="", tmpUSC2Buf[1024]="",	lenBuf[30]="";	
		u16	tmplen;
		s8	retCode=DEF_GSM_NONE_ERR,ret=0;
		u8 *pPhone=pPhoneCode;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// �����κϷ���
				if((*pPhoneCode == '\0') || (len == 0))
				{
						retCode =	DEF_GSM_NONE_ERR;	
				}
				else
				{							
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>= GSM_ONCMD))
						{
								// ��ѯ��ǰSIM��״̬
								// Send "AT+CPIN?"
								ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
								if(ret !=	DEF_GSM_NONE_ERR)
								{
										if(ret == CommandTimeout)
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ֻ�г�ʱ����Ϊ�Ǵ���
										retCode	=	DEF_GSM_CPIN_ERR;
								}
								else
								{
										if(smsMode	==	DEF_GSM_MODE)	
										{
												if(len	>	DEF_SMS_USC2BYTE_SIZE)
												{
														retCode	=	DEF_GSM_NONE_ERR;				
												}
												else
												{
														//	Text Mode
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_1,	strlen((const char *)AT_CMGF_1),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{						
																//	"USC2" Mode ���������ַ�GSMģʽ�������յ���ʶ��
																//	Send AT+CSCS
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CSCS_ERR;
																}
																else
																{													
																		memset((u8 *)tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);
																		strcat((char *)tmpBuf,	(char *)"\"");	// copy "
																		tmplen = ChangeASCII2USC2 (pPhoneCode,	strlen((const char *)pPhoneCode),	tmpUSC2Buf);
																		strcat((char *)tmpBuf,	(char *)tmpUSC2Buf);
																		strcat((char *)tmpBuf,	(char *)"\"\r");																	
																		if(tmplen == 0xffff)
																				retCode	=	DEF_GSM_UNKNOW_ERR;		
																		else
																		{																																
																				//	Send AT+CMGS
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																						retCode	=	DEF_GSM_CMGSACK_ERR;		
																				}
																				else
																				{																
																						memset((u8 *)tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																						tmplen = ChangeASCII2USC2 (pData,	len,	tmpUSC2Buf);
																						strcat((char *)tmpUSC2Buf,	(char *)"\x1a");	
																						if(tmplen == 0xffff)
																								retCode	=	DEF_GSM_UNKNOW_ERR;	
																						else
																						{
																								//	Send SMS data
																								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																								{
																										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																										retCode	=	DEF_GSM_CMGSCPL_ERR;
																								}
																						}
																				}	
																		}
																}	
																//	Send AT+CSCS="GSM" �лص�GSM���뷽ʽ��ֹ�ٴ������������ʱ���뼯��ΪUCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CSCS_ERR;
																}	
														}
												}			
										}
										else
										{
												if(len	> (DEF_SMS_USC2BYTE_SIZE*4))
												{
														retCode	=	DEF_GSM_NONE_ERR;		// �������SMS���ͳ�������Ϊ���ͳɹ� 						
												}
												else
												{
														//  PDU �����Ĵ���
														//	PDU	Mode						
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_0,	strlen((const char *)AT_CMGF_0),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{								
																// 	"UCS2" Mode 
																//	Send AT+CSCS		
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CSCS_ERR;		
																}
																else
																{
																		if(*pPhone == '+')
																				pPhone ++;		 // �����绰�����е�'+'����(PDU��������"+"��)
																						
																		//	Make Pdu data and return the len of data
																		tmplen = PDUMakeUpData(DEF_COM_PHONE, (u8	*)pSmsCenter,	(u8	*)pPhone, (u8	*)pData, len,	(u8	*)tmpUSC2Buf);			
																		sprintf((char *)lenBuf,	"%d",	tmplen);
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);
																		strcat((char *)tmpBuf,	(char *)lenBuf);
																		strcat((char *)tmpBuf,	(char *)"\r");
																		
																		if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																		{
																				BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																				retCode	=	DEF_GSM_CMGSACK_ERR;				
																		}
																		else
																		{
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																						retCode	=	DEF_GSM_CMGSCPL_ERR;				
																				}		
																		}
																}
																//	Send AT+CSCS="GSM" �лص�GSM���뷽ʽ��ֹ�ٴ������������ʱ���뼯��ΪUCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CSCS_ERR;
																}	
														}
												}
										}
								}
						}
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSSendAlarm() 
* Description    : ���ͱ������Ÿö��Ż���ԭ�����绰�����ϼ�����Ҵ��룬DEF_NATION_CODE(��MAIN.H�ж���)        
* Input          : ���ж��Ž���USC2���뷽ʽ���ͣ������ڲ��Ὣ���͵�ASCII��ת��USC2��(Ŀ�ı���GSM�ַ�����һЩ����
*									 �ַ��磺@,$�������޷�ʶ��)[ע]Ĭ�ϲ����Ͷ������ĺ�����ǲ����ֶ����������ĺ���  
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_SMSSendAlarm(u8	smsMode,	u8	*pSmsCenter,	u8	*pPhoneCode,	u8	*pData,	u16	len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[200]="", tmpUSC2Buf[1024]="",	lenBuf[30]="",	tmpPhoneBuf[30]="";		
		u16	tmplen;
		s8	retCode=DEF_GSM_NONE_ERR,ret=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// �����κϷ���
				if((*pPhoneCode == '\0') || (len == 0))
				{
						retCode =	DEF_GSM_NONE_ERR;		// ��ʽӦ�ó������ڲ����޴����³������Է���
				}
				else
				{			
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								// ��ѯ��ǰSIM��״̬
								// Send "AT+CPIN?"
								ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
								if(ret !=	DEF_GSM_NONE_ERR)
								{
										if(ret == CommandTimeout)
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ֻ�г�ʱ����Ϊ�Ǵ���
										retCode	=	DEF_GSM_CPIN_ERR;
								}
								else
								{
										if(smsMode	==	DEF_GSM_MODE)
										{
												if(len	>	DEF_SMS_USC2BYTE_SIZE)
												{
														retCode	=	DEF_GSM_NONE_ERR;		// �������SMS���ͳ�������Ϊ���ͳɹ� 			
												}
												else
												{
														//	Text Mode
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_1,	strlen((const char *)AT_CMGF_1),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{						
																//	"USC2" Mode ���������ַ�GSMģʽ�������յ���ʶ��
																//	Send AT+CSCS
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CSCS_ERR;
																}
																else
																{													
																		memset((u8 *)tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);
																		strcat((char *)tmpBuf,	(char *)"\"");	// copy "
																		tmplen = ChangeASCII2USC2 (pPhoneCode,	strlen((const char *)pPhoneCode),	tmpUSC2Buf);
																		strcat((char *)tmpBuf,	(char *)tmpUSC2Buf);
																		strcat((char *)tmpBuf,	(char *)"\"\r");																	
																		if(tmplen == 0xffff)
																				retCode	=	DEF_GSM_UNKNOW_ERR;		
																		else
																		{																																
																				//	Send AT+CMGS
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																						retCode	=	DEF_GSM_CMGSACK_ERR;		
																				}
																				else
																				{																
																						memset((u8 *)tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																						tmplen = ChangeASCII2USC2 (pData,	len,	tmpUSC2Buf);
																						strcat((char *)tmpUSC2Buf,	(char *)"\x1a");	
																						if(tmplen == 0xffff)
																								retCode	=	DEF_GSM_UNKNOW_ERR;	
																						else
																						{
																								//	Send SMS data
																								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																								{
																										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																										retCode	=	DEF_GSM_CMGSCPL_ERR;
																								}
																						}
																				}	
																		}	
																}
																//	Send AT+CSCS="GSM" �лص�GSM���뷽ʽ��ֹ�ٴ������������ʱ���뼯��ΪUCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CSCS_ERR;
																}			
														}
												}			
										}
										else
										{
												if(len	> (DEF_SMS_USC2BYTE_SIZE*4))
												{
														retCode	=	DEF_GSM_NONE_ERR;		// �������SMS���ͳ�������Ϊ���ͳɹ� 						
												}
												else
												{
														//  PDU �����Ĵ���
														//	PDU	Mode						
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_0,	strlen((const char *)AT_CMGF_0),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{								
																// 	"UCS2" Mode 
																//	Send AT+CSCS		
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CSCS_ERR;		
																}
																else
																{																									
																		memset((u8 *)tmpPhoneBuf,	'\0',	sizeof(tmpPhoneBuf));
																		strcat((char *)tmpPhoneBuf,	(char *)DEF_SMS_CONUTRY_CODE);	// ���ƹ��Ҵ���
																		strcat((char *)tmpPhoneBuf,	(char *)pPhoneCode);				// ���Ʊ����绰
																		
																		//	Make Pdu data and retur the len of data
																		tmplen = PDUMakeUpData(DEF_COM_PHONE, (u8	*)pSmsCenter,	(u8	*)tmpPhoneBuf, (u8	*)pData, len,	(u8	*)tmpUSC2Buf);
																		
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		sprintf((char *)lenBuf,	"%d",	tmplen);
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);	
																		strcat((char *)tmpBuf,	(char *)lenBuf);
																		strcat((char *)tmpBuf,	(char *)"\r");	
																		
																		if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																		{
																				BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																				retCode	=	DEF_GSM_CMGSACK_ERR;				
																		}
																		else
																		{
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																						retCode	=	DEF_GSM_CMGSCPL_ERR;				
																				}		
																		}
																}
																//	Send AT+CSCS="GSM" �лص�GSM���뷽ʽ��ֹ�ٴ������������ʱ���뼯��ΪUCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
																		retCode	=	DEF_GSM_CSCS_ERR;
																}	
														}
												}
										}
								}
						}
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSRead() 
* Description    : ����ȡ�ص��ֻ����뱣��ԭ��ʽ����      
* Input          : ���ж��Ž���USC2���뷽ʽ��ȡ�������ڲ��Ὣ���յ���USC2��ת��ASCII��(Ŀ�ı���GSM�ַ�����һЩ����
*									 �ַ��磺@,$�޷�ʶ��),���ŷ���ʱ���Զ�ת��Ϊ��������ʱ��
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_SMSRead(u8	*pIndexBuf,	SYS_DATETIME *pTime,	u8	*pNumBuf,	u8	*pdBuf,	u16	*pdLen,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[30]="",	tmpUSC2Buf[1024]="",	zoneBuf[5]="";
		u16	p,i,len=0;	
		s8	retCode=DEF_GSM_NONE_ERR,	zoneHour=0;	
		SYS_DATETIME tmpTime;	
		u32	count=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// �����κϷ���
				if(*pIndexBuf == '\0')
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								//	Send AT+CMGF=1
								//	Return to text mode
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_1,	strlen((const char *)AT_CMGF_1),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_CMGF_ERR;
								}
								else
								{				
										//	Send AT+CSCS
										//	Return to "UCS2" �޸ĳ�UCS2��ʽ(GSMģʽ�¶�������Щ�����ַ�������ģ��᷵������)
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												retCode	=	DEF_GSM_CSCS_ERR;			
										}
										else
										{
												//	Send CMGR
												memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
												strcat((char *)tmpBuf,	(char *)AT_CMGR_H);	
												strcat((char *)tmpBuf,	(char *)pIndexBuf);	
												strcat((char *)tmpBuf,	(char *)"\r");	
						
												if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CMGRTimeout) !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
														retCode	=	DEF_GSM_CMGR_ERR;
												}
												else
												{																					
														// ��ѯλ��
														p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CMGR: ",s_GSMcomm.ps_commu->RCNT,9);
														if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
														{
																// ���Ŵ洢��Ϊ��
																retCode	=	DEF_GSM_SMS_ENPTY;
														}
														else
														{
																//	��ȡ�绰����
																p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)",\"",s_GSMcomm.ps_commu->RCNT,2);
																if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																{
																		retCode	=	DEF_GSM_FAIL_ERR;	
																}
																else
																{			
																		memset(tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																		
																		//����ԭ��ʽ����(��+86XXXXXX)
																		p = p + 2;	// ָ��绰��						
																		for(i=0;	(*(s_GSMcomm.pRecBuf + p + i) != '\"') && (i < sizeof(tmpUSC2Buf));	i++)
																		{
																				tmpUSC2Buf[i] =  *(s_GSMcomm.pRecBuf + p + i);
																		}
																		tmpUSC2Buf[i] = '\0';
																		
																		//ת���ɱ�׼ACSII���Ա�Э�����	
																		len = 0;
																		len = ChangeUSC22ASCII (tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	pNumBuf);
																		if(len == 0xffff)
																				retCode	=	DEF_GSM_UNKNOW_ERR;	// ת������
																		else
																		{																				
																				//  ��ȡ���ŷ���ʱ��
																			  p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\"**/",s_GSMcomm.ps_commu->RCNT,4);
																				if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																				{
																						retCode	=	DEF_GSM_BUSY_ERR;	
																				}
																				else
																				{
																						p += 1;								
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->year	=	2000 + atoi((const char *)tmpBuf);
																						tmpTime.year =	2000 + atoi((const char *)tmpBuf);
										
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->month	=	atoi((const char *)tmpBuf);
																						tmpTime.month =atoi((const char *)tmpBuf);
																						
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->day	=	atoi((const char *)tmpBuf);
																						tmpTime.day =atoi((const char *)tmpBuf);
										
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->hour	=	atoi((const char *)tmpBuf);
																						tmpTime.hour =atoi((const char *)tmpBuf);
										
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->minute	=	atoi((const char *)tmpBuf);
																						tmpTime.minute =atoi((const char *)tmpBuf);
										
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->second	=	atoi((const char *)tmpBuf);
																						tmpTime.second =atoi((const char *)tmpBuf);
																						
																						// ��ȡʱ��
																						p += 2;
																						for(i=0;	(i<3) && (*(s_GSMcomm.pRecBuf + p + i) != '"');	i++)
																						{
																								zoneBuf[i] = *(s_GSMcomm.pRecBuf + p + i);
																						}
																						zoneBuf[i] ='\0';
																						zoneHour = atoi((const char *)zoneBuf) / 4; // ת��ʵ��ʱ��																						
																						count = Mktime (tmpTime.year, tmpTime.month,	tmpTime.day, tmpTime.hour,	tmpTime.minute, tmpTime.second);
																						count = count - (zoneHour * 3600);	// ת�ɸ�������ʱ��																	
																						Gettime (count,	pTime);	// תΪ������ʱ�����ʽ

																						//�ӻ����������["\r]λ�ü���������
																						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\"\r",s_GSMcomm.ps_commu->RCNT,2);
																					 	if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																						{
																								retCode	=	DEF_GSM_FAIL_ERR;	
																						}
																						else
																						{
																								memset(tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																								//	��ȡ��������
																								p	+= 3;
																								for(i=0;	*(s_GSMcomm.pRecBuf + p + i) != '\r';	i++)
																								{
																										tmpUSC2Buf[i] = *(s_GSMcomm.pRecBuf + p + i);
																								}
																								tmpUSC2Buf[i] = '\0';
		
																								//ת���ɱ�׼ACSII���Ա�Э�����
																								len = 0;
																								len = ChangeUSC22ASCII (tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	pdBuf);
																								if(len == 0xffff)
																								{
																										*pdLen  = 0;
																										retCode	=	DEF_GSM_UNKNOW_ERR;	// ת������
																								}
																								else
																										*pdLen = len;		
																						}
																				}
																		}
																}	
														}
												}
										}
										//	Send AT+CSCS="GSM" �лص�GSM���뷽ʽ��ֹ�ٴ������������ʱ���뼯��ΪUCS2
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
												retCode	=	DEF_GSM_CSCS_ERR;
										}											
								}
						}
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSDelete() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_SMSDelete(u8	*pIndexBuf,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[30];	
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// �����κϷ���
				if(*pIndexBuf == '\0')
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{							
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								//	Send CMGD
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CMGD_H);	
								strcat((char *)tmpBuf,	(char *)pIndexBuf);	
								strcat((char *)tmpBuf,	(char *)"\r");
								
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CMGDTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_CMGD_ERR;
								}
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
*                            					GSM�ϲ�Ӧ�ú���-�绰����(��OS Ӧ�ú�������)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_CallDail() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_CallDail (u8	*pPhone,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[30]="";
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// �����κϷ���
				if(*pPhone == '\0')
				{	
						retCode =	DEF_GSM_FAIL_ERR;	
				}
				else
				{
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								//	Send ATD
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_ATD_H);	
								strcat((char *)tmpBuf,	(char *)pPhone);	
								strcat((char *)tmpBuf,	(char *)";\r");
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	ATDTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
										retCode	=	DEF_GSM_ATD_ERR;
								}
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;
				}	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_CheckCall() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_CheckCall (u8 *pPhone,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u16 p=0,i=0;
		u8	sta=0;
		s8	tmpRen,	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						tmpRen = BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CLCC,	strlen((const char *)AT_CLCC),	CommandRecOk,	CLCCTimeout);
						if(tmpRen != DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_CLCC_ERR;		
						}
						else
						{
								p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CLCC:",s_GSMcomm.ps_commu->RCNT,6);		
								if((p == 0xffff)&&(p	> s_GSMcomm.ps_commu->RCNT))
								{
										retCode = CALL_DOWN;		// �����ֶ���ֻ��OKû��CLCC��ʾͨ���Ѿ����������Բ��������ۼ�
								}
								else
								{
										//+CLCC: 1,1,0,0,0\r\n\r\nOK\r\n	(��������ʾ����)
										//+CLCC: 1,1,4,0,0,"01058302855",129\r\n\r\nOK\r\n
										sta = *(s_GSMcomm.pRecBuf + p + 9);			// ȡ�绰����״̬
										if(sta == '0')
										{
											 	 // �绰������	
										}
										else if(sta == '1')
										{
												// �绰������ȡ�����ߵ绰����
												if((*(s_GSMcomm.pRecBuf + p + 16) == ',') && (*(s_GSMcomm.pRecBuf + p + 17) == '"')) 	
												{
														for(i=0;	(*(s_GSMcomm.pRecBuf + p + i + 18) != '\"') && (i < s_GSMcomm.ps_commu->RCNT);	i++)
														{
															 *(pPhone + i) = *(s_GSMcomm.pRecBuf + p + i + 18);	
														}
														*(pPhone + i) = '\0';
												}
												else
														*pPhone = '\0';	//	(��������ʾ�޸��ֶ�)				
										}
										sta = *(s_GSMcomm.pRecBuf + p + 11);			// ȡ�绰״̬
									  if(sta == '0')
									 			retCode = CALL_ACTIVE;
									  else if(sta == '1')
									 			retCode = CALL_HELD;
									  else	if(sta ==	'2')
									 			retCode = CALL_DIALLING;
									  else if(sta == '3')
												retCode = CALL_ALERTING;
										else if(sta == '4')
												retCode = CALL_INCOMING;
										else if(sta == '5')
												retCode = CALL_WAITING;
										else
												retCode	=	DEF_GSM_FAIL_ERR;	
								}
						}
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_HungUpCall() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_HungUpCall(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_ATH,	strlen((const char *)AT_ATH),	CommandRecOk,	ATHTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_ATH_ERR;		
						}			
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_ChangeAudioPath() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_ChangeAudioPath(u8 audioPath,	u32	timeout)
{
		audioPath = audioPath;
		timeout		= timeout;
		return	DEF_GSM_NONE_ERR;
}
/*
*********************************************************************************************************
* Function Name  :									  BPS_AckCall() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BPS_AckCall(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_ATA,	strlen((const char *)AT_ATA),	CommandRecOk,	ATATimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_ATA_ERR;		
						}		
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
*                            					GSM�ϲ�Ӧ�ú���-˯�߲���(��OS Ӧ�ú�������)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  GSMGotoSleep() 
* Description    :         
* Input          : RTS-IO = 1;
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void	GSMGotoSleep (void)
{
		// ����˯��ģʽ		
		IO_GSM_DTR_DIS();					
		IO_GSM_DTR_EN();					
}
/*
*********************************************************************************************************
* Function Name  :									  GSMStartWork() 
* Description    :         
* Input          : RTS-IO = 0;
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void	GSMStartWork (void)
{
		// �˳�˯��ģʽ
		IO_GSM_DTR_EN();
		IO_GSM_DTR_DIS();
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMIntoSleep() 
* Description    :  ����AT+CSCLK=1ʹ��˯��ģʽ ������ͷ�DTR�ܽ�(DTR 0->1)������˯�߱�־(ringWUpFlag 0->1)       
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMIntoSleep(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ʹ��˯��ģʽ
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCLK_1,	strlen((const char *)AT_CSCLK_1),	CommandRecOk,	CSCLK1Timeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_CSCLK1_ERR;		
						}	
						else
						{
								gsmDelayMS(2000);	// ����ģ�����2��		
								GSMGotoSleep();		// ֱ�ӽ���˯��	
								ringWUpFlag	=1;		// ����GSM˯�߱�־		
						}							
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMWakeUp() 
* Description    : ����ǰ����Ƿ�����ring���µĻ�������ǵĻ�����Ҫ�ֶ�����һ�ζ�Ϣ��ѯ        
* Input          : ����RTS=1
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMWakeUp(u32	timeout)
{
		timeout = timeout; 
		GSMStartWork();		// ֱ���˳�˯��	
				
		if(ringWUpFlag == 2)	
		{
				ringWUpFlag =0;																					// �ָ���־��Ĭ��״̬
				if(BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0)							// ��ֹ�ظ�����
				{
						if(OSRunning > 0)
						{
								BSP_GSM_SetFlag(DEF_SMSDOING_FLAG);							// ���ö��Ŵ�����
								OSSemPost(GSMSmsSem); 													// �豸��˯�߻����ֶ�����һ�ζ��ż���
						}
				}
		}		
		return	0;	
}

/*
*********************************************************************************************************
*                            					GSM�ϲ�Ӧ�ú���-ͨ�ýӿڲ���(��OS Ӧ�ú�������)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMBypassPro() 
* Description    : GSM AT ָ��͸���ӿ�       
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMBypassPro(u8	*sendBuf,	u16 sendLen,	u8	*recBuf,	u16 *recLen,	u16 maxRecLen,	u32 atTimeout,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// �����λ
						if(BSP_SendATcmdPro(DEF_NULL_MODE,	(u8	*)sendBuf,	sendLen,	CommandTimeout,	atTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// �����ۼ�
								retCode	=	DEF_GSM_UNKNOW_ERR;		
						}	
						else
						{
								// ���Ʒ����ַ���
								if(s_ATcomm.Rlen > maxRecLen)
										memcpy(recBuf,	s_ATcomm.pRecBuf,	maxRecLen);	
								else
										memcpy(recBuf,	s_ATcomm.pRecBuf,	s_ATcomm.Rlen);	
								*recLen	=	s_ATcomm.Rlen;	
								retCode = DEF_GSM_NONE_ERR;		
						}					
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}

/*
******************************************************************************
* Function Name  : BSP_GSMSemPend()
* Description    : ����GSM�ź����Ļ�ȡ��������BSP_GSMSemPost�ɶ�ʹ��
* Input          : timeout :�ȴ��ź�����ʱʱ��
* Output         : None
* Return         : None
******************************************************************************
*/
s8	BSP_GSMSemPend (u32	timeout)
{
		INT8U	err =OS_NO_ERR;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	�����������ź���
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	-1;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}
		return 0;
}
/*
******************************************************************************
* Function Name  : BSP_GSMSemPost()
* Description    : ����GSM�ź������ͷţ�������BSP_GSMSemPend�ɶ�ʹ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
void BSP_GSMSemPost (void)
{
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	�ͷ�ռ�õ��ź���
		else
				GSMMutexFlag	=	1;
}
/*
*********************************************************************************************************
*                            					GSMѭ�����ú���(��ϵͳ��ѭ������)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_Setup()
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_Setup (void)
{
		u16 i;
		u8	*pUint8;

		pUint8 = (u8 *)s_GSMcomm.pSendBuf;

		// Pow on the gsm module 
		if(s_GSMcomm.SetupPhase >= CommandInitOK)
				return;// Already finish the gsm module setup 
		else
				IO_GSM_POW_EN();		// ģ�鹩��	
																				
    if(s_GSMcomm.SetupPhase == PowerOff)
		{
				// Ԥ�ö˿�״̬
				s_GSMcomm.GSMComStatus 	= GSM_POWOFF;

				// ����������ջ�����׼�����տ����ַ���
				memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE);	
				s_GSMcomm.ps_commu->RCNT	=	0;
				GSM_ENABLE_RES_IT();							// ��һ��ʹ��GSMģ���ж�
				BSP_GSMRING_IT_CON(DEF_DISABLE);	// �������йر�RING�ж�
							
				//IO_GSM_RES_OFF();
				IO_GSM_POW_OFF();
				IO_GSM_POW_ON();
	     	s_GSMcomm.SetupPhase = PowerOn;
    }
		else if(s_GSMcomm.SetupPhase == PowerOn)
		{
	      s_GSMcomm.SetupPhase 		= SignalOn;
				s_GSMcomm.Timer 				= 0;							//׼����ʱ2S
    }
		else if(s_GSMcomm.SetupPhase == SignalOn)
		{
				if(s_GSMcomm.Timer < GSMTime2s)		// �ȴ�2S	
		      	return;
	      else
				{
		        IO_GSM_POW_OFF();
		        s_GSMcomm.SetupPhase 		= CheckPowPin;
						s_GSMcomm.Timer					=	0;	 				 //׼���´���ʱ
	      }
    }
		else if(s_GSMcomm.SetupPhase == CheckPowPin)
		{
				if(IO_GSM_POW_DET() == 1u)
				{			
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSMSTA)
						myPrintf("[GSM]: PowerOn OK!\r\n");
						#endif
						s_GSMcomm.SetupPhase 		= CheckStartString;
						s_GSMcomm.Timer					=	0;	 				 // ׼���´���ʱ
						BSP_GSM_ERR_Clr(&err_Gsm.powOnTimes);  // ��������
				}
				else
				{
						if(s_GSMcomm.Timer < GSMTime10s)
								return;
						else
						{
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSMSTA)
								myPrintf("[GSM]: PowerOn Fail!\r\n");
								#endif
								BSP_GSM_ERR_Add(&err_Gsm.powOnTimes,	1);	//����ʧ�ܴ�����һ
								if(BSP_GSM_ERR_Que(&err_Gsm.powOnTimes) >= DEF_GSM_POWON_TIMES)
								{
										BSP_GSM_ERR_Clr(&err_Gsm.powOnTimes);
										s_GSMcomm.HardWareSta		=	DEF_GSMHARD_MODDESTROY_ERR;	
								}
								s_GSMcomm.SetupPhase 	= PowerOff;
								s_GSMcomm.Timer				=	0;	 			//׼���´���ʱ	
								BSP_GSM_POW_RESET(3000);
						}
				}
		}
		// Check power on string 
		else if(s_GSMcomm.SetupPhase == CheckStartString)
		{
				if(s_GSMcomm.Timer < GSMTime5s)		//�ȴ�������ϵ��ַ�"RDY"
				{
						if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"RDY\r\n",s_GSMcomm.ps_commu->RCNT,5)!=0xffff)
						{									
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSM)
								BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pRecBuf,	s_GSMcomm.ps_commu->RCNT);
								#endif
								
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSMSTA)
								myPrintf("[GSM]: Communication OK!\r\n");
								#endif

								s_GSMcomm.SetupPhase 		= CheckSimCardSend;
								s_GSMcomm.Timer					=	0;	 				//׼���´���ʱ																				
								BSP_GSM_ERR_Clr(&err_Gsm.cmmTimes);  // ��������
								gsmDelayMS(1000);	
						}
				}
				else
				{
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSMSTA)
						myPrintf("[GSM]: Fixed Baud Rate!\r\n");
						#endif
										
						// ��Ҫ�̶�������
						GSM_DISABLE_SEND_IT();												
						BSP_PutString(DEF_GSM_UART,(u8 *)"AT+IPR=115200\r");
						gsmDelayMS(500);
						BSP_PutString(DEF_GSM_UART,(u8 *)"AT&W\r");	//SIM800��Ҫ�ֶ��洢
						gsmDelayMS(200);

					  // ������
						BSP_GSM_ERR_Add(&err_Gsm.cmmTimes,	1);	//����ʧ�ܴ�����һ
						if(BSP_GSM_ERR_Que(&err_Gsm.cmmTimes) >= DEF_GSM_STRING_TIMES)
						{
								BSP_GSM_ERR_Clr(&err_Gsm.cmmTimes);
								s_GSMcomm.HardWareSta		=	DEF_GSMHARD_COMM_ERR;	
						}
						s_GSMcomm.SetupPhase 		= PowerOff;
						s_GSMcomm.Timer					=	0;	 			//׼���´���ʱ
						BSP_GSM_POW_RESET(3000);
				}	
		}	
		// send check simcard
		else if(s_GSMcomm.SetupPhase == CheckSimCardSend)
		{
				memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // �建��
				s_GSMcomm.ps_commu->RCNT	=	0;
				GSM_DISABLE_SEND_IT();	
			
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				myPrintf("AT+CPIN?\r\n");	// �������ATָ��
				#endif	
				BSP_PutString(DEF_GSM_UART,(u8 *)"AT+CPIN?\r");	// ��ѯSIM card
				//BSP_PutString(DEF_GSM_UART,(u8 *)"AT+CREG?\r");	// ��ѯSIM card
				s_GSMcomm.SetupPhase = CheckSimCard;
				s_GSMcomm.Timer			 = 0;	 			//׼���´���ʱ
		}
		// check simcard 
		else if(s_GSMcomm.SetupPhase == CheckSimCard)
		{
				if(s_GSMcomm.Timer < GSMTime2s)
				{
						if((StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CPIN: READY",s_GSMcomm.ps_commu->RCNT,12)!=0xffff) ||
						   (StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CPIN: NOT INSERTED",s_GSMcomm.ps_commu->RCNT,19)!=0xffff) ||
						   (StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"ERROR",s_GSMcomm.ps_commu->RCNT,5)!=0xffff))
						{								
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSM)
								BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pRecBuf,	s_GSMcomm.ps_commu->RCNT);
								#endif		
							
								if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CPIN: READY",s_GSMcomm.ps_commu->RCNT,12)!=0xffff) 
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GSMSTA)
										myPrintf("[GSM]: Detect Simcard!\r\n");
										#endif
								}
								else
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GSMSTA)
										myPrintf("[GSM]: SimCard Not Insert!\r\n");
										#endif							
								}
																
								memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // �建��		
								s_GSMcomm.ps_commu->RCNT	=	0;						
								s_GSMcomm.GSMComStatus 	= GSM_POWOK;
								s_GSMcomm.SetupPhase 		= SimCardOK;
								s_GSMcomm.Timer					=	0;	 				//׼���´���ʱ
								s_GSMcomm.HardWareSta		=	DEF_GSMHARD_NONE_ERR;										
								BSP_GSM_ERR_Clr(&err_Gsm.simCardTimes);	//������ʱ��
						}
				}
				else
				{					
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSM)
						BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pRecBuf,	s_GSMcomm.ps_commu->RCNT);
						#endif
						
						memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // �建��		
						s_GSMcomm.ps_commu->RCNT	=	0;						
						s_GSMcomm.GSMComStatus 	= GSM_POWOK;
						s_GSMcomm.SetupPhase 		= SimCardOK;
						s_GSMcomm.Timer					=	0;	 				//׼���´���ʱ
						s_GSMcomm.HardWareSta		=	DEF_GSMHARD_NONE_ERR;										
						BSP_GSM_ERR_Clr(&err_Gsm.simCardTimes);	//������ʱ��
				}	
		}
		// Start send AT cmd 
		else if(s_GSMcomm.SetupPhase == SimCardOK)
		{
	      // send out the first AT cmd. 
	      s_GSMcomm.SetupPhase 	= InitCommand;
	      s_GSMcomm.CmdPhase 		= CommandSend;
	      s_GSMcomm.Timer 			= 0;
	      s_GSMcomm.ActiveCmd 	= 0;
				
				//---------------------------------------------------------------
	      i=0;
	      while(pATcommand[0][i])
				{
		      	*pUint8 = pATcommand[0][i];
		       	pUint8++;
		        i++;
	      }
	      //---------------------------------------------------------------
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pSendBuf,	i);
				#endif		
				memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // �建��	
				s_GSMcomm.ps_commu->RCNT = 0;		
			 	s_GSMcomm.ps_commu->SLEN = i;
	      s_GSMcomm.sendstart();
    }
		else if(s_GSMcomm.SetupPhase == InitCommand)
		{
        //  setting up Parameter to gsm module 
        if(s_GSMcomm.CmdPhase == CommandSend)
				{
            if(s_GSMcomm.ps_commu->State & SS_O)
						{
								s_GSMcomm.CmdPhase 	= CommandSdCpl;
                s_GSMcomm.Timer 		= 0;
								GSM_ENABLE_RES_IT();					//ʹ�ܽ����ж�׼������ ATָ����
            }   
        }
				else if(s_GSMcomm.CmdPhase == CommandSdCpl)
				{
            i = CheckResultCode(DEF_COMMON_MODE,	cmdTimeout[s_GSMcomm.ActiveCmd]);
            if(i)
						{
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSM)
								BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pRecBuf,	s_GSMcomm.ps_commu->RCNT);
								#endif		
							
								// receive check!!
				        if(s_GSMcomm.CmdPhase == CommandRecOk)
								{            						
										// received OK. send next AT command.
				            s_GSMcomm.ActiveCmd++;
				
										// ������ʱ�еĿ���������ʱ��ʼ�����ɹ�
										gsmDelayMS(500);
				
				            if(s_GSMcomm.ActiveCmd == InitCommandCpl)
										{               											
												#if(DEF_GSMINFO_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_GSMSTA)
												myPrintf("[GSM]: Init Success!\r\n");
												#endif											
											
												// the last At command has been send successfully. 
				                s_GSMcomm.SetupPhase 		= CommandInitOK;
				                s_GSMcomm.CmdPhase 			= CommandIdle;
				                s_GSMcomm.ActiveCmd 		= NoCmdActive;	       
												s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ��GSM�˿�״̬ 
												BSP_GSMRING_IT_CON(DEF_ENABLE);				// ����RING�ж�
												BSP_GSM_ERR_Clr(&err_Gsm.initTimes);
												return;
				            }
										else
										{}	//��������һ��AT����
				        }
								else if((s_GSMcomm.CmdPhase == CommandRecErr) || (s_GSMcomm.CmdPhase == CommandTimeout))
								{      					 
										// received ERROR. 
										OSTimeDly(1000);		// ����ظ������ֹ����ָ�������� 
				
										// ������
										BSP_GSM_ERR_Add(&err_Gsm.initTimes,	1);	//����ʧ�ܴ�����һ
										if(BSP_GSM_ERR_Que(&err_Gsm.initTimes) >= DEF_GSM_INIT_TIMES)
										{
												BSP_GSM_ERR_Clr(&err_Gsm.initTimes);
												s_GSMcomm.HardWareSta		=	DEF_GSMHARD_INIT_ERR;	
												s_GSMcomm.SetupPhase 		= PowerOff;
												BSP_GSM_POW_RESET(6000);
										}
				        }
								else
								{
										return;
								}
				        
				        /////////////////////////////////////////////////////////////////////
				        /////////////////////////////////////////////////////////////////////
								// ���͵ڶ����Ժ�ĳ�ʼ��ָ��
				        i=0;
				        while(pATcommand[s_GSMcomm.ActiveCmd][i])
								{
				            *pUint8 = pATcommand[s_GSMcomm.ActiveCmd][i];
				            pUint8++;
				            i++;
				        }

				        //----------------------------------------------------------------
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSM)
								BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pSendBuf,	i);
								#endif		
								memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // �建��	
								s_GSMcomm.ps_commu->RCNT = 0;		
				        s_GSMcomm.Timer = 0;
				        s_GSMcomm.CmdPhase = CommandSend;
				        s_GSMcomm.ps_commu->SLEN = i;
				        s_GSMcomm.sendstart();
						}	
						else
              	return;	// �ȴ���ʱ
        }
				else
          	return;
    }		
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_TransPro()
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_TransPro (void) 
{				
		Start2doSendAT();		// AT���ʹ���
		DoSendProcess();		// AT���մ���		
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_CommonPro()	
* Description    : GSMģ�飬���绰�����ţ�GPRS����������URC�����жϴ���        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_CommonPro (void)
{
  	u16	p=0,q=0,k=0,dotNum=0;

		// �Զ��ϱ���URC��⴦��(��������ݽ׶ν�ֹ���)//////////////////////////////////////////////////
		if((s_GSMcomm.SetupPhase == CommandInitOK) && (s_GSMcomm.GSMComStatus	>=	GSM_ONCMD) && (tcpReadMuxtex == 0))
		{		
				//	���Ring�ܽ�״̬	
				// 2015-1-15 by:gaofei �ֲ�GPRS������©����,���߼���ȡ��,������һ��ʹ��URC ring�жϿ��ܵ������ݽ���
				if(BSP_GSM_GetFlag(DEF_RING_FLAG) == 1)
				{
						BSP_GSM_ReleaseFlag(DEF_RING_FLAG);
						
						/*
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSMSTA)
						myPrintf("[GSM]: Ring!\r\n");
						#endif						
						if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
						{
								ringCheckStep =1;
								OSTmr_Count_Start(&ringCheckCounter);
						}
						*/
				}
				/*
				if(ringCheckStep == 1)
				{
						if(OSTmr_Count_Get(&ringCheckCounter) >= 2000)
						{
								ringCheckStep =0;
								if((BSP_GSM_GetFlag(DEF_CALLDOING_FLAG) == 0) && 
									 (BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0) &&
								   (BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1))
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GSMSTA)
										myPrintf("[GSM]: Ring-reRead!\r\n");
										#endif
										if(OSRunning > 0)
												OSSemPost(GSMGprsDataSem);		// �ֶ���ѯ�Ƿ�ӵ�������
								}
						}			
				}
				*/
				//	����Ƿ��е绰����	
				/*
				p = 0;
				p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"RING\r\n",s_GSMcomm.ps_commu->RCNT,6);		
				if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
				{
						*(s_GSMcomm.pRecBuf + p) 			= '\0';
						*(s_GSMcomm.pRecBuf + p + 1) 	= '\0';		
						BSP_PutString(DEF_GSM_UART,(u8 *)"ATA\r");	// �ӵ绰
						
				}
				*/
				//  �����Ž���	
				//	���GPRS����״̬
				//	���BT״̬
				//	���TTS״̬
				if(1)
				{				
						p =	0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CMTI:",s_GSMcomm.ps_commu->RCNT,6);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';
								*(s_GSMcomm.pRecBuf + p + 1) 	= '\0';								// ɾ���ַ�����ֹͬһ�ַ����ظ����
								if(BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0)					// ��ֹ�ظ�����
								{
										if(OSRunning > 0)
										{
												BSP_GSM_SetFlag(DEF_SMSDOING_FLAG);					// ���ö��Ŵ�����
												OSSemPost(GSMSmsSem); 
										}
								}
						}	
				}
				
				//  ���TTS�������			
				if(1)
				{		
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CTTS: 0",s_GSMcomm.ps_commu->RCNT,8);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';
								*(s_GSMcomm.pRecBuf + p + 1) 	= '\0';								// ɾ���ַ�����ֹͬһ�ַ����ظ����
								if(BSP_GSM_GetFlag(DEF_TTSPLAY_FLAG) == 1)					// ��ֹ�ظ�����
								{
										BSP_GSM_ReleaseFlag (DEF_TTSPLAY_FLAG);					// ��λ��־										
										IO_SPAKER_PA_DIS();															// �رչ���
								}
						}	
				}
				
				//	���BT�������ݼ�ҵ��״̬	
				if(1)
				{						
						// ���ƥ������									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTPAIRING:",s_GSMcomm.ps_commu->RCNT,11);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								gsmDelayMS(5);	// �ȴ���������
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// ɾ���ַ�����ֹͬһ�ַ����ظ����
								p++;
								for(k=0,dotNum=0;	(k<s_GSMcomm.ps_commu->RCNT) && (*(s_GSMcomm.pRecBuf + p) != '\r') && (*(s_GSMcomm.pRecBuf + p) != '\n');	k++)
								{
										if(*(s_GSMcomm.pRecBuf + p + k) == ',')
												dotNum++;	
								}
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								if(dotNum == 1)
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_BT)
												myPrintf("[BT]: Pass pair req!\r\n");
										#endif	
										s_MboxBT.func =BT_PAIR_PASS;											// Passkeyƥ��ҵ��
										if(OSRunning > 0)
												OSMboxPost(GSMBtAppMbox, (void *)&s_MboxBT);	// �������䵽BT���� 										
								}
								else if(dotNum == 2)
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_BT)
												myPrintf("[BT]: Digi pair req!\r\n");
										#endif												
										s_MboxBT.func =BT_PAIR_DIGI;											// ����ƥ��ҵ��
										if(OSRunning > 0)
												OSMboxPost(GSMBtAppMbox, (void *)&s_MboxBT);	// �������䵽BT���� 
								}
								else
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_BT)
												myPrintf("[BT]: Unknow pair req!\r\n");
										#endif	
								}
						}									
							
						// �����������									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTCONNECTING:",s_GSMcomm.ps_commu->RCNT,14);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// ɾ���ַ�����ֹͬһ�ַ����ظ����				
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								s_MboxBT.func =BT_CONNECT;													// ����ҵ��
							
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_BT)
										myPrintf("[BT]: Connect req!\r\n");
								#endif
								if(OSRunning > 0)
										OSMboxPost(GSMBtAppMbox, (void *)&s_MboxBT);		// �������䵽BT���� 
						}	
						
						// ��������Ͽ�									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTDISCONN:",s_GSMcomm.ps_commu->RCNT,11);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// ɾ���ַ�����ֹͬһ�ַ����ظ����
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								s_MboxBT.func =BT_DISCONNECT;												// �Ͽ�����ҵ��							
								BSP_GSM_SetFlag(DEF_BTDING_FLAG);										// �����������߱�־
								
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_BT)
										myPrintf("[BT]: Disconnect!\r\n");
								#endif
						}	

						// �����������
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTSPPMAN: *",s_GSMcomm.ps_commu->RCNT,12);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// ɾ���ַ�����ֹͬһ�ַ����ظ����
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								s_MboxBT.func =BT_RXDATA;														// �յ�����	
								s_MboxBT.cnnId =*(s_GSMcomm.pRecBuf + p + 11);			// ��ȡ����ID
								
								if(OSRunning > 0)
										OSMboxPost(GSMBtDataMbox, (void *)&s_MboxBT);		// �������䵽BT���� 
						}	
				}	
				
				//	���GPRS�������ݼ������쳣	
				if(s_GSMcomm.GSMComStatus	>=	GSM_ONLINE)
				{						
						// ����������									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 1,*",s_GSMcomm.ps_commu->RCNT,14);		
						if((p	!=  0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) = '\0';										// ɾ���ַ�����ֹͬһ�ַ����ظ����
								if(OSRunning > 0)
										OSSemPost(GSMGprsDataSem);		// ���յ��������ź�������ʽͶ�ݳ�ȥ(���ǵ����ܺܶ�ʱ���յ��ܶ��������䲻����) 
						}

						// ��������쳣
						p = 0;
						q = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+PDP: DEACT",s_GSMcomm.ps_commu->RCNT,11);	// �豸GPRS PDP�����Ͽ�
						q = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"0, CLOSE",s_GSMcomm.ps_commu->RCNT,8);			// ͨ��0�����������Ͽ�		
						if(((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT)) || ((q	!=	0xffff)&&(q	<=	s_GSMcomm.ps_commu->RCNT))) 
						{
								if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
								{								
										// GPRS PDP�����ĶϿ�
										*(s_GSMcomm.pRecBuf + p) = '\0';								// ɾ���ַ�����ֹͬһ�ַ����ظ����																			
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("[GPRS]: GPRS PDP break!\r\n");
										#endif
								}
								else
								{
										// �����������Ͽ�
										*(s_GSMcomm.pRecBuf + q) = '\0';								// ɾ���ַ�����ֹͬһ�ַ����ظ����										
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("[GPRS]: Server break!\r\n");
										#endif
								}
								s_GSMcomm.GSMComStatus	=	GSM_ONCMD;								// ��GSM�˿�״̬
								BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);		
						}
				}
		}
}
/*
*********************************************************************************************************
*                                         BSP_GSM_Init()
*
* Description : Init. the GSM device and data type
*
* Argument(s) : 
*
* Return(s)   :	
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_GSM_Init (void)
{
    /* Init variable */
    s_GSMcomm.ps_commu 			= &ComGSM;
    s_GSMcomm.sendstart 		= StartSend_GPRS;
    s_GSMcomm.pSendBuf 			= SBufGPRS;
    s_GSMcomm.pRecBuf 			= RBufGPRS;
    s_GSMcomm.Timer 				= 0;
		s_GSMcomm.TimerApp 			= 0;
    s_GSMcomm.ActiveCmd 		= 0;
    s_GSMcomm.SetupPhase 		= PowerOff;				
    s_GSMcomm.CmdPhase 			= CommandIdle;
		s_GSMcomm.HardWareSta		=	DEF_GSMHARD_CHEKING;	
    s_GSMcomm.GSMComStatus 	= GSM_POWOFF;

		memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	// ��������

		s_ATcomm.Slen						=	0;
		s_ATcomm.Rlen						=	0;
		s_ATcomm.pSendBuf				=	SBufAT;
		s_ATcomm.pRecBuf				=	RBufAT;
		s_ATcomm.timeout				=	0;
		s_ATcomm.CheckMode			=	DEF_COMMON_MODE;
				
		BSP_USART_Init(DEF_GSM_UART,	DEF_GSM_REMAP,	DEF_GSM_BAUT);

		GSMRingPinInit();			//	��ʼ��RING�жϹܽ�
		
		GSM_DISABLE_RES_IT();		//	��ֹ�����ж�
		GSM_DISABLE_SEND_IT();	//	��ֹ�����ж�

		// ��ֹ��GSM��Դ����оƬ����(�����Դֱͨ)ʱӲ���ϵ�GSMҲ���Լ��ɹ�,���������λ���ʧ�� 
		IO_GSM_POW_ON();
		gsmDelayMS(1000);
		IO_GSM_POW_OFF();
		gsmDelayMS(100);
		GSMFeedWDGProcess();	// ι��	
		
		#if(DEF_INITINFO_OUTPUTEN > 0)
		if(dbgInfoSwt & DBG_INFO_SYS)
		{
				if(s_GSMcomm.HardWareSta == DEF_GSMHARD_CHEKING)
						myPrintf("[Init]: GSM........(checking)!\r\n");
				else if(s_GSMcomm.HardWareSta == DEF_GSMHARD_NONE_ERR)
						myPrintf("[Init]: GSM........(ok)!\r\n");
				else
						myPrintf("[Init]: GSM........(error)!\r\n");
		}
		#endif	
		
}
/*****************************************end*********************************************************/

