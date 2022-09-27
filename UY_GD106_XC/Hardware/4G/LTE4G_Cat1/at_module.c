#include "header.h"

#define     MAX_PACKET_SIZE     1024


char *mode_imei="123456789";
char *card_ccid="123456789";

//�ṹ���ʼ��
void module_init(ATModuleConfig* cfg)
{
    memset(cfg,0,sizeof(ATModuleConfig));
}

//ģ���ϵ�
void module_poweron(ATModuleConfig* cfg)
{
    if(cfg && cfg->fpPowerOn)
    {
        cfg->fpPowerOn(1);
        cfg->eModuleStatus = AT_MODULE_IDLE;
    }
}

void module_reset(ATModuleConfig* cfg)
{
	cfg->fpReset(1);
}

AT_ERROR send_CIREGU(ATModuleConfig* cfg,char *buff)
{
	AT_ERROR err;
	char **receipt_addr;
	char *receipt;
	receipt_addr = &receipt;
	unsigned long ulTick = 0;
	unsigned long timeout = 1000*60;
	SEGGER_RTT_printf2("send_CIREGU\r\n");
	
//	SEGGER_RTT_printf2("send_CIREGU at_parser_send_wait1\r\n");
//	ulTick = cfg->atConfig.fpSystick();
//	while(cfg->atConfig.fpSystick() - ulTick < timeout)//�ȴ��Ƿ�ע������
//	{
//		err=at_parser_read_handle(&cfg->atConfig,"+CIREGU: 1",0,0);
//		if(err == AT_ERR_NONE)//�յ�����ֵ
//			return err;
//	}
	
	for(uint8_t i=0;i<10;i++)
	{
		SEGGER_RTT_printf2("send_CIREGU:%d\r\n",i);
		err = at_parser_send_wait(&cfg->atConfig,					//�ͻ���
								"AT\r\n\0",										//ATָ��
								strlen("AT\r\n\0"),								//ATָ���
								"OK",	//���سɹ���ָ��
								(const char **)receipt_addr,				//���ص�ָ��		//ME DETACH
								cfg->atMqttCfg->iTimeout,					//��ʱʱ��
								6											//�ظ�����
								);
		if(err == AT_ERR_NONE)//�յ�����ֵ
			break;
	}
//	SEGGER_RTT_printf2("send_CIREGU AT\r\n");
//	snprintf(buff,strlen("AT+CEREG?\r\n0"),"AT+CEREG?\r\n");//��ȡATָ��
//	err = at_parser_send_wait(&cfg->atConfig,					//�ͻ���
//								buff,										//ATָ��
//								strlen(buff),								//ATָ���
//								"CEREG:",	//���سɹ���ָ��
//								(const char **)receipt_addr,				//���ص�ָ��		//ME DETACH
//								cfg->atMqttCfg->iTimeout,					//��ʱʱ��
//								3											//�ظ�����
//								);
	SEGGER_RTT_printf2("send_CIREGU at_parser_send_wait\r\n");
	ulTick = cfg->atConfig.fpSystick();
	while(cfg->atConfig.fpSystick() - ulTick < timeout)//�ȴ��Ƿ�ע������
	{
		err=at_parser_read_handle(&cfg->atConfig,"+CIREGU: 1",0,0);
		if(err == AT_ERR_NONE)//�յ�����ֵ
			return err;
	}
	SEGGER_RTT_printf2("send_CIREGU at_parser_send_wait2\r\n");
	snprintf(buff,strlen("AT+CEREG?\r\n0"),"AT+CEREG?\r\n");//��ȡATָ��
	err = at_parser_send_wait(&cfg->atConfig,					//�ͻ���
								buff,										//ATָ��
								strlen(buff),								//ATָ���
								"CEREG:",	//���سɹ���ָ��
								(const char **)receipt_addr,				//���ص�ָ��		//ME DETACH
								cfg->atMqttCfg->iTimeout,					//��ʱʱ��
								3											//�ظ�����
								);
	SEGGER_RTT_printf2("send_CIREGU at_parser_send_wait3\r\n");
	char *addr = strstr( receipt, "+CEREG:");
	if(*addr != 0)
	{
		if(strstr( addr, ",1") != 0)
			return err;
	}
	err = AT_ERR_ERROR;				
	return err;	
}

