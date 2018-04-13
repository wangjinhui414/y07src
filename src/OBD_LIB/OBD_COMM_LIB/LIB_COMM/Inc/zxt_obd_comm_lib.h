/*
***********************************************************************************************************************
*                       LIB库更新日志
*
*  V1.00	2016-8-23      完成标准OBD通讯函数的封装
*  V1.10  2016-11-10     1、修改CAN发送接收函数组,标定CAN1只使用第 0 组滤波器,CAN2只使用第 14 组滤波器
*                        2、使用g_stCAN115765ReceiveVar_temp代替g_stCAN115765ReceiveVar,或g_stCAN115765ReceiveVar1
*                           外部中断接收时请把 用g_stCAN115765ReceiveVar_temp指向实际接收变量
*  V1.11	2017-2-24      1、修改TYPEDEF_PROTOCOL_MSGBUF成员u8Len为u16Len
*                        2、修改TYPEDEF_CANMSG成员u8DataCounter为u16DataCounter
***********************************************************************************************************************
*/
#ifndef	_ZXT_OBD_LIB_H_
#define	_ZXT_OBD_LIB_H_

//标准库头文件包含
#include <stdio.h> //定义输入／输出函数
#include <stdarg.h>
#include <stdlib.h> //定义杂项函数及内存分配函数
#include <string.h> //字符串处理
#include "stm32f10x.h"

//基本类型声明
typedef unsigned char                  BOOLEAN;
typedef unsigned char                  uint8;
typedef signed   char                  int8;
typedef unsigned short                 uint16;
typedef signed   short                 int16;
typedef unsigned int                   uint32;
typedef signed   int                   int32;
typedef float                          fp32;
typedef double                         fp64;

typedef unsigned char  INT8U;  
typedef unsigned short INT16U; 
#ifdef	LIB_COMM_GVAR
#		define	LIB_COMM_GVAR_EXT
#else
#		define	LIB_COMM_GVAR_EXT		extern
#endif

#ifdef	LIB_COMM_FUN
#		define	LIB_COMM_FUN_EXT
#else
#		define	LIB_COMM_FUN_EXT		extern
#endif

/*
********************************************************************************
Library struct type define.
********************************************************************************
*/
typedef struct TYPEDEF_CMD_INDEX
{
  const uint8 *pCmdEnter;                                                  //进入命令
  const uint8 *pCmdIdle;                                                   //空闲命令
  const uint8 *pCmdRDtc;                                                   //读当前故障码
  const uint8 *pCmdClearDtc;                                               //清码命令
  const uint8 *pCmdGetOdoMeter;                                            //获取系统里程命令
  const uint8 *pCmdSysExit;                                                //系统退出命令
	const uint8 *pCmdGetVin;                                                 //获取车辆vin码
} TYPEDEF_CMD_INDEX;                                                       //命令索引值成员

typedef struct TYPEDEF_CMD_TAB
{
    const uint8 u8BufferId;                                                 //命令存储的缓冲区
    const uint8 u8CmdLen;                                                   //命令长度
    const uint8 u8CmdIdOffset;                                              //u8CmdIdOffset
    const uint8 u8FilterIdOffset;                                           //滤波器设置
    const uint8 u8FilterGroup;                                              //14组滤波器中哪一组,主要是针对CAN线
    const uint8 *pu8Command;                                                //指向具体的命令
} TYPEDEF_CMD_TAB;

typedef  struct TYPEDEF_MODE_CELL
{
    const TYPEDEF_CMD_TAB    *pCmdTabChoice;                               //指向整个框架中的发动机系统
    const TYPEDEF_CMD_INDEX  *pCmdIndexTabChoice;                          //整个系统命令索引值表
    const uint8            u8ProtocolLibOffset[5];                         //这里放置的是发送和接收函数的选择
} TYPEDEF_MODE_CELL;

typedef union TYPEDEF_1WORD2BYTE
{
    uint16 u16Bit;
    uint8  u8Bit[2];
} TYPEDEF_1WORD2BYTE;

