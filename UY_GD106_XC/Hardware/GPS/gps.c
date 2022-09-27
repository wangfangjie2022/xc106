#include "gps.h" 
#include "delay.h" 								   						   
#include "stdio.h"	 
#include "stdarg.h"	 
#include "string.h"	 
#include "math.h"
#include "header.h"
//////////////////////////////////////////////////////////////////////////////////	 						  
////////////////////////////////////////////////////////////////////////////////// 	   
//���ڷ��ͻ����� 	
//__align(8) u8 GPS_TX_BUF[GPS_MAX_SEND_LEN]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ� 	 
u8 GPS_TX_BUF[GPS_MAX_SEND_LEN]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ� 	  
//���ڽ��ջ����� 	
u8 GPS_RX_BUF[GPS_MAX_RECV_LEN]; 				//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.

vu16 GPS_RX_STA=0; 

nmea_msg gpsx; 											//GPS��Ϣ
//__align(4) u8 dtbuf[50];   								//��ӡ������
u8 dtbuf[50];   								//��ӡ������
const u8*fixmode_tbl[4]={"Fail","Fail"," 2D "," 3D "};	//fix mode�ַ��� 
//��buf����õ���cx���������ڵ�λ��
//����ֵ:0~0XFE,����������λ�õ�ƫ��.
//       0XFF,�������ڵ�cx������							  
u8 NMEA_Comma_Pos(u8 *buf,u8 cx)
{	 		    
	u8 *p=buf;
	while(cx)
	{		 
		if(*buf=='*'||*buf<' '||*buf>'z')return 0XFF;//����'*'���߷Ƿ��ַ�,�򲻴��ڵ�cx������
		if(*buf==',')cx--;
		buf++;
	}
	return buf-p;	 
}
//m^n����
//����ֵ:m^n�η�.
u32 NMEA_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}
//strת��Ϊ����,��','����'*'����
//buf:���ִ洢��
//dx:С����λ��,���ظ����ú���
//����ֵ:ת�������ֵ
int NMEA_Str2num(u8 *buf,u8*dx)
{
	u8 *p=buf;
	u32 ires=0,fres=0;
	u8 ilen=0,flen=0,i;
	u8 mask=0;
	int res;
	while(1) //�õ�������С���ĳ���
	{
		if(*p=='-'){mask|=0X02;p++;}//�Ǹ���
		if(*p==','||(*p=='*'))break;//����������
		if(*p=='.'){mask|=0X01;p++;}//����С������
		else if(*p>'9'||(*p<'0'))	//�зǷ��ַ�
		{	
			ilen=0;
			flen=0;
			break;
		}	
		if(mask&0X01)flen++;
		else ilen++;
		p++;
	}
	if(mask&0X02)buf++;	//ȥ������
	for(i=0;i<ilen;i++)	//�õ�������������
	{  
		ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
	}
	if(flen>5)flen=5;	//���ȡ5λС��
	*dx=flen;	 		//С����λ��
	for(i=0;i<flen;i++)	//�õ�С����������
	{  
		fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
	} 
	res=ires*NMEA_Pow(10,flen)+fres;
	if(mask&0X02)res=-res;		   
	return res;
}	  							 
//����GPGSV��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p,*p1,dx;
	u8 len,i,j,slx=0;
	u8 posx;   	 
	p=buf;
	p1=(u8*)strstr((const char *)p,"$GPGSV");
	len=p1[7]-'0';								//�õ�GPGSV������
	posx=NMEA_Comma_Pos(p1,3); 					//�õ��ɼ���������
	if(posx!=0XFF)gpsx->svnum=NMEA_Str2num(p1+posx,&dx);
	for(i=0;i<len;i++)
	{	 
		p1=(u8*)strstr((const char *)p,"$GPGSV");  
		for(j=0;j<4;j++)
		{	  
			posx=NMEA_Comma_Pos(p1,4+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].num=NMEA_Str2num(p1+posx,&dx);	//�õ����Ǳ��
			else break; 
			posx=NMEA_Comma_Pos(p1,5+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx);//�õ��������� 
			else break;
			posx=NMEA_Comma_Pos(p1,6+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx);//�õ����Ƿ�λ��
			else break; 
			posx=NMEA_Comma_Pos(p1,7+j*4);
			if(posx!=0XFF)gpsx->slmsg[slx].sn=NMEA_Str2num(p1+posx,&dx);	//�õ����������
			else break;
			slx++;	   
		}   
 		p=p1+1;//�л�����һ��GPGSV��Ϣ
	}   
}
//����GPGGA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGGA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;   
//	p1=(u8*)malloc(1200);
	p1=(u8*)strstr((const char *)buf,"$GNGGA");
	posx=NMEA_Comma_Pos(p1,6);								//�õ�GPS״̬
	if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);	
	posx=NMEA_Comma_Pos(p1,7);								//�õ����ڶ�λ��������
	if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx); 
	posx=NMEA_Comma_Pos(p1,9);								//�õ����θ߶�
	if(posx!=0XFF)
	{
		gpsx->altitude=NMEA_Str2num(p1+posx,&dx); 
//		SEGGER_RTT_printf2("posx:%d,gpsx->high:%d\r\n",posx,gpsx->altitude);	
	}
