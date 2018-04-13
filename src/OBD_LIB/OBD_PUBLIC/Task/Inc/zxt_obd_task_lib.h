/*******************************************************************************
* 文 件   :ZXT_task_lib.h
* 作 者   :Mitch mouse
* 说 明   :该文件主要是将所有的任务上变量进行声明,绝对不允许在这里声明任何与任务
*          无关的变量
* 编写时间:
* 版 本   :1.0
* 修改日期:无
********************************************************************************
*/
#ifndef  __ZXT_OBD_TASK_LIB_H__
#define  __ZXT_OBD_TASK_LIB_H__

#ifdef   ZXT_OBD_TASK_CREAT_VAR
#define  ZXT_OBD_TASK_CREAT_VAR_EXT
#else 
#define  ZXT_OBD_TASK_CREAT_VAR_EXT  extern
#endif

#ifdef   ZXT_OBD_TASK_CREAT_FUN
#define  ZXT_OBD_TASK_CREAT_FUN_EXT
#else 
#define  ZXT_OBD_TASK_CREAT_FUN_EXT extern
#endif

#ifdef    ZXT_OBD_TASK_FUN_PROCESS_VAR
#define   ZXT_OBD_TASK_FUN_PROCESS_VAR_EXT
#else
#define   ZXT_OBD_TASK_FUN_PROCESS_VAR_EXT  extern
#endif

#ifdef    ZXT_OBD_TASK_FUN_PROCESS_FUN
#define   ZXT_OBD_TASK_FUN_PROCESS_FUN_EXT
#else
#define   ZXT_OBD_TASK_FUN_PROCESS_FUN_EXT  extern
#endif

#ifdef    ZXT_OBD_TASK_ECU_PROCESS_VAR
#define   ZXT_OBD_TASK_ECU_PROCESS_VAR_EXT
#else
#define   ZXT_OBD_TASK_ECU_PROCESS_VAR_EXT  extern
#endif


#ifdef    ZXT_OBD_TASK_ECU_PROCESS_FUN
#define   ZXT_OBD_TASK_ECU_PROCESS_FUN_EXT
#else
#define   ZXT_OBD_TASK_ECU_PROCESS_FUN_EXT  extern
#endif


#ifdef    ZXT_OBD_TASK_SCHEDULE_VAR
#define   ZXT_OBD_TASK_SCHEDULE_VAR_EXT
#else
#define   ZXT_OBD_TASK_SCHEDULE_VAR_EXT  extern
#endif


#ifdef    ZXT_OBD_TASK_SCHEDULE_FUN
#define   ZXT_OBD_TASK_SCHEDULE_FUN_EXT
#else
#define   ZXT_OBD_TASK_SCHEDULE_FUN_EXT  extern
#endif


#ifdef    ZXT_OBD_TRANSFERLAYER_PROCESS_VAR
#define   ZXT_OBD_TRANSFERLAYER_PROCESS_VAR_EXT
#else
#define   ZXT_OBD_TRANSFERLAYER_PROCESS_VAR_EXT  extern
#endif

#ifdef    ZXT_OBD_TRANSFERLAYER_PROCESS_FUN
#define   ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT
#else
#define   ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT  extern
#endif
/*
********************************************************************************
                                任务优先级宏定义
                                这个优先级已经被暂用，切记不准使用obd(4-8任务优先级)
********************************************************************************
*/
//13-18属于OBD
#define  DEF_OBD_TASK_TRANSFERLAYPER_PRIO           25                     //和硬件部门交互数据相关
#define  DEF_OBD_TASK_SCHEDULE_PRIO                 14                     //任务调度优先级
#define  DEF_OBD_TASK_FUN_PRIO                      15                     //手机端和OBD设备之间通讯
#define  DEF_OBD_TASK_ECU_PRIO                      16                     //OBD设备和ECU之间通讯？？？？？？？

/*
********************************************************************************
*                                任务栈大小定义
********************************************************************************
*/
#define  DEF_OBD_TASK_FUN_SIZE                      256
#define  DEF_OBD_TASK_ECU_SIZE                      512
#define  DEF_OBD_TASK_SCHEDULE_SIZE                 128
#define  DEF_OBD_TASK_TRANSFERLAYPER_SIZE           100

/*
********************************************************************************
                              描述:资源大小定义
********************************************************************************
*/
#define  DEF_SUPPORT_SYS_MAX                         20
#define  DEF_CMD_BUFFER_SIZE                         100
#define  DEF_DATA_BUFFER_SIZE                        500

/*
********************************************************************************
                              描述:服务器端协议命令解析
********************************************************************************
*/
#define DEF_CMD_PROTOCOL_MANUAL_EXAMINATION         0x0293
#define DEF_CMD_PROTOCOL_MANUAL_CLEAR_DTC           0x0a04
#define DEF_CMD_PROTOCOL_MANUAL_DATASTREAM_START    0x0287
#define DEF_CMD_PROTOCOL_MANUAL_DATASTREAM_STOP     0x0288
#define DEF_CMD_PROTOCOL_MANUAL_LCOK                0X01 
#define DEF_CMD_PROTOCOL_MANUAL_UNLCOK              0X00
#define DEF_CMD_PROTOCOL_MANUAL_SET_ODO             0x0132
#define DEF_CMD_PROTOCOL_MANUAL_READ_IO             0x0A06   
#define DEF_CMD_TEST_SUCESS                         0X00
#define DEF_CMD_TEST_FAILURE                        0X02
#define DEF_CMD_PROTOCOL_MANUAL_CONTROL_IO          0X0A01      //HY_2016_8_24 I/O控制预留接口
/*
********************************************************************************
                              描述:所有任务数据类型声明
********************************************************************************
*/

