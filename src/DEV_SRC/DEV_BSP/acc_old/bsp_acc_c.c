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
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

// 驱动说明：
// 驱动程序支持MPU6050,通信总线类型可通过.h文件DEF_ACC_BUSTYPE配置IIC或SPI通信(MPU6050只支持IIC不支持SPI)
// MPU6050 IIC模式下，SCL速率标准模式最大100Khz，快速模式最快400Khz

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


/********** IIC模拟方式端口定义 *************/

#define I2C_Speed     100000

//#define	I2C_ADDR			0x19					// SA0接VCC
#define	I2C_ADDR			0x18					// SA0接GND

#if(DEF_ACC_BUSTYPE == 0) 
#if(DEF_ACC_PORT == DEF_IIC_1)
#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7
#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7
#else // DEF_IIC_2
#define SCL_H         GPIOB->BSRR = GPIO_Pin_10
#define SCL_L         GPIOB->BRR  = GPIO_Pin_10 
#define SDA_H         GPIOB->BSRR = GPIO_Pin_11
#define SDA_L         GPIOB->BRR  = GPIO_Pin_11
#define SCL_read      GPIOB->IDR  & GPIO_Pin_10
#define SDA_read      GPIOB->IDR  & GPIO_Pin_11
#endif
#endif

/********** 内部 ACC I/O 控制接口 *************/

/* Select SPI ACCELEROMETER: CS pin low  */
#define SPI_ACC_CS_EN()   (GPIO_ResetBits	(DEF_ACC_CS_PORTS, DEF_ACC_CS_PINS))
#define SPI_ACC_CS_DIS()  (GPIO_SetBits		(DEF_ACC_CS_PORTS, DEF_ACC_CS_PINS))


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
vu8				ACCMutexFlag=1;												//	无OS时实现SPI总线的互斥


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
static 		bool 			I2C_WaitAck 							(void);
static 		void 			I2C_SendByte 							(u8 SendByte);
static 		u8 				I2C_ReceiveByte 					(void);


