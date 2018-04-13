/*
********************************************************************************
File name  : ZXT_OBD0.c
Description: 该文件主要是某一个系统的发动机配置文件
History    :
修改日期   : 修改者 : 修改内容:简单详述
2012-4-19    小老鼠   建立XX函数
修改日期   : 修改者 : 修改内容:简单详述
********************************************************************************
*/

#define  ZXT_OBD_STD_VAR
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
                              描述:整个系统命令索引值
格式：总数+命令索引值<=255
********************************************************************************
*/

const uint8 u8Obd0Enter0Id0[] = {1, 0};     //进入标准K(高低电平激活)
const uint8 u8Obd0Enter0Id8[] = {1, 6};     //进入标准K(地址码激活)
const uint8  u8Obd0Rdtc0Id0[] = {1, 1};     //读码
const uint8  u8Obd0Cdtc0Id0[] = {1, 2};     //清码
const uint8  u8Obd0Exit0Id0[] = {1, 7};     //退出
const uint8  u8Obd0Vin0Id0[] =  {1, 63}; //VIN获取

const uint8 u8Obd0Enter0Id1[] = {1, 0};     //进入68 6A
const uint8  u8Obd0Rdtc0Id1[] = {1, 1};     //读码
const uint8  u8Obd0Cdtc0Id1[] = {1, 2};     //清码
const uint8  u8Obd0Exit0Id1[] = {1, 0};     //退出

const uint8 u8Obd0Enter0Id2[] = {1, 0};    //进入标准can(7e8)
const uint8  u8Obd0Rdtc0Id2[] = {1, 1};    //读码
const uint8  u8Obd0Cdtc0Id2[] = {1, 2};    //清码
const uint8  u8Obd0Vin0Id2[] =  {1, 61}; //VIN获取

const uint8 u8Obd0Enter0Id3[] = {1, 0};    //进入标准can(7e9)
const uint8  u8Obd0Rdtc0Id3[] = {1, 1};    //读码
const uint8  u8Obd0Cdtc0Id3[] = {1, 2};    //清码
const uint8  u8Obd0Vin0Id3[] =  {1, 61}; //VIN获取

const uint8 u8Obd0Enter0Id4[] = {1, 0};    //进入标准can(7ef)
const uint8  u8Obd0Rdtc0Id4[] = {1, 1};    //读码
const uint8  u8Obd0Cdtc0Id4[] = {1, 2};    //清码
const uint8  u8Obd0Vin0Id4[] =  {1, 61}; //VIN获取

const uint8 u8Obd0Enter0Id5[] = {1, 0};    //进入扩展can(18daf110)
const uint8  u8Obd0Rdtc0Id5[] = {1, 1};    //读码
const uint8  u8Obd0Cdtc0Id5[] = {1, 2};    //清码
const uint8  u8Obd0Vin0Id5[] =  {1, 61}; //VIN获取

const uint8 u8Obd0Enter0Id6[] = {1, 0};    //进入扩展can(18daf111)
const uint8  u8Obd0Rdtc0Id6[] = {1, 1};    //读码
const uint8  u8Obd0Cdtc0Id6[] = {1, 2};    //清码
const uint8  u8Obd0Vin0Id6[] =  {1, 61}; //VIN获取

const uint8 u8Obd0Enter0Id7[] = {1, 0};    //进入扩展can(18daf10e)
const uint8  u8Obd0Rdtc0Id7[] = {1, 1};    //读码
const uint8  u8Obd0Cdtc0Id7[] = {1, 2};    //清码
const uint8  u8Obd0Vin0Id7[] =  {1, 61}; //VIN获取
/*
********************************************************************************
                             描述:整个发动机系统命令
********************************************************************************
*/
const uint8 u8Obd0Mode0Cmd00[] = {0xc1, 0x00, 0x00, 0x81, 0x00};//进入命令(高低电平激活)
const uint8 u8Obd0Mode0Cmd01[] = {0xc1, 0x00, 0x00, 0x03, 0xe8};//读码命令
const uint8 u8Obd0Mode0Cmd02[] = {0xc1, 0x00, 0x00, 0x04, 0xe9};//清码命令
const uint8 u8Obd0Mode0Cmd03[] = {0xc2, 0x00, 0x00, 0x01, 0x42, 0x9D};//读取数据流命令
const uint8 u8Obd0Mode0Cmd04[] = {0xc2, 0x00, 0x00, 0x01, 0x05, 0x9D};//读取数据流命令
const uint8 u8Obd0Mode0Cmd05[] = {0xc2, 0x00, 0x00, 0x01, 0x0C, 0x9D};//读取数据流命令
const uint8 u8Obd0Mode0Cmd06[] = {0xC2, 0x00, 0x00, 0x01, 0x00, 0xe7};//进入命令(地址码激活)
const uint8 u8Obd0Mode0Cmd07[] = {0xc1, 0x00, 0x00, 0x82, 0x00};//退出命令(高低电平激活)

//后续增加数据流
const uint8 u8Obd0Mode0Cmd08[] = {0xc2, 0x00, 0x00, 0x01, 0x21, 0x9D}; //读取数据流命令
const uint8 u8Obd0Mode0Cmd09[] = {0xc2, 0x00, 0x00, 0x01, 0x31, 0x9D}; //读取数据流命令
//为专家坐席添加数据流

