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

// 驱动说明：
// 驱动程序支持SIMCOM公司的SIM800模块（需要注意：32M版本TTS或蓝牙只能取一，只有64M版本有蓝牙和TTS）
// 网络部分支持多个TCP连接，需要注意如果调用一次BSP_GPRSResetPro处理函数则所有被建立的TCP连接都将断开，
// BT部分只支持单一连接，且第一包必须收到"SIMCOMSPPFORAPP"后蓝牙功能才成功激活，否则断开重连。

// 版本说明：
// V1.00:	首版建立
// V1.01: 为了提升接收数据效率，防止数据遗漏特将GPRS接收数据提示方式改为信号量投递，代替之前的邮箱投递方式
// V1.02: 修改因为接收hex数据中包含匹配字符串导致的提前退出，从而数据接收不完整
// V1.03: 增加本机号码读取AT指令定义及操作函数
// V1.04: 调整了TTS语音播报条件，即使不插卡也可播报
// V1.05: 优化初始化命令(取消跟SIM卡检测及相关的初始化指令)，独立增加一个关于SIM卡的初始化函数，网络配置前增加SIM状态判断
//        短信发送前增加SIM状态判断，网络联网应答增加一个"CONNECT FAIL"匹配项
// V1.06: CNUM查询SIM卡号时跳过+86部分
// V1.07: 增加GSM强制断网函数BSP_GPRSFroceBreakSet()
// V1.08:	将TCP接收数据划分位两个函数1个用于应用过程中接收，一个应用于升级下载文件过程中
// V1.09: 优化非升级模式中TCP接收函数
// V1.10: 修改TCP发送函数内部匹配应答AT为特殊模式只有收到期待字符串才退出，避免TTS自动上报的URC结束影响发送判断，导致认为断网;
// V1.11: 在网络配置中增加CGATT的查询操作
// V1.12: 初始化函数中增加URC上传RING中断使能(弥补GPRS接收数据遗漏问题)--可能导致不停读取接收数据,该逻辑已取消!!!
// V1.13: 修改升级网络接收数据函数，读取前判断接收长度从而增加延时确保收到数据完整性
// V1.14: 增加CPIN状态查询的错误字符匹配(插卡: CPIN: READY, 不插卡: +CPIN: NOT INSERTED(第一次上电) or ERROR)
// V1.15: 修改SIM状态查询错误错误处理只有超时才进行错误累加
// V1.16: 修改短信读取函数将运营商发送时间改为格林威治时间；
// V1.17: 将IP地址获取错误合并到通用错误中
// V1.18: 修改GSM模块开机I/O判断错误，判断开启超时时间5s->10s否则SIM800C可能会出现检测超时；


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


/********** 内部 GSM I/O 控制接口 *************/

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

/********** 内部使用函数接口 *************/

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
uc8				AT_CTTSPARAM[]		=	"AT+CTTSPARAM=50,0,50,60,0\r";	// 设置TTS参数（格式：AT+CTTSPARAM=<Vol>,<Mode>,<Pitch>,<Spped>,<Channel>）
uc8				AT_CFGRI_1[]			=	"AT+CFGRI=1\r";			// 设置URC上传时伴随中断产生
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
uc8				AT_CIMI[] 				= "AT+CIMI\r";				// Query IMSI code(全球移动设备签署识别码)
uc8				AT_CGSN[] 				= "AT+CGSN\r";				// Query IMEI code(全球移动设备识别码,等同于AT+GSN)
uc8				AT_GSN[] 					= "AT+GSN\r";					// Query IMEI code(等同于AT+CGSN)

uc8				AT_CSQ[] 					= "AT+CSQ\r";					// Query GSM signal strength
uc8				AT_CPIN[] 				= "AT+CPIN?\r";				// Query SIM card state
uc8				AT_COPS[] 				= "AT+COPS?\r";				// Query Operator Selection(当前的运营商)
uc8				AT_CREG[] 				= "AT+CREG?\r";				// Query GSM regist state
uc8				AT_CGREG[] 				= "AT+CGREG?\r";			// Query GPRS regist state(当设置AT+CGATT=1后,返回0,1)
uc8				AT_CGATT[]				=	"AT+CGATT?\r";			// Query attach or detach from GPRS(查询是否附着或分离到GPRS网络)
uc8				AT_CLCC[]					=	"AT+CLCC\r";				// Query Call state
uc8				AT_CNUM[]					=	"AT+CNUM\r";				// 查询本机号码(只有SIM卡写入该号码后才可读出,否则返回空字符串)

/******************************************************************************************************/
/* GPRS Init And Connect cmd */
/******************************************************************************************************/
uc8				AT_CGATT_0[] 			= "AT+CGATT=0\r";			// Mode attch the gprs net
uc8				AT_CGATT_1[] 			= "AT+CGATT=1\r";			// Mode attch the gprs net

uc8				AT_CIPMUX_1[]			= "AT+CIPMUX=1\r";		// 使能多路连接模式（0~5通道,最大6路连接,多路模式禁止透传模式，
																									// 且只允许最多1个服务器端，5个客户端。多路模式使能后透传设置指令CIPMODE指令响应为ERROR）
uc8				AT_CIPMUX_0[]			= "AT+CIPMUX=0\r";		// 禁止多路连接模式

uc8				AT_CIPMODE_0[]		= "AT+CIPMODE=1\r";		// 配置网络收发透传模式(注意：如果使能多连接模式则该指令任何配置都返回"ERROR")
uc8				AT_CIPMODE_1[]		= "AT+CIPMODE=0\r";		// 配置网络收发命令模式(如果使能多连接模式不能使用该指令)

uc8				AT_CSTT_H[]				=	"AT+CSTT=\"";				// 配置APN,USER,PASS
uc8				AT_CIICR[]				=	"AT+CIICR\r";				// 建立无线链路（GPRS）
uc8				AT_CIFSR[]				= "AT+CIFSR\r";				// 获得本地IP地址
		
/******************************************************************************************************/
/* TCP Init And Connect cmd */
/******************************************************************************************************/

uc8				AT_CIPRXGET_1[]		=	"AT+CIPRXGET=1\r";	// 设置网络数据读取方式为手动方式,有数据用（AT+CIPRXGET=2,id,1000）读取
uc8				AT_CIPHEAD_1[]		=	"AT+CIPHEAD=1\r";		// 设置收到数据数据头格式(+CIPRXGET: )
uc8				AT_CIPSTART_H[]		=	"AT+CIPSTART=";			// 建立TCP连接（格式：AT+CIPSTART=0,"TCP","218.249.201.35","13000"）
uc8				AT_CIPCLOSE_H[]		=	"AT+CIPCLOSE=";			// 关闭TCP连接
uc8				AT_CIPSHUT[]			=	"AT+CIPSHUT\r";			// 关闭当前GPRS上下文环境
uc8				AT_CIPSEND_H[]		=	"AT+CIPSEND=";			// 发送数据（格式：AT+CIPSEND=1,8）
uc8				AT_CIPRXGET_H[]		=	"AT+CIPRXGET=2,";		// 手动获取接收到的网络数据（格式：AT+CIPRXGET=2,1,1000）
uc8				AT_CIPRXGET4_H[]	=	"AT+CIPRXGET=4,";		// 手动查询接收到的数据长度
uc8				AT_CIPSENDQ[]			=	"AT+CIPSEND?\r";		// 用于查询当前可用于发送的数据长度


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

uc8				AT_CPBS_ON[] 			= "AT+CPBS=\"ON\"\r";	// 使能可以从SIM卡电话薄中读取本机号码
uc8				AT_CPBW_H[] 			= "AT+CPBW=1,\"";			// 写入本机号码(写入后可使用CNUM读取本机号码)		

uc8				PDUfixP[]					=	"1100";							// PDU手机号固定字节	
uc8				PDUfixD[]					=	"000800";						// PDU短信数据固定字节
	
uc8				PDUinter[]				=	"91";								// 国际化标志
uc8				PDUbphone[]				=	"91";								// 给手机发送代码
uc8				PDUsphone[]				=	"81";								// 给小灵通发送代码



/******************************************************************************************************/
/* BT Init And Connect cmd */
/******************************************************************************************************/

// 模块作为服务器端时，客户端连接成功第一包数据必须是“SIMCOMSPPFORAPP”用来使模块切换到APP模式
uc8				AT_BTPOWER_1[]		=	"AT+BTPOWER=1\r";		// 打开蓝牙模块电源
uc8				AT_BTPOWER_0[]		=	"AT+BTPOWER=0\r";		// 关闭蓝牙模块电源
uc8				AT_BTSTATUS[]			=	"AT+BTSTATUS?\r";		// 查询蓝牙状态信息
uc8				AT_BTHOST_H[]			=	"AT+BTHOST=";				// 设置主机名
uc8				AT_BTUNPAIR[]			=	"AT+BTUNPAIR=0\r";	// 删除所有配置信息
uc8				AT_BTVIS_1[]			=	"AT+BTVIS=1\r";			// 蓝牙可见使能
uc8				AT_BTVIS_0[]			=	"AT+BTVIS=0\r";			// 蓝牙可见禁止
uc8				AT_BTPAIR_1[]			=	"AT+BTPAIR=1,1\r";	// 响应其它设备数字匹配方式
uc8				AT_BTPAIR_2_H[]		=	"AT+BTPAIR=2,";			// 响应其它设备Passkey匹配方式
uc8				AT_BTACPT_1[]			=	"AT+BTACPT=1\r";		// 接收其它设备的连接请求
uc8				AT_BTACPT_0[]			=	"AT+BTACPT=0\r";		// 拒绝其它设备的连接请求
uc8				AT_BTSPPGET_0[]		= "AT+BTSPPGET=0\r";	// 接收数据模式-自动
uc8				AT_BTSPPGET_1[]		= "AT+BTSPPGET=1\r";	// 接收数据模式-手动
uc8				AT_BTSPPGET_2[]		=	"AT+BTSPPGET=2\r";	// 手动模式下接收收到的数据长度
uc8				AT_BTSPPGET_3_H[] = "AT+BTSPPGET=3,";		// 手动模式下获取接收到的数据（格式：AT+BTSPPGET=3,17 17:数据长度）
uc8				AT_BTSPPCFG[]			=	"AT+BTSPPCFG?\r";		// 获取当前收发数据状态（格式：+BTSPPCFG: S,1,0 S:服务器,C:客户端;1:连接ID;0:AT模式,1:APP模式）
uc8				AT_BTSPPSEND_H[]	= "AT+BTSPPSEND=";		// 按长度发送数据


/******************************************************************************************************/
/* FS Init And APP cmd */
/******************************************************************************************************/

uc8				AT_FSDRIVE_0[]		=	"AT+FSDRIVE=0\r";		// 获取本地磁盘盘符（+FSDRIVE: C）
uc8				AT_FSDRIVE_1[]		= "AT+FSDRIVE=1\r";		// 获取SC卡磁盘盘符（+FSDRIVE: D~G）
uc8				AT_FSLS_H[]				=	"AT+FSLS=";					// 获取本地文件信息
uc8				AT_FSLS_C[]				=	"AT+FSLS=C:\\r";		// 获取本地C盘下文件夹信息
uc8				AT_FSLS_USER[]		=	"AT+FSLS=C:\\User\\r";		// 获取本地User下文件夹信息
uc8				AT_FSMKDIR_H[]		=	"AT+FSMKDIR=";			// 在目录下创建文件夹（格式：AT+FSMKDIR=C:\User\Log）
uc8				AT_FSCREATE_H[]		=	"AT+FSCREATE=";			// 在目录下创建文件（格式：AT+FSCREATE=C:\User\Log\Log.txt）
uc8				AT_FSFLSIZE_H[]		=	"AT+FSFLSIZE=";			// 查询文件大小（格式：AT+FSFLSIZE=C:\User\Log\Log.txt）
uc8				AT_FSWRITE_H[]		=	"AT+FSWRITE=";			// 写文件（格式：AT+FSWRITE=C:\User\Log\Log.txt,0,23,10  
                                                  // 0:在文件开头插入，将覆盖后面的数据 1：在文件结尾写入;23:长度;10:写入超时秒）
uc8				AT_FSREAD_H[]			= "AT+FSREAD=";				// 读文件（格式：AT+FSREAD=C:\User\Log\Log.txt,0,100,0                                                  
																									// 0:从文件头开读 1:从posi读; 100:长度; 0:读文件起始地址position）
uc8				AT_FSMEM[]				=	"AT+FSMEM\r";				// 查询各个盘符空间占用情况（格式：+FSMEM: C:76288byte）	
uc8				AT_FSRENAME_H[]		=	"AT+FSRENAME=";			// 重名了文件（格式：AT+FSRENAME=C:\User\Log\Log.txt,C:\User\Log\Log1.txt）		
uc8				AT_FSDEL_H[]			=	"AT+FSDEL=";				// 删除文件夹（格式：AT_FSDEL=C:\User\Log\Log.txt）


