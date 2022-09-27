#ifndef __FATTESTER_H
#define __FATTESTER_H 			   
#include "gd32f4xx.h"	   
#include "ff.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32F407������
//FATFS ���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

// 
//u8 mf_mount(u8* path,u8 mt);
//u8 mf_open(u8*path,u8 mode);
//u8 mf_close(void);
//u8 mf_read(u16 len);
//u8 mf_write(u8*dat,u16 len);
//u8 mf_opendir(u8* path);
//u8 mf_closedir(void);
//u8 mf_readdir(void);
//u8 mf_scan_files(u8 * path);
//u32 mf_showfree(u8 *drv);
//u8 mf_lseek(u32 offset);
//u32 mf_tell(void);
//u32 mf_size(void);
//u8 mf_mkdir(u8*pname);
//u8 mf_fmkfs(u8* path,u8 mode,u16 au);
//u8 mf_unlink(u8 *pname);
//u8 mf_rename(u8 *oldname,u8* newname);
//void mf_getlabel(u8 *path);
//void mf_setlabel(u8 *path); 
//void mf_gets(u16 size);
//u8 mf_putc(u8 c);
//u8 mf_puts(u8*c);


//u8 ftp_open(u8*path,u8 mode);
//u8 ftp_close(void);
//u8 ftp_read(u16 len);
//u8 ftp_write(u8*dat,u16 len);
//u32 ftp_size(void);
//u8 ftp_lseek(u32 offset);

void myfatfs_init(void);
ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length) ;
 
#endif




























