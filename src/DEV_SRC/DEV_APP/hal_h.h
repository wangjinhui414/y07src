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

// 深度睡眠模式定义																 
#define	OS_DEEPSLEEP_MODE1						(1)						// 关闭所有外设电源及中断，除PVD,RTC,外部电源接入可唤醒
#define	OS_DEEPSLEEP_MODE2						(2)						// 关闭所有外设电源及中断，除PVD,RTC,可唤醒

// PVD特殊标志
#define	OS_INITPVD_FLAG								((u16)0xA56A)	// OS运行前PVD故障时设置

// IAP 升级外部FLASH存储位置定义
#define	DEF_IAP_BACKUP_POSI1					(u16)(0xA5A5)	// 当前程序存储在BACK_UP1位置
#define DEF_IAP_BACKUP_POSI2					(u16)(0xB5B5)	// 当前程序存储在BACK_UP2位置

// IAP 升级状态内部CPU FLASH标志意义定义//////////

#define	DEF_IAP_LOCATIONKEY_FLAG			((u16)0xA1A5)	// 按键升级状态(用于iapFlag赋值)		
#define	DEF_IAP_LOCATIONCMD_FLAG			((u16)0xA2A5)	// 本地串口命令升级状态(用于iapFlag赋值)		
#define	DEF_IAP_FTP_FLAG							((u16)0xA3A5)	// 远程FTP升级状态(用于iapFlag赋值)
#define	DEF_IAP_NETPRO_FLAG						((u16)0xA4A5)	// 远程用户自定义协议升级状态(用于iapFlag赋值)
#define	DEF_IAP_LOCATIONBKP_FLAG			((u16)0xA5A5)		// 本地串口命令升级状态,由BKP关键字触发(用于iapFlag赋值)	
#define	DEF_IAP_PRO1MID_FLAG					((u16)0xC1C5)	// 已经将升级程序下载到本地FLASH1备份位置(用于iapFlag赋值)
#define	DEF_IAP_PRO2MID_FLAG					((u16)0xC2C5)	// 已经将升级程序下载到本地FLASH1备份位置(用于iapFlag赋值)
#define	DEF_IAP_CLEAN_FLAG						((u16)0x0000)	// 清除升级标志		

// 定义升级状态关键字
#define	DEF_UPDATE_WAIT								((u16)0xA4A6)	// 定义升级等待状态(用于iapSta赋值)
#define	DEF_UPDATE_KEYWORD						((u16)0xA5A7)	// 定义升级状态中关键字(用于iapSta赋值)

// CPU内部FLASH地址划分定义////////////////////////
#define	FLASH_CPU_ALL_SIZE						(0x40000)			// CPU FLASH容量大小（STM32F105R8T6 256K）
#define	FLASH_CPU_IAP_START						(0x08000000)	// IAP程序开始地址
#define	FLASH_CPU_IAP_SIZE						(0x7800)			// IAP程序大小30K
#define	FLASH_CPU_COMMON_START				(0x08007800)	// 系统通用参数存储开始地址
#define	FLASH_CPU_COMMON_SIZE					(0x800)				// 系统通用参数大小2K
#define	FLASH_CPU_SYSCFG_START				(0x08008000)	// 系统配置参数存储开始地址
#define	FLASH_CPU_SYSCFG_SIZE					(0x800)				// 系统配置参数大小2K
#define	FLASH_CPU_SYSOBD_START				(0x08008800)	// 系统OBD参数存储开始地址
#define	FLASH_CPU_SYSOBD_SIZE					(0x800)				// 系统OBD参数大小2K
#define	FLASH_CPU_APPNAME_START				(0x08009000)	// 应用程序名字存储开始地址
#define	FLASH_CPU_APPNAME_SIZE				(0x800)				// 应用程序名字大小2K
#define	FLASH_CPU_APPSIZE_START				(0x08009800)	// 应用程序大小存储开始地址
#define	FLASH_CPU_APPSIZE_SIZE				(0x800)				// 应用程序大小大小2K
#define	FLASH_CPU_APP_START						(0x0800A000)	// 应用程序存储开始地址
#define	FLASH_CPU_APP_SIZE						(0x36000)			// 应用程序大小256K-30K-10K=216K
#define	DEF_MAXOF_VERB_LEN						(20)					// 定义版本号最大长度
 