/******************************************************************************************************/
/* TTS Init And APP cmd */
/******************************************************************************************************/

uc8				AT_CTTS_0[]				=	"AT+CTTS=0\r";			// 停止当前语音播放
uc8				AT_CTTS_1_H[]			=	"AT+CTTS=1,";				// UCS2方式播放当前语音
uc8				AT_CTTS_2_H[]			= "AT+CTTS=2,";				// ASCII方式播放当前语音（格式：AT+CTTS=1,欢迎使用）
uc8				AT_CTTSPARAM_H[]	=	"AT+CTTSPARAM=";		// 设置TTS参数（格式：AT+CTTSPARAM=<Vol>,<Mode>,<Pitch>,<Spped>,<Channel>）
uc8				AT_CTTSRING_0[]		= "AT+CTTSRING=0\r";	// 来电时禁止播放语音
uc8				AT_CTTSRING_1[]		= "AT+CTTSRING=1\r";	// 来电时可以播放语音

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
extern		OS_EVENT					*GSMMutexSem;					// 底层互斥信号量
extern		OS_EVENT					*GSMSmsSem;						// 收到短信信号量
extern		OS_EVENT					*GSMGprsDataSem;			// GPRS接收数据信号量
extern		OS_EVENT					*GSMBtDataMbox;				// 蓝牙接收数据邮箱
extern		OS_EVENT					*GSMBtAppMbox;				// 蓝牙获取业务状态邮箱

static		vu8								GSMMutexFlag	=1;			// 如果运行在无OS状态下由该变量实现互斥操作
static		vu32							ringCheckConuter =0;	// ring事件超时定时器
static		vu8								ringWUpFlag	=0;				// 改标志由睡眠任务置“1“由Ring中断设置为”2“表明设备由Ring唤醒
static		vu8								tcpReadMuxtex	=0;			// 为了实现网络读取与模块自动上传的URC字符串实现互�
																									//(如果读取得为升级程序文件，可能会出现一些固化在程序中的URC，
																									// 使模块错误的以为是当前模块收到的URC，导致上报状态的错误)

BTMbox_Typedef							s_MboxBT;							// 蓝牙接收邮箱变量，用于传输蓝牙数据及业务

static		vu8								ringCheckStep=0;			// Ring检测阶段变量
static		vu32							ringCheckCounter=0;		// Ring检测定时器



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

static		void			GSMFeedWDGProcess					(void);		// 初始化过程中需要调用的喂狗操作


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
		IWDG_ReloadCounter();		// 喂狗
		#endif
}


/*
*********************************************************************************************************
*                            					GSM模块工具函数
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
				*(pASCII + j) = (u8)(val & 0x000000ff);	// 取最低字节
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
*                            					GSM模块I/O基本操作函数(提供开/关机/复位等)
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
		// 退出睡眠模式
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
* Input          : 设备复位时调用
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_POW_RESET	(u32	dlayTime)
{	
		//IO_GSM_RES_ON();		// RESET 管脚有可能会漏电导致放电不干�
													//(这里禁止RST管脚控制,如果控制则会输出初始化字符串导致初始化程序接收初始化字符串接收失败)		
		IO_GSM_POW_OFF();
		IO_GSM_POW_DIS();		
    if (dlayTime <= 6000u)
    {
        dlayTime = 6000u;   //防止断电时间过短GSM模块无法复位 20160104
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
* Description    : GSM模块硬件断电处理        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSMHardWarePowShut(void)
{
		memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	// 清错误变量		
		s_GSMcomm.SetupPhase 		= PowerOff;
		s_GSMcomm.GSMComStatus	=	GSM_POWOFF;
		BSP_GSM_ReleaseFlag(DEF_ALL_FLAG);
		IO_GSM_POW_DIS();															// 模块断电 
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMHardWareReset() 
* Description    : 当软件复位无效时执行硬件复位        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMHardWareReset(u32 dlayTime)
{
		memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	// 清错误变量		
		s_GSMcomm.SetupPhase 		= PowerOff;
		s_GSMcomm.GSMComStatus	=	GSM_POWOFF;
		BSP_GSM_ReleaseFlag(DEF_ALL_FLAG);
		BSP_GSM_POW_RESET(dlayTime);
		return	0;
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ReleaseFlag()	
* Description    :  用于上位机清空模块状态         
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
* Description    :  用于上位机设置模块状态          
* Input          : 
* Output         : 	 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_SetFlag (u8	flagType)
{
		if(flagType == DEF_ALL_FLAG)
		{
				;		// 不做处理
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
* Description    :  用于上位机判断模块状态       
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
*                            					GSM字符操作工具函数
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : GSMInvertNumbers
* Description    : 正常顺序的字符串转换为两两颠倒的字符串，若长度为奇数，补'F'凑成偶数 
*									 如："8613851872468" --> "683158812764F8"
*									 输入: pSrc - 源字符串指针
*									       nSrcLength - 源字符串长度
*									 输出: pDst - 目标字符串指针
*									 返回: 目标字符串长度
* Input          :  
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	u16 GSMInvertNumbers (u8* pSrc, u8* pDst, u16 nSrcLength)
{
		u16 nDstLength,i;   			// 目标字符串长度
		u8  ch;    								// 用于临时保存一个字符
	  u8* ptSrc = pSrc;
		u8* ptDst = pDst;
	
		nDstLength = nSrcLength;
		for(i=0; i<nSrcLength;	i+=2)
		{
				ch = *ptSrc++;   			// 保存先出现的字符
				*ptDst++ = *ptSrc++; 	// 复制后出现的字符
				*ptDst++ = ch;   			// 复制先出现的蛀
		}
		if(nSrcLength & 1)
		{
				*(ptDst-2) = 'F'; 		// 补'F'
				nDstLength++;  	 			// 目标串长度�1
		}
		*ptDst = '\0';
		return nDstLength;
}
/*
*********************************************************************************************************
* Function Name  : PDUMakeUpData
* Description    : 将短信以PDU模式打包并返回长度数值 
*									 输入: mode 			- DEF_COM_PHONE / DEF_PHS_PHONE
*												 pSmsCenter - 短信中心号码指针
*												 pPhone			-	目的手机号码指针
*												 pData			-	需要发送的数据指针
*												 pDst				-	目的存储区指针			
* Return         : 打包后信息总长度(改长度不包括短信中心地址部分字节) 并附上0x1a 
*********************************************************************************************************
*/
static  u16 PDUMakeUpData (u8	mode,	u8	*pSmsCenter,	u8 *pPhone,	u8	*pData,	u16	 DataLen, u8	*pDst)
{
		u8	tmpBuf[50]="",tmpDuf[50]="",tmpPhone[50]="";
		u16 j=0,i=0,len=0,tmplen=0;
		
		memset(tmpBuf,	'\0',	sizeof(tmpBuf));
		memset(tmpDuf,	'\0',	sizeof(tmpDuf));
		memset(tmpPhone,	'\0',	sizeof(tmpPhone));		
		
		// 短信中心部分打包
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
		// 手机号码部分打包
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
				//	普通手机
				for(i=0;	PDUbphone[i]!='\0';	i++,j++,len++)
				{
						*(pDst+j) = PDUbphone[i];		 	// copy "91"
				}
		}
		else
		{
				//	小灵通
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
		// 手机信息部分打包
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
* Description : 用于匹配主动请求的AT指令返回码三种模式：DEF_NULL_MODE	DEF_ONLYOK_MODE	DEF_SPECIAL_MODE DEF_COMMON_MODE
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
        ;											 				// 等待超时退出(用于指令透传)
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
		        // 等待超时
		        ;													// 用于需要取返回码并解析的AT如AT+CSQ	AT+CLCC等
		    }
				
		}
		if((checkMode	==	DEF_SPECIAL_MODE) || (checkMode	==	DEF_COMMON_MODE))
		{
				// 特殊模式检测项目
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
				// 短信接收属于自动上报不再发送AT返回配置里处理在URC自动上报配置处理
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
	      /* 最后判断超时,以免程序跑完一循环,再到这就已经超时了.但实际已收到应答. */
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
*                            							GSM底层收/发中断函数(stm32f10x_it.c接口)     
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
		
		// Ring 触发I/O
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
		
		// Ring 触发I/O 
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
* Description: Ring pin interrupts.	 如果ringWUpFlag已被设置”1“则修改ringWUpFlag=2来表示设备睡眠由Ring事件唤醒
* Argument(s): none.
* Return(s)  : none.
* Note(s)    : 
*********************************************************************************************************
*/
void  BSP_GSMRing_ISRHandler (void)
{	
		if(ringWUpFlag == 1)
				ringWUpFlag =2;	// 设置GSM由Ring唤醒标志
		
		// 设置Ring产生标志
		BSP_GSM_SetFlag(DEF_RING_FLAG);	// 通知Ring中断产生
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
		// AT指令数据.
		if(ComGSM.RCNT	<	GSM_GPSRSBUF_RXD_SIZE)
		{				
				RBufGPRS[ComGSM.RCNT] = rx_data;  // 数据读到用户缓冲区.
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
		    /* 处于发送模式 */
		    if(ComGSM.SCNT < ComGSM.SLEN)
				{
						USART_SendData(usart, SBufGPRS[ComGSM.SCNT]);
						while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
						ComGSM.SCNT++;
		    }
				else
				{
			      state=(state&(~SS))|SS_O; 	  		// 置发送完成标志. 
			      state = state&(~MOD);         		// 置为空闲模式. 
			
						ComGSM.State	=	state;		
						GSM_DISABLE_SEND_IT();						// 停止GSM发送中.
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
	
		// 2015-1-16 by：gaofei 增加数据同步机制，防止发送数据截断接收字符串
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
		ComGSM.State =MOD_S | SS_I; //置发送模式及发送进行中标志 
		ComGSM.State &=~SS_O;				//2015-11-4 清发送完成标志
    
		GSM_ENABLE_SEND_IT();				//Enable	send interrupt
																	
		if(ComGSM.SCNT < ComGSM.SLEN)
		{
				USART_SendData(usart, SBufGPRS[ComGSM.SCNT]);
	  		while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);	  // 如果不加该行则发送数据不完整，缺字节！
				ComGSM.SCNT++;
		}
}

/*
*********************************************************************************************************
*                            							GSM错误处理函数   
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ERR_Add()
* Description    : 错误累加        
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
* Description    : 错误累加复位        
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
* Description    : 错误累加次数查询        
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
*                            							GSM底层发送数据应用函数     
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
			  s_GSMcomm.ActiveCmd = ATCmdActive;			//	首先占据串口
				pInit8u = (u8 *)s_GSMcomm.pSendBuf;
				
				for(m=0;	m<s_ATcomm.Slen;	m++)
				{
			  		*pInit8u = *(s_ATcomm.pSendBuf + m);
			  		pInit8u++;
		    }
				memset((u8 *)(s_ATcomm.pSendBuf),	'\0',	GSM_ATSBUF_TXD_SIZE);	 // 发之前清空缓冲区
		    /* call sendstart. */
		    s_GSMcomm.Timer = 0;
		    s_GSMcomm.CmdPhase = CommandSend;
		    s_GSMcomm.ps_commu->SLEN = m;
		    s_GSMcomm.sendstart();			//调用发送
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
								*(s_ATcomm.pRecBuf + m) = *(s_GSMcomm.pRecBuf + m);	//将返回码赋值到AT接收BUF中
						}
							
            s_GSMcomm.CmdPhase = CommandIdle;							//设置为AT命令的空闲模式
            s_GSMcomm.ActiveCmd = NoCmdActive;
						s_GSMcomm.CommandFlag	&= ~ATCommandF;					//清AT发送标志
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
* Return         : 返回:	DEF_GSM_NONE_ERR 成功, DEF_GSM_BUSY_ERR占用中, 
*													其他返回码(数值为1~255)不会与 DEF_GSM_NONE_ERR，DEF_GSM_BUSY_ERR冲突
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
				
				// 输出发送字符串
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				//myPrintf("%s",	s_ATcomm.pSendBuf);
				BSP_OSPutChar (DEF_DBG_UART,	s_ATcomm.pSendBuf,	s_ATcomm.Slen);
				#endif

				s_GSMcomm.CommandFlag	|=	ATCommandF;	// 置标志开始发送;
				
				loop=0;
				while(s_ATcomm.ReturnCode == 0)
				{
						gsmDelayMS(1);		// 2015-1-2 by:felix 提高匹配效率		
						loop++;
						if(loop >= (timeout*100*2))		// 2015-11-4 by:felix 增加超时监控防止一定几率导致的AT发送堵塞导致任务复位
								break;
				}	
				
				// 输出接收字符串
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
* Description    : 手动发送处理，防止自动处理发送前清buf导致的接受gprs URC遗漏         
* Input          : 
* Output         : 
* Return         : 返回:	DEF_GSM_NONE_ERR 成功, DEF_GSM_BUSY_ERR占用中, 
*													其他返回码(数值为1~255)不会与 DEF_GSM_NONE_ERR，DEF_GSM_BUSY_ERR冲突
*********************************************************************************************************
*/
s8		BSP_SendATcmdProExt (u8	checkMode,	u8	*pSendString,	u16	Slen,	u16	PendCode,	u32	timeout)
{
		u16	i=0, p=0;
		s8	retCode=DEF_GSM_NONE_ERR;
    u16 usCnt = s_GSMcomm.ps_commu->RCNT;
		if(s_GSMcomm.ActiveCmd == NoCmdActive)
		{		
				// 输出发送字符串
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 软件复位
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CFUN11,	strlen((const char *)AT_CFUN11),	CommandRecOk,	CFUN11Timeout) !=	DEF_GSM_NONE_ERR)
						{																
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_CFUN11_ERR;		
						}	
						else
						{
								BSP_GSM_ReleaseFlag(DEF_ALL_FLAG);						//  清除所有状态标志
								memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	//  清错误变量
								s_GSMcomm.GSMComStatus 	= GSM_POWOK;					//  置初始化命令开始发送状态
								s_GSMcomm.SetupPhase 		= CheckSimCardSend;		//  软件复位后需要退回到sim卡检测步骤
								s_GSMcomm.Timer					=	0;
								s_GSMcomm.HardWareSta		=	DEF_GSMHARD_NONE_ERR;	
								retCode = DEF_GSM_NONE_ERR;
						}					
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}

