/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                            (c) Copyright 2007-2008; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        APPLICATION CODE
*
*                                     	 hal_ProcessCON_h.h
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessCON_h.h
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*********************************************************************************************************
*/
#ifndef  HAL_PROCESSCON_H_H
#define  HAL_PROCESSCON_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   HAL_PROCESSCON_GLOBLAS
#define  HAL_PROCESSCON_EXT
#else
#define  HAL_PROCESSCON_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include 	"stm32f10x.h"

/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/

// CON接口定义
#define	DEF_CON_BAUT_RATE			        (2400)			// 外部控制串口通信波特率
#define	DEF_CON_UART					        DEF_UART_5
#define	DEF_CON_REMAP					        DEF_USART_REMAP_NOUSE	
#define	DEF_CON_BUFSIZE				        (20)					//	协议数据最大长度(接收数据缓冲区大小)
#define	DEF_CON_RXDDELAY_TIMEOUT		  ((u32)50)		// 接收分包时间间隔

/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

// 外围控制模块硬件状态定义(CON_DEVICE.HardWareSta)
#define	DEF_CONHARD_COMMU_ERR		      ((s8)-1)
#define	DEF_CONHARD_NONE_ERR		      ((s8)0)	
#define	DEF_CONHARD_CHEKING			      ((s8)1)	

#ifdef HAL_PROCESSCON_GLOBLAS
#define    PACK_HEAD               		0xC0AAu
#define    PACK_ADDR               		0u                     //协议中定义的数据地址，目前只用0
#define    PACK_LEN                		10u                    //PDU长度
#define    PACK_TMR_OUT            		100                    //数据包应答超时时间ms
#define    PACK_CRC_CHK_DATA_LEN   		6u                     //CRC需要检测的数据长度

#define    SUCCESS								 		1u
#define    FAIL                    		0u
#define    BUSY                    		0xFF

#define    LOCK                    		1u
#define    UNLOCK                  		0u

#define    PACK_DATA_TYPE_ACK      		0x80u
#define    PACK_REV_ACK_PARA1      		0x80u                   //成功
#define    PACK_REV_ACK_PARA2      		0x00u

#define    PACK_DATA_TYPE_NAK      		0x81u
#define    PACK_SEND_NAK_PARA1     		0x81u                   //无法识别的指令或数据
#define    PACK_SEND_NAK_PARA2     		0x00u                   //保留
#define    PACK_REV_NAK_PARA1_1    		0x80u                   //授权限制
#define    PACK_REV_NAK_PARA1_2    		0x81u                   //无法识别的指令或数据
#define    PACK_REV_NAK_PARA1_3    		0x82u                   //当前模式不支持此操作
#define    PACK_REV_NAK_PARA1_4    		0x83u                   //脚刹车踩下不支持此操作
#define    PACK_REV_NAK_PARA1_5    		0x84u                   //手刹车不支持此操作
#define    PACK_REV_NAK_PARA2      		0x00u                   //保留

#define    PACK_DATA_TYPE_GUARD    		0x82u
#define    PACK_SEND_GUARD_PARA1_1   	0x80u                   //设防
#define    PACK_SEND_GUARD_PARA1_2   	0x81u                   //撤防
#define    PACK_SEND_GUARD_PARA1_3   	0x82u                   //寻车
#define    PACK_SEND_GUARD_PARA1_4   	0xFFu                   //查询状态
#define    PACK_SEND_GUARD_PARA2     	0x00u                   //保留
#define    PACK_REV_GUARD_PARA1_1    	0x80u                   //设防
#define    PACK_REV_GUARD_PARA1_2    	0x81u                   //撤防
#define    PACK_REV_GUARD_PARA1_3    	0x82u                   //行驶中
#define    PACK_REV_GUARD_PARA2      	0x00u                   //保留


#define    PACK_DATA_TYPE_DOOR     		0x83u
#define    PACK_SEND_DOOR_PARA1_1  		0x80u                   //锁门
#define    PACK_SEND_DOOR_PARA1_2 	  0x81u                   //开门
#define    PACK_SEND_DOOR_PARA1_3  		0x82u                   //开后备箱
#define    PACK_SEND_DOOR_PARA1_4  		0xFFu                   //查询状态
#define    PACK_SEND_DOOR_PARA2    		0x00u                   //保留
#define    PACK_REV_DOOR_PARA1_1   		0x80u                   //所有车门已关闭
#define    PACK_REV_DOOR_PARA1_2   		0x81u                   //车门未关闭（非法开门报警）
#define    PACK_REV_DOOR_PARA2     		0x00u                   //保留

