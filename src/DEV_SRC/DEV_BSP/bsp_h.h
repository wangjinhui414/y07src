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
*                                        BOARD SUPPORT PACKAGE
*
*                                             bsp_h.h
*                                              with the
*                                   			   Y05D Board
*
* Filename      : bsp_h.h
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  BSP_BSP_H_H
#define  BSP_BSP_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_GLOBLAS
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include "stm32f10x.h"
#include "bsp_rtc_h.h"

/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/
/************************COMMON DEFINE***********************/ 
#define	 DEF_ENABLE										(1)
#define	 DEF_DISABLE									(0)
#define	 DEF_SET											(1)
#define	 DEF_CLR											(0)


/************************WDG DEFINE**************************/ 
#define	 SYSWDG_TIMEOUT								(12)						// n*s ����IWG���Ź����ʱ��


/* CPU ID num addr define */
#define  DEF_CPU_FLASHID_ADDR    			(0x1FFFF7E8) 		/*CPU inter flash addr*/

#define  Is_0to9(c) 									((c >= '0') && (c <= '9'))
#define  Is_AtoF(c) 									((c >= 'A') && (c <= 'F'))
#define  Is_atof(c) 									((c >= 'a') && (c <= 'f'))
#define  Is_AtoZ(c) 									((c >= 'A') && (c <= 'Z'))
#define  Is_atoz(c) 									((c >= 'a') && (c <= 'z'))


/************************ISR CHANGE**************************/
//#define	 DEF_DTMF_ST									((u8)0)
//#define	 DEF_POW_REMOVE								((u8)1)

/************************ADC CHANNEL**************************/
#define	 DEF_ADC_LIBAT								((u8)0)	// ����LI���ͨ��	
#define	 DEF_ADC_VBAT									((u8)1)	// �����ⲿ��Դͨ��	
#define	 DEF_ADC_CNN1									((u8)2)	// �����ⲿADͨ��1	
#define	 DEF_ADC_CNN2									((u8)3)	// �����ⲿADͨ��2	


/************************PORT DEFINES**************************/

// ��"stm32f10x.h"�ж��壺GPIO_PortSourceGPIOA 

/************************PIN DEFINES***************************/

// ��"stm32f10x.h"�ж��壺GPIO_PinSource0 

/************************USART ID DEFINES**********************/

#define  DEF_USART_1									(1)
#define  DEF_USART_2									(2)
#define  DEF_USART_3									(3)
#define  DEF_UART_4										(4)
#define  DEF_UART_5										(5)

#define  DEF_USART_1_REMAP						(1)
#define  DEF_USART_2_REMAP						(2)
#define  DEF_USART_3_REMAP_PARTIAL		(3)
#define  DEF_USART_3_REMAP_FULL				(4)
#define	 DEF_USART_REMAP_NOUSE				(0)							

/************************IIC OR SPI BUS TYPE DEFINES***********/
#define	 DEF_IIC_BUS									(0)
#define	 DEF_SPI_BUS									(1)

/************************IIC ID DEFINES************************/

#define  DEF_IIC_1										(1)							
#define  DEF_IIC_2										(2)	
/************************SPI ID DEFINES************************/

#define  DEF_SPI_1										(1)							
#define  DEF_SPI_2										(2)		

/************************CAN ID DEFINES************************/

#define  DEF_CAN_1										(1)							
#define  DEF_CAN_2										(2)		

/************************DEBUG DEFINES************************/
// ������Ϣ�����ʽ����
#define  USE_MDK_WINDOW_OUTPUT				(0)											// ����ʹ��MDK���Դ������Printf�ַ�����Ϣ,���д0��
																															// ͨ������ʵ�ʵĺ궨��Ĵ��ڷ���Printf�ַ�����Ϣ
// ������Ϣ�������
//#define	 DFF_EXTIINFO_OUTPUTEN				(1)											// �ж���Ϣ���ʹ��

