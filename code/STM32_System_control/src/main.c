#include <stdio.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "Controller_CFG.h"
#include "ADC.h"
#include "PWM.h"
#include "Timer.h"

void SystemClock_Config(void);

// Variáveis de comunicação entre Interrupção e Loop Principal
volatile uint32_t y_raw = 0;
volatile uint8_t controle_pronto = 0;

Controller_t cntr;
float y = 0.0f; // Variável global para armazenar a saída medida da planta (y[n])
float u = 0.0f; // Variável global para armazenar a saída do controlador (u[n])
float duty = 0.0f; 

int main(void) {
    HAL_Init();
    SystemClock_Config(); 

    __HAL_RCC_ADC_CONFIG(RCC_ADCPCLK2_DIV6);

    Controller_Init(&cntr);
    ADC_Init();
    PWM_Init();

    Timer_Init();

   while (1) {
    // Quando a interrupção avisa que o tempo de 4.2ms estourou...
    if (controle_pronto) {
        
        // 1. Converte o valor bruto para a tensão real (y) em float
        y = ((float)y_raw * 3.3f) / 4095.0f;

        // 2. Calcula a lei de controle usando floats com total segurança
        u = Controller_Compute(&cntr, y); 
        
        // 3. Modula o Duty Cycle do PWM baseado na saída do controle
        duty = (u / 3.3f) * 100.0f;
        

        // 4. Atualiza o atuador
        Set_PWM_Duty(duty);

        // Abaixa a bandeira e espera os próximos 4.2ms
        controle_pronto = 0;
    }
}
}

// void TIM2_IRQHandler(void) {
//     // Confirma se foi o evento de Update (estouro) que chamou a interrupção
//     if (TIM2->SR & TIM_SR_UIF) {
        
//         // Limpa a flag (Desliga o bit UIF)
//         TIM2->SR &= ~TIM_SR_UIF; 

//         // Sincroniza o barramento do clone
//         __IO uint32_t dummy = TIM2->SR;
//         (void)dummy;

//         // ---- AÇÃO DISCRETA (A cada 4.2ms) ----
//         // ---- AÇÃO DISCRETA (A cada 4.2ms) ----
//         y = Read_ADC(); 
//         //u = Controller_Compute(&cntr, y); 
//         //duty = (u / 3.3f) * 100.0f; 
//         Set_PWM_Duty(50.0f); // Para teste, força 50% de duty cycle
//     }
// }


// void TIM2_IRQHandler(void) {
//     if (TIM2->SR & TIM_SR_UIF) {
//         TIM2->SR &= ~TIM_SR_UIF; // Limpa flag
//         __IO uint32_t dummy = TIM2->SR; (void)dummy;

//         // EXECUTA A LEITURA PURAMENTE INTEIRA
//         y_raw = Read_ADC_Raw(); 
//         u = Controller_Compute(&cntr, y_raw); 
//         duty = (u / 3.3f) * 100.0f;
//         Set_PWM_Duty(duty); 
//     }
// }

// Alternativa pra placa chinesa vagabunda
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF; // Limpa flag
        __IO uint32_t dummy = TIM2->SR; (void)dummy;

        // 1. APENAS lê o valor inteiro do ADC (Sem floats aqui!)
        y_raw = Read_ADC_Raw(); 

        // 2. Levanta a bandeira avisando o while(1) que deu 4.2ms
        controle_pronto = 1; 
    }
}


void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}