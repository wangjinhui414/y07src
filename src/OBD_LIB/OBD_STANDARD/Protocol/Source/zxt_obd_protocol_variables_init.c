/*
********************************************************************************
File name  : Zxt_protocol_variables_init.c
Description: ��ͷ�ļ���Ҫ�����protocol_c�ļ����������й���ģ�����Ҫ�õ��ı������г�ʼ���Ͷ���
             ���Բ��������޹صı���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ����Э�������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/

#define   ISO_14230_VAR //14230Э�������ʼ��
#define   ISO_15765_VAR //15765Э�������ʼ��
#define   ISO_1281_VAR  //1281Э�������ʼ��
#define   GM_PROTOCOL_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
********************************************************************************
                         ����:IS0_14230.C��Ҫ���ϵı�����ʼ��
********************************************************************************
*/



/*
********************************************************************************
                              ����:����ϵͳ��ECUͨѶ������
********************************************************************************
*/
uint8   g_u8SentDataToEcu[255] = {0}; //�������ݸ�ECU������
TYPEDEF_PROTOCOL_MSGBUF g_stDataFromEcu = {0, 0};  //�������ݻ�����
TYPEDEF_PROTOCOL_MSGBUF g_stu8CacheData[3] = {0}; //���������ǽ�ECU��ʱ����ô�������ݴ洢
TYPEDEF_ISO14230_RECEIVE g_stUsart3Iso14230ReceiveVar = {0, 0, 0, 0, FALSE}; //����3-14230����������Ҫ���ϵı���
TYPEDEF_CANMSG g_stCAN115765ReceiveVar = {0, 0, {0}, FALSE, FALSE, 0};
TYPEDEF_CANMSG g_stCAN115765ReceiveVar1 = {0, 0, {0}, FALSE, FALSE, 0};
const TYPEDEF_FRAME_STATUS g_enumFrameFlgArr[] = {TIMEOUT, FRAMETIMEOUT, OK};

