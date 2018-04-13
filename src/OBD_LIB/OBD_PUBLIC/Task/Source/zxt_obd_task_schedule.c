/*
********************************************************************************
File name  : Zxt_task_for_obd_schedule.c
Description:
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define ZXT_OBD_TASK_SCHEDULE_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
void fun_obd_task_schedule( void *pArg )
{
    uint8 u8Error;
    SYS_DATETIME    tmpRtc;
    uint32 u32count = 0;


    //获取系统参数
    if( HAL_LoadParaPro( DEF_TYPE_SYSOBD, &g_stObdPar, sizeof( g_stObdPar ) ) == DEF_PARA_INIT_ERR )
    {
        memset( &g_stObdPar, 0, sizeof( g_stObdPar ) );
	    g_stObdPar.u32OdoMeter = 0x00ffffff;//add- liu 2016-9-9 第一次不能清除累加里程的值
        HAL_SaveParaPro( DEF_TYPE_SYSOBD, &g_stObdPar, sizeof( g_stObdPar ) );
    }
    while( 1 )
    {
        OSSemPend( g_pstTransferLayerToSchedule, 0, &u8Error );
        //然后开始投递信号量
        OSSemPost( g_pstScheduleToFunAnalyzer );
        OSSemPend( g_pstFunAnalyzerToSchedule, 0, &u8Error );
//        //这里只是简单的将里程覆盖一遍
//        g_stObdPar.u32OdoMeter = ( !g_u32CalibrationOdo ) ? g_stObdPar.u32OdoMeter : g_u32CalibrationOdo;
//        g_u32CalibrationOdo = 0;
        //首先判断上行缓存是不是为可用,也就是判断是不是已经解锁
        while( g_stObdData.dataLock != DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
        {
            //隔一段时间查询一下
            OSTimeDlyHMSM( 0, 0, 0, 10 );
        }

        BSP_RTC_Get_Current( &tmpRtc );
        u32count = Mktime( tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second );
        {
            //step1:这里是为了上报结果
            switch( g_stPhoneCommandShare.u8PhoneFunctionChoice )
            {
                //首次+手动体检
            case 0:
            case 1:
            {
                uint8 *pu8DataBuf = ( uint8* )g_stObdData.proParaBuf;
                //首先判断是成功还是失败
                if( g_stCarScanSuccess != OK )
                {
                    //打包帧头
                    pack_to_hal_headpack_head( 18, g_stObdData.cmdCode, 0x01, 0x0294, &g_stObdData );
                    //打包序列号
                    pu8DataBuf = ppack_to_hal_pack_word( g_stObdData.proSn, pu8DataBuf );
                    //打包时间
                    pu8DataBuf = ppack_to_hal_pack_dword( u32count, pu8DataBuf );
                    //打包外设类型
                    pu8DataBuf = ppack_to_hal_pack_word( 0x0C, pu8DataBuf );
                    //打包长度
                    pu8DataBuf = ppack_to_hal_pack_dword( 0x04, pu8DataBuf );
                    //打包通信状态
                    pu8DataBuf = ppack_to_hal_pack_dword( 0x00, pu8DataBuf );
                }
                else
                {
                    uint8 u8DsDataLen = ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Datalen;
                    //13个字节固定长+2个字节车型车系
                    uint16 u16DataLen = 3 + g_stFunMsgBuf[0].u16Len + u8DsDataLen + 12 + 1 + 2;
                    //打包帧头
                    pack_to_hal_headpack_head( u16DataLen, g_stObdData.cmdCode, 0x00, 0x0294, &g_stObdData );
                    //打包序列号
                    pu8DataBuf = ppack_to_hal_pack_word( g_stObdData.proSn, pu8DataBuf );
                    //打包时间
                    pu8DataBuf = ppack_to_hal_pack_dword( u32count, pu8DataBuf );
                    //打包外设类型
                    pu8DataBuf = ppack_to_hal_pack_word( 0x0C, pu8DataBuf );
                    //打包长度
                    pu8DataBuf = ppack_to_hal_pack_dword( u16DataLen, pu8DataBuf );
                    //打包通信状态
                    *pu8DataBuf++ = 1;
                    //打包里程
                    pu8DataBuf = ppack_to_hal_pack_data( g_stFunMsgBuf[2].pu8FunBuffer, pu8DataBuf, 3 );
                    //打包故障
                    pu8DataBuf = ppack_to_hal_pack_data( g_stFunMsgBuf[0].pu8FunBuffer, pu8DataBuf, g_stFunMsgBuf[0].u16Len );
                    //打包数据流
                    pu8DataBuf = ppack_to_hal_pack_data(
                                     ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, pu8DataBuf, u8DsDataLen );
                }
                //添加车型代码
                *pu8DataBuf++ = 0X01;
                *pu8DataBuf++ = ( g_stPhoneCommandShare.u8CurCarType != g_stPhoneCommandShare.u8DefaultCarType ) ? 'O' : g_stPhoneCommandShare.u8DefaultCarType;
                //车型车系不匹配返回3
                g_stObdData.retCode = ( g_stPhoneCommandShare.u8CurCarType != g_stPhoneCommandShare.u8DefaultCarType ) ? 3 : g_stObdData.retCode;
                //上传通讯部之前重新赋值指令字段，防止并行指令将该字段覆盖
                g_stObdData.cmdCode = CMD_AUTO_TJ;
                //告知通信部
                fun_post_hal_result();
                break;
            }
            //这里就是清除故障码成功与否进行的后续处理
            case 2:
            {
                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                g_stObdData.proParaLen = 0x07;
                //命令标识
                g_stObdData.proCmdId = 0x0a05;
                //报文序列号
                g_stObdData.proParaBuf[0] = g_stObdData.proSn >> 8;
                g_stObdData.proParaBuf[1] = g_stObdData.proSn;
                //时间
                g_stObdData.proParaBuf[2] = u32count >> 24;
                g_stObdData.proParaBuf[3] = u32count >> 16;
                g_stObdData.proParaBuf[4] = u32count >> 8;
                g_stObdData.proParaBuf[5] = u32count;
                //
                if( g_stCarScanSuccess != OK )
                {
                    g_stObdData.proParaBuf[6] = 0x03;
                    g_stObdData.retCode = 0x1;
                }
                else
                {
                    g_stObdData.proParaBuf[6] = g_stPhoneCommandShare.u8ClearDtc;
                    g_stObdData.retCode = 0x00;
                }
                //上传通讯部之前重新赋值指令字段，防止并行指令将该字段覆盖
                g_stObdData.cmdCode = CMD_PROTOCOL;
                //告知通信部
                fun_post_hal_result();
                break;
            }
            case 3:
            case 4:
            case 7:
            {
                //工装检测
                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                g_stObdData.proParaLen = 0x00;
                g_stObdData.proCmdId =   0x0;
                if( g_stCarScanSuccess != OK )
                {
                    g_stObdData.retCode = DEF_CMD_TEST_FAILURE;
                }
                else
                {
                    g_stObdData.retCode = DEF_CMD_TEST_SUCESS;
                }
                //上传通讯部之前重新赋值指令字段，防止并行指令将该字段覆盖
                g_stObdData.cmdCode = CMD_TEST;
                //告知通信部
                fun_post_hal_result();
                break;
            }
            case 6:
            {
                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                g_stObdData.proParaLen = 0x01;
                g_stObdData.retCode = 00;
                //命令标识
                g_stObdData.proCmdId = 0x0290;
                if( g_stCarScanSuccess != OK )
                {
                    g_stObdData.proParaBuf[0] = DEF_DATASTREAM_STATUS_DATABUS_FAILURE;
                }
                else
                {
                    g_stObdData.proParaBuf[0] = g_stPhoneCommandShare.u8ReadDataStream;
                }
                //回复为默认值，预防第一次手动获取数据成功，第二次失败，第三次不能再读取数据流
                g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_ACTIVE_STOP;
                //重新赋值命令码字段，防止该指令正在执行的时候被新指令覆盖
                g_stObdData.cmdCode = CMD_PROTOCOL;
                //告知通信部
                fun_post_hal_result();
                break;
            }
            //熄火逻辑
            case 8:
            {
                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                g_stObdData.proParaLen = 0x00;
                g_stObdData.retCode = 00;
                if( g_stCarScanSuccess != OK )
                {
                    g_stObdData.retCode = 0x01;
                }
                else
                {
                    g_stObdData.proParaLen = 0x02;
//                            g_stObdData.proParaBuf[0] = g_stPhoneCommandShare.u8VehicleState;
                }
                //重新赋值命令码字段，防止该指令正在执行的时候被新指令覆盖
                g_stObdData.cmdCode = CMD_GET_CARDATA;
                //告知通信部
                fun_post_hal_result();
                break;
            }
            //VIN打包
            case 9:
            {
                uint8 *pu8DataBuf = ( uint8* )g_stObdData.proParaBuf;
                //打包头
                pack_to_hal_headpack_head( 17, CMD_PROTOCOL, 0x00, 0x0292, &g_stObdData );
                pu8DataBuf = ppack_to_hal_pack_data( g_stFunMsgBuf[3].pu8FunBuffer, pu8DataBuf, g_stFunMsgBuf[3].u16Len );
                //重新赋值命令码字段，防止该指令正在执行的时候被新指令覆盖
                g_stObdData.cmdCode = CMD_PROTOCOL;
                //告知通信部
                fun_post_hal_result();
                break;
            }
            default:
            {
                break;
            }
            };
            //OBD通讯任务结束，解锁
#ifdef OBDQUE_DEBUG
            myPrintf("[EVENT]: OBD Communication------->unlock:%d\r\n",g_stObdData.cmdCode);
#endif
            Communication_task_lock=0;
            //读码和清码需要存储系其余不需要
            if(
                ( g_stPhoneCommandShare.u8PhoneFunctionChoice == 0 ) ||
                ( g_stPhoneCommandShare.u8PhoneFunctionChoice == 1 ) ||
                ( g_stPhoneCommandShare.u8PhoneFunctionChoice == 2 ) )
            {
                //存储系统
                if( g_stCarScanSuccess != OK )
                {
//					memset( &g_stObdPar.SysPar, 0, sizeof( g_stObdPar.SysPar ) );//屏蔽该代码-不能全部清0
					g_stObdPar.bCacheSyeFinish = FALSE ; //add- liu 2016-9-9 累加里程不能清除                   
					g_stObdPar.u8CacheSysTotal = 0;		 //add- liu 2016-9-9 31项的值不能清除
					g_stObdPar.u8CacheCarType = 0;		 //add- liu 2016-9-9 其他清除


                }

                else
                {

                    if( g_stPhoneCommandShare.u8CurCarType == g_stPhoneCommandShare.u8DefaultCarType )
                    {
                        g_stObdPar.u8CacheCarType = g_stPhoneCommandShare.u8DefaultCarType;
                    }

                    else
                    {
                        //如果车辆不匹存储O车系
                        g_stObdPar.u8CacheCarType = 'O';
                    }
                }

                //保存车辆信息
                HAL_SaveParaPro( DEF_TYPE_SYSOBD, &g_stObdPar, sizeof( g_stObdPar ) );
            }
        }
    }
}
/*
********************************************************************************
Name       :void pack_to_hal_headpack_to_hal_head(uint8 *pu8OdoInput)
Description:
Input      :
Output     :none
Return     :none
Others     :该函数主要是打包帧头
********************************************************************************
*/
void pack_to_hal_headpack_head( uint16            u16DataLen,
                                OBD_CMDCODE_TYPE  stCmdCode,
                                uint16            u16RetCode,
                                uint16            u16CmdId,
                                OBDMbox_TypeDef   *pstObdDataBuf )
{
    g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
    //设置长度
    pstObdDataBuf->proParaLen = u16DataLen;
    //设置ID
    pstObdDataBuf->proCmdId = u16CmdId;
    //命令代码
    pstObdDataBuf->cmdCode = stCmdCode;
    //返回代码
    pstObdDataBuf->retCode = u16RetCode;

}
/*
********************************************************************************
Name       :pack_to_hal_pack_time(uint32 u32PackTime,uint8 *pu8DataBuf)
Description:
Input      :
Output     :none
Return     :none
Others     :该函数主要用于4个字节打包
********************************************************************************
*/
uint8 *ppack_to_hal_pack_dword( uint32 u32PackTime, uint8 *pu8DataBuf )
{
    *pu8DataBuf++ = u32PackTime >> 24;
    *pu8DataBuf++ = u32PackTime >> 16;
    *pu8DataBuf++ = u32PackTime >> 8;
    *pu8DataBuf++ = u32PackTime;
    return pu8DataBuf;
}
/*
********************************************************************************
Name       :pack_to_hal_pack_cmdtype(uint32 u32CmdType,uint8 pu8DataBuf)
Description:
Input      :
Output     :none
Return     :none
Others     :该函数主要用于2个字节打包
********************************************************************************
*/
uint8 *ppack_to_hal_pack_word( uint16 u16CmdType, uint8 *pu8DataBuf )
{
    *pu8DataBuf++ = u16CmdType >> 8;
    *pu8DataBuf++ = u16CmdType;
    return pu8DataBuf;
}
/*
********************************************************************************
Name       :pack_to_hal_pack_data(uint32 u32CmdType,uint8 pu8DataBuf)
Description:
Input      :
Output     :none
Return     :none
Others     :该函数主要是任意数据
********************************************************************************
*/
uint8 *ppack_to_hal_pack_data( uint8 *pu8SourceData, uint8 *pu8ObjData, uint16 u16Len )
{
    memcpy( pu8ObjData, pu8SourceData, u16Len );
    pu8ObjData += u16Len;
    return pu8ObjData;
};
/*
********************************************************************************
Name       :fun_post_hal_result(void)
Description:
Input      :
Output     :none
Return     :none
Others     :该函数主要是告知数据通信部需要上传数据
********************************************************************************
*/
void fun_post_hal_result( void )
{
    //告知通信部
    OSMboxPost( OS_OBDAckMbox, &g_stObdData );

    //提前上报体检信息,这里都是暂时这么做的后续还得进行很大优化
    while( g_stObdData.dataLock != DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
    {
        OSTimeDlyHMSM( 0, 0, 0, 10 );
    }

    //解锁命令缓存
    g_pstObdCmd->dataLock = DEF_CMD_PROTOCOL_MANUAL_UNLCOK;
}
