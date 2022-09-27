//#include "led.h"
//#include "sys.h"
//#include "timer1.h"
#include "header.h"

u8 led_disp_mode=0;

void LED_Init(void)
{
//		rcu_periph_clock_enable(RCU_GPIOB); 
//		rcu_periph_clock_enable(RCU_GPIOA); 
//		gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_Pin_8);	
//		gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_Pin_6);	
//		SYS_LED=LED_OFF;
//		RELAY=RELAY_CLOSE;
}



void all_unuse_gpio_set(void)
{
//	rcu_periph_clock_enable(RCU_GPIOA); 
//	rcu_periph_clock_enable(RCU_GPIOB); 
//	rcu_periph_clock_enable(RCU_GPIOC); 
//	rcu_periph_clock_enable(RCU_GPIOD); 
//	rcu_periph_clock_enable(RCU_GPIOE); 
//	
//	gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);
//	gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);
//	gpio_init(GPIOD, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_ALL);
//	gpio_bit_reset(GPIOC,GPIO_PIN_ALL);
//	gpio_bit_reset(GPIOD,GPIO_PIN_ALL);

}

//#define ALL_OK 0
//#define CAT1_REGING 1 //cat1 registing net
//#define CAT1_REG_OK 2
//#define CAT1_REG_FAIL_NO_SIM 3
//#define CAT1_REG_FAIL_COMMFAIL 4
//#define MQTT_CONNECTING 5
//#define MQTT_CONNECT_OK 6
//#define MQTT_CONNECT_FAIL 7
//#define MQTT_HEART_BEAT 8
#define CAT1_REGING_LED_TIME_UP 1
#define MQTT_CONNECTING_TIME_UP 5
#define MQTT_HEART_BEAT_TIME_UP 10

void led_disp(u8 mode)
{
	u16 i=0;
	switch(mode)
	{
		case CAT1_REGING:
			if(led_disp_tick++>=CAT1_REGING_LED_TIME_UP)
			{
				led_disp_tick=0;
				SYS_LED=!SYS_LED;
			}
			break;
		case MQTT_CONNECTING:
			if(led_disp_tick++>=MQTT_CONNECTING_TIME_UP)
			{
				led_disp_tick=0;
				SYS_LED=!SYS_LED;
			}
			break;
		case CAT1_REG_OK:
		case MQTT_CONNECT_OK:
			SYS_LED=LED_ON;
			delay_ms(1000);
			SYS_LED=LED_OFF;
			delay_ms(100);
			SYS_LED=LED_ON;
			delay_ms(1000);
			SYS_LED=LED_OFF;
			delay_ms(100);
			SYS_LED=LED_ON;
			delay_ms(1000);
			SYS_LED=LED_OFF;
			break;
		case CAT1_REG_FAIL_NO_SIM:
			for(i=0;i<100;i++)
			{
				SYS_LED=LED_ON;
				delay_ms(100);
				SYS_LED=LED_OFF;
				delay_ms(1000);				
			}
			break;
		case CAT1_REG_FAIL_COMMFAIL:
			for(i=0;i<100;i++)
			{
				SYS_LED=LED_ON;
				delay_ms(200);
				SYS_LED=LED_OFF;
				delay_ms(500);				
			}
			break;
		case MQTT_CONNECT_FAIL:
			for(i=0;i<100;i++)
			{
				SYS_LED=LED_ON;
				delay_ms(50);
				SYS_LED=LED_OFF;
				delay_ms(500);				
			}
			break;
			
	}	
}

