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
*                                     	hal_processSYS_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : hal_processSYS_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  HAL_PROCESSSYS_GLOBLAS

#include	<string.h>
#include	<stdio.h>
#include  <stdlib.h>
#include	"cfg_h.h"
#include	"bsp_h.h"
#include	"bsp_gps_h.h"
#include	"bsp_gsm_h.h"
#include	"bsp_rtc_h.h"
#include	"bsp_storage_h.h"
#include 	"ucos_ii.h"
#include	"main_h.h"
#include	"hal_h.h"
#include 	"hal_protocol_h.h"
#include	"hal_processSYS_h.h"
#include	"hal_processGPS_h.h"
#include 	"hal_processDBG_h.h"
#include 	"hal_processCON_h.h"
#include  "hal_processQUE_h.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
// ϵͳ��ѹ�����
#define	OS_POW_12V										(12)
#define	OS_POW_24V										(24)

// ��ѹ���ƶ���
#define	OS_POW_NONE										((u8)0)	// ��ѹ�ޱ仯
#define	OS_POW_UP											((u8)1)	// ��ѹ����������
#define	OS_POW_DOWN										((u8)2) // ��ѹ���½�����

// ���Ժ���12VΪ��׼У�鶨��
#define	OS_ADC_6V											(u16)(s_common.aadc * 0.5)	// �豸6V AD�����׼ֵ
#define	OS_ADC_12V										(u16)(s_common.aadc)	  		// �豸12V ��׼ֵ(ʵ�ʼ���õ�3.3V)
#define	OS_ADC_18V										(u16)(s_common.aadc * 1.5)	// �豸18V AD�����׼ֵ
#define	OS_ADC_24V										(u16)(s_common.aadc * 2)		// �豸24V AD�����׼ֵ��׼ֵ

/* ���Ժ���24VΪ��׼У�鶨��
#define	OS_ADC_6V											(u16)(s_common.aadc * 0.25)	// �豸6V AD�����׼ֵ
#define	OS_ADC_12V										(u16)(s_common.aadc * 0.5)	// �豸12V AD�����׼ֵ
#define	OS_ADC_18V										(u16)(s_common.aadc * 0.75)	// �豸18V AD�����׼ֵ
#define	OS_ADC_24V										(u16)(s_common.aadc)				// �豸24V ��׼ֵ(ʵ�ʼ���õ�3.3V)
*/

#define	OS_EVENT_REMOVE_PER						(0.5)						// �豸�Ƴ���ѹ������׼
#define	OS_EVENT_NOCHECK_PER					(0.75)					// �豸�����I/O�¼�������׼
#define	OS_EVENT_LOWPOW_PER						(0.875)					// �豸�ص������׼

// ���Ϩ���ж�
#define	OS_ADC12_VOLDOWN							((u16)50)				// Ϩ���Դѹ����ֵ(����100��)
#define	OS_ADC24_VOLDOWN							((u16)100)			// Ϩ���Դѹ����ֵ(����100��)
#define	OS_ADC12_VOLUP								((u16)50)				// Ϩ���Դѹ����ֵ(����100��)
#define	OS_ADC24_VOLUP								((u16)100)			// Ϩ���Դѹ����ֵ(����100��)

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

#define	DEF_GET_INTERVAL_TIME					((u32)600)			// ���Ƶ�ѹ�ɼ����
typedef struct	tag_VolClc_Def
{	
			vu32	tpCounter;		// ��ѹ�ɼ���ʱ��
			u8		tpCount;			// ��ѹ�ɼ���
			u16		tp[5];				// ��ѹ�ɼ�����
			
			u16		valTmp;				// 
			u8		valCount;			// ��ֵ�ж��ۼ�
			u16		val[3];				// 
	
			vu8		igIntFlag;		// IG�жϱ�־				
			vu8		igOnStep;			// IG����жϽ׶α���
			vu32	igOffUpCounter;		// Ϩ����̵�ѹ���߼�ʱ��
			vu32	igOffDownCounter;	// Ϩ����̵�ѹ���ͼ�ʱ��
			vu8		igOffStep;		// IGϨ���жϽ׶α���(1��ʼϨ���ж�)
			vu16	startVol;			// ���ڼ�¼��ʼ��һ�㴥��ѹ������ʼ��ѹ
			
}VolClc_Typedef;


#define	DEF_TJ_INTERVAL_TIME					((u32)5*60000)	// ���������5����
typedef struct	tag_TjCheck_Def
{	
			vu8		firstTjFlag;			//  �״�����־(�����һ�κ���1)										
			vu8		tjStep;						//	�豸�����Զ�����־����
			vu32	tjDelayCounter;		//	�����ʱ��ʱ��
			vu32	tjGvlCounter;			//	���ʱ����˼�ʱ��			
			vu8		tjToStep;					//  ��쳬ʱ�׶α���
			vu32	tjToCounter;			//  ��쳬ʱ��ʱ��
			vu32	tjSoundToCounter;	//	���������ʱ��ʱ��

}TjCheck_Typedef;



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
static			vu8										adPoint=0;													// ADѭ���ɼ�����
static			vu32									adcCvCounter=0,adcPrnCounter=0;			// AD�ɼ�����

static			vu8										powType=0;													// ��Դϵͳ�жϱ���
static			vu16									powType12Times=0,powType24Times=0;	// ��ϵ��Դϵͳȷ����������

static			vu32									pnIgTimerCounter=0;									// �ϵ粹��ON�¼���ʱ��ʱ��
static			vu32									igTimerCounter;

static			u8										lowpowonTimes=0,lowpowoffTimes=0;
static			vu32									lowpowTimerCounter;

static			vu32									naviCounter=0;											//	IPʧЧ������ʱ��ʱ��

static			vu8										logSendedFlag=0;										//  �Ѿ��ϴ���LOG��Ϣ��־
static			vu8										needSendFlag=0;											//  ��Ҫ����LOG��Ϣ��־
static			vu32									logUpDelayCounter=0;								//  ����LOG�ϴ�ʱ������ʱ��

static			VolClc_Typedef				s_vol;															//  ��ѹ�ɼ��������
static			TjCheck_Typedef				s_tj;																//  ����⴦�����

static			vu8										firstIgOnFlag=0;										//  �豸������־
static			vu32									firstPowOnCounter=0;								//  �豸��һ���ϵ��־

static			vu16									u16TmpPowerSystem=OS_ADC12_VOLDOWN;	//  ��ѹ�����жϻ�׼(Ĭ��12Vϵͳ)

static			vu8										glob_igOffRalStep=0;								//  ��ʵϨ��״̬�׶α���
static			vu32									obdOff30secCounter=0;								//  OBDϨ����30�붨ʱ��
static			vu32									obdOff3minCounter=0;								//  OBDϨ����3���Ӷ�ʱ��
static			vu32									obgOffTrasnToCounter=0;							//  OBD��ȡ����״̬ͨ�ų�ʱ��ʱ��

static			vu32									accCheckValCounter=0;								//  ACC����OBD��ȡRPM�ȴ���ʱ��
static			vu32									accOn10secCounter=0;								//  ACCϨ����10�붨ʱ��
static			vu32									accOn2minCounter=0;									//  ACCϨ����2���Ӷ�ʱ��
static			vu32									accOnTrasnToCounter=0;							//  ACC����OBD��ȡ����״̬ͨ�ų�ʱ��ʱ��

static			vu8										igOffMoveStep=0;										//  Ϩ���ƶ��׶α���
static			vu32									igOffMoveCounter=0;									//  Ϩ���ƶ���ʱ��
static			vu32									moveCheckCounter=0;									//  Ϩ���ƶ�GPS�ٶ�ȷ�϶�ʱ��

static			vu32									accShakeCounter=0;									//	���ٶ��񶯱�����ʾ��ʱ��
static			vu8										fuelUpRpm=0,fuelCunRpm=0;						//  �ͺ�ͳ���ϴ�RPM״̬

static			vu8										wakeUpIsrFlag=0;										//  �������жϱ�־
static			vu32									igOffSetTime=0;											//  Ϩ��ƫ����ʱ�����

static			vu8										sysPowOnStep=0;											//	�豸�ϵ��ʶ���ͽ׶α���
static			vu32									powOnCounter=0;											//	�豸�ϵ��ʶ���͵ȴ���ʱ��

static      u8                    S_ucLastPowerRemoveSt = DEF_SET;    //�豸��һ���ϵ�Ĭ��ģ������Դ�ϴ�״̬Ϊδ��

