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
*                                      BOARD SUPPORT PACKAGE
*
*                                     	  bsp_acc_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : bsp_acc_c.c
* Version       : V1.01
* Programmer(s) : Felix
*********************************************************************************************************
*/

// 驱动说明：
// IIC总线通信，I/O模拟方式
// 驱动程序自动识别MPU6050或LIS3DH。
// LIS3DH,MPU6050 IIC模式,快速模式最快400Khz(MPU6050只支持IIC不支持SPI)

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   BSP_ACC_GLOBLAS
#include	<string.h>
#include	<stdio.h>
#include	"cfg_h.h"
#include	"bsp_h.h"
#include	"bsp_acc_h.h"
#include 	"ucos_ii.h"
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
enum ENUM_TWI_REPLY	{TWI_NACK=0,TWI_ACK=1};

/********** IIC模拟方式端口定义 *************/

#define I2C_Speed     100000

#if(DEF_ACC_BUSTYPE == 0) 
#if(DEF_ACC_PORT == DEF_IIC_1)
#define IIC_SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define IIC_SCL_L         GPIOB->BRR  = GPIO_Pin_6 
#define IIC_SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define IIC_SDA_L         GPIOB->BRR  = GPIO_Pin_7
#define IIC_SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define IIC_SDA_read      GPIOB->IDR  & GPIO_Pin_7
#else // DEF_IIC_2
#define IIC_SCL_H         GPIOB->BSRR = GPIO_Pin_10
#define IIC_SCL_L         GPIOB->BRR  = GPIO_Pin_10 
#define IIC_SDA_H         GPIOB->BSRR = GPIO_Pin_11
#define IIC_SDA_L         GPIOB->BRR  = GPIO_Pin_11
#define IIC_SCL_read      GPIOB->IDR  & GPIO_Pin_10
#define IIC_SDA_read      GPIOB->IDR  & GPIO_Pin_11
#endif
#define	IIC_NOP						I2C_delay()
#endif

/********** 内部 ACC I/O 控制接口 *************/

/* Select SPI ACCELEROMETER: CS pin low  */
//#define SPI_ACC_CS_EN()   (GPIO_ResetBits	(DEF_ACC_CS_PORTS, DEF_ACC_CS_PINS))
//#define SPI_ACC_CS_DIS()  (GPIO_SetBits		(DEF_ACC_CS_PORTS, DEF_ACC_CS_PINS))



/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

extern    OS_EVENT  				*ACCMutexSem;     	// 	用于ACC总线底层应用之间的互斥
extern		OS_EVENT					*CrashMEMSSem;			//  碰撞信号量
static		vu8								ACCMutexFlag=1;			//	无OS时实现SPI总线的互斥


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

// I2C 底层函数
static 		void 			I2C_delay 								(void);
static 		bool 			I2C_Start 								(void);
static 		void 			I2C_Stop 									(void);
static 		void 			I2C_Ack 									(void);
static 		void 			I2C_NoAck 								(void);
//static 		bool 			I2C_WaitAck 							(void);

static 		u8	 			I2C_SendByte 							(u8 SendByte);
static 		u8 				I2C_ReceiveByte 					(void);

static		void 			I2C_SingleWrite 					(u8 SlaveAdd,	u8 RomAdd,	u8 Data);
static		u8 				I2C_SingleRead 						(u8 SlaveAdd,	u8 RomAdd);

