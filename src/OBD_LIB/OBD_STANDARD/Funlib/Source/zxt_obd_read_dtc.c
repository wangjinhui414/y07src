/*
********************************************************************************
File name  : ZXT_read_dtc.c
Description: 读取故障码函数库
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立读取故障码函数库
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/

#define READ_DTC_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
********************************************************************************
Name        :pfun_allcar_rdtc_witch_total
Description:该函数主要是处理带总数的故障码
Input      :none
Output     :none
Return     :none
Others     :1.注意这里用上u8FunctionParameter0中的参数
2.该函数主要是处理带总数,故障码是连续性的
********************************************************************************
*/

#if  RDTC_WITCH_TOTAL_EN == 1

void *pfun_allcar_rdtc_witch_total( void *pstsysChoice, void *pu8FunctionParameter )
{

    //u16DtcCodeLen实际故障码总长度
    //u8GetDtcTotalMode故障码总数模式
    //pCmdTmp命令索引值(可以加上当前和历史,或者是单独一个)
    void *pCommStatus = NULL;
    uint16 u16DtcCodeLen = 0;
    uint8  u8GetDtcTotalMode = 0;
    uint8 *pCmdTmp = ( uint8* )g_pModeLibTab->pCmdIndexTabChoice->pCmdRDtc;

    //获取故障处理模式第0参数代表模式
    u8GetDtcTotalMode = ( ( uint8* )pu8FunctionParameter )[0];

    //读取故障码
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //如果通讯超时直接跳出函数
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //处理故障码,返回有效数据长度以后可能用上
    //7e 0c xx xx 00跳过5个字节

    u16DtcCodeLen = fun_rdtc_deal_with_total( u8GetDtcTotalMode, ( ( TYPEDEF_SYS_CFG* )pstsysChoice )->u8SysLibChoice, &g_stFunMsgBuf[0].pu8FunBuffer[g_stFunMsgBuf[0].u16Len] );

    //更改已经存储的长度,为下次存储做好准备
    //如果有故障码才会去更改长度
    if( u16DtcCodeLen )
    {
        g_stFunMsgBuf[0].u16Len += u16DtcCodeLen + 3;
        g_stFunMsgBuf[0].pu8FunBuffer[0]++;
    }

    //返回通讯状态
    return ( pCommStatus );
}

#endif

/*
********************************************************************************
Name        :pfun_allcar_rdtc_witch_nototal
Description:该函数主要是不带总数读取当前故障码
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :1.注意((*g_pModeLibTab).pFunVarInit)[1]用
2.该函数主要是处理常见的不带总数连续性故障码例如OBD-K
********************************************************************************
*/

#if  RDTC_WITCH_NOTOTAL_EN == 1

void *pfun_allcar_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter )
{

    //u16DtcCodeLen实际故障码总长度
    //u8GetDtcMode获取故障码模式
    //u8GetDtcModeTemp获取故障码模式
    //u8GetDtcModeCounter获取故障码方式计数器
    //pCmdTmp命令索引值(可以加上当前和历史,或者是单独一个)
    void *pCommStatus;
    uint16 u16DtcCodeLen = 0;
    uint8 u8GetDtcTotalMode = 0;
    uint8 *pCmdTmp = ( uint8* )( ( *g_pModeLibTab ).pCmdIndexTabChoice )->pCmdRDtc;

    //获取故障处理模式第0参数代表模式
    u8GetDtcTotalMode = ( ( uint8* )pu8FunctionParameter )[0];

    //读取故障码
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //如果通讯超时直接跳出函数
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //处理故障码,返回有效数据长度
    u16DtcCodeLen = fun_rdtc_deal_with_nototal( u8GetDtcTotalMode, ( ( TYPEDEF_SYS_CFG* )pstsysChoice )->u8SysLibChoice , &g_stFunMsgBuf[0].pu8FunBuffer[g_stFunMsgBuf[0].u16Len] );

    //更改已经存储的长度,为下次存储做好准备
    if( u16DtcCodeLen )
    {
        g_stFunMsgBuf[0].u16Len += u16DtcCodeLen + 3;
        g_stFunMsgBuf[0].pu8FunBuffer[0]++;
    }

    //返回通讯状态
    return ( pCommStatus );
}

#endif
/*
********************************************************************************
Name        :pfun_buck_rdtc_witch_nototal
Description:该函数主要是针对通用别克车一帧回应一个故障码进行处理
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :
1.该函数主要是处理一帧一个故障码(有多个故障码会回应多帧),故障码位置可以不固定的
2.1 例如:81 c0 73 00 49 00 00 00
2.前提是帧长度固定都是8个数据
********************************************************************************
*/