const uint8 u8Obd0Mode0Cmd010[] = {0xc2, 0x00, 0x00, 0x01, 0x01, 0x9D};
const uint8 u8Obd0Mode0Cmd011[] = {0xc2, 0x00, 0x00, 0x01, 0x02, 0x9D};
const uint8 u8Obd0Mode0Cmd012[] = {0xc2, 0x00, 0x00, 0x01, 0x04, 0x9D};
const uint8 u8Obd0Mode0Cmd013[] = {0xc2, 0x00, 0x00, 0x01, 0x06, 0x9D};
const uint8 u8Obd0Mode0Cmd014[] = {0xc2, 0x00, 0x00, 0x01, 0x07, 0x9D};
const uint8 u8Obd0Mode0Cmd015[] = {0xc2, 0x00, 0x00, 0x01, 0x08, 0x9D};
const uint8 u8Obd0Mode0Cmd016[] = {0xc2, 0x00, 0x00, 0x01, 0x09, 0x9D};
const uint8 u8Obd0Mode0Cmd017[] = {0xc2, 0x00, 0x00, 0x01, 0x0a, 0x9D};
const uint8 u8Obd0Mode0Cmd018[] = {0xc2, 0x00, 0x00, 0x01, 0x0b, 0x9D};
const uint8 u8Obd0Mode0Cmd019[] = {0xc2, 0x00, 0x00, 0x01, 0x0d, 0x9D};
const uint8 u8Obd0Mode0Cmd020[] = {0xc2, 0x00, 0x00, 0x01, 0x0e, 0x9D};
const uint8 u8Obd0Mode0Cmd021[] = {0xc2, 0x00, 0x00, 0x01, 0x0f, 0x9D};
const uint8 u8Obd0Mode0Cmd022[] = {0xc2, 0x00, 0x00, 0x01, 0x10, 0x9D};
const uint8 u8Obd0Mode0Cmd023[] = {0xc2, 0x00, 0x00, 0x01, 0x11, 0x9D};
const uint8 u8Obd0Mode0Cmd024[] = {0xc2, 0x00, 0x00, 0x01, 0x14, 0x9D};
const uint8 u8Obd0Mode0Cmd025[] = {0xc2, 0x00, 0x00, 0x01, 0x15, 0x9D};
const uint8 u8Obd0Mode0Cmd026[] = {0xc2, 0x00, 0x00, 0x01, 0x16, 0x9D};
const uint8 u8Obd0Mode0Cmd027[] = {0xc2, 0x00, 0x00, 0x01, 0x17, 0x9D};
const uint8 u8Obd0Mode0Cmd028[] = {0xc2, 0x00, 0x00, 0x01, 0x18, 0x9D};
const uint8 u8Obd0Mode0Cmd029[] = {0xc2, 0x00, 0x00, 0x01, 0x19, 0x9D};
const uint8 u8Obd0Mode0Cmd030[] = {0xc2, 0x00, 0x00, 0x01, 0x1a, 0x9D};
const uint8 u8Obd0Mode0Cmd031[] = {0xc2, 0x00, 0x00, 0x01, 0x1b, 0x9D};
const uint8 u8Obd0Mode0Cmd032[] = {0xc2, 0x00, 0x00, 0x01, 0x1e, 0x9D};
const uint8 u8Obd0Mode0Cmd033[] = {0xc2, 0x00, 0x00, 0x01, 0x1f, 0x9D};
const uint8 u8Obd0Mode0Cmd034[] = {0xc2, 0x00, 0x00, 0x01, 0x22, 0x9D};
const uint8 u8Obd0Mode0Cmd035[] = {0xc2, 0x00, 0x00, 0x01, 0x23, 0x9D};
const uint8 u8Obd0Mode0Cmd036[] = {0xc2, 0x00, 0x00, 0x01, 0x24, 0x9D};
const uint8 u8Obd0Mode0Cmd037[] = {0xc2, 0x00, 0x00, 0x01, 0x25, 0x9D};
const uint8 u8Obd0Mode0Cmd038[] = {0xc2, 0x00, 0x00, 0x01, 0x26, 0x9D};
const uint8 u8Obd0Mode0Cmd039[] = {0xc2, 0x00, 0x00, 0x01, 0x27, 0x9D};
const uint8 u8Obd0Mode0Cmd040[] = {0xc2, 0x00, 0x00, 0x01, 0x28, 0x9D};
const uint8 u8Obd0Mode0Cmd041[] = {0xc2, 0x00, 0x00, 0x01, 0x29, 0x9D};
const uint8 u8Obd0Mode0Cmd042[] = {0xc2, 0x00, 0x00, 0x01, 0x2a, 0x9D};
const uint8 u8Obd0Mode0Cmd043[] = {0xc2, 0x00, 0x00, 0x01, 0x2b, 0x9D};
const uint8 u8Obd0Mode0Cmd044[] = {0xc2, 0x00, 0x00, 0x01, 0x2c, 0x9D};
const uint8 u8Obd0Mode0Cmd045[] = {0xc2, 0x00, 0x00, 0x01, 0x2d, 0x9D};
const uint8 u8Obd0Mode0Cmd046[] = {0xc2, 0x00, 0x00, 0x01, 0x2e, 0x9D};
const uint8 u8Obd0Mode0Cmd047[] = {0xc2, 0x00, 0x00, 0x01, 0x2f, 0x9D};
const uint8 u8Obd0Mode0Cmd048[] = {0xc2, 0x00, 0x00, 0x01, 0x30, 0x9D};
const uint8 u8Obd0Mode0Cmd049[] = {0xc2, 0x00, 0x00, 0x01, 0x32, 0x9D};
const uint8 u8Obd0Mode0Cmd050[] = {0xc2, 0x00, 0x00, 0x01, 0x33, 0x9D};
const uint8 u8Obd0Mode0Cmd051[] = {0xc2, 0x00, 0x00, 0x01, 0x34, 0x9D};
const uint8 u8Obd0Mode0Cmd052[] = {0xc2, 0x00, 0x00, 0x01, 0x35, 0x9D};
const uint8 u8Obd0Mode0Cmd053[] = {0xc2, 0x00, 0x00, 0x01, 0x36, 0x9D};
const uint8 u8Obd0Mode0Cmd054[] = {0xc2, 0x00, 0x00, 0x01, 0x37, 0x9D};
const uint8 u8Obd0Mode0Cmd055[] = {0xc2, 0x00, 0x00, 0x01, 0x38, 0x9D};
const uint8 u8Obd0Mode0Cmd056[] = {0xc2, 0x00, 0x00, 0x01, 0x39, 0x9D};
const uint8 u8Obd0Mode0Cmd057[] = {0xc2, 0x00, 0x00, 0x01, 0x3a, 0x9D};
const uint8 u8Obd0Mode0Cmd058[] = {0xc2, 0x00, 0x00, 0x01, 0x3b, 0x9D};
const uint8 u8Obd0Mode0Cmd059[] = {0xc2, 0x00, 0x00, 0x01, 0x3c, 0x9D};
const uint8 u8Obd0Mode0Cmd060[] = {0xc2, 0x00, 0x00, 0x01, 0x3d, 0x9D};
const uint8 u8Obd0Mode0Cmd061[] = {0xc2, 0x00, 0x00, 0x01, 0x3e, 0x9D};
const uint8 u8Obd0Mode0Cmd062[] = {0xc2, 0x00, 0x00, 0x01, 0x3f, 0x9D};

const uint8 u8Obd0Mode0Cmd063[] = {0xc2, 0x00, 0x00, 0x09, 0x02, 0x9D};//获取vin码

//68 6A F1 01 00 C4 进入
//68 6A F1 03 C6读码
//68 6A F1 04 C7清码
//68 6A F1 01 05 C9数据流
//68 6A F1 01 0C D0数据流
//68 6A F1 01 42 06数据流
const uint8 u8Obd0Mode1Cmd00[] = {0x68, 0x6A, 0xF1, 0x01, 0x00, 0xc4};//进入命令
const uint8 u8Obd0Mode1Cmd01[] = {0x68, 0x6A, 0xF1, 0x03, 0xC6};//读码命令
const uint8 u8Obd0Mode1Cmd02[] = {0x68, 0x6A, 0xF1, 0x04, 0xC7};//清码命令
const uint8 u8Obd0Mode1Cmd03[] = {0x68, 0x6A, 0xF1, 0x01, 0x42, 0xc9};//数据流命令
const uint8 u8Obd0Mode1Cmd04[] = {0x68, 0x6A, 0xF1, 0x01, 0x05, 0xd0};//数据流命令
const uint8 u8Obd0Mode1Cmd05[] = {0x68, 0x6A, 0xF1, 0x01, 0x0c, 0x06};//数据流命令


//后续增加数据流
const uint8 u8Obd0Mode1Cmd06[] = {0x68, 0x6a, 0xf1, 0x01, 0x21, 0x9D}; //读取数据流命令
const uint8 u8Obd0Mode1Cmd07[] = {0x68, 0x6a, 0xf1, 0x01, 0x31, 0x9D}; //读取数据流命令


