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
//#define	 APP_TASK_OBD1_PRIO								(13)	// TASK_PRIO 13~18 分配给OBD任务
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

// 软件看门狗各任务超时判断

#define	TASK_GSM_TIMOUT								((u32)30*1000)
#define	TASK_GPRS_TIMOUT							((u32)6*60*1000)
#define	TASK_RXD_TIMOUT								((u32)3*60*1000)
#define	TASK_SEND_TIMOUT							((u32)5*60*1000)
#define	TASK_PROTOCOL_TIMOUT					((u32)5*60*1000)
#define	TASK_BASE_TIMOUT							((u32)40*60*1000)			// 升级任务在该任务处理，由该值定义升级超时时间
#define	TASK_BT_TIMEOUT								((u32)2*60*1000)
#define	TASK_GPS_TIMOUT								((u32)3*60*1000)
#define	TASK_MUTUAL_TIMOUT						((u32)30*1000)
#define	TASK_DBG_TIMOUT								((u32)3*60*1000)
#define	TASK_CON_TIMOUT								((u32)60*1000)
#define	TASK_OBD1_TIMEOUT							((u32)3*60*1000)
#define	TASK_OBD2_TIMEOUT							((u32)3*60*1000)
#define	TASK_OBD3_TIMEOUT							((u32)3*60*1000)


// 软件看门狗数据结构
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

// 系统故障信息数据结构
typedef struct	tag_SYSError_Def
{			
		u8			removeTimes;																	// 24小时移除次数	
		u8			gpsResetTimes;																// GPS 24小时复位次数	
		u8			gsmResetTimes;																// GSM 24小时复位次数		
		u8			gsmSimTimes;																	// GSM SIM卡24小时移除次数
		u8			accTimes;																			// 加速度芯片读写24小时故障次数
		u8			obdBusRWTimes;																// OBD总线读写故障次数
		u8			obdParaRWTimes;																// OBD参数读写故障次数(特指车型车系参数)
		u8			flashTimes;																		// 外置Flash读写错误(指队列或离线数据读写)
		u8			cpuFlashTimes;																// CPU Flash读写错误(主要指系统参数读写)
		u8			conTimes;																			// 外部控制模块通信故障错误
		u8			spiFlashSumTimes;															// SPI FLASH总错误次数累计					
																																																						
}SYSError_Typedef;


// 复位优先级定义//////////////////////////////////
#define	DEF_RESET_PRI0								((vu8)0xAA)		//队列里有数据最多等待10秒后直接复位
#define	DEF_RESET_PRI1								((vu8)0xBB)		//队列里有数据最多等待180秒后直接复位	
#define	DEF_RESET_WAIT								((vu8)0xCC)		//所有数据发送完成后重启等待状态标志



// 系统临时应用变量
typedef struct	tag_APPTmp_Def
{			
		vu32		report24Counter;															// 24小时移除次数	
		vu32		igOnMsgCounter;																// 熄火上传数据间隔定时器
		vu32		igOffMsgCounter;															// 点火上传数据间隔定时器
		vu32		igOnChiXuCounter;															// 设备从上一次点火开始持续时间定时器
		vu8			cnnErrTimes;																	// GSM网络联网错误次数
		vu8			ackErrTimes;																	// GSM网络应答数据次数
		vu16		simErrTimes;																	// SIM卡应用中异常次数
		vu8			gsmConSleep;																	// GSM睡眠控制
		vu32		obdSn;																				// OBD控制流水号
		vu16		sysSn;																				// 系统数据上传流水号
		vu8			resetReq;																			// 系统复位请求标志
		vu16		maxSpeed;																			// 最大时速(用于点火熄火报文上传)
		vu32		sendDataToCounter;														// 发送数据超时定时器
		vu8			obdReadCarStaFlag;														// 读取车辆状态处理中标志
		vu32		obdReadCarStaCounter;													// 读取车辆状态定时器
		vu8			rxdDataLock;																	// 非升级模式事设备接收数据锁
		vu32		igStartTicks;																	// 点火开始系统滴答存储(用于点火熄火时间偏移量计算)	
		vu8			naviStep;																			// IP导航阶段变量
		vu32		appIPWorkCounter;															// 应用IP工作持续时间
		vu8			appIPCnnErrTimes;															// 应用IP联网错误次数
		vu32		igEnDelayCounter;															// IG延时检测定时器
		vu32		upWaitCounter;																// 升级等待定时器
		vu8			upFlag;																				// 升级等待标志
		vu8			sleepWaitFlag;																// 全局睡眠等待标志
		vu32		globSleepDelayCounter;												// 全局睡眠等待时间
		vu8			tjSendFlag;																		// 体检报文发标志
																																																						
}APPTmp_Typedef;

// GSM应用结构变量
typedef struct	tag_GSMAppVar_Def
{		
		u8			simNumWriteFlag;	// SIM卡号写入标志
		u8			pSim[30];					// SIM卡背面号码
		u8			simNum[20];				// SIM卡号码
		u8			IMEI[30];					// IMEI号码
		u8			MCC[4];						// 移动国家代码3char
		u8			MNC[4];						// 移动网络代码2或3char
		u8			LAC[5];						// 小区号码4hexchar
		u8			CELL[5];					// 基站ID4hexchar
		u8			sLeve;						// GSM当前信号强度
		u8			rssi;							// GSM当前误码率
								
}GSMAppVar_TypeDef;