#if  RDTC_GM_BUCK_WITCH_NOTOTAL_EN == 1

void *pfun_buck_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter )
{

    //u16DtcCodeLen实际故障码总长度
    //u8GetDtcModeCounter获取故障码方式计数器
    //pCmdTmp命令索引值(可以加上当前和历史,或者是单独一个)
    void *pCommStatus;
    uint16 u16DtcCodeLen = 0;
    uint8 *pCmdTmp = ( uint8* )( ( *g_pModeLibTab ).pCmdIndexTabChoice )->pCmdRDtc;

    //读取故障码
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //如果通讯超时直接跳出函数
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //处理故障码,返回有效数据长度
    u16DtcCodeLen =
        fun_buck_rdtc_deal_with_nototal( ( ( TYPEDEF_SYS_CFG* )pstsysChoice )->u8SysLibChoice , &g_stFunMsgBuf[0].pu8FunBuffer[g_stFunMsgBuf[0].u16Len] );

    //更改已经存储的长度,为下次存储做好准备
    //如果有故障码才会去更改长度
    if( u16DtcCodeLen )
    {
        g_stFunMsgBuf[0].u16Len += u16DtcCodeLen + 3;
        g_stFunMsgBuf[0].pu8FunBuffer[0]++;
    }

    //返回通讯状态
    return ( pCommStatus );
}

/*
*******************************************************************************
Name        :fun_buck_rdtc_deal_with_nototal
Description:该函数主要是处理通用别克车故障码
Input      :none
Output     :none
Return     :故障码总数长度
Others     :该函数主要是处理通用别克故障码函数
********************************************************************************
*/

//7E8 08  03 7f a9 78 00 00 00 00
//5E8 08  81 (00 00 01) ff 00 00 00
//5E8 08  81 (00 00 00) ff 00 00 00

uint16 fun_buck_rdtc_deal_with_nototal( uint8 u8SysChoice, uint8 *pCache )
{

    //故障码总数缓存地址uint8 u8DtcTotal
    //故障码有效字节u8DtcCodevalidByte
    //几个字节代表一个故障码u8DtcCodeXXByteOneDtc
    //pDtcWithTotalConfigLib获取初始化该函数的变量地址
    //u8DtcCodeCounter故障码计数器
    uint8 u8DtcTotal = 0;
    uint8 *pDtcCodeStart = NULL;


    //找到故障码开始地址,切记这里已经写死啦的
    pDtcCodeStart = g_stu8CacheData[0].u8Buf;

    //判断接收到的数据是不是7f 11 12 21
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

    //如果是7f 78系列数据那么就将总数减少一个
    if( ( pDtcCodeStart[0] == 0x7f ) & ( pDtcCodeStart[2] == 0x78 ) )
    {
        //总数减1
        g_stCAN115765ReceiveVar.u8FrameTotal--;
        //重新指向有效数据
        pDtcCodeStart += 7;
    }

    //如果接收到一帧 7F 78 那么直接跳出去
    if( g_stCAN115765ReceiveVar.u8FrameTotal == 0 )
    {
        return 0;
    }

    //判断第一帧是不是全部为0,处理完毕之后的第一帧
    //81 00 00 00 ff 00 00 00
    if( ( pDtcCodeStart[0] | pDtcCodeStart[1] | pDtcCodeStart[2] ) == 0 )
    {
        return 0;
    }

    //因为有故障码至少回2帧
    if( g_stCAN115765ReceiveVar.u8FrameTotal >= 2 )
    {
        //将最后一帧去掉
        g_stCAN115765ReceiveVar.u8FrameTotal--;
    }

    //系统类型,注意系统类型是从1开始
    *pCache++ = u8SysChoice + 1;
    //码库号,这里已经写死啦的
    *pCache++ = 6;
    //存储好故障码总数=帧总数
    *pCache++ = g_stCAN115765ReceiveVar.u8FrameTotal;

    //装载故障码
    for( u8DtcTotal = 0; u8DtcTotal < g_stCAN115765ReceiveVar.u8FrameTotal; u8DtcTotal++ )
    {
        *pCache++ = 0;
        memcpy( pCache, pDtcCodeStart, 3 );
        pCache += 3;
        pDtcCodeStart += 7;
    }

    //计算出故障码实际占用字节数=故障码总数X4
    //因为协议就是这么确定的
    return ( g_stCAN115765ReceiveVar.u8FrameTotal * 4 );
}