//	free(p1);
}
//����GPGSA��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPGSA_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx; 
	u8 i;  
//	p1=(u8*)malloc(1200);	
	p1=(u8*)strstr((const char *)buf,"$GNGSA");
	posx=NMEA_Comma_Pos(p1,2);								//�õ���λ����
	if(posx!=0XFF)gpsx->fixmode=NMEA_Str2num(p1+posx,&dx);	
	for(i=0;i<12;i++)										//�õ���λ���Ǳ��
	{
		posx=NMEA_Comma_Pos(p1,3+i);					 
		if(posx!=0XFF)gpsx->possl[i]=NMEA_Str2num(p1+posx,&dx);
		else break; 
	}				  
	posx=NMEA_Comma_Pos(p1,15);								//�õ�PDOPλ�þ�������
	if(posx!=0XFF)gpsx->pdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,16);								//�õ�HDOPλ�þ�������
	if(posx!=0XFF)gpsx->hdop=NMEA_Str2num(p1+posx,&dx);  
	posx=NMEA_Comma_Pos(p1,17);								//�õ�VDOPλ�þ�������
	if(posx!=0XFF)gpsx->vdop=NMEA_Str2num(p1+posx,&dx);  
//	free(p1);

}
//����GPRMC��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPRMC_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;     
	u32 temp;	   
	float rs; 
//	p1=(u8*)malloc(1200);
	p1=(u8*)strstr((const char *)buf,"GNRMC");//"$GPRMC",������&��GPRMC�ֿ������,��ֻ�ж�GPRMC.
//	SEGGER_RTT_printf2("GNRMC:%s\r\n",p1);	
	posx=NMEA_Comma_Pos(p1,1);								//�õ�UTCʱ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);	 	//�õ�UTCʱ��,ȥ��ms
		gpsx->utc.hour=temp/10000;
		gpsx->utc.min=(temp/100)%100;
		gpsx->utc.sec=temp%100;	 	 
	}	
	posx=NMEA_Comma_Pos(p1,3);								//�õ�γ��
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->latitude=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	SEGGER_RTT_printf2("posx:%d,gpsx->latitude:%d\r\n",posx,gpsx->latitude);	
	posx=NMEA_Comma_Pos(p1,4);								//��γ���Ǳ�γ 
	if(posx!=0XFF)gpsx->nshemi=*(p1+posx);					 
 	posx=NMEA_Comma_Pos(p1,5);								//�õ�����
	if(posx!=0XFF)
	{												  
		temp=NMEA_Str2num(p1+posx,&dx);		 	 
		gpsx->longitude=temp/NMEA_Pow(10,dx+2);	//�õ���
		rs=temp%NMEA_Pow(10,dx+2);				//�õ�'		 
		gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//ת��Ϊ�� 
	}
	SEGGER_RTT_printf2("posx:%d,gpsx->longitude:%d\r\n",posx,gpsx->longitude);	
	posx=NMEA_Comma_Pos(p1,6);								//������������
	if(posx!=0XFF)gpsx->ewhemi=*(p1+posx);		 
	posx=NMEA_Comma_Pos(p1,9);								//�õ�UTC����
	if(posx!=0XFF)
	{
		temp=NMEA_Str2num(p1+posx,&dx);		 				//�õ�UTC����
		gpsx->utc.date=temp/10000;
		gpsx->utc.month=(temp/100)%100;
		gpsx->utc.year=2000+temp%100;	 	 
	} 
	
