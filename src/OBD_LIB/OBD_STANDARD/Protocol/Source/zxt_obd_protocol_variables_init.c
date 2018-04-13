/*
********************************************************************************
File name  : Zxt_protocol_variables_init.c
Description: 该头文件主要是针对protocol_c文件夹下面所有功能模块的需要用到的变量进行初始化和定义
             绝对不允许定义无关的变量
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立协议变量库
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/

#define   ISO_14230_VAR //14230协议变量初始化
#define   ISO_15765_VAR //15765协议变量初始化
#define   ISO_1281_VAR  //1281协议变量初始化
#define   GM_PROTOCOL_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
********************************************************************************
                         描述:IS0_14230.C需要用上的变量初始化
********************************************************************************
*/



/*
********************************************************************************
                              描述:整个系统与ECU通讯缓冲区
********************************************************************************
*/
uint8   g_u8SentDataToEcu[255] = {0}; //发送数据给ECU缓冲区
TYPEDEF_PROTOCOL_MSGBUF g_stDataFromEcu = {0, 0};  //接收数据缓冲区
TYPEDEF_PROTOCOL_MSGBUF g_stu8CacheData[3] = {0}; //缓冲区就是将ECU暂时就这么多中数据存储
TYPEDEF_ISO14230_RECEIVE g_stUsart3Iso14230ReceiveVar = {0, 0, 0, 0, FALSE}; //串口3-14230接收数据需要用上的变量
TYPEDEF_CANMSG g_stCAN115765ReceiveVar = {0, 0, {0}, FALSE, FALSE, 0};
TYPEDEF_CANMSG g_stCAN115765ReceiveVar1 = {0, 0, {0}, FALSE, FALSE, 0};
const TYPEDEF_FRAME_STATUS g_enumFrameFlgArr[] = {TIMEOUT, FRAMETIMEOUT, OK};

