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
* Version       : V1.01
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

//////////////////////////////////////////////////////////////////////////////////
// ACC接口相关参数定义
#define	DEF_ACC_INT1_EXTI_LINE			EXTI_Line0							// ACC INT1中断线
#define	DEF_ACC_INT1_EXTI_TRIGGER		EXTI_Trigger_Falling		// ACC INT1触发方式
#define	DEF_ACC_INT1_EXTI_PORTS			GPIO_PortSourceGPIOC		// ACC INT1端口号
#define	DEF_ACC_INT1_EXTI_PINS			GPIO_PinSource0					// ACC INT1管位置

//#define	DEF_ACC_INT2_EXTI_LINE			EXTI_Line1							// ACC INT2中断线
//#define	DEF_ACC_INT2_EXTI_TRIGGER		EXTI_Trigger_Falling		// ACC INT2触发方式
//#define	DEF_ACC_INT2_EXTI_PORTS			GPIO_PortSourceGPIOB		// ACC INT2端口号
//#define	DEF_ACC_INT2_EXTI_PINS			GPIO_PinSource1					// ACC INT2管位置

//#define	DEF_ACC_CS_PORTS						GPIOC	
//#define	DEF_ACC_CS_PINS					  	BSP_GPIOC_ACC_CS   //high I2C bus,low SPI bus 

#define	DEF_ACC_PORT								DEF_IIC_1		// 确定总线通道(见bsp_h.h定义)

//////////////////////////////////////////////////////////////////////////////////
// 定义硬件故障状态
#define	DEF_MEMSHARD_DEVICE_ERR			((s8)-1)
#define	DEF_MEMSHARD_NONE_ERR				((s8)0)


//////////////////////////////////////////////////////////////////////////////////
// 定义初始化检测到的芯片类型(用于InitSta赋值)
#define	DEF_CHIP_NONE 							0					 // 无加速度芯片															
#define	DEF_CHIP_LIS3DH							1					 // LIS3DH
#define	DEF_CHIP_MPU6050						2					 // MPU6050

// 量程定义
#define	DEF_SCALE_2G								0 //((u8)0x00)
#define	DEF_SCALE_4G								1 //((u8)0x10)
#define	DEF_SCALE_8G								2	//((u8)0x20)
#define	DEF_SCALE_16G								3	//((u8)0x30)

// 工作参数设定 
#define	DEF_WORK_SCALE							DEF_SCALE_4G					// 
#define	DEF_WORK_MODE								(DEF_ODR_NOMAL_100HZ)

// 根据量程定义加速度单位
#if (DEF_WORK_SCALE == DEF_SCALE_2G)
	#define	LIS3DH_MEMS_UNIT					1/16
	#define	MPU6050_MEMS_UNIT					1000/16384
#elif( DEF_WORK_SCALE == DEF_SCALE_4G)
	#define	LIS3DH_MEMS_UNIT					1/8
	#define	MPU6050_MEMS_UNIT					1000/8192
#elif( DEF_WORK_SCALE == DEF_SCALE_8G)
	#define	LIS3DH_MEMS_UNIT					1/4
	#define	MPU6050_MEMS_UNIT					1000/4096
#elif( DEF_WORK_SCALE == DEF_SCALE_16G)
	#define	LIS3DH_MEMS_UNIT					1
	#define	MPU6050_MEMS_UNIT					1000/2048
#else
	#define	LIS3DH_MEMS_UNIT					1/16
	#define	MPU6050_MEMS_UNIT					1000/8192
#endif

//////////////////////////////////////////////////////////////////////////////////
// LIS3DH 相关定义

// 这里需要特别注意(手册里写的0x18,0x19为bit1~7的定义实际地址还要+一位bit0读为1，写为0)
// 融合后AD0为低(bit7~1:0x18)则写地址为0x30，读地址为0x31
// 融合后AD0为高(bit7~1:0x19)则写地址为0x32，读地址为0x33
#define LIS3DH_Addr                	((u8)0x30)
#define	LIS3DH_WhoAmI								((u8)0x0F)
#define	LIS3DH_WhoAmI_Vaule					((u8)0x33) // LIS3DH固定为0x33
#define	LIS3DH_Status_Reg1					((u8)0x07)
#define	LIS3DH_Ctrl_Reg1						((u8)0x20)
#define	LIS3DH_Ctrl_Reg2						((u8)0x21)
#define	LIS3DH_Ctrl_Reg3						((u8)0x22)
#define	LIS3DH_Ctrl_Reg4						((u8)0x23)
#define	LIS3DH_Ctrl_Reg5						((u8)0x24)
#define	LIS3DH_Ctrl_Reg6						((u8)0x25)
#define	LIS3DH_Status_Reg2					((u8)0x27)
#define	LIS3DH_Out_X_L							((u8)0x28)
#define	LIS3DH_Out_X_H							((u8)0x29)
#define	LIS3DH_Out_Y_L							((u8)0x2A)
#define	LIS3DH_Out_Y_H							((u8)0x2B)
#define	LIS3DH_Out_Z_L							((u8)0x2C)
#define	LIS3DH_Out_Z_H							((u8)0x2D)
#define	LIS3DH_INT1_CFG  						((u8)0x30)
#define	LIS3DH_INT1_SRC  						((u8)0x31)
#define	LIS3DH_INT1_THS  						((u8)0x32)
#define	LIS3DH_INT1_DURATION   			((u8)0x33)
#define	LIS3DH_CLICK_CFG						((u8)0x38)
#define	LIS3DH_CLICK_SRC						((u8)0x39)
#define	LIS3DH_CLICK_THS   					((u8)0x3A)
#define	LIS3DH_TIME_LIMIT						((u8)0x3B)
#define	LIS3DH_TIME_LATENCY  				((u8)0x3C)
#define	LIS3DH_TIME_WINDOW		  		((u8)0x3D)

