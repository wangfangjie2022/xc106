#include "header.h"

	uint8_t ModbusTxcount=0; 
	uint8_t ModbusRxcount=0; 

	uint8_t ModbusRxbuffer[ModbusRx_size];
	uint8_t ModbusTxbuffer[ModbusTx_size];

uint8_t Modbussendflag=MODBUS_HAVE_DATA_SEND_NO;
	
	
//extern uint8_t Modluerecall[4];
extern ModbusPara  ModbusParanormal;       //@get one normally object.
extern ModuleIRQstate IRQstate;
//@modbus state machine judge condition.




//@contral the chip pins to have different functions.
void modbus_readCfg(void)
{
    DE_L();
    RE_L();
}

void modbus_sendCfg(void)
{
    RE_H();
    DE_H();
}

void modbus_sleep(void)
{
	//@Please read the IC datasheet and write this funciton.
	
	//@Accroding the IC whether have the sleep mode or not.

	
}

//@modbus UASRT initializes setting.
void modbusUART_init(uint32_t baud_rate)
{

//	  rcu_periph_clock_enable(RCU_GPIOA);
//		rcu_periph_clock_enable(RCU_USART1);
//	
//    /* connect port to USARTx_Tx */
//    gpio_init(Modbus_ctrl_Port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ,RS485D );

//    /* connect port to USARTx_Rx */
//    gpio_init(Modbus_ctrl_Port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, RS485R);
//	

//		usart_deinit(Modbus_UART_Port); 						
//	//@set the USART baudrate.
//		usart_baudrate_set(Modbus_UART_Port, baud_rate);									

//	//@enable the receiver time out.	
//	  usart_receiver_timeout_enable(Modbus_UART_Port);
//    usart_receiver_timeout_threshold_config(Modbus_UART_Port, baud_rate*1*8); //enable the timeout.timeout time is 4bytes for the baund.
//	
//	//@Enable the receiver buffer on the  chip.	
//	  usart_receive_config(Modbus_UART_Port, USART_RECEIVE_ENABLE);   
//  //@Enable the transmit buffer on the chip.
//		usart_transmit_config(Modbus_UART_Port, USART_TRANSMIT_ENABLE); 
//	
//		usart_enable(Modbus_UART_Port);																	
//		
//		usart_interrupt_flag_clear(Modbus_UART_Port, USART_INT_FLAG_RBNE);
//		usart_interrupt_flag_clear(Modbus_UART_Port, USART_INT_FLAG_RT);
//	//@enable the interrupt		
//		usart_interrupt_enable(Modbus_UART_Port, USART_INT_RBNE);
//		usart_interrupt_enable(Modbus_UART_Port,USART_INT_RT);

}






//@Switch  the modbus working state.
Modbusworkstate Switch_Modbus_state(Modbusworkstate Flag)
{
    if(Flag == Modbus_Slave)
    return Modbus_Master;
    else
    return Modbus_Slave;
}


