/*
********************************************************************************
File name  : ZXT_iso_1281.C
Description: ���ļ���Ҫ�Ǳ�׼��1281Э����غ���,���Բ����������ﶨ�����,�ڸ�
              �ļ������Ķ��Ǻ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ������Ҫ��1281���е�Э�麯��
********************************************************************************
*/

#define LIB_COMM_FUN
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"

/*
*******************************************************************************
Function   :pfun_iso1281_send_command_group
Description:�ú�����Ҫ�ǿ��Ʒ���һ��������,���������صı���������
                    creatlinkOffset�о�����
Input      :void *pCmdId�Ƿ�����������ֵ,void *pArg1����
Output     :none
Return     :ͨѶ�ɹ����
Others     :�ɹ�֮��ȷ�������ֵg_pModeLibTab_lib,�������ĸ�ģ�����ͨ��,
            ͬʱȷ��Э������
            ���������Ҫ��λ���ṩ3����������ģ������ ģ��ֵ ͨѶ����
********************************************************************************
*/

void*pfun_iso1281_send_command_group( void *pCmdId, void *pArg1 )
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
    pstIsoKlineCommLib = &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];

    //������Ҫ�ǿ��Ʒ���һ��������

    for( u8CurSent = 0; u8CurSent < ( ( uint8* ) pCmdId )[0]; u8CurSent++ )
    {
        //���ᷢ�͵������
        u8FrameLen = ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8CmdLen;
        //��������
        fun_iso_1281_send_receive( ( ( uint8* ) pCmdId )[u8CurSent + 1], u8FrameLen, &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            break;//����for ֱ�ӱ���ͨѶ������
        }

        //���Ὣ���ݴ洢����һ��������
        u8CacheOffset = ( ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId ) & 0x0f;

        //��ʼ��������,�����Ѿ�д��û�е��ÿ�
        pfun_iso_14230_receive( &u8CacheOffset, ( void* ) 0 );

        //ͨѶ֡ʱ��
        pfOSdelay( 0, 0, pstIsoKlineCommLib ->u16FrameTime / 1000, pstIsoKlineCommLib ->u16FrameTime % 1000 );
    }

    return pCommStatus;
}



/*
*******************************************************************************
Function   :fun_iso_1281_send_receive
Description:�ú�����Ҫ���Ʒ�������ͬʱ���·��ʹ���
            ����ɹ�Ȼ�������ú���ȥִ�н��պ���
            �ú���������ʼ���Ǻ�ͨѶ������ͬһ��ƫ����
Input      :uint8 cmdid:������һ������ uint8 u8FrameLen:�����
            void **pCommStatus:ͨѶ״̬
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void fun_iso_1281_send_receive( uint8 u8CmdId, uint8 u8FrameLen, void **pCommStatus )
{

    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];

    //���ú�check����
    g_u8ProtocolLibOffset = ( g_pModeLibTab_lib->u8ProtocolLibOffset )[2];

    //���ͳ����һ���ֽ�����
    if( OK != *( TYPEDEF_FRAME_STATUS* )pfun_iso_1281_send_cmd( u8CmdId, u8FrameLen, pstIsoKlineCommLib->u8ByteTime ) )
    {
        //�����������ʧ����ô�͵�����ʱ������
        *pCommStatus = ( void * )&g_enumFrameFlgArr[0];
        return;
    }

    //�������һ���ֽ�
    USART3->CR1 &= ~ 0X4;
    USART_SendData( USART3, 0x03 );

    while( USART_GetFlagStatus( USART3, USART_FLAG_TC ) == RESET );

    //���������ٵ�����
    g_pstEcuToPerformer_lib->OSEventPtr = ( void * ) 0;
    //�Դ���3�������г�ʼ,��ס���ﲻ�ܳ�ʼ�����ͼ�����
    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
    g_stUsart3Iso14230ReceiveVar.bFrameError = FALSE;
    //��������
    USART3->CR1 |= 0X4;
    *pCommStatus = pfun_iso_1281_receive( ( void* ) 0, ( void* ) 0 );
}



/*
*******************************************************************************
Name        :fun_iso_1281_send_cmd
Description:�ú�����Ҫ�Ǳ�׼��1281���ͺ���
Input      :
Output     :none
Return     :ͨѶ״̬
Others     :1.���ȸ���cmdidȥװ������
            2.������һ֡����
            3.�жϷ��͵������Ƿ���ȷ
            ע�����һ���ֽ�03�������﷢��
********************************************************************************
*/