//	free(p1);
}
//����GPVTG��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void NMEA_GPVTG_Analysis(nmea_msg *gpsx,u8 *buf)
{
	u8 *p1,dx;			 
	u8 posx;    
	p1=(u8*)strstr((const char *)buf,"$GNVTG");							 
	posx=NMEA_Comma_Pos(p1,7);								//�õ���������
	if(posx!=0XFF)
	{
		gpsx->speed=NMEA_Str2num(p1+posx,&dx);
		if(dx<3)gpsx->speed*=NMEA_Pow(10,3-dx);	 	 		//ȷ������1000��
	}
}  
//��ȡNMEA-0183��Ϣ
//gpsx:nmea��Ϣ�ṹ��
//buf:���յ���GPS���ݻ������׵�ַ
void GPS_Analysis(nmea_msg *gpsx,u8 *buf)
{
	NMEA_GPGSV_Analysis(gpsx,buf);	//GPGSV����,(�ɼ�������Ϣ)
	NMEA_GPGGA_Analysis(gpsx,buf);	//GPGGA����,����λ��Ϣ��	
	NMEA_GPGSA_Analysis(gpsx,buf);	//GPGSA����,�� ��ǰ������Ϣ��
	NMEA_GPRMC_Analysis(gpsx,buf);	//GPRMC����,���Ƽ���λ��Ϣ���ݸ�ʽ��
	NMEA_GPVTG_Analysis(gpsx,buf);	//GPVTG����,�������ٶ���Ϣ��
	SEGGER_RTT_printf2("=====GPS_Analysis======\r\n");	
	float tp;	
	char* dtbuf;
	dtbuf=(char*)malloc(50);
	u16 data_len1=0,data_len2=0;
	if(gpsx->fixmode<2)														//��λ״̬
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx->fixmode]);	
	  SEGGER_RTT_printf2("%s\r\n",dtbuf);	
		free(dtbuf);
		return;
	}
//	sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx->fixmode]);	
//	  SEGGER_RTT_printf2("%s\r\n",dtbuf);	
	if(gps_mqtt_data_ready==GPS_MQTT_DATA_READY_YES)
	{
		SEGGER_RTT_printf2("gps_mqtt_data_ready==GPS_MQTT_DATA_READY_YES\r\n");	
		free(dtbuf);
		return;
	}
	memset(gps_mqtt_payload,0,GPS_MQTT_PAYLOAD_LENGTH);	
	tp=gpsx->longitude;
	memset(dtbuf,0,50);
	sprintf((char *)dtbuf,"{\"type\":\"gps\",\"longitude\":\"%.5f\",",tp/=100000);	//�õ������ַ���
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);
		   
	tp=gpsx->latitude;	  
	memset(dtbuf,0,50);
	sprintf((char *)dtbuf,"\"latitude\":\"%.5f\",",tp/=100000);	//�õ�γ���ַ���
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);
		 
	tp=gpsx->altitude;	 
	memset(dtbuf,0,50);
 	sprintf((char *)dtbuf,"\"height\":\"%.1f\",",tp/=10);	    			//�õ��߶��ַ���
 	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);
	
	tp=gpsx->speed;	 
	memset(dtbuf,0,50);
 	sprintf((char *)dtbuf,"\"speed\":\"%.1f\",",tp/=1000);		    		//�õ��ٶ��ַ���	 
 	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);	
	
  memset(dtbuf,0,50); 
	sprintf((char *)dtbuf,"\"viewStatellite\":\"%02d\",",gpsx->svnum%100);	 		//�ɼ�������
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);	

	memset(dtbuf,0,50); 
	sprintf((char *)dtbuf,"\"state\":\"%1d\",",gpsx->fixmode);	 		//��λ״̬
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);
	
	memset(dtbuf,0,50); 
	sprintf((char *)dtbuf,"\"dataTime\":\"%04d%02d%02d",gpsx->utc.year,gpsx->utc.month,gpsx->utc.date);	//��ʾUTC����
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);    
	
	sprintf((char *)dtbuf,"%02d%02d%02d\"}",gpsx->utc.hour,gpsx->utc.min,gpsx->utc.sec);	//��ʾUTCʱ��
  data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);	
	SEGGER_RTT_printf2("gps_mqtt_payload=%s\r\n",gps_mqtt_payload);	
	gps_mqtt_data_ready=GPS_MQTT_DATA_READY_YES;
	free(dtbuf);
	
	
	
	
}