//@Modbus write data to the bus for the other online devices.
bool  modbus_write(ModbusPara *cfg)
{
//	SEGGER_RTT_printf2("modbus_write\r\n"); 
  //@switch to the send mode.
	modbus_sendCfg();//发送模式
	
  delay_ms(10);
	
	uint16_t crc_buf = 0;
	
  ModbusTxbuffer[0]= (*cfg).Address;
  ModbusTxbuffer[1] = 0x03;


  ModbusTxbuffer[2] =(*cfg).staraddH;
  ModbusTxbuffer[3] = (*cfg).staraddL;
  if((*cfg).readnumL== DATANUM -(*cfg).staraddL)
  {
		ModbusTxbuffer[4] = (*cfg).readnumH;
		ModbusTxbuffer[5] = (*cfg).readnumL;
  }
  else
  {
			//@function error ,pause this time, command error.
//		while(1)
//		{      
		SEGGER_RTT_printf2(">ID:%d Modbus send data error!\r\n",(*cfg).Address); 
//		}
  }
	ModbusTxbuffer[6]=0;
	ModbusTxbuffer[7]=0;
  crc_buf= GetCRC16(ModbusTxbuffer, (*cfg).sendlen);

  ModbusTxbuffer[7] |= crc_buf;           //CRC Low
  ModbusTxbuffer[6] |= crc_buf>>8;               //CRC High
	
	ModbusTxcount=0;
while(1)
	{
	//@send the data
		if(ModbusTxcount<=ModbusTx_size)
		{
			usart_data_transmit(Modbus_UART_Port, ModbusTxbuffer[ModbusTxcount]);
//			SEGGER_RTT_printf2("ModbusTxbuffer[%d]:%x ",ModbusTxcount,ModbusTxbuffer[ModbusTxcount]);	
			while(RESET == usart_flag_get(Modbus_UART_Port, USART_FLAG_TBE));
			ModbusTxcount++;	
//			SEGGER_RTT_printf2("modbus_write2"); 
		}else
		{		  
			modbus_readCfg();
			delay_ms(500);
//			SEGGER_RTT_printf2(">Modbus TX Done!\r\n");	
			return 0;
		}
 
	}

}

//@save the data to the cache.


uint8_t Data_cache[MaxDevnum][DATANUM*2]={0};
uint8_t Data_cache_lastdata[MaxDevnum][DATANUM*2]={0};
void Packdata_save(uint8_t *indatabuff)//按照地址存储反馈的数据
{
  //Address  Function  Data.
		
   uint8_t j=0;
//	 uint8_t singlebyte=0;
//	 uint8_t Decbyte=0;
	/*  单个最小接收时间
    *	 @(240bit+15*30bit)/9600(bit/s)+30*(10ms~50ms)( in Doc )
    *  @= 380ms~1.6s.	*/	
		
   for(j=0;j<(DATANUM*2);j++)
   {
       Data_cache[(*indatabuff)][j]=*(indatabuff+3+j);
   }
			Data_cache[(*indatabuff)][TEMPA_ADDR]-=0x23;
		 	Data_cache[(*indatabuff)][TEMPB_ADDR]-=0x23;	
	 		Data_cache[(*indatabuff)][TEMPC_ADDR]-=0x23;	
			SEGGER_RTT_printf2(">Save ID:%d \r\n",*(indatabuff));
}


//@Modbus read data from the cache.从串口缓存读数据，当数据不是0XFF的时候表示设备存在，否则表示设备部存在
bool modbus_readFromCache(uint8_t *towhere,uint8_t address)
{
		//@check the device address.
		if(Data_cache[address][0]!=0xFF)
		{
			if(towhere!=NULL)
			{			
				*towhere=Data_cache[address][0];
				uint8_t var=0;
				for (var=0; var<2*DATANUM;var++)
				{
				*(towhere+var)=Data_cache[address][var];
				}
			}
				return 1;
		}
		else
		{
//			SEGGER_RTT_printf2("ID:%d NO ONLINE,",address);
			return 0;
		}
}

extern uint8_t ChipID[12];	//chipID buffer.


//@The number of the be conveted cJSON Object.
//void BuilecJSONnum(uint8_t starAddress,uint8_t builebun)
//{
//	char *cJSONout;
//	if(starAddress<MaxDevnum)
//	{
//			uint8_t var=0;
//			for(var=0;var<builebun;var++)
//			{
//				PackData_Parsing(onlineAddress,starAddress,cJSONout+(var*32));
//			}
//	}
//}

