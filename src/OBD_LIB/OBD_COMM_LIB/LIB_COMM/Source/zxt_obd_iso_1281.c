/*
********************************************************************************
File name  : ZXT_iso_1281.C
Description: 该文件主要是标准的1281协议相关函数,绝对不允许在这里定义变量,在该
              文件建立的都是函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   这里主要是1281所有的协议函数
********************************************************************************
*/

#define LIB_COMM_FUN
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"

/*
*******************************************************************************
Function   :pfun_iso1281_send_command_group
Description:该函数主要是控制发送一整组命令,这个函数相关的变量都是在
                    creatlinkOffset中决定的
Input      :void *pCmdId是发送命令索引值,void *pArg1备用
Output     :none
Return     :通讯成功与否
Others     :成功之后确定啦这个值g_pModeLibTab_lib,具体用哪个模块进行通信,
            同时确定协议类型
            这个函数需要上位机提供3个变量就是模块总数 模块值 通讯函数
********************************************************************************
*/

void*pfun_iso1281_send_command_group( void *pCmdId, void *pArg1 )
{
    //u8CurSent当前发送命令值
    //pCommStatus通讯状态
    //u8CacheOffset缓存存储地址
    uint8 u8CurSent = 0;
    void  *pCommStatus;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    uint8 u8FrameLen = 0;
    uint8 u8CacheOffset = 0;
    //找到要初始化的变量地址
    pstIsoKlineCommLib = &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];

    //这里主要是控制发送一整组命令

    for( u8CurSent = 0; u8CurSent < ( ( uint8* ) pCmdId )[0]; u8CurSent++ )
    {
        //待会发送的命令长度
        u8FrameLen = ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8CmdLen;
        //发送命令
        fun_iso_1281_send_receive( ( ( uint8* ) pCmdId )[u8CurSent + 1], u8FrameLen, &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            break;//跳出for 直接报告通讯不正常
        }

        //待会将数据存储在哪一个缓冲区
        u8CacheOffset = ( ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId ) & 0x0f;

        //开始接收数据,这里已经写死没有调用库
        pfun_iso_14230_receive( &u8CacheOffset, ( void* ) 0 );

        //通讯帧时间
        pfOSdelay( 0, 0, pstIsoKlineCommLib ->u16FrameTime / 1000, pstIsoKlineCommLib ->u16FrameTime % 1000 );
    }

    return pCommStatus;
}



/*
*******************************************************************************
Function   :fun_iso_1281_send_receive
Description:该函数主要控制发送命令同时重新发送次数
            如果成功然后跳出该函数去执行接收函数
            该函数变量初始化是和通讯连接是同一个偏移量
Input      :uint8 cmdid:发送哪一条命令 uint8 u8FrameLen:命令长度
            void **pCommStatus:通讯状态
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void fun_iso_1281_send_receive( uint8 u8CmdId, uint8 u8FrameLen, void **pCommStatus )
{

    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];

    //设置好check函数
    g_u8ProtocolLibOffset = ( g_pModeLibTab_lib->u8ProtocolLibOffset )[2];

    //发送除最后一个字节数据
    if( OK != *( TYPEDEF_FRAME_STATUS* )pfun_iso_1281_send_cmd( u8CmdId, u8FrameLen, pstIsoKlineCommLib->u8ByteTime ) )
    {
        //如果发送数据失败那么就当做超时来处理
        *pCommStatus = ( void * )&g_enumFrameFlgArr[0];
        return;
    }

    //发送最后一个字节
    USART3->CR1 &= ~ 0X4;
    USART_SendData( USART3, 0x03 );

    while( USART_GetFlagStatus( USART3, USART_FLAG_TC ) == RESET );

    //最好先清除再等邮箱
    g_pstEcuToPerformer_lib->OSEventPtr = ( void * ) 0;
    //对串口3变量进行初始,记住这里不能初始化发送计数器
    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
    g_stUsart3Iso14230ReceiveVar.bFrameError = FALSE;
    //开启接收
    USART3->CR1 |= 0X4;
    *pCommStatus = pfun_iso_1281_receive( ( void* ) 0, ( void* ) 0 );
}



/*
*******************************************************************************
Name        :fun_iso_1281_send_cmd
Description:该函数主要是标准的1281发送函数
Input      :
Output     :none
Return     :通讯状态
Others     :1.首先根据cmdid去装载命令
            2.发送完一帧命令
            3.判断发送的数据是否正确
            注意最后一个字节03不在这里发送
********************************************************************************
*/

