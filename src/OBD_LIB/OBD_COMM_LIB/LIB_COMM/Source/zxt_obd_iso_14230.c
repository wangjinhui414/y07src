/*
********************************************************************************
File name  : ZXT_iso_14230.C
Description: ���ļ���Ҫ�Ǳ�׼��14230Э����غ���,���Բ����������ﶨ�����,�ڸ�
              �ļ������Ķ��Ǻ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ������Ҫ��14230���е�Э�麯��
********************************************************************************
*/

#define LIB_COMM_FUN
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"

/*
*******************************************************************************
Function   :pfun_iso14230_send_command_group
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

void*pfun_iso14230_send_command_group( void *pCmdId, void *pArg1 )
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
        fun_iso_14230_send_times( ( ( uint8* ) pCmdId )[u8CurSent + 1], u8FrameLen, &pCommStatus );

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
Function   :fun_iso_14230_send_times
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
void fun_iso_14230_send_times( uint8 u8CmdId, uint8 u8FrameLen, void **pCommStatus )
{

    //u8RetransCounter�ط�����
    uint8 u8Error;
    uint8 u8RetransCounter;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    u8RetransCounter = pstIsoKlineCommLib->u8RetransTime;

    do
    {
        fun_iso_14230_send_cmd( u8CmdId, u8FrameLen, pstIsoKlineCommLib->u8ByteTime );


        /*�����������2�ֿ���
        1.ECU���������ж�
        2.����2�жϣ��������񲻿���ִ��
        3.��Ϊ��Ҷ��ڵ�����
        */
        *pCommStatus = pfOSboxped( g_pstEcuToPerformer_lib, pstIsoKlineCommLib ->u16TimeOut, &u8Error );

        switch( u8Error )
        {

            case  OS_ERR_TIMEOUT:
                {
                    //���ﵱ��֡��ʱ
                    *pCommStatus = ( void * )& g_enumFrameFlgArr[0];

                    //�����ط�����
                    if( ( --u8RetransCounter ) == 0 )
                    {
                        return;//ֱ���˳�����
                    }

                    break;
                }

            //�ɹ������ʼ�,��2�����:1.ͨѶ���� 2.���յ�7f 78���͵�����
            case  OS_NO_ERR:
                {
                    if( ( *( TYPEDEF_FRAME_STATUS* ) *pCommStatus == OK ) )
                    {
                        return;
                    }

                    break;
                }

            default://�����ECU��Ͷ�ݹ�������Ϣ������
                {
                    return;
                }

        };
    }
    while( 1 );
}


/*
*******************************************************************************
Name        :fun_iso_14230_send_cmd
Description:�ú�����Ҫ�Ǳ�׼��14230���ͺ���
Input      :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :���ȸ���cmdidȥװ������
            ������һ֡����
********************************************************************************
*/
void fun_iso_14230_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime )
{

    //u16LenCounter���ȼ�����
    uint16 u16LenCounter = 0;
    //�Դ���3�������г�ʼ
    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
    g_stUsart3Iso14230ReceiveVar.bFrameError = FALSE;

    fun_iso_14230_cmd_load( u8CmdId, u8FrameLen, g_u8SentDataToEcu );
    //���ú�check����
    g_u8ProtocolLibOffset = ( g_pModeLibTab_lib->u8ProtocolLibOffset )[2];
    USART3->CR1 &= ~ 0X4;

    do
    {
        USART_SendData( USART3, g_u8SentDataToEcu[u16LenCounter++] );

        while( USART_GetFlagStatus( USART3, USART_FLAG_TC ) == RESET );

        if( u16LenCounter == u8FrameLen )
        {
            break;
        }

        //�ֽ���ʱ
        pfOSdelay( 0, 0, 0, u8ByteTime );
    }
    while( 1 );

    //��������һ���ٵ�����
    g_pstEcuToPerformer_lib->OSEventPtr = ( void * ) 0;

    USART3->CR1 |= 0X4;
}


/*
*******************************************************************************
Name        :fun_iso_14230_cmd_load
Description:�ú�����Ҫ�ǽ�����װ�ع���,�л�IDͬʱ����Ч���
Input      :uint8 u8Cmdid��������ֵ,uint8 u8FrameLen����� ,uint8 *pCmdBuf���ͻ���
Output     :none
Return     :none
Others     :
********************************************************************************
*/


