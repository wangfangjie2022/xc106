#ifndef __WATCHDOG_H
#define __WATCHDOG_H	 
#include "sys.h"


void Watchdog_init(u8 reset_time);   //unit S
void feed_dog(void);
void watchdog_enable(void);
		 				    
#endif
