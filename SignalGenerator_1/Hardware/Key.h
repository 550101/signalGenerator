#ifndef __KEY_H
#define __KEY_H
#include "stm32f10x.h"                  // Device header

//
void Key_Init(void);
void EC11_1_KeyInit(void);
void EC11_1_Init(void);
void EC11_2_Init(void);
void GpioSyncInit(void);
uint8_t Key_GetNum(void);

//按键外部中断初始化
void key_exti_init(void);


#endif
