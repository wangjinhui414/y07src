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
*                                      	APPLICATION CODE
*
*                                     	hal_protocol_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : hal_protocol_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

// 说明：
// 支持智信通3.0协议版本

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  HAL_PROTOCOL_GLOBLAS
#include	<string.h>
#include	<stdio.h>
#include  <stdlib.h>
#include 	"ucos_ii.h"
#include	"bsp_h.h"
#include	"bsp_gsm_h.h"
#include	"bsp_gps_h.h"
#include	"bsp_storage_h.h"
#include	"bsp_rtc_h.h"
#include	"hal_h.h"
#include	"cfg_h.h"
#include 	"hal_protocol_h.h"
#include	"hal_processGPS_h.h"
#include	"hal_ProcessSYS_h.h"
#include 	"hal_processDBG_h.h"
#include 	"hal_processQUE_h.h"
#include 	"hal_processACC_h.h"
#include 	"hal_processCON_h.h"
#include	"main_h.h"

#ifdef DEF_OBDTASK_CREATE
#include  "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
#endif

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


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
static			vu16			obdSn=0;		// OBD交互流水号
static			vu16			proSn=0;		// 协议流水号
static			vu16			devSn=0;		// 外设交互流水号
static			vu16			proUpSn=0;	// 上一条指令流水号
static			vu8				igOnSaveBuf[18]={0};	// 点火报文参数保存数组
static      SMS_Typedef simulateSms;

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

uc16 CRC_TABLE_XW[256] =
{
    /* CRC查找表 */
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040,
};

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
extern			OS_EVENT							*GSMGprsDataSem;			// GPRS接收数据信号量
extern			OS_EVENT							*OS_DownACKMbox;			// 设备升级交互应答邮箱
extern			OS_EVENT							*OS_ACKMbox;					// 应用中交互应答邮箱				
extern			OS_EVENT							*OS_OBDReqMbox;				// 请求OBD交互邮箱
extern			OS_EVENT							*OS_RecSmsMbox;				// 收到SMS数据送解析
extern			OS_EVENT							*OS_CONReqMbox;				// 外设控制请求邮箱
extern			SendData_TypeDef			comAck;								// 用于存储通用应答临时缓冲区结构
extern			SMSCon_Typedef				conSms;								// 用于临时存储短信控制内容


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static	u16		UpDevSn 									(void);
static	u16		UpObdSn 									(void);
static	u16		UpDataSn 									(void);
static	u16		RetCmdId 									(u8 type);
static	u16		IsDataType 								(u8 type);
static	u16 	GetCheckCRC_XW						(u8 *lpszData, u16 nLength);
static	u8		ZXT_PackCheck_Process 		(u8 *pBuf,	u16 len);
static	u16		ZXT_PackGetPosi_Process		(u8 num, u16 *packLen,	u8 *pBuf,	u16 len);
static	s8 		ZXT_PackAnalyze_Process 	(u8	*srcBuf,	u16 srcLen,	Protocol_Typedef *pPro);
static	s8 		ZXT_PackBuild_Process 		(u16 sn,	u16 cmdId,	u8 *pPara,	u16 paraLen,	u8 *pOutBuf,	u16 *outLen);


// 服务器下行响应相关子函数
static	s8		naviAckProcess						(Protocol_Typedef *pdu);
static	s8		comAckProcess							(Protocol_Typedef *pdu);
static	s8		upDateProcess							(Protocol_Typedef *pdu);
static	s8		upDateRxdProcess					(Protocol_Typedef *pdu);
static	s8		setIpProcess							(Protocol_Typedef *pdu);
static	s8		setInValTimeProcess				(Protocol_Typedef *pdu);
static	s8		setDevEnProcess						(Protocol_Typedef *pdu);
static	s8		setCarInfoProcess					(Protocol_Typedef *pdu);
static	s8		setDelaySleepProcess			(Protocol_Typedef *pdu);
static	s8		setTimeProcess						(Protocol_Typedef *pdu);
static	s8		setDevThreholdProcess			(Protocol_Typedef *pdu);
static	s8		queDevParaProcess					(Protocol_Typedef *pdu);
static	s8		queSMSConPhoProcess				(Protocol_Typedef *pdu);
static	s8		smsCenterPhoProcess				(Protocol_Typedef *pdu);
static	s8		simCardPhoProcess					(Protocol_Typedef *pdu);
static	s8		setSMSConPhoProcess				(Protocol_Typedef *pdu);
static	s8		setSleepTimeProcess				(Protocol_Typedef *pdu);
static	s8		conTTSProcess							(Protocol_Typedef *pdu);
static	s8		setObdComProcess					(Protocol_Typedef *pdu);

// 数据入队通用处理函数
static	s8		dataInQueProcess					(SendData_TypeDef	*sendData);

			
/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PROTOCOL TOOL
*********************************************************************************************************
*********************************************************************************************************
*/

