/*
********************************************************************************
File name  : ZXT_read_dtc.c
Description: ��ȡ�����뺯����
History    :
�޸�����   : �޸��� : �޸�����:������
2012-4-19    С����   ������ȡ�����뺯����
�޸�����   : �޸��� : �޸�����:������
********************************************************************************
*/

#define READ_DTC_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
********************************************************************************
Name        :pfun_allcar_rdtc_witch_total
Description:�ú�����Ҫ�Ǵ���������Ĺ�����
Input      :none
Output     :none
Return     :none
Others     :1.ע����������u8FunctionParameter0�еĲ���
2.�ú�����Ҫ�Ǵ��������,�������������Ե�
********************************************************************************
*/

#if  RDTC_WITCH_TOTAL_EN == 1

void *pfun_allcar_rdtc_witch_total( void *pstsysChoice, void *pu8FunctionParameter )
{

    //u16DtcCodeLenʵ�ʹ������ܳ���
    //u8GetDtcTotalMode����������ģʽ
    //pCmdTmp��������ֵ(���Լ��ϵ�ǰ����ʷ,�����ǵ���һ��)
    void *pCommStatus = NULL;
    uint16 u16DtcCodeLen = 0;
    uint8  u8GetDtcTotalMode = 0;
    uint8 *pCmdTmp = ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdRDtc;

    //��ȡ���ϴ���ģʽ��0��������ģʽ
    u8GetDtcTotalMode = ( ( uint8* )pu8FunctionParameter )[0];

    //��ȡ������
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //���ͨѶ��ʱֱ����������
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //���������,������Ч���ݳ����Ժ��������
    //7e 0c xx xx 00����5���ֽ�

    u16DtcCodeLen = fun_rdtc_deal_with_total( u8GetDtcTotalMode, ( ( TYPEDEF_SYS_CFG* )pstsysChoice )->u8SysLibChoice, &g_stFunMsgBuf[0].pu8FunBuffer[g_stFunMsgBuf[0].u16Len] );

    //�����Ѿ��洢�ĳ���,Ϊ�´δ洢����׼��
    //����й�����Ż�ȥ���ĳ���
    if( u16DtcCodeLen )
    {
        g_stFunMsgBuf[0].u16Len += u16DtcCodeLen + 3;
        g_stFunMsgBuf[0].pu8FunBuffer[0]++;
    }

    //����ͨѶ״̬
    return ( pCommStatus );
}

#endif

/*
********************************************************************************
Name        :pfun_allcar_rdtc_witch_nototal
Description:�ú�����Ҫ�ǲ���������ȡ��ǰ������
Input      :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :1.ע��((*g_pModeLibTab).pFunVarInit)[1]��
2.�ú�����Ҫ�Ǵ������Ĳ������������Թ���������OBD-K
********************************************************************************
*/

#if  RDTC_WITCH_NOTOTAL_EN == 1

void *pfun_allcar_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter )
{

    //u16DtcCodeLenʵ�ʹ������ܳ���
    //u8GetDtcMode��ȡ������ģʽ
    //u8GetDtcModeTemp��ȡ������ģʽ
    //u8GetDtcModeCounter��ȡ�����뷽ʽ������
    //pCmdTmp��������ֵ(���Լ��ϵ�ǰ����ʷ,�����ǵ���һ��)
    void *pCommStatus;
    uint16 u16DtcCodeLen = 0;
    uint8 u8GetDtcTotalMode = 0;
    uint8 *pCmdTmp = ( uint8* )( ( *g_pModeLibTab ).pCmdIndexTabChoice )->pCmdRDtc;

    //��ȡ���ϴ���ģʽ��0��������ģʽ
    u8GetDtcTotalMode = ( ( uint8* )pu8FunctionParameter )[0];

    //��ȡ������
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //���ͨѶ��ʱֱ����������
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //���������,������Ч���ݳ���
    u16DtcCodeLen = fun_rdtc_deal_with_nototal( u8GetDtcTotalMode, ( ( TYPEDEF_SYS_CFG* )pstsysChoice )->u8SysLibChoice , &g_stFunMsgBuf[0].pu8FunBuffer[g_stFunMsgBuf[0].u16Len] );

    //�����Ѿ��洢�ĳ���,Ϊ�´δ洢����׼��
    if( u16DtcCodeLen )
    {
        g_stFunMsgBuf[0].u16Len += u16DtcCodeLen + 3;
        g_stFunMsgBuf[0].pu8FunBuffer[0]++;
    }

    //����ͨѶ״̬
    return ( pCommStatus );
}

