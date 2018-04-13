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
*                                      BOARD SUPPORT PACKAGE
*
*                                     		bsp_storage_c.c
*                                            with the
*                                   			  Y05D Board
*
* Filename      : bsp_storage_c.c
* Version       : V1.01
* Programmer(s) : Felix
*********************************************************************************************************
*/

// 驱动说明：
// 驱动程序支持W25Q80芯片，同时支持CPU内部FLASH解锁及存储操作 

// 修改历史：
// 1.00:	代码建立;

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_STORAGE_GLOBLAS
#include	<string.h>
#include	"cfg_h.h"
#include	"bsp_h.h"
#include	"bsp_storage_h.h"
#include 	"ucos_ii.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

/********** Flash 内部IO接口 *************/

/* Select SPI FLASH: CS pin low  */
#define SPI_FLASH_CS_EN()     			(GPIO_ResetBits	(DEF_FLASH_CS_PORTS, DEF_FLASH_CS_PINS))
#define SPI_FLASH_CS_DIS()    			(GPIO_SetBits		(DEF_FLASH_CS_PORTS, DEF_FLASH_CS_PINS))

/* Select SPI FLASH Write protect: WP pin low  */
#define	SPI_FLASH_WP_EN()						(GPIO_ResetBits	(DEF_FLASH_WP_PORTS, DEF_FLASH_WP_PINS))
#define	SPI_FLASH_WP_DIS()					(GPIO_SetBits		(DEF_FLASH_WP_PORTS, DEF_FLASH_WP_PINS))

/********SPI FLASH***********/

#define FLASH_WRITE									((u8)0x02)   /* Write to Memory instruction */
#define FLASH_WRSR									((u8)0x01)   /* Write Status Register instruction */ 
#define FLASH_WREN 									((u8)0x06)   /* Write enable instruction */
#define FLASH_READ       						((u8)0x03)   /* Read from Memory instruction */
#define	FLASH_FREAD									((u8)0x0B)	 /* Fast read from Memory instruction */
#define FLASH_RDSR       						((u8)0x05)   /* Read Status Register instruction(s7~s0)  */
#define	FLASH_RDSR2									((u8)0x35)   /* Read Status Register instruction(s15~s8)  */
#define FLASH_RDID       						((u8)0x9F)   /* Read identification (JEDEC ID)*/
#define FLASH_SE         						((u8)0x20)   /* Sector Erase instruction(4KB) */
#define FLASH_BE         						((u8)0xD8)   /* Block Erase instruction(64KB) */
#define FLASH_BE1         					((u8)0x52)   /* Block Erase instruction(32KB)*/
#define	FLASH_CHIPER								((u8)0xC7)   /* Chip Erase instruction */
#define FLASH_WIP_Flag   						((u8)0x01)   /* Write In Progress (WIP) flag */
#define FLASH_Dummy_Byte 						((u8)0xA5)

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/

static		vu8				FlashMutexFlag  =1;			//如果运行在无OS状态下由该变量实现互斥操作
static		vu8				CFlashMutexFlag =1;			//CPU FLASH 互斥变量


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

extern    OS_EVENT  *FlashMutexSem;         // 用于FLASH SPI总线底层应用之间的互斥

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static		void 			SPI_FLASH_WaitForWriteEnd							(void);

/*----- Flash	Low layer function -----*/

static		void 			SPI_FLASH_WriteEnable									(void);
static		u16 			SPI_FLASH_SendHalfWord								(u16 HalfWord);
static		void 			SPI_FLASH_PageWrite										(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);

/*----- CPU Flash	Low layer function -----*/
static		void 			CPU_FLASH_Unlock 											(void);
static		void 			CPU_FLASH_Lock 												(void);
static		u8 				FLASH_DisableWriteProtectionPages			(u32 memoryMask);
static		u32 			FLASH_PagesMask												(vu32 size);
/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            BSP_FLASH_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : 
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*********************************************************************************************************
*/
void	BSP_FLASH_Init	(void)
{
		SoftDelayMS(50);	// 防止刚上电CPU处理过快而导致FLASH(TPUW)还没有完成上电初始化
		//SOFT_DELAY_MS(DEF_TPUW_TIME);													

		#if(DEF_FLASH_PORT > 0)
				/* Init the spi bus */
				BSP_SPI_Init(DEF_FLASH_PORT);
		#endif

    // 禁止写保护操作
    SPI_FLASH_WP_DIS();	
	 			
		// 检测FLASH存储器是否工作正常
		BSP_FLASH_Check();
		//FLASH_DEVICE.HardWareSta	=	DEF_FLASHHARD_NONE_ERR;
	
//		#if(DEF_INITINFO_OUTPUTEN > 0)
//		if(dbgInfoSwt & DBG_INFO_SYS)
//		{
//				if(FLASH_DEVICE.HardWareSta== DEF_FLASHHARD_NONE_ERR)
//						myPrintf("[Init]: FLASH......(ok)!\r\n");
//				else
//						myPrintf("[Init]: FLASH......(error)!\r\n");
//		}
//		#endif	
}		

