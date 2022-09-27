#include "header.h"
#include "network.h"


UPLOAD_FILE_PARA upload_file_para;

u8 upload_flag=0;
void upload_file_task(void *pdata)
{
	SEGGER_RTT_printf2("----upload_file_task------\r\n");
//	u16 timeout=0;
//	u8 retry=0;
	
	u8 res;
	u16 rval=0;
 	DIR tdir;	 		//��ʱĿ¼
//	DIR *tdir;	 		//��ʱĿ¼
	FILINFO tfileinfo;	//��ʱ�ļ���Ϣ		
	u8 *fn; 
//	char *messageid;
	
	SEGGER_RTT_printf2(  "----upload_file_task------\r\n");
	tfileinfo.lfsize=TRAN_FILE_LEN+50;						//���ļ�����󳤶�	
	tfileinfo.lfname=malloc(tfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
	fn=malloc(TRAN_FILE_LEN+50);	//Ϊ���ļ������������ڴ�
//	messageid=malloc(6);	//Ϊ���ļ������������ڴ�
//	tmpdata=malloc(150);	//Ϊ���ļ������������ڴ�
	
	while(1)
	{	
//		SEGGER_RTT_printf2(  "----upload_file_task------\r\n");
		if(upload_file_para.upload_flag!=UPLOAD_FLAG_BEGIN)
		{
//				SEGGER_RTT_printf2("-----------upload_file----------\r\n");	

				res=f_opendir(&tdir,RECORD_PATH); //��Ŀ¼		
//				SEGGER_RTT_printf2("---------res=f_opendir--%d\r\n",res);	 	
				memset(tfileinfo.lfname,0,tfileinfo.lfsize);
				if(res==FR_OK&&tfileinfo.lfname!=NULL)
				{
					while(upload_file_para.upload_flag!=UPLOAD_FLAG_BEGIN)//��ѯ�ܵ���Ч�ļ���
					{
								res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�	
								if(res!=FR_OK||tfileinfo.fname[0]==0)
								{
	//								SEGGER_RTT_printf2("-----------res=%d,tfileinfo.fname[0]=%d--------\r\n",res,tfileinfo.fname[0]);	
									break;	//������/��ĩβ��,�˳�	
								}									
									fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);		
									SEGGER_RTT_printf2("file_name is:%s\r\n",fn);
									/////////////////////////////////////////////////////////////////////////
									memset(upload_file_para.local_filename,0,LOCAL_FILE_LEN);
									snprintf(upload_file_para.local_filename,LOCAL_FILE_LEN,"%s",fn);									
														
									upload_file_para.upload_flag=UPLOAD_FLAG_BEGIN;
									//ftp_para.new_cmd_statue=CMD_STAUE_CHANGE_NO;
									u32 timeout=0;
									while((upload_file_para.upload_flag==UPLOAD_FLAG_BEGIN)&&timeout++<UPLOAD_FILE_TIMEOUT)//2020.2.27����ط��и�BUG���ϴ����ɹ�������һֱ�����ѭ�����ȴ���ʱ
									{
											SEGGER_RTT_printf2("---upload_file_para----uploading\r\n");
											OSTimeDlyHMSM(0,0,0,1000);  //
									}
									if((timeout<UPLOAD_FILE_TIMEOUT)&&(upload_file_para.upload_flag==UPLOAD_FLAG_FINISH))
									{
											upload_file_para.upload_flag=UPLOAD_FLAG_NO;
//											f_unlink(upload_file_para.local_filename);
											SEGGER_RTT_printf2("----------------------------ftp_para.upload_flag==UPLOAD_FLAG_FINISH goon---------------------\r\n");																			
									}																	
																															
									////////////////////////////////////////////////////////////////////////			
									
								}	
										
					}  
				}
				OSTimeDlyHMSM(0,0,0,200);  //��ʱ500ms		
//				delay_ms(200);
			}	
}


