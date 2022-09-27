#include "qcloud_ota.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cJSON.h"

//生成推送版本主题
int32_t version_post(char** dest,char *product_key, char *device_name)
{
    int totalSize = strlen("$ota/report//") + strlen(product_key) + strlen(device_name) + 1;
    *dest = (char*)malloc(totalSize);
    if(!*dest)
	{
		printf("version_post,malloc error！\r\n");
		return 0;
	}
    snprintf(*dest,totalSize,"$ota/report/%s/%s",product_key,device_name);
    return totalSize - 1;
}

//生成推送版本payload
int32_t version_json(char** dest,const char* version)
{
    cJSON *root			= cJSON_CreateObject();
    cJSON *paramObj 	= cJSON_CreateObject();

    cJSON_AddStringToObject(root, "type", "report_version");	//发布主题，类型得用“report”
	
    cJSON_AddStringToObject(paramObj, "version", version);
    cJSON_AddItemToObject(root, "report", paramObj);			//添加二层键值对
    
    *dest = cJSON_Print(root);
	
    cJSON_Delete(root);
    return strlen(*dest);
}

//生成订阅固件升级信息主题
int32_t ota_info_sub(char** dest,char *product_key, char *device_name)
{
  int totalSize = strlen("$ota/update//") + strlen(product_key) + strlen(device_name) + 1;
//    *dest = (char*)malloc(totalSize);
	static char topic_1[100];
	memset(topic_1,0,100);
	*dest = topic_1;
    if(!*dest)
	{
		printf("ota_info_sub,申请动态内存空间失败！\r\n");
		return 0;
	}
        
    snprintf(*dest,totalSize,"$ota/update/%s/%s",product_key,device_name);
    return totalSize - 1;
}
