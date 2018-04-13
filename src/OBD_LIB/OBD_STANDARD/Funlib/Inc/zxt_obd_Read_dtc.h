/*
********************************************************************************
File name  : ZXT_Read_dtc.h
Description: ��ͷ�ļ���Ҫ��Datastream.c����ر�������������,����������������޹���Ϣ
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ������ȡ����������ͺ�������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#ifndef  __ZXT_OBD_READ_DTC_H__
#define  __ZXT_OBD_READ_DTC_H__

#ifdef    READ_DTC_VAR
#define   READ_DTC_VAR_EXT
#else
#define   READ_DTC_VAR_EXT  extern
#endif

#ifdef    READ_DTC_FUN
#define   READ_DTC_FUN_EXT
#else
#define   READ_DTC_FUN_EXT  extern
#endif


/*
********************************************************************************
                               ����:��ȡ�������������Ͷ���
********************************************************************************
*/

typedef struct TYPEDEF_DTC_WITHITOTAL
{
    uint8 u8DtcTotalCacheAddr;                                             //���������������ַ
    uint8 u8DtcTotalByte;                                                  //�����������ֽ�
    uint8 u8DtcCodeCacheAddr;                                              //������Ż����ַ
    uint8 u8DtcCodeStartByte;                                              //������ſ�ʼ�ֽ�
    uint8 u8DtcCodevalidByte;                                              //��������Ч�ֽ���(�����ֽ�һ��������)
    uint8 u8DtcCodeXXByteOneDtc;                                           //���ٸ��ֽ�һ��������
    uint8 u8DtcCodeLibChoice;                                              //���ѡ��
} TYPEDEF_DTC_WITHITOTAL;                                                        //�й���������
                                                                           
typedef struct TYPEDEF_DTC_NOTOTAL
{                                                                          
    uint8 u8DtcCodeCacheAddr;                                              //������Ż����ַ
    uint8 u8DtcCodeStartByte;                                              //������ſ�ʼ�ֽ�
    uint8 u8DtcCodevalidByte;                                              //��������Ч�ֽ���(�����ֽ�һ��������)
    uint8 u8DtcCodeXXByteOneDtc;                                           //���ٸ��ֽ�һ��������
    uint8 u8DtcCodeLibChoice;                                              //���ѡ��
} TYPEDEF_DTC_NOTOTAL;                                                      //û�й���������





/*
********************************************************************************
                               ����:��ȡ�������������
********************************************************************************
*/

READ_DTC_VAR_EXT const TYPEDEF_DTC_WITHITOTAL g_stDtcWithTotalConfigLib[];  //�������������ȡ
READ_DTC_VAR_EXT const TYPEDEF_DTC_NOTOTAL    g_stDtcWithNoTotalConfigLib[];//���������������ȡ

/*
********************************************************************************
                               ����:��ȡ�����뺯������
pfun_allcar_rdtc_witch_total��ȡ��ǰ������������
pfun_rdtc_for_history_witch_totall��ȡ��ʷ������������
pfun_allcar_rdtc                  �κκ����������õĶ�ȡ�����뺯��
fun_rdtc_deal_with_total          ���������������
fun_rdtc_load_dtc                 װ�ع�����
********************************************************************************
*/

READ_DTC_VAR_EXT void *pfun_allcar_rdtc_witch_total( void *pstsysChoice, void *pu8FunctionParameter );
READ_DTC_VAR_EXT void *pfun_allcar_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter );
READ_DTC_VAR_EXT void *pfun_buck_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter );
READ_DTC_VAR_EXT void *pfun_vw_pusang_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter );
READ_DTC_VAR_EXT void *pfun_rdtc_allcar( uint8 *pCmdIndex, void *pArg1 );
READ_DTC_VAR_EXT uint16 fun_rdtc_deal_with_total( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache );
READ_DTC_VAR_EXT uint16 fun_rdtc_deal_with_nototal( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache );
READ_DTC_VAR_EXT uint16 fun_buck_rdtc_deal_with_nototal( uint8 u8SysChoice, uint8 *pCache );
READ_DTC_VAR_EXT uint16 fun_vw_pusang_rdtc_deal_with_nototal( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache );
READ_DTC_VAR_EXT void fun_rdtc_load_dtc( uint8 u8DtcTotal, uint8 u8DtcCodeXXByteOneDtc,
        uint8 u8DtcCodevalidByte, uint8 *pCache, uint8 *pDtcCodeStart );

#endif






