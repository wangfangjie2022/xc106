#include "header.h"


u8 ov2640_mode=0;						//����ģʽ:0,RGB565ģʽ;1,JPEGģʽ

#define jpeg_buf_size 31*1024  			//����JPEG���ݻ���jpeg_buf�Ĵ�С(*4�ֽ�)
//#define jpeg_buf_size 25*1024  			//����JPEG���ݻ���jpeg_buf�Ĵ�С(*4�ֽ�)
__align(4) u32 jpeg_buf[jpeg_buf_size];	//JPEG���ݻ���buf
volatile u32 jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile u8 jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־ 
										//0,����û�вɼ���;
										//1,���ݲɼ�����,���ǻ�û����;
										//2,�����Ѿ����������,���Կ�ʼ��һ֡����
//JPEG�ߴ�֧���б�
const u16 jpeg_img_size_tbl[][2]=
{
	176,144,	//QCIF
	160,120,	//QQVGA
	352,288,	//CIF
	320,240,	//QVGA
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,1024,	//SXGA
	1600,1200,	//UXGA
}; 
const char *EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7����Ч 
const char *JPEG_SIZE_TBL[9]={"QCIF","QQVGA","CIF","QVGA","VGA","SVGA","XGA","SXGA","UXGA"};	//JPEGͼƬ 9�ֳߴ� 


//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
void jpeg_data_process(void)
{
		u32 remain_len=0;
		if(jpeg_data_ok==0)	//jpeg���ݻ�δ�ɼ���?
		{	
			dma_channel_disable(DMA1, DMA_CH7);//ֹͣ��ǰ���� 
			while (DMA_GetCmdStatus(DMA1, DMA_CH7) != DISABLE){}//�ȴ�DMA2_Stream1������  
			remain_len=dma_transfer_number_get(DMA1, DMA_CH7);
			jpeg_data_len=jpeg_buf_size-remain_len;//�õ��˴����ݴ���ĳ���			
			jpeg_data_ok=1; 				//���JPEG���ݲɼ��갴��,�ȴ�������������
		}
} 
//JPEG����
//JPEG����,ͨ������2���͸�����.
#define SDIO_D3_MODE 0
#define DCI_D4_MODE 1
void set_sdio_d3_mode(u8 mode)
{
	if(mode==SDIO_D3_MODE)
	{
//		gpio_af_set(SDIO_D3_PORT, GPIO_AF_12, SDIO_D3_PIN);  
//		gpio_mode_set(SDIO_D3_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, SDIO_D3_PIN);
//		gpio_output_options_set(SDIO_D3_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, SDIO_D3_PIN);
	}
	else
	{
		gpio_af_set(DCI_D4_PORT, GPIO_AF_13, DCI_D4_PIN);
//		gpio_mode_set(DCI_D4_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_D4_PIN);
//    gpio_output_options_set(DCI_D4_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_D4_PIN);
	}		
	
}


FIL img_file;
void jpeg_test(void)
{
	while(1)
	{

		if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
		{  
			SEGGER_RTT_printf2("Sending JPEG data...\r\n"); //��ʾ���ڴ�������
			set_sdio_d3_mode(SDIO_D3_MODE);
			write_img_file((u8*)jpeg_buf,jpeg_data_len*4);		
			set_sdio_d3_mode(DCI_D4_MODE);
			SEGGER_RTT_printf2("Send data complete!!");//��ʾ����������� 		
			jpeg_data_ok=2;
		}	
		if(saoma_statue==SAOMA_GETDATA)
		{
			saoma_statue=SAOMA_IDLE;
			DCMI_Stop();
			DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_PERIPH_WIDTH_32BIT,DMA_MEMORY_INCREASE_ENABLE);
			dma_channel_enable(DMA1, DMA_CH7);			//���´���
			dci_capture_enable();//DCMI����ʹ��  
			jpeg_data_ok=0;
		}
		OSTimeDlyHMSM(0,0,0,100);  //��ʱ500ms
	}    
} 