/*
*********************************************************************************************************
*                            					GSM TTS应用函数(由上层应用函数调用)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSPlayPro() 
* Description    : 播放TTS语音(开始播放后设置TTSPlayF,扫描函数检测到结束URC后复位该标志)        
* Input          : mode			:DEF_TTS_PLAY_ASCII or  DEF_TTS_PLAY_UCS2 
*                : * pString: UCS2码或ASCII码字符串
*                : len			: 字符串长度
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
																
								IO_SPAKER_PA_EN();	// 首先使能功放
								if(BSP_SendATcmdProExt(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CTTSTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_CTTS_ERR;			
								}
								else
								{
										BSP_GSM_SetFlag (DEF_TTSPLAY_FLAG);		// 设置TTS播放中标志，在扫描到URC结束后将自动清空
										retCode = DEF_GSM_NONE_ERR;			
								}							
						}
						else
								retCode	=	DEF_GSM_DATA_OVERFLOW;	// 语音数据长度溢出错误								
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSStopPro() 
* Description    : 停止播放TTS语音        
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_CTTS_ERR;			
						}
						else
						{					
								IO_SPAKER_PA_DIS();	// 关闭功放
								retCode = DEF_GSM_NONE_ERR;					
						}
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSCfgPro() 
* Description    : 配置TTS播放参数        
* Input          : vol		:音量大小（0~100,dft:50）
*                : mode		:播放模式（0~3,dft:0）
*                : pitch	:音调，如果高声音细尖（1~100,dft:50）
*                : speed	:语速（1~100,dft:50）
*                : channel:通道号，对于SIM800只有一个通道即通道0，其他值将返回错误（0~1,dft:0）
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
    								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSCfgRingPro() 
* Description    : 播放来电时使能或禁止TTS播放        
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_CTTSRING_ERR;			
						}
						else
								retCode = DEF_GSM_NONE_ERR;			
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}

/*
*********************************************************************************************************
*                            					GSM FS应用函数(由上层应用函数调用)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSCreateFilePro() 
* Description    : 创建文件，如果有该文件则直接返回成功（只支持在跟目录User目录下建立文件）      
* Input          : *pFile:文件名（例如：log.txt）
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 查询User目录是否存在	
						//	Send "AT+FSLS=C:\\r" AT_FSLS_C	
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_FSLS_C,	strlen((const char *)AT_FSLS_C),	CommandRecOk,	FSLSTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_FSLS_ERR;	
						}
						else						
						{
								j = 0;
								j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"User\\",s_GSMcomm.ps_commu->RCNT,5);
								if(j == 0xffff)		
								{
										// User目录不存在,建立目录
										//	Send "AT+FSMKDIR=C:\\User\\r" AT_FSMKDIR_H
		    						memset(tmpBuf,	0,	sizeof(tmpBuf));
		    						strcat((char *)tmpBuf,	(char *)AT_FSMKDIR_H);
		    						strcat((char *)tmpBuf,	"C:\\User\\r");
		
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSMKDIRTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
												retCode	=	DEF_GSM_FSMKDIR_ERR;	
        						}	
        						else
        						{       										       								
        								// 建立文件
            						//	Send "AT+FSCREATE=C:\\User\file.txt\r" AT_FSCREATE_H
            						memset(tmpBuf,	0,	sizeof(tmpBuf));
            						strcat((char *)tmpBuf,	(char *)AT_FSCREATE_H);
            						strcat((char *)tmpBuf,	"C:\\User\\");
            						strcat((char *)tmpBuf,	(char *)pFile);
            						strcat((char *)tmpBuf,	"\r");
            													
            						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSCREATETimeout) !=	DEF_GSM_NONE_ERR)
            						{
            								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
            								retCode	=	DEF_GSM_FSCREATE_ERR;			
            						}	
            						else
            								retCode = DEF_GSM_NONE_ERR;		// 文件建立成功!!										
        						} 																								
								}
								else
								{
										// User目录已经存在，查询文件是否存在	
										//	Send "AT+FSLS=C:\\User\\r" AT_FSLS_USER											
										if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_FSLS_USER,	strlen((const char *)AT_FSLS_USER),	CommandRecOk,	FSLSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
												retCode	=	DEF_GSM_FSLS_ERR;	
										}
										else
										{
												j = 0;
												j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)pFile, s_GSMcomm.ps_commu->RCNT, strlen((const char *)pFile));
												if(j == 0xffff)		
												{
														// 建立文件
                						//	Send "AT+FSCREATE=C:\\User\file.txt\r" AT_FSCREATE_H
                						memset(tmpBuf,	0,	sizeof(tmpBuf));
                						strcat((char *)tmpBuf,	(char *)AT_FSCREATE_H);
                						strcat((char *)tmpBuf,	"C:\\User\\");
                						strcat((char *)tmpBuf,	(char *)pFile);
                						strcat((char *)tmpBuf,	"\r");
                													
                						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSCREATETimeout) !=	DEF_GSM_NONE_ERR)
                						{
                								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
                								retCode	=	DEF_GSM_FSCREATE_ERR;			
                						}	
                						else
                								retCode = DEF_GSM_NONE_ERR;		// 文件建立成功!!		
												}
												else
														retCode = DEF_GSM_NONE_ERR;		// 文件已经存在直接返回
										}
								}									
						}
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSDelFilePro() 
* Description    : 删除文件（只支持在跟目录User目录下删除文件）
* Input          : *pFile:文件名（例如：log.txt）
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 不查询文件直接删除
						//	Send "AT+FSDEL=C:\\User\file.txt\r" AT_FSDEL_H												
						memset(tmpBuf,	0,	sizeof(tmpBuf));
						strcat((char *)tmpBuf,	(char *)AT_FSDEL_H);
						strcat((char *)tmpBuf,	(char *)pFile);
						strcat((char *)tmpBuf,	"\r");													
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSDELTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_FSDEL_ERR;			
						}	
						else
								retCode = DEF_GSM_NONE_ERR;		// 文件删除成功			
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSReadFilePro() 
* Description    : 读取文件（只支持在跟目录User目录下读取文件）
* Input          : *pFile	:文件名（例如：log.txt）
*								 : posi		:读取偏移量（单位：字节）
*								 : *rData	:读取到的数据指针
*								 : reqLen	:请求读取的文件长度（长度不能超过GSM底层读取缓冲区大小）
*								 : *retLen:实际读取的文件长度
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						retCode	=	DEF_GSM_DATA_OVERFLOW;		// 超出GSM接收缓冲区大小不予处理
				else
				{						
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{						
								// 组包
								//	Send "AT+FSREAD=C:\\User\file,0/1,len,posi\r" AT_FSREAD_H												
								memset(tmpBuf,	0,	sizeof(tmpBuf));
								memset(tmpBuf1,	0,	sizeof(tmpBuf1));
								strcat((char *)tmpBuf,	(char *)AT_FSREAD_H);
								strcat((char *)tmpBuf,	"C:\\User\\");
								strcat((char *)tmpBuf,	(char *)pFile);
								
								if(posi == 0) 
										sprintf((char *)tmpBuf1,	",0,%d,0\r",	reqLen);				// 从文件起始位置读取
								else
										sprintf((char *)tmpBuf1,	",1,%d,%d\r",	reqLen,posi);	// 从文件posi偏移位置读取
								
								strcat((char *)tmpBuf,	(char *)tmpBuf1);												
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSREADTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_FSREAD_ERR;			
								}	
								else
								{
										// 复制数据内容(不能判断回车换行作为结束条件，写的过程中会自动给每条记录增加回车换行)
										for(j=0;	(*(s_GSMcomm.pRecBuf + j) != 'O') && (*(s_GSMcomm.pRecBuf + j) != 'K') && (j < s_GSMcomm.ps_commu->RCNT);	j++)
										{
												*(rData + j) = *(s_GSMcomm.pRecBuf + j);
										}
										*retLen = j;	// 获取实际长度
										retCode = DEF_GSM_NONE_ERR;		// 文件读取成功	
								}		
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSWriteFilePro() 
* Description    : 写文件（只支持在跟目录User目录下写文件，自动写到文件结尾处，如果空间不足则自动跳到文件头写覆盖最早的文件部分）
*                : 写每条记录时会自动为每条记录增加回车换行，下次再次写入时在新的一行插入无需手动添加回车换行
*								 : 文件的可用空间决定于模块固件版本，如果为TTS版本的则大一些，如果为蓝牙版本则小一些；最小大约60kByte
* Input          : *pFile	:文件名（例如：log.txt）
*								 : *wData	:写数据缓冲区指针
*								 : wLen		:写的数据长度
*								 : wTimeOut:写超时（单位s必须小于“写超时”才有效，否则程序在没写完前就超时返回了）
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						retCode	=	DEF_GSM_DATA_OVERFLOW;		// 超出GSM发送缓冲区大小不予处理
				else
				{					
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{						
								// 首先判断可用空间大小
								//	Send "AT+FSMEM\r"	AT_FSMEM
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_FSMEM,	strlen((const char *)AT_FSMEM),	CommandRecOk,	FSMEMTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_FSMEM_ERR;			
								}	
								else
								{
										j = 0;
										j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+FSMEM: ", s_GSMcomm.ps_commu->RCNT, 8);
										if(j == 0xffff)	
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
												retCode = DEF_GSM_NONE_ERR;		// 已经存在文件直接返回	
										}
										else
										{
												// 获取长度信息
												j = j + 10; // 指向空间大小
												memset(tmpBuf1,'\0',sizeof(tmpBuf1));
												for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != 'b') && (k < sizeof(tmpBuf1)) && (k < s_GSMcomm.ps_commu->RCNT);	k++)
												{
														tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
												}
												tmpBuf1[k] = '\0';
												avaLen = atoi((const char *)tmpBuf1);
												
												// 组包
												//	Send "AT+FSWRITE=C:\\User\file.txt,0/1,len,timeout\r" AT_FSWRITE_H
												memset(tmpBuf,	0,	sizeof(tmpBuf));
												memset(tmpBuf1,	0,	sizeof(tmpBuf1));
												strcat((char *)tmpBuf,	(char *)AT_FSWRITE_H);
												strcat((char *)tmpBuf,	"C:\\User\\");
												strcat((char *)tmpBuf,	(char *)pFile);
												// 判断请求写入长度是否合法
												if((wLen + 1000) >= avaLen)	
														strcat((char *)tmpBuf,	",0");	// 如果文件溢出跳到开头写入覆盖最开始的信息
												else
														strcat((char *)tmpBuf,	",1");	// 从文件结尾写入
												
												sprintf((char *)tmpBuf1,	",%d,%d\r",	wLen,wTimeOut);	
												strcat((char *)tmpBuf,	(char *)tmpBuf1);								
												
												// Pend">"							
												if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecFSWriteAck,	FSWRITETimeout) !=	DEF_GSM_NONE_ERR)	//暂时固定超时为FSWRITETimeout
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
														retCode	=	DEF_GSM_FSWRITE_ERR;			
												}	
												else
												{
														//	Send data
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)wData,	wLen,	CommandRecOk,	FSWRITECPLTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);	// 错误累加
																retCode	=	DEF_GSM_FSWRITECPL_ERR;
														}	
														else
																retCode = DEF_GSM_NONE_ERR;		// 文件写入成功!!		
												}															
										}										
								}									
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
*                            					GSM BT应用函数(由上层应用函数调用)     
*********************************************************************************************************
*/


