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
*                                        BOARD SUPPORT PACKAGE
*
*                                     			  bsp_gsm_h.h
*                                              with the
*                                   			   Y05D Board
*
* Filename      : bsp_gsm_h.h
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

#ifndef  BSP_GSM_H_H
#define  BSP_GSM_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_GSM_GLOBLAS
#define  BSP_GSM_EXT
#else
#define  BSP_GSM_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "stm32f10x.h"
#include "bsp_rtc_h.h"

/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/
//////////////////////////////////////////////////////////////////////////////////
// 调试信息输出定义
//#define	DEF_GSMINFO_OUTPUTEN		(1)							// GSM信息输出使能

// GSM 通用定义 ////////////////////////////////////////////////////////////////////////////////////////////////////  					
// 定义使用的GPRS通道号

#define	DEF_GPRS_CNN_MAX				(5)							// 通道号0~5 最多支持六个通道
#define	DEF_GPRS_RXDONE_SIZE		(1040)					// GPRS读取1包数据的大小(该大小不能大于GSM底层接收BUF大小)模块允许单次读最大长度为1460

#define	DEF_GPRS_DATA_CNN0			('0')						// GPRS数据通道1


// 定义短信最大发送字符数目
// GSM 模式 160字符
// USC2 模式 70汉字140字符

#define	DEF_SMS_GSMBYTE_SIZE		(170)						// "GMS"字符集最多发送180个字节
#define	DEF_SMS_USC2BYTE_SIZE		(80)						// "USC2"字符集最多发送88个字
#define	DEF_SMS_CONUTRY_CODE		("86")					// 以"USC2"方式发送短信时的国家代码定义


// GSM 拨号信号质量要求
#define	DEF_RSSI_HIGH_LEVEL			((u8)40)				// 天线信号强度高值
#define	DEF_RSSI_LOW_LEVEL			((u8)10)				// 天线信号强度低值		
#define	DEF_RSSI_RATE_LEVEL			((u8)10)				// 天线误码率最大值	
	
// GSM音频通道设置
#define	DEF_GSM_NORMAL					((u8)0)
#define	DEF_GSM_AUX							((u8)1)


// GSM 接口相关参数定义
#define	DEF_GSM_BAUT						(115200)									// GSM 串口波特率
#define	DEF_GSM_UART						DEF_USART_1								// GSM 串口号	
#define	DEF_GSM_REMAP						DEF_USART_REMAP_NOUSE			// GSM 串口镜像	

#define	DEF_GSM_RING_EXTI_LINE			EXTI_Line2								// GSM RING中断线
#define	DEF_GSM_RING_EXTI_TRIGGER		EXTI_Trigger_Falling			// GSM RING触发方式
#define	DEF_GSM_RING_EXTI_PORTS			GPIO_PortSourceGPIOC			// GSM EXTI RING端口号
#define	DEF_GSM_RING_EXTI_PINS			GPIO_PinSource2						// GSM EXTI RING管位置

#define	DEF_GSM_RING_PORTS			GPIOC											// GSM RING 端口号
#define	DEF_GSM_RING_PINS				BSP_GPIOC_GSM_RING				// GSM RING 管位置
#define	DEF_GSM_POWEN_PORTS			GPIOC											// GSM POW EN端口号
#define	DEF_GSM_POWEN_PINS			BSP_GPIOC_GSM_POWEN				// GSM POW EN管位置
#define	DEF_GSM_POWON_PORTS			GPIOA											// GSM POW ON端口号
#define	DEF_GSM_POWON_PINS			BSP_GPIOA_GSM_POWON				// GSM POW ON管位置
#define	DEF_GSM_POWRST_PORTS		GPIOC											// GSM POW RESET端口号
#define	DEF_GSM_POWRST_PINS			BSP_GPIOC_GSM_RESET				// GSM POW RESET管位置
#define	DEF_GSM_POWSLEEP_PORTS	GPIOC											// GSM POW SLEEP端口号
#define	DEF_GSM_POWSLEEP_PINS		BSP_GPIOC_GSM_DTR					// GSM POW SLEEP管位置
#define	DEF_GSM_POWDET_PORTS		GPIOC											// GSM POW DET端口号
#define	DEF_GSM_POWDET_PINS			BSP_GPIOC_GSM_POWDET			// GSM POW DET管位置

#define	GSM_ATSBUF_TXD_SIZE			(800)
#define	GSM_ATSBUF_RXD_SIZE			(1200)		// 考虑到接收升级文件情况
#define	GSM_GPSRSBUF_TXD_SIZE		(800)
#define	GSM_GPSRSBUF_RXD_SIZE		(1200)		// 考虑到接收升级文件情况

#define	GSM_SMSINDEX_SIZE				(5)
#define	GSM_SMSNUM_SIZE					(20)
#define	GSM_SMSBUF_SIZE					(250)
#define	GSM_GDATABUF_SIZE				(1200)		// 考虑到接收升级文件情况
#define	GSM_BDATABUF_SIZE				(300)

// GSM串口接收发送状态定义.
#define MOD 										((u16)0x0003)
#define RS 											((u16)0x000c)
#define SS 											((u16)0x0030)
#define	MOD_R										((u16)0x0001)
#define	MOD_S										((u16)0x0002)
#define	RS_I										((u16)0x0004)
#define	RS_O										((u16)0x0008)
#define	SS_I										((u16)0x0010)
#define	SS_O										((u16)0x0020)

// TTS部分定义
#define	DEF_TTS_PLAY_ASCII			((u8)0)					// TTS以ASCII码方式播放语音
#define DEF_TTS_PLAY_UCS2				((u8)1)					// TTS以UCS2码方式播放语音

