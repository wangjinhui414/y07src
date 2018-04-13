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
*                                     	  hal_ProcessSYS_h.h
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessSYS_h.h
* Version       : V1.00
* Programmer(s) : BAN
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

#ifndef  HAL_PROCESSSYS_H_H
#define  HAL_PROCESSSYS_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   HAL_PROCESSSYS_GLOBLAS
#define  HAL_PROCESSSYS_EXT
#else
#define  HAL_PROCESSSYS_EXT  extern
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

//////////////////////////////////////////////////////////////////////
// 车辆状态定义用于glob_igSta 
#define	DEF_IG_RPM_START							((u8)0x11)					// RPM检测开始
#define	DEF_IG_RPM_0									((u16)0x0000)					// RPM<100
#define	DEF_IG_RPM_100								((u16)100)					// RPM>=100
#define	DEF_IG_ECU_TIMEOUT						((u16)0xFFFF)					// 总线忙/超时


//////////////////////////////////////////////////////////////////////
// 系统内部的-系统状态位定义用于s_Sys.sysState 

#define	BIT_SYSTEM_POW					      ((u32)0x00000001)		// 0:电池供电;1:外部电源供电
#define	BIT_SYSTEM_POW24					    ((u32)0x00000002)		// 0:12V系统供电;1:24V系统供电
#define	BIT_SYSTEM_ON				      		((u32)0x00000004)		// 0:ON关;1:ON开
#define	BIT_SYSTEM_IG					        ((u32)0x00000008)		// 0:熄火;1:点火
#define	BIT_SYSTEM_MOVE_ACC			      ((u32)0x00000010)		// 0:停止状态1:移动状态(由加速度判断)
#define	BIT_SYSTEM_IDLE					      ((u32)0x00000020)		// 0:点火移动:1:点火停车(IDLE)
#define	BIT_SYSTEM_LOCK					      ((u32)0x00000040)		// 0:开门;1:锁门
#define	BIT_SYSTEM_DOOR					      ((u32)0x00000080)		// 0:门关:1:门开

#define	BIT_SYSTEM_ARM					      ((u32)0x00000100)		// 0:撤防;l:设防
#define	BIT_SYSTEM_A0					        ((u32)0x00000200)		// 0:正常1:自动拦截
#define	BIT_SYSTEM_A9					        ((u32)0x00000400)		// 0:正常1:主动拦截状态
#define	BIT_SYSTEM_CATCH					    ((u32)0x00000800)		// 0:复位状态1:拦截状态
#define	BIT_SYSTEM_MUTE					      ((u32)0x00001000)		// 0:静音关闭1:静音模式
#define	BIT_SYSTEM_BELT					      ((u32)0x00002000)		// 0:安全带未系1:安全带系上
#define	BIT_SYSTEM_OBDOK					    ((u32)0x00004000)		// 0:OBD无效1:OBD有效
#define	BIT_SYSTEM_LIBAT					    ((u32)0x00008000)		// 0:锂电池无效1:锂电池有效

#define	BIT_SYSTEM_TJSTA					    ((u32)0x00010000)		// 0:未体检，1体检
#define	BIT_SYSTEM_TJSOUND_END				((u32)0x00020000)		// 0:无需播放体检结束，1需要播放体检结束语音
#define	BIT_SYSTEM_POWDOWN					  ((u32)0x00040000)		// 0:电压正常1:电压降低0.5V
#define	BIT_SYSTEM_MOVE_GPS					  ((u32)0x00080000)		// 0:停止状态1:移动状态(由GPS判断)
#define	BIT_SYSTEM_NULL11					    ((u32)0x00100000)		// 保留
#define	BIT_SYSTEM_NULL12					    ((u32)0x00200000)		// 保留
#define	BIT_SYSTEM_NULL13					    ((u32)0x00400000)		// 保留
#define	BIT_SYSTEM_NULL14					    ((u32)0x00800000)		// 保留

#define	BIT_SYSTEM_NULL15					    ((u32)0x01000000)		// 保留
#define	BIT_SYSTEM_NULL16					    ((u32)0x02000000)		// 保留
#define	BIT_SYSTEM_NULL17					    ((u32)0x04000000)		// 保留
#define	BIT_SYSTEM_NULL18					    ((u32)0x08000000)		// 保留
#define	BIT_SYSTEM_NULL19					    ((u32)0x10000000)		// 保留
#define	BIT_SYSTEM_NULL20					    ((u32)0x20000000)		// 保留
#define	BIT_SYSTEM_NULL21				    	((u32)0x40000000)		// 保留
#define	BIT_SYSTEM_NULL22					    ((u32)0x80000000)		// 保留

