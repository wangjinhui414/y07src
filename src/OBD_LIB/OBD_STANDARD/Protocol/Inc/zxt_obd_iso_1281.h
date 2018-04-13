/*******************************************************************************
* 文 件   :Zxt_iso_1281.h
* 作 者   :Mitch mouse
* 说 明   :该头文件主要是ISO1281相关数据类型声明和相关函数声明
* 编写时间:
* 版 本   :1.0
* 修改日期:无
********************************************************************************
*/

#ifndef  __ZXT_OBD_ISO_1281_H__
#define  __ZXT_OBD_ISO_1281_H__

#ifdef    ISO_1281_VAR
#define   ISO_1281_VAR_EXT
#else
#define   ISO_1281_VAR_EXT  extern
#endif

#ifdef    ISO_1281_FUN
#define   ISO_1281_FUN_EXT
#else
#define   ISO_1281_FUN_EXT  extern
#endif
/*
********************************************************************************
                              描述:iso-1281数据类型声明
********************************************************************************
*/



/*
********************************************************************************
                              描述:iso-1281变量声明
********************************************************************************
*/
/*
********************************************************************************
                              描述:iso-1281函数声明
********************************************************************************
*/
ISO_1281_FUN_EXT void fun_iso_1281_cmd_load( uint8 u8Cmdid, uint8 u8FrameLen , uint8 *pCmdBuf );
ISO_1281_FUN_EXT void fun_iso_1281_send_receive( uint8 u8CmdId, uint8 u8FrameLen, void **pCommStatus );
ISO_1281_FUN_EXT void*pfun_iso_1281_check( void*pArg0, void*pArg1 );
ISO_1281_FUN_EXT void *pfun_iso_1281_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime );
ISO_1281_FUN_EXT void*pfun_iso1281_send_command_group( void *pCmdId, void *pArg1 );
ISO_1281_FUN_EXT void*pfun_iso_1281_receive( void *pArg0, void *pArg1 );
#endif