void fun_iso_14230_cmd_load( uint8 u8Cmdid, uint8 u8FrameLen , uint8 *pCmdBuf )
{
    TYPEDEF_2WORD4BYTE frameid;
    uint8 u8Kwp2000Type = ( ( ( ( g_pModeLibTab_lib ->pCmdTabChoice )[u8Cmdid] ).pu8Command )[0] & 0xc0 ) >> 6;
    //�ҵ�֡ID
    frameid = g_unnu32FrameIdLib_lib[( ( g_pModeLibTab_lib->pCmdTabChoice )[u8Cmdid] ).u8CmdIdOffset];

    //��������
    memcpy( pCmdBuf, ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[u8Cmdid] ).pu8Command, u8FrameLen );

    //�ж�����һ�ָ�ʽ
    switch( u8Kwp2000Type )
    {

        case 0x02://80���80

        case 0x03://0c
            {
                pCmdBuf[1] = frameid.u8Bit[1];//�ı�ID
                pCmdBuf[2] = frameid.u8Bit[0];
            }

            break;

        default  ://68 6a 02 03��ͷ�����л�
            break;
    };

    //����Ч���
    publicfun_block_character_sc( ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8Cmdid] ).u8CmdLen - 1, pCmdBuf );
}


/*
*******************************************************************************
Name        :pfun_iso_14230_80_check
Description:�ú�����Ҫ���жϳ������,��Ҫ�Ǻ˶Դ�ECU���չ����������Ƿ�
                    ��ȷ�����ȷͶ�ݳ�ȥ,���ݴ���÷�����u8receivedatafromecu.u8databuf
Input      :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :1.���Ƚ���һ��������inter_var0.u8DataCounter
            2.ÿ����һ�θú������Զ�+1
            3.Ȼ���жϽ��յ��������Ƿ���ȷ
            4.�����ȷ�����ָ�������Ч����,���򱨸�֡��ʱ��

ע������:������յ����ݲ�����������ʱ����-��Ͷ������
         //������յ����ݲ���ȷҲ����֡ID����ȷ-��Ͷ������
         ������յ���������������Ӧϵ��,����ȷ�����ݹ�������Ͷ����
         ֻ��ͨѶ�����Ż�Ͷ�����䡣�κ�ʱ�򶼲�����Ͷ������,
********************************************************************************
*/

void*pfun_iso_14230_80_check( void*pArg0, void*pArg1 )
{
    void *prg0 = NULL;
    TYPEDEF_FRAME_STATUS *Comm_report = NULL;

    switch( g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter++ )
    {

        case 0:
            {
                //������������Ч���ݵĳ���
                if( g_stDataFromEcu.u8Buf[0] >= 0x81 )
                {
                    g_stDataFromEcu.u8Len = g_stDataFromEcu.u8Buf[0] & 0x3f;
                }

                else if( g_stDataFromEcu.u8Buf[0] == 0x80 )
                {
                    g_stDataFromEcu.u8Len = 0xff;
                }
                else
                {
                    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
                    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;
                }

                return ( prg0 );
            }

        //������������Ч���ݵĳ���
        case 3:
            {
                if( 0x80 == g_stDataFromEcu.u8Buf[0] )
                {
                    g_stDataFromEcu.u8Len = g_stDataFromEcu.u8Buf[3];
                }

                //���µ�������ָ�������������
                if( g_stDataFromEcu.u8Buf[0] == 0x80 )
                {
                    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
                }

                return ( prg0 );
            }

        //�������81�͵����µ�������ָ��
        case 2:
        case 1:
            {
                //���µ�������ָ�������������
                if( g_stDataFromEcu.u8Buf[0] != 0x80 )
                {
                    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
                }

                return ( prg0 );
            }

        default:
            {
                //�������ȷ�Ľ��յ�����
                if( ( g_stDataFromEcu.u8Len + 1 ) == g_stUsart3Iso14230ReceiveVar.u8DataOffset )
                {
                    //�رս���
                    if( g_stUsart3Iso14230ReceiveVar.u8ReceivePack == 1 )
                    {
                        USART3->CR1 &= ~ 0X4;
                    }

                    //������մ洢λ��,������Ҳ���������
                    g_stUsart3Iso14230ReceiveVar.u8DataOffset = 0;
                    g_stUsart3Iso14230ReceiveVar.u8ReceiveCounter = 0;

                    //������������������Ӧ
                    if( !bfun_iso_14230_negative_response() )
                    {
                        //����Ϊ֡����
                        Comm_report = ( void* )& g_enumFrameFlgArr[2];
                        pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                    }

                    else
                    {
                        //�������Ѿ����ֽ��յ���������7F���͵�����
                        //�����7f 78��ô�͵��Ž�����ȷ������,�������������ô�͵��ų�ʱ
                        //�����7f 11˵���ǲ�֧��,ֱ�ӵ��ų�ʱ
                        Comm_report = ( void * )& g_enumFrameFlgArr[1];

                        //�����7f 21˵����Ҫ���·�������,Ȼ��Ͷ���������·�������
                        if( ( 0x7f == g_stDataFromEcu.u8Buf[0] ) & ( 0x21 == g_stDataFromEcu.u8Buf[2] ) )
                        {
                            pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                        }
                    }
                }
            }

            return ( prg0 );

    }
}


