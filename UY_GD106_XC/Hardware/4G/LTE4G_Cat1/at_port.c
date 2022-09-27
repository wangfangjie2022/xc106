#include "at_port.h"
#include "systick.h"
#include "at_module.h"
#include "SEGGER_RTT.h"

ATModuleConfig      g_ATModule;

#define AT_RECEIVE_BUFFER_LEN       1536   
static uint8_t          s_ATCmdReceiveBuffer[AT_RECEIVE_BUFFER_LEN];


/**\part Module bottom Driver**/

//ģ��IO��ʼ��
static void ModuleIoInit(void)
{
	cat1_io_init(); 
}

//���ڳ�ʼ��
static void ATUartInit(void)
{
//    Cat1_UartInit(115200,s_ATCmdReceiveBuffer,AT_RECEIVE_BUFFER_LEN,100,0,GetTimeTick);
			Cat1_UartInit(230400,s_ATCmdReceiveBuffer,AT_RECEIVE_BUFFER_LEN,100,0,GetTimeTick);//20220723
}

//����ȥ��ʼ��
static void ATUartDeinit(void)
{
   SEGGER_RTT_printf2("There is no ATUartDeinit function\r\n");
}

//���ڷ���
static int ATUartSend(const char* cmd,int len)
{
    Cat1_UartSend(cmd,len);
    return 1;
}

//���ڽ���
static int ATUartRead(char* replyBuff,int maxLen)
{
    return Cat1_UartRead(1,(uint8_t*)replyBuff,maxLen);
}

//��Դ��/�µ�
static void ModulePowerOn(int flg)
{
//	cat1_reset();
	cat1_pwrker();      
}

static void ModuleReset(int flg)
{
	cat1_reset();
//	cat1_pwrker();      
}



/**\part Generate MQTT AT command & The underlying architecture of the MQTT system**/

//����MQTT�Ĳ���ATָ��
void MakeMqttParserConfigCMD(const char* clientid,const char* username,const char* password,char buff[],int buffLen)
{
    snprintf(buff,buffLen,"AT+MCONFIG=\"%s\",\"%s\",\"%s\"\r\n",clientid,username,password);
}

//����MQTT�����ӵ�ַ��ATָ��
void MakeMqttAddrConfigCMD(const char* ip, const int port, const int version, char buff[], int buffLen)	//version? 3:3.1v; 4:3.1.1v
{
	snprintf(buff,buffLen,"AT+MIPSTART=\"%s\",%d,%.d\r\n",ip,port,version);
}

//����MQTT������ATָ��
void MakeMqttConnectCMD(const int keepalive ,char buff[],int buffLen)	//clean_session? 0:�����ϴζϿ������ݣ� 1���������ϴε�����
{
    snprintf(buff,buffLen,"AT+MCONNECT=1,%d\r\n", keepalive);
}

//����MQTT�ķ�������ATָ��
void MakeMqttPubCMD(const char* topic,const int qos, const char* payload,int payloadLen,char buff[],int buffLen)//payloadLen? max:512
{
    snprintf(buff,buffLen,"AT+MPUB=\"%s\",%d,0,\"%s\",%d\r\n", topic, qos, payload, payloadLen);
}

//����MQTT�ĳ���Ϣ��������ATָ��
void MakeMqttPubLongCMD(const char* topic, const int qos, const char* payload, int payloadLen, char buff[], int buffLen)//payloadLen? max:65535
{
    snprintf(buff,buffLen,"AT+MPUBEX=\"%s\",%d,0,%d\r\n",topic, qos, payloadLen);
}

//��ඩ��5������
//����MQTT��������ATָ��
void MakeMqttSubCMD(const char* topic, int qos, char buff[], int buffLen)
{
    
    snprintf(buff,buffLen,"AT+MSUB=\"%s\",%d\r\n",topic, qos);
}

//����MQTTȡ����������ATָ��
void MakeMqttUnSubCMD(const char* topic, char buff[], int buffLen)
{
    
    snprintf(buff,buffLen,"AT+MUNSUB=\"%s\"\r\n",topic);
}

