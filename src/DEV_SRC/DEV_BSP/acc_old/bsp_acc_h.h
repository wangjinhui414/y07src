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
// ������Ϣ�������
//#define	DEF_ACCINFO_OUTPUTEN				(1)											// ACC��Ϣ���ʹ��


/* ACC�ӿ���ز������� */
#define	DEF_ACC_INT1_EXTI_LINE			EXTI_Line0							// ACC INT1�ж���
#define	DEF_ACC_INT1_EXTI_TRIGGER		EXTI_Trigger_Falling		// ACC INT1������ʽ
#define	DEF_ACC_INT1_EXTI_PORTS			GPIO_PortSourceGPIOC		// ACC INT1�˿ں�
#define	DEF_ACC_INT1_EXTI_PINS			GPIO_PinSource0					// ACC INT1��λ��

//#define	DEF_ACC_INT2_EXTI_LINE			EXTI_Line1							// ACC INT2�ж���
//#define	DEF_ACC_INT2_EXTI_TRIGGER		EXTI_Trigger_Falling		// ACC INT2������ʽ
//#define	DEF_ACC_INT2_EXTI_PORTS			GPIO_PortSourceGPIOB		// ACC INT2�˿ں�
//#define	DEF_ACC_INT2_EXTI_PINS			GPIO_PinSource1					// ACC INT2��λ��

/*************** IO���� ******************/
//#define	DEF_ACC_CS_PORTS						GPIOC	
//#define	DEF_ACC_CS_PINS					  	BSP_GPIOC_ACC_CS   //high I2C bus,low SPI bus 

// ͨ�����߶���
#define	DEF_ACC_BUSTYPE							DEF_IIC_BUS							// ȷ��ͨ����������(��bsp_h.h����)
#define DEF_ACC_PORT      					DEF_IIC_1								// ȷ��IIC����ͨ��(��bsp_h.h����)

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

/* �����⴫����״̬�Ĵ������� */
#define	CHECK_TIME							  	(2)
/* �������߻���ȴ�ʱ�� 1s */
#define	TIME_OUT   							  	(1000)
/* ���崫����ԭʼ���ݵ�λ 4mg / -2g~+2g */
#define	MEMS_UNIT								  	(1)

/* ����Ӳ������״̬ */	
#define	DEF_MEMSHARD_DEVICE_ERR			((s8)-1)
#define	DEF_MEMSHARD_NONE_ERR				((s8)0)

/* ���������Ϣʹ�� */
#define	INFOPRINT_EN						  	(1)


/************************  ADD������������******************************************************/

/* ADD ����ģʽ���� ������	CTRL_REG1(20h) bit7~bit4λ */
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

/* ADD ���̶��� ������	CTRL_REG4(23h) bit5~bit4λ */
#define	DEF_FS_SCALE_2G							((u8 )0x00)
#define	DEF_FS_SCALE_4G							((u8 )0x10)
#define	DEF_FS_SCALE_8G							((u8 )0x20)
#define	DEF_FS_SCALE_16G						((u8 )0x30)

/* ADD ���������趨*/
#define	DEF_WORK_MODE								(DEF_ODR_NOMAL_100HZ)
#define	DEF_WORK_SCALE							(DEF_FS_SCALE_2G)
#define	DEF_WORK_GRAVITY_EN					(0)											// ��ֹ�󽫹ر��������ٶȵĵ��Ӹ���(ʹ�ܺ���������Z����+1000mg�ĵ���) 

/************************  BSP_ACCELEROMETER ������������	***************************************/


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

}MEMS_data_Typedef;//���������ݽṹ

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
BSP_ACC_EXT		vu16											ACC_IntCnn;	// ���ٶ��жϼƴα���	

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
void			BSP_ACC_INT1_IRQHandler 				(void);											                  //���ٶ�INT1�жϷ�����
s8				BSP_ACC_Init            				(u16 timeout);																//���ٶȳ�ʼ��
s8				BSP_ACC_POWER_CON 							(u8	newSta,	u16 timeout);											//���ٶȹ�������
void			BSP_ACC_IT_IO_CON 							(u8	newSta);																	//���ٶ��ж�I/O����
s8				BSP_ACC_IT_REG_CON 							(u8	newSta,	u16 timeout);                     //���ٶ��жϼĴ�������
s8				BSP_ACC_ReadXYZ		      				(MEMS_data_Typedef *MEMS_data, u16 timeout);	//��ȡ���ٶ�ֵXYZ
s8				BSP_ACC_Set				      				(u8 regAddr,	u8 datas,	u16 timeout);					//���ٶ�ͨ�üĴ������ýӿں���
s8				BSP_ACC_Get											(u8 regAddr,	u8 *datas,	u16 timeout);				//���ٶ�ͨ�üĴ�����ȡ�ӿں���
s8				BSP_ACC_SetInt1Threshold				(u16 val,	u16 timeout);												//���ٶ��ж�1�������ýӿں���
s8				BSP_ACC_GetInt1Threshold				(u16 *val,	u16 timeout);											//���ٶ��ж�1���޶�ȡ�ӿں���
s8				BSP_ACC_SetInt1Duration 				(u32 val,	u16 timeout);												//���ٶ��ж�1ʱ�����ýӿں���
s8				BSP_ACC_GetInt1Duration 				(u32 *val,	u16 timeout);											//���ٶ��ж�1ʱ�޶�ȡ�ӿں���
s8				BSP_ACC_Check				      			(u16 timeout);														    //���������߼��
s8				BSP_ACC_Normal				  				(u16 timeout);														    //��������������ʹ��
s8				BSP_ACC_Lowpower								(u16 timeout);													    	//�������͹���ʹ��
s8				BSP_ACC_GravityCon 							(u8 newSta,	u16 timeout);											//�������ٶȵ���ʹ��

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