/*
*******************************************************************************
Name        :bfun_iso_14230_negative_response
Description:�ú�����Ҫ�Ǵ���ECU����������Ӧ����
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/

bool bfun_iso_14230_negative_response( void )
{
    if( 0X7F == g_stDataFromEcu.u8Buf[0] )
    {
        switch( g_stDataFromEcu.u8Buf[2] )
        {

            case 0x10://(�����󱻾ܾ�)
            case 0x11://(��֧��)
            case 0x12://(��֧�ֻ��߸�ʽ����ȷ)
            case 0x21://(���·�������)
            case 0x22://(���������������������)
            case 0x78://(�ȴ�)
                return TRUE;

            default  :
                return FALSE;
        }
    }

    else
    {
        return FALSE;
    }
}



/*
*******************************************************************************
Name        :pfun_iso_14230_receive
Description:�ú�����Ҫ�Ǳ�׼��14230���պ���
Input      :pCmdId,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :����ֻ�ǽ���һ֡����
********************************************************************************
*/

void*pfun_iso_14230_receive( void *pCacheOffset, void *pArg1 )
{
    void *prg0 = NULL;

    switch( *( uint8* ) pCacheOffset )
    {

        case 0x0f://�����255˵�����ô洢
            break;

        default://�����ݷ��õ���Ҫ���õĵط�
            g_stu8CacheData[*( uint8* ) pCacheOffset] = g_stDataFromEcu;
            break;
    }

    return ( prg0 );
}

/*
*******************************************************************************
Function   :fun_iso_14230_send_times1
Description:�ú�����Ҫ���Ʒ�������ͬʱ���·��ʹ���
            ����ɹ�Ȼ�������ú���ȥִ�н��պ���
            �ú���������ʼ���Ǻ�ͨѶ������ͬһ��ƫ����
Input      :uint8 cmdid:������һ������ uint8 u8FrameLen:�����
            void **pCommStatus:ͨѶ״̬
Output     :none
Return     :none
Others     :�ú���������ն�֡k�����ݣ������������滻��fun_iso_14230_send_times
            Ϊ��ȫ���������ô��
********************************************************************************
*/

