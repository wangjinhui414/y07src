/*
********************************************************************************
File name  : Zxt_get_odometer.c
Description: 该文件主要是包含所有获取里程功能函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-11-20   小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define GET_ODOMETER_FUN //函数声明
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Name       :pfun_get_odometer_withcalculation
Description:该函数主要是通过发送命令来获取系统里程
Input      :none
Output     :none
Return     :通讯状态
Others     :需要与不需要计算都可以使用
该函数注意使用上了pu8FunctionParameter下面是公式
ABC*pu8FunctionParameter[0]/pu8FunctionParameter[1]+pu8FunctionParameter[2]
-pu8FunctionParameter[3]
注意这里使用的是单字节的计算
********************************************************************************
*/
#if GET_ODOMETER_WITHCALCULATION_EN == 1
void *pfun_get_odometer_withcalculation( void *pstsysChoice, void *pu8FunctionParameter )
{
    void *pCommStatus = NULL;
    uint8 *pCmdTmp = ( uint8 * )g_pModeLibTab->pCmdIndexTabChoice->pCmdGetOdoMeter;
    uint8 *pu8odometer = NULL;
    uint32 u32odometer = 0;
    uint8 RevCounter = 0;
    uint8 RevTemp0 = 0;
    uint8 RevTemp1 = 0;
    uint8 RevTemp2 = 0;

    //发送读取里程命令
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //如果通讯超时直接跳出函数
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //这里还得判断有效数据是否够取里程总数
    if( g_stu8CacheData[( ( uint8* )pu8FunctionParameter )[4]].u16Len <
            ( ( uint8* )pu8FunctionParameter )[5] + ( ( uint8* )pu8FunctionParameter )[6] )
    {
        //当做超时处理
        return ( void * )& g_enumFrameFlgArr[0];
    }

    //注意这里的开始位置
    pu8odometer = &g_stu8CacheData[( ( uint8* )pu8FunctionParameter )[4]].
                  u8Buf[( ( uint8* )pu8FunctionParameter )[5]];

    //判断是否要高低位互换
    if( ( ( uint8* )pu8FunctionParameter )[7] )
    {
        RevTemp1 = ( ( uint8* )pu8FunctionParameter )[6] / 2;
        RevTemp2 = ( ( uint8* )pu8FunctionParameter )[6] - 1;

        for( RevCounter = 0; RevCounter < RevTemp1; RevCounter++, RevTemp2-- )
        {
            RevTemp0 = pu8odometer[RevTemp2];
            pu8odometer[RevTemp2] = pu8odometer[RevCounter];
            pu8odometer[RevCounter] = RevTemp0;
        }
    }

    //然后开始取数据,记住这里处理之后永远都是高位在前

    for( RevCounter = ( ( uint8* )pu8FunctionParameter )[6], RevTemp0 = 0; RevCounter > 0; RevCounter--, RevTemp0++ )
    {
        u32odometer |= ( uint32 )pu8odometer[RevTemp0] << ( RevCounter - 1 ) * 8;
    }

    //然后进行计算
    u32odometer *= ( ( uint8* )pu8FunctionParameter )[0];
    u32odometer /= ( ( uint8* )pu8FunctionParameter )[1];
    u32odometer += ( ( uint8* )pu8FunctionParameter )[2];
    u32odometer -= ( ( uint8* )pu8FunctionParameter )[3];
    //计算完毕之后将结果存储于固定位置
    g_stFunMsgBuf[2].pu8FunBuffer[0] = u32odometer >> 16;
    g_stFunMsgBuf[2].pu8FunBuffer[1] = u32odometer >> 8;
    g_stFunMsgBuf[2].pu8FunBuffer[2] = u32odometer;
    return ( pCommStatus );
}
#endif
/*
*******************************************************************************
Name       :pfun_get_odometer_from_bus
Description:该函数主要是直接从总线上直接获取数据
Input      :none
Output     :none
Return     :none
Others     :1.注意这里的特殊处理
2.注意这里用上pu8FunctionParameter
********************************************************************************
*/
void *pfun_get_odometer_from_bus( void *pArg0, void *pu8FunctionParameter )
{
    uint8 u8Error = 0;
    uint8 u8FrameIdChoice = ( ( uint8* )pu8FunctionParameter )[0];
    uint8 u8StartByte = g_stOdoMeterLib[u8FrameIdChoice].u8StartByte;
    //保存好以前的check函数
    uint8 u8ProtocolLibOffsetTemp = g_u8ProtocolLibOffset;
    //设置好现在需要使用的check函数
    g_u8ProtocolLibOffset = ( ( uint8* )pu8FunctionParameter )[1];
    //第一步配置滤波器
    CAN_ITConfig( CAN_COMMUNICATION_PORT, CAN_IT_FMP0, DISABLE );
    fun_can1_baud_init( DEF_CAN500K );
    //这里最好清除一下计数器
    g_stCAN115765ReceiveVar.u16DataCounter = 0;
    //这里默认用0组
    fun_can1_Filter_init( 0, g_stOdoMeterLib[u8FrameIdChoice].u32BusFrameId );
    CAN_ITConfig( CAN_COMMUNICATION_PORT, CAN_IT_FMP0, ENABLE );
    OSMboxPend( g_pstEcuToPerformer, g_stOdoMeterLib[u8FrameIdChoice].u16FrameIdTime, &u8Error );

    //如果超时说明没有数据过来,直接退出去
    if( u8Error != OS_NO_ERR )
    {
        return( ( void * )& g_enumFrameFlgArr[0] );
    }

    //恢复以前的check函数
    g_u8ProtocolLibOffset = u8ProtocolLibOffsetTemp ;

    //到这说明接收数据成功
    //这里是有问题的还没有进行调试
    if( g_stOdoMeterLib[u8FrameIdChoice].bSwapByte )
    {
       uint8 u8temp = g_stDataFromEcu.u8Buf[u8StartByte];
       g_stDataFromEcu.u8Buf[u8StartByte] = g_stDataFromEcu.u8Buf[u8StartByte + 2];
       g_stDataFromEcu.u8Buf[u8StartByte + 2] = u8temp ;
    }

    //计算完毕之后将结果存储于固定位置
    g_stFunMsgBuf[2].pu8FunBuffer[0] = g_stDataFromEcu.u8Buf[u8StartByte];
    g_stFunMsgBuf[2].pu8FunBuffer[1] = g_stDataFromEcu.u8Buf[u8StartByte + 1];
    g_stFunMsgBuf[2].pu8FunBuffer[2] = g_stDataFromEcu.u8Buf[u8StartByte + 2];
    //说明通讯成功
    return ( ( void * )&g_enumFrameFlgArr[2] );
}

