#ifndef __TIMER1_H__
#define __TIMER1_H__

#include "gd32f4xx.h"
#include <stdio.h>
#include "sys.h"

#define GPS_UART_BEGIN_NO 0
#define GPS_UART_BEGIN_YES 1
#define GPS_UART_RX_TIMEOUT 2 //2*0.05S=0.1S

void timer1_config(void);

void time1_enable(void);

void time1_disable(void);
extern u16 led_disp_tick;
extern u32 watch_time_tick;
extern uint16_t timer0tick;
extern u16 gps_uart_rx_timeout;
extern u8 gps_uart_begin;
#endif 

