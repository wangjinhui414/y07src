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

// CON�ӿڶ���
#define	DEF_CON_BAUT_RATE			        (2400)			// �ⲿ���ƴ���ͨ�Ų�����
#define	DEF_CON_UART					        DEF_UART_5
#define	DEF_CON_REMAP					        DEF_USART_REMAP_NOUSE	
#define	DEF_CON_BUFSIZE				        (20)					//	Э��������󳤶�(�������ݻ�������С)
#define	DEF_CON_RXDDELAY_TIMEOUT		  ((u32)50)		// ���շְ�ʱ����

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

// ��Χ����ģ��Ӳ��״̬����(CON_DEVICE.HardWareSta)
#define	DEF_CONHARD_COMMU_ERR		      ((s8)-1)
#define	DEF_CONHARD_NONE_ERR		      ((s8)0)	
#define	DEF_CONHARD_CHEKING			      ((s8)1)	

#ifdef HAL_PROCESSCON_GLOBLAS
#define    PACK_HEAD               		0xC0AAu
#define    PACK_ADDR               		0u                     //Э���ж�������ݵ�ַ��Ŀǰֻ��0
#define    PACK_LEN                		10u                    //PDU����
#define    PACK_TMR_OUT            		100                    //���ݰ�Ӧ��ʱʱ��ms
#define    PACK_CRC_CHK_DATA_LEN   		6u                     //CRC��Ҫ�������ݳ���

#define    SUCCESS								 		1u
#define    FAIL                    		0u
#define    BUSY                    		0xFF

#define    LOCK                    		1u
#define    UNLOCK                  		0u

#define    PACK_DATA_TYPE_ACK      		0x80u
#define    PACK_REV_ACK_PARA1      		0x80u                   //�ɹ�
#define    PACK_REV_ACK_PARA2      		0x00u

#define    PACK_DATA_TYPE_NAK      		0x81u
#define    PACK_SEND_NAK_PARA1     		0x81u                   //�޷�ʶ���ָ�������
#define    PACK_SEND_NAK_PARA2     		0x00u                   //����
#define    PACK_REV_NAK_PARA1_1    		0x80u                   //��Ȩ����
#define    PACK_REV_NAK_PARA1_2    		0x81u                   //�޷�ʶ���ָ�������
#define    PACK_REV_NAK_PARA1_3    		0x82u                   //��ǰģʽ��֧�ִ˲���
#define    PACK_REV_NAK_PARA1_4    		0x83u                   //��ɲ�����²�֧�ִ˲���
#define    PACK_REV_NAK_PARA1_5    		0x84u                   //��ɲ����֧�ִ˲���
#define    PACK_REV_NAK_PARA2      		0x00u                   //����

#define    PACK_DATA_TYPE_GUARD    		0x82u
#define    PACK_SEND_GUARD_PARA1_1   	0x80u                   //���
#define    PACK_SEND_GUARD_PARA1_2   	0x81u                   //����
#define    PACK_SEND_GUARD_PARA1_3   	0x82u                   //Ѱ��
#define    PACK_SEND_GUARD_PARA1_4   	0xFFu                   //��ѯ״̬
#define    PACK_SEND_GUARD_PARA2     	0x00u                   //����
#define    PACK_REV_GUARD_PARA1_1    	0x80u                   //���
#define    PACK_REV_GUARD_PARA1_2    	0x81u                   //����
#define    PACK_REV_GUARD_PARA1_3    	0x82u                   //��ʻ��
#define    PACK_REV_GUARD_PARA2      	0x00u                   //����


#define    PACK_DATA_TYPE_DOOR     		0x83u
#define    PACK_SEND_DOOR_PARA1_1  		0x80u                   //����
#define    PACK_SEND_DOOR_PARA1_2 	  0x81u                   //����
#define    PACK_SEND_DOOR_PARA1_3  		0x82u                   //������
#define    PACK_SEND_DOOR_PARA1_4  		0xFFu                   //��ѯ״̬
#define    PACK_SEND_DOOR_PARA2    		0x00u                   //����
#define    PACK_REV_DOOR_PARA1_1   		0x80u                   //���г����ѹر�
#define    PACK_REV_DOOR_PARA1_2   		0x81u                   //����δ�رգ��Ƿ����ű�����
#define    PACK_REV_DOOR_PARA2     		0x00u                   //����

