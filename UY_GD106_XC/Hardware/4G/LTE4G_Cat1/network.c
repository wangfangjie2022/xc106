#include "header.h"

MqttObject mqttobject;

static bool s_bIsNetstart = false;
static NETWORK_STATUS   s_eNetworkStatus = NETWORK_IDLE;
static NETWORK_STATUS   s_eNetworkLastStatus = NETWORK_IDLE;

static Timer s_tNetworkTimer;
static int s_ucPort = 3883;
static int s_ucVersion = 4;
static int s_ucKeepalive = 120;  //second keep alive.

const char MQTTUserName[]="admin";
const char MQTTPassWord[]="bearer/";

char mqtt_passware[64]={0};

extern uint8_t Data_cache[30][5*2];
extern ServerCertification Certification;


volatile uint32_t  onlineAddress;
extern uint8_t onlinenum;

extern uint8_t Netbeatsendflag;
extern uint8_t Modbussendflag;

extern uint8_t Frimversion[4];
//启动网络
void NetworkStart(void)
{	
	SEGGER_RTT_printf2("NetworkStart\r\n");
	MODULE_ERRPO err;
	if(!s_bIsNetstart)
	{
		at_parser_init(&g_ATModule.atConfig);		//初始化串口
		
		err = module_networking(&g_ATModule);		//开机、等待模块连接上网络
		if(err == MODULE_ERR_NONE)		
			s_bIsNetstart = true;
		else
		{
			SEGGER_RTT_printf2("NetworkStart fail! rebooting\r\n");
			sys_reboot();
		}
	}	
	SEGGER_RTT_printf2("NetworkStart finish\r\n");
}

//网络连接失败处理函数
static void NetworkFailHandle(void)
{
		SEGGER_RTT_printf2("NetworkFailHandle\r\n");
    s_eNetworkLastStatus = s_eNetworkStatus;
    s_eNetworkStatus = NETWORK_WAIT;		//等待重连状态
    StartTimer(&s_tNetworkTimer,10*1000);	
}

//MQTT连接意外断开回调函数(module 4G模块层)
static void MqttMqttDisconnectCallback(int id)
{
		SEGGER_RTT_printf2("MqttMqttDisconnectCallback\r\n");
		s_eNetworkLastStatus = NETWORK_CONNECTING;
    s_eNetworkStatus = NETWORK_WAIT;		//等待重连状态
    StartTimer(&s_tNetworkTimer,5*1000);//20220918
}

void MqttMqttDisconnectCallback2(void)
{
		SEGGER_RTT_printf2("MqttMqttDisconnectCallback2\r\n");
//		s_eNetworkLastStatus = NETWORK_CONNECTING;
    s_eNetworkStatus = NETWORK_CONNECTING;		//等待重连状态
//    StartTimer(&s_tNetworkTimer,5*1000);//20220918
}


//网络初始化
void NetworkInit(void)
{
	SEGGER_RTT_printf2("NetworkInit\r\n");
	ATModuleInit(GetTimeTick);
	module_set_mqtt_disconnect_callback(&g_ATModule, MqttMqttDisconnectCallback);	//设置重连
	SEGGER_RTT_printf2("NetworkInit finish\r\n");
}

uint8_t Property_start = 0;


uint8_t compareversion(char *cha)
{
	uint8_t high=0;
	uint8_t middle1=0;
	uint8_t middle2=0;
	uint8_t low=0;
	*cha-=48;
		cha++;
		high=*cha-48;
		cha++;
		middle1=*cha-48;
		cha++;
		middle2=*cha-48;
		cha++;
		low=*cha-48;	
			if(Frimversion[0]<=high)
			{
				return 1;
			}
			if(Frimversion[1]<=middle1&&Frimversion[0]==high)
			{
				return 1;					
			}
			if(Frimversion[2]<=middle2&&Frimversion[1]==middle1&&Frimversion[0]==high)
			{
				return 1;				
			}
			if(Frimversion[3]<=low&&Frimversion[2]==middle2&&Frimversion[1]==middle1&&Frimversion[0]==high)
			{			
				return 1;			
			}
 
							return 0;
}


