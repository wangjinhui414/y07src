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
*                                        	APPLICATION CODE
*
*                                     				hal_c.c
*                                              with the
*                                   				Y05D Board
*
* Filename      : hal_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  HAL_MODULE
#include  <stdio.h>
#include	<string.h>
#include	"cfg_h.h"
#include 	"ucos_ii.h"
#include	"bsp_h.h"
#include	"bsp_rtc_h.h"
#include	"bsp_gsm_h.h"
#include	"bsp_gps_h.h"
#include	"bsp_can_h.h"
#include	"bsp_storage_h.h"
#include	"bsp_kline_h.h"	
#include	"bsp_acc_h.h"
#include	"hal_h.h"
#include	"main_h.h"
#include	"hal_protocol_h.h"	
#include	"hal_processGPS_h.h"
#include	"hal_processDBG_h.h"
#include	"hal_ProcessSYS_h.h"
#include	"hal_ProcessQUE_h.h"

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


extern		OS_EVENT					*ParaMutexSem;				// CPU Flash������д�����ź���
static		vu8								ParaMutexFlag=1;			//	��OSʱʵ��CPU Flash������д����


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static		void			HardErrVarbInit 					(void);		// ϵͳӲ������״̬�ṹ��ʼ��
static		s8				LoadParaPro 							(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);
static		s8				SaveParaPro 							(ENUM_PARA_TYPE	type,	void *pStr,	u16 len);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     		GLOBLE APP Tool
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*********************************************************************************************************
*                                     OTHER FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  : HAL_CPU_ResetPro
* Description    : CPU��λӦ�ò�������,��λǰ��¼��ǰ���״̬     
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void HAL_CPU_ResetPro (void)
{
		// 2015-1-20 by:gaofei �洢��ǰ���״̬,��ֹ��λ��״̬��ʧ
		if(HAL_SYS_GetState (BIT_SYSTEM_IG) == 1)
				BSP_BACKUP_SaveIGOnSta();
	
		BSP_CPU_Reset();
}
/*
*********************************************************************************************************
* Function Name  : HardErrVarbInit
* Description    : ��ʼ��Ӳ�����Ͻṹ				      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	void	HardErrVarbInit (void)
{
	 	s_devSta.pFlash = &(FLASH_DEVICE.HardWareSta);
		s_devSta.pMems	=	&(MEMS_DEVICE.HardWareSta);
		s_devSta.pGps 	= &(GPS_DEVICE.HardWareSta);
		s_devSta.pGsm 	= &(s_GSMcomm.HardWareSta);	
		s_devSta.pObd 	= (void *)0; 
//		s_devSta.pCon		= &(CON_DEVICE.HardWareSta);
}
/*
******************************************************************************
* Function Name  : LoadParaPro()
* Description    : ��ȡϵͳ���������ϵͳͨ�ò������洢���ݺϷ���(����Ƿ��ʼ������У����Ƿ���ȷ)
* Input          : type :ENUM_PARA_TYPE �������Ա
*								 : *pStr:��Ҫ��ȡ�Ľṹ��ָ�� 
*                : len 	:��Ҫ�Ķ�ȡ�����ݳ���
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	LoadParaPro (ENUM_PARA_TYPE	type,	void *pStr,	u16 len)
{
		SYSCommon_Typedef	*pCommon;
		SYSConfig_Typedef	*pCfg;
#ifdef DEF_OBDTASK_CREATE  
		TYPEDEF_OBD_PAR 	*pObd; 
#endif	
		
		if(type == DEF_TYPE_COMMON)
		{
				if(len == sizeof(SYSCommon_Typedef))
				{		
						if(BSP_CPU_FLASH_Read (ADDR_START_COMMON,	(u8 *)pStr,	len) == DEF_CPUFLASH_NONE_ERR)
						{
								pCommon = (SYSCommon_Typedef *)pStr;
								if(pCommon->savedMark != DEF_DFT_COMMON_SAVED_MARK)
										return	DEF_PARA_INIT_ERR;
								else
								{
										if(pCommon->sum != SumCheck16Pro (((u8 *)pCommon + 4),	(len - 4)))
												return	DEF_PARA_SUM_ERR;
										else
												return	DEF_PARA_NONE_ERR;
								}
						}
						else
								return	DEF_PARA_READ_ERR;		
				}
				else
						return	DEF_PARA_UNKNOW_ERR;	
		}
		else if(type == DEF_TYPE_SYSCFG)
		{
				if(len == sizeof(SYSConfig_Typedef))
				{
						if(BSP_CPU_FLASH_Read (ADDR_START_SYSCFG,	(u8 *)pStr,	len) == DEF_CPUFLASH_NONE_ERR)
						{
								pCfg 		= (SYSConfig_Typedef *)pStr;
								if(pCfg->savedMark != DEF_DFT_SYSCFG_SAVED_MARK)
										return	DEF_PARA_INIT_ERR;
								else
								{
										if(pCfg->sum != SumCheck16Pro (((u8 *)pCfg + 4),	(len - 4)))
												return	DEF_PARA_SUM_ERR;
										else
												return	DEF_PARA_NONE_ERR;
								}
						}
						else
								return	DEF_PARA_READ_ERR;		
				}
				else
						return	DEF_PARA_UNKNOW_ERR;	
		}
		else if(type == DEF_TYPE_SYSOBD)
		{
				if(len <= FLASH_CPU_SYSOBD_SIZE)		// OBD����͸������
				{
						#ifdef DEF_OBDTASK_CREATE
						if(BSP_CPU_FLASH_Read (ADDR_START_SYSOBD,	(u8 *)pStr,	len) == DEF_CPUFLASH_NONE_ERR)
						{
								
								pObd 		= (TYPEDEF_OBD_PAR *)pStr;
								if(pObd->savedMark != DEF_DFT_SYSOBD_SAVED_MARK)
										return	DEF_PARA_INIT_ERR;
								else
								{
										if(pObd->sum != SumCheck16Pro (((u8 *)pObd + 4),	(sizeof(TYPEDEF_OBD_PAR) - 4)))
												return	DEF_PARA_SUM_ERR;
										else
												return	DEF_PARA_NONE_ERR;
								}
								
						}
						else
								return	DEF_PARA_READ_ERR;		
						#else
						return	DEF_PARA_NONE_ERR;
						#endif				
				}
				else
						return	DEF_PARA_UNKNOW_ERR;	
		}
		else
				return	DEF_PARA_TYPE_ERR;
}
/*
******************************************************************************
* Function Name  : SaveParaPro()
* Description    : �洢ϵͳ�������������ۼӺ��ֽ�
* Input          : type :ENUM_PARA_TYPE �������Ա
*								 : *pStr:��Ҫ�洢�Ľṹ��ָ�� 
*								 : len  :��Ҫд������ݳ���
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	SaveParaPro (ENUM_PARA_TYPE	type,	void *pStr,	u16 len)
{
		SYSCommon_Typedef	*pCommon;
		SYSConfig_Typedef	*pCfg; 
#ifdef DEF_OBDTASK_CREATE	
		TYPEDEF_OBD_PAR		*pObd;  
#endif
	
		if(type == DEF_TYPE_COMMON)
		{
				if(len == sizeof(SYSCommon_Typedef))
				{
						pCommon = (SYSCommon_Typedef *)pStr;
            if (DEF_DFT_COMMON_SAVED_MARK == pCommon->savedMark)
            {
                pCommon->sum	=	SumCheck16Pro (((u8 *)pCommon + 4),	(len - 4));
                if(BSP_CPU_FLASH_PageErase(ADDR_START_COMMON,	FLASH_CPU_COMMON_SIZE) == DEF_CPUFLASH_NONE_ERR)	//	����������
                {
                    if(BSP_CPU_FLASH_Write (ADDR_START_COMMON,	(u8	*)pCommon,	len) == DEF_CPUFLASH_NONE_ERR)
                        return	DEF_PARA_NONE_ERR;
                    else
                        return	DEF_PARA_WRITE_ERR;
                }
                else
                    return	DEF_PARA_ERASE_ERR;	
            }
            else
            {
                return	DEF_PARA_UNKNOW_ERR;
            }
				}
				else
						return	DEF_PARA_UNKNOW_ERR;
		}
		else if(type == DEF_TYPE_SYSCFG)
		{
				if(len == sizeof(SYSConfig_Typedef))
				{
						pCfg = (SYSConfig_Typedef *)pStr;
            if (DEF_DFT_SYSCFG_SAVED_MARK == pCfg->savedMark)
            {
                if (   (DEF_DFT_COMMON_SAVED_MARK == s_common.savedMark)
                    && (SumCheck16Pro (((u8 *)&s_common + 4),	(sizeof(SYSCommon_Typedef) - 4)) == s_common.sum)
                   )
                {
                }
                else
                {
                    if (   (*((u16 *)(ADDR_START_SYSCFG + 1024) + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                        && (*((u16 *)(ADDR_START_SYSCFG + 1024) + 1u) == SumCheck16Pro((u8 *)(ADDR_START_SYSCFG + 1024 + 4u), sizeof(s_common) - 4u))
                       )
                    {
                        memcpy((u8 *)&s_common, (u8 *)(ADDR_START_SYSCFG + 1024u), sizeof(SYSCommon_Typedef));
                    }
                    else
                    {
                    }
                }
                
                pCfg->sum	=	SumCheck16Pro (((u8 *)pCfg + 4),	(len - 4));
                if(BSP_CPU_FLASH_PageErase(ADDR_START_SYSCFG,	FLASH_CPU_SYSCFG_SIZE) == DEF_CPUFLASH_NONE_ERR)	//	����������
                {
                    if(BSP_CPU_FLASH_Write (ADDR_START_SYSCFG,	(u8	*)pCfg,	len) == DEF_CPUFLASH_NONE_ERR)
                    {
                        if ((len <= 1024u) && (sizeof(SYSCommon_Typedef) <= 1024u))
                        {
                            if (   (DEF_DFT_COMMON_SAVED_MARK == s_common.savedMark)
                                && (SumCheck16Pro (((u8 *)&s_common + 4),	(sizeof(SYSCommon_Typedef) - 4)) == s_common.sum)
                               )
                            {
                                (void)BSP_CPU_FLASH_Write (ADDR_START_SYSCFG + 1024u,	(u8	*)&s_common,	sizeof(SYSCommon_Typedef));
                            }
                            else
                            {
                            }
                        }
                        else
                        {
                        }
                        return	DEF_PARA_NONE_ERR;
                    }
                    else
                    {
                        return	DEF_PARA_WRITE_ERR;
                    }
                }
                else
                    return	DEF_PARA_ERASE_ERR;	
            }
            else
            {
                return	DEF_PARA_UNKNOW_ERR;
            }
				}
				else
						return	DEF_PARA_UNKNOW_ERR;
		}
		else if(type == DEF_TYPE_SYSOBD)
		{
				if(len <= FLASH_CPU_SYSOBD_SIZE)		// OBD����͸������
				{
						#ifdef DEF_OBDTASK_CREATE
						pObd = (TYPEDEF_OBD_PAR *)pStr;
						pObd->savedMark = DEF_DFT_SYSOBD_SAVED_MARK;
						pObd->sum	=	SumCheck16Pro (((u8 *)pObd + 4),	(sizeof(TYPEDEF_OBD_PAR) - 4));
						if(BSP_CPU_FLASH_PageErase(ADDR_START_SYSOBD,	FLASH_CPU_SYSOBD_SIZE) == DEF_CPUFLASH_NONE_ERR)	//	����������
						{
								if(BSP_CPU_FLASH_Write (ADDR_START_SYSOBD,	(u8	*)pStr,	len) == DEF_CPUFLASH_NONE_ERR)
										return	DEF_PARA_NONE_ERR;
								else
										return	DEF_PARA_WRITE_ERR;
						}
						else
								return	DEF_PARA_ERASE_ERR;	
						#else
						return	DEF_PARA_NONE_ERR;
						#endif
				}
				else
						return	DEF_PARA_UNKNOW_ERR;				
		}	
		else
				return	DEF_PARA_TYPE_ERR;
}
/*
******************************************************************************
* Function Name  : HAL_LoadParaPro()
* Description    : ��ȡϵͳ���������ϵͳͨ�ò������洢���ݺϷ���(����Ƿ��ʼ������У����Ƿ���ȷ)
* Input          : type :ENUM_PARA_TYPE �������Ա
*								 : *pStr:��Ҫ��ȡ�Ľṹ��ָ�� 
*                : len 	:��Ҫ�Ķ�ȡ�����ݳ���
* Output         : None
* Return         : None
******************************************************************************
*/
s8	HAL_LoadParaPro (ENUM_PARA_TYPE	type,	void *pStr,	u16 len)
{
		u8	i=0;
		s8	ret=0;
		u8 err=OS_NO_ERR;
	
		if(OSRunning > 0)
				OSSemPend(ParaMutexSem,	5000,	&err);		//	�����ź���
		else
				if(ParaMutexFlag	== 1)
						ParaMutexFlag	=	0;
				else
						return DEF_PARA_BUSY_ERR;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_PARA_BUSY_ERR;
				else
						return	DEF_PARA_BUSY_ERR;		
		}
		else
		{	
				for(i=0;	i<DEF_TRYTIMES_CPUWRITE;	i++)
				{
						ret =LoadParaPro (type,	pStr,	len);
						if(ret == DEF_PARA_NONE_ERR)
            {
								break;
            }
            else
            {
                OSTimeDly(100u);
                #if(OS_SYSWDG_EN > 0)
                IWDG_ReloadCounter();	
                #endif
            }
				}
				// CPU Flash����ͳ��
				if(i >= DEF_TRYTIMES_CPUWRITE)
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: CPU Flash read error!\r\n");
						#endif
						//APP_ErrAdd(&s_err.cpuFlashTimes);	// 2015-5-29 by:gaofei ֻ��дCPU FLASH�����������ͳ�����ϴ�log����		
				}
		}
		
		if(OSRunning > 0)
				OSSemPost(ParaMutexSem);						
		else
				ParaMutexFlag	=	1;
							
		return ret;
}

