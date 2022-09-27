#ifndef _ATPARSER_H_
#define _ATPARSER_H_
#include <stdint.h>
#include "queue.h"


typedef enum
{
    AT_ERR_NONE,                //�޴���
    AT_ERR_FULL,                //���Ͷ�������
    AT_ERR_TIMEOUT,             //ָ�ʱ
    AT_ERR_ERROR,               //ָ���ERROR
    AT_ERR_DEALING,             //��ǰ��ָ�����ڷ���
    AT_ERR_PARAM_INVAILD,       //�����쳣
    AT_ERR_NO_URC,              //û���ҵ�URC����
    AT_ERR_READ_NONE,           //��ȡ����Ϊ��
    AT_ERR_NO_WILL,             //ָ��û�з����������ַ���
}AT_ERROR;

#define     MAX_URC_NUM                 10      //���URC����
#define     TOPIC_MAX_URC_NUM           5      	//���������5����
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
    const char*     at_cmd;         //ATָ��
    int             at_len;         //ATָ���
    const char*     at_will;        //ϣ���յ��Ļظ�
    pATCmdHandle    handler;        //�ص�������
    void*           arg;            //����
    uint32_t        timeout;        //��ʱʱ��
    uint8_t         resendCnt;      //�ط�����
}ATCmdConfig;

//URC����
typedef struct
{
    const char*         ucURCHead;              //URCͷ��
    pATCmdURCHandle     pURCHandle;             //URC������
    void *              pvURCArg;               //����
}ATCmdURCCfg;

//TopicURC����
typedef struct
{
    const char*         ucTopicURCHead;              //URCͷ��
    pATCmdURCHandle     pTopicURCHandle;             //URC������
    void *              pvTopicURCArg;               //����
}TopicCmdURCCfg;

typedef enum
{
    AT_CMD_IDLE,        //����
    AT_CMD_WAIT,        //�ȴ�
}AT_CMD_STATUS;

typedef struct
{
    pATUartInit         fpUartInit;
    pATUartDeinit       fpUartDeinit;
    pATUartSend         fpUartSend;
    pATUartRead         fpUartRead;
	pIoInit				fpIoInit;
    const char*         cPrefix;                //ǰ׺
    const char*         cSuffix;                //��׺
}ATPort;

typedef struct ATConfig
{
    const ATPort*        atPort;                 //��ֲ�ӿ�
    pATGetSystick        fpSystick;
    
    AT_CMD_STATUS       eATCmdStatus;           //��ǰ����״̬
    uint8_t             ucCurrentSendCnt;       //��ǰ�ط�����
    unsigned long       ulCurrentTimeout;       //��ǰ�ط���ʱʱ��
    Queue               qATQueue;               //AT���Ͷ���
	
    ATCmdURCCfg         sATURCCfg[MAX_URC_NUM]; //URC����
	TopicCmdURCCfg		sTopicURCCfg[TOPIC_MAX_URC_NUM]; //URC����
	
    char                cReceiveBuff[MAX_RECEIVE_BUFF];
    int                 iLogEnable;
}ATConfig;

typedef AT_ERROR(*pATCmdHandle)(void* arg,const char* cmd,int len,AT_ERROR err);


//��ʼ��
void at_parser_init(ATConfig *cfg);

//ѭ��
void at_parser_loop(ATConfig *cfg);

//����������AT
AT_ERROR at_parser_send_no_wait(ATConfig *cfg,const char* cmd,int atLen,const char* will,pATCmdHandle handler,void* arg,uint32_t timeout,uint8_t resendCnt);

//��������ATָ��
AT_ERROR  at_parser_send_wait(ATConfig *cfg,const char* cmd,int atLen,const char* will,const char** recBuff,uint32_t timeout,uint8_t resendCnt);

//20220622
AT_ERROR  at_parser_send_wait2(ATConfig *cfg,const char* cmd,int cmdLen,const char* will,const char* error,const char** recBuff, uint8_t resendCnt, uint32_t timeout);

//20220728
AT_ERROR  at_parser_send_wait_file(ATConfig *cfg,const char* cmd,int atLen,const char* will,const char** recBuff,uint32_t timeout,uint8_t resendCnt);

//��ȡ�ʹ���ָ������
AT_ERROR at_parser_read_handle(ATConfig *cfg,const char* will,pATCmdHandle handle,void* arg);


AT_ERROR at_parser_read_handle2(ATConfig *cfg,const char* will,const char* error,pATCmdHandle handle,void* arg);
//ע��URC�¼��ص�����
AT_ERROR at_parser_register_urc(ATConfig *cfg,const char* cmd,pATCmdURCHandle f,void* arg);

//ֱ�ӷ�������
AT_ERROR at_parser_send(ATConfig *cfg,const char* cmd,int atLen);

//��������
AT_ERROR at_parser_read(ATConfig *cfg,unsigned char* buff,int *len,int maxLen,int timeout);

//������־
void at_parser_log(ATConfig *cfg,int enable);

AT_ERROR  at_parser_send_wait3(ATConfig *cfg,const char* cmd,int cmdLen,const char* will,const char* error,char* recBuff, uint8_t resendCnt, uint32_t timeout);//20220817

AT_ERROR  at_parser_send_no_wait2(ATConfig *cfg,const char* cmd,int atLen);

#endif