//OTA升级参数订阅回调函数
void OTAParamSubCallback(void* arg,const char* data,int len)
{
	
	
	SEGGER_RTT_printf2("2.OTAParamSubCallback Publish ACK!======================\r\n");
	char *cat,*cat1;
	uint8_t i;
	uint8_t var;
	char versionbuf[7];
	char *version=0;//注意
	cat=(char*)malloc(256);
	cat1=(char*)malloc(256);
	version=(char*)malloc(256);
	memset(cat,0,256);
	memset(cat1,0,256);
	memset(version,0,256);
	version=strstr(data,"version");
	memcpy(versionbuf,(uint8_t *)version+11,7);
	i=compareversion(versionbuf);
	SEGGER_RTT_printf2("compareversion i=%d\r\n",i);
	
	audio_play_ctrl(SPEACH_MODE_DIDIDI);
	delay_ms(500);
//	if(i)
	{	
		
		cat=strstr(data,"true");
		SEGGER_RTT_printf2("cat:%s\r\n",cat);
		//@get the upgrade:true.  
		if(cat!=NULL)
		{
				uint32_t urlbuffer,urladdbuff;
				urladdbuff=addr_url-4;
				urlbuffer=0;
				size_t urlsize=0;
				var=0;
				char url[256];
		//@URL max length less than 254.
				cat=strstr(data,"url");
				SEGGER_RTT_printf2("url:%s\r\n",cat);
				cat+=6;
				cat1=strstr(cat,".bin");
				SEGGER_RTT_printf2("bin:%s\r\n",cat1);
				urlsize=cat1-cat+4;
				memset(url,0,sizeof(url));
				memcpy(url,cat,urlsize);
				clea_e2prom(urladdbuff);
				for(var=0;var<urlsize/4;var++)
				{			
							urlbuffer|=url[var*4+3];
							urlbuffer<<=8;
							urlbuffer|=url[var*4+2];
							urlbuffer<<=8;
							urlbuffer|=url[var*4+1];
							urlbuffer<<=8;
							urlbuffer|=url[var*4];
							urladdbuff+=4;
							delay_ms(1);
							lv_write_e2prom(urladdbuff,urlbuffer);
							urlbuffer=0;
				}
				/*The end has '/0'*/
				for(var=1;var<urlsize%4+1;var++)
				{			
							urlbuffer|=url[urlsize-var];
							if(var!=urlsize%4)
							urlbuffer<<=8;
				}
							delay_ms(1);
							lv_write_e2prom(urladdbuff+4,urlbuffer);		
		cat=strstr(data,"md5asum");
		cat+=10;
		uint8_t md5buffer[32];
		uint32_t savebuffer=0,md5addbuff=addr_md5;
		memcpy(md5buffer,cat,32*sizeof(uint8_t));
		for(var=0;var<8;var++)
		{
			Bitmerge(1,md5buffer+var*4,&savebuffer);
			delay_ms(1);
			lv_write_e2prom(md5addbuff+4*var,savebuffer);
		}
		SEGGER_RTT_printf2("ready ota!!!!!\r\n",i);
		sys_reboot();
		}
	}
		//free(data);
	free(cat);
	free(cat1);
	free(version);
}

AT_ERROR PublishGPS(char *gps_payload)
{
	AT_ERROR err;
	char topic[TOPIC_SIZE];
	SEGGER_RTT_printf2("PublishGPS\r\n");

	MQTTMessage  mqttMsg;
	if(mqttMsg.payload==NULL)
	{
		SEGGER_RTT_printf2("mqttMsg.payload==NULL\r\n");
	}
	mqttMsg.dup = 0;
	mqttMsg.qos = QOS0;
	mqttMsg.retained = 0;
	memset(topic,0,TOPIC_SIZE);
	snprintf(topic,TOPIC_SIZE,"/devicegps/%s",ChipIDstring+12);
	mqttMsg.payloadlen = strlen(gps_payload);
	mqttMsg.payload = gps_payload;
	SEGGER_RTT_printf2("mqttMsg.payload:%s\r\n",mqttMsg.payload);
	err=MQTTPublish(&g_ATModule, topic, &mqttMsg);
	SEGGER_RTT_printf2("PublishGPS OVER\r\n");
	return err;
	
}



