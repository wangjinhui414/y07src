/*
********************************************************************************
File name  : Zxt_car_cfg.h
Description:
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#ifndef  __ZXT_OBD_STD_CAR_CFG_H__
#define  __ZXT_OBD_STD_CAR_CFG_H__

#ifdef    OBD_STD_CFG_VAR
#define   OBD_STD_CFG_VAR_EXT
#else
#define   OBD_STD_CFG_VAR_EXT  extern //��������
#endif
/*
********************************************************************************
                              ����:�������ñ��������
���峵��

********************************************************************************
*/
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdRdtc[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdDataStream[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdDataStream1[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdCdtc[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdOdoMeter[];
OBD_STD_CFG_VAR_EXT const TYPEDEF_SYS_CFG g_stObdVin[];
#endif