// 加速度芯片底层应用函数
static 		void			AccDelayMS								(u16	nms);
static 		s8				ACC_Read									(u8 address, u8 *datas);
static 		s8				ACC_Write									(u8 address, u8 datas);

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
 		SDA_H;
    SCL_H;
    I2C_delay();
    if(!SDA_read)return FALSE;        //SDA线为低电平则总线忙,退出
    SDA_L;
    I2C_delay();
    if(SDA_read) return FALSE;        //SDA线为高电平则总线出错,退出
    SDA_L;
    I2C_delay();
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
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SDA_H;
    I2C_delay();
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
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
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
    SCL_L;
    I2C_delay();
    SDA_H;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SCL_L;
    I2C_delay();
}
/*
*********************************************************************************************************
* Function Name  : I2C_NoAck
* Description    : 返回为:=1有ACK,=0无ACK     
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static bool I2C_WaitAck (void)
{
    SCL_L;
    I2C_delay();
    SDA_H;                        
    I2C_delay();
    SCL_H;
    I2C_delay();
    if(SDA_read)
    {
	      SCL_L;
	      return FALSE;
    }
    SCL_L;
    return TRUE;
}
/*
*********************************************************************************************************
* Function Name  : I2C_SendByte
* Description    : 数据从高位到低位    
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static void I2C_SendByte (u8 SendByte)
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        I2C_delay();
      	if(SendByte&0x80)
        		SDA_H;  
      	else 
        		SDA_L;   
        SendByte<<=1;
        I2C_delay();
        SCL_H;
        I2C_delay();
    }
    SCL_L;
}
/*
*********************************************************************************************************
* Function Name  : I2C_ReceiveByte
* Description    : 数据从高位到低位    
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static u8 I2C_ReceiveByte (void)
{ 
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;                                
    while(i--)
    {
	      ReceiveByte<<=1;      
	      SCL_L;
	      I2C_delay();
	      SCL_H;
	      I2C_delay();        
	      if(SDA_read)
	      {
	        	ReceiveByte|=0x01;
	      }
    }
    SCL_L;
    return ReceiveByte;
}
/*
*********************************************************************************************************
*                                      APP(MPU6050) FUNCTION PROTOTYPES
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
void BSP_ACC_INT1_IRQHandler(void)
{
		MEMS_INT1_FLAG=1;
		if(ACC_IntCnn < 0xffff)
				ACC_IntCnn++;

		if(OSRunning > 0)
				OSSemPost(CrashMEMSSem); 		// 向应用层发送碰撞信号量
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
*                                       ACC_Read(u8 address, u8 *datas)
*
* Description : Read one byte datas from Accelerometer address.
*
* Argument(s) : u8 address,accelerometer register address to read, s8 *datas, Data read from the Accelerometer address.		
*
* Return(s)   : 0,read datas OK, <0 ,err num, read datas error.
*
* Caller(s)   : BSP_ACCELEROMETER.
*
* Note(s)     : private funtion.
*********************************************************************************************************
*/
static s8	ACC_Read(u8 address, u8 *datas)
{ 
		
#if(DEF_ACC_BUSTYPE == DEF_IIC_BUS)

		// IIC实现代码 ///////////////////////////////////////      
    if (!I2C_Start()) 
    		return -1;
    I2C_SendByte(I2C_ADDR);//设置器件地址+段地址 
    if (!I2C_WaitAck()) 
    {
        I2C_Stop(); 
        return -1;
    }
    I2C_SendByte(address);   //设置低起始地址      
    I2C_WaitAck();    
    *datas = I2C_ReceiveByte();
    I2C_NoAck();
    I2C_Stop();

#else	

		// SPI实现代码 ///////////////////////////////////////   	
		SPI_TypeDef	*spi;

		if(DEF_ACC_PORT	== DEF_SPI_1)
				spi = SPI1;
		else if(DEF_ACC_PORT	== DEF_SPI_2)
				spi = SPI2;
		else	
				return	-1;
		
		/* Select the ACCELEROMETER: Chip Select low */
    SPI_ACC_CS_EN(); 

		/* Delay some time (PDF:Tsu(CS)=6ns,Th(CS)=8ns) */
		AccDelayMS(1);
    
		/* Loop while DR register in not emplty */
	  while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);	
	  /* process the high 2 bits r/w, m/s */
    address=(address|0x80)&0xBF;
    /* Send address through the SPI1 peripheral */
	  SPI_I2S_SendData(spi, address);
    /* Wait receive one byte */
		while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);
		/* Return the data */
		SPI_I2S_ReceiveData(spi); 
       
    /* Loop while DR register in not emplty */
	  while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);
    /* Send 0xFF through the SPI1 peripheral */
    SPI_I2S_SendData(spi, 0xFF);
	  /* Wait receive one byte */
	  while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);	
	  /* Return the 1 byte data read from the SPI bus */
	  *datas=SPI_I2S_ReceiveData(spi);

		/* Delay some time (PDF:Tsu(CS)=6ns,Th(CS)=8ns) */
		AccDelayMS(2);
    
    /* Select the ACCELEROMETER: Chip Select high */
    SPI_ACC_CS_DIS();
