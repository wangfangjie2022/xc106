#include "at_port.h"
#include "systick.h"
#include "at_module.h"
#include "SEGGER_RTT.h"

ATModuleConfig      g_ATModule;

#define AT_RECEIVE_BUFFER_LEN       1536   
static uint8_t          s_ATCmdReceiveBuffer[AT_RECEIVE_BUFFER_LEN];


/**\part Module bottom Driver**/

//模块IO初始化
static void ModuleIoInit(void)
{
	cat1_io_init(); 
}

//串口初始化
static void ATUartInit(void)
{
//    Cat1_UartInit(115200,s_ATCmdReceiveBuffer,AT_RECEIVE_BUFFER_LEN,100,0,GetTimeTick);
			Cat1_UartInit(230400,s_ATCmdReceiveBuffer,AT_RECEIVE_BUFFER_LEN,100,0,GetTimeTick);//20220723
}

//串口去初始化
static void ATUartDeinit(void)
{
   SEGGER_RTT_printf2("There is no ATUartDeinit function\r\n");
}

//串口发送
static int ATUartSend(const char* cmd,int len)
{
    Cat1_UartSend(cmd,len);
    return 1;
}

//串口接收
static int ATUartRead(char* replyBuff,int maxLen)
{
    return Cat1_UartRead(1,(uint8_t*)replyBuff,maxLen);
}

//电源上/下电
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

//生成MQTT的参数AT指令
void MakeMqttParserConfigCMD(const char* clientid,const char* username,const char* password,char buff[],int buffLen)
{
    snprintf(buff,buffLen,"AT+MCONFIG=\"%s\",\"%s\",\"%s\"\r\n",clientid,username,password);
}

//生成MQTT的连接地址的AT指令
void MakeMqttAddrConfigCMD(const char* ip, const int port, const int version, char buff[], int buffLen)	//version? 3:3.1v; 4:3.1.1v
{
	snprintf(buff,buffLen,"AT+MIPSTART=\"%s\",%d,%.d\r\n",ip,port,version);
}

//生成MQTT的连接AT指令
void MakeMqttConnectCMD(const int keepalive ,char buff[],int buffLen)	//clean_session? 0:保存上次断开的数据； 1：不保存上次的数据
{
    snprintf(buff,buffLen,"AT+MCONNECT=1,%d\r\n", keepalive);
}

//生成MQTT的发布主题AT指令
void MakeMqttPubCMD(const char* topic,const int qos, const char* payload,int payloadLen,char buff[],int buffLen)//payloadLen? max:512
{
    snprintf(buff,buffLen,"AT+MPUB=\"%s\",%d,0,\"%s\",%d\r\n", topic, qos, payload, payloadLen);
}

//生成MQTT的长消息发布主题AT指令
void MakeMqttPubLongCMD(const char* topic, const int qos, const char* payload, int payloadLen, char buff[], int buffLen)//payloadLen? max:65535
{
    snprintf(buff,buffLen,"AT+MPUBEX=\"%s\",%d,0,%d\r\n",topic, qos, payloadLen);
}

//最多订阅5个主题
//生成MQTT订阅主题AT指令
void MakeMqttSubCMD(const char* topic, int qos, char buff[], int buffLen)
{
    
    snprintf(buff,buffLen,"AT+MSUB=\"%s\",%d\r\n",topic, qos);
}

//生成MQTT取消订阅主题AT指令
void MakeMqttUnSubCMD(const char* topic, char buff[], int buffLen)
{
    
    snprintf(buff,buffLen,"AT+MUNSUB=\"%s\"\r\n",topic);
}

//生成MQTT的断开连接AT指令
void MakeMqttDisConnectCMD(char buff[],int buffLen)	//clean_session? 0:保存上次断开的数据； 1：不保存上次的数据
{
    snprintf(buff,buffLen,"AT+MDISCONNECT\r\n");
}

//生成MQTT的释放MQTT资源AT指令
void MakeMqttReleaseCMD(char buff[],int buffLen)	//clean_session? 0:保存上次断开的数据； 1：不保存上次的数据
{
    snprintf(buff,buffLen,"AT+MIPCLOSE\r\n");
}

//生成MQTT的查询MQTT连接AT指令
void MakeMqttStatuCMD(char buff[],int buffLen)	//clean_session? 0:保存上次断开的数据； 1：不保存上次的数据
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
    g_ATModule.fpPowerOn = ModulePowerOn;           //模块上电
		g_ATModule.fpReset	 = ModuleReset;				//模块复位
    g_ATModule.atConfig.atPort = &s_cATParserPort;  //底层函数设置
    g_ATModule.atConfig.fpSystick = f;				//定时函数设置
    g_ATModule.atMqttCfg = &s_cATModuleMqttPort;    //MQTT指令
    
    at_parser_log(&g_ATModule.atConfig,1);          //启动日志监控
		g_ATModule.atConfig.atPort->fpIoInit();			//初始化IO
}
