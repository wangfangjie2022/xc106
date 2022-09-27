#include "at_parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"

#define AT_PREFIX               "\r\n"      //ǰ׺
#define AT_SUFFIX               "\r\n"      //��׺
#define AT_PREFIXASUFFIX        "\r\n\r\n"


/**
* @������:      memstr
* @��������:   �ڴ��������ض����ַ���
* @�������:    src:������,sub�����ݣ�size�����ݳ���
* @�������:    ��
* @����ֵ:      ���ظ�λ�õ�ָ�룬���Ҳ��������ؿ�ָ�롣
*/
static char* memstr(const void* src,const char* sub,int size)
{
    int subSize = 0;
    int i = 0;
    int lenDiff = 0;
    if (!src || !sub)	//����һ����ַΪ���򷵻�
        return NULL;
    subSize = strlen(sub);
    if (subSize > size)
        return NULL;
    lenDiff = size - subSize;
    for (i = 0; i < lenDiff; i++)
    {
        if (memcmp((char*)src + i, sub, subSize) == 0)
        {
            return (char*)src + i;
        }
    }
    
    return NULL;
}

/**
* @������:      at_get_line
* @��������:   
* @�������:    cfg:�ͻ���,headCmd������ͷ��size�����ݳ��ȣ�payLoadCmd����������nextHead����һ��ͷ�ַ�����ַ
* @�������:    ��
* @����ֵ:      ���ظ�λ�õ�ָ�룬���Ҳ��������ؿ�ָ�롣
*/
static int at_get_line(ATConfig* cfg,const char* headCmd,int size, char* payLoadCmd, const char** nextHead)
{
    const char* prefix = 0;
    const char* prefixAsuffix = 0;
    const char* payloadHead = 0;
    int len = 0;
    if (headCmd == 0)
        return 0;
    prefix = memstr(headCmd, AT_PREFIX,size);	//ǰ׺������ַ
    if (prefix)
    {
        payloadHead = prefix + strlen(AT_PREFIX);				//��Ч���ݵ�ַ
        size = size - (prefix + strlen(AT_PREFIX) - headCmd);	//��Ч���ݳ���
        prefixAsuffix = memstr(prefix + strlen(AT_PREFIX), AT_PREFIXASUFFIX, size);       //Ѱ��"\r\n\r\n"
        if (prefixAsuffix)
        {
            *nextHead = prefixAsuffix + strlen(AT_SUFFIX);          //���¶�λ��"\r\n"
            len = prefixAsuffix - prefix - strlen(AT_PREFIX);		//��Ҫ�����ݳ���
            memcpy(payLoadCmd, payloadHead, len);					//��ȡ��Ҫ�ĵ�ַ
            payLoadCmd[len] = 0;									//���ӽ�����־λ
        }
        else
        {
            *nextHead = 0;
            len = size;
            memcpy(payLoadCmd, payloadHead, len);
            payLoadCmd[len] = 0;
        }
    }
    else
    {
        memcpy(payLoadCmd, headCmd,size);
        return size;
    }

    return len;
    
}

//�ж�ATConfig�Ƿ�Ϸ�
AT_ERROR    at_config_vaild(ATConfig *cfg)
{
    if( cfg && cfg->atPort->fpUartInit &&\
		cfg->atPort->fpUartDeinit &&\
		cfg->atPort->fpUartSend &&\
		cfg->atPort->fpUartRead &&\
		cfg->fpSystick)
    {
        return AT_ERR_NONE;
    }
    return AT_ERR_PARAM_INVAILD;
}

//��ʼ��
void at_parser_init(ATConfig *cfg)
{
    if(at_config_vaild(cfg) == AT_ERR_NONE)
    {
        cfg->atPort->fpUartInit();
    }
}

//����URC�¼�
AT_ERROR at_urc_handle(ATConfig *cfg,const char* data,int len)
{
    int i = 0;
    for(i = 0;i < MAX_URC_NUM;i++)
    {
		if(cfg->sATURCCfg[i].ucURCHead && strncmp(cfg->sATURCCfg[i].ucURCHead,data,strlen(cfg->sATURCCfg[i].ucURCHead)) == 0)	//Ѱ��URC
        {
            cfg->sATURCCfg[i].pURCHandle(cfg->sATURCCfg[i].pvURCArg,data,len);		//�ҵ���ӦURC��ִ�лص�����
            return AT_ERR_NONE;
        }
    }
    return AT_ERR_NO_URC;
}

