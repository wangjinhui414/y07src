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
*                                        BOARD SUPPORT PACKAGE
*
*                                     			  bsp_h.h
*                                              with the
*                                   			   Y05D Board
*
* Filename      : bsp_c.c
* Version       : V1.00
* Programmer(s) : Felix
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GLOBLAS
#include 	<stdio.h>
#include 	<string.h>
#include 	<stdarg.h>   // 为了支持可变长参数
#include 	<time.h>		 // 为了支持计算1970年秒转年月日时间	
#include	"cfg_h.h"
#include	"bsp_h.h"
#include	"bsp_storage_h.h"
#include	"bsp_gps_h.h"
#include	"bsp_gsm_h.h"
#include	"bsp_acc_h.h"
#include 	"ucos_ii.h"



/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#if(USE_MDK_WINDOW_OUTPUT > 0)
		//////////////////////////////////////
		// 
		#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
		#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
		#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
		#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
		#define TRCENA          0x01000000

		struct __FILE { int handle; /* Add whatever is needed */ };
		FILE __stdout;
		FILE __stdin;
#else	////////////////////////////////
		#ifdef __GNUC__
				/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
				 set to 'Yes') calls __io_putchar() */
				#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
		#else
				#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
		#endif /* __GNUC__ */
#endif ///////////////////////////////

                         
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

extern			OS_EVENT		*DBGMutexSem; //	DBG接口printf互斥信号量
static			vu8					DBGMutexFlag	=1;		// 如果运行在无OS状态下由该变量实现Printf互斥操作

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static	void	BSP_GPIO_Set_Default	(void);


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                     		GLOBLE APP Tool
*********************************************************************************************************
*/

/*
*********************************************************************************************************
* Function Name  : GetCpuHwID
* Description    : 获取CPU 硬件ID号 96位 12字节         
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	GetCpuHwID (u8 *idBuf)
{
		u8	i=0;
		vu32	CPU_ID_Addr = 0x1FFFF7E8;

		for(i=0;	i<12;	i++)
		{
				*(idBuf + i) = *(vu8*)(CPU_ID_Addr + i);				
		}
}
/*
*********************************************************************************************************
* Function Name  : SoftDelayMS
* Description    : Dead loop         
* Input          : dllay time
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	SoftDelayMS(u32	Nms)
{
		u32	i=0,tmploop=0;
		tmploop	=	Nms; 
		while(tmploop--)
		{
			for(i=0;	i<=8980;	i++)
			{}
		}
}
/*
******************************************************************************
* Function Name  : HEX2BCD
* Description    : 将输入的数值转化成十进制的BCD模式
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
u8 Hex2BCD (u8 hex_data)
{
    u8 bcd_data=0;
    u8 temp=0;
    temp=hex_data%10;
    bcd_data=hex_data/10<<4;
    bcd_data=bcd_data|temp;
    return bcd_data;
}
/*
*********************************************************************************************************
* Function Name  : IsIntString
* Description    : 判断是否为合法的整数型字符串数组
* Input          : 
* Output         : None
* Return         : 成功-返回实际长度，失败-返回0xffff
*********************************************************************************************************
*/
u16	IsIntString (u8 *pBuf,	u16	len)
{
		u16	i=0;
		for(i=0;	i<len;	i++)
		{
				if(Is_0to9(*(pBuf+i)))
				{
						;
				}
				else
						return	0xffff;
		}
		return	i;
}
/*
*********************************************************************************************************
* Function Name  : IsA2ZString
* Description    : 判断是否为合法的A~Z或a~z型字符串数组
* Input          : 
* Output         : None
* Return         : 成功-返回实际长度，失败-返回0xffff
*********************************************************************************************************
*/
u16	IsA2ZString (u8 *pBuf,	u16	len)
{
		u16	i=0;
		for(i=0;	i<len;	i++)
		{
				if(Is_atoz(*(pBuf+i)) || Is_AtoZ(*(pBuf+i)))
				{
						;
				}
				else
						return	0xffff;
		}
		return	i;
}
/*
*********************************************************************************************************
* Function Name  : IsHexString
* Description    : 判断是否为合法的十六禁止形式字符串数组
* Input          : 
* Output         : None
* Return         : 成功-返回实际长度，失败-返回0xffff
*********************************************************************************************************
*/
u16	IsHexString (u8 *pBuf,	u16	len)
{
		u16	i=0;
		for(i=0;	i<len;	i++)
		{
				if(Is_0to9(*(pBuf+i)) || Is_atof(*(pBuf+i)) || Is_AtoF(*(pBuf+i)))
				{
						;
				}
				else
						return	0xffff;
		}
		return	i;
}
/*
*********************************************************************************************************
* Function Name  : IsDotIntString
* Description    : 判断是否为合法的小数或整数字符串数组
* Input          : 
* Output         : None
* Return         : 成功-返回实际长度，失败-返回0xffff
*********************************************************************************************************
*/
u16	IsDotIntString (u8 *pBuf,	u16	len)
{
		u16	i=0,j=0;

		for(i=0;	i<len;	i++)
		{
				if(*(pBuf+i) == '.')
				{
						j++;
						if((j>=2) || (i == 0) || (i == len-1))
								return	0xffff;
				}
		}
		for(i=0;	i<len;	i++)
		{
				if(Is_0to9(*(pBuf+i)) || *(pBuf+i) == '.')
				{
						;
				}
				else
						return	0xffff;
		}
		return	i;
}
/*
*********************************************************************************************************
*                                           Asc2Hex()
*
* Description : two bit asc code transfor one hex asc code range from '0' to '9'
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : 
*********************************************************************************************************
*/
u8	Asc2Hex(u8	*buf)
{
		u8 hex =0,tmpdataH=0,tmpdataL=0;
		hex = hex;
		tmpdataH = *buf;
		tmpdataH = (tmpdataH - 0x30) * 16;
		tmpdataL = *(buf+1);
		tmpdataL = tmpdataL - 0x30;
		hex	= tmpdataH | tmpdataL;
		return hex;
}
/*
*********************************************************************************************************
* Function Name  : 										String2HexArray()
* Description    : 将字符串数组转换成数值数组，并返回数值数组长度(输入的字符串数组长度必须为偶数)        
* Input          : 
* Output         : 
* Return         : 成功:返回数值数组长度;失败:返回0xffff
*********************************************************************************************************
*/
u16		String2HexArray (u8	*pSrc,	u16	SrcLen,	u8 *pObj)
{    
    u8  tmpBuf[3]="";
    u16	i=0;

    memset(tmpBuf,'\0',sizeof(tmpBuf));

    if(SrcLen % 2 != 0)
        return  0xffff;

    for(i=0; i<SrcLen; i++)
    {
        if((Is_0to9(*(pSrc+i)))|| (Is_AtoF(*(pSrc+i))) || (Is_atof(*(pSrc+i))))
        {
            ;
        }
        else
            return  0xffff;
    }

    for(i=0; i<SrcLen; i+=2)
    {
        memcpy(tmpBuf,(pSrc+i), 2);
        sscanf((char *)tmpBuf,	"%02X",	(pObj+i/2))	;		// 将HEX字符串类型转化成数值
    }
    return  i/2;
}
/*
*********************************************************************************************************
* Function Name  : 										ADCalculateProcess()
* Description    : 将源数据去掉最大值去掉最小值后取平均值输出        
* Input          : 
* Output         : 
* Return         : 如果错误则返回0xffff 否则返回实际值
*********************************************************************************************************
*/
u16	ADCalculateProcess(u16 *pBuf,	u16 len) 
{ 		
		u16	i=0,max=0,min=0;
		u32	sum=0;
		
		if(len == 0)
				return	0xffff;

		max = *pBuf;
	  min = *pBuf;

		for(i=0;  i<len;	i++)
		{
		   if (*(pBuf + i) > max)
		   		max=*(pBuf + i);
		   if (*(pBuf + i) < min)
		   		min=*(pBuf + i);
				
				sum=sum+*(pBuf + i);
		}
		sum = (sum - max - min) / (len -2);
		return (u16)sum;	  
}
/*
*********************************************************************************************************
* Function Name  : 										CheckIsNum()
* Description    : 输入数据指针，返回是否全为数值(‘.’也算作为数值因为有可能输入的是小数)        
* Input          : 
* Output         : 
* Return         : 如果全为数值，则返回0否则返回-1
*********************************************************************************************************
*/
s8		CheckIsNum (u8 *pBuf,	u16 len)
{
		u16	i;
		for(i=0;	i<len;	i++)
		{
				if(Is_0to9(*(pBuf + i)) || (*(pBuf + i) == '.'))
				{}
				else
					return	-1;
		}
		return	0;
}	 
/*
*********************************************************************************************************
* Function Name  : 										MakeStrNum2USC2()
* Description    : 输入字符窜数值数据指针，输出符合USC2码的字符并返回长度如"1234"->"0031003200330034"        
* Input          : 
* Output         : 
* Return         : 如果字符区不为数值则返回0xffff否则返回长度
*********************************************************************************************************
*/
u16		MakeStrNum2USC2 (u8 *pSrc,	u16 srcLen,	u8	*pObj)
{
		u8	tmpBuf[10];
		u16 i,j;
	  if(CheckIsNum(pSrc,	srcLen) != 0)
			return	0xffff;
		else
		{
				for(i=0,j=0;	i<srcLen;	i++,j+=4)
				{
						sprintf((char *)tmpBuf,	"%04X",	*(pSrc + i));
						memcpy((pObj + j),	tmpBuf,	4);				
				}
				return	j;
		}
}
/*
*********************************************************************************************************
* Function Name  : 										CheckPhoneFormat()
* Description    : 检查输入的字符串是否符合报警电话的格式如12349584#123947584#1239495#并将这三个电话输出        
* Input          : 如果某一电话参数区为<4或>16则长度返回0xffff但是如果长度为0则认为合法直接写0
* Output         : 
* Return         : 如果是返回0，否则返回-1
*********************************************************************************************************
*/
s8		CheckPhoneFormat (u8 *pSrc,	u16 srcLen,	u8	*pObj1,	u16 *obj1Len,	u8	*pObj2,	u16 *obj2Len, u8	*pObj3,	u16 *obj3Len)
{
		u16 i,j;
		
		// 检查‘#’号是否为3个并且最后一个是否为‘#’
		j=0;
		for(i=0;	i<srcLen;	i++)
		{
				if(*(pSrc + i) == '#')
						j++;
		}
		if((j != 3) || (*(pSrc + srcLen -1) != '#')) // 合法格式首尾必须是有3个‘#’并且结尾是‘#’
				return	-1;
		else
		{
				// 复制号码1
				for(i=0,j=0;	(*(pSrc + i) != '#');	i++,j++)
				{
						*(pObj1 + j) = *(pSrc + i);
				}
				*(pObj1 + j) = '\0';
				if(((j < 4) && (j != 0)) || (j > 16))
						*obj1Len	=	0xffff;
				else
						*obj1Len 	= j;
				
				// 复制号码2
				i++;
				for(j=0;	(*(pSrc + i) != '#');	i++,j++)
				{
						*(pObj2 + j) = *(pSrc + i);
				}
				*(pObj2 + j) = '\0';
				if(((j < 4) && (j != 0)) || (j > 16))
						*obj2Len	=	0xffff;
				else
						*obj2Len 	= j;

				// 复制号码3
				i++;
				for(j=0;	(*(pSrc + i) != '#');	i++,j++)
				{
						*(pObj3 + j) = *(pSrc + i);
				}
				*(pObj3 + j) = '\0';
				if(((j < 4) && (j != 0)) || (j > 16))
						*obj3Len	=	0xffff;
				else
						*obj3Len 	= j;
				return	0;
		}
}

