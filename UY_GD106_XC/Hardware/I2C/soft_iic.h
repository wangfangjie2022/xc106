
#ifndef __MYIIC_H

#define __MYIIC_H

#include "sys.h"


#define I2C_PORT GPIOB
#define IIC_SCL_PIN GPIO_PIN_10
#define IIC_SDA_PIN GPIO_PIN_11
#define SCL_PIN_NUM 10
#define SDA_PIN_NUM 11
 
//#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
//#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//#if(SDA_PIN_NUM>7)//WXF ADD 20200710
//	#define SDA_IN()  {I2C_PORT->CRH&=(~((u32)0xf<<((SDA_PIN_NUM-8)*4)));I2C_PORT->CRH|=(u32)8<<((SDA_PIN_NUM-8)*4);}
//	#define SDA_OUT() {I2C_PORT->CRH&=(~((u32)0xf<<((SDA_PIN_NUM-8)*4)));I2C_PORT->CRH|=(u32)3<<((SDA_PIN_NUM-8)*4);}
//#else
//	#define SDA_IN()  {I2C_PORT->CRL&=(~((u32)0xf<<((SDA_PIN_NUM)*4)));I2C_PORT->CRL|=(u32)8<<((SDA_PIN_NUM)*4);}
//	#define SDA_OUT() {I2C_PORT->CRL&=(~((u32)0xf<<((SDA_PIN_NUM)*4)));I2C_PORT->CRL|=(u32)3<<((SDA_PIN_NUM)*4);}
//#endif
	#define SDA_IN()  {gpio_mode_set(I2C_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, IIC_SDA_PIN);}
	#define SDA_OUT() {gpio_mode_set(I2C_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, IIC_SDA_PIN);}

	
//IO²Ù×÷º¯Êý	 
#define IIC_SCL    PBout(SCL_PIN_NUM) //SCL
#define IIC_SDA    PBout(SDA_PIN_NUM) //SDA	 
#define READ_SDA   PBin(SDA_PIN_NUM)  //ÊäÈëSDA 
 

//IIC所有操作函数

void SOFT_IIC_Init(void);                //初始化IIC的IO口

void IIC_Start(void);				//发送IIC开始信号

void IIC_Stop(void);	  			//发送IIC停止信号

void IIC_Send_Byte(u8 txd);			//IIC发送一个字节

u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节

u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号

void IIC_Ack(void);					//IIC发送ACK信号

void IIC_NAck(void);				//IIC不发送ACK信号

#define  I2C_Direction_Transmitter      ((uint8_t)0x00)
#define  I2C_Direction_Receiver         ((uint8_t)0x01)

uint8_t Soft_I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, unsigned char *data_buf);

uint8_t Soft_I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, unsigned char *data_buf);

u8 ov2640_write(uint8_t reg, uint8_t data);
u8 ov2640_read(uint8_t reg); 

#endif
