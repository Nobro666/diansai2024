#include "uart.h"
void uart0_send_char(char ch)
{
	while( DL_UART_isBusy(UART_k230_INST) == true );
	DL_UART_Main_transmitData(UART_k230_INST, ch);

}
void uart0_send_string(char* str)
{
	
	while(*str!=0&&str!=0)
	{
		uart0_send_char(*str++);
	}
}

// static char uart0_rx_buf[64];
// static uint8_t uart0_rx_idx = 0;
// static bool    uart0_rx_done = false;
// /* UART_0 中断处理 */
// void UART_3_INST_IRQHandler(void)
// {
//     switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
//         case DL_UART_MAIN_IIDX_RX:
//             DL_GPIO_togglePins(GPIO_LEDS_PORT,
//                 GPIO_LEDS_USER_LED_1_PIN | GPIO_LEDS_USER_TEST_PIN);
//             gEchoData = DL_UART_Main_receiveData(UART_0_INST);
//             DL_UART_Main_transmitData(UART_0_INST, gEchoData);
//             break;
//         default:
//             break;
//     }
// }