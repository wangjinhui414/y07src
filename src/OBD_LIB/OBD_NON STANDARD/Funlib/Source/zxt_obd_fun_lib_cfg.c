/*
********************************************************************************
File name  : ZXT_fun_lib_init.c
Description: �����ʼ���Ͷ���ı������ǹ��ײ���õ�ȫ�ֱ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/

#define  FUN_LIB
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
********************************************************************************
                            ����:Creatlink.c������ʼ��
********************************************************************************
*/

const TYPEDEF_pFUNLIB g_pCreatLinkLib[] =
{
#if CREATE_LINK_LOW_HIGH_25MS_EN == 1
    pfun_active_low_high_xxms,//�ߵ͵�ƽ�����
#endif
#if ACTIVE_5BAUD_ONLY_LAST_BYTE_EN == 1
    pfun_active_5baud_negate_last_byte_back_addr,
#endif
#if CREATE_LINK_CAN_BUS_EN ==1
    pfun_active_can_bus,
#endif
#if ACTIVE_5BAUD_KWP1281_EN == 1
    pfun_active_5baud_kwp1281
#endif
};
/*
********************************************************************************
                            ����:Readtc.c������ʼ��
********************************************************************************
*/
const TYPEDEF_pFUNLIB g_pReaDtcLib[] =
{
#if    RDTC_WITCH_TOTAL_EN==1
    pfun_allcar_rdtc_witch_total,//��ȡ��ǰ������,ͬʱ��������
#endif

#if    RDTC_WITCH_NOTOTAL_EN==1
    pfun_allcar_rdtc_witch_nototal,//��ȡ��ǰ������,ͬʱû������
#endif
#if  RDTC_GM_BUCK_WITCH_NOTOTAL_EN == 1
    pfun_buck_rdtc_witch_nototal, //ͨ�ó���ȡ������
#endif
#if RDTC_VW_PUSANG_WITCH_NOTOTAL_EN==1
    pfun_vw_pusang_rdtc_witch_nototal //����1281��ɣ����ȡ������
#endif
};
/*
********************************************************************************
                            ����:Cleardtc.c������ʼ��
********************************************************************************
*/
const TYPEDEF_pFUNLIB g_pClearDtcLib[] =
{
#if CDTC_FOR_ALL_CAR_EN==1
    pfun_cdtc_for_all_car,
#endif
};
/*
********************************************************************************
                            ����:Datastream.c������ʼ��
********************************************************************************
*/
const TYPEDEF_pFUNLIB g_pDataStreamLib[] =
{
#if RDATA_STREAM_EN==1
    pfun_data_stream_all_car,
    pfun_data_stream_all_Obd,
#endif
};
/*
********************************************************************************
                              ����:Zxt_get_odometer.c
********************************************************************************
*/
const TYPEDEF_pFUNLIB g_pOdoMeterLib[] =
{

    pfun_get_odometer_from_obd,
#if GET_ODOMETER_WITHCALCULATION_EN == 1
    pfun_get_odometer_withcalculation,
#endif
    pfun_get_odometer_from_bus,
};
/*
********************************************************************************
                              ����:Zxt_sys_exit.c
********************************************************************************
*/
const TYPEDEF_pFUNLIB      g_pSysExitLib[] =
{
    pfun_sys_exit_fun,
};

/*
********************************************************************************
                              ����:Zxt_sys_vin.c
********************************************************************************
*/
const TYPEDEF_pFUNLIB      g_pSysVinLib[] =
{
    pfun_getvin_for_all_car,
};
/*
********************************************************************************
                            ����:����ϵͳfun_lib�ṹ��
********************************************************************************
*/

const TYPEDEF_pFUNLIB *g_pFunLib[] =
{
    g_pCreatLinkLib,      //����
    g_pReaDtcLib,         //����
    g_pClearDtcLib,       //����
    g_pDataStreamLib,     //������
    g_pOdoMeterLib,       //���
    g_pSysExitLib,        //�˳�
    g_pSysVinLib,         //VIN��ȡ
};




