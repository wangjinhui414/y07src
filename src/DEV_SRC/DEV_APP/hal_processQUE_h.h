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
*                                     		hal_ProcessQUE_h.h
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessQUE_h.h
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

#ifndef  HAL_PROCESSQUE_H_H
#define  HAL_PROCESSQUE_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   HAL_PROCESSQUE_GLOBLAS
#define  HAL_PROCESSQUE_EXT
#else
#define  HAL_PROCESSQUE_EXT  extern
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
// 定义GPS 数据缓冲队列大小
#define	GPS_RAMDATA_SIZE							(3)							// GPS RAM数据缓冲区存储队列大小
#define	GPS_RAMSMS_SIZE								(3)							// GPS RAM短信缓冲区存储队列大小
#define	GPS_FLASHDATA_SIZE						(1296)					// GPS FLASH缓冲区存储队列大小
//#define	GPS_FLASHDATA_SIZE						(50)


// 标志定义
#define	GPS_INITED_FLAG								((u16)0x8888)		// INIT0+INIT1 表示GPS离线数据存储区已经初始化过(2015-0-9 队列结构修改)
#define	GPS_QUEUSED_FLAG							((u16)0xA55A)		// FLAG0+FLAG1 表示GPS队列结构已经使用过

// 定义GPS离线数据队列结构存储占用扇区个数
#define	GPS_QUESECTOR_NUM							(3)												// 循环次数为：扇区内包含页数量(16)*循环次数(3)=48次
#define	GPS_QUE_LOOPS									(GPS_QUESECTOR_NUM * SPI_FLASH_SectorSize / SPI_FLASH_PageSize)	
																																// 扇区1定义为初始化标志存储区，实际页数为48页循环次数为48次
// 外部部FLASH地址划分定义////////////////////////
#define	FLASH_SPI_ALL_SIZE						(SPI_FLASH_Endaddr+1)			// 由FLASH底层驱动定义
#define	FLASH_SPI_ADDR_START					(0x000000)
#define	FLASH_SPI_FLAG1_START					(FLASH_SPI_ADDR_START)		// SPI FLASH标志0起始地址
#define	FLASH_SPI_FLAG1_SIZE					(SPI_FLASH_SectorSize)		// 标志0占用空间大小(4K)预留！
#define	FLASH_SPI_FLAG2_START					(FLASH_SPI_FLAG1_START + FLASH_SPI_FLAG1_SIZE)
#define	FLASH_SPI_FLAG2_SIZE					(SPI_FLASH_SectorSize)		// 标志1占用空间大小(4K)预留！
#define	FLASH_SPI_NAME1_START					(FLASH_SPI_FLAG2_START + FLASH_SPI_FLAG2_SIZE)	
#define	FLASH_SPI_NAME1_SIZE					(SPI_FLASH_SectorSize)		// 程序备份位置1名字占用空间大小(4K)
#define	FLASH_SPI_SIZE1_START					(FLASH_SPI_NAME1_START + FLASH_SPI_NAME1_SIZE)	
#define	FLASH_SPI_SIZE1_SIZE					(SPI_FLASH_SectorSize)		// 程序备份位置1大小占用空间大小(4K)
#define	FLASH_SPI_APP1_START					(FLASH_SPI_SIZE1_START + FLASH_SPI_SIZE1_SIZE)	
#define	FLASH_SPI_APP1_SIZE						(SPI_FLASH_SectorSize*80)	// 程序备份位置1大占用空间大小(4K*80=320K)
#define	FLASH_SPI_NAME2_START					(FLASH_SPI_APP1_START  + FLASH_SPI_APP1_SIZE)	
#define	FLASH_SPI_NAME2_SIZE					(SPI_FLASH_SectorSize)		// 程序备份位置2名字占用空间大小(4K)
#define	FLASH_SPI_SIZE2_START					(FLASH_SPI_NAME2_START + FLASH_SPI_NAME2_SIZE)	
#define	FLASH_SPI_SIZE2_SIZE					(SPI_FLASH_SectorSize)		// 程序备份位置2大小占用空间大小(4K)
#define	FLASH_SPI_APP2_START					(FLASH_SPI_SIZE2_START + FLASH_SPI_SIZE2_SIZE)	
#define	FLASH_SPI_APP2_SIZE						(SPI_FLASH_SectorSize*80)	// 程序备份位置2大占用空间大小(4K*80=320K)
#define	FLASH_SPI_GPSQUE_INIT_START		(FLASH_SPI_APP2_START	 + FLASH_SPI_APP2_SIZE)	// 队列初始化标志存储地址
#define	FLASH_SPI_GPSQUE_INIT_SIZE		(SPI_FLASH_SectorSize)		// 初始化标志占用一个扇区
#define	FLASH_SPI_GPSQUE_START				(FLASH_SPI_GPSQUE_INIT_START + FLASH_SPI_GPSQUE_INIT_SIZE)
#define	FLASH_SPI_GPSQUE_SIZE					(SPI_FLASH_SectorSize*GPS_QUESECTOR_NUM)	// GPS离线数据队列结构占用空间大小
#define	FLASH_SPI_GPSDATA_START				(FLASH_SPI_GPSQUE_START	+ FLASH_SPI_GPSQUE_SIZE)
#define	FLASH_SPI_GPSDATA_SIZE				(SPI_FLASH_Endaddr - FLASH_SPI_GPSDATA_START + 1)	// GPS离线数据剩余存储空间大小