/* 定义检测传感器状态寄存器次数 */
#define	LIS3DH_CHECK_TIME							  		(2)
/* 定义总线互斥等待时间 1s */
#define	LIS3DH_TIME_OUT   							  	(1000)

	
/* 加速度芯片工作模式定义 作用于	CTRL_REG1(20h) bit7~bit4位 */
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

//////////////////////////////////////////////////////////////////////////////////
// MPU6050 相关定义

// 这里需要特别注意(手册里写的0x68,0x69为bit1~7的定义实际地址还要+一位bit0读为1，写为0)
// 融合后AD0为低(bit7~1:0x68)则写地址为0xD0，读地址为0xD1
// 融合后AD0为高(bit7~1:0x69)则写地址为0xD2，读地址为0xD3
#define MPU6050_Addr                ((u8)0xD0) 
#define	MPU6050_WhoAmI_Vaule				((u8)0x68) // 如果AD0为低则为0x68，AD0为高则为0x69
#define MPU6050_RA_XG_OFFS_TC       ((u8)0x00) //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_YG_OFFS_TC       ((u8)0x01) //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_ZG_OFFS_TC       ((u8)0x02) //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU6050_RA_X_FINE_GAIN      ((u8)0x03) //[7:0] X_FINE_GAIN
#define MPU6050_RA_Y_FINE_GAIN      ((u8)0x04) //[7:0] Y_FINE_GAIN
#define MPU6050_RA_Z_FINE_GAIN      ((u8)0x05) //[7:0] Z_FINE_GAIN
#define MPU6050_RA_XA_OFFS_H        ((u8)0x06) //[15:0] XA_OFFS
#define MPU6050_RA_XA_OFFS_L_TC     ((u8)0x07)
#define MPU6050_RA_YA_OFFS_H        ((u8)0x08) //[15:0] YA_OFFS
#define MPU6050_RA_YA_OFFS_L_TC     ((u8)0x09)
#define MPU6050_RA_ZA_OFFS_H        ((u8)0x0A) //[15:0] ZA_OFFS
#define MPU6050_RA_ZA_OFFS_L_TC     ((u8)0x0B)
#define MPU6050_RA_XG_OFFS_USRH     ((u8)0x13) //[15:0] XG_OFFS_USR
#define MPU6050_RA_XG_OFFS_USRL     ((u8)0x14)
#define MPU6050_RA_YG_OFFS_USRH     ((u8)0x15) //[15:0] YG_OFFS_USR
#define MPU6050_RA_YG_OFFS_USRL     ((u8)0x16)
#define MPU6050_RA_ZG_OFFS_USRH     ((u8)0x17) //[15:0] ZG_OFFS_USR
#define MPU6050_RA_ZG_OFFS_USRL     ((u8)0x18)
#define MPU6050_RA_SMPLRT_DIV       ((u8)0x19)
#define MPU6050_RA_CONFIG           ((u8)0x1A)
#define MPU6050_RA_GYRO_CONFIG      ((u8)0x1B)
#define MPU6050_RA_ACCEL_CONFIG     ((u8)0x1C)
#define MPU6050_RA_FF_THR           ((u8)0x1D)
#define MPU6050_RA_FF_DUR           ((u8)0x1E)
#define MPU6050_RA_MOT_THR          ((u8)0x1F)
#define MPU6050_RA_MOT_DUR          ((u8)0x20)
#define MPU6050_RA_ZRMOT_THR        ((u8)0x21)
#define MPU6050_RA_ZRMOT_DUR        ((u8)0x22)
#define MPU6050_RA_FIFO_EN          ((u8)0x23)
#define MPU6050_RA_I2C_MST_CTRL     ((u8)0x24)
#define MPU6050_RA_I2C_SLV0_ADDR    ((u8)0x25)
#define MPU6050_RA_I2C_SLV0_REG     ((u8)0x26)
#define MPU6050_RA_I2C_SLV0_CTRL    ((u8)0x27)
#define MPU6050_RA_I2C_SLV1_ADDR    ((u8)0x28)
#define MPU6050_RA_I2C_SLV1_REG     ((u8)0x29)
#define MPU6050_RA_I2C_SLV1_CTRL    ((u8)0x2A)
#define MPU6050_RA_I2C_SLV2_ADDR    ((u8)0x2B)
#define MPU6050_RA_I2C_SLV2_REG     ((u8)0x2C)
#define MPU6050_RA_I2C_SLV2_CTRL    ((u8)0x2D)
#define MPU6050_RA_I2C_SLV3_ADDR    ((u8)0x2E)
#define MPU6050_RA_I2C_SLV3_REG     ((u8)0x2F)
#define MPU6050_RA_I2C_SLV3_CTRL    ((u8)0x30)
#define MPU6050_RA_I2C_SLV4_ADDR    ((u8)0x31)
#define MPU6050_RA_I2C_SLV4_REG     ((u8)0x32)
#define MPU6050_RA_I2C_SLV4_DO      ((u8)0x33)
#define MPU6050_RA_I2C_SLV4_CTRL    ((u8)0x34)
#define MPU6050_RA_I2C_SLV4_DI      ((u8)0x35)
#define MPU6050_RA_I2C_MST_STATUS   ((u8)0x36)
#define MPU6050_RA_INT_PIN_CFG      ((u8)0x37)
#define MPU6050_RA_INT_ENABLE       ((u8)0x38)
#define MPU6050_RA_DMP_INT_STATUS   ((u8)0x39)
#define MPU6050_RA_INT_STATUS       ((u8)0x3A)
#define MPU6050_RA_ACCEL_XOUT_H     ((u8)0x3B)
#define MPU6050_RA_ACCEL_XOUT_L     ((u8)0x3C)
#define MPU6050_RA_ACCEL_YOUT_H     ((u8)0x3D)
#define MPU6050_RA_ACCEL_YOUT_L     ((u8)0x3E)
#define MPU6050_RA_ACCEL_ZOUT_H     ((u8)0x3F)
#define MPU6050_RA_ACCEL_ZOUT_L     ((u8)0x40)
#define MPU6050_RA_TEMP_OUT_H       ((u8)0x41)
#define MPU6050_RA_TEMP_OUT_L       ((u8)0x42)
#define MPU6050_RA_GYRO_XOUT_H      ((u8)0x43)
#define MPU6050_RA_GYRO_XOUT_L      ((u8)0x44)
#define MPU6050_RA_GYRO_YOUT_H      ((u8)0x45)
#define MPU6050_RA_GYRO_YOUT_L      ((u8)0x46)
#define MPU6050_RA_GYRO_ZOUT_H      ((u8)0x47)
#define MPU6050_RA_GYRO_ZOUT_L      ((u8)0x48)
#define MPU6050_RA_EXT_SENS_DATA_00 ((u8)0x49)
#define MPU6050_RA_EXT_SENS_DATA_01 ((u8)0x4A)
#define MPU6050_RA_EXT_SENS_DATA_02 ((u8)0x4B)
#define MPU6050_RA_EXT_SENS_DATA_03 ((u8)0x4C)
#define MPU6050_RA_EXT_SENS_DATA_04 ((u8)0x4D)
#define MPU6050_RA_EXT_SENS_DATA_05 ((u8)0x4E)
#define MPU6050_RA_EXT_SENS_DATA_06 ((u8)0x4F)
#define MPU6050_RA_EXT_SENS_DATA_07 ((u8)0x50)
#define MPU6050_RA_EXT_SENS_DATA_08 ((u8)0x51)
#define MPU6050_RA_EXT_SENS_DATA_09 ((u8)0x52)
#define MPU6050_RA_EXT_SENS_DATA_10 ((u8)0x53)
#define MPU6050_RA_EXT_SENS_DATA_11 ((u8)0x54)
#define MPU6050_RA_EXT_SENS_DATA_12 ((u8)0x55)
#define MPU6050_RA_EXT_SENS_DATA_13 ((u8)0x56)
#define MPU6050_RA_EXT_SENS_DATA_14 ((u8)0x57)
#define MPU6050_RA_EXT_SENS_DATA_15 ((u8)0x58)
#define MPU6050_RA_EXT_SENS_DATA_16 ((u8)0x59)
#define MPU6050_RA_EXT_SENS_DATA_17 ((u8)0x5A)
#define MPU6050_RA_EXT_SENS_DATA_18 ((u8)0x5B)
#define MPU6050_RA_EXT_SENS_DATA_19 ((u8)0x5C)
#define MPU6050_RA_EXT_SENS_DATA_20 ((u8)0x5D)
#define MPU6050_RA_EXT_SENS_DATA_21 ((u8)0x5E)
#define MPU6050_RA_EXT_SENS_DATA_22 ((u8)0x5F)
#define MPU6050_RA_EXT_SENS_DATA_23 ((u8)0x60)
#define MPU6050_RA_MOT_DETECT_STATUS    ((u8)0x61)
#define MPU6050_RA_I2C_SLV0_DO      ((u8)0x63)
#define MPU6050_RA_I2C_SLV1_DO      ((u8)0x64)
#define MPU6050_RA_I2C_SLV2_DO      ((u8)0x65)
#define MPU6050_RA_I2C_SLV3_DO      ((u8)0x66)
#define MPU6050_RA_I2C_MST_DELAY_CTRL   ((u8)0x67)
#define MPU6050_RA_SIGNAL_PATH_RESET    ((u8)0x68)
#define MPU6050_RA_MOT_DETECT_CTRL      ((u8)0x69)
#define MPU6050_RA_USER_CTRL        ((u8)0x6A)
#define MPU6050_RA_PWR_MGMT_1       ((u8)0x6B)
#define MPU6050_RA_PWR_MGMT_2       ((u8)0x6C)
#define MPU6050_RA_BANK_SEL         ((u8)0x6D)
#define MPU6050_RA_MEM_START_ADDR   ((u8)0x6E)
#define MPU6050_RA_MEM_R_W          ((u8)0x6F)
#define MPU6050_RA_DMP_CFG_1        ((u8)0x70)
#define MPU6050_RA_DMP_CFG_2        ((u8)0x71)
#define MPU6050_RA_FIFO_COUNTH      ((u8)0x72)
#define MPU6050_RA_FIFO_COUNTL      ((u8)0x73)
#define MPU6050_RA_FIFO_R_W         ((u8)0x74)
#define MPU6050_RA_WHO_AM_I         ((u8)0x75)

