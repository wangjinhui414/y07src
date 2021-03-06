/*
********************************************************************************
File name  : Zxt_car_cfg.c
Description:
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define  OBD_NOR_STD_CAR_CFG_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                              描述:功能调配数组
参数填写顺序:
1.选择的是哪一个功能库(例如:创建连接g_pCreatLinkLib,读码库g_pReaDtcLib)
2.选择的是对应库中的那个函数
3.该函数需要的参数,可有可无,需要开发人员自由配置
********************************************************************************
*/


/*
********************************************************************************
                              描述:读码
********************************************************************************
*/

/*
********************************************************************************
                              描述:读里程
********************************************************************************
*/

/*
********************************************************************************
                              描述:具体车型
参数填写顺序:
1.功能总数当前系统支持多少功能(读码，清码, 数据流,里程等等)
2.系统类型(eng=0,AT=1)请以这个g_pstSysLibTab为基准
3.系统模块例如g_stEngLibTab[1]具体的模块
4.激活方式主要是选择例如g_stIso15765ActiveLib[X]或者g_stIso14230ActiveLib[x]
5.系统延时时间
6.功能集配置(可以是数组,也可以是单独的变量)(例如读码，清码等等)。
切记:1.如果该车某一个系统支持多种协议,也当做一个系统来处理,直接往里面填写
     2.请注意命名规则(最好用具体的车型来命名)
********************************************************************************
*/

/*
********************************************************************************
                              描述:东南三菱车
********************************************************************************
*/

/*
********************************************************************************
                              描述:具体车系配置
参数填写顺序:
1.系统总数(包括个性和obd)
2.以前叫车型与车型之间的延时,现在叫功能与功能延时,切记和功能调配数组区别
3.具体系统=个性+obd(切记个性放在前面)
4.这个表放置顺序不允许更改
5.以前读取故障码和读取里程是允许放在一起的，现在不允许放在一起,下一个版本再将他们允许放在一起
********************************************************************************
*/
TYPEDEF_All_CAR_CFG g_stCarType[DEF_MAX_FUN_SIZE] =
{

    //读码
    {
        1000, {0, NULL, 9, g_stObdRdtc}
    },
    //读取数据流
    {
        1000, {9, g_stObdDataStream, 0, NULL}
    },
    //清码
    {
        1000, {9, g_stObdCdtc, 0, NULL}
    },
    //读取里程
    {
        1000, {0, NULL, 1, g_stObdOdoMeter}
    },
    //动态读取数据流
    {
        1000, {9, g_stObdDataStream1, 0, NULL}
    },
    //读VIN
    {
        1000, {8, g_stObdVin, 0, NULL}
    },
};

