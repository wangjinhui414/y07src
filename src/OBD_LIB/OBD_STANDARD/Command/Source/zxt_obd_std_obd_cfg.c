/*
********************************************************************************
File name  : Zxt_obd_cfg.c
Description:
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define OBD_CFG_VAR
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
const uint8 u8FunctionParameter[] = {0, 1, 2, 3, 4, 5, 6, 7};
/*

********************************************************************************
                              描述:OBD所有功能调度
********************************************************************************
*/
//标准K线高低电平激活-0BD读码
const TYPEDEF_FUNCTION_CFG stObdRdtcScheduleKLine0[] =
{
    //进入
    0, 0, &u8FunctionParameter[0],
    //读码(不带总数)
    1, 1, &u8FunctionParameter[0],
    //退出
    5, 0, NULL
};
//标准K线地址码激活  68 6A- OBD读码
const TYPEDEF_FUNCTION_CFG stObdRdtcScheduleKLine1[] =
{
    //进入
    0, 1, &u8FunctionParameter[0],
    //读码(不带总数)
    1, 1, &u8FunctionParameter[0],
    //退出
    5, 0, NULL
};
//标准can线激活-OBD读码(斯巴鲁)
const TYPEDEF_FUNCTION_CFG stObdRdtcScheduleCan[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //读码(带总数)
    1, 0, &u8FunctionParameter[0],
};

//标准K线(高低电平激活)读取数据流
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine0[] =
{
    //进入
    0, 0, &u8FunctionParameter[0],
    //数据流
    3, 0, &u8FunctionParameter[0],
    //退出
    5, 0, NULL
};
//标准K线(地址码激活)读取数据流
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine1[] =
{
    //进入
    0, 1, &u8FunctionParameter[0],
    //数据流
    3, 0, &u8FunctionParameter[0],
    //退出
    5, 0, NULL
};
//68 6a(地址码激活)读取数据流
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine2[] =
{
    //进入
    0, 1, &u8FunctionParameter[0],
    //数据流
    3, 0, &u8FunctionParameter[1],
    //退出
    5, 0, NULL
};
//标准CAN线数据流(7e8)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan0[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 0, &u8FunctionParameter[2],
};
//标准CAN线数据流(7e9)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan1[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 0, &u8FunctionParameter[3],
};
//标准CAN线数据流(7ef)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan2[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 0, &u8FunctionParameter[4],
};
//扩张CAN线数据流(0x18daf110)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan3[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 0, &u8FunctionParameter[5],
};
//扩张CAN线数据流(0x18daf111)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan4[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 0, &u8FunctionParameter[6],
};
//扩张CAN线数据流(0x18daf10e)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan5[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 0, &u8FunctionParameter[7],
};
//标准K线(高低电平激活)读取数据流(动态)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine3[] =
{
    //进入
    0, 0, &u8FunctionParameter[0],
    //数据流
    3, 1, &u8FunctionParameter[0],
    //退出
    5, 0, NULL
};
//标准K线(地址码激活)读取数据流(动态)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine4[] =
{
    //进入
    0, 1, &u8FunctionParameter[0],
    //数据流
    3, 1, &u8FunctionParameter[0],
    //退出
    5, 0, NULL
};
//68 6a(地址码激活)读取数据流(动态)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine5[] =
{
    //进入
    0, 1, &u8FunctionParameter[0],
    //数据流
    3, 1, &u8FunctionParameter[1],
    //退出
    5, 0, NULL
};
//标准CAN线数据流(7e8)(动态)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan6[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 1, &u8FunctionParameter[2],
};
//标准CAN线数据流(7e9)(动态)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan7[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 1, &u8FunctionParameter[3],
};
//标准CAN线数据流(7ef)(动态)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan8[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 1, &u8FunctionParameter[4],
};
//扩张CAN线数据流(0x18daf110)(动态)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan9[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 1, &u8FunctionParameter[5],
};
//扩张CAN线数据流(0x18daf111)(动态)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan10[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 1, &u8FunctionParameter[6],
};
//扩张CAN线数据流(0x18daf10e)(动态)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan11[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //数据流
    3, 1, &u8FunctionParameter[7],
};
//标准K线高低电平激活-0BD清码
const TYPEDEF_FUNCTION_CFG stObdCdtcScheduleKLine0[] =
{
    //进入
    0, 0, &u8FunctionParameter[0],
    //清码
    2, 0, NULL,
    //退出
    5, 0, NULL
};
//标准K线地址码激活  68 6A- OBD清码
const TYPEDEF_FUNCTION_CFG stObdCdtcScheduleKLine1[] =
{
    //进入
    0, 1, &u8FunctionParameter[0],
    //清码
    2, 0, NULL,
    //退出
    5, 0, NULL
};
//标准can线激活-OBD清
const TYPEDEF_FUNCTION_CFG stObdCdtcScheduleCan[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //清码
    2, 0, NULL,
};
//里程获取
const TYPEDEF_FUNCTION_CFG stObdOdoMeterSchedule[] =
{
    //注意这里暂用的是第0个位置
    4, 0, NULL,
};

