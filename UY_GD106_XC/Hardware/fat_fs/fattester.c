#include "fattester.h"	 


  
#include "ff.h"
//#include "string.h"
#include "header.h"
#include "sdcard.h"
#include "diskio.h"
#include "header.h"
//#include "SEGGER_RTT_Conf.h"
//////////////////////////////////////////////////////////////////////////////////	 
//#define printf SEGGER_RTT_printf2
    
//Ϊ����ע�Ṥ����	 
//path:����·��������"0:"��"1:"
//mt:0��������ע�ᣨ�Ժ�ע�ᣩ��1������ע��
//����ֵ:ִ�н��
//u8 mf_mount(u8* path,u8 mt)
//{		   
//	return f_mount(fs[2],(const TCHAR*)path,mt); 
//}
////��·���µ��ļ�
////path:·��+�ļ���
////mode:��ģʽ
////����ֵ:ִ�н��
//u8 mf_open(u8*path,u8 mode)
//{
//	u8 res;	 
//	res=f_open(file,(const TCHAR*)path,mode);//���ļ���
//	return res;
//} 
////�ر��ļ�
////����ֵ:ִ�н��
//u8 mf_close(void)
//{
//	f_close(file);
//	return 0;
//}
////��������
////len:�����ĳ���
////����ֵ:ִ�н��
//u8 mf_read(u16 len)
//{
//	u16 i,t;
//	u8 res=0;
//	u16 tlen=0;
//	SEGGER_RTT_printf2("\r\nRead file data is:\r\n");
//	for(i=0;i<len/512;i++)
//	{
//		res=f_read(file,fatbuf,512,&br);
//		if(res)
//		{
//			SEGGER_RTT_printf2("Read Error:%d\r\n",res);
//			break;
//		}else
//		{
//			tlen+=br;
//			for(t=0;t<br;t++)SEGGER_RTT_printf2("%c",fatbuf[t]); 
//		}
//	}
//	if(len%512)
//	{
//		res=f_read(file,fatbuf,len%512,&br);
//		if(res)	//�����ݳ�����
//		{
//			SEGGER_RTT_printf2("\r\nRead Error:%d\r\n",res);   
//		}else
//		{
//			tlen+=br;
//			for(t=0;t<br;t++)SEGGER_RTT_printf2("%c",fatbuf[t]); 
//		}	 
//	}
//	if(tlen)SEGGER_RTT_printf2("\r\nReaded data len:%d\r\n",tlen);//���������ݳ���
//	SEGGER_RTT_printf2("Read data over\r\n");	 
//	return res;
//}
////д������
////dat:���ݻ�����
////len:д�볤��
////����ֵ:ִ�н��
//u8 mf_write(u8*dat,u16 len)
//{			    
//	u8 res;	   					   

//	SEGGER_RTT_printf2("\r\nBegin Write file...\r\n");
//	SEGGER_RTT_printf2("Write data len:%d\r\n",len);	 
//	res=f_write(file,dat,len,&bw);
//	if(res)
//	{
//		SEGGER_RTT_printf2("Write Error:%d\r\n",res);   
//	}else SEGGER_RTT_printf2("Writed data len:%d\r\n",bw);
//	SEGGER_RTT_printf2("Write data over.\r\n");
//	return res;
//}

////��Ŀ¼
// //path:·��
////����ֵ:ִ�н��
//u8 mf_opendir(u8* path)
//{
//	return f_opendir(&dir,(const TCHAR*)path);	
//}
////�ر�Ŀ¼ 
////����ֵ:ִ�н��
//u8 mf_closedir(void)
//{
//	return f_closedir(&dir);	
//}
////���ȡ�ļ���
////����ֵ:ִ�н��
//u8 mf_readdir(void)
//{
//	u8 res;
//	char *fn;			 
//#if _USE_LFN
// 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
//	fileinfo.lfname = malloc(fileinfo.lfsize);
//#endif		  
//	res=f_readdir(&dir,&fileinfo);//��ȡһ���ļ�����Ϣ
//	if(res!=FR_OK||fileinfo.fname[0]==0)
//	{
//		free(fileinfo.lfname);
//		return res;//������.
//	}
//#if _USE_LFN
//	fn=*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
//#else
//	fn=fileinfo.fname;;
//#endif	
//	SEGGER_RTT_printf2("\r\n DIR info:\r\n");

