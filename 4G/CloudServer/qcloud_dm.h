#ifndef _QCLOUD_DM_H_
#define _QCLOUD_DM_H_

#include <stdint.h>


typedef struct QrCode_
{
  char* QrCode;
  float Temp;
	char  RHQrCode;
	char  RRQrCode;
} QrCode_t;

//�����������Իظ�����
int32_t dm_property_post_reply(char** dest,char *product_key, char *device_name);

//�������Ͷ�ά��ظ�����
int32_t dm_QrCode_reply(char** dest,char *product_key, char *device_name);

//�������Ͷ�ά������
int32_t QrCode_post(char** dest,char *product_key, char *device_name);

//�������Ͷ�ά��payload
int32_t QrCode_json(char** dest, QrCode_t data);

#endif



