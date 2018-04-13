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
#ifndef  __ZXT_OBD_STD_CAR_CFG_H__
#define  __ZXT_OBD_STD_CAR_CFG_H__

#ifdef    OBD_STD_CFG_VAR
#define   OBD_STD_CFG_VAR_EXT
#else
#define   OBD_STD_CFG_VAR_EXT  extern //变量声明
#endif
/*
********************************************************************************
                              描述:车型配置表变量声明
具体车型

********************************************************************************
*/
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdRdtc[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdDataStream[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdDataStream1[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdCdtc[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdOdoMeter[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdVin[];
#endif