/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 系统相关默认参数定义
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

//////////////////////////////////////////////////////////////////////
// 系统通用参数区-默认参数定义
#define	DEF_DFT_COMMON_SAVED_MARK			((u16)0x66A5)				// 系统通用参数区初始化完成标志(默认0x66A5,2015-5-29以后将不改变改值防止参数复位)
#define	DEF_DFT_COMMON_SUM						((u16)0x0000)				// 需要根据具体参数计算!!!!
#define	DEF_DFT_COMMON_IAPFLAG				((u16)0x0000)				// IAP升级标志
#define	DEF_DFT_COMMON_IAPSTA					((u16)0x0000)				// IAP升级状态
#define	DEF_DFT_COMMON_ID							((u8)0x00)					// 系统ID默认值
#define	DEF_DFT_COMMON_CARINFO				((u8)0x00)					// 车辆信息(车系+车型代码+发送机品牌)		
#define	DEF_DFT_COMMON_AADC						((u16)2671)					// 系统默认AD基准 (设置ADC默认值)需要根据具体参数填写!!!!
#define	DEF_DFT_COMMON_SWDG						((u8)0)							// 软件看门狗默认值
#define	DEF_DFT_COMMON_HVERB					("00.00")						// 系统硬件版本号默认值
#define	DEF_DFT_COMMON_GPSDIS					((double)0)					// GPS累加公里数默认值
#define	DEF_DFT_COMMON_OTAPAPN				("CMNET")						// 远程升级APN
#define	DEF_DFT_COMMON_OTAPAPNUSER		("")								// 远程升级APN用户名
#define	DEF_DFT_COMMON_OTAPAPNPASS		("")								// 远程升级APN密码 
#define	DEF_DFT_COMMON_OTAPIP					("")								// 远程升级IP	
#define	DEF_DFT_COMMON_OTAPPORT				("")								// 远程升级PORT
#define	DEF_DFT_COMMON_OTAPFTPUSER		("")								// FTP服务器用户名	
#define	DEF_DFT_COMMON_OTAPFTPPASS		("")								// FTP服务器密码
#define	DEF_DFT_COMMON_OTAPFTPFILE		("")								// FTP服务器升级路径及文件名
#define	DEF_DFT_COMMON_OTAPFILESIZE		((u32)0)						// 远程升级程序大小
#define	DEF_DFT_COMMON_OTAPERRCODE		((u8)0)							// 远程升级错误码


