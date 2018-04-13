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
#ifndef  __ZXT_OBD_STD_CMD_H__
#define  __ZXT_OBD_STD_CMD_H__

#ifdef    ZXT_OBD_STD_VAR
#define   ZXT_OBD_STD_VAR_EXT
#else
#define   ZXT_OBD_STD_VAR_EXT  extern
#endif

/*
********************************************************************************
                                描述:obd标准变量声明
********************************************************************************
*/
//XX系统命令表结构
//xx系统命令索引值配置表
//xx系统数据流
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode0Cmdtab0[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode1Cmdtab0[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab0[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab1[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab2[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab3[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab4[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab5[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode0Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode1Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode2Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode3Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode4Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode5Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode6Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode7Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode8Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_DATASTREAM_GET_DATA_LIST g_stObd0mode0DataStream[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_DATASTREAM_GET_DATA_LIST g_stObd0mode1DataStream[];
#endif