/*
*********************************************************************************************************
* Function Name  :									  BSP_BTInitPro() 
* Description    : 蓝牙初始化，参数配置等
* Input          : *pName	:文件名（例如：Y05D-105）最长18char
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 打开蓝牙电源(该条指令不检测返回码，如果之前蓝牙电源为开启状态则会返回"ERROR"也认为电源已经使能了)
						// Send "AT+BTPOWER=1\r"
						BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTPOWER_1,	strlen((const char *)AT_BTPOWER_1),	CommandRecOk,	BTPOWERTimeout);
						gsmDelayMS(1000);	
					
						// 配置蓝牙主机名称
						// Send "AT+BTHOST=name\r"	AT_BTHOST_H
						memset(tmpBuf,	0,	sizeof(tmpBuf));
        		strcat((char *)tmpBuf,	(char *)AT_BTHOST_H);
        		strcat((char *)tmpBuf,	(char *)pName);
						strcat((char *)tmpBuf,	"\r");
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	BTHOSTTimeout)	!=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	= DEF_GSM_BTHOST_ERR;	
						}	
						else
						{
								gsmDelayMS(500);	
								// 配置蓝牙可见性	
								// Send "AT+BTVIS=1\r"	AT_BTVIS_1
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTVIS_1,	strlen((const char *)AT_BTVIS_1),	CommandRecOk,	BTVISTimeout)	!=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	= DEF_GSM_BTVIS_ERR;	
								}	
								else
								{
										// 配置蓝牙接收数据模式为手动模式
										// Send "AT+BTSPPGET=1\r"	AT_BTSPPGET_1	
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTSPPGET_1,	strlen((const char *)AT_BTSPPGET_1),	CommandRecOk,	BTSPPGET1Timeout)	!=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;				
		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTPairAckPro() 
* Description    : 接收其它客户端发来的匹配请求
* Input          : mode		:0-数字认证，1-Passkey 认证
*                : *pPass	:如果模式为1则需要提供密码（长度4~16字节）
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 接收匹配
						if(mode == 0)
						{
								// 数字匹配
								// Send "AT+BTPAIR=1,1\r" AT_BTPAIR_1
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTPAIR_1,	strlen((const char *)AT_BTPAIR_1),	CommandRecOk,	BTPAIRTimeout)	!=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	= DEF_GSM_BTPAIR_ERR;	
								}	
								else
										retCode = DEF_GSM_NONE_ERR;
						}
						else
						{
								// Passkey匹配
								// Send "AT+BTPAIR=2,Passkey\r" AT_BTPAIR_2_H
								memset(tmpBuf,	0,	sizeof(tmpBuf));
        				strcat((char *)tmpBuf,	(char *)AT_BTPAIR_2_H);
        				strcat((char *)tmpBuf,	(char *)pPass);
        				strcat((char *)tmpBuf,	"\r");
								
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	BTPAIRTimeout)	!=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;				
		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTConnectAckPro() 
* Description    : 接收连接请求
* Input          : en	: DEF_DISABLE-禁止连接;DEF_ENABLE-允许连接 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTConnectAckPro	(u8 en,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 接收连接请求
						// Send "AT+BTACPT=1\r" AT_BTACPT_1
						if(en == DEF_DISABLE)
						{
    						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTACPT_0,	strlen((const char *)AT_BTACPT_0),	CommandRecOk,	BTACPTTimeout)	!=	DEF_GSM_NONE_ERR)
    						{
    								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
    								retCode	= DEF_GSM_BTACPT_ERR;	
    						}	
    						else
								{
    								BSP_GSM_ReleaseFlag(DEF_BTDING_FLAG);					// 设置蓝牙离线标志
										retCode = DEF_GSM_NONE_ERR;		
								}
    				}		
    				else
    				{
    						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTACPT_1,	strlen((const char *)AT_BTACPT_1),	CommandRecOk,	BTACPTTimeout)	!=	DEF_GSM_NONE_ERR)
    						{
    								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
    								retCode	= DEF_GSM_BTACPT_ERR;	
    						}	
    						else
								{
    								BSP_GSM_SetFlag(DEF_BTDING_FLAG);							// 设置蓝牙在线标志
										retCode = DEF_GSM_NONE_ERR;	
								}
    				}		
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;				
		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTRxdDataPro() 
* Description    : 获取BT数据
* Input          : *pBuf	:接收到的数据缓冲区指针
*								 : *len		:接收到的数据长度
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 获取数据长度信息
						//	Send "AT+BTSPPGET=2\r" AT_BTSPPGET_2		
						tcpReadMuxtex =1;	// 置互斥标志,使URC检测挂起
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_BTSPPGET_2,	strlen((const char *)AT_BTSPPGET_2),	CommandRecOk,	BTSPPGET2Timeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_BTSPPGET2_ERR;			
						}	
						else
						{
								gsmDelayMS(30);		// 等30ms接收数据完成(防止接收HEX数据中含‘OK’字符串导致匹配成功提前退出)
								//+BTSPPGET: 1,30
								j = 0;
								j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+BTSPPGET: ", s_GSMcomm.ps_commu->RCNT, 11);
								if(j == 0xffff)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode = DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										// 获取长度信息
										j = j + 13; // 指向长度信息
										memset(tmpBuf1,'\0',sizeof(tmpBuf1));
										for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < sizeof(tmpBuf1)) && (k < s_GSMcomm.ps_commu->RCNT);	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k] = '\0';
										getLen = atoi((const char *)tmpBuf1);
										
										// 获取数据组包
										//	Send "AT+BTSPPGET=3,len\r" AT_BTSPPGET_3_H
										memset(tmpBuf,	'\0',	sizeof(tmpBuf));
        						strcat((char *)tmpBuf,	(char *)AT_BTSPPGET_3_H);
        						strcat((char *)tmpBuf,	(char *)tmpBuf1);
        						strcat((char *)tmpBuf,	"\r");
        						
										// Read							
        						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	BTSPPGET3Timeout) !=	DEF_GSM_NONE_ERR)	
        						{
        								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
        								retCode	=	DEF_GSM_BTSPPGET3_ERR;			
        						}	
        						else
        						{
        								//+BTSPPGET: 1,30,SIMCOMSPPFORAPPSIMCOMSPPFORAPP
        								j = 0;
												j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+BTSPPGET: ", s_GSMcomm.ps_commu->RCNT, 11);
												if(j == 0xffff)	
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
														retCode = DEF_GSM_UNKNOW_ERR;
												}
												else
												{
														// 获取数据信息
														j = j + 16; // 指向数据信息
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
						// 读取完毕后清除接收数据内容,防止URC误检测
						memset(RBufAT,	0,	sizeof(RBufAT));
						memset(RBufGPRS,	0,	sizeof(RBufGPRS));
						s_GSMcomm.ps_commu->RCNT =0;
						tcpReadMuxtex =0;	// 清标志,使能重新URC检测									
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTSendDataPro() 
* Description    : 发送BT数据
* Input          : *pBuf	:需要发送的数据缓冲区指针
*								 : len		:发送数据长度
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 查询当前工作模式（客户端模式/服务器模式）
						//	Send "AT+BTSPPCFG?\r" AT_BTSPPCFG																						
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_BTSPPCFG,	strlen((const char *)AT_BTSPPCFG),	CommandRecOk,	BTSPPCFGTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_BTSPPCFG_ERR;			
						}	
						else
						{
								j = 0;
								j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+BTSPPCFG:", s_GSMcomm.ps_commu->RCNT, 10);
								if(j == 0xffff)		
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode = DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										// 获取模式信息
										j = j + 15; // 指向长度信息
										if(*(s_GSMcomm.pRecBuf + j) != '1')
												retCode	=	DEF_GSM_FAIL_ERR;		// 模式不正确返回错误，不做故障累加
										else
										{
												// 组包
												//	Send "AT+BTSPPSEND=len" AT_BTSPPSEND_H												
												memset(tmpBuf,	'\0',	sizeof(tmpBuf));
												memset(tmpBuf1,	'\0',	sizeof(tmpBuf1));
		        						strcat((char *)tmpBuf,	(char *)AT_BTSPPSEND_H);		        						
		        						sprintf((char *)tmpBuf1,	"%d\r",	len);	
		        						strcat((char *)tmpBuf,	(char *)tmpBuf1);								
		        						
		        						// Pend">"							
		        						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecBTSendAck,	BTSPPSENDTimeout) !=	DEF_GSM_NONE_ERR)	
		        						{
		        								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
		        								retCode	=	DEF_GSM_BTSPPSEND_ERR;			
		        						}	
		        						else
		        						{
		        								//	Send data
														if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)pBuf,	len,	RecBTSendCpl,	BTSPPSENDCPLTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);	// 错误累加
																retCode	=	DEF_GSM_BTSPPSENDCPL_ERR;
														}	
		        								else
		        										retCode = DEF_GSM_NONE_ERR;		// 文件写入成功!!		
		        						}																					
										}
								}
						}		
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
*                            					GSM上层应用函数-状态查询部分(由OS 应用函数调用)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_QCSQCodePro() 
* Description    : 查询信号强度及误码率        
* Input          : 如果信号强度为99(表示无法获取)直接写0
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// Send "AT+CSQ"	首先检测信号强度
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CSQ,	strlen((const char *)AT_CSQ),	CommandRecOk,	CSQTimeout)	!=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	= DEF_GSM_CSQ_ERR;	
						}	
						else
						{
								j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CSQ: ",s_GSMcomm.ps_commu->RCNT,6);
								if(j == 0xffff)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_UNKNOW_ERR;	
								}
								else
								{
										j = j + 6;
										// 取信号强度
										memset(tmpBuf,'\0',sizeof(tmpBuf));
										for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != ',') && (k <= 1);	k++)
										{
												tmpBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										*pSLevel = atoi((const char *)tmpBuf);
										if(*pSLevel == 99)
												*pSLevel = 0;
										
										// 取误码率	
										j = j + k + 1;
										memset(tmpBuf,'\0',sizeof(tmpBuf));	
										for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != '\r') && (k <= 1);	k++)
										{
												tmpBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										*pRssi = atoi((const char *)tmpBuf);	// 取误码率	
										retCode = DEF_GSM_NONE_ERR;
								}											
						}
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QCellGPSPosiPro() 
* Description    : 查询设备基站定位信息(最大超时时间60s)       
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 需要更新指令
						/*
						//	Send "AT^SMOND"
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_SMOND,	strlen((const char *)AT_SMOND),	CommandRecOk,	SMONDTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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

										// 取移动国家代码(MCC)
										j = j+7;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 3));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';										
										strcpy((char *)pMCCBuf,	(const char *)tmpBuf1);

										// 取移动运营商代码(MNC)
										j = j+k+1;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 3));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';
										strcpy((char *)pMNCBuf,	(const char *)tmpBuf1);

										// 取小区号码(LAC)
										j = j+k+1;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 4));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';
										strcpy((char *)pLACBuf,	(const char *)tmpBuf1);
										
										// 取基站号(CELL)
										j = j+k+1;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 4));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';
										strcpy((char *)pCELLBuf,	(const char *)tmpBuf1);
										
										
										// 取信号强度及误码率
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT - j));	k++,j++)
										{
										 		;
										}
										j = j+k-1;
										// 移动到信号强度参数的首位置
										for(;	(*(s_GSMcomm.pRecBuf + j) != ',');	j--)
										{
												;
										}
										j = j+1;
										// 复制
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
										// 复制
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 2));	k++)
										{
												*(pSLevelBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
										}
										*(pSLevelBuf + k) = '\0';
										if((*pSLevelBuf == '9') && (*(pSLevelBuf + 1) == '9'))
										{
												*pSLevelBuf 		 	= '\0';		// 信号强度清空
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QSimCardPrintPro() 
* Description    : 查询SIM卡印刷号(一般为20位但有些卡的位数不同)         
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// SIM800 不支持该命令
						/*
						//	Send "AT^SCID"
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_SCID,	strlen((const char *)AT_SCID),	CommandRecOk,	SCIDTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
										if((k < 4) || (k > 30))		// 长度判断长度非法则清空接收区数据
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	

}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QIMSICodePro() 
* Description    : 查询网络移动设备签署(IMSI)号码(一般为15位)         
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_CIMI_ERR;			
						}
						else
						{								
								for(k = 0;	((*(s_GSMcomm.pRecBuf + k + 2) != '\r') && (*(s_GSMcomm.pRecBuf + k + 2) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
								{
										*(pImsiBuf +k) = *(s_GSMcomm.pRecBuf + k + 2);
								}
								*(pImsiBuf +k) = '\0';		
								if((k < 8) || (k > 30))		// IMSI长度判断长度非法则清空接收区数据
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QIMEICodePro() 
* Description    : 查询国际移动台设备识别码(IMEI)(一般为15位)        
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_GSN_ERR;		
						}	
						else
						{								
								for(k = 0;	((*(s_GSMcomm.pRecBuf + k + 2) != '\r') && (*(s_GSMcomm.pRecBuf + k + 2) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
								{
										*(pImeiBuf +k) = *(s_GSMcomm.pRecBuf + k + 2);
								}
								*(pImeiBuf +k) = '\0';
								if((k < 8) || (k > 30))		// IMEI长度判断长度非法则清空接收区数据
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QCPINStatePro() 
* Description    :  查询SIM卡状态       
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 查询卡状态
						ret =BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
						if(ret !=	DEF_GSM_NONE_ERR)
						{
								if(ret == CommandTimeout)
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 只有超时才认为是错误
								retCode	=	DEF_GSM_CPIN_ERR;
						}
						else
								retCode = DEF_GSM_NONE_ERR;
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_CSCA_ERR;		
						}	
						else
						{
								p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CSCA: \"",s_GSMcomm.ps_commu->RCNT,8);
								if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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

										// 写短信中心号码
										strcpy((char *)pSmsCenter,	(const char *)tmpCSCA);
										retCode = DEF_GSM_NONE_ERR;	
								}
						}	
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QSimCardNumPro() 
* Description    : 查询本机号码(SIM卡必须之前写过SIM卡号码才能读出否则为空字符)    
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_CNUM_ERR;		
						}	
						else
						{	
								// +CNUM: "","+18501191946",145,7,4
								p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)",\"",s_GSMcomm.ps_commu->RCNT,2);
								if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
								{
										// 返回空字符串
										*pSimNum ='\0';
										retCode	=	DEF_GSM_NONE_ERR;										
								}
								else
								{
										p +=2;	// 指向号码
										for(i=0;	(i<19) && (*(s_GSMcomm.pRecBuf + p) != '"');	i++)
										{
												if(*(s_GSMcomm.pRecBuf + p) != '1')		
														p +=1;	// 跳过非手机号码起始字段如"+86"
												else
														break;	
										}
										for(k = 0;	((k < 19) && (*(s_GSMcomm.pRecBuf + p + k) != '\"') && (*(s_GSMcomm.pRecBuf + p + k) != '\r') && (*(s_GSMcomm.pRecBuf + p + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
										{
												*(pSimNum + k) = *(s_GSMcomm.pRecBuf + p + k);	// 不复制'+'号							
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_WriteSimCardNumPro() 
* Description    : 设置本机号码,将本机号码写入SIM卡电话薄中，之后通过AT+CNUM可查询本机号码
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 首先使能CNUM查询本机号码
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CPBS_ON,	strlen((const char *)AT_CPBS_ON),	CommandRecOk,	CPBSTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_CPBS_ERR;		
						}	
						else
						{	
								// 将本机号码写入SIM卡电话薄
								// 组包
								//	Send "AT+CPBW=1,\"13145678901\",145"												
								memset(tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CPBW_H);		        						
								strcat((char *)tmpBuf,	(char *)pSimNum);	
								strcat((char *)tmpBuf,	(char *)"\",145\r");
														
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CPBWTimeout) !=	DEF_GSM_NONE_ERR)	
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
*                            					GSM上层应用函数-SIM卡初始化部分(由OS 应用函数调用)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMSimCardInitPro() 
* Description    :  SIM卡相关功能初始化函数     
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 查询当前SIM卡状态
						// Send "AT+CPIN?"
						ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
						if(ret !=	DEF_GSM_NONE_ERR)
						{
								if(ret == CommandTimeout)
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 只有超时才认为是错误
								retCode	=	DEF_GSM_CPIN_ERR;
						}
						else
						{
								// SIM卡初始化命令
							  //AT_CMGF,			// 需要SIM卡支持								
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF,	strlen((const char *)AT_CMGF),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_CMGF_ERR;
								}
								else
								{
										//AT_CSCS_GSM,	// 需要SIM卡支持								
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
												retCode	=	DEF_GSM_CSCS_ERR;
										}
										else
										{
												//AT_CNMI,			// 需要SIM卡支持								
												if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CNMI,	strlen((const char *)AT_CNMI),	CommandRecOk,	CNMITimeout) !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
														retCode	=	DEF_GSM_CNMI_ERR;
												}
												else
												{
														//AT_CPMS,			// 需要SIM卡支持
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CPMS,	strlen((const char *)AT_CPMS),	CommandRecOk,	CPMSTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																retCode	=	DEF_GSM_CPMS_ERR;
														}
														else
														{
																// 读取SIM卡号码
																if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CNUM,	strlen((const char *)AT_CNUM),	CommandRecOk,	CNUMTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																		retCode	=	DEF_GSM_CNUM_ERR;		
																}	
																else
																{	
																		// +CNUM: "","+18501191946",145,7,4
																		p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)",\"",s_GSMcomm.ps_commu->RCNT,2);
																		if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																		{
																				// 返回空字符串
																				*pSimNum ='\0';
																				retCode	=	DEF_GSM_NONE_ERR;										
																		}
																		else
																		{
																				p +=2;	// 指向号码		
																				if(*(s_GSMcomm.pRecBuf + p) == '+')		
																						p +=1;	// 跳过非手机号码起始字段如"+"																			
																				for(k = 0;	((k < 19) && (*(s_GSMcomm.pRecBuf + p + k) != '\"') && (*(s_GSMcomm.pRecBuf + p + k) != '\r') && (*(s_GSMcomm.pRecBuf + p + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
																				{
																						*(pSimNum + k) = *(s_GSMcomm.pRecBuf + p + k);	// 不复制'+'号							
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
*                            					GSM上层应用函数-网络部分(由OS 应用函数调用)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  :									  BSP_GPRSFroceBreakSet() 
* Description    :  手动强制设置为网络断开状态，触发网络自动重新连接      
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GPRSFroceBreakSet(void)
{
		s_GSMcomm.GSMComStatus	=	GSM_ONCMD;
		BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);		// 置网络断开状态
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GPRSResetPro() 
* Description    :  复位GPRS网络使其重新附着到GPRS网络上(如果多次两网尝试失败需要复位GPRS网络)        
* Input          : *pApn	: APN字符串缓冲区指针，不可以为空
*								 : *pUser : 登录用户名字符串指针，可以为空
*								 : *pPass : 登录密码字符串指针，可以为空
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
				if(*pApn == '\0')
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{			
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								// 查询当前SIM卡状态
								// Send "AT+CPIN?"
								ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
								if(ret !=	DEF_GSM_NONE_ERR)
								{
										if(ret == CommandTimeout)
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 只有超时才认为是错误
										retCode	=	DEF_GSM_CPIN_ERR;
								}
								else
								{
										// 查询当前网络注册状态是否可用	
										//	Send "AT+CREG?"
										if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CREG,	strlen((const char *)AT_CREG),	CommandRecOk,	CREGTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 查询网络注册错误
												retCode	=	DEF_GSM_CREG_ERR;
										}
										else
										{
												j= 0;
												j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CREG: *,*",s_GSMcomm.ps_commu->RCNT,10);
												if(j != 0xffff)	
												{
														j =j+9; // 指向注册状态
														if((*(s_GSMcomm.pRecBuf + j) == '1') || (*(s_GSMcomm.pRecBuf + j) == '5'))
																cregOKFlag =1;	// 注册成功！！			
														else
																cregOKFlag =0;
												}
												else
														cregOKFlag =0;
										}
										
										// 查询当前GPRS附着情况
										if(cregOKFlag == 1)
										{												
												if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CGATT,	strlen((const char *)AT_CGATT),	CommandRecOk,	CGATTTimeout) !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// GPRS查询错误
														retCode	=	DEF_GSM_CGATT_ERR;
												}
												else
												{
														j= 0;
														j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CGATT: *",s_GSMcomm.ps_commu->RCNT,9);
														if(j != 0xffff)	
														{
																j =j+8; // 指向附着状态
																if(*(s_GSMcomm.pRecBuf + j) == '1')
																		cgattOKFlag =1;	// 注册成功！！	
																else
																		cgattOKFlag =0;	
														}
														else
																cgattOKFlag =0;
												}
										}
										
										// 开始激活GPRS
										if((cregOKFlag == 0) || (cgattOKFlag == 0))
										{		
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加(2015-3-10，测试发现有时CGATT长时间返回0状态不改变)
												if(cregOKFlag == 0)
														retCode	=	DEF_GSM_CREG_ERR;			
												else
														retCode	=	DEF_GSM_CGATT_ERR;	
										}
										else
										{											
												// 首先断开当前PDP上下文(尝试3次)
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
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
														retCode	=	DEF_GSM_CIPSHUT_ERR;
												}
												else
												{																											
														s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// 置GSM端口状态
														BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	// 更新当前联网状态为断开连接
																																							
														// 开始GPRS激活流程
														// 首先使能多链路模式
														//	Send "AT+CIPMUX=1\r"
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIPMUX_1,	strlen((const char *)AT_CIPMUX_1),	CommandRecOk,	CIPMUXTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																retCode	=	DEF_GSM_CIPMUX_ERR;
														}		
														else
														{
																// 配置接收数据模式手动模式（该指令必须在GPRS上下文激活前配置）
																//	Send "AT+CIPRXGET=1\r"
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIPRXGET_1,	strlen((const char *)AT_CIPRXGET_1),	CommandRecOk,	CIPRXGET1Timeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																		retCode	=	DEF_GSM_CIPRXGET1_ERR;
																}		
																else
																{
																		// 配置接收URC格式
																		//	Send "AT+CIPHEAD=1\r"
																		if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIPHEAD_1,	strlen((const char *)AT_CIPHEAD_1),	CommandRecOk,	CIPHEADTimeout) !=	DEF_GSM_NONE_ERR)
																		{
																				BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																				retCode	=	DEF_GSM_CIPHEAD_ERR;
																		}
																		else
																		{																			
																				// 配置APN,USER,PASS
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
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																						retCode	=	DEF_GSM_CSTT_ERR;
																				}
																				else
																				{
																						// 激活PDP上下文
																						//	Send "AT+CIICR\r"
																						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIICR,	strlen((const char *)AT_CIICR),	CommandRecOk,	CIICRTimeout) !=	DEF_GSM_NONE_ERR)
																						{
																								//BSP_GSM_ERR_Add(&err_Gsm.cnnErrorTimes,	1);		// 网络错误累加
																								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	20);		// 2015-11-19该错误合并到通用错误中
																								retCode	=	DEF_GSM_CIICR_ERR;
																						}
																						else
																						{	
																								// 获取本地IP（该指令不全字节匹配结果码）
																								//	Send "AT+CIFSR\r"
																								if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CIFSR,	strlen((const char *)AT_CIFSR),	RecCIFSRAck,	CIFSRTimeout) !=	DEF_GSM_NONE_ERR)
																								{
																										//BSP_GSM_ERR_Add(&err_Gsm.cnnErrorTimes,	1);		// 网络错误累加
																										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 2015-11-19该错误合并到通用错误中
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPConnectPro() 
* Description    : sCnn		: 通道号（‘0’~‘5’共6路连接，字符类型）
*								 : *pIp		: IP地址字符串指针
*								 : *pPort	: 端口号字符串指针         
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
				if(((sCnn - 0x30) > DEF_GPRS_CNN_MAX) || (*pIp == '\0') || (*pPort == '\0'))
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{				
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{																																	
								// 组包
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
										// 联网错误处理	
										BSP_GSM_ERR_Add(&err_Gsm.cnnErrorTimes,	1);		// 错误累加
										if(BSP_GSM_ERR_Que(&err_Gsm.cnnErrorTimes) >= DEF_GSM_CNN_TIMES)
										{
												BSP_GSM_ERR_Clr(&err_Gsm.cnnErrorTimes);		// 错误清空	
												
												// 需要重新附着GPRS网络(AT+CGATT=0，AT+CGATT=1)
												tmpRen = BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CGATT_0,	strlen((const char *)AT_CGATT_0),	CommandRecOk,	CGATT0Timeout); 
												if(tmpRen !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
														retCode	=	DEF_GSM_CGATT_ERR;				
												}
												else
												{
														tmpRen = BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CGATT_1,	strlen((const char *)AT_CGATT_1),	CommandRecOk,	CGATT1Timeout); 
														if(tmpRen !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																retCode	=	DEF_GSM_CGATT_ERR;				
														}
												}
										}	

										s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// 置GSM端口状态
										BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);
										retCode	=	DEF_GSM_CIPSTART_ERR;
								}
								else
								{										
										BSP_GSM_ERR_Clr(&err_Gsm.cnnErrorTimes);		// 错误清空				
										s_GSMcomm.GSMComStatus	=	GSM_ONLINE;	// 置GSM端口状态
										BSP_GSM_SetFlag(DEF_GDATA_FLAG);																									
										retCode	=	DEF_GSM_NONE_ERR;
								}
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPCloseNetPro() 
* Description    : sCnn		: 通道号（‘0’~‘5’共6路连接，字符类型）      
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{																							
								// 组包
								//	Send "AT+CIPCLOSE=n\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPCLOSE_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);	
								strcat((char *)tmpBuf,	(char *)"\r");
												
								if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCloseCpl,	CIPCLOSETimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_CIPCLOSE_ERR;
										
										// 可以考虑如果几次断网不成功直接考虑断开PDP连接
								}
								else
								{
										s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// 置GSM端口状态
										BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	
										retCode	=	DEF_GSM_NONE_ERR;		
								}				
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
						retCode	=	DEF_GSM_CIPSHUT_ERR;
				}
				else
				{
						s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// 置GSM端口状态
						BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	
						retCode	=	DEF_GSM_NONE_ERR;		
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPSendData() 
* Description    : sCnn		: 通道号（‘0’~‘5’共6路连接，字符类型）
*								 : *pBuf	: 发送数据缓冲区指针     
*								 : len		: 发送长度    
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{					
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	==	GSM_ONLINE))
						{								
								// 查询可发送的数据长度
								// Send "AT+CIPSEND?\r"
								// 返回格式为（多路模式）：
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
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
											  		j =j+10;	// 取通道号
											  		if(sCnn == *(s_GSMcomm.pRecBuf + j))
											  		{
											  				j =j+2;	// 取可发送数据大小
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
											  				// 偏移检索指针到下一行
											  				for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
																{
																		;
																}
																posi = posi + k + 2; // 移动数据接收区buf指针											  			
											  		}
											  }
											  else
											  		break;
										}
										if(i > DEF_GPRS_CNN_MAX)
										{
												;		// 异常错误，通道都不匹配！！！！！														
										}
								}
								*/
								
								ready2Send =1;	// 取消发送缓冲区查询	
								// 开发发送数据//////////////////////////////////////////////
								if(ready2Send != 1)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 异常错误，通道都不匹配！！！！�
										s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	
										BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);					// 断开网络,置状态重新连接服务器										�				
										retCode =	DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										// 这里需要首先确认可以发送的数据长度是多少
										// 组包								
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
												// 服务器断开,置状态重新连接服务器
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加														
														s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	
														BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	// 服务器断开,置状态重新连接服务器
														retCode	= DEF_GSM_CIPSENDF_ERR;
												}
												else
												{
														// 发送数据成功将复位通用错误累加计数器
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPRxdData() 
* Description    : sCnn		: 通道号（‘0’~‘5’共6路连接，字符类型）改写tcpReadMuxtex变量状态
*								 : *pBuf	: 接收数据缓冲区指针     
*								 : len		: 接收长度             
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{												
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	== GSM_ONLINE))
						{						
								// 组包直接读取数据
								//	Send "AT+CIPRXGET=2,1,1000\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPRXGET_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);	
								strcat((char *)tmpBuf,	(char *)",");	
								sprintf((char *)tmpLenBuf,	"%d",	tmpLen);	// 由头文件宏定义决定DEF_GPRS_RXDONE_SIZE						
								strcat((char *)tmpBuf,	(char *)tmpLenBuf);
								strcat((char *)tmpBuf,	(char *)"\r");		
																				
								tcpReadMuxtex =1;	// 置互斥标志,使URC检测挂起
								
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CIPRXGET2Timeout) != DEF_GSM_NONE_ERR) 
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_CIPGET2_ERR;	
								}
								else
								{
										j = 0;
										j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 2,*",s_GSMcomm.ps_commu->RCNT,14);
										if(j == 0xffff)		
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
												retCode	= DEF_GSM_UNKNOW_ERR;											
										}
										else
										{
												// 取读到的数据长度(+CIPRXGET: 2,0,6,0)
												memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
												j = j + 15;
												for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
												{
														tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
												}
												tmpLenBuf[k] = '\0';
												tmpLen = atoi((const char *)tmpLenBuf);
												
												// 取剩余读取的字节长度
												j = j + k + 1;	// 跳过','
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
												
												// 复制数据
												j = j + k + 2;	// 跳过回车换行指向数据区
												// 判断结束条件有不能为0d 0a如果为hex数据格式，数据流中可能有0d 0a所以出现数据复制不全情况
												for(k =0;	((k < tmpLen) && (k < DEF_GPRS_RXDONE_SIZE));	k++)
												{
														*(pBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
												}
												*(pBuf + k) = '\0';	
												*len = tmpLen;	
												retCode	= DEF_GSM_NONE_ERR;
										}					
								}	
								// 读取完毕后清除接收数据内容,防止URC误检测				
								tcpReadMuxtex =0;	// 清标志,使能重新URC检测						
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPRxdDataUpdata() 
* Description    : sCnn		: 通道号（‘0’~‘5’共6路连接，字符类型）改写tcpReadMuxtex变量状态,用于升级过程中大量数据的接收
*								 : *pBuf	: 接收数据缓冲区指针     
*								 : len		: 接收长度             
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{												
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	== GSM_ONLINE))
						{	
								// 先查询收到的数据长度
								//	Send "AT+CIPRXGET=4,id\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPRXGET4_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);	
								strcat((char *)tmpBuf,	(char *)"\r");	
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CIPRXGET2Timeout) != DEF_GSM_NONE_ERR) 
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_CIPGET2_ERR;	
								}
								else
								{										
										j = 0;
										j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 4,*",s_GSMcomm.ps_commu->RCNT,14);
										if(j == 0xffff)		
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
												retCode	= DEF_GSM_UNKNOW_ERR;													
										}
										else
										{
												// 取读到的数据长度(+CIPRXGET: 4,0,len)
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
												
												// 组包直接读取数据
												//	Send "AT+CIPRXGET=2,1,1000\r"
												tmpLen=DEF_GPRS_RXDONE_SIZE;	// 写入请求读取长度
												memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
												memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
												strcat((char *)tmpBuf,	(char *)AT_CIPRXGET_H);
												strcat((char *)tmpBuf,	(char *)&sCnn);	
												strcat((char *)tmpBuf,	(char *)",");	
												sprintf((char *)tmpLenBuf,	"%d",	tmpLen);	// 由头文件宏定义决定DEF_GPRS_RXDONE_SIZE						
												strcat((char *)tmpBuf,	(char *)tmpLenBuf);
												strcat((char *)tmpBuf,	(char *)"\r");		
																									
												tcpReadMuxtex =1;	// 置互斥标志,使URC检测挂起												
												if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CIPRXGET2Timeout) != DEF_GSM_NONE_ERR) 
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
														retCode	=	DEF_GSM_CIPGET2_ERR;	
												}
												else
												{
														// 接收结束判断处理(防止接收HEX数据中含‘OK’字符串导致匹配成功提前退出)
														tmpRxdCnt =s_GSMcomm.ps_commu->RCNT;	// 记录当前接收到字节长度
														u16Times =250;	// 最大等待超时500ms
														while(u16Times--)
														{
																gsmDelayMS(2);
																if(tmpRxdCnt == s_GSMcomm.ps_commu->RCNT)
																		break;
																else
																		tmpRxdCnt = s_GSMcomm.ps_commu->RCNT;	// 更新记录长度
														}
														j = 0;
														j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 2,*",s_GSMcomm.ps_commu->RCNT,14);
														if(j == 0xffff)		
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																retCode	= DEF_GSM_UNKNOW_ERR;											
														}
														else
														{
																// 取读到的数据长度(+CIPRXGET: 2,0,6,0)
																tmpLen =0;
																memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
																j = j + 15;
																for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
																{
																		tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
																}
																tmpLenBuf[k] = '\0';
																tmpLen = atoi((const char *)tmpLenBuf);
																
																// 取剩余读取的字节长度
																j = j + k + 1;	// 跳过','
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
																
																// 复制数据
																j = j + k + 2;	// 跳过回车换行指向数据区
																// 判断结束条件有不能为0d 0a如果为hex数据格式，数据流中可能有0d 0a所以出现数据复制不全情况
																for(k =0;	((k < tmpLen) && (k < DEF_GPRS_RXDONE_SIZE));	k++)
																{
																		*(pBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
																}
																*(pBuf + k) = '\0';	
																*len = tmpLen;	
																
																// 判断是否还有数据未读取
																if(nrLen != 0)
																		retCode = DEF_GSM_READ_AGAIN;
																else
																{
																		// 再次判断是否有数据接收到
																		j = j + k;	// 跳过数据区
																		if((s_GSMcomm.ps_commu->RCNT -j) > 0)
																		{
																				p = StringSearch((u8 *)(s_GSMcomm.pRecBuf+j),(u8 *)"+CIPRXGET: 1,*",(s_GSMcomm.ps_commu->RCNT-j),14);		
																				if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
																				{
																						*(s_GSMcomm.pRecBuf + p) = '\0';										// 删除字符串防止同一字符串重复检测
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
												// 读取完毕后清除接收数据内容,防止URC误检测
												s_GSMcomm.ps_commu->RCNT =0;
												tcpReadMuxtex =0;	// 清标志,使能重新URC检测		
										}
								}
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
*                            					GSM上层应用函数-短信部分(由OS 应用函数调用)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSSend() 
* Description    : 所有短信将以USC2编码方式发送，函数内部会将发送的ASCII码转成USC2码(目的避免GSM字符集的一些特殊
*									 字符如：@,$服务器无法识别) [注]默认不发送短信中心号码除非参数手动配置了中心号码       
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
				if((*pPhoneCode == '\0') || (len == 0))
				{
						retCode =	DEF_GSM_NONE_ERR;	
				}
				else
				{							
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>= GSM_ONCMD))
						{
								// 查询当前SIM卡状态
								// Send "AT+CPIN?"
								ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
								if(ret !=	DEF_GSM_NONE_ERR)
								{
										if(ret == CommandTimeout)
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 只有超时才认为是错误
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
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{						
																//	"USC2" Mode 避免特殊字符GSM模式服务器收到不识别
																//	Send AT+CSCS
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
																										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																										retCode	=	DEF_GSM_CMGSCPL_ERR;
																								}
																						}
																				}	
																		}
																}	
																//	Send AT+CSCS="GSM" 切回到GSM编码方式防止再次配置网络参数时编码集改为UCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																		retCode	=	DEF_GSM_CSCS_ERR;
																}	
														}
												}			
										}
										else
										{
												if(len	> (DEF_SMS_USC2BYTE_SIZE*4))
												{
														retCode	=	DEF_GSM_NONE_ERR;		// 如果超出SMS发送长度则被认为发送成功 						
												}
												else
												{
														//  PDU 发中文代码
														//	PDU	Mode						
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_0,	strlen((const char *)AT_CMGF_0),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{								
																// 	"UCS2" Mode 
																//	Send AT+CSCS		
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																		retCode	=	DEF_GSM_CSCS_ERR;		
																}
																else
																{
																		if(*pPhone == '+')
																				pPhone ++;		 // 跳过电话号码中的'+'部分(PDU数据中无"+"号)
																						
																		//	Make Pdu data and return the len of data
																		tmplen = PDUMakeUpData(DEF_COM_PHONE, (u8	*)pSmsCenter,	(u8	*)pPhone, (u8	*)pData, len,	(u8	*)tmpUSC2Buf);			
																		sprintf((char *)lenBuf,	"%d",	tmplen);
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);
																		strcat((char *)tmpBuf,	(char *)lenBuf);
																		strcat((char *)tmpBuf,	(char *)"\r");
																		
																		if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																		{
																				BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																				retCode	=	DEF_GSM_CMGSACK_ERR;				
																		}
																		else
																		{
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																						retCode	=	DEF_GSM_CMGSCPL_ERR;				
																				}		
																		}
																}
																//	Send AT+CSCS="GSM" 切回到GSM编码方式防止再次配置网络参数时编码集改为UCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSSendAlarm() 
