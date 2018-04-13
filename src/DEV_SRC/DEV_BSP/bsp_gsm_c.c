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
*                                     			bsp_gsm_c.c
*                                            with the
*                                   			Y05D Board
*
* Filename      : bsp_gsm_c.c
* Version       : V1.07
* Programmer(s) : Felix
*********************************************************************************************************
*/

// Çý¶¯ËµÃ÷£º
// Çý¶¯³ÌÐòÖ§³ÖSIMCOM¹«Ë¾µÄSIM800Ä£¿é£¨ÐèÒª×¢Òâ£º32M°æ±¾TTS»òÀ¶ÑÀÖ»ÄÜÈ¡Ò»£¬Ö»ÓÐ64M°æ±¾ÓÐÀ¶ÑÀºÍTTS£©
// ÍøÂç²¿·ÖÖ§³Ö¶à¸öTCPÁ¬½Ó£¬ÐèÒª×¢ÒâÈç¹ûµ÷ÓÃÒ»´ÎBSP_GPRSResetPro´¦Àíº¯ÊýÔòËùÓÐ±»½¨Á¢µÄTCPÁ¬½Ó¶¼½«¶Ï¿ª£¬
// BT²¿·ÖÖ»Ö§³Öµ¥Ò»Á¬½Ó£¬ÇÒµÚÒ»°ü±ØÐëÊÕµ½"SIMCOMSPPFORAPP"ºóÀ¶ÑÀ¹¦ÄÜ²Å³É¹¦¼¤»î£¬·ñÔò¶Ï¿ªÖØÁ¬¡£

// °æ±¾ËµÃ÷£º
// V1.00:	Ê×°æ½¨Á¢
// V1.01: ÎªÁËÌáÉý½ÓÊÕÊý¾ÝÐ§ÂÊ£¬·ÀÖ¹Êý¾ÝÒÅÂ©ÌØ½«GPRS½ÓÊÕÊý¾ÝÌáÊ¾·½Ê½¸ÄÎªÐÅºÅÁ¿Í¶µÝ£¬´úÌæÖ®Ç°µÄÓÊÏäÍ¶µÝ·½Ê½
// V1.02: ÐÞ¸ÄÒòÎª½ÓÊÕhexÊý¾ÝÖÐ°üº¬Æ¥Åä×Ö·û´®µ¼ÖÂµÄÌáÇ°ÍË³ö£¬´Ó¶øÊý¾Ý½ÓÊÕ²»ÍêÕû
// V1.03: Ôö¼Ó±¾»úºÅÂë¶ÁÈ¡ATÖ¸Áî¶¨Òå¼°²Ù×÷º¯Êý
// V1.04: µ÷ÕûÁËTTSÓïÒô²¥±¨Ìõ¼þ£¬¼´Ê¹²»²å¿¨Ò²¿É²¥±¨
// V1.05: ÓÅ»¯³õÊ¼»¯ÃüÁî(È¡Ïû¸úSIM¿¨¼ì²â¼°Ïà¹ØµÄ³õÊ¼»¯Ö¸Áî)£¬¶ÀÁ¢Ôö¼ÓÒ»¸ö¹ØÓÚSIM¿¨µÄ³õÊ¼»¯º¯Êý£¬ÍøÂçÅäÖÃÇ°Ôö¼ÓSIM×´Ì¬ÅÐ¶Ï
//        ¶ÌÐÅ·¢ËÍÇ°Ôö¼ÓSIM×´Ì¬ÅÐ¶Ï£¬ÍøÂçÁªÍøÓ¦´ðÔö¼ÓÒ»¸ö"CONNECT FAIL"Æ¥ÅäÏî
// V1.06: CNUM²éÑ¯SIM¿¨ºÅÊ±Ìø¹ý+86²¿·Ö
// V1.07: Ôö¼ÓGSMÇ¿ÖÆ¶ÏÍøº¯ÊýBSP_GPRSFroceBreakSet()
// V1.08:	½«TCP½ÓÊÕÊý¾Ý»®·ÖÎ»Á½¸öº¯Êý1¸öÓÃÓÚÓ¦ÓÃ¹ý³ÌÖÐ½ÓÊÕ£¬Ò»¸öÓ¦ÓÃÓÚÉý¼¶ÏÂÔØÎÄ¼þ¹ý³ÌÖÐ
// V1.09: ÓÅ»¯·ÇÉý¼¶Ä£Ê½ÖÐTCP½ÓÊÕº¯Êý
// V1.10: ÐÞ¸ÄTCP·¢ËÍº¯ÊýÄÚ²¿Æ¥ÅäÓ¦´ðATÎªÌØÊâÄ£Ê½Ö»ÓÐÊÕµ½ÆÚ´ý×Ö·û´®²ÅÍË³ö£¬±ÜÃâTTS×Ô¶¯ÉÏ±¨µÄURC½áÊøÓ°Ïì·¢ËÍÅÐ¶Ï£¬µ¼ÖÂÈÏÎª¶ÏÍø;
// V1.11: ÔÚÍøÂçÅäÖÃÖÐÔö¼ÓCGATTµÄ²éÑ¯²Ù×÷
// V1.12: ³õÊ¼»¯º¯ÊýÖÐÔö¼ÓURCÉÏ´«RINGÖÐ¶ÏÊ¹ÄÜ(ÃÖ²¹GPRS½ÓÊÕÊý¾ÝÒÅÂ©ÎÊÌâ)--¿ÉÄÜµ¼ÖÂ²»Í£¶ÁÈ¡½ÓÊÕÊý¾Ý,¸ÃÂß¼­ÒÑÈ¡Ïû!!!
// V1.13: ÐÞ¸ÄÉý¼¶ÍøÂç½ÓÊÕÊý¾Ýº¯Êý£¬¶ÁÈ¡Ç°ÅÐ¶Ï½ÓÊÕ³¤¶È´Ó¶øÔö¼ÓÑÓÊ±È·±£ÊÕµ½Êý¾ÝÍêÕûÐÔ
// V1.14: Ôö¼ÓCPIN×´Ì¬²éÑ¯µÄ´íÎó×Ö·ûÆ¥Åä(²å¿¨: CPIN: READY, ²»²å¿¨: +CPIN: NOT INSERTED(µÚÒ»´ÎÉÏµç) or ERROR)
// V1.15: ÐÞ¸ÄSIM×´Ì¬²éÑ¯´íÎó´íÎó´¦ÀíÖ»ÓÐ³¬Ê±²Å½øÐÐ´íÎóÀÛ¼Ó
// V1.16: ÐÞ¸Ä¶ÌÐÅ¶ÁÈ¡º¯Êý½«ÔËÓªÉÌ·¢ËÍÊ±¼ä¸ÄÎª¸ñÁÖÍþÖÎÊ±¼ä£»
// V1.17: ½«IPµØÖ·»ñÈ¡´íÎóºÏ²¢µ½Í¨ÓÃ´íÎóÖÐ
// V1.18: ÐÞ¸ÄGSMÄ£¿é¿ª»úI/OÅÐ¶Ï´íÎó£¬ÅÐ¶Ï¿ªÆô³¬Ê±Ê±¼ä5s->10s·ñÔòSIM800C¿ÉÄÜ»á³öÏÖ¼ì²â³¬Ê±£»


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GSM_GLOBLAS
#include  <stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	"cfg_h.h"
#include	"bsp_h.h"
#include	"bsp_gsm_h.h"
#include 	"ucos_ii.h"
#include	"main_h.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/********** ÄÚ²¿ GSM I/O ¿ØÖÆ½Ó¿Ú *************/

/* Enable the GSM device pow */
#define	IO_GSM_POW_EN()							(GPIO_SetBits		(DEF_GSM_POWEN_PORTS, DEF_GSM_POWEN_PINS))	
#define	IO_GSM_POW_DIS()						(GPIO_ResetBits	(DEF_GSM_POWEN_PORTS, DEF_GSM_POWEN_PINS))
														 	
/* Power on the GSM device */
#define	IO_GSM_POW_ON()							(GPIO_SetBits		(DEF_GSM_POWON_PORTS, DEF_GSM_POWON_PINS))
#define	IO_GSM_POW_OFF()						(GPIO_ResetBits	(DEF_GSM_POWON_PORTS, DEF_GSM_POWON_PINS))

/* Reset the GSM device  */
#define	IO_GSM_RES_ON()							(GPIO_SetBits		(DEF_GSM_POWRST_PORTS, DEF_GSM_POWRST_PINS))	
#define	IO_GSM_RES_OFF()						(GPIO_ResetBits	(DEF_GSM_POWRST_PORTS, DEF_GSM_POWRST_PINS))

/* GSM DTR control */
#define	IO_GSM_DTR_EN()							(GPIO_SetBits		(DEF_GSM_POWSLEEP_PORTS, DEF_GSM_POWSLEEP_PINS))	
#define	IO_GSM_DTR_DIS()						(GPIO_ResetBits	(DEF_GSM_POWSLEEP_PORTS, DEF_GSM_POWSLEEP_PINS))			

/* Read ring/power pin state */
#define	IO_GSM_RING_DET()						(GPIO_ReadInputDataBit (DEF_GSM_RING_PORTS, DEF_GSM_RING_PINS))
#define	IO_GSM_POW_DET()						(GPIO_ReadInputDataBit (DEF_GSM_POWDET_PORTS, DEF_GSM_POWDET_PINS))

/* Enable/Disable GSM rxd interrupt  */
#define	GSM_ENABLE_RES_IT()					(BSP_RS232_RxIntEn	(DEF_GSM_UART))		// include at bsp.h
#define GSM_DISABLE_RES_IT()				(BSP_RS232_RxIntDis	(DEF_GSM_UART))		// include at bsp.h

/* Enable/Disable GSM txd interrupt  */
#define	GSM_ENABLE_SEND_IT()				(BSP_RS232_TxIntEn	(DEF_GSM_UART))		// include at bsp.h
#define	GSM_DISABLE_SEND_IT()				(BSP_RS232_TxIntDis	(DEF_GSM_UART))		// include at bsp.h

/********** ÄÚ²¿Ê¹ÓÃº¯Êý½Ó¿Ú *************/

//(BSP_USART_Init(x,y,z))		// include at	bsp.h
//(StringSearch(x,y,z,k))		// include at	bsp.h					
//(OSTimeDly(x))						// include at	ucos_ii.h	
	

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

/* SIM800 Init cmd */
																									/* BASE INIT CMD" */
uc8 			AT_ATE0[] 				= "ATE0\r";           // Disable ECHO
uc8				AT_IPR[]					=	"AT+IPR=115200\r";	// Set 115200
uc8				AT_IFC[]					=	"AT+IFC=0,0\r";			// Disable the flow control
uc8				AT_CFUN1[]				=	"AT+CFUN=1\r";			// All function mode disable low power mode
uc8				AT_CFUN0[]				=	"AT+CFUN=0\r";			// Enable low power mode
uc8				AT_CFUN11[]				=	"AT+CFUN=1,1\r";		// Module softrest

																									/* SMS Init CMD */
uc8 			AT_CMGF[] 				= "AT+CMGF=1\r";      // Send SMS in text mode. 
uc8				AT_CSCS_GSM[]			=	"AT+CSCS=\"GSM\"\r";	// Set encode is GSM
uc8				AT_CSCS_UCS2[]		=	"AT+CSCS=\"UCS2\"\r";	// Set encode is UCS2
uc8				AT_CNMI[]					= "AT+CNMI=2,1\r";		// Auto remind on receive the SMS(Save at sim card)
uc8				AT_CPMS[]					= "AT+CPMS=\"SM\",\"SM\",\"SM\"\r";	// Set sms save at sim card
uc8				AT_CSCLK_1[]			= "AT+CSCLK=1\r";			// Enable sleep mode(DTR:1 goto sleep mode/DTR:0 exit sleep mode)
uc8				AT_CSCLK_0[]			= "AT+CSCLK=0\r";			// Disable sleep mode
uc8				AT_CTTSPARAM[]		=	"AT+CTTSPARAM=50,0,50,60,0\r";	// ÉèÖÃTTS²ÎÊý£¨¸ñÊ½£ºAT+CTTSPARAM=<Vol>,<Mode>,<Pitch>,<Spped>,<Channel>£©
uc8				AT_CFGRI_1[]			=	"AT+CFGRI=1\r";			// ÉèÖÃURCÉÏ´«Ê±°éËæÖÐ¶Ï²úÉú
uc8				AT_W[]						= "AT&W\r";						// Save parameter
																						

/*------------------------------------------------------------------------*/
uc8 *pATcommand[] =
{
		AT_ATE0,
    AT_IPR,
    AT_IFC,   	
		AT_CFUN1,
		AT_CSCLK_0,
		AT_CTTSPARAM,
		//AT_CFGRI_1,
		AT_W,			
};
/*------------------------------------------------------------------------*/
uc16 cmdTimeout[] =
{
    GSMTime2s,
    GSMTime2s,    
    GSMTime2s,
		GSMTime2s,
		GSMTime2s,
		//GSMTime2s,
		GSMTime2s,
};

/******************************************************************************************************/
/* AT Query cmd */
/******************************************************************************************************/
uc8				AT_CIMI[] 				= "AT+CIMI\r";				// Query IMSI code(È«ÇòÒÆ¶¯Éè±¸Ç©ÊðÊ¶±ðÂë)
uc8				AT_CGSN[] 				= "AT+CGSN\r";				// Query IMEI code(È«ÇòÒÆ¶¯Éè±¸Ê¶±ðÂë,µÈÍ¬ÓÚAT+GSN)
uc8				AT_GSN[] 					= "AT+GSN\r";					// Query IMEI code(µÈÍ¬ÓÚAT+CGSN)

uc8				AT_CSQ[] 					= "AT+CSQ\r";					// Query GSM signal strength
uc8				AT_CPIN[] 				= "AT+CPIN?\r";				// Query SIM card state
uc8				AT_COPS[] 				= "AT+COPS?\r";				// Query Operator Selection(µ±Ç°µÄÔËÓªÉÌ)
uc8				AT_CREG[] 				= "AT+CREG?\r";				// Query GSM regist state
uc8				AT_CGREG[] 				= "AT+CGREG?\r";			// Query GPRS regist state(µ±ÉèÖÃAT+CGATT=1ºó,·µ»Ø0,1)
uc8				AT_CGATT[]				=	"AT+CGATT?\r";			// Query attach or detach from GPRS(²éÑ¯ÊÇ·ñ¸½×Å»ò·ÖÀëµ½GPRSÍøÂç)
uc8				AT_CLCC[]					=	"AT+CLCC\r";				// Query Call state
uc8				AT_CNUM[]					=	"AT+CNUM\r";				// ²éÑ¯±¾»úºÅÂë(Ö»ÓÐSIM¿¨Ð´Èë¸ÃºÅÂëºó²Å¿É¶Á³ö,·ñÔò·µ»Ø¿Õ×Ö·û´®)

/******************************************************************************************************/
/* GPRS Init And Connect cmd */
/******************************************************************************************************/
uc8				AT_CGATT_0[] 			= "AT+CGATT=0\r";			// Mode attch the gprs net
uc8				AT_CGATT_1[] 			= "AT+CGATT=1\r";			// Mode attch the gprs net

uc8				AT_CIPMUX_1[]			= "AT+CIPMUX=1\r";		// Ê¹ÄÜ¶àÂ·Á¬½ÓÄ£Ê½£¨0~5Í¨µÀ,×î´ó6Â·Á¬½Ó,¶àÂ·Ä£Ê½½ûÖ¹Í¸´«Ä£Ê½£¬
																									// ÇÒÖ»ÔÊÐí×î¶à1¸ö·þÎñÆ÷¶Ë£¬5¸ö¿Í»§¶Ë¡£¶àÂ·Ä£Ê½Ê¹ÄÜºóÍ¸´«ÉèÖÃÖ¸ÁîCIPMODEÖ¸ÁîÏìÓ¦ÎªERROR£©
uc8				AT_CIPMUX_0[]			= "AT+CIPMUX=0\r";		// ½ûÖ¹¶àÂ·Á¬½ÓÄ£Ê½

uc8				AT_CIPMODE_0[]		= "AT+CIPMODE=1\r";		// ÅäÖÃÍøÂçÊÕ·¢Í¸´«Ä£Ê½(×¢Òâ£ºÈç¹ûÊ¹ÄÜ¶àÁ¬½ÓÄ£Ê½Ôò¸ÃÖ¸ÁîÈÎºÎÅäÖÃ¶¼·µ»Ø"ERROR")
uc8				AT_CIPMODE_1[]		= "AT+CIPMODE=0\r";		// ÅäÖÃÍøÂçÊÕ·¢ÃüÁîÄ£Ê½(Èç¹ûÊ¹ÄÜ¶àÁ¬½ÓÄ£Ê½²»ÄÜÊ¹ÓÃ¸ÃÖ¸Áî)

uc8				AT_CSTT_H[]				=	"AT+CSTT=\"";				// ÅäÖÃAPN,USER,PASS
uc8				AT_CIICR[]				=	"AT+CIICR\r";				// ½¨Á¢ÎÞÏßÁ´Â·£¨GPRS£©
uc8				AT_CIFSR[]				= "AT+CIFSR\r";				// »ñµÃ±¾µØIPµØÖ·
		
/******************************************************************************************************/
/* TCP Init And Connect cmd */
/******************************************************************************************************/

uc8				AT_CIPRXGET_1[]		=	"AT+CIPRXGET=1\r";	// ÉèÖÃÍøÂçÊý¾Ý¶ÁÈ¡·½Ê½ÎªÊÖ¶¯·½Ê½,ÓÐÊý¾ÝÓÃ£¨AT+CIPRXGET=2,id,1000£©¶ÁÈ¡
uc8				AT_CIPHEAD_1[]		=	"AT+CIPHEAD=1\r";		// ÉèÖÃÊÕµ½Êý¾ÝÊý¾ÝÍ·¸ñÊ½(+CIPRXGET: )
uc8				AT_CIPSTART_H[]		=	"AT+CIPSTART=";			// ½¨Á¢TCPÁ¬½Ó£¨¸ñÊ½£ºAT+CIPSTART=0,"TCP","218.249.201.35","13000"£©
uc8				AT_CIPCLOSE_H[]		=	"AT+CIPCLOSE=";			// ¹Ø±ÕTCPÁ¬½Ó
uc8				AT_CIPSHUT[]			=	"AT+CIPSHUT\r";			// ¹Ø±Õµ±Ç°GPRSÉÏÏÂÎÄ»·¾³
uc8				AT_CIPSEND_H[]		=	"AT+CIPSEND=";			// ·¢ËÍÊý¾Ý£¨¸ñÊ½£ºAT+CIPSEND=1,8£©
uc8				AT_CIPRXGET_H[]		=	"AT+CIPRXGET=2,";		// ÊÖ¶¯»ñÈ¡½ÓÊÕµ½µÄÍøÂçÊý¾Ý£¨¸ñÊ½£ºAT+CIPRXGET=2,1,1000£©
uc8				AT_CIPRXGET4_H[]	=	"AT+CIPRXGET=4,";		// ÊÖ¶¯²éÑ¯½ÓÊÕµ½µÄÊý¾Ý³¤¶È
uc8				AT_CIPSENDQ[]			=	"AT+CIPSEND?\r";		// ÓÃÓÚ²éÑ¯µ±Ç°¿ÉÓÃÓÚ·¢ËÍµÄÊý¾Ý³¤¶È


/******************************************************************************************************/
/* Call and SMS cmd */
/******************************************************************************************************/

uc8				AT_ATD_H[]	 			= "ATD";							// Call dail
uc8				AT_ATA[]		 			= "ATA\r";						// Call ack
uc8				AT_ATH[]		 			= "ATH\r";						// Call hung up

uc8				AT_CMGR_H[] 			= "AT+CMGR=";					// Read SMS
uc8				AT_CMGD_H[]				= "AT+CMGD=";					// Delete SMS
uc8				AT_CMGF_0[]				=	"AT+CMGF=0\r";			// Set PDU mode
uc8				AT_CMGF_1[]				=	"AT+CMGF=1\r";			// Set TEXT mode
uc8				AT_CSCA_H[] 			= "AT+CSCA=\"";				// Set SMS center code
uc8				AT_QCSCA[]				=	"AT+CSCA?\r\n";			// Queue SMS center code
uc8				AT_CMGS_H[] 			= "AT+CMGS=";					// Send SMS

uc8				AT_CPBS_ON[] 			= "AT+CPBS=\"ON\"\r";	// Ê¹ÄÜ¿ÉÒÔ´ÓSIM¿¨µç»°±¡ÖÐ¶ÁÈ¡±¾»úºÅÂë
uc8				AT_CPBW_H[] 			= "AT+CPBW=1,\"";			// Ð´Èë±¾»úºÅÂë(Ð´Èëºó¿ÉÊ¹ÓÃCNUM¶ÁÈ¡±¾»úºÅÂë)		

uc8				PDUfixP[]					=	"1100";							// PDUÊÖ»úºÅ¹Ì¶¨×Ö½Ú	
uc8				PDUfixD[]					=	"000800";						// PDU¶ÌÐÅÊý¾Ý¹Ì¶¨×Ö½Ú
	
uc8				PDUinter[]				=	"91";								// ¹ú¼Ê»¯±êÖ¾
uc8				PDUbphone[]				=	"91";								// ¸øÊÖ»ú·¢ËÍ´úÂë
uc8				PDUsphone[]				=	"81";								// ¸øÐ¡ÁéÍ¨·¢ËÍ´úÂë



/******************************************************************************************************/
/* BT Init And Connect cmd */
/******************************************************************************************************/

// Ä£¿é×÷Îª·þÎñÆ÷¶ËÊ±£¬¿Í»§¶ËÁ¬½Ó³É¹¦µÚÒ»°üÊý¾Ý±ØÐëÊÇ¡°SIMCOMSPPFORAPP¡±ÓÃÀ´Ê¹Ä£¿éÇÐ»»µ½APPÄ£Ê½
uc8				AT_BTPOWER_1[]		=	"AT+BTPOWER=1\r";		// ´ò¿ªÀ¶ÑÀÄ£¿éµçÔ´
uc8				AT_BTPOWER_0[]		=	"AT+BTPOWER=0\r";		// ¹Ø±ÕÀ¶ÑÀÄ£¿éµçÔ´
uc8				AT_BTSTATUS[]			=	"AT+BTSTATUS?\r";		// ²éÑ¯À¶ÑÀ×´Ì¬ÐÅÏ¢
uc8				AT_BTHOST_H[]			=	"AT+BTHOST=";				// ÉèÖÃÖ÷»úÃû
uc8				AT_BTUNPAIR[]			=	"AT+BTUNPAIR=0\r";	// É¾³ýËùÓÐÅäÖÃÐÅÏ¢
uc8				AT_BTVIS_1[]			=	"AT+BTVIS=1\r";			// À¶ÑÀ¿É¼ûÊ¹ÄÜ
uc8				AT_BTVIS_0[]			=	"AT+BTVIS=0\r";			// À¶ÑÀ¿É¼û½ûÖ¹
uc8				AT_BTPAIR_1[]			=	"AT+BTPAIR=1,1\r";	// ÏìÓ¦ÆäËüÉè±¸Êý×ÖÆ¥Åä·½Ê½
uc8				AT_BTPAIR_2_H[]		=	"AT+BTPAIR=2,";			// ÏìÓ¦ÆäËüÉè±¸PasskeyÆ¥Åä·½Ê½
uc8				AT_BTACPT_1[]			=	"AT+BTACPT=1\r";		// ½ÓÊÕÆäËüÉè±¸µÄÁ¬½ÓÇëÇó
uc8				AT_BTACPT_0[]			=	"AT+BTACPT=0\r";		// ¾Ü¾øÆäËüÉè±¸µÄÁ¬½ÓÇëÇó
uc8				AT_BTSPPGET_0[]		= "AT+BTSPPGET=0\r";	// ½ÓÊÕÊý¾ÝÄ£Ê½-×Ô¶¯
uc8				AT_BTSPPGET_1[]		= "AT+BTSPPGET=1\r";	// ½ÓÊÕÊý¾ÝÄ£Ê½-ÊÖ¶¯
uc8				AT_BTSPPGET_2[]		=	"AT+BTSPPGET=2\r";	// ÊÖ¶¯Ä£Ê½ÏÂ½ÓÊÕÊÕµ½µÄÊý¾Ý³¤¶È
uc8				AT_BTSPPGET_3_H[] = "AT+BTSPPGET=3,";		// ÊÖ¶¯Ä£Ê½ÏÂ»ñÈ¡½ÓÊÕµ½µÄÊý¾Ý£¨¸ñÊ½£ºAT+BTSPPGET=3,17 17:Êý¾Ý³¤¶È£©
uc8				AT_BTSPPCFG[]			=	"AT+BTSPPCFG?\r";		// »ñÈ¡µ±Ç°ÊÕ·¢Êý¾Ý×´Ì¬£¨¸ñÊ½£º+BTSPPCFG: S,1,0 S:·þÎñÆ÷,C:¿Í»§¶Ë;1:Á¬½ÓID;0:ATÄ£Ê½,1:APPÄ£Ê½£©
uc8				AT_BTSPPSEND_H[]	= "AT+BTSPPSEND=";		// °´³¤¶È·¢ËÍÊý¾Ý


/******************************************************************************************************/
/* FS Init And APP cmd */
/******************************************************************************************************/

uc8				AT_FSDRIVE_0[]		=	"AT+FSDRIVE=0\r";		// »ñÈ¡±¾µØ´ÅÅÌÅÌ·û£¨+FSDRIVE: C£©
uc8				AT_FSDRIVE_1[]		= "AT+FSDRIVE=1\r";		// »ñÈ¡SC¿¨´ÅÅÌÅÌ·û£¨+FSDRIVE: D~G£©
uc8				AT_FSLS_H[]				=	"AT+FSLS=";					// »ñÈ¡±¾µØÎÄ¼þÐÅÏ¢
uc8				AT_FSLS_C[]				=	"AT+FSLS=C:\\r";		// »ñÈ¡±¾µØCÅÌÏÂÎÄ¼þ¼ÐÐÅÏ¢
uc8				AT_FSLS_USER[]		=	"AT+FSLS=C:\\User\\r";		// »ñÈ¡±¾µØUserÏÂÎÄ¼þ¼ÐÐÅÏ¢
uc8				AT_FSMKDIR_H[]		=	"AT+FSMKDIR=";			// ÔÚÄ¿Â¼ÏÂ´´½¨ÎÄ¼þ¼Ð£¨¸ñÊ½£ºAT+FSMKDIR=C:\User\Log£©
uc8				AT_FSCREATE_H[]		=	"AT+FSCREATE=";			// ÔÚÄ¿Â¼ÏÂ´´½¨ÎÄ¼þ£¨¸ñÊ½£ºAT+FSCREATE=C:\User\Log\Log.txt£©
uc8				AT_FSFLSIZE_H[]		=	"AT+FSFLSIZE=";			// ²éÑ¯ÎÄ¼þ´óÐ¡£¨¸ñÊ½£ºAT+FSFLSIZE=C:\User\Log\Log.txt£©
uc8				AT_FSWRITE_H[]		=	"AT+FSWRITE=";			// Ð´ÎÄ¼þ£¨¸ñÊ½£ºAT+FSWRITE=C:\User\Log\Log.txt,0,23,10  
                                                  // 0:ÔÚÎÄ¼þ¿ªÍ·²åÈë£¬½«¸²¸ÇºóÃæµÄÊý¾Ý 1£ºÔÚÎÄ¼þ½áÎ²Ð´Èë;23:³¤¶È;10:Ð´Èë³¬Ê±Ãë£©
