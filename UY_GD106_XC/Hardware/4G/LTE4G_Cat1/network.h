#ifndef _NETWORK_H_
#define _NETWORK_H_
#include <stdint.h>
#include "queue.h"
#include "at_parser.h"
extern uint8_t Property_start;
extern char mqtt_passware[64];

#define TOPIC_SIZE 50
#define PAYLOAD_SIZE 100

typedef enum
{
    NETWORK_IDLE,
    NETWORK_CONNECTING,
    NETWORK_CONNECTED,
    NETWORK_WAIT,
}NETWORK_STATUS;

typedef struct
{
	char		DeviceName[16];		
	char 		UserName[64];
	char		Password[128];
}MqttObject;


void NetworkInit(void);

void NetworkStart(void);

void NetworkLoop(char *topic,char *cJSONoutbuffer);

AT_ERROR PublishBeat(void);

void OTAParamSubCallback(void* arg,const char* data,int len);
uint8_t compareversion(char *cha);
void check_if_first_boot(void);
extern char *cJSONoutbuffer;
extern MqttObject mqttobject;
void PublishFile(char *filename);
AT_ERROR PublishData2(char* topic,char* cJSONoutbuffer);
void MqttMqttDisconnectCallback2(void);

#endif

