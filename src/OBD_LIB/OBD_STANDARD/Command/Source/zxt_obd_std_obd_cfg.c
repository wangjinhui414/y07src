/*
********************************************************************************
File name  : Zxt_obd_cfg.c
Description:
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ��ʼ���ײ���Ҫ�ı���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define OBD_CFG_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                              ����:���ܵ�������
������д˳��:
1.ѡ�������һ�����ܿ�(����:��������g_pCreatLinkLib,�����g_pReaDtcLib)
2.ѡ����Ƕ�Ӧ���е��Ǹ�����
3.�ú�����Ҫ�Ĳ���,���п���,��Ҫ������Ա��������
********************************************************************************
*/
const uint8 u8FunctionParameter[] = {0, 1, 2, 3, 4, 5, 6, 7};
/*

********************************************************************************
                              ����:OBD���й��ܵ���
********************************************************************************
*/
//��׼K�߸ߵ͵�ƽ����-0BD����
const TYPEDEF_FUNCTION_CFG stObdRdtcScheduleKLine0[] =
{
    //����
    0, 0, &u8FunctionParameter[0],
    //����(��������)
    1, 1, &u8FunctionParameter[0],
    //�˳�
    5, 0, NULL
};
//��׼K�ߵ�ַ�뼤��  68 6A- OBD����
const TYPEDEF_FUNCTION_CFG stObdRdtcScheduleKLine1[] =
{
    //����
    0, 1, &u8FunctionParameter[0],
    //����(��������)
    1, 1, &u8FunctionParameter[0],
    //�˳�
    5, 0, NULL
};
//��׼can�߼���-OBD����(˹��³)
const TYPEDEF_FUNCTION_CFG stObdRdtcScheduleCan[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //����(������)
    1, 0, &u8FunctionParameter[0],
};

//��׼K��(�ߵ͵�ƽ����)��ȡ������
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine0[] =
{
    //����
    0, 0, &u8FunctionParameter[0],
    //������
    3, 0, &u8FunctionParameter[0],
    //�˳�
    5, 0, NULL
};
//��׼K��(��ַ�뼤��)��ȡ������
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine1[] =
{
    //����
    0, 1, &u8FunctionParameter[0],
    //������
    3, 0, &u8FunctionParameter[0],
    //�˳�
    5, 0, NULL
};
//68 6a(��ַ�뼤��)��ȡ������
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine2[] =
{
    //����
    0, 1, &u8FunctionParameter[0],
    //������
    3, 0, &u8FunctionParameter[1],
    //�˳�
    5, 0, NULL
};
//��׼CAN��������(7e8)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan0[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 0, &u8FunctionParameter[2],
};
//��׼CAN��������(7e9)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan1[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 0, &u8FunctionParameter[3],
};
//��׼CAN��������(7ef)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan2[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 0, &u8FunctionParameter[4],
};
//����CAN��������(0x18daf110)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan3[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 0, &u8FunctionParameter[5],
};
//����CAN��������(0x18daf111)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan4[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 0, &u8FunctionParameter[6],
};
//����CAN��������(0x18daf10e)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan5[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 0, &u8FunctionParameter[7],
};
//��׼K��(�ߵ͵�ƽ����)��ȡ������(��̬)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine3[] =
{
    //����
    0, 0, &u8FunctionParameter[0],
    //������
    3, 1, &u8FunctionParameter[0],
    //�˳�
    5, 0, NULL
};
//��׼K��(��ַ�뼤��)��ȡ������(��̬)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine4[] =
{
    //����
    0, 1, &u8FunctionParameter[0],
    //������
    3, 1, &u8FunctionParameter[0],
    //�˳�
    5, 0, NULL
};
//68 6a(��ַ�뼤��)��ȡ������(��̬)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleKLine5[] =
{
    //����
    0, 1, &u8FunctionParameter[0],
    //������
    3, 1, &u8FunctionParameter[1],
    //�˳�
    5, 0, NULL
};
//��׼CAN��������(7e8)(��̬)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan6[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 1, &u8FunctionParameter[2],
};
//��׼CAN��������(7e9)(��̬)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan7[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 1, &u8FunctionParameter[3],
};
//��׼CAN��������(7ef)(��̬)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan8[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 1, &u8FunctionParameter[4],
};
//����CAN��������(0x18daf110)(��̬)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan9[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 1, &u8FunctionParameter[5],
};
//����CAN��������(0x18daf111)(��̬)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan10[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 1, &u8FunctionParameter[6],
};
//����CAN��������(0x18daf10e)(��̬)
const TYPEDEF_FUNCTION_CFG stObdDataStreamScheduleCan11[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //������
    3, 1, &u8FunctionParameter[7],
};
//��׼K�߸ߵ͵�ƽ����-0BD����
const TYPEDEF_FUNCTION_CFG stObdCdtcScheduleKLine0[] =
{
    //����
    0, 0, &u8FunctionParameter[0],
    //����
    2, 0, NULL,
    //�˳�
    5, 0, NULL
};
//��׼K�ߵ�ַ�뼤��  68 6A- OBD����
const TYPEDEF_FUNCTION_CFG stObdCdtcScheduleKLine1[] =
{
    //����
    0, 1, &u8FunctionParameter[0],
    //����
    2, 0, NULL,
    //�˳�
    5, 0, NULL
};
//��׼can�߼���-OBD��
const TYPEDEF_FUNCTION_CFG stObdCdtcScheduleCan[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //����
    2, 0, NULL,
};
//��̻�ȡ
const TYPEDEF_FUNCTION_CFG stObdOdoMeterSchedule[] =
{
    //ע���������õ��ǵ�0��λ��
    4, 0, NULL,
};