/*
*********************************************************************************************************
* Function Name  : 										SNCode2barCode()
* Description    :         
* Input          : 
* Output         : 
* Return         : 错误返回0xffff 正确返回0
*********************************************************************************************************
*/
u16		SNCode2barCode (u32 *srcID,	u32 *objID)
{
		if((*srcID & 0xff000000) >> 24 != 0x1A)
				return	0xffff;
		else
				*objID	=	*srcID ^ 0x1ABCDEF2;
		return	0;
}
/*
*********************************************************************************************************
* Function Name  : 										StringAdd()
* Description    : Dead loop         
* Input          : dllay time
* Output         : None
* Return         : Len of pStr
*********************************************************************************************************
*/
u16		StringAdd (u8	*pStr,	u8	*pStrF) 
{ 
	  u16 i=0,j=0; 
	
	  for(i=0;	*(pStr+i)!='\0';	i++)
		{;}
		
		for(j=0;	*(pStrF + j)!='\0';	j++)
		{
				*(pStr + i + j)=*(pStrF + j);
		}
	
		pStr[i+j]='\0';
		
		return	(i+j);
} 
/*
*********************************************************************************************************
* Function Name  : 										StringConnect()
* Description    : 字符串链接函数       
* Input          : 
* Output         : 
* Return         : 
*********************************************************************************************************
*/
u16		StringConnect (u8	*pStr,	u8	*pStrF,		u8	*pStrB) 
{ 
	  u16 i=0,j=0; 
	  for(i=0;	*(pStrF+i)!='\0';	i++) 
	  		*(pStr + i)=*(pStrF + i); 
	   
	  for(j=0;	*(pStrB+j)!='\0';	j++)  
	  		*(pStr + i + j)=*(pStrB + j); 
	
		pStr[i+j]='\0';
	
		return	(i+j);
} 
/*
*********************************************************************************************************
* Function Name  : 										Hex2StringArray()
* Description    : 将数值数组转换成字符串数组，并返回字符串数值长度        
* Input          : 
* Output         : 
* Return         : 如果全为数值，则返回0否则返回-1
*********************************************************************************************************
*/
u16		Hex2StringArray (u8	*pSrc,	u16	SrcLen,	u8 *pObj)
{
		u16	i=0;

		for(i=0;	i<SrcLen;	i++)
		{
				sprintf((char *)(pObj + i * 2),	"%02X",	*(pSrc + i));
		}
	  *(pObj + i * 2) = '\0';
		return	(i * 2);
}
/*
*********************************************************************************************************
*                                     StringSearch()
*
* Description: 	Search and matching the string form the source to object and point the position of the first
*							 	byte . （注:如果对比的原字符串或目标字符串为空则返回错误）
*
* Argument(s): 	*pSource - Point to the first byte of the source.
*        				*pObject - Point to the first byte of the object.
*        				SrcLen   - The length of the source array.
*        				ObjLen   - The length of the object array.
*
* Return(s)  :  j 					- success	 //返回的位置是对应的数组中的位置记为j，与实际位置N j=N-1
*								0xffff 			- failure	 //[注]支持通配符操作‘*’
* Note(s)    : 
*********************************************************************************************************
*/
u16 	StringSearch(u8 *pSource,u8 *pObject,u16 SrcLen,u16 ObjLen)
{
    u16 i=0,j=0;

		if((SrcLen == 0) || (ObjLen == 0))
				return	0xffff;									/* 如果原字符串或目标字符串有一个长度为0或为空则非法退出 */
    
		while(j<SrcLen)								 			/* 判断是否源字符串指针已经移到最后 */
		{
        if((j+ObjLen) > SrcLen)
				{
            return 0xffff;		 					/* 说明接收到的字符串(源字符串)长度<目的字符串长度 意味着接收长度不够 */
        }
        for(i=0;i<ObjLen;i++)
				{
            if(*(pSource+i) != *(pObject+i))
						{
								if(*(pObject+i) == '*')		/* 如果是通配符‘*’则跳过 */
								{}
								else
	                	break;
						}
						else
						{}										 			/* 成功匹配了一个字符然后目的字符串指针将后移1位 */
        }
        if(i==ObjLen)						  
            return j;							 			/* 说明循环比对完成(这里目的字符串与源字符串里相同的部分必须为连续出现才行，不支持间断比对) */
        else											 			/* 如需判断间断的匹配需加入通配符‘*’来占位 */
				{
            pSource++;						 			/* 源字符串指针后移，继续比对 */
            j++;
        }  
    }
    return 0xffff;
}
/*
******************************************************************************
* Function Name  : Mktime
* Description    : 将从1970年到现在的年月日时分秒转换成秒
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
u32 Mktime (u32 year, u32 mon,u32 day, u32 hour,u32 min, u32 sec)
{
    if (0 >= (int) (mon -= 2))
		{    /**//* 1..12 -> 11,12,1..10 */
         mon += 12;      /**//* Puts Feb last since it has leap day */
         year -= 1;
    }

    return (((
             (u32) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
             year*365 - 719499
          )*24 + hour /**//* now have hours */
       )*60 + min /**//* now have minutes */
    )*60 + sec; /**//* finally seconds */
}
/*
******************************************************************************
* Function Name  : Gettime
* Description    : 将从1970年的秒数转换成年月日时分秒
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void Gettime (u32 count,	SYS_DATETIME *cnt)
{
		time_t t=count;
		struct tm *p;
	
		p=localtime(&t); 
		
		cnt->year		=1900+p->tm_year;
		cnt->month	=1+p->tm_mon;
		cnt->day		=p->tm_mday;
		cnt->hour		=p->tm_hour;
		cnt->minute	=p->tm_min;
		cnt->second	=p->tm_sec;
}
/*
******************************************************************************
* Function Name  : SumCheck16Pro
* Description    : 16位累加和检验函数
* Input          : *srcBuf :目标数据区首指针(必须符合格式sumH+sumL+data......格式)
*								 : srcLen  :目标数据区长度
* Output         : None
* Return         : 0
******************************************************************************
*/
u16	 SumCheck16Pro (u8 *srcBuf,	u16	srcLen)
{
		u16 i=0,	sum=0;
		sum = *srcBuf;
		for(i=1;	i<(srcLen-1);	i++)
		{
				sum += *(srcBuf + i);
		}
		return	sum;
}

