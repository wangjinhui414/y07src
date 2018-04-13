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
// ∂®“ÂGPS  ˝æ›ª∫≥Â∂”¡–¥Û–°
#define	GPS_RAMDATA_SIZE							(3)							// GPS RAM ˝æ›ª∫≥Â«¯¥Ê¥¢∂”¡–¥Û–°
#define	GPS_RAMSMS_SIZE								(3)							// GPS RAM∂Ã–≈ª∫≥Â«¯¥Ê¥¢∂”¡–¥Û–°
#define	GPS_FLASHDATA_SIZE						(1296)					// GPS FLASHª∫≥Â«¯¥Ê¥¢∂”¡–¥Û–°
//#define	GPS_FLASHDATA_SIZE						(50)


// ±Í÷æ∂®“Â
#define	GPS_INITED_FLAG								((u16)0x8888)		// INIT0+INIT1 ±Ì æGPS¿Îœﬂ ˝æ›¥Ê¥¢«¯“—æ≠≥ı ºªØπ˝(2015-0-9 ∂”¡–Ω·ππ–ﬁ∏ƒ)
#define	GPS_QUEUSED_FLAG							((u16)0xA55A)		// FLAG0+FLAG1 ±Ì æGPS∂”¡–Ω·ππ“—æ≠ π”√π˝

// ∂®“ÂGPS¿Îœﬂ ˝æ›∂”¡–Ω·ππ¥Ê¥¢’º”√…»«¯∏ˆ ˝
#define	GPS_QUESECTOR_NUM							(3)												// —≠ª∑¥Œ ˝Œ™£∫…»«¯ƒ⁄∞¸∫¨“≥ ˝¡ø(16)*—≠ª∑¥Œ ˝(3)=48¥Œ
#define	GPS_QUE_LOOPS									(GPS_QUESECTOR_NUM * SPI_FLASH_SectorSize / SPI_FLASH_PageSize)	
																																// …»«¯1∂®“ÂŒ™≥ı ºªØ±Í÷æ¥Ê¥¢«¯£¨ µº “≥ ˝Œ™48“≥—≠ª∑¥Œ ˝Œ™48¥Œ
