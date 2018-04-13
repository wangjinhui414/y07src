/*
********************************************************************************
File name  :ZXT_sys_cfg.c
Description:���ļ���Ҫ������ϵͳ��Դ���ñ�,����ֻ�Ƕ�����ϵͳ��Դ������,��������
            �����ʼ������Դ�޹صı���,�����ǹ�������Ա�����õ�
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ����ϵͳ��Դ���
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#define OBD_NOR_STD_SYS_CFG_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                         ����:���������ñ�,��ʵ������õľ���ÿһ��ϵͳ����
                              �ı�������������
��Ա0�������
��Ա1����������ֵ��
��Ա2. Э���������ñ�
       u8Protocol_Lib_Offset[0]���ͺ�����
       u8Protocol_Lib_Offset[1]���պ���,��ʱû����
       u8Protocol_Lib_Offset[2]Э��ȷ�Ϻ���
       u8ProtocolLibOffset[3]ͨѶ�������ú���,ֻҪ���ϵͳȷ��ͨѶ�����϶�Ҳ��ȷ����
       u8Protocol_Lib_Offset[4]��ʱû������
********************************************************************************
*/

const TYPEDEF_MODE_CELL g_stEngLibTab[] =
{
    //OBD�ߵ͵�ƽ����K
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode0Cmdtab0,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode0Index,
        {0, 0, 0, 0, 0},

    },
    //OBD��ַ�뼤��K
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode0Cmdtab0,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode8Index,
        {0, 0, 0, 0, 0},

    },
    //OBD��ַ�뼤�� 68 6a
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode1Cmdtab0,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode1Index,
        {1, 0, 1, 0, 0},

    },
    //OBD CAN��׼֡(7e8)
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab0,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode2Index,
        {2, 0, 5, 0, 3},

    },
    //OBD CAN��׼֡(7e9) //
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab1,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode3Index,
        {2, 0, 5, 0, 4},

    },
    //OBD CAN��׼֡(7ef)  //VIN 7e7 ������
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab2,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode4Index,
        {2, 0, 5, 0, 5},

    },
    //OBD CAN����֡(0x18daf110) //0x18DA10F1
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab3,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode5Index,
        {2, 0, 5, 0, 8},
    },
    //OBD CAN����֡(0x18daf111)  //0x18DA11F1
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab4,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode6Index,
        {2, 0, 5, 0, 7},
    },
    //OBD CAN����֡(0x18daf10e) //���� 0x18da0ef1
    {
        ( TYPEDEF_CMD_TAB* ) g_stObd0Mode2Cmdtab5,
        ( TYPEDEF_CMD_INDEX* ) &g_stCmdObd0Mode7Index,
        {2, 0, 5, 0, 6},
    },
};

/*
********************************************************************************
                         ����:���������ñ�,��ʵ������õľ���ÿһ��ϵͳ����
                              �ı�������������
��Ա0�������
��Ա1����������ֵ��
��Ա2. Э���������ñ�u8Protocol_Lib_Offset[0]���Ƿ��ͺ�����u8Protocol_Lib_Offset[1]���պ���,
       Э��ȷ�Ϻ���u8Protocol_Lib_Offset[2],u8ProtocolLibOffset[3]�ñ��������ж��Ƿ���Ҫ���Ϳ���
********************************************************************************
*/

const TYPEDEF_MODE_CELL g_stAtLibTab[] =
{
    0
};
/*
********************************************************************************
                         ����:�Ǳ����ñ�,��ʵ������õľ���ÿһ��ϵͳ����
                              �ı�������������
��Ա0�������
��Ա1����������ֵ��
��Ա2. Э���������ñ�u8Protocol_Lib_Offset[0]���Ƿ��ͺ�����u8Protocol_Lib_Offset[1]���պ���,
       Э��ȷ�Ϻ���u8Protocol_Lib_Offset[2],u8ProtocolLibOffset[3]�ñ��������ж��Ƿ���Ҫ���Ϳ���
********************************************************************************
*/
const TYPEDEF_MODE_CELL g_stInstrumentPanelLibTab[] =
{
    0
};

/*
********************************************************************************
                         ����:����ϵͳ�����ñ�
01.������
02.�Զ�����
03.ABS�ƶ�
08.�յ�/����
09.�������
15.��ȫ����
16.������
17.�Ǳ��
19.CAN����
25.��������
36.��ʻԱ������
42.��ʻԱ�೵��
44.����ת��
46.��������ϵͳ
52.�˿Ͳ೵��
53.פ���ƶ�
55.믵����
56.������
62.�����
72.�Һ���
********************************************************************************
*/
const TYPEDEF_MODE_CELL *g_pstSysLibTab[] =
{
    g_stEngLibTab,   //00������
    g_stAtLibTab,    //01
    NULL,            //02
    NULL,            //03
    NULL,            //04
    NULL,            //05
    NULL,            //06
    NULL,            //07
    NULL,            //08
    NULL,            //09
    NULL,            //10
    NULL,            //11
    NULL,            //12
    NULL,            //13
    NULL,            //14
    NULL,            //15
    g_stInstrumentPanelLibTab,    //16�Ǳ�ϵͳ
    NULL,            //18
    NULL,            //19
    NULL,            //20
    NULL,            //21
    NULL,            //22
    NULL,            //23
};



