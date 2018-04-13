/*
********************************************************************************
File name  : zxt_iso_15765.c
Description: 这里编写都是数据流功能相关函数
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19 冯秋杰   建立数据流函数
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/
#define LIB_COMM_FUN
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"
/*
********************************************************************************
Name    :void *pfun_iso15765_send_command_group(void* pArg0, void* pArg1).
Description:该函数主要是针对某一款车有扫描类型,如果不是扫描类型也可以用。
Input    :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :成功之后确定啦这个值g_pModeLibTab_lib,具体用哪个模块进行通信,
      同时确定协议类型
      这个函数需要上位机提供3个变量就是模块总数 模块值 通讯函数
********************************************************************************
*/

void *pfun_iso15765_send_command_group( void *pCmdId, void *pArg1 )
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
        fun_iso_15765_send_times( ( ( uint8* ) pCmdId )[u8CurSent + 1], &pCommStatus );

        if( *( TYPEDEF_FRAME_STATUS* ) pCommStatus != OK )
        {
            //跳出for 直接报告通讯不正常
            break;
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
Function   :fun_iso_15765_send_times
Description:该函数主要控制发送命令同时重新发送次数
      如果成功然后跳出该函数去执行接收函数
      该函数变量初始化是和通讯连接是同一个偏移量
Input    :uint8 cmdid:发送哪一条命令
      void **pCommStatus:通讯状态
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void fun_iso_15765_send_times( uint8 u8CmdId, void **pCommStatus )
{

    uint8 u8Error;
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
        //在这等邮箱有2种可能1.ECU接收数据中断2.串口2中断，其他任务不可能执行3.因为大家都在等邮箱

        *pCommStatus = pfOSboxped( g_pstEcuToPerformer_lib,
                                   pStIso15765ActiveLib->u16TimeOut, &u8Error );
        if(u8FilterGroup < 14)
        //关闭接收
					CAN1->FA1R &= ~(( uint32 )0x1<<u8FilterGroup);
				else
					CAN1->FA1R &= ~(( uint32 )0x1<<u8FilterGroup);

        //用来等待can中断结束用来判断接收到的数据是否符合协议要求
        switch( u8Error )
        {

            case  OS_ERR_TIMEOUT:
                {
                    //当做帧超时,来处理，这个时候应该是不允许重新发送

                    if( g_stCAN115765ReceiveVar_temp->bFrameError )
                    {
                        *pCommStatus = ( void * )& g_enumFrameFlgArr[1];
                    }

                    else
                        //这里当做没有响应来处理
                    {
                        *pCommStatus = ( void * )& g_enumFrameFlgArr[0];;
                    }

                    if( ( --u8RetransCounter ) == 0 )
                    {
                        //直接退出函数
                        return;
                    }

                    break;
                }

            //成功接收邮件,只有2种情况:1.通讯正常,2.说明是7f 系列帧,得根据相应的条件进行重新发送
            case  OS_NO_ERR:
                {
                    if( ( *( TYPEDEF_FRAME_STATUS* ) *pCommStatus == OK ) )
                    {
                        return;
                    }

                    break;
                }

            default://报告从ECU那投递过来的信息有问题
                return;
        };
    }
    while( 1 );
}



/*
*******************************************************************************
Name    :fun_iso_15765_send_cmd
Description:该函数主要是标准的15765发送函数
Input    :uint8 u8CmdId
Output     :none
Return     :none
Others     :首先根据cmdid去装载命令
      发送完一帧命令
********************************************************************************
*/