// ���Զ˿ڶ���
#define  DEF_DBG_UART									DEF_UART_4
#define	 DEF_DBG_REMAP								DEF_USART_REMAP_NOUSE	
#define	 DEF_DBG_BUAD									115200

/************************SLEEP SWITCH DEFINES******************/
// ˯�߿��ض���
#define	 DEF_SLEEPSWT_ALL							((u8)0xFF)		// �����ж�
#define	 DEF_SLEEPSWT_IG							((u8)0x01)		// ���
#define	 DEF_SLEEPSWT_REMOVE					((u8)0x02)		// �豸�Ƴ�
#define	 DEF_SLEEPSWT_KEY							((u8)0x04)		// ���ܰ���
#define	 DEF_SLEEPSWT_UART						((u8)0x08)		// ���� 
#define	 DEF_SLEEPSWT_RING						((u8)0x10)		// �绰������
#define	 DEF_SLEEPSWT_DIN							((u8)0x20)		// ��������
#define	 DEF_SLEEPSWT_COLLISION				((u8)0x40)		// ��ײ
#define	 DEF_SLEEPSWT_RESERVED				((u8)0x80)		// Ԥ��

/************************DBG INFO SWITCH DEFINES***************/
// ��Ϣϵͳλ����
#define	 DBG_INFO_RTC									((u16)0x8000)	// RTC��Ϣ�������
#define	 DBG_INFO_SYS									((u16)0x4000)	// ϵͳ��ʼ����Ϣ
#define	 DBG_INFO_GSM									((u16)0x2000)	// GSMģ��ײ�ATָ�����Ϣ
#define	 DBG_INFO_GSMSTA							((u16)0x1000)	// GSMģ��״̬��Ϣ��ʵʱ�ź�ǿ��״̬������״̬���£�
#define	 DBG_INFO_GPS									((u16)0x0800)	// GPSģ��MNEAԭʼ������Ϣ
#define	 DBG_INFO_GPSSTA							((u16)0x0400)	// GPSģ�鶨λ״̬��Ϣ������γ�ȸ߶��ٶ���Ϣʵʱ���£�
#define	 DBG_INFO_CON									((u16)0x0200)	// �ⲿ�������ݽ�����Ϣ
#define	 DBG_INFO_OS									((u16)0x0100)	// OSϵͳ��ջ��CPUռ�ö���Ϣ
#define	 DBG_INFO_OBD									((u16)0x0080)	// OBD������Ϣ
#define	 DBG_INFO_ADC									((u16)0x0040)	// AD�ɼ�������Ϣ
#define	 DBG_INFO_ACC									((u16)0x0020)	// ���ٶ�оƬ������Ϣ
#define	 DBG_INFO_GPRS								((u16)0x0010)	// GPRS�շ�������Ϣ
#define	 DBG_INFO_SMS									((u16)0x0008)	// SMS�շ�������Ϣ
#define	 DBG_INFO_BT									((u16)0x0004)	// ���������շ���Ϣ
#define	 DBG_INFO_EVENT								((u16)0x0002)	// ϵͳ�¼���Ϣ
#define	 DBG_INFO_OTHER								((u16)0x0001)	// ϵͳ�����쳣��Ϣ������洢����д�쳣��RTC״̬�ȣ����Ϩ�������߼������
#define	 DBG_INFO_ALL									((u16)0xffff)	// ���е�����Ϣ
#define	 DBG_INFO_DFT									(DBG_INFO_SYS | DBG_INFO_GPRS | DBG_INFO_SMS | DBG_INFO_BT | DBG_INFO_EVENT | DBG_INFO_CON)	// Ĭ�������ʼ����Ϣ��ϵͳ�¼���Ϣ

/************************I/O DEFINES(For GT Series hardware V1.0)****************************/
																											/* -------------------- GPIOA PINS -------------------- */
