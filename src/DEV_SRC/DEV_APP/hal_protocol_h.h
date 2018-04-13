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
// 报警短信定义

#define	USC2_CHECK_BALANCE						("8BF760A853CA65F668C067E58F668F8600530049004D53614F59989D3002")	// 请您及时检查车辆SIM卡余额
#define	USC2_MONEY_CODE1							("4E0D8DB3")	// 不足
#define	USC2_MONEY_CODE2							("4F59989D")	// 余额
#define	USC2_MONEY_CODE3							("8BDD8D39")	// 话费

#define	GSM_MONEY_CODE1								(0x0DBB)	// 不足
#define	GSM_MONEY_CODE2								(0x599D)	// 余额
#define	GSM_MONEY_CODE3								(0xDD39)	// 话费


// 短信控制定义
#define	GSM_SMS_CONTROL_H							("WL-CTL-CMD:")						// 车辆控制信息起始
#define	GSM_SMS_NUMCHECK_H						("WL-NUM-CMD:")						// SIM卡号码请求短信起始



/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 系统协议相关定义
//////////////////////////////////////////////////////////////////////////////////////////////////////////
*/

#define	DEF_ZXT_PRO_START							((u8)0x7E)					// 协议头
#define	DEF_ZXT_PRO_PROFLAG						((u8)0x10)					// 消息标识


//////////////////////////////////////////////////////////////////////
// 通用应答类型定义
#define	DEF_ACKTYPE_OK								((u8)0x00)					// 成功/确认
#define	DEF_ACKTYPE_ERR								((u8)0x01)					// 失败
#define	DEF_ACKTYPE_BUSY							((u8)0x02)					// 忙

//////////////////////////////////////////////////////////////////////
// 命令ID定义-下行部分(服务器下发)

#define	DEF_CMDID_NAVI_PLA						((u16)0x0E01)				// 服务器下发-导航IP地址请求应答(发回新IP地址及APN)
#define	DEF_CMDID_COMACK_PLA					((u16)0x6001)       // 服务器下发-通用应答
#define	DEF_CMDID_UPDATE_PLA					((u16)0x0124)       // 服务器下发-设备升级 
#define	DEF_CMDID_UPDATEINFO_PLA			((u16)0x0125)       // 服务器下发-升级信息
#define	DEF_CMDID_UPDATEDATA_PLA			((u16)0x0126)       // 服务器下发-升级文件
#define	DEF_CMDID_UPDATEQUE_PLA				((u16)0x0131)       // 服务器下发-升级成功查询
#define	DEF_CMDID_SETIP_PLA						((u16)0x0127)       // 服务器下发-设置IP地址
#define	DEF_CMDID_SETODO_PLA					((u16)0x0132)       // 服务器下发-初始化里程
#define	DEF_CMDID_DEVRESET_PLA				((u16)0x0133)       // 服务器下发-设备重启
#define	DEF_CMDID_GPSUPTIME_PLA				((u16)0x0134)     	// 服务器下发-设备GPS采集间隔
#define	DEF_CMDID_MODULESWT_PLA				((u16)0x0135)     	// 服务器下发-设置设备模块开关指令
#define	DEF_CMDID_ENGINEBRAND_PLA			((u16)0x0137)     	// 服务器下发-发动机品牌
#define	DEF_CMDID_POWOFFDELAY_PLA			((u16)0x0140)     	// 服务器下发-延时开关机(进入睡眠)
#define	DEF_CMDID_SETTIME_PLA					((u16)0x0141)     	// 服务器下发-时间下发指令
#define	DEF_CMDID_SETPAILIANG_PLA			((u16)0x0145)     	// 服务器下发-发动机排量
#define	DEF_CMDID_SETTHRESHOLD_PLA		((u16)0x0147)     	// 服务器下发-设备阀值设置
#define	DEF_CMDID_QUEPARA_PLA					((u16)0x0148)     	// 服务器下发-设备参数查询
#define	DEF_CMDID_SMSCENTERPHO_PLA		((u16)0x0151)     	// 服务器下发-短信中心号码(需要设备回复短信到该号码)
#define	DEF_CMDID_SIMCARDPHO_PLA			((u16)0x0152)     	// 服务器下发-设备SIM卡号码
#define DEF_CMDID_SETPHONUM_PLA				((u16)0x0153)     	// 服务器下发-设置合法短信控制号码
#define	DEF_CMDID_QUEPHONUM_PLA				((u16)0x0154)     	// 服务器下发-查询合法短信控制号码
#define	DEF_CMDID_SETSLEEPTIME_PLA		((u16)0x0156)       // 服务器下发-设置设备睡眠及健康包时间间隔
#define	DEF_CMDID_RTSTART_PLA					((u16)0x0287)       // 服务器下发-专家坐席实时数据流开启
#define	DEF_CMDID_RTSTOP_PLA					((u16)0x0288)       // 服务器下发-专家坐席实时数据流停止
#define	DEF_CMDID_STARTTJ_PLA					((u16)0x0293)       // 服务器下发-开始手动体检
#define	DEF_CMDID_READFUEL_PLA				((u16)0x0296)       // 服务器下发-剩余油量查询
#define	DEF_CMDID_CONCAR_PLA					((u16)0x0A01)       // 服务器下发-车辆控制(通过OBD或外部控制模块)
#define	DEF_CMDID_CONTTS_PLA					((u16)0x0A03)       // 服务器下发-TTS控制
#define	DEF_CMDID_CLRCODE_PLA					((u16)0x0A04)       // 服务器下发-清码指令
#define	DEF_CMDID_QUECAR_PLA					((u16)0x0A06)       // 服务器下发-查询车辆状态(通过OBD或外部控制模块)


