/*
********************************************************************************
File name   : ZXT_create_link.c
Description: 该文件都是系统激活用的函数,绝对不允许在这里声明和定义全局变量
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立通讯激活小函数
其他       :每一种激活都附带一个变量库,这些函数必须是经常调用才能建立变量库
********************************************************************************
*/
#define CREATE_LINK_FUN //函数声明
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Function   :void *pfun_active_active_low_high_xxms(void *pArg0,void *pArg1)
Description:该函数主要是高低电平xxMS,主要是考虑到有些ECU不是标准的高低电
平所以才建立变量库
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :pCommStatus通讯状态成功与否的具体地址
Return     :pCommStatus通讯状态成功与否的具体地址
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
        //使能L线
        ISO14230_LLINE_SET;
        KLINE_COMMUNICATION_PORT->CR1 &= ~0X4;
        GPIO_InitStructure.GPIO_Pin  = ISO14230_KLINE_TXPIN;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );
        //首先拉高1s
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
        //重新初始化为默认值
        GPIO_InitStructure.GPIO_Pin   = ISO14230_KLINE_TXPIN;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );
        //允许接收
        KLINE_COMMUNICATION_PORT->CR1 |= 0X4;
        //发送进入命令
        pCommStatus = ppublicfun_send_command_index( ( uint8 * )g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter, ( void* ) 0 );
        //重新将L线拉回高电平
        ISO14230_LLINE_RESET;

        //如果超过3次或者通讯成功都得退出去
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
Description:该函数主要是对通讯总线进行初始化和发送进入命令
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :该函数主要是应对常见的CAN总线通讯
设置通讯波特率
********************************************************************************
*/
#if CREATE_LINK_CAN_BUS_EN == 1

void *pfun_active_can_bus( void *pArg0 , void *pu8FunctionParameter )
{

    void *pCommStatus;
    const TYPEDEF_ISO15765_ACTIVE_PAR *pSTRUCT_Iso15765ActiveLib;
    //直接指向15765通讯库
    pSTRUCT_Iso15765ActiveLib = &g_stIso15765ActiveLib[( ( uint8* )pu8FunctionParameter )[0]];

    //最好屏蔽K线中断
    USART_ITConfig( KLINE_COMMUNICATION_PORT, USART_IT_RXNE, DISABLE );
    //CAN通讯波特率设置
	 	if( g_pModeLibTab->pCmdTabChoice[g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter[1]].u8FilterGroup <14 )
	    fun_can1_baud_init( pSTRUCT_Iso15765ActiveLib->u32BaudRate );
		else
			fun_can2_baud_init( pSTRUCT_Iso15765ActiveLib->u32BaudRate );;
    //发送进入命令
    //这里在等fun_board_to_ecu任务回的数据邮箱来判断是不是已经超时
    pCommStatus = ppublicfun_send_command_index( ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter, ( void* ) 0 );
    return ( pCommStatus );
};

#endif