/*
*********************************************************************************************************
* Function Name  : BSP_RCC_Configuration
* Description    : 				      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_RCC_Configuration (void)
{		
		RCC_ClocksTypeDef RCC_ClockFreq;
		
		SystemInit();		//源自system_stm32f10x.c文件,只需要调用该函数就可实现对RCC的配置
		
		// 使能LSI看门狗振荡器
		RCC_LSICmd(ENABLE);//打开LSI
 		while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);//等待直到LSI稳定
		
		/**************************************************
		获取RCC的信息,调试用
		请参考RCC_ClocksTypeDef结构体的内容,当时钟配置完成后,
		里面变量的值就直接反映了器件各个部分的运行频率
		***************************************************/
		RCC_GetClocksFreq(&RCC_ClockFreq);
		
		/* 这个配置可使外部晶振停振的时候,产生一个NMI中断,不需要用的可屏蔽掉*/
		//RCC_ClockSecuritySystemCmd(ENABLE);

}
/*
*********************************************************************************************************
* Function Name  : BSP_NVIC_Configuration
* Description    : 				      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_NVIC_Configuration(void)
{
		NVIC_InitTypeDef NVIC_InitStructure;
	
		/*!!注意!!*/
		/*应用程序参数地址定义在CPU FLASH中间位置(具体定义见hal_h.h宏定义)*/
	  /*如果去掉程序偏移,当写系统参数时将导致直接改写应用程序代码,从而导致死机*/

		//设置向量地址偏移
		/* Set the Vector Table base location at 0x0800A000 */
		NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0xA000);
		//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
		
		/* Configure one bit for preemption priority */
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
		/* USART3_IRQn 中断 [K] 2015-1-21 by:gaofei 根据OBD要求优先级调整 */
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* PVD_EXIT16中断	[VDD]电压低监控中断 */ 
		NVIC_InitStructure.NVIC_IRQChannel = PVD_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);	
		
		/* USART1_IRQn中断 [GSM] */	
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* USART2_IRQn中断 [GPS] */
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);		
		
		/* CAN1_IRQn中断  [OBD]*/
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;	
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* CAN2_IRQn中断  [Car Control CAN Bus]*/
		NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		/* WAKEUP_DET低脉冲中断	[IO]*/
		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* RING_DET, UART5_RX中断	[IO]*/
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* ACCINT1_DET中断	[IO]*/
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 8;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* UART4_IRQn 中断 [Debug] */
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 9;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* UART5_IRQn 中断 [Control] */
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* RTC_Arlam中断		[RTC]*/
		NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 11;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		/* UART4_RX(DBG)中断 [IO]*/
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 12;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                         OS CORTEX-M3 FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                            BSP_CPU_DisAllInterupt()
*
* Description : 禁止所有中断防止bootloader中使能中断后到应用程序中未初始化之前产生(应该在OS使能之前调用,关所有中断)
*
* Argument(s) : none.
*
* Return(s)   : 
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_CPU_DisAllInterupt	(void)
{
		s8 scCnt = 0;
		
		// 关systick中断
		SysTick->CTRL  = SysTick_CTRL_ENABLE_Msk;  
		// 关其它中断源
//		for (scCnt = 0; scCnt <= 67; scCnt++)
//		{
//				NVIC_DisableIRQ((IRQn_Type)scCnt);
//		}
}

