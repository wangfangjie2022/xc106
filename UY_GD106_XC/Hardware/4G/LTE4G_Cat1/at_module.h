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
    POWER_SUCCEED,          //开机成功
	POWER_ERROR,			//未知错误
    POWER_FAIL,          	//开机失败
	POWER_NONE_SIM,			//没有SIM卡
	POWER_NETWORK_FAIL,		//网络连接失败

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


//错误定义
typedef enum
{
    MODULE_ERR_NONE,        //无错误
    MODULE_CMD_DEALING,     //指令正在执行
    MODULE_ERR_PWD,         //模块没上电
    MODULE_ERR_SLEEP,       //模块休眠中
    MODLUE_ERR_INIT,        //模块初始化失败
    MODULE_ERR_NO_INIT,     //模块尚未初始化完成
//    MODULE_ERR_TCP_NONE,    //TCP未连接
//    MODULE_ERR_TCP_ALREADY, //TCP已就绪
//    MODULE_ERR_TCP_FAIL,    //TCP指令执行失败
    
    MODULE_ERR_MQTT_NONE,   //MQTT未连接
    MODULE_ERR_MQTT_ALREADY,//MQTT已连接
    MODULE_ERR_MQTT_FAIL,   //MQTT指令执行失败
    
    MODULE_ERR_NO_ACK,      //无回应
    MODULE_ERR_PARAM,       //参数异常
    
}MODULE_ERRPO;

typedef struct
{
    const char*     at_cmd;         //AT指令
    int             at_len;         //AT指令长度
    const char*     at_will;        //希望收到的回复
    uint32_t        timeout;        //超时时间
    uint8_t         resendCnt;      //重发次数
}MODULE_ATCMD_CFG;

//生成MQTT的参数AT指令
typedef void(*pMakeMqttParserConfigCMD)(const char* clientid,const char* username,const char* password,char buff[],int buffLen);

//生成MQTT的连接地址的AT指令
typedef void(*pMakeMqttAddrConfigCMD)(const char* ip, const int port, const int version, char buff[], int buffLen);	//version? 3:3.1v; 4:3.1.1v

//生成MQTT的连接AT指令
typedef void(*pMakeMqttConnectCMD)(const int keepalive ,char buff[],int buffLen);	//clean_session? 0:保存上次断开的数据； 1：不保存上次的数据

//生成MQTT的发布主题AT指令
typedef void(*pMakeMqttPubCMD)(const char* topic,const int qos, const char* payload,int payloadLen,char buff[],int buffLen);//payloadLen? max:512

//生成MQTT的长消息发布主题AT指令
typedef void(*pMakeMqttPubLongCMD)(const char* topic, const int qos, const char* payload, int payloadLen, char buff[], int buffLen);//payloadLen? max:65535

//最多订阅5个主题
//生成MQTT订阅主题AT指令
typedef void(*pMakeMqttSubCMD)(const char* topic, int qos, char buff[], int buffLen);

//生成MQTT取消订阅主题AT指令
typedef void(*pMakeMqttUnSubCMD)(const char* topic, char buff[], int buffLen);

//生成MQTT的断开连接AT指令
typedef void(*pMakeMqttDisConnectCMD)(char buff[],int buffLen);	//clean_session? 0:保存上次断开的数据； 1：不保存上次的数据

//生成MQTT的释放MQTT资源AT指令
typedef void(*pMakeMqttReleaseCMD)(char buff[],int buffLen);	//clean_session? 0:保存上次断开的数据； 1：不保存上次的数据

//生成MQTT的查询MQTT连接AT指令
typedef void(*pMakeMqttStatuCMD)(char buff[],int buffLen);	//clean_session? 0:保存上次断开的数据； 1：不保存上次的数据

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
    AT_MODULE_POWER_DOWN,   //下电
    AT_MODULE_SLEEP,        //休眠
    AT_MODULE_IDLE,         //空闲
}AT_MODULE_STATUS;

//电源上/下电
typedef void(*pModulePowerOn)(int flg);

//模块复位
typedef void(*pModuleReset)(int flg);

//模块休眠
typedef void(*pModuleSleep)(void);

//上层处理Mqtt报文
typedef void(*pMqttReceivePacket)(int id,const uint8_t* pkt,int len);

//上次处理意外Mqtt连接断开回调函数
typedef void(*pMqttDisconnectURC)(int id);

//上层处理MQTT报文
typedef void(*pMqttPubPacket)(const char* topic,const char* payload,int payloadLen,int qos);

typedef struct ATModuleConfig_t
{
    pModulePowerOn              fpPowerOn;              //模块上电
	pModuleReset              	fpReset;              	//模块上电
    AT_MODULE_STATUS            eModuleStatus;          //模块当前状态
	
    ATConfig                    atConfig;               //AT配置

    const ATModuleMqtt          *atMqttCfg;              //MQTT连接指令
    
    int                         iMqttConnectFlag;       //MQTT连接标识
    
	pMqttReceivePacket          fpMqttReceiveCallback;   	//Mqtt接收数据回调函数
	pMqttDisconnectURC			fpMqttDisconnectCallback;	//Mqtt连接意外断开处理回调函数
    pMqttPubPacket              fpMqttPubCallback;      	//推送报文回调函数
    
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

//结构体初始化
void module_init(ATModuleConfig* cfg);
//模块上电(阻塞)
void module_poweron(ATModuleConfig* cfg);
//模块复位
void module_reset(ATModuleConfig* cfg);
//等待模块连接网络
MODULE_ERRPO module_networking(ATModuleConfig* cfg);
//模块开始运行初始化指令
MODULE_ERRPO module_start(ATModuleConfig* cfg);
//断开连接
MODULE_ERRPO module_mqtt_disconnect(ATModuleConfig* cfg);
//连接mqtt
MODULE_ERRPO module_mqtt_connect(ATModuleConfig* cfg, mqttConnectData Arg);
//设置MQTT连接意外断开处理回调函数
void module_set_mqtt_disconnect_callback(ATModuleConfig* cfg,pMqttDisconnectURC f);
//订阅主题
AT_ERROR MQTTSubscribe(ATModuleConfig* cfg, const char* topicFilter, enum QoS qos, pATCmdURCHandle f, void* arg);
//发布主题
AT_ERROR MQTTPublish(ATModuleConfig* cfg, const char* topicName, MQTTMessage* message);

AT_ERROR MQTTPublish_File(ATModuleConfig* cfg, char *filename);
//AT模块执行循环
void module_loop(ATModuleConfig* cfg);
#endif