/*
*********************************************************************************************************
*                                            SPI_FLASH_BSP
*********************************************************************************************************
*/

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : SectorAddr: address of the sector to erase.
* Output         : None
* Return         : None
*******************************************************************************/
s8 BSP_SPI_FLASH_SectorErase(u32 SectorAddr,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode =DEF_SPI_NONE_ERR;

		if(OSRunning > 0)
				OSSemPend(FlashMutexSem,	timeout,	&err);		//	请求发送数据信号量
		else
				if(FlashMutexFlag	== 1)
						FlashMutexFlag	=	0;
				else
						return	DEF_SPI_BUSY_ERR;		
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_SPI_TIMEOUT_ERR;
				else
						return	DEF_SPI_BUSY_ERR;		
		}	
		else
		{	  
				#if(DEF_FLASH_WP_EN > 0)	
				/* Disable the FLASH write protect: Write protect high */ 
				SPI_FLASH_WP_DIS();
				#endif
			
				/* Send write enable instruction */
			  SPI_FLASH_WriteEnable();
			
			  /* Sector Erase */
				
			  /* Select the FLASH: Chip Select low */
			  SPI_FLASH_CS_EN();
			  /* Send Sector Erase instruction */
			  BSP_SPI_FLASH_SendByte(FLASH_SE);
			  /* Send SectorAddr high nibble address byte */
			  BSP_SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
			  /* Send SectorAddr medium nibble address byte */
			  BSP_SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
			  /* Send SectorAddr low nibble address byte */
			  BSP_SPI_FLASH_SendByte(SectorAddr & 0xFF);
			  /* Deselect the FLASH: Chip Select high */
			  SPI_FLASH_CS_DIS();
			
			  /* Wait the end of Flash writing */
			  SPI_FLASH_WaitForWriteEnd();
		
				#if(DEF_FLASH_WP_EN > 0)
				/* Enable the FLASH write protect: Write protect low */ 
				SPI_FLASH_WP_EN();
				#endif

				retCode	=	DEF_SPI_NONE_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(FlashMutexSem);											//	释放占用的信号量
		else
				FlashMutexFlag	=	1;

		return	retCode;	
}
/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_BlockErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
s8 BSP_SPI_FLASH_BlockErase	(u32 BlockAddr,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode =DEF_SPI_NONE_ERR;

		if(OSRunning > 0)
				OSSemPend(FlashMutexSem,	timeout,	&err);		//	请求发送数据信号量
		else
				if(FlashMutexFlag	== 1)
						FlashMutexFlag	=	0;
				else
						return	DEF_SPI_BUSY_ERR;		
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_SPI_TIMEOUT_ERR;
				else
						return	DEF_SPI_BUSY_ERR;		
		}	
		else
		{		
				#if(DEF_FLASH_WP_EN > 0)
			  /* Disable the FLASH write protect: Write protect high */ 
				SPI_FLASH_WP_DIS();
				#endif
				/* Send write enable instruction */
			  SPI_FLASH_WriteEnable();
			
			  /* Bulk Erase */ 
			  /* Select the FLASH: Chip Select low */
			  SPI_FLASH_CS_EN();
			  /* Send Bulk Erase instruction  */
			  BSP_SPI_FLASH_SendByte(FLASH_BE);
				/* Send BlockAddr high nibble address byte */
			  BSP_SPI_FLASH_SendByte((BlockAddr & 0xFF0000) >> 16);
			  /* Send BlockAddr medium nibble address byte */
			  BSP_SPI_FLASH_SendByte((BlockAddr & 0xFF00) >> 8);
			  /* Send BlockAddr low nibble address byte */
			  BSP_SPI_FLASH_SendByte(BlockAddr & 0xFF);
			  /* Deselect the FLASH: Chip Select high */
			  SPI_FLASH_CS_DIS();
			
			  /* Wait the end of Flash writing */
			  SPI_FLASH_WaitForWriteEnd();
				#if(DEF_FLASH_WP_EN > 0)
				/* Enable the FLASH write protect: Write protect low */ 
				SPI_FLASH_WP_EN();
				#endif

				retCode	=	DEF_SPI_NONE_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(FlashMutexSem);											//	释放占用的信号量
		else
				FlashMutexFlag	=	1;

		return	retCode;	
}
/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_ChipErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
s8 BSP_SPI_FLASH_ChipErase	(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode =DEF_SPI_NONE_ERR;

		if(OSRunning > 0)
				OSSemPend(FlashMutexSem,	timeout,	&err);		//	请求发送数据信号量
		else
				if(FlashMutexFlag	== 1)
						FlashMutexFlag	=	0;
				else
						return	DEF_SPI_BUSY_ERR;		
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_SPI_TIMEOUT_ERR;
				else
						return	DEF_SPI_BUSY_ERR;		
		}	
		else
		{		
				#if(DEF_FLASH_WP_EN > 0)
			  /* Disable the FLASH write protect: Write protect high */ 
				SPI_FLASH_WP_DIS();
				#endif
				/* Send write enable instruction */
			  SPI_FLASH_WriteEnable();
			
			  /* Bulk Erase */ 
			  /* Select the FLASH: Chip Select low */
			  SPI_FLASH_CS_EN();
			  /* Send Bulk Erase instruction  */
			  BSP_SPI_FLASH_SendByte(FLASH_CHIPER);
			  /* Deselect the FLASH: Chip Select high */
			  SPI_FLASH_CS_DIS();
			
			  /* Wait the end of Flash writing */
			  SPI_FLASH_WaitForWriteEnd();
				#if(DEF_FLASH_WP_EN > 0)
				/* Enable the FLASH write protect: Write protect low */ 
				SPI_FLASH_WP_EN();
				#endif

				retCode	=	DEF_SPI_NONE_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(FlashMutexSem);											//	释放占用的信号量
		else
				FlashMutexFlag	=	1;

		return	retCode;	
}
/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value. 
* Output         : None
* Return         : None
*******************************************************************************/
static	void 	SPI_FLASH_PageWrite	(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
#if(DEF_FLASH_WP_EN > 0)
	  /* Disable the FLASH write protect: Write protect high */ 
		SPI_FLASH_WP_DIS();
#endif
		/* Enable the write access to the FLASH */
	  SPI_FLASH_WriteEnable();
	  
	  /* Select the FLASH: Chip Select low */
	  SPI_FLASH_CS_EN();
	  /* Send "Write to Memory " instruction */
	  BSP_SPI_FLASH_SendByte(FLASH_WRITE);
	  /* Send WriteAddr high nibble address byte to write to */
	  BSP_SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	  /* Send WriteAddr medium nibble address byte to write to */
	  BSP_SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);  
	  /* Send WriteAddr low nibble address byte to write to */
	  BSP_SPI_FLASH_SendByte(WriteAddr & 0xFF);
	  
	  /* while there is data to be written on the FLASH */
	  while(NumByteToWrite--) 
	  {
		    /* Send the current byte */
		    BSP_SPI_FLASH_SendByte(*pBuffer);
		    /* Point on the next byte to be written */
		    pBuffer++; 
	  }
	  
	  /* Deselect the FLASH: Chip Select high */
	  SPI_FLASH_CS_DIS();
	  
	  /* Wait the end of Flash writing */
	  SPI_FLASH_WaitForWriteEnd();

