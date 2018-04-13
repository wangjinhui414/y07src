/*
********************************************************************************
File name  : ZXT_data_stream.c
Description: 这里编写都是数据流功能相关函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立数据流函数
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/

#define DATA_STREAM_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
********************************************************************************
Name        :void *pfun_data_stream_all_car(void* pArg0, void* pArg1).
Description:pfun_data_stream_all_car
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :1一定要注意程序执行的步骤，不建议大家打乱工作顺序
2.该程序运用2个WHILE一定要注意关系
********************************************************************************
*/
#if RDATA_STREAM_EN == 1
void *pfun_data_stream_all_car( void* pArg0, void *pu8FunctionParameter )
{

    void *pCommStatus0 = NULL;
    void *pCommStatus1 = NULL;
    uint8 u8Error;
    //u8DataStreamCmdIdOffSet命令偏移量
    //u8DataStreamCmdId虚拟普通命令发送命令索引值
    //stDataStreamIdBuff存储系统ID表
    //u16DataStreamIdTotal要读取数据流ID总数
    //g_stPhoneCommandShare.bReadDataStream数据流退出布尔变量
    //u16DataStreamDataCacheAddr 数据流存储开始地址
    TYPEDEF_DATASTREAM_LIST *pstDataStreamIdBuff = NULL;
    uint8  u8DataStreamCmdIdOffSet = 0;
    uint8  u8DataStreamCmdId[] = {1, 0};
    uint16 u16DataStreamDataCacheAddr = 1;
    //获取是哪一个车型的数据流
    uint8 u8GetDataStreamchoice = ( ( uint8* )pu8FunctionParameter )[0];
    //获取数据流项总数(这里暂时写死)
    uint16 u16DataStreamIdTotal = 5;
    //直接指向该函数需要用上的变量库
    const TYPEDEF_DATASTREAM_GET_DATA_LIST *pstDataStream = g_pstDataStreamLib[u8GetDataStreamchoice].pstGetdataList;

    //目前最多只能存储30项数据流
    TYPEDEF_DATASTREAM_LIST  stDataStreamIdBuff[6] = {0};

    //设置好上传数据流总数
    ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[0] = u16DataStreamIdTotal * 6;
    ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )-> u8Datalen = u16DataStreamIdTotal * 6 + 1;

    //初始化链表
    pfun_init_data_Stream_idlist( stDataStreamIdBuff, 6, u16DataStreamIdTotal );
    //默认设置为通讯超时
    pCommStatus1 = ( void * )& g_enumFrameFlgArr[1];

    //设置好链表地址
    pstDataStreamIdBuff = stDataStreamIdBuff;

    //设置好数据流存储地址
    u16DataStreamDataCacheAddr = 1;

    //判断链表是不是已经到头
    while( pstDataStreamIdBuff->Next != NULL )
    {
        //发送读数据流命令
        u8DataStreamCmdIdOffSet = pstDataStream[pstDataStreamIdBuff->DataStreamId.u16Bit].u8CmdIndex;
        u8DataStreamCmdId[1] = u8DataStreamCmdIdOffSet;
        //这里在等任务回的数据邮箱来判断是不是已经超时
        OSMboxPost( g_pstAnalyzerToPerformer, u8DataStreamCmdId );
        pCommStatus0 = OSMboxPend( g_pstPerformerToAnalyzer, 0, &u8Error );

        //如果通讯失败那么将对应项设置为0,一定是6个0
        //STEP0:判断是否成功
        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus0 != OK )
        {
            memset( &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr], 0, 6 );
            u16DataStreamDataCacheAddr += 2;
        }

        else
        {
            //只要有一项成功就设置通讯状态,通讯成功
            pCommStatus1 = ( void * )& g_enumFrameFlgArr[2];
            //因为数据流ID是从01开始
            pstDataStreamIdBuff->DataStreamId.u16Bit++;
            ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr++] = pstDataStreamIdBuff->DataStreamId.u8Bit[1];
            ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr++] = pstDataStreamIdBuff->DataStreamId.u8Bit[0];
            pstDataStreamIdBuff->DataStreamId.u16Bit--;
            //同时获取数据流关键字节
            pfun_get_data_stream_calculation_bytes
            (
                pstDataStream[pstDataStreamIdBuff->DataStreamId.u16Bit].u8StartByte,
                pstDataStream[pstDataStreamIdBuff->DataStreamId.u16Bit].u8ByteTotal,
                ( ( ( g_pModeLibTab->pCmdTabChoice )[u8DataStreamCmdIdOffSet] ).u8BufferId ) & 0x0f,
                &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr]
            );

        }

        //step3:指针偏移
        u16DataStreamDataCacheAddr += 4;
        //step4:取下一个离散ID
        pstDataStreamIdBuff = pstDataStreamIdBuff->Next;
    }

    //这里强制将21的数据流改成00,同时是支持的
    memset( &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[19], 0, 6 );
    ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[20] = 4;
    return ( pCommStatus1 );
}
#endif
/*
*******************************************************************************
Name        :pfun_init_data_Stream_idlist
Description:该函数的主要目的是将某一块连续的空间组合成链表，同时对他进行初始化
Input      :TYPEDEF_DATASTREAM_LIST  *pListHead这个是要联合的首地址uint16 u16ListSize
u16DataStreamIdSize 要读取的数据大小，pDataStreamId数据流ID
Output     :none
Return     :none
Others     :
********************************************************************************
*/
#if INIT_DATA_STREAM_IDLIST ==1
void pfun_init_data_Stream_idlist( TYPEDEF_DATASTREAM_LIST *pList, uint16 u16ListSize,
                                   uint16 u16DataStreamIdSize )
{
    //初始化链表的偏移地址
    TYPEDEF_1WORD2BYTE ListSizetmp;
    TYPEDEF_DATASTREAM_LIST *pListHead = pList;

    for( ListSizetmp.u16Bit = 0; ( ListSizetmp.u16Bit < u16DataStreamIdSize && u16DataStreamIdSize < u16ListSize ); ListSizetmp.u16Bit++ )
    {
        //将ID存储好
        pList[ListSizetmp.u16Bit].DataStreamId.u8Bit[1] = ListSizetmp.u8Bit[1];
        pList[ListSizetmp.u16Bit].DataStreamId.u8Bit[0] = ListSizetmp.u8Bit[0];
        pList[ListSizetmp.u16Bit].Next = pListHead + 1 + ListSizetmp.u16Bit;
    }

    pList[ListSizetmp.u16Bit].Next = NULL;
}

