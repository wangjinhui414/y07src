/*
********************************************************************************
File name  : fun_obd_task_transferlayer_process.c
Description:
History    :
修改日期   : 修改者 : 修改内容:简单详述
2014/6/4     小老鼠   该文件主要是处理和硬件部门相关交互数据
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define ZXT_OBD_TRANSFERLAYER_PROCESS_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
********************************************************************************
Name       :fun_obd_task_transferlayer_process
Description:主要是处理和硬件部门关于OBD部分数据
Input      :void *pArg没有用上
Output     :none
Return     :none
Others     :1.
********************************************************************************
*/
void  fun_obd_task_transferlayer_process( void *pArg )
{
    //uint8  u8Error = 0;
    uint8 index;
    while( 1 )
    {
        //é¨?èóê??￡?è?1?óDD§′??óáD
        g_pstObdCmd = ( OBDMbox_TypeDef * )OSMboxAccept( OS_OBDReqMbox);
        if(g_pstObdCmd!= (void *)0)
        {
            //??è?ê?í?μY?°é???￡??aà?òa?a??￡?·??ò??è?ê??T·¨′?′￠D????￠
            g_pstObdCmd->dataLock = 0;
            //?D???óáDê?·??ú￡??ú???aí¨??2??|
            if(HAL_OBD_SaveCMD(g_pstObdCmd))
            {
                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                memcpy(&g_stObdData,g_pstObdCmd,sizeof(OBDMbox_TypeDef));
                g_stObdData.proParaLen = 0;
                g_stObdData.retCode = 1;
                OSMboxPost( OS_OBDAckMbox, &g_stObdData );
                g_pstObdCmd->dataLock = 0;
#ifdef OBDQUE_DEBUG
                myPrintf("OBD Cmd Queue full wait!!");
#endif
            }
        }
        //é¨?è?óáD
        for(index=0;    index<DEF_OBDQUE_NUM;   index++)
        {
            //?óáD?D???÷μ?óDD§×ê?′￡?è??óê±éè?aóDD§
            if(sa_obdQue[index].Step)
            {
#ifdef OBDQUE_DEBUG
                myPrintf("[index]:%d\r\n",index);
#endif
                //?D??3?ê±
                if(OSTmr_Count_Get(&sa_obdQue[index].WaitCounter) >= 120000)
                {
                    //′ó?óáD?Dêí·?
#ifdef OBDQUE_DEBUG
                    myPrintf("[EVENT]: timeout------->cmd:%d\r\n",g_stObdData.cmdCode);
#endif
                    sa_obdQue[index].Step =0;
#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
                    if(dbgInfoSwt & DBG_INFO_OTHER)
                    {
                        BSP_RTC_Get_Current(&tmpRtc);   // ?üD?ê±??
                        //myPrintf("[%02d/%02d/%02d %02d:%02d:%02d]",tmpRtc.year,tmpRtc.month,tmpRtc.day,tmpRtc.hour,tmpRtc.minute,tmpRtc.second);
                        //myPrintf("[Mailbox]: 2min Wait Timeout![cmdCode:%d][sn:%d][proCmdId:%04X][dataLen:%d]\r\n",s_obdQue[index].data.cmdCode,s_obdQue[index].data.sn,s_obdQue[index].data.proCmdId,s_obdQue[index].data.proParaLen);
                    }
#endif
                }
                else//?′3?ê±
                {
                    g_pstObdCmd = ( OBDMbox_TypeDef * )&(sa_obdQue[index].data);
#ifdef OBDQUE_DEBUG
                    myPrintf("[g_pstObdCmd]:%d\r\n",g_pstObdCmd->cmdCode);
#endif
                    while( g_stObdData.dataLock != DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
                    {
                        OSTimeDlyHMSM( 0, 0, 0, 10 );
                    }
                    if( g_stPhoneCommandShare.u8ReadDataStream == DEF_DATASTREAM_STATUS_START &&
                            g_pstObdCmd->proCmdId != DEF_CMD_PROTOCOL_MANUAL_DATASTREAM_STOP &&
                            g_pstObdCmd->proCmdId != DEF_CMD_PROTOCOL_MANUAL_DATASTREAM_START
                      )
                    {
                        //êy?Y?o3?′óD?
                        memset( &g_stObdData0, 0, sizeof( g_stObdData0 ) );
                        //êy?Yé???￡??Yê±?¨ò?ò???±?á?òa2??áoí?áè?êy?Yá÷ê±3?í?
                        g_stObdData0.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                        g_stObdData0 = ( *g_pstObdCmd );
                        g_stObdData0.proParaLen = 0;
                        g_stObdData0.retCode = 1;
                        while( g_stObdData.dataLock != DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
                        {
                            OSTimeDlyHMSM( 0, 0, 0, 10 );
                        }
                        OSMboxPost( OS_OBDAckMbox, &g_stObdData0 );
                        sa_obdQue[index].Step =0;
                        g_pstObdCmd->dataLock = DEF_CMD_PROTOCOL_MANUAL_UNLCOK;
                    }
                    //??óDD§×ê?′?D??￡?Dèòaó?μ?í¨D?è???μ?òa?ó??￡?·à?1á?D?á?ì?í¨????á?
                    else
                    {
                        switch( g_pstObdCmd->cmdCode )
                        {
                        case CMD_CAR_INFO:
                            //这里得将车型添加进来
                            g_stPhoneCommandShare.u8CurCarType = g_pstObdCmd->proParaBuf[0];
                            //点火开
                        case CMD_IG_ON:
                            //点火关,油耗里程
                        case CMD_IG_OFF:
                            //系统唤醒
                        case CMD_SYS_WAKEUP:
                            //读取车辆报警状态
                        case CMD_READ_ALARMSTA:
                        {
                            //数据上锁
                            g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                            memcpy(&g_stObdData,g_pstObdCmd,sizeof(OBDMbox_TypeDef));
                            g_stObdData.proParaLen = 0;
                            g_stObdData.retCode = 00;
                            //告知通信部
                            OSMboxPost( OS_OBDAckMbox, &g_stObdData );
                            g_pstObdCmd->dataLock = 0;
                            //释放资源位
                            sa_obdQue[index].Step =0;
                            break;
                        }
                        //体检
                        case CMD_AUTO_TJ:
                        {
                            //判断通信是否空闲
                            if(Check_Communication_Task_IDLE())
                            {
                                g_stObdData.sn = ( *g_pstObdCmd ).sn;
                                g_stObdData.proSn = ( *g_pstObdCmd ).proSn;
                                g_stObdData.proCmdId = ( *g_pstObdCmd ).proCmdId;
                                g_stObdData.cmdCode = g_pstObdCmd->cmdCode;
                                g_stPhoneCommandShare.u8PhoneFunctionChoice = 0;
                                OSSemPost( g_pstTransferLayerToSchedule );
                                sa_obdQue[index].Step =0;
                            }
                            break;
                        }
                        //协议
                        case CMD_PROTOCOL:
                        {
                            switch( g_pstObdCmd->proCmdId )
                            {
                                //手动体检
                            case DEF_CMD_PROTOCOL_MANUAL_EXAMINATION:
                            {
                                if(Check_Communication_Task_IDLE())
                                {
                                    //备份数据
                                    g_stObdData.sn = ( *g_pstObdCmd ).sn;
                                    g_stObdData.proSn = ( *g_pstObdCmd ).proSn;
                                    g_stObdData.proCmdId = ( *g_pstObdCmd ).proCmdId;
                                    g_stObdData.cmdCode = g_pstObdCmd->cmdCode;
                                    g_stPhoneCommandShare.u8PhoneFunctionChoice = 0;
                                    OSSemPost( g_pstTransferLayerToSchedule );
                                    sa_obdQue[index].Step =0;
                                }
                                break;
                            }
                            //清码
                            case DEF_CMD_PROTOCOL_MANUAL_CLEAR_DTC:
                            {
                                //数据流开启的时候不允许接收新指令，必须得解锁
                                if( g_stPhoneCommandShare.u8ReadDataStream == DEF_DATASTREAM_STATUS_START )
                                {
                                    g_pstObdCmd->dataLock = DEF_CMD_PROTOCOL_MANUAL_UNLCOK;
                                    break;
                                }
                                if(Check_Communication_Task_IDLE())
                                {
                                    //备份数据
                                    g_stObdData.sn = ( *g_pstObdCmd ).sn;
                                    g_stObdData.proSn = ( *g_pstObdCmd ).proSn;
                                    g_stObdData.proCmdId = ( *g_pstObdCmd ).proCmdId;
                                    g_stObdData.cmdCode = g_pstObdCmd->cmdCode;
                                    g_stPhoneCommandShare.u8ClearDtc = 0x00;
                                    g_stPhoneCommandShare.u8PhoneFunctionChoice = 2;
                                    OSSemPost( g_pstTransferLayerToSchedule );
                                    sa_obdQue[index].Step =0;
                                }
                                break;
                            }
                            //数据流开启,这里得改,改成循环，进入数据流开启之后一直在这里死磕停止指令
                            case  DEF_CMD_PROTOCOL_MANUAL_DATASTREAM_START:
                            {
                                //注意这里数据放置结构长度参数内容+注意这里还得加上本身长度
                                memcpy( &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamIdBuf, ( uint8* )&g_pstObdCmd->proParaBuf[1], ( *g_pstObdCmd ).proParaLen - 1 );
                                ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Idlen = ( *g_pstObdCmd ).proParaLen - 1;
                                //如果一直在开启状态就没有必要一直投递邮箱
                                if( g_stPhoneCommandShare.u8ReadDataStream != DEF_DATASTREAM_STATUS_START )
                                {
                                    g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_START;
                                    if(Check_Communication_Task_IDLE())
                                    {
                                        //备份
                                        g_stObdData.sn = ( *g_pstObdCmd ).sn;
                                        g_stObdData.proSn = ( *g_pstObdCmd ).proSn;
                                        g_stObdData.proCmdId = ( *g_pstObdCmd ).proCmdId;
                                        g_stObdData.cmdCode = g_pstObdCmd->cmdCode;
                                        //状态查询做准备
                                        g_stPhoneCommandShare.u8PhoneFunctionChoice = 6;
                                        OSSemPost( g_pstTransferLayerToSchedule );
                                        sa_obdQue[index].Step =0;
                                    }
                                }
                                //如果不投递，释放资源
                                else
                                {
                                    sa_obdQue[index].Step =0;
                                }
                                //解锁命令缓存
                                g_pstObdCmd->dataLock = DEF_CMD_PROTOCOL_MANUAL_UNLCOK;
                                break;
                            }
                            //数据流结束
                            case DEF_CMD_PROTOCOL_MANUAL_DATASTREAM_STOP:
                            {
                                //设置结束标志位
                                //注意这里数据放置结构长度+报文系列号+命令标识—+参数内容
                                if( g_stPhoneCommandShare.u8ReadDataStream == DEF_DATASTREAM_STATUS_START )
                                {
                                    while( g_stObdData.dataLock != DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
                                    {
                                        OSTimeDlyHMSM( 0, 0, 0, 10 );
                                    }
                                    g_stObdData.sn = ( *g_pstObdCmd ).sn;
                                    g_stObdData.proSn = ( *g_pstObdCmd ).proSn;
                                    g_stObdData.proCmdId = ( *g_pstObdCmd ).proCmdId;
                                    g_stObdData.cmdCode = g_pstObdCmd->cmdCode;
                                    g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_ACTIVE_STOP;
                                }
                                else
                                {
                                    //被认为是误操作必须解锁
                                    g_pstObdCmd->dataLock = DEF_CMD_PROTOCOL_MANUAL_UNLCOK;
                                }
                                //释放资源
                                sa_obdQue[index].Step =0;
                                break;
                            }
                            //设置里程
                            case DEF_CMD_PROTOCOL_MANUAL_SET_ODO:
                            {
                                //数据流开启的时候不允许接收新指令，必须得解锁
                                if( g_stPhoneCommandShare.u8ReadDataStream == DEF_DATASTREAM_STATUS_START )
                                {
                                    g_pstObdCmd->dataLock = DEF_CMD_PROTOCOL_MANUAL_UNLCOK;
                                    //释放
                                    sa_obdQue[index].Step =0;
                                    break;
                                }
                                //备份
                                g_stObdData = *g_pstObdCmd;
                                g_stObdPar.u32OdoMeter = g_stObdData.proParaBuf[1] << 16 | g_stObdData.proParaBuf[2] << 8 | g_stObdData.proParaBuf[3];
                                g_u32CalibrationOdo =  g_stObdPar.u32OdoMeter;
                                //写入FLASH
                                HAL_SaveParaPro( DEF_TYPE_SYSOBD, &g_stObdPar, sizeof( g_stObdPar ) );
                                memset( ( void* )g_stObdData.proParaBuf, 0x00, g_stObdData.proParaLen );
                                g_stObdData.proParaLen = 0;
                                g_stObdData.retCode = 0;
                                //告知通信部
                                OSMboxPost( OS_OBDAckMbox, &g_stObdData );
                                g_pstObdCmd->dataLock = 0;
                                //释放资源
                                sa_obdQue[index].Step =0;
                                break;
                            }
                            case DEF_CMD_PROTOCOL_MANUAL_CONTROL_IO:
                            {
                            }
                            default:
                            {
                                //数据上锁
                                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                                memcpy(&g_stObdData,g_pstObdCmd,sizeof(OBDMbox_TypeDef));
                                memset( ( void* )g_stObdData.proParaBuf, 0x00, g_stObdData.proParaLen );
                                g_stObdData.proParaLen = 0;
                                g_stObdData.retCode = 2;
                                //告知通信部
                                OSMboxPost( OS_OBDAckMbox, &g_stObdData );
                                g_pstObdCmd->dataLock = 0;
                                //释放队列资源
                                sa_obdQue[index].Step =0;
                                break;
                            }
                            }

                            break;
                        }
                        //测试总线
                        case CMD_TEST:
                        {
                            if(Check_Communication_Task_IDLE())
                            {
                                if( 0 == g_pstObdCmd->proParaBuf[0] )
                                {
                                    g_stPhoneCommandShare.u8PhoneFunctionChoice = 4;
                                }
                                else if( 1 == g_pstObdCmd->proParaBuf[0] )
                                {
                                    g_stPhoneCommandShare.u8PhoneFunctionChoice = 3;
                                }
                                else if( 3 == g_pstObdCmd->proParaBuf[0] )
                                {
                                    g_stPhoneCommandShare.u8PhoneFunctionChoice = 7;
                                }
                                else
                                {
                                    break;
                                }
                                //备份数据
                                g_stObdData.sn = ( *g_pstObdCmd ).sn;
                                g_stObdData.proSn = ( *g_pstObdCmd ).proSn;
                                g_stObdData.proCmdId = ( *g_pstObdCmd ).proCmdId;
                                g_stObdData.cmdCode = g_pstObdCmd->cmdCode;
                                OSSemPost( g_pstTransferLayerToSchedule );
                                sa_obdQue[index].Step =0;
                            }
                            break;
                        }
                        //获取车辆状态
                        case CMD_GET_CARDATA :
                        {
                            //判断通信是否空闲
                            if(Check_Communication_Task_IDLE())
                            {
                                g_stObdData.sn = ( *g_pstObdCmd ).sn;
                                g_stObdData.proSn = ( *g_pstObdCmd ).proSn;
                                g_stObdData.proCmdId = ( *g_pstObdCmd ).proCmdId;
                                g_stObdData.cmdCode = g_pstObdCmd->cmdCode;
                                g_stPhoneCommandShare.u8PhoneFunctionChoice = 8;
                                OSSemPost( g_pstTransferLayerToSchedule );
                                sa_obdQue[index].Step =0;
                            }
                            break;
                        }
                        //VIN
                        case CMD_READ_VIN:
                        {
                            //判空闲
                            if(Check_Communication_Task_IDLE())
                            {
                                g_stObdData.sn = ( *g_pstObdCmd ).sn;
                                g_stObdData.proSn = ( *g_pstObdCmd ).proSn;
                                g_stObdData.proCmdId = ( *g_pstObdCmd ).proCmdId;
                                g_stObdData.cmdCode = g_pstObdCmd->cmdCode;
                                g_stPhoneCommandShare.u8PhoneFunctionChoice = 9;
                                OSSemPost( g_pstTransferLayerToSchedule );
                                sa_obdQue[index].Step =0;
                            }
                            break;
                        }
                        default:
                        {
                            //上锁
                            g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                            memcpy(&g_stObdData,g_pstObdCmd,sizeof(OBDMbox_TypeDef));
                            memset( ( void* )g_stObdData.proParaBuf, 0x00, g_stObdData.proParaLen );
                            g_stObdData.proParaLen = 0;
                            g_stObdData.retCode = 2;
                            //告知嵌入式
                            OSMboxPost( OS_OBDAckMbox, &g_stObdData );
                            g_pstObdCmd->dataLock = 0;
                            //释放资源
                            sa_obdQue[index].Step =0;
                            break;
                        }
                        }
                    }
                }
            }
        }
				OSTimeDlyHMSM( 0, 0, 0, 10 );
    }
}
/*******************************************************************************
* Function Name  : HAL_OBD_SaveCMD
* Description    : OBD?óê???è?ê?óê??oó′?è??óáD
* Input          : óê??????
* Output         :
* Return         : 0±íê?ò?′?′￠￡?-1±íê??óáD?ú
*******************************************************************************/
s8  HAL_OBD_SaveCMD(OBDMbox_TypeDef *obdmbox)
{
    s8  ret=0;
    u8  i=0;
    CPU_SR  cpu_sr=0;
    OS_ENTER_CRITICAL();    //áù??êy?Y±￡?¤
    //?éó?×ê?′?????? Step:0,?éó?    1,ò???ó?
    for(i=0;    i<DEF_OBDQUE_NUM;   i++)
    {
        if(sa_obdQue[i].Step == 0)
        {
            break;
        }
    }
    if(i < DEF_OBDQUE_NUM)
    {
        memset((u8 *)&sa_obdQue[i], 0,  sizeof(sa_obdQue[i]));
        OSTmr_Count_Start(&sa_obdQue[i].WaitCounter);           // è??ó??ê±￡?é¨?è?óáDê±è?3??D??3?ê±
        // ???￠è??ó
        sa_obdQue[i].data.dataLock = 1; // é???￡??????Yê±??ó?
        OS_EXIT_CRITICAL();
        sa_obdQue[i].data.cmdCode   = obdmbox->cmdCode;
        sa_obdQue[i].data.sn                = obdmbox->sn;
        sa_obdQue[i].data.proSn         = obdmbox->proSn;
        sa_obdQue[i].data.proCmdId  = obdmbox->proCmdId;
        sa_obdQue[i].data.proParaLen    =   obdmbox->proParaLen;
        if((obdmbox->proParaLen != 0) && (obdmbox->proParaLen <= DEF_CMDBUF_SIZE))
        {
            memcpy((u8 *)sa_obdQue[i].data.proParaBuf,    (SYSCommon_Typedef*)&(obdmbox->proParaBuf), obdmbox->proParaLen);
        }
        // éè??×ê?′??ó?±ê??
        sa_obdQue[i].Step =1;
#if(DEF_IGCHECKLOGIC_OUTPUTEN > 0)
        if(dbgInfoSwt & DBG_INFO_OTHER)
#ifdef OBDQUE_DEBUG
            myPrintf("[OBD_Que]: -->Que:%d,sn:%d!!\r\n",i,sa_obdQue[i].data.sn);
#endif
#endif
        ret= 0;
    }
    else
    {
        OS_EXIT_CRITICAL();
#if(DEF_EVENTINFO_OUTPUTEN > 0)
        if(dbgInfoSwt & DBG_INFO_EVENT)
#ifdef OBDQUE_DEBUG
            myPrintf("[EVENT]: OBD Cmd Queue full!![cmdCode:%d]\r\n",obdmbox->cmdCode);
#endif
#endif
        ret = -1;
    }
    return ret;
}
/*******************************************************************************
* Function Name  : IF_OSSemPost
* Description    : ?D??ê?·?·￠?íD?o?á?
* Input          : D?o?á?????
* Output         :
* Return         : 0±íê?í¨??è????y?ú?′DD￡?ò??-é????Y2?′|àí￡?1±íê?í¨??è??????D
*******************************************************************************/
s8  Check_Communication_Task_IDLE(void)
{
    if(Communication_task_lock == 0)
    {
#ifdef OBDQUE_DEBUG
        myPrintf("[EVENT]: OBD Communication------->lock:%d\r\n",g_pstObdCmd->cmdCode);
#endif
        Communication_task_lock=1;
        return 1;
    }
    else
    {
#ifdef OBDQUE_DEBUG
        myPrintf("[EVENT]: wait------->:%d\r\n",g_pstObdCmd->cmdCode);
#endif
        return 0;
    }
}