static      u32                   S_uiIgoffAccTmr = 0u;
static      vu8                   S_uiIgoffAccFlag = 0u;  


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static	u8 checkTrendPro	(u16 val);	// ��ѹ�����ж�
static void IgonAccJudgeIgoff(void);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     IG CHECK TOOL
*********************************************************************************************************
*/
/*
******************************************************************************
* Function Name  : checkTrendPro()
* Description    : ����ǰ������3��Ա��ֵ���ж�(����ĵ�һ�������ĵ�����ѹ��)���½�0.5V���ƻ�����������
* Input          : None
* Output         : None
* Return         : OS_POW_NONE-��Ч��OS_POW_UP-��������(�����ﵽ0.1V����)��OS_POW_DOWN-�½�����(�½��ﵽ0.5V����)
******************************************************************************
*/
static	u8 checkTrendPro(u16 val)
{
    if (s_vol.valCount < 3)
        s_vol.val[s_vol.valCount++] = val;
    else
    {
        s_vol.val[0] = s_vol.val[1];
        s_vol.val[1] = s_vol.val[2];
        s_vol.val[2] = val;
    }
    if (s_vol.valCount < 3)
        return OS_POW_NONE; 
		else
		{
				if(powType == OS_POW_12V)
				{
						if((s_vol.val[0]<s_vol.val[2]) && (s_vol.val[2] - s_vol.val[0] >= OS_ADC12_VOLUP)) 
								return OS_POW_UP;
						if((s_vol.val[2]<s_vol.val[0]) && (s_vol.val[0] - s_vol.val[2] >= OS_ADC12_VOLDOWN)) 
								return OS_POW_DOWN;
				}							
				if(powType == OS_POW_24V)
				{
						if((s_vol.val[0]<s_vol.val[2]) && (s_vol.val[2] - s_vol.val[0] >= OS_ADC24_VOLUP)) 
								return OS_POW_UP;
						if((s_vol.val[2]<s_vol.val[0]) && (s_vol.val[0] - s_vol.val[2] >= OS_ADC24_VOLDOWN)) 
								return OS_POW_DOWN;
				}
		}
		return OS_POW_NONE;
}
/*
*********************************************************************************************************
*                                     API
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : HAL_IG_IRQHandlers()
* Description    : IG�жϷ�����
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void 	HAL_IG_IRQHandlers	(void)
{
		/*
		if(s_vol.igIntFlag == 0)
				s_vol.igIntFlag =1;
		*/
		if(wakeUpIsrFlag == 0)
				wakeUpIsrFlag =1;			// 2015-8-19 ������ֻ��Ϊ��������
}


/*
******************************************************************************
* Function Name  : HAL_SYS_SetState()
* Description    : 
* Input          : staBit ϵͳ״̬λ����(BIT_SYSTEM_POW)
* Output         : None
* Return         : value == DEF_ENABLE or DEF_DISABLE
******************************************************************************
*/
void	HAL_SYS_SetState (u32 staBit,	u8 newSta)
{
		if(newSta == DEF_ENABLE)
				s_sys.sysState	|= staBit;
		else
				s_sys.sysState	&= ~staBit;
}
/*
******************************************************************************
* Function Name  : HAL_SYS_GetState()
* Description    : 
* Input          : staBit ϵͳ״̬λ����(BIT_SYSTEM_POW)
* Output         : None
* Return         : value = DEF_SET or DEF_CLR
******************************************************************************
*/
u8	HAL_SYS_GetState (u32 staBit)
{
		if(s_sys.sysState & staBit)
				return	DEF_SET;
		else
				return	DEF_CLR;
}
/*
******************************************************************************
* Function Name  : HAL_SYSALM_SetState()
* Description    : 
* Input          : staBit ϵͳ����״̬λ����(BIT_ALARM_SOS)
* Output         : None
* Return         : value == DEF_ENABLE or DEF_DISABLE
******************************************************************************
*/
void	HAL_SYSALM_SetState (u32 almBit,	u8 newSta)
{
		if(newSta == DEF_ENABLE)
				s_sys.alarmState	|= almBit;
		else
				s_sys.alarmState	&= ~almBit;
}
/*
******************************************************************************
* Function Name  : HAL_SYSALM_GetState()
* Description    : 
* Input          : staBit ϵͳ����״̬λ����(BIT_ALARM_SOS)
* Output         : None
* Return         : value = DEF_SET or DEF_CLR
******************************************************************************
*/
u8	HAL_SYSALM_GetState (u32 almBit)
{
		if(s_sys.alarmState & almBit)
				return	DEF_SET;
		else
				return	DEF_CLR;
}
/*
******************************************************************************
* Function Name  : HAL_BSP_SetState()
* Description    : 
* Input          : ENUM_BSP_STA �������Ա����
*                : newSta = DEF_ENABLE or DEF_DISABLE
* Output         : None
* Return         : None
******************************************************************************
*/
void	HAL_BSP_SetState (ENUM_BSP_STA type,	u8 newSta)
{	
		switch(type)
		{		
				//case	DEF_BSPSTA_ON					:	// Ŀǰ�������ÿ���
				//			break;
				default: 
							break;
		}				
}
/*
******************************************************************************
* Function Name  : HAL_BSP_GetState()
* Description    : 
* Input          : ENUM_BSP_STA �������Ա����
*                : newSta = DEF_ENABLE or DEF_DISABLE
* Output         : None
* Return         : 0 or 1 or other velue
******************************************************************************
*/
u32  HAL_BSP_GetState  (ENUM_BSP_STA type)
{
    u32	retValue=0;
		switch(type)
		{		
				case	DEF_BSPSTA_ON					:	// ON�����״̬��Y06ϵ�в�Ʒ�޸��ź��ߣ�
							retValue = 0;
							break;
			
				case	DEF_BSPSTA_IG					:	// IG�����״̬
							if(IO_WAKEUP_DET() == DEF_WAKEUP_YES)
									retValue = 1;
							else
									retValue = 0;
							break;
							
				case	DEF_BSPSTA_GPSSTA			:	// GPS�Ƿ�λ��Ч'A'or'V'
							if(s_gps.sta == 'A')
									retValue = 1;
							else
									retValue = 0;
							break;
							
				case	DEF_BSPSTA_GPS3D			:	// GPS�Ƿ�Ϊ3D��λ
							if(s_gps.fs == '3')
									retValue = 1;
							else
									retValue = 0;
							break;
				
				case	DEF_BSPSTA_GPSSPEED		:	// GPS�ٶȻ�ȡ
							if(s_gps.sta == 'A')
									retValue = s_TmpGps.cunSpeed;
							else
									retValue = 0;
							break;
							
				default: 
							break;
		}		
		return	retValue;
}

/*
******************************************************************************
* Function Name  : HAL_IsPwrRemoveSendOk
* Description    : �жϲ�������Ƿ������
* Input          : NONE
* Output         : None
* Return         : 1��������ϣ� 0��δ����
******************************************************************************
*/
u8 HAL_IsPwrRemoveSendOk(void)
{
    return ((HAL_ComQue_size (DEF_RAM_QUE) == 0) 	&& (s_ram.len == 0)  && (HAL_ComQue_size (DEF_FLASH_QUE) == 0) && (s_flash.len == 0));	
}

/*
******************************************************************************
* Function Name  : HAL_GetLastPowerRemoveSt
* Description    : ��ȡ�ϴβ������״̬,
* Input          : NONE
* Output         : None
* Return         : 1������� 0������
******************************************************************************
*/
u8 HAL_GetLastPowerRemoveSt(void)
{
    CPU_SR cpu_sr = 0u;
    u8     ucRet  = 0u;
  
    OS_ENTER_CRITICAL();
    ucRet = S_ucLastPowerRemoveSt;
    OS_EXIT_CRITICAL();
  
    return ucRet;
}

/*
******************************************************************************
* Function Name  : HAL_GetLastPowerRemoveSt
* Description    : ��ȡ�ϴβ������״̬,
* Input          : NONE
* Output         : None
* Return         : 1������� 0������
******************************************************************************
*/
void HAL_SetLastPowerRemoveSt(u8 ucSt)
{
    CPU_SR cpu_sr;
  
    OS_ENTER_CRITICAL();
    S_ucLastPowerRemoveSt = ucSt;
    OS_EXIT_CRITICAL();
}

/*
******************************************************************************
* Function Name  : ModuleRemoveChk
* Description    : ���������,
* Input          : usVal����ѹֵ
*                  usPeriod��������������
* Output         : None
* Return         : 
******************************************************************************
*/	
static void PowerRemoveChk(u16 usVal, u16 usPeriod)
{
    static u16 s_usRemoveCnt = 0u;
    static u16 s_usResumeCnt = 0u;
  
    if (usVal < 250u)
    {
        s_usResumeCnt = 0u;
      
        if (HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE) == 0u)
        {
            s_usRemoveCnt ++;
          
            if ((s_usRemoveCnt * usPeriod) >= 10000u)
            {
                if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 1)
                {
                    HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_DISABLE);								
                    HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_DISABLE); 
                  
                    #if(DEF_EVENTINFO_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_EVENT)
                    {
                        myPrintf("[EVENT]: IG-OFF!\r\n");
                    }
                    #endif
                    HAL_IGInQueueProcess(TYPE_IG_OFFNEW,	0u);	// ƫ����ʱ����ƫ��ʱ�����ȷ��
                }
                else
                {
                }
                
                #if(DEF_EVENTINFO_OUTPUTEN > 0)
                if(dbgInfoSwt & DBG_INFO_EVENT)
                {
                    myPrintf("[EVENT_POWER]:OBD Power Off!!!\r\n");
                }
                #endif
                
                HAL_SetLastPowerRemoveSt(HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE));
                HAL_SYSALM_SetState(BIT_ALARM_POWREMOVE, DEF_ENABLE);
                BKP_WriteBackupRegister(BKP_PWRMOVE_FLAG, 0x0000u);
                
                if (strlen((const char *)&s_cfg.ip2[0u]) > 0u)
                {
                    HAL_AlarmInQueueProcess(ALARM_EXT_BIT_REMOVE);
                }
                else
                {
                    s_cfg.devEn &= ~DEF_DEVEN_BIT_GPRS;
                    (void)HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg));
                }
            }
            else
            {
            }
        }
        else
        {
            if (1u == HAL_SYS_GetState(BIT_SYSTEM_IG))
            {
                if (s_cfg.devEn & DEF_DEVEN_BIT_GPRS)
                {
                }
                else
                {
                    s_cfg.devEn |= DEF_DEVEN_BIT_GPRS;
                    (void)HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg));
                }
            }
            else
            {
                if (strlen((const char *)&s_cfg.ip2[0u]) > 0u)
                {
                }
                else
                {
                    if (s_cfg.devEn & DEF_DEVEN_BIT_GPRS)
                    {
                        s_cfg.devEn &= ~DEF_DEVEN_BIT_GPRS;
                        (void)HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg));
                    }
                    else
                    {
                    }
                }                  
            }
        }
    }
    else
    {
        s_usRemoveCnt = 0u;
        
        if (HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE) == 1u)
        {
            s_usResumeCnt ++;
          
            if ((s_usResumeCnt * usPeriod) >= 3000u)
            {
                HAL_SetLastPowerRemoveSt(HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE));
                HAL_SYSALM_SetState(BIT_ALARM_POWREMOVE, DEF_DISABLE);
                BKP_WriteBackupRegister(BKP_PWRMOVE_FLAG, BKP_SAVE_FLAG_PWRON);
              
                if (s_cfg.devEn & DEF_DEVEN_BIT_GPRS)
                {
                }
                else
                {
                    s_cfg.devEn |= DEF_DEVEN_BIT_GPRS;
                    (void)HAL_SaveParaPro (DEF_TYPE_SYSCFG,	(void *)&s_cfg,	sizeof(s_cfg));
                }
              
                if ((1u == sysPowOnStep) || (2u == sysPowOnStep))
                {
                }
                else
                {
                    sysPowOnStep = 4u;
                }
                
                #if(DEF_EVENTINFO_OUTPUTEN > 0)
                if(dbgInfoSwt & DBG_INFO_EVENT)
                {
                    myPrintf("[EVENT_POWER]:OBD Power On!!!\r\n");
                }
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
}