#endif
/*
*******************************************************************************
Name       :pfun_vw_pusang_rdtc_witch_nototal(void *pstsysChoice, void *pu8FunctionParameter)
Description:该函数主要是大众普桑车读取故障码(KW1281)
Input      :none
Output     :none
Return     :none
Others     :
1.06 xx FC FF FF 88 03代表系统正常
********************************************************************************
*/
#if RDTC_VW_PUSANG_WITCH_NOTOTAL_EN==1
void *pfun_vw_pusang_rdtc_witch_nototal( void *pstsysChoice, void *pu8FunctionParameter )
{
    //u16DtcCodeLen实际故障码总长度
    //u8GetDtcMode获取故障码模式
    //u8GetDtcModeTemp获取故障码模式
    //u8GetDtcModeCounter获取故障码方式计数器
    //pCmdTmp命令索引值(可以加上当前和历史,或者是单独一个)
    void *pCommStatus;
    uint16 u16DtcCodeLen = 0;
    uint8 u8GetDtcTotalMode = 0;
    uint8 *pCmdTmp = ( uint8* )( ( *g_pModeLibTab ).pCmdIndexTabChoice )->pCmdRDtc;

    //获取故障处理模式第0参数代表模式
    u8GetDtcTotalMode = ( ( uint8* )pu8FunctionParameter )[0];

    //读取故障码
    pCommStatus = ppublicfun_send_command_index( pCmdTmp, ( void* ) 0 );

    //如果通讯超时直接跳出函数
    if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
    {
        return ( pCommStatus );
    }

    //处理故障码,返回有效数据长度
    u16DtcCodeLen =
        fun_vw_pusang_rdtc_deal_with_nototal(
            u8GetDtcTotalMode,
            ( ( TYPEDEF_SYS_CFG* )pstsysChoice )->u8SysLibChoice ,
            &g_stFunMsgBuf[0].pu8FunBuffer[g_stFunMsgBuf[0].u16Len]
        );

    //更改已经存储的长度,为下次存储做好准备
    //如果有故障码才会去更改长度
    if( u16DtcCodeLen )
    {
        g_stFunMsgBuf[0].u16Len += u16DtcCodeLen + 3;
        g_stFunMsgBuf[0].pu8FunBuffer[0]++;
    }

    //返回通讯状态
    return ( pCommStatus );
}

/*
*******************************************************************************
Name        :fun_vw_pusang_rdtc_deal_with_nototal
Description:该函数主要是大众普桑车读取故障码(KW1281)
Input      :none
Output     :none
Return     :故障码总数长度
Others     :主要是处理kw1281没有总数的故障码类型数据
********************************************************************************
*/

uint16 fun_vw_pusang_rdtc_deal_with_nototal( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache )
{

    //故障码总数缓存地址uint8 u8DtcTotal
    //故障码号开始地址  uint8 *pDtcCodeStart
    //故障码有效字节u8DtcCodevalidByte
    //几个字节代表一个故障码u8DtcCodeXXByteOneDtc
    //pDtcWithTotalConfigLib获取初始化该函数的变量地址
    //u8DtcCodeCounter故障码计数器
    const TYPEDEF_DTC_NOTOTAL *pDtcWithNoTotalConfigLib;
    uint8 u8DtcTotal = 0;
    uint8 u8DtcCodevalidByte = 0;
    uint8 u8DtcCodeXXByteOneDtc = 0;
    uint8 *pDtcCodeStart = NULL;
    pDtcWithNoTotalConfigLib = g_stDtcWithNoTotalConfigLib + u8GetDtcModeTemp;

    u8DtcCodevalidByte = pDtcWithNoTotalConfigLib->u8DtcCodevalidByte;
    u8DtcCodeXXByteOneDtc = pDtcWithNoTotalConfigLib->u8DtcCodeXXByteOneDtc;

    //首先要判断有效字节的长度是否足够然后才能计算故障码个数至少得有2个字节,包括一个字节代表故障码
    if( g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u16Len < 2 )
    {
        return 0;
    }

    //计算故障码总数
    u8DtcTotal = ( g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u16Len
                   - pDtcWithNoTotalConfigLib->u8DtcCodeStartByte ) / u8DtcCodeXXByteOneDtc;

    //如果总数为0,那么就不用做处理
    if( !u8DtcTotal )
    {
        return 0;
    }

    //找到故障码开始地址
    pDtcCodeStart = g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u8Buf
                    + pDtcWithNoTotalConfigLib->u8DtcCodeStartByte;

    //判断接收到底数据06 xx xx FF FF 88 03,如果属于这个说明系统正常
    if
    (
        ( 0XFF == pDtcCodeStart[0] ) &
        ( 0XFF == pDtcCodeStart[1] ) &
        ( 0X88 == pDtcCodeStart[2] )
    )
    {
        return 0;
    }

    //系统类型,注意系统类型是从1开始
    *pCache++ = u8SysChoice + 1;
    //码库号
    *pCache++ = pDtcWithNoTotalConfigLib->u8DtcCodeLibChoice;
    //存储好故障码总数
    *pCache++ = u8DtcTotal;

    //装载故障码
    fun_rdtc_load_dtc( u8DtcTotal, u8DtcCodeXXByteOneDtc, u8DtcCodevalidByte, pCache, pDtcCodeStart );

    //计算出故障码实际占用字节数=故障码总数X4
    //因为协议就是这么确定的
    return ( u8DtcTotal * 4 );
}