#if(DEF_FLASH_WP_EN > 0)
	  /* Enable the FLASH write protect: Write protect low */ 
		SPI_FLASH_WP_EN();
#endif
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
s8 BSP_SPI_FLASH_BufferWrite (u32 WriteAddr,	u8* pBuffer,  u16 NumByteToWrite,	u32 timeout)
{
	  INT8U	err =OS_NO_ERR;
		s8	retCode =DEF_SPI_NONE_ERR;
		u16 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;	

		if(OSRunning > 0)
				OSSemPend(FlashMutexSem,	timeout,	&err);		//	请求发送数据信号量
		else
				if(FlashMutexFlag	== 1)
						FlashMutexFlag	=	0;
				else
						return	DEF_SPI_BUSY_ERR;	
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_SPI_TIMEOUT_ERR;
				else
						return	DEF_SPI_BUSY_ERR;		
		}	
		else
		{		
			  Addr = WriteAddr % SPI_FLASH_PageSize;
			  count = SPI_FLASH_PageSize - Addr;
			  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
			  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
			  
			  if(Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
			  {
				    if(NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
				    {
				      	SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
				    }
				    else /* NumByteToWrite > SPI_FLASH_PageSize */ 
				    {
					      while(NumOfPage--)
					      {
						        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
						        WriteAddr +=  SPI_FLASH_PageSize;
						        pBuffer += SPI_FLASH_PageSize;  
					      }    
				      	SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
				   	}
			  }
			  else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
			  {
				    if(NumOfPage== 0) /* NumByteToWrite < SPI_FLASH_PageSize */
				    {
					      if(NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
					      {
						        temp = NumOfSingle - count;
						      
						        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
						        WriteAddr +=  count;
						        pBuffer += count; 
						        
						        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
					      }
					      else
					      {
					        	SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
					      }
				    }
				    else /* NumByteToWrite > SPI_FLASH_PageSize */
				    {
					      NumByteToWrite -= count;
					      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
					      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
					      
					      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
					      WriteAddr +=  count;
					      pBuffer += count;  
					     
					      while(NumOfPage--)
					      {
						        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
						        WriteAddr +=  SPI_FLASH_PageSize;
						        pBuffer += SPI_FLASH_PageSize;
					      }
					      
					      if(NumOfSingle != 0)
					      {
					        	SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
					      }
				    }
			  }
				retCode	=	DEF_SPI_NONE_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(FlashMutexSem);											//	释放占用的信号量
		else
				FlashMutexFlag	=	1;

		return	retCode;	
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
s8 BSP_SPI_FLASH_BufferRead (u32 ReadAddr, u8* pBuffer, u16 NumByteToRead,	u32 timeout)
{

		INT8U	err =OS_NO_ERR;
		s8	retCode =DEF_SPI_NONE_ERR;
	
		if(OSRunning > 0)
				OSSemPend(FlashMutexSem,	timeout,	&err);		//	请求发送数据信号量
		else
				if(FlashMutexFlag	== 1)
						FlashMutexFlag	=	0;
				else
						return	DEF_SPI_BUSY_ERR;	
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_SPI_TIMEOUT_ERR;
				else
						return	DEF_SPI_BUSY_ERR;		
		}	
		else
		{		
				#if(DEF_FLASH_WP_EN > 0)
			  /* Disable the FLASH write protect: Write protect high */ 
				SPI_FLASH_WP_DIS();
				#endif
			
				/* Select the FLASH: Chip Select low */
			  SPI_FLASH_CS_EN();
			  
			  /* Send "Read from Memory " instruction */
			  BSP_SPI_FLASH_SendByte(FLASH_READ);
			  
			  /* Send ReadAddr high nibble address byte to read from */
			  BSP_SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
			  /* Send ReadAddr medium nibble address byte to read from */
			  BSP_SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
			  /* Send ReadAddr low nibble address byte to read from */
			  BSP_SPI_FLASH_SendByte(ReadAddr & 0xFF);
		
			  while(NumByteToRead--) /* while there is data to be read */
			  {
				    /* Read a byte from the FLASH */
				    *pBuffer = BSP_SPI_FLASH_SendByte(FLASH_Dummy_Byte);
				    /* Point to the next location where the byte read will be saved */
				    pBuffer++;
			  }
		  
			  /* Deselect the FLASH: Chip Select high */
			  SPI_FLASH_CS_DIS();
				#if(DEF_FLASH_WP_EN > 0)
				/* Enable the FLASH write protect: Write protect low */ 
				SPI_FLASH_WP_EN();
				#endif
				retCode	=	DEF_SPI_NONE_ERR;
		}

		if(OSRunning > 0)
				OSSemPost(FlashMutexSem);											//	释放占用的信号量
		else
				FlashMutexFlag	=	1;
		return	retCode;	
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
s8 BSP_SPI_FLASH_ReadID (u32 *id,	u32	timeout)
{
  	u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

		INT8U	err =OS_NO_ERR;
		s8	retCode =DEF_SPI_NONE_ERR;
			
		if(OSRunning > 0)
				OSSemPend(FlashMutexSem,	timeout,	&err);		//	请求发送数据信号量
		else
				if(FlashMutexFlag	== 1)
						FlashMutexFlag	=	0;
				else
						return DEF_SPI_BUSY_ERR;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_SPI_TIMEOUT_ERR;
				else
						return	DEF_SPI_BUSY_ERR;		
		}
		else
		{				
				#if(DEF_FLASH_WP_EN > 0)
				/* Disable the FLASH write protect: Write protect high */ 
				SPI_FLASH_WP_DIS();
				#endif
			  /* Select the FLASH: Chip Select low */
			  SPI_FLASH_CS_EN();
			  
			  /* Send "RDID " instruction */
			  BSP_SPI_FLASH_SendByte(FLASH_RDID);
			
			  /* Read a byte from the FLASH */
			  Temp0 = BSP_SPI_FLASH_SendByte(FLASH_Dummy_Byte);
			
			  /* Read a byte from the FLASH */
			  Temp1 = BSP_SPI_FLASH_SendByte(FLASH_Dummy_Byte);
			
			  /* Read a byte from the FLASH */
			  Temp2 = BSP_SPI_FLASH_SendByte(FLASH_Dummy_Byte);
			
			  /* Deselect the FLASH: Chip Select high */
			  SPI_FLASH_CS_DIS();
			         
			  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
		
				#if(DEF_FLASH_WP_EN > 0)
				/* Enable the FLASH write protect: Write protect low */ 
				SPI_FLASH_WP_EN();
				#endif

				*id	= Temp;

				retCode	=	DEF_SPI_NONE_ERR;		
		}

		if(OSRunning > 0)
				OSSemPost(FlashMutexSem);											//	释放占用的信号量
		else
				FlashMutexFlag	=	1;

		return retCode;
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_SPI_FLASH_StartReadSequence(u32 ReadAddr)
{  
	  /* Send "Read from Memory " instruction */
	  BSP_SPI_FLASH_SendByte(FLASH_READ);
	
		/* Send the 24-bit address of the address to read from -----------------------*/  
	  /* Send ReadAddr high nibble address byte */
	  BSP_SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	  /* Send ReadAddr medium nibble address byte */
	  BSP_SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
	  /* Send ReadAddr low nibble address byte */
	  BSP_SPI_FLASH_SendByte(ReadAddr & 0xFF);
		/* Enable the FLASH write protect: Write protect low */ 
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_ReadByte
* Description    : Reads a byte from the SPI Flash.
*                  This function must be used only if the Start_Read_Sequence
*                  function has been previously called.
* Input          : None
* Output         : None
* Return         : Byte Read from the SPI Flash.
*******************************************************************************/
u8 BSP_SPI_FLASH_ReadByte	(void)
{
		return (BSP_SPI_FLASH_SendByte(FLASH_Dummy_Byte));
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte 
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 BSP_SPI_FLASH_SendByte	(u8 byte)
{
		SPI_TypeDef	*spi;

		if(DEF_FLASH_PORT	== DEF_SPI_1)
				spi = SPI1;
		else if(DEF_FLASH_PORT	== DEF_SPI_2)
				spi = SPI2;
		else	
				return	0;

	  /* Loop while DR register in not emplty */
	  while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);
	
	  /* Send byte through the SPI1 peripheral */
	  SPI_I2S_SendData(spi, byte);
	
	  /* Wait to receive a byte */
	  while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);
	
	  /* Return the byte read from the SPI bus */
	  return SPI_I2S_ReceiveData(spi);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendHalfWord
* Description    : Sends a Half Word through the SPI interface and return the  
*                  Half Word received from the SPI bus.
* Input          : Half Word : Half Word to send.
* Output         : None
* Return         : The value of the received Half Word.
*******************************************************************************/
static	u16 SPI_FLASH_SendHalfWord	(u16 HalfWord)
{
	  SPI_TypeDef	*spi;

		if(DEF_FLASH_PORT	== DEF_SPI_1)
				spi = SPI1;
		else if(DEF_FLASH_PORT	== DEF_SPI_2)
				spi = SPI2;
		else	
				return	0;
		
		/* Loop while DR register in not emplty */
	  while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE) == RESET);
	
	  /* Send Half Word through the SPI1 peripheral */
	  SPI_I2S_SendData(spi, HalfWord);
	
	  /* Wait to receive a Half Word */
	  while(SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET);
	
	  /* Return the Half Word read from the SPI bus */
	  return SPI_I2S_ReceiveData(spi);
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WriteEnable
* Description    : Enables the write access to the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static	void SPI_FLASH_WriteEnable(void)
{
		/* Select the FLASH: Chip Select low */
	  SPI_FLASH_CS_EN();

		/* Send "Write Enable" instruction */
	  BSP_SPI_FLASH_SendByte(FLASH_WREN);
	  
	  /* Deselect the FLASH: Chip Select high */
	  SPI_FLASH_CS_DIS();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_WaitForWriteEnd
* Description    : Polls the status of the Write In Progress (WIP) flag in the  
*                  FLASH's status  register  and  loop  until write  opertaion
*                  has completed.  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static	void SPI_FLASH_WaitForWriteEnd(void)
{
	  u8 FLASH_Status = 0;
	
		/* Select the FLASH: Chip Select low */
	  SPI_FLASH_CS_EN();
	  
	  /* Send "Read Status Register" instruction */
	  BSP_SPI_FLASH_SendByte(FLASH_RDSR);
	  
	  /* Loop as long as the memory is busy with a write cycle */
	  do
	  {
		    /* Send a dummy byte to generate the clock needed by the FLASH 
		    and put the value of the status register in FLASH_Status variable */
		    FLASH_Status = BSP_SPI_FLASH_SendByte(FLASH_Dummy_Byte);
	
	  } while((FLASH_Status & FLASH_WIP_Flag) == SET); /* Write in progress */
	
	  /* Deselect the FLASH: Chip Select high */
	  SPI_FLASH_CS_DIS();
}

/*******************************************************************************
* Function Name  : BSP_SPI_FLASH_BufferWriteExt(有写入对比机制)
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
s8 BSP_SPI_FLASH_BufferWriteExt (u32 WriteAddr,	u8* pBuffer,  u16 NumByteToWrite,	u32 timeout)
{	
		s8	ret=0;
		u8 readBuf[DEF_CMPWRITE_BUF_SIZE]="";	
	
		//test
		//u8	ascBuf[257]={0};
		//test

		memset(readBuf,	'\0',	DEF_CMPWRITE_BUF_SIZE);
		ret = BSP_SPI_FLASH_BufferWrite(WriteAddr, pBuffer, NumByteToWrite,	timeout);
		if(ret != DEF_SPI_NONE_ERR)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: Write error!\r\n");
				#endif
				return	DEF_SPI_BUSY_ERR;
		}
		
		// 写入长度小于对比缓冲区则只比较缓冲区长度
		if(NumByteToWrite <= DEF_CMPWRITE_BUF_SIZE)
		{
				// 需要写入的数据小于临时比较区
				ret = BSP_SPI_FLASH_BufferRead(WriteAddr, readBuf, NumByteToWrite,	timeout);
				if(ret != DEF_SPI_NONE_ERR)
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: Write read error!\r\n");
						#endif
						return	DEF_SPI_BUSY_ERR;	
				}
				if(memcmp(pBuffer,	readBuf,	NumByteToWrite) != 0)
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: Write cmp error!\r\n");
						#endif
						/*
						//test
						memset(ascBuf,	'\0',	sizeof(ascBuf));
						Hex2StringArray (pBuffer,	NumByteToWrite,	ascBuf);
						myPrintf("[Test]: Write addr is[%x]!\r\n",	WriteAddr);
						myPrintf("[Test]: Write data is[%s]!\r\n",	ascBuf);
						
						memset(ascBuf,	'\0',	sizeof(ascBuf));
						Hex2StringArray (readBuf,	NumByteToWrite,	ascBuf);
						myPrintf("[Test]: Read data is[%s]!\r\n",	ascBuf);
						//tset
						*/
						return	DEF_SPI_WRITECMP_ERR;				
				}
				return	DEF_SPI_NONE_ERR;
		}
		else
		{
				// 写入数据大于对比缓冲区
				ret = BSP_SPI_FLASH_BufferRead(WriteAddr, readBuf, DEF_CMPWRITE_BUF_SIZE,	timeout);
				if(ret != DEF_SPI_NONE_ERR)
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: Write read error!\r\n");
						#endif
						return	DEF_SPI_BUSY_ERR;
				}
				if(memcmp(pBuffer,	readBuf,	DEF_CMPWRITE_BUF_SIZE) != 0)
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: Write cmp error!\r\n");
						#endif
						/*
						//test
						memset(ascBuf,	'\0',	sizeof(ascBuf));
						Hex2StringArray (pBuffer,	NumByteToWrite,	ascBuf);
						myPrintf("[Test]: Write addr is[%x]!\r\n",	WriteAddr);
						myPrintf("[Test]: Write data is[%s]!\r\n",	ascBuf);
						
						memset(ascBuf,	'\0',	sizeof(ascBuf));
						Hex2StringArray (readBuf,	NumByteToWrite,	ascBuf);
						myPrintf("[Test]: Read data is[%s]!\r\n",	ascBuf);
						//tset
						*/
						return	DEF_SPI_WRITECMP_ERR;				
				}
				return	DEF_SPI_NONE_ERR;
		}		
}
/*
******************************************************************************
* Function Name  : BSP_SPI_FLASHSemPend()
* Description    : 用于FLASH信号量的获取，必须与BSP_GSMSemPost成对使用
* Input          : timeout :等待信号量超时时间
* Output         : None
* Return         : None
******************************************************************************
*/
s8	BSP_SPI_FLASHSemPend (u32	timeout)
{
		INT8U	err =OS_NO_ERR;
			
		if(OSRunning > 0)
				OSSemPend(FlashMutexSem,	timeout,	&err);		//	请求发送数据信号量
		else
				if(FlashMutexFlag	== 1)
						FlashMutexFlag	=	0;
				else
						return -1;
		
		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	-1;
				else
						return	-1;		
		}
		return 0;
}
/*
******************************************************************************
* Function Name  : BSP_SPI_FLASHSemPost()
* Description    : 用于FLASH信号量的释放，必须与BSP_GSMSemPend成对使用
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
void BSP_SPI_FLASHSemPost (void)
{
		if(OSRunning > 0)
				OSSemPost(FlashMutexSem);											//	释放占用的信号量
		else
				FlashMutexFlag	=	1;
}
/*
******************************************************************************
* Function Name  : BSP_FLASH_Check
* Description    : [注]FLASH检测使用第0页，执行完后将丢失之前的数据
* Input          : None
* Output         : None
* Return         : DEF_FALSE, DEF_TRUE
******************************************************************************
*/
s8 BSP_FLASH_Check (void)
{		
		u8 checkcode[20]="",tmpbuf[20]="";
				
		if(BSP_SPI_FLASH_SectorErase(0,	1000) != DEF_SPI_NONE_ERR)
		{
				FLASH_DEVICE.HardWareSta	=	DEF_FLASHHARD_DEVICE_ERR;
				return FLASH_DEVICE.HardWareSta;	
		}
		memset((u8 *)checkcode, '7',	sizeof(checkcode));
		BSP_SPI_FLASH_BufferWrite (0,	checkcode,  sizeof(checkcode),5000);
		BSP_SPI_FLASH_BufferRead  (0, tmpbuf, sizeof(tmpbuf),	5000);
	
		if(memcmp((u8 *)tmpbuf, (u8 *)checkcode, sizeof(tmpbuf)) != 0)
		{
				FLASH_DEVICE.HardWareSta	=	DEF_FLASHHARD_DEVICE_ERR;
				FLASH_DEVICE.InitSta      = 0;	// 置模块无效
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
				myPrintf("[FLASH]: Check error!\r\n");
				#endif
		}
		else
		{
				FLASH_DEVICE.HardWareSta	=	DEF_FLASHHARD_NONE_ERR;
				FLASH_DEVICE.InitSta      = 1;	// 置模块有效
		}

		return FLASH_DEVICE.HardWareSta;
}

