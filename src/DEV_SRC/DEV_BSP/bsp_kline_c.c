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
*                                     		bsp_kline_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : bsp_kline_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_KLINE_GLOBLAS
#include	<string.h>
#include	"bsp_h.h"
#include	"bsp_kline_h.h"	 
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

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

//extern	void  OBD_K_Rx_ISQHandler (u8  *rxdData);		// 由OBD实现中断字节的处理

/*
*********************************************************************************************************
*********************************************************************************************************
*                                              BASE FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
/*******************************************************************************
* Function Name  : BSP_K_SW_EN
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_K_SWL_EN(void)
{  	
		GPIO_SetBits(DEF_K_SW_L_PORTS, DEF_K_SW_L_PINS);
}

/*******************************************************************************
* Function Name  : BSP_K_SWL_DIS
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_K_SWL_DIS(void)
{  	
		GPIO_ResetBits(DEF_K_SW_L_PORTS, DEF_K_SW_L_PINS);
}
/*******************************************************************************
* Function Name  : BSP_K_Sleep
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_K_Sleep(void)
{  
		//IO_K_SLEEP_DIS();	
		//IO_K_SLEEP_EN();		 
}
/*******************************************************************************
* Function Name  : BSP_K_Work
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_K_Work(void)
{  
		//IO_K_SLEEP_EN();
		//IO_K_SLEEP_DIS();		 
}
/*******************************************************************************
* Function Name  : BSP_K_Init
* Description    : 
* Input          : 
*									 
* Output         : None
* Return         : 
*******************************************************************************/
void BSP_K_Init(u32	baudRate)
{
		BSP_USART_Init(DEF_K_UART,	DEF_K_REMAP,	baudRate);
}
/*
******************************************************************************
* Function Name  : BSP_KTTL_RxTxISRHandler
* Description    : 
*			
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	BSP_KTTL_RxTxISRHandler (void)
{
    USART_TypeDef  *usart;
		u8  rx_data=0;

#if   (DEF_K_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_K_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_K_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_K_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_K_UART == DEF_UART_5)
		usart = UART5;

#else
		return;
#endif

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
        rx_data = USART_ReceiveData(usart) & 0xFF;              /* Read one byte from the receive data register         */
        //OBD_K_Rx_ISQHandler(&rx_data);

        USART_ClearITPendingBit(usart, USART_IT_RXNE);          /* Clear the DEF_GPS_UART Receive interrupt                   */
    }
		else if (USART_GetITStatus(usart, USART_IT_TXE) != RESET){
        USART_ClearITPendingBit(usart, USART_IT_TXE);           /* Clear the DEF_GPS_UART transmit interrupt  */
    }
		else {
				USART_GetITStatus(usart, USART_IT_RXNE);								// 读SR与DR寄存器来清除其它位置状态如 
				USART_ReceiveData(usart);															  // 如：PE，RXNE，IDLE，ORE，NE，FE，PE
				USART_ClearITPendingBit(usart, USART_IT_RXNE);
		}
}
/*******************************************************************************
* Function Name  : BSP_K_SendPack
* Description    : 
* Input          :
*									 
* Output         : None
* Return         : 
*******************************************************************************/
void  BSP_K_SendData (u8 *buf,	u32	len)
{
		BSP_PutChar (DEF_K_UART,	buf,	len);
}

/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/



/*****************************************end*********************************************************/

