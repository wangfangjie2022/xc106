#ifndef _DCMI_H
#define _DCMI_H
#include "sys.h"    									
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//DCMI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
extern void (*dcmi_rx_callback)(void);//DCMI DMA���ջص�����
void My_DCMI_Init(void);
//void DCMI_DMA_Init(u32 mem0addr,u32 mem1addr,u16 memsize,u32 memblen,u32 meminc);
void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize,u32 DMA_MemoryInc);
void DCMI_Start(void);
void DCMI_Stop(void);

void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height);
void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync);
FlagStatus DMA_GetCmdStatus(uint32_t dma_periph, dma_channel_enum channelx);  //wxf add 20220705;

#define DCI_D0_PIN GPIO_PIN_9
#define DCI_D1_PIN GPIO_PIN_7
#define DCI_D2_PIN GPIO_PIN_8
#define DCI_D3_PIN GPIO_PIN_9
#define DCI_D4_PIN GPIO_PIN_11
#define DCI_D5_PIN GPIO_PIN_6
#define DCI_D6_PIN GPIO_PIN_8
#define DCI_D7_PIN GPIO_PIN_9
#define DCI_XCLK_PIN GPIO_PIN_8
#define DCI_PCLK_PIN GPIO_PIN_6
#define DCI_VSYNC_PIN GPIO_PIN_7
#define DCI_HSYNC_PIN GPIO_PIN_4
#define DCI_RST_PIN GPIO_PIN_3
#define DCI_PWD_PIN GPIO_PIN_15

#define DCI_D0_PORT GPIOA
#define DCI_D1_PORT GPIOC
#define DCI_D2_PORT GPIOC
#define DCI_D3_PORT GPIOC
#define DCI_D4_PORT GPIOC
#define DCI_D5_PORT GPIOB
#define DCI_D6_PORT GPIOB
#define DCI_D7_PORT GPIOB
#define DCI_XCLK_PORT GPIOA
#define DCI_PCLK_PORT GPIOA
#define DCI_VSYNC_PORT GPIOB
#define DCI_HSYNC_PORT GPIOA
#define DCI_RST_PORT GPIOB
#define DCI_PWD_PORT GPIOA
		
		
#endif





