typedef struct TYPEDEF_CACHE_SYS_PAR
{
    uint8 u8SysLibChoice;                                                  //当前成功的系统,为避免重复扫描做好准备
    uint8 u8SysChoice;                                                     //当前成功的系统,为避免重复扫描做好准备
} TYPEDEF_CACHE_SYS_PAR;
typedef struct TYPEDEF_CACHE_SYSDEF_DATASTREAM_START
{
    bool  bCacheSysFinish;
    uint8 u8CacheCarType;                                                  //当前车型更改
    uint8 u8CacheSysTotal;                                                 //系统总数
    TYPEDEF_CACHE_SYS_PAR  SysPar[DEF_SUPPORT_SYS_MAX];                    //最多支持20个系统
} TYPEDEF_CACHE_SYS;

//这里重新规划一遍
//u8ReadDataStream;
//数据流开启和结束标志位
//00:开启 01 关闭  FF：表示手机端主动发起结束 FE表示程序判断到车速
//FD表示发送十分钟结束
//FC 无法获取OBD总线数据 FB:数据流选择项超过103
//u8ClearDtc00表示清码成功   04表示总线通信失败，其它待定  02 车速不允许
//03 不支持清码
//u8FirstTimeScan 00:首次体检 01:手动体检 结束之后

typedef struct TYPEDEF_FUN_FLAG
{
    bool  bRequestStatus;                                                  //手机端查询当前状态,主要用于响应手机端
    uint8 u8FirstTimeScan;                                                 //体检状态报告
    uint8 u8ReadDataStream;                                                //数据流开启和结束标志位
    uint8 u8ClearDtc;                                                      //清除故障码成功或者失败的标志位
    uint8 u8PhoneFunctionChoice;                                           //当前手机端选择的功能默认选择的是开机检测
    uint8 u8CurCarType;                                                    //当前车型更改
	  uint8 u8VehicleState;                                                  //点火或者熄火逻辑中的车辆状态	  
    uint8 u8DefaultCarType;                                                //默认车型车型更改
} TYPEDEF_FUN_FLAG;

/*
********************************************************************************
                              描述:MOBILE_AND_OBD_DEVICE_FUN变量声明
********************************************************************************
*/

ZXT_OBD_TASK_FUN_PROCESS_VAR_EXT TYPEDEF_FRAME_STATUS g_stCarScanSuccess;

/*
********************************************************************************
                              描述:调度任务变量声明
********************************************************************************
*/
ZXT_OBD_TASK_SCHEDULE_VAR_EXT TYPEDEF_FUN_FLAG g_stPhoneCommandShare;
/*
********************************************************************************
                              描述:传输层变量声明
********************************************************************************
*/


ZXT_OBD_TRANSFERLAYER_PROCESS_VAR_EXT  OBDMbox_TypeDef g_stObdData;
ZXT_OBD_TRANSFERLAYER_PROCESS_VAR_EXT  OBDMbox_TypeDef g_stObdData0;
ZXT_OBD_TRANSFERLAYER_PROCESS_VAR_EXT  OBDMbox_TypeDef *g_pstObdCmd;

/*
********************************************************************************
                              obd部分任务栈变量声明
********************************************************************************
*/

ZXT_OBD_TASK_CREAT_VAR_EXT      OS_STK     STK_OBD_TASK_FUN_PROCESS[DEF_OBD_TASK_FUN_SIZE];
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_STK     STK_OBD_DEVICE_AND_ECU[DEF_OBD_TASK_ECU_SIZE];
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_STK     STK_OBD_TASK_SCHEDULE[DEF_OBD_TASK_SCHEDULE_SIZE];
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_STK     STK_OBD_TASK_TRANSFERLAYER[DEF_OBD_TASK_TRANSFERLAYPER_SIZE];
/*
********************************************************************************
                              obd部分信号量声明
********************************************************************************
*/                                
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_EVENT   *g_pstScheduleToFunAnalyzer;
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_EVENT   *g_pstFunAnalyzerToSchedule;
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_EVENT   *g_pstPerformerToEcu;
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_EVENT   *g_pstEcuToPerformer;
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_EVENT   *g_pstAnalyzerToPerformer;
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_EVENT   *g_pstPerformerToAnalyzer;
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_EVENT   *g_pstTransferLayerToSchedule;
/*
********************************************************************************
                              OBD函数创建
********************************************************************************
*/
ZXT_OBD_TASK_CREAT_FUN_EXT void fun_obd_task_create( void );
/*
********************************************************************************
                              描述:MOBILE_AND_OBD_DEVICE_FUN函数声明
********************************************************************************
*/

