#include "header.h"


typedef struct
{
    uint16_t    usReceiveLen;           //���յ������ݳ���
    uint8_t     *pReceiveBuff;          //�������ݻ�����
    uint16_t    usMaxReceiveLen;        //��������󳤶�
    pUartReceiveCallback    pRecCallback;   //���ջص�����
    uint32_t    ulByteInterval;         //�ֽڼ䳬ʱ
    uint32_t    ulRecentTimestamp;      //���һ�ν��յ��ֽڵ�ϵͳʱ��
}UART_PARAM;


static UART_PARAM s_UartParam;

//����ģ��
void cat1_pwrker(void)
{
	CAT1_PWEKER_L();
	delay_ms(1100);
	CAT1_PWEKER_H();
}


//��λģ��
void cat1_reset(void)
{
	CAT1_RESET_L();
	delay_ms(1100);
	CAT1_RESET_H();
	delay_ms(1100);
}

//����ģ��
void cat1_wakeup(void)
{
	CAT1_WAKEUP_L();
	delay_ms(1100);
	CAT1_WAKEUP_H();
}

void cat1_io_init(void)
{

	/* enable the GPIOA&B clock */
    rcu_periph_clock_enable(RCU_GPIOA);
		rcu_periph_clock_enable(RCU_GPIOB);
		rcu_periph_clock_enable(RCU_GPIOC);
	
    /* configure LED GPIO port */ 	
	
		gpio_mode_set(CAT1_RESET_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,CAT1_RESET_PIN );
		gpio_output_options_set(CAT1_RESET_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,CAT1_RESET_PIN );
	
		gpio_mode_set(CAT1_PWRKEY_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,CAT1_PWRKEY_PIN);
		gpio_output_options_set(CAT1_PWRKEY_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,CAT1_PWRKEY_PIN);
	
		gpio_mode_set(CAT1_WAKEUP_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, CAT1_WAKEUP_PIN);
		gpio_output_options_set(CAT1_WAKEUP_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,CAT1_WAKEUP_PIN);
	
    /* reset LED GPIO pin */
	CAT1_PWEKER_H();
	CAT1_RESET_H();
	CAT1_WAKEUP_H();
	
}

