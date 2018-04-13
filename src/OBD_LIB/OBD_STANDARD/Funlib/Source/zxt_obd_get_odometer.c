/*
********************************************************************************
File name  : Zxt_get_odometer.c
Description: ���ļ���Ҫ�ǰ������л�ȡ��̹��ܺ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-11-20   С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define GET_ODOMETER_FUN //��������
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Name       :pfun_get_odometer_withcalculation
Description:�ú�����Ҫ��ͨ��������������ȡϵͳ���
Input      :none
Output     :none
Return     :ͨѶ״̬
Others     :��Ҫ�벻��Ҫ���㶼����ʹ��
�ú���ע��ʹ������pu8FunctionParameter�����ǹ�ʽ
ABC*pu8FunctionParameter[0]/pu8FunctionParameter[1]+pu8FunctionParameter[2]
-pu8FunctionParameter[3]
ע������ʹ�õ��ǵ��ֽڵļ���
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

    //���Ͷ�ȡ�������
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //���ͨѶ��ʱֱ����������
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //���ﻹ���ж���Ч�����Ƿ�ȡ�������
    if( g_stu8CacheData[( ( uint8* )pu8FunctionParameter )[4]].u16Len <
            ( ( uint8* )pu8FunctionParameter )[5] + ( ( uint8* )pu8FunctionParameter )[6] )
    {
        //������ʱ����
        return ( void * )& g_enumFrameFlgArr[0];
    }

    //ע������Ŀ�ʼλ��
    pu8odometer = &g_stu8CacheData[( ( uint8* )pu8FunctionParameter )[4]].
                  u8Buf[( ( uint8* )pu8FunctionParameter )[5]];

    //�ж��Ƿ�Ҫ�ߵ�λ����
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

    //Ȼ��ʼȡ����,��ס���ﴦ��֮����Զ���Ǹ�λ��ǰ

    for( RevCounter = ( ( uint8* )pu8FunctionParameter )[6], RevTemp0 = 0; RevCounter > 0; RevCounter--, RevTemp0++ )
    {
        u32odometer |= ( uint32 )pu8odometer[RevTemp0] << ( RevCounter - 1 ) * 8;
    }

    //Ȼ����м���
    u32odometer *= ( ( uint8* )pu8FunctionParameter )[0];
    u32odometer /= ( ( uint8* )pu8FunctionParameter )[1];
    u32odometer += ( ( uint8* )pu8FunctionParameter )[2];
    u32odometer -= ( ( uint8* )pu8FunctionParameter )[3];
    //�������֮�󽫽���洢�ڹ̶�λ��
    g_stFunMsgBuf[2].pu8FunBuffer[0] = u32odometer >> 16;
    g_stFunMsgBuf[2].pu8FunBuffer[1] = u32odometer >> 8;
    g_stFunMsgBuf[2].pu8FunBuffer[2] = u32odometer;
    return ( pCommStatus );
}
#endif
/*
*******************************************************************************
Name       :pfun_get_odometer_from_bus
Description:�ú�����Ҫ��ֱ�Ӵ�������ֱ�ӻ�ȡ����
Input      :none
Output     :none
Return     :none
Others     :1.ע����������⴦��
2.ע����������pu8FunctionParameter
********************************************************************************
*/
void *pfun_get_odometer_from_bus( void *pArg0, void *pu8FunctionParameter )
{
    uint8 u8Error = 0;
    uint8 u8FrameIdChoice = ( ( uint8* )pu8FunctionParameter )[0];
    uint8 u8StartByte = g_stOdoMeterLib[u8FrameIdChoice].u8StartByte;
    //�������ǰ��check����
    uint8 u8ProtocolLibOffsetTemp = g_u8ProtocolLibOffset;
    //���ú�������Ҫʹ�õ�check����
    g_u8ProtocolLibOffset = ( ( uint8* )pu8FunctionParameter )[1];
    //��һ�������˲���
    CAN_ITConfig( CAN_COMMUNICATION_PORT, CAN_IT_FMP0, DISABLE );
    fun_can1_baud_init( DEF_CAN500K );
    //����������һ�¼�����
    g_stCAN115765ReceiveVar.u16DataCounter = 0;
    //����Ĭ����0��
    fun_can1_Filter_init( 0, g_stOdoMeterLib[u8FrameIdChoice].u32BusFrameId );
    CAN_ITConfig( CAN_COMMUNICATION_PORT, CAN_IT_FMP0, ENABLE );
    OSMboxPend( g_pstEcuToPerformer, g_stOdoMeterLib[u8FrameIdChoice].u16FrameIdTime, &u8Error );

    //�����ʱ˵��û�����ݹ���,ֱ���˳�ȥ
    if( u8Error != OS_NO_ERR )
    {
        return( ( void * )& g_enumFrameFlgArr[0] );
    }

    //�ָ���ǰ��check����
    g_u8ProtocolLibOffset = u8ProtocolLibOffsetTemp ;

    //����˵���������ݳɹ�
    //������������Ļ�û�н��е���
    if( g_stOdoMeterLib[u8FrameIdChoice].bSwapByte )
    {
       uint8 u8temp = g_stDataFromEcu.u8Buf[u8StartByte];
       g_stDataFromEcu.u8Buf[u8StartByte] = g_stDataFromEcu.u8Buf[u8StartByte + 2];
       g_stDataFromEcu.u8Buf[u8StartByte + 2] = u8temp ;
    }

    //�������֮�󽫽���洢�ڹ̶�λ��
    g_stFunMsgBuf[2].pu8FunBuffer[0] = g_stDataFromEcu.u8Buf[u8StartByte];
    g_stFunMsgBuf[2].pu8FunBuffer[1] = g_stDataFromEcu.u8Buf[u8StartByte + 1];
    g_stFunMsgBuf[2].pu8FunBuffer[2] = g_stDataFromEcu.u8Buf[u8StartByte + 2];
    //˵��ͨѶ�ɹ�
    return ( ( void * )&g_enumFrameFlgArr[2] );
}

