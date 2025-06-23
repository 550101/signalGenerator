/*
 * Tlc5615.c
 *
 *  Created on: 2024��12��4��
 *      Author: Thorns
 */

#include"Tlc5615.h"


/******************************************************
**@brief    : ��ʼ������DAC��IO��
**@parameter :��
**@retval   : ��
*******************************************************/
void dac_gpio_init(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//����GPIOA��ʱ��
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIOB_InitStructure;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIOB_InitStructure);		
	
	/*����GPIO��ʼ�����Ĭ�ϵ�ƽ*/
	GPIO_SetBits(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);	
}



/******************************************************
**@brief    : ��DAC��������
**@parameter : @dat:��Ҫ���͵�����
**@retval   : ��
*******************************************************/
void tlc5615_convert(uint16_t dat) 
{
    uint8_t i;
    dat <<= 6; // ���� 6 λ��ȷ�� 10 λ���ݶ���
    CS_LOW; // ѡ�� DAC
    SCLK_LOW; // ȷ�� SCLK Ϊ��
    for (i = 0; i < 12; i++) { // ���� 12 λ����
        if (dat & 0x8000)
        { // ��� MSB
            DIN_HIGH; // �� 1
        }
        else
        {
            DIN_LOW; // �� 0
        }
        SCLK_HIGH; // SCLK �ߵ�ƽ
        //__delay_cycles(10); // �ʶ��ӳ���ȷ��ʱ��
        dat <<= 1; // ����
        SCLK_LOW; // SCLK �͵�ƽ
    }
    SCLK_LOW;
    CS_HIGH; // ȡ��ѡ�� DAC
}



