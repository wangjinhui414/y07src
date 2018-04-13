/*
********************************************************************************
File name   : ZXT_create_link.c
Description: ���ļ�����ϵͳ�����õĺ���,���Բ����������������Ͷ���ȫ�ֱ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ����ͨѶ����С����
����       :ÿһ�ּ������һ��������,��Щ���������Ǿ������ò��ܽ���������
********************************************************************************
*/
#define CREATE_LINK_FUN //��������
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Function   :void *pfun_active_active_low_high_xxms(void *pArg0,void *pArg1)
Description:�ú�����Ҫ�Ǹߵ͵�ƽxxMS,��Ҫ�ǿ��ǵ���ЩECU���Ǳ�׼�ĸߵ͵�
ƽ���ԲŽ���������
Input      :void *pArg0,void *pArg1��2��������û������
Output     :pCommStatusͨѶ״̬�ɹ����ľ����ַ
Return     :pCommStatusͨѶ״̬�ɹ����ľ����ַ
Others     :
********************************************************************************
*/

void *pfun_active_low_high_xxms( void *pArg0 , void *pu8FunctionParameter )
{

    GPIO_InitTypeDef GPIO_InitStructure;

    void *pCommStatus;
    const TYPEDEF_ISO14230_ACTIVE_PAR *pIso14230ActiveLib;
    uint8 u8RetransCounter = 0;
    pIso14230ActiveLib = &g_stIso14230ActiveLib[( ( uint8* )pu8FunctionParameter )[0]];
    u8RetransCounter = pIso14230ActiveLib->u8RetransTime;

    do
    {

        RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, DISABLE );
        //ʹ��L��
        ISO14230_LLINE_SET;
        KLINE_COMMUNICATION_PORT->CR1 &= ~0X4;
        GPIO_InitStructure.GPIO_Pin  = ISO14230_KLINE_TXPIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );
        //��������1s
        ISO14230_KLINE_SET;
        OSTimeDlyHMSM( 0, 0, 1, 0 );

        ISO14230_KLINE_RESET;
        OSTimeDlyHMSM( 0, 0, 0, pIso14230ActiveLib ->u8LowTime );
        ISO14230_KLINE_SET;
        OSTimeDlyHMSM( 0, 0, 0, pIso14230ActiveLib ->u8HighTime );

        RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE );
        fun_usart_baud_init( KLINE_COMMUNICATION_PORT, pIso14230ActiveLib->u16BaudRate );
        USART_ITConfig( KLINE_COMMUNICATION_PORT, USART_IT_RXNE, ENABLE );
        USART_Cmd( KLINE_COMMUNICATION_PORT, ENABLE );
        //���³�ʼ��ΪĬ��ֵ
        GPIO_InitStructure.GPIO_Pin   = ISO14230_KLINE_TXPIN;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );
        //�������
        KLINE_COMMUNICATION_PORT->CR1 |= 0X4;
        //���ͽ�������
        pCommStatus = ppublicfun_send_command_index( ( uint8 * )g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter, ( void* ) 0 );
        //���½�L�����ظߵ�ƽ
        ISO14230_LLINE_RESET;

        //�������3�λ���ͨѶ�ɹ������˳�ȥ
        if( ( *( TYPEDEF_FRAME_STATUS* ) pCommStatus == OK ) | ( ( --u8RetransCounter ) == 0 ) )
        {
            return ( pCommStatus );
        }
    }
    while( 1 );

};
/*
********************************************************************************
Name        :pfun_active_can_bus
Description:�ú�����Ҫ�Ƕ�ͨѶ���߽��г�ʼ���ͷ��ͽ�������
Input      :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :�ú�����Ҫ��Ӧ�Գ�����CAN����ͨѶ
����ͨѶ������
********************************************************************************
*/
#if CREATE_LINK_CAN_BUS_EN == 1

