#include "stm32f1xx_hal.h"
#include "PWM.h"

TIM_HandleTypeDef htim3;

void PWM_Init(void) {
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Pino PA6 (TIM3_CH1)

    // Configuração do Pino GPIO para Função Alternativa (PWM)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Configuração do Timer para PWM (Frequência = 1kHz)
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 71;           // 72MHz / 72 = 1MHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 1000;            // ARR = 1000 (1MHz / 1000 = 1kHz)
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;                 // Começa com 0% para segurança
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

    // Garante o início do sinal físico de PWM
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void Set_PWM_Duty(float duty_cycle) {
    // Saturação de segurança entre 0 e 100%
    if (duty_cycle < 0.0f) duty_cycle = 0.0f;
    if (duty_cycle > 100.0f) duty_cycle = 100.0f;

    // Como o Period (ARR) é 1000:
    // 0% de duty = 0 ticks
    // 100% de duty = 1000 ticks -> (duty_cycle * 10.0f)
    uint32_t pulse = (uint32_t)(duty_cycle * 10.0f); 

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);
}