uc8				AT_FSREAD_H[]			= "AT+FSREAD=";				// ¶ÁÎÄ¼þ£¨¸ñÊ½£ºAT+FSREAD=C:\User\Log\Log.txt,0,100,0                                                  
																									// 0:´ÓÎÄ¼þÍ·¿ª¶Á 1:´Óposi¶Á; 100:³¤¶È; 0:¶ÁÎÄ¼þÆðÊ¼µØÖ·position£©
uc8				AT_FSMEM[]				=	"AT+FSMEM\r";				// ²éÑ¯¸÷¸öÅÌ·û¿Õ¼äÕ¼ÓÃÇé¿ö£¨¸ñÊ½£º+FSMEM: C:76288byte£©	
uc8				AT_FSRENAME_H[]		=	"AT+FSRENAME=";			// ÖØÃûÁËÎÄ¼þ£¨¸ñÊ½£ºAT+FSRENAME=C:\User\Log\Log.txt,C:\User\Log\Log1.txt£©		
uc8				AT_FSDEL_H[]			=	"AT+FSDEL=";				// É¾³ýÎÄ¼þ¼Ð£¨¸ñÊ½£ºAT_FSDEL=C:\User\Log\Log.txt£©


/******************************************************************************************************/
/* TTS Init And APP cmd */
/******************************************************************************************************/

uc8				AT_CTTS_0[]				=	"AT+CTTS=0\r";			// Í£Ö¹µ±Ç°ÓïÒô²¥·Å
uc8				AT_CTTS_1_H[]			=	"AT+CTTS=1,";				// UCS2·½Ê½²¥·Åµ±Ç°ÓïÒô
uc8				AT_CTTS_2_H[]			= "AT+CTTS=2,";				// ASCII·½Ê½²¥·Åµ±Ç°ÓïÒô£¨¸ñÊ½£ºAT+CTTS=1,»¶Ó­Ê¹ÓÃ£©
uc8				AT_CTTSPARAM_H[]	=	"AT+CTTSPARAM=";		// ÉèÖÃTTS²ÎÊý£¨¸ñÊ½£ºAT+CTTSPARAM=<Vol>,<Mode>,<Pitch>,<Spped>,<Channel>£©
uc8				AT_CTTSRING_0[]		= "AT+CTTSRING=0\r";	// À´µçÊ±½ûÖ¹²¥·ÅÓïÒô
uc8				AT_CTTSRING_1[]		= "AT+CTTSRING=1\r";	// À´µçÊ±¿ÉÒÔ²¥·ÅÓïÒô

/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
extern		OS_EVENT					*GSMMutexSem;					// µ×²ã»¥³âÐÅºÅÁ¿
extern		OS_EVENT					*GSMSmsSem;						// ÊÕµ½¶ÌÐÅÐÅºÅÁ¿
extern		OS_EVENT					*GSMGprsDataSem;			// GPRS½ÓÊÕÊý¾ÝÐÅºÅÁ¿
extern		OS_EVENT					*GSMBtDataMbox;				// À¶ÑÀ½ÓÊÕÊý¾ÝÓÊÏä
extern		OS_EVENT					*GSMBtAppMbox;				// À¶ÑÀ»ñÈ¡ÒµÎñ×´Ì¬ÓÊÏä

static		vu8								GSMMutexFlag	=1;			// Èç¹ûÔËÐÐÔÚÎÞOS×´Ì¬ÏÂÓÉ¸Ã±äÁ¿ÊµÏÖ»¥³â²Ù×÷
static		vu32							ringCheckConuter =0;	// ringÊÂ¼þ³¬Ê±¶¨Ê±Æ÷
static		vu8								ringWUpFlag	=0;				// ¸Ä±êÖ¾ÓÉË¯ÃßÈÎÎñÖÃ¡°1¡°ÓÉRingÖÐ¶ÏÉèÖÃÎª¡±2¡°±íÃ÷Éè±¸ÓÉRing»½ÐÑ
static		vu8								tcpReadMuxtex	=0;			// ÎªÁËÊµÏÖÍøÂç¶ÁÈ¡ÓëÄ£¿é×Ô¶¯ÉÏ´«µÄURC×Ö·û´®ÊµÏÖ»¥³
																									//(Èç¹û¶ÁÈ¡µÃÎªÉý¼¶³ÌÐòÎÄ¼þ£¬¿ÉÄÜ»á³öÏÖÒ»Ð©¹Ì»¯ÔÚ³ÌÐòÖÐµÄURC£¬
																									// Ê¹Ä£¿é´íÎóµÄÒÔÎªÊÇµ±Ç°Ä£¿éÊÕµ½µÄURC£¬µ¼ÖÂÉÏ±¨×´Ì¬µÄ´íÎó)

BTMbox_Typedef							s_MboxBT;							// À¶ÑÀ½ÓÊÕÓÊÏä±äÁ¿£¬ÓÃÓÚ´«ÊäÀ¶ÑÀÊý¾Ý¼°ÒµÎñ

static		vu8								ringCheckStep=0;			// Ring¼ì²â½×¶Î±äÁ¿
static		vu32							ringCheckCounter=0;		// Ring¼ì²â¶¨Ê±Æ÷



/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static		void			gsmDelayMS								(u16	nms);

static		void			GSMGotoSleep		 					(void);
static		void			GSMStartWork							(void);

/* Interrupt server */
static		void  		GSMRS232_RxHandler 				(u8 rx_data);
static		void			GSMRS232_TxHandler 				(void);

static		void			Start2doSendAT						(void);
static		void	 		DoSendProcess 						(void);

static		u8				CheckResultCode						(u8	checkMode,	u32 TimeOut);
static 		u16				PDUMakeUpData							(u8	 mode,	u8	*pSmsCenter,	u8 *pPhone,	u8	*pData,		u16	 DataLen, u8	*pDst);
static		u16 			GSMInvertNumbers					(u8* pSrc, u8* pDst, u16 nSrcLength);
static		void			GSMRingPinInit 						(void);

static		u16				ChangeUSC22ASCII 					(u8 *pUSC2,		u16 USC2len,	u8 *pASCII);
static		u16				ChangeASCII2USC2 					(u8 *pASCII,	u16	ASCIIlen,	u8 *pUSC2);

static		void			GSMFeedWDGProcess					(void);		// ³õÊ¼»¯¹ý³ÌÖÐÐèÒªµ÷ÓÃµÄÎ¹¹·²Ù×÷


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*********************************************************************************************************
*                                         GSM FEED WDG
*********************************************************************************************************
*********************************************************************************************************
*/
static	void	GSMFeedWDGProcess (void)
{
		#if(OS_SYSWDG_EN > 0)
		IWDG_ReloadCounter();		// Î¹¹·
		#endif
}


/*
*********************************************************************************************************
*                            					GSMÄ£¿é¹¤¾ßº¯Êý
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : ChangeUSC22ASCII
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	u16	ChangeUSC22ASCII (u8 *pUSC2,	u16 USC2len,	u8 *pASCII)
{
		u16	i=0,j=0;
		u32 val=0;
		u8 tmpBuf[5]="";

		if((USC2len % 4) != 0)
				return	0xffff;
		for(i=0,j=0;	i<USC2len;	i +=4,j++)
		{
				val =0;
				memset(tmpBuf, '\0', sizeof(tmpBuf));
				memcpy(tmpBuf,	(pUSC2 + i),	4);
				sscanf((const char *)&tmpBuf,	"%x",	&val);
				*(pASCII + j) = (u8)(val & 0x000000ff);	// È¡×îµÍ×Ö½Ú
		}
		*(pASCII + j) = '\0';
		return	j;
}
/*
*********************************************************************************************************
* Function Name  : ChangeASCII2USC2
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	u16	ChangeASCII2USC2 (u8 *pASCII,	u16	ASCIIlen,	u8 *pUSC2)
{
		u16	i=0;
		u8 tmpBuf[5]="0000",	valBuf[5]="";
		for(i=0;	i<ASCIIlen;	i++)
		{
				memset(valBuf,	'\0',		sizeof(valBuf));
				memset(tmpBuf,	'0',		strlen((const char *)tmpBuf));
				sprintf((char *)valBuf,	"%02X",	*(pASCII + i));
				memcpy(&tmpBuf[2],	valBuf,	2);
				memcpy((pUSC2 + i * 4),	tmpBuf,	4);
		}
		*(pUSC2 + i * 4) = '\0';
		return	(i * 4);
}

/*
*********************************************************************************************************
*                            					GSMÄ£¿éI/O»ù±¾²Ù×÷º¯Êý(Ìá¹©¿ª/¹Ø»ú/¸´Î»µÈ)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : gsmDelayMS
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	gsmDelayMS	(u16	nms)
{
		if(OSRunning > 0)
				OSTimeDly(nms);
		else
				SoftDelayMS(nms);
}
/*
*********************************************************************************************************
* Function Name  : BSP_GSM_EXIT_SLEEPMODE
* Description    :      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_EXIT_SLEEPMODE	(void)
{
		// ÍË³öË¯ÃßÄ£Ê½
		IO_GSM_DTR_DIS();
		gsmDelayMS(100);
}
/*
*********************************************************************************************************
* Function Name  : BSP_GSM_MODULE_DOWN
* Description    : power down the GSM module power supply
* Input          : newsta = ON or OFF 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSMMODULE_POWDOWN	(void)
{
		IO_GSM_POW_DIS();
		IO_GSM_POW_OFF();
}
/*
*********************************************************************************************************
* Function Name  : BSP_GSM_POWER_CON
* Description    : power on or down the GSM module 
* Input          : newsta = ON or OFF 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_POWER_CON	(void)
{
		IO_GSM_POW_ON();
		/* Delay 2S */
		gsmDelayMS(2000);
		IO_GSM_POW_OFF();
		/* Delay 1S */
		gsmDelayMS(1000);
}
/*
*********************************************************************************************************
* Function Name  : BSP_GSM_SOFT_RESET
* Description    : reset the gsm module    
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void 	BSP_GSM_SOFT_RESET	(void)
{
		IO_GSM_RES_ON();
		// Delay 2S 
		gsmDelayMS(2000);
		IO_GSM_RES_OFF();
		// Delay 1S 
		gsmDelayMS(1000);
}

/*
*********************************************************************************************************
* Function Name  : BSP_GSM_POW_RESET
* Description    : Hard reset the gsm module				      
* Input          : Éè±¸¸´Î»Ê±µ÷ÓÃ
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_POW_RESET	(u32	dlayTime)
{	
		//IO_GSM_RES_ON();		// RESET ¹Ü½ÅÓÐ¿ÉÄÜ»áÂ©µçµ¼ÖÂ·Åµç²»¸É¾
													//(ÕâÀï½ûÖ¹RST¹Ü½Å¿ØÖÆ,Èç¹û¿ØÖÆÔò»áÊä³ö³õÊ¼»¯×Ö·û´®µ¼ÖÂ³õÊ¼»¯³ÌÐò½ÓÊÕ³õÊ¼»¯×Ö·û´®½ÓÊÕÊ§°Ü)		
		IO_GSM_POW_OFF();
		IO_GSM_POW_DIS();		
    if (dlayTime <= 6000u)
    {
        dlayTime = 6000u;   //·ÀÖ¹¶ÏµçÊ±¼ä¹ý¶ÌGSMÄ£¿éÎÞ·¨¸´Î» 20160104
    }
    else
    {
    }
		gsmDelayMS(dlayTime);	
		IO_GSM_RES_OFF();		
		IO_GSM_POW_EN();
		gsmDelayMS(1000);
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMHardWarePowShut() 
* Description    : GSMÄ£¿éÓ²¼þ¶Ïµç´¦Àí        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSMHardWarePowShut(void)
{
		memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	// Çå´íÎó±äÁ¿		
		s_GSMcomm.SetupPhase 		= PowerOff;
		s_GSMcomm.GSMComStatus	=	GSM_POWOFF;
		BSP_GSM_ReleaseFlag(DEF_ALL_FLAG);
		IO_GSM_POW_DIS();															// Ä£¿é¶Ïµç 
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMHardWareReset() 
* Description    : µ±Èí¼þ¸´Î»ÎÞÐ§Ê±Ö´ÐÐÓ²¼þ¸´Î»        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMHardWareReset(u32 dlayTime)
{
		memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	// Çå´íÎó±äÁ¿		
		s_GSMcomm.SetupPhase 		= PowerOff;
		s_GSMcomm.GSMComStatus	=	GSM_POWOFF;
		BSP_GSM_ReleaseFlag(DEF_ALL_FLAG);
		BSP_GSM_POW_RESET(dlayTime);
		return	0;
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ReleaseFlag()	
* Description    :  ÓÃÓÚÉÏÎ»»úÇå¿ÕÄ£¿é×´Ì¬         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_ReleaseFlag (u8	flagType)
{
		if(flagType == DEF_ALL_FLAG)
				s_GSMcomm.CommandFlag &= ~AllF;	
		else if(flagType == DEF_RING_FLAG)
				s_GSMcomm.CommandFlag &= ~RingComeF;	
		else if(flagType == DEF_RINGCHECK_FLAG)	
				s_GSMcomm.CommandFlag &= ~RingCheckF;
		else if(flagType == DEF_CALLDOING_FLAG)	
				s_GSMcomm.CommandFlag &= ~CallDoingF;
		else if(flagType == DEF_SMSDOING_FLAG)	
				s_GSMcomm.CommandFlag &= ~SmsDoingF;
		else if(flagType == DEF_GDATA_FLAG)	
				s_GSMcomm.CommandFlag &= ~GprsDoingF;				
		else if(flagType == DEF_TTSPLAY_FLAG)	
				s_GSMcomm.CommandFlag &= ~TTSPlayF;
		else if(flagType == DEF_BTDING_FLAG)	
				s_GSMcomm.CommandFlag &= ~BTDoingF;
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_SetFlag()	
* Description    :  ÓÃÓÚÉÏÎ»»úÉèÖÃÄ£¿é×´Ì¬          
* Input          : 
* Output         : 	 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_SetFlag (u8	flagType)
{
		if(flagType == DEF_ALL_FLAG)
		{
				;		// ²»×ö´¦Àí
		}
		else if(flagType == DEF_RING_FLAG)
				s_GSMcomm.CommandFlag |= RingComeF;	
		else if(flagType == DEF_RINGCHECK_FLAG)	
				s_GSMcomm.CommandFlag |= RingCheckF;
		else if(flagType == DEF_CALLDOING_FLAG)	
				s_GSMcomm.CommandFlag |= CallDoingF;
		else if(flagType == DEF_SMSDOING_FLAG)	
				s_GSMcomm.CommandFlag |= SmsDoingF;
		else if(flagType == DEF_GDATA_FLAG)	
				s_GSMcomm.CommandFlag |= GprsDoingF;
		else if(flagType == DEF_TTSPLAY_FLAG)	
				s_GSMcomm.CommandFlag |= TTSPlayF;
		else if(flagType == DEF_BTDING_FLAG)	
				s_GSMcomm.CommandFlag |= BTDoingF;
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_GetFlag()	
* Description    :  ÓÃÓÚÉÏÎ»»úÅÐ¶ÏÄ£¿é×´Ì¬       
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
u8	BSP_GSM_GetFlag (u8	flagType)
{
		if(flagType	==	DEF_RING_FLAG)
		{
				if(s_GSMcomm.CommandFlag & RingComeF)
						return	1;
		}
		else	if(flagType	==	DEF_RINGCHECK_FLAG)
		{
				if(s_GSMcomm.CommandFlag & RingCheckF)
						return	1;
		}
		else	if(flagType	==	DEF_CALLDOING_FLAG)
		{
				if(s_GSMcomm.CommandFlag & CallDoingF)
						return	1;	
		}
		else	if(flagType	==	DEF_SMSDOING_FLAG)
		{
				if(s_GSMcomm.CommandFlag & SmsDoingF)
						return	1;	
		}
		else	if(flagType	==	DEF_GDATA_FLAG)
		{
				if(s_GSMcomm.CommandFlag & GprsDoingF)
						return	1;	
		}
		else	if(flagType	==	DEF_TTSPLAY_FLAG)
		{
				if(s_GSMcomm.CommandFlag & TTSPlayF)
						return	1;	
		}
		else	if(flagType	==	DEF_BTDING_FLAG)
		{
				if(s_GSMcomm.CommandFlag & BTDoingF)
						return	1;	
		}
		return	0;		
}
/*
*********************************************************************************************************
*                            					GSM×Ö·û²Ù×÷¹¤¾ßº¯Êý
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : GSMInvertNumbers
* Description    : Õý³£Ë³ÐòµÄ×Ö·û´®×ª»»ÎªÁ½Á½µßµ¹µÄ×Ö·û´®£¬Èô³¤¶ÈÎªÆæÊý£¬²¹'F'´Õ³ÉÅ¼Êý 
*									 Èç£º"8613851872468" --> "683158812764F8"
*									 ÊäÈë: pSrc - Ô´×Ö·û´®Ö¸Õë
*									       nSrcLength - Ô´×Ö·û´®³¤¶È
*									 Êä³ö: pDst - Ä¿±ê×Ö·û´®Ö¸Õë
*									 ·µ»Ø: Ä¿±ê×Ö·û´®³¤¶È
* Input          :  
* Output         : None
* Return         : 
*********************************************************************************************************
*/
static	u16 GSMInvertNumbers (u8* pSrc, u8* pDst, u16 nSrcLength)
{
		u16 nDstLength,i;   			// Ä¿±ê×Ö·û´®³¤¶È
		u8  ch;    								// ÓÃÓÚÁÙÊ±±£´æÒ»¸ö×Ö·û
	  u8* ptSrc = pSrc;
		u8* ptDst = pDst;
	
		nDstLength = nSrcLength;
		for(i=0; i<nSrcLength;	i+=2)
		{
				ch = *ptSrc++;   			// ±£´æÏÈ³öÏÖµÄ×Ö·û
				*ptDst++ = *ptSrc++; 	// ¸´ÖÆºó³öÏÖµÄ×Ö·û
				*ptDst++ = ch;   			// ¸´ÖÆÏÈ³öÏÖµÄÖû
		}
		if(nSrcLength & 1)
		{
				*(ptDst-2) = 'F'; 		// ²¹'F'
				nDstLength++;  	 			// Ä¿±ê´®³¤¶ÈÓ1
		}
		*ptDst = '\0';
		return nDstLength;
}
/*
*********************************************************************************************************
* Function Name  : PDUMakeUpData
* Description    : ½«¶ÌÐÅÒÔPDUÄ£Ê½´ò°ü²¢·µ»Ø³¤¶ÈÊýÖµ 
*									 ÊäÈë: mode 			- DEF_COM_PHONE / DEF_PHS_PHONE
*												 pSmsCenter - ¶ÌÐÅÖÐÐÄºÅÂëÖ¸Õë
*												 pPhone			-	Ä¿µÄÊÖ»úºÅÂëÖ¸Õë
*												 pData			-	ÐèÒª·¢ËÍµÄÊý¾ÝÖ¸Õë
*												 pDst				-	Ä¿µÄ´æ´¢ÇøÖ¸Õë			
* Return         : ´ò°üºóÐÅÏ¢×Ü³¤¶È(¸Ä³¤¶È²»°üÀ¨¶ÌÐÅÖÐÐÄµØÖ·²¿·Ö×Ö½Ú) ²¢¸½ÉÏ0x1a 
*********************************************************************************************************
*/
static  u16 PDUMakeUpData (u8	mode,	u8	*pSmsCenter,	u8 *pPhone,	u8	*pData,	u16	 DataLen, u8	*pDst)
{
		u8	tmpBuf[50]="",tmpDuf[50]="",tmpPhone[50]="";
		u16 j=0,i=0,len=0,tmplen=0;
		
		memset(tmpBuf,	'\0',	sizeof(tmpBuf));
		memset(tmpDuf,	'\0',	sizeof(tmpDuf));
		memset(tmpPhone,	'\0',	sizeof(tmpPhone));		
		
		// ¶ÌÐÅÖÐÐÄ²¿·Ö´ò°ü
		if(strlen((const char *)pSmsCenter) == 0)
		{
				*(pDst+j) = '0';	j++;			   		// copy "00"
				*(pDst+j) = '0';	j++;
		}
		else
		{
				strcpy((char	*)tmpPhone,	(char	*)pSmsCenter);				
				tmplen = GSMInvertNumbers(tmpPhone,	tmpBuf,	 strlen((const char *)tmpPhone));	
				tmplen = (strlen((const char *)PDUinter)+ tmplen) /2;
				sprintf((char *)tmpDuf,"%02X",	tmplen);
				for(i=0;	tmpDuf[i]!='\0';	i++,j++)
				{
						*(pDst+j) = tmpDuf[i];			 	// copy sms center len
				}
				for(i=0;	PDUinter[i]!='\0';	i++,j++)
				{
						*(pDst+j) = PDUinter[i];		 	// copy "91"
				}
				for(i=0;	tmpBuf[i]!='\0';	i++,j++)
				{
						*(pDst+j)	=	tmpBuf[i];		   	// copy smscenter number
				}
		}
		// ÊÖ»úºÅÂë²¿·Ö´ò°ü
		for(i=0;	PDUfixP[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j)	=	PDUfixP[i];				 		// copy "1100"
		}
		strcpy((char	*)tmpPhone,	(char	*)pPhone);
		GSMInvertNumbers(tmpPhone,	tmpBuf,	 strlen((const char *)tmpPhone));	
		tmplen = strlen((const char *)tmpPhone);
		sprintf((char *)tmpDuf,	"%02X",	tmplen);
		for(i=0;	tmpDuf[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j) = tmpDuf[i];				 		// copy phone number len
		}
		if(mode == DEF_COM_PHONE)				 
		{
				//	ÆÕÍ¨ÊÖ»ú
				for(i=0;	PDUbphone[i]!='\0';	i++,j++,len++)
				{
						*(pDst+j) = PDUbphone[i];		 	// copy "91"
				}
		}
		else
		{
				//	Ð¡ÁéÍ¨
				for(i=0;	PDUsphone[i]!='\0';	i++,j++,len++)
				{
						*(pDst+j) = PDUsphone[i];		 	// copy "81"
				}
		}
		for(i=0;	tmpBuf[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j)	=	tmpBuf[i];				 		// copy "phone number"
		}
		for(i=0;	PDUfixD[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j)	=	PDUfixD[i];				 		// copy "000800"
		}
		// ÊÖ»úÐÅÏ¢²¿·Ö´ò°ü
		tmplen = strlen((const char *)pData)/2;
		sprintf((char *)tmpDuf,	"%02X",	tmplen);
		for(i=0;	tmpDuf[i]!='\0';	i++,j++,len++)
		{
				*(pDst+j)	=	tmpDuf[i];				 		// copy data len
		}
		for(i=0;	i<DataLen;	i++,j++,len++)
		{
				*(pDst+j)	=	pData[i];				   		// copy data
		}
		*(pDst+j) = 0x1a;								 			// sms data end
		*(pDst+j+1) = '\0';
	
		return len/2;
}
/*
*********************************************************************************************************
*                                           CheckResultCode()
*
* Description : ÓÃÓÚÆ¥ÅäÖ÷¶¯ÇëÇóµÄATÖ¸Áî·µ»ØÂëÈýÖÖÄ£Ê½£ºDEF_NULL_MODE	DEF_ONLYOK_MODE	DEF_SPECIAL_MODE DEF_COMMON_MODE
*
* Argument(s) : none.			 
*
* Return(s)   : none.	
*
* Caller(s)   : 
*
* Note(s)     : 
*********************************************************************************************************
*/
static	u8 CheckResultCode(u8	checkMode,	u32 TimeOut)
{
		if(checkMode ==  DEF_NULL_MODE)
		{
        ;											 				// µÈ´ý³¬Ê±ÍË³ö(ÓÃÓÚÖ¸ÁîÍ¸´«)
		}
		if((checkMode	==	DEF_ONLYOK_MODE) || (checkMode	==	DEF_COMMON_MODE))
		{    
				if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\nOK",s_GSMcomm.ps_commu->RCNT,4)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = CommandRecOk;
						s_GSMcomm.Timer = 0;
		        return CommandRecOk;
		    }
				else
				{
		        // µÈ´ý³¬Ê±
		        ;													// ÓÃÓÚÐèÒªÈ¡·µ»ØÂë²¢½âÎöµÄATÈçAT+CSQ	AT+CLCCµÈ
		    }
				
		}
		if((checkMode	==	DEF_SPECIAL_MODE) || (checkMode	==	DEF_COMMON_MODE))
		{
				// ÌØÊâÄ£Ê½¼ì²âÏîÄ¿
				if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\nERROR\r\n",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = CommandRecErr;
						s_GSMcomm.Timer = 0;
		        return CommandRecErr;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\nNO CARRIER\r\n",s_GSMcomm.ps_commu->RCNT,14)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecNoCarrier;
						s_GSMcomm.Timer = 0;
		        return RecNoCarrier;														 
		    }
				/* 
				// ¶ÌÐÅ½ÓÊÕÊôÓÚ×Ô¶¯ÉÏ±¨²»ÔÙ·¢ËÍAT·µ»ØÅäÖÃÀï´¦ÀíÔÚURC×Ô¶¯ÉÏ±¨ÅäÖÃ´¦Àí
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CMTI: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecSms;
						s_GSMcomm.Timer = 0;
		        return RecSms;
		    }			
				*/
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n>",s_GSMcomm.ps_commu->RCNT,3)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCMGSAck;
						s_GSMcomm.Timer = 0;
		        return RecCMGSAck;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CSQ: ",s_GSMcomm.ps_commu->RCNT,8)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCSQcpl;
						s_GSMcomm.Timer = 0;
		        return RecCSQcpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CLCC: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCSCCcpl;
						s_GSMcomm.Timer = 0;
		        return RecCSCCcpl;
		    }		
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CPIN: READY",s_GSMcomm.ps_commu->RCNT,14)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCPINcpl;
						s_GSMcomm.Timer = 0;
		        return RecCPINcpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CPIN: NOT INSERTED",s_GSMcomm.ps_commu->RCNT,21)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCPINcplErr;
						s_GSMcomm.Timer = 0;
		        return RecCPINcplErr;
		    }
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CREG: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCREGcpl;
						s_GSMcomm.Timer = 0;
		        return RecCREGcpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CGREG: ",s_GSMcomm.ps_commu->RCNT,10)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCGREGcpl;
						s_GSMcomm.Timer = 0;
		        return RecCGREGcpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CMGS: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCMGScpl;
						s_GSMcomm.Timer = 0;
		        return RecCMGScpl;
		    }	    
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CMGR: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCMGRAck;
						s_GSMcomm.Timer = 0;
		        return RecCMGRAck;
		    }
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"SEND OK",s_GSMcomm.ps_commu->RCNT,7)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecSENDOKcpl;
						s_GSMcomm.Timer = 0;
		        return RecSENDOKcpl;
		    }
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"SHUT OK",s_GSMcomm.ps_commu->RCNT,7)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecShutCpl;
						s_GSMcomm.Timer = 0;
		        return RecShutCpl;
		    }
		    else if((StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"CONNECT OK",s_GSMcomm.ps_commu->RCNT,10)!=0xffff) ||
		    				(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"ALREADY CONNECT",s_GSMcomm.ps_commu->RCNT,15)!=0xffff))
				{
		        s_GSMcomm.CmdPhase = RecConnectCpl;
						s_GSMcomm.Timer = 0;
		        return RecConnectCpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"CONNECT FAIL",s_GSMcomm.ps_commu->RCNT,12)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecConnectFail;
						s_GSMcomm.Timer = 0;
		        return RecConnectFail;
		    }
		    else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"CLOSE OK",s_GSMcomm.ps_commu->RCNT,8)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCloseCpl;
						s_GSMcomm.Timer = 0;
		        return RecCloseCpl;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)".",s_GSMcomm.ps_commu->RCNT,1)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCIFSRAck;
						s_GSMcomm.Timer = 0;
		        return RecCIFSRAck;
		    }
				else if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CNUM: ",s_GSMcomm.ps_commu->RCNT,9)!=0xffff)
				{
		        s_GSMcomm.CmdPhase = RecCNUMAck;
						s_GSMcomm.Timer = 0;
		        return RecCNUMAck;
		    }
		}
		
    if(s_GSMcomm.Timer > TimeOut)
		{
	      /* ×îºóÅÐ¶Ï³¬Ê±,ÒÔÃâ³ÌÐòÅÜÍêÒ»Ñ­»·,ÔÙµ½Õâ¾ÍÒÑ¾­³¬Ê±ÁË.µ«Êµ¼ÊÒÑÊÕµ½Ó¦´ð. */
	      s_GSMcomm.CmdPhase = CommandTimeout;
	      s_GSMcomm.Timer = 0;
				//s_GSMcomm.ps_commu->RCNT = 0;
        return CommandTimeout;
    }
		else
		{
        /* wait for timeout */
        return 0;
    }
}
/*
*********************************************************************************************************
*                            							GSMµ×²ãÊÕ/·¢ÖÐ¶Ïº¯Êý(stm32f10x_it.c½Ó¿Ú)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                     BSP_GSMRING_IT_CON()
*
* Description: Ring pin interrupts cfg.
* Argument(s): none.
* Return(s)  : none.
* Note(s)    : 
*********************************************************************************************************
*/
void	BSP_GSMRING_IT_CON (u8	newSta)
{
		EXTI_InitTypeDef EXTI_InitStructure;
		
		// Ring ´¥·¢I/O
		// Clear the Key Button EXTI line pending bit
	  EXTI_ClearITPendingBit(DEF_GSM_RING_EXTI_LINE);		
	  EXTI_InitStructure.EXTI_Line 		= DEF_GSM_RING_EXTI_LINE;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = DEF_GSM_RING_EXTI_TRIGGER;
	  EXTI_InitStructure.EXTI_LineCmd = (FunctionalState)newSta;
	  EXTI_Init(&EXTI_InitStructure);
}

