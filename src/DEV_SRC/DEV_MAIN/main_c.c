/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            	MAIN CODE
*
*                                               main_c.c
*                                              with the
*                                             Y05D Board
*
* Filename      : main_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define	 MAIN_MODULE
#include	<string.h>
#include	<stdio.h>
#include  <stdlib.h>
#include 	"stm32f10x.h"
#include	"cfg_h.h"
#include 	"main_h.h"
#include 	"bsp_h.h"
#include 	"hal_h.h"
#include 	"ucos_ii.h"
#include 	"bsp_rtc_h.h"
#include 	"bsp_gsm_h.h"
#include 	"bsp_gps_h.h"
#include	"bsp_storage_h.h"
#include 	"bsp_can_h.h"
#include	"bsp_acc_h.h"
#include	"des_h.h"
#include 	"hal_protocol_h.h"
#include 	"hal_processSYS_h.h"
#include 	"hal_processGPS_h.h"
#include 	"hal_processDBG_h.h"
#include 	"hal_processQUE_h.h"
#include 	"hal_processACC_h.h"
#include 	"hal_processCON_h.h"

#ifdef DEF_OBDTASK_CREATE
#include  "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
#endif

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

// LED״̬����
#define		LED_STA_OFF							((u8)0)				// LED����״̬		
#define		LED_STA_ONERR						((u8)1)				// LED��������״̬	
#define		LED_STA_FONE						((u8)2)				// LED����˸һ��״̬
#define		LED_STA_FTWO						((u8)3)				// LED����˸����״̬	
#define		LED_STA_FTHREE					((u8)4)				// LED����˸����״̬	

#define   LED_SETP1_TIME          (500)         // ����
#define   LED_SETP2_TIME          (50)          // ��һ�ε���
#define   LED_SETP3_TIME          (200)         // ����
#define   LED_SETP4_TIME          (50)          // �ڶ��ε���
#define   LED_SETP5_TIME          (200)         // ����
#define   LED_SETP6_TIME          (50)          // �����ε���
#define   LED_SETP7_TIME          (500)         // ����

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

// �ײ�Ӧ��/////////////////////////////////////////////
OS_EVENT							*FlashMutexSem;						// ����FLASH SPI���ߵײ�Ӧ��֮��Ļ���
OS_EVENT							*ACCMutexSem;							// ����ACC���ߵײ�Ӧ��֮��Ļ���
OS_EVENT							*CrashMEMSSem;						// ��ײ�ź���
OS_EVENT							*GPSComSem;								// GPS���ڽ����ź���
OS_EVENT							*GSMMutexSem;							// GSM�ײ�Ӧ�û����ź���
OS_EVENT							*GSMSmsSem;								// �յ������ź���
OS_EVENT							*GSMGprsDataSem;					// GPRS���������ź���
OS_EVENT							*GSMBtDataMbox;						// ����������������
OS_EVENT							*GSMBtAppMbox;						// ������ȡҵ��״̬����
OS_EVENT							*DBGComSem;								// DBG�յ������ź���
OS_EVENT							*DBGMutexSem;							// DBG printf���������Ϣ�����ź���
OS_EVENT							*RamQueMutexSem;					// RAM���ݶ��в��������ź���
OS_EVENT							*FlashQueMutexSem;				// FLASH���ݶ��в��������ź���
OS_EVENT							*QueMutexSem;							// ���в��������ź���
OS_EVENT							*ParaMutexSem;						// CPU Flash������д�����ź���
OS_EVENT							*CONComMbox;							// ���贮�ڵײ��������

// ϵͳ��Ӧ��///////////////////////////////////////////
OS_EVENT							*OS_DownACKMbox;					// �豸��������Ӧ������
OS_EVENT							*OS_ACKMbox;							// �յ�Ӧ��Э�����ݰ�
OS_EVENT							*OS_OBDReqMbox;						// OBD��������(ͨѶ->OBD)
OS_EVENT							*OS_OBDAckMbox;						// OBDӦ������(OBD->ͨѶ)
OS_EVENT							*OS_RecGprsMbox;					// �յ�GPRS�����ͽ���
OS_EVENT							*OS_RecSmsMbox;						// �յ�SMS�����ͽ���
OS_EVENT							*OS_RecBtMbox;						// �յ�BT�����ͽ���
OS_EVENT							*OS_CONReqMbox;						// ���������������
OS_EVENT							*OS_CONAckMbox;						// ����Ӧ�����Ӧ������

// ���ݷ�����ʱ����/////////////////////////////////////
SendData_TypeDef			comAck;			// ���ڴ洢ͨ��Ӧ����ʱ�������ṹ 
SendData_TypeDef			tmpSms;			// ���ڷ��Ͷ�����ʱ������ 
SMSCon_Typedef				conSms;			// ������ʱ�洢���ſ�������

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

// ϵͳ����ȫ�ֱ���
static  OS_STK        App_TaskSTARTStk[APP_TASK_START_STK_SIZE];
static  OS_STK     		App_TaskGSMStk[APP_TASK_GSM_STK_SIZE];    
static  OS_STK     		App_TaskGPRSStk[APP_TASK_GPRS_STK_SIZE];
static	OS_STK				App_TaskRXDStk[APP_TASK_RXD_STK_SIZE];
static	OS_STK				App_TaskSENDStk[APP_TASK_SEND_STK_SIZE];
static	OS_STK				App_TaskPROTOCOLStk[APP_TASK_PROTOCOL_STK_SIZE];
static	OS_STK				App_TaskBASEStk[APP_TASK_BASE_STK_SIZE];
static	OS_STK				App_TaskBTStk[APP_TASK_BT_STK_SIZE];
static	OS_STK				App_TaskGPSStk[APP_TASK_GPS_STK_SIZE];
static	OS_STK				App_TaskMUTUALStk[APP_TASK_MUTUAL_STK_SIZE];
static  OS_STK     		App_TaskCONStk[APP_TASK_CON_STK_SIZE];
static  OS_STK     		App_TaskDBGStk[APP_TASK_DBG_STK_SIZE];

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static	s8						smsConForamtCheck		 	(SMS_Typedef *pSMS,	SMSCon_Typedef *pSMSOut);
static	s8						smsCarControlProcess 	(SMS_Typedef *pSMS,	SMSCon_Typedef *pSMSOut);
static	void					SWDGInSleepReload 		(void);
static 	u8						SWDGTimeOutCheck 			(void);
static	void					sleepCheckProcess 		(vu8 *step,	vu32 *idleCounter);
static	s8						taskDelProcess 				(u32	timeout);

static  void  				App_TaskCreate				(void);
static  void  				App_EventCreate      	(void);
static  void  				App_TaskSTART        	(void	*p_arg);
static	void  				App_TaskGSM 				 	(void	*p_arg);
static	void  				App_TaskGPRS 				 	(void	*p_arg);
static	void					App_TaskRXD						(void	*p_arg);
static	void					App_TaskSEND					(void	*p_arg);
static	void					App_TaskPROTOCOL			(void	*p_arg);
static	void  				App_TaskBASE 				 	(void	*p_arg);
static	void  				App_TaskBT 				 		(void	*p_arg);
static	void  				App_TaskGPS 				 	(void	*p_arg);
static	void					App_TaskMUTUAL				(void	*p_arg);
static	void  				App_TaskCON	 					(void	*p_arg);
static	void  				App_TaskDBG	 					(void	*p_arg);

/*
*********************************************************************************************************
*********************************************************************************************************
*                                          Main App
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
* Function Name  :									  APP_ErrClr() 
* Description    :         
* Input          :		
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	APP_ErrClr (vu8	*Val)
{
		*Val = 0;	
}
void	APP_ErrAdd (vu8	*Val)
{
		if(*Val < 0xff)
				(*Val)++;	
}
u8	APP_ErrQue (vu8	*Val)
{
		return	*Val;	
}

/*
*********************************************************************************************************
*                                     OSTmr_Count_Start
*
* Description: Time Count start base on OS [note]Must use after OS start!!!!!
*
* Argument(s): Max count 0xFFFFFFFF (49.7Сʱ)
*
* Return(s)  : 
*
* Note(s)    : 
*********************************************************************************************************
*/
void	OSTmr_Count_Start(vu32	*pCounter)
{
		CPU_SR  cpu_sr=0;

		/* Get system os time */
		OS_ENTER_CRITICAL();
		*pCounter	=	OSTime;
		OS_EXIT_CRITICAL();	
}
/*
*********************************************************************************************************
*                                     OSTmr_Count_Get
*
* Description: Time Count stop [note]Must use after OS start!!!!!
*
* Argument(s): none.
*
* Return(s)  : time * ms (os tick must be 1000HZ)
*
* Note(s)    : 
*********************************************************************************************************
*/
u32	OSTmr_Count_Get(vu32	*pCounter)
{
		CPU_SR  cpu_sr=0;
		vu32	tmptime = 0;
		/* Get system os time */
		OS_ENTER_CRITICAL();
		tmptime	=	OSTime;
		OS_EXIT_CRITICAL();
		/* Overflow */
		if(tmptime < *pCounter)
				return	(0xffffffff - *pCounter + tmptime);	
		else
				return  (tmptime - *pCounter);	
}
/*
*********************************************************************************************************
*                                     sendPrintProcess
* Description: hex�������ݷ��ʹ������(����������gprs����Debug���)
* Argument(s): 0��ʾ��ӡgprs����1��ʾ��ӡbt����
* Return(s)  : 
* Note(s)    : 
*********************************************************************************************************
*/
void sendPrintProcess (u8 type,	u8 *pHexBuf,	u16 len)
{
		u8	ascBuf[520]={0};
		u16	ascLen=0;
		SYS_DATETIME	tmpRtc;
		
		memset(ascBuf,	'\0',	sizeof(ascBuf));
		
		// ��ȡ��ǰRTCʱ��
		if(s_common.iapSta != DEF_UPDATE_KEYWORD)	// ���������в���ӡRTCʱ���
				BSP_RTC_Get_Current(&tmpRtc);
		if(type == 0)
		{				
				if((len * 2) < sizeof(ascBuf))
				{
						ascLen =Hex2StringArray (pHexBuf,	len,	ascBuf);
						myPrintf("\r\n[%02d/%02d/%02d %02d:%02d:%02d][GPRS-Txd]: %s\r\n",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second,ascBuf);
				}
				else
				{
						ascLen =Hex2StringArray (pHexBuf,	(sizeof(ascBuf)/2-1),	ascBuf);
						myPrintf("\r\n[%02d/%02d/%02d %02d:%02d:%02d][GPRS-Txd]: %s...\r\n",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second,ascBuf);					
				}
		}
		else
		{
				if((len * 2) < sizeof(ascBuf))
				{
						ascLen =Hex2StringArray (pHexBuf,	len,	ascBuf);
						myPrintf("\r\n[BT-Txd]: %s\r\n",	ascBuf);
				}
				else
				{
						ascLen =Hex2StringArray (pHexBuf,	(sizeof(ascBuf)/2-1),	ascBuf);
						myPrintf("\r\n[BT-Txd]: %s...\r\n",	ascBuf);
				}
		}
}