//ע��URC�¼��ص�����
AT_ERROR at_parser_register_urc(ATConfig *cfg,const char* cmd,pATCmdURCHandle f,void* arg)
{
    uint8_t i = 0;
    for(i = 0;i < MAX_URC_NUM;i++)
    {	
		if(strncmp(cfg->sATURCCfg[i].ucURCHead,cmd,strlen(cfg->sATURCCfg[i].ucURCHead)) == 0)
		{
			SEGGER_RTT_printf2("%s URC duplicate registration.\r\n", cmd);
			break;
		}
			
        if(cfg->sATURCCfg[i].ucURCHead == 0)
        {
            cfg->sATURCCfg[i].ucURCHead = cmd;
            cfg->sATURCCfg[i].pURCHandle = f;
            cfg->sATURCCfg[i].pvURCArg = arg;
            break;
        }
    }
    return AT_ERR_NONE;
}

//��ȡ�ʹ���ָ������
AT_ERROR at_parser_read_handle_file(ATConfig *cfg,const char* will,pATCmdHandle handle,void* arg)
{
    int size = cfg->atPort->fpUartRead(cfg->cReceiveBuff,sizeof(cfg->cReceiveBuff));
		AT_ERROR	eATerror =AT_ERR_NO_WILL;
    if(size)        //��ȡ������
    {
        //���Ȳ鿴URC�б�
        char *cmdPayload = (char*)malloc(size);
        const char* cmdHead = (const char*)cfg->cReceiveBuff;
        const char* nextHead = 0;
        int payloadLen = 0;
        cfg->cReceiveBuff[size] = 0;
        //
				if(cmdPayload == NULL)
				{
						if(cfg->iLogEnable)
							SEGGER_RTT_printf2("Payload buffer error!\r\n");
						return eATerror;
				}
				if(cfg->iLogEnable)//��־���ض�
						SEGGER_RTT_printf2("<%s=====>\r\n",cfg->cReceiveBuff);
				while((payloadLen = at_get_line(cfg,cmdHead,size,cmdPayload,&nextHead))>0)		
				{
								//printf("Line Head:%s\r\n",cmdPayload);
							if(AT_ERR_NO_URC == at_urc_handle(cfg,cmdPayload,payloadLen))
							{
										if(strstr(cfg->cReceiveBuff,"ERROR"))	//ָ�����
										{
												if(handle)
														handle(arg,(const char*)cfg->cReceiveBuff,size,AT_ERR_ERROR);	//ִ�лص�����
												eATerror = AT_ERR_ERROR;
												goto safe_free;
										}
										if(will)	//ִ�гɹ�ָ��
										{
												if(strstr(cfg->cReceiveBuff,will))
												{
														if(handle)
																handle(arg,(const char*)cfg->cReceiveBuff,size,AT_ERR_NONE);	//ִ�лص�����
														eATerror = AT_ERR_NONE;
														goto safe_free;
												}
										}
								}
					
								if(nextHead)
										size = size - (nextHead - cmdHead);
								cmdHead = nextHead;
								
						}
						eATerror = AT_ERR_NO_WILL;
		safe_free:
				free(cmdPayload);
					return eATerror;
			}
			else
        return AT_ERR_READ_NONE;	//��ȡ����Ϊ��
}


//��ȡ�ʹ���ָ������
AT_ERROR at_parser_read_handle(ATConfig *cfg,const char* will,pATCmdHandle handle,void* arg)
{
    int size = cfg->atPort->fpUartRead(cfg->cReceiveBuff,sizeof(cfg->cReceiveBuff));
		AT_ERROR	eATerror =AT_ERR_NO_WILL;
    if(size)        //��ȡ������
    {
        //���Ȳ鿴URC�б�
        char *cmdPayload = (char*)malloc(size);
        const char* cmdHead = (const char*)cfg->cReceiveBuff;
        const char* nextHead = 0;
        int payloadLen = 0;
        cfg->cReceiveBuff[size] = 0;
        //
				if(cmdPayload == NULL)
				{
						if(cfg->iLogEnable)
							SEGGER_RTT_printf2("Payload buffer error!\r\n");
						return eATerror;
				}
        if(cfg->iLogEnable)//��־���ض�
            SEGGER_RTT_printf2("<%s=====>\r\n",cfg->cReceiveBuff);
        while((payloadLen = at_get_line(cfg,cmdHead,size,cmdPayload,&nextHead))>0)		
        {
            //printf("Line Head:%s\r\n",cmdPayload);
						if(AT_ERR_NO_URC == at_urc_handle(cfg,cmdPayload,payloadLen))
            {
                if(strstr(cfg->cReceiveBuff,"ERROR"))	//ָ�����
                {
                    if(handle)
                        handle(arg,(const char*)cfg->cReceiveBuff,size,AT_ERR_ERROR);	//ִ�лص�����
										eATerror = AT_ERR_ERROR;
										goto safe_free;
                }
                if(will)	//ִ�гɹ�ָ��
                {
                    if(strstr(cfg->cReceiveBuff,will))
                    {
                        if(handle)
                            handle(arg,(const char*)cfg->cReceiveBuff,size,AT_ERR_NONE);	//ִ�лص�����
												eATerror = AT_ERR_NONE;
												goto safe_free;
                    }
                }
            }
			
            if(nextHead)
                size = size - (nextHead - cmdHead);
						cmdHead = nextHead;
            
        }
				eATerror = AT_ERR_NO_WILL;
				safe_free:
					free(cmdPayload);
        return eATerror;
    }
    else
        return AT_ERR_READ_NONE;	//��ȡ����Ϊ��
}