AT_ERROR PublishBeat(void)
{
	AT_ERROR err;
	char topic[TOPIC_SIZE];
	char payload[PAYLOAD_SIZE]="{\"type\": \"beat\",\"deviceId\":\"";
	SEGGER_RTT_printf2("PublishBeat\r\n");
//	strcat(payload,&Certification.key);
	strcat(payload,ChipIDstring+12);
		
	SEGGER_RTT_printf2("PublishBeat\r\n");
	strcat(payload,"\",}");
	MQTTMessage  mqttMsg;
	if(mqttMsg.payload==NULL)
	{
		SEGGER_RTT_printf2("mqttMsg.payload==NULL\r\n");
	}
	mqttMsg.dup = 0;
	mqttMsg.qos = QOS0;
	mqttMsg.retained = 0;
	memset(topic,0,TOPIC_SIZE);
	snprintf(topic,TOPIC_SIZE,"/device/%s",ChipIDstring+12);
	mqttMsg.payloadlen = strlen(payload);
	mqttMsg.payload = payload;
	SEGGER_RTT_printf2("mqttMsg.payload:%s\r\n",mqttMsg.payload);
	err=MQTTPublish(&g_ATModule, topic, &mqttMsg);
	SEGGER_RTT_printf2("PublishBeat OVER\r\n");
	return err;
	
}

//发布变压器和DTU的消息，版本号，
void PublishData(char* cJSONoutbuffer)
{
	char topic[TOPIC_SIZE];
	char payload[MAX_SEND_MQTT_LENGTH];
	MQTTMessage  mqttMsg;
	SEGGER_RTT_printf2("PublishData\r\n");
	SEGGER_RTT_printf2("data:%s\r\n",cJSONoutbuffer);
	mqttMsg.dup = 0;
	mqttMsg.qos = QOS0;
	mqttMsg.retained = 0;
	memset(payload,0,MAX_SEND_MQTT_LENGTH);
	memset(topic,0,TOPIC_SIZE);
	snprintf(topic,TOPIC_SIZE,"/device/%s",ChipIDstring+12);
	memcpy(payload,cJSONoutbuffer,strlen(cJSONoutbuffer));
	mqttMsg.payloadlen = strlen(payload);
	mqttMsg.payload = payload;
	MQTTPublish(&g_ATModule, topic, &mqttMsg);
}


AT_ERROR PublishData2(char* topic,char* cJSONoutbuffer)
{
	AT_ERROR err;
	MQTTMessage  mqttMsg;
	SEGGER_RTT_printf2("topic:%s\r\n",topic);
//	SEGGER_RTT_printf2("data:%s\r\n",cJSONoutbuffer);
	mqttMsg.dup = 0;
	mqttMsg.qos = QOS0;
	mqttMsg.retained = 0;
	mqttMsg.payloadlen = strlen(cJSONoutbuffer);
	mqttMsg.payload = cJSONoutbuffer;
	err=MQTTPublish(&g_ATModule, topic, &mqttMsg);
	SEGGER_RTT_printf2("PublishData2 err:%d\r\n",err);
	return err;
}