/*
*********************************************************************************************************
*                                     SWDGInSleepReload
* Description: ������Ź�˯����ι������
* Argument(s): 
* Return(s)  : 
* Note(s)    : 
*********************************************************************************************************
*/
static	void	SWDGInSleepReload (void)
{
		OSTmr_Count_Start(&s_swg.gsmCounter);	
		OSTmr_Count_Start(&s_swg.gprsCounter);		
		OSTmr_Count_Start(&s_swg.rxdCounter);
		OSTmr_Count_Start(&s_swg.sendCounter);
		OSTmr_Count_Start(&s_swg.protocolCounter);
		OSTmr_Count_Start(&s_swg.baseCounter);
		OSTmr_Count_Start(&s_swg.btCounter);
		OSTmr_Count_Start(&s_swg.gpsCounter);
		OSTmr_Count_Start(&s_swg.mutualCounter);
		OSTmr_Count_Start(&s_swg.dbgCounter);
		OSTmr_Count_Start(&s_swg.conCounter);
}
/*
*********************************************************************************************************
*                                     OS_SuspendAllTaskProcess
* Description: ���������(˯��ǰ�����Թ����˯�������������������)
* Argument(s): 
* Return(s)  : 
* Note(s)    : 
*********************************************************************************************************
*/
void	OS_SuspendAllTaskProcess (void)
{
		// ����ͨ�Ų����������
		OSTaskSuspend(APP_TASK_GSM_PRIO);
		OSTaskSuspend(APP_TASK_GPRS_PRIO);
		OSTaskSuspend(APP_TASK_RXD_PRIO);
		OSTaskSuspend(APP_TASK_SEND_PRIO);
		OSTaskSuspend(APP_TASK_PROTOCOL_PRIO);
		OSTaskSuspend(APP_TASK_BASE_PRIO);
		OSTaskSuspend(APP_TASK_BT_PRIO);
		OSTaskSuspend(APP_TASK_GPS_PRIO);
		OSTaskSuspend(APP_TASK_MUTUAL_PRIO);
		OSTaskSuspend(APP_TASK_CON_PRIO);
		OSTaskSuspend(APP_TASK_DBG_PRIO);
		// ����OBD�������	
		#ifdef DEF_OBDTASK_CREATE
		OSTaskSuspend(DEF_OBD_TASK_TRANSFERLAYPER_PRIO);
		OSTaskSuspend(DEF_OBD_TASK_SCHEDULE_PRIO);
		OSTaskSuspend(DEF_OBD_TASK_FUN_PRIO);
		OSTaskSuspend(DEF_OBD_TASK_ECU_PRIO);
		#endif
}
/*
*********************************************************************************************************
*                                     OS_ResumeAllTaskProcess
* Description: ����ָ�����(���Ѻ�����Իָ�֮ǰ���������)
* Argument(s): 
* Return(s)  : 
* Note(s)    : 
*********************************************************************************************************
*/
void	OS_ResumeAllTaskProcess (void)
{
		// ����ͨ�Ų����������
		OSTaskResume(APP_TASK_GSM_PRIO);
		OSTaskResume(APP_TASK_GPRS_PRIO);
		OSTaskResume(APP_TASK_RXD_PRIO);
		OSTaskResume(APP_TASK_SEND_PRIO);
		OSTaskResume(APP_TASK_PROTOCOL_PRIO);
		OSTaskResume(APP_TASK_BASE_PRIO);
		OSTaskResume(APP_TASK_BT_PRIO);
		OSTaskResume(APP_TASK_GPS_PRIO);
		OSTaskResume(APP_TASK_MUTUAL_PRIO);
		OSTaskResume(APP_TASK_CON_PRIO);
		OSTaskResume(APP_TASK_DBG_PRIO);
		// ����OBD�������	
		#ifdef DEF_OBDTASK_CREATE
		OSTaskResume(DEF_OBD_TASK_TRANSFERLAYPER_PRIO);
		OSTaskResume(DEF_OBD_TASK_SCHEDULE_PRIO);
		OSTaskResume(DEF_OBD_TASK_FUN_PRIO);
		OSTaskResume(DEF_OBD_TASK_ECU_PRIO);
		#endif
}
/*
*********************************************************************************************************
*                                     SWDGTimeOutCheck
* Description: ������Ź���ʱ�жϴ���
* Argument(s): 
* Return(s)  : 
* Note(s)    : 
*********************************************************************************************************
*/
static 	u8	SWDGTimeOutCheck (void)
{
		vu8	sWDTFlag =0;
		if(OSTmr_Count_Get(&s_swg.gsmCounter)		>= TASK_GSM_TIMOUT) 
		{
				// ���������������б�����
				sWDTFlag =1;
				if(s_common.swdg[0] < 0xff)
						s_common.swdg[0] ++;		
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: GSM task swdg timeout!\r\n");
				#endif
		}
		if(OSTmr_Count_Get(&s_swg.gprsCounter) 	>= TASK_GPRS_TIMOUT) 
		{
				// ���������������б�����
				sWDTFlag =1;
				if(s_common.swdg[1] < 0xff)
						s_common.swdg[1] ++;	
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: GPRS	task swdg timeout!\r\n");
				#endif
		}
		if(OSTmr_Count_Get(&s_swg.rxdCounter) 	>= TASK_RXD_TIMOUT) 
		{
				// ���������������б�����
				sWDTFlag =1;
				if(s_common.swdg[2] < 0xff)
						s_common.swdg[2] ++;		
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: RXD task swdg timeout!\r\n");
				#endif
		}				
		if(OSTmr_Count_Get(&s_swg.sendCounter) 	>= TASK_SEND_TIMOUT) 
		{
				// ���������������й���
				sWDTFlag =1;
				if(s_common.swdg[3] < 0xff)
						s_common.swdg[3] ++;		
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: Sent task swdg timeout!\r\n");
				#endif
		}
		if(OSTmr_Count_Get(&s_swg.protocolCounter) 	>= TASK_PROTOCOL_TIMOUT) 
		{
				// ���������������б�����
				sWDTFlag =1;
				if(s_common.swdg[4] < 0xff)
						s_common.swdg[4] ++;	
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: Protocol task swdg timeout!\r\n");
				#endif
		}		
		if(OSTmr_Count_Get(&s_swg.baseCounter) 	>= TASK_BASE_TIMOUT) 
		{
				// ���������������б�����
				sWDTFlag =1;
				if(s_common.swdg[5] < 0xff)
						s_common.swdg[5] ++;		
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: Base task swdg timeout!\r\n");
				#endif
		}
		if(OSTmr_Count_Get(&s_swg.btCounter) 	>= TASK_BT_TIMEOUT) 
		{
				// ���������������й���
				sWDTFlag =1;
				if(s_common.swdg[6] < 0xff)
						s_common.swdg[6] ++;	
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: Protocol task swdg timeout!\r\n");
				#endif
		}
		if(OSTmr_Count_Get(&s_swg.gpsCounter) 	>= TASK_GPS_TIMOUT)
		{
				// ���������������й���
				sWDTFlag =1;
				if(s_common.swdg[7] < 0xff)
						s_common.swdg[7] ++;
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: GPS task swdg timeout!\r\n");
				#endif
		}
		if(OSTmr_Count_Get(&s_swg.mutualCounter) 	>= TASK_MUTUAL_TIMOUT) 
		{
				// ���������������й���
				sWDTFlag =1;
				if(s_common.swdg[8] < 0xff)
						s_common.swdg[8] ++;		
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: Mutual task swdg timeout!\r\n");
				#endif
		}		
		if(OSTmr_Count_Get(&s_swg.dbgCounter) 	>= TASK_DBG_TIMOUT) 
		{
				// ���������������й���
				sWDTFlag =1;
				if(s_common.swdg[9] < 0xff)
						s_common.swdg[9] ++;	
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: Debug task swdg timeout!\r\n");
				#endif
		}
		if(OSTmr_Count_Get(&s_swg.conCounter) 	>= TASK_CON_TIMOUT) 
		{
				// ���������������й���
				sWDTFlag =1;
				// ������������ݲ����俴�Ź�����
				/*
				if(s_common.swdg[9] < 0xff)
						s_common.swdg[9] ++;	
				*/
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: Contorl task swdg timeout!\r\n");
				#endif
		}
		/*
		// OBD������ʱ�������
		if(OSTmr_Count_Get(&s_swg.obd1Counter) 	>= TASK_OBD1_TIMOUT)
		{
				sWDTFlag =1;
				if(s_common.swdg[10] < 0xff)
						s_common.swdg[10] ++;		
		}		
		if(OSTmr_Count_Get(&s_swg.obd2Counter) 	>= TASK_OBD2_TIMOUT)
		{
				sWDTFlag =1;
				if(s_common.swdg[11] < 0xff)
						s_common.swdg[11] ++;		
		}		
		if(OSTmr_Count_Get(&s_swg.obd3Counter) 	>= TASK_OBD3_TIMOUT)
		{
				sWDTFlag =1;
				if(s_common.swdg[12] < 0xff)
						s_common.swdg[12] ++;		
		}		
		*/		

		////////////////////////////////////
		if(sWDTFlag == 1)
		{
				sWDTFlag =0;
				// �洢					
				HAL_CPU_ResetPro();				// �����λ		
				return	1;	
		}
		return 0;
}
/*
*********************************************************************************************************
*                                     sleepCheckProcess
* Description: ˯���߼���鴦��(��Ҫɨ�����)
* Argument(s): sendCounter ���ڲ��ų�ʱ��idleCounter���ڽ���˯��ǰ�Ŀ��г�ʱ
* Return(s)  : 
* Note(s)    : 
*********************************************************************************************************
*/
static	void	sleepCheckProcess (vu8 *step,	vu32 *idleCounter)
{
		s8	ret=0;
		
		// ˯�߼�� /////////////////////////////////////////////////////////
		if(*step	== 0)
		{	
				if((HAL_SYS_GetState(BIT_SYSTEM_IG)  == 0) && (HAL_SYSALM_GetState(BIT_ALARM_ILLMOVE) == 0))
				{
						if((s_common.iapSta != DEF_UPDATE_WAIT) && 				// �����ȴ������в�����˯��	
							 (s_common.iapSta != DEF_UPDATE_KEYWORD) && 		// ���������в�����˯��						
							 (BSP_GSM_GetFlag(DEF_CALLDOING_FLAG) == 0)	&& 	// �е绰������Ҳ������˯��
					 		 (BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0))
						{
								if(s_osTmp.obdReadCarStaFlag == 1)
								{
										if(OSTmr_Count_Get(&s_osTmp.obdReadCarStaCounter) >= OS_INSLEEP_READCARSTA_TIMEOUT)
										{
												s_osTmp.obdReadCarStaFlag =0;	// ���ȡ����״̬��־
												//*step =1;											// ��ȡ����״̬��ʱֱ�ӽ���˯��													
										}
								}
								else
								{
										// �������ݷ������!(������SMS����)
										if((HAL_ComQue_size (DEF_RAM_QUE) == 0) 	&& (s_ram.len == 0) 	&&
											 (HAL_ComQue_size (DEF_FLASH_QUE) == 0) && (s_flash.len == 0) &&
										   (s_obd.len == 0) && (s_comAck.len == 0))										   
										{	
												if(OSTmr_Count_Get(&s_osTmp.sendDataToCounter) >= OS_INSLEEP_SENDDATA_TIMEOUT)
														s_osTmp.sleepWaitFlag =1;	// ����˯�ߵȴ�
										}
										// �������ݷ���
										else
										{		
												if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
												{
														//OSTmr_Count_Start(&s_osTmp.sendDataToCounter);		// ��ʼ�������ݳ�ʱ��ʱ		
												}
												else
												{
														if(OSTmr_Count_Get(&s_osTmp.sendDataToCounter) >= OS_INSLEEP_SENDDATA_TIMEOUT)
																s_osTmp.sleepWaitFlag =1;	// ����˯�ߵȴ�
																//*step =1;		// �������������������ݳ�ʱ(Ĭ��1����)	
												}
										}
								}
						}
						else	
								OSTmr_Count_Start(&s_osTmp.sendDataToCounter);		// ��ʼ�������ݳ�ʱ��ʱ	
						
						// ����˯�ߵȴ�����
						if(s_osTmp.sleepWaitFlag == 1)
						{
								if(s_cfg.globSleepTime >= s_cfg.igOffDelay)
								{
										if((OSTmr_Count_Get(&s_osTmp.globSleepDelayCounter) >= (s_cfg.globSleepTime - s_cfg.igOffDelay) * 1000) &&
											 (glob_accOnRalStep == 0))		
										{
												*step =1;		// ǿ�Ƶȴ�30���Ӻ�ֱ�ӽ���˯��(����Ҫ�������߼�����ͬ��)
										}
										else
										{
//												// ˯�ߵȴ�������ʵʱ��������Ϣ����������ݲ������˳��ȴ�״̬
//												if((HAL_ComQue_size (DEF_RAM_QUE) == 0) 	&& (s_ram.len == 0) 	&&
//													 (HAL_ComQue_size (DEF_FLASH_QUE) == 0) && (s_flash.len == 0) &&
//													 (s_obd.len == 0) && (s_comAck.len == 0))	
//												{
//														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
//																s_osTmp.sleepWaitFlag =0;	// �˳��ȴ�״̬											
//												}
										}
								}
								else
								{
										if(glob_accOnRalStep == 0)
												*step =1;		// ֱ�ӽ���˯��(����Ҫ�������߼�����ͬ��)
								}
						}						
				}
				else
				{
					  OSTmr_Count_Start(&s_osTmp.sendDataToCounter);		// ��ʼ�������ݳ�ʱ��ʱ
						s_osTmp.sleepWaitFlag =0;
						OSTmr_Count_Start(&s_osTmp.globSleepDelayCounter);		// ��ʼȫ��˯�ߵȴ���ʱ 2015-2-4 by:gaofei ������30���ӵĳ���״̬��ȡʱ��					
				}
		}
		
		// ˯�ߴ��� /////////////////////////////////////////////////////////////
		if(*step >= 1)
		{
				if(*step == 1)
				{
						// �����������ݰ�
						if((s_comPack.packType == 0) && (BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1))
						{
								s_comPack.packType = PACK_TYPE_SLEEP;		// ����˯�����ݰ�			
								OSTmr_Count_Start(idleCounter);		
								*step =2;
						}
						else
						{
								s_osTmp.gsmConSleep =1;	// ����GSMģ�����˯��
								*step =3;
						}
				}
				else if(*step == 2)
				{
						if(OSTmr_Count_Get(idleCounter) >= 3000)
						{
								s_osTmp.gsmConSleep =1;	// ����GSMģ�����˯��
								*step =3;
						}
				}
				else if(*step == 3)
				{
						if(s_osTmp.gsmConSleep == 0)
						{
								*step	=0;		// GSM��������˯��ģʽִ��ʧ���˳�(�е绰����Ż��ٴε��������쳣����)
						}
						else if(s_osTmp.gsmConSleep == 1)
						{
								;						// GSMģ�����˯�ߴ�����
						}
						else if(s_osTmp.gsmConSleep == 2)
						{									
								ret	= HAL_PerpheralInPowerMode();													
								if(ret ==	0)
								{
										// �ɹ�����˯�ߺ�ģʽ
												// ˯����..................
										// ��˯��ģʽ�л���	
										while(1)
										{
												#if(OS_SYSWDG_EN > 0)	
												IWDG_ReloadCounter();						// ι��(����GD32��˵ι�깷����ֱ�ӵ��ý���STOPģʽ,����������RTC ALM���ٽ���STOP)
												#endif											
												SWDGInSleepReload();						// ������Ź�ι��(��ʽ�豸��ʱ��˯�߻��Ѻ���������Ź���λ)									
											
												if(BSP_PVD_GetSta() == 1)
														break;											// PVD<2.9V�¼�ֱ���˳�����˯������ֱ���жϸ�λCPU
												if(rtcWakeUpFlag == 1)
												{
														rtcWakeUpFlag	=0;																
														BSP_RTCSoft_ISRHandler();		// �ֶ�����RTC������
																													
														if(s_cfg.offReportTime != 0)																
														{
																if(RTCTmr_Count_Get(&s_osTmp.igOffMsgCounter) >= s_cfg.offReportTime)
																		break;							// ������������
														}	
														if(1)		// ������ʹ��
														{												
																if(RTCTmr_Count_Get(&s_osTmp.report24Counter) >= s_cfg.healthReportTime)
																		break;							// ��������������(24Сʱ����)					
														}		
														BSP_RTCSetAlarmCounter(OS_RTC_ALM_SECS);
														PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
												}
												else
														break;
										}
										HAL_PerpheralOutPowerMode();	
										//HAL_SendOBDProProcess(CMD_SYS_WAKEUP,	0,	0,	(u8 *)"",	0);		// ֪ͨobd�����豸����		
										if(s_cfg.devEn & DEF_DEVEN_BIT_CON)
												HAL_SendCONProProcess(CAR_CMD_READSTA,	0x0000,	(u8 *)"",	0);	// ֪ͨ��������һ��״̬��ȡ
										
										// �ɻ�����������˯����ʱʱ��
										s_osTmp.sleepWaitFlag =0;	// ���¿�ʼ˯�ߵȴ���ʱ
										if(s_wakeUp.state == 0)
												OSTmr_Count_Start(&s_osTmp.sendDataToCounter);						// �豸��������5���ӳ�ʱ˯��
										else
										{
												if((s_wakeUp.state & DEF_WAKEUP_SHAKE) ||
													 (RTCTmr_Count_Get(&s_osTmp.report24Counter) >= s_cfg.healthReportTime))
														OSTmr_Count_Start(&s_osTmp.sendDataToCounter);				// 2015-8-6 �ɼ��ٶ��񶯼������������豸5���ӽ���˯��
												else
														OSTmr_Count_Start(&s_osTmp.globSleepDelayCounter);		// �豸��������30���ӳ�ʱ˯��
										}											
										*step	=0;													
								}
								else
								{
										OSTmr_Count_Start(&s_osTmp.sendDataToCounter);		// ���¿�ʼ��¼���ͳ�ʱ��ʱ
										*step	=0;		
								}
						}
						else 
								*step	=0;			
				}
				else
						*step	=0;
		}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                             ��APP Event��
*********************************************************************************************************
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                             App_EventCreate()
*
* Description : Create the application events.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskSTART().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  App_EventCreate (void)
{
		INT8U		os_err;	

// �ײ�Ӧ��
//FLASH/////////////////////////////////////////////
		FlashMutexSem	=	OSSemCreate(1);								// ����FLASH SPI���ߵײ�Ӧ��֮��Ļ���
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(FlashMutexSem, (u8 *)"FlashMutexSem", &os_err);
#endif

//MPU6050///////////////////////////////////////////	
		ACCMutexSem	=	OSSemCreate(1);									// ����ACC���ߵײ�Ӧ��֮��Ļ���
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(ACCMutexSem, (u8 *)"ACCMutexSem", &os_err);
#endif

		CrashMEMSSem	=	OSSemCreate(0);								// ��ײ�ź���
#if (OS_EVENT_NAME_SIZE > 12)
    OSEventNameSet(CrashMEMSSem, (u8 *)"CrashMEMSSem", &os_err);
#endif	
	
//GPS///////////////////////////////////////////////
		GPSComSem	=	OSSemCreate(0);										// GPS���ڽ����ź���
#if (OS_EVENT_NAME_SIZE > 9)
    OSEventNameSet(GPSComSem, (u8 *)"GPSComSem", &os_err);
#endif		
	
//GSM///////////////////////////////////////////////
		GSMMutexSem	=	OSSemCreate(1);									// GSM�ײ�Ӧ�û����ź���
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(GSMMutexSem, (u8 *)"GSMMutexSem", &os_err);
#endif		
	
		GSMSmsSem	=	OSSemCreate(0);										// �յ������ź���
#if (OS_EVENT_NAME_SIZE > 9)
    OSEventNameSet(GSMSmsSem, (u8 *)"GSMSmsSem", &os_err);
#endif	

		GSMGprsDataSem	=	OSSemCreate(0);							// GPRS���������ź���
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(GSMGprsDataSem, (u8 *)"GprsDataSem", &os_err);
#endif	

		GSMBtDataMbox = OSMboxCreate((void *)0);  		// ����������������
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(GSMBtDataMbox, (u8 *)"GSMBtDataMbox", &os_err);
#endif

		GSMBtAppMbox = OSMboxCreate((void *)0);  			// ������ȡҵ��״̬����
#if (OS_EVENT_NAME_SIZE > 12)
    OSEventNameSet(GSMBtAppMbox, (u8 *)"GSMBtAppMbox", &os_err);
#endif

//Debug/////////////////////////////////////////////
		DBGComSem	=	OSSemCreate(0);										// DBG�յ������ź���
#if (OS_EVENT_NAME_SIZE > 9)
    OSEventNameSet(DBGComSem, (u8 *)"DBGComSem", &os_err);
#endif		

		DBGMutexSem	=	OSSemCreate(1);									// DBG printf���������Ϣ�����ź���
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(DBGMutexSem, (u8 *)"DBGMutexSem", &os_err);
#endif	

		RamQueMutexSem = OSSemCreate(1);  						// RAM���ݶ��в��������ź���
#if (OS_EVENT_NAME_SIZE > 14)
    OSEventNameSet(RamQueMutexSem, (u8 *)"RamQueMutexSem", &os_err);
#endif

		FlashQueMutexSem = OSSemCreate(1);  					// FLASH���ݶ��в��������ź���
#if (OS_EVENT_NAME_SIZE > 16)
    OSEventNameSet(FlashQueMutexSem, (u8 *)"FlashQueMutexSem", &os_err);
#endif

		QueMutexSem = OSSemCreate(1);  								// ���в��������ź���
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(QueMutexSem, (u8 *)"QueMutexSem", &os_err);
#endif

		ParaMutexSem = OSSemCreate(1);  							// CPU Flash������д�����ź���
#if (OS_EVENT_NAME_SIZE > 12)
    OSEventNameSet(ParaMutexSem, (u8 *)"ParaMutexSem", &os_err);
#endif

		CONComMbox = OSMboxCreate((void *)0);  				// ����ײ������������
#if (OS_EVENT_NAME_SIZE > 10)
    OSEventNameSet(CONComMbox, (u8 *)"CONComMbox", &os_err);
#endif

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OS��Ӧ��
		OS_ACKMbox = OSMboxCreate((void *)0);  		 		// �յ�Ӧ��Э�����ݰ�
#if (OS_EVENT_NAME_SIZE > 10)
    OSEventNameSet(OS_ACKMbox, (u8 *)"OS_ACKMbox", &os_err);
#endif

		OS_DownACKMbox = OSMboxCreate((void *)0);  		 		// �յ�Ӧ��Э�����ݰ�
#if (OS_EVENT_NAME_SIZE > 14)
    OSEventNameSet(OS_DownACKMbox, (u8 *)"OS_DownACKMbox", &os_err);
#endif

		OS_OBDReqMbox = OSMboxCreate((void *)0);  				// ����OBD��������
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(OS_OBDReqMbox, (u8 *)"OS_OBDReqMbox", &os_err);
#endif

		OS_OBDAckMbox = OSMboxCreate((void *)0);  				// OBDӦ������
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(OS_OBDAckMbox, (u8 *)"OS_OBDAckMbox", &os_err);
#endif

		OS_RecGprsMbox = OSMboxCreate((void *)0);  		// �յ�GPRS�����ͽ�������
#if (OS_EVENT_NAME_SIZE > 14)
    OSEventNameSet(OS_RecGprsMbox, (u8 *)"OS_RecGprsMbox", &os_err);
#endif

		OS_RecSmsMbox = OSMboxCreate((void *)0);  		// �յ�SMS�����ͽ�������
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(OS_RecSmsMbox, (u8 *)"OS_RecSmsMbox", &os_err);
#endif

		OS_RecBtMbox = OSMboxCreate((void *)0);  			// �յ�BT�����ͽ�������
#if (OS_EVENT_NAME_SIZE > 14)
    OSEventNameSet(OS_RecBtMbox, (u8 *)"OS_RecBtMbox", &os_err);
#endif

		OS_CONReqMbox = OSMboxCreate((void *)0);  		// ���������������
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(OS_CONReqMbox, (u8 *)"OS_CONReqMbox", &os_err);
#endif

		OS_CONAckMbox = OSMboxCreate((void *)0);  		// �������Ӧ������
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(OS_CONAckMbox, (u8 *)"OS_CONAckMbox", &os_err);
#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
}
/*
*********************************************************************************************************
*                                            App_TaskCreate()
*
* Description : Create the application tasks.
*
* Argument(s) : Note OS_STK_GROWTH Must be '1'
*
* Return(s)   : none.
*
* Caller(s)   : App_TaskSTART().
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  App_TaskCreate (void)
{

		INT8U		os_err;


/*APP GSM Lowlayer Task!!!---------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskGSM,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskGSMStk[APP_TASK_GSM_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_GSM_PRIO,
                             (INT16U          ) APP_TASK_GSM_PRIO,
                             (OS_STK        * )&App_TaskGSMStk[0],
                             (INT32U          ) APP_TASK_GSM_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskGSM,
                        		 (void 					 *) 0,
                       		  &App_TaskGSMStk[APP_TASK_GSM_STK_SIZE - 1],
                        	   APP_TASK_GSM_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_GSM_PRIO, (INT8U *)"GSM Task!", &os_err);
#endif
/*APP EVENT Task!!!---------------------------------------------------------------------------------*/

/*APP GPRS Task!!!---------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskGPRS,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskGPRSStk[APP_TASK_GPRS_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_GPRS_PRIO,
                             (INT16U          ) APP_TASK_GPRS_PRIO,
                             (OS_STK        * )&App_TaskGPRSStk[0],
                             (INT32U          ) APP_TASK_GPRS_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskGPRS,
                        		 (void 					 *) 0,
                       		  &App_TaskGPRSStk[APP_TASK_GPRS_STK_SIZE - 1],
                        	   APP_TASK_GPRS_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 10)
    OSTaskNameSet(APP_TASK_GPRS_PRIO, (INT8U *)"GPRS Task!", &os_err);
#endif
/*APP RXD Task!!!---------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskRXD,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskRXDStk[APP_TASK_RXD_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_RXD_PRIO,
                             (INT16U          ) APP_TASK_RXD_PRIO,
                             (OS_STK        * )&App_TaskRXDStk[0],
                             (INT32U          ) APP_TASK_RXD_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskRXD,
                        		 (void 					 *) 0,
                       		  &App_TaskRXDStk[APP_TASK_RXD_STK_SIZE - 1],
                        	   APP_TASK_RXD_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_RXD_PRIO, (INT8U *)"RXD Task!", &os_err);
#endif														 
/*APP SEND Task!!!---------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskSEND,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskSENDStk[APP_TASK_SEND_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_SEND_PRIO,
                             (INT16U          ) APP_TASK_SEND_PRIO,
                             (OS_STK        * )&App_TaskSENDStk[0],
                             (INT32U          ) APP_TASK_SEND_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskSEND,
                        		 (void 					 *) 0,
                       		  &App_TaskSENDStk[APP_TASK_SEND_STK_SIZE - 1],
                        	   APP_TASK_SEND_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 10)
    OSTaskNameSet(APP_TASK_SEND_PRIO, (INT8U *)"SEND Task!", &os_err);
#endif	
														 
/*APP Protocol Task!!!---------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskPROTOCOL,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskPROTOCOLStk[APP_TASK_PROTOCOL_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_PROTOCOL_PRIO,
                             (INT16U          ) APP_TASK_PROTOCOL_PRIO,
                             (OS_STK        * )&App_TaskPROTOCOLStk[0],
                             (INT32U          ) APP_TASK_PROTOCOL_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskPROTOCOL,
                        		 (void 					 *) 0,
                       		  &App_TaskPROTOCOLStk[APP_TASK_PROTOCOL_STK_SIZE - 1],
                        	   APP_TASK_PROTOCOL_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 14)
    OSTaskNameSet(APP_TASK_PROTOCOL_PRIO, (INT8U *)"Protocol Task!", &os_err);
#endif
														 
/*APP Base Task!!!---------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskBASE,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskBASEStk[APP_TASK_BASE_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_BASE_PRIO,
                             (INT16U          ) APP_TASK_BASE_PRIO,
                             (OS_STK        * )&App_TaskBASEStk[0],
                             (INT32U          ) APP_TASK_BASE_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskBASE,
                        		 (void 					 *) 0,
                       		  &App_TaskBASEStk[APP_TASK_BASE_STK_SIZE - 1],
                        	   APP_TASK_BASE_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 10)
    OSTaskNameSet(APP_TASK_BASE_PRIO, (INT8U *)"BASE Task!", &os_err);
#endif

/*APP BT Lowlayer Task!!!---------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskBT,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskBTStk[APP_TASK_BT_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_BT_PRIO,
                             (INT16U          ) APP_TASK_BT_PRIO,
                             (OS_STK        * )&App_TaskBTStk[0],
                             (INT32U          ) APP_TASK_BT_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskBT,
                        		 (void 					 *) 0,
                       		  &App_TaskBTStk[APP_TASK_BT_STK_SIZE - 1],
                        	   APP_TASK_BT_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 8)
    OSTaskNameSet(APP_TASK_BT_PRIO, (INT8U *)"BT Task!", &os_err);
#endif

/*APP GPS Task!!!---------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskGPS,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskGPSStk[APP_TASK_GPS_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_GPS_PRIO,
                             (INT16U          ) APP_TASK_GPS_PRIO,
                             (OS_STK        * )&App_TaskGPSStk[0],
                             (INT32U          ) APP_TASK_GPS_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskGPS,
                        		 (void 					 *) 0,
                       		  &App_TaskGPSStk[APP_TASK_GPS_STK_SIZE - 1],
                        	   APP_TASK_GPS_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_GPS_PRIO, (INT8U *)"GPS Task!", &os_err);
#endif

/*APP MUTUAL Task!!!--------------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskMUTUAL,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskMUTUALStk[APP_TASK_MUTUAL_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_MUTUAL_PRIO,
                             (INT16U          ) APP_TASK_MUTUAL_PRIO,
                             (OS_STK        * )&App_TaskMUTUALStk[0],
                             (INT32U          ) APP_TASK_MUTUAL_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskMUTUAL,
                        		 (void 					 *) 0,
                       		  &App_TaskMUTUALStk[APP_TASK_MUTUAL_STK_SIZE - 1],
                        	   APP_TASK_MUTUAL_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 12)
    OSTaskNameSet(APP_TASK_MUTUAL_PRIO, (INT8U *)"MUTUAL Task!", &os_err);
#endif		
/*APP CON Task!!!--------------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskCON,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskCONStk[APP_TASK_CON_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_CON_PRIO,
                             (INT16U          ) APP_TASK_CON_PRIO,
                             (OS_STK        * )&App_TaskCONStk[0],
                             (INT32U          ) APP_TASK_CON_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskCON,
                        		 (void 					 *) 0,
                       		  &App_TaskCONStk[APP_TASK_CON_STK_SIZE - 1],
                        	   APP_TASK_CON_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_CON_PRIO, (INT8U *)"CON Task!", &os_err);
#endif	
/*APP DBG Task!!!--------------------------------------------------------------------------------------*/
#if (OS_TASK_CREATE_EXT_EN > 0)	
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskDBG,
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskDBGStk[APP_TASK_DBG_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_DBG_PRIO,
                             (INT16U          ) APP_TASK_DBG_PRIO,
                             (OS_STK        * )&App_TaskDBGStk[0],
                             (INT32U          ) APP_TASK_DBG_STK_SIZE,
                             (void          * ) 0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskDBG,
                        		 (void 					 *) 0,
                       		  &App_TaskDBGStk[APP_TASK_DBG_STK_SIZE - 1],
                        	   APP_TASK_DBG_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 9)
    OSTaskNameSet(APP_TASK_DBG_PRIO, (INT8U *)"DBG Task!", &os_err);
#endif		



														 
}

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Argument(s) : none.
*
* Return(s)   : none.
*********************************************************************************************************
*/
int  main (void)
{
    INT8U  os_err;

		//����ʹ��UCOS,���µĳ�ʼ����Ȼ������OS����֮ǰ����,��ע���ʹ���κ��ж�.
		//�ر������ж�
		BSP_CPU_DisAllInterupt();
	
		HAL_INIT_ALL();

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel".         */

#if (OS_TASK_CREATE_EXT_EN > 0)
    os_err = OSTaskCreateExt((void (*)(void *)) App_TaskSTART,  /* Create the start task.                               */
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskSTARTStk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO,
                             (INT16U          ) APP_TASK_START_PRIO,
                             (OS_STK        * )&App_TaskSTARTStk[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
#else
		os_err = OSTaskCreate		 ((void (*)(void *)) App_TaskSTART,
                        		 (void 					 *) 0,
                       		  &App_TaskSTARTStk[APP_TASK_START_STK_SIZE - 1],
                        	   APP_TASK_START_PRIO);
#endif

#if (OS_TASK_NAME_SIZE >= 11)
    OSTaskNameSet(APP_TASK_START_PRIO, (INT8U *)"Start Task!", &os_err);
#endif

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II).  */

		return (0);
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                             ��APP Task��
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                             App_TaskSTART()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskSTART()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  App_TaskSTART (void *p_arg)
{	
		vu8	sleepStep=0;
		vu16	pvdCheckTimes=0;
		vu32	iwgCounter =0,	rtcCounter	=0,	idleCounter =0, checkCounter =0,	rstCount =0;
					
		p_arg = p_arg;
	
    OS_CPU_SysTickInit();                                       /* Initialize the SysTick.*/

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity*/
#endif

    App_EventCreate();                                          /* Create application events*/
    App_TaskCreate();                                           /* Create application tasks.*/
	
#ifdef DEF_OBDTASK_CREATE
		fun_obd_task_create();		// OBD���񴴽�	
#endif
	
		// ���ϵͳ������ //////////////////
		BSP_CPU_FLASH_ProCheck(FLASH_CPU_COMMON_START);
		BSP_CPU_FLASH_ProCheck(FLASH_CPU_SYSCFG_START);
		BSP_CPU_FLASH_ProCheck(FLASH_CPU_SYSOBD_START);

		// ʹ��RTC ALARM�ж�	//////////////
		BSP_RTC_IT_CON(DEF_RTC_ALARM_IT,	DEF_ENABLE);

		OSTmr_Count_Start(&iwgCounter);				// Ӳ�����Ź�������	
		OSTmr_Count_Start(&rtcCounter);				// RTC������			
		OSTmr_Count_Start(&checkCounter);
		OSTmr_Count_Start(&idleCounter);

		pvdCheckTimes=0;
		sleepStep=0;								// ������Ҫ�ֶ������������ÿ�ν�������ʱ�����п���Ϊ1��2(��ʼ��Ϊ0��������)
		
		while (1)																										/* Task body, always written as an infinite loop*/
		{                                          															
				// ���Ź����� /////////////////////////////////////////////////////////////////////
				#if(OS_SYSWDG_EN > 0)
				if(OSTmr_Count_Get(&iwgCounter) >= OS_FEEDWDG_TIME * 1000)
				{
						IWDG_ReloadCounter();						// ι��	
						OSTmr_Count_Start(&iwgCounter);				
				}
				#endif

				// RTC���ˢ�´��� ////////////////////////////////////////////////////////////////
				if(OSTmr_Count_Get(&rtcCounter) >= OS_RTC_REFRESH * 1000)
				{
						BSP_RTCSoft_ISRHandler();				// ����RTC������
						OSTmr_Count_Start(&rtcCounter);
				}

				// ������Ź�������ʱ�ж� ///////////////////////////////////////////////////////
				if(SWDGTimeOutCheck() == 1)
				{
						while(1);
				}

				// ˯�������̴��� ///////////////////////////////////////////////////////////////////
				#if(WOK_SLEEPMODE_EN > 0)
				// PVDɨ��
			 	if(BSP_PVD_GetSta() == 1)	
				{
						pvdCheckTimes++;
						if(pvdCheckTimes >= 200)
						{
								pvdCheckTimes	=0;
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[ERROR]: PVD power (<2.9V), CPU will reset now!\r\n");
								#endif	
								HAL_CPU_ResetPro();
						}
				}
				else
				{
						pvdCheckTimes =0;	
						if(globInitPVDFlag == OS_INITPVD_FLAG)	 		// �ϵ�PVD�쳣�����λ
						{
								globInitPVDFlag =0;
								HAL_CPU_ResetPro();				
						}	
				}
				
				// �豸���ϵ�5�Ӻ����˯�߼������(��ֹϵͳһ�ϵ�ֱ�ӽ���˯��)			
				if(OSTmr_Count_Get(&checkCounter) >= OS_RST_CHECKSLEEP_TIME)		
						sleepCheckProcess (&sleepStep,	&idleCounter);				
				#endif	
				
				// ϵͳ����µĸ�λ���� ///////////////////////////////////////////////////////////////////
				if((s_osTmp.resetReq == DEF_RESET_PRI0) || (s_osTmp.resetReq == DEF_RESET_PRI1) || (s_osTmp.resetReq == DEF_RESET_WAIT))
				{
						rstCount++;
						if(s_osTmp.resetReq == DEF_RESET_PRI0) // �����ʱ10��
						{								
								if((rstCount >= 1000) || 
									 ((HAL_ComQue_size (DEF_SMS_QUE) == 0) && (HAL_ComQue_size(DEF_RAM_QUE) == 0) && (HAL_ComQue_size(DEF_FLASH_QUE) == 0) &&
								    (tmpSms.len == 0) && (s_ram.len == 0) && (s_flash.len == 0)))
								{																					
										rstCount =0;										
										s_osTmp.resetReq =DEF_RESET_WAIT;	
								}
						}	
						else if(s_osTmp.resetReq == DEF_RESET_PRI1)	// �����ʱ180��
						{								
								if((rstCount >= 18000) || 
									 ((HAL_ComQue_size (DEF_SMS_QUE) == 0) && (HAL_ComQue_size(DEF_RAM_QUE) == 0) && (HAL_ComQue_size(DEF_FLASH_QUE) == 0) &&
								    (tmpSms.len == 0) && (s_ram.len == 0) && (s_flash.len == 0)))
								{
										rstCount =0;
										s_osTmp.resetReq =DEF_RESET_WAIT;	
								}
						}	
						else if(s_osTmp.resetReq == DEF_RESET_WAIT)	// ������ɵȴ�3s��λ(����GSMģ�鷢��ʱ��)
						{
								if(rstCount >= 300)
								{
										if(s_comPack.packType == 0)
												s_comPack.packType = PACK_TYPE_SHUTNET;		// ���Ͷ������ݰ�										
										
										HAL_CPU_ResetPro();
								}
						}
						else
								HAL_CPU_ResetPro();	// �Ƿ�����ֱ�Ӹ�λ		
				}
				else
						s_osTmp.resetReq =0;										
				
				// �ͷ�CPUʱ�� ////////////////////			
				OSTimeDly(10);
		}
}
/*
*********************************************************************************************************
*                                            App_TaskGSM()
*
* Description : 
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void		App_TaskGSM	(void 	*p_arg)
{	
		vu32	gsmTimer =0;
		
		p_arg = p_arg;

		// ����GSM����������Ź�  /////////////
		OSTmr_Count_Start(&s_swg.gsmCounter);
	
		while(1)
		{																			  									
				// GSMģ��ײ�ʱ�Ӵ���  /////////// 
				gsmTimer++;
				if(gsmTimer>=100)
				{
						gsmTimer = 0;
						s_GSMcomm.Timer++;
						s_GSMcomm.TimerApp++;
						OSTmr_Count_Start(&s_swg.gsmCounter);		// �������	
				}	
				// GSM�շ��������� ////////////////	
				BSP_GSM_TransPro();	
				// GSM������ ////////////////////		
				BSP_GSM_CommonPro();	
				// �ͷ�CPUʱ�� ////////////////////			
				OSTimeDly(1);
		}
}

#define APP1_ADDRESS 0x08030000
typedef void(*pFunction)(void);
uint32_t JumpAddress;
pFunction Jump_To_Application;

void JumpToApp1(void)
{
	  JumpAddress = *(__IO uint32_t*)(APP1_ADDRESS + 4);
	  Jump_To_Application = (pFunction)JumpAddress;
	  __set_PSP(*(vu32*)APP1_ADDRESS);
	  __set_CONTROL(0);
	  __set_MSP(*(__IO uint32_t*)APP1_ADDRESS); 
	  Jump_To_Application();
}
/*
*********************************************************************************************************
*                                            App_TaskGPRS()
*
* Description : GPRS������SMS�ظ�����
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
 static u8 connectappiperr = 0;	
static	void		App_TaskGPRS	(void 	*p_arg)
{	
		s8 gsmRet=0;
		u8 i=0,qSimFlag=0,qImeiFlag=0,btCfgFlag=0;
		vu32	gsmMonCounter=0, simpCounter=0, netCfgCounter=0,	cNetCounter=0,	imeiCounter=0,	btInitCounter=0,	btInitTimes=0;
		vu32	csqCounter=0, simWCounter=0;
		u32		baseGTime=2000;
		SYS_DATETIME	tmpRtc;
   
		
		// GSMģ���ʼ��������  //////////////
		BSP_GSM_Init();		
		BSP_GSM_POW_RESET(500);  // �ϵ�����¸�λģ���Դ			

		// ʹ�ܸ�������  /////////////////////
		OSTmr_Count_Start(&cNetCounter);
		OSTmr_Count_Start(&gsmMonCounter);
		OSTmr_Count_Start(&simpCounter);
		OSTmr_Count_Start(&netCfgCounter);	
		OSTmr_Count_Start(&imeiCounter);	
		OSTmr_Count_Start(&csqCounter);
		OSTmr_Count_Start(&simWCounter);
	
		// ����Ӧ��IP��ʱ��ʱ��///////////////
		RTCTmr_Count_Get(&s_osTmp.appIPWorkCounter);

		// ����GPRS����������Ź�  ///////////
		OSTmr_Count_Start(&s_swg.gprsCounter);

		while(1)
		{		
				// GSM�������� /////////////////////////////////////////////////////////////////
				BSP_GSM_Setup();			

				// SIM����ʼ������
				if(OSTmr_Count_Get(&simpCounter) >= 1000)
				{
						if(qSimFlag == 0)
						{
								gsmRet =0;
								gsmRet =BSP_GSMSimCardInitPro (s_gsmApp.simNum,	3000);
								if(gsmRet ==	DEF_GSM_NONE_ERR)
								{
										s_osTmp.simErrTimes =0;	// ��SIM�����ϴ���
										qSimFlag =1;
								}
								else
								{
										if(gsmRet == DEF_GSM_CPIN_ERR)
										{
												if(s_osTmp.simErrTimes < 0xffff)
														s_osTmp.simErrTimes++;	// SIM�����ۼ�
										}
								}
						}
						OSTmr_Count_Start(&simpCounter);	
				}
				
				// SIM����д�봦��
				if(OSTmr_Count_Get(&simWCounter) >= 1000)
				{
						if(s_gsmApp.simNumWriteFlag == 1)
						{
								if(BSP_WriteSimCardNumPro (s_gsmApp.simNum,	3000) ==	DEF_GSM_NONE_ERR)	
										s_gsmApp.simNumWriteFlag =0;
						}
						OSTmr_Count_Start(&simWCounter);
				}
				
				// GSM�����ش���(Ӧ�ò���) ///////////////////////////////////////////////////
				if(OSTmr_Count_Get(&gsmMonCounter) >= 1000)	
				{
					 	if((BSP_GSM_ERR_Que(&err_Gsm.gsmErrorTimes)  >= OS_GSM_MAXERR_TIME) ||
							 (APP_ErrQue(&s_osTmp.cnnErrTimes) >= 5)	||
							 (APP_ErrQue(&s_osTmp.ackErrTimes) >= 5)  ||
						   (s_osTmp.simErrTimes >= OS_SIM_MAXERR_TIME))
						{
								// д��λ��������
								if(APP_ErrQue(&s_err.gsmResetTimes) < 0xff)
								{
										APP_ErrAdd(&s_err.gsmResetTimes);										
								}
								////////////////////////////////////////////////
								BSP_GSM_ERR_Clr(&err_Gsm.gsmErrorTimes);
								APP_ErrClr(&s_osTmp.cnnErrTimes);
								APP_ErrClr(&s_osTmp.ackErrTimes);		// ��Ӧ��������
								s_osTmp.simErrTimes =0;							// ��SIM�����ϴ���	

								qSimFlag=0;	qImeiFlag=0;	btCfgFlag=0;										
								BSP_GSMHardWareReset(5000);					// �����λʧ��ֱ��ִ��Ӳ����λ							
						}
						OSTmr_Count_Start(&gsmMonCounter);				
				}
			
				// ȡ�豸IMEI���� ///////////////////	
				/*
				if(OSTmr_Count_Get(&imeiCounter) >= 10000)
				{	
						if(qImeiFlag == 0)	
						{
								if(BSP_QIMEICodePro(s_gsmApp.IMEI,	1000) ==	DEF_GSM_NONE_ERR)
										qImeiFlag	=1;		// ֻ��ȡһ��			
						}	
						OSTmr_Count_Start(&imeiCounter);
				}	
				*/

				// �����ź�ǿ��//////////////////////////////////////////////////////////////
				/*
				if(OSTmr_Count_Get(&csqCounter) >= 10000)
				{	
						if(s_common.iapSta != DEF_UPDATE_KEYWORD)		// ���������в���ѯ�ź�ǿ��
								BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	
						OSTmr_Count_Start(&csqCounter);	
				}
				*/
							        
        // ˯��ǰGSMģ�����͹���ģʽ����(ֻ����˯�߹����еĶ�������)///////////////
        if(s_osTmp.gsmConSleep == 1)
        {
        		// ģ��Ͽ�GPRS����
						if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
						{
								for(i=0; i<OS_LP_SHUTNET_TINES; i++)
								{
										if((HAL_SYS_GetState(BIT_SYSTEM_IG)  == 0)		&&
											 (BSP_GSM_GetFlag(DEF_CALLDOING_FLAG) == 0)	&& 
											 (BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) 	== 0))							
										{
												if(BSP_TCPShutNetPro(5000) == DEF_GSM_NONE_ERR)
														break;						
										}
										else
												break;	// �˳�˯��ģʽ										
								}	
								if(i >= OS_LP_SHUTNET_TINES)
								{
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[LOWPOW]-> GSM fail shut net!\r\n");
										#endif	 
								}
								else
								{
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[LOWPOW]-> GSM Success shut net!\r\n");
										#endif						
								}
						}
						// ģ�����˯��ģʽ
						if((HAL_SYS_GetState(BIT_SYSTEM_IG)  == 0)	&&
							 (BSP_GSM_GetFlag(DEF_CALLDOING_FLAG) == 0)	&& 
							 (BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) 	== 0))
						{
								for(i=0; i<OS_LP_GSMSLEEP_TINES; i++)
								{
										if(BSP_GSMIntoSleep(2000) == DEF_GSM_NONE_ERR)
												break;
								}	
								if(i >= OS_LP_GSMSLEEP_TINES)
								{
										// ģ����Ͻ��ر�GSMģ���Դ
										qSimFlag=0;	qImeiFlag=0;
										BSP_GSMHardWarePowShut();		
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[LOWPOW]-> GSM fail into sleep mode!\r\n");
										#endif
								}
								else
								{
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[LOWPOW]-> GSM success into sleep mode!\r\n");
										#endif						
								}
								s_osTmp.gsmConSleep  =2;		// ���۳ɹ����ֱ�ӷ���˯�߽���
								OSTimeDly(3000);						// ˯�߳ɹ�����Ҫ�����豸3���ֹ�豸���Ѻ�ֱ�ӽ�����������	
						}
						else
						{
								s_osTmp.gsmConSleep =0;			// �����˳�˯��ģʽ�ж�
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[LOWPOW]-> Break sleep mode!\r\n");
								#endif
						}
        }
				
				// ��ʼ������ģ��(��������������)///////////////////////////////////////////// 
				if(s_cfg.devEn & DEF_DEVEN_BIT_BT)
				{
						if(s_common.iapSta != DEF_UPDATE_KEYWORD)	// ���������в����Գ�ʼ������
						{
								if(btCfgFlag == 0)	
								{
										btInitTimes++;
										if(btInitTimes < 20)
										{
												if(OSTmr_Count_Get(&btInitCounter) >= 10000)
												{
														if(BSP_BTInitPro((u8 *)DEF_BT_NAME, 1000) == DEF_GSM_NONE_ERR)
														{
																btCfgFlag =1;									// ��ʼ���ɹ��󽫲��ٽ���
																#if(DEF_BTINFO_OUTPUTEN > 0)							
																if(dbgInfoSwt & DBG_INFO_BT)
																		myPrintf("[BT]: Init OK!\r\n");
																#endif
														}
														else
														{								
																//#if(DEF_BTINFO_OUTPUTEN > 0)	// ��ʼ��ʧ�ܲ������						
																//if(dbgInfoSwt & DBG_INFO_BT)
																//		myPrintf("[BT]: Init fail!\r\n");
																//#endif					
														}
														OSTmr_Count_Start(&btInitCounter);
												}										
										}
										else
												btCfgFlag =1;	// ����һ�������󽫲��ٳ��Գ�ʼ������
								}
						}
				}
				
				// TTSʵʱ������������///////////////////////////////////////////// 
				if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
				{
						if(s_conDev1.dataLock == 1)
						{
								if((s_conDev1.type == TTS_CMD) && (s_conDev1.paraLen != 0x00))
								{
										if(BSP_TTSPlayPro(0,	s_conDev1.paraBuf, s_conDev1.paraLen,	3000) ==DEF_GSM_NONE_ERR)
												memset((u8 *)&s_conDev1,	0,	sizeof(s_conDev1));	// ���������ṹ����(ͬʱ���ݽ���)
										else
												OSTimeDly(1000);	// ����ʧ�ܵȴ�1s������
								}
						}
						else if(s_conDev2.dataLock == 1)
						{
								if((s_conDev2.type == TTS_CMD) && (s_conDev2.paraLen != 0x00))
								{
										if(BSP_TTSPlayPro(0,	s_conDev2.paraBuf, s_conDev2.paraLen,	3000) ==DEF_GSM_NONE_ERR)
												memset((u8 *)&s_conDev2,	0,	sizeof(s_conDev2));	// ���������ṹ����(ͬʱ���ݽ���)
										else
												OSTimeDly(1000);	// ����ʧ�ܵȴ�1s������
								}
						}
				}
								
				// GSM�Զ����� //////////////////////////////////////////////////////////
				// ��¼������������1)������GPRSʹ��;2)�豸���ڶ���״̬;3)�޵绰�����Ų���;
				if((s_wakeUp.state & DEF_WAKEUP_RING) || (s_wakeUp.state & DEF_WAKEUP_IG))	
				{
						// 2015-5-30 ֻ�е绰����ż�IG���Ѳ��ӳ�����
						/*
						if(s_wakeUp.state & DEF_WAKEUP_RING)
								s_wakeUp.state	&= ~DEF_WAKEUP_RING;
						else if(s_wakeUp.state & DEF_WAKEUP_IG)
								s_wakeUp.state	&= ~DEF_WAKEUP_IG;
						*/						
						s_wakeUp.state =0;	// �廽��Դ��־
						baseGTime	=	5000;		// ֻҪ�豸��˯���л��Ѿ��ӳ�5�벦��(����TTS������������Ų�ѯ����ʱ��)				
						OSTmr_Count_Start(&cNetCounter);
				}			
				if(OSTmr_Count_Get(&cNetCounter) >= baseGTime)	
				{						
						if((qSimFlag == 1)                            &&	// ��Ҫ�ȴ�SIM����ʼ�����
							 (s_cfg.devEn & DEF_DEVEN_BIT_GPRS) 				&& 
					 		 (s_GSMcomm.GSMComStatus	==	GSM_ONCMD)		&&
					 		 (BSP_GSM_GetFlag(DEF_CALLDOING_FLAG) == 0) && 
					 		 (BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0))
						{	
								// ����ǰ��ѯ�µ�ǰ�ź�ǿ��
								BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);
								// ������������ //////////////////////////////////////////////////////////////
								gsmRet =DEF_GSM_NONE_ERR;
								gsmRet =BSP_GPRSResetPro(s_cfg.apn,	s_cfg.apnUser,	s_cfg.apnPass,	5000);
								if(gsmRet == DEF_GSM_NONE_ERR)
								{
										gsmRet =DEF_GSM_NONE_ERR;									
										#if(OS_NAVIREQ_EN > 0)	
										if(s_osTmp.naviStep == 0)
										{
												gsmRet = BSP_TCPConnectPro(DEF_GPRS_DATA_CNN0,	s_cfg.ip1, s_cfg.port1,	5000);		// ���ӵ���IP
										}
										else if(s_osTmp.naviStep == 1) //WJH_TEST
                    {
											  if(strcmp((char*)s_cfg.port2,"4000")==0)
												{
													   myPrintf("JUMP to APP2 ,TCP IP:%s PORT:%s \r\n" ,s_cfg.ip2 ,s_cfg.port2);
														 GPIO_ResetBits		(GPIOC, BSP_GPIOC_GSM_DTR | BSP_GPIOC_GSM_POWEN | BSP_GPIOC_SPAKEREN);
														 GPIO_SetBits		(GPIOA, BSP_GPIOA_GSM_POWON);
														 OSTimeDly(5000);
														 USART_DeInit(USART1);
														 USART_Cmd(USART1,DISABLE);
														 USART_DeInit(USART2);
														 USART_Cmd(USART2,DISABLE);	
														 USART_DeInit(USART3);
														 USART_Cmd(USART3,DISABLE);	
														 USART_DeInit(UART4);
														 USART_Cmd(UART4,DISABLE);	
														 USART_DeInit(UART5);
														 USART_Cmd(UART5,DISABLE);
														 BSP_SYSEXIT_Init();	
														 JumpToApp1();
												}
												gsmRet = BSP_TCPConnectPro(DEF_GPRS_DATA_CNN0,	s_cfg.ip2, s_cfg.port2,	5000);		// ����Ӧ��IP													
										#else
												gsmRet = BSP_TCPConnectPro(DEF_GPRS_DATA_CNN0,	s_cfg.ip1, s_cfg.port1,	5000);		// ���ӵ���IP
										#endif
										}
								}
								// ʧ�ܴ��� //////////////////////////////////////////////////////////////
								if(gsmRet == DEF_GSM_NONE_ERR)
								{		
										#if(DEF_GPRSINFO_OUTPUTEN > 0)							
										if(dbgInfoSwt & DBG_INFO_GPRS)
										{		
												BSP_RTC_Get_Current(&tmpRtc);	// ��ȡ��ǰRTCʱ��
												myPrintf("\r\n[%02d/%02d/%02d %02d:%02d:%02d][GPRS-Cnn]: Success!\r\n",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
										}
										#endif
										baseGTime = 2000;										// �ɹ���2���ѯһ���Ƿ����
										APP_ErrClr(&s_osTmp.ackErrTimes);		// ���ųɹ�����Ӧ�����
										APP_ErrClr(&s_osTmp.cnnErrTimes);		// �������ɹ�����  
										s_osTmp.appIPCnnErrTimes=0;					// �����ɹ�����������ʧ�ܴ���
								}
								else
								{	
										/*
										#if(DEF_GPRSINFO_OUTPUTEN > 0)							
										if(dbgInfoSwt & DBG_INFO_GPRS)
										{											
												BSP_RTC_Get_Current(&tmpRtc);	// ��ȡ��ǰRTCʱ��
												myPrintf("\r\n[%02d/%02d/%02d %02d:%02d:%02d][GPRS-Cnn]: Fail!\r\n",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
										}
										#endif
										*/
										if((gsmRet == DEF_GSM_CREG_ERR) || (gsmRet == DEF_GSM_CPIN_ERR))
										{
												if(gsmRet == DEF_GSM_CPIN_ERR)
														qSimFlag =0;		// SIM���ڹ����б��Ƴ��򴥷�SIM�����³�ʼ��						
												baseGTime = 1000;										// GSM����ע�������޿�1s������
										}
										else 
										{														
												// ģ���������ϴ���ͳ��
												if((gsmRet == DEF_GSM_CIFSR_ERR) || (gsmRet == DEF_GSM_CIPSTART_ERR)) 
												{
														if(BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	2000) == DEF_GSM_NONE_ERR)
														{												
																if(s_gsmApp.sLeve >= 20)
																		APP_ErrAdd(&s_osTmp.cnnErrTimes);	//	����ʧ�ܴ����ۼӵ�һ������Ӳ����λģ��
														}
												}			
												// Ӧ��IP�������ͳ��
												if(gsmRet == DEF_GSM_CIPSTART_ERR)
												{
														if(s_osTmp.appIPCnnErrTimes < 0xff)
																s_osTmp.appIPCnnErrTimes ++;												
												}
												baseGTime = 5000;			// ʧ�ܺ�5�볢����һ�β���
										}
								}
						}
						else
								baseGTime = 1000;						// ����������1����һ�β�������

						OSTmr_Count_Start(&cNetCounter);
				}			
				
				// ����������� ////////////////////							
				OSTmr_Count_Start(&s_swg.gprsCounter);
				OSTimeDly(50);	
		}
}