//GPSУ��ͼ���
//buf:���ݻ������׵�ַ
//len:���ݳ���
//cka,ckb:����У����.
void Ublox_CheckSum(u8 *buf,u16 len,u8* cka,u8*ckb)
{
	u16 i;
	*cka=0;*ckb=0;
	for(i=0;i<len;i++)
	{
		*cka=*cka+buf[i];
		*ckb=*ckb+*cka;
	}
}
/////////////////////////////////////////UBLOX ���ô���/////////////////////////////////////
//���CFG����ִ�����
//����ֵ:0,ACK�ɹ�
//       1,���ճ�ʱ����
//       2,û���ҵ�ͬ���ַ�
//       3,���յ�NACKӦ��
u8 Ublox_Cfg_Ack_Check(void)
{			 
	u16 len=0,i;
	u8 rval=0;
	while((GPS_RX_STA&0X8000)==0 && len<100)//�ȴ����յ�Ӧ��   
	{
		len++;
		delay_ms(5);
	}		 
	if(len<250)   	//��ʱ����.
	{
		len=GPS_RX_STA&0X7FFF;	//�˴ν��յ������ݳ��� 
		for(i=0;i<len;i++)if(GPS_RX_BUF[i]==0XB5)break;//����ͬ���ַ� 0XB5
		if(i==len)rval=2;						//û���ҵ�ͬ���ַ�
		else if(GPS_RX_BUF[i+3]==0X00)rval=3;//���յ�NACKӦ��
		else rval=0;	   						//���յ�ACKӦ��
	}else rval=1;								//���ճ�ʱ����
    GPS_RX_STA=0;							//�������
	return rval;  
}
//���ñ���
//����ǰ���ñ������ⲿEEPROM����
//����ֵ:0,ִ�гɹ�;1,ִ��ʧ��.
u8 Ublox_Cfg_Cfg_Save(void)
{
	u8 i;
	_ublox_cfg_cfg *cfg_cfg=(_ublox_cfg_cfg *)GPS_RX_BUF;
	cfg_cfg->header=0X62B5;		//cfg header
	cfg_cfg->id=0X0906;			//cfg cfg id
	cfg_cfg->dlength=13;		//����������Ϊ13���ֽ�.		 
	cfg_cfg->clearmask=0;		//�������Ϊ0
	cfg_cfg->savemask=0XFFFF; 	//��������Ϊ0XFFFF
	cfg_cfg->loadmask=0; 		//��������Ϊ0 
	cfg_cfg->devicemask=4; 		//������EEPROM����		 
	Ublox_CheckSum((u8*)(&cfg_cfg->id),sizeof(_ublox_cfg_cfg)-4,&cfg_cfg->cka,&cfg_cfg->ckb);
	Ublox_Send_Date((u8*)cfg_cfg,sizeof(_ublox_cfg_cfg));//�������ݸ�NEO-6M     
	for(i=0;i<6;i++)if(Ublox_Cfg_Ack_Check()==0)break;		//EEPROMд����Ҫ�ȽϾ�ʱ��,���������ж϶��
	return i==6?1:0;
}
//����NMEA�����Ϣ��ʽ
//msgid:Ҫ������NMEA��Ϣ��Ŀ,���������Ĳ�����
//      00,GPGGA;01,GPGLL;02,GPGSA;
//		03,GPGSV;04,GPRMC;05,GPVTG;
//		06,GPGRS;07,GPGST;08,GPZDA;
//		09,GPGBS;0A,GPDTM;0D,GPGNS;
//uart1set:0,����ر�;1,�������.	  
//����ֵ:0,ִ�гɹ�;����,ִ��ʧ��.
u8 Ublox_Cfg_Msg(u8 msgid,u8 uart1set)
{
	_ublox_cfg_msg *cfg_msg=(_ublox_cfg_msg *)GPS_RX_BUF;
	cfg_msg->header=0X62B5;		//cfg header
	cfg_msg->id=0X0106;			//cfg msg id
	cfg_msg->dlength=8;			//����������Ϊ8���ֽ�.	
	cfg_msg->msgclass=0XF0;  	//NMEA��Ϣ
	cfg_msg->msgid=msgid; 		//Ҫ������NMEA��Ϣ��Ŀ
	cfg_msg->iicset=1; 			//Ĭ�Ͽ���
	cfg_msg->uart1set=uart1set; //��������
	cfg_msg->uart2set=1; 	 	//Ĭ�Ͽ���
	cfg_msg->usbset=1; 			//Ĭ�Ͽ���
	cfg_msg->spiset=1; 			//Ĭ�Ͽ���
	cfg_msg->ncset=1; 			//Ĭ�Ͽ���	  
	Ublox_CheckSum((u8*)(&cfg_msg->id),sizeof(_ublox_cfg_msg)-4,&cfg_msg->cka,&cfg_msg->ckb);
	Ublox_Send_Date((u8*)cfg_msg,sizeof(_ublox_cfg_msg));//�������ݸ�NEO-6M    
	return Ublox_Cfg_Ack_Check();
}
//����NMEA�����Ϣ��ʽ
//baudrate:������,4800/9600/19200/38400/57600/115200/230400	  
//����ֵ:0,ִ�гɹ�;����,ִ��ʧ��(���ﲻ�᷵��0��)
u8 Ublox_Cfg_Prt(u32 baudrate)
{
	_ublox_cfg_prt *cfg_prt=(_ublox_cfg_prt *)GPS_RX_BUF;
	cfg_prt->header=0X62B5;		//cfg header
	cfg_prt->id=0X0006;			//cfg prt id
	cfg_prt->dlength=20;		//����������Ϊ20���ֽ�.	
	cfg_prt->portid=1;			//��������1
	cfg_prt->reserved=0;	 	//�����ֽ�,����Ϊ0
	cfg_prt->txready=0;	 		//TX Ready����Ϊ0
	cfg_prt->mode=0X08D0; 		//8λ,1��ֹͣλ,��У��λ
	cfg_prt->baudrate=baudrate; //����������
	cfg_prt->inprotomask=0X0007;//0+1+2
	cfg_prt->outprotomask=0X0007;//0+1+2
 	cfg_prt->reserved4=0; 		//�����ֽ�,����Ϊ0
 	cfg_prt->reserved5=0; 		//�����ֽ�,����Ϊ0 
	Ublox_CheckSum((u8*)(&cfg_prt->id),sizeof(_ublox_cfg_prt)-4,&cfg_prt->cka,&cfg_prt->ckb);
	Ublox_Send_Date((u8*)cfg_prt,sizeof(_ublox_cfg_prt));//�������ݸ�NEO-6M   
	delay_ms(200);				//�ȴ�������� 
	gps_init(baudrate);	//���³�ʼ������3  
	return Ublox_Cfg_Ack_Check();//���ﲻ�ᷴ��0,��ΪUBLOX��������Ӧ���ڴ������³�ʼ����ʱ���Ѿ���������.
} 
//����UBLOX NEO-6��ʱ���������
//interval:������(us)
//length:������(us)
//status:��������:1,�ߵ�ƽ��Ч;0,�ر�;-1,�͵�ƽ��Ч.
//����ֵ:0,���ͳɹ�;����,����ʧ��.
u8 Ublox_Cfg_Tp(u32 interval,u32 length,signed char status)
{
	_ublox_cfg_tp *cfg_tp=(_ublox_cfg_tp *)GPS_RX_BUF;
	cfg_tp->header=0X62B5;		//cfg header
	cfg_tp->id=0X0706;			//cfg tp id
	cfg_tp->dlength=20;			//����������Ϊ20���ֽ�.
	cfg_tp->interval=interval;	//������,us
	cfg_tp->length=length;		//������,us
	cfg_tp->status=status;	   	//ʱ����������
	cfg_tp->timeref=0;			//�ο�UTC ʱ��
	cfg_tp->flags=0;			//flagsΪ0
	cfg_tp->reserved=0;		 	//����λΪ0
	cfg_tp->antdelay=820;    	//������ʱΪ820ns
	cfg_tp->rfdelay=0;    		//RF��ʱΪ0ns
	cfg_tp->userdelay=0;    	//�û���ʱΪ0ns
	Ublox_CheckSum((u8*)(&cfg_tp->id),sizeof(_ublox_cfg_tp)-4,&cfg_tp->cka,&cfg_tp->ckb);
	Ublox_Send_Date((u8*)cfg_tp,sizeof(_ublox_cfg_tp));//�������ݸ�NEO-6M  
	return Ublox_Cfg_Ack_Check();
}
//����UBLOX NEO-6�ĸ�������	    
//measrate:����ʱ��������λΪms�����ٲ���С��200ms��5Hz��
//reftime:�ο�ʱ�䣬0=UTC Time��1=GPS Time��һ������Ϊ1��
//����ֵ:0,���ͳɹ�;����,����ʧ��.
u8 Ublox_Cfg_Rate(u16 measrate,u8 reftime)
{
	_ublox_cfg_rate *cfg_rate=(_ublox_cfg_rate *)GPS_RX_BUF;
 	if(measrate<200)return 1;	//С��200ms��ֱ���˳�
 	cfg_rate->header=0X62B5;	//cfg header
	cfg_rate->id=0X0806;	 	//cfg rate id
	cfg_rate->dlength=6;	 	//����������Ϊ6���ֽ�.
	cfg_rate->measrate=measrate;//������,us
	cfg_rate->navrate=1;		//�������ʣ����ڣ����̶�Ϊ1
	cfg_rate->timeref=reftime; 	//�ο�ʱ��ΪGPSʱ��
	Ublox_CheckSum((u8*)(&cfg_rate->id),sizeof(_ublox_cfg_rate)-4,&cfg_rate->cka,&cfg_rate->ckb);
	Ublox_Send_Date((u8*)cfg_rate,sizeof(_ublox_cfg_rate));//�������ݸ�NEO-6M 
	return Ublox_Cfg_Ack_Check();
}
//����һ�����ݸ�Ublox NEO-6M������ͨ������3����
//dbuf�����ݻ����׵�ַ
//len��Ҫ���͵��ֽ���
void Ublox_Send_Date(u8* dbuf,u16 len)
{
	u16 j;
	for(j=0;j<len;j++)//ѭ����������
	{
//		while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
//		USART3->DR=dbuf[j];  
		
		usart_data_transmit(GPS_UART, dbuf[j]);
		while(RESET == usart_flag_get(GPS_UART, USART_FLAG_TBE));
	}	
}

