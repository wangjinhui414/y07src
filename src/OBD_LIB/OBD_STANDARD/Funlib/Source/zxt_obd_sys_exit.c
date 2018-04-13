/*
********************************************************************************
File name  : Zxt_sys_exit.c
Description:该文件主要是
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-11-20   小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define SYS_EXIT_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Name       :pfun_sys_exit_fun
Description:系统退出函数
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void *pfun_sys_exit_fun( void *pstsysChoice, void *pu8FunctionParameter )
{
    void *pCommStatus;
    //调用退出函数
    pCommStatus = ppublicfun_send_command_index( ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdSysExit, ( void* )0 );
    return ( pCommStatus );

}