void *pfun_active_can_bus( void *pArg0 , void *pu8FunctionParameter )
{

    void *pCommStatus;
    const TYPEDEF_ISO15765_ACTIVE_PAR *pSTRUCT_Iso15765ActiveLib;
    //ֱ��ָ��15765ͨѶ��
    pSTRUCT_Iso15765ActiveLib = &g_stIso15765ActiveLib[( ( uint8* )pu8FunctionParameter )[0]];

    //�������K���ж�
    USART_ITConfig( KLINE_COMMUNICATION_PORT, USART_IT_RXNE, DISABLE );
    //CANͨѶ����������
	 	if( g_pModeLibTab->pCmdTabChoice[g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter[1]].u8FilterGroup <14 )
	    fun_can1_baud_init( pSTRUCT_Iso15765ActiveLib->u32BaudRate );
		else
			fun_can2_baud_init( pSTRUCT_Iso15765ActiveLib->u32BaudRate );;
    //���ͽ�������
    //�����ڵ�fun_board_to_ecu����ص������������ж��ǲ����Ѿ���ʱ
    pCommStatus = ppublicfun_send_command_index( ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter, ( void* ) 0 );
    return ( pCommStatus );
};

#endif


/*
*******************************************************************************
Function   :pfun_active_5baud_negate_last_byte_back_addr
Description:��ַ�뼤�ʽ0(
ECU����ϵ�3���ֽ�֮��
������ȡ����3���ֽ�
ECUȡ����ַ��
���������ͽ�������
)
Input      :void *pArg0
pu8FunctionParameterѡ�������һ��ͨѶ��ʼ��������
Output     :pCommStatusͨѶ״̬
Return     :pCommStatusͨѶ״̬
Others     :1.����ִ�е�ַ�뼤��(1.0���͵�ַ��
1.1�ȴ�500MS����3���ֽ�
1.2�ȴ�һ��ʱ��(���ʱ���ǿ��Կ��Ƶ�)�������һ���ֽ�ȡ��
1.3�ȴ�500ms����1���ֽ�,������յ��ǵ�ַ��ȡ�����ֽ���ô��ִ��1.5
1.4������յ��Ĳ��Ƕ�Ӧ�ĵ�ַ����ô֮������ȥ
1.5��ʱһ��ʱ�䷢�ͽ�������(��ʱ���ǿ��Կ��Ƶ�),��Ҫ����Ӧ��һЩ�Ƚ�����    ��ECU
)
********************************************************************************
*/
#if ACTIVE_5BAUD_ONLY_LAST_BYTE_EN == 1
void *pfun_active_5baud_negate_last_byte_back_addr( void *pArg0 , void *pu8FunctionParameter )
{


    const TYPEDEF_ISO9141_ACTIVE_PAR *pIso9141ActiveLib;
    void *pCommStatus;
    uint8 u8LastByte = 0;
    uint8 u8activecounter = 0;
    pIso9141ActiveLib = &g_stIso9141ActiveLib[( ( uint8* )pu8FunctionParameter )[0]];


    //��������ʼ���ʧ��ֱ����������,����������¼���
    while( 1 )
    {
        //����ɹ���������
        if( bfun_active_5baud_init( &u8LastByte, ( ( uint8* )pu8FunctionParameter )[0] ) )
        {
            break;
        }

        //�������·��ʹ���
        if( ++u8activecounter == pIso9141ActiveLib->u8RetransTime )
        {
            return ( ( void * )&g_enumFrameFlgArr[0] );
        }

        //����һ������ʱ,�����һ�μ���ɹ��ȴ�һ��ʱ���ٴμ���
        else
        {
            OSTimeDlyHMSM
            (
                0,
                0,
                ( pIso9141ActiveLib->u16ProtocolReversed )[2] / 1000,
                ( pIso9141ActiveLib->u16ProtocolReversed )[2] % 1000
            );
        }

    }

    //���ʱ���ǿ��Կ��Ƶ�,�ø���ʵ�ʵ�ECU������,��Щ����Ҫ���Ϳ�һ�㣬��Щ����Ҫ������һ��
    OSTimeDlyHMSM( 0, 0, 0, ( pIso9141ActiveLib->u16ProtocolReversed )[0] );

    KLINE_COMMUNICATION_PORT->CR1 &= ~0X4;

    //�����һ���ֽ�ȡ��
    USART_SendData( KLINE_COMMUNICATION_PORT, ~u8LastByte );

    while( USART_GetFlagStatus( KLINE_COMMUNICATION_PORT, USART_FLAG_TC ) == RESET );

    //���յ�ַ��ȡ���ֽ�
    KLINE_COMMUNICATION_PORT->CR1 |= 0X4;

    //ע������������ʱ��������1���ؼ��ֽ�,��ʱʱ����0.5s
    fun_time_init( TIM2, 500 );
    //������ʱ��
    TIM2->CR1 |= 1;

    while( 1 )
    {
        //�ж��Ƿ������ݵĵ���,�оͽ�ȥ
        if( USART_GetFlagStatus( KLINE_COMMUNICATION_PORT, USART_FLAG_RXNE ) != RESET )
        {
            //�жϽ��յ��������ǲ��ǵ�ַ��ȡ���ֽ�
            if( ( pIso9141ActiveLib->u8AddCode ) != ( uint8 )( ~USART_ReceiveData( KLINE_COMMUNICATION_PORT ) ) )
            {
                return ( ( void* )&g_enumFrameFlgArr[0] );
            }

            else
            {
                //�رն�ʱ��
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                break;
            }
        }

        //�ж��Ƿ��Ѿ���ʱ
        if( ( TIM2->SR & ( uint16 ) 1 ) == 1 )
        {
            //�رն�ʱ��
            TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
            return ( ( void * )&g_enumFrameFlgArr[0] );
        }
    }

    //��һ��ʱ��Ȼ���ͽ�������
    OSTimeDlyHMSM( 0, 0, 0, ( pIso9141ActiveLib->u16ProtocolReversed )[1] );
    //���������ж�
    USART_ITConfig( KLINE_COMMUNICATION_PORT, USART_IT_RXNE, ENABLE );

    pCommStatus = ppublicfun_send_command_index( ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter, ( void* ) 0 );

    return ( pCommStatus );
}

