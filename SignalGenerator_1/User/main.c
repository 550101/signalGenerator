#include "Key.h"
#include "Tlc5615.h"
#include "wave.h"
#include "Delay.h"
#include "tim_fclk.h"
#include "OLED.h"

#define MAG_COUNT_MAX 90

#define MAG_COUNT_MAX1 92

#define MAG_COUNT_MAX2 90


#define MAG_OVER_NUM 100

#define FWORD_ADJUST 10



//#define 

volatile uint16_t data_buf[1024] = {0};//波形数据缓冲区

uint16_t voltage_Ajust[20] = {95,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const uint16_t DC_OFFSET = 512;  // 固定的直流偏置（中间值）

//设置标志
extern uint8_t waveSwitch_flag;
extern uint8_t freqAdd_flag;
extern uint8_t freqDec_flag;
extern uint8_t freqStep_flag;
extern uint8_t magAdd_flag; 
extern uint8_t magDec_flag;
extern uint8_t magStep_flag; 
extern uint8_t dutySet_flag;
//函数定义
void WaveSwitchDeal(void);//波形切换处理
void MainLoop(void);//主循环
//void DutySet(void);//矩形波占空比设置

//相关参数定义
volatile uint16_t mag_count = 0;	 //幅度数值
volatile uint16_t mag_count_delay = 0;	 //幅度显示数值

volatile uint16_t mag_hundrd = 0;//幅度百位上的值
volatile uint16_t mag_tens = 0;//幅度十位上的值
volatile uint16_t mag_uint = 0;//幅度个位上的值


volatile uint16_t mag_step = 1;      //幅度步进，初始化为0.1V
volatile uint16_t mag_step_tens = 0;
volatile uint16_t mag_step_ones = 1;
volatile uint8_t wave_slect = 0;     //波形选择
volatile uint32_t phase_counter = 0; //相位累加器
volatile uint32_t f_word = 0;        //频率控制字
volatile uint16_t freq_count = 0;	//频率数值
volatile uint16_t freq_step = 1;     //频率步进，初始化为1Hz


              
/************************************
*
*	main函数
*
*************************************/
int main(void)
{
	/*模块初始化*/	
	OLED_Init();
	dac_gpio_init();
	Key_Init();
	EC11_1_KeyInit();
	EC11_1_Init();
	EC11_2_Init();
	tim_fclk_init();
	
	
	OLED_ShowString(1, 1, "wave:");
	WaveSwitchDeal();
	OLED_ShowString(1, 6, "DC ");//默认输出直流
	OLED_ShowString(2, 1, "f:");
	OLED_ShowNum(2, 4,freq_count,4);
	OLED_ShowString(2, 9, "Hz");
	OLED_ShowString(3, 1, "s:");
	OLED_ShowNum(3, 4,freq_step,4);
	OLED_ShowString(3, 9, "Hz");
	
	OLED_ShowString(4, 1, "Vp:");
	OLED_ShowNum(4, 5,mag_tens,1);
	OLED_ShowString(4, 6, ".");
	OLED_ShowNum(4, 7,mag_uint,1);
	
	OLED_ShowString(4, 8, "V");
	OLED_ShowString(4, 10, "s:");
	
	OLED_ShowNum(4, 12,mag_step_tens,1);
	OLED_ShowString(4, 13, ".");
	OLED_ShowNum(4, 14,mag_step_ones,1);
	OLED_ShowString(4, 15, "V");
	//while(1);
    /*主循环*/
	MainLoop();
}

/************************************
** @func:void TIM2_IRQHandler(void)
** @brief:定时器输出波形数据中断函数
** @parameter:无
** @retval:无
*************************************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{	
		phase_counter = (phase_counter+f_word)%16777216;//取余防止相位累加器的值溢出	
		tlc5615_convert((data_buf[phase_counter>>14]));//取高10位作为波表地址	
	
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除更新事件标志位
	}
}


/************************************
** @func:void WaveSwitchDeal(void)
** @brief:波形切换处理（带固定直流偏置）
** @parameter:无
** @retval:无
*************************************/
void WaveSwitchDeal(void)
{
    uint16_t i;    
    
    switch(wave_slect)
    {
        case 0:  // 输出直流（保持原处理方式）
            for(i = 0; i < 1024; i++){
                data_buf[i] = 1023 * mag_count / MAG_COUNT_MAX;							
            }
            OLED_ShowString(1, 6, "DC ");
            break;
            
        case 1:  // 正弦波（双极性处理）
            for(i = 0; i < 1024; i++){
                /* 转换为双极性处理流程：
                   1. 原始值转有符号（减去中间值512）
                   2. 乘以幅度系数
                   3. 加回固定直流偏置 */
                int32_t temp = (int32_t)sin_wave[i] - 512;  // 转双极性
				if(mag_count <= 13){
					temp = (temp * mag_count)/ MAG_COUNT_MAX;    // 幅度缩放
				}else if((mag_count > 13)&&(mag_count <= 60)){
					temp = (temp * mag_count)/ MAG_COUNT_MAX1;
				}else if((mag_count > 60)&&(mag_count < 90)){
					temp = (temp * mag_count)/ MAG_COUNT_MAX2;
				}else if((mag_count > 89)){
					temp = (temp * mag_count)/ 100;
				}
                
                data_buf[i] = (uint16_t)(temp + DC_OFFSET); // 加固定偏置
            }
            OLED_ShowString(1, 6, "sin");
            break;
            
        case 2:  // 方波（双极性处理）
            for(i = 0; i < 1024; i++){
                int32_t temp = (int32_t)square_wave[i] - 512;
                temp = temp * mag_count / MAG_COUNT_MAX;
                data_buf[i] = (uint16_t)(temp + DC_OFFSET);
            }
            OLED_ShowString(1, 6, "squ");
            break;
            
        case 3:  // 三角波（双极性处理）
            for(i = 0; i < 1024; i++){
                int32_t temp = (int32_t)triangle_wave[i] - 512;		
                temp = temp * mag_count / MAG_COUNT_MAX;
                data_buf[i] = (uint16_t)(temp + DC_OFFSET);
            }
            OLED_ShowString(1, 6, "tri");
            break;
            
        case 4:  // 锯齿波（双极性处理）
            for(i = 0; i < 1024; i++){
                int32_t temp = (int32_t)sawtooth[i] - 512;
                temp = temp * mag_count / MAG_COUNT_MAX;
                data_buf[i] = (uint16_t)(temp + DC_OFFSET);
            }	
            OLED_ShowString(1, 6, "saw");
            break;
        
        default:
            break;
    }
}

/************************************
** @func:MainLoop(void)
** @brief:主循环
** @parameter:无
** @retval:无
*************************************/
void MainLoop(void)
{
	while(1)
	{
		if(magStep_flag == 1)//幅度步进切换
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(20);//延时消抖
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0){
				if(mag_step < 10){
					mag_step = mag_step*10;//
				}else{
					mag_step = 1;
				}			
			}
			//更新显示
			
			mag_step_tens = mag_step/10;
			mag_step_ones = mag_step%10;
			
			OLED_ShowNum(4, 12,mag_step_tens,1);
			OLED_ShowNum(4, 14,mag_step_ones,1);
			
			magStep_flag = 0;
			TIM_Cmd(TIM2, ENABLE);			
		}

		if(waveSwitch_flag == 1)//波形切换
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(20);//延时消抖
			
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0){//再次判断端口电平
				if(wave_slect < 4){
					wave_slect += 1;
				}else{
					wave_slect = 0;
				}				
			}		
			WaveSwitchDeal();//波形切换处理，更新波形数据缓冲区的值和显示的值
			waveSwitch_flag = 0;
			TIM_Cmd(TIM2, ENABLE);			
		}

		if(magAdd_flag == 1)//幅度增加
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(10);//延时消抖
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
			{
				if(mag_step == 1)//判断步进值
				{
					if(mag_count < MAG_OVER_NUM){
						mag_count = mag_count+1;
					}else{
					    mag_count = 0;
					}				
				}
				if(mag_step == 10)//判断步进值
				{
					if(mag_count < MAG_OVER_NUM-10){
						mag_count = mag_count + 10;
					}else{
						mag_count = 0;
					}
				}					
			}

			WaveSwitchDeal();//幅度值处理
			//更新显示
			mag_count_delay = mag_count;
			mag_hundrd = mag_count_delay/100;
			mag_tens = mag_count_delay/10;
			mag_uint = mag_count_delay%10;
			
			OLED_ShowNum(4, 4,mag_hundrd,1);			
			OLED_ShowNum(4, 5,mag_tens,1);
			OLED_ShowNum(4, 7,mag_uint,1);
			magAdd_flag = 0;
			TIM_Cmd(TIM2, ENABLE);					
		}
		
		if(magDec_flag == 1)//幅度减小
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(10);//延时消抖
			//幅度设置
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
			{
				if(mag_step == 1)//判断步进值
				{
					if(mag_count > 0){
						mag_count -= 1;
					}
				}
				if(mag_step == 10)//判断步进值
				{
					if(mag_count > 10){
						mag_count -= 10;
					}	
				}			
			}

			WaveSwitchDeal();//幅度值处理
			//更新显示			
			mag_count_delay = mag_count;
			
			mag_hundrd = mag_count_delay/100;
			mag_tens = mag_count_delay/10;
			mag_uint = mag_count_delay%10;			

			OLED_ShowNum(4, 4,mag_hundrd,1);	
			OLED_ShowNum(4, 5,mag_tens,1);
			OLED_ShowNum(4, 7,mag_uint,1);
			magDec_flag = 0;
			TIM_Cmd(TIM2, ENABLE);					
			
		}			
			
		if(freqStep_flag == 1)//频率步进切换
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(20);//延时消抖
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0){
				if(freq_step < 1000){
					freq_step = freq_step*10;//
				}else{
					freq_step = 1;
				}			
			}

			//更新显示内容
			OLED_ShowNum(3, 4,freq_step,4);
			freqStep_flag = 0;
			TIM_Cmd(TIM2, ENABLE);		
		}
		
		if(freqAdd_flag == 1)//频率增加
		{			
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(10);//延时消抖
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
			{
			    if(freq_step == 1)//判断步进值
				{
					freq_count = (freq_count+1)%10001;//频率加1
					f_word = (freq_count<<8)+FWORD_ADJUST;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字					
				}
				if(freq_step == 10)//判断步进值
				{
					freq_count = (freq_count+10)%10001;//频率加10
					f_word = (freq_count<<8)+FWORD_ADJUST;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字					
				}
				if(freq_step == 100)//判断步进值
				{
					freq_count = (freq_count+100)%10001;//频率加100
					f_word = (freq_count<<8)+FWORD_ADJUST;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字				
				}
				if(freq_step == 1000)//判断步进值
				{
					freq_count = (freq_count+1000)%10001;//频率加1000
					f_word = (freq_count<<8)+FWORD_ADJUST;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字				
				}			
			}

			//更新显示
			
			OLED_ShowNum(2, 4,freq_count,5);
			freqAdd_flag = 0;
			TIM_Cmd(TIM2, ENABLE);					
		}
		
		if(freqDec_flag == 1)//频率减小
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(10);//延时消抖
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
			{
				if(freq_step == 1)//判断步进值
				{
					if(freq_count > 0){
					  freq_count = freq_count - 1;//频率减1
					}else{
					  freq_count = 10000;
					}						
					f_word = (freq_count<<8)+ FWORD_ADJUST;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字					
				}
				if(freq_step == 10)//判断步进值
				{
					if(freq_count > 10){
					  freq_count = freq_count - 10;//频率减10
					}else{
					  freq_count = 10000;
					}	
					f_word = (freq_count<<8)+FWORD_ADJUST;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字					
				}
				if(freq_step == 100)//判断步进值
				{
					if(freq_count > 100){
					  freq_count = freq_count - 100;//频率减100
					}else{
					  freq_count = 10000;
					}	
					f_word = (freq_count<<8)+FWORD_ADJUST;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字				
				}
				if(freq_step == 1000)//判断步进值
				{
					if(freq_count > 1000){
					  freq_count = freq_count - 1000;//频率减1000
					}else{
					  freq_count = 10000;
					}	
					f_word = (freq_count<<8)+FWORD_ADJUST;//除以时钟频率60kHz再乘以相位累加器宽度转换为将输出频率转换为频率控制字				
				}			
			}

			//更新显示
			OLED_ShowNum(2, 4,freq_count,5);
			freqDec_flag = 0;
			TIM_Cmd(TIM2, ENABLE);					
		}		
	}
}