#endif
/*
********************************************************************************
Name        :pfun_buck_rdtc_witch_nototal
Description:�ú�����Ҫ�����ͨ�ñ�˳�һ֡��Ӧһ����������д���
Input      :void *pArg0,void *pArg1��2��������û������
Output     :none
Return     :none
Others     :
1.�ú�����Ҫ�Ǵ���һ֡һ��������(�ж����������Ӧ��֡),������λ�ÿ��Բ��̶���
2.1 ����:81 c0 73 00 49 00 00 00
2.ǰ����֡���ȹ̶�����8������
********************************************************************************
*/

#if  RDTC_GM_BUCK_WITCH_NOTOTAL_EN == 1

void *pfun_buck_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter )
{

    //u16DtcCodeLenʵ�ʹ������ܳ���
    //u8GetDtcModeCounter��ȡ�����뷽ʽ������
    //pCmdTmp��������ֵ(���Լ��ϵ�ǰ����ʷ,�����ǵ���һ��)
    void *pCommStatus;
    uint16 u16DtcCodeLen = 0;
    uint8 *pCmdTmp = ( uint8* )( ( *g_pModeLibTab ).pCmdIndexTabChoice )->pCmdRDtc;

    //��ȡ������
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //���ͨѶ��ʱֱ����������
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //���������,������Ч���ݳ���
    u16DtcCodeLen =
        fun_buck_rdtc_deal_with_nototal( ( ( TYPEDEF_SYS_CFG* )pstsysChoice )->u8SysLibChoice , &g_stFunMsgBuf[0].pu8FunBuffer[g_stFunMsgBuf[0].u16Len] );

    //�����Ѿ��洢�ĳ���,Ϊ�´δ洢����׼��
    //����й�����Ż�ȥ���ĳ���
    if( u16DtcCodeLen )
    {
        g_stFunMsgBuf[0].u16Len += u16DtcCodeLen + 3;
        g_stFunMsgBuf[0].pu8FunBuffer[0]++;
    }

    //����ͨѶ״̬
    return ( pCommStatus );
}

/*
*******************************************************************************
Name        :fun_buck_rdtc_deal_with_nototal
Description:�ú�����Ҫ�Ǵ���ͨ�ñ�˳�������
Input      :none
Output     :none
Return     :��������������
Others     :�ú�����Ҫ�Ǵ���ͨ�ñ�˹����뺯��
********************************************************************************
*/

//7E8 08  03 7f a9 78 00 00 00 00
//5E8 08  81 (00 00 01) ff 00 00 00
//5E8 08  81 (00 00 00) ff 00 00 00

