/*
********************************************************************************
File name  : ZXT_clear_dtc.c
Description: �����д������������빦����غ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ������������뺯��
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define CLEAR_DTC_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Name       :pfun_cdtc_for_all_car
Description:�ú�������ƽ�������ļ����������
Input      :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :
********************************************************************************
*/
#if CDTC_FOR_ALL_CAR_EN==1
void *pfun_cdtc_for_all_car( void *pArg0, void *pArg1 )
{
    void *pCommStatus;
    //������������뺯��
    pCommStatus = ppublicfun_send_command_index( ( uint8 * )g_pModeLibTab->pCmdIndexTabChoice->pCmdClearDtc, ( void* ) 0 );
    return ( pCommStatus );
}

#endif

