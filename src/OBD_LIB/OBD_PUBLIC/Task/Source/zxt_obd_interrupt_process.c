/*
********************************************************************************

* File                   : zxt_obd_interript_process
* By                    : Mitch mouse
* Version: V1.00
*description :obd�����жϴ���
********************************************************************************
*/
#include "../OBD_LIB/OBD_PUBLIC/Driver/Inc/zxt_obd_include.h"
/*
********************************************************************************
Name        :void USART3_IRQHandler(void)
Description:K�ߴ�����
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/

void USART3_IRQHandler( void )
{
    OS_CPU_SR  cpu_sr;
    const TYPEDEF_pPROTOCOLLIB *pprotocol = g_stProtocolCheckLlib;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    //������ʱ��һ��������������,�Ժ����ֱ�ӷŵ�����ĵ�ַ����
    if( USART_GetITStatus( KLINE_COMMUNICATION_PORT, USART_IT_RXNE ) != RESET )
    {
        g_stDataFromEcu.u8Buf[g_stUsart3Iso14230ReceiveVar.u8DataOffset++] = USART_ReceiveData( KLINE_COMMUNICATION_PORT );;
        ( *( pprotocol + g_u8ProtocolLibOffset ) )( ( void* ) 0, ( void* ) 0 );
    }

    OSIntExit();
}
/*
*******************************************************************************
Name       :USB_LP_CAN1_RX0_IRQHandler
Description:CAN�ж���ں���
Input      :none
Output     :none
Return     :none
Others     :
********************************************************************************
*/
void CAN1_RX0_IRQHandler( void )
{
    OS_CPU_SR  cpu_sr;
    const TYPEDEF_pPROTOCOLLIB *pprotocol = g_stProtocolCheckLlib;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    if( CAN_GetITStatus( CAN_COMMUNICATION_PORT, CAN_IT_FMP0 ) != RESET )
    {
        CAN_Receive( CAN_COMMUNICATION_PORT, CAN_FIFO0, &g_stCAN115765ReceiveVar.RxMessage );
			  g_stCAN115765ReceiveVar_temp = &g_stCAN115765ReceiveVar;
        ( *( pprotocol + g_u8ProtocolLibOffset ) )( ( void* ) 0, ( void* ) 0 );
    }

    OSIntExit();
}
/*
*******************************************************************************
Name       :CAN2_RX0_IRQHandler
Description:CAN�ж���ں���
Input      :none
Output     :none
Return     :none
Others     :can2�ж�û�н����κδ���,����Ҫʹ�õ�ʱ���ڴ���
********************************************************************************
*/
void CAN2_RX0_IRQHandler( void )
{
    OS_CPU_SR  cpu_sr;
	const TYPEDEF_pPROTOCOLLIB *pprotocol = g_stProtocolCheckLlib;
    OS_ENTER_CRITICAL();
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    if( CAN_GetITStatus( CAN_COMMUNICATION_PORT1, CAN_IT_FMP0 ) != RESET )
    {
        CAN_Receive( CAN_COMMUNICATION_PORT1, CAN_FIFO0, &g_stCAN115765ReceiveVar1.RxMessage );
			  g_stCAN115765ReceiveVar_temp = &g_stCAN115765ReceiveVar1;
        ( *( pprotocol + g_u8ProtocolLibOffset ) )( ( void* ) 0, ( void* ) 0 );
    }

    OSIntExit();
}
