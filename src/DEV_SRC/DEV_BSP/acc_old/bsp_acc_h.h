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
*                                     	     bsp_acc_h.h
*                                              with the
*                                   			   Y05D Board
*
* Filename      : bsp_acc_h.h
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

#ifndef  BSP_ACC_H_H
#define  BSP_ACC_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_ACC_GLOBLAS
#define  BSP_ACC_EXT
#else
#define  BSP_ACC_EXT  extern
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
//#define	DEF_ACCINFO_OUTPUTEN				(1)											// ACC信息输出使能


/* ACC接口相关参数定义 */
#define	DEF_ACC_INT1_EXTI_LINE			EXTI_Line0							// ACC INT1中断线
#define	DEF_ACC_INT1_EXTI_TRIGGER		EXTI_Trigger_Falling		// ACC INT1触发方式
#define	DEF_ACC_INT1_EXTI_PORTS			GPIO_PortSourceGPIOC		// ACC INT1端口号
#define	DEF_ACC_INT1_EXTI_PINS			GPIO_PinSource0					// ACC INT1管位置

//#define	DEF_ACC_INT2_EXTI_LINE			EXTI_Line1							// ACC INT2中断线
//#define	DEF_ACC_INT2_EXTI_TRIGGER		EXTI_Trigger_Falling		// ACC INT2触发方式
//#define	DEF_ACC_INT2_EXTI_PORTS			GPIO_PortSourceGPIOB		// ACC INT2端口号
//#define	DEF_ACC_INT2_EXTI_PINS			GPIO_PinSource1					// ACC INT2管位置

/*************** IO定义 ******************/
//#define	DEF_ACC_CS_PORTS						GPIOC	
//#define	DEF_ACC_CS_PINS					  	BSP_GPIOC_ACC_CS   //high I2C bus,low SPI bus 

// 通信总线定义
#define	DEF_ACC_BUSTYPE							DEF_IIC_BUS							// 确定通信总线类型(见bsp_h.h定义)
#define DEF_ACC_PORT      					DEF_IIC_1								// 确定IIC总线通道(见bsp_h.h定义)

/* accelerometer register address define */
#define	Status_Reg1								  ((u8)0x07)
#define	Who_Am_I									  ((u8)0x0F)
#define	Ctrl_Reg1									  ((u8)0x20)
#define	Ctrl_Reg2									  ((u8)0x21)
#define	Ctrl_Reg3									  ((u8)0x22)
#define	Ctrl_Reg4								  	((u8)0x23)
#define	Ctrl_Reg5								  	((u8)0x24)
#define	Ctrl_Reg6								  	((u8)0x25)
#define	Status_Reg2							  	((u8)0x27)
#define	Out_X_L									  	((u8)0x28)
#define	Out_X_H									  	((u8)0x29)
#define	Out_Y_L									  	((u8)0x2A)
#define	Out_Y_H									  	((u8)0x2B)
#define	Out_Z_L									  	((u8)0x2C)
#define	Out_Z_H									  	((u8)0x2D)
#define	INT1_CFG  							  	((u8)0x30)
#define	INT1_SRC  							  	((u8)0x31)
#define	INT1_THS  							  	((u8)0x32)
#define	INT1_DURATION   				  	((u8)0x33)
#define	CLICK_CFG								  	((u8)0x38)
#define	CLICK_SRC								  	((u8)0x39)
#define	CLICK_THS   						  	((u8)0x3A)
#define	TIME_LIMIT							  	((u8)0x3B)
#define	TIME_LATENCY  					  	((u8)0x3C)
#define	TIME_WINDOW		  				  	((u8)0x3D)

/* 定义检测传感器状态寄存器次数 */
#define	CHECK_TIME							  	(2)
/* 定义总线互斥等待时间 1s */
#define	TIME_OUT   							  	(1000)
/* 定义传感器原始数据单位 4mg / -2g~+2g */
#define	MEMS_UNIT								  	(1)

/* 定义硬件故障状态 */	
#define	DEF_MEMSHARD_DEVICE_ERR			((s8)-1)
#define	DEF_MEMSHARD_NONE_ERR				((s8)0)

/* 定义输出信息使能 */
#define	INFOPRINT_EN						  	(1)


/************************  ADD工作参数定义******************************************************/