#if (GPS_FLASHDATA_SIZE <= (FLASH_SPI_GPSDATA_SIZE / 256))	// 如果定义的离线数据超过FLASH存储空间则使用实际大小
#define	GPS_FLASHDATA_USE							(GPS_FLASHDATA_SIZE - GPS_FLASHDATA_SIZE % 16)	// 队列大小必需为16的整数倍
#else
#define	GPS_FLASHDATA_USE							((FLASH_SPI_GPSDATA_SIZE/SPI_FLASH_PageSize) - ((FLASH_SPI_GPSDATA_SIZE/SPI_FLASH_PageSize) % 16))		// FLASH每页存储一个GPS点				
#endif			

/////////////////////////////////////////////////////////////////////
// 操作失败重试次数定义
#define	DEF_TRYTIMES_FLASHINIT				(3)				// SPI flash初始化障重试次数
#define	DEF_TRYTIMES_FLASHREAD				(3)				// SPI flash读重试次数
#define	DEF_TRYTIMES_FLASHWRITE				(3)				// SPI flash写重试次数	


//////////////////////////////////////////////////////////////////////

// 队列通用定义 
#define	DEF_PROQUE_QUE_OK							((s8)0)
#define	DEF_PROQUE_QUE_FULL						((s8)-1)
#define	DEF_PROQUE_QUE_EMPTY					((s8)-2)
#define	DEF_PROQUE_QUE_OVERFLOW				((s8)-3)
#define	DEF_PROQUE_QUE_ERR						((s8)-4) 
#define	DEF_PROQUE_QUE_WCMPERR				((s8)-5)

// 队列模式定义
#define	DEF_RAMQUE_LOST_MODE					(1)				// 定义队列如果满了丢掉最早的数据			
#define	DEF_FLASHQUE_LOST_MODE				(1)				// 定义队列如果满了自动丢掉最早的数据			


/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

typedef enum {		
		DEF_RAM_QUE = 0,								// 队列类型定义-RAM数据队列		
		DEF_SMS_QUE,										// 队列类型定义-RAM短信队列		
		DEF_FLASH_QUE,									// 队列类型定义-FLASH队列	
		DEF_ALL_QUE,										// 队列类型定义-所有队列	
}ENUM_QUE_TYPE;

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// 发送数据包结构

// 数据路径定义
#define	DEF_PATH_BT										((u8)2)
#define	DEF_PATH_SMS									((u8)1)
#define	DEF_PATH_GPRS									((u8)0)

// 入队打包时使用的数据类型定�
#define	DEF_TYPE_ACK									((u8)0xf0)					// 通用应答数据
#define	DEF_TYPE_GPS									((u8)0x01)					// GPS类型数据
#define	DEF_TYPE_IG										((u8)0x02)					// 点火/熄火类型数据
#define	DEF_TYPE_ZITAI								((u8)0x03)					// 姿态类型数据
#define	DEF_TYPE_LOG									((u8)0x04)					// LOG类型数据
#define	DEF_TYPE_ALARM								((u8)0x05)					// 报警类型数据
#define	DEF_TYPE_24REPORT							((u8)0x06)					// 24小时健康包类型数据
#define	DEF_TYPE_QUEPARA							((u8)0x07)					// 参数查询返回类型数据
#define	DEF_TYPE_VIN									((u8)0x08)					// VIN返回类型数据
#define	DEF_TYPE_CARCON								((u8)0x09)					// 车辆控制返回类型数据
#define	DEF_TYPE_CLRCODE							((u8)0x0A)					// 清故障码返回类型数据
#define	DEF_TYPE_CARSTA								((u8)0x0B)					// 车辆状态返回类型数据
#define	DEF_TYPE_QUEPHO								((u8)0x0C)					// 短信控制号码查询返回类型数据
#define	DEF_TYPE_SMSCOM								((u8)0x0D)					// 短信车辆控制返回类型数据
#define	DEF_TYPE_POWON								((u8)0x10)					// 设备上电报文类型数据