#define  BSP_GPIOA_GPS_POWEN					GPIO_Pin_0			/* I/O  GPS POWON.																			*/
#define  BSP_GPIOA_AIN                GPIO_Pin_1    	/* POW12V AIN                                    				*/ 
#define  BSP_GPIOA_USART2_TX          GPIO_Pin_2    	/* GPS TXD.                                           	*/
#define  BSP_GPIOA_USART2_RX          GPIO_Pin_3    	/* GPS RXD.                                           	*/
#define  BSP_GPIOA_GPS_RTC_POW				GPIO_Pin_4    	/* I/O  GPS RTC POW.                                 		*/
#define  BSP_GPIOA_SPI1_SCK						GPIO_Pin_5    	/* FLASH SCK.                                      			*/
#define  BSP_GPIOA_SPI1_MISO					GPIO_Pin_6    	/* FLASH MISO.                                     			*/
#define  BSP_GPIOA_SPI1_MOSI					GPIO_Pin_7    	/* FLASH MOSI.                                     			*/
#define  BSP_GPIOA_GSM_POWON					GPIO_Pin_8    	/* I/O  GSM POWON.                                      */
#define  BSP_GPIOA_USART1_TX					GPIO_Pin_9    	/* GSM	TXD.                                           	*/
#define  BSP_GPIOA_USART1_RX					GPIO_Pin_10    	/* GSM	RXD.                                           	*/
#define  BSP_GPIOA_CAN1_RX						GPIO_Pin_11    	/* CAN1 RX. 				                              			*/
#define  BSP_GPIOA_CAN1_TX  					GPIO_Pin_12    	/* CAN1_TX.                               							*/
#define  BSP_GPIOA_JTAG_TMS						GPIO_Pin_13    	/* JTAG TMS/SWDIO.                                      */
#define  BSP_GPIOA_JTAG_TCK						GPIO_Pin_14    	/* JTAG TCK/SWCLK.                                      */
#define  BSP_GPIOA_JTAG_TDI						GPIO_Pin_15    	/* NULL.                                            		*/

                                                      /* -------------------- GPIOB PINS -------------------- */
#define  BSP_GPIOB_NULL0							GPIO_Pin_0    	/* NULL.                                    						*/
#define  BSP_GPIOB_K_SW_L							GPIO_Pin_1    	/* I/O	K SW L.                                   			*/
#define  BSP_GPIOB_CPU_BOOT1					GPIO_Pin_2    	/* CPU  BOOT1.                                          */
#define  BSP_GPIOB_JTAG_TDO						GPIO_Pin_3    	/* NULL.                                            		*/
#define  BSP_GPIOB_JTAG_TRST					GPIO_Pin_4    	/* NULL.                                           			*/
#define  BSP_GPIOB_SP3232EN						GPIO_Pin_5    	/* I/O	SP3232EN                                   			*/
#define  BSP_GPIOB_IIC_SCL						GPIO_Pin_6    	/* IIC  SCL.                                           	*/
#define  BSP_GPIOB_IIC_SDA						GPIO_Pin_7    	/* IIC  SDA.                                       			*/
#define  BSP_GPIOB_LED_DATA						GPIO_Pin_8    	/* I/O 	LED DATA.                          							*/
#define  BSP_GPIOB_LED_RUN						GPIO_Pin_9    	/* I/O 	LED RUN.                                				*/
#define  BSP_GPIOB_USART3_TX					GPIO_Pin_10    	/* K/LIN	TXD.                                          */
#define  BSP_GPIOB_USART3_RX					GPIO_Pin_11    	/* K/LIN	RXD.                                          */
#define  BSP_GPIOB_CAN2_RX						GPIO_Pin_12    	/* CAN2 RX.                                        			*/
#define  BSP_GPIOB_CAN2_TX						GPIO_Pin_13    	/* CAN2 TX.                                       			*/
#define  BSP_GPIOB_CAN2_STB						GPIO_Pin_14    	/* I/O	CAN2 STB.                                      	*/
#define  BSP_GPIOB_CAN1_STB						GPIO_Pin_15    	/* I/O	CAN1 STB.                                      	*/


                                                      /* -------------------- GPIOC PINS -------------------- */
