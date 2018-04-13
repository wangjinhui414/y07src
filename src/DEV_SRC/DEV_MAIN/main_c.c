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

// LED状态定义
#define		LED_STA_OFF							((u8)0)				// LED常关状态		
#define		LED_STA_ONERR						((u8)1)				// LED常开故障状态	
#define		LED_STA_FONE						((u8)2)				// LED快闪烁一次状态
#define		LED_STA_FTWO						((u8)3)				// LED快闪烁两次状态	
#define		LED_STA_FTHREE					((u8)4)				// LED快闪烁三次状态	

#define   LED_SETP1_TIME          (500)         // 空闲
#define   LED_SETP2_TIME          (50)          // 第一次点亮
#define   LED_SETP3_TIME          (200)         // 空闲
#define   LED_SETP4_TIME          (50)          // 第二次点亮
#define   LED_SETP5_TIME          (200)         // 空闲
#define   LED_SETP6_TIME          (50)          // 第三次点亮
#define   LED_SETP7_TIME          (500)         // 空闲

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

// 底层应用/////////////////////////////////////////////
OS_EVENT							*FlashMutexSem;						// 用于FLASH SPI总线底层应用之间的互斥
OS_EVENT							*ACCMutexSem;							// 用于ACC总线底层应用之间的互斥
OS_EVENT							*CrashMEMSSem;						// 碰撞信号量
OS_EVENT							*GPSComSem;								// GPS串口接收信号量
OS_EVENT							*GSMMutexSem;							// GSM底层应用互斥信号量
OS_EVENT							*GSMSmsSem;								// 收到短信信号量
OS_EVENT							*GSMGprsDataSem;					// GPRS接收数据信号量
OS_EVENT							*GSMBtDataMbox;						// 蓝牙接收数据邮箱
OS_EVENT							*GSMBtAppMbox;						// 蓝牙获取业务状态邮箱
OS_EVENT							*DBGComSem;								// DBG收到数据信号量
OS_EVENT							*DBGMutexSem;							// DBG printf输出调试信息互斥信号量
OS_EVENT							*RamQueMutexSem;					// RAM数据队列操作互斥信号量
OS_EVENT							*FlashQueMutexSem;				// FLASH数据队列操作互斥信号量
OS_EVENT							*QueMutexSem;							// 队列操作互斥信号量
OS_EVENT							*ParaMutexSem;						// CPU Flash参数读写互斥信号量
OS_EVENT							*CONComMbox;							// 外设串口底层接收邮箱

// 系统层应用///////////////////////////////////////////
OS_EVENT							*OS_DownACKMbox;					// 设备升级交互应答邮箱
OS_EVENT							*OS_ACKMbox;							// 收到应答协议数据包
OS_EVENT							*OS_OBDReqMbox;						// OBD请求邮箱(通讯->OBD)
OS_EVENT							*OS_OBDAckMbox;						// OBD应答邮箱(OBD->通讯)
OS_EVENT							*OS_RecGprsMbox;					// 收到GPRS数据送解析
OS_EVENT							*OS_RecSmsMbox;						// 收到SMS数据送解析
OS_EVENT							*OS_RecBtMbox;						// 收到BT数据送解析
OS_EVENT							*OS_CONReqMbox;						// 外设请求控制邮箱
OS_EVENT							*OS_CONAckMbox;						// 外设应答控制应答邮箱

// 数据发送临时变量/////////////////////////////////////
SendData_TypeDef			comAck;			// 用于存储通用应答临时缓冲区结构 
SendData_TypeDef			tmpSms;			// 用于发送短信临时发送区 
SMSCon_Typedef				conSms;			// 用于临时存储短信控制内容

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

