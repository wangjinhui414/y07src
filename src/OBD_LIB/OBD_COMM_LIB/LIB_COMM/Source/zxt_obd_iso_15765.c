/*
********************************************************************************
File name  : zxt_iso_15765.c
Description: �����д����������������غ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19 �����   ��������������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define LIB_COMM_FUN
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"
/*
********************************************************************************
Name    :void *pfun_iso15765_send_command_group(void* pArg0, void* pArg1).
Description:�ú�����Ҫ�����ĳһ���ɨ������,�������ɨ������Ҳ�����á�
Input    :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :�ɹ�֮��ȷ�������ֵg_pModeLibTab_lib,�������ĸ�ģ�����ͨ��,
      ͬʱȷ��Э������
      ���������Ҫ��λ���ṩ3����������ģ������ ģ��ֵ ͨѶ����
********************************************************************************
*/

void *pfun_iso15765_send_command_group( void *pCmdId, void *pArg1 )
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
        fun_iso_15765_send_times( ( ( uint8* ) pCmdId )[u8CurSent + 1], &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            //����for ֱ�ӱ���ͨѶ������
            break;
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
Function   :fun_iso_15765_send_times
Description:�ú�����Ҫ���Ʒ�������ͬʱ���·��ʹ���
      ����ɹ�Ȼ�������ú���ȥִ�н��պ���
      �ú���������ʼ���Ǻ�ͨѶ������ͬһ��ƫ����
Input    :uint8 cmdid:������һ������
      void **pCommStatus:ͨѶ״̬
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void fun_iso_15765_send_times( uint8 u8CmdId, void **pCommStatus )
{

    uint8 u8Error;
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
        //�����������2�ֿ���1.ECU���������ж�2.����2�жϣ��������񲻿���ִ��3.��Ϊ��Ҷ��ڵ�����

        *pCommStatus = pfOSboxped( g_pstEcuToPerformer_lib,
                                   pStIso15765ActiveLib->u16TimeOut, &u8Error );
        if(u8FilterGroup < 14)
        //�رս���
					CAN1->FA1R &= ~(( uint32 )0x1<<u8FilterGroup);
				else
					CAN1->FA1R &= ~(( uint32 )0x1<<u8FilterGroup);

        //�����ȴ�can�жϽ��������жϽ��յ��������Ƿ����Э��Ҫ��
        switch( u8Error )
        {

            case  OS_ERR_TIMEOUT:
                {
                    //����֡��ʱ,���������ʱ��Ӧ���ǲ��������·���

                    if( g_stCAN115765ReceiveVar_temp->bFrameError )
                    {
                        *pCommStatus = ( void * )& g_enumFrameFlgArr[1];
                    }

                    else
                        //���ﵱ��û����Ӧ������
                    {
                        *pCommStatus = ( void * )& g_enumFrameFlgArr[0];;
                    }

                    if( ( --u8RetransCounter ) == 0 )
                    {
                        //ֱ���˳�����
                        return;
                    }

                    break;
                }

            //�ɹ������ʼ�,ֻ��2�����:1.ͨѶ����,2.˵����7f ϵ��֡,�ø�����Ӧ�������������·���
            case  OS_NO_ERR:
                {
                    if( ( *( TYPEDEF_FRAME_STATUS* ) *pCommStatus == OK ) )
                    {
                        return;
                    }

                    break;
                }

            default://�����ECU��Ͷ�ݹ�������Ϣ������
                return;
        };
    }
    while( 1 );
}



/*
*******************************************************************************
Name    :fun_iso_15765_send_cmd
Description:�ú�����Ҫ�Ǳ�׼��15765���ͺ���
Input    :uint8 u8CmdId
Output     :none
Return     :none
Others     :���ȸ���cmdidȥװ������
      ������һ֡����
********************************************************************************
*/