//////////////////////////////////////////////////////////////////////
// 命令ID定义-上行部分(设备上传)

#define	DEF_CMDID_NAVI_DEV						((u16)0x0E01)				// 设备上传-导航IP地址请求
#define	DEF_CMDID_COMACK_DEV					((u16)0x6001)       // 设备上传-通用应答
#define	DEF_CMDID_UPDATEREQ_DEV				((u16)0x01AD)       // 设备上传-请求升级指令
#define	DEF_CMDID_UPDATEACK_DEV				((u16)0x01AF)       // 设备上传-应答升级结果
#define	DEF_CMDID_NETBRAKREQ_DEV			((u16)0x01AA)       // 设备上传-请求断开指令
#define	DEF_CMDID_SLEEPREQ_DEV				((u16)0x01BB)       // 设备上传-请求睡眠指令
#define	DEF_CMDID_HEALTH_DEV					((u16)0x0146)       // 设备上传-设备状态上传(24H健康包)
#define	DEF_CMDID_PARAACK_DEV					((u16)0x0149)       // 设备上传-参数查询应答
#define	DEF_CMDID_ERRORLOG_DEV				((u16)0x0150)       // 设备上传-故障报文上传
#define	DEF_CMDID_PHONUMACK_DEV				((u16)0x0155)       // 设备上传-合法短信控制号码查询应答
#define	DEF_CMDID_POWON_DEV						((u16)0x0202)       // 设备上传-设备第一次上电报文
#define	DEF_CMDID_LOGIN_DEV						((u16)0x0286)       // 设备上传-设备登陆/握手$$$$$$$$
#define	DEF_CMDID_HEARD_DEV						((u16)0x0282)       // 设备上传-设备心跳
#define	DEF_CMDID_GPS_DEV							((u16)0x0283)       // 设备上传-批量GPS上传数据
#define	DEF_CMDID_ZITAI_DEV						((u16)0x0284)       // 设备上传-姿态上传数据
#define	DEF_CMDID_IG_DEV							((u16)0x0285)       // 设备上传-设备点火熄火数据
#define	DEF_CMDID_RTDATA_DEV					((u16)0x0289)       // 设备上传-实时数据流数据
#define	DEF_CMDID_RTDATAEND_DEV				((u16)0x0290)       // 设备上传-实时数据流数据结束
#define	DEF_CMDID_FLASH_DEV						((u16)0x0291)       // 设备上传-FLASH数据批量上传
#define	DEF_CMDID_VIN_DEV							((u16)0x0292)       // 设备上传-VIN码上传 
#define	DEF_CMDID_TJDATA_DEV					((u16)0x0294)       // 设备上传-体检数据上传
#define	DEF_CMDID_SMSCON_DEV					((u16)0x0295)       // 设备上传-短信车辆控制响应
#define	DEF_CMDID_READFUEL_DEV				((u16)0x0297)       // 设备上传-剩余油量查询响应
#define	DEF_CMDID_CONCARACK_DEV				((u16)0x0A02)       // 设备上传-控制结果应答
#define	DEF_CMDID_CLRCODEACK_DEV			((u16)0x0A05)       // 设备上传-清码应答
#define	DEF_CMDID_QUECARACK_DEV				((u16)0x0A07)       // 设备上传-查询车辆状态应答
#define	DEF_CMDID_ALARM_DEV						((u16)0x0A08)       // 设备上传-报警数据上传


