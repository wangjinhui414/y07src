/*
********************************************************************************
File name  : ZXT_data_stream.h
Description: ��ͷ�ļ���Ҫ��Datastream.c����ر�������������,����������������޹���Ϣ
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ������������ر����ͺ�������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/
#ifndef  __ZXT_OBD_DATA_STREAM_H__
#define  __ZXT_OBD_DATA_STREAM_H__


#ifdef    DATA_STREAM_VAR
#define   DATA_STREAM_VAR_EXT
#else
#define   DATA_STREAM_VAR_EXT  extern
#endif

#ifdef    DATA_STREAM_FUN
#define   DATA_STREAM_FUN_EXT
#else
#define   DATA_STREAM_FUN_EXT  extern
#endif

/*
********************************************************************************
                              ����:��������ر�־λ����
********************************************************************************
*/
#define  DEF_DATASTREAM_STATUS_START                       0X00
#define  DEF_DATASTREAM_STATUS_ACTIVE_STOP                 0XFF
#define  DEF_DATASTREAM_STATUS_SPEED_STOP                  0XFE
#define  DEF_DATASTREAM_STATUS_TIMEOUT_STOP                0XFD
#define  DEF_DATASTREAM_STATUS_DATABUS_FAILURE             0XFC
#define  DEF_DATASTREAM_STATUS_OPTION_OVER                 0XFB


#define  DEF_DATASTREAM_ID_TOTAL_MAX                       10 
#define  DEF_DATASTREAM_ID_OFFSET_MAX                      103
/*
********************************************************************************
                            ����:����������������������
********************************************************************************
*/
//�������������������

typedef struct TYPEDEF_DATASTREAM_LIST
{
    TYPEDEF_1WORD2BYTE DataStreamId;
    struct TYPEDEF_DATASTREAM_LIST *Next;
} TYPEDEF_DATASTREAM_LIST;

//�����������ؼ��ֽڽṹ��
typedef struct TYPEDEF_DATASTREAM_GET_DATA_LIST
{
    uint8 u8StartByte;                                                     //����һ���ֽڿ�ʼ��ȡ����
    uint8 u8ByteTotal;                                                     //�ܹ�Ҫ��ȡ���ٸ��ֽ�
    uint8 u8CmdIndex;                                                      //����������
} TYPEDEF_DATASTREAM_GET_DATA_LIST;
//���������йؼ��ֽ������
typedef struct TYPEDEF_DATASTREAM_LIB
{
    uint8 u8Total;//����������
    const TYPEDEF_DATASTREAM_GET_DATA_LIST *pstGetdataList;
} TYPEDEF_DATASTREAM_LIB;
/*
********************************************************************************
                            ����:����������������������
********************************************************************************
*/
DATA_STREAM_VAR_EXT const TYPEDEF_DATASTREAM_LIB g_pstDataStreamLib[];
/*
********************************************************************************
                            ����:������������������
********************************************************************************
*/

DATA_STREAM_FUN_EXT void *pfun_data_stream_all_car( void* pArg0, void* pArg1 );
DATA_STREAM_FUN_EXT void *pfun_data_stream_all_Obd( void* pArg0, void *pu8FunctionParameter );
DATA_STREAM_FUN_EXT void pfun_init_data_Stream_idlist( TYPEDEF_DATASTREAM_LIST *pList,
        uint16 u16ListSize, uint16 u16DataStreamIdSize );
DATA_STREAM_FUN_EXT void pfun_init_data_Stream_idlist0( TYPEDEF_DATASTREAM_LIST *pList, uint8 u8ListSize,
        uint8 u8DataStreamIdSize, uint8 *pu8DataStreamId );
DATA_STREAM_FUN_EXT void pfun_get_data_stream_calculation_bytes( uint8 u8StartByte,
        uint8 u8ByteTotal, uint8 u8DataStreamCurId, uint8 *pSaveBuff );
#endif






