#include "PWM.h"


void TIM3_PWM_Init(uint16_t period, uint16_t prescaler)
{ 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef       TIM_OCInitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能定时器3时钟


	/* 使能GPIOB时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	/* 配置LED相应引脚PB1*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_1);

	TIM_TimeBaseStructure.TIM_Period        = period;// 设置自动重装周期值
	TIM_TimeBaseStructure.TIM_Prescaler     = prescaler;//设置预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);    //初始化定时器3

	TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM2;// PWM2模式
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_Low;//输出低
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);//使能预装载寄存器
	TIM_Cmd(TIM3, ENABLE);                          //  使能定时器3

	TIM_SetCompare4(TIM3,1);		
}