/*
*********************************************************************************************************
*                                            BSP_CPU_Reset()
*
* Description : CPU SOFT RESET 
*
* Argument(s) : none.
*
* Return(s)   : 
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_CPU_Reset(void)
{
		/* system soft reset */
		NVIC_SystemReset();		// 函数在core_m3.c中
		//*((u32 *)0xE000ED0C) = 0x05fa0004;
		while(1);
}
/*
*********************************************************************************************************
*                                            BSP_CPU_GET_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  BSP_CPU_GET_ClkFreq (RCC_ClocksTypeDef	*clockFreq)
{
		RCC_GetClocksFreq(clockFreq);
}

/*******************************************************************************
* Function Name  : BSP_SYSCLKConfig_FromStop
* Description    : Configures system clock after wake-up from STOP: enable HSE, PLL
*                  and select PLL as system clock source.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BSP_SYSCLKConfig_FromStop (void)
{
		ErrorStatus 			HSEStartUpStatus;
		
		/* Enable HSE */
		RCC_HSEConfig(RCC_HSE_ON);
		
		/* Wait till HSE is ready */
		HSEStartUpStatus = RCC_WaitForHSEStartUp();
		
		if(HSEStartUpStatus == SUCCESS)
		{
			
#ifdef STM32F10X_CL	
				/* Enable PLL2 */ 
				RCC_PLL2Cmd(ENABLE);

				/* Wait till PLL2 is ready */
				while(RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
				{
				}
#endif
			
				/* Enable PLL */ 
				RCC_PLLCmd(ENABLE);
				
				/* Wait till PLL is ready */
				while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
				{
				}
				
				/* Select PLL as system clock source */
				RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
				
				/* Wait till PLL is used as system clock source */
				while(RCC_GetSYSCLKSource() != 0x08)
				{
				}
		}
}
/*
*********************************************************************************************************
*                                         BSP_WDTDOG_Init()
*
* Description : Init the watchdog time
*
* Argument(s) : none.
*
* Return(s)   : 
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_WDTDOG_Init(void)
{
		
		/* IWDG timeout equal to 280 ms (the timeout may varies due to LSI frequency
	     dispersion) */
	  /* Enable write access to IWDG_PR and IWDG_RLR registers */
	  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	  /* IWDG counter clock: 40KHz(LSI) 256/40 每记一次数时间为： 6.4ms*/ 
	  IWDG_SetPrescaler(IWDG_Prescaler_256);
	
	  /* Set counter reload value to 349 总计时时间为：6.4ms * 1875 = OS_SYSWDG_TIMES */ 
		IWDG_SetReload((u16)(SYSWDG_TIMEOUT * 1000 /6.4 + 0.5));  
	
	  /* Reload IWDG counter */
	  IWDG_ReloadCounter();

}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                            Board I/O FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                              BSP_GPIO_Init()
*
* Description : Initialize the board's COMMON	GPIO (include led , keyboard and other con and det i/0).
*
* Argument(s) : //特殊功能管脚如：EXIT中断脚，也将被暂时初始化为输入
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init_All().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void  BSP_GPIO_Init (void)
{
    GPIO_InitTypeDef  gpio_init;
					  																															/*************PortA************/	
		
		/* GPS_POWEN | GPS_RTC_POW | GSM_POWON */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio_init.GPIO_Pin   = BSP_GPIOA_GPS_POWEN | BSP_GPIOA_GPS_RTC_POW | BSP_GPIOA_GSM_POWON;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &gpio_init);

																																					/*************PortB************/
		/* K_SW_L | SP3232EN | LED_DATA | LED_RUN | CAN2_STB | CAN1_STB */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  	gpio_init.GPIO_Pin   = BSP_GPIOB_K_SW_L | BSP_GPIOB_SP3232EN |  BSP_GPIOB_LED_DATA | BSP_GPIOB_LED_RUN |
  												 BSP_GPIOB_CAN2_STB | BSP_GPIOB_CAN1_STB;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &gpio_init);
		
																																					/*************PortC************/
		/* ACC_INT1 | GSM_RING | WAKEUP | GSM_POWDET */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    gpio_init.GPIO_Pin   = BSP_GPIOC_ACC_INT1 	| BSP_GPIOC_GSM_RING | BSP_GPIOC_WAKEUP | BSP_GPIOC_GSM_POWDET;
    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &gpio_init);

		/* GSM_DTR | FLASH_WP | FLASH_CS | SPAKEREN | GSM_POWEN | GSM_RESET */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    gpio_init.GPIO_Pin   = BSP_GPIOC_GSM_DTR | BSP_GPIOC_FLASH_WP 	|  BSP_GPIOC_FLASH_CS | BSP_GPIOC_SPAKEREN | 
    											 BSP_GPIOC_GSM_POWEN | BSP_GPIOC_GSM_RESET;	     											 
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC, &gpio_init);

																																					/*************PortD************/

		
																																					/*************PortE************/
		
		/* GPIO DEFAULT*/
		BSP_GPIO_Set_Default();	
}
/*
*********************************************************************************************************
*                                             BSP_GPIO_Set_Default()
*
* Description : Set default value of GPIO
*
* Argument(s) : 
*
* Return(s)   : none.
*
* Caller(s)   : BSP_GPIO_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
static	void	BSP_GPIO_Set_Default(void)
{	
		// Port_A //		
		/* GPS_POWEN=0 | GPS_RTC_POW=0 | GSM_POWON */	
		GPIO_ResetBits(GPIOA,	BSP_GPIOA_GPS_POWEN  | BSP_GPIOA_GPS_RTC_POW | BSP_GPIOA_GSM_POWON);

		// Port_B //
		/* K_SW_L=0 | SP3232EN=0 | LED_DATA=1 | LED_RUN=1 | CAN2_STB=1(Sleep mode) |  CAN1_STB=1(Sleep mode) */	
		GPIO_ResetBits(GPIOB,	BSP_GPIOB_K_SW_L | BSP_GPIOB_SP3232EN);
		GPIO_SetBits	(GPIOB,	BSP_GPIOB_LED_DATA | BSP_GPIOB_LED_RUN | BSP_GPIOB_CAN2_STB | BSP_GPIOB_CAN1_STB);	

		// Port_C //
		/* GSM_DTR=0(GSM Don't sleep) | FLASH_WP=1 | FLASH_CS=1 | SPAKEREN=0 | GSM_POWEN=0 | GSM_RESET=0 */
		GPIO_ResetBits(GPIOC,	BSP_GPIOC_GSM_DTR | BSP_GPIOC_SPAKEREN | BSP_GPIOC_GSM_POWEN | BSP_GPIOC_GSM_RESET);
	  GPIO_SetBits	(GPIOC,	BSP_GPIOC_FLASH_WP | BSP_GPIOC_FLASH_CS);

		BSP_USART_Init(DEF_DBG_UART,	DEF_DBG_REMAP,	DEF_DBG_BUAD);	// 初始化DBG串口
									 				
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                              EXTI FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/