u8 gps_mqtt_data_ready=GPS_MQTT_DATA_READY_NO;
//��ʾGPS��λ��Ϣ 
char* gps_mqtt_payload;
void Gps_Msg_Show(nmea_msg gpsx)
{
 	float tp;	
	char* dtbuf;
	dtbuf=(char*)malloc(50);
	u16 data_len1=0,data_len2=0;
	if(gpsx.fixmode<2)														//��λ״̬
	{  
		sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);	
	  SEGGER_RTT_printf2("%s\r\n",dtbuf);	
		free(dtbuf);
		return;
	}
	if(gps_mqtt_data_ready==GPS_MQTT_DATA_READY_YES)
	{
		free(dtbuf);
		return;
	}
	memset(gps_mqtt_payload,0,GPS_MQTT_PAYLOAD_LENGTH);	
	tp=gpsx.longitude;
//	SEGGER_RTT_printf2("tp=%d\r\n",tp);	
	memset(dtbuf,0,50);
	sprintf((char *)dtbuf,"{\"longitude\":\"%.5f\",",tp/=100000);	//�õ������ַ���
//	SEGGER_RTT_printf2("dtbuf=%s\r\n",dtbuf);	
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);
		   
	tp=gpsx.latitude;	  
	memset(dtbuf,0,50);
	sprintf((char *)dtbuf,"\"latitude\":\"%.5f\",",tp/=100000);	//�õ�γ���ַ���
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);
		 
	tp=gpsx.altitude;	 
	memset(dtbuf,0,50);
 	sprintf((char *)dtbuf,"\"height\":\"%.1f\",",tp/=10);	    			//�õ��߶��ַ���
 	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);
	
	tp=gpsx.speed;	 
	memset(dtbuf,0,50);
 	sprintf((char *)dtbuf,"\"speed\":\"%.1f\",",tp/=1000);		    		//�õ��ٶ��ַ���	 
 	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);	
	
  memset(dtbuf,0,50); 
	sprintf((char *)dtbuf,"\"viewStatellite\":\"%02d\",",gpsx.svnum%100);	 		//�ɼ�������
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);	

	memset(dtbuf,0,50); 
	sprintf((char *)dtbuf,"\"state\":\"%1d\",",gpsx.fixmode);	 		//��λ״̬
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);
	
	memset(dtbuf,0,50); 
	sprintf((char *)dtbuf,"\"dataTime\":\"%04d/%02d/%02d_",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
	data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);    
	
	sprintf((char *)dtbuf,"%02d:%02d:%02d\"}",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��
  data_len2=strlen(dtbuf);
	memcpy(gps_mqtt_payload+data_len1,dtbuf,data_len2);
	data_len1=strlen(gps_mqtt_payload);	
	
	gps_mqtt_data_ready=GPS_MQTT_DATA_READY_YES;
	free(dtbuf);
	
//	tp=gpsx.longitude;	   
//	sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//�õ������ַ���
// 	SEGGER_RTT_printf2("%s\r\n",dtbuf);	 	   
//	tp=gpsx.latitude;	   
//	sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���
// 	SEGGER_RTT_printf2("%s\r\n",dtbuf);	 	 
//	tp=gpsx.altitude;	   
// 	sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//�õ��߶��ַ���
// 	SEGGER_RTT_printf2("%s\r\n",dtbuf);	 			   
//	tp=gpsx.speed;	   
// 	sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//�õ��ٶ��ַ���	 
// 	SEGGER_RTT_printf2("%s\r\n",dtbuf);	 				    	 	   
//	sprintf((char *)dtbuf,"Valid satellite:%02d",gpsx.posslnum);	 		//���ڶ�λ��������
// 	SEGGER_RTT_printf2("%s\r\n",dtbuf);	    
//	sprintf((char *)dtbuf,"Visible satellite:%02d",gpsx.svnum%100);	 		//�ɼ�������
// 	SEGGER_RTT_printf2("%s\r\n",dtbuf);		 
//	sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
//	//printf("year2:%d\r\n",gpsx.utc.year);
//	SEGGER_RTT_printf2("%s\r\n",dtbuf);		    
//	sprintf((char *)dtbuf,"UTC Time:%02d:%02d:%02d   ",gpsx.utc.hour,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��
//  SEGGER_RTT_printf2("%s\r\n",dtbuf);		  
}	 

