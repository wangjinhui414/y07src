/*
********************************************************************************
File name  : ZXT_Protocol_cfg.h
Description: ���ļ���Ҫ�Ƕ�Э��ջ�����ø�ͷ�ļ�ֻ������ú궨��
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ������Ҫ�Ƕ�����Э���������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#ifndef  __ZXT_OBD_PROTOCOL_CFG_H__
#define  __ZXT_OBD_PROTOCOL_CFG_H__
#ifdef    PROTOCOL_LIB         
#define   PROTOCOL_LIB_EXT
#else
#define   PROTOCOL_LIB_EXT  extern
#endif
#endif
/*
********************************************************************************
                              ����:IS0-14230Э�����ñ�
********************************************************************************
*/
#define  IS0_14230_SEND_GROUP_EN          1                                //IS014230���������ʹ��
#define  IS0_14230_SEND_EN                1                                //IS014230����ʹ��
#define  ISO_14230_80_CHECK_EN            1                                //IS014230Э��ջʹ��
#define  IS0_14230_RECEIVE_EN             1                                //IS014230����ʹ��
#define  ISO_14230_CMD_LOAD_EN            1                                //IS014230����װ��
#define  ISO14230_NEGATIVE_RESPONSE_EN    1                                //IS014230������Ӧ����

/*
********************************************************************************
                              ����:ISO_15765Э�����ñ�
********************************************************************************
*/
#define  ISO_15765_SEND_GROUP_EN       1                                   //iso15765Э��ͨѶ���ñ�
#define  IS0_15765_SEND_TIMES_EN       1                                   //����ʹ��
#define  ISO_15765_CMD_LOAD_EN         1                                   //����װ��ʹ��
#define  ISO_15765_CHECK_EN            1                                   //IS015765Э��ջʹ��
#define  ISO_15765_CHECK_EN1           1                                   //IS015765Э��ջʹ��
#define  IS0_15765_RECEIVE_EN          1                                   //IS014230����ʹ��


/*
********************************************************************************
                              ����:ISO_9141Э�����ñ�
********************************************************************************
*/
#define  IS0_9141_SEND_GROUP_EN        1                                   //ISO9141ͨѶ����ʹ��
#define  IS0_9141_SEND_EN              1                                   //ISO9141ͨѶ����ʹ��
#define  ISO_9141_68_CHECK_EN          1                                   //ISO9141У��ʹ��

/*
********************************************************************************
                              ����:ͨ�ó�Э�����ñ�
********************************************************************************
*/
#define  GM_CHECK_EN                   1                                   //ͨ�ó�У��ʹ��
#define  GM_SEND_TIMES_EN              1                                   //����ʹ��
#define  GM_SEND_GROUP_EN              1                                   //���������ʹ��

/*
********************************************************************************
                              ����:IS0-1281Э�����ñ�
********************************************************************************
*/
#define  IS0_1281_SEND_GROUP_EN        1                                   //IS01281���������ʹ��
#define  IS0_1281_SEND_EN              1                                   //IS01281Э��ջʹ��
#define  ISO_1281_CMD_LOAD_EN          1                                   //IS01281����װ��
#define  ISO_1281_CHECK_EN             1                                   //IS01281�˲麯��
#define  IS0_1281_RECEIVE_EN           1                                   //IS01281����ʹ��

/*
********************************************************************************
                              ����:���ݿ��С
********************************************************************************
*/
//�ײ�������ݿ��С
#define DEF_RECEVIE_DATA_MAX           500

//Э����������Ͷ���
//typedef struct TYPEDEF_PROTOCOL_MSGBUF
//{
////    uint8  u8Len;/*Ϊ������550��80��������룬�����ޱ����޸� 2017-2-7 liu */
////  uint8  u8Buf[255];
//	//�޸�����
//	uint16  u16Len;
//    uint8  u8Buf[DEF_RECEVIE_DATA_MAX];
//} TYPEDEF_PROTOCOL_MSGBUF;

//typedef  enum {TIMEOUT, FRAMETIMEOUT, OK} TYPEDEF_FRAME_STATUS;
typedef void*( *TYPEDEF_pPROTOCOLLIB )( void*, void* );
typedef void*( *TYPEDEF_pPROTOCOLCHECKLIB )( void*, void* );
/*
********************************************************************************
                              ����:����ϵͳͨѶ��ر�־λ��������
;TIMEOUT����û����Ӧ��frametiemeout����֡��ʱ����������ͨѶ�ж�,ok����ͨѶ����
********************************************************************************
*/
//PROTOCOL_LIB_EXT             const TYPEDEF_FRAME_STATUS g_enumFrameFlgArr[];
PROTOCOL_LIB_EXT             const TYPEDEF_pPROTOCOLLIB g_stProtocolSendLib[];   //���ͺ���
PROTOCOL_LIB_EXT             const TYPEDEF_pPROTOCOLLIB g_stProtocolReceiveLib[];//���պ���
PROTOCOL_LIB_EXT             const TYPEDEF_pPROTOCOLLIB g_stProtocolCheckLlib[]; //Ч��Э������
PROTOCOL_LIB_EXT             const TYPEDEF_2WORD4BYTE g_unnu32FrameIdLib[];
PROTOCOL_LIB_EXT             const TYPEDEF_2WORD4BYTE g_unnu32FilterId [];
