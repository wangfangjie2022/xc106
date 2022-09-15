#include "qcloud_dynreg.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "at_port.h"
#include "uart_timer.h"
#include "flash_e2prom.h"
#include "sys.h"
#include "CJSON.h"
#include "utils_base64.h"
#include "utils_hmac.h"
#include "utils_aes.h"

//后台登陆密钥
const unsigned char key[] = {"5e8430dc190c3608"};	 //从flash里面取

//固定HTTP头文件
const unsigned char HTTPfixheader[]=	{"bearer/key"}; 			 //HTTP_header

/* Max size of base64 encoded PSK = 64, after decode: 64/4*3 = 48*/
#define DECODE_PSK_LENGTH 1

#define MAX_CONN_ID_LEN (6)

#define cur_timestamp 	(1690577219)								//no use 


//服务器密钥
uint8_t Server_key[10]={0};



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
        if(g_ATModule.atConfig.atPort->fpUartRead(RXBuffer,512) == 0)				//接收没有结束，跳过下面判断 ,接收为真后，进行下面判断
        {
            continue;
        }
        if((strstr((char *)RXBuffer, reply2) != NULL)&&(reply2!=NULL))		//接受的错误响应，返回假
        {
            return false ;
        }
        if(strstr((char *)RXBuffer, reply1) != NULL)		  				//接收到正确信息，返回真
        {
            printf("%s",RXBuffer);
			return true ;
        }
    }
    return false ;															//等待结束，没有收到信息，返回假
}



char num_hext2str(uint8_t num)
{
	switch (num)
	{
	case 1:	return'1'; break;
	case 2: return'2'; break;
	case 3: return'3'; break;		 
	default:	break;
	return 0;
	}
}



void HTTP_header_encode(char *data,char datalength,unsigned char *calcbuffer)
{
	utils_aes_context Uctx;						//创建一个对象
	utils_aes_setkey_enc(&Uctx,key, AES_KEY_BITS_128);	//输入AES密钥
	
	//合并字符 
	uint8_t var;char cmd_buff_source[32];
	for(var=0;var<12;var++)
	{
		snprintf(cmd_buff_source,sizeof(cmd_buff_source),"bearer/key%d",ChipID[var]);
  }
	
	//计算ecb
	calc_ecb(cmd_buff_source,sizeof(cmd_buff_source),calcbuffer);
}



//需要修改变压器个数和ID
void HTTP_body(bool reporttype,uint8_t NumOfTransformer)
{
		
	
	char buffer[]="{\"deviceId\":\"";
		uint8_t var;	

		//{"deviceId": "XXXXXXXXXXXX",
		for(var=0;var<12;var++)
		{
		strcat(buffer,ChipID[var]);
		}
		strcat(buffer,"\",");
		//"dtuMode": true,
		if(reporttype)
				strcat(buffer,"\"dtuMode\":true");
		else
				strcat(buffer,"\"dtuMode\":false");
		//"voltNum": 3}	
		char num;
		num=num_hext2str(NumOfTransformer);
		strcat(buffer,"\"voltNum\":");
		strcat(buffer,&num);
		strcat(buffer,"}");
}


/*******************************************************/



uint8_t qcloud_dynreg_sign( char *buff, uint16_t buff_len, char *buff_out, uint16_t len_out)
{
	memset(buff,0,buff_len);

	//@debug
//	if(	qcloud_iot_utils_base64encode((unsigned char *)buff, 256, &receiv_olen, (unsigned char*)buff_out,strlen(buff_out)))	
//	{
//		printf("Base64_Error\r\n");
//		return 1;
//	}
	
/*	@HTTP header
		@content-type=appliation/json(可以去掉)
		@Author=xxx  格式：bearer/key+sn号 AES加密后转base64 
		@encode the header data;   */
	
	HTTP_header_encode(buff,buff_len,buff_out);

/*	
		@HTTP body
		{
				"deviceId": "123456",
				"dtuMode": true,
				"voltNum": 3
		}
		需要修改成变量
*/
HTTP_body(1,3);

//@state machine 	
DYNAMIC_PORT();

//@debug
//	if(strlen(buff_out) == 0)
//	{
//		return 2;
//	}
	return 0;
}







/*
clint (sn)(HTTP)- >  					server 
clint <-(certificate)(HTTP)		server 	

clint (certificate)(MQTT)->		server	
*/


