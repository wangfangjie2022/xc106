#ifndef __MYTASK_H
#define __MYTASK_H	 

#include "includes.h"



/*
#define LED_TASK_PRIO		16 //�忴�Ź��̣߳�ָʾ����˸�߳�
#define FTP_TASK_PRIO		10 
#define FTP_POLL_TASK_PRIO		15
#define GET_NUM_TASK_PRIO		8
#define DISPLAY_TASK_PRIO	9
#define HEAT_BEAT_TASK_PRIO		12  //�����������߳�
#define NET_PROCESS_TASK_PRIO		14 //?
#define PICK_DETECT_TASK_PRIO		4  //wav_recorder2(0)  ¼���߳�
#define START_TASK_PRIO		11
#define LWIP_DHCP_TASK_PRIO  17
#define BOHAO_DETECT_TASK_PRIO		13    //wxf change 20200107
#define TCPCLIENT_PRIO		6
#define TCPIP_THREAD_PRIO		5	//�����ں���������ȼ�Ϊ5
#define DEFAULT_THREAD_PRIO		2


*/

//==========================================================================
//START����
//�������ȼ�
#define START_TASK_PRIO		12
//�����ջ��С
#define START_STK_SIZE		512
//�����ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata); 


//LED����
//�������ȼ�
#define MQTT_TASK_PRIO		10
//�����ջ��С
#define MQTT_TASK_SIZE		2048
//#define LED_STK_SIZE		2048
//�����ջ
OS_STK	MQTT_TASK_STK[MQTT_TASK_SIZE];
//������
void mqtt_task(void *pdata);  

//LED����
//�������ȼ�
#define WATCHDOG_TASK_PRIO		18
//�����ջ��С
#define WATCHDOG_TASK_SIZE		128
//�����ջ
__align(8) OS_STK	WATCHDOG_TASK_STK[WATCHDOG_TASK_SIZE];
//������
void watchdog_task(void *pdata); 

//SAOMA����
//�������ȼ�
#define SAOMA_TASK_PRIO		7
//�����ջ��С
#define SAOMA_TASK_SIZE		512
//#define LED_STK_SIZE		2048
//�����ջ
OS_STK	SAOMA_TASK_STK[SAOMA_TASK_SIZE];
//������
void saoma_task(void *pdata);  


//#define CAMERA_TASK_PRIO		11
////�����ջ��С
//#define CAMERA_TASK_SIZE		512
////#define LED_STK_SIZE		2048
////�����ջ
//OS_STK	CAMERA_TASK_STK[SAOMA_TASK_SIZE];
//������
void camera_task(void *pdata);  

#define UPLOAD_FILE_TASK_PRIO		11
//�����ջ��С
#define UPLOAD_FILE_TASK_SIZE		256
//#define LED_STK_SIZE		2048
//�����ջ
OS_STK	UPLOAD_FILE_TASK_STK[UPLOAD_FILE_TASK_SIZE];
//������
void upload_file_task(void *pdata);  



#endif

