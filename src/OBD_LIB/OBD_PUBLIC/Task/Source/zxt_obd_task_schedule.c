/*
********************************************************************************
File name  : Zxt_task_for_obd_schedule.c
Description:
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define ZXT_OBD_TASK_SCHEDULE_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
void fun_obd_task_schedule( void *pArg )
{
    uint8 u8Error;
    SYS_DATETIME    tmpRtc;
    uint32 u32count = 0;


    //��ȡϵͳ����
    if( HAL_LoadParaPro( DEF_TYPE_SYSOBD, &g_stObdPar, sizeof( g_stObdPar ) ) == DEF_PARA_INIT_ERR )
    {
        memset( &g_stObdPar, 0, sizeof( g_stObdPar ) );
	    g_stObdPar.u32OdoMeter = 0x00ffffff;//add- liu 2016-9-9 ��һ�β�������ۼ���̵�ֵ
        HAL_SaveParaPro( DEF_TYPE_SYSOBD, &g_stObdPar, sizeof( g_stObdPar ) );
    }
    while( 1 )
    {
        OSSemPend( g_pstTransferLayerToSchedule, 0, &u8Error );
        //Ȼ��ʼͶ���ź���
        OSSemPost( g_pstScheduleToFunAnalyzer );
        OSSemPend( g_pstFunAnalyzerToSchedule, 0, &u8Error );
//        //����ֻ�Ǽ򵥵Ľ���̸���һ��
//        g_stObdPar.u32OdoMeter = ( !g_u32CalibrationOdo ) ? g_stObdPar.u32OdoMeter : g_u32CalibrationOdo;
//        g_u32CalibrationOdo = 0;
        //�����ж����л����ǲ���Ϊ����,Ҳ�����ж��ǲ����Ѿ�����
        while( g_stObdData.dataLock != DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
        {
            //��һ��ʱ���ѯһ��
            OSTimeDlyHMSM( 0, 0, 0, 10 );
        }

        BSP_RTC_Get_Current( &tmpRtc );
        u32count = Mktime( tmpRtc.year, tmpRtc.month, tmpRtc.day, tmpRtc.hour, tmpRtc.minute, tmpRtc.second );
        {
            //step1:������Ϊ���ϱ����
            switch( g_stPhoneCommandShare.u8PhoneFunctionChoice )
            {
                //�״�+�ֶ����
            case 0:
            case 1:
            {
                uint8 *pu8DataBuf = ( uint8* )g_stObdData.proParaBuf;
                //�����ж��ǳɹ�����ʧ��
                if( g_stCarScanSuccess != OK )
                {
                    //���֡ͷ
                    pack_to_hal_headpack_head( 18, g_stObdData.cmdCode, 0x01, 0x0294, &g_stObdData );
                    //������к�
                    pu8DataBuf = ppack_to_hal_pack_word( g_stObdData.proSn, pu8DataBuf );
                    //���ʱ��
                    pu8DataBuf = ppack_to_hal_pack_dword( u32count, pu8DataBuf );
                    //�����������
                    pu8DataBuf = ppack_to_hal_pack_word( 0x0C, pu8DataBuf );
                    //�������
                    pu8DataBuf = ppack_to_hal_pack_dword( 0x04, pu8DataBuf );
                    //���ͨ��״̬
                    pu8DataBuf = ppack_to_hal_pack_dword( 0x00, pu8DataBuf );
                }
                else
                {
                    uint8 u8DsDataLen = ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Datalen;
                    //13���ֽڹ̶���+2���ֽڳ��ͳ�ϵ
                    uint16 u16DataLen = 3 + g_stFunMsgBuf[0].u16Len + u8DsDataLen + 12 + 1 + 2;
                    //���֡ͷ
                    pack_to_hal_headpack_head( u16DataLen, g_stObdData.cmdCode, 0x00, 0x0294, &g_stObdData );
                    //������к�
                    pu8DataBuf = ppack_to_hal_pack_word( g_stObdData.proSn, pu8DataBuf );
                    //���ʱ��
                    pu8DataBuf = ppack_to_hal_pack_dword( u32count, pu8DataBuf );
                    //�����������
                    pu8DataBuf = ppack_to_hal_pack_word( 0x0C, pu8DataBuf );
                    //�������
                    pu8DataBuf = ppack_to_hal_pack_dword( u16DataLen, pu8DataBuf );
                    //���ͨ��״̬
                    *pu8DataBuf++ = 1;
                    //������
                    pu8DataBuf = ppack_to_hal_pack_data( g_stFunMsgBuf[2].pu8FunBuffer, pu8DataBuf, 3 );
                    //�������
                    pu8DataBuf = ppack_to_hal_pack_data( g_stFunMsgBuf[0].pu8FunBuffer, pu8DataBuf, g_stFunMsgBuf[0].u16Len );
                    //���������
                    pu8DataBuf = ppack_to_hal_pack_data(
                                     ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, pu8DataBuf, u8DsDataLen );
                }
                //��ӳ��ʹ���
                *pu8DataBuf++ = 0X01;
                *pu8DataBuf++ = ( g_stPhoneCommandShare.u8CurCarType != g_stPhoneCommandShare.u8DefaultCarType ) ? 'O' : g_stPhoneCommandShare.u8DefaultCarType;
                //���ͳ�ϵ��ƥ�䷵��3
                g_stObdData.retCode = ( g_stPhoneCommandShare.u8CurCarType != g_stPhoneCommandShare.u8DefaultCarType ) ? 3 : g_stObdData.retCode;
                //�ϴ�ͨѶ��֮ǰ���¸�ֵָ���ֶΣ���ֹ����ָ����ֶθ���
                g_stObdData.cmdCode = CMD_AUTO_TJ;
                //��֪ͨ�Ų�
                fun_post_hal_result();
                break;
            }
            //����������������ɹ������еĺ�������
            case 2:
            {
                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                g_stObdData.proParaLen = 0x07;
                //�����ʶ
                g_stObdData.proCmdId = 0x0a05;
                //�������к�
                g_stObdData.proParaBuf[0] = g_stObdData.proSn >> 8;
                g_stObdData.proParaBuf[1] = g_stObdData.proSn;
                //ʱ��
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
                //�ϴ�ͨѶ��֮ǰ���¸�ֵָ���ֶΣ���ֹ����ָ����ֶθ���
                g_stObdData.cmdCode = CMD_PROTOCOL;
                //��֪ͨ�Ų�
                fun_post_hal_result();
                break;
            }
            case 3:
            case 4:
            case 7:
            {
                //��װ���
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
                //�ϴ�ͨѶ��֮ǰ���¸�ֵָ���ֶΣ���ֹ����ָ����ֶθ���
                g_stObdData.cmdCode = CMD_TEST;
                //��֪ͨ�Ų�
                fun_post_hal_result();
                break;
            }
            case 6:
            {
                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                g_stObdData.proParaLen = 0x01;
                g_stObdData.retCode = 00;
                //�����ʶ
                g_stObdData.proCmdId = 0x0290;
                if( g_stCarScanSuccess != OK )
                {
                    g_stObdData.proParaBuf[0] = DEF_DATASTREAM_STATUS_DATABUS_FAILURE;
                }
                else
                {
                    g_stObdData.proParaBuf[0] = g_stPhoneCommandShare.u8ReadDataStream;
                }
                //�ظ�ΪĬ��ֵ��Ԥ����һ���ֶ���ȡ���ݳɹ����ڶ���ʧ�ܣ������β����ٶ�ȡ������
                g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_ACTIVE_STOP;
                //���¸�ֵ�������ֶΣ���ֹ��ָ������ִ�е�ʱ����ָ���
                g_stObdData.cmdCode = CMD_PROTOCOL;
                //��֪ͨ�Ų�
                fun_post_hal_result();
                break;
            }
            //Ϩ���߼�
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
                //���¸�ֵ�������ֶΣ���ֹ��ָ������ִ�е�ʱ����ָ���
                g_stObdData.cmdCode = CMD_GET_CARDATA;
                //��֪ͨ�Ų�
                fun_post_hal_result();
                break;
            }
            //VIN���
            case 9:
            {
                uint8 *pu8DataBuf = ( uint8* )g_stObdData.proParaBuf;
                //���ͷ
                pack_to_hal_headpack_head( 17, CMD_PROTOCOL, 0x00, 0x0292, &g_stObdData );
                pu8DataBuf = ppack_to_hal_pack_data( g_stFunMsgBuf[3].pu8FunBuffer, pu8DataBuf, g_stFunMsgBuf[3].u16Len );
                //���¸�ֵ�������ֶΣ���ֹ��ָ������ִ�е�ʱ����ָ���
                g_stObdData.cmdCode = CMD_PROTOCOL;
                //��֪ͨ�Ų�
                fun_post_hal_result();
                break;
            }
            default:
            {
                break;
            }
            };
            //OBDͨѶ�������������
#ifdef OBDQUE_DEBUG
            myPrintf("[EVENT]: OBD Communication------->unlock:%d\r\n",g_stObdData.cmdCode);
#endif
            Communication_task_lock=0;
            //�����������Ҫ�洢ϵ���಻��Ҫ
            if(
                ( g_stPhoneCommandShare.u8PhoneFunctionChoice == 0 ) ||
                ( g_stPhoneCommandShare.u8PhoneFunctionChoice == 1 ) ||
                ( g_stPhoneCommandShare.u8PhoneFunctionChoice == 2 ) )
            {
                //�洢ϵͳ
                if( g_stCarScanSuccess != OK )
                {
//					memset( &g_stObdPar.SysPar, 0, sizeof( g_stObdPar.SysPar ) );//���θô���-����ȫ����0
					g_stObdPar.bCacheSyeFinish = FALSE ; //add- liu 2016-9-9 �ۼ���̲������                   
					g_stObdPar.u8CacheSysTotal = 0;		 //add- liu 2016-9-9 31���ֵ�������
					g_stObdPar.u8CacheCarType = 0;		 //add- liu 2016-9-9 �������


                }

                else
                {

                    if( g_stPhoneCommandShare.u8CurCarType == g_stPhoneCommandShare.u8DefaultCarType )
                    {
                        g_stObdPar.u8CacheCarType = g_stPhoneCommandShare.u8DefaultCarType;
                    }

                    else
                    {
                        //���������ƥ�洢O��ϵ
                        g_stObdPar.u8CacheCarType = 'O';
                    }
                }

                //���泵����Ϣ
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
Others     :�ú�����Ҫ�Ǵ��֡ͷ
********************************************************************************
*/
void pack_to_hal_headpack_head( uint16            u16DataLen,
                                OBD_CMDCODE_TYPE  stCmdCode,
                                uint16            u16RetCode,
                                uint16            u16CmdId,
                                OBDMbox_TypeDef   *pstObdDataBuf )
{
    g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
    //���ó���
    pstObdDataBuf->proParaLen = u16DataLen;
    //����ID
    pstObdDataBuf->proCmdId = u16CmdId;
    //�������
    pstObdDataBuf->cmdCode = stCmdCode;
    //���ش���
    pstObdDataBuf->retCode = u16RetCode;

}
/*
********************************************************************************
Name       :pack_to_hal_pack_time(uint32 u32PackTime,uint8 *pu8DataBuf)
Description:
Input      :
Output     :none
Return     :none
Others     :�ú�����Ҫ����4���ֽڴ��
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
Others     :�ú�����Ҫ����2���ֽڴ��
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
Others     :�ú�����Ҫ����������
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
Others     :�ú�����Ҫ�Ǹ�֪����ͨ�Ų���Ҫ�ϴ�����
********************************************************************************
*/
void fun_post_hal_result( void )
{
    //��֪ͨ�Ų�
    OSMboxPost( OS_OBDAckMbox, &g_stObdData );

    //��ǰ�ϱ������Ϣ,���ﶼ����ʱ��ô���ĺ������ý��кܴ��Ż�
    while( g_stObdData.dataLock != DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
    {
        OSTimeDlyHMSM( 0, 0, 0, 10 );
    }

    //���������
    g_pstObdCmd->dataLock = DEF_CMD_PROTOCOL_MANUAL_UNLCOK;
}