void gps_init(u32 BAUD)
{
		nvic_irq_enable(GPS_UART_IRQ, 0, 0);
		
		rcu_periph_clock_enable(GPS_TX_RCU);
		
		rcu_periph_clock_enable(GPS_RX_RCU);

    /* enable USART clock */
    rcu_periph_clock_enable(GPS_UART_CLK);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPS_TX_PORT, GPS_UART_AF, GPS_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPS_RX_PORT, GPS_UART_AF, GPS_RX_PIN);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPS_TX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPS_TX_PIN);
    gpio_output_options_set(GPS_TX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPS_TX_PIN);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPS_RX_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPS_RX_PIN);
    gpio_output_options_set(GPS_RX_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPS_RX_PIN);

    /* USART configure */
    usart_deinit(GPS_UART);
    usart_baudrate_set(GPS_UART,BAUD);
    usart_receive_config(GPS_UART, USART_RECEIVE_ENABLE);
    usart_transmit_config(GPS_UART, USART_TRANSMIT_ENABLE);
    usart_enable(GPS_UART);
		
		/* enable receive interrupt */
    usart_interrupt_enable(GPS_UART, USART_INT_RBNE);
		
		gps_mqtt_payload=(char*)malloc(GPS_MQTT_PAYLOAD_LENGTH);
	
}


