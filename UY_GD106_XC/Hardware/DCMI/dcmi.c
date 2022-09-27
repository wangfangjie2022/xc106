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

u8 ov_frame=0;  						//֡��
extern void jpeg_data_process(void);	//JPEG���ݴ�����


//DCMI�жϷ�����
void DCI_IRQHandler(void)
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif
	if(dci_interrupt_flag_get(DCI_INT_FLAG_EF)==SET)//����һ֡ͼ��
	{
		jpeg_data_process(); 	//jpeg���ݴ���	
		dci_interrupt_flag_clear(DCI_INT_FLAG_EF);//���֡�ж�
		ov_frame++;  
	}
#if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
#endif
} 



void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize,u32 DMA_MemoryInc)
{ 
//	dci_parameter_struct dci_struct;
   dma_single_data_parameter_struct dma_single_struct;
//	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ�DMA2_Stream1������ 
//	dma_fifo_status_get
	    /* DCI DMA configuration */ 
    rcu_periph_clock_enable(RCU_DMA1);
    dma_deinit(DMA1,DMA_CH7);
    dma_single_struct.periph_addr= (uint32_t)DCI_DR_ADDRESS;//�����ַΪ:DCMI->DR
    dma_single_struct.memory0_addr= DMA_Memory0BaseAddr;//DMA �洢��0��ַ
    dma_single_struct.direction= DMA_PERIPH_TO_MEMORY;//���赽�洢��ģʽ
    dma_single_struct.number= DMA_BufferSize;//���ݴ����� 
    dma_single_struct.periph_inc= DMA_PERIPH_INCREASE_DISABLE;//���������ģʽ
    dma_single_struct.memory_inc= DMA_MemoryInc;//�洢������ģʽ
    dma_single_struct.periph_memory_width= DMA_MemoryDataSize;//�洢�����ݳ��� 
    dma_single_struct.priority= DMA_PRIORITY_HIGH;//�����ȼ�
    dma_single_struct.circular_mode= DMA_CIRCULAR_MODE_ENABLE;// ʹ��ѭ��ģʽ 
    dma_single_data_mode_init(DMA1,DMA_CH7,&dma_single_struct);

    dma_channel_subperipheral_select(DMA1,DMA_CH7,DMA_SUBPERI1);
	
	
} 

void (*dcmi_rx_callback)(void);//DCMI DMA���ջص�����
//DMA1_Stream1�жϷ�����(��˫����ģʽ���õ�)
void DMA1_Channel7_IRQHandler(void)
{
#if SYSTEM_SUPPORT_UCOS  //ʹ��UCOS����ϵͳ
	OSIntEnter();    
#endif	
    if(dma_interrupt_flag_get(DMA1, DMA_CH7, DMA_INT_FLAG_FTF)) 
		{
        dma_interrupt_flag_clear(DMA1, DMA_CH7, DMA_INT_FLAG_FTF);//�����������ж�
				dcmi_rx_callback();	//ִ������ͷ���ջص�����,��ȡ���ݵȲ����������洦��  
    }
#if SYSTEM_SUPPORT_UCOS  
	OSIntExit();    	//�˳��ж�
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
//DCMI��ʼ��
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
    
//		dci_deinit();//���ԭ�������� 
    /* DCI configuration */ 
    dci_struct.capture_mode=DCI_CAPTURE_MODE_CONTINUOUS;//����ģʽ 
    dci_struct.clock_polarity=  DCI_CK_POLARITY_RISING;//PCLK ��������Ч
    dci_struct.hsync_polarity= DCI_HSYNC_POLARITY_LOW;//HSYNC �͵�ƽ��Ч
    dci_struct.vsync_polarity= DCI_VSYNC_POLARITY_LOW;//VSYNC �͵�ƽ��Ч
    dci_struct.frame_rate= DCI_FRAME_RATE_ALL;//ȫ֡����
    dci_struct.interface_format= DCI_INTERFACE_FORMAT_8BITS;//8λ���ݸ�ʽ  
    dci_init(&dci_struct);
		
		dci_interrupt_enable(DCI_INT_EF);//����֡�ж� 
		
		dci_enable();//DCMIʹ��
		
		nvic_irq_enable(DCI_IRQn, 0, 0);// set DCI interuput�������ȼ�0 ��ռ���ȼ�0

} 
//DCMI,��������
void DCMI_Start(void)
{  
	/* enable DMA1 channel7 */
  dma_channel_enable(DMA1, DMA_CH7);//����DMA1,Stream1 
	dci_capture_enable();//DCMI����ʹ��  
}
//DCMI,�رմ���
void DCMI_Stop(void)
{ 
  dci_capture_disable();//DCMI����ʹ�ر�	
	
	while(DCI_CTL&0X01);		//�ȴ�������� 
	 	
	dma_channel_disable(DMA1, DMA_CH7);//�ر�DMA1,Stream1 
} 
////////////////////////////////////////////////////////////////////////////////
//������������,��usmart����,���ڵ��Դ���

