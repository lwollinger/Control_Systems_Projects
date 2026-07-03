#include "stm32f1xx.h" // Usa apenas as definições de hardware, sem o HAL

void Timer_Init(void) {
    // 1. Liga o Clock do Timer 2 no barramento APB1
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 2. Configura a base de tempo para 4.2ms (Clock 72MHz)
    TIM2->PSC = 71;          // Prescaler: 72MHz / 72 = 1MHz (1µs por tick)
    TIM2->ARR = 4200;        // Auto-Reload: Conta até 4200 (4.2ms)

    // 3. Habilita a interrupção de Update (Estouro do timer) direto no periférico
    TIM2->DIER |= TIM_DIER_UIE;

    // 4. Configura a prioridade e liga a interrupção no núcleo ARM (NVIC)
    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);

    // 5. Dá o "Play" no Timer
    TIM2->CR1 |= TIM_CR1_CEN;
}