///////////////////////////////////////////////////
// 用于发送结构的各类型数据结构定义

#define	DEF_ACKTYPE_ACK								((u8)0)				// 数据包应答类型-需要应答
#define	DEF_ACKTYPE_NOACK							((u8)1)				// 数据包应答类型-不需要

// 登录信息发送结构
#define	DEF_LOGINSENDBUF_SIZE					(100)					// 用于登陆的临时缓冲区大小
typedef struct	tag_TmpLoginSend_Def
{	
		vu8			handShake;															// 设备登陆登陆/握手状态0-未登陆1-登陆/握手完成
		vu16		sn;		
		u16			len;
		u8			buf[DEF_LOGINSENDBUF_SIZE];		
	
}TmpLoginSend_TypeDef;

// 用于发送OBD数据的发送结构
#define	DEF_OBDSENDBUF_SIZE						(600)					// 用于发送的临时缓冲区大小(应大于体检信息大小)
typedef struct	tag_TmpObdSend_Def
{
		vu8			obdSendFlag;														// OBD数据发送标志1为发送
		vu16		sn;
		u8			ackType;																// 数据应答类型见定义(DEF_ACKTYPE_ACK or DEF_ACKTYPE_NOACK)		
		u16			len;
		u8			buf[DEF_OBDSENDBUF_SIZE];
	
}TmpObdSend_TypeDef;

// 用于发送网络RAM队列发送结构
#define	DEF_RAMSENDBUF_SIZE						(200)					// 用于发送的临时缓冲区大小(应大于体检信息大小)
typedef struct	tag_TmpRamSend_Def
{		
		vu16		sn;
		u8			ackType;																// 数据应答类型见定义(DEF_ACKTYPE_ACK or DEF_ACKTYPE_NOACK)		
		u8			tryTimes;																// 尝试发送次数
		u8			type;																		// 打包的数据类型
		u16			len;
		u8			buf[DEF_RAMSENDBUF_SIZE];
	
}TmpRamSend_TypeDef;

// 用于发送网络FLASH打包数据的发送结构
#define	DEF_FLASHSENDBUF_SIZE					(600)					// 用于发送的临时缓冲区大小(应大于体检信息大小)
typedef struct	tag_TmpFlashSend_Def
{		
		vu16		sn;
		u8			ackType;																// 数据应答类型见定义(DEF_ACKTYPE_ACK or DEF_ACKTYPE_NOACK)	
		u16			queOutNum;															// 数据出队个数 
		u16			queLastOut;															// 打包之前的OUT指针值
		u8			tryTimes;																// 数据发送尝试次数
		u16			len;
		u8			buf[DEF_FLASHSENDBUF_SIZE];
	
}TmpFlashSend_TypeDef;

// 用于发送通用应答，心跳，断网，睡眠数据包的发送结构
#define	DEF_ACKSENDBUF_SIZE						(30)					// 用于发送的临时缓冲区大小
typedef struct	tag_TmpAckSend_Def
{		
		vu16		sn;		
		u16			len;
		u8			buf[DEF_ACKSENDBUF_SIZE];
	
}TmpAckSend_TypeDef;

// 通用发送结构打包数据类型定义
typedef enum
{
		PACK_TYPE_HEART					=0xAA,              		// 心跳包
		PACK_TYPE_SLEEP					=0xBB,		              // 睡眠数据包
		PACK_TYPE_SHUTNET				=0xCC,            			// 断网数据包
	
}PACK_SEND_TYPE;
#define	DEF_COMSENDBUF_SIZE						(30)					// 用于发送的临时缓冲区大小
typedef struct	tag_TmpComSend_Def
{		
		PACK_SEND_TYPE	packType;												// 打包数据类型具体见数据包发送类型枚举定义
		vu16		sn;		
		u16			len;
		u8			buf[DEF_COMSENDBUF_SIZE];
	
}TmpComSend_TypeDef;

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

MAIN_EXT		SYSError_Typedef	s_err;			// 系统故障数据结构
MAIN_EXT		SoftIWG_TypeDef		s_swg;			// 软件看门狗结构变量
MAIN_EXT		APPTmp_Typedef		s_osTmp;		// 系统应用临时变量(任务间需要共享的全局变量)
MAIN_EXT		GSMAppVar_TypeDef	s_gsmApp;		// GSM应用数据结构

MAIN_EXT		TmpLoginSend_TypeDef	s_login;	// 登录信息发送临时结构变量
MAIN_EXT		TmpObdSend_TypeDef		s_obd;		// OBD实时信息发送临时结构变量
MAIN_EXT		TmpRamSend_TypeDef		s_ram;		// RAM队列信息发送临时结构变量
MAIN_EXT		TmpFlashSend_TypeDef	s_flash;	// FLASH队列信息发送临时结构变量	
MAIN_EXT		TmpAckSend_TypeDef		s_comAck;	// 通用应答信息发送临时结构变量
MAIN_EXT		TmpComSend_TypeDef		s_comPack;	// 心跳，网络断开及睡眠信息发送临时结构变量


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