void fun_iso_14230_send_times1( uint8   u8CmdId,      uint8 u8FrameLen,
                                uint8   u8CacheOffset, uint8 u8ReceivePack,
                                void    **pCommStatus )
{

    //u8RetransCounter�ط�����
    uint8 u8Error;
    uint8 u8RetransCounter;
    uint8 u8ReceivePackTmp;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    uint8 *pu8CacheData = g_stu8CacheData[u8CacheOffset].u8Buf;
    uint8 *pu8DataLen = &g_stu8CacheData[u8CacheOffset].u8Len;
    *pu8DataLen = 0 ;
    pstIsoKlineCommLib =
        &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    u8RetransCounter = pstIsoKlineCommLib->u8RetransTime;

    do
    {
        //���³�ʼ�����հ���
        u8ReceivePackTmp = 0;
        g_stUsart3Iso14230ReceiveVar.u8ReceivePack = u8ReceivePack;
        fun_iso_14230_send_cmd( u8CmdId, u8FrameLen, pstIsoKlineCommLib->u8ByteTime );

        /*�����������2�ֿ���
        1.ECU���������ж�
        2.����2�жϣ��������񲻿���ִ��
        3.��Ϊ��Ҷ��ڵ�����
        */
        do
        {
            *pCommStatus = pfOSboxped( g_pstEcuToPerformer_lib, pstIsoKlineCommLib ->u16TimeOut, &u8Error );

            if( OS_ERR_NONE != u8Error )
            {
                //�رս���
                USART3->CR1 &= ~ 0X4;
                //���ﵱ��֡��ʱ
                *pCommStatus = ( void * )& g_enumFrameFlgArr[0];

                //�����ط�����
                if( ( --u8RetransCounter ) == 0 )
                {
                    return;//ֱ���˳�����
                }

                break;
            }
            else if( ( *( TYPEDEF_FRAME_STATUS* ) *pCommStatus != OK ) )
            {
                break;
            }
            else
            {
                //����Ҫ���ؽ���
                //���Ὣ���ݴ洢����һ��������
                if( u8CacheOffset == 0x0f )
                {
                    //�رս���????
                    USART3->CR1 &= ~ 0X4;
                    return;
                }
                else
                {

                    memcpy( pu8CacheData,
                            &g_stDataFromEcu.u8Buf[( u8ReceivePackTmp > 0 ) ? 1 : 0],
                            ( u8ReceivePackTmp > 0 ) ? g_stDataFromEcu.u8Len - 1 : g_stDataFromEcu.u8Len );
                    pu8CacheData += ( u8ReceivePackTmp > 0 ) ? g_stDataFromEcu.u8Len - 1 : g_stDataFromEcu.u8Len;
                    *pu8DataLen += ( u8ReceivePackTmp > 0 ) ? g_stDataFromEcu.u8Len - 1 : g_stDataFromEcu.u8Len;

                    //�жϽ��հ��Ƿ����
                    if( u8ReceivePack == ++u8ReceivePackTmp )
                    {
                        //�رս���
                        USART3->CR1 &= ~ 0X4;
                        return;
                    }

//                    else
//                    {
//                        //��������
//                        USART3->CR1 |= 0X4;
//                    }
                }
            }
        }
        while( 1 );
    }
    while( 1 );
}


void*pfun_iso14230_send_command_group1( void *pCmdId, void *pArg1 )
{
    //u8CurSent��ǰ��������ֵ
    //pCommStatusͨѶ״̬
    //u8CacheOffset����洢��ַ
    uint8 u8CurSent = 0;
    void  *pCommStatus;
    const TYPEDEF_ISO14230_COMMUNICATION_PAR *pstIsoKlineCommLib;
    uint8 u8FrameLen = 0;
    uint8 u8CacheOffset = 0;
    uint8 u8ReceivePack = 0;
    //�ҵ�Ҫ��ʼ���ı�����ַ
    pstIsoKlineCommLib = &g_stIsoKlineCommLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];

    //������Ҫ�ǿ��Ʒ���һ��������

    for( u8CurSent = 0; u8CurSent < ( ( uint8* ) pCmdId )[0]; u8CurSent++ )
    {
        //���ᷢ�͵������
        u8FrameLen = ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8CmdLen;
        //���Ὣ���ݴ洢����һ��������
        u8CacheOffset = ( ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId ) & 0x0f;
        u8ReceivePack = ( ( ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId ) & 0xf0 ) >> 4;

        //��������
        fun_iso_14230_send_times1( ( ( uint8* ) pCmdId )[u8CurSent + 1],
                                   u8FrameLen,
                                   u8CacheOffset,
                                   u8ReceivePack,
                                   &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            break;//����for ֱ�ӱ���ͨѶ������
        }

        //ͨѶ֡ʱ��
        pfOSdelay( 0, 0, pstIsoKlineCommLib ->u16FrameTime / 1000, pstIsoKlineCommLib ->u16FrameTime % 1000 );
    }

    return pCommStatus;
}
/*
*******************************************************************************
Name        :pfun_iso_14230_80_check
����У���
********************************************************************************
*/
void publicfun_block_character_sc( uint16 DataLen, uint8 *pData )
{
    uint8 sum = 0;
    uint16 i = 0;

    for( i = 0; i < DataLen; i++ )
    {
        sum += pData[i];
    };

    pData[i] = sum;       //���һ���ֽڷ��õ���У���
}