typedef union  TYPEDEF_2WORD4BYTE
{
    uint32 u32Bit;
    uint16 u16Bit;
    uint8  u8Bit[4];
} TYPEDEF_2WORD4BYTE;

typedef struct TYPEDEF_PROTOCOL_MSGBUF
{
    uint16  u16Len;
    uint8  u8Buf[255];
} TYPEDEF_PROTOCOL_MSGBUF;

typedef struct TYPEDEF_CANMSG
{
    uint16  u16DataCounter;                                                  //CAN中用来记住接收多少个有效数据
    uint8  u8FrameTotal;                                                   //CAN中用来记住总共接收多少帧
    CanRxMsg RxMessage;                                                    //CAN接收数据
    bool   bFrameError;                                                    //用于判断当前接收到的帧是否正确
    bool   bFrameLengTh;                                                   //用来判断是否已经找到帧长(主要是针对10帧)
    uint8  u8FrameCounter;                                                 //用于接收多帧的帧数(主要是针对2X帧)
} TYPEDEF_CANMSG;

typedef struct TYPEDEF_ISO14230_RECEIVE
{
    uint8  u8DataOffset;                                                   //串口3中用来控制数据放置位置
    uint8  u8ReceiveCounter;                                               //串口3中用来记住接收多少个数据
    uint8  u8SentCounter;                                                  //kwp1281中用来记住发送多少条命令
    uint8  u8ReceivePack;                                                  //由于K线需要接收多帧,特意对这里进行处理 
		uint8  u8FrameLen;																										 //固定帧长多帧
	  bool   bFrameError;                                                    //用于判断当前接收到的帧是否正确
	  
} TYPEDEF_ISO14230_RECEIVE;

typedef struct TYPEDEF_ISO15765_ACTIVE_PAR
{
    uint32 u32BaudRate;                                                    //波特率
    uint8  u8FrameTime;                                                    //CAN通讯帧时间
    uint16 u16TimeOut;                                                     //CAN通讯超时时间
    uint8  u8RetransTime;                                                  //重发次数
} TYPEDEF_ISO15765_ACTIVE_PAR;

typedef struct TYPEDEF_ISO14230_COMMUNICATION_PAR
{
    uint8   u8ByteTime;                                                    //通讯字节时间
    uint16  u16FrameTime;                                                  //通讯帧时间
    uint8   u8RetransTime;                                                 //重发次数
    uint16  u16TimeOut;                                                    //超时时间
} TYPEDEF_ISO14230_COMMUNICATION_PAR;

typedef  enum {TIMEOUT, FRAMETIMEOUT, OK , NOSTAUS} TYPEDEF_FRAME_STATUS;


#ifndef OS_VERSION
	#define OS_ERR_TIMEOUT               10u
	#define OS_ERR_NONE                   0u
	#define OS_NO_ERR                    OS_ERR_NONE
	#define OS_LOWEST_PRIO           31    									/* Defines the lowest priority that can be assigned ...         */
	#define  OS_EVENT_TBL_SIZE ((OS_LOWEST_PRIO) / 8 + 1)   /* Size of event table                         */
	typedef struct os_event {
			INT8U    OSEventType;                   					 /* Type of event control block (see OS_EVENT_TYPE_xxxx)    */
			void    *OSEventPtr;                    						 /* Pointer to message or queue structure                   */
			INT16U   OSEventCnt;                     						/* Semaphore Count (not used if other EVENT type)          */
	#if OS_LOWEST_PRIO <= 63
			INT8U    OSEventGrp;                     							/* Group corresponding to tasks waiting for event to occur */
			INT8U    OSEventTbl[OS_EVENT_TBL_SIZE];  						/* List of tasks waiting for event to occur                */
	#else
			INT16U   OSEventGrp;                     						/* Group corresponding to tasks waiting for event to occur */
			INT16U   OSEventTbl[OS_EVENT_TBL_SIZE];  						/* List of tasks waiting for event to occur                */
	#endif

	#if OS_EVENT_NAME_SIZE > 1
			INT8U    OSEventName[OS_EVENT_NAME_SIZE];
	#endif
	} OS_EVENT;