void cat1_uart_init(uint32_t BAUD)
{
		
		/* USART interrupt configuration */
		nvic_irq_enable(CAT1_UART_IRQ, 0, 0);
		
		rcu_periph_clock_enable(CAT1_UART_GPIO_CLK);

    /* enable USART clock */
    rcu_periph_clock_enable(CAT1_UART_CLK);

    /* connect port to USARTx_Tx */
    gpio_af_set(CAT1_UART_GPIO_PORT, CAT1_UART_AF, CAT1_UART_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_af_set(CAT1_UART_GPIO_PORT, CAT1_UART_AF, CAT1_UART_RX_PIN);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(CAT1_UART_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,CAT1_UART_TX_PIN);
    gpio_output_options_set(CAT1_UART_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,CAT1_UART_TX_PIN);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(CAT1_UART_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP,CAT1_UART_RX_PIN);
    gpio_output_options_set(CAT1_UART_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,CAT1_UART_RX_PIN);

    /* USART configure */
    usart_deinit(CAT1_UART);
    usart_baudrate_set(CAT1_UART,BAUD);
    usart_receive_config(CAT1_UART, USART_RECEIVE_ENABLE);
    usart_transmit_config(CAT1_UART, USART_TRANSMIT_ENABLE);
    usart_enable(CAT1_UART);
		
		/* enable USART0 receive interrupt */
    usart_interrupt_enable(CAT1_UART, USART_INT_RBNE);
		
}

/**
* @������:      SetUartTimeStamp
* @��������:    ����1msTICK����
* @�������:    pUartGetTimeStamp
* @�������:    ��
* @����ֵ:      ��
*/
void SetUartTimeStamp(pUartGetTimeStamp fun)
{
    g_pfGetTimeStamp = fun;
}


/**
* @������:      UartInit
* @��������:    ��ʼ������
* @�������:    uart:���ڽṹ��
* @�������:    ��
* @����ֵ:      ��
*/
void Cat1_UartInit(uint32_t baud, uint8_t* receiveBuff, uint16_t maxLen,uint32_t interval, pUartReceiveCallback recFunc, pUartGetTimeStamp timeFunc)
{
    memset(&s_UartParam,0,sizeof(s_UartParam));

    s_UartParam.pReceiveBuff = receiveBuff;
    s_UartParam.usReceiveLen = 0;
    s_UartParam.ulByteInterval = interval;		//���ճ�ʱʱ�䣨ms��
    s_UartParam.usMaxReceiveLen = maxLen;
    s_UartParam.pRecCallback = recFunc;
	SetUartTimeStamp(timeFunc);
	cat1_uart_init(baud);

}

/**
* @������:      UartRead
* @��������:    ��ȡ��������
* @�������:    needCopy:�Ƿ���Ҫ����,buf�����������ݣ�len����ȡ�ֽ���
* @�������:    ��
* @����ֵ:      ʵ�ʶ������ֽ���
*/
uint16_t Cat1_UartRead(bool needCopy,uint8_t* buff,uint16_t len)
{
    
	if(g_pfGetTimeStamp == 0)
        return 0;
    if(s_UartParam.usReceiveLen && g_pfGetTimeStamp()-s_UartParam.ulRecentTimestamp >= s_UartParam.ulByteInterval)		//���ڽ��ճ�ʱ�ж�
    {
        uint16_t retLen = s_UartParam.usReceiveLen;		//���յ������ݳ���
				if(retLen >= s_UartParam.usMaxReceiveLen)		//������յĴ��ڻ�����
					retLen = s_UartParam.usMaxReceiveLen - 1;		//����ֻ����Ч����󻺳���
		
				s_UartParam.pReceiveBuff[retLen] = 0;//���ڶϿ�����ĸ���
				if(len < retLen)						//��������ֽڴ�������ȡ�ֽڣ���һ����ȡ�ֽ�Ϊ׼
					retLen = len;
		
        if(needCopy)
        {
            memcpy(buff,&s_UartParam.pReceiveBuff[0],retLen);	
				}
        s_UartParam.usReceiveLen = 0;
        return retLen;
    }
    return 0;
} 

/**
* @������:      UartSend
* @��������:    д��������
* @�������:    buf��д������ݣ�lenд����ֽ���
* @�������:    ��
* @����ֵ:      �Ƿ�ɹ�����
*/
bool Cat1_UartSend(const char* buff,int len)
{
	uint32_t i = 0;
	
	if(len)
	{
		while(1)
		{
			usart_data_transmit(CAT1_UART, *(buff+i));
			i++;
			if(i >= len)
			{
				return true;
			}
			while(RESET == usart_flag_get(CAT1_UART, USART_FLAG_TBE));
		}
	}
	else
	{
		while(1)
		{	
			usart_data_transmit(CAT1_UART, *(buff+i));
			i++;
			if(*(buff+i) == 0)
			{
				return true;
			}
			while(RESET == usart_flag_get(CAT1_UART, USART_FLAG_TBE));
		}
	}
}

uint8_t tmp08; 
void CAT1_UART_IRQHandler(void)
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif
    //���������ж�	
		 if((RESET != usart_interrupt_flag_get(CAT1_UART, USART_INT_FLAG_RBNE)) &&(RESET != usart_flag_get(CAT1_UART, USART_FLAG_RBNE))) 
		 {
        /* receive data */
        tmp08 = usart_data_receive(CAT1_UART);
         if(s_UartParam.usReceiveLen < s_UartParam.usMaxReceiveLen)
        {
            s_UartParam.pReceiveBuff[s_UartParam.usReceiveLen] = tmp08;					
            s_UartParam.usReceiveLen++;
        }
        s_UartParam.ulRecentTimestamp = g_pfGetTimeStamp();
    }
 #if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
#endif
    
}




