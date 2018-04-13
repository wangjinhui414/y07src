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
// 参数配置查询指令
#define	DBG_CMD_CODE_QCOMON							("Q_COMON")					//	查询系统通用参数
#define	DBG_CMD_CODE_QSYSCFG						("Q_SYSCFG")				//	查询系统应用参数
#define	DBG_CMD_CODE_QBAK   						("Q_BAK")				    //	查询备份区数据
#define	DBG_CMD_CODE_QCOMONEXT					("Q_COMONEXT")			//	查询RAM系统通用参数
#define	DBG_CMD_CODE_QSYSCFGEXT 				("Q_SYSCFGEXT")			//	查询RAM系统应用参数
#define	DBG_CMD_CODE_QSYSOBD						("Q_SYSOBD")				//	查询系统OBD参数
#define	DBG_CMD_CODE_QVERB							("Q_VERB")					//  查询通讯系统软件版本号
#define	DBG_CMD_CODE_SVERB							("S_VERB")					//  设置版本号(这里只设置升级版本号无法修改内部版本号)

#define	DBG_CMD_CODE_SETDFT							("S_DFT")						//  设置参数为默认值
#define	DBG_CMD_CODE_SETID							("S_ID")						//	设置设备ID号(6位)
#define	DBG_CMD_CODE_QUEID							("Q_ID")						//  查询设备ID号
#define	DBG_CMD_CODE_CV									("S_CV")						//	自动校验电压
#define	DBG_CMD_CODE_SETAADC						("S_AADC")					//	设置设备AD基准
#define	DBG_CMD_CODE_QUEAADC						("Q_AADC")					//  查询设备AD基准
#define	DBG_CMD_CODE_SETTIME						("S_TIME")					//	设置设备RTC时钟(GMT时间即为标准时间-8小时)	
#define	DBG_CMD_CODE_QUETIME						("Q_TIME")					//	查询设备RTC时钟
#define	DBG_CMD_CODE_SETAPN							("S_APN")						//	设置APN
#define	DBG_CMD_CODE_QUEAPN							("Q_APN")						//	查询APN
#define	DBG_CMD_CODE_SETIP							("S_IP")						//	设置IP地址
#define	DBG_CMD_CODE_QUEIP							("Q_IP")						//	查询IP地址
#define	DBG_CMD_CODE_SETUPDATE					("S_UPDATE")				//  设置开始本地升级
#define	DBG_CMD_CODE_SETUPDATE1					("S_UPDATE1")				//  设置触发BKP升级
#define	DBG_CMD_CODE_SETOTAP						("S_OTAP")					//  设置开始远程升级
#define	DBG_CMD_CODE_QUEQUEUE						("Q_QUE")						//	查询队列状态
#define	DBG_CMD_CODE_SETQUEUE						("S_QUE")						//  清空队列信息
#define	DBG_CMD_CODE_QUECARINFO					("Q_CARINFO")				//  查询车系车型发动机品牌
#define	DBG_CMD_CODE_SETCARINFO					("S_CARINFO")				//  设置车系车型发动机品牌
#define	DBG_CMD_CODE_QGPSRPTTIME				("Q_GPSRPT")				//	查询GPS采集及上报时间间隔单位s
#define	DBG_CMD_CODE_SGPSRPTTIME				("S_GPSRPT")				//	设置GPS采集及上报时间间隔单位s
#define	DBG_CMD_CODE_QUEHEALTHRPT				("Q_HEALTHRPT")			//  查询设备健康包发送时间间隔单位s
#define	DBG_CMD_CODE_SETHEALTHRPT				("S_HEALTHRPT")			//  设置设备健康包发送时间间隔单位s
#define	DBG_CMD_CODE_QUEIGOFFDELAY			("Q_IGOFFDELAY")		//  查询设备熄火状态判断延时时间单位s
#define	DBG_CMD_CODE_SETIGOFFDELAY			("S_IGOFFDELAY")		//  设置设备熄火状态判断延时时间单位s
#define	DBG_CMD_CODE_QUEDSS							("Q_DSS")						//	查询驾驶习惯参数
#define	DBG_CMD_CODE_SETDSS							("S_DSS")						//  设置驾驶习惯参数
#define	DBG_CMD_CODE_QUEACC							("Q_ACC")						//	查询加速度中断相关参数
#define	DBG_CMD_CODE_SETACC							("S_ACC")						//	设置加速度中断相关参数
#define	DBG_CMD_CODE_QUESLEEPTIME				("Q_SLEEPTIME")			//	查询设备睡眠时间单位s
#define	DBG_CMD_CODE_SETSLEEPTIME				("S_SLEEPTIME")			//	设置设备睡眠时间单位s
#define	DBG_CMD_CODE_QUEIGOFFMOVE				("Q_IGOFFMOVE")			//	查询系统熄火移动报警参数
#define	DBG_CMD_CODE_SETIGOFFMOVE				("S_IGOFFMOVE")			//	设置系统熄火移动报警参数
#define	DBG_CMD_CODE_SETDEVEN						("S_DEVEN")					//	设置模块使能开关
#define	DBG_CMD_CODE_QUEDEVEN						("Q_DEVEN")					// 	查询模块使能开关
#define	DBG_CMD_CHANGE_SETAPPIP					("S_APP2")					//	设置应用IP
#define	DBG_CMD_CHANGE_QUEAPPIP					("Q_APP2")					// 	查询应用IP


