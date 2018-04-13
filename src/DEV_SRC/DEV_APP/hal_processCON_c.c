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
*                                        APPLICATION CODE
*
*                                     	 hal_ProcessCON_c.c
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessCON_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  HAL_PROCESSCON_GLOBLAS
#include	<string.h>
#include	"main_h.h"
#include 	"ucos_ii.h"
#include	"cfg_h.h"
#include	"bsp_h.h"
#include	"hal_h.h"
#include	"hal_processCON_h.h"
#include  "hal_protocol_h.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/* Enable receive interrupt */
#define	CON_ENABLE_RES_IT()					(BSP_RS232_RxIntEn	(DEF_CON_UART))		// include at bsp.h
#define	CON_DISABLE_RES_IT()				(BSP_RS232_RxIntDis	(DEF_CON_UART))		// include at bsp.h

/* Enable send interrupt */
#define	CON_ENABLE_TXD_IT()					(BSP_RS232_TxIntEn	(DEF_CON_UART))		// include at bsp.h
#define	CON_DISABLE_TXD_IT()				(BSP_RS232_TxIntDis	(DEF_CON_UART))		// include at bsp.h


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

extern	OS_EVENT							*CONComMbox;							// 外部控制串口收到数据邮箱
extern	OS_EVENT							*OS_CONReqMbox;						// 应用层请求通信邮箱
extern	OS_EVENT							*OS_CONAckMbox;						// 应用层请求应答邮箱

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

static STU_PACK_INFO       S_stuDatapackInfo;
static STU_MODULE_ST       S_stuModuleSt;
static u32                 S_uiSendTmr;
static u32                 S_uiAlarmTmr;
static CONReqMbox_TypeDef  *PconReqMsg = 0u;
static CONAckMbox_TypeDef  ConAckMsg;
static STU_CAR_ST          S_carSt;


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
// static			u16		MB_CRC16 							(u8 *pucFrame, u16 usLen);
static			void	CONUART_TxHandler 		(void);
static			void	CONUART_RxHandler 		(u8	rxData);
static			void	CON_RxdPackProcess 		(u8 *pBuf,	u16	len);
static			void  CON_RXBUF_UNLOCK 			(void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : HAL_CON_UpDataSn
* Description    : 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
void	HAL_CON_UpDataSn (vu16 *sn)
{		
		if(*sn >= 0xffff)
				*sn =0;
		else
				(*sn)++;				 
}

/*
*********************************************************************************************************
* Function Name  : HAL_CONRES_IT_CON
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	HAL_CONRES_IT_CON(u8	newSta)
{
		if(newSta	==	0)
				CON_DISABLE_RES_IT();
		else
				CON_ENABLE_RES_IT();						
}
/*
*********************************************************************************************************
* Function Name  : BSP_CONTXD_IT_CON
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_CONTXD_IT_CON(u8	newSta)
{
		if(newSta	==	0)
				CON_DISABLE_TXD_IT();
		else
				CON_ENABLE_TXD_IT();						
}
/*
******************************************************************************
* Function Name  : HAL_CONUART_RxTxISRHandler
* Description    : 
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_CONUART_RxTxISRHandler (void)
{
    USART_TypeDef  *usart;
		u8     rx_data;

#if   (DEF_CON_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_CON_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_CON_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_CON_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_CON_UART == DEF_UART_5)
		usart = UART5;

#else
		return;
#endif

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
        rx_data = USART_ReceiveData(usart) & 0xFF;              /* Read one byte from the receive data register         */
        CONUART_RxHandler(rx_data);
        
        USART_ClearITPendingBit(usart, USART_IT_RXNE);          /* Clear the DEF_GPS_UART Receive interrupt                   */
    }
		else if (USART_GetITStatus(usart, USART_IT_TXE) != RESET){
        CONUART_TxHandler();

        USART_ClearITPendingBit(usart, USART_IT_TXE);           /* Clear the DEF_GPS_UART transmit interrupt  */
    }
		else {
				USART_GetITStatus(usart, USART_IT_RXNE);								// 读SR与DR寄存器来清除其它位置状态如 
				USART_ReceiveData(usart);															  // 如：PE，RXNE，IDLE，ORE，NE，FE，PE
				USART_ClearITPendingBit(usart, USART_IT_RXNE);
		}
}
/*
******************************************************************************
* Function Name  : CONUART_TxHandler
* Description    : 
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
static	void	CONUART_TxHandler (void)
{
}
/*
******************************************************************************
* Function Name  : CONUART_RxHandler
* Description    : 
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
static	void	CONUART_RxHandler (u8	rxData)
{			
		OSTmr_Count_Start(&s_conPort.rxdTimer);
		
		if(s_conPort.rxLen < DEF_CON_BUFSIZE)
		{
				s_conPort.rxdBuf[s_conPort.rxLen++]	 = rxData;
		}
}

/*
******************************************************************************
* Function Name  : HAL_CON_UARTSendCmd
* Description    : 用于对CON设备发送命令(如果命令类型长度为0则只发送$*格式执行)
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_CON_UARTSendCmd (u8 *pBuf,	u16	len)
{
		BSP_PutChar(DEF_CON_UART, pBuf,	len);
}
/*
******************************************************************************
* Function Name  : MB_CRC16
* Description    : CRC16校验计算
* Input          : --pucFrame:指向被校验数组起始位置的指针
*                  --usLen:被校验数组的长度
* Output         : None
* Return         : CRC16校验值
******************************************************************************
*/
static	u16	MB_CRC16 (u8 *pucFrame, u16 usLen)
{
    u16 crc=0x0000,i=0,j=0;
    for(i=0;i<usLen;i++)
    {
        crc = crc ^pucFrame[i]<< 8;
        for (j = 0; j < 8; j++)
        if (crc & 0x8000)
            crc = crc << 1 ^ 0x1021;
        else
            crc = crc << 1; 
    }   
    return __REV16(crc & 0xFFFF);
}