void *pfun_iso_1281_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime )
{

    //u16LenCounter���ȼ�����
    uint8 u8Error;
    uint8 u8RetransCounter;
    uint8 u8ReceiveDataTemp = 0;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;

    uint16 u16LenCounter = 0;
    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    u8RetransCounter = pstIsoKlineCommLib->u8RetransTime;
    //�Դ���3�������г�ʼ,��ס���ﲻ�ܳ�ʼ�����ͼ�����
    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
    g_stUsart3Iso14230ReceiveVar.bFrameError = FALSE;

    //�رս���
    USART3->CR1 &= ~ 0X4;
    //ע�������˳����ñ�ߵ�
    fun_iso_1281_cmd_load( u8CmdId, u8FrameLen, g_u8SentDataToEcu );

    do
    {
        USART_SendData( USART3, g_u8SentDataToEcu[u16LenCounter] );

        while( USART_GetFlagStatus( USART3, USART_FLAG_TC ) == RESET );

        //���������ٵ�����
        g_pstEcuToPerformer_lib->OSEventPtr = ( void * ) 0;
        //��������
        USART3->CR1 |= 0X4;
        //�ȴ���ʱ
        pfOSboxped( g_pstEcuToPerformer_lib, pstIsoKlineCommLib ->u16TimeOut, &u8Error );

        //�رս���
        USART3->CR1 &= ~ 0X4;

        //�жϽ��յ����ֽ��Ƿ�ʱ
        switch( u8Error )
        {
            //��ʱ
            case  OS_ERR_TIMEOUT:
                {
                    //�����ط�����
                    if( ( --u8RetransCounter ) == 0 )
                    {
                        //ͨѶ��ʱ
                        return( ( void * )&g_enumFrameFlgArr[0] ); //ֱ���˳�����
                    }

                    break;
                }

            //�������յ�����:1:ȷʵ����ȷ������ 2:������ȷ������
            case  OS_NO_ERR:
                {
                    //�жϽ��յ��������Ƿ�����
                    u8ReceiveDataTemp = ~g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1];

                    if( g_u8SentDataToEcu[u16LenCounter] == u8ReceiveDataTemp )
                    {
                        //ȡ��һ����
                        u16LenCounter++;

                        if( u16LenCounter == u8FrameLen )
                        {
                            //ͨѶ����
                            return( ( void * )&g_enumFrameFlgArr[2] );
                        }
                    }

                    //���������
                    else
                    {
                        //�����ط�����
                        if( ( --u8RetransCounter ) == 0 )
                        {
                            //ͨѶ��ʱ
                            return( ( void * )&g_enumFrameFlgArr[0] ); //ֱ���˳�����
                        }
                    }

                    break;
                }

            default:
                {
                    return( ( void * )&g_enumFrameFlgArr[0] ); //ֱ���˳�����
                }
        }

        //�ֽ���ʱ
        pfOSdelay( 0, 0, 0, u8ByteTime );
    }
    while( 1 );
}



/*
*******************************************************************************
Name        :fun_iso_1281_cmd_load
Description:�ú�����Ҫ�ǽ�����װ�ع���,ͬʱ�Լ���������+1
Input      :uint8 u8Cmdid��������ֵ,uint8 u8FrameLen����� ,uint8 *pCmdBuf���ͻ���
Output     :none
Return     :none
Others     :
********************************************************************************
*/


void fun_iso_1281_cmd_load( uint8 u8Cmdid, uint8 u8FrameLen , uint8 *pCmdBuf )
{
    //��������
    memcpy( pCmdBuf, ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[u8Cmdid] ).pu8Command, u8FrameLen );
    //�����������м�2
    pCmdBuf[1] = g_stUsart3Iso14230ReceiveVar.u8SentCounter + 2;
    g_stUsart3Iso14230ReceiveVar.u8SentCounter = pCmdBuf[1];
}

/*
*******************************************************************************
Name        :pfun_iso_1281_80_check
Description:�ú�����Ҫ�Ǳ��жϳ������,ֻ�Ǽ򵥵Ľ����ݴ���÷�����u8receivedatafromecu.u8databuf
Input      :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :1.���Ƚ���һ��������inter_var0.u8ReceiveCounter
            2.ÿ����һ�θú������Զ�+1
ע������:ֻҪ�����ݾ�Ͷ���ʼ�
********************************************************************************
*/


void*pfun_iso_1281_check( void*pArg0, void*pArg1 )
{
    void *prg0 = NULL;
    g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter++];
    pfOSboxpst( g_pstEcuToPerformer_lib, ( void * )& g_enumFrameFlgArr[1] );
    return ( prg0 );

}


/*
*******************************************************************************
Name        :pfun_iso_1281_receive
Description:�ú�����Ҫ�Ǳ�׼��1281���պ���
Input      :pCmdId,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :����ֻ�ǽ���һ֡����
********************************************************************************
*/

void*pfun_iso_1281_receive( void *pArg0, void *pArg1 )
{
    uint8 u8Error;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;

    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    //���ú�check����
    g_u8ProtocolLibOffset = ( g_pModeLibTab_lib->u8ProtocolLibOffset )[2];
    //���������ж�
    USART_ITConfig( USART3, USART_IT_RXNE, ENABLE );

    do
    {
        //��������
        USART3->CR1 |= 0X4;
        //�ȴ���ʱ
        pfOSboxped( g_pstEcuToPerformer_lib, pstIsoKlineCommLib ->u16TimeOut, &u8Error );

        //�رս���
        USART3->CR1 &= ~ 0X4;

        //������ճ�ʱ,ֱ���˳�ȥ
        if( u8Error != OS_NO_ERR )
        {
            return( ( void * )&g_enumFrameFlgArr[0] );
        }

        //�жϽ��յ��������Ƿ�����
        if( g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1 == g_stDataFromEcu.u8Buf[0] )
        {
            //���ú��������ݳ���
            g_stDataFromEcu.u8Len = g_stDataFromEcu.u8Buf[0] - 1;
            //���µ�������λ��
            memmove( &g_stDataFromEcu.u8Buf[0], &g_stDataFromEcu.u8Buf[2], g_stDataFromEcu.u8Len );
            return( ( void * )&g_enumFrameFlgArr[2] );
        }

        //�ֽ���ʱ,���ʱ����ʱ��ô��,�Ժ��������ٽ��
        pfOSdelay( 0, 0, 0, pstIsoKlineCommLib->u8ByteTime / 2 );
        //���û�н��������ô�Ϳ�ʼȡ����Ӧ����
        USART_SendData( USART3, ~g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter - 1] );

        while( USART_GetFlagStatus( USART3, USART_FLAG_TC ) == RESET );
    }
    while( 1 );
}