//////////////////////////////////////////////////////////////////////
// 打包数据包相关定义

#define	DEF_PRO_MKPACK_ERRPACK				((s8)-2)
#define	DEF_PRO_MKPACK_ERRTYPE				((s8)-1)
#define	DEF_PRO_MKPACK_NONE						((s8)0)

//////////////////////////////////////////////////////////////////////
// 解析数据包相关定义

#define DEF_PRO_UNPACK_UNKNOW         ((s8)-100)
#define	DEF_PRO_UNPACK_ERRLEN					((s8)-5)
#define	DEF_PRO_UNPACK_ERRCRC					((s8)-4)
#define DEF_PRO_UNPACK_ERRPARA        ((s8)-3)
#define	DEF_PRO_UNPACK_ERRCMDID				((s8)-2)
#define	DEF_PRO_UNPACK_ERRSTART				((s8)-1)
#define	DEF_PRO_UNPACK_NONE						((s8)0)

//////////////////////////////////////////////////////////////////////
// 设备状态位定义(用于登陆/握手信息设备状态上传)
//#define	DEF_DEVSTA_BIT_TTS						((u8)0x01)					// TTS是否支持,0支持,1不支持
//#define	DEF_DEVSTA_BIT_TTSEN					((u8)0x02)					// TTS是否使能,1使能,0禁止
//#define	DEF_DEVSTA_BIT_GPS						((u8)0x04)					// GPS是否支持,0支持,1不支持
//#define	DEF_DEVSTA_BIT_GPSEN					((u8)0x08)					// GPS是否使能,1使能,0禁止
//#define	DEF_DEVSTA_BIT_BT							((u8)0x10)					// BT是否支持,0支持,1不支持
//#define	DEF_DEVSTA_BIT_BTEN						((u8)0x20)					// BT是否使能,1使能,0禁止
//#define	DEF_DEVSTA_BIT_CON						((u8)0x40)					// CON是否支持,0支持,1不支持
//#define	DEF_DEVSTA_BIT_CONEN					((u8)0x80)					// CON是否使能,1使能,0禁止

		
/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/
//////////////////////////////////////////////////////////////////////
// 短信控制返回码定义(非协议返回码)
			
#define DEF_SMSCONTROL_NONE       		((s8)0)							// 成功
#define	DEF_SMSCONTROL_FORMAT					((s8)-1)						// 格式错误
#define	DEF_SMSCONTROL_DES						((s8)-2)						// 解密失败
#define DEF_SMSCONTROL_BUSY      			((s8)-3)						// 总线忙
#define DEF_SMSCONTROL_NOSUPORT   		((s8)-4)						// 总线不支持
#define DEF_SMSCONTROL_NUM    				((s8)-5)						// 非法控制号码
#define DEF_SMSCONTROL_TIMEOUT    		((s8)-6)						// 短息控制超时
#define DEF_SMSCONTROL_TIMESTAMP    	((s8)-7)						// 短信时间戳非法
#define DEF_SMSCONTROL_INVALID    		((s8)-8)						// 无效信息

