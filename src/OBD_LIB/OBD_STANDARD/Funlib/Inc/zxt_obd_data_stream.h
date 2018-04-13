/*
********************************************************************************
File name  : ZXT_data_stream.h
Description: 该头文件主要是Datastream.c中相关变量函数的声明,不允许在这里放置无关信息
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立数据流相关变量和函数声明
修改日期   : 修改者 : 修改内容:简单详述
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
                              描述:数据流相关标志位定义
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
                            描述:数据流数据类型声明区域
********************************************************************************
*/
//处理数据流发送命令表

typedef struct TYPEDEF_DATASTREAM_LIST
{
    TYPEDEF_1WORD2BYTE DataStreamId;
    struct TYPEDEF_DATASTREAM_LIST *Next;
} TYPEDEF_DATASTREAM_LIST;

//处理数据流关键字节结构体
typedef struct TYPEDEF_DATASTREAM_GET_DATA_LIST
{
    uint8 u8StartByte;                                                     //从哪一个字节开始获取数据
    uint8 u8ByteTotal;                                                     //总共要获取多少给字节
    uint8 u8CmdIndex;                                                      //数据流命令
} TYPEDEF_DATASTREAM_GET_DATA_LIST;
//数据流所有关键字节命令库
typedef struct TYPEDEF_DATASTREAM_LIB
{
    uint8 u8Total;//数据流总数
    const TYPEDEF_DATASTREAM_GET_DATA_LIST *pstGetdataList;
} TYPEDEF_DATASTREAM_LIB;
/*
********************************************************************************
                            描述:数据流函数变量声明区域
********************************************************************************
*/
DATA_STREAM_VAR_EXT const TYPEDEF_DATASTREAM_LIB g_pstDataStreamLib[];
/*
********************************************************************************
                            描述:数据流函数声明区域
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






