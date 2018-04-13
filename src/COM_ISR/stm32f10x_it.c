/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* stm32f10x_it.c
* 如果中断需要OS管理,则在进入中断前要加入OSIntNesting++;,退出的时候要OSIntExit(); 
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "bsp_h.h"
#include "bsp_rtc_h.h"
#include "bsp_gsm_h.h"
#include "bsp_gps_h.h"
#include "bsp_can_h.h"
#include "bsp_kline_h.h"
#include "bsp_acc_h.h"
#include "ucos_ii.h"
#include "hal_h.h"
#include "hal_processSYS_h.h"
#include "hal_processDBG_h.h"
#include "hal_processCON_h.h"


/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
	BSP_PutString(DEF_UART_4,	(u8 *)"\r\n[Error]System HardFault_Handler error!!\r\n");
	HAL_CPU_ResetPro();	// 硬件复位前保存当前设备点火状态	
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
	BSP_PutString(DEF_UART_4,	(u8 *)"\r\n[Error]System BusFault_Handler error!!\r\n");
	HAL_CPU_ResetPro();	// 硬件复位前保存当前设备点火状态
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}
/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None 
*******************************************************************************/
//void PendSV_Handler(void)
//{
//}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    :UCOS 系统时钟，
*******************************************************************************/
void SysTick_Handler(void)
{
		CPU_SR         cpu_sr;
		
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
		OSIntNesting++;
		OS_EXIT_CRITICAL();
		}		
		
		//用户程序..
		OSTimeTick();                                /* Call uC/OS-II's OSTimeTick()                       */
		BSP_GPS_TimerServer();
		//HAL_CON_TimerServer();
		
		if(OSRunning > 0){
		OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR          */
		}
}

