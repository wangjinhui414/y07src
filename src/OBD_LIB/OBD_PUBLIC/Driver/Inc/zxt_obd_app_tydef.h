/*******************************************************************************
File name  :zxt_obd_datastruct_tydef.h
Description:该文件主要是涉及到底层相关数据类型定义
History    :Ver0.1
修改日期   :修改者:修改内容:简单详述
2014/11/19 小老鼠 重新建立底层数据接口
*******************************************************************************/
/********************************************************************************
                              描述:数据类型重新定义
********************************************************************************
*/
#ifndef _ZXT_OBD_DATASTRUCT_TYDEF_H_
#define _ZXT_OBD_DATASTRUCT_TYDEF_H_

#ifdef    ZXT_OBD_DATASTRUCT_VAR
#define   ZXT_OBD_DATASTRUCT_VAR_EXT
#else
#define   ZXT_OBD_DATASTRUCT_VAR_EXT  extern
#endif

/*
********************************************************************************
                              描述:应用层相关数据块大小定义
********************************************************************************
*/
#define  DEF_MSGBUF_SIZE            4
#define  DEF_RDTCBUF_SIZE           600
#define  DEF_RDATASTREAMBUF_SIZE    11
#define  DEF_ODOMETERBUF_SIZE       4
#define  DEF_RVIN_SIZE              25
/*
********************************************************************************
                              描述:底层系统数据类型定义-信息结构
********************************************************************************
*/

typedef struct TYPEDEF_FUNCTION_MSGBUF
{
    uint16  u16Len;
    uint8  *pu8FunBuffer;
} TYPEDEF_FUNCTION_MSGBUF;

typedef struct TYPEDEF_DATASTREAM_MSGBUF
{
	   uint8 u8Datalen;
	   uint8 u8Idlen;
	   uint8 u8DataStreamDataBuf[DEF_RDATASTREAMBUF_SIZE*5];
	   uint8 u8DataStreamIdBuf  [DEF_RDATASTREAMBUF_SIZE];
}TYPEDEF_DATASTREAM_MSGBUF;




/*
********************************************************************************
                              描述:车型定义
********************************************************************************
*/
typedef struct TYPEDEF_OBD_CACHE_CAR
{    
    uint8 u8SysLibChoice;                
    uint8 u8SysChoice;                   
} TYPEDEF_OBD_CACHE_CAR;

typedef struct TYPEDEF_OBD_PAR
{
  uint16  savedMark;                                                       //参数初始化过标志
  uint16  sum;                                                             //参数累加和标志


  uint16  u16OdoPAR0;
  uint32  u32OdoMeter:24;                                                  //obd计算
  
  bool    bCacheSyeFinish;                                                 //判断是否缓存过系统
	uint8   u8CacheSysTotal;                                                 //系统总数
  uint8   u8CacheCarType;                                                  //当前车型更改  
  TYPEDEF_OBD_CACHE_CAR  SysPar[20];                                       //最多支持20个系统	
}TYPEDEF_OBD_PAR;

typedef struct TYPEDEF_FUNCTION_CFG
{
    const uint8 u8FunLibChoice;                                            //选择的是哪一个库中的数据函数
    const uint8 u8FunChoice;                                               //选择具体的函数
    const uint8 *pFunctionParameter;                                       //该函数需要的参数
}TYPEDEF_FUNCTION_CFG;

typedef struct TYPEDEF_SYS_CFG
{
    const uint8 u8FunTotal;                                                //当前车型功能总数
    const uint8 u8SysLibChoice;                                            //具体系统
    const uint8 u8SysChoice;                                               //具体模块
    const uint16 u16SysDelay;                                              //系统与系统之间延时
    const TYPEDEF_FUNCTION_CFG *pstFunction;
} TYPEDEF_SYS_CFG;