//	SEGGER_RTT_printf2("dir.id:%d\r\n",dir.id);
//	SEGGER_RTT_printf2("dir.index:%d\r\n",dir.index);
//	SEGGER_RTT_printf2("dir.sclust:%d\r\n",dir.sclust);
//	SEGGER_RTT_printf2("dir.clust:%d\r\n",dir.clust);
//	SEGGER_RTT_printf2("dir.sect:%d\r\n",dir.sect);	  

//	SEGGER_RTT_printf2("\r\n");
//	SEGGER_RTT_printf2("File Name is:%s\r\n",fn);
//	SEGGER_RTT_printf2("File Size is:%d\r\n",fileinfo.fsize);
//	SEGGER_RTT_printf2("File data is:%d\r\n",fileinfo.fdate);
//	SEGGER_RTT_printf2("File time is:%d\r\n",fileinfo.ftime);
//	SEGGER_RTT_printf2("File Attr is:%d\r\n",fileinfo.fattrib);
//	SEGGER_RTT_printf2("\r\n");
//	free(fileinfo.lfname);
//	return 0;
//}			 

// //�����ļ�
// //path:·��
// //����ֵ:ִ�н��
//u8 mf_scan_files(u8 * path)
//{
//	FRESULT res;	  
//    char *fn;   /* This function is assuming non-Unicode cfg. */
//#if _USE_LFN
// 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
//	fileinfo.lfname = malloc(fileinfo.lfsize);
//#endif		  

//    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
//    if (res == FR_OK) 
//	{	
//		SEGGER_RTT_printf2("\r\n"); 
//		while(1)
//		{
//	        res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
//	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //������/��ĩβ��,�˳�
//	        //if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
//#if _USE_LFN
//        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
//#else							   
//        	fn = fileinfo.fname;
//#endif	                                              /* It is a file. */
//			SEGGER_RTT_printf2("%s/", path);//��ӡ·��	
//			SEGGER_RTT_printf2("%s\r\n",  fn);//��ӡ�ļ���	  
//		} 
//    }	  
//	free(fileinfo.lfname);
//    return res;	  
//}
////��ʾʣ������
////drv:�̷�
////����ֵ:ʣ������(�ֽ�)
//u32 mf_showfree(u8 *drv)
//{
//	FATFS *fs1;
//	u8 res;
//    u32 fre_clust=0, fre_sect=0, tot_sect=0;
//    //�õ�������Ϣ�����д�����
//    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
//    if(res==0)
//	{											   
//	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
//	    fre_sect = fre_clust * fs1->csize;			//�õ�����������	   
//#if _MAX_SS!=512
//		tot_sect*=fs1->ssize/512;
//		fre_sect*=fs1->ssize/512;
//#endif	  
//		if(tot_sect<20480)//������С��10M
//		{
//		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
//		    SEGGER_RTT_printf2("\r\n����������:%d KB\r\n"
//		           "���ÿռ�:%d KB\r\n",
//		           tot_sect>>1,fre_sect>>1);
//		}else
//		{
//		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
//		    SEGGER_RTT_printf2("\r\n����������:%d MB\r\n"
//		           "���ÿռ�:%d MB\r\n",
//		           tot_sect>>11,fre_sect>>11);
//		}
//	}
//	return fre_sect;
//}		    
////�ļ���дָ��ƫ��
////offset:����׵�ַ��ƫ����
////����ֵ:ִ�н��.
//u8 mf_lseek(u32 offset)
//{
//	return f_lseek(file,offset);
//}
////��ȡ�ļ���ǰ��дָ���λ��.
////����ֵ:λ��
//u32 mf_tell(void)
//{
//	return f_tell(file);
//}
////��ȡ�ļ���С
////����ֵ:�ļ���С
//u32 mf_size(void)
//{
//	return f_size(file);
//} 
////����Ŀ¼
////pname:Ŀ¼·��+����
////����ֵ:ִ�н��
//u8 mf_mkdir(u8*pname)
//{
//	return f_mkdir((const TCHAR *)pname);
//}
////��ʽ��
////path:����·��������"0:"��"1:"
////mode:ģʽ
////au:�ش�С
////����ֵ:ִ�н��
//u8 mf_fmkfs(u8* path,u8 mode,u16 au)
//{
//	return f_mkfs((const TCHAR*)path,mode,au);//��ʽ��,drv:�̷�;mode:ģʽ;au:�ش�С
//} 
////ɾ���ļ�/Ŀ¼
////pname:�ļ�/Ŀ¼·��+����
////����ֵ:ִ�н��
//u8 mf_unlink(u8 *pname)
//{
//	return  f_unlink((const TCHAR *)pname);
//}

