#include "stm32f10x.h"                  // Device header

/*
**@brief :��ʼ����ʱ��TIM2,ʹ���ڲ�ʱ��
**@parameters:none
**@rerurn:none
**@
*/
void tim_fclk_init(void)
{
	/*������������ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	/*��ʱ��ʱ������*/
	TIM_InternalClockConfig(TIM2);//ʹ���ڲ�ʱ��
	
	
	/*��ʱ����ʼ��*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 10 - 1;//10��Ƶ��7.2MHz
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 10   - 1;//ÿ����10������һ���жϣ��൱���ٽ���10��Ƶ����0.72MHz=720 000Hz
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//��������¼���־λ
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);//�ж�ʹ�ܣ���NVIC
	/*��ʱ���жϳ�ʼ��*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ���
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		
	NVIC_Init(&NVIC_InitStruct);
	/*��ʱ��ʹ��*/
	TIM_Cmd(TIM2, ENABLE);	
}


uint16_t get_counter(void)
{
	return TIM_GetCounter(TIM2);
}
 
