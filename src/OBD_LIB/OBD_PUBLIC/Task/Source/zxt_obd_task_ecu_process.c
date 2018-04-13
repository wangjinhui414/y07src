/*
********************************************************************************
File name  : Zxt_task_for_obd_device_and_ecu_communications.c
Description:
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define ZXT_OBD_TASK_ECU_PROCESS_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
*******************************************************************************
Name        :OBD_DEVICE_AND_ECU_COMMUNICATIONS
Description:�ú���������ϵͳ�Ľӿڰ��ECU�������ݵ�����
Input      :void *pArgû������
Output     :none
Return     :none
Others     :������ֻ��ִ�м򵥵������,��������
            1.������Ҫ�������Ǹ�ECU��������
            2.�����ش���һ������,���������ڵ���λ�����͹���������
            3.Ȼ����н���,Ȼ������ECU���ӿڰ����������
            4.Ȼ�����һ��whileѭ�����п�������ķ���,�����ǲ����ٷ���,Ȼ��ͨ����ѯ�����Ƿ�
            ���µ�����͹�����
********************************************************************************
*/
void  fun_obd_task_ecu_process( void *pArg )
{
    uint8 u8Error = 0;
    uint8  *pCmdId = NULL;
    const TYPEDEF_pPROTOCOLLIB *pprotocol = g_stProtocolSendLib;
    void *pCommStatus = NULL;
    ( void ) pArg;

    while( 1 )
    {

        pCmdId = ( uint8* ) OSMboxPend( g_pstAnalyzerToPerformer, 0, &u8Error );

        switch( u8Error )
        {

            case  OS_NO_ERR:
                {
										g_pModeLibTab_lib = g_pModeLibTab;
										g_unnu32FrameIdLib_lib = g_unnu32FrameIdLib;
										g_unnu32FilterId_lib = g_unnu32FilterId;
										g_stIsoKlineCommLib_lib = g_stIsoKlineCommLib;
										g_stIso15765ActiveLib_lib = g_stIso15765ActiveLib;
                    pCommStatus =
                        ( * ( pprotocol + ( g_pModeLibTab->u8ProtocolLibOffset )[0] ) )( pCmdId, ( void* ) 0 );
                }
                break;
        }

        OSMboxPost( g_pstPerformerToAnalyzer, pCommStatus );
    }
}



