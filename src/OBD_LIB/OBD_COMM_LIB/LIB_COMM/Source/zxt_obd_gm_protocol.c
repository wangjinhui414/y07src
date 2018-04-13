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
#define LIB_COMM_FUN
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"

void *pfun_gm_send_command_group( void *pCmdId, void *pArg1 )
{
    //u8CurSent��ǰ��������ֵ
    //pCommStatusͨѶ״̬
    //u8CacheOffset����洢��ַ
    uint8 u8CurSent = 0;
    void  *pCommStatus;
    const TYPEDEF_ISO15765_ACTIVE_PAR *piso15765_active_Lib;
    uint8 u8CacheOffset = 0;

    piso15765_active_Lib = &g_stIso15765ActiveLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];

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
        u8CacheOffset = ( ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId );

        //��ʼ��������,�����Ѿ�д��û�е��ÿ�,���յķ�����14230�ǲ�һ����
        pfun_iso_15765_receive( &u8CacheOffset, ( void* ) 0 );

        //ͨѶ֡ʱ��
        pfOSdelay( 0, 0, 0, piso15765_active_Lib->u8FrameTime );
    }

    return pCommStatus;

}


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


void fun_gm_send_times( uint8 u8CmdId, void **pCommStatus )
{

    uint8 u8RetransCounter;
    const TYPEDEF_ISO15765_ACTIVE_PAR *pStIso15765ActiveLib;
	   uint8 u8FilterGroup = ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8FilterGroup;
    //ֱ��ָ��ͨѶ������
    pStIso15765ActiveLib =
        &g_stIso15765ActiveLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    u8RetransCounter = pStIso15765ActiveLib ->u8RetransTime;//�ط�����
    //���ú�check����
    g_u8ProtocolLibOffset = ( g_pModeLibTab_lib->u8ProtocolLibOffset )[2];

    do
    {
        //�����,CANͨѶ��Ҫ���ϵı�����ʼ��
        fun_iso_15765_send_cmd( u8CmdId );

        //ecu��ʱʱ��,�������֡ʱ��Ϊ1S,������Ӧ10֡������
        pfOSdelay( 0, 0, pStIso15765ActiveLib->u16TimeOut / 1000, pStIso15765ActiveLib->u16TimeOut % 1000 );


        //�رս���
			  if(u8FilterGroup < 14)
					CAN1->FA1R &= ~(( uint32 )0x1 << u8FilterGroup);
				else
					CAN1->FA1R &= ~(( uint32 )0x1 << u8FilterGroup);

        //ΪԤ��ECUû����Ӧ����
        if( g_stCAN115765ReceiveVar_temp->u8DataCounter == 0 )
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


void*pfun_gm_check( void*pArg0, void*pArg1 )
{
    //װ�غ�����
    memcpy( &g_stDataFromEcu.u8Buf[g_stCAN115765ReceiveVar_temp->u8DataCounter], &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1], ( g_stCAN115765ReceiveVar_temp->RxMessage.DLC - 1 ) );
    //���úý��ռ�����,Ϊ��һ�ν�������׼��
    g_stCAN115765ReceiveVar_temp->u8DataCounter += g_stCAN115765ReceiveVar_temp->RxMessage.DLC - 1 ;
    //���ú�֡��,Ϊ������������׼��
    g_stDataFromEcu.u8Len = g_stCAN115765ReceiveVar_temp->u8DataCounter;
    //֡��������,�ڷ��͵�ʱ���Ѿ���0,���ֵ��һЩ����ȵ���Щ���ܺ���ʹ�ú�����(ͨ�ó��ͺܹ���)
    g_stCAN115765ReceiveVar_temp->u8FrameTotal++;
    return ( ( void * )0 );
}