/*
******************************************************************************
* Function Name  : ParaChksumCalc()
* Description    : �������У���
* Input          : pucData :��У������
*								 : uiLen  :���ݳ���
* Output         : None
* Return         : У���
******************************************************************************
*/
static __inline u16 ParaChksumCalc(u8 *pucData, u32 uiLen)
{
    return SumCheck16Pro(pucData, uiLen);
}

/*
******************************************************************************
* Function Name  : BackupParaTypeGet()
* Description    : ��ȡ��������������
* Input          : None
* Output         : None
* Return         : (u8)DEF_TYPE_COMMON��(u8)DEF_TYPE_SYSCFG��0xFF
******************************************************************************
*/
static __inline u8 BackupParaTypeGet(void)
{
    u8  ucRet  = 0xFFu;
    u16 usMark = *(u16 *)FLASH_CPU_APPSIZE_START;
    
    if (DEF_DFT_COMMON_SAVED_MARK == usMark)
    {
        ucRet = (u8)DEF_TYPE_COMMON;
    }
    else if (DEF_DFT_SYSCFG_SAVED_MARK == usMark)
    {
        ucRet = (u8)DEF_TYPE_SYSCFG;
    }
    else
    {
    }
    
    return ucRet;
}

/*
******************************************************************************
* Function Name  : BackupparaLoad()
* Description    : �����������ݶ���
* Input          : type :����
*								 : *pStr:��Ҫ�����Ľṹ��ָ�� 
*								 : len  :��Ҫ���������ݳ���
* Output         : None
* Return         : 0:ʧ�ܣ�1���ɹ�
******************************************************************************
*/
static u8 BackupparaLoad(u8 ucType, void *pData, u32 uiLen)
{
    u8 ucRet = 0u;
  
    if (0xFFu != ucType)
    {
        memcpy((u8 *)pData, (u8 *)FLASH_CPU_APPSIZE_START, uiLen);
        if ((u8)DEF_TYPE_COMMON == ucType)
        {
            if (   (*((u16 *)pData + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                && (*((u16 *)pData + 1u) == ParaChksumCalc((u8 *)pData + 4u, uiLen - 4u))
               )
            {
                ucRet = 1u;
            }
            else
            {
            }
        }
        else if ((u8)DEF_TYPE_SYSCFG == ucType)
        {
            if (   (*((u16 *)pData + 0u) == DEF_DFT_SYSCFG_SAVED_MARK) 
                && (*((u16 *)pData + 1u) == ParaChksumCalc((u8 *)pData + 4u, uiLen - 4u))
               )
            {
                ucRet = 1u;
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
* Function Name  : BackupParaSave()
* Description    : ��ϵͳ�����洢�ڱ�����
* Input          : type :����
*								 : *pStr:��Ҫ�洢�Ľṹ��ָ�� 
*								 : len  :��Ҫд������ݳ���
* Output         : None
* Return         : DEF_PARA_WRITE_ERR��DEF_PARA_NONE_ERR��DEF_PARA_ERASE_ERR
******************************************************************************
*/
static s8  BackupParaSave(u8 ucType, void *pData, u32 uiLen)
{
    s8 scRet     = DEF_PARA_NONE_ERR;
    u8 ucHitFlag = 0u;
    u8 ucBkType  = BackupParaTypeGet();
  
    if (((u8)DEF_TYPE_COMMON == ucType) || ((u8)DEF_TYPE_SYSCFG == ucType))
    {
        *((u16*)pData + 1u)	=	SumCheck16Pro ((u8 *)((u8 *)pData + 4),	(uiLen - 4));
      
        if ((u8)DEF_TYPE_COMMON == ucType)
        {
            if(uiLen == sizeof(SYSCommon_Typedef))
            {
                if (DEF_DFT_COMMON_SAVED_MARK == *(u16*)pData)
                {
                    if ((u8)DEF_TYPE_SYSCFG == ucBkType)
                    {
                        if (   (*((u16 *)FLASH_CPU_SYSCFG_START + 0u) == DEF_DFT_SYSCFG_SAVED_MARK) 
                            && (*((u16 *)FLASH_CPU_SYSCFG_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_SYSCFG_START + 4u), sizeof(SYSConfig_Typedef) - 4u))
                           )
                        {
                            ucHitFlag = 1u;
                        }
                        else
                        {
                            if (   (*((u16 *)FLASH_CPU_APPSIZE_START + 0u) == DEF_DFT_SYSCFG_SAVED_MARK) 
                                && (*((u16 *)FLASH_CPU_APPSIZE_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_APPSIZE_START + 4u), sizeof(SYSConfig_Typedef) - 4u))
                               )
                            {
                                if (DEF_DFT_COMMON_IAPFLAG != s_common.iapFlag)
                                {
                                    ucHitFlag = 1u;
                                }
                                else
                                {
                                }
                            }
                            else
                            {
                                ucHitFlag = 1u;
                            }
                        }
                    }
                    else if ((u8)DEF_TYPE_COMMON == ucBkType)
                    {
                        if (   (*((u16 *)FLASH_CPU_COMMON_START + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                            && (*((u16 *)FLASH_CPU_COMMON_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_COMMON_START + 4u), uiLen - 4u))
                           )
                        {
                            ucHitFlag = 1u;
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                        ucHitFlag = 1u;
                    }
                }
                else
                {
                }
            }
            else
            {
            }
        }
        else if ((u8)DEF_TYPE_SYSCFG == ucType)
        {
            if(uiLen == sizeof(SYSConfig_Typedef))
            {
                if (DEF_DFT_SYSCFG_SAVED_MARK == *(u16*)pData)
                {
                    if ((u8)DEF_TYPE_COMMON == ucBkType)
                    {
                        if (   (*((u16 *)FLASH_CPU_COMMON_START + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                            && (*((u16 *)FLASH_CPU_COMMON_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_COMMON_START + 4u), sizeof(SYSCommon_Typedef) - 4u))
                           )
                        {
                            ucHitFlag = 1u;
                        }
                        else
                        {
                            if (   (*((u16 *)FLASH_CPU_APPSIZE_START + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                                && (*((u16 *)FLASH_CPU_APPSIZE_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_APPSIZE_START + 4u), sizeof(SYSCommon_Typedef) - 4u))
                               )
                            {
                            }
                            else
                            {
                                ucHitFlag = 1u;
                            }
                        }
                    }
                    else if ((u8)DEF_TYPE_SYSCFG == ucBkType)
                    {
                        if (   (*((u16 *)FLASH_CPU_SYSCFG_START + 0u) == DEF_DFT_SYSCFG_SAVED_MARK) 
                            && (*((u16 *)FLASH_CPU_SYSCFG_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_SYSCFG_START + 4u), uiLen - 4u))
                           )
                        {
                            ucHitFlag = 1u;
                        }
                        else
                        {
                            if ((*((u16 *)FLASH_CPU_SYSCFG_START + 0u) == DEF_DFT_SYSCFG_SAVED_MARK) && (1u == s_common.otapErrCode))
                            {
                                ucHitFlag = 1u;
                            }
                            else
                            {
                            }
                        }
                    }
                    else
                    {
                        ucHitFlag = 1u;
                    }
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
        
        if (1u == ucHitFlag)
        {
            if(BSP_CPU_FLASH_PageErase(FLASH_CPU_APPSIZE_START,	FLASH_CPU_APPSIZE_SIZE) == DEF_CPUFLASH_NONE_ERR)	//	����������
            {
                if(BSP_CPU_FLASH_Write(FLASH_CPU_APPSIZE_START,	(u8	*)pData,	uiLen) == DEF_CPUFLASH_NONE_ERR)
                {
                    scRet =	DEF_PARA_NONE_ERR;
                }
                else
                {
                    scRet =	DEF_PARA_WRITE_ERR;
                }
            }
            else
            {
                scRet = DEF_PARA_ERASE_ERR;
            }
        }
        else
        {
        }
    }
    else
    {
    }
    
    return scRet;
}

/*
******************************************************************************
* Function Name  : HAL_CommonParaReload
* Description    : ���¶�ȡͨ�ò��� 
*								 : 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
s8 HAL_CommonParaReload(void)
{
    s8 ret = -1;
  
		ret = HAL_LoadParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));
  
    if((ret == DEF_PARA_INIT_ERR) || ((ret == DEF_PARA_SUM_ERR)))
		{
        #if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: CPU Flash Common parameter reset!\r\n");
				#endif	
        
        if ((u8)DEF_TYPE_COMMON == BackupParaTypeGet())
        {
            if (   ((SYSCommon_Typedef *)FLASH_CPU_APPSIZE_START)->sum 
                == ParaChksumCalc((u8 *)(((u8 *)FLASH_CPU_APPSIZE_START) + 4u), sizeof(s_common) - 4u)
               )
            {
                if (1u == BackupparaLoad((u8)DEF_TYPE_COMMON, (void *)&s_common, sizeof(s_common)))
                {
                    ret = 0;
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
		}	
    else if (DEF_PARA_NONE_ERR == ret)
    {
        ret = 0;
    }
    else
    {
    }
    
    return ret;
}

/*
******************************************************************************
* Function Name  : HAL_SysParaReload
* Description    : ���¶�ȡϵͳ���� 
*								 : 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
void HAL_SysParaReload(void)
{
		s8 ret = 0x00u;
  
		ret =HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg));
    
    if (DEF_PARA_NONE_ERR == ret)
    {
    }
    else if((ret == DEF_PARA_INIT_ERR) || (ret == DEF_PARA_SUM_ERR))
		{
        #if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: CPU Flash Syscfg parameter reset!\r\n");
				#endif	
        
        if (1u == s_common.otapErrCode)
        {
            if (ret == DEF_PARA_SUM_ERR)
            {
            }
            else
            {
                HAL_ParaSetDefault(DEF_TYPE_SYSCFG);		// ������λ
            }
            
            HAL_ParaSetDefaultExt(DEF_TYPE_SYSCFG);		// ���ֲ�����λ(ֻ��λ�޸ĵĲ������֣�������������)
        }
        else
        {
            if ((u8)DEF_TYPE_SYSCFG == BackupParaTypeGet())
            {
                if (   ((SYSConfig_Typedef *)FLASH_CPU_APPSIZE_START)->sum 
                    == ParaChksumCalc((u8 *)(((u8 *)FLASH_CPU_APPSIZE_START) + 4u), sizeof(s_cfg) - 4u)
                   )
                {
                    if (1u == BackupparaLoad((u8)DEF_TYPE_SYSCFG, (void *)&s_cfg, sizeof(s_cfg)))
                    {
                    }
                    else
                    {
                        HAL_ParaSetDefault   (DEF_TYPE_SYSCFG);		// ������λ                   
                        HAL_ParaSetDefaultExt(DEF_TYPE_SYSCFG);		// ���ֲ�����λ(ֻ��λ�޸ĵĲ������֣�������������)
                    }
                }
                else
                {
                    HAL_ParaSetDefault   (DEF_TYPE_SYSCFG);		// ������λ                   
                    HAL_ParaSetDefaultExt(DEF_TYPE_SYSCFG);		// ���ֲ�����λ(ֻ��λ�޸ĵĲ������֣�������������)
                }
            }
            else
            {
                HAL_ParaSetDefault   (DEF_TYPE_SYSCFG);		// ������λ
                HAL_ParaSetDefaultExt(DEF_TYPE_SYSCFG);		// ���ֲ�����λ(ֻ��λ�޸ĵĲ������֣�������������)
            }
        }
		}	
    else
    {
    }
}

/*
******************************************************************************
* Function Name  : HAL_CfgChk
* Description    : ���RAM�е������Ƿ���ȷ 
*								 : 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
extern s8	HAL_CfgLogInQueueProcess	(u8 ucCfg);
void HAL_CfgChk(void)
{
    static u8   s_ucStart       = 0u;
    static u8   s_ucComCnt      = 0u;
    static u8   s_ucComFlashCnt = 0u;
    static u8   s_ucCfgCnt      = 0u;
    static u8   s_ucCfgFlashCnt = 0u;
    static vu32	s_CfgChkTmr     = 0u;
    u8 ucCfgErrFalg             = 0u;
  
    if (0u == s_ucStart)
    {
        s_ucStart = 1u;
        OSTmr_Count_Start(&s_CfgChkTmr);
    }
    else
    {
    }
    
    if (OSTmr_Count_Get(&s_CfgChkTmr) > 10000u)
    {
        OSTmr_Count_Start(&s_CfgChkTmr);
      
        if (   (DEF_DFT_COMMON_SAVED_MARK != s_common.savedMark)
            || (ParaChksumCalc((u8 *)(((u8 *)&s_common) + 4u), sizeof(s_common) - 4u) != s_common.sum)
           )
        {
            if (   (*((u16 *)FLASH_CPU_COMMON_START + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                && (*((u16 *)FLASH_CPU_COMMON_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_COMMON_START + 4u), sizeof(SYSCommon_Typedef) - 4u))
               )
            {
                s_ucComCnt ++;
            }
            else
            {
                if ((u8)DEF_TYPE_COMMON == BackupParaTypeGet())
                {
                    if (   (*((u16 *)FLASH_CPU_APPSIZE_START + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                        && (*((u16 *)FLASH_CPU_APPSIZE_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_APPSIZE_START + 4u), sizeof(SYSCommon_Typedef) - 4u))
                       )
                    {
                        s_ucComCnt ++;
                    }
                    else
                    {
                        if (   (*((u16 *)(ADDR_START_SYSCFG + 1024) + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                            && (*((u16 *)(ADDR_START_SYSCFG + 1024) + 1u) == ParaChksumCalc((u8 *)((ADDR_START_SYSCFG + 1024) + 4u), sizeof(SYSCommon_Typedef) - 4u))
                           )
                        {
                            s_ucComCnt ++;
                        }
                        else
                        {
                            s_ucComFlashCnt ++;
                        }
                    }
                }
                else
                {
                    if (   (*((u16 *)(ADDR_START_SYSCFG + 1024) + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                        && (*((u16 *)(ADDR_START_SYSCFG + 1024) + 1u) == ParaChksumCalc((u8 *)((ADDR_START_SYSCFG + 1024) + 4u), sizeof(SYSCommon_Typedef) - 4u))
                       )
                    {
                        s_ucComCnt ++;
                    }
                    else
                    {
                        s_ucComFlashCnt ++;
                    }
                }
            }
        }
        else
        {
            s_ucComCnt      = 0u;
            s_ucComFlashCnt = 0u;
        }
        
        if (   (DEF_DFT_SYSCFG_SAVED_MARK != s_cfg.savedMark)
            || (ParaChksumCalc((u8 *)(((u8 *)&s_cfg) + 4u), sizeof(s_cfg) - 4u) != s_cfg.sum)
           )
        {
            if (   (*((u16 *)FLASH_CPU_SYSCFG_START + 0u) == DEF_DFT_SYSCFG_SAVED_MARK) 
                && (*((u16 *)FLASH_CPU_SYSCFG_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_SYSCFG_START + 4u), sizeof(SYSConfig_Typedef) - 4u))
               )
            {
                s_ucCfgCnt ++;
            }
            else
            {
                if ((u8)DEF_TYPE_SYSCFG == BackupParaTypeGet())
                {
                     if (   (*((u16 *)FLASH_CPU_APPSIZE_START + 0u) == DEF_DFT_SYSCFG_SAVED_MARK) 
                         && (*((u16 *)FLASH_CPU_APPSIZE_START + 1u) == ParaChksumCalc((u8 *)(FLASH_CPU_APPSIZE_START + 4u), sizeof(SYSConfig_Typedef) - 4u))
                        )
                     {
                          s_ucCfgCnt ++;
                     }
                     else
                     {
                          s_ucCfgFlashCnt ++;
                     }
                }
                else
                {
                      s_ucCfgFlashCnt ++;
                }
            }
        }
        else
        {
            s_ucCfgCnt      = 0u;
            s_ucCfgFlashCnt = 0u;
        }
        
        if (6u == s_ucComCnt)
        {
            if (DEF_DFT_COMMON_SAVED_MARK != s_common.savedMark)
            {
                ucCfgErrFalg |= 0x10u;
            }
            else
            {
            }
            
            if (ParaChksumCalc((u8 *)(((u8 *)&s_common) + 4u), sizeof(s_common) - 4u) != s_common.sum)
            {
                ucCfgErrFalg |= 0x20u;
            }
            else
            {
            }
            
            if (s_ucComFlashCnt > 0u)
            {
                ucCfgErrFalg |= 0x40u;
            }
            else
            {
            }
            
            s_ucComCnt      = 0u;
            s_ucComFlashCnt = 0u;
        }
        else
        {
        }
        
        if (6u == s_ucCfgCnt)
        {
            if (DEF_DFT_SYSCFG_SAVED_MARK != s_cfg.savedMark)
            {
                ucCfgErrFalg |= 0x01u;
            }
            else
            {
            }
            
            if (ParaChksumCalc((u8 *)(((u8 *)&s_cfg) + 4u), sizeof(s_cfg) - 4u) != s_cfg.sum)
            {
                ucCfgErrFalg |= 0x02u;
            }
            else
            {
            }
            
            if (s_ucCfgFlashCnt > 0u)
            {
                ucCfgErrFalg |= 0x04u;
            }
            else
            {
            }
            
            s_ucCfgCnt      = 0u;
            s_ucCfgFlashCnt = 0u;
        }
        else
        {
        }
        
        if (0u != ucCfgErrFalg)
        {
            HAL_CfgLogInQueueProcess(ucCfgErrFalg);
          
            #if(DEF_MEMINFO_OUTPUTEN > 0)
            if(dbgInfoSwt & DBG_INFO_OTHER)
                myPrintf("[CFG_ERR]: RAM CFG Err: 0x%02X!\r\n",ucCfgErrFalg);
            #endif	
        
            if (0u != (ucCfgErrFalg & 0xF0u))
            {
                HAL_CommonParaReload();
                #if(DEF_MEMINFO_OUTPUTEN > 0)
                if(dbgInfoSwt & DBG_INFO_OTHER)
                    myPrintf("[PARA RELOAD]: Common Para Reloaded!\r\n",ucCfgErrFalg);
                #endif	
            }
            else
            {
            }
            
            if (0u != (ucCfgErrFalg & 0x0Fu))
            {
                HAL_SysParaReload();
                #if(DEF_MEMINFO_OUTPUTEN > 0)
                if(dbgInfoSwt & DBG_INFO_OTHER)
                    myPrintf("[PARA RELOAD]: Sys Para Reloaded!\r\n",ucCfgErrFalg);
                #endif
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
}

/*
******************************************************************************
* Function Name  : HAL_SaveParaPro()
* Description    : �洢ϵͳ�������������ۼӺ��ֽ�
* Input          : type :ENUM_PARA_TYPE �������Ա
*								 : *pStr:��Ҫ�洢�Ľṹ��ָ�� 
*								 : len  :��Ҫд������ݳ���
* Output         : None
* Return         : None
******************************************************************************
*/
s8	HAL_SaveParaPro (ENUM_PARA_TYPE	type,	void *pStr,	u16 len)
{
		u8	i=0;
		s8	ret=0;
		u8 err=OS_NO_ERR;
	
		if(OSRunning > 0)
				OSSemPend(ParaMutexSem,	5000,	&err);		//	�����ź���
		else
				if(ParaMutexFlag	== 1)
						ParaMutexFlag	=	0;
				else
						return DEF_PARA_BUSY_ERR;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_PARA_BUSY_ERR;
				else
						return	DEF_PARA_BUSY_ERR;		
		}
		else
		{
        if ((DEF_TYPE_COMMON == type) || (DEF_TYPE_SYSCFG == type))
        {
            for(i=0;	i<DEF_TRYTIMES_CPUWRITE;	i++)
            {
                ret = BackupParaSave(type,	pStr,	len);
                if(ret == DEF_PARA_NONE_ERR)
                {
                    break;
                }
                else
                {
                    OSTimeDly(100u);
                    #if(OS_SYSWDG_EN > 0)
                    IWDG_ReloadCounter();	
                    #endif
                }
            }
        }
        else
        {
        }
        
        if(ret == DEF_PARA_NONE_ERR)
        {
            for(i=0;	i<DEF_TRYTIMES_CPUWRITE;	i++)
            {
                ret =SaveParaPro (type,	pStr,	len);
                if(ret == DEF_PARA_NONE_ERR)
                {
                    break;
                }
                else
                {
                    OSTimeDly(100u);
                    #if(OS_SYSWDG_EN > 0)
                    IWDG_ReloadCounter();	
                    #endif	
                }
            }
        }
        else
        {
            i = DEF_TRYTIMES_CPUWRITE;
        }
				// CPU Flash����ͳ��
				if(i >= DEF_TRYTIMES_CPUWRITE)
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: CPU Flash write error!\r\n");
						#endif
						APP_ErrAdd(&s_err.cpuFlashTimes);	// CPU Flash�����ۼ�		
				}
		}
		
		if(OSRunning > 0)
				OSSemPost(ParaMutexSem);						
		else
				ParaMutexFlag	=	1;	
    
    if (i >= DEF_TRYTIMES_CPUWRITE)
    {
        if (DEF_TYPE_SYSCFG == type)
        {
            HAL_SysParaReload();
        }
        else if (DEF_TYPE_COMMON == type)
        {
            HAL_CommonParaReload();
        }
        else
        {
        }
    }
    else
    {
    }
		
		return ret;
}
/*
******************************************************************************
* Function Name  : HAL_SYSParaSemPend()
* Description    : ���ڲ�����д�ź����Ļ�ȡ��������HAL_SYSParaSemPost�ɶ�ʹ��
* Input          : timeout :�ȴ��ź�����ʱʱ��
* Output         : None
* Return         : None
******************************************************************************
*/
s8	HAL_SYSParaSemPend (u32	timeout)
{
		u8 err=OS_NO_ERR;
	
    err = err;
		if(OSRunning > 0)
				OSSemPend(ParaMutexSem,	timeout,	&err);		//	�����ź���
		else
				if(ParaMutexFlag	== 1)
						ParaMutexFlag	=	0;
				else
						return -1;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}
		return 0;
}
/*
******************************************************************************
* Function Name  : HAL_SYSParaSemPost()
* Description    : ���ڲ�����д�ź������ͷţ�������HAL_SYSParaSemPend�ɶ�ʹ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
void	HAL_SYSParaSemPost (void)
{		
		if(OSRunning > 0)
				OSSemPost(ParaMutexSem);						
		else
				ParaMutexFlag	=	1;
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                     PARA Wrtie/Read FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/
/*
******************************************************************************
* Function Name  : HAL_ParaSetDefault
* Description    : ֻ��ϵͳ�����ò�����λ(ϵͳӲ��ID������λ����)
* Input          : [ע]��λǰ��Ҫ�ȶ�ȡϵͳ����
* Output         : None
* Return         : None
******************************************************************************
*/
void	HAL_ParaSetDefault (ENUM_PARA_TYPE	type)
{		
		if(type == DEF_TYPE_COMMON)
		{
				//memset((u8 *)&s_common,	0,	sizeof(s_common));
				s_common.savedMark 	= DEF_DFT_COMMON_SAVED_MARK;
				s_common.iapFlag		=	DEF_DFT_COMMON_IAPFLAG;
				s_common.iapSta			=	DEF_DFT_COMMON_IAPSTA;
				//memset((u8 *)&s_common.id,	DEF_DFT_COMMON_ID,	sizeof(s_common.id)); // ϵͳID������λ����
				memset((u8 *)&s_common.carInfo,	DEF_DFT_COMMON_CARINFO,	sizeof(s_common.carInfo));
				s_common.carInfo[0] = 'O';	s_common.carInfo[1] = 0x00;	s_common.carInfo[2] = 0x00;
				s_common.aadc				=	DEF_DFT_COMMON_AADC;
				memset((u8 *)&s_common.swdg,	DEF_DFT_COMMON_SWDG,	sizeof(s_common.swdg));
				strcpy((char *)s_common.hverb,			DEF_DFT_COMMON_HVERB);		
				s_common.gpsDis			=	DEF_DFT_COMMON_GPSDIS;			
				strcpy((char *)s_common.otapApn,			DEF_DFT_COMMON_OTAPAPN);
				strcpy((char *)s_common.otapApnUser,	DEF_DFT_COMMON_OTAPAPNUSER);
				strcpy((char *)s_common.otapApnPass,	DEF_DFT_COMMON_OTAPAPNPASS);
				strcpy((char *)s_common.otapIp,				DEF_DFT_COMMON_OTAPIP);
				strcpy((char *)s_common.otapPort,			DEF_DFT_COMMON_OTAPPORT);		
				strcpy((char *)s_common.otapFtpUser,	DEF_DFT_COMMON_OTAPFTPUSER);
				strcpy((char *)s_common.otapFtpPass,	DEF_DFT_COMMON_OTAPFTPPASS);
				strcpy((char *)s_common.otapFtpFile,	DEF_DFT_COMMON_OTAPFTPFILE);	
				s_common.otapFileSize=	DEF_DFT_COMMON_OTAPFILESIZE;
				s_common.otapErrCode=	DEF_DFT_COMMON_OTAPERRCODE;
				
				// �����ۼӺ�
				//s_common.sum =SumCheck16Pro (((u8 *)&s_common + 4),	(sizeof(SYSCommon_Typedef) - 4));			
		}
		else if(type == DEF_TYPE_SYSCFG)
		{
				memset((u8 *)&s_cfg,	0,	sizeof(s_cfg));
				s_cfg.savedMark			=DEF_DFT_SYSCFG_SAVED_MARK;				
				strcpy((char *)s_cfg.apn,							DEF_DFT_SYSCFG_APN);
				strcpy((char *)s_cfg.apnUser,					DEF_DFT_SYSCFG_APNUSER);
				strcpy((char *)s_cfg.apnPass,					DEF_DFT_SYSCFG_APNPASS);
				strcpy((char *)s_cfg.dnsIp,						DEF_DFT_SYSCFG_DNSIP);
				strcpy((char *)s_cfg.domain1,					DEF_DFT_SYSCFG_DOMAIN1);
				strcpy((char *)s_cfg.domain2,					DEF_DFT_SYSCFG_DOMAIN2);
				strcpy((char *)s_cfg.ip1,							DEF_DFT_SYSCFG_IP1);
				strcpy((char *)s_cfg.port1,						DEF_DFT_SYSCFG_PORT1);
				strcpy((char *)s_cfg.ip2,							DEF_DFT_SYSCFG_IP2);
				strcpy((char *)s_cfg.port2,						DEF_DFT_SYSCFG_PORT2);
				s_cfg.domainEn			= DEF_DFT_SYSCFG_DOMAINEN;
				s_cfg.gpsCollectTime= DEF_DFT_SYSCFG_GPSCOLLECTTIME;
				s_cfg.onReportTime	= DEF_DFT_SYSCFG_ONREPORTTIME;
				s_cfg.offReportTime	= DEF_DFT_SYSCFG_OFFREPORTTIME;
				s_cfg.heartbeatTime	= DEF_DFT_SYSCFG_HEARTBEATTTIME;
				s_cfg.tcpAckTimeout	= DEF_DFT_SYSCFG_TCPACKTIMEOUT;
				s_cfg.healthReportTime = DEF_DFT_SYSCFG_HEALTHREPORTTIME;
				strcpy((char *)s_cfg.smsCenter,				DEF_DFT_SYSCFG_SMSCENTER);
				s_cfg.eventSwt			= DEF_DFT_SYSCFG_EVENTSWT;
				s_cfg.overSpeed			= DEF_DFT_SYSCFG_OVERSPEED;
				s_cfg.overSpeedTime	= DEF_DFT_SYSCFG_OVERSPEEDTIME;
				s_cfg.tiredDirveTime= DEF_DFT_SYSCFG_TIREDDIRVETIME;
				strcpy((char *)s_cfg.pho1,						DEF_DFT_SYSCFG_PHO1);
				strcpy((char *)s_cfg.pho2,						DEF_DFT_SYSCFG_PHO2);
				strcpy((char *)s_cfg.pho3,						DEF_DFT_SYSCFG_PHO3);
				strcpy((char *)s_cfg.pho4,						DEF_DFT_SYSCFG_PHO4);
				strcpy((char *)s_cfg.pho5,						DEF_DFT_SYSCFG_PHO5);
				strcpy((char *)s_cfg.pho6,						DEF_DFT_SYSCFG_PHO6);
				memset((u8 *)&s_cfg.aeskey,	DEF_DFT_SYSCFG_AESKEY,	sizeof(s_cfg.aeskey));
				s_cfg.aesType				= DEF_DFT_SYSCFG_AESTYPE;
				s_cfg.devEn					= DEF_DFT_SYSCFG_DEVEN;
				s_cfg.wMode					= DEF_DFT_SYSCFG_WMODE;
				strcpy((char *)s_cfg.nationCode,			DEF_DFT_SYSCFG_NATIONCODE);
				s_cfg.sleepSwt			= DEF_DFT_SYSCFG_SLEEPSWT;
				s_cfg.dssATH				= DEF_DFT_SYSCFG_DSSATH;
				s_cfg.dssDTH				= DEF_DFT_SYSCFG_DSSDTH;
				s_cfg.dssRTH				= DEF_DFT_SYSCFG_DSSRTH;
				s_cfg.dssPTH				= DEF_DFT_SYSCFG_DSSPTH;
				s_cfg.accMT					= DEF_DFT_SYSCFG_ACCMT;
				s_cfg.accMD					= DEF_DFT_SYSCFG_ACCMD;
				s_cfg.sysMove				= DEF_DFT_SYSCFG_SYSMOVE;
				s_cfg.sysMoveT			= DEF_DFT_SYSCFG_SYSMOVET;
				s_cfg.lowPower			= DEF_DFT_SYSCFG_LOWPOWER;
				s_cfg.tjDelayTime		= DEF_DFT_SYSCFG_TJDELAYTIME;
				s_cfg.igOffDelay		= DEF_DFT_SYSCFG_IGOFFDELAY;
				s_cfg.naviStep			= DEF_DFT_SYSCFG_NAVISTEP;
				s_cfg.globSleepTime	=	DEF_DFT_SYSCFG_GLOBSLEEPTIME;
				strcpy((char *)s_cfg.sound1,					DEF_DFT_SYSCFG_SOOUND1);
				strcpy((char *)s_cfg.sound2,					DEF_DFT_SYSCFG_SOOUND2);
			
				// �����ۼӺ�
				//s_cfg.sum =SumCheck16Pro (((u8 *)&s_cfg + 4),	(sizeof(SYSConfig_Typedef) - 4));		
		}
		else if(type == DEF_TYPE_SYSOBD)
		{
//				memset((u8 *)&s_obd,	0,	sizeof(s_obd));
//				s_obd.savedMark			=DEF_DFT_SYSOBD_SAVED_MARK;
//				s_obd.odometer			=DEF_DFT_SYSOBD_ODO;
//				s_obd.fuel					=DEF_DFT_SYSOBD_FUEL;
			
				// �����ۼӺ�
				//s_obd.sum =SumCheck16Pro (((u8 *)&s_obd + 4),	(sizeof(SYSObd_Typedef) - 4));	
				// ����OBD
		}	
}	
/*
******************************************************************************
* Function Name  : HAL_ParaSetDefaultExt
* Description    : ֻ�Բ�����Ҫ�ı�Ĭ�ϲ�������ֵ��д��ÿ������Ӧ�ò������дĬ�ϲ�������Ҫ���øú���������Ҫ�����Ӧ����ֵ��
* Input          : [ע]ֻ����������������־����ɺ�ŵ��øú���
* Output         : None
* Return         : None
******************************************************************************
*/
void	HAL_ParaSetDefaultExt (ENUM_PARA_TYPE	type)
{		
		if(type == DEF_TYPE_COMMON)
		{				
				// �����ۼӺ�
				//s_common.sum =SumCheck16Pro (((u8 *)&s_common + 4),	(sizeof(SYSCommon_Typedef) - 4));			
		}
		else if(type == DEF_TYPE_SYSCFG)
		{
				s_cfg.savedMark			=DEF_DFT_SYSCFG_SAVED_MARK;	
				// 2015-5-27: �ڶ����Ż����ݸı������V04.49 �޸ļ��ٶ�Ĭ��ֵ350��550��1100 
				//s_cfg.dssATH				= DEF_DFT_SYSCFG_DSSATH;
				//s_cfg.dssDTH				= DEF_DFT_SYSCFG_DSSDTH;
				//s_cfg.dssRTH				= DEF_DFT_SYSCFG_DSSRTH;
				//s_cfg.dssPTH				= DEF_DFT_SYSCFG_DSSPTH;
				//s_cfg.accMT					= DEF_DFT_SYSCFG_ACCMT;								// 2015-8-19Ϊ������Ʒ�����޸�
				//s_cfg.accMD					= DEF_DFT_SYSCFG_ACCMD;
				//s_cfg.sysMove				= DEF_DFT_SYSCFG_SYSMOVE;
				//s_cfg.sysMoveT			= DEF_DFT_SYSCFG_SYSMOVET;
				//s_cfg.lowPower			= DEF_DFT_SYSCFG_LOWPOWER;
				//s_cfg.healthReportTime = DEF_DFT_SYSCFG_HEALTHREPORTTIME;	// 2015-6-30�����Ϊ��ʹ�豸GPS��ʧ����������ʱ���Ϊ3Сʱ
			
				// 2015-6-19: ��ֹ�¶���Ʋ���
				//s_cfg.devEn					=	DEF_DFT_SYSCFG_DEVEN;
			
				// 2015-8-3: 	�޸�ϵͳ˯��ʱ��Ϊ23:54��86000
				// 2015-9-28: �޸Ļ�30����
				//s_cfg.globSleepTime	=	DEF_DFT_SYSCFG_GLOBSLEEPTIME;
			
			
				// �����ۼӺ�
				//s_cfg.sum =SumCheck16Pro (((u8 *)&s_cfg + 4),	(sizeof(SYSConfig_Typedef) - 4));		
		}
		else if(type == DEF_TYPE_SYSOBD)
		{			
				// �����ۼӺ�
				//s_obd.sum =SumCheck16Pro (((u8 *)&s_obd + 4),	(sizeof(SYSObd_Typedef) - 4));	
				// ����OBD
		}	
}	

/*
******************************************************************************
* Function Name  : HAL_SysParaLoadProcess
* Description    : ϵͳ������ȡ������ֻ����һ�����FLASH����δ����ʼ��������ָ�Ĭ��ֵ 
*								 : 
* Input          : None
* Output         : None
* Return         : ����0��ʾ�������Ѿ�����ʼ��������1��ʾ��������δ����ʼ����
******************************************************************************
*/
u8	HAL_SysParaLoadProcess (void)
{
    s8 ret        = 0x00u;
    u8 ucHit      = 0x01u;
    u8 retCode    = 0x00u;
  
		ret = HAL_LoadParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));
  
    if (DEF_PARA_BUSY_ERR == ret)
    {
        HAL_CPU_ResetPro();
        while(1u);
    }
    else if((ret == DEF_PARA_INIT_ERR) || ((ret == DEF_PARA_SUM_ERR)))
		{
        #if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: CPU Flash Common parameter reset!\r\n");
				#endif	
        
        if ((u8)DEF_TYPE_COMMON == BackupParaTypeGet())
        {
            if (   ((SYSCommon_Typedef *)FLASH_CPU_APPSIZE_START)->sum 
                == ParaChksumCalc((u8 *)(((u8 *)FLASH_CPU_APPSIZE_START) + 4u), sizeof(s_common) - 4u)
               )
            {
                if (1u == BackupparaLoad((u8)DEF_TYPE_COMMON, (void *)&s_common, sizeof(s_common)))
                {
                    if (DEF_PARA_NONE_ERR == HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common)))
                    {
                        retCode = 1u;
                        ucHit   = 0u;
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
        }
        else
        {
        }
        
        if (1u == ucHit)
        {
            if (   (*((u16 *)(ADDR_START_SYSCFG + 1024) + 0u) == DEF_DFT_COMMON_SAVED_MARK) 
                && (*((u16 *)(ADDR_START_SYSCFG + 1024) + 1u) == ParaChksumCalc((u8 *)(ADDR_START_SYSCFG + 1024 + 4u), sizeof(s_common) - 4u))
               )
            {
                memcpy((u8 *)&s_common, (u8 *)(ADDR_START_SYSCFG + 1024), sizeof(s_common));
                if (DEF_PARA_NONE_ERR == HAL_SaveParaPro (DEF_TYPE_COMMON, (void *)&s_common,	sizeof(s_common)))
                {
                    retCode = 1;
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
        
        if (0u == retCode)
        {
            HAL_ParaSetDefault(DEF_TYPE_COMMON);
            if (DEF_PARA_NONE_ERR == HAL_SaveParaPro (DEF_TYPE_COMMON, (void *)&s_common,	sizeof(s_common)))
            {
                retCode = 1;
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
    
    if ((DEF_PARA_NONE_ERR == ret) || (1u == retCode))
    {
        if (0xFF == BackupParaTypeGet())
        {
            BackupParaSave(DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));
        }
        else
        {
        }
        
        retCode = 1;
    }
     
		///////////////////////////
		ret =HAL_LoadParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg));
    
    if (DEF_PARA_BUSY_ERR == ret)
    {
        HAL_CPU_ResetPro();
        while(1u);
    }
    else if (DEF_PARA_NONE_ERR == ret)
    {
        if (1u == s_common.otapErrCode)
        {
            #if(DEF_MEMINFO_OUTPUTEN > 0)
            if(dbgInfoSwt & DBG_INFO_OTHER)
                myPrintf("[FLASH]: CPU Flash Syscfg parameter reset!\r\n");
            #endif
        
            HAL_ParaSetDefaultExt(DEF_TYPE_SYSCFG);		// ���ֲ�����λ(ֻ��λ�޸ĵĲ������֣�������������)
            if (DEF_PARA_NONE_ERR == HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)))
            {
                retCode = 1;
            }
            else
            {
            }
        }
        else
        {
            retCode = 1;
        }
    }
    else if((ret == DEF_PARA_INIT_ERR) || (ret == DEF_PARA_SUM_ERR))
		{
        #if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: CPU Flash Syscfg parameter reset!\r\n");
				#endif	
        
        if (1u == s_common.otapErrCode)
        {
            if (ret == DEF_PARA_SUM_ERR)
            {
            }
            else
            {
                HAL_ParaSetDefault(DEF_TYPE_SYSCFG);		// ������λ
            }
            
            HAL_ParaSetDefaultExt(DEF_TYPE_SYSCFG);		// ���ֲ�����λ(ֻ��λ�޸ĵĲ������֣�������������)
        }
        else
        {
            if ((u8)DEF_TYPE_SYSCFG == BackupParaTypeGet())
            {
                if (   ((SYSConfig_Typedef *)FLASH_CPU_APPSIZE_START)->sum 
                    == ParaChksumCalc((u8 *)(((u8 *)FLASH_CPU_APPSIZE_START) + 4u), sizeof(s_cfg) - 4u)
                   )
                {
                    if (1u == BackupparaLoad((u8)DEF_TYPE_SYSCFG, (void *)&s_cfg, sizeof(s_cfg)))
                    {
                    }
                    else
                    {
                        HAL_ParaSetDefault   (DEF_TYPE_SYSCFG);		// ������λ                   
                        HAL_ParaSetDefaultExt(DEF_TYPE_SYSCFG);		// ���ֲ�����λ(ֻ��λ�޸ĵĲ������֣�������������)
                    }
                }
                else
                {
                    HAL_ParaSetDefault   (DEF_TYPE_SYSCFG);		// ������λ                   
                    HAL_ParaSetDefaultExt(DEF_TYPE_SYSCFG);		// ���ֲ�����λ(ֻ��λ�޸ĵĲ������֣�������������)
                }
            }
            else
            {
                HAL_ParaSetDefault   (DEF_TYPE_SYSCFG);		// ������λ
                HAL_ParaSetDefaultExt(DEF_TYPE_SYSCFG);		// ���ֲ�����λ(ֻ��λ�޸ĵĲ������֣�������������)
            }
        }

        if (DEF_PARA_NONE_ERR == HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg)))
        {
            retCode = 1;
        }
        else
        {
        }
		}	
    else
    {
    }
    
		return retCode;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                     LOWPOWER FUNCTION
*********************************************************************************************************
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : HAL_PerpheralInPowerMode()
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
s8	HAL_PerpheralInPowerMode (void)
{	
		// �ٴ��ж�ON״̬�����ʱON��Чֱ���˳� 
		if(HAL_SYS_GetState(BIT_SYSTEM_IG)	== 1)
				return	-1;
	
		// ι��(����GD32��˵ι�깷����ֱ�ӵ��ý���STOPģʽ,����������RTC ALM���ٽ���STOP)
		// ʵ��֤�������и�λ��������ι���������������
		#if(OS_SYSWDG_EN > 0)	
		IWDG_ReloadCounter();		
		#endif 
	
		// GSMģ�����
		// �ڸú���֮ǰ������
	
		// �رչ���
		IO_SPAKER_PA_DIS();															

		// ��GPS��ʱ��λ״̬����˯��
		HAL_GpsModuleSleep();
		
		//	��CAN�շ�����˯��
		BSP_CanTransSleep(DEF_CAN_1);
		BSP_CanSleep(DEF_CAN_1);
		BSP_CanTransSleep(DEF_CAN_2);
		BSP_CanSleep(DEF_CAN_2);	
		
		// ��K�շ���������˯�� 
		BSP_K_Sleep();
		
		// ��ϵͳָʾ��
		IO_LED_RUN_OFF();
		IO_LED_DATA_OFF();

		// ʹ��IG,ADD,CON_RXD�ж�
		BSP_SYSEXIT_IT_CON (s_cfg.sleepSwt,	DEF_ENABLE);
		
		#if(DEF_EVENTINFO_OUTPUTEN > 0)
		if(dbgInfoSwt & DBG_INFO_EVENT)
				myPrintf("[LOWPOW]-> Success into sleep mode!\r\n");
		#endif
										
		// ��RS232���ڵ�Դ
		IO_RS232_POW_DIS();
		
		// ��ֹDBG���ڽ��ռĴ���˯���н����ж�
		BSP_RS232_RxIntDis(DEF_UART_4);
		// ��ֹCON���ڽ��ռĴ���˯���н����ж�
		//BSP_RS232_RxIntDis(DEF_UART_5);		// ��OBD���������ⲿ���Ʋ��ִ�����ڴ�		
		
		// �������ӻ���ʱ��
		BSP_RTCSetAlarmCounter(OS_RTC_ALM_SECS);

		// ����ALARM EXTI�ж�
		BSP_RTCEXIT_IT_CON(DEF_ENABLE);
		
		s_wakeUp.state =0;			//	���˯�߻���״̬����,���Ѻ��ɸñ����жϻ�������
		s_ACC.IntFlag	 =0;			//  2015-8-5 ����ٶ��жϱ�־
		s_ACC.IntFlag1 =0;
		s_ACC.IntFlag2 =0;
		
		
		// �����˯�ߴ������������ϵͳ����
		OS_SuspendAllTaskProcess();	

		// ����ֹͣģʽ,ֻ���ⲿ�жϿ��Ի���
   	PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

		return	0;
}
/*
******************************************************************************
* Function Name  : HAL_PerpheralOutPowerMode()
* Description    : 
* Input          : None
* Output         : None
* Return         : success 0��failed -1
******************************************************************************
*/
s8	HAL_PerpheralOutPowerMode (void)
{								
		#if(OS_SYSWDG_EN > 0)	
		IWDG_ReloadCounter();		// ι��	
		#endif
		
		// ����ϵͳʱ��
		BSP_SYSCLKConfig_FromStop();
	
		// �ָ�֮ǰ���������
		OS_ResumeAllTaskProcess();

		// ʹ��RS232���ڵ�Դ
		IO_RS232_POW_EN();
		// ʹ��DBG���ڽ��ռĴ���˯���н����ж�
		BSP_RS232_RxIntEn(DEF_UART_4);
		// ʹ��CON���ڽ��ռĴ���˯���н����ж�
		//BSP_RS232_RxIntEn(DEF_UART_5);	// ��OBD���������ⲿ���Ʋ��ִ�����ڴ�
		
		// ��ֹON,�ű�,BS_RXD�ж�
		BSP_SYSEXIT_IT_CON (s_cfg.sleepSwt,	DEF_DISABLE);

		// ��ֹALARM EXTI�ж�
		BSP_RTCEXIT_IT_CON(DEF_DISABLE);

		// ʹ��GPSģ�鹤��
		HAL_GpsModuleWork();

		// GSMģ�黽��
		BSP_GSMWakeUp(100);
		
		//	��CAN�շ���
		BSP_CanWork(DEF_CAN_1);
		BSP_CanTransWork(DEF_CAN_1);
		BSP_CanWork(DEF_CAN_2);
		BSP_CanTransWork(DEF_CAN_2);
		
		// ��K�շ���
		BSP_K_Work();		

		// ��������AD
		BSP_ADC_Init();
		
		#if(DEF_EVENTINFO_OUTPUTEN > 0)
		if(dbgInfoSwt & DBG_INFO_EVENT)
		{	
				if(s_wakeUp.state & DEF_WAKEUP_IG)
						myPrintf("[LOWPOW]-> Success out sleep mode!(Ig)\r\n");
				else if(s_wakeUp.state & DEF_WAKEUP_REMOVE)
						myPrintf("[LOWPOW]-> Success out sleep mode!(Remove)\r\n");
				else if(s_wakeUp.state & DEF_WAKEUP_KEY)
						myPrintf("[LOWPOW]-> Success out sleep mode!(Key)\r\n");
				else if(s_wakeUp.state & DEF_WAKEUP_UART1)
						myPrintf("[LOWPOW]-> Success out sleep mode!(Uart-Debug)\r\n");
				else if(s_wakeUp.state & DEF_WAKEUP_UART2)
						myPrintf("[LOWPOW]-> Success out sleep mode!(Uart-Control)\r\n");
				else if(s_wakeUp.state & DEF_WAKEUP_RING)
						myPrintf("[LOWPOW]-> Success out sleep mode!(Ring)\r\n");
				else if(s_wakeUp.state & DEF_WAKEUP_SHAKE)
						myPrintf("[LOWPOW]-> Success out sleep mode!(Shake)\r\n");
				else
						myPrintf("[LOWPOW]-> Success out sleep mode!\r\n");
		}
		#endif				
		
		return	0;
}
/*
*********************************************************************************************************
*                                              HAL_INIT_ALL()
*
* Description : Initialize the board's COMMON	GPIO (include led , keyboard and other con and det i/0).
*
* Argument(s) : 
*
* Return(s)   : none.
*
* Caller(s)   : OS.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	HAL_INIT_ALL (void)
{
		u8 i=0;
	
		//��ʼ��ʱ��Դ
		BSP_RCC_Configuration();
		
		//��ʼ���ж�
		BSP_NVIC_Configuration();
	
		//��ʼ����ͨ�������I/O
		BSP_GPIO_Init();

		//������ⲿ�жϳ�ʼ��
		BSP_SYSEXIT_Init();
	
		//��ʼ��ϵͳȫ��Ӳ��״̬����
		HardErrVarbInit();			// �����ڸ�����ΧӲ����ʼ��ǰ��ʼ��

		//AD��ʼ��
		BSP_ADC_Init();

		//PVD��ʼ��(��ʼ����ʹ��)
		BSP_PVD_Init(PWR_PVDLevel_2V9);	
		BSP_PVDEXIT_IT_CON(DEF_ENABLE);

		//���Զ˿ڳ�ʼ��
		HAL_DBG_Init();					// ���Դ��ڱ����ڵ���printf�����������Ϣǰ��ʼ��

		//�ⲿ���ƽӿڳ�ʼ��
		//HAL_CON_Init();				// ��CON�����г�ʼ��	

		//���Ź���ʼ��////////////////////////////////////////////////////////////////////////////////////////////////////
		#if(OS_SYSWDG_EN > 0)
		BSP_WDTDOG_Init(); 			// 12s��ʱ
		IWDG_Enable();
		IWDG_ReloadCounter();		// ι��(���Ź���ʼ����) 	
		#endif		
	
		//��ȡϵͳ��������	
		SoftDelayMS(300);							
		if(BSP_PVD_GetSta() == 1)
		{
				/*
				// PVD��Ч�򲻲���CPU ���ⲿFLASHֱ��ʹ��Ĭ�ϲ�������	
				HAL_ParaSetDefault(DEF_TYPE_COMMON);		
				HAL_ParaSetDefault(DEF_TYPE_SYSCFG);
				HAL_ParaSetDefault(DEF_TYPE_SYSOBD);
				globInitPVDFlag = OS_INITPVD_FLAG;			// ����OS��PVD�쳣�ж�
				*/
				#if(DFE_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
						myPrintf("[ERROR]: PVD power (<2.9V), CPU will reset now!\r\n");
				#endif	
				BSP_CPU_Reset();
		}		
		else
		{	
				// ���ǵ��ϵ�˲���豸��Դ���ȣ��п��ܵ���FLASH��ʼ��ʧ��
				for(i=0;	i<10; i++)
				{
						BSP_FLASH_Init();	 
						if((FLASH_DEVICE.InitSta == 0) || (FLASH_DEVICE.HardWareSta	!=	DEF_FLASHHARD_NONE_ERR))
								SoftDelayMS(300);
						else
								break;
				}
				if(HAL_SysParaLoadProcess() == 1)		// ��ȡϵͳ��������
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
								myPrintf("[FLASH]: CPU flash para read succees!\r\n");
						#endif	
				}
				else
				{	
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
								myPrintf("[FLASH]: CPU Flash para read/write error!\r\n");
						#endif	
				}
				HAL_ComQue_init(DEF_ALL_QUE);	// ��ʼ�����ݶ���
		}	
		
		//���ٶȴ��������Ի�
		//BSP_ACC_Init();		// ��DBG�����г�ʼ��	
		
		//GSMģ���ʼ��������
		//BSP_GSM_Init();		// ��GSM�����г�ʼ��

		//GPSģ���ʼ��������
		//BSP_GPS_Init();		// ��GPS�����г�ʼ��

		//RTC��ʼ��
				
		BSP_RTC_Init();
		//BSP_BACKUP_ReadDBGWrod(&dbgInfoSwt);		// ��ȡDBG��Ϣ������
		dbgInfoSwt =DBG_INFO_DFT;		// ������ȡֱ�����ó�Ĭ��ֵ
		

		//ι��
		#if(OS_SYSWDG_EN > 0)
		IWDG_ReloadCounter();		// ι��
		#endif

		// ��ʼ���ַ��� //////////////////////////////////////////////////
		#if(DEF_INITINFO_OUTPUTEN > 0)
		if(dbgInfoSwt & DBG_INFO_SYS)
		{
						myPrintf("------<SYSTEM START>-----\r\n");
				if(FLASH_DEVICE.InitSta == 1)
						myPrintf("[Init]: FLASH......(ok)!\r\n");
				else
						myPrintf("[Init]: FLASH......(error)!\r\n");
		}
		#endif	
}	
/*
*********************************************************************************************************
*********************************************************************************************************
*                                            MEMORY FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