void fun_iso_15765_send_cmd( uint8 u8CmdId )
{

    //u32FrameId命令发送帧ID
    //u8Filterpid滤波器设置ID
    //u8FilterGroup滤波器组选择
    uint32 u32FrameId;
    uint8  u8Filterpid;
    uint8  u8FilterGroup;

    //对CAN1变量进行初始
    g_stCAN115765ReceiveVar_temp->u8DataCounter = 0;
    //这个变量ISO15765没有用上,通用协议上用上,注意这个值是根据ECU发送数据进行变化的,所以得注意使用的时间段
    g_stCAN115765ReceiveVar_temp->u8FrameTotal = 0;
    g_stCAN115765ReceiveVar_temp->bFrameError = FALSE;
    g_stCAN115765ReceiveVar_temp->bFrameLengTh = FALSE;
    g_stCAN115765ReceiveVar_temp->u8FrameCounter = 0;
    //装载命令
    memcpy
    (
        g_stTxMessage.Data,
        ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).pu8Command ,
        8
    );
    //发送命令
    u32FrameId = g_unnu32FrameIdLib_lib[( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8CmdIdOffset].u32Bit;
    //滤波器设置ID
    u8Filterpid = ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8FilterIdOffset;
    //滤波器组选择
    u8FilterGroup = ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8FilterGroup;
    //先禁止CAN接收中断
		if(u8FilterGroup < 14)
			CAN_ITConfig( CAN1, CAN_IT_FMP0, DISABLE );
		else
			CAN_ITConfig( CAN2, CAN_IT_FMP0, DISABLE );
    //配置好CAN滤波器
    pfCAN1FilterInit( u8FilterGroup, g_unnu32FilterId_lib[u8Filterpid].u32Bit );
    //开启can接收中断
		if(u8FilterGroup < 14)
			CAN_ITConfig( CAN1, CAN_IT_FMP0, ENABLE );
		else
			CAN_ITConfig( CAN2, CAN_IT_FMP0, ENABLE );
    //最好先清除一次再等邮箱
    g_pstEcuToPerformer_lib->OSEventPtr = ( void * ) 0;

    //配置好CAN通讯ID
    g_stTxMessage.StdId = u32FrameId;
    g_stTxMessage.ExtId = u32FrameId;
    //暂时写死是数据帧
    g_stTxMessage.RTR = 0;
    //是不是扩展帧
    g_stTxMessage.IDE = ( u32FrameId > 0X7FF ) ? 4 : 0;
    //这里也是根据所填写的长度来决定的
    g_stTxMessage.DLC = ( ( g_pModeLibTab_lib->pCmdTabChoice )[u8CmdId] ).u8CmdLen;
		if(u8FilterGroup < 14)
			CAN_Transmit( CAN1, &g_stTxMessage );
		else
			CAN_Transmit( CAN2, &g_stTxMessage );
}


/*
********************************************************************************
Name    :void*pfun_iso_15765_receive( void *pCacheOffset, void *pArg1 )
Description:该函数主要是针对CAN接收函数进行处理,主要是判断有没有必要
Input    :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void *pfun_iso_15765_receive( void *pCacheOffset, void *pArg1 )
{
    void *prg0 = NULL;

    switch( *( uint8* ) pCacheOffset )
    {

        case 255://如果是255说明不用存储
            break;

        default://将数据放置到需要放置的地方
            g_stu8CacheData[*( uint8* ) pCacheOffset] = g_stDataFromEcu;
            break;
    }

    return ( prg0 );
}

/*
********************************************************************************
Name    :void *pfun_iso_15765_check(void* pArg0, void* pArg1).
Description:该函数是15765判断接收过来的数据是否正常的判断函数
Input    :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :通讯程成功之后就会设置bFrameError=false同时投递邮箱,其他的是不会投递邮箱的
注意问题:如果接收的数据不完整当做超时处理-不投递邮箱
         只有通讯正常才会投递邮箱。任何时候都不允许投递邮箱
         如果接收的数据属于消极响应系列(注意啦单帧才会出现消极响应)
********************************************************************************
*/