uint16 fun_buck_rdtc_deal_with_nototal( uint8 u8SysChoice, uint8 *pCache )
{

    //���������������ַuint8 u8DtcTotal
    //��������Ч�ֽ�u8DtcCodevalidByte
    //�����ֽڴ���һ��������u8DtcCodeXXByteOneDtc
    //pDtcWithTotalConfigLib��ȡ��ʼ���ú����ı�����ַ
    //u8DtcCodeCounter�����������
    uint8 u8DtcTotal = 0;
    uint8 *pDtcCodeStart = NULL;


    //�ҵ������뿪ʼ��ַ,�м������Ѿ�д������
    pDtcCodeStart = g_stu8CacheData[0].u8Buf;

    //�жϽ��յ��������ǲ���7f 11 12 21
    //0x10
    //0x11
    //0x12
    //0x21
    //0x22
    if
    (
        ( pDtcCodeStart[0] == 0x7f ) &
        (
            ( pDtcCodeStart[2] == 0x10 ) | ( pDtcCodeStart[2] == 0x11 ) |
            ( pDtcCodeStart[2] == 0x12 ) | ( pDtcCodeStart[2] == 0x21 )
        )

    )
    {
        return 0;
    }

    //�����7f 78ϵ��������ô�ͽ���������һ��
    if( ( pDtcCodeStart[0] == 0x7f ) & ( pDtcCodeStart[2] == 0x78 ) )
    {
        //������1
        g_stCAN115765ReceiveVar.u8FrameTotal--;
        //����ָ����Ч����
        pDtcCodeStart += 7;
    }

    //������յ�һ֡ 7F 78 ��ôֱ������ȥ
    if( g_stCAN115765ReceiveVar.u8FrameTotal == 0 )
    {
        return 0;
    }

    //�жϵ�һ֡�ǲ���ȫ��Ϊ0,�������֮��ĵ�һ֡
    //81 00 00 00 ff 00 00 00
    if( ( pDtcCodeStart[0] | pDtcCodeStart[1] | pDtcCodeStart[2] ) == 0 )
    {
        return 0;
    }

    //��Ϊ�й��������ٻ�2֡
    if( g_stCAN115765ReceiveVar.u8FrameTotal >= 2 )
    {
        //�����һ֡ȥ��
        g_stCAN115765ReceiveVar.u8FrameTotal--;
    }

    //ϵͳ����,ע��ϵͳ�����Ǵ�1��ʼ
    *pCache++ = u8SysChoice + 1;
    //����,�����Ѿ�д������
    *pCache++ = 6;
    //�洢�ù���������=֡����
    *pCache++ = g_stCAN115765ReceiveVar.u8FrameTotal;

    //װ�ع�����
    for( u8DtcTotal = 0; u8DtcTotal < g_stCAN115765ReceiveVar.u8FrameTotal; u8DtcTotal++ )
    {
        *pCache++ = 0;
        memcpy( pCache, pDtcCodeStart, 3 );
        pCache += 3;
        pDtcCodeStart += 7;
    }

    //�����������ʵ��ռ���ֽ���=����������X4
    //��ΪЭ�������ôȷ����
    return ( g_stCAN115765ReceiveVar.u8FrameTotal * 4 );
}

#endif
/*
*******************************************************************************
Name       :pfun_vw_pusang_rdtc_witch_nototal(void *pstsysChoice, void *pu8FunctionParameter)
Description:�ú�����Ҫ�Ǵ�����ɣ����ȡ������(KW1281)
Input      :none
Output     :none
Return     :none
Others     :
1.06 xx FC FF FF 88 03����ϵͳ����
********************************************************************************
*/
#if RDTC_VW_PUSANG_WITCH_NOTOTAL_EN==1
void *pfun_vw_pusang_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter )
{
    //u16DtcCodeLenʵ�ʹ������ܳ���
    //u8GetDtcMode��ȡ������ģʽ
    //u8GetDtcModeTemp��ȡ������ģʽ
    //u8GetDtcModeCounter��ȡ�����뷽ʽ������
    //pCmdTmp��������ֵ(���Լ��ϵ�ǰ����ʷ,�����ǵ���һ��)
    void *pCommStatus;
    uint16 u16DtcCodeLen = 0;
    uint8 u8GetDtcTotalMode = 0;
    uint8 *pCmdTmp = ( uint8* )( ( *g_pModeLibTab ).pCmdIndexTabChoice )->pCmdRDtc;

    //��ȡ���ϴ���ģʽ��0��������ģʽ
    u8GetDtcTotalMode = ( ( uint8* )pu8FunctionParameter )[0];

    //��ȡ������
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //���ͨѶ��ʱֱ����������
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //���������,������Ч���ݳ���
    u16DtcCodeLen =
        fun_vw_pusang_rdtc_deal_with_nototal(
            u8GetDtcTotalMode,
            ( ( TYPEDEF_SYS_CFG* )pstsysChoice )->u8SysLibChoice ,
            &g_stFunMsgBuf[0].pu8FunBuffer[g_stFunMsgBuf[0].u16Len]
        );

    //�����Ѿ��洢�ĳ���,Ϊ�´δ洢����׼��
    //����й�����Ż�ȥ���ĳ���
    if( u16DtcCodeLen )
    {
        g_stFunMsgBuf[0].u16Len += u16DtcCodeLen + 3;
        g_stFunMsgBuf[0].pu8FunBuffer[0]++;
    }

    //����ͨѶ״̬
    return ( pCommStatus );
}