#endif

/*
*******************************************************************************
Function   :pfun_active_5baud_kwp1281
Description:��ַ�뼤�ʽ1(
ECU����ϵ�3���ֽ�֮��
������ȡ����3���ֽ�
ECU����1281Э�鷢����Ӧ����
���������ͽ�������
)
Input      :void *pArg0
pu8FunctionParameterѡ�������һ��ͨѶ��ʼ��������
Output     :pCommStatusͨѶ״̬
Return     :pCommStatusͨѶ״̬
Others     :1.����ִ�е�ַ�뼤��(1.0���͵�ַ��
1.1�ȴ�500MS����3���ֽ�
1.2�ȴ�һ��ʱ��(���ʱ���ǿ��Կ��Ƶ�)�������һ���ֽ�ȡ��
1.3�ȴ�500ms����1֡����,���ʧ��ֱ������ȥ
1.5��ʱһ��ʱ�䷢�ͽ�������(��ʱ���ǿ��Կ��Ƶ�),��Ҫ����Ӧ��һЩ�Ƚ������ECU
)
********************************************************************************
*/
#if ACTIVE_5BAUD_KWP1281_EN == 1
void *pfun_active_5baud_kwp1281( void *pArg0 , void *pu8FunctionParameter )
{


    const TYPEDEF_ISO9141_ACTIVE_PAR *pIso9141ActiveLib;
    void *pCommStatus;
    uint8 u8LastByte = 0;
    uint8 u8activecounter = 0;
    pIso9141ActiveLib = &g_stIso9141ActiveLib[( ( uint8* )pu8FunctionParameter )[0]];

    //��������ʼ���ʧ��ֱ����������,����������¼���
    while( 1 )
    {
        //����ɹ���������
        if( bfun_active_5baud_init( &u8LastByte, ( ( uint8* )pu8FunctionParameter )[0] ) )
        {
            break;
        }

        //�������·��ʹ���
        if( ++u8activecounter == pIso9141ActiveLib->u8RetransTime )
        {
            return ( ( void* )&g_enumFrameFlgArr[0] );
        }

        //����һ������ʱ,�����һ�μ���ɹ��ȴ�һ��ʱ���ٴμ���
        else
        {
            OSTimeDlyHMSM
            (
                0,
                0,
                ( pIso9141ActiveLib->u16ProtocolReversed )[2] / 1000,
                ( pIso9141ActiveLib->u16ProtocolReversed )[2] % 1000
            );
        }
    }

    //���ʱ���ǿ��Կ��Ƶ�,�ø���ʵ�ʵ�ECU������,��Щ����Ҫ���Ϳ�һ�㣬��Щ����Ҫ������һ��
    OSTimeDlyHMSM( 0, 0, 0, ( pIso9141ActiveLib->u16ProtocolReversed )[0] );

    KLINE_COMMUNICATION_PORT->CR1 &= ~0X4;

    //�����һ���ֽ�ȡ��
    USART_SendData( KLINE_COMMUNICATION_PORT, ~u8LastByte );

    while( USART_GetFlagStatus( KLINE_COMMUNICATION_PORT, USART_FLAG_TC ) == RESET );

    //���������ٵ�����
    g_pstEcuToPerformer->OSEventPtr = ( void * ) 0;
    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
    //ע�����ֵ������ϵͳ��ֻ�ǳ�ʼ��һ��
    g_stUsart3Iso14230ReceiveVar.u8SentCounter = 0;
    g_stUsart3Iso14230ReceiveVar.bFrameError = FALSE;
		
											g_pModeLibTab_lib = g_pModeLibTab;
										g_unnu32FrameIdLib_lib = g_unnu32FrameIdLib;
										g_unnu32FilterId_lib = g_unnu32FilterId;
										g_stIsoKlineCommLib_lib = g_stIsoKlineCommLib;
										g_stIso15765ActiveLib_lib = g_stIso15765ActiveLib;

    //����һ֡����
    if( OK != *( TYPEDEF_FRAME_STATUS* )pfun_iso_1281_receive( ( void* ) 0, ( void* ) 0 ) )
    {
        return ( ( void * )&g_enumFrameFlgArr[0] );
    }

    //��һ��ʱ��Ȼ���ͽ�������
    OSTimeDlyHMSM( 0, 0, 0, ( pIso9141ActiveLib->u16ProtocolReversed )[1] );
    pCommStatus = ppublicfun_send_command_index( ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter, ( void* ) 0 );

    return ( pCommStatus );
}

