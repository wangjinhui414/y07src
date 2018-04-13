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
*                                     	 hal_ProcessDBG_c.c
*                                           with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessDBG_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  HAL_PROCESSDBG_GLOBLAS

#include	<string.h>
#include	<stdio.h>
#include  <stdlib.h>
#include	"cfg_h.h"
#include 	"ucos_ii.h"
#include	"bsp_h.h"
#include 	"bsp_gsm_h.h"
#include 	"bsp_gps_h.h"
#include	"bsp_rtc_h.h"
#include	"bsp_storage_h.h"
#include	"bsp_can_h.h"
#include	"bsp_kline_h.h"
#include	"hal_h.h"
#include 	"hal_protocol_h.h"
#include	"hal_processDBG_h.h"
#include	"hal_processSYS_h.h"
#include	"hal_processQUE_h.h"
#include	"hal_processGPS_h.h"
#include	"hal_processCON_h.h"
#include	"main_h.h"

#ifdef DEF_OBDTASK_CREATE
#include  "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
#endif



/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/* Enable receive interrupt */
#define	DBG_ENABLE_RES_IT()					(BSP_RS232_RxIntEn	(DEF_DBG_UART))		// include at bsp.h
#define	DBG_DISABLE_RES_IT()				(BSP_RS232_RxIntDis	(DEF_DBG_UART))		// include at bsp.h

/* Enable send interrupt */
#define	DBG_ENABLE_TXD_IT()					(BSP_RS232_TxIntEn	(DEF_DBG_UART))		// include at bsp.h
#define	DBG_DISABLE_TXD_IT()				(BSP_RS232_TxIntDis	(DEF_DBG_UART))		// include at bsp.h


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