//////////////////////////////////////////////////////////////////////
// 系统应用参数区-默认参数定义
#define	DEF_DFT_SYSCFG_SAVED_MARK			((u16)0x0034)				// 系统应用参数区初始化完成标志(默认0x0034,2015-5-29以后将不改变改值防止参数复位)
#define	DEF_DFT_SYSCFG_SUM						((u16)0x0000)				// 需要根据具体参数计算!!!!
#define	DEF_DFT_SYSCFG_APN						("CMNET")						// 系统APN	
#define	DEF_DFT_SYSCFG_APNUSER				("")								// 系统APN登陆用户	
#define	DEF_DFT_SYSCFG_APNPASS				("")								// 系统APN登陆密码	
#define	DEF_DFT_SYSCFG_DNSIP					("202.106.0.20")		// 系统域名服务器IP地址
#define	DEF_DFT_SYSCFG_DOMAIN1				("zxs3.wiselink.net.cn")						// 系统域名1
#define	DEF_DFT_SYSCFG_DOMAIN2				("zxs3.wiselink.net.cn")						// 系统域名2
#define	DEF_DFT_SYSCFG_IP1						("221.123.179.91")	// 系统主服务器IP地址(导航IP)
#define	DEF_DFT_SYSCFG_PORT1					("7005")						// 系统主服务器端口号
#define	DEF_DFT_SYSCFG_IP2						("")								// 系统辅助服务器IP地址(应用IP)
#define	DEF_DFT_SYSCFG_PORT2					("")								// 系统辅助服务器端口号
#define	DEF_DFT_SYSCFG_DOMAINEN				((u8)0)							// 域名连接使能如果为1则使能域名连接为0则禁止域名连接即IP连接
#define	DEF_DFT_SYSCFG_GPSCOLLECTTIME	((u8)30)						// GPS采集时间间隔，单位为秒(s)
#define	DEF_DFT_SYSCFG_ONREPORTTIME		((u32)30)						// 点火信息汇报间隔，单位为秒(s)	
#define	DEF_DFT_SYSCFG_OFFREPORTTIME	((u32)0)						// 熄火信息汇报间隔，单位为秒(s)	
#define	DEF_DFT_SYSCFG_HEARTBEATTTIME	((u32)32)						// 终端心跳发送间隔，单位为秒(s)	
#define	DEF_DFT_SYSCFG_TCPACKTIMEOUT	((u32)30)						// TCP消息应答超时时间，单位为秒(s)
#define	DEF_DFT_SYSCFG_HEALTHREPORTTIME	((u32)10800)			// 健康包上报时间间隔，单位为秒(s)，默认4小时 2015-6-30 改为3小时
#define	DEF_DFT_SYSCFG_SMSCENTER			("")								// 监控平台SMS猫电话号码	
#define	DEF_DFT_SYSCFG_EVENTSWT				((u32)0xffffffff)		// 事件上报开关字，相应位为1则上报相应事件0则不上报
#define	DEF_DFT_SYSCFG_OVERSPEED			((u16)120)					// 超速报警门限，单位为公里每小时(km/h)
#define	DEF_DFT_SYSCFG_OVERSPEEDTIME	((u16)10)						// 超速持续时间，单位为秒(s)
#define	DEF_DFT_SYSCFG_TIREDDIRVETIME	((u32)10800)				// 疲劳驾驶时间门限，单位为秒(s)默认3小时		
#define	DEF_DFT_SYSCFG_PHO1						("")								// 车辆报警电话号码1
#define	DEF_DFT_SYSCFG_PHO2						("")								// 车辆报警电话号码2
#define	DEF_DFT_SYSCFG_PHO3						("")								// 车辆报警电话号码3
#define	DEF_DFT_SYSCFG_PHO4						("")								// 车辆报警电话号码4
#define	DEF_DFT_SYSCFG_PHO5						("")								// 车辆报警电话号码5
#define	DEF_DFT_SYSCFG_PHO6						("")								// 车辆报警电话号码6
#define	DEF_DFT_SYSCFG_AESKEY					((u8)0x00)					// 秘钥
#define	DEF_DFT_SYSCFG_AESTYPE				((u8)0x00)					// 加密类型	
#define	DEF_DFT_SYSCFG_DEVEN					((u8)0x0f)					// 设备使能开关(默认关闭振动报警提示及外设控制功能)
#define	DEF_DFT_SYSCFG_WMODE					((u8)0)							// 设备工作方式
#define	DEF_DFT_SYSCFG_NATIONCODE			("86")							// 国家代码(中国默认"86")使用PDU方式发送短信时使用		
#define	DEF_DFT_SYSCFG_SLEEPSWT				((u8)0xff)					// 睡眠唤醒开关
#define	DEF_DFT_SYSCFG_DSSATH					((u16)350)					// DSS急加速门限	
#define	DEF_DFT_SYSCFG_DSSDTH					((u16)550)					// DSS急加速门限
#define	DEF_DFT_SYSCFG_DSSRTH					((u16)0)						// DSS急转弯门限	
#define	DEF_DFT_SYSCFG_DSSPTH					((u16)1100)					// DSS碰撞门限	
#define	DEF_DFT_SYSCFG_ACCMT					((u16)8)						// 加速度芯片移动INT1唤醒门限，单位为毫G(mg)	
#define	DEF_DFT_SYSCFG_ACCMD					((u16)1)						// 加速度芯片移动INT1唤醒时长，单位毫秒(ms)
#define	DEF_DFT_SYSCFG_SYSMOVE				((u8)15)						// 系统移动速度门限,单位:公里/小时(使用GPS速度判断移动时依据)
#define	DEF_DFT_SYSCFG_SYSMOVET				((u8)60)						// 系统移动时长门限,单位:秒(使用GPS速度判断移动时依据)
#define	DEF_DFT_SYSCFG_LOWPOWER				((u16)1150)					// 低压报警门限
#define	DEF_DFT_SYSCFG_TJDELAYTIME		((u8)5)							// 体检延时时间默认5,单位:秒
#define	DEF_DFT_SYSCFG_IGOFFDELAY			((u16)60)						// 熄火检测延时时间默认1min,单位:秒
#define	DEF_DFT_SYSCFG_NAVISTEP				((u8)0)							// IP导航阶段变量
#define	DEF_DFT_SYSCFG_GLOBSLEEPTIME	((u32)1800)				// 设备全局睡眠进入时间默认30分钟,单位:秒2015-8-3 修改为23:54分钟,2015-9-28 修改为30分钟
#define	DEF_DFT_SYSCFG_SOOUND1				("车联网系统开始对您的爱车进行体检，请稍候")											// 体检前播放信息
#define	DEF_DFT_SYSCFG_SOOUND2				("请留意手机客户端的体检通知")		// 体检完成播放信息

