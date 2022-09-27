/*!
    \file  systick.h
    \brief the header file of systick
    
    \version 2016-08-15, V1.0.0, demo for GD32F4xx
    \version 2018-12-12, V2.0.0, demo for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

/* configure systick */
void systick_config(void);
/* delay a time in milliseconds */
void delay_1ms(uint32_t count);
/* delay decrement */
void delay_decrement(void);


#define timeruse 1

#ifdef timeruse
/*!
    \brief      the handle of the systick timer.
    \param[in]  none
    \param[out] none
    \retval     none
*/
__weak void systimetick_Handle(void);



extern volatile unsigned long s_TickCnt;


//define the a struct to the RTC.
typedef volatile struct 
{
	unsigned int s_Timer_year;
	unsigned char s_Timer_month;
	unsigned char s_Timer_day;
	unsigned char s_Timer_hour;
	unsigned char s_Timer_minute;
	unsigned char s_Timer_second;

}s_Timer_Clock;

extern s_Timer_Clock s_Clock;

typedef struct
{
	unsigned long IntervalTime;		//想要定时的时间间隔
	unsigned long BaseTime;			//定时开始的系统时基
}Timer;

//设定一个定时器并启动
void	StartTimer(Timer* tm,unsigned long t);

//判断一个定时器是否已经溢出
int		TimeOut(Timer* tm);

//剩余时间
int     TimerLeft(Timer* tm);

//获取tick
unsigned long GetTimeTick(void);

/*!
    \brief      Calculate systick between the tick and  return get a time .
    \param[in]  none
    \param[out] none
    \retval     none
*/
uint32_t  Calculatesystick(uint16_t last );
#endif



#endif /* SYSTICK_H */