#define CEREG_RETRY_TIMES 100
#define ADDR_MAX_SIZE 300
#define ADDR2_MAX_SIZE 300
//#define RECBUFF_MAX_SIZE 300




POWER_ENUM powState(ATConfig *cfg)		//����״̬��
{
	AT_ERROR returnStatus = AT_ERR_PARAM_INVAILD ;
    uint8_t cmdStatus = 0;
	uint8_t checkCmdRetry = 0;
	const char *recBuff;
	char *addr = NULL;
	char *addr2 = NULL;
//	unsigned long ulTick;
//	unsigned long timeout = 1000*30;
	SEGGER_RTT_printf2("powState\r\n");
	addr=(char *)malloc(ADDR_MAX_SIZE);
	addr2=(char *)malloc(ADDR2_MAX_SIZE);
//	recBuff=(char *)malloc(RECBUFF_MAX_SIZE);
	while(1)
  {
        switch(cmdStatus)
        {
						case 0:		//�ȴ�ģ�鿪��
							returnStatus = at_parser_send_wait2(cfg, "AT\r\n\0", 0, 0, 0, 0, 10, 1000);

							SEGGER_RTT_printf2("case0 :%d\r\n",returnStatus);
						
							if(returnStatus == AT_ERR_NONE)
								cmdStatus++;
							else
							{
//								free(addr);
//								free(addr2);
								return POWER_FAIL;
							}
								
						case 1:		//�رջ���
							returnStatus = at_parser_send_wait2(cfg, "ATE0\r\n\0",0, 0, 0, 0, 3, 3000);		
							SEGGER_RTT_printf2("case1 :%d\r\n",returnStatus);
							if(returnStatus == AT_ERR_NONE)
								cmdStatus++;
							else
							{
//								free(addr);
//								free(addr2);
								return POWER_FAIL;
							}
						
						case 2:		//���ICCID		

							returnStatus = at_parser_send_wait2(cfg, "ATI\r\n\0",0, "OK", 0, &recBuff, 3, 3000);	
						
							memset(addr,0,ADDR_MAX_SIZE);
							memset(addr2,0,ADDR2_MAX_SIZE);
							returnStatus = at_parser_send_wait2(cfg, "AT+ICCID\r\n\0",0, "ICCID", 0, &recBuff, 3, 3000);	
							SEGGER_RTT_printf2("GET ICCID ERR:%d,recbuff:%s\r\n",returnStatus,recBuff);
							if(returnStatus == AT_ERR_NONE)
							{	
								
								if(recBuff != NULL)
								{
									
									addr = strstr( recBuff, "+ICCID:");		//+CEREG: 0,1   1 mean registered, home network,and 5 mean registered, roaming	
									addr2 =strstr( addr, "\r\n");
									if(addr != NULL)
									{
										memset(card_ccid,0,CCID_MAX_SIZE);
										memcpy(card_ccid,addr+7,addr2-addr-7);
										SEGGER_RTT_printf2("--->GET ICCID %s\r\n",card_ccid);
										SEGGER_RTT_printf2("---------------------------\r\n");
										checkCmdRetry=0;
										cmdStatus++;													
									}
								}
								else
								{
									checkCmdRetry++;
								}
								
							}	
							else
							{
								if(checkCmdRetry++>5)
								{
//									free(addr);
//									free(addr2);
									return POWER_FAIL;
								}
															
							}
																	
						case 3:		//���ڻ��		
							memset(addr,0,ADDR_MAX_SIZE);
							memset(addr2,0,ADDR2_MAX_SIZE);
							returnStatus = at_parser_send_wait2(cfg, "AT+CGSN\r\n\0",0, "OK", 0, &recBuff, 3, 3000);	
							SEGGER_RTT_printf2("GET IMEI ERR:%d,recbuff:%s\r\n",returnStatus,recBuff);//"863853057398131"
							if(returnStatus == AT_ERR_NONE)
							{	
								if(recBuff != NULL)
								{
									
									addr = strstr( recBuff, "\"");		//+CEREG: 0,1   1 mean registered, home network,and 5 mean registered, roaming	
									addr2 =strstr( addr+1, "\"");
									if(addr != NULL)
									{
										memset(mode_imei,0,IMEI_MAX_SIZE);
										memcpy(mode_imei,addr+1,addr2-addr-1);
										SEGGER_RTT_printf2("--->GET IMEI %s\r\n",mode_imei);
										checkCmdRetry=0;
										cmdStatus++;													
									}
								}
								else
								{
									checkCmdRetry++;
								}
								
							}	
							else
							{
								if(checkCmdRetry++>5)
								{
//									free(addr);
//									free(addr2);
									return POWER_FAIL;
								}
															
							}							
						case 4:		//��ѯ�Ƿ�����������
							memset(addr,0,ADDR_MAX_SIZE);
							returnStatus = at_parser_send_wait2(cfg, "AT+CEREG?\r\n\0",0, "CEREG", 0, &recBuff, 3, 3000);	
							SEGGER_RTT_printf2("case4 :%d,recbuff:%s\r\n",returnStatus,recBuff);
							if(returnStatus == AT_ERR_NONE)
							{	
								
								if(recBuff != NULL)
									addr = strstr( recBuff, "+CEREG:");		//+CEREG: 0,1   1 mean registered, home network,and 5 mean registered, roaming	
								
								if(*addr != 0)
								{
									if(strstr( addr, ",1") != 0)//20220623
									{
										cmdStatus++;		
									}
									else if(strstr( addr, ",5") != 0)//20220623
									{
										cmdStatus++;		
									}
									else
										checkCmdRetry++;
								}	
								else
									checkCmdRetry++;
							}  
							else
								checkCmdRetry++;
							delay_ms(1000);
							if(checkCmdRetry>CEREG_RETRY_TIMES)
							{
								return POWER_FAIL;
							}
							break ;
					case 5:		//��ѯ�Ƿ�����������
							memset(addr,0,ADDR_MAX_SIZE);
							memset(addr2,0,ADDR2_MAX_SIZE);
							returnStatus = at_parser_send_wait2(cfg, "AT+CCLK?\r\n\0",0, "+CCLK", 0, &recBuff, 3, 3000);	
							SEGGER_RTT_printf2("case5 :%d,recbuff:%s\r\n",returnStatus,recBuff);
							if(returnStatus == AT_ERR_NONE)
							{	
								
								if(recBuff != NULL)
								{
									addr = strstr( recBuff, "+CCLK:");		//+CEREG: 0,1   1 mean registered, home network,and 5 mean registered, roaming	
									addr2 = strstr( addr, ",");
								}
								if((addr != NULL)&&(addr2 != NULL))
								{
									int year,month,date,hour,min,sec;
									SEGGER_RTT_printf2("addr:%s\r\n",addr);
//									sscanf(addr,"+CCLK: \"%d/%d/%d/,%d:%d:%d",&year,&month,&date,&hour,&min,&sec);
									sscanf(addr,"+CCLK: \"%d/%d/%d/",&year,&month,&date);
									
									SEGGER_RTT_printf2("addr2:%s\r\n",addr2);
									sscanf(addr2+1,"%2d:%2d:%2d",&hour,&min,&sec);
									SEGGER_RTT_printf2("we get time:%02d/%02d/%02d/, %02d:%02d:%02d",year,month,date,hour,min,sec);
									RTC_Set_Date_Time(year,month,date,hour,min,sec);
									
									return POWER_SUCCEED;				
								}	
								else
								{
									
									SEGGER_RTT_printf2("addr==NULL\r\n");
									SEGGER_RTT_printf2("recbuff2:%s\r\n",recBuff);
									checkCmdRetry++;
								}
							}  
							else
								checkCmdRetry++;
							delay_ms(1000);
							if(checkCmdRetry>CEREG_RETRY_TIMES)
							{
//								free(addr);
//								free(addr2);
								return POWER_FAIL;
							}
							break ;		
														
						default:
//							free(addr);
//							free(addr2);
							return POWER_ERROR;
			}
	}
}