#define MPU6050_TC_PWR_MODE_BIT     ((u8)7)
#define MPU6050_TC_OFFSET_BIT       ((u8)6)
#define MPU6050_TC_OFFSET_LENGTH    ((u8)6)
#define MPU6050_TC_OTP_BNK_VLD_BIT  ((u8)0)

#define MPU6050_VDDIO_LEVEL_VLOGIC          ((u8)0)
#define MPU6050_VDDIO_LEVEL_VDD             ((u8)1)

#define MPU6050_CFG_EXT_SYNC_SET_BIT        ((u8)5)
#define MPU6050_CFG_EXT_SYNC_SET_LENGTH     ((u8)3)
#define MPU6050_CFG_DLPF_CFG_BIT            ((u8)2)
#define MPU6050_CFG_DLPF_CFG_LENGTH         ((u8)3)

#define MPU6050_EXT_SYNC_DISABLED           ((u8)0x00)
#define MPU6050_EXT_SYNC_TEMP_OUT_L         ((u8)0x01)
#define MPU6050_EXT_SYNC_GYRO_XOUT_L        ((u8)0x02)
#define MPU6050_EXT_SYNC_GYRO_YOUT_L        ((u8)0x03)
#define MPU6050_EXT_SYNC_GYRO_ZOUT_L        ((u8)0x04)
#define MPU6050_EXT_SYNC_ACCEL_XOUT_L       ((u8)0x05)
#define MPU6050_EXT_SYNC_ACCEL_YOUT_L       ((u8)0x06)
#define MPU6050_EXT_SYNC_ACCEL_ZOUT_L       ((u8)0x07)