/*
******************************************************************************
* Function Name  : CON_RxdPackProcess
* Description    : 将接收到的数据写入接收邮箱(与OS接口)								 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
static	void	CON_RxdPackProcess (u8 *pBuf,	u16	len)
{
		if(len != 0)		// 去掉首字节为0x00 判断
		{
				if(OSRunning > 0)
				{
						memset((u8 *)&s_rxdData,	0,	sizeof(s_rxdData));
						memcpy((u8 *)s_rxdData.dataBuf,	pBuf,	len);
						s_rxdData.len	=	len;
						OSMboxPost(CONComMbox, (void *)&s_rxdData);	
				}			
		}
		else
				CON_RXBUF_UNLOCK();	// 重新使能接收	
}
/*
*********************************************************************************************************
*                                         HAL_CON_TimerServer()
*
* Description : 1ms 检查一次 (CON接收分包时间服务函数,应在1ms定时器中调用)
* Argument(s) : 
* Return(s)   :	
* Caller(s)   : 
* Note(s)     : none.
*********************************************************************************************************
*/
void	 HAL_CON_TimerServer (void)
{
		if(s_conPort.rxdStep == 0)
		{
				OSTmr_Count_Start(&s_conPort.rxdTimer);
				s_conPort.rxdStep	=1;
		}		
		else if(s_conPort.rxdStep == 1)
		{
				if(OSTmr_Count_Get(&s_conPort.rxdTimer) >= DEF_CON_RXDDELAY_TIMEOUT)	
				{	
						if(s_conPort.rxLen	> 0)
						{
								// 通知解析开始
								s_conPort.rxdStep =2;	
								CON_DISABLE_RES_IT();
								CON_RxdPackProcess(s_conPort.rxdBuf,	s_conPort.rxLen);	// 处理接收到的数据包									
						}
						else
						{
								s_conPort.rxLen	=0;
								s_conPort.rxdStep =0;	
						}
				}				
		}
}
/*
*********************************************************************************************************
*                                         CON_RXBUF_UNLOCK()
*
* Description : 当处理完接收数据时调用
* Argument(s) : 
* Return(s)   :	
* Caller(s)   : 
* Note(s)     : none.
*********************************************************************************************************
*/
static	void  CON_RXBUF_UNLOCK (void)
{
		s_conPort.rxLen		=	0;
		s_conPort.rxdStep	=	0;
		memset(s_conPort.rxdBuf,	'\0',	DEF_CON_BUFSIZE);		
		CON_ENABLE_RES_IT();
}
/*
******************************************************************************
* Function Name  : HAL_CON_Init
* Description    : 需要在配置参数读取完后调用
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_CON_Init (void)
{
		memset((u8 *)&s_conPort,	0,	sizeof(s_conPort));
		memset((u8 *)&s_rxdData,	0,	sizeof(s_rxdData));
    HAL_ModuleInit();
		
		IO_RS232_POW_EN();
		BSP_USART_Init(DEF_CON_UART,	DEF_CON_REMAP,	DEF_CON_BAUT_RATE);	
  
    CON_DEVICE.HardWareSta = DEF_CONHARD_NONE_ERR;	// 设置硬件状态为存在
    CON_DEVICE.InitSta		 = 0u;	                  // 置设备有效标志
	
		// 该处添加外接模块检测函数，根据结果修改CON_DEVICE.HardWareSta及CON_DEVICE.InitSta
#if (DEVICE_CHECK_EN == 1u)
    #if(DEF_INITINFO_OUTPUTEN > 0)
    if(dbgInfoSwt & DBG_INFO_SYS)
    {			
        if(CON_DEVICE.HardWareSta == DEF_CONHARD_NONE_ERR)
            myPrintf("[Init]: CON........(ok)!\r\n");
        else if(CON_DEVICE.HardWareSta == DEF_CONHARD_CHEKING)
            myPrintf("[Init]: CON........(checking)!\r\n");
        else
            myPrintf("[Init]: CON........(error)!\r\n");
    }
    #endif	
#endif
}

/*
******************************************************************************
* Function Name  : void ModuleInit(void)
* Description    : SSES模块初始化函数
* Input          : NONE
*								 : NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
void HAL_ModuleInit(void)
{
		memset((u8 *)&S_stuDatapackInfo, 0u, sizeof(STU_PACK_INFO));
		memset((u8 *)&S_stuModuleSt,     0u, sizeof(STU_MODULE_ST));
    memset((u8 *)&S_carSt,           0u, sizeof(STU_CAR_ST));
    S_uiSendTmr = 0u;
}

/*
******************************************************************************
* Function Name  : u16 SdGet(void)
* Description    : 获取模块序列号
* Input          : NONE
*								 : NONE
* Output         : NONE
* Return         : 模块序列号
******************************************************************************
*/
static __inline u16 SdGet(void)
{
    return (  (s_common.id[PRA_MAXOF_COMMON_ID - 1] << 8u) 
             | s_common.id[PRA_MAXOF_COMMON_ID - 2]);
}

