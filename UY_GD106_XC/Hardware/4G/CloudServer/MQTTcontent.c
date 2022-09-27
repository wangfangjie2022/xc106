#include "header.h"

//extern Modbusworkstate sta;
void auth_mqtt_url(char* dest)
{
//	strcat(dest,"124.71.164.105");
//	strcat(dest,"121.41.77.2");//20220720
		strcat(dest,"dici.shengzhouchengtou.com");
//	strcat(dest,"broker-cn.emqx.io");
}

int32_t ota_info_subOne(char** dest)
{
int totalSize = strlen("/firmware/")+sizeof(ChipIDstring)+4;
//    *dest = (char*)malloc(totalSize);
	static char topic_1[100];
	memset(topic_1,0,100);
	*dest = topic_1;
    if(!*dest)
	{
		SEGGER_RTT_printf2("ota_info_sub,ÉêÇë¶¯Ì¬ÄÚ´æ¿Õ¼äÊ§°Ü£¡\r\n");
		return 0;
	}
	snprintf(*dest,50,"/firmware/%s",ChipIDstring+12);
//    snprintf(*dest,totalSize,"firmware/%.12s",ChipIDstring+12);
    return totalSize-1;
}


int32_t ota_info_subAll(char** dest)
{
	int totalSize = strlen("/firmware/all")+1;
	static char topic_1[100];
	memset(topic_1,0,100);
	*dest = topic_1;
  if(!*dest)
	{
		SEGGER_RTT_printf2("ota_info_sub,ÉêÇë¶¯Ì¬ÄÚ´æ¿Õ¼äÊ§°Ü£¡\r\n");
		return 0;
	}
	snprintf(*dest,50,"/firmware/all");
  return totalSize-1;
}

		
int32_t reset_subDTU(char** dest)
{
	static char topic_1[100];
	memset(topic_1,0,100);
	*dest = topic_1;
  if(!*dest)
	{
		SEGGER_RTT_printf2("reset_subDTU,ÉêÇë¶¯Ì¬ÄÚ´æ¿Õ¼äÊ§°Ü£¡\r\n");
		return 1;
	}
	snprintf(*dest,100,"/reset_device/%s",ChipIDstring+12);
  return 0;
}

		
int32_t info_subVolt(char** dest)
{
int totalSize = strlen("/order_device/")+13;
//    *dest = (char*)malloc(totalSize);
	static char topic_1[100];
	memset(topic_1,0,100);
	*dest = topic_1;
    if(!*dest)
	{
		SEGGER_RTT_printf2("ota_info_sub,ÉêÇë¶¯Ì¬ÄÚ´æ¿Õ¼äÊ§°Ü£¡\r\n");
		return 0;
	}
	snprintf(*dest,50,"/order_device/%s",ChipIDstring+12);
//    snprintf(*dest,totalSize,"firmware/%.12s",ChipIDstring+12);
    return totalSize-1;
}


uint8_t FlagReboot;
uint8_t FlagDTU;
void info_subVoltCallback(void* arg,const char* data,int len)
{
	char *strcat=NULL;
	char *strcat1=NULL;

	strcat=strstr(data,"isDtu");
	strcat1=strstr(strcat,"ture");
	
	if(strcat1!=NULL)
	{
//		dtu_work_mode=Modbus_Master;
	}
	else
	{
//		dtu_work_mode=Modbus_Slave;	
	}
	
	strcat=strstr(data,"isReboot");
	strcat1=strstr(strcat,"ture");
	if(strcat1!=NULL)
	{
		FlagReboot=1;
	}
	
	strcat=strstr(data,"isRecovered");
	strcat1=strstr(strcat,"ture");
	if(strcat1!=NULL)
	{
		fmc_erase_pages(FLASH_BASE+0xF400,1);
		FlagReboot=1;
	}
	
}



void reset_subDTUCallback(void* arg,const char* data,int len)
{
	SEGGER_RTT_printf2("ready reboot!!!!!\r\n");
	delay_ms(1000);
	sys_reboot();

}	
