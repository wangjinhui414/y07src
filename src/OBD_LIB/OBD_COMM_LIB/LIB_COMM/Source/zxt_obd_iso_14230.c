/*
********************************************************************************
File name  : ZXT_iso_14230.C
Description: 该文件主要是标准的14230协议相关函数,绝对不允许在这里定义变量,在该
              文件建立的都是函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   这里主要是14230所有的协议函数
********************************************************************************
*/

#define LIB_COMM_FUN
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"

/*
*******************************************************************************
Function   :pfun_iso14230_send_command_group
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

void*pfun_iso14230_send_command_group( void *pCmdId, void *pArg1 )
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
        fun_iso_14230_send_times( ( ( uint8* ) pCmdId )[u8CurSent + 1], u8FrameLen, &pCommStatus );

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
Function   :fun_iso_14230_send_times
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
void fun_iso_14230_send_times( uint8 u8CmdId, uint8 u8FrameLen, void **pCommStatus )
{

    //u8RetransCounter重发次数
    uint8 u8Error;
    uint8 u8RetransCounter;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    u8RetransCounter = pstIsoKlineCommLib->u8RetransTime;

    do
    {
        fun_iso_14230_send_cmd( u8CmdId, u8FrameLen, pstIsoKlineCommLib->u8ByteTime );


        /*在这等邮箱有2种可能
        1.ECU接收数据中断
        2.串口2中断，其他任务不可能执行
        3.因为大家都在等邮箱
        */
        *pCommStatus = pfOSboxped( g_pstEcuToPerformer_lib, pstIsoKlineCommLib ->u16TimeOut, &u8Error );

        switch( u8Error )
        {

            case  OS_ERR_TIMEOUT:
                {
                    //这里当做帧超时
                    *pCommStatus = ( void * )& g_enumFrameFlgArr[0];

                    //建立重发机制
                    if( ( --u8RetransCounter ) == 0 )
                    {
                        return;//直接退出函数
                    }

                    break;
                }

            //成功接收邮件,有2种情况:1.通讯正常 2.接收到7f 78类型的数据
            case  OS_NO_ERR:
                {
                    if( ( *( TYPEDEF_FRAME_STATUS* ) *pCommStatus == OK ) )
                    {
                        return;
                    }

                    break;
                }

            default://报告从ECU那投递过来的信息有问题
                {
                    return;
                }

        };
    }
    while( 1 );
}


/*
*******************************************************************************
Name        :fun_iso_14230_send_cmd
Description:该函数主要是标准的14230发送函数
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :首先根据cmdid去装载命令
            发送完一帧命令
********************************************************************************
*/
void fun_iso_14230_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime )
{

    //u16LenCounter长度计数器
    uint16 u16LenCounter = 0;
    //对串口3变量进行初始
    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
    g_stUsart3Iso14230ReceiveVar.bFrameError = FALSE;

    fun_iso_14230_cmd_load( u8CmdId, u8FrameLen, g_u8SentDataToEcu );
    //设置好check函数
    g_u8ProtocolLibOffset = ( g_pModeLibTab_lib->u8ProtocolLibOffset )[2];
    USART3->CR1 &= ~ 0X4;

    do
    {
        USART_SendData( USART3, g_u8SentDataToEcu[u16LenCounter++] );

        while( USART_GetFlagStatus( USART3, USART_FLAG_TC ) == RESET );

        if( u16LenCounter == u8FrameLen )
        {
            break;
        }

        //字节延时
        pfOSdelay( 0, 0, 0, u8ByteTime );
    }
    while( 1 );

    //最好先清除一次再等邮箱
    g_pstEcuToPerformer_lib->OSEventPtr = ( void * ) 0;

    USART3->CR1 |= 0X4;
}


/*
*******************************************************************************
Name        :fun_iso_14230_cmd_load
Description:该函数主要是将命令装载过来,切换ID同时进行效验和
Input      :uint8 u8Cmdid命令索引值,uint8 u8FrameLen命令长度 ,uint8 *pCmdBuf发送缓存
Output     :none
Return     :none
Others     :
********************************************************************************
*/