/*
*********************************************************************************************************
*                                     SPI_FLASH_UPDATE(升级过程中使用到的外部FLASH接口函数)
*********************************************************************************************************
*/
/*******************************************************************************
* Function Name  : BSP_IAP_Erase_WFlash
* Description    : 擦除外部Flash相应区域
* Input          : addr(只支持整个扇区的整数倍擦除,如果长度能被块整除则采用块擦除，否则采用扇区擦除)
* Output         : None
* Return         : None	
*******************************************************************************/
s8	BSP_IAP_Erase_WFlash (u32	addr,	u32	len)
{
		u32	i=0;
		u32	tmpCunAddr =addr;
		u32	tBeforeSector=0,	tMidBlock=0,	tRemainSector=0;
		
		// 判断地址及长度合法性/////////////
		if(((addr % SPI_FLASH_SectorSize) != 0) || ((len % SPI_FLASH_SectorSize) != 0) || (len == 0))
				return	-1;
		
		// 地址块对齐//////////////////////
		if((addr % SPI_FLASH_BlockSize) == 0)
		{
				// 不足一整块/////////////
				if(len < SPI_FLASH_BlockSize)
				{
						tBeforeSector = len/SPI_FLASH_SectorSize;
						for(i=0;	i<tBeforeSector;	i++)
						{
								if(BSP_SPI_FLASH_SectorErase	(tmpCunAddr,	5000) != 0)
										return	-1;
								
								tmpCunAddr += SPI_FLASH_SectorSize; 	
						}					
				}
				// 大于或等于一整块
				else
				{
						// 首先擦除整块部分
						tMidBlock = len/SPI_FLASH_BlockSize;
						for(i=0;	i<tMidBlock;	i++)
						{
								if(BSP_SPI_FLASH_BlockErase	(tmpCunAddr,	5000) != 0)
										return	-1;
								
								tmpCunAddr += SPI_FLASH_BlockSize; 	
						}

						// 然后擦除剩余不足一整块部分						
						tRemainSector = (len - tMidBlock * SPI_FLASH_BlockSize)/SPI_FLASH_SectorSize;
						for(i=0;	i<tRemainSector;	i++)
						{
								if(BSP_SPI_FLASH_SectorErase	(tmpCunAddr,	5000) != 0)
										return	-1;
								
								tmpCunAddr += SPI_FLASH_SectorSize; 	
						}
				}
		}	
		// 地址非块对齐//////////////////////
		else
		{		
				// 擦除之前不足整Block部分扇区//////////////
				tBeforeSector = (SPI_FLASH_BlockSize - (addr % SPI_FLASH_BlockSize)) / SPI_FLASH_SectorSize;	
				if((len / SPI_FLASH_SectorSize) < tBeforeSector)
						tBeforeSector = len / SPI_FLASH_SectorSize;

				for(i=0;	i<tBeforeSector;	i++)
				{
						if(BSP_SPI_FLASH_SectorErase	(tmpCunAddr,	5000) != 0)
								return	-1;
						
						tmpCunAddr += SPI_FLASH_SectorSize; 	
				}	
		
				// 擦除整Block部分/////////////////
				if((len > (SPI_FLASH_BlockSize - (addr % SPI_FLASH_BlockSize))) && 
				   (((len - (SPI_FLASH_BlockSize - (addr % SPI_FLASH_BlockSize))) / SPI_FLASH_BlockSize) > 0)) 
				{
						tMidBlock = (len - (SPI_FLASH_BlockSize - (addr % SPI_FLASH_BlockSize))) / SPI_FLASH_BlockSize;		
						for(i=0;	i<tMidBlock;	i++)		// 整块擦写
						{
								if(BSP_SPI_FLASH_BlockErase	(tmpCunAddr,	5000) != 0)
										return	-1;
								
								tmpCunAddr +=	SPI_FLASH_BlockSize;		// 地址移动 	
						}
				}

				// 擦除剩余部分扇区/////////////////
				if((len > (tBeforeSector * SPI_FLASH_SectorSize) + (tMidBlock * SPI_FLASH_BlockSize)) &&
				   ((len - (tBeforeSector * SPI_FLASH_SectorSize) + (tMidBlock * SPI_FLASH_BlockSize)) / SPI_FLASH_SectorSize) > 0) 
				{
						tRemainSector = (len - (tBeforeSector * SPI_FLASH_SectorSize) - (tMidBlock * SPI_FLASH_BlockSize)) / SPI_FLASH_SectorSize;
						for(i=0;	i<tRemainSector;	i++)
						{
								if(BSP_SPI_FLASH_SectorErase	(tmpCunAddr,	5000) != 0)
										return	-1;
								
								tmpCunAddr += SPI_FLASH_SectorSize; 	
						}		
				}
		}
		return	0;
}