#define MPU6050_DLPF_BW_256                 ((u8)0x00)
#define MPU6050_DLPF_BW_188                 ((u8)0x01)
#define MPU6050_DLPF_BW_98                  ((u8)0x02)
#define MPU6050_DLPF_BW_42                  ((u8)0x03)
#define MPU6050_DLPF_BW_20                  ((u8)0x04)
#define MPU6050_DLPF_BW_10                  ((u8)0x05)
#define MPU6050_DLPF_BW_5                   ((u8)0x06)

#define MPU6050_GCONFIG_FS_SEL_BIT          ((u8)4)
#define MPU6050_GCONFIG_FS_SEL_LENGTH       ((u8)2)

#define MPU6050_GYRO_FS_250                 ((u8)0x00)
#define MPU6050_GYRO_FS_500                 ((u8)0x01)
#define MPU6050_GYRO_FS_1000                ((u8)0x02)
#define MPU6050_GYRO_FS_2000                ((u8)0x03)

#define MPU6050_ACONFIG_XA_ST_BIT           ((u8)7)
#define MPU6050_ACONFIG_YA_ST_BIT           ((u8)6)
#define MPU6050_ACONFIG_ZA_ST_BIT           ((u8)5)
#define MPU6050_ACONFIG_AFS_SEL_BIT         ((u8)4)
#define MPU6050_ACONFIG_AFS_SEL_LENGTH      ((u8)2)
#define MPU6050_ACONFIG_ACCEL_HPF_BIT       ((u8)2)
#define MPU6050_ACONFIG_ACCEL_HPF_LENGTH    ((u8)3)

#define MPU6050_ACCEL_FS_2                   ((u8)0x00)
#define MPU6050_ACCEL_FS_4                   ((u8)0x01)
#define MPU6050_ACCEL_FS_8                   ((u8)0x02)
#define MPU6050_ACCEL_FS_16                  ((u8)0x03)

#define MPU6050_DHPF_RESET                   ((u8)0x00)
#define MPU6050_DHPF_5                       ((u8)0x01)
#define MPU6050_DHPF_2P5                     ((u8)0x02)
#define MPU6050_DHPF_1P25                    ((u8)0x03)
#define MPU6050_DHPF_0P63                    ((u8)0x04)
#define MPU6050_DHPF_HOLD                    ((u8)0x07)

