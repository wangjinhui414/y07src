/*******************************************************************************
* �� ��   :Zxt_task_variables.c
* �� ��   :Mitch mouse
* ˵ ��   :���ļ���Ҫ�ǽ������������Ҫ���ϵı������г�ʼ��,���Բ������������ʼ
*          ���������޹صı���
* ��дʱ��:2013-5-17
* �� ��   :1.0
* �޸�����:��
********************************************************************************
*/
#define  ZXT_OBD_TASK_SCHEDULE_VAR
#define  ZXT_OBD_TASK_ECU_PROCESS_VAR
#define  ZXT_OBD_TASK_FUN_PROCESS_VAR
#define  ZXT_OBD_TRANSFERLAYER_PROCESS_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
TYPEDEF_FUN_FLAG g_stPhoneCommandShare = {TRUE , 00, 01, 00, 0, 0, 0, 'O'};
/*
********************************************************************************
            ����:Zxt_task_for_mobile_and_obd_device_communications
********************************************************************************
*/
static TYPEDEF_FRAME_STATUS g_stCarScanSuccess ;
static OBDMbox_TypeDef g_stObdData;
static OBDMbox_TypeDef g_stObdData0;
static OBDMbox_TypeDef *g_pstObdCmd;
