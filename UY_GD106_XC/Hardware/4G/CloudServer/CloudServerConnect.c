#include "header.h"
extern const char *Version;
extern uint8_t ChipID[12];	//ChipID buffer.
extern char ChipIDstring[25];
extern ServerCertification Certification;
//@save in FLASH
const unsigned char key[] = "5e8430dc190c3608"; //encode AES KEY	 		
extern void HexToAscii(unsigned char *pHex, unsigned char *pAscii, int nLen);


int HexStrTobyte(char *str, unsigned char *out, u16 *outlen)
{
	char *p = str;
	char high = 0, low = 0;
	int tmplen = strlen(p), cnt = 0;
	tmplen = strlen(p);
	while(cnt < (tmplen / 2))
	{
		high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
		low = (*(++ p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p) - 48 - 7 : *(p) - 48;
		out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
		p ++;
		cnt ++;
	}
	if(tmplen % 2 != 0) out[cnt] = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;

	if(outlen != NULL) *outlen = tmplen / 2 + tmplen % 2;
	return tmplen / 2 + tmplen % 2;
}



void MQTT_password_encode(u8 *key,u8 *output)
{
	 //@"Author"
  char headerword[7]="bearer/";
		
	uint8_t AESoutput[128]={0};
		
	uint8_t AESinput[128]={0};

//@use for test the AEC_ECB-Base64 encryption.				
	memcpy(AESinput,headerword,7);
    
	memcpy(AESinput+7,key,strlen((char*)key));
	memcpy(AESinput+7+strlen((char*)key),mode_imei+3,12);
	SEGGER_RTT_printf2("AESinput:%s\r\n",AESinput);
//@input bearer/key 000000000000(Chip UID)
//@Creat a utils_aes obkect.set the object property.
	
	utils_aes_context cfg;	cfg.nr=10;
	
	utils_aes_init(&cfg);

//@Set aes_enc key.
//	u8 keys[16]={0x0e,0x84,0x46,0x93,0xea,0x42,0x16,07,0x8a,0xe9,0x1f,0x11,0x15,0xab,0xc9,0xe4};
	u8 keys[16]={0};
	u16 key_len=0;
	HexStrTobyte((char*)key,keys,&key_len);
	
	utils_aes_setkey_enc(&cfg, (u8*)keys, AES_KEY_BITS_128);
//@get the code. 
	memset(AESoutput,0,128);
	u16 aesinput_len=strlen((char*)AESinput);
	u16 aes_crypt_times=aesinput_len/16+1;//20220721需要加密次数
	u16 need_add_num=aesinput_len%16;//需要补充的数据
	if(need_add_num==0)
		aes_crypt_times-=1;
	if(need_add_num!=0)
	{
		need_add_num=16-need_add_num;
		memset(AESinput+aesinput_len,need_add_num,need_add_num);
	}
	for(int j=0;j<aes_crypt_times;j++)
	{
		for(int i=0;i<16;i++)
		{
			utils_aes_crypt_ecb(&cfg,UTILS_AES_ENCRYPT,(unsigned char *)AESinput+j*16,AESoutput+j*16); 
		}		
	}

	size_t recnum=0;
//@ready to encode the AES_data.
	uint8_t *base64out;	
	base64out=output;	
//@base64 encode. 	
	qcloud_iot_utils_base64encode(base64out,128,&recnum,AESoutput,64);
	
	SEGGER_RTT_printf2("base64out:%s\r\n",base64out);
	
}







void HTTP_header_encode( uint8_t output[44])
{
    //@"Author"
  char headerword[10]="bearer/key";
		
	uint8_t AESoutput[32];
		
	uint8_t AESinput[32];
	

//@use for test the AEC_ECB-Base64 encryption.				
	memcpy(AESinput,ChipIDstring+12,22*sizeof(uint8_t));
    
	memcpy(AESinput+12,headerword,10*sizeof(uint8_t));
	
//@input bearer/key 000000000000(Chip UID)
//@Creat a utils_aes obkect.set the object property.

	utils_aes_context cfg;	cfg.nr=10;
	
	utils_aes_init(&cfg);

//@Set aes_enc key.

	utils_aes_setkey_enc(&cfg, key, AES_KEY_BITS_128);
	
	SEGGER_RTT_printf2("%s",headerword);

//@get the code. 
	memset(AESoutput,0,32*sizeof(uint8_t));
	memset(AESinput+22,0x0A,10*sizeof(uint8_t));
	
	utils_aes_crypt_ecb(&cfg,UTILS_AES_ENCRYPT,(unsigned char *)AESinput,AESoutput); 
	utils_aes_crypt_ecb(&cfg,UTILS_AES_ENCRYPT,(unsigned char *)AESinput+16,AESoutput+16); 
	size_t recnum=0;

//@ready to encode the AES_data.

	uint8_t *base64out;
	
	base64out=output;
	
//@base64 encode. 
	
	qcloud_iot_utils_base64encode(base64out,128,&recnum,AESoutput,32);
	
}

#define HTTP_BODY_LENGTH 1024
void HTTP_body(bool reporttype,uint8_t NumOfTransformer,char *cJSONout)
{
		char *cJSONbuffer=NULL;
		uint8_t calculatebuffer[44];
		HTTP_header_encode(calculatebuffer);
	
		cJSONbuffer=(char*)malloc(HTTP_BODY_LENGTH);//最大字节数据
		
		memset(cJSONbuffer,0,HTTP_BODY_LENGTH);
		
		memset(cJSONbuffer,0,HTTP_BODY_LENGTH);
		snprintf(cJSONbuffer,HTTP_BODY_LENGTH,"{\"deviceId\":\"%s\",\"version\":\"%s\",\"imei\":\"%s\",\"ccid\":\"%s\"}",ChipIDstring+12,Version,mode_imei+3,card_ccid);
		SEGGER_RTT_printf2("%s",cJSONbuffer);
		memcpy(cJSONout,cJSONbuffer,(strlen(cJSONbuffer)));
		free(cJSONbuffer);

}


//extern bool feeddog;


uint8_t Cmd(char *cmd,char *reply1,char *reply2, char *RXBuffer)
{   //指令发送
    unsigned long ulTick = 0;
	unsigned long timeout = 1000*60;
	memset(RXBuffer,0,512); //清空接收缓冲

    g_ATModule.atConfig.atPort->fpUartSend(cmd,strlen(cmd));

    if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
        return true;
	ulTick = GetTimeTick();
    while(GetTimeTick() - ulTick < timeout)			
    {
        if(g_ATModule.atConfig.atPort->fpUartRead(RXBuffer,800) == 0)				//接收没有结束，跳过下面判断 ,接收为真后，进行下面判断
        {
            continue;
        }
        if((strstr((char *)RXBuffer, reply2) != NULL)&&(reply2!=NULL))		//接受的错误响应，返回假
        {
            return false ;
        }
        if(strstr((char *)RXBuffer, reply1) != NULL)		  				//接收到正确信息，返回真
        {
            SEGGER_RTT_printf2("Cmd ACK--->%s\r\n",RXBuffer);
			return true ;
        }
    }
    return false ;															//等待结束，没有收到信息，返回假
}

//WXF 20220724
uint8_t Cmd_bytes(char *cmd,u16 data_len,char *reply1,char *reply2, char *RXBuffer)
{   //指令发送
    unsigned long ulTick = 0;
	unsigned long timeout = 1000*60;
	memset(RXBuffer,0,512); //清空接收缓冲

    g_ATModule.atConfig.atPort->fpUartSend(cmd,data_len);

    if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
        return true;
		ulTick = GetTimeTick();
    while(GetTimeTick() - ulTick < timeout)			
    {
        if(g_ATModule.atConfig.atPort->fpUartRead(RXBuffer,800) == 0)				//接收没有结束，跳过下面判断 ,接收为真后，进行下面判断
        {
            continue;
        }
        if((strstr((char *)RXBuffer, reply2) != NULL)&&(reply2!=NULL))		//接受的错误响应，返回假
        {
            return false ;
        }
        if(strstr((char *)RXBuffer, reply1) != NULL)		  				//接收到正确信息，返回真
        {
            SEGGER_RTT_printf2("Cmd ACK--->%s\r\n",RXBuffer);
			return true ;
        }
    }
    return false ;															//等待结束，没有收到信息，返回假
}


//动态注册函数
bool DYNAMIC_PORT( char *rec_buff)
{
	SEGGER_RTT_printf2("DYNAMIC_PORT!!!\r\n");
    //@state machine contral.
  uint8_t Init_Cmd_Status = 0;
	
	//@command buffer.	
  char cmd_buff[256];		
	char sendbuff[150];	
	size_t bodylen;
	SEGGER_RTT_printf2("DYNAMIC_PORT1!!!\r\n");	
	//@return the run state.
  bool status_return=false;   
	//@state machine Command retry flag times.
	uint8_t check_Cmd_at_retry=0;	
  char *cat,*cat1;//小心这个位置
	cat=(char*)malloc(512);
	cat1=(char*)malloc(512);
	SEGGER_RTT_printf2("DYNAMIC_PORT2!!!\r\n");
  uint8_t var;  
	while(1)
	{

    //@the state machine 
			switch(Init_Cmd_Status)
			{
		
				case 0:
				
					status_return = Cmd("AT+QICSGP=1,1,\"cmnet\",\"\",\"\"\r\n", "OK", "ERROR", rec_buff);
				
					if(false == status_return)
						
						check_Cmd_at_retry++;		
				
					else
					{	
						Init_Cmd_Status++;	   		
						status_return=0;
					}	
					if(check_Cmd_at_retry>3)
					{
						
						check_Cmd_at_retry = 0;
						free(cat);
						free(cat1);
						return status_return;
					}
				
				break ;
					
				case 1:
					
					status_return = Cmd("AT$HTTPOPEN\r\n", "OK", 0, rec_buff);	
					
					if(false == status_return)
						check_Cmd_at_retry++;	  //@retry.
					else
					{	
						SEGGER_RTT_printf2("HTTPOPEN SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}		
				
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("HTTPOPEN FAIL!!!\r\n");
						check_Cmd_at_retry = 0;	
						free(cat);
						free(cat1);
						return status_return;
					}	
				break ;
				
				case 2:
//					status_return = Cmd("AT$HTTPPARA=https://lxdlyc.com/inter/auth,80,0,0\r\n","OK","ERROR",rec_buff);
					status_return = Cmd("AT$HTTPPARA=http://dici.shengzhouchengtou.com/v1/device/inter/auth,8888,0,0\r\n","OK","ERROR",rec_buff);
					if(false == status_return)
						check_Cmd_at_retry++;	  
					else
					{	
						Init_Cmd_Status++;	   		
						status_return=0;
						SEGGER_RTT_printf2("HTTPPARA SUCESS!!!\r\n");
					}		
					if(check_Cmd_at_retry>3)
					{
						
						check_Cmd_at_retry = 0;	
						SEGGER_RTT_printf2("HTTPPARA FAIL!!!\r\n");
						free(cat);
						free(cat1);
						return status_return;
					}
			break ;
					
			case 3:
			SEGGER_RTT_printf2("case 3!!!\r\n");
			memset(cmd_buff,0,sizeof(cmd_buff));
      memset(sendbuff,0,sizeof(sendbuff));
			HTTP_body(1,3,sendbuff);
			bodylen=strlen(sendbuff);
			snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPRQH=Content-Length,%d\r\n",bodylen);
      status_return = Cmd(cmd_buff, "OK", "ERROR", rec_buff);		//设置发送长度
      if(false == status_return)
          check_Cmd_at_retry++;			//AT发送失败
      else
				Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
      if(check_Cmd_at_retry>3)
      {
       check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
				free(cat);
				free(cat1);
       return status_return;
      }
      break ;
						
			case 4:
					SEGGER_RTT_printf2("case 4!!!\r\n");
            status_return = Cmd("AT$HTTPRQH=Content-Type,application/json\r\n", "OK", "ERROR", rec_buff);		//设置发送长度
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
								Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
								SEGGER_RTT_printf2("HTTPRQH FAIL!!!\r\n");
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
								free(cat);
								free(cat1);
                return status_return;
            }
            break ;
		  	case 5:
            status_return = Cmd("AT$HTTPRQH=Connection,keep-alive\r\n", "OK", "ERROR", rec_buff);		//设置发送长度
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
								Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
								free(cat);
								free(cat1);
                return status_return;
            }
            break ;
					case 6:
            status_return = Cmd("AT$HTTPRQH=User-Agent,PostmanRuntime/7.29.0\r\n", "OK", "ERROR", rec_buff);		//设置发送长度
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
							Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
								free(cat);
								free(cat1);
                return status_return;
            }
            break ;	
			
				case 7: 
					status_return = Cmd("AT$HTTPACTION=1\r\n", "OK", "ERROR", rec_buff);		
					
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						SEGGER_RTT_printf2("HTTPACTION SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}			
			
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("HTTPACTION FAIL!!!\r\n");
						check_Cmd_at_retry = 0;		
						free(cat);
						free(cat1);
						return status_return;
					}
					
				break ;
				
				case 8:  
	//@Send "AT$HTTPDATA=%d\r\n"   send body
					memset(cmd_buff,0,sizeof(cmd_buff));
					
					memset(sendbuff,0,sizeof(sendbuff));
					HTTP_body(1,3,sendbuff);
					bodylen=strlen(sendbuff);
				
					snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPDATA=%d\r\n",bodylen);
					
					status_return = Cmd(cmd_buff, ">>", "ERROR", rec_buff);		
					
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						SEGGER_RTT_printf2("HTTPDATA SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}				
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("HTTPDATA FAIL!!!\r\n");
						check_Cmd_at_retry = 0;
						free(cat);
						free(cat1);
						return status_return;
					}
				break ;
				
				case 9:
	//@send body reply
					status_return = Cmd(sendbuff, "OK", "ERROR", rec_buff);		
					
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						SEGGER_RTT_printf2("sendbuff SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}		
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("sendbuff FAIL!!!\r\n");
						check_Cmd_at_retry = 0;	
						free(cat);
						free(cat1);
						return status_return;
					}
			break ;
			
			case 10:
					memset(cmd_buff,0,sizeof(cmd_buff));
					status_return = Cmd("AT$HTTPSEND\r\n", "OK", "ERROR", rec_buff);		
					
					if(false == status_return)
						check_Cmd_at_retry++;		
					else
					{	
						Init_Cmd_Status++;	   		
						status_return=0;
					}
					if(check_Cmd_at_retry>3)
					{
						check_Cmd_at_retry = 0;	
						free(cat);
						free(cat1);
						return status_return;
					}
			break ;
			
			case 11:
					status_return = Cmd("AT$HTTPDATA=0\r\n", "OK", "ERROR", rec_buff);		
					
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						SEGGER_RTT_printf2("CASE11 SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}			
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("CASE11 FAIL!!!\r\n");
						check_Cmd_at_retry = 0;			
						free(cat);
						free(cat1);
						return status_return;
					}
					
			break ;
			
			case 12:
					status_return = Cmd("AT$HTTPSEND\r\n", "$HTTPRECV", "ERROR", rec_buff);		
					SEGGER_RTT_printf2("CASE12 rec_buff:%s!!!\r\n",rec_buff);
					memset(cat,0,512);
//						cat=strstr(rec_buff,"success");
						cat=strstr(rec_buff,"code\":200");
						if(cat!=NULL)
						{
							//@get the server key.
//							uint32_t keybuffer=0;
//							uint32_t urlbuffer,urladdbuff;
//							urladdbuff=addr_url-4;
//							urlbuffer=0;
//							size_t urlsize=0;
							var=0;
//							char url[256];
							cat=strstr(rec_buff,"key")+6;	
							memset(mqtt_passware,0,64);
							memcpy(mqtt_passware,cat,32);
							for(var=4;var>=1;var--)
							{
								Certification.key[var-1]=*(cat+var-1);
							}
							SEGGER_RTT_printf2("--->Certification.key:%s\r\n",mqtt_passware);
							/*
							delay_ms(1);
							uint32_t *buf=NULL;
							buf=&keybuffer;
							Bitmerge(1,Certification.key,buf);
							delay_ms(1);
							clea_e2prom(addr_server_key);
							lv_write_e2prom(addr_Factory_set_flag,0xAAFFAAFF);
							lv_write_e2prom(addr_server_key,keybuffer);//服务器密钥
							
							lv_read_e2prom(addr_server_key,buf);
							uint8_t versionbuf[8];
							char *buffer=NULL;
							buffer=strstr(rec_buff,"version\":")+9;
							memcpy(versionbuf,buffer,7);
							uint8_t i=0;
							i=compareversion((char*)versionbuf);//需要完善
							if(i==1)
							{
								cat=strstr(rec_buff,"true");
							//@get the upgrade:true.OTA  
							if(cat!=NULL)
							{
							//@URL max length less than 254.
									cat=strstr(rec_buff,"url");
									cat+=6;
									cat1=strstr(cat,"\"");
									urlsize=cat1-cat;
									memset(url,0,sizeof(url));
									memcpy(url,cat,urlsize);
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
									for(var=1;var<urlsize%4+1;var++)
									{			
												urlbuffer|=url[urlsize-var];
												if(var!=urlsize%4)
												urlbuffer<<=8;
									}
									delay_ms(1);
												lv_write_e2prom(urladdbuff+4,urlbuffer);		
							memset(cat,0,512);
							cat=strstr(rec_buff,"md5asum");
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
							rcu_periph_clock_enable(RCU_WWDGT);
							wwdgt_config(127, 80, WWDGT_CFG_PSC_DIV8);
							wwdgt_enable();
							}
						
						}
			*/
					}
					else
					{
						SEGGER_RTT_printf2("cat is NULL!!!\r\n");
					}
					if(false == status_return)
						check_Cmd_at_retry++;			
					
					else
					{
						SEGGER_RTT_printf2("CASE12 SUCESS!!!\r\n");
						Init_Cmd_Status++;	
						free(cat);
						free(cat1);
						return status_return;
					}
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("CASE12 fail!!!\r\n");
						check_Cmd_at_retry = 0;		
						free(cat);
						free(cat1);
						return status_return;
					}
					
			break ;
			
			default:
				free(cat);
						free(cat1);
			
					return false;
			
		}
	}
}

