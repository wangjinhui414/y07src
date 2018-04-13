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
*                                     			bsp_can_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : bsp_can_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_CAN_GLOBLAS
#include	<string.h>
#include	"bsp_h.h"
#include	"bsp_can_h.h"
#include 	"ucos_ii.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/********** 内部 CAN I/O 控制接口 *************/

// CAN 工作模式设置
#define	IO_CAN1_STAR_WORK()					(GPIO_ResetBits	(DEF_CAN1_SLEEP_PORTS, DEF_CAN1_SLEEP_PINS))
#define	IO_CAN1_GOTO_SLEEP()				(GPIO_SetBits		(DEF_CAN1_SLEEP_PORTS, DEF_CAN1_SLEEP_PINS))
#define	IO_CAN2_STAR_WORK()					(GPIO_ResetBits	(DEF_CAN2_SLEEP_PORTS, DEF_CAN2_SLEEP_PINS))
#define	IO_CAN2_GOTO_SLEEP()				(GPIO_SetBits		(DEF_CAN2_SLEEP_PORTS, DEF_CAN2_SLEEP_PINS))
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
uc8	CAN_BAUD_100K[4]		={CAN_SJW_1tq,	CAN_BS1_16tq,	CAN_BS2_3tq,	18};

uc8	CAN_BAUD_125K[4]		={CAN_SJW_1tq,	CAN_BS1_14tq,	CAN_BS2_3tq,	16};

uc8	CAN_BAUD_250K[4]		={CAN_SJW_1tq,	CAN_BS1_14tq,	CAN_BS2_3tq,	8};

uc8	CAN_BAUD_500K[4]		={CAN_SJW_1tq,	CAN_BS1_8tq,	CAN_BS2_3tq,	6};

uc8	CAN_BAUD_1000K[4]		={CAN_SJW_1tq,	CAN_BS1_5tq,	CAN_BS2_3tq,	4};	


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

//extern	void  OBD_CAN1_RX0_IRQHandler (CanRxMsg  *RxMsg);		// 由OBD实现中断字节的处理
//extern	void  OBD_CAN2_RX0_IRQHandler (CanRxMsg  *RxMsg);		// 由OBD实现中断字节的处理

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static	void	CanDelayMS	(u16	nms);

/*
*********************************************************************************************************
*********************************************************************************************************
*                                              CAN FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : CanDelayMS
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	void	CanDelayMS	(u16	nms)
{
		if(OSRunning > 0)
				OSTimeDly(nms);
		else
				SoftDelayMS(nms);
}

/*******************************************************************************
* Function Name  : BSP_CanWork
* Description    : 
* Input          : 
*									 
* Output         : None
* Return         : 
*******************************************************************************/
void	BSP_CanWork	(u8	CAN_ID)
{
		if(CAN_ID	== DEF_CAN_1){
				CAN_WakeUp(CAN1);
		}else if(CAN_ID==DEF_CAN_2){
				CAN_WakeUp(CAN2);
		}
}

/*******************************************************************************
* Function Name  : BSP_CanSleep
* Description    : 
* Input          : 
*									 
* Output         : None
* Return         : 
*******************************************************************************/
void	BSP_CanSleep (u8	CAN_ID)
{
		if(CAN_ID	== DEF_CAN_1){
				CAN_Sleep(CAN1);	
		}else if(CAN_ID==DEF_CAN_2){
				CAN_Sleep(CAN2);
		}
}
/*******************************************************************************
* Function Name  : BSP_CanTransWork
* Description    : 
* Input          : 
*									 
* Output         : None
* Return         : 
*******************************************************************************/
void	BSP_CanTransWork	(u8	CAN_ID)
{
		if(CAN_ID	== DEF_CAN_1){
				IO_CAN1_STAR_WORK();
		}else if(CAN_ID==DEF_CAN_2){
				IO_CAN2_STAR_WORK();
		}
}

/*******************************************************************************
* Function Name  : BSP_CanTransSleep
* Description    : 
* Input          : 
*									 
* Output         : None
* Return         : 
*******************************************************************************/
void	BSP_CanTransSleep (u8	CAN_ID)
{
		if(CAN_ID	== DEF_CAN_1)
				IO_CAN1_GOTO_SLEEP();	
		else if(CAN_ID	== DEF_CAN_2)
				IO_CAN2_GOTO_SLEEP();	
}