#define	DBG_CMD_CODE_JUMP1						  ("JUMP1")					// 	跳转到1应用
//////////////////////////////////////////////////////////////////////////////////
// 控制指令
#define	DBG_CMD_CODE_RESET							("C_RESET")					//	设备软件复位
#define	DBG_CMD_CODE_SHUTNET						("C_SHUTNET")				//  强制断开网络
#define	DBG_CMD_CODE_GSMBYPASS					("C_GSMBYPASS")			// 	GSM透传指令
#define	DBG_CMD_CODE_BTBYPASS						("C_BTBYPASS")			// 	BT透传指令
#define	DBG_CMD_CODE_TTSBYPASS					("C_TTSBYPASS")			// 	TTS透传指令

//////////////////////////////////////////////////////////////////////////////////
// 以下命令为临时调试命令，状态存储在backup寄存器中
#define	DBG_CMD_SHOW										("SHOW")						//  显示信息头
#define	DBG_CMD_HIDE										("HIDE")						//  隐藏信息头
#define	DBG_CMD_SHOW_ALL								("SHOW_ALL")				//  显示ALL信息
#define	DBG_CMD_HIDE_ALL								("HIDE_ALL")				//	隐藏ALL信息
#define	DBG_CMD_SHOW_RTC								("SHOW_RTC")				//  显示RTC信息
#define	DBG_CMD_HIDE_RTC								("HIDE_RTC")				//  隐藏RTC信息
#define	DBG_CMD_SHOW_SYS								("SHOW_SYS")				//  显示SYS信息
#define	DBG_CMD_HIDE_SYS								("HIDE_SYS")				//  隐藏SYS信息
#define	DBG_CMD_SHOW_GSM								("SHOW_GSM")				//  显示GSM信息
#define	DBG_CMD_HIDE_GSM								("HIDE_GSM")				//  隐藏GSM信息
#define	DBG_CMD_SHOW_GSMSTA							("SHOW_GSMSTA")			//  显示GSMSTA信息
#define	DBG_CMD_HIDE_GSMSTA							("HIDE_GSMSTA")			//  隐藏GSMSTA信息
#define	DBG_CMD_SHOW_GPS								("SHOW_GPS")				//  显示GPS信息
#define	DBG_CMD_HIDE_GPS								("HIDE_GPS")				//  隐藏GPS信息
#define	DBG_CMD_SHOW_GPSSTA							("SHOW_GPSSTA")			//  显示GPSSTA信息
#define	DBG_CMD_HIDE_GPSSTA							("HIDE_GPSSTA")			//  隐藏GPSSTA信息
#define	DBG_CMD_SHOW_CON								("SHOW_CON")				//  显示CON信息
#define	DBG_CMD_HIDE_CON								("HIDE_CON")				//  隐藏CON信息
#define	DBG_CMD_SHOW_OS									("SHOW_OS")					//  显示OS信息
#define	DBG_CMD_HIDE_OS									("HIDE_OS")					//  隐藏OS信息
#define	DBG_CMD_SHOW_OBD								("SHOW_OBD")				//  显示OBD信息
#define	DBG_CMD_HIDE_OBD								("HIDE_OBD")				//  隐藏OBD信息
#define	DBG_CMD_SHOW_ADC								("SHOW_ADC")				//  显示ADC信息
#define	DBG_CMD_HIDE_ADC								("HIDE_ADC")				//  隐藏ADC信息
#define	DBG_CMD_SHOW_ACC								("SHOW_ACC")				//  显示ACC信息
#define	DBG_CMD_HIDE_ACC								("HIDE_ACC")				//  隐藏ACC信息
#define	DBG_CMD_SHOW_GPRS								("SHOW_GPRS")				//  显示GPRS信息
#define	DBG_CMD_HIDE_GPRS								("HIDE_GPRS")				//  隐藏GPRS信息
#define	DBG_CMD_SHOW_SMS								("SHOW_SMS")				//  显示SMS信息
#define	DBG_CMD_HIDE_SMS								("HIDE_SMS")				//  隐藏SMS信息
#define	DBG_CMD_SHOW_BT									("SHOW_BT")					//  显示BT信息
#define	DBG_CMD_HIDE_BT									("HIDE_BT")					//  隐藏BT信息
#define	DBG_CMD_SHOW_EVENT							("SHOW_EVENT")			//  显示EVENT信息
#define	DBG_CMD_HIDE_EVENT							("HIDE_EVENT")			//  隐藏EVENT信息
#define	DBG_CMD_SHOW_OTHER							("SHOW_OTHER")			//  显示OTHER信息
#define	DBG_CMD_HIDE_OTHER							("HIDE_OTHER")			//  隐藏OTHER信息

