/*******************************************************************************
File name  :zxt_obd_include.h
Description:��ͷ�ļ���Ҫ���漰���ײ���ص��������ͣ�������������������
History    :Ver0.1
�޸�����   :�޸���:�޸�����:������
2012-11-19   С���� ��Ҫ������ϵͳͷ�ļ�����
�޸�����   :�޸���:�޸�����:������
*******************************************************************************/
#ifndef __ZXT_OBD_INCLUDE_H_
#define __ZXT_OBD_INCLUDE_H_
//������������
typedef unsigned char                  BOOLEAN;
typedef unsigned char                  uint8;
typedef signed   char                  int8;
typedef unsigned short                 uint16;
typedef signed   short                 int16;
typedef unsigned int                   uint32;
typedef signed   int                   int32;
typedef float                          fp32;
typedef double                         fp64;

#include <stdio.h> //�������룯�������
#include <stdarg.h>
#include <stdlib.h> //������������ڴ���亯��
#include <string.h> //�ַ�������
#include "stm32f10x.h"
#include "ucos_ii.h"
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"

#include "zxt_obd_bsp.h"
#include "zxt_obd_app_tydef.h"
#include "zxt_obd_fun_lib.h"
#include "zxt_obd_protocol_lib.h"
#include "zxt_obd_all_command_lib.h"
#include "../DEV_SRC/dev_include_h.h"
#include "../OBD_LIB/OBD_PUBLIC/Task/Inc/zxt_obd_task_lib.h"

/*
********************************************************************************
                              ������OBD����ϵͳ���ò�������
********************************************************************************
*/
#define  Y06D
#define  DEF_OBD_VERSION               "13.10"                             //��2�������(��������ΪOBD�ܹ�+С������Ϊ���Գ��汾��)

//#define  Y07D
//#define  DEF_OBD_VERSION               "13.10"                             //��2�������(��������ΪOBD�ܹ�+С������Ϊ���Գ��汾��)







 #endif
/************************************�ļ�����**********************************/



