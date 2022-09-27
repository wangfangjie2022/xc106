#ifndef __MODBUS_BASE_H__
#define __MODBUS_BASE_H__

//@Modbus use UART1


#include "sys.h"
#include "crc.h"
#include "cJSON.h"

/*************************************************************************************
         (API)                  define the GPIO contral the modbus
**************************************************************************************/
//@define the pins the RS485 has used 
#define RS485DE GPIO_Pin_5	//GPIOA
#define RS485RE GPIO_Pin_4
#define RS485R  GPIO_Pin_3
#define RS485D  GPIO_Pin_2

//@define the min time for both data package.
#define ModWait 100         

/*******************************use for the modbus **************************************/
//@define the reciver buffer the number of the members.
	#define ModbusTx_size 8
	#define ModbusRx_size 16
	#define Modbuserror_size 5


//@define some parameters of the modbus Object.
#define MaxDevnum  30       	//define the max number of the online device
#define ReserveDev 2					
#define functioncode 0x03			
#define DATANUM 5   					//use for check how many data bytes

#define Modbus_ctrl_Port  GPIOA			
#define Modbus_UART_Port  USART1		

/**
//@this program use USART1,so AHB1 CLK F = 60MHz   
so the overtime is 4 bytes time at you need baud rate.
\@notice: need to smaller than 16,777,215 

**/
#define Modbus_timeoutByet 4 

#define DE_H()   gpio_bit_set(Modbus_ctrl_Port,RS485DE )
#define RE_H()   gpio_bit_set( Modbus_ctrl_Port,RS485RE )

#define DE_L()   gpio_bit_reset( Modbus_ctrl_Port,RS485DE )
#define RE_L()   gpio_bit_reset( Modbus_ctrl_Port,RS485RE )


extern uint8_t Data_cache[MaxDevnum][DATANUM*2];
extern uint8_t Data_cache_lastdata[MaxDevnum][DATANUM*2];

//@record the online device.
//@record the error device. byte 2 data error. byte 1 command error. 
extern uint8_t errorrecord[MaxDevnum];

extern uint8_t errorbuff[5];
	
volatile extern uint32_t  onlineAddress;

//#define MAX_SEND_MQTT_LENGTH 1024 //MQTT最大发送数据数量？？？
#define MAX_SEND_MQTT_LENGTH 1024 //MQTT最大发送数据数量？？？

#define MAX_BIANYAQI_NUM 32 //变压器最大数量

#define MODBUS_HAVE_DATA_SEND_NO 0
#define MODBUS_HAVE_DATA_SEND_YES 1
extern uint8_t Modbussendflag;

/*************************************************************************************
           (DATA API)        define the parameters of the modbus
    Address     Function    Data        Check
     8bit         8bit      N 8bit      16bit(CRC-16)
**************************************************************************************/
#define AddrLoc         0
#define CMcode          1
#define Datacode        2
#define Funcode         3
#define fixlen          20

#define STATUE_FLAG_ADDR 1
#define TEMPA_ADDR 3
#define TEMPB_ADDR 5
#define TEMPC_ADDR 7
#define TIMER_ADDR 9

//notice:! is the necessary condition
typedef struct 
{

    uint8_t Datacontral;     //@data Contral number

    uint8_t  sendlen;        //@data length

    uint8_t  Address;
//  uint8_t  Function;       //@the function code number, no be use

    uint8_t *Dataadd;        //@the data address  number
    uint8_t  Datalen;        //@the data length


  //@funciton code
    uint8_t  staraddH;       //
    uint8_t  staraddL;       //
    uint8_t  readnumH;       //!must initialize  readnum =functionnum - staraddL
    uint8_t  readnumL;       //

    uint8_t  DEVICE_NUM[MaxDevnum-1];  // Record the online Device number(address)

    uint8_t  Sendbuff[fixlen];

    uint16_t Modbussystick;     //@set the tick to record the tick.
    uint16_t Modbuslastsystick; //@save last time tick.
		
		bool checkFlag;
		uint16_t checktick;
		
		unsigned long Early_packpertick;
		unsigned long lase_packpertick;
			
		uint8_t packintervaltime;
		uint8_t framintervaltime;

}ModbusPara;