//����MQTT�ĶϿ�����ATָ��
void MakeMqttDisConnectCMD(char buff[],int buffLen)	//clean_session? 0:�����ϴζϿ������ݣ� 1���������ϴε�����
{
    snprintf(buff,buffLen,"AT+MDISCONNECT\r\n");
}

//����MQTT���ͷ�MQTT��ԴATָ��
void MakeMqttReleaseCMD(char buff[],int buffLen)	//clean_session? 0:�����ϴζϿ������ݣ� 1���������ϴε�����
{
    snprintf(buff,buffLen,"AT+MIPCLOSE\r\n");
}

//����MQTT�Ĳ�ѯMQTT����ATָ��
void MakeMqttStatuCMD(char buff[],int buffLen)	//clean_session? 0:�����ϴζϿ������ݣ� 1���������ϴε�����
{
    snprintf(buff,buffLen,"AT+MQTTSTATU\r\n");
}

static const ATPort    s_cATParserPort = 
{
    .fpUartInit = ATUartInit,
	.fpUartDeinit = ATUartDeinit,
    .fpUartSend = ATUartSend,
    .fpUartRead = ATUartRead,
	.fpIoInit =	 ModuleIoInit,
    .cPrefix = "\r\n",
    .cSuffix = "\r\n",
};

//@Target accepts bytes after sending command ( Briefly: Will )

static const ATModuleMqtt   s_cATModuleMqttPort = 
{
    .fpMakeMqttParserConfig = 	MakeMqttParserConfigCMD,
	.fpMakeMqttAddrConfig = 	MakeMqttAddrConfigCMD,
	.fpMakeMqttConnect = 		MakeMqttConnectCMD,
	.fpMakeMqttPub = 			MakeMqttPubCMD,
	.fpMakeMqttPubLong = 		MakeMqttPubLongCMD,
	.fpMakeMqttSub = 			MakeMqttSubCMD,
	.fpMakeMqttUnSub = 			MakeMqttUnSubCMD,	
	.fpMakeMqttDisConnect = 	MakeMqttDisConnectCMD,
	.fpMakeMqttRelease = 		MakeMqttReleaseCMD,
	.fpMakeMqttStatu = 			MakeMqttStatuCMD,
	
	.pcMqttParserConfigSuccess = 	"OK",
	.pcMqttAddrConfigSuccess = 		"+MIPSTART: SUCCESS",
    .pcMqttConnectSuccess =			"+MCONNECT: SUCCESS",
	.pcMqttStatuSuccess = 			"+MQTTSTATU: 1",
	
	.pcMqttSubTopicSuccess = 		"+MSUB: SUCCESS",
	.pcMqttPubTopicSuccess = 		"+MPUB: SUCCESS",

	
	.pcDisconnectHead = 			"+MDISCONNECT: SUCCESS",
	.pcMqttSubTopicURC = 			"+MSUB: \"",
	.pcMqttUpDataTimeURC = 			"+NITZ:",
	
	.pcMakeMqttDisConnectURC = 		"+MQTT:DISCONNECTED",
//    .iTimeout = 20*1000,
	  .iTimeout = 10*1000,
};


//@The underlying architecture of the AT command system

void ATModuleInit(pATGetSystick f)
{
    module_init(&g_ATModule);
    g_ATModule.fpPowerOn = ModulePowerOn;           //ģ���ϵ�
		g_ATModule.fpReset	 = ModuleReset;				//ģ�鸴λ
    g_ATModule.atConfig.atPort = &s_cATParserPort;  //�ײ㺯������
    g_ATModule.atConfig.fpSystick = f;				//��ʱ��������
    g_ATModule.atMqttCfg = &s_cATModuleMqttPort;    //MQTTָ��
    
    at_parser_log(&g_ATModule.atConfig,1);          //������־���
		g_ATModule.atConfig.atPort->fpIoInit();			//��ʼ��IO
}