//VIN码获取
//标准can线激活-OBD VIN码获取
const TYPEDEF_FUNCTION_CFG stObdVinScheduleCan[] =
{
    //进入
    0, 2, &u8FunctionParameter[0],
    //vin获取
    6, 0, &u8FunctionParameter[0],
};

//标准K线高低电平激活-OBD VIN码获取
const TYPEDEF_FUNCTION_CFG stObdVinScheduleKLine0[] =
{
    //进入
    0, 0, &u8FunctionParameter[0],
    //VIN码获取
    6, 0, &u8FunctionParameter[1],
    //退出
    5, 0, NULL
};
//标准K线地址码激活  68 6A- OBD VIN码获取
const TYPEDEF_FUNCTION_CFG stObdVinScheduleKLine1[] =
{
    //进入
    0, 1, &u8FunctionParameter[0],
    //VIN码获取
    6, 0, &u8FunctionParameter[1],
    //退出
    5, 0, NULL
};

/*

********************************************************************************
                              描述:OBD读码
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdRdtc[] =
{
    //CAN总线 标准帧_发动机(7e8)
    {
        2, 0, 3,  200, stObdRdtcScheduleCan//0,
    },
    //CAN总线 标准帧_发动机(7e9)
    {
        2, 0, 4,  200, stObdRdtcScheduleCan//0,
    },
    //CAN总线 标准帧_发动机(7ef)
    {
        2, 0, 5,  200, stObdRdtcScheduleCan//0,
    },
    //can总线扩张帧
    {
        2, 0, 6,  200, stObdRdtcScheduleCan//0,
    },
    //can总线扩张帧
    {
        2, 0, 7,  200, stObdRdtcScheduleCan//0,
    },
    //can总线扩张帧
    {
        2, 0, 8,  200, stObdRdtcScheduleCan//0,
    },
    //高低电平激活  K线_发动机(模拟检测通过)
    {
        3, 0, 0,  2000, stObdRdtcScheduleKLine0  //0,
    },
    //地址码激活 K线_发动机(模拟检测通过)
    {
        3, 0, 1,  3000, stObdRdtcScheduleKLine1//(三菱车比较特别)
    },
    //地址码激活 68 6A_发动机(三菱车检查通过)
    {
        3, 0, 2,  200, stObdRdtcScheduleKLine1
    },
    //j1850vpm,pwm放在后面
};
/*
********************************************************************************
                              描述:OBD数据流固定
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdDataStream[] =
{
    //CAN总线 标准帧_发动机(7e8)
    {
        2, 0, 3,  200, stObdDataStreamScheduleCan0
    },
    //CAN总线 标准帧_发动机(7e9)
    {
        2, 0, 4,  200, stObdDataStreamScheduleCan1
    },
    //CAN总线 标准帧_发动机(7ef)
    {
        2, 0, 5,  200, stObdDataStreamScheduleCan2
    },
    //can总线扩张帧
    {
        2, 0, 6,  200, stObdDataStreamScheduleCan3
    },
    //can总线扩张帧
    {
        2, 0, 7,  200, stObdDataStreamScheduleCan4
    },
    //can总线扩张帧
    {
        2, 0, 8,  200, stObdDataStreamScheduleCan5
    },
    //高低电平激活  K线_发动机(模拟检测通过)
    {
        3, 0, 0,  2000, stObdDataStreamScheduleKLine0
    },
    //地址码激活 K线_发动机
    {
        3, 0, 1,  3000, stObdDataStreamScheduleKLine1//(三菱车比较特别)
    },
    //地址码激活 68 6A_发动机(模拟检测通过,三菱车检查通过)
    {
        3, 0, 2,  200, stObdDataStreamScheduleKLine2
    },
    //j1850vpm,pwm放在后面
};
/*
********************************************************************************
                              描述:OBD数据流动态
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdDataStream1[] =
{
    //CAN总线 标准帧_发动机(7e8)
    {
        2, 0, 3,  200, stObdDataStreamScheduleCan6
    },
    //CAN总线 标准帧_发动机(7e9)
    {
        2, 0, 4,  200, stObdDataStreamScheduleCan7
    },
    //CAN总线 标准帧_发动机(7ef)
    {
        2, 0, 5,  200, stObdDataStreamScheduleCan8
    },
    //can总线扩张帧
    {
        2, 0, 6,  200, stObdDataStreamScheduleCan9
    },
    //can总线扩张帧
    {
        2, 0, 7,  200, stObdDataStreamScheduleCan10
    },
    //can总线扩张帧
    {
        2, 0, 8,  200, stObdDataStreamScheduleCan11
    },
    //高低电平激活  K线_发动机(模拟检测通过)
    {
        3, 0, 0,  2000, stObdDataStreamScheduleKLine3
    },
    //地址码激活 K线_发动机
    {
        3, 0, 1,  3000, stObdDataStreamScheduleKLine4//(三菱车比较特别)
    },
    //地址码激活 68 6A_发动机(模拟检测通过,三菱车检查通过)
    {
        3, 0, 2,  200, stObdDataStreamScheduleKLine5
    },
    //j1850vpm,pwm放在后面
};
/*

********************************************************************************
                              描述:OBD清码
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdCdtc[] =
{
    //CAN总线 标准帧_发动机(7e8)
    {
        2, 0, 3,  200, stObdCdtcScheduleCan
    },
    //CAN总线 标准帧_发动机(7e9)
    {
        2, 0, 4,  200, stObdCdtcScheduleCan
    },
    //CAN总线 标准帧_发动机(7ef)
    {
        2, 0, 5,  200, stObdCdtcScheduleCan
    },
    //can总线扩张帧
    {
        2, 0, 6,  200, stObdCdtcScheduleCan
    },
    //can总线扩张帧
    {
        2, 0, 7,  200, stObdCdtcScheduleCan
    },
    //can总线扩张帧
    {
        2, 0, 8,  200, stObdCdtcScheduleCan
    },
    //高低电平激活  K线_发动机(模拟检测通过)
    {
        3, 0, 0,  2000, stObdCdtcScheduleKLine0
    },
    //地址码激活 K线_发动机(模拟检测通过)
    {
        3, 0, 1,  3000, stObdCdtcScheduleKLine1//(三菱车比较特别)
    },
    //地址码激活 68 6A_发动机(三菱车检查通过)
    {
        3, 0, 2,  200, stObdCdtcScheduleKLine1
    },
    //j1850vpm,pwm放在后面
};
/*

********************************************************************************
                              描述:OBD读里程
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdOdoMeter[] =
{
    //注意这里系统数和模块数都是255,其实是不存在的
    {
        1, 0xff, 0xff, 200, stObdOdoMeterSchedule
    },
};

/*

********************************************************************************
                              描述:OBD读VIN
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdVin[] =
{
    //CAN总线 标准帧_发动机(7e8)
    {
        2, 0, 3,  200, stObdVinScheduleCan//0,
    },
    //CAN总线 标准帧_发动机(7e9)
    {
        2, 0, 4,  200, stObdVinScheduleCan//0,
    },
    //CAN总线 标准帧_发动机(7ef)
    {
        2, 0, 5,  200, stObdVinScheduleCan//0,
    },
    //can总线扩张帧
    {
        2, 0, 6,  200, stObdVinScheduleCan//0,
    },
    //can总线扩张帧
    {
        2, 0, 7,  200, stObdVinScheduleCan//0,
    },
    //can总线扩张帧
    {
        2, 0, 8,  200, stObdVinScheduleCan//0,
    },
    //高低电平激活  K线_发动机(模拟检测通过)
    {
        3, 0, 0,  2000, stObdVinScheduleKLine0  //0,
    },
    //地址码激活 K线_发动机(模拟检测通过)
    {
        3, 0, 1,  3000, stObdVinScheduleKLine1//(三菱车比较特别)
    },
};