#define MPU6050_TEMP_FIFO_EN_BIT             ((u8)7)
#define MPU6050_XG_FIFO_EN_BIT               ((u8)6)
#define MPU6050_YG_FIFO_EN_BIT               ((u8)5)
#define MPU6050_ZG_FIFO_EN_BIT               ((u8)4)
#define MPU6050_ACCEL_FIFO_EN_BIT            ((u8)3)
#define MPU6050_SLV2_FIFO_EN_BIT             ((u8)2)
#define MPU6050_SLV1_FIFO_EN_BIT             ((u8)1)
#define MPU6050_SLV0_FIFO_EN_BIT             ((u8)0)

#define MPU6050_MULT_MST_EN_BIT              ((u8)7)
#define MPU6050_WAIT_FOR_ES_BIT              ((u8)6)
#define MPU6050_SLV_3_FIFO_EN_BIT            ((u8)5)
#define MPU6050_I2C_MST_P_NSR_BIT            ((u8)4)
#define MPU6050_I2C_MST_CLK_BIT              ((u8)3)
#define MPU6050_I2C_MST_CLK_LENGTH           ((u8)4)

#define MPU6050_CLOCK_DIV_348               ((u8)0x00)
#define MPU6050_CLOCK_DIV_333               ((u8)0x01)
#define MPU6050_CLOCK_DIV_320               ((u8)0x02)
#define MPU6050_CLOCK_DIV_308               ((u8)0x03)
#define MPU6050_CLOCK_DIV_296               ((u8)0x04)
#define MPU6050_CLOCK_DIV_286               ((u8)0x05)
#define MPU6050_CLOCK_DIV_276               ((u8)0x06)
#define MPU6050_CLOCK_DIV_267               ((u8)0x07)
#define MPU6050_CLOCK_DIV_258               ((u8)0x08)
#define MPU6050_CLOCK_DIV_500               ((u8)0x09)
#define MPU6050_CLOCK_DIV_471               ((u8)0x0A)
#define MPU6050_CLOCK_DIV_444               ((u8)0x0B)
#define MPU6050_CLOCK_DIV_421               ((u8)0x0C)
#define MPU6050_CLOCK_DIV_400               ((u8)0x0D)
#define MPU6050_CLOCK_DIV_381               ((u8)0x0E)
#define MPU6050_CLOCK_DIV_364               ((u8)0x0F)

#define MPU6050_I2C_SLV_RW_BIT              ((u8)7)
#define MPU6050_I2C_SLV_ADDR_BIT            ((u8)6)
#define MPU6050_I2C_SLV_ADDR_LENGTH         ((u8)7)
#define MPU6050_I2C_SLV_EN_BIT              ((u8)7)
#define MPU6050_I2C_SLV_BYTE_SW_BIT         ((u8)6)
#define MPU6050_I2C_SLV_REG_DIS_BIT         ((u8)5)
#define MPU6050_I2C_SLV_GRP_BIT             ((u8)4)
#define MPU6050_I2C_SLV_LEN_BIT             ((u8)3)
#define MPU6050_I2C_SLV_LEN_LENGTH          ((u8)4)

#define MPU6050_I2C_SLV4_RW_BIT             ((u8)7)
#define MPU6050_I2C_SLV4_ADDR_BIT           ((u8)6)
#define MPU6050_I2C_SLV4_ADDR_LENGTH        ((u8)7)
#define MPU6050_I2C_SLV4_EN_BIT             ((u8)7)
#define MPU6050_I2C_SLV4_INT_EN_BIT         ((u8)6)
#define MPU6050_I2C_SLV4_REG_DIS_BIT        ((u8)5)
#define MPU6050_I2C_SLV4_MST_DLY_BIT        ((u8)4)
#define MPU6050_I2C_SLV4_MST_DLY_LENGTH     ((u8)5)

#define MPU6050_MST_PASS_THROUGH_BIT        ((u8)7)
#define MPU6050_MST_I2C_SLV4_DONE_BIT       ((u8)6)
#define MPU6050_MST_I2C_LOST_ARB_BIT        ((u8)5)
#define MPU6050_MST_I2C_SLV4_NACK_BIT       ((u8)4)
#define MPU6050_MST_I2C_SLV3_NACK_BIT       ((u8)3)
#define MPU6050_MST_I2C_SLV2_NACK_BIT       ((u8)2)
#define MPU6050_MST_I2C_SLV1_NACK_BIT       ((u8)1)
#define MPU6050_MST_I2C_SLV0_NACK_BIT       ((u8)0)

#define MPU6050_INTCFG_INT_LEVEL_BIT        ((u8)7)
#define MPU6050_INTCFG_INT_OPEN_BIT         ((u8)6)
#define MPU6050_INTCFG_LATCH_INT_EN_BIT     ((u8)5)
#define MPU6050_INTCFG_INT_RD_CLEAR_BIT     ((u8)4)
#define MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT  ((u8)3)
#define MPU6050_INTCFG_FSYNC_INT_EN_BIT     ((u8)2)
#define MPU6050_INTCFG_I2C_BYPASS_EN_BIT    ((u8)1)
#define MPU6050_INTCFG_CLKOUT_EN_BIT        ((u8)0)