/*
*******************************************************************************
Name        :fun_vw_pusang_rdtc_deal_with_nototal
Description:�ú�����Ҫ�Ǵ�����ɣ����ȡ������(KW1281)
Input      :none
Output     :none
Return     :��������������
Others     :��Ҫ�Ǵ���kw1281û�������Ĺ�������������
********************************************************************************
*/

uint16 fun_vw_pusang_rdtc_deal_with_nototal( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache )
{

    //���������������ַuint8 u8DtcTotal
    //������ſ�ʼ��ַ  uint8 *pDtcCodeStart
    //��������Ч�ֽ�u8DtcCodevalidByte
    //�����ֽڴ���һ��������u8DtcCodeXXByteOneDtc
    //pDtcWithTotalConfigLib��ȡ��ʼ���ú����ı�����ַ
    //u8DtcCodeCounter�����������
    const TYPEDEF_DTC_NOTOTAL *pDtcWithNoTotalConfigLib;
    uint8 u8DtcTotal = 0;
    uint8 u8DtcCodevalidByte = 0;
    uint8 u8DtcCodeXXByteOneDtc = 0;
    uint8 *pDtcCodeStart = NULL;
    pDtcWithNoTotalConfigLib = g_stDtcWithNoTotalConfigLib + u8GetDtcModeTemp;

    u8DtcCodevalidByte = pDtcWithNoTotalConfigLib->u8DtcCodevalidByte;
    u8DtcCodeXXByteOneDtc = pDtcWithNoTotalConfigLib->u8DtcCodeXXByteOneDtc;

    //����Ҫ�ж���Ч�ֽڵĳ����Ƿ��㹻Ȼ����ܼ��������������ٵ���2���ֽ�,����һ���ֽڴ��������
    if( g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u16Len < 2 )
    {
        return 0;
    }

    //�������������
    u8DtcTotal = ( g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u16Len
                   - pDtcWithNoTotalConfigLib->u8DtcCodeStartByte ) / u8DtcCodeXXByteOneDtc;

    //�������Ϊ0,��ô�Ͳ���������
    if( !u8DtcTotal )
    {
        return 0;
    }

    //�ҵ������뿪ʼ��ַ
    pDtcCodeStart = g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u8Buf
                    + pDtcWithNoTotalConfigLib->u8DtcCodeStartByte;

    //�жϽ��յ�������06 xx xx FF FF 88 03,����������˵��ϵͳ����
    if
    (
        ( 0XFF == pDtcCodeStart[0] ) &
        ( 0XFF == pDtcCodeStart[1] ) &
        ( 0X88 == pDtcCodeStart[2] )
    )
    {
        return 0;
    }

    //ϵͳ����,ע��ϵͳ�����Ǵ�1��ʼ
    *pCache++ = u8SysChoice + 1;
    //����
    *pCache++ = pDtcWithNoTotalConfigLib->u8DtcCodeLibChoice;
    //�洢�ù���������
    *pCache++ = u8DtcTotal;

    //װ�ع�����
    fun_rdtc_load_dtc( u8DtcTotal, u8DtcCodeXXByteOneDtc, u8DtcCodevalidByte, pCache, pDtcCodeStart );

    //�����������ʵ��ռ���ֽ���=����������X4
    //��ΪЭ�������ôȷ����
    return ( u8DtcTotal * 4 );
}

#endif
/*
*******************************************************************************
Name        :fun_rdtc_deal_with_total
Description:�ú������������
Input      :uint8 u8GetDtcModeTemp�ñ�����Ҫ������ѡ��������ȡ��ʽ��
uint8 *pCache�������뱣��������
Output     :none
Return     :ʵ�ʹ���������+����+����+Ҳ�����ϴ�����Ч���ݳ���
Others     :ʹ�����1:K��2����������,��һ����ĳһ���ֽڴ�������,�ڶ�����ʼ�ǹ���
��(��ʱ��֧��)
ʹ�����2:K��1����������,�����͹�����Ŷ�����ͬһ�����ݻ����С�
********************************************************************************
*/