// 短信(GPRS)控制，报文非法应答返回定义
typedef enum
{
		SMS_ACK_BUSY						=0x01,      // 设备忙
		SMS_ACK_NOSUPORT				=0x02,			// 不支持
		SMS_ACK_ILLNUM					=0x03,      // 非法号码
		SMS_ACK_TIMEOUT         =0x04,			// 短信接收超时
		SMS_ACK_TIMESTAMP       =0x05,			// 短信内容时间戳超时
	
}SMS_ACK_TYPE;


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// 协议解析结构定义
#define	DEF_PARABUF_SIZE					    (1060)             	// 接收数据包参数缓冲区大小(考虑到接收升级文件大小)
#define	DEF_PARAPACK_SIZE					    (240)             	// 打包的数据包参数缓冲区大小
typedef	struct tag_PROTOCOL_Def
{
		u8		start;							// 起始字节
		u8		proFlag;						// 协议标识
		u16		proLen;							// 协议长度
		u16		bodySn;							// 报文序列号	
		u16		bodycmdId;					// 报文命令标识
		u16		paraLen;						// 参数长度
		u8		paraData[DEF_PARABUF_SIZE];			// 参数区
		u16		crcCode;						// CRC校验码

}Protocol_Typedef;

//////////////////////////////////////////////////////////////////////
// 服务器应答数据包结构
typedef struct	tag_AckData_Def
{
		u16		msgId;																					// 固定0x6001或0x0E01
		u16		ackSn;																					// 应答流水号
		u8		result;																					// DEF_ACKTYPE_OK or DEF_ACKTYPE_ERR
	
}AckData_TypeDef;

//////////////////////////////////////////////////////////////////////
// 升级过程接收数据包结构

// 升级故障码定义

#define	DEF_IAP_XUCHUAN										((u8)0xAA)			// 断点续传
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

#define	DEF_UPDATEBUF_SIZE					      (1040)          // 应答结构数据缓冲区大小(考虑到接收升级数据包情况(一次1K))
typedef struct	tag_UpdateData_Def
{
		u16		msgId;																					// 固定0x6001或0x0E01
		u16		sn;																							// 收到消息流水号
		vu8		writeLock;																			// 参数区上锁标志(0:空闲可写入，1:占用中)
		u16		dataLen;																				// 接收数据区长度															
		u8		dataBuf[DEF_UPDATEBUF_SIZE];										// 接收数据区内容
	
}UpdateData_TypeDef;

//////////////////////////////////////////////////////////////////////
// 需要设备处理的控制结构(设备处理)

// 设备控制类型定义
typedef enum
{
		TTS_CMD								=0xA0,				// TTS即时语音播报		
		CAR_CMD_IG_ON					=0xC0,        // 车辆点火
		CAR_CMD_IG_OFF				=0xC1,        // 车辆熄火
		CAR_CMD_ARM_SET				=0xC2,				// 车辆设防
		CAR_CMD_ARM_RST				=0xC3,				// 车辆撤防
		CAR_CMD_FIND_CAR			=0xC4,				// 寻车
		CAR_CMD_CATCH_CAR			=0xC5,				// 拦截车辆
		CAR_CMD_FREE_CAR			=0xC6,				// 恢复车辆
		CAR_CMD_TRUNK					=0xC7,				// 后备箱
		CAR_CMD_READSTA				=0xC8,				// 读取车辆状态
		CAR_CMD_WINUP					=0xC9,				// 升窗
		CAR_CMD_WINDOWN				=0xCA,				// 降窗

}DEV_CMDCODE_TYPE;

#define	DEF_CONDEVBUF_SIZE					  (120)             	// 参数区大小(TTS实时播报最大长度字节限制60)
typedef struct	tag_ConDevData_Def
{
		DEV_CMDCODE_TYPE		type;															// 控制类别
		vu8		dataLock;										 									 	// 数据锁(发送方置位,接收方清空)
		u16		ackSn;																					// 需要给应答的流水号
		u16		paraLen;																				// 参数长度
		u8		paraBuf[DEF_CONDEVBUF_SIZE];										// 参数区
							
}ConDevData_TypeDef;

//////////////////////////////////////////////////////////////////////
// 需要OBD处理的控制结构(OBD处理)

