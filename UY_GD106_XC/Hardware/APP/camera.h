#ifndef __CAMERA_H
#define __CAMERA_H	 

#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "sys.h"


void jpeg_data_process(void);
void jpeg_test(void);
void camera_task(void *pdata);
void camera_task2(void);
void write_img_file(u8 *buffer,u32 buffer_length);
void camera_init(void);
void camera_take_pic(char* file_name);
u8 camera_take_pic_base64(char* file_name);

#endif