//为专家坐席添加数据流
const uint8 u8Obd0Mode1Cmd008[] = {0x68, 0x6a, 0xf1, 0x01, 0x01, 0x9D}; //读取数据流命令
const uint8 u8Obd0Mode1Cmd009[] = {0x68, 0x6a, 0xf1, 0x01, 0x02, 0x9D}; //读取数据流命令
const uint8 u8Obd0Mode1Cmd010[] = {0x68, 0x6a, 0xf1, 0x01, 0x04, 0x9D};
const uint8 u8Obd0Mode1Cmd011[] = {0x68, 0x6a, 0xf1, 0x01, 0x06, 0x9D};
const uint8 u8Obd0Mode1Cmd012[] = {0x68, 0x6a, 0xf1, 0x01, 0x07, 0x9D};
const uint8 u8Obd0Mode1Cmd013[] = {0x68, 0x6a, 0xf1, 0x01, 0x08, 0x9D};
const uint8 u8Obd0Mode1Cmd014[] = {0x68, 0x6a, 0xf1, 0x01, 0x09, 0x9D};
const uint8 u8Obd0Mode1Cmd015[] = {0x68, 0x6a, 0xf1, 0x01, 0x0a, 0x9D};
const uint8 u8Obd0Mode1Cmd016[] = {0x68, 0x6a, 0xf1, 0x01, 0x0b, 0x9D};
const uint8 u8Obd0Mode1Cmd017[] = {0x68, 0x6a, 0xf1, 0x01, 0x0d, 0x9D};
const uint8 u8Obd0Mode1Cmd018[] = {0x68, 0x6a, 0xf1, 0x01, 0x0e, 0x9D};
const uint8 u8Obd0Mode1Cmd019[] = {0x68, 0x6a, 0xf1, 0x01, 0x0f, 0x9D};
const uint8 u8Obd0Mode1Cmd020[] = {0x68, 0x6a, 0xf1, 0x01, 0x10, 0x9D};
const uint8 u8Obd0Mode1Cmd021[] = {0x68, 0x6a, 0xf1, 0x01, 0x11, 0x9D};
const uint8 u8Obd0Mode1Cmd022[] = {0x68, 0x6a, 0xf1, 0x01, 0x14, 0x9D};
const uint8 u8Obd0Mode1Cmd023[] = {0x68, 0x6a, 0xf1, 0x01, 0x15, 0x9D};
const uint8 u8Obd0Mode1Cmd024[] = {0x68, 0x6a, 0xf1, 0x01, 0x16, 0x9D};
const uint8 u8Obd0Mode1Cmd025[] = {0x68, 0x6a, 0xf1, 0x01, 0x17, 0x9D};
const uint8 u8Obd0Mode1Cmd026[] = {0x68, 0x6a, 0xf1, 0x01, 0x18, 0x9D};
const uint8 u8Obd0Mode1Cmd027[] = {0x68, 0x6a, 0xf1, 0x01, 0x19, 0x9D};
const uint8 u8Obd0Mode1Cmd028[] = {0x68, 0x6a, 0xf1, 0x01, 0x1a, 0x9D};
const uint8 u8Obd0Mode1Cmd029[] = {0x68, 0x6a, 0xf1, 0x01, 0x1b, 0x9D};
const uint8 u8Obd0Mode1Cmd030[] = {0x68, 0x6a, 0xf1, 0x01, 0x1e, 0x9D};
const uint8 u8Obd0Mode1Cmd031[] = {0x68, 0x6a, 0xf1, 0x01, 0x1f, 0x9D};
const uint8 u8Obd0Mode1Cmd032[] = {0x68, 0x6a, 0xf1, 0x01, 0x22, 0x9D};
const uint8 u8Obd0Mode1Cmd033[] = {0x68, 0x6a, 0xf1, 0x01, 0x23, 0x9D};
const uint8 u8Obd0Mode1Cmd034[] = {0x68, 0x6a, 0xf1, 0x01, 0x24, 0x9D};
const uint8 u8Obd0Mode1Cmd035[] = {0x68, 0x6a, 0xf1, 0x01, 0x25, 0x9D};
const uint8 u8Obd0Mode1Cmd036[] = {0x68, 0x6a, 0xf1, 0x01, 0x26, 0x9D};
const uint8 u8Obd0Mode1Cmd037[] = {0x68, 0x6a, 0xf1, 0x01, 0x27, 0x9D};
const uint8 u8Obd0Mode1Cmd038[] = {0x68, 0x6a, 0xf1, 0x01, 0x28, 0x9D};
const uint8 u8Obd0Mode1Cmd039[] = {0x68, 0x6a, 0xf1, 0x01, 0x29, 0x9D};
const uint8 u8Obd0Mode1Cmd040[] = {0x68, 0x6a, 0xf1, 0x01, 0x2a, 0x9D};
const uint8 u8Obd0Mode1Cmd041[] = {0x68, 0x6a, 0xf1, 0x01, 0x2b, 0x9D};
const uint8 u8Obd0Mode1Cmd042[] = {0x68, 0x6a, 0xf1, 0x01, 0x2c, 0x9D};
const uint8 u8Obd0Mode1Cmd043[] = {0x68, 0x6a, 0xf1, 0x01, 0x2d, 0x9D};
const uint8 u8Obd0Mode1Cmd044[] = {0x68, 0x6a, 0xf1, 0x01, 0x2e, 0x9D};
const uint8 u8Obd0Mode1Cmd045[] = {0x68, 0x6a, 0xf1, 0x01, 0x2f, 0x9D};
const uint8 u8Obd0Mode1Cmd046[] = {0x68, 0x6a, 0xf1, 0x01, 0x30, 0x9D};
const uint8 u8Obd0Mode1Cmd047[] = {0x68, 0x6a, 0xf1, 0x01, 0x32, 0x9D};
const uint8 u8Obd0Mode1Cmd048[] = {0x68, 0x6a, 0xf1, 0x01, 0x33, 0x9D};
const uint8 u8Obd0Mode1Cmd049[] = {0x68, 0x6a, 0xf1, 0x01, 0x34, 0x9D};
const uint8 u8Obd0Mode1Cmd050[] = {0x68, 0x6a, 0xf1, 0x01, 0x35, 0x9D};
const uint8 u8Obd0Mode1Cmd051[] = {0x68, 0x6a, 0xf1, 0x01, 0x36, 0x9D};
const uint8 u8Obd0Mode1Cmd052[] = {0x68, 0x6a, 0xf1, 0x01, 0x37, 0x9D};
const uint8 u8Obd0Mode1Cmd053[] = {0x68, 0x6a, 0xf1, 0x01, 0x38, 0x9D};
const uint8 u8Obd0Mode1Cmd054[] = {0x68, 0x6a, 0xf1, 0x01, 0x39, 0x9D};
const uint8 u8Obd0Mode1Cmd055[] = {0x68, 0x6a, 0xf1, 0x01, 0x3a, 0x9D};
const uint8 u8Obd0Mode1Cmd056[] = {0x68, 0x6a, 0xf1, 0x01, 0x3b, 0x9D};
const uint8 u8Obd0Mode1Cmd057[] = {0x68, 0x6a, 0xf1, 0x01, 0x3c, 0x9D};
const uint8 u8Obd0Mode1Cmd058[] = {0x68, 0x6a, 0xf1, 0x01, 0x3d, 0x9D};
const uint8 u8Obd0Mode1Cmd059[] = {0x68, 0x6a, 0xf1, 0x01, 0x3e, 0x9D};
const uint8 u8Obd0Mode1Cmd060[] = {0x68, 0x6a, 0xf1, 0x01, 0x3f, 0x9D};


