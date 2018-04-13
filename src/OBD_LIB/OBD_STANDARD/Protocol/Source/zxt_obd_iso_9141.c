/*
********************************************************************************
File name  : zxt_iso_9141.c
Description: 该文件主要是标准的9141协议相关函数,绝对不允许在这里定义变量,在该
             文件建立的都是函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    冯秋杰   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define ISO_9141_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Name        :pfun_iso_9141_send_command_group
Description:该函数主要是控制发送一整组命令,这个函数相关的变量
Input      :void *pcmdid      xx命令索引值
Output     :none
Return     :void *pCommStatus 通讯报告地址
Others     :
********************************************************************************
*/
#if IS0_9141_SEND_GROUP_EN == 1
void*pfun_iso_9141_send_command_group( void *pCmdId, void *prg1 )
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
    pstIsoKlineCommLib = &g_stIsoKlineCommLib[( g_pModeLibTab->u8ProtocolLibOffset )[3]];
    //这里主要是控制发送一整组命令

    for( u8CurSent = 0; u8CurSent < ( ( uint8* ) pCmdId )[0]; u8CurSent++ )
    {
        //待会发送的命令长度
        u8FrameLen = ( ( ( *g_pModeLibTab ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8CmdLen;
        //发送命令
        fun_iso_9141_send_times( ( ( uint8* ) pCmdId )[u8CurSent + 1], u8FrameLen, &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            break;//跳出for 直接报告通讯不正常
        }

        //待会将数据存储在哪一个缓冲区
        u8CacheOffset = ( ( ( ( *g_pModeLibTab ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId ) & 0x0f;

        //开始接收数据,这里已经写死没有调用库
        pfun_iso_14230_receive( &u8CacheOffset, ( void* ) 0 );

        //通讯帧时间
        OSTimeDlyHMSM( 0, 0, pstIsoKlineCommLib ->u16FrameTime / 1000, pstIsoKlineCommLib ->u16FrameTime % 1000 );
    }

    return pCommStatus;
}

#endif
/*
*******************************************************************************
Name        :fun_iso_9141_send_times
Description:该函数主要控制发送命令同时重新发送次数
            如果成功然后跳出该函数去执行接收函数
            该函数变量初始化是和通讯连接是同一个偏移量
Input      :uint8 cmdid:命令 uint8 u8FrameLen:命令长度
            void **pCommStatus:通讯状态
Output     :none
Return     :none
Others     :
********************************************************************************
*/
#if IS0_9141_SEND_EN == 1
void fun_iso_9141_send_times( uint8 cmdid, uint8 u8FrameLen, void **pCommStatus )
{

    //u8RetransCounter重发次数
    uint8 u8RetransCounter;
    uint8 u8CheckSum = 0;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;

    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib[( g_pModeLibTab->u8ProtocolLibOffset )[3]];
    u8RetransCounter = ( *pstIsoKlineCommLib ).u8RetransTime;

    do
    {
        fun_iso_14230_send_cmd( cmdid, u8FrameLen, ( *pstIsoKlineCommLib ).u8ByteTime );
        //ecu超时时间
        OSTimeDlyHMSM( 0, 0, pstIsoKlineCommLib->u16TimeOut / 1000, pstIsoKlineCommLib->u16TimeOut % 1000 );
        //这里是预防ECU没有响应

        if( g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter == 0 )
        {
            *pCommStatus = ( void * )&g_enumFrameFlgArr[0];
            //重新发送

            if( ( --u8RetransCounter ) != 0 )
                continue;

            else
                return;//如果重发次数已经到那么就跳出函数
        }

        //接收到的数据不正确
        if( g_stUsart3Iso14230ReceiveVar.bFrameError )
        {
            //当做帧超时
            *pCommStatus = ( void * )&g_enumFrameFlgArr[1];

            if( ( --u8RetransCounter ) != 0 )
                continue;//重新发送

            else
                return;//如果重发次数已经到那么就跳出函数
        }

        //通过计算效验和和接收到的效验和进行比较判断数据的正确
        u8CheckSum = g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1];

        publicfun_block_character_sc( g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1, g_stDataFromEcu.u8Buf );

        if( u8CheckSum != g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1] )
        {
            *pCommStatus = ( void * )&g_enumFrameFlgArr[1]; //当做帧超时

            if( ( --u8RetransCounter ) != 0 )
                //重新发送
                continue;

            else
                return;//如果重发次数已经到那么就跳出函数
        }

        *pCommStatus = ( void * )&g_enumFrameFlgArr[2]; //当做接收正常

        //设置帧长只是取有效数据长度
        g_stDataFromEcu.u16Len = g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 4;
        //重新使用该变量,重新将数据移动一次

        for( u8CheckSum = 0; u8CheckSum < g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 4; u8CheckSum++ )
        {
            g_stDataFromEcu.u8Buf[u8CheckSum] = g_stDataFromEcu.u8Buf[u8CheckSum + 3];
        }

        //跳出函数
        return;
    }
    while( 1 );
}

#endif
/*
*******************************************************************************
Name        :pfun_iso_9141_68_check
Description:该函数主要被中断程序调用,主要是核对从ECU接收过来的数据是否
            正确如果正确投递出去,数据处理好放置在u8receivedatafromecu.u8databuf
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :g_stUsart3Iso14230ReceiveVar.bFrameError判断接收到的帧数据是否正确
            注意问题:这里没有帧长，所以只能判断前面2个字节
                     接收过来的数据包含u8datafromecu.u8Buf[68,6A,ID,DATE0]
                    1.首先建立一个计数器g_stUsart3Iso14230ReceiveVar.u8DataCounter
                    2.然后判断接收过来的数据是否正确如果不正确
                      g_stUsart3Iso1420ReceiveVar.bFrameError设置为假
                    3.然后一直接收数据
********************************************************************************
*/
#if ISO_9141_68_CHECK_EN==1
void*pfun_iso_9141_68_check( void*pArg0, void*pArg1 )
{
    void *prg0 = NULL;

    switch( g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter++ )
    {

        case 0:
            {
                //这里判断的是不是我们想接的数据
                if( 0x48 != g_stDataFromEcu.u8Buf[0] )
                {
                    //设置为帧错误,用于选择上传通讯状态
                    g_stUsart3Iso14230ReceiveVar.bFrameError = TRUE ;
                }
            }

            return ( prg0 );

        case 1:
            {
                //这里判断的是不是我们想接的数据
                if( 0x6B != g_stDataFromEcu.u8Buf[1] )
                {
                    //设置为帧错误,用于选择上传通讯状态
                    g_stUsart3Iso14230ReceiveVar.bFrameError = TRUE ;
                }
            }

        default:
            return ( prg0 );
    }
}

#endif



