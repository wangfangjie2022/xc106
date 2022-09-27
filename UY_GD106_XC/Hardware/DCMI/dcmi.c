#include "sys.h"
#include "dcmi.h" 

#include "sys.h" 

#include "gd32f4xx.h" 

#include "ov2640.h"
//#include "ov2640cfg.h"
#include "gd32f4xx_dci.h"
#include "systick.h"
#include "header.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//DCMI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/14
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

u8 ov_frame=0;  						//帧率
extern void jpeg_data_process(void);	//JPEG数据处理函数


//DCMI中断服务函数
void DCI_IRQHandler(void)
{
#if SYSTEM_SUPPORT_UCOS  //使用UCOS操作系统
	OSIntEnter();    
#endif
	if(dci_interrupt_flag_get(DCI_INT_FLAG_EF)==SET)//捕获到一帧图像
	{
		jpeg_data_process(); 	//jpeg数据处理	
		dci_interrupt_flag_clear(DCI_INT_FLAG_EF);//清除帧中断
		ov_frame++;  
	}
#if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//退出中断
#endif
} 



void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize,u32 DMA_MemoryInc)
{ 
//	dci_parameter_struct dci_struct;
   dma_single_data_parameter_struct dma_single_struct;
//	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//等待DMA2_Stream1可配置 
//	dma_fifo_status_get
	    /* DCI DMA configuration */ 
    rcu_periph_clock_enable(RCU_DMA1);
    dma_deinit(DMA1,DMA_CH7);
    dma_single_struct.periph_addr= (uint32_t)DCI_DR_ADDRESS;//外设地址为:DCMI->DR
    dma_single_struct.memory0_addr= DMA_Memory0BaseAddr;//DMA 存储器0地址
    dma_single_struct.direction= DMA_PERIPH_TO_MEMORY;//外设到存储器模式
    dma_single_struct.number= DMA_BufferSize;//数据传输量 
    dma_single_struct.periph_inc= DMA_PERIPH_INCREASE_DISABLE;//外设非增量模式
    dma_single_struct.memory_inc= DMA_MemoryInc;//存储器增量模式
    dma_single_struct.periph_memory_width= DMA_MemoryDataSize;//存储器数据长度 
    dma_single_struct.priority= DMA_PRIORITY_HIGH;//高优先级
    dma_single_struct.circular_mode= DMA_CIRCULAR_MODE_ENABLE;// 使用循环模式 
    dma_single_data_mode_init(DMA1,DMA_CH7,&dma_single_struct);

    dma_channel_subperipheral_select(DMA1,DMA_CH7,DMA_SUBPERI1);
	
	
} 

void (*dcmi_rx_callback)(void);//DCMI DMA接收回调函数
//DMA1_Stream1中断服务函数(仅双缓冲模式会用到)
void DMA1_Channel7_IRQHandler(void)
{
#if SYSTEM_SUPPORT_UCOS  //使用UCOS操作系统
	OSIntEnter();    
#endif	
    if(dma_interrupt_flag_get(DMA1, DMA_CH7, DMA_INT_FLAG_FTF)) 
		{
        dma_interrupt_flag_clear(DMA1, DMA_CH7, DMA_INT_FLAG_FTF);//清除传输完成中断
				dcmi_rx_callback();	//执行摄像头接收回调函数,读取数据等操作在这里面处理  
    }
#if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//退出中断
#endif
}

/*
	D0  PA9
	D1  PC7
	D2  PC8
	D3  PC9
	D4  PC11
	D5  PB6
	D6  PB8
	D7  PB9
	XCLK PA8
	PIXCLK PA6
	VSYNC PB7
	HSYNC PA4
	RST PB3
	PD PA15	
	SCL PB10
	SDA PB11
*/
	////////////////////////////////////////////////////////////////////////////////////////////