//7DF  02 01 00 00 00 00 00 00进入
//7DF  01 03 00 00 00 00 00 00读码
//7DF  01 04 00 00 00 00 00 00清码
//7DF  02 01 42 00 00 00 00 00数据流
//7DF  02 01 05 00 00 00 00 00数据流
//7DF  02 01 0c 00 00 00 00 00数据流
const uint8 u8Obd0Mode2Cmd00[] = {0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//进入命令
const uint8 u8Obd0Mode2Cmd01[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//读码命令
const uint8 u8Obd0Mode2Cmd02[] = {0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//清码命令
const uint8 u8Obd0Mode2Cmd03[] = {0x02, 0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00};//数据流命令
const uint8 u8Obd0Mode2Cmd04[] = {0x02, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00};//数据流命令
const uint8 u8Obd0Mode2Cmd05[] = {0x02, 0x01, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00};//数据流命令

//后续增加数据流
const uint8 u8Obd0Mode2Cmd06[] = {0x02, 0x01, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00}; //读取数据流命令
const uint8 u8Obd0Mode2Cmd07[] = {0x02, 0x01, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00}; //读取数据流命令

//为专家坐席添加数据流
const uint8 u8Obd0Mode2Cmd008[] = {0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd009[] = {0x02, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd010[] = {0x02, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd011[] = {0x02, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd012[] = {0x02, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd013[] = {0x02, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd014[] = {0x02, 0x01, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd015[] = {0x02, 0x01, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd016[] = {0x02, 0x01, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd017[] = {0x02, 0x01, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd018[] = {0x02, 0x01, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd019[] = {0x02, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd020[] = {0x02, 0x01, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd021[] = {0x02, 0x01, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd022[] = {0x02, 0x01, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd023[] = {0x02, 0x01, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd024[] = {0x02, 0x01, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd025[] = {0x02, 0x01, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd026[] = {0x02, 0x01, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd027[] = {0x02, 0x01, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd028[] = {0x02, 0x01, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd029[] = {0x02, 0x01, 0x1b, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd030[] = {0x02, 0x01, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd031[] = {0x02, 0x01, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd032[] = {0x02, 0x01, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd033[] = {0x02, 0x01, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd034[] = {0x02, 0x01, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd035[] = {0x02, 0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd036[] = {0x02, 0x01, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd037[] = {0x02, 0x01, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd038[] = {0x02, 0x01, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd039[] = {0x02, 0x01, 0x29, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd040[] = {0x02, 0x01, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd041[] = {0x02, 0x01, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd042[] = {0x02, 0x01, 0x2c, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd043[] = {0x02, 0x01, 0x2d, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd044[] = {0x02, 0x01, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd045[] = {0x02, 0x01, 0x2f, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd046[] = {0x02, 0x01, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd047[] = {0x02, 0x01, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd048[] = {0x02, 0x01, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd049[] = {0x02, 0x01, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd050[] = {0x02, 0x01, 0x35, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd051[] = {0x02, 0x01, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd052[] = {0x02, 0x01, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd053[] = {0x02, 0x01, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd054[] = {0x02, 0x01, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd055[] = {0x02, 0x01, 0x3a, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd056[] = {0x02, 0x01, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd057[] = {0x02, 0x01, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd058[] = {0x02, 0x01, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd059[] = {0x02, 0x01, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8 u8Obd0Mode2Cmd060[] = {0x02, 0x01, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8 u8Obd0Mode2Cmd061[] = {0x02, 0x09, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00};//获取车辆vin


/*
********************************************************************************
                              描述:命令索引值-标准K线(高低电平激活)
********************************************************************************
*/

const TYPEDEF_CMD_INDEX g_stCmdObd0Mode0Index =
{
    ( uint8 * ) u8Obd0Enter0Id0,//进入
    NULL,//空闲
    ( uint8 * ) u8Obd0Rdtc0Id0,//读码
    ( uint8 * ) u8Obd0Cdtc0Id0,//清码
    NULL,
    ( uint8 * )u8Obd0Exit0Id0,//退出
    ( uint8 * )u8Obd0Vin0Id0,//获取VIN码
};
/*
********************************************************************************
                              描述:命令索引值-标准K线(地址码激活)
********************************************************************************
*/

const TYPEDEF_CMD_INDEX g_stCmdObd0Mode8Index =
{
    ( uint8 * ) u8Obd0Enter0Id8,//进入
    NULL,//空闲
    ( uint8 * ) u8Obd0Rdtc0Id0,//读码
    ( uint8 * ) u8Obd0Cdtc0Id0,//清码
    NULL,
    ( uint8 * )u8Obd0Exit0Id0, //退出
    ( uint8 * )u8Obd0Vin0Id0,  //获取VIN码
};
/*
********************************************************************************
                              描述:命令索引值-68 6A
********************************************************************************
*/
const TYPEDEF_CMD_INDEX g_stCmdObd0Mode1Index =
{
    ( uint8 * ) u8Obd0Enter0Id1,//进入
    NULL,//空闲
    ( uint8 * ) u8Obd0Rdtc0Id1,//读码
    ( uint8 * ) u8Obd0Cdtc0Id1,//清码
    NULL,
    ( uint8 * )u8Obd0Exit0Id1,//退出(读码当做退出命令)
};
/*
********************************************************************************
                              描述:命令索引值-标准can(7E8)
********************************************************************************
*/

const TYPEDEF_CMD_INDEX g_stCmdObd0Mode2Index =
{
    ( uint8 * ) u8Obd0Enter0Id2,//进入
    NULL,//空闲
    ( uint8 * ) u8Obd0Rdtc0Id2,//读码
    ( uint8 * ) u8Obd0Cdtc0Id2,//清码
    NULL,
    NULL,
    ( uint8 * )u8Obd0Vin0Id2,  //获取VIN码
};
/*
********************************************************************************
                              描述:命令索引值-标准can(7E9)
********************************************************************************
*/

const TYPEDEF_CMD_INDEX g_stCmdObd0Mode3Index =
{
    ( uint8 * ) u8Obd0Enter0Id3,//进入
    NULL,//空闲
    ( uint8 * ) u8Obd0Rdtc0Id3,//读码
    ( uint8 * ) u8Obd0Cdtc0Id3,//清码
    NULL,
    NULL,
    ( uint8 * )u8Obd0Vin0Id3,  //获取VIN码
};
/*
********************************************************************************
                              描述:命令索引值-标准can(7EF)
********************************************************************************
*/

const TYPEDEF_CMD_INDEX g_stCmdObd0Mode4Index =
{
    ( uint8 * ) u8Obd0Enter0Id4,//进入
    NULL,//空闲
    ( uint8 * ) u8Obd0Rdtc0Id4,//读码
    ( uint8 * ) u8Obd0Cdtc0Id4,//清码
    NULL,
    NULL,
    ( uint8 * )u8Obd0Vin0Id4,  //获取VIN码
};
/*
********************************************************************************
                              描述:命令索引值-扩张can(0x18daf110)
********************************************************************************
*/
const TYPEDEF_CMD_INDEX g_stCmdObd0Mode5Index =
{
    ( uint8 * ) u8Obd0Enter0Id5,//进入
    NULL,//空闲
    ( uint8 * ) u8Obd0Rdtc0Id5,//读码
    ( uint8 * ) u8Obd0Cdtc0Id5,//清码
    NULL,
    NULL,
    ( uint8 * )u8Obd0Vin0Id5,  //获取VIN码
};
/*
********************************************************************************
                              描述:命令索引值-扩张can(0x18daf111)
********************************************************************************
*/
const TYPEDEF_CMD_INDEX g_stCmdObd0Mode6Index =
{
    ( uint8 * ) u8Obd0Enter0Id6,//进入
    NULL,//空闲
    ( uint8 * ) u8Obd0Rdtc0Id6,//读码
    ( uint8 * ) u8Obd0Cdtc0Id6,//清码
    NULL,
    NULL,
    ( uint8 * )u8Obd0Vin0Id6,  //获取VIN码
};
/*
********************************************************************************
                              描述:命令索引值-扩张can(0x18daf10e)
********************************************************************************
*/
const TYPEDEF_CMD_INDEX g_stCmdObd0Mode7Index =
{
    ( uint8 * ) u8Obd0Enter0Id7,//进入
    NULL,//空闲
    ( uint8 * ) u8Obd0Rdtc0Id7,//读码
    ( uint8 * ) u8Obd0Cdtc0Id7,//清码
    NULL,
    NULL,
    ( uint8 * )u8Obd0Vin0Id7,  //获取VIN码
};

/*
********************************************************************************
                             描述:整个系统命令总表
1.接收到的数据存储缓冲区注意低4位代码地址，高4位代表接收包数，注意是针对K线
  接收多帧
2.命令长度KWP2000用上尤其是没有长度的那种68 6A
3.帧ID命令表
4.滤波ID选择
5.备用
6.具体命令地址
********************************************************************************
*/

const TYPEDEF_CMD_TAB g_stObd0Mode0Cmdtab0[] =
{
    {0x1f, 5, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd00},//00进入命令 K(高低电平激活)      0
    {0x10, 5, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd01},//01读码命令                      1
    {0x1f, 5, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd02},//02清码命令                      2
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd03},//03读取数据流命令                3
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd04},//04读取数据流命令                4
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd05},//05读取数据流命令                5
    {0x1f, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd06},//06(标准K,地址码激活K线进入命令) 6
    {0x1f, 5, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd07},//07(标准K,地址码激活K线退出命令) 7
    //后续增加数据流标准K
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd08},//50标准K线数据流                 8
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd09},//51                              9

    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd010},//52                             10
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd011},//53                             11
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd012},//54                             12
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd013},//55                             13
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd014},//56                             14
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd015},//57                             15
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd016},//58                             16
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd017},//59                             17
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd018},//60                             18
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd019},//61                             19
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd020},//62                             20
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd021},//63                             21
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd022},//64                             22
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd023},//65                             23
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd024},//66                             24
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd025},//67                             25
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd026},//68                             26
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd027},//69                             27
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd028},//70                             28
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd029},//71                             29
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd030},//72                             30
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd031},//73                             31
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd032},//74                             32
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd033},//75                             33
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd034},//76                             34
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd035},//77                             35
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd036},//78                             36
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd037},//79                             37
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd038},//80                             38
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd039},//81                             39
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd040},//82                             40
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd041},//83                             41
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd042},//84                             42
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd043},//85                             43
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd044},//86                             44
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd045},//87                             45
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd046},//88                             46
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd047},//89                             47
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd048},//90                             48
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd049},//91                             49
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd050},//92                             50
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd051},//93                             51
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd052},//94                             52
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd053},//95                             53
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd054},//96                             54
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd055},//97                             55
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd056},//98                             56
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd057},//99                             57
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd058},//100                            58
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd059},//101                            59
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd060},//102                            60
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd061},//103                            61
    {0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd062},//104                            62
    //VIN码获取
    {0x50, 6, 0, 0, 0, ( uint8* )u8Obd0Mode0Cmd063},//106                            63
};

