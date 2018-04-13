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
*                                     			  bsp_gps_h.h
*                                              with the
*                                   			   Y05D Board
*
* Filename      : bsp_gps_h.h
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

#ifndef  BSP_GPS_H_H
#define  BSP_GPS_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_GPS_GLOBLAS
#define  BSP_GPS_EXT
#else
#define  BSP_GPS_EXT  extern
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
//#define	DEF_GPSINFO_OUTPUTEN				(1)											// GPS信息输出使能

/********** global enable def**************/

#define	GPS_EN							(1)
#define	GPS_MULTIMODE_EN		(1)							// 使能接收GPS多模协议（例如：上传数据包存在BDRMC或者GLRMC等需要使能多模模式）
#define	GPS_SEND_CMD_EN			(0)							// 发送命令使能 
#define	Rec_XOR_CHECK_EN		(1)							// 接收数据校验使能
															
#define	Rec_GPRMC						(1)							// 基本定位参数，时间，经度，纬度，航向，速度	
#define	Rec_GPGGA						(1)							// 高度，卫星数量
#define	Rec_GPGLL						(0)
#define	Rec_GPGSA						(1)							// 该数据包只为取GPS定位精度状态(FS：1.2.3) 
#define	Rec_GPGSV						(0)
#define	Rec_GPVTG						(0)
#define	Rec_GPGRS						(0)
#define	Rec_GPGST						(0)
#define	Rec_GPZDA						(0)

#define	DEF_GPSANT_YES			((u8)0)					// 当GPS天线接触良好时为‘0’
#define	DEF_GPSANT_NO				((u8)1)					// 当GPS天线断开时为‘1’


/**********interface def***********/

#define	DEF_GPS_BAUT_RATE		(9600)
#define	DEF_GPS_UART				DEF_USART_2
#define	DEF_GPS_REMAP				DEF_USART_REMAP_NOUSE

#define	DEF_GPS_RTCPOWEN_PORTS	GPIOA											// GPS RTC POW EN端口号
#define	DEF_GPS_RTCPOWEN_PINS		BSP_GPIOA_GPS_RTC_POW			// GPS RTC POW EN管位置	
#define	DEF_GPS_POWEN_PORTS			GPIOA											// GPS POW EN端口号
#define	DEF_GPS_POWEN_PINS			BSP_GPIOA_GPS_POWEN				// GPS POW EN管位置		
//#define	DEF_GPS_ANT_PORTS				GPIOD											// GPS ANT端口号
//#define	DEF_GPS_ANT_PINS				BSP_GPIOD_GPS_ANT_DET			// GPS ANT管位置											

/**********common def**************/

#define	DEF_GPS_HEADSIZE					(5)
#define	DEF_MAXOF_ONEPACK					(160)								// Used in XOR check
#define	DEF_GPS_RBUFSIZE					(800)								// GPS 接收缓冲区大小(GPS一个完整数据块大小)
#define	DEF_GPS_TBUFSIZE					(10)								// GPS 发送缓冲区大小(暂时不支持GPS下发控制功能)
#define	DEF_GPS_TRY_TIMES					(3)									// GPS devices try to check times
#define	DEF_GPS_REC_TIMEROUT			(13000000)					// GPS receive one data cost time
#define	DEF_GPS_RXDDELAY_TIMEOUT	(100)								// GPS 接收分包超时时间

/*********Size of gps par	def**********/

#define	SIZEOF_Time					(10)
#define	SIZEOF_Sta					(1)
#define	SIZEOF_Latitude			(11)
#define	SIZEOF_N_S					(1)
#define	SIZEOF_Longitude		(11)
#define	SIZEOF_E_W					(1)
#define	SIZEOF_Speed				(5)
#define	SIZEOF_Azimuth			(6)
#define	SIZEOF_Date					(6)
#define	SIZEOF_mv						(3)
#define	SIZEOF_mvE					(3)
#define	SIZEOF_Mode					(1)

#define	SIZEOF_FS						(1)
#define	SIZEOF_NoSV					(2)	
#define	SIZEOF_HDOP					(4)
#define	SIZEOF_MSLHigh			(7)
#define	SIZEOF_MU						(1)
#define	SIZEOF_Altref				(4)
#define	SIZEOF_AU						(1)
#define	SIZEOF_DiffAge			(9)
#define	SIZEOF_DiffStation	(9)

