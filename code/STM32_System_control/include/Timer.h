#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f1xx_hal.h"

// Avisa a todos os arquivos que incluírem o Timer.h que a htim2 existe
extern TIM_HandleTypeDef htim2; 

void Timer_Init(void);

#endif /* __TIMER_H */