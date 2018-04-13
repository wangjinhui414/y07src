/*
********************************************************************************
File name  : zxt_obd_getvin.h
Description: 该头文件主要是zxt_obd_getvin.c中相关变量和函数声明,不允许在这里放置无关信息
History    :
修改日期   : 修改者 : 修改内容:简单详述
2015/7/19    小老鼠   建立创建通讯连接相关函数，变量声明
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/

#ifndef _ZXT_OBD_GETVIN_H
#define _ZXT_OBD_GETVIN_H

#ifdef    GET_VIN__VAR
#define   GET_VIN__VAR_EXT
#else
#define   GET_VIN__VAR_EXT  extern //变量声明
#endif

#ifdef    GET_VIN_FUN
#define   GET_VIN_FUN_EXT
#else
#define   GET_VIN_FUN_EXT  extern//函数声明
#endif

/*
********************************************************************************
                              描述:VIN码数据类型声明
********************************************************************************
*/
//总线里程获取
typedef struct TYPEDEF_VIN_PAR0
{        
    uint8  u8StartByte;                                     //高四位00:can 01:K 低四位从哪一个字节开始获取
    uint8  u8PackTotal;                                     //处理包数
    uint8  u8FirstFramePid;                                 //默认是将数据往左移动
    uint8  u8PackLen;                                       //包长度 
} TYPEDEF_VIN_PAR0;
/*
********************************************************************************
                              描述:VIN码变量声明
********************************************************************************
*/

GET_VIN__VAR_EXT TYPEDEF_VIN_PAR0 g_stVinLib[];
 
/*
********************************************************************************
                            描述:VIN码函数声明
********************************************************************************
*/
GET_VIN_FUN_EXT void *pfun_getvin_for_all_car( void *pArg0, void *pu8FunctionParameter );






#endif 
