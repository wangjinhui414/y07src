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

extern		OS_EVENT					*QueMutexSem;				// 队列操作互斥信号量
static		vu8								QueMutexFlag=1;			//	无OS时实现队列操作互斥

static		vu16							flashQuePosi=0;	

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


// FLASH存储操作函数
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

// RAM队列操作函数
static		void			RamQue_init 									(DataQue_TypeDef	*que,	u16	size);
static		s8				RamQue_in 										(ENUM_QUE_TYPE queType,	DataQue_TypeDef	*que,	SendData_TypeDef	*sendData);
static		s8				RamQue_out										(ENUM_QUE_TYPE queType,	DataQue_TypeDef	*que,	SendData_TypeDef	*sendData);
static		s8				RamQue_outend									(DataQue_TypeDef	*que);

// FLASH队列操作函数
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
* Description    : 初始化GPS离线队列结构及相应外部存储器储存空间 
* Input          : *que  : 需要初始化的队列指针
*								 : *quePosi : 返回的队列结构存储页位置
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashQueInit_s (DataQue_TypeDef *que,	vu16 *quePosi)
{
		DataQue_TypeDef tmpQue;
		u8	tmpBuf[10]="";	// 只做初始化标志读取及写入
		u16	i=0,j=0;
	
		// 读取页0初始化标志
		memset((u8 *)tmpBuf,	0,	sizeof(tmpBuf));
		if(BSP_SPI_FLASH_BufferRead (FLASH_SPI_GPSQUE_INIT_START, tmpBuf, sizeof(tmpBuf),	3000) != DEF_SPI_NONE_ERR)
				return	DEF_PARA_READ_ERR;	
		
		// 未被初始化
		if(((tmpBuf[0] << 8) | tmpBuf[1]) != GPS_INITED_FLAG)
		{
				// 初始化存储区擦除队列及数据区(按扇区擦除)
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
				// 初始化队列参数
				que->useFlag 			= GPS_QUEUSED_FLAG;	// 指示队列存储区域队列信息有效
				que->start        = 0; 	
				que->end          = GPS_FLASHDATA_USE;	
				que->in           = 0;	
				que->out          = 0;	
				que->size         = GPS_FLASHDATA_USE;	
				que->entries      = 0;
				que->outExt				= 0;	// 2015-9-29 队列结构修改
				que->entriesExt		= 0;
				// 计算校验和及存储队列和标志
				que->sum	=	SumCheck16Pro (((u8 *)que + 4),	(sizeof(DataQue_TypeDef) - 4));	// 去掉存储标志与校验和本身
				
				// 先写入队列信息成功后再写初始化完成标志(队列信息从第二扇区开始存储)
				memset((u8 *)&tmpQue,	0,	sizeof(DataQue_TypeDef));
				memcpy((u8 *)&tmpQue,	(u8* )que,	sizeof(DataQue_TypeDef));
				if(BSP_SPI_FLASH_BufferWriteExt (FLASH_SPI_GPSQUE_START,	(u8 *)&tmpQue,  sizeof(DataQue_TypeDef),	3000) != DEF_SPI_NONE_ERR)
						return	DEF_PARA_WRITE_ERR;	// 从第二扇区开始操作
				else
				{
						// 写入初始化完成标志
						memset(tmpBuf,	0,	sizeof(tmpBuf));
						tmpBuf[0] = (u8)(GPS_INITED_FLAG >> 8);
						tmpBuf[1] = (u8)(GPS_INITED_FLAG);
						if(BSP_SPI_FLASH_BufferWriteExt (FLASH_SPI_GPSQUE_INIT_START,	tmpBuf,  2,	3000) != DEF_SPI_NONE_ERR)
								return	DEF_PARA_WRITE_ERR;
						else
						{
								*quePosi = 0;	// 返回队列结构所储存的页号(0~GPS_QUE_LOOPS-1)
								return	DEF_PARA_NONE_ERR;
						}
				}	
		}		
		// 已被初始化
		else
		{
				// 扫描队列存储位置(从队列存储区中0~GPS_QUE_LOOPS-1中扫描)
				for(i=0;	i<=(GPS_QUE_LOOPS-1);	i++)
				{
						memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));				
						if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize*i), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
						{
								if(tmpQue.useFlag == GPS_QUEUSED_FLAG)	// 已经写入过
										continue;
								else if(tmpQue.useFlag == 0xffff)				// 检索第一个未写入的空间
								{
										if(i == 0)
										{
												//return	DEF_PARA_UNKNOW_ERR;		// 未知错误
												// 有可能上次存储到最后一个存储位置将第一个存储位置擦除了，或者上次异常断电导致的擦除还未写入正确值
												// 判断最后一个位置是否有效、
												memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));	
												if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize*(GPS_QUE_LOOPS - 1)), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
												{
														// 再次判断数据准确性
														if((tmpQue.useFlag == GPS_QUEUSED_FLAG) &&
															 (tmpQue.sum ==	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4))))
														{													
																memcpy((u8 *)que,	(u8 *)&tmpQue,	sizeof(DataQue_TypeDef));
																*quePosi = GPS_QUE_LOOPS-1;							// 返回位置信息
																return	DEF_PARA_NONE_ERR;
														}
														else
														{
																// 最后一个位置无效则存储信息丢失将队列初始化为默认值(可能丢掉原来存储的数据)
														}
												}
												else
														return	DEF_PARA_READ_ERR;
										}
										else
										{
												// i>=1
												j=i-1;	// 提取上一个存储位置信息(即实际队列有效位置)
												memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));	
												if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize*j), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
												{
														// 再次判断数据准确性
														if((tmpQue.useFlag == GPS_QUEUSED_FLAG) &&
															 (tmpQue.sum ==	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4))))
														{													
																memcpy((u8 *)que,	(u8 *)&tmpQue,	sizeof(DataQue_TypeDef));
																*quePosi = j;							// 返回位置信息
																return	DEF_PARA_NONE_ERR;
														}
														else
																return	DEF_PARA_SUM_ERR;	// 校验失败
												}		
												else
														return	DEF_PARA_READ_ERR;
										}
								}
								else
										return	DEF_PARA_UNKNOW_ERR;	// 未知错误(读取标志异常)
						}
						else
								return	DEF_PARA_READ_ERR;
				}	
				if(i > (GPS_QUE_LOOPS-1))
				{
						// 所有存储位置都使用过(提取最后一个使用位置)
						memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));	
						if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize*(GPS_QUE_LOOPS - 1)), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
						{
								// 再次判断数据准确性
								if((tmpQue.useFlag == GPS_QUEUSED_FLAG) &&
									 (tmpQue.sum ==	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4))))
								{													
										memcpy((u8 *)que,	(u8 *)&tmpQue,	sizeof(DataQue_TypeDef));
										*quePosi = GPS_QUE_LOOPS-1;							// 返回位置信息
										return	DEF_PARA_NONE_ERR;
								}
								else
										return	DEF_PARA_SUM_ERR;	// 校验失败
						}
						else
								return	DEF_PARA_READ_ERR;
				}
				else
				{
						// 所有位置都不正确将重新初始化队列位置 
						// 初始化存储区擦除队列及数据区(按扇区擦除)
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
						// 初始化队列参数
						que->useFlag 			= GPS_QUEUSED_FLAG;	// 指示队列存储区域队列信息有效
						que->start        = 0; 	
						que->end          = GPS_FLASHDATA_USE;	
						que->in           = 0;	
						que->out          = 0;	
						que->size         = GPS_FLASHDATA_USE;	
						que->entries      = 0;
						que->outExt				= 0;	// 2015-9-29 队列结构修改
						que->entriesExt		= 0;
						// 计算校验和及存储队列和标志
						que->sum	=	SumCheck16Pro (((u8 *)que + 4),	(sizeof(DataQue_TypeDef) - 4));	// 去掉存储标志与校验和本身
						
						// 先写入队列信息成功后再写初始化完成标志(队列信息从第二扇区开始存储)
						memset((u8 *)&tmpQue,	0,	sizeof(DataQue_TypeDef));
						memcpy((u8 *)&tmpQue,	(u8* )que,	sizeof(DataQue_TypeDef));
						if(BSP_SPI_FLASH_BufferWriteExt (FLASH_SPI_GPSQUE_START,	(u8 *)&tmpQue,  sizeof(DataQue_TypeDef),	3000) != DEF_SPI_NONE_ERR)
								return	DEF_PARA_WRITE_ERR;	// 从第二扇区开始操作
						else
						{
								// 写入初始化完成标志
								memset(tmpBuf,	0,	sizeof(tmpBuf));
								tmpBuf[0] = (u8)(GPS_INITED_FLAG >> 8);
								tmpBuf[1] = (u8)(GPS_INITED_FLAG);
								if(BSP_SPI_FLASH_BufferWriteExt (FLASH_SPI_GPSQUE_INIT_START,	tmpBuf,  2,	3000) != DEF_SPI_NONE_ERR)
										return	DEF_PARA_WRITE_ERR;
								else
								{
										*quePosi = 0;	// 返回队列结构所储存的页号(0~GPS_QUE_LOOPS-1)
										return	DEF_PARA_NONE_ERR;
								}
						}	
				}
		}
}

