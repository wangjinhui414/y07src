/*
********************************************************************************
File name  : ZXT_sys_lib_init.c
Description: 这里初始化和定义的变量都是供底层调用的全局变量
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define ZXT_OBD_DATASTRUCT_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                         描述:底层需要用上的变量初始化
********************************************************************************
*/
const TYPEDEF_MODE_CELL   *g_pModeLibTab = NULL;                           //模块指针
//uint8 g_u8ProtocolLibOffset = 0;                                           //该变量主要是为中断函数更加灵活设计
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
