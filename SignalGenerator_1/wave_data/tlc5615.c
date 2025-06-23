/*
 * Tlc5615.c
 *
 *  Created on: 2024年12月4日
 *      Author: Thorns
 */

#include"Tlc5615.h"


/******************************************************
**@brief    : 初始化连接DAC的IO口
**@parameter :无
**@retval   : 无
*******************************************************/
void dac_gpio_init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOA的时钟
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIOB_InitStructure;
	GPIOB_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIOB_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIOB_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIOB_InitStructure);		
	
	/*设置GPIO初始化后的默认电平*/
	GPIO_SetBits(GPIOB, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);	
}



/******************************************************
**@brief    : 向DAC发送数据
**@parameter : @dat:需要发送的数据
**@retval   : 无
*******************************************************/
void tlc5615_convert(uint16_t dat) 
{
    uint8_t i;
    dat <<= 6; // 左移 6 位，确保 10 位数据对齐
    CS_LOW; // 选择 DAC
    SCLK_LOW; // 确保 SCLK 为低
    for (i = 0; i < 12; i++) { // 发送 12 位数据
        if (dat & 0x8000)
        { // 检查 MSB
            DIN_HIGH; // 置 1
        }
        else
        {
            DIN_LOW; // 置 0
        }
        SCLK_HIGH; // SCLK 高电平
        //__delay_cycles(10); // 适度延迟以确保时序
        dat <<= 1; // 左移
        SCLK_LOW; // SCLK 低电平
    }
    SCLK_LOW;
    CS_HIGH; // 取消选择 DAC
}