#define WRITE_FILE_LENGTH 4000
void write_img_file(u8 *buffer,u32 buffer_length)
{
	FRESULT res;
	UINT  bw;
	int i=0;
	u16 write_times=0;
	u16 remain_num=0;
	char file_name[80]={0};
	RTC_Get_String_Date_Time(file_name);
	res = f_open(&img_file, file_name, FA_CREATE_NEW | FA_WRITE);
	if(res)
	{
		SEGGER_RTT_printf2("open file fail res:%d!!!...\r\n",res); //��ʾ���ڴ�������
		f_close(&img_file);
		return;
	}
	write_times=buffer_length/WRITE_FILE_LENGTH;
	remain_num=buffer_length%WRITE_FILE_LENGTH;
	SEGGER_RTT_printf2("write_times:%d,remain_num:%d\r\n",write_times,remain_num);
	for(i=0;i<write_times;i++)
	{
		res = f_write(&img_file, buffer+i*WRITE_FILE_LENGTH, WRITE_FILE_LENGTH, &bw); 
	}
	if(remain_num!=0)
	{
		res = f_write(&img_file, buffer+i*WRITE_FILE_LENGTH,remain_num, &bw); 		
	}
	if(res)
	{
		SEGGER_RTT_printf2("write_img_file fail res:%d!!!...\r\n",res); 
	}
	else
	{
		SEGGER_RTT_printf2("write_img_file OK\r\n"); 
	}
	f_close(&img_file);
//	f_rename((char *)pname,record_file_name);
	return;
}


///////////////////////////////////////////camera_task����
void camera_task(void *pdata)
{
	SEGGER_RTT_printf2("----camera_task------\r\n");
//	camera_init();
	while(1)
	{	
		jpeg_test();  
		OSTimeDlyHMSM(0,0,0,100);  //��ʱ500ms
	}
}

void camera_init(void)
{
	u8 size=8;		//Ĭ����QVGA 320*240�ߴ�
	SEGGER_RTT_printf2("----camera_init------\r\n");
	while(OV2640_Init())//��ʼ��OV2640
	{
		SEGGER_RTT_printf2("OV2640 ERR\r\n");
	}
	SEGGER_RTT_printf2("OV2640 JPEG Mode\r\n");
	SEGGER_RTT_printf2("JPEG Size:%s\r\n",JPEG_SIZE_TBL[size]);
	
 	OV2640_JPEG_Mode();		//JPEGģʽ
	My_DCMI_Init();			//DCMI����
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_PERIPH_WIDTH_32BIT,DMA_MEMORY_INCREASE_ENABLE);//DCMI DMA����   
	OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ� 
	delay_ms(2000);//������ɺ�Ҫ�ȴ�����ͷ�ȶ�
	
}

void camera_take_pic(char* file_name)
{
	saoma_statue=SAOMA_IDLE;
	DCMI_Stop();
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_PERIPH_WIDTH_32BIT,DMA_MEMORY_INCREASE_ENABLE);
	dma_channel_enable(DMA1, DMA_CH7);			//���´���
	dci_capture_enable();//DCMI����ʹ��  
	jpeg_data_ok=0;
	while(jpeg_data_ok==0)
	{
			SEGGER_RTT_printf2("water picture ok\r\n");
			OSTimeDlyHMSM(0,0,0,100);  //��ʱ500ms		
	}
	if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
	{  
		SEGGER_RTT_printf2("Sending JPEG data...\r\n"); //��ʾ���ڴ�������
		set_sdio_d3_mode(SDIO_D3_MODE);
/////////////////////////////////////////////////////////////////////////////////
		
				FRESULT res;
				UINT  bw;
				int i=0;
				u16 write_times=0;
				u16 remain_num=0;
				res = f_open(&img_file, file_name, FA_CREATE_NEW | FA_WRITE);
				if(res)
				{
					SEGGER_RTT_printf2("open file fail res:%d!!!...\r\n",res); //��ʾ���ڴ�������
					f_close(&img_file);
					return;
				}
				write_times=jpeg_data_len*4/WRITE_FILE_LENGTH;
				remain_num=jpeg_data_len*4%WRITE_FILE_LENGTH;
				SEGGER_RTT_printf2("write_times:%d,remain_num:%d\r\n",write_times,remain_num);
				for(i=0;i<write_times;i++)
				{
					res = f_write(&img_file, (u8*)jpeg_buf+i*WRITE_FILE_LENGTH, WRITE_FILE_LENGTH, &bw); 
				}
				if(remain_num!=0)
				{
					res = f_write(&img_file, (u8*)jpeg_buf+i*WRITE_FILE_LENGTH,remain_num, &bw); 		
				}
				if(res)
				{
					SEGGER_RTT_printf2("write_img_file fail res:%d!!!...\r\n",res); 
				}
				else
				{
					SEGGER_RTT_printf2("write_img_file OK\r\n"); 
				}
				f_close(&img_file);
				

/////////////////////////////////////////////////////////////////////////////////
		set_sdio_d3_mode(DCI_D4_MODE);
		SEGGER_RTT_printf2("Send data complete!!");//��ʾ����������� 		
		jpeg_data_ok=2;
	}	
	
}

