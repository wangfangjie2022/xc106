#include "rtc.h"
#include "header.h"
#include "delay.h"

#define RTC_CLOCK_SOURCE_IRC32K 1
//////////////////////////////////////////////////////////////////////////////////	 
__IO uint32_t prescaler_a = 0, prescaler_s = 0;				  

rtc_parameter_struct   rtc_initpara;
//ʮ����ת��ΪBCD��
//val:Ҫת����ʮ������
//����ֵ:BCD��
u8 RTC_DEC2BCD(u8 val)
{
	u8 bcdhigh = 0;
	while(val >= 10)
	{
		bcdhigh++;
		val -= 10;
	}
	return ((u8)(bcdhigh<<4)|val);
}

//BCD��ת��Ϊʮ��������
//val:Ҫת����BCD��
//����ֵ:ʮ��������
u8 RTC_BCD2DEC(u8 val)
{
	u8 temp = 0;
	temp = (val>>4)*10;
	return (temp+(val&0x0f));
}


u8 RTC_Set_Date_Time_String(char *time)
{
	//20191009183005
	char tmp[2];
	u8 year,month,date,hour,min,sec;
	
	SEGGER_RTT_printf2("----RTC_Set_Date_Time_String----\r\n");
	memset(tmp,0,2);
	memcpy(tmp,time+2,2);
	year=(tmp[0]-0x30)*10+tmp[1]-0x30;
	SEGGER_RTT_printf2("year=%d",year);
	
	memset(tmp,0,2);
	memcpy(tmp,time+4,2);
	month=(tmp[0]-0x30)*10+tmp[1]-0x30;
	SEGGER_RTT_printf2("month=%d",month);
	
	memset(tmp,0,2);
	memcpy(tmp,time+6,2);
	date=(tmp[0]-0x30)*10+tmp[1]-0x30;
	SEGGER_RTT_printf2("date=%d",date);
	
	memset(tmp,0,2);
	memcpy(tmp,time+8,2);
	hour=(tmp[0]-0x30)*10+tmp[1]-0x30;
	SEGGER_RTT_printf2("hour=%d",hour);
	
	memset(tmp,0,2);
	memcpy(tmp,time+10,2);
	min=(tmp[0]-0x30)*10+tmp[1]-0x30;
	SEGGER_RTT_printf2("min=%d",min);
	
	memset(tmp,0,2);
	memcpy(tmp,time+12,2);
	sec=(tmp[0]-0x30)*10+tmp[1]-0x30;
	SEGGER_RTT_printf2("sec=%d",sec);
	
	RTC_Set_Date(year,month,date,1);
	RTC_Set_Time(hour,min,sec,0);
	return 1;
	//Ӧ�������ݴ����
	
	//myfree(SRAMIN,tmp);
}
//RTCʱ������
//hour,min,sec:Сʱ,����,����
//ampm:AM/PM,0=AM/24H,1=PM
//����ֵ:0,�ɹ�
//			 1,�����ʼ��ģʽʧ��
u8 RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	/* setup RTC time value */
    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.hour = RTC_DEC2BCD(hour);
    rtc_initpara.minute = RTC_DEC2BCD(min);
    rtc_initpara.second = RTC_DEC2BCD(sec);
    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara))
		{
        return 1;
    }
		else
		{
        return 0;
    }
}