void pfun_init_data_Stream_idlist0( TYPEDEF_DATASTREAM_LIST *pList, uint8 u8ListSize,
                                    uint8 u8DataStreamIdSize, uint8 *pu8DataStreamId )
{
    //初始化链表的偏移地址
    uint8 u8SizeTmp = 0;
    TYPEDEF_DATASTREAM_LIST *pListHead = pList;

    for( u8SizeTmp = 0; ( u8SizeTmp < u8DataStreamIdSize && u8DataStreamIdSize <= u8ListSize ); u8SizeTmp++ )
    {
        //将ID存储好
        pList[u8SizeTmp].DataStreamId.u16Bit = ( uint16 ) * pu8DataStreamId;
        pList[u8SizeTmp].Next = ++pListHead;
        pu8DataStreamId++;

    }

    pList[u8SizeTmp - 1].Next = &pList[0];
}

#endif

/*
********************************************************************************
Name        :pfun_get_data_Stream_Calculation_bytes
Description:该函数主要是针对从ECU那获取数据
Input      :u8datastreamstartbyte从ECU哪个字节开始获取数据
从ECU那里获取多少个数据u8DataStreamByteTotal
从哪个缓冲区获取数据pSaveBuff保存到哪里去
Output     :none
Return     :none
Others     :成功之后确定啦这个值g_pModeLibTab,具体用哪个模块进行通信,
同时确定协议类型
这个函数需要上位机提供3个变量就是模块总数 模块值 通讯函数
********************************************************************************
*/
#if GET_DATA_STREAM_CALCULATION_BYTES_EN == 1
void pfun_get_data_stream_calculation_bytes( uint8 u8StartByte,
        uint8 u8ByteTotal, uint8 u8DataStreamCurId, uint8 * pSaveBuff )
{
    uint8 counter = 0;

    for( ; counter < u8ByteTotal; counter++ )
    {
        //有待于优化
        *( pSaveBuff + counter + ( 4 - u8ByteTotal ) ) = g_stu8CacheData[u8DataStreamCurId].u8Buf[u8StartByte + counter];
    }
}
#endif