void fun_iso_15765_send_cmd( uint8 u8CmdId )
{

    //u32FrameId�����֡ID
    //u8Filterpid�˲�������ID
    //u8FilterGroup�˲�����ѡ��
    uint32 u32FrameId;
    uint8  u8Filterpid;
    uint8  u8FilterGroup;

    //��CAN1�������г�ʼ
    g_stCAN115765ReceiveVar_temp->u8DataCounter = 0;
    //�������ISO15765û������,ͨ��Э��������,ע�����ֵ�Ǹ���ECU�������ݽ��б仯��,���Ե�ע��ʹ�õ�ʱ���
    g_stCAN115765ReceiveVar_temp->u8FrameTotal = 0;
    g_stCAN115765ReceiveVar_temp->bFrameError = FALSE;
    g_stCAN115765ReceiveVar_temp->bFrameLengTh = FALSE;
    g_stCAN115765ReceiveVar_temp->u8FrameCounter = 0;
    //װ������
    memcpy
    (
        g_stTxMessage.Data,
        ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).pu8Command ,
        8
    );
    //��������
    u32FrameId = g_unnu32FrameIdLib_lib[( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8CmdIdOffset].u32Bit;
    //�˲�������ID
    u8Filterpid = ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8FilterIdOffset;
    //�˲�����ѡ��
    u8FilterGroup = ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8FilterGroup;
    //�Ƚ�ֹCAN�����ж�
		if(u8FilterGroup < 14)
			CAN_ITConfig( CAN1, CAN_IT_FMP0, DISABLE );
		else
			CAN_ITConfig( CAN2, CAN_IT_FMP0, DISABLE );
    //���ú�CAN�˲���
    pfCAN1FilterInit( u8FilterGroup, g_unnu32FilterId_lib[u8Filterpid].u32Bit );
    //����can�����ж�
		if(u8FilterGroup < 14)
			CAN_ITConfig( CAN1, CAN_IT_FMP0, ENABLE );
		else
			CAN_ITConfig( CAN2, CAN_IT_FMP0, ENABLE );
    //��������һ���ٵ�����
    g_pstEcuToPerformer_lib->OSEventPtr = ( void * ) 0;

    //���ú�CANͨѶID
    g_stTxMessage.StdId = u32FrameId;
    g_stTxMessage.ExtId = u32FrameId;
    //��ʱд��������֡
    g_stTxMessage.RTR = 0;
    //�ǲ�����չ֡
    g_stTxMessage.IDE = ( u32FrameId > 0X7FF ) ? 4 : 0;
    //����Ҳ�Ǹ�������д�ĳ�����������
    g_stTxMessage.DLC = ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8CmdLen;
		if(u8FilterGroup < 14)
			CAN_Transmit( CAN1, &g_stTxMessage );
		else
			CAN_Transmit( CAN2, &g_stTxMessage );
}


/*
********************************************************************************
Name    :void*pfun_iso_15765_receive( void *pCacheOffset, void *pArg1 )
Description:�ú�����Ҫ�����CAN���պ������д���,��Ҫ���ж���û�б�Ҫ
Input    :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void *pfun_iso_15765_receive( void *pCacheOffset, void *pArg1 )
{
    void *prg0 = NULL;

    switch( *( uint8* ) pCacheOffset )
    {

        case 255://�����255˵�����ô洢
            break;

        default://�����ݷ��õ���Ҫ���õĵط�
            g_stu8CacheData[*( uint8* ) pCacheOffset] = g_stDataFromEcu;
            break;
    }

    return ( prg0 );
}

/*
********************************************************************************
Name    :void *pfun_iso_15765_check(void* pArg0, void* pArg1).
Description:�ú�����15765�жϽ��չ����������Ƿ��������жϺ���
Input    :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :ͨѶ�̳ɹ�֮��ͻ�����bFrameError=falseͬʱͶ������,�������ǲ���Ͷ�������
ע������:������յ����ݲ�����������ʱ����-��Ͷ������
         ֻ��ͨѶ�����Ż�Ͷ�����䡣�κ�ʱ�򶼲�����Ͷ������
         ������յ���������������Ӧϵ��(ע������֡�Ż����������Ӧ)
********************************************************************************
*/

