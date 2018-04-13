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
*                                     			bsp_gps_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : bsp_gps_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

// 驱动说明：
// 驱动程序支持符合GPS NMEA 输出格式的所有,单模及多模模块，例如：GPRMC,BDRMC,GLRMC等

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GPS_GLOBLAS
#include	<string.h>
#include	<stdio.h>
#include	"cfg_h.h"
#include	"bsp_h.h"
#include	"bsp_gps_h.h"
#include 	"ucos_ii.h"

#if	(GPS_EN > 0)
/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/********** 内部 GPS I/O 控制接口 *************/

/* Power RTC of the GPS device : rtc pin */
#define	IO_GPS_RTCPOW_EN()					(GPIO_SetBits		(DEF_GPS_RTCPOWEN_PORTS, DEF_GPS_RTCPOWEN_PINS))
#define	IO_GPS_RTCPOW_DIS()					(GPIO_ResetBits	(DEF_GPS_RTCPOWEN_PORTS, DEF_GPS_RTCPOWEN_PINS))

/* Power on the GPS device : pow pin */
#define	IO_GPS_POW_EN()							(GPIO_SetBits		(DEF_GPS_POWEN_PORTS, DEF_GPS_POWEN_PINS))
#define	IO_GPS_POW_DIS()						(GPIO_ResetBits	(DEF_GPS_POWEN_PORTS, DEF_GPS_POWEN_PINS))

/* Enable receive interrupt */
#define	GPS_ENABLE_RES_IT()					(BSP_RS232_RxIntEn	(DEF_GPS_UART))		// include at bsp.h
#define	GPS_DISABLE_RES_IT()				(BSP_RS232_RxIntDis	(DEF_GPS_UART))		// include at bsp.h

/* Enable send interrupt */
#define	GPS_ENABLE_TXD_IT()					(BSP_RS232_TxIntEn	(DEF_GPS_UART))		// include at bsp.h
#define	GPS_DISABLE_TXD_IT()				(BSP_RS232_TxIntDis	(DEF_GPS_UART))		// include at bsp.h

/* Read gps ant state	*/
//#define	IO_GET_GPSANT_STATE()				(GPIO_ReadInputDataBit(DEF_GPS_ANT_PORTS,	DEF_GPS_ANT_PINS))


/********** 内部通用函数接口 *************/

//SOFT_DELAY_MS();																// at	bsp.h	
//void  OSTimeDly (INT16U ticks);									// at os_time.h



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

#if (GPS_SEND_CMD_EN > 0)

uc8	CMD_COOLSTART[]		={0xB5,0x62,0x06,0x04,0x04,0x00,0x00,0x00,0x02,0x00,0x10,0x68}; 		   		//cool start

uc8 CMD_DISPLAY_1HZ[]	={0xB5,0x62,0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00,0x01,0x39}; //1Hz
uc8	CMD_DISPLAY_4HZ[]	={0xB5,0x62,0x06,0x08,0x06,0x00,0xFA,0x00,0x01,0x00,0x01,0x00,0x10,0x96}; //4HZ

uc8	CMD_RMC_SHOW[]		={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x04,0x01,0x01,0x01,0x01,0x05,0x33}; //show GPRMC data
uc8	CMD_RMC_HIDDEN[]	={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x01,0x29}; //hidden GPRMC data

uc8	CMD_GGA_SHOW[]		={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x00,0x01,0x01,0x01,0x01,0x01,0x1F}; //show GPGGA data
uc8	CMD_GGA_HIDDEN[]	={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0xFD,0x15}; //hidden GPGGA data

uc8	CMD_GLL_SHOW[]		={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x01,0x01,0x01,0x01,0x01,0x02,0x24}; //show GPGLL data
uc8	CMD_GLL_HIDDEN[]	={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0xFE,0x1A};	//hidden GPGLL data

uc8	CMD_GSA_SHOW[]		={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x02,0x01,0x01,0x01,0x01,0x03,0x29}; //show GPGSA data
uc8	CMD_GSA_HIDDEN[]	={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0xFF,0x1F}; //hidden GPGSA data

uc8	CMD_GSV_SHOW[]		={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x03,0x01,0x01,0x01,0x01,0x04,0x2E}; //show GPGSV data
uc8	CMD_GSV_HIDDEN[]	={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x24}; //hidden GPGSV data

uc8	CMD_VTG_SHOW[]		={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x05,0x01,0x01,0x01,0x01,0x06,0x38}; //show GPVTG data
uc8	CMD_VTG_HIDDEN[]	={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x02,0x2E}; //hidden GPVTG data

uc8	CMD_GRS_SHOW[]		={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x06,0x01,0x01,0x01,0x01,0x07,0x3D}; //show GPGRS data
uc8	CMD_GRS_HIDDEN[]	={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x06,0x00,0x00,0x00,0x00,0x03,0x33}; //hidden GPGRS data

uc8	CMD_GST_SHOW[]		={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x07,0x01,0x01,0x01,0x01,0x08,0x42}; //show GPGST data
uc8	CMD_GST_HIDDEN[]	={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x07,0x00,0x00,0x00,0x00,0x04,0x38}; //hidden GPGST data

uc8	CMD_ZDA_SHOW[]		={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x08,0x01,0x01,0x01,0x01,0x09,0x47}; //show GPZDA data
uc8	CMD_ZDA_HIDDEN[]	={0xB5,0x62,0x06,0x01,0x06,0x00,0xF0,0x08,0x00,0x00,0x00,0x00,0x05,0x3D}; //hidden GPZDA data

