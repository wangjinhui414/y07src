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
*                                      	APPLICATION CODE
*
*                                     	hal_ProcessQUE_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : hal_ProcessQUE_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  HAL_PROCESSQUE_GLOBLAS

#include	<string.h>
#include 	"ucos_ii.h"
#include	"bsp_h.h"
#include	"bsp_storage_h.h"
#include	"hal_h.h"
#include	"hal_ProcessQUE_h.h"
#include	"hal_Protocol_h.h"
#include	"cfg_h.h"
#include	"main_h.h"



/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

extern		OS_EVENT					*QueMutexSem;				// ���в��������ź���
static		vu8								QueMutexFlag=1;			//	��OSʱʵ�ֶ��в�������

static		vu16							flashQuePosi=0;	

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


// FLASH�洢��������
static		s8				GPSFlashQueInit_s 						(DataQue_TypeDef *que,	vu16 *quePosi);
static		s8				GPSFlashQueSave_s 						(DataQue_TypeDef *que,	vu16 *quePosi);
static		s8				GPSFlashQueLoad_s 						(DataQue_TypeDef *que,	vu16 *quePosi);
static		s8				GPSFlashDataSave_s 						(u16	index,	SendData_TypeDef *sendData);
static		s8				GPSFlashDataLoad_s 						(u16	index,	SendData_TypeDef *sendData);

static		s8				GPSFlashQueInit 							(DataQue_TypeDef *que,	vu16 *quePosi);
static		s8				GPSFlashQueSave 							(DataQue_TypeDef *que,	vu16 *quePosi);
static		s8				GPSFlashQueLoad 							(DataQue_TypeDef *que,	vu16 *quePosi);
static		s8				GPSFlashDataSave 							(u16	index,	SendData_TypeDef *sendData);
static		s8				GPSFlashDataLoad 							(u16	index,	SendData_TypeDef *sendData);

// RAM���в�������
static		void			RamQue_init 									(DataQue_TypeDef	*que,	u16	size);
static		s8				RamQue_in 										(ENUM_QUE_TYPE queType,	DataQue_TypeDef	*que,	SendData_TypeDef	*sendData);
static		s8				RamQue_out										(ENUM_QUE_TYPE queType,	DataQue_TypeDef	*que,	SendData_TypeDef	*sendData);
static		s8				RamQue_outend									(DataQue_TypeDef	*que);