//DCMI初始化
void My_DCMI_Init(void)
{
		dci_parameter_struct dci_struct;
//    dma_single_data_parameter_struct dma_single_struct;
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_DCI);
		

    
    /* DCI GPIO AF configuration */ 
    gpio_af_set(DCI_D0_PORT, GPIO_AF_13, DCI_D0_PIN);
    gpio_af_set(DCI_D1_PORT, GPIO_AF_13, DCI_D1_PIN);
    gpio_af_set(DCI_D2_PORT, GPIO_AF_13, DCI_D2_PIN);
    gpio_af_set(DCI_D3_PORT, GPIO_AF_13, DCI_D3_PIN);
    gpio_af_set(DCI_D4_PORT, GPIO_AF_13, DCI_D4_PIN);
    gpio_af_set(DCI_D5_PORT, GPIO_AF_13, DCI_D5_PIN);
    gpio_af_set(DCI_D6_PORT, GPIO_AF_13, DCI_D6_PIN);
    gpio_af_set(DCI_D7_PORT, GPIO_AF_13, DCI_D7_PIN);
    gpio_af_set(DCI_PCLK_PORT, GPIO_AF_13, DCI_PCLK_PIN);
    gpio_af_set(DCI_VSYNC_PORT, GPIO_AF_13, DCI_VSYNC_PIN);
    gpio_af_set(DCI_HSYNC_PORT, GPIO_AF_13, DCI_HSYNC_PIN);

    /* configure DCI_PIXCLK(PA6), DCI_VSYNC(PG9), DCI_HSYNC(PA4), DCI_D0(PC6), DCI_D1(PC7)
                 DCI_D2(PC8), DCI_D3(PC9), DCI_D4(PC11), DCI_D5(PD3), DCI_D6(PB8), DCI_D7(PB9) */
    gpio_mode_set(DCI_D0_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_D0_PIN);
    gpio_output_options_set(DCI_D0_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_D0_PIN);
		
    gpio_mode_set(DCI_D1_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_D1_PIN);
    gpio_output_options_set(DCI_D1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_D1_PIN);
    
    gpio_mode_set(DCI_D2_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_D2_PIN);
    gpio_output_options_set(DCI_D2_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_D2_PIN);
		
    gpio_mode_set(DCI_D3_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_D3_PIN);
    gpio_output_options_set(DCI_D3_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_D3_PIN);
		
    gpio_mode_set(DCI_D4_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_D4_PIN);
    gpio_output_options_set(DCI_D4_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_D4_PIN);
		
    gpio_mode_set(DCI_D5_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_D5_PIN);
    gpio_output_options_set(DCI_D5_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_D5_PIN);
		
    gpio_mode_set(DCI_D6_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_D6_PIN);
    gpio_output_options_set(DCI_D6_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_D6_PIN);
    
    gpio_mode_set(DCI_D7_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_D7_PIN);
    gpio_output_options_set(DCI_D7_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_D7_PIN);
		
    gpio_mode_set(DCI_PCLK_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_PCLK_PIN);
    gpio_output_options_set(DCI_PCLK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_PCLK_PIN);
    
    gpio_mode_set(DCI_VSYNC_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_VSYNC_PIN);
    gpio_output_options_set(DCI_VSYNC_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_VSYNC_PIN);

    gpio_mode_set(DCI_HSYNC_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, DCI_HSYNC_PIN);
    gpio_output_options_set(DCI_HSYNC_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX,DCI_HSYNC_PIN);
    
//		dci_deinit();//清除原来的设置 
    /* DCI configuration */ 
    dci_struct.capture_mode=DCI_CAPTURE_MODE_CONTINUOUS;//连续模式 
    dci_struct.clock_polarity=  DCI_CK_POLARITY_RISING;//PCLK 上升沿有效
    dci_struct.hsync_polarity= DCI_HSYNC_POLARITY_LOW;//HSYNC 低电平有效
    dci_struct.vsync_polarity= DCI_VSYNC_POLARITY_LOW;//VSYNC 低电平有效
    dci_struct.frame_rate= DCI_FRAME_RATE_ALL;//全帧捕获
    dci_struct.interface_format= DCI_INTERFACE_FORMAT_8BITS;//8位数据格式  
    dci_init(&dci_struct);
		
		dci_interrupt_enable(DCI_INT_EF);//开启帧中断 
		
		dci_enable();//DCMI使能
		
		nvic_irq_enable(DCI_IRQn, 0, 0);// set DCI interuput、子优先级0 抢占优先级0

} 
//DCMI,启动传输
void DCMI_Start(void)
{  
	/* enable DMA1 channel7 */
  dma_channel_enable(DMA1, DMA_CH7);//开启DMA1,Stream1 
	dci_capture_enable();//DCMI捕获使能  
}
//DCMI,关闭传输
void DCMI_Stop(void)
{ 
  dci_capture_disable();//DCMI捕获使关闭	
	
	while(DCI_CTL&0X01);		//等待传输结束 
	 	
	dma_channel_disable(DMA1, DMA_CH7);//关闭DMA1,Stream1 
} 
////////////////////////////////////////////////////////////////////////////////
//以下两个函数,供usmart调用,用于调试代码

//DCMI设置显示窗口
//sx,sy;LCD的起始坐标
//width,height:LCD显示范围.
void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{
	DCMI_Stop(); 

	OV2640_OutSize_Set(width,height);
	
	dma_channel_enable(DMA1, DMA_CH7);//开启DMA1,Stream1 
	
	dci_capture_enable();//DCMI捕获使能  
	
}
   