// OBD交互接口数据结构
typedef enum
{
		CMD_IG_ON					=0,               // 设备点火事件
		CMD_IG_OFF,                 			  // 设备熄火事件
		CMD_SYS_WAKEUP,            				  // 设备唤醒
		CMD_SYS_SLEEP,											// 设备准备进入睡眠
		CMD_AUTO_TJ,											  // 自动体检(如设备升级完成或点火时使用)
		CMD_READ_ALARMSTA,									// 读取车辆报警状态信息(每次熄火后下发OBD返回报警状态(中控锁未锁，未关车门，未关窗))
		CMD_READ_BOXSTA,										// 读取控制转接板状态信息(是否有效)
		CMD_CAR_INFO,												// 车系车型信息下发(设备每次上电推送给OBD一次)
		CMD_PROTOCOL,											  // 协议控制
		CMD_SMS,														// 短信控制
		CMD_TEST,														// 车辆总线测试
		CMD_BT,															// 蓝牙命令通信
		CMD_GET_CARDATA,										// 获取车辆数据信息(RPM状态)
		CMD_READ_FUEL,											// 获取当前油量信息
		CMD_READ_VIN,												// 请求打包VIN数据信息

}OBD_CMDCODE_TYPE;

#define	DEF_OBDBUF_SIZE					  	(500)             	// 参数区大小(用于OBD回复使用)
typedef struct	tag_OBDMbox_Def
{
		OBD_CMDCODE_TYPE	cmdCode;				  // 命令代码(见枚举定义)
		vu8		dataLock;										  // 数据锁(发送方置位,接收方清空)
		vu16	sn;													  // 控制流水号(非协议流水号)			
		vu16	proSn;											  // 协议流水号
		vu16	proCmdId;										  // 协议命令ID
		vu16	proParaLen;									  // 参数长度
		vu8		proParaBuf[DEF_OBDBUF_SIZE];		// 参数区
		vu8		retCode;
							
}OBDMbox_TypeDef;

#define	DEF_CMDBUF_SIZE					  (30)             		// 参数区大小(用于下发OBD控制命令)
typedef struct	tag_OBDMboxCmd_Def
{
		OBD_CMDCODE_TYPE	cmdCode;				  // 命令代码(见枚举定义)
		vu8		dataLock;										  // 数据锁(发送方置位,接收方清空)
		vu16	sn;													  // 控制流水号(非协议流水号)			
		vu16	proSn;											  // 协议流水号
		vu16	proCmdId;										  // 协议命令ID
		vu16	proParaLen;									  // 参数长度
		vu8		proParaBuf[DEF_CMDBUF_SIZE];		// 参数区
		vu8		retCode;
							
}OBDMboxCmd_TypeDef;

//////////////////////////////////////////////////////////////////////
// 点熄火类型
typedef enum
{
		TYPE_IG_ON		=0x00,             		// 点火
		TYPE_IG_OFF		=0x01,                // 熄火
		TYPE_IG_OFFEXT	=0x02,              // 熄火补报(补报数据无时间偏移量)
		TYPE_IG_OFFNEW	=0x03,							// 新的熄火报文格式(增加了点火信息)

}IG_TYPE;

//////////////////////////////////////////////////////////////////////
// 姿态类型
typedef enum
{
		TYPE_ACC_ATH	=0x00,             		// 急加速
		TYPE_ACC_DTH	=0x01,                // 急减速
		TYPE_ACC_ROLL	=0x02,                // 翻滚
		TYPE_ACC_CRASH=0x03,                // 碰撞
		TYPE_ACC_RTH	=0x04,                // 急转弯

}ACC_TYPE;

//////////////////////////////////////////////////////////////////////
// 报警类型
typedef enum
{
		ALARM_BIT_POWER		=0x01,            // 低压报警
		ALARM_BIT_SHAKE		=0x02,            // 震动报警
		ALARM_BIT_MOVE		=0x04,            // 熄火位移报警
		ALARM_BIT_WIN			=0x08,            // 车窗未关报警
		ALARM_BIT_DOOR		=0x10,            // 车门未关报警
		ALARM_BIT_LOCK		=0x20,            // 中控锁未关报警
		ALARM_BIT_LDOOR		=0x40,            // 锁车后车门未关报警
		ALARM_BIT_LTRUNK	=0x80,            // 锁车后后备箱未关报警
	
		ALARM_EXT_BIT_ILLOPEN	=0x0100,			// 扩展报警位非法开门或未关门提示
  
    ALARM_EXT_BIT_REMOVE	=0x010000,	  // 扩展报警位拆除提示

}ALARM_BIT_TYPE;

