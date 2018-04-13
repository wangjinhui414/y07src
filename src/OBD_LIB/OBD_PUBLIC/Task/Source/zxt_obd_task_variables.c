/*******************************************************************************
* 文 件   :Zxt_task_variables.c
* 作 者   :Mitch mouse
* 说 明   :该文件主要是将所有任务间需要用上的变量进行初始化,绝对不允许在这里初始
*          化与任务无关的变量
* 编写时间:2013-5-17
* 版 本   :1.0
* 修改日期:无
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
            描述:Zxt_task_for_mobile_and_obd_device_communications
********************************************************************************
*/
static TYPEDEF_FRAME_STATUS g_stCarScanSuccess ;
static OBDMbox_TypeDef g_stObdData;
static OBDMbox_TypeDef g_stObdData0;
static OBDMbox_TypeDef *g_pstObdCmd;