/*
*********************************************************************************************************
* Function Name  : BSP_SYSEXIT_Init
* Description    : 对外的外部中断初始化(ACC_INT1,WAKEUP,PVD)				      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_SYSEXIT_Init (void)
{
		EXTI_InitTypeDef EXTI_InitStructure;
		
		// Enbale	exti clock 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	  // Connect Key Button EXTI Line to Key Button GPIO Pin); 	
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);
		//GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource2);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource3);
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource11);
	 
		// Clear the Key Button EXTI line pending bit
	  EXTI_ClearITPendingBit(EXTI_Line0);
		//EXTI_ClearITPendingBit(EXTI_Line2);
		EXTI_ClearITPendingBit(EXTI_Line3);
		EXTI_ClearITPendingBit(EXTI_Line16);	// PVD中断
		EXTI_ClearITPendingBit(EXTI_Line11);
	
		// Configure Key Button EXTI Line to generate an interrupt on falling edge 
		
		// PC0_ACCINT1_DET 
	  EXTI_InitStructure.EXTI_Line 		= EXTI_Line0;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	  EXTI_Init(&EXTI_InitStructure);
	  
	  // PC2_RING_DET, UART5_RX_DET
	  // RING中断由GSM驱动函数提供(gsm底层将重新初始化该中断)
//		EXTI_InitStructure.EXTI_Line 		= EXTI_Line2;
//	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
//	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
//	  EXTI_Init(&EXTI_InitStructure);
		
		// PC3_WAKEUP_DET 
	  EXTI_InitStructure.EXTI_Line 		= EXTI_Line3;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	  EXTI_Init(&EXTI_InitStructure);
	  
		// PVD_EXIT
	  EXTI_InitStructure.EXTI_Line 		= EXTI_Line16;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	  EXTI_Init(&EXTI_InitStructure);
		
		// PC11_UART4RX_EXIT
	  EXTI_InitStructure.EXTI_Line 		= EXTI_Line11;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	  EXTI_Init(&EXTI_InitStructure);

}
/*
*********************************************************************************************************
* Function Name  : BSP_SYSEXIT_IT_CON
* Description    : (ACC_INT1,WAKEUP,PVD)中断控制				      
* Input          : swt: 睡眠控制开关,	ENABLE or DISABLE
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_SYSEXIT_IT_CON (u8	swt,	u8	newSta)
{
		EXTI_InitTypeDef EXTI_InitStructure;

		// Clear the Key Button EXTI line pending bit
	  EXTI_ClearITPendingBit(EXTI_Line0);		
		EXTI_ClearITPendingBit(EXTI_Line3);
		EXTI_ClearITPendingBit(EXTI_Line16);	// PVD中断
		EXTI_ClearITPendingBit(EXTI_Line11);
	
		// Configure Key Button EXTI Line to generate an interrupt on falling edge 	
		if((swt & DEF_SLEEPSWT_IG) != 0)
		{
//				// PC3_WAKEUP_DET (IG唤醒不受开关控制) IG中断单独初始化(多任务环境开始后使能,且不关闭)
//			  EXTI_InitStructure.EXTI_Line 		= EXTI_Line3;
//			  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
//			  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
//			  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//			  EXTI_Init(&EXTI_InitStructure);
		}		

		if((swt & DEF_SLEEPSWT_COLLISION) != 0)
		{
				// PC0_ACCINT1_DET (加速度中断不受开关控制,一直开启状态)
//			  EXTI_InitStructure.EXTI_Line 		= EXTI_Line0;
//			  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
//			  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//			  EXTI_InitStructure.EXTI_LineCmd = (FunctionalState)newSta;
//			  EXTI_Init(&EXTI_InitStructure);

		}
		
		if((swt & DEF_SLEEPSWT_UART) != 0)
		{
				// PD2_CON UART RXD
//				EXTI_InitStructure.EXTI_Line 		= EXTI_Line2;
//				EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
//				EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//				EXTI_InitStructure.EXTI_LineCmd = ENABLE;	// 串口2接收中断与RING中断共享不允许关闭否则将不产生RING中断 
//				EXTI_Init(&EXTI_InitStructure);
		}		

		if((swt & DEF_SLEEPSWT_RING) != 0)
		{
				// 使能RING唤醒
//				BSP_GSMRING_IT_CON(1);						
		}

		// PVD_EXIT (PVD不受SWT控制)
	  EXTI_InitStructure.EXTI_Line 		= EXTI_Line16;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

}
/*
*********************************************************************************************************
* Function Name  : BSP_IG_IT_CON
* Description    : IG中断控制				      
* Input          : 
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_IG_IT_CON (u8	newSta)
{
		EXTI_InitTypeDef EXTI_InitStructure;

		// Clear the Key Button EXTI line pending bit
		EXTI_ClearITPendingBit(EXTI_Line3);
	
		// Configure Key Button EXTI Line to generate an interrupt on falling edge 	

		// PC3_WAKEUP_DET 
		EXTI_InitStructure.EXTI_Line 		= EXTI_Line3;
		EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		EXTI_InitStructure.EXTI_LineCmd = (FunctionalState)newSta;
		EXTI_Init(&EXTI_InitStructure);
}
/*
******************************************************************************
* Function Name  : BSP_PVD_IRQHook()
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	BSP_PVD_IRQHook (void)
{
		s_wakeUp.state	|=	DEF_WAKEUP_PVD;
	
		BSP_CPU_Reset();	// 2015-11-16 PVD终端直接复位设备
		
		// 无操作只作为唤醒系统
		#if(DEF_EXTIINFO_OUTPUTEN > 0)
		printf("[EXTI]: PVD!!\r\n");
		#endif		
			
}
/*
******************************************************************************
* Function Name  : BSP_ON_IRQHook()
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	BSP_ON_IRQHook (void)
{
		s_wakeUp.state	|=	DEF_WAKEUP_IG;
		
		// 无操作只作为唤醒系统
		#if(DEF_EXTIINFO_OUTPUTEN > 0)
		printf("[EXTI]: Wakup det!!\r\n");	// 中断中不能调用带互斥访问的myPrint函数
		#endif
}
/*
******************************************************************************
* Function Name  : BSP_ACC_INT1_IRQHook()
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	BSP_ACC_INT1_IRQHook (void)
{
		s_wakeUp.state	|=	DEF_WAKEUP_SHAKE;
		
		// 无操作只作为唤醒系统 
		#if(DEF_EXTIINFO_OUTPUTEN > 0)
		printf("[EXTI]: Acc int1!!\r\n");		// 中断中不能调用带互斥访问的myPrint函数
		#endif
}
/*
******************************************************************************
* Function Name  : BSP_RING_IRQHook()
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	BSP_RING_IRQHook (void)
{
		s_wakeUp.state	|=	DEF_WAKEUP_RING;
		
		// 无操作只作为唤醒系统
		#if(DEF_EXTIINFO_OUTPUTEN > 0)
		printf("[EXTI]: Ring!!\r\n");		// 中断中不能调用带互斥访问的myPrint函数
		#endif
}
/*
******************************************************************************
* Function Name  : BSP_UART4RX_IRQHook()
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	BSP_UART4RX_IRQHook (void)
{
		s_wakeUp.state	|=	DEF_WAKEUP_UART1;
		
		// 无操作只作为唤醒系统
		#if(DEF_EXTIINFO_OUTPUTEN > 0)
		printf("[EXTI]: Debug port!!\r\n");	// 中断中不能调用带互斥访问的myPrint函数
		#endif		
}
/*
******************************************************************************
* Function Name  : BSP_UART4RX_IRQHook()
* Description    : 
* Input          : None
* Output         : None
* Return         : 
******************************************************************************
*/
void	BSP_UART5RX_IRQHook (void)
{
		s_wakeUp.state	|=	DEF_WAKEUP_UART2;
		
		// 无操作只作为唤醒系统
		#if(DEF_EXTIINFO_OUTPUTEN > 0)
		printf("[EXTI]: Control port!!\r\n");	// 中断中不能调用带互斥访问的myPrint函数
		#endif		
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                              PVD FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_PVD_Init()
*
* Description : Initialize the board's PVD
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : PvdLevel:PWR_PVDLevel_2V2 ~ PWR_PVDLevel_2V9.
*********************************************************************************************************
*/
void	BSP_PVD_Init (u32 PvdLevel)
{
		// Enbale PWR & BKP
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE); // 使用PVD功能必须使能PWR及BKP

		// Configure the PVD Level to 2.9V */
  	PWR_PVDLevelConfig(PvdLevel);

  	// Enable the PVD Output */
  	PWR_PVDCmd(ENABLE);
}

/*
*********************************************************************************************************
*                                             BSP_PVDEXIT_IT_CON()
*
* Description : PVD EXIT Control
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : newSta :DEF_ENABLE or DEF_DISABLE
*********************************************************************************************************
*/
void	BSP_PVDEXIT_IT_CON (u8 newSta)
{
		EXTI_InitTypeDef EXTI_InitStructure;

		EXTI_ClearITPendingBit(EXTI_Line16);	 // PVD中断

		// PVD_EXIT (PVD不受SWT控制)
	  EXTI_InitStructure.EXTI_Line 		= EXTI_Line16;
	  EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	  EXTI_InitStructure.EXTI_LineCmd = (FunctionalState)newSta;
	  EXTI_Init(&EXTI_InitStructure);
}