//对动态注册的包处理修改
bool DYNAMIC_PORT(char *header,char *body, char *rec_buff)
{
	char cmd_buff[128];								//命令缓存	
	char cmd_buff_encode[128];				//命令编码缓存
	uint8_t var;
	uint8_t calcbuffer[256]={0};
	
	uint8_t status_return = false ; 	//返回状态
  uint8_t Init_Cmd_Status = 0;			//设置body解析流程状态机初始状态
  uint8_t check_Cmd_at_retry = 0;		//重复检测AT指令

	//@data length :strlen(calcbuffer);
	
	while(1)
    {
				//动态注册状态机
        switch(Init_Cmd_Status)
        {
            //获取MAC修改连接密码
        case 0:
            status_return = Cmd("AT+QICSGP=1,1,\"cmnet\",\"\",\"\"\r\n", "OK", "ERROR", rec_buff);		//连接网络
            if(false == status_return)
                check_Cmd_at_retry++;		//AT发送失败
            else
						Init_Cmd_Status++;			//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;	//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 1:
            status_return = Cmd("AT$HTTPOPEN\r\n", "OK", 0, rec_buff);		//连接网络
            if(false == status_return)
                check_Cmd_at_retry++;		//AT发送失败
            else
						Init_Cmd_Status++;			//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;	//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 2:
            status_return = Cmd("AT$HTTPPARA=http://121.41.77.2//inter/auth,80,0,0\r\n", "OK", "ERROR", rec_buff);		//设置URL
            if(false == status_return)
                check_Cmd_at_retry++;		//AT发送失败
            else
						Init_Cmd_Status++;			//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;	//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 3:
			memset(cmd_buff,0,sizeof(cmd_buff));//  /"Author/"
		//AES_ECB: x/128+128*bool(x%128)				(bit) 			(x>16)
		//base64 conver:[(x*8/6)*2+x*8]/8 			(bit)
		
		//cat the encode data to de string.
		for(var=0;var<(uint8_t)strlen(calcbuffer);var++)
		{
			snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPRQH=\"Author\",%d",calcbuffer[var]);
		}	
			snprintf(cmd_buff,sizeof(cmd_buff),"\r\n");
	
			status_return = Cmd(cmd_buff, "OK", "ERROR", rec_buff);		//设置发送长度
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
						Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 4://连接HTTP服务器，设置保活心跳
            status_return = Cmd("AT$HTTPRQH=Connection,keep-alive\r\n", "OK", "ERROR", rec_buff);		//设置发送长度
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
				Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 5: //设置HTTP连接的请求位POST
            status_return = Cmd("AT$HTTPACTION=1\r\n", "OK", "ERROR", rec_buff);		
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
				Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 6: //发送POST请求 
						memset(cmd_buff,0,sizeof(cmd_buff));
						snprintf(cmd_buff,sizeof(cmd_buff),"AT$HTTPDATA=%d\r\n",strlen(body));
            status_return = Cmd(cmd_buff, ">>", "ERROR", rec_buff);		
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
				Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 7://发送数据
            status_return = Cmd(body, "OK", "ERROR", rec_buff);		
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
				Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 8://查询是否发送完成
            status_return = Cmd("AT$HTTPSEND\r\n", "OK", "ERROR", rec_buff);		
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
				Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 9://
            status_return = Cmd("AT$HTTPDATA=0\r\n", "OK", "ERROR", rec_buff);		
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
				Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
		case 10:
            status_return = Cmd("AT$HTTPSEND\r\n", "$HTTPRECV", "ERROR", rec_buff);		
            if(false == status_return)
                check_Cmd_at_retry++;			//AT发送失败
            else
						{
								Init_Cmd_Status++;				//AT发送成功，进入到下一个状态
								return status_return;
						}
            if(check_Cmd_at_retry>3)
            {
                check_Cmd_at_retry = 0;			//超过失败三次，出现错误，直接跳出
                return status_return;
            }
            break ;
        default:
            return false;
        }
	}
}




//@frist turn on use it. Read is error.
uint8_t read_password(void)
{
		//@value
		uint8_t var=0,buffer=0;
		lv_read_e2prom (addr_server_key,10,Server_key);
		
		for(var=0;var<10;var++)
		{
		//@byte shrinkage.
		buffer=*(Server_key+var);	
		if(buffer==0xFF)
				return 1;
		else
				return 0;
		}
}


//@save the password data.
uint8_t save_password(void)
{
			//@write the server password.
			lv_write_e2prom (addr_server_key,10,Server_key);
			return 0;
}





//动态注册，录入设备
uint8_t get_code(DevPart_t *DevPart)
{
	char buff[265];			
	char rec_buff[512];		
	size_t receiv_olen = 0;
  uint8_t var=0;
  memset(buff,0,sizeof(buff));
	memset(rec_buff,0,sizeof(rec_buff));

	
	utils_aes_context Uctx;		
	
	qcloud_dynreg_sign(DevPart, rec_buff, sizeof(rec_buff), buff, sizeof(buff));
	memset(rec_buff,0,sizeof(rec_buff));
	
	//动态注册状态机
	DYNAMIC_PORT(buff, rec_buff);
	
	
	char *str_addr = 0;																				
	str_addr = strstr(rec_buff,"success");	
	//@return string "data":"XXXXXXXXXX"
	str_addr = strstr(rec_buff,"a\":\"");
	//@point to the password.
	str_addr+=5;
	for(var=0;var<10;var++)
	{
		Server_key[var] = *str_addr;
	}
	bool i=read_password();
	//@if not password save it. 
	if(i)
	{
		save_password();
	}

	return 0;
}

//@Calculate the ECB data .
void calc_ecb(char *data,char datalength,unsigned char *calcbuffer)
{
	
	uint8_t recnum=0;
	
	//@Var use the datalength
	uint8_t var;  var=datalength%16;
	//@Get a place to save the data.
	
	//@Creat a utils_aes obkect.set the object property.
	utils_aes_context buffer;	buffer.nr=1;unsigned char outbuffer[16]={0};
	
	//use for the Base64
	unsigned char base64buffer[512]={0};
	
	//@Set aes_enc key.
	utils_aes_setkey_enc(&buffer, key, AES_KEY_BITS_128);
	
	if(var!=0){
		#if Pkcs7
		//@Pkcs7
 		while(var--)
		{
			//@Padding the data buffer.
			*(data+datalength-var)=0x07; 	
		}
		#else
		//@Pkcs5
		while(var--)
		{
			*(data+datalength-var)=0x07;	
		}
		#endif
	}
	//@calculate the datalength after padding.
	datalength=(datalength+(bool)var)*16;
	
	//need fix
	
	//@get the code. 
	utils_aes_crypt_ecb(&buffer,UTILS_AES_ENCRYPT,(unsigned char)data,(unsigned char)outbuffer); 
	
	//@ready to encode the AES_data.
	memcpy(base64buffer,outbuffer,16*(sizeof(unsigned char)));		
	
	//@base64 encode. 
	qcloud_iot_utils_base64encode(calcbuffer,256,&recnum,base64buffer,datalength);
}

