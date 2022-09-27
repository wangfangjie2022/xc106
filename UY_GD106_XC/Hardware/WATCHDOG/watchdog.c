#include "watchdog.h"
#include "sys.h"


void Watchdog_init(u8 reset_time)   //unit S  max 26S
{
		u16 tmp=0;
		tmp=(u16)(reset_time*1000/8);
		/* confiure FWDGT counter clock: 40KHz(IRC40K) / 256 = 156Hz */ //8mS
    fwdgt_config(tmp,FWDGT_PSC_DIV256);   //  tmp max 4095    
}

void feed_dog(void)
{
	 fwdgt_counter_reload();	
}

void watchdog_enable(void)
{
	fwdgt_enable();
}