#define	SIZEOF_SMODE				(1)

// GPS硬件状态定义(GPS_DEVICE.HardWareSta)
#define	DEF_GPSHARD_ANTERROR		((s8)-2)	
#define	DEF_GPSHARD_DESTROY_ERR	((s8)-1)
#define	DEF_GPSHARD_NONE_ERR		((s8)0)	
#define	DEF_GPSHARD_CHEKING			((s8)1)			

/*********Receive data State	def**********/	 

#define	GPS_ERR_PARSE				((s8)-3)
#define	GPS_ERR_LOCKED			((s8)-2)
#define	GPS_ERR_XOR					((s8)-1)
#define	GSP_DATA_OK					((s8)0)	
#define	GPS_PARSE_OK				((s8)1)
						
/********* data type	def **********/

#define	GPS_ERR_TYPE				(0x60)
#define	GPS_RMC_TYPE				(0x61)
#define	GPS_GGA_TYPE				(0x62)		
#define	GPS_GLL_TYPE				(0x63)		
#define	GPS_GSA_TYPE				(0x64)		
#define	GPS_GSV_TYPE				(0x65)		
#define	GPS_VTG_TYPE				(0x66)		
#define	GPS_GRS_TYPE				(0x67)		
#define	GPS_GST_TYPE				(0x68)
#define	GPS_ZDA_TYPE				(0x69)

/********* Bit define ************/
#define	GPS_BIT_RMC					((u16)0x0001)
#define	GPS_BIT_GGA					((u16)0x0002)
#define	GPS_BIT_GLL					((u16)0x0004)
#define	GPS_BIT_GSA					((u16)0x0008)
#define	GPS_BIT_GSV					((u16)0x0010)
#define	GPS_BIT_VTG					((u16)0x0020)
#define	GPS_BIT_GRS					((u16)0x0040)
#define	GPS_BIT_GST					((u16)0x0080)
#define	GPS_BIT_ZDA					((u16)0x0100)
		

// 注意这里用到了Rec_GP***及GPS_BIT_***的顺序关系，如果顺序修改则计算错误！！
#define	Rec_PACK_DEF				((u16)((Rec_GPRMC<<0) | (Rec_GPGGA<<1) | (Rec_GPGLL<<2) | (Rec_GPGSA<<3) |\
																	 (Rec_GPGSV<<4) | (Rec_GPVTG<<5) | (Rec_GPGRS<<6) | (Rec_GPGST<<7) |\
																	 (Rec_GPZDA<<8))) 


/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

#if	(GPS_EN > 0)

/*****************GPS device state define**********************/

typedef	struct tag_GPS_DEVICE_STA
{
		s8	InitSta;											// 初始化状态(1:表示初始化成功有效，0:表示初始化失败无效
		s8	HardWareSta;									/*power_on,idel,busy,ant_error,sleep,and so on....*/
			
}GPS_DEVICE_STA_Typedef;

/*****************GPS port state define**********************/

// GPS接口数据结构定义
typedef struct	tag_GPSPORTt_Def
{	
		vu8			rxdStep;
		vu32		rxdTimer;												// 接收超时计时器
		vu16		txLen;
		vu16		rxLen;
		u8			rxdBuf[DEF_GPS_RBUFSIZE];
		u8			txdBuf[DEF_GPS_TBUFSIZE];
		vu16		resSta;													// 已经解析的数据包状态
			 
}GPSPORT_TypeDef;							

#endif

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

#if	(GPS_EN > 0)
/*****************GPS data type define**********************/

#if	(Rec_GPRMC > 0)
typedef struct tag_GPRMC
{
		u8		Time[SIZEOF_Time+1];
		u8 		Sta[SIZEOF_Sta+1];
		u8 		Latitude[SIZEOF_Latitude+1];
		u8		N_S[SIZEOF_N_S+1];
		u8		Longitude[SIZEOF_Longitude+1];
		u8		E_W[SIZEOF_E_W+1];
		u8		Speed[SIZEOF_Speed+1];
		u8		Azimuth[SIZEOF_Azimuth+1];
		u8		Date[SIZEOF_Date+1];
		u8		mv[SIZEOF_mv+1];
		u8		mvE[SIZEOF_mvE+1];
		u8		Mode[SIZEOF_Mode+1];

}GPRMC_Typedef;

