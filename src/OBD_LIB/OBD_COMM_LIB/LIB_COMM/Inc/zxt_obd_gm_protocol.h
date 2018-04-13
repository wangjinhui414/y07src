/*******************************************************************************
* 文 件   :Zxt_gm_protocol.h
* 作 者   :Mitch mouse
* 说 明   :该头文件主要是通用协议相关数据类型声明和相关函数声明
* 编写时间:
* 版 本   :1.0
* 修改日期:无
********************************************************************************
*/

#ifndef  __ZXT_OBD_GM_PROTOCOL_H__
#define  __ZXT_OBD_GM_PROTOCOL_H__

#ifdef    GM_PROTOCOL_VAR
#define   GM_PROTOCOL_VAR_EXT
#else
#define   GM_PROTOCOL_VAR_EXT  extern
#endif

#ifdef    GM_PROTOCOL_FUN
#define   GM_PROTOCOL_FUN_EXT
#else
#define   GM_PROTOCOL_FUN_EXT  extern
#endif
/*
********************************************************************************
                              描述:通用车数据类型声明
********************************************************************************
*/





/*
********************************************************************************
                              描述:iso-15765变量声明
********************************************************************************
*/

/*
********************************************************************************
                              描述:通用协议函数声明
********************************************************************************
*/

GM_PROTOCOL_FUN_EXT void  fun_gm_send_times( uint8 u8CmdId, void **pCommStatus );
GM_PROTOCOL_FUN_EXT void *pfun_gm_check( void*pArg0, void*pArg1 );
GM_PROTOCOL_FUN_EXT void *pfun_gm_send_command_group( void *pCmdId, void *pArg1 );

#endif








