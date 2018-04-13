/*******************************************************************************
* 文 件   :Zxt_iso_14230.h
* 作 者   :Mitch mouse
* 说 明   :该头文件主要是ISO14230相关数据类型声明和相关函数声明
* 编写时间:
* 版 本   :1.0
* 修改日期:无
********************************************************************************
*/

#ifndef  __ZXT_OBD_ISO_14230_H__
#define  __ZXT_OBD_ISO_14230_H__

#ifdef    ISO_14230_VAR
#define   ISO_14230_VAR_EXT
#else
#define   ISO_14230_VAR_EXT  extern
#endif

#ifdef    ISO_14230_FUN
#define   ISO_14230_FUN_EXT
#else
#define   ISO_14230_FUN_EXT  extern
#endif
/*
********************************************************************************
                              描述:iso-14230数据类型声明
********************************************************************************
*/

typedef struct TYPEDEF_ISO14230_RECEIVE
{
    uint8  u8DataOffset;                                                   //串口3中用来控制数据放置位置
    uint8  u8ReceiveCounter;                                               //串口3中用来记住接收多少个数据
    uint8  u8SentCounter;                                                  //kwp1281中用来记住发送多少条命令
    uint8  u8ReceivePack;                                                  //由于K线需要接收多帧,特意对这里进行处理  
	  bool   bFrameError;                                                    //用于判断当前接收到的帧是否正确
	  
} TYPEDEF_ISO14230_RECEIVE;

/*
********************************************************************************
                              描述:整个系统接受数据缓冲区数据类型定义
********************************************************************************
*/

/*
********************************************************************************
                              描述:iso-14230变量声明
********************************************************************************
*/
ISO_14230_VAR_EXT   uint8                    g_u8SentDataToEcu[];          //发送数据给ECU
ISO_14230_VAR_EXT   TYPEDEF_PROTOCOL_MSGBUF  g_stDataFromEcu;              //接收数据缓冲区
ISO_14230_VAR_EXT   TYPEDEF_PROTOCOL_MSGBUF  g_stu8CacheData[];            //数据的存储区
ISO_14230_VAR_EXT   TYPEDEF_ISO14230_RECEIVE g_stUsart3Iso14230ReceiveVar; //串口3中断变量
/*
********************************************************************************
                              描述:iso-14230函数声明
********************************************************************************
*/

ISO_14230_FUN_EXT void fun_iso_14230_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime );
ISO_14230_FUN_EXT void*pfun_iso_14230_receive( void *pCacheOffset, void *pArg1 );
ISO_14230_FUN_EXT void fun_iso_14230_cmd_load( uint8 u8Cmdid, uint8 u8FrameLen , uint8 *pCmdBuf );
ISO_14230_FUN_EXT void*pfun_iso_14230_80_check( void*pArg0, void*pArg1 );
ISO_14230_FUN_EXT void*pfun_iso_14230_80_check1( void*pArg0, void*pArg1 );
ISO_14230_FUN_EXT void*pfun_iso_14230_OBD_check( void*pArg0, void*pArg1 );
ISO_14230_FUN_EXT void*pfun_iso14230_send_command_group( void *pCmdId, void *pArg1 );
ISO_14230_FUN_EXT void*pfun_iso14230_send_command_group1( void *pCmdId, void *pArg1 );
ISO_14230_FUN_EXT void fun_iso_14230_send_times( uint8 u8CmdId, uint8 u8FrameLen, void **pCommStatus );
ISO_14230_FUN_EXT void fun_iso_14230_send_times1( uint8 u8CmdId,uint8 u8FrameLen, uint8 u8CacheOffset,uint8 u8ReceivePack, void **pCommStatus );
ISO_14230_FUN_EXT bool bfun_iso_14230_negative_response( void );

#endif