/*
******************************************************************************
* Function Name  : u8 DataPackSend(u8 *pucData, u16 usLen)
* Description    : 数据包发送函数
* Input          : pucData：数据buffer
*								 : usLen  ：数据长度
* Output         : NONE
* Return         : SUCCESS/FAIL
******************************************************************************
*/
static __inline u8 DataPackSend(u8 *pucData, u16 usLen)
{
	  u8  ucRet    = SUCCESS;
  
    HAL_CON_UARTSendCmd(pucData, usLen);

#if(DEF_INITINFO_OUTPUTEN > 0)
    if(dbgInfoSwt & DBG_INFO_CON)
		{
        u8	ucBuf[24]= {0};
        
        Hex2StringArray (pucData,	usLen,	ucBuf);
        myPrintf("\r\n[CON-Txd]: %s\r\n",ucBuf);
    }
    else
    {
    }
#endif
  
    return ucRet;
}

/*
******************************************************************************
* Function Name  : u8 GetHour(void)
* Description    : 获取当前时间的小时
* Input          : NONE
*								 : NONE
* Output         : NONE
* Return         : 小时（最高位为1）
******************************************************************************
*/
static u8 GetHour(void) 
{
    SYS_DATETIME	tmpRtc;
  
    BSP_RTC_Get_Current(&tmpRtc);
		tmpRtc.hour = 0u;                 //test
  
		return (tmpRtc.hour | 0x80);
}

/*
******************************************************************************
* Function Name  : u8 GetMinute(void)
* Description    : 获取当前时间的分钟
* Input          : NONE
*								 : NONE
* Output         : NONE
* Return         : 分钟
******************************************************************************
*/
static u8 GetMinute(void)
{
    SYS_DATETIME	tmpRtc;
  
    BSP_RTC_Get_Current(&tmpRtc);
		tmpRtc.minute = 0u;             //test
  
		return tmpRtc.minute;
}

/*
******************************************************************************
* Function Name  : void PackData(STU_PACK_INFO *pBuf, u8 ucDataType, u8 ucPara1, u8 ucPara2)
* Description    : 打包要发送的数据
* Input          : pBuf：数据buffer
*								 : ucDataType：协议数据类型
*                ：ucPara1：协议参数1
*                ：ucPara2：协议参数2
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static __inline void PackData(STU_PACK_INFO *pBuf, u8 ucDataType, u8 ucPara1, u8 ucPara2)
{
    pBuf->ucSd ++;                                                    //流水号每次发送前加1
    memset((u8 *)&(pBuf->stuPackdata), 0u, sizeof(STU_DATA_PACK));
    pBuf->stuPackdata.usStartFlag                  = __REV16(PACK_HEAD);
    pBuf->stuPackdata.unionAddrSd.stuAddrSd.ucAddr = PACK_ADDR;
    pBuf->stuPackdata.unionAddrSd.stuAddrSd.ucSd   = pBuf->ucSd;
    pBuf->stuPackdata.usSerialNum                  = SdGet();
    pBuf->stuPackdata.ucDataType                   = ucDataType;
    pBuf->stuPackdata.ucPara1                      = ucPara1;
    pBuf->stuPackdata.ucPara2                      = ucPara2;
    pBuf->stuPackdata.usCrc                        = MB_CRC16( (u8 *)&(pBuf->stuPackdata.unionAddrSd.ucAddrAndSd), 
                                                                  PACK_CRC_CHK_DATA_LEN);
}

/*
******************************************************************************
* Function Name  : u8 IsCmdSendEnable(void)
* Description    : 判断当前是否可以发送命令
* Input          : NONE
*                ：NONE
* Output         : NONE
* Return         : 0 ：不可发送
*                ：1 ：可发送
******************************************************************************
*/
static __inline u8 IsCmdSendEnable(void)
{
		return (S_stuModuleSt.ucAckType != PACK_SENDING_ST);
}

/*
******************************************************************************
* Function Name  : u8 CmdSend(u8 ucDataType, u8 ucPara1, u8 ucPara2)
* Description    : 命令发送
* Input          : ucDataType：协议数据类型
*                ：ucPara1：协议参数1
*                ：ucPara2：协议参数2
* Output         : NONE
* Return         : BUSY/SUCCESS/FAIL
******************************************************************************
*/
static u8 CmdSend(u8 ucDataType, u8 ucPara1, u8 ucPara2)
{
		u8   ucRet = BUSY;
		
    if (1u == IsCmdSendEnable())
    {
        if (UNLOCK == S_stuDatapackInfo.ucDataLock)
        {
            S_stuDatapackInfo.ucDataLock = LOCK;
            S_stuModuleSt.ucAckType      = PACK_SENDING_ST;
            PackData(&S_stuDatapackInfo, ucDataType, ucPara1, ucPara2);                 //数据进行协议打包
          
            if (SUCCESS == DataPackSend((u8 *)&S_stuDatapackInfo.stuPackdata, sizeof(STU_DATA_PACK)))
            {
                OSTmr_Count_Start(&S_uiSendTmr);                                        //启动重发延迟定时器
                ucRet = SUCCESS;
            }
            else
            {
                ucRet = FAIL;
            }
						
						if ((PACK_DATA_TYPE_ACK == ucDataType) || (PACK_DATA_TYPE_NAK == ucDataType))
						{
								S_stuDatapackInfo.ucDataLock = UNLOCK;
								S_stuModuleSt.ucAckType      = PACK_REV_ACK;
						}
						else
						{
						}
        }
        else
        {
        }
    }
    else
    {
    }
		
		return ucRet;
}
 
