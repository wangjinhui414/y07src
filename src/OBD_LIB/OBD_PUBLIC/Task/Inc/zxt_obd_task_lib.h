/*******************************************************************************
* �� ��   :ZXT_task_lib.h
* �� ��   :Mitch mouse
* ˵ ��   :���ļ���Ҫ�ǽ����е������ϱ�����������,���Բ����������������κ�������
*          �޹صı���
* ��дʱ��:
* �� ��   :1.0
* �޸�����:��
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
                                �������ȼ��궨��
                                ������ȼ��Ѿ������ã��мǲ�׼ʹ��obd(4-8�������ȼ�)
********************************************************************************
*/
//13-18����OBD
#define  DEF_OBD_TASK_TRANSFERLAYPER_PRIO           25                     //��Ӳ�����Ž����������
#define  DEF_OBD_TASK_SCHEDULE_PRIO                 14                     //����������ȼ�
#define  DEF_OBD_TASK_FUN_PRIO                      15                     //�ֻ��˺�OBD�豸֮��ͨѶ
#define  DEF_OBD_TASK_ECU_PRIO                      16                     //OBD�豸��ECU֮��ͨѶ��������������

/*
********************************************************************************
*                                ����ջ��С����
********************************************************************************
*/
#define  DEF_OBD_TASK_FUN_SIZE                      256
#define  DEF_OBD_TASK_ECU_SIZE                      512
#define  DEF_OBD_TASK_SCHEDULE_SIZE                 128
#define  DEF_OBD_TASK_TRANSFERLAYPER_SIZE           100

/*
********************************************************************************
                              ����:��Դ��С����
********************************************************************************
*/
#define  DEF_SUPPORT_SYS_MAX                         20
#define  DEF_CMD_BUFFER_SIZE                         100
#define  DEF_DATA_BUFFER_SIZE                        500

/*
********************************************************************************
                              ����:��������Э���������
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
#define DEF_CMD_PROTOCOL_MANUAL_CONTROL_IO          0X0A01      //HY_2016_8_24 I/O����Ԥ���ӿ�
/*
********************************************************************************
                              ����:��������������������
********************************************************************************
*/

typedef struct TYPEDEF_CACHE_SYS_PAR
{
    uint8 u8SysLibChoice;                                                  //��ǰ�ɹ���ϵͳ,Ϊ�����ظ�ɨ������׼��
    uint8 u8SysChoice;                                                     //��ǰ�ɹ���ϵͳ,Ϊ�����ظ�ɨ������׼��
} TYPEDEF_CACHE_SYS_PAR;
typedef struct TYPEDEF_CACHE_SYSDEF_DATASTREAM_START
{
    bool  bCacheSysFinish;
    uint8 u8CacheCarType;                                                  //��ǰ���͸���
    uint8 u8CacheSysTotal;                                                 //ϵͳ����
    TYPEDEF_CACHE_SYS_PAR  SysPar[DEF_SUPPORT_SYS_MAX];                    //���֧��20��ϵͳ
} TYPEDEF_CACHE_SYS;

//�������¹滮һ��
//u8ReadDataStream;
//�����������ͽ�����־λ
//00:���� 01 �ر�  FF����ʾ�ֻ�������������� FE��ʾ�����жϵ�����
//FD��ʾ����ʮ���ӽ���
//FC �޷���ȡOBD�������� FB:������ѡ�����103
//u8ClearDtc00��ʾ����ɹ�   04��ʾ����ͨ��ʧ�ܣ���������  02 ���ٲ�����
//03 ��֧������
//u8FirstTimeScan 00:�״���� 01:�ֶ���� ����֮��

typedef struct TYPEDEF_FUN_FLAG
{
    bool  bRequestStatus;                                                  //�ֻ��˲�ѯ��ǰ״̬,��Ҫ������Ӧ�ֻ���
    uint8 u8FirstTimeScan;                                                 //���״̬����
    uint8 u8ReadDataStream;                                                //�����������ͽ�����־λ
    uint8 u8ClearDtc;                                                      //���������ɹ�����ʧ�ܵı�־λ
    uint8 u8PhoneFunctionChoice;                                           //��ǰ�ֻ���ѡ��Ĺ���Ĭ��ѡ����ǿ������
    uint8 u8CurCarType;                                                    //��ǰ���͸���
	  uint8 u8VehicleState;                                                  //������Ϩ���߼��еĳ���״̬	  
    uint8 u8DefaultCarType;                                                //Ĭ�ϳ��ͳ��͸���
} TYPEDEF_FUN_FLAG;

/*
********************************************************************************
                              ����:MOBILE_AND_OBD_DEVICE_FUN��������
********************************************************************************
*/