// GSM 超时时间定义 ////////////////////////////////////////////////////////////////////////////////////////////////////  					
// 该定义值与GSM任务自加频率有关，本程序为100ms记一次数
#define GSMTime100ms   					((u32)2)
#define GSMTime200ms   					((u32)3)
#define	GSMTime300ms						((u32)4)
#define GSMTime500ms   					((u32)5)
#define GSMTime1s      					((u32)10)
#define GSMTime2s      					((u32)20)
#define	GSMTime3s								((u32)30)
#define GSMTime5s      					((u32)50)
#define GSMTime6s      					((u32)60)
#define GSMTime8s      					((u32)80)
#define GSMTime10s     					((u32)100)
#define GSMTime15s     					((u32)150)
#define GSMTime20s     					((u32)200)
#define GSMTime25s     					((u32)250)
#define	GSMTime30s							((u32)300)
#define	GSMTime40s							((u32)400)
#define	GSMTime50s							((u32)500)
#define GSMTime60s     					((u32)600)
#define GSMTime65s     					((u32)650)
#define GSMTime75s     					((u32)750)
#define GSMTime85s     					((u32)850)
#define GSMTime90s     					((u32)900)
#define GSMTime120s    					((u32)1200)
#define GSMTime180s    					((u32)1800)

// 定义主要命令超时时间
// 网络部分
#define	CIPSHUTTimeout					(GSMTime65s)		// "AT+CIPSHUT\r"
#define	CIPMUXTimeout						(GSMTime2s)			// "AT+CIPMUX=0/1\r"
#define	CIPRXGET1Timeout				(GSMTime2s)			// "AT+CIPRXGET=1\r"
#define	CIPHEADTimeout					(GSMTime2s)			// "AT+CIPHEAD=1\r"
#define	CSTTTimeout							(GSMTime2s)			// "AT+CSTT="CMNET","",""\r"
#define	CIICRTimeout						(GSMTime85s)		// "AT+CIICR\r"
#define	CIFSRTimeout						(GSMTime10s)		// "AT+CIFSR\r"
#define	CIPSTARTTimeout					(GSMTime75s)		// "AT+CIPSTART=n,\"TCP\",\"218.249.201.35\",\"13000\"\r"
#define	CIPCLOSETimeout					(GSMTime2s)			// "AT+CIPCLOSE=n\r"
#define	CIPRXGET2Timeout				(GSMTime5s)			// "AT+CIPRXGET=2,1,1000\r"
#define	CIPSENDATimeout					(GSMTime5s)			// "AT+CIPSEND=n,len\r"
#define	CIPSENDFTimeout					(GSMTime30s)		// SEND OK
#define	CIPSENDQTimeout					(GSMTime2s)			// "AT+CIPSEND?\r"

// 电话部分
#define	ATDTimeout							(GSMTime5s)
#define	CLCCTimeout							(GSMTime5s)
#define	ATHTimeout							(GSMTime2s)
#define	ATATimeout							(GSMTime3s)

// 短信部分
#define	CMGFTimeout							(GSMTime2s)
#define	CSCSTimeout							(GSMTime2s)
#define	CNMITimeout							(GSMTime2s)
#define	CPMSTimeout							(GSMTime5s)
#define	CSCATimeout							(GSMTime2s)
#define	CMGSACKTimeout					(GSMTime5s)
#define	CMGSCPLTimeout					(GSMTime60s)
#define	CMGRTimeout							(GSMTime8s)
#define	CMGDTimeout							(GSMTime5s)

// 其它部分
#define	CPINTimeout							(GSMTime2s)			//  "AT+CPIN?\r"
#define	CSQTimeout							(GSMTime1s)			//  "AT+CSQ\r" 
#define	CREGTimeout							(GSMTime1s)			//	"AT+CREG?\r"
#define	CGREGTimeout						(GSMTime1s)			//	"AT+CGREG?\r"
#define	COPSTimeout							(GSMTime1s)			//	"AT+COPS?\r"
#define	CGSNTimeout							(GSMTime1s)			//	"AT+CGSN\r"
#define	GSNTimeout							(GSMTime1s)			//	"AT+GSN\r"
#define	CGATTTimeout						(GSMTime2s)			//	"AT+CGATT?\r"
#define	CGATT0Timeout						(GSMTime20s)		//	"AT+CGATT=0\r"
#define	CGATT1Timeout						(GSMTime20s)		//	"AT+CGATT=1\r"
#define	CFUN0Timeout						(GSMTime5s)
#define	CFUN1Timeout						(GSMTime5s)
#define	CFUN11Timeout						(GSMTime10s)
#define CSCLK1Timeout						(GSMTime2s)
#define CSCLK0Timeout						(GSMTime2s)
#define	CIMITimeout							(GSMTime2s)			//	"AT+CIMI\r"
#define	CNUMTimeout							(GSMTime3s)			//	"AT+GNUM\r"
#define	CPBSTimeout							(GSMTime3s)			//	"AT+CPBS=\"ON\""
#define	CPBWTimeout							(GSMTime3s)			//	"AT+CPBW=1,\"13145678901\",145"

// TTS部分
#define	CTTSTimeout							(GSMTime2s)			// "AT+CCTS=1/2,string\r" 超时需要查询手册确定!!!!!!!!!!
#define	CTTSPARAMTimeout				(GSMTime2s)			// "AT+CTTSPARAM=<vol>,<mode>,<pitch>,<speed>,<channel>\r"
#define	CTTSRINGTimeout					(GSMTime2s)			// "AT+CTTSRING=0/1\r"

