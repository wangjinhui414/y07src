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
// 系统电压类别定义
#define	OS_POW_12V										(12)
#define	OS_POW_24V										(24)

// 电压趋势定义
#define	OS_POW_NONE										((u8)0)	// 电压无变化
#define	OS_POW_UP											((u8)1)	// 电压有上升趋势
#define	OS_POW_DOWN										((u8)2) // 电压有下降趋势

// 测试盒以12V为基准校验定义
#define	OS_ADC_6V											(u16)(s_common.aadc * 0.5)	// 设备6V AD估算基准值
#define	OS_ADC_12V										(u16)(s_common.aadc)	  		// 设备12V 基准值(实际检验得到3.3V)
#define	OS_ADC_18V										(u16)(s_common.aadc * 1.5)	// 设备18V AD估算基准值
#define	OS_ADC_24V										(u16)(s_common.aadc * 2)		// 设备24V AD估算基准值基准值

/* 测试盒以24V为基准校验定义
#define	OS_ADC_6V											(u16)(s_common.aadc * 0.25)	// 设备6V AD估算基准值
#define	OS_ADC_12V										(u16)(s_common.aadc * 0.5)	// 设备12V AD估算基准值
#define	OS_ADC_18V										(u16)(s_common.aadc * 0.75)	// 设备18V AD估算基准值
#define	OS_ADC_24V										(u16)(s_common.aadc)				// 设备24V 基准值(实际检验得到3.3V)
*/

#define	OS_EVENT_REMOVE_PER						(0.5)						// 设备移除电压比例标准
#define	OS_EVENT_NOCHECK_PER					(0.75)					// 设备不检测I/O事件比例标准
#define	OS_EVENT_LOWPOW_PER						(0.875)					// 设备地电比例标准

// 点火熄火判断
#define	OS_ADC12_VOLDOWN							((u16)50)				// 熄火电源压降数值(扩大100倍)
#define	OS_ADC24_VOLDOWN							((u16)100)			// 熄火电源压降数值(扩大100倍)
#define	OS_ADC12_VOLUP								((u16)50)				// 熄火电源压升数值(扩大100倍)
#define	OS_ADC24_VOLUP								((u16)100)			// 熄火电源压升数值(扩大100倍)

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

#define	DEF_GET_INTERVAL_TIME					((u32)600)			// 控制电压采集间隔
typedef struct	tag_VolClc_Def
{	
			vu32	tpCounter;		// 电压采集定时器
			u8		tpCount;			// 电压采集点
			u16		tp[5];				// 电压采集数组
			
			u16		valTmp;				// 
			u8		valCount;			// 数值判断累计
			u16		val[3];				// 
	
			vu8		igIntFlag;		// IG中断标志				
			vu8		igOnStep;			// IG点火判断阶段变量
			vu32	igOffUpCounter;		// 熄火过程电压升高计时器
			vu32	igOffDownCounter;	// 熄火过程电压降低计时器
			vu8		igOffStep;		// IG熄火判断阶段变量(1开始熄火判断)
			vu16	startVol;			// 用于记录开始第一点触发压降的起始电压
			
}VolClc_Typedef;


#define	DEF_TJ_INTERVAL_TIME					((u32)5*60000)	// 两次体检间隔5分钟
typedef struct	tag_TjCheck_Def
{	
			vu8		firstTjFlag;			//  首次体检标志(当体检一次后置1)										
			vu8		tjStep;						//	设备触发自动体检标志变量
			vu32	tjDelayCounter;		//	体检延时计时器
			vu32	tjGvlCounter;			//	体检时间过滤计时器			
			vu8		tjToStep;					//  体检超时阶段变量
			vu32	tjToCounter;			//  体检超时计时器
			vu32	tjSoundToCounter;	//	体检语音超时计时器

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
static			vu8										adPoint=0;													// AD循环采集计数
static			vu32									adcCvCounter=0,adcPrnCounter=0;			// AD采集处理

static			vu8										powType=0;													// 电源系统判断变量
static			vu16									powType12Times=0,powType24Times=0;	// 车系电源系统确定次数变量

static			vu32									pnIgTimerCounter=0;									// 上电补报ON事件延时定时器
static			vu32									igTimerCounter;

static			u8										lowpowonTimes=0,lowpowoffTimes=0;
static			vu32									lowpowTimerCounter;

static			vu32									naviCounter=0;											//	IP失效触发延时定时器

static			vu8										logSendedFlag=0;										//  已经上传过LOG信息标志
static			vu8										needSendFlag=0;											//  需要发送LOG信息标志
static			vu32									logUpDelayCounter=0;								//  错误LOG上传时间间隔定时器

static			VolClc_Typedef				s_vol;															//  电压采集计算变量
static			TjCheck_Typedef				s_tj;																//  体检检测处理变量

static			vu8										firstIgOnFlag=0;										//  设备点火检测标志
static			vu32									firstPowOnCounter=0;								//  设备第一次上电标志

static			vu16									u16TmpPowerSystem=OS_ADC12_VOLDOWN;	//  电压趋势判断基准(默认12V系统)

static			vu8										glob_igOffRalStep=0;								//  真实熄火状态阶段变量
static			vu32									obdOff30secCounter=0;								//  OBD熄火检测30秒定时器
static			vu32									obdOff3minCounter=0;								//  OBD熄火检测3分钟定时器
static			vu32									obgOffTrasnToCounter=0;							//  OBD获取车辆状态通信超时定时器

static			vu32									accCheckValCounter=0;								//  ACC触发OBD获取RPM等待定时器
static			vu32									accOn10secCounter=0;								//  ACC熄火检测10秒定时器
static			vu32									accOn2minCounter=0;									//  ACC熄火检测2分钟定时器
static			vu32									accOnTrasnToCounter=0;							//  ACC触发OBD获取车辆状态通信超时定时器

static			vu8										igOffMoveStep=0;										//  熄火移动阶段变量
static			vu32									igOffMoveCounter=0;									//  熄火移动定时器
static			vu32									moveCheckCounter=0;									//  熄火移动GPS速度确认定时器

static			vu32									accShakeCounter=0;									//	加速度振动报警提示定时器
static			vu8										fuelUpRpm=0,fuelCunRpm=0;						//  油耗统计上次RPM状态

static			vu8										wakeUpIsrFlag=0;										//  低脉冲中断标志
static			vu32									igOffSetTime=0;											//  熄火偏移量时间变量

static			vu8										sysPowOnStep=0;											//	设备上电标识发送阶段变量
static			vu32									powOnCounter=0;											//	设备上电标识发送等待计时器

static      u8                    S_ucLastPowerRemoveSt = DEF_SET;    //设备第一次上电默认模块主电源上次状态为未接

static      u32                   S_uiIgoffAccTmr = 0u;
static      vu8                   S_uiIgoffAccFlag = 0u;  


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static	u8 checkTrendPro	(u16 val);	// 电压趋势判断
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
* Description    : 将当前点纳入3成员数值，判断(最早的第一点与最后的第三点压差)有下降0.5V趋势还是上升趋势
* Input          : None
* Output         : None
* Return         : OS_POW_NONE-无效，OS_POW_UP-上升趋势(上升达到0.1V以上)，OS_POW_DOWN-下降趋势(下降达到0.5V以上)
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
* Description    : IG中断服务函数
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
				wakeUpIsrFlag =1;			// 2015-8-19 低脉冲只作为触发条件
}