* Description    : 发送报警短信该短信会在原报警电话基础上加入国家代码，DEF_NATION_CODE(在MAIN.H中定义)        
* Input          : 所有短信将以USC2编码方式发送，函数内部会将发送的ASCII码转成USC2码(目的避免GSM字符集的一些特殊
*									 字符如：@,$服务器无法识别)[注]默认不发送短信中心号码除非参数手动配置了中心号码  
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
				if((*pPhoneCode == '\0') || (len == 0))
				{
						retCode =	DEF_GSM_NONE_ERR;		// 方式应用程序由于参数无错，导致持续尝试发送
				}
				else
				{			
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								// 查询当前SIM卡状态
								// Send "AT+CPIN?"
								ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
								if(ret !=	DEF_GSM_NONE_ERR)
								{
										if(ret == CommandTimeout)
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 只有超时才认为是错误
										retCode	=	DEF_GSM_CPIN_ERR;
								}
								else
								{
										if(smsMode	==	DEF_GSM_MODE)
										{
												if(len	>	DEF_SMS_USC2BYTE_SIZE)
												{
														retCode	=	DEF_GSM_NONE_ERR;		// 如果超出SMS发送长度则被认为发送成功 			
												}
												else
												{
														//	Text Mode
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_1,	strlen((const char *)AT_CMGF_1),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{						
																//	"USC2" Mode 避免特殊字符GSM模式服务器收到不识别
																//	Send AT+CSCS
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
																										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																										retCode	=	DEF_GSM_CMGSCPL_ERR;
																								}
																						}
																				}	
																		}	
																}
																//	Send AT+CSCS="GSM" 切回到GSM编码方式防止再次配置网络参数时编码集改为UCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																		retCode	=	DEF_GSM_CSCS_ERR;
																}			
														}
												}			
										}
										else
										{
												if(len	> (DEF_SMS_USC2BYTE_SIZE*4))
												{
														retCode	=	DEF_GSM_NONE_ERR;		// 如果超出SMS发送长度则被认为发送成功 						
												}
												else
												{
														//  PDU 发中文代码
														//	PDU	Mode						
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_0,	strlen((const char *)AT_CMGF_0),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{								
																// 	"UCS2" Mode 
																//	Send AT+CSCS		
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																		retCode	=	DEF_GSM_CSCS_ERR;		
																}
																else
																{																									
																		memset((u8 *)tmpPhoneBuf,	'\0',	sizeof(tmpPhoneBuf));
																		strcat((char *)tmpPhoneBuf,	(char *)DEF_SMS_CONUTRY_CODE);	// 复制国家代码
																		strcat((char *)tmpPhoneBuf,	(char *)pPhoneCode);				// 复制报警电话
																		
																		//	Make Pdu data and retur the len of data
																		tmplen = PDUMakeUpData(DEF_COM_PHONE, (u8	*)pSmsCenter,	(u8	*)tmpPhoneBuf, (u8	*)pData, len,	(u8	*)tmpUSC2Buf);
																		
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		sprintf((char *)lenBuf,	"%d",	tmplen);
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);	
																		strcat((char *)tmpBuf,	(char *)lenBuf);
																		strcat((char *)tmpBuf,	(char *)"\r");	
																		
																		if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																		{
																				BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																				retCode	=	DEF_GSM_CMGSACK_ERR;				
																		}
																		else
																		{
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
																						retCode	=	DEF_GSM_CMGSCPL_ERR;				
																				}		
																		}
																}
																//	Send AT+CSCS="GSM" 切回到GSM编码方式防止再次配置网络参数时编码集改为UCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSRead() 