MODULE_ERRPO module_networking(ATModuleConfig* cfg)
{
	POWER_ENUM err;
//	char buff[256];
	char i;
	module_reset(cfg);			//��λ//20220625
	module_poweron(cfg);			//����
	for(i=0;i<3;i++)
	{		
		err=powState(&cfg->atConfig);		//����״̬��
		SEGGER_RTT_printf2("powState finish err:%d\r\n",err);
		if(err == POWER_SUCCEED)
			break;
		module_reset(cfg);			//��λ
	}	
	if(i>=3)
	{
		cfg->eModuleStatus = AT_MODULE_POWER_DOWN;
		return MODLUE_ERR_INIT;
	}	
	return MODULE_ERR_NONE;
}

//����MQTT������״̬��
MODULE_ERRPO module_mqtt_connect(ATModuleConfig* cfg, mqttConnectData Arg)
{
    if(cfg)
    {
        char buff[256];
        AT_ERROR err;
				int Init_module_Status = 0;
				int check_module_at_retry = 0;
        if(cfg->eModuleStatus != AT_MODULE_IDLE)	//�ж�ģ���Ƿ��ڿ���״̬
				{
						SEGGER_RTT_printf2("cfg->eModuleStatus != AT_MODULE_IDLE!!!!\r\n");
            return MODULE_ERR_NO_INIT;
				}
        if(cfg->iMqttConnectFlag)					//�ж�ģ���Ƿ�������
				{
						SEGGER_RTT_printf2("MODULE_ERR_MQTT_ALREADY!!!!\r\n");
            return MODULE_ERR_MQTT_ALREADY;
				}
		
				while(1)
				{
					switch(Init_module_Status)
					{

						
					case 0:		//MQTT��������
						cfg->atMqttCfg->fpMakeMqttParserConfig(Arg.devicename,Arg.username,Arg.password,buff,256);		//��ȡATָ��
						err = at_parser_send_wait(&cfg->atConfig,					//@AT��������
																		buff,										//@ATָ��ͻ��棨ƴ��������ͣ�
																		strlen(buff),								//@AT����ָ��泤��
										cfg->atMqttCfg->pcMqttParserConfigSuccess,	//@���سɹ���ָ�� Content: "OK"
																		0,											//@���صĽ���ָ��������ַ�ĵ�ַ
																		cfg->atMqttCfg->iTimeout,					//@ÿ���������ͳ�ʱʱ��
																		0											//@�����ظ����ʹ���
																		);
						if(err != AT_ERR_NONE)
						{
							check_module_at_retry++;		//AT����ʧ��
						}
						else
						{
							Init_module_Status++;			//AT���ͳɹ������뵽��һ��״̬

						}
						if(check_module_at_retry>3)
						{
							check_module_at_retry = 0;	//����ʧ�����Σ����ִ���ֱ������
							SEGGER_RTT_printf2("fpMakeMqttParserConfig:%d\r\n",err);
							return MODULE_ERR_MQTT_NONE;
						}
						break ;
					case 1:		//��������
						cfg->atMqttCfg->fpMakeMqttAddrConfig(Arg.mqtturl,Arg.urlport,Arg.mqttversion,buff,256);		//��ȡATָ��
						err = at_parser_send_wait(&cfg->atConfig,					//����ATָ��
																		buff,
																		strlen(buff),
																		cfg->atMqttCfg->pcMqttAddrConfigSuccess,			//���سɹ���ָ��
																		0,
																		cfg->atMqttCfg->iTimeout,
																		0
																		);
						if(err != AT_ERR_NONE)
						{
							check_module_at_retry++;		//AT����ʧ��
						}
						else
						{
							Init_module_Status++;			//AT���ͳɹ������뵽��һ��״̬

						}
						if(check_module_at_retry>3)
						{
							check_module_at_retry = 0;	//����ʧ�����Σ����ִ���ֱ������
							SEGGER_RTT_printf2("fpMakeMqttAddrConfig:%d\r\n",err);
							return MODULE_ERR_MQTT_NONE;
						}
						break ;
					case 2:		//���ӷ�����
						cfg->atMqttCfg->fpMakeMqttConnect(Arg.keepalive,buff,256);		//��ȡATָ��
						err = at_parser_send_wait(&cfg->atConfig,					//����ATָ��
																		buff,
																		strlen(buff),
																		cfg->atMqttCfg->pcMqttConnectSuccess,			//���سɹ���ָ��
																		0,
//																		cfg->atMqttCfg->iTimeout,
																		2000,//20220918
																		0
																		);
						if(err != AT_ERR_NONE)
						{
							SEGGER_RTT_printf2("fpMakeMqttConnect err:%d\r\n",err);
							check_module_at_retry++;		//AT����ʧ��
						}
						else
						{
							Init_module_Status++;			//AT���ͳɹ������뵽��һ��״̬

						}
						if(check_module_at_retry>3)
						{
							check_module_at_retry = 0;	//����ʧ�����Σ����ִ���ֱ������
							return MODULE_ERR_MQTT_NONE;
						}
						break ;
					case 3:		//���Ӳ�ѯ
						cfg->atMqttCfg->fpMakeMqttStatu(buff,256);		//��ȡATָ��
						err = at_parser_send_wait(&cfg->atConfig,					//����ATָ��
																		buff,
																		strlen(buff),
																		cfg->atMqttCfg->pcMqttStatuSuccess,			//���سɹ���ָ��
																		0,
																		cfg->atMqttCfg->iTimeout,
																		0
																		);
						if(err != AT_ERR_NONE)
						{
							check_module_at_retry++;		//AT����ʧ��
						}
						else
						{
							Init_module_Status++;			//AT���ͳɹ������뵽��һ��״̬
							cfg->iMqttConnectFlag = 1;
							return MODULE_ERR_NONE;

						}
						if(check_module_at_retry>3)
						{
							check_module_at_retry = 0;	//����ʧ�����Σ����ִ���ֱ������
							return MODULE_ERR_MQTT_NONE;
						}
						break ;
					default:
						return MODULE_ERR_PARAM;
					}
				}
    }
    return MODULE_ERR_PARAM;
}