#define	MEMBER_OF_GRRMC			(12)
#endif

#if (Rec_GPGGA > 0)
typedef struct tag_GPGGA
{
		u8		Time[SIZEOF_Time+1];
		u8 		Latitude[SIZEOF_Latitude+1];
		u8		N_S[SIZEOF_N_S+1];
		u8		Longitude[SIZEOF_Longitude+1];
		u8		E_W[SIZEOF_E_W+1];
		u8		FS[SIZEOF_FS+1];									// 定位模式（2D,3D）
		u8		NoSV[SIZEOF_NoSV+1];							// 卫星数量
		u8		HDOP[SIZEOF_HDOP+1];	
		u8		MSLHigh[SIZEOF_MSLHigh+1];		    // 海拔高度
		u8		MU[SIZEOF_MU+1];			 						// 单位
		u8		Altref[SIZEOF_Altref+1];   				// 大地椭球面高度
		u8		AU[SIZEOF_AU+1];
		u8		DiffAge[SIZEOF_DiffAge+1];
		u8		DiffStation[SIZEOF_DiffStation+1];
	
}GPGGA_Typedef;

#define	MEMBER_OF_GRGGA			(14)
#endif

#if (Rec_GPGSA > 0)
typedef struct tag_GPGSA
{
		u8		Smode[SIZEOF_SMODE+1];						// 定位模式A,M
		u8		FS[SIZEOF_FS+1];									// 绑定模式1,2,3
																						// 该数据包后面还有一些参数,本程序将不解析				
	
}GPGSA_Typedef;

#define	MEMBER_OF_GRGSA			(2)
#endif

#endif
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

#if	(GPS_EN > 0)
BSP_GPS_EXT			GPS_DEVICE_STA_Typedef			GPS_DEVICE;
BSP_GPS_EXT			GPSPORT_TypeDef							GPS_PORT;

#if	(Rec_GPRMC > 0) 
BSP_GPS_EXT			GPRMC_Typedef								GPS_GPRMC;
#endif
#if	(Rec_GPGGA > 0)
BSP_GPS_EXT			GPGGA_Typedef								GPS_GPGGA;
#endif
#if	(Rec_GPGLL > 0)
BSP_GPS_EXT			GPGLL_Typedef								GPS_GPGLL;
#endif
#if	(Rec_GPGSA > 0)
BSP_GPS_EXT			GPGSA_Typedef								GPS_GPGSA;
#endif
#if	(Rec_GPGSV > 0)
BSP_GPS_EXT			GPGSV_Typedef								GPS_GPGSV;
#endif
#if	(Rec_GPVTG > 0)
BSP_GPS_EXT			GPVTG_Typedef								GPS_GPVTG;
#endif
#if	(Rec_GPGRS > 0)
BSP_GPS_EXT			GPGRS_Typedef								GPS_GPGRS;
#endif
#if	(Rec_GPGST > 0)
BSP_GPS_EXT			GPGST_Typedef								GPS_GPGST;
#endif
#if	(Rec_GPZDA > 0)
BSP_GPS_EXT			GPZDA_Typedef								GPS_GPZDA;
#endif

#endif
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
#if	(GPS_EN > 0)
void			BSP_GPSPOW_CON									(u8	newSta);
void			BSP_GPS_POWDOWN									(void);
void			BSP_GPS_POWON										(void);
void			BSP_GPSRES_IT_CON								(u8	newSta);
void			BSP_GPSTXD_IT_CON								(u8	newSta);
u8				BSP_GETGPSANT_STA								(void);
void			BSP_GPS_POW_RESET								(u32	delayTime);

void			BSP_GPS_Init 										(void);
s8 				BSP_GPS_CheckDevice							(void);

s8			  BSP_GPS_ParseRxPkt	 						(u8 *pBuf);
u8				BSP_GPS_CheckpackNum						(u8	*pBuf,	u16 len);
u16	 			BSP_GPS_ReturnPosition					(u8 num,	u8 *pBuf,	u16 len);

void 			BSP_GPSRS232_RxTxISRHandler 		(void);
void	 		BSP_GPS_TimerServer							(void);
void	 		BSP_GPS_RXBUF_UNLOCK 						(void);

#endif

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
