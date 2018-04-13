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
*                                     			  bsp_can_h.h
*                                              with the
*                                   			   Y05D Board
*
* Filename      : bsp_can_h.h
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

#ifndef  BSP_CAN_H_H
#define  BSP_CAN_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_CAN_GLOBLAS
#define  BSP_CAN_EXT
#else
#define  BSP_CAN_EXT  extern
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

//

#define	DEF_SYSFREQ_CUN					(72000000)						//	ϵͳ��ǰ������Ƶ
#define	DEF_SYSFREQ_MIN					(8000000)							//	ϵͳ��С��Ƶ


// CAN ����Ӧ�ñ�־λ����(�����˲������ú���)
#define	bit32_IDE								((u16)0x0004)
#define	bit32_RTR								((u16)0x0002)
#define	bit16_IDE								((u16)0x0008)
#define	bit16_RTR								((u16)0x0010)

// CAN ����Ӧ�ñ�־λ����(���ڽ��շ������ݰ������ж�)
#define	IDE_BIT									((u32)0x80000000)    /* �����˲��������ʱ��ʹ��(��ID���л����) */
#define	RTR_BIT									((u32)0x40000000)		 /* �Լ����Ϳ�������ʱ�ж��� */


/////////////////////Baud define /////////////////////////////////////////
typedef enum {		
		BAUD_C_100K = 0,
		BAUD_C_125K,
		BAUD_C_250K,
		BAUD_C_500K,
		BAUD_C_1000K
}ENUM_CAN_BAUD;

#define	DEF_CAN_FAILED_ERR			((s8)(-1))
#define	DEF_CAN_NONE_ERR				((s8)(0))

//////////////////////////////////////////////////////////////////////////////////

#define	DEF_CAN1_SLEEP_PORTS		GPIOB											// CAN1 STB �˿ں�
#define	DEF_CAN1_SLEEP_PINS			BSP_GPIOB_CAN1_STB				// CAN1 STB �ܽ�λ��
#define	DEF_CAN2_SLEEP_PORTS		GPIOB											// CAN2 STB �˿ں�
#define	DEF_CAN2_SLEEP_PINS			BSP_GPIOB_CAN2_STB				// CAN2 STB �ܽ�λ��


/* Private typedef -----------------------------------------------------------*/


/*********************************************************************/

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
void			BSP_CanWork											(u8	CAN_ID);	// CPU CAN���ܹ���
void			BSP_CanSleep 										(u8	CAN_ID);	// CPU CAN����˯��
void			BSP_CanTransWork								(u8	CAN_ID);	// CAN �շ�������
void			BSP_CanTransSleep 							(u8	CAN_ID);	// CAN �շ���˯��

s8  			BSP_CAN_Device_Init							(u8 CAN_ID,	u8	mode,	ENUM_CAN_BAUD	baudRate);
s8 				BSP_CAN_Setup_Baud							(u8 CAN_ID,	u8	mode,	ENUM_CAN_BAUD	baudRate);
void 			BSP_CAN_Setup_Filter32					(u8	CAN_ID,	u8	filterNum,	u8 filterMode,	u32 id,	 u32 maskId,
												 		 							 u8 assignmen,	FunctionalState enable);

void 			BSP_CAN_Setup_Filter16					(u8	CAN_ID,	u8	filterNum,	u8 filterMode,	u32 id1,	u32 maskId1,	
																					 u32	id2,	u32	maskId2,	u8 assignmen,	FunctionalState enable);
																				 
s8				BSP_CAN_SendPack								(u8 CAN_ID,	CanTxMsg *Message);
//void 			BSP_CAN1_RX0_IRQHandler 				(void);
//void 			BSP_CAN2_RX0_IRQHandler 				(void);

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