/* ADD 工作模式定义 作用于	CTRL_REG1(20h) bit7~bit4位 */
#define	DEF_ODR_MODE_POWERDOWN			((u8 )0x00)
#define	DEF_ODR_NOMAL_1HZ						((u8 )0x10)
#define	DEF_ODR_NOMAL_10HZ					((u8 )0x20)
#define	DEF_ODR_NOMAL_25HZ					((u8 )0x30)
#define	DEF_ODR_NOMAL_50HZ					((u8 )0x40)
#define	DEF_ODR_NOMAL_100HZ					((u8 )0x50)
#define	DEF_ODR_NOMAL_200HZ					((u8 )0x60)
#define	DEF_ODR_NOMAL_400HZ					((u8 )0x70)
#define	DEF_ODR_MODE_LOWPOWER				((u8 )0x80)
#define	DEF_ODR_MODE_N125L5					((u8 )0x90)

/* ADD 量程定义 作用于	CTRL_REG4(23h) bit5~bit4位 */
#define	DEF_FS_SCALE_2G							((u8 )0x00)
#define	DEF_FS_SCALE_4G							((u8 )0x10)
#define	DEF_FS_SCALE_8G							((u8 )0x20)
#define	DEF_FS_SCALE_16G						((u8 )0x30)

/* ADD 工作参数设定*/
#define	DEF_WORK_MODE								(DEF_ODR_NOMAL_100HZ)
#define	DEF_WORK_SCALE							(DEF_FS_SCALE_2G)
#define	DEF_WORK_GRAVITY_EN					(0)											// 禁止后将关闭重力加速度的叠加干扰(使能后正常放置Z轴有+1000mg的叠加) 

/************************  BSP_ACCELEROMETER 函数参数定义	***************************************/


/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/
typedef struct
{
		s16 X_data;//unit mg
		s16 Y_data;//unit mg
		s16 Z_data;//unit mg

}MEMS_data_Typedef;//传感器数据结构

typedef	struct tag_MEMS_DEVICE_STA
{
		s8	HardWareSta; // 
				
}MEMS_DEVICE_STA_Typedef;

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
BSP_ACC_EXT		u8												MEMS_INT1_FLAG;		// 
BSP_ACC_EXT		MEMS_DEVICE_STA_Typedef		MEMS_DEVICE;
BSP_ACC_EXT		MEMS_data_Typedef	 				s_Mems;	
BSP_ACC_EXT		vu16											ACC_IntCnn;	// 加速度中断计次变量	

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
void			BSP_ACC_INT1_IRQHandler 				(void);											                  //加速度INT1中断服务函数
s8				BSP_ACC_Init            				(u16 timeout);																//加速度初始化
s8				BSP_ACC_POWER_CON 							(u8	newSta,	u16 timeout);											//加速度工作控制
void			BSP_ACC_IT_IO_CON 							(u8	newSta);																	//加速度中断I/O控制
s8				BSP_ACC_IT_REG_CON 							(u8	newSta,	u16 timeout);                     //加速度中断寄存器控制
s8				BSP_ACC_ReadXYZ		      				(MEMS_data_Typedef *MEMS_data, u16 timeout);	//读取加速度值XYZ
s8				BSP_ACC_Set				      				(u8 regAddr,	u8 datas,	u16 timeout);					//加速度通用寄存器设置接口函数
s8				BSP_ACC_Get											(u8 regAddr,	u8 *datas,	u16 timeout);				//加速度通用寄存器读取接口函数
s8				BSP_ACC_SetInt1Threshold				(u16 val,	u16 timeout);												//加速度中断1门限设置接口函数
s8				BSP_ACC_GetInt1Threshold				(u16 *val,	u16 timeout);											//加速度中断1门限读取接口函数
s8				BSP_ACC_SetInt1Duration 				(u32 val,	u16 timeout);												//加速度中断1时限设置接口函数
s8				BSP_ACC_GetInt1Duration 				(u32 *val,	u16 timeout);											//加速度中断1时限读取接口函数
s8				BSP_ACC_Check				      			(u16 timeout);														    //传感器在线检测
s8				BSP_ACC_Normal				  				(u16 timeout);														    //传感器正常工作使能
s8				BSP_ACC_Lowpower								(u16 timeout);													    	//传感器低功耗使能
s8				BSP_ACC_GravityCon 							(u8 newSta,	u16 timeout);											//重力加速度叠加使能

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
