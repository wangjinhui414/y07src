/*
********************************************************************************
File name  : Zxt_get_odometer.h
Description:该文件主要是所有获取里程文件相关函数类型声明
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-11-20   小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#ifndef  __ZXT_OBD_GET_ODOMETER_H__
#define  __ZXT_OBD_GET_ODOMETER_H__

#ifdef    GET_ODOMETER_VAR
#define   GET_ODOMETER_VAR_EXT
#else
#define   GET_ODOMETER_VAR_EXT  extern
#endif

#ifdef    GET_ODOMETER_FUN
#define   GET_ODOMETER_FUN_EXT
#else
#define   GET_ODOMETER_FUN_EXT  extern
#endif
/*
********************************************************************************
                              描述:数据声明
********************************************************************************
*/
//总线里程获取
typedef struct TYPEDEF_BUS_ODO_PAR
{
    uint32 u32BusFrameId;                                                  //帧ID
    uint8  u8StartByte;                                                    //从哪一个字节开始获取
    uint16 u16FrameIdTime;                                                 //等待时间
    bool   bSwapByte;                                                      //高低位互换
} TYPEDEF_BUS_ODO_PAR;


/*
********************************************************************************
                              描述:变量声明
********************************************************************************
*/
GET_ODOMETER_VAR_EXT TYPEDEF_BUS_ODO_PAR g_stOdoMeterLib    [];
GET_ODOMETER_VAR_EXT uint32              g_u32CalibrationOdo;
/*
********************************************************************************
                              描述:函数声明
********************************************************************************
*/
GET_ODOMETER_FUN_EXT void *pfun_get_odometer_withcalculation( void *pstsysChoice, void *pu8FunctionParameter );
GET_ODOMETER_FUN_EXT void *pfun_get_odometer_allcar( uint8 *pCmdIndex, void *pArg1 );
GET_ODOMETER_FUN_EXT void *pfun_get_odometer_from_bus( void *pArg0, void *pu8FunctionParameter );
GET_ODOMETER_FUN_EXT void *pfun_get_odometer_from_obd( void *pArg0, void *pu8FunctionParameter );

#endif