#endif
/*
*******************************************************************************
Function   :bfun_active_5baud_init
Description:�ú�����Ҫ��5�����ʼ����,
1.���͵�ַ��
2.Ȼ����Ž���3���ֽ�
3.���سɹ����
Input      :void
Output     :true(�ɹ�) false(ʧ��)
Return     :true(�ɹ�) false(ʧ��)
Others     :
********************************************************************************
*/
#if ACTIVE_5BAUD_INIT_EN == 1
bool bfun_active_5baud_init( uint8 *pu8LastByte, uint8 u8FunctionParameter )
{

    GPIO_InitTypeDef GPIO_InitStructure;
    const TYPEDEF_ISO9141_ACTIVE_PAR *pIso9141ActiveLib;

    //u8KeyNum�ؼ��ֽڸ���
    //u8AddTime��ַ��λʱ��
    //u8AddCode��ַ��
    //u16Baudrate
    uint8 u8KeyNum = 0;
    uint8 u8AddTime = 0;
    uint8 u8AddCode = 0XFF;
    uint16 u16Baudrate = 0;
    pIso9141ActiveLib = &g_stIso9141ActiveLib[u8FunctionParameter];

    u8AddTime = pIso9141ActiveLib ->u8AddCodeTime;
    u8AddCode = pIso9141ActiveLib ->u8AddCode;
    u16Baudrate = pIso9141ActiveLib->u16BaudRate;
    //���͵�ַ��
    fun_send_addcode( u8AddTime, u8AddCode );

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE );
    fun_usart_baud_init( KLINE_COMMUNICATION_PORT, u16Baudrate );
    USART_ITConfig( KLINE_COMMUNICATION_PORT, USART_IT_RXNE, DISABLE );
    USART_Cmd( KLINE_COMMUNICATION_PORT, ENABLE );
    //���³�ʼ��ΪĬ��ֵ
    GPIO_InitStructure.GPIO_Pin   = ISO14230_KLINE_TXPIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );
    //�������
    KLINE_COMMUNICATION_PORT->CR1 |= 0X4;
    //ע������������ʱ��������3���ؼ��ֽ�,��ʱʱ����0.5s
    fun_time_init( TIM2, 500 );
    //������ʱ��
    TIM2->CR1 |= 1;

    while( 1 )
    {
        //�ж��Ƿ������ݵĵ���
        if( USART_GetFlagStatus( KLINE_COMMUNICATION_PORT, USART_FLAG_RXNE ) != RESET )
        {
            //�����ʱ��û��������ô��ֱ������ȥ
            *pu8LastByte = USART_ReceiveData( KLINE_COMMUNICATION_PORT );

            //�ж��Ƿ��Ѿ��������3���ֽ�
            if( 0x3 == ++u8KeyNum )
            {
                //�رն�ʱ��
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                break;
            }
        }

        //�ж��Ƿ��Ѿ���ʱ
        if( ( TIM2->SR & ( uint16 ) 1 ) == 1 )
        {
            //�رն�ʱ��
            TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
            return ( FALSE );
        }

        //��ʱ1ms
        OSTimeDly( 1 );
    }

    //ֱ�ӷ��سɹ�
    return ( TRUE );
}
#endif
/*
*******************************************************************************
Function   :fun_send_addcode
Description:�ú�����Ҫ�Ƿ��͵�ַ��
Input      :u8AddTime ��ַ�뷢�ͼ��
u8AddCode ��ַ��
Output     :none
Return     :none
Others     :
********************************************************************************
*/

