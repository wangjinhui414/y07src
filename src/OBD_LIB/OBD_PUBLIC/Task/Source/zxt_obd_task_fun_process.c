/*
********************************************************************************
File name  : Zxt_task_for_mobile_and_obd_device_communications.c
Description: 该文件主要是将调度任务传递过来的具体功能进行执行
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/

#define ZXT_OBD_TASK_FUN_PROCESS_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
*******************************************************************************
Name        :fun_obd_task_fun_process
Description:该函数主要是处理串口2发送过来的数据
Input      :void *pArg这个参数都没有用上
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void  fun_obd_task_fun_process( void *pArg )
{
    uint8 u8Error = 0;
    uint8 u8FunChoiceStepTemp = 0;
    bool  bFunExecutionFinish = FALSE;
    ( void ) pArg;

    while( 1 )
    {
        //等OBD调度任务投递过来的命令
        OSSemPend( g_pstScheduleToFunAnalyzer, 0, &u8Error );

        u8FunChoiceStepTemp = g_stPhoneCommandShare.u8PhoneFunctionChoice;
        bFunExecutionFinish = TRUE;

        //因为有些步骤需要连着
        while( bFunExecutionFinish )
        {
            switch( u8Error )
            {

                case  OS_NO_ERR:
                    {
                        switch( u8FunChoiceStepTemp )
                        {
                            //开机检测是执行读取故障码和读取数据流
                            case 0:
                                {
                                    //如果读取OBD故障码失败那么就没有必要读取系统数据流
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    g_stCarScanSuccess = TIMEOUT;
                                    memset( g_stFunMsgBuf[0].pu8FunBuffer, 0, g_stFunMsgBuf[0].u16Len );
                                    g_stFunMsgBuf[0].u16Len = 1;

                                    //读取故障码
                                    fun_scan_vehmatch_fun( &g_stCarType[0], &stFunStatus, TRUE );

                                    //只有读取故障码成功才能读取系统数据流
                                    if( stFunStatus == OK )
                                    {
                                        bFunExecutionFinish = TRUE;
                                        u8FunChoiceStepTemp = 1;
                                        break;
                                    }

                                    else
                                    {
                                        //没有必要获取里程
                                        bFunExecutionFinish = FALSE;
                                        break;
                                    }
                                }

                            //读取数据流(静态)
                            case 1:
                                {
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    //注意这里的改变
                                    memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                    //获取数据流
                                    fun_scan_vehmatch_fun( &g_stCarType[1], &stFunStatus, FALSE );

                                    if( stFunStatus != OK )
                                    {
                                        //如果体检失败，数据流的长度还是存在的,默认全部填充0
                                        ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )-> u8Datalen = 0x1f;
                                    }

                                    //直接获取里程
                                    bFunExecutionFinish = TRUE;
                                    u8FunChoiceStepTemp = 5;
                                    break;
                                }

                            //清除故障码
                            case 2:
                                {
                                    //能进来清码功能说明有故障码,同时手机端已经发送清码命令
                                    //数据备份,因为这个过程会影响车辆系统个数
                                    TYPEDEF_OBD_PAR g_stObdParTmp = g_stObdPar;
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    g_stCarScanSuccess = TIMEOUT;
                                    memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                    fun_scan_cdtc_fun( &stFunStatus );
                                    //数据还原影响车辆系统个数
                                    g_stObdPar = g_stObdParTmp;
                                    //清除故障码之后,不再体检
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //K线检测
                            case 3:
                                {
                                    uint8 u8Aprg = 0;
                                    g_pModeLibTab = &( g_pstSysLibTab[0] )[0];
                                    g_stCarScanSuccess = *( TYPEDEF_FRAME_STATUS* )( *( g_pFunLib[0] + 0 ) )( ( void* )0, &u8Aprg );
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //can线检测
                            case 4:
                                {
                                    uint8 u8Aprg = 0;
                                    g_pModeLibTab = &( g_pstSysLibTab[0] )[3];
                                    g_stCarScanSuccess = *( TYPEDEF_FRAME_STATUS* )( *( g_pFunLib[0] + 2 ) )( ( void* )0, &u8Aprg );
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //can线检测
                            case 7:
                                {
                                    //
                                    //g_stCarScanSuccess = *( TYPEDEF_FRAME_STATUS* )bfun_cmd_can2_test();
                                    if( bfun_cmd_can2_test() )
                                    {
                                        g_stCarScanSuccess = OK;
                                    }
                                    else
                                    {
                                        g_stCarScanSuccess = TIMEOUT;
                                    }

                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //读取里程
                            case 5:
                                {
                                    //不管成功与否都得扫描里程
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    uint16 u16DataStreamCurpid31 = 0;
                                    g_stFunMsgBuf[2].u16Len = 3;
                                    memset( g_stFunMsgBuf[2].pu8FunBuffer, 0, g_stFunMsgBuf[2].u16Len );

                                    fun_scan_vehmatch_fun( &g_stCarType[3], &stFunStatus, TRUE );
                                    //将31获取出来
                                    u16DataStreamCurpid31 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[29] << 8 ) |
                                                            ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[30];

                                    if( u16DataStreamCurpid31 >= 0xf000 )
                                    {
                                        //这里还得执行一次清除故障码指令
                                        stFunStatus = TIMEOUT;
                                        fun_scan_car_fun( &g_stCarType[2], &stFunStatus );

                                        //如果清除故障码成功,那么得再次读取数据流
                                        if( stFunStatus == OK )
                                        {
                                            //重新调整数据流指针,同时读取数据流
                                            stFunStatus = TIMEOUT;
                                            //先初始化数据流缓冲区
                                            memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                            fun_scan_vehmatch_fun( &g_stCarType[1], &stFunStatus, FALSE );

                                            //必须通讯成功
                                            if( stFunStatus == OK )
                                            {

                                                //将31获取出来
                                                u16DataStreamCurpid31 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[29] << 8 ) |
                                                                        ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[30];

                                                if( ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[25] << 8 ) |
                                                        ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[26] ) == 0 )
                                                {
                                                    //直接退出去
                                                    bFunExecutionFinish = FALSE;
                                                    break;
                                                }

                                                //判断31为0
                                                g_stObdPar.u16OdoPAR0 = ( u16DataStreamCurpid31 == 0x00 ) ? 0 : g_stObdPar.u16OdoPAR0;
                                            }
                                        }
                                    }

                                    //直接跳出循环
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //手动体检获取数据流
                            case 6:
                                {
                                    //数据备份,因为这个过程会影响车辆系统个数
                                    TYPEDEF_OBD_PAR g_stObdParTmp = g_stObdPar;
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    //必须重新初始化一次
                                    g_stCarScanSuccess = TIMEOUT;
                                    //初始化缓冲区
                                    memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                    //获取数据流
                                    fun_scan_vehmatch_fun( &g_stCarType[4], &stFunStatus, FALSE );
                                    bFunExecutionFinish = FALSE;
                                    //数据还原影响车辆系统个数
                                    g_stObdPar = g_stObdParTmp;
                                    break;
                                }

                            //熄火逻辑判断
                            case 8:
                                {
                                    //这里需要优化
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    //注意这里的改变
                                    memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                    g_stCarScanSuccess = TIMEOUT;
                                    //执行一次熄火逻辑
                                    fun_scan_poweroff_fun( &g_stCarType[1], &stFunStatus );
                                    //直接获取里程
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //获取vin码,有可能不支持
                            case 9:
                                {
                                    //数据备份,因为这个过程会影响车辆系统个数
                                    TYPEDEF_OBD_PAR g_stObdParTmp = g_stObdPar;
                                    //不管成功与否都得获取VIN码
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    g_stCarScanSuccess = TIMEOUT;
                                    g_stFunMsgBuf[3].u16Len = 17;
                                    //注意这里OBD获取都成功可能VIN码获取失败
                                    memset( g_stFunMsgBuf[3].pu8FunBuffer, 0, g_stFunMsgBuf[3].u16Len );
                                    //执行vin获取函数
                                    fun_scan_vehmatch_fun( &g_stCarType[5], &stFunStatus, FALSE );

                                    //设置通信状态,因为有通信正常但是VIN码不正确的情况。
                                    if( stFunStatus != OK )
                                    {
                                        g_stCarScanSuccess = TIMEOUT;
                                        memset( g_stFunMsgBuf[3].pu8FunBuffer, 0xff, g_stFunMsgBuf[3].u16Len );
                                    }

                                    //直接跳出整个逻辑
                                    bFunExecutionFinish = FALSE;
                                    //数据还原影响车辆系统个数
                                    g_stObdPar = g_stObdParTmp;
                                    break;
                                }

                            default:
                                break;
                        }

                        break;
                    };

                default :
                    break;
            }
        }

        OSSemPost( g_pstFunAnalyzerToSchedule );
    }
}
/*
*******************************************************************************
Name       :fun_scan_vehmatch_fun
Description:该函数主要是整个系统故障码扫描(个性+OBD)
Input      :none
Output     :none
Return     :none
Others     :这里主要是预防以后还要添加新的功能,就像清除故障码一样
********************************************************************************
*/
void fun_scan_vehmatch_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus, bool bvehmatch )
{
    if( bvehmatch )
    {
        //在进行扫描之前先判断flash车系和程序存储的是不是一个车系
        if( g_stPhoneCommandShare.u8CurCarType != g_stPhoneCommandShare.u8DefaultCarType )
        {
            *( uint8* )&pstCarChoice->stPersonAndObdCfg[0].u8SysTotal = 0;
        }
    }


    fun_scan_car_fun( pstCarChoice, pstFunStatus );
}
/*
*******************************************************************************
Name       :fun_scan_cdtc_stream_fun
Description:个性清除故障+OBD
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_scan_cdtc_fun( TYPEDEF_FRAME_STATUS * pstFunStatus )
{
    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
    //默认清除故障码是成功的
    g_stPhoneCommandShare.u8ClearDtc = 0x00;
    //首先扫描数据流
    fun_scan_vehmatch_fun( &g_stCarType[1], &stFunStatus, FALSE );

    //判断读取数据流是否成功
    if( stFunStatus == OK )
    {
        //判断车速是否允许<100
        if( ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[14] ) != 0x00 ) &
                ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[17] << 8 | ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[18] ) < 400 ) )
        {
            //重新获取一次里程
            stFunStatus = TIMEOUT;
            //清空OBD诊断数据流
            g_stFunMsgBuf[2].u16Len = 3;
            memset( g_stFunMsgBuf[2].pu8FunBuffer, 0, g_stFunMsgBuf[2].u16Len );
            fun_scan_vehmatch_fun( &g_stCarType[3], &stFunStatus, TRUE );
            //直接调用清除故障码指令函数
            stFunStatus = TIMEOUT;
            fun_scan_car_fun( &g_stCarType[2], &stFunStatus );

            if( stFunStatus == OK )
            {
                uint16 u16Dsid31 = 0;
                stFunStatus = TIMEOUT;
                //再次获取数据流
                memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                fun_scan_vehmatch_fun( &g_stCarType[1], &stFunStatus, FALSE );

                if( stFunStatus == OK )
                {

                    u16Dsid31 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[29] << 8 | ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[30] );

                    if( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[25] << 8 | ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[26] ) != 0 )
                    {
                        g_stObdPar.u16OdoPAR0 = ( u16Dsid31 == 0x00 ) ? 0 : g_stObdPar.u16OdoPAR0;
                    }

                    //说明部分清码成功
                    g_stPhoneCommandShare.u8ClearDtc = 0x05;
                }
            }
            //说明清码不成功
            else
            {
                g_stPhoneCommandShare.u8ClearDtc = 0x04;
            }
        }
        //说明车速不允许
        else
        {
            g_stPhoneCommandShare.u8ClearDtc = 0x01;
        }
    }
    //不支持清码操作
    else
    {
        g_stPhoneCommandShare.u8ClearDtc = 0x02;
    }

    *pstFunStatus = stFunStatus;
}

/*
*******************************************************************************
Name       :fun_scan_car_fun
Description:该函数主要是实现某一个功能中的个性和OBD扫描(例如;读个性故障和OBD故障)
Input      :none
Output     :none
Return     :none
Others     :1.首先执行个性功能,如果成功直接出去。否则执行obd功能
2.通讯失败接着扫描obd
********************************************************************************
*/
void fun_scan_car_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus )
{
    //这里根据传进来的功能号判断有没有必要进行obd部分扫描
    uint8 u8CarChoiceOffset  = 0;

    for( ; u8CarChoiceOffset < 2; u8CarChoiceOffset++ )
    {
        //个性的扫描成功
        pfun_scan_sys_remem_fun( &pstCarChoice->stPersonAndObdCfg[u8CarChoiceOffset], pstFunStatus );

        if( *pstFunStatus == OK )
        {
            break;
        }

        //这里一定要添加功能与功能之间的延时
        OSTimeDlyHMSM( 0, 0, pstCarChoice->u16CarDelay / 1000, pstCarChoice->u16CarDelay % 1000 );
    }

    return;
}
/*
*******************************************************************************
Name        :fun_scan_sys_fun
Description:该函数的主要功能是对某一车的系统进行扫描带记忆功能
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void *pfun_scan_sys_remem_fun( TYPEDEF_All_SYS_CFG *pstPersonObdChoice, TYPEDEF_FRAME_STATUS * pstFunStatus )
{
    uint8 u8SysChoiceOffset = 0;
    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
    uint8 u8SysChoiceSuccess[21] = {0};
    uint8 u8CurSysOffset = 0;
    uint8 u8SysChoiceOffsetTmp = 0;
    bool  bIsFindSys = FALSE;

    //系统扫描
    for( ; u8SysChoiceOffset < pstPersonObdChoice->u8SysTotal ; )
    {
        //这里最好重新初始化一遍
        stFunStatus = TIMEOUT;
        bIsFindSys = FALSE;

        //这里开始避免同一款车多种协议重复扫描
        if( NULL != memchr(
                    &u8SysChoiceSuccess[1],
                    ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice, u8SysChoiceSuccess[0] ) )
        {
            u8SysChoiceOffset++;
            continue;
        }

        //首先优先选择,预防系统总数超过20
        for( u8CurSysOffset = 0; u8CurSysOffset < ( g_stObdPar.u8CacheSysTotal > 20 ? 0 : g_stObdPar.u8CacheSysTotal ); u8CurSysOffset++ )
        {
            //进来这里说明曾经扫描过同一个车系的系统,到这里看看有没有对应的系统
            if( g_stObdPar.SysPar[u8CurSysOffset].u8SysLibChoice != ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice )
            {
                continue;
            }

            //这里还得回到总表看看有没有这个系统,同时看看在总系统里面是哪一个
            for( u8SysChoiceOffsetTmp = 0; u8SysChoiceOffsetTmp < pstPersonObdChoice->u8SysTotal ; u8SysChoiceOffsetTmp++ )
            {
                if( g_stObdPar.SysPar[u8CurSysOffset].u8SysChoice != ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysChoice )
                {
                    continue;
                }

                if( g_stObdPar.SysPar[u8CurSysOffset].u8SysLibChoice != ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysLibChoice )
                {
                    continue;
                }

                //这里就得出啦当前的u8SysLibChoice+u8SysChoice
                bIsFindSys = TRUE;
                break;
            }

            if( bIsFindSys )
            {
                break;
            }
        }

        //判断是不是真的找到
        if( !bIsFindSys )
        {
            //找不到回复为默认值
            u8SysChoiceOffsetTmp = u8SysChoiceOffset;
        }

        //其次开始执行
        fun_scan_fun_fun( ( TYPEDEF_SYS_CFG* ) & ( ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp] ), &stFunStatus );

        //接着判断是否成功
        if( OK == stFunStatus )
        {
            //避免重新扫描
            *pstFunStatus = stFunStatus;
            //个数增加
            u8SysChoiceSuccess[0] += 1;
            //存储当前成功系统
            u8SysChoiceSuccess[u8SysChoiceSuccess[0]] = ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysLibChoice;

            //如果不是找到成功的
            if( !bIsFindSys )
            {
                //往数组里面插入,在尾巴插入
                g_stObdPar.SysPar[g_stObdPar.u8CacheSysTotal % 20].u8SysLibChoice =
                    ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysLibChoice;
                g_stObdPar.SysPar[g_stObdPar.u8CacheSysTotal % 20].u8SysChoice =
                    ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysChoice;
                g_stObdPar.u8CacheSysTotal++;
            }
        }

        else if( bIsFindSys )
        {
            //说明失败啦,进行相关处理
            g_stObdPar.u8CacheSysTotal--;
            memmove(
                &g_stObdPar.SysPar[u8CurSysOffset],
                &g_stObdPar.SysPar[u8CurSysOffset + 1], ( g_stObdPar.u8CacheSysTotal - u8CurSysOffset ) * 2 );
            //必须加上这句话,如果是找到失败的话就必须按照默认的进行扫描
            continue;
        }

        u8SysChoiceOffset++;
        OSTimeDlyHMSM( 0, 0, ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u16SysDelay / 1000,
                       ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u16SysDelay % 1000 );
    }

    //这里还得重新更新一下flash本身的存储表

    return ( void* )0;
}
/*
*******************************************************************************
Name       :fun_scan_fun_fun
Description:该函数主要是针对当前车型所需要执行的功能,这里的延时是帧与帧之间的延时
Input      :none
Output     :none
Return     :none
Others     :这里一定要确定一个变量g_pModeLibTab，应为其他功能或者其他任务得用上它
********************************************************************************
*/
void fun_scan_fun_fun( TYPEDEF_SYS_CFG *pstsysChoice, TYPEDEF_FRAME_STATUS * pstFunStatus )
{
    uint8 u8FunChoiceOffset = 0;
    void *pCommStatus = NULL;
    //首先设置好,系统命令获取表
    g_pModeLibTab = &( g_pstSysLibTab[pstsysChoice->u8SysLibChoice] )[pstsysChoice->u8SysChoice];

    for( ; u8FunChoiceOffset < pstsysChoice->u8FunTotal; u8FunChoiceOffset++ )
    {

        //执行的是哪一个功能中的哪一个函数
        //这里传入2个参数
        //参数1:当前选择的系统
        //参数2:当前功能需要的参数
        pCommStatus =
            ( * ( ( g_pFunLib[( pstsysChoice->pstFunction )[u8FunChoiceOffset].u8FunLibChoice] ) + ( pstsysChoice->pstFunction )[u8FunChoiceOffset].u8FunChoice ) )
            (
                pstsysChoice,
                ( void* )( pstsysChoice->pstFunction )[u8FunChoiceOffset].pFunctionParameter
            );

        //条件1：默认成功是继续发送下一帧命令的,失败直接跳出去,扫描下一个系统
        //条件2：如果失败可定是不再发送下一条命令了，返回到系统扫描
        //条件3: 如果读取故障码成功那么就没有必要进行下一个系统扫描
        switch( *( TYPEDEF_FRAME_STATUS* ) pCommStatus )
        {

            case FRAMETIMEOUT:
            case TIMEOUT:
                {
                    return;
                }

            default:
                {
                    //这里处理的是主要成功都得记住状态
                    switch( ( pstsysChoice->pstFunction )[u8FunChoiceOffset].u8FunLibChoice )
                    {
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 6:
                            {
                                //设置标志位告知已经有车型读取数据流成功过,记住该变量可是全局的
                                g_stCarScanSuccess = *( TYPEDEF_FRAME_STATUS* ) pCommStatus;
                                //这个变量只是提供给系统扫描使用
                                *pstFunStatus = g_stCarScanSuccess;
                                break;
                            }

                        default:
                            break;
                    }

                    break;
                }
        };
    }

    return;
}
/*
*******************************************************************************
Name       :fun_scan_car_fun
Description:该函数主要是为啦熄火逻辑进行循环扫描进行设置
Input      :none
Output     :none
Return     :none
Others     :1.首先执行个性功能，如果成功直接出去，否则执行obd功能更
2.通信失败接着扫描obd
********************************************************************************
*/
void fun_scan_poweroff_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus )
{

    uint8 u8CarChoiceOffset  = 0;
	uint16 u16SaveRpmTemp = 0;
    for( ; u8CarChoiceOffset < 2; u8CarChoiceOffset++ )
    {
        //建立循环方式
        if( pstCarChoice->stPersonAndObdCfg[u8CarChoiceOffset].u8SysTotal == 0 )
        {
            continue ;
        }
        else
        {
            fun_scan_poweroff_car_fun( &pstCarChoice->stPersonAndObdCfg[u8CarChoiceOffset], &( *pstFunStatus ) );
        }

        if( *pstFunStatus == OK )
        {
			
			u16SaveRpmTemp = ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[17] << 8 |
			                 ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[18] ;
			
			u16SaveRpmTemp/=4;
			//直接赋值转速信息
			g_stObdData.proParaBuf[0] = u16SaveRpmTemp >> 8;			
			g_stObdData.proParaBuf[1] = u16SaveRpmTemp ;				
//            //成功之后判断车速
//            //判断车速是否为100
//            if( ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[14] ) != 0x00 ) &
//                    ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[17] << 8 |
//                        ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[18] ) <= 100 ) )
//            {
//                //车速小于100
//                g_stPhoneCommandShare.u8VehicleState = 0xaa;
//            }
//            else
//            {
//                //车速
//                g_stPhoneCommandShare.u8VehicleState = 0xbb;
//            }

            break;
        }
        else
        {
            //失败
            g_stPhoneCommandShare.u8VehicleState = 00;
        }
    }

    return;
}