// FLASH���в�������
static		s8				FlashQue_init 								(DataQue_TypeDef	*que,	u16 size);
static		s8				FlashQue_in 									(DataQue_TypeDef	*que,	SendData_TypeDef	*sendData);
static		s8				FlashQue_out									(DataQue_TypeDef	*que,	SendData_TypeDef	*sendData);
static		s8				FlashQue_outend								(DataQue_TypeDef	*que);
static		s8				FlashQue_outFinish						(DataQue_TypeDef	*que,	u16 lastOut,	u16 outNum);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : GPSFlashQueInit_s()
* Description    : ��ʼ��GPS���߶��нṹ����Ӧ�ⲿ�洢������ռ� 
* Input          : *que  : ��Ҫ��ʼ���Ķ���ָ��
*								 : *quePosi : ���صĶ��нṹ�洢ҳλ��
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashQueInit_s (DataQue_TypeDef *que,	vu16 *quePosi)
{
		DataQue_TypeDef tmpQue;
		u8	tmpBuf[10]="";	// ֻ����ʼ����־��ȡ��д��
		u16	i=0,j=0;
	
		// ��ȡҳ0��ʼ����־
		memset((u8 *)tmpBuf,	0,	sizeof(tmpBuf));
		if(BSP_SPI_FLASH_BufferRead (FLASH_SPI_GPSQUE_INIT_START, tmpBuf, sizeof(tmpBuf),	3000) != DEF_SPI_NONE_ERR)
				return	DEF_PARA_READ_ERR;	
		
		// δ����ʼ��
		if(((tmpBuf[0] << 8) | tmpBuf[1]) != GPS_INITED_FLAG)
		{
				// ��ʼ���洢���������м�������(����������)
				for(i=0;	i<(FLASH_SPI_GPSQUE_INIT_SIZE + FLASH_SPI_GPSQUE_SIZE + FLASH_SPI_GPSDATA_SIZE)/SPI_FLASH_SectorSize;	i++)
				{
						for(j=0;	j<3;	j++)
						{
								if(BSP_SPI_FLASH_SectorErase((FLASH_SPI_GPSQUE_INIT_START+i*SPI_FLASH_SectorSize),	3000) == DEF_SPI_NONE_ERR)
										break;
						}
						if(j>=3)
								return	DEF_PARA_ERASE_ERR;
				}
				// ��ʼ�����в���
				que->useFlag 			= GPS_QUEUSED_FLAG;	// ָʾ���д洢���������Ϣ��Ч
				que->start        = 0; 	
				que->end          = GPS_FLASHDATA_USE;	
				que->in           = 0;	
				que->out          = 0;	
				que->size         = GPS_FLASHDATA_USE;	
				que->entries      = 0;
				que->outExt				= 0;	// 2015-9-29 ���нṹ�޸�
				que->entriesExt		= 0;
				// ����У��ͼ��洢���кͱ�־
				que->sum	=	SumCheck16Pro (((u8 *)que + 4),	(sizeof(DataQue_TypeDef) - 4));	// ȥ���洢��־��У��ͱ���
				
				// ��д�������Ϣ�ɹ�����д��ʼ����ɱ�־(������Ϣ�ӵڶ�������ʼ�洢)
				memset((u8 *)&tmpQue,	0,	sizeof(DataQue_TypeDef));
				memcpy((u8 *)&tmpQue,	(u8* )que,	sizeof(DataQue_TypeDef));
				if(BSP_SPI_FLASH_BufferWriteExt (FLASH_SPI_GPSQUE_START,	(u8 *)&tmpQue,  sizeof(DataQue_TypeDef),	3000) != DEF_SPI_NONE_ERR)
						return	DEF_PARA_WRITE_ERR;	// �ӵڶ�������ʼ����
				else
				{
						// д���ʼ����ɱ�־
						memset(tmpBuf,	0,	sizeof(tmpBuf));
						tmpBuf[0] = (u8)(GPS_INITED_FLAG >> 8);
						tmpBuf[1] = (u8)(GPS_INITED_FLAG);
						if(BSP_SPI_FLASH_BufferWriteExt (FLASH_SPI_GPSQUE_INIT_START,	tmpBuf,  2,	3000) != DEF_SPI_NONE_ERR)
								return	DEF_PARA_WRITE_ERR;
						else
						{
								*quePosi = 0;	// ���ض��нṹ�������ҳ��(0~GPS_QUE_LOOPS-1)
								return	DEF_PARA_NONE_ERR;
						}
				}	
		}		
		// �ѱ���ʼ��
		else
		{
				// ɨ����д洢λ��(�Ӷ��д洢����0~GPS_QUE_LOOPS-1��ɨ��)
				for(i=0;	i<=(GPS_QUE_LOOPS-1);	i++)
				{
						memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));				
						if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize*i), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
						{
								if(tmpQue.useFlag == GPS_QUEUSED_FLAG)	// �Ѿ�д���
										continue;
								else if(tmpQue.useFlag == 0xffff)				// ������һ��δд��Ŀռ�
								{
										if(i == 0)
										{
												//return	DEF_PARA_UNKNOW_ERR;		// δ֪����
												// �п����ϴδ洢�����һ���洢λ�ý���һ���洢λ�ò����ˣ������ϴ��쳣�ϵ絼�µĲ�����δд����ȷֵ
												// �ж����һ��λ���Ƿ���Ч��
												memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));	
												if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize*(GPS_QUE_LOOPS - 1)), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
												{
														// �ٴ��ж�����׼ȷ��
														if((tmpQue.useFlag == GPS_QUEUSED_FLAG) &&
															 (tmpQue.sum ==	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4))))
														{													
																memcpy((u8 *)que,	(u8 *)&tmpQue,	sizeof(DataQue_TypeDef));
																*quePosi = GPS_QUE_LOOPS-1;							// ����λ����Ϣ
																return	DEF_PARA_NONE_ERR;
														}
														else
														{
																// ���һ��λ����Ч��洢��Ϣ��ʧ�����г�ʼ��ΪĬ��ֵ(���ܶ���ԭ���洢������)
														}
												}
												else
														return	DEF_PARA_READ_ERR;
										}
										else
										{
												// i>=1
												j=i-1;	// ��ȡ��һ���洢λ����Ϣ(��ʵ�ʶ�����Чλ��)
												memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));	
												if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize*j), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
												{
														// �ٴ��ж�����׼ȷ��
														if((tmpQue.useFlag == GPS_QUEUSED_FLAG) &&
															 (tmpQue.sum ==	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4))))
														{													
																memcpy((u8 *)que,	(u8 *)&tmpQue,	sizeof(DataQue_TypeDef));
																*quePosi = j;							// ����λ����Ϣ
																return	DEF_PARA_NONE_ERR;
														}
														else
																return	DEF_PARA_SUM_ERR;	// У��ʧ��
												}		
												else
														return	DEF_PARA_READ_ERR;
										}
								}
								else
										return	DEF_PARA_UNKNOW_ERR;	// δ֪����(��ȡ��־�쳣)
						}
						else
								return	DEF_PARA_READ_ERR;
				}	
				if(i > (GPS_QUE_LOOPS-1))
				{
						// ���д洢λ�ö�ʹ�ù�(��ȡ���һ��ʹ��λ��)
						memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));	
						if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize*(GPS_QUE_LOOPS - 1)), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
						{
								// �ٴ��ж�����׼ȷ��
								if((tmpQue.useFlag == GPS_QUEUSED_FLAG) &&
									 (tmpQue.sum ==	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4))))
								{													
										memcpy((u8 *)que,	(u8 *)&tmpQue,	sizeof(DataQue_TypeDef));
										*quePosi = GPS_QUE_LOOPS-1;							// ����λ����Ϣ
										return	DEF_PARA_NONE_ERR;
								}
								else
										return	DEF_PARA_SUM_ERR;	// У��ʧ��
						}
						else
								return	DEF_PARA_READ_ERR;
				}
				else
				{
						// ����λ�ö�����ȷ�����³�ʼ������λ�� 
						// ��ʼ���洢���������м�������(����������)
						for(i=0;	i<(FLASH_SPI_GPSQUE_INIT_SIZE + FLASH_SPI_GPSQUE_SIZE + FLASH_SPI_GPSDATA_SIZE)/SPI_FLASH_SectorSize;	i++)
						{
								for(j=0;	j<3;	j++)
								{
										if(BSP_SPI_FLASH_SectorErase((FLASH_SPI_GPSQUE_INIT_START+i*SPI_FLASH_SectorSize),	3000) == DEF_SPI_NONE_ERR)
												break;
								}
								if(j>=3)
										return	DEF_PARA_ERASE_ERR;
						}
						// ��ʼ�����в���
						que->useFlag 			= GPS_QUEUSED_FLAG;	// ָʾ���д洢���������Ϣ��Ч
						que->start        = 0; 	
						que->end          = GPS_FLASHDATA_USE;	
						que->in           = 0;	
						que->out          = 0;	
						que->size         = GPS_FLASHDATA_USE;	
						que->entries      = 0;
						que->outExt				= 0;	// 2015-9-29 ���нṹ�޸�
						que->entriesExt		= 0;
						// ����У��ͼ��洢���кͱ�־
						que->sum	=	SumCheck16Pro (((u8 *)que + 4),	(sizeof(DataQue_TypeDef) - 4));	// ȥ���洢��־��У��ͱ���
						
						// ��д�������Ϣ�ɹ�����д��ʼ����ɱ�־(������Ϣ�ӵڶ�������ʼ�洢)
						memset((u8 *)&tmpQue,	0,	sizeof(DataQue_TypeDef));
						memcpy((u8 *)&tmpQue,	(u8* )que,	sizeof(DataQue_TypeDef));
						if(BSP_SPI_FLASH_BufferWriteExt (FLASH_SPI_GPSQUE_START,	(u8 *)&tmpQue,  sizeof(DataQue_TypeDef),	3000) != DEF_SPI_NONE_ERR)
								return	DEF_PARA_WRITE_ERR;	// �ӵڶ�������ʼ����
						else
						{
								// д���ʼ����ɱ�־
								memset(tmpBuf,	0,	sizeof(tmpBuf));
								tmpBuf[0] = (u8)(GPS_INITED_FLAG >> 8);
								tmpBuf[1] = (u8)(GPS_INITED_FLAG);
								if(BSP_SPI_FLASH_BufferWriteExt (FLASH_SPI_GPSQUE_INIT_START,	tmpBuf,  2,	3000) != DEF_SPI_NONE_ERR)
										return	DEF_PARA_WRITE_ERR;
								else
								{
										*quePosi = 0;	// ���ض��нṹ�������ҳ��(0~GPS_QUE_LOOPS-1)
										return	DEF_PARA_NONE_ERR;
								}
						}	
				}
		}
}

