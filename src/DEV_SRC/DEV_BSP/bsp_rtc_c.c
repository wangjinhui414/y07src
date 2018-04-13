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
*                                     			bsp_rtc_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : bsp_rtc_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_RTC_GLOBLAS
#include	"cfg_h.h"
#include	"bsp_h.h"
#include	"bsp_rtc_h.h"
#include 	"ucos_ii.h"

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

static			u8	rtcInitFinish=0;		// RTC初始化完成标志,用于同步后备寄存器的读写操作


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static u8 	 IsLeapYear					(u16 nYear);
static void  BSP_Date_Update    (SYS_DATETIME *pDTime);

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                              RTC FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                     RTCTmr_Count_Start
*
* Description: 以RTC为计时依据即使系统睡眠也可保证准确记时(单位：秒)
*
* Argument(s): Max count 0xFFFFFFFF (秒)
*
* Return(s)  : 
*
* Note(s)    : 
*********************************************************************************************************
*/
void	RTCTmr_Count_Start(vu32	*pSecond)
{
		*pSecond = RTC_GetCounter();			
}
/*
*********************************************************************************************************
*                                     RTCTmr_Count_Get
*
* Description: 以RTC为计时依据即使系统睡眠也可保证准确记时(单位：秒)
*
* Argument(s): Max count 0xFFFFFFFF (秒)
*
* Return(s)  : 
*
* Note(s)    : 
*********************************************************************************************************
*/
u32	RTCTmr_Count_Get(vu32	*pSecond)
{
		vu32	cunRtc=0;

		cunRtc	=	RTC_GetCounter();
		if(cunRtc >= *pSecond)
				return	(cunRtc - *pSecond);
		else
				return	(86399	-	*pSecond + cunRtc);		
}
/*
*********************************************************************************************************
*                                             BSP_RTC_Init()
*
* Description : Initialize the board's RTC
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_RTC_Init(void)
{
		u32 i = 0;
		#if(INFO_OUT_RTC_INIT_EN > 0)
		u8	tmpBuf[60]="";
		#endif

		/* Clear reset flags */
	  RCC_ClearFlag();

		// 这里标志必须跟测试程序一致否则时间被复位成默认
		if (BKP_ReadBackupRegister(BKP_DR1) != RTC_SAVE_FLAG) 
	  {
		    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		    /* Allow access to BKP Domain */
		    PWR_BackupAccessCmd(ENABLE);
				
				/* Backup data register value is not correct or not yet programmed (when
		       the first time the program is executed) */
		
				#if(DEF_RTCINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_RTC)
						myPrintf("[RTC]: RTC not yet configured....\r\n");
				#endif
		
		    /* RTC Configuration */
		    BSP_RTC_Config();
	
				#if(DEF_RTCINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_RTC)
						myPrintf("[RTC]: RTC finish configured....\r\n");
				#endif
		    
				/* Set default time */
				SYS_RTC.year		=		Default_year;
				SYS_RTC.month		=		Default_month;
				SYS_RTC.day			=		Default_day;	
				SYS_RTC.hour		=		Default_hour;
				SYS_RTC.minute	=		Default_minute;
				SYS_RTC.second	=		Default_second;
		
				/* Adjust time by values entred by the user on the hyperterminal */
		    BSP_RTC_Set_Current(&SYS_RTC);
		
				BKP_WriteBackupRegister(BKP_DR1, RTC_SAVE_FLAG);
	  }
	  else
	  {
		    /* Check if the Power On Reset flag is set */
		    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
		    {						
						#if(DEF_RTCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_RTC)
								myPrintf("[RTC]: Power(POR/PDR) reset occurred....\r\n");
						#endif
		    }
		    /* Check if the Pin Reset flag is set */
		    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
		    {
						#if(DEF_RTCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_RTC)
								myPrintf("[RTC]: External reset occurred....\r\n");
						#endif
		    }
				/* Check if the Independent Watchdog Reset flag is set */
		    else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
		    {					
						#if(DEF_RTCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_RTC)
								myPrintf("[RTC]: Independent watchdog reset occurred....\r\n");
						#endif
		    }
				/* Check if the Window Watchdog reset flag is set */
		    else if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
		    {
						#if(DEF_RTCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_RTC)
								myPrintf("[RTC]: Window watchdog reset occurred....\r\n");
						#endif
		    }
				/* Check if the Low Power reset flag is set */
		    else if (RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET)
		    {
						#if(DEF_RTCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_RTC)
								myPrintf("[RTC]: Low power reset occurred....\r\n");
						#endif
		    }
				/* Check if the Software reset flag is set */
		    else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
		    {
						#if(DEF_RTCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_RTC)
								myPrintf("[RTC]: Software reset occurred....\r\n");
						#endif
		    }
				
				#if(DEF_RTCINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_RTC)
						myPrintf("[RTC]: No need to configure RTC....\r\n");
				#endif
		  
		    /* Enable PWR and BKP clocks */
		  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		
		  	/* Allow access to BKP Domain */
		  	PWR_BackupAccessCmd(ENABLE);
				
				/* Wait for RTC registers synchronization */
		    RTC_WaitForSynchro();
		
				/* Wait until last write operation on RTC registers has finished */
		    RTC_WaitForLastTask();
		
		    /* Enable the RTC Second */
		    //RTC_ITConfig(RTC_IT_SEC, ENABLE);								// 不能在系统运行前使能中断 
				//RTC_ITConfig(RTC_IT_ALR, ENABLE);								// 系统闹钟中断
		    /* Wait until last write operation on RTC registers has finished */
		    //RTC_WaitForLastTask();													// 不能在系统运行前使能中断 
		
				/* Initialize Date structure */
		    SYS_RTC.year 	= BKP_ReadBackupRegister(BKP_DR4);
		    SYS_RTC.month	= BKP_ReadBackupRegister(BKP_DR3);
		    SYS_RTC.day 	= BKP_ReadBackupRegister(BKP_DR2);
		      		
				if(RTC_GetCounter() / 86399 != 0)
		    {
			      for(i = 0; i < (RTC_GetCounter() / 86399); i++)
			      {
			        	BSP_Date_Update(&SYS_RTC);
			      }
			     
					 	/* Wait until last write operation on RTC registers has finished */
			  		RTC_WaitForLastTask();
					  RTC_SetCounter(RTC_GetCounter() % 86399);
						/* Wait until last write operation on RTC registers has finished */
			  		RTC_WaitForLastTask();
			
			      BKP_WriteBackupRegister(BKP_DR4, SYS_RTC.year);
			      BKP_WriteBackupRegister(BKP_DR3, SYS_RTC.month);
						BKP_WriteBackupRegister(BKP_DR2, SYS_RTC.day);
		    }
	  }
		/* Clear the RTC Second Interrupt pending bit */      
		RTC_ClearITPendingBit(RTC_IT_SEC);										// 防止系统初始化未完成前进入中断程序
		RTC_ClearFlag(RTC_IT_SEC);
		
		/* Enable  one second interrupe */	
		//RTC_ITConfig(RTC_IT_SEC,	ENABLE);									// 不能在系统运行前使能中断
		rtcInitFinish	=1;		// 设置初始化完成标志
}

