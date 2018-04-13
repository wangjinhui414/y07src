/*
********************************************************************************
File name  : Zxt_bsp.c
Description:
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-11-20   小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/

#define ZXT_OBD_BSP_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

void fun_obd_bsp_init( void )
{
    //暂时这么做后续优化
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );
    //can总线初始化
    fun_obd_can_init();
    fun_obd_Kline_init();
}
#ifdef Y06D
void fun_obd_can_init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2 , ENABLE );

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // Configure CAN1 pin: RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    //Configure CAN1 pin: TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    //Configure CAN2 pin: RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    //Configure CAN2 pin: TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOB, &GPIO_InitStructure );


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    //允许can2
    GPIO_ResetBits( GPIOB, GPIO_Pin_15 | GPIO_Pin_14 );
}
#endif
#ifdef Y07D
void fun_obd_can_init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_CAN1 | RCC_APB1Periph_CAN2 , ENABLE );

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // Configure CAN1 pin: RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    //Configure CAN1 pin: TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    //Configure CAN2 pin: RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    //Configure CAN2 pin: TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOB, &GPIO_InitStructure );


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOE, &GPIO_InitStructure );

    //CAN2 select double line can
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOE, &GPIO_InitStructure );

    GPIO_ResetBits( GPIOE, GPIO_Pin_13 );

    //允许can线
    GPIO_ResetBits( GPIOB, GPIO_Pin_15 );
    GPIO_ResetBits( GPIOE, GPIO_Pin_15 );
}
#endif

void fun_obd_Kline_init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //使能对应外设时钟,串口2用于和手机端通讯,串口3,can1用于和ECU通讯,定时器2用于
    //许多功能
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
    //请看具体功能
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    //B端口浮空管脚配置
    GPIO_InitStructure.GPIO_Pin = ISO14230_KLINE_RXPIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );

    //B端口推挽管脚配置
    GPIO_InitStructure.GPIO_Pin = ISO14230_KLINE_TXPIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );

    GPIO_InitStructure.GPIO_Pin = ISO14230_LLINE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init( GPIOB, &GPIO_InitStructure );

    ISO14230_LLINE_RESET;
    ISO14230_KLINE_SET;
}
/*
********************************************************************************
Name       :void fun_usart_baud_init( void )
Description:1.注意是针对串口波特率进行配置，使能没有进行配置
2.串口配置成8,1,无,能发能收,硬件流=无
3.注意这里使能串口接收中断
Input      :USARTx=串口名称，u32UsartBaud串口波特率
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_usart_baud_init( USART_TypeDef* USARTx, uint32 u32UsartBaud )
{
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = u32UsartBaud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init( USARTx, &USART_InitStructure );
}
/*
********************************************************************************
Name       :fun_can1_baud_init
Description:
Input      :u8CanBaud波特率设置
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_can1_baud_init( uint32 u32CanBaud )
{
    CAN_InitTypeDef        CAN_InitStructure;
    //最好先禁止中断
    //重新初始化默认值
    //CAN_DeInit( CAN_COMMUNICATION_PORT );
    CAN_StructInit( &CAN_InitStructure );

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = u32CanBaud >> 24;
    CAN_InitStructure.CAN_BS1 = ( u32CanBaud >> 16 ) & 0xff;
    CAN_InitStructure.CAN_BS2 = ( u32CanBaud >> 8 ) & 0xff;
    CAN_InitStructure.CAN_Prescaler = u32CanBaud & 0xff;;
    CAN_Init( CAN_COMMUNICATION_PORT, &CAN_InitStructure );
}

/*
********************************************************************************
Name       :fun_can1_baud_init
Description:
Input      :u8CanBaud波特率设置
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_can2_baud_init( uint32 u32CanBaud )
{
    CAN_InitTypeDef        CAN_InitStructure;
    //最好先禁止中断
    //重新初始化默认值
    //CAN_DeInit( CAN_COMMUNICATION_PORT );
    CAN_StructInit( &CAN_InitStructure );

    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = u32CanBaud >> 24;
    CAN_InitStructure.CAN_BS1 = ( u32CanBaud >> 16 ) & 0xff;
    CAN_InitStructure.CAN_BS2 = ( u32CanBaud >> 8 ) & 0xff;
    CAN_InitStructure.CAN_Prescaler = u32CanBaud & 0xff;;
    CAN_Init( CAN_COMMUNICATION_PORT1, &CAN_InitStructure );
}
/*
********************************************************************************
Name       :fun_can1_Filter_init
Description:该函数主要是配置CAN总线滤波器
Input      :u8FilterGroup滤波器组，u32FilterId帧ID
Output     :none
Return     :none
Others     :暂时没有发现问题
********************************************************************************
*/
void fun_can1_Filter_init( uint8 u8FilterGroup, uint32 u32FilterId )
{

    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    CAN_COMMUNICATION_PORT->FA1R &= ~( ( uint32 )0x1 << u8FilterGroup );
    CAN_FilterInitStructure.CAN_FilterNumber = u8FilterGroup;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh =
        ( u32FilterId > 0x7ff ) ? ( ( ( u32FilterId << 3 ) & 0xFFFF0000 ) >> 16 ) : ( ( ( u32FilterId << 21 ) & 0xffff0000 ) >> 16 ) ;
    CAN_FilterInitStructure.CAN_FilterIdLow =
        ( u32FilterId > 0x7ff ) ? ( ( ( u32FilterId << 3 ) | CAN_ID_EXT | CAN_RTR_DATA ) & 0xFFFF ) : ( ( ( u32FilterId << 21 ) | CAN_ID_STD | CAN_RTR_DATA ) & 0xffff );
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xffff;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xffff;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit( &CAN_FilterInitStructure );
}
/*
********************************************************************************
Name       :void fun_time_init(TIM_TypeDef* TIMx)
Description:该函数主要是对定时器进行基本配置
Input      :TIMx初始化哪一个定时器,这个函数是不走中断延时的,得自己查询中断时间
Output     :none
Return     :none
Others     :切记这里没有开始定时器,要自己手动开启
********************************************************************************
*/
void fun_time_init( TIM_TypeDef* TIMx, uint16 u16DelayTime_XXms )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //定时器基本配置
    TIM_TimeBaseStructure.TIM_Period = u16DelayTime_XXms * 10 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIMx, &TIM_TimeBaseStructure );
    //清除溢出中断标志
    TIM_ClearFlag( TIMx, TIM_FLAG_Update );
}

/*
********************************************************************************
Name       :fun_board_to_ecu
Description:该函数是整个系统的接口板给ECU发送数据的任务
Input      :TIMx运用哪一个定时器,u16XxMs延时多少ms,u16TimIt开启哪一个比较中断
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_time_delay_xxms( TIM_TypeDef* TIMx, uint16 u16XxMs, uint16 u16TimIt )
{
    TIM_ClearITPendingBit( TIMx, u16TimIt );
    TIM_SetCompare3( TIMx, TIMx->CNT + u16XxMs * 10 );
    TIM_ITConfig( TIMx, u16TimIt, ENABLE );
}
/*
********************************************************************************
Name       :fun_obd_Monitor_interface_init
Description:该函数主要是初始化串口5
Input      :
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_obd_monitor_interface_init( void )
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_UART5 , ENABLE );
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE );


    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( GPIOC, &GPIO_InitStruct );

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOD, &GPIO_InitStruct );
}

