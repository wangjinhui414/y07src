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

// ˵����
// ֧������ͨ3.0Э��汾

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
static			vu16			obdSn=0;		// OBD������ˮ��
static			vu16			proSn=0;		// Э����ˮ��
static			vu16			devSn=0;		// ���轻����ˮ��
static			vu16			proUpSn=0;	// ��һ��ָ����ˮ��
static			vu8				igOnSaveBuf[18]={0};	// ����Ĳ�����������
static      SMS_Typedef simulateSms;

/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

uc16 CRC_TABLE_XW[256] =
{
    /* CRC���ұ� */
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
extern			OS_EVENT							*GSMGprsDataSem;			// GPRS���������ź���
extern			OS_EVENT							*OS_DownACKMbox;			// �豸��������Ӧ������
extern			OS_EVENT							*OS_ACKMbox;					// Ӧ���н���Ӧ������				
extern			OS_EVENT							*OS_OBDReqMbox;				// ����OBD��������
extern			OS_EVENT							*OS_RecSmsMbox;				// �յ�SMS�����ͽ���
extern			OS_EVENT							*OS_CONReqMbox;				// ���������������
extern			SendData_TypeDef			comAck;								// ���ڴ洢ͨ��Ӧ����ʱ�������ṹ
extern			SMSCon_Typedef				conSms;								// ������ʱ�洢���ſ�������


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


// ������������Ӧ����Ӻ���
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

// �������ͨ�ô�����
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
* Description    : ��ȡ"adc,efg,hlm"�ַ�����ʽ�Ĳ������� 
* Input          : index��ʾ���ȡ�ڼ�������(>=1)
* Output         : 
* Return         : ���س�����Ϣ, ����Χ0xffff
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
				*(outBuf + i) ='\0';	// ��ӽ�����
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
										i =i+1;	// ����','
										for(p=0;	(i<srcLen) && (*(srcBuf + i) != ',');	i++,p++)
										{
												*(outBuf + p)	=	*(srcBuf + i);			
										}
										*(outBuf + p) ='\0';	// ��ӽ�����
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
* Description    : ��ȡCRCУ����
* Input          : 
* Output         : 
* Return         : ���س�����Ϣ, ����Χ0xffff
*******************************************************************************/
static	u16 GetCheckCRC_XW(u8 *pData, u16 len)
{
    u16 i=0;
    u16 crc = 0xffff;

    for ( i = 0; i < len; i++ )
    {
        crc = ( crc >> 8 ) ^ CRC_TABLE_XW[( crc ^ pData[i] ) & 0xff];
    }
    return __REV16(crc);	// ȡ������
}
/*******************************************************************************
* Function Name  : ZXT_PackCheck_Process
* Description    : Э��ְ���⴦��(�������ճ�Ͱ���ķ���ֵ>1)
* Input          : 
* Output         : 
* Return         : û��:����0, ��ȷ:����һ֡����������Э���ʽ�����ݰ�����
*******************************************************************************/
static	u8	ZXT_PackCheck_Process (u8 *pBuf,	u16 len)
{
		u8 packs=0;
		u16 i=0,tmpLen=0,tmpCRC=0;
	
		for(i=0; i<len; i++)
		{
				if((*(pBuf + i) == DEF_ZXT_PRO_START) && (*(pBuf + i + 1) == DEF_ZXT_PRO_PROFLAG))
				{
						tmpLen = (u16)((*(pBuf + i + 2) << 8) | *(pBuf + i + 3));	// ��ȡЭ�鳤���ֶ�
						tmpCRC = (u16)((*(pBuf + i + 4 + tmpLen - 2) << 8) | *(pBuf + i + 4 + tmpLen - 1));
						
						if((tmpLen < 2) || (tmpLen >= 2000))
								continue;
					
						if(tmpCRC == GetCheckCRC_XW((pBuf + i + 4),	(tmpLen - 2)))
						{
								packs++;
								// ������������
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
* Description    : �õ�һ֡���ݰ�����ָ����Э�������λ��(����ճ�Ͱ�ʱʹ��)
* Input          : posi :���ݰ�λ��(��1~255��)
* Output         : 
* Return         : ����:����0xffff, ��ȷ:λ��
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
								// ������������
								i = i + 4 + tmpLen - 1;
						}
				}
		}
		return 0xffff;
}

/*******************************************************************************
* Function Name  : ZXT_PackAnalyze_Process
* Description    : Э���������
* Input          : 
* Output         : 
* Return         : ����0, ���󷵻�0xffff
*******************************************************************************/
static	s8 ZXT_PackAnalyze_Process (u8	*srcBuf,	u16 srcLen,	Protocol_Typedef *pPro)
{
		u16 tmpCRC=0;
		u16	tmpLen=0;
	
		if((*srcBuf != DEF_ZXT_PRO_START) || (*(srcBuf + 1) != DEF_ZXT_PRO_PROFLAG))
				return	DEF_PRO_UNPACK_ERRSTART;	// Э��ͷ����
		
		tmpLen = (u16)((*(srcBuf + 2) << 8) | *(srcBuf + 3));
		if(tmpLen != (srcLen - 4))
				return	DEF_PRO_UNPACK_ERRLEN;	// Э�鳤�ȴ���
			
		tmpCRC = (u16)((*(srcBuf + srcLen - 2) << 8) | *(srcBuf + srcLen - 1));
		if(tmpCRC != GetCheckCRC_XW((srcBuf + 4),	(srcLen - 6)))
				return	DEF_PRO_UNPACK_ERRCRC;	// CRCУ�����
		
		// ��ʼ��ȡЭ������
		pPro->start 		=*srcBuf;
		pPro->proFlag		=*(srcBuf + 1);
		pPro->proLen		=tmpLen;
		pPro->bodySn		=(u16)((*(srcBuf + 4) << 8) | *(srcBuf + 5));
		pPro->bodycmdId	=(u16)((*(srcBuf + 6) << 8) | *(srcBuf + 7));
		if((srcLen > 10) && (srcLen <= DEF_PARABUF_SIZE))
		{
				// ��ȡ����������
				memcpy((pPro->paraData),	(srcBuf + 8),	(srcLen - 10));
		}
		pPro->paraLen = srcLen - 10;	// ��ȡ��������
		pPro->crcCode = tmpCRC;
		
		return	DEF_PRO_UNPACK_NONE;
}
/*******************************************************************************
* Function Name  : ZXT_PackBuild_Process
* Description    : ͨ��Э��������
* Input          : 
* Output         : 
* Return         : ����0, ���󷵻�0xffff
*******************************************************************************/
static	s8 ZXT_PackBuild_Process (u16 sn,	u16 cmdId,	u8 *pPara,	u16 paraLen,	u8 *pOutBuf,	u16 *outLen)
{
		u16	i=0,p=0,tmpCRC=0,tmpLen=0;
		
		tmpLen = 6 + paraLen;
		*pOutBuf 					= DEF_ZXT_PRO_START;		p +=1;	// Э��ͷ		
		*(pOutBuf + p) 		= DEF_ZXT_PRO_PROFLAG;	p +=1;	// ��Ϣ��ʶ
	  *(pOutBuf + p)		= (u8)((tmpLen & 0xff00) >> 8);		p +=1;	// ���ȸ߸�ֵ
		*(pOutBuf + p)		= (u8)(tmpLen & 0x00ff);					p +=1;	// ���ȵ͸�ֵ
	  *(pOutBuf + p)		= (u8)((sn & 0xff00) >> 8);		p +=1;	// ��ֵ��ˮ�Ÿ�
		*(pOutBuf + p)		= (u8)(sn & 0x00ff);		p +=1;	// ��ֵ��ˮ�ŵ�
		*(pOutBuf + p)		= (u8)((cmdId & 0xff00) >> 8);		p +=1;	// ��ֵ����ID��
		*(pOutBuf + p)		= (u8)(cmdId & 0x00ff);		p +=1;	// ��ֵ����ID��
		
		// ��ֵ����������
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
* Description    : �������IP���ݴ�� 
* Input          : 
* Output         : 
* Return         : ����0, ���󷵻�0xffff
*******************************************************************************/
s8 HAL_BuildNavi_Process (u8 *pOutBuf,	u16 *outLen)
{
		u8 tmpPara[60]={0};
		u16 paraLen=0,i=0;	
		u32	filename = FLASH_CPU_APPNAME_START;
		
		// �����������	
		// �豸ID(IDC)
		memcpy((tmpPara + paraLen),	s_common.id,	PRA_SIZEOF_COMMON_ID);
		paraLen	+=PRA_SIZEOF_COMMON_ID;
		
		// ���ͳ�ϵ���ͻ���
		memcpy((tmpPara + paraLen),	s_common.carInfo,	PRA_SIZEOF_COMMON_CARINFO);
		paraLen	+=PRA_SIZEOF_COMMON_CARINFO;
		
		// ����汾��
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
* Description    : ���ִ������
* Input          : 
* Output         : 
* Return         : ����0, ���󷵻�0xffff
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
		
		// �����������	
		// ͨ��Э��汾
		tmpPara[paraLen] = 0x03;		// 2015-10-16 3.0Э��汾
		paraLen++;
		
		// ����ID
		tmpPara[paraLen] = DEF_COMPANY_ID;
		paraLen++;
	
		// �豸ID(IDC)
		memcpy((tmpPara + paraLen),	s_common.id,	PRA_SIZEOF_COMMON_ID);
		paraLen	+=PRA_SIZEOF_COMMON_ID;
	
		// ����,��ϵ,���ͻ���
		memcpy((tmpPara + paraLen),	s_common.carInfo,	PRA_SIZEOF_COMMON_CARINFO);
		paraLen	+=PRA_SIZEOF_COMMON_CARINFO;		
		
		// ����汾��
		for(i=0;	i<5;	i++)
		{
				tmpPara[paraLen + i] =*(u8*)(filename + i);
		}
		paraLen	+=5;
		
		// SIM�����������ַ�ʽ
		if((strlen((const char *)s_gsmApp.simNum) == 0) || (strlen((const char *)s_gsmApp.simNum) >= sizeof(s_gsmApp.simNum)))
		{
				// ���볤��Ϊ0�������󳤶�(20)������Ϊ�ǷǷ�ֵ
				tmpPara[paraLen] =0x00;		// �����ֽ�д0
				paraLen++;
		}
		else
		{
				tmpPara[paraLen] =strlen((const char *)s_gsmApp.simNum);		// �����ֽ�
				paraLen++;
				memcpy((tmpPara + paraLen),	s_gsmApp.simNum,	strlen((const char *)s_gsmApp.simNum));	
				paraLen	+=strlen((const char *)s_gsmApp.simNum);
		}
		
		// ʱ���
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
	
		// ״̬����
		tmpPara[paraLen] = 0x05;
		paraLen++;
		
		// TTS״̬(��4λ����1-������4λ֧��״̬1-��֧��)
		if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		tmpPara[paraLen] &= 0xf0;	// SIM800�ں����� 
		paraLen++;
		
		// GPS״̬(��4λ����-������4λ֧��״̬1-��֧��)
		if(s_cfg.devEn & DEF_DEVEN_BIT_GPS)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		if(GPS_DEVICE.InitSta == 1)
				tmpPara[paraLen] &= 0xf0;		
		else
				tmpPara[paraLen] |= 0x01;
		paraLen++;
		
		// BT״̬(��4λ����ģ����𣬵�4λ֧��״̬1-��֧��)
		if(s_cfg.devEn & DEF_DEVEN_BIT_BT)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		tmpPara[paraLen] &= 0xf0;	// SIM800�ں����� 
		paraLen++;
		
		// ����ģ��״̬(��4λ���أ���4λ֧��״̬1-��֧��)
		if(s_cfg.devEn & DEF_DEVEN_BIT_CON)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		if(CON_DEVICE.InitSta != 0)
				tmpPara[paraLen] &= 0xf0;	// ��������豸
		else
				tmpPara[paraLen] |= 0x01;		
		paraLen++;
		
		// SHAKE״̬(��4λ����״̬����4λ֧��״̬1-��֧��)
		if(s_cfg.devEn & DEF_DEVEN_BIT_SHAKE)
				tmpPara[paraLen] |= 0x10;	
		else
				tmpPara[paraLen] &= 0x0f;
		
		if(MEMS_DEVICE.InitSta != 0)
				tmpPara[paraLen] &= 0xf0;	// ���ٶ�оƬ��Ч
		else
				tmpPara[paraLen] |= 0x01;	
		paraLen++;
			
		tmpSn =UpDataSn();
		*retSn =tmpSn;
		return ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_LOGIN_DEV,	tmpPara,	paraLen,	pOutBuf,	outLen);
}
/*******************************************************************************
* Function Name  : HAL_BulidComPack_Process
* Description    : ��������������˯�߽������ݴ��
* Input          : 
* Output         : 
* Return         : ����0, ���󷵻�0xffff
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
* Description    : OBD����������ˮ��
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
* Description    : DEV����������ˮ��
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
* Description    : ����������ͷ�������ID
* Input          : type:��������DEF_TYPE_ACK~DEF_TYPE_CARSTA
* Output         : None
* Return         : ��ƥ�䷵��0xffff
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
* Description    : �ж���������������Ƿ���ȷ
* Input          : type:��������DEF_TYPE_ACK~DEF_TYPE_CARSTA
* Output         : None
* Return         : ���󷵻� 0xffff����ȷ���� 0
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
* Description : ����GPRS�������ݵ�Э�����.
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
			
		// Э��ճ�ͱ��ļ��//////////////////////
		packs =ZXT_PackCheck_Process (pBuf,	len);		// ����Ƿ���ճ�Ͱ�����
		if(packs == 0)
		{
				s_update.writeLock =0;	// �ͷ�������
				return -1;
		}
		else
		{
				if(packs > 1)
				{
						// ��ӡGPRS���շ�����ճ�Ͱ���Ϣ
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
						s_update.writeLock =0;	// �ͷ�������
						return -1;
				}
				
				// Э�����//////////////////////////////	
				ret =ZXT_PackAnalyze_Process ((pBuf+tmpPosi),	tmpLen,	&pdu);	
				if(ret != DEF_PRO_UNPACK_NONE)
				{
						s_update.writeLock =0;	// �ͷ�������
						return -1;
				}
				
				// �ж�ָ��Ϸ���////////////////////////
				if(pdu.bodycmdId != DEF_CMDID_COMACK_PLA)
				{
						if(pdu.bodySn != proUpSn)
								proUpSn =pdu.bodySn;
						//else
						//		return	-1;
				}			
				
				// Э��ִ��//////////////////////////////
				switch(pdu.bodycmdId)
				{
						case		DEF_CMDID_NAVI_PLA				  :
										ret =naviAckProcess(&pdu);					// �������·�-����IP��ȡӦ��
										break;
						case		DEF_CMDID_COMACK_PLA				:
										if(s_common.iapSta != DEF_UPDATE_KEYWORD)
												ret =comAckProcess(&pdu);				// �������·�-ͨ��Ӧ��	
										else
												ret =upDateRxdProcess(&pdu);		// �������·�-���������е�ͨ��Ӧ��	
										break;
						case		DEF_CMDID_UPDATE_PLA				:
										ret =upDateProcess(&pdu);						// �������·�-�豸���� 
										break;
						case		DEF_CMDID_UPDATEINFO_PLA		:	
										ret =upDateRxdProcess(&pdu);				// �������·�-������Ϣ
										break;
						case		DEF_CMDID_UPDATEDATA_PLA		:
										ret =upDateRxdProcess(&pdu);				// �������·�-�����ļ�
										break;
						case		DEF_CMDID_UPDATEQUE_PLA			:
										ret =upDateRxdProcess(&pdu);				// �������·�-�����ɹ���ѯ
										break;
						case		DEF_CMDID_SETIP_PLA					:
										ret =setIpProcess(&pdu);						// �������·�-����IP��ַ
										break;
						case		DEF_CMDID_SETODO_PLA				:
										ret =setObdComProcess(&pdu);				// �������·�-��ʼ�����(OBD)
										break;
						case		DEF_CMDID_DEVRESET_PLA			:
										s_osTmp.resetReq = DEF_RESET_PRI0;	// �������·�-�豸����
										ret =0;
										break;				
						case		DEF_CMDID_GPSUPTIME_PLA			:
										ret =setInValTimeProcess(&pdu);			// �������·�-�豸GPS�ɼ����												
										break;
						case		DEF_CMDID_MODULESWT_PLA			:
										ret =setDevEnProcess(&pdu);					// �������·�-�����豸ģ�鿪��ָ��
										break;
						case		DEF_CMDID_ENGINEBRAND_PLA		:
										ret =setCarInfoProcess(&pdu);				// �������·�-������Ʒ��
										break;
						case		DEF_CMDID_POWOFFDELAY_PLA		:
										ret =setDelaySleepProcess(&pdu);		// �������·�-��ʱ���ػ�(����˯��)
										break;
						case		DEF_CMDID_SETTIME_PLA				:
										ret =setTimeProcess(&pdu);					// �������·�-ʱ���·�ָ��
										break;
						case		DEF_CMDID_SETPAILIANG_PLA		:
										ret =setObdComProcess(&pdu);				// �������·�-����������(OBD)
										break;						
						case		DEF_CMDID_SETTHRESHOLD_PLA	:
										ret =setDevThreholdProcess(&pdu);		// �������·�-�豸��ֵ����
										break;
						case		DEF_CMDID_QUEPARA_PLA				:
										ret =0;															// �������·�-�豸������ѯ(�첽���ݷ�����������ɴ��)
										break;
						case		DEF_CMDID_SMSCENTERPHO_PLA	:
										ret =smsCenterPhoProcess(&pdu);			// �������·�-���ŷ��������ĺ���
										break;
						case		DEF_CMDID_SIMCARDPHO_PLA	:
										ret =simCardPhoProcess(&pdu);				// �������·�-�豸SIM������
										break;	
						case		DEF_CMDID_SETPHONUM_PLA	:
										ret =setSMSConPhoProcess(&pdu);			// �������·�-���úϷ����ſ��ƺ���
										break;	
						case		DEF_CMDID_QUEPHONUM_PLA			:
										ret =0;															// �������·�-��ѯ�Ϸ����ſ��ƺ���
										break;
						case		DEF_CMDID_SETSLEEPTIME_PLA	:
										ret =setSleepTimeProcess(&pdu);			// �������·�-�豸˯��ʱ�估�������ϴ�ʱ����
										break;					
						case		DEF_CMDID_RTSTART_PLA				:
										ret =setObdComProcess(&pdu);				// �������·�-ר����ϯʵʱ����������(OBD)
										break;
						case		DEF_CMDID_RTSTOP_PLA				:
										ret =setObdComProcess(&pdu);				// �������·�-ר����ϯʵʱ������ֹͣ(OBD)
										break;
						case		DEF_CMDID_STARTTJ_PLA				:
										ret =setObdComProcess(&pdu);				// �������·�-��ʼ�ֶ����(OBD)
										break;
						case		DEF_CMDID_READFUEL_PLA				:
										ret =setObdComProcess(&pdu);				// �������·�-��ѯʣ������
										break;
						case		DEF_CMDID_CONCAR_PLA				:
										ret =setObdComProcess(&pdu);				// �������·�-��������(ͨ��OBD���ⲿ����ģ��)
										break;
						case		DEF_CMDID_CONTTS_PLA				:
										ret =conTTSProcess(&pdu);						// �������·�-TTS����
										break;
						case		DEF_CMDID_CLRCODE_PLA				:
										ret =setObdComProcess(&pdu);				// �������·�-����ָ��(OBD)
										break;
						case		DEF_CMDID_QUECAR_PLA				:
										if(s_common.carInfo[0] == 'O')
												ret =-1;												// Oϵ��ͨ��Ӧ��ֱ�ӷ��ش���
										else
												ret =setObdComProcess(&pdu);		// �������·�-��ѯ����״̬(ͨ��OBD���ⲿ����ģ��)
										break;
						default : break;  
				}
				
				// ͨ��Ӧ����(ͨ��Ӧ�𼰵����ظ���������ͨ��Ӧ��)///////////////////
				if((pdu.bodycmdId != DEF_CMDID_COMACK_PLA) && (pdu.bodycmdId != DEF_CMDID_NAVI_PLA))
				{
						// Ӧ����//////////////////////////////
						if(ret == 0)
								result = DEF_ACKTYPE_OK;
						else if(ret == -2)
								result = DEF_ACKTYPE_BUSY;	
            else
								result = DEF_ACKTYPE_ERR;
						
						// ͨ��Ӧ�����ݷ���ͨ��Ӧ�����ͽṹ�洢,�������������滻֮ǰ������
						// (ͨ��Ӧ�����ʱЧ������յ�ָ�������Ͽ���,����Ӧ��)
						memset((u8 *)&comAck,	0,	sizeof(comAck));
						tmpAck.sum	= 0;		// RAM���в�У�����ݺϷ���
						tmpAck.path = DEF_PATH_GPRS;
						tmpAck.type = DEF_TYPE_ACK;
						tmpAck.len  = 3;
						tmpAck.data[0] = (u8)(pdu.bodySn >> 8);	
						tmpAck.data[1] = (u8)(pdu.bodySn);	
						tmpAck.data[2] = result;	
						memcpy((u8 *)&comAck,	(u8 *)&tmpAck,	sizeof(tmpAck));
				}
				// �豸������ѯ�첽���
				if(pdu.bodycmdId == DEF_CMDID_QUEPARA_PLA)
				{
						ret =queDevParaProcess(&pdu);	
				}	
				// �豸���ſ��ƺ����ѯ�첽���
				if(pdu.bodycmdId == DEF_CMDID_QUEPHONUM_PLA)
				{
						ret =queSMSConPhoProcess(&pdu);	
				}
//				// ����SIM���������Ҫ��λ���紥���������ֱ���
//				if(pdu.bodycmdId == DEF_CMDID_SIMCARDPHO_PLA)
//				BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
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
* Description    : �������ͨ���㷨����
* Input          : SendData_TypeDef * ��������ӽṹ��ָ��
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	dataInQueProcess	(SendData_TypeDef	*sendData)
{
		u8	path=0;
		s8	ret =0;
	
		// �����뷢�Ͷ���///////////////////////
		if(HAL_ComQue_size (DEF_FLASH_QUE) != 0)
		{
				// FLASH��������ֱ���뵽FLASH������
				ret =HAL_ComQue_in (DEF_FLASH_QUE,	sendData);	
				path =1;
		}
		else
		{
				// 2015-9-28���Ϊ���Ϩ����ֱ�Ӵ�FLASH
				if((sendData->type == DEF_TYPE_IG) || (1u == HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE)))
				{
						ret =HAL_ComQue_in (DEF_FLASH_QUE,	sendData);
						path =1;
				}
				else
				{					
						// ����豸����RAM����Ϊ��ֱ�ӷŵ�RAM���з���ֱ����FLASH����
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
		// ��Ӵ�����
		if((ret != DEF_PROQUE_QUE_OK) && (path == 1))
		{
				if(APP_ErrQue (&s_err.spiFlashSumTimes) >= 33)
				{						
						// ֻ�������Ĵ����ۼƴﵽ33���ϲ�ǿ�Ƹ�λ����
						ret =0;
						ret =HAL_ComQue_ForceReset(DEF_FLASH_QUE);
						if(ret == DEF_PROQUE_QUE_OK)
						{
								APP_ErrClr (&s_err.spiFlashSumTimes);			// ֻ����ȷ�ĳ�ʼ����Ÿ�λ�����������һֱ����
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
						ret =HAL_ComQue_in (DEF_RAM_QUE,	sendData);		// 2015-1-6 by:gaofei FLASH���ݴ洢ʧ���ٷŵ���ʱ���Ͷ�����(������GPS����)
		}
		return ret;
}


/*******************************************************************************
* Function Name  : naviAckProcess
* Description    : �������·���IP��ȡӦ��
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
		
		tmpLen1 =GetProParameter(1,	pdu->paraData+1,	pdu->paraData[0],	tmpIp);		// ��������1���������ֽ�
		tmpLen2 =GetProParameter(2,	pdu->paraData+1,	pdu->paraData[0],	tmpPort);
		tmpLen3 =GetProParameter(3,	pdu->paraData+1,	pdu->paraData[0],	tmpAPN);
		tmpLen4 =GetProParameter(4,	pdu->paraData+1,	pdu->paraData[0],	tmpUser);
		tmpLen5 =GetProParameter(5,	pdu->paraData+1,	pdu->paraData[0],	tmpPass);

//		if(s_cfg.testapp2)//WJH_TEST�����˲���
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
		
		// ����Ӧ������
		s_ack.msgId		=pdu->bodycmdId;	
		s_ack.ackSn 	=pdu->bodySn;	
		if(ret == 0)
				s_ack.result	=DEF_ACKTYPE_OK;
		else
				s_ack.result	=DEF_ACKTYPE_ERR;
	
		OSMboxPost(OS_ACKMbox, (void *)&s_ack); // ����Ӧ���  
		
		return ret;
}
/*******************************************************************************
* Function Name  : comAckProcess
* Description    : �������·���ͨ��Ӧ����(����Ӧ�����䵽��������)
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	comAckProcess	(Protocol_Typedef *pdu)
{
		if(pdu->paraLen == 0)
				return -1;
		
		memset((u8 *)&s_ack,	0,	sizeof(s_ack));
		s_ack.msgId		=pdu->bodycmdId;	
		s_ack.ackSn 	=(u16)((pdu->paraData[0] << 8) | pdu->paraData[1]);
		s_ack.result	=pdu->paraData[2];			
		OSMboxPost(OS_ACKMbox, (void *)&s_ack); // ����Ӧ���  
		
    return  0;
}
/*******************************************************************************
* Function Name  : upDateProcess
* Description    : �������·����豸��������
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	upDateProcess	(Protocol_Typedef *pdu)
{
		if(*(s_devSta.pFlash)	!=	DEF_FLASHHARD_NONE_ERR)
				return	-1;		// �ⲿFLASH���Ͻ�ֹ��������
		
		//if(s_common.iapSta != 0)
		if((s_common.iapSta == DEF_UPDATE_WAIT) || (s_common.iapSta == DEF_UPDATE_KEYWORD))
				return	-1;		// �����Ѿ��ڴ�����
		
		s_osTmp.upFlag		=1;	// ���������ȴ�
		s_common.iapSta  	=DEF_UPDATE_WAIT;	// ���������ȴ��м�״̬,Ϊ��ͬ����ʱ����������
		OSTmr_Count_Start(&s_osTmp.upWaitCounter);
		
		s_update.dataLen 	=0;
		memset(s_update.dataBuf,	0,	DEF_UPDATEBUF_SIZE);
		
		return 0;
}
/*******************************************************************************
* Function Name  : upDateRxdProcess
* Description    : ���������з������·�����Ϣ����
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	upDateRxdProcess	(Protocol_Typedef *pdu)
{
		// ����������δ�������ݸ�����Ҫ�ж�дʹ����
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
		OSMboxPost(OS_DownACKMbox, (void *)&s_update); // ����Ӧ���  
		return 0;
}
/*******************************************************************************
* Function Name  : setIpProcess
* Description    : �������·�����IP��ַ����
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
		
		tmpLen1 =GetProParameter(1,	pdu->paraData+1,	pdu->paraData[0],	tmpIp);		// ��������1���������ֽ�
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
		
		// �����Ϸ����ж�
    HAL_SysParaReload();
		strcpy((char *)s_cfg.ip1,				(const char *)tmpIp);
		strcpy((char *)s_cfg.port1,			(const char *)tmpPort);
		strcpy((char *)s_cfg.apn,				(const char *)tmpAPN);
		strcpy((char *)s_cfg.apnUser,		(const char *)tmpUser);
		strcpy((char *)s_cfg.apnPass,		(const char *)tmpPass);
		if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
		{
				s_osTmp.resetReq = DEF_RESET_PRI0;	// ���ø�λ��־
				return 0;
		}
		else
				return -1;
}
/*******************************************************************************
* Function Name  : setInValTimeProcess
* Description    : �������·����òɼ�ʱ�䴦��
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
* Description    : �������·�����ģ�鿪��ָ���
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	setDevEnProcess	(Protocol_Typedef *pdu)
{
		if(pdu->paraLen != 2)
				return -1;	

    HAL_SysParaReload();
		if(pdu->paraData[1] == 0x01)
		{
				// ģ�鹦�ܿ�
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
				// ģ�鹦�ܹ�
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
* Description    : �������·����÷�����Ʒ��
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
* Description    : �������·�������ʱ˯�ߴ���
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
* Description    : �������·�����У׼ʱ�䴦��
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
		BSP_RTC_Set_Current(&cutTime);	// ����ʱ��
		return 0;
}
/*******************************************************************************
* Function Name  : setDevThreholdProcess
* Description    : �������·������豸����ֵ(�������Ϊ0xFF���ʾ���Ըò�����������)
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	setDevThreholdProcess	(Protocol_Typedef *pdu)
{
		u8 tmpU8=0;
		u16	p=0,tmpU16=0;
		if(pdu->paraLen < 13)	// ���в���һ��13byte
				return -1;	
				
		// ���ٶ���ײ��������
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
		
		// ��ѹ������������
		tmpU16 =(u16)((pdu->paraData[p] << 8) | pdu->paraData[p+1]);
		if(tmpU16 != 0xffff)
				s_cfg.lowPower	=tmpU16;
		p	+=2;
		
		// �𶯻�������
		tmpU16 =(u16)((pdu->paraData[p] << 8) | pdu->paraData[p+1]);
		if(tmpU16 != 0xffff)
				s_cfg.accMT	=tmpU16;
		p	+=2;
		
		// �޸ļ��ٶȼĴ�����ֵ
		BSP_ACC_CfgInt (DEF_ENABLE,	(u8)s_cfg.accMT,	(u8)s_cfg.accMD,	5000);
		
		// �豸�ƶ�����ʱ�䷧ֵ
		tmpU8 =(u8)(pdu->paraData[p]);
		if(tmpU8 != 0xff)
				s_cfg.sysMoveT	=tmpU8;
		p	+=1;
		
		// �豸�ƶ������ٶȷ�ֵ
		tmpU8 =(u8)(pdu->paraData[p]);
		if(tmpU8 != 0xff)
				s_cfg.sysMove	 =tmpU8;
		p	+=1;
		
		// �豸��ʱ���ʱ��
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
* Description    : �������·���ѯ�豸����
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
	
		// �ظ�ָ������к�
		tmpPara[paraLen] =(u8)(pdu->bodySn >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(pdu->bodySn);	
		paraLen++;
			
		// ʱ���
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
		
		// ��������
		tmpPara[paraLen] =0x09;	
		paraLen++;	
		
		// ���ٶȱ�����ֵ
		tmpPara[paraLen] =(u8)(s_cfg.dssATH >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.dssATH);	
		paraLen++;
		
		// ���ٶȱ�����ֵ
		tmpPara[paraLen] =(u8)(s_cfg.dssDTH >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.dssDTH);	
		paraLen++;
		
		// ��ײ������ֵ
		tmpPara[paraLen] =(u8)(s_cfg.dssPTH >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.dssPTH);	
		paraLen++;
		
		// ��ѹ������ֵ
		tmpPara[paraLen] =(u8)(s_cfg.lowPower >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.lowPower);	
		paraLen++;
		
		// �𶯱�����ֵ
		tmpPara[paraLen] =(u8)(s_cfg.accMT >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.accMT);	
		paraLen++;
		
		// �豸�ƶ�ʱ�䱨����ֵ
		tmpPara[paraLen] =(u8)(s_cfg.sysMoveT);	
		paraLen++;		
		
		// GPS�ɼ����
		/*
		tmpPara[paraLen] =(u8)((s_cfg.onReportTime & 0x0000ff00) >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.onReportTime & 0x000000ff);	
		paraLen++;
		*/
		tmpPara[paraLen] =(u8)(s_cfg.gpsCollectTime);	
		paraLen++;
		
		// �豸�ƶ��ٶȷ�ֵ
		tmpPara[paraLen] =(u8)(s_cfg.sysMove);	
		paraLen++;
		
		// �����ʱʱ��
		tmpPara[paraLen] =(u8)(s_cfg.tjDelayTime);	
		paraLen++;	

		// �������ϴ�ʱ��
		tmpPara[paraLen] =(u8)(s_cfg.healthReportTime >> 24);	
		paraLen++;	
		tmpPara[paraLen] =(u8)(s_cfg.healthReportTime >> 16);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.healthReportTime >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.healthReportTime);	
		paraLen++;
		
		// �ͺĻ�ȡƵ��
		tmpPara[paraLen] =0x00;	
		paraLen++;	

		// �豸����ʱ��
		tmpPara[paraLen] =(u8)(s_cfg.globSleepTime >> 24);	
		paraLen++;	
		tmpPara[paraLen] =(u8)(s_cfg.globSleepTime >> 16);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.globSleepTime >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(s_cfg.globSleepTime);	
		paraLen++;
	
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_QUEPARA;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : queSMSConPhoProcess
* Description    : �������·���ѯ�豸���ſ��ƺϷ�����(���ظ�ʽlen+,ph1,ph2,ph3,ph4,ph5,ph6)
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	queSMSConPhoProcess	(Protocol_Typedef *pdu)
{
		u8 ret =0;
		u8 tmpPara[150]={0};
		u16	paraLen=0;
		SendData_TypeDef sendData;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
			
		// �ظ�ָ������к�
		tmpPara[paraLen] =(u8)(pdu->bodySn >> 8);	
		paraLen++;
		tmpPara[paraLen] =(u8)(pdu->bodySn);	
		paraLen++;
	
		// �ַ�������
		tmpPara[paraLen] =0;	//	��д0
		paraLen++;
		
		// ���ƺ���
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
		
		tmpPara[2] = paraLen-3;	// ��д����
	
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_QUEPHO;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : smsCenterPhoProcess
* Description    : �������·����ŷ��������ĺ���
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	smsCenterPhoProcess	(Protocol_Typedef *pdu)
{
		s8 ret=0;
		u8 tmpLen=0,	tmpIDBuf[20]={0},	i=0;
		u32	filename = FLASH_CPU_APPNAME_START;
		SendData_TypeDef   smsTmpSend;  // ���ŷ�����ʱ���ݽṹ 			
	
		memset((u8 *)&smsTmpSend,	'\0',	sizeof(smsTmpSend));
		memset((u8 *)&tmpIDBuf,	'\0',	sizeof(tmpIDBuf));
		
		tmpLen =pdu->paraData[0];
		smsTmpSend.path				=	DEF_PATH_SMS;
		smsTmpSend.smsAlType 	= DEF_SMS_COM;  
		smsTmpSend.smsMode 		= DEF_GSM_MODE; 
		memcpy(smsTmpSend.smsNum,	&pdu->paraData[1],	tmpLen);
		
		strcpy((char *)smsTmpSend.data,	(char *)GSM_SMS_NUMCHECK_H);
		
		// ���IDC
		Hex2StringArray (s_common.id,	PRA_SIZEOF_COMMON_ID,	tmpIDBuf);
		tmpIDBuf[PRA_SIZEOF_COMMON_ID*2-1] = '\0';	// ȥ��ID���油���0	
		strcat((char *)smsTmpSend.data,	(char *)tmpIDBuf);	
		
		// ��ӳ�ϵ
		memset((u8 *)&tmpIDBuf,	'\0',	sizeof(tmpIDBuf));
		tmpIDBuf[0] = ',';
		tmpIDBuf[1] = s_common.carInfo[0];	
		tmpIDBuf[2] = ',';
		tmpIDBuf[3] = '\0';
		strcat((char *)smsTmpSend.data,	(char *)tmpIDBuf);
			
		// ����豸����汾��
		memset((u8 *)&tmpIDBuf,	'\0',	sizeof(tmpIDBuf));
		for(i=0;	i<5;	i++)
		{
				tmpIDBuf[i] =*(u8*)(filename + i);
		}
		tmpIDBuf[i] = '\0';
		strcat((char *)smsTmpSend.data,	(char *)tmpIDBuf);
		smsTmpSend.len = strlen((const char *)smsTmpSend.data);
	
		// �����뷢�Ͷ���///////////////////////
		ret =HAL_ComQue_in (DEF_SMS_QUE,	&smsTmpSend);	
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : simCardPhoProcess
* Description    : �������·��豸SIM������
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	simCardPhoProcess	(Protocol_Typedef *pdu)
{		
		if((pdu->paraData[0] >=5) && (pdu->paraData[0] < 20))
		{
				memset(s_gsmApp.simNum,	'\0',	sizeof(s_gsmApp.simNum));
				memcpy(s_gsmApp.simNum,	&pdu->paraData[1],	pdu->paraData[0]);
				
				// ��SIM����д��
				s_gsmApp.simNumWriteFlag = 1;	// ֪ͨGPRS������SIM����д��SIM����
				return 0;
		}
		else
				return -1;
}
/*******************************************************************************
* Function Name  : setSMSConPhoProcess
* Description    : �������·����ö��ſ��ƺϷ�����(��ʽ��len+ph1,ph2,ph3,ph4,ph5,ph6)
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
		
		tmpLen1 =GetProParameter(1,	pdu->paraData+1,	pdu->paraData[0],	tmpPh1);		// ��������1���������ֽ�
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
		
		// �����Ϸ����ж�
    HAL_SysParaReload();
		if(tmpLen1 == 0)
				memset(s_cfg.pho1, 0,	PRA_MAXOF_SYSCFG_PHO1);		// 2015-6-15 �޸Ĳ���Ϊ0ʱ�����ԭ�洢��������			
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
* Description    : �������·������豸˯��ʱ�估�������ϴ�ʱ��
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	setSleepTimeProcess	(Protocol_Typedef *pdu)
{
		if((pdu->paraLen != 5) || ((pdu->paraData[0] != 0) && (pdu->paraData[0] != 1)))
				return -1;	
		
    HAL_SysParaReload();
		if(pdu->paraData[0] == 0)
		{
				// �����豸Ϩ������ʱ��
				s_cfg.globSleepTime = (u32)((pdu->paraData[1] << 24) | (pdu->paraData[2] << 16) | (pdu->paraData[3] << 8) | pdu->paraData[4]);
		}
		else if(pdu->paraData[0] == 1)
		{
				// �����豸�������ϴ�ʱ��
				s_cfg.healthReportTime = (u32)((pdu->paraData[1] << 24) | (pdu->paraData[2] << 16) | (pdu->paraData[3] << 8) | pdu->paraData[4]);
		}
		if(HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)) == DEF_PARA_NONE_ERR)
				return	0;
		else
				return	-1;
}

/*******************************************************************************
* Function Name  : conTTSProcess
* Description    : �������·�������������
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
static	s8	conTTSProcess	(Protocol_Typedef *pdu)
{
		u8	tmpBuf[PRA_MAXOF_SYSCFG_SOUND1]={0};
		u16 p=0,tmpLen=0;
		u32	proCount=0, cutCount=0;
		SYS_DATETIME	cunTime;
	
		if(pdu->paraLen < 1)
				return -1;	
		
		// ����ʱ���ֵ
		proCount =(u32)((pdu->paraData[0] << 24) | (pdu->paraData[1] << 16) | (pdu->paraData[2] << 8) | pdu->paraData[3]);		
		memset((u8 *)&cunTime,	0,	sizeof(cunTime));
		BSP_RTC_Get_Current(&cunTime);
		cutCount	=	Mktime(cunTime.year, cunTime.month, cunTime.day, cunTime.hour, cunTime.minute, cunTime.second);
		
		if(pdu->paraData[4] == 0x00)
		{
				// �̶�����
			
				// ��ȡλ��1����
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
				// ��ȡλ��2����
				if(pdu->paraLen > (p + 2 + tmpLen + 1))
				{	
						p= p+2+tmpLen+1; // ָ��λ��2����
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
						// ��ʱ����(ֻ����1�����ڼ�ʱ����)
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
						return -1;	// 2015-6-15 :���TTS����ֹ���ֹ���ŷ������·�����
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
* Description    : �������·�OBDͨ�ô�����
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1, æ����-2
*******************************************************************************/
static	s8	setObdComProcess	(Protocol_Typedef *pdu)
{	
		u32	tmpOdo=0;
	
		/*
		// 2016-1-18 ȡ�����У׼��������͸������OBD����
		if(pdu->bodycmdId == DEF_CMDID_SETODO_PLA)
		{
				// ��ʼ����̵��������Ժ�鵽OBD���ִ���
				tmpOdo =(u32)((pdu->paraData[0] << 24) | (pdu->paraData[1] << 16) | (pdu->paraData[2] << 8) | (pdu->paraData[3]));
				u8fun_odo_Calibration(tmpOdo);
				return 0;	// ��ʼ�����û��æ״̬,ֱ�ӷ��سɹ�
		}
		else if(pdu->bodycmdId == DEF_CMDID_CONCAR_PLA)
		*/
		if(pdu->bodycmdId == DEF_CMDID_CONCAR_PLA)
		{
				// ��������ĳ����������⴦��
				if(conSms.dataLock == 0)
				{
						memset((u8 *)&simulateSms,	0,	sizeof(simulateSms));
						simulateSms.dataLen = pdu->paraLen-1;
						memcpy(simulateSms.dataBuf,	(pdu->paraData + 1),	(pdu->paraLen-1));
						OSMboxPost(OS_RecSmsMbox, (void *)&simulateSms);		// ����ģ�⳵�����ƶ���	
						return 0;
				}
				else
						return -2;	
		}
		else
		{
				if(pdu->bodycmdId == DEF_CMDID_READFUEL_PLA)
				{
						// ʣ��������ѯ
						if(HAL_SendOBDProProcess(CMD_READ_FUEL,	pdu->bodySn,	0,	(u8 *)"",	0) == 0)			// ֪ͨOBD��ȡ��ǰ����	
								return 0;
						else
								return -2;
				}
				else
				{
						if(HAL_SendOBDProProcess(CMD_PROTOCOL,	pdu->bodySn,	pdu->bodycmdId,	pdu->paraData,	pdu->paraLen) == 0)
						{
								// Э�����⴦��
								if(pdu->bodycmdId == DEF_CMDID_STARTTJ_PLA)
								{
										HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_ENABLE);				// ϵͳ�����״̬��
										HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_ENABLE);	// ���ò��Ż�δ����������״̬
										if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
												HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound1,	strlen((const char *)s_cfg.sound1));		// ������쿪ʼ����		
								}	
								// ����״̬��ȡ
								else if(pdu->bodycmdId == DEF_CMDID_QUECAR_PLA)
								{
										glob_readCarSn			=pdu->bodySn;	// ��ʱ����Ӧ����ˮ��
										glob_readTimeStamp	=(vu32)((pdu->paraData[0] << 24) | (pdu->paraData[1] << 16) | (pdu->paraData[2] << 8) | pdu->paraData[3]);	// ��ʱ����Ӧ��ʱ���												
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
* Description    : ͨ��Ӧ��ʵʱ���ݴ������
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
s8	HAL_PackComAckProcess	(u8 *pPara,	u16 paraLen,	u8 *outBuf,	u16 *outLen)
{
		u16	tmpSn=0;
		tmpSn =UpDataSn();	
		return ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_COMACK_DEV,	pPara,	paraLen,	outBuf,	outLen);
}
/*******************************************************************************
* Function Name  : HAL_PackOBDDataProcess
* Description    : OBDʵʱ���ݴ������
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
* Description    : Ram�������ݴ������
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
	
		// ���ݶ�ȡ
		for(i=0;	(HAL_ComQue_size (DEF_RAM_QUE) > 0) && (i < GPS_RAMDATA_SIZE); i++)
		{
				ret =HAL_ComQue_out(DEF_RAM_QUE,	&tmpSend);
				if(ret == DEF_PROQUE_QUE_OK)
				{
						HAL_ComQue_outend (DEF_RAM_QUE);	// ���ݳ���
						if(((tmpSend.path != DEF_PATH_GPRS) && (tmpSend.path != DEF_PATH_BT)) || (tmpSend.len == 0) ||
   						  (tmpSend.len > SPI_FLASH_PageSize) ||
							  (IsDataType(tmpSend.type) == 0xffff))
						{								
								continue;	// �Ƿ�����,���������ݰ�									
						}
						else
						{
								// ���ݴ��
								tmpSn =UpDataSn();
								*retSn =tmpSn;
								cmdId	=RetCmdId(tmpSend.type);
								if(tmpSend.type == DEF_TYPE_GPS)
								{
										// GPS���ݱȽ�����(������һ�ֽ���Ҫ��д֡��)
										memset(tmpPara,	0,	sizeof(tmpPara));
										tmpPara[0] = 0x01;	// ֡��Ϊ1֡(�Ȳ�������GPS��֡ʵʱ�ϴ����)
										memcpy((tmpPara + 1),	tmpSend.data,	tmpSend.len);
										memset(tmpSend.data,	0,	DEF_PACK_BUF_SIZE);	
										tmpSend.len +=1;
										memcpy(tmpSend.data,	tmpPara,	tmpSend.len);								
								}
								else if(tmpSend.type == DEF_TYPE_ZITAI)
								{
										// ��̬���ݱȽ�����(������һ�ֽ���Ҫ��д֡��)
										memset(tmpPara,	0,	sizeof(tmpPara));
										tmpPara[0] = 0x01;	// ֡��Ϊ1֡(�Ȳ���������̬��֡ʵʱ�ϴ����)
										memcpy((tmpPara + 1),	tmpSend.data,	tmpSend.len);
										memset(tmpSend.data,	0,	DEF_PACK_BUF_SIZE);	
										tmpSend.len +=1;
										memcpy(tmpSend.data,	tmpPara,	tmpSend.len);								
								}
								else if(tmpSend.type == DEF_TYPE_IG)
								{
										if(tmpSend.data[0] == TYPE_IG_ON)
										{
												// ��дʱ��ƫ��
												count =OSTmr_Count_Get(&s_osTmp.igStartTicks);
												tmpSend.data[1] =(u8)(count >> 24);	
												tmpSend.data[2] =(u8)(count >> 16);
												tmpSend.data[3] =(u8)(count >> 8);
												tmpSend.data[4] =(u8)(count);		
												
												// ͬ������������buf
												igOnSaveBuf[0]	=(u8)(count >> 24);		// 2015-8-19 Ϩ���ĸ�ʽ�޸�
												igOnSaveBuf[1]	=(u8)(count >> 16);
												igOnSaveBuf[2]	=(u8)(count >> 8);
												igOnSaveBuf[3]	=(u8)(count);	
										}
								}
								ret =ZXT_PackBuild_Process (tmpSn,	cmdId,	tmpSend.data,	tmpSend.len,	outBuf,	outLen);	
								*dataType =tmpSend.type;		// ������������
								break;
						}
				}
				else if(ret == DEF_PROQUE_QUE_EMPTY)
				{
						break;	// �����ѿ�
				}
				else if(ret == DEF_PROQUE_QUE_ERR)
				{
						HAL_ComQue_outend (DEF_RAM_QUE);	// ���ݳ��ӳ���,���������ݰ�
						continue;	// ���ݳ��ӳ���,���������ݰ�
				}			
		}
		return ret;

}
/*******************************************************************************
* Function Name  : HAL_PackFlashDataProcess
* Description    : FLASH�������ݴ������
* Input          : maxLen: ����������󳤶�(���ܳ���FLASH����������ʱ���ͽṹ����BUF���弴DEF_FLASHSENDBUF_SIZE��С)
* Output         : 
* Return         : ��ȷ����ʵ�ʳ��Ӹ���
*******************************************************************************/
s8	HAL_PackFlashDataProcess	(u16	maxLen,	u8 *outBuf,	u16 *outLen, u16 *retSn,	u16 *lastOut,	u16 *outNum)
{
		SendData_TypeDef	tmpSend;
		u8	tmpPara[DEF_FLASHSENDBUF_SIZE]={0};
		u8	frameNum=0;	// ʵ����Ч����֡
		s8	ret=0;
		u16 i=0,	tmpSn=0,	paraLen=1,	queOutNum=0;	// ������һ�ֽڵ���֡��
		u32	count=0;
		CPU_SR   cpu_sr;
		
		// ���ݺϷ����ж�
		if(maxLen > DEF_FLASHSENDBUF_SIZE)
				return 0;
		
		OS_ENTER_CRITICAL();
		*lastOut 							=s_queFlash.out;			// �������֮ǰ��outָ��
		s_queFlash.outExt			=s_queFlash.out;			// ��ȡ��ʼλ��
		s_queFlash.entriesExt	=s_queFlash.entries;	
		OS_EXIT_CRITICAL();
	
		// ������ȡ
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
								HAL_ComQue_outend (DEF_FLASH_QUE);	// ���ݳ���
								queOutNum++;	// ��¼������
								continue;	// �Ƿ�����,���������ݰ�					
						}
						else
						{
								if((maxLen - 10 - paraLen) >= (tmpSend.len + 2))	// �ж����³����Ƿ񹻼�����һ��(Э���������Э���ܳ���10byte)
								{							
										tmpPara[paraLen]	= tmpSend.type;	
										paraLen++;
										tmpPara[paraLen]	= tmpSend.len;
										paraLen++;
										
										// ���ʱ��ƫ��������
										if(tmpSend.type == DEF_TYPE_IG)
										{
												if(tmpSend.data[0] == TYPE_IG_ON)
												{
														// ��дʱ��ƫ��
														count =OSTmr_Count_Get(&s_osTmp.igStartTicks);
														tmpSend.data[1] =(u8)(count >> 24);	
														tmpSend.data[2] =(u8)(count >> 16);
														tmpSend.data[3] =(u8)(count >> 8);
														tmpSend.data[4] =(u8)(count);	
													
														// ͬ������������buf
														igOnSaveBuf[0]	=(u8)(count >> 24);		// 2015-8-19 Ϩ���ĸ�ʽ�޸�
														igOnSaveBuf[1]	=(u8)(count >> 16);
														igOnSaveBuf[2]	=(u8)(count >> 8);
														igOnSaveBuf[3]	=(u8)(count);	
												}
										}
										memcpy((tmpPara + paraLen),	tmpSend.data,	tmpSend.len);
										paraLen +=tmpSend.len;						
										frameNum +=1;		// ����һ֡����(�Ȳ�����һ���洢��Ԫ�д洢��֡���)										
										queOutNum++;	// ��¼������
										HAL_ComQue_outend (DEF_FLASH_QUE);	// ���ݳ���
								}
								else
										break;	// �ٴ�����ݽ����ܳ�����Χ
						}
				}
				else if(ret == DEF_PROQUE_QUE_EMPTY)
						break;	// �����ѿ�
				else if(ret == DEF_PROQUE_QUE_ERR)
				{
						queOutNum++;	// ��¼������
						HAL_ComQue_outend (DEF_FLASH_QUE);	// ���ݳ���
						continue;	// ���ݳ��ӳ���,���������ݰ�
				}
				else
						break;		// �Ƿ��ķ���ֵ����,�˳����ݳ���				
		}
		
		// ���ݴ��
		ret =0;
		if(frameNum != 0)
		{
				// �������
				tmpSn =UpDataSn();
				*retSn =tmpSn;
				tmpPara[0] = frameNum;	// ����֡����
				ret =ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_FLASH_DEV,	tmpPara,	paraLen,	outBuf,	outLen);							
		}
		*outNum =queOutNum;
		return ret;
}
/*******************************************************************************
* Function Name  : HAL_GPSInQueueProcess
* Description    : GPS������Ӵ���
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
s8	HAL_GPSInQueueProcess	(void)
{
		u8 ret =0;
		SendData_TypeDef sendData;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
	
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_GPS;
		sendData.len  = sizeof(GPSProData_Def);
		HAL_GpsProDataOut(sendData.data,	&s_gpsPro);	// STM32Ĭ��ΪС��ģʽ,���ݰ����ģʽ���
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_ZiTaiInQueueProcess
* Description    : ��̬������Ӵ���
* Input          : type ��ZITAI_TYPEȷ��
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
s8	HAL_ZiTaiInQueueProcess	(ZITAI_TYPE type,	GPSProData_Def *pGps,	s16 X_Axis, s16 Y_Axis, s16 Z_Axis, s16 XYZ_Axis)
{
		u8 ret =0;
		u8 tmpPara[60]={0};
		u16	paraLen=0;
		SendData_TypeDef sendData;
	
		memset((u8 *)&sendData,	0,	sizeof(sendData));
		memset(tmpPara,	0,	sizeof(tmpPara));
	
		// ��������ֵ
		// ��̬֡����ʱ���ʹ��ʱ���
		
		// ���GPS��Ϣ	
		if(pGps != (void *)0)
		{
				HAL_GpsProDataOut((tmpPara + paraLen),	pGps);	// STM32Ĭ��ΪС��ģʽ,���ݰ����ģʽ���
				paraLen +=sizeof(GPSProData_Def);
		}
		else
				paraLen +=sizeof(GPSProData_Def);		// ��0���
		
		// �������
		tmpPara[paraLen] =type;	
		paraLen++;
		
		// ��Ӽ��ٶ���Ϣ
		tmpPara[paraLen] =(u8)((u16)X_Axis >> 8);	// ���X����ٶ�
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)X_Axis);
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)Y_Axis >> 8);	// ���Y����ٶ�
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)Y_Axis);
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)Z_Axis >> 8);	// ���Z����ٶ�
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)Z_Axis);
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00 >> 8);	// ���X����ٶ�
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00);
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00 >> 8);	// ���Y����ٶ�
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00);
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00 >> 8);	// ���Z����ٶ�
		paraLen++;
		tmpPara[paraLen] =(u8)(0x00);
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)XYZ_Axis >> 8);	// ��Ӻϳɼ��ٶ�
		paraLen++;
		tmpPara[paraLen] =(u8)((u16)XYZ_Axis);
		paraLen++;
	
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_ZITAI;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_24RptInQueueProcess
* Description    : 24Сʱ����������Ӵ���
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
	
		// ��������ֵ
		tmpPara[paraLen] =6;	// 2015-8-20 ����������1(������ʱ����ֶ�)
		paraLen++;
		tmpPara[paraLen] =(u8)(s_adInfo.VBatVol >> 8);	// ȡ��ѹֵ
		paraLen++;
		tmpPara[paraLen] =(u8)(s_adInfo.VBatVol);
		paraLen++;
		tmpPara[paraLen] =s_gsmApp.sLeve;	// GSM�ź�ǿ��
		paraLen++;
		tmpPara[paraLen] =s_gpsPro.noSV;	// ��������
		paraLen++;
		tmpPara[paraLen] =s_err.gsmResetTimes;	// 24Сʱ��GSMģ����ϴ���
		paraLen++;
		tmpPara[paraLen] =s_err.gpsResetTimes;	// 24Сʱ��GPSģ����ϴ���
		paraLen++;
		
		// ʱ��� 2015-8-19 ������ʱ����ֶ�
		BSP_RTC_Get_Current(&tmpRtc);	
		count =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);	
		if((tmpRtc.year < 2014) || (tmpRtc.year >= 2050))
				count =0;	// RTCʱ����Чֱ����0						
		tmpPara[paraLen] =(u8)(count >> 24);	
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 16);
		paraLen++;
		tmpPara[paraLen] =(u8)(count >> 8);
		paraLen++;
		tmpPara[paraLen] =(u8)(count);
		paraLen++;
	
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_24REPORT;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;

}
/*******************************************************************************
* Function Name  : HAL_IGInQueueProcess
* Description    : ���Ϩ��������Ӵ���
* Input          : type ��IG_TYPEȷ��
*                : offSetSec : ʱ��ƫ������(ֻ��Ϩ���¼�����Ч����ΪϨ��״̬�ж�ͨ�������ӳ�������Ҫ������ƫ����)
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
	
		// ��������ֵ
		tmpPara[paraLen] =type;	// ���Ϩ������
		paraLen++;
		
		if(type == TYPE_IG_ON)
		{
				// ����ʱ��ƫ�����ֶ�		
				tmpPara[paraLen] =0;	// ���ʱ��ƫ����ʱд��0�����ʱ�ڸ�дʵ��ƫ����
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				OSTmr_Count_Start(&s_osTmp.igStartTicks);	// ��¼��ʼtickֵ		
			
				// ���
				tmpPara[paraLen] =0;	
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
			
				// ���ʱ��
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;			

				// �ۼ��ͺ�
				tmpPara[paraLen] =0;	
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
			
				// RTCʱ��
				BSP_RTC_Get_Current(&tmpRtc);	
				if((tmpRtc.year < 2014) || (tmpRtc.year >= 2050))
						rtcCount =0;	// RTCʱ����Чֱ����0		
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
				
				// ���������
				memset((u8 *)igOnSaveBuf,	0,	sizeof(igOnSaveBuf));	
				memcpy((u8 *)igOnSaveBuf,	(tmpPara + 1),	sizeof(igOnSaveBuf));	// ֻ���������������18Byte
				
				// 2015-9-25 Ϩ���ʹ����칫������ֵ
				glob_odoBuf[0] =0;
				glob_odoBuf[1] =0;
				glob_odoBuf[2] =0;
		}	
		else
		{
				memcpy((tmpPara + paraLen),	(u8 *)igOnSaveBuf,	sizeof(igOnSaveBuf));
				paraLen +=sizeof(igOnSaveBuf);
			
				// Ϩ���ȡʱ��ƫ��(ֻ������Ϩ�����ݲ���ƫ�ƴ���)
				if(OSTmr_Count_Get(&s_osTmp.igStartTicks) < (offSetSec * 1000))
						offSetCount =0;
				else
						offSetCount =OSTmr_Count_Get(&s_osTmp.igStartTicks) - (offSetSec * 1000);	// ��ȡʱ��ƫ��
			
				tmpPara[paraLen] =(u8)(offSetCount >> 24);	
				paraLen++;
				tmpPara[paraLen] =(u8)(offSetCount >> 16);
				paraLen++;
				tmpPara[paraLen] =(u8)(offSetCount >> 8);
				paraLen++;
				tmpPara[paraLen] =(u8)(offSetCount);
				paraLen++;
				
				// ���
				tmpPara[paraLen] =0;	
				paraLen++;
				tmpPara[paraLen] =glob_odoBuf[0];		// 2015-9-25 Ϩ���ʹ����칫������ֵ
				paraLen++;
				tmpPara[paraLen] =glob_odoBuf[1];
				paraLen++;
				tmpPara[paraLen] =glob_odoBuf[2];
				paraLen++;
				
				// ���ʱ��
        if (1u == HAL_GetIgonAccJudgeIgoffFlag())
        {
            tmpPara[paraLen] = 0xAAu;
            paraLen++;
            tmpPara[paraLen] = 0xAAu;
            paraLen++;
        }
        else
        {
            tmpPara[paraLen] =(u8)(s_osTmp.maxSpeed >> 8);	// ֻ��Ϩ����ϴ����ʱ��
            paraLen++;
            tmpPara[paraLen] =(u8)(s_osTmp.maxSpeed);
            paraLen++;
            s_osTmp.maxSpeed =0;	// ���Ϩ���ĺ����ʱ����0���²ɼ�
        }
				
				// �ۼ��ͺ�
				tmpPara[paraLen] =0;	
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				tmpPara[paraLen] =0;
				paraLen++;
				
				// RTCʱ��
				BSP_RTC_Get_Current(&tmpRtc);	
				if((tmpRtc.year < 2014) || (tmpRtc.year >= 2050))
						rtcCount =0;	// RTCʱ����Чֱ����0			
				else
				{
						// ʱ��ƫ��������
						rtcCount =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);
						if(rtcCount < offSetSec)
								rtcCount =0;
						else
								rtcCount =rtcCount - offSetSec;	// ����ʱ��ƫ����			
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
	
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_IG;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
		{
				if(type == TYPE_IG_ON)
						BSP_BACKUP_SaveIGOnSta();		// 2015-8-19 �洢���״̬
				else
						BSP_BACKUP_ClrIGOnSta();		// 2015-8-19 ������״̬
						
				return 0;
		}
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_AlarmInQueueProcess
* Description    : ����������Ӵ���
* Input          : alarmBit ��ALARM_BIT_TYPEȷ��(֧����չ���豨����alarmBit>0xffΪ��չ��������)
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
		
		// �����ֽ�
		tmpPara[paraLen] = 0x00;	// ���ֵ
		paraLen++;
		
		// ��������
		if(alarmBit < 0xff)
		{
				tmpPara[paraLen] =(u8)alarmBit;	// ͨ�ñ���״̬λ
				paraLen++;	
		}
		else
		{		
				tmpPara[paraLen] =0x00;
				paraLen++;
				tmpPara[paraLen] =(u8)(alarmBit>>8);	// ��չ����״̬λ
				paraLen++;
        tmpPara[paraLen] =(u8)(alarmBit>>16u);	// ��չ����״̬λ
				paraLen++;
		}
		
		// ���ʱ���
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
		
		// ���GPS��Ϣ
		HAL_GpsProDataOut((tmpPara+paraLen),	&s_gpsPro);	// STM32Ĭ��ΪС��ģʽ,���ݰ����ģʽ���
		paraLen +=sizeof(GPSProData_Def);
		
		// ���ȷ����������
		tmpPara[0] = paraLen-1;	// ȥ�������ֽڱ���
	
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_ALARM;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_LogInQueueProcess
* Description    : ����log������Ӵ���
* Input          : type ��LOG_TYPEȷ��
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
	
		// ��������ֵ
		// ��������
		tmpPara[paraLen] =type;	
		paraLen++;
	
		if(type == TYPE_LOG_FLASH)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =1;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =s_err.flashTimes;	
				paraLen++;		
				APP_ErrClr(&s_err.flashTimes);	// ����ϱ���
		}
		else if(type == TYPE_LOG_SIM)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =1;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =s_err.gsmSimTimes;	
				paraLen++;
				APP_ErrClr(&s_err.gsmSimTimes);	// ����ϱ���
		}
		else if(type == TYPE_LOG_GSM)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =0;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =s_err.gsmResetTimes;	
				paraLen++;
				APP_ErrClr(&s_err.gsmResetTimes);	// ����ϱ���
		}
		else if(type == TYPE_LOG_GSMNET)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =0;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =0;	
				paraLen++;
		}
		else if(type == TYPE_LOG_GPS)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =1;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =s_err.gpsResetTimes;	
				paraLen++;
				APP_ErrClr(&s_err.gpsResetTimes);	// ����ϱ���
		}
		else if(type == TYPE_LOG_OBDBUS)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =1;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =s_err.obdBusRWTimes;	
				paraLen++;
				APP_ErrClr(&s_err.obdBusRWTimes);	// ����ϱ���
		}
		else if(type == TYPE_LOG_OBDFLAG)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =1;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =s_err.obdParaRWTimes;	
				paraLen++;
				APP_ErrClr(&s_err.obdParaRWTimes);	// ����ϱ���
		}
		else if(type == TYPE_LOG_ACC)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =0;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =s_err.accTimes;	
				paraLen++;
				APP_ErrClr(&s_err.accTimes);	// ����ϱ���
		}
		else if(type == TYPE_LOG_CPUFLASH)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =1;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =s_err.cpuFlashTimes;	
				paraLen++;
				APP_ErrClr(&s_err.cpuFlashTimes);	// ����ϱ���
		}
		else if(type == TYPE_LOG_CON)
		{
				// �Ƿ��Ͷ���
				tmpPara[paraLen] =1;	
				paraLen++;
				// ���ϴ���
				tmpPara[paraLen] =s_err.conTimes;	
				paraLen++;
				APP_ErrClr(&s_err.conTimes);	// ����ϱ���
		}
		else
				return -1;	// �����Ƿ� 
		
		// ���ʱ���
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
	
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_LOG;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// �����뷢�Ͷ���///////////////////////
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
	
		// ��������ֵ
		// ��������
		tmpPara[paraLen] =0xFFu;	
		paraLen++;
	
    // �Ƿ��Ͷ���
    tmpPara[paraLen] = 0x00u;	
    paraLen++;
    // ���ϴ���
    tmpPara[paraLen] = ucCfg;	
    paraLen++;		
 
		
		// ���ʱ���
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
	
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_LOG;
		sendData.len  = paraLen;
		memcpy(sendData.data,	tmpPara,	paraLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;
}
/*******************************************************************************
* Function Name  : HAL_DssInQueueProcess
* Description    : DSS�¼�������Ӵ���(Ŀǰֻ����������������ϴ�����)
* Input          : 
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
		
		// ��������ֵ
		for(i =0;	(HAL_GpsGetDssData(&gpsPro) != DEF_PROQUE_QUE_EMPTY) && (i<DSS_POINT_NUM);	i++)
		{					
				sendData.sum	= 0;
				sendData.path = DEF_PATH_GPRS;
				sendData.type = DEF_TYPE_GPS;
				sendData.len  = sizeof(GPSProData_Def);
				HAL_GpsProDataOut(sendData.data,	&gpsPro);	// STM32Ĭ��ΪС��ģʽ,���ݰ����ģʽ���
				
				// �����뷢�Ͷ���///////////////////////
				ret =HAL_ComQue_in (DEF_FLASH_QUE,	&sendData);	 // ����DSSΪ���GPS����ֱ�ӷŵ�RAM�д洢���£�����ֱ����FLASH����

				if(ret == DEF_PROQUE_QUE_OK)
						memset((u8 *)&gpsPro,	0,	sizeof(gpsPro));	// ����ʱ������
				else
						break;		
		}
		return 0;

}
/*******************************************************************************
* Function Name  : HAL_SMSConRetInQueueProcess
* Description    : ���ſ���״̬�ش�GPRSЭ������Ӵ���
* Input          :
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
				
		// �绰���볤��
		tmpParaBuf[tmpParaLen] = conSms->numLen;
		tmpParaLen++;
		
		// �绰����
		memcpy((tmpParaBuf + tmpParaLen),	conSms->numBuf ,	conSms->numLen);
		tmpParaLen +=conSms->numLen; 
		
		// �������ݳ���
		tmpParaBuf[tmpParaLen] = conSms->dataLen;
		tmpParaLen++;
		
		// ��������
		memcpy((tmpParaBuf + tmpParaLen),	conSms->dataBuf,	conSms->dataLen);
		tmpParaLen +=conSms->dataLen; 
		
		// ����ʱ���
		tmpParaBuf[tmpParaLen] = (u8)(conSms->smsTimStamp >> 24);	
		tmpParaLen++;
		tmpParaBuf[tmpParaLen] = (u8)(conSms->smsTimStamp >> 16);	
		tmpParaLen++;
		tmpParaBuf[tmpParaLen] = (u8)(conSms->smsTimStamp >> 8);	
		tmpParaLen++;
		tmpParaBuf[tmpParaLen] = (u8)(conSms->smsTimStamp);	
		tmpParaLen++;
		
		// ���ƽ��
		tmpParaBuf[tmpParaLen] =conSms->resultCode;
		tmpParaLen++;
			
		// ʱ���
		BSP_RTC_Get_Current(&tmpRtc);	
		count =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);	
		if((tmpRtc.year >= 2014) && (tmpRtc.year <= 2030))
		{
				// RTCʱ����Ч			
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
		
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_SMSCOM;	// ����Ϣ������������
		sendData.len  = tmpParaLen;
		memcpy(sendData.data,	tmpParaBuf,	tmpParaLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;

}
/*******************************************************************************
* Function Name  : HAL_ReadCarStaInQueueProcess
* Description    : ��ȡ����״̬���ض����������
* Input          :
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
		
		// Ӧ����ˮ��
		tmpParaBuf[tmpParaLen] = (u8)(ackSn >> 8);
		tmpParaLen++;
		
		tmpParaBuf[tmpParaLen] = (u8)(ackSn);
		tmpParaLen++;
		
		// Ӧ������ʱ���
		tmpParaBuf[tmpParaLen] = (u8)(reqTimeStmp >> 24);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(reqTimeStmp >> 16);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(reqTimeStmp >> 8);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(reqTimeStmp);
		tmpParaLen++;	
		
		// ��Ӳ��������
		tmpNum =HAL_UpdataCarState(tmpBuf,	&tmpLen);
		tmpParaBuf[tmpParaLen] = tmpNum;
		tmpParaLen++;
		
		// ��ȡ�¶೵��״̬��Ϣ
		memcpy((tmpParaBuf + tmpParaLen),	tmpBuf ,	tmpLen);
		tmpParaLen +=tmpLen; 
		
		
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_CARSTA;	// ����״̬��������
		sendData.len  = tmpParaLen;
		memcpy(sendData.data,	tmpParaBuf,	tmpParaLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;

}
/*******************************************************************************
* Function Name  : HAL_PowOnInQueueProcess
* Description    : �豸�ϵ籨��
* Input          :
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
*******************************************************************************/
s8	HAL_PowOnInQueueProcess	(u8 staByte,	u32 offSetTick,	u32 powOnRtc)
{
	  s8	ret=0;
		u8	tmpParaBuf[20]={0};
		u16 tmpParaLen=0;
		SendData_TypeDef sendData;
			
		memset(tmpParaBuf,	0,	sizeof(tmpParaBuf));
		memset((u8 *)&sendData,	0,	sizeof(sendData));	
		
		// ״̬�ֽ�
		tmpParaBuf[tmpParaLen] = staByte;
		tmpParaLen++;
		
		// ƫ����
		tmpParaBuf[tmpParaLen] = (u8)(offSetTick >> 24);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(offSetTick >> 16);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(offSetTick >> 8);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(offSetTick);
		tmpParaLen++;	
		
		// ʵ���ϵ�ʱ���
		tmpParaBuf[tmpParaLen] = (u8)(powOnRtc >> 24);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(powOnRtc >> 16);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(powOnRtc >> 8);
		tmpParaLen++;	
		tmpParaBuf[tmpParaLen] = (u8)(powOnRtc);
		tmpParaLen++;	
		
		// ���нṹ��ֵ 
		sendData.sum	= 0;
		sendData.path = DEF_PATH_GPRS;
		sendData.type = DEF_TYPE_POWON;	// �豸�ϵ籨��
		sendData.len  = tmpParaLen;
		memcpy(sendData.data,	tmpParaBuf,	tmpParaLen);
		
		// �����뷢�Ͷ���///////////////////////
		ret =dataInQueProcess(&sendData);
		if(ret == DEF_PROQUE_QUE_OK)
				return 0;
		else
				return -1;

}
/*******************************************************************************
* Function Name  : HAL_SMSInQueueProcess
* Description    : ���ŷ�����Ӵ���
* Input          : smsMode: DEF_GSM_MODE or DEF_UCS2_MODE
*								 : smsType: DEF_SMS_COM(ASCII�����) or DEF_SMS_ALARM(�������Ķ���)
* Output         : 
* Return         : ��ȷ����0, ���󷵻�-1
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
* Description    : ��ʵ�����賵��״̬��Э��״̬��ʽ���
* Input          : outBuf:���bufָ�룬*len:������ݳ���
* Output         : 
* Return         : ��ӵĲ��������
*******************************************************************************/
u8	HAL_UpdataCarState	(u8 *outBuf,	u16	*len)
{
		u16	tmpLen=0;
		STU_CAR_ST	tmpState;
	
		tmpState =HAL_CarStGet();	// ��ȡ��ǰ����״̬

		// ���/���״̬
		*(outBuf + tmpLen)	=0xA3;
		tmpLen++;
		*(outBuf + tmpLen)	=0x01;
		tmpLen++;		
		if(tmpState.uiGuardSt == 0)
				*(outBuf + tmpLen)	=0x00;
		else if(tmpState.uiGuardSt == 1)	// ���
				*(outBuf + tmpLen)	=0x03;
		else if(tmpState.uiGuardSt == 2)	// ����
				*(outBuf + tmpLen)	=0x01;
		else
				*(outBuf + tmpLen)	=0x00;
		tmpLen++;
		
		// ��������/��״̬
		*(outBuf + tmpLen)	=0xA1;
		tmpLen++;
		*(outBuf + tmpLen)	=0x01;
		tmpLen++;		
		if(tmpState.uiEngienSt == 0)
				*(outBuf + tmpLen)	=0x00;
		else if(tmpState.uiEngienSt == 1)	// ����
				*(outBuf + tmpLen)	=0x03;
		else if(tmpState.uiEngienSt == 2)	// Ϩ��
				*(outBuf + tmpLen)	=0x01;
		else if(tmpState.uiEngienSt == 3)	// ON
				*(outBuf + tmpLen)	=0x0D;
		else
				*(outBuf + tmpLen)	=0x00;
		tmpLen++;
		
		// �ű߿�/��״̬
		*(outBuf + tmpLen)	=0xA2;
		tmpLen++;
		*(outBuf + tmpLen)	=0x01;
		tmpLen++;		
		if(tmpState.uiLFDoorSt == 0)
				*(outBuf + tmpLen)	=0x00;
		else if(tmpState.uiLFDoorSt == 1)	// �ſ�
				*(outBuf + tmpLen)	=0x03;
		else if(tmpState.uiLFDoorSt == 2)	// �Ź�
				*(outBuf + tmpLen)	=0x01;
		else
				*(outBuf + tmpLen)	=0x00;
		tmpLen++;
			
		*len =tmpLen;
		return 3;	// �̶�����3��������
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
* Description    : ����������ƽ���ʹ��(��������,���ƺУ�Զ�̿س�����ý���豸����)
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
s8	HAL_SendCONProProcess(DEV_CMDCODE_TYPE	cmdType,	u16 proSn,	u8 *pPara,	u16 len)
{
		s8	ret=0;

		// TTS��������
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
								s_conDev1.dataLock	=1;	// ���ı������Դ�������ִ��
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
								s_conDev2.dataLock	=1;	// ���ı������Դ�������ִ��
								ret =0;
						}
						else
								ret =-1;
				}
				else
						ret =-1;
		}
		// ��������
		else if((cmdType == CAR_CMD_IG_ON)		|| (cmdType == CAR_CMD_IG_OFF) || 
			      (cmdType == CAR_CMD_ARM_SET) 	|| (cmdType == CAR_CMD_ARM_RST) ||
						(cmdType == CAR_CMD_FIND_CAR) || (cmdType == CAR_CMD_READSTA) ||
						(cmdType == CAR_CMD_WINUP) 		|| (cmdType == CAR_CMD_WINDOWN)	|| 	
						(cmdType == CAR_CMD_TRUNK))		         
		{
				if(s_devReq.dataLock == 0)
				{
						memset((u8 *)&s_devReq,	0,	sizeof(s_devReq));
						s_devReq.dataLock =1;	// ռ����Դ
						s_devReq.sn	=UpDevSn();
						s_devReq.cmdCode	=cmdType;
						OSMboxPost(OS_CONReqMbox, (void *)&s_devReq); // ����DEV��������
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
* Description    : ��������OBD����ʹ��(���ֶ���죬ʵʱ��������)
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
				s_obdCmd.sn 				= UpObdSn();	// ��ˮ�Ÿ���
				s_obdCmd.proSn			= proSn;
				s_obdCmd.proCmdId		= cmdId;
				s_obdCmd.proParaLen	=	len;	
				if((len != 0) && (len <= DEF_CMDBUF_SIZE))
						memcpy((u8 *)s_obdCmd.proParaBuf,	pPara, len);
				
				OSMboxPost(OS_OBDReqMbox, (void *)&s_obdCmd); // ����OBD��������
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
* Description    : ���������ļ����̴���
* Input          : times: ���ʹ���ĳ��Դ���
* Output         : 
* Return         : 
*******************************************************************************/
u8	HAL_DownLoadFileProcess (u8 times)
{
		INT8U	os_err=0;
		u8	loops=0,retCode=DEF_IAP_NONE_ERR;
		u16	i=0,num=0,tmpLen=0,tmpSend=0,	cunU16Posi=0;
		u16 tmpRev=0,	tmpSn=0,allPacks=0,cunPacks=0;
		u8	packRevBuf[256]={0};	// ÿ�ֽڶ�Ӧ1k�������ݰ��Ƿ�ɹ�����
		u8	tmpSendBuf[300]={0};	// ��ʱ���ͻ����� 
		u8  tmpParaBuf[280]={0};	// ��ʱ����������
		u32	tmpPosiAdd=0;	// ���յ��������ļ���ʼ��ַλ��
		vu32	downLoadCounter=0;	// ���������ļ���ʱ��
		UpdateData_TypeDef	*rxdMsg;
		
		// ����Ҫ�洢����Ϣ
		u8	nameLen=0,  fileName[30]={0},	verb[6]={0},	carInfo[3]={0,0,0}, sizeBuf[8]={0};	
		u32	fileSize=0,	tmpVerbAddr =0,	tmpSizeAddr=0,	tmpAppAddr=0,	cunAppAddr=0;
		
		// ��ʱ��ʼ
		OSTmr_Count_Start(&downLoadCounter);
			
		// ����������///////////////////////////////////////////////////////////////////////////////////////////////		
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
						retCode =DEF_IAP_NET_ERR;	// ���������ݷ��ʹ���
						continue;		// ��һ�γ���
				}
				
				// ��ͨ��Ӧ������
				rxdMsg = (UpdateData_TypeDef *)(OSMboxAccept(OS_DownACKMbox));	// �������
				rxdMsg = (void *)0;
				
				// ������������01AD/////////////////////////////////////////////////////////////////////////////////////
				tmpSend =0;
				memset((u8 *)&tmpSendBuf,	0,	sizeof(tmpSendBuf));
				tmpSn =UpDataSn();
				ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_UPDATEREQ_DEV,	(u8 *)s_common.id,	6,	tmpSendBuf,	&tmpSend);	
				s_update.writeLock =0;	// ���ݽ���		

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
						retCode =DEF_IAP_NET_ERR;	// ���������ݷ��ʹ���					
				}
				else
				{
						#if(DEF_GPRSINFO_OUTPUTEN > 0)							
						if(dbgInfoSwt & DBG_INFO_GPRS)
								sendPrintProcess (0,	tmpSendBuf,	tmpSend);
						#endif
				}
				// ������
				if(retCode !=DEF_IAP_NONE_ERR)
				{
						BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
						continue;
				}
				
				// �ȴ�ͨ��Ӧ��6001/////////////////////////////////////////////////////////////////////////////////////				
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
										retCode =DEF_IAP_NET_ERR;	// Ӧ����ˮ�Ŵ���
								}
						}
						else if(rxdMsg->msgId == DEF_CMDID_UPDATEINFO_PLA)
						{
								// ����յ�0125�����ȴ�ͨ��Ӧ��ֱ�Ӽ�����������(����6001��0125��ճ�����µ��ظ�����)
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
								retCode =DEF_IAP_NET_ERR;	// �Ƿ�Ӧ��ID
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
								retCode =DEF_IAP_NET_ERR;	// Ӧ��ʱ
						}
				}
				// ������
				if(retCode !=DEF_IAP_NONE_ERR)
				{
						BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
						continue;
				}	
				
				// ����������Ϣ0125/////////////////////////////////////////////////////////////////////////////////////
				while(1)
				{
						s_update.writeLock =0;	// ���ݽ���		
						rxdMsg = (UpdateData_TypeDef *)(OSMboxPend(OS_DownACKMbox, 60000, &os_err));	
						if((os_err != OS_TIMEOUT)&&(rxdMsg != (void *)0)) 
						{
								if(rxdMsg->msgId == DEF_CMDID_UPDATEINFO_PLA)
								{
										// �Ƿ�Ϊ�ϵ�����//////////////////////////////
										if((strlen((const char *)fileName) != 0) && (strlen((const char *)fileName) < sizeof(fileName)) &&
											 (strlen((const char *)verb) != 0) 		 && (strlen((const char *)verb) < sizeof(verb)) 				&&
											 (memcmp(fileName,	(rxdMsg->dataBuf+4),	rxdMsg->dataBuf[3]) == 0))
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Continue receive updata pack!!\r\n");
												#endif
												retCode = DEF_IAP_XUCHUAN;	// ��Ҫ�ϵ�����
												break;
										}									
										// ȫ������////////////////////////////////////
										else
										{											 
												// �������
												tmpRev =0;
												tmpLen =0; fileSize =0; 	
												memset(carInfo,	0,	sizeof(carInfo));
												memset(fileName,0,	sizeof(fileName));
												memset(verb,		0,	sizeof(verb));
												memset(packRevBuf,	0,	sizeof(packRevBuf));	// �����ݰ����ռ�¼
												
												// ��ȡ������Ϣ
												tmpLen=0;
												carInfo[0] = rxdMsg->dataBuf[tmpLen];	// ��ϵ
												tmpLen++;
												carInfo[1] = rxdMsg->dataBuf[tmpLen];	// ����
												tmpLen++;
												carInfo[2] = rxdMsg->dataBuf[tmpLen];	// ������Ʒ��
												tmpLen++;
												nameLen		 = rxdMsg->dataBuf[tmpLen];	// �ļ�����
												tmpLen++;
												for(i=0;	(i<sizeof(fileName)) && (i<nameLen);	i++)
												{
														fileName[i] = rxdMsg->dataBuf[tmpLen+i]; // �ļ���
												}
												fileName[i] ='\0';
												tmpLen +=i;
												for(i=0;	(i<sizeof(verb)) && (i<5);	i++)
												{
														verb[i] = rxdMsg->dataBuf[tmpLen+i]; // �汾��
												}
												verb[i] ='\0';
												tmpLen +=i;
												fileSize = (u32)((rxdMsg->dataBuf[tmpLen] << 24) | (rxdMsg->dataBuf[tmpLen+1] << 16) | (rxdMsg->dataBuf[tmpLen+2] << 8) | rxdMsg->dataBuf[tmpLen+3]);
												
												// ���ȺϷ����ж�
												if((fileSize == 0) || (fileSize > FLASH_CPU_APP_SIZE))
												{	
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: 0125 file size error(%d)!!\r\n",	fileSize);
														#endif
														retCode = DEF_IAP_FILE_SIZE_ERR;	// �����ļ����ȷǷ�ֱ���˳�		
														break;
												}	
												
												// ����һ����Ҫ���յ����ݰ���
												cunPacks =0;	// ��ǰ������������λ
												if(fileSize%1024 == 0)
														allPacks =fileSize/1024;	
												else
														allPacks =fileSize/1024 + 1;	
												
												// ����׼������//////////////////////////////////////////////////////////////
												// ��ȡ��һ�������ļ��洢λ��
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
														retCode =DEF_IAP_WFLASH_READ_ERR;	// �ⲿFLASH��ȡ����
														break;
												}
												
												// �ж�λ�úϷ���
												if((cunU16Posi != DEF_IAP_BACKUP_POSI1) && (cunU16Posi != DEF_IAP_BACKUP_POSI2))
														cunU16Posi =DEF_IAP_BACKUP_POSI1;	// λ�÷Ƿ���λ��λ��1

												// д���������б�־(�����λ��ɼ�������)
												
												// �����ⲿFLASH��Ӧ�洢��
												if(cunU16Posi == DEF_IAP_BACKUP_POSI1)
												{
														tmpVerbAddr = FLASH_SPI_NAME2_START;			// ��ǰ��λ��1�򱸷ݵ�λ��2	
														tmpSizeAddr = FLASH_SPI_SIZE2_START;
														tmpAppAddr  = FLASH_SPI_APP2_START;
														cunU16Posi	= DEF_IAP_BACKUP_POSI2;				// �޸ĵ�ǰ����洢λ�õ�2
												}
												else
												{
														tmpVerbAddr = FLASH_SPI_NAME1_START;			// ��ǰ��λ��2�򱸷ݵ�λ��1
														tmpSizeAddr = FLASH_SPI_SIZE1_START;
														tmpAppAddr  = FLASH_SPI_APP1_START;
														cunU16Posi	= DEF_IAP_BACKUP_POSI1;				// �޸ĵ�ǰ����洢λ�õ�1
												}
												// �����汾���ֶ�
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
														retCode =DEF_IAP_WFLASH_ERASE_ERR;	// �ⲿFLASH��������
														break;
												}
												// �����ļ���С�ֶ�
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
														retCode =DEF_IAP_WFLASH_ERASE_ERR;	// �ⲿFLASH��������
														break;
												}
												// ��������洢��
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
														retCode =DEF_IAP_WFLASH_ERASE_ERR;	// �ⲿFLASH��������
														break;
												}
												retCode =DEF_IAP_NONE_ERR;	// �޴������ִ��				
										}									
								}
								else
								{
										#if(DEF_GPRSINFO_OUTPUTEN > 0)							
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("\r\n[GPRS-Update]: Pend 0125 id error(0x%04X)!!\r\n",	rxdMsg->msgId);
										#endif
										retCode =DEF_IAP_NET_ERR;	// �������������ݰ�����
										break;
								}														
						}
						else
						{
								#if(DEF_GPRSINFO_OUTPUTEN > 0)							
								if(dbgInfoSwt & DBG_INFO_GPRS)
										myPrintf("\r\n[GPRS-Update]: Receive 0125 timeout!!\r\n");
								#endif
								retCode =DEF_IAP_NET_ERR;	// ���������ļ���Ϣ��ʱ		
								break;
						}
						break;
				}
				// ������
				if((retCode !=DEF_IAP_NONE_ERR) && (retCode !=DEF_IAP_XUCHUAN))
				{
						BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
						continue;
				}
				
				// ����ͨ��Ӧ��6001////////////////////////////////////////////////////////////////////////////////////////
				tmpSend =0;
				memset((u8 *)&tmpParaBuf,	0,	sizeof(tmpParaBuf));
				memset((u8 *)&tmpSendBuf,	0,	sizeof(tmpSendBuf));
				
				tmpLen =0x03;				
				tmpParaBuf[0] = (u8)(rxdMsg->sn >> 8);	// Ӧ��SN
				tmpParaBuf[1] = (u8)(rxdMsg->sn);	
			
				if(retCode == DEF_IAP_NONE_ERR)
						tmpParaBuf[2] = 0x00;	// �޴���
				else if(retCode == DEF_IAP_XUCHUAN)
						tmpParaBuf[2] = 0x03;	// Ӧ����Ҫ��������
				else
						tmpParaBuf[2] = 0x01;	// Ӧ�����
				
				retCode =DEF_IAP_NONE_ERR;
				ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_COMACK_DEV,	tmpParaBuf,	tmpLen,	tmpSendBuf,	&tmpSend);		
				s_update.writeLock	 =0;	// ����д��		

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
						retCode =DEF_IAP_NET_ERR;	// ���������ݷ��ʹ���
				}
				else
				{
						#if(DEF_GPRSINFO_OUTPUTEN > 0)							
						if(dbgInfoSwt & DBG_INFO_GPRS)
								sendPrintProcess (0,	tmpSendBuf,	tmpSend);
						#endif
				}
				// ������
				if(retCode !=DEF_IAP_NONE_ERR)
				{
						BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
						continue;
				}
				
				// ����������Ϣ0126/////////////////////////////////////////////////////////////////////////////////////
				retCode =DEF_IAP_NONE_ERR;
				s_update.writeLock =0;	// ���ݽ���		
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
												OSSemPost(GSMGprsDataSem);		// �ֶ���ѯ�Ƿ��Ѿ����յ�����			
										}
								}				
								// �ж��豸�Ƿ��Ѿ�����(���չ����д���ﵽһ���̶ȷ������Ͽ��豸)
								if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 0)
										break;								
								OSTimeDly(100);				// ɨ����										
						}
						if(rxdMsg != (void *)0)
						{
								if(rxdMsg->msgId == DEF_CMDID_UPDATEDATA_PLA)
								{
										// �յ������ļ�
										tmpPosiAdd = 0;
										tmpPosiAdd = (u32)((rxdMsg->dataBuf[0] << 24) | 
																			 (rxdMsg->dataBuf[1] << 16) | 
																			 (rxdMsg->dataBuf[2] << 8) 	| 
																			 (rxdMsg->dataBuf[3]));
										if((tmpPosiAdd % 1024) == 0)
												tmpRev = tmpPosiAdd/1024;					// ȡ��ʼλ���ֶ�
										else
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Start addr error(0x%X)!!\r\n",tmpPosiAdd);
												#endif
												retCode = DEF_IAP_START_SIZE_ERR;	// ��ʼλ�����ݴ���
												break;
										}	
										
										if(tmpRev>512)
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Start posi overflow(%d)!!\r\n",tmpRev);
												#endif
												retCode = DEF_IAP_START_SIZE_ERR;	// ��ʼλ���������
												break;
										}										
										tmpLen =(u16)((rxdMsg->dataBuf[4] << 8) | rxdMsg->dataBuf[5]);	// ȡ���ݰ�����
										if(tmpLen>1024)
										{
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Pack len error(%d)!!\r\n",tmpLen);
												#endif
												retCode = DEF_IAP_PACK_SIZE_ERR;	// �����ݰ����ȴ���
												break;
										}
										
										// ����洢���ⲿFLASH
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
														retCode = DEF_IAP_WFLASH_WRITE_ERR;	// �ⲿFLASHд����												
														break;
												}
												// ��¼���ճɹ������ݰ�
												cunPacks ++;	// ��ǰ���ճɹ����ݰ�++
												packRevBuf[tmpRev] = 0x01;	// 1��ʾ��λ���Ѿ��ɹ�д������ļ�

												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Rxd posi= %d, pack len=%d, (%d%%)!\r\n",	tmpRev,tmpLen,(u16)(cunPacks*100/allPacks));
												#endif										
										}
										else
										{
												// �Ѿ��ɽ����˸����ݰ�
												#if(DEF_GPRSINFO_OUTPUTEN > 0)							
												if(dbgInfoSwt & DBG_INFO_GPRS)
														myPrintf("\r\n[GPRS-Update]: Rxd posi= %d, is repeat pack!\r\n",	tmpRev);
												#endif
										}
								}	
								else if(rxdMsg->msgId == DEF_CMDID_UPDATEQUE_PLA)
								{
										// �յ������ɹ���ѯ
										num =0;
										memset(tmpParaBuf,	0,	sizeof(tmpParaBuf));
										tmpSend =0;
										memset((u8 *)&tmpSendBuf,	0,	sizeof(tmpSendBuf));
										
										tmpParaBuf[0] = (u8)(rxdMsg->sn >> 8);	// Ӧ��SN
										tmpParaBuf[1] = (u8)(rxdMsg->sn);	
										tmpParaBuf[2] = 0x00;	// д����״̬									
										for(i=0;	i<allPacks;	i++)
										{
												if(packRevBuf[i] == 0x00)
												{														
														tmpParaBuf[4+num] =i;	// ��¼δ�洢�ɹ����ݰ�λ��
														num++;
												}													
										}										
										tmpParaBuf[3] = num;	// ��¼δ�ɹ��������ݰ�����
										tmpSn =UpDataSn();
										// ���Ӧ���
										ZXT_PackBuild_Process (tmpSn,	DEF_CMDID_UPDATEACK_DEV,	tmpParaBuf,	(num+4),	tmpSendBuf,	&tmpSend);
										s_update.writeLock =0;	// ���ݽ���		
										
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
												retCode =DEF_IAP_NET_ERR;	// ���������ݷ��ʹ���
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
												// �������ݰ�������ɣ�д���ͳ�ϵ�����汾��Ϣ
												// д��ǰ����洢λ��
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
														retCode =DEF_IAP_WFLASH_WRITE_ERR;	// �ⲿFLASHд����
														break;	// �˳����³���
												}
												// д����汾��
												for(i=0; i<5; i++)
												{
														if(BSP_IAP_ProgramBuf_to_WFlash (verb,	6,	tmpVerbAddr) == 0)	
																break;	// д��ʱ����1�ֽڽ�����
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Receive finish Write verb error !!\r\n");
														#endif
														retCode =DEF_IAP_WFLASH_WRITE_ERR;	// �ⲿFLASHд����
														break;	// �˳����³���
												}
												// д�ļ���С
												memset(sizeBuf, 0, sizeof(sizeBuf));
												sprintf((char *)sizeBuf, "%d",	fileSize);	// �ⲿFLASH�ļ������ֶ�ͳһʹ���ַ�����ʽ�洢			
												for(i=0; i<5;	i++)
												{																								
														if(BSP_IAP_ProgramBuf_to_WFlash (sizeBuf,	(strlen((const char *)sizeBuf)+1),	tmpSizeAddr) == 0)
																break;	// д��ʱ����1�ֽڽ�����
												}
												if(i>=5)
												{
														#if(DEF_GPRSINFO_OUTPUTEN > 0)							
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("\r\n[GPRS-Update]: Receive finish Write size error !!\r\n");
														#endif
														retCode =DEF_IAP_WFLASH_WRITE_ERR;	// �ⲿFLASHд����
														break;	// �˳����³���
												}
												// д���ͳ�ϵ������Ʒ�Ƽ���д������־������д����
												HAL_CommonParaReload();
												s_common.carInfo[0] = carInfo[0];
												s_common.carInfo[1] = carInfo[1];
												s_common.carInfo[2] = carInfo[2];	
												s_common.iapSta =DEF_IAP_CLEAN_FLAG;	// ��������־
												if(cunU16Posi	== DEF_IAP_BACKUP_POSI1)
														s_common.iapFlag =DEF_IAP_PRO1MID_FLAG;	// ��дIAP��־
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
														retCode =DEF_IAP_CFLASH_WRITE_ERR;	// CPU FLASH����д����
														break;	// �˳����³���
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
										retCode =DEF_IAP_NET_ERR;	// ���շǷ�������IDֱ���˳�
										break;
								}		
								s_update.writeLock =0;	// ���ݽ���		
						}
						else
						{
								#if(DEF_GPRSINFO_OUTPUTEN > 0)							
								if(dbgInfoSwt & DBG_INFO_GPRS)
										myPrintf("\r\n[GPRS-Update]: Pend 0126 timeout or server break!!\r\n");
								#endif
								retCode =DEF_IAP_NET_ERR;	// ���������ļ���ʱ
								break;	
						}
				}
				if(retCode == DEF_IAP_NONE_ERR)
						break;		// �ɹ�����
				else
				{
						BSP_GPRSFroceBreakSet();	// ����ǿ�ƶ���ʹ��������������
						continue;
				}
		}
		if(retCode != DEF_IAP_NONE_ERR)
		{
				// �ﵽ���Դ�����������־�´β��ٽ���
        HAL_CommonParaReload();
				s_common.iapSta =DEF_IAP_CLEAN_FLAG;	// ��������־
				HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));
		}
		// ��ʱ����//////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
* Description    : ������Ƴ�ʱ
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
* Description    : ���ų������ƴ���(��ƣ����ȣ����ƿ������رյ�)
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
				return HAL_SendOBDProProcess(CMD_SMS,	0x0000,	DEF_CMDID_CONCAR_PLA,	pCmdCode,	cmdCodeLen);	// ����OBD���Ƴ���
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
						return HAL_SendCONProProcess(cmdType,	0x0000,	(u8 *)"",	0);	// �����������
				else
						return DEF_SMSCONTROL_NOSUPORT;	// ��ֹ�������(���ز�֧��)
		}
		else
				return -1;
}

/*****************************************end*********************************************************/



