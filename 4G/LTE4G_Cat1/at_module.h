#ifndef _AT_MODULE_H_
#define _AT_MODULE_H_
#include <stdint.h>
#include "at_parser.h"
#include <stdio.h>

typedef struct ATModuleConfig_t ATModuleConfig;

extern char *mode_imei;
extern char *card_ccid;

#define IMEI_MAX_SIZE 30
#define CCID_MAX_SIZE 30

typedef enum
{
    POWER_SUCCEED,          //�����ɹ�
	POWER_ERROR,			//δ֪����
    POWER_FAIL,          	//����ʧ��
	POWER_NONE_SIM,			//û��SIM��
	POWER_NETWORK_FAIL,		//��������ʧ��

}POWER_ENUM;

enum QoS { QOS0, QOS1, QOS2, SUBFAIL=0x80 };

typedef struct MQTTMessage
{
    enum QoS qos;
    unsigned char retained;
    unsigned char dup;
    unsigned short id;
    void *payload;
    size_t payloadlen;
} MQTTMessage;


//������
typedef enum
{
    MODULE_ERR_NONE,        //�޴���
    MODULE_CMD_DEALING,     //ָ������ִ��
    MODULE_ERR_PWD,         //ģ��û�ϵ�
    MODULE_ERR_SLEEP,       //ģ��������
    MODLUE_ERR_INIT,        //ģ���ʼ��ʧ��
    MODULE_ERR_NO_INIT,     //ģ����δ��ʼ�����
//    MODULE_ERR_TCP_NONE,    //TCPδ����
//    MODULE_ERR_TCP_ALREADY, //TCP�Ѿ���
//    MODULE_ERR_TCP_FAIL,    //TCPָ��ִ��ʧ��
    
    MODULE_ERR_MQTT_NONE,   //MQTTδ����
    MODULE_ERR_MQTT_ALREADY,//MQTT������
    MODULE_ERR_MQTT_FAIL,   //MQTTָ��ִ��ʧ��
    
    MODULE_ERR_NO_ACK,      //�޻�Ӧ
    MODULE_ERR_PARAM,       //�����쳣
    
}MODULE_ERRPO;

typedef struct
{
    const char*     at_cmd;         //ATָ��
    int             at_len;         //ATָ���
    const char*     at_will;        //ϣ���յ��Ļظ�
    uint32_t        timeout;        //��ʱʱ��
    uint8_t         resendCnt;      //�ط�����
}MODULE_ATCMD_CFG;

//����MQTT�Ĳ���ATָ��
typedef void(*pMakeMqttParserConfigCMD)(const char* clientid,const char* username,const char* password,char buff[],int buffLen);

//����MQTT�����ӵ�ַ��ATָ��
typedef void(*pMakeMqttAddrConfigCMD)(const char* ip, const int port, const int version, char buff[], int buffLen);	//version? 3:3.1v; 4:3.1.1v

//����MQTT������ATָ��
typedef void(*pMakeMqttConnectCMD)(const int keepalive ,char buff[],int buffLen);	//clean_session? 0:�����ϴζϿ������ݣ� 1���������ϴε�����

//����MQTT�ķ�������ATָ��
typedef void(*pMakeMqttPubCMD)(const char* topic,const int qos, const char* payload,int payloadLen,char buff[],int buffLen);//payloadLen? max:512

//����MQTT�ĳ���Ϣ��������ATָ��
typedef void(*pMakeMqttPubLongCMD)(const char* topic, const int qos, const char* payload, int payloadLen, char buff[], int buffLen);//payloadLen? max:65535

//��ඩ��5������
//����MQTT��������ATָ��
typedef void(*pMakeMqttSubCMD)(const char* topic, int qos, char buff[], int buffLen);

//����MQTTȡ����������ATָ��
typedef void(*pMakeMqttUnSubCMD)(const char* topic, char buff[], int buffLen);

//����MQTT�ĶϿ�����ATָ��
typedef void(*pMakeMqttDisConnectCMD)(char buff[],int buffLen);	//clean_session? 0:�����ϴζϿ������ݣ� 1���������ϴε�����

//����MQTT���ͷ�MQTT��ԴATָ��
typedef void(*pMakeMqttReleaseCMD)(char buff[],int buffLen);	//clean_session? 0:�����ϴζϿ������ݣ� 1���������ϴε�����

//����MQTT�Ĳ�ѯMQTT����ATָ��
typedef void(*pMakeMqttStatuCMD)(char buff[],int buffLen);	//clean_session? 0:�����ϴζϿ������ݣ� 1���������ϴε�����

typedef struct
{
    pMakeMqttParserConfigCMD    fpMakeMqttParserConfig;
    pMakeMqttAddrConfigCMD      fpMakeMqttAddrConfig;
    pMakeMqttConnectCMD         fpMakeMqttConnect;
    pMakeMqttPubCMD         	fpMakeMqttPub;
    pMakeMqttPubLongCMD    		fpMakeMqttPubLong;
    pMakeMqttSubCMD				fpMakeMqttSub;
    pMakeMqttUnSubCMD       	fpMakeMqttUnSub;
    pMakeMqttDisConnectCMD      fpMakeMqttDisConnect;
	pMakeMqttReleaseCMD			fpMakeMqttRelease;
	pMakeMqttStatuCMD			fpMakeMqttStatu;

	const char*                 pcMqttParserConfigSuccess;
	const char* 				pcMqttAddrConfigSuccess;
	const char*					pcMqttConnectSuccess;
	const char*					pcMqttStatuSuccess;
	
	const char* 				pcMqttSubTopicSuccess;
	const char* 				pcMqttPubTopicSuccess;
	
	const char*					pcDisconnectHead;
	
	const char* 				pcMakeMqttDisConnectURC;
	const char* 				pcMqttSubTopicURC;
	const char* 				pcMqttUpDataTimeURC;

    int                         iTimeout;
}ATModuleMqtt;

typedef enum
{
    AT_MODULE_POWER_DOWN,   //�µ�
    AT_MODULE_SLEEP,        //����
    AT_MODULE_IDLE,         //����
}AT_MODULE_STATUS;

//��Դ��/�µ�
typedef void(*pModulePowerOn)(int flg);

//ģ�鸴λ
typedef void(*pModuleReset)(int flg);

//ģ������
typedef void(*pModuleSleep)(void);

//�ϲ㴦��Mqtt����
typedef void(*pMqttReceivePacket)(int id,const uint8_t* pkt,int len);

//�ϴδ�������Mqtt���ӶϿ��ص�����
typedef void(*pMqttDisconnectURC)(int id);

//�ϲ㴦��MQTT����
typedef void(*pMqttPubPacket)(const char* topic,const char* payload,int payloadLen,int qos);

typedef struct ATModuleConfig_t
{
    pModulePowerOn              fpPowerOn;              //ģ���ϵ�
	pModuleReset              	fpReset;              	//ģ���ϵ�
    AT_MODULE_STATUS            eModuleStatus;          //ģ�鵱ǰ״̬
	
    ATConfig                    atConfig;               //AT����

    const ATModuleMqtt          *atMqttCfg;              //MQTT����ָ��
    
    int                         iMqttConnectFlag;       //MQTT���ӱ�ʶ
    
	pMqttReceivePacket          fpMqttReceiveCallback;   	//Mqtt�������ݻص�����
	pMqttDisconnectURC			fpMqttDisconnectCallback;	//Mqtt��������Ͽ�����ص�����
    pMqttPubPacket              fpMqttPubCallback;      	//���ͱ��Ļص�����
    
}ATModuleConfig;

typedef struct mqttConnectData_t
{
	const char* 			username;
	const char* 			password;
	const char*				mqtturl;
	const char*				devicename;
	int						urlport;
	int						mqttversion;
	int 					keepalive;

    
}mqttConnectData;

//�ṹ���ʼ��
void module_init(ATModuleConfig* cfg);
//ģ���ϵ�(����)
void module_poweron(ATModuleConfig* cfg);
//ģ�鸴λ
void module_reset(ATModuleConfig* cfg);
//�ȴ�ģ����������
MODULE_ERRPO module_networking(ATModuleConfig* cfg);
//ģ�鿪ʼ���г�ʼ��ָ��
MODULE_ERRPO module_start(ATModuleConfig* cfg);
//�Ͽ�����
MODULE_ERRPO module_mqtt_disconnect(ATModuleConfig* cfg);
//����mqtt
MODULE_ERRPO module_mqtt_connect(ATModuleConfig* cfg, mqttConnectData Arg);
//����MQTT��������Ͽ�����ص�����
void module_set_mqtt_disconnect_callback(ATModuleConfig* cfg,pMqttDisconnectURC f);
//��������
AT_ERROR MQTTSubscribe(ATModuleConfig* cfg, const char* topicFilter, enum QoS qos, pATCmdURCHandle f, void* arg);
//��������
AT_ERROR MQTTPublish(ATModuleConfig* cfg, const char* topicName, MQTTMessage* message);

AT_ERROR MQTTPublish_File(ATModuleConfig* cfg, char *filename);
//ATģ��ִ��ѭ��
void module_loop(ATModuleConfig* cfg);
#endif