/*
*********************************************************************************************************
*                                             BSP_PVD_GetSta()
*
* Description : Get the VDD PVD State
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : APP.
*
* Note(s)     : none.
*********************************************************************************************************
*/
u8	BSP_PVD_GetSta (void)
{
		if(PWR_GetFlagStatus(PWR_FLAG_PVDO) == SET)
				return	1;
		else
				return	0;
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                              ADC FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_ADC_Init()
*
* Description : Initialize the board's ADC
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_ADC_Init (void)
{
    ADC_InitTypeDef   adc_init;
    GPIO_InitTypeDef  gpio_init;

    ADC_DeInit(ADC1);
	
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);	// 2015-3-12 by：gaofei AD手册上要求时钟不能超过14M否则将影响精度（72/6=12M）	

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	// 采集电压AD值
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio_init.GPIO_Pin  = BSP_GPIOA_AIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &gpio_init);

    adc_init.ADC_Mode               = ADC_Mode_Independent;
    adc_init.ADC_ScanConvMode       = DISABLE;
    adc_init.ADC_ContinuousConvMode = ENABLE;
    adc_init.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    adc_init.ADC_DataAlign          = ADC_DataAlign_Right;
    adc_init.ADC_NbrOfChannel       = 1;
    ADC_Init(ADC1, &adc_init);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5); 	// 默认配置通道12,如果通道采用手动切换方式采集

    ADC_Cmd(ADC1, ENABLE);

		// 复位校准寄存器  
		ADC_ResetCalibration(ADC1);
		
		// 检查复位结束
		while(ADC_GetResetCalibrationStatus(ADC1));
			
		// 开始校准
		ADC_StartCalibration(ADC1);
		
		// 检查校准结束
		while(ADC_GetCalibrationStatus(ADC1));

		// 初始化后关闭AD1,手动转换是开启
    ADC_SoftwareStartConvCmd(ADC1, DISABLE);	
}
/*
*********************************************************************************************************
*                                           BSP_ADC_GetValue()
*
* Description : This function initializes the board's ADC
*
* Argument(s) : adc             ID of the ADC to probe.  For this board, the only legitimate value is 1.
*
* Return(s)   : The numerator of the binary fraction representing the result of the latest ADC conversion.
*               This value will be a 12-bit value between 0x0000 and 0x0FFF, inclusive.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
u16  BSP_ADC_GetValue (u8	channel)
{		     		
		u32	loop=0;

		ADC_SoftwareStartConvCmd(ADC1, DISABLE);	
		if(channel == DEF_ADC_CNN1)	
				return 0;
		else if(channel == DEF_ADC_CNN2)
				return 0;
		else if(channel == DEF_ADC_VBAT)
				ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_13Cycles5);
		else if(channel == DEF_ADC_LIBAT)		
				return 0;
		else
				return	0;
				
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		while((ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET) && (loop < 6000000))	// 超时大约1s
		{
				loop++;
		}
		if(loop >= 6000000)
				return 0;
		else
				return	ADC_GetConversionValue(ADC1);			
}

/*
*********************************************************************************************************
* Function Name  : BSP_ADC_CON
* Description    : AD转化使能或禁止				      
* Input          : DEF_ENABLE or DEF_DISABLE
* Output         : None
* Return         : 
*********************************************************************************************************
*/
void	BSP_ADC_CON (u8	newSta)
{
		if(newSta == DEF_ENABLE)
		{			
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
				ADC_Cmd(ADC1, ENABLE);
		    ADC_SoftwareStartConvCmd(ADC1, ENABLE);	
		}
		else
		{		 		
				ADC_Cmd(ADC1, DISABLE);
		    ADC_SoftwareStartConvCmd(ADC1, DISABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
		}
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                              USART FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/
/*
*********************************************************************************************************
*                                      BSP_USART_Init()
*
* Description : Initialize the UART for Board	communication.
*
* Argument(s) : baud_rate   Intended baud rate of the RS-232.
*
* Return(s)   : none.
*
* Note(s )    : (1) The following constants control the GPIO remap for the USART control lines:
*
*                        GPSRS232_UART_1_REMAP
*                        GPSRS232_UART_2_REMAP
*                        GPSRS232_UART_3_REMAP_PARTIAL
*                        GPSRS232_UART_3_REMAP_FULL
*
*                    Though the #error directives in "Local Configuration Errors" will require that
*                    all are defined, the value of those bearing on the USART not used will have no
*                    effect.
*
*                (2) PROBE_RS232_UART_3_REMAP_PARTIAL has precedence over PROBE_RS232_UART_3_REMAP_FULL,
*                    if both are defined to DEF_TRUE.
*********************************************************************************************************
*/
void	BSP_USART_Init(u8	USART_ID,	u8	REMAP_DEF,	u32 baud_rate)
{
	 	GPIO_InitTypeDef        gpio_init;
    USART_InitTypeDef       usart_init;
    USART_ClockInitTypeDef  usart_clk_init;

                                                                /* ----------------- INIT USART STRUCT ---------------- */
    usart_init.USART_BaudRate            = baud_rate;
    usart_init.USART_WordLength          = USART_WordLength_8b;
    usart_init.USART_StopBits            = USART_StopBits_1;
    usart_init.USART_Parity              = USART_Parity_No ;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    usart_clk_init.USART_Clock           = USART_Clock_Disable;
    usart_clk_init.USART_CPOL            = USART_CPOL_Low;
    usart_clk_init.USART_CPHA            = USART_CPHA_2Edge;
    usart_clk_init.USART_LastBit         = USART_LastBit_Disable;

//-------------------------------------------USART1
		if (USART_ID == DEF_USART_1)
		{
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		
		                                                                /* ----------------- SETUP USART1 GPIO ---------------- */
				if (REMAP_DEF == DEF_USART_1_REMAP)
				{
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
								
				    GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
						                                                            /* Configure GPIOB.6 as push-pull                       */
				    gpio_init.GPIO_Pin   = GPIO_Pin_6;
				    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
				    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
				    GPIO_Init(GPIOB, &gpio_init);
				
				                                                                /* Configure GPIOB.7 as input floating                  */
				    gpio_init.GPIO_Pin   = GPIO_Pin_7;
				    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
				    GPIO_Init(GPIOB, &gpio_init);
				}
				else
				{
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
				
				                                                                /* Configure GPIOA.9 as push-pull                       */
				    gpio_init.GPIO_Pin   = GPIO_Pin_9;
				    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
				    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
				    GPIO_Init(GPIOA, &gpio_init);
				
				                                                                /* Configure GPIOA.10 as input floating                 */
				    gpio_init.GPIO_Pin   = GPIO_Pin_10;
				    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
				    GPIO_Init(GPIOA, &gpio_init);		
				}
		                                                             /* ------------------ SETUP USART1 -------------------- */
		    USART_Init(USART1, &usart_init);
		    USART_ClockInit(USART1, &usart_clk_init);
		    
				// Clean interrupt flag, and disable txd & rxd interrupt
				USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
				USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	
				USART_ClearITPendingBit(USART1, USART_IT_RXNE);
				USART_ClearITPendingBit(USART1, USART_IT_TXE);
				
				USART_Cmd(USART1, ENABLE);			
	
		}

//-------------------------------------------USART2
		else if (USART_ID == DEF_USART_2)
		{
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
		                                                                /* ----------------- SETUP USART2 GPIO ---------------- */
				if (REMAP_DEF ==	DEF_USART_2_REMAP)
				{
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
				    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
				
				                                                                /* Configure GPIOD.5 as push-pull                       */
				    gpio_init.GPIO_Pin   = GPIO_Pin_5;
				    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
				    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
				    GPIO_Init(GPIOD, &gpio_init);
				
				                                                                /* Configure GPIOD.6 as input floating                  */
				    gpio_init.GPIO_Pin   = GPIO_Pin_6;
				    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
				    GPIO_Init(GPIOD, &gpio_init);
				}
				else
				{
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
				
				                                                                /* Configure GPIOA.2 as push-pull                       */
				    gpio_init.GPIO_Pin   = GPIO_Pin_2;
				    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
				    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
				    GPIO_Init(GPIOA, &gpio_init);
				
				                                                                /* Configure GPIOA.3 as input floating                  */
				    gpio_init.GPIO_Pin   = GPIO_Pin_3;
				    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
				    GPIO_Init(GPIOA, &gpio_init);
				}
					
				                                                                /* ------------------ SETUP USART2 -------------------- */
		    USART_Init(USART2, &usart_init);
		    USART_ClockInit(USART2, &usart_clk_init);
	
				// Clean interrupt flag, and disable txd & rxd interrupt
				USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
				USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	
				USART_ClearITPendingBit(USART2, USART_IT_RXNE);
				USART_ClearITPendingBit(USART2, USART_IT_TXE);
	
		    USART_Cmd(USART2, ENABLE);
		}
//-------------------------------------------USART3
		else if (USART_ID == DEF_USART_3)
		{
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		
		                                                                /* ----------------- SETUP USART3 GPIO ---------------- */
				if (REMAP_DEF ==	DEF_USART_3_REMAP_PARTIAL)
				{
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
				    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
				
				                                                                /* Configure GPIOC.10 as push-pull                      */
				    gpio_init.GPIO_Pin   = GPIO_Pin_10;
				    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
				    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
				    GPIO_Init(GPIOC, &gpio_init);
				
				                                                                /* Configure GPIOC.11 as input floating                 */
				    gpio_init.GPIO_Pin   = GPIO_Pin_11;
				    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
				    GPIO_Init(GPIOC, &gpio_init);
				}
				
				else if (REMAP_DEF ==	DEF_USART_3_REMAP_FULL)
				{
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
				    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
				
				                                                                /* Configure GPIOD.8 as push-pull                       */
				    gpio_init.GPIO_Pin   = GPIO_Pin_8;
				    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
				    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
				    GPIO_Init(GPIOD, &gpio_init);
				
				                                                                /* Configure GPIOD.9 as input floating                  */
				    gpio_init.GPIO_Pin   = GPIO_Pin_9;
				    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
				    GPIO_Init(GPIOD, &gpio_init);
				}
				else
				{
						RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
				                                                                /* Configure GPIOB.10 as push-pull                      */
				    gpio_init.GPIO_Pin   = GPIO_Pin_10;
				    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
				    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
				    GPIO_Init(GPIOB, &gpio_init);
				
				                                                                /* Configure GPIOB.11 as input floating                 */
				    gpio_init.GPIO_Pin   = GPIO_Pin_11;
				    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
				    GPIO_Init(GPIOB, &gpio_init);
				}
		
		                                                                /* ------------------ SETUP USART3 -------------------- */
		    USART_Init(USART3, &usart_init);
		    USART_ClockInit(USART3, &usart_clk_init);
				// Clean interrupt flag, and disable txd & rxd interrupt
				USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
				USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	
				USART_ClearITPendingBit(USART3, USART_IT_RXNE);
				USART_ClearITPendingBit(USART3, USART_IT_TXE);
	
		    USART_Cmd(USART3, ENABLE);
		}

//-------------------------------------------UART4
		else if (USART_ID == DEF_UART_4)
		{
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		
		                                                                /* ----------------- SETUP USART4 GPIO ---------------- */
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		
		                                                                /* Configure GPIOC.10 as push-pull                       */
				gpio_init.GPIO_Pin   = GPIO_Pin_10;
		    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
		    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
		    GPIO_Init(GPIOC, &gpio_init);
		
		                                                                /* Configure GPIOC.11 as input floating                 */
		    gpio_init.GPIO_Pin   = GPIO_Pin_11;
		    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
		    GPIO_Init(GPIOC, &gpio_init);	
		
		                                                                /* ------------------ SETUP UART4 -------------------- */
		    USART_Init(UART4, &usart_init);
		    USART_ClockInit(UART4, &usart_clk_init);
	
				// Clean interrupt flag, and disable txd & rxd interrupt
				USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
				USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
	
				USART_ClearITPendingBit(UART4, USART_IT_RXNE);
				USART_ClearITPendingBit(UART4, USART_IT_TXE);
	
		    USART_Cmd(UART4, ENABLE);
	
		}
//-------------------------------------------UART5
		else if (USART_ID == DEF_UART_5)
		{
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		
		                                                                /* ----------------- SETUP UART5 GPIO ---------------- */
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		                                                                /* Configure GPIOC.12 as push-pull                      */
		    gpio_init.GPIO_Pin   = GPIO_Pin_12;
		    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
		    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
		    GPIO_Init(GPIOC, &gpio_init);
		                                                                /* Configure GPIOD.2 as input floating                 */
		    gpio_init.GPIO_Pin   = GPIO_Pin_2;
		    gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
		    GPIO_Init(GPIOD, &gpio_init);
		
		                                                                /* ------------------ SETUP USART5 -------------------- */
		    USART_Init(UART5, &usart_init);
		    USART_ClockInit(UART5, &usart_clk_init);
	
				// Clean interrupt flag, and disable txd & rxd interrupt
				USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);
				USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
	
				USART_ClearITPendingBit(UART5, USART_IT_RXNE);
				USART_ClearITPendingBit(UART5, USART_IT_TXE);
	
		    USART_Cmd(UART5, ENABLE);
		}
}
/*
*********************************************************************************************************
*                                      BSP_RS232_RxIntDis()
*
* Description: Disable Rx interrupts.
*
* Argument(s): none.
*
* Return(s)  : none.
*********************************************************************************************************
*/
void  BSP_RS232_RxIntDis (u8	USART_ID)
{
		if(USART_ID == DEF_USART_1)
		{   USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);}
		else if(USART_ID == DEF_USART_2) 
		{		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);}
		else if(USART_ID == DEF_USART_3)
		{   USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);}
		else if(USART_ID == DEF_UART_4)
		{   USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);}
		else if(USART_ID == DEF_UART_5) 
		{   USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);}
}
/*
*********************************************************************************************************
*                                      BSP_RS232_RxIntEn()
*
* Description: Enable Rx interrupts.
*
* Argument(s): none.
*
* Return(s)  : none.
*********************************************************************************************************
*/

