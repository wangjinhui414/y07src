/*
********************************************************************************
File name  : Zxt_task_for_mobile_and_obd_device_communications.c
Description: ���ļ���Ҫ�ǽ��������񴫵ݹ����ľ��幦�ܽ���ִ��
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/

#define ZXT_OBD_TASK_FUN_PROCESS_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
*******************************************************************************
Name        :fun_obd_task_fun_process
Description:�ú�����Ҫ�Ǵ�����2���͹���������
Input      :void *pArg���������û������
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
        //��OBD��������Ͷ�ݹ���������
        OSSemPend( g_pstScheduleToFunAnalyzer, 0, &u8Error );

        u8FunChoiceStepTemp = g_stPhoneCommandShare.u8PhoneFunctionChoice;
        bFunExecutionFinish = TRUE;

        //��Ϊ��Щ������Ҫ����
        while( bFunExecutionFinish )
        {
            switch( u8Error )
            {

                case  OS_NO_ERR:
                    {
                        switch( u8FunChoiceStepTemp )
                        {
                            //���������ִ�ж�ȡ������Ͷ�ȡ������
                            case 0:
                                {
                                    //�����ȡOBD������ʧ����ô��û�б�Ҫ��ȡϵͳ������
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    g_stCarScanSuccess = TIMEOUT;
                                    memset( g_stFunMsgBuf[0].pu8FunBuffer, 0, g_stFunMsgBuf[0].u16Len );
                                    g_stFunMsgBuf[0].u16Len = 1;

                                    //��ȡ������
                                    fun_scan_vehmatch_fun( &g_stCarType[0], &stFunStatus, TRUE );

                                    //ֻ�ж�ȡ������ɹ����ܶ�ȡϵͳ������
                                    if( stFunStatus == OK )
                                    {
                                        bFunExecutionFinish = TRUE;
                                        u8FunChoiceStepTemp = 1;
                                        break;
                                    }

                                    else
                                    {
                                        //û�б�Ҫ��ȡ���
                                        bFunExecutionFinish = FALSE;
                                        break;
                                    }
                                }

                            //��ȡ������(��̬)
                            case 1:
                                {
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    //ע������ĸı�
                                    memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                    //��ȡ������
                                    fun_scan_vehmatch_fun( &g_stCarType[1], &stFunStatus, FALSE );

                                    if( stFunStatus != OK )
                                    {
                                        //������ʧ�ܣ��������ĳ��Ȼ��Ǵ��ڵ�,Ĭ��ȫ�����0
                                        ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )-> u8Datalen = 0x1f;
                                    }

                                    //ֱ�ӻ�ȡ���
                                    bFunExecutionFinish = TRUE;
                                    u8FunChoiceStepTemp = 5;
                                    break;
                                }

                            //���������
                            case 2:
                                {
                                    //�ܽ������빦��˵���й�����,ͬʱ�ֻ����Ѿ�������������
                                    //���ݱ���,��Ϊ������̻�Ӱ�쳵��ϵͳ����
                                    TYPEDEF_OBD_PAR g_stObdParTmp = g_stObdPar;
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    g_stCarScanSuccess = TIMEOUT;
                                    memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                    fun_scan_cdtc_fun( &stFunStatus );
                                    //���ݻ�ԭӰ�쳵��ϵͳ����
                                    g_stObdPar = g_stObdParTmp;
                                    //���������֮��,�������
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //K�߼��
                            case 3:
                                {
                                    uint8 u8Aprg = 0;
                                    g_pModeLibTab = &( g_pstSysLibTab[0] )[0];
                                    g_stCarScanSuccess = *( TYPEDEF_FRAME_STATUS* )( *( g_pFunLib[0] + 0 ) )( ( void* )0, &u8Aprg );
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //can�߼��
                            case 4:
                                {
                                    uint8 u8Aprg = 0;
                                    g_pModeLibTab = &( g_pstSysLibTab[0] )[3];
                                    g_stCarScanSuccess = *( TYPEDEF_FRAME_STATUS* )( *( g_pFunLib[0] + 2 ) )( ( void* )0, &u8Aprg );
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //can�߼��
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

                            //��ȡ���
                            case 5:
                                {
                                    //���ܳɹ���񶼵�ɨ�����
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    uint16 u16DataStreamCurpid31 = 0;
                                    g_stFunMsgBuf[2].u16Len = 3;
                                    memset( g_stFunMsgBuf[2].pu8FunBuffer, 0, g_stFunMsgBuf[2].u16Len );

                                    fun_scan_vehmatch_fun( &g_stCarType[3], &stFunStatus, TRUE );
                                    //��31��ȡ����
                                    u16DataStreamCurpid31 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[29] << 8 ) |
                                                            ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[30];

                                    if( u16DataStreamCurpid31 >= 0xf000 )
                                    {
                                        //���ﻹ��ִ��һ�����������ָ��
                                        stFunStatus = TIMEOUT;
                                        fun_scan_car_fun( &g_stCarType[2], &stFunStatus );

                                        //������������ɹ�,��ô���ٴζ�ȡ������
                                        if( stFunStatus == OK )
                                        {
                                            //���µ���������ָ��,ͬʱ��ȡ������
                                            stFunStatus = TIMEOUT;
                                            //�ȳ�ʼ��������������
                                            memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                            fun_scan_vehmatch_fun( &g_stCarType[1], &stFunStatus, FALSE );

                                            //����ͨѶ�ɹ�
                                            if( stFunStatus == OK )
                                            {

                                                //��31��ȡ����
                                                u16DataStreamCurpid31 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[29] << 8 ) |
                                                                        ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[30];

                                                if( ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[25] << 8 ) |
                                                        ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[26] ) == 0 )
                                                {
                                                    //ֱ���˳�ȥ
                                                    bFunExecutionFinish = FALSE;
                                                    break;
                                                }

                                                //�ж�31Ϊ0
                                                g_stObdPar.u16OdoPAR0 = ( u16DataStreamCurpid31 == 0x00 ) ? 0 : g_stObdPar.u16OdoPAR0;
                                            }
                                        }
                                    }

                                    //ֱ������ѭ��
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //�ֶ�����ȡ������
                            case 6:
                                {
                                    //���ݱ���,��Ϊ������̻�Ӱ�쳵��ϵͳ����
                                    TYPEDEF_OBD_PAR g_stObdParTmp = g_stObdPar;
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    //�������³�ʼ��һ��
                                    g_stCarScanSuccess = TIMEOUT;
                                    //��ʼ��������
                                    memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                    //��ȡ������
                                    fun_scan_vehmatch_fun( &g_stCarType[4], &stFunStatus, FALSE );
                                    bFunExecutionFinish = FALSE;
                                    //���ݻ�ԭӰ�쳵��ϵͳ����
                                    g_stObdPar = g_stObdParTmp;
                                    break;
                                }

                            //Ϩ���߼��ж�
                            case 8:
                                {
                                    //������Ҫ�Ż�
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    //ע������ĸı�
                                    memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                                    g_stCarScanSuccess = TIMEOUT;
                                    //ִ��һ��Ϩ���߼�
                                    fun_scan_poweroff_fun( &g_stCarType[1], &stFunStatus );
                                    //ֱ�ӻ�ȡ���
                                    bFunExecutionFinish = FALSE;
                                    break;
                                }

                            //��ȡvin��,�п��ܲ�֧��
                            case 9:
                                {
                                    //���ݱ���,��Ϊ������̻�Ӱ�쳵��ϵͳ����
                                    TYPEDEF_OBD_PAR g_stObdParTmp = g_stObdPar;
                                    //���ܳɹ���񶼵û�ȡVIN��
                                    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
                                    g_stCarScanSuccess = TIMEOUT;
                                    g_stFunMsgBuf[3].u16Len = 17;
                                    //ע������OBD��ȡ���ɹ�����VIN���ȡʧ��
                                    memset( g_stFunMsgBuf[3].pu8FunBuffer, 0, g_stFunMsgBuf[3].u16Len );
                                    //ִ��vin��ȡ����
                                    fun_scan_vehmatch_fun( &g_stCarType[5], &stFunStatus, FALSE );

                                    //����ͨ��״̬,��Ϊ��ͨ����������VIN�벻��ȷ�������
                                    if( stFunStatus != OK )
                                    {
                                        g_stCarScanSuccess = TIMEOUT;
                                        memset( g_stFunMsgBuf[3].pu8FunBuffer, 0xff, g_stFunMsgBuf[3].u16Len );
                                    }

                                    //ֱ�����������߼�
                                    bFunExecutionFinish = FALSE;
                                    //���ݻ�ԭӰ�쳵��ϵͳ����
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
Description:�ú�����Ҫ������ϵͳ������ɨ��(����+OBD)
Input      :none
Output     :none
Return     :none
Others     :������Ҫ��Ԥ���Ժ�Ҫ����µĹ���,�������������һ��
********************************************************************************
*/
void fun_scan_vehmatch_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus, bool bvehmatch )
{
    if( bvehmatch )
    {
        //�ڽ���ɨ��֮ǰ���ж�flash��ϵ�ͳ���洢���ǲ���һ����ϵ
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
Description:�����������+OBD
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_scan_cdtc_fun( TYPEDEF_FRAME_STATUS * pstFunStatus )
{
    TYPEDEF_FRAME_STATUS stFunStatus = TIMEOUT;
    //Ĭ������������ǳɹ���
    g_stPhoneCommandShare.u8ClearDtc = 0x00;
    //����ɨ��������
    fun_scan_vehmatch_fun( &g_stCarType[1], &stFunStatus, FALSE );

    //�ж϶�ȡ�������Ƿ�ɹ�
    if( stFunStatus == OK )
    {
        //�жϳ����Ƿ�����<100
        if( ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[14] ) != 0x00 ) &
                ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[17] << 8 | ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[18] ) < 400 ) )
        {
            //���»�ȡһ�����
            stFunStatus = TIMEOUT;
            //���OBD���������
            g_stFunMsgBuf[2].u16Len = 3;
            memset( g_stFunMsgBuf[2].pu8FunBuffer, 0, g_stFunMsgBuf[2].u16Len );
            fun_scan_vehmatch_fun( &g_stCarType[3], &stFunStatus, TRUE );
            //ֱ�ӵ������������ָ���
            stFunStatus = TIMEOUT;
            fun_scan_car_fun( &g_stCarType[2], &stFunStatus );

            if( stFunStatus == OK )
            {
                uint16 u16Dsid31 = 0;
                stFunStatus = TIMEOUT;
                //�ٴλ�ȡ������
                memset( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf, 0, DEF_RDATASTREAMBUF_SIZE * 4 );
                fun_scan_vehmatch_fun( &g_stCarType[1], &stFunStatus, FALSE );

                if( stFunStatus == OK )
                {

                    u16Dsid31 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[29] << 8 | ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[30] );

                    if( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[25] << 8 | ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[26] ) != 0 )
                    {
                        g_stObdPar.u16OdoPAR0 = ( u16Dsid31 == 0x00 ) ? 0 : g_stObdPar.u16OdoPAR0;
                    }

                    //˵����������ɹ�
                    g_stPhoneCommandShare.u8ClearDtc = 0x05;
                }
            }
            //˵�����벻�ɹ�
            else
            {
                g_stPhoneCommandShare.u8ClearDtc = 0x04;
            }
        }
        //˵�����ٲ�����
        else
        {
            g_stPhoneCommandShare.u8ClearDtc = 0x01;
        }
    }
    //��֧���������
    else
    {
        g_stPhoneCommandShare.u8ClearDtc = 0x02;
    }

    *pstFunStatus = stFunStatus;
}

