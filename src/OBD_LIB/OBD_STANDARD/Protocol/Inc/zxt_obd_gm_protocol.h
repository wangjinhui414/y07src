/*******************************************************************************
* �� ��   :Zxt_gm_protocol.h
* �� ��   :Mitch mouse
* ˵ ��   :��ͷ�ļ���Ҫ��ͨ��Э���������������������غ�������
* ��дʱ��:
* �� ��   :1.0
* �޸�����:��
********************************************************************************
*/

#ifndef  __ZXT_OBD_GM_PROTOCOL_H__
#define  __ZXT_OBD_GM_PROTOCOL_H__

#ifdef    GM_PROTOCOL_VAR
#define   GM_PROTOCOL_VAR_EXT
#else
#define   GM_PROTOCOL_VAR_EXT  extern
#endif

#ifdef    GM_PROTOCOL_FUN
#define   GM_PROTOCOL_FUN_EXT
#else
#define   GM_PROTOCOL_FUN_EXT  extern
#endif
/*
********************************************************************************
                              ����:ͨ�ó�������������
********************************************************************************
*/





/*
********************************************************************************
                              ����:iso-15765��������
********************************************************************************
*/

/*
********************************************************************************
                              ����:ͨ��Э�麯������
********************************************************************************
*/

GM_PROTOCOL_FUN_EXT void  fun_gm_send_times( uint8 u8CmdId, void **pCommStatus );
GM_PROTOCOL_FUN_EXT void *pfun_gm_check( void*pArg0, void*pArg1 );
GM_PROTOCOL_FUN_EXT void *pfun_gm_send_command_group( void *pCmdId, void *pArg1 );

#endif








