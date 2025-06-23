#include "stm32f10x.h"                  // Device header

/*
**@brief :初始化定时器TIM2,使用内部时钟
**@parameters:none
**@rerurn:none
**@
*/
void tim_fclk_init(void)
{
	/*开启外设总线时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/*定时器时钟配置*/
	TIM_InternalClockConfig(TIM2);//使用内部时钟
	
	
	/*定时器初始化*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 10 - 1;//10分频得7.2MHz
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10   - 1;//每计数10个产生一次中断，相当于再进行10分频，得0.72MHz=720 000Hz
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除更新事件标志位
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);//中断使能，到NVIC
	/*定时器中断初始化*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		
	NVIC_Init(&NVIC_InitStruct);
	/*定时器使能*/
	TIM_Cmd(TIM2, ENABLE);	
}


uint16_t get_counter(void)
{
	return TIM_GetCounter(TIM2);
}
 
