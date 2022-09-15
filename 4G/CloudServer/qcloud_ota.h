#ifndef _QCLOUD_OTA_H_
#define _QCLOUD_OTA_H_
#include <stdint.h>

typedef enum
{
    OTA_UPDATE_FAIL = -1,           //升级失败
    OTA_DOWNLOAD_FAIL = -2,         //下载失败
    OTA_VERIFY_FAIL = -3,           //校验失败
    OTA_PROGRAM_FAIL = -4,          //烧写失败
}OTA_PROCESS;

//生成推送版本主题
int32_t version_post(char** dest,char *product_key, char *device_name);

//生成推送版本payload
int32_t version_json(char** dest,const char* version);

//生成订阅固件升级信息主题
int32_t ota_info_sub(char** dest,char *product_key, char *device_name);

//生成推送升级进度主题
int32_t ota_process_post(char** dest,char *product_key, char *device_name);

//生成升级进度payload
int32_t ota_process_json(char** dest,int id,int step);

//生成主动拉取升级信息主题
int32_t ota_request_topic(char** dest,char *product_key, char *device_name);

//生成主动拉取升级信息payload
int32_t ota_request_payload(char** dest,int id);

#endif