//�Ͽ�����
MODULE_ERRPO module_mqtt_disconnect(ATModuleConfig* cfg)
{
    if(cfg)
    {
        char buff[128];
        AT_ERROR err;
        if(cfg->eModuleStatus != AT_MODULE_IDLE)
            return MODULE_ERR_NO_INIT;
		
        cfg->atMqttCfg->fpMakeMqttDisConnect(buff,128);					//��ȡ�Ͽ�MQTT��ATָ��
        err = at_parser_send_wait(&cfg->atConfig,						//�ͻ���
                                buff,									//ATָ��
                                strlen(buff),							//ATָ���
                                cfg->atMqttCfg->pcDisconnectHead,		//���سɹ���ָ��
                                0,										//���ص�ָ��
                                cfg->atMqttCfg->iTimeout,				//��ʱʱ��
                                3										//�ظ�����
                                );
        
        cfg->iMqttConnectFlag = 0;
        if(err != AT_ERR_TIMEOUT)
            return MODULE_ERR_NONE;
        else
            return MODULE_ERR_NO_ACK;
    }
    return MODULE_ERR_PARAM;
}

//����MQTT��������Ͽ�����ص�����
void module_set_mqtt_disconnect_callback(ATModuleConfig* cfg,pMqttDisconnectURC f)
{
	cfg->fpMqttDisconnectCallback = f;
}