/*
*********************************************************************************************************
*                                             BSP_RTC_Config()
*
* Description : Config the board's RTC
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_RTC_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_RTC_Config(void)
{
		u32 counter = 0;
		/* Enable PWR and BKP clocks */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	  /* Allow access to BKP Domain */
	  PWR_BackupAccessCmd(ENABLE);
	
	  /* Reset Backup Domain */
	  BKP_DeInit();
	
	  /* Enable LSE */
	  RCC_LSEConfig(RCC_LSE_ON);
	
	  /* Wait till LSE is ready */
	  while ((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (counter >= REC_EXT_OSC_TIMEOUT))
		{	counter++;	}
		/* If exten 32.768k osc is ready then select LES as RTC clock source */
		if(counter < REC_EXT_OSC_TIMEOUT)	 			
		{
			  /* Select LSE as RTC Clock Source */
			  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
				/* Show the massage */			
				#if(DEF_RTCINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_RTC)
						myPrintf("[RTC]: RTC use the external 32.768k osc!\r\n");
				#endif
		}
		/* Check the internal osc */
		else
		{
				/* Clr counter */
				counter = 0;
				/* Disable LSE */
		  	RCC_LSEConfig(RCC_LSE_OFF);
				/* Enable LSI */
				RCC_LSICmd(ENABLE);  
			  while ((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (counter >= REC_EXT_OSC_TIMEOUT))
				{	counter++;	}
				if(counter < REC_EXT_OSC_TIMEOUT)
				{
						/* Select LSI as RTC Clock Source */
					  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
						/* Show the massage */
						#if(DEF_RTCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_RTC)
								myPrintf("[RTC]: RTC use the internal osc!\r\n");
						#endif
				}
				else
				{
						/* Show the massage */
						#if(DEF_RTCINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_RTC)
								myPrintf("[RTC]: Fatal error low speed osc not work!!\r\n");
						#endif
						/* Dead loop */
						while(1);
				}
		}
	
	  /* Enable RTC Clock */
	  RCC_RTCCLKCmd(ENABLE);
	
	  /* Wait for RTC registers synchronization */
	  RTC_WaitForSynchro();
	
	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
	
	  /* Enable the RTC Second */
	  //RTC_ITConfig(RTC_IT_SEC, ENABLE);										// 不能在系统运行前使能中断
	
	  /* Wait until last write operation on RTC registers has finished */
	  //RTC_WaitForLastTask();															// 不能在系统运行前使能中断
	
	  /* Set RTC prescaler: set RTC period to 1sec */
	  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
	
	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();

}

/*
*********************************************************************************************************
*                                             BSP_RTC_Set_Current()
*
* Description : Set the board's RTC time
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_RTC_Set_Current(SYS_DATETIME *pDTime)
{
		u32 time=0;
	
		time = (pDTime->hour * 3600) + (pDTime->minute * 60) + pDTime->second;
		
	  /* Updata system SYS_RTC variables */
		SYS_RTC.year 		= pDTime->year;
		SYS_RTC.month 	= pDTime->month;	
		SYS_RTC.day 		= pDTime->day;
		SYS_RTC.hour 		= pDTime->hour;
		SYS_RTC.minute 	= pDTime->minute;
		SYS_RTC.second 	= pDTime->second;
	
	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();	
		/* Change the current time */
	  RTC_SetCounter(time);
	  /* Wait until last write operation on RTC registers has finished */
	  RTC_WaitForLastTask();
		/* Write the date */
		BKP_WriteBackupRegister(BKP_DR2, pDTime->day); 
	  BKP_WriteBackupRegister(BKP_DR3, pDTime->month); 
	  BKP_WriteBackupRegister(BKP_DR4, pDTime->year);
}
/*
*********************************************************************************************************
*                                             BSP_RTC_Get_Current()
*
* Description : Get the board's current RTC time & date
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void BSP_RTC_Get_Current (SYS_DATETIME *pDTime)
{
		u32 time = 0;
		time = RTC_GetCounter();
		pDTime->hour 	= (u8)(time / 3600);
		pDTime->minute = (u8)((time % 3600) / 60);
		pDTime->second = (u8)((time % 3600) % 60);
		
		pDTime->year 	= BKP_ReadBackupRegister(BKP_DR4);
		pDTime->month	= BKP_ReadBackupRegister(BKP_DR3);
		pDTime->day 	= BKP_ReadBackupRegister(BKP_DR2);
}
/*
*********************************************************************************************************
*                                             BSP_RTC_Get_Time()
*
* Description : Get the board's current RTC time
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void BSP_RTC_Get_Time (SYS_DATETIME *pDTime)
{
		u32 time = 0;
		time = RTC_GetCounter();
		pDTime->hour 	= (u8)(time / 3600);
		pDTime->minute = (u8)((time % 3600) / 60);
		pDTime->second = (u8)((time % 3600) % 60);

		pDTime->year 	= 0;
		pDTime->month	= 0;
		pDTime->day 	= 0;
}
/*
*********************************************************************************************************
*                                             BSP_Date_Update()
*
* Description : Update RTC date 
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static void BSP_Date_Update(SYS_DATETIME *pDTime)
{
		if(pDTime->month == 1 || pDTime->month == 3 || pDTime->month == 5 || pDTime->month == 7 ||
		   pDTime->month == 8 || pDTime->month == 10 || pDTime->month == 12)
		{
			  if(pDTime->day < 31)
			  {
			    	pDTime->day++;
			  }
			  /* Date structure member: pDTime->day = 31 */
			  else
			  {
				    if(pDTime->month != 12)
				    {
					      pDTime->month++;
					      pDTime->day = 1;
				    }
				    /* Date structure member: date_s.day = 31 & date_s.month =12 */
				    else
				    {
					      pDTime->month = 1;
					      pDTime->day = 1;
					      pDTime->year++;
				    }
			  }
		}
		else if(pDTime->month == 4 || pDTime->month == 6 || pDTime->month == 9 ||
		        pDTime->month == 11)
		{
			  if(pDTime->day < 30)
			  {
			    	pDTime->day++;
			  }
			  /* Date structure member: pDTime->day = 30 */
			  else
			  {
				    pDTime->month++;
				    pDTime->day = 1;
			  }
		}
		else if(pDTime->month == 2)
		{
			  if(pDTime->day < 28)
			  {
			    	pDTime->day++;
			  }
			  else if(pDTime->day == 28)
			  {
				    /* Leap year */
				    if(IsLeapYear(pDTime->year))
				    {
				      	pDTime->day++;
				    }
				    else
				    {
					      pDTime->month++;
					      pDTime->day = 1;
				    }
			  }
			  else if(pDTime->day == 29)
			  {
				    pDTime->month++;
				    pDTime->day = 1;
			  }
		}
}
/*
*********************************************************************************************************
*                                             IsLeapYear()
*
* Description : Check whether the passed year is Leap or not. 
*
* Argument(s) : none.
*
* Return(s)   : 1: leap year
*               0: not leap year
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static u8 IsLeapYear(u16 nYear)
{
	  if(nYear % 4 != 0) return 0;
	  if(nYear % 100 != 0) return 1;
	  return (u8)(nYear % 400 == 0);
}
/*
*********************************************************************************************************
*                                             BSP_RTC_IT_CON()
* Description : 
* Argument(s) : none.
* Return(s)   :               
* Caller(s)   : APP.
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_RTC_IT_CON(u8	type,	u8	newSta)
{	
		if(type == DEF_RTC_SEC_IT)
				RTC_ITConfig(RTC_IT_SEC,	(FunctionalState)newSta);		/* Enable RTC secend inttettupt */	
		else if(type == DEF_RTC_ALARM_IT)
				RTC_ITConfig(RTC_IT_ALR,	(FunctionalState)newSta);		/* Enable RTC alarm inttettupt */
		
		RTC_WaitForLastTask();
}
/*
*********************************************************************************************************
*                                             BSP_RTCEXIT_IT_CON()
* Description : 使能后将产生外部中断
* Argument(s) : none.
* Return(s)   :               
* Caller(s)   : APP.
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_RTCEXIT_IT_CON(u8	newSta)
{	
		EXTI_InitTypeDef EXTI_InitStructure;
		
		/* Configure EXTI Line17(RTC Alarm) to generate an interrupt on rising edge */
		EXTI_ClearITPendingBit(EXTI_Line17);
		EXTI_InitStructure.EXTI_Line 		= EXTI_Line17;
		EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = (FunctionalState)newSta;
		EXTI_Init(&EXTI_InitStructure);
}
/*
*********************************************************************************************************
*                                             BSP_RTCSetAlarmCounter()
* Description : 设置闹钟时间
* Argument(s) : none.
* Return(s)   :               
* Caller(s)   : APP.
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_RTCSetAlarmCounter (u32	seconds)
{
		/* Alarm in n second */
		RTC_SetAlarm(RTC_GetCounter()+ seconds);
		/* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}
/*
*********************************************************************************************************
*                                     BSP_RTCAlarm_ISRHandler()
*
* Description: RTC Alarm interrupts.
*
* Argument(s): none.
*
* Return(s)  : none.
*
* Note(s)    : (1) This ISR handler handles the interrupt entrance/exit as expected by
*                  by uC/OS-II v2.85.  If you are using a different RTOS (or no RTOS), then this
*                  procedure may need to be modified or eliminated.  However, the logic in the handler
*                  need not be changed.
*********************************************************************************************************
*/
void  BSP_RTCAlarm_ISRHandler (void)
{
		rtcWakeUpFlag	=	1;
		
		if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
	  {
		    /* Clear EXTI line17 pending bit */
		    EXTI_ClearITPendingBit(EXTI_Line17);
		
		    /* Check if the Wake-Up flag is set */
		    if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
		    {
			      /* Clear Wake Up flag */
			      PWR_ClearFlag(PWR_FLAG_WU);
		    }
		
		    /* Wait until last write operation on RTC registers has finished */
		    RTC_WaitForLastTask();   
		    /* Clear RTC Alarm interrupt pending bit */
		    RTC_ClearITPendingBit(RTC_IT_ALR);
		    /* Wait until last write operation on RTC registers has finished */
		    RTC_WaitForLastTask();
		}
}
/*
*********************************************************************************************************
*                                     BSP_RTC_ISRHandler()
*
* Description: RTC second interrupts.
*
* Argument(s): none.
*
* Return(s)  : none.
*
* Note(s)    : (1) This ISR handler handles the interrupt entrance/exit as expected by
*                  by uC/OS-II v2.85.  If you are using a different RTOS (or no RTOS), then this
*                  procedure may need to be modified or eliminated.  However, the logic in the handler
*                  need not be changed.
*********************************************************************************************************
*/
void  BSP_RTC_ISRHandler (void)
{
		if (RTC_GetITStatus(RTC_IT_SEC) != RESET )
		{		 
				/* Clear the RTC Second Interrupt pending bit */  
				RTC_ClearITPendingBit(RTC_IT_SEC);

				/* Wait until last write operation on RTC registers has finished */
				RTC_WaitForLastTask();
				
				/* If counter is equal to 86339: one day was elapsed */
				if(RTC_GetCounter() >= 86399)								
				{
				    /* Wait until last write operation on RTC registers has finished */
				    //RTC_WaitForLastTask();
				    /* Reset counter value */
				    ///RTC_SetCounter(0x0);
						RTC_SetCounter((u32)(RTC_GetCounter() - 86399));
				    /* Wait until last write operation on RTC registers has finished */
				    RTC_WaitForLastTask();
						/* Increment the date */
						BSP_Date_Update(&SYS_RTC);
				
						/* Write the date */														 
						BKP_WriteBackupRegister(BKP_DR2, SYS_RTC.day); 	 
					  BKP_WriteBackupRegister(BKP_DR3, SYS_RTC.month); 
					  BKP_WriteBackupRegister(BKP_DR4, SYS_RTC.year);	
				}
				
		}
}
/*
*********************************************************************************************************
*                                     BSP_RTCSoft_ISRHandler()
*
* Description: RTC Soft	interrupts.
*
* Argument(s): none.
*
* Return(s)  : none.
*
* Note(s)    : (1) This ISR handler handles the interrupt entrance/exit as expected by
*                  by uC/OS-II v2.85.  If you are using a different RTOS (or no RTOS), then this
*                  procedure may need to be modified or eliminated.  However, the logic in the handler
*                  need not be changed.
*********************************************************************************************************
*/
void  BSP_RTCSoft_ISRHandler (void)
{				
		/* If counter is equal to 86339: one day was elapsed */	
		if(RTC_GetCounter() >= 86399)								
		{
		    /* Wait until last write operation on RTC registers has finished */
		    //RTC_WaitForLastTask();
		    /* Reset counter value */
		    ///RTC_SetCounter(0x0);
				RTC_SetCounter((u32)(RTC_GetCounter() - 86399));
		    /* Wait until last write operation on RTC registers has finished */
		    RTC_WaitForLastTask();
				/* Increment the date */
				BSP_Date_Update(&SYS_RTC);
		
				/* Write the date */														 
				BKP_WriteBackupRegister(BKP_DR2, SYS_RTC.day); 	 
			  BKP_WriteBackupRegister(BKP_DR3, SYS_RTC.month); 
			  BKP_WriteBackupRegister(BKP_DR4, SYS_RTC.year);	 
		}
}