// FS部分
#define	FSLSTimeout							(GSMTime2s)			// "AT+FSLS=path\r"
#define	FSMKDIRTimeout					(GSMTime2s)			// "AT+FSMKDIR=C:\User\r""
#define	FSCREATETimeout					(GSMTime2s)			// "AT+FSCREATE=C:\file.txt\r"
#define	FSDELTimeout						(GSMTime2s)			// "AT+FSDEL=C:\User\file.txt\r"
#define	FSREADTimeout						(GSMTime5s)			// "AT+FSDEL=C:\User\file.txt,0,100,0\r"
#define	FSMEMTimeout						(GSMTime2s)			// "AT+FSMEM\r"
#define	FSWRITETimeout					(GSMTime5s)			// "AT+FSWRITE=C:\User\file.txt,0,len,timeout\r"
#define	FSWRITECPLTimeout				(GSMTime2s)			// SEND DATA

// BT部分
#define BTPOWERTimeout					(GSMTime2s)			// "AT+BTPOWER=1\r"
#define	BTHOSTTimeout						(GSMTime2s)			// "AT+BTHOST=name\r"
#define	BTVISTimeout						(GSMTime2s)			// "AT+BTVIS=1\r"
#define	BTSPPGET1Timeout				(GSMTime2s)			// "AT+BTSPPGET=1\r"
#define	BTPAIRTimeout						(GSMTime2s)			// "AT+BTPAIR=1,1\r"
#define	BTACPTTimeout						(GSMTime2s)			// "AT+BTACPT=1\r"
#define	BTSPPGET2Timeout				(GSMTime2s)			// "AT+BTSPPGET=2\r"
#define	BTSPPGET3Timeout				(GSMTime2s)			// "AT+BTSPPGET=3,len\r"
#define	BTSPPCFGTimeout					(GSMTime2s)			// "AT+BTSPPCFG?\r"
#define	BTSPPSENDTimeout				(GSMTime2s)			// "AT+BTSPPSEND=len"
#define	BTSPPSENDCPLTimeout			(GSMTime5s)			// SEND DATA



// GSM 软件错误定义 ////////////////////////////////////////////////////////////////////////////////////////////////////

// GSM错误最大次数定义.
#define	DEF_GSM_INIT_TIMES			((u16)30)				// GSM初始化命令错误次数30
#define	DEF_GSM_POWON_TIMES			((u16)3)				// GSM初始化模块开机检测次数3
#define	DEF_GSM_STRING_TIMES		((u16)5)				// GSM初始化START字符串错误次数5
#define	DEF_GSM_SIMCARD_TIMES		((u16)3)				// GSM初始化SIM检查错误次数3
#define	DEF_GSM_APP_TIMES				((u16)30)			 	// GSM应用层错误最大次数30
#define	DEF_GSM_CNN_TIMES				((u16)20)			 	// GPRS联网错误最大次数20


// 软件错误定义(用于软件错误返回)
#define	DEF_GSM_RET_NULL				((s8)120)				// 定义所一个未被使用的返回码用于初始化返回码
#define	DEF_GSM_READ_AGAIN			((s8)60)				// 该返回码只存在于读数据返回中,表示数据为完整读取还需要再读一次
#define	DEF_GSM_NONE_ERR				((s8)0)	
#define	DEF_GSM_TIMEOUT_ERR			((s8)-1)
#define	DEF_GSM_BUSY_ERR				((s8)-2)
#define	DEF_GSM_FAIL_ERR				((s8)-3)
#define	DEF_GSM_COMERING_ERR		((s8)-4)				// 来电退出
#define	DEF_GSM_DATA_OVERFLOW		((s8)-5)				// 数据溢出				
#define	DEF_GSM_UNKNOW_ERR			((s8)-6)				// 未知错误

// 网络部分错误定义
#define	DEF_GSM_CIPSHUT_ERR			((s8)-20)				// "AT+CIPSHUT\r"
#define	DEF_GSM_CIPMUX_ERR			((s8)-21)				// "AT+CIPMUX=0/1\r"
#define	DEF_GSM_CIPRXGET1_ERR		((s8)-22)				// "AT+CIPRXGET=1\r"
#define	DEF_GSM_CIPHEAD_ERR			((s8)-23)				// "AT+CIPHEAD=1\r"
#define	DEF_GSM_CSTT_ERR				((s8)-24)				// "AT+CSTT="CMNET","",""\r"
#define	DEF_GSM_CIICR_ERR				((s8)-25)				// "AT+CIICR\r"
#define	DEF_GSM_CIFSR_ERR				((s8)-26)				// "AT+CIFSR\r"
#define	DEF_GSM_CIPSTART_ERR		((s8)-27)				// "AT+CIPSTART=n,\"TCP\",\"218.249.201.35\",\"13000\"\r"
#define	DEF_GSM_CIPCLOSE_ERR		((s8)-28)				// "AT+CIPCLOSE=n\r"
#define	DEF_GSM_CIPGET2_ERR			((s8)-29)				// "AT+CIPRXGET=2,1,1000\r"
#define	DEF_GSM_CIPSENDA_ERR		((s8)-30)				// "AT+CIPSEND=n,len\r"
#define	DEF_GSM_CIPSENDF_ERR		((s8)-31)				// SEND OK
#define	DEF_GSM_CIPSENDQ_ERR		((s8)-32)				// "AT+CIPSEND?"

// 电话部分错误定义
#define	DEF_GSM_ATD_ERR					((s8)-40)
#define	DEF_GSM_CLCC_ERR				((s8)-41)	
#define	DEF_GSM_ATH_ERR					((s8)-42)	
#define	DEF_GSM_ATA_ERR					((s8)-43)	
	