FIL img_file2;
char *payload_file;
char topic_file[TOPIC_SIZE];
AT_ERROR tranfile_err=0;
void PublishFile(char *filename)
{
	SEGGER_RTT_printf2("PublishFile\r\n");
//	FIL img_file;
	FRESULT res;
	UINT  bw;
	u32 filesize=0;
	
	char filename2[80];
	char *read_data;
	u16 timeout;
	
	snprintf(filename2,80,"%s/%s",RECORD_PATH,filename);
	SEGGER_RTT_printf2("PublishFile2\r\n");
	payload_file=(char*)malloc(MQTT_MAX_FLIE_SEND_LENGTH+200);
	read_data=(char*)malloc(MQTT_MAX_FLIE_SEND_LENGTH+10);
	if(payload_file==NULL)
	{
		SEGGER_RTT_printf2("payload==null!!!...\r\n");
		free(payload_file);
		free(read_data);
		return;
	}
	SEGGER_RTT_printf2("filename2:%s\r\n",filename2);
	res = f_open(&img_file2, filename2, FA_OPEN_EXISTING | FA_READ);
	SEGGER_RTT_printf2("PublishFile3\r\n");
	if(res)
	{
		SEGGER_RTT_printf2("open file fail res:%d!!!...\r\n",res); //��ʾ���ڴ�������
		f_close(&img_file2);
		free(payload_file);
		free(read_data);
		return;
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
//	u16 para_length=0;
	SEGGER_RTT_printf2("filesize:%d,tran_times:%d,remain_length:%d\r\n",filesize,tran_times,remain_length);
	while(tran_times>0)
	{
		memset(topic_file,0,TOPIC_SIZE);
		snprintf(topic_file,TOPIC_SIZE,"/devicecamera/%s",ChipIDstring+12);
		watch_time_tick=0;
		if(tran_times-->0)
			tran_length=MQTT_MAX_FLIE_SEND_LENGTH;				
		else
			tran_length=remain_length;
		page_num++;
		memset(payload_file,0,MQTT_MAX_FLIE_SEND_LENGTH+200);//payload�����������ݱ�ʵ��Ҫ���͵�����С
		snprintf(payload_file,MQTT_MAX_FLIE_SEND_LENGTH+200,"{\"type\":\"pic_file\",\"deviceId\":\"%s\",\"fileSize\":\"%d\",\"pgSize\":\"%d\",\"pgNum\":\"%d\",\"fileName\":\"%s\",\"content\":\"",ChipIDstring+12,filesize,total_tran_times,page_num,filename);			
//		para_length=strlen(payload_file);
		memset(read_data,0,MQTT_MAX_FLIE_SEND_LENGTH+10);//payload�����������ݱ�ʵ��Ҫ���͵�����С		
		res = f_read(&img_file2, (u8*)read_data, tran_length, &bw);
		if(res)
		{
			tranfile_err=1;
			SEGGER_RTT_printf2("f_read,err:%d\r\n",res);
			break;
		}
		strcat(payload_file,read_data);
		strcat(payload_file,"\"}");
		upload_file_para.upload_flag=UPLOAD_FLAG_BEGIN;
		timeout=0;
		while((upload_file_para.upload_flag==UPLOAD_FLAG_BEGIN)&&timeout++<UPLOAD_FILE_TIMEOUT)//2020.2.27����ط��и�BUG���ϴ����ɹ�������һֱ�����ѭ�����ȴ���ʱ
		{									
			delay_ms(10);
		}
		if(timeout==UPLOAD_FILE_TIMEOUT)
		{				
				tranfile_err=1;		
				break;
		}	
	}
	upload_file_para.upload_flag=UPLOAD_FLAG_NO;
	f_close(&img_file2);	
	if(tranfile_err==0)
	{
		f_unlink(filename2);
		SEGGER_RTT_printf2("f_unlink:%s\r\n",filename2);
		SEGGER_RTT_printf2("-----------ftp_para.upload_flag==UPLOAD_FLAG_FINISH goon------------\r\n");
	}
	else
	{
		SEGGER_RTT_printf2("upload fail\r\n");
	}
	free(payload_file);
	free(read_data);
	SEGGER_RTT_printf2("PublishFile end\r\n");
}