// 系统本地全局变量
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
* Argument(s): Max count 0xFFFFFFFF (49.7小时)
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
* Description: hex进制数据发送打包处理(用于蓝牙或gprs发送Debug输出)
* Argument(s): 0表示打印gprs数据1表示打印bt数据
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
		
		// 获取当前RTC时间
		if(s_common.iapSta != DEF_UPDATE_KEYWORD)	// 升级过程中不打印RTC时间戳
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
* Description: 软件看门狗睡眠中喂狗处理
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
* Description: 任务挂起处理(睡眠前调用以挂起除睡眠以外的其它所有任务)
* Argument(s): 
* Return(s)  : 
* Note(s)    : 
*********************************************************************************************************
*/
void	OS_SuspendAllTaskProcess (void)
{
		// 挂起通信部分相关任务
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
		// 挂起OBD相关任务	
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
* Description: 任务恢复处理(唤醒后调用以恢复之前挂起的任务)
* Argument(s): 
* Return(s)  : 
* Note(s)    : 
*********************************************************************************************************
*/
void	OS_ResumeAllTaskProcess (void)
{
		// 挂起通信部分相关任务
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
		// 挂起OBD相关任务	
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
* Description: 软件看门狗超时判断处理
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
				// 该任务升级过程中保留！
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
				// 该任务升级过程中保留！
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
				// 该任务升级过程中保留！
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
				// 该任务升级过程中挂起！
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
				// 该任务升级过程中保留！
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
				// 该任务升级过程中保留！
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
				// 该任务升级过程中挂起！
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
				// 该任务升级过程中挂起！
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
				// 该任务升级过程中挂起！
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
				// 该任务升级过程中挂起！
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
				// 该任务升级过程中挂起！
				sWDTFlag =1;
				// 外设控制任务暂不分配看门狗变量
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
		// OBD任务暂时不做监控
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
				// 存储					
				HAL_CPU_ResetPro();				// 软件复位		
				return	1;	
		}
		return 0;
}
/*
*********************************************************************************************************
*                                     sleepCheckProcess
* Description: 睡眠逻辑检查处理(需要扫描调用)
* Argument(s): sendCounter 用于拨号超时，idleCounter用于进入睡眠前的空闲超时
* Return(s)  : 
* Note(s)    : 
*********************************************************************************************************
*/
static	void	sleepCheckProcess (vu8 *step,	vu32 *idleCounter)
{
		s8	ret=0;
		
		// 睡眠检测 /////////////////////////////////////////////////////////
		if(*step	== 0)
		{	
				if((HAL_SYS_GetState(BIT_SYSTEM_IG)  == 0) && (HAL_SYSALM_GetState(BIT_ALARM_ILLMOVE) == 0))
				{
						if((s_common.iapSta != DEF_UPDATE_WAIT) && 				// 升级等待过程中不进入睡眠	
							 (s_common.iapSta != DEF_UPDATE_KEYWORD) && 		// 升级过程中不进入睡眠						
							 (BSP_GSM_GetFlag(DEF_CALLDOING_FLAG) == 0)	&& 	// 有电话及短信也不进入睡眠
					 		 (BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0))
						{
								if(s_osTmp.obdReadCarStaFlag == 1)
								{
										if(OSTmr_Count_Get(&s_osTmp.obdReadCarStaCounter) >= OS_INSLEEP_READCARSTA_TIMEOUT)
										{
												s_osTmp.obdReadCarStaFlag =0;	// 清读取车辆状态标志
												//*step =1;											// 读取车辆状态超时直接进入睡眠													
										}
								}
								else
								{
										// 所有数据发送完毕!(不包含SMS队列)
										if((HAL_ComQue_size (DEF_RAM_QUE) == 0) 	&& (s_ram.len == 0) 	&&
											 (HAL_ComQue_size (DEF_FLASH_QUE) == 0) && (s_flash.len == 0) &&
										   (s_obd.len == 0) && (s_comAck.len == 0))										   
										{	
												if(OSTmr_Count_Get(&s_osTmp.sendDataToCounter) >= OS_INSLEEP_SENDDATA_TIMEOUT)
														s_osTmp.sleepWaitFlag =1;	// 进入睡眠等待
										}
										// 还有数据发送
										else
										{		
												if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
												{
														//OSTmr_Count_Start(&s_osTmp.sendDataToCounter);		// 开始发送数据超时计时		
												}
												else
												{
														if(OSTmr_Count_Get(&s_osTmp.sendDataToCounter) >= OS_INSLEEP_SENDDATA_TIMEOUT)
																s_osTmp.sleepWaitFlag =1;	// 进入睡眠等待
																//*step =1;		// 断网后重连及发送数据超时(默认1分钟)	
												}
										}
								}
						}
						else	
								OSTmr_Count_Start(&s_osTmp.sendDataToCounter);		// 开始发送数据超时计时	
						
						// 进入睡眠等待流程
						if(s_osTmp.sleepWaitFlag == 1)
						{
								if(s_cfg.globSleepTime >= s_cfg.igOffDelay)
								{
										if((OSTmr_Count_Get(&s_osTmp.globSleepDelayCounter) >= (s_cfg.globSleepTime - s_cfg.igOffDelay) * 1000) &&
											 (glob_accOnRalStep == 0))		
										{
												*step =1;		// 强制等待30分钟后直接进入睡眠(且需要与点火检测逻辑结束同步)
										}
										else
										{
//												// 睡眠等待过程中实时检测队列信息，如果有数据产生则退出等待状态
//												if((HAL_ComQue_size (DEF_RAM_QUE) == 0) 	&& (s_ram.len == 0) 	&&
//													 (HAL_ComQue_size (DEF_FLASH_QUE) == 0) && (s_flash.len == 0) &&
//													 (s_obd.len == 0) && (s_comAck.len == 0))	
//												{
//														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
//																s_osTmp.sleepWaitFlag =0;	// 退出等待状态											
//												}
										}
								}
								else
								{
										if(glob_accOnRalStep == 0)
												*step =1;		// 直接进入睡眠(且需要与点火检测逻辑结束同步)
								}
						}						
				}
				else
				{
					  OSTmr_Count_Start(&s_osTmp.sendDataToCounter);		// 开始发送数据超时计时
						s_osTmp.sleepWaitFlag =0;
						OSTmr_Count_Start(&s_osTmp.globSleepDelayCounter);		// 开始全局睡眠等待计时 2015-2-4 by:gaofei 给与用30分钟的车辆状态读取时间					
				}
		}
		
		// 睡眠处理 /////////////////////////////////////////////////////////////
		if(*step >= 1)
		{
				if(*step == 1)
				{
						// 发送休眠数据包
						if((s_comPack.packType == 0) && (BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1))
						{
								s_comPack.packType = PACK_TYPE_SLEEP;		// 发送睡眠数据包			
								OSTmr_Count_Start(idleCounter);		
								*step =2;
						}
						else
						{
								s_osTmp.gsmConSleep =1;	// 请求GSM模块进入睡眠
								*step =3;
						}
				}
				else if(*step == 2)
				{
						if(OSTmr_Count_Get(idleCounter) >= 3000)
						{
								s_osTmp.gsmConSleep =1;	// 请求GSM模块进入睡眠
								*step =3;
						}
				}
				else if(*step == 3)
				{
						if(s_osTmp.gsmConSleep == 0)
						{
								*step	=0;		// GSM断网进入睡眠模式执行失败退出(有电话或短信或再次点火等其它异常发生)
						}
						else if(s_osTmp.gsmConSleep == 1)
						{
								;						// GSM模块断网睡眠处理中
						}
						else if(s_osTmp.gsmConSleep == 2)
						{									
								ret	= HAL_PerpheralInPowerMode();													
								if(ret ==	0)
								{
										// 成功进入睡眠耗模式
												// 睡眠中..................
										// 从睡眠模式中唤醒	
										while(1)
										{
												#if(OS_SYSWDG_EN > 0)	
												IWDG_ReloadCounter();						// 喂狗(对于GD32来说喂完狗后不能直接调用进入STOP模式,必须先配置RTC ALM后再进入STOP)
												#endif											
												SWDGInSleepReload();						// 软件看门狗喂狗(方式设备长时间睡眠唤醒后导致软件看门狗复位)									
											
												if(BSP_PVD_GetSta() == 1)
														break;											// PVD<2.9V事件直接退出正常睡眠流程直接判断复位CPU
												if(rtcWakeUpFlag == 1)
												{
														rtcWakeUpFlag	=0;																
														BSP_RTCSoft_ISRHandler();		// 手动更新RTC年月日
																													
														if(s_cfg.offReportTime != 0)																
														{
																if(RTCTmr_Count_Get(&s_osTmp.igOffMsgCounter) >= s_cfg.offReportTime)
																		break;							// 产生离线数据
														}	
														if(1)		// 健康包使能
														{												
																if(RTCTmr_Count_Get(&s_osTmp.report24Counter) >= s_cfg.healthReportTime)
																		break;							// 产生健康包数据(24小时报告)					
														}		
														BSP_RTCSetAlarmCounter(OS_RTC_ALM_SECS);
														PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
												}
												else
														break;
										}
										HAL_PerpheralOutPowerMode();	
										//HAL_SendOBDProProcess(CMD_SYS_WAKEUP,	0,	0,	(u8 *)"",	0);		// 通知obd任务设备唤醒		
										if(s_cfg.devEn & DEF_DEVEN_BIT_CON)
												HAL_SendCONProProcess(CAR_CMD_READSTA,	0x0000,	(u8 *)"",	0);	// 通知外设请求一次状态读取
										
										// 由唤醒条件决定睡觉超时时间
										s_osTmp.sleepWaitFlag =0;	// 重新开始睡眠等待计时
										if(s_wakeUp.state == 0)
												OSTmr_Count_Start(&s_osTmp.sendDataToCounter);						// 设备主动唤醒5分钟超时睡眠
										else
										{
												if((s_wakeUp.state & DEF_WAKEUP_SHAKE) ||
													 (RTCTmr_Count_Get(&s_osTmp.report24Counter) >= s_cfg.healthReportTime))
														OSTmr_Count_Start(&s_osTmp.sendDataToCounter);				// 2015-8-6 由加速度振动及健康包唤醒设备5分钟进入睡眠
												else
														OSTmr_Count_Start(&s_osTmp.globSleepDelayCounter);		// 设备被动唤醒30分钟超时睡眠
										}											
										*step	=0;													
								}
								else
								{
										OSTmr_Count_Start(&s_osTmp.sendDataToCounter);		// 重新开始记录发送超时计时
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
*                                             【APP Event】
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

// 底层应用
//FLASH/////////////////////////////////////////////
		FlashMutexSem	=	OSSemCreate(1);								// 用于FLASH SPI总线底层应用之间的互斥
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(FlashMutexSem, (u8 *)"FlashMutexSem", &os_err);
#endif

//MPU6050///////////////////////////////////////////	
		ACCMutexSem	=	OSSemCreate(1);									// 用于ACC总线底层应用之间的互斥
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(ACCMutexSem, (u8 *)"ACCMutexSem", &os_err);
#endif

		CrashMEMSSem	=	OSSemCreate(0);								// 碰撞信号量
#if (OS_EVENT_NAME_SIZE > 12)
    OSEventNameSet(CrashMEMSSem, (u8 *)"CrashMEMSSem", &os_err);
#endif	
	
//GPS///////////////////////////////////////////////
		GPSComSem	=	OSSemCreate(0);										// GPS串口接收信号量
#if (OS_EVENT_NAME_SIZE > 9)
    OSEventNameSet(GPSComSem, (u8 *)"GPSComSem", &os_err);
#endif		
	
//GSM///////////////////////////////////////////////
		GSMMutexSem	=	OSSemCreate(1);									// GSM底层应用互斥信号量
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(GSMMutexSem, (u8 *)"GSMMutexSem", &os_err);
#endif		
	
		GSMSmsSem	=	OSSemCreate(0);										// 收到短信信号量
#if (OS_EVENT_NAME_SIZE > 9)
    OSEventNameSet(GSMSmsSem, (u8 *)"GSMSmsSem", &os_err);
#endif	

		GSMGprsDataSem	=	OSSemCreate(0);							// GPRS接收数据信号量
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(GSMGprsDataSem, (u8 *)"GprsDataSem", &os_err);
#endif	

		GSMBtDataMbox = OSMboxCreate((void *)0);  		// 蓝牙接收数据邮箱
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(GSMBtDataMbox, (u8 *)"GSMBtDataMbox", &os_err);
#endif

		GSMBtAppMbox = OSMboxCreate((void *)0);  			// 蓝牙获取业务状态邮箱
#if (OS_EVENT_NAME_SIZE > 12)
    OSEventNameSet(GSMBtAppMbox, (u8 *)"GSMBtAppMbox", &os_err);
#endif

//Debug/////////////////////////////////////////////
		DBGComSem	=	OSSemCreate(0);										// DBG收到数据信号量
#if (OS_EVENT_NAME_SIZE > 9)
    OSEventNameSet(DBGComSem, (u8 *)"DBGComSem", &os_err);
#endif		

		DBGMutexSem	=	OSSemCreate(1);									// DBG printf输出调试信息互斥信号量
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(DBGMutexSem, (u8 *)"DBGMutexSem", &os_err);
#endif	

		RamQueMutexSem = OSSemCreate(1);  						// RAM数据队列操作互斥信号量
#if (OS_EVENT_NAME_SIZE > 14)
    OSEventNameSet(RamQueMutexSem, (u8 *)"RamQueMutexSem", &os_err);
#endif

		FlashQueMutexSem = OSSemCreate(1);  					// FLASH数据队列操作互斥信号量
#if (OS_EVENT_NAME_SIZE > 16)
    OSEventNameSet(FlashQueMutexSem, (u8 *)"FlashQueMutexSem", &os_err);
#endif

		QueMutexSem = OSSemCreate(1);  								// 队列操作互斥信号量
#if (OS_EVENT_NAME_SIZE > 11)
    OSEventNameSet(QueMutexSem, (u8 *)"QueMutexSem", &os_err);
#endif

		ParaMutexSem = OSSemCreate(1);  							// CPU Flash参数读写互斥信号量
#if (OS_EVENT_NAME_SIZE > 12)
    OSEventNameSet(ParaMutexSem, (u8 *)"ParaMutexSem", &os_err);
#endif

		CONComMbox = OSMboxCreate((void *)0);  				// 外设底层接收数据邮箱
#if (OS_EVENT_NAME_SIZE > 10)
    OSEventNameSet(CONComMbox, (u8 *)"CONComMbox", &os_err);
#endif

	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OS层应用
		OS_ACKMbox = OSMboxCreate((void *)0);  		 		// 收到应答协议数据包
#if (OS_EVENT_NAME_SIZE > 10)
    OSEventNameSet(OS_ACKMbox, (u8 *)"OS_ACKMbox", &os_err);
#endif

		OS_DownACKMbox = OSMboxCreate((void *)0);  		 		// 收到应答协议数据包
#if (OS_EVENT_NAME_SIZE > 14)
    OSEventNameSet(OS_DownACKMbox, (u8 *)"OS_DownACKMbox", &os_err);
#endif

		OS_OBDReqMbox = OSMboxCreate((void *)0);  				// 请求OBD处理邮箱
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(OS_OBDReqMbox, (u8 *)"OS_OBDReqMbox", &os_err);
#endif

		OS_OBDAckMbox = OSMboxCreate((void *)0);  				// OBD应答邮箱
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(OS_OBDAckMbox, (u8 *)"OS_OBDAckMbox", &os_err);
#endif

		OS_RecGprsMbox = OSMboxCreate((void *)0);  		// 收到GPRS数据送解析任务
#if (OS_EVENT_NAME_SIZE > 14)
    OSEventNameSet(OS_RecGprsMbox, (u8 *)"OS_RecGprsMbox", &os_err);
#endif

		OS_RecSmsMbox = OSMboxCreate((void *)0);  		// 收到SMS数据送解析任务
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(OS_RecSmsMbox, (u8 *)"OS_RecSmsMbox", &os_err);
#endif

		OS_RecBtMbox = OSMboxCreate((void *)0);  			// 收到BT数据送解析任务
#if (OS_EVENT_NAME_SIZE > 14)
    OSEventNameSet(OS_RecBtMbox, (u8 *)"OS_RecBtMbox", &os_err);
#endif

		OS_CONReqMbox = OSMboxCreate((void *)0);  		// 外设控制请求邮箱
#if (OS_EVENT_NAME_SIZE > 13)
    OSEventNameSet(OS_CONReqMbox, (u8 *)"OS_CONReqMbox", &os_err);
#endif

		OS_CONAckMbox = OSMboxCreate((void *)0);  		// 外设控制应答邮箱
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

		//由于使用UCOS,以下的初始化虽然可以在OS运行之前运行,但注意别使能任何中断.
		//关闭所有中断
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
*                                             【APP Task】
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
		fun_obd_task_create();		// OBD任务创建	
#endif
	
		// 检测系统参数区 //////////////////
		BSP_CPU_FLASH_ProCheck(FLASH_CPU_COMMON_START);
		BSP_CPU_FLASH_ProCheck(FLASH_CPU_SYSCFG_START);
		BSP_CPU_FLASH_ProCheck(FLASH_CPU_SYSOBD_START);

		// 使能RTC ALARM中断	//////////////
		BSP_RTC_IT_CON(DEF_RTC_ALARM_IT,	DEF_ENABLE);

		OSTmr_Count_Start(&iwgCounter);				// 硬件看门狗计数器	
		OSTmr_Count_Start(&rtcCounter);				// RTC计数器			
		OSTmr_Count_Start(&checkCounter);
		OSTmr_Count_Start(&idleCounter);

		pvdCheckTimes=0;
		sleepStep=0;								// 这里需要手动清除变量否则每次进入任务时变量有可能为1或2(初始化为0不起作用)
		
		while (1)																										/* Task body, always written as an infinite loop*/
		{                                          															
				// 看门狗处理 /////////////////////////////////////////////////////////////////////
				#if(OS_SYSWDG_EN > 0)
				if(OSTmr_Count_Get(&iwgCounter) >= OS_FEEDWDG_TIME * 1000)
				{
						IWDG_ReloadCounter();						// 喂狗	
						OSTmr_Count_Start(&iwgCounter);				
				}
				#endif

				// RTC软件刷新处理 ////////////////////////////////////////////////////////////////
				if(OSTmr_Count_Get(&rtcCounter) >= OS_RTC_REFRESH * 1000)
				{
						BSP_RTCSoft_ISRHandler();				// 更新RTC年月日
						OSTmr_Count_Start(&rtcCounter);
				}

				// 软件看门狗各任务超时判断 ///////////////////////////////////////////////////////
				if(SWDGTimeOutCheck() == 1)
				{
						while(1);
				}

				// 睡眠流流程处理 ///////////////////////////////////////////////////////////////////
				#if(WOK_SLEEPMODE_EN > 0)
				// PVD扫描
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
						if(globInitPVDFlag == OS_INITPVD_FLAG)	 		// 上电PVD异常软件复位
						{
								globInitPVDFlag =0;
								HAL_CPU_ResetPro();				
						}	
				}
				
				// 设备刚上电5钟后进入睡眠检查流程(防止系统一上电直接进入睡眠)			
				if(OSTmr_Count_Get(&checkCounter) >= OS_RST_CHECKSLEEP_TIME)		
						sleepCheckProcess (&sleepStep,	&idleCounter);				
				#endif	
				
				// 系统命令导致的复位处理 ///////////////////////////////////////////////////////////////////
				if((s_osTmp.resetReq == DEF_RESET_PRI0) || (s_osTmp.resetReq == DEF_RESET_PRI1) || (s_osTmp.resetReq == DEF_RESET_WAIT))
				{
						rstCount++;
						if(s_osTmp.resetReq == DEF_RESET_PRI0) // 最大延时10秒
						{								
								if((rstCount >= 1000) || 
									 ((HAL_ComQue_size (DEF_SMS_QUE) == 0) && (HAL_ComQue_size(DEF_RAM_QUE) == 0) && (HAL_ComQue_size(DEF_FLASH_QUE) == 0) &&
								    (tmpSms.len == 0) && (s_ram.len == 0) && (s_flash.len == 0)))
								{																					
										rstCount =0;										
										s_osTmp.resetReq =DEF_RESET_WAIT;	
								}
						}	
						else if(s_osTmp.resetReq == DEF_RESET_PRI1)	// 最大延时180秒
						{								
								if((rstCount >= 18000) || 
									 ((HAL_ComQue_size (DEF_SMS_QUE) == 0) && (HAL_ComQue_size(DEF_RAM_QUE) == 0) && (HAL_ComQue_size(DEF_FLASH_QUE) == 0) &&
								    (tmpSms.len == 0) && (s_ram.len == 0) && (s_flash.len == 0)))
								{
										rstCount =0;
										s_osTmp.resetReq =DEF_RESET_WAIT;	
								}
						}	
						else if(s_osTmp.resetReq == DEF_RESET_WAIT)	// 发送完成等待3s后复位(给予GSM模块发送时间)
						{
								if(rstCount >= 300)
								{
										if(s_comPack.packType == 0)
												s_comPack.packType = PACK_TYPE_SHUTNET;		// 发送断网数据包										
										
										HAL_CPU_ResetPro();
								}
						}
						else
								HAL_CPU_ResetPro();	// 非法参数直接复位		
				}
				else
						s_osTmp.resetReq =0;										
				
				// 释放CPU时间 ////////////////////			
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

		// 开启GSM任务软件看门狗  /////////////
		OSTmr_Count_Start(&s_swg.gsmCounter);
	
		while(1)
		{																			  									
				// GSM模块底层时钟处理  /////////// 
				gsmTimer++;
				if(gsmTimer>=100)
				{
						gsmTimer = 0;
						s_GSMcomm.Timer++;
						s_GSMcomm.TimerApp++;
						OSTmr_Count_Start(&s_swg.gsmCounter);		// 清计数器	
				}	
				// GSM收发数据流程 ////////////////	
				BSP_GSM_TransPro();	
				// GSM常规监测 ////////////////////		
				BSP_GSM_CommonPro();	
				// 释放CPU时间 ////////////////////			
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
* Description : GPRS联网及SMS回复操作
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
   
		
		// GSM模块初始化及供电  //////////////
		BSP_GSM_Init();		
		BSP_GSM_POW_RESET(500);  // 上电后重新复位模块电源			

		// 使能各计数器  /////////////////////
		OSTmr_Count_Start(&cNetCounter);
		OSTmr_Count_Start(&gsmMonCounter);
		OSTmr_Count_Start(&simpCounter);
		OSTmr_Count_Start(&netCfgCounter);	
		OSTmr_Count_Start(&imeiCounter);	
		OSTmr_Count_Start(&csqCounter);
		OSTmr_Count_Start(&simWCounter);
	
		// 开启应用IP计时定时器///////////////
		RTCTmr_Count_Get(&s_osTmp.appIPWorkCounter);

		// 开启GPRS任务软件看门狗  ///////////
		OSTmr_Count_Start(&s_swg.gprsCounter);

		while(1)
		{		
				// GSM配置流程 /////////////////////////////////////////////////////////////////
				BSP_GSM_Setup();			

				// SIM卡初始化处理
				if(OSTmr_Count_Get(&simpCounter) >= 1000)
				{
						if(qSimFlag == 0)
						{
								gsmRet =0;
								gsmRet =BSP_GSMSimCardInitPro (s_gsmApp.simNum,	3000);
								if(gsmRet ==	DEF_GSM_NONE_ERR)
								{
										s_osTmp.simErrTimes =0;	// 清SIM卡故障次数
										qSimFlag =1;
								}
								else
								{
										if(gsmRet == DEF_GSM_CPIN_ERR)
										{
												if(s_osTmp.simErrTimes < 0xffff)
														s_osTmp.simErrTimes++;	// SIM故障累加
										}
								}
						}
						OSTmr_Count_Start(&simpCounter);	
				}
				
				// SIM卡号写入处理
				if(OSTmr_Count_Get(&simWCounter) >= 1000)
				{
						if(s_gsmApp.simNumWriteFlag == 1)
						{
								if(BSP_WriteSimCardNumPro (s_gsmApp.simNum,	3000) ==	DEF_GSM_NONE_ERR)	
										s_gsmApp.simNumWriteFlag =0;
						}
						OSTmr_Count_Start(&simWCounter);
				}
				
				// GSM出错监控处理(应用部分) ///////////////////////////////////////////////////
				if(OSTmr_Count_Get(&gsmMonCounter) >= 1000)	
				{
					 	if((BSP_GSM_ERR_Que(&err_Gsm.gsmErrorTimes)  >= OS_GSM_MAXERR_TIME) ||
							 (APP_ErrQue(&s_osTmp.cnnErrTimes) >= 5)	||
							 (APP_ErrQue(&s_osTmp.ackErrTimes) >= 5)  ||
						   (s_osTmp.simErrTimes >= OS_SIM_MAXERR_TIME))
						{
								// 写复位次数变量
								if(APP_ErrQue(&s_err.gsmResetTimes) < 0xff)
								{
										APP_ErrAdd(&s_err.gsmResetTimes);										
								}
								////////////////////////////////////////////////
								BSP_GSM_ERR_Clr(&err_Gsm.gsmErrorTimes);
								APP_ErrClr(&s_osTmp.cnnErrTimes);
								APP_ErrClr(&s_osTmp.ackErrTimes);		// 清应答错误变量
								s_osTmp.simErrTimes =0;							// 清SIM卡故障次数	

								qSimFlag=0;	qImeiFlag=0;	btCfgFlag=0;										
								BSP_GSMHardWareReset(5000);					// 软件复位失败直接执行硬件复位							
						}
						OSTmr_Count_Start(&gsmMonCounter);				
				}
			
				// 取设备IMEI号码 ///////////////////	
				/*
				if(OSTmr_Count_Get(&imeiCounter) >= 10000)
				{	
						if(qImeiFlag == 0)	
						{
								if(BSP_QIMEICodePro(s_gsmApp.IMEI,	1000) ==	DEF_GSM_NONE_ERR)
										qImeiFlag	=1;		// 只读取一次			
						}	
						OSTmr_Count_Start(&imeiCounter);
				}	
				*/

				// 更新信号强度//////////////////////////////////////////////////////////////
				/*
				if(OSTmr_Count_Get(&csqCounter) >= 10000)
				{	
						if(s_common.iapSta != DEF_UPDATE_KEYWORD)		// 升级过程中不查询信号强度
								BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	
						OSTmr_Count_Start(&csqCounter);	
				}
				*/
							        
        // 睡眠前GSM模块进入低功耗模式操作(只处理睡眠过程中的断网操作)///////////////
        if(s_osTmp.gsmConSleep == 1)
        {
        		// 模块断开GPRS网络
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
												break;	// 退出睡眠模式										
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
						// 模块进入睡眠模式
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
										// 模块故障将关闭GSM模块电源
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
								s_osTmp.gsmConSleep  =2;		// 无论成功与否都直接返回睡眠结束
								OSTimeDly(3000);						// 睡眠成功后需要挂起设备3秒防止设备唤醒后直接进行联网操作	
						}
						else
						{
								s_osTmp.gsmConSleep =0;			// 设置退出睡眠模式判断
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[LOWPOW]-> Break sleep mode!\r\n");
								#endif
						}
        }
				
				// 初始化蓝牙模块(配置蓝牙设名称)///////////////////////////////////////////// 
				if(s_cfg.devEn & DEF_DEVEN_BIT_BT)
				{
						if(s_common.iapSta != DEF_UPDATE_KEYWORD)	// 升级过程中不尝试初始化蓝牙
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
																btCfgFlag =1;									// 初始化成功后将不再进入
																#if(DEF_BTINFO_OUTPUTEN > 0)							
																if(dbgInfoSwt & DBG_INFO_BT)
																		myPrintf("[BT]: Init OK!\r\n");
																#endif
														}
														else
														{								
																//#if(DEF_BTINFO_OUTPUTEN > 0)	// 初始化失败不做输出						
																//if(dbgInfoSwt & DBG_INFO_BT)
																//		myPrintf("[BT]: Init fail!\r\n");
																//#endif					
														}
														OSTmr_Count_Start(&btInitCounter);
												}										
										}
										else
												btCfgFlag =1;	// 超过一定次数后将不再尝试初始化蓝牙
								}
						}
				}
				
				// TTS实时语音播报处理///////////////////////////////////////////// 
				if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
				{
						if(s_conDev1.dataLock == 1)
						{
								if((s_conDev1.type == TTS_CMD) && (s_conDev1.paraLen != 0x00))
								{
										if(BSP_TTSPlayPro(0,	s_conDev1.paraBuf, s_conDev1.paraLen,	3000) ==DEF_GSM_NONE_ERR)
												memset((u8 *)&s_conDev1,	0,	sizeof(s_conDev1));	// 播报完成清结构变量(同时数据解锁)
										else
												OSTimeDly(1000);	// 播放失败等待1s后重试
								}
						}
						else if(s_conDev2.dataLock == 1)
						{
								if((s_conDev2.type == TTS_CMD) && (s_conDev2.paraLen != 0x00))
								{
										if(BSP_TTSPlayPro(0,	s_conDev2.paraBuf, s_conDev2.paraLen,	3000) ==DEF_GSM_NONE_ERR)
												memset((u8 *)&s_conDev2,	0,	sizeof(s_conDev2));	// 播报完成清结构变量(同时数据解锁)
										else
												OSTimeDly(1000);	// 播放失败等待1s后重试
								}
						}
				}
								
				// GSM自动联网 //////////////////////////////////////////////////////////
				// 登录服务器条件：1)设置了GPRS使能;2)设备处于断网状态;3)无电话及短信操作;
				if((s_wakeUp.state & DEF_WAKEUP_RING) || (s_wakeUp.state & DEF_WAKEUP_IG))	
				{
						// 2015-5-30 只有电话或短信及IG唤醒才延迟联网
						/*
						if(s_wakeUp.state & DEF_WAKEUP_RING)
								s_wakeUp.state	&= ~DEF_WAKEUP_RING;
						else if(s_wakeUp.state & DEF_WAKEUP_IG)
								s_wakeUp.state	&= ~DEF_WAKEUP_IG;
						*/						
						s_wakeUp.state =0;	// 清唤醒源标志
						baseGTime	=	5000;		// 只要设备从睡眠中唤醒就延迟5秒拨号(给与TTS语音播报或短信查询处理时间)				
						OSTmr_Count_Start(&cNetCounter);
				}			
				if(OSTmr_Count_Get(&cNetCounter) >= baseGTime)	
				{						
						if((qSimFlag == 1)                            &&	// 需要等待SIM卡初始化完毕
							 (s_cfg.devEn & DEF_DEVEN_BIT_GPRS) 				&& 
					 		 (s_GSMcomm.GSMComStatus	==	GSM_ONCMD)		&&
					 		 (BSP_GSM_GetFlag(DEF_CALLDOING_FLAG) == 0) && 
					 		 (BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0))
						{	
								// 拨号前查询下当前信号强度
								BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);
								// 正常拨号流程 //////////////////////////////////////////////////////////////
								gsmRet =DEF_GSM_NONE_ERR;
								gsmRet =BSP_GPRSResetPro(s_cfg.apn,	s_cfg.apnUser,	s_cfg.apnPass,	5000);
								if(gsmRet == DEF_GSM_NONE_ERR)
								{
										gsmRet =DEF_GSM_NONE_ERR;									
										#if(OS_NAVIREQ_EN > 0)	
										if(s_osTmp.naviStep == 0)
										{
												gsmRet = BSP_TCPConnectPro(DEF_GPRS_DATA_CNN0,	s_cfg.ip1, s_cfg.port1,	5000);		// 连接导航IP
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
												gsmRet = BSP_TCPConnectPro(DEF_GPRS_DATA_CNN0,	s_cfg.ip2, s_cfg.port2,	5000);		// 连接应用IP													
										#else
												gsmRet = BSP_TCPConnectPro(DEF_GPRS_DATA_CNN0,	s_cfg.ip1, s_cfg.port1,	5000);		// 连接导航IP
										#endif
										}
								}
								// 失败处理 //////////////////////////////////////////////////////////////
								if(gsmRet == DEF_GSM_NONE_ERR)
								{		
										#if(DEF_GPRSINFO_OUTPUTEN > 0)							
										if(dbgInfoSwt & DBG_INFO_GPRS)
										{		
												BSP_RTC_Get_Current(&tmpRtc);	// 获取当前RTC时间
												myPrintf("\r\n[%02d/%02d/%02d %02d:%02d:%02d][GPRS-Cnn]: Success!\r\n",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
										}
										#endif
										baseGTime = 2000;										// 成功后2秒查询一次是否断网
										APP_ErrClr(&s_osTmp.ackErrTimes);		// 拨号成功后清应答次数
										APP_ErrClr(&s_osTmp.cnnErrTimes);		// 清联网成功次数  
										s_osTmp.appIPCnnErrTimes=0;					// 联网成功清连续联网失败次数
								}
								else
								{	
										/*
										#if(DEF_GPRSINFO_OUTPUTEN > 0)							
										if(dbgInfoSwt & DBG_INFO_GPRS)
										{											
												BSP_RTC_Get_Current(&tmpRtc);	// 获取当前RTC时间
												myPrintf("\r\n[%02d/%02d/%02d %02d:%02d:%02d][GPRS-Cnn]: Fail!\r\n",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
										}
										#endif
										*/
										if((gsmRet == DEF_GSM_CREG_ERR) || (gsmRet == DEF_GSM_CPIN_ERR))
										{
												if(gsmRet == DEF_GSM_CPIN_ERR)
														qSimFlag =0;		// SIM卡在工作中被移除则触发SIM卡重新初始化						
												baseGTime = 1000;										// GSM网络注册错误或无卡1s后重试
										}
										else 
										{														
												// 模块联网故障次数统计
												if((gsmRet == DEF_GSM_CIFSR_ERR) || (gsmRet == DEF_GSM_CIPSTART_ERR)) 
												{
														if(BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	2000) == DEF_GSM_NONE_ERR)
														{												
																if(s_gsmApp.sLeve >= 20)
																		APP_ErrAdd(&s_osTmp.cnnErrTimes);	//	拨号失败次数累加到一定次数硬件复位模块
														}
												}			
												// 应用IP错误次数统计
												if(gsmRet == DEF_GSM_CIPSTART_ERR)
												{
														if(s_osTmp.appIPCnnErrTimes < 0xff)
																s_osTmp.appIPCnnErrTimes ++;												
												}
												baseGTime = 5000;			// 失败后5秒尝试下一次拨号
										}
								}
						}
						else
								baseGTime = 1000;						// 条件不满足1秒检查一次拨号条件

						OSTmr_Count_Start(&cNetCounter);
				}			
				
				// 清软件计数器 ////////////////////							
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

		// 开启任务软件看门狗  /////////////
		OSTmr_Count_Start(&s_swg.rxdCounter);
		
		while(1)
		{			
				// GPRS数据处理
				if(OSSemAccept(GSMGprsDataSem) > 0) 
				{		
						while(1)
						{
								OSTmr_Count_Start(&s_swg.rxdCounter);		// 清空软件计数器													
								if(((s_common.iapSta != DEF_UPDATE_KEYWORD) && (s_osTmp.rxdDataLock == 0)) || 
									 ((s_common.iapSta == DEF_UPDATE_KEYWORD) && (s_update.writeLock == 0))) 		// 如果升级需要判断数据锁
								{
										memset((u8 *)&s_gprs,	'\0',	sizeof(s_gprs));
										if(s_common.iapSta != DEF_UPDATE_KEYWORD)
												ret =BSP_TCPRxdData(DEF_GPRS_DATA_CNN0,	(u8 *)(&s_gprs.dataBuf),	&s_gprs.dataLen,	40000);
										else
												ret =BSP_TCPRxdDataUpdata(DEF_GPRS_DATA_CNN0,	(u8 *)(&s_gprs.dataBuf),	&s_gprs.dataLen,	10000); 		
										
										if((ret == DEF_GSM_NONE_ERR) || (ret == DEF_GSM_READ_AGAIN))
										{											
												// 打印GPRS收到的数据
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
												{				
														if(s_common.iapSta != DEF_UPDATE_KEYWORD)	// 升级过程中不打印RTC时间戳
																BSP_RTC_Get_Current(&tmpRtc);	// 获取当前RTC时间
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
												s_update.writeLock =1;	// 数据上锁
												s_osTmp.rxdDataLock	=1;	// 非升级模式数据锁
												OSMboxPost(OS_RecGprsMbox, (void *)&s_gprs);		// 发送邮箱开始解析数据	
												
												// 判断是否需要重读
												if(ret == DEF_GSM_NONE_ERR)
														break;
												else if(ret == DEF_GSM_READ_AGAIN)
												{
														// 还需要再读取一次
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Rxd]: need read again!!\r\n");
														#endif		
												}												
										}	
										else
										{
												// 错误直接返回
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("\r\n[GPRS-Rxd]: read error %-d!!\r\n",ret);
												#endif
												break;
										}	
								}
								else
										OSTimeDly(10);	// 等待
						}
				}
				// BT数据处理
				bMsg = (BTMbox_Typedef *)(OSMboxAccept(GSMBtDataMbox));
				if(bMsg != (void *)0)
				{
						if(s_common.iapSta != DEF_UPDATE_KEYWORD)		// 升级过程中不处理蓝牙数据
						{
								OSTmr_Count_Start(&s_swg.rxdCounter);		// 清空软件计数器						
								memset((u8 *)&s_bt,	'\0',	sizeof(s_bt));							
								if(bMsg->func == BT_RXDATA)	// 数据接收处理
								{
										if(BSP_BTRxdDataPro((u8 *)(&s_bt.dataBuf),	&s_bt.dataLen,	6000) == DEF_GSM_NONE_ERR)
										{
												// 打印BT收到的数据
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
												OSMboxPost(OS_RecBtMbox, (void *)&s_bt);		// 发送邮箱开始解析数据	
										}
								}
						}
				}
				// SMS短信处理
				if(OSSemAccept(GSMSmsSem) > 0) 
				{
						if(s_common.iapSta != DEF_UPDATE_KEYWORD)		// 升级过程中不读取短信
						{
								OSTimeDly(1000);	// 避免设备从睡眠到唤醒未准备好

								smsIndex  =1;						
								while(smsIndex < 20)
								{						
										OSTmr_Count_Start(&s_swg.rxdCounter);		// 清空软件计数器						
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

												OSMboxPost(OS_RecSmsMbox, (void *)&s_sms);		// 发送邮箱开始解析数据		
												// 删除短信
												BSP_SMSDelete(smsHbuf,	8000);
										}
										else if(ret == DEF_GSM_SMS_ENPTY)
										{
												// 短信存储区已经为空
												break;
										}	
										smsIndex++;						
								}
						}
						// 释放标志
						BSP_GSM_ReleaseFlag(DEF_SMSDOING_FLAG);
				}
				
				// 清软件计数器 ////////////////////			
				OSTmr_Count_Start(&s_swg.rxdCounter);		// 清空软件计数器	
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
		vu32	heartBCounter=0;	// 心跳包发送计时器			
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
		
		
		// 开启任务软件看门狗  /////////////
		OSTmr_Count_Start(&s_swg.sendCounter);
		OSTmr_Count_Start(&heartBCounter);

		while(1)
		{
				// 升级任务挂起等待
				while(s_common.iapSta == DEF_UPDATE_KEYWORD)
				{
						OSTmr_Count_Start(&s_swg.sendCounter);	// 清软件计数器
						OSTimeDly(10000);	
				}

				// 短信数据发送 ///////////////////////////////////////////
				if(HAL_ComQue_size (DEF_SMS_QUE) > 0)
				{
						memset((u8 *)&tmpSms,		0,	sizeof(tmpSms));
						if(HAL_ComQue_out(DEF_SMS_QUE,	&tmpSms) == DEF_PROQUE_QUE_OK)
						{
								if((tmpSms.path == DEF_PATH_SMS) && (tmpSms.len != 0) && (tmpSms.len <= DEF_SMS_GSMBYTE_SIZE) &&
									 (strlen((const char *)tmpSms.smsNum) >= 5) && (tmpSms.smsNum[0] != '\0'))
								{
										// 发送短信息
										// SMS发送数据处理
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
												memset((u8 *)&tmpSms,		0,	sizeof(tmpSms));	// 清短信临时结构变量防止外部判断有误
												HAL_ComQue_outend (DEF_SMS_QUE);	// 数据出队	
                    }
										else										
												OSTimeDly(1000);  //防止过度频繁操作模块	
								}	
								else
								{
										memset((u8 *)&tmpSms,		0,	sizeof(tmpSms));	// 清短信临时结构变量防止外部判断有误
										HAL_ComQue_outend (DEF_SMS_QUE);	// 非法数据直接丢掉
								}
						}
				}
				// IP导航流程 /////////////////////////////////////////////
				#if(OS_NAVIREQ_EN > 0)
				if(s_osTmp.naviStep == 0)
				{
						// 系统还未获取应用IP
						if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
						{	
								if(s_login.len == 0)
								{
										// 系统还未打包应用IP请求数据包
										memset((u8 *)&s_login,	0,	sizeof(s_login));										
										HAL_BuildNavi_Process(s_login.buf,	&s_login.len);	// 组包
								}
								if(s_login.len != 0)
								{
										BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// 发送数据前更新下当前信号强度
										ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// 清空通用应答邮箱
										ackMsg = (void *)0;
										if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_login.buf,	s_login.len,	3000) == DEF_GSM_NONE_ERR)
										{
												// 打印GPRS发送的数据
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														sendPrintProcess (0,	s_login.buf,	s_login.len);
												#endif
												OSTmr_Count_Start(&heartBCounter);	// 只要单一方向发送成功就复位心跳包发送定时器
												// 应用IP获取数据必须有应答
												
												// 2015-1-17 by:gaofei 修改为扫描方式接收方式
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
																				OSSemPost(GSMGprsDataSem);		// 手动查询是否已经接收到数据			
																}
														}													
														OSTimeDly(100);				// 扫描间隔										
												}
												if(ackMsg != (void *)0)
												{
														APP_ErrClr(&s_osTmp.ackErrTimes);		// 只有连续的无应答才计数		
														if((ackMsg->msgId == DEF_CMDID_NAVI_PLA) && (ackMsg->result == DEF_ACKTYPE_OK))
														{
																s_osTmp.naviStep =1;
																RTCTmr_Count_Start(&s_osTmp.appIPWorkCounter); // 开始应用IP超时统计
																BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
																//if(BSP_TCPCloseNetPro	(DEF_GPRS_DATA_CNN0,	3000) == DEF_GSM_NONE_ERR)	// 断开当前网络连接																
																	
																memset((u8 *)&s_login,	0,	sizeof(s_login));	// 清发送结构变量
																OSTimeDly(500);				// 操作延时
														}		
														else
																OSTimeDly(500); 		// 错误的应答数据
												}
												else
												{													
														APP_ErrAdd(&s_osTmp.ackErrTimes);						//	应答超时累计处理
														if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
														{
																APP_ErrClr(&s_osTmp.ackErrTimes);
																BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
														}
														OSTimeDly(500); 		// 服务器应答超时
												}
										}
										else
												OSTimeDly(500); 		// 发送失败							
								}
						}
						else
						{
								s_login.len =0;			// 2015-7-10 by:gaofei 防止结构未清除导致向导航IP中发送握手报文问题
								OSTimeDly(500); 		// 设备未在网
						}
				}				
				// 正常发数据流程 /////////////////////////////////////////////
				else if(s_osTmp.naviStep == 1)
				#else
				if(1)
				#endif
				{	
						#if(OS_NAVIREQ_EN > 0)						
						// 应用IP失效处理////////////////////////////////////////
						if((RTCTmr_Count_Get(&s_osTmp.appIPWorkCounter) >= OS_APPIP_VALID_TIME) ||
							 (s_osTmp.appIPCnnErrTimes >= OS_APPIP_CNNERROR_TIMES))
						{
								if((s_common.iapSta != DEF_UPDATE_WAIT) && (s_common.iapSta != DEF_UPDATE_KEYWORD))	// 升级过程不做IP失效处理
								{
										s_osTmp.naviStep =0;	// 应用IP失效
										BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
										memset((u8 *)&s_login,	0,	sizeof(s_login));	// 2015-7-10 by:gaofei 防止结构未清除导致向导航IP中发送握手报文问题
										if(s_osTmp.appIPCnnErrTimes >= OS_APPIP_CNNERROR_TIMES)
												s_osTmp.appIPCnnErrTimes =0;		
								}
						}												
						// 正常数据发送处理//////////////////////////////////////
						else
						#else
						if(1)
						#endif
						{
								if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
								{
										s_login.handShake	=0;	// 设备未联网复位状态变量
								}
								else
								{
										// 握手数据发送 /////////////////////////////////////////////
										if(s_login.handShake == 0)
										{
												// 组包
												memset((u8 *)&s_login,	0,	sizeof(s_login));	
												HAL_BulidLogin_Process (s_login.buf,	&s_login.len,	(u16 *)&s_login.sn);
												if(s_login.len > DEF_LOGINSENDBUF_SIZE)
														memset((u8 *)&s_login,	0,	sizeof(s_login));	// 非法数据直接丢掉
												else
														s_login.handShake =1;	// 进入发送环节
										}
										if(s_login.handShake == 1)
										{
												if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
												{
														BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// 发送数据前更新下当前信号强度
														ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// 清空通用应答邮箱
														ackMsg = (void *)0;
														if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_login.buf,	s_login.len,	3000) == DEF_GSM_NONE_ERR)
														{
																// 打印GPSR发送的数据
																#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																if(dbgInfoSwt & DBG_INFO_GPRS)
																		sendPrintProcess (0,	s_login.buf,	s_login.len);
																#endif
																OSTmr_Count_Start(&heartBCounter);	// 只要单一方向发送成功就复位心跳包定时器
																// 2015-1-17 by:gaofei 修改为扫描方式接收方式
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
																								OSSemPost(GSMGprsDataSem);		// 手动查询是否已经接收到数据			
																				}												
																		}													
																		OSTimeDly(100);				// 扫描间隔										
																}
																if(ackMsg != (void *)0)
																{
																		APP_ErrClr(&s_osTmp.ackErrTimes);		// 清应答错误次数	
																		if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_login.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																		{
																				s_login.len 	=0;
																				s_login.handShake =2;		// 握手成功进入正式发送流程																
																				OSTimeDly(500);		
																		}		
																		else
																				OSTimeDly(500); 	
																}
																else
																{
																		APP_ErrAdd(&s_osTmp.ackErrTimes);						// 应答错误累计
																		if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																		{
																				APP_ErrClr(&s_osTmp.ackErrTimes);
																				BSP_GPRSFroceBreakSet();	// 强制断网
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
								// 已经握手成功发送应用数据 /////////////////////////////////////////////
								if(s_login.handShake == 2)
								{
										// 通用应答数据发送处理/////////////////////////////////////////
										if((comAck.len != 0) || (s_comAck.len	!= 0))
										{
												if(comAck.len != 0)
												{
														memset((u8 *)&s_comAck,	0,	sizeof(s_comAck));										
														HAL_PackComAckProcess(comAck.data,	comAck.len,	s_comAck.buf,	&s_comAck.len);
														memset((u8 *)&comAck,	0,	sizeof(comAck));	// 打包完成清除通用应答临时存储区		
														if(s_comAck.len > DEF_COMSENDBUF_SIZE)
																memset((u8 *)&s_comAck,	0,	sizeof(s_comAck));	// 如果数据非法直接丢掉																						
												}		
												// 发送处理///////////////////////////////////////////////		
												if(s_comAck.len	!= 0)
												{
														// GPRS数据发送
														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
																memset((u8 *)&s_comAck,	0,	sizeof(s_comAck));	// 如果设备不在线直接删掉通用应答												
														else
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// 发送数据前更新下当前信号强度
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_comAck.buf,	s_comAck.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// 打印GPRS发送的数据
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_comAck.buf,	s_comAck.len);
																		#endif																		
																		memset((u8 *)&s_comAck,	0,	sizeof(s_comAck));	// 发送完成清通用应答发送缓冲区																												
																		OSTimeDly(200);				// 操作延时	
																}
																else
																		OSTimeDly(500); 		// 发送失败		
														}
												}
										}
										// RAM实时数据发送处理/////////////////////////////////////////
										else if((s_ram.len != 0) || (HAL_ComQue_size (DEF_RAM_QUE) > 0))
										{
												if(s_ram.len == 0)
												{
														if(HAL_ComQue_size (DEF_RAM_QUE) > 0)
														{
																memset((u8 *)&s_ram,	0,	sizeof(s_ram));
																HAL_PackRealTimeDataProcess	(&s_ram.type,	s_ram.buf,	&s_ram.len, (u16 *)&s_ram.sn);	
																if(s_ram.len > DEF_RAMSENDBUF_SIZE)
																		memset((u8 *)&s_ram,	0,	sizeof(s_ram));	// 如果数据非法直接丢掉
																s_ram.ackType =DEF_ACKTYPE_ACK;					// 需要应答
														}
												}
												// 发送处理///////////////////////////////////////////////
												if(s_ram.len != 0)
												{
														// GPRS数据发送
														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// 发送数据前更新下当前信号强度
																ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// 清空通用应答邮箱
																ackMsg = (void *)0;
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_ram.buf,	s_ram.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// 打印GPRS发送的数据
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_ram.buf,	s_ram.len);
																		#endif
																		OSTmr_Count_Start(&heartBCounter);	// 只要单一方向发送成功就复位心跳包发送定时器
																		if(s_ram.ackType == DEF_ACKTYPE_NOACK)
																		{
																				memset((u8 *)&s_ram,	0,	sizeof(s_ram));	// 清发送缓冲防止再次发送
																				OSTimeDly(200);				// 无需应答的发送延时,避免发送过快
																		}
																		else
																		{
																				// 2015-1-17 by:gaofei 修改为扫描方式接收方式
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
																												OSSemPost(GSMGprsDataSem);		// 手动查询是否已经接收到数据			
																								}												
																						}													
																						OSTimeDly(100);				// 扫描间隔										
																				}
																				if(ackMsg != (void *)0) 
																				{																
																						APP_ErrClr(&s_osTmp.ackErrTimes);			// 只有连续的无应答才计数		
																						if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_ram.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																						{
																								memset((u8 *)&s_ram,	0,	sizeof(s_ram));	// 清发送缓冲														
																								OSTimeDly(300);				// 操作延时
																						}		
																						else
																								OSTimeDly(500); 		// 错误的应答数据
																				}
																				else
																				{
																						APP_ErrAdd(&s_osTmp.ackErrTimes);						//	应答超时累计处理													
																						if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																						{
																								APP_ErrClr(&s_osTmp.ackErrTimes);
																								BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
																						}
																						OSTimeDly(500); 		// 服务器应答超时
																				}
																		}	
																}
																else
																		OSTimeDly(500); 		// 发送失败		
														}
														else
																OSTimeDly(500); 		// 设备未在网
												}	
										}
										// FLASH批量打包数据发送处理/////////////////////////////////////////
										else if((s_flash.len != 0) || (HAL_ComQue_size (DEF_FLASH_QUE) > 0))
										{
												if(s_flash.len == 0)
												{
														if(HAL_ComQue_size (DEF_FLASH_QUE) > 0)
														{
																if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
																{
																		// 只有设备在网才取数据防止FLASH数据取出后设备复位或异常下电导致数据丢失!!
																		memset((u8 *)&s_flash,	0,	sizeof(s_flash));																		
																		if(HAL_PackFlashDataProcess(DEF_FLASHSENDBUF_SIZE,	s_flash.buf,	&s_flash.len, (u16 *)&s_flash.sn,	&s_flash.queLastOut,	&s_flash.queOutNum) != 0)
																				s_flash.queOutNum =0;	// 数据保护防止数据丢失
																		else
																		{
																				if(s_flash.len > DEF_FLASHSENDBUF_SIZE)
																						memset((u8 *)&s_flash,	0,	sizeof(s_flash));	// 如果数据非法直接丢掉
																		}
																		s_flash.ackType =DEF_ACKTYPE_ACK;			// FLASH打包数据都需要应答
																}
														}				
												}
												// 发送处理///////////////////////////////////////////////
												if(s_flash.len != 0)
												{
														// GPRS数据发送
														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// 发送数据前更新下当前信号强度
																ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// 清空通用应答邮箱
																ackMsg = (void *)0;
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_flash.buf,	s_flash.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// 打印GPRS发送的数据
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_flash.buf,	s_flash.len);
																		#endif
																		OSTmr_Count_Start(&heartBCounter);	// 只要单一方向发送成功就复位心跳包发送定时器
																		if(s_flash.ackType == DEF_ACKTYPE_NOACK)
																		{
																				HAL_ComQue_outFinish (DEF_FLASH_QUE,	s_flash.queLastOut,	s_flash.queOutNum);	// 保存最终状态
																				memset((u8 *)&s_flash,	0,	sizeof(s_flash));	// 清发送缓冲防止再次发送											
																				OSTimeDly(200);				// 无需应答的发送延时,避免发送过快
																		}
																		else
																		{
																				// 2015-1-17 by:gaofei 修改为扫描方式接收方式
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
																												OSSemPost(GSMGprsDataSem);		// 手动查询是否已经接收到数据			
																								}													
																						}													
																						OSTimeDly(100);				// 扫描间隔										
																				}
																				if(ackMsg != (void *)0) 
																				{																
																						APP_ErrClr(&s_osTmp.ackErrTimes);			// 只有连续的无应答才计数		
																						if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_flash.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																						{
																								HAL_ComQue_outFinish (DEF_FLASH_QUE,	s_flash.queLastOut,	s_flash.queOutNum);	// 保存最终状态
																								memset((u8 *)&s_flash,	0,	sizeof(s_flash));	// 清发送缓冲										
																								OSTimeDly(300);				// 操作延时
																						}		
																						else
																								OSTimeDly(500); 		// 错误的应答数据
																				}
																				else
																				{
																						APP_ErrAdd(&s_osTmp.ackErrTimes);						//	应答超时累计处理
																						if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																						{
																								APP_ErrClr(&s_osTmp.ackErrTimes);
																								BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
																						}
																						OSTimeDly(500); 		// 服务器应答超时
																				}
																		}
																}
																else
																		OSTimeDly(500); 		// 发送失败		
														}
														else
																OSTimeDly(500); 		// 设备未在网
												}	
										}
										// OBD实时数据发送处理/////////////////////////////////////////
										else if(s_obd.obdSendFlag == 1)
										{
												if(s_obd.len > DEF_OBDSENDBUF_SIZE)
														memset((u8 *)&s_obd,	0,	sizeof(s_obd));	// 如果数据非法直接丢掉
												else
												{
														// 发送处理///////////////////////////////////////////////
														if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// 发送数据前更新下当前信号强度
																ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// 清空通用应答邮箱
																ackMsg = (void *)0;
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_obd.buf,	s_obd.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// 判断当前发送数据类型
																		if((s_obd.buf[6] == 0x02) && (s_obd.buf[7] == 0x94))
																				s_osTmp.tjSendFlag =0x55;	// 2015-7-23 为支持VIN所做修改，设置体检数据发送标志
																		else
																				s_osTmp.tjSendFlag =0;
																
																		// 打印GPRS发送的数据
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_obd.buf,	s_obd.len);
																		#endif
																		OSTmr_Count_Start(&heartBCounter);	// 只要单一方向发送成功就复位心跳包发送定时器
																		if(s_obd.ackType == DEF_ACKTYPE_NOACK)
																		{
																				memset((u8 *)&s_obd,	0,	sizeof(s_obd));	// 清发送缓冲防止再次发送
																				OSTimeDly(200);				// 无需应答的发送延时,避免发送过快
																		}
																		else
																		{
																				// 2015-1-17 by:gaofei 修改为扫描方式接收方式
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
																												OSSemPost(GSMGprsDataSem);		// 手动查询是否已经接收到数据			
																								}													
																						}													
																						OSTimeDly(100);				// 扫描间隔										
																				}	
																				if(ackMsg != (void *)0) 
																				{																
																						APP_ErrClr(&s_osTmp.ackErrTimes);			// 只有连续的无应答才计数		
																						if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_obd.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																						{																						
																								// 判断是否需要发送打包VIN
																								if(s_osTmp.tjSendFlag == 0x55)
																								{
																										s_osTmp.tjSendFlag =0;
																										HAL_SendOBDProProcess(CMD_READ_VIN,	0,	0,	(u8 *)"",	0);	// 2015-7-23 支持VIN功能所做修改
																								}
																								memset((u8 *)&s_obd,	0,	sizeof(s_obd));	// 清发送缓冲			
																								OSTimeDly(300);				// 操作延时
																						}		
																						else
																								OSTimeDly(500); 		// 错误的应答数据
																				}
																				else
																				{
																						APP_ErrAdd(&s_osTmp.ackErrTimes);						//	应答超时累计处理
																						if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																						{
																								APP_ErrClr(&s_osTmp.ackErrTimes);
																								BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
																						}
																						OSTimeDly(500); 		// 服务器应答超时
																				}
																		}	
																}
																else
																		OSTimeDly(500); 		// 发送失败		
														}
														else
																OSTimeDly(500); 		// 设备未在网
												}
										}																							
										
										// 断网(睡眠)数据发送处理/////////////////////////////////////////
										else if((s_comPack.packType == PACK_TYPE_SHUTNET) || (s_comPack.packType == PACK_TYPE_SLEEP))
										{														
												// GPRS数据发送
												if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
												{
														memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// 如果设备断网则直接放弃发送
														OSTimeDly(500); 		// 设备未在网
												}
												else
												{
														// 打包断网数据包
														if(s_comPack.packType == PACK_TYPE_SHUTNET)
																HAL_BulidComPack_Process (DEF_CMDID_NETBRAKREQ_DEV,	s_comPack.buf,	&s_comPack.len, (u16 *)&s_comPack.sn);
														
														// 打包睡眠数据包
														else if(s_comPack.packType == PACK_TYPE_SLEEP)
																HAL_BulidComPack_Process (DEF_CMDID_SLEEPREQ_DEV,	s_comPack.buf,	&s_comPack.len, (u16 *)&s_comPack.sn);
													
														if((s_comPack.len == 0) || (s_comPack.len > DEF_COMSENDBUF_SIZE))
																memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// 非法数据直接丢掉
														else
														{
																BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// 发送数据前更新下当前信号强度
																if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_comPack.buf,	s_comPack.len,	3000) == DEF_GSM_NONE_ERR)
																{
																		// 打印GPRS发送的数据
																		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				sendPrintProcess (0,	s_comPack.buf,	s_comPack.len);
																		#endif
																		OSTmr_Count_Start(&heartBCounter);	// 只要单一方向发送成功就复位心跳包发送定时器																			
																}
																memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// 不管发送成功与否都只尝试一次
																OSTimeDly(300);		
														}
												}	
										}
										else
										{												
												// 心跳包数据发送处理/////////////////////////////////////////
												#if(OS_HEARTBEAT_EN > 0)										
												if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
														OSTmr_Count_Start(&heartBCounter);	// 如果断网复位心跳包发送定时器			
												else
												{
														if(s_common.iapSta == DEF_UPDATE_KEYWORD)
																OSTmr_Count_Start(&heartBCounter);	// 升级过程中不发心跳包
														if(OSTmr_Count_Get(&heartBCounter) >= (s_cfg.heartbeatTime * 1000))
														{												
																memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));
																s_comPack.packType = PACK_TYPE_HEART;	// 置心跳包数据类型
																HAL_BulidComPack_Process (DEF_CMDID_HEARD_DEV,	s_comPack.buf,	&s_comPack.len, (u16 *)&s_comPack.sn);
																if((s_comPack.len == 0) || (s_comPack.len > DEF_COMSENDBUF_SIZE))
																		memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// 非法数据直接丢掉
																else
																{
																		BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000);	// 发送数据前更新下当前信号强度
																		ackMsg = (AckData_TypeDef *)(OSMboxAccept(OS_ACKMbox));	// 清空通用应答邮箱
																		ackMsg = (void *)0;
																		if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	s_comPack.buf,	s_comPack.len,	3000) == DEF_GSM_NONE_ERR)
																		{
																				// 打印GPRS发送的数据
																				#if(DEF_GPRSINFO_OUTPUTEN > 0)							
																				if(dbgInfoSwt & DBG_INFO_GPRS)
																						sendPrintProcess (0,	s_comPack.buf,	s_comPack.len);
																				#endif
																				OSTmr_Count_Start(&heartBCounter);	// 复位心跳包发送定时器
																				// 2015-1-17 by:gaofei 修改为扫描方式接收方式
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
																												OSSemPost(GSMGprsDataSem);		// 手动查询是否已经接收到数据			
																								}											
																						}													
																						OSTimeDly(100);				// 扫描间隔										
																				}
																				if(ackMsg != (void *)0)
																				{																				
																						APP_ErrClr(&s_osTmp.ackErrTimes);			// 只有连续的无应答才计数		
																						if((ackMsg->msgId == DEF_CMDID_COMACK_PLA) && (ackMsg->ackSn == s_comPack.sn) && (ackMsg->result == DEF_ACKTYPE_OK))
																						{
																								memset((u8 *)&s_comPack,	0,	sizeof(s_comPack));	// 清发送缓冲	
																								OSTimeDly(300);				// 操作延时
																						}		
																						else
																								OSTimeDly(500); 		// 错误的应答数据
																				}
																				else
																				{
																						APP_ErrAdd(&s_osTmp.ackErrTimes);						//	应答超时累计处理
																						if(APP_ErrQue(&s_osTmp.ackErrTimes) >= OS_HEART_ACK_ERR_TIMES)
																						{
																								APP_ErrClr(&s_osTmp.ackErrTimes);
																								BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
																						}
																						OSTimeDly(500); 		// 服务器应答超时
																				}
																		}
																		else
																				OSTimeDly(500); 		// 发送失败		
																}
														}
												}						
												#endif													
										}
								}
								else
										s_login.handShake =0;	// 非法数值复位到未登陆前状态
						}
				}
				else
				{
						s_osTmp.naviStep =0;	// 参数非法设置重新获取IP地址
				}				

				// 清软件计数器 ////////////////////			
				OSTmr_Count_Start(&s_swg.sendCounter);	// 清软件计数器
				OSTimeDly(50);				
		}
}
/*
*********************************************************************************************************
*                                            App_TaskPROTOCOL()
*
* Description : 处理协议数据
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
	
		// 短信控制结构清空
		memset((u8 *)&conSms,	0,	sizeof(conSms));

		// 开启任务软件看门狗  /////////////
		OSTmr_Count_Start(&s_swg.protocolCounter);

		while (1) 
		{		
				// 协议处理(GPRS部分)/////////////////////////////////////////////////////////////	
				gMsg = (GPRS_Typedef *)OSMboxAccept(OS_RecGprsMbox);
				if(gMsg != (void *)0)
				{
						HAL_ProUnpackMain	(gMsg->dataBuf,	gMsg->dataLen);
						s_osTmp.rxdDataLock =0;	// 数据解锁
				}
				
				// 协议处理(BT部分)/////////////////////////////////////////////////////////////	
				bMsg = (BT_Typedef *)OSMboxAccept(OS_RecBtMbox);
				if(bMsg != (void *)0)
				{
//						// TEST 将收到的数据发回到发送端
//						if(BSP_BTSendDataPro	(bMsg->dataBuf,	bMsg->dataLen,	3000) == DEF_GSM_NONE_ERR)
//						{
//								// 打印BT发送的数据
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

				// 协议处理(SMS部分)
				sMsg = (SMS_Typedef *)OSMboxAccept(OS_RecSmsMbox);
				if(sMsg != (void *)0)
				{						
						// 短信协议处理			
						if(conSms.dataLock == 0)
						{
								memset((u8 *)&conSms,	0,	sizeof(conSms));
								conSms.dataLock 	=1;	// 开启数据锁
								if(smsConForamtCheck(sMsg, &conSms) == DEF_SMSCONTROL_NONE)
								{
										ret =smsCarControlProcess(sMsg,	&conSms);
										if(ret == DEF_SMSCONTROL_NONE)
												conSms.dataLock =1;	// 数据锁状态维持不变(等待OBD或外设控制返回)
										else
										{
												// 错误处理
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
												conSms.dataLock =0;	// 取消数据锁释放资源
										}										
								}
								else
										conSms.dataLock =0;	// 格式非法不予处理	
						}
						else
						{
								// 在上一条短信控制为结束时收到了新的指令
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[EVENT]: SMS Control struct data-lock =1, Device will ignore this SMS!\r\n");
								#endif
						}					
				}
				// 短信控制应答接收
				conAckMsg = (CONAckMbox_TypeDef *)OSMboxAccept(OS_CONAckMbox);
				if(conAckMsg != (void *)0)
				{
						if(conSms.dataLock == 1)	// 只有来之短信或服务器请求的动作才给与打包回复
						{
								conSms.resultCode =conAckMsg->retCode; 						
								HAL_SMSConRetInQueueProcess	(&conSms);	// 车辆控制返回
								conSms.dataLock =0;	// 取消数据锁,准备处理下一条短信控制指令	
						}
						else
						{
								;	// 内部测试返回无需打包
						}
				}
				
				// 升级过程中指示灯控制/////////////////////////////////////////////////////
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
						 
			 	OSTmr_Count_Start(&s_swg.protocolCounter);				// 清软件看门狗计数器
			 	OSTimeDly(10);																// 交出CPU使用时间以便其他低优先级的任务运行
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
	
		glob_accOnRalStep =0;	// 清点火检测阶段变量

		// 开启BASE任务软件看门狗  /////////////
		OSTmr_Count_Start(&s_swg.baseCounter);
	
		// 开始点火熄火计时器
		OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);
		OSTmr_Count_Start(&s_osTmp.igOnChiXuCounter);				// 点火持续时间计时初始化
		OSTmr_Count_Start(&s_osTmp.sendDataToCounter);			// 开始发送超时统计
		OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);		// 开始读取车辆状态超时统计
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
				// 设备升级处理(升级过程中该任务长时占用CPU)/////////////////////////////////////////			
				if((s_common.iapSta == DEF_UPDATE_WAIT) || (s_common.iapSta == DEF_UPDATE_KEYWORD))
				{
						// 升级前等待当前队列数据全部发送完毕
						if(s_osTmp.upFlag == 1)
						{
								s_osTmp.upFlag =0;
								OSTimeDly(500);	// 进入升级流程等待500ms同步0124通用应答打包
						}
						if(OSTmr_Count_Get(&s_osTmp.upWaitCounter) < 40000)
						{
								if((HAL_ComQue_size (DEF_RAM_QUE) == 0) && (s_ram.len == 0) 	&&	
									 (s_flash.len == 0) && (comAck.len == 0) && (s_comAck.len	== 0))
										s_common.iapSta  	=DEF_UPDATE_KEYWORD;	// 设置升级全局标志进入升级流程
						}
						else
								s_common.iapSta  	=DEF_UPDATE_KEYWORD;	// 设置升级全局标志进入升级流程
						
						// 升级处理
						if(s_common.iapSta == DEF_UPDATE_KEYWORD)
						{
					
								// 关闭系统体检状态指示及状态指示灯
								IO_LED_RUN_OFF();
								HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_DISABLE);									
								
								// 删除不必要的任务
								if(taskDelProcess(10000) != 0)
								{
										// 不清标志复位后继续执行
										HAL_CPU_ResetPro();	// 如果未能成功删除任务则系统复位	
								}
								
								// 清除之前产生的GSM故障信息防止升级过程中出错直接导致模块复位
								BSP_GSM_ERR_Clr(&err_Gsm.gsmErrorTimes);
								APP_ErrClr(&s_osTmp.cnnErrTimes);
								APP_ErrClr(&s_osTmp.ackErrTimes);		// 清应答错误变量
							
								// 开始程序下载流程
								ret =HAL_DownLoadFileProcess(10);		
								HAL_CPU_ResetPro();			// 下载成功或尝试几次后都不能下载成功则系统复位
						}
				}				
				
				// IG启动延时处理//////////////////////////////////////////////////////////////////////
				if(igEnFlag == 0)
				{
						if(OSTmr_Count_Get(&s_osTmp.igEnDelayCounter) >= 2000)
						{
								igEnFlag =1;
								BSP_IG_IT_CON(ENABLE);	// 使能IG(低脉冲唤醒)中断
						}					
				}
		
				// 报警检测////////////////////////////////////////////////////////////////////////////
				HAL_EventAlarmProcess(s_cfg.eventSwt);

				// 正常点火流程//////////////////////////////////////
				if(HAL_SYS_GetState (BIT_SYSTEM_IG) == 1)
				{
						if(s_cfg.gpsCollectTime != 0)
						{
								if((s_cfg.devEn & DEF_DEVEN_BIT_GPS) && (GPS_DEVICE.InitSta == 1))
								{							
										if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0)
												OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);		// GPS不定位复位间隔计时器
										
										if(OSTmr_Count_Get(&s_osTmp.igOnMsgCounter) >= (s_cfg.gpsCollectTime * 1000))	 	// 点火状态根据设置上传数据
										{											 								 				 			 
												HAL_GPSInQueueProcess();
												//memset((u8 *)&s_gpsGL,	'\0',	sizeof(GPSProData_Def));	// 打包完成后清GPS信息
												OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);											
										}
								}
						}
				}
				// 正常熄火流程//////////////////////////////////////
				else
				{
						// 如果熄火移动报警则上报GPS位置信息
						if( HAL_SYSALM_GetState(BIT_ALARM_ILLMOVE) == 1 )
						{
								if(s_cfg.gpsCollectTime != 0)
								{
										if((s_cfg.devEn & DEF_DEVEN_BIT_GPS) && (GPS_DEVICE.InitSta == 1))
										{							
												if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0)
														OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);		// GPS不定位复位间隔计时器
												
												if(OSTmr_Count_Get(&s_osTmp.igOnMsgCounter) >= (s_cfg.gpsCollectTime * 1000))	 	// 点火状态根据设置上传数据
												{											 								 				 			 
														HAL_GPSInQueueProcess();
														//memset((u8 *)&s_gpsGL,	'\0',	sizeof(GPSProData_Def));	// 打包完成后清GPS信息
														OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);											
												}
										}
								}						
						}
						// 熄火暂时不上报GPS信息
//						if(s_cfg.offReportTime != 0)
//						{
//								if((s_cfg.devEn & DEF_DEVEN_BIT_GPS) && (GPS_DEVICE.InitSta == 1))
//								{
//										if(RTCTmr_Count_Get(&s_osTmp.igOffMsgCounter) >= s_cfg.offReportTime)
//										{
//												//	等待GPS定位
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
				// 24小时报告流程////////////////////////////////////
				if(RTCTmr_Count_Get(&s_osTmp.report24Counter) >= s_cfg.healthReportTime)
				{
						HAL_24RptInQueueProcess();
						RTCTmr_Count_Start(&s_osTmp.report24Counter);
						
						if(s_cfg.healthReportTime <= 14400)	// 4小时以内再次打包GPS数据包，以免丢失星历
						{
								healthFlag =1;					
								OSTmr_Count_Start(&healthGpsCounter);
						}
				}	
				
				// 2015-6-30日添加为了使设备GPS丢失星历
				if(healthFlag == 1)
				{
						if(OSTmr_Count_Get(&healthGpsCounter) <= 60000)
						{
								if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
								{
										OSTimeDly(4000);	// 	2015-8-5给予GPS,3点过滤赋值时间
										HAL_GPSInQueueProcess();
										//memset((u8 *)&s_gpsGL,	'\0',	sizeof(GPSProData_Def));	// 打包完成后清GPS信息
										healthFlag =0;
								}	
						}
						else
								healthFlag =0;	// 超时将退出GPS上报
				}

				// 清软件计数器 ////////////////////			
				OSTmr_Count_Start(&s_swg.baseCounter);
				OSTimeDly(50);	
		}
}
/*
*********************************************************************************************************
*                                            App_TaskBT()
*
* Description : 处理蓝牙业务
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

		// 开启任务软件看门狗  /////////////
		OSTmr_Count_Start(&s_swg.btCounter);

		while (1) 
		{	
				// 升级任务挂起等待
				while(s_common.iapSta == DEF_UPDATE_KEYWORD)
				{
						OSTmr_Count_Start(&s_swg.btCounter);	// 清软件计数器
						OSTimeDly(10000);	
				}

//				// 初始化蓝牙模块(配置蓝牙设名称) 
//				if(btCfgFlag == 0)
//				{
//						retCode	=DEF_GSM_NONE_ERR;
//						retCode	=BSP_BTInitPro((u8 *)DEF_BT_NAME, 1000);
//						if(retCode == DEF_GSM_NONE_ERR)
//						{
//								btCfgFlag = 1;		// 开机只配置一次
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
				// 连接/匹配处理
				btMsg = (BTMbox_Typedef *)(OSMboxAccept(GSMBtAppMbox));
				if(btMsg != (void *)0)
				{
						retCode	=DEF_GSM_NONE_ERR;
						if(btMsg->func == BT_PAIR_DIGI)				// 数字匹配申请
								retCode =BSP_BTPairAckPro(0,	(u8 *)"", 3000);
						else if(btMsg->func == BT_PAIR_PASS)	// 密码匹配申请
								retCode =BSP_BTPairAckPro(1,	(u8 *)DEF_BT_PASS, 3000);
						else if(btMsg->func == BT_CONNECT)		// 请求连接申请
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
						 
			 	OSTmr_Count_Start(&s_swg.btCounter);				// 清软件看门狗计数器
			 	OSTimeDly(50);																// 交出CPU使用时间以便其他低优先级的任务运行
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
		
		// GPS模块初始化及供电/////////////
		BSP_GPS_Init();	
	
		// GPS应用变量初始化
		HAL_GpsAppInit();

		// 开启任务软件看门狗  /////////////
		OSTmr_Count_Start(&s_swg.gpsCounter);	
    OSTmr_Count_Start(&gpsResetCounter);	// GPS错误复位计时器
		OSTmr_Count_Start(&rthPackCounter);		// GPS驾驶行为打包间隔计时器
	
		BSP_GPSRES_IT_CON(1);																						/* Enable GPS RXD	interrupt*/
    while (1) 
		{   
				// 升级任务挂起等待
				while(s_common.iapSta == DEF_UPDATE_KEYWORD)
				{
						OSTmr_Count_Start(&s_swg.gpsCounter);	// 清软件计数器
						OSTimeDly(10000);	
				}
				
				// GPS数据处理/////////////////////////////////////////////////////////
				if(OSSemAccept(GPSComSem) > 0) 															/* Wait for a packet to be received */	
				{											
						// 打印出GPS详细信息						
						#if(DEF_GPSINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GPS)
								BSP_OSPutChar(DEF_DBG_UART,	GPS_PORT.rxdBuf,	GPS_PORT.rxLen);
						#endif
						
						trsFinishF	=0;		// 清解析完成标志,开始解析数据
						paNum = BSP_GPS_CheckpackNum(GPS_PORT.rxdBuf,	GPS_PORT.rxLen);	// 检测符合$GPRMC格式数据包个数	
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
																break;	// 解析完成退出
												}
												else
												{
														#if(DEF_GPSINFO_OUTPUTEN > 0)
														if(dbgInfoSwt & DBG_INFO_GPS)
																myPrintf("\r\n[GPS]: Data pack parse fail[%d]!\r\n",	i);	// 数据包解析失败
														#endif
												}
										}
										else
										{		
												#if(DEF_GPSINFO_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_GPS)
														myPrintf("\r\n[GPS]: Data pack idex fail!\r\n");	// 数据检索失败
												#endif									
										}	
								}
								if((GPS_PORT.resSta & Rec_PACK_DEF) != Rec_PACK_DEF)
								{
										#if(DEF_GPSINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GPS)
												myPrintf("\r\n[GPS]: Data not enough!\r\n");	// 未达到用于解析的所有数据包
										#endif	
								}
								else
								{
										GPS_DEVICE.HardWareSta 	= DEF_GPSHARD_NONE_ERR;   // 清检测状态
										OSTmr_Count_Start(&gpsResetCounter);							// 清GPS复位计时器
										/////////////////////////////
										if(GPS_GPRMC.Sta[0] == 'A')
										{
												// 更新上次GPS基本状态便于超速,DSS计算							
												s_TmpGps.upSpeed 		= s_TmpGps.cunSpeed;
												s_TmpGps.upAngle 		= s_TmpGps.cunAngle;
										}
										////////////////////////////
										HAL_ConvertGpsData();		// 将GPS原始数据转成协议要求数据
										HAL_SaveDssProData(&s_gps);	// 将GPS输入存储到DSS队列
									  trsFinishF =1; 					// 设置启动计算标志
										
										// 解析成功不打印字符串避免频发上传
										//#if(DEF_GPSINFO_OUTPUTEN > 0)
										//if(dbgInfoSwt & DBG_INFO_GPS)
										//		myPrintf("\r\n[GPS]: Data parse success!\r\n");	// 数据解析成功
										//#endif	
								}
						}
						else
						{
								#if(DEF_GPSINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GPS)
										myPrintf("\r\n[GPS]: Illegal GPS data!\r\n");	// 非法GPS数据
								#endif	
						}
						BSP_GPS_RXBUF_UNLOCK(); 	// 重新开始接收数据 						
								
						// GPS数据应用处理 /////////////////////////////////////////////////////////////
						// 2015-9-21 修改未收到GPS数据包时也错误的进行应用处理的bug
						if((trsFinishF == 1) && (*(s_devSta.pGps)	==	DEF_GPSHARD_NONE_ERR))	
						{
								// 公里数计算
								// 超速报警
								// 区域报警							
								// DSS数据处理
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
														dssStep 		=1;	// DSS执行阶段变量
														dssPackNum	=0;	// 清后续接收数据包
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
														OSTmr_Count_Start(&rthPackCounter);		// GPS驾驶行为打包间隔计时器
												}									
												
												// DSS临时变量清空
												dssStep			=0;
												dssCode 		=0;
												dssAtimes 	=0;
												dssPackNum	=0;										
										}		
								}
								else
										dssStep =0;		

						}	
						
						// GPS数据过滤处理(用于位置上传) ///////////////////////////////////////////////
						if((trsFinishF == 1) && (*(s_devSta.pGps)	==	DEF_GPSHARD_NONE_ERR))	
						{
								// 点数累加
								lvboStep ++;
								if(lvboStep == 1)
										ljPoint =0;	// 循环开始清2D累加点数
								
								// 缓冲区更新
								if(s_gps.sta == 'V')
								{
										;						// 不定位不更新GPS缓冲区数据
								}
								else if(s_gps.sta == 'A')
								{
										if(s_gps.fs == '2')
										{
												ljPoint++;
												if(ljPoint >= GPS_LB_NUM)	
														memcpy((u8 *)&s_gpsBuf,	(u8 *)&s_gps,	sizeof(GPSData_TypeDef));	// 更新GPS缓存区数据
										}
										else if(s_gps.fs == '3') 
												memcpy((u8 *)&s_gpsBuf,	(u8 *)&s_gps,	sizeof(GPSData_TypeDef));	// 更新GPS缓存区数据		
										else
										{
												;				// 非法数据不予处理
										}								
								}
								else
								{
										;		// 非法数据不予处理
								}
								
								// 精确点确定
								if(lvboStep >= GPS_LB_NUM)
								{
										lvboStep =0;
										if(s_gpsBuf.sta == 'A')	
										{
												memcpy((u8 *)&s_gpsGL,	(u8 *)&s_gpsBuf,	sizeof(GPSData_TypeDef));	// 得到有效定位点,更新到最终过滤数据
										}
										else
										{
												;	// 所有过滤点都不定位则维持原定位信息不变
										}
										//////////////////////////////////////////////////////////////																							
										HAL_ConvertGpsProData(&s_gpsGL,	&s_gpsPro);													// 将最终过滤数据更新为协议数据
										memset((u8 *)&s_gpsBuf,	'\0',	sizeof(GPSData_TypeDef));							// 清除GPS缓冲区数据		
										/////////////////////////////
										if(s_gpsPro.speed > s_osTmp.maxSpeed)
													s_osTmp.maxSpeed =s_gpsPro.speed;		// 更新最大时速
								}		
								
								// GPS移动判定处理
		//						if(s_gpsGL.sta == 'A')
		//								gpsCheckEn =DEF_ENABLE;
		//						else
		//								gpsCheckEn =DEF_DISABLE;
		//						HAL_GpsMoveCheckProcess	(gpsCheckEn,	s_gpsGL.speed/10,	s_cfg.sysMove,	s_cfg.sysMoveT, 5, 60);
						}
				}
						
        // 检查GPS错误////////////////////////////////////////////////////////////////
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

				// 清软件计数器 ////////////////////			
				OSTmr_Count_Start(&s_swg.gpsCounter);
				OSTimeDly(50);
		}
}
/*
*********************************************************************************************************
*                                            App_TaskMUTUAL()
*
* Description : 通讯部分与OBD部分交互任务
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
		u8 sendCarInfoFlag=0,	carStateBuf[20]={0},tmpNum=0;	// 发送车辆信息标志
		u8 fuelBuf[4]={0};
		u16	carStateLen=0,tmpFuel=0;
		OBDMbox_TypeDef		*obdMsg;	// OBD交互消息结构
		CONAckMbox_TypeDef	conAck;	// OBD控制应啊结构
		SYS_DATETIME	tmpRtc;	// 调试输出时间戳
	
		p_arg = p_arg;	
		
		// 开启任务软件看门狗  //////////////////////////////////
		OSTmr_Count_Start(&s_swg.mutualCounter);
	
		// 清发送消息队列
		memset((u8 *)&s_obdCmd,	0,	sizeof(s_obdCmd));	
			
		while(1)
		{		
				// 升级任务挂起等待
				while(s_common.iapSta == DEF_UPDATE_KEYWORD)
				{
						OSTmr_Count_Start(&s_swg.mutualCounter);	// 清软件计数器
						OSTimeDly(10000);	
				}
				
				// 第一次上电下发车辆信息////////////////////////////
				if(sendCarInfoFlag == 0)
				{
						sendCarInfoFlag =1;	// 清标志
						// 第一次上电将车系车型推送给OBD部分(OBD部分还没加暂时取消)
						HAL_SendOBDProProcess(CMD_CAR_INFO,	0,	0,	s_common.carInfo,	3);	// 推送OBD当前车型车系发送机型号		
				}
				
				// OBD接收交互处理////////////////////////////////////////
				obdMsg = (OBDMbox_TypeDef *)(OSMboxAccept(OS_OBDAckMbox));
				if(obdMsg != (void *)0)
				{						
						#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						{
								BSP_RTC_Get_Current(&tmpRtc);	// 获取当前时间
								myPrintf("[%02d/%02d/%02d %02d:%02d:%02d]",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
								myPrintf("[Mailbox]: Receive from OBD!![cmdCode:%d][sn:%d][proCmdId:%04X][dataLen:%d][retCode:%d]\r\n",obdMsg->cmdCode,obdMsg->sn,obdMsg->proCmdId,obdMsg->proParaLen,obdMsg->retCode);
						}
						#endif
					
						// 需要打包数据处理
						if((obdMsg->cmdCode == CMD_PROTOCOL) ||
							 (obdMsg->cmdCode == CMD_READ_ALARMSTA) ||
							 (obdMsg->cmdCode == CMD_AUTO_TJ) ||
						   (obdMsg->cmdCode == CMD_READ_FUEL))
						{
								// 体检处理
								if((obdMsg->cmdCode == CMD_AUTO_TJ) ||
									 ((obdMsg->cmdCode == CMD_PROTOCOL) && (obdMsg->proCmdId == DEF_CMDID_TJDATA_DEV)))
								{
										// 体检语音处理
										if(HAL_SYS_GetState (BIT_SYSTEM_TJSOUND_END) == 1)
										{
												HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_DISABLE);	// 体检结束语音状态清除
												if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
														HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound2,	strlen((const char *)s_cfg.sound2));		// 播放体检结束语音
										}
										// 体检状态处理
										if(HAL_SYS_GetState (BIT_SYSTEM_TJSTA) == 1)
												HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_DISABLE);	// 系统置体检状态清除
										
										// 2015-9-28自动体检公里数获取
										if((obdMsg->cmdCode == CMD_AUTO_TJ) && (obdMsg->proParaLen >= 16))
										{												
												glob_odoBuf[0]	=obdMsg->proParaBuf[13];
												glob_odoBuf[1]	=obdMsg->proParaBuf[14];
												glob_odoBuf[2]	=obdMsg->proParaBuf[15];
										}
								}
								
								// 车辆报警状态读取
								if(obdMsg->cmdCode == CMD_READ_ALARMSTA)
								{
										s_osTmp.obdReadCarStaFlag =0;	// 置读取车辆状态完成
										OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);	// 重新开始读取车辆状态超时统计
								}
											
								// 数据处理(处理原则:当有新的OBD信息来时即使之前信息未发送成功也替换老的信息)
								if((obdMsg->proParaLen != 0) && (obdMsg->proParaLen <= DEF_OBDBUF_SIZE))
								{
										// 追加外设车辆状信息
										/* 2015-6-19 奥多车辆状态查询支持
										if((obdMsg->cmdCode == CMD_PROTOCOL) &&(obdMsg->proCmdId == DEF_CMDID_QUECARACK_DEV))
										{
												tmpNum =0;
												carStateLen =0;
												memset(carStateBuf,	0,	sizeof(carStateBuf));
												tmpNum =HAL_UpdataCarState(carStateBuf,	&carStateLen);
												// 改写协议数据包
												if((tmpNum != 0) && (tmpNum <= 5) && (carStateLen != 0) && (carStateLen <= 20))
												{
														*(obdMsg->proParaBuf + 6) = *(obdMsg->proParaBuf + 6) + tmpNum;	// 改写状态项
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
											
												// 赋值应答流水号
												obdMsg->proParaBuf[0] =(u8)(obdMsg->proSn>>8);
												obdMsg->proParaBuf[1] =(u8)(obdMsg->proSn);										
											
												if(obdMsg->retCode == 0)
												{
														// 支持
														*(obdMsg->proParaBuf+2) = 0x00;	// 成功
														if(fuelBuf[0] == 1)
														{
																*(obdMsg->proParaBuf+3) = 0x01;	// 百分比
																*(obdMsg->proParaBuf+4) = fuelBuf[2];
																*(obdMsg->proParaBuf+5) = fuelBuf[3];							
														}
														else
														{
																*(obdMsg->proParaBuf+3) = 0x00;	// 数值(L)
																tmpFuel = (u16)(((fuelBuf[1]<<16) | (fuelBuf[2]<<8) | (fuelBuf[3])) / 100 + 0.5);
																*(obdMsg->proParaBuf+4) = (u8)(tmpFuel>>8);
																*(obdMsg->proParaBuf+5) = (u8)tmpFuel;																	
														}														
												}
												else
												{
														// 不支持
														*(obdMsg->proParaBuf+2) = 0x01;	// 失败
														*(obdMsg->proParaBuf+3) = 0x00;	
														*(obdMsg->proParaBuf+4) = 0x00;	
														*(obdMsg->proParaBuf+5) = 0x00;	
												}											
										}
								
										s_obd.obdSendFlag = 0;	// 禁止当前的发送处理				
										HAL_PackOBDDataProcess	((u16)obdMsg->proCmdId,	(u8 *)obdMsg->proParaBuf,	(u16)obdMsg->proParaLen,	s_obd.buf,	&s_obd.len, (u16 *)&s_obd.sn);														
										
										if(obdMsg->proCmdId == DEF_CMDID_RTDATA_DEV)
												s_obd.ackType	=DEF_ACKTYPE_NOACK;	// 实时数据流异步上传无需应答
										else
												s_obd.ackType	=DEF_ACKTYPE_ACK;		// 其他OBD需上传到服务器的指令都需要应答
										s_obd.obdSendFlag =1;	// 触发发送任务发送数据包
								}
								
								// 车辆状态读取OBD不支持奥多状态独立打包处理
								/* 2015-6-19 奥多车辆状态查询支持
								if((obdMsg->cmdCode == CMD_PROTOCOL) && (obdMsg->proCmdId == DEF_CMDID_QUECAR_PLA))
								{
										if(obdMsg->retCode == 2)	// 不支持
												HAL_ReadCarStaInQueueProcess(glob_readCarSn,	glob_readTimeStamp);										
								}
								*/
								
								// OBD车系不匹配故障log打包处理
								if(((obdMsg->cmdCode == CMD_PROTOCOL) && (obdMsg->proCmdId == DEF_CMDID_STARTTJ_PLA)) ||
									 (obdMsg->cmdCode == CMD_AUTO_TJ))
								{
										if(obdMsg->retCode == 3)
												s_err.obdParaRWTimes++;	// 累加错误次数由事件监测任务统一打包
								}																							
						}
						// 其它控制应答处理
						else
						{
								// 下发车辆信息返回
								if(obdMsg->cmdCode == CMD_CAR_INFO)
								{
										;
								}
								// 短信控制车辆返回
								if(obdMsg->cmdCode == CMD_SMS) 
								{
										conAck.sn=0;									
										conAck.retCode =obdMsg->retCode;				// 存储结果码		 
										OSMboxPost(OS_CONAckMbox, (void *)&conAck); // 发送OBD控制应答邮箱 								
								}
								// 测试指令返回
								if(obdMsg->cmdCode == CMD_TEST)
								{
										if(obdMsg->retCode == 0)
												s_test.busTestCode =0x00;	// 置成功状态
										else if(obdMsg->retCode == 2)
												s_test.busTestCode =0xEE;	// 置错误状态
								}
								// 获取车辆点熄火状态返回
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
														glob_igSta =DEF_IG_ECU_TIMEOUT;			// 总线忙或/超时
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
														glob_igStaExt =DEF_IG_ECU_TIMEOUT;	// 总线忙或/超时
                        }
										}
								}
						}																																
						obdMsg->dataLock =0;	// 数据解锁
				}																					

				// 清软件计数器 ///////////////////////////////////////////////////////			
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
	
		// 开启任务软件看门狗  /////////////
		OSTmr_Count_Start(&s_swg.conCounter);
	
		// 初始化外部控制串口///////////////
		IO_RS232_POW_EN();
		BSP_USART_Init(DEF_CON_UART,	DEF_CON_REMAP,	DEF_CON_BAUT_RATE);	// 如果不初始化外部串口则串口测试将失败
		
		/* 2015-6-19 奥多控制支持
		// 初始化外部控制串口///////////////
		HAL_CON_Init();		
		
		// 开启调试串口中断
		HAL_CONRES_IT_CON(DEF_ENABLE);			// 使能外设串口接收中断	
		*/
				
		while(1)
		{	
				// 升级及升级等待过程中con任务将被挂起
				while((s_common.iapSta == DEF_UPDATE_WAIT) || (s_common.iapSta == DEF_UPDATE_KEYWORD))	
				{
						OSTmr_Count_Start(&s_swg.conCounter);	// 清软件计数器
						OSTimeDly(10000);	
				}
				
				/* 2015-6-19 奥多控制支持
				HAL_CONMainLoop();
				*/

				// 清软件计数器 ///////////////////////////////////////////////////////			
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
	
		// 初始化加速度传感器///////////////
		BSP_ACC_Init();		
		
		// 开启任务软件看门狗  /////////////
		OSTmr_Count_Start(&s_swg.dbgCounter);
		
		// 开启各应用计时器  /////////////
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
	
		// 开启调试串口中断
		HAL_DBGRES_IT_CON(DEF_ENABLE);			// 使能调试串口接收中断	
		
		// 开启加速度移动中断		
		BSP_ACC_CfgInt (DEF_ENABLE,	(u8)s_cfg.accMT,	(u8)s_cfg.accMD,	5000);
		BSP_ACC_IT_IO_CON(ENABLE);	// 初始化后默认使能加速度外部中断(休眠唤醒后不禁止)
				
		while(1)
		{	
				// 升级及升级等待过程中debug任务将被挂起
				while((s_common.iapSta == DEF_UPDATE_WAIT) || (s_common.iapSta == DEF_UPDATE_KEYWORD))	
				{
						OSTmr_Count_Start(&s_swg.dbgCounter);	// 清软件计数器
						OSTimeDly(10000);	
				}
				
				// 处理DBG接口数据 /////////////////////////////////////////////////////	
				if(OSSemAccept(DBGComSem) > 0) 	
				{
						HAL_DBGCmdProcess();
				}
				
				// 指示灯状态处理 /////////////////////////////////////////////////////
				if(s_cfg.devEn & DEF_DEVEN_BIT_GPRS)
				{
						// LED Run指示灯控制//////////////////////////////////////////////////////////////////////////        
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
								 
						// 开始进入循环
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
						// 退出循环
						else if(runSetp == 7)
						{
								if(OSTmr_Count_Get(&runCounter) >= LED_SETP7_TIME)
								{
										OSTmr_Count_Start(&runCounter);
										runSetp	=0;	
								}
						}             
						// LED Data 指示灯控制//////////////////////////////////////////////////////////////////////////        
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
								 
						// 进入循环
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
						// 退出循环
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

				// 加速度数据更新处理 /////////////////////////////////////////////////////
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
										HAL_Calculate_Main (s_Mems.X_Acc,	s_Mems.Y_Acc,	s_Mems.Z_Acc,	(u8 *)&glob_accAdjustFlag);	// 判断事件
						}	
						OSTmr_Count_Start(&accCounter);	
				}
				
				// 加速度移动探测
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
				
				// 加速度芯片故障检测
				if(OSTmr_Count_Get(&accWCheckCounter) >= 10000)
				{
						BSP_ACC_Check();
						OSTmr_Count_Start(&accWCheckCounter);	
				}			

				// OS信息输出//////////////////////////////////////////////////////////////////////////////
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

				// OBD信息输出
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

				// GPS信息输出
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

				// GSM信息输出
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

				// 清软件计数器 ///////////////////////////////////////////////////////			
				OSTmr_Count_Start(&s_swg.dbgCounter);							
				OSTimeDly(10);		
		}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                             【APP FUNCTION】
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                             taskDelProcess()
* Description : 任务删除处理timeout 单位ms
* Argument(s) : none.
* Return(s)   : none.
*********************************************************************************************************
*/
static	s8	taskDelProcess (u32	timeout)
{
		u16	i=0;
		vu32	timeoutConuter=0;
		
		// 超时计时开始
		OSTmr_Count_Start(&timeoutConuter);
	
		// 删除OBD任务
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
				HAL_SYSParaSemPost();	// 释放参数读写信号量以便其它函数使用(该处一定要释放否则将不可读写系统参数)		
		}	
		return	0;
}

