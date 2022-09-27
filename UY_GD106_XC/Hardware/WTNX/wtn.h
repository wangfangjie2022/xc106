#ifndef __WTN_H
#define __WTN_H	 
#include "sys.h"


#define SPEEK_MUTE PCout(3)// PB5
#define MUTE_YES 1
#define MUTE_NO 0

#define WTN_DATA PCout(2)

#define WTN_DATA_PORT GPIOC
#define WTN_DATA_URC RCU_GPIOC
#define WTN_DATA_PIN GPIO_PIN_2

#define SPEEK_MUTE_PORT GPIOC
#define SPEEK_MUTE_RCU RCU_GPIOC
#define SPEEK_MUTE_PIN GPIO_PIN_3


#define SPEACH_MODE_MUTE 0  	//����
#define SPEACH_MODE_SAOMA 1  	//ɨ��
#define SPEACH_MODE_CEWEN 2 	//����
#define SPEACH_MODE_WANGLUO 3 	//����
#define SPEACH_MODE_JKM 4  		//������
#define SPEACH_MODE_XCM 5 		//�г���
#define SPEACH_MODE_TWZC 6  	//��������	
#define SPEACH_MODE_TWYC 7 		//�����쳣
#define SPEACH_MODE_QCW 8  		//�����
#define SPEACH_MODE_QSM 9  		//��ɨ��
#define SPEACH_MODE_QTX 10  	//��ͨ��
#define SPEACH_MODE_QDD 11  	//��ȴ�
#define SPEACH_MODE_DI 12  		//��
#define SPEACH_MODE_DIDIDI 13  		//�εε�
#define SPEACH_MODE_RED_MA 14  		//����
#define SPEACH_MODE_GREEN_MA 15  	//����
#define SPEACH_MODE_YELLOW_MA 16  	//����
#define SPEACH_MODE_CONNECT 17  	//������
#define SPEACH_MODE_DISCONNECT 18  	//�ѶϿ�
#define SPEACH_MODE_NORMAL 19  		//����
#define SPEACH_MODE_UNNORMAL 20  	//�쳣
#define SPEACH_MODE_WRONG 21		//����
#define SPEACH_MODE_CHENGGONG 22	//�ɹ�
#define SPEACH_MODE_SHIBAI 23		//�쳣
#define SPEACH_MODE_PAIZHAO 24		//����

#define MAX_LANGUAGE 6

#define	 GET_WTN_BYUS()			(gpio_input_bit_get(GPIOA, GPIO_Pin_12))

void WTN_Init(void);//��ʼ��
void Line_1A_WTN6( u8 DDATA) ;
void audio_play_ctrl(u8 mode);
extern u8 audio_index;
void play_wtn_audio(u8 index);
		 				    
#endif
