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
*                                     		hal_ProcessQUE_h.h
*                                              with the
*                                   			 Y05D Board
*
* Filename      : hal_ProcessQUE_h.h
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

#ifndef  HAL_PROCESSQUE_H_H
#define  HAL_PROCESSQUE_H_H

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   HAL_PROCESSQUE_GLOBLAS
#define  HAL_PROCESSQUE_EXT
#else
#define  HAL_PROCESSQUE_EXT  extern
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
// ����GPS ���ݻ�����д�С
#define	GPS_RAMDATA_SIZE							(3)							// GPS RAM���ݻ������洢���д�С
#define	GPS_RAMSMS_SIZE								(3)							// GPS RAM���Ż������洢���д�С
#define	GPS_FLASHDATA_SIZE						(1296)					// GPS FLASH�������洢���д�С
//#define	GPS_FLASHDATA_SIZE						(50)


// ��־����
#define	GPS_INITED_FLAG								((u16)0x8888)		// INIT0+INIT1 ��ʾGPS�������ݴ洢���Ѿ���ʼ����(2015-0-9 ���нṹ�޸�)
#define	GPS_QUEUSED_FLAG							((u16)0xA55A)		// FLAG0+FLAG1 ��ʾGPS���нṹ�Ѿ�ʹ�ù�

// ����GPS�������ݶ��нṹ�洢ռ����������
#define	GPS_QUESECTOR_NUM							(3)												// ѭ������Ϊ�������ڰ���ҳ����(16)*ѭ������(3)=48��
#define	GPS_QUE_LOOPS									(GPS_QUESECTOR_NUM * SPI_FLASH_SectorSize / SPI_FLASH_PageSize)	
																																// ����1����Ϊ��ʼ����־�洢����ʵ��ҳ��Ϊ48ҳѭ������Ϊ48��
// �ⲿ��FLASH��ַ���ֶ���////////////////////////
#define	FLASH_SPI_ALL_SIZE						(SPI_FLASH_Endaddr+1)			// ��FLASH�ײ���������
#define	FLASH_SPI_ADDR_START					(0x000000)
#define	FLASH_SPI_FLAG1_START					(FLASH_SPI_ADDR_START)		// SPI FLASH��־0��ʼ��ַ
#define	FLASH_SPI_FLAG1_SIZE					(SPI_FLASH_SectorSize)		// ��־0ռ�ÿռ��С(4K)Ԥ����
#define	FLASH_SPI_FLAG2_START					(FLASH_SPI_FLAG1_START + FLASH_SPI_FLAG1_SIZE)
#define	FLASH_SPI_FLAG2_SIZE					(SPI_FLASH_SectorSize)		// ��־1ռ�ÿռ��С(4K)Ԥ����
#define	FLASH_SPI_NAME1_START					(FLASH_SPI_FLAG2_START + FLASH_SPI_FLAG2_SIZE)	
#define	FLASH_SPI_NAME1_SIZE					(SPI_FLASH_SectorSize)		// ���򱸷�λ��1����ռ�ÿռ��С(4K)
#define	FLASH_SPI_SIZE1_START					(FLASH_SPI_NAME1_START + FLASH_SPI_NAME1_SIZE)	
#define	FLASH_SPI_SIZE1_SIZE					(SPI_FLASH_SectorSize)		// ���򱸷�λ��1��Сռ�ÿռ��С(4K)
#define	FLASH_SPI_APP1_START					(FLASH_SPI_SIZE1_START + FLASH_SPI_SIZE1_SIZE)	
#define	FLASH_SPI_APP1_SIZE						(SPI_FLASH_SectorSize*80)	// ���򱸷�λ��1��ռ�ÿռ��С(4K*80=320K)
#define	FLASH_SPI_NAME2_START					(FLASH_SPI_APP1_START  + FLASH_SPI_APP1_SIZE)	
#define	FLASH_SPI_NAME2_SIZE					(SPI_FLASH_SectorSize)		// ���򱸷�λ��2����ռ�ÿռ��С(4K)
#define	FLASH_SPI_SIZE2_START					(FLASH_SPI_NAME2_START + FLASH_SPI_NAME2_SIZE)	
#define	FLASH_SPI_SIZE2_SIZE					(SPI_FLASH_SectorSize)		// ���򱸷�λ��2��Сռ�ÿռ��С(4K)
#define	FLASH_SPI_APP2_START					(FLASH_SPI_SIZE2_START + FLASH_SPI_SIZE2_SIZE)	
#define	FLASH_SPI_APP2_SIZE						(SPI_FLASH_SectorSize*80)	// ���򱸷�λ��2��ռ�ÿռ��С(4K*80=320K)
#define	FLASH_SPI_GPSQUE_INIT_START		(FLASH_SPI_APP2_START	 + FLASH_SPI_APP2_SIZE)	// ���г�ʼ����־�洢��ַ
#define	FLASH_SPI_GPSQUE_INIT_SIZE		(SPI_FLASH_SectorSize)		// ��ʼ����־ռ��һ������
#define	FLASH_SPI_GPSQUE_START				(FLASH_SPI_GPSQUE_INIT_START + FLASH_SPI_GPSQUE_INIT_SIZE)
#define	FLASH_SPI_GPSQUE_SIZE					(SPI_FLASH_SectorSize*GPS_QUESECTOR_NUM)	// GPS�������ݶ��нṹռ�ÿռ��С
#define	FLASH_SPI_GPSDATA_START				(FLASH_SPI_GPSQUE_START	+ FLASH_SPI_GPSQUE_SIZE)
#define	FLASH_SPI_GPSDATA_SIZE				(SPI_FLASH_Endaddr - FLASH_SPI_GPSDATA_START + 1)	// GPS��������ʣ��洢�ռ��С