#define    PACK_DATA_TYPE_ENGINE      0x84u
#define    PACK_SEND_ENGINE_PARA1_1   0x80u                   //�ر�
#define    PACK_SEND_ENGINE_PARA1_2   0x81u                   //ACC
#define    PACK_SEND_ENGINE_PARA1_3   0x82u                   //ON
#define    PACK_SEND_ENGINE_PARA1_4   0x83u                   //��������
#define    PACK_SEND_ENGINE_PARA1_5   0xFFu                   //��ѯ״̬
#define    PACK_SEND_ENGINE_PARA2     0x00u                   //����
#define    PACK_REV_ENGINE_PARA1_1    0x80u                   //�ر�
#define    PACK_REV_ENGINE_PARA1_2    0x81u                   //ACC
#define    PACK_REV_ENGINE_PARA1_3    0x82u                   //ON
#define    PACK_REV_ENGINE_PARA1_4    0x83u                   //���淢����
#define    PACK_REV_ENGINE_PARA1_5    0x84u                   //�ȴ�����
#define    PACK_REV_ENGINE_PARA2_1    0x00u                   //������
#define    PACK_REV_ENGINE_PARA2_2    0x01u                   //���������ɹ�
#define    PACK_REV_ENGINE_PARA2_3    0x00u                   //����δ������


#define    PACK_TMROUT                500u                    //���ݰ����ͳ�ʱʱ��
#define    PACK_ALARM_TMROUT          2000u                   //������ֵʱ��
#define    PACK_RESEND_TMRS           3u                      //���ݰ��ط�����

#define    PACK_DEVICE_UNKNOW         0u                      //����豸��ʼ��Ϊδ֪״̬
#define    PACK_NO_ANSWER             1u                      //����豸û��Ӧ��
#define    PACK_SENDING_ST            2u                      //����������豸����ָ��
#define    PACK_REV_ACK               3u                      //����豸�ظ��϶�Ӧ��
#define    PACK_REV_NAK               4u                      //����豸�ظ���Ӧ��

#define    CAR_ST_UNKNOW              0u
#define    CAR_ST_OPEN                1u
#define    CAR_ST_CLOSE               2u

#define    DEVICE_CHECK_EN            0u

#define    RDST_RST_CAR_ST_EN        1u

#define    IG_ERR_ACK_FILTER_EN       0u


#define SEND_ACK()                    CmdSend(PACK_DATA_TYPE_ACK,    GetHour(),               GetMinute()        )                    //����ACK
#define SEND_NAK()                    CmdSend(PACK_DATA_TYPE_NAK,    PACK_SEND_NAK_PARA1,     PACK_SEND_NAK_PARA2)										//����NAK

#define SEND_SET_GUARD()              CmdSend(PACK_DATA_TYPE_GUARD,  PACK_SEND_GUARD_PARA1_1,  PACK_SEND_GUARD_PARA2)  								//��� 
#define SEND_CLR_GUARD()              CmdSend(PACK_DATA_TYPE_GUARD,  PACK_SEND_GUARD_PARA1_2,  PACK_SEND_GUARD_PARA2)    							//����
#define SEND_SEARCH_CAR()             CmdSend(PACK_DATA_TYPE_GUARD,  PACK_SEND_GUARD_PARA1_3,  PACK_SEND_GUARD_PARA2)   							//Ѱ�� 
#define SEND_GET_GUARD_ST()           CmdSend(PACK_DATA_TYPE_GUARD,  PACK_SEND_GUARD_PARA1_4,  PACK_SEND_GUARD_PARA2)    							//��ȡ����״̬   
  
#define SEND_OPEN_TRUNK()             CmdSend(PACK_DATA_TYPE_DOOR,   PACK_SEND_DOOR_PARA1_3,   PACK_SEND_DOOR_PARA2)    							//�򿪺��� 
#define SEND_GET_DOOR_ST()            CmdSend(PACK_DATA_TYPE_DOOR,   PACK_SEND_DOOR_PARA1_4,   PACK_SEND_DOOR_PARA2)  								//��ȡ����״̬

#define SEND_ENGINE_SHUT()						CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_1, PACK_SEND_ENGINE_PARA2) 								//�ر�����
#define SEND_ENGINE_ACC()						  CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_2, PACK_SEND_ENGINE_PARA2) 								//ACC
#define SEND_ENGINE_ON()						  CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_3, PACK_SEND_ENGINE_PARA2) 								//ON
#define SEND_ENGINE_START()						CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_4, PACK_SEND_ENGINE_PARA2) 								//��������
#define SEND_ENGINE_ST_GET()					CmdSend(PACK_DATA_TYPE_ENGINE, PACK_SEND_ENGINE_PARA1_5, PACK_SEND_ENGINE_PARA2)								//��ȡ����״̬
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Ӳ�����Ͻṹ����
typedef	struct tag_CON_DEVICE_STA
{
		s8	          InitSta;											// ��ʼ��״̬(1:��ʾ��ʼ���ɹ���Ч��0:��ʾ��ʼ��ʧ����Ч
		s8	          HardWareSta;							
			
}CON_DEVICE_STA_Typedef;