//��ȡ�ʹ���ָ������  20220622
AT_ERROR at_parser_read_handle2(ATConfig *cfg,const char* will,const char* error,pATCmdHandle handle,void* arg)
{
    int size = cfg->atPort->fpUartRead(cfg->cReceiveBuff,sizeof(cfg->cReceiveBuff));		//��ȡ���ڻ���������
	AT_ERROR	eATerror =AT_ERR_NO_WILL;
    if(size)        //��ȡ������
    {
        //���Ȳ鿴URC�б�
        char *cmdPayload = (char*)malloc(size);
        const char* cmdHead = (const char*)cfg->cReceiveBuff;
        const char* nextHead = 0;
        int payloadLen = 0;
        cfg->cReceiveBuff[size] = 0;
        //
		if(cmdPayload == NULL)
		{
			if(cfg->iLogEnable)
				SEGGER_RTT_printf2("Payload buffer error!\r\n");
			return eATerror;
		}
        if(cfg->iLogEnable)//��־���ض�
            SEGGER_RTT_printf2("<%s------------\r\n",cfg->cReceiveBuff);
        while((payloadLen = at_get_line(cfg,cmdHead,size,cmdPayload,&nextHead))>0)		
        {
            //printf("Line Head:%s\r\n",cmdPayload);
			if(AT_ERR_NO_URC == at_urc_handle(cfg,cmdPayload,payloadLen))//�ж�URC�����ص�������
            {
                if(error)
				{
					if(strstr(cfg->cReceiveBuff,error))	//ָ�����
					{
						if(handle)
							handle(arg,(const char*)cfg->cReceiveBuff,size,AT_ERR_ERROR);	//ִ�лص�����
						eATerror = AT_ERR_ERROR;
						goto safe_free;
					}
				}
				else
				{
					if(strstr(cfg->cReceiveBuff,"ERROR"))	//ָ�����
					{
						if(handle)
							handle(arg,(const char*)cfg->cReceiveBuff,size,AT_ERR_ERROR);	//ִ�лص�����
						eATerror = AT_ERR_ERROR;
						goto safe_free;
					}
				}
				
                if(will)	//ִ�гɹ�ָ��
                {
                    if(strstr(cfg->cReceiveBuff,will))
                    {
                        if(handle)
                            handle(arg,(const char*)cfg->cReceiveBuff,size,AT_ERR_NONE);	//ִ�лص�����
						eATerror = AT_ERR_NONE;
						goto safe_free;
                    }
                }
				else
				{
					if(strstr(cfg->cReceiveBuff,"OK"))
                    {
                        if(handle)
                            handle(arg,(const char*)cfg->cReceiveBuff,size,AT_ERR_NONE);	//ִ�лص�����
						eATerror = AT_ERR_NONE;
						goto safe_free;
                    }
				}
            }
			
            if(nextHead)
                size = size - (nextHead - cmdHead);
            cmdHead = nextHead;
            
        }
		eATerror = AT_ERR_NO_WILL;
safe_free:
		free(cmdPayload);
        return eATerror;
    }
    else
        return AT_ERR_READ_NONE;	//��ȡ����Ϊ��
}