// Õ‚≤ø≤øFLASHµÿ÷∑ªÆ∑÷∂®“Â////////////////////////
#define	FLASH_SPI_ALL_SIZE						(SPI_FLASH_Endaddr+1)			// ”…FLASHµ◊≤„«˝∂Ø∂®“Â
#define	FLASH_SPI_ADDR_START					(0x000000)
#define	FLASH_SPI_FLAG1_START					(FLASH_SPI_ADDR_START)		// SPI FLASH±Í÷æ0∆ ºµÿ÷∑
#define	FLASH_SPI_FLAG1_SIZE					(SPI_FLASH_SectorSize)		// ±Í÷æ0’º”√ø’º‰¥Û–°(4K)‘§¡Ù£°
#define	FLASH_SPI_FLAG2_START					(FLASH_SPI_FLAG1_START + FLASH_SPI_FLAG1_SIZE)
#define	FLASH_SPI_FLAG2_SIZE					(SPI_FLASH_SectorSize)		// ±Í÷æ1’º”√ø’º‰¥Û–°(4K)‘§¡Ù£°
#define	FLASH_SPI_NAME1_START					(FLASH_SPI_FLAG2_START + FLASH_SPI_FLAG2_SIZE)	
#define	FLASH_SPI_NAME1_SIZE					(SPI_FLASH_SectorSize)		// ≥Ã–Ú±∏∑›Œª÷√1√˚◊÷’º”√ø’º‰¥Û–°(4K)
#define	FLASH_SPI_SIZE1_START					(FLASH_SPI_NAME1_START + FLASH_SPI_NAME1_SIZE)	
#define	FLASH_SPI_SIZE1_SIZE					(SPI_FLASH_SectorSize)		// ≥Ã–Ú±∏∑›Œª÷√1¥Û–°’º”√ø’º‰¥Û–°(4K)
#define	FLASH_SPI_APP1_START					(FLASH_SPI_SIZE1_START + FLASH_SPI_SIZE1_SIZE)	
#define	FLASH_SPI_APP1_SIZE						(SPI_FLASH_SectorSize*80)	// ≥Ã–Ú±∏∑›Œª÷√1¥Û’º”√ø’º‰¥Û–°(4K*80=320K)
#define	FLASH_SPI_NAME2_START					(FLASH_SPI_APP1_START  + FLASH_SPI_APP1_SIZE)	
#define	FLASH_SPI_NAME2_SIZE					(SPI_FLASH_SectorSize)		// ≥Ã–Ú±∏∑›Œª÷√2√˚◊÷’º”√ø’º‰¥Û–°(4K)
#define	FLASH_SPI_SIZE2_START					(FLASH_SPI_NAME2_START + FLASH_SPI_NAME2_SIZE)	
#define	FLASH_SPI_SIZE2_SIZE					(SPI_FLASH_SectorSize)		// ≥Ã–Ú±∏∑›Œª÷√2¥Û–°’º”√ø’º‰¥Û–°(4K)
#define	FLASH_SPI_APP2_START					(FLASH_SPI_SIZE2_START + FLASH_SPI_SIZE2_SIZE)	
#define	FLASH_SPI_APP2_SIZE						(SPI_FLASH_SectorSize*80)	// ≥Ã–Ú±∏∑›Œª÷√2¥Û’º”√ø’º‰¥Û–°(4K*80=320K)
#define	FLASH_SPI_GPSQUE_INIT_START		(FLASH_SPI_APP2_START	 + FLASH_SPI_APP2_SIZE)	// ∂”¡–≥ı ºªØ±Í÷æ¥Ê¥¢µÿ÷∑
#define	FLASH_SPI_GPSQUE_INIT_SIZE		(SPI_FLASH_SectorSize)		// ≥ı ºªØ±Í÷æ’º”√“ª∏ˆ…»«¯
#define	FLASH_SPI_GPSQUE_START				(FLASH_SPI_GPSQUE_INIT_START + FLASH_SPI_GPSQUE_INIT_SIZE)
#define	FLASH_SPI_GPSQUE_SIZE					(SPI_FLASH_SectorSize*GPS_QUESECTOR_NUM)	// GPS¿Îœﬂ ˝æ›∂”¡–Ω·ππ’º”√ø’º‰¥Û–°
#define	FLASH_SPI_GPSDATA_START				(FLASH_SPI_GPSQUE_START	+ FLASH_SPI_GPSQUE_SIZE)
#define	FLASH_SPI_GPSDATA_SIZE				(SPI_FLASH_Endaddr - FLASH_SPI_GPSDATA_START + 1)	// GPS¿Îœﬂ ˝æ› £”‡¥Ê¥¢ø’º‰¥Û–°

#if (GPS_FLASHDATA_SIZE <= (FLASH_SPI_GPSDATA_SIZE / 256))	// »Áπ˚∂®“Âµƒ¿Îœﬂ ˝æ›≥¨π˝FLASH¥Ê¥¢ø’º‰‘Ú π”√ µº ¥Û–°
#define	GPS_FLASHDATA_USE							(GPS_FLASHDATA_SIZE - GPS_FLASHDATA_SIZE % 16)	// ∂”¡–¥Û–°±ÿ–ËŒ™16µƒ’˚ ˝±∂
#else
#define	GPS_FLASHDATA_USE							((FLASH_SPI_GPSDATA_SIZE/SPI_FLASH_PageSize) - ((FLASH_SPI_GPSDATA_SIZE/SPI_FLASH_PageSize) % 16))		// FLASH√ø“≥¥Ê¥¢“ª∏ˆGPSµ„				
#endif			

/////////////////////////////////////////////////////////////////////
// ≤Ÿ◊˜ ß∞‹÷ÿ ‘¥Œ ˝∂®“Â
#define	DEF_TRYTIMES_FLASHINIT				(3)				// SPI flash≥ı ºªØ’œ÷ÿ ‘¥Œ ˝
#define	DEF_TRYTIMES_FLASHREAD				(3)				// SPI flash∂¡÷ÿ ‘¥Œ ˝
#define	DEF_TRYTIMES_FLASHWRITE				(3)				// SPI flash–¥÷ÿ ‘¥Œ ˝	


//////////////////////////////////////////////////////////////////////

