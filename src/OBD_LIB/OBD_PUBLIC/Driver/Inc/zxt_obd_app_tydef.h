/*******************************************************************************
File name  :zxt_obd_datastruct_tydef.h
Description:���ļ���Ҫ���漰���ײ�����������Ͷ���
History    :Ver0.1
�޸�����   :�޸���:�޸�����:������
2014/11/19 С���� ���½����ײ����ݽӿ�
*******************************************************************************/
/********************************************************************************
                              ����:�����������¶���
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
                              ����:Ӧ�ò�������ݿ��С����
********************************************************************************
*/
#define  DEF_MSGBUF_SIZE            4
#define  DEF_RDTCBUF_SIZE           600
#define  DEF_RDATASTREAMBUF_SIZE    11
#define  DEF_ODOMETERBUF_SIZE       4
#define  DEF_RVIN_SIZE              25
/*
********************************************************************************
                              ����:�ײ�ϵͳ�������Ͷ���-��Ϣ�ṹ
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
                              ����:���Ͷ���
********************************************************************************
*/
typedef struct TYPEDEF_OBD_CACHE_CAR
{    
    uint8 u8SysLibChoice;                
    uint8 u8SysChoice;                   
} TYPEDEF_OBD_CACHE_CAR;

typedef struct TYPEDEF_OBD_PAR
{
  uint16  savedMark;                                                       //������ʼ������־
  uint16  sum;                                                             //�����ۼӺͱ�־


  uint16  u16OdoPAR0;
  uint32  u32OdoMeter:24;                                                  //obd����
  
  bool    bCacheSyeFinish;                                                 //�ж��Ƿ񻺴��ϵͳ
	uint8   u8CacheSysTotal;                                                 //ϵͳ����
  uint8   u8CacheCarType;                                                  //��ǰ���͸���  
  TYPEDEF_OBD_CACHE_CAR  SysPar[20];                                       //���֧��20��ϵͳ	
}TYPEDEF_OBD_PAR;

typedef struct TYPEDEF_FUNCTION_CFG
{
    const uint8 u8FunLibChoice;                                            //ѡ�������һ�����е����ݺ���
    const uint8 u8FunChoice;                                               //ѡ�����ĺ���
    const uint8 *pFunctionParameter;                                       //�ú�����Ҫ�Ĳ���
}TYPEDEF_FUNCTION_CFG;

typedef struct TYPEDEF_SYS_CFG
{
    const uint8 u8FunTotal;                                                //��ǰ���͹�������
    const uint8 u8SysLibChoice;                                            //����ϵͳ
    const uint8 u8SysChoice;                                               //����ģ��
    const uint16 u16SysDelay;                                              //ϵͳ��ϵͳ֮����ʱ
    const TYPEDEF_FUNCTION_CFG *pstFunction;
} TYPEDEF_SYS_CFG;


typedef struct TYPEDEF_All_SYS_CFG
{
    const uint8    u8SysTotal;                                             //��ǰ���ܰ�����ϵͳ����
    const TYPEDEF_SYS_CFG *pstSysCfg;                                      //ָ������ϵͳ
} TYPEDEF_All_SYS_CFG;
typedef struct
{
    const uint16   u16CarDelay;                                            //�����빦��֮�����ʱ
    TYPEDEF_All_SYS_CFG stPersonAndObdCfg[2];
} TYPEDEF_All_CAR_CFG;                                                     //���幦������

///*
//********************************************************************************
//                              ����:��Դ����
//********************************************************************************
//*/
////����ϵͳ��������ֵ,�Ժ��ٵ���������һ��
//typedef struct TYPEDEF_CMD_INDEX
//{
//  const uint8 *pCmdEnter;                                                  //��������
//  const uint8 *pCmdIdle;                                                   //��������
//  const uint8 *pCmdRDtc;                                                   //����ǰ������
//  const uint8 *pCmdClearDtc;                                               //��������
//  const uint8 *pCmdGetOdoMeter;                                            //��ȡϵͳ�������
//  const uint8 *pCmdSysExit;                                                //ϵͳ�˳�����
//	const uint8 *pCmdGetVin;                                                 //��ȡ����vin��
//} TYPEDEF_CMD_INDEX;                                                       //��������ֵ��Ա

//typedef struct TYPEDEF_CMD_TAB
//{
//    const uint8 u8BufferId;                                                 //����洢�Ļ�����
//    const uint8 u8CmdLen;                                                   //�����
//    const uint8 u8CmdIdOffset;                                              //u8CmdIdOffset
//    const uint8 u8FilterIdOffset;                                           //�˲�������
//    const uint8 u8FilterGroup;                                              //14���˲�������һ��,��Ҫ�����CAN��
//    const uint8 *pu8Command;                                                //ָ����������
//} TYPEDEF_CMD_TAB;

//typedef  struct TYPEDEF_MODE_CELL
//{
//    const TYPEDEF_CMD_TAB    *pCmdTabChoice;                               //ָ����������еķ�����ϵͳ
//    const TYPEDEF_CMD_INDEX  *pCmdIndexTabChoice;                          //����ϵͳ��������ֵ��
//    const uint8            u8ProtocolLibOffset[5];                         //������õ��Ƿ��ͺͽ��պ�����ѡ��
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

//�ײ���ر���
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
