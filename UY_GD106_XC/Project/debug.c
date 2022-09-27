#include "debug.h"
#include <stdio.h>

#define DEBUG_UART_EN 0

void debug_init(uint32_t BAUD)
{	
		nvic_irq_enable(DEBUG_UART_IRQ, 0, 0);
		
		rcu_periph_clock_enable(DEBUG_TX_RCU);
		
		rcu_periph_clock_enable(DEBUG_RX_RCU);

    /* enable USART clock */
    rcu_periph_clock_enable(DEBUG_UART_CLK);

    /* connect port to USARTx_Tx */
    gpio_af_set(DEBUG_TX_PORT, DEBUG_UART_AF, DEBUG_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_af_set(DEBUG_RX_PORT, DEBUG_UART_AF, DEBUG_RX_PIN);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(DEBUG_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,DEBUG_TX_PIN);
    gpio_output_options_set(DEBUG_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,DEBUG_TX_PIN);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(DEBUG_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,DEBUG_RX_PIN);
    gpio_output_options_set(DEBUG_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,DEBUG_RX_PIN);

    /* USART configure */
    usart_deinit(DEBUG_UART);
    usart_baudrate_set(DEBUG_UART,BAUD);
    usart_receive_config(DEBUG_UART, USART_RECEIVE_ENABLE);
    usart_transmit_config(DEBUG_UART, USART_TRANSMIT_ENABLE);
    usart_enable(DEBUG_UART);
		
		/* enable USART0 receive interrupt */
    usart_interrupt_enable(DEBUG_UART, USART_INT_RBNE);
}

/* retarget the C library printf function to the USART0 */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(DEBUG_UART, (uint8_t)ch);
    while(RESET == usart_flag_get(DEBUG_UART, USART_FLAG_TBE));

    return ch;
}

#if DEBUG_UART_EN == 0
void DEBUG_IRQHandler(void)
{
	if(RESET != usart_interrupt_flag_get(DEBUG_UART, USART_INT_FLAG_RBNE))
	{
				usart_data_receive(DEBUG_UART);
//        usart_interrupt_flag_clear(DEBUG_UART,USART_INT_FLAG_RBNE);
	}
}

//#elif DEBUG_UART == 2
//void USART2_IRQHandler(void)
//{
//	if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_RBNE))
//	{
//        usart_interrupt_flag_clear(USART2,USART_INT_FLAG_RBNE);
//		usart_data_transmit(USART2, (uint8_t)usart_data_receive(USART2));
//	}
//}

#endif