// 加速度芯片底层应用函数
static 		void			AccDelayMS								(u16	nms);
/*
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                      IIC LOWER FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : I2C_delay
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static void I2C_delay (void)
{        
   u8 i=150; 	//这里可以优化速度 ,经测试最低到5还能写入
   while(i) 
   { 
     	i--; 
   } 
}
/*
*********************************************************************************************************
* Function Name  : I2C_Start
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static bool I2C_Start (void)
{
 		IIC_SDA_H;
    IIC_SCL_H;
    IIC_NOP;
    if(!IIC_SDA_read)return FALSE;        //SDA线为低电平则总线忙,退出
    IIC_SDA_L;
    IIC_NOP;
    if(IIC_SDA_read) return FALSE;        //SDA线为高电平则总线出错,退出
    return TRUE;		
}
/*
*********************************************************************************************************
* Function Name  : I2C_Stop
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static void I2C_Stop (void)
{
    IIC_SCL_L;
    IIC_NOP;
    IIC_SDA_L;
    IIC_NOP;
    IIC_SCL_H;
    IIC_NOP;
    IIC_SDA_H;
    IIC_NOP;
}
/*
*********************************************************************************************************
* Function Name  : I2C_Ack
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static void I2C_Ack (void)
{        
    IIC_SCL_L;
    IIC_NOP;
    IIC_SDA_L;
    IIC_NOP;
    IIC_SCL_H;
    IIC_NOP;
    IIC_SCL_L;
    IIC_NOP;
}
/*
*********************************************************************************************************
* Function Name  : I2C_NoAck
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static void I2C_NoAck (void)
{        
    IIC_SCL_L;
    IIC_NOP;
    IIC_SDA_H;
    IIC_NOP;
    IIC_SCL_H;
    IIC_NOP;
    IIC_SCL_L;
    IIC_NOP;
}
/*
*********************************************************************************************************
* Function Name  : I2C_WaitAck
* Description    : 返回为:=1有ACK,=0无ACK     
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
//static bool I2C_WaitAck (void)
//{
//    IIC_SCL_L;
//    IIC_NOP;
//    IIC_SDA_H;                        
//    IIC_NOP;
//    IIC_SCL_H;
//    IIC_NOP;
//    if(IIC_SDA_read)
//    {
//	      IIC_SCL_L;
//	      return FALSE;
//    }
//    IIC_SCL_L;
//    return TRUE;
//}
/*
*********************************************************************************************************
* Function Name  : I2C_SendByte
* Description    : 发送一个字节  
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static u8 I2C_SendByte (u8 SendByte)
{
		u8 i=0,	tmpData=0;
		tmpData =SendByte;
	
		IIC_SCL_L;
		for(i=0;i<8;i++)
		{  
				//---------数据建立----------
				if(tmpData&0x80)
				{
						IIC_SDA_H;
				}
				else
				{
						IIC_SDA_L;
				} 											   
				tmpData<<=1;
				IIC_NOP;
				//---数据建立保持一定延时----

				//----产生一个上升沿[正脉冲] 
				IIC_SCL_H;
				IIC_NOP;
				IIC_SCL_L;
				IIC_NOP;//延时,防止SCL还没变成低时改变SDA,从而产生START/STOP信号 
		}
		//接收从机的应答
		IIC_SCL_L;
		IIC_NOP;
		IIC_SDA_H;
		IIC_NOP;
		IIC_SCL_H;
		IIC_NOP;
		if(IIC_SDA_read)
		{
				IIC_SCL_L;
				IIC_NOP;
				//////DebugPrint("TWI_NACK!\n");
				return TWI_NACK;
		}
		else
		{
				IIC_SCL_L;
				IIC_NOP;
				//////DebugPrint("TWI_ACK!\n");
				return TWI_ACK;  
		} 
}
/*
*********************************************************************************************************
* Function Name  : I2C_ReceiveByte
* Description    : 接收一个字节
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static u8 I2C_ReceiveByte (void)
{ 
		u8 i=0,recData=0;
	
		IIC_SDA_H;
		IIC_SCL_L; 
		for(i=0;i<8;i++)
		{
				IIC_SCL_H;//产生时钟上升沿[正脉冲],让从机准备好数据 
				IIC_NOP;
				recData<<=1;
				if(IIC_SDA_read) //读引脚状态
				{
						recData|=0x01; 
				}   
				IIC_SCL_L;//准备好再次接收数据  
				IIC_NOP;//等待数据准备好         
		}
		///DebugPrint("TWI_Dat:%x\n",Dat);
		return recData;    
}
/*
*********************************************************************************************************
* Function Name  : I2C_SingleWrite
* Description    : 写操作 
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	void I2C_SingleWrite (u8 SlaveAdd,	u8 RomAdd,	u8 Data)
{
		u8 i=0,retval=0;

		for(i=0;i<15;i++)
		{      
				I2C_Start();
				retval=I2C_SendByte(SlaveAdd);
				if(retval == TWI_ACK)
						break;
				else 
				{
						I2C_delay();
						continue;
				}
		}
		I2C_SendByte(RomAdd);   //----------------------
		I2C_SendByte(Data);	
		I2C_Stop();  
}
/*
*********************************************************************************************************
* Function Name  : I2C_SingleRead
* Description    : 读操作  
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	u8 I2C_SingleRead (u8 SlaveAdd,	u8 RomAdd)
{
		u8 i=0,retval=0;  
		for(i=0;i<15;i++)
		{
				I2C_Start();
				retval=I2C_SendByte(SlaveAdd);
				if(retval==TWI_ACK)
						break;
				else 
				{
						I2C_delay();
						continue;
				}
		}							   
		I2C_SendByte((INT8U)(RomAdd)); //-------------------------
		I2C_Start();
		I2C_SendByte(SlaveAdd+1);
		retval=I2C_ReceiveByte();
		I2C_NoAck();
		I2C_Stop();
		return retval;
}	

/*
*********************************************************************************************************
*                                      APP(Public) FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : AccDelayMS
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static void	AccDelayMS	(u16	nms)
{
		if(OSRunning > 0)
				OSTimeDly(nms);
		else
				SoftDelayMS(nms);
}
/*
******************************************************************************
* Function Name  : BSP_ACC_INT1_IRQHandlers()
* Description    : 加速度INT1中断服务函数
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void BSP_ACC_INT1_IRQHandler (void)
{
		s_ACC.IntFlag=1;
		s_ACC.IntFlag1=1;
		s_ACC.IntFlag2=1;
		if(s_ACC.IntCnn < 0xffff)
				s_ACC.IntCnn++;
}
/*
******************************************************************************
* Function Name  : BSP_ACC_IT_IO_CON()
* Description    : 加速度INT1中断控制
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	BSP_ACC_IT_IO_CON (u8	newSta)
{
		EXTI_InitTypeDef EXTI_InitStructure;	
				
		EXTI_ClearITPendingBit(DEF_ACC_INT1_EXTI_LINE);			
		// ADDINT1_EXIT 		
		EXTI_InitStructure.EXTI_Line 		= DEF_ACC_INT1_EXTI_LINE;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = DEF_ACC_INT1_EXTI_TRIGGER;
	  EXTI_InitStructure.EXTI_LineCmd = (FunctionalState)newSta;
	  EXTI_Init(&EXTI_InitStructure);	
}

/*
********************************************************************************************************
*                                       BSP_ACC_ReadRegister
*
* Description : 读取寄存器读取
*
* Argument(s) : 
*
* Return(s)   : 
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
u8 BSP_ACC_ReadRegister (u8 RomAdd)
{
		if(MEMS_DEVICE.InitSta == DEF_CHIP_LIS3DH)
				return	I2C_SingleRead(LIS3DH_Addr,RomAdd);
		else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
				return	I2C_SingleRead(MPU6050_Addr,RomAdd);
		else
				return  0;
}
/*
********************************************************************************************************
*                                       BSP_ACC_WriteRegister
*
* Description : 写寄存器
*
* Argument(s) : 
*
* Return(s)   : 
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8 BSP_ACC_WriteRegister (u8 RomAdd,	u8 Data)
{
		if(MEMS_DEVICE.InitSta == DEF_CHIP_LIS3DH)
				I2C_SingleWrite(LIS3DH_Addr,RomAdd,Data);
		else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
				I2C_SingleWrite(MPU6050_Addr,RomAdd,Data);
		return 0;	
}
/*
*********************************************************************************************************
*                                      APP FUNCTION PROTOTYPES
*********************************************************************************************************
*/
/*
********************************************************************************************************
*                                       BSP_ACC_CfgInt
*
* Description : 配置加速度中断寄存器(中断门限及时长)
*
* Argument(s) : intEn: 使能状态DEF_ENABLE,DEF_DISABLE(寄存器配置完毕后中断的使能状态), threshold:单位,mg	duration:单位ms
*
* Return(s)   : 
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8 BSP_ACC_CfgInt (u8 intEn,	u8 threshold,	u8 duration,	u32	timeout)
{   
		u8 err1=OS_NO_ERR,tmp=0,val=0;   
		s8 retCode=0;
	
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求信号量
		else
				if(ACCMutexFlag	== 1)
						ACCMutexFlag	=	0;
				else
						return -1;
		
		if(err1 != OS_NO_ERR)
		{
				if(err1	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}
		else
		{		
				if(MEMS_DEVICE.InitSta == DEF_CHIP_LIS3DH)
				{			
						// 中断门限配置
						if((threshold > 127) || (duration > 127))	// LIS3DH为有效位为7bit
								retCode =-1;
						else
						{
								// 配置前关中断
								I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg3,0x00);// 禁止INT1加速度外部中断(L1_AOL1)
								/*
								// LIS3DH该处算法存在问题暂时取消
								if(DEF_WORK_SCALE == DEF_SCALE_2G)		
										tmp=((u8)(threshold/16 +0.5))&0x7F;
								else if(DEF_WORK_SCALE == DEF_SCALE_4G)		
										tmp=((u8)(threshold/31 +0.5))&0x7F;
								else if(DEF_WORK_SCALE == DEF_SCALE_8G)		
										tmp=((u8)(threshold/63 +0.5))&0x7F;
								else if(DEF_WORK_SCALE == DEF_SCALE_16G)		
										tmp=((u8)(threshold/125 +0.5))&0x7F;
								else
										tmp=((u8)(threshold/31 +0.5))&0x7F;
								*/
								tmp =threshold;
								I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_THS,tmp);// 配置INT1中断门限
								/*
								// LIS3DH该处算法存在问题暂时取消
								if(DEF_WORK_MODE == DEF_ODR_NOMAL_1HZ)
										tmp = (u8)(duration / 1000 + 0.5) & 0x7F;
								else if(DEF_WORK_MODE == DEF_ODR_NOMAL_10HZ)
										tmp = (u8)(duration / 100 + 0.5) & 0x7F;
								else if(DEF_WORK_MODE == DEF_ODR_NOMAL_25HZ)
										tmp = (u8)(duration / 40 + 0.5) & 0x7F;
								else if(DEF_WORK_MODE == DEF_ODR_NOMAL_50HZ)
										tmp = (u8)(duration / 20 + 0.5) & 0x7F;
								else if(DEF_WORK_MODE == DEF_ODR_NOMAL_100HZ)
										tmp = (u8)(duration / 10 + 0.5) & 0x7F;
								else if(DEF_WORK_MODE == DEF_ODR_NOMAL_200HZ)
										tmp = (u8)(duration / 5 + 0.5) & 0x7F;
								else if(DEF_WORK_MODE == DEF_ODR_NOMAL_400HZ)
										tmp = (u8)(duration / 2.5 + 0.5) & 0x7F;
								else
										tmp = (u8)(duration / 10 + 0.5) & 0x7F;
								*/
								tmp =duration;
								I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_DURATION,tmp);	// 配置中断时长ms
								if(intEn == DEF_ENABLE)
										I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg3,0x40);// 使能INT1加速度外部中断(L1_AOL1)								
								retCode =0;
						}
				}
				else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
				{
						I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_INT_ENABLE , 0x00);  		// 配置前禁止中断使能寄存器,移动中断使能
						I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_MOT_THR , threshold);  	// 中断门限1kHZ 1个数值为1mg
						I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_MOT_DUR , duration);  	// 中断时长1kHZ 1个数值为1ms
						if(intEn == DEF_ENABLE)
								I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_INT_ENABLE , 0x40);  //MPU6050_RA_INT_ENABLE  //中断使能寄存器,移动中断使能
						retCode =0;
				}
		}
		
		memset((u8 *)&s_ACC,	0,	sizeof(s_ACC));	// 清中断结构变量
		
		if(OSRunning > 0)
				OSSemPost(ACCMutexSem);	
		else
				ACCMutexFlag	=	1;

		return retCode;  										
}