#endif
		
	
/*
********************************************************************************
Import function type define.
********************************************************************************
*/
typedef  INT8U (*OSDELAY)(INT8U hours, INT8U minutes, INT8U seconds, INT16U ms);//OSTimeDlyHMSM
typedef	 INT8U (*OSMboxPST) (OS_EVENT *pevent, void *pmsg);//OSMboxPost
typedef	 void* (*OSMboxPED) (OS_EVENT *pevent, INT16U timeout, INT8U *perr);//OSMboxPend
typedef	 void (*CAN1FilterInit) (uint8 u8FilterGroup, uint32 u32FilterId);//fun_can1_Filter_init

/*
********************************************************************************
Import function
********************************************************************************
*/
LIB_COMM_GVAR_EXT	OSDELAY	pfOSdelay;
LIB_COMM_GVAR_EXT	OSMboxPST pfOSboxpst;
LIB_COMM_GVAR_EXT	OSMboxPED	pfOSboxped;
LIB_COMM_GVAR_EXT	CAN1FilterInit pfCAN1FilterInit;	

/*
********************************************************************************
Library global variable interface.
********************************************************************************
*/
//Import parameter.
LIB_COMM_GVAR_EXT	const TYPEDEF_MODE_CELL *g_pModeLibTab_lib;
LIB_COMM_GVAR_EXT	const TYPEDEF_2WORD4BYTE	*g_unnu32FrameIdLib_lib;
LIB_COMM_GVAR_EXT	const TYPEDEF_2WORD4BYTE	*g_unnu32FilterId_lib;
LIB_COMM_GVAR_EXT	const TYPEDEF_ISO14230_COMMUNICATION_PAR *g_stIsoKlineCommLib_lib;
LIB_COMM_GVAR_EXT	const TYPEDEF_ISO15765_ACTIVE_PAR *g_stIso15765ActiveLib_lib;
LIB_COMM_GVAR_EXT	OS_EVENT  *g_pstEcuToPerformer_lib;
//Export parameter.
LIB_COMM_GVAR_EXT	TYPEDEF_FRAME_STATUS g_commstaus ;//通讯状态
LIB_COMM_GVAR_EXT	uint8   g_u8SentDataToEcu[255]; //发送数据给ECU缓冲区
LIB_COMM_GVAR_EXT	TYPEDEF_PROTOCOL_MSGBUF g_stDataFromEcu;  //接收数据缓冲区
LIB_COMM_GVAR_EXT	TYPEDEF_PROTOCOL_MSGBUF g_stu8CacheData[3]; //缓冲区就是将ECU暂时就这么多中数据存储
LIB_COMM_GVAR_EXT	TYPEDEF_ISO14230_RECEIVE g_stUsart3Iso14230ReceiveVar ; //串口3-14230接收数据需要用上的变量
LIB_COMM_GVAR_EXT	TYPEDEF_CANMSG g_stCAN115765ReceiveVar ;
LIB_COMM_GVAR_EXT	TYPEDEF_CANMSG g_stCAN115765ReceiveVar1 ;
LIB_COMM_GVAR_EXT	TYPEDEF_CANMSG *g_stCAN115765ReceiveVar_temp;
LIB_COMM_GVAR_EXT	CanTxMsg g_stTxMessage;//CAN
LIB_COMM_GVAR_EXT	CanTxMsg g_stTxMessage1;//CAN
LIB_COMM_GVAR_EXT	uint8 g_u8ProtocolLibOffset;
LIB_COMM_GVAR_EXT	const TYPEDEF_FRAME_STATUS g_enumFrameFlgArr[];


/*
********************************************************************************
Library function interface.
********************************************************************************
*/
//ISO_15765
#define DEF_CAN500K	0x00070206
#define DEF_CAN250K	0x000D0208
#define DEF_CAN125K	0x000D0216
#define DEF_CAN833K	0x000a0510
#define DEF_CAN50K	0x00090420

