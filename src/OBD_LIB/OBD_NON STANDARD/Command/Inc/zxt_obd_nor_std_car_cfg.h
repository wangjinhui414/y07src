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
#ifndef  __ZXT_OBD_OBD_CAR_CFG_H__
#define  __ZXT_OBD_OBD_CAR_CFG_H__

#ifdef    OBD_NOR_STD_SYS_CFG_VAR
#define   OBD_NOR_STD_SYS_CFG_VAR_EXT
#else
#define   OBD_NOR_STD_SYS_CFG_VAR_EXT  extern //��������
#endif

#ifdef    OBD_NOR_STD_CAR_CFG_VAR
#define   OBD_NOR_STD_CAR_CFG_VAR_EXT
#else
#define   OBD_NOR_STD_CAR_CFG_VAR_EXT  extern //��������
#endif
/*
********************************************************************************
                              ����:������ش�С����

********************************************************************************
*/
#define DEF_MAX_FUN_SIZE               6                                   //Ŀǰ����ϵͳ�Ĺ������� 

/*
********************************************************************************
                              ����:�������ñ��������
���峵��

********************************************************************************
*/
OBD_NOR_STD_CAR_CFG_VAR_EXT TYPEDEF_All_CAR_CFG  g_stCarType[DEF_MAX_FUN_SIZE];   //��ǰ���ͱ�
OBD_NOR_STD_SYS_CFG_VAR_EXT const TYPEDEF_MODE_CELL   *g_pstSysLibTab[];         //ϵͳ��

#endif