/*
******************************************************************************
* Function Name  : GPSFlashQueSave_s()
* Description    : ��ָ�����нṹ��ѭ���㷨�洢��flash�У��Ҽ���У���
* Input          : *que  		: ��Ҫ�洢�Ķ���ָ��
*								 : *quePosi : ��ǰ���д洢��λ��
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashQueSave_s (DataQue_TypeDef *que,	vu16 *quePosi)
{
		DataQue_TypeDef tmpQue;
		u16	i=0,j=0,tmpPosi=0;
		s8	ret=0;
		
		tmpPosi =*quePosi;	// �洢��ǰλ���Է�����ʧ�ָܻ�ʹ��
	
		if(*quePosi < GPS_QUE_LOOPS)
		{		
				memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));
				memcpy((u8 *)&tmpQue,	(u8* )que,	sizeof(DataQue_TypeDef));
			
				// ��ǰ�洢λ���Ѿ������һҳ(���²�������ѭ���ռ�)
				if(*quePosi == (GPS_QUE_LOOPS-1))	
				{
						// �������пռ�(����������)
						for(i=0;	i<GPS_QUESECTOR_NUM;	i++)
						{
								for(j=0;	j<3;	j++)
								{
										if(BSP_SPI_FLASH_SectorErase((FLASH_SPI_GPSQUE_START + SPI_FLASH_SectorSize*i),	3000) == DEF_SPI_NONE_ERR)
												break;
								}
								if(j>=3)
										return	DEF_PARA_ERASE_ERR;
						}
						*quePosi =0;	// �洢λ�ûص���һҳ
				}
				else
						*quePosi =*quePosi + 1; // ������һ���洢λ��
						
				// ����У��ͼ��洢���кͱ�־			
				tmpQue.useFlag 	= GPS_QUEUSED_FLAG;	// ָʾ���д洢���������Ϣ��Ч
				tmpQue.sum			=	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4));	// ȥ���洢��־��У��ͱ���
				
				// ����Դ������Ϣ 
				que->useFlag = tmpQue.useFlag;
				que->sum		 = tmpQue.sum;
				
				// ��д�������Ϣ�ɹ�����д��ʼ����ɱ�־(������Ϣ�ӵ�һ������ʼ�洢,��0����Ϊ��ʼ����־�洢)		
				ret =BSP_SPI_FLASH_BufferWriteExt ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize * *quePosi),	(u8 *)&tmpQue,  sizeof(DataQue_TypeDef),	3000);
				if(ret == DEF_SPI_NONE_ERR)
						return	DEF_PARA_NONE_ERR;
				else
				{
						//*quePosi =tmpPosi;	// ��ʹдʧ��Ҳ�����ô洢λ��
						if(ret == DEF_SPI_WRITECMP_ERR)
								return	DEF_PARA_WRITECMP_ERR;
						else
								return	DEF_PARA_WRITE_ERR;
				}
		}
		else
				return	DEF_PARA_TYPE_ERR;
}
/*
******************************************************************************
* Function Name  : GPSFlashQueLoad_s()
* Description    : ��ָ��λ�õĶ�����Ϣ���¶�ȡ�����нṹ�У����ж�У���
* Input          : *que  		: ��Ҫ��ȡ�Ķ���ָ��
*								 : *quePosi : ��ǰ���д洢��λ��
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashQueLoad_s (DataQue_TypeDef *que,	vu16 *quePosi)
{
		DataQue_TypeDef tmpQue;
		
		if(*quePosi < GPS_QUE_LOOPS)
		{
				// ���д洢λ�ö�ʹ�ù�(��ȡ���һ��ʹ��λ��)
				memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));	
				if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize * *quePosi), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
				{
						// �ٴ��ж�����׼ȷ��
						if((tmpQue.useFlag == GPS_QUEUSED_FLAG) &&
							 (tmpQue.sum ==	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4))))
						{													
								memcpy((u8 *)que,	(u8 *)&tmpQue,	sizeof(DataQue_TypeDef));
								return	DEF_PARA_NONE_ERR;
						}
						else
								return	DEF_PARA_SUM_ERR;	// У��ʧ��
				}
				else
						return	DEF_PARA_READ_ERR;
		}
		else
				return	DEF_PARA_TYPE_ERR;
}

/*
******************************************************************************
* Function Name  : GPSFlashDataSave_s()
* Description    : ���ṩ������д����Ӧλ���У�ע��Ҫ�����ݽṹ��һ���ڶ��ֽڱ��붨��ΪУ���
* Input          : index 	: ������ѭ���洢�����λ��(����ͨ��λ��������Ҫ�����ĵ�ַ)
*								 : pWBuf	:	д��λ�õ��������ṹָ��
*								 : wLen		: д�볤��
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashDataSave_s (u16	index,	SendData_TypeDef *sendData)
{
		SendData_TypeDef tmpSend;
		u32	eraseAddr=0;
		u16	j=0;
		s8	ret=0;
	
		if((index > (GPS_FLASHDATA_USE - 1)) || (sizeof(SendData_TypeDef) > SPI_FLASH_PageSize))
				return	DEF_PARA_TYPE_ERR;
		else
		{
				// У������
				memset((u8 *)&tmpSend,	0,	sizeof(tmpSend));
				memcpy((u8 *)&tmpSend,	(u8 *)sendData,		sizeof(tmpSend));
				tmpSend.sum = SumCheck16Pro (((u8 *)&tmpSend + 2),	(sizeof(tmpSend) - 2));	// ȥ��У���ֽڱ���
				
				// ����Դ������Ϣ 
				sendData->sum	=	tmpSend.sum;					
				
				// �ж�����洢λ�ü����ַΪ�������һ��ҳ��ַ,���Զ�������һ�������ռ�(Ӧ�Ȳ�����һ������Ȼ������д����������д��ɺ��豸�쳣�ϵ�)
				// �����´�д�¸�����ʱ����δ������
				if((index % (SPI_FLASH_SectorSize / SPI_FLASH_PageSize) == (SPI_FLASH_SectorSize / SPI_FLASH_PageSize - 1)) ||
					 (index == 0))
				{
						// ���β���Ϊ���һ����������Ϊ��һ��������һ���洢��Ԫ	
						if((index == (GPS_FLASHDATA_USE - 1)) || (index == 0))
								eraseAddr = FLASH_SPI_GPSDATA_START;	// ������һ������
						else
								eraseAddr = (FLASH_SPI_GPSDATA_START + (index + 1) * SPI_FLASH_PageSize);	// ��ַ������һ���ڽ�����
						
						// ��ʼ����
						for(j=0;	j<3;	j++)
						{
								if(BSP_SPI_FLASH_SectorErase(eraseAddr,	3000) == DEF_SPI_NONE_ERR)
										break;
						}
						if(j>=3)
								return	DEF_PARA_ERASE_ERR;				
				}	
				// д������
				ret =BSP_SPI_FLASH_BufferWriteExt ((FLASH_SPI_GPSDATA_START + SPI_FLASH_PageSize * index),	(u8 *)&tmpSend,  sizeof(tmpSend),	3000);
				if(ret == DEF_SPI_NONE_ERR)
						return	DEF_PARA_NONE_ERR;	
				else
				{
						if(ret == DEF_SPI_WRITECMP_ERR)
								return	DEF_PARA_WRITECMP_ERR;
						else
								return	DEF_PARA_WRITE_ERR;		
				}
		}
}
/*
******************************************************************************
* Function Name  : GPSFlashDataLoad_s()
* Description    : ����Ӧλ���е����ݶ�ȡ��������ȡ��У��ṹ�Ƿ���ȷ
* Input          : 
*								 : 
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashDataLoad_s (u16	index,	SendData_TypeDef *sendData)
{
		SendData_TypeDef tmpSend;
	
		memset((u8 *)&tmpSend,	0,	sizeof(SendData_TypeDef));
		if(index > (GPS_FLASHDATA_USE - 1))
				return	DEF_PARA_TYPE_ERR;
		else
		{
				if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSDATA_START + SPI_FLASH_PageSize * index), (u8 *)&tmpSend, sizeof(SendData_TypeDef),	3000) == DEF_SPI_NONE_ERR)
				{
						// �ٴ��ж�����׼ȷ��
						if(tmpSend.sum ==	SumCheck16Pro (((u8 *)&tmpSend + 2),	(sizeof(SendData_TypeDef) - 2)))
						{													
								memcpy((u8 *)sendData,	(u8 *)&tmpSend,	sizeof(SendData_TypeDef));
								return	DEF_PARA_NONE_ERR;
						}
						else
						{
								if((tmpSend.sum == 0xffff) && (tmpSend.len == 0xffff))
										return	DEF_PARA_UNWRITE_ERR;		// ����δ��д����Ѿ�������
								else
										return	DEF_PARA_SUM_ERR;				// У��ʧ��
						}
				}
				else
						return	DEF_PARA_READ_ERR;
		}
}
/*
******************************************************************************
* Function Name  : GPSFlashQueInit()
* Description    : ��ʼ��GPS���߶��нṹ����Ӧ�ⲿ�洢������ռ� 
* Input          : *que  : ��Ҫ��ʼ���Ķ���ָ��
*								 : *quePosi : ���صĶ��нṹ�洢ҳλ��
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashQueInit (DataQue_TypeDef *que,	vu16 *quePosi)
{
		u8 i=0;
		s8	ret=0;
	
		for(i=0;	i<DEF_TRYTIMES_FLASHINIT;	i++)
		{
				ret =GPSFlashQueInit_s(que,	quePosi);
				if(ret == DEF_PARA_NONE_ERR)
				{
						APP_ErrClr (&s_err.spiFlashSumTimes);	// �ɹ�һ��������ۼӸ�λ
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHINIT)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: SPI Flash queue init error!\r\n");
				#endif
				APP_ErrAdd(&s_err.flashTimes);	// SPI Flash�����ۼ�	
				APP_ErrAdd(&s_err.spiFlashSumTimes);	// �ܴ���++
		}
		return ret;
}
/*
******************************************************************************
* Function Name  : GPSFlashQueSave()
* Description    : ��ָ�����нṹ��ѭ���㷨�洢��flash�У��Ҽ���У���
* Input          : *que  		: ��Ҫ�洢�Ķ���ָ��
*								 : *quePosi : ��ǰ���д洢��λ��
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashQueSave (DataQue_TypeDef *que,	vu16 *quePosi)
{
		u8 i=0;
		s8	ret=0;
	
		for(i=0;	i<DEF_TRYTIMES_FLASHWRITE;	i++)
		{
				ret =GPSFlashQueSave_s(que,	quePosi);
				if(ret == DEF_PARA_NONE_ERR)
				{
						APP_ErrClr (&s_err.spiFlashSumTimes);	// �ɹ�һ��������ۼӸ�λ
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHWRITE)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: SPI Flash queue write error!\r\n");
				#endif
				APP_ErrAdd(&s_err.flashTimes);	// SPI Flash�����ۼ�		
				APP_ErrAdd(&s_err.spiFlashSumTimes);	// �ܴ���++
		}
		return ret;
}
/*
******************************************************************************
* Function Name  : GPSFlashQueLoad()
* Description    : ��ָ��λ�õĶ�����Ϣ���¶�ȡ�����нṹ�У����ж�У���
* Input          : *que  		: ��Ҫ��ȡ�Ķ���ָ��
*								 : *quePosi : ��ǰ���д洢��λ��
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashQueLoad (DataQue_TypeDef *que,	vu16 *quePosi)
{
		u8 i=0;
		s8	ret=0;
	
		for(i=0;	i<DEF_TRYTIMES_FLASHREAD;	i++)
		{
				ret =GPSFlashQueLoad_s(que,	quePosi);
				if(ret == DEF_PARA_NONE_ERR)
				{
						APP_ErrClr (&s_err.spiFlashSumTimes);	// �ɹ�һ��������ۼӸ�λ
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHREAD)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: SPI Flash queue read error!\r\n");
				#endif
				APP_ErrAdd(&s_err.flashTimes);	// SPI Flash�����ۼ�		
				APP_ErrAdd(&s_err.spiFlashSumTimes);	// �ܴ���++
		}
		return ret;
}
/*
******************************************************************************
* Function Name  : GPSFlashDataSave()
* Description    : ���ṩ������д����Ӧλ���У�ע��Ҫ�����ݽṹ��һ���ڶ��ֽڱ��붨��ΪУ���
* Input          : index 	: ������ѭ���洢�����λ��(����ͨ��λ��������Ҫ�����ĵ�ַ)
*								 : pWBuf	:	д��λ�õ��������ṹָ��
*								 : wLen		: д�볤��
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashDataSave (u16	index,	SendData_TypeDef *sendData)
{
		u8 i=0;
		s8	ret=0;
	
		for(i=0;	i<DEF_TRYTIMES_FLASHWRITE;	i++)
		{
				ret =GPSFlashDataSave_s(index,	sendData);
				if(ret == DEF_PARA_NONE_ERR)
				{
						APP_ErrClr (&s_err.spiFlashSumTimes);	// �ɹ�һ��������ۼӸ�λ
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHWRITE)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: SPI Flash data write error!\r\n");
				#endif
				APP_ErrAdd(&s_err.flashTimes);	// SPI Flash�����ۼ�		
				APP_ErrAdd(&s_err.spiFlashSumTimes);	// �ܴ���++
		}
		return ret;
}
/*
******************************************************************************
* Function Name  : GPSFlashDataLoad()
* Description    : ����Ӧλ���е����ݶ�ȡ��������ȡ��У��ṹ�Ƿ���ȷ
* Input          : 
*								 : 
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashDataLoad (u16	index,	SendData_TypeDef *sendData)
{
		u8 i=0;
		s8	ret=0;
	
		for(i=0;	i<DEF_TRYTIMES_FLASHREAD;	i++)
		{
				ret =GPSFlashDataLoad_s(index,	sendData);
				if(ret == DEF_PARA_NONE_ERR)
				{
						APP_ErrClr (&s_err.spiFlashSumTimes);	// �ɹ�һ��������ۼӸ�λ
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHREAD)
		{
				if(ret != DEF_PARA_UNWRITE_ERR)
				{
						// �������δ��д�룬���ѱ��������ۼƴ���(�������ݸ��Ǻ����16ҳ��һ�����������ݶ�ȡ����)
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: SPI Flash data read error!\r\n");
						#endif
						APP_ErrAdd(&s_err.flashTimes);	// SPI Flash�����ۼ�	
						APP_ErrAdd(&s_err.spiFlashSumTimes);	// �ܴ���++
				}
		}
		return ret;
}
/*
*********************************************************************************************************
*                                  RAM ���в�������
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : RamQue_init()
* Description    : ��ʼ��GSP RAM���нṹ
* Input          : *que	:	����ָ��
*								 : size	: ���д�С
* Output         : None
* Return         : None
******************************************************************************
*/
static	void	RamQue_init (DataQue_TypeDef	*que,	u16	size)
{
		// Ram���г�ʼ��
		que->start        = 0; 	
		que->end          = size;	
		que->in           = 0;	
		que->out          = 0;	
		que->size         = size;	
		que->entries      = 0;
		que->outExt				= 0;	// 2015-9-29 ���нṹ�޸�
		que->entriesExt		= 0;
}