/*******************************************************************************
* Function Name  : BSP_CAN_Device_Init
* Description    : 
* Input          : 
*									 
* Output         : None
* Return         : 
*******************************************************************************/
s8  BSP_CAN_Device_Init(u8 CAN_ID,	u8	mode,	ENUM_CAN_BAUD	baudRate)
{
		u8	i=0;
		s8	sta=DEF_CAN_FAILED_ERR;

		if(CAN_ID	==	DEF_CAN_1)		
		{
				// Config can bus I/O
				BSP_CAN_Init(DEF_CAN_1);

				// Setup baudrate
				while((sta != DEF_CAN_NONE_ERR) && (i<3))
				{
						sta = BSP_CAN_Setup_Baud(DEF_CAN_1,  mode,	baudRate);
						i++;
						CanDelayMS(100);
				}
		}
		else	if(CAN_ID	==	DEF_CAN_2)
		{
				// Config can bus I/O
				BSP_CAN_Init(DEF_CAN_2);

				// Setup baudrate
				while((sta != DEF_CAN_NONE_ERR) && (i<3))
				{
						sta = BSP_CAN_Setup_Baud(DEF_CAN_2,  mode,	baudRate);
						i++;
						CanDelayMS(100);
				}
		}

		return	sta;
}
/*******************************************************************************
* Function Name  : BSP_CAN_Setup_Baud
* Description    : Configures the CAN and transmit and receive mode and baudRate
* Input          : mode : CAN_Mode_Normal / CAN_Mode_LoopBack / CAN_Mode_Silent / CAN_Mode_Silent_LoopBack
*									 
* Output         : None
* Return         : PASSED if the reception is well done, FAILED in other case
*******************************************************************************/
s8  BSP_CAN_Setup_Baud(u8 CAN_ID,	u8	mode,	ENUM_CAN_BAUD	baudRate)
{
	  CAN_InitTypeDef        CAN_InitStructure;
		u8	TestInit;
		u8  *baud;	
	
	  /* CAN register init */
	
		if(CAN_ID == DEF_CAN_1)
		{
	  		CAN_DeInit(CAN1);
		}
		else if(CAN_ID == DEF_CAN_2)
		{
				CAN_DeInit(CAN2);
		}	
	
	  CAN_StructInit(&CAN_InitStructure);
	
		if(baudRate == BAUD_C_100K)  
		  	baud	=	(u8 *)CAN_BAUD_100K;
		else if(baudRate == BAUD_C_125K)
				baud	=	(u8 *)CAN_BAUD_125K;
		else if(baudRate == BAUD_C_250K)
				baud	=	(u8 *)CAN_BAUD_250K;
		else if(baudRate == BAUD_C_500K)
				baud	=	(u8 *)CAN_BAUD_500K;
		else if(baudRate == BAUD_C_1000K)
				baud	=	(u8 *)CAN_BAUD_1000K;
		else
				return DEF_CAN_FAILED_ERR;	
		
		/* CAN cell init */
	  CAN_InitStructure.CAN_TTCM=DISABLE;			// 使能时间触发通信模式(禁止后只影响发送时间戳产生)
	  CAN_InitStructure.CAN_ABOM=ENABLE;			// 一旦硬件检测到128次11位连续的隐性位，则自动退出离线状态
	  CAN_InitStructure.CAN_AWUM=ENABLE;			// 睡眠模式通过检测CAN报文，由硬件自动唤醒
	  CAN_InitStructure.CAN_NART=DISABLE;			// 按照CAN标准，CAN硬件在发送报文失败时会一直自动重传直到发送成功
	  CAN_InitStructure.CAN_RFLM=DISABLE;			// 在接收溢出时FIFO未被锁定当接收FIFO的报文未被读出下一个收到的报文会覆盖原有的报文
	  CAN_InitStructure.CAN_TXFP=ENABLE; 			// 优先级由发送请求的顺序来决定
	  CAN_InitStructure.CAN_Mode=mode;
	  CAN_InitStructure.CAN_SJW=baud[0];
	  CAN_InitStructure.CAN_BS1=baud[1];
	  CAN_InitStructure.CAN_BS2=baud[2];
	  CAN_InitStructure.CAN_Prescaler=baud[3];
		
	  if(CAN_ID == DEF_CAN_1)
		{
				TestInit = CAN_Init(CAN1,	&CAN_InitStructure);
				CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
		}
		else if(CAN_ID == DEF_CAN_2)
		{
				TestInit = CAN_Init(CAN2,	&CAN_InitStructure);
				CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);
		}	
		
		if(TestInit == CANINITFAILED)
				return	DEF_CAN_FAILED_ERR;
		else
				return	DEF_CAN_NONE_ERR;
}
/*******************************************************************************
* Function Name  : BSP_CAN_Setup_Filter32
* Description    : Configures the CAN(CAN1 & CAN2 share) filter32
* Input          : filtgerNum = 0~27 or 0~13
*									 filterMode = CAN_FilterMode_IdList or CAN_FilterMode_IdMask
*									 id					= id | (BIT_IDE/BIT_RTR or BIT_IDE & BIT_RTR)
*									 maskId			=	maskId | (BIT_IDE/BIT_RTR or BIT_IDE & BIT_RTR)
*									 assignmen	= CAN_FIFO0 or CAN_FIFO1
*									 enable			=	ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void	BSP_CAN_Setup_Filter32(u8	CAN_ID,	u8	filterNum,	u8 filterMode,	u32 id,	 u32 maskId,
												 		 u8 assignmen,	FunctionalState enable)
{
		CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	  u16	idHigh =0,idLow =0, maskIdHigh =0, maskIdLow =0;
	
		if((id & IDE_BIT) !=  0) //29bit ID
		{
				idHigh 			= (u16)((((id & 0x1FFFFFFF) << 3) & 0xFFFF0000) >> 16);
				idLow	 			= (u16)(((id & 0x1FFFFFFF) << 3) & 0x0000FFFF | bit32_IDE);
		}
		else 									 //11bit ID
		{
				idHigh 			= (u16)((id & 0x000007FF) << 5);
				idLow				= 0x0000;
		}
		if((id & RTR_BIT) != 0) //remote frame
		{
				idLow				|= bit32_RTR;
		}
		//////////////////////////////////////////////////////////////////////////////////
		if((maskId & IDE_BIT) !=  0) //29bit ID
		{
				maskIdHigh 	=	(u16)((((maskId & 0x1FFFFFFF) << 3) & 0xFFFF0000) >> 16);
				maskIdLow		=	(u16)(((maskId & 0x1FFFFFFF) << 3) & 0x0000FFFF | bit32_IDE);
		}
		else 									 //11bit ID
		{
				maskIdHigh	=	(u16)((maskId & 0x000007FF) << 5);
				maskIdLow		=	0x0000;	 
		}
		if((maskId & RTR_BIT) != 0) //remote frame
		{
				maskIdLow		|= bit32_RTR;
		} 
		
		/* CAN filter init */
	  CAN_FilterInitStructure.CAN_FilterNumber=filterNum;//filter.CAN_FilterNumber;//1;
	  CAN_FilterInitStructure.CAN_FilterMode=filterMode;//CAN_FilterMode_IdMask;
	  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	  CAN_FilterInitStructure.CAN_FilterIdHigh=idHigh;//0x0000;
	  CAN_FilterInitStructure.CAN_FilterIdLow=idLow;//0x0000;
	  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=maskIdHigh;//0x0000;
	  CAN_FilterInitStructure.CAN_FilterMaskIdLow=maskIdLow;//0x0000;
	  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=assignmen;//CAN_FIFO0;
	  CAN_FilterInitStructure.CAN_FilterActivation=enable;//ENABLE;
	
		if(CAN_ID == DEF_CAN_1){
	  		CAN_FilterInit(&CAN_FilterInitStructure);
		}else if(CAN_ID == DEF_CAN_2){
				CAN_FilterInit(&CAN_FilterInitStructure);
		}
}