#define MPU6050_INTMODE_ACTIVEHIGH          ((u8)0x00)
#define MPU6050_INTMODE_ACTIVELOW           ((u8)0x01)

#define MPU6050_INTDRV_PUSHPULL             ((u8)0x00)
#define MPU6050_INTDRV_OPENDRAIN            ((u8)0x01)

#define MPU6050_INTLATCH_50USPULSE          ((u8)0x00)
#define MPU6050_INTLATCH_WAITCLEAR          ((u8)0x01)

#define MPU6050_INTCLEAR_STATUSREAD         ((u8)0x00)
#define MPU6050_INTCLEAR_ANYREAD            ((u8)0x01)

#define MPU6050_INTERRUPT_FF_BIT            ((u8)7)
#define MPU6050_INTERRUPT_MOT_BIT           ((u8)6)
#define MPU6050_INTERRUPT_ZMOT_BIT          ((u8)5)
#define MPU6050_INTERRUPT_FIFO_OFLOW_BIT    ((u8)4)
#define MPU6050_INTERRUPT_I2C_MST_INT_BIT   ((u8)3)
#define MPU6050_INTERRUPT_PLL_RDY_INT_BIT   ((u8)2)
#define MPU6050_INTERRUPT_DMP_INT_BIT       ((u8)1)
#define MPU6050_INTERRUPT_DATA_RDY_BIT      ((u8)0)

// TODO: Need to work on DMP related things
#define MPU6050_DMPINT_5_BIT                ((u8)5)
#define MPU6050_DMPINT_4_BIT                ((u8)4)
#define MPU6050_DMPINT_3_BIT                ((u8)3)
#define MPU6050_DMPINT_2_BIT                ((u8)2)
#define MPU6050_DMPINT_1_BIT                ((u8)1)
#define MPU6050_DMPINT_0_BIT                ((u8)0)

#define MPU6050_MOTION_MOT_XNEG_BIT         ((u8)7)
#define MPU6050_MOTION_MOT_XPOS_BIT         ((u8)6)
#define MPU6050_MOTION_MOT_YNEG_BIT         ((u8)5)
#define MPU6050_MOTION_MOT_YPOS_BIT         ((u8)4)
#define MPU6050_MOTION_MOT_ZNEG_BIT         ((u8)3)
#define MPU6050_MOTION_MOT_ZPOS_BIT         ((u8)2)
#define MPU6050_MOTION_MOT_ZRMOT_BIT        ((u8)0)

#define MPU6050_DELAYCTRL_DELAY_ES_SHADOW_BIT   ((u8)7)
#define MPU6050_DELAYCTRL_I2C_SLV4_DLY_EN_BIT   ((u8)4)
#define MPU6050_DELAYCTRL_I2C_SLV3_DLY_EN_BIT   ((u8)3)
#define MPU6050_DELAYCTRL_I2C_SLV2_DLY_EN_BIT   ((u8)2)
#define MPU6050_DELAYCTRL_I2C_SLV1_DLY_EN_BIT   ((u8)1)
#define MPU6050_DELAYCTRL_I2C_SLV0_DLY_EN_BIT   ((u8)0)

#define MPU6050_PATHRESET_GYRO_RESET_BIT        ((u8)2)
#define MPU6050_PATHRESET_ACCEL_RESET_BIT       ((u8)1)
#define MPU6050_PATHRESET_TEMP_RESET_BIT        ((u8)0)

#define MPU6050_DETECT_ACCEL_ON_DELAY_BIT       ((u8)5)
#define MPU6050_DETECT_ACCEL_ON_DELAY_LENGTH    ((u8)2)
#define MPU6050_DETECT_FF_COUNT_BIT             ((u8)3)
#define MPU6050_DETECT_FF_COUNT_LENGTH          ((u8)2)
#define MPU6050_DETECT_MOT_COUNT_BIT            ((u8)1)
#define MPU6050_DETECT_MOT_COUNT_LENGTH         ((u8)2)

#define MPU6050_DETECT_DECREMENT_RESET          ((u8)0x00)
#define MPU6050_DETECT_DECREMENT_1              ((u8)0x01)
#define MPU6050_DETECT_DECREMENT_2              ((u8)0x02)
#define MPU6050_DETECT_DECREMENT_4              ((u8)0x03)

#define MPU6050_USERCTRL_DMP_EN_BIT             ((u8)7)
#define MPU6050_USERCTRL_FIFO_EN_BIT            ((u8)6)
#define MPU6050_USERCTRL_I2C_MST_EN_BIT         ((u8)5)
#define MPU6050_USERCTRL_I2C_IF_DIS_BIT         ((u8)4)
#define MPU6050_USERCTRL_DMP_RESET_BIT          ((u8)3)
#define MPU6050_USERCTRL_FIFO_RESET_BIT         ((u8)2)
#define MPU6050_USERCTRL_I2C_MST_RESET_BIT      ((u8)1)
#define MPU6050_USERCTRL_SIG_COND_RESET_BIT     ((u8)0)

