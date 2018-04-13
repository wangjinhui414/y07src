/*
********************************************************************************
File name  : ZXT_data_stream.c
Description: �����д����������������غ���
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ��������������
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/

#define DATA_STREAM_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
********************************************************************************
Name        :void *pfun_data_stream_all_car(void* pArg0, void* pArg1).
Description:pfun_data_stream_all_car
Input      :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :1һ��Ҫע�����ִ�еĲ��裬�������Ҵ��ҹ���˳��
2.�ó�������2��WHILEһ��Ҫע���ϵ
********************************************************************************
*/
#if RDATA_STREAM_EN == 1
void *pfun_data_stream_all_car( void* pArg0, void *pu8FunctionParameter )
{

    void *pCommStatus0 = NULL;
    void *pCommStatus1 = NULL;
    uint8 u8Error;
    //u8DataStreamCmdIdOffSet����ƫ����
    //u8DataStreamCmdId������ͨ�������������ֵ
    //stDataStreamIdBuff�洢ϵͳID��
    //u16DataStreamIdTotalҪ��ȡ������ID����
    //g_stPhoneCommandShare.bReadDataStream�������˳���������
    //u16DataStreamDataCacheAddr �������洢��ʼ��ַ
    TYPEDEF_DATASTREAM_LIST *pstDataStreamIdBuff = NULL;
    uint8  u8DataStreamCmdIdOffSet = 0;
    uint8  u8DataStreamCmdId[] = {1, 0};
    uint16 u16DataStreamDataCacheAddr = 1;
    //��ȡ����һ�����͵�������
    uint8 u8GetDataStreamchoice = ( ( uint8* )pu8FunctionParameter )[0];
    //��ȡ������������(������ʱд��)
    uint16 u16DataStreamIdTotal = 5;
    //ֱ��ָ��ú�����Ҫ���ϵı�����
    const TYPEDEF_DATASTREAM_GET_DATA_LIST *pstDataStream = g_pstDataStreamLib[u8GetDataStreamchoice].pstGetdataList;

    //Ŀǰ���ֻ�ܴ洢30��������
    TYPEDEF_DATASTREAM_LIST  stDataStreamIdBuff[6] = {0};

    //���ú��ϴ�����������
    ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[0] = u16DataStreamIdTotal * 6;
    ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )-> u8Datalen = u16DataStreamIdTotal * 6 + 1;

    //��ʼ������
    pfun_init_data_Stream_idlist( stDataStreamIdBuff, 6, u16DataStreamIdTotal );
    //Ĭ������ΪͨѶ��ʱ
    pCommStatus1 = ( void * )& g_enumFrameFlgArr[1];

    //���ú������ַ
    pstDataStreamIdBuff = stDataStreamIdBuff;

    //���ú��������洢��ַ
    u16DataStreamDataCacheAddr = 1;

    //�ж������ǲ����Ѿ���ͷ
    while( pstDataStreamIdBuff->Next != NULL )
    {
        //���Ͷ�����������
        u8DataStreamCmdIdOffSet = pstDataStream[pstDataStreamIdBuff->DataStreamId.u16Bit].u8CmdIndex;
        u8DataStreamCmdId[1] = u8DataStreamCmdIdOffSet;
        //�����ڵ�����ص������������ж��ǲ����Ѿ���ʱ
        OSMboxPost( g_pstAnalyzerToPerformer, u8DataStreamCmdId );
        pCommStatus0 = OSMboxPend( g_pstPerformerToAnalyzer, 0, &u8Error );

        //���ͨѶʧ����ô����Ӧ������Ϊ0,һ����6��0
        //STEP0:�ж��Ƿ�ɹ�
        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus0 != OK )
        {
            memset( &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr], 0, 6 );
            u16DataStreamDataCacheAddr += 2;
        }

        else
        {
            //ֻҪ��һ��ɹ�������ͨѶ״̬,ͨѶ�ɹ�
            pCommStatus1 = ( void * )& g_enumFrameFlgArr[2];
            //��Ϊ������ID�Ǵ�01��ʼ
            pstDataStreamIdBuff->DataStreamId.u16Bit++;
            ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr++] = pstDataStreamIdBuff->DataStreamId.u8Bit[1];
            ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr++] = pstDataStreamIdBuff->DataStreamId.u8Bit[0];
            pstDataStreamIdBuff->DataStreamId.u16Bit--;
            //ͬʱ��ȡ�������ؼ��ֽ�
            pfun_get_data_stream_calculation_bytes
            (
                pstDataStream[pstDataStreamIdBuff->DataStreamId.u16Bit].u8StartByte,
                pstDataStream[pstDataStreamIdBuff->DataStreamId.u16Bit].u8ByteTotal,
                ( ( ( g_pModeLibTab->pCmdTabChoice )[u8DataStreamCmdIdOffSet] ).u8BufferId ) & 0x0f,
                &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr]
            );

        }

        //step3:ָ��ƫ��
        u16DataStreamDataCacheAddr += 4;
        //step4:ȡ��һ����ɢID
        pstDataStreamIdBuff = pstDataStreamIdBuff->Next;
    }

    //����ǿ�ƽ�21���������ĳ�00,ͬʱ��֧�ֵ�
    memset( &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[19], 0, 6 );
    ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[20] = 4;
    return ( pCommStatus1 );
}
#endif
/*
*******************************************************************************
Name        :pfun_init_data_Stream_idlist
Description:�ú�������ҪĿ���ǽ�ĳһ�������Ŀռ���ϳ�����ͬʱ�������г�ʼ��
Input      :TYPEDEF_DATASTREAM_LIST  *pListHead�����Ҫ���ϵ��׵�ַuint16 u16ListSize
u16DataStreamIdSize Ҫ��ȡ�����ݴ�С��pDataStreamId������ID
Output     :none
Return     :none
Others     :
********************************************************************************
*/
#if INIT_DATA_STREAM_IDLIST ==1
void pfun_init_data_Stream_idlist( TYPEDEF_DATASTREAM_LIST *pList, uint16 u16ListSize,
                                   uint16 u16DataStreamIdSize )
{
    //��ʼ�������ƫ�Ƶ�ַ
    TYPEDEF_1WORD2BYTE ListSizetmp;
    TYPEDEF_DATASTREAM_LIST *pListHead = pList;

    for( ListSizetmp.u16Bit = 0; ( ListSizetmp.u16Bit < u16DataStreamIdSize && u16DataStreamIdSize < u16ListSize ); ListSizetmp.u16Bit++ )
    {
        //��ID�洢��
        pList[ListSizetmp.u16Bit].DataStreamId.u8Bit[1] = ListSizetmp.u8Bit[1];
        pList[ListSizetmp.u16Bit].DataStreamId.u8Bit[0] = ListSizetmp.u8Bit[0];
        pList[ListSizetmp.u16Bit].Next = pListHead + 1 + ListSizetmp.u16Bit;
    }

    pList[ListSizetmp.u16Bit].Next = NULL;
}