void*pfun_iso_15765_check( void*pArg0, void*pArg1 )
{
  	//首先判断帧的类型单帧<7 10帧 20 30帧
    switch( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0xf0 )
    {
        //这里根据汽车发送过来的数据进行归类存放

        case 0x10:
            {
                //公共帧 即是在收到10 后给回复的
                uint8 u8publicFrame [] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00};
                //设置好帧长
                g_stDataFromEcu.u8Len = g_stCAN115765ReceiveVar_temp->RxMessage.Data[1];
                //同时告知已经找到帧长,主要是为21帧做准备
                g_stCAN115765ReceiveVar_temp->bFrameLengTh = TRUE;
                //存储10帧,只是存储6个字节
                memcpy( g_stDataFromEcu.u8Buf, &g_stCAN115765ReceiveVar_temp->RxMessage.Data[2], 6 );
                //记住当前存储的数据长度
                g_stCAN115765ReceiveVar_temp->u8DataCounter = 6;
                //发送公共帧,暂时在这里发送, 如果以后不合适可以考虑变化位置
                memcpy( &g_stTxMessage.Data, u8publicFrame, 8 );
								if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
									CAN_Transmit( CAN1, &g_stTxMessage );
								else
									CAN_Transmit( CAN2, &g_stTxMessage );
                //因为有可能接收到10帧之后不给回数据,导致通讯报告错误
                g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;
                //为接收2X帧设置计数器
                g_stCAN115765ReceiveVar_temp->u8FrameCounter = 1;
                return ( ( void * )0 );
            }

        case 0x20:
            {
                uint16 u8DataLen = 0;
                TYPEDEF_FRAME_STATUS *Comm_report = NULL;

                //判断是否找到帧长
                if( !g_stCAN115765ReceiveVar_temp->bFrameLengTh )
                {
                    return ( ( void * )0 );
                }

                //判断是不是想要接收的帧
                if( g_stCAN115765ReceiveVar_temp->u8FrameCounter != ( uint8 )( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0x0f ) )
                {
                    return ( ( void * )0 );
                }

                //为下次接收2X帧做好准备
                g_stCAN115765ReceiveVar_temp->u8FrameCounter++;
                //存储2X帧,只是存储7个字节
                memcpy( g_stDataFromEcu.u8Buf + g_stCAN115765ReceiveVar_temp->u8DataCounter,
                        &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1],
                        7
                      );
                //设置好偏移地址为下一次做好准备
                g_stCAN115765ReceiveVar_temp->u8DataCounter = g_stCAN115765ReceiveVar_temp->u8DataCounter + 7;
                //判断接收到的数据是否正确
                u8DataLen =
                    g_stDataFromEcu.u8Len +
                    ( 7 - ( ( ( g_stDataFromEcu.u8Len - 6 ) % 7 ) ? ( ( g_stDataFromEcu.u8Len - 6 ) % 7 ) : 7 ) );

                //判断接收到的数据是否已经达到有效字节
                if( u8DataLen != g_stCAN115765ReceiveVar_temp->u8DataCounter )
                {
                    //这个标志位用于判断接收到的数据是不是正确的
                    //因为在接收20帧的过程中随时都有可能断掉
                    g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;
                    return ( ( void * )0 );
                }

                //设置帧通讯为正常
                g_stCAN115765ReceiveVar_temp->bFrameError = FALSE ;

                //设置为帧正常
                Comm_report = ( void * )& g_enumFrameFlgArr[2];

                //设置为帧正常
                pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                //关闭接收
								if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
									CAN1->FA1R &= ~(( uint32 )0x1);
								else
									CAN1->FA1R &= ~(( uint32 )0x1<<14);
                return ( ( void * )0 );
            }

        //这里备用

        case 0x30:
            {

            } break ;

        //这里默认就是单帧

        default :
            {
                //设置好帧长
                TYPEDEF_FRAME_STATUS *Comm_report = NULL;
                //获取数据有效长度,这里只是针对标准数据帧<=7
                g_stDataFromEcu.u8Len = ( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0x07 );
                //存储10帧,只是存储有效长度个字节
                memcpy( g_stDataFromEcu.u8Buf, &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1], g_stDataFromEcu.u8Len );
                //默认设置为通讯错误,因为有可能接收到7F 78之后再也不回数据
                g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;

                //判断接收到的数据是否正确

                if( !bfun_iso_14230_negative_response() )
                {
                    //设置帧通讯为正常
                    g_stCAN115765ReceiveVar_temp->bFrameError = FALSE ;
                    //设置为帧正常
                    Comm_report = ( void * )& g_enumFrameFlgArr[2];
                    //关闭接收
										if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
											CAN1->FA1R &= ~(( uint32 )0x1);																	else
											CAN1->FA1R &= ~(( uint32 )0x1<<14);
                    //投递邮箱给执行者报告接收0k
                    pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                }

                else
                {
                    //到这里已经出现接收到的数据是7F类型的数据
                    //如果是7f 78那么就等着接收正确的数据,如果不回数据那么就等着超时
                    //如果是7f 11说明是不支持,直接等着超时
                    Comm_report = ( void * )& g_enumFrameFlgArr[1];

                    //如果是7f 21说明是要重新发送数据,然后投递邮箱重新发送数据
                    if( ( 0x7f == g_stDataFromEcu.u8Buf[0] ) & ( 0x21 == g_stDataFromEcu.u8Buf[2] ) )
                    {
                        pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                    }
                }
            }

            break ;

    }

    return ( ( void * )0 );
}