#endif
/*
*******************************************************************************
Name        :fun_rdtc_deal_with_total
Description:该函数处理带总数
Input      :uint8 u8GetDtcModeTemp该变量主要是用来选择故障码获取方式的
uint8 *pCache将故障码保存在哪里
Output     :none
Return     :实际故障码总数+类型+总数+也就是上传的有效数据长度
Others     :使用情况1:K线2条读码命令,第一条是某一个字节代表总数,第二条开始是故障
码(暂时不支持)
使用情况2:K先1条读码命令,总数和故障码号都是在同一个数据缓存中。
********************************************************************************
*/

#if RDTC_DEAL_WITH_TOTAL_EN==1

uint16 fun_rdtc_deal_with_total( uint8 u8GetDtcModeTemp, uint8 u8SysChoice , uint8 *pCache )
{

    //故障码总数缓存地址uint8 u8DtcTotal
    //故障码号开始地址  uint8 *pDtcCodeStart
    //故障码有效字节u8DtcCodevalidByte
    //几个字节代表一个故障码u8DtcCodeXXByteOneDtc
    //pDtcWithTotalConfigLib获取初始化该函数的变量地址
    const TYPEDEF_DTC_WITHITOTAL *pDtcWithTotalConfigLib;
    uint8 u8DtcTotal = 0;
    uint8 u8DtcCodevalidByte = 0;
    uint8 u8DtcCodeXXByteOneDtc = 0;
    uint8 *pDtcCodeStart = NULL;
    pDtcWithTotalConfigLib = g_stDtcWithTotalConfigLib + u8GetDtcModeTemp;

    u8DtcCodevalidByte = pDtcWithTotalConfigLib->u8DtcCodevalidByte;
    u8DtcCodeXXByteOneDtc = pDtcWithTotalConfigLib->u8DtcCodeXXByteOneDtc;

    //找到故障码总数
    u8DtcTotal = g_stu8CacheData[pDtcWithTotalConfigLib->u8DtcTotalCacheAddr]
                 .u8Buf[pDtcWithTotalConfigLib->u8DtcTotalByte];

    //如果总数为0,那么就不用做处理
    if( !u8DtcTotal )
    {
        return 0;
    }

    //系统类型,注意系统类型是从1开始的
    *pCache++ = u8SysChoice + 1;
    //码库号
    *pCache++ = pDtcWithTotalConfigLib->u8DtcCodeLibChoice;
    //存储好故障码总数
    *pCache++ = u8DtcTotal;
    //找到故障码开始地址
    pDtcCodeStart = g_stu8CacheData[pDtcWithTotalConfigLib->u8DtcCodeCacheAddr].u8Buf
                    + pDtcWithTotalConfigLib->u8DtcCodeStartByte;
    //装载故障码
    fun_rdtc_load_dtc( u8DtcTotal, u8DtcCodeXXByteOneDtc, u8DtcCodevalidByte, pCache, pDtcCodeStart );

    //返回故障码总数
    return ( u8DtcTotal * 4 );

}

#endif

/*
*******************************************************************************
Name        :fun_rdtc_deal_with_nototal
Description:该函数处理不带总数
Input      :none
Output     :none
Return     :故障码总数长度
Others     :主要是处理没有总数的故障码类型数据
********************************************************************************
*/

#if RDTC_DEAL_WITH_NOTOTALL_EN==1