* Description    : 短信取回的手机号码保持原格式不变      
* Input          : 所有短信将以USC2编码方式读取，函数内部会将接收到的USC2码转成ASCII码(目的避免GSM字符集的一些特殊
*									 字符如：@,$无法识别),短信发送时间自动转换为格林威治时间
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
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
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_CMGF_ERR;
								}
								else
								{				
										//	Send AT+CSCS
										//	Return to "UCS2" 修改成UCS2方式(GSM模式下读短信有些特殊字符西门子模块会返回乱码)
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
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
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
														retCode	=	DEF_GSM_CMGR_ERR;
												}
												else
												{																					
														// 查询位置
														p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CMGR: ",s_GSMcomm.ps_commu->RCNT,9);
														if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
														{
																// 短信存储区为空
																retCode	=	DEF_GSM_SMS_ENPTY;
														}
														else
														{
																//	提取电话号码
																p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)",\"",s_GSMcomm.ps_commu->RCNT,2);
																if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																{
																		retCode	=	DEF_GSM_FAIL_ERR;	
																}
																else
																{			
																		memset(tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																		
																		//保留原格式不变(即+86XXXXXX)
																		p = p + 2;	// 指向电话区						
																		for(i=0;	(*(s_GSMcomm.pRecBuf + p + i) != '\"') && (i < sizeof(tmpUSC2Buf));	i++)
																		{
																				tmpUSC2Buf[i] =  *(s_GSMcomm.pRecBuf + p + i);
																		}
																		tmpUSC2Buf[i] = '\0';
																		
																		//转换成标准ACSII码以便协议解析	
																		len = 0;
																		len = ChangeUSC22ASCII (tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	pNumBuf);
																		if(len == 0xffff)
																				retCode	=	DEF_GSM_UNKNOW_ERR;	// 转换错误
																		else
																		{																				
																				//  提取短信发送时间
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
																						
																						// 提取时区
																						p += 2;
																						for(i=0;	(i<3) && (*(s_GSMcomm.pRecBuf + p + i) != '"');	i++)
																						{
																								zoneBuf[i] = *(s_GSMcomm.pRecBuf + p + i);
																						}
																						zoneBuf[i] ='\0';
																						zoneHour = atoi((const char *)zoneBuf) / 4; // 转成实际时区																						
																						count = Mktime (tmpTime.year, tmpTime.month,	tmpTime.day, tmpTime.hour,	tmpTime.minute, tmpTime.second);
																						count = count - (zoneHour * 3600);	// 转成格林威治时间																	
																						Gettime (count,	pTime);	// 转为年月日时分秒格式

																						//从缓冲区里查找["\r]位置即短信内容
																						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\"\r",s_GSMcomm.ps_commu->RCNT,2);
																					 	if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																						{
																								retCode	=	DEF_GSM_FAIL_ERR;	
																						}
																						else
																						{
																								memset(tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																								//	提取短信内容
																								p	+= 3;
																								for(i=0;	*(s_GSMcomm.pRecBuf + p + i) != '\r';	i++)
																								{
																										tmpUSC2Buf[i] = *(s_GSMcomm.pRecBuf + p + i);
																								}
																								tmpUSC2Buf[i] = '\0';
		
																								//转换成标准ACSII码以便协议解析
																								len = 0;
																								len = ChangeUSC22ASCII (tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	pdBuf);
																								if(len == 0xffff)
																								{
																										*pdLen  = 0;
																										retCode	=	DEF_GSM_UNKNOW_ERR;	// 转换错误
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
										//	Send AT+CSCS="GSM" 切回到GSM编码方式防止再次配置网络参数时编码集改为UCS2
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
												retCode	=	DEF_GSM_CSCS_ERR;
										}											
								}
						}
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
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
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_CMGD_ERR;
								}
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
*                            					GSM上层应用函数-电话部分(由OS 应用函数调用)     
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
				// 检测入参合法性
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
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
										retCode	=	DEF_GSM_ATD_ERR;
								}
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;
				}	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_CLCC_ERR;		
						}
						else
						{
								p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CLCC:",s_GSMcomm.ps_commu->RCNT,6);		
								if((p == 0xffff)&&(p	> s_GSMcomm.ps_commu->RCNT))
								{
										retCode = CALL_DOWN;		// 返回字段里只有OK没有CLCC表示通话已经结束，所以不做错误累加
								}
								else
								{
										//+CLCC: 1,1,0,0,0\r\n\r\nOK\r\n	(无来电显示返回)
										//+CLCC: 1,1,4,0,0,"01058302855",129\r\n\r\nOK\r\n
										sta = *(s_GSMcomm.pRecBuf + p + 9);			// 取电话发起状态
										if(sta == '0')
										{
											 	 // 电话发起者	
										}
										else if(sta == '1')
										{
												// 电话接收者取打来者电话号码
												if((*(s_GSMcomm.pRecBuf + p + 16) == ',') && (*(s_GSMcomm.pRecBuf + p + 17) == '"')) 	
												{
														for(i=0;	(*(s_GSMcomm.pRecBuf + p + i + 18) != '\"') && (i < s_GSMcomm.ps_commu->RCNT);	i++)
														{
															 *(pPhone + i) = *(s_GSMcomm.pRecBuf + p + i + 18);	
														}
														*(pPhone + i) = '\0';
												}
												else
														*pPhone = '\0';	//	(无来电显示无该字段)				
										}
										sta = *(s_GSMcomm.pRecBuf + p + 11);			// 取电话状态
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_ATH_ERR;		
						}			
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_ATA_ERR;		
						}		
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
*                            					GSM上层应用函数-睡眠部分(由OS 应用函数调用)     
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
		// 进入睡眠模式		
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
		// 退出睡眠模式
		IO_GSM_DTR_EN();
		IO_GSM_DTR_DIS();
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMIntoSleep() 
* Description    :  发送AT+CSCLK=1使能睡眠模式 后操作释放DTR管脚(DTR 0->1)，设置睡眠标志(ringWUpFlag 0->1)       
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 使能睡眠模式
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCLK_1,	strlen((const char *)AT_CSCLK_1),	CommandRecOk,	CSCLK1Timeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_CSCLK1_ERR;		
						}	
						else
						{
								gsmDelayMS(2000);	// 保持模块空闲2秒		
								GSMGotoSleep();		// 直接进入睡眠	
								ringWUpFlag	=1;		// 设置GSM睡眠标志		
						}							
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMWakeUp() 
* Description    : 唤醒前检查是否由于ring导致的唤醒如果是的话则需要手动启动一次短息查询        
* Input          : 操作RTS=1
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMWakeUp(u32	timeout)
{
		timeout = timeout; 
		GSMStartWork();		// 直接退出睡眠	
				
		if(ringWUpFlag == 2)	
		{
				ringWUpFlag =0;																					// 恢复标志到默认状态
				if(BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0)							// 防止重复进入
				{
						if(OSRunning > 0)
						{
								BSP_GSM_SetFlag(DEF_SMSDOING_FLAG);							// 设置短信处理中
								OSSemPost(GSMSmsSem); 													// 设备从睡眠唤醒手动触发一次短信检索
						}
				}
		}		
		return	0;	
}