/*
******************************************************************************
* Function Name  : u8 CmdResend(void)
* Description    : 命令重新发送
* Input          : NONE
*                ：NONE
* Output         : NONE
* Return         : SUCCESS/FAIL
******************************************************************************
*/
static u8 CmdResend(void)
{
		u8   ucRet = FAIL;
		
		if (LOCK == S_stuDatapackInfo.ucDataLock)
		{
				if (SUCCESS == DataPackSend((u8 *)&S_stuDatapackInfo.stuPackdata, sizeof(STU_DATA_PACK)))
				{
            OSTmr_Count_Start(&S_uiSendTmr);                    //启动重发延迟定时器
						ucRet = SUCCESS;
				}
				else
				{
						ucRet = FAIL;
				}
		}
		else
		{
		}
		
		return ucRet;
}

/*
******************************************************************************
* Function Name  : void PconReqMsgRelease(void)
* Description    : 释放应用层数据锁
* Input          : NONE
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static __inline void PconReqMsgRelease(void)
{
    CPU_SR         cpu_sr;
  
    if (0u != PconReqMsg)
    {
        if (0u == S_stuModuleSt.ucRdCarStFlag)
        {
            OS_ENTER_CRITICAL();
            PconReqMsg->dataLock = UNLOCK;
            OS_EXIT_CRITICAL();
            PconReqMsg           = 0u;
        }
        else
        {
        }
    }
    else
    {
    }
}

/*
******************************************************************************
* Function Name  : void CmdAckToApp(CAR_ACK_TYPE emAck)
* Description    : 向应用层发送应答
* Input          : emAck：应答类型
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static __inline void CmdAckToApp(CAR_ACK_TYPE emAck)
{
    if (CAR_CMD_READSTA != PconReqMsg->cmdCode)
    {
        ConAckMsg.sn      = PconReqMsg->sn;
        ConAckMsg.retCode = emAck;
        ConAckMsg.cmdCode = PconReqMsg->cmdCode;
      
        PconReqMsgRelease();
      
        if (OS_ERR_NONE == OSMboxPost(OS_CONAckMbox, (void *)&ConAckMsg)) // 向应用层发送应答包 
        {
        }
        else
        {
#if(DEF_INITINFO_OUTPUTEN > 0)
            if(dbgInfoSwt & DBG_INFO_CON)
            {
                myPrintf("\r\n[CON-ERR]:AckMbox Post Fail!!\r\n");
            }
            else
            {
            }
#endif
        }
    }
    else
    {
    }
}

/*
******************************************************************************
* Function Name  : void CmdSendPeriodTask(void)
* Description    : CON模块周期处理函数
* Input          : NONE
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static void CmdSendPeriodTask(void)
{
		if (PACK_SENDING_ST == S_stuModuleSt.ucAckType)
		{
				if (OSTmr_Count_Get(&S_uiSendTmr) >= PACK_TMROUT)               //判断数据是否发送500毫秒还未收到应答
				{
						if (S_stuModuleSt.ucSendFailCnt < PACK_RESEND_TMRS)         //判断未收到应答的次数是否超过4次
						{
								if (SUCCESS == CmdResend())
								{
										S_stuModuleSt.ucSendFailCnt ++;
								}
								else
								{
								}
						}
						else
						{
                S_stuDatapackInfo.ucDataLock = UNLOCK;
								S_stuModuleSt.ucSendFailCnt  = 0u;
								S_stuModuleSt.ucAckType      = PACK_NO_ANSWER;
                CmdAckToApp(CAR_ACK_TIMEOUT);
						}
				}
				else
				{
				}
		}
		else
		{
				S_stuModuleSt.ucSendFailCnt = 0u;
		}
}

/*
******************************************************************************
* Function Name  : void CmdNakToApp(STU_DATA_PACK *pData)
* Description    : 向应用层发送否定应答
* Input          : pData：接收到的数据buffer指针
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static __forceinline void CmdNakToApp(STU_DATA_PACK *pData)
{
    S_stuDatapackInfo.ucDataLock = UNLOCK;
    S_stuModuleSt.ucAckType      = PACK_REV_NAK;
    S_stuModuleSt.ucNakSype      = pData->ucPara1;
#if (1 == IG_ERR_ACK_FILTER_EN) 
    if (   (CAR_CMD_IG_ON == PconReqMsg->cmdCode) 
        || (CAR_CMD_IG_OFF == PconReqMsg->cmdCode))             //当发送点火熄火时返回的为否定应答（为处理外接设备bug）
    {
        if (PACK_REV_NAK_PARA1_3 == S_stuModuleSt.ucNakSype)
        {
            CmdAckToApp(CAR_ACK_SUCCESS);
        }
        else
        {
        }
    }
    else
    {
    }
#endif    
    switch(S_stuModuleSt.ucNakSype)
    {
        case PACK_REV_NAK_PARA1_1:
            CmdAckToApp(CAR_ACK_AUTH);
            break;
        
        case PACK_REV_NAK_PARA1_2:
            CmdAckToApp(CAR_ACK_ILLCMD);
            break;
#if (0 == IG_ERR_ACK_FILTER_EN) 				
				case PACK_REV_NAK_PARA1_3:
						CmdAckToApp(CAR_ACK_ILLMODE);
						break;
#endif        
        case PACK_REV_NAK_PARA1_4:
            CmdAckToApp(CAR_ACK_BRAKE);
            break;
        
        case PACK_REV_NAK_PARA1_5:
            CmdAckToApp(CAR_ACK_HBRAKE);
            break;
        
        default:
            break;
    }
}

/*
******************************************************************************
* Function Name  : void CmdGuardStProcess(STU_DATA_PACK *pData)
* Description    : 防盗状态处理函数
* Input          : pData：数据buffer
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static __forceinline void CmdGuardStProcess(STU_DATA_PACK *pData)
{
    if (    (pData->ucPara1 >= PACK_REV_GUARD_PARA1_1)
         && (pData->ucPara1 <= PACK_REV_GUARD_PARA1_3))               //判断是否为合法参数
    {
        SEND_ACK();
      
        if(PACK_REV_GUARD_PARA1_3 == (pData->ucPara1))
        {
            S_stuModuleSt.ucRunSt = PACK_REV_GUARD_PARA1_3;           //车辆行驶中状态
        }
        else
        {
            S_stuModuleSt.ucGuardSt  = pData->ucPara1;
          
            if (PACK_REV_GUARD_PARA1_1 == S_stuModuleSt.ucGuardSt)
            {
                S_carSt.uiGuardSt = CAR_ST_OPEN;                      //设防
                OSTmr_Count_Start(&S_uiAlarmTmr);
            }
            else
            {
                S_carSt.uiGuardSt = CAR_ST_CLOSE;                     //撤防
            }
        }
    }
    else
    {
        SEND_NAK();
    }
}

/*
******************************************************************************
* Function Name  : void CmdDoorStProcess(STU_DATA_PACK *pData)
* Description    : 车门状态处理函数
* Input          : pData：数据buffer
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static __forceinline void CmdDoorStProcess(STU_DATA_PACK *pData)
{
    if (   (pData->ucPara1 >= PACK_REV_DOOR_PARA1_1)
        && (pData->ucPara1 <= PACK_REV_DOOR_PARA1_2))                          //判断是否为合法参数
    {
        SEND_ACK();
      
        S_stuModuleSt.ucDoorSt = pData->ucPara1;
      
        if (PACK_REV_DOOR_PARA1_2 == S_stuModuleSt.ucDoorSt)
        {
            S_carSt.uiLFDoorSt = CAR_ST_OPEN;                                  //车门未关闭		
          
            if (OSTmr_Count_Get(&S_uiAlarmTmr) >= PACK_ALARM_TMROUT) 
            {
                glob_illOpenFlag	=1;	// 由外部打包报警数据包	
            }
            else
            {
            }
        }
        else
        {
            S_carSt.uiLFDoorSt = CAR_ST_CLOSE;                                 //所有车门已关闭
        }
    }
    else
    {
        SEND_NAK();
    }
}

/*
******************************************************************************
* Function Name  : void CmdEngineStProcess(STU_DATA_PACK *pData)
* Description    : 引擎状态处理函数
* Input          : pData：数据buffer
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static __forceinline void CmdEngineStProcess(STU_DATA_PACK *pData)
{
    if (   (pData->ucPara1 >= PACK_REV_ENGINE_PARA1_1)
        && (pData->ucPara1 <= PACK_REV_ENGINE_PARA1_5))                             //判断是否为合法参数
    {
        SEND_ACK();
      
        if (   (PACK_REV_ENGINE_PARA1_4 == pData->ucPara1)
            || (PACK_REV_ENGINE_PARA1_1 == pData->ucPara1)
            || (PACK_REV_ENGINE_PARA1_3 == pData->ucPara1))
        {
            S_stuModuleSt.ucEngienSt = pData->ucPara1;
            
            if (PACK_REV_ENGINE_PARA1_4 == S_stuModuleSt.ucEngienSt)
            {
                S_carSt.uiEngienSt = CAR_ST_OPEN;                                   //引擎打开
            }
            else if (PACK_REV_ENGINE_PARA1_1 == S_stuModuleSt.ucEngienSt)
            {
                S_carSt.uiEngienSt = CAR_ST_CLOSE;                                  //引擎关闭
            }
            else
            {
                S_carSt.uiEngienSt = 3u;                                            //ON
            }
        }
        else
        {
        }
    }
    else
    {
        SEND_NAK();
    }
}

/*
******************************************************************************
* Function Name  : void CmdRecv(u8 *pucData, u16 usLen)
* Description    :命令处理函数
* Input          : pucData：命令buffer
*                ：usLen：命令长度
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static void CmdRecv(u8 *pucData, u16 usLen)
{
		if (PACK_LEN == usLen)
		{
				if ((u16)((pucData[0u] << 8) | pucData[1u]) == PACK_HEAD) 
				{
						S_stuModuleSt.ucValid = 1u;
          
            if (((STU_DATA_PACK *)pucData)->usCrc == MB_CRC16(&pucData[2u], PACK_CRC_CHK_DATA_LEN))
            {
                switch(((STU_DATA_PACK *)pucData)->ucDataType)
                {
                    case PACK_DATA_TYPE_ACK:                            //ACK
                        S_stuDatapackInfo.ucDataLock = UNLOCK;
                        S_stuModuleSt.ucAckType      = PACK_REV_ACK;
                        CmdAckToApp(CAR_ACK_SUCCESS);                   //向应用层发送肯定应答
                        break;
                    
                    case PACK_DATA_TYPE_NAK:									          //NAK
                        CmdNakToApp((STU_DATA_PACK *)pucData);          //向应用层发送否定应答          
                        break;
                    
                    case PACK_DATA_TYPE_GUARD:                          //系统
                        CmdGuardStProcess((STU_DATA_PACK *)pucData);    //向应用层发送系统状态数据
                        break;
                        
                    case PACK_DATA_TYPE_DOOR:									          //车门后备箱
                        CmdDoorStProcess((STU_DATA_PACK *)pucData);     //向应用层发送门状态数据
                        break;
                    
                    case PACK_DATA_TYPE_ENGINE:                         //引擎
                        CmdEngineStProcess((STU_DATA_PACK *)pucData);   //向应用层发送引擎状态数据
                        break;
                    
                    default:
                        SEND_NAK();
#if(DEF_INITINFO_OUTPUTEN > 0)
                        if(dbgInfoSwt & DBG_INFO_CON)
                        {
                            myPrintf("\r\n[CON-ERR-DATATYPE]: %02X\r\n",((STU_DATA_PACK *)pucData)->ucDataType);
                        }
                        else
                        {
                        }
#endif
                        break;
                }
            }
            else
            {
#if(DEF_INITINFO_OUTPUTEN > 0)
                if(dbgInfoSwt & DBG_INFO_CON)
                {
                    myPrintf( "\r\n[CON-ERR-CRC]: Src_%04X, Clc_%04X\r\n",
                              ((STU_DATA_PACK *)pucData)->usCrc, 
                              MB_CRC16(&pucData[2u], PACK_CRC_CHK_DATA_LEN));
                }
                else
                {
                }
#endif
            }
				}
				else
				{
#if(DEF_INITINFO_OUTPUTEN > 0)
            if(dbgInfoSwt & DBG_INFO_CON)
            {
                myPrintf("\r\n[CON-ERR-HEAD]: %04X\r\n",(u16)((pucData[0u] << 8) | pucData[1u]));
            }
            else
            {
            }
#endif
				}
		}
		else
		{	
#if(DEF_INITINFO_OUTPUTEN > 0)
        if(dbgInfoSwt & DBG_INFO_CON)
        {
            myPrintf("\r\n[CON-ERR-LEN]: %02X\r\n",usLen);
        }
        else
        {
        }
#endif
		}   
}

/*
******************************************************************************
* Function Name  : void CmdRecvProcess(void)
* Description    :命令接收函数
* Input          : NONE
*                ：NONE
* Output          : NONE
* Return         : NONE
******************************************************************************
*/
static void CmdRecvProcess(void)
{
    u8 ucBuf[24] = {0};
    u8 ucLen     = 0u;
    CONComm_TypeDef  *pconMsg  = (CONComm_TypeDef *)(OSMboxAccept(CONComMbox));
  
    if(pconMsg != (void *)0)
    {
#if(DEF_INITINFO_OUTPUTEN > 0)
        if(dbgInfoSwt & DBG_INFO_CON)
        {
            Hex2StringArray (pconMsg->dataBuf,	(pconMsg->len > 10 ? 10 : pconMsg->len),	ucBuf);
            myPrintf("\r\n[CON-Rxd]: %s\r\n",ucBuf);
        }
        else
        {
        }
#endif
				
        memcpy(ucBuf, pconMsg->dataBuf, (pconMsg->len > 10 ? 10 : pconMsg->len));
        ucLen = pconMsg->len;
        CON_RXBUF_UNLOCK();
        
        CmdRecv(ucBuf, ucLen);        //处理来自串口的指令
    }
    else
    {
    }
}