void*pfun_iso_15765_check( void*pArg0, void*pArg1 )
{
  	//�����ж�֡�����͵�֡<7 10֡ 20 30֡
    switch( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0xf0 )
    {
        //��������������͹��������ݽ��й�����

        case 0x10:
            {
                //����֡ �������յ�10 ����ظ���
                uint8 u8publicFrame [] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00};
                //���ú�֡��
                g_stDataFromEcu.u8Len = g_stCAN115765ReceiveVar_temp->RxMessage.Data[1];
                //ͬʱ��֪�Ѿ��ҵ�֡��,��Ҫ��Ϊ21֡��׼��
                g_stCAN115765ReceiveVar_temp->bFrameLengTh = TRUE;
                //�洢10֡,ֻ�Ǵ洢6���ֽ�
                memcpy( g_stDataFromEcu.u8Buf, &g_stCAN115765ReceiveVar_temp->RxMessage.Data[2], 6 );
                //��ס��ǰ�洢�����ݳ���
                g_stCAN115765ReceiveVar_temp->u8DataCounter = 6;
                //���͹���֡,��ʱ�����﷢��, ����Ժ󲻺��ʿ��Կ��Ǳ仯λ��
                memcpy( &g_stTxMessage.Data, u8publicFrame, 8 );
								if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
									CAN_Transmit( CAN1, &g_stTxMessage );
								else
									CAN_Transmit( CAN2, &g_stTxMessage );
                //��Ϊ�п��ܽ��յ�10֮֡�󲻸�������,����ͨѶ�������
                g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;
                //Ϊ����2X֡���ü�����
                g_stCAN115765ReceiveVar_temp->u8FrameCounter = 1;
                return ( ( void * )0 );
            }

        case 0x20:
            {
                uint16 u8DataLen = 0;
                TYPEDEF_FRAME_STATUS *Comm_report = NULL;

                //�ж��Ƿ��ҵ�֡��
                if( !g_stCAN115765ReceiveVar_temp->bFrameLengTh )
                {
                    return ( ( void * )0 );
                }

                //�ж��ǲ�����Ҫ���յ�֡
                if( g_stCAN115765ReceiveVar_temp->u8FrameCounter != ( uint8 )( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0x0f ) )
                {
                    return ( ( void * )0 );
                }

                //Ϊ�´ν���2X֡����׼��
                g_stCAN115765ReceiveVar_temp->u8FrameCounter++;
                //�洢2X֡,ֻ�Ǵ洢7���ֽ�
                memcpy( g_stDataFromEcu.u8Buf + g_stCAN115765ReceiveVar_temp->u8DataCounter,
                        &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1],
                        7
                      );
                //���ú�ƫ�Ƶ�ַΪ��һ������׼��
                g_stCAN115765ReceiveVar_temp->u8DataCounter = g_stCAN115765ReceiveVar_temp->u8DataCounter + 7;
                //�жϽ��յ��������Ƿ���ȷ
                u8DataLen =
                    g_stDataFromEcu.u8Len +
                    ( 7 - ( ( ( g_stDataFromEcu.u8Len - 6 ) % 7 ) ? ( ( g_stDataFromEcu.u8Len - 6 ) % 7 ) : 7 ) );

                //�жϽ��յ��������Ƿ��Ѿ��ﵽ��Ч�ֽ�
                if( u8DataLen != g_stCAN115765ReceiveVar_temp->u8DataCounter )
                {
                    //�����־λ�����жϽ��յ��������ǲ�����ȷ��
                    //��Ϊ�ڽ���20֡�Ĺ�������ʱ���п��ܶϵ�
                    g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;
                    return ( ( void * )0 );
                }

                //����֡ͨѶΪ����
                g_stCAN115765ReceiveVar_temp->bFrameError = FALSE ;

                //����Ϊ֡����
                Comm_report = ( void * )& g_enumFrameFlgArr[2];

                //����Ϊ֡����
                pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                //�رս���
								if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
									CAN1->FA1R &= ~(( uint32 )0x1);
								else
									CAN1->FA1R &= ~(( uint32 )0x1<<14);
                return ( ( void * )0 );
            }

        //���ﱸ��

        case 0x30:
            {

            } break ;

        //����Ĭ�Ͼ��ǵ�֡

        default :
            {
                //���ú�֡��
                TYPEDEF_FRAME_STATUS *Comm_report = NULL;
                //��ȡ������Ч����,����ֻ����Ա�׼����֡<=7
                g_stDataFromEcu.u8Len = ( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0x07 );
                //�洢10֡,ֻ�Ǵ洢��Ч���ȸ��ֽ�
                memcpy( g_stDataFromEcu.u8Buf, &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1], g_stDataFromEcu.u8Len );
                //Ĭ������ΪͨѶ����,��Ϊ�п��ܽ��յ�7F 78֮����Ҳ��������
                g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;

                //�жϽ��յ��������Ƿ���ȷ

                if( !bfun_iso_14230_negative_response() )
                {
                    //����֡ͨѶΪ����
                    g_stCAN115765ReceiveVar_temp->bFrameError = FALSE ;
                    //����Ϊ֡����
                    Comm_report = ( void * )& g_enumFrameFlgArr[2];
                    //�رս���
										if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
											CAN1->FA1R &= ~(( uint32 )0x1);																	else
											CAN1->FA1R &= ~(( uint32 )0x1<<14);
                    //Ͷ�������ִ���߱������0k
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

            break ;

    }

    return ( ( void * )0 );
}