/*
******************************************************************************
* Function Name  : RamQue_in()
* Description    : queType : ENUM_QUE_TYPE ��Ա����(ֻ����RAM���ݼ�RAM��Ϣ����)
* Input          : None
* Output         : None
* Return         : DEF_QUE_OK;DEF_QUE_FULL;DEF_QUE_EMPTY;DEF_QUE_ERROR;DEF_QUE_OVERFLOW
******************************************************************************
*/
static	s8	RamQue_in (ENUM_QUE_TYPE queType,	DataQue_TypeDef	*que,	SendData_TypeDef	*sendData)
{		
		if (que->entries >= que->size) 
		{ 
		    #if(DEF_RAMQUE_LOST_MODE > 0)
				if(queType == DEF_RAM_QUE)
						memcpy((u8 *)&s_ramData[que->in],	(u8 *)sendData,	sizeof(SendData_TypeDef));
				else if(queType == DEF_SMS_QUE)
						memcpy((u8 *)&s_ramSms[que->in],	(u8 *)sendData,	sizeof(SendData_TypeDef));
				
				que->in++;
				if (que->in == que->end) 
				{
		    		que->in = que->start;
		    }
				que->out++;
				if (que->out == que->end) 
				{
			  		que->out = que->start;
			  }
				return	DEF_PROQUE_QUE_OK;
				#else
				return	DEF_PROQUE_QUE_FULL;
				#endif	
	  } 
		else 
		{
				if(queType == DEF_RAM_QUE)
						memcpy((u8 *)&s_ramData[que->in],	(u8 *)sendData,	sizeof(SendData_TypeDef));
				else if(queType == DEF_SMS_QUE)
						memcpy((u8 *)&s_ramSms[que->in],	(u8 *)sendData,	sizeof(SendData_TypeDef));
				
				que->in++;
		    que->entries++;
		    if (que->in == que->end) 
				{
		       	que->in = que->start;
		    }			 	
	  }
		return	DEF_PROQUE_QUE_OK;
}
	
