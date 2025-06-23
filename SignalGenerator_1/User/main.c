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

volatile uint16_t data_buf[1024] = {0};//�������ݻ�����

uint16_t voltage_Ajust[20] = {95,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const uint16_t DC_OFFSET = 512;  // �̶���ֱ��ƫ�ã��м�ֵ��

//���ñ�־
extern uint8_t waveSwitch_flag;
extern uint8_t freqAdd_flag;
extern uint8_t freqDec_flag;
extern uint8_t freqStep_flag;
extern uint8_t magAdd_flag; 
extern uint8_t magDec_flag;
extern uint8_t magStep_flag; 
extern uint8_t dutySet_flag;
//��������
void WaveSwitchDeal(void);//�����л�����
void MainLoop(void);//��ѭ��
//void DutySet(void);//���β�ռ�ձ�����

//��ز�������
volatile uint16_t mag_count = 0;	 //������ֵ
volatile uint16_t mag_count_delay = 0;	 //������ʾ��ֵ

volatile uint16_t mag_hundrd = 0;//���Ȱ�λ�ϵ�ֵ
volatile uint16_t mag_tens = 0;//����ʮλ�ϵ�ֵ
volatile uint16_t mag_uint = 0;//���ȸ�λ�ϵ�ֵ


volatile uint16_t mag_step = 1;      //���Ȳ�������ʼ��Ϊ0.1V
volatile uint16_t mag_step_tens = 0;
volatile uint16_t mag_step_ones = 1;
volatile uint8_t wave_slect = 0;     //����ѡ��
volatile uint32_t phase_counter = 0; //��λ�ۼ���
volatile uint32_t f_word = 0;        //Ƶ�ʿ�����
volatile uint16_t freq_count = 0;	//Ƶ����ֵ
volatile uint16_t freq_step = 1;     //Ƶ�ʲ�������ʼ��Ϊ1Hz


              
/************************************
*
*	main����
*
*************************************/
int main(void)
{
	/*ģ���ʼ��*/	
	OLED_Init();
	dac_gpio_init();
	Key_Init();
	EC11_1_KeyInit();
	EC11_1_Init();
	EC11_2_Init();
	tim_fclk_init();
	
	
	OLED_ShowString(1, 1, "wave:");
	WaveSwitchDeal();
	OLED_ShowString(1, 6, "DC ");//Ĭ�����ֱ��
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
    /*��ѭ��*/
	MainLoop();
}

/************************************
** @func:void TIM2_IRQHandler(void)
** @brief:��ʱ��������������жϺ���
** @parameter:��
** @retval:��
*************************************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
	{	
		phase_counter = (phase_counter+f_word)%16777216;//ȡ���ֹ��λ�ۼ�����ֵ���	
		tlc5615_convert((data_buf[phase_counter>>14]));//ȡ��10λ��Ϊ�����ַ	
	
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//��������¼���־λ
	}
}


/************************************
** @func:void WaveSwitchDeal(void)
** @brief:�����л��������̶�ֱ��ƫ�ã�
** @parameter:��
** @retval:��
*************************************/
void WaveSwitchDeal(void)
{
    uint16_t i;    
    
    switch(wave_slect)
    {
        case 0:  // ���ֱ��������ԭ����ʽ��
            for(i = 0; i < 1024; i++){
                data_buf[i] = 1023 * mag_count / MAG_COUNT_MAX;							
            }
            OLED_ShowString(1, 6, "DC ");
            break;
            
        case 1:  // ���Ҳ���˫���Դ���
            for(i = 0; i < 1024; i++){
                /* ת��Ϊ˫���Դ������̣�
                   1. ԭʼֵת�з��ţ���ȥ�м�ֵ512��
                   2. ���Է���ϵ��
                   3. �ӻع̶�ֱ��ƫ�� */
                int32_t temp = (int32_t)sin_wave[i] - 512;  // ת˫����
				if(mag_count <= 13){
					temp = (temp * mag_count)/ MAG_COUNT_MAX;    // ��������
				}else if((mag_count > 13)&&(mag_count <= 60)){
					temp = (temp * mag_count)/ MAG_COUNT_MAX1;
				}else if((mag_count > 60)&&(mag_count < 90)){
					temp = (temp * mag_count)/ MAG_COUNT_MAX2;
				}else if((mag_count > 89)){
					temp = (temp * mag_count)/ 100;
				}
                
                data_buf[i] = (uint16_t)(temp + DC_OFFSET); // �ӹ̶�ƫ��
            }
            OLED_ShowString(1, 6, "sin");
            break;
            
        case 2:  // ������˫���Դ���
            for(i = 0; i < 1024; i++){
                int32_t temp = (int32_t)square_wave[i] - 512;
                temp = temp * mag_count / MAG_COUNT_MAX;
                data_buf[i] = (uint16_t)(temp + DC_OFFSET);
            }
            OLED_ShowString(1, 6, "squ");
            break;
            
        case 3:  // ���ǲ���˫���Դ���
            for(i = 0; i < 1024; i++){
                int32_t temp = (int32_t)triangle_wave[i] - 512;		
                temp = temp * mag_count / MAG_COUNT_MAX;
                data_buf[i] = (uint16_t)(temp + DC_OFFSET);
            }
            OLED_ShowString(1, 6, "tri");
            break;
            
        case 4:  // ��ݲ���˫���Դ���
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
** @brief:��ѭ��
** @parameter:��
** @retval:��
*************************************/
void MainLoop(void)
{
	while(1)
	{
		if(magStep_flag == 1)//���Ȳ����л�
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(20);//��ʱ����
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0){
				if(mag_step < 10){
					mag_step = mag_step*10;//
				}else{
					mag_step = 1;
				}			
			}
			//������ʾ
			
			mag_step_tens = mag_step/10;
			mag_step_ones = mag_step%10;
			
			OLED_ShowNum(4, 12,mag_step_tens,1);
			OLED_ShowNum(4, 14,mag_step_ones,1);
			
			magStep_flag = 0;
			TIM_Cmd(TIM2, ENABLE);			
		}

		if(waveSwitch_flag == 1)//�����л�
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(20);//��ʱ����
			
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 0){//�ٴ��ж϶˿ڵ�ƽ
				if(wave_slect < 4){
					wave_slect += 1;
				}else{
					wave_slect = 0;
				}				
			}		
			WaveSwitchDeal();//�����л��������²������ݻ�������ֵ����ʾ��ֵ
			waveSwitch_flag = 0;
			TIM_Cmd(TIM2, ENABLE);			
		}

		if(magAdd_flag == 1)//��������
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(10);//��ʱ����
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
			{
				if(mag_step == 1)//�жϲ���ֵ
				{
					if(mag_count < MAG_OVER_NUM){
						mag_count = mag_count+1;
					}else{
					    mag_count = 0;
					}				
				}
				if(mag_step == 10)//�жϲ���ֵ
				{
					if(mag_count < MAG_OVER_NUM-10){
						mag_count = mag_count + 10;
					}else{
						mag_count = 0;
					}
				}					
			}

			WaveSwitchDeal();//����ֵ����
			//������ʾ
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
		
		if(magDec_flag == 1)//���ȼ�С
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(10);//��ʱ����
			//��������
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 0)
			{
				if(mag_step == 1)//�жϲ���ֵ
				{
					if(mag_count > 0){
						mag_count -= 1;
					}
				}
				if(mag_step == 10)//�жϲ���ֵ
				{
					if(mag_count > 10){
						mag_count -= 10;
					}	
				}			
			}

			WaveSwitchDeal();//����ֵ����
			//������ʾ			
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
			
		if(freqStep_flag == 1)//Ƶ�ʲ����л�
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(20);//��ʱ����
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0){
				if(freq_step < 1000){
					freq_step = freq_step*10;//
				}else{
					freq_step = 1;
				}			
			}

			//������ʾ����
			OLED_ShowNum(3, 4,freq_step,4);
			freqStep_flag = 0;
			TIM_Cmd(TIM2, ENABLE);		
		}
		
		if(freqAdd_flag == 1)//Ƶ������
		{			
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(10);//��ʱ����
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)
			{
			    if(freq_step == 1)//�жϲ���ֵ
				{
					freq_count = (freq_count+1)%10001;//Ƶ�ʼ�1
					f_word = (freq_count<<8)+FWORD_ADJUST;//����ʱ��Ƶ��60kHz�ٳ�����λ�ۼ������ת��Ϊ�����Ƶ��ת��ΪƵ�ʿ�����					
				}
				if(freq_step == 10)//�жϲ���ֵ
				{
					freq_count = (freq_count+10)%10001;//Ƶ�ʼ�10
					f_word = (freq_count<<8)+FWORD_ADJUST;//����ʱ��Ƶ��60kHz�ٳ�����λ�ۼ������ת��Ϊ�����Ƶ��ת��ΪƵ�ʿ�����					
				}
				if(freq_step == 100)//�жϲ���ֵ
				{
					freq_count = (freq_count+100)%10001;//Ƶ�ʼ�100
					f_word = (freq_count<<8)+FWORD_ADJUST;//����ʱ��Ƶ��60kHz�ٳ�����λ�ۼ������ת��Ϊ�����Ƶ��ת��ΪƵ�ʿ�����				
				}
				if(freq_step == 1000)//�жϲ���ֵ
				{
					freq_count = (freq_count+1000)%10001;//Ƶ�ʼ�1000
					f_word = (freq_count<<8)+FWORD_ADJUST;//����ʱ��Ƶ��60kHz�ٳ�����λ�ۼ������ת��Ϊ�����Ƶ��ת��ΪƵ�ʿ�����				
				}			
			}

			//������ʾ
			
			OLED_ShowNum(2, 4,freq_count,5);
			freqAdd_flag = 0;
			TIM_Cmd(TIM2, ENABLE);					
		}
		
		if(freqDec_flag == 1)//Ƶ�ʼ�С
		{
			TIM_Cmd(TIM2, DISABLE);
			Delay_ms(10);//��ʱ����
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
			{
				if(freq_step == 1)//�жϲ���ֵ
				{
					if(freq_count > 0){
					  freq_count = freq_count - 1;//Ƶ�ʼ�1
					}else{
					  freq_count = 10000;
					}						
					f_word = (freq_count<<8)+ FWORD_ADJUST;//����ʱ��Ƶ��60kHz�ٳ�����λ�ۼ������ת��Ϊ�����Ƶ��ת��ΪƵ�ʿ�����					
				}
				if(freq_step == 10)//�жϲ���ֵ
				{
					if(freq_count > 10){
					  freq_count = freq_count - 10;//Ƶ�ʼ�10
					}else{
					  freq_count = 10000;
					}	
					f_word = (freq_count<<8)+FWORD_ADJUST;//����ʱ��Ƶ��60kHz�ٳ�����λ�ۼ������ת��Ϊ�����Ƶ��ת��ΪƵ�ʿ�����					
				}
				if(freq_step == 100)//�жϲ���ֵ
				{
					if(freq_count > 100){
					  freq_count = freq_count - 100;//Ƶ�ʼ�100
					}else{
					  freq_count = 10000;
					}	
					f_word = (freq_count<<8)+FWORD_ADJUST;//����ʱ��Ƶ��60kHz�ٳ�����λ�ۼ������ת��Ϊ�����Ƶ��ת��ΪƵ�ʿ�����				
				}
				if(freq_step == 1000)//�жϲ���ֵ
				{
					if(freq_count > 1000){
					  freq_count = freq_count - 1000;//Ƶ�ʼ�1000
					}else{
					  freq_count = 10000;
					}	
					f_word = (freq_count<<8)+FWORD_ADJUST;//����ʱ��Ƶ��60kHz�ٳ�����λ�ۼ������ת��Ϊ�����Ƶ��ת��ΪƵ�ʿ�����				
				}			
			}

			//������ʾ
			OLED_ShowNum(2, 4,freq_count,5);
			freqDec_flag = 0;
			TIM_Cmd(TIM2, ENABLE);					
		}		
	}
}