//����Ͽ����ӻص�����
void module_mqtt_disconnect_urc(void* arg,const char* cmd,int len)
{
    ATModuleConfig* cfg = (ATModuleConfig*)arg;
    cfg->iMqttConnectFlag = 0;
	if(cfg->fpMqttDisconnectCallback)
		cfg->fpMqttDisconnectCallback(0);
}

//ATģ��ִ��ѭ��
void module_loop(ATModuleConfig* cfg)
{
    if(cfg)
        at_parser_loop(&cfg->atConfig);
}

//ע��TopicURC�¼��ص�����
AT_ERROR mqtt_topic_register_urc(ATConfig *cfg,const char* cmd,pATCmdURCHandle f,void* arg)
{
    uint8_t i = 0;
		AT_ERROR ree = AT_ERR_NONE;
    for(i = 0;i < TOPIC_MAX_URC_NUM;i++)
    {
				if(strncmp(cfg->sTopicURCCfg[i].ucTopicURCHead,cmd,strlen(cfg->sTopicURCCfg[i].ucTopicURCHead)) == 0)
				{
						SEGGER_RTT_printf2("(%s) URC duplicate registration.\r\n", cmd);
		//			free((char*)cmd);
						return ree;
				}
        if(cfg->sTopicURCCfg[i].ucTopicURCHead == 0)
        {
            cfg->sTopicURCCfg[i].ucTopicURCHead = cmd;
            cfg->sTopicURCCfg[i].pTopicURCHandle = f;
            cfg->sTopicURCCfg[i].pvTopicURCArg = arg;
						return ree;
        }
    }
		ree = AT_ERR_FULL;
//		free((char*)cmd);
		return ree;
}

