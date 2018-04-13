/*
********************************************************************************
File name  : ZXT_create_link.h
Description: 该头文件主要是Creatlink.c中相关变量函数的声明,不允许在这里放置无关信息
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   创建连接函数和变量声明
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#ifndef  __ZXT_OBD_CREATE_LINK_H__
#define  __ZXT_OBD_CREATE_LINK_H__


#ifdef    CREATE_LINK_VAR
#define   CREATE_LINK_VAR_EXT
#else
#define   CREATE_LINK_VAR_EXT  extern
#endif

#ifdef    CREATE_LINK_FUN
#define   CREATE_LINK_FUN_EXT
#else
#define   CREATE_LINK_FUN_EXT  extern
#endif

/*
********************************************************************************
                              描述:14230/9141数据类型声明
********************************************************************************
*/
typedef struct TYPEDEF_ISO14230_ACTIVE_PAR
{
    uint8   u8LowTime;                                                     //低电平时间
    uint8   u8HighTime;                                                    //高电平时间
    uint8   u8RetransTime;                                                 //重发次数
    uint16  u16BaudRate;                                                   //波特率
} TYPEDEF_ISO14230_ACTIVE_PAR;

typedef struct TYPEDEF_ISO9141_ACTIVE_PAR
{
    uint8   u8AddCode ;                                                    //协议地址
    uint8   u8AddCodeTime;                                                 //地址码发送时间
    uint8   u8RetransTime;                                                 //重发次数
    uint16  u16BaudRate;                                                   //波特率
    uint16  u16ProtocolReversed[10];                                       //保留
} TYPEDEF_ISO9141_ACTIVE_PAR;

//typedef struct TYPEDEF_ISO14230_COMMUNICATION_PAR
//{
//    uint8   u8ByteTime;                                                    //通讯字节时间
//    uint16  u16FrameTime;                                                  //通讯帧时间
//    uint8   u8RetransTime;                                                 //重发次数
//    uint16  u16TimeOut;                                                    //超时时间
//} TYPEDEF_ISO14230_COMMUNICATION_PAR;
/*
********************************************************************************
                              描述:IS0-15765协议数据类型声明
********************************************************************************
*/

//typedef struct TYPEDEF_ISO15765_ACTIVE_PAR
//{
//    uint32 u32BaudRate;                                                    //波特率
//    uint8  u8FrameTime;                                                    //CAN通讯帧时间
//    uint16 u16TimeOut;                                                     //CAN通讯超时时间
//    uint8  u8RetransTime;                                                  //重发次数
//} TYPEDEF_ISO15765_ACTIVE_PAR;


/*
********************************************************************************
                              描述:函数需要使用的变量库声明
********************************************************************************
*/
CREATE_LINK_VAR_EXT const TYPEDEF_ISO14230_ACTIVE_PAR g_stIso14230ActiveLib[];  //高低电平函数库变量库
CREATE_LINK_VAR_EXT const TYPEDEF_ISO15765_ACTIVE_PAR g_stIso15765ActiveLib[];  //ISO15765标准库
CREATE_LINK_VAR_EXT const TYPEDEF_ISO9141_ACTIVE_PAR  g_stIso9141ActiveLib[];   //地址码激活函数库
CREATE_LINK_VAR_EXT const TYPEDEF_ISO14230_COMMUNICATION_PAR  g_stIsoKlineCommLib[]; //K线通讯参数,主要是应对发送和接收部分参数

/*
********************************************************************************
                              描述:创建连接函数声明
********************************************************************************
*/
//协议扫描
//高低电平激活函数25ms
//发送数据给手机
//计算效验和
CREATE_LINK_FUN_EXT void *pfun_active_low_high_xxms( void *pArg0 , void *pu8FunctionParameter );
CREATE_LINK_FUN_EXT void publicfun_upload_data_to_phone( void  *pData );
CREATE_LINK_FUN_EXT uint8 u8publicfun_block_character_xorc( uint16 DataLen, uint8 *pData );
//CREATE_LINK_FUN_EXT void  publicfun_block_character_sc( uint16 DataLen, uint8 *pData );
CREATE_LINK_FUN_EXT void publicfun_upload_data_frame_heard( uint8 u8CmdType, uint16 u16CmdLen, uint8 *pCache );
CREATE_LINK_FUN_EXT void *pfun_active_can_bus( void *pArg0 , void *pu8FunctionParameter );
CREATE_LINK_FUN_EXT void *ppublicfun_send_command_index( uint8 *pCmdIndex, void *pArg1 );
CREATE_LINK_FUN_EXT void *pfun_active_5baud_negate_last_byte_back_addr( void *pArg0 , void *pu8FunctionParameter );
CREATE_LINK_FUN_EXT void *pfun_active_5baud_kwp1281( void *pArg0 , void *pu8FunctionParameter );
CREATE_LINK_FUN_EXT void fun_send_addcode( uint8 u8AddTime, uint8 u8AddCode );
CREATE_LINK_FUN_EXT bool bfun_active_5baud_init( uint8 *pu8LastByte, uint8 u8FunctionParameter );
CREATE_LINK_FUN_EXT bool bfun_cmd_can2_test(void);
#endif