//////////////////////////////////////////////////////////////////////
// 系统内部的-系统报警状态位定义s_Sys.alarmState

#define	BIT_ALARM_SOS					      	((u32)0x00000001)		// 1:紧急报瞥触动报警开关后触发(收到应答后清零)
#define	BIT_ALARM_OVERSPEED				  	((u32)0x00000002)		// 1:超速报警(标志维持至报警条件解除)
#define	BIT_ALARM_TIRDDRIVE				  	((u32)0x00000004)		// 1:疲劳驾驶(标志维持至报警条件解除)
#define	BIT_ALARM_ILLALARM				  	((u32)0x00000008)		// 1:预警(收到应答后清零)
#define	BIT_ALARM_GPSERR					  	((u32)0x00000010)		// 1:GNSS模块发生故障(标志维持至报警条件解除)
#define	BIT_ALARM_ANTBREAK				  	((u32)0x00000020)		// 1:GNSS天线未接或被剪断(标志维持至报警条件解除)
#define	BIT_ALARM_ANTSHORT				  	((u32)0x00000040)		// 1:GNSS天线短路(标志维持至报警条件解除)
#define	BIT_ALARM_POWLOW					  	((u32)0x00000080)		// 1:终端主电源欠压(标志维持至报警条件解除)

#define	BIT_ALARM_POWREMOVE				  	((u32)0x00000100)		// 1:终端主电源掉电(标志维持至报警条件解除)
#define	BIT_ALARM_DIRVEOVT				  	((u32)0x00000200)		// 1:当天累计驾驶超时(标志维持至报警条件解除
#define	BIT_ALARM_PARKOVT					  	((u32)0x00000400)		// 1:超时停车(标志维持至报警条件解除)
#define	BIT_ALARM_ILLIG					    	((u32)0x00000800)		// 1:车辆非法点火(收到应答后清零)
#define	BIT_ALARM_ILLMOVE					  	((u32)0x00001000)		// 1:车辆非法位移
#define	BIT_ALARM_OILERR					  	((u32)0x00002000)		// 1:车辆油量异常(标志维持至报警条件解除)
#define	BIT_ALARM_ILLOPEN				    	((u32)0x00004000)		// 1:车辆非法开门(通过车辆防盗器)(标志维持至报警条件解除)
#define	BIT_ALARM_SIMERR					  	((u32)0x00008000)		// 1:SIM卡余额不足(收到应答后清零)

#define	BIT_ALARM_CRASH					    	((u32)0x00010000)		// 1:疑似发生碰撞报警(收到应答后清零)
#define	BIT_ALARM_NULL2		  	  	    ((u32)0x00020000)		// 保留
#define	BIT_ALARM_NULL3				  			((u32)0x00040000)		// 保留
#define	BIT_ALARM_NULL4					  		((u32)0x00080000)		// 保留
#define	BIT_ALARM_NULL5					    	((u32)0x00100000)		// 保留
#define	BIT_ALARM_NULL6					    	((u32)0x00200000)		// 保留
#define	BIT_ALARM_NULL7					  		((u32)0x00400000)		// 保留
#define	BIT_ALARM_NULL8					  		((u32)0x00800000)		// 保留

#define	BIT_ALARM_NULL9					  		((u32)0x01000000)		// 保留
#define	BIT_ALARM_NULL10					  	((u32)0x02000000)		// 保留
#define	BIT_ALARM_NULL11				    	((u32)0x04000000)		// 保留
#define	BIT_ALARM_NULL12					  	((u32)0x08000000)		// 保留
#define	BIT_ALARM_NULL13					  	((u32)0x10000000)		// 保留
#define	BIT_ALARM_NULL14					  	((u32)0x20000000)		// 保留
#define	BIT_ALARM_NULL15					  	((u32)0x40000000)		// 保留
#define	BIT_ALARM_NULL16					  	((u32)0x80000000)		// 保留

//////////////////////////////////////////////////////////////////////
// 系统状态数据类型定义
typedef struct	tag_SYSState_Def
{						
		vu32		sysState;																			// 系统全局状态
		vu32		alarmState;																		// 系统全局报警状态
								
}SYSState_TypeDef;