void pfun_init_data_Stream_idlist0( TYPEDEF_DATASTREAM_LIST *pList, uint8 u8ListSize,
                                    uint8 u8DataStreamIdSize, uint8 *pu8DataStreamId )
{
    //��ʼ�������ƫ�Ƶ�ַ
    uint8 u8SizeTmp = 0;
    TYPEDEF_DATASTREAM_LIST *pListHead = pList;

    for( u8SizeTmp = 0; ( u8SizeTmp < u8DataStreamIdSize && u8DataStreamIdSize <= u8ListSize ); u8SizeTmp++ )
    {
        //��ID�洢��
        pList[u8SizeTmp].DataStreamId.u16Bit = ( uint16 ) * pu8DataStreamId;
        pList[u8SizeTmp].Next = ++pListHead;
        pu8DataStreamId++;

    }

    pList[u8SizeTmp - 1].Next = &pList[0];
}

#endif

/*
********************************************************************************
Name        :pfun_get_data_Stream_Calculation_bytes
Description:�ú�����Ҫ����Դ�ECU�ǻ�ȡ����
Input      :u8datastreamstartbyte��ECU�ĸ��ֽڿ�ʼ��ȡ����
��ECU�����ȡ���ٸ�����u8DataStreamByteTotal
���ĸ���������ȡ����pSaveBuff���浽����ȥ
Output     :none
Return     :none
Others     :�ɹ�֮��ȷ�������ֵg_pModeLibTab,�������ĸ�ģ�����ͨ��,
ͬʱȷ��Э������
���������Ҫ��λ���ṩ3����������ģ������ ģ��ֵ ͨѶ����
********************************************************************************
*/
#if GET_DATA_STREAM_CALCULATION_BYTES_EN == 1
void pfun_get_data_stream_calculation_bytes( uint8 u8StartByte,
        uint8 u8ByteTotal, uint8 u8DataStreamCurId, uint8 * pSaveBuff )
{
    uint8 counter = 0;

    for( ; counter < u8ByteTotal; counter++ )
    {
        //�д����Ż�
        *( pSaveBuff + counter + ( 4 - u8ByteTotal ) ) = g_stu8CacheData[u8DataStreamCurId].u8Buf[u8StartByte + counter];
    }
}
#endif