//////////////////////////////////////////////////////////////////////////////////////////////////////
// ���ƴ��ڽ������ݽṹ����(���ڵײ㴮�ڽ���)
typedef struct	tag_CONPort_Def
{	
		vu8			      rxdStep;
		vu32		      rxdTimer;										// ���ճ�ʱ��ʱ��
		u16			      rxLen;
		u8			      rxdBuf[DEF_CON_BUFSIZE];
			 
}CONPort_TypeDef;	


// �ײ�ͨ���շ����ݽṹ(��������ײ��շ����ݴ���ʹ�ã����ڽ�������)
typedef struct	tag_CONComm_Def
{
		u8			      flag;
		u16			      len;
		u8			      dataBuf[DEF_CON_BUFSIZE];
					 
}CONComm_TypeDef;


typedef __packed struct 
{
    u8            ucSd       : 4;            //ָ����ˮ��
    u8            ucAddr     : 2;            //���ݵ�ַ
    u8            ucReserved : 2;            //����
}STU_ADDR_SD;

typedef __packed union
{
    STU_ADDR_SD  stuAddrSd;                  //�豸��ַ����ˮ�ŵ�λ���ʾ
    u8           ucAddrAndSd;                //�豸��ַ����ˮ�ŵ��ֽڱ�ʾ
}UNION_ADDR_SD;

typedef __packed struct
{
    u16           usStartFlag;               //Э����ʼ��0xC0AA
    UNION_ADDR_SD unionAddrSd;               //�豸��ַ����ˮ��
    u16           usSerialNum;               //Y06ģ�����кŻ�����豸����
    u8            ucDataType;                //��������
    u8            ucPara1;                   //����1
    u8            ucPara2;                   //����2
    u16           usCrc;                     //CRCУ���
}STU_DATA_PACK;

typedef struct
{
    u8            ucSd;                      //�ϴ�ָ�����ˮ��
    u8            ucDataLock;                //buffer������
    STU_DATA_PACK stuPackdata;               //Э�����ݰ�PDU
}STU_PACK_INFO;

typedef struct
{
		u8            ucValid;                   //ģ����Ч
    u8            ucRdCarStFlag;             //��ȡ����״̬��ʶ
		u8            ucSendFailCnt;             //����ʧ�ܼ�����
		u8            ucAckType;                 //Ӧ������
		u8            ucNakSype;                 //��Ӧ������
		u8            ucGuardSt;                 //����״̬
		u8            ucDoorSt;                  //����״̬
		u8            ucRunSt;                   //��ʻ״̬
		u8            ucEngienSt;                //����״̬
}STU_MODULE_ST;

typedef struct
{
    u32           uiGuardSt     :   2;       //����״̬  (0:δ֪��1�������2������)
    u32           uiEngienSt    :   2;       //����״̬  (0:δ֪��1��������2���ر�, 3:ON)
    u32           uiLFDoorSt    :   2;       //��ǰ��״̬(0:δ֪��1���򿪣�2���ر�)
    u32           uiRFDoorSt    :   2;       //��ǰ��״̬(0:δ֪��1���򿪣�2���ر�)
    u32           uiLBDoorSt    :   2;       //�����״̬(0:δ֪��1���򿪣�2���ر�)
    u32           uiRBDoorSt    :   2;       //�Һ���״̬(0:δ֪��1���򿪣�2���ر�)
    u32           uiTrunk       :   2;       //����״̬(0:δ֪��1���򿪣�2���ر�)
    u32           uiSkylight    :   2;       //�촰״̬  (0:δ֪��1���򿪣�2���ر�)
    u32           uiLFWindow    :   2;       //��ǰ��״̬(0:δ֪��1���򿪣�2���ر�)
    u32           uiRFWindow    :   2;       //��ǰ��״̬(0:δ֪��1���򿪣�2���ر�)
    u32           uiLBWindow    :   2;       //���״̬(0:δ֪��1���򿪣�2���ر�)
    u32           uiRBWindow    :   2;       //�Һ�״̬(0:δ֪��1���򿪣�2���ر�)
    u32           uiReserved    :   8;       //����
}STU_CAR_ST;
	
//////////////////////////////////////////////////////////////////////////////////////////////////////
// CONӦ�ò���������ݽṹ(����Ӧ�ò����Ӧ��CON���ݽṹ������Ӧ��������)

	
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/	
HAL_PROCESSCON_EXT		CON_DEVICE_STA_Typedef	CON_DEVICE;						  // ��Χ����ģ��Ӳ��״̬
HAL_PROCESSCON_EXT		CONPort_TypeDef				  s_conPort;							// CON�ײ����ȫ�ֱ���
HAL_PROCESSCON_EXT		CONComm_TypeDef				  s_rxdData;							// ���ڵײ������ݽṹ
HAL_PROCESSCON_EXT		vu16									  conSn;									// ���轻����ˮ�� 
HAL_PROCESSCON_EXT		u8											glob_illOpenFlag;				// �Ƿ����ű�����־
		
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