/*
******************************************************************************
* Function Name  : RamQue_out()
* Description    : queType : ENUM_QUE_TYPE ��Ա����(ֻ����RAM���ݼ�RAM��Ϣ����)
* Input          : None
* Output         : None
* Return         : DEF_QUE_IS_EMPTY, DEF_QUE_IS_SUCCESS, DEF_FRAM_ERROR
******************************************************************************
*/
static	s8	RamQue_out(ENUM_QUE_TYPE queType, DataQue_TypeDef	*que,	SendData_TypeDef	*sendData)
{	
		if (que->entries != 0) 
		{
				// ��ȡ���л�����	
				if(queType == DEF_RAM_QUE)
						memcpy((u8 *)sendData,	(u8 *)&s_ramData[que->out],	sizeof(SendData_TypeDef));
				else if(queType == DEF_SMS_QUE)
						memcpy((u8 *)sendData,	(u8 *)&s_ramSms[que->out],	sizeof(SendData_TypeDef));
				
				return	DEF_PROQUE_QUE_OK;
	  } 
		else 
			 	return  DEF_PROQUE_QUE_EMPTY;		
}

/*
******************************************************************************
* Function Name  : RamQue_outend()
* Description    : 
* Input          : None
* Output         : None
* Return         : DEF_QUE_IS_EMPTY, DEF_QUE_IS_SUCCESS, DEF_FRAM_ERROR
******************************************************************************
*/
static	s8	RamQue_outend(DataQue_TypeDef	*que)
{
		if (que->entries != 0) 
		{
				que->out++;
			  que->entries--;
			  if (que->out == que->end) 
				{
			      que->out = que->start;
			  }
		}		
		return	DEF_PROQUE_QUE_OK;	
}