/*
********************************************************************************
Name        :void *pfun_data_stream_all_Obd(void* pArg0, void* pArg1).
Description:pfun_data_stream_all_car
Input      :
Output     :none
Return     :none
Others     :动态实时数据流
********************************************************************************
*/
#if RDATA_STREAM_EN == 1
void *pfun_data_stream_all_Obd( void* pArg0, void *pu8FunctionParameter )
{
    void *pCommStatus0 = NULL;
    uint8 u8Error;
    //u8DataStreamCmdIdOffSet命令偏移量
    //u8DataStreamCmdId虚拟普通命令发送命令索引值
    //stDataStreamIdBuff存储系统ID表
    TYPEDEF_DATASTREAM_LIST *pstDataStreamIdBuff = NULL;
    uint8  u8DataStreamCmdIdOffSet = 0;
    uint8  u8DataStreamCmdId[] = {1, 0};
    uint8  u8DataStreamCmdIdtmp[20] = {0};
    uint16 u16DataStreamDataCacheAddr = 0;
    uint8 u8TimeCounter = 0;
    uint8  u8DataStreamIdLen = 0;
    //目前最多只能存储11项数据流
    TYPEDEF_DATASTREAM_LIST  stDataStreamIdBuff[11] = {0};

    //获取是哪一个车型的数据流
    uint8 u8GetDataStreamchoice = ( ( uint8* )pu8FunctionParameter )[0];

    //直接指向该函数需要用上的变量库
    const TYPEDEF_DATASTREAM_GET_DATA_LIST *pstDataStream = g_pstDataStreamLib[u8GetDataStreamchoice].pstGetdataList;
    //6s钟产生一次溢出
    fun_time_init( TIM2, 6000 );
    //启动定时器
    TIM2->CR1 |= 1;
    //设置好链表地址
    pstDataStreamIdBuff = stDataStreamIdBuff;

    //数据流停止命令是否存在
    while( g_stPhoneCommandShare.u8ReadDataStream == DEF_DATASTREAM_STATUS_START )
    {
        //判断是否需要动态加载,注意这里比较的是数据流项目,项数不一样重新加载,长度不一样也会重新加载
        if(
            ( 0X00 != memcmp( &u8DataStreamCmdIdtmp[1], &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamIdBuf, ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Idlen ) ) |
            ( u8DataStreamIdLen != ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Idlen )
        )
        {
            u8DataStreamIdLen = ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Idlen;
            //注意这里估计有点小问题就是数据流项 从多变小时会不会出现问题
            memcpy( &u8DataStreamCmdIdtmp[1], &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamIdBuf, u8DataStreamIdLen );
            //插入车速
            u8DataStreamCmdIdtmp[0] = 35;

            //判断是不是超过11
            if( u8DataStreamIdLen + 1 > DEF_DATASTREAM_ID_TOTAL_MAX + 2 )
            {
                //说明数据流项目超过12
                g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_OPTION_OVER;
                //关闭定时器
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                break;
            }

            pfun_init_data_Stream_idlist0( stDataStreamIdBuff, 11, u8DataStreamIdLen + 1, u8DataStreamCmdIdtmp );
        }

        //判断是不是超过103项
        if( ( pstDataStreamIdBuff->DataStreamId.u8Bit[0] > DEF_DATASTREAM_ID_OFFSET_MAX ) )
        {
            //说明数据流项目超过103
            g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_OPTION_OVER;
            //关闭定时器
            TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
            break;
        }

        //发送读数据流命令,切记这里所有的数据流都是偏移4的
        u8DataStreamCmdId[1] = pstDataStream[pstDataStreamIdBuff->DataStreamId.u8Bit[0] + 4].u8CmdIndex;
        u8DataStreamCmdIdOffSet = u8DataStreamCmdId[1];
        //这里在等任务回的数据邮箱来判断是不是已经超时
        OSMboxPost( g_pstAnalyzerToPerformer, u8DataStreamCmdId );
        pCommStatus0 = OSMboxPend( g_pstPerformerToAnalyzer, 0, &u8Error );

        //STEP0:判断是否成功
        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus0 != OK )
        {
            //进来这里说明有通讯失败的数据
            if( 35 == pstDataStreamIdBuff->DataStreamId.u8Bit[0] )
            {
                //说明车速不允许,报告通讯失败
                g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_DATABUS_FAILURE;
                //关闭定时器
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                break;
            }

            else
            {
                memset( &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr], 0, 5 );
                u16DataStreamDataCacheAddr += 1;
            }
        }

        else
        {
            //因为数据流ID是从0开始
            ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr++] =
                pstDataStreamIdBuff->DataStreamId.u8Bit[0];
            //同时获取数据流关键字节
            pfun_get_data_stream_calculation_bytes
            (
                pstDataStream[pstDataStreamIdBuff->DataStreamId.u8Bit[0] + 4].u8StartByte,
                pstDataStream[pstDataStreamIdBuff->DataStreamId.u8Bit[0] + 4].u8ByteTotal,
                ( ( ( g_pModeLibTab->pCmdTabChoice )[u8DataStreamCmdIdOffSet] ).u8BufferId ) & 0x0f,
                &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr]
            );

            //判断车速是否允许
            if( 35 == pstDataStreamIdBuff->DataStreamId.u8Bit[0] )
            {
                //说明车速大于0
                if( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr + 3] != 0x00 )
                {
                    TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                    //说明车速大于0
                    g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_SPEED_STOP;
                    break;
                }
            }
        }

        //判断是不是已经到6s
        if( ( TIM2->SR & ( uint16 ) 1 ) == 1 )
        {
            //关闭定时器
            TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );

            //判断时间是不是已经到啦
            if( ++u8TimeCounter > 100 )
            {
                //关闭定时器
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                //说明通讯时间已经到
                g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_TIMEOUT_STOP;
                break;
            }

            else
            {
                //6s钟产生一次溢出
                fun_time_init( TIM2, 6000 );
                //启动定时器
                TIM2->CR1 |= 1;
            }
        }

        //step3:缓存地址开始加1
        u16DataStreamDataCacheAddr += 4;
        //step4:取下一个离散ID
        pstDataStreamIdBuff = pstDataStreamIdBuff->Next;

        //判断是不是已经到表头
        if( stDataStreamIdBuff == pstDataStreamIdBuff )
        {
            if( g_stObdData.dataLock == DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
            {
                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                g_stObdData.proParaLen = 0;
                g_stObdData.retCode = 00;
                //命令标识
                g_stObdData.proCmdId = 0x0289;
                g_stObdData.proParaBuf[g_stObdData.proParaLen] = 01;//打包方式
                g_stObdData.proParaLen += 1;
                memcpy( ( uint8* )&g_stObdData.proParaBuf[g_stObdData.proParaLen],
                        &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[5], u8DataStreamIdLen * 5 );
                g_stObdData.proParaLen += u8DataStreamIdLen * 5;

                while( g_stObdData0.dataLock != DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
                {
                    OSTimeDlyHMSM( 0, 0, 0, 10 );
                }

                OSMboxPost( OS_OBDAckMbox, &g_stObdData );
            }

            //数据缓存从01开始
            u16DataStreamDataCacheAddr = 0;
        }

    }

    return( ( void * )& g_enumFrameFlgArr[2] );
}

#endif