/*******************************************************************************
* Function Name  : BSP_IAP_ProgramBuf_to_WFlash
* Description    : 用提供的数据烧写外部Flash(外部FLASH擦除后操作)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
s8	BSP_IAP_ProgramBuf_to_WFlash (u8 *pSrcBuf,	u32	writeLen,	u32	WFlashAddr)	
{
		s8	ret=0;

		ret = BSP_SPI_FLASH_BufferWriteExt(WFlashAddr, pSrcBuf, writeLen,	5000); 
		if(ret == DEF_SPI_NONE_ERR)
				return	0;
		else
				return	-1;

}
/*
*********************************************************************************************************
*                                            CPU_FLASH_BSP(内部CPU FLASH操作函数)
*********************************************************************************************************
*/


/*
******************************************************************************
* Function Name  : CPU_FLASH_Unlock
* Description    : 解锁CPU Flash
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
static	void CPU_FLASH_Unlock (void)
{		
		//FLASH解锁
		FLASH_Unlock();
}
/*
******************************************************************************
* Function Name  : CPU_FLASH_Lock
* Description    : CPU Flash加锁
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
static	void CPU_FLASH_Lock (void)
{		
		//FLASH解锁
		FLASH_Lock();
}

/*******************************************************************************
* Function Name  : FLASH_DisableWriteProtectionPages
* Description    : Disable the write protection of desired pages
* Input          : None
* Output         : None
* Return         : 0-success 1-no write protect 0xff-failed
*******************************************************************************/
static	u8 	FLASH_DisableWriteProtectionPages	(u32 memoryMask)
{
		u32 useroptionbyte = 0, WRPR = 0;
		u16 var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
		FLASH_Status status = FLASH_BUSY;
		
		WRPR = FLASH_GetWriteProtectionOptionByte();
		
		/* Test if user memory is write protected */
		if ((WRPR & memoryMask) != memoryMask)
		{
				useroptionbyte = FLASH_GetUserOptionByte();
				
				memoryMask |= WRPR;
				
				status = FLASH_EraseOptionBytes();
				
				if (memoryMask != 0xFFFFFFFF)
				{
						status = FLASH_EnableWriteProtection((u32)~memoryMask);
				}
				
				/* Test if user Option Bytes are programmed */
				if ((useroptionbyte & 0x07) != 0x07)
				{ /* Restore user Option Bytes */
						if ((useroptionbyte & 0x01) == 0x0)
						{
								var1 = OB_IWDG_HW;
						}
						if ((useroptionbyte & 0x02) == 0x0)
						{
								var2 = OB_STOP_RST;
						}
						if ((useroptionbyte & 0x04) == 0x0)
						{
								var3 = OB_STDBY_RST;
						}
						
						FLASH_UserOptionByteConfig(var1, var2, var3);
				}			
				if (status == FLASH_COMPLETE)
						return	0;			// 解除写保护成功，系统需要复位来重新装载新的控制字
				else
						return	0xff;		// 解除写保护失败
		}
		else
				return	1;					// 未写保护
}
/*
******************************************************************************
* Function Name  : BSP_CPU_FLASH_ProCheck
* Description    : 查询CPU FLASH对应地址字段是否写保护如果写保护则打印提示信息后解除写保护后复位CPU
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
void 	BSP_CPU_FLASH_ProCheck (u32	addr)
{				
		u8	retSta=0;
		u32  BlockNbr =0,	UserMemoryMask = 0;
		bool 	FlashProtection =FALSE;
		
		// Get the number of block (4 or 2 pages) from where the user program will be loaded
		BlockNbr = (addr - 0x08000000) >> 12;
		
		// Compute the mask to test if the Flash memory, where the user program will be
		if (BlockNbr < 62)
		{
				UserMemoryMask = ((u32)~((1 << BlockNbr) - 1));
		}
		else
		{
				UserMemoryMask = ((u32)0x80000000);
		}
		
		// Test if any page of Flash memory where program user will be loaded is write protected 
		if ((FLASH_GetWriteProtectionOptionByte() & UserMemoryMask) != UserMemoryMask)
		{
				#if(DEF_MEMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_OTHER)
						myPrintf("[FLASH]: CPU flash write protected! program will disable and do reset!\r\n");
				#endif
				retSta = FLASH_DisableWriteProtectionPages(UserMemoryMask);
				if(retSta == 0)
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
								myPrintf("[FLASH]: CPU flash disable write success and will reset!\r\n");
						#endif
						BSP_CPU_Reset();	// 复位CPU
				}
				else if(retSta == 1)
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
								myPrintf("[FLASH]: CPU flash no need disable write protected!\r\n");
						#endif
				}
				else if(retSta == 0xff)
				{
						#if(DEF_MEMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_OTHER)
								myPrintf("[FLASH]: CPU flash disable write fail! cpu will reset!\r\n");
						#endif
						BSP_CPU_Reset();	// 复位CPU
				}
		}		
}
/*******************************************************************************
* Function Name  : FLASH_PagesMask
* Description    : Calculate the number of pages
* Input          : - Size: The image size
* Output         : None
* Return         : The number of pages
*******************************************************************************/
static	u32 FLASH_PagesMask	(vu32 size)
{
		u32 pagenumber = 0;	
		if ((size % DEF_CPUFLASH_PAGE_SIZE) != 0)
		{
				//pagenumber = (size / DEF_CPUFLASH_PAGE_SIZE) + 1;
				pagenumber = size / DEF_CPUFLASH_PAGE_SIZE;	// 如果不为页的整数倍也不擦除多余部分避免将其它程序区清除！！
		}
		else
		{
				pagenumber = size / DEF_CPUFLASH_PAGE_SIZE;
		}
		return pagenumber;
}
/*******************************************************************************
* Function Name  : BSP_CPU_FLASH_Erase
* Description    : 擦除内部CPU Flash相应区域
* Input          : addr(必须为页对齐的地址)
*                : len (必须为页的整数倍大小)
* Output         : None
* Return         : None
*******************************************************************************/
s8	BSP_CPU_FLASH_PageErase	(u32	addr,	u32	len)
{
		s8	retCode =DEF_CPUFLASH_NONE_ERR; 
		u32	i=0;
		u32	tNbrOfPage	=0;
		FLASH_Status status =FLASH_COMPLETE;

		if(CFlashMutexFlag	== 1)
				CFlashMutexFlag	=	0;
		else
				return	DEF_CPUFLASH_BUSY_ERR;		
		
		if (((len % DEF_CPUFLASH_PAGE_SIZE) != 0) || ((addr % DEF_CPUFLASH_PAGE_SIZE) != 0))
				retCode =DEF_CPUFLASH_FAIL_ERR;		// 地址非页对齐，长度非页的整数倍不做处理
		else
		{	
				CPU_FLASH_Unlock();		// 解锁FLASH			
				tNbrOfPage = FLASH_PagesMask(len);		// 按页擦除相应CPU flash 区,不足一页,不擦除
				for (i = 0; i < tNbrOfPage; i++)
				{
						status = FLASH_ErasePage(addr + (DEF_CPUFLASH_PAGE_SIZE * i));
						if(status != FLASH_COMPLETE)
						{
								retCode =DEF_CPUFLASH_FAIL_ERR;
								break;
						}
				}
				CPU_FLASH_Lock();			// 上锁FLASH			
		}
		CFlashMutexFlag	= 1;			// 释放资源
		return	retCode;
}
/*******************************************************************************
* Function Name  : BSP_CPU_FLASH_Write
* Description    : 将指定地址内容写入到CPU FLASH(按字写入，注意写之前请确保写入区域为擦除过的区域)
* Input          : *pWrite :需要写入的数据指针
*                : writeLen:需要写入的数据长度(写入长度必须为1页)
*                : writeAddr:需要写入CPU FLASH 页首地址(必须为页对齐的地址)
* Output         : None
* Return         : 0-success -1-fail
*******************************************************************************/
s8	BSP_CPU_FLASH_Write (u32	writeAddr,	u8	*pWrite,	u32  writeLen)	
{
		s8	retCode =DEF_CPUFLASH_NONE_ERR; 
		u32	j=0;
		u32	tmpCPUflashAddr	=writeAddr;	
		u32 RamSource  =0;
		u32	loopTimes	 =writeLen/DEF_CPUFLASH_PAGE_SIZE;
		u32	remainSize =writeLen%DEF_CPUFLASH_PAGE_SIZE;	
			
		if(CFlashMutexFlag	== 1)
				CFlashMutexFlag	=	0;
		else
				return	DEF_CPUFLASH_BUSY_ERR;			
		
		// 编写整块部分///////////////////////////////////			
		if((writeAddr % 0x400) != 0)
				retCode =DEF_CPUFLASH_FAIL_ERR;		// 地址非页对齐不做处理
		else
		{
				CPU_FLASH_Unlock();		// 解锁FLASH	
				RamSource = (u32)pWrite;
				for (j = 0;(j < writeLen) && (tmpCPUflashAddr < DEF_CPUFLASH_END_ADDR);j += 4)
				{
						// Program the data received into STM32F10x Flash 
						FLASH_ProgramWord(tmpCPUflashAddr , *(u32*)RamSource);
						if (*(u32*)tmpCPUflashAddr != *(u32*)RamSource)
						{									
								retCode =DEF_CPUFLASH_FAIL_ERR;
								break;
						}
						tmpCPUflashAddr += 4;
						RamSource += 4;
				}			
				CPU_FLASH_Lock();			// 上锁FLASH	
		}
		CFlashMutexFlag	= 1;	// 释放资源
		return	retCode;
}
/*******************************************************************************
* Function Name  : BSP_CPU_FLASH_Read
* Description    : 读取CPUFLASH相应区域内容(按字节读取)
* Input          : readAddr :需要读取的地址
*                : readLen	:需要写入的数据长度(读取长度必须为1页)
*                : *pRead		:读取数据缓冲区指针
* Output         : None
* Return         : 0-success -1-fail
*******************************************************************************/
s8	BSP_CPU_FLASH_Read (u32	readAddr,	u8 *pRead,	u32  readLen)	
{
		s8	retCode =DEF_CPUFLASH_NONE_ERR; 
		u32	j=0;
		u32	tmpCPUflashAddr	=readAddr;	
			
		if(CFlashMutexFlag	== 1)
				CFlashMutexFlag	=	0;
		else
				return	DEF_CPUFLASH_BUSY_ERR;			
		
		// 按字节读取数据///////////////////////////////////			
		for (j = 0;(j < readLen) && (tmpCPUflashAddr < DEF_CPUFLASH_END_ADDR);j++)
		{
				*(pRead + j) = *(u8*)tmpCPUflashAddr;
				tmpCPUflashAddr ++;
		}
		retCode =DEF_CPUFLASH_NONE_ERR;

		CFlashMutexFlag	= 1;	// 释放资源
		return	retCode;
}

/*****************************************end*********************************************************/

