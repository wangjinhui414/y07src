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

// ����˵����
// IIC����ͨ�ţ�I/Oģ�ⷽʽ
// ���������Զ�ʶ��MPU6050��LIS3DH��
// LIS3DH,MPU6050 IICģʽ,����ģʽ���400Khz(MPU6050ֻ֧��IIC��֧��SPI)

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

/********** IICģ�ⷽʽ�˿ڶ��� *************/

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

/********** �ڲ� ACC I/O ���ƽӿ� *************/

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

extern    OS_EVENT  				*ACCMutexSem;     	// 	����ACC���ߵײ�Ӧ��֮��Ļ���
extern		OS_EVENT					*CrashMEMSSem;			//  ��ײ�ź���
static		vu8								ACCMutexFlag=1;			//	��OSʱʵ��SPI���ߵĻ���


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

// I2C �ײ㺯��
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

// ���ٶ�оƬ�ײ�Ӧ�ú���
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
   u8 i=150; 	//��������Ż��ٶ� ,��������͵�5����д��
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
    if(!IIC_SDA_read)return FALSE;        //SDA��Ϊ�͵�ƽ������æ,�˳�
    IIC_SDA_L;
    IIC_NOP;
    if(IIC_SDA_read) return FALSE;        //SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
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
* Description    : ����Ϊ:=1��ACK,=0��ACK     
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
* Description    : ����һ���ֽ�  
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
				//---------���ݽ���----------
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
				//---���ݽ�������һ����ʱ----

				//----����һ��������[������] 
				IIC_SCL_H;
				IIC_NOP;
				IIC_SCL_L;
				IIC_NOP;//��ʱ,��ֹSCL��û��ɵ�ʱ�ı�SDA,�Ӷ�����START/STOP�ź� 
		}
		//���մӻ���Ӧ��
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
* Description    : ����һ���ֽ�
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
				IIC_SCL_H;//����ʱ��������[������],�ôӻ�׼�������� 
				IIC_NOP;
				recData<<=1;
				if(IIC_SDA_read) //������״̬
				{
						recData|=0x01; 
				}   
				IIC_SCL_L;//׼�����ٴν�������  
				IIC_NOP;//�ȴ�����׼����         
		}
		///DebugPrint("TWI_Dat:%x\n",Dat);
		return recData;    
}
/*
*********************************************************************************************************
* Function Name  : I2C_SingleWrite
* Description    : д���� 
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
* Description    : ������  
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
* Description    : ���ٶ�INT1�жϷ�����
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
* Description    : ���ٶ�INT1�жϿ���
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
* Description : ��ȡ�Ĵ�����ȡ
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
* Description : д�Ĵ���
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
* Description : ���ü��ٶ��жϼĴ���(�ж����޼�ʱ��)
*
* Argument(s) : intEn: ʹ��״̬DEF_ENABLE,DEF_DISABLE(�Ĵ���������Ϻ��жϵ�ʹ��״̬), threshold:��λ,mg	duration:��λms
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
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	�����ź���
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
						// �ж���������
						if((threshold > 127) || (duration > 127))	// LIS3DHΪ��ЧλΪ7bit
								retCode =-1;
						else
						{
								// ����ǰ���ж�
								I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg3,0x00);// ��ֹINT1���ٶ��ⲿ�ж�(L1_AOL1)
								/*
								// LIS3DH�ô��㷨����������ʱȡ��
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
								I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_THS,tmp);// ����INT1�ж�����
								/*
								// LIS3DH�ô��㷨����������ʱȡ��
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
								I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_DURATION,tmp);	// �����ж�ʱ��ms
								if(intEn == DEF_ENABLE)
										I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg3,0x40);// ʹ��INT1���ٶ��ⲿ�ж�(L1_AOL1)								
								retCode =0;
						}
				}
				else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
				{
						I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_INT_ENABLE , 0x00);  		// ����ǰ��ֹ�ж�ʹ�ܼĴ���,�ƶ��ж�ʹ��
						I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_MOT_THR , threshold);  	// �ж�����1kHZ 1����ֵΪ1mg
						I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_MOT_DUR , duration);  	// �ж�ʱ��1kHZ 1����ֵΪ1ms
						if(intEn == DEF_ENABLE)
								I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_INT_ENABLE , 0x40);  //MPU6050_RA_INT_ENABLE  //�ж�ʹ�ܼĴ���,�ƶ��ж�ʹ��
						retCode =0;
				}
		}
		
		memset((u8 *)&s_ACC,	0,	sizeof(s_ACC));	// ���жϽṹ����
		
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
* Description : ��ȡ3����ٶ���Ϣ
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
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	�����ź���
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
								AccDelayMS(20);	// ����Ƶ������  
						}
						if(i > 2)//read status timeout ,return err
						{
								retCode	=	-1;		
						}
						else
						{
								// ��ȡX����ֵ///////////////////////////////////////////
								tmp1=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_X_L); 
								tmp2=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_X_H); 
								tmpX =(s16)(tmp2<<8)|tmp1;

								sData->X_Acc =tmpX*LIS3DH_MEMS_UNIT;
							
								// ��ȡY����ֵ///////////////////////////////////////////
								tmp1=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_Y_L);
								tmp2=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_Y_H);
								tmpY=	(tmp2<<8)|tmp1;			 

								sData->Y_Acc =tmpY*LIS3DH_MEMS_UNIT;
								
								// ��ȡZ����ֵ///////////////////////////////////////////
								tmp1=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_Z_L);
								tmp2=I2C_SingleRead(LIS3DH_Addr,LIS3DH_Out_Z_H);
								tmpZ=	(tmp2<<8)|tmp1;

								sData->Z_Acc =tmpZ*LIS3DH_MEMS_UNIT;
								
								retCode =0;
						}
				}
				else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
				{
						// ��ȡX����ֵ///////////////////////////////////////////
						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_XOUT_L); 
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_XOUT_H); 
						tmpX =(tmp2<<8)|tmp1;

						
						sData->X_Acc =tmpX*MPU6050_MEMS_UNIT;
						
						// ��ȡY����ֵ///////////////////////////////////////////
						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_YOUT_L);
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_ACCEL_YOUT_H);
						tmpY=	(tmp2<<8)|tmp1;			 

						sData->Y_Acc =tmpY*MPU6050_MEMS_UNIT;
						
						// ��ȡZ����ֵ///////////////////////////////////////////
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
* Description : ��ȡ3����������Ϣ
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
				OSSemPend(ACCMutexSem,	timeout,	&err1);		//	�����ź���
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
						// tmp=(float)tmp/16.4  ;  //2000��/s���̣��ֱ���Ϊ16.4��/s

						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_YOUT_L); 
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_YOUT_H); 
						sData->Y_Gyro=	(tmp2<<8)|tmp1;
						// tmp=(float)tmp/16.4  ;  //2000��/s���̣��ֱ���Ϊ16.4��/s


						tmp1=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_ZOUT_L); 
						tmp2=I2C_SingleRead(MPU6050_Addr,MPU6050_RA_GYRO_ZOUT_H); 
						sData->Z_Gyro=	(tmp2<<8)|tmp1;
						// tmp=(float)tmp/16.4  ;  //2000��/s���̣��ֱ���Ϊ16.4��/s
					
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
* Description : �Ǳ���ǰ���ٶȴ���������״̬��оƬ���ͣ�����MEMS_DEVICE�ṹ������
*
* Argument(s) :  ���������Լ����ٶ�оƬ״̬
*
* Return(s)   : 0оƬ�ɹ�ʶ��-1ʶ�����
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
				MEMS_DEVICE.InitSta     = DEF_CHIP_LIS3DH;	// ��ģ���ٶ�оƬ����
				ret =0;
		}
		else if(I2C_SingleRead(MPU6050_Addr,MPU6050_RA_WHO_AM_I) == MPU6050_WhoAmI_Vaule)
		{
				MEMS_DEVICE.HardWareSta	=	DEF_MEMSHARD_NONE_ERR;
				MEMS_DEVICE.InitSta     = DEF_CHIP_MPU6050;	// ��ģ���ٶ�оƬ����
				ret =0; 
		}
		else
		{
				MEMS_DEVICE.HardWareSta	=	DEF_MEMSHARD_DEVICE_ERR;
				MEMS_DEVICE.InitSta     = DEF_CHIP_NONE;		// ��ģ���ٶ�оƬ����
				ret =-1;
		}
		return ret;	
}
/*
********************************************************************************************************
*                                       BSP_ACC_Init
*
* Description : Init accelerometer,set default mode(normal mode, INT1 off�� filter bypassed).
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
	
		// ��ʼ��IIC����(I/Oģ�ⷽʽ)
		BSP_IIC_Init(DEF_ACC_PORT);	
	
		// ȫ�ֱ�����ʼ��
		memset((u8 *)&s_ACC,	0,	sizeof(s_ACC));
		memset((u8 *)&s_Mems,	0,	sizeof(s_Mems));
		
	
		// ���ٶ�оƬ̽��
		AccDelayMS(500);
		BSP_ACC_Check();
	
		// ��ӡ��ʼ����Ϣ
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
		
		// ��ʼ�����ٶ�оƬ
		if(MEMS_DEVICE.InitSta == DEF_CHIP_LIS3DH)
		{	
				tmp = DEF_ODR_NOMAL_100HZ | 0x07;
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg1,tmp); 	// XYZ�ɼ����ʹ��,�ɼ�Ƶ��100HZ
			
				// I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg2,0x89);	// ʹ������������������˹���
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg2,0x81);	// ʹ�������������������,FDSʹ��,ʹ��INT1�ϵĸ�ͨ�˲���(���˵���������)	
				// 2015-8-31 �����з��ֵ��豸��б��װʱ6D�ƶ���⹦�ܴ�������������81������
				
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg4,0x18);	// 4G����,�߷ֱ���ʹ��(��4bit,0-2G,1-4G,2-8G,3-16G)
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg5,0x00);	// ��ֹINT0���ж���������
			
				tmp = 0x01|0x02|0x04|0x08|0x10|0x20 | (0x01<<6);
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_CFG,tmp);		// 6�����ƶ��ж��¼�ʹ��
			
				//I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_DURATION,1);	// �����ж�ʱ��ms	
				//I2C_SingleWrite(LIS3DH_Addr,LIS3DH_INT1_THS,10);		// ����INT1�ж�����mg
				I2C_SingleWrite(LIS3DH_Addr,LIS3DH_Ctrl_Reg3,0x00);// ��ֹʹ��INT1,INT2���ٶ��ⲿ�ж�(L1_AOL1,L1_AOL2)			
		}
		else if(MEMS_DEVICE.InitSta == DEF_CHIP_MPU6050)
		{
				//����MPU6050����
				//1������ʱ��Դ 
				//2��Set full-scale gyroscope range
				//3��5.371
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_PWR_MGMT_1, MPU_60X0_RESET_REG_VALU); //��λ��1�������ڲ��Ĵ�����Ĭ��ֵ����λ��ɺ��λ�Զ���0�������Ĵ�����Ĭ��ֵ�ڵ�3 �ڡ�  
				AccDelayMS(10);//��ʱ1s
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_PWR_MGMT_1, MPU_60X0_PWR_MGMT_1_REG_VALU); //Disable temperature sensor, Internal 8MHz oscillator   0x08
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_SMPLRT_DIV, MPU_60X0_SMPLRT_DIV_REG_VALU);	  //��Ƶϵ��=7 ,���ղ���Ƶ��1khz

				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_CONFIG,0); 	//MPU_60X0_CONFIG_REG_VALU	  //DLPF=6
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_GYRO_CONFIG, MPU_60X0_GYRO_CONFIG_REG_VALU);  //����������Ϊȫ���̷�Χ�� 2000 ��/s
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_ACCEL_CONFIG, MPU_60X0_ACCEL_CONFIG_REG_VALU); //���ٶȴ�����ȫ�̷�Χ �� 4g
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_FIFO_EN, MPU_60X0_FIFO_EN_REG_VALU);    //ֻ�м��ٶȺ������Ǵ�����ʹ�ܷ������ݵ�FIFO

				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_INT_PIN_CFG, 0x80);   //MPU6050_RA_INT_PIN_CFG  //�жϹܽ����üĴ��� �͵�ƽ,�жϺ�λ�õ׵�ƽֱ���ж����	
				//I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_MOT_THR , 10);  		//�ж�����1kHZ 1����ֵΪ1mg
				//I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_MOT_DUR , 1);  		//�ж�ʱ��1kHZ 1����ֵΪ1ms				
				I2C_SingleWrite(MPU6050_Addr,MPU6050_RA_INT_ENABLE , 0x00);  //MPU6050_RA_INT_ENABLE  //��ֹʹ�ܼĴ���,�ƶ��ж�ʹ��
		}	
		BSP_ACC_IT_IO_CON(DISABLE);	// Ĭ�Ϲر��ⲿ�ж�
		return MEMS_DEVICE.HardWareSta;
}

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/



/*****************************************end*********************************************************/