#define  BSP_GPIOC_ACC_INT1						GPIO_Pin_0    	/* ACC 	INT1#.																					*/
#define  BSP_GPIOC_GSM_DTR						GPIO_Pin_1    	/* GSM  DTR.	 																					*/
#define  BSP_GPIOC_GSM_RING					  GPIO_Pin_2    	/* GSM  RING#.	 																				*/
#define  BSP_GPIOC_WAKEUP							GPIO_Pin_3    	/* I/O  WAKEUP#. 																				*/
#define  BSP_GPIOC_FLASH_WP						GPIO_Pin_4    	/* I/O 	FLASH WP.                                      	*/
#define  BSP_GPIOC_FLASH_CS						GPIO_Pin_5    	/* I/O	FLASH CS .                                      */
#define	 BSP_GPIOC_SPAKEREN						GPIO_Pin_6			/* I/O	SPAKER EN .																			*/
#define  BSP_GPIOC_GSM_POWDET					GPIO_Pin_7    	/* I/O	GSM POW DET .                                   */
#define  BSP_GPIOC_GSM_POWEN					GPIO_Pin_8    	/* I/O 	POWER EN.                                       */
#define  BSP_GPIOC_GSM_RESET					GPIO_Pin_9    	/* I/O  GSM RESET.   	                                  */
#define  BSP_GPIOC_UART4_TX						GPIO_Pin_10    	/* DBG TXD                               								*/
#define  BSP_GPIOC_UART4_RX						GPIO_Pin_11    	/* DBG RXD.                                        	  	*/
#define  BSP_GPIOC_UART5_TX						GPIO_Pin_12    	/* CON TXD.                                      	  		*/
#define  BSP_GPIOC_NULL1							GPIO_Pin_13    	/* NULL1.																								*/
#define  BSP_GPIOC_OSC32_IN						GPIO_Pin_14    	/* OSC32.768 IN.                                        */
#define  BSP_GPIOC_OSC32_OUT					GPIO_Pin_15    	/* OSC32.768 OUT.                                       */

                                                      /* -------------------- GPIOD PINS -------------------- */
#define  BSP_GPIOD_OSC_IN							GPIO_Pin_0    	/* OSC  IN                       												*/
#define  BSP_GPIOD_OSC_OUT						GPIO_Pin_1    	/* OSC	OUT                        											*/
#define  BSP_GPIOD_UART5_RX						GPIO_Pin_2    	/* CON RXDL.                                          	*/
#define  BSP_GPIOD_NODEF1							GPIO_Pin_3    	/* No such pin.                                    			*/
#define  BSP_GPIOD_NODEF2							GPIO_Pin_4    	/* No such pin.                                					*/
#define  BSP_GPIOD_NODEF3							GPIO_Pin_5    	/* No such pin.                                 				*/
#define  BSP_GPIOD_NPDEF4							GPIO_Pin_6    	/* No such pin.                                  				*/
#define  BSP_GPIOD_NPDEF5							GPIO_Pin_7    	/* No such pin.                                  				*/
#define  BSP_GPIOD_NPDEF6							GPIO_Pin_8    	/* No such pin. 																				*/
#define  BSP_GPIOD_NPDEF7							GPIO_Pin_9    	/* No such pin.                                 				*/
#define  BSP_GPIOD_NPDEF8							GPIO_Pin_10    	/* No such pin.                                    			*/
#define  BSP_GPIOD_NPDEF9							GPIO_Pin_11    	/* No such pin.                                  				*/
#define  BSP_GPIOD_NPDEF10						GPIO_Pin_12    	/* No such pin.                                   			*/
#define  BSP_GPIOD_NPDEF11						GPIO_Pin_13    	/* No such pin.                                   		 	*/
#define  BSP_GPIOD_NPDEF12						GPIO_Pin_14    	/* No such pin.                               					*/
#define  BSP_GPIOD_NPDEF13						GPIO_Pin_15    	/* No such pin.																					*/

																											/* -------------------- GPIOE PINS -------------------- */
