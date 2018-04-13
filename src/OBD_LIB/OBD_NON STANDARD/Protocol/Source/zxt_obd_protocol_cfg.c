/*
********************************************************************************
File name  : ZXT_protocol_cfg.c
Description: ���ļ���Ҫ�Ƕ�����ϵͳ��ܵ�Э�����й���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ��Э����������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/

#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                         ����:����ϵͳ���Э��ջ�����ñ�
                              ������2�����ñ� 1:���ͺ������ñ�
                                              2:���պ������ñ�
********************************************************************************
*/

//���ͺ���
const TYPEDEF_pPROTOCOLLIB g_stProtocolSendLib[] =
{
#if IS0_14230_SEND_GROUP_EN==1
    pfun_iso14230_send_command_group1,
#endif
#if IS0_9141_SEND_GROUP_EN == 1
    pfun_iso_9141_send_command_group,
#endif
#if ISO_15765_SEND_GROUP_EN==1
    pfun_iso15765_send_command_group,
#endif
#if  GM_SEND_GROUP_EN ==1
    pfun_gm_send_command_group,
#endif
#if IS0_1281_SEND_GROUP_EN == 1
    pfun_iso1281_send_command_group,
#endif
};
//���պ���
const TYPEDEF_pPROTOCOLLIB g_stProtocolReceiveLib[] =
{
    pfun_iso_14230_receive,
};

//Э��У�Ժ���
const TYPEDEF_pPROTOCOLLIB g_stProtocolCheckLlib[] =
{
#if ISO_14230_80_CHECK_EN==1
    pfun_iso_14230_80_check,
#endif
#if ISO_9141_68_CHECK_EN==1
    pfun_iso_9141_68_check,
#endif
#if ISO_15765_CHECK_EN==1
    pfun_iso_15765_check,
#endif
#if GM_CHECK_EN==1
    pfun_gm_check,
#endif
#if ISO_1281_CHECK_EN == 1
    pfun_iso_1281_check,
#endif
#if ISO_15765_CHECK_EN1==1
    pfun_iso_15765_check1,
#endif
};
/*
********************************************************************************
                              ����:����ϵͳ֡ID��
********************************************************************************
*/
const TYPEDEF_2WORD4BYTE g_unnu32FrameIdLib[] =
{
    0x33f1,     //0��׼K��
    0x07df,     //1��׼CAN��
    0x18db33f1, //2����֡
    0X7E0,      //3 OBD
    0X7E1,      //4 OBD
    0x7e7,      //5 OBD   ������
    0x18DAF10E, //6 OBD   ������չ֡
    0x18DA11F1, //7 obd   ������չ֡
    0x18DA10F1, //8 obd   �ֶ���
};

/*
********************************************************************************
                              ����:����ϵͳ�˲�ID����
********************************************************************************
*/
const TYPEDEF_2WORD4BYTE g_unnu32FilterId [] =
{
    0x7e8,     //00 (obd)7E8
    0x7e9,     //01 (obd)7e9
    0x7ef,     //02 (obd)7EF(�ִ�)
    0x18daf110,//03 OBD��չ֡
    0x18daf111,//04 OBD��չ֡
    0x18daf10e,//05 OBD��չ֡
    0xf110,    //06 ������������(K)
};