#if RDTC_DEAL_WITH_TOTAL_EN==1

uint16 fun_rdtc_deal_with_total( uint8 u8GetDtcModeTemp, uint8 u8SysChoice , uint8 *pCache )
{

    //���������������ַuint8 u8DtcTotal
    //������ſ�ʼ��ַ  uint8 *pDtcCodeStart
    //��������Ч�ֽ�u8DtcCodevalidByte
    //�����ֽڴ���һ��������u8DtcCodeXXByteOneDtc
    //pDtcWithTotalConfigLib��ȡ��ʼ���ú����ı�����ַ
    const TYPEDEF_DTC_WITHITOTAL *pDtcWithTotalConfigLib;
    uint8 u8DtcTotal = 0;
    uint8 u8DtcCodevalidByte = 0;
    uint8 u8DtcCodeXXByteOneDtc = 0;
    uint8 *pDtcCodeStart = NULL;
    pDtcWithTotalConfigLib = g_stDtcWithTotalConfigLib + u8GetDtcModeTemp;

    u8DtcCodevalidByte = pDtcWithTotalConfigLib->u8DtcCodevalidByte;
    u8DtcCodeXXByteOneDtc = pDtcWithTotalConfigLib->u8DtcCodeXXByteOneDtc;

    //�ҵ�����������
    u8DtcTotal = g_stu8CacheData[pDtcWithTotalConfigLib->u8DtcTotalCacheAddr]
                 .u8Buf[pDtcWithTotalConfigLib->u8DtcTotalByte];

    //�������Ϊ0,��ô�Ͳ���������
    if( !u8DtcTotal )
    {
        return 0;
    }

    //ϵͳ����,ע��ϵͳ�����Ǵ�1��ʼ��
    *pCache++ = u8SysChoice + 1;
    //����
    *pCache++ = pDtcWithTotalConfigLib->u8DtcCodeLibChoice;
    //�洢�ù���������
    *pCache++ = u8DtcTotal;
    //�ҵ������뿪ʼ��ַ
    pDtcCodeStart = g_stu8CacheData[pDtcWithTotalConfigLib->u8DtcCodeCacheAddr].u8Buf
                    + pDtcWithTotalConfigLib->u8DtcCodeStartByte;
    //װ�ع�����
    fun_rdtc_load_dtc( u8DtcTotal, u8DtcCodeXXByteOneDtc, u8DtcCodevalidByte, pCache, pDtcCodeStart );

    //���ع���������
    return ( u8DtcTotal * 4 );

}

#endif

/*
*******************************************************************************
Name        :fun_rdtc_deal_with_nototal
Description:�ú�������������
Input      :none
Output     :none
Return     :��������������
Others     :��Ҫ�Ǵ���û�������Ĺ�������������
********************************************************************************
*/

#if RDTC_DEAL_WITH_NOTOTALL_EN==1

