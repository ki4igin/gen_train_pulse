#ifndef __tim_H
#define __tim_H

#include "main.h"
#include "stm32l4xx_ll_tim.h"

#define TIM3_PERIOD (16000)

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM6_Init(void);

#endif