#define    PACK_DATA_TYPE_ENGINE      0x84u
#define    PACK_SEND_ENGINE_PARA1_1   0x80u                   //关闭
#define    PACK_SEND_ENGINE_PARA1_2   0x81u                   //ACC
#define    PACK_SEND_ENGINE_PARA1_3   0x82u                   //ON
#define    PACK_SEND_ENGINE_PARA1_4   0x83u                   //启动引擎
#define    PACK_SEND_ENGINE_PARA1_5   0xFFu                   //查询状态
#define    PACK_SEND_ENGINE_PARA2     0x00u                   //保留
#define    PACK_REV_ENGINE_PARA1_1    0x80u                   //关闭
#define    PACK_REV_ENGINE_PARA1_2    0x81u                   //ACC
#define    PACK_REV_ENGINE_PARA1_3    0x82u                   //ON
#define    PACK_REV_ENGINE_PARA1_4    0x83u                   //引擎发动中
#define    PACK_REV_ENGINE_PARA1_5    0x84u                   //等待启动
#define    PACK_REV_ENGINE_PARA2_1    0x00u                   //无意义
#define    PACK_REV_ENGINE_PARA2_2    0x01u                   //引擎启动成功
#define    PACK_REV_ENGINE_PARA2_3    0x00u                   //引擎未能启动


#define    PACK_TMROUT                500u                    //数据包发送超时时间
#define    PACK_ALARM_TMROUT          2000u                   //报警阀值时间
#define    PACK_RESEND_TMRS           3u                      //数据包重发次数

#define    PACK_DEVICE_UNKNOW         0u                      //外接设备初始化为未知状态
#define    PACK_NO_ANSWER             1u                      //外接设备没有应答
#define    PACK_SENDING_ST            2u                      //正在向外接设备发送指令
#define    PACK_REV_ACK               3u                      //外接设备回复肯定应答
#define    PACK_REV_NAK               4u                      //外接设备回复否定应答

#define    CAR_ST_UNKNOW              0u
#define    CAR_ST_OPEN                1u
#define    CAR_ST_CLOSE               2u

#define    DEVICE_CHECK_EN            0u

#define    RDST_RST_CAR_ST_EN        1u

#define    IG_ERR_ACK_FILTER_EN       0u


#define SEND_ACK()                    CmdSend(PACK_DATA_TYPE_ACK,    GetHour(),               GetMinute()        )                    //发送ACK
#define SEND_NAK()                    CmdSend(PACK_DATA_TYPE_NAK,    PACK_SEND_NAK_PARA1,     PACK_SEND_NAK_PARA2)										//发送NAK

#define SEND_SET_GUARD()              CmdSend(PACK_DATA_TYPE_GUARD,  PACK_SEND_GUARD_PARA1_1,  PACK_SEND_GUARD_PARA2)  								//设防 
#define SEND_CLR_GUARD()              CmdSend(PACK_DATA_TYPE_GUARD,  PACK_SEND_GUARD_PARA1_2,  PACK_SEND_GUARD_PARA2)    							//撤防
#define SEND_SEARCH_CAR()             CmdSend(PACK_DATA_TYPE_GUARD,  PACK_SEND_GUARD_PARA1_3,  PACK_SEND_GUARD_PARA2)   							//寻车 
#define SEND_GET_GUARD_ST()           CmdSend(PACK_DATA_TYPE_GUARD,  PACK_SEND_GUARD_PARA1_4,  PACK_SEND_GUARD_PARA2)    							//读取防盗状态   
  
#define SEND_OPEN_TRUNK()             CmdSend(PACK_DATA_TYPE_DOOR,   PACK_SEND_DOOR_PARA1_3,   PACK_SEND_DOOR_PARA2)    							//打开后备箱 
#define SEND_GET_DOOR_ST()            CmdSend(PACK_DATA_TYPE_DOOR,   PACK_SEND_DOOR_PARA1_4,   PACK_SEND_DOOR_PARA2)  								//读取车门状态

#define SEND_ENGINE_SHUT()						CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_1, PACK_SEND_ENGINE_PARA2) 								//关闭引擎
#define SEND_ENGINE_ACC()						  CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_2, PACK_SEND_ENGINE_PARA2) 								//ACC
#define SEND_ENGINE_ON()						  CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_3, PACK_SEND_ENGINE_PARA2) 								//ON
#define SEND_ENGINE_START()						CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_4, PACK_SEND_ENGINE_PARA2) 								//启动引擎
#define SEND_ENGINE_ST_GET()					CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_5, PACK_SEND_ENGINE_PARA2)								//读取引擎状态
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 硬件故障结构定义
typedef	struct tag_CON_DEVICE_STA
{
		s8	          InitSta;											// 初始化状态(1:表示初始化成功有效，0:表示初始化失败无效
		s8	          HardWareSta;							
			
}CON_DEVICE_STA_Typedef;


//////////////////////////////////////////////////////////////////////////////////////////////////////
// 控制串口接收数据结构定义(用于底层串口接收)
typedef struct	tag_CONPort_Def
{	
		vu8			      rxdStep;
		vu32		      rxdTimer;										// 接收超时计时器
		u16			      rxLen;
		u8			      rxdBuf[DEF_CON_BUFSIZE];
			 
}CONPort_TypeDef;	


