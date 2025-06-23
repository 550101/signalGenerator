/*
 * wave_data.h
 *
 *  Created on: 2024��12��4��
 *      Author: Thorns
 */

#ifndef __TLC5615_H
#define __TLC5615_H

#include "stm32f10x.h"                  // Device header
#include "stdint.h"

//DAC Tlc5615���Ŷ���
#define CS_LOW    (GPIO_ResetBits(GPIOB,GPIO_Pin_7 ))
#define CS_HIGH	  (GPIO_SetBits(GPIOB,GPIO_Pin_7))	

#define SCLK_LOW    (GPIO_ResetBits(GPIOB,GPIO_Pin_6))
#define SCLK_HIGH   (GPIO_SetBits(GPIOB,GPIO_Pin_6))	

#define DIN_LOW     (GPIO_ResetBits(GPIOB,GPIO_Pin_5))
#define DIN_HIGH    (GPIO_SetBits(GPIOB,GPIO_Pin_5))	

//TLC5615  ��������

void dac_gpio_init(void);
void tlc5615_convert(uint16_t dat);


#endif
