/*
********************************************************************************
File name  : zxt_iso_9141.c
Description: ���ļ���Ҫ�Ǳ�׼��9141Э����غ���,���Բ����������ﶨ�����,�ڸ�
             �ļ������Ķ��Ǻ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    �����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define ISO_9141_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Name        :pfun_iso_9141_send_command_group
Description:�ú�����Ҫ�ǿ��Ʒ���һ��������,���������صı���
Input      :void *pcmdid      xx��������ֵ
Output     :none
Return     :void *pCommStatus ͨѶ�����ַ
Others     :
********************************************************************************
*/
#if IS0_9141_SEND_GROUP_EN == 1
void*pfun_iso_9141_send_command_group( void *pCmdId, void *prg1 )
{
    //u8CurSent��ǰ��������ֵ
    //pCommStatusͨѶ״̬
    //u8CacheOffset����洢��ַ
    uint8 u8CurSent = 0;
    void  *pCommStatus;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    uint8 u8FrameLen = 0;
    uint8 u8CacheOffset = 0;
    //�ҵ�Ҫ��ʼ���ı�����ַ
    pstIsoKlineCommLib = &g_stIsoKlineCommLib[( g_pModeLibTab->u8ProtocolLibOffset )[3]];
    //������Ҫ�ǿ��Ʒ���һ��������

    for( u8CurSent = 0; u8CurSent < ( ( uint8* ) pCmdId )[0]; u8CurSent++ )
    {
        //���ᷢ�͵������
        u8FrameLen = ( ( ( *g_pModeLibTab ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8CmdLen;
        //��������
        fun_iso_9141_send_times( ( ( uint8* ) pCmdId )[u8CurSent + 1], u8FrameLen, &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            break;//����for ֱ�ӱ���ͨѶ������
        }

        //���Ὣ���ݴ洢����һ��������
        u8CacheOffset = ( ( ( ( *g_pModeLibTab ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId ) & 0x0f;

        //��ʼ��������,�����Ѿ�д��û�е��ÿ�
        pfun_iso_14230_receive( &u8CacheOffset, ( void* ) 0 );

        //ͨѶ֡ʱ��
        OSTimeDlyHMSM( 0, 0, pstIsoKlineCommLib ->u16FrameTime / 1000, pstIsoKlineCommLib ->u16FrameTime % 1000 );
    }

    return pCommStatus;
}

#endif
/*
*******************************************************************************
Name        :fun_iso_9141_send_times
Description:�ú�����Ҫ���Ʒ�������ͬʱ���·��ʹ���
            ����ɹ�Ȼ�������ú���ȥִ�н��պ���
            �ú���������ʼ���Ǻ�ͨѶ������ͬһ��ƫ����
Input      :uint8 cmdid:���� uint8 u8FrameLen:�����
            void **pCommStatus:ͨѶ״̬
Output     :none
Return     :none
Others     :
********************************************************************************
*/
#if IS0_9141_SEND_EN == 1
void fun_iso_9141_send_times( uint8 cmdid, uint8 u8FrameLen, void **pCommStatus )
{

    //u8RetransCounter�ط�����
    uint8 u8RetransCounter;
    uint8 u8CheckSum = 0;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;

    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib[( g_pModeLibTab->u8ProtocolLibOffset )[3]];
    u8RetransCounter = ( *pstIsoKlineCommLib ).u8RetransTime;

    do
    {
        fun_iso_14230_send_cmd( cmdid, u8FrameLen, ( *pstIsoKlineCommLib ).u8ByteTime );
        //ecu��ʱʱ��
        OSTimeDlyHMSM( 0, 0, pstIsoKlineCommLib->u16TimeOut / 1000, pstIsoKlineCommLib->u16TimeOut % 1000 );
        //������Ԥ��ECUû����Ӧ

        if( g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter == 0 )
        {
            *pCommStatus = ( void * )&g_enumFrameFlgArr[0];
            //���·���

            if( ( --u8RetransCounter ) != 0 )
                continue;

            else
                return;//����ط������Ѿ�����ô����������
        }

        //���յ������ݲ���ȷ
        if( g_stUsart3Iso14230ReceiveVar.bFrameError )
        {
            //����֡��ʱ
            *pCommStatus = ( void * )&g_enumFrameFlgArr[1];

            if( ( --u8RetransCounter ) != 0 )
                continue;//���·���

            else
                return;//����ط������Ѿ�����ô����������
        }

        //ͨ������Ч��ͺͽ��յ���Ч��ͽ��бȽ��ж����ݵ���ȷ
        u8CheckSum = g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1];

        publicfun_block_character_sc( g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1, g_stDataFromEcu.u8Buf );

        if( u8CheckSum != g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1] )
        {
            *pCommStatus = ( void * )&g_enumFrameFlgArr[1]; //����֡��ʱ

            if( ( --u8RetransCounter ) != 0 )
                //���·���
                continue;

            else
                return;//����ط������Ѿ�����ô����������
        }

        *pCommStatus = ( void * )&g_enumFrameFlgArr[2]; //������������

        //����֡��ֻ��ȡ��Ч���ݳ���
        g_stDataFromEcu.u16Len = g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 4;
        //����ʹ�øñ���,���½������ƶ�һ��

        for( u8CheckSum = 0; u8CheckSum < g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 4; u8CheckSum++ )
        {
            g_stDataFromEcu.u8Buf[u8CheckSum] = g_stDataFromEcu.u8Buf[u8CheckSum + 3];
        }

        //��������
        return;
    }
    while( 1 );
}

#endif
/*
*******************************************************************************
Name        :pfun_iso_9141_68_check
Description:�ú�����Ҫ���жϳ������,��Ҫ�Ǻ˶Դ�ECU���չ����������Ƿ�
            ��ȷ�����ȷͶ�ݳ�ȥ,���ݴ���÷�����u8receivedatafromecu.u8databuf
Input      :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :g_stUsart3Iso14230ReceiveVar.bFrameError�жϽ��յ���֡�����Ƿ���ȷ
            ע������:����û��֡��������ֻ���ж�ǰ��2���ֽ�
                     ���չ��������ݰ���u8datafromecu.u8Buf[68,6A,ID,DATE0]
                    1.���Ƚ���һ��������g_stUsart3Iso14230ReceiveVar.u8DataCounter
                    2.Ȼ���жϽ��չ����������Ƿ���ȷ�������ȷ
                      g_stUsart3Iso1420ReceiveVar.bFrameError����Ϊ��
                    3.Ȼ��һֱ��������
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
                //�����жϵ��ǲ���������ӵ�����
                if( 0x48 != g_stDataFromEcu.u8Buf[0] )
                {
                    //����Ϊ֡����,����ѡ���ϴ�ͨѶ״̬
                    g_stUsart3Iso14230ReceiveVar.bFrameError = TRUE ;
                }
            }

            return ( prg0 );

        case 1:
            {
                //�����жϵ��ǲ���������ӵ�����
                if( 0x6B != g_stDataFromEcu.u8Buf[1] )
                {
                    //����Ϊ֡����,����ѡ���ϴ�ͨѶ״̬
                    g_stUsart3Iso14230ReceiveVar.bFrameError = TRUE ;
                }
            }

        default:
            return ( prg0 );
    }
}

#endif



