#include "header.h"
#include "Version.h"

u16 SAOMA_USART_RX_STA=0;       //接收状态标记	
u8 saoma_buffer[SAOMA_BUFFER_SIZE];
typedef struct
{
    uint16_t    usReceiveLen;           //接收到的数据长度
    uint8_t     *pReceiveBuff;          //接受数据缓冲区
    uint16_t    usMaxReceiveLen;        //缓冲区最大长度
    pUartReceiveCallback    pRecCallback;   //接收回调函数
    uint32_t    ulByteInterval;         //字节间超时
    uint32_t    ulRecentTimestamp;      //最近一次接收到字节的系统时基
}UART_PARAM;

void saoma_uart_init(uint32_t BAUD)
{	
		/* USART interrupt configuration */
		nvic_irq_enable(SAOMA_UART_IRQ, 1, 1);
		
		rcu_periph_clock_enable(SAOMA_UART_GPIO_CLK);
    /* enable USART clock */
    rcu_periph_clock_enable(SAOMA_UART_CLK);

    /* connect port to USARTx_Tx */
    gpio_af_set(SAOMA_UART_GPIO_PORT, SAOMA_UART_AF, SAOMA_UART_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_af_set(SAOMA_UART_GPIO_PORT, SAOMA_UART_AF, SAOMA_UART_RX_PIN);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(SAOMA_UART_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,SAOMA_UART_TX_PIN);
    gpio_output_options_set(SAOMA_UART_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,SAOMA_UART_TX_PIN);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(SAOMA_UART_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,SAOMA_UART_RX_PIN);
    gpio_output_options_set(SAOMA_UART_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,SAOMA_UART_RX_PIN);

    /* USART configure */
    usart_deinit(SAOMA_UART);
    usart_baudrate_set(SAOMA_UART,BAUD);
    usart_receive_config(SAOMA_UART, USART_RECEIVE_ENABLE);
    usart_transmit_config(SAOMA_UART, USART_TRANSMIT_ENABLE);
    usart_enable(SAOMA_UART);
		
		/* enable USART0 receive interrupt */
    usart_interrupt_enable(SAOMA_UART, USART_INT_RBNE);
		
}


/**
* @函数名:      UartSend
* @功能描述:    写串口数据
* @输入参数:    buf：写入的数据，len写入的字节数
* @输出参数:    无
* @返回值:      是否成功发送
*/
bool SAOMA_UartSend(const char* buff,int len)
{
	uint32_t i = 0;
	
	if(len)
	{
		while(1)
		{
			usart_data_transmit(SAOMA_UART, *(buff+i));
			i++;
			if(i >= len)
			{
				return true;
			}
			while(RESET == usart_flag_get(SAOMA_UART, USART_FLAG_TBE));
		}
	}
	else
	{
		while(1)
		{	
			usart_data_transmit(SAOMA_UART, *(buff+i));
			i++;
			if(*(buff+i) == 0)
			{
				return true;
			}
			while(RESET == usart_flag_get(SAOMA_UART, USART_FLAG_TBE));
		}
	}
}

void SAOMA_UART_IRQHandler(void)
{
		uint8_t Res; 
#if SYSTEM_SUPPORT_UCOS  //使用UCOS操作系统
	OSIntEnter();    
#endif
    //发生接收中断	
		 if((RESET != usart_interrupt_flag_get(SAOMA_UART, USART_INT_FLAG_RBNE)) &&(RESET != usart_flag_get(SAOMA_UART, USART_FLAG_RBNE))) 
		 {
        /* receive data */
        Res = usart_data_receive(SAOMA_UART);
        if((SAOMA_USART_RX_STA&0x8000)==0)//接收未完成
				{
					if(SAOMA_USART_RX_STA&0x4000)//接收到了0x0d
					{
						if(Res!=0x0a)SAOMA_USART_RX_STA=0;//接收错误,重新开始
						else SAOMA_USART_RX_STA|=0x8000;	//接收完成了 
					}
					else //还没收到0X0D
					{	
						if(Res==0x0d)SAOMA_USART_RX_STA|=0x4000;
						else
						{
							saoma_buffer[SAOMA_USART_RX_STA&0X3FFF]=Res ;
							SAOMA_USART_RX_STA++;
							if(SAOMA_USART_RX_STA>(SAOMA_BUFFER_SIZE-1))SAOMA_USART_RX_STA=0;//接收数据错误,重新开始接收	  
						}		 
					}
				}   		 	   		 
		} 
#if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//退出中断
#endif
		
}

u8 saoma_statue=SAOMA_IDLE;

char *saoma_data;

char last_id[30];
////////////////////////////////////////////led任务////////////////////////////////////////
void saoma_task(void *pdata)
{
	SEGGER_RTT_printf2("----saoma_task------\r\n");
	char *strjson;
	char file_name[80]={0};
	char file_name2[80]={0};
	char time_str[40]={0};
	saoma_data=(char*)malloc(SAOMA_BUFFER_SIZE);
	u32 saoma_timeout=0;
	while(1)
	{
		
		if(SAOMA_USART_RX_STA&0x8000)
		{
					strjson=(char*)malloc(SAOMA_BUFFER_SIZE);
					memset(strjson,0,SAOMA_BUFFER_SIZE);
					memcpy(strjson,saoma_buffer,(SAOMA_USART_RX_STA&0X3FFF));
					SEGGER_RTT_printf2("strjson:%s\r\n",strjson);
					saoma_timeout=0;
					cJSON* cjson = cJSON_Parse(strjson);
					cJSON* nextcjson;
					if(cjson == NULL)
					{
							//提示无效数据
							free(strjson);
							saoma_statue=SAOMA_IDLE;
							SEGGER_RTT_printf2("json pack into cjson error...\r\n");
					}
					else
					{
						nextcjson=cJSON_GetObjectItem(cjson,"id");
//						char* id = cJSON_GetObjectItem(cjson,"id")->valuestring;
						char* id = nextcjson->valuestring;
						if((id==NULL)||(nextcjson==NULL))
						{
							SEGGER_RTT_printf2(" error  data...");		
							audio_play_ctrl(SPEACH_MODE_SAOMA);
							delay_ms(600);
							audio_play_ctrl(SPEACH_MODE_WRONG);
							//提示无效数据
						}
						else
						{
								int i=0;
								for(i=0;i<strlen(id);i++)
								{
									if(last_id[i]!=*(id+i))
									{										
										SEGGER_RTT_printf2("lasd_id[%d]=%c,*id+i=%c",i,last_id[i],*(id+i));
										break;
									}								
								}
								if(i==strlen(id))
								{		
									audio_play_ctrl(SPEACH_MODE_DI);
									delay_ms(600);
									SEGGER_RTT_printf2("repead id:%s\r\n",id);		
								}
								else
								{
									memset(last_id,0,30);
									memcpy(last_id,id,strlen(id));
									audio_play_ctrl(SPEACH_MODE_SAOMA);
									delay_ms(600);
									audio_play_ctrl(SPEACH_MODE_CHENGGONG);
			//						SEGGER_RTT_printf2("id:%s,name:%s,class:%s,checksum:%d\r\n",id,name,classs,checksum);			
									SEGGER_RTT_printf2("id:%s\r\n",id);		
									memset(saoma_data,0,SAOMA_BUFFER_SIZE);
									memcpy(saoma_data,strjson,strlen(strjson));//拷贝有效数据
									memset(file_name,0,80);
									memset(time_str,0,40);
									RTC_Get_String_Date_Time(time_str);
									snprintf(file_name,80,"%s/%s_%s.jpg",RECORD_PATH2,id,time_str);
									create_saoma_publish_data(id,time_str);
									SEGGER_RTT_printf2("file_name:%s\r\n",file_name);
									
			//						camera_take_pic(file_name);
									u8 res=0;
									res=camera_take_pic_base64(file_name);
									if(res==0)
									{
										memset(file_name2,0,80);
										snprintf(file_name2,80,"%s/%s_%s.jpg",RECORD_PATH3,id,time_str);
										res=f_rename(file_name,file_name2);//更换文件位置
										SEGGER_RTT_printf2("\r\nfile_name:%s,file_name2:%s,f_rename,res:%d",file_name,file_name2,res);
										
									}
									else
									{
										SEGGER_RTT_printf2("\r\ntake pic fail !!!!!!!!!!!!\r\n");
										myfatfs_init();
										res=camera_take_pic_base64(file_name);
										if(res==0)
										{
											memset(file_name2,0,80);
											snprintf(file_name2,80,"%s/%s_%s.jpg",RECORD_PATH3,id,time_str);
											res=f_rename(file_name,file_name2);//更换文件位置
											SEGGER_RTT_printf2("\r\nfile_name:%s,file_name2:%s,f_rename,res:%d",file_name,file_name2,res);
											
										}
										else
										{
											SEGGER_RTT_printf2("\r\ntake pic fail !!!!!!!!!!!!\r\n");
											myfatfs_init();
											res=camera_take_pic_base64(file_name);
											if(res==0)
											{
												memset(file_name2,0,80);
												snprintf(file_name2,80,"%s/%s_%s.jpg",RECORD_PATH3,id,time_str);
												res=f_rename(file_name,file_name2);//更换文件位置
												SEGGER_RTT_printf2("\r\nfile_name:%s,file_name2:%s,f_rename,res:%d",file_name,file_name2,res);												
											}
											else
											{
												SEGGER_RTT_printf2("\r\ntake pic fail !!!!!!!!!!!!\r\n");
												myfatfs_init();
											}
											
											
										}
										
									}
								}
						}
					}
					free(strjson);
					cJSON_Delete(cjson);
					SAOMA_USART_RX_STA=0;
		}
		if(saoma_timeout++>200)
		{
			saoma_timeout=0;
			memset(last_id,0,30);
		}
		//SEGGER_RTT_printf2("----saoma_task------\r\n");
		OSTimeDlyHMSM(0,0,0,100);  //延时500ms
 	}
}

void create_saoma_publish_data(char* saomaid,char * saomatime)
{
		u16 tmpstr_len=0,last_tmpstr_offset=0;
		SEGGER_RTT_printf2("create_saoma_publish_data\r\n");
		char *temp_buffer=NULL;//20220620
		temp_buffer=(char*)malloc(MAX_SEND_MQTT_LENGTH/2);//最大字节数据		
		memset(cJSONoutbuffer,0,MAX_SEND_MQTT_LENGTH);		
		memset(temp_buffer,0,MAX_SEND_MQTT_LENGTH/2);
		snprintf(temp_buffer,MAX_SEND_MQTT_LENGTH/2,"{\"type\":\"param\",\"deviceId\":\"%s\",\"version\":\"%s\",\"imei\":\"%s\",\"ccid\":\"%s\",\"data\":{\"saomaID\":\"%s\",\"saomaTIME\":\"%s\",\"pictID\":\"%s_%s.jpg\"}}",ChipIDstring+12,Version,mode_imei,card_ccid,saomaid,saomatime,saomaid,saomatime);
		last_tmpstr_offset=0;
		tmpstr_len=strlen(temp_buffer);
		memcpy(cJSONoutbuffer+last_tmpstr_offset,temp_buffer,tmpstr_len);
		last_tmpstr_offset=tmpstr_len;						
		SEGGER_RTT_printf2("cJSONoutbuffer4:%s\r\n",cJSONoutbuffer);
		Modbussendflag=MODBUS_HAVE_DATA_SEND_YES;
		free(temp_buffer);			
}


