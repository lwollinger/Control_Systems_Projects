#ifndef PWM_H
#define PWM_H

#include "PWM.h"
#include "stm32f1xx_hal.h"

TIM_HandleTypeDef htim3;

void PWM_Init(){
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Supondo PA6 (TIM3_CH1)

    // Configuração do Pino GPIO para PWM
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // Alternate Function Push-Pull
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configuração do Timer para PWM (ex: 20kHz para não apitar)
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 71;           // 72MHz / 72 = 1MHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 1000;            // 1MHz / 1000 = 1kHz de frequência PWM
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;                 // Começa com 0% de duty cycle
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void Set_PWM_Duty(float duty_cycle){
    // Saturação de segurança: garante que o duty esteja entre 0 e 100%
    if (duty_cycle < 0.0f) duty_cycle = 0.0f;
    if (duty_cycle > 100.0f) duty_cycle = 100.0f;

    // Converte a porcentagem (0-100) para o valor do registrador CCR (0-1000)
    // Se o seu Period (ARR) é 1000, entao 100% = 1000.
    uint32_t pulse = (uint32_t)(duty_cycle * 10.0f); 

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);
}

#endif // PWM_H