//ѭ��
void at_parser_loop(ATConfig *cfg)
{
    AT_ERROR err;
    if(at_config_vaild(cfg) != AT_ERR_NONE)
        return;
    switch(cfg->eATCmdStatus)
    {
        case AT_CMD_IDLE:
            if(!queue_empty(&cfg->qATQueue))
            {
                ATCmdConfig* cmdCfg = (ATCmdConfig*)queue_top(&cfg->qATQueue);
                if(cmdCfg)
                {
                    cfg->atPort->fpUartSend(cmdCfg->at_cmd,cmdCfg->at_len);
                    cfg->eATCmdStatus = AT_CMD_WAIT;
                    cfg->ulCurrentTimeout = cfg->fpSystick();
                }
            }
            else
                at_parser_read_handle(cfg,0,0,0);
            break;
        case AT_CMD_WAIT:
            if(!queue_empty(&cfg->qATQueue))
            {
                ATCmdConfig* cmdCfg = (ATCmdConfig*)queue_top(&cfg->qATQueue);
                if(cfg->fpSystick() - cfg->ulCurrentTimeout < cmdCfg->timeout)
                {
                    err = at_parser_read_handle(cfg,cmdCfg->at_will,cmdCfg->handler,cmdCfg->arg);
                    if(err == AT_ERR_NONE || err == AT_ERR_ERROR)
                    {
                        free(cmdCfg);
                        queue_pop(&cfg->qATQueue);
                        cfg->eATCmdStatus = AT_CMD_IDLE;
                    }
                }
                else
                {
                    if(cmdCfg->resendCnt)
                        cmdCfg->resendCnt--;
                    if(cmdCfg->resendCnt)
                    {
                        cfg->atPort->fpUartSend(cmdCfg->at_cmd,cmdCfg->at_len);
                        cmdCfg->timeout = cfg->fpSystick() + cmdCfg->timeout;
                    }
                    else
                    {
                        free(cmdCfg);
                        queue_pop(&cfg->qATQueue);
                        cfg->eATCmdStatus = AT_CMD_IDLE;
                    }
                }
            }
            else
            {
                cfg->eATCmdStatus = AT_CMD_IDLE;
            }
            break;
        default:break;
    }
}


//����������AT
AT_ERROR at_parser_send_no_wait(ATConfig *cfg,const char* cmd,int atLen,const char* will,pATCmdHandle handler,void* arg,uint32_t timeout,uint8_t resendCnt)
{
    ATCmdConfig *cmdCfg = (ATCmdConfig*)malloc(sizeof(ATCmdConfig));
    if(cmdCfg)
    {
        cmdCfg->at_cmd = cmd;
        cmdCfg->at_len = atLen;
        cmdCfg->at_will = will;
        cmdCfg->handler = handler;
        cmdCfg->arg = arg;
        cmdCfg->timeout = timeout;
        cmdCfg->resendCnt = resendCnt;
        if(queue_push(&cfg->qATQueue,cmdCfg))
            return AT_ERR_NONE;
        else
        {
            free(cmdCfg);
            return AT_ERR_FULL;
        }
    }
    else
        return AT_ERR_FULL;
}

//����������ATָ��
AT_ERROR  at_parser_send_no_wait2(ATConfig *cfg,const char* cmd,int atLen)
{
	
    AT_ERROR err = AT_ERR_NONE;
    
	unsigned long ulTick = 0;
    
    err = at_config_vaild(cfg);				//�жϿͻ����Ƿ�Ϸ�
    
	if(err != AT_ERR_NONE)
        return err;
    if(cfg->eATCmdStatus == AT_CMD_WAIT)	//�ж�ATָ��״̬
        return AT_ERR_DEALING;
		if(cfg->iLogEnable)
		{
			SEGGER_RTT_printf2("at_parser_send_no_wait2:cmd>==%s\r\n",cmd);
		}

    cfg->atPort->fpUartSend(cmd,atLen);		//����

    return AT_ERR_TIMEOUT;
}