// 短信部分
#define	DEF_GSM_CMGF_ERR				((s8)-50)
#define	DEF_GSM_CNMI_ERR				((s8)-51)
#define	DEF_GSM_CSCS_ERR				((s8)-52)
#define	DEF_GSM_CSCA_ERR				((s8)-53)
#define	DEF_GSM_CMGSACK_ERR			((s8)-54)
#define	DEF_GSM_CMGSCPL_ERR			((s8)-55)
#define	DEF_GSM_CMGR_ERR				((s8)-56)
#define	DEF_GSM_CMGD_ERR				((s8)-57)
#define	DEF_GSM_SMS_ENPTY				((s8)-58)
#define	DEF_GSM_CPMS_ERR				((s8)-59)

// 其它部分
#define	DEF_GSM_CPIN_ERR				((s8)-70)				// "AT+CPIN?\r"
#define	DEF_GSM_CSQ_ERR					((s8)-71)				// "AT+CSQ\r" 
#define	DEF_GSM_CREG_ERR				((s8)-72)				// "AT+CREG?\r"
#define	DEF_GSM_CGREG_ERR				((s8)-73)				// "AT+CGREG?\r"
#define	DEF_GSM_COPS_ERR				((s8)-74)				// "AT+COPS?\r"
#define	DEF_GSM_CGSN_ERR				((s8)-75)				// "AT+CGSN\r"
#define	DEF_GSM_GSN_ERR					((s8)-76)				// "AT+GSN\r"
#define	DEF_GSM_CGATT_ERR				((s8)-77)				// "AT+CGATT=n\r"
#define	DEF_GSM_CFUN0_ERR				((s8)-79)
#define	DEF_GSM_CFUN1_ERR				((s8)-80)
#define	DEF_GSM_CFUN11_ERR			((s8)-81)
#define	DEF_GSM_CSCLK1_ERR			((s8)-82)
#define	DEF_GSM_CSCLK0_ERR			((s8)-83)
#define	DEF_GSM_CIMI_ERR				((s8)-84)				// "AT+CIMI\r"
#define	DEF_GSM_CNUM_ERR				((s8)-85)				// "AT+CNUM\r"
#define	DEF_GSM_CPBS_ERR				((s8)-86)				// "AT+CPBS=\"ON\"\r"
#define	DEF_GSM_CPBW_ERR				((s8)-87)				// "AT+CPBW=1,\"13145678901\",145"

// TTS部分
#define	DEF_GSM_CTTS_ERR				((s8)-90)				// "AT+CTTS=1/2,string\r"	
#define	DEF_GSM_CTTSPARAM_ERR		((s8)-91)				// "AT+CTTSPARAM=<vol>,<mode>,<pitch>,<speed>,<channel>\r"
#define	DEF_GSM_CTTSRING_ERR		((s8)-92)				// "AT+CTTSRING=0/1\r"	

// FS部分
#define	DEF_GSM_FSLS_ERR				((s8)-101)			// "AT+FSLS=path\r"	
#define	DEF_GSM_FSMKDIR_ERR			((s8)-102)			// "AT+FSMKDIR=C:\User\r"
#define	DEF_GSM_FSCREATE_ERR		((s8)-103)			// "AT+FSCREATE=C:\file.txt\r"
#define	DEF_GSM_FSDEL_ERR				((s8)-104)			// "AT+FSDEL=C:\User\file.txt\r"
#define	DEF_GSM_FSREAD_ERR			((s8)-105)			// "AT+FSREAD=C:\User\file.txt,0,100,0\r"
#define	DEF_GSM_FSMEM_ERR				((s8)-106)			// "AT+FSMEM\r"
#define	DEF_GSM_FSWRITE_ERR			((s8)-107)			// "AT+FSWRITE=C:\User\file.txt,0,len,timeout\r"
#define	DEF_GSM_FSWRITECPL_ERR			((s8)-108)			// SEND DATA

// BT部分
#define	DEF_GSM_BTPOWER_ERR			((s8)-111)			// "AT+BTPOWER=1\r"
#define	DEF_GSM_BTHOST_ERR			((s8)-112)			// "AT+BTHOST=name\r"
#define	DEF_GSM_BTVIS_ERR				((s8)-113)			// "AT+BTVIS=1\r"
#define	DEF_GSM_BTSPPGET1_ERR		((s8)-114)			// "AT+BTSPPGET=1\r"
#define	DEF_GSM_BTPAIR_ERR			((s8)-115)			// "AT+BTPAIR=1,1\r"
#define	DEF_GSM_BTACPT_ERR			((s8)-116)			// "AT+BTACPT=1\r"
#define	DEF_GSM_BTSPPGET2_ERR		((s8)-117)			// "AT+BTSPPGET=2\r"
#define	DEF_GSM_BTSPPGET3_ERR		((s8)-118)			// "AT+BTSPPGET=3,len\r"
#define	DEF_GSM_BTSPPCFG_ERR		((s8)-119)			// "AT+BTSPPCFG?\r""
#define	DEF_GSM_BTSPPSEND_ERR		((s8)-120)			// "AT+BTSPPSEND=len"
#define	DEF_GSM_BTSPPSENDCPL_ERR		((s8)-121)			// SEND DATA

