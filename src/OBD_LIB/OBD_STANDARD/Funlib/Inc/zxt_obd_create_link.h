/*
********************************************************************************
File name  : ZXT_create_link.h
Description: ��ͷ�ļ���Ҫ��Creatlink.c����ر�������������,����������������޹���Ϣ
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   �������Ӻ����ͱ�������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#ifndef  __ZXT_OBD_CREATE_LINK_H__
#define  __ZXT_OBD_CREATE_LINK_H__


#ifdef    CREATE_LINK_VAR
#define   CREATE_LINK_VAR_EXT
#else
#define   CREATE_LINK_VAR_EXT  extern
#endif

#ifdef    CREATE_LINK_FUN
#define   CREATE_LINK_FUN_EXT
#else
#define   CREATE_LINK_FUN_EXT  extern
#endif

/*
********************************************************************************
                              ����:14230/9141������������
********************************************************************************
*/
typedef struct TYPEDEF_ISO14230_ACTIVE_PAR
{
    uint8   u8LowTime;                                                     //�͵�ƽʱ��
    uint8   u8HighTime;                                                    //�ߵ�ƽʱ��
    uint8   u8RetransTime;                                                 //�ط�����
    uint16  u16BaudRate;                                                   //������
} TYPEDEF_ISO14230_ACTIVE_PAR;

typedef struct TYPEDEF_ISO9141_ACTIVE_PAR
{
    uint8   u8AddCode ;                                                    //Э���ַ
    uint8   u8AddCodeTime;                                                 //��ַ�뷢��ʱ��
    uint8   u8RetransTime;                                                 //�ط�����
    uint16  u16BaudRate;                                                   //������
    uint16  u16ProtocolReversed[10];                                       //����
} TYPEDEF_ISO9141_ACTIVE_PAR;

//typedef struct TYPEDEF_ISO14230_COMMUNICATION_PAR
//{
//    uint8   u8ByteTime;                                                    //ͨѶ�ֽ�ʱ��
//    uint16  u16FrameTime;                                                  //ͨѶ֡ʱ��
//    uint8   u8RetransTime;                                                 //�ط�����
//    uint16  u16TimeOut;                                                    //��ʱʱ��
//} TYPEDEF_ISO14230_COMMUNICATION_PAR;
/*
********************************************************************************
                              ����:IS0-15765Э��������������
********************************************************************************
*/

//typedef struct TYPEDEF_ISO15765_ACTIVE_PAR
//{
//    uint32 u32BaudRate;                                                    //������
//    uint8  u8FrameTime;                                                    //CANͨѶ֡ʱ��
//    uint16 u16TimeOut;                                                     //CANͨѶ��ʱʱ��
//    uint8  u8RetransTime;                                                  //�ط�����
//} TYPEDEF_ISO15765_ACTIVE_PAR;


/*
********************************************************************************
                              ����:������Ҫʹ�õı���������
********************************************************************************
*/
CREATE_LINK_VAR_EXT const TYPEDEF_ISO14230_ACTIVE_PAR g_stIso14230ActiveLib[];  //�ߵ͵�ƽ�����������
CREATE_LINK_VAR_EXT const TYPEDEF_ISO15765_ACTIVE_PAR g_stIso15765ActiveLib[];  //ISO15765��׼��
CREATE_LINK_VAR_EXT const TYPEDEF_ISO9141_ACTIVE_PAR  g_stIso9141ActiveLib[];   //��ַ�뼤�����
CREATE_LINK_VAR_EXT const TYPEDEF_ISO14230_COMMUNICATION_PAR  g_stIsoKlineCommLib[]; //K��ͨѶ����,��Ҫ��Ӧ�Է��ͺͽ��ղ��ֲ���

/*
********************************************************************************
                              ����:�������Ӻ�������
********************************************************************************
*/
//Э��ɨ��
//�ߵ͵�ƽ�����25ms
//�������ݸ��ֻ�
//����Ч���
CREATE_LINK_FUN_EXT void *pfun_active_low_high_xxms( void *pArg0 , void *pu8FunctionParameter );
CREATE_LINK_FUN_EXT void publicfun_upload_data_to_phone( void  *pData );
CREATE_LINK_FUN_EXT uint8 u8publicfun_block_character_xorc( uint16 DataLen, uint8 *pData );
//CREATE_LINK_FUN_EXT void  publicfun_block_character_sc( uint16 DataLen, uint8 *pData );
CREATE_LINK_FUN_EXT void publicfun_upload_data_frame_heard( uint8 u8CmdType, uint16 u16CmdLen, uint8 *pCache );
CREATE_LINK_FUN_EXT void *pfun_active_can_bus( void *pArg0 , void *pu8FunctionParameter );
CREATE_LINK_FUN_EXT void *ppublicfun_send_command_index( uint8 *pCmdIndex, void *pArg1 );
CREATE_LINK_FUN_EXT void *pfun_active_5baud_negate_last_byte_back_addr( void *pArg0 , void *pu8FunctionParameter );
CREATE_LINK_FUN_EXT void *pfun_active_5baud_kwp1281( void *pArg0 , void *pu8FunctionParameter );
CREATE_LINK_FUN_EXT void fun_send_addcode( uint8 u8AddTime, uint8 u8AddCode );
CREATE_LINK_FUN_EXT bool bfun_active_5baud_init( uint8 *pu8LastByte, uint8 u8FunctionParameter );
CREATE_LINK_FUN_EXT bool bfun_cmd_can2_test(void);
#endif