/*
*********************************************************************************************************
*                                     GSMRingPinInit()
*
* Description: Ring pin interrupts.
* Argument(s): none.
* Return(s)  : none.
* Note(s)    : 
*********************************************************************************************************
*/
static	void	GSMRingPinInit (void)
{
	  EXTI_InitTypeDef EXTI_InitStructure;
		
		// Enbale	exti clock 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	  // Connect Key Button EXTI Line to Key Button GPIO Pin 
	  GPIO_EXTILineConfig(DEF_GSM_RING_EXTI_PORTS, DEF_GSM_RING_EXTI_PINS);
	 
		// Clear the Key Button EXTI line pending bit
	  EXTI_ClearITPendingBit(DEF_GSM_RING_EXTI_LINE);
	
		// Configure Key Button EXTI Line to generate an interrupt on falling edge  
		
		// Ring ´¥·¢I/O 
	  EXTI_InitStructure.EXTI_Line 		= DEF_GSM_RING_EXTI_LINE;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = DEF_GSM_RING_EXTI_TRIGGER;
	  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	  EXTI_Init(&EXTI_InitStructure);
}
/*
*********************************************************************************************************
*                                     BSP_GSMRing_ISRHandler()
*
* Description: Ring pin interrupts.	 Èç¹ûringWUpFlagÒÑ±»ÉèÖÃ¡±1¡°ÔòÐÞ¸ÄringWUpFlag=2À´±íÊ¾Éè±¸Ë¯ÃßÓÉRingÊÂ¼þ»½ÐÑ
* Argument(s): none.
* Return(s)  : none.
* Note(s)    : 
*********************************************************************************************************
*/
void  BSP_GSMRing_ISRHandler (void)
{	
		if(ringWUpFlag == 1)
				ringWUpFlag =2;	// ÉèÖÃGSMÓÉRing»½ÐÑ±êÖ¾
		
		// ÉèÖÃRing²úÉú±êÖ¾
		BSP_GSM_SetFlag(DEF_RING_FLAG);	// Í¨ÖªRingÖÐ¶Ï²úÉú
}
/*
*********************************************************************************************************
*                                     BSP_GSMRS232_RxTxISRHandler()
*
* Description: Handle Rx and Tx interrupts.
*
* Argument(s): none.
*
* Return(s)  : none.
*
* Note(s)    : (1) This ISR handler handles the interrupt entrance/exit as expected by
*                  by uC/OS-II v2.85.  If you are using a different RTOS (or no RTOS), then this
*                  procedure may need to be modified or eliminated.  However, the logic in the handler
*                  need not be changed.
*********************************************************************************************************
*/
void  BSP_GSMRS232_RxTxISRHandler (void)
{
		vu8  rx_data;
		USART_TypeDef  *usart;

#if   (DEF_GSM_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_GSM_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_GSM_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_GSM_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_GSM_UART == DEF_UART_5)
		usart = UART5;
#else
		return;
#endif

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
        rx_data = USART_ReceiveData(usart) & 0xFF;              /* Read one byte from the receive data register         */
        GSMRS232_RxHandler(rx_data);
        USART_ClearITPendingBit(usart, USART_IT_RXNE);          /* Clear the GSM_UART Receive interrupt                   */
    }

    else if (USART_GetITStatus(usart, USART_IT_TXE) != RESET) {
        GSMRS232_TxHandler();
        USART_ClearITPendingBit(usart, USART_IT_TXE);           /* Clear the GSM_UART transmit interrupt  */
    }  
		else {
				USART_ReceiveData(usart);
				USART_ClearITPendingBit(usart, USART_IT_RXNE);
		}   
}
/*
*********************************************************************************************************
* Function Name  : 												GSMRS232_RxHandler()
* Description    : Handle a received byte.   
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void  GSMRS232_RxHandler (u8 rx_data)
{
		// ATÖ¸ÁîÊý¾Ý.
		if(ComGSM.RCNT	<	GSM_GPSRSBUF_RXD_SIZE)
		{				
				RBufGPRS[ComGSM.RCNT] = rx_data;  // Êý¾Ý¶Áµ½ÓÃ»§»º³åÇø.
				ComGSM.RCNT++;
		}
}
/*
*********************************************************************************************************
* Function Name  : 												GSMRS232_TxHandler()
* Description    : Handle a byte transmission.     
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void  GSMRS232_TxHandler (void)
{
		vu16 state;
		USART_TypeDef  *usart;

#if   (DEF_GSM_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_GSM_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_GSM_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_GSM_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_GSM_UART == DEF_UART_5)
		usart = UART5;
#else
		return;
#endif
	
		

		state = ComGSM.State;
		
		if((state & MOD) == MOD_S)
		{
		    /* ´¦ÓÚ·¢ËÍÄ£Ê½ */
		    if(ComGSM.SCNT < ComGSM.SLEN)
				{
						USART_SendData(usart, SBufGPRS[ComGSM.SCNT]);
						while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
						ComGSM.SCNT++;
		    }
				else
				{
			      state=(state&(~SS))|SS_O; 	  		// ÖÃ·¢ËÍÍê³É±êÖ¾. 
			      state = state&(~MOD);         		// ÖÃÎª¿ÕÏÐÄ£Ê½. 
			
						ComGSM.State	=	state;		
						GSM_DISABLE_SEND_IT();						// Í£Ö¹GSM·¢ËÍÖÐ.
		    }
		}
}
/*
*********************************************************************************************************
* Function Name  : 												StartSend_GPRS()
* Description    : use to start the send        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	StartSend_GPRS (void)
{  
		u16	i=0;
		//u16 tmpRCNT=0;
		USART_TypeDef  *usart;

#if   (DEF_GSM_UART == DEF_USART_1)
    usart = USART1;
#elif (DEF_GSM_UART == DEF_USART_2)
    usart = USART2;
#elif (DEF_GSM_UART == DEF_USART_3)
    usart = USART3;
#elif	(DEF_GSM_UART == DEF_UART_4)
		usart = UART4;
#elif	(DEF_GSM_UART == DEF_UART_5)
		usart = UART5;
#else
    return;
#endif
	
		// 2015-1-16 by£ºgaofei Ôö¼ÓÊý¾ÝÍ¬²½»úÖÆ£¬·ÀÖ¹·¢ËÍÊý¾Ý½Ø¶Ï½ÓÊÕ×Ö·û´®
		/*
		for(i=0;	i<1000;	i++)
		{
				tmpRCNT = ComGSM.RCNT;
				if(tmpRCNT == 0)
						break;
				else
				{
						tmpRCNT = tmpRCNT - 1;
						if((RBufGPRS[tmpRCNT]  == '\r') || (RBufGPRS[tmpRCNT]  == '\n'))
						{
								gsmDelayMS(10);
								if((tmpRCNT + 1) == ComGSM.RCNT)
										break;
						}
				}	
				gsmDelayMS(1);
		}
		*/

	 	ComGSM.SCNT =0;
    ComGSM.RCNT =0;
    ComGSM.RLEN =GSM_GPSRSBUF_RXD_SIZE;
		ComGSM.State =MOD_S | SS_I; //ÖÃ·¢ËÍÄ£Ê½¼°·¢ËÍ½øÐÐÖÐ±êÖ¾ 
		ComGSM.State &=~SS_O;				//2015-11-4 Çå·¢ËÍÍê³É±êÖ¾
    
		GSM_ENABLE_SEND_IT();				//Enable	send interrupt
																	
		if(ComGSM.SCNT < ComGSM.SLEN)
		{
				USART_SendData(usart, SBufGPRS[ComGSM.SCNT]);
	  		while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);	  // Èç¹û²»¼Ó¸ÃÐÐÔò·¢ËÍÊý¾Ý²»ÍêÕû£¬È±×Ö½Ú£¡
				ComGSM.SCNT++;
		}
}

/*
*********************************************************************************************************
*                            							GSM´íÎó´¦Àíº¯Êý   
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ERR_Add()
* Description    : ´íÎóÀÛ¼Ó        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void		BSP_GSM_ERR_Add (u16 *Var,	u16	AddTimes)
{
		if(*Var + AddTimes >= 0xffff)
				*Var = 0xffff;
		else
				*Var +=AddTimes;		
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ERR_Clr()
* Description    : ´íÎóÀÛ¼Ó¸´Î»        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void		BSP_GSM_ERR_Clr (u16 *Var)
{
		*Var = 0;
}

/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_ERR_Que()
* Description    : ´íÎóÀÛ¼Ó´ÎÊý²éÑ¯        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
u16		BSP_GSM_ERR_Que (u16 *Var)
{
		return	*Var;
}
/*
*********************************************************************************************************
*                            							GSMµ×²ã·¢ËÍÊý¾ÝÓ¦ÓÃº¯Êý     
*********************************************************************************************************
*/


/*
*********************************************************************************************************
* Function Name  : 													Start2doSendAT()
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void	Start2doSendAT (void)
{
		u16	m=0;
		u8	*pInit8u;
		
    if(s_GSMcomm.SetupPhase == CommandInitOK)
		{}
		else
        return;
    if(s_GSMcomm.ActiveCmd == NoCmdActive)
		{}
    else
				return;
    if(s_GSMcomm.CommandFlag & ATCommandF)
		{}
    else
				return;
    if(s_GSMcomm.CmdPhase == CommandIdle)
		{}
    else																		
        return;

		if(1)
		{
			  s_GSMcomm.ActiveCmd = ATCmdActive;			//	Ê×ÏÈÕ¼¾Ý´®¿Ú
				pInit8u = (u8 *)s_GSMcomm.pSendBuf;
				
				for(m=0;	m<s_ATcomm.Slen;	m++)
				{
			  		*pInit8u = *(s_ATcomm.pSendBuf + m);
			  		pInit8u++;
		    }
				memset((u8 *)(s_ATcomm.pSendBuf),	'\0',	GSM_ATSBUF_TXD_SIZE);	 // ·¢Ö®Ç°Çå¿Õ»º³åÇø
		    /* call sendstart. */
		    s_GSMcomm.Timer = 0;
		    s_GSMcomm.CmdPhase = CommandSend;
		    s_GSMcomm.ps_commu->SLEN = m;
		    s_GSMcomm.sendstart();			//µ÷ÓÃ·¢ËÍ
		}
}
/*
*********************************************************************************************************
* Function Name  : 													DoSendProcess()	
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void	 DoSendProcess (void)
{
		u8	i=0;
		u16	m=0;
		if(s_GSMcomm.SetupPhase == CommandInitOK)
		{}
		else
				return;
    if(s_GSMcomm.ActiveCmd == ATCmdActive)
		{}
		else
        return;
    if(s_GSMcomm.CmdPhase == CommandSend)
		{
        if(s_GSMcomm.ps_commu->State & SS_O)
				{
            /* The whole Command fram has been send out. */
            s_GSMcomm.CmdPhase = CommandSdCpl;
            s_GSMcomm.Timer = 0;
        }
    }
		else if(s_GSMcomm.CmdPhase == CommandSdCpl)
		{
				i = CheckResultCode(s_ATcomm.CheckMode,	s_ATcomm.timeout);			
        if(i)
				{ 
						s_ATcomm.ReturnCode	=	i;
						s_ATcomm.Rlen = s_GSMcomm.ps_commu->RCNT;		
								
						for(m=0; ((m<s_GSMcomm.ps_commu->RCNT) && (m<GSM_ATSBUF_RXD_SIZE));	m++)
						{
								*(s_ATcomm.pRecBuf + m) = *(s_GSMcomm.pRecBuf + m);	//½«·µ»ØÂë¸³Öµµ½AT½ÓÊÕBUFÖÐ
						}
							
            s_GSMcomm.CmdPhase = CommandIdle;							//ÉèÖÃÎªATÃüÁîµÄ¿ÕÏÐÄ£Ê½
            s_GSMcomm.ActiveCmd = NoCmdActive;
						s_GSMcomm.CommandFlag	&= ~ATCommandF;					//ÇåAT·¢ËÍ±êÖ¾
        }
				else
				{}/* wait for receive complete */
    }
		else
		{}/* wait for send complete */
}
/*
*********************************************************************************************************
* Function Name  : 										BSP_SendATcmdPro()
* Description    :          
* Input          : 
* Output         : 
* Return         : ·µ»Ø:	DEF_GSM_NONE_ERR ³É¹¦, DEF_GSM_BUSY_ERRÕ¼ÓÃÖÐ, 
*													ÆäËû·µ»ØÂë(ÊýÖµÎª1~255)²»»áÓë DEF_GSM_NONE_ERR£¬DEF_GSM_BUSY_ERR³åÍ»
*********************************************************************************************************
*/
s8		BSP_SendATcmdPro (u8	checkMode,	u8	*pSendString,	u16	Slen,	u16	PendCode,	u32	timeout)
{
		u16	i=0;
		u32	loop=0;
		s8	retCode=DEF_GSM_NONE_ERR;
		if(s_GSMcomm.ActiveCmd == NoCmdActive)
		{
				for(i=0;	i<Slen;	i++)
				{
					 *(s_ATcomm.pSendBuf + i)	=	*(pSendString + i);	
				}
				s_ATcomm.Slen				=	Slen;					
				s_ATcomm.Rlen				=	0;
				s_ATcomm.timeout		=	timeout;
				s_ATcomm.ReturnCode	=	0;
				s_ATcomm.CheckMode	=	checkMode;
				memset((u8	*)s_ATcomm.pRecBuf,	'\0',	GSM_ATSBUF_RXD_SIZE);
				
				// Êä³ö·¢ËÍ×Ö·û´®
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				//myPrintf("%s",	s_ATcomm.pSendBuf);
				BSP_OSPutChar (DEF_DBG_UART,	s_ATcomm.pSendBuf,	s_ATcomm.Slen);
				#endif

				s_GSMcomm.CommandFlag	|=	ATCommandF;	// ÖÃ±êÖ¾¿ªÊ¼·¢ËÍ;
				
				loop=0;
				while(s_ATcomm.ReturnCode == 0)
				{
						gsmDelayMS(1);		// 2015-1-2 by:felix Ìá¸ßÆ¥ÅäÐ§ÂÊ		
						loop++;
						if(loop >= (timeout*100*2))		// 2015-11-4 by:felix Ôö¼Ó³¬Ê±¼à¿Ø·ÀÖ¹Ò»¶¨¼¸ÂÊµ¼ÖÂµÄAT·¢ËÍ¶ÂÈûµ¼ÖÂÈÎÎñ¸´Î»
								break;
				}	
				
				// Êä³ö½ÓÊÕ×Ö·û´®
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				myPrintf("%s",	s_ATcomm.pRecBuf);
				#endif
																	
				if(s_ATcomm.ReturnCode	!=	PendCode)
						retCode	=	(s8)(s_ATcomm.ReturnCode);
				else
						retCode	=	DEF_GSM_NONE_ERR;
		}
		else
				retCode	=	DEF_GSM_BUSY_ERR;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  : 										BSP_SendATcmdProExt()
* Description    : ÊÖ¶¯·¢ËÍ´¦Àí£¬·ÀÖ¹×Ô¶¯´¦Àí·¢ËÍÇ°Çåbufµ¼ÖÂµÄ½ÓÊÜgprs URCÒÅÂ©         
* Input          : 
* Output         : 
* Return         : ·µ»Ø:	DEF_GSM_NONE_ERR ³É¹¦, DEF_GSM_BUSY_ERRÕ¼ÓÃÖÐ, 
*													ÆäËû·µ»ØÂë(ÊýÖµÎª1~255)²»»áÓë DEF_GSM_NONE_ERR£¬DEF_GSM_BUSY_ERR³åÍ»
*********************************************************************************************************
*/
s8		BSP_SendATcmdProExt (u8	checkMode,	u8	*pSendString,	u16	Slen,	u16	PendCode,	u32	timeout)
{
		u16	i=0, p=0;
		s8	retCode=DEF_GSM_NONE_ERR;
    u16 usCnt = s_GSMcomm.ps_commu->RCNT;
		if(s_GSMcomm.ActiveCmd == NoCmdActive)
		{		
				// Êä³ö·¢ËÍ×Ö·û´®
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				//myPrintf("%s",	s_ATcomm.pSendBuf);
				BSP_OSPutChar (DEF_DBG_UART,	s_ATcomm.pSendBuf,	s_ATcomm.Slen);
				#endif

        BSP_PutChar(DEF_USART_1,pSendString,Slen);

        i=0;
        usCnt = s_GSMcomm.ps_commu->RCNT;
				while(1)
				{
						if(s_GSMcomm.ps_commu->RCNT >= usCnt)
						{
								p = StringSearch((u8 *)&s_GSMcomm.pRecBuf[usCnt],(u8 *)"\r\nOK",(s_GSMcomm.ps_commu->RCNT - usCnt),4);		
								if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
								{
										break;
								}		
								OSTimeDly(1);   
								if(i++  >= timeout * 100u)
								{
										retCode	=	DEF_GSM_BUSY_ERR;
										break;
								}
								else
								{
								}
						}
						else
						{
								retCode	=	DEF_GSM_BUSY_ERR;
								break;
						}
				}				
		}
		else
				retCode	=	DEF_GSM_BUSY_ERR;

		return	retCode;	
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMSoftReset() 
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMSoftReset(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// Èí¼þ¸´Î»
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CFUN11,	strlen((const char *)AT_CFUN11),	CommandRecOk,	CFUN11Timeout) !=	DEF_GSM_NONE_ERR)
						{																
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_CFUN11_ERR;		
						}	
						else
						{
								BSP_GSM_ReleaseFlag(DEF_ALL_FLAG);						//  Çå³ýËùÓÐ×´Ì¬±êÖ¾
								memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	//  Çå´íÎó±äÁ¿
								s_GSMcomm.GSMComStatus 	= GSM_POWOK;					//  ÖÃ³õÊ¼»¯ÃüÁî¿ªÊ¼·¢ËÍ×´Ì¬
								s_GSMcomm.SetupPhase 		= CheckSimCardSend;		//  Èí¼þ¸´Î»ºóÐèÒªÍË»Øµ½sim¿¨¼ì²â²½Öè
								s_GSMcomm.Timer					=	0;
								s_GSMcomm.HardWareSta		=	DEF_GSMHARD_NONE_ERR;	
								retCode = DEF_GSM_NONE_ERR;
						}					
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}