// 底层通信收发数据结构(用于外设底层收发数据传递使用，用于接收邮箱)
typedef struct	tag_CONComm_Def
{
		u8			      flag;
		u16			      len;
		u8			      dataBuf[DEF_CON_BUFSIZE];
					 
}CONComm_TypeDef;


typedef __packed struct 
{
    u8            ucSd       : 4;            //指令流水号
    u8            ucAddr     : 2;            //数据地址
    u8            ucReserved : 2;            //保留
}STU_ADDR_SD;

typedef __packed union
{
    STU_ADDR_SD  stuAddrSd;                  //设备地址和流水号的位域表示
    u8           ucAddrAndSd;                //设备地址和流水号的字节表示
}UNION_ADDR_SD;

typedef __packed struct
{
    u16           usStartFlag;               //协议起始符0xC0AA
    UNION_ADDR_SD unionAddrSd;               //设备地址和流水号
    u16           usSerialNum;               //Y06模块序列号或外接设备数据
    u8            ucDataType;                //数据类型
    u8            ucPara1;                   //参数1
    u8            ucPara2;                   //参数2
    u16           usCrc;                     //CRC校验和
}STU_DATA_PACK;

typedef struct
{
    u8            ucSd;                      //上次指令的流水号
    u8            ucDataLock;                //buffer数据锁
    STU_DATA_PACK stuPackdata;               //协议数据包PDU
}STU_PACK_INFO;

typedef struct
{
		u8            ucValid;                   //模块有效
    u8            ucRdCarStFlag;             //读取车辆状态标识
		u8            ucSendFailCnt;             //发送失败计数器
		u8            ucAckType;                 //应答类型
		u8            ucNakSype;                 //否定应答类型
		u8            ucGuardSt;                 //防盗状态
		u8            ucDoorSt;                  //车门状态
		u8            ucRunSt;                   //行驶状态
		u8            ucEngienSt;                //引擎状态
}STU_MODULE_ST;

typedef struct
{
    u32           uiGuardSt     :   2;       //防盗状态  (0:未知，1：设防，2：撤防)
    u32           uiEngienSt    :   2;       //引擎状态  (0:未知，1：启动，2：关闭, 3:ON)
    u32           uiLFDoorSt    :   2;       //左前门状态(0:未知，1：打开，2：关闭)
    u32           uiRFDoorSt    :   2;       //右前门状态(0:未知，1：打开，2：关闭)
    u32           uiLBDoorSt    :   2;       //左后门状态(0:未知，1：打开，2：关闭)
    u32           uiRBDoorSt    :   2;       //右后门状态(0:未知，1：打开，2：关闭)
    u32           uiTrunk       :   2;       //后备箱状态(0:未知，1：打开，2：关闭)
    u32           uiSkylight    :   2;       //天窗状态  (0:未知，1：打开，2：关闭)
    u32           uiLFWindow    :   2;       //左前窗状态(0:未知，1：打开，2：关闭)
    u32           uiRFWindow    :   2;       //右前窗状态(0:未知，1：打开，2：关闭)
    u32           uiLBWindow    :   2;       //左后窗状态(0:未知，1：打开，2：关闭)
    u32           uiRBWindow    :   2;       //右后窗状态(0:未知，1：打开，2：关闭)
    u32           uiReserved    :   8;       //保留
}STU_CAR_ST;
	
//////////////////////////////////////////////////////////////////////////////////////////////////////
// CON应用层控制数数据结构(用于应用层控制应答CON数据结构，用于应答发送邮箱)

	
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/	
HAL_PROCESSCON_EXT		CON_DEVICE_STA_Typedef	CON_DEVICE;						  // 外围控制模块硬件状态
HAL_PROCESSCON_EXT		CONPort_TypeDef				  s_conPort;							// CON底层接收全局变量
HAL_PROCESSCON_EXT		CONComm_TypeDef				  s_rxdData;							// 串口底层收数据结构
HAL_PROCESSCON_EXT		vu16									  conSn;									// 外设交互流水号 
HAL_PROCESSCON_EXT		u8											glob_illOpenFlag;				// 非法开门报警标志
		
/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void			 HAL_CON_UpDataSn 					(vu16 *sn);
void			 HAL_CONRES_IT_CON					(u8	newSta);
void			 HAL_CONTXD_IT_CON					(u8	newSta);
void			 HAL_CONUART_RxTxISRHandler (void);

void			 HAL_CON_Init								(void);
void			 HAL_CON_UARTSendCmd 				(u8 *pBuf,	u16	len);
void			 HAL_CON_RxdProcess 				(u8	*pRxdBuf,	u16	RxdLen);
void	 		 HAL_CON_TimerServer 				(void);


void       HAL_ModuleInit             (void);
void       HAL_CONMainLoop            (void);
STU_CAR_ST HAL_CarStGet               (void);

u8 				 HAL_ConTest								(void);

/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