/*
******************************************************************************
* Function Name  : HAL_SYS_SetState()
* Description    : 
* Input          : staBit 系统状态位定义(BIT_SYSTEM_POW)
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
* Input          : staBit 系统状态位定义(BIT_SYSTEM_POW)
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
* Input          : staBit 系统报警状态位定义(BIT_ALARM_SOS)
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
* Input          : staBit 系统报警状态位定义(BIT_ALARM_SOS)
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
* Input          : ENUM_BSP_STA 联合体成员变量
*                : newSta = DEF_ENABLE or DEF_DISABLE
* Output         : None
* Return         : None
******************************************************************************
*/
void	HAL_BSP_SetState (ENUM_BSP_STA type,	u8 newSta)
{	
		switch(type)
		{		
				//case	DEF_BSPSTA_ON					:	// 目前暂无设置控制
				//			break;
				default: 
							break;
		}				
}
/*
******************************************************************************
* Function Name  : HAL_BSP_GetState()
* Description    : 
* Input          : ENUM_BSP_STA 联合体成员变量
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
				case	DEF_BSPSTA_ON					:	// ON检测线状态（Y06系列产品无该信号线）
							retValue = 0;
							break;
			
				case	DEF_BSPSTA_IG					:	// IG检测线状态
							if(IO_WAKEUP_DET() == DEF_WAKEUP_YES)
									retValue = 1;
							else
									retValue = 0;
							break;
							
				case	DEF_BSPSTA_GPSSTA			:	// GPS是否定位有效'A'or'V'
							if(s_gps.sta == 'A')
									retValue = 1;
							else
									retValue = 0;
							break;
							
				case	DEF_BSPSTA_GPS3D			:	// GPS是否为3D定位
							if(s_gps.fs == '3')
									retValue = 1;
							else
									retValue = 0;
							break;
				
				case	DEF_BSPSTA_GPSSPEED		:	// GPS速度获取
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
* Description    : 判断拆除报警是否发送完毕
* Input          : NONE
* Output         : None
* Return         : 1：发送完毕， 0：未发送
******************************************************************************
*/
u8 HAL_IsPwrRemoveSendOk(void)
{
    return ((HAL_ComQue_size (DEF_RAM_QUE) == 0) 	&& (s_ram.len == 0)  && (HAL_ComQue_size (DEF_FLASH_QUE) == 0) && (s_flash.len == 0));	
}

