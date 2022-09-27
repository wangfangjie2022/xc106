#ifndef __SAOMA_H
#define __SAOMA_H	 

#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "sys.h"


#define SAOMA_UART_IRQHandler             USART2_IRQHandler
#define SAOMA_UART_IRQ                    USART2_IRQn

#define SAOMA_UART                        USART2
#define SAOMA_UART_CLK                    RCU_USART2

#define SAOMA_UART_TX_PIN                 GPIO_PIN_10
#define SAOMA_UART_RX_PIN                 GPIO_PIN_5

#define SAOMA_UART_GPIO_PORT              GPIOC
#define SAOMA_UART_GPIO_CLK            	 RCU_GPIOC
#define SAOMA_UART_AF                     GPIO_AF_7


#define SAOMA_BUFFER_SIZE 512

#define SAOMA_IDLE 0  //扫码空闲
#define SAOMA_GETDATA 1//扫码已经获得有效数据
#define SAOMA_DATAPROCESS 2//扫码的数据已经被处理
#define SAOMA_CAMERA_WORKING 3
#define SAOMA_CAMERA_IDEL 4
extern u8 saoma_statue;

extern u8 saoma_buffer[SAOMA_BUFFER_SIZE];
uint16_t Saoma_UartRead(bool needCopy,uint8_t* buff,uint16_t len);
bool Saoma_UartSend(const char* buff,int len);
void saoma_uart_init(uint32_t BAUD);
void create_saoma_publish_data(char* saomaid,char * saomatime);

#endif

