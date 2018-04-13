/*******************************************************************************
* �� ��   :Zxt_iso_14230.h
* �� ��   :Mitch mouse
* ˵ ��   :��ͷ�ļ���Ҫ��ISO14230�������������������غ�������
* ��дʱ��:
* �� ��   :1.0
* �޸�����:��
********************************************************************************
*/

#ifndef  __ZXT_OBD_ISO_14230_H__
#define  __ZXT_OBD_ISO_14230_H__

#ifdef    ISO_14230_VAR
#define   ISO_14230_VAR_EXT
#else
#define   ISO_14230_VAR_EXT  extern
#endif

#ifdef    ISO_14230_FUN
#define   ISO_14230_FUN_EXT
#else
#define   ISO_14230_FUN_EXT  extern
#endif
/*
********************************************************************************
                              ����:iso-14230������������
********************************************************************************
*/

typedef struct TYPEDEF_ISO14230_RECEIVE
{
    uint8  u8DataOffset;                                                   //����3�������������ݷ���λ��
    uint8  u8ReceiveCounter;                                               //����3��������ס���ն��ٸ�����
    uint8  u8SentCounter;                                                  //kwp1281��������ס���Ͷ���������
    uint8  u8ReceivePack;                                                  //����K����Ҫ���ն�֡,�����������д���  
	  bool   bFrameError;                                                    //�����жϵ�ǰ���յ���֡�Ƿ���ȷ
	  
} TYPEDEF_ISO14230_RECEIVE;

/*
********************************************************************************
                              ����:����ϵͳ�������ݻ������������Ͷ���
********************************************************************************
*/

/*
********************************************************************************
                              ����:iso-14230��������
********************************************************************************
*/
ISO_14230_VAR_EXT   uint8                    g_u8SentDataToEcu[];          //�������ݸ�ECU
ISO_14230_VAR_EXT   TYPEDEF_PROTOCOL_MSGBUF  g_stDataFromEcu;              //�������ݻ�����
ISO_14230_VAR_EXT   TYPEDEF_PROTOCOL_MSGBUF  g_stu8CacheData[];            //���ݵĴ洢��
ISO_14230_VAR_EXT   TYPEDEF_ISO14230_RECEIVE g_stUsart3Iso14230ReceiveVar; //����3�жϱ���
/*
********************************************************************************
                              ����:iso-14230��������
********************************************************************************
*/

ISO_14230_FUN_EXT void fun_iso_14230_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime );
ISO_14230_FUN_EXT void*pfun_iso_14230_receive( void *pCacheOffset, void *pArg1 );
ISO_14230_FUN_EXT void fun_iso_14230_cmd_load( uint8 u8Cmdid, uint8 u8FrameLen , uint8 *pCmdBuf );
ISO_14230_FUN_EXT void*pfun_iso_14230_80_check( void*pArg0, void*pArg1 );
ISO_14230_FUN_EXT void*pfun_iso_14230_80_check1( void*pArg0, void*pArg1 );
ISO_14230_FUN_EXT void*pfun_iso_14230_OBD_check( void*pArg0, void*pArg1 );
ISO_14230_FUN_EXT void*pfun_iso14230_send_command_group( void *pCmdId, void *pArg1 );
ISO_14230_FUN_EXT void*pfun_iso14230_send_command_group1( void *pCmdId, void *pArg1 );
ISO_14230_FUN_EXT void fun_iso_14230_send_times( uint8 u8CmdId, uint8 u8FrameLen, void **pCommStatus );
ISO_14230_FUN_EXT void fun_iso_14230_send_times1( uint8 u8CmdId,uint8 u8FrameLen, uint8 u8CacheOffset,uint8 u8ReceivePack, void **pCommStatus );
ISO_14230_FUN_EXT bool bfun_iso_14230_negative_response( void );

#endif