/*
********************************************************************************
Name        :void *pfun_data_stream_all_Obd(void* pArg0, void* pArg1).
Description:pfun_data_stream_all_car
Input      :
Output     :none
Return     :none
Others     :��̬ʵʱ������
********************************************************************************
*/
#if RDATA_STREAM_EN == 1
void *pfun_data_stream_all_Obd( void* pArg0, void *pu8FunctionParameter )
{
    void *pCommStatus0 = NULL;
    uint8 u8Error;
    //u8DataStreamCmdIdOffSet����ƫ����
    //u8DataStreamCmdId������ͨ�������������ֵ
    //stDataStreamIdBuff�洢ϵͳID��
    TYPEDEF_DATASTREAM_LIST *pstDataStreamIdBuff = NULL;
    uint8  u8DataStreamCmdIdOffSet = 0;
    uint8  u8DataStreamCmdId[] = {1, 0};
    uint8  u8DataStreamCmdIdtmp[20] = {0};
    uint16 u16DataStreamDataCacheAddr = 0;
    uint8 u8TimeCounter = 0;
    uint8  u8DataStreamIdLen = 0;
    //Ŀǰ���ֻ�ܴ洢11��������
    TYPEDEF_DATASTREAM_LIST  stDataStreamIdBuff[11] = {0};

    //��ȡ����һ�����͵�������
    uint8 u8GetDataStreamchoice = ( ( uint8* )pu8FunctionParameter )[0];

    //ֱ��ָ��ú�����Ҫ���ϵı�����
    const TYPEDEF_DATASTREAM_GET_DATA_LIST *pstDataStream = g_pstDataStreamLib[u8GetDataStreamchoice].pstGetdataList;
    //6s�Ӳ���һ�����
    fun_time_init( TIM2, 6000 );
    //������ʱ��
    TIM2->CR1 |= 1;
    //���ú������ַ
    pstDataStreamIdBuff = stDataStreamIdBuff;

    //������ֹͣ�����Ƿ����
    while( g_stPhoneCommandShare.u8ReadDataStream == DEF_DATASTREAM_STATUS_START )
    {
        //�ж��Ƿ���Ҫ��̬����,ע������Ƚϵ�����������Ŀ,������һ�����¼���,���Ȳ�һ��Ҳ�����¼���
        if(
            ( 0X00 != memcmp( &u8DataStreamCmdIdtmp[1], &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamIdBuf, ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Idlen ) ) |
            ( u8DataStreamIdLen != ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Idlen )
        )
        {
            u8DataStreamIdLen = ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8Idlen;
            //ע����������е�С��������������� �Ӷ��Сʱ�᲻���������
            memcpy( &u8DataStreamCmdIdtmp[1], &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamIdBuf, u8DataStreamIdLen );
            //���복��
            u8DataStreamCmdIdtmp[0] = 35;

            //�ж��ǲ��ǳ���11
            if( u8DataStreamIdLen + 1 > DEF_DATASTREAM_ID_TOTAL_MAX + 2 )
            {
                //˵����������Ŀ����12
                g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_OPTION_OVER;
                //�رն�ʱ��
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                break;
            }

            pfun_init_data_Stream_idlist0( stDataStreamIdBuff, 11, u8DataStreamIdLen + 1, u8DataStreamCmdIdtmp );
        }

        //�ж��ǲ��ǳ���103��
        if( ( pstDataStreamIdBuff->DataStreamId.u8Bit[0] > DEF_DATASTREAM_ID_OFFSET_MAX ) )
        {
            //˵����������Ŀ����103
            g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_OPTION_OVER;
            //�رն�ʱ��
            TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
            break;
        }

        //���Ͷ�����������,�м��������е�����������ƫ��4��
        u8DataStreamCmdId[1] = pstDataStream[pstDataStreamIdBuff->DataStreamId.u8Bit[0] + 4].u8CmdIndex;
        u8DataStreamCmdIdOffSet = u8DataStreamCmdId[1];
        //�����ڵ�����ص������������ж��ǲ����Ѿ���ʱ
        OSMboxPost( g_pstAnalyzerToPerformer, u8DataStreamCmdId );
        pCommStatus0 = OSMboxPend( g_pstPerformerToAnalyzer, 0, &u8Error );

        //STEP0:�ж��Ƿ�ɹ�
        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus0 != OK )
        {
            //��������˵����ͨѶʧ�ܵ�����
            if( 35 == pstDataStreamIdBuff->DataStreamId.u8Bit[0] )
            {
                //˵�����ٲ�����,����ͨѶʧ��
                g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_DATABUS_FAILURE;
                //�رն�ʱ��
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                break;
            }

            else
            {
                memset( &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr], 0, 5 );
                u16DataStreamDataCacheAddr += 1;
            }
        }

        else
        {
            //��Ϊ������ID�Ǵ�0��ʼ
            ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr++] =
                pstDataStreamIdBuff->DataStreamId.u8Bit[0];
            //ͬʱ��ȡ�������ؼ��ֽ�
            pfun_get_data_stream_calculation_bytes
            (
                pstDataStream[pstDataStreamIdBuff->DataStreamId.u8Bit[0] + 4].u8StartByte,
                pstDataStream[pstDataStreamIdBuff->DataStreamId.u8Bit[0] + 4].u8ByteTotal,
                ( ( ( g_pModeLibTab->pCmdTabChoice )[u8DataStreamCmdIdOffSet] ).u8BufferId ) & 0x0f,
                &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr]
            );

            //�жϳ����Ƿ�����
            if( 35 == pstDataStreamIdBuff->DataStreamId.u8Bit[0] )
            {
                //˵�����ٴ���0
                if( ( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[u16DataStreamDataCacheAddr + 3] != 0x00 )
                {
                    TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                    //˵�����ٴ���0
                    g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_SPEED_STOP;
                    break;
                }
            }
        }

        //�ж��ǲ����Ѿ���6s
        if( ( TIM2->SR & ( uint16 ) 1 ) == 1 )
        {
            //�رն�ʱ��
            TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );

            //�ж�ʱ���ǲ����Ѿ�����
            if( ++u8TimeCounter > 100 )
            {
                //�رն�ʱ��
                TIM2->CR1 &= ( uint16 )( ~( ( uint16 ) 1 ) );
                //˵��ͨѶʱ���Ѿ���
                g_stPhoneCommandShare.u8ReadDataStream = DEF_DATASTREAM_STATUS_TIMEOUT_STOP;
                break;
            }

            else
            {
                //6s�Ӳ���һ�����
                fun_time_init( TIM2, 6000 );
                //������ʱ��
                TIM2->CR1 |= 1;
            }
        }

        //step3:�����ַ��ʼ��1
        u16DataStreamDataCacheAddr += 4;
        //step4:ȡ��һ����ɢID
        pstDataStreamIdBuff = pstDataStreamIdBuff->Next;

        //�ж��ǲ����Ѿ�����ͷ
        if( stDataStreamIdBuff == pstDataStreamIdBuff )
        {
            if( g_stObdData.dataLock == DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
            {
                g_stObdData.dataLock = DEF_CMD_PROTOCOL_MANUAL_LCOK;
                g_stObdData.proParaLen = 0;
                g_stObdData.retCode = 00;
                //�����ʶ
                g_stObdData.proCmdId = 0x0289;
                g_stObdData.proParaBuf[g_stObdData.proParaLen] = 01;//�����ʽ
                g_stObdData.proParaLen += 1;
                memcpy( ( uint8* )&g_stObdData.proParaBuf[g_stObdData.proParaLen],
                        &( ( TYPEDEF_DATASTREAM_MSGBUF* )( g_stFunMsgBuf[1].pu8FunBuffer ) )->u8DataStreamDataBuf[5], u8DataStreamIdLen * 5 );
                g_stObdData.proParaLen += u8DataStreamIdLen * 5;

                while( g_stObdData0.dataLock != DEF_CMD_PROTOCOL_MANUAL_UNLCOK )
                {
                    OSTimeDlyHMSM( 0, 0, 0, 10 );
                }

                OSMboxPost( OS_OBDAckMbox, &g_stObdData );
            }

            //���ݻ����01��ʼ
            u16DataStreamDataCacheAddr = 0;
        }

    }

    return( ( void * )& g_enumFrameFlgArr[2] );
}

#endif