uint16 fun_rdtc_deal_with_nototal( uint8 u8GetDtcModeTemp, uint8 u8SysChoice, uint8 *pCache )
{

    //故障码总数缓存地址uint8 u8DtcTotal
    //故障码号开始地址  uint8 *pDtcCodeStart
    //故障码有效字节u8DtcCodevalidByte
    //几个字节代表一个故障码u8DtcCodeXXByteOneDtc
    //pDtcWithTotalConfigLib获取初始化该函数的变量地址
    //u8DtcCodeCounter故障码计数器
    const TYPEDEF_DTC_NOTOTAL *pDtcWithNoTotalConfigLib;
    uint8 u8DtcTotal = 0;
    uint8 u8DtcCodevalidByte = 0;
    uint8 u8DtcCodeXXByteOneDtc = 0;
    uint8 *pDtcCodeStart = NULL;
    uint8 u8DtcCodeCounter = 0;
    pDtcWithNoTotalConfigLib = g_stDtcWithNoTotalConfigLib + u8GetDtcModeTemp;

    u8DtcCodevalidByte = pDtcWithNoTotalConfigLib->u8DtcCodevalidByte;
    u8DtcCodeXXByteOneDtc = pDtcWithNoTotalConfigLib->u8DtcCodeXXByteOneDtc;

    //首先要判断有效字节的长度是否足够然后才能计算故障码个数至少得有2个字节,包括一个字节代表故障码
    if( g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u16Len < 2 )
    {
        return 0;
    }

    //计算故障码总数
    u8DtcTotal = ( g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u16Len
                   - pDtcWithNoTotalConfigLib->u8DtcCodeStartByte ) / u8DtcCodeXXByteOneDtc;

    //如果总数为0,那么就不用做处理
    if( !u8DtcTotal )
    {
        return 0;
    }

    //找到故障码开始地址
    pDtcCodeStart = g_stu8CacheData[pDtcWithNoTotalConfigLib->u8DtcCodeCacheAddr].u8Buf
                    + pDtcWithNoTotalConfigLib->u8DtcCodeStartByte;

    //判断接收到数据是不是全部为0,全部为0,不处理。
    for( u8DtcCodeCounter = 0; u8DtcCodeCounter < u8DtcTotal * u8DtcCodeXXByteOneDtc; u8DtcCodeCounter++ )
    {
        //判断是不是0,只要有非0就代表有故障码
        if( 0x00 != pDtcCodeStart[u8DtcCodeCounter] )
        {
            break;
        }

        //如果全部为0，那么就的直接跳出去
        if( u8DtcCodeCounter + 1 == u8DtcTotal * u8DtcCodeXXByteOneDtc )
        {
            return 0;
        }
    }

    //系统类型,注意系统类型是从1开始
    *pCache++ = u8SysChoice + 1;
    //码库号
    *pCache++ = pDtcWithNoTotalConfigLib->u8DtcCodeLibChoice;
    //存储好故障码总数
    *pCache++ = u8DtcTotal;

    //装载故障码
    fun_rdtc_load_dtc( u8DtcTotal, u8DtcCodeXXByteOneDtc, u8DtcCodevalidByte, pCache, pDtcCodeStart );

    //计算出故障码实际占用字节数=故障码总数X4
    //因为协议就是这么确定的
    return ( u8DtcTotal * 4 );
}

#endif

/*
*******************************************************************************
Name        :fun_rdtc_load_dtc
Description:该函数主要是处理故障码,根据总数装载故障码到指定区域
Input      :u8DtcTotal故障码总数 u8DtcCodeXXByteOneDtc几个字节代表一个故障码
u8DtcCodevalidByte故障码有效字节个数 pCache将故障码存储在哪里
pDtcCodeStart取故障码开始地址
Output     :none
Return     :none
Others     :
********************************************************************************
*/

#if RDTC_LOAD_DTC_EN==1

void fun_rdtc_load_dtc( uint8 u8DtcTotal, uint8 u8DtcCodeXXByteOneDtc,
                        uint8 u8DtcCodevalidByte, uint8 *pCache, uint8 *pDtcCodeStart )
{
    //由于故障码上传的规定,4个字节中的前3个是故障码，后面一个是状态位

    uint8 u8DtcCodeSavePid = 0;

    switch( u8DtcCodevalidByte )
    {
        //如果是一个字节一个故障码没有见过
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

        //如果是一个字节一个故障码也没有见过
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
            //这里还得注意一下,2个字节一个故障码和3个字节一个故障都是从a[]={00,xx,xx,xx}开始
            *( uint8* )( pCache + i + u8DtcCodeSavePid ) = * ( pDtcCodeStart + i );
        }

        pDtcCodeStart = pDtcCodeStart + u8DtcCodeXXByteOneDtc;
        pCache += 4;
    }
}

#endif






