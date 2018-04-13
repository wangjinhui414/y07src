/*
********************************************************************************
File name  : ZXT_fun_variables_init.h
Description: 该头文件主要是底层要调用的功能库数组声明，只是声明，不允许在这里声明
             函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立整个系统框架的功能库
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#ifndef  __ZXT_OBD_FUN_LIB_CFG_H__
#define  __ZXT_OBD_FUN_LIB_CFG_H__

#ifdef    FUN_LIB
#define   FUN_LIB_EXT
#else
#define   FUN_LIB_EXT  extern
#endif
/*
********************************************************************************
                            描述:ZXT_create_link.c函数配置
********************************************************************************
*/
#define   CREATE_LINK_LOW_HIGH_25MS_EN        1                            //低高电平25MS
#define   ACTIVE_5BAUD_INIT_EN                1                            //地址码激活
#define   ACTIVE_5BAUD_ONLY_LAST_BYTE_EN      1                            //地址码激活类型0
#define   ACTIVE_5BAUD_KWP1281_EN             1                            //地址码激活类型0
#define   ACTIVE_SEND_ADDCODE_EN              1                            //自动检测波特率
#define   CREATE_LINK_CAN_BUS_EN              1                            //CAN总线初始化使能

/*
********************************************************************************
                            描述:ZXT_read_dtc.c函数配置
********************************************************************************
*/

#define RDTC_WITCH_TOTAL_EN             1                                  //当前故障码函数配置带总数
#define ALLCAR_RDTCL_EN                 1                                  //框架中的读取故障码函数
#define RDTC_DEAL_WITH_TOTAL_EN         1                                  //处理带总数的故障码函数
#define RDTC_LOAD_DTC_EN                1                                  //装载故障码
#define RDTC_WITCH_NOTOTAL_EN           1                                  //当前故障码函数配置不带总数
#define RDTC_DEAL_WITH_NOTOTALL_EN      1                                  //处理不带总数的故障码函数
#define RDTC_GM_BUCK_WITCH_NOTOTAL_EN   1                                  //当前通用故障码函数配置不带总数
#define RDTC_VW_PUSANG_WITCH_NOTOTAL_EN 1                                  //当前普桑故障码函数配置不带总数

/*
********************************************************************************
                            描述:ZXT_clear_dtc.c函数配置
********************************************************************************
*/
#define CDTC_FOR_ALL_CAR_EN            1                                   //清除故障码函数配置表
/*
********************************************************************************
                            描述:ZXT_data_stream.c函数配置
********************************************************************************
*/
#define RDATA_STREAM_EN                1                                   //读取系统数据流
#define INIT_DATA_STREAM_IDLIST        1                                   //初始化数据流链表
#define GET_DATA_STREAM_CALCULATION_BYTES_EN 1                             //获取ECU数据关键字节

/*
********************************************************************************
                             描述:Zxt_get_odometer函数配置表
********************************************************************************
*/
#define GET_ODOMETER_WITHCALCULATION_EN  1       //通过命令获取系统里程函数
#define GET_ODO_METER_ALLCAR_EN          1       //系统架构读取里程函数

/*
********************************************************************************
                             描述:Zxt_obd_getvin函数配置表
********************************************************************************
*/
#define GET_VIN_FOR_ALLCAR_EN  1       //通过命令获取系统里程函数

/*
********************************************************************************
                             描述:数据类型
********************************************************************************
*/
typedef void*( *TYPEDEF_pFUNLIB )( void*, void* );

/*
********************************************************************************
                              描述:底层要调用的函数库变量声明
********************************************************************************
*/
FUN_LIB_EXT const TYPEDEF_pFUNLIB      *g_pFunLib[];//整个系统总
#endif




