#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint8_t waveSwitch_flag = 0;//波形切换标志

uint8_t freqAdd_flag = 0;
uint8_t freqDec_flag = 0;
uint8_t freqStep_flag = 0;

uint8_t magAdd_flag = 0; 
uint8_t magDec_flag = 0;
uint8_t magStep_flag = 0; 

uint8_t dutySet_flag = 0;


					 
/*****************************************
*@func	   :void Key_Init(void)
*@brief    :按键初始化
*@parameter:无 
*@retval   :无
*****************************************/
void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	/*EXTI初始化*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource9);
	
	EXTI_InitTypeDef EXTI_InitStructure;						//定义结构体变量
	EXTI_InitStructure.EXTI_Line = EXTI_Line8| EXTI_Line9;		//选择配置外部中断的8号线
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//指定外部中断线使能
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//指定外部中断线为中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//指定外部中断线为下降沿触发
	EXTI_Init(&EXTI_InitStructure);	
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~1，响应优先级范围：0~7
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前															//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			
	NVIC_Init(&NVIC_InitStructure);		
}

void EC11_1_Init(void)
{
	/*幅度按键*/
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	/*旋转编码器-GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1|GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	/*EXTI初始化*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	
	EXTI_InitTypeDef EXTI_InitStructure;						//定义结构体变量
	EXTI_InitStructure.EXTI_Line = EXTI_Line1| EXTI_Line10| EXTI_Line11;		//选择配置外部中断的8号线
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//指定外部中断线使能
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//指定外部中断线为中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//指定外部中断线为下降沿触发
	EXTI_Init(&EXTI_InitStructure);	
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~1，响应优先级范围：0~7
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前															//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			
	NVIC_Init(&NVIC_InitStructure);	
}

void EC11_1_KeyInit(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	/*旋转编码器-GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	/*EXTI初始化*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);
	
	EXTI_InitTypeDef EXTI_InitStructure;						//定义结构体变量
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;		//选择配置外部中断的1号线
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//指定外部中断线使能
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//指定外部中断线为中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//指定外部中断线为下降沿触发
	EXTI_Init(&EXTI_InitStructure);	
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//配置NVIC为分组2
																//即抢占优先级范围：0~1，响应优先级范围：0~7
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前															//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量	
	
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);	


}
void EC11_2_Init(void)
{
	
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	/*旋转编码器-GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource15);
	
	EXTI_InitTypeDef EXTI_InitStructure;						//定义结构体变量
	EXTI_InitStructure.EXTI_Line = EXTI_Line13| EXTI_Line14| EXTI_Line15;		//
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//指定外部中断线使能
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//指定外部中断线为中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//指定外部中断线为下降沿触发
	EXTI_Init(&EXTI_InitStructure);	
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				//配置NVIC为分组1
																//即抢占优先级范围：0~1，响应优先级范围：0~7
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前															//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							//将结构体变量交给NVIC_Init，配置NVIC外设
	
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			
	NVIC_Init(&NVIC_InitStructure);							
	
	NVIC_InitStructure.NVIC_IRQChannel =  EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			
	NVIC_Init(&NVIC_InitStructure);	
			
}

/*****************************************
*@func	   :GpioSyncInit(void)
*@brief    :同步信号输出IO口初始化
*@parameter:无 
*@retval   :无
*****************************************/
void GpioSyncInit(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA的时钟
	/*旋转编码器-GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*************************************************
  * 函    数：EXTI1外部中断函数,幅度步进切换
  * 参    数：无
  * 返 回 值：无
  * 
  ***********************************************/
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) == SET)		//判断是否是外部中断1号线触发的中断
	{
		magStep_flag = 1;

		EXTI_ClearITPendingBit(EXTI_Line1);			//清除外部中断1号线的中断标志位
													//中断标志位必须清除													//否则中断将连续不断地触发，导致主程序卡死
	}
}

/*************************************************
  * 函    数：EXTI8外部中断函数,波形切换
  * 参    数：无
  * 返 回 值：无
  * 
  ***********************************************/
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line8) == SET)		//判断是否是外部中断8号线触发的中断
	{
		/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0)
		{
			waveSwitch_flag = 1;
		}
		EXTI_ClearITPendingBit(EXTI_Line8);			
													//中断标志位必须清除
													//否则中断将连续不断地触发，导致主程序卡死
	}
	
	if(EXTI_GetITStatus(EXTI_Line9) == SET)		//判断是否是外部中断9号线触发的中断
	{
//		if(dutySet_flag == 0){
//			dutySet_flag = 1;
//		}else if(dutySet_flag == 1){
//		    dutySet_flag = 0;
//		}
		EXTI_ClearITPendingBit(EXTI_Line9);			
													//中断标志位必须清除													//否则中断将连续不断地触发，导致主程序卡死
	}
}

/*************************************************
  * 函    数：EXTI15~10外部中断函数,波形切换
  * 参    数：无
  * 返 回 值：无
  * 
  ***********************************************/
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line10) == SET)//幅度增加
	{
		/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
		{
			magDec_flag = 1;
		}
		EXTI_ClearITPendingBit(EXTI_Line10);																												
	}
	
	if (EXTI_GetITStatus(EXTI_Line11) == SET)//幅度减小
	{
		/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
		{			
			magAdd_flag = 1;
		}
		EXTI_ClearITPendingBit(EXTI_Line11);																												
	}
	
	if (EXTI_GetITStatus(EXTI_Line13) == SET)	//频率步进切换	
	{
		freqStep_flag = 1;
		
		EXTI_ClearITPendingBit(EXTI_Line13);	
    }
	
	if (EXTI_GetITStatus(EXTI_Line14) == SET)//频率增加
	{
		/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
		{
			freqDec_flag = 1;			
		}
		
		EXTI_ClearITPendingBit(EXTI_Line14);																												
	}
	
	if (EXTI_GetITStatus(EXTI_Line15) == SET)//频率减小
	{
		/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
		if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
		{
			freqAdd_flag = 1;
		}
		EXTI_ClearITPendingBit(EXTI_Line15);																												
	}
	
}