void gps_loop(void)
{ 
	u16 rxlen;
//	u16 lenx;
//	u8 key=0XFF;
//	u8 upload=0;
 
//	if(Ublox_Cfg_Rate(1000,1)!=0)	//���ö�λ��Ϣ�����ٶ�Ϊ1000ms,˳���ж�GPSģ���Ƿ���λ. 
//	{
//		while((Ublox_Cfg_Rate(1000,1)!=0)&&key)	//�����ж�,ֱ�����Լ�鵽NEO-6M,�����ݱ���ɹ�
//		{
//			usart3_init(9600);				//��ʼ������3������Ϊ9600(EEPROMû�б������ݵ�ʱ��,������Ϊ9600.)
//	  	Ublox_Cfg_Prt(38400);			//��������ģ��Ĳ�����Ϊ38400
//			usart3_init(38400);				//��ʼ������3������Ϊ38400
//			Ublox_Cfg_Tp(1000000,100000,1);	//����PPSΪ1�������1��,������Ϊ100ms	    
//			key=Ublox_Cfg_Cfg_Save();		//��������  
//		}	  					 
//	  LCD_ShowString(30,120,200,16,16,"NEO-6M Set Done!!");
//		delay_ms(500);
//	}
//	while(1) 
//	{		
//		delay_ms(1);
//		nmea_msg gpsx2;
		if(GPS_RX_STA&0X8000)		//���յ�һ��������
		{
			rxlen=GPS_RX_STA&0X7FFF;	//�õ����ݳ���
//			for(i=0;i<rxlen;i++)GPS_TX_BUF[i]=GPS_RX_BUF[i];	   
 			
			GPS_RX_BUF[rxlen]=0;			//�Զ���ӽ�����
//			SEGGER_RTT_printf2("\r\n---------------GPS_DATA---------------\r\n%s\r\n",GPS_RX_BUF);
			GPS_Analysis(&gpsx,(u8*)GPS_RX_BUF);//�����ַ���
//			Gps_Msg_Show(gpsx);				//��ʾ��Ϣ	
			GPS_RX_STA=0;		   	//������һ�ν���
 		}
//		key=KEY_Scan(0);
//		if(key==KEY0_PRES)
//		{
//			upload=!upload;
//			POINT_COLOR=RED;
//			if(upload)LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:ON ");
//			else LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:OFF");
// 		}
//		if((lenx%500)==0)LED0=!LED0; 	    				 
//		lenx++;	
//	}									    
}

  

void GPS_UART_IRQHandler(void)
{
	
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif
	u8 res;	
	if(RESET != usart_interrupt_flag_get(GPS_UART, USART_INT_FLAG_RBNE))
	{
			res=usart_data_receive(GPS_UART);		   
			if((GPS_RX_STA&(1<<15))==0)//�������һ������,��û�б�����,���ٽ�����������
			{ 
				if(GPS_RX_STA<GPS_MAX_RECV_LEN)		//�����Խ�������
				{
					gps_uart_rx_timeout=0;//���������        				 
					if(GPS_RX_STA==0)		
						gps_uart_begin=GPS_UART_BEGIN_YES;  //ʹ�ܶ�ʱ��7 
					GPS_RX_BUF[GPS_RX_STA++]=res;		//��¼���յ���ֵ	 
				}
				else 
				{
					GPS_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
				} 
			}  	
	}
#if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
#endif
}