//��������ATָ��
AT_ERROR  at_parser_send_wait_file(ATConfig *cfg,const char* cmd,int atLen,const char* will,const char** recBuff,uint32_t timeout,uint8_t resendCnt)
{
	
    AT_ERROR err = AT_ERR_NONE;
    
	unsigned long ulTick = 0;
    
    err = at_config_vaild(cfg);				//�жϿͻ����Ƿ�Ϸ�
    
	if(err != AT_ERR_NONE)
        return err;
    if(cfg->eATCmdStatus == AT_CMD_WAIT)	//�ж�ATָ��״̬
        return AT_ERR_DEALING;
		if(cfg->iLogEnable)
		{
			SEGGER_RTT_printf2("at_parser_send_wait_file:cmd>==%s",cmd);
		}
    do
    {
        cfg->atPort->fpUartSend(cmd,atLen);		//����

        ulTick = cfg->fpSystick();				//������ʱ��
//				SEGGER_RTT_printf2("at_parser_read_handle ulTick:%d\r\n",ulTick);
        while(cfg->fpSystick() - ulTick < timeout)
        {
            err = at_parser_read_handle(cfg,will,0,0);		//����
					//SEGGER_RTT_printf2("err:%d,cfg->fpSystick():%d,ulTick:%d,timeout:%d\r\n",err,cfg->fpSystick(),ulTick,timeout);
            if(recBuff)
                *recBuff = cfg->cReceiveBuff;
            if(err == AT_ERR_NONE || err == AT_ERR_ERROR)
                return err;			//�յ����ݲ�����
        }
				
        if(resendCnt)
            resendCnt--;
    }while(resendCnt);
    return AT_ERR_TIMEOUT;
}

//��������ATָ��
AT_ERROR  at_parser_send_wait(ATConfig *cfg,const char* cmd,int atLen,const char* will,const char** recBuff,uint32_t timeout,uint8_t resendCnt)
{
	
    AT_ERROR err = AT_ERR_NONE;
    
	unsigned long ulTick = 0;
    
    err = at_config_vaild(cfg);				//�жϿͻ����Ƿ�Ϸ�
    
	if(err != AT_ERR_NONE)
        return err;
    if(cfg->eATCmdStatus == AT_CMD_WAIT)	//�ж�ATָ��״̬
        return AT_ERR_DEALING;
		if(cfg->iLogEnable)
		{
				SEGGER_RTT_printf2(">==%s",cmd);
		}
    do
    {
        cfg->atPort->fpUartSend(cmd,atLen);		//����
				SEGGER_RTT_printf2("\r\n-----fpUartSend--------\r\n");
        ulTick = cfg->fpSystick();				//������ʱ��
//				SEGGER_RTT_printf2("at_parser_read_handle ulTick:%d\r\n",ulTick);
        while(cfg->fpSystick() - ulTick < timeout)
        {
            err = at_parser_read_handle(cfg,will,0,0);		//����
					//SEGGER_RTT_printf2("err:%d,cfg->fpSystick():%d,ulTick:%d,timeout:%d\r\n",err,cfg->fpSystick(),ulTick,timeout);
            if(recBuff)
                *recBuff = cfg->cReceiveBuff;
            if(err == AT_ERR_NONE || err == AT_ERR_ERROR)
                return err;			//�յ����ݲ�����
        }
				
        if(resendCnt)
            resendCnt--;
    }while(resendCnt);
    return AT_ERR_TIMEOUT;
}


/*!
\brief      ��������ATָ��
    \param[in]  *cfg		�ͻ���ʵ��
				*cmd		Ҫ���͵�ָ��		Ĭ��ֵ0(������ָ�ֻ��ȡ����)
				cmdLen		ָ��ȣ�			Ĭ��ֵ0�������Զ������ַ���ָ��ȣ�
				*will		ָ��ɹ�����		Ĭ��ֵ0��"OK"��
				error		ָ�������		Ĭ��ֵ0��"ERROR"��
				recBuff		�������ݻ�����	Ĭ��ֵ0������ȡ��
				resend		����ʧ�����Դ���
				timeout		������ʱʱ��
    \param[out] none
    \retval     AT_ERROR
*/
//��������ATָ��
AT_ERROR  at_parser_send_wait2(ATConfig *cfg,const char* cmd,int cmdLen,const char* will,const char* error,const char** recBuff, uint8_t resendCnt, uint32_t timeout)
{
    AT_ERROR err = AT_ERR_NONE;
    unsigned long ulTick = 0;
    
    err = at_config_vaild(cfg);				//�жϿͻ����Ƿ�Ϸ�
    if(err != AT_ERR_NONE)
        return err;
    if(cfg->eATCmdStatus == AT_CMD_WAIT)	//�ж�ATָ��״̬
        return AT_ERR_DEALING;
    do
    {
        if(cmd != 0)
				{
					if(cmdLen == 0)
						cmdLen = strlen(cmd);
					cfg->atPort->fpUartSend(cmd,cmdLen);		//����
					if(cfg->iLogEnable)
						SEGGER_RTT_printf2(">==%s",cmd);
				}

        ulTick = cfg->fpSystick();					//������ʱ��
        while(cfg->fpSystick() - ulTick < timeout)
        {
            err = at_parser_read_handle2(cfg,will,error,0,0);		//����
            if(recBuff)
                *recBuff = cfg->cReceiveBuff;
            if(err == AT_ERR_NONE || err == AT_ERR_ERROR)
                return err;			//�յ����ݲ�����
        }
        if(resendCnt)
            resendCnt--;
    }while(resendCnt);
    return AT_ERR_TIMEOUT;
}