/*
*******************************************************************************
Function   :pfun_active_5baud_negate_last_byte_back_addr
Description:地址码激活方式0(
ECU回完毕第3个字节之后
解码器取反第3个字节
ECU取反地址码
解码器发送进入命令
)
Input      :void *pArg0
pu8FunctionParameter选择的是哪一个通讯初始化参数组
Output     :pCommStatus通讯状态
Return     :pCommStatus通讯状态
Others     :1.首先执行地址码激活(1.0发送地址码
1.1等待500MS接收3个字节
1.2等待一段时间(这个时间是可以控制的)发送最后一个字节取反
1.3等待500ms接收1个字节,如果接收的是地址码取反的字节那么就执行1.5
1.4如果接收到的不是对应的地址码那么之间跳出去
1.5延时一段时间发送进入命令(该时间是可以控制的),主要还是应对一些比较特殊    的ECU
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


    //如果波特率激活失败直接跳出程序,这里最好重新激活
    while( 1 )
    {
        //如果成功往下面走
        if( bfun_active_5baud_init( &u8LastByte, ( ( uint8* )pu8FunctionParameter )[0] ) )
        {
            break;
        }

        //控制重新发送次数
        if( ++u8activecounter == pIso9141ActiveLib->u8RetransTime )
        {
            return ( ( void * )&g_enumFrameFlgArr[0] );
        }

        //增加一激活延时,如果第一次激活不成功等待一段时间再次激活
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

    //这个时间是可以控制的,得根据实际的ECU来控制,有些车需要发送快一点，有些车需要发送慢一点
    OSTimeDlyHMSM( 0, 0, 0, ( pIso9141ActiveLib->u16ProtocolReversed )[0] );

    KLINE_COMMUNICATION_PORT->CR1 &= ~0X4;

    //将最后一个字节取反
    USART_SendData( KLINE_COMMUNICATION_PORT, ~u8LastByte );

    while( USART_GetFlagStatus( KLINE_COMMUNICATION_PORT, USART_FLAG_TC ) == RESET );

    //接收地址码取反字节
    KLINE_COMMUNICATION_PORT->CR1 |= 0X4;

    //注意这里启动定时器来接收1个关键字节,超时时间是0.5s
    fun_time_init( TIM2, 500 );
    //启动定时器
    TIM2->CR1 |= 1;

    while( 1 )
    {
        //判断是否有数据的到来,有就进去
        if( USART_GetFlagStatus( KLINE_COMMUNICATION_PORT, USART_FLAG_RXNE ) != RESET )
        {
            //判断接收到的数据是不是地址码取反字节
            if( ( pIso9141ActiveLib->u8AddCode ) != ( uint8 )( ~USART_ReceiveData( KLINE_COMMUNICATION_PORT ) ) )
            {
                return ( ( void* )&g_enumFrameFlgArr[0] );
            }

            else
            {
                //关闭定时器
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                break;
            }
        }

        //判断是否已经超时
        if( ( TIM2->SR & ( uint16 ) 1 ) == 1 )
        {
            //关闭定时器
            TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
            return ( ( void * )&g_enumFrameFlgArr[0] );
        }
    }

    //等一段时间然后发送进入命令
    OSTimeDlyHMSM( 0, 0, 0, ( pIso9141ActiveLib->u16ProtocolReversed )[1] );
    //开启接收中断
    USART_ITConfig( KLINE_COMMUNICATION_PORT, USART_IT_RXNE, ENABLE );

    pCommStatus = ppublicfun_send_command_index( ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter, ( void* ) 0 );

    return ( pCommStatus );
}

#endif

/*
*******************************************************************************
Function   :pfun_active_5baud_kwp1281
Description:地址码激活方式1(
ECU回完毕第3个字节之后
解码器取反第3个字节
ECU根据1281协议发送相应命令
解码器发送进入命令
)
Input      :void *pArg0
pu8FunctionParameter选择的是哪一个通讯初始化参数组
Output     :pCommStatus通讯状态
Return     :pCommStatus通讯状态
Others     :1.首先执行地址码激活(1.0发送地址码
1.1等待500MS接收3个字节
1.2等待一段时间(这个时间是可以控制的)发送最后一个字节取反
1.3等待500ms接收1帧数据,如果失败直接跳出去
1.5延时一段时间发送进入命令(该时间是可以控制的),主要还是应对一些比较特殊的ECU
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

    //如果波特率激活失败直接跳出程序,这里最好重新激活
    while( 1 )
    {
        //如果成功往下面走
        if( bfun_active_5baud_init( &u8LastByte, ( ( uint8* )pu8FunctionParameter )[0] ) )
        {
            break;
        }

        //控制重新发送次数
        if( ++u8activecounter == pIso9141ActiveLib->u8RetransTime )
        {
            return ( ( void* )&g_enumFrameFlgArr[0] );
        }

        //增加一激活延时,如果第一次激活不成功等待一段时间再次激活
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

    //这个时间是可以控制的,得根据实际的ECU来控制,有些车需要发送快一点，有些车需要发送慢一点
    OSTimeDlyHMSM( 0, 0, 0, ( pIso9141ActiveLib->u16ProtocolReversed )[0] );

    KLINE_COMMUNICATION_PORT->CR1 &= ~0X4;

    //将最后一个字节取反
    USART_SendData( KLINE_COMMUNICATION_PORT, ~u8LastByte );

    while( USART_GetFlagStatus( KLINE_COMMUNICATION_PORT, USART_FLAG_TC ) == RESET );

    //最好先清除再等邮箱
    g_pstEcuToPerformer->OSEventPtr = ( void * ) 0;
    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
    //注意这个值在整个系统中只是初始化一次
    g_stUsart3Iso14230ReceiveVar.u8SentCounter = 0;
    g_stUsart3Iso14230ReceiveVar.bFrameError = FALSE;
		
											g_pModeLibTab_lib = g_pModeLibTab;
										g_unnu32FrameIdLib_lib = g_unnu32FrameIdLib;
										g_unnu32FilterId_lib = g_unnu32FilterId;
										g_stIsoKlineCommLib_lib = g_stIsoKlineCommLib;
										g_stIso15765ActiveLib_lib = g_stIso15765ActiveLib;

    //接收一帧数据
    if( OK != *( TYPEDEF_FRAME_STATUS* )pfun_iso_1281_receive( ( void* ) 0, ( void* ) 0 ) )
    {
        return ( ( void * )&g_enumFrameFlgArr[0] );
    }

    //等一段时间然后发送进入命令
    OSTimeDlyHMSM( 0, 0, 0, ( pIso9141ActiveLib->u16ProtocolReversed )[1] );
    pCommStatus = ppublicfun_send_command_index( ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdEnter, ( void* ) 0 );

    return ( pCommStatus );
}

#endif
/*
*******************************************************************************
Function   :bfun_active_5baud_init
Description:该函数主要是5比特率激活函数,
1.发送地址码
2.然后等着接收3个字节
3.返回成功与否
Input      :void
Output     :true(成功) false(失败)
Return     :true(成功) false(失败)
Others     :
********************************************************************************
*/
#if ACTIVE_5BAUD_INIT_EN == 1
bool bfun_active_5baud_init( uint8 *pu8LastByte, uint8 u8FunctionParameter )
{

    GPIO_InitTypeDef GPIO_InitStructure;
    const TYPEDEF_ISO9141_ACTIVE_PAR *pIso9141ActiveLib;

    //u8KeyNum关键字节个数
    //u8AddTime地址码位时间
    //u8AddCode地址码
    //u16Baudrate
    uint8 u8KeyNum = 0;
    uint8 u8AddTime = 0;
    uint8 u8AddCode = 0XFF;
    uint16 u16Baudrate = 0;
    pIso9141ActiveLib = &g_stIso9141ActiveLib[u8FunctionParameter];

    u8AddTime = pIso9141ActiveLib ->u8AddCodeTime;
    u8AddCode = pIso9141ActiveLib ->u8AddCode;
    u16Baudrate = pIso9141ActiveLib->u16BaudRate;
    //发送地址码
    fun_send_addcode( u8AddTime, u8AddCode );

    RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE );
    fun_usart_baud_init( KLINE_COMMUNICATION_PORT, u16Baudrate );
    USART_ITConfig( KLINE_COMMUNICATION_PORT, USART_IT_RXNE, DISABLE );
    USART_Cmd( KLINE_COMMUNICATION_PORT, ENABLE );
    //重新初始化为默认值
    GPIO_InitStructure.GPIO_Pin   = ISO14230_KLINE_TXPIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );
    //允许接收
    KLINE_COMMUNICATION_PORT->CR1 |= 0X4;
    //注意这里启动定时器来接收3个关键字节,超时时间是0.5s
    fun_time_init( TIM2, 500 );
    //启动定时器
    TIM2->CR1 |= 1;

    while( 1 )
    {
        //判断是否有数据的到来
        if( USART_GetFlagStatus( KLINE_COMMUNICATION_PORT, USART_FLAG_RXNE ) != RESET )
        {
            //如果超时还没有数据那么就直接跳出去
            *pu8LastByte = USART_ReceiveData( KLINE_COMMUNICATION_PORT );

            //判断是否已经接收完毕3个字节
            if( 0x3 == ++u8KeyNum )
            {
                //关闭定时器
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                break;
            }
        }

        //判断是否已经超时
        if( ( TIM2->SR & ( uint16 ) 1 ) == 1 )
        {
            //关闭定时器
            TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
            return ( FALSE );
        }

        //延时1ms
        OSTimeDly( 1 );
    }

    //直接返回成功
    return ( TRUE );
}
#endif
/*
*******************************************************************************
Function   :fun_send_addcode
Description:该函数主要是发送地址码
Input      :u8AddTime 地址码发送间隔
u8AddCode 地址码
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

    //使能L线
    ISO14230_LLINE_SET;
    KLINE_COMMUNICATION_PORT->CR1 &= ~0X4;
    GPIO_InitStructure.GPIO_Pin  = ISO14230_KLINE_TXPIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init( ISO14230_KLINE_PORT, &GPIO_InitStructure );
    //首先拉高1s
    ISO14230_KLINE_SET;
    OSTimeDlyHMSM( 0, 0, 1, 0 );
    //启始位;
    ISO14230_KLINE_RESET;
    OSTimeDlyHMSM( 0, 0, 0, u8AddTime );
    //数据位

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

    //停止位
    ISO14230_KLINE_SET;
    OSTimeDlyHMSM( 0, 0, 0, u8AddTime );
    //将L线重新拉回高电平
    ISO14230_LLINE_RESET;
}
#endif

/*
*******************************************************************************
Function   :u8publicfun_block_character_sc
Description:该函数主要是将接收到的数据或者要上传的数据进行异或校验和
Input      :datalen待会要上传的实际有效数据,pdata有效数据地址
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
//Description:该函数主要是
//Input      :datalen待会要上传的实际有效数据,pdata有效数据地址
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
//    pData[i] = sum;       //最后一个字节放置的是校验和
//}
/*
*******************************************************************************
Name        :allcar_up_load_data_config
Description:该函数主要功能是将待会要上传的数据进行组合
Input      :u8CmdType命令类型备用
u16CmdLen切记该长度是所有数据的长度
pCache要把命令装载到哪里去
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
Description:该函数是一个公用的函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-11-20   小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
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
Description:该函数是一个公用的函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-11-20   小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
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
    //波特率初始化
    fun_can2_baud_init( DEF_CAN500K );
    //装载命令
    memcpy( g_stTxMessage1.Data, u8Cmd, 8 );
    //先禁止CAN接收中断
    CAN_ITConfig( CAN_COMMUNICATION_PORT1, CAN_IT_FMP0, DISABLE );
    //配置好CAN滤波器
    fun_can1_Filter_init( 14, 0x7E8 );
    //开启can接收中断
    CAN_ITConfig( CAN_COMMUNICATION_PORT1, CAN_IT_FMP0, ENABLE );
    //最好先清除一次再等邮箱
    g_stTxMessage1.StdId = 0X7DF;
    g_stTxMessage1.RTR = 0;
    g_stTxMessage1.IDE = 0;
    //这里也是根据所填写的长度来决定的
    g_stTxMessage1.DLC = 8;
    //清空一下邮箱
    g_pstEcuToPerformer->OSEventPtr = ( void * ) 0;
    CAN_Transmit( CAN_COMMUNICATION_PORT1, &g_stTxMessage1 );
    OSMboxPend( g_pstEcuToPerformer, 500, &u8Error );

    //如果超时直接退出去
    if( u8Error != OS_ERR_NONE )
    {
        return FALSE;
    }

    return TRUE;
}







