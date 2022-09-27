/*!
    \file  systick.c
    \brief the systick configuration file
    
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

#include "gd32f4xx.h"
#include "systick.h"

volatile static uint32_t delay;

/*!
    \brief      configure systick
    \param[in]  none
    \param[out] none
    \retval     none
*/
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

/*!
    \brief      delay a time in milliseconds
    \param[in]  count: count in milliseconds
    \param[out] none
    \retval     none
*/
void delay_1ms(uint32_t count)
{
    delay = count;

    while(0U != delay){
    }
}

/*!
    \brief      delay decrement
    \param[in]  none
    \param[out] none
    \retval     none
*/
void delay_decrement(void)
{
    if (0U != delay){
        delay--;
    }
}




/**************************************************************************************
 *	\brief 下面的程序，利用利用程序构建更长事件的定时器，和系统时间戳获取。
****************************************************************************************/


#ifdef timeruse 
volatile unsigned long s_TickCnt 		= 0; //set the system time.

//RTC  sturct
volatile  s_Timer_Clock s_Clock = 
{
	.s_Timer_year = 0,
	.s_Timer_month = 0,
	.s_Timer_day = 0,
	.s_Timer_hour = 0,
	.s_Timer_minute = 0,
	.s_Timer_second = 0,
};

//设定一个定时器并启动
void	StartTimer(Timer* tm,unsigned long t)
{
	tm->IntervalTime = t;
	tm->BaseTime = s_TickCnt;
}

//判断一个定时器是否已经溢出
int		TimeOut(Timer* tm)
{
	if((s_TickCnt - tm->BaseTime) >= tm->IntervalTime)
	{
		return 1;		//溢出
	}
	return 0;
}

//剩余时间
int     TimerLeft(Timer* tm)
{
    return tm->BaseTime + tm->IntervalTime - s_TickCnt;
}

//获取tick
unsigned long GetTimeTick(void)
{
	return s_TickCnt;
}

//计算上一次记录至本次时间间隔
uint32_t  Calculatesystick(uint16_t last )
{
    uint16_t current;
    current=GetTimeTick();
    if(current<last)
    {
     return current + (0xFFFF-last);
    }else
    {
     return current-last;
    }
}

//RTC clock.
void clock(void)
{
	s_Clock.s_Timer_second++;
	if(s_Clock.s_Timer_second > 60)
	{
		s_Clock.s_Timer_second -= 60;
		s_Clock.s_Timer_minute++;
		if(s_Clock.s_Timer_minute > 60)
		{
			s_Clock.s_Timer_minute -= 60;
			s_Clock.s_Timer_hour++;
			if(s_Clock.s_Timer_hour > 24)
			{
				s_Clock.s_Timer_hour -= 24;
				s_Clock.s_Timer_day++;
			}
		}
	}
}



#endif 