//////////////////////////////////////////////////////////////////////////////////
// 测试命令
#define	DBG_CMD_TEST_TEST1							("T_TEST1")					//	产生24小时报告数据包
#define	DBG_CMD_TEST_TEST2							("T_TEST2")					//	产生故障报文数据包
#define	DBG_CMD_TEST_TEST3							("T_TEST3")					//	产生GPS报文数据包
#define	DBG_CMD_TEST_TEST4							("T_TEST4")					//	产生姿态报文数据包
#define	DBG_CMD_TEST_TEST5							("T_TEST5")					//	产生GPS拐点补报报文数据包
#define	DBG_CMD_TEST_TEST6							("T_TEST6")					//	产生低压报警
#define	DBG_CMD_TEST_TEST7							("T_TEST7")					//	产生阀值查询应答数据包
#define	DBG_CMD_TEST_TJ									("T_TJ")						//	手动触发OBD体检
#define	DBG_CMD_TEST_TJ1								("T_TJ1")						//	手动触发升级后的OBD体检

#define	DBG_CMD_TEST_IG_ON							("T_IGON")					//	手动触发点火
#define	DBG_CMD_TEST_IG_OFF							("T_IGOFF")					//	手动触发熄火
#define	DBG_CMD_TEST_ARM_SET						("T_ARMSET")				//	手动触发设防
#define	DBG_CMD_TEST_ARM_RST						("T_ARMRST")				//	手动触发撤防
#define	DBG_CMD_TEST_FIND_CAR						("T_FINDCAR")				//	手动触发寻车
#define	DBG_CMD_TEST_READ_STA						("T_READSTA")				//	手动触发状态读取
#define	DBG_CMD_TEST_TRUNK							("T_TRUNK")					//	手动触发开启后备箱

#define	DBG_CMD_TEST_FLASH							("T_FLASH")					//	FLASH芯片测试	
#define	DBG_CMD_TEST_ACC								("T_ACC")						//	ACC加速度芯片测试
#define	DBG_CMD_TEST_GPS								("T_GPS")						//	GPS模块测试
#define	DBG_CMD_TEST_GSM								("T_GSM")						//	GSM模块测试
#define	DBG_CMD_TEST_CAN								("T_CAN")						//	GAN总线测试
#define	DBG_CMD_TEST_K									("T_K")							//	K总线测试
#define	DBG_CMD_TEST_UART1							("T_UART1")					//	外部串口测试
#define	DBG_CMD_TEST_CSQ								("T_CSQ")						//	GSM信号强度测试

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


// DBG接口数据结构定义

typedef struct	tag_DBGPort_Def
{
		u8			rxFlag;
		u16			txlen;
		u16			rxlen;
		u8			rxdBuf[DEF_DBG_BUFSIZE];
			 
}DBGPort_TypeDef;	


typedef struct	tag_TESTStr_Def
{
		u8			busTestCode;			// 总线测试码，0-can测试，1-k测试，2-控制串口测试
			 
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