//����TopicURC�¼�
void module_mqtt_subtpoic_urc(void* arg,const char* data,int len)
{
	int i = 0;
	ATConfig *cfg = arg;
	data += strlen("+MSUB: \"");
    for(i = 0;i < TOPIC_MAX_URC_NUM;i++)
    {
		if(cfg->sTopicURCCfg[i].ucTopicURCHead && strncmp(cfg->sTopicURCCfg[i].ucTopicURCHead,data,strlen(cfg->sTopicURCCfg[i].ucTopicURCHead)) == 0)	//Ѱ��URC
        {
            data += strlen(cfg->sTopicURCCfg[i].ucTopicURCHead)+1;
			for(int j=0; j<20;j++)
			{
				if(*(data+j) == '"')
				{
					data += (j+1);
					break;
				}
					
			}
			cfg->sTopicURCCfg[i].pTopicURCHandle(cfg->sTopicURCCfg[i].pvTopicURCArg,data,len);		//�ҵ���ӦURC��ִ�лص�����
			break;
		}
    }

}

//��������
AT_ERROR MQTTSubscribe(ATModuleConfig* cfg, const char* topicFilter, enum QoS qos, pATCmdURCHandle f, void* arg)
{
	AT_ERROR err;
//	char *dest = 0;
	char dest[100]={0};
//	int totalSize = strlen("AT+MSUB=\"\",0\r\n") + strlen(topicFilter) + 1;
//	dest = (char*)malloc(totalSize);
//	if(!dest)
//	{
//				SEGGER_RTT_printf2("\r\nMQTTSubscribe dest==NULL!!!!!\r\n");
//        return AT_ERR_PARAM_INVAILD;
//	}
	snprintf(dest,100,"AT+MSUB=\"%s\",%d\r\n",topicFilter,qos);

	err = at_parser_send_wait(&cfg->atConfig,				//�ͻ���
				dest,										//ATָ��
				strlen(dest),								//ATָ���
				cfg->atMqttCfg->pcMqttSubTopicSuccess,		//���سɹ���ָ��
				0,											//���ص�ָ��
				cfg->atMqttCfg->iTimeout,					//��ʱʱ��
				3											//�ظ�����
				);
	
//	free(dest);
	
	if(err == AT_ERR_NONE)
	{
		mqtt_topic_register_urc(&cfg->atConfig, topicFilter, f, arg);
	}

	return err;
}

//��������
AT_ERROR MQTTPublish(ATModuleConfig* cfg, const char* topicName, MQTTMessage* message)
{
	AT_ERROR err;
	char *dest = 0;
	char data_len[8];
	memset(data_len,0,sizeof(data_len));
	snprintf(data_len,8,"%d",strlen(message->payload));
	int totalSize = strlen("AT+MPUBEX=\"\",0,0,\r\n") + strlen(topicName) + strlen(data_len) + 1;

	dest = (char*)malloc(totalSize);
	if(!dest)
        return AT_ERR_PARAM_INVAILD;
	snprintf(dest,totalSize,"AT+MPUBEX=\"%s\",%d,0,%d\r\n",topicName,message->qos, strlen(message->payload));
	SEGGER_RTT_printf2("\r\ntotalSize:%d,dest:%s\r\n",totalSize,dest);
	err = at_parser_send_wait(&cfg->atConfig,				//�ͻ���
				dest,										//ATָ��
				strlen(dest),								//ATָ���
				">",		//���سɹ���ָ��
				0,											//���ص�ָ��
				cfg->atMqttCfg->iTimeout,					//��ʱʱ��
				0											//�ظ�����
				);
	free(dest);
	if(err == AT_ERR_TIMEOUT)
	{
		SEGGER_RTT_printf2("1.MQTTPublish send timeout\r\n");
		MqttMqttDisconnectCallback2();
		return err;
	}
	err = at_parser_send_wait(&cfg->atConfig,				//�ͻ���
				message->payload,							//ATָ��
				strlen(message->payload),					//ATָ���
				"OK\r\n",//cfg->atMqttCfg->pcMqttPubTopicSuccess, ��ȷ��
				0,		//���سɹ���ָ��
				cfg->atMqttCfg->iTimeout,					//��ʱʱ��
				0											//�ظ�����
				);
	if(err == AT_ERR_TIMEOUT)
	{
		SEGGER_RTT_printf2("2.MQTTPublish send timeout\r\n");
		MqttMqttDisconnectCallback2();
		return err;
	}
	return err;
}

