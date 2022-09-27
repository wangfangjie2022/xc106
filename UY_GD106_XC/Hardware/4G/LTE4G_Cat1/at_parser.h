#ifndef _ATPARSER_H_
#define _ATPARSER_H_
#include <stdint.h>
#include "queue.h"


typedef enum
{
    AT_ERR_NONE,                //无错误
    AT_ERR_FULL,                //发送队列已满
    AT_ERR_TIMEOUT,             //指令超时
    AT_ERR_ERROR,               //指令返回ERROR
    AT_ERR_DEALING,             //当前有指令正在发送
    AT_ERR_PARAM_INVAILD,       //参数异常
    AT_ERR_NO_URC,              //没有找到URC处理
    AT_ERR_READ_NONE,           //读取数据为空
    AT_ERR_NO_WILL,             //指令没有返回期望的字符串
}AT_ERROR;

#define     MAX_URC_NUM                 10      //最大URC个数
#define     TOPIC_MAX_URC_NUM           5      	//最大主题数5个数
#define     MAX_RECEIVE_BUFF            1536

typedef AT_ERROR(*pATCmdHandle)(void* arg,const char* cmd,int len,AT_ERROR err);
typedef void(*pATCmdURCHandle)(void* arg,const char* cmd,int len);
	
typedef void(*pIoInit)(void);
typedef void(*pATUartInit)(void);
typedef void(*pATUartDeinit)(void);
typedef int(*pATUartSend)(const char* cmd,int len);
typedef int(*pATUartRead)(char* replyBuff,int maxLen);

typedef unsigned long(*pATGetSystick)(void);

typedef struct
{
    const char*     at_cmd;         //AT指令
    int             at_len;         //AT指令长度
    const char*     at_will;        //希望收到的回复
    pATCmdHandle    handler;        //回调处理函数
    void*           arg;            //参数
    uint32_t        timeout;        //超时时间
    uint8_t         resendCnt;      //重发次数
}ATCmdConfig;

//URC配置
typedef struct
{
    const char*         ucURCHead;              //URC头部
    pATCmdURCHandle     pURCHandle;             //URC处理函数
    void *              pvURCArg;               //参数
}ATCmdURCCfg;

//TopicURC配置
typedef struct
{
    const char*         ucTopicURCHead;              //URC头部
    pATCmdURCHandle     pTopicURCHandle;             //URC处理函数
    void *              pvTopicURCArg;               //参数
}TopicCmdURCCfg;

typedef enum
{
    AT_CMD_IDLE,        //空闲
    AT_CMD_WAIT,        //等待
}AT_CMD_STATUS;

typedef struct
{
    pATUartInit         fpUartInit;
    pATUartDeinit       fpUartDeinit;
    pATUartSend         fpUartSend;
    pATUartRead         fpUartRead;
	pIoInit				fpIoInit;
    const char*         cPrefix;                //前缀
    const char*         cSuffix;                //后缀
}ATPort;

typedef struct ATConfig
{
    const ATPort*        atPort;                 //移植接口
    pATGetSystick        fpSystick;
    
    AT_CMD_STATUS       eATCmdStatus;           //当前流程状态
    uint8_t             ucCurrentSendCnt;       //当前重发次数
    unsigned long       ulCurrentTimeout;       //当前重发超时时间
    Queue               qATQueue;               //AT发送队列
	
    ATCmdURCCfg         sATURCCfg[MAX_URC_NUM]; //URC处理
	TopicCmdURCCfg		sTopicURCCfg[TOPIC_MAX_URC_NUM]; //URC处理
	
    char                cReceiveBuff[MAX_RECEIVE_BUFF];
    int                 iLogEnable;
}ATConfig;

typedef AT_ERROR(*pATCmdHandle)(void* arg,const char* cmd,int len,AT_ERROR err);


//初始化
void at_parser_init(ATConfig *cfg);

//循环
void at_parser_loop(ATConfig *cfg);

//非阻塞发送AT
AT_ERROR at_parser_send_no_wait(ATConfig *cfg,const char* cmd,int atLen,const char* will,pATCmdHandle handler,void* arg,uint32_t timeout,uint8_t resendCnt);

//阻塞发送AT指令
AT_ERROR  at_parser_send_wait(ATConfig *cfg,const char* cmd,int atLen,const char* will,const char** recBuff,uint32_t timeout,uint8_t resendCnt);

//20220622
AT_ERROR  at_parser_send_wait2(ATConfig *cfg,const char* cmd,int cmdLen,const char* will,const char* error,const char** recBuff, uint8_t resendCnt, uint32_t timeout);

//20220728
AT_ERROR  at_parser_send_wait_file(ATConfig *cfg,const char* cmd,int atLen,const char* will,const char** recBuff,uint32_t timeout,uint8_t resendCnt);

//读取和处理指令数据
AT_ERROR at_parser_read_handle(ATConfig *cfg,const char* will,pATCmdHandle handle,void* arg);


AT_ERROR at_parser_read_handle2(ATConfig *cfg,const char* will,const char* error,pATCmdHandle handle,void* arg);
//注册URC事件回调函数
AT_ERROR at_parser_register_urc(ATConfig *cfg,const char* cmd,pATCmdURCHandle f,void* arg);

//直接发送数据
AT_ERROR at_parser_send(ATConfig *cfg,const char* cmd,int atLen);

//读入数据
AT_ERROR at_parser_read(ATConfig *cfg,unsigned char* buff,int *len,int maxLen,int timeout);

//启用日志
void at_parser_log(ATConfig *cfg,int enable);

AT_ERROR  at_parser_send_wait3(ATConfig *cfg,const char* cmd,int cmdLen,const char* will,const char* error,char* recBuff, uint8_t resendCnt, uint32_t timeout);//20220817

AT_ERROR  at_parser_send_no_wait2(ATConfig *cfg,const char* cmd,int atLen);

#endif