void fun_iso_14230_cmd_load( uint8 u8Cmdid, uint8 u8FrameLen , uint8 *pCmdBuf )
{
    TYPEDEF_2WORD4BYTE frameid;
    uint8 u8Kwp2000Type = ( ( ( ( g_pModeLibTab_lib ->pCmdTabChoice )[u8Cmdid] ).pu8Command )[0] & 0xc0 ) >> 6;
    //找到帧ID
    frameid = g_unnu32FrameIdLib_lib[( ( g_pModeLibTab_lib->pCmdTabChoice )[u8Cmdid] ).u8CmdIdOffset];

    //加载命令
    memcpy( pCmdBuf, ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[u8Cmdid] ).pu8Command, u8FrameLen );

    //判断是哪一种格式
    switch( u8Kwp2000Type )
    {

        case 0x02://80与非80

        case 0x03://0c
            {
                pCmdBuf[1] = frameid.u8Bit[1];//改变ID
                pCmdBuf[2] = frameid.u8Bit[0];
            }

            break;

        default  ://68 6a 02 03开头不用切换
            break;
    };

    //计算效验和
    publicfun_block_character_sc( ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8Cmdid] ).u8CmdLen - 1, pCmdBuf );
}


/*
*******************************************************************************
Name        :pfun_iso_14230_80_check
Description:该函数主要被中断程序调用,主要是核对从ECU接收过来的数据是否
                    正确如果正确投递出去,数据处理好放置在u8receivedatafromecu.u8databuf
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :1.首先建立一个计数器inter_var0.u8DataCounter
            2.每进来一次该函数就自动+1
            3.然后判断接收到的数据是否正确
            4.如果正确则调整指针接收有效数据,否则报告帧超时。

注意问题:如果接收的数据不完整当做超时处理-不投递邮箱
         //如果接收的数据不正确也就是帧ID不正确-不投递邮箱
         如果接收的数据属于消极响应系列,等正确的数据过来否则不投邮箱
         只有通讯正常才会投递邮箱。任何时候都不允许投递邮箱,
********************************************************************************
*/

void*pfun_iso_14230_80_check( void*pArg0, void*pArg1 )
{
    void *prg0 = NULL;
    TYPEDEF_FRAME_STATUS *Comm_report = NULL;

    switch( g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter++ )
    {

        case 0:
            {
                //这里计算的是有效数据的长度
                if( g_stDataFromEcu.u8Buf[0] >= 0x81 )
                {
                    g_stDataFromEcu.u8Len = g_stDataFromEcu.u8Buf[0] & 0x3f;
                }

                else if( g_stDataFromEcu.u8Buf[0] == 0x80 )
                {
                    g_stDataFromEcu.u8Len = 0xff;
                }
                else
                {
                    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
                    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
                }

                return ( prg0 );
            }

        //这里计算的是有效数据的长度
        case 3:
            {
                if( 0x80 == g_stDataFromEcu.u8Buf[0] )
                {
                    g_stDataFromEcu.u8Len = g_stDataFromEcu.u8Buf[3];
                }

                //重新调整接收指针接收邮箱数据
                if( g_stDataFromEcu.u8Buf[0] == 0x80 )
                {
                    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
                }

                return ( prg0 );
            }

        //如果属于81就得重新调整接收指针
        case 2:
        case 1:
            {
                //重新调整接收指针接收邮箱数据
                if( g_stDataFromEcu.u8Buf[0] != 0x80 )
                {
                    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
                }

                return ( prg0 );
            }

        default:
            {
                //如果能正确的接收到数据
                if( ( g_stDataFromEcu.u8Len + 1 ) == g_stUsart3Iso14230ReceiveVar.u8DataOffset )
                {
                    //关闭接收
                    if( g_stUsart3Iso14230ReceiveVar.u8ReceivePack == 1 )
                    {
                        USART3->CR1 &= ~ 0X4;
                    }

                    //清除接收存储位置,计数器也得重新清楚
                    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
                    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;

                    //返回真代表出现消极响应
                    if( !bfun_iso_14230_negative_response() )
                    {
                        //设置为帧正常
                        Comm_report = ( void* )& g_enumFrameFlgArr[2];
                        pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                    }

                    else
                    {
                        //到这里已经出现接收到的数据是7F类型的数据
                        //如果是7f 78那么就等着接收正确的数据,如果不回数据那么就等着超时
                        //如果是7f 11说明是不支持,直接等着超时
                        Comm_report = ( void * )& g_enumFrameFlgArr[1];

                        //如果是7f 21说明是要重新发送数据,然后投递邮箱重新发送数据
                        if( ( 0x7f == g_stDataFromEcu.u8Buf[0] ) & ( 0x21 == g_stDataFromEcu.u8Buf[2] ) )
                        {
                            pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                        }
                    }
                }
            }

            return ( prg0 );

    }
}


