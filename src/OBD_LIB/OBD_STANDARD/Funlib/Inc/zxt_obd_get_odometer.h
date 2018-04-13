/*
********************************************************************************
File name  : Zxt_get_odometer.h
Description:���ļ���Ҫ�����л�ȡ����ļ���غ�����������
History    :
�޸�����   : �޸��� : �޸�����:������
2012-11-20   С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#ifndef  __ZXT_OBD_GET_ODOMETER_H__
#define  __ZXT_OBD_GET_ODOMETER_H__

#ifdef    GET_ODOMETER_VAR
#define   GET_ODOMETER_VAR_EXT
#else
#define   GET_ODOMETER_VAR_EXT  extern
#endif

#ifdef    GET_ODOMETER_FUN
#define   GET_ODOMETER_FUN_EXT
#else
#define   GET_ODOMETER_FUN_EXT  extern
#endif
/*
********************************************************************************
                              ����:��������
********************************************************************************
*/
//������̻�ȡ
typedef struct TYPEDEF_BUS_ODO_PAR
{
    uint32 u32BusFrameId;                                                  //֡ID
    uint8  u8StartByte;                                                    //����һ���ֽڿ�ʼ��ȡ
    uint16 u16FrameIdTime;                                                 //�ȴ�ʱ��
    bool   bSwapByte;                                                      //�ߵ�λ����
} TYPEDEF_BUS_ODO_PAR;


/*
********************************************************************************
                              ����:��������
********************************************************************************
*/
GET_ODOMETER_VAR_EXT TYPEDEF_BUS_ODO_PAR g_stOdoMeterLib    [];
GET_ODOMETER_VAR_EXT uint32              g_u32CalibrationOdo;
/*
********************************************************************************
                              ����:��������
********************************************************************************
*/
GET_ODOMETER_FUN_EXT void *pfun_get_odometer_withcalculation( void *pstsysChoice, void *pu8FunctionParameter );
GET_ODOMETER_FUN_EXT void *pfun_get_odometer_allcar( uint8 *pCmdIndex, void *pArg1 );
GET_ODOMETER_FUN_EXT void *pfun_get_odometer_from_bus( void *pArg0, void *pu8FunctionParameter );
GET_ODOMETER_FUN_EXT void *pfun_get_odometer_from_obd( void *pArg0, void *pu8FunctionParameter );

#endif