//////////////////////////////////////////////////////////////////////
// 故障LOG类型
typedef enum
{
		TYPE_LOG_FLASH		=0xE0,            // FLASH读写故障
		TYPE_LOG_SIM			=0xE1,            // SIM卡故障
		TYPE_LOG_GSM			=0xE2,            // GSM模块故障
		TYPE_LOG_GSMNET		=0xE3,            // GSM联网故障
		TYPE_LOG_GPS			=0xE4,            // GPS模块故障
		TYPE_LOG_OBDBUS		=0xE5,            // OBD总线通信故障
		TYPE_LOG_OBDFLAG	=0xE6,            // OBD车型代码故障
		TYPE_LOG_ACC			=0xE7,            // 加速度芯片故障
		TYPE_LOG_CPUFLASH	=0xE8,						// CPU FLASH读写故障
		TYPE_LOG_CON			=0xE9,						// 外部控制模块通信故障					

}LOG_TYPE;

//////////////////////////////////////////////////////////////////////
// DSS类型(驾驶习惯)
typedef enum
{
		TYPE_DSS_ATH	=0x00,            		// 加速
		TYPE_DSS_DTH	=0x01,            		// 减速
		TYPE_DSS_RTH	=0x02,            		// 转弯

}DSS_TYPE;

//////////////////////////////////////////////////////////////////////
// 姿态类型(驾驶习惯)
typedef enum
{
		TYPE_ZITAI_ATH	=0x00,            	// 急加速
		TYPE_ZITAI_DTH	=0x01,            	// 急减速
		TYPE_ZITAI_OTH	=0x02,            	// 翻滚
		TYPE_ZITAI_PTH	=0x03,            	// 碰撞
		TYPE_ZITAI_WTH	=0x04,            	// 急转弯

}ZITAI_TYPE;

//////////////////////////////////////////////////////////////////////
// 车辆状态类型入参(支持多状态输入)
typedef enum
{
		CAR_STATE_TYPE_LIGHT		=0x01,			// 车灯
		CAR_STATE_TYPE_DOOR			=0x02,      // 车门及后备箱
		CAR_STATE_TYPE_IG_A			=0xA1,      // 引擎状态(奥多)
		CAR_STATE_TYPE_ARM_A		=0xA2,    	// 设防状态(奥多)
		CAR_STATE_TYPE_DOOR_A		=0xA3,      // 车门及后备箱(奥多)

}CAR_STATE_TYPE;


//////////////////////////////////////////////////////////////////////
// 车辆控制短息结构

typedef	struct tag_SMSCon_Def
{
		u8		dataLock;											//  数据锁
		u8		numLen;												//	电话号码长度
		u8		numBuf[20];										//	所发送的短信号码(解密后去除‘+’)
		u16		dataLen;											//	接收到短信数据长度(解密后源数据)
		u8		dataBuf[160];									//	短信数据区(解密后源数据)		
		u8		cmdLen;												//  命令码长度
		u8		cmdCode[8];										//  命令码
		u32		smsTimStamp;									//  短信内容中时间戳(HEX数值格式,与短信内容时间戳相同)
		u8		finishFlag;										//  处理完成标志
		u8		resultCode;										//	控制指令执行结果码
					    
}SMSCon_Typedef;


//////////////////////////////////////////////////////////////////////
// OBD通信等待数据结构

//////////////////////////////////////////////////////////////////////
// 外设控制请求数据结构

typedef struct	tag_CONReqMbox_Def
{
		DEV_CMDCODE_TYPE	cmdCode;				  // 命令代码(见枚举定义)
		vu8		dataLock;										  // 数据锁(发送方置位,接收方清空)
		vu16	sn;													  // 交互流水号(非协议流水号)			
//		vu16	ackSn;											  // 协议应答SN(打包应答数据包时填入)
//		u32		timeStamp;										// 控制时间戳(打包应答数据包时填入)
							
}CONReqMbox_TypeDef;