typedef enum
{
    Modbus_Slave,   //@as a slaver to monitor which device is online.
    Modbus_Master   //@as a Master to check which device is online.
}Modbusworkstate;



//@get the Modbus state. 
typedef enum
{
    Modbus_idle = 0,    				//@	the RS485 moudle idle.
    Modbus_error,   				//@	the RS485 working state error.
    Modbus_recived,				  //@	the RS485 reciving data.
    Modbus_sendied,					//@	the RS485 sending data.
    Modbus_buzy,						//@	the RS485
    Modbus_SlaveDONE,						//@	the RS485 action done.
		Modbus_MasterDONE,
    Modbus_command_error, 	//@	the RS485 command error code. 
    Modbus_data_error, 			//@	the RS485 data error code.
		Modbus_FindAddErr_recv,	
}Modbusstate;






/*************************************************************************************
                           define the parameters of the modbus
**************************************************************************************/
/**
 * \brief          modbus_init    This function initializes the Modbus context. 
 *					
 *								It must be the first API called before using the contex.							
 */
//ban
void modbusUART_init(uint32_t baud_rate);         //Modbus UART initialization.

Modbusstate modbus_init(uint32_t baud_rate);             //Modbus initialization.





/**
 * \brief         Switch  the modbus working state.
 * \para[in]			Flag     				 Modbus_Master / Modbus_Slave
 * \para[out]			Modbusworkstate	 Modbus_Master / Modbus_Slave	
 */
Modbusworkstate Switch_Modbus_state(Modbusworkstate Flag); //select the working state.





/**
 * \brief          contral the 485 IC.
 * \para					 none								
 */
void modbus_readCfg(void);          //@read configure.
void modbus_sendCfg(void);          //@send configure.





/**
 * \brief          	Modbus save  in  the cache.
 *									16bit_byte1:address 	
 *									16bit_byte2:device
 *									16bit_byte3:data1
 * 									16bit_byte4:data2
 * 									16bit_byte5:data3
 * 									use 8bit to save the data.
 * \para[in]			  databuff   
 */
void Packdata_save(uint8_t *databuff);

/**
 * \brief          Modbus read.
 *					       read from the cache.
 * \para[out]			 towhere   
 * \para[in]			 startaddress
 * \return         return 0 is fail , 1 is successs.
 */
bool modbus_readFromCache(uint8_t *towhere,uint8_t address);//@Modubus read command from the cache.


/**
 * \brief          Modbus write.   
 * 								 write to the bus.  
 * \para[in]			 ModbusPara *cfg
 * \return         return 0 is fail , 1 is successs.
 */

bool modbus_write(ModbusPara *cfg);   									//@Modbus witer command



/*************************************************************************************
                    define the parameters of the modbus Parsing
**************************************************************************************/

extern uint8_t Location;
volatile extern uint32_t  onlineAddress;
/*************************************************************************************
                    define the parameters of the modbus Parsing Function
**************************************************************************************/

/**
* \brief        Modbus data package Parsing.
* \pram[in]			onlineAddressID input the online record byte.
* \pram[in]			input the transformer device ID(Address). 
*	\return 			bool 
*								success \0  
*								fail \1
**/
//bool PackData_Parsing(uint32_t onlineAddressID,uint8_t var,char *cJSONoutbuffer);


///**
//* \brief        Modbus data package Parsing.
//* \pram[in]			starAddress the star address need to transform to cJSON.
//* \pram[in]			input the transformer device ID(Address). 
//*	\return 			none
//**/
//void BuilecJSONnum(uint8_t starAddress,uint8_t builebun);

/*************************************************************************************
                    need to modify
**************************************************************************************/

/**
* \brief         Modbus data package Parsing.
* 
**/
//@ Parsing modbus data.
//void ModbusParsing();		



#endif