////�޸��ļ�/Ŀ¼����(���Ŀ¼��ͬ,�������ƶ��ļ�Ŷ!)
////oldname:֮ǰ������
////newname:������
////����ֵ:ִ�н��
//u8 mf_rename(u8 *oldname,u8* newname)
//{
//	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
//}
////��ȡ�̷����������֣�
////path:����·��������"0:"��"1:"  
//void mf_getlabel(u8 *path)
//{
//	u8 buf[20];
//	u32 sn=0;
//	u8 res;
//	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
//	if(res==FR_OK)
//	{
//		SEGGER_RTT_printf2("\r\n����%s ���̷�Ϊ:%s\r\n",path,buf);
//		SEGGER_RTT_printf2("����%s �����к�:%X\r\n\r\n",path,sn); 
//	}else SEGGER_RTT_printf2("\r\n��ȡʧ�ܣ�������:%X\r\n",res);
//}
////�����̷����������֣����11���ַ�������֧�����ֺʹ�д��ĸ����Լ����ֵ�
////path:���̺�+���֣�����"0:ALIENTEK"��"1:OPENEDV"  
//void mf_setlabel(u8 *path)
//{
//	u8 res;
//	res=f_setlabel ((const TCHAR *)path);
//	if(res==FR_OK)
//	{
//		SEGGER_RTT_printf2("\r\n�����̷����óɹ�:%s\r\n",path);
//	}else SEGGER_RTT_printf2("\r\n�����̷�����ʧ�ܣ�������:%X\r\n",res);
//} 

////���ļ������ȡһ���ַ���
////size:Ҫ��ȡ�ĳ���
//void mf_gets(u16 size)
//{
// 	TCHAR* rbuf;
//	rbuf=f_gets((TCHAR*)fatbuf,size,file);
//	if(*rbuf==0)return  ;//û�����ݶ���
//	else
//	{
//		SEGGER_RTT_printf2("\r\nThe String Readed Is:%s\r\n",rbuf);  	  
//	}			    	
//}
////��Ҫ_USE_STRFUNC>=1
////дһ���ַ����ļ�
////c:Ҫд����ַ�
////����ֵ:ִ�н��
//u8 mf_putc(u8 c)
//{
//	return f_putc((TCHAR)c,file);
//}
////д�ַ������ļ�
////c:Ҫд����ַ���
////����ֵ:д����ַ�������
//u8 mf_puts(u8*c)
//{
//	return f_puts((TCHAR*)c,file);
//}





////==================================================20190105 wxf add==============================



//u8 ftp_open(u8*path,u8 mode)
//{
//	u8 res;	 
//	res=f_open(ftp_file,(const TCHAR*)path,mode);//���ļ���
//	return res;
//} 
////�ر��ļ�
////����ֵ:ִ�н��
//u8 ftp_close(void)
//{
//	f_close(ftp_file);
//	return 0;
//}
////��������
////len:�����ĳ���
////����ֵ:ִ�н��
//u8 ftp_read(u16 len)
//{
//	u16 i,t;
//	u8 res=0;
//	u16 tlen=0;
//	SEGGER_RTT_printf2("\r\nRead file data is:\r\n");
//	for(i=0;i<len/512;i++)
//	{
//		res=f_read(ftp_file,ftp_fatbuf,512,&ftp_br);
//		if(res)
//		{
//			SEGGER_RTT_printf2("Read Error:%d\r\n",res);
//			break;
//		}else
//		{
//			tlen+=br;
//			for(t=0;t<br;t++)SEGGER_RTT_printf2("%c",ftp_fatbuf[t]); 
//		}
//	}
//	if(len%512)
//	{
//		res=f_read(ftp_file,ftp_fatbuf,len%512,&br);
//		if(res)	//�����ݳ�����
//		{
//			SEGGER_RTT_printf2("\r\nRead Error:%d\r\n",res);   
//		}else
//		{
//			tlen+=br;
//			for(t=0;t<br;t++)SEGGER_RTT_printf2("%c",ftp_fatbuf[t]); 
//		}	 
//	}
//	if(tlen)SEGGER_RTT_printf2("\r\nReaded data len:%d\r\n",tlen);//���������ݳ���
//	SEGGER_RTT_printf2("Read data over\r\n");	 
//	return res;
//}
////д������
////dat:���ݻ�����
////len:д�볤��
////����ֵ:ִ�н��
//u8 ftp_write(u8*dat,u16 len)
//{			    
//	u8 res;	   					   

