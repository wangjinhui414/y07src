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
*                                     			  bsp_rtc_h.h
*                                              with the
*                                   			   Y05D Board
*
* Filename      : bsp_rtc_h.h
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

#ifndef  BSP_RTC_H_H
#define  BSP_RTC_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_RTC_GLOBLAS
#define  BSP_RTC_EXT
#else
#define  BSP_RTC_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "stm32f10x.h"

/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////////////////
// 调试信息输出定义
#define	DEF_RTCINFO_OUTPUTEN					(0)											// RTC信息输出使能

/* Uesr inter osc as RTC clock */
#define	REC_EXT_OSC_TIMEOUT					(9000000)

#define	RTC_SAVE_FLAG								((u16)0xA5A9)

#define BKP_SAVE_FLAG_PWRON         0xAA55u

#define	Default_year								(2010)
#define	Default_month								(1)
#define	Default_day									(1)
#define	Default_hour								(0)
#define	Default_minute							(0)
#define	Default_second							(0)

#define	DEF_RTC_SEC_IT							((u8)0)
#define	DEF_RTC_ALARM_IT						((u8)1)

// 后备寄存器存储空间划分（32k~128k 容量Flash为10个，256k~1024k 容量Flash为42个）
#define	BKP_RTC_Flag								(BKP_DR1)
#define	BKP_RTC_Day									(BKP_DR2)
#define	BKP_RTC_Month								(BKP_DR3)
#define	BKP_RTC_Year								(BKP_DR4)
#define	BKP_DBG_ShieldWord					(BKP_DR5)
#define	BKP_IG_Sta									(BKP_DR6)
#define	BKP_UPDATE_Flag							(BKP_DR7)	// 生产过程升级使用
#define BKP_PWRMOVE_FLAG            (BKP_DR8)

// 定义BKP寄存器访问故障
#define	DEF_BKP_UNINIT_ERR					((s8)-1)
#define	DEF_BKP_NONE_ERR						((s8)0)

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

typedef	struct tag_Date_Time
{
	u16		year;									/* year */	
	u8		month;								/* month */
	u8		day;									/* day */
	u8		hour;			 						/* hour */
	u8		minute;								/* minute */
	u8		second;								/* second */

}SYS_DATETIME;

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

/* GLOBLAS */
BSP_RTC_EXT		SYS_DATETIME 		SYS_RTC;
BSP_RTC_EXT		vu8							rtcWakeUpFlag;			// RTC时钟唤醒标志，系统从RTC唤醒后该标志置1;

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
void 		BSP_RTC_Init												(void);
void 		BSP_RTC_Config											(void);
void 		BSP_RTC_Set_Current									(SYS_DATETIME *pDTime);
void 		BSP_RTC_Get_Current									(SYS_DATETIME *pDTime);	// 取当前日期
void 		BSP_RTC_Get_Time										(SYS_DATETIME *pDTime);	// 取当前时间
void		BSP_RTC_IT_CON											(u8	type,	u8	newSta);
void		BSP_RTCEXIT_IT_CON									(u8	newSta);
void  	BSP_RTC_ISRHandler 									(void);
void		BSP_RTCSoft_ISRHandler							(void);
void  	BSP_RTCAlarm_ISRHandler 						(void);
void		BSP_RTCSetAlarmCounter 							(u32	seconds);
void		RTCTmr_Count_Start									(vu32	*pSecond);
u32			RTCTmr_Count_Get										(vu32	*pSecond);
//void 		BSP_BACKUP_Write										(u8 BKP_Num,	u16 sData);
//void  	BSP_BACKUP_Read 										(u8 BKP_Num,	u16 *sData);
s8			BSP_BACKUP_WriteDBGWrod 						(u16 wData);
s8			BSP_BACKUP_ReadDBGWrod 							(u16 *rData);

s8			BSP_BACKUP_ClrIGOnSta								(void);
s8			BSP_BACKUP_SaveIGOnSta 							(void);
s8			BSP_BACKUP_LoadIGOnSta 							(void);

s8			BSP_BACKUP_UpdateFlagSet 						(u16 updateWord);
u16			BSP_BACKUP_UpdateFlagGet 						(void);

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