#if (GPS_FLASHDATA_SIZE <= (FLASH_SPI_GPSDATA_SIZE / 256))	// ���������������ݳ���FLASH�洢�ռ���ʹ��ʵ�ʴ�С
#define	GPS_FLASHDATA_USE							(GPS_FLASHDATA_SIZE - GPS_FLASHDATA_SIZE % 16)	// ���д�С����Ϊ16��������
#else
#define	GPS_FLASHDATA_USE							((FLASH_SPI_GPSDATA_SIZE/SPI_FLASH_PageSize) - ((FLASH_SPI_GPSDATA_SIZE/SPI_FLASH_PageSize) % 16))		// FLASHÿҳ�洢һ��GPS��				
#endif			

/////////////////////////////////////////////////////////////////////
// ����ʧ�����Դ�������
#define	DEF_TRYTIMES_FLASHINIT				(3)				// SPI flash��ʼ�������Դ���
#define	DEF_TRYTIMES_FLASHREAD				(3)				// SPI flash�����Դ���
#define	DEF_TRYTIMES_FLASHWRITE				(3)				// SPI flashд���Դ���	


//////////////////////////////////////////////////////////////////////

// ����ͨ�ö��� 
#define	DEF_PROQUE_QUE_OK							((s8)0)
#define	DEF_PROQUE_QUE_FULL						((s8)-1)
#define	DEF_PROQUE_QUE_EMPTY					((s8)-2)
#define	DEF_PROQUE_QUE_OVERFLOW				((s8)-3)
#define	DEF_PROQUE_QUE_ERR						((s8)-4) 
#define	DEF_PROQUE_QUE_WCMPERR				((s8)-5)

// ����ģʽ����
#define	DEF_RAMQUE_LOST_MODE					(1)				// �������������˶������������			
#define	DEF_FLASHQUE_LOST_MODE				(1)				// ���������������Զ��������������			


/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

typedef enum {		
		DEF_RAM_QUE = 0,								// �������Ͷ���-RAM���ݶ���		
		DEF_SMS_QUE,										// �������Ͷ���-RAM���Ŷ���		
		DEF_FLASH_QUE,									// �������Ͷ���-FLASH����	
		DEF_ALL_QUE,										// �������Ͷ���-���ж���	
}ENUM_QUE_TYPE;

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// �������ݰ��ṹ

// ����·������
#define	DEF_PATH_BT										((u8)2)
#define	DEF_PATH_SMS									((u8)1)
#define	DEF_PATH_GPRS									((u8)0)

// ��Ӵ��ʱʹ�õ��������Ͷ��
#define	DEF_TYPE_ACK									((u8)0xf0)					// ͨ��Ӧ������
#define	DEF_TYPE_GPS									((u8)0x01)					// GPS��������
#define	DEF_TYPE_IG										((u8)0x02)					// ���/Ϩ����������
#define	DEF_TYPE_ZITAI								((u8)0x03)					// ��̬��������
#define	DEF_TYPE_LOG									((u8)0x04)					// LOG��������
#define	DEF_TYPE_ALARM								((u8)0x05)					// ������������
#define	DEF_TYPE_24REPORT							((u8)0x06)					// 24Сʱ��������������
#define	DEF_TYPE_QUEPARA							((u8)0x07)					// ������ѯ������������
#define	DEF_TYPE_VIN									((u8)0x08)					// VIN������������
#define	DEF_TYPE_CARCON								((u8)0x09)					// �������Ʒ�����������
#define	DEF_TYPE_CLRCODE							((u8)0x0A)					// ������뷵����������
#define	DEF_TYPE_CARSTA								((u8)0x0B)					// ����״̬������������
#define	DEF_TYPE_QUEPHO								((u8)0x0C)					// ���ſ��ƺ����ѯ������������
#define	DEF_TYPE_SMSCOM								((u8)0x0D)					// ���ų������Ʒ�����������
#define	DEF_TYPE_POWON								((u8)0x10)					// �豸�ϵ籨����������

