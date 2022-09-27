#ifndef __MYTASK_H
#define __MYTASK_H	 

#include "includes.h"



/*
#define LED_TASK_PRIO		16 //清看门狗线程，指示灯闪烁线程
#define FTP_TASK_PRIO		10 
#define FTP_POLL_TASK_PRIO		15
#define GET_NUM_TASK_PRIO		8
#define DISPLAY_TASK_PRIO	9
#define HEAT_BEAT_TASK_PRIO		12  //发送心跳包线程
#define NET_PROCESS_TASK_PRIO		14 //?
#define PICK_DETECT_TASK_PRIO		4  //wav_recorder2(0)  录音线程
#define START_TASK_PRIO		11
#define LWIP_DHCP_TASK_PRIO  17
#define BOHAO_DETECT_TASK_PRIO		13    //wxf change 20200107
#define TCPCLIENT_PRIO		6
#define TCPIP_THREAD_PRIO		5	//定义内核任务的优先级为5
#define DEFAULT_THREAD_PRIO		2


*/

//==========================================================================
//START任务
//任务优先级
#define START_TASK_PRIO		12
//任务堆栈大小
#define START_STK_SIZE		512
//任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata); 


//LED任务
//任务优先级
#define MQTT_TASK_PRIO		10
//任务堆栈大小
#define MQTT_TASK_SIZE		2048
//#define LED_STK_SIZE		2048
//任务堆栈
OS_STK	MQTT_TASK_STK[MQTT_TASK_SIZE];
//任务函数
void mqtt_task(void *pdata);  

//LED任务
//任务优先级
#define WATCHDOG_TASK_PRIO		18
//任务堆栈大小
#define WATCHDOG_TASK_SIZE		128
//任务堆栈
__align(8) OS_STK	WATCHDOG_TASK_STK[WATCHDOG_TASK_SIZE];
//任务函数
void watchdog_task(void *pdata); 

//SAOMA任务
//任务优先级
#define SAOMA_TASK_PRIO		7
//任务堆栈大小
#define SAOMA_TASK_SIZE		512
//#define LED_STK_SIZE		2048
//任务堆栈
OS_STK	SAOMA_TASK_STK[SAOMA_TASK_SIZE];
//任务函数
void saoma_task(void *pdata);  


//#define CAMERA_TASK_PRIO		11
////任务堆栈大小
//#define CAMERA_TASK_SIZE		512
////#define LED_STK_SIZE		2048
////任务堆栈
//OS_STK	CAMERA_TASK_STK[SAOMA_TASK_SIZE];
//任务函数
void camera_task(void *pdata);  

#define UPLOAD_FILE_TASK_PRIO		11
//任务堆栈大小
#define UPLOAD_FILE_TASK_SIZE		256
//#define LED_STK_SIZE		2048
//任务堆栈
OS_STK	UPLOAD_FILE_TASK_STK[UPLOAD_FILE_TASK_SIZE];
//任务函数
void upload_file_task(void *pdata);  



#endif