//RTC��������
//year,month,date:��(0~99),��(1~12),��(0~31)
//week:����(1~7,0,�Ƿ�!)
//����ֵ:0,�ɹ�
//       1,�����ʼ��ģʽʧ��
u8 RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = RTC_DEC2BCD(year);
    rtc_initpara.day_of_week = (week);
    rtc_initpara.month = (month);
    rtc_initpara.date = RTC_DEC2BCD(date);
    rtc_initpara.display_format = RTC_24HOUR;
		SEGGER_RTT_printf2("RTC_Set_Date %d-%d-%d\r\n",year,month,date,week);
		/* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara))
		{
				SEGGER_RTT_printf2("RTC_Set_Date err\r\n");
        return 1;
    }
		else
		{
        return 0;
    }
}

u8 RTC_Set_Date_Time(u8 year,u8 month,u8 date,u8 hour,u8 min,u8 sec)
{
		rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = RTC_DEC2BCD(year);
    rtc_initpara.month = (month);
    rtc_initpara.date = RTC_DEC2BCD(date);
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.hour = RTC_DEC2BCD(hour);
    rtc_initpara.minute = RTC_DEC2BCD(min);
    rtc_initpara.second = RTC_DEC2BCD(sec);
		/* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara))
		{
				SEGGER_RTT_printf2("RTC_Set_Date err\r\n");
        return 1;
    }
		else
		{
        return 0;
    }
}


//��ȡRTCʱ��
//*hour,*min,*sec:Сʱ,����,���� 
//*ampm:AM/PM,0=AM/24H,1=PM.
void RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm)
{
	rtc_current_time_get(&rtc_initpara);
	*hour = RTC_BCD2DEC(rtc_initpara.hour); //��ȡСʱ
	*min = RTC_BCD2DEC(rtc_initpara.minute); //��ȡ����
	*sec = RTC_BCD2DEC(rtc_initpara.second); //��ȡ����
	*ampm = rtc_initpara.am_pm;  //��ȡampm
}
//��ȡRTC����
//*year,*mon,*date:��,��,��
//*week:����
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week)
{
	rtc_current_time_get(&rtc_initpara);
	*year = RTC_BCD2DEC(rtc_initpara.year);
	*month = rtc_initpara.month;
	*date = RTC_BCD2DEC(rtc_initpara.date);
	*week = rtc_initpara.day_of_week;
	SEGGER_RTT_printf2("RTC_Get_Date %d-%d-%d\r\n",*year,*month,*date);
}

void RTC_Get_String_Date_Time(char* string_time)
{
	rtc_current_time_get(&rtc_initpara);
	sprintf(string_time,"20%02d%02d%02d%02d%02d%02d",RTC_BCD2DEC(rtc_initpara.year),RTC_BCD2DEC(rtc_initpara.month),RTC_BCD2DEC(rtc_initpara.date),RTC_BCD2DEC(rtc_initpara.hour),RTC_BCD2DEC(rtc_initpara.minute),RTC_BCD2DEC(rtc_initpara.second));
}

//RTC��ʼ��
//����ֵ:0,��ʼ���ɹ�;
//       1,LSE����ʧ��;
//       2,�����ʼ��ģʽʧ��;
void  RTC_Timer_Init(void)
{
	
	    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();
	
		#if defined (RTC_CLOCK_SOURCE_IRC32K)
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);

					prescaler_s = 0x110;
          prescaler_a = 0x7F;
//          prescaler_s = 0x13F;
//          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
			#error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
	
}

/*!
    \brief      display the current time
    \param[in]  none
    \param[out] none
    \retval     none
*/

void rtc_show_date(void)
{
	u8 year,month,date,week;
	rtc_current_time_get(&rtc_initpara);
	year = RTC_BCD2DEC(rtc_initpara.year);
	month = (rtc_initpara.month);
	date = RTC_BCD2DEC(rtc_initpara.date);
	week = rtc_initpara.day_of_week;
	SEGGER_RTT_printf2("RTC_Get_Date %d-%d-%d\r\n",year,month,date,week);
}

void rtc_show_time(void)
{
    uint32_t time_subsecond = 0;
    uint8_t subsecond_ss = 0,subsecond_ts = 0,subsecond_hs = 0;

    rtc_current_time_get(&rtc_initpara);

    /* get the subsecond value of current time, and convert it into fractional format */
    time_subsecond = rtc_subsecond_get();
    subsecond_ss=(1000-(time_subsecond*1000+1000)/400)/100;
    subsecond_ts=(1000-(time_subsecond*1000+1000)/400)%100/10;
    subsecond_hs=(1000-(time_subsecond*1000+1000)/400)%10;

    SEGGER_RTT_printf2("Current time: %0.2d:%0.2d:%0.2d .%d%d%d \n\r", \
          RTC_BCD2DEC(rtc_initpara.hour), RTC_BCD2DEC(rtc_initpara.minute), RTC_BCD2DEC(rtc_initpara.second),\
          subsecond_ss, subsecond_ts, subsecond_hs);
}

//��������ʱ��(����������,24Сʱ��)
//week:���ڼ�(1~7)
//hour,min,sec:Сʱ,����,����
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{

}

//�����Ի��Ѷ�ʱ������
//wksel:000,RTC/16;001,RTC/8;010,RTC/4;011,RTC/2;
//      10x,ck_spre,1Hz;11x,1Hz/2^16.
//ע��:RTC����RTC��������ʱ��Ƶ��!
//cnt:�Զ���װ��ֵ.����0,�����ж�.
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{
//	u32 temp;
	;
}

//RTC�����жϷ�����
void RTC_Alarm_IRQHandler(void)
{
    if(RESET != rtc_flag_get(RTC_FLAG_ALRM0)){
        rtc_flag_clear(RTC_FLAG_ALRM0);
    } 
}


//RTC WAKE UP�жϷ�����
void RTC_WKUP_IRQHandler(void)
{
    if(RESET != rtc_flag_get(RTC_FLAG_WT))
		{
        rtc_flag_clear(RTC_FLAG_WT); 
    } 
}










