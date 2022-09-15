#ifndef _QCLOUD_CONNECT_H_
#define _QCLOUD_CONNECT_H_
#include <stdint.h>
//  
//生成生成用户名
int32_t auth_mqtt_username(char *dest, char *product_key, char *device_name);

//生成密码
int32_t auth_mqtt_password(char *dest, char *product_key, char *device_name, char *device_secret);

//生成clientid
int32_t auth_mqtt_clientid(char *dest, char *product_key, char *device_name, char *secure_mode);

//生成连接域名
int32_t auth_mqtt_url(char* dest,char *product_key,char *region);

#endif
