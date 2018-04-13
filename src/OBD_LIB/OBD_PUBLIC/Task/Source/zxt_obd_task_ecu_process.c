/*
********************************************************************************
File name  : Zxt_task_for_obd_device_and_ecu_communications.c
Description:
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define ZXT_OBD_TASK_ECU_PROCESS_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
*******************************************************************************
Name        :OBD_DEVICE_AND_ECU_COMMUNICATIONS
Description:该函数是整个系统的接口板给ECU发送数据的任务
Input      :void *pArg没有用上
Output     :none
Return     :none
Others     :该任务只是执行简单的命令发送,接收数据
            1.这里主要工作就是给ECU发送数据
            2.首先呢创建一个邮箱,该邮箱是在等上位机发送过来的命令
            3.然后进行解析,然后启动ECU给接口板回数据任务。
            4.然后进入一个while循环进行空闲命令的发送,这里是不会再返回,然后通过查询邮箱是否
            有新的命令发送过来。
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