/*
******************************************************************************
* Function Name  : void CmdSendProcess(void)
* Description    : 解析并发送来自GPRS的命令
* Input          : NONE
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static void CmdSendProcess(void)
{
    CONReqMbox_TypeDef  *pconReqMsg = (CONReqMbox_TypeDef *)(OSMboxAccept(OS_CONReqMbox));
  
    if(pconReqMsg != (void *)0)
    {
        if (0u == PconReqMsg)
        {
            PconReqMsg = pconReqMsg;
          
            switch((u8)(PconReqMsg->cmdCode))
            {
                case (u8)CAR_CMD_IG_ON:
                    SEND_ENGINE_START();                  //启动引擎
                    break;
                
                case (u8)CAR_CMD_IG_OFF:
                    SEND_ENGINE_SHUT();                   //关闭引擎
                    break;
                
                case (u8)CAR_CMD_ARM_SET:
                    SEND_SET_GUARD();                     //设防
                    break;
                
                case (u8)CAR_CMD_ARM_RST:
                    SEND_CLR_GUARD();                     //撤防
                    break;
                
                case (u8)CAR_CMD_FIND_CAR:
                    SEND_SEARCH_CAR();                    //寻车
                    break;
                
                case (u8)CAR_CMD_TRUNK:
                    SEND_OPEN_TRUNK();                    //打开后备箱
                    break;
                
                case (u8)CAR_CMD_READSTA:                 //车辆状态读取
#if (RDST_RST_CAR_ST_EN == 1)
                    memset((u8 *)&S_carSt, 0u, sizeof(STU_CAR_ST));
                    S_stuModuleSt.ucGuardSt     = 0u;
                    S_stuModuleSt.ucDoorSt      = 0u;
                    S_stuModuleSt.ucRunSt       = 0u;
                    S_stuModuleSt.ucEngienSt    = 0u;
#endif
                    S_stuModuleSt.ucRdCarStFlag = 1u;
                    break;
                
                default:
                    CmdAckToApp(CAR_ACK_UNSUPS);          //不支持该指令
                    break;
            }
        }
        else
        {
            (void)OSMboxPost(OS_CONReqMbox, (void *)pconReqMsg);
#if(DEF_INITINFO_OUTPUTEN > 0)
            if(dbgInfoSwt & DBG_INFO_CON)
            {
                myPrintf("\r\n[CON-WARNNING]:Last ReqMbox is processing!!\r\n");
            }
            else
            {
            }
#endif
        }
    }
    else
    {
    }
}

/*
******************************************************************************
* Function Name  : void ConHardwareCheck(void)
* Description    : 检测外围硬件是否存在
* Input          : NONE
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
#if (DEVICE_CHECK_EN == 1u)
static void ConHardwareCheck(void)
{
    static u8 s_ucFirstFlag = 0u;
    CPU_SR         cpu_sr;
  
    if (0u == s_ucFirstFlag)
    {
        if      (   (PACK_REV_ACK  == S_stuModuleSt.ucAckType) 
                 || (PACK_REV_NAK  == S_stuModuleSt.ucAckType))
        {
            OS_ENTER_CRITICAL();
            CON_DEVICE.HardWareSta = DEF_CONHARD_NONE_ERR;	// 设置硬件状态为存在
            CON_DEVICE.InitSta		 = 1u;	                  // 置设备有效标志
            OS_EXIT_CRITICAL();
            s_ucFirstFlag          = 1u;
        }
        else if (PACK_NO_ANSWER == S_stuModuleSt.ucAckType)
        {
            OS_ENTER_CRITICAL();
            CON_DEVICE.HardWareSta = DEF_CONHARD_COMMU_ERR;	// 设置硬件状态为不存在
            CON_DEVICE.InitSta		 = 0u;	                  // 置设备无效标志
            OS_EXIT_CRITICAL();
            s_ucFirstFlag          = 1u;
        }
        else
        {
        }
        
        if (1u == s_ucFirstFlag)
        {
#if(DEF_INITINFO_OUTPUTEN > 0)
            if(dbgInfoSwt & DBG_INFO_SYS)
            {			
                if(CON_DEVICE.HardWareSta == DEF_CONHARD_NONE_ERR)
                    myPrintf("[Init]: CON........(ok)!\r\n");
                else if(CON_DEVICE.HardWareSta == DEF_CONHARD_CHEKING)
                    myPrintf("[Init]: CON........(checking)!\r\n");
                else
                    myPrintf("[Init]: CON........(error)!\r\n");
            }
#endif	
        }
        else
        {
        }
    }
    else
    {
        if      (PACK_SENDING_ST == S_stuModuleSt.ucAckType)
        {
        }
        else if (PACK_NO_ANSWER  == S_stuModuleSt.ucAckType)
        {
            OS_ENTER_CRITICAL();
            s_err.conTimes ++;
            OS_EXIT_CRITICAL();
        }
        else
        {
            OS_ENTER_CRITICAL();
            s_err.conTimes = 0u;
            CON_DEVICE.HardWareSta = DEF_CONHARD_NONE_ERR;	// 设置硬件状态为存在
            CON_DEVICE.InitSta		 = 1u;	                  // 置设备有效标志
            OS_EXIT_CRITICAL();
        }
    }
}

#else
static void ConHardwareCheck(void)
{
    CPU_SR         cpu_sr;
  
    if (PACK_NO_ANSWER  == S_stuModuleSt.ucAckType)
    {
        if (1u == S_stuModuleSt.ucValid)
        {
            S_stuModuleSt.ucValid = 0u;
            OS_ENTER_CRITICAL();
            CON_DEVICE.InitSta    = 0u;
            if (s_err.conTimes < 0xFFu)
            {
                s_err.conTimes ++;
            }
            else
            {
            }
            OS_EXIT_CRITICAL();
        }
        else
        {
        }
    }
    else
    {
        if ((0u == CON_DEVICE.InitSta) && (1u == S_stuModuleSt.ucValid))
        {
            OS_ENTER_CRITICAL();
            CON_DEVICE.InitSta  = 1u;
            OS_EXIT_CRITICAL();
        }
        else
        {
        }
    }
}
#endif

/*
******************************************************************************
* Function Name  : void CarStAckPro(u8 *pucStep, u8 ucStep)
* Description    : 获取车辆状态响应处理
* Input          : pucStep：获取车辆状态的阶段
*                ：ucStep ：阶段
*                : pucSended:命令发送标识
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static __forceinline void CarStAckPro(u8 *pucStep, u8 ucStep, u8 *pucSended)
{
    if (PACK_SENDING_ST == S_stuModuleSt.ucAckType)
    {
    }
    else if (PACK_REV_ACK == S_stuModuleSt.ucAckType)
    {
        *pucStep   = ucStep;
        *pucSended = 0u;
    }
    else
    {
        if ((PACK_NO_ANSWER == S_stuModuleSt.ucAckType) || (PACK_REV_NAK == S_stuModuleSt.ucAckType))
        {
            ucStep = 0u;
        }
        else
        {
        }
    }
    
    if ((PACK_SENDING_ST != S_stuModuleSt.ucAckType) && (0u == ucStep))
    {
        S_stuModuleSt.ucRdCarStFlag = 0u;
        *pucStep                    = 0u;
        *pucSended                  = 0u;
        PconReqMsgRelease();
    }
    else
    {
    }
}

/*
******************************************************************************
* Function Name  : void CarStGet(void)
* Description    : 执行获取车辆状态命令
* Input          : NONE
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
static void CarStGet(void)
{
    static u8 s_ucRdCarStStep = 0u;
    static u8 s_ucSendedFlag  = 0u;
		static u32 s_uiTmr        = 0u;
  
    if (1u == S_stuModuleSt.ucRdCarStFlag)
    {
        if (0u == s_ucRdCarStStep)
        {
            if (0u == s_ucSendedFlag)
            {
                SEND_GET_GUARD_ST();
                s_ucSendedFlag = 1u;
								OSTmr_Count_Start(&s_uiTmr);
            }
            else
            {
            }
            CarStAckPro(&s_ucRdCarStStep, 1u, &s_ucSendedFlag);
        }
        else if (1u == s_ucRdCarStStep)
        {
            if ((0u == s_ucSendedFlag) &&(OSTmr_Count_Get(&s_uiTmr) > 2500))
            {
                SEND_GET_DOOR_ST();
                s_ucSendedFlag = 1u;
								OSTmr_Count_Start(&s_uiTmr);
            }
            else
            {
            }
						if (s_ucSendedFlag)
						{
								CarStAckPro(&s_ucRdCarStStep, 2u, &s_ucSendedFlag);
						}
        }
        else if (2u == s_ucRdCarStStep)
        {
            if ((0u == s_ucSendedFlag) &&(OSTmr_Count_Get(&s_uiTmr) > 2500))
            {
                SEND_ENGINE_ST_GET();
                s_ucSendedFlag = 1u;
            }
            else
            {
            }
						if (s_ucSendedFlag)
						{
								CarStAckPro(&s_ucRdCarStStep, 0u, &s_ucSendedFlag);
						}
        }
        else
        {
        }
    }
    else
    {
#if(DEF_INITINFO_OUTPUTEN > 0)
        if(dbgInfoSwt & DBG_INFO_CON)
        {
            static STU_CAR_ST s_stuLastCarSt;
            
            if (*(u32 *)&S_carSt != *(u32 *)&s_stuLastCarSt)
            {
                s_stuLastCarSt = S_carSt;
                myPrintf("\r\n[CON-CAR-ST]: %08X!!\r\n", *(u32 *)&S_carSt);
            }
            else
            {
            }
        }
        else
        {
        }
#endif
    }
}

/*
*********************************************************************************************************
*                                      EXTERNAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
******************************************************************************
* Function Name  : void HAL_CONMainLoop(void)
* Description    : 对外调用总函数，处理数据接收和发送
* Input          : NONE
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
void HAL_CONMainLoop(void)
{
#if (DEVICE_CHECK_EN == 1u)
    static u8 s_ucCheckHardware = 0u;
  
    if (0u == s_ucCheckHardware)            //第一次运行检测硬件
    {
        SEND_GET_GUARD_ST();                  //发送状态检测指令
        s_ucCheckHardware = 1u;
    }
    else
    {
    }
#endif   
  
    CmdSendProcess   ();                    //接收应用层命令并发送指令
    CarStGet         ();                    //获取车辆状态
    CmdSendPeriodTask();                    //指令重发
    CmdRecvProcess   ();                    //指令接受处理
    ConHardwareCheck ();                    //硬件检测
}

/*
******************************************************************************
* Function Name  : STU_SYS_ST HAL_SYSStGet(void)
* Description    : 获取车辆状态
* Input          : NONE
*                ：NONE
* Output         : NONE
* Return         : NONE
******************************************************************************
*/
STU_CAR_ST HAL_CarStGet(void)
{
    return S_carSt;
}

/*
******************************************************************************
* Function Name  : u8 HAL_ConTest(void)
* Description    : 串口5测试
* Input          : NONE
*                : NONE
* Output         : NONE
* Return         : 1:成功
*                : 0:失败
******************************************************************************
*/
u8 HAL_ConTest(void)
{
    u16 u8counter = 100;
    u8  u8counter0 = 0;
    u8  ucRet = 0u;

    USART_Cmd(UART5, ENABLE );
    USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);
    USART_SendData(UART5, 0XAA );

    while( USART_GetFlagStatus( UART5, USART_FLAG_TXE ) == RESET );

    while( --u8counter )
    {
        if( USART_GetFlagStatus( UART5, USART_FLAG_RXNE ) != RESET )
        {
            u8counter0 = ( u8 )( USART_ReceiveData( UART5 ) );

            if( u8counter0 == 0xAA )
            {
                ucRet = 1;
                break ;
            }
            else
            {
                break;
            }
        }
        OSTimeDly( 10 );
    }
		USART_ITConfig( UART5, USART_IT_RXNE, ENABLE );
    
    return ucRet;
}


/*****************************************end*********************************************************/