//通过usmart调试,辅助测试用.
//pclk/hsync/vsync:三个信号的有限电平设置
void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync)
{
//	DCMI_DeInit();//清除原来的设置 
// 
//  DCMI_InitStructure.DCMI_CaptureMode=DCMI_CaptureMode_Continuous;//连续模式
//	DCMI_InitStructure.DCMI_CaptureRate=DCMI_CaptureRate_All_Frame;//全帧捕获
//	DCMI_InitStructure.DCMI_ExtendedDataMode= DCMI_ExtendedDataMode_8b;//8位数据格式  
//	DCMI_InitStructure.DCMI_HSPolarity= hsync<<6;//HSYNC 低电平有效
//	DCMI_InitStructure.DCMI_PCKPolarity= pclk<<5;//PCLK 上升沿有效
//	DCMI_InitStructure.DCMI_SynchroMode= DCMI_SynchroMode_Hardware;//硬件同步HSYNC,VSYNC
//	DCMI_InitStructure.DCMI_VSPolarity=vsync<<7;//VSYNC 低电平有效
//	DCMI_Init(&DCMI_InitStructure);

//	DCMI_CaptureCmd(ENABLE);//DCMI捕获使能 
//	DCMI_Cmd(ENABLE);	//DCMI使能
  
}
/**
  * @brief  Returns the status of EN bit for the specified DMAy Streamx.
  * @param  DMAy_Streamx: where y can be 1 or 2 to select the DMA and x can be 0
  *          to 7 to select the DMA Stream.
  *   
  * @note    After configuring the DMA Stream (DMA_Init() function) and enabling
  *          the stream, it is recommended to check (or wait until) the DMA Stream
  *          is effectively enabled. A Stream may remain disabled if a configuration
  *          parameter is wrong.
  *          After disabling a DMA Stream, it is also recommended to check (or wait 
  *          until) the DMA Stream is effectively disabled. If a Stream is disabled
  *          while a data transfer is ongoing, the current data will be transferred
  *          and the Stream will be effectively disabled only after the transfer
  *          of this single data is finished.  
  *      
  * @retval Current state of the DMAy Streamx (ENABLE or DISABLE).
  */
FlagStatus DMA_GetCmdStatus(uint32_t dma_periph, dma_channel_enum channelx)  //wxf add 20220705
{
  FlagStatus state = RESET;
	
  if ((DMA_CHCTL(dma_periph, channelx) & (uint32_t)DMA_CHXCTL_CHEN) != 0)
  {
    /* The selected DMAy Streamx EN bit is set (DMA is still transferring) */
    state = SET;
  }
  else
  {
    /* The selected DMAy Streamx EN bit is cleared (DMA is disabled and 
        all transfers are complete) */
    state = RESET;
  }
  return state;
}

//DCMI DMA配置
//memaddr:存储器地址    将要存储摄像头数据的内存地址(也可以是外设地址)
//DMA_BufferSize:存储器长度    0~65535
//DMA_MemoryDataSize:存储器位宽    @defgroup DMA_memory_data_size :DMA_MemoryDataSize_Byte/DMA_MemoryDataSize_HalfWord/DMA_MemoryDataSize_Word
//DMA_MemoryInc:存储器增长方式  @defgroup DMA_memory_incremented_mode  /** @defgroup DMA_memory_incremented_mode : DMA_MemoryInc_Enable/DMA_MemoryInc_Disable
 
//void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u32 DMA_Memory1BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize,u32 DMA_MemoryInc)
//{ 
////	dci_parameter_struct dci_struct;
//   dma_single_data_parameter_struct dma_single_struct;
////	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//等待DMA2_Stream1可配置 
////	dma_fifo_status_get
//	    /* DCI DMA configuration */ 
//    rcu_periph_clock_enable(RCU_DMA1);
//    dma_deinit(DMA1,DMA_CH7);
//    dma_single_struct.periph_addr= (uint32_t)DCI_DR_ADDRESS;//外设地址为:DCMI->DR
//    dma_single_struct.memory0_addr= DMA_Memory0BaseAddr;//DMA 存储器0地址
//    dma_single_struct.direction= DMA_PERIPH_TO_MEMORY;//外设到存储器模式
//    dma_single_struct.number= DMA_BufferSize;//数据传输量 
//    dma_single_struct.periph_inc= DMA_PERIPH_INCREASE_DISABLE;//外设非增量模式
//    dma_single_struct.memory_inc= DMA_MemoryInc;//存储器增量模式
//    dma_single_struct.periph_memory_width= DMA_MemoryDataSize;//存储器数据长度 
//    dma_single_struct.priority= DMA_PRIORITY_HIGH;//高优先级
//    dma_single_struct.circular_mode= DMA_CIRCULAR_MODE_ENABLE;// 使用循环模式 
//    dma_single_data_mode_init(DMA1,DMA_CH7,&dma_single_struct);

//    dma_channel_subperipheral_select(DMA1,DMA_CH7,DMA_SUBPERI1);
//	
//	
//	if(DMA_Memory1BaseAddr)
//  {

//		
//		/* enable dma switch buffer mode */
//    dma_switch_buffer_mode_config(DMA1, DMA_CH7, DMA_Memory1BaseAddr, DMA_MEMORY_1); //配置目标地址1
//    dma_switch_buffer_mode_enable(DMA1, DMA_CH7, ENABLE); //双缓冲模式	
//		/* enable DMA1 transfer complete interrupt */
//    dma_interrupt_enable(DMA1, DMA_CH7, DMA_CHXCTL_FTFIE); //开启传输完成中断
//		
//		nvic_irq_enable(DMA1_Channel7_IRQn, 0, 0);//根据指定的参数初始化VIC寄存器、子优先级0 抢占优先级0
//			
//	}
//	
//	
//		
//	
//} 




