#ifndef __DEBUG_H
#define __DEBUG_H	 

#include "gd32f4xx.h"


//UART3
#define DEBUG_TX_PORT 		(GPIOA)
#define DEBUG_TX_PIN 		(GPIO_PIN_0)
#define	DEBUG_TX_RCU		(RCU_GPIOA)


#define DEBUG_RX_PORT 		(GPIOA)
#define DEBUG_RX_PIN 		(GPIO_PIN_1)
#define	DEBUG_RX_RCU		(RCU_GPIOA)

#define DEBUG_UART UART3

#define DEBUG_UART_IRQHandler             UART3_IRQHandler
#define DEBUG_UART_IRQ                    UART3_IRQn
#define DEBUG_UART_CLK                    RCU_UART3
#define DEBUG_UART_AF                     GPIO_AF_8



void debug_init(uint32_t BAUD);

#endif