/*
*********************************************************************************************************
*                                  FLASH ���в�������
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : FlashQue_init()
* Description    : 
* Input          : None
* Output         : None
* Return         : DEF_QUE_OK;DEF_QUE_FULL;DEF_QUE_EMPTY;DEF_QUE_ERROR;DEF_QUE_OVERFLOW
******************************************************************************
*/
static	s8	FlashQue_init (DataQue_TypeDef	*que,	u16 size)
{
		if(GPSFlashQueInit (&s_queFlash,	&flashQuePosi) == DEF_PARA_NONE_ERR)
				return	DEF_PROQUE_QUE_OK;
		else
				return	DEF_PROQUE_QUE_ERR;
}
/*
******************************************************************************
* Function Name  : FlashQue_in()
* Description    : 
* Input          : None
* Output         : None
* Return         : DEF_QUE_OK;DEF_QUE_FULL;DEF_QUE_EMPTY;DEF_QUE_ERROR;DEF_QUE_OVERFLOW
******************************************************************************
*/
static	s8	FlashQue_in (DataQue_TypeDef	*que,	SendData_TypeDef	*sendData)
{		
		s8	ret=0;
		u8	loop=0;
		if (que->entries >= que->size) 
		{ 
				// �洢���л�����  
				#if(DEF_FLASHQUE_LOST_MODE > 0)
				ret=GPSFlashDataSave (que->in,	sendData);		
				que->in++;
				if (que->in == que->end) 
				{
						que->in = que->start;
				}
				
				// 2015-9-29 ���нṹ�޸�
				if(que->out == que->outExt)
				{
						que->outExt++;				// ָ����Ʒ�ֹ��ʱ���ӹ���������������ݸ��ǵ��³����µ�����
						if (que->outExt == que->end) 
						{
								que->outExt = que->start;
						}
				}
				
				que->out++;
				if (que->out == que->end) 
				{
						que->out = que->start;
				}
				
				if(ret != DEF_PARA_NONE_ERR)	
				{
						if(ret == DEF_PARA_WRITECMP_ERR)
								return 	DEF_PROQUE_QUE_WCMPERR;
						else
								return	DEF_PROQUE_QUE_ERR; 
				}
				else
				{
						// �洢���л�����
						for(loop=0;	loop<3;	loop++)
						{
								ret=GPSFlashQueSave (que,	&flashQuePosi);
								if(ret == DEF_PARA_NONE_ERR)
										break;
								else
										OSTimeDly(800);
						}
						return DEF_PROQUE_QUE_OK;	
				}			

				#else
				return DEF_PROQUE_QUE_FULL;
				#endif
	  } 
		else 
		{
				// �洢���л�����	
				ret=GPSFlashDataSave (que->in,	sendData);	
			
				que->in++;
				que->entries++;
				if (que->in == que->end) 
				{
						que->in = que->start;
				}
				
				if(ret != DEF_PARA_NONE_ERR)	
				{
						if(ret == DEF_PARA_WRITECMP_ERR)
								return 	DEF_PROQUE_QUE_WCMPERR;
						else
								return	DEF_PROQUE_QUE_ERR; 
				}
				else
				{
						// �洢���л�����
						for(loop=0;	loop<3;	loop++)
						{
								ret=GPSFlashQueSave (que,	&flashQuePosi);
								if(ret == DEF_PARA_NONE_ERR)
										break;
								else
										OSTimeDly(800);
						}
						return DEF_PROQUE_QUE_OK;	
				}
	  }
}
/*
******************************************************************************
* Function Name  : FlashQue_out()
* Description    : 
* Input          : None
* Output         : None
* Return         : DEF_QUE_IS_EMPTY, DEF_QUE_IS_SUCCESS, DEF_FRAM_ERROR
******************************************************************************
*/
static	s8	FlashQue_out(DataQue_TypeDef	*que,	SendData_TypeDef	*sendData)
{
		s8 ret=0;	
		if (que->entriesExt != 0) 
		{
				// �洢���л�����	
				ret=GPSFlashDataLoad (que->outExt,	sendData);	// 2015-9-29 �����޸�
				if(ret == DEF_PARA_NONE_ERR)
						return	DEF_PROQUE_QUE_OK;
				else if(ret == DEF_PARA_UNWRITE_ERR)
						return	DEF_PROQUE_QUE_OK;	// 2015-9-29 �����Ѿ�������������Ϊ������ȷ��ֹ�ϲ����ȴ���������Ӧ�ò��жϣ���ȷ��
				else if(ret == DEF_PARA_SUM_ERR)
						return	DEF_PROQUE_QUE_ERR;
				else 
						return	DEF_PROQUE_QUE_ERR;
	  } 
		else 
			 	return  DEF_PROQUE_QUE_EMPTY;		
}

/*
******************************************************************************
* Function Name  : FlashQue_outend()
* Description    : 
* Input          : None
* Output         : None
* Return         : DEF_QUE_IS_EMPTY, DEF_QUE_IS_SUCCESS, DEF_FRAM_ERROR
******************************************************************************
*/
static	s8	FlashQue_outend(DataQue_TypeDef	*que)
{
		s8 ret=0;	
		u8 loop=0;
		if (que->entriesExt != 0) 
		{
				que->outExt++;
			  que->entriesExt--;
			  if (que->outExt == que->end) 
				{
			      que->outExt = que->start;
			  }
				/*
				// �洢���л�����
				ret =GPSFlashQueSave (que,	&flashQuePosi);				
				for(loop=0;	loop<3;	loop++)
				{
						ret=GPSFlashQueSave (que,	&flashQuePosi);
						if(ret == DEF_PARA_NONE_ERR)
								break;
						else
								OSTimeDly(800);
				}
				*/
				return DEF_PROQUE_QUE_OK;	
		}		
		else
				return	DEF_PROQUE_QUE_OK;
}
/*
******************************************************************************
* Function Name  : FlashQue_outFinish()
* Description    : 
* Input          : None
* Output         : None
* Return         : DEF_QUE_IS_EMPTY, DEF_QUE_IS_SUCCESS, DEF_FRAM_ERROR
******************************************************************************
*/
static	s8	FlashQue_outFinish(DataQue_TypeDef	*que,	u16 lastOut,	u16 outNum)
{
		s8 ret=0;	
		u8 loop=0;
	
		if(que->out == lastOut)
		{
				que->entries =que->entries - outNum;		// ����δ�����򽫱�����Ӻ��״̬
				que->out		 =que->outExt;	
		}
		else
		{
				que->entriesExt =que->entries;		// �����Ѿ������򽫱�����Ӻ��״̬
				que->outExt		 	=que->out;				
		}

		// �洢���л�����
		ret =GPSFlashQueSave (que,	&flashQuePosi);				
		for(loop=0;	loop<3;	loop++)
		{
				ret=GPSFlashQueSave (que,	&flashQuePosi);
				if(ret == DEF_PARA_NONE_ERR)
						break;
				else
						OSTimeDly(800);
		}
		return DEF_PROQUE_QUE_OK;			
}