LIB_COMM_FUN_EXT	void *pfun_iso15765_send_command_group( void *pCmdId, void *pArg1 );
LIB_COMM_FUN_EXT	void fun_iso_15765_send_times( uint8 u8CmdId, void **pCommStatus );
LIB_COMM_FUN_EXT	void fun_iso_15765_send_cmd( uint8 u8CmdId );
LIB_COMM_FUN_EXT	void *pfun_iso_15765_receive( void *pCacheOffset, void *pArg1 );
LIB_COMM_FUN_EXT	void *pfun_iso_15765_check( void*pArg0, void*pArg1 );
LIB_COMM_FUN_EXT	void *pfun_iso_15765_check1( void*pArg0, void*pArg1 );

//ISO_14230
LIB_COMM_FUN_EXT	void fun_iso_14230_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime );
LIB_COMM_FUN_EXT	void*pfun_iso_14230_receive( void *pCacheOffset, void *pArg1 );
LIB_COMM_FUN_EXT	void fun_iso_14230_cmd_load( uint8 u8Cmdid, uint8 u8FrameLen , uint8 *pCmdBuf );
LIB_COMM_FUN_EXT	void*pfun_iso_14230_80_check( void*pArg0, void*pArg1 );
LIB_COMM_FUN_EXT	void*pfun_iso_14230_80_check1( void*pArg0, void*pArg1 );
LIB_COMM_FUN_EXT	void*pfun_iso_14230_OBD_check( void*pArg0, void*pArg1 );
LIB_COMM_FUN_EXT	void*pfun_iso14230_send_command_group( void *pCmdId, void *pArg1 );
LIB_COMM_FUN_EXT	void*pfun_iso14230_send_command_group1( void *pCmdId, void *pArg1 );
LIB_COMM_FUN_EXT	void fun_iso_14230_send_times( uint8 u8CmdId, uint8 u8FrameLen, void **pCommStatus );
LIB_COMM_FUN_EXT	void fun_iso_14230_send_times1( uint8 u8CmdId,uint8 u8FrameLen, uint8 u8CacheOffset,uint8 u8ReceivePack, void **pCommStatus );
LIB_COMM_FUN_EXT	void publicfun_block_character_sc( uint16 DataLen, uint8 *pData );
LIB_COMM_FUN_EXT	bool bfun_iso_14230_negative_response( void );

//ISO_9141
LIB_COMM_FUN_EXT	void*pfun_iso_9141_68_check( void*pArg0, void*pArg1 );
LIB_COMM_FUN_EXT	void*pfun_iso_9141_send_command_group( void *pCmdId, void *prg1 );
LIB_COMM_FUN_EXT	void fun_iso_9141_send_times( uint8 cmdid, uint8 u8FrameLen, void **pCommStatus );
LIB_COMM_FUN_EXT	void fun_iso_9141_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime );
//ISO_1281
LIB_COMM_FUN_EXT	void fun_iso_1281_cmd_load( uint8 u8Cmdid, uint8 u8FrameLen , uint8 *pCmdBuf );
LIB_COMM_FUN_EXT	void fun_iso_1281_send_receive( uint8 u8CmdId, uint8 u8FrameLen, void **pCommStatus );
LIB_COMM_FUN_EXT	void*pfun_iso_1281_check( void*pArg0, void*pArg1 );
LIB_COMM_FUN_EXT	void *pfun_iso_1281_send_cmd( uint8 u8CmdId, uint8 u8FrameLen, uint8 u8ByteTime );
LIB_COMM_FUN_EXT	void*pfun_iso1281_send_command_group( void *pCmdId, void *pArg1 );
LIB_COMM_FUN_EXT	void*pfun_iso_1281_receive( void *pArg0, void *pArg1 );

//gm can
LIB_COMM_FUN_EXT	void  fun_gm_send_times( uint8 u8CmdId, void **pCommStatus );
LIB_COMM_FUN_EXT	void *pfun_gm_check( void*pArg0, void*pArg1 );
LIB_COMM_FUN_EXT	void *pfun_gm_send_command_group( void *pCmdId, void *pArg1 );


#endif