/*
********************************************************************************************************
*                                       BSP_ACC_ReadXYZ
*
* Description : 读取3轴加速度信息
*
* Argument(s) : 
*
* Return(s)   : 0,init accelerometer OK, <0 ,init accelerometer error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8 BSP_ACC_ReadXYZ (MEMS_data_Typedef *sData,	u32	timeout)
{   
		u8 i=0,err1=OS_NO_ERR,tmp=0,tmp1=0,tmp2=0;	    
		s8 retCode=0;
		s16	tmpX=0,tmpY=0,tmpZ=0;
	
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求信号量
		else
				if(ACCMutexFlag	== 1)
						ACCMutexFlag	=	0;
				else
						return -1;
		
		if(err1 != OS_NO_ERR)
		{
				if(err1	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}
		else
		{
				if(MEMS_DEVICE.InitSta == DEF_CHIP_LIS3DH)
				{
						// check if data is OK
						for(i=1;	i<=2;	i++)
						{
								/* read the data status to confirm the data is ok to read*/
								tmp =I2C_SingleRead(LIS3DH_Addr,LIS3DH_Status_Reg2);
								if(0x08 & tmp)//data is ok
								{
										break;
								}
								AccDelayMS(20);	// 避免频繁处理  
						}
						if(i > 2)//read status timeout ,return err
						{
								retCode	=	-1;		
						}
						else
						{
								// 读取X轴数值///////////////////////////////////////////
								tmp1=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_X_L); 
								tmp2=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_X_H); 
								tmpX =(s16)(tmp2<<8)|tmp1;

								sData->X_Acc =tmpX*LIS3DH_MEMS_UNIT;
							
								// 读取Y轴数值///////////////////////////////////////////
								tmp1=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_Y_L);
								tmp2=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_Y_H);
								tmpY=	(tmp2<<8)|tmp1;			 

								sData->Y_Acc =tmpY*LIS3DH_MEMS_UNIT;
								
								// 读取Z轴数值///////////////////////////////////////////
								tmp1=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_Z_L);
								tmp2=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_Z_H);
								tmpZ=	(tmp2<<8)|tmp1;

								sData->Z_Acc =tmpZ*LIS3DH_MEMS_UNIT;
								
								retCode =0;
						}
				}
				else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
				{
						// 读取X轴数值///////////////////////////////////////////
						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_XOUT_L); 
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_XOUT_H); 
						tmpX =(tmp2<<8)|tmp1;

						
						sData->X_Acc =tmpX*MPU6050_MEMS_UNIT;
						
						// 读取Y轴数值///////////////////////////////////////////
						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_YOUT_L);
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_YOUT_H);
						tmpY=	(tmp2<<8)|tmp1;			 

						sData->Y_Acc =tmpY*MPU6050_MEMS_UNIT;
						
						// 读取Z轴数值///////////////////////////////////////////
						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_ZOUT_L);
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_ZOUT_H);
						tmpZ=	(tmp2<<8)|tmp1;

						sData->Z_Acc =tmpZ*MPU6050_MEMS_UNIT;
						
						retCode =0;
				}
		}
		
		if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	????????
		else
				ACCMutexFlag	=	1;

		return retCode;
  										
}
/*
********************************************************************************************************
*                                       BSP_ACC_ReadGyro(u16 timeout)
*
* Description : 读取3轴陀螺仪信息
*
* Argument(s) : 
*
* Return(s)   : 0,init accelerometer OK, <0 ,init accelerometer error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8 BSP_ACC_ReadGyro (MEMS_data_Typedef *sData,	u32	timeout)
{   
		u8 err1=OS_NO_ERR,tmp1=0,tmp2=0;	    
		s8 retCode=0;
	
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求信号量
		else
				if(ACCMutexFlag	== 1)
						ACCMutexFlag	=	0;
				else
						return -1;
		
		if(err1 != OS_NO_ERR)
		{
				if(err1	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}
		else
		{ 
				if(MEMS_DEVICE.InitSta == DEF_CHIP_LIS3DH)
				{
						retCode = -1;
				}
				else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
				{
						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_XOUT_L); 
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_XOUT_H); 
						sData->X_Gyro=	(tmp2<<8)|tmp1;
						// tmp=(float)tmp/16.4  ;  //2000度/s量程，分辨率为16.4度/s

						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_YOUT_L); 
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_YOUT_H); 
						sData->Y_Gyro=	(tmp2<<8)|tmp1;
						// tmp=(float)tmp/16.4  ;  //2000度/s量程，分辨率为16.4度/s


						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_ZOUT_L); 
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_ZOUT_H); 
						sData->Z_Gyro=	(tmp2<<8)|tmp1;
						// tmp=(float)tmp/16.4  ;  //2000度/s量程，分辨率为16.4度/s
					
						retCode =0;
				}
		}
		
		if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	????????
		else
				ACCMutexFlag	=	1;

		return retCode;
  										
}

/*
********************************************************************************************************
*                                       BSP_ACC_Check
*
* Description : 是被当前加速度传感器工作状态及芯片类型，更新MEMS_DEVICE结构体内容
*
* Argument(s) :  用于周期性检查加速度芯片状态
*
* Return(s)   : 0芯片成功识别，-1识别错误
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8 BSP_ACC_Check (void)	
{
		s8	ret=0;
		if(I2C_SingleRead(LIS3DH_Addr,LIS3DH_WhoAmI) == LIS3DH_WhoAmI_Vaule)
		{
				MEMS_DEVICE.HardWareSta	=	DEF_MEMSHARD_NONE_ERR;
				MEMS_DEVICE.InitSta     = DEF_CHIP_LIS3DH;	// 置模加速度芯片类型
				ret =0;
		}
		else if(I2C_SingleRead(MPU6050_Addr,MPU6050_RA_WHO_AM_I) == MPU6050_WhoAmI_Vaule)
		{
				MEMS_DEVICE.HardWareSta	=	DEF_MEMSHARD_NONE_ERR;
				MEMS_DEVICE.InitSta     = DEF_CHIP_MPU6050;	// 置模加速度芯片类型
				ret =0; 
		}
		else
		{
				MEMS_DEVICE.HardWareSta	=	DEF_MEMSHARD_DEVICE_ERR;
				MEMS_DEVICE.InitSta     = DEF_CHIP_NONE;		// 置模加速度芯片类型
				ret =-1;
		}
		return ret;	
}
/*
********************************************************************************************************
*                                       BSP_ACC_Init
*
* Description : Init accelerometer,set default mode(normal mode, INT1 off， filter bypassed).
*
* Argument(s) : 
*
* Return(s)   : 0,init accelerometer OK, <0 ,init accelerometer error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8 BSP_ACC_Init (void)
{	
		u8 tmp=0,val=0;
	
		// 初始化IIC总线(I/O模拟方式)
		BSP_IIC_Init(DEF_ACC_PORT);	
	
		// 全局变量初始化
		memset((u8 *)&s_ACC,	0,	sizeof(s_ACC));
		memset((u8 *)&s_Mems,	0,	sizeof(s_Mems));
		
	
		// 加速度芯片探测
		AccDelayMS(500);
		BSP_ACC_Check();
	
		// 打印初始化信息
		#if(DEF_INITINFO_OUTPUTEN > 0)
		if(dbgInfoSwt & DBG_INFO_SYS)
		{			
				if(MEMS_DEVICE.InitSta == DEF_CHIP_LIS3DH)
						myPrintf("[Init]: ACC........(ok)->LIS3DH!\r\n");
				else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
						myPrintf("[Init]: ACC........(ok)->MPU6050!\r\n");
				else
						myPrintf("[Init]: ACC........(error)!\r\n");
		}
		#endif	
		
		// 初始化加速度芯片
		if(MEMS_DEVICE.InitSta == DEF_CHIP_LIS3DH)
		{	
				tmp = DEF_ODR_NOMAL_100HZ | 0x07;
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg1,tmp); 	// XYZ采集输出使能,采集频率100HZ
			
				// I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg2,0x89);	// 使能数据输出的重力过滤功能
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg2,0x81);	// 使能数据输出带重力分量,FDS使能,使能INT1上的高通滤波器(过滤掉重力因素)	
				// 2015-8-31 测试中发现当设备倾斜安装时6D移动检测功能大大减弱，调整到81后正常
				
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg4,0x18);	// 4G量程,高分辨率使能(高4bit,0-2G,1-4G,2-8G,3-16G)
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg5,0x00);	// 禁止INT0的中断锁定功能
			
				tmp = 0x01|0x02|0x04|0x08|0x10|0x20 | (0x01<<6);
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_CFG,tmp);		// 6方向移动中断事件使能
			
				//I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_DURATION,1);	// 配置中断时长ms	
				//I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_THS,10);		// 配置INT1中断门限mg
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg3,0x00);// 禁止使能INT1,INT2加速度外部中断(L1_AOL1,L1_AOL2)			
		}
		else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
		{
				//设置MPU6050步骤
				//1、设置时钟源 
				//2、Set full-scale gyroscope range
				//3、5.371
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_PWR_MGMT_1, MPU_60X0_RESET_REG_VALU); //该位置1，重启内部寄存器到默认值。复位完成后该位自动清0。各个寄存器的默认值在第3 节。  
				AccDelayMS(10);//延时1s
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_PWR_MGMT_1, MPU_60X0_PWR_MGMT_1_REG_VALU); //Disable temperature sensor, Internal 8MHz oscillator   0x08
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_SMPLRT_DIV, MPU_60X0_SMPLRT_DIV_REG_VALU);	  //分频系数=7 ,最终采样频率1khz

				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_CONFIG,0); 	//MPU_60X0_CONFIG_REG_VALU	  //DLPF=6
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_GYRO_CONFIG, MPU_60X0_GYRO_CONFIG_REG_VALU);  //陀螺仪配置为全量程范围± 2000 °/s
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_ACCEL_CONFIG, MPU_60X0_ACCEL_CONFIG_REG_VALU); //加速度传感器全程范围 ± 4g
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_FIFO_EN, MPU_60X0_FIFO_EN_REG_VALU);    //只有加速度和陀螺仪传感器使能发送数据到FIFO

				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_INT_PIN_CFG, 0x80);   //MPU6050_RA_INT_PIN_CFG  //中断管脚配置寄存器 低电平,中断后位置底电平直到中断清除	
				//I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_MOT_THR , 10);  		//中断门限1kHZ 1个数值为1mg
				//I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_MOT_DUR , 1);  		//中断时长1kHZ 1个数值为1ms				
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_INT_ENABLE , 0x00);  //MPU6050_RA_INT_ENABLE  //禁止使能寄存器,移动中断使能
		}	
		BSP_ACC_IT_IO_CON(DISABLE);	// 默认关闭外部中断
		return MEMS_DEVICE.HardWareSta;
}

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/



/*****************************************end*********************************************************/