// SMS���ݰ��ṹ�������Ͷ��� 
#define	DEF_SMS_COM						        ((u8)0)      				// ͨ�����Ͷ���
#define	DEF_SMS_ALARM						      ((u8)1)      				// �����۶���

#define	DEF_PACK_BUF_SIZE					    (200)             	// �洢��ӵ����ݲ�������СӦ��С��(256-26)
typedef struct	tag_SendData_Def
{
		u16			sum;																					// �洢�����������ۼӺ�(path~data[])
		u8			path;																					// DEF_PATH_GPRS/SMS 
		u8			type;																					// �������ͼ������������Ͷ���(DEF_TYPE_ACK~DEF_TYPE_CARSTA)
		u8			smsAlType;																		// �ظ��Ķ������ͣ�ͨ�û��Ǳ���/path=SMSʱ��Ч
		u8			smsMode;																			// �ظ��Ķ����ַ�ģʽ��GSM����UCS2/path=SMSʱ��Ч		
		u8			smsNum[20];																		// �ظ����ŵĵ绰����/path=SMSʱ��Ч		
		u16			len;																					//
		u8			data[DEF_PACK_BUF_SIZE];											// ���ΪGPRS������Ϊ���������ݣ����ΪSMS��Ϊʵ�ʻظ��Ķ����������� 
							
}SendData_TypeDef;

//////////////////////////////////////////////////////////////////////
// ���Ͷ��нṹ
typedef struct	tag_DataQue_Def
{
		u16			useFlag;																			// ������Ч��־(����ָʾ���¶��нṹ�Ƿ�洢��Ϣ�Ƿ���Ч)
		u16			sum;																					// ���������ۼӺ�У���ֽ�(FLASH������ҪУ�飬Ram���в�ʹ��)
		u16			start;																				// ���п�ʼλ��
		u16			end;																					// ���н���λ��
		u16			in;																						// ����ͷָ��
		u16			out;																					// ����βָ��
		u16			size;																					// �����ܴ�С
		u16			entries;																			// ���е�ǰ�洢���ݰ���
		u16			outExt;																				// ������ʱoutָ��(2015-9-29 ��Ҫ���FLASH����)
		u16			entriesExt;																		// ������ʱ��С(2015-9-29 ��Ҫ���FLASH����)
							
}DataQue_TypeDef;
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

HAL_PROCESSQUE_EXT		DataQue_TypeDef				s_queRam;							// RAM�д洢����
HAL_PROCESSQUE_EXT		DataQue_TypeDef				s_queSms;							// SMS�д洢����
HAL_PROCESSQUE_EXT		DataQue_TypeDef				s_queFlash;						// FLASH�д洢����
HAL_PROCESSQUE_EXT		SendData_TypeDef			s_ramData[GPS_RAMDATA_SIZE];		// RAM���ݻ�����		
HAL_PROCESSQUE_EXT		SendData_TypeDef			s_ramSms[GPS_RAMSMS_SIZE];			// RAM���Ż�����	

			
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

// ����Ӧ�ú���
s8				HAL_ComQue_init 								(ENUM_QUE_TYPE queType);
u16	 			HAL_ComQue_size 								(ENUM_QUE_TYPE queType);
s8	 			HAL_ComQue_in 									(ENUM_QUE_TYPE queType,	SendData_TypeDef	*sendData);
s8	 			HAL_ComQue_out 									(ENUM_QUE_TYPE queType,	SendData_TypeDef	*sendData);
s8	 			HAL_ComQue_outend 							(ENUM_QUE_TYPE queType);
s8	 			HAL_ComQue_outFinish 						(ENUM_QUE_TYPE queType,	u16 lastOut,	u16 outNum);
s8				HAL_ComQue_ForceReset 					(ENUM_QUE_TYPE queType);
s8	 			HAL_ComQue_SemPend 							(u16 timeout);
s8	 			HAL_ComQue_SemPost 							(void);


/*
*********************************************************************************************************
*                                               MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of module include.                               */
