/*
********************************************************************************
File name  : ZXT_Read_dtc.h
Description: 该头文件主要是Datastream.c中相关变量函数的声明,不允许在这里放置无关信息
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立读取故障码变量和函数声明
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#ifndef  __ZXT_OBD_READ_DTC_H__
#define  __ZXT_OBD_READ_DTC_H__

#ifdef    READ_DTC_VAR
#define   READ_DTC_VAR_EXT
#else
#define   READ_DTC_VAR_EXT  extern
#endif

#ifdef    READ_DTC_FUN
#define   READ_DTC_FUN_EXT
#else
#define   READ_DTC_FUN_EXT  extern
#endif


/*
********************************************************************************
                               描述:读取故障码数据类型定义
********************************************************************************
*/

typedef struct TYPEDEF_DTC_WITHITOTAL
{
    uint8 u8DtcTotalCacheAddr;                                             //故障码总数缓存地址
    uint8 u8DtcTotalByte;                                                  //故障码总数字节
    uint8 u8DtcCodeCacheAddr;                                              //故障码号缓存地址
    uint8 u8DtcCodeStartByte;                                              //故障码号开始字节
    uint8 u8DtcCodevalidByte;                                              //故障码有效字节数(几个字节一个故障码)
    uint8 u8DtcCodeXXByteOneDtc;                                           //多少个字节一个故障码
    uint8 u8DtcCodeLibChoice;                                              //码库选择
} TYPEDEF_DTC_WITHITOTAL;                                                        //有故障码总数
                                                                           
typedef struct TYPEDEF_DTC_NOTOTAL
{                                                                          
    uint8 u8DtcCodeCacheAddr;                                              //故障码号缓存地址
    uint8 u8DtcCodeStartByte;                                              //故障码号开始字节
    uint8 u8DtcCodevalidByte;                                              //故障码有效字节数(几个字节一个故障码)
    uint8 u8DtcCodeXXByteOneDtc;                                           //多少个字节一个故障码
    uint8 u8DtcCodeLibChoice;                                              //码库选择
} TYPEDEF_DTC_NOTOTAL;                                                      //没有故障码总数





/*
********************************************************************************
                               描述:读取故障码变量声明
********************************************************************************
*/

READ_DTC_VAR_EXT const TYPEDEF_DTC_WITHITOTAL g_stDtcWithTotalConfigLib[];  //带总数故障码获取
READ_DTC_VAR_EXT const TYPEDEF_DTC_NOTOTAL    g_stDtcWithNoTotalConfigLib[];//不带总数故障码获取

/*
********************************************************************************
                               描述:读取故障码函数声明
pfun_allcar_rdtc_witch_total读取当前故障码有总数
pfun_rdtc_for_history_witch_totall读取历史故障码有总数
pfun_allcar_rdtc                  任何函数都可以用的读取故障码函数
fun_rdtc_deal_with_total          处理带总数故障码
fun_rdtc_load_dtc                 装载故障码
********************************************************************************
*/

READ_DTC_VAR_EXT void *pfun_allcar_rdtc_witch_total( void *pstsysChoice, void *pu8FunctionParameter );
READ_DTC_VAR_EXT void *pfun_allcar_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter );
READ_DTC_VAR_EXT void *pfun_buck_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter );
READ_DTC_VAR_EXT void *pfun_vw_pusang_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter );
READ_DTC_VAR_EXT void *pfun_rdtc_allcar( uint8 *pCmdIndex, void *pArg1 );
READ_DTC_VAR_EXT uint16 fun_rdtc_deal_with_total( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache );
READ_DTC_VAR_EXT uint16 fun_rdtc_deal_with_nototal( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache );
READ_DTC_VAR_EXT uint16 fun_buck_rdtc_deal_with_nototal( uint8 u8SysChoice, uint8 *pCache );
READ_DTC_VAR_EXT uint16 fun_vw_pusang_rdtc_deal_with_nototal( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache );
READ_DTC_VAR_EXT void fun_rdtc_load_dtc( uint8 u8DtcTotal, uint8 u8DtcCodeXXByteOneDtc,
        uint8 u8DtcCodevalidByte, uint8 *pCache, uint8 *pDtcCodeStart );

#endif






