/*
********************************************************************************
File name  : ZXT_OBD_lib.h
Description: ���ļ���Ҫ���������ϵͳ�����һЩ������������������,���Բ�������
             ��������������
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ������������������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#ifndef  __ZXT_OBD_STD_CMD_H__
#define  __ZXT_OBD_STD_CMD_H__

#ifdef    ZXT_OBD_STD_VAR
#define   ZXT_OBD_STD_VAR_EXT
#else
#define   ZXT_OBD_STD_VAR_EXT  extern
#endif

/*
********************************************************************************
                                ����:obd��׼��������
********************************************************************************
*/
//XXϵͳ�����ṹ
//xxϵͳ��������ֵ���ñ�
//xxϵͳ������
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode0Cmdtab0[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode1Cmdtab0[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab0[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab1[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab2[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab3[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab4[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_TAB                g_stObd0Mode2Cmdtab5[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode0Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode1Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode2Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode3Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode4Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode5Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode6Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode7Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_CMD_INDEX              g_stCmdObd0Mode8Index;
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_DATASTREAM_GET_DATA_LIST g_stObd0mode0DataStream[];
ZXT_OBD_STD_VAR_EXT  const TYPEDEF_DATASTREAM_GET_DATA_LIST g_stObd0mode1DataStream[];
#endif
