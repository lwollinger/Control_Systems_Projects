#ifndef PWM_H
#define PWM_H

#include "stm32f1xx_hal.h"

// Protótipos das funções
void PWM_Init(void);
void Set_PWM_Duty(float duty_cycle);

#endif // PWM_H