#include "qcloud_dm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"



////�����������Իظ�����
//int32_t dm_property_post_reply(char** dest,char *product_key, char *device_name)
//{
//	int totalSize = strlen("$thing/down/property//") + strlen(product_key) + strlen(device_name) + 1;
//    *dest = (char*)malloc(totalSize);
//    if(!*dest)
//	{
//		printf("dm_property_post_reply,���붯̬�ڴ�ռ�ʧ�ܣ�\r\n");
//		 return 0;
//	}
//    snprintf(*dest,totalSize,"$thing/down/property/%s/%s",product_key,device_name);
//    return totalSize - 1;
//}


////�������Ͷ�ά��ظ�����
//int32_t dm_QrCode_reply(char** dest,char *product_key, char *device_name)
//{
//	int totalSize = strlen("%s/%s/down/QrCode") + strlen(product_key) + strlen(device_name) + 1;
//    *dest = (char*)malloc(totalSize);
//	static char topic_2[100];
//	memset(topic_2,0,100);
//	*dest = topic_2;
//    if(!*dest)
//	{
//		printf("dm_property_post_reply,malloc ERROR��\r\n");
//		 return 0;
//	}
//    snprintf(*dest,totalSize,"%s/%s/down/QrCode",product_key,device_name);
//    return totalSize - 1;
//}

////�������Ͷ�ά������
//int32_t QrCode_post(char** dest,char *product_key, char *device_name)
//{
//    int totalSize = strlen("$thing/up/property//") + strlen(product_key) + strlen(device_name) + 1;
//    *dest = (char*)malloc(totalSize);
//    if(!*dest)
//	{
//		printf("version_post,malloc ERROR��\r\n");
//		return 0;
//	}
//    snprintf(*dest,totalSize,"$thing/up/property/%s/%s",product_key,device_name);
//    return totalSize - 1;
//}

////�������Ͷ�ά��payload
//int32_t QrCode_json(char** dest, QrCode_t data)
//{
//    cJSON *root			= cJSON_CreateObject();
//    cJSON *paramsObj 	= cJSON_CreateObject();

//    cJSON_AddStringToObject(root, "method", "report");	//��Ӽ�ֵ��
//    cJSON_AddStringToObject(root, "clientToken", "123456");
//	
//	if(data.Temp >= (float)1)
//		cJSON_AddNumberToObject(paramsObj, "Temp", data.Temp);
//	else
//		cJSON_AddStringToObject(paramsObj, "QrCode", data.QrCode);

//    cJSON_AddItemToObject(root, "params", paramsObj);			//��Ӷ����ֵ��
//	
//    *dest = cJSON_Print(root);
//	
//    cJSON_Delete(root);
//    return strlen(*dest);
//}