//////////////////////////////////////////////////////////////////////
// 系统OBD参数区-默认参数定义(由OBD部门定义)
#define	DEF_DFT_SYSOBD_SAVED_MARK			((u16)0x0002)				// 系统OBD参数区初始化完成标志
//#define	DEF_DFT_SYSOBD_SUM						((u16)0x0000)				// 需要根据具体参数计算!!!!
//#define	DEF_DFT_SYSOBD_ODO						((double)0)					// OBD公里数
//#define	DEF_DFT_SYSOBD_FUEL						((double)0)					// OBD累加油耗

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 系统相关参数大小定义
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

//////////////////////////////////////////////////////////////////////
// 系统通用参数区-参数大小定义
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
// 系统应用参数区-参数大小定义
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
// 系统OBD参数区-参数大小定义(由OBD部门定义)
//#define PRA_SIZEOF_SYSOBD_SAVED_MARK  (sizeof(s_obd.savedMark)) 	  		// 
//#define PRA_SIZEOF_SYSOBD_SUM  				(sizeof(s_obd.sum)) 	        		// 
//#define PRA_SIZEOF_SYSOBD_ODO  				(sizeof(s_obd.odometer)) 	    		// 
//#define PRA_SIZEOF_SYSOBD_FUEL  			(sizeof(s_obd.fuel)) 	        		// 

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 系统相关参数最大长度定义
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

/////////////////////////////////////////////////////////////////////
// 系统通用参数区-最大长度定义
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
// 系统应用参数区-最大长度定义
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
// 系统OBD参数区-最大长度定义(由OBD部门定义)

//#define PRA_MAXOF_SYSOBD_SAVED_MARK  	(PRA_SIZEOF_SYSOBD_SAVED_MARK) 	  // 
//#define PRA_MAXOF_SYSOBD_SUM  				(PRA_SIZEOF_SYSOBD_SUM) 	        // 
//#define PRA_MAXOF_SYSOBD_ODO  				(PRA_SIZEOF_SYSOBD_ODO) 	    		// 
//#define PRA_MAXOF_SYSOBD_FUEL  				(PRA_SIZEOF_SYSOBD_FUEL) 	        // 
//#define	PRA_MAXOF_SYSOBD							(sizeof(SYSObd_Typedef))

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 系统相关参数CPU内部FLASH起始地址定义
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

