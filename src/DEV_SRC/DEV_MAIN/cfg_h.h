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
// 厂商基本配置

// 系统版本号
#define	DEF_SOFTWARE_VER							("05.00")				// 通讯部分版本号(非网络版本号)

// 蓝牙配置
#define	DEF_BT_NAME										("ZXT-Y06")			// 蓝牙模块名字(最大长度18字节)
#define	DEF_BT_PASS										("1234")				// 密码连接方式时提供的密码(长度4~16字节)

// 厂商ID信息
#define	DEF_COMPANY_ID								((u8)0x01)			// 设备登陆/握手数据包厂商ID智信通为:0x01

///////////////////////////////////////////////////////////////////////////////////////
// 系统-看门狗与睡眠控制																 
#define	OS_SYSWDG_EN									(1)							// 如果为1则使用看门狗
#define	OS_FEEDWDG_TIME								(2)							// n*s 系统喂狗时间间隔
#define	OS_SYSWDG_TIME								(SYSWDG_TIMEOUT)	// n*s 设置IWG看门狗溢出时间
#define	OS_RTC_ALM_SECS								(10)						// n*s RTC ALM闹钟睡眠时间间隔秒数(该时间应该比看门狗溢出时间小)
#define	OS_RTC_REFRESH								(1)							// n*s RTC 软件中断刷新时间
	
#define WOK_SLEEPMODE_EN							(1)							// 如果为1则使能进入睡眠模式
#define	OS_INSLEEP_IDLE_TIME					((u32)10*1000)	// n*ms 当所有条件满足后空闲该时间后正式进入睡眠状态
#define	OS_INSLEEP_READCARSTA_TIMEOUT	((u32)30*60*1000)	// n*ms 如果该时间内收不到OBD的车辆状态返回也强制进入睡眠
#define	OS_INSLEEP_SENDDATA_TIMEOUT		((u32)5*60*1000)	// n*ms 如果该时间内还是不能发完队列数据或不能联网成功则进入睡眠
#define	OS_RST_CHECKSLEEP_TIME				((u32)5*60*1000) //n*ms 系统复位后进入睡眠监测的延迟时间
//#define	OS_RST_CHECKSLEEP_TIME				((u32)60*1000) //n*ms 系统复位后进入睡眠监测的延迟时间
#define	OS_OBDOFF_CHECKINTERVAL_TIME	((u32)30*1000) 	//n*ms 系统通过OBD检查车辆状态时间间隔
#define OS_TJSOUND_MAX_TIMEOUT				((u32)30*1000) 	//n*ms 体检结束语音播报最大超时时间
#define OS_TJ_MAX_TIMEOUT							((u32)300*1000)	//n*ms 体检最大超时时间

// 协议相关
#define	OS_NAVIREQ_EN									(1)							// 如果使能后将取消导航数据包发送及取消IP失效机制
#define	OS_SENDACK_EN									(1)							// 发送数据后是否需要服务器应答
#define	OS_HEARTBEAT_EN								(1)							// 是否发送心跳包数据

//////////////////////////////////////////////////////////////////////////////////////		
// 系统应用定义		
#define	OS_GPSREFIX_WAIT_TIMEOUT			((u16)1200)			// n*100ms 设备熄火自动上传数据包等待GPS(默认120s)
#define	OS_HEART_ACK_ERR_TIMES				(3) 						// 心跳包及数据包接收应答错误次数(默认5：次)

#define	OS_ACK_ERR_TIMES							(10) 						// 应答数据包接收应答错误次数(默认10：次)

#define	OS_LP_SHUTNET_TINES						((u8)5)					// 低功耗模式下断网尝试次数
#define	OS_LP_GSMSLEEP_TINES					((u8)5)					// 低功耗模式下GSM模块睡眠尝试次数
#define	OS_PROCOMSMS_STAMPTIMEOUT			((u32)600)				// 协议控制短信时间戳超时时间单位秒(默认：10分钟)
#define	OS_PROCONSMS_RECTIMEOUT				((u32)600)			// 协议控制短信接收超时时间单位秒(默认：10分钟)

#define	OS_IGON_CHECK_TIMES						((u8)2)					// IG ON 状态确认次数
#define	OS_IGOFF_CHECK_TIMES					((u8)50)				// IG OFF 状态确认次数
#define	OS_IG_CHECK_TIME							((u32)100)			// IG 状态确认时间间隔

#define	OS_LOWPOWIN_CHECK_TIMES				((u8)20)				// LOW POW IN 状态确认次数
#define	OS_LOWPOWOFF_CHECK_TIMES			((u8)20)				// LOW POW OUT 状态确认次数
#define	OS_LOWPOW_CHECK_TIME					((u32)1000)			// LOW POW 状态确认时间间隔

#define	OS_GSM_MAXERR_TIME						((u16)60)				// GSM底层错误最大次数
#define	OS_SIM_MAXERR_TIME						((u16)60)			  // SIM卡检测错误最大次数10分钟后复位
#define	OS_GPS_ERRCHECK_TIME					((u32)8000)			// GPS应用层错误确定时隔

#define	OS_LOGUP_INTERVAL_TIME				((u32)10*60*1000)	// 设备故障LOG上传时间间隔默认10分钟
#define	OS_APPIP_VALID_TIME						((u32)86000)		// n*ms 基于RTC记时(默认23小时54分底层RTC晶振超过24小时自动从0开始)
#define	OS_APPIP_CNNERROR_TIMES				((u8)3)					// 应用IP联网连续失败最大次数(超过该次数应用IP将做失效处理)						


///////////////////////////////////////////////////////////////////////////////////////
// 调试信息-调试信息输出控制
#define	DEF_INITINFO_OUTPUTEN					(1)							// 初始化信息输出使能
#define	DEF_MEMINFO_OUTPUTEN			  	(1)							// 存储器全局调试信息输出使能
#define	DEF_ACCINFO_OUTPUTEN					(1)							// ACC信息输出使能
#define	DEF_GPSINFO_OUTPUTEN					(1)							// GPS信息输出使能
#define	DEF_GSMINFO_OUTPUTEN					(1)							// GSM信息输出使能
#define	DEF_EXTIINFO_OUTPUTEN					(0)							// 中断信息输出使能
#define	DEF_EVENTINFO_OUTPUTEN				(1)							// 使能事件信息输出
#define	DEF_ACCINFO_OUTPUTEN					(1)							// 使能后将输出ACC信息
#define	DEF_OSINFO_OUTPUTEN						(1)							// 系统OS信息输出
#define	DEF_OBDINFO_OUTPUTEN					(1)							// 使能输出OBD数据信息
#define	DEF_GPRSINFO_OUTPUTEN					(1)							// GPRS信息输出使
#define	DEF_BTINFO_OUTPUTEN						(1)							// BT信息输出使能
#define	DEF_SMSINFO_OUTPUTEN					(1)							// SMS信息输出使能
#define	DEF_ADCINFO_OUTPUTEN					(1)							// AD采集信息输出
#define	DEF_IGCHECKLOGIC_OUTPUTEN			(1)							// IG点火熄火及相应加速度状态输出使能
						
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