// 外设控制应答数据结构(0x01,0x03~0x05定义为控制短信内容合法性应答)
typedef enum
{
		CAR_ACK_SUCCESS					=0x00,      // 控制成功
		CAR_ACK_UNSUPS					=0x02,      // 不支持
		CAR_ACK_AUTH            =0x06,			// 权限限制
		CAR_ACK_ILLCMD          =0x07,			// 无法识别的指令或数据
		CAR_ACK_ILLMODE					=0x08,			// 当前模式不支持此操作
		CAR_ACK_BRAKE						=0x09,			// 脚刹车未踩下不支持此操作
		CAR_ACK_HBRAKE					=0x0A,			// 手刹车未拉不支持此操作
		CAR_ACK_DRIVEING				=0x0B,			// 行驶中
		CAR_ACK_TIMEOUT					=0x0C,			// 控制超时
	
}CAR_ACK_TYPE;

typedef struct	tag_CONAckMbox_Def
{
		vu8		finishFlag;										// 应答完成标志
		vu16	sn;													  // 交互流水号(非协议流水号)		
		DEV_CMDCODE_TYPE	cmdCode;				  // 命令代码(见枚举定义)
		CAR_ACK_TYPE		retCode;						// 控制结果
							
}CONAckMbox_TypeDef;


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

HAL_PROTOCOL_EXT			AckData_TypeDef				s_ack;								// 通用应答变量结构
HAL_PROTOCOL_EXT			ConDevData_TypeDef		s_conDev1,s_conDev2;	// 通用设备控制变量结构(2个控制临时缓冲)
HAL_PROTOCOL_EXT			UpdateData_TypeDef		s_update;							// 升级过程中使用的接收数据结构
HAL_PROTOCOL_EXT			CONReqMbox_TypeDef		s_devReq;							// 外设控制请求邮箱

HAL_PROTOCOL_EXT			vu16									glob_readCarSn;				//  车辆状态读取请求sn
HAL_PROTOCOL_EXT			vu32									glob_readTimeStamp;		//  车辆状态读取请求时间戳
HAL_PROTOCOL_EXT			vu8										glob_odoBuf[3];				//	OBD体检公里数保存

HAL_PROTOCOL_EXT			OBDMboxCmd_TypeDef		s_obdCmd;							//  与OBD交互数据结构



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

// 协议相关函数
s8				HAL_ProUnpackMain								(u8	*pBuf,	u16	len);

// OBD/系统控制交互
s8				HAL_SendOBDProProcess						(OBD_CMDCODE_TYPE	cmdCode,	u16 proSn,	u16 cmdId,	u8 *pPara,	u16 len);
s8				HAL_SendCONProProcess						(DEV_CMDCODE_TYPE	cmdtype,	u16 proSn,	u8 *pPara,	u16 len);

// 设备上传的入队处理(需入队列存储)
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


// 发送打包函数///////////////////////////////////////////////////////////////////
s8				HAL_PackComAckProcess						(u8 *pPara,	u16 paraLen,	u8 *outBuf,	u16 *outLen);
s8				HAL_PackOBDDataProcess					(u16 cmdId,	u8 *pPara,	u16 paraLen,	u8 *outBuf,	u16 *outLen, u16 *retSn);
s8				HAL_PackRealTimeDataProcess			(u8 *dataType,	u8 *outBuf,	u16 *outLen, u16 *retSn);
s8				HAL_PackFlashDataProcess				(u16	maxLen,	u8 *outBuf,	u16 *outLen, u16 *retSn,	u16 *lastOut,	u16 *outNum);

// 导航/登陆/握手打包函数(实时产生无需存储)
s8 				HAL_BuildNavi_Process 					(u8 *pOutBuf,	u16 *outLen);
s8 				HAL_BulidLogin_Process 					(u8 *pOutBuf,	u16 *outLen, u16 *retSn);

// 心跳/断网/睡眠进入打包函数(实时产生无需存储)
s8 				HAL_BulidComPack_Process 				(u16	cmdId,	u8 *pOutBuf,	u16 *outLen, u16 *retSn);

// 升级文件下载处理///////////////////////////////////////////////////////////////
u8				HAL_DownLoadFileProcess					(u8 times);

// 短信控制协议处理///////////////////////////////////////////////////////////////
s8				HAl_SMSTimeOut									(u8 *pSrcBuf, u16 srcLen,	u8 *pRetBuf,	u16 *retLen);
s8				HAl_SMSCarControl								(u8 *pCmdCode,	u16 cmdCodeLen);

// 外设协议处理///////////////////////////////////////////////////////////////
u8				HAL_UpdataCarState							(u8 *outBuf,	u16	*len);


/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