//////////////////////////////////////////////////////////////////////
// AD采集数据类型定义
#define DEF_ADC_MAXNUM								5										// AD转换计算平均值所用的源数据个数
typedef struct	tag_ADInfo_Def
{	
		u16			VBatVol;																			// 外部电源电压值(算法处理后扩大100倍)	
		u16			ADCVBatBK;																		// 外部电源12VADC基准值
		u16			ADCVBat[DEF_ADC_MAXNUM];											// 外部电源AD原始数据缓存

}ADCInfo_Typedef;

//////////////////////////////////////////////////////////////////////
// 系统板载硬件相关类型定义(HAL_BSP_SetState()HAL_BSP_GetState()入参)
typedef enum {		
		DEF_BSPSTA_ON = 0,								// ON检测线状态(对于Y06产品无该信号)	
		DEF_BSPSTA_IG,										// IG检测线状态(对于Y06产品为低脉冲信号线即wakeup信号线)	
		DEF_BSPSTA_GPSSTA,								// GPS是否定位有效'A'or'V'
		DEF_BSPSTA_GPS3D,									// GPS是否为3D定位
		DEF_BSPSTA_GPSSPEED,							// GPS速度

}ENUM_BSP_STA;


//////////////////////////////////////////////////////////////////////
// 触发类数据结构
typedef struct	tag_Trigger_Def
{						
		bool		up;
		bool		upup;
										
}Trigger_TypeDef;

//////////////////////////////////////////////////////////////////////
// 事件状态结构(基于触发机制的上次状态与本次状态的对比)
typedef struct	tag_EventTriType_Def
{						
		Trigger_TypeDef		on;					// ON按键状态（Y06产品无该信号线）
		Trigger_TypeDef		ig;					// 点火按键状态
		Trigger_TypeDef		igOffMov;		// 熄火移动按键状态
		Trigger_TypeDef		lowPow;			// 电压低按键状态
		Trigger_TypeDef		gpsFixSta;	// GPS定位有效按键状态
		Trigger_TypeDef		conErrSta;	// CON控制模块故障状态		
		Trigger_TypeDef		healtySta;	// 健康包数据
		Trigger_TypeDef		dssATHSta;	// DSS急加速
		Trigger_TypeDef		dssDTHSta;	// DSS急减速
		Trigger_TypeDef		dssRTHSta;	// DSS急转弯
								
}EventTri_TypeDef;

//////////////////////////////////////////////////////////////////////
// 油耗统计结构

typedef struct	tag_FuelValType_Def
{			
		vu8		step;										// 阶段状态
		u32		up;											// 上次油量
		u32		cun;										// 当前油量
		u8		type;										// 类型(0-不支持，1-百分比，2-油量)
		u32		sum;										// 累加值		
								
}FuelVal_TypeDef;

	
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

HAL_PROCESSSYS_EXT		ADCInfo_Typedef				s_adInfo;			// AD采集全局结构变量
HAL_PROCESSSYS_EXT		SYSState_TypeDef			s_sys;				// 系统全局状态结构变量
HAL_PROCESSSYS_EXT		EventTri_TypeDef			s_event;			// 触发类型事件结构体变量
HAL_PROCESSSYS_EXT		vu16									glob_igSta,glob_igStaExt,glob_igStaLast;		// 真实熄火状态变量
HAL_PROCESSSYS_EXT		vu8										glob_accOnRalStep;	//  ACC判断车辆点火阶段变量
HAL_PROCESSSYS_EXT		vu8										glob_accAdjustFlag;	//  加速度角度调整全局标志
HAL_PROCESSSYS_EXT		FuelVal_TypeDef				s_fuel;				// 油耗计算结构
			
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
void 					HAL_IG_IRQHandlers							(void);
void					HAL_EventAlarmProcess 			    (u32 	eventSwt);
void			    HAL_BSP_SetState 								(ENUM_BSP_STA type,	u8 newSta);
u32				    HAL_BSP_GetState 								(ENUM_BSP_STA type);
void					HAL_SYS_SetState 								(u32 staBit,	u8 newSta);
u8						HAL_SYS_GetState 								(u32 staBit);
void					HAL_SYSALM_SetState 						(u32 almBit,	u8 newSta);
u8						HAL_SYSALM_GetState 						(u32 almBit);

u8            HAL_GetLastPowerRemoveSt        (void);
void          HAL_SetLastPowerRemoveSt        (u8 ucSt);

u8 HAL_GetIgonAccJudgeIgoffFlag(void);

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