extern uint8_t serverkeyerror;
void NetworkLoop(char *topic,char *cJSONoutbuffer)
{
		AT_ERROR err;
//		SEGGER_RTT_printf2("s_eNetworkStatus:%d,s_bIsNetstart:%d\r\n",s_eNetworkStatus,s_bIsNetstart);
//		SEGGER_RTT_printf2("timer0tick:%d,Netbeatsendflag:%d\r\n",timer0tick,Netbeatsendflag);
    switch(s_eNetworkStatus)
    {
			case NETWORK_IDLE:      //空闲状态
				if(!s_bIsNetstart)
									return;
				if(MODULE_ERR_NONE != module_start(&g_ATModule))    //执行初始化指令
				{
						NetworkFailHandle();
						return;
				}
			case NETWORK_CONNECTING:		//连接操作
				{
						led_disp_mode=MQTT_CONNECTING;
						SEGGER_RTT_printf2("NETWORK_CONNECTING\r\n");
						if(serverkeyerror==1)
						{
							if(topic==NULL)
							{
								SEGGER_RTT_printf2("topic==NULL\r\n");
							}
							char ucMqttUrl[128];
							memset(ucMqttUrl,0,128);
							mqttConnectData mqttConnectArg;
							//生成连接域名
							auth_mqtt_url(ucMqttUrl);
							memcpy(mqttobject.UserName,ChipIDstring+12,strlen(ChipIDstring+12));
							MQTT_password_encode((u8*)mqtt_passware,(u8*)mqttobject.Password);
							mqttConnectArg.username = 	mqttobject.UserName;
							mqttConnectArg.password = 	mqttobject.Password;
							mqttConnectArg.mqtturl  = 	ucMqttUrl;
							mqttConnectArg.devicename = ChipIDstring+12;
							mqttConnectArg.urlport  =   s_ucPort;
							mqttConnectArg.mqttversion = s_ucVersion;
							mqttConnectArg.keepalive = s_ucKeepalive;
							if(MODULE_ERR_NONE != module_mqtt_connect(&g_ATModule,mqttConnectArg))
							{	
								NetworkFailHandle();
								module_mqtt_disconnect(&g_ATModule);
								audio_play_ctrl(SPEACH_MODE_WANGLUO);
								delay_ms(600);
								audio_play_ctrl(SPEACH_MODE_WRONG);
								delay_ms(500);
								SEGGER_RTT_printf2(">Connect MQTT error!rebooting\r\n");
								sys_reboot();		
								return;
							}
							SEGGER_RTT_printf2(">Connect MQTT Success!\r\n");
							
							reset_subDTU(&topic);//DTU：主题: order_device/设备ID  服务器设置DTU的特性，如是否主动上报，是否重启
							MQTTSubscribe(&g_ATModule, topic, QOS1,reset_subDTUCallback,0);
							//订阅 OTA升级参数 主题
							ota_info_subAll(&topic);
							MQTTSubscribe(&g_ATModule, topic, QOS1, OTAParamSubCallback,0);
							ota_info_subOne(&topic);
							MQTTSubscribe(&g_ATModule, topic, QOS1, OTAParamSubCallback,0);
							SEGGER_RTT_printf2(">MQTTSubscribe!\r\n");
							audio_play_ctrl(SPEACH_MODE_WANGLUO);
							delay_ms(600);
							audio_play_ctrl(SPEACH_MODE_CONNECT);
							delay_ms(500);
							s_eNetworkStatus = NETWORK_CONNECTED;		
						}
						else
						{
							SEGGER_RTT_printf2("NETWORK_CONNECTING!\r\n");
						}
					}
				break;
			case NETWORK_CONNECTED:			//保持连接
				
				led_disp_mode=MQTT_CONNECT_OK;
				led_disp(led_disp_mode);
				if(!s_bIsNetstart)  		//断开网络标志
				{	
					module_mqtt_disconnect(&g_ATModule);		//进行MQTT网络断开操作
					s_eNetworkStatus = NETWORK_IDLE;
				}
				//Modbus总线设备在线数量大于0
				
				if(Modbussendflag==MODBUS_HAVE_DATA_SEND_YES)
				{
					Modbussendflag=MODBUS_HAVE_DATA_SEND_NO;
					SEGGER_RTT_printf2("Modbussendflag==MODBUS_HAVE_DATA_SEND_YES\r\n");
					PublishData(cJSONoutbuffer);
				}
				else if(upload_file_para.upload_flag==UPLOAD_FLAG_BEGIN)	
				{				
					MQTTPublish_File(&g_ATModule,upload_file_para.local_filename);

					upload_file_para.upload_flag=UPLOAD_FLAG_FINISH;					
				}
				if(gps_mqtt_data_ready==GPS_MQTT_DATA_READY_YES)
				{
						PublishGPS(gps_mqtt_payload);
						gps_mqtt_data_ready=GPS_MQTT_DATA_READY_NO;
				}
			break;
			case NETWORK_WAIT:		//网络等待
				if(TimeOut(&s_tNetworkTimer))
				{
					s_eNetworkStatus = s_eNetworkLastStatus;		//10秒更新状态
				}
				break;
			default:break;
		}
	module_loop(&g_ATModule);
}


void fisrt_publish(char *cJSONoutbuffer)
{
		u16 tmpstr_len=0,last_tmpstr_offset=0;
//		uint8_t    onlinebuffer[MAX_BIANYAQI_NUM];
		uint32_t   buff=onlineAddress;
//		u8 online_bianyaqi_num=0;
		SEGGER_RTT_printf2("get_device_statue_loop\r\n");
		char *temp_buffer=NULL;//20220620
		temp_buffer=(char*)malloc(MAX_SEND_MQTT_LENGTH/2);//最大字节数据
		
		memset(cJSONoutbuffer,0,MAX_SEND_MQTT_LENGTH);
		
		memset(temp_buffer,0,MAX_SEND_MQTT_LENGTH/2);
		snprintf(temp_buffer,MAX_SEND_MQTT_LENGTH/2,"{\"type\":\"online\",\"deviceId\":\"%s\",\"version\":\"%s\",\"imei\":\"%s\",\"ccid\":\"%s\"}",ChipIDstring+12,Version,mode_imei,card_ccid);
		last_tmpstr_offset=0;
		tmpstr_len=strlen(temp_buffer);
		memcpy(cJSONoutbuffer+last_tmpstr_offset,temp_buffer,tmpstr_len);
		last_tmpstr_offset=tmpstr_len;						
		SEGGER_RTT_printf2("cJSONoutbuffer4:%s\r\n",cJSONoutbuffer);
		Modbussendflag=MODBUS_HAVE_DATA_SEND_YES;
		free(temp_buffer);			
}


