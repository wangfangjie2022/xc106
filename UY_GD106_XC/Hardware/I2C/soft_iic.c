
#include "soft_iic.h"

#include "delay.h"

#include "sccb.h"

//初始化IIC

#define IIC_DELAY1 8
#define IIC_DELAY2 4

void SOFT_IIC_Init(void)

{
	/* enable GPIOB and I2C clock */
    rcu_periph_clock_enable(DCI_SCCB_SCL_GPIO_CLK);
		rcu_periph_clock_enable(DCI_SCCB_SDA_GPIO_CLK);

		gpio_mode_set(DCI_SCCB_SCL_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, DCI_SCCB_SCL_PIN);
    gpio_output_options_set(DCI_SCCB_SCL_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ,DCI_SCCB_SCL_PIN);
    
    gpio_mode_set(DCI_SCCB_SDA_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, DCI_SCCB_SDA_PIN);
    gpio_output_options_set(DCI_SCCB_SDA_GPIO_PORT, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ,DCI_SCCB_SDA_PIN);

	IIC_SCL=1;
	IIC_SDA=1;

}

//产生IIC起始信号

void IIC_Start(void)

{

	SDA_OUT();     //sda线输出
	
	IIC_SDA=1;	  	  

	IIC_SCL=1;

	delay_us(IIC_DELAY1);

 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 

	delay_us(IIC_DELAY1);

	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 

}	  

//产生IIC停止信号

void IIC_Stop(void)

{

	SDA_OUT();//sda线输出

	IIC_SCL=0;

	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high

 	delay_us(IIC_DELAY1);

	IIC_SCL=1; 

	IIC_SDA=1;//发送I2C总线结束信号

	delay_us(IIC_DELAY1);							   	

}

//等待应答信号到来

//返回值：1，接收应答失败

//        0，接收应答成功

u8 IIC_Wait_Ack(void)

{

	u8 ucErrTime=0;

	SDA_IN();      //SDA设置为输入  

	IIC_SDA=1;delay_us(IIC_DELAY2);	   

	IIC_SCL=1;delay_us(IIC_DELAY2);	 

	while(READ_SDA)

	{

		ucErrTime++;

		if(ucErrTime>250)

		{

			IIC_Stop();

			return 1;

		}

	}

	IIC_SCL=0;//时钟输出0 	   

	return 0;  

} 

//产生ACK应答

void IIC_Ack(void)

{

	IIC_SCL=0;

	SDA_OUT();

	IIC_SDA=0;

	delay_us(IIC_DELAY2);

	IIC_SCL=1;

	delay_us(IIC_DELAY2);

	IIC_SCL=0;

}

//不产生ACK应答		    

void IIC_NAck(void)

{

	IIC_SCL=0;

	SDA_OUT();

	IIC_SDA=1;

	delay_us(IIC_DELAY2);

	IIC_SCL=1;

	delay_us(IIC_DELAY2);

	IIC_SCL=0;

}					 				     

//IIC发送一个字节

//返回从机有无应答

//1，有应答

//0，无应答			  

void IIC_Send_Byte(u8 txd)

{                        

    u8 t;

	SDA_OUT();

    IIC_SCL=0;//拉低时钟开始数据传输

    for(t=0;t<8;t++)

    {

        IIC_SDA=(txd&0x80)>>7;

        txd<<=1;

		delay_us(IIC_DELAY2);

		IIC_SCL=1;

		delay_us(IIC_DELAY2);

		IIC_SCL=0;

		delay_us(IIC_DELAY2);

    }

}

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   

u8 IIC_Read_Byte(unsigned char ack)

{

	unsigned char i,receive=0;

	SDA_IN();//SDA设置为输入

    for(i=0;i<8;i++ )

	{

        IIC_SCL=0; 

        delay_us(IIC_DELAY2);

		IIC_SCL=1;

        receive<<=1;

        if(READ_SDA)receive++;   

		delay_us(IIC_DELAY2); 

    }					 

    if (!ack)

        IIC_NAck();//发送nACK

    else

        IIC_Ack(); //发送ACK   

    return receive;

}

 

 

 

/* 对器件读写的封装，从机器件地址为1字节 */

 

 

/**

  * @brief  向I2C设备连续写数据（适用于符合IIC通信协议的寄存器地址为uint8类型的器件）

  * @param  addr:I2C从机器件地址

  * @param  reg: I2C从机寄存器地址

  * @param  len: 写入长度

  * @param  buf: uint8数据数组

  * @retval 0,正常; 其他,错误代码;

  */

uint8_t Soft_I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, unsigned char *data_buf)

{

    uint8_t i;

	

	IIC_Start();

	IIC_Send_Byte(dev_addr << 1 | I2C_Direction_Transmitter);//发送器件地址+写命令

	if(IIC_Wait_Ack())//等待应答

	{

		IIC_Stop();

		return 1;

	}

	

	IIC_Send_Byte(reg_addr);//写寄存器地址

    IIC_Wait_Ack();//等待应答

	

	for(i=0;i<len;i++)

	{

		IIC_Send_Byte(data_buf[i]);//发送数据

		if(IIC_Wait_Ack())//等待ACK

		{

			IIC_Stop();

			return 1;

		}

	}

    IIC_Stop();

	return 0;

}

 

/**

  * @brief  从I2C设备连续读数据（适用于符合IIC通信协议的寄存器地址为uint8类型的器件）

  * @param  addr:I2C从机器件地址

  * @param  reg: I2C从机寄存器地址

  * @param  len: 读出长度

  * @param  buf: uint8数据数组

  * @retval 0,正常; 其他,错误代码;

  */

uint8_t Soft_I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, unsigned char *data_buf)

{

//	uint8_t result;

	

	IIC_Start();

	IIC_Send_Byte(dev_addr << 1 | I2C_Direction_Transmitter);//发送器件地址+写命令

	if(IIC_Wait_Ack())//等待应答

	{

		IIC_Stop();

		return 1;

	}

	

	IIC_Send_Byte(reg_addr);//写寄存器地址

    IIC_Wait_Ack();//等待应答

	

	IIC_Start();

	IIC_Send_Byte(dev_addr << 1 | I2C_Direction_Receiver);//发送器件地址+读命令

	IIC_Wait_Ack();//等待应答

	

    while(len)

	{

		if(len==1)*data_buf=IIC_Read_Byte(0);//读数据,发送nACK

		else *data_buf=IIC_Read_Byte(1);//读数据,发送ACK

		len--;

		data_buf++;

	}

    IIC_Stop();//产生一个停止条件

	return 0;

}


u8 ov2640_write(uint8_t reg, uint8_t data)
{
	return Soft_I2C_Write(OV2640_DEVICE_WRITE_ADDRESS>>1,reg,1,&data);	
}

u8 ov2640_read(uint8_t reg)
{
	u8 data;
	Soft_I2C_Read(OV2640_DEVICE_WRITE_ADDRESS>>1,reg,1,&data);
	return data;
}