// ∂”¡–Õ®”√∂®“Â 
#define	DEF_PROQUE_QUE_OK							((s8)0)
#define	DEF_PROQUE_QUE_FULL						((s8)-1)
#define	DEF_PROQUE_QUE_EMPTY					((s8)-2)
#define	DEF_PROQUE_QUE_OVERFLOW				((s8)-3)
#define	DEF_PROQUE_QUE_ERR						((s8)-4) 
#define	DEF_PROQUE_QUE_WCMPERR				((s8)-5)

// ∂”¡–ƒ£ Ω∂®“Â
#define	DEF_RAMQUE_LOST_MODE					(1)				// ∂®“Â∂”¡–»Áπ˚¬˙¡À∂™µÙ◊Ó‘Áµƒ ˝æ›			
#define	DEF_FLASHQUE_LOST_MODE				(1)				// ∂®“Â∂”¡–»Áπ˚¬˙¡À◊‘∂Ø∂™µÙ◊Ó‘Áµƒ ˝æ›			


/*
*********************************************************************************************************
*                                               TYPE DEFINES
*********************************************************************************************************
*/

typedef enum {		
		DEF_RAM_QUE = 0,								// ∂”¡–¿‡–Õ∂®“Â-RAM ˝æ›∂”¡–		
		DEF_SMS_QUE,										// ∂”¡–¿‡–Õ∂®“Â-RAM∂Ã–≈∂”¡–		
		DEF_FLASH_QUE,									// ∂”¡–¿‡–Õ∂®“Â-FLASH∂”¡–	
		DEF_ALL_QUE,										// ∂”¡–¿‡–Õ∂®“Â-À˘”–∂”¡–	
}ENUM_QUE_TYPE;

/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/

//////////////////////////////////////////////////////////////////////
// ∑¢ÀÕ ˝æ›∞¸Ω·ππ

//  ˝æ›¬∑æ∂∂®“Â
#define	DEF_PATH_BT										((u8)2)
#define	DEF_PATH_SMS									((u8)1)
#define	DEF_PATH_GPRS									((u8)0)

// »Î∂”¥Ú∞¸ ± π”√µƒ ˝æ›¿‡–Õ∂®“
#define	DEF_TYPE_ACK									((u8)0xf0)					// Õ®”√”¶¥ ˝æ›
#define	DEF_TYPE_GPS									((u8)0x01)					// GPS¿‡–Õ ˝æ›
#define	DEF_TYPE_IG										((u8)0x02)					// µ„ª/œ®ª¿‡–Õ ˝æ›
#define	DEF_TYPE_ZITAI								((u8)0x03)					// ◊ÀÃ¨¿‡–Õ ˝æ›
#define	DEF_TYPE_LOG									((u8)0x04)					// LOG¿‡–Õ ˝æ›
#define	DEF_TYPE_ALARM								((u8)0x05)					// ±®æØ¿‡–Õ ˝æ›
#define	DEF_TYPE_24REPORT							((u8)0x06)					// 24–° ±Ω°øµ∞¸¿‡–Õ ˝æ›
#define	DEF_TYPE_QUEPARA							((u8)0x07)					// ≤Œ ˝≤È—Ø∑µªÿ¿‡–Õ ˝æ›
#define	DEF_TYPE_VIN									((u8)0x08)					// VIN∑µªÿ¿‡–Õ ˝æ›
#define	DEF_TYPE_CARCON								((u8)0x09)					// ≥µ¡æøÿ÷∆∑µªÿ¿‡–Õ ˝æ›
#define	DEF_TYPE_CLRCODE							((u8)0x0A)					// «Âπ ’œ¬Î∑µªÿ¿‡–Õ ˝æ›
#define	DEF_TYPE_CARSTA								((u8)0x0B)					// ≥µ¡æ◊¥Ã¨∑µªÿ¿‡–Õ ˝æ›
#define	DEF_TYPE_QUEPHO								((u8)0x0C)					// ∂Ã–≈øÿ÷∆∫≈¬Î≤È—Ø∑µªÿ¿‡–Õ ˝æ›
#define	DEF_TYPE_SMSCOM								((u8)0x0D)					// ∂Ã–≈≥µ¡æøÿ÷∆∑µªÿ¿‡–Õ ˝æ›
#define	DEF_TYPE_POWON								((u8)0x10)					// …Ë±∏…œµÁ±®Œƒ¿‡–Õ ˝æ›

