/*
********************************************************************************
File name  : ZXT_protocol_cfg.c
Description: 该文件主要是对整个系统框架的协议层进行管理
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   对协议层进行配置
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/

#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                         描述:整个系统框架协议栈的配置表
                              这里有2个配置表 1:发送函数配置表
                                              2:接收函数配置表
********************************************************************************
*/

//发送函数
const TYPEDEF_pPROTOCOLLIB g_stProtocolSendLib[] =
{
#if IS0_14230_SEND_GROUP_EN==1
    pfun_iso14230_send_command_group1,
#endif
#if IS0_9141_SEND_GROUP_EN == 1
    pfun_iso_9141_send_command_group,
#endif
#if ISO_15765_SEND_GROUP_EN==1
    pfun_iso15765_send_command_group,
#endif
#if  GM_SEND_GROUP_EN ==1
    pfun_gm_send_command_group,
#endif
#if IS0_1281_SEND_GROUP_EN == 1
    pfun_iso1281_send_command_group,
#endif
};
//接收函数
const TYPEDEF_pPROTOCOLLIB g_stProtocolReceiveLib[] =
{
    pfun_iso_14230_receive,
};

//协议校对函数
const TYPEDEF_pPROTOCOLLIB g_stProtocolCheckLlib[] =
{
#if ISO_14230_80_CHECK_EN==1
    pfun_iso_14230_80_check,
#endif
#if ISO_9141_68_CHECK_EN==1
    pfun_iso_9141_68_check,
#endif
#if ISO_15765_CHECK_EN==1
    pfun_iso_15765_check,
#endif
#if GM_CHECK_EN==1
    pfun_gm_check,
#endif
#if ISO_1281_CHECK_EN == 1
    pfun_iso_1281_check,
#endif
#if ISO_15765_CHECK_EN1==1
    pfun_iso_15765_check1,
#endif
};
/*
********************************************************************************
                              描述:整个系统帧ID库
********************************************************************************
*/
const TYPEDEF_2WORD4BYTE g_unnu32FrameIdLib[] =
{
    0x33f1,     //0标准K线
    0x07df,     //1标准CAN线
    0x18db33f1, //2扩张帧
    0X7E0,      //3 OBD
    0X7E1,      //4 OBD
    0x7e7,      //5 OBD   伊兰特
    0x18DAF10E, //6 OBD   本田扩展帧
    0x18DA11F1, //7 obd   本田扩展帧
    0x18DA10F1, //8 obd   沃尔沃
};

/*
********************************************************************************
                              描述:整个系统滤波ID设置
********************************************************************************
*/
const TYPEDEF_2WORD4BYTE g_unnu32FilterId [] =
{
    0x7e8,     //00 (obd)7E8
    0x7e9,     //01 (obd)7e9
    0x7ef,     //02 (obd)7EF(现代)
    0x18daf110,//03 OBD扩展帧
    0x18daf111,//04 OBD扩展帧
    0x18daf10e,//05 OBD扩展帧
    0xf110,    //06 宝来车发动机(K)
};