// GSM 硬件错误定义 ////////////////////////////////////////////////////////////////////////////////////////////////////
// (s_GSMcomm.HardWareSta)
#define	DEF_GSMHARD_INIT_ERR				((s8)-4)		// 初始化命令错误
#define	DEF_GSMHARD_SIMCARD_ERR			((s8)-3)		// SIM卡错误
#define	DEF_GSMHARD_COMM_ERR				((s8)-2)		// 通信错误
#define	DEF_GSMHARD_MODDESTROY_ERR	((s8)-1)		// 模块硬件损坏
#define	DEF_GSMHARD_NONE_ERR				((s8)0)			// 无错误
#define	DEF_GSMHARD_CHEKING					((s8)1)			// 模块检测中

// GSM 模式及其它定义	//////////////////////////////////////////////////////////////////////////////////////////////////
#define	DEF_CMD_MODE						((u8)0)					// 命令模式连接网络 
#define	DEF_DATA_MODE						((u8)1)					// 透传/数据模式连接网络  
#define	DEF_TCP_MODE						((u8)2)					// TCP协议拨号 
#define	DEF_UDP_MODE						((u8)3)					// UDP协议拨号 
#define	DEF_GSM_MODE						((u8)4)					// 短信TEXT方式发送 
#define	DEF_UCS2_MODE						((u8)5)					// 短信USC2方式发送 
#define	DEF_COMMON_MODE					((u8)6)					// 匹配所有字符									(用于AT命令返回匹配)
#define	DEF_SPECIAL_MODE				((u8)7)					// 只匹配除"OK"外的特殊字符			(用于AT命令返回匹配)
#define	DEF_ONLYOK_MODE					((u8)8)					// 只匹配"OK"										(用于AT命令返回匹配)
#define	DEF_NULL_MODE						((u8)9)					// 不匹配任何字符只等待超时退出	(用于AT命令返回匹配)
#define	DEF_COM_PHONE						((u8)10)				// 手机				(用于中断短信打包)											
#define	DEF_PHS_PHONE						((u8)11)				// 小灵通			(用于中断短信打包)	

#define	DEF_RING_FLAG						((u8)21) 				// Ring管角变化标志
#define	DEF_RINGCHECK_FLAG			((u8)22)				// 电话及短信查找标志
#define	DEF_CALLDOING_FLAG			((u8)23)				// 打电话正在处理标志
#define	DEF_SMSDOING_FLAG				((u8)24)				// 短信正在处理中标志
#define	DEF_GDATA_FLAG					((u8)25)				// GPRS在线状态标志
#define	DEF_TTSPLAY_FLAG				((u8)26)				// TTS播放中标志
#define	DEF_BTDING_FLAG					((u8)27)				// 蓝牙处于连接状态标志
#define	DEF_ALL_FLAG						((u8)28)				// 用于清空所有GSM模块工作状态标志

// GSM 值定义(s_GSMcomm.GSMComStatus)
#define	GSM_ONLINE							((u8)3)					// 已经登录到GPRS网络上
#define	GSM_ONCMD								((u8)2)					// 初始化完成但未登录到GPRS网络上	
#define	GSM_POWOK								((u8)1)					// 硬件完成开机过程并未发现错误
#define	GSM_POWOFF							((u8)0)					// 硬件还未开机	
							
// GSM通话状态定义 
#define CALL_ACTIVE							((s8)60)				// 通话状态
#define CALL_HELD								((s8)61)				// 保持状态
#define CALL_DIALLING						((s8)62)				// 拨号状态
#define CALL_ALERTING						((s8)63)				// 振铃状态
#define CALL_INCOMING						((s8)64)				// 来电状态
#define CALL_WAITING						((s8)65)				// 等待状态
#define	CALL_DOWN								((s8)66)				// 结束通话

// 定义命令标志位
#define	AllF										(0xffffffff)		// 所有GSM标志，用于模块复位时清标志使用	
#define	ATCommandF							(0x00000001)		// AT收发标志	
#define	RingComeF								(0x00000002)		// RING来电标志	
#define	RingCheckF							(0x00000004)		// RING检测标志	 							
#define	CallDoingF							(0x00000008)		// 电话处理中标志			
#define	SmsDoingF								(0x00000010)		// 收短信处理中标志
#define	GprsDoingF							(0x00000020)		// Gprs协议处理中标志	
#define	ShutCmdF								(0x00000040)		// 断网标志	
#define	TTSPlayF								(0x00000080)		// TTS语音播放中标志
#define	BTDoingF								(0x00000100)		// 蓝牙处理中标志（主要指接收数据中）								

// 定义初始化配置直接阶段(s_GSMcomm.SetupPhase) /////////////////////////////////////////////////////////////////////
#define PowerOff        				((u8)0)
#define PowerOn         				((u8)1)
#define SignalOn        				((u8)2)
#define	CheckPowPin							((u8)3)
#define	CheckStartString				((u8)4)
#define	CheckSimCardSend				((u8)5)
#define	CheckSimCard						((u8)6)
#define	SimCardOK								((u8)7)
#define InitCommand     				((u8)8)
#define CommandInitOK   				((u8)9)