uint16 fun_rdtc_deal_with_nototal( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache )
{

    //���������������ַuint8 u8DtcTotal
    //������ſ�ʼ��ַ  uint8 *pDtcCodeStart
    //��������Ч�ֽ�u8DtcCodevalidByte
    //�����ֽڴ���һ��������u8DtcCodeXXByteOneDtc
    //pDtcWithTotalConfigLib��ȡ��ʼ���ú����ı�����ַ
    //u8DtcCodeCounter�����������
    const TYPEDEF_DTC_NOTOTAL *pDtcWithNoTotalConfigLib;
    uint8 u8DtcTotal = 0;
    uint8 u8DtcCodevalidByte = 0;
    uint8 u8DtcCodeXXByteOneDtc = 0;
    uint8 *pDtcCodeStart = NULL;
    uint8 u8DtcCodeCounter = 0;
    pDtcWithNoTotalConfigLib = g_stDtcWithNoTotalConfigLib + u8GetDtcModeTemp;

    u8DtcCodevalidByte = pDtcWithNoTotalConfigLib->u8DtcCodevalidByte;
    u8DtcCodeXXByteOneDtc = pDtcWithNoTotalConfigLib->u8DtcCodeXXByteOneDtc;

    //����Ҫ�ж���Ч�ֽڵĳ����Ƿ��㹻Ȼ����ܼ��������������ٵ���2���ֽ�,����һ���ֽڴ��������
    if( g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u16Len < 2 )
    {
        return 0;
    }

    //�������������
    u8DtcTotal = ( g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u16Len
                   - pDtcWithNoTotalConfigLib->u8DtcCodeStartByte ) / u8DtcCodeXXByteOneDtc;

    //�������Ϊ0,��ô�Ͳ���������
    if( !u8DtcTotal )
    {
        return 0;
    }

    //�ҵ������뿪ʼ��ַ
    pDtcCodeStart = g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u8Buf
                    + pDtcWithNoTotalConfigLib->u8DtcCodeStartByte;

    //�жϽ��յ������ǲ���ȫ��Ϊ0,ȫ��Ϊ0,������
    for( u8DtcCodeCounter = 0; u8DtcCodeCounter < u8DtcTotal * u8DtcCodeXXByteOneDtc; u8DtcCodeCounter++ )
    {
        //�ж��ǲ���0,ֻҪ�з�0�ʹ����й�����
        if( 0x00 != pDtcCodeStart[u8DtcCodeCounter] )
        {
            break;
        }

        //���ȫ��Ϊ0����ô�͵�ֱ������ȥ
        if( u8DtcCodeCounter + 1 == u8DtcTotal * u8DtcCodeXXByteOneDtc )
        {
            return 0;
        }
    }

    //ϵͳ����,ע��ϵͳ�����Ǵ�1��ʼ
    *pCache++ = u8SysChoice + 1;
    //����
    *pCache++ = pDtcWithNoTotalConfigLib->u8DtcCodeLibChoice;
    //�洢�ù���������
    *pCache++ = u8DtcTotal;

    //װ�ع�����
    fun_rdtc_load_dtc( u8DtcTotal, u8DtcCodeXXByteOneDtc, u8DtcCodevalidByte, pCache, pDtcCodeStart );

    //�����������ʵ��ռ���ֽ���=����������X4
    //��ΪЭ�������ôȷ����
    return ( u8DtcTotal * 4 );
}

#endif

/*
*******************************************************************************
Name        :fun_rdtc_load_dtc
Description:�ú�����Ҫ�Ǵ��������,��������װ�ع����뵽ָ������
Input      :u8DtcTotal���������� u8DtcCodeXXByteOneDtc�����ֽڴ���һ��������
u8DtcCodevalidByte��������Ч�ֽڸ��� pCache��������洢������
pDtcCodeStartȡ�����뿪ʼ��ַ
Output     :none
Return     :none
Others     :
********************************************************************************
*/

#if RDTC_LOAD_DTC_EN==1

void fun_rdtc_load_dtc( uint8 u8DtcTotal, uint8 u8DtcCodeXXByteOneDtc,
                        uint8 u8DtcCodevalidByte, uint8 *pCache, uint8 *pDtcCodeStart )
{
    //���ڹ������ϴ��Ĺ涨,4���ֽ��е�ǰ3���ǹ����룬����һ����״̬λ

    uint8 u8DtcCodeSavePid = 0;

    switch( u8DtcCodevalidByte )
    {
        //�����һ���ֽ�һ��������û�м���
        case 1:
            {
                return;
            }

        case 2:
            {
                u8DtcCodeSavePid = 1;
                break;
            }

        case 3:
            {
                u8DtcCodeSavePid = 1;
                break;
            }

        //�����һ���ֽ�һ��������Ҳû�м���
        case 4:
            {
                u8DtcCodeSavePid = 0;
                break;
            }

        default:
            break;
    }

    while( u8DtcTotal-- )
    {
        uint8 i = 0;

        for( ; i < u8DtcCodevalidByte; i++ )
        {
            //���ﻹ��ע��һ��,2���ֽ�һ���������3���ֽ�һ�����϶��Ǵ�a[]={00,xx,xx,xx}��ʼ
            *( uint8* )( pCache + i + u8DtcCodeSavePid ) = * ( pDtcCodeStart + i );
        }

        pDtcCodeStart = pDtcCodeStart + u8DtcCodeXXByteOneDtc;
        pCache += 4;
    }
}

#endif