/*
******************************************************************************
* Function Name  : HAL_EventAlarmProcess
* Description    : �¼���ϵͳ�����߼�����,
*                  ����ϵͳ״̬����:s_sys.sysState  �� s_sys.alarmState
* Input          : s_Tmp.alarmState �� s_Cfg.alarmState������ͬ
* Output         : None
* Return         : 
******************************************************************************
*/			
void	HAL_EventAlarmProcess (u32 eventSwt)
{		
		s8	tmpIgSta=0;
		u8 	tmpV=0;
		u16	u16TmpADC=0;
		u32	counter=0;
		SYS_DATETIME	tmpRtc;
  
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// AD�ɼ�����/////////////////////////////////////////////////////////////////////////////////////
		// ������ʱ�䴥��
		//////////////////////////////////////////////////////////////////////////////////////////////////
		if(OSTmr_Count_Get(&adcCvCounter) >= 50)
		{
				if(adPoint < DEF_ADC_MAXNUM)	// 100ms����һ��
				{								
						// AD����
						s_adInfo.ADCVBat[adPoint]  = BSP_ADC_GetValue(DEF_ADC_VBAT);	  // �豸�ⲿ��Դ��ѹ����
						adPoint++;
				}	
				if(adPoint >= DEF_ADC_MAXNUM)
				{
						adPoint=0;
						// �����ⲿ��ƿ��ѹ /////////////////////////////////////////
						u16TmpADC = ADCalculateProcess((u16 *)&(s_adInfo.ADCVBat[0]),	DEF_ADC_MAXNUM); 
						s_adInfo.ADCVBatBK = u16TmpADC; 
																				
						if((s_common.aadc <= 1500) || (s_common.aadc >= 3200))		// ����豸����δУ����ʹ��Ĭ��ֵ����
								s_common.aadc =	DEF_DFT_COMMON_AADC;
						
						s_adInfo.VBatVol	=	(u16)((12 * u16TmpADC * 100)/s_common.aadc + 0.5); 
            
            PowerRemoveChk(s_adInfo.VBatVol, (50u * DEF_ADC_MAXNUM));
						
						// ��ӡ���
						if(OSTmr_Count_Get(&adcPrnCounter) >= 500)
						{
								#if(DEF_ADCINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_ADC)
										myPrintf("\r\n[ADC]: VBat=%4d/%2dV",	u16TmpADC,s_adInfo.VBatVol);
								#endif
								OSTmr_Count_Start(&adcPrnCounter);
						}
				}													
				OSTmr_Count_Start(&adcCvCounter);
		}
	
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// �ж�������ѹ����(12V or 24V)///////////////////////////////////////////////////////////////////
		// ��������ѹ����18Vʱ��Ϊ24V��ϵ����18Vʱ��Ϊ12V��ϵ
		//////////////////////////////////////////////////////////////////////////////////////////////////
		if((s_common.aadc != 0) && (s_adInfo.ADCVBatBK != 0))
		{
				if(s_adInfo.ADCVBatBK >= OS_ADC_18V)
				{
						powType12Times=0;
						powType24Times++;
						if(powType24Times >= 200)  // 50ms * n
						{
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
								{
										if((powType == 0) || (powType == OS_POW_12V))
												myPrintf("[EVENT]: 24V power system!\r\n");
								}
								#endif
								powType24Times =0;
								powType = OS_POW_24V;										
								u16TmpPowerSystem =OS_ADC24_VOLDOWN;	// �޸ĵ�ѹ�����жϱ�׼
								
						}
				}
				else if((s_adInfo.ADCVBatBK > OS_ADC_6V) && (s_adInfo.ADCVBatBK < OS_ADC_18V))
				{
						powType24Times =0;
						powType12Times++;
						if(powType12Times >= 200)  // 50ms * n
						{
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
								{
										if((powType == 0) || (powType == OS_POW_24V))
												myPrintf("[EVENT]: 12V power system!\r\n");
								}
								#endif
								powType12Times =0;
								powType = OS_POW_12V;							
								u16TmpPowerSystem =OS_ADC12_VOLDOWN;	// �޸ĵ�ѹ�����жϱ�׼
						}
				}
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// ϵͳ״̬����////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////
  
		// ��Դ״̬����/////////////////////////
    HAL_SYS_SetState(BIT_SYSTEM_POW,	DEF_ENABLE);
		
    // ��ӵ�Դ���͸���////////////////////
    if(powType == OS_POW_24V)
        HAL_SYS_SetState(BIT_SYSTEM_POW24,	DEF_ENABLE);
    else
        HAL_SYS_SetState(BIT_SYSTEM_POW24,	DEF_DISABLE);
		
		/////////////////////////////////////////////////////////////////////////////////////////////////
		// ON �¼�///////////////////////////////////////////////////////////////////////////////////////
		// �����������崥�����(�����𳬹�Ϩ���ж�ʱ����ٲ���������Ҳ����Ϊ�ٴε��)����ѹ���ʹ���Ϩ��
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// ��һ���ϵ紦��////////////////////////////
		if(firstIgOnFlag == 0)
		{
				firstIgOnFlag =1;	// ��ֹ�ٴν���
			
				// ��𲹱�����///////////////////
				tmpIgSta =BSP_BACKUP_LoadIGOnSta();
				if(tmpIgSta == 1)
				{
						if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 0) 
						{
								//////////////////////////////////////////
								OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);			// ���¿�ʼ���GPS��Ϣ�ϴ�ʱ��
								OSTmr_Count_Start(&s_osTmp.igOnChiXuCounter);		// ������ʱ���ʱ����ʼ
								
								HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_ENABLE);
								HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_ENABLE);               
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
								{
										//myPrintf("[EVENT]: ON-ON!\r\n");
										myPrintf("[EVENT]: IG-ON-Keep!\r\n");
								}
								#endif
								
								// ��Ϩ�����־ ///////////////////
								s_vol.igOnStep  =0;
								s_vol.igIntFlag =0;			// ������崥������		
								s_vol.startVol  =0;			// ����ʼ��ѹ���ֵ
								s_vol.valCount	=0;			// ���¿�ʼ�ɼ�3���ѹֵ
								s_vol.igOffStep =1;			// ��ʼϨ����		

								// ����OBDϨ�����߼�//////////////
								glob_igOffRalStep =0;		// ��׶α�����ʼϨ����
								OSTmr_Count_Start(&obdOff30secCounter);		// 30s��ʱ��ʼ
								HAL_SYS_SetState(BIT_SYSTEM_POWDOWN,	DEF_DISABLE);	// ���ѹ����״̬
						}
				}
		}	
		
		// �����˴���//////////////////////////////
		if(s_vol.igIntFlag == 1)
		{
				OSTimeDly(1000);	// ������ʱ�����ݷ���ʱ���Ϸֿ�(ģ�����ĵ������Ӱ�����ݷ���,����ģ����ϸ�λ)
			
				/*
				if((HAL_SYS_GetState(BIT_SYSTEM_IG) == 0) ||
					 ((HAL_SYS_GetState(BIT_SYSTEM_IG) == 1) && (OSTmr_Count_Get(&s_osTmp.igOnChiXuCounter) >= (s_cfg.igOffDelay * 1000))))
				*/
				if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 0)
				{	
						// �ͺĽṹ������ʼ�� 2015-6-19 �ͺ���Ŀ���
						/*
						fuelUpRpm =0xAA;	// �ó�ʼ״̬
						memset((u8 *)&s_fuel,	0, sizeof(s_fuel));
						HAL_SendOBDProProcess(CMD_READ_FUEL,	0,	0,	(u8 *)"",	0);			// ֪ͨOBD��ȡ��ǰ����			
						*/
					
						// ��쿪ʼ��������(�Ȳ�����������������ݱ������������뷢�����ݳ�ͻ)/////////////////
						HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_ENABLE);				// ϵͳ�����״̬��
						HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_ENABLE);	// ���ò��Ż�δ����������״̬
						if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
								HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound1,	strlen((const char *)s_cfg.sound1));		// ������쿪ʼ����											
						
						/*
						// Ϩ�𲹱� //////////////////////
						if((HAL_SYS_GetState(BIT_SYSTEM_IG) == 1) && (OSTmr_Count_Get(&s_osTmp.igOnChiXuCounter) >= (s_cfg.igOffDelay * 1000)))
						{
								// �Ѿ�����Ϩ����ʱ(�е���������Ϊ��Ч(��ֹϨ���߼��ж�ʧ�ܵ���һֱ��Ϩ��))
								RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);	 // Ϩ�������ݼ�ʱ�����¿���					
								HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_DISABLE);								
								HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_DISABLE); 
							
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
								{
										//myPrintf("[EVENT]: ON-OFF!\r\n");
										myPrintf("[EVENT]: IG-OFF-BUBAO!\r\n");
								}
								#endif
								// Ϩ��״̬��ȷ�� /////////////////
								HAL_IGInQueueProcess(TYPE_IG_OFFNEW,0);		// 2015-4-15 ������Ϩ��ƫ����Ϊ0
								// ֪ͨobd�����豸Ϩ�� ////////////
								HAL_SendOBDProProcess(CMD_IG_OFF,	0,	0,	(u8 *)"",	0);								
								// ����Ϩ���� ///////////////////
								s_vol.igOffStep =0;
						}
						*/
						// ��ʼ����� ///////////////////
						s_vol.igOnStep =1;
				}						
				else
						s_vol.igIntFlag =0;	// ��������������˵����ε��
		}
		// ����߼�����//////////////////////////////
		if(s_vol.igOnStep == 1)
		{
				//////////////////////////////////////////
				OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);			// ���¿�ʼ���GPS��Ϣ�ϴ�ʱ��
				OSTmr_Count_Start(&s_osTmp.igOnChiXuCounter);		// ������ʱ���ʱ����ʼ
				
				HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_ENABLE);
				HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_ENABLE);               
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
				{
						OSTimeDly(500);	// ����1s��ʱ�������ϵͳ�մ��ж��л��Ѵ���ʱ��δ�ȶ����´�����Ϣ��ӡ��ȫ
						//myPrintf("[EVENT]: ON-ON!\r\n");
						myPrintf("[EVENT]: IG-ON!\r\n");
				}
				#endif
				
				// ���״̬��ȷ�� //////////////////
				HAL_IGInQueueProcess(TYPE_IG_ON,0);		// 2015-4-15 ���û��ʱ��ƫ����
				// ֪ͨobd�����豸���//////////////
				HAL_SendOBDProProcess(CMD_IG_ON,	0,	0,	(u8 *)"",	0);	
				// ֪ͨ�Զ�����⿪ʼ	////////////
				s_tj.tjStep	=1;	
				
				// ��Ϩ�����־ ///////////////////
				s_vol.igOnStep  =0;
				s_vol.igIntFlag =0;			// ������崥������		
				s_vol.startVol  =0;			// ����ʼ��ѹ���ֵ
				s_vol.valCount	=0;			// ���¿�ʼ�ɼ�3���ѹֵ
				s_vol.igOffStep =1;			// ��ʼϨ����
				
				// ����OBDϨ�����߼�//////////////
				glob_igOffRalStep =0;		// ��׶α�����ʼϨ����
				OSTmr_Count_Start(&obdOff30secCounter);		// 30s��ʱ��ʼ
				HAL_SYS_SetState(BIT_SYSTEM_POWDOWN,	DEF_DISABLE);	// ���ѹ����״̬
		}
		
		// ��ѹ�ɼ�����(Ϩ���߼�����)///////////////////////////////////////////////////
		if(OSTmr_Count_Get(&s_vol.tpCounter) >= DEF_GET_INTERVAL_TIME)	// 600ms�ɼ�һ��
		{
				OSTmr_Count_Start(&s_vol.tpCounter);	// ���¼�ʱ			
				s_vol.tp[s_vol.tpCount++]	=s_adInfo.VBatVol;	// ��ȡ��ǰ��ѹ			
				if(s_vol.tpCount >= 5)
				{
						s_vol.tpCount =0;	// ���¿�ʼ�ɼ�
						s_vol.valTmp =ADCalculateProcess(s_vol.tp,	5); 	// ȡƽ��ֵ
					
						// ��ѹ�����ж�				
						if(s_vol.igOffStep >= 1)
								tmpV =checkTrendPro(s_vol.valTmp);	// ֻ�е���ſ�ʼ���µ�ѹ����	
											
						if(s_vol.igOffStep == 1)
						{	
								// �����ѹ����
								if(tmpV == OS_POW_UP)
								{										
										s_vol.igOffStep =2;
										s_vol.startVol =s_vol.val[0];								// ��¼��һ�㴥��ѹ������ʼ��ѹ
										OSTmr_Count_Start(&s_vol.igOffUpCounter);	// ��ʼ��ʱ��
								}
								else if(tmpV == OS_POW_DOWN)
								{						
										s_vol.igOffStep =4;	
										s_vol.startVol =s_vol.val[0];								// ��¼��һ�㴥��ѹ������ʼ��ѹ
										OSTmr_Count_Start(&s_vol.igOffDownCounter);	// ��ʼϨ��״̬ȷ�ϼ�ʱ
								}
								else
								{
										;	// ��ѹ��������(-0.5V < Vol < +0.5V)���账��
								}
						}
						else if(s_vol.igOffStep == 2)
						{
								// ��ѹ���߽׶�1
								if((s_vol.startVol <= s_vol.valTmp) && ((s_vol.valTmp - s_vol.startVol) >= u16TmpPowerSystem))
								{
										if(OSTmr_Count_Get(&s_vol.igOffUpCounter) >= 20000)
												s_vol.igOffStep =1;											// ���������жϽ׶�
								}
								else
										s_vol.igOffStep =1;											// ���������жϽ׶�
						}
						else if(s_vol.igOffStep == 3)
						{
								// ��ѹ���߽׶�2
								if((s_vol.startVol <= s_vol.valTmp) && ((s_vol.valTmp - s_vol.startVol) >= u16TmpPowerSystem))
								{
										if(OSTmr_Count_Get(&s_vol.igOffUpCounter) >= 20000)
												s_vol.igOffStep =1;	// ��ѹ���߳���20s�������²ɼ���ʼ��ѹ
								}
								else
										s_vol.igOffStep = 4;		// ����Ϩ���ж�
						}
						else if(s_vol.igOffStep == 4)
						{							
								if((s_vol.startVol <= s_vol.valTmp) && ((s_vol.valTmp - s_vol.startVol) >= u16TmpPowerSystem))
								{
										// ��ѹ���ߴ���
										s_vol.igOffStep = 3;						// ��ѹ������һ�����ֵ�ѹ������20���ѹ���˽׶�3
										OSTmr_Count_Start(&s_vol.igOffUpCounter);	
								}	
								else
								{
										// ��ѹ�����׶�										
										if((s_vol.startVol > s_vol.valTmp) && ((s_vol.startVol - s_vol.valTmp) >= u16TmpPowerSystem))
										{												
												if(OSTmr_Count_Get(&s_vol.igOffDownCounter) >= (s_cfg.igOffDelay * 1000))		
												{
														// Ϩ���¼�����
														/*
														RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);	 // Ϩ�������ݼ�ʱ�����¿���	
														HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_DISABLE);								
														HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_DISABLE); 
													
														#if(DEF_EVENTINFO_OUTPUTEN > 0)
														if(dbgInfoSwt & DBG_INFO_EVENT)
														{
																//myPrintf("[EVENT]: ON-OFF!\r\n");
																myPrintf("[EVENT]: IG-OFF!\r\n");
														}
														#endif
														// Ϩ��״̬��ȷ�� /////////////////
														HAL_IGInQueueProcess(TYPE_IG_OFFNEW,	(s_cfg.igOffDelay + 180));	// �ɵ�ѹ�����жϳ���Ϩ��ƫ����ΪϨ���ӳ�+OBD�ж��ӳ�	
														// ֪ͨobd�����豸Ϩ�� ////////////
														HAL_SendOBDProProcess(CMD_IG_OFF,	0,	0,	(u8 *)"",	0);
														OSTimeDly(200);	// ����OBD����ʱ��
														// ֪ͨobd�����豸������˯�� //////
														// OBD���ֻ�û��
														//HAL_SendOBDProProcess(CMD_SYS_SLEEP,	0,	0,	(u8 *)"",	0); 
														OSTimeDly(200);	// ����OBD����ʱ��
														// ֪ͨobd��ʼ��ȡ����״̬/////////	
														HAL_SendOBDProProcess(CMD_READ_STA,	0,	0,	(u8 *)"",	0);
														s_osTmp.obdReadCarStaFlag =1;	// ���ö�ȡ����״̬�����б�־
														OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);	// ��ʼ��ȡ����״̬��ʱͳ��
														if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
																HAL_GPSInQueueProcess();											// ����һ��GPSλ����Ϣ
														*/

														HAL_SYS_SetState(BIT_SYSTEM_POWDOWN,	DEF_ENABLE);	//���õ�ѹ����״̬
																
														// ����Ϩ���� ///////////////////
														s_vol.igOffStep =0;		
														
												}
										}
										else
										{
												;	// ��������е�ѹ�ٴ����������20s��ֹ��ѹ��⻷�ں����ȷ����ʼֵ
										}
								}
						}
						else
								s_vol.igOffStep =0;	// �����Ƿ���λ
				}
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//������촦��///////////////////////////////////////////////////////////////////////
		// �������豸�Զ�������������
		//////////////////////////////////////////////////////////////////////////////////////////////////	
		if(OSTmr_Count_Get(&firstPowOnCounter) >= 3000)
		{
				if(s_common.otapErrCode != 0)
				{
						if(s_common.otapErrCode == 1)
						{
								HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_ENABLE);				// ϵͳ�����״̬��
								HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_ENABLE);	// ���ò��Ż�δ����������״̬
								if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
										HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound1,	strlen((const char *)s_cfg.sound1));		// ������쿪ʼ����	
								
								HAL_SendOBDProProcess(CMD_AUTO_TJ,	0,	0,	(u8 *)"",	0);	// ֪ͨobd����ִ���Զ����
						}
            HAL_CommonParaReload();
						s_common.otapErrCode =0;	// ����ϱ�־
						HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));				
				}			
		}		
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//ϵͳ�Զ�����ӳٴ������///////////////////////////////////////////////////////////////////////
		// ��������������ʱ�󴥷����
		//////////////////////////////////////////////////////////////////////////////////////////////////				
		if(s_tj.tjStep == 1)
		{
				s_tj.tjStep =2;
				OSTmr_Count_Start(&s_tj.tjDelayCounter);		// ����ӳټ�ʱ��ʼ		
		}
		else if(s_tj.tjStep == 2)
		{
				if(OSTmr_Count_Get(&s_tj.tjDelayCounter) >= (s_cfg.tjDelayTime * 1000))
				{
						s_tj.tjStep =3;
						HAL_SendOBDProProcess(CMD_AUTO_TJ,	0,	0,	(u8 *)"",	0);	// ֪ͨobd����ִ���Զ����							
				}	
		}
		
		// ��쳬ʱ�ж�
		if(s_tj.tjToStep == 0)
		{
				if(HAL_SYS_GetState (BIT_SYSTEM_TJSTA) == 1)
				{
						s_tj.tjToStep =1;					
						OSTmr_Count_Start(&s_tj.tjSoundToCounter); 	// ��ʼ����������ʱ��ʱ	
						OSTmr_Count_Start(&s_tj.tjToCounter); 			// ��ʼ��ʱ��ʱ		
				}
		}
		else if(s_tj.tjToStep == 1)
		{
				// ���������ʱ����
				if(HAL_SYS_GetState (BIT_SYSTEM_TJSOUND_END) == 1)
				{
						if(OSTmr_Count_Get(&s_tj.tjSoundToCounter) >= OS_TJSOUND_MAX_TIMEOUT)
						{
								HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_DISABLE);	// ����������״̬���
								if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
										HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound2,	strlen((const char *)s_cfg.sound2));		// ��������������
						}
				}
				// ���״̬��ʱ����
				if(HAL_SYS_GetState (BIT_SYSTEM_TJSTA) == 1)
				{										
						if(OSTmr_Count_Get(&s_tj.tjToCounter) >= OS_TJ_MAX_TIMEOUT)
						{
								s_tj.tjToStep =0;
								HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_DISABLE);	// ϵͳ�����״̬�����	
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[EVENT]: TiJian %ds timeout!\r\n",	OS_TJ_MAX_TIMEOUT/1000);
								#endif
						}						
				}
				else
						s_tj.tjToStep =0;	// ��־��λ���¼�ʱ		
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//ȡ������״̬��������///////////////////////////////////////////////////////////////////////
		// �������������ʱ�����ϴ����ʱ������5���ӣ��Ҿ�����������ʱ�󴥷�
		//////////////////////////////////////////////////////////////////////////////////////////////////				
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// GPS���¶�λ ���� //////////////////////////////////////////////////////////////////////////////	
		// �������ϴ�δ��λ���ζ�λʱ����
		//////////////////////////////////////////////////////////////////////////////////////////////////
		if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) != (u8)(s_event.gpsFixSta.up))
		{
				if(s_event.gpsFixSta.up == FALSE)
				{
						if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
						{		
								// GPS���¶�λ
								s_event.gpsFixSta.up = TRUE;								
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[EVENT]: GPS-REFIX!\r\n");
								#endif								
						} 		
				}
				else if(s_event.gpsFixSta.up == TRUE)
				{		
						if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0)
						{
								s_event.gpsFixSta.up	= FALSE;
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[EVENT]: GPS-NOT-FIX!\r\n");
								#endif	
						}
				}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////
		// ������ѹ״̬ ////////////////////////////////////////////////////////////////////////////////// 
		// ��������ѹ����ϵͳ����ֵ
		//////////////////////////////////////////////////////////////////////////////////////////////////
		if(powType != 0)
		{
				if(((powType == OS_POW_12V) && (s_adInfo.VBatVol > (s_cfg.lowPower)))  ||
					 ((powType == OS_POW_24V) && (s_adInfo.VBatVol > (s_cfg.lowPower*2))))
				{		
						lowpowoffTimes = 0;
						// �豸������ѹ�ж� //////////////
						if(OSTmr_Count_Get(&lowpowTimerCounter) >= OS_LOWPOW_CHECK_TIME)
						{
								lowpowonTimes++;
								OSTmr_Count_Start(&lowpowTimerCounter);			
						}		
						if(lowpowonTimes >= OS_LOWPOWIN_CHECK_TIMES)
						{
								lowpowonTimes 	= 0;
								lowpowoffTimes 	= 0;
								if(s_event.lowPow.up == TRUE)
								{
									 	s_event.lowPow.up	= FALSE;
                    HAL_SYSALM_SetState(BIT_ALARM_POWLOW,	DEF_DISABLE);
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[EVENT]: POW-NORMAL!\r\n");
										#endif
								}	
						}	
				}
			  else if(((powType == OS_POW_12V) && (s_adInfo.ADCVBatBK >= (OS_ADC_12V * OS_EVENT_REMOVE_PER)) && (s_adInfo.VBatVol <= (s_cfg.lowPower))) ||
					 			((powType == OS_POW_24V) && (s_adInfo.ADCVBatBK >= (OS_ADC_24V * OS_EVENT_REMOVE_PER)) && (s_adInfo.VBatVol <= (s_cfg.lowPower*2))))
				{
						lowpowonTimes = 0;
						// �豸��ѹ�ж� ///////////////						
						if(OSTmr_Count_Get(&lowpowTimerCounter) >= OS_LOWPOW_CHECK_TIME)
						{
								lowpowoffTimes++;
								OSTmr_Count_Start(&lowpowTimerCounter);			
						}		
						if(lowpowoffTimes >= OS_LOWPOWOFF_CHECK_TIMES)
						{
								lowpowonTimes 	= 0;
								lowpowoffTimes 	= 0;
								if(s_event.lowPow.up == FALSE)
								{
										s_event.lowPow.up	= TRUE;
                    HAL_SYSALM_SetState(BIT_ALARM_POWLOW,	DEF_ENABLE);
										//HAL_AlarmInQueueProcess(ALARM_BIT_POWER);	// �����ѹ�������ݰ� 2015-9-30 ȡ����ѹ�������ݰ�
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[ALARM]: POW-LOW!\r\n");
										#endif
								}
						}			
				}
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////
		// �豸Ϩ���ۺ��ж� ///////////////////////////////////////////////////////////////////////
		// ��������ǰΪ���״̬����ѹ����11V���豸ֹͣ״̬�����ѹѹ������0.5V��ѭ�����3min
		///////////////////////////////////////////////////////////////////////////////////////////
		if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 1)
		{
				// �жϿ�ʼ
				if(glob_igOffRalStep == 0)
				{
						if((HAL_SYSALM_GetState(BIT_ALARM_POWLOW) == 1) ||
							 ((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0)) ||
							 (HAL_SYS_GetState(BIT_SYSTEM_POWDOWN) == 1))
							 //((*(s_devSta.pMems) != DEF_MEMSHARD_NONE_ERR) && (HAL_SYS_GetState(BIT_SYSTEM_POWDOWN) == 1)))
						{
								if(OSTmr_Count_Get(&obdOff30secCounter) >= 30000)// OBDÿ�μ��ʱ��Ӧ����30s
								{
										OSTmr_Count_Start(&obdOff3minCounter);	// ��ʼ3���Ӽ�ʱ
										glob_igOffRalStep =1;	// ��ʼ����ͨ��
									
										// 2015-8-19 ���ô��������������޸�Ϩ��ƫ����ƫ������һ������
										if(HAL_SYSALM_GetState(BIT_ALARM_POWLOW) == 1)		
												igOffSetTime =180;												// �ɼӵ�ѹ����״̬������Ϩ��ƫ����Ϊ��ֹ�ӳ�180s	
										else if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0))
												igOffSetTime =180;												// �ɼ��ٶ��жϳ�����ֹ״̬������Ϩ��ƫ����Ϊ��ֹ�ӳ�180s	
										else if(HAL_SYS_GetState(BIT_SYSTEM_POWDOWN) == 1)
												igOffSetTime =(s_cfg.igOffDelay + 180);		// �ɵ�ѹ�����жϳ���Ϩ��ƫ����ΪϨ���ӳ�+OBD�ж��ӳ�	
										else
												igOffSetTime =180;												// ����Ĭ��180s	
									
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
										{
												if(HAL_SYSALM_GetState(BIT_ALARM_POWLOW) == 1)
														myPrintf("[EVENT]: Start OBD check mainloop->Powlow!\r\n");
												else if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0))
														myPrintf("[EVENT]: Start OBD check mainloop->StopState!\r\n");
												else if(HAL_SYS_GetState(BIT_SYSTEM_POWDOWN) == 1)
														myPrintf("[EVENT]: Start OBD check mainloop->0.5V!\r\n");
										}
										#endif
								}
						}
				}
				// 30��ѭ���жϽ׶�
				else if((glob_igOffRalStep >= 1) && (glob_igOffRalStep <= 3))
				{
						// 30sѭ����ʼ�׶�
						if(glob_igOffRalStep == 1)
						{
								if(OSTmr_Count_Get(&obdOff30secCounter) >= 30000)
								{
										glob_igSta	=DEF_IG_RPM_START;	// �ÿ�ʼ����־
										glob_igOffRalStep =2;
								}
						}
						// ��������OBD��ȡ����״̬�׶�
						else if(glob_igOffRalStep == 2)
						{
								// ֪ͨobd����ʼ��⳵���Ƿ�Ϩ�� ////////////
								if(HAL_SendOBDProProcess(CMD_GET_CARDATA,	0,	0,	(u8 *)"",	0) == 0)
								{
										glob_igOffRalStep =3;	// �ȴ�OBD�ظ��׶�
										OSTmr_Count_Start(&obgOffTrasnToCounter);	// ͨ�ų�ʱͳ�ƿ�ʼ						
								}
								else
								{
										glob_igOffRalStep =1;	// �����ж�״̬
										OSTmr_Count_Start(&obdOff30secCounter);		// 30s��ʱ��ʼ
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
												myPrintf("[EVENT]: OBD datalock=1 check next 30s!\r\n");
										#endif
								}
						}
						// �ȴ�OBD�ظ��׶�
						else if(glob_igOffRalStep == 3)
						{
								if((glob_igSta == DEF_IG_RPM_0) || (glob_igSta == DEF_IG_ECU_TIMEOUT))
								{
										glob_igOffRalStep =1;		// ������س���״̬
										OSTmr_Count_Start(&obdOff30secCounter);		// 30s��ʱ��ʼ
									
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
										{
												if(glob_igSta == DEF_IG_RPM_0)
														myPrintf("[EVENT]: OBD return RPM<%d!\r\n", glob_igSta);
												else if(glob_igSta == DEF_IG_ECU_TIMEOUT)
														myPrintf("[EVENT]: OBD return ECU timeout!\r\n");
										}
										#endif
								}
                else if(glob_igSta >= DEF_IG_RPM_100)
								{
                    if ((DEF_IG_RPM_100 != glob_igSta) && ((0u == glob_igStaLast) || (glob_igStaLast == glob_igSta)))
                    {
                        glob_igOffRalStep =1;		// ������س���״̬
                        OSTmr_Count_Start(&obdOff30secCounter);		// 30s��ʱ��ʼ
                    }
                    else
                    {
                        glob_igOffRalStep =0;		// ������س���״̬		
                        OSTmr_Count_Start(&obdOff30secCounter);		// 30s��ʱ��ʼ
                        if(HAL_SYS_GetState(BIT_SYSTEM_POWDOWN) == 1)
                        {							
                            // ����ɵ�ѹ���µ�Ϩ���ж�����
                            s_vol.igOnStep  =0;
                            s_vol.igIntFlag =0;			// ������崥������		
                            s_vol.startVol  =0;			// ����ʼ��ѹ���ֵ
                            s_vol.valCount	=0;			// ���¿�ʼ�ɼ�3���ѹֵ
                            s_vol.igOffStep =1;			// ��ʼϨ����
                            HAL_SYS_SetState(BIT_SYSTEM_POWDOWN,	DEF_DISABLE);	// ���ѹ����״̬		
                        }		
                    }
                    
                    #if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_OTHER)
                        myPrintf("[EVENT]: OBD return RPM>=%d!\r\n", glob_igSta);
                    #endif
                    
                    glob_igStaLast = glob_igSta;
								}
								else
								{
										if(OSTmr_Count_Get(&obgOffTrasnToCounter) >=  60000)
										{
												glob_igOffRalStep =1;		// ������س���״̬
												OSTmr_Count_Start(&obdOff30secCounter);		// 30s��ʱ��ʼ
												#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_OTHER)
														myPrintf("[EVENT]: OBD return 60s timeout!\r\n");
												#endif
										}					
								}
						}
						// 3�����жϳ�ʱ�жϴ���
						if(OSTmr_Count_Get(&obdOff3minCounter) >=  180000)
						{
								glob_igOffRalStep =4;	// ����Ϩ���߼�����		
								#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_OTHER)
										myPrintf("[EVENT]: OBD Check 3min timeout!\r\n");
								#endif
						}
				}
				// Ϩ���¼�ȷ���������
				else if(glob_igOffRalStep == 4)
				{
						glob_igOffRalStep =5;		// �豸����˯�ߵȴ�״̬
					
						// Ϩ���¼�����
						RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);	 // Ϩ�������ݼ�ʱ�����¿���	
						HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_DISABLE);								
						HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_DISABLE); 
					
						#if(DEF_EVENTINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_EVENT)
						{
								//myPrintf("[EVENT]: ON-OFF!\r\n");
								myPrintf("[EVENT]: IG-OFF!\r\n");
						}
						#endif
						// Ϩ��״̬��ȷ��(ͨ�����������ж�ƫ����) /////////////////
						HAL_IGInQueueProcess(TYPE_IG_OFFNEW,	igOffSetTime);	// ƫ����ʱ����ƫ��ʱ�����ȷ��
						
						// ֪ͨobd�����豸Ϩ�� ////////////
						//HAL_SendOBDProProcess(CMD_IG_OFF,	0,	0,	(u8 *)"",	0);
						//OSTimeDly(200);	// ����OBD����ʱ��
						// ֪ͨobd�����豸������˯�� //////
						//HAL_SendOBDProProcess(CMD_SYS_SLEEP,	0,	0,	(u8 *)"",	0); 
						//OSTimeDly(200);	// ����OBD����ʱ��
						// ֪ͨobd��ʼ��ȡ����״̬/////////	
						if(HAL_SendOBDProProcess(CMD_READ_ALARMSTA,	0,	0,	(u8 *)"",	0) == 0)
								s_osTmp.obdReadCarStaFlag =1;	// ���ö�ȡ����״̬�����б�־
						else
								s_osTmp.obdReadCarStaFlag =0;
						OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);	// ��ʼ��ȡ����״̬��ʱͳ��
						if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
								HAL_GPSInQueueProcess();											// ����һ��GPSλ����Ϣ		
						
						// ֪ͨACC�������±궨�豸�Ƕ�
						if(glob_accAdjustFlag != 1)
								glob_accAdjustFlag =1;	// ��ʼ�궨
				}
				// ���ߵȴ��׶�
				else if(glob_igOffRalStep == 5)
				{
						;		// �ȴ�����˯��
				}
				else
						glob_igOffRalStep =0;	// �Ƿ�������λ
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////
		// ͨ�����ٶ��ж��ֲ�Ϩ�������߼� /////////////////////////////////////////////////////////
		// ��������ǰΪϨ��״̬���Ҽ��ٶ��жϲ�����ѭ�����2min
		///////////////////////////////////////////////////////////////////////////////////////////
		if(wakeUpIsrFlag == 1)
		{			
				if(s_ACC.IntFlag == 0)
				{
						s_ACC.IntFlag =1;		// 2015-8-19 ģ���𶯲����������߼��ж�
						wakeUpIsrFlag =0;		
				}
				else
				{
						;	// �豸���ڵ��������еȴ��������ٴδ������
				}
		}
		if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 0)
		{
				// �жϿ�ʼ
				if(glob_accOnRalStep == 0)
				{
						if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (s_ACC.IntFlag == 1))
						{
								OSTmr_Count_Start(&accOn2minCounter);	// ��ʼ2���Ӽ�ʱ
								glob_accOnRalStep =1;	// ��ʼ����ͨ��
								OSTmr_Count_Start(&accCheckValCounter);	// ��ʼ����ӳٶ�ʱ��
							
								#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_OTHER)
										myPrintf("[EVENT]: (ACC)Start OBD check mainloop!\r\n");
								#endif
						}
				}
				// ����ӳٽ׶�
				else if(glob_accOnRalStep == 1)
				{
						if(OSTmr_Count_Get(&accCheckValCounter) >= 1500)// 1.5s��ʼ���
						{
								glob_igSta	=DEF_IG_RPM_START;	// �ü�⿪ʼ
								glob_accOnRalStep =3;	// ��һ�ν���ѭ������10s��ʱ
						}
				}
				// 10sѭ���׶�
				else if((glob_accOnRalStep >= 2) && (glob_accOnRalStep <= 4))
				{
						// 10sѭ����ʼ�׶�
						if(glob_accOnRalStep == 2)
						{
								if(OSTmr_Count_Get(&accOn10secCounter) >= 10000)
								{
										glob_igSta	=DEF_IG_RPM_START;	// �ü�⿪ʼ
										glob_accOnRalStep =3;
								}
						}
						// ��������OBD��ȡ����״̬�׶�
						else if(glob_accOnRalStep == 3)
						{
								// ֪ͨobd����ʼ��⳵���Ƿ�Ϩ�� ////////////
								if(HAL_SendOBDProProcess(CMD_GET_CARDATA,	0,	0,	(u8 *)"",	0) == 0)
								{
										glob_accOnRalStep =4;	// �ȴ�OBD�ظ��׶�
										OSTmr_Count_Start(&accOnTrasnToCounter);	// ͨ�ų�ʱͳ�ƿ�ʼ						
								}
								else
								{
										glob_accOnRalStep =2;	// �����ж�״̬
										OSTmr_Count_Start(&accOn10secCounter);		// 10s��ʱ��ʼ
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
												myPrintf("[EVENT]: (ACC)OBD datalock=1 check next 10s!\r\n");
										#endif
								}
						}
						// �ȴ�OBD�ظ��׶�
						else if(glob_accOnRalStep == 4)
						{
								if(glob_igSta == DEF_IG_RPM_0)
								{
										glob_accOnRalStep =2;		// 2015-11-4 RPM��ȡ�����������RPM<100��������ֱ����ʱ�˳�
										OSTmr_Count_Start(&accOn10secCounter);	// ��ʼ����ӳټ�ʱ		
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
												myPrintf("[EVENT]: (ACC)OBD return RPM<%d!\r\n", glob_igSta);
										#endif
								}									
								else if(glob_igSta == DEF_IG_ECU_TIMEOUT)
								{
										glob_accOnRalStep =2;		// ������س���״̬
										OSTmr_Count_Start(&accOn10secCounter);		// 10s��ʱ��ʼ								
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
												myPrintf("[EVENT]: (ACC)OBD return ECU timeout!\r\n");
										#endif
								}
                else if (glob_igSta >= DEF_IG_RPM_100)
                {
                    if ((DEF_IG_RPM_100 != glob_igSta) && ((0u == glob_igStaLast) || (glob_igSta == glob_igStaLast)))
                    {
                        glob_accOnRalStep =2;		// ������س���״̬
                        OSTmr_Count_Start(&accOn10secCounter);		// 10s��ʱ��ʼ		
                    }
                    else
                    {
                        s_vol.igIntFlag = 1;		// ģ�⳵�������嶯��,ǿ��ִ�е���߼�!!!!
                        s_ACC.IntFlag =0;				// ���жϱ�־			
                        glob_accOnRalStep =0;		// �˳����μ��		
                        OSTmr_Count_Start(&accOn10secCounter);	// ��ʼ����ӳټ�ʱ	
                    }
                    
                    #if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_OTHER)
                        myPrintf("[EVENT]: (ACC)OBD return RPM>=%d!\r\n", glob_igSta);
                    #endif
                    glob_igStaLast =  glob_igSta;
                }
								else
								{
										if(OSTmr_Count_Get(&accOnTrasnToCounter) >=  60000)
										{
												glob_accOnRalStep =2;		// ������س���״̬
												OSTmr_Count_Start(&accOn10secCounter);		// 10s��ʱ��ʼ
												#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_OTHER)
														myPrintf("[EVENT]: (ACC)OBD return 60s timeout!\r\n");
												#endif
										}					
								}
						}
						// 3�����жϳ�ʱ�жϴ���
						if(OSTmr_Count_Get(&accOn2minCounter) >=  180000)
						{
								s_ACC.IntFlag =0;				// ���жϱ�־			
								glob_accOnRalStep =0;		// �˳����μ��		
								OSTmr_Count_Start(&accOn10secCounter);	// ��ʼ����ӳټ�ʱ		
								#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_OTHER)
										myPrintf("[EVENT]: (ACC)OBD Check 3min timeout!\r\n");
								#endif
						}
				}
				else
						glob_accOnRalStep =0;	// �Ƿ�������λ				
		}
		else
		{
				s_ACC.IntFlag =0;				// ���жϱ�־��ֹ�豸�ӵ��ת��Ϩ��ʱ��֮ǰ�������е��жϱ�־��¼����
				glob_accOnRalStep = 0;	// �׶θ�λ��ֹ�����м������̺���ֱ���˳�֮���޷��ٴν�������
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// ����Ϩ���ƶ�״̬ ///////////////////////////////////////////////////////////////////////////
		// ������GPS��λ�ҵ�ǰΪϨ��״̬
		//////////////////////////////////////////////////////////////////////////////////////////////
		if((HAL_SYS_GetState(BIT_SYSTEM_IG) == 0) && (HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE) == 0u))
		{
				if(igOffMoveStep == 0)
				{
						if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (s_ACC.IntFlag1 == 1))
						{
								igOffMoveStep =1;
								OSTmr_Count_Start(&igOffMoveCounter);	// ��ʼ�ƶ���ⶨʱ��
						}			
				}
				// �ƶ�״̬�����׶�
				else if(igOffMoveStep == 1)
				{
						if(OSTmr_Count_Get(&igOffMoveCounter) < (s_cfg.sysMoveT*1000))
						{
								if((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1) && (HAL_BSP_GetState(DEF_BSPSTA_GPSSPEED) >= s_cfg.sysMove))
								{								
										igOffMoveStep =2;
										OSTmr_Count_Start(&moveCheckCounter);	// ��ʼ��ʱ��										
								}
						}
						else
						{
								s_ACC.IntFlag1 =0;	// ���жϱ�־
								igOffMoveStep =0;	// �˳����μ��
						}							
				}
				// �ƶ�ȷ���׶�(���ǵ�GPSƯ��)
				else if(igOffMoveStep == 2)
				{
						if((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1) && (HAL_BSP_GetState(DEF_BSPSTA_GPSSPEED) >= s_cfg.sysMove))
						{
								if(OSTmr_Count_Get(&moveCheckCounter) >= 60000)	// 2015-9-28 �ƶ���������ʱ���޸�Ϊ60s
								{
//										igOffMoveStep = 3;
//										glob_igStaExt =DEF_IG_RPM_START;	// ��ʼ��⳵��RPM
//										if(HAL_SendOBDProProcess(CMD_GET_CARDATA,	0,	0,	(u8 *)"",	0) == 0)
//												OSTmr_Count_Start(&moveCheckCounter);	// ��ʼ��ʱ��	
//										else
//												igOffMoveStep =4;	// ͨ�Ŷ�������RPM�жϹ���
										igOffMoveStep =4;	// 2015-8-5 ֱ������Ϩ���ж�
								}
						}
						else 
								igOffMoveStep =1;	// GPS����λ���ٶȵ�����������״̬ȷ�Ͻ׶�
				}
				// ����Ϩ��״̬�ٴ�ȷ��
				else if(igOffMoveStep == 3)
				{
						if(glob_igStaExt == DEF_IG_RPM_100)
						{
								s_ACC.IntFlag1 =0;	// ���жϱ�־
								igOffMoveStep =0;	// �˳����μ��
						}
						else if((glob_igStaExt == DEF_IG_RPM_0) || (glob_igStaExt == DEF_IG_ECU_TIMEOUT))
								igOffMoveStep =4;	// Ϩ���ƶ�״̬ȷ��						
						else
						{
								if(OSTmr_Count_Get(&moveCheckCounter) >= 60000)
										igOffMoveStep =4;	// ͨ�ų�ʱ��Ϊ����Ϩ��
						}
				}
				// Ϩ���ƶ�״̬ȷ��
				else if(igOffMoveStep == 4)
				{
						if(glob_accOnRalStep == 0)
						{
								igOffMoveStep =5;
								HAL_SYSALM_SetState(BIT_ALARM_ILLMOVE,	DEF_ENABLE);	// ��Ϩ���ƶ�����״̬
								HAL_AlarmInQueueProcess	(ALARM_BIT_MOVE);	// ���Ϩ���ƶ���������
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[ALARM]: IG-OFF-Move Enter!\r\n");
								#endif			
						}
						else
						{
								igOffMoveStep =7;
								OSTmr_Count_Start(&moveCheckCounter);	// ��ʼ��ʱ��	
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[ALARM]: IG-OFF-Move Wait!\r\n");
								#endif							
						}
				}
				// ״̬�˳����
				else if(igOffMoveStep == 5)
				{
						if(((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0) && (HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0))	||
							 ((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1) && (HAL_BSP_GetState(DEF_BSPSTA_GPSSPEED) < s_cfg.sysMove)))
						{
								igOffMoveStep =6;
								OSTmr_Count_Start(&moveCheckCounter);	// ��ʼ��ʱ��								
						}
				}
				// ��ֹ״̬ȷ���׶�
				else if(igOffMoveStep == 6)
				{
						if(((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0) && (HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0))	||
							 ((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1) && (HAL_BSP_GetState(DEF_BSPSTA_GPSSPEED) < s_cfg.sysMove)))
						{
								if(OSTmr_Count_Get(&moveCheckCounter) >= 10000)
								{
										s_ACC.IntFlag1 =0;	// ���жϱ�־
										igOffMoveStep = 0;	// �˳�ѭ��
										HAL_SYSALM_SetState(BIT_ALARM_ILLMOVE,	DEF_DISABLE);	// ��Ϩ���ƶ�����״̬
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[ALARM]: IG-OFF-Move Exit!\r\n");
										#endif
								}						
						}
						else
								OSTmr_Count_Start(&moveCheckCounter);	// ���¼�ʱ
				}
				// Ϩ���ƶ����Ĵ���ȴ��׶�
				else if(igOffMoveStep == 7)
				{
						if(glob_accOnRalStep == 0)
						{
								igOffMoveStep =5;
								HAL_SYSALM_SetState(BIT_ALARM_ILLMOVE,	DEF_ENABLE);	// ��Ϩ���ƶ�����״̬
								HAL_AlarmInQueueProcess	(ALARM_BIT_MOVE);	// ���Ϩ���ƶ���������
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[ALARM]: IG-OFF-Move Enter!\r\n");
								#endif			
						}		
						else
						{
								if(OSTmr_Count_Get(&moveCheckCounter) >= 300000)	// 5���ӳ�ʱ
								{
										s_ACC.IntFlag1 =0;	// ���жϱ�־
										igOffMoveStep =0;	
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[ALARM]: IG-OFF-Move break!\r\n");
										#endif			
								}
						}
				}
				else
				{
						s_ACC.IntFlag1 =0;	// ���жϱ�־
						igOffMoveStep =0;	
				}
		}	
		else
		{
				s_ACC.IntFlag1 =0;	// ���жϱ�־
				if(igOffMoveStep != 0)
				{					
						HAL_SYSALM_SetState(BIT_ALARM_ILLMOVE,	DEF_DISABLE);	// ��Ϩ���ƶ�����״̬
						if(igOffMoveStep >= 5)
						{
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[ALARM]: IG-OFF-Move Exit!\r\n");
								#endif
						}
						igOffMoveStep =0;
				}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////
		// �񶯱��� ///////////////////////////////////////////////////////////////////////////////////	
		// ������Ϩ�����м��ٶ��񶯲���
		///////////////////////////////////////////////////////////////////////////////////////////////
		if((HAL_SYS_GetState(BIT_SYSTEM_IG) == 0) && (HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE) == 0u))
		{
				if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (s_ACC.IntFlag2 == 1))
				{
						s_ACC.IntFlag2 =0;	// �����־��ֹ�ٴν���
						if(s_cfg.devEn & DEF_DEVEN_BIT_SHAKE)
						{
								if(OSTmr_Count_Get(&accShakeCounter) >= 60000)
								{
										OSTmr_Count_Start(&accShakeCounter);	// ���¼�ʱ
										HAL_AlarmInQueueProcess(ALARM_BIT_SHAKE);
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[ALARM]: ACC-SHAKE!\r\n");
										#endif
								}
						}				
				}	
		}

		///////////////////////////////////////////////////////////////////////////////////////////////
		// ��������״̬ ///////////////////////////////////////////////////////////////////////////////	
		// ������
		///////////////////////////////////////////////////////////////////////////////////////////////
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// �����Ƿ�����״̬(�����ṩ) /////////////////////////////////////////////////////////////////
		// ������
		///////////////////////////////////////////////////////////////////////////////////////////////
		if(glob_illOpenFlag)
		{
				glob_illOpenFlag =0;
				if(s_cfg.devEn & DEF_DEVEN_BIT_CON)
						HAL_AlarmInQueueProcess(ALARM_EXT_BIT_ILLOPEN);		// �¶�Ƿ����ű���
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// OTAP �������淢�� ///////////////////////////////////////////////////////////////////////////	
		// ���������������0ʱ����,��������������һ���ֶ����
		///////////////////////////////////////////////////////////////////////////////////////////////
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// ����LOG�ϴ� ///////////////////////////////////////////////////////////////////////////////	
		// ������GPS,GSM,FLASH,ACC�ȹ���״̬�ϴ�(�ϴ�����������)
		///////////////////////////////////////////////////////////////////////////////////////////////
		if((s_err.flashTimes)		 	|| (s_err.gsmSimTimes) 		|| (s_err.gsmResetTimes) || (s_err.gpsResetTimes) || 
			 (s_err.obdBusRWTimes) 	|| (s_err.obdParaRWTimes) || (s_err.accTimes) 		 || (s_err.cpuFlashTimes) ||
		   (s_err.conTimes))
		{			
				if(logSendedFlag == 0)
				{
						logSendedFlag =1;	// �豸��δ���͹�LOG��Ϣ
						needSendFlag 	=1;							
				}
				else if(logSendedFlag == 1)
				{
						if(OSTmr_Count_Get(&logUpDelayCounter) >= OS_LOGUP_INTERVAL_TIME)
								needSendFlag =1;
				}
				else
						logSendedFlag =0;
				
				// �������LOG��Ϣ
				if(needSendFlag  == 1)
				{
						needSendFlag =0;
						if((s_err.flashTimes) && (FLASH_DEVICE.InitSta == 1))  
								HAL_LogInQueueProcess	(TYPE_LOG_FLASH);
						else if(s_err.gsmSimTimes)
								HAL_LogInQueueProcess	(TYPE_LOG_SIM);
						else if(s_err.gsmResetTimes)
								HAL_LogInQueueProcess	(TYPE_LOG_GSM);
						else if(s_err.gpsResetTimes)
								HAL_LogInQueueProcess	(TYPE_LOG_GPS);
						else if(s_err.obdBusRWTimes)
								HAL_LogInQueueProcess	(TYPE_LOG_OBDBUS);
						else if(s_err.obdParaRWTimes)
								HAL_LogInQueueProcess	(TYPE_LOG_OBDFLAG);
						else if((s_err.accTimes) && (MEMS_DEVICE.InitSta != 0))  
								HAL_LogInQueueProcess	(TYPE_LOG_ACC);
						else if(s_err.cpuFlashTimes)
								HAL_LogInQueueProcess	(TYPE_LOG_CPUFLASH);
						else if((s_err.conTimes) && (CON_DEVICE.InitSta == 1))  
								HAL_LogInQueueProcess	(TYPE_LOG_CON);
						
						OSTmr_Count_Start(&logUpDelayCounter);	// ���¿�ʼ�����ʱ
				}
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// �豸�ϵ��� ///////////////////////////////////////////////////////////////////////////////	
		// �������豸�ϵ�����RTCΪĬ��ʱ������Ϊ�豸���԰γ�����У׼RTCʱ������豸�ϵ籨��
		///////////////////////////////////////////////////////////////////////////////////////////////
		if(sysPowOnStep == 0)
		{
				BSP_RTC_Get_Current(&tmpRtc);	
				if((tmpRtc.year == Default_year) && (tmpRtc.month == Default_month))
						sysPowOnStep =1;
				else
						sysPowOnStep =3;	// �˳��߼�
		}
		else if(sysPowOnStep == 1)
		{
				BSP_RTC_Get_Current(&tmpRtc);	
				if(tmpRtc.year >= 2015)
				{
						sysPowOnStep =2;
						OSTmr_Count_Start(&powOnCounter);
				}
		}
		else if(sysPowOnStep == 2)
		{
				if(OSTmr_Count_Get(&powOnCounter) >= 100)
				{			
            if (HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE) == 0u)
            {
                sysPowOnStep =3;
                OSTmr_Count_Start(&powOnCounter);	// ��ȡ��ǰƫ����
                BSP_RTC_Get_Current(&tmpRtc);	
                counter =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);
                if(counter > powOnCounter/1000)								
                    HAL_PowOnInQueueProcess (0x00,	powOnCounter,	(counter-powOnCounter/1000));
                else
                    HAL_PowOnInQueueProcess (0x00,	powOnCounter,	0);
            }
            else
            {
            }
				}
		}
		else if(sysPowOnStep == 3)
		{
				;
		}
    else if(sysPowOnStep == 4)
		{
        if (HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE) == 0u)
        {
            BSP_RTC_Get_Current(&tmpRtc);	
            counter =Mktime(tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second);		
            if(tmpRtc.year >= 2015)
            {
                HAL_PowOnInQueueProcess (0x00u,	0u,	counter);
                sysPowOnStep = 3u;
            }
            else
            {
            }
        }
        else
        {
            sysPowOnStep = 3u;
        }
		}
		else
				sysPowOnStep =0;
    
		///////////////////////////////////////////////////////////////////////////////////////////////
		// ϵͳ������� ///////////////////////////////////////////////////////////////////////////////	
		// ������
		///////////////////////////////////////////////////////////////////////////////////////////////
    HAL_CfgChk();
		
		IgonAccJudgeIgoff();
}