/*
******************************************************************************
* Function Name  : GPSFlashQueSave_s()
* Description    : 将指定队列结构按循环算法存储到flash中，且计算校验和
* Input          : *que  		: 需要存储的队列指针
*								 : *quePosi : 当前队列存储的位置
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashQueSave_s (DataQue_TypeDef *que,	vu16 *quePosi)
{
		DataQue_TypeDef tmpQue;
		u16	i=0,j=0,tmpPosi=0;
		s8	ret=0;
		
		tmpPosi =*quePosi;	// 存储当前位置以防操作失败恢复使用
	
		if(*quePosi < GPS_QUE_LOOPS)
		{		
				memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));
				memcpy((u8 *)&tmpQue,	(u8* )que,	sizeof(DataQue_TypeDef));
			
				// 当前存储位置已经是最后一页(重新擦除整个循环空间)
				if(*quePosi == (GPS_QUE_LOOPS-1))	
				{
						// 擦除队列空间(按扇区擦除)
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
						*quePosi =0;	// 存储位置回到第一页
				}
				else
						*quePosi =*quePosi + 1; // 跳到下一个存储位置
						
				// 计算校验和及存储队列和标志			
				tmpQue.useFlag 	= GPS_QUEUSED_FLAG;	// 指示队列存储区域队列信息有效
				tmpQue.sum			=	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4));	// 去掉存储标志与校验和本身
				
				// 更新源队列信息 
				que->useFlag = tmpQue.useFlag;
				que->sum		 = tmpQue.sum;
				
				// 先写入队列信息成功后再写初始化完成标志(队列信息从第一扇区开始存储,第0扇区为初始化标志存储)		
				ret =BSP_SPI_FLASH_BufferWriteExt ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize * *quePosi),	(u8 *)&tmpQue,  sizeof(DataQue_TypeDef),	3000);
				if(ret == DEF_SPI_NONE_ERR)
						return	DEF_PARA_NONE_ERR;
				else
				{
						//*quePosi =tmpPosi;	// 即使写失败也跳过该存储位置
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
* Description    : 将指定位置的队列信息从新读取到队列结构中，且判断校验和
* Input          : *que  		: 需要读取的队列指针
*								 : *quePosi : 当前队列存储的位置
* Output         : None
* Return         : None
******************************************************************************
*/
static	s8	GPSFlashQueLoad_s (DataQue_TypeDef *que,	vu16 *quePosi)
{
		DataQue_TypeDef tmpQue;
		
		if(*quePosi < GPS_QUE_LOOPS)
		{
				// 所有存储位置都使用过(提取最后一个使用位置)
				memset((u8 *)&tmpQue,	0,	sizeof(tmpQue));	
				if(BSP_SPI_FLASH_BufferRead ((FLASH_SPI_GPSQUE_START + SPI_FLASH_PageSize * *quePosi), (u8 *)&tmpQue, sizeof(DataQue_TypeDef),	3000) == DEF_SPI_NONE_ERR)
				{
						// 再次判断数据准确性
						if((tmpQue.useFlag == GPS_QUEUSED_FLAG) &&
							 (tmpQue.sum ==	SumCheck16Pro (((u8 *)&tmpQue + 4),	(sizeof(DataQue_TypeDef) - 4))))
						{													
								memcpy((u8 *)que,	(u8 *)&tmpQue,	sizeof(DataQue_TypeDef));
								return	DEF_PARA_NONE_ERR;
						}
						else
								return	DEF_PARA_SUM_ERR;	// 校验失败
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
* Description    : 将提供的数据写入相应位置中，注意要求数据结构第一根第二字节必须定义为校验和
* Input          : index 	: 数据在循环存储区域的位置(函数通过位置推算需要操作的地址)
*								 : pWBuf	:	写入位置的数据区结构指针
*								 : wLen		: 写入长度
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
				// 校验数据
				memset((u8 *)&tmpSend,	0,	sizeof(tmpSend));
				memcpy((u8 *)&tmpSend,	(u8 *)sendData,		sizeof(tmpSend));
				tmpSend.sum = SumCheck16Pro (((u8 *)&tmpSend + 2),	(sizeof(tmpSend) - 2));	// 去掉校验字节本身
				
				// 更新源队列信息 
				sendData->sum	=	tmpSend.sum;					
				
				// 判断如果存储位置计算地址为扇区最后一个页地址,则自动擦除下一个扇区空间(应先擦除下一个扇区然后在做写操作，考虑写完成后设备异常断电)
				// 导致下次写下个扇区时扇区未被擦除
				if((index % (SPI_FLASH_SectorSize / SPI_FLASH_PageSize) == (SPI_FLASH_SectorSize / SPI_FLASH_PageSize - 1)) ||
					 (index == 0))
				{
						// 本次操作为最后一个扇区或者为第一个扇区第一个存储单元	
						if((index == (GPS_FLASHDATA_USE - 1)) || (index == 0))
								eraseAddr = FLASH_SPI_GPSDATA_START;	// 擦除第一个扇区
						else
								eraseAddr = (FLASH_SPI_GPSDATA_START + (index + 1) * SPI_FLASH_PageSize);	// 地址跳到下一个邻近扇区
						
						// 开始擦除
						for(j=0;	j<3;	j++)
						{
								if(BSP_SPI_FLASH_SectorErase(eraseAddr,	3000) == DEF_SPI_NONE_ERR)
										break;
						}
						if(j>=3)
								return	DEF_PARA_ERASE_ERR;				
				}	
				// 写入数据
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
* Description    : 将相应位置中的数据读取出来，读取后校验结构是否正确
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
						// 再次判断数据准确性
						if(tmpSend.sum ==	SumCheck16Pro (((u8 *)&tmpSend + 2),	(sizeof(SendData_TypeDef) - 2)))
						{													
								memcpy((u8 *)sendData,	(u8 *)&tmpSend,	sizeof(SendData_TypeDef));
								return	DEF_PARA_NONE_ERR;
						}
						else
						{
								if((tmpSend.sum == 0xffff) && (tmpSend.len == 0xffff))
										return	DEF_PARA_UNWRITE_ERR;		// 区域未被写入或已经被擦除
								else
										return	DEF_PARA_SUM_ERR;				// 校验失败
						}
				}
				else
						return	DEF_PARA_READ_ERR;
		}
}
/*
******************************************************************************
* Function Name  : GPSFlashQueInit()
* Description    : 初始化GPS离线队列结构及相应外部存储器储存空间 
* Input          : *que  : 需要初始化的队列指针
*								 : *quePosi : 返回的队列结构存储页位置
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
						APP_ErrClr (&s_err.spiFlashSumTimes);	// 成功一次则错误累加复位
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHINIT)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: SPI Flash queue init error!\r\n");
				#endif
				APP_ErrAdd(&s_err.flashTimes);	// SPI Flash故障累加	
				APP_ErrAdd(&s_err.spiFlashSumTimes);	// 总次数++
		}
		return ret;
}
/*
******************************************************************************
* Function Name  : GPSFlashQueSave()
* Description    : 将指定队列结构按循环算法存储到flash中，且计算校验和
* Input          : *que  		: 需要存储的队列指针
*								 : *quePosi : 当前队列存储的位置
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
						APP_ErrClr (&s_err.spiFlashSumTimes);	// 成功一次则错误累加复位
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHWRITE)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: SPI Flash queue write error!\r\n");
				#endif
				APP_ErrAdd(&s_err.flashTimes);	// SPI Flash故障累加		
				APP_ErrAdd(&s_err.spiFlashSumTimes);	// 总次数++
		}
		return ret;
}
/*
******************************************************************************
* Function Name  : GPSFlashQueLoad()
* Description    : 将指定位置的队列信息从新读取到队列结构中，且判断校验和
* Input          : *que  		: 需要读取的队列指针
*								 : *quePosi : 当前队列存储的位置
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
						APP_ErrClr (&s_err.spiFlashSumTimes);	// 成功一次则错误累加复位
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHREAD)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: SPI Flash queue read error!\r\n");
				#endif
				APP_ErrAdd(&s_err.flashTimes);	// SPI Flash故障累加		
				APP_ErrAdd(&s_err.spiFlashSumTimes);	// 总次数++
		}
		return ret;
}
/*
******************************************************************************
* Function Name  : GPSFlashDataSave()
* Description    : 将提供的数据写入相应位置中，注意要求数据结构第一根第二字节必须定义为校验和
* Input          : index 	: 数据在循环存储区域的位置(函数通过位置推算需要操作的地址)
*								 : pWBuf	:	写入位置的数据区结构指针
*								 : wLen		: 写入长度
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
						APP_ErrClr (&s_err.spiFlashSumTimes);	// 成功一次则错误累加复位
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHWRITE)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: SPI Flash data write error!\r\n");
				#endif
				APP_ErrAdd(&s_err.flashTimes);	// SPI Flash故障累加		
				APP_ErrAdd(&s_err.spiFlashSumTimes);	// 总次数++
		}
		return ret;
}
/*
******************************************************************************
* Function Name  : GPSFlashDataLoad()
* Description    : 将相应位置中的数据读取出来，读取后校验结构是否正确
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
						APP_ErrClr (&s_err.spiFlashSumTimes);	// 成功一次则错误累加复位
						break;
				}
		}
		if(i >= DEF_TRYTIMES_FLASHREAD)
		{
				if(ret != DEF_PARA_UNWRITE_ERR)
				{
						// 如果数据未被写入，或已被擦除则不累计错误(比如数据覆盖后会有16页即一个扇区的数据读取错误)
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: SPI Flash data read error!\r\n");
						#endif
						APP_ErrAdd(&s_err.flashTimes);	// SPI Flash故障累加	
						APP_ErrAdd(&s_err.spiFlashSumTimes);	// 总次数++
				}
		}
		return ret;
}
/*
*********************************************************************************************************
*                                  RAM 队列操作函数
*********************************************************************************************************
*/