/*******************************************************************************
* Function Name  : BSP_CAN_Setup_Filter16
* Description    : Configures the CAN(CAN1 & CAN2 share) filter16
* Input          : filtgerNum = 0~27 or 0~13
*									 filterMode = CAN_FilterMode_IdList or CAN_FilterMode_IdMask
*									 id1					= id1 | (BIT_IDE/BIT_RTR or BIT_IDE & BIT_RTR)
*									 maskId1			=	maskId1 | (BIT_IDE/BIT_RTR or BIT_IDE & BIT_RTR)
*									 id2					= id2 | (BIT_IDE/BIT_RTR or BIT_IDE & BIT_RTR)
*									 maskId2			=	maskId2 | (BIT_IDE/BIT_RTR or BIT_IDE & BIT_RTR)
*									 assignmen	= CAN_FIFO0 or CAN_FIFO1
*									 enable			=	ENABLE or DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void	BSP_CAN_Setup_Filter16(u8	CAN_ID,	u8	filterNum,	u8 filterMode,	u32 id1,	u32 maskId1,	u32	id2,	u32	maskId2,	 
											 	 		 u8 assignmen,	FunctionalState enable)
{
		CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	  u16	idHigh =0,idLow =0, maskIdHigh =0, maskIdLow =0;
		
		if((id1 & IDE_BIT) != 0) //29bit ID
		{
		 		idLow 			= (u16)(((id1 & 0x00038000) >> 15) | bit16_IDE);
		}
		else										//11bit ID
		{
				idLow				=	(u16)((id1 & 0x000007FF) << 5);
		}
		if((id1 & RTR_BIT) != 0) //remote frame
		{
				idLow 			|= bit16_RTR;
		}
		//////////////////////////////////////////////////////////////////////////////////
		if((maskId1 & IDE_BIT) != 0) //29bit ID
		{
		 		idHigh 			= (u16)(((maskId1 & 0x00038000) >> 15) | bit16_IDE);
		}
		else										//11bit ID
		{
				idHigh				=	(u16)((maskId1 & 0x000007FF) << 5);
		}
		if((maskId1 & RTR_BIT) != 0) //remote frame
		{
				idHigh 			|= bit16_RTR;
		}
		//////////////////////////////////////////////////////////////////////////////////
		if((id2 & IDE_BIT) != 0) //29bit ID
		{
		 		maskIdLow 			= (u16)(((id2 & 0x00038000) >> 15) | bit16_IDE);
		}
		else										//11bit ID
		{
				maskIdLow				=	(u16)((id2 & 0x000007FF) << 5);
		}
		if((id2 & RTR_BIT) != 0) //remote frame
		{
				maskIdLow 			|= bit16_RTR;
		}
		//////////////////////////////////////////////////////////////////////////////////
		if((maskId2 & IDE_BIT) != 0) //29bit ID
		{
		 		maskIdHigh 			= (u16)(((maskId2 & 0x00038000) >> 15) | bit16_IDE);
		}
		else										//11bit ID
		{
				maskIdHigh				=	(u16)((maskId2 & 0x000007FF) << 5);
		}
		if((maskId2 & RTR_BIT) != 0) //remote frame
		{
				maskIdHigh 			|= bit16_RTR;
		}
	
		/* CAN filter init */
	  CAN_FilterInitStructure.CAN_FilterNumber=filterNum;//filter.CAN_FilterNumber;//1;
	  CAN_FilterInitStructure.CAN_FilterMode=filterMode;//CAN_FilterMode_IdMask;
	  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_16bit;
	  CAN_FilterInitStructure.CAN_FilterIdHigh=idHigh;//0x0000;
	  CAN_FilterInitStructure.CAN_FilterIdLow=idLow;//0x0000;
	  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=maskIdHigh;//0x0000;
	  CAN_FilterInitStructure.CAN_FilterMaskIdLow=maskIdLow;//0x0000;
	  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=assignmen;//CAN_FIFO0;
	  CAN_FilterInitStructure.CAN_FilterActivation=enable;//ENABLE;
	
		if(CAN_ID == DEF_CAN_1){
	  		CAN_FilterInit(&CAN_FilterInitStructure); 
		}else if(CAN_ID == DEF_CAN_2){
				CAN_FilterInit(&CAN_FilterInitStructure);
		}
}
/*******************************************************************************
* Function Name  : BSP_CAN_SendPack
* Description    : Configures the CAN and transmit and receive by interruption
* Input          : None
* Output         : None
* Return         : PASSED if the reception is well done, FAILED in other case
*******************************************************************************/
s8 BSP_CAN_SendPack(u8 CAN_ID,	CanTxMsg *Message)
{
		u16 i=0;
		u8	TransmitMailbox;

	  /* transmit 1 message */
		if(CAN_ID == DEF_CAN_1)
		{
				TransmitMailbox = CAN_Transmit(CAN1, Message);	

				while((CAN_TransmitStatus(CAN1,	TransmitMailbox) != CANTXOK) && (i <= 0xFF*(DEF_SYSFREQ_CUN/DEF_SYSFREQ_MIN)))
			  {
			    	i++;
			  }
		}
		else if(CAN_ID == DEF_CAN_2)
		{
				TransmitMailbox = CAN_Transmit(CAN2, Message);	

				while((CAN_TransmitStatus(CAN2,	TransmitMailbox) != CANTXOK) && (i <= 0xFF*(DEF_SYSFREQ_CUN/DEF_SYSFREQ_MIN)))
			  {
			    	i++;
			  }
		}
	  
		if(i >= 0xFF*(DEF_SYSFREQ_CUN/DEF_SYSFREQ_MIN))
				return DEF_CAN_FAILED_ERR;
		else{
	  		return DEF_CAN_NONE_ERR;
		}
}

/*******************************************************************************
* Function Name  : BSP_CAN1_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void BSP_CAN1_RX0_IRQHandler (void)
//{
//		CanRxMsg	RxMsg;
//	
//		memset((u8 *)&RxMsg, 0x00, sizeof(CanRxMsg));
//			
//		CAN_Receive(CAN1, CAN_FIFO0, &RxMsg);

//		//OBD_CAN1_RX0_IRQHandler(&RxMsg);	// 调用OBD CAN接收复位程序
//}
/*******************************************************************************
* Function Name  : BSP_CAN2_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void BSP_CAN2_RX0_IRQHandler (void)
//{
//		
//		CanRxMsg	RxMsg;
//	
//		memset((u8 *)&RxMsg, 0x00, sizeof(CanRxMsg));
//			
//		CAN_Receive(CAN2, CAN_FIFO0, &RxMsg);

//		//OBD_CAN2_RX0_IRQHandler(&RxMsg);	// 调用OBD CAN接收复位程序
//		
//}
/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/



/*****************************************end*********************************************************/