void  BSP_RS232_RxIntEn (u8	USART_ID)
{
		if(USART_ID == DEF_USART_1)
		{   USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);}
		else if(USART_ID == DEF_USART_2) 
		{		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);}
		else if(USART_ID == DEF_USART_3)
		{   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);}
		else if(USART_ID == DEF_UART_4)
		{   USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);}
		else if(USART_ID == DEF_UART_5) 
		{   USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);}
}
/*
*********************************************************************************************************
*                                      BSP_RS232_TxIntDis()
*
* Description: Disable Tx interrupts.
*
* Argument(s): none.
*
* Return(s)  : none.
*********************************************************************************************************
*/

void  BSP_RS232_TxIntDis (u8	USART_ID)
{
		if(USART_ID == DEF_USART_1)
		{   USART_ITConfig(USART1, USART_IT_TXE, DISABLE);}
		else if(USART_ID == DEF_USART_2) 
		{		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);}
		else if(USART_ID == DEF_USART_3)
		{   USART_ITConfig(USART3, USART_IT_TXE, DISABLE);}
		else if(USART_ID == DEF_UART_4)
		{   USART_ITConfig(UART4, USART_IT_TXE, DISABLE);}
		else if(USART_ID == DEF_UART_5) 
		{   USART_ITConfig(UART5, USART_IT_TXE, DISABLE);}
}
/*
*********************************************************************************************************
*                                      BSP_RS2322_TxIntEn()
*
* Description: Enable Tx interrupts.
*
* Argument(s): none.
*
* Return(s)  : none.
*********************************************************************************************************
*/
void  BSP_RS232_TxIntEn (u8	USART_ID)
{
		if(USART_ID == DEF_USART_1)
		{   USART_ITConfig(USART1, USART_IT_TXE, ENABLE);}
		else if(USART_ID == DEF_USART_2) 
		{		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);}
		else if(USART_ID == DEF_USART_3)
		{   USART_ITConfig(USART3, USART_IT_TXE, ENABLE);}
		else if(USART_ID == DEF_UART_4)
		{   USART_ITConfig(UART4, USART_IT_TXE, ENABLE);}
		else if(USART_ID == DEF_UART_5) 
		{   USART_ITConfig(UART5, USART_IT_TXE, ENABLE);}
}
/*
*********************************************************************************************************
*                                  BSP_PutChar()
*
* Description: 
*
* Argument(s): none.
*
* Return(s)  : none.
*********************************************************************************************************
*/
void  BSP_PutChar (u8	USART_ID,	u8 *buf,	u32	len)
{
		USART_TypeDef  *usart;
		u32	i=0;
	
		if  (USART_ID == DEF_USART_1)
		    usart = USART1;
		else if (USART_ID == DEF_USART_2)
		    usart = USART2;
		else if (USART_ID == DEF_USART_3)
		    usart = USART3;
		else if	(USART_ID == DEF_UART_4)
				usart = UART4;
		else if	(USART_ID == DEF_UART_5)
				usart = UART5;
		for(i=0;	i< len;	i++)
		{
				USART_SendData(usart, *(buf+i));
				while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
		}
}
/*
*********************************************************************************************************
*                                  BSP_PutString()
*
* Description: 
*
* Argument(s): none.
*
* Return(s)  : none.
*********************************************************************************************************
*/
void  BSP_PutString (u8	USART_ID,	u8 *buf)
{
		USART_TypeDef  *usart;
		u32	i;
	
		if  (USART_ID == DEF_USART_1)
		    usart = USART1;
		else if (USART_ID == DEF_USART_2)
		    usart = USART2;
		else if (USART_ID == DEF_USART_3)
		    usart = USART3;
		else if	(USART_ID == DEF_UART_4)
				usart = UART4;
		else if	(USART_ID == DEF_UART_5)
				usart = UART5;
	
		for(i=0;	*(buf+i) != '\0';	i++)
		{
				USART_SendData(usart, *(buf+i));
				while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
		}
}
/*
*********************************************************************************************************
*                                  BSP_OSPutChar
*
* Description: 重新封装PutChar函数，带访问互斥判断，如果资源占用则调用系统级等待
*
* Argument(s): none.
*
* Return(s)  : 0: 成功； -1: 失败
*********************************************************************************************************
*/
s8	BSP_OSPutChar (u8	USART_ID,	u8 *buf,	u32	len)
{	
		INT8U	err =OS_NO_ERR;
		USART_TypeDef  *usart;
		u32	i;
	
		if(OSRunning > 0)
				OSSemPend(DBGMutexSem,	3000,	&err);		//	最大等待3s
		else
				if(DBGMutexFlag	== 1)
						DBGMutexFlag	=	0;
				else
						return	-1;			

		if(err != OS_NO_ERR)
				return	-1;
		else
		{
				if  (USART_ID == DEF_USART_1)
						usart = USART1;
				else if (USART_ID == DEF_USART_2)
						usart = USART2;
				else if (USART_ID == DEF_USART_3)
						usart = USART3;
				else if	(USART_ID == DEF_UART_4)
						usart = UART4;
				else if	(USART_ID == DEF_UART_5)
						usart = UART5;
				for(i=0;	i< len;	i++)
				{
						USART_SendData(usart, *(buf+i));
						while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
				}
		}
		if(OSRunning > 0)
				OSSemPost(DBGMutexSem);									//	释放信号量
		else
				DBGMutexFlag	=	1;
		
		return 0;
}
/*
*********************************************************************************************************
*                                  PUTCHAR_PROTOTYPE
*
* Description: 
*
* Argument(s): none.
*
* Return(s)  : none.
*********************************************************************************************************
*/
#if(USE_MDK_WINDOW_OUTPUT > 0)
int fputc(int ch, FILE *f) 	// Printf输出到MDK Printf窗口时使用
{
		if (DEMCR & TRCENA) 
		{
				while (ITM_Port32(0) == 0);
				ITM_Port8(0) = ch;
		}
		return(ch);
}
#else
PUTCHAR_PROTOTYPE						// Printf输出到硬件串口时使用
{
    USART_TypeDef  *usart;

		if  		(DEF_DBG_UART == DEF_USART_1)
	    	usart = USART1;
		else if (DEF_DBG_UART == DEF_USART_2)
		    usart = USART2;
		else if (DEF_DBG_UART == DEF_USART_3)
		    usart = USART3;
		else if	(DEF_DBG_UART == DEF_UART_4)
				usart = UART4;
		else if	(DEF_DBG_UART == DEF_UART_5)
				usart = UART5;				
		
		/* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    USART_SendData(usart, (u8) ch);
    
    /* Loop until the end of transmission */
    while(USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET)
    {
    }
    return ch;
}
#endif
/*
*********************************************************************************************************
*                                  myPrintf
*
* Description: 重新封装Printf函数，带访问互斥判断，如果资源占用则调用系统级等待
*
* Argument(s): none.
*
* Return(s)  : 0: 成功； -1: 失败
*********************************************************************************************************
*/
s8	myPrintf (const char * format, ...)
{	
		INT8U	err =OS_NO_ERR;
		int n = 0;
		va_list arg;
	
		if(OSRunning > 0)
				OSSemPend(DBGMutexSem,	3000,	&err);		//	最大等待3s
		else
				if(DBGMutexFlag	== 1)
						DBGMutexFlag	=	0;
				else
						return	-1;			

		if(err != OS_NO_ERR)
				return	-1;
		else
		{
				va_start(arg,format);
				n = vprintf(format,arg);
				va_end(arg);
		}
		if(OSRunning > 0)
				OSSemPost(DBGMutexSem);									//	释放信号量
		else
				DBGMutexFlag	=	1;
		
		return 0;
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                              SPI FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_SPI_Init()
*
* Description : Initialize the board's SPI Bus
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_SPI_Init (u8	SPI_ID)
{
		SPI_InitTypeDef  SPI_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;

		/* SPI configuration */ 
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	  //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;	// Baud = 72/Presecaler (Presecaler min =2)
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	  SPI_InitStructure.SPI_CRCPolynomial = 7;

		//-------------------------------------------SPI1
		if (SPI_ID == DEF_SPI_1)
		{
				/* Enable SPI1 and GPIOA clocks */
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOA, ENABLE);

				/* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
//			  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
//			  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

//				GPIO_Init(GPIOA, &GPIO_InitStructure);
			
				// 2014-12-12 为GD32所做修改(GD32要求输入脚MISO必须配置成IN_FLOATING模式,STM32则也可以配置成AF_PP)
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
			  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
				GPIO_Init(GPIOA, &GPIO_InitStructure);
			
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
				GPIO_Init(GPIOA, &GPIO_InitStructure);
				
				SPI_Init(SPI1, &SPI_InitStructure);
	  
			  /* Enable SPI1  */
			  SPI_Cmd(SPI1, ENABLE); 
		}
		else if	(SPI_ID == DEF_SPI_2)
		{
				/* Enable SPI2 and GPIOB clocks */
			  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,	ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
			
			  /* Configure SPI2 pins: NSS, SCK, MISO and MOSI */
//			  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//			  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;

//			  GPIO_Init(GPIOB, &GPIO_InitStructure);
			
				// 2014-12-12 为GD32所做修改(GD32要求输入脚MISO必须配置成IN_FLOATING模式,STM32则也可以配置成AF_PP)
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
			  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
				GPIO_Init(GPIOB, &GPIO_InitStructure);
			
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
				GPIO_Init(GPIOB, &GPIO_InitStructure);				
		
				SPI_Init(SPI2, &SPI_InitStructure);
	  
			  /* Enable SPI2  */
			  SPI_Cmd(SPI2, ENABLE); 
		}	  
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                              IIC FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_IIC_Init()
*
* Description : Initialize the board's IIC Bus(I/O 模拟方式,I/O 只要不初始化模拟输入方式就可以读取状态)
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_IIC_Init (u8	IIC_ID)
{
		GPIO_InitTypeDef  gpio_init;
		
		/* I2C 软件模拟方式 I/O 初始化 */			  																						
		if(IIC_ID == DEF_IIC_1)
		{
				// SCL: Pin_6, SDA: Pin_7
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		    gpio_init.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
		    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
		    gpio_init.GPIO_Mode  = GPIO_Mode_Out_OD;
				GPIO_Init(GPIOB, &gpio_init);	
			
				// SCL=1, SDA=1
				GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);
		}
		else if(IIC_ID == DEF_IIC_2)
		{
				// SCL: Pin_10, SDA: Pin_11
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		    gpio_init.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
		    gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
		    gpio_init.GPIO_Mode  = GPIO_Mode_Out_OD;
				GPIO_Init(GPIOB, &gpio_init);		
			
				// SCL=1, SDA=1
				GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
		}	
}