uint32_t Factory_settings=0;
uint8_t serverkeyerror=0;
/*Ser or Reset Factory settings Flags.*/
/**
\mode  :1 Read and set mode   
				0 Reset mode
**/
void Factorymodeset(bool mode)
{
	/*FLASH_BASE save the first power on data.*/
	/*judge the data whether need to dynamic reigstration.*/
			fmc_erase_pages(addr_Factory_set_flag,1);
			lv_write_e2prom(addr_Factory_set_flag,0xAAFFAAFF);
			delay_ms(10);		
			lv_read_e2prom(addr_Factory_set_flag,&Factory_settings);
			if(0xAAFFAAFF==Factory_settings)
			{
			
			}
			else
			{
				while(1)
				{
					SEGGER_RTT_printf2(">Chip FMC(Flash) ERROR !!");			
				}
			}
}



void check_if_first_boot(void)
{
		//读出厂设置的FLASH地址，判断是否为第一次上电，设置出厂设置标志位
//	lv_read_e2prom(addr_Factory_set_flag,&Factory_settings);
//	SEGGER_RTT_printf2("lv_read_e2prom addr_Factory_set_flag\r\n");
//	if(0xAAFFAAFF!=Factory_settings)
//	{
//		SEGGER_RTT_printf2("Factorymodeset\r\n");
//		Factorymodeset(1);
//	}
//	//读服务器发送过来的密钥
//	lv_read_e2prom(addr_server_key,&Factory_settings);
//	SEGGER_RTT_printf2("addr_server_key:%x\r\n",Factory_settings);
//	Certification.keybyte=Factory_settings;
//	if(0xFFFFFFFF!=Certification.keybyte)
//	{
//		Certification.key[0]=Certification.keybyte;
//		Certification.keybyte>>=8;
//		Certification.key[1]=Certification.keybyte;
//		Certification.keybyte>>=8;
//		Certification.key[2]=Certification.keybyte;
//		Certification.keybyte>>=8;
//		Certification.key[3]=Certification.keybyte;
//		Certification.keybyte>>=8;
//	}
//	else
//	{
		//如果没有密钥，就进行动态注册
		SEGGER_RTT_printf2("no server_key!!!\r\n");
		char DYNAMIC_BUFFER[800];
//reset:
		DYNAMIC_PORT(DYNAMIC_BUFFER);
		SEGGER_RTT_printf2("DYNAMIC_PORT finish\r\n");
		MQTT_password_encode((u8*)mqtt_passware,(u8*)mqttobject.Password);		
//		http_upload_file("0:/123456.jpg");
//		while(1)
//		{
//			watch_time_tick=0;
//			SEGGER_RTT_printf2(".");
//			delay_ms(1000);
//		}
//	}
//	lv_read_e2prom(addr_server_key,&Factory_settings);
//	SEGGER_RTT_printf2("Certification:%x\r\n",Factory_settings);
//	Certification.keybyte=Factory_settings;
//	if(0xFFFFFFFF!=Certification.keybyte)
//	{
		serverkeyerror=1;
//	}else
//	{
//		serverkeyerror=0;
//		goto reset;
//	}			
}
char *cJSONoutbuffer;
///////////////////////////////////////////mqtt_task任务
void mqtt_task(void *pdata)
{
	SEGGER_RTT_printf2("----mqtt_task------\r\n");
	
	cJSONoutbuffer=(char*)malloc(MAX_SEND_MQTT_LENGTH);//最大字节数据
	char *topic;
	topic=(char*)malloc(128);//最大字节数据
	fisrt_publish(cJSONoutbuffer);//第一次上电就需要上报数据
	while(1)
	{
		NetworkLoop(topic,cJSONoutbuffer);
		OSTimeDlyHMSM(0,0,0,100);  //延时500ms
 	}
}