const TYPEDEF_CMD_TAB g_stObd0Mode1Cmdtab0[] =
{
    {  0x1f, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd00},//进入命令 68 6A                00
    {  0x10, 5, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd01},//读码命令                      01
    {  0x1f, 5, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd02},//清码命令                      02
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd03},//读取数据流命令                03
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd04},//读取数据流命令                04
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd05},//读取数据流命令                05

    //后续增加数据流68 6A
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd06},//                                06
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd07},//                                07
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd008},//                               08
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd009},//                               09
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd010},//                               10
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd011},//                               11
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd012},//                               12
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd013},//                               13
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd014},//                               14
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd015},//                               15
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd016},//                               16
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd017},//                               17
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd018},//                               18
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd019},//                               19
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd020},//                               20
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd021},//                               21
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd022},//                               22
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd023},//                               23
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd024},//                               24
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd025},//                               25
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd026},//                               26
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd027},//                               27
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd028},//                               28
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd029},//                               29
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd030},//                               30
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd031},//                               31
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd032},//                               32
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd033},//                               33
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd034},//                               34
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd035},//                               35
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd036},//                               36
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd037},//                               37
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd038},//                               38
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd039},//                               39
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd040},//                               40
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd041},//                               41
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd042},//                               42
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd043},//                               43
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd044},//                               44
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd045},//                               45
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd046},//                               46
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd047},//                               47
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd048},//                               48
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd049},//                               49
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd050},//                               50
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd051},//                               51
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd052},//                               52
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd053},//                               53
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd054},//                               54
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd055},//                               55
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd056},//                               56
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd057},//                               57
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd058},//                               58
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd059},//                               59
    {  0x10, 6, 0, 0, 0, ( uint8* )u8Obd0Mode1Cmd060},//                               60
};