#define  BSP_GPIOE_NODEF1							GPIO_Pin_0    	/* No such pin.                                       	*/
#define  BSP_GPIOE_NODEF2							GPIO_Pin_1    	/* No such pin.                                      		*/
#define  BSP_GPIOE_NODEF3							GPIO_Pin_2			/* No such pin.																					*/
#define  BSP_GPIOE_NODEF4							GPIO_Pin_3			/* No such pin.																					*/
#define	 BSP_GPIOE_NODEF5							GPIO_Pin_4			/* No such pin.																					*/
#define  BSP_GPIOE_NODEF6							GPIO_Pin_5			/* No such pin.																					*/
#define  BSP_GPIOE_NODEF7							GPIO_Pin_6    	/* No such pin.                               					*/
#define  BSP_GPIOE_NODEF8							GPIO_Pin_7    	/* No such pin.																					*/																			
#define  BSP_GPIOE_NODEF9							GPIO_Pin_8    	/* No such pin.																					*/
#define  BSP_GPIOE_NODEF10						GPIO_Pin_9    	/* No such pin.                                    			*/
#define  BSP_GPIOE_NODEF11						GPIO_Pin_10    	/* No such pin.                                   			*/
#define  BSP_GPIOE_NODEF12						GPIO_Pin_11    	/* No such pin.                                     		*/
#define  BSP_GPIOE_NODEF13						GPIO_Pin_12    	/* No such pin.                                     		*/
#define  BSP_GPIOE_NODEF14						GPIO_Pin_13    	/* No such pin.                                     		*/
#define  BSP_GPIOE_NODEF15						GPIO_Pin_14    	/* No such pin.                                    			*/
#define  BSP_GPIOE_NODEF16						GPIO_Pin_15    	/* No such pin.                                      		*/

/**************************************************************************************************************/

// ͨ��I/O���ƶ���

////////////////////////////////////////////////////////////////////////////////////////
// LED_RUN ϵͳ����״ָ̬ʾ�ƿ���
#define IO_LED_RUN_ON()								(GPIO_ResetBits		(GPIOB, BSP_GPIOB_LED_RUN))
#define IO_LED_RUN_OFF()							(GPIO_SetBits			(GPIOB, BSP_GPIOB_LED_RUN))

// LED_DATA ϵͳͨ��״ָ̬ʾ�ƿ���
#define IO_LED_DATA_ON()							(GPIO_ResetBits		(GPIOB, BSP_GPIOB_LED_DATA))
#define IO_LED_DATA_OFF()							(GPIO_SetBits			(GPIOB, BSP_GPIOB_LED_DATA))

// RS232 ��Դʹ�ܿ���
#define IO_RS232_POW_EN()							(GPIO_ResetBits		(GPIOB, BSP_GPIOB_SP3232EN))
#define IO_RS232_POW_DIS()						(GPIO_SetBits			(GPIOB, BSP_GPIOB_SP3232EN))

// TTS ����ʹ�ܿ���
#define IO_SPAKER_PA_EN()							(GPIO_SetBits			(GPIOC, BSP_GPIOC_SPAKEREN))
#define IO_SPAKER_PA_DIS()						(GPIO_ResetBits		(GPIOC, BSP_GPIOC_SPAKEREN))

////////////////////////////////////////////////////////////////////////////////////////
// ������I/O״̬
#define	DEF_WAKEUP_YES								(0)
#define	DEF_WAKEUP_NO									(1)
#define	IO_WAKEUP_DET()								(GPIO_ReadInputDataBit (GPIOC, BSP_GPIOC_WAKEUP))


/*
*********************************************************************************************************
*                                               OTHER DEFINES
*********************************************************************************************************
*/

