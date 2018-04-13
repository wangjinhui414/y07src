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
*                                           dev_include_h.h
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

#ifndef  DEV_INCLUDE_PRESENT
#define  DEV_INCLUDE_PRESENT

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   DEV_INCLUDE_MODULE
#define  DEV_INCLUDE_EXT
#else
#define  DEV_INCLUDE_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  "stm32f10x.h"
#include 	"ucos_ii.h"
#include  "hal_h.h"
#include	"bsp_h.h"
#include	"bsp_rtc_h.h"
#include	"bsp_kline_h.h"	 
#include  "bsp_can_h.h"
#include 	"hal_protocol_h.h"

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

// CAN ID定义
#define  DEF_CAN_1										(1)							
#define  DEF_CAN_2										(2)	

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
// 系统参数存储类型入参定义///////////////////////////////////////////////
//typedef enum {		
//		DEF_TYPE_COMMON = 0,			// 通用参数																	
//		DEF_TYPE_SYSCFG,					// 应用参数		
//		DEF_TYPE_SYSOBD,					// OBD参数		
//}ENUM_PARA_TYPE;

// CAN波特率定义//////////////////////////////////////////////////////////
//typedef enum {		
//		BAUD_C_100K = 0,
//		BAUD_C_125K,
//		BAUD_C_250K,
//		BAUD_C_500K,
//		BAUD_C_1000K
//}ENUM_CAN_BAUD;


// RTC时间日期结构定义////////////////////////////////////////////////////
//typedef	struct tag_Date_Time
//{
//	u16		year;									/* year */	
//	u8		month;								/* month */
//	u8		day;									/* day */
//	u8		hour;			 						/* hour */
//	u8		minute;								/* minute */
//	u8		second;								/* second */

//}SYS_DATETIME;

// OBD交互接口数据结构////////////////////////////////////////////////////
//typedef enum
//{
//		CMD_IG_ON					=0,             // 设备点火事件
//		CMD_IG_OFF,                 			// 设备熄火事件
//		CMD_SYS_WAKEUP,            				// 设备唤醒
//		CMD_AUTO_TJ,											// 自动体检(如设备升级完成或点火时使用)
//		CMD_READ_STA,											// 读取车辆状态信息
//		CMD_PROTOCOL,											// 协议控制

//}OBD_CMDCODE_TYPE;

//#define	DEF_PROPARA_SIZE					  	(200)             	// 参数区大小(实时数据流控制字长度字节最大200)
//typedef struct	tag_OBDMbox_Def
//{
//		OBD_CMDCODE_TYPE	cmdCode;														// 命令代码(见枚举定义)
//		vu16	sn;																							// 控制流水号(非协议流水号)			
//		vu16	proSn;																					// 协议流水号
//		vu16	proCmdId;																				// 协议命令ID
//		vu16	proParaLen;																			// 参数长度
//		vu8		proParaBuf[DEF_PROPARA_SIZE];										// 参数区
//		vu8		retCode;
//							
//}OBDMbox_TypeDef;

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

DEV_INCLUDE_EXT		OS_EVENT		*OS_OBDReqMbox;							// OBD请求邮箱(通讯->OBD)
DEV_INCLUDE_EXT		OS_EVENT		*OS_OBDAckMbox;							// OBD应答邮箱(OBD->通讯)

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

// 时间应用API
void					OSTmr_Count_Start				(vu32	*pCounter);
u32						OSTmr_Count_Get					(vu32	*pCounter);
void					RTCTmr_Count_Start			(vu32	*pSecond);
u32						RTCTmr_Count_Get				(vu32	*pSecond);
void 					BSP_RTC_Get_Current			(SYS_DATETIME *pDTime);		// RTC时间获取
u32 					Mktime 									(u32 year, u32 mon,u32 day, u32 hour,u32 min, u32 sec);		// 将年月日时分秒转成从1970年到现在的秒数
void 					Gettime 								(u32 count,	SYS_DATETIME *cnt);		// 将从1970年到现在的秒数转成年月日时分秒

// 系统参数读写接口API
s8						HAL_LoadParaPro 				(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);
s8						HAL_SaveParaPro 				(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);

// CAN总线应用API
void					BSP_CanWork							(u8	CAN_ID);	// CPU CAN功能工作
void					BSP_CanSleep 						(u8	CAN_ID);	// CPU CAN功能睡眠
void					BSP_CanTransWork				(u8	CAN_ID);	// CAN 收发器工作
void					BSP_CanTransSleep 			(u8	CAN_ID);	// CAN 收发器睡眠
s8  					BSP_CAN_Device_Init			(u8 CAN_ID,	u8	mode,	ENUM_CAN_BAUD	baudRate);
s8 						BSP_CAN_Setup_Baud			(u8 CAN_ID,	u8	mode,	ENUM_CAN_BAUD	baudRate);
void 					BSP_CAN_Setup_Filter32	(u8	CAN_ID,	u8	filterNum,	u8 filterMode,	u32 id,	 u32 maskId,
												 		 					 u8 assignmen,	FunctionalState enable);
void 					BSP_CAN_Setup_Filter16	(u8	CAN_ID,	u8	filterNum,	u8 filterMode,	u32 id1,	u32 maskId1,	
																			 u32	id2,	u32	maskId2,	u8 assignmen,	FunctionalState enable);																				 
s8						BSP_CAN_SendPack				(u8 CAN_ID,	CanTxMsg *Message);

// K-line应用API
void					BSP_K_SWL_EN						(void);
void					BSP_K_SWL_DIS						(void);
void 					BSP_K_Sleep						  (void);
void 					BSP_K_Work						  (void);
void 					BSP_K_Init						  (u32	baudRate);
void  				BSP_K_SendData				 	(u8 *buf,	u32	len);

// 串口调试输出API
s8						myPrintf 								(const char * format, ...);	// 带互斥访问的字符串打印输出函数


/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