#endif

		return 0;	
}
/*
********************************************************************************************************
*                                       ACC_Write(u8 address, u8 datas)
*
* Description : Write one byte datas to Accelerometer address.
*
* Argument(s) : u8 address,accelerometer register address to write, u8 datas, Data write to the Accelerometer address.		
*
* Return(s)   : 0,write datas OK, <0 ,err num, write datas error.
*
* Caller(s)   : BSP_ACCELEROMETER.
*
* Note(s)     : private funtion.
*********************************************************************************************************
*/
static s8	ACC_Write(u8 address, u8 datas)
{
#if(DEF_ACC_BUSTYPE == DEF_IIC_BUS)

		// IIC实现代码 /////////////////////////////////////// 
		
    if (!I2C_Start()) 
    		return FALSE;
    I2C_SendByte(I2C_ADDR);//设置器件地址+段地址 
    if (!I2C_WaitAck())
    {
    		I2C_Stop(); 
        return -1;
    }
    I2C_SendByte(address);   //设置段内地址      
    I2C_WaitAck();        
    I2C_SendByte(datas);
    I2C_WaitAck();
    I2C_Stop();

#else	
		// SPI实现代码 ///////////////////////////////////////
		SPI_TypeDef	*spi;

		if(DEF_ACC_PORT	== DEF_SPI_1)
				spi = SPI1;
		else if(DEF_ACC_PORT	== DEF_SPI_2)
				spi = SPI2;
		else	
				return	-1;
		
    /* Select the ACCELEROMETER: Chip Select low */
    SPI_ACC_CS_EN();

		/* Delay some time (PDF:Tsu(CS)=6ns,Th(CS)=8ns) */
		AccDelayMS(1);
    
		/* Loop while DR register in not emplty */
	  while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);
	  /* process the address high 2bit r/w, m/s */
    address=address&0x3F;
    /* Send address through the SPI1 peripheral */
	  SPI_I2S_SendData(spi, address);
    /* Wait receive one byte */
    while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);
		/* Return the data */
    SPI_I2S_ReceiveData(spi);

    /* Loop while DR register in not emplty */
	  while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);    
    /* Send data through the SPI1 peripheral */
	  SPI_I2S_SendData(spi, datas); 
    /* Wait receive one byte */
    while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);
		/* Return the data */
    SPI_I2S_ReceiveData(spi);

		/* Delay some time (PDF:Tsu(CS)=6ns,Th(CS)=8ns) */
		AccDelayMS(2);
      
    /* Select the ACCELEROMETER: Chip Select high */
    SPI_ACCELEROMETER_CS_DIS();