#if ACTIVE_SEND_ADDCODE_EN == 1
void fun_send_addcode( uint8 u8AddTime, uint8 u8AddCode )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint32 u32BitCounter;

    //ʹ��L��
    ISO14230_LLINE_SET;
    KLINE_COMMUNICATION_PORT->CR1 &= ~0X4;
    GPIO_InitStructure.GPIO_Pin  = ISO14230_KLINE_TXPIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );
    //��������1s
    ISO14230_KLINE_SET;
    OSTimeDlyHMSM( 0, 0, 1, 0 );
    //��ʼλ;
    ISO14230_KLINE_RESET;
    OSTimeDlyHMSM( 0, 0, 0, u8AddTime );
    //����λ

    for( u32BitCounter = 0; u32BitCounter < 8; u32BitCounter++ )
    {
        if( ( ( u8AddCode ) >> u32BitCounter ) & 0x01 )
        {
            ISO14230_KLINE_SET;
        }

        else
        {
            ISO14230_KLINE_RESET;
        }

        OSTimeDlyHMSM( 0, 0, 0, u8AddTime );
    }

    //ֹͣλ
    ISO14230_KLINE_SET;
    OSTimeDlyHMSM( 0, 0, 0, u8AddTime );
    //��L���������ظߵ�ƽ
    ISO14230_LLINE_RESET;
}
#endif

/*
*******************************************************************************
Function   :u8publicfun_block_character_sc
Description:�ú�����Ҫ�ǽ����յ������ݻ���Ҫ�ϴ������ݽ������У���
Input      :datalen����Ҫ�ϴ���ʵ����Ч����,pdata��Ч���ݵ�ַ
Output     :none
Return     :none
Others     :none
********************************************************************************
*/

uint8 u8publicfun_block_character_xorc( uint16 DataLen, uint8 *pData )
{
    uint8 sum = 0;

    while( DataLen-- )
    {
        sum ^= *pData++;
    }

    return sum;
}