/*
*********************************************************************************************************
*                            					GSM TTSÓ¦ÓÃº¯Êý(ÓÉÉÏ²ãÓ¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSPlayPro() 
* Description    : ²¥·ÅTTSÓïÒô(¿ªÊ¼²¥·ÅºóÉèÖÃTTSPlayF,É¨Ãèº¯Êý¼ì²âµ½½áÊøURCºó¸´Î»¸Ã±êÖ¾)        
* Input          : mode			:DEF_TTS_PLAY_ASCII or  DEF_TTS_PLAY_UCS2 
*                : * pString: UCS2Âë»òASCIIÂë×Ö·û´®
*                : len			: ×Ö·û´®³¤¶È
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TTSPlayPro	(u8 mode,	u8* pString, u16 len,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8  tmpBuf[100]={0};	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase >= CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))	
				{
						//	Send "AT+CTTS=1/2,string\r" AT_CTTS_2_H
						memset(tmpBuf,	0,	sizeof(tmpBuf));
						
						if(mode == DEF_TTS_PLAY_ASCII)
								strcat((char *)tmpBuf,	(char *)AT_CTTS_2_H);
						else 
								strcat((char *)tmpBuf,	(char *)AT_CTTS_1_H);
						
						if(len < (sizeof(tmpBuf) - strlen((const char *)tmpBuf) - 1))
						{
								strcat((char *)tmpBuf,	(char *)pString);
								strcat((char *)tmpBuf,	(char *)"\r");
																
								IO_SPAKER_PA_EN();	// Ê×ÏÈÊ¹ÄÜ¹¦·Å
								if(BSP_SendATcmdProExt(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CTTSTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_CTTS_ERR;			
								}
								else
								{
										BSP_GSM_SetFlag (DEF_TTSPLAY_FLAG);		// ÉèÖÃTTS²¥·ÅÖÐ±êÖ¾£¬ÔÚÉ¨Ãèµ½URC½áÊøºó½«×Ô¶¯Çå¿Õ
										retCode = DEF_GSM_NONE_ERR;			
								}							
						}
						else
								retCode	=	DEF_GSM_DATA_OVERFLOW;	// ÓïÒôÊý¾Ý³¤¶ÈÒç³ö´íÎó								
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSStopPro() 
* Description    : Í£Ö¹²¥·ÅTTSÓïÒô        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TTSStopPro	(u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						//	Send "AT+CTTS=0" AT_CTTS_0
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CTTS_0,	strlen((const char *)AT_CTTS_0),	CommandRecOk,	CTTSTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_CTTS_ERR;			
						}
						else
						{					
								IO_SPAKER_PA_DIS();	// ¹Ø±Õ¹¦·Å
								retCode = DEF_GSM_NONE_ERR;					
						}
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSCfgPro() 
* Description    : ÅäÖÃTTS²¥·Å²ÎÊý        
* Input          : vol		:ÒôÁ¿´óÐ¡£¨0~100,dft:50£©
*                : mode		:²¥·ÅÄ£Ê½£¨0~3,dft:0£©
*                : pitch	:Òôµ÷£¬Èç¹û¸ßÉùÒôÏ¸¼â£¨1~100,dft:50£©
*                : speed	:ÓïËÙ£¨1~100,dft:50£©
*                : channel:Í¨µÀºÅ£¬¶ÔÓÚSIM800Ö»ÓÐÒ»¸öÍ¨µÀ¼´Í¨µÀ0£¬ÆäËûÖµ½«·µ»Ø´íÎó£¨0~1,dft:0£©
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TTSCfgPro	(u8 vol, u8 mode,	u8 pitch,	u8 speed,	u8 channel,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8  tmpBuf[50]="",tmpBuf1[25]="";	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((vol > 100) || (mode > 4) || (pitch == 0) || (pitch > 100) || (speed == 0) || (speed > 100) || (channel != 0))
						retCode = DEF_GSM_FAIL_ERR; 
				else
				{
    				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
    				{
    						//	Send "AT+CTTSPARAM=<vol>,<mode>,<pitch>,<speed>,<channel>\r" AT_CTTSPARAM_H     						 						
    						memset(tmpBuf,	0,	sizeof(tmpBuf));
    						memset(tmpBuf1,	0,	sizeof(tmpBuf1));   						
    						strcat((char *)tmpBuf,	(char *)AT_CTTSPARAM_H);
    						sprintf((char *)tmpBuf1,	"%d,%d,%d,%d,%d\r",	vol,mode,pitch,speed,channel);
    						strcat((char *)tmpBuf,	(char *)tmpBuf1);
    													
    						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CTTSPARAMTimeout) !=	DEF_GSM_NONE_ERR)
    						{
    								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
    								retCode	=	DEF_GSM_CTTSPARAM_ERR;
    						}
    						else
    								retCode = DEF_GSM_NONE_ERR;
    				}
    				else
    						retCode	=	DEF_GSM_BUSY_ERR;
    		}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TTSCfgRingPro() 
* Description    : ²¥·ÅÀ´µçÊ±Ê¹ÄÜ»ò½ûÖ¹TTS²¥·Å        
* Input          : en:DEF_ENABLE or  DEF_DISENABLE 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TTSCfgRingPro	(u8 en,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[20]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						//	Send "AT+CTTSRING=0/1\r" AT_CTTSRING_
						memset(tmpBuf,	0,	sizeof(tmpBuf));
						if(en == DEF_ENABLE)
								strcat((char *)tmpBuf,	(char *)AT_CTTSRING_1);
						else 
								strcat((char *)tmpBuf,	(char *)AT_CTTSRING_0);
													
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CTTSRINGTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_CTTSRING_ERR;			
						}
						else
								retCode = DEF_GSM_NONE_ERR;			
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}

/*
*********************************************************************************************************
*                            					GSM FSÓ¦ÓÃº¯Êý(ÓÉÉÏ²ãÓ¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSCreateFilePro() 
* Description    : ´´½¨ÎÄ¼þ£¬Èç¹ûÓÐ¸ÃÎÄ¼þÔòÖ±½Ó·µ»Ø³É¹¦£¨Ö»Ö§³ÖÔÚ¸úÄ¿Â¼UserÄ¿Â¼ÏÂ½¨Á¢ÎÄ¼þ£©      
* Input          : *pFile:ÎÄ¼þÃû£¨ÀýÈç£ºlog.txt£©
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_FSCreateFilePro	(u8 *pFile,	u32 timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="";
		u16	j=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// ²éÑ¯UserÄ¿Â¼ÊÇ·ñ´æÔÚ	
						//	Send "AT+FSLS=C:\\r" AT_FSLS_C	
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_FSLS_C,	strlen((const char *)AT_FSLS_C),	CommandRecOk,	FSLSTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_FSLS_ERR;	
						}
						else						
						{
								j = 0;
								j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"User\\",s_GSMcomm.ps_commu->RCNT,5);
								if(j == 0xffff)		
								{
										// UserÄ¿Â¼²»´æÔÚ,½¨Á¢Ä¿Â¼
										//	Send "AT+FSMKDIR=C:\\User\\r" AT_FSMKDIR_H
		    						memset(tmpBuf,	0,	sizeof(tmpBuf));
		    						strcat((char *)tmpBuf,	(char *)AT_FSMKDIR_H);
		    						strcat((char *)tmpBuf,	"C:\\User\\r");
		
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSMKDIRTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												retCode	=	DEF_GSM_FSMKDIR_ERR;	
        						}	
        						else
        						{       										       								
        								// ½¨Á¢ÎÄ¼þ
            						//	Send "AT+FSCREATE=C:\\User\file.txt\r" AT_FSCREATE_H
            						memset(tmpBuf,	0,	sizeof(tmpBuf));
            						strcat((char *)tmpBuf,	(char *)AT_FSCREATE_H);
            						strcat((char *)tmpBuf,	"C:\\User\\");
            						strcat((char *)tmpBuf,	(char *)pFile);
            						strcat((char *)tmpBuf,	"\r");
            													
            						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSCREATETimeout) !=	DEF_GSM_NONE_ERR)
            						{
            								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
            								retCode	=	DEF_GSM_FSCREATE_ERR;			
            						}	
            						else
            								retCode = DEF_GSM_NONE_ERR;		// ÎÄ¼þ½¨Á¢³É¹¦!!										
        						} 																								
								}
								else
								{
										// UserÄ¿Â¼ÒÑ¾­´æÔÚ£¬²éÑ¯ÎÄ¼þÊÇ·ñ´æÔÚ	
										//	Send "AT+FSLS=C:\\User\\r" AT_FSLS_USER											
										if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_FSLS_USER,	strlen((const char *)AT_FSLS_USER),	CommandRecOk,	FSLSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												retCode	=	DEF_GSM_FSLS_ERR;	
										}
										else
										{
												j = 0;
												j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)pFile, s_GSMcomm.ps_commu->RCNT, strlen((const char *)pFile));
												if(j == 0xffff)		
												{
														// ½¨Á¢ÎÄ¼þ
                						//	Send "AT+FSCREATE=C:\\User\file.txt\r" AT_FSCREATE_H
                						memset(tmpBuf,	0,	sizeof(tmpBuf));
                						strcat((char *)tmpBuf,	(char *)AT_FSCREATE_H);
                						strcat((char *)tmpBuf,	"C:\\User\\");
                						strcat((char *)tmpBuf,	(char *)pFile);
                						strcat((char *)tmpBuf,	"\r");
                													
                						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSCREATETimeout) !=	DEF_GSM_NONE_ERR)
                						{
                								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
                								retCode	=	DEF_GSM_FSCREATE_ERR;			
                						}	
                						else
                								retCode = DEF_GSM_NONE_ERR;		// ÎÄ¼þ½¨Á¢³É¹¦!!		
												}
												else
														retCode = DEF_GSM_NONE_ERR;		// ÎÄ¼þÒÑ¾­´æÔÚÖ±½Ó·µ»Ø
										}
								}									
						}
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSDelFilePro() 
* Description    : É¾³ýÎÄ¼þ£¨Ö»Ö§³ÖÔÚ¸úÄ¿Â¼UserÄ¿Â¼ÏÂÉ¾³ýÎÄ¼þ£©
* Input          : *pFile:ÎÄ¼þÃû£¨ÀýÈç£ºlog.txt£©
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_FSDelFilePro	(u8 *pFile,	u32 timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// ²»²éÑ¯ÎÄ¼þÖ±½ÓÉ¾³ý
						//	Send "AT+FSDEL=C:\\User\file.txt\r" AT_FSDEL_H												
						memset(tmpBuf,	0,	sizeof(tmpBuf));
						strcat((char *)tmpBuf,	(char *)AT_FSDEL_H);
						strcat((char *)tmpBuf,	(char *)pFile);
						strcat((char *)tmpBuf,	"\r");													
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSDELTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_FSDEL_ERR;			
						}	
						else
								retCode = DEF_GSM_NONE_ERR;		// ÎÄ¼þÉ¾³ý³É¹¦			
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSReadFilePro() 
* Description    : ¶ÁÈ¡ÎÄ¼þ£¨Ö»Ö§³ÖÔÚ¸úÄ¿Â¼UserÄ¿Â¼ÏÂ¶ÁÈ¡ÎÄ¼þ£©
* Input          : *pFile	:ÎÄ¼þÃû£¨ÀýÈç£ºlog.txt£©
*								 : posi		:¶ÁÈ¡Æ«ÒÆÁ¿£¨µ¥Î»£º×Ö½Ú£©
*								 : *rData	:¶ÁÈ¡µ½µÄÊý¾ÝÖ¸Õë
*								 : reqLen	:ÇëÇó¶ÁÈ¡µÄÎÄ¼þ³¤¶È£¨³¤¶È²»ÄÜ³¬¹ýGSMµ×²ã¶ÁÈ¡»º³åÇø´óÐ¡£©
*								 : *retLen:Êµ¼Ê¶ÁÈ¡µÄÎÄ¼þ³¤¶È
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_FSReadFilePro	(u8 *pFile,	u16 posi, u8 *rData, u16 reqLen, u16 *retLen,	u32 timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="",tmpBuf1[20]="";
		u16 j=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{							
				if(reqLen > (GSM_ATSBUF_RXD_SIZE - 100))
						retCode	=	DEF_GSM_DATA_OVERFLOW;		// ³¬³öGSM½ÓÊÕ»º³åÇø´óÐ¡²»Óè´¦Àí
				else
				{						
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{						
								// ×é°ü
								//	Send "AT+FSREAD=C:\\User\file,0/1,len,posi\r" AT_FSREAD_H												
								memset(tmpBuf,	0,	sizeof(tmpBuf));
								memset(tmpBuf1,	0,	sizeof(tmpBuf1));
								strcat((char *)tmpBuf,	(char *)AT_FSREAD_H);
								strcat((char *)tmpBuf,	"C:\\User\\");
								strcat((char *)tmpBuf,	(char *)pFile);
								
								if(posi == 0) 
										sprintf((char *)tmpBuf1,	",0,%d,0\r",	reqLen);				// ´ÓÎÄ¼þÆðÊ¼Î»ÖÃ¶ÁÈ¡
								else
										sprintf((char *)tmpBuf1,	",1,%d,%d\r",	reqLen,posi);	// ´ÓÎÄ¼þposiÆ«ÒÆÎ»ÖÃ¶ÁÈ¡
								
								strcat((char *)tmpBuf,	(char *)tmpBuf1);												
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	FSREADTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_FSREAD_ERR;			
								}	
								else
								{
										// ¸´ÖÆÊý¾ÝÄÚÈÝ(²»ÄÜÅÐ¶Ï»Ø³µ»»ÐÐ×÷Îª½áÊøÌõ¼þ£¬Ð´µÄ¹ý³ÌÖÐ»á×Ô¶¯¸øÃ¿Ìõ¼ÇÂ¼Ôö¼Ó»Ø³µ»»ÐÐ)
										for(j=0;	(*(s_GSMcomm.pRecBuf + j) != 'O') && (*(s_GSMcomm.pRecBuf + j) != 'K') && (j < s_GSMcomm.ps_commu->RCNT);	j++)
										{
												*(rData + j) = *(s_GSMcomm.pRecBuf + j);
										}
										*retLen = j;	// »ñÈ¡Êµ¼Ê³¤¶È
										retCode = DEF_GSM_NONE_ERR;		// ÎÄ¼þ¶ÁÈ¡³É¹¦	
								}		
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_FSWriteFilePro() 
* Description    : Ð´ÎÄ¼þ£¨Ö»Ö§³ÖÔÚ¸úÄ¿Â¼UserÄ¿Â¼ÏÂÐ´ÎÄ¼þ£¬×Ô¶¯Ð´µ½ÎÄ¼þ½áÎ²´¦£¬Èç¹û¿Õ¼ä²»×ãÔò×Ô¶¯Ìøµ½ÎÄ¼þÍ·Ð´¸²¸Ç×îÔçµÄÎÄ¼þ²¿·Ö£©
*                : Ð´Ã¿Ìõ¼ÇÂ¼Ê±»á×Ô¶¯ÎªÃ¿Ìõ¼ÇÂ¼Ôö¼Ó»Ø³µ»»ÐÐ£¬ÏÂ´ÎÔÙ´ÎÐ´ÈëÊ±ÔÚÐÂµÄÒ»ÐÐ²åÈëÎÞÐèÊÖ¶¯Ìí¼Ó»Ø³µ»»ÐÐ
*								 : ÎÄ¼þµÄ¿ÉÓÃ¿Õ¼ä¾ö¶¨ÓÚÄ£¿é¹Ì¼þ°æ±¾£¬Èç¹ûÎªTTS°æ±¾µÄÔò´óÒ»Ð©£¬Èç¹ûÎªÀ¶ÑÀ°æ±¾ÔòÐ¡Ò»Ð©£»×îÐ¡´óÔ¼60kByte
* Input          : *pFile	:ÎÄ¼þÃû£¨ÀýÈç£ºlog.txt£©
*								 : *wData	:Ð´Êý¾Ý»º³åÇøÖ¸Õë
*								 : wLen		:Ð´µÄÊý¾Ý³¤¶È
*								 : wTimeOut:Ð´³¬Ê±£¨µ¥Î»s±ØÐëÐ¡ÓÚ¡°Ð´³¬Ê±¡±²ÅÓÐÐ§£¬·ñÔò³ÌÐòÔÚÃ»Ð´ÍêÇ°¾Í³¬Ê±·µ»ØÁË£©
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_FSWriteFilePro	(u8 *pFile, u8 *wData, u16 wLen, u8 wTimeOut,	u32 timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="",tmpBuf1[20]="";
		u16 j=0,k=0;
		u32	avaLen=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{							
				if(wLen > (GSM_ATSBUF_TXD_SIZE - 100))
						retCode	=	DEF_GSM_DATA_OVERFLOW;		// ³¬³öGSM·¢ËÍ»º³åÇø´óÐ¡²»Óè´¦Àí
				else
				{					
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{						
								// Ê×ÏÈÅÐ¶Ï¿ÉÓÃ¿Õ¼ä´óÐ¡
								//	Send "AT+FSMEM\r"	AT_FSMEM
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_FSMEM,	strlen((const char *)AT_FSMEM),	CommandRecOk,	FSMEMTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_FSMEM_ERR;			
								}	
								else
								{
										j = 0;
										j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+FSMEM: ", s_GSMcomm.ps_commu->RCNT, 8);
										if(j == 0xffff)	
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												retCode = DEF_GSM_NONE_ERR;		// ÒÑ¾­´æÔÚÎÄ¼þÖ±½Ó·µ»Ø	
										}
										else
										{
												// »ñÈ¡³¤¶ÈÐÅÏ¢
												j = j + 10; // Ö¸Ïò¿Õ¼ä´óÐ¡
												memset(tmpBuf1,'\0',sizeof(tmpBuf1));
												for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != 'b') && (k < sizeof(tmpBuf1)) && (k < s_GSMcomm.ps_commu->RCNT);	k++)
												{
														tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
												}
												tmpBuf1[k] = '\0';
												avaLen = atoi((const char *)tmpBuf1);
												
												// ×é°ü
												//	Send "AT+FSWRITE=C:\\User\file.txt,0/1,len,timeout\r" AT_FSWRITE_H
												memset(tmpBuf,	0,	sizeof(tmpBuf));
												memset(tmpBuf1,	0,	sizeof(tmpBuf1));
												strcat((char *)tmpBuf,	(char *)AT_FSWRITE_H);
												strcat((char *)tmpBuf,	"C:\\User\\");
												strcat((char *)tmpBuf,	(char *)pFile);
												// ÅÐ¶ÏÇëÇóÐ´Èë³¤¶ÈÊÇ·ñºÏ·¨
												if((wLen + 1000) >= avaLen)	
														strcat((char *)tmpBuf,	",0");	// Èç¹ûÎÄ¼þÒç³öÌøµ½¿ªÍ·Ð´Èë¸²¸Ç×î¿ªÊ¼µÄÐÅÏ¢
												else
														strcat((char *)tmpBuf,	",1");	// ´ÓÎÄ¼þ½áÎ²Ð´Èë
												
												sprintf((char *)tmpBuf1,	",%d,%d\r",	wLen,wTimeOut);	
												strcat((char *)tmpBuf,	(char *)tmpBuf1);								
												
												// Pend">"							
												if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecFSWriteAck,	FSWRITETimeout) !=	DEF_GSM_NONE_ERR)	//ÔÝÊ±¹Ì¶¨³¬Ê±ÎªFSWRITETimeout
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
														retCode	=	DEF_GSM_FSWRITE_ERR;			
												}	
												else
												{
														//	Send data
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)wData,	wLen,	CommandRecOk,	FSWRITECPLTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);	// ´íÎóÀÛ¼Ó
																retCode	=	DEF_GSM_FSWRITECPL_ERR;
														}	
														else
																retCode = DEF_GSM_NONE_ERR;		// ÎÄ¼þÐ´Èë³É¹¦!!		
												}															
										}										
								}									
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
*                            					GSM BTÓ¦ÓÃº¯Êý(ÓÉÉÏ²ãÓ¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/


/*
*********************************************************************************************************
* Function Name  :									  BSP_BTInitPro() 
* Description    : À¶ÑÀ³õÊ¼»¯£¬²ÎÊýÅäÖÃµÈ
* Input          : *pName	:ÎÄ¼þÃû£¨ÀýÈç£ºY05D-105£©×î³¤18char
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTInitPro	(u8 *pName, u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8	tmpBuf[50]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ´ò¿ªÀ¶ÑÀµçÔ´(¸ÃÌõÖ¸Áî²»¼ì²â·µ»ØÂë£¬Èç¹ûÖ®Ç°À¶ÑÀµçÔ´Îª¿ªÆô×´Ì¬Ôò»á·µ»Ø"ERROR"Ò²ÈÏÎªµçÔ´ÒÑ¾­Ê¹ÄÜÁË)
						// Send "AT+BTPOWER=1\r"
						BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTPOWER_1,	strlen((const char *)AT_BTPOWER_1),	CommandRecOk,	BTPOWERTimeout);
						gsmDelayMS(1000);	
					
						// ÅäÖÃÀ¶ÑÀÖ÷»úÃû³Æ
						// Send "AT+BTHOST=name\r"	AT_BTHOST_H
						memset(tmpBuf,	0,	sizeof(tmpBuf));
        		strcat((char *)tmpBuf,	(char *)AT_BTHOST_H);
        		strcat((char *)tmpBuf,	(char *)pName);
						strcat((char *)tmpBuf,	"\r");
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	BTHOSTTimeout)	!=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	= DEF_GSM_BTHOST_ERR;	
						}	
						else
						{
								gsmDelayMS(500);	
								// ÅäÖÃÀ¶ÑÀ¿É¼ûÐÔ	
								// Send "AT+BTVIS=1\r"	AT_BTVIS_1
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTVIS_1,	strlen((const char *)AT_BTVIS_1),	CommandRecOk,	BTVISTimeout)	!=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	= DEF_GSM_BTVIS_ERR;	
								}	
								else
								{
										// ÅäÖÃÀ¶ÑÀ½ÓÊÕÊý¾ÝÄ£Ê½ÎªÊÖ¶¯Ä£Ê½
										// Send "AT+BTSPPGET=1\r"	AT_BTSPPGET_1	
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTSPPGET_1,	strlen((const char *)AT_BTSPPGET_1),	CommandRecOk,	BTSPPGET1Timeout)	!=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												retCode	= DEF_GSM_BTSPPGET1_ERR;	
										}	
										else
												retCode = DEF_GSM_NONE_ERR;
								}							
						}
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;				
		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTPairAckPro() 
* Description    : ½ÓÊÕÆäËü¿Í»§¶Ë·¢À´µÄÆ¥ÅäÇëÇó
* Input          : mode		:0-Êý×ÖÈÏÖ¤£¬1-Passkey ÈÏÖ¤
*                : *pPass	:Èç¹ûÄ£Ê½Îª1ÔòÐèÒªÌá¹©ÃÜÂë£¨³¤¶È4~16×Ö½Ú£©
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTPairAckPro	(u8 mode,	u8 *pPass, u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8  tmpBuf[50]="";
		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ½ÓÊÕÆ¥Åä
						if(mode == 0)
						{
								// Êý×ÖÆ¥Åä
								// Send "AT+BTPAIR=1,1\r" AT_BTPAIR_1
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTPAIR_1,	strlen((const char *)AT_BTPAIR_1),	CommandRecOk,	BTPAIRTimeout)	!=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	= DEF_GSM_BTPAIR_ERR;	
								}	
								else
										retCode = DEF_GSM_NONE_ERR;
						}
						else
						{
								// PasskeyÆ¥Åä
								// Send "AT+BTPAIR=2,Passkey\r" AT_BTPAIR_2_H
								memset(tmpBuf,	0,	sizeof(tmpBuf));
        				strcat((char *)tmpBuf,	(char *)AT_BTPAIR_2_H);
        				strcat((char *)tmpBuf,	(char *)pPass);
        				strcat((char *)tmpBuf,	"\r");
								
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	BTPAIRTimeout)	!=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	= DEF_GSM_BTPAIR_ERR;	
								}	
								else
										retCode = DEF_GSM_NONE_ERR;
						}
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;				
		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTConnectAckPro() 
* Description    : ½ÓÊÕÁ¬½ÓÇëÇó
* Input          : en	: DEF_DISABLE-½ûÖ¹Á¬½Ó;DEF_ENABLE-ÔÊÐíÁ¬½Ó 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTConnectAckPro	(u8 en,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ½ÓÊÕÁ¬½ÓÇëÇó
						// Send "AT+BTACPT=1\r" AT_BTACPT_1
						if(en == DEF_DISABLE)
						{
    						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTACPT_0,	strlen((const char *)AT_BTACPT_0),	CommandRecOk,	BTACPTTimeout)	!=	DEF_GSM_NONE_ERR)
    						{
    								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
    								retCode	= DEF_GSM_BTACPT_ERR;	
    						}	
    						else
								{
    								BSP_GSM_ReleaseFlag(DEF_BTDING_FLAG);					// ÉèÖÃÀ¶ÑÀÀëÏß±êÖ¾
										retCode = DEF_GSM_NONE_ERR;		
								}
    				}		
    				else
    				{
    						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_BTACPT_1,	strlen((const char *)AT_BTACPT_1),	CommandRecOk,	BTACPTTimeout)	!=	DEF_GSM_NONE_ERR)
    						{
    								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
    								retCode	= DEF_GSM_BTACPT_ERR;	
    						}	
    						else
								{
    								BSP_GSM_SetFlag(DEF_BTDING_FLAG);							// ÉèÖÃÀ¶ÑÀÔÚÏß±êÖ¾
										retCode = DEF_GSM_NONE_ERR;	
								}
    				}		
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;				
		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTRxdDataPro() 
* Description    : »ñÈ¡BTÊý¾Ý
* Input          : *pBuf	:½ÓÊÕµ½µÄÊý¾Ý»º³åÇøÖ¸Õë
*								 : *len		:½ÓÊÕµ½µÄÊý¾Ý³¤¶È
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTRxdDataPro	(u8	*pBuf,	u16	*len,	u32	timeout)
{
		
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="",tmpBuf1[20]="";
		u16 j=0,k=0,getLen=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{											
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// »ñÈ¡Êý¾Ý³¤¶ÈÐÅÏ¢
						//	Send "AT+BTSPPGET=2\r" AT_BTSPPGET_2		
						tcpReadMuxtex =1;	// ÖÃ»¥³â±êÖ¾,Ê¹URC¼ì²â¹ÒÆð
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_BTSPPGET_2,	strlen((const char *)AT_BTSPPGET_2),	CommandRecOk,	BTSPPGET2Timeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_BTSPPGET2_ERR;			
						}	
						else
						{
								gsmDelayMS(30);		// µÈ30ms½ÓÊÕÊý¾ÝÍê³É(·ÀÖ¹½ÓÊÕHEXÊý¾ÝÖÐº¬¡®OK¡¯×Ö·û´®µ¼ÖÂÆ¥Åä³É¹¦ÌáÇ°ÍË³ö)
								//+BTSPPGET: 1,30
								j = 0;
								j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+BTSPPGET: ", s_GSMcomm.ps_commu->RCNT, 11);
								if(j == 0xffff)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode = DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										// »ñÈ¡³¤¶ÈÐÅÏ¢
										j = j + 13; // Ö¸Ïò³¤¶ÈÐÅÏ¢
										memset(tmpBuf1,'\0',sizeof(tmpBuf1));
										for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < sizeof(tmpBuf1)) && (k < s_GSMcomm.ps_commu->RCNT);	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k] = '\0';
										getLen = atoi((const char *)tmpBuf1);
										
										// »ñÈ¡Êý¾Ý×é°ü
										//	Send "AT+BTSPPGET=3,len\r" AT_BTSPPGET_3_H
										memset(tmpBuf,	'\0',	sizeof(tmpBuf));
        						strcat((char *)tmpBuf,	(char *)AT_BTSPPGET_3_H);
        						strcat((char *)tmpBuf,	(char *)tmpBuf1);
        						strcat((char *)tmpBuf,	"\r");
        						
										// Read							
        						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	BTSPPGET3Timeout) !=	DEF_GSM_NONE_ERR)	
        						{
        								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
        								retCode	=	DEF_GSM_BTSPPGET3_ERR;			
        						}	
        						else
        						{
        								//+BTSPPGET: 1,30,SIMCOMSPPFORAPPSIMCOMSPPFORAPP
        								j = 0;
												j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+BTSPPGET: ", s_GSMcomm.ps_commu->RCNT, 11);
												if(j == 0xffff)	
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
														retCode = DEF_GSM_UNKNOW_ERR;
												}
												else
												{
														// »ñÈ¡Êý¾ÝÐÅÏ¢
														j = j + 16; // Ö¸ÏòÊý¾ÝÐÅÏ¢
														for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n')  && (k < s_GSMcomm.ps_commu->RCNT);	k++)
														{
																*(pBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
														}
														*(pBuf + k) = '\0';	
														*len =getLen;
														retCode = DEF_GSM_NONE_ERR;
				        				}
										}
								}
						}
						// ¶ÁÈ¡Íê±ÏºóÇå³ý½ÓÊÕÊý¾ÝÄÚÈÝ,·ÀÖ¹URCÎó¼ì²â
						memset(RBufAT,	0,	sizeof(RBufAT));
						memset(RBufGPRS,	0,	sizeof(RBufGPRS));
						s_GSMcomm.ps_commu->RCNT =0;
						tcpReadMuxtex =0;	// Çå±êÖ¾,Ê¹ÄÜÖØÐÂURC¼ì²â									
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_BTSendDataPro() 
* Description    : ·¢ËÍBTÊý¾Ý
* Input          : *pBuf	:ÐèÒª·¢ËÍµÄÊý¾Ý»º³åÇøÖ¸Õë
*								 : len		:·¢ËÍÊý¾Ý³¤¶È
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_BTSendDataPro	(u8	*pBuf,	u16	len,	u32	timeout)
{	
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8 	tmpBuf[50]="",tmpBuf1[20]="";
		u16 j=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{											
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// ²éÑ¯µ±Ç°¹¤×÷Ä£Ê½£¨¿Í»§¶ËÄ£Ê½/·þÎñÆ÷Ä£Ê½£©
						//	Send "AT+BTSPPCFG?\r" AT_BTSPPCFG																						
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_BTSPPCFG,	strlen((const char *)AT_BTSPPCFG),	CommandRecOk,	BTSPPCFGTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_BTSPPCFG_ERR;			
						}	
						else
						{
								j = 0;
								j = StringSearch((u8 *)s_GSMcomm.pRecBuf, (u8 *)"+BTSPPCFG:", s_GSMcomm.ps_commu->RCNT, 10);
								if(j == 0xffff)		
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode = DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										// »ñÈ¡Ä£Ê½ÐÅÏ¢
										j = j + 15; // Ö¸Ïò³¤¶ÈÐÅÏ¢
										if(*(s_GSMcomm.pRecBuf + j) != '1')
												retCode	=	DEF_GSM_FAIL_ERR;		// Ä£Ê½²»ÕýÈ··µ»Ø´íÎó£¬²»×ö¹ÊÕÏÀÛ¼Ó
										else
										{
												// ×é°ü
												//	Send "AT+BTSPPSEND=len" AT_BTSPPSEND_H												
												memset(tmpBuf,	'\0',	sizeof(tmpBuf));
												memset(tmpBuf1,	'\0',	sizeof(tmpBuf1));
		        						strcat((char *)tmpBuf,	(char *)AT_BTSPPSEND_H);		        						
		        						sprintf((char *)tmpBuf1,	"%d\r",	len);	
		        						strcat((char *)tmpBuf,	(char *)tmpBuf1);								
		        						
		        						// Pend">"							
		        						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecBTSendAck,	BTSPPSENDTimeout) !=	DEF_GSM_NONE_ERR)	
		        						{
		        								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
		        								retCode	=	DEF_GSM_BTSPPSEND_ERR;			
		        						}	
		        						else
		        						{
		        								//	Send data
														if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)pBuf,	len,	RecBTSendCpl,	BTSPPSENDCPLTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);	// ´íÎóÀÛ¼Ó
																retCode	=	DEF_GSM_BTSPPSENDCPL_ERR;
														}	
		        								else
		        										retCode = DEF_GSM_NONE_ERR;		// ÎÄ¼þÐ´Èë³É¹¦!!		
		        						}																					
										}
								}
						}		
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;					
}
/*
*********************************************************************************************************
*                            					GSMÉÏ²ãÓ¦ÓÃº¯Êý-×´Ì¬²éÑ¯²¿·Ö(ÓÉOS Ó¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_QCSQCodePro() 
* Description    : ²éÑ¯ÐÅºÅÇ¿¶È¼°ÎóÂëÂÊ        
* Input          : Èç¹ûÐÅºÅÇ¿¶ÈÎª99(±íÊ¾ÎÞ·¨»ñÈ¡)Ö±½ÓÐ´0
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QCSQCodePro	(u8 *pSLevel, u8*pRssi,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8	tmpBuf[20]="";	
		u16	k=0,j=0;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// Send "AT+CSQ"	Ê×ÏÈ¼ì²âÐÅºÅÇ¿¶È
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CSQ,	strlen((const char *)AT_CSQ),	CommandRecOk,	CSQTimeout)	!=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	= DEF_GSM_CSQ_ERR;	
						}	
						else
						{
								j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CSQ: ",s_GSMcomm.ps_commu->RCNT,6);
								if(j == 0xffff)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_UNKNOW_ERR;	
								}
								else
								{
										j = j + 6;
										// È¡ÐÅºÅÇ¿¶È
										memset(tmpBuf,'\0',sizeof(tmpBuf));
										for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != ',') && (k <= 1);	k++)
										{
												tmpBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										*pSLevel = atoi((const char *)tmpBuf);
										if(*pSLevel == 99)
												*pSLevel = 0;
										
										// È¡ÎóÂëÂÊ	
										j = j + k + 1;
										memset(tmpBuf,'\0',sizeof(tmpBuf));	
										for(k=0;	(*(s_GSMcomm.pRecBuf + j + k) != '\r') && (k <= 1);	k++)
										{
												tmpBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										*pRssi = atoi((const char *)tmpBuf);	// È¡ÎóÂëÂÊ	
										retCode = DEF_GSM_NONE_ERR;
								}											
						}
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}			
				
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QCellGPSPosiPro() 
* Description    : ²éÑ¯Éè±¸»ùÕ¾¶¨Î»ÐÅÏ¢(×î´ó³¬Ê±Ê±¼ä60s)       
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QCellGPSPosiPro	(u8 *pLonBuf, u8 *pLatBuf, u8 *pDTime,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8	tmpBuf1[20]="";
		u16	k=0,j=0;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// ÐèÒª¸üÐÂÖ¸Áî
						/*
						//	Send "AT^SMOND"
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_SMOND,	strlen((const char *)AT_SMOND),	CommandRecOk,	SMONDTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_SMOND_ERR;			
						}
						else
						{
								j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"^SMOND:",s_GSMcomm.ps_commu->RCNT,7);
								if(j == 0xffff)	
								{
										retCode	=	DEF_GSM_UNKNOW_ERR;	
								}
								else
								{
										memset((u8 *)tmpBuf1,	'\0',	sizeof(tmpBuf1));

										// È¡ÒÆ¶¯¹ú¼Ò´úÂë(MCC)
										j = j+7;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 3));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';										
										strcpy((char *)pMCCBuf,	(const char *)tmpBuf1);

										// È¡ÒÆ¶¯ÔËÓªÉÌ´úÂë(MNC)
										j = j+k+1;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 3));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';
										strcpy((char *)pMNCBuf,	(const char *)tmpBuf1);

										// È¡Ð¡ÇøºÅÂë(LAC)
										j = j+k+1;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 4));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';
										strcpy((char *)pLACBuf,	(const char *)tmpBuf1);
										
										// È¡»ùÕ¾ºÅ(CELL)
										j = j+k+1;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 4));	k++)
										{
												tmpBuf1[k] = *(s_GSMcomm.pRecBuf + j + k);
										}
										tmpBuf1[k]='\0';
										strcpy((char *)pCELLBuf,	(const char *)tmpBuf1);
										
										
										// È¡ÐÅºÅÇ¿¶È¼°ÎóÂëÂÊ
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT - j));	k++,j++)
										{
										 		;
										}
										j = j+k-1;
										// ÒÆ¶¯µ½ÐÅºÅÇ¿¶È²ÎÊýµÄÊ×Î»ÖÃ
										for(;	(*(s_GSMcomm.pRecBuf + j) != ',');	j--)
										{
												;
										}
										j = j+1;
										// ¸´ÖÆ
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 2));	k++)
										{
												*(pRssiBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
										}
										*(pRssiBuf + k) = '\0';
										j = j-2;
										for(;	(*(s_GSMcomm.pRecBuf + j) != ',');	j--)
										{
												;
										}
										j = j+1;
										// ¸´ÖÆ
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (k < 2));	k++)
										{
												*(pSLevelBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
										}
										*(pSLevelBuf + k) = '\0';
										if((*pSLevelBuf == '9') && (*(pSLevelBuf + 1) == '9'))
										{
												*pSLevelBuf 		 	= '\0';		// ÐÅºÅÇ¿¶ÈÇå¿Õ
												*(pSLevelBuf + 1) = '\0';						
										}	
										retCode	=	DEF_GSM_NONE_ERR;	
								}	
						}		
						*/			
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QSimCardPrintPro() 
* Description    : ²éÑ¯SIM¿¨Ó¡Ë¢ºÅ(Ò»°ãÎª20Î»µ«ÓÐÐ©¿¨µÄÎ»Êý²»Í¬)         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QSimCardPrintPro	(u8 *pCnumBuf,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u16	k=0,j=0;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						// SIM800 ²»Ö§³Ö¸ÃÃüÁî
						/*
						//	Send "AT^SCID"
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_SCID,	strlen((const char *)AT_SCID),	CommandRecOk,	SCIDTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_SCID_ERR;			
						}
						else
						{
								j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"^SCID: ",s_GSMcomm.ps_commu->RCNT,7);
								if(j == 0xffff)	
								{
										retCode	=	DEF_GSM_UNKNOW_ERR;		
								}
								else
								{
										j= j + 7;
										for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
										{
												*(pCnumBuf +k) = *(s_GSMcomm.pRecBuf + j + k);
										}
										*(pCnumBuf +k) = '\0';
										if((k < 4) || (k > 30))		// ³¤¶ÈÅÐ¶Ï³¤¶È·Ç·¨ÔòÇå¿Õ½ÓÊÕÇøÊý¾Ý
										{
												*pCnumBuf = '\0';
												retCode	=	DEF_GSM_FAIL_ERR;
										}
										else
												retCode	=	DEF_GSM_NONE_ERR;		
								}	
						}	
						*/				
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	

}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QIMSICodePro() 
* Description    : ²éÑ¯ÍøÂçÒÆ¶¯Éè±¸Ç©Êð(IMSI)ºÅÂë(Ò»°ãÎª15Î»)         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QIMSICodePro	(u8 *pImsiBuf,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u16	k=0;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{						
						//	Send "AT+CIMI"
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CIMI,	strlen((const char *)AT_CIMI),	CommandRecOk,	CIMITimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_CIMI_ERR;			
						}
						else
						{								
								for(k = 0;	((*(s_GSMcomm.pRecBuf + k + 2) != '\r') && (*(s_GSMcomm.pRecBuf + k + 2) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
								{
										*(pImsiBuf +k) = *(s_GSMcomm.pRecBuf + k + 2);
								}
								*(pImsiBuf +k) = '\0';		
								if((k < 8) || (k > 30))		// IMSI³¤¶ÈÅÐ¶Ï³¤¶È·Ç·¨ÔòÇå¿Õ½ÓÊÕÇøÊý¾Ý
								{
										*pImsiBuf = '\0';
										retCode	=	DEF_GSM_CIMI_ERR;
								}
								else
										retCode	=	DEF_GSM_NONE_ERR;		
						}					
				}	
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QIMEICodePro() 
* Description    : ²éÑ¯¹ú¼ÊÒÆ¶¯Ì¨Éè±¸Ê¶±ðÂë(IMEI)(Ò»°ãÎª15Î»)        
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QIMEICodePro(u8 *pImeiBuf,	u32 timeout)
{
		u16	k=0;
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_GSN,	strlen((const char *)AT_GSN),	CommandRecOk,	GSNTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_GSN_ERR;		
						}	
						else
						{								
								for(k = 0;	((*(s_GSMcomm.pRecBuf + k + 2) != '\r') && (*(s_GSMcomm.pRecBuf + k + 2) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
								{
										*(pImeiBuf +k) = *(s_GSMcomm.pRecBuf + k + 2);
								}
								*(pImeiBuf +k) = '\0';
								if((k < 8) || (k > 30))		// IMEI³¤¶ÈÅÐ¶Ï³¤¶È·Ç·¨ÔòÇå¿Õ½ÓÊÕÇøÊý¾Ý
								{
										*pImeiBuf = '\0';
										retCode	=	DEF_GSM_GSN_ERR;
								}
								else
										retCode	=	DEF_GSM_NONE_ERR;
						}	
				}
				else
						retCode	=	DEF_GSM_GSN_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QCPINStatePro() 
* Description    :  ²éÑ¯SIM¿¨×´Ì¬       
* Input          : 
* Output         : =0 ready
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QCPINStatePro(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR,ret=0;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ²éÑ¯¿¨×´Ì¬
						ret =BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
						if(ret !=	DEF_GSM_NONE_ERR)
						{
								if(ret == CommandTimeout)
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// Ö»ÓÐ³¬Ê±²ÅÈÏÎªÊÇ´íÎó
								retCode	=	DEF_GSM_CPIN_ERR;
						}
						else
								retCode = DEF_GSM_NONE_ERR;
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QSMSCenterPro() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QSMSCenterPro(u8 *pSmsCenter,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		
		u16 p=0,i=0;
		u8	tmpCSCA[20]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_QCSCA,	strlen((const char *)AT_QCSCA),	CommandRecOk,	CSCATimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_CSCA_ERR;		
						}	
						else
						{
								p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CSCA: \"",s_GSMcomm.ps_commu->RCNT,8);
								if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										p = p + 11;
										memset(tmpCSCA,	'\0',	sizeof(tmpCSCA));
										for(i=0;	*(s_GSMcomm.pRecBuf + p + i) != '\"';	i++)
										{
												tmpCSCA[i]	=		*(s_GSMcomm.pRecBuf + p + i);
										}
										tmpCSCA[i]	=	'\0';

										// Ð´¶ÌÐÅÖÐÐÄºÅÂë
										strcpy((char *)pSmsCenter,	(const char *)tmpCSCA);
										retCode = DEF_GSM_NONE_ERR;	
								}
						}	
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_QSimCardNumPro() 
* Description    : ²éÑ¯±¾»úºÅÂë(SIM¿¨±ØÐëÖ®Ç°Ð´¹ýSIM¿¨ºÅÂë²ÅÄÜ¶Á³ö·ñÔòÎª¿Õ×Ö·û)    
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_QSimCardNumPro(u8	*pSimNum,		u32	timeout)
{
		u16	p=0,k=0,i=0;
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;	

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CNUM,	strlen((const char *)AT_CNUM),	CommandRecOk,	CNUMTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_CNUM_ERR;		
						}	
						else
						{	
								// +CNUM: "","+18501191946",145,7,4
								p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)",\"",s_GSMcomm.ps_commu->RCNT,2);
								if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
								{
										// ·µ»Ø¿Õ×Ö·û´®
										*pSimNum ='\0';
										retCode	=	DEF_GSM_NONE_ERR;										
								}
								else
								{
										p +=2;	// Ö¸ÏòºÅÂë
										for(i=0;	(i<19) && (*(s_GSMcomm.pRecBuf + p) != '"');	i++)
										{
												if(*(s_GSMcomm.pRecBuf + p) != '1')		
														p +=1;	// Ìø¹ý·ÇÊÖ»úºÅÂëÆðÊ¼×Ö¶ÎÈç"+86"
												else
														break;	
										}
										for(k = 0;	((k < 19) && (*(s_GSMcomm.pRecBuf + p + k) != '\"') && (*(s_GSMcomm.pRecBuf + p + k) != '\r') && (*(s_GSMcomm.pRecBuf + p + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
										{
												*(pSimNum + k) = *(s_GSMcomm.pRecBuf + p + k);	// ²»¸´ÖÆ'+'ºÅ							
										}
										*(pSimNum +k) = '\0';
										retCode	=	DEF_GSM_NONE_ERR;
								}
						}	
				}
				else
						retCode =	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_WriteSimCardNumPro() 
* Description    : ÉèÖÃ±¾»úºÅÂë,½«±¾»úºÅÂëÐ´ÈëSIM¿¨µç»°±¡ÖÐ£¬Ö®ºóÍ¨¹ýAT+CNUM¿É²éÑ¯±¾»úºÅÂë
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_WriteSimCardNumPro(u8	*pSimNum,		u32	timeout)
{
		u16	p=0,k=0;
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;	
		u8	tmpBuf[30]={0};

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// Ê×ÏÈÊ¹ÄÜCNUM²éÑ¯±¾»úºÅÂë
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CPBS_ON,	strlen((const char *)AT_CPBS_ON),	CommandRecOk,	CPBSTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_CPBS_ERR;		
						}	
						else
						{	
								// ½«±¾»úºÅÂëÐ´ÈëSIM¿¨µç»°±¡
								// ×é°ü
								//	Send "AT+CPBW=1,\"13145678901\",145"												
								memset(tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CPBW_H);		        						
								strcat((char *)tmpBuf,	(char *)pSimNum);	
								strcat((char *)tmpBuf,	(char *)"\",145\r");
														
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CPBWTimeout) !=	DEF_GSM_NONE_ERR)	
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_CPBW_ERR;			
								}	
								else
										retCode	=	DEF_GSM_NONE_ERR;
						}	
				}
				else
						retCode =	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
*                            					GSMÉÏ²ãÓ¦ÓÃº¯Êý-SIM¿¨³õÊ¼»¯²¿·Ö(ÓÉOS Ó¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMSimCardInitPro() 
* Description    :  SIM¿¨Ïà¹Ø¹¦ÄÜ³õÊ¼»¯º¯Êý     
* Input          : 
*								 : 
*								 : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMSimCardInitPro (u8	*pSimNum,	u32 timeout)
{
		INT8U	err =OS_NO_ERR;		
		s8	retCode=DEF_GSM_NONE_ERR,ret=0;
		u16 p=0,k=0,i=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// ²éÑ¯µ±Ç°SIM¿¨×´Ì¬
						// Send "AT+CPIN?"
						ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
						if(ret !=	DEF_GSM_NONE_ERR)
						{
								if(ret == CommandTimeout)
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// Ö»ÓÐ³¬Ê±²ÅÈÏÎªÊÇ´íÎó
								retCode	=	DEF_GSM_CPIN_ERR;
						}
						else
						{
								// SIM¿¨³õÊ¼»¯ÃüÁî
							  //AT_CMGF,			// ÐèÒªSIM¿¨Ö§³Ö								
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF,	strlen((const char *)AT_CMGF),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_CMGF_ERR;
								}
								else
								{
										//AT_CSCS_GSM,	// ÐèÒªSIM¿¨Ö§³Ö								
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												retCode	=	DEF_GSM_CSCS_ERR;
										}
										else
										{
												//AT_CNMI,			// ÐèÒªSIM¿¨Ö§³Ö								
												if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CNMI,	strlen((const char *)AT_CNMI),	CommandRecOk,	CNMITimeout) !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
														retCode	=	DEF_GSM_CNMI_ERR;
												}
												else
												{
														//AT_CPMS,			// ÐèÒªSIM¿¨Ö§³Ö
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CPMS,	strlen((const char *)AT_CPMS),	CommandRecOk,	CPMSTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																retCode	=	DEF_GSM_CPMS_ERR;
														}
														else
														{
																// ¶ÁÈ¡SIM¿¨ºÅÂë
																if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CNUM,	strlen((const char *)AT_CNUM),	CommandRecOk,	CNUMTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CNUM_ERR;		
																}	
																else
																{	
																		// +CNUM: "","+18501191946",145,7,4
																		p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)",\"",s_GSMcomm.ps_commu->RCNT,2);
																		if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																		{
																				// ·µ»Ø¿Õ×Ö·û´®
																				*pSimNum ='\0';
																				retCode	=	DEF_GSM_NONE_ERR;										
																		}
																		else
																		{
																				p +=2;	// Ö¸ÏòºÅÂë		
																				if(*(s_GSMcomm.pRecBuf + p) == '+')		
																						p +=1;	// Ìø¹ý·ÇÊÖ»úºÅÂëÆðÊ¼×Ö¶ÎÈç"+"																			
																				for(k = 0;	((k < 19) && (*(s_GSMcomm.pRecBuf + p + k) != '\"') && (*(s_GSMcomm.pRecBuf + p + k) != '\r') && (*(s_GSMcomm.pRecBuf + p + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
																				{
																						*(pSimNum + k) = *(s_GSMcomm.pRecBuf + p + k);	// ²»¸´ÖÆ'+'ºÅ							
																				}
																				*(pSimNum +k) = '\0';
																				retCode	=	DEF_GSM_NONE_ERR;
																		}
																}	
														}
												}
										}
								}
						}											
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;
		}
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
*                            					GSMÉÏ²ãÓ¦ÓÃº¯Êý-ÍøÂç²¿·Ö(ÓÉOS Ó¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  :									  BSP_GPRSFroceBreakSet() 
* Description    :  ÊÖ¶¯Ç¿ÖÆÉèÖÃÎªÍøÂç¶Ï¿ª×´Ì¬£¬´¥·¢ÍøÂç×Ô¶¯ÖØÐÂÁ¬½Ó      
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GPRSFroceBreakSet(void)
{
		s_GSMcomm.GSMComStatus	=	GSM_ONCMD;
		BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);		// ÖÃÍøÂç¶Ï¿ª×´Ì¬
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GPRSResetPro() 
* Description    :  ¸´Î»GPRSÍøÂçÊ¹ÆäÖØÐÂ¸½×Åµ½GPRSÍøÂçÉÏ(Èç¹û¶à´ÎÁ½Íø³¢ÊÔÊ§°ÜÐèÒª¸´Î»GPRSÍøÂç)        
* Input          : *pApn	: APN×Ö·û´®»º³åÇøÖ¸Õë£¬²»¿ÉÒÔÎª¿Õ
*								 : *pUser : µÇÂ¼ÓÃ»§Ãû×Ö·û´®Ö¸Õë£¬¿ÉÒÔÎª¿Õ
*								 : *pPass : µÇÂ¼ÃÜÂë×Ö·û´®Ö¸Õë£¬¿ÉÒÔÎª¿Õ
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GPRSResetPro(u8 *pApn,	u8	*pUser,	 u8  *pPass,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;		
		u8 i=0,tmpBuf[80]={0},	cregOKFlag=0,	cgattOKFlag=0;
		s8	tmpRen=0,retCode=DEF_GSM_NONE_ERR,ret=0;
		u16 j=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if(*pApn == '\0')
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{			
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								// ²éÑ¯µ±Ç°SIM¿¨×´Ì¬
								// Send "AT+CPIN?"
								ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
								if(ret !=	DEF_GSM_NONE_ERR)
								{
										if(ret == CommandTimeout)
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// Ö»ÓÐ³¬Ê±²ÅÈÏÎªÊÇ´íÎó
										retCode	=	DEF_GSM_CPIN_ERR;
								}
								else
								{
										// ²éÑ¯µ±Ç°ÍøÂç×¢²á×´Ì¬ÊÇ·ñ¿ÉÓÃ	
										//	Send "AT+CREG?"
										if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CREG,	strlen((const char *)AT_CREG),	CommandRecOk,	CREGTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ²éÑ¯ÍøÂç×¢²á´íÎó
												retCode	=	DEF_GSM_CREG_ERR;
										}
										else
										{
												j= 0;
												j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CREG: *,*",s_GSMcomm.ps_commu->RCNT,10);
												if(j != 0xffff)	
												{
														j =j+9; // Ö¸Ïò×¢²á×´Ì¬
														if((*(s_GSMcomm.pRecBuf + j) == '1') || (*(s_GSMcomm.pRecBuf + j) == '5'))
																cregOKFlag =1;	// ×¢²á³É¹¦£¡£¡			
														else
																cregOKFlag =0;
												}
												else
														cregOKFlag =0;
										}
										
										// ²éÑ¯µ±Ç°GPRS¸½×ÅÇé¿ö
										if(cregOKFlag == 1)
										{												
												if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CGATT,	strlen((const char *)AT_CGATT),	CommandRecOk,	CGATTTimeout) !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// GPRS²éÑ¯´íÎó
														retCode	=	DEF_GSM_CGATT_ERR;
												}
												else
												{
														j= 0;
														j= StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CGATT: *",s_GSMcomm.ps_commu->RCNT,9);
														if(j != 0xffff)	
														{
																j =j+8; // Ö¸Ïò¸½×Å×´Ì¬
																if(*(s_GSMcomm.pRecBuf + j) == '1')
																		cgattOKFlag =1;	// ×¢²á³É¹¦£¡£¡	
																else
																		cgattOKFlag =0;	
														}
														else
																cgattOKFlag =0;
												}
										}
										
										// ¿ªÊ¼¼¤»îGPRS
										if((cregOKFlag == 0) || (cgattOKFlag == 0))
										{		
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó(2015-3-10£¬²âÊÔ·¢ÏÖÓÐÊ±CGATT³¤Ê±¼ä·µ»Ø0×´Ì¬²»¸Ä±ä)
												if(cregOKFlag == 0)
														retCode	=	DEF_GSM_CREG_ERR;			
												else
														retCode	=	DEF_GSM_CGATT_ERR;	
										}
										else
										{											
												// Ê×ÏÈ¶Ï¿ªµ±Ç°PDPÉÏÏÂÎÄ(³¢ÊÔ3´Î)
												//	Send "AT+CIPSHUT\r"
												tmpRen =0;
												for(i=0;	i<3;	i++)
												{
														tmpRen	=	 BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CIPSHUT,	strlen((const char *)AT_CIPSHUT),	RecShutCpl,	CIPSHUTTimeout);
														if(tmpRen == DEF_GSM_NONE_ERR)	
																break;
												}
												if(i >=	3)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
														retCode	=	DEF_GSM_CIPSHUT_ERR;
												}
												else
												{																											
														s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ÖÃGSM¶Ë¿Ú×´Ì¬
														BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	// ¸üÐÂµ±Ç°ÁªÍø×´Ì¬Îª¶Ï¿ªÁ¬½Ó
																																							
														// ¿ªÊ¼GPRS¼¤»îÁ÷³Ì
														// Ê×ÏÈÊ¹ÄÜ¶àÁ´Â·Ä£Ê½
														//	Send "AT+CIPMUX=1\r"
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIPMUX_1,	strlen((const char *)AT_CIPMUX_1),	CommandRecOk,	CIPMUXTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																retCode	=	DEF_GSM_CIPMUX_ERR;
														}		
														else
														{
																// ÅäÖÃ½ÓÊÕÊý¾ÝÄ£Ê½ÊÖ¶¯Ä£Ê½£¨¸ÃÖ¸Áî±ØÐëÔÚGPRSÉÏÏÂÎÄ¼¤»îÇ°ÅäÖÃ£©
																//	Send "AT+CIPRXGET=1\r"
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIPRXGET_1,	strlen((const char *)AT_CIPRXGET_1),	CommandRecOk,	CIPRXGET1Timeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CIPRXGET1_ERR;
																}		
																else
																{
																		// ÅäÖÃ½ÓÊÕURC¸ñÊ½
																		//	Send "AT+CIPHEAD=1\r"
																		if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIPHEAD_1,	strlen((const char *)AT_CIPHEAD_1),	CommandRecOk,	CIPHEADTimeout) !=	DEF_GSM_NONE_ERR)
																		{
																				BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																				retCode	=	DEF_GSM_CIPHEAD_ERR;
																		}
																		else
																		{																			
																				// ÅäÖÃAPN,USER,PASS
																				//	Send "AT+CSTT="CMNET","",""\r"
																				memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																				strcat((char *)tmpBuf,	(char *)AT_CSTT_H);
																				strcat((char *)tmpBuf,	(char *)pApn);																		
																				strcat((char *)tmpBuf,	(char *)"\",\"");
																				strcat((char *)tmpBuf,	(char *)pUser);																		
																				strcat((char *)tmpBuf,	(char *)"\",\"");
																				strcat((char *)tmpBuf,	(char *)pPass);																		
																				strcat((char *)tmpBuf,	(char *)"\"\r");
																				
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CSTTTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																						retCode	=	DEF_GSM_CSTT_ERR;
																				}
																				else
																				{
																						// ¼¤»îPDPÉÏÏÂÎÄ
																						//	Send "AT+CIICR\r"
																						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CIICR,	strlen((const char *)AT_CIICR),	CommandRecOk,	CIICRTimeout) !=	DEF_GSM_NONE_ERR)
																						{
																								//BSP_GSM_ERR_Add(&err_Gsm.cnnErrorTimes,	1);		// ÍøÂç´íÎóÀÛ¼Ó
																								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	20);		// 2015-11-19¸Ã´íÎóºÏ²¢µ½Í¨ÓÃ´íÎóÖÐ
																								retCode	=	DEF_GSM_CIICR_ERR;
																						}
																						else
																						{	
																								// »ñÈ¡±¾µØIP£¨¸ÃÖ¸Áî²»È«×Ö½ÚÆ¥Åä½á¹ûÂë£©
																								//	Send "AT+CIFSR\r"
																								if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CIFSR,	strlen((const char *)AT_CIFSR),	RecCIFSRAck,	CIFSRTimeout) !=	DEF_GSM_NONE_ERR)
																								{
																										//BSP_GSM_ERR_Add(&err_Gsm.cnnErrorTimes,	1);		// ÍøÂç´íÎóÀÛ¼Ó
																										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// 2015-11-19¸Ã´íÎóºÏ²¢µ½Í¨ÓÃ´íÎóÖÐ
																										retCode	= DEF_GSM_CIFSR_ERR;	
																								}
																								else
																										retCode = DEF_GSM_NONE_ERR;																							
																						}							
																				}																																		
																		}		
																}																						
														}	
												}
										}
								}
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPConnectPro() 
* Description    : sCnn		: Í¨µÀºÅ£¨¡®0¡¯~¡®5¡¯¹²6Â·Á¬½Ó£¬×Ö·ûÀàÐÍ£©
*								 : *pIp		: IPµØÖ·×Ö·û´®Ö¸Õë
*								 : *pPort	: ¶Ë¿ÚºÅ×Ö·û´®Ö¸Õë         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPConnectPro	(u8	sCnn,	u8 *pIp, u8 *pPort,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR,tmpRen=0;
		u8	tmpBuf[60]="";		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if(((sCnn - 0x30) > DEF_GPRS_CNN_MAX) || (*pIp == '\0') || (*pPort == '\0'))
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{				
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{																																	
								// ×é°ü
								//	Send "AT+CIPSTART=0,\"TCP\",\"218.249.201.35\",\"13000\"\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPSTART_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);										
								strcat((char *)tmpBuf,	(char *)",\"TCP\",\"");
								strcat((char *)tmpBuf,	(char *)pIp);
								strcat((char *)tmpBuf,	(char *)"\",\"");
								strcat((char *)tmpBuf,	(char *)pPort);
								strcat((char *)tmpBuf,	(char *)"\"\r");
												
								tmpRen = BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecConnectCpl,	CIPSTARTTimeout); 
								if(tmpRen !=	DEF_GSM_NONE_ERR)
								{
										// ÁªÍø´íÎó´¦Àí	
										BSP_GSM_ERR_Add(&err_Gsm.cnnErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										if(BSP_GSM_ERR_Que(&err_Gsm.cnnErrorTimes) >= DEF_GSM_CNN_TIMES)
										{
												BSP_GSM_ERR_Clr(&err_Gsm.cnnErrorTimes);		// ´íÎóÇå¿Õ	
												
												// ÐèÒªÖØÐÂ¸½×ÅGPRSÍøÂç(AT+CGATT=0£¬AT+CGATT=1)
												tmpRen = BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CGATT_0,	strlen((const char *)AT_CGATT_0),	CommandRecOk,	CGATT0Timeout); 
												if(tmpRen !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
														retCode	=	DEF_GSM_CGATT_ERR;				
												}
												else
												{
														tmpRen = BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CGATT_1,	strlen((const char *)AT_CGATT_1),	CommandRecOk,	CGATT1Timeout); 
														if(tmpRen !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																retCode	=	DEF_GSM_CGATT_ERR;				
														}
												}
										}	

										s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ÖÃGSM¶Ë¿Ú×´Ì¬
										BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);
										retCode	=	DEF_GSM_CIPSTART_ERR;
								}
								else
								{										
										BSP_GSM_ERR_Clr(&err_Gsm.cnnErrorTimes);		// ´íÎóÇå¿Õ				
										s_GSMcomm.GSMComStatus	=	GSM_ONLINE;	// ÖÃGSM¶Ë¿Ú×´Ì¬
										BSP_GSM_SetFlag(DEF_GDATA_FLAG);																									
										retCode	=	DEF_GSM_NONE_ERR;
								}
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPCloseNetPro() 
* Description    : sCnn		: Í¨µÀºÅ£¨¡®0¡¯~¡®5¡¯¹²6Â·Á¬½Ó£¬×Ö·ûÀàÐÍ£©      
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPCloseNetPro	(u8	sCnn,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;
		u8	tmpBuf[20]="";

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{			
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{																							
								// ×é°ü
								//	Send "AT+CIPCLOSE=n\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPCLOSE_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);	
								strcat((char *)tmpBuf,	(char *)"\r");
												
								if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCloseCpl,	CIPCLOSETimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_CIPCLOSE_ERR;
										
										// ¿ÉÒÔ¿¼ÂÇÈç¹û¼¸´Î¶ÏÍø²»³É¹¦Ö±½Ó¿¼ÂÇ¶Ï¿ªPDPÁ¬½Ó
								}
								else
								{
										s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ÖÃGSM¶Ë¿Ú×´Ì¬
										BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	
										retCode	=	DEF_GSM_NONE_ERR;		
								}				
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPShutNetPro() 
* Description    :      
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPShutNetPro	(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{		
				if(BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CIPSHUT,	strlen((const char *)AT_CIPSHUT),	RecShutCpl,	CIPSHUTTimeout) != DEF_GSM_NONE_ERR)
				{
						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
						retCode	=	DEF_GSM_CIPSHUT_ERR;
				}
				else
				{
						s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ÖÃGSM¶Ë¿Ú×´Ì¬
						BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	
						retCode	=	DEF_GSM_NONE_ERR;		
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPSendData() 
* Description    : sCnn		: Í¨µÀºÅ£¨¡®0¡¯~¡®5¡¯¹²6Â·Á¬½Ó£¬×Ö·ûÀàÐÍ£©
*								 : *pBuf	: ·¢ËÍÊý¾Ý»º³åÇøÖ¸Õë     
*								 : len		: ·¢ËÍ³¤¶È    
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPSendData(u8	sCnn,	u8	*pBuf,	u16	len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	i=0,	tmpBuf[30]="",	lenBuf[8]="",	ready2Send=0;
		s8	tmpRen=0,	retCode=DEF_GSM_NONE_ERR;	
		u16 j=0,k=0,avaLen=0,posi=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{					
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	==	GSM_ONLINE))
						{								
								// ²éÑ¯¿É·¢ËÍµÄÊý¾Ý³¤¶È
								// Send "AT+CIPSEND?\r"
								// ·µ»Ø¸ñÊ½Îª£¨¶àÂ·Ä£Ê½£©£º
								// +CIPSEND: 0,1460
								// +CIPSEND: 1,1460
								// +CIPSEND: 2,0
								// +CIPSEND: 3,0
								// +CIPSEND: 4,0
								// +CIPSEND: 5,0
								// 
								// OK
								/*
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CIPSENDQ,	strlen((const char *)AT_CIPSENDQ),	CommandRecOk,	CIPSENDQTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	= DEF_GSM_CIPSENDQ_ERR;			
								}
								else
								{										
										for(i=0;	i<=DEF_GPRS_CNN_MAX;	i++)
										{
												j=0;
												j= StringSearch((u8 *)(s_GSMcomm.pRecBuf + posi),(u8 *)"+CIPSEND: *,*",s_GSMcomm.ps_commu->RCNT,13);
												if(j != 0xffff)		
											  {
											  		j =j+10;	// È¡Í¨µÀºÅ
											  		if(sCnn == *(s_GSMcomm.pRecBuf + j))
											  		{
											  				j =j+2;	// È¡¿É·¢ËÍÊý¾Ý´óÐ¡
											  				memset((u8 *)lenBuf,	'\0',	sizeof(lenBuf));
											  				for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < 5));	k++)
																{
																		lenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
																}
																lenBuf[k]='\0';	
											  			 	avaLen = atoi((const char *)lenBuf);
											  			 	if(len < avaLen)
																		ready2Send =1;
											  			 	break;
											  		}
											  		else
											  		{
											  				// Æ«ÒÆ¼ìË÷Ö¸Õëµ½ÏÂÒ»ÐÐ
											  				for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k < s_GSMcomm.ps_commu->RCNT));	k++)
																{
																		;
																}
																posi = posi + k + 2; // ÒÆ¶¯Êý¾Ý½ÓÊÕÇøbufÖ¸Õë											  			
											  		}
											  }
											  else
											  		break;
										}
										if(i > DEF_GPRS_CNN_MAX)
										{
												;		// Òì³£´íÎó£¬Í¨µÀ¶¼²»Æ¥Åä£¡£¡£¡£¡£¡														
										}
								}
								*/
								
								ready2Send =1;	// È¡Ïû·¢ËÍ»º³åÇø²éÑ¯	
								// ¿ª·¢·¢ËÍÊý¾Ý//////////////////////////////////////////////
								if(ready2Send != 1)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// Òì³£´íÎó£¬Í¨µÀ¶¼²»Æ¥Åä£¡£¡£¡£¡£
										s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	
										BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);					// ¶Ï¿ªÍøÂç,ÖÃ×´Ì¬ÖØÐÂÁ¬½Ó·þÎñÆ÷										¡				
										retCode =	DEF_GSM_UNKNOW_ERR;
								}
								else
								{
										// ÕâÀïÐèÒªÊ×ÏÈÈ·ÈÏ¿ÉÒÔ·¢ËÍµÄÊý¾Ý³¤¶ÈÊÇ¶àÉÙ
										// ×é°ü								
										//	Send "AT+CIPSEND=n,len\r"									
										memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
										memset((u8 *)lenBuf,	'\0',	sizeof(lenBuf));
										strcat((char *)tmpBuf,	(char *)AT_CIPSEND_H);
										strcat((char *)tmpBuf,	(char *)&sCnn);
										strcat((char *)tmpBuf,	(char *)",");
										sprintf((char *)lenBuf,"%d",	len);
										strcat((char *)tmpBuf,	(char *)lenBuf);
										strcat((char *)tmpBuf,	(char *)"\r");
										
										tmpRen = BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecTCPSendAck,	CIPSENDATimeout); 
										if(tmpRen !=	DEF_GSM_NONE_ERR)
										{
												// ·þÎñÆ÷¶Ï¿ª,ÖÃ×´Ì¬ÖØÐÂÁ¬½Ó·þÎñÆ÷
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												s_GSMcomm.GSMComStatus	=	GSM_ONCMD;
												BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);
												retCode	= DEF_GSM_CIPSENDA_ERR;			
										}
										else
										{
												// SEND DATA
												tmpRen = BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)pBuf,	len,	RecTCPSendCpl,	CIPSENDFTimeout);	
												if(tmpRen	!=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó														
														s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	
														BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);	// ·þÎñÆ÷¶Ï¿ª,ÖÃ×´Ì¬ÖØÐÂÁ¬½Ó·þÎñÆ÷
														retCode	= DEF_GSM_CIPSENDF_ERR;
												}
												else
												{
														// ·¢ËÍÊý¾Ý³É¹¦½«¸´Î»Í¨ÓÃ´íÎóÀÛ¼Ó¼ÆÊýÆ÷
														BSP_GSM_ERR_Clr(&err_Gsm.gsmErrorTimes);
														retCode = DEF_GSM_NONE_ERR;	
												}						
										}
								}
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPRxdData() 
* Description    : sCnn		: Í¨µÀºÅ£¨¡®0¡¯~¡®5¡¯¹²6Â·Á¬½Ó£¬×Ö·ûÀàÐÍ£©¸ÄÐ´tcpReadMuxtex±äÁ¿×´Ì¬
*								 : *pBuf	: ½ÓÊÕÊý¾Ý»º³åÇøÖ¸Õë     
*								 : len		: ½ÓÊÕ³¤¶È             
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPRxdData(u8 sCnn,	u8	*pBuf,	u16	*len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8  tmpBuf[30]="",tmpLenBuf[8]="";
		s8	retCode=DEF_GSM_NONE_ERR;	
		u16	j=0,k=0,p=0,tmpLen=DEF_GPRS_RXDONE_SIZE,	nrLen=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{												
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	== GSM_ONLINE))
						{						
								// ×é°üÖ±½Ó¶ÁÈ¡Êý¾Ý
								//	Send "AT+CIPRXGET=2,1,1000\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPRXGET_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);	
								strcat((char *)tmpBuf,	(char *)",");	
								sprintf((char *)tmpLenBuf,	"%d",	tmpLen);	// ÓÉÍ·ÎÄ¼þºê¶¨Òå¾ö¶¨DEF_GPRS_RXDONE_SIZE						
								strcat((char *)tmpBuf,	(char *)tmpLenBuf);
								strcat((char *)tmpBuf,	(char *)"\r");		
																				
								tcpReadMuxtex =1;	// ÖÃ»¥³â±êÖ¾,Ê¹URC¼ì²â¹ÒÆð
								
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CIPRXGET2Timeout) != DEF_GSM_NONE_ERR) 
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_CIPGET2_ERR;	
								}
								else
								{
										j = 0;
										j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 2,*",s_GSMcomm.ps_commu->RCNT,14);
										if(j == 0xffff)		
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												retCode	= DEF_GSM_UNKNOW_ERR;											
										}
										else
										{
												// È¡¶Áµ½µÄÊý¾Ý³¤¶È(+CIPRXGET: 2,0,6,0)
												memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
												j = j + 15;
												for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
												{
														tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
												}
												tmpLenBuf[k] = '\0';
												tmpLen = atoi((const char *)tmpLenBuf);
												
												// È¡Ê£Óà¶ÁÈ¡µÄ×Ö½Ú³¤¶È
												j = j + k + 1;	// Ìø¹ý','
												for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
												{
														tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);	
												}
												tmpLenBuf[k] = '\0';
												nrLen = atoi((const char *)tmpLenBuf);
												if(nrLen != 0)
												{
														#if(DEF_GSMINFO_OUTPUTEN > 0)
														if(dbgInfoSwt & DBG_INFO_GPRS)
																myPrintf("[GPRS]: Read abnormal, %d byte need to be read!\r\n",	nrLen);
														#endif 											
												}
												
												// ¸´ÖÆÊý¾Ý
												j = j + k + 2;	// Ìø¹ý»Ø³µ»»ÐÐÖ¸ÏòÊý¾ÝÇø
												// ÅÐ¶Ï½áÊøÌõ¼þÓÐ²»ÄÜÎª0d 0aÈç¹ûÎªhexÊý¾Ý¸ñÊ½£¬Êý¾ÝÁ÷ÖÐ¿ÉÄÜÓÐ0d 0aËùÒÔ³öÏÖÊý¾Ý¸´ÖÆ²»È«Çé¿ö
												for(k =0;	((k < tmpLen) && (k < DEF_GPRS_RXDONE_SIZE));	k++)
												{
														*(pBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
												}
												*(pBuf + k) = '\0';	
												*len = tmpLen;	
												retCode	= DEF_GSM_NONE_ERR;
										}					
								}	
								// ¶ÁÈ¡Íê±ÏºóÇå³ý½ÓÊÕÊý¾ÝÄÚÈÝ,·ÀÖ¹URCÎó¼ì²â				
								tcpReadMuxtex =0;	// Çå±êÖ¾,Ê¹ÄÜÖØÐÂURC¼ì²â						
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_TCPRxdDataUpdata() 
* Description    : sCnn		: Í¨µÀºÅ£¨¡®0¡¯~¡®5¡¯¹²6Â·Á¬½Ó£¬×Ö·ûÀàÐÍ£©¸ÄÐ´tcpReadMuxtex±äÁ¿×´Ì¬,ÓÃÓÚÉý¼¶¹ý³ÌÖÐ´óÁ¿Êý¾ÝµÄ½ÓÊÕ
*								 : *pBuf	: ½ÓÊÕÊý¾Ý»º³åÇøÖ¸Õë     
*								 : len		: ½ÓÊÕ³¤¶È             
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_TCPRxdDataUpdata(u8 sCnn,	u8	*pBuf,	u16	*len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8  tmpBuf[30]="",tmpLenBuf[8]="";
		s8	retCode=DEF_GSM_NONE_ERR;	
		u16	j=0,k=0,p=0,tmpLen=DEF_GPRS_RXDONE_SIZE,	nrLen=0,	tmpRxdCnt=0,	u16Times=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if((sCnn - 0x30) > DEF_GPRS_CNN_MAX)
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{												
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	== GSM_ONLINE))
						{	
								// ÏÈ²éÑ¯ÊÕµ½µÄÊý¾Ý³¤¶È
								//	Send "AT+CIPRXGET=4,id\r"
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CIPRXGET4_H);
								strcat((char *)tmpBuf,	(char *)&sCnn);	
								strcat((char *)tmpBuf,	(char *)"\r");	
								if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CIPRXGET2Timeout) != DEF_GSM_NONE_ERR) 
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_CIPGET2_ERR;	
								}
								else
								{										
										j = 0;
										j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 4,*",s_GSMcomm.ps_commu->RCNT,14);
										if(j == 0xffff)		
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												retCode	= DEF_GSM_UNKNOW_ERR;													
										}
										else
										{
												// È¡¶Áµ½µÄÊý¾Ý³¤¶È(+CIPRXGET: 4,0,len)
												tmpLen =0;
												memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));												
												j = j + 15;
												for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
												{
														tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
												}
												tmpLenBuf[k] = '\0';
												tmpLen = atoi((const char *)tmpLenBuf);
												if(tmpLen < 1040)
														gsmDelayMS(200);
												
												// ×é°üÖ±½Ó¶ÁÈ¡Êý¾Ý
												//	Send "AT+CIPRXGET=2,1,1000\r"
												tmpLen=DEF_GPRS_RXDONE_SIZE;	// Ð´ÈëÇëÇó¶ÁÈ¡³¤¶È
												memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
												memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
												strcat((char *)tmpBuf,	(char *)AT_CIPRXGET_H);
												strcat((char *)tmpBuf,	(char *)&sCnn);	
												strcat((char *)tmpBuf,	(char *)",");	
												sprintf((char *)tmpLenBuf,	"%d",	tmpLen);	// ÓÉÍ·ÎÄ¼þºê¶¨Òå¾ö¶¨DEF_GPRS_RXDONE_SIZE						
												strcat((char *)tmpBuf,	(char *)tmpLenBuf);
												strcat((char *)tmpBuf,	(char *)"\r");		
																									
												tcpReadMuxtex =1;	// ÖÃ»¥³â±êÖ¾,Ê¹URC¼ì²â¹ÒÆð												
												if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CIPRXGET2Timeout) != DEF_GSM_NONE_ERR) 
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
														retCode	=	DEF_GSM_CIPGET2_ERR;	
												}
												else
												{
														// ½ÓÊÕ½áÊøÅÐ¶Ï´¦Àí(·ÀÖ¹½ÓÊÕHEXÊý¾ÝÖÐº¬¡®OK¡¯×Ö·û´®µ¼ÖÂÆ¥Åä³É¹¦ÌáÇ°ÍË³ö)
														tmpRxdCnt =s_GSMcomm.ps_commu->RCNT;	// ¼ÇÂ¼µ±Ç°½ÓÊÕµ½×Ö½Ú³¤¶È
														u16Times =250;	// ×î´óµÈ´ý³¬Ê±500ms
														while(u16Times--)
														{
																gsmDelayMS(2);
																if(tmpRxdCnt == s_GSMcomm.ps_commu->RCNT)
																		break;
																else
																		tmpRxdCnt = s_GSMcomm.ps_commu->RCNT;	// ¸üÐÂ¼ÇÂ¼³¤¶È
														}
														j = 0;
														j = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 2,*",s_GSMcomm.ps_commu->RCNT,14);
														if(j == 0xffff)		
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																retCode	= DEF_GSM_UNKNOW_ERR;											
														}
														else
														{
																// È¡¶Áµ½µÄÊý¾Ý³¤¶È(+CIPRXGET: 2,0,6,0)
																tmpLen =0;
																memset((u8 *)tmpLenBuf,	'\0',	sizeof(tmpLenBuf));
																j = j + 15;
																for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != ',') && (*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
																{
																		tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);
																}
																tmpLenBuf[k] = '\0';
																tmpLen = atoi((const char *)tmpLenBuf);
																
																// È¡Ê£Óà¶ÁÈ¡µÄ×Ö½Ú³¤¶È
																j = j + k + 1;	// Ìø¹ý','
																for(k=0;	((*(s_GSMcomm.pRecBuf + j + k) != '\r') && (*(s_GSMcomm.pRecBuf + j + k) != '\n') && (k <= 4));	k++)
																{
																		tmpLenBuf[k] = *(s_GSMcomm.pRecBuf + j + k);	
																}
																tmpLenBuf[k] = '\0';
																nrLen = atoi((const char *)tmpLenBuf);
																if(nrLen != 0)
																{
																		#if(DEF_GSMINFO_OUTPUTEN > 0)
																		if(dbgInfoSwt & DBG_INFO_GPRS)
																				myPrintf("[GPRS]: Read abnormal, %d byte need to be read!\r\n",	nrLen);
																		#endif 											
																}
																
																// ¸´ÖÆÊý¾Ý
																j = j + k + 2;	// Ìø¹ý»Ø³µ»»ÐÐÖ¸ÏòÊý¾ÝÇø
																// ÅÐ¶Ï½áÊøÌõ¼þÓÐ²»ÄÜÎª0d 0aÈç¹ûÎªhexÊý¾Ý¸ñÊ½£¬Êý¾ÝÁ÷ÖÐ¿ÉÄÜÓÐ0d 0aËùÒÔ³öÏÖÊý¾Ý¸´ÖÆ²»È«Çé¿ö
																for(k =0;	((k < tmpLen) && (k < DEF_GPRS_RXDONE_SIZE));	k++)
																{
																		*(pBuf + k) = *(s_GSMcomm.pRecBuf + j + k);
																}
																*(pBuf + k) = '\0';	
																*len = tmpLen;	
																
																// ÅÐ¶ÏÊÇ·ñ»¹ÓÐÊý¾ÝÎ´¶ÁÈ¡
																if(nrLen != 0)
																		retCode = DEF_GSM_READ_AGAIN;
																else
																{
																		// ÔÙ´ÎÅÐ¶ÏÊÇ·ñÓÐÊý¾Ý½ÓÊÕµ½
																		j = j + k;	// Ìø¹ýÊý¾ÝÇø
																		if((s_GSMcomm.ps_commu->RCNT -j) > 0)
																		{
																				p = StringSearch((u8 *)(s_GSMcomm.pRecBuf+j),(u8 *)"+CIPRXGET: 1,*",(s_GSMcomm.ps_commu->RCNT-j),14);		
																				if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
																				{
																						*(s_GSMcomm.pRecBuf + p) = '\0';										// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â
																						#if(DEF_GSMINFO_OUTPUTEN > 0)
																						if(dbgInfoSwt & DBG_INFO_GPRS)
																								myPrintf("[GPRS]: Rxd data end with \"+CIPRXGET: 1,n\"!\r\n",	nrLen);
																						#endif 																			
																						retCode = DEF_GSM_READ_AGAIN;
																				}
																				else
																						retCode	= DEF_GSM_NONE_ERR;
																		}
																		else
																				retCode	= DEF_GSM_NONE_ERR;
																}
														}					
												}	
												// ¶ÁÈ¡Íê±ÏºóÇå³ý½ÓÊÕÊý¾ÝÄÚÈÝ,·ÀÖ¹URCÎó¼ì²â
												s_GSMcomm.ps_commu->RCNT =0;
												tcpReadMuxtex =0;	// Çå±êÖ¾,Ê¹ÄÜÖØÐÂURC¼ì²â		
										}
								}
						}
						else
								retCode =	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
