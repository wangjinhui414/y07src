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
*                                     			  bsp_kline_h.h
*                                              with the
*                                   			   Y05D Board
*
* Filename      : bsp_kline_h.h
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

#ifndef  BSP_KLINE_H_H
#define  BSP_KLINE_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_KLINE_GLOBLAS
#define  BSP_KLINE_EXT
#else
#define  BSP_KLINE_EXT  extern
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

// KRX 检测
#define		DEF_KRX_ST_YES			(0)
#define		DEF_KRX_ST_NO				(1)
#define		IO_KRX_DET()				(GPIO_ReadInputDataBit	(GPIOB, BSP_GPIOB_USART3_RX))

/************************************************************************/
/* 模式定义 定义                                                        */
/************************************************************************/

/************************************************************************/
/* 定义                                                             */
/************************************************************************/

/////////////////////Baud define /////////////////////////////////////////

#define		BAUD_DEFAULT				((u32)10400)
#define		BAUD_2400 					((u32)2400)
#define		BAUD_4800 					((u32)4800)
#define		BAUD_9600 					((u32)9600)
#define		BAUD_10400					((u32)10400)
#define		BAUD_14400					((u32)14400)
#define		BAUD_19200					((u32)19200)

/************************************************************************/
/* K 定义                                                               */
/************************************************************************/

#define		DEF_K_BAUT_RATE			(BAUD_DEFAULT)
#define		DEF_K_UART					DEF_USART_3
#define		DEF_K_REMAP					DEF_USART_REMAP_NOUSE	

#define		DEF_K_SW_L_PORTS		GPIOB											// K_SW_L 端口号
#define		DEF_K_SW_L_PINS			BSP_GPIOB_K_SW_L					// K_SW_L 管脚位置

//#define		DEF_K_SLEEP_PORTS		GPIOB											// K 收发器睡眠端口号
//#define		DEF_K_SLEEP_PINS		BSP_GPIOB_K_SW_L					// K 收发器睡眠管脚位置


// K_SW_L控制
//#define		IO_K_SWL_EN()				(GPIO_SetBits			(DEF_K_SW_L_PORTS, DEF_K_SW_L_PINS))
//#define		IO_K_SWL_DIS()			(GPIO_ResetBits		(DEF_K_SW_L_PORTS, DEF_K_SW_L_PINS))

// K 睡眠控制
//#define 	IO_K_SLEEP_EN()			(GPIO_SetBits			(DEF_K_SLEEP_PORTS, DEF_K_SLEEP_PINS))
//#define 	IO_K_SLEEP_DIS()		(GPIO_ResetBits		(DEF_K_SLEEP_PORTS, DEF_K_SLEEP_PINS))


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

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


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

// K 操作函数
void				BSP_K_SWL_EN						(void);
void				BSP_K_SWL_DIS						(void);
void 				BSP_K_Sleep						  (void);
void 				BSP_K_Work						  (void);
void 				BSP_K_Init						  (u32	baudRate);
void				BSP_KTTL_RxTxISRHandler (void);
void  			BSP_K_SendData				 	(u8 *buf,	u32	len);

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
