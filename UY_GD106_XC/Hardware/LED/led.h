#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define SYS_LED PBout(8)
#define RELAY PAout(6)

#define LED_ON 1
#define LED_OFF 0

#define RELAY_OPEN 1
#define RELAY_CLOSE 0


void LED_Init(void);//≥ı ºªØ
void led_disp(u8 mode);

#define ALL_OK 0
#define CAT1_REGING 1 //cat1 registing net
#define CAT1_REG_OK 2
#define CAT1_REG_FAIL_NO_SIM 3
#define CAT1_REG_FAIL_COMMFAIL 4
#define MQTT_CONNECTING 5
#define MQTT_CONNECT_OK 6
#define MQTT_CONNECT_FAIL 7
#define MQTT_HEART_BEAT 8

extern u8 led_disp_mode;
	 				    
#endif