/*
*********************************************************************************************************
*                                             smsConForamtCheck()
* Description : 检测控制短信格式如果正确将输出到SMSCon_Typedef结构中
* Argument(s) : none.
* Return(s)   : 0正确，<0错误
*********************************************************************************************************
*/
static	s8	smsConForamtCheck (SMS_Typedef *pSMS,	SMSCon_Typedef *pSMSOut)
{
		u16 i=0,j=0,tmpHexLen=0,tmpLen=0;
		u8	tmpHexBuf[80]={0};
		u8	tmpBuf[10]={0};	// 短信内容临时存储
			
		// 短信内容解密	
		if((pSMS->dataLen != 48) || (IsHexString(pSMS->dataBuf,	pSMS->dataLen) == 0xffff))
				return DEF_SMSCONTROL_FORMAT;	// 控制短信加密后长度为48char即24byte,且都为hex字符串形式
		
		// 格式转换成hex数组
		memset(tmpHexBuf, 0, sizeof(tmpHexBuf));
		tmpHexLen =String2HexArray (pSMS->dataBuf,	pSMS->dataLen,	tmpHexBuf);	
		if(desDecipherPro(tmpHexBuf,	tmpHexLen,	pSMSOut->dataBuf,	&pSMSOut->dataLen) != 0)
		{
				memcpy(pSMSOut->dataBuf,	pSMS->dataBuf,	pSMS->dataLen);	// 解密失败直接把未解密的原短信内容保存以便后续发送出去
				pSMSOut->dataLen =pSMS->dataLen;
				return DEF_SMSCONTROL_DES;	// 控制短信解密失败
		}
		
		j =StringSearch(pSMSOut->dataBuf,(u8 *)GSM_SMS_CONTROL_H,pSMSOut->dataLen,strlen((const char*)GSM_SMS_CONTROL_H));
		if(j == 0xffff)
				return DEF_SMSCONTROL_FORMAT;	// 非法格式
		
		// 获取命令码
		memset(tmpBuf, 0, sizeof(tmpBuf));
		j +=strlen((const char*)GSM_SMS_CONTROL_H);						
		for(i=0;	(i<4) && (i<pSMSOut->dataLen) && (pSMSOut->dataBuf[i] != ',');	i++)
		{
				tmpBuf[i] = pSMSOut->dataBuf[j+i];
		}
		tmpBuf[i] = '\0';
		pSMSOut->cmdLen =String2HexArray (tmpBuf,	i,	pSMSOut->cmdCode);	
		
		// 获取时间戳
		memset(tmpBuf, 0, sizeof(tmpBuf));
		j =j+i+1;		
		for(i=0;	(i<8) && (i<pSMSOut->dataLen) && (pSMSOut->dataBuf[i] != ',');	i++)
		{
				tmpBuf[i] = pSMSOut->dataBuf[j+i];
		}
		tmpBuf[i] = '\0';		
		sscanf((const char *)&tmpBuf,	"%x",	&pSMSOut->smsTimStamp);
		
		// 电话号码格式合法化	(该处兼容网络控制没有电话号码的情况)
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
* Description : 短信控制(检查短信超时，内容时间戳超时)
* Argument(s) : none.
* Return(s)   : DEF_SMSCONTROL_NONE~DEF_SMSCONTROL_TIMESTAMP
*********************************************************************************************************
*/
static	s8	smsCarControlProcess (SMS_Typedef *pSMS,	SMSCon_Typedef *pSMSOut)
{
		s8	ret=0;
		u32	tmpCun=0,	tmpSms=0;
		SYS_DATETIME   tmpR;		
						
		// 控制号码合法性
		if(strlen((const char *)pSMS->numBuf) != 0)
		{
				// 短信下发控制
				if((StringSearch(pSMS->numBuf,s_cfg.pho1,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho1)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho2,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho2)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho3,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho3)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho4,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho4)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho5,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho5)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.pho6,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.pho6)) != 0xffff) ||
					 (StringSearch(pSMS->numBuf,s_cfg.smsCenter,strlen((const char *)pSMS->numBuf),strlen((const char *)s_cfg.smsCenter)) != 0xffff))
				{				
						//短信超时判断  /////////////////////////////////////////////////////////////////////////////
						
						BSP_RTC_Get_Current(&tmpR);	
						tmpCun	=	Mktime(tmpR.year, tmpR.month, tmpR.day, tmpR.hour, tmpR.minute, tmpR.second); // 设备本身为GMT时间
						tmpSms	=	Mktime(pSMS->sTime.year, pSMS->sTime.month, pSMS->sTime.day, pSMS->sTime.hour, pSMS->sTime.minute, pSMS->sTime.second);										
						if(((tmpCun >= tmpSms) && ((tmpCun - tmpSms) <= OS_PROCONSMS_RECTIMEOUT)) || ((tmpCun < tmpSms) && ((tmpSms - tmpCun) <= OS_PROCONSMS_RECTIMEOUT)))
						{
								// 短信内容时间戳超时判断
								if(((pSMSOut->smsTimStamp >= tmpSms) && ((pSMSOut->smsTimStamp - tmpSms) <= OS_PROCOMSMS_STAMPTIMEOUT)) || 
									 ((pSMSOut->smsTimStamp < tmpSms) && ((tmpSms - pSMSOut->smsTimStamp) <= OS_PROCOMSMS_STAMPTIMEOUT)))
								{
										if(HAl_SMSCarControl(pSMSOut->cmdCode, pSMSOut->cmdLen) == 0)										
												ret =DEF_SMSCONTROL_NONE;	// 成功
										else
												ret =DEF_SMSCONTROL_BUSY;	// 总线忙
								}
								else
								{
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[EVENT]: SMS timestamp timeout!\r\n");
										#endif
										ret =DEF_SMSCONTROL_TIMESTAMP;	// 时间戳超
								}
						}
						else
						{
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[EVENT]: SMS Control timeout!\r\n");
								#endif
								ret =DEF_SMSCONTROL_TIMEOUT;	// 控制超时
						}
				}
				else
						ret =DEF_SMSCONTROL_NUM;	// 非法号码
		}
		else 
		{
				// 短信内容时间戳超时判断////////////////////////////////////////////////////////////////////
				BSP_RTC_Get_Current(&tmpR);	
				tmpCun	=	Mktime(tmpR.year, tmpR.month, tmpR.day, tmpR.hour, tmpR.minute, tmpR.second); // 设备本身为GMT时间
				if(((pSMSOut->smsTimStamp >= tmpCun) && ((pSMSOut->smsTimStamp - tmpCun) <= OS_PROCOMSMS_STAMPTIMEOUT)) || ((pSMSOut->smsTimStamp < tmpCun) && ((tmpCun - pSMSOut->smsTimStamp) <= OS_PROCOMSMS_STAMPTIMEOUT)))
				{
						if(HAl_SMSCarControl(pSMSOut->cmdCode, pSMSOut->cmdLen) == 0)										
								ret =DEF_SMSCONTROL_NONE;	// 成功
						else
								ret =DEF_SMSCONTROL_BUSY;	// 总线忙
				}
				else
				{
						#if(DEF_EVENTINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_EVENT)
								myPrintf("[EVENT]: SMS timestamp timeout!\r\n");
						#endif
						ret =DEF_SMSCONTROL_TIMESTAMP;	// 时间戳超时
				}			
		}
		return ret;
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                             【APP Tool】
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