// ��ֵ�������ڴ�С��ģʽת��(STM32 Ĭ��ΪС��ģʽ����λ�ֽ��ڴ��ַ)
#define REV16(A) ((((u16)(A) & 0xff00) >> 8 ) | (((u16)(A) & 0x00ff) << 8 ))										
#define REV32(A) ((((u32)(A) & 0xff000000) >> 24) | (((u32)(A) & 0x00ff0000) >> 8 ) | (((u32)(A) & 0x0000ff00) << 8 ) | (((u32)(A) & 0x000000ff) << 24))

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// ϵͳӲ��״̬
typedef struct	tag_DeviceSta_Def
{			
		s8			*pFlash;
		s8			*pMems;		//	���ٶȴ�����
		s8			*pGsm;		
		s8			*pGps;
		s8			*pObd;	
		s8			*pCon;		//	����ģ��״̬

}DeviceSta_Typedef;

//////////////////////////////////////////////////////////////////////
// ϵͳ˯�߻������ݽṹ

// ������������
#define	 DEF_WAKEUP_IG								((u16)0x0001)		// ���
#define	 DEF_WAKEUP_REMOVE						((u16)0x0002)		// �豸�Ƴ�
#define	 DEF_WAKEUP_KEY								((u16)0x0004)		// ���ܰ���
#define	 DEF_WAKEUP_UART1							((u16)0x0008)		// ����1(DBG ����)
#define	 DEF_WAKEUP_RING							((u16)0x0010)		// �绰������
#define	 DEF_WAKEUP_DIN								((u16)0x0020)		// ��������
#define	 DEF_WAKEUP_SHAKE							((u16)0x0040)		// ��ײ
#define	 DEF_WAKEUP_UART2							((u16)0x0080)		// ����2(�ⲿ����ģ�� ����)
#define	 DEF_WAKEUP_PVD								((u16)0x1000)		// PVD�ж�

typedef struct	tag_WakeUpSta_Def
{	
		u16			state;		//	ϵͳ˯�߻���״̬

}WakeUpSta_Typedef;


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

BSP_EXT			DeviceSta_Typedef						s_devSta;
BSP_EXT			WakeUpSta_Typedef						s_wakeUp;
BSP_EXT			vu16												dbgInfoSwt;		//	������Ϣ����ȫ�ֱ�����������backup�Ĵ����У�


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                           MAIN SERVICES
*********************************************************************************************************
*/

void				BSP_NVIC_Configuration			(void);

/*
*********************************************************************************************************
*                                           CPU SERVICES
*********************************************************************************************************
*/
void				BSP_CPU_DisAllInterupt			(void);
void				BSP_CPU_Reset								(void);

/*
*********************************************************************************************************
*                                     PERIPHERAL POWER/CLOCK SERVICES
*********************************************************************************************************
*/
void				BSP_RCC_Configuration				(void);
void  			BSP_CPU_GET_ClkFreq 				(RCC_ClocksTypeDef	*clockFreq);
void 				BSP_SYSCLKConfig_FromStop 	(void);

/*
*********************************************************************************************************
*                                           I/O SERVICES
*********************************************************************************************************
*/

void  			BSP_GPIO_Init 							(void);
void				BSP_SYSEXIT_Init 						(void);
void				BSP_SYSEXIT_IT_CON 					(u8	swt,	u8 newSta);
void				BSP_IG_IT_CON 							(u8	newSta);

void				BSP_ON_IRQHook 							(void);
void				BSP_ACC_INT1_IRQHook 				(void);
void				BSP_RING_IRQHook 						(void);
void				BSP_UART4RX_IRQHook					(void);
void				BSP_UART5RX_IRQHook 				(void);


/*
*********************************************************************************************************
*                                              WDT SERVICES
*********************************************************************************************************
*/

void				BSP_WDTDOG_Init							(void);

/*
*********************************************************************************************************
*                                              PVD SERVICES
*********************************************************************************************************
*/
void				BSP_PVDEXIT_IT_CON 					(u8 newSta);
void				BSP_PVD_Init 								(u32 PvdLevel);
u8					BSP_PVD_GetSta 							(void);
void				BSP_PVD_IRQHook 						(void);
/*
*********************************************************************************************************
*                                              ADC SERVICES
*********************************************************************************************************
*/