ZXT_OBD_TASK_FUN_PROCESS_VAR_EXT TYPEDEF_FRAME_STATUS g_stCarScanSuccess;

/*
********************************************************************************
                              ����:���������������
********************************************************************************
*/
ZXT_OBD_TASK_SCHEDULE_VAR_EXT TYPEDEF_FUN_FLAG g_stPhoneCommandShare;
/*
********************************************************************************
                              ����:������������
********************************************************************************
*/


ZXT_OBD_TRANSFERLAYER_PROCESS_VAR_EXT  OBDMbox_TypeDef g_stObdData;
ZXT_OBD_TRANSFERLAYER_PROCESS_VAR_EXT  OBDMbox_TypeDef g_stObdData0;
ZXT_OBD_TRANSFERLAYER_PROCESS_VAR_EXT  OBDMbox_TypeDef *g_pstObdCmd;

/*
********************************************************************************
                              obd��������ջ��������
********************************************************************************
*/

ZXT_OBD_TASK_CREAT_VAR_EXT      OS_STK     STK_OBD_TASK_FUN_PROCESS[DEF_OBD_TASK_FUN_SIZE];
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_STK     STK_OBD_DEVICE_AND_ECU[DEF_OBD_TASK_ECU_SIZE];
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_STK     STK_OBD_TASK_SCHEDULE[DEF_OBD_TASK_SCHEDULE_SIZE];
ZXT_OBD_TASK_CREAT_VAR_EXT      OS_STK     STK_OBD_TASK_TRANSFERLAYER[DEF_OBD_TASK_TRANSFERLAYPER_SIZE];
/*
********************************************************************************
                              obd�����ź�������
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
                              OBD��������
********************************************************************************
*/
ZXT_OBD_TASK_CREAT_FUN_EXT void fun_obd_task_create( void );
/*
********************************************************************************
                              ����:MOBILE_AND_OBD_DEVICE_FUN��������
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
                              ����:��������������
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
                              ����:OBD�豸��ECUͨѶ����
********************************************************************************
*/
ZXT_OBD_TASK_ECU_PROCESS_FUN_EXT void fun_obd_task_ecu_process( void *pArg );
ZXT_OBD_TASK_ECU_PROCESS_FUN_EXT void fun_scan_poweroff_fun( TYPEDEF_All_CAR_CFG *pstCarChoice, TYPEDEF_FRAME_STATUS * pstFunStatus );
ZXT_OBD_TASK_ECU_PROCESS_FUN_EXT void *fun_scan_poweroff_car_fun( TYPEDEF_All_SYS_CFG *pstPersonObdChoice, TYPEDEF_FRAME_STATUS * pstFunStatus );


/*
********************************************************************************
                              ����:OBD�豸��ECUͨѶ����
********************************************************************************
*/
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT void fun_obd_task_transferlayer_process( void *pArg );
/*
********************************************************************************
                              ����:OBD�豸��Ƕ��ʽͨѶ����
********************************************************************************
*/
typedef struct  tag_OBDQueData_Def
{
    OBD_CMDCODE_TYPE    cmdCode;                  // �������(��ö�ٶ���)
    vu8     dataLock;                                         // ������(���ͷ���λ,���շ����)
    vu16    sn;                                                   // ������ˮ��(��Э����ˮ��)
    vu16    proSn;                                            // Э����ˮ��
    vu16    proCmdId;                                         // Э������ID
    vu16    proParaLen;                                   // ��������
    vu8     proParaBuf[DEF_CMDBUF_SIZE];        // ������
    vu8     retCode;

} OBDQueData_TypeDef;
typedef struct  tag_OBDQue_Def
{
    vu8     Step;                       //  �ȴ��׶α���
    vu32    WaitCounter;        //  �ȴ���ʱ��
    OBDQueData_TypeDef  data;   //  �ȴ���ʱ�������ݽṹ

} OBDQue_Typedef;
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT uint8_t Communication_task_lock;// ͨѶ������ ��ͨѶ�ź�Ͷ��ǰ������ͨѶ�����
#define DEF_OBDQUE_NUM                          (8)                 // OBD�����ṹ����
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT   OBDQue_Typedef              sa_obdQue[DEF_OBDQUE_NUM];                  // ��������OBD����ʹ��(���ֶ���죬ʵʱ��������)
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT s8                                        HAL_OBD_SaveCMD                                 (OBDMbox_TypeDef *obdmbox);//�洢Ƕ��ʽͶ�ݹ������ź�
ZXT_OBD_TRANSFERLAYER_PROCESS_FUN_EXT s8                                        Check_Communication_Task_IDLE       (void);//�ж�ͨѶ�����Ƿ����
#endif