*                            					GSMÉÏ²ãÓ¦ÓÃº¯Êý-¶ÌÐÅ²¿·Ö(ÓÉOS Ó¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSSend() 
* Description    : ËùÓÐ¶ÌÐÅ½«ÒÔUSC2±àÂë·½Ê½·¢ËÍ£¬º¯ÊýÄÚ²¿»á½«·¢ËÍµÄASCIIÂë×ª³ÉUSC2Âë(Ä¿µÄ±ÜÃâGSM×Ö·û¼¯µÄÒ»Ð©ÌØÊâ
*									 ×Ö·ûÈç£º@,$·þÎñÆ÷ÎÞ·¨Ê¶±ð) [×¢]Ä¬ÈÏ²»·¢ËÍ¶ÌÐÅÖÐÐÄºÅÂë³ý·Ç²ÎÊýÊÖ¶¯ÅäÖÃÁËÖÐÐÄºÅÂë       
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_SMSSend(u8	smsMode,	u8	*pSmsCenter,	u8	*pPhoneCode,	u8	*pData,	u16	len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[200]="", tmpUSC2Buf[1024]="",	lenBuf[30]="";	
		u16	tmplen;
		s8	retCode=DEF_GSM_NONE_ERR,ret=0;
		u8 *pPhone=pPhoneCode;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if((*pPhoneCode == '\0') || (len == 0))
				{
						retCode =	DEF_GSM_NONE_ERR;	
				}
				else
				{							
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>= GSM_ONCMD))
						{
								// ²éÑ¯µ±Ç°SIM¿¨×´Ì¬
								// Send "AT+CPIN?"
								ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
								if(ret !=	DEF_GSM_NONE_ERR)
								{
										if(ret == CommandTimeout)
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// Ö»ÓÐ³¬Ê±²ÅÈÏÎªÊÇ´íÎó
										retCode	=	DEF_GSM_CPIN_ERR;
								}
								else
								{
										if(smsMode	==	DEF_GSM_MODE)	
										{
												if(len	>	DEF_SMS_USC2BYTE_SIZE)
												{
														retCode	=	DEF_GSM_NONE_ERR;				
												}
												else
												{
														//	Text Mode
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_1,	strlen((const char *)AT_CMGF_1),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{						
																//	"USC2" Mode ±ÜÃâÌØÊâ×Ö·ûGSMÄ£Ê½·þÎñÆ÷ÊÕµ½²»Ê¶±ð
																//	Send AT+CSCS
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CSCS_ERR;
																}
																else
																{													
																		memset((u8 *)tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);
																		strcat((char *)tmpBuf,	(char *)"\"");	// copy "
																		tmplen = ChangeASCII2USC2 (pPhoneCode,	strlen((const char *)pPhoneCode),	tmpUSC2Buf);
																		strcat((char *)tmpBuf,	(char *)tmpUSC2Buf);
																		strcat((char *)tmpBuf,	(char *)"\"\r");																	
																		if(tmplen == 0xffff)
																				retCode	=	DEF_GSM_UNKNOW_ERR;		
																		else
																		{																																
																				//	Send AT+CMGS
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																						retCode	=	DEF_GSM_CMGSACK_ERR;		
																				}
																				else
																				{																
																						memset((u8 *)tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																						tmplen = ChangeASCII2USC2 (pData,	len,	tmpUSC2Buf);
																						strcat((char *)tmpUSC2Buf,	(char *)"\x1a");	
																						if(tmplen == 0xffff)
																								retCode	=	DEF_GSM_UNKNOW_ERR;	
																						else
																						{
																								//	Send SMS data
																								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																								{
																										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																										retCode	=	DEF_GSM_CMGSCPL_ERR;
																								}
																						}
																				}	
																		}
																}	
																//	Send AT+CSCS="GSM" ÇÐ»Øµ½GSM±àÂë·½Ê½·ÀÖ¹ÔÙ´ÎÅäÖÃÍøÂç²ÎÊýÊ±±àÂë¼¯¸ÄÎªUCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CSCS_ERR;
																}	
														}
												}			
										}
										else
										{
												if(len	> (DEF_SMS_USC2BYTE_SIZE*4))
												{
														retCode	=	DEF_GSM_NONE_ERR;		// Èç¹û³¬³öSMS·¢ËÍ³¤¶ÈÔò±»ÈÏÎª·¢ËÍ³É¹¦ 						
												}
												else
												{
														//  PDU ·¢ÖÐÎÄ´úÂë
														//	PDU	Mode						
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_0,	strlen((const char *)AT_CMGF_0),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{								
																// 	"UCS2" Mode 
																//	Send AT+CSCS		
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CSCS_ERR;		
																}
																else
																{
																		if(*pPhone == '+')
																				pPhone ++;		 // Ìø¹ýµç»°ºÅÂëÖÐµÄ'+'²¿·Ö(PDUÊý¾ÝÖÐÎÞ"+"ºÅ)
																						
																		//	Make Pdu data and return the len of data
																		tmplen = PDUMakeUpData(DEF_COM_PHONE, (u8	*)pSmsCenter,	(u8	*)pPhone, (u8	*)pData, len,	(u8	*)tmpUSC2Buf);			
																		sprintf((char *)lenBuf,	"%d",	tmplen);
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);
																		strcat((char *)tmpBuf,	(char *)lenBuf);
																		strcat((char *)tmpBuf,	(char *)"\r");
																		
																		if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																		{
																				BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																				retCode	=	DEF_GSM_CMGSACK_ERR;				
																		}
																		else
																		{
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																						retCode	=	DEF_GSM_CMGSCPL_ERR;				
																				}		
																		}
																}
																//	Send AT+CSCS="GSM" ÇÐ»Øµ½GSM±àÂë·½Ê½·ÀÖ¹ÔÙ´ÎÅäÖÃÍøÂç²ÎÊýÊ±±àÂë¼¯¸ÄÎªUCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CSCS_ERR;
																}	
														}
												}
										}
								}
						}
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSSendAlarm() 
* Description    : ·¢ËÍ±¨¾¯¶ÌÐÅ¸Ã¶ÌÐÅ»áÔÚÔ­±¨¾¯µç»°»ù´¡ÉÏ¼ÓÈë¹ú¼Ò´úÂë£¬DEF_NATION_CODE(ÔÚMAIN.HÖÐ¶¨Òå)        
* Input          : ËùÓÐ¶ÌÐÅ½«ÒÔUSC2±àÂë·½Ê½·¢ËÍ£¬º¯ÊýÄÚ²¿»á½«·¢ËÍµÄASCIIÂë×ª³ÉUSC2Âë(Ä¿µÄ±ÜÃâGSM×Ö·û¼¯µÄÒ»Ð©ÌØÊâ
*									 ×Ö·ûÈç£º@,$·þÎñÆ÷ÎÞ·¨Ê¶±ð)[×¢]Ä¬ÈÏ²»·¢ËÍ¶ÌÐÅÖÐÐÄºÅÂë³ý·Ç²ÎÊýÊÖ¶¯ÅäÖÃÁËÖÐÐÄºÅÂë  
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_SMSSendAlarm(u8	smsMode,	u8	*pSmsCenter,	u8	*pPhoneCode,	u8	*pData,	u16	len,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[200]="", tmpUSC2Buf[1024]="",	lenBuf[30]="",	tmpPhoneBuf[30]="";		
		u16	tmplen;
		s8	retCode=DEF_GSM_NONE_ERR,ret=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if((*pPhoneCode == '\0') || (len == 0))
				{
						retCode =	DEF_GSM_NONE_ERR;		// ·½Ê½Ó¦ÓÃ³ÌÐòÓÉÓÚ²ÎÊýÎÞ´í£¬µ¼ÖÂ³ÖÐø³¢ÊÔ·¢ËÍ
				}
				else
				{			
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								// ²éÑ¯µ±Ç°SIM¿¨×´Ì¬
								// Send "AT+CPIN?"
								ret =BSP_SendATcmdPro(DEF_SPECIAL_MODE,	(u8	*)AT_CPIN,	strlen((const char *)AT_CPIN),	RecCPINcpl,	CPINTimeout);
								if(ret !=	DEF_GSM_NONE_ERR)
								{
										if(ret == CommandTimeout)
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// Ö»ÓÐ³¬Ê±²ÅÈÏÎªÊÇ´íÎó
										retCode	=	DEF_GSM_CPIN_ERR;
								}
								else
								{
										if(smsMode	==	DEF_GSM_MODE)
										{
												if(len	>	DEF_SMS_USC2BYTE_SIZE)
												{
														retCode	=	DEF_GSM_NONE_ERR;		// Èç¹û³¬³öSMS·¢ËÍ³¤¶ÈÔò±»ÈÏÎª·¢ËÍ³É¹¦ 			
												}
												else
												{
														//	Text Mode
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_1,	strlen((const char *)AT_CMGF_1),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{						
																//	"USC2" Mode ±ÜÃâÌØÊâ×Ö·ûGSMÄ£Ê½·þÎñÆ÷ÊÕµ½²»Ê¶±ð
																//	Send AT+CSCS
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CSCS_ERR;
																}
																else
																{													
																		memset((u8 *)tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);
																		strcat((char *)tmpBuf,	(char *)"\"");	// copy "
																		tmplen = ChangeASCII2USC2 (pPhoneCode,	strlen((const char *)pPhoneCode),	tmpUSC2Buf);
																		strcat((char *)tmpBuf,	(char *)tmpUSC2Buf);
																		strcat((char *)tmpBuf,	(char *)"\"\r");																	
																		if(tmplen == 0xffff)
																				retCode	=	DEF_GSM_UNKNOW_ERR;		
																		else
																		{																																
																				//	Send AT+CMGS
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																						retCode	=	DEF_GSM_CMGSACK_ERR;		
																				}
																				else
																				{																
																						memset((u8 *)tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																						tmplen = ChangeASCII2USC2 (pData,	len,	tmpUSC2Buf);
																						strcat((char *)tmpUSC2Buf,	(char *)"\x1a");	
																						if(tmplen == 0xffff)
																								retCode	=	DEF_GSM_UNKNOW_ERR;	
																						else
																						{
																								//	Send SMS data
																								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																								{
																										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																										retCode	=	DEF_GSM_CMGSCPL_ERR;
																								}
																						}
																				}	
																		}	
																}
																//	Send AT+CSCS="GSM" ÇÐ»Øµ½GSM±àÂë·½Ê½·ÀÖ¹ÔÙ´ÎÅäÖÃÍøÂç²ÎÊýÊ±±àÂë¼¯¸ÄÎªUCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CSCS_ERR;
																}			
														}
												}			
										}
										else
										{
												if(len	> (DEF_SMS_USC2BYTE_SIZE*4))
												{
														retCode	=	DEF_GSM_NONE_ERR;		// Èç¹û³¬³öSMS·¢ËÍ³¤¶ÈÔò±»ÈÏÎª·¢ËÍ³É¹¦ 						
												}
												else
												{
														//  PDU ·¢ÖÐÎÄ´úÂë
														//	PDU	Mode						
														if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_0,	strlen((const char *)AT_CMGF_0),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
														{
																BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																retCode	=	DEF_GSM_CMGF_ERR;
														}
														else
														{								
																// 	"UCS2" Mode 
																//	Send AT+CSCS		
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CSCS_ERR;		
																}
																else
																{																									
																		memset((u8 *)tmpPhoneBuf,	'\0',	sizeof(tmpPhoneBuf));
																		strcat((char *)tmpPhoneBuf,	(char *)DEF_SMS_CONUTRY_CODE);	// ¸´ÖÆ¹ú¼Ò´úÂë
																		strcat((char *)tmpPhoneBuf,	(char *)pPhoneCode);				// ¸´ÖÆ±¨¾¯µç»°
																		
																		//	Make Pdu data and retur the len of data
																		tmplen = PDUMakeUpData(DEF_COM_PHONE, (u8	*)pSmsCenter,	(u8	*)tmpPhoneBuf, (u8	*)pData, len,	(u8	*)tmpUSC2Buf);
																		
																		memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
																		sprintf((char *)lenBuf,	"%d",	tmplen);
																		strcat((char *)tmpBuf,	(char *)AT_CMGS_H);	
																		strcat((char *)tmpBuf,	(char *)lenBuf);
																		strcat((char *)tmpBuf,	(char *)"\r");	
																		
																		if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	RecCMGSAck,	CMGSACKTimeout) !=	DEF_GSM_NONE_ERR)
																		{
																				BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																				retCode	=	DEF_GSM_CMGSACK_ERR;				
																		}
																		else
																		{
																				if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	RecCMGScpl,	CMGSCPLTimeout) !=	DEF_GSM_NONE_ERR)
																				{
																						BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																						retCode	=	DEF_GSM_CMGSCPL_ERR;				
																				}		
																		}
																}
																//	Send AT+CSCS="GSM" ÇÐ»Øµ½GSM±àÂë·½Ê½·ÀÖ¹ÔÙ´ÎÅäÖÃÍøÂç²ÎÊýÊ±±àÂë¼¯¸ÄÎªUCS2
																if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
																{
																		BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
																		retCode	=	DEF_GSM_CSCS_ERR;
																}	
														}
												}
										}
								}
						}
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSRead() 
* Description    : ¶ÌÐÅÈ¡»ØµÄÊÖ»úºÅÂë±£³ÖÔ­¸ñÊ½²»±ä      
* Input          : ËùÓÐ¶ÌÐÅ½«ÒÔUSC2±àÂë·½Ê½¶ÁÈ¡£¬º¯ÊýÄÚ²¿»á½«½ÓÊÕµ½µÄUSC2Âë×ª³ÉASCIIÂë(Ä¿µÄ±ÜÃâGSM×Ö·û¼¯µÄÒ»Ð©ÌØÊâ
*									 ×Ö·ûÈç£º@,$ÎÞ·¨Ê¶±ð),¶ÌÐÅ·¢ËÍÊ±¼ä×Ô¶¯×ª»»Îª¸ñÁÖÍþÖÎÊ±¼ä
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_SMSRead(u8	*pIndexBuf,	SYS_DATETIME *pTime,	u8	*pNumBuf,	u8	*pdBuf,	u16	*pdLen,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[30]="",	tmpUSC2Buf[1024]="",	zoneBuf[5]="";
		u16	p,i,len=0;	
		s8	retCode=DEF_GSM_NONE_ERR,	zoneHour=0;	
		SYS_DATETIME tmpTime;	
		u32	count=0;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if(*pIndexBuf == '\0')
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								//	Send AT+CMGF=1
								//	Return to text mode
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CMGF_1,	strlen((const char *)AT_CMGF_1),	CommandRecOk,	CMGFTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_CMGF_ERR;
								}
								else
								{				
										//	Send AT+CSCS
										//	Return to "UCS2" ÐÞ¸Ä³ÉUCS2·½Ê½(GSMÄ£Ê½ÏÂ¶Á¶ÌÐÅÓÐÐ©ÌØÊâ×Ö·ûÎ÷ÃÅ×ÓÄ£¿é»á·µ»ØÂÒÂë)
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_UCS2,	strlen((const char *)AT_CSCS_UCS2),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												retCode	=	DEF_GSM_CSCS_ERR;			
										}
										else
										{
												//	Send CMGR
												memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
												strcat((char *)tmpBuf,	(char *)AT_CMGR_H);	
												strcat((char *)tmpBuf,	(char *)pIndexBuf);	
												strcat((char *)tmpBuf,	(char *)"\r");	
						
												if(BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CMGRTimeout) !=	DEF_GSM_NONE_ERR)
												{
														BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
														retCode	=	DEF_GSM_CMGR_ERR;
												}
												else
												{																					
														// ²éÑ¯Î»ÖÃ
														p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\r\n+CMGR: ",s_GSMcomm.ps_commu->RCNT,9);
														if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
														{
																// ¶ÌÐÅ´æ´¢ÇøÎª¿Õ
																retCode	=	DEF_GSM_SMS_ENPTY;
														}
														else
														{
																//	ÌáÈ¡µç»°ºÅÂë
																p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)",\"",s_GSMcomm.ps_commu->RCNT,2);
																if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																{
																		retCode	=	DEF_GSM_FAIL_ERR;	
																}
																else
																{			
																		memset(tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																		
																		//±£ÁôÔ­¸ñÊ½²»±ä(¼´+86XXXXXX)
																		p = p + 2;	// Ö¸Ïòµç»°Çø						
																		for(i=0;	(*(s_GSMcomm.pRecBuf + p + i) != '\"') && (i < sizeof(tmpUSC2Buf));	i++)
																		{
																				tmpUSC2Buf[i] =  *(s_GSMcomm.pRecBuf + p + i);
																		}
																		tmpUSC2Buf[i] = '\0';
																		
																		//×ª»»³É±ê×¼ACSIIÂëÒÔ±ãÐ­Òé½âÎö	
																		len = 0;
																		len = ChangeUSC22ASCII (tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	pNumBuf);
																		if(len == 0xffff)
																				retCode	=	DEF_GSM_UNKNOW_ERR;	// ×ª»»´íÎó
																		else
																		{																				
																				//  ÌáÈ¡¶ÌÐÅ·¢ËÍÊ±¼ä
																			  p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\"**/",s_GSMcomm.ps_commu->RCNT,4);
																				if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																				{
																						retCode	=	DEF_GSM_BUSY_ERR;	
																				}
																				else
																				{
																						p += 1;								
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->year	=	2000 + atoi((const char *)tmpBuf);
																						tmpTime.year =	2000 + atoi((const char *)tmpBuf);
										
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->month	=	atoi((const char *)tmpBuf);
																						tmpTime.month =atoi((const char *)tmpBuf);
																						
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->day	=	atoi((const char *)tmpBuf);
																						tmpTime.day =atoi((const char *)tmpBuf);
										
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->hour	=	atoi((const char *)tmpBuf);
																						tmpTime.hour =atoi((const char *)tmpBuf);
										
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->minute	=	atoi((const char *)tmpBuf);
																						tmpTime.minute =atoi((const char *)tmpBuf);
										
																						p += 3;
																						memcpy(tmpBuf,	(s_GSMcomm.pRecBuf + p),	2);
																						tmpBuf[2] ='\0';
																						//pTime->second	=	atoi((const char *)tmpBuf);
																						tmpTime.second =atoi((const char *)tmpBuf);
																						
																						// ÌáÈ¡Ê±Çø
																						p += 2;
																						for(i=0;	(i<3) && (*(s_GSMcomm.pRecBuf + p + i) != '"');	i++)
																						{
																								zoneBuf[i] = *(s_GSMcomm.pRecBuf + p + i);
																						}
																						zoneBuf[i] ='\0';
																						zoneHour = atoi((const char *)zoneBuf) / 4; // ×ª³ÉÊµ¼ÊÊ±Çø																						
																						count = Mktime (tmpTime.year, tmpTime.month,	tmpTime.day, tmpTime.hour,	tmpTime.minute, tmpTime.second);
																						count = count - (zoneHour * 3600);	// ×ª³É¸ñÁÖÍþÖÎÊ±¼ä																	
																						Gettime (count,	pTime);	// ×ªÎªÄêÔÂÈÕÊ±·ÖÃë¸ñÊ½

																						//´Ó»º³åÇøÀï²éÕÒ["\r]Î»ÖÃ¼´¶ÌÐÅÄÚÈÝ
																						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"\"\r",s_GSMcomm.ps_commu->RCNT,2);
																					 	if((p	==	0xffff)||(p	>	s_GSMcomm.ps_commu->RCNT))
																						{
																								retCode	=	DEF_GSM_FAIL_ERR;	
																						}
																						else
																						{
																								memset(tmpUSC2Buf,	'\0',	sizeof(tmpUSC2Buf));
																								//	ÌáÈ¡¶ÌÐÅÄÚÈÝ
																								p	+= 3;
																								for(i=0;	*(s_GSMcomm.pRecBuf + p + i) != '\r';	i++)
																								{
																										tmpUSC2Buf[i] = *(s_GSMcomm.pRecBuf + p + i);
																								}
																								tmpUSC2Buf[i] = '\0';
		
																								//×ª»»³É±ê×¼ACSIIÂëÒÔ±ãÐ­Òé½âÎö
																								len = 0;
																								len = ChangeUSC22ASCII (tmpUSC2Buf,	strlen((const char *)tmpUSC2Buf),	pdBuf);
																								if(len == 0xffff)
																								{
																										*pdLen  = 0;
																										retCode	=	DEF_GSM_UNKNOW_ERR;	// ×ª»»´íÎó
																								}
																								else
																										*pdLen = len;		
																						}
																				}
																		}
																}	
														}
												}
										}
										//	Send AT+CSCS="GSM" ÇÐ»Øµ½GSM±àÂë·½Ê½·ÀÖ¹ÔÙ´ÎÅäÖÃÍøÂç²ÎÊýÊ±±àÂë¼¯¸ÄÎªUCS2
										if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCS_GSM,	strlen((const char *)AT_CSCS_GSM),	CommandRecOk,	CSCSTimeout) !=	DEF_GSM_NONE_ERR)
										{
												BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
												retCode	=	DEF_GSM_CSCS_ERR;
										}											
								}
						}
						else
								retCode	=	DEF_GSM_BUSY_ERR;	
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_SMSDelete() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_SMSDelete(u8	*pIndexBuf,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[30];	
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if(*pIndexBuf == '\0')
				{
						retCode =	DEF_GSM_FAIL_ERR;
				}
				else
				{							
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								//	Send CMGD
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_CMGD_H);	
								strcat((char *)tmpBuf,	(char *)pIndexBuf);	
								strcat((char *)tmpBuf,	(char *)"\r");
								
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	CMGDTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_CMGD_ERR;
								}
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;
				}
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;

		return	retCode;		
}
/*
*********************************************************************************************************
*                            					GSMÉÏ²ãÓ¦ÓÃº¯Êý-µç»°²¿·Ö(ÓÉOS Ó¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_CallDail() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_CallDail (u8	*pPhone,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u8	tmpBuf[30]="";
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;			

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				// ¼ì²âÈë²ÎºÏ·¨ÐÔ
				if(*pPhone == '\0')
				{	
						retCode =	DEF_GSM_FAIL_ERR;	
				}
				else
				{
						if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
						{
								//	Send ATD
								memset((u8 *)tmpBuf,	'\0',	sizeof(tmpBuf));
								strcat((char *)tmpBuf,	(char *)AT_ATD_H);	
								strcat((char *)tmpBuf,	(char *)pPhone);	
								strcat((char *)tmpBuf,	(char *)";\r");
								if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)tmpBuf,	strlen((const char *)tmpBuf),	CommandRecOk,	ATDTimeout) !=	DEF_GSM_NONE_ERR)
								{
										BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
										retCode	=	DEF_GSM_ATD_ERR;
								}
						}	
						else
								retCode	=	DEF_GSM_BUSY_ERR;
				}	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_CheckCall() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_CheckCall (u8 *pPhone,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		u16 p=0,i=0;
		u8	sta=0;
		s8	tmpRen,	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						tmpRen = BSP_SendATcmdPro(DEF_ONLYOK_MODE,	(u8	*)AT_CLCC,	strlen((const char *)AT_CLCC),	CommandRecOk,	CLCCTimeout);
						if(tmpRen != DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_CLCC_ERR;		
						}
						else
						{
								p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CLCC:",s_GSMcomm.ps_commu->RCNT,6);		
								if((p == 0xffff)&&(p	> s_GSMcomm.ps_commu->RCNT))
								{
										retCode = CALL_DOWN;		// ·µ»Ø×Ö¶ÎÀïÖ»ÓÐOKÃ»ÓÐCLCC±íÊ¾Í¨»°ÒÑ¾­½áÊø£¬ËùÒÔ²»×ö´íÎóÀÛ¼Ó
								}
								else
								{
										//+CLCC: 1,1,0,0,0\r\n\r\nOK\r\n	(ÎÞÀ´µçÏÔÊ¾·µ»Ø)
										//+CLCC: 1,1,4,0,0,"01058302855",129\r\n\r\nOK\r\n
										sta = *(s_GSMcomm.pRecBuf + p + 9);			// È¡µç»°·¢Æð×´Ì¬
										if(sta == '0')
										{
											 	 // µç»°·¢ÆðÕß	
										}
										else if(sta == '1')
										{
												// µç»°½ÓÊÕÕßÈ¡´òÀ´Õßµç»°ºÅÂë
												if((*(s_GSMcomm.pRecBuf + p + 16) == ',') && (*(s_GSMcomm.pRecBuf + p + 17) == '"')) 	
												{
														for(i=0;	(*(s_GSMcomm.pRecBuf + p + i + 18) != '\"') && (i < s_GSMcomm.ps_commu->RCNT);	i++)
														{
															 *(pPhone + i) = *(s_GSMcomm.pRecBuf + p + i + 18);	
														}
														*(pPhone + i) = '\0';
												}
												else
														*pPhone = '\0';	//	(ÎÞÀ´µçÏÔÊ¾ÎÞ¸Ã×Ö¶Î)				
										}
										sta = *(s_GSMcomm.pRecBuf + p + 11);			// È¡µç»°×´Ì¬
									  if(sta == '0')
									 			retCode = CALL_ACTIVE;
									  else if(sta == '1')
									 			retCode = CALL_HELD;
									  else	if(sta ==	'2')
									 			retCode = CALL_DIALLING;
									  else if(sta == '3')
												retCode = CALL_ALERTING;
										else if(sta == '4')
												retCode = CALL_INCOMING;
										else if(sta == '5')
												retCode = CALL_WAITING;
										else
												retCode	=	DEF_GSM_FAIL_ERR;	
								}
						}
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_HungUpCall() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_HungUpCall(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_ATH,	strlen((const char *)AT_ATH),	CommandRecOk,	ATHTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_ATH_ERR;		
						}			
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_ChangeAudioPath() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_ChangeAudioPath(u8 audioPath,	u32	timeout)
{
		audioPath = audioPath;
		timeout		= timeout;
		return	DEF_GSM_NONE_ERR;
}
/*
*********************************************************************************************************
* Function Name  :									  BPS_AckCall() 
* Description    :         
* Input          :
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BPS_AckCall(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_ATA,	strlen((const char *)AT_ATA),	CommandRecOk,	ATATimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_ATA_ERR;		
						}		
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
*                            					GSMÉÏ²ãÓ¦ÓÃº¯Êý-Ë¯Ãß²¿·Ö(ÓÉOS Ó¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  GSMGotoSleep() 
* Description    :         
* Input          : RTS-IO = 1;
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void	GSMGotoSleep (void)
{
		// ½øÈëË¯ÃßÄ£Ê½		
		IO_GSM_DTR_DIS();					
		IO_GSM_DTR_EN();					
}
/*
*********************************************************************************************************
* Function Name  :									  GSMStartWork() 
* Description    :         
* Input          : RTS-IO = 0;
* Output         : 
* Return         : 
*********************************************************************************************************
*/
static	void	GSMStartWork (void)
{
		// ÍË³öË¯ÃßÄ£Ê½
		IO_GSM_DTR_EN();
		IO_GSM_DTR_DIS();
}

