/*
********************************************************************************
File name  : ZXT_Protocol_cfg.h
Description: 该文件主要是对协议栈的配置该头文件只允许放置宏定义
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   这里主要是对整个协议进行配置
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#ifndef  __ZXT_OBD_PROTOCOL_CFG_H__
#define  __ZXT_OBD_PROTOCOL_CFG_H__
#ifdef    PROTOCOL_LIB         
#define   PROTOCOL_LIB_EXT
#else
#define   PROTOCOL_LIB_EXT  extern
#endif
#endif
/*
********************************************************************************
                              描述:IS0-14230协议配置表
********************************************************************************
*/
#define  IS0_14230_SEND_GROUP_EN          1                                //IS014230整组命令发送使能
#define  IS0_14230_SEND_EN                1                                //IS014230发送使能
#define  ISO_14230_80_CHECK_EN            1                                //IS014230协议栈使能
#define  IS0_14230_RECEIVE_EN             1                                //IS014230接收使能
#define  ISO_14230_CMD_LOAD_EN            1                                //IS014230命令装载
#define  ISO14230_NEGATIVE_RESPONSE_EN    1                                //IS014230消极响应处理

/*
********************************************************************************
                              描述:ISO_15765协议配置表
********************************************************************************
*/
#define  ISO_15765_SEND_GROUP_EN       1                                   //iso15765协议通讯配置表
#define  IS0_15765_SEND_TIMES_EN       1                                   //发送使用
#define  ISO_15765_CMD_LOAD_EN         1                                   //命令装载使能
#define  ISO_15765_CHECK_EN            1                                   //IS015765协议栈使能
#define  ISO_15765_CHECK_EN1           1                                   //IS015765协议栈使能
#define  IS0_15765_RECEIVE_EN          1                                   //IS014230接收使能


/*
********************************************************************************
                              描述:ISO_9141协议配置表
********************************************************************************
*/
#define  IS0_9141_SEND_GROUP_EN        1                                   //ISO9141通讯命令使能
#define  IS0_9141_SEND_EN              1                                   //ISO9141通讯命令使能
#define  ISO_9141_68_CHECK_EN          1                                   //ISO9141校验使能

/*
********************************************************************************
                              描述:通用车协议配置表
********************************************************************************
*/
#define  GM_CHECK_EN                   1                                   //通用车校验使能
#define  GM_SEND_TIMES_EN              1                                   //发送使能
#define  GM_SEND_GROUP_EN              1                                   //整组命令发送使能

/*
********************************************************************************
                              描述:IS0-1281协议配置表
********************************************************************************
*/
#define  IS0_1281_SEND_GROUP_EN        1                                   //IS01281整组命令发送使能
#define  IS0_1281_SEND_EN              1                                   //IS01281协议栈使能
#define  ISO_1281_CMD_LOAD_EN          1                                   //IS01281命令装载
#define  ISO_1281_CHECK_EN             1                                   //IS01281核查函数
#define  IS0_1281_RECEIVE_EN           1                                   //IS01281接收使能

/*
********************************************************************************
                              描述:数据块大小
********************************************************************************
*/
//底层相关数据块大小
#define DEF_RECEVIE_DATA_MAX           500

//协议层数据类型定义
//typedef struct TYPEDEF_PROTOCOL_MSGBUF
//{
////    uint8  u8Len;/*为了荣威550，80多个故障码，串口无报文修改 2017-2-7 liu */
////  uint8  u8Buf[255];
//	//修改如下
//	uint16  u16Len;
//    uint8  u8Buf[DEF_RECEVIE_DATA_MAX];
//} TYPEDEF_PROTOCOL_MSGBUF;

//typedef  enum {TIMEOUT, FRAMETIMEOUT, OK} TYPEDEF_FRAME_STATUS;
typedef void*( *TYPEDEF_pPROTOCOLLIB )( void*, void* );
typedef void*( *TYPEDEF_pPROTOCOLCHECKLIB )( void*, void* );
/*
********************************************************************************
                              描述:整个系统通讯相关标志位类型声明
;TIMEOUT代表没有响应，frametiemeout代表帧超时与汽车电脑通讯中断,ok代表通讯正常
********************************************************************************
*/
//PROTOCOL_LIB_EXT             const TYPEDEF_FRAME_STATUS g_enumFrameFlgArr[];
PROTOCOL_LIB_EXT             const TYPEDEF_pPROTOCOLLIB g_stProtocolSendLib[];   //发送函数
PROTOCOL_LIB_EXT             const TYPEDEF_pPROTOCOLLIB g_stProtocolReceiveLib[];//接收函数
PROTOCOL_LIB_EXT             const TYPEDEF_pPROTOCOLLIB g_stProtocolCheckLlib[]; //效验协议类型
PROTOCOL_LIB_EXT             const TYPEDEF_2WORD4BYTE g_unnu32FrameIdLib[];
PROTOCOL_LIB_EXT             const TYPEDEF_2WORD4BYTE g_unnu32FilterId [];