/*
******************************************************************************
* Function Name  : HAL_GetLastPowerRemoveSt
* Description    : 获取上次拆除报警状态,
* Input          : NONE
* Output         : None
* Return         : 1：拆除， 0：插上
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
* Description    : 获取上次拆除报警状态,
* Input          : NONE
* Output         : None
* Return         : 1：拆除， 0：插上
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
* Description    : 检测拆除报警,
* Input          : usVal：电压值
*                  usPeriod：函数调用周期
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
                    HAL_IGInQueueProcess(TYPE_IG_OFFNEW,	0u);	// 偏移量时间由偏移时间变量确定
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
* Description    : 事件及系统报警逻辑处理,
*                  更新系统状态变量:s_sys.sysState  及 s_sys.alarmState
* Input          : s_Tmp.alarmState 与 s_Cfg.alarmState定义相同
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
		// AD采集处理/////////////////////////////////////////////////////////////////////////////////////
		// 条件：时间触发
		//////////////////////////////////////////////////////////////////////////////////////////////////
		if(OSTmr_Count_Get(&adcCvCounter) >= 50)
		{
				if(adPoint < DEF_ADC_MAXNUM)	// 100ms计算一次
				{								
						// AD采样
						s_adInfo.ADCVBat[adPoint]  = BSP_ADC_GetValue(DEF_ADC_VBAT);	  // 设备外部电源电压处理
						adPoint++;
				}	
				if(adPoint >= DEF_ADC_MAXNUM)
				{
						adPoint=0;
						// 计算外部电瓶电压 /////////////////////////////////////////
						u16TmpADC = ADCalculateProcess((u16 *)&(s_adInfo.ADCVBat[0]),	DEF_ADC_MAXNUM); 
						s_adInfo.ADCVBatBK = u16TmpADC; 
																				
						if((s_common.aadc <= 1500) || (s_common.aadc >= 3200))		// 如果设备出厂未校验则使用默认值计算
								s_common.aadc =	DEF_DFT_COMMON_AADC;
						
						s_adInfo.VBatVol	=	(u16)((12 * u16TmpADC * 100)/s_common.aadc + 0.5); 
            
            PowerRemoveChk(s_adInfo.VBatVol, (50u * DEF_ADC_MAXNUM));
						
						// 打印输出
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
		// 判断汽车电压类型(12V or 24V)///////////////////////////////////////////////////////////////////
		// 条件：电压高于18V时认为24V车系低于18V时认为12V车系
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
								u16TmpPowerSystem =OS_ADC24_VOLDOWN;	// 修改电压趋势判断标准
								
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
								u16TmpPowerSystem =OS_ADC12_VOLDOWN;	// 修改电压趋势判断标准
						}
				}
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// 系统状态更新////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////
  
		// 电源状态更新/////////////////////////
    HAL_SYS_SetState(BIT_SYSTEM_POW,	DEF_ENABLE);
		
    // 外接电源类型更新////////////////////
    if(powType == OS_POW_24V)
        HAL_SYS_SetState(BIT_SYSTEM_POW24,	DEF_ENABLE);
    else
        HAL_SYS_SetState(BIT_SYSTEM_POW24,	DEF_DISABLE);
		
		/////////////////////////////////////////////////////////////////////////////////////////////////
		// ON 事件///////////////////////////////////////////////////////////////////////////////////////
		// 条件：低脉冲触发点火(如果点火超过熄火判断时间后再产生低脉冲也将认为再次点火)，电压降低触发熄火
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// 第一次上电处理////////////////////////////
		if(firstIgOnFlag == 0)
		{
				firstIgOnFlag =1;	// 禁止再次进入
			
				// 点火补报处理///////////////////
				tmpIgSta =BSP_BACKUP_LoadIGOnSta();
				if(tmpIgSta == 1)
				{
						if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 0) 
						{
								//////////////////////////////////////////
								OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);			// 从新开始点火GPS信息上传时间
								OSTmr_Count_Start(&s_osTmp.igOnChiXuCounter);		// 点火持续时间计时器开始
								
								HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_ENABLE);
								HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_ENABLE);               
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
								{
										//myPrintf("[EVENT]: ON-ON!\r\n");
										myPrintf("[EVENT]: IG-ON-Keep!\r\n");
								}
								#endif
								
								// 清熄火检测标志 ///////////////////
								s_vol.igOnStep  =0;
								s_vol.igIntFlag =0;			// 清低脉冲触发变量		
								s_vol.startVol  =0;			// 清起始电压最大值
								s_vol.valCount	=0;			// 重新开始采集3点电压值
								s_vol.igOffStep =1;			// 开始熄火检测		

								// 启动OBD熄火检测逻辑//////////////
								glob_igOffRalStep =0;		// 清阶段变量开始熄火检测
								OSTmr_Count_Start(&obdOff30secCounter);		// 30s计时开始
								HAL_SYS_SetState(BIT_SYSTEM_POWDOWN,	DEF_DISABLE);	// 清电压降低状态
						}
				}
		}	
		
		// 点火过滤处理//////////////////////////////
		if(s_vol.igIntFlag == 1)
		{
				OSTimeDly(1000);	// 增加延时与数据发送时间上分开(模拟器的低脉冲会影响数据发送,导致模块故障复位)
			
				/*
				if((HAL_SYS_GetState(BIT_SYSTEM_IG) == 0) ||
					 ((HAL_SYS_GetState(BIT_SYSTEM_IG) == 1) && (OSTmr_Count_Get(&s_osTmp.igOnChiXuCounter) >= (s_cfg.igOffDelay * 1000))))
				*/
				if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 0)
				{	
						// 油耗结构变量初始化 2015-6-19 油耗项目添加
						/*
						fuelUpRpm =0xAA;	// 置初始状态
						memset((u8 *)&s_fuel,	0, sizeof(s_fuel));
						HAL_SendOBDProProcess(CMD_READ_FUEL,	0,	0,	(u8 *)"",	0);			// 通知OBD读取当前油量			
						*/
					
						// 体检开始语音处理(先播放语音后打包点火数据避免语音播报与发送数据冲突)/////////////////
						HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_ENABLE);				// 系统置体检状态中
						HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_ENABLE);	// 设置播放还未播放体检结束状态
						if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
								HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound1,	strlen((const char *)s_cfg.sound1));		// 播放体检开始语音											
						
						/*
						// 熄火补报 //////////////////////
						if((HAL_SYS_GetState(BIT_SYSTEM_IG) == 1) && (OSTmr_Count_Get(&s_osTmp.igOnChiXuCounter) >= (s_cfg.igOffDelay * 1000)))
						{
								// 已经超过熄火延时(有低脉冲则认为有效(防止熄火逻辑判断失败导致一直不熄火))
								RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);	 // 熄火间隔数据计时器重新开启					
								HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_DISABLE);								
								HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_DISABLE); 
							
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
								{
										//myPrintf("[EVENT]: ON-OFF!\r\n");
										myPrintf("[EVENT]: IG-OFF-BUBAO!\r\n");
								}
								#endif
								// 熄火状态已确定 /////////////////
								HAL_IGInQueueProcess(TYPE_IG_OFFNEW,0);		// 2015-4-15 补报的熄火偏移量为0
								// 通知obd任务设备熄火 ////////////
								HAL_SendOBDProProcess(CMD_IG_OFF,	0,	0,	(u8 *)"",	0);								
								// 结束熄火检测 ///////////////////
								s_vol.igOffStep =0;
						}
						*/
						// 开始点火检测 ///////////////////
						s_vol.igOnStep =1;
				}						
				else
						s_vol.igIntFlag =0;	// 条件不满满足过滤掉本次点火
		}
		// 点火逻辑处理//////////////////////////////
		if(s_vol.igOnStep == 1)
		{
				//////////////////////////////////////////
				OSTmr_Count_Start(&s_osTmp.igOnMsgCounter);			// 从新开始点火GPS信息上传时间
				OSTmr_Count_Start(&s_osTmp.igOnChiXuCounter);		// 点火持续时间计时器开始
				
				HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_ENABLE);
				HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_ENABLE);               
				#if(DEF_EVENTINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_EVENT)
				{
						OSTimeDly(500);	// 增加1s延时否则可能系统刚从中断中唤醒串口时钟未稳定导致串口信息打印不全
						//myPrintf("[EVENT]: ON-ON!\r\n");
						myPrintf("[EVENT]: IG-ON!\r\n");
				}
				#endif
				
				// 点火状态已确定 //////////////////
				HAL_IGInQueueProcess(TYPE_IG_ON,0);		// 2015-4-15 点火没有时间偏移量
				// 通知obd任务设备点火//////////////
				HAL_SendOBDProProcess(CMD_IG_ON,	0,	0,	(u8 *)"",	0);	
				// 通知自动体检检测开始	////////////
				s_tj.tjStep	=1;	
				
				// 清熄火检测标志 ///////////////////
				s_vol.igOnStep  =0;
				s_vol.igIntFlag =0;			// 清低脉冲触发变量		
				s_vol.startVol  =0;			// 清起始电压最大值
				s_vol.valCount	=0;			// 重新开始采集3点电压值
				s_vol.igOffStep =1;			// 开始熄火检测
				
				// 启动OBD熄火检测逻辑//////////////
				glob_igOffRalStep =0;		// 清阶段变量开始熄火检测
				OSTmr_Count_Start(&obdOff30secCounter);		// 30s计时开始
				HAL_SYS_SetState(BIT_SYSTEM_POWDOWN,	DEF_DISABLE);	// 清电压降低状态
		}
		
		// 电压采集过滤(熄火逻辑处理)///////////////////////////////////////////////////
		if(OSTmr_Count_Get(&s_vol.tpCounter) >= DEF_GET_INTERVAL_TIME)	// 600ms采集一点
		{
				OSTmr_Count_Start(&s_vol.tpCounter);	// 重新计时			
				s_vol.tp[s_vol.tpCount++]	=s_adInfo.VBatVol;	// 获取当前电压			
				if(s_vol.tpCount >= 5)
				{
						s_vol.tpCount =0;	// 重新开始采集
						s_vol.valTmp =ADCalculateProcess(s_vol.tp,	5); 	// 取平均值
					
						// 电压趋势判断				
						if(s_vol.igOffStep >= 1)
								tmpV =checkTrendPro(s_vol.valTmp);	// 只有点火后才开始更新电压趋势	
											
						if(s_vol.igOffStep == 1)
						{	
								// 处理电压趋势
								if(tmpV == OS_POW_UP)
								{										
										s_vol.igOffStep =2;
										s_vol.startVol =s_vol.val[0];								// 记录第一点触发压升的起始电压
										OSTmr_Count_Start(&s_vol.igOffUpCounter);	// 开始计时器
								}
								else if(tmpV == OS_POW_DOWN)
								{						
										s_vol.igOffStep =4;	
										s_vol.startVol =s_vol.val[0];								// 记录第一点触发压降的起始电压
										OSTmr_Count_Start(&s_vol.igOffDownCounter);	// 开始熄火状态确认记时
								}
								else
								{
										;	// 电压基本不变(-0.5V < Vol < +0.5V)不予处理
								}
						}
						else if(s_vol.igOffStep == 2)
						{
								// 电压升高阶段1
								if((s_vol.startVol <= s_vol.valTmp) && ((s_vol.valTmp - s_vol.startVol) >= u16TmpPowerSystem))
								{
										if(OSTmr_Count_Get(&s_vol.igOffUpCounter) >= 20000)
												s_vol.igOffStep =1;											// 返回趋势判断阶段
								}
								else
										s_vol.igOffStep =1;											// 返回趋势判断阶段
						}
						else if(s_vol.igOffStep == 3)
						{
								// 电压升高阶段2
								if((s_vol.startVol <= s_vol.valTmp) && ((s_vol.valTmp - s_vol.startVol) >= u16TmpPowerSystem))
								{
										if(OSTmr_Count_Get(&s_vol.igOffUpCounter) >= 20000)
												s_vol.igOffStep =1;	// 电压升高持续20s以上重新采集起始电压
								}
								else
										s_vol.igOffStep = 4;		// 继续熄火判断
						}
						else if(s_vol.igOffStep == 4)
						{							
								if((s_vol.startVol <= s_vol.valTmp) && ((s_vol.valTmp - s_vol.startVol) >= u16TmpPowerSystem))
								{
										// 电压升高处理
										s_vol.igOffStep = 3;						// 低压过程中一旦发现电压升高则20秒电压过滤阶段3
										OSTmr_Count_Start(&s_vol.igOffUpCounter);	
								}	
								else
								{
										// 低压持续阶段										
										if((s_vol.startVol > s_vol.valTmp) && ((s_vol.startVol - s_vol.valTmp) >= u16TmpPowerSystem))
										{												
												if(OSTmr_Count_Get(&s_vol.igOffDownCounter) >= (s_cfg.igOffDelay * 1000))		
												{
														// 熄火事件产生
														/*
														RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);	 // 熄火间隔数据计时器重新开启	
														HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_DISABLE);								
														HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_DISABLE); 
													
														#if(DEF_EVENTINFO_OUTPUTEN > 0)
														if(dbgInfoSwt & DBG_INFO_EVENT)
														{
																//myPrintf("[EVENT]: ON-OFF!\r\n");
																myPrintf("[EVENT]: IG-OFF!\r\n");
														}
														#endif
														// 熄火状态已确定 /////////////////
														HAL_IGInQueueProcess(TYPE_IG_OFFNEW,	(s_cfg.igOffDelay + 180));	// 由电压降低判断出的熄火偏移量为熄火延迟+OBD判断延迟	
														// 通知obd任务设备熄火 ////////////
														HAL_SendOBDProProcess(CMD_IG_OFF,	0,	0,	(u8 *)"",	0);
														OSTimeDly(200);	// 给与OBD处理时间
														// 通知obd任务设备将进入睡眠 //////
														// OBD部分还没做
														//HAL_SendOBDProProcess(CMD_SYS_SLEEP,	0,	0,	(u8 *)"",	0); 
														OSTimeDly(200);	// 给与OBD处理时间
														// 通知obd开始读取车辆状态/////////	
														HAL_SendOBDProProcess(CMD_READ_STA,	0,	0,	(u8 *)"",	0);
														s_osTmp.obdReadCarStaFlag =1;	// 设置读取车辆状态处理中标志
														OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);	// 开始读取车辆状态超时统计
														if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
																HAL_GPSInQueueProcess();											// 补充一条GPS位置信息
														*/

														HAL_SYS_SetState(BIT_SYSTEM_POWDOWN,	DEF_ENABLE);	//设置电压降低状态
																
														// 结束熄火检测 ///////////////////
														s_vol.igOffStep =0;		
														
												}
										}
										else
										{
												;	// 如果过程中电压再次升高则进入20s禁止电压检测环节后从新确定起始值
										}
								}
						}
						else
								s_vol.igOffStep =0;	// 变量非法复位
				}
		}
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//升级体检处理///////////////////////////////////////////////////////////////////////
		// 条件：设备自动升级后出发体检
		//////////////////////////////////////////////////////////////////////////////////////////////////	
		if(OSTmr_Count_Get(&firstPowOnCounter) >= 3000)
		{
				if(s_common.otapErrCode != 0)
				{
						if(s_common.otapErrCode == 1)
						{
								HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_ENABLE);				// 系统置体检状态中
								HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_ENABLE);	// 设置播放还未播放体检结束状态
								if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
										HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound1,	strlen((const char *)s_cfg.sound1));		// 播放体检开始语音	
								
								HAL_SendOBDProProcess(CMD_AUTO_TJ,	0,	0,	(u8 *)"",	0);	// 通知obd任务执行自动体检
						}
            HAL_CommonParaReload();
						s_common.otapErrCode =0;	// 清故障标志
						HAL_SaveParaPro (DEF_TYPE_COMMON,	(void *)&s_common,	sizeof(s_common));				
				}			
		}		
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//系统自动体检延迟触发检测///////////////////////////////////////////////////////////////////////
		// 条件：点火体检延时后触发体检
		//////////////////////////////////////////////////////////////////////////////////////////////////				
		if(s_tj.tjStep == 1)
		{
				s_tj.tjStep =2;
				OSTmr_Count_Start(&s_tj.tjDelayCounter);		// 体检延迟计时开始		
		}
		else if(s_tj.tjStep == 2)
		{
				if(OSTmr_Count_Get(&s_tj.tjDelayCounter) >= (s_cfg.tjDelayTime * 1000))
				{
						s_tj.tjStep =3;
						HAL_SendOBDProProcess(CMD_AUTO_TJ,	0,	0,	(u8 *)"",	0);	// 通知obd任务执行自动体检							
				}	
		}
		
		// 体检超时判断
		if(s_tj.tjToStep == 0)
		{
				if(HAL_SYS_GetState (BIT_SYSTEM_TJSTA) == 1)
				{
						s_tj.tjToStep =1;					
						OSTmr_Count_Start(&s_tj.tjSoundToCounter); 	// 开始结束语音超时计时	
						OSTmr_Count_Start(&s_tj.tjToCounter); 			// 开始超时计时		
				}
		}
		else if(s_tj.tjToStep == 1)
		{
				// 体检语音超时处理
				if(HAL_SYS_GetState (BIT_SYSTEM_TJSOUND_END) == 1)
				{
						if(OSTmr_Count_Get(&s_tj.tjSoundToCounter) >= OS_TJSOUND_MAX_TIMEOUT)
						{
								HAL_SYS_SetState (BIT_SYSTEM_TJSOUND_END,	DEF_DISABLE);	// 体检结束语音状态清除
								if(s_cfg.devEn & DEF_DEVEN_BIT_TTS)
										HAL_SendCONProProcess(TTS_CMD,	0,	s_cfg.sound2,	strlen((const char *)s_cfg.sound2));		// 播放体检结束语音
						}
				}
				// 体检状态超时处理
				if(HAL_SYS_GetState (BIT_SYSTEM_TJSTA) == 1)
				{										
						if(OSTmr_Count_Get(&s_tj.tjToCounter) >= OS_TJ_MAX_TIMEOUT)
						{
								s_tj.tjToStep =0;
								HAL_SYS_SetState (BIT_SYSTEM_TJSTA,	DEF_DISABLE);	// 系统置体检状态中清除	
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[EVENT]: TiJian %ds timeout!\r\n",	OS_TJ_MAX_TIMEOUT/1000);
								#endif
						}						
				}
				else
						s_tj.tjToStep =0;	// 标志复位重新计时		
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//取车车辆状态变量处理///////////////////////////////////////////////////////////////////////
		// 条件：本次体检时间与上次体检时间差大于5分钟，且经过点火体检延时后触发
		//////////////////////////////////////////////////////////////////////////////////////////////////				
		
		//////////////////////////////////////////////////////////////////////////////////////////////////
		// GPS重新定位 处理 //////////////////////////////////////////////////////////////////////////////	
		// 条件：上次未定位本次定位时触发
		//////////////////////////////////////////////////////////////////////////////////////////////////
		if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) != (u8)(s_event.gpsFixSta.up))
		{
				if(s_event.gpsFixSta.up == FALSE)
				{
						if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
						{		
								// GPS重新定位
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
		// 报警低压状态 ////////////////////////////////////////////////////////////////////////////////// 
		// 条件：电压低与系统配置值
		//////////////////////////////////////////////////////////////////////////////////////////////////
		if(powType != 0)
		{
				if(((powType == OS_POW_12V) && (s_adInfo.VBatVol > (s_cfg.lowPower)))  ||
					 ((powType == OS_POW_24V) && (s_adInfo.VBatVol > (s_cfg.lowPower*2))))
				{		
						lowpowoffTimes = 0;
						// 设备正常电压判断 //////////////
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
						// 设备低压判断 ///////////////						
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
										//HAL_AlarmInQueueProcess(ALARM_BIT_POWER);	// 打包低压报警数据包 2015-9-30 取消低压报警数据包
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[ALARM]: POW-LOW!\r\n");
										#endif
								}
						}			
				}
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////
		// 设备熄火综合判断 ///////////////////////////////////////////////////////////////////////
		// 条件：当前为点火状态，电压低于11V，设备停止状态，或电压压降超过0.5V，循环检测3min
		///////////////////////////////////////////////////////////////////////////////////////////
		if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 1)
		{
				// 判断开始
				if(glob_igOffRalStep == 0)
				{
						if((HAL_SYSALM_GetState(BIT_ALARM_POWLOW) == 1) ||
							 ((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0)) ||
							 (HAL_SYS_GetState(BIT_SYSTEM_POWDOWN) == 1))
							 //((*(s_devSta.pMems) != DEF_MEMSHARD_NONE_ERR) && (HAL_SYS_GetState(BIT_SYSTEM_POWDOWN) == 1)))
						{
								if(OSTmr_Count_Get(&obdOff30secCounter) >= 30000)// OBD每次检查时间应大于30s
								{
										OSTmr_Count_Start(&obdOff3minCounter);	// 开始3分钟计时
										glob_igOffRalStep =1;	// 开始车辆通信
									
										// 2015-8-19 设置触发条件变量，修改熄火偏减的偏移量不一致问题
										if(HAL_SYSALM_GetState(BIT_ALARM_POWLOW) == 1)		
												igOffSetTime =180;												// 由加低压报警状态触发的熄火偏移量为静止延迟180s	
										else if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0))
												igOffSetTime =180;												// 由加速度判断车辆静止状态触发的熄火偏移量为静止延迟180s	
										else if(HAL_SYS_GetState(BIT_SYSTEM_POWDOWN) == 1)
												igOffSetTime =(s_cfg.igOffDelay + 180);		// 由电压降低判断出的熄火偏移量为熄火延迟+OBD判断延迟	
										else
												igOffSetTime =180;												// 其它默认180s	
									
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
				// 30秒循环判断阶段
				else if((glob_igOffRalStep >= 1) && (glob_igOffRalStep <= 3))
				{
						// 30s循环开始阶段
						if(glob_igOffRalStep == 1)
						{
								if(OSTmr_Count_Get(&obdOff30secCounter) >= 30000)
								{
										glob_igSta	=DEF_IG_RPM_START;	// 置开始检测标志
										glob_igOffRalStep =2;
								}
						}
						// 发送请求OBD读取车辆状态阶段
						else if(glob_igOffRalStep == 2)
						{
								// 通知obd任务开始检测车辆是否熄火 ////////////
								if(HAL_SendOBDProProcess(CMD_GET_CARDATA,	0,	0,	(u8 *)"",	0) == 0)
								{
										glob_igOffRalStep =3;	// 等待OBD回复阶段
										OSTmr_Count_Start(&obgOffTrasnToCounter);	// 通信超时统计开始						
								}
								else
								{
										glob_igOffRalStep =1;	// 重新判断状态
										OSTmr_Count_Start(&obdOff30secCounter);		// 30s计时开始
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
												myPrintf("[EVENT]: OBD datalock=1 check next 30s!\r\n");
										#endif
								}
						}
						// 等待OBD回复阶段
						else if(glob_igOffRalStep == 3)
						{
								if((glob_igSta == DEF_IG_RPM_0) || (glob_igSta == DEF_IG_ECU_TIMEOUT))
								{
										glob_igOffRalStep =1;		// 继续监控车辆状态
										OSTmr_Count_Start(&obdOff30secCounter);		// 30s计时开始
									
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
                        glob_igOffRalStep =1;		// 继续监控车辆状态
                        OSTmr_Count_Start(&obdOff30secCounter);		// 30s计时开始
                    }
                    else
                    {
                        glob_igOffRalStep =0;		// 继续监控车辆状态		
                        OSTmr_Count_Start(&obdOff30secCounter);		// 30s计时开始
                        if(HAL_SYS_GetState(BIT_SYSTEM_POWDOWN) == 1)
                        {							
                            // 清空由电压导致的熄火判断条件
                            s_vol.igOnStep  =0;
                            s_vol.igIntFlag =0;			// 清低脉冲触发变量		
                            s_vol.startVol  =0;			// 清起始电压最大值
                            s_vol.valCount	=0;			// 重新开始采集3点电压值
                            s_vol.igOffStep =1;			// 开始熄火检测
                            HAL_SYS_SetState(BIT_SYSTEM_POWDOWN,	DEF_DISABLE);	// 清电压降低状态		
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
												glob_igOffRalStep =1;		// 继续监控车辆状态
												OSTmr_Count_Start(&obdOff30secCounter);		// 30s计时开始
												#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_OTHER)
														myPrintf("[EVENT]: OBD return 60s timeout!\r\n");
												#endif
										}					
								}
						}
						// 3分钟判断超时判断处理
						if(OSTmr_Count_Get(&obdOff3minCounter) >=  180000)
						{
								glob_igOffRalStep =4;	// 进入熄火逻辑处理		
								#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_OTHER)
										myPrintf("[EVENT]: OBD Check 3min timeout!\r\n");
								#endif
						}
				}
				// 熄火事件确定打包处理
				else if(glob_igOffRalStep == 4)
				{
						glob_igOffRalStep =5;		// 设备进入睡眠等待状态
					
						// 熄火事件产生
						RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);	 // 熄火间隔数据计时器重新开启	
						HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_DISABLE);								
						HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_DISABLE); 
					
						#if(DEF_EVENTINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_EVENT)
						{
								//myPrintf("[EVENT]: ON-OFF!\r\n");
								myPrintf("[EVENT]: IG-OFF!\r\n");
						}
						#endif
						// 熄火状态已确定(通过触发条件判断偏移量) /////////////////
						HAL_IGInQueueProcess(TYPE_IG_OFFNEW,	igOffSetTime);	// 偏移量时间由偏移时间变量确定
						
						// 通知obd任务设备熄火 ////////////
						//HAL_SendOBDProProcess(CMD_IG_OFF,	0,	0,	(u8 *)"",	0);
						//OSTimeDly(200);	// 给与OBD处理时间
						// 通知obd任务设备将进入睡眠 //////
						//HAL_SendOBDProProcess(CMD_SYS_SLEEP,	0,	0,	(u8 *)"",	0); 
						//OSTimeDly(200);	// 给与OBD处理时间
						// 通知obd开始读取车辆状态/////////	
						if(HAL_SendOBDProProcess(CMD_READ_ALARMSTA,	0,	0,	(u8 *)"",	0) == 0)
								s_osTmp.obdReadCarStaFlag =1;	// 设置读取车辆状态处理中标志
						else
								s_osTmp.obdReadCarStaFlag =0;
						OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);	// 开始读取车辆状态超时统计
						if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
								HAL_GPSInQueueProcess();											// 补充一条GPS位置信息		
						
						// 通知ACC任务重新标定设备角度
						if(glob_accAdjustFlag != 1)
								glob_accAdjustFlag =1;	// 开始标定
				}
				// 休眠等待阶段
				else if(glob_igOffRalStep == 5)
				{
						;		// 等待进入睡眠
				}
				else
						glob_igOffRalStep =0;	// 非法参数复位
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////
		// 通过加速度中断弥补熄火误判逻辑 /////////////////////////////////////////////////////////
		// 条件：当前为熄火状态，且加速度中断产生，循环检测2min
		///////////////////////////////////////////////////////////////////////////////////////////
		if(wakeUpIsrFlag == 1)
		{			
				if(s_ACC.IntFlag == 0)
				{
						s_ACC.IntFlag =1;		// 2015-8-19 模拟震动产生进入点火逻辑判断
						wakeUpIsrFlag =0;		
				}
				else
				{
						;	// 设备处于点火检测过程中等待检测完成再次触发检测
				}
		}
		if(HAL_SYS_GetState(BIT_SYSTEM_IG) == 0)
		{
				// 判断开始
				if(glob_accOnRalStep == 0)
				{
						if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (s_ACC.IntFlag == 1))
						{
								OSTmr_Count_Start(&accOn2minCounter);	// 开始2分钟计时
								glob_accOnRalStep =1;	// 开始车辆通信
								OSTmr_Count_Start(&accCheckValCounter);	// 开始检测延迟定时器
							
								#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_OTHER)
										myPrintf("[EVENT]: (ACC)Start OBD check mainloop!\r\n");
								#endif
						}
				}
				// 检测延迟阶段
				else if(glob_accOnRalStep == 1)
				{
						if(OSTmr_Count_Get(&accCheckValCounter) >= 1500)// 1.5s后开始检测
						{
								glob_igSta	=DEF_IG_RPM_START;	// 置检测开始
								glob_accOnRalStep =3;	// 第一次进入循环跳过10s延时
						}
				}
				// 10s循环阶段
				else if((glob_accOnRalStep >= 2) && (glob_accOnRalStep <= 4))
				{
						// 10s循环开始阶段
						if(glob_accOnRalStep == 2)
						{
								if(OSTmr_Count_Get(&accOn10secCounter) >= 10000)
								{
										glob_igSta	=DEF_IG_RPM_START;	// 置检测开始
										glob_accOnRalStep =3;
								}
						}
						// 发送请求OBD读取车辆状态阶段
						else if(glob_accOnRalStep == 3)
						{
								// 通知obd任务开始检测车辆是否熄火 ////////////
								if(HAL_SendOBDProProcess(CMD_GET_CARDATA,	0,	0,	(u8 *)"",	0) == 0)
								{
										glob_accOnRalStep =4;	// 等待OBD回复阶段
										OSTmr_Count_Start(&accOnTrasnToCounter);	// 通信超时统计开始						
								}
								else
								{
										glob_accOnRalStep =2;	// 重新判断状态
										OSTmr_Count_Start(&accOn10secCounter);		// 10s计时开始
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
												myPrintf("[EVENT]: (ACC)OBD datalock=1 check next 10s!\r\n");
										#endif
								}
						}
						// 等待OBD回复阶段
						else if(glob_accOnRalStep == 4)
						{
								if(glob_igSta == DEF_IG_RPM_0)
								{
										glob_accOnRalStep =2;		// 2015-11-4 RPM读取操作如果返回RPM<100则继续检测直到超时退出
										OSTmr_Count_Start(&accOn10secCounter);	// 开始检测延迟计时		
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
												myPrintf("[EVENT]: (ACC)OBD return RPM<%d!\r\n", glob_igSta);
										#endif
								}									
								else if(glob_igSta == DEF_IG_ECU_TIMEOUT)
								{
										glob_accOnRalStep =2;		// 继续监控车辆状态
										OSTmr_Count_Start(&accOn10secCounter);		// 10s计时开始								
										#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_OTHER)
												myPrintf("[EVENT]: (ACC)OBD return ECU timeout!\r\n");
										#endif
								}
                else if (glob_igSta >= DEF_IG_RPM_100)
                {
                    if ((DEF_IG_RPM_100 != glob_igSta) && ((0u == glob_igStaLast) || (glob_igSta == glob_igStaLast)))
                    {
                        glob_accOnRalStep =2;		// 继续监控车辆状态
                        OSTmr_Count_Start(&accOn10secCounter);		// 10s计时开始		
                    }
                    else
                    {
                        s_vol.igIntFlag = 1;		// 模拟车辆低脉冲动作,强制执行点火逻辑!!!!
                        s_ACC.IntFlag =0;				// 清中断标志			
                        glob_accOnRalStep =0;		// 退出本次检测		
                        OSTmr_Count_Start(&accOn10secCounter);	// 开始检测延迟计时	
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
												glob_accOnRalStep =2;		// 继续监控车辆状态
												OSTmr_Count_Start(&accOn10secCounter);		// 10s计时开始
												#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_OTHER)
														myPrintf("[EVENT]: (ACC)OBD return 60s timeout!\r\n");
												#endif
										}					
								}
						}
						// 3分钟判断超时判断处理
						if(OSTmr_Count_Get(&accOn2minCounter) >=  180000)
						{
								s_ACC.IntFlag =0;				// 清中断标志			
								glob_accOnRalStep =0;		// 退出本次检测		
								OSTmr_Count_Start(&accOn10secCounter);	// 开始检测延迟计时		
								#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_OTHER)
										myPrintf("[EVENT]: (ACC)OBD Check 3min timeout!\r\n");
								#endif
						}
				}
				else
						glob_accOnRalStep =0;	// 非法参数复位				
		}
		else
		{
				s_ACC.IntFlag =0;				// 清中断标志防止设备从点火转入熄火时将之前点火过程中的中断标志记录下来
				glob_accOnRalStep = 0;	// 阶段复位防止处于中间检测流程后点火直接退出之后无法再次进入流程
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// 报警熄火移动状态 ///////////////////////////////////////////////////////////////////////////
		// 条件：GPS定位且当前为熄火状态
		//////////////////////////////////////////////////////////////////////////////////////////////
		if((HAL_SYS_GetState(BIT_SYSTEM_IG) == 0) && (HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE) == 0u))
		{
				if(igOffMoveStep == 0)
				{
						if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (s_ACC.IntFlag1 == 1))
						{
								igOffMoveStep =1;
								OSTmr_Count_Start(&igOffMoveCounter);	// 开始移动检测定时器
						}			
				}
				// 移动状态触发阶段
				else if(igOffMoveStep == 1)
				{
						if(OSTmr_Count_Get(&igOffMoveCounter) < (s_cfg.sysMoveT*1000))
						{
								if((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1) && (HAL_BSP_GetState(DEF_BSPSTA_GPSSPEED) >= s_cfg.sysMove))
								{								
										igOffMoveStep =2;
										OSTmr_Count_Start(&moveCheckCounter);	// 开始定时器										
								}
						}
						else
						{
								s_ACC.IntFlag1 =0;	// 清中断标志
								igOffMoveStep =0;	// 退出本次检测
						}							
				}
				// 移动确定阶段(考虑到GPS漂移)
				else if(igOffMoveStep == 2)
				{
						if((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1) && (HAL_BSP_GetState(DEF_BSPSTA_GPSSPEED) >= s_cfg.sysMove))
						{
								if(OSTmr_Count_Get(&moveCheckCounter) >= 60000)	// 2015-9-28 移动报警进入时间修改为60s
								{
//										igOffMoveStep = 3;
//										glob_igStaExt =DEF_IG_RPM_START;	// 开始检测车辆RPM
//										if(HAL_SendOBDProProcess(CMD_GET_CARDATA,	0,	0,	(u8 *)"",	0) == 0)
//												OSTmr_Count_Start(&moveCheckCounter);	// 开始定时器	
//										else
//												igOffMoveStep =4;	// 通信堵塞跳过RPM判断过程
										igOffMoveStep =4;	// 2015-8-5 直接跳过熄火判断
								}
						}
						else 
								igOffMoveStep =1;	// GPS不定位或速度低于门限跳到状态确认阶段
				}
				// 车辆熄火状态再次确认
				else if(igOffMoveStep == 3)
				{
						if(glob_igStaExt == DEF_IG_RPM_100)
						{
								s_ACC.IntFlag1 =0;	// 清中断标志
								igOffMoveStep =0;	// 退出本次检测
						}
						else if((glob_igStaExt == DEF_IG_RPM_0) || (glob_igStaExt == DEF_IG_ECU_TIMEOUT))
								igOffMoveStep =4;	// 熄火移动状态确定						
						else
						{
								if(OSTmr_Count_Get(&moveCheckCounter) >= 60000)
										igOffMoveStep =4;	// 通信超时认为车辆熄火
						}
				}
				// 熄火移动状态确认
				else if(igOffMoveStep == 4)
				{
						if(glob_accOnRalStep == 0)
						{
								igOffMoveStep =5;
								HAL_SYSALM_SetState(BIT_ALARM_ILLMOVE,	DEF_ENABLE);	// 置熄火移动报警状态
								HAL_AlarmInQueueProcess	(ALARM_BIT_MOVE);	// 打包熄火移动报警报文
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[ALARM]: IG-OFF-Move Enter!\r\n");
								#endif			
						}
						else
						{
								igOffMoveStep =7;
								OSTmr_Count_Start(&moveCheckCounter);	// 开始定时器	
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[ALARM]: IG-OFF-Move Wait!\r\n");
								#endif							
						}
				}
				// 状态退出检测
				else if(igOffMoveStep == 5)
				{
						if(((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0) && (HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0))	||
							 ((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1) && (HAL_BSP_GetState(DEF_BSPSTA_GPSSPEED) < s_cfg.sysMove)))
						{
								igOffMoveStep =6;
								OSTmr_Count_Start(&moveCheckCounter);	// 开始定时器								
						}
				}
				// 静止状态确定阶段
				else if(igOffMoveStep == 6)
				{
						if(((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 0) && (HAL_SYS_GetState(BIT_SYSTEM_MOVE_ACC) == 0))	||
							 ((HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1) && (HAL_BSP_GetState(DEF_BSPSTA_GPSSPEED) < s_cfg.sysMove)))
						{
								if(OSTmr_Count_Get(&moveCheckCounter) >= 10000)
								{
										s_ACC.IntFlag1 =0;	// 清中断标志
										igOffMoveStep = 0;	// 退出循环
										HAL_SYSALM_SetState(BIT_ALARM_ILLMOVE,	DEF_DISABLE);	// 清熄火移动报警状态
										#if(DEF_EVENTINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_EVENT)
												myPrintf("[ALARM]: IG-OFF-Move Exit!\r\n");
										#endif
								}						
						}
						else
								OSTmr_Count_Start(&moveCheckCounter);	// 重新计时
				}
				// 熄火移动报文打包等待阶段
				else if(igOffMoveStep == 7)
				{
						if(glob_accOnRalStep == 0)
						{
								igOffMoveStep =5;
								HAL_SYSALM_SetState(BIT_ALARM_ILLMOVE,	DEF_ENABLE);	// 置熄火移动报警状态
								HAL_AlarmInQueueProcess	(ALARM_BIT_MOVE);	// 打包熄火移动报警报文
								#if(DEF_EVENTINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_EVENT)
										myPrintf("[ALARM]: IG-OFF-Move Enter!\r\n");
								#endif			
						}		
						else
						{
								if(OSTmr_Count_Get(&moveCheckCounter) >= 300000)	// 5分钟超时
								{
										s_ACC.IntFlag1 =0;	// 清中断标志
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
						s_ACC.IntFlag1 =0;	// 清中断标志
						igOffMoveStep =0;	
				}
		}	
		else
		{
				s_ACC.IntFlag1 =0;	// 清中断标志
				if(igOffMoveStep != 0)
				{					
						HAL_SYSALM_SetState(BIT_ALARM_ILLMOVE,	DEF_DISABLE);	// 清熄火移动报警状态
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
		// 振动报警 ///////////////////////////////////////////////////////////////////////////////////	
		// 条件：熄火且有加速度振动产生
		///////////////////////////////////////////////////////////////////////////////////////////////
		if((HAL_SYS_GetState(BIT_SYSTEM_IG) == 0) && (HAL_SYSALM_GetState(BIT_ALARM_POWREMOVE) == 0u))
		{
				if((*(s_devSta.pMems) == DEF_MEMSHARD_NONE_ERR) && (s_ACC.IntFlag2 == 1))
				{
						s_ACC.IntFlag2 =0;	// 清除标志防止再次进入
						if(s_cfg.devEn & DEF_DEVEN_BIT_SHAKE)
						{
								if(OSTmr_Count_Get(&accShakeCounter) >= 60000)
								{
										OSTmr_Count_Start(&accShakeCounter);	// 重新计时
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
		// 报警超速状态 ///////////////////////////////////////////////////////////////////////////////	
		// 条件：
		///////////////////////////////////////////////////////////////////////////////////////////////
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// 报警非法开门状态(外设提供) /////////////////////////////////////////////////////////////////
		// 条件：
		///////////////////////////////////////////////////////////////////////////////////////////////
		if(glob_illOpenFlag)
		{
				glob_illOpenFlag =0;
				if(s_cfg.devEn & DEF_DEVEN_BIT_CON)
						HAL_AlarmInQueueProcess(ALARM_EXT_BIT_ILLOPEN);		// 奥多非法开门报警
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// OTAP 升级报告发送 ///////////////////////////////////////////////////////////////////////////	
		// 条件：当故障码非0时触发,触发后联动触发一次手动体检
		///////////////////////////////////////////////////////////////////////////////////////////////
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// 错误LOG上传 ///////////////////////////////////////////////////////////////////////////////	
		// 条件：GPS,GSM,FLASH,ACC等故障状态上传(上传后清错误变量)
		///////////////////////////////////////////////////////////////////////////////////////////////
		if((s_err.flashTimes)		 	|| (s_err.gsmSimTimes) 		|| (s_err.gsmResetTimes) || (s_err.gpsResetTimes) || 
			 (s_err.obdBusRWTimes) 	|| (s_err.obdParaRWTimes) || (s_err.accTimes) 		 || (s_err.cpuFlashTimes) ||
		   (s_err.conTimes))
		{			
				if(logSendedFlag == 0)
				{
						logSendedFlag =1;	// 设备还未发送过LOG信息
						needSendFlag 	=1;							
				}
				else if(logSendedFlag == 1)
				{
						if(OSTmr_Count_Get(&logUpDelayCounter) >= OS_LOGUP_INTERVAL_TIME)
								needSendFlag =1;
				}
				else
						logSendedFlag =0;
				
				// 打包发送LOG信息
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
						
						OSTmr_Count_Start(&logUpDelayCounter);	// 重新开始间隔计时
				}
		}
		
		///////////////////////////////////////////////////////////////////////////////////////////////
		// 设备上电监控 ///////////////////////////////////////////////////////////////////////////////	
		// 条件：设备上电后如果RTC为默认时间则认为设备可以拔出，当校准RTC时间后上设备上电报文
		///////////////////////////////////////////////////////////////////////////////////////////////
		if(sysPowOnStep == 0)
		{
				BSP_RTC_Get_Current(&tmpRtc);	
				if((tmpRtc.year == Default_year) && (tmpRtc.month == Default_month))
						sysPowOnStep =1;
				else
						sysPowOnStep =3;	// 退出逻辑
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
                OSTmr_Count_Start(&powOnCounter);	// 获取当前偏移量
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
		// 系统参数检测 ///////////////////////////////////////////////////////////////////////////////	
		// 条件：
		///////////////////////////////////////////////////////////////////////////////////////////////
    HAL_CfgChk();
		
		IgonAccJudgeIgoff();
}

/*
******************************************************************************
* Function Name  : IgonAccJudgeIgoff
* Description    : 基于静止9小时判断熄火
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
                    // 熄火事件产生
                    RTCTmr_Count_Start(&s_osTmp.igOffMsgCounter);	 // 熄火间隔数据计时器重新开启	
                    HAL_SYS_SetState(BIT_SYSTEM_ON, DEF_DISABLE);								
                    HAL_SYS_SetState(BIT_SYSTEM_IG, DEF_DISABLE); 
                  
                    #if(DEF_EVENTINFO_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_EVENT)
                    {
                        //myPrintf("[EVENT]: ON-OFF!\r\n");
                        myPrintf("[EVENT]: IG-OFFEXT!\r\n");
                    }
                    #endif
                    // 熄火状态已确定(通过触发条件判断偏移量) /////////////////
                    S_uiIgoffAccFlag = 1u;
                    HAL_IGInQueueProcess(TYPE_IG_OFFNEW,	igOffSetTime);	// 偏移量时间由偏移时间变量确定
                    S_uiIgoffAccFlag = 0u;
                    
                    // 通知obd开始读取车辆状态/////////	
                    if(HAL_SendOBDProProcess(CMD_READ_ALARMSTA,	0,	0,	(u8 *)"",	0) == 0)
                        s_osTmp.obdReadCarStaFlag =1;	// 设置读取车辆状态处理中标志
                    else
                        s_osTmp.obdReadCarStaFlag =0;
                    OSTmr_Count_Start(&s_osTmp.obdReadCarStaCounter);	// 开始读取车辆状态超时统计
                    if(HAL_BSP_GetState(DEF_BSPSTA_GPSSTA) == 1)
                        HAL_GPSInQueueProcess();											// 补充一条GPS位置信息		
                    
                    // 通知ACC任务重新标定设备角度
                    if(glob_accAdjustFlag != 1)
                        glob_accAdjustFlag =1;	// 开始标定
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
* Description    : 判断是否基于静止9小时判断熄火
* Input          : None
* Output         : None
* Return         : 0：非静止9小时判断熄火，1：基于静止9小时判断熄火
******************************************************************************
*/
u8 HAL_GetIgonAccJudgeIgoffFlag(void)
{
    return  S_uiIgoffAccFlag;
}

/*****************************************end*********************************************************/