typedef struct TYPEDEF_All_SYS_CFG
{
    const uint8    u8SysTotal;                                             //当前功能包含的系统总数
    const TYPEDEF_SYS_CFG *pstSysCfg;                                      //指向具体的系统
} TYPEDEF_All_SYS_CFG;
typedef struct
{
    const uint16   u16CarDelay;                                            //功能与功能之间的延时
    TYPEDEF_All_SYS_CFG stPersonAndObdCfg[2];
} TYPEDEF_All_CAR_CFG;                                                     //具体功能配置

///*
//********************************************************************************
//                              描述:资源定义
//********************************************************************************
//*/
////整个系统命令索引值,以后再调整调整这一块
//typedef struct TYPEDEF_CMD_INDEX
//{
//  const uint8 *pCmdEnter;                                                  //进入命令
//  const uint8 *pCmdIdle;                                                   //空闲命令
//  const uint8 *pCmdRDtc;                                                   //读当前故障码
//  const uint8 *pCmdClearDtc;                                               //清码命令
//  const uint8 *pCmdGetOdoMeter;                                            //获取系统里程命令
//  const uint8 *pCmdSysExit;                                                //系统退出命令
//	const uint8 *pCmdGetVin;                                                 //获取车辆vin码
//} TYPEDEF_CMD_INDEX;                                                       //命令索引值成员

//typedef struct TYPEDEF_CMD_TAB
//{
//    const uint8 u8BufferId;                                                 //命令存储的缓冲区
//    const uint8 u8CmdLen;                                                   //命令长度
//    const uint8 u8CmdIdOffset;                                              //u8CmdIdOffset
//    const uint8 u8FilterIdOffset;                                           //滤波器设置
//    const uint8 u8FilterGroup;                                              //14组滤波器中哪一组,主要是针对CAN线
//    const uint8 *pu8Command;                                                //指向具体的命令
//} TYPEDEF_CMD_TAB;

//typedef  struct TYPEDEF_MODE_CELL
//{
//    const TYPEDEF_CMD_TAB    *pCmdTabChoice;                               //指向整个框架中的发动机系统
//    const TYPEDEF_CMD_INDEX  *pCmdIndexTabChoice;                          //整个系统命令索引值表
//    const uint8            u8ProtocolLibOffset[5];                         //这里放置的是发送和接收函数的选择
//} TYPEDEF_MODE_CELL;

//typedef union TYPEDEF_1WORD2BYTE
//{
//    uint16 u16Bit;
//    uint8  u8Bit[2];
//} TYPEDEF_1WORD2BYTE;

//typedef union  TYPEDEF_2WORD4BYTE
//{
//    uint32 u32Bit;
//    uint16 u16Bit;
//    uint8  u8Bit[4];
//} TYPEDEF_2WORD4BYTE;

//底层相关变量
ZXT_OBD_DATASTRUCT_VAR_EXT    const                      TYPEDEF_MODE_CELL   *g_pModeLibTab;
//ZXT_OBD_DATASTRUCT_VAR_EXT    uint8                      g_u8ProtocolLibOffset;
ZXT_OBD_DATASTRUCT_VAR_EXT    uint8                      g_u8RdtcBuf0[DEF_RDTCBUF_SIZE];
ZXT_OBD_DATASTRUCT_VAR_EXT    TYPEDEF_DATASTREAM_MSGBUF  g_u8RdataStreamBuf;
ZXT_OBD_DATASTRUCT_VAR_EXT    uint8                      g_u8OdoMeterBuf[DEF_ODOMETERBUF_SIZE];
ZXT_OBD_DATASTRUCT_VAR_EXT    uint8                      g_u8RvinBuf[DEF_RVIN_SIZE];
ZXT_OBD_DATASTRUCT_VAR_EXT    TYPEDEF_FUNCTION_MSGBUF    g_stFunMsgBuf[DEF_MSGBUF_SIZE];
ZXT_OBD_DATASTRUCT_VAR_EXT    TYPEDEF_OBD_PAR            g_stObdPar;
ZXT_OBD_DATASTRUCT_VAR_EXT    TYPEDEF_OBD_PAR            g_stObdParVehicleState;
#endif