/*
*********************************************************************************************************
*                                 ͨ�ö��в�������(�ⲿ����)
*********************************************************************************************************
*/
/*
******************************************************************************
* Function Name  : HAL_ComQue_SemPend()
* Description    : �����ź�������
* Input          : None
* Output         : None
* Return         : len
******************************************************************************
*/
s8	 HAL_ComQue_SemPend (u16 timeout)
{
		s8	ret=0;
		u8 err=OS_NO_ERR;
	
		if(OSRunning > 0)
				OSSemPend(QueMutexSem,	timeout,	&err);		//	�����ź���
		else
				if(QueMutexFlag	== 1)
						QueMutexFlag	=	0;
				else
						return -1;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}	
		
		return ret;
}
/*
******************************************************************************
* Function Name  : HAL_ComQue_SemPost()
* Description    : �����ź����ͷ�
* Input          : None
* Output         : None
* Return         : len
******************************************************************************
*/
s8	 HAL_ComQue_SemPost (void)
{
		if(OSRunning > 0)
				OSSemPost(QueMutexSem);						
		else
				QueMutexFlag	=	1;	
		return 0;
}
/*
******************************************************************************
* Function Name  : HAL_ComQue_ForceReset()
* Description    : ǿ�����ram��flash������Ϣ
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
s8	HAL_ComQue_ForceReset (ENUM_QUE_TYPE queType)
{
		u8 j=0;	
		s8	ret=0;
		u8 err=OS_NO_ERR;
	
		if(OSRunning > 0)
				OSSemPend(QueMutexSem,	5000,	&err);		//	�����ź���
		else
				if(QueMutexFlag	== 1)
						QueMutexFlag	=	0;
				else
						return -1;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}	
		else
		{	
				if(queType == DEF_RAM_QUE)
				{
						memset((u8 *)&s_queRam,		0,	sizeof(s_queRam));
						memset((u8 *)&s_ramData,	0,	sizeof(s_ramData));	// ���RAM������
						RamQue_init	(&s_queRam,	GPS_RAMDATA_SIZE);				// RAM���ݶ��д�С��ͷ�ļ�����	
						ret	=DEF_PROQUE_QUE_OK;	
				}
				else if(queType == DEF_SMS_QUE)
				{
						memset((u8 *)&s_queSms,		0,	sizeof(s_queSms));
						memset((u8 *)&s_ramSms,		0,	sizeof(s_ramSms));	// ���RAM������
						RamQue_init	(&s_queSms,	GPS_RAMSMS_SIZE);					// RAM���Ŷ��д�С��ͷ�ļ�����	
						ret	=DEF_PROQUE_QUE_OK;	
				}
				else if(queType == DEF_FLASH_QUE)
				{
						memset((u8 *)&s_queFlash,	0,	sizeof(s_queFlash));
						// Flash���г�ʼ��(���Ӳ�����ϻ�û���򲻲���FLASH)
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
						{
								// ���Flash��ʼ����־ǿ�Ƴ�ʼ��ִ��
								for(j=0;	j<3;	j++)
								{
										if(BSP_SPI_FLASH_SectorErase(FLASH_SPI_GPSQUE_INIT_START,	3000) == DEF_SPI_NONE_ERR)
												break;
								}
								if(j>=3)
										ret =DEF_PARA_ERASE_ERR;
								else
										ret	=FlashQue_init (&s_queFlash,	GPS_FLASHDATA_USE);	// FLASH���д�С��ͷ�ļ�����
						}
						else
								ret	=DEF_PROQUE_QUE_OK;	
				}	
				else if(queType == DEF_ALL_QUE)
				{
						memset((u8 *)&s_queRam,		0,	sizeof(s_queRam));
						memset((u8 *)&s_queSms,		0,	sizeof(s_queSms));
						memset((u8 *)&s_queFlash,	0,	sizeof(s_queFlash));
					
						memset((u8 *)&s_ramData,	0,	sizeof(s_ramData));	// ���RAM������
						memset((u8 *)&s_ramSms,		0,	sizeof(s_ramSms));	// ���RAM������
					
						// Ram���г�ʼ��
						RamQue_init	(&s_queRam,	GPS_RAMDATA_SIZE);	// RAM���ݶ��д�С��ͷ�ļ�����	
						RamQue_init	(&s_queSms,	GPS_RAMSMS_SIZE);		// RAM���Ŷ��д�С��ͷ�ļ�����	
						
						// Flash���г�ʼ��(���Ӳ�����ϻ�û���򲻲���FLASH)
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
						{
								// ���Flash��ʼ����־ǿ�Ƴ�ʼ��ִ��
								for(j=0;	j<3;	j++)
								{
										if(BSP_SPI_FLASH_SectorErase(FLASH_SPI_GPSQUE_INIT_START,	3000) == DEF_SPI_NONE_ERR)
												break;
								}
								if(j>=3)
										ret =DEF_PARA_ERASE_ERR;
								else
										ret	=FlashQue_init (&s_queFlash,	GPS_FLASHDATA_USE);	// FLASH���д�С��ͷ�ļ�����
						}
						else
								ret	=DEF_PROQUE_QUE_OK;
				}	
		}
		if(OSRunning > 0)
				OSSemPost(QueMutexSem);						
		else
				QueMutexFlag	=	1;	
		
		return ret;
}
/*
******************************************************************************
* Function Name  : HAL_ComQue_init()
* Description    : 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
s8	HAL_ComQue_init (ENUM_QUE_TYPE queType)
{
		s8	ret=0;
		u8 err=OS_NO_ERR;
	
		if(OSRunning > 0)
				OSSemPend(QueMutexSem,	5000,	&err);		//	�����ź���
		else
				if(QueMutexFlag	== 1)
						QueMutexFlag	=	0;
				else
						return -1;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}	
		else
		{	
				if(queType == DEF_RAM_QUE)
				{
						memset((u8 *)&s_queRam,		0,	sizeof(s_queRam));
						memset((u8 *)&s_ramData,	0,	sizeof(s_ramData));	// ���RAM������
						RamQue_init	(&s_queRam,	GPS_RAMDATA_SIZE);				// RAM���д�С��ͷ�ļ�����	
						ret	=DEF_PROQUE_QUE_OK;	
				}
				else if(queType == DEF_SMS_QUE)
				{
						memset((u8 *)&s_queSms,		0,	sizeof(s_queSms));
						memset((u8 *)&s_ramSms,		0,	sizeof(s_ramSms));	// ���RAM������
						RamQue_init	(&s_queSms,	GPS_RAMSMS_SIZE);		// RAM���Ŷ��д�С��ͷ�ļ�����	
						ret	=DEF_PROQUE_QUE_OK;	
				}
				else if(queType == DEF_FLASH_QUE)
				{
						memset((u8 *)&s_queFlash,	0,	sizeof(s_queRam));
						// Flash���г�ʼ��(���Ӳ�����ϻ�û���򲻲���FLASH)
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
								ret =FlashQue_init (&s_queFlash,	GPS_FLASHDATA_USE);	// FLASH���д�С��ͷ�ļ�����
						else
								ret =DEF_PROQUE_QUE_OK;	
				}	
				else if(queType == DEF_ALL_QUE)
				{
						memset((u8 *)&s_queRam,		0,	sizeof(s_queRam));
						memset((u8 *)&s_queSms,		0,	sizeof(s_queSms));
						memset((u8 *)&s_queFlash,	0,	sizeof(s_queRam));
					
						memset((u8 *)&s_ramData,	0,	sizeof(s_ramData));	// ���RAM������
						memset((u8 *)&s_ramSms,		0,	sizeof(s_ramSms));	// ���RAM������
					
						// Ram���г�ʼ��
						RamQue_init	(&s_queRam,	GPS_RAMDATA_SIZE);	// RAM���д�С��ͷ�ļ�����	
						RamQue_init	(&s_queSms,	GPS_RAMSMS_SIZE);		// RAM���Ŷ��д�С��ͷ�ļ�����	
						
						// Flash���г�ʼ��(���Ӳ�����ϻ�û���򲻲���FLASH)
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
								ret =FlashQue_init (&s_queFlash,	GPS_FLASHDATA_USE);	// FLASH���д�С��ͷ�ļ�����
						else
								ret =DEF_PROQUE_QUE_OK;	
				}	
		}
		
		if(OSRunning > 0)
				OSSemPost(QueMutexSem);						
		else
				QueMutexFlag	=	1;	
		
		return ret;
}
/*
******************************************************************************
* Function Name  : HAL_ComQue_size()
* Description    : 
* Input          : None
* Output         : None
* Return         : len
******************************************************************************
*/
u16	 HAL_ComQue_size (ENUM_QUE_TYPE queType)
{
		u16	retSize=0;
		u8 err=OS_NO_ERR;
	
		if(OSRunning > 0)
				OSSemPend(QueMutexSem,	5000,	&err);		//	�����ź���
		else
				if(QueMutexFlag	== 1)
						QueMutexFlag	=	0;
				else
						return 0;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	0;
				else
						return	0;		
		}	
		else
		{
				if(queType == DEF_RAM_QUE)
						retSize =s_queRam.entries;
				else if(queType == DEF_SMS_QUE)
						retSize	=s_queSms.entries;
				else if(queType == DEF_FLASH_QUE)
						retSize	=s_queFlash.entries;		// ���FLASH�й��ϣ�����Ӧ�÷���0
				else
						retSize	=0;
		}
		if(OSRunning > 0)
				OSSemPost(QueMutexSem);						
		else
				QueMutexFlag	=	1;	
		
		return retSize;
}
/*
******************************************************************************
* Function Name  : HAL_ComQue_in()
* Description    : �Զ���ʽʱ���RAM��������ֱ��д��FLASH����(DEF_AUTO_QUE)
* Input          : None
* Output         : None
* Return         : len
******************************************************************************
*/
s8	 HAL_ComQue_in (ENUM_QUE_TYPE queType,	SendData_TypeDef	*sendData)
{
		s8	ret=0;
		u8 err=OS_NO_ERR, loop=0;
	
		if(OSRunning > 0)
				OSSemPend(QueMutexSem,	5000,	&err);		//	�����ź���
		else
				if(QueMutexFlag	== 1)
						QueMutexFlag	=	0;
				else
						return -1;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}	
		else
		{							
				if(queType == DEF_RAM_QUE)
						ret	=RamQue_in (DEF_RAM_QUE,	&s_queRam,	sendData);
				else if(queType == DEF_SMS_QUE)
						ret	=RamQue_in (DEF_SMS_QUE,	&s_queSms,	sendData);	
				else if(queType == DEF_FLASH_QUE)
				{					
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
						{
								for(loop=0;	loop<3;	loop++)
								{
										ret	=FlashQue_in (&s_queFlash,	sendData);
										if(ret == DEF_PROQUE_QUE_OK)
												break;
										else
												OSTimeDly(1000);	// 1s���ٴγ���
								}
						}
						else
								ret	=DEF_PROQUE_QUE_ERR;
				}
				else
						ret	=DEF_PROQUE_QUE_ERR;
		}
		if(OSRunning > 0)
				OSSemPost(QueMutexSem);						
		else
				QueMutexFlag	=	1;	
		
		return ret;
}

