/*******************************************************************************
File name  :zxt_obd_include.h
Description:该头文件主要是涉及到底层相关的数据类型，函数声明，变量声明
History    :Ver0.1
修改日期   :修改者:修改内容:简单详述
2012-11-19   小老鼠 主要是整个系统头文件包含
修改日期   :修改者:修改内容:简单详述
*******************************************************************************/
#ifndef __ZXT_OBD_INCLUDE_H_
#define __ZXT_OBD_INCLUDE_H_
//数据类型声明
typedef unsigned char                  BOOLEAN;
typedef unsigned char                  uint8;
typedef signed   char                  int8;
typedef unsigned short                 uint16;
typedef signed   short                 int16;
typedef unsigned int                   uint32;
typedef signed   int                   int32;
typedef float                          fp32;
typedef double                         fp64;

#include <stdio.h> //定义输入／输出函数
#include <stdarg.h>
#include <stdlib.h> //定义杂项函数及内存分配函数
#include <string.h> //字符串处理
#include "stm32f10x.h"
#include "ucos_ii.h"
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"

#include "zxt_obd_bsp.h"
#include "zxt_obd_app_tydef.h"
#include "zxt_obd_fun_lib.h"
#include "zxt_obd_protocol_lib.h"
#include "zxt_obd_all_command_lib.h"
#include "../DEV_SRC/dev_include_h.h"
#include "../OBD_LIB/OBD_PUBLIC/Task/Inc/zxt_obd_task_lib.h"

/*
********************************************************************************
                              描述：OBD部分系统内置参数定义
********************************************************************************
*/
#define  Y06D
#define  DEF_OBD_VERSION               "13.10"                             //由2部分组成(整数部分为OBD架构+小数部分为个性车版本号)

//#define  Y07D
//#define  DEF_OBD_VERSION               "13.10"                             //由2部分组成(整数部分为OBD架构+小数部分为个性车版本号)







 #endif
/************************************文件结束**********************************/