// 定义命令返回码(s_GSMcomm.CmdPhase) ///////////////////////////////////////////////////////////////////////////////
#define CommandIdle     				((u8)0)					// 	idel state
#define CommandSend     				((u8)1)		 			// 	start send
#define CommandSdCpl    				((u8)2)		 			// 	cmd send success!!
#define CommandRecOk    				((u8)3)					//	Received "OK"
#define CommandRecErr   				((u8)4)					//	Received "ERROR"
#define CommandTimeout  				((u8)5)					//	Received NONE!
#define CommandWait1    				((u8)6) 				// 	"+++" AT command need 1s Idle before it.
#define CommandWait2    				((u8)7) 				// 	"+++" AT command need 1s Idle after it.
#define RecNoCarrier       			((u8)8)					//	Received "\r\nNO CARRIER\r\n"
#define	RecSms									((u8)10)	 			//	Received "\r\n+CMTI: "SM",1"
#define	RecRing									((u8)11)	 			//	Received "RING"
#define	RecCSQcpl								((u8)15)				//	Received "\r\n+CSQ: 13,0"
#define	RecCSCCcpl							((u8)16)				//	Received "\r\n+CLCC: 1,1,4,0,0,"01058302857",129,"""
#define	RecCPINcpl							((u8)19)				//	Received "\r\n+CPIN: READY"
#define	RecCOPScpl							((u8)20)				//	Received "\r\n+COPS: 0,0,"CHINA MOBILE"
#define	RecCREGcpl							((u8)21)				//	Received "\r\n+CREG: 0,1"
#define	RecCGREGcpl							((u8)22)				//	Received "\r\n+CGREG: 0,1"
#define	RecCMGSAck							((u8)23)				//	Received "\r\n>"
#define	RecCMGScpl							((u8)24)				//	Received "\r\n+CMGS: "
#define	RecCMGRAck							((u8)25)				//	Received "\r\n+CMGR: "
#define	RecSENDOKcpl						((u8)26)				//  Received "SEND OK"	
#define	RecCIFSRAck							((u8)27)				//  Received "."	
#define	RecCNUMAck							((u8)28)				//  Received "+CNUM: "	
#define	RecCPINcplErr						((u8)29)				//	Received "\r\n+CPIN: NOT INSERTED"	(未插卡SIM800模块第一次上电输出，以后查询返回ERROR)

// 网络操作
#define	RecShutCpl							((u8)61)				//	Received "SHUT OK"	
#define	RecConnectCpl						((u8)62)				//	Received "CONNECT OK" or "ALREADY CONNECT"
#define	RecConnectFail					((u8)63)				//  Received "CONNECT FAIL"
#define	RecCloseCpl							((u8)64)				//	Received "CLOSE OK"
#define	RecTCPSendAck						RecCMGSAck			//	Received "\r\n> "	
#define	RecTCPSendCpl						RecSENDOKcpl		//	Received "SEND OK"

// FS操作
#define	RecFSWriteAck						RecCMGSAck			// 	Received "\r\n>"		

// BT操作
#define	RecBTSendAck						RecCMGSAck			// 	Received "\r\n>"		
#define	RecBTSendCpl						RecSENDOKcpl		//	Received "SEND OK"				 																									

// 定义当SetupPhase == InitCommand时Active命令(s_GSMcomm.ActiveCmd)////////////////////////////////////////////////////
#define	ATE0Actived							((u8)0)					//	关闭回显
#define	ATIPRActived						((u8)1)					//	设置波特率
#define	ATIFCActived						((u8)2)					//	关闭流控制
//#define	ATCMGFActived						((u8)3)					//	设置短信模式(TEXT)	3~6操作已经独立到SIM初始化函数中
//#define	ATCSCSActived						((u8)4)					//	设置短信及网络配置字符集为"GSM"
//#define	ATCNMIActived						((u8)5)					//	设置短信提示方式自动上传
//#define	ATCPMSActived						((u8)6)					//	设置短信存储位置
#define	ATCFUNActived						((u8)3)					//	设置模块全功能工作模式
#define ATCLK0Actived						((u8)4)					//	禁止低功耗模式
#define ATTTSActived						((u8)5)					//	配置TTS
//#define	ATCFGRIActived					((u8)6)					//  使能URC上报产生RING中断
#define	ATWActived							((u8)6)					//  设置存储所有参数
#define InitCommandCpl  				((u8)7)       	// 	Max number of bgs2 init Cmd send {sizeof(pATcommand[])}
    

// 定义当SetupPhase == CommandInitOK时Active命令(s_GSMcomm.ActiveCmd)//////////////////////////////////////////////////

#define NoCmdActive    					((u8)0)
#define	ATCmdActive							((u8)80)

/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

// GSM通用事件邮件数据结构
typedef struct	tag_GSMCommon_Def
{
		u8		cmdType;			//	GSM_APP_CALL/GSM_APP_ACKCALL/GSM_APP_HUNGUPGSM_APP_CHECKCLL
												//  GSM_APP_SWCNN/GSM_APP_CFGNET/GSM_APP_CNNNET/GSM_APP_SHUTNET
		u8		callNum[20];
		u32		timeout;		
		s8		renSta;		
							
}GSMCommon_TypeDef;


typedef void (*pSendStart)(void);

/*********************************************/
typedef	struct tag_Commu_Def
{
    u16 	State;       	//Mod bit0-1 0: 空闲模式	1：接收模式			2：发送模式 
                        //RS 	bit2-3 0: 空闲状态 	1：接收进行中		2：接收完成           
                        //SS 	bit4-5 0: 空闲状态 	1：发送进行中		2：发送完成 
                                        
    u16  	RCNT;       	//	Rxd counter
    u16  	RLEN;       	//	Rxd lenght
    u16  	SCNT;       	//	Txd counter
    u16  	SLEN;       	//	Txd lenght
}Commu_Typedef;

/*********************************************/
typedef	struct tag_GsmError_Def
{
		u16		powOnTimes;				//	GSM初始化开机错误次数	
		u16		cmmTimes;					//	GSM初始化START字符串错误次数	
		u16		simCardTimes;			//	GSM初始化SIM卡检测错误次数			
		u16		initTimes;				//	GSM初始化命令错误次数	
		u16		gsmErrorTimes;		//	GSM处理总错误次数
		u16		cnnErrorTimes;		//	GSM联网错误次数
			    
}GsmErr_Typedef;