/////////////////////////////////////////////////////////////////////
// 系统通用参数区-起始地址定义
#define	ADDR_START_COMMON							(FLASH_CPU_COMMON_START)					// 系统通用存储区开始地址

//////////////////////////////////////////////////////////////////////
// 系统应用参数区-起始地址定义
#define	ADDR_START_SYSCFG							(FLASH_CPU_SYSCFG_START)					// 系统应用参数存储区开始地址

//////////////////////////////////////////////////////////////////////
// 系统应用参数区-起始地址定义
#define	ADDR_START_SYSOBD							(FLASH_CPU_SYSOBD_START)					// 系统OBD参数存储区开始地址

/////////////////////////////////////////////////////////////////////
// 操作失败重试次数定义
#define	DEF_TRYTIMES_CPUREAD				(3)						// CPU flash读重试次数
#define	DEF_TRYTIMES_CPUWRITE				(3)						// CPU flash写重试次数

/*
*********************************************************************************************************
*                                               OTHER DEFINES
*********************************************************************************************************
*/

// 系统参数类型入参
typedef enum {		
		DEF_TYPE_COMMON = 0,							// 通用参数																	
		DEF_TYPE_SYSCFG,									// 应用参数		
		DEF_TYPE_SYSOBD,									// OBD参数	
}ENUM_PARA_TYPE;

#define	DEF_PARA_BUSY_ERR							((s8)-10)						// 参数读取忙
#define	DEF_PARA_UNWRITE_ERR					((s8)-9)						// 参数未被写入
#define	DEF_PARA_UNKNOW_ERR						((s8)-8)						// 参数未知错误
#define	DEF_PARA_ERASE_ERR						((s8)-7)						// 参数擦除错误
#define	DEF_PARA_WRITECMP_ERR					((s8)-6)						// 参数写入对比错误
#define	DEF_PARA_WRITE_ERR						((s8)-5)						// 参数写入错误
#define	DEF_PARA_READ_ERR							((s8)-4)						// 参数读取错误
#define	DEF_PARA_TYPE_ERR							((s8)-3)						// 参数类型错误
#define	DEF_PARA_SUM_ERR							((s8)-2)						// 参数校验和错误
#define	DEF_PARA_INIT_ERR							((s8)-1)						// 参数未初始化错误
#define	DEF_PARA_NONE_ERR							((s8)0)							// 参数正确

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// 通用信息数据结构
typedef struct	tag_SYSCommon_Def
{
		u16			savedMark;																		// 参数初始化过标志
		u16			sum;																					// 参数累加和标志
		u16			iapFlag;																			// IAP升级标志(用于BootLoader中使用)
		u16			iapSta;																				// IAP升级状态(用于应用中使用)
		u8			id[PRA_MAXOF_COMMON_ID];											// 设备ID号
		u8			carInfo[PRA_MAXOF_COMMON_CARINFO];						// 车辆信息(车系+车型代码+发送机品牌)				
		u16			aadc;																					// 设备AD基准			
		u8			swdg[13];																			// 设备软件看门狗信息
		u8			hverb[PRA_MAXOF_COMMON_HVERB];								// 设备硬件版本号
		double	gpsDis;																				// GPS累加公里数					
		u8			otapApn[PRA_MAXOF_COMMON_OTAPAPN]; 						// OTAP APN	
		u8			otapApnUser[PRA_MAXOF_COMMON_OTAPAPNUSER];		// OTAP APN登陆用户名	
		u8			otapApnPass[PRA_MAXOF_COMMON_OTAPAPNPASS];		// OTAP APN登陆密码	
		u8			otapIp[PRA_MAXOF_COMMON_OTAPIP];							// OTAP服务器IP地址
		u8			otapPort[PRA_MAXOF_COMMON_OTAPPORT];					// OTAP服务器端口号	
		u8			otapFtpUser[PRA_MAXOF_COMMON_OTAPFTPUSER];		// FTP登陆用户名	
		u8			otapFtpPass[PRA_MAXOF_COMMON_OTAPFTPPASS];		// FTP登录密码
		u8			otapFtpFile[PRA_MAXOF_COMMON_OTAPFTPFILE];		// FTP文件目录及文件
		u32			otapFileSize;																	// OTAP文件大小
		u8			otapErrCode;																	// OTAP更新错误码																													

}SYSCommon_Typedef;

