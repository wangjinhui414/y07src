/*******************************************************************************
File name  :zxt_obd_bsp.h
Description:该文件主要是涉及到和硬件相关重新定义
History    :Ver0.1
修改日期   :修改者:修改内容:简单详述
2014/11/19 小老鼠 重新建立硬件部分资源接口
*******************************************************************************/
#ifndef __ZXT_OBD_BSP_H_
#define __ZXT_OBD_BSP_H_

#ifdef    ZXT_OBD_BSP_FUN
#define   ZXT_OBD_BSP_FUN_EXT
#else
#define   ZXT_OBD_BSP_FUN_EXT  extern
#endif
//信息存储标志
#define FLASH_CARINF    0x0801f000                                         //FLASH存储车型和故障地址
#define FLASH_OBDODO    0x0801E800                                         //FLASH存储obd里程

//通讯串口配置
#define KLINE_COMMUNICATION_PORT       USART3
#define CAN_COMMUNICATION_PORT         CAN1
#define CAN_COMMUNICATION_PORT1        CAN2

//通讯管脚配置
#define ISO14230_KLINE_PORT            GPIOB
#define ISO14230_LLINE_PORT            GPIOB
#define ISO14230_KLINE_TXPIN           GPIO_Pin_10
#define ISO14230_KLINE_RXPIN           GPIO_Pin_11
#define ISO14230_LLINE_PIN             GPIO_Pin_1
#define ISO14230_LLINE_SET             GPIO_ResetBits( ISO14230_LLINE_PORT, ISO14230_LLINE_PIN )
#define ISO14230_LLINE_RESET           GPIO_SetBits( ISO14230_LLINE_PORT, ISO14230_LLINE_PIN )
#define ISO14230_KLINE_RESET           GPIO_ResetBits( ISO14230_KLINE_PORT, ISO14230_KLINE_TXPIN )
#define ISO14230_KLINE_SET             GPIO_SetBits( ISO14230_KLINE_PORT, ISO14230_KLINE_TXPIN )



ZXT_OBD_BSP_FUN_EXT void fun_usart_baud_init( USART_TypeDef* USARTx, uint32 u32UsartBaud );
ZXT_OBD_BSP_FUN_EXT void fun_can1_baud_init( uint32 u32CanBaud );
ZXT_OBD_BSP_FUN_EXT void fun_can2_baud_init( uint32 u32CanBaud );
ZXT_OBD_BSP_FUN_EXT void fun_can1_Filter_init( uint8 u8FilterGroup, uint32 u32FilterId );
ZXT_OBD_BSP_FUN_EXT void fun_time_init( TIM_TypeDef* TIMx, uint16 u16DelayTime_XXms );
ZXT_OBD_BSP_FUN_EXT void fun_obd_bsp_init(void);
ZXT_OBD_BSP_FUN_EXT void fun_obd_can_init( void);
ZXT_OBD_BSP_FUN_EXT void fun_obd_Kline_init( void);
ZXT_OBD_BSP_FUN_EXT void fun_obd_monitor_interface_init(void);
#endif
