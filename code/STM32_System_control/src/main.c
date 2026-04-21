#include <stdio.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "Controller_CFG.h"
#include "ADC.h"
#include "PWM.h"

void SystemClock_Config(void);

float y = 0.0f; // Variável global para armazenar a saída medida da planta (y[n])
float u = 0.0f; // Variável global para armazenar a saída do controlador (u[n])
float duty = 0.0f; // Variável global para armazenar o duty cycle calculado

int main(void) {
	HAL_Init(); // Inicializa a HAL
	SystemClock_Config(); // Configura o clock do sistema (geralmente 72MHz para STM32F103)
	Controller_t cntr;
	Controller_Init(&cntr); // Configura os coeficientes do controlador e zera os históricos
	ADC_Init();
	PWM_Init();
    
	
    u = Controller_Compute(&cntr, y);
	duty = (u / 3.3f) * 100.0f; 

	Set_PWM_Duty(duty);

	return 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        // 1. LER: Busca o valor da planta no ADC
        y = Read_ADC(); 
    }
}