void				BSP_ADC_CON								  (u8	newSta);
void  			BSP_ADC_Init 								(void);
u16  				BSP_ADC_GetValue 						(u8	channel);

/*
*********************************************************************************************************
*                                              RTC SERVICES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                              LED SERVICES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              PB SERVICES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              USART SERVICES
*********************************************************************************************************
*/
void				BSP_USART_Init							(u8	USART_ID,	u8	REMAP_DEF,	u32 baud_rate);
void  			BSP_RS232_RxIntDis 					(u8	USART_ID);
void  			BSP_RS232_RxIntEn 					(u8	USART_ID);
void  			BSP_RS232_TxIntDis				 	(u8	USART_ID);
void  			BSP_RS232_TxIntEn 					(u8	USART_ID);
void  			BSP_PutChar 								(u8	USART_ID,	u8 *buf,	u32	len);
s8					BSP_OSPutChar 							(u8	USART_ID,	u8 *buf,	u32	len);
void  			BSP_PutString 							(u8	USART_ID,	u8 *buf);
s8					myPrintf 										(const char * format, ...);


/*
*********************************************************************************************************
*                                              SPI SERVICES
*********************************************************************************************************
*/
void				BSP_SPI_Init								(u8	SPI_ID);

/*
*********************************************************************************************************
*                                              SPI SERVICES
*********************************************************************************************************
*/
void				BSP_IIC_Init 								(u8	IIC_ID);

/*
*********************************************************************************************************
*                                              CAN SERVICES
*********************************************************************************************************
*/
void				BSP_CAN_Init								(u8	CAN_ID);

/*
*********************************************************************************************************
*                                             MEMORY SERVICES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             OTHER SERVICES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                           	TOOLS
*********************************************************************************************************
*/
void				GetCpuHwID 									(u8 *idBuf);
void				SoftDelayMS									(u32 Nms);
u8				 	Asc2Hex											(u8	*buf);
u8 					Hex2BCD 										(u8 hex_data);
u16					IsA2ZString 								(u8 *pBuf,	u16	len);
u16					IsIntString 								(u8 *pBuf,	u16	len);
u16					IsHexString 								(u8 *pBuf,	u16	len);
u16					IsDotIntString 							(u8 *pBuf,	u16	len);
u16					StringAdd 									(u8	*pStr,	u8	*pStrF);
u16					String2HexArray 						(u8	*pSrc,	u16	SrcLen,	u8 *pObj);
u16					SNCode2barCode 							(u32 *srcID,	u32 *objID);
s8					CheckPhoneFormat 						(u8 *pSrc,	u16 srcLen,	u8	*pObj1,	u16 *obj1Len,	u8	*pObj2,	u16 *obj2Len, u8	*pObj3,	u16 *obj3Len);
s8					CheckIsNum 									(u8 *pBuf,	u16 len);
u16					ADCalculateProcess					(u16 *pBuf,	u16 len); 
u16					MakeStrNum2USC2 						(u8 *pSrc,	u16 srcLen,	u8	*pObj);
u16					StringConnect 							(u8	*pStr,	u8	*pStrF,		u8	*pStrB); 
u16 				StringSearch								(u8 *pSource,u8 *pObject,u16 SrcLen,u16 ObjLen);
u32 				Mktime 											(u32 year, u32 mon,u32 day, u32 hour,u32 min, u32 sec);
void 				Gettime 										(u32 count,	SYS_DATETIME *cnt);
u16					Hex2StringArray 						(u8	*pSrc,	u16	SrcLen,	u8 *pObj);
u16	 				SumCheck16Pro 							(u8 *srcBuf,	u16	srcLen);

/*
*********************************************************************************************************
*                                           	EXTI 
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           	OTHER 
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
