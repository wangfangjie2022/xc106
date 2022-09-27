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

//生成推送属性回复主题
int32_t dm_property_post_reply(char** dest,char *product_key, char *device_name);

//生成推送二维码回复主题
int32_t dm_QrCode_reply(char** dest,char *product_key, char *device_name);

//生成推送二维码主题
int32_t QrCode_post(char** dest,char *product_key, char *device_name);

//生成推送二维码payload
int32_t QrCode_json(char** dest, QrCode_t data);

#endif



