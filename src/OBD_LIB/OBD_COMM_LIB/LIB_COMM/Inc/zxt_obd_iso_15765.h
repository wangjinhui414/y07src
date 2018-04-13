/*******************************************************************************
* �� ��   :Zxt_iso_15765.h
* �� ��   :Mitch mouse
* ˵ ��   :��ͷ�ļ���Ҫ��ISO15765�������������������غ�������
* ��дʱ��:
* �� ��   :1.0
* �޸�����:��
********************************************************************************
*/

#ifndef  __ZXT_OBD_ISO_15765_H__
#define  __ZXT_OBD_ISO_15765_H__

#include "../LIB/Inc/zxt_obd_lib.h"

#ifdef    ISO_15765_VAR
#define   ISO_15765_VAR_EXT
#else
#define   ISO_15765_VAR_EXT  extern
#endif

#ifdef    ISO_15765_FUN
#define   ISO_15765_FUN_EXT
#else
#define   ISO_15765_FUN_EXT  extern
#endif
/*
********************************************************************************
                              ����:CAN���������ñ�
********************************************************************************
*/
#define DEF_CAN500K  0x00070206                                            
#define DEF_CAN250K  0x000D0208                                            
#define DEF_CAN125K  0x000D0216                                            
//#define DEF_CAN833K  0x000a0510                                            
//#define DEF_CAN50K   0x00090420                                            

/*
********************************************************************************
                              ����:iso-15765��������
********************************************************************************
*/
ISO_15765_VAR_EXT TYPEDEF_CANMSG g_stCAN115765ReceiveVar; //CAN1�жϱ���
ISO_15765_VAR_EXT TYPEDEF_CANMSG g_stCAN115765ReceiveVar1;//CAN2�жϱ���
ISO_15765_VAR_EXT CanTxMsg g_stTxMessage;//CAN�����
ISO_15765_VAR_EXT CanTxMsg g_stTxMessage1;//CAN�����

/*
********************************************************************************
                              ����:iso-15765��������
********************************************************************************
*/

ISO_15765_FUN_EXT void *pfun_iso15765_send_command_group( void *pCmdId, void *pArg1 );
ISO_15765_FUN_EXT void fun_iso_15765_send_times( uint8 u8CmdId, void **pCommStatus );
ISO_15765_FUN_EXT void fun_iso_15765_send_cmd( uint8 u8CmdId );
ISO_15765_FUN_EXT void *pfun_iso_15765_receive( void *pCacheOffset, void *pArg1 );
ISO_15765_FUN_EXT void *pfun_iso_15765_check( void*pArg0, void*pArg1 );
ISO_15765_FUN_EXT void *pfun_iso_15765_check1( void*pArg0, void*pArg1 );

#endif