#define MPU6050_PWR1_DEVICE_RESET_BIT           ((u8)7)
#define MPU6050_PWR1_SLEEP_BIT                  ((u8)6)
#define MPU6050_PWR1_CYCLE_BIT                  ((u8)5)
#define MPU6050_PWR1_TEMP_DIS_BIT               ((u8)3)
#define MPU6050_PWR1_CLKSEL_BIT                 ((u8)2)
#define MPU6050_PWR1_CLKSEL_LENGTH              ((u8)3)
										 
#define MPU6050_CLOCK_INTERNAL                  ((u8)0x00)
#define MPU6050_CLOCK_PLL_XGYRO                 ((u8)0x01)
#define MPU6050_CLOCK_PLL_YGYRO                 ((u8)0x02)
#define MPU6050_CLOCK_PLL_ZGYRO                 ((u8)0x03)
#define MPU6050_CLOCK_PLL_EXT32K                ((u8)0x04)
#define MPU6050_CLOCK_PLL_EXT19M                ((u8)0x05)
#define MPU6050_CLOCK_KEEP_RESET                ((u8)0x07)

#define MPU6050_PWR2_LP_WAKE_CTRL_BIT           ((u8)7)
#define MPU6050_PWR2_LP_WAKE_CTRL_LENGTH        ((u8)2)
#define MPU6050_PWR2_STBY_XA_BIT                ((u8)5)
#define MPU6050_PWR2_STBY_YA_BIT                ((u8)4)
#define MPU6050_PWR2_STBY_ZA_BIT                ((u8)3)
#define MPU6050_PWR2_STBY_XG_BIT                ((u8)2)
#define MPU6050_PWR2_STBY_YG_BIT                ((u8)1)
#define MPU6050_PWR2_STBY_ZG_BIT                ((u8)0)

#define MPU6050_WAKE_FREQ_1P25                  ((u8)0x00)
#define MPU6050_WAKE_FREQ_2P5                   ((u8)0x01)
#define MPU6050_WAKE_FREQ_5                     ((u8)0x02)
#define MPU6050_WAKE_FREQ_10                    ((u8)0x03)

#define MPU6050_BANKSEL_PRFTCH_EN_BIT           ((u8)6)
#define MPU6050_BANKSEL_CFG_USER_BANK_BIT       ((u8)5)
#define MPU6050_BANKSEL_MEM_SEL_BIT             ((u8)4)
#define MPU6050_BANKSEL_MEM_SEL_LENGTH          ((u8)5)

#define MPU6050_WHO_AM_I_BIT                    ((u8)6)
#define MPU6050_WHO_AM_I_LENGTH                 ((u8)6)

#define MPU6050_DMP_MEMORY_BANKS                8
#define MPU6050_DMP_MEMORY_BANK_SIZE            256
#define MPU6050_DMP_MEMORY_CHUNK_SIZE           16


#define MPU_60X0_RESET_REG_VALU            	((u8)0x80)  
#define MPU_60X0_PWR_MGMT_1_REG_VALU        ((u8)0x08)    // Disable temperature sensor, Internal 8MHz oscillator 
#define MPU_60X0_USER_CTRL_REG_VALU         ((u8)0x47)    // Enable FIFO. Reset FIFO and signal paths for all sensors 
#define MPU_60X0_SMPLRT_DIV_REG_VALU        ((u8)0x07)    //
#define MPU_60X0_CONFIG_REG_VALU            ((u8)0x06)    //   

#define MPU_60X0_GYRO_CONFIG_REG_VALU       ((u8)0x18)    // 陀螺仪配置为全量程范围± 2000 °/s
#define MPU_60X0_ACCEL_CONFIG_REG_VALU      ((u8)0x09)    // Accelerometer works at 4g range. If selftest is needed, REMEMBER to put this to 8g range  
#define MPU_60X0_FIFO_EN_REG_VALU           ((u8)0x78)    // Only enable accel and gyro  