/*
*******************************************************************************
Name       :fun_scan_poweroff_car_fun
Description:获取记忆车型
Input      :none
Output     :none
Return     :none
Others     :

********************************************************************************
*/
void *fun_scan_poweroff_car_fun( TYPEDEF_All_SYS_CFG *pstPersonObdChoice, TYPEDEF_FRAME_STATUS * pstFunStatus )
{
    uint8 u8SysChoiceOffsetTmp = 0;
    uint8 u8CurSysOffset = 0;
    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;

    //判断是否有记忆
    if( g_stObdParVehicleState.u8CacheSysTotal != 0 )
    {

        for( u8CurSysOffset = 0; u8CurSysOffset < g_stObdPar.u8CacheSysTotal ; u8CurSysOffset++ )
        {
            if( 0 != g_stObdParVehicleState.SysPar[u8CurSysOffset].u8SysLibChoice )
            {
                continue;
            }

            if( 8 < g_stObdParVehicleState.SysPar[u8CurSysOffset].u8SysChoice )
            {
                continue;
            }

            break;
        }


        for( u8SysChoiceOffsetTmp = 0; u8SysChoiceOffsetTmp < pstPersonObdChoice->u8SysTotal ; u8SysChoiceOffsetTmp++ )
        {
            if( g_stObdParVehicleState.SysPar[u8CurSysOffset].u8SysChoice != ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysChoice )
            {
                continue;
            }

            if( g_stObdParVehicleState.SysPar[u8CurSysOffset].u8SysLibChoice != ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysLibChoice )
            {
                continue;
            }

            break;
        }

        //带记忆执行
        fun_scan_fun_fun( ( TYPEDEF_SYS_CFG* ) & ( ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp] ), &stFunStatus );

//        //如果带记忆功能通信不成功，那么下次直接轮询
//        if( stFunStatus != OK )
//        {
//            g_stObdParVehicleState.u8CacheSysTotal = 0 ;
//        }
    }
    else
    {
        stFunStatus = TIMEOUT;
        //注意这里的改变
        memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
        //g_stObdParVehicleState
        pfun_scan_sys_noremem_fun( pstPersonObdChoice, &stFunStatus );
    }

    *pstFunStatus = stFunStatus;
    return ( void* )0;
}