/*
*******************************************************************************
Name       :pfun_get_odometer_from_obd
Description:�ú�����Ҫ�Ǵ�obd�ϻ�ȡ���
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void *pfun_get_odometer_from_obd( void *pArg0, void *pu8FunctionParameter )
{
    //�����ж�֧��֧��31
    //����ָ��ָ�����2��������
    uint16 u16DataStreampid = ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Datalen - 6; //??????

    //�������������˵�����ж�31�Ƿ�֧��
    if( *( uint16* ) & ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid] == 0 )
    {
        //���سɹ���ͬʱ��21��Ŀȫ������Ϊ0
        memset( &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[19], 0, 6 );
        return ( ( void * )&g_enumFrameFlgArr[2] );
    }

    //��21��Ӧ����ֵ�е����һλ��Ϊ01��ֻҪ֧�־͸�Ϊ1
    *( uint16* )&( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid - 1] = 1 ;

    //�ǵ�һ�ν�������,��2�ֿ���1:û�����ó�ʼ���,����һ�ֿ��������ó�ʼ���
    if( g_stObdPar.u32OdoMeter != 0x00FFFFFF )
    {

        uint16 u16DataStreamCurpid31 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid + 4] << 8 |
                                         ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid + 5] );

        //���У׼��
        if( g_u32CalibrationOdo )
        {
            //У�Թ������ôֱ��ִ������
            g_stObdPar.u32OdoMeter = g_u32CalibrationOdo;
            //ʹ����ϱ������
            g_u32CalibrationOdo = 0;
        }
        else if( u16DataStreamCurpid31 > g_stObdPar.u16OdoPAR0 )
        {
            u16DataStreamCurpid31  = u16DataStreamCurpid31 - g_stObdPar.u16OdoPAR0;
            g_stObdPar.u32OdoMeter = u16DataStreamCurpid31 + g_stObdPar.u32OdoMeter;
        }

        //�������֮�󽫽���洢�ڹ̶�λ��
        g_stFunMsgBuf[2].pu8FunBuffer[0] = g_stObdPar.u32OdoMeter >> 16;
        g_stFunMsgBuf[2].pu8FunBuffer[1] = g_stObdPar.u32OdoMeter >> 8;
        g_stFunMsgBuf[2].pu8FunBuffer[2] = g_stObdPar.u32OdoMeter;
    }

    else
    {
        //������˵����û�����ó�ʼ���
        //��ճ�ʼ���,Ԥ�����ó�ʼ���Ϊ00���
        g_stObdPar.u32OdoMeter = 0x00FFFFFF;
        //�������֮�󽫽���洢�ڹ̶�λ��
        g_stFunMsgBuf[2].pu8FunBuffer[0] = g_stObdPar.u32OdoMeter >> 16;
        g_stFunMsgBuf[2].pu8FunBuffer[1] = g_stObdPar.u32OdoMeter >> 8;
        g_stFunMsgBuf[2].pu8FunBuffer[2] = g_stObdPar.u32OdoMeter;
    }

    g_stObdPar.u16OdoPAR0 = ( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid + 4] << 8 |
                              ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreampid + 5] );
    return ( ( void * )&g_enumFrameFlgArr[2] );

}