#define BIT_I2C_MST_VDDIO   				((u8)0x80)
#define BIT_FIFO_EN         				((u8)0x40)
#define BIT_DMP_EN          				((u8)0x80)
#define BIT_FIFO_RST        				((u8)0x04)
#define BIT_DMP_RST         				((u8)0x08)
#define BIT_FIFO_OVERFLOW   				((u8)0x10)
#define BIT_DATA_RDY_EN     				((u8)0x01)
#define BIT_DMP_INT_EN      				((u8)0x02)
#define BIT_MOT_INT_EN      				((u8)0x40)
#define BITS_FSR            				((u8)0x18)
#define BITS_LPF            				((u8)0x07)
#define BITS_HPF            				((u8)0x07)
#define BITS_CLK            				((u8)0x07)
#define BIT_FIFO_SIZE_1024  				((u8)0x40)
#define BIT_FIFO_SIZE_2048  				((u8)0x80)
#define BIT_FIFO_SIZE_4096  				((u8)0xC0)
#define BIT_RESET           				((u8)0x80)
#define BIT_SLEEP           				((u8)0x40)
#define BIT_S0_DELAY_EN     				((u8)0x01)
#define BIT_S2_DELAY_EN     				((u8)0x04)
#define BITS_SLAVE_LENGTH   				((u8)0x0F)
#define BIT_SLAVE_BYTE_SW   				((u8)0x40)
#define BIT_SLAVE_GROUP     				((u8)0x10)
#define BIT_SLAVE_EN        				((u8)0x80)
#define BIT_I2C_READ        				((u8)0x80)
#define BITS_I2C_MASTER_DLY 				((u8)0x1F)
#define BIT_AUX_IF_EN       				((u8)0x20)
#define BIT_ACTL            				((u8)0x80)
#define BIT_LATCH_EN        				((u8)0x20)
#define BIT_ANY_RD_CLR      				((u8)0x10)
#define BIT_BYPASS_EN       				((u8)0x02)
#define BITS_WOM_EN         				((u8)0xC0)
#define BIT_LPA_CYCLE       				((u8)0x20)
#define BIT_STBY_XA         				((u8)0x20)
#define BIT_STBY_YA         				((u8)0x10)
#define BIT_STBY_ZA         				((u8)0x08)
#define BIT_STBY_XG        					((u8)0x04)
#define BIT_STBY_YG         				((u8)0x02)
#define BIT_STBY_ZG         				((u8)0x01)
#define BIT_STBY_XYZA       				(BIT_STBY_XA | BIT_STBY_YA | BIT_STBY_ZA)
#define BIT_STBY_XYZG       				(BIT_STBY_XG | BIT_STBY_YG | BIT_STBY_ZG) 


//MPU6050_RA_SMPLRT_DIV
//传感器寄存器输出，FITO输出，DMP采样，Motion检测，ZeroMotion检测和Free Fall
//检测都基于这个采样频率。
//采样频率=陀螺仪输出频率/（1+SMPLRT_DIV）
//当DLPF is disabled（DLPF_CFG=0 or 7），陀螺输出频率=8kHz；当DLPF is enabled
//（see寄存器26），陀螺仪输出频率=1KHz。


// MPU6050_RA_GYRO_CONFIG	
// FS_SEL选择陀螺仪的满量程
//   D4   D3
//   FS1	FS0

//FS_SEL Full    Scale Range
//		0		     ± 250  °/s
//		1	         ± 500  °/s
//		2	         ± 1000 °/s
//		3	         ± 2000 °/s 

//MPU6050_RA_ACCEL_CONFIG    加速度计配置
//FS_SEL选择陀螺仪的满量程
//D4   D3
//FS1  FS0

//AFS_SEL      Full Scale Range
//	0	           ± 2g
//	1              ± 4g
//	2              ± 8g
//	3              ± 16g

/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/
typedef struct
{
		s16 X_Acc;//unit mg
		s16 Y_Acc;//unit mg
		s16 Z_Acc;//unit mg
		s16 X_Gyro;//unit mg
		s16 Y_Gyro;//unit mg
		s16 Z_Gyro;//unit mg
	
}MEMS_data_Typedef;//传感器数据结构

typedef	struct tag_MEMS_DEVICE_STA
{
		s8	InitSta;				// 初始化状态(DEF_CHIP_NONE:表示为检测到芯片，DEF_CHIP_LIS3DH:表示当前为LIS3DH，DEF_CHIP_MPU6050:表示当前为MPU6050
		s8	HardWareSta; 		// 硬件状态
				
}MEMS_DEVICE_STA_Typedef;

typedef	struct tag_ACC_Int
{
		u8		IntFlag;			// 加速度中断标志
		u8		IntFlag1;			// 加速度中断标志1
		u8		IntFlag2;			// 加速度中断标志2
		vu16	IntCnn;				// 加速度中断次数累加
				
}ACC_Int_Typedef;

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
BSP_ACC_EXT		MEMS_DEVICE_STA_Typedef		MEMS_DEVICE;
BSP_ACC_EXT		MEMS_data_Typedef	 				s_Mems;	
BSP_ACC_EXT		ACC_Int_Typedef						s_ACC;					// 加速度芯片中断结构变量

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
void			BSP_ACC_INT1_IRQHandler 				(void);			
void			BSP_ACC_IT_IO_CON 							(u8	newSta);								//加速度INT1中断服务函数
s8 				BSP_ACC_Check 									(void);
s8				BSP_ACC_Init            				(void);											//加速度初始化
u8 				BSP_ACC_ReadRegister 						(u8 RomAdd);
s8 				BSP_ACC_WriteRegister 					(u8 RomAdd,	u8 Data);
s8 				BSP_ACC_CfgInt 									(u8 intEn,	u8 threshold,	u8 duration,	u32	timeout);
s8 				BSP_ACC_ReadXYZ								 	(MEMS_data_Typedef *sData,	u32	timeout);
s8 				BSP_ACC_ReadGyro 								(MEMS_data_Typedef *sData,	u32	timeout);


/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
