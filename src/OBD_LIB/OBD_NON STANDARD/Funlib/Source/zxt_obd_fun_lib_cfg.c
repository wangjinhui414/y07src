/*
********************************************************************************
File name  : ZXT_fun_lib_init.c
Description: 这里初始化和定义的变量都是供底层调用的全局变量
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/

#define  FUN_LIB
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
********************************************************************************
                            描述:Creatlink.c变量初始化
********************************************************************************
*/

const TYPEDEF_pFUNLIB g_pCreatLinkLib[] =
{
#if CREATE_LINK_LOW_HIGH_25MS_EN == 1
    pfun_active_low_high_xxms,//高低电平激活函数
#endif
#if ACTIVE_5BAUD_ONLY_LAST_BYTE_EN == 1
    pfun_active_5baud_negate_last_byte_back_addr,
#endif
#if CREATE_LINK_CAN_BUS_EN ==1
    pfun_active_can_bus,
#endif
#if ACTIVE_5BAUD_KWP1281_EN == 1
    pfun_active_5baud_kwp1281
#endif
};
/*
********************************************************************************
                            描述:Readtc.c变量初始化
********************************************************************************
*/
const TYPEDEF_pFUNLIB g_pReaDtcLib[] =
{
#if    RDTC_WITCH_TOTAL_EN==1
    pfun_allcar_rdtc_witch_total,//读取当前故障码,同时是有总数
#endif

#if    RDTC_WITCH_NOTOTAL_EN==1
    pfun_allcar_rdtc_witch_nototal,//读取当前故障码,同时没有总数
#endif
#if  RDTC_GM_BUCK_WITCH_NOTOTAL_EN == 1
    pfun_buck_rdtc_witch_nototal, //通用车读取故障码
#endif
#if RDTC_VW_PUSANG_WITCH_NOTOTAL_EN==1
    pfun_vw_pusang_rdtc_witch_nototal //大众1281普桑车读取故障码
#endif
};
/*
********************************************************************************
                            描述:Cleardtc.c变量初始化
********************************************************************************
*/
const TYPEDEF_pFUNLIB g_pClearDtcLib[] =
{
#if CDTC_FOR_ALL_CAR_EN==1
    pfun_cdtc_for_all_car,
#endif
};
/*
********************************************************************************
                            描述:Datastream.c变量初始化
********************************************************************************
*/
const TYPEDEF_pFUNLIB g_pDataStreamLib[] =
{
#if RDATA_STREAM_EN==1
    pfun_data_stream_all_car,
    pfun_data_stream_all_Obd,
#endif
};
/*
********************************************************************************
                              描述:Zxt_get_odometer.c
********************************************************************************
*/
const TYPEDEF_pFUNLIB g_pOdoMeterLib[] =
{

    pfun_get_odometer_from_obd,
#if GET_ODOMETER_WITHCALCULATION_EN == 1
    pfun_get_odometer_withcalculation,
#endif
    pfun_get_odometer_from_bus,
};
/*
********************************************************************************
                              描述:Zxt_sys_exit.c
********************************************************************************
*/
const TYPEDEF_pFUNLIB      g_pSysExitLib[] =
{
    pfun_sys_exit_fun,
};

/*
********************************************************************************
                              描述:Zxt_sys_vin.c
********************************************************************************
*/
const TYPEDEF_pFUNLIB      g_pSysVinLib[] =
{
    pfun_getvin_for_all_car,
};
/*
********************************************************************************
                            描述:整个系统fun_lib结构体
********************************************************************************
*/

const TYPEDEF_pFUNLIB *g_pFunLib[] =
{
    g_pCreatLinkLib,      //链接
    g_pReaDtcLib,         //读码
    g_pClearDtcLib,       //清码
    g_pDataStreamLib,     //数据流
    g_pOdoMeterLib,       //里程
    g_pSysExitLib,        //退出
    g_pSysVinLib,         //VIN获取
};




