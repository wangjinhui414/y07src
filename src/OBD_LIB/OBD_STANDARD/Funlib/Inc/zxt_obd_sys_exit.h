/*
********************************************************************************
File name  : Zxt_sys_exit.h
Description: ��������Ҫ������ϵͳ�˳�����ļ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-11-20   С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#ifndef  __ZXT_OBD_SYS_EXIT_H__
#define  __ZXT_OBD_SYS_EXIT_H__

#ifdef    SYS_EXIT_VAR
#define   SYS_EXIT_VAR_EXT
#else
#define   SYS_EXIT_VAR_EXT  extern
#endif

#ifdef    SYS_EXIT_FUN
#define   SYS_EXIT_FUN_EXT
#else
#define   SYS_EXIT_FUN_EXT  extern
#endif















/*
********************************************************************************
                              ����:ϵͳ�˳���������
********************************************************************************
*/
SYS_EXIT_FUN_EXT void *pfun_sys_exit_fun( void *pstsysChoice, void *pu8FunctionParameter );

#endif






