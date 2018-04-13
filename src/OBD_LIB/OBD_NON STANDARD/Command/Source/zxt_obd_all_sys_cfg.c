/*
********************************************************************************
File name  :ZXT_sys_cfg.c
Description:该文件主要是整个系统资源配置表,这里只是对整个系统资源的配置,不允许在
            这里初始化与资源无关的变量,这里是供开发人员可配置的
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   整个系统资源框架
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define OBD_NOR_STD_SYS_CFG_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                         描述:发动机配置表,其实这里放置的就是每一个系统必须
                              的变量放置在这里
成员0：命令表
成员1：命令索引值表
成员2. 协议类型配置表
       u8Protocol_Lib_Offset[0]发送函数，
       u8Protocol_Lib_Offset[1]接收函数,暂时没用上
       u8Protocol_Lib_Offset[2]协议确认函数
       u8ProtocolLibOffset[3]通讯参数配置函数,只要你的系统确定通讯参数肯定也是确定的
       u8Protocol_Lib_Offset[4]暂时没有用上
********************************************************************************
*/

const TYPEDEF_MODE_CELL g_stEngLibTab[] =
{
    //OBD高低电平激活K
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode0Cmdtab0,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode0Index,
        {0, 0, 0, 0, 0},

    },
    //OBD地址码激活K
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode0Cmdtab0,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode8Index,
        {0, 0, 0, 0, 0},

    },
    //OBD地址码激活 68 6a
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode1Cmdtab0,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode1Index,
        {1, 0, 1, 0, 0},

    },
    //OBD CAN标准帧(7e8)
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab0,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode2Index,
        {2, 0, 5, 0, 3},

    },
    //OBD CAN标准帧(7e9) //
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab1,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode3Index,
        {2, 0, 5, 0, 4},

    },
    //OBD CAN标准帧(7ef)  //VIN 7e7 伊兰特
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab2,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode4Index,
        {2, 0, 5, 0, 5},

    },
    //OBD CAN扩张帧(0x18daf110) //0x18DA10F1
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab3,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode5Index,
        {2, 0, 5, 0, 8},
    },
    //OBD CAN扩张帧(0x18daf111)  //0x18DA11F1
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab4,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode6Index,
        {2, 0, 5, 0, 7},
    },
    //OBD CAN扩张帧(0x18daf10e) //本田 0x18da0ef1
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab5,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode7Index,
        {2, 0, 5, 0, 6},
    },
};

/*
********************************************************************************
                         描述:变速箱配置表,其实这里放置的就是每一个系统必须
                              的变量放置在这里
成员0：命令表
成员1：命令索引值表
成员2. 协议类型配置表u8Protocol_Lib_Offset[0]这是发送函数，u8Protocol_Lib_Offset[1]接收函数,
       协议确认函数u8Protocol_Lib_Offset[2],u8ProtocolLibOffset[3]该变量用于判断是否需要发送空闲
********************************************************************************
*/

const TYPEDEF_MODE_CELL g_stAtLibTab[] =
{
    0
};
/*
********************************************************************************
                         描述:仪表配置表,其实这里放置的就是每一个系统必须
                              的变量放置在这里
成员0：命令表
成员1：命令索引值表
成员2. 协议类型配置表u8Protocol_Lib_Offset[0]这是发送函数，u8Protocol_Lib_Offset[1]接收函数,
       协议确认函数u8Protocol_Lib_Offset[2],u8ProtocolLibOffset[3]该变量用于判断是否需要发送空闲
********************************************************************************
*/
const TYPEDEF_MODE_CELL g_stInstrumentPanelLibTab[] =
{
    0
};

/*
********************************************************************************
                         描述:整个系统总配置表
01.发动机
02.自动变速
03.ABS制动
08.空调/加热
09.中央电器
15.安全气囊
16.方向盘
17.仪表板
19.CAN网络
25.防启动锁
36.驾驶员侧座椅
42.驾驶员侧车门
44.助理转向
46.中央舒适系统
52.乘客侧车门
53.驻车制动
55.氙灯射程
56.收音机
62.左后车门
72.右后车门
********************************************************************************
*/
const TYPEDEF_MODE_CELL *g_pstSysLibTab[] =
{
    g_stEngLibTab,   //00发动机
    g_stAtLibTab,    //01
    NULL,            //02
    NULL,            //03
    NULL,            //04
    NULL,            //05
    NULL,            //06
    NULL,            //07
    NULL,            //08
    NULL,            //09
    NULL,            //10
    NULL,            //11
    NULL,            //12
    NULL,            //13
    NULL,            //14
    NULL,            //15
    g_stInstrumentPanelLibTab,    //16仪表系统
    NULL,            //18
    NULL,            //19
    NULL,            //20
    NULL,            //21
    NULL,            //22
    NULL,            //23
};



