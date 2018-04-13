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
*                                     			bsp_storage_h.h
*                                              with the
*                                   			   Y05D Board
*
* Filename      : bsp_storage_h.h
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

#ifndef  BSP_STORAGE_H_H
#define  BSP_STORAGE_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_STORAGE_GLOBLAS
#define  BSP_STORAGE_EXT
#else
#define  BSP_STORAGE_EXT  extern
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
//#define	DEF_MEMINFO_OUTPUTEN			  (1)		// 存储器全局调试信息输出使能

// SPI 接口相关参数定义
#define	DEF_FLASH_PORT							DEF_SPI_1
#define	DEF_FLASH_WP_EN							(1)		// FLASW 写保护控制使能

#define	DEF_FLASH_CS_PORTS					GPIOC											// FLASH CS端口号
#define	DEF_FLASH_CS_PINS						BSP_GPIOC_FLASH_CS				// FLASH CS管位置
#define	DEF_FLASH_WP_PORTS					GPIOC											// FLASH WP端口号
#define	DEF_FLASH_WP_PINS						BSP_GPIOC_FLASH_WP				// FLASH WP管位置

// FLASH类型定义
#define	DEF_TYPE_W25Q80							(0)		// 定义容量为1M字节
#define	DEF_TYPE_W25Q16							(1)		// 定义容量为2M字节
#define	DEF_TYPE_W25Q32							(2)		// 定义容量为4M字节
#define	DEF_FLASH_TYPE							(DEF_TYPE_W25Q80)

// 为了兼容所有容量FLASH所做的修改
#if(DEF_FLASH_TYPE == DEF_TYPE_W25Q80)
#define	SPI_FLASH_Endaddr						(0xFFFFF)	 	 // 1MByte字节
#endif
#if(DEF_FLASH_TYPE == DEF_TYPE_W25Q16)
#define	SPI_FLASH_Endaddr						(0x1FFFFF)	 // 2MByte字节
#endif
#if(DEF_FLASH_TYPE == DEF_TYPE_W25Q32)
#define	SPI_FLASH_Endaddr						(0x3FFFFF)	 // 4MByte字节
#endif

#define SPI_FLASH_PageSize 					(256)				// 页大小(可编程的最大字节数)0x100
#define	SPI_FLASH_SectorSize				(4096)			// 扇区大小(可擦除的最小单元)0x1000
#define	SPI_FLASH_BlockSize					(65536)			// 块大小0x10000


// FLASH写入对比缓冲区大小
#define	DEF_CMPWRITE_BUF_SIZE				((u16)1024)

// FLASH写失败尝试次数
#define	DEF_WRITETRY_TIMES					(3)

//////////////////////////////////////////////////////////////////////////////////
// 故障定义

// 定义硬件故障状态
#define	DEF_FLASHHARD_DEVICE_ERR		((s8)-1)
#define	DEF_FLASHHARD_NONE_ERR			((s8)0)

// 定义SPI故障状态
#define	DEF_SPI_WRITECMP_ERR				((s8)-3)
#define	DEF_SPI_TIMEOUT_ERR					((s8)-2)
#define	DEF_SPI_BUSY_ERR						((s8)-1)
#define	DEF_SPI_NONE_ERR						((s8)0)

//////////////////////////////////////////////////////////////////////////////////
// CPU FLASH 定义
#define DEF_CPUFLASH_PAGE_SIZE      (0x800)				// 大容量器件一页2K
#define	DEF_CPUFLASH_SIZE						(0x40000)			// 256K
#define	DEF_CPUFLASH_END_ADDR				(0x8040000-1)	// 105RCT6 CPU FLASH结束地址

// 定义CPU FLASH 操作故障状态
#define	DEF_CPUFLASH_FAIL_ERR				((s8)-3)
#define	DEF_CPUFLASH_TIMEOUT_ERR		((s8)-2)
#define	DEF_CPUFLASH_BUSY_ERR				((s8)-1)
#define	DEF_CPUFLASH_NONE_ERR				((s8)0)


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

typedef	struct tag_FLASH_DEVICE_STA
{
	s8	InitSta;											// 初始化状态(1:表示初始化成功有效，0:表示初始化失败无效
	s8	HardWareSta;									/*power_on,idel,busy,ant_error,sleep,and so on....*/
			
}FLASH_DEVICE_STA_Typedef;

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

BSP_STORAGE_EXT			FLASH_DEVICE_STA_Typedef		FLASH_DEVICE;		

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
/********UPDATE API***********/
// 升级过程中调用的接口函数
s8				BSP_IAP_Erase_WFlash 						(u32	addr,	u32	len);			
s8				BSP_IAP_ProgramBuf_to_WFlash 		(u8 *pSrcBuf,	u32	writeLen,	u32	WFlashAddr);

/********SPI COMMON***********/
void			BSP_FLASH_Init									(void);
s8 				BSP_FLASH_Check 								(void);

/********SPI FLASH***********/

//	不受互斥保护的底层函数用于底层直接控制调用，调用前需要接收信号量
//  并且没有WP及CS操作过程需要外部添加
u8 				BSP_SPI_FLASH_SendByte					(u8 byte);
u8 				BSP_SPI_FLASH_ReadByte					(void);
void 			BSP_SPI_FLASH_StartReadSequence	(u32 ReadAddr);


//	受互斥保护的函数
//  函数内部已经包含WP及CS的操作,WP控制可以通过.h的宏来使能
s8 				BSP_SPI_FLASH_ReadID						(u32 *id,	u32 timeout);
s8 				BSP_SPI_FLASH_SectorErase				(u32 SectorAddr,	u32	timeout);
s8 				BSP_SPI_FLASH_BlockErase				(u32 BlockAddr,		u32 timeout);
s8 				BSP_SPI_FLASH_ChipErase					(u32 timeout);
s8 				BSP_SPI_FLASH_BufferRead 				(u32 ReadAddr, 	u8* pBuffer, 	u16 NumByteToRead,	u32 timeout);
s8 				BSP_SPI_FLASH_BufferWrite 			(u32 WriteAddr,	u8* pBuffer,  u16 NumByteToWrite,	u32 timeout);
s8 				BSP_SPI_FLASH_BufferWriteExt 		(u32 WriteAddr,	u8* pBuffer,  u16 NumByteToWrite,	u32 timeout);


/*******CPU FLASH***********/
void 			BSP_CPU_FLASH_ProCheck 					(u32	addr);
s8				BSP_CPU_FLASH_PageErase					(u32	addr,	u32	len);
s8				BSP_CPU_FLASH_Write 						(u32	writeAddr,	u8	*pWrite,	u32  writeLen);
s8				BSP_CPU_FLASH_Read							(u32	readAddr,		u8 	*pRead,		u32  readLen);	

// SPI FLASH信号量控制(用于任务删除时判断信号量占用)
s8				BSP_SPI_FLASHSemPend 						(u32	timeout);
void 			BSP_SPI_FLASHSemPost 						(void);

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