//DCMI������ʾ����
//sx,sy;LCD����ʼ����
//width,height:LCD��ʾ��Χ.
void DCMI_Set_Window(u16 sx,u16 sy,u16 width,u16 height)
{
	DCMI_Stop(); 

	OV2640_OutSize_Set(width,height);
	
	dma_channel_enable(DMA1, DMA_CH7);//����DMA1,Stream1 
	
	dci_capture_enable();//DCMI����ʹ��  
	
}
   
//ͨ��usmart����,����������.
//pclk/hsync/vsync:�����źŵ����޵�ƽ����
void DCMI_CR_Set(u8 pclk,u8 hsync,u8 vsync)
{
//	DCMI_DeInit();//���ԭ�������� 
// 
//  DCMI_InitStructure.DCMI_CaptureMode=DCMI_CaptureMode_Continuous;//����ģʽ
//	DCMI_InitStructure.DCMI_CaptureRate=DCMI_CaptureRate_All_Frame;//ȫ֡����
//	DCMI_InitStructure.DCMI_ExtendedDataMode= DCMI_ExtendedDataMode_8b;//8λ���ݸ�ʽ  
//	DCMI_InitStructure.DCMI_HSPolarity= hsync<<6;//HSYNC �͵�ƽ��Ч
//	DCMI_InitStructure.DCMI_PCKPolarity= pclk<<5;//PCLK ��������Ч
//	DCMI_InitStructure.DCMI_SynchroMode= DCMI_SynchroMode_Hardware;//Ӳ��ͬ��HSYNC,VSYNC
//	DCMI_InitStructure.DCMI_VSPolarity=vsync<<7;//VSYNC �͵�ƽ��Ч
//	DCMI_Init(&DCMI_InitStructure);

//	DCMI_CaptureCmd(ENABLE);//DCMI����ʹ�� 
//	DCMI_Cmd(ENABLE);	//DCMIʹ��
  
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

//DCMI DMA����
//memaddr:�洢����ַ    ��Ҫ�洢����ͷ���ݵ��ڴ��ַ(Ҳ�����������ַ)
//DMA_BufferSize:�洢������    0~65535
//DMA_MemoryDataSize:�洢��λ��    @defgroup DMA_memory_data_size :DMA_MemoryDataSize_Byte/DMA_MemoryDataSize_HalfWord/DMA_MemoryDataSize_Word
//DMA_MemoryInc:�洢��������ʽ  @defgroup DMA_memory_incremented_mode  /** @defgroup DMA_memory_incremented_mode : DMA_MemoryInc_Enable/DMA_MemoryInc_Disable
 
//void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u32 DMA_Memory1BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize,u32 DMA_MemoryInc)
//{ 
////	dci_parameter_struct dci_struct;
//   dma_single_data_parameter_struct dma_single_struct;
////	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ�DMA2_Stream1������ 
////	dma_fifo_status_get
//	    /* DCI DMA configuration */ 
//    rcu_periph_clock_enable(RCU_DMA1);
//    dma_deinit(DMA1,DMA_CH7);
//    dma_single_struct.periph_addr= (uint32_t)DCI_DR_ADDRESS;//�����ַΪ:DCMI->DR
//    dma_single_struct.memory0_addr= DMA_Memory0BaseAddr;//DMA �洢��0��ַ
//    dma_single_struct.direction= DMA_PERIPH_TO_MEMORY;//���赽�洢��ģʽ
//    dma_single_struct.number= DMA_BufferSize;//���ݴ����� 
//    dma_single_struct.periph_inc= DMA_PERIPH_INCREASE_DISABLE;//���������ģʽ
//    dma_single_struct.memory_inc= DMA_MemoryInc;//�洢������ģʽ
//    dma_single_struct.periph_memory_width= DMA_MemoryDataSize;//�洢�����ݳ��� 
//    dma_single_struct.priority= DMA_PRIORITY_HIGH;//�����ȼ�
//    dma_single_struct.circular_mode= DMA_CIRCULAR_MODE_ENABLE;// ʹ��ѭ��ģʽ 
//    dma_single_data_mode_init(DMA1,DMA_CH7,&dma_single_struct);

//    dma_channel_subperipheral_select(DMA1,DMA_CH7,DMA_SUBPERI1);
//	
//	
//	if(DMA_Memory1BaseAddr)
//  {

//		
//		/* enable dma switch buffer mode */
//    dma_switch_buffer_mode_config(DMA1, DMA_CH7, DMA_Memory1BaseAddr, DMA_MEMORY_1); //����Ŀ���ַ1
//    dma_switch_buffer_mode_enable(DMA1, DMA_CH7, ENABLE); //˫����ģʽ	
//		/* enable DMA1 transfer complete interrupt */
//    dma_interrupt_enable(DMA1, DMA_CH7, DMA_CHXCTL_FTFIE); //������������ж�
//		
//		nvic_irq_enable(DMA1_Channel7_IRQn, 0, 0);//����ָ���Ĳ�����ʼ��VIC�Ĵ����������ȼ�0 ��ռ���ȼ�0
//			
//	}
//	
//	
//		
//	
//} 