/*
*******************************************************************************
Name       :pfun_get_odometer_from_obd
Description:该函数主要是从obd上获取里程
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void *pfun_get_odometer_from_obd( void *pArg0, void *pu8FunctionParameter )
{
    //首先判断支不支持31
    //调整指针指到最后2项数据流
    uint16 u16DataStreampid = ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Datalen - 6; //??????

    //进来这个函数就说明，判断31是否支持
    if( *( uint16* ) & ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid] == 0 )
    {
        //返回成功，同时将21项目全部设置为0
        memset( &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[19], 0, 6 );
        return ( ( void * )&g_enumFrameFlgArr[2] );
    }

    //将21对应的数值中的最后一位改为01，只要支持就改为1
    *( uint16* )&( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid - 1] = 1 ;

    //非第一次进来这里,有2种可能1:没有设置初始里程,还有一种可能是设置初始里程
    if( g_stObdPar.u32OdoMeter != 0x00FFFFFF )
    {

        uint16 u16DataStreamCurpid31 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid + 4] << 8 |
                                         ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid + 5] );

        //如果校准过
        if( g_u32CalibrationOdo )
        {
            //校对过里程那么直接执行这里
            g_stObdPar.u32OdoMeter = g_u32CalibrationOdo;
            //使用完毕必须清空
            g_u32CalibrationOdo = 0;
        }
        else if( u16DataStreamCurpid31 > g_stObdPar.u16OdoPAR0 )
        {
            u16DataStreamCurpid31  = u16DataStreamCurpid31 - g_stObdPar.u16OdoPAR0;
            g_stObdPar.u32OdoMeter = u16DataStreamCurpid31 + g_stObdPar.u32OdoMeter;
        }

        //计算完毕之后将结果存储于固定位置
        g_stFunMsgBuf[2].pu8FunBuffer[0] = g_stObdPar.u32OdoMeter >> 16;
        g_stFunMsgBuf[2].pu8FunBuffer[1] = g_stObdPar.u32OdoMeter >> 8;
        g_stFunMsgBuf[2].pu8FunBuffer[2] = g_stObdPar.u32OdoMeter;
    }

    else
    {
        //到这里说明是没有设置初始里程
        //清空初始里程,预防设置初始里程为00情况
        g_stObdPar.u32OdoMeter = 0x00FFFFFF;
        //计算完毕之后将结果存储于固定位置
        g_stFunMsgBuf[2].pu8FunBuffer[0] = g_stObdPar.u32OdoMeter >> 16;
        g_stFunMsgBuf[2].pu8FunBuffer[1] = g_stObdPar.u32OdoMeter >> 8;
        g_stFunMsgBuf[2].pu8FunBuffer[2] = g_stObdPar.u32OdoMeter;
    }

    g_stObdPar.u16OdoPAR0 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid + 4] << 8 |
                              ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid + 5] );
    return ( ( void * )&g_enumFrameFlgArr[2] );

}