/*
*******************************************************************************
Name        :bfun_iso_14230_negative_response
Description:该函数主要是处理ECU出现消极响应函数
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/

bool bfun_iso_14230_negative_response( void )
{
    if( 0X7F == g_stDataFromEcu.u8Buf[0] )
    {
        switch( g_stDataFromEcu.u8Buf[2] )
        {

            case 0x10://(该请求被拒绝)
            case 0x11://(不支持)
            case 0x12://(不支持或者格式不正确)
            case 0x21://(重新发送请求)
            case 0x22://(命令错误或者是条件不充分)
            case 0x78://(等待)
                return TRUE;

            default  :
                return FALSE;
        }
    }

    else
    {
        return FALSE;
    }
}



/*
*******************************************************************************
Name        :pfun_iso_14230_receive
Description:该函数主要是标准的14230接收函数
Input      :pCmdId,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :这里只是接收一帧命令
********************************************************************************
*/

void*pfun_iso_14230_receive( void *pCacheOffset, void *pArg1 )
{
    void *prg0 = NULL;

    switch( *( uint8* ) pCacheOffset )
    {

        case 0x0f://如果是255说明不用存储
            break;

        default://将数据放置到需要放置的地方
            g_stu8CacheData[*( uint8* ) pCacheOffset] = g_stDataFromEcu;
            break;
    }

    return ( prg0 );
}

/*
*******************************************************************************
Function   :fun_iso_14230_send_times1
Description:该函数主要控制发送命令同时重新发送次数
            如果成功然后跳出该函数去执行接收函数
            该函数变量初始化是和通讯连接是同一个偏移量
Input      :uint8 cmdid:发送哪一条命令 uint8 u8FrameLen:命令长度
            void **pCommStatus:通讯状态
Output     :none
Return     :none
Others     :该函数允许接收多帧k线数据，后续会慢慢替换掉fun_iso_14230_send_times
            为安全起见，先这么做
********************************************************************************
*/