void *pfun_iso_1281_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime )
{

    //u16LenCounter长度计数器
    uint8 u8Error;
    uint8 u8RetransCounter;
    uint8 u8ReceiveDataTemp = 0;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;

    uint16 u16LenCounter = 0;
    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    u8RetransCounter = pstIsoKlineCommLib->u8RetransTime;
    //对串口3变量进行初始,记住这里不能初始化发送计数器
    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
    g_stUsart3Iso14230ReceiveVar.bFrameError = FALSE;

    //关闭接收
    USART3->CR1 &= ~ 0X4;
    //注意这里的顺序最好别颠倒
    fun_iso_1281_cmd_load( u8CmdId, u8FrameLen, g_u8SentDataToEcu );

    do
    {
        USART_SendData( USART3, g_u8SentDataToEcu[u16LenCounter] );

        while( USART_GetFlagStatus( USART3, USART_FLAG_TC ) == RESET );

        //最好先清除再等邮箱
        g_pstEcuToPerformer_lib->OSEventPtr = ( void * ) 0;
        //开启接收
        USART3->CR1 |= 0X4;
        //等待超时
        pfOSboxped( g_pstEcuToPerformer_lib, pstIsoKlineCommLib ->u16TimeOut, &u8Error );

        //关闭接收
        USART3->CR1 &= ~ 0X4;

        //判断接收单个字节是否超时
        switch( u8Error )
        {
            //超时
            case  OS_ERR_TIMEOUT:
                {
                    //建立重发机制
                    if( ( --u8RetransCounter ) == 0 )
                    {
                        //通讯超时
                        return( ( void * )&g_enumFrameFlgArr[0] ); //直接退出函数
                    }

                    break;
                }

            //正常接收到数据:1:确实是正确的数据 2:不是正确的数据
            case  OS_NO_ERR:
                {
                    //判断接收到的数据是否正常
                    u8ReceiveDataTemp = ~g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1];

                    if( g_u8SentDataToEcu[u16LenCounter] == u8ReceiveDataTemp )
                    {
                        //取下一个数
                        u16LenCounter++;

                        if( u16LenCounter == u8FrameLen )
                        {
                            //通讯正常
                            return( ( void * )&g_enumFrameFlgArr[2] );
                        }
                    }

                    //如果不正常
                    else
                    {
                        //建立重发机制
                        if( ( --u8RetransCounter ) == 0 )
                        {
                            //通讯超时
                            return( ( void * )&g_enumFrameFlgArr[0] ); //直接退出函数
                        }
                    }

                    break;
                }

            default:
                {
                    return( ( void * )&g_enumFrameFlgArr[0] ); //直接退出函数
                }
        }

        //字节延时
        pfOSdelay( 0, 0, 0, u8ByteTime );
    }
    while( 1 );
}



/*
*******************************************************************************
Name        :fun_iso_1281_cmd_load
Description:该函数主要是将命令装载过来,同时对计数器进行+1
Input      :uint8 u8Cmdid命令索引值,uint8 u8FrameLen命令长度 ,uint8 *pCmdBuf发送缓存
Output     :none
Return     :none
Others     :
********************************************************************************
*/


void fun_iso_1281_cmd_load( uint8 u8Cmdid, uint8 u8FrameLen , uint8 *pCmdBuf )
{
    //加载命令
    memcpy( pCmdBuf, ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[u8Cmdid] ).pu8Command, u8FrameLen );
    //将计数器自行加2
    pCmdBuf[1] = g_stUsart3Iso14230ReceiveVar.u8SentCounter + 2;
    g_stUsart3Iso14230ReceiveVar.u8SentCounter = pCmdBuf[1];
}

/*
*******************************************************************************
Name        :pfun_iso_1281_80_check
Description:该函数主要是被中断程序调用,只是简单的将数据处理好放置在u8receivedatafromecu.u8databuf
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :1.首先建立一个计数器inter_var0.u8ReceiveCounter
            2.每进来一次该函数就自动+1
注意问题:只要有数据就投递邮件
********************************************************************************
*/


void*pfun_iso_1281_check( void*pArg0, void*pArg1 )
{
    void *prg0 = NULL;
    g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter++];
    pfOSboxpst( g_pstEcuToPerformer_lib, ( void * )& g_enumFrameFlgArr[1] );
    return ( prg0 );

}


/*
*******************************************************************************
Name        :pfun_iso_1281_receive
Description:该函数主要是标准的1281接收函数
Input      :pCmdId,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :这里只是接收一帧命令
********************************************************************************
*/

void*pfun_iso_1281_receive( void *pArg0, void *pArg1 )
{
    uint8 u8Error;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;

    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    //设置好check函数
    g_u8ProtocolLibOffset = ( g_pModeLibTab_lib->u8ProtocolLibOffset )[2];
    //开启接收中断
    USART_ITConfig( USART3, USART_IT_RXNE, ENABLE );

    do
    {
        //开启接收
        USART3->CR1 |= 0X4;
        //等待超时
        pfOSboxped( g_pstEcuToPerformer_lib, pstIsoKlineCommLib ->u16TimeOut, &u8Error );

        //关闭接收
        USART3->CR1 &= ~ 0X4;

        //如果接收超时,直接退出去
        if( u8Error != OS_NO_ERR )
        {
            return( ( void * )&g_enumFrameFlgArr[0] );
        }

        //判断接收到的数据是否完整
        if( g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1 == g_stDataFromEcu.u8Buf[0] )
        {
            //设置好邮箱数据长度
            g_stDataFromEcu.u8Len = g_stDataFromEcu.u8Buf[0] - 1;
            //重新调整数据位置
            memmove( &g_stDataFromEcu.u8Buf[0], &g_stDataFromEcu.u8Buf[2], g_stDataFromEcu.u8Len );
            return( ( void * )&g_enumFrameFlgArr[2] );
        }

        //字节延时,这个时间暂时这么做,以后有问题再解决
        pfOSdelay( 0, 0, 0, pstIsoKlineCommLib->u8ByteTime / 2 );
        //如果没有接收完毕那么就开始取反回应数据
        USART_SendData( USART3, ~g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1] );

        while( USART_GetFlagStatus( USART3, USART_FLAG_TC ) == RESET );
    }
    while( 1 );
}

