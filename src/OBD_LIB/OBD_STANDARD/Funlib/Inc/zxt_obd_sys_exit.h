/*
********************************************************************************
File name  : Zxt_sys_exit.h
Description: 这里面主要是整个系统退出命令的集合
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-11-20   小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#ifndef  __ZXT_OBD_SYS_EXIT_H__
#define  __ZXT_OBD_SYS_EXIT_H__

#ifdef    SYS_EXIT_VAR
#define   SYS_EXIT_VAR_EXT
#else
#define   SYS_EXIT_VAR_EXT  extern
#endif

#ifdef    SYS_EXIT_FUN
#define   SYS_EXIT_FUN_EXT
#else
#define   SYS_EXIT_FUN_EXT  extern
#endif















/*
********************************************************************************
                              描述:系统退出函数声明
********************************************************************************
*/
SYS_EXIT_FUN_EXT void *pfun_sys_exit_fun( void *pstsysChoice, void *pu8FunctionParameter );

#endif