/*******************************************************************************
* Function Name  : GetProParameter
* Description    : 获取"adc,efg,hlm"字符串格式的参数数据 
* Input          : index表示想获取第几个参数(>=1)
* Output         : 
* Return         : 返回长度信息, 错误范围0xffff
*******************************************************************************/
static	u16	GetProParameter(u8	index,	u8	*srcBuf,	u16 srcLen,	u8 *outBuf)
{
		u8	num=1;
		u16	i=0,p=0;
		
		if(index == 0)
				return 0xffff;
		if(index == 1)
		{
				for(i=0;	(i<srcLen) && (*(srcBuf + i) != ',');	i++)
				{
						*(outBuf + i)	=	*(srcBuf + i);			
				}
				*(outBuf + i) ='\0';	// 添加结束符
				return	i;					
		}
		else
		{	
				for(i=0;	(i<srcLen);	i++)
				{
						if(*(srcBuf + i) == ',')
						{
								num++;
								if(num == index)
								{
										i =i+1;	// 跳过','
										for(p=0;	(i<srcLen) && (*(srcBuf + i) != ',');	i++,p++)
										{
												*(outBuf + p)	=	*(srcBuf + i);			
										}
										*(outBuf + p) ='\0';	// 添加结束符
										return	p;				
								}					
						}			
				}				
		}
		return	0xffff;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PROTOCOL API
*********************************************************************************************************
*********************************************************************************************************
*/


/*******************************************************************************
* Function Name  : GetCheckCRC_XW
* Description    : 获取CRC校验码
* Input          : 
* Output         : 
* Return         : 返回长度信息, 错误范围0xffff
*******************************************************************************/
static	u16 GetCheckCRC_XW(u8 *pData, u16 len)
{
    u16 i=0;
    u16 crc = 0xffff;

    for ( i = 0; i < len; i++ )
    {
        crc = ( crc >> 8 ) ^ CRC_TABLE_XW[( crc ^ pData[i] ) & 0xff];
    }
    return __REV16(crc);	// 取反操作
}
/*******************************************************************************
* Function Name  : ZXT_PackCheck_Process
* Description    : 协议分包检测处理(如果存在粘滞包则改返回值>1)
* Input          : 
* Output         : 
* Return         : 没有:返回0, 正确:返回一帧中所含符合协议格式的数据包个数
*******************************************************************************/
static	u8	ZXT_PackCheck_Process (u8 *pBuf,	u16 len)
{
		u8 packs=0;
		u16 i=0,tmpLen=0,tmpCRC=0;
	
		for(i=0; i<len; i++)
		{
				if((*(pBuf + i) == DEF_ZXT_PRO_START) && (*(pBuf + i + 1) == DEF_ZXT_PRO_PROFLAG))
				{
						tmpLen = (u16)((*(pBuf + i + 2) << 8) | *(pBuf + i + 3));	// 获取协议长度字段
						tmpCRC = (u16)((*(pBuf + i + 4 + tmpLen - 2) << 8) | *(pBuf + i + 4 + tmpLen - 1));
						
						if((tmpLen < 2) || (tmpLen >= 2000))
								continue;
					
						if(tmpCRC == GetCheckCRC_XW((pBuf + i + 4),	(tmpLen - 2)))
						{
								packs++;
								// 跳过本条报文
								i = i + 4 + tmpLen - 1;
						}
						else
						{
								#if(DEF_GPRSINFO_OUTPUTEN > 0)							
								if(dbgInfoSwt & DBG_INFO_GPRS)
										myPrintf("\r\n[GPRS-Rxd]: error crc check!\r\n");
								#endif
						}
						
				}
		}
		return packs;
}
/*******************************************************************************
* Function Name  : ZXT_PackGet_Process
* Description    : 得到一帧数据包中所指定的协议包序列位置(存在粘滞包时使用)
* Input          : posi :数据包位置(第1~255包)
* Output         : 
* Return         : 错误:返回0xffff, 正确:位置
*******************************************************************************/
static	u16	ZXT_PackGetPosi_Process (u8 num, u16 *packLen, u8 *pBuf,	u16 len)
{
		u8 packs=0;
		u16 i=0,tmpLen=0,tmpCRC=0;
	
		for(i=0; i<len; i++)
		{
				if((*(pBuf + i) == DEF_ZXT_PRO_START) && (*(pBuf + i + 1) == DEF_ZXT_PRO_PROFLAG))
				{						
						tmpLen = (u16)((*(pBuf + i + 2) << 8) | *(pBuf + i + 3));	//
						tmpCRC = (u16)((*(pBuf + i + 4 + tmpLen - 2) << 8) | *(pBuf + i + 4 + tmpLen - 1));
					
						if((tmpLen < 2) || (tmpLen >= 2000))
								continue;
								
						if(tmpCRC == GetCheckCRC_XW((pBuf + i + 4),	(tmpLen - 2)))
						{
								packs++;
								if(num == packs)
								{
										*packLen =tmpLen + 4;
										return i;
								}	
								// 跳过本条报文
								i = i + 4 + tmpLen - 1;
						}
				}
		}
		return 0xffff;
}

/*******************************************************************************
* Function Name  : ZXT_PackAnalyze_Process
* Description    : 协议解析函数
* Input          : 
* Output         : 
* Return         : 返回0, 错误返回0xffff
*******************************************************************************/
static	s8 ZXT_PackAnalyze_Process (u8	*srcBuf,	u16 srcLen,	Protocol_Typedef *pPro)
{
		u16 tmpCRC=0;
		u16	tmpLen=0;
	
		if((*srcBuf != DEF_ZXT_PRO_START) || (*(srcBuf + 1) != DEF_ZXT_PRO_PROFLAG))
				return	DEF_PRO_UNPACK_ERRSTART;	// 协议头错误
		
		tmpLen = (u16)((*(srcBuf + 2) << 8) | *(srcBuf + 3));
		if(tmpLen != (srcLen - 4))
				return	DEF_PRO_UNPACK_ERRLEN;	// 协议长度错误
			
		tmpCRC = (u16)((*(srcBuf + srcLen - 2) << 8) | *(srcBuf + srcLen - 1));
		if(tmpCRC != GetCheckCRC_XW((srcBuf + 4),	(srcLen - 6)))
				return	DEF_PRO_UNPACK_ERRCRC;	// CRC校验错误
		
		// 开始获取协议数据
		pPro->start 		=*srcBuf;
		pPro->proFlag		=*(srcBuf + 1);
		pPro->proLen		=tmpLen;
		pPro->bodySn		=(u16)((*(srcBuf + 4) << 8) | *(srcBuf + 5));
		pPro->bodycmdId	=(u16)((*(srcBuf + 6) << 8) | *(srcBuf + 7));
		if((srcLen > 10) && (srcLen <= DEF_PARABUF_SIZE))
		{
				// 获取参数区内容
				memcpy((pPro->paraData),	(srcBuf + 8),	(srcLen - 10));
		}
		pPro->paraLen = srcLen - 10;	// 获取参数长度
		pPro->crcCode = tmpCRC;
		
		return	DEF_PRO_UNPACK_NONE;
}
/*******************************************************************************
* Function Name  : ZXT_PackBuild_Process
* Description    : 通用协议打包函数
* Input          : 
* Output         : 
* Return         : 返回0, 错误返回0xffff
*******************************************************************************/
static	s8 ZXT_PackBuild_Process (u16 sn,	u16 cmdId,	u8 *pPara,	u16 paraLen,	u8 *pOutBuf,	u16 *outLen)
{
		u16	i=0,p=0,tmpCRC=0,tmpLen=0;
		
		tmpLen = 6 + paraLen;
		*pOutBuf 					= DEF_ZXT_PRO_START;		p +=1;	// 协议头		
		*(pOutBuf + p) 		= DEF_ZXT_PRO_PROFLAG;	p +=1;	// 消息标识
	  *(pOutBuf + p)		= (u8)((tmpLen & 0xff00) >> 8);		p +=1;	// 长度高赋值
		*(pOutBuf + p)		= (u8)(tmpLen & 0x00ff);					p +=1;	// 长度低赋值
	  *(pOutBuf + p)		= (u8)((sn & 0xff00) >> 8);		p +=1;	// 赋值流水号高
		*(pOutBuf + p)		= (u8)(sn & 0x00ff);		p +=1;	// 赋值流水号低
		*(pOutBuf + p)		= (u8)((cmdId & 0xff00) >> 8);		p +=1;	// 赋值参数ID高
		*(pOutBuf + p)		= (u8)(cmdId & 0x00ff);		p +=1;	// 赋值参数ID低
		
		// 赋值参数区内容
		if(paraLen != 0)
		{
				for(i=0;	i<paraLen;	i++,p++)
				{
						*(pOutBuf + p) = *(pPara + i);				
				}
		}
		tmpCRC = GetCheckCRC_XW((pOutBuf + 4),	(paraLen + 4));	
		*(pOutBuf + p)    = (u8)(tmpCRC >> 8);		p +=1;	// CRC1
		*(pOutBuf + p)    = (u8)tmpCRC;						p +=1;	// CRC2
		*outLen = p;
		return DEF_PRO_MKPACK_NONE;	
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PROTOCOL LOGIN FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/

/*******************************************************************************
* Function Name  : HAL_BuildNavi_Process
* Description    : 请求分配IP数据打包 
* Input          : 
* Output         : 
* Return         : 返回0, 错误返回0xffff
*******************************************************************************/
s8 HAL_BuildNavi_Process (u8 *pOutBuf,	u16 *outLen)
{
		u8 tmpPara[60]={0};
		u16 paraLen=0,i=0;	
		u32	filename = FLASH_CPU_APPNAME_START;
		
		// 参数内容添加	
		// 设备ID(IDC)
		memcpy((tmpPara + paraLen),	s_common.id,	PRA_SIZEOF_COMMON_ID);
		paraLen	+=PRA_SIZEOF_COMMON_ID;
		
		// 车型车系发送机号
		memcpy((tmpPara + paraLen),	s_common.carInfo,	PRA_SIZEOF_COMMON_CARINFO);
		paraLen	+=PRA_SIZEOF_COMMON_CARINFO;
		
		// 网络版本号
		for(i=0;	i<5;	i++)
		{
				tmpPara[paraLen + i] =*(u8*)(filename + i);
		}
		paraLen	+=5;
		// TEST
		//memcpy((tmpPara + paraLen),	"99.99",	5);
		//paraLen	+=5;
		// TEST
		
		return ZXT_PackBuild_Process (0x0000,	DEF_CMDID_NAVI_DEV,	tmpPara, paraLen,	pOutBuf,	outLen);
}
/*******************************************************************************
* Function Name  : HAL_BulidLogin_Process
* Description    : 握手打包函数
* Input          : 
* Output         : 
* Return         : 返回0, 错误返回0xffff
*******************************************************************************/
s8 HAL_BulidLogin_Process (u8 *pOutBuf,	u16 *outLen, u16 *retSn)
{
		u8 tmpPara[60]={0},i=0,tmpBuf[20]={0};
		u16 paraLen=0,p=0,tmpSn=0;
		u32	filename = FLASH_CPU_APPNAME_START,count=0;
		SYS_DATETIME	tmpRtc;
	
		memset(tmpPara,	0,	sizeof(tmpPara));	
		memset(tmpBuf,	0,	sizeof(tmpBuf));
		memset((u8 *)&tmpRtc,	0,	sizeof(tmpRtc));
		
		// 参数内容添加	
		// 通信协议版本
		tmpPara[paraLen] = 0x03;		// 2015-10-16 3.0协议版本
		paraLen++;
		
		// 厂商ID
		tmpPara[paraLen] = DEF_COMPANY_ID;
		paraLen++;
	
		// 设备ID(IDC)
		memcpy((tmpPara + paraLen),	s_common.id,	PRA_SIZEOF_COMMON_ID);
		paraLen	+=PRA_SIZEOF_COMMON_ID;
	
		// 车型,车系,发送机号
		memcpy((tmpPara + paraLen),	s_common.carInfo,	PRA_SIZEOF_COMMON_CARINFO);
		paraLen	+=PRA_SIZEOF_COMMON_CARINFO;		
		
		// 网络版本号
		for(i=0;	i<5;	i++)
		{
				tmpPara[paraLen + i] =*(u8*)(filename + i);
		}
		paraLen	+=5;
		
		// SIM卡号码新握手方式
		if((strlen((const char *)s_gsmApp.simNum) == 0) || (strlen((const char *)s_gsmApp.simNum) >= sizeof(s_gsmApp.simNum)))
		{
				// 号码长度为0或大于最大长度(20)都被认为是非法值
				tmpPara[paraLen] =0x00;		// 长度字节写0
				paraLen++;
		}
		else
		{
				tmpPara[paraLen] =strlen((const char *)s_gsmApp.simNum);		// 长度字节
				paraLen++;
				memcpy((tmpPara + paraLen),	s_gsmApp.simNum,	strlen((const char *)s_gsmApp.simNum));	
				paraLen	+=strlen((const char *)s_gsmApp.simNum);
		}
		
		// 时间戳
		BSP_RTC_Get_Current(&tmpRtc);	
		count =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);			
		tmpPara[paraLen] =(u8)(count >> 24);	
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 16);
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 8);
		paraLen++;
		tmpPara[paraLen] =(u8)(count);
		paraLen++;
	
		// 状态长度
		tmpPara[paraLen] = 0x05;
		paraLen++;
		
		// TTS状态(高4位开关1-开，低4位支持状态1-不支持)
		if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		tmpPara[paraLen] &= 0xf0;	// SIM800内含功能 
		paraLen++;
		
		// GPS状态(高4位开关-开，低4位支持状态1-不支持)
		if(s_cfg.devEn & DEF_DEVEN_BIT_GPS)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		if(GPS_DEVICE.InitSta == 1)
				tmpPara[paraLen] &= 0xf0;		
		else
				tmpPara[paraLen] |= 0x01;
		paraLen++;
		
		// BT状态(高4位控制模块类别，低4位支持状态1-不支持)
		if(s_cfg.devEn & DEF_DEVEN_BIT_BT)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		tmpPara[paraLen] &= 0xf0;	// SIM800内含功能 
		paraLen++;
		
		// 控制模块状态(高4位开关，低4位支持状态1-不支持)
		if(s_cfg.devEn & DEF_DEVEN_BIT_CON)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		if(CON_DEVICE.InitSta != 0)
				tmpPara[paraLen] &= 0xf0;	// 外设控制设备
		else
				tmpPara[paraLen] |= 0x01;		
		paraLen++;
		
		// SHAKE状态(高4位开关状态，低4位支持状态1-不支持)
		if(s_cfg.devEn & DEF_DEVEN_BIT_SHAKE)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		if(MEMS_DEVICE.InitSta != 0)
				tmpPara[paraLen] &= 0xf0;	// 加速度芯片有效
		else
				tmpPara[paraLen] |= 0x01;	
		paraLen++;
			
		tmpSn =UpDataSn();
		*retSn =tmpSn;
		return ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_LOGIN_DEV,	tmpPara,	paraLen,	pOutBuf,	outLen);
}
/*******************************************************************************
* Function Name  : HAL_BulidComPack_Process
* Description    : 心跳包，断网，睡眠进入数据打包
* Input          : 
* Output         : 
* Return         : 返回0, 错误返回0xffff
*******************************************************************************/
s8 HAL_BulidComPack_Process (u16	cmdId,	u8 *pOutBuf,	u16 *outLen, u16 *retSn)
{
		u16	tmpSn=0;
		tmpSn =UpDataSn();
		*retSn =tmpSn;
		return ZXT_PackBuild_Process (tmpSn,	cmdId,	(u8 *)"",	0,	pOutBuf,	outLen);
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PROTOCOL MAIN FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : UpDataSn
* Description    : 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
static	u16	UpDataSn (void)
{
		if(proSn >= 0xffff)
				proSn =1;	
		else
				proSn ++;
		return	proSn;   
}
/*
******************************************************************************
* Function Name  : UpObdSn
* Description    : OBD交互控制流水号
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
static	u16	UpObdSn (void)
{
		if(obdSn >= 0xffff)
				obdSn =1;	
		else
				obdSn ++;
		return	obdSn;   
}
/*
******************************************************************************
* Function Name  : UpDevSn
* Description    : DEV交互控制流水号
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
static	u16	UpDevSn (void)
{
		if(devSn >= 0xffff)
				devSn =1;	
		else
				devSn ++;
		return	devSn;   
}
/*
******************************************************************************
* Function Name  : RetCmdId
* Description    : 输入参数类型发回命令ID
* Input          : type:数据类型DEF_TYPE_ACK~DEF_TYPE_CARSTA
* Output         : None
* Return         : 不匹配返回0xffff
******************************************************************************
*/
static	u16	RetCmdId (u8 type)
{
		u16 cmdId=0;
		switch(type)
		{
				case DEF_TYPE_ACK				:	cmdId	=DEF_CMDID_COMACK_DEV;		break;		
				case DEF_TYPE_GPS				:	cmdId	=DEF_CMDID_GPS_DEV;				break;
				case DEF_TYPE_IG				:	cmdId	=DEF_CMDID_IG_DEV;				break;
				case DEF_TYPE_ZITAI			:	cmdId	=DEF_CMDID_ZITAI_DEV;			break;
				case DEF_TYPE_LOG				:	cmdId	=DEF_CMDID_ERRORLOG_DEV;	break;
				case DEF_TYPE_ALARM			:	cmdId	=DEF_CMDID_ALARM_DEV;			break;
				case DEF_TYPE_24REPORT	:	cmdId	=DEF_CMDID_HEALTH_DEV;		break;
				case DEF_TYPE_QUEPARA		:	cmdId	=DEF_CMDID_PARAACK_DEV;		break;
				case DEF_TYPE_VIN				:	cmdId	=DEF_CMDID_VIN_DEV;				break;
				case DEF_TYPE_CARCON		:	cmdId	=DEF_CMDID_CONCARACK_DEV;	break;
				case DEF_TYPE_CLRCODE		:	cmdId	=DEF_CMDID_CLRCODEACK_DEV;break;
				case DEF_TYPE_CARSTA		:	cmdId	=DEF_CMDID_QUECARACK_DEV;	break;
				case DEF_TYPE_QUEPHO		:	cmdId	=DEF_CMDID_PHONUMACK_DEV;	break;
				case DEF_TYPE_SMSCOM		:	cmdId	=DEF_CMDID_SMSCON_DEV;		break;
				case DEF_TYPE_POWON			:	cmdId	=DEF_CMDID_POWON_DEV;			break;
			
				default	: cmdId = 0xffff;	break;	
		}
		return	cmdId;
}
/*
******************************************************************************
* Function Name  : IsDataType
* Description    : 判断输入的数据类型是否正确
* Input          : type:数据类型DEF_TYPE_ACK~DEF_TYPE_CARSTA
* Output         : None
* Return         : 错误返回 0xffff，正确返回 0
******************************************************************************
*/
static	u16	IsDataType (u8 type)
{
		u16 ret=0xffff;
		switch(type)
		{
				case DEF_TYPE_ACK				:	ret=0;		break;		
				case DEF_TYPE_GPS				:	ret=0;		break;
				case DEF_TYPE_IG				:	ret=0;		break;
				case DEF_TYPE_ZITAI			:	ret=0;		break;
				case DEF_TYPE_LOG				:	ret=0;		break;
				case DEF_TYPE_ALARM			:	ret=0;		break;
				case DEF_TYPE_24REPORT	:	ret=0;		break;
				case DEF_TYPE_QUEPARA		:	ret=0;		break;
				case DEF_TYPE_VIN				:	ret=0;		break;
				case DEF_TYPE_CARCON		:	ret=0;		break;
				case DEF_TYPE_CLRCODE		:	ret=0;		break;
				case DEF_TYPE_CARSTA		:	ret=0;		break;
				case DEF_TYPE_QUEPHO		:	ret=0;		break;
				case DEF_TYPE_SMSCOM		:	ret=0;		break;
				case DEF_TYPE_POWON			:	ret=0;		break;
			
				default	: ret=0xffff;	break;	
		}
		return	ret;
}
/*
*********************************************************************************************************
*                                           HAL_ProUnpackMain()
*
* Description : 用于GPRS接收数据的协议解析.
*
* Argument(s) : 
*
* Return(s)   : none.
*
* Caller(s)   : app.
*
* Note(s)     : none.
*********************************************************************************************************
*/
s8	HAL_ProUnpackMain	(u8	*pBuf,	u16	len)
{
		s8 ret =0;
		u8 result=0, packs=0, cun=1;
		u16 tmpLen=0,tmpPosi=0;
		Protocol_Typedef pdu;
		SendData_TypeDef tmpAck;	
			
		// 协议粘滞报文检测//////////////////////
		packs =ZXT_PackCheck_Process (pBuf,	len);		// 检测是否有粘滞包出现
		if(packs == 0)
		{
				s_update.writeLock =0;	// 释放数据锁
				return -1;
		}
		else
		{
				if(packs > 1)
				{
						// 打印GPRS接收发生的粘滞包信息
						#if(DEF_GPRSINFO_OUTPUTEN > 0)							
						if(dbgInfoSwt & DBG_INFO_GPRS)
								myPrintf("\r\n[GPRS-Rxd]: %d packs in one receive!!\r\n",	packs);
						#endif				
				}
		}
		
		for(cun=1;	cun <= packs;	cun++)
		{
				tmpLen=0; tmpPosi=0;
				memset((u8 *)&pdu,	0,	sizeof(Protocol_Typedef));	
				memset((u8 *)&tmpAck,	0,	sizeof(SendData_TypeDef));
					  
				tmpPosi =ZXT_PackGetPosi_Process(cun, &tmpLen,	pBuf,	len);
				if(tmpPosi  == 0xffff)
				{
						s_update.writeLock =0;	// 释放数据锁
						return -1;
				}
				
				// 协议解析//////////////////////////////	
				ret =ZXT_PackAnalyze_Process ((pBuf+tmpPosi),	tmpLen,	&pdu);	
				if(ret != DEF_PRO_UNPACK_NONE)
				{
						s_update.writeLock =0;	// 释放数据锁
						return -1;
				}
				
				// 判断指令合法性////////////////////////
				if(pdu.bodycmdId != DEF_CMDID_COMACK_PLA)
				{
						if(pdu.bodySn != proUpSn)
								proUpSn =pdu.bodySn;
						//else
						//		return	-1;
				}			
				
				// 协议执行//////////////////////////////
				switch(pdu.bodycmdId)
				{
						case		DEF_CMDID_NAVI_PLA				  :
										ret =naviAckProcess(&pdu);					// 服务器下发-导航IP获取应答
										break;
						case		DEF_CMDID_COMACK_PLA				:
										if(s_common.iapSta != DEF_UPDATE_KEYWORD)
												ret =comAckProcess(&pdu);				// 服务器下发-通用应答	
										else
												ret =upDateRxdProcess(&pdu);		// 服务器下发-升级过程中的通用应答	
										break;
						case		DEF_CMDID_UPDATE_PLA				:
										ret =upDateProcess(&pdu);						// 服务器下发-设备升级 
										break;
						case		DEF_CMDID_UPDATEINFO_PLA		:	
										ret =upDateRxdProcess(&pdu);				// 服务器下发-升级信息
										break;
						case		DEF_CMDID_UPDATEDATA_PLA		:
										ret =upDateRxdProcess(&pdu);				// 服务器下发-升级文件
										break;
						case		DEF_CMDID_UPDATEQUE_PLA			:
										ret =upDateRxdProcess(&pdu);				// 服务器下发-升级成功查询
										break;
						case		DEF_CMDID_SETIP_PLA					:
										ret =setIpProcess(&pdu);						// 服务器下发-设置IP地址
										break;
						case		DEF_CMDID_SETODO_PLA				:
										ret =setObdComProcess(&pdu);				// 服务器下发-初始化里程(OBD)
										break;
						case		DEF_CMDID_DEVRESET_PLA			:
										s_osTmp.resetReq = DEF_RESET_PRI0;	// 服务器下发-设备重启
										ret =0;
										break;				
						case		DEF_CMDID_GPSUPTIME_PLA			:
										ret =setInValTimeProcess(&pdu);			// 服务器下发-设备GPS采集间隔												
										break;
						case		DEF_CMDID_MODULESWT_PLA			:
										ret =setDevEnProcess(&pdu);					// 服务器下发-设置设备模块开关指令
										break;
						case		DEF_CMDID_ENGINEBRAND_PLA		:
										ret =setCarInfoProcess(&pdu);				// 服务器下发-发动机品牌
										break;
						case		DEF_CMDID_POWOFFDELAY_PLA		:
										ret =setDelaySleepProcess(&pdu);		// 服务器下发-延时开关机(进入睡眠)
										break;
						case		DEF_CMDID_SETTIME_PLA				:
										ret =setTimeProcess(&pdu);					// 服务器下发-时间下发指令
										break;
						case		DEF_CMDID_SETPAILIANG_PLA		:
										ret =setObdComProcess(&pdu);				// 服务器下发-发动机排量(OBD)
										break;						
						case		DEF_CMDID_SETTHRESHOLD_PLA	:
										ret =setDevThreholdProcess(&pdu);		// 服务器下发-设备阀值设置
										break;
						case		DEF_CMDID_QUEPARA_PLA				:
										ret =0;															// 服务器下发-设备参数查询(异步数据返回由下面完成打包)
										break;
						case		DEF_CMDID_SMSCENTERPHO_PLA	:
										ret =smsCenterPhoProcess(&pdu);			// 服务器下发-短信服务器中心号码
										break;
						case		DEF_CMDID_SIMCARDPHO_PLA	:
										ret =simCardPhoProcess(&pdu);				// 服务器下发-设备SIM卡号码
										break;	
						case		DEF_CMDID_SETPHONUM_PLA	:
										ret =setSMSConPhoProcess(&pdu);			// 服务器下发-设置合法短信控制号码
										break;	
						case		DEF_CMDID_QUEPHONUM_PLA			:
										ret =0;															// 服务器下发-查询合法短信控制号码
										break;
						case		DEF_CMDID_SETSLEEPTIME_PLA	:
										ret =setSleepTimeProcess(&pdu);			// 服务器下发-设备睡眠时间及健康包上传时间间隔
										break;					
						case		DEF_CMDID_RTSTART_PLA				:
										ret =setObdComProcess(&pdu);				// 服务器下发-专家坐席实时数据流开启(OBD)
										break;
						case		DEF_CMDID_RTSTOP_PLA				:
										ret =setObdComProcess(&pdu);				// 服务器下发-专家坐席实时数据流停止(OBD)
										break;
						case		DEF_CMDID_STARTTJ_PLA				:
										ret =setObdComProcess(&pdu);				// 服务器下发-开始手动体检(OBD)
										break;
						case		DEF_CMDID_READFUEL_PLA				:
										ret =setObdComProcess(&pdu);				// 服务器下发-查询剩余油量
										break;
						case		DEF_CMDID_CONCAR_PLA				:
										ret =setObdComProcess(&pdu);				// 服务器下发-车辆控制(通过OBD或外部控制模块)
										break;
						case		DEF_CMDID_CONTTS_PLA				:
										ret =conTTSProcess(&pdu);						// 服务器下发-TTS控制
										break;
						case		DEF_CMDID_CLRCODE_PLA				:
										ret =setObdComProcess(&pdu);				// 服务器下发-清码指令(OBD)
										break;
						case		DEF_CMDID_QUECAR_PLA				:
										if(s_common.carInfo[0] == 'O')
												ret =-1;												// O系车通用应答直接返回错误
										else
												ret =setObdComProcess(&pdu);		// 服务器下发-查询车辆状态(通过OBD或外部控制模块)
										break;
						default : break;  
				}
				
				// 通用应答打包(通用应答及导航回复报文无需通用应答)///////////////////
				if((pdu.bodycmdId != DEF_CMDID_COMACK_PLA) && (pdu.bodycmdId != DEF_CMDID_NAVI_PLA))
				{
						// 应答结果//////////////////////////////
						if(ret == 0)
								result = DEF_ACKTYPE_OK;
						else if(ret == -2)
								result = DEF_ACKTYPE_BUSY;	
            else
								result = DEF_ACKTYPE_ERR;
						
						// 通用应答数据放在通用应啊发送结构存储,且如有新数据替换之前的数据
						// (通用应答具有时效性如果收到指令后网络断开了,则不做应答)
						memset((u8 *)&comAck,	0,	sizeof(comAck));
						tmpAck.sum	= 0;		// RAM队列不校验数据合法性
						tmpAck.path = DEF_PATH_GPRS;
						tmpAck.type = DEF_TYPE_ACK;
						tmpAck.len  = 3;
						tmpAck.data[0] = (u8)(pdu.bodySn >> 8);	
						tmpAck.data[1] = (u8)(pdu.bodySn);	
						tmpAck.data[2] = result;	
						memcpy((u8 *)&comAck,	(u8 *)&tmpAck,	sizeof(tmpAck));
				}
				// 设备参数查询异步打包
				if(pdu.bodycmdId == DEF_CMDID_QUEPARA_PLA)
				{
						ret =queDevParaProcess(&pdu);	
				}	
				// 设备短信控制号码查询异步打包
				if(pdu.bodycmdId == DEF_CMDID_QUEPHONUM_PLA)
				{
						ret =queSMSConPhoProcess(&pdu);	
				}
//				// 设置SIM卡号码后需要复位网络触发重新握手报文
//				if(pdu.bodycmdId == DEF_CMDID_SIMCARDPHO_PLA)
//				BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
		}
		
		return	DEF_PRO_UNPACK_NONE;		
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PROTOCOL SUB FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/

/*******************************************************************************
* Function Name  : dataInQueProcess
* Description    : 数据入队通用算法处理
* Input          : SendData_TypeDef * ：数据入队结构体指针
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	dataInQueProcess	(SendData_TypeDef	*sendData)
{
		u8	path=0;
		s8	ret =0;
	
		// 数据入发送队列///////////////////////
		if(HAL_ComQue_size (DEF_FLASH_QUE) != 0)
		{
				// FLASH里有数据直接入到FLASH队列里
				ret =HAL_ComQue_in (DEF_FLASH_QUE,	sendData);	
				path =1;
		}
		else
		{
				// 2015-9-28如果为点火熄火报文直接存FLASH
				if((sendData->type == DEF_TYPE_IG) || (1u == HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE)))
				{
						ret =HAL_ComQue_in (DEF_FLASH_QUE,	sendData);
						path =1;
				}
				else
				{					
						// 如果设备在网RAM队列为空直接放到RAM队列否则直接入FLASH队列
						if((HAL_ComQue_size (DEF_RAM_QUE) == 0) && (BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1))
						{
								ret =HAL_ComQue_in (DEF_RAM_QUE,	sendData);	
								path =0;
						}
						else
						{
								ret =HAL_ComQue_in (DEF_FLASH_QUE,	sendData);
								path =1;
						}
				}
		}
		// 入队错误处理
		if((ret != DEF_PROQUE_QUE_OK) && (path == 1))
		{
				if(APP_ErrQue (&s_err.spiFlashSumTimes) >= 33)
				{						
						// 只有连续的错误累计达到33以上才强制复位队列
						ret =0;
						ret =HAL_ComQue_ForceReset(DEF_FLASH_QUE);
						if(ret == DEF_PROQUE_QUE_OK)
						{
								APP_ErrClr (&s_err.spiFlashSumTimes);			// 只有正确的初始化后才复位错误变量否则将一直尝试
								#if(DEF_MEMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_OTHER)
										myPrintf("[FLASH]: SPI queue force reset success!!\r\n",	s_err.spiFlashSumTimes);
								#endif	
						}
						else
						{	
								#if(DEF_MEMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_OTHER)
										myPrintf("[FLASH]: SPI queue abnormal=%d, queue will force reset!!\r\n",	s_err.spiFlashSumTimes);
								#endif	
						}								
				}					
				else
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
								myPrintf("[FLASH]: SPI queue abnormal=%d!!\r\n",	s_err.spiFlashSumTimes);
						#endif	
				}

				if(sendData->type != DEF_TYPE_GPS)
						ret =HAL_ComQue_in (DEF_RAM_QUE,	sendData);		// 2015-1-6 by:gaofei FLASH数据存储失败再放到临时发送队列中(不包括GPS数据)
		}
		return ret;
}


/*******************************************************************************
* Function Name  : naviAckProcess
* Description    : 服务器下发的IP获取应答
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/

static	s8	naviAckProcess	(Protocol_Typedef *pdu)
{
		s8	ret=0;
		u16 tmpLen1=0,tmpLen2=0,tmpLen3=0,tmpLen4=0,tmpLen5=0;
		u8	tmpIp[20]={0},tmpPort[8]={0},tmpAPN[20]={0},tmpUser[8]={0},tmpPass[8]={0};
	
		if(pdu->paraLen == 0)
				return -1;
		
		memset((u8 *)&s_ack,	0,	sizeof(s_ack));
		memset(tmpIp,		'\0',	sizeof(tmpIp));
		memset(tmpPort,	'\0',	sizeof(tmpPort));
		memset(tmpAPN,	'\0',	sizeof(tmpAPN));
		memset(tmpUser,	'\0',	sizeof(tmpUser));
		memset(tmpPass,	'\0',	sizeof(tmpPass));
		
		tmpLen1 =GetProParameter(1,	pdu->paraData+1,	pdu->paraData[0],	tmpIp);		// 参数区加1跳过长度字节
		tmpLen2 =GetProParameter(2,	pdu->paraData+1,	pdu->paraData[0],	tmpPort);
		tmpLen3 =GetProParameter(3,	pdu->paraData+1,	pdu->paraData[0],	tmpAPN);
		tmpLen4 =GetProParameter(4,	pdu->paraData+1,	pdu->paraData[0],	tmpUser);
		tmpLen5 =GetProParameter(5,	pdu->paraData+1,	pdu->paraData[0],	tmpPass);

//		if(s_cfg.testapp2)//WJH_TEST启用了测试
//		{
//				memcpy(tmpIp,"123.207.144.45",strlen("123.207.144.45")+1);
//				memcpy(tmpPort,"4000",strlen("4000")+1);
//	  }
		myPrintf("IP:%s ,PORT:%s\r\n",tmpIp,tmpPort);
		if((tmpLen1 != 0xffff) && (tmpLen2 != 0xffff) && (tmpLen3 != 0xffff) && (tmpLen4 != 0xffff) && (tmpLen5 != 0xffff)) 
		{
        HAL_SysParaReload();
				if(strlen((const char *)tmpIp) <= PRA_SIZEOF_SYSCFG_IP2)
						strcpy((char *)s_cfg.ip2,		(const char *)tmpIp);
				if(strlen((const char *)tmpPort) <= PRA_SIZEOF_SYSCFG_PORT2)
						strcpy((char *)s_cfg.port2,	(const char *)tmpPort);
				if(strlen((const char *)tmpAPN) <= PRA_SIZEOF_SYSCFG_APN)
						strcpy((char *)s_cfg.apn,		(const char *)tmpAPN);
				if(strlen((const char *)tmpUser) <= PRA_SIZEOF_SYSCFG_APNUSER)
						strcpy((char *)s_cfg.apnUser,		(const char *)tmpUser);
				if(strlen((const char *)tmpPass) <= PRA_SIZEOF_SYSCFG_APNPASS)
						strcpy((char *)s_cfg.apnPass,		(const char *)tmpPass);
				if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
						ret =0;
				else
						ret =-1;
		}
		else
				ret =-1;
		
		// 发送应答邮箱
		s_ack.msgId		=pdu->bodycmdId;	
		s_ack.ackSn 	=pdu->bodySn;	
		if(ret == 0)
				s_ack.result	=DEF_ACKTYPE_OK;
		else
				s_ack.result	=DEF_ACKTYPE_ERR;
	
		OSMboxPost(OS_ACKMbox, (void *)&s_ack); // 发送应答包  
		
		return ret;
}
/*******************************************************************************
* Function Name  : comAckProcess
* Description    : 服务器下发的通用应答处理(发送应答邮箱到发送任务)
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	comAckProcess	(Protocol_Typedef *pdu)
{
		if(pdu->paraLen == 0)
				return -1;
		
		memset((u8 *)&s_ack,	0,	sizeof(s_ack));
		s_ack.msgId		=pdu->bodycmdId;	
		s_ack.ackSn 	=(u16)((pdu->paraData[0] << 8) | pdu->paraData[1]);
		s_ack.result	=pdu->paraData[2];			
		OSMboxPost(OS_ACKMbox, (void *)&s_ack); // 发送应答包  
		
    return  0;
}
/*******************************************************************************
* Function Name  : upDateProcess
* Description    : 服务器下发的设备升级处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	upDateProcess	(Protocol_Typedef *pdu)
{
		if(*(s_devSta.pFlash)	!=	DEF_FLASHHARD_NONE_ERR)
				return	-1;		// 外部FLASH故障禁止升级操作
		
		//if(s_common.iapSta != 0)
		if((s_common.iapSta == DEF_UPDATE_WAIT) || (s_common.iapSta == DEF_UPDATE_KEYWORD))
				return	-1;		// 升级已经在处理中
		
		s_osTmp.upFlag		=1;	// 允许升级等待
		s_common.iapSta  	=DEF_UPDATE_WAIT;	// 设置升级等待中间状态,为了同步临时缓冲区数据
		OSTmr_Count_Start(&s_osTmp.upWaitCounter);
		
		s_update.dataLen 	=0;
		memset(s_update.dataBuf,	0,	DEF_UPDATEBUF_SIZE);
		
		return 0;
}
/*******************************************************************************
* Function Name  : upDateRxdProcess
* Description    : 升级过程中服务器下发的消息处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	upDateRxdProcess	(Protocol_Typedef *pdu)
{
		// 升级过程中未避免数据覆盖需要判断写使能锁
		s_update.dataLen 	=0;
		memset(s_update.dataBuf,	0,	DEF_UPDATEBUF_SIZE);
		s_update.msgId		=pdu->bodycmdId;	
		s_update.sn 			=pdu->bodySn;
		if(pdu->paraLen <= DEF_UPDATEBUF_SIZE)
		{
				memcpy(s_update.dataBuf,	pdu->paraData,	pdu->paraLen);
				s_update.dataLen  =pdu->paraLen;
		}
		else
				s_update.dataLen =0;
		OSMboxPost(OS_DownACKMbox, (void *)&s_update); // 发送应答包  
		return 0;
}
/*******************************************************************************
* Function Name  : setIpProcess
* Description    : 服务器下发设置IP地址处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	setIpProcess	(Protocol_Typedef *pdu)
{
		u16 tmpLen1=0,tmpLen2=0,tmpLen3=0,tmpLen4=0,tmpLen5=0;
		u8	tmpIp[20]={0},tmpPort[8]={0},tmpAPN[20]={0},tmpUser[8]={0},tmpPass[8]={0};
	
		if(pdu->paraLen == 0)
				return -1;
		
		memset(tmpIp,		'\0',	sizeof(tmpIp));
		memset(tmpPort,	'\0',	sizeof(tmpPort));
		memset(tmpAPN,	'\0',	sizeof(tmpAPN));
		memset(tmpUser,	'\0',	sizeof(tmpUser));
		memset(tmpPass,	'\0',	sizeof(tmpPass));
		
		tmpLen1 =GetProParameter(1,	pdu->paraData+1,	pdu->paraData[0],	tmpIp);		// 参数区加1跳过长度字节
		if((tmpLen1 == 0xffff) || (tmpLen1 == 0) || (tmpLen1 > PRA_SIZEOF_SYSCFG_IP1))
				return -1;
		tmpLen2 =GetProParameter(2,	pdu->paraData+1,	pdu->paraData[0],	tmpPort);
		if((tmpLen2 == 0xffff) || (tmpLen2 == 0) || (tmpLen2 > PRA_SIZEOF_SYSCFG_PORT1) || (IsIntString(tmpPort,	tmpLen2) == 0xffff) || (atoi((const char*)tmpPort) > 65535))
				return -1;
		tmpLen3 =GetProParameter(3,	pdu->paraData+1,	pdu->paraData[0],	tmpAPN);
		if((tmpLen3 == 0xffff) || (tmpLen3 == 0) || (tmpLen3 > PRA_SIZEOF_SYSCFG_APN))
				return -1;
		tmpLen4 =GetProParameter(4,	pdu->paraData+1,	pdu->paraData[0],	tmpUser);
		if((tmpLen4 == 0xffff) || (tmpLen4 > PRA_SIZEOF_SYSCFG_APNUSER))
				return -1;
		tmpLen5 =GetProParameter(5,	pdu->paraData+1,	pdu->paraData[0],	tmpPass);
		if((tmpLen5 == 0xffff) || (tmpLen5 > PRA_SIZEOF_SYSCFG_APNPASS))
				return -1;
		
		// 参数合法性判断
    HAL_SysParaReload();
		strcpy((char *)s_cfg.ip1,				(const char *)tmpIp);
		strcpy((char *)s_cfg.port1,			(const char *)tmpPort);
		strcpy((char *)s_cfg.apn,				(const char *)tmpAPN);
		strcpy((char *)s_cfg.apnUser,		(const char *)tmpUser);
		strcpy((char *)s_cfg.apnPass,		(const char *)tmpPass);
		if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
		{
				s_osTmp.resetReq = DEF_RESET_PRI0;	// 设置复位标志
				return 0;
		}
		else
				return -1;
}
/*******************************************************************************
* Function Name  : setInValTimeProcess
* Description    : 服务器下发设置采集时间处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	setInValTimeProcess	(Protocol_Typedef *pdu)
{
		if(pdu->paraLen != 1)
				return -1;	
		
    HAL_SysParaReload();
		s_cfg.gpsCollectTime	=pdu->paraData[0];
		if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
				return	0;
		else
				return	-1;
}
/*******************************************************************************
* Function Name  : setDevEnProcess
* Description    : 服务器下发设置模块开关指令处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	setDevEnProcess	(Protocol_Typedef *pdu)
{
		if(pdu->paraLen != 2)
				return -1;	

    HAL_SysParaReload();
		if(pdu->paraData[1] == 0x01)
		{
				// 模块功能开
				if(pdu->paraData[0] == 0x00)
						s_cfg.devEn	|= DEF_DEVEN_BIT_TTS;
				else if(pdu->paraData[0] == 0x01)
						s_cfg.devEn	|= DEF_DEVEN_BIT_GPS;
				else if(pdu->paraData[0] == 0x02)
						s_cfg.devEn	|= DEF_DEVEN_BIT_BT;
				else if(pdu->paraData[0] == 0x03)
						s_cfg.devEn	|= DEF_DEVEN_BIT_SHAKE;
		}
		else if(pdu->paraData[1] == 0x00)
		{
				// 模块功能关
				if(pdu->paraData[0] == 0x00)
						s_cfg.devEn	&= ~DEF_DEVEN_BIT_TTS;
				else if(pdu->paraData[0] == 0x01)
						s_cfg.devEn	&= ~DEF_DEVEN_BIT_GPS;
				else if(pdu->paraData[0] == 0x02)
						s_cfg.devEn	&= ~DEF_DEVEN_BIT_BT;
				else if(pdu->paraData[0] == 0x03)
						s_cfg.devEn	&= ~DEF_DEVEN_BIT_SHAKE;				
		}
		else
				return	-1;
	
		if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
				return	0;
		else
				return	-1;
}
/*******************************************************************************
* Function Name  : setCarInfoProcess
* Description    : 服务器下发设置发动机品牌
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	setCarInfoProcess	(Protocol_Typedef *pdu)
{
		if(pdu->paraLen != 1)
				return -1;	
		
    HAL_CommonParaReload();
		s_common.carInfo[2] =pdu->paraData[0];
		if(HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
				return	0;
		else
				return	-1;
}
/*******************************************************************************
* Function Name  : setDelaySleepProcess
* Description    : 服务器下发设置延时睡眠处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	setDelaySleepProcess	(Protocol_Typedef *pdu)
{
		if(pdu->paraLen != 2)
				return -1;	
		
    HAL_SysParaReload();
		s_cfg.igOffDelay	=(u16)((pdu->paraData[0] << 8) | pdu->paraData[1]);
		if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
				return	0;
		else
				return	-1;
}
/*******************************************************************************
* Function Name  : setTimeProcess
* Description    : 服务器下发设置校准时间处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	setTimeProcess	(Protocol_Typedef *pdu)
{
		SYS_DATETIME	cutTime;
		u32	count=0;
	
		if(pdu->paraLen != 4)
				return -1;	
		
		count =(u32)((pdu->paraData[0] << 24) | (pdu->paraData[1] << 16) | (pdu->paraData[2] << 8) | pdu->paraData[3]);
		memset((u8 *)&cutTime,	0,	sizeof(cutTime));
		Gettime(count,	&cutTime);
		BSP_RTC_Set_Current(&cutTime);	// 设置时间
		return 0;
}
/*******************************************************************************
* Function Name  : setDevThreholdProcess
* Description    : 服务器下发设置设备门限值(如果参数为0xFF则表示不对该参数进行设置)
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	setDevThreholdProcess	(Protocol_Typedef *pdu)
{
		u8 tmpU8=0;
		u16	p=0,tmpU16=0;
		if(pdu->paraLen < 13)	// 所有参数一共13byte
				return -1;	
				
		// 减速读碰撞门限设置
    HAL_SysParaReload();
		tmpU16 =(u16)((pdu->paraData[p] << 8) | pdu->paraData[p+1]);
		if(tmpU16 != 0xffff)
				s_cfg.dssATH	=tmpU16;
		p	+=2;
		tmpU16 =(u16)((pdu->paraData[p] << 8) | pdu->paraData[p+1]);
		if(tmpU16 != 0xffff)
				s_cfg.dssDTH	=tmpU16;
		p	+=2;
		tmpU16 =(u16)((pdu->paraData[p] << 8) | pdu->paraData[p+1]);
		if(tmpU16 != 0xffff)
				s_cfg.dssPTH	=tmpU16;
		p	+=2;
		
		// 低压报警门限设置
		tmpU16 =(u16)((pdu->paraData[p] << 8) | pdu->paraData[p+1]);
		if(tmpU16 != 0xffff)
				s_cfg.lowPower	=tmpU16;
		p	+=2;
		
		// 震动唤醒门限
		tmpU16 =(u16)((pdu->paraData[p] << 8) | pdu->paraData[p+1]);
		if(tmpU16 != 0xffff)
				s_cfg.accMT	=tmpU16;
		p	+=2;
		
		// 修改加速度寄存器数值
		BSP_ACC_CfgInt (DEF_ENABLE,	(u8)s_cfg.accMT,	(u8)s_cfg.accMD,	5000);
		
		// 设备移动报警时间阀值
		tmpU8 =(u8)(pdu->paraData[p]);
		if(tmpU8 != 0xff)
				s_cfg.sysMoveT	=tmpU8;
		p	+=1;
		
		// 设备移动报警速度阀值
		tmpU8 =(u8)(pdu->paraData[p]);
		if(tmpU8 != 0xff)
				s_cfg.sysMove	 =tmpU8;
		p	+=1;
		
		// 设备延时体检时间
		tmpU8 =(u8)(pdu->paraData[p]);
				s_cfg.tjDelayTime	=tmpU8;
		p	+=1;
			
		if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
				return	0;
		else
				return	-1;
}

/*******************************************************************************
* Function Name  : queDevParaProcess
* Description    : 服务器下发查询设备参数
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	queDevParaProcess	(Protocol_Typedef *pdu)
{
		u8 ret =0;
		u8 tmpPara[60]={0};
		u16	paraLen=0;
		u32	count=0;
		SYS_DATETIME	tmpRtc;
		SendData_TypeDef sendData;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
		memset((u8 *)&tmpRtc,	0,	sizeof(tmpRtc));
	
		// 回复指令报文序列号
		tmpPara[paraLen] =(u8)(pdu->bodySn >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(pdu->bodySn);	
		paraLen++;
			
		// 时间戳
		BSP_RTC_Get_Current(&tmpRtc);	
		count =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);	
		tmpPara[paraLen] =(u8)(count >> 24);	
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 16);
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 8);
		paraLen++;
		tmpPara[paraLen] =(u8)(count);
		paraLen++;
		
		// 参数总数
		tmpPara[paraLen] =0x09;	
		paraLen++;	
		
		// 加速度报警阀值
		tmpPara[paraLen] =(u8)(s_cfg.dssATH >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.dssATH);	
		paraLen++;
		
		// 减速度报警阀值
		tmpPara[paraLen] =(u8)(s_cfg.dssDTH >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.dssDTH);	
		paraLen++;
		
		// 碰撞报警阀值
		tmpPara[paraLen] =(u8)(s_cfg.dssPTH >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.dssPTH);	
		paraLen++;
		
		// 电压报警阀值
		tmpPara[paraLen] =(u8)(s_cfg.lowPower >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.lowPower);	
		paraLen++;
		
		// 震动报警阀值
		tmpPara[paraLen] =(u8)(s_cfg.accMT >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.accMT);	
		paraLen++;
		
		// 设备移动时间报警阀值
		tmpPara[paraLen] =(u8)(s_cfg.sysMoveT);	
		paraLen++;		
		
		// GPS采集间隔
		/*
		tmpPara[paraLen] =(u8)((s_cfg.onReportTime & 0x0000ff00) >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.onReportTime & 0x000000ff);	
		paraLen++;
		*/
		tmpPara[paraLen] =(u8)(s_cfg.gpsCollectTime);	
		paraLen++;
		
		// 设备移动速度阀值
		tmpPara[paraLen] =(u8)(s_cfg.sysMove);	
		paraLen++;
		
		// 体检延时时间
		tmpPara[paraLen] =(u8)(s_cfg.tjDelayTime);	
		paraLen++;	

		// 健康包上传时间
		tmpPara[paraLen] =(u8)(s_cfg.healthReportTime >> 24);	
		paraLen++;	
		tmpPara[paraLen] =(u8)(s_cfg.healthReportTime >> 16);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.healthReportTime >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.healthReportTime);	
		paraLen++;
		
		// 油耗获取频率
		tmpPara[paraLen] =0x00;	
		paraLen++;	

		// 设备休眠时间
		tmpPara[paraLen] =(u8)(s_cfg.globSleepTime >> 24);	
		paraLen++;	
		tmpPara[paraLen] =(u8)(s_cfg.globSleepTime >> 16);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.globSleepTime >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.globSleepTime);	
		paraLen++;
	
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_QUEPARA;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : queSMSConPhoProcess
* Description    : 服务器下发查询设备短信控制合法号码(返回格式len+,ph1,ph2,ph3,ph4,ph5,ph6)
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	queSMSConPhoProcess	(Protocol_Typedef *pdu)
{
		u8 ret =0;
		u8 tmpPara[150]={0};
		u16	paraLen=0;
		SendData_TypeDef sendData;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
			
		// 回复指令报文序列号
		tmpPara[paraLen] =(u8)(pdu->bodySn >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(pdu->bodySn);	
		paraLen++;
	
		// 字符串长度
		tmpPara[paraLen] =0;	//	先写0
		paraLen++;
		
		// 复制号码
		memcpy((tmpPara + paraLen),	s_cfg.pho1,	strlen((const char *)s_cfg.pho1));
		paraLen +=strlen((const char *)s_cfg.pho1);
		tmpPara[paraLen] =',';
		paraLen++;
		
		memcpy((tmpPara + paraLen),	s_cfg.pho2,	strlen((const char *)s_cfg.pho2));
		paraLen +=strlen((const char *)s_cfg.pho2);
		tmpPara[paraLen] =',';
		paraLen++;
		
		memcpy((tmpPara + paraLen),	s_cfg.pho3,	strlen((const char *)s_cfg.pho3));
		paraLen +=strlen((const char *)s_cfg.pho3);
		tmpPara[paraLen] =',';
		paraLen++;
		
		memcpy((tmpPara + paraLen),	s_cfg.pho4,	strlen((const char *)s_cfg.pho4));
		paraLen +=strlen((const char *)s_cfg.pho4);
		tmpPara[paraLen] =',';
		paraLen++;
		
		memcpy((tmpPara + paraLen),	s_cfg.pho5,	strlen((const char *)s_cfg.pho5));
		paraLen +=strlen((const char *)s_cfg.pho5);
		tmpPara[paraLen] =',';
		paraLen++;
		
		memcpy((tmpPara + paraLen),	s_cfg.pho6,	strlen((const char *)s_cfg.pho6));
		paraLen +=strlen((const char *)s_cfg.pho6);
		
		tmpPara[2] = paraLen-3;	// 填写长度
	
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_QUEPHO;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : smsCenterPhoProcess
* Description    : 服务器下发短信服务器中心号码
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	smsCenterPhoProcess	(Protocol_Typedef *pdu)
{
		s8 ret=0;
		u8 tmpLen=0,	tmpIDBuf[20]={0},	i=0;
		u32	filename = FLASH_CPU_APPNAME_START;
		SendData_TypeDef   smsTmpSend;  // 短信发送临时数据结构 			
	
		memset((u8 *)&smsTmpSend,	'\0',	sizeof(smsTmpSend));
		memset((u8 *)&tmpIDBuf,	'\0',	sizeof(tmpIDBuf));
		
		tmpLen =pdu->paraData[0];
		smsTmpSend.path				=	DEF_PATH_SMS;
		smsTmpSend.smsAlType 	= DEF_SMS_COM;  
		smsTmpSend.smsMode 		= DEF_GSM_MODE; 
		memcpy(smsTmpSend.smsNum,	&pdu->paraData[1],	tmpLen);
		
		strcpy((char *)smsTmpSend.data,	(char *)GSM_SMS_NUMCHECK_H);
		
		// 添加IDC
		Hex2StringArray (s_common.id,	PRA_SIZEOF_COMMON_ID,	tmpIDBuf);
		tmpIDBuf[PRA_SIZEOF_COMMON_ID*2-1] = '\0';	// 去除ID后面补足的0	
		strcat((char *)smsTmpSend.data,	(char *)tmpIDBuf);	
		
		// 添加车系
		memset((u8 *)&tmpIDBuf,	'\0',	sizeof(tmpIDBuf));
		tmpIDBuf[0] = ',';
		tmpIDBuf[1] = s_common.carInfo[0];	
		tmpIDBuf[2] = ',';
		tmpIDBuf[3] = '\0';
		strcat((char *)smsTmpSend.data,	(char *)tmpIDBuf);
			
		// 添加设备软件版本号
		memset((u8 *)&tmpIDBuf,	'\0',	sizeof(tmpIDBuf));
		for(i=0;	i<5;	i++)
		{
				tmpIDBuf[i] =*(u8*)(filename + i);
		}
		tmpIDBuf[i] = '\0';
		strcat((char *)smsTmpSend.data,	(char *)tmpIDBuf);
		smsTmpSend.len = strlen((const char *)smsTmpSend.data);
	
		// 数据入发送队列///////////////////////
		ret =HAL_ComQue_in (DEF_SMS_QUE,	&smsTmpSend);	
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : simCardPhoProcess
* Description    : 服务器下发设备SIM卡号码
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	simCardPhoProcess	(Protocol_Typedef *pdu)
{		
		if((pdu->paraData[0] >=5) && (pdu->paraData[0] < 20))
		{
				memset(s_gsmApp.simNum,	'\0',	sizeof(s_gsmApp.simNum));
				memcpy(s_gsmApp.simNum,	&pdu->paraData[1],	pdu->paraData[0]);
				
				// 将SIM卡号写入
				s_gsmApp.simNumWriteFlag = 1;	// 通知GPRS任务将新SIM卡号写入SIM卡中
				return 0;
		}
		else
				return -1;
}
/*******************************************************************************
* Function Name  : setSMSConPhoProcess
* Description    : 服务器下发设置短信控制合法号码(格式：len+ph1,ph2,ph3,ph4,ph5,ph6)
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	setSMSConPhoProcess	(Protocol_Typedef *pdu)
{
		u16 tmpLen1=0,tmpLen2=0,tmpLen3=0,tmpLen4=0,tmpLen5=0,tmpLen6=0;
		u8	tmpPh1[20]={0},tmpPh2[20]={0},tmpPh3[20]={0},tmpPh4[20]={0},tmpPh5[20]={0},tmpPh6[20]={0};
	
		if(pdu->paraLen == 0)
				return -1;
		
		memset(tmpPh1,	'\0',	sizeof(tmpPh1));
		memset(tmpPh2,	'\0',	sizeof(tmpPh2));
		memset(tmpPh3,	'\0',	sizeof(tmpPh3));
		memset(tmpPh4,	'\0',	sizeof(tmpPh4));
		memset(tmpPh5,	'\0',	sizeof(tmpPh5));
		memset(tmpPh6,	'\0',	sizeof(tmpPh6));
		
		tmpLen1 =GetProParameter(1,	pdu->paraData+1,	pdu->paraData[0],	tmpPh1);		// 参数区加1跳过长度字节
		if((tmpLen1 == 0xffff) || (tmpLen1 > PRA_SIZEOF_SYSCFG_PHO1) || (IsIntString(tmpPh1,	tmpLen1) == 0xffff))
				return -1;
		tmpLen2 =GetProParameter(2,	pdu->paraData+1,	pdu->paraData[0],	tmpPh2);
		if((tmpLen2 == 0xffff) || (tmpLen2 > PRA_SIZEOF_SYSCFG_PHO2) || (IsIntString(tmpPh2,	tmpLen2) == 0xffff))
				return -1;
		tmpLen3 =GetProParameter(3,	pdu->paraData+1,	pdu->paraData[0],	tmpPh3);
		if((tmpLen3 == 0xffff) || (tmpLen3 > PRA_SIZEOF_SYSCFG_PHO3) || (IsIntString(tmpPh3,	tmpLen3) == 0xffff))
				return -1;
		tmpLen4 =GetProParameter(4,	pdu->paraData+1,	pdu->paraData[0],	tmpPh4);
		if((tmpLen4 == 0xffff) || (tmpLen4 > PRA_SIZEOF_SYSCFG_PHO4) || (IsIntString(tmpPh4,	tmpLen4) == 0xffff))
				return -1;
		tmpLen5 =GetProParameter(5,	pdu->paraData+1,	pdu->paraData[0],	tmpPh5);
		if((tmpLen5 == 0xffff) || (tmpLen5 > PRA_SIZEOF_SYSCFG_PHO5) || (IsIntString(tmpPh5,	tmpLen5) == 0xffff))
				return -1;
		tmpLen6 =GetProParameter(6,	pdu->paraData+1,	pdu->paraData[0],	tmpPh6);
		if((tmpLen6 == 0xffff) || (tmpLen6 > PRA_SIZEOF_SYSCFG_PHO6) || (IsIntString(tmpPh6,	tmpLen6) == 0xffff))
				return -1;
		
		// 参数合法性判断
    HAL_SysParaReload();
		if(tmpLen1 == 0)
				memset(s_cfg.pho1, 0,	PRA_MAXOF_SYSCFG_PHO1);		// 2015-6-15 修改参数为0时不清空原存储参数问题			
		else
				strcpy((char *)s_cfg.pho1,			(const char *)tmpPh1);
					
		if(tmpLen2 == 0)
				memset(s_cfg.pho2, 0,	PRA_MAXOF_SYSCFG_PHO2);
		else
				strcpy((char *)s_cfg.pho2,			(const char *)tmpPh2);
					
		if(tmpLen3 == 0)
				memset(s_cfg.pho3, 0,	PRA_MAXOF_SYSCFG_PHO3);
		else
				strcpy((char *)s_cfg.pho3,			(const char *)tmpPh3);
					
		if(tmpLen4 == 0)
				memset(s_cfg.pho4, 0,	PRA_MAXOF_SYSCFG_PHO4);
		else
				strcpy((char *)s_cfg.pho4,			(const char *)tmpPh4);
				
		if(tmpLen5 == 0)
				memset(s_cfg.pho5, 0,	PRA_MAXOF_SYSCFG_PHO5);
		else
				strcpy((char *)s_cfg.pho5,			(const char *)tmpPh5);
					
		if(tmpLen6 == 0)
				memset(s_cfg.pho6, 0,	PRA_MAXOF_SYSCFG_PHO6);				
		else
				strcpy((char *)s_cfg.pho6,			(const char *)tmpPh6);
		
		if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : setSleepTimeProcess
* Description    : 服务器下发设置设备睡眠时间及健康包上传时间
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	setSleepTimeProcess	(Protocol_Typedef *pdu)
{
		if((pdu->paraLen != 5) || ((pdu->paraData[0] != 0) && (pdu->paraData[0] != 1)))
				return -1;	
		
    HAL_SysParaReload();
		if(pdu->paraData[0] == 0)
		{
				// 设置设备熄火休眠时间
				s_cfg.globSleepTime = (u32)((pdu->paraData[1] << 24) | (pdu->paraData[2] << 16) | (pdu->paraData[3] << 8) | pdu->paraData[4]);
		}
		else if(pdu->paraData[0] == 1)
		{
				// 设置设备健康包上传时间
				s_cfg.healthReportTime = (u32)((pdu->paraData[1] << 24) | (pdu->paraData[2] << 16) | (pdu->paraData[3] << 8) | pdu->paraData[4]);
		}
		if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
				return	0;
		else
				return	-1;
}

/*******************************************************************************
* Function Name  : conTTSProcess
* Description    : 服务器下发语音播报控制
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
static	s8	conTTSProcess	(Protocol_Typedef *pdu)
{
		u8	tmpBuf[PRA_MAXOF_SYSCFG_SOUND1]={0};
		u16 p=0,tmpLen=0;
		u32	proCount=0, cutCount=0;
		SYS_DATETIME	cunTime;
	
		if(pdu->paraLen < 1)
				return -1;	
		
		// 计算时间差值
		proCount =(u32)((pdu->paraData[0] << 24) | (pdu->paraData[1] << 16) | (pdu->paraData[2] << 8) | pdu->paraData[3]);		
		memset((u8 *)&cunTime,	0,	sizeof(cunTime));
		BSP_RTC_Get_Current(&cunTime);
		cutCount	=	Mktime(cunTime.year, cunTime.month, cunTime.day, cunTime.hour, cunTime.minute, cunTime.second);
		
		if(pdu->paraData[4] == 0x00)
		{
				// 固定播报
			
				// 获取位置1语音
        HAL_SysParaReload();
				p =5;		
				memset(tmpBuf,	'\0',	sizeof(tmpBuf));
				tmpLen = (u16)((pdu->paraData[p+1] << 8) | pdu->paraData[p+2]);	// ???
				if(pdu->paraData[p] == 0x00)
				{			
						if(tmpLen <= PRA_SIZEOF_SYSCFG_SOUND1)
						{
								memcpy(tmpBuf,	(u8 *)&(pdu->paraData[p+3]),	tmpLen);						
								tmpBuf[tmpLen] ='\0'; 
								strcpy((char *)s_cfg.sound1,	(const char*)tmpBuf);
						}
						else 
								return -1;
				}
				else if(pdu->paraData[p] == 0x01)
				{
						if(tmpLen <= PRA_SIZEOF_SYSCFG_SOUND2)
						{
								memcpy(tmpBuf,	(u8 *)&(pdu->paraData[p+3]),	tmpLen);						
								tmpBuf[tmpLen] ='\0'; 
								strcpy((char *)s_cfg.sound2,	(const char*)tmpBuf);
						}
						else
								return -1;
				}
				// 获取位置2语音
				if(pdu->paraLen > (p + 2 + tmpLen + 1))
				{	
						p= p+2+tmpLen+1; // 指向位置2语音
						memset(tmpBuf,	'\0',	sizeof(tmpBuf));
						tmpLen = (u16)((pdu->paraData[p+1] << 8) | pdu->paraData[p+2]);	// ???
						if(pdu->paraData[p] == 0x00)
						{			
								if(tmpLen <= PRA_SIZEOF_SYSCFG_SOUND1)
								{
										memcpy(tmpBuf,	(u8 *)&(pdu->paraData[p+3]),	tmpLen);						
										tmpBuf[tmpLen] ='\0'; 
										strcpy((char *)s_cfg.sound1,	(const char*)tmpBuf);
								}
								else
										return -1;
						}
						else if(pdu->paraData[p] == 0x01)
						{
								if(tmpLen <= PRA_SIZEOF_SYSCFG_SOUND2)
								{
										memcpy(tmpBuf,	(u8 *)&(pdu->paraData[p+3]),	tmpLen);						
										tmpBuf[tmpLen] ='\0'; 
										strcpy((char *)s_cfg.sound2,	(const char*)tmpBuf);
								}
								else
										return -1;
						}
				}
				if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
						return	0;
				else
						return	-1;				
		}	
		else if(pdu->paraData[4] == 0x01)
		{
				if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
				{
						// 即时播报(只处理1分钟内计时播报)
						tmpLen = (u16)((pdu->paraData[5] << 8) | pdu->paraData[6]);										
						if(((cutCount >= proCount) && ((cutCount - proCount) < 60)) ||
							 ((proCount >  cutCount) && ((proCount - cutCount) < 60)))
						{
								if(HAL_SendCONProProcess(TTS_CMD,	pdu->bodySn,	(pdu->paraData + 7),	tmpLen) == 0)
										return 0;
								else
										return -2;						
						}
						else
								return -1;
				}
				else
						return -1;	// 2015-6-15 :如果TTS被禁止则禁止播放服务器下发语音
		}
		else 
				return -1;		
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PROTOCOL SUB-OBD FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/

/*******************************************************************************
* Function Name  : setObdComProcess
* Description    : 服务器下发OBD通用处理函数
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1, 忙返回-2
*******************************************************************************/
static	s8	setObdComProcess	(Protocol_Typedef *pdu)
{	
		u32	tmpOdo=0;
	
		/*
		// 2016-1-18 取消里程校准单独处理，透传交由OBD处理
		if(pdu->bodycmdId == DEF_CMDID_SETODO_PLA)
		{
				// 初始化里程单独处理以后归到OBD部分处理
				tmpOdo =(u32)((pdu->paraData[0] << 24) | (pdu->paraData[1] << 16) | (pdu->paraData[2] << 8) | (pdu->paraData[3]));
				u8fun_odo_Calibration(tmpOdo);
				return 0;	// 初始化里程没有忙状态,直接返回成功
		}
		else if(pdu->bodycmdId == DEF_CMDID_CONCAR_PLA)
		*/
		if(pdu->bodycmdId == DEF_CMDID_CONCAR_PLA)
		{
				// 基于网络的车辆控制特殊处理
				if(conSms.dataLock == 0)
				{
						memset((u8 *)&simulateSms,	0,	sizeof(simulateSms));
						simulateSms.dataLen = pdu->paraLen-1;
						memcpy(simulateSms.dataBuf,	(pdu->paraData + 1),	(pdu->paraLen-1));
						OSMboxPost(OS_RecSmsMbox, (void *)&simulateSms);		// 发送模拟车辆控制短信	
						return 0;
				}
				else
						return -2;	
		}
		else
		{
				if(pdu->bodycmdId == DEF_CMDID_READFUEL_PLA)
				{
						// 剩余油量查询
						if(HAL_SendOBDProProcess(CMD_READ_FUEL,	pdu->bodySn,	0,	(u8 *)"",	0) == 0)			// 通知OBD读取当前油量	
								return 0;
						else
								return -2;
				}
				else
				{
						if(HAL_SendOBDProProcess(CMD_PROTOCOL,	pdu->bodySn,	pdu->bodycmdId,	pdu->paraData,	pdu->paraLen) == 0)
						{
								// 协议特殊处理
								if(pdu->bodycmdId == DEF_CMDID_STARTTJ_PLA)
								{
										HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_ENABLE);				// 系统置体检状态中
										HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_ENABLE);	// 设置播放还未播放体检结束状态
										if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
												HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound1,	strlen((const char *)s_cfg.sound1));		// 播放体检开始语音		
								}	
								// 车辆状态读取
								else if(pdu->bodycmdId == DEF_CMDID_QUECAR_PLA)
								{
										glob_readCarSn			=pdu->bodySn;	// 临时保存应答流水号
										glob_readTimeStamp	=(vu32)((pdu->paraData[0] << 24) | (pdu->paraData[1] << 16) | (pdu->paraData[2] << 8) | pdu->paraData[3]);	// 临时保存应答时间戳												
								}
								return 0;
						}				
						else
								return -2;
				}
		}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PROTOCOL PACK FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/

/*******************************************************************************
* Function Name  : HAL_PackComAckProcess
* Description    : 通用应答实时数据打包处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_PackComAckProcess	(u8 *pPara,	u16 paraLen,	u8 *outBuf,	u16 *outLen)
{
		u16	tmpSn=0;
		tmpSn =UpDataSn();	
		return ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_COMACK_DEV,	pPara,	paraLen,	outBuf,	outLen);
}
/*******************************************************************************
* Function Name  : HAL_PackOBDDataProcess
* Description    : OBD实时数据打包处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_PackOBDDataProcess	(u16 cmdId,	u8 *pPara,	u16 paraLen,	u8 *outBuf,	u16 *outLen, u16 *retSn)
{
		u16	tmpSn=0;
		tmpSn =UpDataSn();
		*retSn =tmpSn;
		return ZXT_PackBuild_Process (tmpSn,	cmdId,	pPara,	paraLen,	outBuf,	outLen);
}
/*******************************************************************************
* Function Name  : HAL_PackRealTimeDataProcess
* Description    : Ram队列数据打包处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_PackRealTimeDataProcess	(u8 *dataType,	u8 *outBuf,	u16 *outLen, u16 *retSn)
{
		s8	ret=0;
		u8	tmpPara[50]={0};
		u16	i=0;
		u16	tmpSn=0,cmdId=0;
		u32	count=0;
		SendData_TypeDef	tmpSend;
	
		memset((u8 *)&tmpSend,	0,	sizeof(tmpSend));
	
		// 数据读取
		for(i=0;	(HAL_ComQue_size (DEF_RAM_QUE) > 0) && (i < GPS_RAMDATA_SIZE); i++)
		{
				ret =HAL_ComQue_out(DEF_RAM_QUE,	&tmpSend);
				if(ret == DEF_PROQUE_QUE_OK)
				{
						HAL_ComQue_outend (DEF_RAM_QUE);	// 数据出队
						if(((tmpSend.path != DEF_PATH_GPRS) && (tmpSend.path != DEF_PATH_BT)) || (tmpSend.len == 0) ||
   						  (tmpSend.len > SPI_FLASH_PageSize) ||
							  (IsDataType(tmpSend.type) == 0xffff))
						{								
								continue;	// 非法数据,跳过该数据包									
						}
						else
						{
								// 数据打包
								tmpSn =UpDataSn();
								*retSn =tmpSn;
								cmdId	=RetCmdId(tmpSend.type);
								if(tmpSend.type == DEF_TYPE_GPS)
								{
										// GPS数据比较特殊(参数第一字节需要填写帧数)
										memset(tmpPara,	0,	sizeof(tmpPara));
										tmpPara[0] = 0x01;	// 帧数为1帧(先不不考虑GPS多帧实时上传情况)
										memcpy((tmpPara + 1),	tmpSend.data,	tmpSend.len);
										memset(tmpSend.data,	0,	DEF_PACK_BUF_SIZE);	
										tmpSend.len +=1;
										memcpy(tmpSend.data,	tmpPara,	tmpSend.len);								
								}
								else if(tmpSend.type == DEF_TYPE_ZITAI)
								{
										// 姿态数据比较特殊(参数第一字节需要填写帧数)
										memset(tmpPara,	0,	sizeof(tmpPara));
										tmpPara[0] = 0x01;	// 帧数为1帧(先不不考虑姿态多帧实时上传情况)
										memcpy((tmpPara + 1),	tmpSend.data,	tmpSend.len);
										memset(tmpSend.data,	0,	DEF_PACK_BUF_SIZE);	
										tmpSend.len +=1;
										memcpy(tmpSend.data,	tmpPara,	tmpSend.len);								
								}
								else if(tmpSend.type == DEF_TYPE_IG)
								{
										if(tmpSend.data[0] == TYPE_IG_ON)
										{
												// 改写时间偏移
												count =OSTmr_Count_Get(&s_osTmp.igStartTicks);
												tmpSend.data[1] =(u8)(count >> 24);	
												tmpSend.data[2] =(u8)(count >> 16);
												tmpSend.data[3] =(u8)(count >> 8);
												tmpSend.data[4] =(u8)(count);		
												
												// 同步点火参数备份buf
												igOnSaveBuf[0]	=(u8)(count >> 24);		// 2015-8-19 熄火报文格式修改
												igOnSaveBuf[1]	=(u8)(count >> 16);
												igOnSaveBuf[2]	=(u8)(count >> 8);
												igOnSaveBuf[3]	=(u8)(count);	
										}
								}
								ret =ZXT_PackBuild_Process (tmpSn,	cmdId,	tmpSend.data,	tmpSend.len,	outBuf,	outLen);	
								*dataType =tmpSend.type;		// 反回数据类型
								break;
						}
				}
				else if(ret == DEF_PROQUE_QUE_EMPTY)
				{
						break;	// 队列已空
				}
				else if(ret == DEF_PROQUE_QUE_ERR)
				{
						HAL_ComQue_outend (DEF_RAM_QUE);	// 数据出队出错,跳过该数据包
						continue;	// 数据出队出错,跳过该数据包
				}			
		}
		return ret;

}
/*******************************************************************************
* Function Name  : HAL_PackFlashDataProcess
* Description    : FLASH队列数据打包处理
* Input          : maxLen: 允许打包的最大长度(不能超过FLASH批量数据临时发送结构数据BUF定义即DEF_FLASHSENDBUF_SIZE大小)
* Output         : 
* Return         : 正确返回实际出队个数
*******************************************************************************/
s8	HAL_PackFlashDataProcess	(u16	maxLen,	u8 *outBuf,	u16 *outLen, u16 *retSn,	u16 *lastOut,	u16 *outNum)
{
		SendData_TypeDef	tmpSend;
		u8	tmpPara[DEF_FLASHSENDBUF_SIZE]={0};
		u8	frameNum=0;	// 实际有效数据帧
		s8	ret=0;
		u16 i=0,	tmpSn=0,	paraLen=1,	queOutNum=0;	// 跳过第一字节的总帧数
		u32	count=0;
		CPU_SR   cpu_sr;
		
		// 数据合法性判断
		if(maxLen > DEF_FLASHSENDBUF_SIZE)
				return 0;
		
		OS_ENTER_CRITICAL();
		*lastOut 							=s_queFlash.out;			// 保存出队之前的out指针
		s_queFlash.outExt			=s_queFlash.out;			// 获取启始位置
		s_queFlash.entriesExt	=s_queFlash.entries;	
		OS_EXIT_CRITICAL();
	
		// 数据提取
		for(i=0;	(s_queFlash.entriesExt > 0) && (i < GPS_FLASHDATA_USE); i++)
		{			
				memset((u8 *)&tmpSend,	0,	sizeof(tmpSend));
				ret =HAL_ComQue_out(DEF_FLASH_QUE,	&tmpSend);
				if(ret == DEF_PROQUE_QUE_OK)
				{		
						if(((tmpSend.path != DEF_PATH_GPRS) && (tmpSend.path != DEF_PATH_BT)) || (tmpSend.len == 0) ||
							 (tmpSend.len > SPI_FLASH_PageSize) ||
							 (IsDataType(tmpSend.type) == 0xffff))
						{
								HAL_ComQue_outend (DEF_FLASH_QUE);	// 数据出队
								queOutNum++;	// 记录点增加
								continue;	// 非法数据,跳过该数据包					
						}
						else
						{
								if((maxLen - 10 - paraLen) >= (tmpSend.len + 2))	// 判断余下长度是否够加入下一包(协议除参数外协议框架长度10byte)
								{							
										tmpPara[paraLen]	= tmpSend.type;	
										paraLen++;
										tmpPara[paraLen]	= tmpSend.len;
										paraLen++;
										
										// 点火时间偏移量处理
										if(tmpSend.type == DEF_TYPE_IG)
										{
												if(tmpSend.data[0] == TYPE_IG_ON)
												{
														// 改写时间偏移
														count =OSTmr_Count_Get(&s_osTmp.igStartTicks);
														tmpSend.data[1] =(u8)(count >> 24);	
														tmpSend.data[2] =(u8)(count >> 16);
														tmpSend.data[3] =(u8)(count >> 8);
														tmpSend.data[4] =(u8)(count);	
													
														// 同步点火参数备份buf
														igOnSaveBuf[0]	=(u8)(count >> 24);		// 2015-8-19 熄火报文格式修改
														igOnSaveBuf[1]	=(u8)(count >> 16);
														igOnSaveBuf[2]	=(u8)(count >> 8);
														igOnSaveBuf[3]	=(u8)(count);	
												}
										}
										memcpy((tmpPara + paraLen),	tmpSend.data,	tmpSend.len);
										paraLen +=tmpSend.len;						
										frameNum +=1;		// 增加一帧数据(先不考虑一个存储单元中存储多帧情况)										
										queOutNum++;	// 记录点增加
										HAL_ComQue_outend (DEF_FLASH_QUE);	// 数据出队
								}
								else
										break;	// 再打包数据将可能超出范围
						}
				}
				else if(ret == DEF_PROQUE_QUE_EMPTY)
						break;	// 队列已空
				else if(ret == DEF_PROQUE_QUE_ERR)
				{
						queOutNum++;	// 记录点增加
						HAL_ComQue_outend (DEF_FLASH_QUE);	// 数据出队
						continue;	// 数据出队出错,跳过该数据包
				}
				else
						break;		// 非法的返回值类型,退出数据出队				
		}
		
		// 数据打包
		ret =0;
		if(frameNum != 0)
		{
				// 打包数据
				tmpSn =UpDataSn();
				*retSn =tmpSn;
				tmpPara[0] = frameNum;	// 加入帧数量
				ret =ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_FLASH_DEV,	tmpPara,	paraLen,	outBuf,	outLen);							
		}
		*outNum =queOutNum;
		return ret;
}
/*******************************************************************************
* Function Name  : HAL_GPSInQueueProcess
* Description    : GPS数据入队处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_GPSInQueueProcess	(void)
{
		u8 ret =0;
		SendData_TypeDef sendData;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
	
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_GPS;
		sendData.len  = sizeof(GPSProData_Def);
		HAL_GpsProDataOut(sendData.data,	&s_gpsPro);	// STM32默认为小端模式,数据按大端模式输出
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_ZiTaiInQueueProcess
* Description    : 姿态数据入队处理
* Input          : type 由ZITAI_TYPE确定
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_ZiTaiInQueueProcess	(ZITAI_TYPE type,	GPSProData_Def *pGps,	s16 X_Axis, s16 Y_Axis, s16 Z_Axis, s16 XYZ_Axis)
{
		u8 ret =0;
		u8 tmpPara[60]={0};
		u16	paraLen=0;
		SendData_TypeDef sendData;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
	
		// 参数区赋值
		// 姿态帧数临时发送打包时添加
		
		// 添加GPS信息	
		if(pGps != (void *)0)
		{
				HAL_GpsProDataOut((tmpPara + paraLen),	pGps);	// STM32默认为小端模式,数据按大端模式输出
				paraLen +=sizeof(GPSProData_Def);
		}
		else
				paraLen +=sizeof(GPSProData_Def);		// 用0填充
		
		// 添加类型
		tmpPara[paraLen] =type;	
		paraLen++;
		
		// 添加加速度信息
		tmpPara[paraLen] =(u8)((u16)X_Axis >> 8);	// 添加X轴加速度
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)X_Axis);
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)Y_Axis >> 8);	// 添加Y轴加速度
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)Y_Axis);
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)Z_Axis >> 8);	// 添加Z轴加速度
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)Z_Axis);
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00 >> 8);	// 添加X轴角速度
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00);
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00 >> 8);	// 添加Y轴角速度
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00);
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00 >> 8);	// 添加Z轴角速度
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00);
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)XYZ_Axis >> 8);	// 添加合成加速度
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)XYZ_Axis);
		paraLen++;
	
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_ZITAI;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_24RptInQueueProcess
* Description    : 24小时报告数据入队处理
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_24RptInQueueProcess	(void)
{
		u8 ret =0;
		u8 tmpPara[60]={0};
		u16	paraLen=0;
		u32	count=0;
		SendData_TypeDef sendData;
		SYS_DATETIME	tmpRtc;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
		memset((u8 *)&tmpRtc,	0,	sizeof(tmpRtc));
	
		// 参数区赋值
		tmpPara[paraLen] =6;	// 2015-8-20 参数总数加1(增加了时间戳字段)
		paraLen++;
		tmpPara[paraLen] =(u8)(s_adInfo.VBatVol >> 8);	// 取电压值
		paraLen++;
		tmpPara[paraLen] =(u8)(s_adInfo.VBatVol);
		paraLen++;
		tmpPara[paraLen] =s_gsmApp.sLeve;	// GSM信号强度
		paraLen++;
		tmpPara[paraLen] =s_gpsPro.noSV;	// 卫星数量
		paraLen++;
		tmpPara[paraLen] =s_err.gsmResetTimes;	// 24小时内GSM模块故障次数
		paraLen++;
		tmpPara[paraLen] =s_err.gpsResetTimes;	// 24小时内GPS模块故障次数
		paraLen++;
		
		// 时间戳 2015-8-19 增加了时间戳字段
		BSP_RTC_Get_Current(&tmpRtc);	
		count =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);	
		if((tmpRtc.year < 2014) || (tmpRtc.year >= 2050))
				count =0;	// RTC时间无效直接清0						
		tmpPara[paraLen] =(u8)(count >> 24);	
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 16);
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 8);
		paraLen++;
		tmpPara[paraLen] =(u8)(count);
		paraLen++;
	
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_24REPORT;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;

}
/*******************************************************************************
* Function Name  : HAL_IGInQueueProcess
* Description    : 点火熄火数据入队处理
* Input          : type 由IG_TYPE确定
*                : offSetSec : 时间偏移秒数(只在熄火事件中有效，因为熄火状态判断通常会有延迟所以需要减掉该偏移量)
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_IGInQueueProcess	(IG_TYPE type,	u32 offSetSec)
{
		u8 ret =0;
		u8 tmpPara[60]={0};
		u16	paraLen=0;
		u32	offSetCount=0,rtcCount=0;
		SendData_TypeDef sendData;
		SYS_DATETIME	tmpRtc;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
		memset((u8 *)&tmpRtc,	0,	sizeof(tmpRtc));
	
		// 参数区赋值
		tmpPara[paraLen] =type;	// 点火熄火类型
		paraLen++;
		
		if(type == TYPE_IG_ON)
		{
				// 复制时间偏移量字段		
				tmpPara[paraLen] =0;	// 点火时间偏移暂时写成0，打包时在改写实际偏移量
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				OSTmr_Count_Start(&s_osTmp.igStartTicks);	// 记录起始tick值		
			
				// 里程
				tmpPara[paraLen] =0;	
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
			
				// 最大时速
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;			

				// 累计油耗
				tmpPara[paraLen] =0;	
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
			
				// RTC时间
				BSP_RTC_Get_Current(&tmpRtc);	
				if((tmpRtc.year < 2014) || (tmpRtc.year >= 2050))
						rtcCount =0;	// RTC时间无效直接清0		
				else
						rtcCount =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);
				
				tmpPara[paraLen] =(u8)(rtcCount >> 24);	
				paraLen++;
				tmpPara[paraLen] =(u8)(rtcCount >> 16);
				paraLen++;
				tmpPara[paraLen] =(u8)(rtcCount >> 8);
				paraLen++;
				tmpPara[paraLen] =(u8)(rtcCount);
				paraLen++;
				
				// 保存点火参数
				memset((u8 *)igOnSaveBuf,	0,	sizeof(igOnSaveBuf));	
				memcpy((u8 *)igOnSaveBuf,	(tmpPara + 1),	sizeof(igOnSaveBuf));	// 只保留除类型以外的18Byte
				
				// 2015-9-25 熄火后使用体检公里数赋值
				glob_odoBuf[0] =0;
				glob_odoBuf[1] =0;
				glob_odoBuf[2] =0;
		}	
		else
		{
				memcpy((tmpPara + paraLen),	(u8 *)igOnSaveBuf,	sizeof(igOnSaveBuf));
				paraLen +=sizeof(igOnSaveBuf);
			
				// 熄火获取时间偏移(只有真正熄火数据才做偏移处理)
				if(OSTmr_Count_Get(&s_osTmp.igStartTicks) < (offSetSec * 1000))
						offSetCount =0;
				else
						offSetCount =OSTmr_Count_Get(&s_osTmp.igStartTicks) - (offSetSec * 1000);	// 获取时间偏移
			
				tmpPara[paraLen] =(u8)(offSetCount >> 24);	
				paraLen++;
				tmpPara[paraLen] =(u8)(offSetCount >> 16);
				paraLen++;
				tmpPara[paraLen] =(u8)(offSetCount >> 8);
				paraLen++;
				tmpPara[paraLen] =(u8)(offSetCount);
				paraLen++;
				
				// 里程
				tmpPara[paraLen] =0;	
				paraLen++;
				tmpPara[paraLen] =glob_odoBuf[0];		// 2015-9-25 熄火后使用体检公里数赋值
				paraLen++;
				tmpPara[paraLen] =glob_odoBuf[1];
				paraLen++;
				tmpPara[paraLen] =glob_odoBuf[2];
				paraLen++;
				
				// 最大时速
        if (1u == HAL_GetIgonAccJudgeIgoffFlag())
        {
            tmpPara[paraLen] = 0xAAu;
            paraLen++;
            tmpPara[paraLen] = 0xAAu;
            paraLen++;
        }
        else
        {
            tmpPara[paraLen] =(u8)(s_osTmp.maxSpeed >> 8);	// 只有熄火才上传最大时速
            paraLen++;
            tmpPara[paraLen] =(u8)(s_osTmp.maxSpeed);
            paraLen++;
            s_osTmp.maxSpeed =0;	// 打包熄火报文后最大时速清0重新采集
        }
				
				// 累计油耗
				tmpPara[paraLen] =0;	
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				
				// RTC时间
				BSP_RTC_Get_Current(&tmpRtc);	
				if((tmpRtc.year < 2014) || (tmpRtc.year >= 2050))
						rtcCount =0;	// RTC时间无效直接清0			
				else
				{
						// 时间偏移量计算
						rtcCount =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);
						if(rtcCount < offSetSec)
								rtcCount =0;
						else
								rtcCount =rtcCount - offSetSec;	// 减掉时间偏移量			
				}
				tmpPara[paraLen] =(u8)(rtcCount >> 24);	
				paraLen++;
				tmpPara[paraLen] =(u8)(rtcCount >> 16);
				paraLen++;
				tmpPara[paraLen] =(u8)(rtcCount >> 8);
				paraLen++;
				tmpPara[paraLen] =(u8)(rtcCount);
				paraLen++;			
		}
	
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_IG;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
		{
				if(type == TYPE_IG_ON)
						BSP_BACKUP_SaveIGOnSta();		// 2015-8-19 存储点火状态
				else
						BSP_BACKUP_ClrIGOnSta();		// 2015-8-19 清除点火状态
						
				return 0;
		}
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_AlarmInQueueProcess
* Description    : 报警数据入队处理
* Input          : alarmBit 由ALARM_BIT_TYPE确定(支持扩展外设报警，alarmBit>0xff为扩展报警类型)
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_AlarmInQueueProcess	(ALARM_BIT_TYPE alarmBit)
{
		u8 ret =0;
		u8 tmpPara[60]={0};
		u16	paraLen=0;
		u32 count=0;
		SendData_TypeDef sendData;
		SYS_DATETIME	tmpRtc;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
		memset((u8 *)&tmpRtc,	0,	sizeof(tmpRtc));
		
		// 长度字节
		tmpPara[paraLen] = 0x00;	// 最后赋值
		paraLen++;
		
		// 报警内容
		if(alarmBit < 0xff)
		{
				tmpPara[paraLen] =(u8)alarmBit;	// 通用报警状态位
				paraLen++;	
		}
		else
		{		
				tmpPara[paraLen] =0x00;
				paraLen++;
				tmpPara[paraLen] =(u8)(alarmBit>>8);	// 扩展报警状态位
				paraLen++;
        tmpPara[paraLen] =(u8)(alarmBit>>16u);	// 扩展报警状态位
				paraLen++;
		}
		
		// 添加时间戳
		BSP_RTC_Get_Current(&tmpRtc);	
		count =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);
		tmpPara[paraLen] =(u8)(count >> 24);	
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 16);
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 8);
		paraLen++;
		tmpPara[paraLen] =(u8)(count);
		paraLen++;
		
		// 添加GPS信息
		HAL_GpsProDataOut((tmpPara+paraLen),	&s_gpsPro);	// STM32默认为小端模式,数据按大端模式输出
		paraLen +=sizeof(GPSProData_Def);
		
		// 最后确定参数长度
		tmpPara[0] = paraLen-1;	// 去除长度字节本身
	
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_ALARM;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_LogInQueueProcess
* Description    : 故障log数据入队处理
* Input          : type 由LOG_TYPE确定
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_LogInQueueProcess	(LOG_TYPE type)
{
		u8 ret =0;
		u8 tmpPara[60]={0};
		u16	paraLen=0;
		u32 count=0;
		SendData_TypeDef sendData;
		SYS_DATETIME	tmpRtc;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
	
		// 参数区赋值
		// 故障类型
		tmpPara[paraLen] =type;	
		paraLen++;
	
		if(type == TYPE_LOG_FLASH)
		{
				// 是否发送短信
				tmpPara[paraLen] =1;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =s_err.flashTimes;	
				paraLen++;		
				APP_ErrClr(&s_err.flashTimes);	// 清故障变量
		}
		else if(type == TYPE_LOG_SIM)
		{
				// 是否发送短信
				tmpPara[paraLen] =1;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =s_err.gsmSimTimes;	
				paraLen++;
				APP_ErrClr(&s_err.gsmSimTimes);	// 清故障变量
		}
		else if(type == TYPE_LOG_GSM)
		{
				// 是否发送短信
				tmpPara[paraLen] =0;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =s_err.gsmResetTimes;	
				paraLen++;
				APP_ErrClr(&s_err.gsmResetTimes);	// 清故障变量
		}
		else if(type == TYPE_LOG_GSMNET)
		{
				// 是否发送短信
				tmpPara[paraLen] =0;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =0;	
				paraLen++;
		}
		else if(type == TYPE_LOG_GPS)
		{
				// 是否发送短信
				tmpPara[paraLen] =1;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =s_err.gpsResetTimes;	
				paraLen++;
				APP_ErrClr(&s_err.gpsResetTimes);	// 清故障变量
		}
		else if(type == TYPE_LOG_OBDBUS)
		{
				// 是否发送短信
				tmpPara[paraLen] =1;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =s_err.obdBusRWTimes;	
				paraLen++;
				APP_ErrClr(&s_err.obdBusRWTimes);	// 清故障变量
		}
		else if(type == TYPE_LOG_OBDFLAG)
		{
				// 是否发送短信
				tmpPara[paraLen] =1;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =s_err.obdParaRWTimes;	
				paraLen++;
				APP_ErrClr(&s_err.obdParaRWTimes);	// 清故障变量
		}
		else if(type == TYPE_LOG_ACC)
		{
				// 是否发送短信
				tmpPara[paraLen] =0;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =s_err.accTimes;	
				paraLen++;
				APP_ErrClr(&s_err.accTimes);	// 清故障变量
		}
		else if(type == TYPE_LOG_CPUFLASH)
		{
				// 是否发送短信
				tmpPara[paraLen] =1;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =s_err.cpuFlashTimes;	
				paraLen++;
				APP_ErrClr(&s_err.cpuFlashTimes);	// 清故障变量
		}
		else if(type == TYPE_LOG_CON)
		{
				// 是否发送短信
				tmpPara[paraLen] =1;	
				paraLen++;
				// 故障次数
				tmpPara[paraLen] =s_err.conTimes;	
				paraLen++;
				APP_ErrClr(&s_err.conTimes);	// 清故障变量
		}
		else
				return -1;	// 参数非法 
		
		// 添加时间戳
		BSP_RTC_Get_Current(&tmpRtc);	
		count =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);
		tmpPara[paraLen] =(u8)(count >> 24);	
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 16);
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 8);
		paraLen++;
		tmpPara[paraLen] =(u8)(count);
		paraLen++;	
	
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_LOG;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}

s8	HAL_CfgLogInQueueProcess	(u8 ucCfg)
{
		u8 ret =0;
		u8 tmpPara[60]={0};
		u16	paraLen=0;
		u32 count=0;
		SendData_TypeDef sendData;
		SYS_DATETIME	tmpRtc;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
	
		// 参数区赋值
		// 故障类型
		tmpPara[paraLen] =0xFFu;	
		paraLen++;
	
    // 是否发送短信
    tmpPara[paraLen] = 0x00u;	
    paraLen++;
    // 故障次数
    tmpPara[paraLen] = ucCfg;	
    paraLen++;		
 
		
		// 添加时间戳
		BSP_RTC_Get_Current(&tmpRtc);	
		count =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);
		tmpPara[paraLen] =(u8)(count >> 24);	
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 16);
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 8);
		paraLen++;
		tmpPara[paraLen] =(u8)(count);
		paraLen++;	
	
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_LOG;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_DssInQueueProcess
* Description    : DSS事件数据入队处理(目前只有弯道补报做数据上传处理)
* Input          : 
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_DssInQueueProcess	(DSS_TYPE type)
{
		u8 ret =0,i=0;
		GPSProData_Def gpsPro;
		SendData_TypeDef sendData;
		
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset((u8 *)&gpsPro,	0,	sizeof(gpsPro));
	
		if((type != TYPE_DSS_ATH) && (type != TYPE_DSS_DTH) && (type != TYPE_DSS_RTH))
				return 0;
		
		// 参数区赋值
		for(i =0;	(HAL_GpsGetDssData(&gpsPro) != DEF_PROQUE_QUE_EMPTY) && (i<DSS_POINT_NUM);	i++)
		{					
				sendData.sum	= 0;
				sendData.path = DEF_PATH_GPRS;
				sendData.type = DEF_TYPE_GPS;
				sendData.len  = sizeof(GPSProData_Def);
				HAL_GpsProDataOut(sendData.data,	&gpsPro);	// STM32默认为小端模式,数据按大端模式输出
				
				// 数据入发送队列///////////////////////
				ret =HAL_ComQue_in (DEF_FLASH_QUE,	&sendData);	 // 由于DSS为多点GPS数据直接放到RAM中存储不下，所以直接入FLASH队列

				if(ret == DEF_PROQUE_QUE_OK)
						memset((u8 *)&gpsPro,	0,	sizeof(gpsPro));	// 清临时缓冲区
				else
						break;		
		}
		return 0;

}
/*******************************************************************************
* Function Name  : HAL_SMSConRetInQueueProcess
* Description    : 短信控制状态回传GPRS协议打包入队处理
* Input          :
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_SMSConRetInQueueProcess	(SMSCon_Typedef* conSms)
{
	  s8	ret=0;
		u8	tmpBuf[20]={0},tmpBuf1[20]={0};
		u8	tmpParaBuf[160]={0};
		u16 i=0,j=0,p=0,tmpParaLen=0,tmpLen=0;
		u32	count=0;
		SYS_DATETIME	tmpRtc;
		SendData_TypeDef sendData;
		
		memset(tmpBuf,	0,	sizeof(tmpBuf));
		memset(tmpBuf1,	0,	sizeof(tmpBuf1));
		memset(tmpParaBuf,	0,	sizeof(tmpParaBuf));
		memset((u8 *)&sendData,	0,	sizeof(sendData));
				
		// 电话号码长度
		tmpParaBuf[tmpParaLen] = conSms->numLen;
		tmpParaLen++;
		
		// 电话号码
		memcpy((tmpParaBuf + tmpParaLen),	conSms->numBuf ,	conSms->numLen);
		tmpParaLen +=conSms->numLen; 
		
		// 短信内容长度
		tmpParaBuf[tmpParaLen] = conSms->dataLen;
		tmpParaLen++;
		
		// 短信内容
		memcpy((tmpParaBuf + tmpParaLen),	conSms->dataBuf,	conSms->dataLen);
		tmpParaLen +=conSms->dataLen; 
		
		// 内容时间戳
		tmpParaBuf[tmpParaLen] = (u8)(conSms->smsTimStamp >> 24);	
		tmpParaLen++;
		tmpParaBuf[tmpParaLen] = (u8)(conSms->smsTimStamp >> 16);	
		tmpParaLen++;
		tmpParaBuf[tmpParaLen] = (u8)(conSms->smsTimStamp >> 8);	
		tmpParaLen++;
		tmpParaBuf[tmpParaLen] = (u8)(conSms->smsTimStamp);	
		tmpParaLen++;
		
		// 控制结果
		tmpParaBuf[tmpParaLen] =conSms->resultCode;
		tmpParaLen++;
			
		// 时间戳
		BSP_RTC_Get_Current(&tmpRtc);	
		count =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);	
		if((tmpRtc.year >= 2014) && (tmpRtc.year <= 2030))
		{
				// RTC时间有效			
				tmpParaBuf[tmpParaLen] =(u8)(count >> 24);	
				tmpParaLen++;
				tmpParaBuf[tmpParaLen] =(u8)(count >> 16);
				tmpParaLen++;
				tmpParaBuf[tmpParaLen] =(u8)(count >> 8);
				tmpParaLen++;
				tmpParaBuf[tmpParaLen] =(u8)(count);
				tmpParaLen++;
		}
		else
		{
				tmpParaBuf[tmpParaLen] =0x00;	
				tmpParaLen++;
				tmpParaBuf[tmpParaLen] =0x00;
				tmpParaLen++;
				tmpParaBuf[tmpParaLen] =0x00;
				tmpParaLen++;
				tmpParaBuf[tmpParaLen] =0x00;
				tmpParaLen++;
		}
		
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_SMSCOM;	// 短信息车辆控制类型
		sendData.len  = tmpParaLen;
		memcpy(sendData.data,	tmpParaBuf,	tmpParaLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;

}
/*******************************************************************************
* Function Name  : HAL_ReadCarStaInQueueProcess
* Description    : 读取车辆状态返回独立打包处理
* Input          :
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_ReadCarStaInQueueProcess	(u16 ackSn,	u32 reqTimeStmp)
{
	  s8	ret=0;
		u8	tmpBuf[20]={0},tmpNum=0;
		u8	tmpParaBuf[160]={0};
		u16 tmpParaLen=0,tmpLen=0;
		SendData_TypeDef sendData;
			
		memset(tmpBuf,	0,	sizeof(tmpBuf));
		memset(tmpParaBuf,	0,	sizeof(tmpParaBuf));
		memset((u8 *)&sendData,	0,	sizeof(sendData));	
		
		// 应答流水号
		tmpParaBuf[tmpParaLen] = (u8)(ackSn >> 8);
		tmpParaLen++;
		
		tmpParaBuf[tmpParaLen] = (u8)(ackSn);
		tmpParaLen++;
		
		// 应答请求时间戳
		tmpParaBuf[tmpParaLen] = (u8)(reqTimeStmp >> 24);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(reqTimeStmp >> 16);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(reqTimeStmp >> 8);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(reqTimeStmp);
		tmpParaLen++;	
		
		// 添加参数项个数
		tmpNum =HAL_UpdataCarState(tmpBuf,	&tmpLen);
		tmpParaBuf[tmpParaLen] = tmpNum;
		tmpParaLen++;
		
		// 获取奥多车辆状态信息
		memcpy((tmpParaBuf + tmpParaLen),	tmpBuf ,	tmpLen);
		tmpParaLen +=tmpLen; 
		
		
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_CARSTA;	// 车辆状态返回类型
		sendData.len  = tmpParaLen;
		memcpy(sendData.data,	tmpParaBuf,	tmpParaLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;

}
/*******************************************************************************
* Function Name  : HAL_PowOnInQueueProcess
* Description    : 设备上电报文
* Input          :
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_PowOnInQueueProcess	(u8 staByte,	u32 offSetTick,	u32 powOnRtc)
{
	  s8	ret=0;
		u8	tmpParaBuf[20]={0};
		u16 tmpParaLen=0;
		SendData_TypeDef sendData;
			
		memset(tmpParaBuf,	0,	sizeof(tmpParaBuf));
		memset((u8 *)&sendData,	0,	sizeof(sendData));	
		
		// 状态字节
		tmpParaBuf[tmpParaLen] = staByte;
		tmpParaLen++;
		
		// 偏移量
		tmpParaBuf[tmpParaLen] = (u8)(offSetTick >> 24);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(offSetTick >> 16);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(offSetTick >> 8);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(offSetTick);
		tmpParaLen++;	
		
		// 实际上电时间戳
		tmpParaBuf[tmpParaLen] = (u8)(powOnRtc >> 24);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(powOnRtc >> 16);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(powOnRtc >> 8);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(powOnRtc);
		tmpParaLen++;	
		
		// 队列结构赋值 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_POWON;	// 设备上电报文
		sendData.len  = tmpParaLen;
		memcpy(sendData.data,	tmpParaBuf,	tmpParaLen);
		
		// 数据入发送队列///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;

}
/*******************************************************************************
* Function Name  : HAL_SMSInQueueProcess
* Description    : 短信发送入队处理
* Input          : smsMode: DEF_GSM_MODE or DEF_UCS2_MODE
*								 : smsType: DEF_SMS_COM(ASCII码短信) or DEF_SMS_ALARM(发送中文短信)
* Output         : 
* Return         : 正确返回0, 错误返回-1
*******************************************************************************/
s8	HAL_SMSInQueueProcess	(u8 smsMode, u8 smsType,	u8 *num,	u16 numLen,	u8 *pData,	u16 len)
{
		s8	ret =0;
		SendData_TypeDef   smsTmpSend;
	
		if((numLen >= 5) && (numLen < 20) && (len < DEF_PACK_BUF_SIZE) &&
			 ((smsMode == DEF_GSM_MODE) || (smsMode == DEF_UCS2_MODE)) 		&&
			 ((smsType == DEF_SMS_COM) || (smsType == DEF_SMS_ALARM)))
		{
				memset((u8 *)&smsTmpSend,	0,	sizeof(smsTmpSend));
				smsTmpSend.path				=	DEF_PATH_SMS;
				smsTmpSend.smsAlType 	= smsType; 
				smsTmpSend.smsMode 		= smsMode; 
				memcpy((char *)&smsTmpSend.smsNum,	num,	numLen);
				memcpy((char *)&smsTmpSend.data,	pData,	len);
				smsTmpSend.len = len;
				ret =HAL_ComQue_in (DEF_SMS_QUE,	&smsTmpSend);	
				if(ret == DEF_PROQUE_QUE_OK)
						return 0;
				else
						return -1;
		}
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_UpdataCarState
* Description    : 将实际外设车辆状态安协议状态格式输出
* Input          : outBuf:输出buf指针，*len:输出数据长度
* Output         : 
* Return         : 添加的参数项个数
*******************************************************************************/
u8	HAL_UpdataCarState	(u8 *outBuf,	u16	*len)
{
		u16	tmpLen=0;
		STU_CAR_ST	tmpState;
	
		tmpState =HAL_CarStGet();	// 读取当前车辆状态

		// 设防/测防状态
		*(outBuf + tmpLen)	=0xA3;
		tmpLen++;
		*(outBuf + tmpLen)	=0x01;
		tmpLen++;		
		if(tmpState.uiGuardSt == 0)
				*(outBuf + tmpLen)	=0x00;
		else if(tmpState.uiGuardSt == 1)	// 设防
				*(outBuf + tmpLen)	=0x03;
		else if(tmpState.uiGuardSt == 2)	// 撤防
				*(outBuf + tmpLen)	=0x01;
		else
				*(outBuf + tmpLen)	=0x00;
		tmpLen++;
		
		// 发动机开/关状态
		*(outBuf + tmpLen)	=0xA1;
		tmpLen++;
		*(outBuf + tmpLen)	=0x01;
		tmpLen++;		
		if(tmpState.uiEngienSt == 0)
				*(outBuf + tmpLen)	=0x00;
		else if(tmpState.uiEngienSt == 1)	// 启动
				*(outBuf + tmpLen)	=0x03;
		else if(tmpState.uiEngienSt == 2)	// 熄火
				*(outBuf + tmpLen)	=0x01;
		else if(tmpState.uiEngienSt == 3)	// ON
				*(outBuf + tmpLen)	=0x0D;
		else
				*(outBuf + tmpLen)	=0x00;
		tmpLen++;
		
		// 门边开/关状态
		*(outBuf + tmpLen)	=0xA2;
		tmpLen++;
		*(outBuf + tmpLen)	=0x01;
		tmpLen++;		
		if(tmpState.uiLFDoorSt == 0)
				*(outBuf + tmpLen)	=0x00;
		else if(tmpState.uiLFDoorSt == 1)	// 门开
				*(outBuf + tmpLen)	=0x03;
		else if(tmpState.uiLFDoorSt == 2)	// 门关
				*(outBuf + tmpLen)	=0x01;
		else
				*(outBuf + tmpLen)	=0x00;
		tmpLen++;
			
		*len =tmpLen;
		return 3;	// 固定返回3个参数项
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PROTOCOL CONTROL FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/
/*******************************************************************************
* Function Name  : HAL_SendCONProProcess
* Description    : 服务器与控制交互使用(语音播报,控制盒，远程控车，多媒体设备交互)
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
s8	HAL_SendCONProProcess(DEV_CMDCODE_TYPE	cmdType,	u16 proSn,	u8 *pPara,	u16 len)
{
		s8	ret=0;

		// TTS语音控制
		if(cmdType == TTS_CMD)
		{
				if(s_conDev1.dataLock == 0)
				{					
						memset((u8 *)&s_conDev1,	0,	sizeof(s_conDev1));		
						s_conDev1.type 		=cmdType;
						s_conDev1.ackSn		=proSn;
						s_conDev1.paraLen	=len;
						if((len != 0) && (len <= DEF_CONDEVBUF_SIZE))
						{
								memcpy((u8 *)s_conDev1.paraBuf,	pPara,	len);				
								s_conDev1.dataLock	=1;	// 最后改变类型以触发控制执行
								ret =0;
						}
						else
								ret =-1;
				}
				else if(s_conDev2.dataLock == 0)
				{		
						memset((u8 *)&s_conDev2,	0,	sizeof(s_conDev2));		
						s_conDev2.type 		=cmdType;
						s_conDev2.ackSn		=proSn;
						s_conDev2.paraLen	=len;
						if((len != 0) && (len <= DEF_CONDEVBUF_SIZE))
						{
								memcpy((u8 *)s_conDev2.paraBuf,	pPara,	len);			
								s_conDev2.dataLock	=1;	// 最后改变类型以触发控制执行
								ret =0;
						}
						else
								ret =-1;
				}
				else
						ret =-1;
		}
		// 车辆控制
		else if((cmdType == CAR_CMD_IG_ON)		|| (cmdType == CAR_CMD_IG_OFF) || 
			      (cmdType == CAR_CMD_ARM_SET) 	|| (cmdType == CAR_CMD_ARM_RST) ||
						(cmdType == CAR_CMD_FIND_CAR) || (cmdType == CAR_CMD_READSTA) ||
						(cmdType == CAR_CMD_WINUP) 		|| (cmdType == CAR_CMD_WINDOWN)	|| 	
						(cmdType == CAR_CMD_TRUNK))		         
		{
				if(s_devReq.dataLock == 0)
				{
						memset((u8 *)&s_devReq,	0,	sizeof(s_devReq));
						s_devReq.dataLock =1;	// 占用资源
						s_devReq.sn	=UpDevSn();
						s_devReq.cmdCode	=cmdType;
						OSMboxPost(OS_CONReqMbox, (void *)&s_devReq); // 发送DEV请求邮箱
				}
				else
						ret =-1;								
		}
		else
				ret =-1;
		return ret;
}
/*******************************************************************************
* Function Name  : hal_SendOBDProProcess
* Description    : 服务器与OBD交互使用(如手动体检，实时数据流等)
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
s8	HAL_SendOBDProProcess(OBD_CMDCODE_TYPE	cmdCode,	u16 proSn,	u16 cmdId,	u8 *pPara,	u16 len)
{
		s8	ret=0;
		u8	i=0;
		
		for(i=0;	i<40;	i++)
		{
				if(s_obdCmd.dataLock == 0)
						break;
				else
						OSTimeDly(50);
		}
		if(i < 40)
		{
				s_obdCmd.dataLock =1;
				s_obdCmd.cmdCode 		= cmdCode;	
				s_obdCmd.sn 				= UpObdSn();	// 流水号更新
				s_obdCmd.proSn			= proSn;
				s_obdCmd.proCmdId		= cmdId;
				s_obdCmd.proParaLen	=	len;	
				if((len != 0) && (len <= DEF_CMDBUF_SIZE))
						memcpy((u8 *)s_obdCmd.proParaBuf,	pPara, len);
				
				OSMboxPost(OS_OBDReqMbox, (void *)&s_obdCmd); // 发送OBD请求邮箱
				#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[EVENT]: OBD Cmd Data Post OK(%d)!!\r\n",s_obdCmd.cmdCode);
				#endif
				ret= 0;
		}
		else
		{
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[EVENT]: OBD Cmd Data busy(%d)!!\r\n",s_obdCmd.cmdCode);
				#endif
				ret = -1;
		}	
		return ret;
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                     UPDATE PROCESS FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/
/*******************************************************************************
* Function Name  : HAL_DownLoadFileProcess
* Description    : 下载升级文件流程处理
* Input          : times: 发送错误的尝试次数
* Output         : 
* Return         : 
*******************************************************************************/
u8	HAL_DownLoadFileProcess (u8 times)
{
		INT8U	os_err=0;
		u8	loops=0,retCode=DEF_IAP_NONE_ERR;
		u16	i=0,num=0,tmpLen=0,tmpSend=0,	cunU16Posi=0;
		u16 tmpRev=0,	tmpSn=0,allPacks=0,cunPacks=0;
		u8	packRevBuf[256]={0};	// 每字节对应1k程序数据包是否成功接收
		u8	tmpSendBuf[300]={0};	// 临时发送缓冲区 
		u8  tmpParaBuf[280]={0};	// 临时参数缓冲区
		u32	tmpPosiAdd=0;	// 接收到的升级文件起始地址位置
		vu32	downLoadCounter=0;	// 下载升级文件计时器
		UpdateData_TypeDef	*rxdMsg;
		
		// 升需要存储的信息
		u8	nameLen=0,  fileName[30]={0},	verb[6]={0},	carInfo[3]={0,0,0}, sizeBuf[8]={0};	
		u32	fileSize=0,	tmpVerbAddr =0,	tmpSizeAddr=0,	tmpAppAddr=0,	cunAppAddr=0;
		
		// 计时开始
		OSTmr_Count_Start(&downLoadCounter);
			
		// 升级主流程///////////////////////////////////////////////////////////////////////////////////////////////		
		for(loops=0;	loops<times;	loops++)
		{
				retCode =DEF_IAP_NONE_ERR;
				for(i=0;	i<30000;	i++)
				{
						if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
								break;
						else
								OSTimeDly(10);
				}
				if(i >= 30000)
				{
						#if(DEF_GPRSINFO_OUTPUTEN > 0)							
						if(dbgInfoSwt & DBG_INFO_GPRS)
								myPrintf("\r\n[GPRS-Update]: Net break!!\r\n");
						#endif
						retCode =DEF_IAP_NET_ERR;	// 联网或数据发送错误
						continue;		// 下一次尝试
				}
				
				// 清通用应答邮箱
				rxdMsg = (UpdateData_TypeDef *)(OSMboxAccept(OS_DownACKMbox));	// 清空邮箱
				rxdMsg = (void *)0;
				
				// 发送升级请求01AD/////////////////////////////////////////////////////////////////////////////////////
				tmpSend =0;
				memset((u8 *)&tmpSendBuf,	0,	sizeof(tmpSendBuf));
				tmpSn =UpDataSn();
				ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_UPDATEREQ_DEV,	(u8 *)s_common.id,	6,	tmpSendBuf,	&tmpSend);	
				s_update.writeLock =0;	// 数据解锁		

				for(i=0;	i<10;	i++)
				{
						if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	tmpSendBuf,	tmpSend,	3000) == DEF_GSM_NONE_ERR)
								break;
						else
								OSTimeDly(500);			
				}
				if(i >= 10)
				{
						#if(DEF_GPRSINFO_OUTPUTEN > 0)							
						if(dbgInfoSwt & DBG_INFO_GPRS)
								myPrintf("\r\n[GPRS-Update]: Send 01AD error!!\r\n");
						#endif
						retCode =DEF_IAP_NET_ERR;	// 联网或数据发送错误					
				}
				else
				{
						#if(DEF_GPRSINFO_OUTPUTEN > 0)							
						if(dbgInfoSwt & DBG_INFO_GPRS)
								sendPrintProcess (0,	tmpSendBuf,	tmpSend);
						#endif
				}
				// 错误处理
				if(retCode !=DEF_IAP_NONE_ERR)
				{
						BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
						continue;
				}
				
				// 等待通用应答6001/////////////////////////////////////////////////////////////////////////////////////				
				rxdMsg = (UpdateData_TypeDef *)(OSMboxPend(OS_DownACKMbox, 10000, &os_err));	
				if((os_err != OS_TIMEOUT)&&(rxdMsg != (void *)0))  
				{
						if(rxdMsg->msgId == DEF_CMDID_COMACK_PLA)
						{							
								if(((rxdMsg->dataBuf[0]<<8) | (rxdMsg->dataBuf[1])) != tmpSn)
								{
										#if(DEF_GPRSINFO_OUTPUTEN > 0)							
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("\r\n[GPRS-Update]: Receive 6001 sn error!!\r\n");
										#endif
										retCode =DEF_IAP_NET_ERR;	// 应答流水号错误
								}
						}
						else if(rxdMsg->msgId == DEF_CMDID_UPDATEINFO_PLA)
						{
								// 如果收到0125将不等待通用应答直接继续升级流程(避免6001与0125的粘包导致的重复错误)
								#if(DEF_GPRSINFO_OUTPUTEN > 0)							
								if(dbgInfoSwt & DBG_INFO_GPRS)
										myPrintf("\r\n[GPRS-Update]: Receive 0125 continue!!\r\n");
								#endif
								retCode =DEF_IAP_NONE_ERR;
						}
						else
						{	
								#if(DEF_GPRSINFO_OUTPUTEN > 0)							
								if(dbgInfoSwt & DBG_INFO_GPRS)
										myPrintf("\r\n[GPRS-Update]: Send 01AD Pend 6001 error ID(0x%04X)!!\r\n",rxdMsg->msgId);
								#endif
								retCode =DEF_IAP_NET_ERR;	// 非法应答ID
						}
				}
				else 
				{
						if(os_err	== OS_TIMEOUT)
						{
								#if(DEF_GPRSINFO_OUTPUTEN > 0)							
								if(dbgInfoSwt & DBG_INFO_GPRS)
										myPrintf("\r\n[GPRS-Update]: Send 01AD Pend 6001 timeout!!\r\n");
								#endif
								retCode =DEF_IAP_NET_ERR;	// 应答超时
						}
				}
				// 错误处理
				if(retCode !=DEF_IAP_NONE_ERR)
				{
						BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
						continue;
				}	
				
				// 接收升级信息0125/////////////////////////////////////////////////////////////////////////////////////
				while(1)
				{
						s_update.writeLock =0;	// 数据解锁		
						rxdMsg = (UpdateData_TypeDef *)(OSMboxPend(OS_DownACKMbox, 60000, &os_err));	
						if((os_err != OS_TIMEOUT)&&(rxdMsg != (void *)0)) 
						{
								if(rxdMsg->msgId == DEF_CMDID_UPDATEINFO_PLA)
								{
										// 是否为断点续传//////////////////////////////
										if((strlen((const char *)fileName) != 0) && (strlen((const char *)fileName) < sizeof(fileName)) &&
											 (strlen((const char *)verb) != 0) 		 && (strlen((const char *)verb) < sizeof(verb)) 				&&
											 (memcmp(fileName,	(rxdMsg->dataBuf+4),	rxdMsg->dataBuf[3]) == 0))
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Continue receive updata pack!!\r\n");
												#endif
												retCode = DEF_IAP_XUCHUAN;	// 需要断点续传
												break;
										}									
										// 全新升级////////////////////////////////////
										else
										{											 
												// 参数清空
												tmpRev =0;
												tmpLen =0; fileSize =0; 	
												memset(carInfo,	0,	sizeof(carInfo));
												memset(fileName,0,	sizeof(fileName));
												memset(verb,		0,	sizeof(verb));
												memset(packRevBuf,	0,	sizeof(packRevBuf));	// 清数据包接收记录
												
												// 提取升级信息
												tmpLen=0;
												carInfo[0] = rxdMsg->dataBuf[tmpLen];	// 车系
												tmpLen++;
												carInfo[1] = rxdMsg->dataBuf[tmpLen];	// 车型
												tmpLen++;
												carInfo[2] = rxdMsg->dataBuf[tmpLen];	// 发动机品牌
												tmpLen++;
												nameLen		 = rxdMsg->dataBuf[tmpLen];	// 文件名长
												tmpLen++;
												for(i=0;	(i<sizeof(fileName)) && (i<nameLen);	i++)
												{
														fileName[i] = rxdMsg->dataBuf[tmpLen+i]; // 文件名
												}
												fileName[i] ='\0';
												tmpLen +=i;
												for(i=0;	(i<sizeof(verb)) && (i<5);	i++)
												{
														verb[i] = rxdMsg->dataBuf[tmpLen+i]; // 版本号
												}
												verb[i] ='\0';
												tmpLen +=i;
												fileSize = (u32)((rxdMsg->dataBuf[tmpLen] << 24) | (rxdMsg->dataBuf[tmpLen+1] << 16) | (rxdMsg->dataBuf[tmpLen+2] << 8) | rxdMsg->dataBuf[tmpLen+3]);
												
												// 长度合法性判断
												if((fileSize == 0) || (fileSize > FLASH_CPU_APP_SIZE))
												{	
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: 0125 file size error(%d)!!\r\n",	fileSize);
														#endif
														retCode = DEF_IAP_FILE_SIZE_ERR;	// 升级文件长度非法直接退出		
														break;
												}	
												
												// 计算一共需要接收的数据包数
												cunPacks =0;	// 当前接收升级包复位
												if(fileSize%1024 == 0)
														allPacks =fileSize/1024;	
												else
														allPacks =fileSize/1024 + 1;	
												
												// 升级准备工作//////////////////////////////////////////////////////////////
												// 读取上一次升级文件存储位置
												cunU16Posi =0;
												for(i=0; i<5;	i++)
												{
														if(BSP_SPI_FLASH_BufferRead (FLASH_SPI_FLAG2_START, (u8*)&cunU16Posi, 2,	5000) == DEF_SPI_NONE_ERR)
																break;
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Read flash error!!\r\n");
														#endif
														retCode =DEF_IAP_WFLASH_READ_ERR;	// 外部FLASH读取错误
														break;
												}
												
												// 判断位置合法性
												if((cunU16Posi != DEF_IAP_BACKUP_POSI1) && (cunU16Posi != DEF_IAP_BACKUP_POSI2))
														cunU16Posi =DEF_IAP_BACKUP_POSI1;	// 位置非法复位到位置1

												// 写升级进行中标志(如果复位后可继续升级)
												
												// 擦除外部FLASH对应存储区
												if(cunU16Posi == DEF_IAP_BACKUP_POSI1)
												{
														tmpVerbAddr = FLASH_SPI_NAME2_START;			// 当前在位置1则备份到位置2	
														tmpSizeAddr = FLASH_SPI_SIZE2_START;
														tmpAppAddr  = FLASH_SPI_APP2_START;
														cunU16Posi	= DEF_IAP_BACKUP_POSI2;				// 修改当前程序存储位置到2
												}
												else
												{
														tmpVerbAddr = FLASH_SPI_NAME1_START;			// 当前在位置2则备份到位置1
														tmpSizeAddr = FLASH_SPI_SIZE1_START;
														tmpAppAddr  = FLASH_SPI_APP1_START;
														cunU16Posi	= DEF_IAP_BACKUP_POSI1;				// 修改当前程序存储位置到1
												}
												// 擦除版本号字段
												for(i=0; i<5;	i++)
												{
														if(BSP_IAP_Erase_WFlash (tmpVerbAddr,	FLASH_SPI_NAME1_SIZE) == 0)
																break;
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Erase flash error!!\r\n");
														#endif
														retCode =DEF_IAP_WFLASH_ERASE_ERR;	// 外部FLASH擦除错误
														break;
												}
												// 擦除文件大小字段
												for(i=0; i<5;	i++)
												{
														if(BSP_IAP_Erase_WFlash (tmpSizeAddr,	FLASH_SPI_SIZE1_SIZE) == 0)
																break;
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Erase flash error!!\r\n");
														#endif
														retCode =DEF_IAP_WFLASH_ERASE_ERR;	// 外部FLASH擦除错误
														break;
												}
												// 擦除程序存储区
												for(i=0; i<5;	i++)
												{
														if(BSP_IAP_Erase_WFlash (tmpAppAddr,	FLASH_SPI_APP1_SIZE) == 0)
																break;
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Erase flash error!!\r\n");
														#endif
														retCode =DEF_IAP_WFLASH_ERASE_ERR;	// 外部FLASH擦除错误
														break;
												}
												retCode =DEF_IAP_NONE_ERR;	// 无错误继续执行				
										}									
								}
								else
								{
										#if(DEF_GPRSINFO_OUTPUTEN > 0)							
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("\r\n[GPRS-Update]: Pend 0125 id error(0x%04X)!!\r\n",	rxdMsg->msgId);
										#endif
										retCode =DEF_IAP_NET_ERR;	// 服务器升级数据包错误
										break;
								}														
						}
						else
						{
								#if(DEF_GPRSINFO_OUTPUTEN > 0)							
								if(dbgInfoSwt & DBG_INFO_GPRS)
										myPrintf("\r\n[GPRS-Update]: Receive 0125 timeout!!\r\n");
								#endif
								retCode =DEF_IAP_NET_ERR;	// 接收升级文件信息超时		
								break;
						}
						break;
				}
				// 错误处理
				if((retCode !=DEF_IAP_NONE_ERR) && (retCode !=DEF_IAP_XUCHUAN))
				{
						BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
						continue;
				}
				
				// 发送通用应答6001////////////////////////////////////////////////////////////////////////////////////////
				tmpSend =0;
				memset((u8 *)&tmpParaBuf,	0,	sizeof(tmpParaBuf));
				memset((u8 *)&tmpSendBuf,	0,	sizeof(tmpSendBuf));
				
				tmpLen =0x03;				
				tmpParaBuf[0] = (u8)(rxdMsg->sn >> 8);	// 应答SN
				tmpParaBuf[1] = (u8)(rxdMsg->sn);	
			
				if(retCode == DEF_IAP_NONE_ERR)
						tmpParaBuf[2] = 0x00;	// 无错误
				else if(retCode == DEF_IAP_XUCHUAN)
						tmpParaBuf[2] = 0x03;	// 应答需要补报数据
				else
						tmpParaBuf[2] = 0x01;	// 应答错误
				
				retCode =DEF_IAP_NONE_ERR;
				ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_COMACK_DEV,	tmpParaBuf,	tmpLen,	tmpSendBuf,	&tmpSend);		
				s_update.writeLock	 =0;	// 允许写入		

				for(i=0;	i<10;	i++)
				{
						if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	tmpSendBuf,	tmpSend,	3000) == DEF_GSM_NONE_ERR)
								break;
						else
								OSTimeDly(500);			
				}
				if(i >= 10)
				{
						#if(DEF_GPRSINFO_OUTPUTEN > 0)							
						if(dbgInfoSwt & DBG_INFO_GPRS)
								myPrintf("\r\n[GPRS-Update]: Erase flash error!!\r\n");
						#endif
						retCode =DEF_IAP_NET_ERR;	// 联网或数据发送错误
				}
				else
				{
						#if(DEF_GPRSINFO_OUTPUTEN > 0)							
						if(dbgInfoSwt & DBG_INFO_GPRS)
								sendPrintProcess (0,	tmpSendBuf,	tmpSend);
						#endif
				}
				// 错误处理
				if(retCode !=DEF_IAP_NONE_ERR)
				{
						BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
						continue;
				}
				
				// 接收升级信息0126/////////////////////////////////////////////////////////////////////////////////////
				retCode =DEF_IAP_NONE_ERR;
				s_update.writeLock =0;	// 数据解锁		
				while(1)
				{		
						for(i=0;	(i < 600);	i++)
						{
								rxdMsg = (UpdateData_TypeDef *)(OSMboxAccept(OS_DownACKMbox));	
								if(rxdMsg != (void *)0)  
										break;
								else
								{
										if((i == 250) || (i == 500))
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: No \"+CIPRXGET: 1,n\", will auto read!!\r\n");
												#endif
												OSSemPost(GSMGprsDataSem);		// 手动查询是否已经接收到数据			
										}
								}				
								// 判断设备是否已经断网(接收过程中错误达到一定程度服务器断开设备)
								if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
										break;								
								OSTimeDly(100);				// 扫描间隔										
						}
						if(rxdMsg != (void *)0)
						{
								if(rxdMsg->msgId == DEF_CMDID_UPDATEDATA_PLA)
								{
										// 收到升级文件
										tmpPosiAdd = 0;
										tmpPosiAdd = (u32)((rxdMsg->dataBuf[0] << 24) | 
																			 (rxdMsg->dataBuf[1] << 16) | 
																			 (rxdMsg->dataBuf[2] << 8) 	| 
																			 (rxdMsg->dataBuf[3]));
										if((tmpPosiAdd % 1024) == 0)
												tmpRev = tmpPosiAdd/1024;					// 取起始位置字段
										else
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Start addr error(0x%X)!!\r\n",tmpPosiAdd);
												#endif
												retCode = DEF_IAP_START_SIZE_ERR;	// 起始位置数据错误
												break;
										}	
										
										if(tmpRev>512)
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Start posi overflow(%d)!!\r\n",tmpRev);
												#endif
												retCode = DEF_IAP_START_SIZE_ERR;	// 起始位置溢出错误
												break;
										}										
										tmpLen =(u16)((rxdMsg->dataBuf[4] << 8) | rxdMsg->dataBuf[5]);	// 取数据包长度
										if(tmpLen>1024)
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Pack len error(%d)!!\r\n",tmpLen);
												#endif
												retCode = DEF_IAP_PACK_SIZE_ERR;	// 单数据包长度错误
												break;
										}
										
										// 程序存储到外部FLASH
										if(packRevBuf[tmpRev] == 0)
										{													
												for(i=0;i<5;i++)
												{
														if(BSP_IAP_ProgramBuf_to_WFlash ((rxdMsg->dataBuf + 6),	tmpLen,	(tmpAppAddr + (tmpRev * 1024))) == 0)
																break;
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Write posi %d error!\r\n",	tmpRev);
														#endif
														retCode = DEF_IAP_WFLASH_WRITE_ERR;	// 外部FLASH写错误												
														break;
												}
												// 记录接收成功的数据包
												cunPacks ++;	// 当前接收成功数据包++
												packRevBuf[tmpRev] = 0x01;	// 1表示该位置已经成功写入程序文件

												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Rxd posi= %d, pack len=%d, (%d%%)!\r\n",	tmpRev,tmpLen,(u16)(cunPacks*100/allPacks));
												#endif										
										}
										else
										{
												// 已经成接收了该数据包
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Rxd posi= %d, is repeat pack!\r\n",	tmpRev);
												#endif
										}
								}	
								else if(rxdMsg->msgId == DEF_CMDID_UPDATEQUE_PLA)
								{
										// 收到升级成功查询
										num =0;
										memset(tmpParaBuf,	0,	sizeof(tmpParaBuf));
										tmpSend =0;
										memset((u8 *)&tmpSendBuf,	0,	sizeof(tmpSendBuf));
										
										tmpParaBuf[0] = (u8)(rxdMsg->sn >> 8);	// 应答SN
										tmpParaBuf[1] = (u8)(rxdMsg->sn);	
										tmpParaBuf[2] = 0x00;	// 写故障状态									
										for(i=0;	i<allPacks;	i++)
										{
												if(packRevBuf[i] == 0x00)
												{														
														tmpParaBuf[4+num] =i;	// 记录未存储成功数据包位置
														num++;
												}													
										}										
										tmpParaBuf[3] = num;	// 记录未成功接收数据包总数
										tmpSn =UpDataSn();
										// 打包应答包
										ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_UPDATEACK_DEV,	tmpParaBuf,	(num+4),	tmpSendBuf,	&tmpSend);
										s_update.writeLock =0;	// 数据解锁		
										
										for(i=0;	i<10;	i++)
										{
												if(BSP_TCPSendData	(DEF_GPRS_DATA_CNN0,	tmpSendBuf,	tmpSend,	3000) == DEF_GSM_NONE_ERR)
														break;
												else
														OSTimeDly(500);			
										}
										if(i >= 10)
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Receive 0131 send 01AF timeout!!\r\n");
												#endif
												retCode =DEF_IAP_NET_ERR;	// 联网或数据发送错误
												break;
										}
										else
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														sendPrintProcess (0,	tmpSendBuf,	tmpSend);
												#endif
										}
										
										if(num == 0)
										{
												// 所有数据包接收完成，写车型车系，及版本信息
												// 写当前程序存储位置
												for(i=0; i<10; i++)
												{
														if(BSP_IAP_Erase_WFlash (FLASH_SPI_FLAG2_START,	FLASH_SPI_FLAG2_SIZE) == 0)
														{
																if(BSP_IAP_ProgramBuf_to_WFlash ((u8*)&cunU16Posi,	2,	FLASH_SPI_FLAG2_START) == 0)
																		break;
														}
												}
												if(i>=10)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Receive finish Erase wflash posi error !!\r\n");
														#endif
														retCode =DEF_IAP_WFLASH_WRITE_ERR;	// 外部FLASH写错误
														break;	// 退出重新尝试
												}
												// 写网络版本号
												for(i=0; i<5; i++)
												{
														if(BSP_IAP_ProgramBuf_to_WFlash (verb,	6,	tmpVerbAddr) == 0)	
																break;	// 写入时加入1字节结束符
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Receive finish Write verb error !!\r\n");
														#endif
														retCode =DEF_IAP_WFLASH_WRITE_ERR;	// 外部FLASH写错误
														break;	// 退出重新尝试
												}
												// 写文件大小
												memset(sizeBuf, 0, sizeof(sizeBuf));
												sprintf((char *)sizeBuf, "%d",	fileSize);	// 外部FLASH文件长度字段统一使用字符串方式存储			
												for(i=0; i<5;	i++)
												{																								
														if(BSP_IAP_ProgramBuf_to_WFlash (sizeBuf,	(strlen((const char *)sizeBuf)+1),	tmpSizeAddr) == 0)
																break;	// 写入时加入1字节结束符
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Receive finish Write size error !!\r\n");
														#endif
														retCode =DEF_IAP_WFLASH_WRITE_ERR;	// 外部FLASH写错误
														break;	// 退出重新尝试
												}
												// 写车型车系发动机品牌及改写升级标志触发烧写处理
												HAL_CommonParaReload();
												s_common.carInfo[0] = carInfo[0];
												s_common.carInfo[1] = carInfo[1];
												s_common.carInfo[2] = carInfo[2];	
												s_common.iapSta =DEF_IAP_CLEAN_FLAG;	// 清升级标志
												if(cunU16Posi	== DEF_IAP_BACKUP_POSI1)
														s_common.iapFlag =DEF_IAP_PRO1MID_FLAG;	// 改写IAP标志
												else
														s_common.iapFlag =DEF_IAP_PRO2MID_FLAG;
												
												for(i=0; i<5;	i++)
												{														
														if(HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)) == DEF_PARA_NONE_ERR)
																break;
														else
																i++;
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Receive finish Write car info error !!\r\n");
														#endif
														retCode =DEF_IAP_CFLASH_WRITE_ERR;	// CPU FLASH参数写错误
														break;	// 退出重新尝试
												}
												retCode = DEF_IAP_NONE_ERR;
												break;
										}												
								}
								else
								{		
										#if(DEF_GPRSINFO_OUTPUTEN > 0)							
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("\r\n[GPRS-Update]: Receive error id(0x%04X) !!\r\n",rxdMsg->msgId);
										#endif
										retCode =DEF_IAP_NET_ERR;	// 接收非法的命令ID直接退出
										break;
								}		
								s_update.writeLock =0;	// 数据解锁		
						}
						else
						{
								#if(DEF_GPRSINFO_OUTPUTEN > 0)							
								if(dbgInfoSwt & DBG_INFO_GPRS)
										myPrintf("\r\n[GPRS-Update]: Pend 0126 timeout or server break!!\r\n");
								#endif
								retCode =DEF_IAP_NET_ERR;	// 接收升级文件超时
								break;	
						}
				}
				if(retCode == DEF_IAP_NONE_ERR)
						break;		// 成功返回
				else
				{
						BSP_GPRSFroceBreakSet();	// 设置强制断网使能重新连接网络
						continue;
				}
		}
		if(retCode != DEF_IAP_NONE_ERR)
		{
				// 达到尝试次数清升级标志下次不再进入
        HAL_CommonParaReload();
				s_common.iapSta =DEF_IAP_CLEAN_FLAG;	// 清升级标志
				HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));
		}
		// 计时结束//////////////////////////////////////////////////////////////////////////////////////////////////////////////
		tmpPosiAdd =OSTmr_Count_Get(&downLoadCounter)/1000;
		#if(DEF_GPRSINFO_OUTPUTEN > 0)							
		if(dbgInfoSwt & DBG_INFO_GPRS)
				myPrintf("\r\n[GPRS-Update]: Spend time %ds!\r\n",	tmpPosiAdd);
		#endif
		
		return retCode;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PROTOCOL SMS CONTROL FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/