/*
******************************************************************************
* Function Name  : RamQue_init()
* Description    : 初始化GSP RAM队列结构
* Input          : *que	:	队列指针
*								 : size	: 队列大小
* Output         : None
* Return         : None
******************************************************************************
*/
static	void	RamQue_init (DataQue_TypeDef	*que,	u16	size)
{
		// Ram队列初始化
		que->start        = 0; 	
		que->end          = size;	
		que->in           = 0;	
		que->out          = 0;	
		que->size         = size;	
		que->entries      = 0;
		que->outExt				= 0;	// 2015-9-29 队列结构修改
		que->entriesExt		= 0;
}

/*
******************************************************************************
* Function Name  : RamQue_in()
* Description    : queType : ENUM_QUE_TYPE 成员变量(只能是RAM数据及RAM短息队列)
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
* Description    : queType : ENUM_QUE_TYPE 成员变量(只能是RAM数据及RAM短息队列)
* Input          : None
* Output         : None
* Return         : DEF_QUE_IS_EMPTY, DEF_QUE_IS_SUCCESS, DEF_FRAM_ERROR
******************************************************************************
*/
static	s8	RamQue_out(ENUM_QUE_TYPE queType, DataQue_TypeDef	*que,	SendData_TypeDef	*sendData)
{	
		if (que->entries != 0) 
		{
				// 读取队列或数据	
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
*                                  FLASH 队列操作函数
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
				// 存储队列或数据  
				#if(DEF_FLASHQUE_LOST_MODE > 0)
				ret=GPSFlashDataSave (que->in,	sendData);		
				que->in++;
				if (que->in == que->end) 
				{
						que->in = que->start;
				}
				
				// 2015-9-29 队列结构修改
				if(que->out == que->outExt)
				{
						que->outExt++;				// 指针后移防止临时出队过程中由于入队数据覆盖导致出了新的数据
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
						// 存储队列或数据
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
				// 存储队列或数据	
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
						// 存储队列或数据
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
				// 存储队列或数据	
				ret=GPSFlashDataLoad (que->outExt,	sendData);	// 2015-9-29 队列修改
				if(ret == DEF_PARA_NONE_ERR)
						return	DEF_PROQUE_QUE_OK;
				else if(ret == DEF_PARA_UNWRITE_ERR)
						return	DEF_PROQUE_QUE_OK;	// 2015-9-29 对于已经擦除的区域认为返回正确防止上层过多等待，由最外应用层判断，正确性
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
				// 存储队列或数据
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
				que->entries =que->entries - outNum;		// 数据未覆盖则将保存出队后的状态
				que->out		 =que->outExt;	
		}
		else
		{
				que->entriesExt =que->entries;		// 数据已经覆盖则将保存出队后的状态
				que->outExt		 	=que->out;				
		}

		// 存储队列或数据
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
*                                 通用队列操作函数(外部调用)
*********************************************************************************************************
*/
/*
******************************************************************************
* Function Name  : HAL_ComQue_SemPend()
* Description    : 队列信号量请求
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
				OSSemPend(QueMutexSem,	timeout,	&err);		//	请求信号量
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
* Description    : 队列信号量释放
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
* Description    : 强制清空ram及flash队列信息
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
				OSSemPend(QueMutexSem,	5000,	&err);		//	请求信号量
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
						memset((u8 *)&s_ramData,	0,	sizeof(s_ramData));	// 清空RAM缓冲区
						RamQue_init	(&s_queRam,	GPS_RAMDATA_SIZE);				// RAM数据队列大小见头文件定义	
						ret	=DEF_PROQUE_QUE_OK;	
				}
				else if(queType == DEF_SMS_QUE)
				{
						memset((u8 *)&s_queSms,		0,	sizeof(s_queSms));
						memset((u8 *)&s_ramSms,		0,	sizeof(s_ramSms));	// 清空RAM缓冲区
						RamQue_init	(&s_queSms,	GPS_RAMSMS_SIZE);					// RAM短信队列大小见头文件定义	
						ret	=DEF_PROQUE_QUE_OK;	
				}
				else if(queType == DEF_FLASH_QUE)
				{
						memset((u8 *)&s_queFlash,	0,	sizeof(s_queFlash));
						// Flash队列初始化(如果硬件故障或没有则不操作FLASH)
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
						{
								// 清空Flash初始化标志强制初始化执行
								for(j=0;	j<3;	j++)
								{
										if(BSP_SPI_FLASH_SectorErase(FLASH_SPI_GPSQUE_INIT_START,	3000) == DEF_SPI_NONE_ERR)
												break;
								}
								if(j>=3)
										ret =DEF_PARA_ERASE_ERR;
								else
										ret	=FlashQue_init (&s_queFlash,	GPS_FLASHDATA_USE);	// FLASH队列大小见头文件定义
						}
						else
								ret	=DEF_PROQUE_QUE_OK;	
				}	
				else if(queType == DEF_ALL_QUE)
				{
						memset((u8 *)&s_queRam,		0,	sizeof(s_queRam));
						memset((u8 *)&s_queSms,		0,	sizeof(s_queSms));
						memset((u8 *)&s_queFlash,	0,	sizeof(s_queFlash));
					
						memset((u8 *)&s_ramData,	0,	sizeof(s_ramData));	// 清空RAM缓冲区
						memset((u8 *)&s_ramSms,		0,	sizeof(s_ramSms));	// 清空RAM缓冲区
					
						// Ram队列初始化
						RamQue_init	(&s_queRam,	GPS_RAMDATA_SIZE);	// RAM数据队列大小见头文件定义	
						RamQue_init	(&s_queSms,	GPS_RAMSMS_SIZE);		// RAM短信队列大小见头文件定义	
						
						// Flash队列初始化(如果硬件故障或没有则不操作FLASH)
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
						{
								// 清空Flash初始化标志强制初始化执行
								for(j=0;	j<3;	j++)
								{
										if(BSP_SPI_FLASH_SectorErase(FLASH_SPI_GPSQUE_INIT_START,	3000) == DEF_SPI_NONE_ERR)
												break;
								}
								if(j>=3)
										ret =DEF_PARA_ERASE_ERR;
								else
										ret	=FlashQue_init (&s_queFlash,	GPS_FLASHDATA_USE);	// FLASH队列大小见头文件定义
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
				OSSemPend(QueMutexSem,	5000,	&err);		//	请求信号量
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
						memset((u8 *)&s_ramData,	0,	sizeof(s_ramData));	// 清空RAM缓冲区
						RamQue_init	(&s_queRam,	GPS_RAMDATA_SIZE);				// RAM队列大小见头文件定义	
						ret	=DEF_PROQUE_QUE_OK;	
				}
				else if(queType == DEF_SMS_QUE)
				{
						memset((u8 *)&s_queSms,		0,	sizeof(s_queSms));
						memset((u8 *)&s_ramSms,		0,	sizeof(s_ramSms));	// 清空RAM缓冲区
						RamQue_init	(&s_queSms,	GPS_RAMSMS_SIZE);		// RAM短信队列大小见头文件定义	
						ret	=DEF_PROQUE_QUE_OK;	
				}
				else if(queType == DEF_FLASH_QUE)
				{
						memset((u8 *)&s_queFlash,	0,	sizeof(s_queRam));
						// Flash队列初始化(如果硬件故障或没有则不操作FLASH)
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
								ret =FlashQue_init (&s_queFlash,	GPS_FLASHDATA_USE);	// FLASH队列大小见头文件定义
						else
								ret =DEF_PROQUE_QUE_OK;	
				}	
				else if(queType == DEF_ALL_QUE)
				{
						memset((u8 *)&s_queRam,		0,	sizeof(s_queRam));
						memset((u8 *)&s_queSms,		0,	sizeof(s_queSms));
						memset((u8 *)&s_queFlash,	0,	sizeof(s_queRam));
					
						memset((u8 *)&s_ramData,	0,	sizeof(s_ramData));	// 清空RAM缓冲区
						memset((u8 *)&s_ramSms,		0,	sizeof(s_ramSms));	// 清空RAM缓冲区
					
						// Ram队列初始化
						RamQue_init	(&s_queRam,	GPS_RAMDATA_SIZE);	// RAM队列大小见头文件定义	
						RamQue_init	(&s_queSms,	GPS_RAMSMS_SIZE);		// RAM短信队列大小见头文件定义	
						
						// Flash队列初始化(如果硬件故障或没有则不操作FLASH)
						if(*(s_devSta.pFlash)	==	DEF_FLASHHARD_NONE_ERR)
								ret =FlashQue_init (&s_queFlash,	GPS_FLASHDATA_USE);	// FLASH队列大小见头文件定义
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
				OSSemPend(QueMutexSem,	5000,	&err);		//	请求信号量
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
						retSize	=s_queFlash.entries;		// 如果FLASH有故障，这里应该返回0
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
* Description    : 自动方式时如果RAM队列满了直接写到FLASH队列(DEF_AUTO_QUE)
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
				OSSemPend(QueMutexSem,	5000,	&err);		//	请求信号量
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
												OSTimeDly(1000);	// 1s后再次尝试
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
				OSSemPend(QueMutexSem,	5000,	&err);		//	请求信号量
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
												OSTimeDly(1000);	// 1s后再次尝试
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
* Description    : 注意该函数必须与出队函数成对出现,否则会出现数据混乱问题
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
				OSSemPend(QueMutexSem,	5000,	&err);		//	请求信号量
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
												OSTimeDly(1000);	// 1s后再次尝试
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
* Description    : 注意该函数必须与出队函数成对出现,否则会出现数据混乱问题
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
				OSSemPend(QueMutexSem,	5000,	&err);		//	请求信号量
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
												OSTimeDly(1000);	// 1s后再次尝试
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



