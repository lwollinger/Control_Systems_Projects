#ifndef PWM_H
#define PWM_H

// Funções para inicialização e controle do PWM
void PWM_Init(void);
void PWM_SetDutyCycle(uint8_t channel, uint16_t duty_cycle);

#endif // PWM_H