/*
*********************************************************************************************************
*                                            App_TaskRXD()
*
* Description : 
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void		App_TaskRXD	(void 	*p_arg)
{
		GPRS_Typedef	s_gprs;
		BT_Typedef		s_bt;
		SMS_Typedef		s_sms;	
		SYS_DATETIME	tmpRtc;
		BTMbox_Typedef	*bMsg;
	
		u8	i=0,smsIndex=0,smsHbuf[3]={0},tmpBuf[160]={0};
		u16	tmpLen=0;
		s8	ret=0;

		p_arg = p_arg;

		// ��������������Ź�  /////////////
		OSTmr_Count_Start(&s_swg.rxdCounter);
		
		while(1)
		{			
				// GPRS���ݴ���
				if(OSSemAccept(GSMGprsDataSem) > 0) 
				{		
						while(1)
						{
								OSTmr_Count_Start(&s_swg.rxdCounter);		// ������������													
								if(((s_common.iapSta != DEF_UPDATE_KEYWORD) && (s_osTmp.rxdDataLock == 0)) || 
									 ((s_common.iapSta == DEF_UPDATE_KEYWORD) && (s_update.writeLock == 0))) 		// ���������Ҫ�ж�������
								{
										memset((u8 *)&s_gprs,	'\0',	sizeof(s_gprs));
										if(s_common.iapSta != DEF_UPDATE_KEYWORD)
												ret =BSP_TCPRxdData(DEF_GPRS_DATA_CNN0,	(u8 *)(&s_gprs.dataBuf),	&s_gprs.dataLen,	40000);
										else
												ret =BSP_TCPRxdDataUpdata(DEF_GPRS_DATA_CNN0,	(u8 *)(&s_gprs.dataBuf),	&s_gprs.dataLen,	10000); 		
										
										if((ret == DEF_GSM_NONE_ERR) || (ret == DEF_GSM_READ_AGAIN))
										{											
												// ��ӡGPRS�յ�������
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
												{				
														if(s_common.iapSta != DEF_UPDATE_KEYWORD)	// ���������в���ӡRTCʱ���
																BSP_RTC_Get_Current(&tmpRtc);	// ��ȡ��ǰRTCʱ��
														memset(tmpBuf,	'\0',	sizeof(tmpBuf));
														if((s_gprs.dataLen * 2) > sizeof(tmpBuf))
																tmpLen = Hex2StringArray ((u8 *)(&s_gprs.dataBuf),	(sizeof(tmpBuf)/2),	tmpBuf);
														else
																tmpLen = Hex2StringArray ((u8 *)(&s_gprs.dataBuf),	s_gprs.dataLen,	tmpBuf);
														if((tmpLen != 0xffff) && (tmpLen != 0))
														{
																//myPrintf("\r\n[GPRS-Rxd]: %c,",	gMsg->cnn);																
																myPrintf("\r\n%[%02d/%02d/%02d %02d:%02d:%02d][GPRS-Rxd]: ",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);																
																BSP_OSPutChar(DEF_DBG_UART, tmpBuf,	tmpLen);
																if(s_gprs.dataLen * 2 > sizeof(tmpBuf))
																		myPrintf("...\r\n");
																else
																		myPrintf("\r\n");
														}
												}
												#endif
												s_update.writeLock =1;	// ��������
												s_osTmp.rxdDataLock	=1;	// ������ģʽ������
												OSMboxPost(OS_RecGprsMbox, (void *)&s_gprs);		// �������俪ʼ��������	
												
												// �ж��Ƿ���Ҫ�ض�
												if(ret == DEF_GSM_NONE_ERR)
														break;
												else if(ret == DEF_GSM_READ_AGAIN)
												{
														// ����Ҫ�ٶ�ȡһ��
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Rxd]: need read again!!\r\n");
														#endif		
												}												
										}	
										else
										{
												// ����ֱ�ӷ���
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("\r\n[GPRS-Rxd]: read error %-d!!\r\n",ret);
												#endif
												break;
										}	
								}
								else
										OSTimeDly(10);	// �ȴ�
						}
				}
				// BT���ݴ���
				bMsg = (BTMbox_Typedef *)(OSMboxAccept(GSMBtDataMbox));
				if(bMsg != (void *)0)
				{
						if(s_common.iapSta != DEF_UPDATE_KEYWORD)		// ���������в�������������
						{
								OSTmr_Count_Start(&s_swg.rxdCounter);		// ������������						
								memset((u8 *)&s_bt,	'\0',	sizeof(s_bt));							
								if(bMsg->func == BT_RXDATA)	// ���ݽ��մ���
								{
										if(BSP_BTRxdDataPro((u8 *)(&s_bt.dataBuf),	&s_bt.dataLen,	6000) == DEF_GSM_NONE_ERR)
										{
												// ��ӡBT�յ�������
												#if(DEF_BTINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_BT)
												{
														memset(tmpBuf,	'\0',	sizeof(tmpBuf));
														tmpLen = Hex2StringArray ((u8 *)(&s_bt.dataBuf),	s_bt.dataLen,	tmpBuf);
														if(tmpLen != 0xffff)
														{
																myPrintf("\r\n[BT-Rxd]: %c,",	bMsg->cnnId);
																BSP_OSPutChar(DEF_DBG_UART, tmpBuf,	tmpLen);
																myPrintf("\r\n");
														}
												}
												#endif
												OSMboxPost(OS_RecBtMbox, (void *)&s_bt);		// �������俪ʼ��������	
										}
								}
						}
				}
				// SMS���Ŵ���
				if(OSSemAccept(GSMSmsSem) > 0) 
				{
						if(s_common.iapSta != DEF_UPDATE_KEYWORD)		// ���������в���ȡ����
						{
								OSTimeDly(1000);	// �����豸��˯�ߵ�����δ׼����

								smsIndex  =1;						
								while(smsIndex < 20)
								{						
										OSTmr_Count_Start(&s_swg.rxdCounter);		// ������������						
										memset((u8 *)&s_sms,	'\0',	sizeof(s_sms));
										sprintf((char *)smsHbuf,	"%d",	smsIndex);
										ret	=	BSP_SMSRead(smsHbuf,	&(s_sms.sTime),	s_sms.numBuf,	s_sms.dataBuf,	&(s_sms.dataLen),	6000);
										if(ret == DEF_GSM_NONE_ERR)
										{
												#if(DEF_SMSINFO_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_SMS)
												{
														myPrintf("\r\n[SMS-Rxd]: \r\n");
														myPrintf("[SMS-Idx]%s\r\n",	smsHbuf);
														myPrintf("[SMS-Num]%s\r\n",	s_sms.numBuf);
														myPrintf("[SMS-Len]%d\r\n",	s_sms.dataLen);
														myPrintf("[SMS-Dat]%s\r\n",	s_sms.dataBuf);
														myPrintf("[SMS-Tim]%d-%d-%d %d:%d:%d\r\n",	s_sms.sTime.year,s_sms.sTime.month,s_sms.sTime.day,s_sms.sTime.hour,s_sms.sTime.minute,s_sms.sTime.second);		
												}
												#endif

												OSMboxPost(OS_RecSmsMbox, (void *)&s_sms);		// �������俪ʼ��������		
												// ɾ������
												BSP_SMSDelete(smsHbuf,	8000);
										}
										else if(ret == DEF_GSM_SMS_ENPTY)
										{
												// ���Ŵ洢���Ѿ�Ϊ��
												break;
										}	
										smsIndex++;						
								}
						}
						// �ͷű�־
						BSP_GSM_ReleaseFlag(DEF_SMSDOING_FLAG);
				}
				
				// ����������� ////////////////////			
				OSTmr_Count_Start(&s_swg.rxdCounter);		// ������������	
				OSTimeDly(10);			
		}
}
/*
*********************************************************************************************************
*                                            App_TaskSEND()
*
* Description : 
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void		App_TaskSEND	(void 	*p_arg)
{
		INT8U		os_err=0;		
		s8 ret=0;
		u16 i=0;
		vu32	heartBCounter=0;	// ���������ͼ�ʱ��			
		SYS_DATETIME	tmpRtc;	
		AckData_TypeDef		*ackMsg;
			
		p_arg = p_arg;
		
		memset((u8 *)&tmpSms,		0,	sizeof(tmpSms));
		memset((u8 *)&comAck,		0,	sizeof(comAck));
		memset((u8 *)&s_login,	0,	sizeof(s_login));
		memset((u8 *)&s_obd,		0,	sizeof(s_obd));
		memset((u8 *)&s_ram,		0,	sizeof(s_ram));
		memset((u8 *)&s_flash,	0,	sizeof(s_flash));
		memset((u8 *)&s_comAck,	0,	sizeof(s_comAck));
		memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));
		
		
		// ��������������Ź�  /////////////
		OSTmr_Count_Start(&s_swg.sendCounter);
		OSTmr_Count_Start(&heartBCounter);

		while(1)
		{
				// �����������ȴ�
				while(s_common.iapSta == DEF_UPDATE_KEYWORD)
				{
						OSTmr_Count_Start(&s_swg.sendCounter);	// �����������
						OSTimeDly(10000);	
				}

				// �������ݷ��� ///////////////////////////////////////////
				if(HAL_ComQue_size (DEF_SMS_QUE) > 0)
				{
						memset((u8 *)&tmpSms,		0,	sizeof(tmpSms));
						if(HAL_ComQue_out(DEF_SMS_QUE,	&tmpSms) == DEF_PROQUE_QUE_OK)
						{
								if((tmpSms.path == DEF_PATH_SMS) && (tmpSms.len != 0) && (tmpSms.len <= DEF_SMS_GSMBYTE_SIZE) &&
									 (strlen((const char *)tmpSms.smsNum) >= 5) && (tmpSms.smsNum[0] != '\0'))
								{
										// ���Ͷ���Ϣ
										// SMS�������ݴ���
    								if(tmpSms.smsAlType == DEF_SMS_ALARM)
    		                ret	= BSP_SMSSendAlarm(tmpSms.smsMode,	(u8 *)"",	tmpSms.smsNum,	tmpSms.data,	tmpSms.len,	2000); 
    		            else
    		                ret	= BSP_SMSSend(tmpSms.smsMode,	(u8 *)"",	tmpSms.smsNum,	tmpSms.data,	tmpSms.len,	2000); 
    		            if(ret == DEF_GSM_NONE_ERR)
                    {
												#if(DEF_SMSINFO_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_SMS)
												{
														BSP_RTC_Get_Current(&tmpRtc);
														myPrintf("\r\n[SMS-Txd]: \r\n");
														myPrintf("[SMS-Num]%s\r\n",	tmpSms.smsNum);
														myPrintf("[SMS-Len]%d\r\n",	tmpSms.len);
														myPrintf("[SMS-Dat]%s\r\n",	tmpSms.data);
														myPrintf("[SMS-Tim]%d-%d-%d %d:%d:%d\r\n",	tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);		
												}
												#endif
												memset((u8 *)&tmpSms,		0,	sizeof(tmpSms));	// �������ʱ�ṹ������ֹ�ⲿ�ж�����
												HAL_ComQue_outend (DEF_SMS_QUE);	// ���ݳ���	
                    }
										else										
												OSTimeDly(1000);  //��ֹ����Ƶ������ģ��	
								}	
								else
								{
										memset((u8 *)&tmpSms,		0,	sizeof(tmpSms));	// �������ʱ�ṹ������ֹ�ⲿ�ж�����
										HAL_ComQue_outend (DEF_SMS_QUE);	// �Ƿ�����ֱ�Ӷ���
								}
						}
				}
				// IP�������� /////////////////////////////////////////////
				#if(OS_NAVIREQ_EN > 0)
				if(s_osTmp.naviStep == 0)
				{
						// ϵͳ��δ��ȡӦ��IP
						if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
						{	
								if(s_login.len == 0)
								{
										// ϵͳ��δ���Ӧ��IP�������ݰ�
										memset((u8 *)&s_login,	0,	sizeof(s_login));										
										HAL_BuildNavi_Process(s_login.buf,	&s_login.len);	// ���
								}
								if(s_login.len != 0)
								{
										BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// ��������ǰ�����µ�ǰ�ź�ǿ��
										ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// ���ͨ��Ӧ������
										ackMsg = (void *)0;
										if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_login.buf,	s_login.len,	3000) == DEF_GSM_NONE_ERR)
										{
												// ��ӡGPRS���͵�����
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														sendPrintProcess (0,	s_login.buf,	s_login.len);
												#endif
												OSTmr_Count_Start(&heartBCounter);	// ֻҪ��һ�����ͳɹ��͸�λ���������Ͷ�ʱ��
												// Ӧ��IP��ȡ���ݱ�����Ӧ��
												
												// 2015-1-17 by:gaofei �޸�Ϊɨ�跽ʽ���շ�ʽ
												for(i=0;	(i < s_cfg.tcpAckTimeout * 10) && (i < 1200);	i++)
												{
														ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	
														if(ackMsg != (void *)0)  
																break;
														else
														{
																if(s_cfg.tcpAckTimeout >= 5)
																{
																		if(i == (s_cfg.tcpAckTimeout - 5) * 10)
																				OSSemPost(GSMGprsDataSem);		// �ֶ���ѯ�Ƿ��Ѿ����յ�����			
																}
														}													
														OSTimeDly(100);				// ɨ����										
												}
												if(ackMsg != (void *)0)
												{
														APP_ErrClr(&s_osTmp.ackErrTimes);		// ֻ����������Ӧ��ż���		
														if((ackMsg->msgId == DEF_CMDID_NAVI_PLA) && (ackMsg->result == DEF_ACKTYPE_OK))
														{
																s_osTmp.naviStep =1;
																RTCTmr_Count_Start(&s_osTmp.appIPWorkCounter); // ��ʼӦ��IP��ʱͳ��
																BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
																//if(BSP_TCPCloseNetPro	(DEF_GPRS_DATA_CNN0,	3000) == DEF_GSM_NONE_ERR)	// �Ͽ���ǰ��������																
																	
																memset((u8 *)&s_login,	0,	sizeof(s_login));	// �巢�ͽṹ����
																OSTimeDly(500);				// ������ʱ
														}		
														else
																OSTimeDly(500); 		// �����Ӧ������
												}
												else
												{													
														APP_ErrAdd(&s_osTmp.ackErrTimes);						//	Ӧ��ʱ�ۼƴ���
														if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
														{
																APP_ErrClr(&s_osTmp.ackErrTimes);
																BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
														}
														OSTimeDly(500); 		// ������Ӧ��ʱ
												}
										}
										else
												OSTimeDly(500); 		// ����ʧ��							
								}
						}
						else
						{
								s_login.len =0;			// 2015-7-10 by:gaofei ��ֹ�ṹδ��������򵼺�IP�з������ֱ�������
								OSTimeDly(500); 		// �豸δ����
						}
				}				
				// �������������� /////////////////////////////////////////////
				else if(s_osTmp.naviStep == 1)
				#else
				if(1)
				#endif
				{	
						#if(OS_NAVIREQ_EN > 0)						
						// Ӧ��IPʧЧ����////////////////////////////////////////
						if((RTCTmr_Count_Get(&s_osTmp.appIPWorkCounter) >= OS_APPIP_VALID_TIME) ||
							 (s_osTmp.appIPCnnErrTimes >= OS_APPIP_CNNERROR_TIMES))
						{
								if((s_common.iapSta != DEF_UPDATE_WAIT) && (s_common.iapSta != DEF_UPDATE_KEYWORD))	// �������̲���IPʧЧ����
								{
										s_osTmp.naviStep =0;	// Ӧ��IPʧЧ
										BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
										memset((u8 *)&s_login,	0,	sizeof(s_login));	// 2015-7-10 by:gaofei ��ֹ�ṹδ��������򵼺�IP�з������ֱ�������
										if(s_osTmp.appIPCnnErrTimes >= OS_APPIP_CNNERROR_TIMES)
												s_osTmp.appIPCnnErrTimes =0;		
								}
						}												
						// �������ݷ��ʹ���//////////////////////////////////////
						else
						#else
						if(1)
						#endif
						{
								if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
								{
										s_login.handShake	=0;	// �豸δ������λ״̬����
								}
								else
								{
										// �������ݷ��� /////////////////////////////////////////////
										if(s_login.handShake == 0)
										{
												// ���
												memset((u8 *)&s_login,	0,	sizeof(s_login));	
												HAL_BulidLogin_Process (s_login.buf,	&s_login.len,	(u16 *)&s_login.sn);
												if(s_login.len > DEF_LOGINSENDBUF_SIZE)
														memset((u8 *)&s_login,	0,	sizeof(s_login));	// �Ƿ�����ֱ�Ӷ���
												else
														s_login.handShake =1;	// ���뷢�ͻ���
										}
										if(s_login.handShake == 1)
										{
												if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
												{
														BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// ��������ǰ�����µ�ǰ�ź�ǿ��
														ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// ���ͨ��Ӧ������
														ackMsg = (void *)0;
														if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_login.buf,	s_login.len,	3000) == DEF_GSM_NONE_ERR)
														{
																// ��ӡGPSR���͵�����
																#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																if(dbgInfoSwt & DBG_INFO_GPRS)
																		sendPrintProcess (0,	s_login.buf,	s_login.len);
																#endif
																OSTmr_Count_Start(&heartBCounter);	// ֻҪ��һ�����ͳɹ��͸�λ��������ʱ��
																// 2015-1-17 by:gaofei �޸�Ϊɨ�跽ʽ���շ�ʽ
																for(i=0;	(i < s_cfg.tcpAckTimeout * 10) && (i < 1200);	i++)
																{
																		ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	
																		if(ackMsg != (void *)0)  
																				break;
																		else
																		{
																				if(s_cfg.tcpAckTimeout >= 5)
																				{
																						if(i == (s_cfg.tcpAckTimeout - 5) * 10)
																								OSSemPost(GSMGprsDataSem);		// �ֶ���ѯ�Ƿ��Ѿ����յ�����			
																				}												
																		}													
																		OSTimeDly(100);				// ɨ����										
																}
																if(ackMsg != (void *)0)
																{
																		APP_ErrClr(&s_osTmp.ackErrTimes);		// ��Ӧ��������	
																		if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_login.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																		{
																				s_login.len 	=0;
																				s_login.handShake =2;		// ���ֳɹ�������ʽ��������																
																				OSTimeDly(500);		
																		}		
																		else
																				OSTimeDly(500); 	
																}
																else
																{
																		APP_ErrAdd(&s_osTmp.ackErrTimes);						// Ӧ������ۼ�
																		if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																		{
																				APP_ErrClr(&s_osTmp.ackErrTimes);
																				BSP_GPRSFroceBreakSet();	// ǿ�ƶ���
																		}
																		OSTimeDly(500); 
																}												
														}
														else
																OSTimeDly(500); 
												}
												else
														OSTimeDly(500); 	
										}
								}
								// �Ѿ����ֳɹ�����Ӧ������ /////////////////////////////////////////////
								if(s_login.handShake == 2)
								{
										// ͨ��Ӧ�����ݷ��ʹ���/////////////////////////////////////////
										if((comAck.len != 0) || (s_comAck.len	!= 0))
										{
												if(comAck.len != 0)
												{
														memset((u8 *)&s_comAck,	0,	sizeof(s_comAck));										
														HAL_PackComAckProcess(comAck.data,	comAck.len,	s_comAck.buf,	&s_comAck.len);
														memset((u8 *)&comAck,	0,	sizeof(comAck));	// ���������ͨ��Ӧ����ʱ�洢��		
														if(s_comAck.len > DEF_COMSENDBUF_SIZE)
																memset((u8 *)&s_comAck,	0,	sizeof(s_comAck));	// ������ݷǷ�ֱ�Ӷ���																						
												}		
												// ���ʹ���///////////////////////////////////////////////		
												if(s_comAck.len	!= 0)
												{
														// GPRS���ݷ���
														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
																memset((u8 *)&s_comAck,	0,	sizeof(s_comAck));	// ����豸������ֱ��ɾ��ͨ��Ӧ��												
														else
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// ��������ǰ�����µ�ǰ�ź�ǿ��
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_comAck.buf,	s_comAck.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// ��ӡGPRS���͵�����
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_comAck.buf,	s_comAck.len);
																		#endif																		
																		memset((u8 *)&s_comAck,	0,	sizeof(s_comAck));	// ���������ͨ��Ӧ���ͻ�����																												
																		OSTimeDly(200);				// ������ʱ	
																}
																else
																		OSTimeDly(500); 		// ����ʧ��		
														}
												}
										}
										// RAMʵʱ���ݷ��ʹ���/////////////////////////////////////////
										else if((s_ram.len != 0) || (HAL_ComQue_size (DEF_RAM_QUE) > 0))
										{
												if(s_ram.len == 0)
												{
														if(HAL_ComQue_size (DEF_RAM_QUE) > 0)
														{
																memset((u8 *)&s_ram,	0,	sizeof(s_ram));
																HAL_PackRealTimeDataProcess	(&s_ram.type,	s_ram.buf,	&s_ram.len, (u16 *)&s_ram.sn);	
																if(s_ram.len > DEF_RAMSENDBUF_SIZE)
																		memset((u8 *)&s_ram,	0,	sizeof(s_ram));	// ������ݷǷ�ֱ�Ӷ���
																s_ram.ackType =DEF_ACKTYPE_ACK;					// ��ҪӦ��
														}
												}
												// ���ʹ���///////////////////////////////////////////////
												if(s_ram.len != 0)
												{
														// GPRS���ݷ���
														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// ��������ǰ�����µ�ǰ�ź�ǿ��
																ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// ���ͨ��Ӧ������
																ackMsg = (void *)0;
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_ram.buf,	s_ram.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// ��ӡGPRS���͵�����
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_ram.buf,	s_ram.len);
																		#endif
																		OSTmr_Count_Start(&heartBCounter);	// ֻҪ��һ�����ͳɹ��͸�λ���������Ͷ�ʱ��
																		if(s_ram.ackType == DEF_ACKTYPE_NOACK)
																		{
																				memset((u8 *)&s_ram,	0,	sizeof(s_ram));	// �巢�ͻ����ֹ�ٴη���
																				OSTimeDly(200);				// ����Ӧ��ķ�����ʱ,���ⷢ�͹���
																		}
																		else
																		{
																				// 2015-1-17 by:gaofei �޸�Ϊɨ�跽ʽ���շ�ʽ
																				for(i=0;	(i < s_cfg.tcpAckTimeout * 10) && (i < 1200);	i++)
																				{
																						ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	
																						if(ackMsg != (void *)0)  
																								break;
																						else
																						{
																								if(s_cfg.tcpAckTimeout >= 5)
																								{
																										if(i == (s_cfg.tcpAckTimeout - 5) * 10)
																												OSSemPost(GSMGprsDataSem);		// �ֶ���ѯ�Ƿ��Ѿ����յ�����			
																								}												
																						}													
																						OSTimeDly(100);				// ɨ����										
																				}
																				if(ackMsg != (void *)0) 
																				{																
																						APP_ErrClr(&s_osTmp.ackErrTimes);			// ֻ����������Ӧ��ż���		
																						if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_ram.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																						{
																								memset((u8 *)&s_ram,	0,	sizeof(s_ram));	// �巢�ͻ���														
																								OSTimeDly(300);				// ������ʱ
																						}		
																						else
																								OSTimeDly(500); 		// �����Ӧ������
																				}
																				else
																				{
																						APP_ErrAdd(&s_osTmp.ackErrTimes);						//	Ӧ��ʱ�ۼƴ���													
																						if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																						{
																								APP_ErrClr(&s_osTmp.ackErrTimes);
																								BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
																						}
																						OSTimeDly(500); 		// ������Ӧ��ʱ
																				}
																		}	
																}
																else
																		OSTimeDly(500); 		// ����ʧ��		
														}
														else
																OSTimeDly(500); 		// �豸δ����
												}	
										}
										// FLASH����������ݷ��ʹ���/////////////////////////////////////////
										else if((s_flash.len != 0) || (HAL_ComQue_size (DEF_FLASH_QUE) > 0))
										{
												if(s_flash.len == 0)
												{
														if(HAL_ComQue_size (DEF_FLASH_QUE) > 0)
														{
																if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
																{
																		// ֻ���豸������ȡ���ݷ�ֹFLASH����ȡ�����豸��λ���쳣�µ絼�����ݶ�ʧ!!
																		memset((u8 *)&s_flash,	0,	sizeof(s_flash));																		
																		if(HAL_PackFlashDataProcess(DEF_FLASHSENDBUF_SIZE,	s_flash.buf,	&s_flash.len, (u16 *)&s_flash.sn,	&s_flash.queLastOut,	&s_flash.queOutNum) != 0)
																				s_flash.queOutNum =0;	// ���ݱ�����ֹ���ݶ�ʧ
																		else
																		{
																				if(s_flash.len > DEF_FLASHSENDBUF_SIZE)
																						memset((u8 *)&s_flash,	0,	sizeof(s_flash));	// ������ݷǷ�ֱ�Ӷ���
																		}
																		s_flash.ackType =DEF_ACKTYPE_ACK;			// FLASH������ݶ���ҪӦ��
																}
														}				
												}
												// ���ʹ���///////////////////////////////////////////////
												if(s_flash.len != 0)
												{
														// GPRS���ݷ���
														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// ��������ǰ�����µ�ǰ�ź�ǿ��
																ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// ���ͨ��Ӧ������
																ackMsg = (void *)0;
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_flash.buf,	s_flash.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// ��ӡGPRS���͵�����
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_flash.buf,	s_flash.len);
																		#endif
																		OSTmr_Count_Start(&heartBCounter);	// ֻҪ��һ�����ͳɹ��͸�λ���������Ͷ�ʱ��
																		if(s_flash.ackType == DEF_ACKTYPE_NOACK)
																		{
																				HAL_ComQue_outFinish (DEF_FLASH_QUE,	s_flash.queLastOut,	s_flash.queOutNum);	// ��������״̬
																				memset((u8 *)&s_flash,	0,	sizeof(s_flash));	// �巢�ͻ����ֹ�ٴη���											
																				OSTimeDly(200);				// ����Ӧ��ķ�����ʱ,���ⷢ�͹���
																		}
																		else
																		{
																				// 2015-1-17 by:gaofei �޸�Ϊɨ�跽ʽ���շ�ʽ
																				for(i=0;	(i < s_cfg.tcpAckTimeout * 10) && (i < 1200);	i++)
																				{
																						ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	
																						if(ackMsg != (void *)0)  
																								break;
																						else
																						{
																								if(s_cfg.tcpAckTimeout >= 5)
																								{
																										if(i == (s_cfg.tcpAckTimeout - 5) * 10)
																												OSSemPost(GSMGprsDataSem);		// �ֶ���ѯ�Ƿ��Ѿ����յ�����			
																								}													
																						}													
																						OSTimeDly(100);				// ɨ����										
																				}
																				if(ackMsg != (void *)0) 
																				{																
																						APP_ErrClr(&s_osTmp.ackErrTimes);			// ֻ����������Ӧ��ż���		
																						if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_flash.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																						{
																								HAL_ComQue_outFinish (DEF_FLASH_QUE,	s_flash.queLastOut,	s_flash.queOutNum);	// ��������״̬
																								memset((u8 *)&s_flash,	0,	sizeof(s_flash));	// �巢�ͻ���										
																								OSTimeDly(300);				// ������ʱ
																						}		
																						else
																								OSTimeDly(500); 		// �����Ӧ������
																				}
																				else
																				{
																						APP_ErrAdd(&s_osTmp.ackErrTimes);						//	Ӧ��ʱ�ۼƴ���
																						if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																						{
																								APP_ErrClr(&s_osTmp.ackErrTimes);
																								BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
																						}
																						OSTimeDly(500); 		// ������Ӧ��ʱ
																				}
																		}
																}
																else
																		OSTimeDly(500); 		// ����ʧ��		
														}
														else
																OSTimeDly(500); 		// �豸δ����
												}	
										}
										// OBDʵʱ���ݷ��ʹ���/////////////////////////////////////////
										else if(s_obd.obdSendFlag == 1)
										{
												if(s_obd.len > DEF_OBDSENDBUF_SIZE)
														memset((u8 *)&s_obd,	0,	sizeof(s_obd));	// ������ݷǷ�ֱ�Ӷ���
												else
												{
														// ���ʹ���///////////////////////////////////////////////
														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// ��������ǰ�����µ�ǰ�ź�ǿ��
																ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// ���ͨ��Ӧ������
																ackMsg = (void *)0;
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_obd.buf,	s_obd.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// �жϵ�ǰ������������
																		if((s_obd.buf[6] == 0x02) && (s_obd.buf[7] == 0x94))
																				s_osTmp.tjSendFlag =0x55;	// 2015-7-23 Ϊ֧��VIN�����޸ģ�����������ݷ��ͱ�־
																		else
																				s_osTmp.tjSendFlag =0;
																
																		// ��ӡGPRS���͵�����
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_obd.buf,	s_obd.len);
																		#endif
																		OSTmr_Count_Start(&heartBCounter);	// ֻҪ��һ�����ͳɹ��͸�λ���������Ͷ�ʱ��
																		if(s_obd.ackType == DEF_ACKTYPE_NOACK)
																		{
																				memset((u8 *)&s_obd,	0,	sizeof(s_obd));	// �巢�ͻ����ֹ�ٴη���
																				OSTimeDly(200);				// ����Ӧ��ķ�����ʱ,���ⷢ�͹���
																		}
																		else
																		{
																				// 2015-1-17 by:gaofei �޸�Ϊɨ�跽ʽ���շ�ʽ
																				for(i=0;	(i < s_cfg.tcpAckTimeout * 10) && (i < 1200);	i++)
																				{
																						ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	
																						if(ackMsg != (void *)0)  
																								break;
																						else
																						{
																								if(s_cfg.tcpAckTimeout >= 5)
																								{
																										if(i == (s_cfg.tcpAckTimeout - 5) * 10)
																												OSSemPost(GSMGprsDataSem);		// �ֶ���ѯ�Ƿ��Ѿ����յ�����			
																								}													
																						}													
																						OSTimeDly(100);				// ɨ����										
																				}	
																				if(ackMsg != (void *)0) 
																				{																
																						APP_ErrClr(&s_osTmp.ackErrTimes);			// ֻ����������Ӧ��ż���		
																						if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_obd.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																						{																						
																								// �ж��Ƿ���Ҫ���ʹ��VIN
																								if(s_osTmp.tjSendFlag == 0x55)
																								{
																										s_osTmp.tjSendFlag =0;
																										HAL_SendOBDProProcess(CMD_READ_VIN,	0,	0,	(u8 *)"",	0);	// 2015-7-23 ֧��VIN���������޸�
																								}
																								memset((u8 *)&s_obd,	0,	sizeof(s_obd));	// �巢�ͻ���			
																								OSTimeDly(300);				// ������ʱ
																						}		
																						else
																								OSTimeDly(500); 		// �����Ӧ������
																				}
																				else
																				{
																						APP_ErrAdd(&s_osTmp.ackErrTimes);						//	Ӧ��ʱ�ۼƴ���
																						if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																						{
																								APP_ErrClr(&s_osTmp.ackErrTimes);
																								BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
																						}
																						OSTimeDly(500); 		// ������Ӧ��ʱ
																				}
																		}	
																}
																else
																		OSTimeDly(500); 		// ����ʧ��		
														}
														else
																OSTimeDly(500); 		// �豸δ����
												}
										}																							
										
										// ����(˯��)���ݷ��ʹ���/////////////////////////////////////////
										else if((s_comPack.packType == PACK_TYPE_SHUTNET) || (s_comPack.packType == PACK_TYPE_SLEEP))
										{														
												// GPRS���ݷ���
												if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
												{
														memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// ����豸������ֱ�ӷ�������
														OSTimeDly(500); 		// �豸δ����
												}
												else
												{
														// ����������ݰ�
														if(s_comPack.packType == PACK_TYPE_SHUTNET)
																HAL_BulidComPack_Process (DEF_CMDID_NETBRAKREQ_DEV,	s_comPack.buf,	&s_comPack.len, (u16 *)&s_comPack.sn);
														
														// ���˯�����ݰ�
														else if(s_comPack.packType == PACK_TYPE_SLEEP)
																HAL_BulidComPack_Process (DEF_CMDID_SLEEPREQ_DEV,	s_comPack.buf,	&s_comPack.len, (u16 *)&s_comPack.sn);
													
														if((s_comPack.len == 0) || (s_comPack.len > DEF_COMSENDBUF_SIZE))
																memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// �Ƿ�����ֱ�Ӷ���
														else
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// ��������ǰ�����µ�ǰ�ź�ǿ��
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_comPack.buf,	s_comPack.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// ��ӡGPRS���͵�����
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_comPack.buf,	s_comPack.len);
																		#endif
																		OSTmr_Count_Start(&heartBCounter);	// ֻҪ��һ�����ͳɹ��͸�λ���������Ͷ�ʱ��																			
																}
																memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// ���ܷ��ͳɹ����ֻ����һ��
																OSTimeDly(300);		
														}
												}	
										}
										else
										{												
												// ���������ݷ��ʹ���/////////////////////////////////////////
												#if(OS_HEARTBEAT_EN > 0)										
												if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
														OSTmr_Count_Start(&heartBCounter);	// ���������λ���������Ͷ�ʱ��			
												else
												{
														if(s_common.iapSta == DEF_UPDATE_KEYWORD)
																OSTmr_Count_Start(&heartBCounter);	// ���������в���������
														if(OSTmr_Count_Get(&heartBCounter) >= (s_cfg.heartbeatTime * 1000))
														{												
																memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));
																s_comPack.packType = PACK_TYPE_HEART;	// ����������������
																HAL_BulidComPack_Process (DEF_CMDID_HEARD_DEV,	s_comPack.buf,	&s_comPack.len, (u16 *)&s_comPack.sn);
																if((s_comPack.len == 0) || (s_comPack.len > DEF_COMSENDBUF_SIZE))
																		memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// �Ƿ�����ֱ�Ӷ���
																else
																{
																		BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// ��������ǰ�����µ�ǰ�ź�ǿ��
																		ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// ���ͨ��Ӧ������
																		ackMsg = (void *)0;
																		if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_comPack.buf,	s_comPack.len,	3000) == DEF_GSM_NONE_ERR)
																		{
																				// ��ӡGPRS���͵�����
																				#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																				if(dbgInfoSwt & DBG_INFO_GPRS)
																						sendPrintProcess (0,	s_comPack.buf,	s_comPack.len);
																				#endif
																				OSTmr_Count_Start(&heartBCounter);	// ��λ���������Ͷ�ʱ��
																				// 2015-1-17 by:gaofei �޸�Ϊɨ�跽ʽ���շ�ʽ
																				for(i=0;	(i < s_cfg.tcpAckTimeout * 10) && (i < 1200);	i++)
																				{
																						ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	
																						if(ackMsg != (void *)0)  
																								break;
																						else
																						{
																								if(s_cfg.tcpAckTimeout >= 5)
																								{
																										if(i == (s_cfg.tcpAckTimeout - 5) * 10)
																												OSSemPost(GSMGprsDataSem);		// �ֶ���ѯ�Ƿ��Ѿ����յ�����			
																								}											
																						}													
																						OSTimeDly(100);				// ɨ����										
																				}
																				if(ackMsg != (void *)0)
																				{																				
																						APP_ErrClr(&s_osTmp.ackErrTimes);			// ֻ����������Ӧ��ż���		
																						if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_comPack.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																						{
																								memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// �巢�ͻ���	
																								OSTimeDly(300);				// ������ʱ
																						}		
																						else
																								OSTimeDly(500); 		// �����Ӧ������
																				}
																				else
																				{
																						APP_ErrAdd(&s_osTmp.ackErrTimes);						//	Ӧ��ʱ�ۼƴ���
																						if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																						{
																								APP_ErrClr(&s_osTmp.ackErrTimes);
																								BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
																						}
																						OSTimeDly(500); 		// ������Ӧ��ʱ
																				}
																		}
																		else
																				OSTimeDly(500); 		// ����ʧ��		
																}
														}
												}						
												#endif													
										}
								}
								else
										s_login.handShake =0;	// �Ƿ���ֵ��λ��δ��½ǰ״̬
						}
				}
				else
				{
						s_osTmp.naviStep =0;	// �����Ƿ��������»�ȡIP��ַ
				}				

				// ����������� ////////////////////			
				OSTmr_Count_Start(&s_swg.sendCounter);	// �����������
				OSTimeDly(50);				
		}
}
/*
*********************************************************************************************************
*                                            App_TaskPROTOCOL()
*
* Description : ����Э������
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void		App_TaskPROTOCOL	(void 	*p_arg)
{		
		GPRS_Typedef	*gMsg;	
		SMS_Typedef		*sMsg;
		BT_Typedef		*bMsg;
		CONAckMbox_TypeDef	*conAckMsg;
		s8	ret=0;
		vu8	 ledJump=0,findCarFlag=0;
		u16  upDateLedCounter=0;
		vu32 findCarCounter=0;
	
		p_arg = p_arg;
	
		// ���ſ��ƽṹ���
		memset((u8 *)&conSms,	0,	sizeof(conSms));

		// ��������������Ź�  /////////////
		OSTmr_Count_Start(&s_swg.protocolCounter);

		while (1) 
		{		
				// Э�鴦��(GPRS����)/////////////////////////////////////////////////////////////	
				gMsg = (GPRS_Typedef *)OSMboxAccept(OS_RecGprsMbox);
				if(gMsg != (void *)0)
				{
						HAL_ProUnpackMain	(gMsg->dataBuf,	gMsg->dataLen);
						s_osTmp.rxdDataLock =0;	// ���ݽ���
				}
				
				// Э�鴦��(BT����)/////////////////////////////////////////////////////////////	
				bMsg = (BT_Typedef *)OSMboxAccept(OS_RecBtMbox);
				if(bMsg != (void *)0)
				{
//						// TEST ���յ������ݷ��ص����Ͷ�
//						if(BSP_BTSendDataPro	(bMsg->dataBuf,	bMsg->dataLen,	3000) == DEF_GSM_NONE_ERR)
//						{
//								// ��ӡBT���͵�����
//								#if(DEF_BTINFO_OUTPUTEN > 0)							
//								if(dbgInfoSwt & DBG_INFO_BT)
//								{
//										memset(tmpBuf,	'\0',	sizeof(tmpBuf));
//										tmpLen = Hex2StringArray ((u8 *)bMsg->dataBuf,	bMsg->dataLen,	tmpBuf);
//										if(tmpLen != 0xffff)
//										{
//												myPrintf("\r\n[BT-Txd]: ");
//												BSP_OSPutChar(DEF_DBG_UART, tmpBuf,	tmpLen);
//												myPrintf("\r\n");
//										}
//								}
//								#endif					
//						}
					
						//HAL_ProUnpackMain	(bMsg->dataBuf,	bMsg->dataLen);
				}

				// Э�鴦��(SMS����)
				sMsg = (SMS_Typedef *)OSMboxAccept(OS_RecSmsMbox);
				if(sMsg != (void *)0)
				{						
						// ����Э�鴦��			
						if(conSms.dataLock == 0)
						{
								memset((u8 *)&conSms,	0,	sizeof(conSms));
								conSms.dataLock 	=1;	// ����������
								if(smsConForamtCheck(sMsg, &conSms) == DEF_SMSCONTROL_NONE)
								{
										ret =smsCarControlProcess(sMsg,	&conSms);
										if(ret == DEF_SMSCONTROL_NONE)
												conSms.dataLock =1;	// ������״̬ά�ֲ���(�ȴ�OBD��������Ʒ���)
										else
										{
												// ������
												if(ret == DEF_SMSCONTROL_BUSY)
														conSms.resultCode =SMS_ACK_BUSY;														
												else if(ret == DEF_SMSCONTROL_NUM)
														conSms.resultCode =SMS_ACK_ILLNUM;
												else if(ret == DEF_SMSCONTROL_TIMEOUT)
														conSms.resultCode =SMS_ACK_TIMEOUT;
												else if(ret == DEF_SMSCONTROL_TIMESTAMP)
														conSms.resultCode =SMS_ACK_TIMESTAMP;
												else if(ret == DEF_SMSCONTROL_NOSUPORT)
														conSms.resultCode =SMS_ACK_NOSUPORT;
												
												HAL_SMSConRetInQueueProcess	(&conSms);												
												conSms.dataLock =0;	// ȡ���������ͷ���Դ
										}										
								}
								else
										conSms.dataLock =0;	// ��ʽ�Ƿ����账��	
						}
						else
						{
								// ����һ�����ſ���Ϊ����ʱ�յ����µ�ָ��
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[EVENT]: SMS Control struct data-lock =1, Device will ignore this SMS!\r\n");
								#endif
						}					
				}
				// ���ſ���Ӧ�����
				conAckMsg = (CONAckMbox_TypeDef *)OSMboxAccept(OS_CONAckMbox);
				if(conAckMsg != (void *)0)
				{
						if(conSms.dataLock == 1)	// ֻ����֮���Ż����������Ķ����Ÿ������ظ�
						{
								conSms.resultCode =conAckMsg->retCode; 						
								HAL_SMSConRetInQueueProcess	(&conSms);	// �������Ʒ���
								conSms.dataLock =0;	// ȡ��������,׼��������һ�����ſ���ָ��	
						}
						else
						{
								;	// �ڲ����Է���������
						}
				}
				
				// ����������ָʾ�ƿ���/////////////////////////////////////////////////////
				if(s_common.iapSta == DEF_UPDATE_KEYWORD)
				{					
						upDateLedCounter++;
						if(upDateLedCounter >= 10)	// 100ms
						{
								upDateLedCounter =0;
								if(ledJump == 0)
								{
										IO_LED_DATA_ON();
										ledJump =1;
								}
								else
								{
										IO_LED_DATA_OFF();
										ledJump =0;
								}
						}				
				}
						 
			 	OSTmr_Count_Start(&s_swg.protocolCounter);				// ��������Ź�������
			 	OSTimeDly(10);																// ����CPUʹ��ʱ���Ա����������ȼ�����������
		}			
}
/*
*********************************************************************************************************
*                                            App_TaskBASE()
*
* Description : 
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void		App_TaskBASE	(void 	*p_arg)
{		
		u8	ret =0;
		vu8	igEnFlag=0, healthFlag=0;
		vu16	i=0;	
		vu32	healthGpsCounter=0;
		
		p_arg = p_arg;
	
		glob_accOnRalStep =0;	// ������׶α���

		// ����BASE����������Ź�  /////////////
		OSTmr_Count_Start(&s_swg.baseCounter);
	
		// ��ʼ���Ϩ���ʱ��
		OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);
		OSTmr_Count_Start(&s_osTmp.igOnChiXuCounter);				// ������ʱ���ʱ��ʼ��
		OSTmr_Count_Start(&s_osTmp.sendDataToCounter);			// ��ʼ���ͳ�ʱͳ��
		OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);		// ��ʼ��ȡ����״̬��ʱͳ��
		OSTmr_Count_Start(&s_osTmp.igEnDelayCounter);
		OSTmr_Count_Start(&s_osTmp.upWaitCounter);
		RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);
		RTCTmr_Count_Start(&s_osTmp.report24Counter);
    
    if (BKP_SAVE_FLAG_PWRON == BKP_ReadBackupRegister(BKP_PWRMOVE_FLAG))
    {
        HAL_SYSALM_SetState(BIT_ALARM_POWREMOVE, DEF_DISABLE);
    }
    else
    {
        HAL_SYSALM_SetState(BIT_ALARM_POWREMOVE, DEF_ENABLE);
    }
    
    HAL_SetLastPowerRemoveSt(HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE));
	
		while(1)
		{
				// �豸��������(���������и�����ʱռ��CPU)/////////////////////////////////////////			
				if((s_common.iapSta == DEF_UPDATE_WAIT) || (s_common.iapSta == DEF_UPDATE_KEYWORD))
				{
						// ����ǰ�ȴ���ǰ��������ȫ���������
						if(s_osTmp.upFlag == 1)
						{
								s_osTmp.upFlag =0;
								OSTimeDly(500);	// �����������̵ȴ�500msͬ��0124ͨ��Ӧ����
						}
						if(OSTmr_Count_Get(&s_osTmp.upWaitCounter) < 40000)
						{
								if((HAL_ComQue_size (DEF_RAM_QUE) == 0) && (s_ram.len == 0) 	&&	
									 (s_flash.len == 0) && (comAck.len == 0) && (s_comAck.len	== 0))
										s_common.iapSta  	=DEF_UPDATE_KEYWORD;	// ��������ȫ�ֱ�־������������
						}
						else
								s_common.iapSta  	=DEF_UPDATE_KEYWORD;	// ��������ȫ�ֱ�־������������
						
						// ��������
						if(s_common.iapSta == DEF_UPDATE_KEYWORD)
						{
					
								// �ر�ϵͳ���״ָ̬ʾ��״ָ̬ʾ��
								IO_LED_RUN_OFF();
								HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_DISABLE);									
								
								// ɾ������Ҫ������
								if(taskDelProcess(10000) != 0)
								{
										// �����־��λ�����ִ��
										HAL_CPU_ResetPro();	// ���δ�ܳɹ�ɾ��������ϵͳ��λ	
								}
								
								// ���֮ǰ������GSM������Ϣ��ֹ���������г���ֱ�ӵ���ģ�鸴λ
								BSP_GSM_ERR_Clr(&err_Gsm.gsmErrorTimes);
								APP_ErrClr(&s_osTmp.cnnErrTimes);
								APP_ErrClr(&s_osTmp.ackErrTimes);		// ��Ӧ��������
							
								// ��ʼ������������
								ret =HAL_DownLoadFileProcess(10);		
								HAL_CPU_ResetPro();			// ���سɹ����Լ��κ󶼲������سɹ���ϵͳ��λ
						}
				}				
				
				// IG������ʱ����//////////////////////////////////////////////////////////////////////
				if(igEnFlag == 0)
				{
						if(OSTmr_Count_Get(&s_osTmp.igEnDelayCounter) >= 2000)
						{
								igEnFlag =1;
								BSP_IG_IT_CON(ENABLE);	// ʹ��IG(�����廽��)�ж�
						}					
				}
		
				// �������////////////////////////////////////////////////////////////////////////////
				HAL_EventAlarmProcess(s_cfg.eventSwt);

				// �����������//////////////////////////////////////
				if(HAL_SYS_GetState (BIT_SYSTEM_IG) == 1)
				{
						if(s_cfg.gpsCollectTime != 0)
						{
								if((s_cfg.devEn & DEF_DEVEN_BIT_GPS) && (GPS_DEVICE.InitSta == 1))
								{							
										if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0)
												OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);		// GPS����λ��λ�����ʱ��
										
										if(OSTmr_Count_Get(&s_osTmp.igOnMsgCounter) >= (s_cfg.gpsCollectTime * 1000))	 	// ���״̬���������ϴ�����
										{											 								 				 			 
												HAL_GPSInQueueProcess();
												//memset((u8 *)&s_gpsGL,	'\0',	sizeof(GPSProData_Def));	// �����ɺ���GPS��Ϣ
												OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);											
										}
								}
						}
				}
				// ����Ϩ������//////////////////////////////////////
				else
				{
						// ���Ϩ���ƶ��������ϱ�GPSλ����Ϣ
						if( HAL_SYSALM_GetState(BIT_ALARM_ILLMOVE) == 1 )
						{
								if(s_cfg.gpsCollectTime != 0)
								{
										if((s_cfg.devEn & DEF_DEVEN_BIT_GPS) && (GPS_DEVICE.InitSta == 1))
										{							
												if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0)
														OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);		// GPS����λ��λ�����ʱ��
												
												if(OSTmr_Count_Get(&s_osTmp.igOnMsgCounter) >= (s_cfg.gpsCollectTime * 1000))	 	// ���״̬���������ϴ�����
												{											 								 				 			 
														HAL_GPSInQueueProcess();
														//memset((u8 *)&s_gpsGL,	'\0',	sizeof(GPSProData_Def));	// �����ɺ���GPS��Ϣ
														OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);											
												}
										}
								}						
						}
						// Ϩ����ʱ���ϱ�GPS��Ϣ
//						if(s_cfg.offReportTime != 0)
//						{
//								if((s_cfg.devEn & DEF_DEVEN_BIT_GPS) && (GPS_DEVICE.InitSta == 1))
//								{
//										if(RTCTmr_Count_Get(&s_osTmp.igOffMsgCounter) >= s_cfg.offReportTime)
//										{
//												//	�ȴ�GPS��λ
//												for(i=0;	((i<OS_GPSREFIX_WAIT_TIMEOUT) && (HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0));	i++)
//												{
//														OSTimeDly(100);
//												}		 				
//												HAL_GPSInQueueProcess();																				
//												RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);
//										}
//								}
//						}
				}
				// 24Сʱ��������////////////////////////////////////
				if(RTCTmr_Count_Get(&s_osTmp.report24Counter) >= s_cfg.healthReportTime)
				{
						HAL_24RptInQueueProcess();
						RTCTmr_Count_Start(&s_osTmp.report24Counter);
						
						if(s_cfg.healthReportTime <= 14400)	// 4Сʱ�����ٴδ��GPS���ݰ������ⶪʧ����
						{
								healthFlag =1;					
								OSTmr_Count_Start(&healthGpsCounter);
						}
				}	
				
				// 2015-6-30�����Ϊ��ʹ�豸GPS��ʧ����
				if(healthFlag == 1)
				{
						if(OSTmr_Count_Get(&healthGpsCounter) <= 60000)
						{
								if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
								{
										OSTimeDly(4000);	// 	2015-8-5����GPS,3����˸�ֵʱ��
										HAL_GPSInQueueProcess();
										//memset((u8 *)&s_gpsGL,	'\0',	sizeof(GPSProData_Def));	// �����ɺ���GPS��Ϣ
										healthFlag =0;
								}	
						}
						else
								healthFlag =0;	// ��ʱ���˳�GPS�ϱ�
				}

				// ����������� ////////////////////			
				OSTmr_Count_Start(&s_swg.baseCounter);
				OSTimeDly(50);	
		}
}
/*
*********************************************************************************************************
*                                            App_TaskBT()
*
* Description : ��������ҵ��
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void		App_TaskBT	(void 	*p_arg)
{		
		BTMbox_Typedef *btMsg;
		//vu8	btCfgFlag=0;
		s8	retCode=0;
		
		p_arg = p_arg;

		// ��������������Ź�  /////////////
		OSTmr_Count_Start(&s_swg.btCounter);

		while (1) 
		{	
				// �����������ȴ�
				while(s_common.iapSta == DEF_UPDATE_KEYWORD)
				{
						OSTmr_Count_Start(&s_swg.btCounter);	// �����������
						OSTimeDly(10000);	
				}

//				// ��ʼ������ģ��(��������������) 
//				if(btCfgFlag == 0)
//				{
//						retCode	=DEF_GSM_NONE_ERR;
//						retCode	=BSP_BTInitPro((u8 *)DEF_BT_NAME, 1000);
//						if(retCode == DEF_GSM_NONE_ERR)
//						{
//								btCfgFlag = 1;		// ����ֻ����һ��
//								#if(DEF_BTINFO_OUTPUTEN > 0)							
//								if(dbgInfoSwt & DBG_INFO_BT)
//										myPrintf("[BT]: Init OK and visible!\r\n");
//								#endif
//						}
//						else
//						{
//								#if(DEF_BTINFO_OUTPUTEN > 0)							
//								if(dbgInfoSwt & DBG_INFO_BT)
//								{
//										if((retCode == DEF_GSM_TIMEOUT_ERR) || (retCode == DEF_GSM_BUSY_ERR))
//												myPrintf("[BT]: Init time out!\r\n");
//										else
//												myPrintf("[BT]: Init error!\r\n");
//								}
//								#endif					
//						}
//				}
				// ����/ƥ�䴦��
				btMsg = (BTMbox_Typedef *)(OSMboxAccept(GSMBtAppMbox));
				if(btMsg != (void *)0)
				{
						retCode	=DEF_GSM_NONE_ERR;
						if(btMsg->func == BT_PAIR_DIGI)				// ����ƥ������
								retCode =BSP_BTPairAckPro(0,	(u8 *)"", 3000);
						else if(btMsg->func == BT_PAIR_PASS)	// ����ƥ������
								retCode =BSP_BTPairAckPro(1,	(u8 *)DEF_BT_PASS, 3000);
						else if(btMsg->func == BT_CONNECT)		// ������������
								retCode =BSP_BTConnectAckPro(DEF_ENABLE,	3000);					
						
						#if(DEF_BTINFO_OUTPUTEN > 0)							
						if(dbgInfoSwt & DBG_INFO_BT)
						{
								if((btMsg->func == BT_PAIR_DIGI) || (btMsg->func == BT_PAIR_PASS))
								{
										if(retCode == DEF_GSM_NONE_ERR)
												myPrintf("[BT]: Pair success,	type:%d!\r\n",	btMsg->func);
										else
												myPrintf("[BT]: Pair fail, type:%d!\r\n",	btMsg->func);
								}
								else if(btMsg->func == BT_CONNECT)
								{
										if(retCode == DEF_GSM_NONE_ERR)
												myPrintf("[BT]: Connect success!\r\n");
										else
												myPrintf("[BT]: Connect fail!\r\n");							
								}							
						}
						#endif
				}
						 
			 	OSTmr_Count_Start(&s_swg.btCounter);				// ��������Ź�������
			 	OSTimeDly(50);																// ����CPUʹ��ʱ���Ա����������ȼ�����������
		}			
}
/*
*********************************************************************************************************
*                                            App_TaskGPS()
*
* Description : 
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void		App_TaskGPS	(void 	*p_arg)
{
		vu8	  	paNum=0,i=0,trsFinishF=0,lvboStep=0,ljPoint=0,dssAtimes=0,dssPackNum=0,dssStep=0,dssCode=0,gpsCheckEn=0;
		vu16		pPosi=0;
		vu32		gpsResetCounter=0,rthPackCounter=0;

		p_arg = p_arg;
		
		// GPSģ���ʼ��������/////////////
		BSP_GPS_Init();	
	
		// GPSӦ�ñ�����ʼ��
		HAL_GpsAppInit();

		// ��������������Ź�  /////////////
		OSTmr_Count_Start(&s_swg.gpsCounter);	
    OSTmr_Count_Start(&gpsResetCounter);	// GPS����λ��ʱ��
		OSTmr_Count_Start(&rthPackCounter);		// GPS��ʻ��Ϊ��������ʱ��
	
		BSP_GPSRES_IT_CON(1);																						/* Enable GPS RXD	interrupt*/
    while (1) 
		{   
				// �����������ȴ�
				while(s_common.iapSta == DEF_UPDATE_KEYWORD)
				{
						OSTmr_Count_Start(&s_swg.gpsCounter);	// �����������
						OSTimeDly(10000);	
				}
				
				// GPS���ݴ���/////////////////////////////////////////////////////////
				if(OSSemAccept(GPSComSem) > 0) 															/* Wait for a packet to be received */	
				{											
						// ��ӡ��GPS��ϸ��Ϣ						
						#if(DEF_GPSINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GPS)
								BSP_OSPutChar(DEF_DBG_UART,	GPS_PORT.rxdBuf,	GPS_PORT.rxLen);
						#endif
						
						trsFinishF	=0;		// �������ɱ�־,��ʼ��������
						paNum = BSP_GPS_CheckpackNum(GPS_PORT.rxdBuf,	GPS_PORT.rxLen);	// ������$GPRMC��ʽ���ݰ�����	
						if(paNum != 0)
						{								
								for(i=1;	i<=paNum;	 i++)				
								{
										pPosi =	BSP_GPS_ReturnPosition(i,	GPS_PORT.rxdBuf,	GPS_PORT.rxLen);
										if(pPosi != 0xffff)
										{
												if(BSP_GPS_ParseRxPkt((u8 *)&GPS_PORT.rxdBuf[pPosi]) == GPS_PARSE_OK) 
												{														 
														if((GPS_PORT.resSta & Rec_PACK_DEF) == Rec_PACK_DEF)
																break;	// ��������˳�
												}
												else
												{
														#if(DEF_GPSINFO_OUTPUTEN > 0)
														if(dbgInfoSwt & DBG_INFO_GPS)
																myPrintf("\r\n[GPS]: Data pack parse fail[%d]!\r\n",	i);	// ���ݰ�����ʧ��
														#endif
												}
										}
										else
										{		
												#if(DEF_GPSINFO_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_GPS)
														myPrintf("\r\n[GPS]: Data pack idex fail!\r\n");	// ���ݼ���ʧ��
												#endif									
										}	
								}
								if((GPS_PORT.resSta & Rec_PACK_DEF) != Rec_PACK_DEF)
								{
										#if(DEF_GPSINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GPS)
												myPrintf("\r\n[GPS]: Data not enough!\r\n");	// δ�ﵽ���ڽ������������ݰ�
										#endif	
								}
								else
								{
										GPS_DEVICE.HardWareSta 	= DEF_GPSHARD_NONE_ERR;   // ����״̬
										OSTmr_Count_Start(&gpsResetCounter);							// ��GPS��λ��ʱ��
										/////////////////////////////
										if(GPS_GPRMC.Sta[0] == 'A')
										{
												// �����ϴ�GPS����״̬���ڳ���,DSS����							
												s_TmpGps.upSpeed 		= s_TmpGps.cunSpeed;
												s_TmpGps.upAngle 		= s_TmpGps.cunAngle;
										}
										////////////////////////////
										HAL_ConvertGpsData();		// ��GPSԭʼ����ת��Э��Ҫ������
										HAL_SaveDssProData(&s_gps);	// ��GPS����洢��DSS����
									  trsFinishF =1; 					// �������������־
										
										// �����ɹ�����ӡ�ַ�������Ƶ���ϴ�
										//#if(DEF_GPSINFO_OUTPUTEN > 0)
										//if(dbgInfoSwt & DBG_INFO_GPS)
										//		myPrintf("\r\n[GPS]: Data parse success!\r\n");	// ���ݽ����ɹ�
										//#endif	
								}
						}
						else
						{
								#if(DEF_GPSINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GPS)
										myPrintf("\r\n[GPS]: Illegal GPS data!\r\n");	// �Ƿ�GPS����
								#endif	
						}
						BSP_GPS_RXBUF_UNLOCK(); 	// ���¿�ʼ�������� 						
								
						// GPS����Ӧ�ô��� /////////////////////////////////////////////////////////////
						// 2015-9-21 �޸�δ�յ�GPS���ݰ�ʱҲ����Ľ���Ӧ�ô����bug
						if((trsFinishF == 1) && (*(s_devSta.pGps)	==	DEF_GPSHARD_NONE_ERR))	
						{
								// ����������
								// ���ٱ���
								// ���򱨾�							
								// DSS���ݴ���
								if(s_gps.sta == 'A')
								{
										if(dssAtimes < 0xff)
												dssAtimes++;
								}
								else
										dssAtimes=0;	
								/////////////////////////////////
								if(dssStep == 0)
								{	
										if(dssAtimes >= 3)
										{
												dssCode =HAL_GpsDssProcess	(s_TmpGps.upAngle,	s_TmpGps.upSpeed, s_TmpGps.cunAngle,	s_TmpGps.cunSpeed);	
												if(dssCode != 0)
												{
														dssStep 		=1;	// DSSִ�н׶α���
														dssPackNum	=0;	// ������������ݰ�
														if(dssCode == DEF_GPSDSS_ATH)
														{
																#if(DEF_EVENTINFO_OUTPUTEN > 0)
																if(dbgInfoSwt & DBG_INFO_EVENT)
																		myPrintf("[EVENT]: DSS Sudden acceleration!\r\n");
																#endif
														}
														else if(dssCode == DEF_GPSDSS_DTH)
														{
																#if(DEF_EVENTINFO_OUTPUTEN > 0)
																if(dbgInfoSwt & DBG_INFO_EVENT)
																		myPrintf("[EVENT]: DSS Sudden braking!\r\n");
																#endif
														}
														else if(dssCode == DEF_GPSDSS_RTH)
														{
																#if(DEF_EVENTINFO_OUTPUTEN > 0)
																if(dbgInfoSwt & DBG_INFO_EVENT)
																		myPrintf("[EVENT]: DSS Sudden maneuvers!\r\n");
																#endif
														}
														else if(dssCode == DEF_GPSDSS_WTH)
														{
																#if(DEF_EVENTINFO_OUTPUTEN > 0)
																if(dbgInfoSwt & DBG_INFO_EVENT)
																		myPrintf("[EVENT]: DSS Harsh maneuvers!\r\n");
																#endif
														}
												}
										}
								}
								else if(dssStep == 1)
								{
										dssPackNum ++;
										if(dssPackNum >= 2)
										{
												if(OSTmr_Count_Get(&rthPackCounter) >= 10000)
												{
														if(dssCode == DEF_GPSDSS_ATH)
														{
																//HAL_DssInQueueProcess	(TYPE_DSS_ATH);
																//HAL_ZiTaiInQueueProcess	(TYPE_ZITAI_ATH,&s_gpsPro,0,0,0,0);
														}		
														else if(dssCode == DEF_GPSDSS_DTH)
														{
																//HAL_DssInQueueProcess	(TYPE_DSS_DTH);
																//HAL_ZiTaiInQueueProcess	(TYPE_ZITAI_DTH,&s_gpsPro,0,0,0,0);
														}	
														else if(dssCode == DEF_GPSDSS_RTH)
														{
																HAL_DssInQueueProcess	(TYPE_DSS_RTH);
														}	
														else if(dssCode == DEF_GPSDSS_WTH)
														{
																HAL_ZiTaiInQueueProcess	(TYPE_ZITAI_WTH,&s_gpsPro,0,0,0,0);
														}	
														OSTmr_Count_Start(&rthPackCounter);		// GPS��ʻ��Ϊ��������ʱ��
												}									
												
												// DSS��ʱ�������
												dssStep			=0;
												dssCode 		=0;
												dssAtimes 	=0;
												dssPackNum	=0;										
										}		
								}
								else
										dssStep =0;		

						}	
						
						// GPS���ݹ��˴���(����λ���ϴ�) ///////////////////////////////////////////////
						if((trsFinishF == 1) && (*(s_devSta.pGps)	==	DEF_GPSHARD_NONE_ERR))	
						{
								// �����ۼ�
								lvboStep ++;
								if(lvboStep == 1)
										ljPoint =0;	// ѭ����ʼ��2D�ۼӵ���
								
								// ����������
								if(s_gps.sta == 'V')
								{
										;						// ����λ������GPS����������
								}
								else if(s_gps.sta == 'A')
								{
										if(s_gps.fs == '2')
										{
												ljPoint++;
												if(ljPoint >= GPS_LB_NUM)	
														memcpy((u8 *)&s_gpsBuf,	(u8 *)&s_gps,	sizeof(GPSData_TypeDef));	// ����GPS����������
										}
										else if(s_gps.fs == '3') 
												memcpy((u8 *)&s_gpsBuf,	(u8 *)&s_gps,	sizeof(GPSData_TypeDef));	// ����GPS����������		
										else
										{
												;				// �Ƿ����ݲ��账��
										}								
								}
								else
								{
										;		// �Ƿ����ݲ��账��
								}
								
								// ��ȷ��ȷ��
								if(lvboStep >= GPS_LB_NUM)
								{
										lvboStep =0;
										if(s_gpsBuf.sta == 'A')	
										{
												memcpy((u8 *)&s_gpsGL,	(u8 *)&s_gpsBuf,	sizeof(GPSData_TypeDef));	// �õ���Ч��λ��,���µ����չ�������
										}
										else
										{
												;	// ���й��˵㶼����λ��ά��ԭ��λ��Ϣ����
										}
										//////////////////////////////////////////////////////////////																							
										HAL_ConvertGpsProData(&s_gpsGL,	&s_gpsPro);													// �����չ������ݸ���ΪЭ������
										memset((u8 *)&s_gpsBuf,	'\0',	sizeof(GPSData_TypeDef));							// ���GPS����������		
										/////////////////////////////
										if(s_gpsPro.speed > s_osTmp.maxSpeed)
													s_osTmp.maxSpeed =s_gpsPro.speed;		// �������ʱ��
								}		
								
								// GPS�ƶ��ж�����
		//						if(s_gpsGL.sta == 'A')
		//								gpsCheckEn =DEF_ENABLE;
		//						else
		//								gpsCheckEn =DEF_DISABLE;
		//						HAL_GpsMoveCheckProcess	(gpsCheckEn,	s_gpsGL.speed/10,	s_cfg.sysMove,	s_cfg.sysMoveT, 5, 60);
						}
				}
						
        // ���GPS����////////////////////////////////////////////////////////////////
				if(GPS_DEVICE.InitSta == 1)
				{
						if(OSTmr_Count_Get(&gpsResetCounter) >= OS_GPS_ERRCHECK_TIME)
						{
								if(APP_ErrQue(&s_err.gpsResetTimes) < 0xff)
								{
										APP_ErrAdd(&s_err.gpsResetTimes);
								}
								HAL_GpsModuleReset(60000);
								OSTmr_Count_Start(&gpsResetCounter);
						}	
				}

				// ����������� ////////////////////			
				OSTmr_Count_Start(&s_swg.gpsCounter);
				OSTimeDly(50);
		}
}
/*
*********************************************************************************************************
*                                            App_TaskMUTUAL()
*
* Description : ͨѶ������OBD���ֽ�������
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void	App_TaskMUTUAL	(void 	*p_arg)
{	
		u8 i=0,index=0,obdIdleFlag=0;
		u8 sendCarInfoFlag=0,	carStateBuf[20]={0},tmpNum=0;	// ���ͳ�����Ϣ��־
		u8 fuelBuf[4]={0};
		u16	carStateLen=0,tmpFuel=0;
		OBDMbox_TypeDef		*obdMsg;	// OBD������Ϣ�ṹ
		CONAckMbox_TypeDef	conAck;	// OBD����Ӧ���ṹ
		SYS_DATETIME	tmpRtc;	// �������ʱ���
	
		p_arg = p_arg;	
		
		// ��������������Ź�  //////////////////////////////////
		OSTmr_Count_Start(&s_swg.mutualCounter);
	
		// �巢����Ϣ����
		memset((u8 *)&s_obdCmd,	0,	sizeof(s_obdCmd));	
			
		while(1)
		{		
				// �����������ȴ�
				while(s_common.iapSta == DEF_UPDATE_KEYWORD)
				{
						OSTmr_Count_Start(&s_swg.mutualCounter);	// �����������
						OSTimeDly(10000);	
				}
				
				// ��һ���ϵ��·�������Ϣ////////////////////////////
				if(sendCarInfoFlag == 0)
				{
						sendCarInfoFlag =1;	// ���־
						// ��һ���ϵ罫��ϵ�������͸�OBD����(OBD���ֻ�û����ʱȡ��)
						HAL_SendOBDProProcess(CMD_CAR_INFO,	0,	0,	s_common.carInfo,	3);	// ����OBD��ǰ���ͳ�ϵ���ͻ��ͺ�		
				}
				
				// OBD���ս�������////////////////////////////////////////
				obdMsg = (OBDMbox_TypeDef *)(OSMboxAccept(OS_OBDAckMbox));
				if(obdMsg != (void *)0)
				{						
						#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						{
								BSP_RTC_Get_Current(&tmpRtc);	// ��ȡ��ǰʱ��
								myPrintf("[%02d/%02d/%02d %02d:%02d:%02d]",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
								myPrintf("[Mailbox]: Receive from OBD!![cmdCode:%d][sn:%d][proCmdId:%04X][dataLen:%d][retCode:%d]\r\n",obdMsg->cmdCode,obdMsg->sn,obdMsg->proCmdId,obdMsg->proParaLen,obdMsg->retCode);
						}
						#endif
					
						// ��Ҫ������ݴ���
						if((obdMsg->cmdCode == CMD_PROTOCOL) ||
							 (obdMsg->cmdCode == CMD_READ_ALARMSTA) ||
							 (obdMsg->cmdCode == CMD_AUTO_TJ) ||
						   (obdMsg->cmdCode == CMD_READ_FUEL))
						{
								// ��촦��
								if((obdMsg->cmdCode == CMD_AUTO_TJ) ||
									 ((obdMsg->cmdCode == CMD_PROTOCOL) && (obdMsg->proCmdId == DEF_CMDID_TJDATA_DEV)))
								{
										// �����������
										if(HAL_SYS_GetState (BIT_SYSTEM_TJSOUND_END) == 1)
										{
												HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_DISABLE);	// ����������״̬���
												if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
														HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound2,	strlen((const char *)s_cfg.sound2));		// ��������������
										}
										// ���״̬����
										if(HAL_SYS_GetState (BIT_SYSTEM_TJSTA) == 1)
												HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_DISABLE);	// ϵͳ�����״̬���
										
										// 2015-9-28�Զ���칫������ȡ
										if((obdMsg->cmdCode == CMD_AUTO_TJ) && (obdMsg->proParaLen >= 16))
										{												
												glob_odoBuf[0]	=obdMsg->proParaBuf[13];
												glob_odoBuf[1]	=obdMsg->proParaBuf[14];
												glob_odoBuf[2]	=obdMsg->proParaBuf[15];
										}
								}
								
								// ��������״̬��ȡ
								if(obdMsg->cmdCode == CMD_READ_ALARMSTA)
								{
										s_osTmp.obdReadCarStaFlag =0;	// �ö�ȡ����״̬���
										OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);	// ���¿�ʼ��ȡ����״̬��ʱͳ��
								}
											
								// ���ݴ���(����ԭ��:�����µ�OBD��Ϣ��ʱ��ʹ֮ǰ��Ϣδ���ͳɹ�Ҳ�滻�ϵ���Ϣ)
								if((obdMsg->proParaLen != 0) && (obdMsg->proParaLen <= DEF_OBDBUF_SIZE))
								{
										// ׷�����賵��״��Ϣ
										/* 2015-6-19 �¶೵��״̬��ѯ֧��
										if((obdMsg->cmdCode == CMD_PROTOCOL) &&(obdMsg->proCmdId == DEF_CMDID_QUECARACK_DEV))
										{
												tmpNum =0;
												carStateLen =0;
												memset(carStateBuf,	0,	sizeof(carStateBuf));
												tmpNum =HAL_UpdataCarState(carStateBuf,	&carStateLen);
												// ��дЭ�����ݰ�
												if((tmpNum != 0) && (tmpNum <= 5) && (carStateLen != 0) && (carStateLen <= 20))
												{
														*(obdMsg->proParaBuf + 6) = *(obdMsg->proParaBuf + 6) + tmpNum;	// ��д״̬��
														memcpy((u8 *)(obdMsg->proParaBuf + obdMsg->proParaLen), carStateBuf,	carStateLen);
														obdMsg->proParaLen = obdMsg->proParaLen + carStateLen;
												}
										}
										*/
										if(obdMsg->cmdCode == CMD_READ_FUEL)
										{
												obdMsg->proCmdId =DEF_CMDID_READFUEL_DEV;
												obdMsg->proParaLen =6;
																						
												fuelBuf[0] = obdMsg->proParaBuf[0];
												fuelBuf[1] = obdMsg->proParaBuf[1];
												fuelBuf[2] = obdMsg->proParaBuf[2];
												fuelBuf[3] = obdMsg->proParaBuf[3];
											
												// ��ֵӦ����ˮ��
												obdMsg->proParaBuf[0] =(u8)(obdMsg->proSn>>8);
												obdMsg->proParaBuf[1] =(u8)(obdMsg->proSn);										
											
												if(obdMsg->retCode == 0)
												{
														// ֧��
														*(obdMsg->proParaBuf+2) = 0x00;	// �ɹ�
														if(fuelBuf[0] == 1)
														{
																*(obdMsg->proParaBuf+3) = 0x01;	// �ٷֱ�
																*(obdMsg->proParaBuf+4) = fuelBuf[2];
																*(obdMsg->proParaBuf+5) = fuelBuf[3];							
														}
														else
														{
																*(obdMsg->proParaBuf+3) = 0x00;	// ��ֵ(L)
																tmpFuel = (u16)(((fuelBuf[1]<<16) | (fuelBuf[2]<<8) | (fuelBuf[3])) / 100 + 0.5);
																*(obdMsg->proParaBuf+4) = (u8)(tmpFuel>>8);
																*(obdMsg->proParaBuf+5) = (u8)tmpFuel;																	
														}														
												}
												else
												{
														// ��֧��
														*(obdMsg->proParaBuf+2) = 0x01;	// ʧ��
														*(obdMsg->proParaBuf+3) = 0x00;	
														*(obdMsg->proParaBuf+4) = 0x00;	
														*(obdMsg->proParaBuf+5) = 0x00;	
												}											
										}
								
										s_obd.obdSendFlag = 0;	// ��ֹ��ǰ�ķ��ʹ���				
										HAL_PackOBDDataProcess	((u16)obdMsg->proCmdId,	(u8 *)obdMsg->proParaBuf,	(u16)obdMsg->proParaLen,	s_obd.buf,	&s_obd.len, (u16 *)&s_obd.sn);														
										
										if(obdMsg->proCmdId == DEF_CMDID_RTDATA_DEV)
												s_obd.ackType	=DEF_ACKTYPE_NOACK;	// ʵʱ�������첽�ϴ�����Ӧ��
										else
												s_obd.ackType	=DEF_ACKTYPE_ACK;		// ����OBD���ϴ�����������ָ���ҪӦ��
										s_obd.obdSendFlag =1;	// �����������������ݰ�
								}
								
								// ����״̬��ȡOBD��֧�ְ¶�״̬�����������
								/* 2015-6-19 �¶೵��״̬��ѯ֧��
								if((obdMsg->cmdCode == CMD_PROTOCOL) && (obdMsg->proCmdId == DEF_CMDID_QUECAR_PLA))
								{
										if(obdMsg->retCode == 2)	// ��֧��
												HAL_ReadCarStaInQueueProcess(glob_readCarSn,	glob_readTimeStamp);										
								}
								*/
								
								// OBD��ϵ��ƥ�����log�������
								if(((obdMsg->cmdCode == CMD_PROTOCOL) && (obdMsg->proCmdId == DEF_CMDID_STARTTJ_PLA)) ||
									 (obdMsg->cmdCode == CMD_AUTO_TJ))
								{
										if(obdMsg->retCode == 3)
												s_err.obdParaRWTimes++;	// �ۼӴ���������¼��������ͳһ���
								}																							
						}
						// ��������Ӧ����
						else
						{
								// �·�������Ϣ����
								if(obdMsg->cmdCode == CMD_CAR_INFO)
								{
										;
								}
								// ���ſ��Ƴ�������
								if(obdMsg->cmdCode == CMD_SMS) 
								{
										conAck.sn=0;									
										conAck.retCode =obdMsg->retCode;				// �洢�����		 
										OSMboxPost(OS_CONAckMbox, (void *)&conAck); // ����OBD����Ӧ������ 								
								}
								// ����ָ���
								if(obdMsg->cmdCode == CMD_TEST)
								{
										if(obdMsg->retCode == 0)
												s_test.busTestCode =0x00;	// �óɹ�״̬
										else if(obdMsg->retCode == 2)
												s_test.busTestCode =0xEE;	// �ô���״̬
								}
								// ��ȡ������Ϩ��״̬����
								if(obdMsg->cmdCode == CMD_GET_CARDATA)
								{
										if(glob_igSta == DEF_IG_RPM_START)
										{
												if(obdMsg->retCode == 0)
												{
														if(obdMsg->proParaBuf[0] == 0xAA)
                            {
																glob_igSta =DEF_IG_RPM_0;		// RPM<100
                            }
														else if(obdMsg->proParaBuf[0] == 0xBB)
                            {
																glob_igSta =DEF_IG_RPM_100;	// RPM>=100
                            }
                            else
                            {
                                glob_igSta = ((u16)(obdMsg->proParaBuf[0u] << 8u)) | ((u16)obdMsg->proParaBuf[1u]);
                            }
												}
												else
                        {
														glob_igSta =DEF_IG_ECU_TIMEOUT;			// ����æ��/��ʱ
                        }
										}
										if(glob_igStaExt == DEF_IG_RPM_START)
										{
												if(obdMsg->retCode == 0)
												{
														if(obdMsg->proParaBuf[0] == 0xAA)
                            {
																glob_igStaExt =DEF_IG_RPM_0;		// RPM<100
                            }
														else if(obdMsg->proParaBuf[0] == 0xBB)
                            {
																glob_igStaExt =DEF_IG_RPM_100;	// RPM>=100
                            }
                            else
                            {
                                glob_igStaExt = ((u16)(obdMsg->proParaBuf[0u] << 8u)) | ((u16)obdMsg->proParaBuf[1u]);
                            }
												}
												else
                        {
														glob_igStaExt =DEF_IG_ECU_TIMEOUT;	// ����æ��/��ʱ
                        }
										}
								}
						}																																
						obdMsg->dataLock =0;	// ���ݽ���
				}																					

				// ����������� ///////////////////////////////////////////////////////			
				OSTmr_Count_Start(&s_swg.mutualCounter);							
				OSTimeDly(50);		
		}
}
/*
*********************************************************************************************************
*                                            App_TaskCON()
*
* Description : 
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void		App_TaskCON	(void 	*p_arg)
{					
		p_arg = p_arg;
	
		// ��������������Ź�  /////////////
		OSTmr_Count_Start(&s_swg.conCounter);
	
		// ��ʼ���ⲿ���ƴ���///////////////
		IO_RS232_POW_EN();
		BSP_USART_Init(DEF_CON_UART,	DEF_CON_REMAP,	DEF_CON_BAUT_RATE);	// �������ʼ���ⲿ�����򴮿ڲ��Խ�ʧ��
		
		/* 2015-6-19 �¶����֧��
		// ��ʼ���ⲿ���ƴ���///////////////
		HAL_CON_Init();		
		
		// �������Դ����ж�
		HAL_CONRES_IT_CON(DEF_ENABLE);			// ʹ�����贮�ڽ����ж�	
		*/
				
		while(1)
		{	
				// �����������ȴ�������con���񽫱�����
				while((s_common.iapSta == DEF_UPDATE_WAIT) || (s_common.iapSta == DEF_UPDATE_KEYWORD))	
				{
						OSTmr_Count_Start(&s_swg.conCounter);	// �����������
						OSTimeDly(10000);	
				}
				
				/* 2015-6-19 �¶����֧��
				HAL_CONMainLoop();
				*/

				// ����������� ///////////////////////////////////////////////////////			
				OSTmr_Count_Start(&s_swg.conCounter);							
				OSTimeDly(10);		
		}
}
/*
*********************************************************************************************************
*                                            App_TaskDBG()
*
* Description : 
*
* Argument(s) : p_arg 
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void		App_TaskDBG	(void 	*p_arg)
{				
		s8		ret=0;
		vu8		runSetp=0, dataSetp=0,	runSta=0,	dataSta=0, accPoint=0, staticFinish=0;
		vu16	upAccIntCnn=0;
		vu32	runCounter=0,dataCounter=0,osStaCounter=0,obdStaCounter=0,gpsStaCounter=0;
		vu32	gsmStaCounter=0,accCounter=0,accCheckCounter=0,accWCheckCounter=0,accMCheckCounter=0,accAdjustCounter=0;	
		OS_STK_DATA		stkCheck;
		SYS_DATETIME	tmpRtc;
		
		p_arg = p_arg;
	
		// ��ʼ�����ٶȴ�����///////////////
		BSP_ACC_Init();		
		
		// ��������������Ź�  /////////////
		OSTmr_Count_Start(&s_swg.dbgCounter);
		
		// ������Ӧ�ü�ʱ��  /////////////
		OSTmr_Count_Start(&runCounter);
		OSTmr_Count_Start(&dataCounter);
		OSTmr_Count_Start(&osStaCounter);
		OSTmr_Count_Start(&obdStaCounter);
		OSTmr_Count_Start(&gpsStaCounter);
		OSTmr_Count_Start(&gsmStaCounter);
		OSTmr_Count_Start(&accCounter);
		OSTmr_Count_Start(&accCheckCounter);
		OSTmr_Count_Start(&accWCheckCounter);
		OSTmr_Count_Start(&accMCheckCounter);
		OSTmr_Count_Start(&accAdjustCounter);
	
		// �������Դ����ж�
		HAL_DBGRES_IT_CON(DEF_ENABLE);			// ʹ�ܵ��Դ��ڽ����ж�	
		
		// �������ٶ��ƶ��ж�		
		BSP_ACC_CfgInt (DEF_ENABLE,	(u8)s_cfg.accMT,	(u8)s_cfg.accMD,	5000);
		BSP_ACC_IT_IO_CON(ENABLE);	// ��ʼ����Ĭ��ʹ�ܼ��ٶ��ⲿ�ж�(���߻��Ѻ󲻽�ֹ)
				
		while(1)
		{	
				// �����������ȴ�������debug���񽫱�����
				while((s_common.iapSta == DEF_UPDATE_WAIT) || (s_common.iapSta == DEF_UPDATE_KEYWORD))	
				{
						OSTmr_Count_Start(&s_swg.dbgCounter);	// �����������
						OSTimeDly(10000);	
				}
				
				// ����DBG�ӿ����� /////////////////////////////////////////////////////	
				if(OSSemAccept(DBGComSem) > 0) 	
				{
						HAL_DBGCmdProcess();
				}
				
				// ָʾ��״̬���� /////////////////////////////////////////////////////
				if(s_cfg.devEn & DEF_DEVEN_BIT_GPRS)
				{
						// LED Runָʾ�ƿ���//////////////////////////////////////////////////////////////////////////        
						if(((FLASH_DEVICE.InitSta == 1) && (*(s_devSta.pFlash)	!=	DEF_FLASHHARD_NONE_ERR)) || 
							 ((MEMS_DEVICE.InitSta != 0) && (*(s_devSta.pMems)		!=	DEF_MEMSHARD_NONE_ERR)))				
								runSta	=LED_STA_ONERR;
						else	
						{
								if(HAL_SYS_GetState(BIT_SYSTEM_TJSTA) == 1)
										runSta	=LED_STA_FTHREE;
								else
								{
										if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 0)
												runSta	=LED_STA_FONE;
										else
												runSta	=LED_STA_FTWO;	
								}
						}
								 
						// ��ʼ����ѭ��
						if(runSetp	==	0)
						{
								OSTmr_Count_Start(&runCounter);
								runSetp	=1;			
						}
						else if(runSetp == 1)
						{
								if(OSTmr_Count_Get(&runCounter) >= LED_SETP1_TIME)	
								{
										if((runSta == LED_STA_ONERR) || (runSta == LED_STA_FONE) || (runSta == LED_STA_FTWO) || (runSta == LED_STA_FTHREE))
												IO_LED_RUN_ON();
										OSTmr_Count_Start(&runCounter);
										runSetp	=2;	
								}	
						}
						else if(runSetp == 2)
						{
								if(OSTmr_Count_Get(&runCounter) >= LED_SETP2_TIME)
								{
										if((runSta == LED_STA_OFF) || (runSta == LED_STA_FONE) || (runSta == LED_STA_FTWO) || (runSta == LED_STA_FTHREE))
												IO_LED_RUN_OFF();
										OSTmr_Count_Start(&runCounter);
										runSetp	=3;	
								}
						}
						else if(runSetp == 3)
						{
								if(OSTmr_Count_Get(&runCounter) >= LED_SETP3_TIME)
								{
										if((runSta == LED_STA_ONERR) || (runSta == LED_STA_FTWO) || (runSta == LED_STA_FTHREE))
												IO_LED_RUN_ON();
										OSTmr_Count_Start(&runCounter);
										runSetp	=4;	
								}
						}
						else if(runSetp == 4)
						{
								if(OSTmr_Count_Get(&runCounter) >= LED_SETP4_TIME)
								{
										if((runSta == LED_STA_OFF) || (runSta == LED_STA_FTWO) || (runSta == LED_STA_FTHREE))
												IO_LED_RUN_OFF();
										OSTmr_Count_Start(&runCounter);
										runSetp	=5;	
								}
						}		
						else if(runSetp == 5)
						{
								if(OSTmr_Count_Get(&runCounter) >= LED_SETP5_TIME)
								{
										if((runSta == LED_STA_ONERR) || (runSta == LED_STA_FTHREE))
												IO_LED_RUN_ON();
										OSTmr_Count_Start(&runCounter);
										runSetp	=6;	
								}
						}
						else if(runSetp == 6)
						{
								if(OSTmr_Count_Get(&runCounter) >= LED_SETP6_TIME)
								{
										if((runSta == LED_STA_OFF) || (runSta == LED_STA_FTHREE))
												IO_LED_RUN_OFF();
										OSTmr_Count_Start(&runCounter);
										runSetp	=7;	
								}
						}							
						// �˳�ѭ��
						else if(runSetp == 7)
						{
								if(OSTmr_Count_Get(&runCounter) >= LED_SETP7_TIME)
								{
										OSTmr_Count_Start(&runCounter);
										runSetp	=0;	
								}
						}             
						// LED Data ָʾ�ƿ���//////////////////////////////////////////////////////////////////////////        
						if(((GPS_DEVICE.InitSta == 1) && (*(s_devSta.pGps) !=	DEF_GPSHARD_NONE_ERR)) || 
							 ((*(s_devSta.pGsm)		!=	DEF_GSMHARD_CHEKING) && (*(s_devSta.pGsm)	!=	DEF_GSMHARD_NONE_ERR))) 					
								dataSta	=LED_STA_ONERR;
						else	
						{
								if((BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1) && 
									 ((BSP_GSM_GetFlag(DEF_BTDING_FLAG) == 1) ||
									  (HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)))
										dataSta	=LED_STA_FTHREE;
								else
								{
										if((BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0) && 
											 (BSP_GSM_GetFlag(DEF_BTDING_FLAG) == 0) &&
											 (HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0))
												dataSta	=LED_STA_OFF;
										else
										{
												if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
														dataSta	=LED_STA_FONE;
												if((BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1) || 
													 (BSP_GSM_GetFlag(DEF_BTDING_FLAG) == 1))
														dataSta	=LED_STA_FTWO;																		
										}
								}						
						}
								 
						// ����ѭ��
						if(dataSetp	==	0)
						{
								OSTmr_Count_Start(&dataCounter);
								dataSetp	=1;			
						}
						else if(dataSetp == 1)
						{
								if(OSTmr_Count_Get(&dataCounter) >= LED_SETP1_TIME)	
								{
										if((dataSta == LED_STA_ONERR) || (dataSta == LED_STA_FONE) || (dataSta == LED_STA_FTWO) || (dataSta == LED_STA_FTHREE))
												IO_LED_DATA_ON();
										OSTmr_Count_Start(&dataCounter);
										dataSetp	=2;	
								}	
						}
						else if(dataSetp == 2)
						{
								if(OSTmr_Count_Get(&dataCounter) >= LED_SETP2_TIME)
								{
										if((dataSta == LED_STA_OFF) || (dataSta == LED_STA_FONE) || (dataSta == LED_STA_FTWO) || (dataSta == LED_STA_FTHREE))
												IO_LED_DATA_OFF();
										OSTmr_Count_Start(&dataCounter);
										dataSetp	=3;	
								}
						}
						else if(dataSetp == 3)
						{
								if(OSTmr_Count_Get(&dataCounter) >= LED_SETP3_TIME)
								{
										if((dataSta == LED_STA_ONERR) || (dataSta == LED_STA_FTWO) || (dataSta == LED_STA_FTHREE))
												IO_LED_DATA_ON();
										OSTmr_Count_Start(&dataCounter);
										dataSetp	=4;	
								}
						}
						else if(dataSetp == 4)
						{
								if(OSTmr_Count_Get(&dataCounter) >= LED_SETP4_TIME)
								{
										if((dataSta == LED_STA_OFF) || (dataSta == LED_STA_FTWO) || (dataSta == LED_STA_FTHREE))
												IO_LED_DATA_OFF();
										OSTmr_Count_Start(&dataCounter);
										dataSetp	=5;	
								}
						}	
						else if(dataSetp == 5)
						{
								if(OSTmr_Count_Get(&dataCounter) >= LED_SETP5_TIME)
								{
										if((dataSta == LED_STA_ONERR) || (dataSta == LED_STA_FTHREE))
												IO_LED_DATA_ON();
										OSTmr_Count_Start(&dataCounter);
										dataSetp	=6;	
								}
						}
						else if(dataSetp == 6)
						{
								if(OSTmr_Count_Get(&dataCounter) >= LED_SETP6_TIME)
								{
										if((dataSta == LED_STA_OFF) || (dataSta == LED_STA_FTHREE))
												IO_LED_DATA_OFF();
										OSTmr_Count_Start(&dataCounter);
										dataSetp	=7;	
								}
						}						
						// �˳�ѭ��
						else if(dataSetp == 7)
						{
								if(OSTmr_Count_Get(&dataCounter) >= LED_SETP7_TIME)
								{
										OSTmr_Count_Start(&dataCounter);
										dataSetp	=0;	
								}
						}      
				}
        else
        {
            IO_LED_RUN_OFF();
            IO_LED_DATA_OFF();
        }

				// ���ٶ����ݸ��´��� /////////////////////////////////////////////////////
				if(OSTmr_Count_Get(&accCounter) >= 1)
				{
						if((*(s_devSta.pMems) ==	 DEF_MEMSHARD_NONE_ERR) && (MEMS_DEVICE.InitSta != 0))
						{
								memset((u8 *)&s_Mems,	0,	sizeof(s_Mems));
								ret = BSP_ACC_ReadXYZ(&s_Mems,	1000);
								if(ret != 0)
											myPrintf("[ACC]:error read\r\n");
								//ret = BSP_ACC_ReadGyro(&s_Mems,	1000);
								//if(ret != 0)
								//			myPrintf("[GYRO]:error read\r\n");
								if(OSTmr_Count_Get(&accAdjustCounter) >= 5000)
										HAL_Calculate_Main (s_Mems.X_Acc,	s_Mems.Y_Acc,	s_Mems.Z_Acc,	(u8 *)&glob_accAdjustFlag);	// �ж��¼�
						}	
						OSTmr_Count_Start(&accCounter);	
				}
				
				// ���ٶ��ƶ�̽��
				if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (MEMS_DEVICE.InitSta != 0))
				{
						#if(DEF_ACCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_ACC)
						{
								if(s_ACC.IntCnn!=upAccIntCnn)
								{
										upAccIntCnn =s_ACC.IntCnn;
										if(s_ACC.IntCnn != 0)
												myPrintf("\r\nACC_IntCnn:%d\r\n",	s_ACC.IntCnn);
								}
						}
						#endif
						if(OSTmr_Count_Get(&accMCheckCounter) >= 1000)
						{
								HAL_AccMoveCheckProcess	(&s_ACC.IntCnn,	10,	10);
								OSTmr_Count_Start(&accMCheckCounter);	
						}		
				}
				
				// ���ٶ�оƬ���ϼ��
				if(OSTmr_Count_Get(&accWCheckCounter) >= 10000)
				{
						BSP_ACC_Check();
						OSTmr_Count_Start(&accWCheckCounter);	
				}			

				// OS��Ϣ���//////////////////////////////////////////////////////////////////////////////
				if(OSTmr_Count_Get(&osStaCounter) >= 10000)
				{
						#if(DEF_OSINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OS)
						{														
								OSTaskStkChk(APP_TASK_START_PRIO,	&stkCheck);		
								myPrintf("\r\n\r\n[START-Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));
								OSTaskStkChk(APP_TASK_GSM_PRIO,	&stkCheck);	  
								myPrintf("[GSM  -Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));
								OSTaskStkChk(APP_TASK_GPRS_PRIO,	&stkCheck); 
								myPrintf("[GPRS -Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));								
								OSTaskStkChk(APP_TASK_RXD_PRIO,	&stkCheck);		
								myPrintf("[RXD  -Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));		
								OSTaskStkChk(APP_TASK_SEND_PRIO,	&stkCheck);	
								myPrintf("[SEND -Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));								
								OSTaskStkChk(APP_TASK_PROTOCOL_PRIO,	&stkCheck);	
								myPrintf("[PROTO-Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));								
								OSTaskStkChk(APP_TASK_BASE_PRIO,	&stkCheck);	
								myPrintf("[BASE -Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));								
								OSTaskStkChk(APP_TASK_BT_PRIO,	&stkCheck);		
								myPrintf("[BT   -Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));																			
								OSTaskStkChk(APP_TASK_GPS_PRIO,	&stkCheck);		
								myPrintf("[GPS  -Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));								
								OSTaskStkChk(APP_TASK_MUTUAL_PRIO,	&stkCheck);		
								myPrintf("[MUTUA-Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));
								OSTaskStkChk(APP_TASK_CON_PRIO,	&stkCheck);		
								myPrintf("[CON  -Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));
								OSTaskStkChk(APP_TASK_DBG_PRIO,	&stkCheck);		
								myPrintf("[DBG  -Task]All: %d ;Pt: %2d%% \r\n",	(stkCheck.OSFree/4 + stkCheck.OSUsed/4),	((stkCheck.OSUsed*100)/(stkCheck.OSFree + stkCheck.OSUsed)));
																					
								myPrintf("[CPU&&USAGE]Usage = %d%%\r\n",	OSCPUUsage);
								BSP_RTC_Get_Current(&tmpRtc);
								myPrintf("%d-%d-%d  %d:%d:%d\r\n\r\n",	tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
						}
						#endif
						OSTmr_Count_Start(&osStaCounter);		
				}

				// OBD��Ϣ���
				if(OSTmr_Count_Get(&obdStaCounter) >= 10000)
				{
				
						#if(DEF_OBDINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OBD)
						{
//								myPrintf("\r\n[RPM    ]=%d",		(u16)(J1939_data.rpm/8));
//								myPrintf("\r\n[SPEED  ]=%d",		(u16)(J1939_data.speed/256));
//								myPrintf("\r\n[ENG_LAD]=%d%%",	(u8)J1939_data.eng_load);
//								myPrintf("\r\n[COOL_TP]=%-d",		(s16)(J1939_data.eng_cool_tmp-40));								
//								myPrintf("\r\n[IN_TP  ]=%-d",		(s16)(J1939_data.eng_intake_tmp-40));
//								myPrintf("\r\n[FU_RATE]=%d",		(u16)(J1939_data.fuel_rate/20));
//								myPrintf("\r\n[FU_LEVL]=%d%%",	(u16)(J1939_data.fuel_level/2.5));
//								myPrintf("\r\n[RUNTIME]=%d",		(u32)(J1939_save_data.ecu_run_time));
//								myPrintf("\r\n[ODO    ]=%d",		(u32)(J1939_save_data.odometer/8));
//								myPrintf("\r\n[FU     ]=%d",		(u32)(J1939_save_data.fuel_used/2));
//								myPrintf("\r\n[MIL    ]=%d",		J1939_data.mil);
//								myPrintf("\r\n[DCT_NUM]=%d",		J1939_data.dtc_num);
//								myPrintf("\r\n[DCT_STR]=%s",		J1939_data_string.dtc_code);
//								myPrintf("\r\n");
				
								BSP_RTC_Get_Current(&tmpRtc);
								myPrintf("%d-%d-%d	%d:%d:%d\r\n\r\n",	tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
						}
						#endif
						OSTmr_Count_Start(&obdStaCounter);
				}

				// GPS��Ϣ���
				if(OSTmr_Count_Get(&gpsStaCounter) >= 5000)
				{
						#if(DEF_GPSINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GPSSTA)
						{																					
								myPrintf("\r\n[GPS-STA  ]=%c",	s_gpsGL.sta);
								myPrintf("\r\n[GPS-FS   ]=%c",	s_gpsGL.fs);
								myPrintf("\r\n[GPS-LAT  ]=%d",	s_gpsGL.latitude);
								myPrintf("\r\n[GPS-LON  ]=%d",	s_gpsGL.longitude);
								myPrintf("\r\n[GPS-HIGH ]=%-d",	s_gpsGL.high);
								myPrintf("\r\n[GPS-SPEED]=%d",	s_gpsGL.speed);
								myPrintf("\r\n[GPS-HEAD ]=%d",	s_gpsGL.heading);
								myPrintf("\r\n[GPS-NOSV ]=%d",	s_gpsGL.noSV);
								myPrintf("\r\n[GPS-24RST]=%d",	s_err.gpsResetTimes);								
								BSP_RTC_Get_Current(&tmpRtc);
								myPrintf("\r\n%d-%d-%d	%d:%d:%d\r\n",	tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);		
						}
						#endif
						OSTmr_Count_Start(&gpsStaCounter);
				}

				// GSM��Ϣ���
				if(OSTmr_Count_Get(&gsmStaCounter) >= 5000)
				{
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSMSTA)
						{										
								if(BSP_GSM_GetFlag(DEF_BTDING_FLAG) == 1)
										myPrintf("\r\n[GSM-BTSTA]=A");
								else
										myPrintf("\r\n[GSM-BTSTA]=V");
								
								if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
										myPrintf("\r\n[GSM-STA  ]=A");
								else
										myPrintf("\r\n[GSM-STA  ]=V");	
												
								myPrintf("\r\n[GSM-LEVEL]=%d",	s_gsmApp.sLeve);
								myPrintf("\r\n[GSM-RSSI ]=%d",	s_gsmApp.rssi);
								myPrintf("\r\n[GSM-24RST]=%d",	s_err.gsmResetTimes);
								
								BSP_RTC_Get_Current(&tmpRtc);
								myPrintf("\r\n%d-%d-%d	%d:%d:%d\r\n",	tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);		
						}
						#endif
            
            #if(DEF_EVENTINFO_OUTPUTEN > 0)
            if(dbgInfoSwt & DBG_INFO_EVENT)
            {
                if (s_cfg.devEn & DEF_DEVEN_BIT_GPRS)
                {
                }
                else
                {
                    myPrintf("[EVENT_POWER]:Device is transporting!!!\r\n");
                }
            }
            #endif
            
						OSTmr_Count_Start(&gsmStaCounter);
				}			

				// ����������� ///////////////////////////////////////////////////////			
				OSTmr_Count_Start(&s_swg.dbgCounter);							
				OSTimeDly(10);		
		}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                             ��APP FUNCTION��
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                             taskDelProcess()
* Description : ����ɾ������timeout ��λms
* Argument(s) : none.
* Return(s)   : none.
*********************************************************************************************************
*/
static	s8	taskDelProcess (u32	timeout)
{
		u16	i=0;
		vu32	timeoutConuter=0;
		
		// ��ʱ��ʱ��ʼ
		OSTmr_Count_Start(&timeoutConuter);
	
		// ɾ��OBD����
		if(HAL_SYSParaSemPend(timeout) != 0)
		{
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: Fatal error happen, Can't pend parameter w/r sem!!\r\n");
				#endif
				return -1;
		}
		else
		{
				#ifdef DEF_OBDTASK_CREATE
				OSTaskDel(DEF_OBD_TASK_TRANSFERLAYPER_PRIO);
				OSTaskDel(DEF_OBD_TASK_SCHEDULE_PRIO);
				OSTaskDel(DEF_OBD_TASK_FUN_PRIO);
				OSTaskDel(DEF_OBD_TASK_ECU_PRIO);
				#endif
				HAL_SYSParaSemPost();	// �ͷŲ�����д�ź����Ա���������ʹ��(�ô�һ��Ҫ�ͷŷ��򽫲��ɶ�дϵͳ����)		
		}	
		return	0;
}

