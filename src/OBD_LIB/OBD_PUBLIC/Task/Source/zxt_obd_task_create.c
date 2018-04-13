/*
********************************************************************************
File name  : Zxt_main.c
Description:
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-11-20   小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define  ZXT_OBD_TASK_CREAT_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                            描述:OBD任务栈大小放置
********************************************************************************
*/
OS_STK              STK_OBD_TASK_FUN_PROCESS[DEF_OBD_TASK_FUN_SIZE];
OS_STK              STK_OBD_DEVICE_AND_ECU[DEF_OBD_TASK_ECU_SIZE];
OS_STK              STK_OBD_TASK_SCHEDULE[DEF_OBD_TASK_SCHEDULE_SIZE];
OS_STK              STK_OBD_TASK_TRANSFERLAYER[DEF_OBD_TASK_TRANSFERLAYPER_SIZE];

/*
********************************************************************************
                             描述:OBD任务邮箱或者信号量初始化
********************************************************************************
*/
OS_EVENT            *g_pstScheduleToFunAnalyzer    = NULL;
OS_EVENT            *g_pstPerformerToEcu    = NULL;
OS_EVENT            *g_pstEcuToPerformer         = NULL;
OS_EVENT            *g_pstFunAnalyzerToSchedule = NULL;
OS_EVENT            *g_pstTransferLayerToSchedule = NULL;
OS_EVENT            *g_pstAnalyzerToPerformer = NULL;
OS_EVENT            *g_pstPerformerToAnalyzer = NULL;
/*
*******************************************************************************
Name       :fun_obd_task_create
Description:该函数是OBD任务创建函数
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void fun_obd_task_create( void )
{

    fun_obd_bsp_init();

    g_pstScheduleToFunAnalyzer = OSSemCreate( 0 );
    g_pstFunAnalyzerToSchedule = OSSemCreate( 0 );
    g_pstAnalyzerToPerformer = OSMboxCreate( 0 );
    g_pstPerformerToAnalyzer = OSMboxCreate( 0 );
    g_pstPerformerToEcu  = OSSemCreate( 0 );
    g_pstEcuToPerformer = OSMboxCreate( 0 );
    g_pstTransferLayerToSchedule = OSSemCreate( 0 );

    OSTaskCreateExt
    ( fun_obd_task_ecu_process,
      ( void * ) 0,
      ( OS_STK * ) &STK_OBD_DEVICE_AND_ECU[DEF_OBD_TASK_ECU_SIZE - 1],
      DEF_OBD_TASK_ECU_PRIO,
      DEF_OBD_TASK_ECU_PRIO,
      ( OS_STK * ) &STK_OBD_DEVICE_AND_ECU[0],
      DEF_OBD_TASK_ECU_SIZE,
      ( void * ) 0,
      OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
    );

    OSTaskCreateExt
    ( fun_obd_task_fun_process,
      ( void * ) 0,
      ( OS_STK * ) &STK_OBD_TASK_FUN_PROCESS[DEF_OBD_TASK_FUN_SIZE - 1],
      DEF_OBD_TASK_FUN_PRIO,
      DEF_OBD_TASK_FUN_PRIO,
      ( OS_STK * ) &STK_OBD_TASK_FUN_PROCESS[0],
      DEF_OBD_TASK_FUN_SIZE,
      ( void * ) 0,
      OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
    );
    OSTaskCreateExt
    ( fun_obd_task_schedule,
      ( void * ) 0,
      ( OS_STK * ) &STK_OBD_TASK_SCHEDULE[DEF_OBD_TASK_SCHEDULE_SIZE - 1],
      DEF_OBD_TASK_SCHEDULE_PRIO,
      DEF_OBD_TASK_SCHEDULE_PRIO,
      ( OS_STK * ) &STK_OBD_TASK_SCHEDULE[0],
      DEF_OBD_TASK_SCHEDULE_SIZE,
      ( void * ) 0,
      OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
    );

    OSTaskCreateExt
    ( fun_obd_task_transferlayer_process,
      ( void * ) 0,
      ( OS_STK * ) &STK_OBD_TASK_TRANSFERLAYER[DEF_OBD_TASK_TRANSFERLAYPER_SIZE - 1],
      DEF_OBD_TASK_TRANSFERLAYPER_PRIO,
      DEF_OBD_TASK_TRANSFERLAYPER_PRIO,
      ( OS_STK * ) &STK_OBD_TASK_TRANSFERLAYER[0],
      DEF_OBD_TASK_TRANSFERLAYPER_SIZE,
      ( void * ) 0,
      OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR
    );
			
		pfOSdelay = OSTimeDlyHMSM;
		pfOSboxpst = OSMboxPost;
		pfOSboxped = OSMboxPend;
		pfCAN1FilterInit = fun_can1_Filter_init;
		g_pstEcuToPerformer_lib = g_pstEcuToPerformer;
}