/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMIntoSleep() 
* Description    :  ·¢ËÍAT+CSCLK=1Ê¹ÄÜË¯ÃßÄ£Ê½ ºó²Ù×÷ÊÍ·ÅDTR¹Ü½Å(DTR 0->1)£¬ÉèÖÃË¯Ãß±êÖ¾(ringWUpFlag 0->1)       
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMIntoSleep(u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// Ê¹ÄÜË¯ÃßÄ£Ê½
						if(BSP_SendATcmdPro(DEF_COMMON_MODE,	(u8	*)AT_CSCLK_1,	strlen((const char *)AT_CSCLK_1),	CommandRecOk,	CSCLK1Timeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_CSCLK1_ERR;		
						}	
						else
						{
								gsmDelayMS(2000);	// ±£³ÖÄ£¿é¿ÕÏÐ2Ãë		
								GSMGotoSleep();		// Ö±½Ó½øÈëË¯Ãß	
								ringWUpFlag	=1;		// ÉèÖÃGSMË¯Ãß±êÖ¾		
						}							
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMWakeUp() 
* Description    : »½ÐÑÇ°¼ì²éÊÇ·ñÓÉÓÚringµ¼ÖÂµÄ»½ÐÑÈç¹ûÊÇµÄ»°ÔòÐèÒªÊÖ¶¯Æô¶¯Ò»´Î¶ÌÏ¢²éÑ¯        
* Input          : ²Ù×÷RTS=1
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMWakeUp(u32	timeout)
{
		timeout = timeout; 
		GSMStartWork();		// Ö±½ÓÍË³öË¯Ãß	
				
		if(ringWUpFlag == 2)	
		{
				ringWUpFlag =0;																					// »Ö¸´±êÖ¾µ½Ä¬ÈÏ×´Ì¬
				if(BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0)							// ·ÀÖ¹ÖØ¸´½øÈë
				{
						if(OSRunning > 0)
						{
								BSP_GSM_SetFlag(DEF_SMSDOING_FLAG);							// ÉèÖÃ¶ÌÐÅ´¦ÀíÖÐ
								OSSemPost(GSMSmsSem); 													// Éè±¸´ÓË¯Ãß»½ÐÑÊÖ¶¯´¥·¢Ò»´Î¶ÌÐÅ¼ìË÷
						}
				}
		}		
		return	0;	
}

/*
*********************************************************************************************************
*                            					GSMÉÏ²ãÓ¦ÓÃº¯Êý-Í¨ÓÃ½Ó¿Ú²¿·Ö(ÓÉOS Ó¦ÓÃº¯Êýµ÷ÓÃ)     
*********************************************************************************************************
*/
/*
*********************************************************************************************************
* Function Name  :									  BSP_GSMBypassPro() 
* Description    : GSM AT Ö¸ÁîÍ¸´«½Ó¿Ú       
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
s8	BSP_GSMBypassPro(u8	*sendBuf,	u16 sendLen,	u8	*recBuf,	u16 *recLen,	u16 maxRecLen,	u32 atTimeout,	u32	timeout)
{
		INT8U	err =OS_NO_ERR;
		s8	retCode=DEF_GSM_NONE_ERR;		

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	DEF_GSM_FAIL_ERR;	

		if(err != OS_NO_ERR)
		{
				if(err	==	OS_ERR_TIMEOUT)
						return	DEF_GSM_TIMEOUT_ERR;
				else
						return	DEF_GSM_FAIL_ERR;		
		}
		else
		{
				if((s_GSMcomm.SetupPhase == CommandInitOK)&&(s_GSMcomm.GSMComStatus	>=	GSM_ONCMD))
				{
						// Èí¼þ¸´Î»
						if(BSP_SendATcmdPro(DEF_NULL_MODE,	(u8	*)sendBuf,	sendLen,	CommandTimeout,	atTimeout) !=	DEF_GSM_NONE_ERR)
						{
								BSP_GSM_ERR_Add(&err_Gsm.gsmErrorTimes,	1);		// ´íÎóÀÛ¼Ó
								retCode	=	DEF_GSM_UNKNOW_ERR;		
						}	
						else
						{
								// ¸´ÖÆ·µ»Ø×Ö·û´®
								if(s_ATcomm.Rlen > maxRecLen)
										memcpy(recBuf,	s_ATcomm.pRecBuf,	maxRecLen);	
								else
										memcpy(recBuf,	s_ATcomm.pRecBuf,	s_ATcomm.Rlen);	
								*recLen	=	s_ATcomm.Rlen;	
								retCode = DEF_GSM_NONE_ERR;		
						}					
				}
				else
						retCode	=	DEF_GSM_BUSY_ERR;	
		}

		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
		return	retCode;	
}

/*
******************************************************************************
* Function Name  : BSP_GSMSemPend()
* Description    : ÓÃÓÚGSMÐÅºÅÁ¿µÄ»ñÈ¡£¬±ØÐëÓëBSP_GSMSemPost³É¶ÔÊ¹ÓÃ
* Input          : timeout :µÈ´ýÐÅºÅÁ¿³¬Ê±Ê±¼ä
* Output         : None
* Return         : None
******************************************************************************
*/
s8	BSP_GSMSemPend (u32	timeout)
{
		INT8U	err =OS_NO_ERR;

		if(OSRunning > 0)
				OSSemPend(GSMMutexSem,	timeout,	&err);		//	ÇëÇó·¢ËÍÊý¾ÝÐÅºÅÁ¿
		else
				if(GSMMutexFlag	== 1)
						GSMMutexFlag	=	0;
				else
						return	-1;	

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
* Function Name  : BSP_GSMSemPost()
* Description    : ÓÃÓÚGSMÐÅºÅÁ¿µÄÊÍ·Å£¬±ØÐëÓëBSP_GSMSemPend³É¶ÔÊ¹ÓÃ
* Input          : None
* Output         : None
* Return         : None
******************************************************************************
*/
void BSP_GSMSemPost (void)
{
		if(OSRunning > 0)
				OSSemPost(GSMMutexSem);									//	ÊÍ·ÅÕ¼ÓÃµÄÐÅºÅÁ¿
		else
				GSMMutexFlag	=	1;
}
/*
*********************************************************************************************************
*                            					GSMÑ­»·µ÷ÓÃº¯Êý(ÐèÏµÍ³ÖÐÑ­»·µ÷ÓÃ)     
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_Setup()
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_Setup (void)
{
		u16 i;
		u8	*pUint8;

		pUint8 = (u8 *)s_GSMcomm.pSendBuf;

		// Pow on the gsm module 
		if(s_GSMcomm.SetupPhase >= CommandInitOK)
				return;// Already finish the gsm module setup 
		else
				IO_GSM_POW_EN();		// Ä£¿é¹©µç	
																				
    if(s_GSMcomm.SetupPhase == PowerOff)
		{
				// Ô¤ÖÃ¶Ë¿Ú×´Ì¬
				s_GSMcomm.GSMComStatus 	= GSM_POWOFF;

				// Ê×ÏÈÇå³ý½ÓÊÕ»º³åÇø×¼±¸½ÓÊÕ¿ª»ú×Ö·û´®
				memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE);	
				s_GSMcomm.ps_commu->RCNT	=	0;
				GSM_ENABLE_RES_IT();							// µÚÒ»´ÎÊ¹ÄÜGSMÄ£¿éÖÐ¶Ï
				BSP_GSMRING_IT_CON(DEF_DISABLE);	// ¼ì²â¹ý³ÌÖÐ¹Ø±ÕRINGÖÐ¶Ï
							
				//IO_GSM_RES_OFF();
				IO_GSM_POW_OFF();
				IO_GSM_POW_ON();
	     	s_GSMcomm.SetupPhase = PowerOn;
    }
		else if(s_GSMcomm.SetupPhase == PowerOn)
		{
	      s_GSMcomm.SetupPhase 		= SignalOn;
				s_GSMcomm.Timer 				= 0;							//×¼±¸ÑÓÊ±2S
    }
		else if(s_GSMcomm.SetupPhase == SignalOn)
		{
				if(s_GSMcomm.Timer < GSMTime2s)		// µÈ´ý2S	
		      	return;
	      else
				{
		        IO_GSM_POW_OFF();
		        s_GSMcomm.SetupPhase 		= CheckPowPin;
						s_GSMcomm.Timer					=	0;	 				 //×¼±¸ÏÂ´ÎÑÓÊ±
	      }
    }
		else if(s_GSMcomm.SetupPhase == CheckPowPin)
		{
				if(IO_GSM_POW_DET() == 1u)
				{			
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSMSTA)
						myPrintf("[GSM]: PowerOn OK!\r\n");
						#endif
						s_GSMcomm.SetupPhase 		= CheckStartString;
						s_GSMcomm.Timer					=	0;	 				 // ×¼±¸ÏÂ´ÎÑÓÊ±
						BSP_GSM_ERR_Clr(&err_Gsm.powOnTimes);  // Çå´íÎó´ÎÊý
				}
				else
				{
						if(s_GSMcomm.Timer < GSMTime10s)
								return;
						else
						{
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSMSTA)
								myPrintf("[GSM]: PowerOn Fail!\r\n");
								#endif
								BSP_GSM_ERR_Add(&err_Gsm.powOnTimes,	1);	//Æô¶¯Ê§°Ü´ÎÊý¼ÓÒ»
								if(BSP_GSM_ERR_Que(&err_Gsm.powOnTimes) >= DEF_GSM_POWON_TIMES)
								{
										BSP_GSM_ERR_Clr(&err_Gsm.powOnTimes);
										s_GSMcomm.HardWareSta		=	DEF_GSMHARD_MODDESTROY_ERR;	
								}
								s_GSMcomm.SetupPhase 	= PowerOff;
								s_GSMcomm.Timer				=	0;	 			//×¼±¸ÏÂ´ÎÑÓÊ±	
								BSP_GSM_POW_RESET(3000);
						}
				}
		}
		// Check power on string 
		else if(s_GSMcomm.SetupPhase == CheckStartString)
		{
				if(s_GSMcomm.Timer < GSMTime5s)		//µÈ´ýÊä³öÍêÉÏµç×Ö·û"RDY"
				{
						if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"RDY\r\n",s_GSMcomm.ps_commu->RCNT,5)!=0xffff)
						{									
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSM)
								BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pRecBuf,	s_GSMcomm.ps_commu->RCNT);
								#endif
								
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSMSTA)
								myPrintf("[GSM]: Communication OK!\r\n");
								#endif

								s_GSMcomm.SetupPhase 		= CheckSimCardSend;
								s_GSMcomm.Timer					=	0;	 				//×¼±¸ÏÂ´ÎÑÓÊ±																				
								BSP_GSM_ERR_Clr(&err_Gsm.cmmTimes);  // Çå´íÎó´ÎÊý
								gsmDelayMS(1000);	
						}
				}
				else
				{
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSMSTA)
						myPrintf("[GSM]: Fixed Baud Rate!\r\n");
						#endif
										
						// ÐèÒª¹Ì¶¨²¨ÌØÂÊ
						GSM_DISABLE_SEND_IT();												
						BSP_PutString(DEF_GSM_UART,(u8 *)"AT+IPR=115200\r");
						gsmDelayMS(500);
						BSP_PutString(DEF_GSM_UART,(u8 *)"AT&W\r");	//SIM800ÐèÒªÊÖ¶¯´æ´¢
						gsmDelayMS(200);

					  // ´íÎó´¦Àí
						BSP_GSM_ERR_Add(&err_Gsm.cmmTimes,	1);	//Æô¶¯Ê§°Ü´ÎÊý¼ÓÒ»
						if(BSP_GSM_ERR_Que(&err_Gsm.cmmTimes) >= DEF_GSM_STRING_TIMES)
						{
								BSP_GSM_ERR_Clr(&err_Gsm.cmmTimes);
								s_GSMcomm.HardWareSta		=	DEF_GSMHARD_COMM_ERR;	
						}
						s_GSMcomm.SetupPhase 		= PowerOff;
						s_GSMcomm.Timer					=	0;	 			//×¼±¸ÏÂ´ÎÑÓÊ±
						BSP_GSM_POW_RESET(3000);
				}	
		}	
		// send check simcard
		else if(s_GSMcomm.SetupPhase == CheckSimCardSend)
		{
				memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // Çå»º³å
				s_GSMcomm.ps_commu->RCNT	=	0;
				GSM_DISABLE_SEND_IT();	
			
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				myPrintf("AT+CPIN?\r\n");	// Êä³ö·¢ËÍATÖ¸Áî
				#endif	
				BSP_PutString(DEF_GSM_UART,(u8 *)"AT+CPIN?\r");	// ²éÑ¯SIM card
				//BSP_PutString(DEF_GSM_UART,(u8 *)"AT+CREG?\r");	// ²éÑ¯SIM card
				s_GSMcomm.SetupPhase = CheckSimCard;
				s_GSMcomm.Timer			 = 0;	 			//×¼±¸ÏÂ´ÎÑÓÊ±
		}
		// check simcard 
		else if(s_GSMcomm.SetupPhase == CheckSimCard)
		{
				if(s_GSMcomm.Timer < GSMTime2s)
				{
						if((StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CPIN: READY",s_GSMcomm.ps_commu->RCNT,12)!=0xffff) ||
						   (StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CPIN: NOT INSERTED",s_GSMcomm.ps_commu->RCNT,19)!=0xffff) ||
						   (StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"ERROR",s_GSMcomm.ps_commu->RCNT,5)!=0xffff))
						{								
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSM)
								BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pRecBuf,	s_GSMcomm.ps_commu->RCNT);
								#endif		
							
								if(StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CPIN: READY",s_GSMcomm.ps_commu->RCNT,12)!=0xffff) 
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GSMSTA)
										myPrintf("[GSM]: Detect Simcard!\r\n");
										#endif
								}
								else
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GSMSTA)
										myPrintf("[GSM]: SimCard Not Insert!\r\n");
										#endif							
								}
																
								memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // Çå»º³å		
								s_GSMcomm.ps_commu->RCNT	=	0;						
								s_GSMcomm.GSMComStatus 	= GSM_POWOK;
								s_GSMcomm.SetupPhase 		= SimCardOK;
								s_GSMcomm.Timer					=	0;	 				//×¼±¸ÏÂ´ÎÑÓÊ±
								s_GSMcomm.HardWareSta		=	DEF_GSMHARD_NONE_ERR;										
								BSP_GSM_ERR_Clr(&err_Gsm.simCardTimes);	//Çå´íÎó¼ÆÊ±Æ÷
						}
				}
				else
				{					
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSM)
						BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pRecBuf,	s_GSMcomm.ps_commu->RCNT);
						#endif
						
						memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // Çå»º³å		
						s_GSMcomm.ps_commu->RCNT	=	0;						
						s_GSMcomm.GSMComStatus 	= GSM_POWOK;
						s_GSMcomm.SetupPhase 		= SimCardOK;
						s_GSMcomm.Timer					=	0;	 				//×¼±¸ÏÂ´ÎÑÓÊ±
						s_GSMcomm.HardWareSta		=	DEF_GSMHARD_NONE_ERR;										
						BSP_GSM_ERR_Clr(&err_Gsm.simCardTimes);	//Çå´íÎó¼ÆÊ±Æ÷
				}	
		}
		// Start send AT cmd 
		else if(s_GSMcomm.SetupPhase == SimCardOK)
		{
	      // send out the first AT cmd. 
	      s_GSMcomm.SetupPhase 	= InitCommand;
	      s_GSMcomm.CmdPhase 		= CommandSend;
	      s_GSMcomm.Timer 			= 0;
	      s_GSMcomm.ActiveCmd 	= 0;
				
				//---------------------------------------------------------------
	      i=0;
	      while(pATcommand[0][i])
				{
		      	*pUint8 = pATcommand[0][i];
		       	pUint8++;
		        i++;
	      }
	      //---------------------------------------------------------------
				#if(DEF_GSMINFO_OUTPUTEN > 0)
				if(dbgInfoSwt & DBG_INFO_GSM)
				BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pSendBuf,	i);
				#endif		
				memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // Çå»º³å	
				s_GSMcomm.ps_commu->RCNT = 0;		
			 	s_GSMcomm.ps_commu->SLEN = i;
	      s_GSMcomm.sendstart();
    }
		else if(s_GSMcomm.SetupPhase == InitCommand)
		{
        //  setting up Parameter to gsm module 
        if(s_GSMcomm.CmdPhase == CommandSend)
				{
            if(s_GSMcomm.ps_commu->State & SS_O)
						{
								s_GSMcomm.CmdPhase 	= CommandSdCpl;
                s_GSMcomm.Timer 		= 0;
								GSM_ENABLE_RES_IT();					//Ê¹ÄÜ½ÓÊÕÖÐ¶Ï×¼±¸½ÓÊÕ ATÖ¸Áî½á¹û
            }   
        }
				else if(s_GSMcomm.CmdPhase == CommandSdCpl)
				{
            i = CheckResultCode(DEF_COMMON_MODE,	cmdTimeout[s_GSMcomm.ActiveCmd]);
            if(i)
						{
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSM)
								BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pRecBuf,	s_GSMcomm.ps_commu->RCNT);
								#endif		
							
								// receive check!!
				        if(s_GSMcomm.CmdPhase == CommandRecOk)
								{            						
										// received OK. send next AT command.
				            s_GSMcomm.ActiveCmd++;
				
										// ¼ÓÈëÑÓÊ±ÓÐµÄ¿¨²»¼ÓÈëÑÓÊ±³õÊ¼»¯²»³É¹¦
										gsmDelayMS(500);
				
				            if(s_GSMcomm.ActiveCmd == InitCommandCpl)
										{               											
												#if(DEF_GSMINFO_OUTPUTEN > 0)
												if(dbgInfoSwt & DBG_INFO_GSMSTA)
												myPrintf("[GSM]: Init Success!\r\n");
												#endif											
											
												// the last At command has been send successfully. 
				                s_GSMcomm.SetupPhase 		= CommandInitOK;
				                s_GSMcomm.CmdPhase 			= CommandIdle;
				                s_GSMcomm.ActiveCmd 		= NoCmdActive;	       
												s_GSMcomm.GSMComStatus	=	GSM_ONCMD;	// ÖÃGSM¶Ë¿Ú×´Ì¬ 
												BSP_GSMRING_IT_CON(DEF_ENABLE);				// ¿ªÆôRINGÖÐ¶Ï
												BSP_GSM_ERR_Clr(&err_Gsm.initTimes);
												return;
				            }
										else
										{}	//¼ÌÐø·¢ÏÂÒ»ÌõATÃüÁî
				        }
								else if((s_GSMcomm.CmdPhase == CommandRecErr) || (s_GSMcomm.CmdPhase == CommandTimeout))
								{      					 
										// received ERROR. 
										OSTimeDly(1000);		// Èç¹û»Ø¸´´íÎó·ÀÖ¹ÖØÊÔÖ¸Áî¼ä¸ô¹ý¶Ì 
				
										// ´íÎó´¦Àí
										BSP_GSM_ERR_Add(&err_Gsm.initTimes,	1);	//Æô¶¯Ê§°Ü´ÎÊý¼ÓÒ»
										if(BSP_GSM_ERR_Que(&err_Gsm.initTimes) >= DEF_GSM_INIT_TIMES)
										{
												BSP_GSM_ERR_Clr(&err_Gsm.initTimes);
												s_GSMcomm.HardWareSta		=	DEF_GSMHARD_INIT_ERR;	
												s_GSMcomm.SetupPhase 		= PowerOff;
												BSP_GSM_POW_RESET(6000);
										}
				        }
								else
								{
										return;
								}
				        
				        /////////////////////////////////////////////////////////////////////
				        /////////////////////////////////////////////////////////////////////
								// ·¢ËÍµÚ¶þ¸öÒÔºóµÄ³õÊ¼»¯Ö¸Áî
				        i=0;
				        while(pATcommand[s_GSMcomm.ActiveCmd][i])
								{
				            *pUint8 = pATcommand[s_GSMcomm.ActiveCmd][i];
				            pUint8++;
				            i++;
				        }

				        //----------------------------------------------------------------
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_GSM)
								BSP_OSPutChar (DEF_DBG_UART,	s_GSMcomm.pSendBuf,	i);
								#endif		
								memset((u8 *)s_GSMcomm.pRecBuf,	'\0',	GSM_GPSRSBUF_RXD_SIZE); // Çå»º³å	
								s_GSMcomm.ps_commu->RCNT = 0;		
				        s_GSMcomm.Timer = 0;
				        s_GSMcomm.CmdPhase = CommandSend;
				        s_GSMcomm.ps_commu->SLEN = i;
				        s_GSMcomm.sendstart();
						}	
						else
              	return;	// µÈ´ý³¬Ê±
        }
				else
          	return;
    }		
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_TransPro()
* Description    :         
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_TransPro (void) 
{				
		Start2doSendAT();		// AT·¢ËÍ´¦Àí
		DoSendProcess();		// AT½ÓÊÕ´¦Àí		
}
/*
*********************************************************************************************************
* Function Name  : 													BSP_GSM_CommonPro()	
* Description    : GSMÄ£¿é£¬À´µç»°£¬¶ÌÐÅ£¬GPRS£¬À¶ÑÀ£¬µÈURCÊý¾ÝÅÐ¶Ï´¦Àí        
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
void	BSP_GSM_CommonPro (void)
{
  	u16	p=0,q=0,k=0,dotNum=0;

		// ×Ô¶¯ÉÏ±¨µÄURC¼ì²â´¦Àí(ÍøÂç¶ÁÊý¾Ý½×¶Î½ûÖ¹¼ì²â)//////////////////////////////////////////////////
		if((s_GSMcomm.SetupPhase == CommandInitOK) && (s_GSMcomm.GSMComStatus	>=	GSM_ONCMD) && (tcpReadMuxtex == 0))
		{		
				//	¼ì²âRing¹Ü½Å×´Ì¬	
				// 2015-1-15 by:gaofei ÃÖ²¹GPRSÊý¾ÝÒÅÂ©ÎÊÌâ,¸ÃÂß¼­ÒÑÈ¡Ïû,¾­²âÊÔÒ»µ©Ê¹ÄÜURC ringÖÐ¶Ï¿ÉÄÜµ¼ÖÂÊý¾Ý½ÓÊÕ
				if(BSP_GSM_GetFlag(DEF_RING_FLAG) == 1)
				{
						BSP_GSM_ReleaseFlag(DEF_RING_FLAG);
						
						/*
						#if(DEF_GSMINFO_OUTPUTEN > 0)
						if(dbgInfoSwt & DBG_INFO_GSMSTA)
						myPrintf("[GSM]: Ring!\r\n");
						#endif						
						if(BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1)
						{
								ringCheckStep =1;
								OSTmr_Count_Start(&ringCheckCounter);
						}
						*/
				}
				/*
				if(ringCheckStep == 1)
				{
						if(OSTmr_Count_Get(&ringCheckCounter) >= 2000)
						{
								ringCheckStep =0;
								if((BSP_GSM_GetFlag(DEF_CALLDOING_FLAG) == 0) && 
									 (BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0) &&
								   (BSP_GSM_GetFlag(DEF_GDATA_FLAG) == 1))
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GSMSTA)
										myPrintf("[GSM]: Ring-reRead!\r\n");
										#endif
										if(OSRunning > 0)
												OSSemPost(GSMGprsDataSem);		// ÊÖ¶¯²éÑ¯ÊÇ·ñ½Óµ½µ½Êý¾Ý
								}
						}			
				}
				*/
				//	¼ì²âÊÇ·ñÓÐµç»°´òÈë	
				/*
				p = 0;
				p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"RING\r\n",s_GSMcomm.ps_commu->RCNT,6);		
				if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
				{
						*(s_GSMcomm.pRecBuf + p) 			= '\0';
						*(s_GSMcomm.pRecBuf + p + 1) 	= '\0';		
						BSP_PutString(DEF_GSM_UART,(u8 *)"ATA\r");	// ½Óµç»°
						
				}
				*/
				//  ¼ì²â¶ÌÐÅ½ÓÊÕ	
				//	¼ì²âGPRSÍøÂç×´Ì¬
				//	¼ì²âBT×´Ì¬
				//	¼ì²âTTS×´Ì¬
				if(1)
				{				
						p =	0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CMTI:",s_GSMcomm.ps_commu->RCNT,6);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';
								*(s_GSMcomm.pRecBuf + p + 1) 	= '\0';								// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â
								if(BSP_GSM_GetFlag(DEF_SMSDOING_FLAG) == 0)					// ·ÀÖ¹ÖØ¸´½øÈë
								{
										if(OSRunning > 0)
										{
												BSP_GSM_SetFlag(DEF_SMSDOING_FLAG);					// ÉèÖÃ¶ÌÐÅ´¦ÀíÖÐ
												OSSemPost(GSMSmsSem); 
										}
								}
						}	
				}
				
				//  ¼ì²âTTS²¥·ÅÍê³É			
				if(1)
				{		
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CTTS: 0",s_GSMcomm.ps_commu->RCNT,8);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';
								*(s_GSMcomm.pRecBuf + p + 1) 	= '\0';								// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â
								if(BSP_GSM_GetFlag(DEF_TTSPLAY_FLAG) == 1)					// ·ÀÖ¹ÖØ¸´½øÈë
								{
										BSP_GSM_ReleaseFlag (DEF_TTSPLAY_FLAG);					// ¸´Î»±êÖ¾										
										IO_SPAKER_PA_DIS();															// ¹Ø±Õ¹¦·Å
								}
						}	
				}
				
				//	¼ì²âBT½ÓÊÕÊý¾Ý¼°ÒµÎñ×´Ì¬	
				if(1)
				{						
						// ¼ì²âÆ¥ÅäÇëÇó									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTPAIRING:",s_GSMcomm.ps_commu->RCNT,11);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								gsmDelayMS(5);	// µÈ´ý½ÓÊÕÍêÕû
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â
								p++;
								for(k=0,dotNum=0;	(k<s_GSMcomm.ps_commu->RCNT) && (*(s_GSMcomm.pRecBuf + p) != '\r') && (*(s_GSMcomm.pRecBuf + p) != '\n');	k++)
								{
										if(*(s_GSMcomm.pRecBuf + p + k) == ',')
												dotNum++;	
								}
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								if(dotNum == 1)
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_BT)
												myPrintf("[BT]: Pass pair req!\r\n");
										#endif	
										s_MboxBT.func =BT_PAIR_PASS;											// PasskeyÆ¥ÅäÒµÎñ
										if(OSRunning > 0)
												OSMboxPost(GSMBtAppMbox, (void *)&s_MboxBT);	// ·¢ËÍÓÊÏäµ½BTÈÎÎñ 										
								}
								else if(dotNum == 2)
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_BT)
												myPrintf("[BT]: Digi pair req!\r\n");
										#endif												
										s_MboxBT.func =BT_PAIR_DIGI;											// Êý¾ÝÆ¥ÅäÒµÎñ
										if(OSRunning > 0)
												OSMboxPost(GSMBtAppMbox, (void *)&s_MboxBT);	// ·¢ËÍÓÊÏäµ½BTÈÎÎñ 
								}
								else
								{
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_BT)
												myPrintf("[BT]: Unknow pair req!\r\n");
										#endif	
								}
						}									
							
						// ¼ì²âÁ¬½ÓÇëÇó									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTCONNECTING:",s_GSMcomm.ps_commu->RCNT,14);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â				
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								s_MboxBT.func =BT_CONNECT;													// Á¬½ÓÒµÎñ
							
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_BT)
										myPrintf("[BT]: Connect req!\r\n");
								#endif
								if(OSRunning > 0)
										OSMboxPost(GSMBtAppMbox, (void *)&s_MboxBT);		// ·¢ËÍÓÊÏäµ½BTÈÎÎñ 
						}	
						
						// ¼ì²âÀ¶ÑÀ¶Ï¿ª									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTDISCONN:",s_GSMcomm.ps_commu->RCNT,11);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								s_MboxBT.func =BT_DISCONNECT;												// ¶Ï¿ªÁ¬½ÓÒµÎñ							
								BSP_GSM_SetFlag(DEF_BTDING_FLAG);										// ÉèÖÃÀ¶ÑÀÔÚÏß±êÖ¾
								
								#if(DEF_GSMINFO_OUTPUTEN > 0)
								if(dbgInfoSwt & DBG_INFO_BT)
										myPrintf("[BT]: Disconnect!\r\n");
								#endif
						}	

						// ¼ì²âÀ¶ÑÀÊý¾Ý
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+BTSPPMAN: *",s_GSMcomm.ps_commu->RCNT,12);		
						if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) 			= '\0';								// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â
								memset((u8 *)&s_MboxBT,	0,	sizeof(s_MboxBT));
								s_MboxBT.func =BT_RXDATA;														// ÊÕµ½Êý¾Ý	
								s_MboxBT.cnnId =*(s_GSMcomm.pRecBuf + p + 11);			// »ñÈ¡Á¬½ÓID
								
								if(OSRunning > 0)
										OSMboxPost(GSMBtDataMbox, (void *)&s_MboxBT);		// ·¢ËÍÓÊÏäµ½BTÈÎÎñ 
						}	
				}	
				
				//	¼ì²âGPRS½ÓÊÕÊý¾Ý¼°ÍøÂçÒì³£	
				if(s_GSMcomm.GSMComStatus	>=	GSM_ONLINE)
				{						
						// ¼ì²é½ÓÊÕÊý¾Ý									
						p = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+CIPRXGET: 1,*",s_GSMcomm.ps_commu->RCNT,14);		
						if((p	!=  0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
						{
								*(s_GSMcomm.pRecBuf + p) = '\0';										// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â
								if(OSRunning > 0)
										OSSemPost(GSMGprsDataSem);		// ½«ÊÕµ½Êý¾ÝÒÔÐÅºÅÁ¿µÄÐÎÊ½Í¶µÝ³öÈ¥(¿¼ÂÇµ½¿ÉÄÜºÜ¶ÌÊ±¼äÊÕµ½ºÜ¶àÊý¾ÝÓÊÏä²»ºÏÊÊ) 
						}

						// ¼ì²éÍøÂçÒì³£
						p = 0;
						q = 0;
						p = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"+PDP: DEACT",s_GSMcomm.ps_commu->RCNT,11);	// Éè±¸GPRS PDP»·¾³¶Ï¿ª
						q = StringSearch((u8 *)s_GSMcomm.pRecBuf,(u8 *)"0, CLOSE",s_GSMcomm.ps_commu->RCNT,8);			// Í¨µÀ0·þÎñÆ÷Ö÷¶¯¶Ï¿ª		
						if(((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT)) || ((q	!=	0xffff)&&(q	<=	s_GSMcomm.ps_commu->RCNT))) 
						{
								if((p	!=	0xffff)&&(p	<=	s_GSMcomm.ps_commu->RCNT))
								{								
										// GPRS PDPÉÏÏÂÎÄ¶Ï¿ª
										*(s_GSMcomm.pRecBuf + p) = '\0';								// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â																			
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("[GPRS]: GPRS PDP break!\r\n");
										#endif
								}
								else
								{
										// ·þÎñÆ÷Ö÷¶¯¶Ï¿ª
										*(s_GSMcomm.pRecBuf + q) = '\0';								// É¾³ý×Ö·û´®·ÀÖ¹Í¬Ò»×Ö·û´®ÖØ¸´¼ì²â										
										#if(DEF_GSMINFO_OUTPUTEN > 0)
										if(dbgInfoSwt & DBG_INFO_GPRS)
												myPrintf("[GPRS]: Server break!\r\n");
										#endif
								}
								s_GSMcomm.GSMComStatus	=	GSM_ONCMD;								// ÖÃGSM¶Ë¿Ú×´Ì¬
								BSP_GSM_ReleaseFlag(DEF_GDATA_FLAG);		
						}
				}
		}
}
/*
*********************************************************************************************************
*                                         BSP_GSM_Init()
*
* Description : Init. the GSM device and data type
*
* Argument(s) : 
*
* Return(s)   :	
*
* Caller(s)   : 
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_GSM_Init (void)
{
    /* Init variable */
    s_GSMcomm.ps_commu 			= &ComGSM;
    s_GSMcomm.sendstart 		= StartSend_GPRS;
    s_GSMcomm.pSendBuf 			= SBufGPRS;
    s_GSMcomm.pRecBuf 			= RBufGPRS;
    s_GSMcomm.Timer 				= 0;
		s_GSMcomm.TimerApp 			= 0;
    s_GSMcomm.ActiveCmd 		= 0;
    s_GSMcomm.SetupPhase 		= PowerOff;				
    s_GSMcomm.CmdPhase 			= CommandIdle;
		s_GSMcomm.HardWareSta		=	DEF_GSMHARD_CHEKING;	
    s_GSMcomm.GSMComStatus 	= GSM_POWOFF;

		memset((u8 *)&err_Gsm,	0,	sizeof(err_Gsm));	// Çå´íÎó±äÁ¿

		s_ATcomm.Slen						=	0;
		s_ATcomm.Rlen						=	0;
		s_ATcomm.pSendBuf				=	SBufAT;
		s_ATcomm.pRecBuf				=	RBufAT;
		s_ATcomm.timeout				=	0;
		s_ATcomm.CheckMode			=	DEF_COMMON_MODE;
				
		BSP_USART_Init(DEF_GSM_UART,	DEF_GSM_REMAP,	DEF_GSM_BAUT);

		GSMRingPinInit();			//	³õÊ¼»¯RINGÖÐ¶Ï¹Ü½Å
		
		GSM_DISABLE_RES_IT();		//	½ûÖ¹½ÓÊÕÖÐ¶Ï
		GSM_DISABLE_SEND_IT();	//	½ûÖ¹·¢ËÍÖÐ¶Ï

		// ·ÀÖ¹µ±GSMµçÔ´¿ª¹ØÐ¾Æ¬»µµô(±ÈÈçµçÔ´Ö±Í¨)Ê±Ó²¼þÉÏµçGSMÒ²ÄÜÒÔ¼ì²â³É¹¦,µ«ÊÇÈí¼þ¸´Î»¼ì²âÊ§°Ü 
		IO_GSM_POW_ON();
		gsmDelayMS(1000);
		IO_GSM_POW_OFF();
		gsmDelayMS(100);
		GSMFeedWDGProcess();	// Î¹¹·	
		
		#if(DEF_INITINFO_OUTPUTEN > 0)
		if(dbgInfoSwt & DBG_INFO_SYS)
		{
				if(s_GSMcomm.HardWareSta == DEF_GSMHARD_CHEKING)
						myPrintf("[Init]: GSM........(checking)!\r\n");
				else if(s_GSMcomm.HardWareSta == DEF_GSMHARD_NONE_ERR)
						myPrintf("[Init]: GSM........(ok)!\r\n");
				else
						myPrintf("[Init]: GSM........(error)!\r\n");
		}
		#endif	
		
}
/*****************************************end*********************************************************/