ZXT_OBD_TASK_FUN_PROCESS_FUN_EXT void fun_scan_car_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus );
ZXT_OBD_TASK_FUN_PROCESS_FUN_EXT void *pfun_scan_sys_remem_fun( TYPEDEF_All_SYS_CFG *pstPersonObdChoice, TYPEDEF_FRAME_STATUS * pstFunStatus );
ZXT_OBD_TASK_FUN_PROCESS_FUN_EXT void *pfun_scan_sys_noremem_fun( TYPEDEF_All_SYS_CFG *pstPersonObdChoice, TYPEDEF_FRAME_STATUS * pstFunStatus );
ZXT_OBD_TASK_FUN_PROCESS_FUN_EXT void fun_scan_fun_fun( TYPEDEF_SYS_CFG *pstsysChoice, TYPEDEF_FRAME_STATUS * pstFunStatus );
ZXT_OBD_TASK_FUN_PROCESS_FUN_EXT void fun_scan_vehmatch_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus, bool bvehmatch );
ZXT_OBD_TASK_FUN_PROCESS_FUN_EXT void fun_scan_cdtc_fun( TYPEDEF_FRAME_STATUS * pstFunStatus );
ZXT_OBD_TASK_FUN_PROCESS_FUN_EXT void fun_obd_task_fun_process( void *pArg );
/*
********************************************************************************
                              描述:调度任务函数声明
********************************************************************************
*/
ZXT_OBD_TASK_SCHEDULE_FUN_EXT void  fun_obd_task_schedule( void *pArg );
//ZXT_OBD_TASK_SCHEDULE_FUN_EXT uint8 u8fun_odo_Calibration( uint32 u32OdoInput );
ZXT_OBD_TASK_SCHEDULE_FUN_EXT void  pack_to_hal_headpack_head(uint16 u16DataLen,OBD_CMDCODE_TYPE stCmdCode,uint16 u16RetCode,
                                                              uint16 u16CmdId,OBDMbox_TypeDef   *pstObdDataBuf );
ZXT_OBD_TASK_SCHEDULE_FUN_EXT uint8 *ppack_to_hal_pack_dword(uint32 u32PackTime,uint8 *pu8DataBuf);
ZXT_OBD_TASK_SCHEDULE_FUN_EXT uint8 *ppack_to_hal_pack_word (uint16 u16CmdType,uint8 *pu8DataBuf);
ZXT_OBD_TASK_SCHEDULE_FUN_EXT uint8 *ppack_to_hal_pack_data(uint8 *pu8SourceData,uint8 *pu8ObjData,uint16 u16Len);
ZXT_OBD_TASK_SCHEDULE_FUN_EXT void   fun_post_hal_result( void );
/*
********************************************************************************
                              描述:OBD设备和ECU通讯任务
********************************************************************************
*/
ZXT_OBD_TASK_ECU_PROCESS_FUN_EXT void fun_obd_task_ecu_process( void *pArg );
ZXT_OBD_TASK_ECU_PROCESS_FUN_EXT void fun_scan_poweroff_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus );
ZXT_OBD_TASK_ECU_PROCESS_FUN_EXT void *fun_scan_poweroff_car_fun( TYPEDEF_All_SYS_CFG *pstPersonObdChoice, TYPEDEF_FRAME_STATUS * pstFunStatus );


/*
********************************************************************************
                              描述:OBD设备和ECU通讯任务
********************************************************************************
*/
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT void fun_obd_task_transferlayer_process( void *pArg );
/*
********************************************************************************
                              描述:OBD设备和嵌入式通讯任务
********************************************************************************
*/
typedef struct  tag_OBDQueData_Def
{
    OBD_CMDCODE_TYPE    cmdCode;                  // 命令代码(见枚举定义)
    vu8     dataLock;                                         // 数据锁(发送方置位,接收方清空)
    vu16    sn;                                                   // 控制流水号(非协议流水号)
    vu16    proSn;                                            // 协议流水号
    vu16    proCmdId;                                         // 协议命令ID
    vu16    proParaLen;                                   // 参数长度
    vu8     proParaBuf[DEF_CMDBUF_SIZE];        // 参数区
    vu8     retCode;

} OBDQueData_TypeDef;
typedef struct  tag_OBDQue_Def
{
    vu8     Step;                       //  等待阶段变量
    vu32    WaitCounter;        //  等待计时器
    OBDQueData_TypeDef  data;   //  等待临时缓冲数据结构

} OBDQue_Typedef;
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT uint8_t Communication_task_lock;// 通讯任务锁 ，通讯信号投递前加锁，通讯完解锁
#define DEF_OBDQUE_NUM                          (8)                 // OBD交互结构数量
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT   OBDQue_Typedef              sa_obdQue[DEF_OBDQUE_NUM];                  // 服务器与OBD交互使用(如手动体检，实时数据流等)
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT s8                                        HAL_OBD_SaveCMD                                 (OBDMbox_TypeDef *obdmbox);//存储嵌入式投递过来的信号
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT s8                                        Check_Communication_Task_IDLE       (void);//判断通讯任务是否空闲
#endif