void fun_iso_14230_send_times1( uint8   u8CmdId,      uint8 u8FrameLen,
                                uint8   u8CacheOffset, uint8 u8ReceivePack,
                                void    **pCommStatus )
{

    //u8RetransCounter重发次数
    uint8 u8Error;
    uint8 u8RetransCounter;
    uint8 u8ReceivePackTmp;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    uint8 *pu8CacheData = g_stu8CacheData[u8CacheOffset].u8Buf;
    uint8 *pu8DataLen = &g_stu8CacheData[u8CacheOffset].u8Len;
    *pu8DataLen = 0 ;
    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    u8RetransCounter = pstIsoKlineCommLib->u8RetransTime;

    do
    {
        //重新初始化接收包数
        u8ReceivePackTmp = 0;
        g_stUsart3Iso14230ReceiveVar.u8ReceivePack = u8ReceivePack;
        fun_iso_14230_send_cmd( u8CmdId, u8FrameLen, pstIsoKlineCommLib->u8ByteTime );

        /*在这等邮箱有2种可能
        1.ECU接收数据中断
        2.串口2中断，其他任务不可能执行
        3.因为大家都在等邮箱
        */
        do
        {
            *pCommStatus = pfOSboxped( g_pstEcuToPerformer_lib, pstIsoKlineCommLib ->u16TimeOut, &u8Error );

            if( OS_ERR_NONE != u8Error )
            {
                //关闭接收
                USART3->CR1 &= ~ 0X4;
                //这里当做帧超时
                *pCommStatus = ( void * )& g_enumFrameFlgArr[0];

                //建立重发机制
                if( ( --u8RetransCounter ) == 0 )
                {
                    return;//直接退出函数
                }

                break;
            }
            else if( ( *( TYPEDEF_FRAME_STATUS* ) *pCommStatus != OK ) )
            {
                break;
            }
            else
            {
                //这里要多重接收
                //待会将数据存储在哪一个缓冲区
                if( u8CacheOffset == 0x0f )
                {
                    //关闭接收????
                    USART3->CR1 &= ~ 0X4;
                    return;
                }
                else
                {

                    memcpy( pu8CacheData,
                            &g_stDataFromEcu.u8Buf[( u8ReceivePackTmp > 0 ) ? 1 : 0],
                            ( u8ReceivePackTmp > 0 ) ? g_stDataFromEcu.u8Len - 1 : g_stDataFromEcu.u8Len );
                    pu8CacheData += ( u8ReceivePackTmp > 0 ) ? g_stDataFromEcu.u8Len - 1 : g_stDataFromEcu.u8Len;
                    *pu8DataLen += ( u8ReceivePackTmp > 0 ) ? g_stDataFromEcu.u8Len - 1 : g_stDataFromEcu.u8Len;

                    //判断接收包是否完成
                    if( u8ReceivePack == ++u8ReceivePackTmp )
                    {
                        //关闭接收
                        USART3->CR1 &= ~ 0X4;
                        return;
                    }

//                    else
//                    {
//                        //开启接收
//                        USART3->CR1 |= 0X4;
//                    }
                }
            }
        }
        while( 1 );
    }
    while( 1 );
}


void*pfun_iso14230_send_command_group1( void *pCmdId, void *pArg1 )
{
    //u8CurSent当前发送命令值
    //pCommStatus通讯状态
    //u8CacheOffset缓存存储地址
    uint8 u8CurSent = 0;
    void  *pCommStatus;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    uint8 u8FrameLen = 0;
    uint8 u8CacheOffset = 0;
    uint8 u8ReceivePack = 0;
    //找到要初始化的变量地址
    pstIsoKlineCommLib = &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];

    //这里主要是控制发送一整组命令

    for( u8CurSent = 0; u8CurSent < ( ( uint8* ) pCmdId )[0]; u8CurSent++ )
    {
        //待会发送的命令长度
        u8FrameLen = ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8CmdLen;
        //待会将数据存储在哪一个缓冲区
        u8CacheOffset = ( ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId ) & 0x0f;
        u8ReceivePack = ( ( ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId ) & 0xf0 ) >> 4;

        //发送命令
        fun_iso_14230_send_times1( ( ( uint8* ) pCmdId )[u8CurSent + 1],
                                   u8FrameLen,
                                   u8CacheOffset,
                                   u8ReceivePack,
                                   &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            break;//跳出for 直接报告通讯不正常
        }

        //通讯帧时间
        pfOSdelay( 0, 0, pstIsoKlineCommLib ->u16FrameTime / 1000, pstIsoKlineCommLib ->u16FrameTime % 1000 );
    }

    return pCommStatus;
}
/*
*******************************************************************************
Name        :pfun_iso_14230_80_check
计算校验和
********************************************************************************
*/
void publicfun_block_character_sc( uint16 DataLen, uint8 *pData )
{
    uint8 sum = 0;
    uint16 i = 0;

    for( i = 0; i < DataLen; i++ )
    {
        sum += pData[i];
    };

    pData[i] = sum;       //最后一个字节放置的是校验和
}