/*
*********************************************************************************************************
*********************************************************************************************************
*                                              CAN FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             BSP_CAN_Init()
*
* Description : Initialize the board's SPI Bus
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : BSP_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/
void	BSP_CAN_Init (u8	CAN_ID)
{
		GPIO_InitTypeDef GPIO_InitStructure;			
		
		//-------------------------------------------CAN1
		if (CAN_ID == DEF_CAN_1)
		{
				GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1 , ENABLE);

				// Configure CAN pin: RX 
			  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			  GPIO_Init(GPIOB, &GPIO_InitStructure);
			  
			  // Configure CAN pin: TX 
			  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			  GPIO_Init(GPIOB, &GPIO_InitStructure);

		}
		
		else	if (CAN_ID == DEF_CAN_2)
		{
				GPIO_PinRemapConfig(GPIO_Remap_CAN2, ENABLE);
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2 , ENABLE);

				// Configure CAN pin: RX 
			  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			  GPIO_Init(GPIOB, &GPIO_InitStructure);
			  
			  // Configure CAN pin: TX 
			  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
			  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
			  GPIO_Init(GPIOB, &GPIO_InitStructure);			
		}			
}
/*
*********************************************************************************************************
*********************************************************************************************************
*                                            MEMORY FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*********************************************************************************************************
*                                           OTHER FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