/*******************************************************************************
* Function Name  : HAl_SMSTimeOut
* Description    : 打包控制超时
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
s8	HAl_SMSTimeOut(u8 *pSrcBuf, u16 srcLen,	u8 *pRetBuf,	u16 *retLen)
{
		strcat((char *)pRetBuf,	(char *)pSrcBuf);
		strcat((char *)pRetBuf,	(char *)"(ERR-Timeout)!");	
		*retLen = strlen((const char *)pRetBuf);
		return 0;
}
/*******************************************************************************
* Function Name  : HAl_SMSCarControl
* Description    : 短信车辆控制处理(大灯，喇叭，控制开启，关闭等)
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
s8	HAl_SMSCarControl(u8 *pCmdCode,	u16 cmdCodeLen)
{
		u8 typeCode=0;
		DEV_CMDCODE_TYPE cmdType=0;
		typeCode =*pCmdCode >> 4;
		if(typeCode == 0x00)
				return HAL_SendOBDProProcess(CMD_SMS,	0x0000,	DEF_CMDID_CONCAR_PLA,	pCmdCode,	cmdCodeLen);	// 请求OBD控制车辆
		else if(typeCode == 0x0A)
		{
				if((*pCmdCode == 0xA1) && (*(pCmdCode + 1) == 0x00))	
						cmdType =CAR_CMD_CATCH_CAR;
				else if((*pCmdCode == 0xA1) && (*(pCmdCode + 1) == 0x01))
						cmdType =CAR_CMD_FREE_CAR;
				else if((*pCmdCode == 0xA3) && (*(pCmdCode + 1) == 0x00))
						cmdType =CAR_CMD_IG_ON;
				else if((*pCmdCode == 0xA3) && (*(pCmdCode + 1) == 0x01))
						cmdType =CAR_CMD_IG_OFF;
				else if((*pCmdCode == 0xA4) && (*(pCmdCode + 1) == 0x00))
						cmdType =CAR_CMD_FIND_CAR;
				else if((*pCmdCode == 0xA5) && (*(pCmdCode + 1) == 0x01))
						cmdType =CAR_CMD_ARM_SET;
				else if((*pCmdCode == 0xA5) && (*(pCmdCode + 1) == 0x00))
						cmdType =CAR_CMD_ARM_RST;
				else if((*pCmdCode == 0xA6) && (*(pCmdCode + 1) == 0x00))
						cmdType =CAR_CMD_TRUNK;
				else if((*pCmdCode == 0xAA) && (*(pCmdCode + 1) == 0x01))
						cmdType =CAR_CMD_WINUP;
				else if((*pCmdCode == 0xAA) && (*(pCmdCode + 1) == 0x00))
						cmdType =CAR_CMD_WINDOWN;
		
				if(s_cfg.devEn & DEF_DEVEN_BIT_CON)
						return HAL_SendCONProProcess(cmdType,	0x0000,	(u8 *)"",	0);	// 请求外设控制
				else
						return DEF_SMSCONTROL_NOSUPORT;	// 禁止外设控制(返回不支持)
		}
		else
				return -1;
}

/*****************************************end*********************************************************/