extern			OS_EVENT							*DBGComSem; 	// DBG接口命令处理信号量
extern			SendData_TypeDef			tmpSms;				// 用于发送短信临时发送区 

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static			void	DBGRS232_TxHandler 		(void);
static			void	DBGRS232_RxHandler 		(u8	rxData);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/
				
				
/*
*********************************************************************************************************
* Function Name  : HAL_BSRES_IT_CON
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	HAL_DBGRES_IT_CON(u8	newSta)
{
		if(newSta	==	0)
				DBG_DISABLE_RES_IT();
		else
				DBG_ENABLE_RES_IT();						
}
/*
*********************************************************************************************************
* Function Name  : HAL_DBGTXD_IT_CON
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	HAL_DBGTXD_IT_CON(u8	newSta)
{
		if(newSta	==	0)
				DBG_DISABLE_TXD_IT();
		else
				DBG_ENABLE_TXD_IT();						
}


/*
******************************************************************************
* Function Name  : HAL_DBGUART_RxTxISRHandler
* Description    : 
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_DBGUART_RxTxISRHandler (void)
{
    USART_TypeDef  *usart;
		u8     rx_data;

#if   (DEF_DBG_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_DBG_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_DBG_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_DBG_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_DBG_UART == DEF_UART_5)
		usart = UART5;

#else
		return;
#endif

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
        rx_data = USART_ReceiveData(usart) & 0xFF;              /* Read one byte from the receive data register         */
        DBGRS232_RxHandler(rx_data);

        USART_ClearITPendingBit(usart, USART_IT_RXNE);          /* Clear the DEF_GPS_UART Receive interrupt                   */
    }
		else if (USART_GetITStatus(usart, USART_IT_TXE) != RESET){
        DBGRS232_TxHandler();

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
* Function Name  : BSTTL_TxHandler
* Description    : 
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
static	void	DBGRS232_TxHandler (void)
{
		;	// 发送中断不做处理
}
/*
******************************************************************************
* Function Name  : DBGRS232_RxHandler
* Description    : 
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
static	void	DBGRS232_RxHandler (u8	rxData)
{
		if(s_dbgPort.rxFlag == 0)
		{		
				if(rxData == DBG_CMD_HEAD_START)
				{					
						s_dbgPort.rxFlag	=1;
						s_dbgPort.rxlen		=0;		
				}
		}
		if(s_dbgPort.rxFlag == 1)
		{
				if(s_dbgPort.rxlen < DEF_DBG_BUFSIZE)
				{
						s_dbgPort.rxdBuf[s_dbgPort.rxlen++]	=	rxData;				
						if((rxData == '\r') || (rxData == '\n'))
						{
								// 正确接收一个命令包	
								HAL_DBGRES_IT_CON(DEF_DISABLE);
								s_dbgPort.rxFlag	=2;
								// 处理命令包
								OSSemPost(DBGComSem);											
						}
				}	
				else
				{
						s_dbgPort.rxFlag	=0;
						s_dbgPort.rxlen		=0;
						memset(s_dbgPort.rxdBuf,	'\0',	DEF_DBG_BUFSIZE);
				}	
		}		
}

/*
******************************************************************************
* Function Name  : PrintCommon
* Description    : 打印COMMON配置
*								 : 
* Input          : pucBuf：数据buffer
* Output         : None
* Return         : None
******************************************************************************
*/
void PrintCommon(u8 *pucBuf)
{
    myPrintf("\r\n[savedMark] =%04X",	s_common.savedMark);
    myPrintf("\r\n[sum]       =%04X",	s_common.sum);
    myPrintf("\r\n[iapFlag]   =%04X",	s_common.iapFlag);
    myPrintf("\r\n[iapSta]    =%04X",	s_common.iapSta);
    Hex2StringArray (s_common.id,	sizeof(s_common.id),	pucBuf);
    myPrintf("\r\n[id]        =%s",	pucBuf);	
    myPrintf("\r\n[carInfo]   ='%c',%02d,%02d",	s_common.carInfo[0],s_common.carInfo[1],s_common.carInfo[2]);						
    myPrintf("\r\n[aadc]      =%d",	s_common.aadc);
    Hex2StringArray (s_common.swdg,	sizeof(s_common.swdg),	pucBuf);
    myPrintf("\r\n[swdg]      =%s",	pucBuf);
    myPrintf("\r\n[hverb]     =\"%s\"",	s_common.hverb);	
    myPrintf("\r\n[gpsDis*10] =%d",	(u32)(s_common.gpsDis*10));							
    myPrintf("\r\n[Apn]       =\"%s\"",	s_common.otapApn);
    myPrintf("\r\n[ApnUser]   =\"%s\"",	s_common.otapApnUser);
    myPrintf("\r\n[ApnPass]   =\"%s\"",	s_common.otapApnPass);
    myPrintf("\r\n[Ip]        =\"%s\"",	s_common.otapIp);
    myPrintf("\r\n[Port]      =\"%s\"",	s_common.otapPort);
//								myPrintf("\r\n[FtpUser]   =\"%s\"",	s_common.otapFtpUser);
//								myPrintf("\r\n[FtpPass]   =\"%s\"",	s_common.otapFtpPass);
//								myPrintf("\r\n[FtpFile]   =\"%s\"",	s_common.otapFtpFile);
    myPrintf("\r\n[fileSize]  =%d",	s_common.otapFileSize);
    myPrintf("\r\n[errCode]   =%02X",	s_common.otapErrCode);
    myPrintf("\r\n");	
}

/*
******************************************************************************
* Function Name  : PrintSys
* Description    : 打印Sys配置
*								 : 
* Input          : pucBuf：数据buffer
* Output         : None
* Return         : None
******************************************************************************
*/
static void PrintSys(u8 *pucBuf)
{
    myPrintf("\r\n[savedMark] =%04X",	s_cfg.savedMark);
    myPrintf("\r\n[sum]       =%04X",	s_cfg.sum);
    myPrintf("\r\n[apn]       =\"%s\"",	s_cfg.apn);
    myPrintf("\r\n[apnUser]   =\"%s\"",	s_cfg.apnUser);
    myPrintf("\r\n[apnPass]   =\"%s\"",	s_cfg.apnPass);
    myPrintf("\r\n[dnsIp]     =\"%s\"",	s_cfg.dnsIp);
    myPrintf("\r\n[domain1]   =\"%s\"",	s_cfg.domain1);
    myPrintf("\r\n[domain2]   =\"%s\"",	s_cfg.domain2);
    myPrintf("\r\n[ip1-navi]  =\"%s\"",	s_cfg.ip1);
    myPrintf("\r\n[port1-navi]=\"%s\"",	s_cfg.port1);
    myPrintf("\r\n[ip2-app]   =\"%s\"",	s_cfg.ip2);
    myPrintf("\r\n[port2-app] =\"%s\"",	s_cfg.port2);
    myPrintf("\r\n[domainEn]  =%d",	s_cfg.domainEn);
    myPrintf("\r\n[gpsCollect]=%d",	s_cfg.gpsCollectTime);
    myPrintf("\r\n[onRptTime] =%d",	s_cfg.onReportTime);
    myPrintf("\r\n[offRptTime]=%d",	s_cfg.offReportTime);
    myPrintf("\r\n[heartBTime]=%d",	s_cfg.heartbeatTime);
    myPrintf("\r\n[ackTimeout]=%d",	s_cfg.tcpAckTimeout);
    myPrintf("\r\n[healthRptT]=%d",	s_cfg.healthReportTime);								
    myPrintf("\r\n[smsCenter] =\"%s\"",	s_cfg.smsCenter);							
    myPrintf("\r\n[eventSwt]  =%08X",	s_cfg.eventSwt);
    myPrintf("\r\n[overSpeed] =%d",	s_cfg.overSpeed);
    myPrintf("\r\n[overSpeedT]=%d",	s_cfg.overSpeedTime);
    myPrintf("\r\n[tiredDirT] =%d",	s_cfg.tiredDirveTime);							
    myPrintf("\r\n[pho1]      =\"%s\"",	s_cfg.pho1);
    myPrintf("\r\n[pho2]      =\"%s\"",	s_cfg.pho2);
    myPrintf("\r\n[pho3]      =\"%s\"",	s_cfg.pho3);
    myPrintf("\r\n[pho4]      =\"%s\"",	s_cfg.pho4);
    myPrintf("\r\n[pho5]      =\"%s\"",	s_cfg.pho5);
    myPrintf("\r\n[pho6]      =\"%s\"",	s_cfg.pho6);
    Hex2StringArray (s_cfg.aeskey,	sizeof(s_cfg.aeskey),	pucBuf);
    myPrintf("\r\n[aesKey]    =%s",	pucBuf);		
    myPrintf("\r\n[aesType]   =%d",	s_cfg.aesType);	
    myPrintf("\r\n[devEn]     =%02X",	s_cfg.devEn);	
    myPrintf("\r\n[wMode]     =%d",	s_cfg.wMode);	
    myPrintf("\r\n[nationCode]=\"%s\"",	s_cfg.nationCode);							
    myPrintf("\r\n[sleepSwt]  =%02X",	s_cfg.sleepSwt);	
    myPrintf("\r\n[dssATH]    =%d",	s_cfg.dssATH);	
    myPrintf("\r\n[dssDTH]    =%d",	s_cfg.dssDTH);	
    myPrintf("\r\n[dssRTH]    =%d",	s_cfg.dssRTH);	
    myPrintf("\r\n[dssPTH]    =%d",	s_cfg.dssPTH);	
    myPrintf("\r\n[accMT]     =%d",	s_cfg.accMT);	
    myPrintf("\r\n[accMD]     =%d",	s_cfg.accMD);								
    myPrintf("\r\n[sysMove]   =%d",	s_cfg.sysMove);	
    myPrintf("\r\n[sysMoveT]  =%d",	s_cfg.sysMoveT);	
    myPrintf("\r\n[lowPower]  =%d",	s_cfg.lowPower);	
    myPrintf("\r\n[tjDelayT]  =%d",	s_cfg.tjDelayTime);	
    myPrintf("\r\n[igOffDelay]=%d",	s_cfg.igOffDelay);	
//								myPrintf("\r\n[naviStep]  =%d",	s_cfg.naviStep);	// 不使用
    myPrintf("\r\n[sleepTime] =%d",	s_cfg.globSleepTime);
    myPrintf("\r\n[sound1]    =\"%s\"",	s_cfg.sound1);
    myPrintf("\r\n[sound2]    =\"%s\"",	s_cfg.sound2);
    myPrintf("\r\n");	
}
/*
******************************************************************************
* Function Name  : HAL_DBGCmdProcess
* Description    : 
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_DBGCmdProcess (void)
{
		s8	ret=0;
		u8	tmpCode[DBG_CMD_CODE_SIZE]="",tmpPara[DBG_CMD_PARA_SIZE]="";
		u16	codeLen=0,paraLen=0,u16TmpLen=0,u16i=0,u16p=0,u16m=0;
		u8	u8Tmp=0,tmpBuf[100]="",tmpBuf1[50]="",tmpBuf2[50]="",flag0=0,flag1=0,flag2=0,flag3=0,flag4=0;
		u32	filename = FLASH_CPU_APPNAME_START,	u32Tmp=0;
		SYS_DATETIME tmpRtc;
		
		memset(tmpCode,	'\0',	sizeof(tmpCode));
		memset(tmpPara,	'\0',	sizeof(tmpPara));		
		memset((u8 *)&tmpRtc,	0,	sizeof(tmpRtc));
		memset(tmpBuf,	'\0',	sizeof(tmpBuf));
		memset(tmpBuf1,	'\0',	sizeof(tmpBuf1));
		memset(tmpBuf2,	'\0',	sizeof(tmpBuf2));

		if(s_dbgPort.rxdBuf[0] == DBG_CMD_HEAD_START)
		{
				// 复制命令头
				for(codeLen=0;	((s_dbgPort.rxdBuf[codeLen+1] != ':') && (s_dbgPort.rxdBuf[codeLen+1] != '\r') && (s_dbgPort.rxdBuf[codeLen+1] != '\n') && (codeLen < DBG_CMD_CODE_SIZE) && ((codeLen+1) < s_dbgPort.rxlen));	codeLen++)
				{
						tmpCode[codeLen] = s_dbgPort.rxdBuf[codeLen+1];		
				}

				// 复制参数区
				for(paraLen=0;	((s_dbgPort.rxdBuf[paraLen+codeLen+2] != '\r') && (s_dbgPort.rxdBuf[paraLen+codeLen+2] != '\n') && (paraLen < DBG_CMD_PARA_SIZE) && ((paraLen+codeLen+2) < s_dbgPort.rxlen));	paraLen++)
				{
						tmpPara[paraLen] = s_dbgPort.rxdBuf[paraLen+codeLen+2];		
				}		
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 复位系统 ////////////////////////////////////////////////
				if(strcmp((char *)DBG_CMD_CODE_RESET,	(char *)tmpCode) == 0)
				{						
						printf("\r\nC_RESET=(OK!)\r\n");
						HAL_CPU_ResetPro();
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 复位系统参数 ////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETDFT,	(char *)tmpCode) == 0)	
				{
						// common参数复位
						HAL_LoadParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));
						HAL_ParaSetDefault (DEF_TYPE_COMMON);		// 参数复位
						HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));	
						
						// syscfg参数复位
						HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg));
						HAL_ParaSetDefault (DEF_TYPE_SYSCFG);		// 参数复位
						HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg));
						
						// sysobd参数复位
						HAL_ParaSetDefault (DEF_TYPE_SYSOBD);		// 参数复位
					
						printf("\r\nS_DFT=(OK!)\r\n");		
				}
        else if(strcmp((char *)DBG_CMD_CODE_QCOMONEXT,	(char *)tmpCode) == 0)
				{
            myPrintf("\r\nQ_COMONEXT=");	
            PrintCommon(tmpBuf);				
				}
        else if(strcmp((char *)DBG_CMD_CODE_QSYSCFGEXT,	(char *)tmpCode) == 0)
				{
            myPrintf("\r\nQ_SYSCFGEXT=");	
            PrintSys(tmpBuf);			
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 读取系统通用参数 ////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_QCOMON,	(char *)tmpCode) == 0)
				{
						if(HAL_LoadParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
						{
              
								myPrintf("\r\nQ_COMON=");	
								PrintCommon(tmpBuf);	
						}
						else
								myPrintf("\r\nQ_COMON=[Error-Read!]\r\n");			
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 读取系统应用参数 ////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_QSYSCFG,	(char *)tmpCode) == 0)
				{
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
						{
								myPrintf("\r\nQ_SYSCFG=");	
								PrintSys(tmpBuf);
						}
						else
								myPrintf("\r\nQ_SYSCFG=[Error-Read!]\r\n");			
				}
        else if(strcmp((char *)DBG_CMD_CODE_QBAK,	(char *)tmpCode) == 0)
				{
            if (*((u16 *)FLASH_CPU_APPSIZE_START + 0u) == DEF_DFT_COMMON_SAVED_MARK)
            {
                memcpy((u8 *)&s_common, (u8 *)FLASH_CPU_APPSIZE_START, sizeof(s_common));
          
                myPrintf("\r\nQ_COMBAK=");	
                PrintCommon(tmpBuf);
                
                HAL_CommonParaReload();
	
            }
            else if (*((u16 *)FLASH_CPU_APPSIZE_START + 0u) == DEF_DFT_SYSCFG_SAVED_MARK)
            {
                memcpy((u8 *)&s_cfg, (u8 *)FLASH_CPU_APPSIZE_START, sizeof(s_cfg));
          
                myPrintf("\r\nQ_SYSBAK=");	
                PrintSys(tmpBuf);
                
                HAL_SysParaReload();
            }
            else
            {
                myPrintf("\r\nQ_BAK=[EMPTY!]\r\n");
            }
        }
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 读取OBD应用参数 ////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_QSYSOBD,	(char *)tmpCode) == 0)
				{
						#ifdef DEF_OBDTASK_CREATE
						myPrintf("\r\nQ_SYSOBD=%d\r\n",	g_stObdPar.u32OdoMeter);
						#endif
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 读取通信系统版本号(网络升级版本号+通信部分版本号) ////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_QVERB,	(char *)tmpCode) == 0)
				{				
						myPrintf("\r\nQ_VERB=Net :");						
						for(u16i=0;	u16i<5;	u16i++)
						{
								BSP_OSPutChar (DEF_DBG_UART,	(u8*)(filename + u16i),	1);
						}
						myPrintf("\r\n       Main:%s",	DEF_SOFTWARE_VER);					// 通信部分版本号
						#ifdef DEF_OBDTASK_CREATE
						myPrintf("\r\n       OBD :%s\r\n",	DEF_OBD_VERSION);				// OBD版本号
						#endif
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置网络版本号 ////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SVERB,	(char *)tmpCode) == 0)
				{				
						// 设置网络版本号(格式为XX.XX,允许版本号为空)	
						if((paraLen == 0) || (paraLen == 5))
						{		
								memcpy(tmpBuf,tmpPara,paraLen);
								if(BSP_CPU_FLASH_PageErase(FLASH_CPU_APPNAME_START,	FLASH_CPU_APPNAME_SIZE) == DEF_CPUFLASH_NONE_ERR)	
								{
										if(paraLen == 0)
												myPrintf("\r\nS_VERB=%s(OK!)\r\n",	tmpBuf); 
										else
										{
												if(BSP_CPU_FLASH_Write (FLASH_CPU_APPNAME_START,	tmpBuf,	paraLen) == DEF_CPUFLASH_NONE_ERR)	
														myPrintf("\r\nS_VERB=%s(OK!)\r\n",	tmpBuf); 
												else
														myPrintf("\r\nS_VERB=[Error-Write!]\r\n"); 	
										}
								}
								else
										myPrintf("\r\nS_VERB=[Error-Erase!]\r\n"); 
						}
						else
								myPrintf("\r\nS_VERB=[Error-ParaLen!]\r\n");
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 配置/读取设备ID号 ////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETID,	(char *)tmpCode) == 0)
				{
						// 设置ID号(手动输入11位ID程序自动补0组成6byte BCD)						
						if(paraLen == 11)
						{			
								memcpy(tmpBuf,tmpPara,11);
								tmpBuf[11] ='0';	tmpBuf[12] ='\0';
								u16TmpLen = String2HexArray((u8 *)tmpBuf,	12,	tmpBuf1);
								if(u16TmpLen != 0xffff)
								{      
                    HAL_CommonParaReload();
		                memcpy((u8 *)&s_common.id,	tmpBuf1,	u16TmpLen);	
										
										if(HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
												myPrintf("\r\nS_ID=%s(OK!)\r\n",  tmpPara);
										else
												myPrintf("\r\nS_ID=[Error-Write!]\r\n");
								}
								else
										myPrintf("\r\nS_ID=[Error-ParaLen!]\r\n");
						}
						else
								myPrintf("\r\nS_ID=[Error-ParaLen!]\r\n");
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEID,	(char *)tmpCode) == 0)
				{
						// 查询ID号
						if(HAL_LoadParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
						{
								u16TmpLen =	Hex2StringArray ((u8 *)&s_common.id,	PRA_SIZEOF_COMMON_ID,	tmpBuf);
								tmpBuf[PRA_SIZEOF_COMMON_ID*2-1] ='\0';	// 去除最后的0补位
								myPrintf("\r\nQ_ID=%s\r\n",  tmpBuf);	
						}
						else
								myPrintf("\r\nQ_ID=[Error-Read!]\r\n");						
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 电压自动校准////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_CV,	(char *)tmpCode) == 0)
				{
						// 自动校准12V基准
						if(s_adInfo.ADCVBatBK != 0)
						{
                HAL_CommonParaReload();
								s_common.aadc =s_adInfo.ADCVBatBK;
								if(HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
										myPrintf("\r\nS_CV=%d(OK!)\r\n",	s_common.aadc);
								else
										myPrintf("\r\nS_CV=[Error-Write!]\r\n");								
						}
						else
								myPrintf("\r\nS_CV=[Error-Busy!]\r\n");			
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询设备AD基准////////////////////////////////////////////////
				if(strcmp((char *)DBG_CMD_CODE_SETAADC,	(char *)tmpCode) == 0)
				{
						// 设置设备电压基准						
						if(paraLen <= 4)
						{
								memcpy(tmpBuf,	tmpPara,	paraLen);
								u16TmpLen =atoi((const char *)tmpBuf) & 0xfff; 
							  if(u16TmpLen <= 4095)
								{
                    HAL_CommonParaReload();
										s_common.aadc =u16TmpLen;
										if(HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
												myPrintf("\r\nS_AADC=%d(OK!)\r\n", s_common.aadc);
										else
												myPrintf("\r\nS_AADC=[Error-Write!]\r\n");
								}
								else
										myPrintf("\r\nS_AADC=[Error-Para!]\r\n");
						}
						else
								myPrintf("\r\nS_AADC=[Error-ParaLen!]\r\n");
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEAADC,	(char *)tmpCode) == 0)
				{
						// 查询设备电压基准
						if(HAL_LoadParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nQ_AADC=%d\r\n", s_common.aadc);		
						else
								myPrintf("\r\nQ_AADC=[Error-Read!]\r\n");					
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询设备RTC时间////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETTIME,	(char *)tmpCode) == 0)
				{
						// 设置时间	(格式必须为：YYYY-MM-DD-HH-MM-SS)
						for(u16p=0;	(tmpPara[u16p] != '-') && (u16p < paraLen);	u16p++)
						{
								tmpBuf[u16p] = tmpPara[u16p];
						}
						tmpBuf[u16p] = '\0';
						tmpRtc.year = atoi((const char *)tmpBuf);
						u16p++;
						///////////////////////////////////////
						for(u16m=0;	(tmpPara[u16p] != '-') && (u16p < paraLen);	u16p++,u16m++)
						{
								tmpBuf[u16m] = tmpPara[u16p];
						}
						tmpBuf[u16m] = '\0';
						tmpRtc.month = atoi((const char *)tmpBuf);
						u16p++;
						///////////////////////////////////////
						for(u16m=0;	(tmpPara[u16p] != '-') && (u16p < paraLen);	u16p++,u16m++)
						{
								tmpBuf[u16m] = tmpPara[u16p];
						}
						tmpBuf[u16m] = '\0';
						tmpRtc.day = atoi((const char *)tmpBuf);
						u16p++;
						///////////////////////////////////////
						for(u16m=0;	(tmpPara[u16p] != '-') && (u16p < paraLen);	u16p++,u16m++)
						{
								tmpBuf[u16m] = tmpPara[u16p];
						}
						tmpBuf[u16m] = '\0';
						tmpRtc.hour = atoi((const char *)tmpBuf);
						u16p++;
						///////////////////////////////////////
						for(u16m=0;	(tmpPara[u16p] != '-') && (u16p < paraLen);	u16p++,u16m++)
						{
								tmpBuf[u16m] = tmpPara[u16p];
						}
						tmpBuf[u16m] = '\0';
						tmpRtc.minute = atoi((const char *)tmpBuf);
						u16p++;
						///////////////////////////////////////
						for(u16m=0;	(tmpPara[u16p] != '-') && (u16p < paraLen);	u16p++,u16m++)
						{
								tmpBuf[u16m] = tmpPara[u16p];
						}
						tmpBuf[u16m] = '\0';
						tmpRtc.second = atoi((const char *)tmpBuf);
						BSP_RTC_Set_Current(&tmpRtc);
						///////////////////////////////////////						
						myPrintf("\r\nS_TIME=");
						myPrintf("%d-%d-%d %d:%d:%d\r\n\r\n",	tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
						myPrintf("(OK!)\r\n");
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUETIME,	(char *)tmpCode) == 0)
				{
						// 查询时间
						myPrintf("\r\nQ_TIME=");
						BSP_RTC_Get_Current(&tmpRtc);
						myPrintf("%d-%d-%d %d:%d:%d\r\n\r\n",	tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询APN////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETAPN,	(char *)tmpCode) == 0)
				{
						// (格式:$S_APN:CMNET,,)
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) > PRA_SIZEOF_SYSCFG_APN) || (strlen((const char *)tmpBuf) == 0))
						{
								myPrintf("\r\nS_APN=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)
						{
								tmpBuf1[u16m] = tmpPara[u16p];
						}
						tmpBuf1[u16m] = '\0';
						if(strlen((const char *)tmpBuf1) > PRA_SIZEOF_SYSCFG_APNUSER)
						{
								myPrintf("\r\nS_APN=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)
						{
								tmpBuf2[u16m] = tmpPara[u16p];
						}
						tmpBuf2[u16m] = '\0';
						if(strlen((const char *)tmpBuf2) > PRA_SIZEOF_SYSCFG_APNPASS)
						{
								myPrintf("\r\nS_APN=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
            HAL_SysParaReload();
						strcpy((char *)(s_cfg.apn),	(char *)tmpBuf);
						strcpy((char *)(s_cfg.apnUser),	(char *)tmpBuf1);
						strcpy((char *)(s_cfg.apnPass),	(char *)tmpBuf2);
						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_APN=%s,%s,%s(OK!)\r\n",	s_cfg.apn,s_cfg.apnUser,s_cfg.apnPass);
						else
								myPrintf("\r\nS_APN=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEAPN,	(char *)tmpCode) == 0)
				{
						// 查询APN
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)	
								myPrintf("\r\nQ_APN=%s,%s,%s",	s_cfg.apn,s_cfg.apnUser,s_cfg.apnPass);
						else
								myPrintf("\r\nQ_APN=[Error-Read!]\r\n");
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询IP////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETIP,	(char *)tmpCode) == 0)
				{
						// (格式:$S_IP:126.124.67.176,13000)
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if(strlen((const char *)tmpBuf) > PRA_SIZEOF_SYSCFG_IP1)
						{
								myPrintf("\r\nS_IP=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						/////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)
						{
								tmpBuf1[u16m] = tmpPara[u16p];
						}
						tmpBuf1[u16m] = '\0';
						if((strlen((const char *)tmpBuf1) > PRA_SIZEOF_SYSCFG_PORT1) ||
							 (IsIntString(tmpBuf1,	strlen((const char *)tmpBuf1)) == 0xffff) ||
							 (atoi((const char *)tmpBuf1) > 0xffff))
						{
								myPrintf("\r\nS_IP=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
            HAL_SysParaReload();
						strcpy((char *)(s_cfg.ip1),	(char *)tmpBuf);
						strcpy((char *)(s_cfg.port1),	(char *)tmpBuf1);
						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_IP=%s,%s(OK!)\r\n",	s_cfg.ip1,s_cfg.port1);
						else
								myPrintf("\r\nS_IP=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEIP,	(char *)tmpCode) == 0)
				{
						// 查询IP地址
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nQ_IP=%s,%s\r\n",	s_cfg.ip1,s_cfg.port1);
						else
								myPrintf("\r\nQ_IP=[Error-Read!]\r\n");
				}
//				else if(strcmp((char *)DBG_CMD_CHANGE_SETAPPIP,	(char *)tmpCode) == 0)
//				{
//						u16p=0;
//						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
//						{
//								tmpBuf[u16m] = tmpPara[u16p];		
//						}	
//						tmpBuf[u16m] = '\0';
//						if(tmpBuf[0] != 0x30)
//						{
//							s_cfg.testapp2 = 1;
//						}
//						else
//						{
//							s_cfg.testapp2 = 0;
//						}
//						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
//								myPrintf("\r\nS_APP2=%d(OK!)\r\n",	s_cfg.testapp2);
//						else
//								myPrintf("\r\nS_APP2=[Error-Write!]\r\n");							
//				}
//				else if(strcmp((char *)DBG_CMD_CHANGE_QUEAPPIP,	(char *)tmpCode) == 0)
//				{
//						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
//								myPrintf("\r\nQ_APP2=%d(OK!)\r\n",s_cfg.testapp2);
//						else
//								myPrintf("\r\nQ_APP2=[Error-Read!]\r\n");						
//				}
				else if(strcmp((char *)DBG_CMD_CODE_JUMP1,	(char *)tmpCode) == 0)
				{
					   myPrintf("JUMP1\r\n");
						GPIO_ResetBits		(GPIOC, BSP_GPIOC_GSM_DTR | BSP_GPIOC_GSM_POWEN | BSP_GPIOC_SPAKEREN);
						GPIO_SetBits		(GPIOA, BSP_GPIOA_GSM_POWON);
 					   OSTimeDly(5000);
						 USART_DeInit(USART1);
						 USART_Cmd(USART1,DISABLE);
						 USART_DeInit(USART2);
						 USART_Cmd(USART2,DISABLE);	
						 USART_DeInit(USART3);
						 USART_Cmd(USART3,DISABLE);	
						 USART_DeInit(UART4);
						 USART_Cmd(UART4,DISABLE);	
						 USART_DeInit(UART5);
						 USART_Cmd(UART5,DISABLE);
						 BSP_SYSEXIT_Init();	
						 JumpToApp1();		
				}						
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置开始本地升级////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETUPDATE,	(char *)tmpCode) == 0)
				{							
            HAL_CommonParaReload();
						s_common.iapFlag =DEF_IAP_LOCATIONCMD_FLAG;
						if(HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
						{
								myPrintf("\r\nS_UPDATE=(OK!)\r\n");
								HAL_CPU_ResetPro(); // 复位进入BootLoader
						}
						else
						{
								s_common.iapFlag =DEF_IAP_CLEAN_FLAG;
								myPrintf("\r\nS_UPDATE=[Error-Write!]\r\n");
						}
				}			
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置开始本地升级1////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETUPDATE1,	(char *)tmpCode) == 0)
				{							
						if(BSP_BACKUP_UpdateFlagSet(DEF_IAP_LOCATIONBKP_FLAG) == DEF_BKP_NONE_ERR)
						{
								myPrintf("\r\nS_UPDATE1=(OK!)\r\n");						
								HAL_CPU_ResetPro(); // 复位进入BootLoader
						}
						else
						{
								s_common.iapFlag =DEF_IAP_CLEAN_FLAG;
								myPrintf("\r\nS_UPDATE1=[Error-Write!]\r\n");
						}
				}			
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置开始远程升级////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETOTAP,	(char *)tmpCode) == 0)
				{		
						s_common.iapSta = DEF_UPDATE_KEYWORD;						
						myPrintf("\r\nS_OTAP=(OK!)\r\n");							
				}	
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 查询当前队列状态////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_QUEQUEUE,	(char *)tmpCode) == 0)
				{		
						if(tmpSms.len != 0)
								flag0 =1;
						if(s_login.len != 0)
								flag1 =1;
						if(s_obd.len != 0)
								flag2 =1;
						if(s_ram.len != 0)
								flag3 =1;
						if(s_flash.len != 0)
								flag4 =1;
					
						myPrintf("\r\nQ_QUE=TmpSms  :%d",flag0);
						myPrintf("\r\n      TmpLogin:%d",flag1);
						myPrintf("\r\n      TmpObd  :%d",flag2);
						myPrintf("\r\n      TmpRam  :%d",flag3);
						myPrintf("\r\n      TmpFlash:%d",flag4);
						myPrintf("\r\n      SmsQue  :%d/%d,in=%d,out=%d",s_queSms.entries,s_queSms.size,s_queSms.in,s_queSms.out);
						myPrintf("\r\n      RamQue  :%d/%d,in=%d,out=%d",s_queRam.entries,s_queRam.size,s_queRam.in,s_queRam.out);
						myPrintf("\r\n      FlashQue:%d/%d,in=%d,out=%d",s_queFlash.entries,s_queFlash.size,s_queFlash.in,s_queFlash.out);
						myPrintf("\r\n      GpsQue  :%d/%d,in=%d,out=%d\r\n",s_qDss.OSQEntries,s_qDss.OSQSize,s_qDss.OSQIn,s_qDss.OSQOut);
				}	
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 清空队列数据信息////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETQUEUE,	(char *)tmpCode) == 0)
				{							
						HAL_ComQue_ForceReset(DEF_ALL_QUE);
						if(tmpSms.len != 0)
								flag0 =1;
						if(s_login.len != 0)
								flag1 =1;
						if(s_obd.len != 0)
								flag2 =1;
						if(s_ram.len != 0)
								flag3 =1;
						if(s_flash.len != 0)
								flag4 =1;
						
						myPrintf("\r\nS_QUE=TmpSms  :%d",flag0);
						myPrintf("\r\n      TmpLogin:%d",flag1);
						myPrintf("\r\n      TmpObd  :%d",flag2);
						myPrintf("\r\n      TmpRam  :%d",flag3);
						myPrintf("\r\n      TmpFlash:%d",flag4);
						myPrintf("\r\n      SmsQue  :%d/%d,in=%d,out=%d",s_queSms.entries,s_queSms.size,s_queSms.in,s_queSms.out);
						myPrintf("\r\n      RamQue  :%d/%d,in=%d,out=%d",s_queRam.entries,s_queRam.size,s_queRam.in,s_queRam.out);
						myPrintf("\r\n      FlashQue:%d/%d,in=%d,out=%d",s_queFlash.entries,s_queFlash.size,s_queFlash.in,s_queFlash.out);		
						myPrintf("\r\n      GpsQue  :%d/%d,in=%d,out=%d(OK!)\r\n",s_qDss.OSQEntries,s_qDss.OSQSize,s_qDss.OSQIn,s_qDss.OSQOut);
				}		
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询车系,车型及发送机品牌////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETCARINFO,	(char *)tmpCode) == 0)
				{
						// (格式:$S_CARINFO:车系,车型,发动机品牌) 每个字段一个字符
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) != 1) || ((IsA2ZString(tmpBuf, 1) == 0xffff) && (IsIntString(tmpBuf, 1) == 0xffff)))
						{
								myPrintf("\r\nS_CARINFO=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf1[u16m] = tmpPara[u16p];		
						}	
						tmpBuf1[u16m] = '\0';
						if((strlen((const char *)tmpBuf1) != 1) || ((IsA2ZString(tmpBuf1, 1) == 0xffff) && (IsIntString(tmpBuf1, 1) == 0xffff)))
						{
								myPrintf("\r\nS_CARINFO=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						tmpBuf1[0] -= 0x30;	// 转成数值
						///////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf2[u16m] = tmpPara[u16p];		
						}	
						tmpBuf2[u16m] = '\0';
						if((strlen((const char *)tmpBuf2) != 1) || ((IsA2ZString(tmpBuf2, 1) == 0xffff) && (IsIntString(tmpBuf2, 1) == 0xffff)))
						{
								myPrintf("\r\nS_CARINFO=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						tmpBuf2[0] -= 0x30;	// 转成数值
						///////////////////
            HAL_CommonParaReload();
						s_common.carInfo[0] =tmpBuf[0];
						s_common.carInfo[1] =tmpBuf1[0];
						s_common.carInfo[2] =tmpBuf2[0];
						if(HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_CARINFO=%c,%d,%d(OK!)\r\n",	s_common.carInfo[0],s_common.carInfo[1],s_common.carInfo[2]);
						else
								myPrintf("\r\nS_CARINFO=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUECARINFO,	(char *)tmpCode) == 0)
				{
						// 查询车系,车型及发送机品牌
						if(HAL_LoadParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)	
								myPrintf("\r\nQ_CARINFO='%c',%d,%d",	s_common.carInfo[0],s_common.carInfo[1],s_common.carInfo[2]);
						else
								myPrintf("\r\nQ_CARINFO=[Error-Read!]\r\n");
				}		
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询GPS上报时间间隔////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SGPSRPTTIME,	(char *)tmpCode) == 0)
				{
						// (格式:$S_GPSRPT:GPS采集间隔(byte:秒),GPS上传间隔(u16:秒)) 
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) > 3) || 
							 (IsIntString(tmpBuf, strlen((const char *)tmpBuf)) == 0xffff) || 
							 (atoi((const char *)tmpBuf) > 255))
						{
								myPrintf("\r\nS_GPSRPT=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf1[u16m] = tmpPara[u16p];		
						}	
						tmpBuf1[u16m] = '\0';
						if((strlen((const char *)tmpBuf1) > 5) || 
							 (IsIntString(tmpBuf1, strlen((const char *)tmpBuf1)) == 0xffff) || 
							 (atoi((const char *)tmpBuf1) > 65545))
						{
								myPrintf("\r\nS_GPSRPT=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////						
            HAL_SysParaReload();
						s_cfg.gpsCollectTime =atoi((const char *)tmpBuf);
						s_cfg.onReportTime	 =atoi((const char *)tmpBuf1);
						
						if(s_cfg.gpsCollectTime <= 5)
								s_cfg.gpsCollectTime =5;		// 采集间隔最小为5秒
						if(s_cfg.onReportTime <= 5)
								s_cfg.onReportTime 	 =5;		// 上传间隔最小为5秒
											
						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_GPSRPT=%d,%d(OK!)\r\n",	s_cfg.gpsCollectTime,s_cfg.onReportTime);
						else
								myPrintf("\r\nS_GPSRPT=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QGPSRPTTIME,	(char *)tmpCode) == 0)
				{
						// 查询GPS上报时间间隔
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)	
								myPrintf("\r\nQ_GPSRPT=%d,%d(OK!)\r\n",	s_cfg.gpsCollectTime,s_cfg.onReportTime);
						else
								myPrintf("\r\nQ_GPSRPT=[Error-Read!]\r\n");
				}		
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询健康包上报时间间隔////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETHEALTHRPT,	(char *)tmpCode) == 0)
				{
						// (格式:$S_HEALTHRPT:时间间隔(u32:秒)) 
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) > 10) || 
							 (IsIntString(tmpBuf, strlen((const char *)tmpBuf)) == 0xffff) || 
							 (atoi((const char *)tmpBuf) > 0xffffffff))
						{
								myPrintf("\r\nS_HEALTHRPT=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						
						///////////////////			
            HAL_SysParaReload();
						s_cfg.healthReportTime =atoi((const char *)tmpBuf);									
						if(s_cfg.healthReportTime <= 300)
								s_cfg.healthReportTime =300;		// 最小上传间隔为5分钟
						if(s_cfg.healthReportTime > 86000)
								s_cfg.healthReportTime =86000;		// 最大上传间隔为23小时54分
											
						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_HEALTHRPT=%d(OK!)\r\n",	s_cfg.healthReportTime);
						else
								myPrintf("\r\nS_HEALTHRPT=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEHEALTHRPT,	(char *)tmpCode) == 0)
				{
						// 查询健康包
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)	
								myPrintf("\r\nQ_HEALTHRPT=%d(OK!)\r\n",	s_cfg.healthReportTime);
						else
								myPrintf("\r\nQ_HEALTHRPT=[Error-Read!]\r\n");
				}	
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询设备开关////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETDEVEN,	(char *)tmpCode) == 0)
				{
						// (格式:$S_DEVEN:开关关键字(u8:)) 
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) != 2) || 
							 (IsHexString(tmpBuf, strlen((const char *)tmpBuf)) == 0xffff))
						{
								myPrintf("\r\nS_DEVEN=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						
						///////////////////
						u32Tmp =0;
						sscanf((const char *)&tmpBuf,	"%x",	&u32Tmp);
						s_cfg.devEn	=(u8)(u32Tmp);
											
						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_DEVEN=%02X(OK!)\r\n",	s_cfg.devEn);
						else
								myPrintf("\r\nS_DEVEN=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEDEVEN,	(char *)tmpCode) == 0)
				{
						// 查询设备开关
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)	
								myPrintf("\r\nQ_DEVEN=%02X(OK!)\r\n",	s_cfg.devEn);
						else
								myPrintf("\r\nQ_DEVEN=[Error-Read!]\r\n");
				}		
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询设备熄火状态判断延时时间////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETIGOFFDELAY,	(char *)tmpCode) == 0)
				{
						// (格式:$S_IGOFFDELAY:延迟时间(u16:秒)) 
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) > 5) || 
							 (IsIntString(tmpBuf, strlen((const char *)tmpBuf)) == 0xffff) || 
							 (atoi((const char *)tmpBuf) > 0xffff))
						{
								myPrintf("\r\nS_IGOFFDELAY=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						
						///////////////////				
            HAL_SysParaReload();
						s_cfg.igOffDelay =atoi((const char *)tmpBuf);				
											
						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_IGOFFDELAY=%d(OK!)\r\n",	s_cfg.igOffDelay);
						else
								myPrintf("\r\nS_IGOFFDELAY=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEIGOFFDELAY,	(char *)tmpCode) == 0)
				{
						// 查询熄火状态判断延迟时间
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)	
								myPrintf("\r\nQ_IGOFFDELAY=%d(OK!)\r\n",	s_cfg.igOffDelay);
						else
								myPrintf("\r\nQ_IGOFFDELAY=[Error-Read!]\r\n");
				}		
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询驾驶习惯参数////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETDSS,	(char *)tmpCode) == 0)
				{
						// (格式:$S_DSS:急加速门限,急减速门限,碰撞门限) 每个字段80~65535(至少要大于姿态调整下门限80)
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) == 0) || (strlen((const char *)tmpBuf) > 5)|| (IsIntString(tmpBuf, strlen((const char *)tmpBuf)) == 0xffff))
						{
								myPrintf("\r\nS_DSS=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf1[u16m] = tmpPara[u16p];		
						}	
						tmpBuf1[u16m] = '\0';
						if((strlen((const char *)tmpBuf1) == 0) || (strlen((const char *)tmpBuf1) > 5)|| (IsIntString(tmpBuf1, strlen((const char *)tmpBuf1)) == 0xffff))
						{
								myPrintf("\r\nS_DSS=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf2[u16m] = tmpPara[u16p];		
						}	
						tmpBuf2[u16m] = '\0';
						if((strlen((const char *)tmpBuf2) == 0) || (strlen((const char *)tmpBuf2) > 5)|| (IsIntString(tmpBuf2, strlen((const char *)tmpBuf2)) == 0xffff))
						{
								myPrintf("\r\nS_DSS=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
            HAL_SysParaReload();
						u16TmpLen = atoi((const char *)tmpBuf);
						if(u16TmpLen < 80)
								u16TmpLen =80;
						else if(u16TmpLen > 65535)
								u16TmpLen =65535;
						s_cfg.dssATH = u16TmpLen;
						///////////////////
						u16TmpLen = atoi((const char *)tmpBuf1);
						if(u16TmpLen < 80)
								u16TmpLen =80;
						else if(u16TmpLen > 65535)
								u16TmpLen =65535;
						s_cfg.dssDTH = u16TmpLen;
						///////////////////
						u16TmpLen = atoi((const char *)tmpBuf2);
						if(u16TmpLen < 80)
								u16TmpLen =80;
						else if(u16TmpLen > 65535)
								u16TmpLen =65535;
						s_cfg.dssPTH = u16TmpLen;
						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_DSS=%d,%d,%d(OK!)\r\n",s_cfg.dssATH,s_cfg.dssDTH,s_cfg.dssPTH);
						else
								myPrintf("\r\nS_DSS=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEDSS,	(char *)tmpCode) == 0)
				{
						// 查询驾驶习惯参数
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nQ_DSS=%d,%d,%d",	s_cfg.dssATH,s_cfg.dssDTH,s_cfg.dssPTH);
						else
								myPrintf("\r\nQ_DSS=[Error-Read!]\r\n");
				}		
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询加速度中断相关参数////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETACC,	(char *)tmpCode) == 0)
				{
						// (格式:$S_ACC:门限(mg),阀值(ms) (LIS3DH:0~127,MPU6050:0~255)
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) == 0) || (strlen((const char *)tmpBuf) > 3)|| (IsIntString(tmpBuf, strlen((const char *)tmpBuf)) == 0xffff))
						{
								myPrintf("\r\nS_ACC=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf1[u16m] = tmpPara[u16p];		
						}	
						tmpBuf1[u16m] = '\0';
						if((strlen((const char *)tmpBuf1) == 0) || (strlen((const char *)tmpBuf1) > 3)|| (IsIntString(tmpBuf1, strlen((const char *)tmpBuf1)) == 0xffff))
						{
								myPrintf("\r\nS_ACC=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////					
						flag0 = (u8)(atoi((const char *)tmpBuf));
						flag1 = (u8)(atoi((const char *)tmpBuf1));
						
						if(MEMS_DEVICE.InitSta == DEF_CHIP_LIS3DH)
						{
								flag0 &= 0x7f;	// LIS3DH最大值127
								flag1 &= 0x7f;
						}						
						if(BSP_ACC_CfgInt (DEF_ENABLE,	(u8)flag0,	(u8)flag1,	5000) == 0)
						{
                HAL_SysParaReload();
								s_cfg.accMT = flag0;
								s_cfg.accMD = flag1;
								if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
										myPrintf("\r\nS_ACC=%d,%d(OK!)\r\n",s_cfg.accMT,s_cfg.accMD);
								else
										myPrintf("\r\nS_ACC=[Error-Write!]\r\n");	
						}
						else
								myPrintf("\r\nS_ACC=[Error-Write-Reg!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEACC,	(char *)tmpCode) == 0)
				{
						// 查询加速度门限
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nQ_ACC=%d,%d",	s_cfg.accMT,s_cfg.accMD);
						else
								myPrintf("\r\nQ_ACC=[Error-Read!]\r\n");
				}	
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询设备睡眠时间////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETSLEEPTIME,	(char *)tmpCode) == 0)
				{
						// (格式:$S_SLEEPTIME:时间(u32:秒)) 
						u16p=0;
            HAL_SysParaReload();
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) > 10) || 
							 (IsIntString(tmpBuf, strlen((const char *)tmpBuf)) == 0xffff) || 
							 (atoi((const char *)tmpBuf) > 0xffffffff))
						{
								myPrintf("\r\nS_SLEEPTIME=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						
						///////////////////						
						u32Tmp =atoi((const char *)tmpBuf);									
						if(u32Tmp <= 60)
								s_cfg.globSleepTime =60;			// 最小睡眠时间为60s
						else if(u32Tmp > 86000)
								s_cfg.globSleepTime =86000;		// 最大休眠时间为86000(23小时54分)
						else
								s_cfg.globSleepTime =u32Tmp;	// 
											
						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_SLEEPTIME=%d(OK!)\r\n",	s_cfg.globSleepTime);
						else
								myPrintf("\r\nS_SLEEPTIME=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUESLEEPTIME,	(char *)tmpCode) == 0)
				{
						// 查询睡眠时间
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)	
								myPrintf("\r\nQ_SLEEPTIME=%d(OK!)\r\n",	s_cfg.globSleepTime);
						else
								myPrintf("\r\nQ_SLEEPTIME=[Error-Read!]\r\n");
				}		
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 设置/查询系统熄火移动报警参数////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SETIGOFFMOVE,	(char *)tmpCode) == 0)
				{
						// (格式:$S_IGOFFMOVE:Speed(1~255km/h),Time(1~255s)
						u16p=0;
            HAL_SysParaReload();
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if((strlen((const char *)tmpBuf) == 0) || (strlen((const char *)tmpBuf) > 3)|| (IsIntString(tmpBuf, strlen((const char *)tmpBuf)) == 0xffff))
						{
								myPrintf("\r\nS_IGOFFMOVE=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf1[u16m] = tmpPara[u16p];		
						}	
						tmpBuf1[u16m] = '\0';
						if((strlen((const char *)tmpBuf1) == 0) || (strlen((const char *)tmpBuf1) > 3)|| (IsIntString(tmpBuf1, strlen((const char *)tmpBuf1)) == 0xffff))
						{
								myPrintf("\r\nS_IGOFFMOVE=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						///////////////////					
						flag0 = (u8)(atoi((const char *)tmpBuf));
						if(flag0 == 0)
								flag0 =1;
						flag1 = (u8)(atoi((const char *)tmpBuf1));
						if(flag1 == 0)
								flag1 =1;
						s_cfg.sysMove 	=flag0;
						s_cfg.sysMoveT 	=flag1;
												
						if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nS_IGOFFMOVE=%d,%d(OK!)\r\n",s_cfg.sysMove,s_cfg.sysMoveT);
						else
								myPrintf("\r\nS_IGOFFMOVE=[Error-Write!]\r\n");	
				}
				else if(strcmp((char *)DBG_CMD_CODE_QUEIGOFFMOVE,	(char *)tmpCode) == 0)
				{
						// 查询系统熄火移动报警相关参数
						if(HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
								myPrintf("\r\nQ_IGOFFMOVE=%d,%d",	s_cfg.sysMove,s_cfg.sysMoveT);
						else
								myPrintf("\r\nQ_IGOFFMOVE=[Error-Read!]\r\n");
				}					
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// GSM透传指令////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_GSMBYPASS,	(char *)tmpCode) == 0)
				{
						// (格式:"$C_GSMBYPASS:ATXXXX~2" AT指令最长100字节,超时单位s最长120s)
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != '~') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\r'; u16m++; // 添加结束符
						tmpBuf[u16m] = '\0';
						if(strlen((const char *)tmpBuf) > 100)
						{
								myPrintf("\r\nC_GSMBYPASS=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						////////////////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf1[u16m] = tmpPara[u16p];		
						}	
						tmpBuf1[u16m] = '\0';
						if((strlen((const char *)tmpBuf1) > 3) || (IsIntString(tmpBuf1,	strlen((const char *)tmpBuf1)) == 0xffff) ||
							 (atoi((const char *)tmpBuf1) > 120))
						{
								myPrintf("\r\nC_GSMBYPASS=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						////////////////////////////				
						u16m=0;
						u32Tmp = atoi((const char *)tmpBuf1)*10;	// GSM超时基准为100ms
						ret = BSP_GSMBypassPro(tmpBuf,	strlen((const char *)tmpBuf),	tmpBuf,	&u16m, sizeof(tmpBuf),	u32Tmp,	5000);
						if(ret == DEF_GSM_NONE_ERR)
						{ 
								myPrintf("\r\nC_GSMBYPASS=");
								BSP_PutChar (DEF_DBG_UART,	tmpBuf,	u16m);
								printf("\r\n");
						}		
						else
								myPrintf("\r\nC_GSMBYPASS=%-d\r\n",	ret);	
				}		
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// BT透传指令////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_BTBYPASS,	(char *)tmpCode) == 0)
				{
						// (格式:"$C_BTBYPASS:XXXXX" 透传数据最长100字节)
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != '~') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\r'; 
						if(strlen((const char *)tmpBuf) > 100)
						{
								myPrintf("\r\nC_BTBYPASS=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						////////////////////////////				
						ret = BSP_BTSendDataPro	(tmpBuf,	strlen((const char *)tmpBuf),	5000);
						if(ret == DEF_GSM_NONE_ERR)
						{ 
								myPrintf("\r\nC_BTBYPASS=(OK!)");
						}		
						else
								myPrintf("\r\nC_BTBYPASS=%-d\r\n",	ret);	
				}				
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// TTS透传指令////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_TTSBYPASS,	(char *)tmpCode) == 0)
				{
						// (格式:"$C_TTSBYPASS:mode,XXXXX" 透传数据最长100字节)
						u16p=0;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf1[u16m] = tmpPara[u16p];		
						}	
						tmpBuf1[u16m] = '\0'; 
						if((strlen((const char *)tmpBuf1) > 1) ||
							 (IsIntString(tmpBuf1,	strlen((const char *)tmpBuf1)) == 0xffff))
						{
								myPrintf("\r\nC_TTSBYPASS=[Error-Para!]\r\n");		
								goto dbgendloop;
						}
						////////////////////////////
						u16p++;
						for(u16m=0;	(tmpPara[u16p] != ',') && (u16p < paraLen);	u16p++,u16m++)	
						{
								tmpBuf[u16m] = tmpPara[u16p];		
						}	
						tmpBuf[u16m] = '\0';
						if(strlen((const char *)tmpBuf) > 100)
						{
								myPrintf("\r\nC_TTSBYPASS=[Error-Para!]\r\n");		
								goto dbgendloop;
						}					
						////////////////////////////	
						u8Tmp = atoi((const char *)tmpBuf1);
						ret = BSP_TTSPlayPro(u8Tmp,	tmpBuf, u16m,	5000);
						if(ret == DEF_GSM_NONE_ERR)
						{ 
								myPrintf("\r\nC_TTSBYPASS=(OK!)");
						}		
						else
								myPrintf("\r\nC_TTSBYPASS=%-d\r\n",	ret);	
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 主动断网指令////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_CODE_SHUTNET,	(char *)tmpCode) == 0)
				{
						if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
						{
								BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
								myPrintf("\r\nC_SHUTNET=(OK!)");		
						}
						else
								myPrintf("\r\nC_SHUTNET=Already break!!(OK!)");
				}
				
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 测试指令-软件模拟////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_TEST1,	(char *)tmpCode) == 0)
				{
						HAL_24RptInQueueProcess();
						myPrintf("\r\nT_TEST1=(OK!)");
				}
				else if(strcmp((char *)DBG_CMD_TEST_TEST2,	(char *)tmpCode) == 0)
				{
						HAL_LogInQueueProcess(TYPE_LOG_FLASH);
						myPrintf("\r\nT_TEST2=(OK!)");
				}
				else if(strcmp((char *)DBG_CMD_TEST_TEST3,	(char *)tmpCode) == 0)
				{
						HAL_GPSInQueueProcess();
						myPrintf("\r\nT_TEST3=(OK!)");
				}
				else if(strcmp((char *)DBG_CMD_TEST_TEST4,	(char *)tmpCode) == 0)
				{
						HAL_ZiTaiInQueueProcess(TYPE_ZITAI_WTH,&s_gpsPro,0,0,0,0);
						myPrintf("\r\nT_TEST4=(OK!)");
				}
				else if(strcmp((char *)DBG_CMD_TEST_TEST5,	(char *)tmpCode) == 0)
				{
						HAL_DssInQueueProcess(TYPE_DSS_DTH);	
						myPrintf("\r\nT_TEST5=(OK!)");
				}
				else if(strcmp((char *)DBG_CMD_TEST_TEST6,	(char *)tmpCode) == 0)
				{
						HAL_AlarmInQueueProcess(ALARM_BIT_POWER);
						myPrintf("\r\nT_TEST6=(OK!)");
				}
				else if(strcmp((char *)DBG_CMD_TEST_TEST7,	(char *)tmpCode) == 0)
				{
						//queDevParaProcess((Protocol_Typedef *)"");
						myPrintf("\r\nT_TEST7=(OK!)");
				}
				else if(strcmp((char *)DBG_CMD_TEST_TJ,	(char *)tmpCode) == 0)
				{
						if(HAL_SYS_GetState (BIT_SYSTEM_TJSTA) == 0)
						{
								HAL_SendOBDProProcess(CMD_AUTO_TJ,	0,	0,	(u8 *)"",	0);	// 通知obd任务执行自动体检
								HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_ENABLE);	// 系统置体检状态中
								if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
										HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound1,	strlen((const char *)s_cfg.sound1));		// 播放体检开始语音	
								myPrintf("\r\nT_TJ=(OK!)");
						}	
						else
								myPrintf("\r\nT_TJ=(Busy!)");
				}	
				else if(strcmp((char *)DBG_CMD_TEST_TJ1,	(char *)tmpCode) == 0)
				{
						if(HAL_SYS_GetState (BIT_SYSTEM_TJSTA) == 0)
						{
                HAL_CommonParaReload();
								s_common.otapErrCode =1;	// 清故障标志
								HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));			
								myPrintf("\r\nT_TJ1=(OK!)");
						}	
						else
								myPrintf("\r\nT_TJ1=(Busy!)");
				}	
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_IG_ON,	(char *)tmpCode) == 0)
				{
						if(HAL_SendCONProProcess(CAR_CMD_IG_ON,	0x0000,	(u8 *)"",	0) == 0)		
								myPrintf("\r\nT_IGON=(OK!)");
						else
								myPrintf("\r\nT_IGON=(Busy!)");
				}	
				else if(strcmp((char *)DBG_CMD_TEST_IG_OFF,	(char *)tmpCode) == 0)
				{
						if(HAL_SendCONProProcess(CAR_CMD_IG_OFF,	0x0000,	(u8 *)"",	0) == 0)		
								myPrintf("\r\nT_IGOFF=(OK!)");
						else
								myPrintf("\r\nT_IGOFF=(Busy!)");
				}	
				else if(strcmp((char *)DBG_CMD_TEST_ARM_SET,	(char *)tmpCode) == 0)
				{
						if(HAL_SendCONProProcess(CAR_CMD_ARM_SET,	0x0000,	(u8 *)"",	0) == 0)		
								myPrintf("\r\nT_ARMSET=(OK!)");
						else
								myPrintf("\r\nT_ARMSET=(Busy!)");
				}	
				else if(strcmp((char *)DBG_CMD_TEST_ARM_RST,	(char *)tmpCode) == 0)
				{
						if(HAL_SendCONProProcess(CAR_CMD_ARM_RST,	0x0000,	(u8 *)"",	0) == 0)		
								myPrintf("\r\nT_ARMRST=(OK!)");
						else
								myPrintf("\r\nT_ARMRST=(Busy!)");
				}	
				else if(strcmp((char *)DBG_CMD_TEST_FIND_CAR,	(char *)tmpCode) == 0)
				{
						if(HAL_SendCONProProcess(CAR_CMD_FIND_CAR,	0x0000,	(u8 *)"",	0) == 0)		
								myPrintf("\r\nT_FINDCAR=(OK!)");
						else
								myPrintf("\r\nT_FINDCAR=(Busy!)");
				}	
				else if(strcmp((char *)DBG_CMD_TEST_READ_STA,	(char *)tmpCode) == 0)
				{
						if(HAL_SendCONProProcess(CAR_CMD_READSTA,	0x0000,	(u8 *)"",	0) == 0)		
								myPrintf("\r\nT_READSTA=(OK!)");
						else
								myPrintf("\r\nT_READSTA=(Busy!)");
				}	
				else if(strcmp((char *)DBG_CMD_TEST_TRUNK,	(char *)tmpCode) == 0)
				{
						if(HAL_SendCONProProcess(CAR_CMD_TRUNK,	0x0000,	(u8 *)"",	0) == 0)		
								myPrintf("\r\nT_TRUNK=(OK!)");
						else
								myPrintf("\r\nT_TRUNK=(Busy!)");
				}	
				
				
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 测试指令-硬件测试////////////////////////////////////////////////
				// FLASH测试////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_FLASH,	(char *)tmpCode) == 0)
				{
						BSP_FLASH_Check();	// FLASH第0页读写测试
						if((FLASH_DEVICE.InitSta == 1) && (*(s_devSta.pFlash)	== DEF_FLASHHARD_NONE_ERR))
								myPrintf("\r\nT_FLASH=(OK!)");
						else
								myPrintf("\r\nT_FLASH=(ERROR!)");				
				}
				// ACC测试////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_ACC,	(char *)tmpCode) == 0)
				{
						if((MEMS_DEVICE.InitSta != 0) && (*(s_devSta.pMems)	== DEF_MEMSHARD_NONE_ERR))
								myPrintf("\r\nT_ACC=(OK!)");
						else
								myPrintf("\r\nT_ACC=(ERROR!)");
				}
				// GPS测试////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_GPS,	(char *)tmpCode) == 0)
				{
						if((GPS_DEVICE.InitSta == 1) && (*(s_devSta.pGps) == DEF_GPSHARD_NONE_ERR))
								myPrintf("\r\nT_GPS=(OK!)");
						else
								myPrintf("\r\nT_GPS=(ERROR!)");
				}
				// GSM测试////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_GSM,	(char *)tmpCode) == 0)
				{
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
								myPrintf("\r\nT_GSM=(OK!)");
						else
								myPrintf("\r\nT_GSM=(ERROR!)");		
				}
				// CAN测试////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_CAN,	(char *)tmpCode) == 0)
				{
						s_test.busTestCode = 0xAA;
						u8Tmp =0x00;	// CAN总线测试
						if(HAL_SendOBDProProcess(CMD_TEST,	0x0000,	0x0000,	&u8Tmp,	1) != 0)
								myPrintf("\r\nT_CAN=(BUSY!)");
						else
						{
								for(u16m=0;	u16m<100; u16m++)
								{
										if((s_test.busTestCode == 0x00) || (s_test.busTestCode == 0xEE))
												break;
										else
												OSTimeDly(100);
								}
								if(u16m < 100)
								{
										if(s_test.busTestCode == 0x00)
												myPrintf("\r\nT_CAN=(OK!)");
										else if(s_test.busTestCode == 0xEE)
												myPrintf("\r\nT_CAN=(ERROR!)");
								}
								else
										myPrintf("\r\nT_CAN=(ERROR-TO!)");	
						}
				}
				// K测试////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_K,	(char *)tmpCode) == 0)
				{
						s_test.busTestCode = 0xBB;
						u8Tmp =0x01;	// K总线测试
						if(HAL_SendOBDProProcess(CMD_TEST,	0x0000,	0x0000,	&u8Tmp,	1) != 0)
								myPrintf("\r\nT_K=(BUSY!)");
						else
						{
								for(u16m=0;	u16m<100; u16m++)
								{
										if((s_test.busTestCode == 0x00) || (s_test.busTestCode == 0xEE))
												break;
										else
												OSTimeDly(100);
								}
								if(u16m < 100)
								{
										if(s_test.busTestCode == 0x00)
												myPrintf("\r\nT_K=(OK!)");
										else if(s_test.busTestCode == 0xEE)
												myPrintf("\r\nT_K=(ERROR!)");
								}
								else
										myPrintf("\r\nT_K=(ERROR-TO!)");	
						}
				}
				// 控制串口测试////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_UART1,	(char *)tmpCode) == 0)
				{
						if(HAL_ConTest() == 1)
								myPrintf("\r\nT_UART1=(OK!)");
						else
								myPrintf("\r\nT_UART1=(ERROR!)");
				}
				// CSQ测试////////////////////////////////////////////////
				else if(strcmp((char *)DBG_CMD_TEST_CSQ,	(char *)tmpCode) == 0)
				{
						if(BSP_QCSQCodePro(&s_gsmApp.sLeve,	&s_gsmApp.rssi,	3000) == DEF_GSM_NONE_ERR)	// 更新当前信号强度
								myPrintf("\r\nT_CSQ=%d(OK!)",s_gsmApp.sLeve);
						else
								myPrintf("\r\nT_CSQ=(BUSY!)");		
				}
				
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// 调试信息打开/关闭控制
				if((strcmp((char *)tmpCode,	(char *)DBG_CMD_SHOW)	>= 0) ||
					 (strcmp((char *)tmpCode,	(char *)DBG_CMD_HIDE)	>= 0))
				{
						// 显示ALL信息 ////////////////////////////////////////////////
						if(strcmp((char *)DBG_CMD_SHOW_ALL,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt = 0xffff;
								myPrintf("\r\nSHOW_ALL(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_ALL,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt = 0x0000;
								myPrintf("\r\nHIDE_ALL(OK!)\r\n");
						}
						// 显示RTC信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_RTC,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_RTC;
								myPrintf("\r\nSHOW_RTC(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_RTC,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_RTC;
								myPrintf("\r\nHIDE_RTC(OK!)\r\n");
						}
						// 显示SYS信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_SYS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_SYS;
								myPrintf("\r\nSHOW_SYS(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_SYS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_SYS;
								myPrintf("\r\nHIDE_SYS(OK!)\r\n");
						}
						// 显示GSM信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_GSM,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_GSM;
								myPrintf("\r\nSHOW_GSM(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_GSM,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_GSM;
								myPrintf("\r\nHIDE_GSM(OK!)\r\n");
						}
						// 显示GSMSTA信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_GSMSTA,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_GSMSTA;
								myPrintf("\r\nSHOW_GSMSTA(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_GSMSTA,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_GSMSTA;
								myPrintf("\r\nHIDE_GSMSTA(OK!)\r\n");
						}
						// 显示GPS信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_GPS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_GPS;
								myPrintf("\r\nSHOW_GPS(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_GPS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_GPS;
								myPrintf("\r\nHIDE_GPS(OK!)\r\n");
						}
						// 显示GPSSTA信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_GPSSTA,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_GPSSTA;
								myPrintf("\r\nSHOW_GPSSTA(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_GPSSTA,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_GPSSTA;
								myPrintf("\r\nHIDE_GPSSTA(OK!)\r\n");
						}
						// 显示CON信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_CON,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_CON;
								myPrintf("\r\nSHOW_CON(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_CON,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_CON;
								myPrintf("\r\nHIDE_CON(OK!)\r\n");
						}
						// 显示OS信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_OS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_OS;
								myPrintf("\r\nSHOW_OS(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_OS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_OS;
								myPrintf("\r\nHIDE_OS(OK!)\r\n");
						}
						// 显示OBD信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_OBD,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_OBD;
								myPrintf("\r\nSHOW_OBD(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_OBD,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_OBD;
								myPrintf("\r\nHIDE_OBD(OK!)\r\n");
						}
						// 显示ADD信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_ADC,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_ADC;
								myPrintf("\r\nSHOW_ADC(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_ADC,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_ADC;
								myPrintf("\r\nHIDE_ADC(OK!)\r\n");
						}
						// 显示ACC信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_ACC,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_ACC;
								myPrintf("\r\nSHOW_ACC(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_ACC,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_ACC;
								myPrintf("\r\nHIDE_ACC(OK!)\r\n");
						}
						// 显示GPRS信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_GPRS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_GPRS;
								myPrintf("\r\nSHOW_GPRS(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_GPRS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_GPRS;
								myPrintf("\r\nHIDE_GPRS(OK!)\r\n");
						}
						// 显示SMS信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_SMS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_SMS;
								myPrintf("\r\nSHOW_SMS(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_SMS,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_SMS;
								myPrintf("\r\nHIDE_SMS(OK!)\r\n");
						}
						// 显示BT信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_BT,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_BT;
								myPrintf("\r\nSHOW_BT(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_BT,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_BT;
								myPrintf("\r\nHIDE_BT(OK!)\r\n");
						}
						// 显示EVENT信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_EVENT,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_EVENT;
								myPrintf("\r\nSHOW_EVENT(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_EVENT,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_EVENT;
								myPrintf("\r\nHIDE_EVENT(OK!)\r\n");
						}
						// 显示OTHER信息 ////////////////////////////////////////////////
						else if(strcmp((char *)DBG_CMD_SHOW_OTHER,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt |= DBG_INFO_OTHER;
								myPrintf("\r\nSHOW_OTHER(OK!)\r\n");				
						}
						else if(strcmp((char *)DBG_CMD_HIDE_OTHER,	(char *)tmpCode) == 0)
						{
								dbgInfoSwt &= ~DBG_INFO_OTHER;
								myPrintf("\r\nHIDE_OTHER(OK!)\r\n");
						}
						// 存储开关状态
						//BSP_BACKUP_WriteDBGWrod(dbgInfoSwt);	// 暂时不做存储
				}
		}

dbgendloop:

		// 重新使能接收
		s_dbgPort.rxFlag =0;
		HAL_DBGRES_IT_CON(DEF_ENABLE);
}
/*
******************************************************************************
* Function Name  : HAL_DBG_Init
* Description    : 
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	HAL_DBG_Init (void)
{
													// 调试口初始化在BSP.C中实现
		IO_RS232_POW_EN();		// 使能232收发器电源
}

/*****************************************end*********************************************************/