/*
********************************************************************************
Name    :void *pfun_iso_15765_check1(void* pArg0, void* pArg1).
Description:�ú�����15765�жϽ��չ����������Ƿ��������жϺ���
Input    :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :ͨѶ�̳ɹ�֮��ͻ�����bFrameError=falseͬʱͶ������,�������ǲ���Ͷ�������
ע������:pfun_iso_15765_check1����ע���pfun_iso_15765_check����,��Щ������֡��Ӧ��ʱ���
         id��һ���Ƶ�����ʹ��,�ú���Ĭ���ǲ�������
         ����: 7e0 02 10 81
               7e8 10 08 50  ecu��֡
               790 30 00 00  Y05A ���Ͷ�֡
********************************************************************************
*/


void*pfun_iso_15765_check1( void*pArg0, void*pArg1 )
{
  	//�����ж�֡�����͵�֡<7 10֡ 20 30֡
    switch( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0xf0 )
    {
        //��������������͹��������ݽ��й�����

        case 0x10:
            {
                //����֡ �������յ�10 ����ظ���
                uint8 u8publicFrame [] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00};

                //��������
                uint32 u32FrameId = g_unnu32FrameIdLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[4]].u32Bit;
                //���ú�֡��
                g_stDataFromEcu.u8Len = g_stCAN115765ReceiveVar_temp->RxMessage.Data[1];
                //ͬʱ��֪�Ѿ��ҵ�֡��,��Ҫ��Ϊ21֡��׼��
                g_stCAN115765ReceiveVar_temp->bFrameLengTh = TRUE;
                //�洢10֡,ֻ�Ǵ洢6���ֽ�
                memcpy( g_stDataFromEcu.u8Buf, &g_stCAN115765ReceiveVar_temp->RxMessage.Data[2], 6 );
                //��ס��ǰ�洢�����ݳ���
                g_stCAN115765ReceiveVar_temp->u8DataCounter = 6;
                //���͹���֡,��ʱ�����﷢��, ����Ժ󲻺��ʿ��Կ��Ǳ仯λ��
                memcpy( &g_stTxMessage.Data, u8publicFrame, 8 );

                g_stTxMessage.StdId = u32FrameId;
                g_stTxMessage.ExtId = u32FrameId;
								if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
									CAN_Transmit( CAN1, &g_stTxMessage );
								else
									CAN_Transmit( CAN2, &g_stTxMessage );
                //��Ϊ�п��ܽ��յ�10֮֡�󲻸�������,����ͨѶ�������
                g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;
                //Ϊ����2X֡���ü�����
                g_stCAN115765ReceiveVar_temp->u8FrameCounter = 1;
                return ( ( void * )0 );
            }

        case 0x20:
            {
                uint16 u8DataLen = 0;
                TYPEDEF_FRAME_STATUS *Comm_report = NULL;

                //�ж��Ƿ��ҵ�֡��
                if( !g_stCAN115765ReceiveVar_temp->bFrameLengTh )
                {
                    return ( ( void * )0 );
                }

                //�ж��ǲ�����Ҫ���յ�֡
                if( g_stCAN115765ReceiveVar_temp->u8FrameCounter != ( uint8 )( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0x0f ) )
                {
                    return ( ( void * )0 );
                }

                //Ϊ�´ν���2X֡����׼��
                g_stCAN115765ReceiveVar_temp->u8FrameCounter++;
                //�洢2X֡,ֻ�Ǵ洢7���ֽ�
                memcpy( g_stDataFromEcu.u8Buf + g_stCAN115765ReceiveVar_temp->u8DataCounter,
                        &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1],
                        7
                      );
                //���ú�ƫ�Ƶ�ַΪ��һ������׼��
                g_stCAN115765ReceiveVar_temp->u8DataCounter = g_stCAN115765ReceiveVar_temp->u8DataCounter + 7;
                //�жϽ��յ��������Ƿ���ȷ
                u8DataLen =
                    g_stDataFromEcu.u8Len +
                    ( 7 - ( ( ( g_stDataFromEcu.u8Len - 6 ) % 7 ) ? ( ( g_stDataFromEcu.u8Len - 6 ) % 7 ) : 7 ) );

                //�жϽ��յ��������Ƿ��Ѿ��ﵽ��Ч�ֽ�
                if( u8DataLen != g_stCAN115765ReceiveVar_temp->u8DataCounter )
                {
                    //�����־λ�����жϽ��յ��������ǲ�����ȷ��
                    //��Ϊ�ڽ���20֡�Ĺ�������ʱ���п��ܶϵ�
                    g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;
                    return ( ( void * )0 );
                }

                //����֡ͨѶΪ����
                g_stCAN115765ReceiveVar_temp->bFrameError = FALSE ;

                //����Ϊ֡����
                Comm_report = ( void * )& g_enumFrameFlgArr[2];

                //����Ϊ֡����
                pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                //�رս���
								if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
										CAN1->FA1R &= ~(( uint32 )0x1);
								else
									  CAN1->FA1R &= ~(( uint32 )0x1<<14);
                return ( ( void * )0 );
            }

        //���ﱸ��

        case 0x30:
            {

            } break ;

        //����Ĭ�Ͼ��ǵ�֡

        default :
            {
                //���ú�֡��
                TYPEDEF_FRAME_STATUS *Comm_report = NULL;
                //��ȡ������Ч����,����ֻ����Ա�׼����֡<=7
                g_stDataFromEcu.u8Len = ( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0x07 );
                //�洢10֡,ֻ�Ǵ洢��Ч���ȸ��ֽ�
                memcpy( g_stDataFromEcu.u8Buf, &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1], g_stDataFromEcu.u8Len );
                //Ĭ������ΪͨѶ����,��Ϊ�п��ܽ��յ�7F 78֮����Ҳ��������
                g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;

                //�жϽ��յ��������Ƿ���ȷ

                if( !bfun_iso_14230_negative_response() )
                {
                    //����֡ͨѶΪ����
                    g_stCAN115765ReceiveVar_temp->bFrameError = FALSE ;
                    //����Ϊ֡����
                    Comm_report = ( void * )& g_enumFrameFlgArr[2];
                    //�رս���
									  if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
											CAN1->FA1R &= ~(( uint32 )0x1);
										else
											CAN1->FA1R &= ~(( uint32 )0x1<<14);
                    //Ͷ�������ִ���߱������0k
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

            break ;

    }

    return ( ( void * )0 );
}