/*!
\brief      ��������ATָ��
    \param[in]  *cfg		�ͻ���ʵ��
				*cmd		Ҫ���͵�ָ��		Ĭ��ֵ0(������ָ�ֻ��ȡ����)
				cmdLen		ָ��ȣ�			Ĭ��ֵ0�������Զ������ַ���ָ��ȣ�
				*will		ָ��ɹ�����		Ĭ��ֵ0��"OK"��
				error		ָ�������		Ĭ��ֵ0��"ERROR"��
				recBuff		�������ݻ�����	Ĭ��ֵ0������ȡ��
				resend		����ʧ�����Դ���
				timeout		������ʱʱ��
    \param[out] none
    \retval     AT_ERROR
*/
//��������ATָ��
AT_ERROR  at_parser_send_wait3(ATConfig *cfg,const char* cmd,int cmdLen,const char* will,const char* error,char* recBuff, uint8_t resendCnt, uint32_t timeout)
{
    AT_ERROR err = AT_ERR_NONE;
    unsigned long ulTick = 0;
    
    err = at_config_vaild(cfg);				//�жϿͻ����Ƿ�Ϸ�
    if(err != AT_ERR_NONE)
        return err;
    if(cfg->eATCmdStatus == AT_CMD_WAIT)	//�ж�ATָ��״̬
        return AT_ERR_DEALING;
		memset(cfg->cReceiveBuff,0,MAX_RECEIVE_BUFF);
    do
    {
        if(cmd != 0)
				{
					if(cmdLen == 0)
						cmdLen = strlen(cmd);
					cfg->atPort->fpUartSend(cmd,cmdLen);		//����
					if(cfg->iLogEnable)
						SEGGER_RTT_printf2(">==%s",cmd);
				}

        ulTick = cfg->fpSystick();					//������ʱ��
        while(cfg->fpSystick() - ulTick < timeout)
        {
            err = at_parser_read_handle2(cfg,will,error,0,0);		//����
            if(recBuff)
						{
//								memcpy(recBuff,cfg->cReceiveBuff,strlen(cfg->cReceiveBuff));
                recBuff = cfg->cReceiveBuff;
						}
            if(err == AT_ERR_NONE || err == AT_ERR_ERROR)
						{
								
                return err;			//�յ����ݲ�����
						}
        }
        if(resendCnt)
            resendCnt--;
    }while(resendCnt);
    return AT_ERR_TIMEOUT;
}

//ֱ�ӷ�������
AT_ERROR at_parser_send(ATConfig *cfg,const char* cmd,int atLen)
{
	AT_ERROR err = AT_ERR_NONE;
    
    err = at_config_vaild(cfg);
    if(err != AT_ERR_NONE)
        return err;
    if(cfg->eATCmdStatus == AT_CMD_WAIT)
        return AT_ERR_DEALING;

	cfg->atPort->fpUartSend(cmd,atLen);
	if(cfg->iLogEnable)
		SEGGER_RTT_printf2("%s",cmd);
	return AT_ERR_NONE;
}

//��������
AT_ERROR at_parser_read(ATConfig *cfg,unsigned char* buff,int *len,int maxLen,int timeout)
{
	AT_ERROR err = AT_ERR_NONE;
    unsigned long ulTick = 0;
    
    err = at_config_vaild(cfg);
    if(err != AT_ERR_NONE)
        return err;
    if(cfg->eATCmdStatus == AT_CMD_WAIT)
        return AT_ERR_DEALING;

    ulTick = cfg->fpSystick();
	while(cfg->fpSystick() - ulTick < timeout)
	{
		int readLen = cfg->atPort->fpUartRead((char*)buff,maxLen);
		if(readLen)
		{
			if(cfg->iLogEnable)
				SEGGER_RTT_printf2("%s",(char*)buff);
			*len = readLen;
			buff[readLen] = 0;
			return err;
		}
	}
	*len = 0;
    return err;
}


//������־
void at_parser_log(ATConfig *cfg,int enable)
{
    cfg->iLogEnable = enable;
}