const TYPEDEF_CMD_TAB g_stObd0Mode2Cmdtab0[] =
{
    {255, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd00},//进入命令 标准CAN(7e8)     0
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd01},//读码命令                  1
    {255, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd02},//清码命令                  2
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd03},//读取数据流命令            3
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd04},//读取数据流命令            4
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd05},//读取数据流命令            5

    //后续增加数据流CAN线
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd06},//                            06
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd07},//                            07
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd008},//                           08
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd009},//                           09
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd010},//                           10
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd011},//                           11
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd012},//                           12
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd013},//                           13
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd014},//                           14
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd015},//                           15
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd016},//                           16
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd017},//                           17
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd018},//                           18
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd019},//                           19
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd020},//                           20
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd021},//                           21
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd022},//                           22
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd023},//                           23
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd024},//                           24
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd025},//                           25
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd026},//                           26
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd027},//                           27
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd028},//                           28
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd029},//                           29
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd030},//                           30
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd031},//                           31
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd032},//                           32
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd033},//                           33
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd034},//                           34
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd035},//                           35
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd036},//                           36
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd037},//                           37
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd038},//                           38
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd039},//                           39
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd040},//                           40
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd041},//                           41
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd042},//                           42
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd043},//                           43
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd044},//                           44
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd045},//                           45
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd046},//                           46
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd047},//                           47
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd048},//                           48
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd049},//                           49
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd050},//                           50
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd051},//                           51
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd052},//                           52
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd053},//                           53
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd054},//                           54
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd055},//                           55
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd056},//                           56
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd057},//                           57
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd058},//                           58
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd059},//                           59
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd060},//                           60
    //VIN
    {  0, 8, 1, 0, 0, ( uint8* )u8Obd0Mode2Cmd061},//                           61
};
const TYPEDEF_CMD_TAB g_stObd0Mode2Cmdtab1[] =
{
    {255, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd00},//20进入命令 标准CAN(7e9)         0
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd01},//21读码命令                      1
    {255, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd02},//22清码命令                      2
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd03},//23读取数据流命令                3
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd04},//24读取数据流命令                4
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd05},//25读取数据流命令                5

    //后续增加数据流CAN线
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd06},//71can线数据流(7E1)   //210     06
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd07},//72                   //211     07
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd008},//12                            08
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd009},//13                            09
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd010},//14                            10
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd011},//15                            11
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd012},//16                            12
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd013},//17                            13
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd014},//18                            14
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd015},//19                            15
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd016},//20                            16
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd017},//21                            17
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd018},//22                            18
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd019},//23                            19
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd020},//24                            20
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd021},//25                            21
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd022},//26                            22
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd023},//27                            23
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd024},//28                            24
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd025},//29                            25
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd026},//30                            26
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd027},//31                            27
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd028},//32                            28
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd029},//33                            29
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd030},//34                            30
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd031},//35                            31
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd032},//36                            32
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd033},//37                            33
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd034},//38                            34
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd035},//39                            35
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd036},//40                            36
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd037},//41                            37
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd038},//42                            38
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd039},//43                            39
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd040},//44                            40
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd041},//45                            41
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd042},//46                            42
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd043},//47                            43
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd044},//48                            44
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd045},//49                            45
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd046},//50                            46
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd047},//51                            47
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd048},//52                            48
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd049},//53                            49
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd050},//54                            50
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd051},//55                            51
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd052},//56                            52
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd053},//57                            53
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd054},//58                            54
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd055},//59                            55
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd056},//60                            56
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd057},//61                            57
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd058},//62                            58
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd059},//61                            59
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd060},//62                            60
    //VIN                                           /
    {  0, 8, 1, 1, 0, ( uint8* )u8Obd0Mode2Cmd061},//63                            61
};
const TYPEDEF_CMD_TAB g_stObd0Mode2Cmdtab2[] =
{
    {255, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd00},//26进入命令 标准CAN(7ef)     00
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd01},//27读码命令                  01
    {255, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd02},//28清码命令                  02
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd03},//29读取数据流命令            03
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd04},//30读取数据流命令            04
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd05},//31读取数据流命令            05

    //后续增加数据流CAN线
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd06},//can线数据流(7E7)            06
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd07},//                            07

    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd008},//                           08
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd009},//                           09
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd010},//                           10
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd011},//                           11
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd012},//                           12
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd013},//                           13
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd014},//                           14
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd015},//                           15
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd016},//                           16
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd017},//                           17
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd018},//                           18
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd019},//                           19
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd020},//                           20
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd021},//                           21
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd022},//                           22
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd023},//                           23
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd024},//                           24
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd025},//                           25
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd026},//                           26
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd027},//                           27
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd028},//                           28
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd029},//                           29
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd030},//                           30
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd031},//                           31
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd032},//                           32
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd033},//                           33
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd034},//                           34
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd035},//                           35
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd036},//                           36
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd037},//                           37
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd038},//                           38
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd039},//                           39
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd040},//                           40
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd041},//                           41
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd042},//                           42
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd043},//                           43
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd044},//                           44
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd045},//                           45
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd046},//                           46
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd047},//                           47
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd048},//                           48
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd049},//                           49
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd050},//                           50
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd051},//                           51
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd052},//                           52
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd053},//                           53
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd054},//                           54
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd055},//                           55
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd056},//                           56
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd057},//                           57
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd058},//                           58
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd059},//                           59
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd060},//                           60
    //VIN
    {  0, 8, 1, 2, 0, ( uint8* )u8Obd0Mode2Cmd061},//                           61

};
const TYPEDEF_CMD_TAB g_stObd0Mode2Cmdtab3[] =
{
    {255, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd00},//进入命令 扩张CAN(0x18daf110)   00
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd01},//读码命令                       01
    {255, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd02},//清码命令                       02
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd03},//读取数据流命令                 03
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd04},//读取数据流命令                 04
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd05},//读取数据流命令                 05

    //后续增加数据流扩展CAN线
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd06},//can线数据流0x18daf110            06
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd07},//                                 07

    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd008},//                                08
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd009},//                                09
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd010},//                                10
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd011},//                                11
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd012},//                                12
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd013},//                                13
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd014},//                                14
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd015},//                                15
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd016},//                                16
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd017},//                                17
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd018},//                                18
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd019},//                                19
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd020},//                                20
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd021},//                                21
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd022},//                                22
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd023},//                                23
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd024},//                                24
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd025},//                                25
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd026},//                                26
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd027},//                                27
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd028},//                                28
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd029},//                                29
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd030},//                                30
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd031},//                                31
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd032},//                                32
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd033},//                                33
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd034},//                                34
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd035},//                                35
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd036},//                                36
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd037},//                                37
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd038},//                                38
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd039},//                                39
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd040},//                                40
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd041},//                                41
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd042},//                                42
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd043},//                                43
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd044},//                                44
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd045},//                                45
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd046},//                                46
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd047},//                                47
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd048},//                                48
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd049},//                                49
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd050},//                                50
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd051},//                                51
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd052},//                                52
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd053},//                                53
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd054},//                                54
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd055},//                                55
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd056},//                                56
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd057},//                                57
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd058},//                                58
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd059},//                                59
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd060},//                                60
    //VIN
    {  0, 8, 2, 3, 0, ( uint8* )u8Obd0Mode2Cmd061},//                                61
};
const TYPEDEF_CMD_TAB g_stObd0Mode2Cmdtab4[] =
{
    {255, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd00},//进入命令 扩张CAN(0x18daf111)   00
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd01},//读码命令                       01
    {255, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd02},//清码命令                       02
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd03},//读取数据流命令                 03
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd04},//读取数据流命令                 04
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd05},//读取数据流命令                 05

    //后续增加数据流扩展CAN线
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd06},//can线数据流0x18daf111          06
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd07},//                               07

    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd008},//                                08
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd009},//                                09
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd010},//                                10
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd011},//                                11
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd012},//                                12
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd013},//                                13
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd014},//                                14
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd015},//                                15
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd016},//                                16
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd017},//                                17
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd018},//                                18
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd019},//                                19
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd020},//                                20
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd021},//                                21
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd022},//                                22
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd023},//                                23
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd024},//                                24
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd025},//                                25
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd026},//                                26
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd027},//                                27
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd028},//                                28
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd029},//                                29
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd030},//                                30
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd031},//                                31
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd032},//                                32
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd033},//                                33
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd034},//                                34
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd035},//                                35
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd036},//                                36
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd037},//                                37
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd038},//                                38
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd039},//                                39
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd040},//                                40
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd041},//                                41
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd042},//                                42
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd043},//                                43
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd044},//                                44
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd045},//                                45
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd046},//                                46
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd047},//                                47
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd048},//                                48
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd049},//                                49
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd050},//                                50
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd051},//                                51
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd052},//                                52
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd053},//                                53
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd054},//                                54
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd055},//                                55
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd056},//                                56
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd057},//                                57
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd058},//                                58
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd059},//                                59
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd060},//                                60
    //VIN
    {  0, 8, 2, 4, 0, ( uint8* )u8Obd0Mode2Cmd061},//                                61
};
const TYPEDEF_CMD_TAB g_stObd0Mode2Cmdtab5[] =
{
    {255, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd00},//进入命令 扩张CAN(0x18daf10e)     00
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd01},//读码命令                         01
    {255, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd02},//清码命令                         02
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd03},//读取数据流命令                   03
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd04},//读取数据流命令                   04
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd05},//读取数据流命令                   05


    //后续增加数据流扩展CAN线
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd06},//                                  06
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd07},//                                  07

    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd008},//                                 08
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd009},//                                 09
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd010},//                                 10
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd011},//                                 11
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd012},//                                 12
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd013},//                                 13
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd014},//                                 14
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd015},//                                 15
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd016},//                                 16
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd017},//                                 17
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd018},//                                 18
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd019},//                                 19
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd020},//                                 20
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd021},//                                 21
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd022},//                                 22
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd023},//                                 23
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd024},//                                 24
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd025},//                                 25
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd026},//                                 26
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd027},//                                 27
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd028},//                                 28
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd029},//                                 29
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd030},//                                 30
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd031},//                                 31
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd032},//                                 32
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd033},//                                 33
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd034},//                                 34
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd035},//                                 35
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd036},//                                 36
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd037},//                                 37
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd038},//                                 38
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd039},//                                 39
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd040},//                                 40
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd041},//                                 41
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd042},//                                 42
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd043},//                                 43
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd044},//                                 44
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd045},//                                 45
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd046},//                                 46
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd047},//                                 47
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd048},//                                 48
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd049},//                                 49
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd050},//                                 50
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd051},//                                 51
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd052},//                                 52
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd053},//                                 53
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd054},//                                 54
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd055},//                                 55
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd056},//                                 56
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd057},//                                 57
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd058},//                                 58
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd059},//                                 59
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd060},//                                 60
    //VIN
    {  0, 8, 2, 5, 0, ( uint8* )u8Obd0Mode2Cmd061},//                                 61
};
/*
********************************************************************************
                            描述:系统读取数据流
********************************************************************************
*/
//K线
const TYPEDEF_DATASTREAM_GET_DATA_LIST g_stObd0mode0DataStream[] =
{

    { 2,  2,   3},    //00  水温
    { 2,  1,   4},    //01  电压
    { 2,  2,   5},    //02  转速

    //后续增加数据流
    { 2,  2,   8},    //03  21
    { 2,  2,   9},    //04  31

    { 2,  1,   10},      //001
    { 2,  1,   10},      //002
    { 3,  1,   10},      //003
    { 3,  1,   10},      //004
    { 3,  1,   10},      //005
    { 3,  1,   10},      //006
    { 3,  1,   10},      //007
    { 3,  1,   10},      //008
    { 4,  1,   10},      //009
    { 4,  1,   10},      //010
    { 4,  1,   10},      //011
    { 4,  1,   10},      //012
    { 4,  1,   10},      //013
    { 4,  1,   10},      //014
    { 4,  1,   10},      //015
    { 4,  1,   10},      //016
    { 5,  1,   10},      //017
    { 5,  1,   10},      //018
    { 5,  1,   10},      //019
    { 5,  1,   10},      //020
    { 5,  1,   10},      //021
    { 5,  1,   10},      //022
    { 5,  1,   10},      //023
    { 5,  1,   10},      //024
    { 2,  2,   11},      //025
    { 2,  1,   12},      //026
    { 2,  1,   4},      //027
    { 2,  1,   13},      //028
    { 2,  1,   14},      //029
    { 2,  1,   15},      //030
    { 2,  1,   16},      //031
    { 2,  1,   17},      //032
    { 2,  1,   18},      //033
    { 2,  2,   5},       //034
    { 2,  1,   19},      //035
    { 2,  1,   20},      //036
    { 2,  1,   21},      //037
    { 2,  2,   22},      //038
    { 2,  1,   23},      //039
    { 2,  1,   24},      //040
    { 3,  1,   24},      //041
    { 2,  1,   25},      //042
    { 3,  1,   25},      //043
    { 2,  1,   26},      //044
    { 3,  1,   26},      //045
    { 2,  1,   27},      //046
    { 3,  1,   27},      //047
    { 2,  1,   28},      //048
    { 3,  1,   28},      //049
    { 2,  1,   29},      //050
    { 3,  1,   29},      //051
    { 2,  1,   30},      //052
    { 3,  1,   30},      //053
    { 2,  1,   31},      //054
    { 3,  1,   31},      //055
    { 2,  1,   32},      //056
    { 2,  2,   33},      //057
    { 2,  2,   34},      //058
    { 2,  2,   35},      //059
    { 2,  2,   36},      //060
    { 4,  2,   36},      //061
    { 2,  2,   37},      //062
    { 4,  2,   37},      //063
    { 2,  2,   38},      //064
    { 4,  2,   38},      //065
    { 2,  2,   39},      //066
    { 4,  2,   39},      //067
    { 2,  2,   40},      //068
    { 4,  2,   40},      //069
    { 2,  2,   41},      //070
    { 4,  2,   41},      //071
    { 2,  2,   42},      //072
    { 4,  2,   42},      //073
    { 2,  2,   43},      //074
    { 4,  2,   43},      //075
    { 2,  1,   44},      //076
    { 2,  1,   45},      //077
    { 2,  1,   46},      //078
    { 2,  1,   47},      //079
    { 2,  1,   48},      //080
    { 2,  2,   49},      //081
    { 2,  1,   50},      //082
    { 2,  2,   51},      //083
    { 4,  2,   51},      //084
    { 2,  2,   52},      //085
    { 4,  2,   52},      //086
    { 2,  2,   53},      //087
    { 4,  2,   53},      //088
    { 2,  2,   54},      //089
    { 4,  2,   54},      //090
    { 2,  2,   55},      //091
    { 4,  2,   55},      //092
    { 2,  2,   56},      //093
    { 4,  2,   56},      //094
    { 2,  2,   57},      //095
    { 4,  2,   57},      //096
    { 2,  2,   58},      //097
    { 4,  2,   58},      //098
    { 2,  2,   59},      //099
    { 2,  2,   60},      //100
    { 2,  2,   61},      //101
    { 2,  2,   62},      //102
    { 2,  2,   3},      //103
};
//68 6a(三菱车校队通过)
const TYPEDEF_DATASTREAM_GET_DATA_LIST g_stObd0mode1DataStream[] =
{

    { 2,  2,   3},       //00  水温
    { 2,  1,   4},       //01  电压
    { 2,  2,   5},       //02  转速

    //后续增加数据流
    { 2,  2,   6},       //3   21
    { 2,  2,   7},       //4   31

    { 2,  1,   8},      //001
    { 2,  1,   8},      //002
    { 3,  1,   8},      //003
    { 3,  1,   8},      //004
    { 3,  1,   8},      //005
    { 3,  1,   8},      //006
    { 3,  1,   8},      //007
    { 3,  1,   8},      //008
    { 4,  1,   8},      //009
    { 4,  1,   8},      //010
    { 4,  1,   8},      //011
    { 4,  1,   8},      //012
    { 4,  1,   8},      //013
    { 4,  1,   8},      //014
    { 4,  1,   8},      //015
    { 4,  1,   8},      //016
    { 5,  1,   8},      //017
    { 5,  1,   8},      //018
    { 5,  1,   8},      //019
    { 5,  1,   8},      //020
    { 5,  1,   8},      //021
    { 5,  1,   8},      //022
    { 5,  1,   8},      //023
    { 5,  1,   8},      //024
    { 2,  2,   9},      //025
    { 2,  1,   10},     //026
    { 2,  1,   4},      //027
    { 2,  1,   11},     //028
    { 2,  1,   12},     //029
    { 2,  1,   13},     //030
    { 2,  1,   14},     //031
    { 2,  1,   15},     //032
    { 2,  1,   16},     //033
    { 2,  2,   5},      //034
    { 2,  1,   17},     //035
    { 2,  1,   18},     //036
    { 2,  1,   19},     //037
    { 2,  2,   20},     //038
    { 2,  1,   21},     //039
    { 2,  1,   22},     //040
    { 3,  1,   22},     //041
    { 2,  1,   23},     //042
    { 3,  1,   23},     //043
    { 2,  1,   24},     //044
    { 3,  1,   24},     //045
    { 2,  1,   25},     //046
    { 3,  1,   25},     //047
    { 2,  1,   26},     //048
    { 3,  1,   26},     //049
    { 2,  1,   27},     //050
    { 3,  1,   27},     //051
    { 2,  1,   28},     //052
    { 3,  1,   28},     //053
    { 2,  1,   29},     //054
    { 3,  1,   29},     //055
    { 2,  1,   30},     //056
    { 2,  2,   31},     //057
    { 2,  2,   32},     //058
    { 2,  2,   33},     //059
    { 2,  2,   34},     //060
    { 4,  2,   34},     //061
    { 2,  2,   35},     //062
    { 4,  2,   35},     //063
    { 2,  2,   36},     //064
    { 4,  2,   36},     //065
    { 2,  2,   37},     //066
    { 4,  2,   37},     //067
    { 2,  2,   38},     //068
    { 4,  2,   38},     //069
    { 2,  2,   39},     //070
    { 4,  2,   39},     //071
    { 2,  2,   40},     //072
    { 4,  2,   40},     //073
    { 2,  2,   41},     //074
    { 4,  2,   41},     //075
    { 2,  1,   42},     //076
    { 2,  1,   43},     //077
    { 2,  1,   44},     //078
    { 2,  1,   45},     //079
    { 2,  1,   46},     //080
    { 2,  2,   47},     //081
    { 2,  1,   48},     //082
    { 2,  2,   49},     //083
    { 4,  2,   49},     //084
    { 2,  2,   50},     //085
    { 4,  2,   50},     //086
    { 2,  2,   51},     //087
    { 4,  2,   51},     //088
    { 2,  2,   52},     //089
    { 4,  2,   52},     //090
    { 2,  2,   53},     //091
    { 4,  2,   53},     //092
    { 2,  2,   54},     //093
    { 4,  2,   54},     //094
    { 2,  2,   55},     //095
    { 4,  2,   55},     //096
    { 2,  2,   56},     //097
    { 4,  2,   56},     //098
    { 2,  2,   57},     //099
    { 2,  2,   58},     //100
    { 2,  2,   59},     //101
    { 2,  2,   60},     //102
    { 2,  2,   3},      //103
};


