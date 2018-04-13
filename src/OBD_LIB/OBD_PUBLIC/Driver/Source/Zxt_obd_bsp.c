/*
********************************************************************************
File name  : Zxt_bsp.c
Description:
History    :
�޸�����   : �޸��� : �޸�����:������
2012-11-20   С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/

#define ZXT_OBD_BSP_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

void fun_obd_bsp_init( void )
{
    //��ʱ��ô�������Ż�
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );
    //can���߳�ʼ��
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

    //����can2
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

    //����can��
    GPIO_ResetBits( GPIOB, GPIO_Pin_15 );
    GPIO_ResetBits( GPIOE, GPIO_Pin_15 );
}
#endif

void fun_obd_Kline_init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //ʹ�ܶ�Ӧ����ʱ��,����2���ں��ֻ���ͨѶ,����3,can1���ں�ECUͨѶ,��ʱ��2����
    //��๦��
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
    //�뿴���幦��
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );

    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    //B�˿ڸ��չܽ�����
    GPIO_InitStructure.GPIO_Pin = ISO14230_KLINE_RXPIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );

    //B�˿�����ܽ�����
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
Description:1.ע������Դ��ڲ����ʽ������ã�ʹ��û�н�������
2.�������ó�8,1,��,�ܷ�����,Ӳ����=��
3.ע������ʹ�ܴ��ڽ����ж�
Input      :USARTx=�������ƣ�u32UsartBaud���ڲ�����
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
Input      :u8CanBaud����������
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_can1_baud_init( uint32 u32CanBaud )
{
    CAN_InitTypeDef        CAN_InitStructure;
    //����Ƚ�ֹ�ж�
    //���³�ʼ��Ĭ��ֵ
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
Input      :u8CanBaud����������
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_can2_baud_init( uint32 u32CanBaud )
{
    CAN_InitTypeDef        CAN_InitStructure;
    //����Ƚ�ֹ�ж�
    //���³�ʼ��Ĭ��ֵ
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
Description:�ú�����Ҫ������CAN�����˲���
Input      :u8FilterGroup�˲����飬u32FilterId֡ID
Output     :none
Return     :none
Others     :��ʱû�з�������
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
Description:�ú�����Ҫ�ǶԶ�ʱ�����л�������
Input      :TIMx��ʼ����һ����ʱ��,��������ǲ����ж���ʱ��,���Լ���ѯ�ж�ʱ��
Output     :none
Return     :none
Others     :�м�����û�п�ʼ��ʱ��,Ҫ�Լ��ֶ�����
********************************************************************************
*/
void fun_time_init( TIM_TypeDef* TIMx, uint16 u16DelayTime_XXms )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    //��ʱ����������
    TIM_TimeBaseStructure.TIM_Period = u16DelayTime_XXms * 10 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit( TIMx, &TIM_TimeBaseStructure );
    //�������жϱ�־
    TIM_ClearFlag( TIMx, TIM_FLAG_Update );
}

/*
********************************************************************************
Name       :fun_board_to_ecu
Description:�ú���������ϵͳ�Ľӿڰ��ECU�������ݵ�����
Input      :TIMx������һ����ʱ��,u16XxMs��ʱ����ms,u16TimIt������һ���Ƚ��ж�
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
Description:�ú�����Ҫ�ǳ�ʼ������5
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