/*
******************************************************************************
* Function Name  : HAL_ComQue_out()
* Description    : 
* Input          : None
* Output         : None
* Return         : len
******************************************************************************
*/
s8	 HAL_ComQue_out (ENUM_QUE_TYPE queType,	 SendData_TypeDef	*sendData)
{
		s8	ret=0;
		u8 err=OS_NO_ERR, loop=0;
	
		if(OSRunning > 0)
				OSSemPend(QueMutexSem,	5000,	&err);		//	�����ź���
		else
				if(QueMutexFlag	== 1)
						QueMutexFlag	=	0;
				else
						return -1;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}	
		else
		{
				if(queType == DEF_RAM_QUE)
						ret	=RamQue_out(DEF_RAM_QUE,	&s_queRam,	sendData);	
				else if(queType == DEF_SMS_QUE)
						ret	=RamQue_out (DEF_SMS_QUE,	&s_queSms,	sendData);
				else if(queType == DEF_FLASH_QUE)
				{
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
						{
								for(loop=0;	loop<3;	loop++)
								{
										ret	=FlashQue_out(&s_queFlash,	sendData);
										if(ret == DEF_PROQUE_QUE_OK)
												break;
										else
												OSTimeDly(1000);	// 1s���ٴγ���
								}
						}								
						else
								ret	=DEF_PROQUE_QUE_ERR;
				}
				else
						ret	=DEF_PROQUE_QUE_ERR;	
		}
		if(OSRunning > 0)
				OSSemPost(QueMutexSem);						
		else
				QueMutexFlag	=	1;	
		
		return ret;
}
/*
******************************************************************************
* Function Name  : HAL_ComQue_outend()
* Description    : ע��ú�����������Ӻ����ɶԳ���,�����������ݻ�������
* Input          : None
* Output         : None
* Return         : len
******************************************************************************
*/
s8	 HAL_ComQue_outend (ENUM_QUE_TYPE queType)
{
		s8	ret=0;
		u8 err=OS_NO_ERR, loop=0;
	
		if(OSRunning > 0)
				OSSemPend(QueMutexSem,	5000,	&err);		//	�����ź���
		else
				if(QueMutexFlag	== 1)
						QueMutexFlag	=	0;
				else
						return -1;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}	
		else
		{		
				if(queType == DEF_RAM_QUE)
						ret	=RamQue_outend(&s_queRam);
				else if(queType == DEF_SMS_QUE)
						ret	=RamQue_outend(&s_queSms);
				else if(queType == DEF_FLASH_QUE)
				{
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
						{						
								for(loop=0;	loop<3;	loop++)
								{
										ret	=FlashQue_outend(&s_queFlash);
										if(ret == DEF_PROQUE_QUE_OK)
												break;
										else
												OSTimeDly(1000);	// 1s���ٴγ���
								}						
						}
						else
								ret	=DEF_PROQUE_QUE_ERR;
				}
				else
						ret	=DEF_PROQUE_QUE_ERR;
		}
		if(OSRunning > 0)
				OSSemPost(QueMutexSem);						
		else
				QueMutexFlag	=	1;	
		
		return ret;
}
/*
******************************************************************************
* Function Name  : HAL_ComQue_outFinish()
* Description    : ע��ú�����������Ӻ����ɶԳ���,�����������ݻ�������
* Input          : None
* Output         : None
* Return         : len
******************************************************************************
*/
s8	 HAL_ComQue_outFinish (ENUM_QUE_TYPE queType,	u16 lastOut,	u16 outNum)
{
		s8	ret=0;
		u8 err=OS_NO_ERR, loop=0;
	
		if(OSRunning > 0)
				OSSemPend(QueMutexSem,	5000,	&err);		//	�����ź���
		else
				if(QueMutexFlag	== 1)
						QueMutexFlag	=	0;
				else
						return -1;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}	
		else
		{		
				if(queType != DEF_FLASH_QUE)
						ret	=-1;
				else
				{
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
						{							
								for(loop=0;	loop<3;	loop++)
								{
										ret	=FlashQue_outFinish(&s_queFlash,	lastOut,	outNum);
										if(ret == DEF_PROQUE_QUE_OK)
												break;
										else
												OSTimeDly(1000);	// 1s���ٴγ���
								}						
						}
						else
								ret	=DEF_PROQUE_QUE_ERR;
				}
		}
		if(OSRunning > 0)
				OSSemPost(QueMutexSem);						
		else
				QueMutexFlag	=	1;	
		
		return ret;
}

/*****************************************end*********************************************************/



