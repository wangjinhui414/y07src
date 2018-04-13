/*
********************************************************************************
File name  : ZXT_OBD_lib.h
Description: 该文件主要是针对整个系统命令表一些变量共享对其进行声明,绝对不允许在
             这里声明函数。
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立发动机变量声明
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#ifndef  __ZXT_OBD_NOR_STD_CMD_H__
#define  __ZXT_OBD_NOR_STD_CMD_H__

#ifdef    OBD_NOR_STD_VAR
#define   OBD_NOR_STD_VAR_EXT
#else
#define   OBD_NOR_STD_VAR_EXT  extern
#endif
//请将个性车的变量放在这里
#endif
