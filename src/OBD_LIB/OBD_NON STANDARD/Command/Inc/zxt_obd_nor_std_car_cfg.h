/*
********************************************************************************
File name  : Zxt_car_cfg.h
Description:
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#ifndef  __ZXT_OBD_OBD_CAR_CFG_H__
#define  __ZXT_OBD_OBD_CAR_CFG_H__

#ifdef    OBD_NOR_STD_SYS_CFG_VAR
#define   OBD_NOR_STD_SYS_CFG_VAR_EXT
#else
#define   OBD_NOR_STD_SYS_CFG_VAR_EXT  extern //变量声明
#endif

#ifdef    OBD_NOR_STD_CAR_CFG_VAR
#define   OBD_NOR_STD_CAR_CFG_VAR_EXT
#else
#define   OBD_NOR_STD_CAR_CFG_VAR_EXT  extern //变量声明
#endif
/*
********************************************************************************
                              描述:功能相关大小定义

********************************************************************************
*/
#define DEF_MAX_FUN_SIZE               6                                   //目前所有系统的功能总数 

/*
********************************************************************************
                              描述:车型配置表变量声明
具体车型

********************************************************************************
*/
OBD_NOR_STD_CAR_CFG_VAR_EXT TYPEDEF_All_CAR_CFG  g_stCarType[DEF_MAX_FUN_SIZE];   //当前车型表
OBD_NOR_STD_SYS_CFG_VAR_EXT const TYPEDEF_MODE_CELL   *g_pstSysLibTab[];         //系统库

#endif
