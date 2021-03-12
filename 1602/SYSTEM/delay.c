#include "delay.h"
#include "includes.h"
//static int16_t  fac_us=600;//100us
static int32_t fac_ms=48000;//1ms

static __IO uint32_t TimingDelay;
static __IO uint32_t g_ul_us_ticks=0;



void delay_us(int16_t nus)
{    
	u16 i = 0 ; 

	while(nus--){

		i = 10 ; 

		while(i--);

	} 
}

void delay_init()	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//HCLK/8	
	while(SysTick_Config(fac_ms)) //1ms进中断一次
		;
}	
/*
//延时Nus
void delay_us(int32_t nus)
{		
//	int32_t temp;	    	 
//	SysTick->LOAD=nus*fac_us; //时间加载	  		 
//	SysTick->VAL=0x00;        //清空计数器
//	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数	 
//	do
//	{
//		temp=SysTick->CTRL;
//	}
//	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
//	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
//	SysTick->VAL =0X00;       //清空计数器	 
	TimingDelay = nus/10;
	while(TimingDelay!=0)
		;
}
*/

//延时Nms
void delay_ms(int16_t nms)
{	 		  	  
	int32_t temp;		   
	SysTick->LOAD=(int32_t)nms*fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  
//	TimingDelay = nms;
//	while(TimingDelay!=0)
		;
} 

 /**
 * @brief 获取节拍程序
 * @param 无
 * @retval 无
 * @attention 在 SysTick 中断函数 SysTick_Handler()调用
 */
 void TimingDelay_Decrement(void)
 {
	 if (TimingDelay != 0) {
	 TimingDelay--;
	}
 }

  /**
 * @brief 获取当前毫秒值
 * @param 存储最新毫秒值的变量
 * @retval 无
 */
 int get_tick_count(unsigned long *count)
 {
	 count[0] = g_ul_us_ticks;
	 return 0;
 }
 
  /**
 * @brief 毫秒累加器，在中断里每1ms加 1
 * @param 无
 * @retval 无
 */
void TimeStamp_Increment (void)
{
    g_ul_us_ticks++;
}
 
