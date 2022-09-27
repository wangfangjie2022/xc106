#ifndef __CAT1_PORT_H
#define __CAT1_PORT_H	 

#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>



typedef unsigned long(*pUartGetTimeStamp)(void);
typedef void(*pUartReceiveCallback)(uint8_t* buff,uint16_t len);
static pUartGetTimeStamp g_pfGetTimeStamp = NULL;

#define CAT1_UART_IRQHandler             USART1_IRQHandler
#define CAT1_UART_IRQ                    USART1_IRQn

#define CAT1_UART                        USART1
#define CAT1_UART_CLK                    RCU_USART1

#define CAT1_UART_TX_PIN                 GPIO_PIN_2
#define CAT1_UART_RX_PIN                 GPIO_PIN_3

#define CAT1_UART_GPIO_PORT              GPIOA
#define CAT1_UART_GPIO_CLK            	 RCU_GPIOA
#define CAT1_UART_AF                     GPIO_AF_7




//电源开关
#define CAT1_PWRKEY_PORT 	(GPIOA)
#define CAT1_PWRKEY_PIN 	(GPIO_PIN_7)

#define CAT1_RESET_PORT 	(GPIOA)
#define CAT1_RESET_PIN 		(GPIO_PIN_5)

#define CAT1_WAKEUP_PORT 	(GPIOC)
#define CAT1_WAKEUP_PIN 	(GPIO_PIN_4)

//控制开关接口
#define CAT1_PWEKER_H()    	(gpio_bit_reset(CAT1_PWRKEY_PORT,CAT1_PWRKEY_PIN))
#define CAT1_PWEKER_L()    	(gpio_bit_set(CAT1_PWRKEY_PORT,CAT1_PWRKEY_PIN))

#define CAT1_RESET_H()    	(gpio_bit_reset(CAT1_RESET_PORT,CAT1_RESET_PIN))
#define CAT1_RESET_L()    	(gpio_bit_set(CAT1_RESET_PORT,CAT1_RESET_PIN))

#define CAT1_WAKEUP_H()    	(gpio_bit_reset(CAT1_WAKEUP_PORT,CAT1_WAKEUP_PIN))
#define CAT1_WAKEUP_L()    	(gpio_bit_set(CAT1_WAKEUP_PORT,CAT1_WAKEUP_PIN))

void cat1_init(void);

//模块重启
void cat1_pwrker(void);
//模块复位
void cat1_reset(void);
//唤醒模块
void cat1_wakeup(void);
//IO口初始化
void cat1_io_init(void);

void Cat1_UartInit(uint32_t baud, uint8_t* receiveBuff, uint16_t maxLen,uint32_t interval, pUartReceiveCallback recFunc, pUartGetTimeStamp timeFunc);
uint16_t Cat1_UartRead(bool needCopy,uint8_t* buff,uint16_t len);
bool Cat1_UartSend(const char* buff,int len);

#endif

