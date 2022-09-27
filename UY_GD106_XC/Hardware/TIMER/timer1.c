#include "header.h"


#define HEART_BEAT_TIME 600  //600*0.05S=30S
#define WATCH_DOG_RESET 3*60*20 //2����
uint8_t Netbeatsendflag=0;

u16 led_disp_tick=0;
u16 led_disp_timer=0;
u32 watch_time_tick=0;//���Ź� ������ ÿ��0.1�����һ��
uint16_t timer0tick=0; 
u16 gps_uart_rx_timeout=0;
u8 gps_uart_begin=GPS_UART_BEGIN_NO;


/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer1_config(void)
{
    
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 11999;//(0~5999)  //120M/12000=10K
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 499;//(0~9999)   //10K/500   0.05 Second every IRQ
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);
		
    /* enable the TIMER interrupt */
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);

    /* TIMER1 counter enable */
//    timer_enable(TIMER1);
}

void time1_enable(void)
{
	timer_enable(TIMER1);	
}

void time1_disable(void)
{
	timer_disable(TIMER1);
}

 /*!
    \brief      this function handles TIMER1 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER1_IRQHandler(void)   //0.05S ����һ��
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif	
		u8 led_disp_mode1=led_disp_mode;//��ֹ��ָʾ����˸��ʱ���л���ָʾ��״̬
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP))
		{
		//28-30��֮�䷢�������������ݰ���
				if(timer0tick>HEART_BEAT_TIME-40&&timer0tick<HEART_BEAT_TIME)
				{
						Netbeatsendflag=1;
						timer0tick=0;
				}
				timer0tick++;
				if((led_disp_mode1==CAT1_REGING)||(led_disp_mode1==MQTT_CONNECTING))
				{
					led_disp(led_disp_mode1);				
				}
				if(watch_time_tick++<WATCH_DOG_RESET)
				{
					feed_dog();								
				}
				if(gps_uart_begin==GPS_UART_BEGIN_YES)//gps ���ڿ�ʼ��������
				{
					if(gps_uart_rx_timeout++>GPS_UART_RX_TIMEOUT)//���ڽ������ݳ�ʱ
					{
						gps_uart_begin=GPS_UART_BEGIN_NO;
						gps_uart_rx_timeout=0;
						GPS_RX_STA|=1<<15;	//��ǽ������
					}
				}
				/* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);	
    }
#if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
#endif		
}