// 设备开关位定义(devEn)//////////////////////////////////////////////
#define	DEF_DEVEN_BIT_TTS							((u8)0x01)					// TTS使能开关(1:开启,0:关闭)
#define	DEF_DEVEN_BIT_GPS							((u8)0x02)					// GPS使能开关(1:开启,0:关闭)
#define	DEF_DEVEN_BIT_BT							((u8)0x04)					// BT使能开关(1:开启,0:关闭)
#define	DEF_DEVEN_BIT_GPRS						((u8)0x08)					// GPRS使能开关(1:开启,0:关闭)
#define	DEF_DEVEN_BIT_CON							((u8)0x10)					// 远程控制模块使能开关(1:开启,0:关闭)
#define	DEF_DEVEN_BIT_SHAKE						((u8)0x20)					// 振动报警开关(1:开启,0:关闭)

//////////////////////////////////////////////////////////////////////
// 系统配置信息数据结构
typedef struct	tag_SYSConfig_Def
{			
		u16			savedMark;																		// 参数初始化过标志															
		u16			sum;																					// 参数累加和标志									
		///////////////////////////////////////////////////////////////////////////
		u8			apn[PRA_MAXOF_SYSCFG_APN]; 										// 系统APN	
		u8			apnUser[PRA_MAXOF_SYSCFG_APNUSER];						// 系统APN登陆用户	
		u8			apnPass[PRA_MAXOF_SYSCFG_APNPASS];						// 系统APN登陆密码	
		u8			dnsIp[PRA_MAXOF_SYSCFG_DNSIP];								// 系统域名服务器IP地址
		u8			domain1[PRA_MAXOF_SYSCFG_DOMAIN1];						// 系统域名1 
		u8			domain2[PRA_MAXOF_SYSCFG_DOMAIN2];						// 系统域名2
		u8			ip1[PRA_MAXOF_SYSCFG_IP1];										// 系统主服务器IP地址(导航IP)
		u8			port1[PRA_MAXOF_SYSCFG_PORT1];								// 系统主服务器端口号
		u8			ip2[PRA_MAXOF_SYSCFG_IP2];										// 系统辅助服务器IP地址(应用IP)
		u8			port2[PRA_MAXOF_SYSCFG_PORT2];								// 系统辅助服务器端口号
		u8			domainEn;																			// 域名连接使能如果为1则使能域名连接为0则禁止域名连接即IP连接
		///////////////////////////////////////////////////////////////////////////
		u8			gpsCollectTime;																// GPS采集时间间隔，单位为秒(s)
		u32			onReportTime;																	// 点火信息汇报间隔，单位为秒(s)	
		u32			offReportTime;																// 熄火信息汇报间隔，单位为秒(s)	
		u32     heartbeatTime;                                // 终端心跳发送间隔，单位为秒(s)	
		u32     tcpAckTimeout;                                // TCP消息应答超时时间，单位为秒(s)
		u32			healthReportTime;															// 健康包上报时间间隔，单位为秒(s)
		u8			smsCenter[PRA_MAXOF_SYSCFG_SMSCENTER];				// 监控平台SMS猫电话号码	
		u32     eventSwt;                              				// 事件上报开关字。相应位为1则上报相应事件0则不上报
		u16     overSpeed;                                		// 超速报警门限，单位为公里每小时(km/h)
		u16     overSpeedTime;                                // 超速持续时间，单位为秒(s)
		u32     tiredDirveTime;                               // 疲劳驾驶时间门限，单位为秒(s)																																																		
		u8			pho1[PRA_MAXOF_SYSCFG_PHO1];									// 车辆报警电话号码1
		u8			pho2[PRA_MAXOF_SYSCFG_PHO2];									// 车辆报警电话号码2
		u8			pho3[PRA_MAXOF_SYSCFG_PHO3];									// 车辆报警电话号码3
		u8			pho4[PRA_MAXOF_SYSCFG_PHO4];									// 车辆报警电话号码4
		u8			pho5[PRA_MAXOF_SYSCFG_PHO5];									// 车辆报警电话号码5
		u8			pho6[PRA_MAXOF_SYSCFG_PHO6];									// 车辆报警电话号码6
		u8			aeskey[PRA_MAXOF_SYSCFG_AESKEY];							// 终端AES密钥
		u8			aesType;																			// 加密类型	
		///////////////////////////////////////////////////////////////////////////
    u8			devEn;											                  // 设备使能开关(见上面设备使能位定义)
    u8      wMode;                                        // 设备工作方式
    u8			nationCode[PRA_MAXOF_SYSCFG_NATIONCODE];			// 国家代码(中国默认086)		 
		u8			sleepSwt;																			// 睡眠开关
		u16			dssATH;																				// DSS急加速门限	
		u16			dssDTH;																				// DSS急加速门限
		u16			dssRTH;																				// DSS急转弯门限	
		u16			dssPTH;																				// DSS碰撞门限			
		u16			accMT;																				// 加速度芯片移动INT1唤醒门限，单位为毫G(mg)	
		u16			accMD;																				// 加速度芯片移动INT1唤醒时长，单位毫秒(ms)		
		u8			sysMove;																			// 系统移动速度门限,单位:公里/小时(使用GPS速度判断移动时依据)
		u8			sysMoveT;																			// 系统移动时长门限,单位:秒(使用GPS速度判断移动时依据)
		u16			lowPower;																			// 低压报警门限,单位:V(扩大100倍存储)
		u8			tjDelayTime;																	// 体检延时时长单位秒
		u16			igOffDelay;																		// 熄火检测延时时间单位秒
		vu8			naviStep;																			// IP导航阶段变量(0-需要重新申请IP;1-获取IP成功;2-IP已经失效)
		u32			globSleepTime;																// 设备睡眠全局时间(单位:s)
		u8			sound1[PRA_MAXOF_SYSCFG_SOUND1];							// 体检开始播放声音
		u8			sound2[PRA_MAXOF_SYSCFG_SOUND2];							// 体检结束
		//u8      testapp2;
}SYSConfig_Typedef;

////////////////////////////////////////////////////////////////////////
//// OBD存储数据结构
//typedef struct	tag_SYSObd_Def
//{			
//		u16			savedMark;																		// 参数初始化过标志														
//		u16			sum;																					// 参数累加和标志
//		double	odometer;																			// OBD公里数
//		double	fuel;																					// OBD累加油耗
//																																																						
//}SYSObd_Typedef;


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/
HAL_EXT		u16										globInitPVDFlag;			// 初始化中PVD故障标志(OS运行前如果PVD有效该标志设置)
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

// 系统初始化函数
void 			HAL_CPU_ResetPro 					(void);
void			HAL_INIT_ALL							(void);
u8				HAL_SysParaLoadProcess 		(void);
void			HAL_ParaSetDefault 				(ENUM_PARA_TYPE	type);
void			HAL_ParaSetDefaultExt 		(ENUM_PARA_TYPE	type);

// 系统参数读写接口函数
s8				HAL_LoadParaPro 					(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);
s8				HAL_SaveParaPro 					(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);

// 系统睡眠接口函数
s8				HAL_PerpheralInPowerMode	(void);
s8				HAL_PerpheralOutPowerMode	(void);

// 系统参数信号量获取
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
