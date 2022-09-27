#include "header.h"


typedef struct
{
    uint16_t    usReceiveLen;           //接收到的数据长度
    uint8_t     *pReceiveBuff;          //接受数据缓冲区
    uint16_t    usMaxReceiveLen;        //缓冲区最大长度
    pUartReceiveCallback    pRecCallback;   //接收回调函数
    uint32_t    ulByteInterval;         //字节间超时
    uint32_t    ulRecentTimestamp;      //最近一次接收到字节的系统时基
}UART_PARAM;


static UART_PARAM s_UartParam;

//重启模块
void cat1_pwrker(void)
{
	CAT1_PWEKER_L();
	delay_ms(1100);
	CAT1_PWEKER_H();
}


//复位模块
void cat1_reset(void)
{
	CAT1_RESET_L();
	delay_ms(1100);
	CAT1_RESET_H();
	delay_ms(1100);
}

//唤醒模块
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
* @函数名:      SetUartTimeStamp
* @功能描述:    设置1msTICK函数
* @输入参数:    pUartGetTimeStamp
* @输出参数:    无
* @返回值:      无
*/
void SetUartTimeStamp(pUartGetTimeStamp fun)
{
    g_pfGetTimeStamp = fun;
}


/**
* @函数名:      UartInit
* @功能描述:    初始化串口
* @输入参数:    uart:串口结构体
* @输出参数:    无
* @返回值:      无
*/
void Cat1_UartInit(uint32_t baud, uint8_t* receiveBuff, uint16_t maxLen,uint32_t interval, pUartReceiveCallback recFunc, pUartGetTimeStamp timeFunc)
{
    memset(&s_UartParam,0,sizeof(s_UartParam));

    s_UartParam.pReceiveBuff = receiveBuff;
    s_UartParam.usReceiveLen = 0;
    s_UartParam.ulByteInterval = interval;		//接收超时时间（ms）
    s_UartParam.usMaxReceiveLen = maxLen;
    s_UartParam.pRecCallback = recFunc;
	SetUartTimeStamp(timeFunc);
	cat1_uart_init(baud);

}

/**
* @函数名:      UartRead
* @功能描述:    读取串口数据
* @输入参数:    needCopy:是否需要拷贝,buf：拷贝的数据，len最大读取字节数
* @输出参数:    无
* @返回值:      实际读到的字节数
*/
uint16_t Cat1_UartRead(bool needCopy,uint8_t* buff,uint16_t len)
{
    
	if(g_pfGetTimeStamp == 0)
        return 0;
    if(s_UartParam.usReceiveLen && g_pfGetTimeStamp()-s_UartParam.ulRecentTimestamp >= s_UartParam.ulByteInterval)		//用于接收超时判断
    {
        uint16_t retLen = s_UartParam.usReceiveLen;		//接收到的数据长度
				if(retLen >= s_UartParam.usMaxReceiveLen)		//如果接收的大于缓冲区
					retLen = s_UartParam.usMaxReceiveLen - 1;		//长度只能有效到最大缓冲区
		
				s_UartParam.pReceiveBuff[retLen] = 0;//用于断开后面的复制
				if(len < retLen)						//如果缓存字节大于最大读取字节，就一最大读取字节为准
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
* @函数名:      UartSend
* @功能描述:    写串口数据
* @输入参数:    buf：写入的数据，len写入的字节数
* @输出参数:    无
* @返回值:      是否成功发送
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
#if SYSTEM_SUPPORT_UCOS  //使用UCOS操作系统
	OSIntEnter();    
#endif
    //发生接收中断	
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
	OSIntExit();    	//退出中断
#endif
    
}