#endif

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

extern		OS_EVENT					*GPSComSem;			// GPS收到一包完整NMEA数据包信号量


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
extern		void	OSTmr_Count_Start							(vu32	*pCounter);		// at main.h
extern		u32		OSTmr_Count_Get								(vu32	*pCounter);		// at main.h

static		void			GpsDelayMS								(u16	nms);
#if (GPS_SEND_CMD_EN > 0)
static		void 			GPS_SendCmd								(uc8 *buf,	u8	len);
#endif

static		void  		GPSRS232_RxHandler 				(u8 rx_data);
static		void  		GPSRS232_TxHandler 				(void);

#if (Rec_XOR_CHECK_EN >0)
static		s8  			PKG_XOR_Check 						(u8 *buf);
#endif
static		s8  			PKG_HEAD_Check 						(u8 *buf);
static		void  		PKG_DATA_Copy  						(u8 *desc,	u8	*sorc,	u16 size);

#if (Rec_GPRMC > 0)
static		s8				Parse_RMC_Pack						(u8 * pSrc);
#endif
#if (Rec_GPGGA > 0)
static		s8				Parse_GGA_Pack						(u8 * pSrc);
#endif
#if (Rec_GPGLL > 0)
static		s8				Parse_GLL_Pack						(u8 * pSrc);
#endif
#if (Rec_GPGSA > 0)
static		s8				Parse_GSA_Pack						(u8 * pSrc);
#endif
#if (Rec_GPGSV > 0)
static		s8				Parse_GSV_Pack						(u8 * pSrc);
#endif
#if (Rec_GPVTG > 0)
static		s8				Parse_VTG_Pack						(u8 * pSrc);
#endif
#if (Rec_GPGRS > 0)
static		s8				Parse_GRS_Pack						(u8 * pSrc);
#endif
#if (Rec_GPGST > 0)
static		s8				Parse_GST_Pack						(u8 * pSrc);
#endif
#if (Rec_GPZDA > 0)
static		s8				Parse_ZDA_Pack						(u8 * pSrc);
#endif
static		void			GPSFeedWDG_Process				(void);		// 初始化过程中需要调用的喂狗操作

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                         GPS FEED WDG
*********************************************************************************************************
*********************************************************************************************************
*/
static	void	GPSFeedWDG_Process (void)
{
		#if(OS_SYSWDG_EN > 0)
		IWDG_ReloadCounter();		// 喂狗
		#endif
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                         GPS POW CONTROL
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                         GPS IT CONTROL
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : GpsDelayMS
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	void	GpsDelayMS	(u16	nms)
{
		if(OSRunning > 0)
				OSTimeDly(nms);
		else
				SoftDelayMS(nms);
}

/*
*********************************************************************************************************
* Function Name  : BSP_GPSPOW_CON
* Description    : 系统休眠时调用        
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GPSPOW_CON(u8	newSta)
{
		if(newSta	==	DEF_ENABLE)
		{
				IO_GPS_POW_EN();
				IO_GPS_RTCPOW_EN();
		}
		else
		{
				//IO_GPS_RTCPOW_DIS();		// 关电时不关闭RTC电源
				IO_GPS_POW_DIS();
				
				// 复位GPS各变量状态
				GPS_PORT.resSta			 	= 0;
				memset(GPS_PORT.rxdBuf, '\0',	DEF_GPS_RBUFSIZE);
			
#if (Rec_GPGGA > 0)			
				memset((u8 *)&GPS_GPRMC,	'\0',	sizeof(GPRMC_Typedef));
				GPS_GPRMC.Sta[0]	=	'V';
				GPS_GPRMC.Sta[1]	=	'\0';
#endif
			
#if (Rec_GPGSA > 0)
				memset((u8 *)&GPS_GPGSA,	'\0',	sizeof(GPGSA_Typedef));
				GPS_GPGSA.FS[0]		=	'1';
				GPS_GPGSA.FS[1]		=	'\0';	
#endif													
		}					
}
/*
*********************************************************************************************************
* Function Name  : BSP_GPS_POWDOWN
* Description    : 用于彻底关闭GPS模块使用，避免不必要电源消耗
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GPS_POWDOWN	(void)
{		
		IO_GPS_RTCPOW_DIS();		
		IO_GPS_POW_DIS();
}
/*
*********************************************************************************************************
* Function Name  : BSP_GPS_POWON
* Description    : 用于彻底关闭GPS模块使用，避免不必要电源消耗
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GPS_POWON	(void)
{			
		IO_GPS_POW_EN();
		IO_GPS_RTCPOW_EN();
}
/*
*********************************************************************************************************
* Function Name  : BSP_GPSRES_IT_CON
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GPSRES_IT_CON(u8	newSta)
{
		if(newSta	==	0)
				GPS_DISABLE_RES_IT();
		else
				GPS_ENABLE_RES_IT();						
}
/*
*********************************************************************************************************
* Function Name  : BSP_GPSTXD_IT_CON
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GPSTXD_IT_CON(u8	newSta)
{
		if(newSta	==	0)
				GPS_DISABLE_TXD_IT();
		else
				GPS_ENABLE_TXD_IT();						
}

/*
*********************************************************************************************************
* Function Name  : BSP_GETGPSANT_STA
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
u8	BSP_GETGPSANT_STA	(void)
{		
		//return	IO_GET_GPSANT_STATE();
		return	0;
}

/*
*********************************************************************************************************
* Function Name  : BSP_GPS_POW_RESET
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GPS_POW_RESET	(u32	delayTime)
{
		GPS_DISABLE_RES_IT();
		IO_GPS_POW_DIS();
		IO_GPS_RTCPOW_DIS();
    GpsDelayMS(delayTime);
		IO_GPS_POW_EN();
		IO_GPS_RTCPOW_EN();
    GpsDelayMS(2000);	
		GPS_ENABLE_RES_IT();							
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                         GPS SYSTEM APP
*********************************************************************************************************
*********************************************************************************************************
*/
#if(GPS_SEND_CMD_EN	> 0 )
/*
*********************************************************************************************************
* Function Name  : GPS_SendCmd
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	void GPS_SendCmd(uc8 *buf,	u8	len)
{
		USART_TypeDef  *usart;
		INT8U      i;

#if   (DEF_GPS_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_GPS_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_GPS_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_GPS_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_GPS_UART == DEF_UART_5)
		usart = UART5;
#endif

	for(i=0; i < len;	i++)
	{
			USART_SendData(usart, *(buf+i));
  		while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);	
	} 	  
}
#endif
/*
*********************************************************************************************************
*                                     BSP_GPSRS232_RxTxISRHandler()
*
* Description: Handle Rx and Tx interrupts.
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
void 	BSP_GPSRS232_RxTxISRHandler (void)
{
    USART_TypeDef  *usart;
		u8     rx_data;

#if   (DEF_GPS_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_GPS_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_GPS_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_GPS_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_GPS_UART == DEF_UART_5)
		usart = UART5;

#else
		return;
#endif

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
        rx_data = USART_ReceiveData(usart) & 0xFF;              /* Read one byte from the receive data register         */
        GPSRS232_RxHandler(rx_data);

        USART_ClearITPendingBit(usart, USART_IT_RXNE);          /* Clear the DEF_GPS_UART Receive interrupt                   */
    }

    else if (USART_GetITStatus(usart, USART_IT_TXE) != RESET) {
        GPSRS232_TxHandler();

        USART_ClearITPendingBit(usart, USART_IT_TXE);           /* Clear the DEF_GPS_UART transmit interrupt  */
    }
		else {
				USART_GetITStatus(usart, USART_IT_RXNE);								// 读SR与DR寄存器来清除其它位置状态如 
				USART_ReceiveData(usart);															  // 如：PE，RXNE，IDLE，ORE，NE，FE，PE
				USART_ClearITPendingBit(usart, USART_IT_RXNE);
		}

}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                            RX & TX HANDLERS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                         GPSRS232_RxHandler()
*
* Description : Handle a received byte.
*
* Argument(s) : rx_data         Received data byte.
*
* Return(s)   : none.
*
* Caller(s)   : Rx ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void  GPSRS232_RxHandler (u8 rx_data)
{ 
		OSTmr_Count_Start(&GPS_PORT.rxdTimer);
		
		if(GPS_PORT.rxLen < DEF_GPS_RBUFSIZE)
		{
				GPS_PORT.rxdBuf[GPS_PORT.rxLen++]	 = rx_data;
		}
}
/*
*********************************************************************************************************
*                                         GPSRS232_TxHandler()
*
* Description : Handle a byte transmission.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Tx ISR.
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void  GPSRS232_TxHandler (void)
{
		;
}

/*
*********************************************************************************************************
*                                         BSP_GPS_TimerServer()
*
* Description : 1ms 检查一次 (GPS接收分包时间服务函数,应在1ms定时器中调用)
* Argument(s) : 
* Return(s)   :	
* Caller(s)   : 
* Note(s)     : none.
*********************************************************************************************************
*/
void	 BSP_GPS_TimerServer (void)
{
		if(GPS_PORT.rxdStep == 0)
		{
				OSTmr_Count_Start(&GPS_PORT.rxdTimer);
				GPS_PORT.rxdStep	=1;
		}		
		else if(GPS_PORT.rxdStep == 1)
		{
				if(OSTmr_Count_Get(&GPS_PORT.rxdTimer) >= DEF_GPS_RXDDELAY_TIMEOUT)	
				{	
						if(GPS_PORT.rxLen	> 0)
						{
								// 通知解析开始
								GPS_PORT.rxdStep =2;	
								GPS_DISABLE_RES_IT();							
								if(OSRunning > 0)
										OSSemPost(GPSComSem);  	// A packet has been received 						
						}
						else
						{
								GPS_PORT.rxLen	=0;
								GPS_PORT.rxdStep =0;	
						}
				}				
		}
}
/*
*********************************************************************************************************
*                                         BSP_GPS_RXBUF_UNLOCK()
*
* Description : 当处理完接收数据时调用使GPS串口重新接收数据包，尽量在1s内释放GPS串口否则会影响下一包数据接收
* Argument(s) : 
* Return(s)   :	
* Caller(s)   : 
* Note(s)     : none.
*********************************************************************************************************
*/
void	 BSP_GPS_RXBUF_UNLOCK (void)
{
		memset((u8 *)&GPS_PORT,		0,	sizeof(GPS_PORT));
		GPS_ENABLE_RES_IT();
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GPS PARSE 
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                         PKG_DATA_Copy()
*
* Description : from srting sorc to srting desc until string sorc have key word ','
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void  PKG_DATA_Copy (u8 *desc,	u8	*sorc,	u16 size)
{
		u8	tmp_i=0;
		for(tmp_i=0;	(*(sorc+tmp_i)!=',') &&  (*(sorc+tmp_i)!='*') && (tmp_i < size);	tmp_i++)
		{
				*(desc+tmp_i) = *(sorc+tmp_i);
		}
		*(desc+tmp_i) = '\0';		// 2015-9-16 修改GPS参数复制错误导致覆盖后面变量问题
}
/*
*********************************************************************************************************
*                                         PKG_HEAD_Check()
*
* Description : Check the type of the gps data pack
*
* Argument(s) : none.
*
* Return(s)   : GPS_RMC_TYPE,GPS_GGA_TYPE,GPS_GLL_TYPE,GPS_GSA_TYPE,GPS_GSV_TYPE,GPS_VTG_TYPE,GPS_GRS_TYPE
*								GPS_GST_TYPE,	GPS_TYPE_ERROR（这里兼容BD及GL模式）
*
* Caller(s)   : 
*               
*
* Note(s)     :
*********************************************************************************************************
*/
static	s8  PKG_HEAD_Check (u8 *buf)
{
		u8 tmp_i=0;
		u8 tmpbuf[10]="";
		for(tmp_i=0;	(*(buf+tmp_i+1)!=',')&&(tmp_i<DEF_GPS_HEADSIZE);	tmp_i++)
		{
				tmpbuf[tmp_i] = *(buf+tmp_i+1);	// 指针跳过'$'字符
		}
		tmpbuf[tmp_i] = '\0';
		
#if(GPS_MULTIMODE_EN > 0)	
		if(strcmp("RMC",	(const char *)(tmpbuf + 2)) == 0)
				return GPS_RMC_TYPE;

		else if(strcmp("GGA",	(const char *)(tmpbuf +2)) == 0)
				return GPS_GGA_TYPE;

		else if(strcmp("GLL",	(const char *)(tmpbuf +2)) == 0)
				return GPS_GLL_TYPE;

		else if(strcmp("GSA",	(const char *)(tmpbuf +2)) == 0)
				return GPS_GSA_TYPE;

		else if(strcmp("GSV",	(const char *)(tmpbuf +2)) == 0)
				return GPS_GSV_TYPE;

		else if(strcmp("VTG",	(const char *)(tmpbuf +2)) == 0)
				return GPS_VTG_TYPE;

		else if(strcmp("GRS",	(const char *)(tmpbuf +2)) == 0)
				return GPS_GRS_TYPE;

		else if(strcmp("GST",	(const char *)(tmpbuf +2)) == 0)
				return GPS_GST_TYPE;

		else if(strcmp("ZDA",	(const char *)(tmpbuf +2)) == 0)
				return GPS_ZDA_TYPE;
#else	
		if(strcmp("GPRMC",	(const char *)tmpbuf) == 0)
				return GPS_RMC_TYPE;

		else if(strcmp("GPGGA",	(const char *)tmpbuf) == 0)
				return GPS_GGA_TYPE;

		else if(strcmp("GPGLL",	(const char *)tmpbuf) == 0)
				return GPS_GLL_TYPE;

		else if(strcmp("GPGSA",	(const char *)tmpbuf) == 0)
				return GPS_GSA_TYPE;

		else if(strcmp("GPGSV",	(const char *)tmpbuf) == 0)
				return GPS_GSV_TYPE;

		else if(strcmp("GPVTG",	(const char *)tmpbuf) == 0)
				return GPS_VTG_TYPE;

		else if(strcmp("GPGRS",	(const char *)tmpbuf) == 0)
				return GPS_GRS_TYPE;

		else if(strcmp("GPGST",	(const char *)tmpbuf) == 0)
				return GPS_GST_TYPE;

		else if(strcmp("GPZDA",	(const char *)tmpbuf) == 0)
				return GPS_ZDA_TYPE;
#endif		
		else 
				return	GPS_ERR_TYPE;		
}
/*
*********************************************************************************************************
*                                         PKG_XOR_Check()
*
* Description : Parse a received packet & formulate a response.
*
* Argument(s) : none.
*
* Return(s)   : the result of xor check, if check ok return GPS_XOR_OK else return GPS_XOR_ERROR
*
* Caller(s)   : 
*               
*
* Note(s)     :
*********************************************************************************************************
*/
#if(Rec_XOR_CHECK_EN >0)

static	s8  PKG_XOR_Check (u8 *buf)
{
		u16 tmp_i=0;
		u8	xor_sum = *(buf+1);
		u8	tmpbuf[3]= {'\0','\0','\0'};
				
		for(tmp_i=2;	(*(buf+tmp_i) != '*')&&(tmp_i < DEF_MAXOF_ONEPACK);	tmp_i++)
		{
				xor_sum = xor_sum ^ (*(buf+tmp_i));	
		}
		if(tmp_i >= DEF_MAXOF_ONEPACK)
				return GPS_ERR_XOR;

		tmp_i = tmp_i + 1;													/*adjust the offset point to the xor char*/
		sprintf((char *)tmpbuf,	"%02X",	xor_sum);			/*transfor hex to char hex*/

		if((tmpbuf[0] == *(buf+tmp_i)) && (tmpbuf[1] == *(buf+tmp_i+1))) 
		{
				return 	GSP_DATA_OK;
		}  
    return GPS_ERR_XOR;
}
#endif


/*
*********************************************************************************************************
* Function Name  : Parse_RMC_Pack
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
#if	(Rec_GPRMC > 0) 
static	s8	Parse_RMC_Pack	(u8 * pSrc)
{
		u16 offset=0;
		u8	index=0;
		
		if(GPS_PORT.resSta & GPS_BIT_RMC)
				return	GPS_ERR_LOCKED;				// 之前已经接收完成一包数据直接返回
		
		for(offset=DEF_GPS_HEADSIZE+1;	*(pSrc+offset)!='*'; offset++)
		{
				if(*(pSrc+offset) == ',')
				{
						switch (index) 
						{
								case 0:
										PKG_DATA_Copy(GPS_GPRMC.Time,	(pSrc+offset+1),	SIZEOF_Time);
								break;
							 	case 1:
										PKG_DATA_Copy(GPS_GPRMC.Sta,	(pSrc+offset+1),	SIZEOF_Sta);
								break;
								case 2:
										PKG_DATA_Copy(GPS_GPRMC.Latitude,	(pSrc+offset+1),	SIZEOF_Latitude);
								break;
								case 3:
										PKG_DATA_Copy(GPS_GPRMC.N_S,	(pSrc+offset+1),	SIZEOF_N_S);
								break;
								case 4:
										PKG_DATA_Copy(GPS_GPRMC.Longitude,	(pSrc+offset+1),	SIZEOF_Longitude);
								break;
								case 5:
										PKG_DATA_Copy(GPS_GPRMC.E_W,	(pSrc+offset+1),	SIZEOF_E_W);
								break;
								case 6:
										PKG_DATA_Copy(GPS_GPRMC.Speed,	(pSrc+offset+1),	SIZEOF_Speed);
								break;
								case 7:
										PKG_DATA_Copy(GPS_GPRMC.Azimuth,	(pSrc+offset+1),	SIZEOF_Azimuth);
								break;
								case 8:
										PKG_DATA_Copy(GPS_GPRMC.Date,	(pSrc+offset+1),	SIZEOF_Date);	
								break;
								case 9:
										PKG_DATA_Copy(GPS_GPRMC.mv,	(pSrc+offset+1),	SIZEOF_mv);	
								break;
								case 10:
										PKG_DATA_Copy(GPS_GPRMC.mvE,	(pSrc+offset+1),	SIZEOF_mvE);	
								break;
								case 11:
										PKG_DATA_Copy(GPS_GPRMC.Mode,	(pSrc+offset+1),	SIZEOF_Mode);
								break;
								default:;
								break;		
						}
						index++;
				} 
		}
		if(index < MEMBER_OF_GRRMC)
		{
				return GPS_ERR_PARSE;
		}
		else
		{	
				GPS_PORT.resSta |= GPS_BIT_RMC;		// 置数据包接收完成数据位
				return	GPS_PARSE_OK;
		}
}
#endif
/*
*********************************************************************************************************
* Function Name  : Parse_GGA_Pack
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
#if	(Rec_GPGGA > 0) 
static	s8	Parse_GGA_Pack	(u8 * pSrc)
{
		u16 offset=0;
		u8	index=0;
		
		if(GPS_PORT.resSta & GPS_BIT_GGA)
				return	GPS_ERR_LOCKED;				// 之前已经接收完成一包数据直接返回
		
		for(offset=DEF_GPS_HEADSIZE+1;	*(pSrc+offset)!='*'; offset++)
		{
				if(*(pSrc+offset) == ',')
				{
						switch (index) 
						{
								case 0:
										PKG_DATA_Copy(GPS_GPGGA.Time,	(pSrc+offset+1),	SIZEOF_Time);
								break;
								case 1:
										PKG_DATA_Copy(GPS_GPGGA.Latitude,	(pSrc+offset+1),	SIZEOF_Latitude);
								break;
								case 2:
										PKG_DATA_Copy(GPS_GPGGA.N_S,	(pSrc+offset+1),	SIZEOF_N_S);
								break;
								case 3:
										PKG_DATA_Copy(GPS_GPGGA.Longitude,	(pSrc+offset+1),	SIZEOF_Longitude);
								break;
								case 4:
										PKG_DATA_Copy(GPS_GPGGA.E_W,	(pSrc+offset+1),	SIZEOF_E_W);
								break;
								case 5:
										PKG_DATA_Copy(GPS_GPGGA.FS,	(pSrc+offset+1),	SIZEOF_FS);
								break;
								case 6:
										PKG_DATA_Copy(GPS_GPGGA.NoSV,	(pSrc+offset+1),	SIZEOF_NoSV);
								break;
								case 7:
										PKG_DATA_Copy(GPS_GPGGA.HDOP,	(pSrc+offset+1),	SIZEOF_HDOP);
								break;
								case 8:
										PKG_DATA_Copy(GPS_GPGGA.MSLHigh,	(pSrc+offset+1),	SIZEOF_MSLHigh);	
								break;
								case 9:
										PKG_DATA_Copy(GPS_GPGGA.MU,	(pSrc+offset+1),	SIZEOF_MU);	
								break;
								case 10:
										PKG_DATA_Copy(GPS_GPGGA.Altref,	(pSrc+offset+1),	SIZEOF_Altref);	
								break;
								case 11:
										PKG_DATA_Copy(GPS_GPGGA.AU,	(pSrc+offset+1),	SIZEOF_AU);	
								break;
								case 12:
										PKG_DATA_Copy(GPS_GPGGA.DiffAge,	(pSrc+offset+1),	SIZEOF_DiffAge);	
								break;
								case 13:
										PKG_DATA_Copy(GPS_GPGGA.DiffStation,	(pSrc+offset+1),	SIZEOF_DiffStation);
								break;
								default:;	
								break;		
						}
				index++;
				} 
		}
		if(index < MEMBER_OF_GRGGA)
		{
				return GPS_ERR_PARSE;
		}
		else
		{
				GPS_PORT.resSta |= GPS_BIT_GGA;		// 置数据包接收完成数据位
				return	GPS_PARSE_OK;
		}
}
#endif
/*
*********************************************************************************************************
* Function Name  : Parse_GLL_Pack
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
#if	(Rec_GPGLL > 0) 
static	s8	Parse_GLL_Pack	(u8 * pSrc)
{
		return	GPS_PARSE_OK;
}
#endif
/*
*********************************************************************************************************
* Function Name  : Parse_GSA_Pack
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
#if	(Rec_GPGSA > 0) 
static	s8	Parse_GSA_Pack	(u8 * pSrc)
{
		u16 offset=0;
		u8	index=0;
		
		if(GPS_PORT.resSta & GPS_BIT_GSA)
				return	GPS_ERR_LOCKED;				// 之前已经接收完成一包数据直接返回
		
		for(offset=DEF_GPS_HEADSIZE+1;	*(pSrc+offset)!='*'; offset++)
		{
				if(*(pSrc+offset) == ',')
				{
						switch (index) 
						{
								case 0:
										PKG_DATA_Copy(GPS_GPGSA.Smode,	(pSrc+offset+1),	SIZEOF_SMODE);
								break;

								case 1:
										PKG_DATA_Copy(GPS_GPGSA.FS,	(pSrc+offset+1),	SIZEOF_FS);
								break;
																
								default:;	
								break;		
						}
						index++;
						if(index	>= MEMBER_OF_GRGSA)	// 只解析2个参数
								break;
				} 
		}
		if(index < MEMBER_OF_GRGSA)
		{
				return GPS_ERR_PARSE;
		}
		else
		{
				GPS_PORT.resSta |= GPS_BIT_GSA;		// 置数据包接收完成数据位
				return	GPS_PARSE_OK;
		}
}
#endif
/*
*********************************************************************************************************
* Function Name  : Parse_GSV_Pack
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
#if	(Rec_GPGSV > 0) 
static	s8	Parse_GSV_Pack	(u8 * pSrc)
{
		return	GPS_PARSE_OK;
}
#endif
/*
*********************************************************************************************************
* Function Name  : Parse_VTG_Pack
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
#if	(Rec_GPVTG > 0) 
static	s8	Parse_VTG_Pack	(u8 * pSrc)
{
		return	GPS_PARSE_OK;
}
#endif
/*
*********************************************************************************************************
* Function Name  : Parse_GRS_Pack
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
#if	(Rec_GPGRS > 0) 
static	s8	Parse_GRS_Pack	(u8 * pSrc)
{
		return	GPS_PARSE_OK;
}
#endif
/*
*********************************************************************************************************
* Function Name  : Parse_GST_Pack
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
#if	(Rec_GPGST > 0) 
static	s8	Parse_GST_Pack	(u8 * pSrc)
{
		return	GPS_PARSE_OK;
}
#endif
/*
*********************************************************************************************************
* Function Name  : Parse_ZDA_Pack
* Description    :          
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
#if	(Rec_GPZDA > 0) 
static	s8	Parse_ZDA_Pack	(u8 * pSrc)
{
		return	GSP_DATA_OK;
}
#endif
/*
*********************************************************************************************************
*                                         BSP_GPS_CheckpackNum()
*
* Description : 检测BUF内有用数据包数目
*
* Argument(s) : none.
*
* Return(s)   :	0 or num
*
* Caller(s)   : 
*               
* Note(s)     : 
*********************************************************************************************************
*/
u8	BSP_GPS_CheckpackNum(u8	*pBuf,	u16 len)
{
		u16	offset=0,inoffset=0;
		u8	ren=0;
			
		for(offset=0;	offset < len; offset++)
		{
				if(*(pBuf + offset) == '$')
				{
						for(inoffset=0;	(offset+inoffset)<len;	inoffset++)
						{
								if((*(pBuf + offset + inoffset) == '\r')&&(*(pBuf + offset + inoffset + 1) == '\n'))
								{
										ren++;
										break;
								}
						}
				}
		}
		return ren;
}
/*
*********************************************************************************************************
* Function Name  : BSP_GPS_ReturnPosition
* Description    : 查询指出的数据包号在数组中的实际位置          
* Input          : 
* Output         : None
* Return         : position or oxffff
*********************************************************************************************************
*/
u16	 BSP_GPS_ReturnPosition (u8 num,	u8 *pBuf,	u16 len)
{
		u16	offset=0;
		u8 	k=0;
		for(offset=0;	offset < len;	offset++)
		{
				if(*(pBuf + offset) == '$')
				{
						k++;
						if(k == num)
						{
								return offset;
						}	
				}
		}
		return 0xffff;
}
/*
*********************************************************************************************************
*                                         BSP_GPS_ParseRxPkt()
*
* Description : Parse a received packet & formulate a response.
*
* Argument(s) : none.
*
* Return(s)   : The number of bytes in the data segment of the packet to transmit in response.
*
* Caller(s)   : 
*               
*
* Note(s)     : modify the GPS_DATA.resSta if ok and return GPS_PARSE_OK else return GPS_XOR_ERROR 
*********************************************************************************************************
*/
s8  BSP_GPS_ParseRxPkt (u8 *pBuf)
{
		s8 	ren = GPS_PARSE_OK;
		u8	datatype= GPS_ERR_TYPE;
			
#if (Rec_XOR_CHECK_EN > 0)		
		if(PKG_XOR_Check(pBuf) == GPS_ERR_XOR)
		{
		 		#if(DEF_GPSINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GPS)
				myPrintf("[GPS]: Pack xor check error!\r\n");
				#endif
				return GPS_ERR_XOR;
		}
#endif
		datatype = PKG_HEAD_Check(pBuf);
		if(datatype == GPS_ERR_TYPE)
		{
				#if(DEF_GPSINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GPS)
				myPrintf("[GPS]: Unknow data pack!\r\n");
				#endif
				return	GPS_ERR_TYPE;
		}
		// 解析数据包数据
		switch(datatype)
		{
				case 0:
						ren = GPS_ERR_PARSE;
				break;
				//-----------------------------------------
				case GPS_RMC_TYPE:
#if	(Rec_GPRMC > 0) 
						ren = Parse_RMC_Pack(pBuf);
#endif
				break;
				//-----------------------------------------
				case GPS_GGA_TYPE:
#if	(Rec_GPGGA > 0)
						ren = Parse_GGA_Pack(pBuf);
#endif
				break;
				//-----------------------------------------
				case GPS_GLL_TYPE:
#if	(Rec_GPGLL > 0)
						ren = Parse_GLL_Pack(pBuf);
#endif
				break;
				//-----------------------------------------
				case GPS_GSA_TYPE:
#if	(Rec_GPGSA > 0)
						ren = Parse_GSA_Pack(pBuf);
#endif
				break;
				//-----------------------------------------
				case GPS_GSV_TYPE:
#if	(Rec_GPGSV > 0)
						ren = Parse_GSV_Pack(pBuf);
#endif
				break;		
				//-----------------------------------------
				case GPS_VTG_TYPE:
#if	(Rec_GPVTG > 0)
						ren = Parse_VTG_Pack(pBuf);
#endif
				break;		
				//-----------------------------------------
				case GPS_GRS_TYPE:
#if	(Rec_GPGRS > 0)
						ren = Parse_GRS_Pack(pBuf);
#endif
				break;
				//-----------------------------------------
				case GPS_GST_TYPE:
#if	(Rec_GPGST > 0)
						ren = Parse_GST_Pack(pBuf);
#endif
				break;
				//-----------------------------------------
				case GPS_ZDA_TYPE:
#if	(Rec_GPZDA > 0)
						ren = Parse_ZDA_Pack(pBuf);
#endif
				break;
				//-----------------------------------------
				default:
						ren = GPS_ERR_PARSE;
				break;		
		}
		return ren;				
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                          GPS Initialize Device
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  : BSP_GPS_CheckDevice
* Description    : Soft receive the GPS pack and to find the key word(',')         
* Input          : 
* Output         : 
* Return         : If have key word then return GPS_OK ,else return GPS_ERROR
*********************************************************************************************************
*/
s8  BSP_GPS_CheckDevice (void)
{
		USART_TypeDef  *usart;		
		u32	timerout=0;

#if   (DEF_GPS_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_GPS_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_GPS_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_GPS_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_GPS_UART == DEF_UART_5)
		usart = UART5;
#endif
	
		timerout =0;		// 时间优化调整版
		while(timerout < DEF_GPS_REC_TIMEROUT)
		{
				timerout++;
				if(USART_GetFlagStatus(usart, USART_FLAG_RXNE) == SET)
				{
						if(USART_ReceiveData(usart) == '$')
								break;
				}
		}
		if(timerout < DEF_GPS_REC_TIMEROUT)	
				return	DEF_GPSHARD_NONE_ERR;
		else
				return DEF_GPSHARD_DESTROY_ERR;													 /*havenot GPS device or module !!*/
}
/*
*********************************************************************************************************
*                                         BSP_GPS_Init()
*
* Description : Init. the gps device and data type
*
* Argument(s) : Modify 		GPS_DEVICE.HardWareSta
*
* Return(s)   :	
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_GPS_Init (void)
{		
		memset((u8 *)&GPS_DEVICE,	0,	sizeof(GPS_DEVICE));		
		memset((u8 *)&GPS_PORT,		0,	sizeof(GPS_PORT));
		#if	(Rec_GPRMC > 0) 
		memset((u8 *)&GPS_GPRMC,	0,	sizeof(GPS_GPRMC));
		#endif
		#if	(Rec_GPGGA > 0)
		memset((u8 *)&GPS_GPGGA,	0,	sizeof(GPS_GPGGA));
		#endif	
		#if	(Rec_GPGSA > 0)
		memset((u8 *)&GPS_GPGSA,	0,	sizeof(GPS_GPGSA));
		#endif
		GPS_DEVICE.HardWareSta 		= DEF_GPSHARD_CHEKING;
		GPS_DEVICE.InitSta        =0;

		/* GPS USART Init */
		BSP_USART_Init(DEF_GPS_UART,	DEF_GPS_REMAP,	DEF_GPS_BAUT_RATE);

		/* Feed WDG */
		GPSFeedWDG_Process();	// 喂狗

		/* Power on the GPS module */
		IO_GPS_POW_DIS();
		IO_GPS_RTCPOW_DIS();	
    GpsDelayMS(1000);
		IO_GPS_POW_EN();
		IO_GPS_RTCPOW_EN();
    GpsDelayMS(100);

		/* Feed WDG */
		GPSFeedWDG_Process();	// 喂狗
		if(BSP_GPS_CheckDevice() == DEF_GPSHARD_DESTROY_ERR)
		{			
				//IO_GPS_POW_DIS();		// 关闭GPS模块电源
				//IO_GPS_RTCPOW_DIS();
				GPS_DEVICE.HardWareSta	=	DEF_GPSHARD_DESTROY_ERR;
				GPS_DEVICE.InitSta      = 0;  // 置模块无效
		}
		else
		{
				GPS_DEVICE.HardWareSta 	= DEF_GPSHARD_NONE_ERR;
				GPS_DEVICE.InitSta			= 1;	// 置模块有效
		}
		
		GPS_DISABLE_RES_IT();
		GPSFeedWDG_Process();	// 喂狗
		
	
//		/* Check GPS device work well or not? */
//		if(BSP_GPS_CheckDevice() == DEF_GPS_DESTROY_ERR)
//		{
//				GPS_DEVICE.HardWareSta	=	DEF_GPS_DESTROY_ERR;
//				IO_GPS_POW_DIS();		// 关闭GPS模块电源
//				IO_GPS_RTCPOW_DIS();
//		}
//		else
//		{
//				GPS_DEVICE.HardWareSta 		= DEF_GPS_NONE_ERR;
//				/* Check	GPS Ant state */
//				//if(IO_GET_GPSANT_STATE()	!= DEF_GPSANT_YES)
//				//		GPS_DEVICE.HardWareSta	=	DEF_GPS_ANTERROR;						
//				
//				/* Send GPS cmd */
//						
//				#if (GPS_SEND_CMD_EN > 0)		
//						GPS_SendCmd(CMD_DISPLAY_1HZ,	sizeof(CMD_DISPLAY_1HZ));		//Speed 1HZ
//				
//				#if (Rec_GPRMC > 0)
//						GPS_SendCmd(CMD_RMC_SHOW,	sizeof(CMD_RMC_SHOW));						//Show RMC
//				#else
//						GPS_SendCmd(CMD_RMC_HIDDEN,	sizeof(CMD_RMC_HIDDEN));			//Hide RMC
//				#endif
//				#if (Rec_GPGGA > 0)
//						GPS_SendCmd(CMD_GGA_SHOW,	sizeof(CMD_GGA_SHOW));						//Show GGA
//				#else
//						GPS_SendCmd(CMD_GGA_HIDDEN,	sizeof(CMD_GGA_HIDDEN));			//Hide GGA
//				#endif
//				#if (Rec_GPGLL > 0)
//						GPS_SendCmd(CMD_GLL_SHOW,	sizeof(CMD_GLL_SHOW));						//Show GLL
//				#else
//						GPS_SendCmd(CMD_GLL_HIDDEN,	sizeof(CMD_GLL_HIDDEN));			//Hide GLL
//				#endif
//				#if (Rec_GPGSA > 0)
//						GPS_SendCmd(CMD_GSA_SHOW,	sizeof(CMD_GSA_SHOW));						//Show GSA	
//				#else
//						GPS_SendCmd(CMD_GSA_HIDDEN,	sizeof(CMD_GSA_HIDDEN));			//Hide GSA
//				#endif
//				#if (Rec_GPGSV > 0)
//						GPS_SendCmd(CMD_GSV_SHOW,	sizeof(CMD_GSV_SHOW));						//Show GSV
//				#else
//						GPS_SendCmd(CMD_GSV_HIDDEN,	sizeof(CMD_GSV_HIDDEN));			//Hide GSV
//				#endif
//				#if (Rec_GPVTG > 0)
//						GPS_SendCmd(CMD_VTG_SHOW,	sizeof(CMD_VTG_SHOW));						//Show VTG
//				#else
//						GPS_SendCmd(CMD_VTG_HIDDEN,	sizeof(CMD_VTG_HIDDEN));			//Hide VTG
//				#endif		
//				#if (Rec_GPGRS > 0)
//						GPS_SendCmd(CMD_GRS_SHOW,	sizeof(CMD_GRS_SHOW));						//Show GRS
//				#else
//						GPS_SendCmd(CMD_GRS_HIDDEN,	sizeof(CMD_GRS_HIDDEN));			//Hide GRS
//				#endif
//				#if (Rec_GPGST > 0)
//						GPS_SendCmd(CMD_GST_SHOW,	sizeof(CMD_GST_SHOW));						//Show GST	
//				#else
//						GPS_SendCmd(CMD_GST_HIDDEN,	sizeof(CMD_GST_HIDDEN));			//Hide GST
//				#endif	
//				#if (Rec_GPZDA > 0)
//						GPS_SendCmd(CMD_ZDA_SHOW,	sizeof(CMD_ZDA_SHOW));						//Show ZDA		
//				#else
//						GPS_SendCmd(CMD_ZDA_HIDDEN,	sizeof(CMD_ZDA_HIDDEN));			//Hide ZDA
//				#endif		
//						
//				#endif
//		}

		#if(DEF_INITINFO_OUTPUTEN > 0)
		if(dbgInfoSwt & DBG_INFO_SYS)
		{
				if(GPS_DEVICE.InitSta == 1)
						myPrintf("[Init]: GPS........(ok)!\r\n");
				else
						myPrintf("[Init]: GPS........(error)!\r\n");
		}
		#endif			
}

#endif

/*****************************************end*********************************************************/