#endif
    
		return 0;
}
/*
******************************************************************************
* Function Name  : BSP_ACC_POWER_CON()
* Description    : 执行power on 后所有参数应重新配置
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
s8	BSP_ACC_POWER_CON (u8	newSta,	u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
				
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
				if(newSta == DEF_ENABLE)
				{
						tmp = DEF_WORK_MODE | 0x07;
						ACC_Write(Ctrl_Reg1,tmp);//0101:normal/low power mode (100Hz),0:normal mode,111:XYZ enable.
				}
				else
						ACC_Write(Ctrl_Reg1,0x00);//0101:normal/low power mode (100Hz),0:normal mode,111:XYZ enable.
		}		
		if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1; 

		return retCode;
}
/*
******************************************************************************
* Function Name  : BSP_ACC_IT_REG_CON()
* Description    : 加速度中断寄存器使能控制
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
s8	BSP_ACC_IT_REG_CON (u8 newSta, u16 timeout)
{				
		u8 err1=OS_NO_ERR,tmp=0;
		s8 retCode=0;
				
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
				ACC_Read(INT1_SRC,&tmp);		    
		    if(newSta == DEF_ENABLE)
		    {       
						ACC_Write(Ctrl_Reg3,0x40);//enable INT
		    }
		    else
		    {
		        ACC_Write(Ctrl_Reg3,0x00);//disable INT
		    }
				MEMS_INT1_FLAG =0; 	// 清中断标志
		}
		if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}
/*
********************************************************************************************************
*                                       BSP_ACC_Set(u8 regAddr,	u8 datas,	u16 timeout)
*
* Description : Set accelerometer.
*
* Argument(s) : 	
*
* Return(s)   : 0,Set OK, <0,Set error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8	BSP_ACC_Set(u8 regAddr,	u8 datas,	u16 timeout)
{
		u8 err1=OS_NO_ERR;
		s8 retCode=0;
		
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
    		ACC_Write(regAddr, datas);//Set register data
		}
    
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}	

/*
********************************************************************************************************
*                                       BSP_ACC_Get(u8 regAddr,	u8 datas,	u16 timeout)
*
* Description : Set accelerometer.
*
* Argument(s) : 
*
* Return(s)   : >=0,Set OK, <0,Set error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8	BSP_ACC_Get(u8 regAddr,	u8 *datas,	u16 timeout)
{
		u8 err1=OS_NO_ERR;
		s8 retCode=0;
		
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
    		ACC_Read(regAddr,	datas);//Get register data
		}   
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}	
/*
********************************************************************************************************
*                                       BSP_ACC_SetInt1Threshold(u16 val,	u16 timeout)
*
* Description : Set accelerometer threshold.
*
* Argument(s) : 	
*
* Return(s)   : 0,Set OK, <0,Set error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8	BSP_ACC_SetInt1Threshold (u16 val,	u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
		
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
   			if(DEF_WORK_SCALE == DEF_FS_SCALE_2G)		
						tmp=((u8)(val/16 +0.5))&0x7F;
				else if(DEF_WORK_SCALE == DEF_FS_SCALE_4G)		
						tmp=((u8)(val/31 +0.5))&0x7F;
				else if(DEF_WORK_SCALE == DEF_FS_SCALE_8G)		
						tmp=((u8)(val/63 +0.5))&0x7F;
				else if(DEF_WORK_SCALE == DEF_FS_SCALE_16G)		
						tmp=((u8)(val/125 +0.5))&0x7F;
				else
						return -1;

				ACC_Write(INT1_THS,tmp);//Set wake up threshold 
		}
    
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}	
/*
********************************************************************************************************
*                                       BSP_ACC_GetInt1Threshold(u16 *val,	u16 timeout)
*
* Description : Get accelerometer.
*
* Argument(s) : 
*
* Return(s)   : >=0,Set OK, <0,Set error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8	BSP_ACC_GetInt1Threshold (u16 *val,	u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
		
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
				ACC_Read(INT1_THS,&tmp);//Get wake up threshold

				if(DEF_WORK_SCALE == DEF_FS_SCALE_2G)		
						*val = tmp*16;
				else if(DEF_WORK_SCALE == DEF_FS_SCALE_4G)		
						*val = tmp*31;
				else if(DEF_WORK_SCALE == DEF_FS_SCALE_8G)		
						*val = tmp*63;
				else if(DEF_WORK_SCALE == DEF_FS_SCALE_16G)		
						*val = (tmp*125);
				else
						return -1;		
		}   
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}	
/*
********************************************************************************************************
*                                       BSP_ACC_SetInt1Duration(u32 val,	u16 timeout)
*
* Description : Set accelerometer threshold.
*
* Argument(s) : 	
*
* Return(s)   : 0,Set OK, <0,Set error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8	BSP_ACC_SetInt1Duration (u32 val,	u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
		
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
				if(DEF_WORK_MODE == DEF_ODR_NOMAL_1HZ)
						tmp = (u8)(val / 1000 + 0.5) & 0x7F;
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_10HZ)
						tmp = (u8)(val / 100 + 0.5) & 0x7F;
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_25HZ)
						tmp = (u8)(val / 40 + 0.5) & 0x7F;
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_50HZ)
						tmp = (u8)(val / 20 + 0.5) & 0x7F;
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_100HZ)
						tmp = (u8)(val / 10 + 0.5) & 0x7F;
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_200HZ)
						tmp = (u8)(val / 5 + 0.5) & 0x7F;
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_400HZ)
						tmp = (u8)(val / 2.5 + 0.5) & 0x7F;
				else
						return -1;

				ACC_Write(INT1_DURATION,tmp);//Set wake up threshold 				
		}
    
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}	
/*
********************************************************************************************************
*                                       BSP_ACC_GetInt1Duration(u32 *val,	u16 timeout)
*
* Description : Get accelerometer.
*
* Argument(s) : 
*
* Return(s)   : >=0,Set OK, <0,Set error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8	BSP_ACC_GetInt1Duration (u32 *val,	u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
		
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
				ACC_Read(INT1_DURATION,&tmp);//Get wake up duration

				if(DEF_WORK_MODE == DEF_ODR_NOMAL_1HZ)
						*val = tmp * 1000; 
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_10HZ)
						*val = tmp * 100; 
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_25HZ)
						*val = tmp * 40; 
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_50HZ)
						*val = tmp * 20; 
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_100HZ)
						*val = tmp * 10; 
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_200HZ)
						*val = tmp * 5; 
				else if(DEF_WORK_MODE == DEF_ODR_NOMAL_400HZ)
						*val = (u32)(tmp * 2.5); 
				else
						return -1;		
		}   
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}	

/*
********************************************************************************************************
*                                       BSP_ACC_Check(void)
*
* Description : Identify Accelerometer LIS3DH is OK.
*
* Argument(s) : None.		
*
* Return(s)   : 0,accelerometer OK, <0,accelerometer error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8	BSP_ACC_Check(u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
    
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	1000,	&err1);		//	请求发送数据信号量
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
			
					
//    		ACC_Read(Who_Am_I,&tmp);
//				if(tmp == 0x33 && retCode == 0)
//				{
//						MEMS_DEVICE.HardWareSta	=	DEF_MEMSHARD_NONE_ERR;
//						retCode =0;
//				}
//				else
//				{
//						MEMS_DEVICE.HardWareSta	=	DEF_MEMSHARD_DEVICE_ERR;
//						retCode =-1;
//				}
    }
    
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}
/*
********************************************************************************************************
*                                       BSP_ACC_Normal(void)
*
* Description : Accelerometer come into normal mode.
*
* Argument(s) : None.		
*
* Return(s)   : 0,accelerometer come into normal mode successful, <0, accelerometer come into normal mode error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8 BSP_ACC_Normal(u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
  
    if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
    		/* accelerometer active mode */
				tmp = DEF_WORK_MODE | 0x07;
		    ACC_Write(Ctrl_Reg1,tmp);//0111:normal/low power mode (100Hz),0:normal mode,111:XYZ enable.
		}
    
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}
/*
********************************************************************************************************
*                                       BSP_ACC_Lowpower(void)
*
* Description : Accelerometer come into low power mode.
*
* Argument(s) : None.	()	
*
* Return(s)   : 0,accelerometer come into low power mode successful, <0,accelerometer come into low power mode error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8 BSP_ACC_Lowpower(u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
    
    if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
  	   	/* accelerometer power down mode */
				tmp = DEF_WORK_MODE | 0x0F;
        ACC_Write(Ctrl_Reg1,tmp);//low power mode
    }
    
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}
/*
********************************************************************************************************
*                                       BSP_ACC_GravityCon(u8 newSta,	u16 timeout)
*
* Description : 重力加速度过滤设置
*
* Argument(s) : DEF_ENABLE:使能重力加速度, DEF_DISABLE:禁止重力加速度
*
* Return(s)   : >=0,Set OK, <0,Set error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8	BSP_ACC_GravityCon (u8 newSta,	u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
		
		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
				if(newSta == DEF_ENABLE)
						ACC_Write(Ctrl_Reg2,0x49);//high pass filter enable
				else
						ACC_Write(Ctrl_Reg2,0x89);//high pass filter enable				
		}   
    if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1;

		return retCode;
}	
/*
********************************************************************************************************
*                                       BSP_ACC_ReadXYZ(MEMS_data_Typedef *MEMS_data ,u16 timeout)
*
* Description : Read XYZ datas from accelerometer.
*
* Argument(s) : MEMS_data_Typedef *datas,XYZ data return to *datas.		
*
* Return(s)   : 0,read datas OK, <0,read data error.
*
* Caller(s)   : App funtion.
*
* Note(s)     : public funtion.
*********************************************************************************************************
*/
s8	BSP_ACC_ReadXYZ(MEMS_data_Typedef *MEMS_data,u16 timeout)
{
		u8 err1=OS_NO_ERR,i;
		s8 retCode=0;
    u8 X_data_h=0,X_data_l=0,Y_data_h=0,Y_data_l=0,Z_data_h=0,Z_data_l=0,tmp=0;
    s16 tmpdata=0;

		if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
    		// 清数据区
				MEMS_data->X_data = 0;
				MEMS_data->Y_data = 0;
				MEMS_data->Z_data = 0;

				// check if data is OK
    		for(i=1;	i<=CHECK_TIME;	i++)
    		{
            /* read the data status to confirm the data is ok to read*/
    				ACC_Read(Status_Reg2, &tmp);
    			  if(0x08 & tmp)//data is ok
    				{
    						break;
    				}
						AccDelayMS(20);	// 避免频繁处理  
    		}
    		if(i > CHECK_TIME)//read status timeout ,return err
    		{
						#if(DEF_ACCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_ACC)
						myPrintf("[ACC]: Read status register error!\r\n");
						#endif
						retCode	=	-1;		
    		}
        else
        {
        		// read X data
        		ACC_Read(Out_X_L,&X_data_l);
        		ACC_Read(Out_X_H,&X_data_h);
            // read Y data
        		ACC_Read(Out_Y_L,&Y_data_l);
        		ACC_Read(Out_Y_H,&Y_data_h);
            // read Z data
        		ACC_Read(Out_Z_L,&Z_data_l);
        		ACC_Read(Out_Z_H,&Z_data_h);
        		
            /* calculate the data */
            tmpdata = (s16)((X_data_h<<8)|X_data_l);
        		MEMS_data->X_data=tmpdata/16*MEMS_UNIT;
            tmpdata = (s16)((Y_data_h<<8)|Y_data_l);
        		MEMS_data->Y_data=tmpdata/16*MEMS_UNIT;
            tmpdata = (s16)((Z_data_h<<8)|Z_data_l);
        		MEMS_data->Z_data=tmpdata/16*MEMS_UNIT;
        }
    }
   
		if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1; 

		return retCode;
}
/*
********************************************************************************************************
*                                       BSP_ACC_Init(u16 timeout)
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
s8 BSP_ACC_Init (u16 timeout)
{
		u8 err1=OS_NO_ERR, tmp=0;
		s8 retCode=0;
    
    if(OSRunning > 0)
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	请求发送数据信号量
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
				#if(DEF_ACC_BUSTYPE == DEF_IIC_BUS)
				/* Init the spi bus */
				BSP_IIC_Init(DEF_ACC_PORT);	
				#else					
				/* Init the spi bus */
				BSP_SPI_Init(DEF_ACC_PORT);	
				#endif	
				
				SCL_H;
				SDA_H;
				
				


			