// SMS ˝æ›∞¸Ω·ππ±®æØ¿‡–Õ∂®“Â 
#define	DEF_SMS_COM						        ((u8)0)      				// Õ®”√¿‡–Õ∂Ã–≈
#define	DEF_SMS_ALARM						      ((u8)1)      				// ±®æØ¿€∂Ã–≈

#define	DEF_PACK_BUF_SIZE					    (200)             	// ¥Ê¥¢»Î∂”µƒ ˝æ›≤Œ ˝«¯¥Û–°”¶∏√–°”⁄(256-26)
typedef struct	tag_SendData_Def
{
		u16			sum;																					// ¥Ê¥¢µƒÀ˘”– ˝æ›¿€º”∫Õ(path~data[])
		u8			path;																					// DEF_PATH_GPRS/SMS 
		u8			type;																					//  ˝æ›¿‡–Õº˚…œ√Ê ˝æ›¿‡–Õ∂®“Â(DEF_TYPE_ACK~DEF_TYPE_CARSTA)
		u8			smsAlType;																		// ªÿ∏¥µƒ∂Ã–≈¿‡–Õ£¨Õ®”√ªÚ «±®æØ/path=SMS ±”––ß
		u8			smsMode;																			// ªÿ∏¥µƒ∂Ã–≈◊÷∑˚ƒ£ Ω£¨GSMªÚ «UCS2/path=SMS ±”––ß		
		u8			smsNum[20];																		// ªÿ∏¥∂Ã–≈µƒµÁª∞∫≈¬Î/path=SMS ±”––ß		
		u16			len;																					//
		u8			data[DEF_PACK_BUF_SIZE];											// »Áπ˚Œ™GPRS ˝æ›‘ÚŒ™≤Œ ˝«¯ƒ⁄»›£¨»Áπ˚Œ™SMS‘ÚŒ™ µº ªÿ∏¥µƒ∂Ã–≈ ˝æ›ƒ⁄»› 
							
}SendData_TypeDef;

//////////////////////////////////////////////////////////////////////
// ∑¢ÀÕ∂”¡–Ω·ππ
typedef struct	tag_DataQue_Def
{
		u16			useFlag;																			// ∂”¡–”––ß±Í÷æ(”√”⁄÷∏ æ“‘œ¬∂”¡–Ω·ππ «∑Ò¥Ê¥¢–≈œ¢ «∑Ò”––ß)
		u16			sum;																					// ∂”¡– ˝æ›¿€º”∫Õ–£—È◊÷Ω⁄(FLASH∂”¡––Ë“™–£—È£¨Ram∂”¡–≤ª π”√)
		u16			start;																				// ∂”¡–ø™ ºŒª÷√
		u16			end;																					// ∂”¡–Ω· ¯Œª÷√
		u16			in;																						// ∂”¡–Õ∑÷∏’Î
		u16			out;																					// ∂”¡–Œ≤÷∏’Î
		u16			size;																					// ∂”¡–◊‹¥Û–°
		u16			entries;																			// ∂”¡–µ±«∞¥Ê¥¢ ˝æ›∞¸ ˝
		u16			outExt;																				// ∂”¡–¡Ÿ ±out÷∏’Î(2015-9-29 ÷˜“™’Î∂‘FLASH∂”¡–)
		u16			entriesExt;																		// ∂”¡–¡Ÿ ±¥Û–°(2015-9-29 ÷˜“™’Î∂‘FLASH∂”¡–)
							
}DataQue_TypeDef;
/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/

HAL_PROCESSQUE_EXT		DataQue_TypeDef				s_queRam;							// RAM÷–¥Ê¥¢∂”¡–
HAL_PROCESSQUE_EXT		DataQue_TypeDef				s_queSms;							// SMS÷–¥Ê¥¢∂”¡–
HAL_PROCESSQUE_EXT		DataQue_TypeDef				s_queFlash;						// FLASH÷–¥Ê¥¢∂”¡–
HAL_PROCESSQUE_EXT		SendData_TypeDef			s_ramData[GPS_RAMDATA_SIZE];		// RAM ˝æ›ª∫≥Â«¯		
HAL_PROCESSQUE_EXT		SendData_TypeDef			s_ramSms[GPS_RAMSMS_SIZE];			// RAM∂Ã–≈ª∫≥Â«¯	

			
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

// ∂”¡–”¶”√∫Ø ˝
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