#define BOUNDARY_STRING "--------------------ser13824302385wxf"

#define HTTP_TRANCE_LEN 1000
bool http_upload_file(char *filename)
{
	SEGGER_RTT_printf2("http_upload_file!!!\r\n");
    //@state machine contral.
  uint8_t Init_Cmd_Status = 0;
	char rec_buff[800];
	//@command buffer.	
  char cmd_buff[256];		
	char sendbuff[1024];	
	FIL img_file;
	FRESULT res;
	UINT  bw;
	size_t bodylen;
	u32 filesize=0;
	//@return the run state.
  bool status_return=false;   
		
	u16 tran_times=filesize/HTTP_TRANCE_LEN+1;
	u16 remain_length=filesize%HTTP_TRANCE_LEN;
	u16 tran_length=0;
	if(remain_length==0)
		tran_times--;
	//@state machine Command retry flag times.
	uint8_t check_Cmd_at_retry=0;	
  char *cat,*cat1;//小心这个位置
	cat=(char*)malloc(512);
	cat1=(char*)malloc(512);
  uint8_t var; 
	res = f_open(&img_file, filename, FA_OPEN_EXISTING | FA_READ);
	if(res)
	{
		SEGGER_RTT_printf2("open file fail res:%d!!!...\r\n",res); //提示正在传输数据		
	}
	f_close(&img_file);
	filesize=f_size(&img_file);
	while(1)
	{

    //@the state machine 
			switch(Init_Cmd_Status)
			{
						
				case 0://设置HTTP请求地址和端口
						status_return = Cmd("AT$HTTPPARA=http://dici.shengzhouchengtou.com/v1/device/upload,8888,0,0\r\n","OK","ERROR",rec_buff);
						if(false == status_return)
							check_Cmd_at_retry++;	  
						else
						{	
							Init_Cmd_Status++;	   		
							status_return=0;
							SEGGER_RTT_printf2("HTTPPARA SUCESS!!!\r\n");
						}		
						if(check_Cmd_at_retry>3)
						{
							
							check_Cmd_at_retry = 0;	
							SEGGER_RTT_printf2("HTTPPARA FAIL!!!\r\n");
							free(cat);
							free(cat1);
							return status_return;
						}
			break ;
					
			case 1://设置请求头,secret
					memset(cmd_buff,0,sizeof(cmd_buff));
					snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPRQH=secret,%s\r\n",mqttobject.Password);
					status_return = Cmd(cmd_buff, "OK", "ERROR", rec_buff);		//设置发送长度
					if(false == status_return)
					{
							SEGGER_RTT_printf2("AT$HTTPRQH=secret Faile!!!\r\n");
							check_Cmd_at_retry++;			//AT发送失败
					}
					else
					{
						SEGGER_RTT_printf2("AT$HTTPRQH=secret SUCESS!!!\r\n");
						Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
					}
					if(check_Cmd_at_retry>3)
					{
					 check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
						free(cat);
						free(cat1);
					 return status_return;
					}
					break ;
			
			case 2://设置请求头，模拟postman
					status_return = Cmd("AT$HTTPRQH=User-Agent,PostmanRuntime/7.29.0\r\n", "OK", "ERROR", rec_buff);		//
					if(false == status_return)
							check_Cmd_at_retry++;			//AT发送失败
					else
					{
						SEGGER_RTT_printf2("AT$HTTPRQH=User-Agent,PostmanRuntime SUCESS!!!\r\n");
						Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
					}
					if(check_Cmd_at_retry>3)
					{
							check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
							free(cat);
							free(cat1);
							return status_return;
					}
					break ;	
					
			case 3://设置请求头
					memset(cmd_buff,0,sizeof(cmd_buff));
//					snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPRQH=Content-Type,multipart/form-data; boundary=%s\r\n",BOUNDARY_STRING);
					snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPRQH=Content-Type,multipart/form-data\r\n");
					SEGGER_RTT_printf2("case 3 cmd_buff:%s!!!\r\n",cmd_buff);
					status_return = Cmd(cmd_buff, "OK", "ERROR", rec_buff);		//设置发送长度
					if(false == status_return)
					{
						SEGGER_RTT_printf2("AT$HTTPRQH=form-data ERROR:%s!!!\r\n",rec_buff);
							check_Cmd_at_retry++;			//AT发送失败
					}
					else
					{
							SEGGER_RTT_printf2("AT$HTTPRQH=ultipart/form-data SUCESS!!!\r\n");
							Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
					}
					if(check_Cmd_at_retry>3)
					{
							SEGGER_RTT_printf2("HTTPRQH FAIL!!!\r\n");
							check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
							free(cat);
							free(cat1);
							return status_return;
					}
					break ;
						
			case 4://设置表格格式发送请求头
					memset(cmd_buff,0,sizeof(cmd_buff));
//					snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPRQH=boundary=%s\r\n",BOUNDARY_STRING);
					snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPRQH=boundary,%s\r\n",BOUNDARY_STRING);
//					snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPRQH=Content-Type,multipart/form-data\r\n");
					SEGGER_RTT_printf2("case 4 cmd_buff:%s!!!\r\n",cmd_buff);
					status_return = Cmd(cmd_buff, "OK", "ERROR", rec_buff);		//设置发送长度
					if(false == status_return)
					{
						SEGGER_RTT_printf2("AT$HTTPRQH=boundary,%s!!!\r\n",rec_buff);
							check_Cmd_at_retry++;			//AT发送失败
					}
					else
					{
							SEGGER_RTT_printf2("AT$HTTPRQH=boundary SUCESS!!!\r\n");
							Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
					}
					if(check_Cmd_at_retry>3)
					{
							SEGGER_RTT_printf2("HTTPRQH FAIL!!!\r\n");
							check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
							free(cat);
							free(cat1);
							return status_return;
					}
					break ;
		  	case 5://设置请求头
            status_return = Cmd("AT$HTTPRQH=Connection,keep-alive\r\n", "OK", "ERROR", rec_buff);		
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
						{
								SEGGER_RTT_printf2("AT$HTTPRQH=Connection,keep-alive SUCESS!!!\r\n");
								Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
						}
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
								free(cat);
								free(cat1);
                return status_return;
            }
            break ;
				case 6://请求头
						memset(cmd_buff,0,sizeof(cmd_buff));
            snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPRQH=Content-Length,%d-10\r\n",filesize+strlen(BOUNDARY_STRING)*2+155+strlen(filename));
						status_return = Cmd(cmd_buff, "OK", "ERROR", rec_buff);		//设置发送长度
						if(false == status_return)
								check_Cmd_at_retry++;			//AT发送失败
						else
						{
							SEGGER_RTT_printf2("AT$HTTPRQH=Content-Length,%d SUCESS!!!\r\n",filesize+strlen(BOUNDARY_STRING)*2+155+strlen(filename));
							Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
						}
						if(check_Cmd_at_retry>3)
						{
						 check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
							free(cat);
							free(cat1);
						 return status_return;
						}
						break ;
			
				case 7: //发送HTTP请求头
					status_return = Cmd("AT$HTTPACTION=1\r\n", "OK", "ERROR", rec_buff);							
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						SEGGER_RTT_printf2("HTTPACTION SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}			
			
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("HTTPACTION FAIL!!!\r\n");
						check_Cmd_at_retry = 0;		
						free(cat);
						free(cat1);
						return status_return;
					}
					
				break ;
				
				case 8: //发送boundary_string和标志数据的总长度 
					memset(cmd_buff,0,sizeof(cmd_buff));	
					memset(sendbuff,0,sizeof(sendbuff));
					snprintf(sendbuff,sizeof(sendbuff),"\r\n%s\r\nContent-Disposition: form-data; name=\"deviceSn\"\r\n\r\n%s\r\n%s\r\nContent-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\nContent-Type: image/jpeg\r\n",BOUNDARY_STRING,mode_imei+3,BOUNDARY_STRING,filename);
					snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPDATA=%d\r\n",strlen(sendbuff));					
					status_return = Cmd(cmd_buff, ">>", "ERROR", rec_buff);							
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						SEGGER_RTT_printf2("HTTPDATA SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}				
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("HTTPDATA FAIL!!!\r\n");
						check_Cmd_at_retry = 0;
						free(cat);
						free(cat1);
						return status_return;
					}
				break ;
				
				case 9://发送boundary_string和标志数据
	//@send body reply
					status_return = Cmd(sendbuff, "OK", "ERROR", rec_buff);		
					
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						SEGGER_RTT_printf2("sendbuff SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}		
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("sendbuff FAIL!!!\r\n");
						check_Cmd_at_retry = 0;	
						free(cat);
						free(cat1);
						return status_return;
					}
			break ;
			
			case 10://开启发送命令
					status_return = Cmd("AT$HTTPSEND\r\n", "OK", "ERROR", rec_buff);							
					if(false == status_return)
						check_Cmd_at_retry++;		
					else
					{	
						Init_Cmd_Status++;	   		
						status_return=0;
					}
					if(check_Cmd_at_retry>3)
					{
						check_Cmd_at_retry = 0;	
						free(cat);
						free(cat1);
						return status_return;
					}
				break ;
			case 11://OPEN FILE
				res = f_open(&img_file, filename, FA_OPEN_EXISTING | FA_READ);
				if(res)
				{
					SEGGER_RTT_printf2("open file fail res:%d!!!...\r\n",res); //提示正在传输数据
					f_close(&img_file);
				}
				else
				{
					Init_Cmd_Status++;	   		
				}
					
			case 12://读取文件
					watch_time_tick=0;
					if(tran_times-->0)
						tran_length=HTTP_TRANCE_LEN;				
					else
						tran_length=remain_length;
					
					memset(cmd_buff,0,sizeof(cmd_buff));	
					snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPDATA=%d\r\n",tran_length);					
					status_return = Cmd(cmd_buff, ">>", "ERROR", rec_buff);							
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						//SEGGER_RTT_printf2("HTTPDATA SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}				
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("HTTPDATA FAIL!!!\r\n");
						check_Cmd_at_retry = 0;
						free(cat);
						free(cat1);
						return status_return;
					}
					break ;

			case 13:
					memset(sendbuff,0,sizeof(sendbuff));
					res = f_read(&img_file, (u8*)sendbuff, tran_length, &bw); 
					status_return = Cmd_bytes(sendbuff,tran_length, "OK", "ERROR", rec_buff);						
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						//SEGGER_RTT_printf2("sendbuff SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}		
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("sendbuff FAIL!!!\r\n");
						check_Cmd_at_retry = 0;	
						free(cat);
						free(cat1);
						return status_return;
					}
			break ;
					
			case 14://开启发送命令
					status_return = Cmd("AT$HTTPSEND\r\n", "OK", "ERROR", rec_buff);							
					if(false == status_return)
						check_Cmd_at_retry++;		
					else
					{	
						if(tran_times==0)
						{
							Init_Cmd_Status++;	   		
							status_return=0;
						}
						else
						{
							Init_Cmd_Status=11;	   		
							status_return=0;
						}
					}
					if(check_Cmd_at_retry>3)
					{
						check_Cmd_at_retry = 0;	
						free(cat);
						free(cat1);
						return status_return;
					}
				break ;
				
			
			case 15://标志发送已完成
					status_return = Cmd("AT$HTTPDATA=0\r\n", "OK", "ERROR", rec_buff);		
					
					if(false == status_return)
						check_Cmd_at_retry++;			
					else
					{	
						SEGGER_RTT_printf2("CASE15 SUCESS!!!\r\n");
						Init_Cmd_Status++;	   		
						status_return=0;
					}			
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("CASE11 FAIL!!!\r\n");
						check_Cmd_at_retry = 0;			
						free(cat);
						free(cat1);
						return status_return;
					}
					
			break ;
			
			case 16:
					status_return = Cmd("AT$HTTPSEND\r\n", "$HTTPRECV", "ERROR", rec_buff);		
					SEGGER_RTT_printf2("CASE15 rec_buff:%s!!!\r\n",rec_buff);
					memset(cat,0,512);
					cat=strstr(rec_buff,"code\":200");
					if(cat!=NULL)
					{
						SEGGER_RTT_printf2("code:200\r\n");

					}
					else
					{
						SEGGER_RTT_printf2("not 200!!!\r\n");
					}
					if(false == status_return)
						check_Cmd_at_retry++;			
					
					else
					{
						SEGGER_RTT_printf2("CASE15 SUCESS!!!\r\n");
						Init_Cmd_Status++;	
						free(cat);
						free(cat1);
						return status_return;
					}
					
					if(check_Cmd_at_retry>3)
					{
						SEGGER_RTT_printf2("CASE15 fail!!!\r\n");
						check_Cmd_at_retry = 0;		
						free(cat);
						free(cat1);
						return status_return;
					}
					
			break ;
			
			default:
				free(cat);
						free(cat1);
			
					return false;
			
		}
	}
}




