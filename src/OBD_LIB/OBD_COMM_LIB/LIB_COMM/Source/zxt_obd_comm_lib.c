#define LIB_COMM_GVAR
#include "../OBD_LIB/OBD_COMM_LIB/LIB_COMM/Inc/zxt_obd_comm_lib.h"

TYPEDEF_FRAME_STATUS g_commstaus = NOSTAUS;
uint8   g_u8SentDataToEcu[255] = {0};
TYPEDEF_PROTOCOL_MSGBUF g_stDataFromEcu =  {0, 0};
TYPEDEF_PROTOCOL_MSGBUF g_stu8CacheData[3] = {0};
TYPEDEF_ISO14230_RECEIVE g_stUsart3Iso14230ReceiveVar = {0, 0, 0, 0, 0, FALSE};
TYPEDEF_CANMSG g_stCAN115765ReceiveVar = {0, 0, {0}, FALSE, FALSE, 0};
TYPEDEF_CANMSG g_stCAN115765ReceiveVar1 = {0, 0, {0}, FALSE, FALSE, 0};
TYPEDEF_CANMSG *g_stCAN115765ReceiveVar_temp = &g_stCAN115765ReceiveVar;
uint8 g_u8ProtocolLibOffset = 0;
const TYPEDEF_FRAME_STATUS g_enumFrameFlgArr[] = {TIMEOUT, FRAMETIMEOUT, OK , NOSTAUS};
CanTxMsg g_stTxMessage = {0};
CanTxMsg g_stTxMessage1 = {0};

const TYPEDEF_MODE_CELL *g_pModeLibTab_lib = NULL;
const TYPEDEF_2WORD4BYTE	*g_unnu32FrameIdLib_lib = NULL;
const TYPEDEF_2WORD4BYTE	*g_unnu32FilterId_lib = NULL;
const TYPEDEF_ISO14230_COMMUNICATION_PAR *g_stIsoKlineCommLib_lib = NULL;
const TYPEDEF_ISO15765_ACTIVE_PAR *g_stIso15765ActiveLib_lib = NULL;


OSDELAY	pfOSdelay = NULL;
OSMboxPST pfOSboxpst = NULL;
OSMboxPED	pfOSboxped = NULL;
OS_EVENT  *g_pstEcuToPerformer_lib= NULL;