///*
//*******************************************************************************
//Function   :u8publicfun_block_character_sc
//Description:�ú�����Ҫ��
//Input      :datalen����Ҫ�ϴ���ʵ����Ч����,pdata��Ч���ݵ�ַ
//Output     :none
//Return     :none
//Others     :
//********************************************************************************
//*/
//void publicfun_block_character_sc( uint16 DataLen, uint8 *pData )
//{
//    uint8 sum = 0;
//    uint16 i = 0;
//    for( i = 0; i < DataLen; i++ )
//    {
//        sum += pData[i];
//    };
//    pData[i] = sum;       //���һ���ֽڷ��õ���У���
//}
/*
*******************************************************************************
Name        :allcar_up_load_data_config
Description:�ú�����Ҫ�����ǽ�����Ҫ�ϴ������ݽ������
Input      :u8CmdType�������ͱ���
u16CmdLen�мǸó������������ݵĳ���
pCacheҪ������װ�ص�����ȥ
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void publicfun_upload_data_frame_heard( uint8 u8CmdType, uint16 u16CmdLen, uint8 *pCache )
{

    pCache[0] = 0X7E;
    pCache[1] = 0X0C;
    pCache[2] = ( u16CmdLen - 6 ) >> 8;
    pCache[3] = u16CmdLen - 6;
}
/*
********************************************************************************
File name  : void *ppublicfun_send_command_index(uint8 *pCmdIndex, void *pArg1);
Description:�ú�����һ�����õĺ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-11-20   С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
void *ppublicfun_send_command_index( uint8 *pCmdIndex, void *pArg1 )
{
    void *pCommStatus;
    uint8 u8Error;
    OSMboxPost( g_pstAnalyzerToPerformer, pCmdIndex );
    pCommStatus = OSMboxPend( g_pstPerformerToAnalyzer, 0, &u8Error );
    return ( pCommStatus );
}

/*
********************************************************************************
File name  : bool bfun_cmd_can2_test(void)
Description:�ú�����һ�����õĺ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-11-20   С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
bool bfun_cmd_can2_test( void )
{
    uint8  u8Error = 0;
    uint8  u8Cmd[] = {0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    g_stCAN115765ReceiveVar1.u16DataCounter  = 0;
    g_stCAN115765ReceiveVar1.u8FrameTotal   = 0;
    g_stCAN115765ReceiveVar1.bFrameError    = FALSE;
    g_stCAN115765ReceiveVar1.bFrameLengTh   = FALSE;
    g_stCAN115765ReceiveVar1.u8FrameCounter = 0;
    //�����ʳ�ʼ��
    fun_can2_baud_init( DEF_CAN500K );
    //װ������
    memcpy( g_stTxMessage1.Data, u8Cmd, 8 );
    //�Ƚ�ֹCAN�����ж�
    CAN_ITConfig( CAN_COMMUNICATION_PORT1, CAN_IT_FMP0, DISABLE );
    //���ú�CAN�˲���
    fun_can1_Filter_init( 14, 0x7E8 );
    //����can�����ж�
    CAN_ITConfig( CAN_COMMUNICATION_PORT1, CAN_IT_FMP0, ENABLE );
    //��������һ���ٵ�����
    g_stTxMessage1.StdId = 0X7DF;
    g_stTxMessage1.RTR = 0;
    g_stTxMessage1.IDE = 0;
    //����Ҳ�Ǹ�������д�ĳ�����������
    g_stTxMessage1.DLC = 8;
    //���һ������
    g_pstEcuToPerformer->OSEventPtr = ( void * ) 0;
    CAN_Transmit( CAN_COMMUNICATION_PORT1, &g_stTxMessage1 );
    OSMboxPend( g_pstEcuToPerformer, 500, &u8Error );

    //�����ʱֱ���˳�ȥ
    if( u8Error != OS_ERR_NONE )
    {
        return FALSE;
    }

    return TRUE;
}