//				/* init the accerometer */
//				tmp = DEF_WORK_MODE | 0x07;
//				ACC_Write(Ctrl_Reg1,tmp);//0101:normal/low power mode (100Hz),0:normal mode,111:XYZ enable.
//				//ACCELEROMETER_Read(Ctrl_Reg1,&tmp);
//				#if(DEF_WORK_GRAVITY_EN > 0)
//				ACC_Write(Ctrl_Reg2,0x49);//high pass filter enable
//				#else						
//				ACC_Write(Ctrl_Reg2,0x89);//high pass filter enable
//				#endif
//		    //ACCELEROMETER_Read(Ctrl_Reg2,&tmp);
//				ACC_Write(Ctrl_Reg3,0x00);//disable INT
//		    //ACCELEROMETER_Read(Ctrl_Reg3,&tmp);
//				tmp = DEF_WORK_SCALE | 0x08;
//				ACC_Write(Ctrl_Reg4,tmp);//2: Full scale selection +/- 2G High resolution enable
//		    //ACCELEROMETER_Read(Ctrl_Reg4,&tmp);
//				ACC_Write(Ctrl_Reg5,0x00);//INT1 IO not latched
//		    //ACCELEROMETER_Read(Ctrl_Reg5,&tmp);
//				ACC_Write(Ctrl_Reg6,0x02);//INT1 IO active low(??)
//		    //ACCELEROMETER_Read(Ctrl_Reg6,&tmp);
//				ACC_Write(INT1_DURATION,0x00);//set INT1 mode OR, high event
//		    //ACCELEROMETER_Read(INT1_DURATION,&tmp);
//				ACC_Write(INT1_CFG,0x2A);//set INT1 mode OR, high and low event	 0X3F
//		    //ACCELEROMETER_Read(INT1_CFG,&tmp);
//						
//		    /* Ask who am I to comfirm the hardware status */
//		    retCode =ACC_Read(Who_Am_I,&tmp);
//				if((tmp == 0x33) && (retCode == 0))
//				{
//						MEMS_DEVICE.HardWareSta	=	DEF_MEMSHARD_NONE_ERR;
//						retCode =0;
//				}
//				else
//				{
//						MEMS_DEVICE.HardWareSta	=	DEF_MEMSHARD_DEVICE_ERR;
//						retCode =-1;
//				}	
		}
		if(OSRunning > 0)
				OSSemPost(ACCMutexSem);											//	释放占用的信号量
		else
				ACCMutexFlag	=	1; 

		return retCode;
}

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/



/*****************************************end*********************************************************/