/*
*******************************************************************************
Name        :pfun_scan_sys_noremem_fun
Description:该函数的主要功能是不带记忆的进行扫描
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void *pfun_scan_sys_noremem_fun( TYPEDEF_All_SYS_CFG *pstPersonObdChoice, TYPEDEF_FRAME_STATUS * pstFunStatus )
{
    uint8 u8SysChoiceOffset = 0;
    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
    uint8 u8SysChoiceSuccess[21] = {0};

    //系统扫描
    for( ; u8SysChoiceOffset < pstPersonObdChoice->u8SysTotal ; u8SysChoiceOffset++ )
    {
        //这里最好重新初始化一遍
        stFunStatus = TIMEOUT;

        //这里开始避免同一款车多种协议重复扫描
        if( NULL != memchr( &u8SysChoiceSuccess[1],
                            ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice, u8SysChoiceSuccess[0] ) )
        {
            continue;
        }

        //系统与系统之间的扫描是不用考虑通讯状态的,只要有一个成功就OK
        fun_scan_fun_fun( ( TYPEDEF_SYS_CFG* ) & ( ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset] ), &stFunStatus );

        //将该类型的系统保存起来,避免重新扫描为pfun_scan_sys_fun做准备
        if( OK == stFunStatus )
        {
            //个数增加
            u8SysChoiceSuccess[0] += 1;
            //存储当前成功系统
            u8SysChoiceSuccess[u8SysChoiceSuccess[0]] = ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice;
            //只要有一个系统读取故障码或者数据流,都上传成功状态
            *pstFunStatus = stFunStatus;
            //往数组里面插入,在尾巴插入
            g_stObdParVehicleState.SysPar[g_stObdParVehicleState.u8CacheSysTotal % 20].u8SysLibChoice =
                ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice;
            g_stObdParVehicleState.SysPar[g_stObdParVehicleState.u8CacheSysTotal % 20].u8SysChoice =
                ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysChoice;
            g_stObdParVehicleState.u8CacheSysTotal++;
            //只要记住一个就可以
            break;
        }

        OSTimeDlyHMSM( 0, 0, ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u16SysDelay / 1000, ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u16SysDelay % 1000 );
    }

    return ( void* )0;
}