// SMS数据包结构报警类型定义 
#define	DEF_SMS_COM						        ((u8)0)      				// 通用类型短信
#define	DEF_SMS_ALARM						      ((u8)1)      				// 报警累短信

#define	DEF_PACK_BUF_SIZE					    (200)             	// 存储入队的数据参数区大小应该小于(256-26)
typedef struct	tag_SendData_Def
{
		u16			sum;																					// 存储的所有数据累加和(path~data[])
		u8			path;																					// DEF_PATH_GPRS/SMS 
		u8			type;																					// 数据类型见上面数据类型定义(DEF_TYPE_ACK~DEF_TYPE_CARSTA)
		u8			smsAlType;																		// 回复的短信类型，通用或是报警/path=SMS时有效
		u8			smsMode;																			// 回复的短信字符模式，GSM或是UCS2/path=SMS时有效		
		u8			smsNum[20];																		// 回复短信的电话号码/path=SMS时有效		
		u16			len;																					//
		u8			data[DEF_PACK_BUF_SIZE];											// 如果为GPRS数据则为参数区内容，如果为SMS则为实际回复的短信数据内容 
							
}SendData_TypeDef;

//////////////////////////////////////////////////////////////////////
// 发送队列结构
typedef struct	tag_DataQue_Def
{
		u16			useFlag;																			// 队列有效标志(用于指示以下队列结构是否存储信息是否有效)
		u16			sum;																					// 队列数据累加和校验字节(FLASH队列需要校验，Ram队列不使用)
		u16			start;																				// 队列开始位置
		u16			end;																					// 队列结束位置
		u16			in;																						// 队列头指针
		u16			out;																					// 队列尾指针
		u16			size;																					// 队列总大小
		u16			entries;																			// 队列当前存储数据包数
		u16			outExt;																				// 队列临时out指针(2015-9-29 主要针对FLASH队列)
		u16			entriesExt;																		// 队列临时大小(2015-9-29 主要针对FLASH队列)
							
}DataQue_TypeDef;
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

HAL_PROCESSQUE_EXT		DataQue_TypeDef				s_queRam;							// RAM中存储队列
HAL_PROCESSQUE_EXT		DataQue_TypeDef				s_queSms;							// SMS中存储队列
HAL_PROCESSQUE_EXT		DataQue_TypeDef				s_queFlash;						// FLASH中存储队列
HAL_PROCESSQUE_EXT		SendData_TypeDef			s_ramData[GPS_RAMDATA_SIZE];		// RAM数据缓冲区		
HAL_PROCESSQUE_EXT		SendData_TypeDef			s_ramSms[GPS_RAMSMS_SIZE];			// RAM短信缓冲区	

			
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

// 队列应用函数
s8				HAL_ComQue_init 								(ENUM_QUE_TYPE queType);
u16	 			HAL_ComQue_size 								(ENUM_QUE_TYPE queType);
s8	 			HAL_ComQue_in 									(ENUM_QUE_TYPE queType,	SendData_TypeDef	*sendData);
s8	 			HAL_ComQue_out 									(ENUM_QUE_TYPE queType,	SendData_TypeDef	*sendData);
s8	 			HAL_ComQue_outend 							(ENUM_QUE_TYPE queType);
s8	 			HAL_ComQue_outFinish 						(ENUM_QUE_TYPE queType,	u16 lastOut,	u16 outNum);
s8				HAL_ComQue_ForceReset 					(ENUM_QUE_TYPE queType);
s8	 			HAL_ComQue_SemPend 							(u16 timeout);
s8	 			HAL_ComQue_SemPost 							(void);


/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
