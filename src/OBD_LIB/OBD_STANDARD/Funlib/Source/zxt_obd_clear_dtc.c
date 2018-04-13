/*
********************************************************************************
File name  : ZXT_clear_dtc.c
Description: 这里编写都是清除故障码功能相关函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立清除故障码函数
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define CLEAR_DTC_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
*******************************************************************************
Name       :pfun_cdtc_for_all_car
Description:该函数就是平常见到的简单清除故障码
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :
********************************************************************************
*/
#if CDTC_FOR_ALL_CAR_EN==1
void *pfun_cdtc_for_all_car( void *pArg0, void *pArg1 )
{
    void *pCommStatus;
    //调用清除故障码函数
    pCommStatus = ppublicfun_send_command_index( ( uint8 * )g_pModeLibTab->pCmdIndexTabChoice->pCmdClearDtc, ( void* ) 0 );
    return ( pCommStatus );
}

#endif

