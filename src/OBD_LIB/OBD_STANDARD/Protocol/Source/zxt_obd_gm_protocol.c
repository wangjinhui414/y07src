/*
********************************************************************************
File name  : Zxt_gm_protocol.c
Description: �����д����������������غ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19 �����   ��������������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define GM_PROTOCOL_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

#if  GM_SEND_GROUP_EN ==1

void *pfun_gm_send_command_group( void *pCmdId, void *pArg1 )
{
    //u8CurSent��ǰ��������ֵ
    //pCommStatusͨѶ״̬
    //u8CacheOffset����洢��ַ
    uint8 u8CurSent = 0;
    void  *pCommStatus;
    const TYPEDEF_ISO15765_ACTIVE_PAR *piso15765_active_Lib;
    uint8 u8CacheOffset = 0;

    piso15765_active_Lib = &g_stIso15765ActiveLib[( g_pModeLibTab->u8ProtocolLibOffset )[3]];

    //������Ҫ�ǿ��Ʒ���һ��������

    for( u8CurSent = 0; u8CurSent < ( ( uint8* ) pCmdId )[0]; u8CurSent++ )
    {

        //��������
        fun_gm_send_times( ( ( uint8* ) pCmdId )[u8CurSent + 1], &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            break;//����for ֱ�ӱ���ͨѶ������
        }

        //���Ὣ���ݴ洢����һ��������
        u8CacheOffset = ( ( ( ( *g_pModeLibTab ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId );

        //��ʼ��������,�����Ѿ�д��û�е��ÿ�,���յķ�����14230�ǲ�һ����
        pfun_iso_15765_receive( &u8CacheOffset, ( void* ) 0 );

        //ͨѶ֡ʱ��
        OSTimeDlyHMSM( 0, 0, 0, piso15765_active_Lib->u8FrameTime );
    }

    return pCommStatus;

}

#endif
/*
*******************************************************************************
Function   :fun_gm_send_times
Description:�ú�����Ҫ���Ʒ�������ͬʱ���·��ʹ���
����ɹ�Ȼ�������ú���ȥִ�н��պ���
�ú���������ʼ���Ǻ�ͨѶ������ͬһ��ƫ����
Input    :uint8 cmdid:������һ������
void **pCommStatus:ͨѶ״̬
Output     :none
Return     :none
Others     :�м�����û�д���7f ϵ������,�õ����幦���н��д���
********************************************************************************
*/
#if GM_SEND_TIMES_EN == 1

void fun_gm_send_times( uint8 u8CmdId, void **pCommStatus )
{

    uint8 u8RetransCounter;
    const TYPEDEF_ISO15765_ACTIVE_PAR *pStIso15765ActiveLib;
    //ֱ��ָ��ͨѶ������
    pStIso15765ActiveLib =
        &g_stIso15765ActiveLib[( g_pModeLibTab->u8ProtocolLibOffset )[3]];
    u8RetransCounter = pStIso15765ActiveLib ->u8RetransTime;//�ط�����
    //���ú�check����
    g_u8ProtocolLibOffset = ( g_pModeLibTab->u8ProtocolLibOffset )[2];

    do
    {
        //�����,CANͨѶ��Ҫ���ϵı�����ʼ��
        fun_iso_15765_send_cmd( u8CmdId );

        //ecu��ʱʱ��,�������֡ʱ��Ϊ1S,������Ӧ10֡������
        OSTimeDlyHMSM( 0, 0, pStIso15765ActiveLib->u16TimeOut / 1000, pStIso15765ActiveLib->u16TimeOut % 1000 );


        //�رս���
        CAN_COMMUNICATION_PORT->FA1R &= ~( ( uint32 )0x3fff );

        //ΪԤ��ECUû����Ӧ����
        if( g_stCAN115765ReceiveVar.u16DataCounter == 0 )
        {
            *pCommStatus = ( void * )&g_enumFrameFlgArr[0];
            //���·���

            if( ( --u8RetransCounter ) != 0 )
            {
                continue;
            }

            else
            {
                return;//����ط������Ѿ�����ô����������
            }
        }

        //������˵��ȷʵ���յ����ݵ�����������,����������ȷ�������������������ɾ��幦�ܾ���
        *pCommStatus = ( void * )&g_enumFrameFlgArr[2];
        return;
    }
    while( 1 );
}

#endif
/*
********************************************************************************
Name    :void *pfun_gm_check(void* pArg0, void* pArg1).
Description:�ú�����Ҫ��Ӧ��ͨ��CAN��������
Input    :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :�ʺ�ʹ������
1.ECU�ص�����û�й�������:��֡û�г���,û�ж�֡��־,
ÿһ֡������һ���ֽ�
2.ע���������õ��ǳ�ʱ�ķ�ʽ���յ�
********************************************************************************
*/
#if GM_CHECK_EN==1

void*pfun_gm_check( void*pArg0, void*pArg1 )
{
    //װ�غ�����
    memcpy( &g_stDataFromEcu.u8Buf[g_stCAN115765ReceiveVar.u16DataCounter], &g_stCAN115765ReceiveVar.RxMessage.Data[1], ( g_stCAN115765ReceiveVar.RxMessage.DLC - 1 ) );
    //���úý��ռ�����,Ϊ��һ�ν�������׼��
    g_stCAN115765ReceiveVar.u16DataCounter += g_stCAN115765ReceiveVar.RxMessage.DLC - 1 ;
    //���ú�֡��,Ϊ������������׼��
    g_stDataFromEcu.u16Len = g_stCAN115765ReceiveVar.u16DataCounter;
    //֡��������,�ڷ��͵�ʱ���Ѿ���0,���ֵ��һЩ����ȵ���Щ���ܺ���ʹ�ú�����(ͨ�ó��ͺܹ���)
    g_stCAN115765ReceiveVar.u8FrameTotal++;
    return ( ( void * )0 );
}
#endif





