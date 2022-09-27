#ifndef __UPLOAD_FILE_H
#define __UPLOAD_FILE_H	 

#include "gd32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "sys.h"

#define UPLOAD_FLAG_NO 0
#define UPLOAD_FLAG_BEGIN 1
#define UPLOAD_FLAG_FINISH 2
#define RECORD_PATH "0:/DCMI"
#define RECORD_PATH2 "0:/DCMI2"
#define RECORD_PATH3 "/DCMI"
#define UPLOAD_FILE_TIMEOUT 6000
#define TRAN_FILE_LEN 50
#define LOCAL_FILE_LEN 50

//#define MQTT_MAX_FLIE_SEND_LENGTH 20000
#define MQTT_MAX_FLIE_SEND_LENGTH 3500
typedef struct
{
	u8 upload_flag;
	char 		local_filename[LOCAL_FILE_LEN];		//ÎÄ¼þÂ·¾¶
}UPLOAD_FILE_PARA;
extern UPLOAD_FILE_PARA upload_file_para;

extern char *payload_file;
extern char topic_file[TOPIC_SIZE];
extern u8 upload_flag;
extern AT_ERROR tranfile_err;

void upload_file_task(void *pdata);
void PublishFile(char *filename);

#endif