/*
********************************************************************************
Name    :void *pfun_iso_15765_check1(void* pArg0, void* pArg1).
Description:该函数是15765判断接收过来的数据是否正常的判断函数
Input    :void *pArg0,void *pArg1这2个参数都没有用上
Output     :none
Return     :none
Others     :通讯程成功之后就会设置bFrameError=false同时投递邮箱,其他的是不会投递邮箱的
注意问题:pfun_iso_15765_check1函数注意和pfun_iso_15765_check区别,有些车公共帧响应的时候的
         id不一样酒店重新使用,该函数默认是不开启的
         比如: 7e0 02 10 81
               7e8 10 08 50  ecu多帧
               790 30 00 00  Y05A 发送多帧
********************************************************************************
*/


void*pfun_iso_15765_check1( void*pArg0, void*pArg1 )
{
  	//首先判断帧的类型单帧<7 10帧 20 30帧
    switch( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0xf0 )
    {
        //这里根据汽车发送过来的数据进行归类存放

        case 0x10:
            {
                //公共帧 即是在收到10 后给回复的
                uint8 u8publicFrame [] = {0x30, 0x00, 0x00, 0x00, 0x00, 0x00 , 0x00 , 0x00};

                //发送命令
                uint32 u32FrameId = g_unnu32FrameIdLib_lib[( g_pModeLibTab_lib->u8ProtocolLibOffset )[4]].u32Bit;
                //设置好帧长
                g_stDataFromEcu.u8Len = g_stCAN115765ReceiveVar_temp->RxMessage.Data[1];
                //同时告知已经找到帧长,主要是为21帧做准备
                g_stCAN115765ReceiveVar_temp->bFrameLengTh = TRUE;
                //存储10帧,只是存储6个字节
                memcpy( g_stDataFromEcu.u8Buf, &g_stCAN115765ReceiveVar_temp->RxMessage.Data[2], 6 );
                //记住当前存储的数据长度
                g_stCAN115765ReceiveVar_temp->u8DataCounter = 6;
                //发送公共帧,暂时在这里发送, 如果以后不合适可以考虑变化位置
                memcpy( &g_stTxMessage.Data, u8publicFrame, 8 );

                g_stTxMessage.StdId = u32FrameId;
                g_stTxMessage.ExtId = u32FrameId;
								if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
									CAN_Transmit( CAN1, &g_stTxMessage );
								else
									CAN_Transmit( CAN2, &g_stTxMessage );
                //因为有可能接收到10帧之后不给回数据,导致通讯报告错误
                g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;
                //为接收2X帧设置计数器
                g_stCAN115765ReceiveVar_temp->u8FrameCounter = 1;
                return ( ( void * )0 );
            }

        case 0x20:
            {
                uint16 u8DataLen = 0;
                TYPEDEF_FRAME_STATUS *Comm_report = NULL;

                //判断是否找到帧长
                if( !g_stCAN115765ReceiveVar_temp->bFrameLengTh )
                {
                    return ( ( void * )0 );
                }

                //判断是不是想要接收的帧
                if( g_stCAN115765ReceiveVar_temp->u8FrameCounter != ( uint8 )( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0x0f ) )
                {
                    return ( ( void * )0 );
                }

                //为下次接收2X帧做好准备
                g_stCAN115765ReceiveVar_temp->u8FrameCounter++;
                //存储2X帧,只是存储7个字节
                memcpy( g_stDataFromEcu.u8Buf + g_stCAN115765ReceiveVar_temp->u8DataCounter,
                        &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1],
                        7
                      );
                //设置好偏移地址为下一次做好准备
                g_stCAN115765ReceiveVar_temp->u8DataCounter = g_stCAN115765ReceiveVar_temp->u8DataCounter + 7;
                //判断接收到的数据是否正确
                u8DataLen =
                    g_stDataFromEcu.u8Len +
                    ( 7 - ( ( ( g_stDataFromEcu.u8Len - 6 ) % 7 ) ? ( ( g_stDataFromEcu.u8Len - 6 ) % 7 ) : 7 ) );

                //判断接收到的数据是否已经达到有效字节
                if( u8DataLen != g_stCAN115765ReceiveVar_temp->u8DataCounter )
                {
                    //这个标志位用于判断接收到的数据是不是正确的
                    //因为在接收20帧的过程中随时都有可能断掉
                    g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;
                    return ( ( void * )0 );
                }

                //设置帧通讯为正常
                g_stCAN115765ReceiveVar_temp->bFrameError = FALSE ;

                //设置为帧正常
                Comm_report = ( void * )& g_enumFrameFlgArr[2];

                //设置为帧正常
                pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                //关闭接收
								if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
										CAN1->FA1R &= ~(( uint32 )0x1);
								else
									  CAN1->FA1R &= ~(( uint32 )0x1<<14);
                return ( ( void * )0 );
            }

        //这里备用

        case 0x30:
            {

            } break ;

        //这里默认就是单帧

        default :
            {
                //设置好帧长
                TYPEDEF_FRAME_STATUS *Comm_report = NULL;
                //获取数据有效长度,这里只是针对标准数据帧<=7
                g_stDataFromEcu.u8Len = ( g_stCAN115765ReceiveVar_temp->RxMessage.Data[0] & 0x07 );
                //存储10帧,只是存储有效长度个字节
                memcpy( g_stDataFromEcu.u8Buf, &g_stCAN115765ReceiveVar_temp->RxMessage.Data[1], g_stDataFromEcu.u8Len );
                //默认设置为通讯错误,因为有可能接收到7F 78之后再也不回数据
                g_stCAN115765ReceiveVar_temp->bFrameError = TRUE ;

                //判断接收到的数据是否正确

                if( !bfun_iso_14230_negative_response() )
                {
                    //设置帧通讯为正常
                    g_stCAN115765ReceiveVar_temp->bFrameError = FALSE ;
                    //设置为帧正常
                    Comm_report = ( void * )& g_enumFrameFlgArr[2];
                    //关闭接收
									  if(g_stCAN115765ReceiveVar_temp == &g_stCAN115765ReceiveVar)
											CAN1->FA1R &= ~(( uint32 )0x1);
										else
											CAN1->FA1R &= ~(( uint32 )0x1<<14);
                    //投递邮箱给执行者报告接收0k
                    pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                }

                else
                {
                    //到这里已经出现接收到的数据是7F类型的数据
                    //如果是7f 78那么就等着接收正确的数据,如果不回数据那么就等着超时
                    //如果是7f 11说明是不支持,直接等着超时
                    Comm_report = ( void * )& g_enumFrameFlgArr[1];

                    //如果是7f 21说明是要重新发送数据,然后投递邮箱重新发送数据
                    if( ( 0x7f == g_stDataFromEcu.u8Buf[0] ) & ( 0x21 == g_stDataFromEcu.u8Buf[2] ) )
                    {
                        pfOSboxpst( g_pstEcuToPerformer_lib, Comm_report );
                    }
                }
            }

            break ;

    }

    return ( ( void * )0 );
}