///*
//*********************************************************************************************************
//*                                             BSP_BACKUP_Write()
//*
//* Description : 将数据存储到后备寄存器中（合法寄存器号为5~42,1~4RTC占用）
//* 						: [注]中小容量产品16，32，64，128k，为10个16位寄存器；大容量产品256，512，1024k为42个16位寄存器
//* Argument(s) : none.
//*
//* Return(s)   : none.
//*
//* Caller(s)   : APP.
//*
//* Note(s)     : none.
//*********************************************************************************************************
//*/
//void 	BSP_BACKUP_Write	(u8 BKP_Num,	u16 sData)
//{
//	  if((BKP_Num >= 5) && (BKP_Num <=42))
//	  {
//				BKP_WriteBackupRegister(BKP_Num, sData); 
//		}
//}
///*
//*********************************************************************************************************
//*                                             BSP_BACKUP_Read()
//*
//* Description : 将数据从后备寄存器中读出（合法寄存器号为5~42,1~4RTC占用）
//*             : [注]中小容量产品16，32，64，128k，为10个16位寄存器；大容量产品256，512，1024k为42个16位寄存器
//* Argument(s) : none.
//*
//* Return(s)   : none.
//*
//* Caller(s)   : APP.
//*
//* Note(s)     : none.
//*********************************************************************************************************
//*/
//void  BSP_BACKUP_Read (u8 BKP_Num,	u16 *sData)
//{	
//		if((BKP_Num >= 5) && (BKP_Num <=42))
//				*sData 	= BKP_ReadBackupRegister(BKP_Num);
//		else
//				*sData  = 0;
//}