/*
*********************************************************************************************************
*                                             smsConForamtCheck()
* Description : �����ƶ��Ÿ�ʽ�����ȷ�������SMSCon_Typedef�ṹ��
* Argument(s) : none.
* Return(s)   : 0��ȷ��<0����
*********************************************************************************************************
*/
static	s8	smsConForamtCheck (SMS_Typedef *pSMS,	SMSCon_Typedef *pSMSOut)
{
		u16 i=0,j=0,tmpHexLen=0,tmpLen=0;
		u8	tmpHexBuf[80]={0};
		u8	tmpBuf[10]={0};	// ����������ʱ�洢
			
		// �������ݽ���	
		if((pSMS->dataLen != 48) || (IsHexString(pSMS->dataBuf,	pSMS->dataLen) == 0xffff))
				return DEF_SMSCONTROL_FORMAT;	// ���ƶ��ż��ܺ󳤶�Ϊ48char��24byte,�Ҷ�Ϊhex�ַ�����ʽ
		
		// ��ʽת����hex����
		memset(tmpHexBuf, 0, sizeof(tmpHexBuf));
		tmpHexLen =String2HexArray (pSMS->dataBuf,	pSMS->dataLen,	tmpHexBuf);	
		if(desDecipherPro(tmpHexBuf,	tmpHexLen,	pSMSOut->dataBuf,	&pSMSOut->dataLen) != 0)
		{
				memcpy(pSMSOut->dataBuf,	pSMS->dataBuf,	pSMS->dataLen);	// ����ʧ��ֱ�Ӱ�δ���ܵ�ԭ�������ݱ����Ա�������ͳ�ȥ
				pSMSOut->dataLen =pSMS->dataLen;
				return DEF_SMSCONTROL_DES;	// ���ƶ��Ž���ʧ��
		}
		
		j =StringSearch(pSMSOut->dataBuf,(u8 *)GSM_SMS_CONTROL_H,pSMSOut->dataLen,strlen((const char*)GSM_SMS_CONTROL_H));
		if(j == 0xffff)
				return DEF_SMSCONTROL_FORMAT;	// �Ƿ���ʽ
		
		// ��ȡ������
		memset(tmpBuf, 0, sizeof(tmpBuf));
		j +=strlen((const char*)GSM_SMS_CONTROL_H);						
		for(i=0;	(i<4) && (i<pSMSOut->dataLen) && (pSMSOut->dataBuf[i] != ',');	i++)
		{
				tmpBuf[i] = pSMSOut->dataBuf[j+i];
		}
		tmpBuf[i] = '\0';
		pSMSOut->cmdLen =String2HexArray (tmpBuf,	i,	pSMSOut->cmdCode);	
		
		// ��ȡʱ���
		memset(tmpBuf, 0, sizeof(tmpBuf));
		j =j+i+1;		
		for(i=0;	(i<8) && (i<pSMSOut->dataLen) && (pSMSOut->dataBuf[i] != ',');	i++)
		{
				tmpBuf[i] = pSMSOut->dataBuf[j+i];
		}
		tmpBuf[i] = '\0';		
		sscanf((const char *)&tmpBuf,	"%x",	&pSMSOut->smsTimStamp);
		
		// �绰�����ʽ�Ϸ���	(�ô������������û�е绰��������)
		if(strlen((const char *)pSMS->numBuf) != 0)
		{
				j =0;
				tmpLen =strlen((const char *)(pSMS->numBuf + j));
				if(pSMS->numBuf[0] == '+')
				{
						j +=1;
				}
				for(i=0;	j<tmpLen;	i++,j++)
				{
						pSMSOut->numBuf[i] = pSMS->numBuf[j]; 		
				}	
				pSMSOut->numBuf[i] ='\0';
				pSMSOut->numLen=i;
		}
		
		return DEF_SMSCONTROL_NONE;
}
/*
*********************************************************************************************************
*                                             smsCarControlProcess()
* Description : ���ſ���(�����ų�ʱ������ʱ�����ʱ)
* Argument(s) : none.
* Return(s)   : DEF_SMSCONTROL_NONE~DEF_SMSCONTROL_TIMESTAMP
*********************************************************************************************************
*/
static	s8	smsCarControlProcess (SMS_Typedef *pSMS,	SMSCon_Typedef *pSMSOut)
{
		s8	ret=0;
		u32	tmpCun=0,	tmpSms=0;
		SYS_DATETIME   tmpR;		
						
		// ���ƺ���Ϸ���
		if(strlen((const char *)pSMS->numBuf) != 0)
		{
				// �����·�����
				if((StringSearch(pSMS->numBuf,s_cfg.pho1,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho1)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho2,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho2)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho3,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho3)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho4,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho4)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho5,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho5)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho6,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho6)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.smsCenter,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.smsCenter)) != 0xffff))
				{				
						//���ų�ʱ�ж�  /////////////////////////////////////////////////////////////////////////////
						
						BSP_RTC_Get_Current(&tmpR);	
						tmpCun	=	Mktime(tmpR.year, tmpR.month, tmpR.day, tmpR.hour, tmpR.minute, tmpR.second); // �豸����ΪGMTʱ��
						tmpSms	=	Mktime(pSMS->sTime.year, pSMS->sTime.month, pSMS->sTime.day, pSMS->sTime.hour, pSMS->sTime.minute, pSMS->sTime.second);										
						if(((tmpCun >= tmpSms) && ((tmpCun - tmpSms) <= OS_PROCONSMS_RECTIMEOUT)) || ((tmpCun < tmpSms) && ((tmpSms - tmpCun) <= OS_PROCONSMS_RECTIMEOUT)))
						{
								// ��������ʱ�����ʱ�ж�
								if(((pSMSOut->smsTimStamp >= tmpSms) && ((pSMSOut->smsTimStamp - tmpSms) <= OS_PROCOMSMS_STAMPTIMEOUT)) || 
									 ((pSMSOut->smsTimStamp < tmpSms) && ((tmpSms - pSMSOut->smsTimStamp) <= OS_PROCOMSMS_STAMPTIMEOUT)))
								{
										if(HAl_SMSCarControl(pSMSOut->cmdCode, pSMSOut->cmdLen) == 0)										
												ret =DEF_SMSCONTROL_NONE;	// �ɹ�
										else
												ret =DEF_SMSCONTROL_BUSY;	// ����æ
								}
								else
								{
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[EVENT]: SMS timestamp timeout!\r\n");
										#endif
										ret =DEF_SMSCONTROL_TIMESTAMP;	// ʱ�����
								}
						}
						else
						{
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[EVENT]: SMS Control timeout!\r\n");
								#endif
								ret =DEF_SMSCONTROL_TIMEOUT;	// ���Ƴ�ʱ
						}
				}
				else
						ret =DEF_SMSCONTROL_NUM;	// �Ƿ�����
		}
		else 
		{
				// ��������ʱ�����ʱ�ж�////////////////////////////////////////////////////////////////////
				BSP_RTC_Get_Current(&tmpR);	
				tmpCun	=	Mktime(tmpR.year, tmpR.month, tmpR.day, tmpR.hour, tmpR.minute, tmpR.second); // �豸����ΪGMTʱ��
				if(((pSMSOut->smsTimStamp >= tmpCun) && ((pSMSOut->smsTimStamp - tmpCun) <= OS_PROCOMSMS_STAMPTIMEOUT)) || ((pSMSOut->smsTimStamp < tmpCun) && ((tmpCun - pSMSOut->smsTimStamp) <= OS_PROCOMSMS_STAMPTIMEOUT)))
				{
						if(HAl_SMSCarControl(pSMSOut->cmdCode, pSMSOut->cmdLen) == 0)										
								ret =DEF_SMSCONTROL_NONE;	// �ɹ�
						else
								ret =DEF_SMSCONTROL_BUSY;	// ����æ
				}
				else
				{
						#if(DEF_EVENTINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_EVENT)
								myPrintf("[EVENT]: SMS timestamp timeout!\r\n");
						#endif
						ret =DEF_SMSCONTROL_TIMESTAMP;	// ʱ�����ʱ
				}			
		}
		return ret;
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                             ��APP Tool��
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*********************************************************************************************************
*                                          uC/OS-II APP HOOKS
*********************************************************************************************************
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)


#endif