//000  故障码存储数量                    |          |        01   |ret01:   BYTE5低7位             // 10 (check)  4
//001  故障指示灯状态                    |          |        01   |ret01:   BYTE5.bit7             // 10 (check)  on
//002  支持失火监测                      |          |        01   |ret01:   BYTE6.bit0             // 10 (check)  4
//003  支持燃油系统监测                  |          |        01   |ret01:   BYTE6.bit1             // 10 (check)  4
//004  支持综合部件监测                  |          |        01   |ret01:   BYTE6.bit2             // 10 (check)  4
//005  失火监测准备就绪                  |          |        01   |ret01:   BYTE6.bit4             // 10
//006  燃油系统的监测准备就绪            |          |        01   |ret01:   BYTE6.bit5             // 10
//007  综合成分监测准备就绪              |          |        01   |ret01:   BYTE6.bit6             // 10
//008  支持:催化转换器                   |          |        01   |ret01:   BYTE7.BIT0             // 10
//009  支持:加热器催化转换器             |          |        01   |ret01:   BYTE7.BIT1             // 10
//010  支持:燃油蒸发排放系统             |          |        01   |ret01:   BYTE7.BIT2             // 10
//011  支持:二次空气喷射系统             |          |        01   |ret01:   BYTE7.BIT3             // 10
//012  支持:空调冷媒                     |          |        01   |ret01:   BYTE7.BIT4             // 10
//013  支持:氧传感器                     |          |        01   |ret01:   BYTE7.BIT5             // 10
//014  支持:氧传感器加热器               |          |        01   |ret01:   BYTE7.BIT6             // 10
//015  支持:废气再循环系统               |          |        01   |ret01:   BYTE7.BIT7             // 10
//016  催化转换器状态                    |          |        01   |ret01:   BYTE8.BIT0             // 10
//017  加热式催化转换器状态              |          |        01   |ret01:   BYTE8.BIT1             // 10
//018  燃油蒸发排放系统状态              |          |        01   |ret01:   BYTE8.BIT2             // 10
//019  二次空气喷射系统状态              |          |        01   |ret01:   BYTE8.BIT3             // 10
//020  空调制冷剂状态                    |          |        01   |ret01:   BYTE8.BIT4             // 10
//021  氧传感器状态                      |          |        01   |ret01:   BYTE8.BIT5             // 10
//022  氧传感器加热器状态                |          |        01   |ret01:   BYTE8.BIT6             // 10
//023  废气再循环系统状态                |          |        01   |ret01:   BYTE8.BIT7             // 10
//024  产生冻结帧的故障码                |          |        02   |ret02:   byte5,6                // 11
//025  燃油系统状态 B1                   |          |        03   |ret03:   BYTE5                  // 暂时去掉
//026  燃油系统状态 B2                   |          |        03   |ret03:   BYTE6                  // 暂时去掉
//027  计算负荷值                        |          |%       04   |ret04:   BYTE5*100/255          // 12
//028  发动机冷却液温度                  |          |℃      05   |ret05:   BYTE5-40               // 04
//029  短期燃油修正(缸组1)               |          |%       06   |ret06:   BYTE5*100/128-100      // 13
//030  长期燃油修正(缸组1)               |          |%       07   |ret07:   BYTE5*100/128-100      // 14
//031  短期燃油修正(缸组2)               |          |%       08   |ret08:   BYTE5*100/128-100      // 15
//032  长期燃油修正(缸组2)               |          |%       09   |ret09:   BYTE5*100/128-100      // 16
//033  燃油压力                          |          |kPaG    0A   |ret0A:   BYTE5*3                // 17
//034  进气歧管绝对压力                  |          |kPaA    0B   |ret0B:   BYTE5*1                // 18
//035  发动机转速                        |          |RPM     0C   |ret0C:   BYTE5,6/4              // 05
//036  车速                              |          |km/h    0D   |ret0D:   BYTE5*1                // 19
//037  点火正时                          |          |°       0E   |ret0E:   BYTE5/2-64             // 20
//038  进气温度                          |          |℃      0F   |ret0F:   BYTE5-40               // 21
//039  空气流量                          |          |g/s     10   |ret10:   BYTE5,6*0.01           // 22
//040  节气门位置                        |          |%       11   |ret11:   BYTE5*100/255          // 23
//041  二次空气请求                      |          |        12   |ret12:   BYTE5                  //  暂时去掉
//042  氧传感器位置                      |          |        13   |ret13:   BYTE5                  // 暂时去掉
//043  氧传感器输出电压(缸组1,传感器1)   |          |V       14   |ret14:   BYTE5*0.005            // 24
//044  短期燃油修正(缸组1,传感器1)       |          |%       14   |ret14:   BYTE6*100/128-100      // 24
//045  氧传感器输出电压(缸组1,传感器2)   |          |V       15   |ret15:   BYTE5*0.005            // 25
//046  短期燃油修正(缸组1,传感器2)       |          |%       15   |ret15:   BYTE6*100/128-100      // 25
//047  氧传感器输出电压(缸组1,传感器3)   |          |V       16   |ret16:   BYTE5*0.005            // 26
//048  短期燃油修正(缸组1,传感器3)       |          |%       16   |ret16:   BYTE6*100/128-100      // 26
//049  氧传感器输出电压(缸组1,传感器4)   |          |V       17   |ret17:   BYTE5*0.005            // 27
//050  短期燃油修正(缸组1,传感器4)       |          |%       17   |ret17:   BYTE6*100/128-100      // 27
//051  氧传感器输出电压(缸组2,传感器1)   |          |V       18   |ret18:   BYTE5*0.005            // 28
//052  短期燃油修正(缸组2,传感器1)       |          |%       18   |ret18:   BYTE6*100/128-100      // 28
//053  氧传感器输出电压(缸组2,传感器2)   |          |V       19   |ret19:   BYTE5*0.005            // 29
//054  短期燃油修正(缸组2,传感器2)       |          |%       19   |ret19:   BYTE6*100/128-100      // 29
//055  氧传感器输出电压(缸组2,传感器3)   |          |V       1A   |ret1A:   BYTE5*0.005            // 30
//056  短期燃油修正(缸组2,传感器3)       |          |%       1A   |ret1A:   BYTE6*100/128-100      // 30
//057  氧传感器输出电压(缸组2,传感器4)   |          |V       1B   |ret1B:   BYTE5*0.005            // 31
//058  短期燃油修正(缸组2,传感器4)       |          |%       1B   |ret1B:   BYTE6*100/128-100      // 31
//059  车辆或发动机认证的OBD要求         |          |        1C   |ret1C:   BYTE5                  //  暂时去掉
//060  氧传感器位置                      |          |        1D   |ret1D:   BYTE5                  //  暂时去掉
//061  PTO(动力输出)状态                 |          |        1E   |ret1E:   BYTE5.bit0             // 32
//062  发动机起动后时间                  |          |sec     1f   |ret1f:   BYTE5,6                // 33
//063  故障指示灯点亮后的行驶距离        |          |Km      21   |ret21:   BYTE5,6                // 08
//064  相对燃油压力                      |          |kPa     22   |ret22:   BYTE5,6                // 34
//065  燃油轨压力                        |          |kPa     23   |ret23:   BYTE5,6*10             // 35
//066  当量比(λ)(缸组1,传感器1)         |          |        24   |ret24:   BYTE5,6*1.999/65535     // 36
//067  氧传感器电压(缸组1,传感器1)       |          |mV      24   |ret24:   BYTE7,8*7999/65535     // 36
//068  当量比(λ)(缸组1,传感器2)         |          |        25   |ret25:   BYTE5,6*1.999/65535     // 37
//069  氧传感器电压(缸组1,传感器2)       |          |mV      25   |ret25:   BYTE7,8*7999/65535     // 37
//070  当量比(λ)(缸组1,传感器3)         |          |        26   |ret26:   BYTE5,6*1.999/65535     // 38
//071  氧传感器电压(缸组1,传感器3)       |          |mV      26   |ret26:   BYTE7,8*7999/65535     // 38
//072  当量比(λ)(缸组1,传感器4)         |          |        27   |ret27:   BYTE5,6*1.999/65535     // 39
//073  氧传感器电压(缸组1,传感器4)       |          |mV      27   |ret27:   BYTE7,8*7999/65535     // 39
//074  当量比(λ)(缸组2,传感器1)         |          |        28   |ret28:   BYTE5,6*1.999/65535     // 40
//075  氧传感器电压(缸组2,传感器1)       |          |mV      28   |ret28:   BYTE7,8*7999/65535     // 40
//076  当量比(λ)(缸组2,传感器2)         |          |        29   |ret29:   BYTE5,6*1.999/65535     // 41
//077  氧传感器电压(缸组2,传感器2)       |          |mV      29   |ret29:   BYTE7,8*7999/65535     // 41
//078  当量比(λ)(缸组2,传感器3)         |          |        2A   |ret2A:   BYTE5,6*1.999/65535     // 42
//079  氧传感器电压(缸组2,传感器3)       |          |mV      2A   |ret2A:   BYTE7,8*7999/65535     // 42
//080  当量比(λ)(缸组2,传感器4)         |          |        2B   |ret2B:   BYTE5,6*1.999/65535     // 43
//081  氧传感器电压(缸组2,传感器4)       |          |mV      2B   |ret2B:   BYTE7,8*7999/65535     // 43
//082  废气再循环指令                    |          |%       2C   |ret2C:   BYTE5*100/255          // 44
//083  废气再循环错误                    |          |%       2D   |ret2D:   BYTE5*100/128-100      // 45
//084  燃油蒸气净化                      |          |%       2E   |ret2E:   BYTE5*100/255          // 46
//085  燃油液位输入                      |          |%       2F   |ret2F:   BYTE5*100/255          // 47
//086  清除故障码后的暖机次数            |          |        30   |ret30:   BYTE5十进制            // 48
//087  清除故障码后的行驶距离            |          |        31   |ret31:   BYTE5,6(DEC)           // 09
//088  燃油蒸气系统压力                  |          |Pa      32   |ret32:   BYTE5,6*1/4            // 49
//089  大气压力                          |          |kPa     33   |ret33:   BYTE5十进制            // 50
//090  当量比(λ)(缸组1,传感器1)         |          |        34   |ret34:   BYTE5,6*1.999/65535     // 51
//091  氧传感器电流(缸组1,传感器1)       |          |mA      34   |ret34:   BYTE7,8*256/65535-128  // 51
//092  当量比(λ)(缸组1,传感器2)         |          |        35   |ret35:   BYTE5,6*1.999/65535     // 52
//093  氧传感器电流(缸组1,传感器2)       |          |mA      35   |ret35:   BYTE7,8*256/65535-128  // 52
//094  当量比(λ)(缸组1,传感器3)         |          |        36   |ret36:   BYTE5,6*1.999/65535     // 53
//095  氧传感器电流(缸组1,传感器3)       |          |mA      36   |ret36:   BYTE7,8*256/65535-128  // 53
//096  当量比(λ)(缸组1,传感器4)         |          |        37   |ret37:   BYTE5,6*1.999/65535     // 54
//097  氧传感器电流(缸组1,传感器4)       |          |mA      37   |ret37:   BYTE7,8*256/65535-128  // 54
//098  当量比(λ)(缸组2,传感器1)         |          |        38   |ret38:   BYTE5,6*1.999/65535     // 55
//099  氧传感器电流(缸组2,传感器1)       |          |mA      38   |ret38:   BYTE7,8*256/65535-128  // 55
//100  当量比(λ)(缸组2,传感器2)         |          |        39   |ret39:   BYTE5,6*1.999/65535     // 56
//101  氧传感器电流(缸组2,传感器2)       |          |mA      39   |ret39:   BYTE7,8*256/65535-128  // 56
//102  当量比(λ)(缸组2,传感器3)         |          |        3A   |ret3A:   BYTE5,6*1.999/65535     // 57
//103  氧传感器电流(缸组2,传感器3)       |          |mA      3A   |ret3A:   BYTE7,8*256/65535-128  // 57
//104  当量比(λ)(缸组2,传感器4)         |          |        3B   |ret3B:   BYTE5,6*1.999/65535     // 58
//105  氧传感器电流(缸组2,传感器4)       |          |mA      3B   |ret3B:   BYTE7,8*256/65535-128  // 58
//106  催化剂温度(缸组1,传感器1)         |          |℃      3C   |ret3C:   BYTE5,6/10-40          // 59
//107  催化剂温度(缸组2,传感器1)         |          |℃      3D   |ret3D:   BYTE5,6/10-40          // 60
//108  催化剂温度(缸组1,传感器2)         |          |℃      3E   |ret3E:   BYTE5,6/10-40          // 61
//109  催化剂温度(缸组2,传感器2)         |          |℃      3F   |ret3F:   BYTE5,6/10-40          // 62
//110  控制模块电压                      |          |        42   |ret00:   byte5,6/1000           // 03
