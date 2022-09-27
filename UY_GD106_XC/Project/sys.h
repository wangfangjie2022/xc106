#ifndef __SYS_H
#define __SYS_H	

/*C Language Standard Library*/
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/*Chips and Core Modules*/
#include "gd32f4xx.h"
#include "gd32f4xx_it.h"
//#include "gd32e103c_start.h"

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"

#include "systick.h"

//0,²»Ö§³Öucos
//1,Ö§³Öucos
#define SYSTEM_SUPPORT_UCOS		1		//¶¨ÒåÏµÍ³ÎÄ¼þ¼ÐÊÇ·ñÖ§³ÖUCOS

//#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 


//#define GPIOA_ODR_Addr    (GPIOA+12) //0x4001080C //#define APB2_BUS_BASE         ((uint32_t)0x40010000U)        /*!< apb2 base address                */
//#define GPIOB_ODR_Addr    (GPIOB+12) //0x40010C0C 
//#define GPIOC_ODR_Addr    (GPIOC+12) //0x4001100C 
//#define GPIOD_ODR_Addr    (GPIOD+12)//0x4001140C 
//#define GPIOE_ODR_Addr    (GPIOE+12) //0x4001180C 

//#define GPIOA_IDR_Addr    (GPIOA+8) //0x40010808 
//#define GPIOB_IDR_Addr    (GPIOB+8) //0x40010C08 
//#define GPIOC_IDR_Addr    (GPIOC+8) //0x40011008 
//#define GPIOD_IDR_Addr    (GPIOD+8) //0x40011408 
//#define GPIOE_IDR_Addr    (GPIOE+8) //0x40011808 

#define GPIOA_ODR_Addr    (GPIOA+0x14) //0x4001080C //#define APB2_BUS_BASE         ((uint32_t)0x40010000U)        /*!< apb2 base address                */
#define GPIOB_ODR_Addr    (GPIOB+0x14) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC+0x14) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD+0x14)//0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE+0x14) //0x4001180C 

#define GPIOA_IDR_Addr    (GPIOA+0x10) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB+0x10) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC+0x10) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD+0x10) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE+0x10) //0x40011808 


//#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 


#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //���� 


//#define uint32_t u32 //wxf add 20200420
#define float32_t float   
#define boolean_t u8

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;



typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
//typedef unsigned       __INT64 u64;

typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
//typedef unsigned       __INT64 uint64_t;


#define GPIO_Pin_0 GPIO_PIN_0
#define GPIO_Pin_1 GPIO_PIN_1
#define GPIO_Pin_2 GPIO_PIN_2
#define GPIO_Pin_3 GPIO_PIN_3
#define GPIO_Pin_4 GPIO_PIN_4
#define GPIO_Pin_5 GPIO_PIN_5
#define GPIO_Pin_6 GPIO_PIN_6
#define GPIO_Pin_7 GPIO_PIN_7
#define GPIO_Pin_8 GPIO_PIN_8
#define GPIO_Pin_9 GPIO_PIN_9
#define GPIO_Pin_10 GPIO_PIN_10
#define GPIO_Pin_11 GPIO_PIN_11
#define GPIO_Pin_12 GPIO_PIN_12
#define GPIO_Pin_13 GPIO_PIN_13
#define GPIO_Pin_14 GPIO_PIN_14
#define GPIO_Pin_15 GPIO_PIN_15



//@Alexander C add in 2021

#define ID_BaseAddress	0x1FFF7A10 //20220707 GD32F425
#define RUN_IAP_MODE 0
#define IAP_SIZE 0x4000

typedef enum
{
		sta_Done,
    sta_RDY,					//@state idle 
    sta_BSY,
		sta_initialized,	//@state initialized 
		sta_error,				//@state error
    sta_logtime,    
}states;



#define add 0
#if add

typedef enum
{
		sta_Done,
    sta_RDY,					//@state idle 
    sta_BSY,
		sta_initialized,	//@state initialized 
		sta_error,				//@state error
    sta_logtime,    
}states;


typedef enum
{
	 sys_idel,
   sys_modbus,
   sys_network,
   sys_restart,    //@device restart
   sys_OTA,        //@OTA state
   sys_recovered,  //@device recovered to the factory setting
}sys_moduleWork;


 typedef struct 
{
	 sys_moduleWork     moduleWork;
	 states             wrokstate;
}sysstate;



/**
 * \brief          system state machine loop.
 * 								
 */
uint8_t  SystemstateSwitch(sysstate syswork);
#endif


extern char ChipIDstring[25];	//chipID buffer.
void system_init(void);

#define printf SEGGER_RTT_printf2;

/**
 * \brief          system state machine task scheduling.
 * 								 if Network's interruption break the Modbus's interruption.
 *								 preserve the data stream node.	
 */
void system_task_scheduling(void);

//用于记录中断标志，也可以直接查寄存器标志。
 typedef struct 
{
	 bool USART1IRQstate;
	 bool USART0IRQstate;
}ModuleIRQstate;

/*Use for RTC.*/
void RTCclock(void);
/*inite functions.*/
void init_fun(void);

void Factorymodeset(bool mode);

void GetUniqueID(unsigned char *p);
void GetUniqueID_String(char *string_id);
void sys_reboot(void);

#endif