/*
*********************************************************************************************************
*                                             BSP_BACKUP_WriteDBGWrod()
*
* Description : 写调试信息屏蔽字(该函数必须在RTC初始化后使用！！！)

* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BACKUP_DBGWrod_SetDft (u16 wData)
{
		BKP_WriteBackupRegister(BKP_DBG_ShieldWord, wData);
}
/*
*********************************************************************************************************
*                                             BSP_BACKUP_WriteDBGWrod()
*
* Description : 写调试信息屏蔽字(该函数必须在RTC初始化后使用！！！)

* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
s8	BSP_BACKUP_WriteDBGWrod (u16 wData)
{
		if(rtcInitFinish == 1)
		{
				BKP_WriteBackupRegister(BKP_DBG_ShieldWord, wData);
				return	DEF_BKP_NONE_ERR;
		}
		else
				return	DEF_BKP_UNINIT_ERR;
}
/*
*********************************************************************************************************
*                                             BSP_BACKUP_ReadDBGWrod()
*
* Description : 写调试信息屏蔽字，如果后备寄存器掉电则以默认值代替(该函数必须在RTC初始化后使用！！！)

* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
s8	BSP_BACKUP_ReadDBGWrod (u16 *rData)
{	
		if(rtcInitFinish == 1)
		{
				if (BKP_ReadBackupRegister(BKP_DR1) != RTC_SAVE_FLAG)
				{
						*rData = DBG_INFO_DFT;
						BKP_WriteBackupRegister(BKP_DBG_ShieldWord, DBG_INFO_DFT);
				}
				else
						*rData 	= BKP_ReadBackupRegister(BKP_DBG_ShieldWord);
				return	DEF_BKP_NONE_ERR;
		}
		else
				return	DEF_BKP_UNINIT_ERR;
}
/*
*********************************************************************************************************
*                                             BSP_BACKUP_ClrIGOnSta()
*
* Description : 清除点火记录状态

* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
s8	BSP_BACKUP_ClrIGOnSta (void)
{
		if(rtcInitFinish == 1)
		{
				BKP_WriteBackupRegister(BKP_IG_Sta, 0x0000);
				return	DEF_BKP_NONE_ERR;
		}
		else
				return	DEF_BKP_UNINIT_ERR;
}
/*
*********************************************************************************************************
*                                             BSP_BACKUP_SaveIGOnSta()
*
* Description : 记录点火状态(0xA8A9)

* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
s8	BSP_BACKUP_SaveIGOnSta (void)
{
		if(rtcInitFinish == 1)
		{
				BKP_WriteBackupRegister(BKP_IG_Sta, 0xA8A9);
				return	DEF_BKP_NONE_ERR;
		}
		else
				return	DEF_BKP_UNINIT_ERR;
}
/*
*********************************************************************************************************
*                                             BSP_BACKUP_LoadIGOnSta()
*
* Description : 读取点火状态

* Argument(s) : none.
*
* Return(s)   : 1为点火,0为熄火
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
s8	BSP_BACKUP_LoadIGOnSta (void)
{	
		if(rtcInitFinish == 1)
		{
				if (BKP_ReadBackupRegister(BKP_DR1) != RTC_SAVE_FLAG)
						return 0;	// 设备掉电不记录状态
				else
				{
						if(BKP_ReadBackupRegister(BKP_IG_Sta) == 0xA8A9)
								return 1;
						else
								return 0;				
				}
		}
		else
				return	0;
}
/*
*********************************************************************************************************
*                                             BSP_BACKUP_UpdateFlagSet()
*
* Description : 生产过程触发本地升级(0xA5A5)或清除升级标志

* Argument(s) : updateWord: 0xA5A5 触发升级，0x0000清除
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
s8	BSP_BACKUP_UpdateFlagSet (u16 updateWord)
{
		if(rtcInitFinish == 1)
		{
				BKP_WriteBackupRegister(BKP_UPDATE_Flag, updateWord);
				return	DEF_BKP_NONE_ERR;
		}
		else
				return	DEF_BKP_UNINIT_ERR;
}
/*
*********************************************************************************************************
*                                             BSP_BACKUP_UpdateFlagSet()
*
* Description : 读取升级标志

* Argument(s) : none.
*
* Return(s)   : 0xA5A5 需要升级否则禁止升级
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
u16	BSP_BACKUP_UpdateFlagGet (void)
{
		if(rtcInitFinish == 1)
		{
				if (BKP_ReadBackupRegister(BKP_DR1) != RTC_SAVE_FLAG)
						return 0;	// 设备掉电不记录状态
				else
						return BKP_ReadBackupRegister(BKP_UPDATE_Flag);
		}
		else
				return	0;
}

/*****************************************end*********************************************************/