//	SEGGER_RTT_printf2("\r\nBegin Write file...\r\n");
//	SEGGER_RTT_printf2("Write data len:%d\r\n",len);	 
//	res=f_write(ftp_file,dat,len,&ftp_bw);
//	if(res)
//	{
//		SEGGER_RTT_printf2("Write Error:%d\r\n",res);   
//	}else SEGGER_RTT_printf2("Writed data len:%d\r\n",ftp_bw);
//	SEGGER_RTT_printf2("Write data over.\r\n");
//	return res;
//}

////�ļ���дָ��ƫ��
////offset:����׵�ַ��ƫ����
////����ֵ:ִ�н��.
//u8 ftp_lseek(u32 offset)
//{
//	return f_lseek(ftp_file,offset);
//}
////��ȡ�ļ���ǰ��дָ���λ��.
////����ֵ:λ��
//u32 ftp_tell(void)
//{
//	return f_tell(ftp_file);
//}
////��ȡ�ļ���С
////����ֵ:�ļ���С
//u32 ftp_size(void)
//{
//	return f_size(ftp_file);
//} 
#if 1
FIL fdst;
FIL fsrc;
FATFS fs;
UINT br, bw;
BYTE textfilebuffer[] = "wxfddddddddddddd\r\n";
BYTE buffer[512];
extern sd_error_enum status;  

ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length);

////////////////////////////////////////////////////////////////////////////////////////////
#define IMG_FILE_PATH "0:/DCMI"
#define IMG_FILE_PATH2 "0:/DCMI2"
//#define printf SEGGER_RTT_printf2
u32 temp=0;
void myfatfs_init(void)
{
    DSTATUS stat = 0;
    uint16_t i = 5;
		nvic_irq_enable(SDIO_IRQn, 0, 0);
    temp=rcu_clock_freq_get(CK_SYS)/1000000;
    SEGGER_RTT_printf2("\r\nSYSCLK: %dM\r\n",temp);
    SEGGER_RTT_printf2("HCLK: %dM\r\n",rcu_clock_freq_get(CK_AHB)/1000000);
    SEGGER_RTT_printf2("PCLK1: %dM\r\n",rcu_clock_freq_get(CK_APB1)/1000000);
    SEGGER_RTT_printf2("PCLK2: %dM\r\n",rcu_clock_freq_get(CK_APB2)/1000000);
    
    do{
        stat = disk_initialize(0);
    }while((stat != 0) && (--i));
    
    SEGGER_RTT_printf2("disk_initialize:%d\r\n",stat);
    SEGGER_RTT_printf2("f_mount:%d\r\n",f_mount(0, &fs));
    if(SD_OK == status)
		{
        SEGGER_RTT_printf2("\r\nSD Card Initialize Success!\r\n");       
    }else
		{
        SEGGER_RTT_printf2("\r\nSD Card Initialize Failed!\r\n");
    }   
}


/*!
    \brief      memory compare function
    \param[in]  src : source data pointer
    \param[in]  dst : destination data pointer
    \param[in]  length : the compare data length
    \param[out] none
    \retval     ErrStatus : ERROR or SUCCESS
*/
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length) 
{
    while (length--){
        if (*src++ != *dst++)
            return ERROR;
    }
    return SUCCESS;
}

#endif





