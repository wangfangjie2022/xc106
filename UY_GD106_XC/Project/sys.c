#include "header.h"

/*functional module*/
#include "flash_e2prom.h"

uint8_t ChipID[12];	//chipID buffer.
char ChipIDstring[25];


states modulesta;
///*
//	Modluerecall[0] 是否需要回调
//	Modluerecall[1] 回调地址	
//	Modluerecall[2] 回调是否执行
//	Modluerecall[3] 上一次L1缓存的设备地址
//*/
//uint8_t Modluerecall[4];


//@System restart
void sys_reboot(void)
{
	//下线
		
	//复位
	__disable_fault_irq(); 
	NVIC_SystemReset();
}


//@Get chip id save.
void HexToAscii(unsigned char *pHex, unsigned char *pAscii, int nLen)
{
    unsigned char Nibble[2];
    unsigned int i,j;
    for (i = 0; i < nLen; i++)
    {
        Nibble[0] = (pHex[i] & 0xF0) >> 4;
        Nibble[1] = pHex[i] & 0x0F;
        for (j = 0; j < 2; j++)
        {
            if (Nibble[j] < 10)
            {            
                Nibble[j] += 0x30;
            }
            else
            {
                if (Nibble[j] < 16)
                    Nibble[j] = Nibble[j] - 10 + 'A';
            }
            *pAscii++ = Nibble[j];
        }               // for (int j = ...)
    }           // for (int i = ...)
}

void GetUniqueID(unsigned char *p)
{
     unsigned char i;
     unsigned char *pIDStart=(unsigned char *)(ID_BaseAddress);   
     for(i=0;i!=12;i++)
     {*p++=*pIDStart++;}
}


//@Get ID number to the ASCII number.
void GetUniqueID_String(char *string_id)
{
		unsigned char buffer_id[12];
		unsigned char var;
		char pstring[24];
		
		GetUniqueID(buffer_id);
		//memset(protocol_devic_sn,0,50);
		for(var=0;var<12;var++)
		{
			AsciiHexNumber(buffer_id[var],pstring+var*2);
		}
		memcpy(string_id,pstring,24*sizeof(char));
		memcpy(string_id+24,"",sizeof(char));
		#ifdef DEBUG_PRINT
		SEGGER_RTT_printf2("protocol_devic_sn=%s",string_id);
	#endif
}




void	nvic_config(void)
{
	nvic_irq_enable(USART0_IRQn, 1, 1);
	nvic_irq_enable(TIMER1_IRQn, 1, 2);
	nvic_irq_enable(SDIO_IRQn, 1, 1);
}
void system_init(void)
{
	
	//@Set the NVIC 
	nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
	
	//@Set the systick clock.  120M/8=15MHz
	//systick_clksource_set(SYSTICK_CLKSOURCE_HCLK_DIV8);		
	
	//@Setup systick timer for 1000Hz interrupts
	//SysTick_Config(SystemCoreClock / 15000U);
//	systick_config();
	delay_init(200);
	//@get chip id 
	GetUniqueID(ChipID);
	GetUniqueID_String(ChipIDstring);
	//@set the interrupt priority
	nvic_config();
}





//@system recover. 
void sys_recover()
{
//擦除FLASH DATA重新分配
//	lv_erasure_e2prom();


}	

//@system OTA. 
void system_OTA()
{	
	//OTA升级操作



}

#if add
//@system state machine. 
uint8_t SystemstateSwitch(sysstate syswork) 
{
	switch(syswork.moduleWork)
	{
/*Embedded the wrok modules you have been used.*/

//@The systime modbule.
		case sys_restart:
			sys_reboot();		
			return 0;
		break;

		case sys_OTA:
			
			return 0;
		break;

		case sys_recovered:
			sys_recover();
			return 0;
		break;
		
//@The network module.
		case sys_network:
				if(syswork.wrokstate==sta_Done)
				return 0;
		break;

//@The modbus module.
		case sys_modbus:
				if(syswork.wrokstate==sta_Done)
				return 0;
		break;
		
		default:
			break;
	}	
}

uint8_t  SystemstateSwitch(sysstate syswork)
{

}


#endif



//@systimer IRQ Handle.
void systimetick_Handle(void)
{
	
	
	
	
	
	
	
}


