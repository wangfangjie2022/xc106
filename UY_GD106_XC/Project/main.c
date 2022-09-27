
#include "header.h"
#include "mytask.h"

void fisrt_publish(char *cJSONoutbuffer);

#define INFLASH_START_ADDR      ((uint32_t)0x08000000)  // STM32 内部FLASH的起始地址
#define INFLASH_VTOR_OFFSET     ((uint32_t)0x00004000)  // APP向量表的偏移地址，与APP的起始地址保持一致

extern const char *Version;
ServerCertification Certification;

#define BIANYAQI_STATUE_CHANGE_NO 0
#define BIANYAQI_STATUE_CHANGE_YES 1
u8 check_bianyaqi_statue_change(u8 var);
void get_device_statue_loop(char *cJSONoutbuffer);
//void set_jiami(void)
//{
////	 erase_jiami_data();
//	if(!check_jiami_flag())
//	{
//		StmWriteUniqueIDFlag();
//		StmWriteUniqueID();	
//		gd_set_read_protect();
//	}
//	while(!(StmCheckUniqueID()))
//	{
//		LED_JK_RED=!LED_JK_RED;
//		LED_XC_RED=!LED_XC_RED;
//		delay_ms(100);
//	}		
//}

bool FactoryFlag=1;	//1 is not set ,0 is set.

void test_modbus(void);

//bool feeddog=1;
void init_fun(void)
{
	SEGGER_RTT_Init();//RTT INIT
//	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	system_init();				//@ system init.
	FLASH_init();
//	debug_init(4800);	
	LED_Init();
	RTC_Timer_Init();
	timer1_config();//心跳定时的初始化	
	saoma_uart_init(9600);
	camera_init();
	WTN_Init();
	gps_init(9600);//GPS初始化
//	while(1)
//	{
//		gps_loop();
//		SEGGER_RTT_printf2("\r\n--------------------------------------\r\n");
//		delay_ms(1000);
//		
//	}
	audio_play_ctrl(SPEACH_MODE_QDD);
	delay_ms(500);
	Watchdog_init(10); //set watchdog 10s
	SEGGER_RTT_printf2("\r\n>Run_Log begin.\r\n");
	mode_imei=(char*)malloc(IMEI_MAX_SIZE);
	card_ccid=(char*)malloc(CCID_MAX_SIZE);
	SEGGER_RTT_printf2("main start\r\n");
	SEGGER_RTT_printf2(">version:%s\r\n",Version);
	SEGGER_RTT_printf2(">mcu_id:%s\r\n",ChipIDstring+12);
}



extern uint8_t FlagReboot;
extern uint8_t FlagDTU;

//============================================================main=========================
int main()
{
	#ifdef RUN_IAP_MODE
	nvic_vector_table_set(NVIC_VECTTAB_FLASH,IAP_SIZE);
	__set_PRIMASK(0);//wxf 20200428 这句话需要增加，否则各种中断无法进行 
#endif

	init_fun();		
	time1_enable();
	watchdog_enable();
	watch_time_tick=0;
//	delay_ms(20);	
	myfatfs_init();
	led_disp_mode=CAT1_REGING;
	watch_time_tick=0;
	
	//4G INIT
	NetworkInit();
	NetworkStart();
	memcpy(ChipIDstring+12,mode_imei+3,12);//20220626 将CPU ID替换为IMEI号码，方便后续统计设备，记录设备
	watch_time_tick=0;
	led_disp_mode=CAT1_REG_OK;
	led_disp(led_disp_mode);
	check_if_first_boot();
	watch_time_tick=0;

	/*define the parameters of the network modbule.*/
	SEGGER_RTT_printf2(">Initialization is completed.\r\n");
	///////////////////////////////////////////////////////////////////////////////////////////////
	OSInit(); 					//UCOS初始化
	SEGGER_RTT_printf2("OSInit finish.\r\n");
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO);
	OSStart(); //开启UCOS

}




/////////////////////////////////////////////start任务////////////////////////////////////////////////////
void start_task(void *pdata)
{
	SEGGER_RTT_printf2("start_task.\r\n");
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//初始化统计任务
	SEGGER_RTT_printf2("OSStatInit.\r\n");
	OS_ENTER_CRITICAL();  	//关中断
	SEGGER_RTT_printf2("OS_ENTER_CRITICAL.\r\n");
	OSTaskCreate(mqtt_task,(void*)0,(OS_STK*)&MQTT_TASK_STK[MQTT_TASK_SIZE-1],MQTT_TASK_PRIO);	
	OSTaskCreate(saoma_task,(void*)0,(OS_STK*)&SAOMA_TASK_STK[SAOMA_TASK_SIZE-1],SAOMA_TASK_PRIO);	
	OSTaskCreate(upload_file_task,(void*)0,(OS_STK*)&UPLOAD_FILE_TASK_STK[UPLOAD_FILE_TASK_SIZE-1],UPLOAD_FILE_TASK_PRIO);	
	OSTaskCreate(watchdog_task,(void*)0,(OS_STK*)&WATCHDOG_TASK_STK[WATCHDOG_TASK_SIZE-1],WATCHDOG_TASK_PRIO);	
	OSTaskSuspend(OS_PRIO_SELF); //挂起start_task任务
	OS_EXIT_CRITICAL();  //开中断
	
}
//u8 http_upload_file=0;
////////////////////////////////////////////led任务////////////////////////////////////////
void watchdog_task(void *pdata)
{
	u16 gps_timeup=0;
	SEGGER_RTT_printf2("----watchdog_task------\r\n");
	gps_mqtt_payload=(char*)malloc(GPS_MQTT_PAYLOAD_LENGTH);
	while(1)
	{		
		if(gps_timeup++>10)
		{
			gps_timeup=0;
			gps_loop();
		}
		watch_time_tick=0;
//		SEGGER_RTT_printf2("----watchdog_task------\r\n");		
		OSTimeDlyHMSM(0,0,0,1000);  //延时500ms
 	}
}
 