//��������
AT_ERROR MQTTPublish2(ATModuleConfig* cfg, const char* topicName, MQTTMessage* message)
{
	AT_ERROR err;
	char *dest = 0;
	char data_len[8];
	memset(data_len,0,sizeof(data_len));
	snprintf(data_len,8,"%d",strlen(message->payload));
	int totalSize = strlen("AT+MPUBEX=\"\",0,0,\r\n") + strlen(topicName) + strlen(data_len) + 1;

	dest = (char*)malloc(totalSize);
	if(!dest)
        return AT_ERR_PARAM_INVAILD;
	snprintf(dest,totalSize,"AT+MPUBEX=\"%s\",%d,0,%d\r\n",topicName,message->qos, strlen(message->payload));
//	SEGGER_RTT_printf2("\r\ntotalSize:%d,dest:%s\r\n",totalSize,dest);
	err = at_parser_send_wait_file(&cfg->atConfig,				//�ͻ���
				dest,										//ATָ��
				strlen(dest),								//ATָ���
				">",		//���سɹ���ָ��
				0,											//���ص�ָ��
				cfg->atMqttCfg->iTimeout,					//��ʱʱ��
				0											//�ظ�����
				);
	free(dest);
	if(err == AT_ERR_TIMEOUT)
	{
		SEGGER_RTT_printf2("1.MQTTPublish send timeout\r\n");
		MqttMqttDisconnectCallback2();
		return err;
	}
	err = at_parser_send_wait(&cfg->atConfig,				//�ͻ���
				message->payload,							//ATָ��
				strlen(message->payload),					//ATָ���
				"OK\r\n",//cfg->atMqttCfg->pcMqttPubTopicSuccess, ��ȷ��
				0,		//���سɹ���ָ��
				cfg->atMqttCfg->iTimeout,					//��ʱʱ��
				0											//�ظ�����
				);
	if(err == AT_ERR_TIMEOUT)
	{
		SEGGER_RTT_printf2("2.MQTTPublish send timeout\r\n");
		MqttMqttDisconnectCallback2();
		return err;
	}
	return err;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////


//�����ļ�
#define FILE_NAME_STR_LEN 80
//#define MPUBEX_MAX_LEN 65000 //���������MPUBEX��󳤶�65535
#define MPUBEX_MAX_LEN 10000 //���������MPUBEX��󳤶�65535
#define TOPIC_LENGTH  200
AT_ERROR MQTTPublish_File(ATModuleConfig* cfg, char *filename)
{	
	
	FIL img_file2;	
	AT_ERROR err=0,tmp_err=0;
	FRESULT res;
	UINT  bw;
	u32 filesize=0;
	char topic[TOPIC_SIZE];
	char filename2[FILE_NAME_STR_LEN];
	char *payload,*read_data;
	//����ļ�·����Ϣ
	snprintf(filename2,FILE_NAME_STR_LEN,"%s/%s",RECORD_PATH,filename);
	SEGGER_RTT_printf2("MQTTPublish_File\r\n");
	//��ʼ��ָ��
	payload=(char*)malloc(MQTT_MAX_FLIE_SEND_LENGTH+200);
	read_data=(char*)malloc(MQTT_MAX_FLIE_SEND_LENGTH+1);
	//��������ڴ�ʧ�ܣ���ô����
	if((payload==NULL)||(read_data==NULL))
	{
		SEGGER_RTT_printf2("payload==null!!!...\r\n");
		free(payload);
		free(read_data);
		return AT_ERR_ERROR;
	}
	
	SEGGER_RTT_printf2("filename2:%s\r\n",filename2);
	res = f_open(&img_file2, filename2, FA_OPEN_EXISTING | FA_READ);
	SEGGER_RTT_printf2("PublishFile3\r\n");
	if(res)
	{
		SEGGER_RTT_printf2("open file fail res:%d!!!...\r\n",res); //��ʾ���ڴ�������
		f_close(&img_file2);
		free(payload);
		free(read_data);
		return AT_ERR_ERROR;
	}	
	else
	{
		SEGGER_RTT_printf2("open file sucess!!!...\r\n");
	}
	SEGGER_RTT_printf2("PublishFile4\r\n");
	//send file info
	filesize=f_size(&img_file2);
	u16 tran_times=filesize/MQTT_MAX_FLIE_SEND_LENGTH+1;
	u16 remain_length=filesize%MQTT_MAX_FLIE_SEND_LENGTH;
	u16 tran_length=0;
	if(remain_length==0)
		tran_times--;
	u16 total_tran_times=tran_times;
	u16 page_num=0;
	SEGGER_RTT_printf2("filesize:%d,tran_times:%d,remain_length:%d\r\n",filesize,tran_times,remain_length);
	while(tran_times>0)
	{
		
		watch_time_tick=0;
		if(tran_times-->0)
			tran_length=MQTT_MAX_FLIE_SEND_LENGTH;				
		else
			tran_length=remain_length;
		page_num++;
		memset(payload,0,MQTT_MAX_FLIE_SEND_LENGTH+200);//payload�����������ݱ�ʵ��Ҫ���͵�����С
		snprintf(payload,MQTT_MAX_FLIE_SEND_LENGTH+200,"{\"type\":\"pic_file\",\"deviceId\":\"%s\",\"fileSize\":\"%d\",\"pgSize\":\"%d\",\"pgNum\":\"%d\",\"fileName\":\"%s\",\"content\":\"",ChipIDstring+12,filesize,total_tran_times,page_num,filename);			
		memset(read_data,0,MQTT_MAX_FLIE_SEND_LENGTH+1);//payload�����������ݱ�ʵ��Ҫ���͵�����С		
		res = f_read(&img_file2, (u8*)read_data, tran_length, &bw);
		if(res)
		{
			err=1;
			SEGGER_RTT_printf2("f_read,err:%d\r\n",res);
			break;
		}
//		SEGGER_RTT_printf2("\r\nbw:%d,read_data:%s\r\n",bw,read_data);
		strcat(payload,read_data);
		strcat(payload,"\"}");
		memset(topic,0,TOPIC_SIZE);
		snprintf(topic,TOPIC_LENGTH,"AT+MPUBEX=\"/devicecamera/%s\",%d,0,%d\r\n",ChipIDstring+12,QOS0, strlen(payload));

		at_parser_send_no_wait2(&cfg->atConfig,topic,strlen(topic));
		
		delay_ms(20);
		
		if(tran_times==0)
		{
			at_parser_send_wait(&cfg->atConfig,				//�ͻ���
			payload,							//ATָ��
			strlen(payload),					//ATָ���
			"OK\r\n",//cfg->atMqttCfg->pcMqttPubTopicSuccess, ��ȷ��
			0,		//���سɹ���ָ��
			cfg->atMqttCfg->iTimeout,					//��ʱʱ��
			0											//�ظ�����
			);			
		}
			
		else
			at_parser_send_no_wait2(&cfg->atConfig,payload,strlen(payload));
		
		delay_ms(100);
		
		SEGGER_RTT_printf2("\r\ntran_times:%d\r\n",tran_times);
	}
	f_close(&img_file2);	
	if(err==0)
	{
		f_unlink(filename2);
		SEGGER_RTT_printf2("f_unlink:%s\r\n",filename2);
	}
	else
	{
		SEGGER_RTT_printf2("upload fail\r\n");
	}
	free(payload);
	free(read_data);
	SEGGER_RTT_printf2("PublishFile end\r\n");
	
}







//////////////////////////////////////////////////////////////////////////////////////////////////////////


MODULE_ERRPO module_start(ATModuleConfig* cfg)
{
	SEGGER_RTT_printf2("module_start\r\n");
	if(cfg)
	{
		if(cfg->eModuleStatus == AT_MODULE_POWER_DOWN)
            return MODULE_ERR_PWD;
		if(cfg->eModuleStatus == AT_MODULE_SLEEP)
            return MODULE_ERR_SLEEP;
		
		//AT״̬��				//ע��URC�ص��������ͻ��ˣ�URCָ��ص��������ͻ��ˣ�
		at_parser_register_urc(&cfg->atConfig,cfg->atMqttCfg->pcMakeMqttDisConnectURC,module_mqtt_disconnect_urc,cfg);			//ע������ص�����
		at_parser_register_urc(&cfg->atConfig, cfg->atMqttCfg->pcMqttSubTopicURC, module_mqtt_subtpoic_urc, &cfg->atConfig);	//ע�ᶩ������ص�����
		
		cfg->eModuleStatus = AT_MODULE_IDLE;
        return MODULE_ERR_NONE;
	}
	return MODULE_ERR_PARAM;
}