/*******************************************************************************
* Function Name  : WWDG_IRQHandler
* Description    : This function handles WWDG interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WWDG_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..	
	  
		if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : PVD_IRQHandler
* Description    : This function handles PVD interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PVD_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
		if(EXTI_GetITStatus(EXTI_Line16) != RESET)
	  {
	    	// PVD中断
	    	BSP_PVD_IRQHook();	
	    	// Clear the Key Button EXTI line pending bit
	    	EXTI_ClearITPendingBit(EXTI_Line16);
	  }
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : TAMPER_IRQHandler
* Description    : This function handles Tamper interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TAMPER_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
		if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler    RTC中断
* Description    : RTC中断中，处理好Real_Time的值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : FLASH_IRQHandler
* Description    : This function handles Flash interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : RCC_IRQHandler
* Description    : This function handles RCC interrupt request. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : EXTI0_IRQHandler
* Description    : This function handles External interrupt Line 0 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI0_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		// 用户程序
		if(EXTI_GetITStatus(EXTI_Line0) != RESET)
		{
				// ACCINT1_DET (PB0)
				BSP_ACC_INT1_IRQHook();
				BSP_ACC_INT1_IRQHandler();	// 处理碰撞中断
				// Clear the Key Button EXTI line pending bit 
		    EXTI_ClearITPendingBit(EXTI_Line0);
		}		
	
		if(OSRunning > 0){
	  OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : EXTI1_IRQHandler
* Description    : This function handles External interrupt Line 1 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI1_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		// 用户程序
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External interrupt Line 2 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		// 用户程序
		if(EXTI_GetITStatus(EXTI_Line2) != RESET)
		{	
				// 中断区分
				//if(GPIO_ReadInputDataBit (GPIOC, BSP_GPIOC_GSM_RING) == 0)
				//{
						// Ring_DET (PC2)
						BSP_RING_IRQHook();
						BSP_GSMRing_ISRHandler();				
				//}
//				else
//				{
//						// UART5_RXD_DET (PD2)
//						BSP_UART5RX_IRQHook();	// 同一中断源如果使能会导致误触发				
//				}
				// Clear the Key Button EXTI line pending bit 
		    EXTI_ClearITPendingBit(EXTI_Line2);
		}			
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI3_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		// 用户程序
		if(EXTI_GetITStatus(EXTI_Line3) != RESET)
		{
				// WAKEUP (PC3)
				BSP_ON_IRQHook();
				HAL_IG_IRQHandlers();
				// Clear the Key Button EXTI line pending bit 
		    EXTI_ClearITPendingBit(EXTI_Line3);
		}		
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External interrupt Line 4 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI4_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		// 用户程序
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA Stream 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : DMA1_Channel2_IRQHandler
* Description    : This function handles DMA Stream 2 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel2_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : DMA1_Channel3_IRQHandler
* Description    : This function handles DMA Stream 3 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel3_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : DMA1_Channel4_IRQHandler
* Description    : This function handles DMA Stream 4 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : DMA1_Channel5_IRQHandler
* Description    : This function handles DMA Stream 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : DMA1_Channel6_IRQHandler
* Description    : This function handles DMA Stream 6 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel6_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : DMA1_Channel7_IRQHandler
* Description    : This function handles DMA Stream 7 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : ADC1_2_IRQHandler
* Description    : This function handles ADC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC1_2_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : CAN1_TX_IRQHandler
* Description    : This function handles USB High Priority or CAN TX interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_TX_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..		
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : CAN1_RX0_IRQHandler(该中断服务器函数由OBD来实现)
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void CAN1_RX0_IRQHandler(void)
//{
//		CPU_SR         cpu_sr;

//		if(OSRunning > 0){
//		OS_ENTER_CRITICAL();
//	  OSIntNesting++;
//	  OS_EXIT_CRITICAL();
//		}
//		//用户程序.
//		//BSP_CAN1_RX0_IRQHandler();
//	
//	  if(OSRunning > 0){
//		OSIntExit();
//		}
//}

/*******************************************************************************
* Function Name  : CAN1_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void CAN1_RX1_IRQHandler(void)
//{
//		CPU_SR         cpu_sr;

//		if(OSRunning > 0){
//		OS_ENTER_CRITICAL();
//	  OSIntNesting++;
//	  OS_EXIT_CRITICAL();
//		}
//		//用户程序..

//	  if(OSRunning > 0){
//		OSIntExit();
//		}
//}

/*******************************************************************************
* Function Name  : CAN1_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_SCE_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : CAN2_RX0_IRQHandler(该中断服务器函数由OBD来实现)
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void CAN2_RX0_IRQHandler(void)
//{
//		CPU_SR         cpu_sr;

//		if(OSRunning > 0){
//		OS_ENTER_CRITICAL();
//	  OSIntNesting++;
//	  OS_EXIT_CRITICAL();
//		}
//		//用户程序.
//		//BSP_CAN2_RX0_IRQHandler();

//	  if(OSRunning > 0){
//		OSIntExit();
//		}
//}
/*******************************************************************************
* Function Name  : CAN2_RX1_IRQHandler
* Description    : This function handles CAN RX1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void CAN2_RX1_IRQHandler(void)
//{
//		CPU_SR         cpu_sr;

//		if(OSRunning > 0){
//		OS_ENTER_CRITICAL();
//			OSIntNesting++;
//	  OS_EXIT_CRITICAL();
//		}
//		//用户程序..

//	  if(OSRunning > 0){
//		OSIntExit();
//		}
//}

/*******************************************************************************
* Function Name  : CAN2_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN2_SCE_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}



/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : This function handles External lines 9 to 5 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}

		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : TIM1_BRK_IRQHandler
* Description    : This function handles TIM1 Break interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : TIM1_TRG_COM_IRQHandler
* Description    : This function handles TIM1 Trigger and commutation interrupts 
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : TIM1_CC_IRQHandler
* Description    : This function handles TIM1 capture compare interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler TIM2中断
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void TIM2_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
		
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : I2C1_EV_IRQHandler
* Description    : This function handles I2C1 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : I2C1_ER_IRQHandler
* Description    : This function handles I2C1 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : I2C2_EV_IRQHandler
* Description    : This function handles I2C2 Event interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : I2C2_ER_IRQHandler
* Description    : This function handles I2C2 Error interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : SPI1_IRQHandler
* Description    : This function handles SPI1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_IRQHandler(void)
{
		CPU_SR         cpu_sr;

		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}

}

/*******************************************************************************
* Function Name  : SPI2_IRQHandler
* Description    : This function handles SPI2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
		BSP_GSMRS232_RxTxISRHandler();
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		//用户程序..
		BSP_GPSRS232_RxTxISRHandler();
	
		if(OSRunning > 0){
	  OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : USART3_IRQHandler(该中断服务器函数由OBD来实现)
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void USART3_IRQHandler(void)
//{
//		CPU_SR         cpu_sr;
//	
//		if(OSRunning > 0){
//		OS_ENTER_CRITICAL();
//	  OSIntNesting++;
//	  OS_EXIT_CRITICAL();
//		}
//		//用户程序..
//		//BSP_KTTL_RxTxISRHandler();

//		if(OSRunning > 0){
//	  OSIntExit();
//		}
//}
/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
	
		//用户程序..
		if(EXTI_GetITStatus(EXTI_Line11) != RESET)
		{
				// UART4(DBG) RXD Line (PC11)
				BSP_UART4RX_IRQHook();
				// Clear the Key Button EXTI line pending bit 
		    EXTI_ClearITPendingBit(EXTI_Line11);
		}			
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : RTCAlarm_IRQHandler
* Description    : This function handles RTC Alarm interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
	
		//用户程序..
		BSP_RTCAlarm_ISRHandler();
		
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
	
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : SPI3_IRQHandler
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI3_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
	
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : GPS Module Use.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
	
		//用户程序..		
		HAL_DBGUART_RxTxISRHandler();
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : GSM Module Use.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
		//HAL_CONUART_RxTxISRHandler();
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : GSM Module Use.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : GSM Module Use.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : DMA2_Channel1_IRQHandler
* Description    : GSM Module Use.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel1_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : DMA2_Channel2_IRQHandler
* Description    : GSM Module Use.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel2_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : DMA2_Channel3_IRQHandler
* Description    : GSM Module Use.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel3_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : DMA2_Channel4_IRQHandler
* Description    : GSM Module Use.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel4_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}
/*******************************************************************************
* Function Name  : DMA2_Channel5_IRQHandler
* Description    : GSM Module Use.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA2_Channel5_IRQHandler(void)
{
		CPU_SR         cpu_sr;
	
		if(OSRunning > 0){
		OS_ENTER_CRITICAL();
	  OSIntNesting++;
	  OS_EXIT_CRITICAL();
		}
		
		//用户程序..
	
	  if(OSRunning > 0){
		OSIntExit();
		}
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