/*
*******************************************************************************
Name       :fun_scan_car_fun
Description:�ú�����Ҫ��ʵ��ĳһ�������еĸ��Ժ�OBDɨ��(����;�����Թ��Ϻ�OBD����)
Input      :none
Output     :none
Return     :none
Others     :1.����ִ�и��Թ���,����ɹ�ֱ�ӳ�ȥ������ִ��obd����
2.ͨѶʧ�ܽ���ɨ��obd
********************************************************************************
*/
void fun_scan_car_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus )
{
    //������ݴ������Ĺ��ܺ��ж���û�б�Ҫ����obd����ɨ��
    uint8 u8CarChoiceOffset  = 0;

    for( ; u8CarChoiceOffset < 2; u8CarChoiceOffset++ )
    {
        //���Ե�ɨ��ɹ�
        pfun_scan_sys_remem_fun( &pstCarChoice->stPersonAndObdCfg[u8CarChoiceOffset], pstFunStatus );

        if( *pstFunStatus == OK )
        {
            break;
        }

        //����һ��Ҫ��ӹ����빦��֮�����ʱ
        OSTimeDlyHMSM( 0, 0, pstCarChoice->u16CarDelay / 1000, pstCarChoice->u16CarDelay % 1000 );
    }

    return;
}
/*
*******************************************************************************
Name        :fun_scan_sys_fun
Description:�ú�������Ҫ�����Ƕ�ĳһ����ϵͳ����ɨ������书��
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

    //ϵͳɨ��
    for( ; u8SysChoiceOffset < pstPersonObdChoice->u8SysTotal ; )
    {
        //����������³�ʼ��һ��
        stFunStatus = TIMEOUT;
        bIsFindSys = FALSE;

        //���￪ʼ����ͬһ�����Э���ظ�ɨ��
        if( NULL != memchr(
                    &u8SysChoiceSuccess[1],
                    ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice, u8SysChoiceSuccess[0] ) )
        {
            u8SysChoiceOffset++;
            continue;
        }

        //��������ѡ��,Ԥ��ϵͳ��������20
        for( u8CurSysOffset = 0; u8CurSysOffset < ( g_stObdPar.u8CacheSysTotal > 20 ? 0 : g_stObdPar.u8CacheSysTotal ); u8CurSysOffset++ )
        {
            //��������˵������ɨ���ͬһ����ϵ��ϵͳ,�����￴����û�ж�Ӧ��ϵͳ
            if( g_stObdPar.SysPar[u8CurSysOffset].u8SysLibChoice != ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice )
            {
                continue;
            }

            //���ﻹ�ûص��ܱ�����û�����ϵͳ,ͬʱ��������ϵͳ��������һ��
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

                //����͵ó�����ǰ��u8SysLibChoice+u8SysChoice
                bIsFindSys = TRUE;
                break;
            }

            if( bIsFindSys )
            {
                break;
            }
        }

        //�ж��ǲ�������ҵ�
        if( !bIsFindSys )
        {
            //�Ҳ����ظ�ΪĬ��ֵ
            u8SysChoiceOffsetTmp = u8SysChoiceOffset;
        }

        //��ο�ʼִ��
        fun_scan_fun_fun( ( TYPEDEF_SYS_CFG* ) & ( ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp] ), &stFunStatus );

        //�����ж��Ƿ�ɹ�
        if( OK == stFunStatus )
        {
            //��������ɨ��
            *pstFunStatus = stFunStatus;
            //��������
            u8SysChoiceSuccess[0] += 1;
            //�洢��ǰ�ɹ�ϵͳ
            u8SysChoiceSuccess[u8SysChoiceSuccess[0]] = ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysLibChoice;

            //��������ҵ��ɹ���
            if( !bIsFindSys )
            {
                //�������������,��β�Ͳ���
                g_stObdPar.SysPar[g_stObdPar.u8CacheSysTotal % 20].u8SysLibChoice =
                    ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysLibChoice;
                g_stObdPar.SysPar[g_stObdPar.u8CacheSysTotal % 20].u8SysChoice =
                    ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u8SysChoice;
                g_stObdPar.u8CacheSysTotal++;
            }
        }

        else if( bIsFindSys )
        {
            //˵��ʧ����,������ش���
            g_stObdPar.u8CacheSysTotal--;
            memmove(
                &g_stObdPar.SysPar[u8CurSysOffset],
                &g_stObdPar.SysPar[u8CurSysOffset + 1], ( g_stObdPar.u8CacheSysTotal - u8CurSysOffset ) * 2 );
            //���������仰,������ҵ�ʧ�ܵĻ��ͱ��밴��Ĭ�ϵĽ���ɨ��
            continue;
        }

        u8SysChoiceOffset++;
        OSTimeDlyHMSM( 0, 0, ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u16SysDelay / 1000,
                       ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp].u16SysDelay % 1000 );
    }

    //���ﻹ�����¸���һ��flash����Ĵ洢��

    return ( void* )0;
}
/*
*******************************************************************************
Name       :fun_scan_fun_fun
Description:�ú�����Ҫ����Ե�ǰ��������Ҫִ�еĹ���,�������ʱ��֡��֮֡�����ʱ
Input      :none
Output     :none
Return     :none
Others     :����һ��Ҫȷ��һ������g_pModeLibTab��ӦΪ�������ܻ������������������
********************************************************************************
*/
void fun_scan_fun_fun( TYPEDEF_SYS_CFG *pstsysChoice, TYPEDEF_FRAME_STATUS * pstFunStatus )
{
    uint8 u8FunChoiceOffset = 0;
    void *pCommStatus = NULL;
    //�������ú�,ϵͳ�����ȡ��
    g_pModeLibTab = &( g_pstSysLibTab[pstsysChoice->u8SysLibChoice] )[pstsysChoice->u8SysChoice];

    for( ; u8FunChoiceOffset < pstsysChoice->u8FunTotal; u8FunChoiceOffset++ )
    {

        //ִ�е�����һ�������е���һ������
        //���ﴫ��2������
        //����1:��ǰѡ���ϵͳ
        //����2:��ǰ������Ҫ�Ĳ���
        pCommStatus =
            ( * ( ( g_pFunLib[( pstsysChoice->pstFunction )[u8FunChoiceOffset].u8FunLibChoice] ) + ( pstsysChoice->pstFunction )[u8FunChoiceOffset].u8FunChoice ) )
            (
                pstsysChoice,
                ( void* )( pstsysChoice->pstFunction )[u8FunChoiceOffset].pFunctionParameter
            );

        //����1��Ĭ�ϳɹ��Ǽ���������һ֡�����,ʧ��ֱ������ȥ,ɨ����һ��ϵͳ
        //����2�����ʧ�ܿɶ��ǲ��ٷ�����һ�������ˣ����ص�ϵͳɨ��
        //����3: �����ȡ������ɹ���ô��û�б�Ҫ������һ��ϵͳɨ��
        switch( *( TYPEDEF_FRAME_STATUS* ) pCommStatus )
        {

            case FRAMETIMEOUT:
            case TIMEOUT:
                {
                    return;
                }

            default:
                {
                    //���ﴦ�������Ҫ�ɹ����ü�ס״̬
                    switch( ( pstsysChoice->pstFunction )[u8FunChoiceOffset].u8FunLibChoice )
                    {
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 6:
                            {
                                //���ñ�־λ��֪�Ѿ��г��Ͷ�ȡ�������ɹ���,��ס�ñ�������ȫ�ֵ�
                                g_stCarScanSuccess = *( TYPEDEF_FRAME_STATUS* ) pCommStatus;
                                //�������ֻ���ṩ��ϵͳɨ��ʹ��
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
Description:�ú�����Ҫ��Ϊ��Ϩ���߼�����ѭ��ɨ���������
Input      :none
Output     :none
Return     :none
Others     :1.����ִ�и��Թ��ܣ�����ɹ�ֱ�ӳ�ȥ������ִ��obd���ܸ�
2.ͨ��ʧ�ܽ���ɨ��obd
********************************************************************************
*/
void fun_scan_poweroff_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus )
{

    uint8 u8CarChoiceOffset  = 0;
	uint16 u16SaveRpmTemp = 0;
    for( ; u8CarChoiceOffset < 2; u8CarChoiceOffset++ )
    {
        //����ѭ����ʽ
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
			//ֱ�Ӹ�ֵת����Ϣ
			g_stObdData.proParaBuf[0] = u16SaveRpmTemp >> 8;			
			g_stObdData.proParaBuf[1] = u16SaveRpmTemp ;				
//            //�ɹ�֮���жϳ���
//            //�жϳ����Ƿ�Ϊ100
//            if( ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[14] ) != 0x00 ) &
//                    ( ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[17] << 8 |
//                        ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[18] ) <= 100 ) )
//            {
//                //����С��100
//                g_stPhoneCommandShare.u8VehicleState = 0xaa;
//            }
//            else
//            {
//                //����
//                g_stPhoneCommandShare.u8VehicleState = 0xbb;
//            }

            break;
        }
        else
        {
            //ʧ��
            g_stPhoneCommandShare.u8VehicleState = 00;
        }
    }

    return;
}

/*
*******************************************************************************
Name       :fun_scan_poweroff_car_fun
Description:��ȡ���䳵��
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

    //�ж��Ƿ��м���
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

        //������ִ��
        fun_scan_fun_fun( ( TYPEDEF_SYS_CFG* ) & ( ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffsetTmp] ), &stFunStatus );

//        //��������书��ͨ�Ų��ɹ�����ô�´�ֱ����ѯ
//        if( stFunStatus != OK )
//        {
//            g_stObdParVehicleState.u8CacheSysTotal = 0 ;
//        }
    }
    else
    {
        stFunStatus = TIMEOUT;
        //ע������ĸı�
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
Description:�ú�������Ҫ�����ǲ�������Ľ���ɨ��
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

    //ϵͳɨ��
    for( ; u8SysChoiceOffset < pstPersonObdChoice->u8SysTotal ; u8SysChoiceOffset++ )
    {
        //����������³�ʼ��һ��
        stFunStatus = TIMEOUT;

        //���￪ʼ����ͬһ�����Э���ظ�ɨ��
        if( NULL != memchr( &u8SysChoiceSuccess[1],
                            ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice, u8SysChoiceSuccess[0] ) )
        {
            continue;
        }

        //ϵͳ��ϵͳ֮���ɨ���ǲ��ÿ���ͨѶ״̬��,ֻҪ��һ���ɹ���OK
        fun_scan_fun_fun( ( TYPEDEF_SYS_CFG* ) & ( ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset] ), &stFunStatus );

        //�������͵�ϵͳ��������,��������ɨ��Ϊpfun_scan_sys_fun��׼��
        if( OK == stFunStatus )
        {
            //��������
            u8SysChoiceSuccess[0] += 1;
            //�洢��ǰ�ɹ�ϵͳ
            u8SysChoiceSuccess[u8SysChoiceSuccess[0]] = ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice;
            //ֻҪ��һ��ϵͳ��ȡ���������������,���ϴ��ɹ�״̬
            *pstFunStatus = stFunStatus;
            //�������������,��β�Ͳ���
            g_stObdParVehicleState.SysPar[g_stObdParVehicleState.u8CacheSysTotal % 20].u8SysLibChoice =
                ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysLibChoice;
            g_stObdParVehicleState.SysPar[g_stObdParVehicleState.u8CacheSysTotal % 20].u8SysChoice =
                ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u8SysChoice;
            g_stObdParVehicleState.u8CacheSysTotal++;
            //ֻҪ��סһ���Ϳ���
            break;
        }

        OSTimeDlyHMSM( 0, 0, ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u16SysDelay / 1000, ( pstPersonObdChoice->pstSysCfg )[u8SysChoiceOffset].u16SysDelay % 1000 );
    }

    return ( void* )0;
}

