#include "stm32f10x.h"                  // Device header
#include "pwm.h"

extern uint16_t freq_count;		//频率数值
extern uint8_t  amp_count;		//幅度数值
extern uint16_t autoreload;//定时器自动重装值

uint8_t AmpSet_flag;    //幅度设置标志
uint8_t FreqSet_flag;	//频率设置标志

extern uint32_t f_word;//频率控制字
extern float amp_word;//幅度控制
extern uint8_t amp_tens;
extern uint8_t amp_ones;
extern uint8_t set_mode;

uint8_t amp_str[] = {'0','.','0','V','\0'};//定义数组，用于存储幅度值

uint8_t freq_step;//频率步进值设置标志

/*******************************
  * 函    数：旋转编码器初始化
  * 参    数：无
  * 返 回 值：无
  ******************************/
void Encoder_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//开启AFIO的时钟，外部中断必须开启AFIO的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB0和PB1引脚初始化为上拉输入
	
	/*AFIO选择中断引脚*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);//将外部中断的0号线映射到GPIOB，即选择PB0为外部中断引脚
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);//将外部中断的1号线映射到GPIOB，即选择PB1为外部中断引脚
	
	/*EXTI初始化*/
	EXTI_InitTypeDef EXTI_InitStructure;						//定义结构体变量
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;		//选择配置外部中断的0号线和1号线
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//指定外部中断线使能
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//指定外部中断线为中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		//指定外部中断线为下降沿触发
	EXTI_Init(&EXTI_InitStructure);								//将结构体变量交给EXTI_Init，配置EXTI外设
	
	/*NVIC中断分组*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);				//配置NVIC为分组2
																//即抢占优先级范围：0~3，响应优先级范围：0~3
																//此分组配置在整个工程中仅需调用一次
																//若有多个中断，可以把此代码放在main函数内，while循环之前
																//若调用多次配置分组的代码，则后执行的配置会覆盖先执行的配置
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;						//定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//选择配置NVIC的EXTI0线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//选择配置NVIC的EXTI1线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;			//指定NVIC线路的响应优先级为2
	NVIC_Init(&NVIC_InitStructure);								//将结构体变量交给NVIC_Init，配置NVIC外设
}



/*************************************************
  * 函    数：EXTI0外部中断函数
  * 参    数：无
  * 返 回 值：无
  * 
  ***********************************************/
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) == SET)		//判断是否是外部中断0号线触发的中断
	{
		/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
		{
				//进行参数设置形式判断
				if(FreqSet_flag == 1)//频率设置
				{
					
					if(freq_step == 0)//判断步进值
					{
						if(freq_count > 9999)
						{
							freq_count = 10000;
						}
						freq_count -= 1;//频率减1
						f_word = freq_count<<8;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字
						//计算定时器自动重装值，用于生成同步信号
//						autoreload = 50000/freq_count;
//						//设置自动重装值
//						PwmAutoreloadSet(autoreload);
//						BLPwm_SetCompare2(autoreload/2);					
					}
					if(freq_step == 1)//判断步进值
					{
						if(freq_count > 9999)
						{
							freq_count = 10000;
						}
						freq_count -= 100;//频率减100
						f_word = freq_count<<8;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字
						//计算定时器自动重装值，用于生成同步信号
//						autoreload = 50000/freq_count;
//						//设置自动重装值
//						PwmAutoreloadSet(autoreload);
//						BLPwm_SetCompare2(autoreload/2);					
					}
					if(freq_step == 2)//判断步进值
					{
						if(freq_count > 9999)
						{
							freq_count = 10000;
						}
						freq_count -= 1000;//频率减1000
						f_word = freq_count<<8;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字
						//计算定时器自动重装值，用于生成同步信号
//						autoreload = 50000/freq_count;
//						//设置自动重装值
//						PwmAutoreloadSet(autoreload);
//						BLPwm_SetCompare2(autoreload/2);					
					}										
					
//					if(freq_count > 0)
//					{
//						freq_count -= 1;//频率减1
//						f_word = freq_count<<8;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字
//					}	
				}
				
				if(AmpSet_flag == 1)//幅度设置
				{
					if(amp_count > 0)	
					{
						amp_count -= 1;//幅度减1
						amp_tens = amp_count/10;//取出10位 
						amp_ones = amp_count%10;//取出个位
						//amp_word = amp_count/50;//幅度值转换
						amp_str[0] = amp_tens;
						amp_str[2] = amp_ones;
						
					}
				}			
		}
		EXTI_ClearITPendingBit(EXTI_Line0);			//清除外部中断0号线的中断标志位
													//中断标志位必须清除
													//否则中断将连续不断地触发，导致主程序卡死
	}
}

/**********************************************
  * 函    数：EXTI1外部中断函数
  * 参    数：无
  * 返 回 值：无
  **********************************************/
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) == SET)		//判断是否是外部中断1号线触发的中断
	{
		/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
		{	
				//进行参数设置形式判断
				if(FreqSet_flag == 1)//频率设置
				{
					if(freq_step == 0)//步进判断
					{
						if(freq_count < 10000 )
						{
							freq_count += 1;//频率加1
							f_word = freq_count<<8;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字	
							//计算定时器自动重装值，用于生成同步信号
							autoreload = 50000/freq_count;	
							//设置自动重装值
							PwmAutoreloadSet(autoreload);
							BLPwm_SetCompare2(autoreload/2);
						}					
					}
					if(freq_step == 1)//步进判断
					{
						if(freq_count < 10000 )
						{
							freq_count += 100;//频率加100
							f_word = freq_count<<8;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字	
							//计算定时器自动重装值，用于生成同步信号
							autoreload = 50000/freq_count;	
							//设置自动重装值
							PwmAutoreloadSet(autoreload);
							BLPwm_SetCompare2(autoreload/2);
						}					
					}
					if(freq_step == 2)//步进判断
					{
						if(freq_count < 10000 )
						{
							freq_count += 1000;//频率加1000
							f_word = freq_count<<8;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字	
							//计算定时器自动重装值，用于生成同步信号
							autoreload = 50000/freq_count;	
							//设置自动重装值
							PwmAutoreloadSet(autoreload);
							BLPwm_SetCompare2(autoreload/2);
						}					
					}					


				}
				if(AmpSet_flag == 1)//幅度设置
				{
					if(amp_count < 50)
					{
						amp_count += 1;//幅度加1
						amp_tens = amp_count/10;//取出10位 
						amp_ones = amp_count%10;//取出个位						
						//amp_word = amp_count/50;//幅度值转换
						amp_str[0] = amp_tens;
						amp_str[2] = amp_ones;
					}
				}			
		}
		EXTI_ClearITPendingBit(EXTI_Line1);			//清除外部中断1号线的中断标志位
	}												//中断标志位必须清除													//否则中断将连续不断地触发，导致主程序卡死
}