///*!
//    \brief      this function handles USART1 exception
//    \param[in]  none
//    \param[out] none
//    \retval     none
//*/
//void  USART1_IRQHandler(void)
//{
//	  uint16_t crc_buf = 0;
//		uint8_t  crc_LB=0x00,crc_HB=0x00;
//		
//		if(RESET != usart_interrupt_flag_get(Modbus_UART_Port, USART_INT_FLAG_RBNE))
//		{
//        /* receive data */
//						
//        ModbusRxbuffer[ModbusRxcount] = usart_data_receive(Modbus_UART_Port);
//				SEGGER_RTT_printf2(">ModbusRxbuffer[%d]:%x\r\n",ModbusRxcount,ModbusRxbuffer[ModbusRxcount]);
//				ModbusRxcount++;
//				usart_interrupt_flag_clear(Modbus_UART_Port, USART_INT_FLAG_RBNE);
//		
//				if(ModbusRx_size-1 <= ModbusRxcount)
//				{
//					
//					crc_buf= GetCRC16(ModbusRxbuffer,13);
//					crc_HB|=crc_buf;
//					crc_LB|=crc_buf>>8;
//					if(crc_LB==ModbusRxbuffer[13]&&crc_HB==ModbusRxbuffer[14])
//					{
//						Packdata_save(ModbusRxbuffer);
//						memset(ModbusRxbuffer, 0, 16*sizeof(uint8_t));
//					}
//					else 
//					{
//						memset(ModbusRxbuffer, 0, 16*sizeof(uint8_t));        
//								
//					}
//					ModbusRxcount=0;
//				}
//		}
//		
//		
//		if(RESET != usart_interrupt_flag_get(Modbus_UART_Port, USART_INT_FLAG_RT))
//		{
//        /* receive timeout(overtime) */
//			
//										 crc_buf= GetCRC16(ModbusRxbuffer, 3);
//                     crc_HB=crc_buf;
//                     crc_LB=crc_buf>>8;
//			
//								if(ModbusRx_size-1 <= ModbusRxcount)
//								{
//									
//									crc_buf= GetCRC16(ModbusRxbuffer,13);
//									crc_HB|=crc_buf;
//									crc_LB|=crc_buf>>8;
//									//@It will clear the buffer, if the crc is not right.
//									if(crc_LB==ModbusRxbuffer[13]&&crc_HB==ModbusRxbuffer[14])
//									{
//										Packdata_save(ModbusRxbuffer);
//										memset(ModbusRxbuffer, 0, 16*sizeof(uint8_t));
//									}
//									else 
//									{
//										memset(ModbusRxbuffer, 0, 16*sizeof(uint8_t));        
//												
//									}
//								}
//								if(ModbusRxcount== ModbusTx_size)
//                {
//                //@get crc and judge whether save it or not.    
//                    
//                     if(crc_HB==ModbusRxbuffer[7]&&ModbusRxbuffer[6]==crc_LB)
//                     {
//                        memset(ModbusRxbuffer, 0, 16*sizeof(uint8_t));;
//                     }
//                }
//								else
//								{
//									memset(ModbusRxbuffer, 0, 16*sizeof(uint8_t));
//								}
//						 memset(ModbusRxbuffer, 0, 16*sizeof(uint8_t));
//						 ModbusRxcount=0;
//						 usart_interrupt_flag_clear(USART1, USART_INT_FLAG_RT);
//		}
//		
//				ModbusParanormal.Early_packpertick=GetTimeTick();
//}



//@modbus initializes setting.
Modbusstate modbus_init(uint32_t baud_rate)
{
//	uint8_t var=0;
//	memset(Data_cache,0,sizeof(Data_cache));
//	for(var=0;var<MaxDevnum;var++)//20220321
//		Data_cache[var][0]=0xFF;
//	rcu_periph_clock_enable(RCU_GPIOA); 
//	gpio_init(GPIOA, GPIO_MODE_OUT_PP,GPIO_OSPEED_50MHZ,RS485DE|RS485RE );
//	//设置协议中查询起始变量个数。
//	//@this is acrodding to the Poject.
//	ModbusParanormal.staraddL=0x00;	
//	ModbusParanormal.staraddH=0x00;		
//	modbus_readCfg();
//	//@initialize the UART1
//	modbusUART_init(baud_rate); 
	return Modbus_idle;

}


