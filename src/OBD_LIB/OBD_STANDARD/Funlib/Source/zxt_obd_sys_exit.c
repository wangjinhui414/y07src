/*
********************************************************************************
File name  : Zxt_sys_exit.c
Description:���ļ���Ҫ��
History    :
�޸�����   : �޸��� : �޸�����:������
2012-11-20   С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define SYS_EXIT_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Name       :pfun_sys_exit_fun
Description:ϵͳ�˳�����
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void *pfun_sys_exit_fun( void *pstsysChoice, void *pu8FunctionParameter )
{
    void *pCommStatus;
    //�����˳�����
    pCommStatus = ppublicfun_send_command_index( ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdSysExit, ( void* )0 );
    return ( pCommStatus );

}