/*********************************************/

typedef	struct tag_GSMsetup_Def 
{
    volatile					Commu_Typedef     *ps_commu;      // 端口状态.
    pSendStart  			sendstart;      					// 发送函数指针
    u8       					*pSendBuf;								// 发送缓冲区指针      
    u8       					*pRecBuf;									// 接收缓冲区指针  
		vu32      				Timer;          					// 超时计时器  
		vu32							TimerApp;									// 发送超时计时器
    vu32      				CommandFlag;							// 命令标志
    vu8       				ActiveCmd; 								// 当前激活的命令     
    vu8       				SetupPhase;			 					// 配置阶段指示变量
    vu8       				CmdPhase;				 					// 命令阶段指示变量   
		s8								HardWareSta;							// GSM硬件设备状态
		vu8								GSMComStatus;							// GSM软件接口状态

}GSMcomm_Typedef;

/*********************************************/
typedef	struct tag_ATcmd_Def
{
		u16		Slen;					// 需要发送的命令长度
		u16		Rlen;					// 接收返回数据长度
		u8		*pSendBuf;		// 发送区指针
		u8		*pRecBuf;			// 接收区指针
		vu32	timeout;			// 指令发送超时大小	
		u8		ReturnCode;		// 返回码定义见CmdPhase定义
		u8		CheckMode;		// 返回码检测模式定义
			    
}ATcomm_Typedef;

/*********************************************/
typedef	struct tag_SMS_Def
{
		u8		indexBuf[GSM_SMSINDEX_SIZE];	//	短信内存索引序号	
		SYS_DATETIME	sTime;								//	短信发送的时间						
		u8		numBuf[GSM_SMSNUM_SIZE];			//	所发送的短信号码	
		u16		dataLen;											//	接收到短信数据长度
		u8		dataBuf[GSM_SMSBUF_SIZE];			//	短信数据区
					    
}SMS_Typedef;

/*********************************************/
typedef	struct tag_GPRS_Def
{
		u16		dataLen;											//	接收到数据长度
		u8		dataBuf[GSM_GDATABUF_SIZE];		//	数据区
					    
}GPRS_Typedef;

/*********************************************/
typedef	struct tag_BT_Def
{
		u16		dataLen;											//	接收到数据长度
		u8		dataBuf[GSM_BDATABUF_SIZE];		//	数据区
					    
}BT_Typedef;

/*********************************************/
typedef	struct tag_GsmRssi_Def
{
		u8		sLeve;												//	信号强度(0~99)
		u8		errRate;											//	误码率(0~99)
					    
}GRssi_Typedef;
/*********************************************/
// 定义当BT业务逻辑
#define	BT_PAIR_DIGI						((u8)0)	//	收到数字匹配申请
#define	BT_PAIR_PASS						((u8)1)	//	收到Passkey匹配申请
#define	BT_CONNECT							((u8)2)	//	收到连接申请
#define	BT_DISCONNECT						((u8)3)	//	收到断开连接申请
#define	BT_RXDATA								((u8)4)	//	收到蓝牙数据

typedef	struct tag_BTMbox_Def
{
		u8		func;													//	BT功能变量（BT_PAIR_DIGI~BT_DISCONNECT）
		u8		cnnId;												//  连接ID(字符类型)
					    
}BTMbox_Typedef;
/*********************************************/
typedef	struct tag_GPRSMbox_Def
{
		u8		func;													//	GPRS功能变量（固定为0表示收到数据）
		u8		cnn;													//  接收数据通道号（'0'~'5'字符）
					    
}GPRSMbox_Typedef;

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

BSP_GSM_EXT			GsmErr_Typedef							err_Gsm;
BSP_GSM_EXT			Commu_Typedef								ComGSM;			
BSP_GSM_EXT			GSMcomm_Typedef							s_GSMcomm;	
BSP_GSM_EXT			ATcomm_Typedef							s_ATcomm;

BSP_GSM_EXT			u8 													SBufAT[GSM_ATSBUF_TXD_SIZE];
BSP_GSM_EXT			u8 													RBufAT[GSM_ATSBUF_RXD_SIZE];       			//AT通信数据缓冲区

BSP_GSM_EXT			u8 													SBufGPRS[GSM_GPSRSBUF_TXD_SIZE];
BSP_GSM_EXT			u8 													RBufGPRS[GSM_GPSRSBUF_RXD_SIZE];       	//GPRS通信数据缓冲区

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

// 底层操作API函数
void			BSP_GSMMODULE_POWDOWN						(void);
void			BSP_GSMRING_IT_CON 							(u8	newSta);
void			BSP_GSM_POW_RESET								(u32 dlayTime);
void 			BSP_GSM_SOFT_RESET							(void);
void			BSP_GSM_POWER_CON								(void);
void			BSP_GSM_EXIT_SLEEPMODE					(void);

void			BSP_GSM_Init 										(void);
void 			BSP_GSM_Setup										(void);
void			BSP_GSM_TransPro								(void);
void			BSP_GSM_CommonPro 							(void);

void  		BSP_GSMRing_ISRHandler 					(void);
void			BSP_GSMRS232_RxTxISRHandler			(void);

void			BSP_GSM_ReleaseFlag							(u8	flagType);
void			BSP_GSM_SetFlag 								(u8	flagType);
u8				BSP_GSM_GetFlag 								(u8	flagType);

// 故障处理函数
void			BSP_GSM_ERR_Add 								(u16 *Var,	u16	AddTimes);
void			BSP_GSM_ERR_Clr 								(u16 *Var);
u16				BSP_GSM_ERR_Que 								(u16 *Var);

