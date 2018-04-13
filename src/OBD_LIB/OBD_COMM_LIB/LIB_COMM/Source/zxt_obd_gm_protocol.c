/*
********************************************************************************
File name  : Zxt_gm_protocol.c
Description: 这里编写都是数据流功能相关函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19 冯秋杰   建立数据流函数
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define LIB_COMM_FUN
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"

void *pfun_gm_send_command_group( void *pCmdId, void *pArg1 )
{
    //u8CurSent当前发送命令值
    //pCommStatus通讯状态
    //u8CacheOffset缓存存储地址
    uint8 u8CurSent = 0;
    void  *pCommStatus;
    const TYPEDEF_ISO15765_ACTIVE_PAR *piso15765_active_Lib;
    uint8 u8CacheOffset = 0;

    piso15765_active_Lib = &g_stIso15765ActiveLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];

    //这里主要是控制发送一整组命令

    for( u8CurSent = 0; u8CurSent < ( ( uint8* ) pCmdId )[0]; u8CurSent++ )
    {

        //发送命令
        fun_gm_send_times( ( ( uint8* ) pCmdId )[u8CurSent + 1], &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            break;//跳出for 直接报告通讯不正常
        }

        //待会将数据存储在哪一个缓冲区
        u8CacheOffset = ( ( ( ( *g_pModeLibTab_lib ).pCmdTabChoice )[( ( uint8* ) pCmdId )[u8CurSent + 1]] ).u8BufferId );

        //开始接收数据,这里已经写死没有调用库,接收的方法和14230是不一样的
        pfun_iso_15765_receive( &u8CacheOffset, ( void* ) 0 );

        //通讯帧时间
        pfOSdelay( 0, 0, 0, piso15765_active_Lib->u8FrameTime );
    }

    return pCommStatus;

}


/*
*******************************************************************************
Function   :fun_gm_send_times
Description:该函数主要控制发送命令同时重新发送次数
如果成功然后跳出该函数去执行接收函数
该函数变量初始化是和通讯连接是同一个偏移量
Input    :uint8 cmdid:发送哪一条命令
void **pCommStatus:通讯状态
Output     :none
Return     :none
Others     :切记这里没有处理7f 系列数据,得到具体功能中进行处理
********************************************************************************
*/


void fun_gm_send_times( uint8 u8CmdId, void **pCommStatus )
{

    uint8 u8RetransCounter;
    const TYPEDEF_ISO15765_ACTIVE_PAR *pStIso15765ActiveLib;
	   uint8 u8FilterGroup = ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8FilterGroup;
    //直接指向通讯设置组
    pStIso15765ActiveLib =
        &g_stIso15765ActiveLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[3]];
    u8RetransCounter = pStIso15765ActiveLib ->u8RetransTime;//重发次数
    //设置好check函数
    g_u8ProtocolLibOffset = ( g_pModeLibTab_lib->u8ProtocolLibOffset )[2];

    do
    {
        //命令发送,CAN通讯需要用上的变量初始化
        fun_iso_15765_send_cmd( u8CmdId );

        //ecu超时时间,最好设置帧时间为1S,当做回应10帧来处理
        pfOSdelay( 0, 0, pStIso15765ActiveLib->u16TimeOut / 1000, pStIso15765ActiveLib->u16TimeOut % 1000 );


        //关闭接收
			  if(u8FilterGroup < 14)
					CAN1->FA1R &= ~(( uint32 )0x1 << u8FilterGroup);
				else
					CAN1->FA1R &= ~(( uint32 )0x1 << u8FilterGroup);

        //为预防ECU没有响应设置
        if( g_stCAN115765ReceiveVar_temp->u8DataCounter == 0 )
        {
            *pCommStatus = ( void * )&g_enumFrameFlgArr[0];
            //重新发送

            if( ( --u8RetransCounter ) != 0 )
            {
                continue;
            }

            else
            {
                return;//如果重发次数已经到那么就跳出函数
            }
        }

        //到这里说明确实接收到数据当做接收正常,至于数据正确与否不是由这里决定，是由具体功能决定
        *pCommStatus = ( void * )&g_enumFrameFlgArr[2];
        return;
    }
    while( 1 );
}


/*
********************************************************************************
Name    :void *pfun_gm_check(void* pArg0, void* pArg1).
Description:该函数主要是应对通用CAN总线数据
Input    :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :适合使用条件
1.ECU回的数据没有规律例如:单帧没有长度,没有多帧标志,
每一帧舍弃第一个字节
2.注意这里运用的是超时的方式接收的
********************************************************************************
*/


void*pfun_gm_check( void*pArg0, void*pArg1 )
{
    //装载好数据
    memcpy( &g_stDataFromEcu.u8Buf[g_stCAN115765ReceiveVar_temp->u8DataCounter], &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1], ( g_stCAN115765ReceiveVar_temp->RxMessage.DLC - 1 ) );
    //设置好接收计数器,为下一次接收做好准备
    g_stCAN115765ReceiveVar_temp->u8DataCounter += g_stCAN115765ReceiveVar_temp->RxMessage.DLC - 1 ;
    //设置好帧长,为处理数据做好准备
    g_stDataFromEcu.u8Len = g_stCAN115765ReceiveVar_temp->u8DataCounter;
    //帧总数增加,在发送的时候已经清0,这个值给一些读码等等这些功能函数使用很有用(通用车就很管用)
    g_stCAN115765ReceiveVar_temp->u8FrameTotal++;
    return ( ( void * )0 );
}

