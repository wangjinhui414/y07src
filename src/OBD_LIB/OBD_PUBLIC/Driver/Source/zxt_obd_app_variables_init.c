/*
********************************************************************************
File name  : ZXT_sys_lib_init.c
Description: �����ʼ���Ͷ���ı������ǹ��ײ���õ�ȫ�ֱ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define ZXT_OBD_DATASTRUCT_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                         ����:�ײ���Ҫ���ϵı�����ʼ��
********************************************************************************
*/
const TYPEDEF_MODE_CELL   *g_pModeLibTab = NULL;                           //ģ��ָ��
//uint8 g_u8ProtocolLibOffset = 0;                                           //�ñ�����Ҫ��Ϊ�жϺ�������������
uint8 g_u8RdtcBuf0[DEF_RDTCBUF_SIZE] = {0};
TYPEDEF_DATASTREAM_MSGBUF g_u8RdataStreamBuf = {0, 0, 0, 0};
uint8 g_u8OdoMeterBuf[DEF_ODOMETERBUF_SIZE] = {0};
uint8 g_u8RvinBuf[DEF_RVIN_SIZE] = {0};
TYPEDEF_FUNCTION_MSGBUF g_stFunMsgBuf[DEF_MSGBUF_SIZE] =
{
    {0, g_u8RdtcBuf0},
    {0, ( uint8* )&g_u8RdataStreamBuf},
    {0, g_u8OdoMeterBuf},
    {0, g_u8RvinBuf},
};
static TYPEDEF_OBD_PAR g_stObdPar;
static TYPEDEF_OBD_PAR g_stObdParVehicleState;
