#include "wtn.h"
#include "delay.h"
#include "sys.h"

u8 speak_mute_ctrl=0;
u32 time3_freq=0;


void WTN_Init(void)
{
		rcu_periph_clock_enable(WTN_DATA_URC);
	
		gpio_mode_set(WTN_DATA_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, WTN_DATA_PIN);
    gpio_output_options_set(WTN_DATA_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,WTN_DATA_PIN);
	
		gpio_mode_set(SPEEK_MUTE_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, SPEEK_MUTE_PIN);
    gpio_output_options_set(SPEEK_MUTE_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,SPEEK_MUTE_PIN);

		gpio_bit_reset(WTN_DATA_PORT,WTN_DATA_PIN|SPEEK_MUTE_PIN);	
	
}

void play_wtn_audio(u8 index)
{
	Line_1A_WTN6(0xef);
	delay_ms(3);
	Line_1A_WTN6(index);		
}


/*-------------------------------------- 
;模块名称:Line_1A_WTN6(UC8 DDATA) ;
功 能:实现一线串口通信函数 ;
入 参: DDATA 为发送数据 ;
出 参: ;P_DATA 为数据口 ;
-------------------------------------*/ 
void Line_1A_WTN6( u8 DDATA) 
{ 
	u8 S_DATA,j; 
	u8 B_DATA;
	S_DATA = DDATA; 	
	WTN_DATA = 0; 
	delay_ms(5); //延时 5ms 
	B_DATA = S_DATA&0X01; 
	for(j=0;j<8;j++) 
	{ 
		if(B_DATA == 1) 
			{ 
				WTN_DATA = 1;
				delay_us(600); //延时 600us 
				WTN_DATA = 0; 
				delay_us(200); //延时 200us 
			}
			else 
			{ 
				WTN_DATA = 1; 
				delay_us(200); //延时 200us 
				WTN_DATA = 0; 
				delay_us(600); //延时 600us 
			}
			S_DATA = S_DATA>>1; 
			B_DATA = S_DATA&0X01; 
	}
	WTN_DATA = 1; 
}


void audio_play_ctrl(u8 mode)
{
			SPEEK_MUTE=MUTE_NO;
			play_wtn_audio(mode);
//			SPEEK_MUTE=MUTE_YES;
}
