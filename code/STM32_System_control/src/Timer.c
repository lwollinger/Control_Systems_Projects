#include <stdio.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"

TIM_HandleTypeDef htim2;

void Timer_Init(void) {
    __HAL_RCC_TIM2_CLK_ENABLE(); // Ativa o clock do Timer 2

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7199;         // Reduz 72MHz para 10kHz
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 42;              // Conta ate 42 para dar o T = 4.2ms
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        // Erro
    }

    // Ativa a interrupção do Timer
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    // Inicia o Timer com Interrupção
    HAL_TIM_Base_Start_IT(&htim2);
}