#define BASE64_BUFFER_LEN 4500
#define WRITE_BUFFER_LEN 3000
u8 camera_take_pic_base64(char* file_name)
{
	FRESULT res;
	UINT  bw;
	int i=0;
	u16 write_times=0;
	u16 remain_num=0;
	saoma_statue=SAOMA_IDLE;
	DCMI_Stop();
	DCMI_DMA_Init((u32)&jpeg_buf,jpeg_buf_size,DMA_PERIPH_WIDTH_32BIT,DMA_MEMORY_INCREASE_ENABLE);
	dma_channel_enable(DMA1, DMA_CH7);			//���´���
	dci_capture_enable();//DCMI����ʹ��  
	jpeg_data_ok=0;
//	u8 *buf=(u8*)jpeg_buf;
	u8 *ba64_buff;
	ba64_buff=(u8*)malloc(BASE64_BUFFER_LEN);
	u16 timeout=0;
	while(jpeg_data_ok==0)
	{
			if(timeout++>100)
				break;
			SEGGER_RTT_printf2("water picture ok\r\n");
			OSTimeDlyHMSM(0,0,0,100);  //��ʱ500ms		
	}
	if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
	{  
		SEGGER_RTT_printf2("Sending JPEG data...\r\n"); //��ʾ���ڴ�������
		set_sdio_d3_mode(SDIO_D3_MODE);
/////////////////////////////////////////////////////////////////////////////////
		
				
				res = f_open(&img_file, file_name, FA_CREATE_NEW | FA_WRITE);
				if(res==FR_EXIST)
				{
					SEGGER_RTT_printf2("open file fail res==FR_EXIST !!!\r\n");
					f_close(&img_file);
					f_unlink(file_name);
					res = f_open(&img_file, file_name, FA_CREATE_NEW | FA_WRITE);
				}
				if(res)
				{
						SEGGER_RTT_printf2("open file fail res:%d!!!...\r\n",res); //��ʾ���ڴ�������
						f_close(&img_file);
						free(ba64_buff);
						set_sdio_d3_mode(DCI_D4_MODE);
						jpeg_data_ok=2;
						return 1;
				}
				size_t recnum=0;
				write_times=jpeg_data_len*4/WRITE_BUFFER_LEN;
				remain_num=jpeg_data_len*4%WRITE_BUFFER_LEN;
				SEGGER_RTT_printf2("write_times:%d,remain_num:%d\r\n",write_times,remain_num);
				for(i=0;i<write_times;i++)
				{
					memset(ba64_buff,0,BASE64_BUFFER_LEN);
					qcloud_iot_utils_base64encode(ba64_buff,BASE64_BUFFER_LEN,&recnum,(u8*)jpeg_buf+i*WRITE_BUFFER_LEN,WRITE_BUFFER_LEN);
					res = f_write(&img_file, ba64_buff, recnum, &bw); 
					if(res)
					{
						SEGGER_RTT_printf2("write_img_file fail [%d] res:%d,recnum:%d...\r\n",i,res,recnum); 
						//20220919
						f_close(&img_file);
						free(ba64_buff);
						set_sdio_d3_mode(DCI_D4_MODE);
						jpeg_data_ok=2;
						return 1;
					}
					else
					{
						SEGGER_RTT_printf2("write_img_file ok [%d] res:%d,recnum:%d...\r\n",i,res,recnum); 						
					}
				}
				if(remain_num!=0)
				{
					qcloud_iot_utils_base64encode(ba64_buff,BASE64_BUFFER_LEN,&recnum,(u8*)jpeg_buf+i*WRITE_BUFFER_LEN,remain_num);
					res = f_write(&img_file, ba64_buff, recnum, &bw); 
				}
				if(res)
				{
					SEGGER_RTT_printf2("write_img_file fail res:%d!!!...\r\n",res); 
					f_close(&img_file);
					free(ba64_buff);
					jpeg_data_ok=2;
					return 1;
				}
				else
				{
					SEGGER_RTT_printf2("write_img_file OK\r\n"); 
				}
				f_close(&img_file);
				free(ba64_buff);
/////////////////////////////////////////////////////////////////////////////////
				set_sdio_d3_mode(DCI_D4_MODE);
				SEGGER_RTT_printf2("Send data complete!!");//��ʾ����������� 		
				jpeg_data_ok=2;
				return 0;
	}	
	SEGGER_RTT_printf2("water picture timeout\r\n");
	return 1;
	
}

