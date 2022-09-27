#ifndef __MQTT_content_h__
#define __MQTT_content_h__

#include "CloudServerConnect.h"

// #define MAX_CONN_ID_LEN (6)


/**
 * \brief         simple hext2str.
 * \para[in]			num
 */
void MQTTinit(void);

/**
 * \brief         simple hext2str.
 * \para[in]			num
 */
void connectMQTT(void);

/**
 * \brief         simple hext2str.
 * \para[in]			num
 */
void MQTTtopic(void);


/**
 * \brief         MQTTLogout.
 * \para[in]			num
 */
void MQTTlogout(void);



void auth_mqtt_url(char* dest);
int32_t ota_info_subAll(char** dest);
int32_t ota_info_subOne(char** dest);

int32_t reset_subDTU(char** dest);
int32_t info_subVolt(char** dest);

void info_subVoltCallback(void* arg,const char* data,int len);
void reset_subDTUCallback(void* arg,const char* data,int len);
#endif 
