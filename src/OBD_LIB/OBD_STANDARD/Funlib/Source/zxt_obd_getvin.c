/*
********************************************************************************
File name  : Zxt_obd_cfg.c
Description:
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   初始化底层需要的变量
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define GET_VIN_FUN
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"

/*
*******************************************************************************
Name       :pfun_cdtc_for_all_car
Description:该函数就是平常见到的简单清除故障码
Input      :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :
********************************************************************************
*/
#if GET_VIN_FOR_ALLCAR_EN==1
void *pfun_getvin_for_all_car( void *pArg0, void *pu8FunctionParameter )
{
    void *pCommStatus;
    uint8 u8GetVinMode = ( ( uint8* )pu8FunctionParameter )[0];
    const TYPEDEF_VIN_PAR0 *pVinLib = &g_stVinLib[u8GetVinMode];
    uint8 u8ProType = ( pVinLib->u8StartByte & 0xf0 ) >> 4;
    uint8 u8StartByte = ( pVinLib->u8StartByte & 0x0f );
    uint8 u8PackTotal =  pVinLib->u8PackTotal;
    uint8 u8FirstFramePid = pVinLib->u8FirstFramePid;
    uint8 u8PackLen     = pVinLib->u8PackLen;
    uint8 u8VinCnt      = 0;
    uint8 u8VinCnt0     = 0;
    bool  bVinOk        = TRUE;

    for( u8VinCnt0 = 0; u8VinCnt0 < 3; u8VinCnt0++ )
    {
        //调用VIN码获取函数
        pCommStatus = ppublicfun_send_command_index( ( uint8 * )g_pModeLibTab->pCmdIndexTabChoice->pCmdGetVin, ( void* ) 0 );

        //如果通讯超时直接跳出函数
        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            return ( pCommStatus );
        }

        //首先判断协议类型K =1
        if( u8ProType )
        {
            uint8 u8PackTmp = 0;
            uint8 *pu8CacheBuf = &g_stu8CacheData[0].u8Buf[u8StartByte];
            uint8 *pu8VinSn    = &g_stu8CacheData[0].u8Buf[u8StartByte - 1];
            uint8 *pu8VinBuf = g_stFunMsgBuf[3].pu8FunBuffer;

            //装载VIN
            for( ; u8PackTmp < u8PackTotal; u8PackTmp++ )
            {
                if( pu8VinSn[u8PackTmp * 6] != u8PackTmp + 1 )
                {
                    //清空vin
                    memset( g_stFunMsgBuf[3].pu8FunBuffer, 0, g_stFunMsgBuf[3].u16Len );
                    return ( pCommStatus );
                }

                memcpy( pu8VinBuf, pu8CacheBuf, u8PackLen );
                pu8VinBuf += u8PackLen;
                pu8CacheBuf += u8PackLen + u8FirstFramePid;
            }

            //调整VIN位置
            memmove( g_stFunMsgBuf[3].pu8FunBuffer, &g_stFunMsgBuf[3].pu8FunBuffer[u8FirstFramePid + 1], 17 );
        }
        else
        {
            uint8 *pu8CacheBuf = &g_stu8CacheData[0].u8Buf[u8StartByte];
            uint8 *pu8VinBuf = g_stFunMsgBuf[3].pu8FunBuffer;
            memcpy( pu8VinBuf, pu8CacheBuf, u8PackLen );
        }

        //判断VIN码正确性
        for( u8VinCnt = 0 ; u8VinCnt < 17; u8VinCnt++ )
        {
            if( !( ( g_stFunMsgBuf[3].pu8FunBuffer[u8VinCnt] >= 'A' ) & ( g_stFunMsgBuf[3].pu8FunBuffer[u8VinCnt] <= 'Z' ) ||
                    ( g_stFunMsgBuf[3].pu8FunBuffer[u8VinCnt] >= '0' ) & ( g_stFunMsgBuf[3].pu8FunBuffer[u8VinCnt] <= '9' ) ) )
            {
                //清空vin
                memset( g_stFunMsgBuf[3].pu8FunBuffer, 0, g_stFunMsgBuf[3].u16Len );
                bVinOk = FALSE;
                break;
            }
            else
            {
                bVinOk = TRUE;
            }
        }

        if( bVinOk )
        {
            break;
        }

        OSTimeDlyHMSM( 0, 0, 1, 0 );
    }

    return ( pCommStatus );
}

#endif