//VIN���ȡ
//��׼can�߼���-OBD VIN���ȡ
const TYPEDEF_FUNCTION_CFG stObdVinScheduleCan[] =
{
    //����
    0, 2, &u8FunctionParameter[0],
    //vin��ȡ
    6, 0, &u8FunctionParameter[0],
};

//��׼K�߸ߵ͵�ƽ����-OBD VIN���ȡ
const TYPEDEF_FUNCTION_CFG stObdVinScheduleKLine0[] =
{
    //����
    0, 0, &u8FunctionParameter[0],
    //VIN���ȡ
    6, 0, &u8FunctionParameter[1],
    //�˳�
    5, 0, NULL
};
//��׼K�ߵ�ַ�뼤��  68 6A- OBD VIN���ȡ
const TYPEDEF_FUNCTION_CFG stObdVinScheduleKLine1[] =
{
    //����
    0, 1, &u8FunctionParameter[0],
    //VIN���ȡ
    6, 0, &u8FunctionParameter[1],
    //�˳�
    5, 0, NULL
};

/*

********************************************************************************
                              ����:OBD����
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdRdtc[] =
{
    //CAN���� ��׼֡_������(7e8)
    {
        2, 0, 3,  200, stObdRdtcScheduleCan//0,
    },
    //CAN���� ��׼֡_������(7e9)
    {
        2, 0, 4,  200, stObdRdtcScheduleCan//0,
    },
    //CAN���� ��׼֡_������(7ef)
    {
        2, 0, 5,  200, stObdRdtcScheduleCan//0,
    },
    //can��������֡
    {
        2, 0, 6,  200, stObdRdtcScheduleCan//0,
    },
    //can��������֡
    {
        2, 0, 7,  200, stObdRdtcScheduleCan//0,
    },
    //can��������֡
    {
        2, 0, 8,  200, stObdRdtcScheduleCan//0,
    },
    //�ߵ͵�ƽ����  K��_������(ģ����ͨ��)
    {
        3, 0, 0,  2000, stObdRdtcScheduleKLine0  //0,
    },
    //��ַ�뼤�� K��_������(ģ����ͨ��)
    {
        3, 0, 1,  3000, stObdRdtcScheduleKLine1//(���⳵�Ƚ��ر�)
    },
    //��ַ�뼤�� 68 6A_������(���⳵���ͨ��)
    {
        3, 0, 2,  200, stObdRdtcScheduleKLine1
    },
    //j1850vpm,pwm���ں���
};
/*
********************************************************************************
                              ����:OBD�������̶�
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdDataStream[] =
{
    //CAN���� ��׼֡_������(7e8)
    {
        2, 0, 3,  200, stObdDataStreamScheduleCan0
    },
    //CAN���� ��׼֡_������(7e9)
    {
        2, 0, 4,  200, stObdDataStreamScheduleCan1
    },
    //CAN���� ��׼֡_������(7ef)
    {
        2, 0, 5,  200, stObdDataStreamScheduleCan2
    },
    //can��������֡
    {
        2, 0, 6,  200, stObdDataStreamScheduleCan3
    },
    //can��������֡
    {
        2, 0, 7,  200, stObdDataStreamScheduleCan4
    },
    //can��������֡
    {
        2, 0, 8,  200, stObdDataStreamScheduleCan5
    },
    //�ߵ͵�ƽ����  K��_������(ģ����ͨ��)
    {
        3, 0, 0,  2000, stObdDataStreamScheduleKLine0
    },
    //��ַ�뼤�� K��_������
    {
        3, 0, 1,  3000, stObdDataStreamScheduleKLine1//(���⳵�Ƚ��ر�)
    },
    //��ַ�뼤�� 68 6A_������(ģ����ͨ��,���⳵���ͨ��)
    {
        3, 0, 2,  200, stObdDataStreamScheduleKLine2
    },
    //j1850vpm,pwm���ں���
};
/*
********************************************************************************
                              ����:OBD��������̬
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdDataStream1[] =
{
    //CAN���� ��׼֡_������(7e8)
    {
        2, 0, 3,  200, stObdDataStreamScheduleCan6
    },
    //CAN���� ��׼֡_������(7e9)
    {
        2, 0, 4,  200, stObdDataStreamScheduleCan7
    },
    //CAN���� ��׼֡_������(7ef)
    {
        2, 0, 5,  200, stObdDataStreamScheduleCan8
    },
    //can��������֡
    {
        2, 0, 6,  200, stObdDataStreamScheduleCan9
    },
    //can��������֡
    {
        2, 0, 7,  200, stObdDataStreamScheduleCan10
    },
    //can��������֡
    {
        2, 0, 8,  200, stObdDataStreamScheduleCan11
    },
    //�ߵ͵�ƽ����  K��_������(ģ����ͨ��)
    {
        3, 0, 0,  2000, stObdDataStreamScheduleKLine3
    },
    //��ַ�뼤�� K��_������
    {
        3, 0, 1,  3000, stObdDataStreamScheduleKLine4//(���⳵�Ƚ��ر�)
    },
    //��ַ�뼤�� 68 6A_������(ģ����ͨ��,���⳵���ͨ��)
    {
        3, 0, 2,  200, stObdDataStreamScheduleKLine5
    },
    //j1850vpm,pwm���ں���
};
/*

********************************************************************************
                              ����:OBD����
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdCdtc[] =
{
    //CAN���� ��׼֡_������(7e8)
    {
        2, 0, 3,  200, stObdCdtcScheduleCan
    },
    //CAN���� ��׼֡_������(7e9)
    {
        2, 0, 4,  200, stObdCdtcScheduleCan
    },
    //CAN���� ��׼֡_������(7ef)
    {
        2, 0, 5,  200, stObdCdtcScheduleCan
    },
    //can��������֡
    {
        2, 0, 6,  200, stObdCdtcScheduleCan
    },
    //can��������֡
    {
        2, 0, 7,  200, stObdCdtcScheduleCan
    },
    //can��������֡
    {
        2, 0, 8,  200, stObdCdtcScheduleCan
    },
    //�ߵ͵�ƽ����  K��_������(ģ����ͨ��)
    {
        3, 0, 0,  2000, stObdCdtcScheduleKLine0
    },
    //��ַ�뼤�� K��_������(ģ����ͨ��)
    {
        3, 0, 1,  3000, stObdCdtcScheduleKLine1//(���⳵�Ƚ��ر�)
    },
    //��ַ�뼤�� 68 6A_������(���⳵���ͨ��)
    {
        3, 0, 2,  200, stObdCdtcScheduleKLine1
    },
    //j1850vpm,pwm���ں���
};
/*

********************************************************************************
                              ����:OBD�����
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdOdoMeter[] =
{
    //ע������ϵͳ����ģ��������255,��ʵ�ǲ����ڵ�
    {
        1, 0xff, 0xff, 200, stObdOdoMeterSchedule
    },
};

/*

********************************************************************************
                              ����:OBD��VIN
********************************************************************************
*/
const TYPEDEF_SYS_CFG g_stObdVin[] =
{
    //CAN���� ��׼֡_������(7e8)
    {
        2, 0, 3,  200, stObdVinScheduleCan//0,
    },
    //CAN���� ��׼֡_������(7e9)
    {
        2, 0, 4,  200, stObdVinScheduleCan//0,
    },
    //CAN���� ��׼֡_������(7ef)
    {
        2, 0, 5,  200, stObdVinScheduleCan//0,
    },
    //can��������֡
    {
        2, 0, 6,  200, stObdVinScheduleCan//0,
    },
    //can��������֡
    {
        2, 0, 7,  200, stObdVinScheduleCan//0,
    },
    //can��������֡
    {
        2, 0, 8,  200, stObdVinScheduleCan//0,
    },
    //�ߵ͵�ƽ����  K��_������(ģ����ͨ��)
    {
        3, 0, 0,  2000, stObdVinScheduleKLine0  //0,
    },
    //��ַ�뼤�� K��_������(ģ����ͨ��)
    {
        3, 0, 1,  3000, stObdVinScheduleKLine1//(���⳵�Ƚ��ر�)
    },
};