/*
*********************************************************************************************************
*                            					GSM上层应用函数-通用接口部分(由OS 应用函数调用)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMBypassPro() 
* Description    : GSM AT 指令透传接口       
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
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
						// 软件复位
						if(BSP_SendATcmdPro(DEF_NULL_MODE,	(u8	*)sendBuf,	sendLen,	CommandTimeout,	atTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 错误累加
								retCode	=	DEF_GSM_UNKNOW_ERR;		
						}	
						else
						{
								// 复制返回字符串
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
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}

/*
******************************************************************************
* Function Name  : BSP_GSMSemPend()
* Description    : 用于GSM信号量的获取，必须与BSP_GSMSemPost成对使用
* Input          : timeout :等待信号量超时时间
* Output         : None
* Return         : None
******************************************************************************
*/
s8	BSP_GSMSemPend (u32	timeout)
{
		INT8U	err =OS_NO_ERR;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	请求发送数据信号量
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
* Description    : 用于GSM信号量的释放，必须与BSP_GSMSemPend成对使用
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
void BSP_GSMSemPost (void)
{
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	释放占用的信号量
		else
				GSMMutexFlag	=	1;
}
/*
*********************************************************************************************************
*                            					GSM循环调用函数(需系统中循环调用)     
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
				IO_GSM_POW_EN();		// 模块供电	
																				
    if(s_GSMcomm.SetupPhase == PowerOff)
		{
				// 预置端口状态
				s_GSMcomm.GSMComStatus 	= GSM_POWOFF;

				// 首先清除接收缓冲区准备接收开机字符串
				memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE);	
				s_GSMcomm.ps_commu->RCNT	=	0;
				GSM_ENABLE_RES_IT();							// 第一次使能GSM模块中断
				BSP_GSMRING_IT_CON(DEF_DISABLE);	// 检测过程中关闭RING中断
							
				//IO_GSM_RES_OFF();
				IO_GSM_POW_OFF();
				IO_GSM_POW_ON();
	     	s_GSMcomm.SetupPhase = PowerOn;
    }
		else if(s_GSMcomm.SetupPhase == PowerOn)
		{
	      s_GSMcomm.SetupPhase 		= SignalOn;
				s_GSMcomm.Timer 				= 0;							//准备延时2S
    }
		else if(s_GSMcomm.SetupPhase == SignalOn)
		{
				if(s_GSMcomm.Timer < GSMTime2s)		// 等待2S	
		      	return;
	      else
				{
		        IO_GSM_POW_OFF();
		        s_GSMcomm.SetupPhase 		= CheckPowPin;
						s_GSMcomm.Timer					=	0;	 				 //准备下次延时
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
						s_GSMcomm.Timer					=	0;	 				 // 准备下次延时
						BSP_GSM_ERR_Clr(&err_Gsm.powOnTimes);  // 清错误次数
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
								BSP_GSM_ERR_Add(&err_Gsm.powOnTimes,	1);	//启动失败次数加一
								if(BSP_GSM_ERR_Que(&err_Gsm.powOnTimes) >= DEF_GSM_POWON_TIMES)
								{
										BSP_GSM_ERR_Clr(&err_Gsm.powOnTimes);
										s_GSMcomm.HardWareSta		=	DEF_GSMHARD_MODDESTROY_ERR;	
								}
								s_GSMcomm.SetupPhase 	= PowerOff;
								s_GSMcomm.Timer				=	0;	 			//准备下次延时	
								BSP_GSM_POW_RESET(3000);
						}
				}
		}
		// Check power on string 
		else if(s_GSMcomm.SetupPhase == CheckStartString)
		{
				if(s_GSMcomm.Timer < GSMTime5s)		//等待输出完上电字符"RDY"
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
								s_GSMcomm.Timer					=	0;	 				//准备下次延时																				
								BSP_GSM_ERR_Clr(&err_Gsm.cmmTimes);  // 清错误次数
								gsmDelayMS(1000);	
						}
				}
				else
				{
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSMSTA)
						myPrintf("[GSM]: Fixed Baud Rate!\r\n");
						#endif
										
						// 需要固定波特率
						GSM_DISABLE_SEND_IT();												
						BSP_PutString(DEF_GSM_UART,(u8 *)"AT+IPR=115200\r");
						gsmDelayMS(500);
						BSP_PutString(DEF_GSM_UART,(u8 *)"AT&W\r");	//SIM800需要手动存储
						gsmDelayMS(200);

					  // 错误处理
						BSP_GSM_ERR_Add(&err_Gsm.cmmTimes,	1);	//启动失败次数加一
						if(BSP_GSM_ERR_Que(&err_Gsm.cmmTimes) >= DEF_GSM_STRING_TIMES)
						{
								BSP_GSM_ERR_Clr(&err_Gsm.cmmTimes);
								s_GSMcomm.HardWareSta		=	DEF_GSMHARD_COMM_ERR;	
						}
						s_GSMcomm.SetupPhase 		= PowerOff;
						s_GSMcomm.Timer					=	0;	 			//准备下次延时
						BSP_GSM_POW_RESET(3000);
				}	
		}	
		// send check simcard
		else if(s_GSMcomm.SetupPhase == CheckSimCardSend)
		{
				memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // 清缓冲
				s_GSMcomm.ps_commu->RCNT	=	0;
				GSM_DISABLE_SEND_IT();	
			
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				myPrintf("AT+CPIN?\r\n");	// 输出发送AT指令
				#endif	
				BSP_PutString(DEF_GSM_UART,(u8 *)"AT+CPIN?\r");	// 查询SIM card
				//BSP_PutString(DEF_GSM_UART,(u8 *)"AT+CREG?\r");	// 查询SIM card
				s_GSMcomm.SetupPhase = CheckSimCard;
				s_GSMcomm.Timer			 = 0;	 			//准备下次延时
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
																
								memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // 清缓冲		
								s_GSMcomm.ps_commu->RCNT	=	0;						
								s_GSMcomm.GSMComStatus 	= GSM_POWOK;
								s_GSMcomm.SetupPhase 		= SimCardOK;
								s_GSMcomm.Timer					=	0;	 				//准备下次延时
								s_GSMcomm.HardWareSta		=	DEF_GSMHARD_NONE_ERR;										
								BSP_GSM_ERR_Clr(&err_Gsm.simCardTimes);	//清错误计时器
						}
				}
				else
				{					
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSM)
						BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pRecBuf,	s_GSMcomm.ps_commu->RCNT);
						#endif
						
						memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // 清缓冲		
						s_GSMcomm.ps_commu->RCNT	=	0;						
						s_GSMcomm.GSMComStatus 	= GSM_POWOK;
						s_GSMcomm.SetupPhase 		= SimCardOK;
						s_GSMcomm.Timer					=	0;	 				//准备下次延时
						s_GSMcomm.HardWareSta		=	DEF_GSMHARD_NONE_ERR;										
						BSP_GSM_ERR_Clr(&err_Gsm.simCardTimes);	//清错误计时器
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
				memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // 清缓冲	
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
								GSM_ENABLE_RES_IT();					//使能接收中断准备接收 AT指令结果
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
				
										// 加入延时有的卡不加入延时初始化不成功
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
												s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// 置GSM端口状态 
												BSP_GSMRING_IT_CON(DEF_ENABLE);				// 开启RING中断
												BSP_GSM_ERR_Clr(&err_Gsm.initTimes);
												return;
				            }
										else
										{}	//继续发下一条AT命令
				        }
								else if((s_GSMcomm.CmdPhase == CommandRecErr) || (s_GSMcomm.CmdPhase == CommandTimeout))
								{      					 
										// received ERROR. 
										OSTimeDly(1000);		// 如果回复错误防止重试指令间隔过短 
				
										// 错误处理
										BSP_GSM_ERR_Add(&err_Gsm.initTimes,	1);	//启动失败次数加一
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
								// 发送第二个以后的初始化指令
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
								memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // 清缓冲	
								s_GSMcomm.ps_commu->RCNT = 0;		
				        s_GSMcomm.Timer = 0;
				        s_GSMcomm.CmdPhase = CommandSend;
				        s_GSMcomm.ps_commu->SLEN = i;
				        s_GSMcomm.sendstart();
						}	
						else
              	return;	// 等待超时
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
		Start2doSendAT();		// AT发送处理
		DoSendProcess();		// AT接收处理		
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_CommonPro()	
* Description    : GSM模块，来电话，短信，GPRS，蓝牙，等URC数据判断处理        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_CommonPro (void)
{
  	u16	p=0,q=0,k=0,dotNum=0;

		// 自动上报的URC检测处理(网络读数据阶段禁止检测)//////////////////////////////////////////////////
		if((s_GSMcomm.SetupPhase == CommandInitOK) && (s_GSMcomm.GSMComStatus	>=	GSM_ONCMD) && (tcpReadMuxtex == 0))
		{		
				//	检测Ring管脚状态	
				// 2015-1-15 by:gaofei 弥补GPRS数据遗漏问题,该逻辑已取消,经测试一旦使能URC ring中断可能导致数据接收
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
												OSSemPost(GSMGprsDataSem);		// 手动查询是否接到到数据
								}
						}			
				}
				*/
				//	检测是否有电话打入	
				/*
				p = 0;
				p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"RING\r\n",s_GSMcomm.ps_commu->RCNT,6);		
				if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
				{
						*(s_GSMcomm.pRecBuf + p) 			= '\0';
						*(s_GSMcomm.pRecBuf + p + 1) 	= '\0';		
						BSP_PutString(DEF_GSM_UART,(u8 *)"ATA\r");	// 接电话
						
				}
				*/
				//  检测短信接收	
				//	检测GPRS网络状态
				//	检测BT状态
				//	检测TTS状态
				if(1)
				{				
						p =	0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CMTI:",s_GSMcomm.ps_commu->RCNT,6);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';
								*(s_GSMcomm.pRecBuf + p + 1) 	= '\0';								// 删除字符串防止同一字符串重复检测
								if(BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0)					// 防止重复进入
								{
										if(OSRunning > 0)
										{
												BSP_GSM_SetFlag(DEF_SMSDOING_FLAG);					// 设置短信处理中
												OSSemPost(GSMSmsSem); 
										}
								}
						}	
				}
				
				//  检测TTS播放完成			
				if(1)
				{		
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CTTS: 0",s_GSMcomm.ps_commu->RCNT,8);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';
								*(s_GSMcomm.pRecBuf + p + 1) 	= '\0';								// 删除字符串防止同一字符串重复检测
								if(BSP_GSM_GetFlag(DEF_TTSPLAY_FLAG) == 1)					// 防止重复进入
								{
										BSP_GSM_ReleaseFlag (DEF_TTSPLAY_FLAG);					// 复位标志										
										IO_SPAKER_PA_DIS();															// 关闭功放
								}
						}	
				}
				
				//	检测BT接收数据及业务状态	
				if(1)
				{						
						// 检测匹配请求									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTPAIRING:",s_GSMcomm.ps_commu->RCNT,11);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								gsmDelayMS(5);	// 等待接收完整
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// 删除字符串防止同一字符串重复检测
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
										s_MboxBT.func =BT_PAIR_PASS;											// Passkey匹配业务
										if(OSRunning > 0)
												OSMboxPost(GSMBtAppMbox, (void *)&s_MboxBT);	// 发送邮箱到BT任务 										
								}
								else if(dotNum == 2)
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_BT)
												myPrintf("[BT]: Digi pair req!\r\n");
										#endif												
										s_MboxBT.func =BT_PAIR_DIGI;											// 数据匹配业务
										if(OSRunning > 0)
												OSMboxPost(GSMBtAppMbox, (void *)&s_MboxBT);	// 发送邮箱到BT任务 
								}
								else
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_BT)
												myPrintf("[BT]: Unknow pair req!\r\n");
										#endif	
								}
						}									
							
						// 检测连接请求									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTCONNECTING:",s_GSMcomm.ps_commu->RCNT,14);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// 删除字符串防止同一字符串重复检测				
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								s_MboxBT.func =BT_CONNECT;													// 连接业务
							
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_BT)
										myPrintf("[BT]: Connect req!\r\n");
								#endif
								if(OSRunning > 0)
										OSMboxPost(GSMBtAppMbox, (void *)&s_MboxBT);		// 发送邮箱到BT任务 
						}	
						
						// 检测蓝牙断开									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTDISCONN:",s_GSMcomm.ps_commu->RCNT,11);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// 删除字符串防止同一字符串重复检测
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								s_MboxBT.func =BT_DISCONNECT;												// 断开连接业务							
								BSP_GSM_SetFlag(DEF_BTDING_FLAG);										// 设置蓝牙在线标志
								
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_BT)
										myPrintf("[BT]: Disconnect!\r\n");
								#endif
						}	

						// 检测蓝牙数据
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTSPPMAN: *",s_GSMcomm.ps_commu->RCNT,12);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// 删除字符串防止同一字符串重复检测
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								s_MboxBT.func =BT_RXDATA;														// 收到数据	
								s_MboxBT.cnnId =*(s_GSMcomm.pRecBuf + p + 11);			// 获取连接ID
								
								if(OSRunning > 0)
										OSMboxPost(GSMBtDataMbox, (void *)&s_MboxBT);		// 发送邮箱到BT任务 
						}	
				}	
				
				//	检测GPRS接收数据及网络异常	
				if(s_GSMcomm.GSMComStatus	>=	GSM_ONLINE)
				{						
						// 检查接收数据									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 1,*",s_GSMcomm.ps_commu->RCNT,14);		
						if((p	!=  0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) = '\0';										// 删除字符串防止同一字符串重复检测
								if(OSRunning > 0)
										OSSemPost(GSMGprsDataSem);		// 将收到数据以信号量的形式投递出去(考虑到可能很短时间收到很多数据邮箱不合适) 
						}

						// 检查网络异常
						p = 0;
						q = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+PDP: DEACT",s_GSMcomm.ps_commu->RCNT,11);	// 设备GPRS PDP环境断开
						q = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"0, CLOSE",s_GSMcomm.ps_commu->RCNT,8);			// 通道0服务器主动断开		
						if(((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT)) || ((q	!=	0xffff)&&(q	<=	s_GSMcomm.ps_commu->RCNT))) 
						{
								if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
								{								
										// GPRS PDP上下文断开
										*(s_GSMcomm.pRecBuf + p) = '\0';								// 删除字符串防止同一字符串重复检测																			
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("[GPRS]: GPRS PDP break!\r\n");
										#endif
								}
								else
								{
										// 服务器主动断开
										*(s_GSMcomm.pRecBuf + q) = '\0';								// 删除字符串防止同一字符串重复检测										
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("[GPRS]: Server break!\r\n");
										#endif
								}
								s_GSMcomm.GSMComStatus	=	GSM_ONCMD;								// 置GSM端口状态
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

		memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	// 清错误变量

		s_ATcomm.Slen						=	0;
		s_ATcomm.Rlen						=	0;
		s_ATcomm.pSendBuf				=	SBufAT;
		s_ATcomm.pRecBuf				=	RBufAT;
		s_ATcomm.timeout				=	0;
		s_ATcomm.CheckMode			=	DEF_COMMON_MODE;
				
		BSP_USART_Init(DEF_GSM_UART,	DEF_GSM_REMAP,	DEF_GSM_BAUT);

		GSMRingPinInit();			//	初始化RING中断管脚
		
		GSM_DISABLE_RES_IT();		//	禁止接收中断
		GSM_DISABLE_SEND_IT();	//	禁止发送中断

		// 防止当GSM电源开关芯片坏掉(比如电源直通)时硬件上电GSM也能以检测成功,但是软件复位检测失败 
		IO_GSM_POW_ON();
		gsmDelayMS(1000);
		IO_GSM_POW_OFF();
		gsmDelayMS(100);
		GSMFeedWDGProcess();	// 喂狗	
		
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