// 底层应用函数
void			BSP_GSMHardWarePowShut					(void);
s8				BSP_GSMHardWareReset						(u32 dlayTime);
s8				BSP_GSMSoftReset								(u32	timeout);
s8				BSP_SendATcmdPro 								(u8	checkMode,	u8	*pSendString,	u16	Slen,	u16	PendCode,	u32	timeout);
s8				BSP_SendATcmdProExt 						(u8	checkMode,	u8	*pSendString,	u16	Slen,	u16	PendCode,	u32	timeout);

// 状态查询API函数
s8				BSP_QSMSCenterPro								(u8 *pSmsCenter,	u32 timeout);
s8				BSP_QCSQCodePro									(u8 *pSLevle,	u8 *pRssi,	u32 timeout);			
s8				BSP_QIMEICodePro								(u8 *pImeiBuf,	u32 timeout);	
s8				BSP_QIMSICodePro								(u8 *pImsiBuf,	u32 timeout);
s8				BSP_QSimCardPrintPro						(u8 *pCnumBuf,	u32 timeout);
s8				BSP_QCPINStatePro								(u32	timeout);
s8				BSP_QCellGPSPosiPro							(u8 *pLonBuf, u8 *pLatBuf, u8 *pDTime,	u32 timeout);
s8				BSP_QSimCardNumPro							(u8	*pSimNum,		u32	timeout);
s8				BSP_WriteSimCardNumPro					(u8	*pSimNum,		u32	timeout);

// 睡眠部分API函数
s8				BSP_GSMIntoSleep								(u32	timeout);
s8				BSP_GSMWakeUp										(u32	timeout);

// 通用API接口函数
s8				BSP_GSMBypassPro								(u8	*sendBuf,	u16 sendLen,	u8	*recBuf,	u16 *recLen,	u16 maxRecLen,	u32 atTimeout,	u32	timeout);

// SIM卡相关初始化指令(如SIM存储,短信配置等)
s8				BSP_GSMSimCardInitPro						(u8	*pSimNum,	u32 timeout);

// 网络部分API函数
void			BSP_GPRSFroceBreakSet						(void);					
s8				BSP_GPRSResetPro								(u8 *pApn,	u8	*pUser,	 u8  *pPass,	u32	timeout);
s8				BSP_TCPConnectPro								(u8	sCnn,	u8 *pIp, u8 *pPort,	u32	timeout);
s8				BSP_TCPCloseNetPro							(u8	sCnn,	u32	timeout);
s8				BSP_TCPShutNetPro								(u32	timeout);
s8				BSP_TCPSendData									(u8	sCnn,	u8	*pBuf,	u16	len,	u32	timeout);
s8				BSP_TCPRxdData									(u8	sCnn,	u8	*pBuf,	u16	*len,	u32	timeout);
s8				BSP_TCPRxdDataUpdata						(u8 sCnn,	u8	*pBuf,	u16	*len,	u32	timeout);

// 短信部分API函数
s8				BSP_SMSSend											(u8	smsMode,	u8	*pSmsCenter,	u8	*pPhoneCode,	u8	*pData,	u16	len,	u32 timeout);
s8				BSP_SMSSendAlarm								(u8	smsMode,	u8	*pSmsCenter,	u8	*pPhoneCode,	u8	*pData,	u16	len,	u32	timeout);
s8				BSP_SMSRead											(u8	*pIndexBuf,	SYS_DATETIME *pTime,	u8	*pNumBuf,	u8	*pdBuf,	u16	*pdLen,	u32	timeout);
s8				BSP_SMSDelete										(u8	*pIndexBuf,	u32	timeout);

// 电话部分API函数
s8				BSP_CallDail										(u8	*pPhone,	u32	timeout);
s8				BSP_CheckCall										(u8 *pPhone,	u32	timeout);
s8				BSP_ChangeAudioPath							(u8	audioPath, u32 timeout);
s8				BSP_HungUpCall									(u32 timeout);
s8				BPS_AckCall											(u32 timeout);	

// TTS部分API函数
s8				BSP_TTSPlayPro									(u8 mode,	u8* pString, u16 len,	u32 timeout);
s8				BSP_TTSStopPro									(u32 timeout);
s8				BSP_TTSCfgPro										(u8 vol, u8 mode,	u8 pitch,	u8 speed,	u8 channel,	u32 timeout);
s8				BSP_TTSCfgRingPro								(u8 en,	u32 timeout);

// FS部分API函数
s8				BSP_FSCreateFilePro							(u8 *pFile,	u32 timeout);
s8				BSP_FSDelFilePro								(u8 *pFile,	u32 timeout);
s8				BSP_FSReadFilePro								(u8 *pFile,	u16 posi, u8 *pData, u16 reqLen, u16 *retLen,	u32 timeout);
s8				BSP_FSWriteFilePro							(u8 *pFile, u8 *wData, u16 wLen, u8 wTimeOut,	u32 timeout);

// BT部分API函数
s8				BSP_BTInitPro										(u8 *pName, u32 timeout);
s8				BSP_BTPairAckPro								(u8 mode,	u8 *pPass, u32 timeout);
s8				BSP_BTConnectAckPro							(u8 en,	u32 timeout);
s8				BSP_BTRxdDataPro								(u8	*pBuf,	u16	*len,	u32	timeout);
s8				BSP_BTSendDataPro								(u8	*pBuf,	u16	len,	u32	timeout);

// GSM信号量控制(用于任务删除时判断信号量占用)
s8				BSP_GSMSemPend 									(u32 timeout);
void 			BSP_GSMSemPost 									(void);
				
/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