/*
******************************************************************************
* Function Name  : IgonAccJudgeIgoff
* Description    : ���ھ�ֹ9Сʱ�ж�Ϩ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
static void IgonAccJudgeIgoff(void)
{
    if (*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR)
    {
        if (HAL_SYS_GetState(BIT_SYSTEM_IG) == 1u)
        {
            if(HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 1u)
            {
                OSTmr_Count_Start(&S_uiIgoffAccTmr);
            }
            else
            {
                if (OSTmr_Count_Get(&S_uiIgoffAccTmr) >= (2u * 60u * 60u * 1000u))
                {
                    // Ϩ���¼�����
                    RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);	 // Ϩ�������ݼ�ʱ�����¿���	
                    HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_DISABLE);								
                    HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_DISABLE); 
                  
                    #if(DEF_EVENTINFO_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_EVENT)
                    {
                        //myPrintf("[EVENT]: ON-OFF!\r\n");
                        myPrintf("[EVENT]: IG-OFFEXT!\r\n");
                    }
                    #endif
                    // Ϩ��״̬��ȷ��(ͨ�����������ж�ƫ����) /////////////////
                    S_uiIgoffAccFlag = 1u;
                    HAL_IGInQueueProcess(TYPE_IG_OFFNEW,	igOffSetTime);	// ƫ����ʱ����ƫ��ʱ�����ȷ��
                    S_uiIgoffAccFlag = 0u;
                    
                    // ֪ͨobd��ʼ��ȡ����״̬/////////	
                    if(HAL_SendOBDProProcess(CMD_READ_ALARMSTA,	0,	0,	(u8 *)"",	0) == 0)
                        s_osTmp.obdReadCarStaFlag =1;	// ���ö�ȡ����״̬�����б�־
                    else
                        s_osTmp.obdReadCarStaFlag =0;
                    OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);	// ��ʼ��ȡ����״̬��ʱͳ��
                    if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
                        HAL_GPSInQueueProcess();											// ����һ��GPSλ����Ϣ		
                    
                    // ֪ͨACC�������±궨�豸�Ƕ�
                    if(glob_accAdjustFlag != 1)
                        glob_accAdjustFlag =1;	// ��ʼ�궨
                }
                else
                {
                }
            }
        }
        else
        {
            OSTmr_Count_Start(&S_uiIgoffAccTmr);
        }
    }
    else
    {
    }
}

/*
******************************************************************************
* Function Name  : HAL_GetIgonAccJudgeIgoffFlag
* Description    : �ж��Ƿ���ھ�ֹ9Сʱ�ж�Ϩ��
* Input          : None
* Output         : None
* Return         : 0���Ǿ�ֹ9Сʱ�ж�Ϩ��1�����ھ�ֹ9Сʱ�ж�Ϩ��
******************************************************************************
*/
u8 HAL_GetIgonAccJudgeIgoffFlag(void)
{
    return  S_uiIgoffAccFlag;
}

/*****************************************end*********************************************************/

