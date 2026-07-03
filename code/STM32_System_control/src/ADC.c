#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "ADC.h"


ADC_HandleTypeDef hadc1;

void ADC_Init(void){

    // Ativa os Clocks necessários
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Configura o pino PA0 como entrada analógica
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE; // <--- IMPORTANTE: Modo contínuo desligado
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // Início via software (manual)
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        // Erro de inicialização
    }

    // Configuração do Canal (Ex: Canal 0)
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5; // Tempo de amostragem curto para controle
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        // Erro de configuração
    }

}

// float Read_ADC(void) {
//     // 1. Dispara a conversão do ADC1 direto no registrador (Bit SWSTART)
//     ADC1->CR2 |= ADC_CR2_SWSTART;
    
//     // 2. Espera a conversão acabar (Bit EOC - End of Conversion)
//     // Isso leva apenas cerca de 1.5 a 2 microssegundos, sem travar o chip!
//     while (!(ADC1->SR & ADC_SR_EOC)) {
//         // Espera o hardware processar a leitura analógica
//     }
    
//     // 3. Pega o valor digitalizado de 12 bits (0 a 4095)
//     // Ler o registrador DR limpa automaticamente a flag EOC no hardware
//     uint32_t raw_value = ADC1->DR;

//     // 4. Converte e retorna os Volts (0V a 3.3V)
//     return ((float)raw_value * 3.3f) / 4095.0f;
// }

// float Read_ADC(void) {
//     // 1. Garante que o ADC1 está ligado (Bit ADON)
//     if (!(ADC1->CR2 & ADC_CR2_ADON)) {
//         ADC1->CR2 |= ADC_CR2_ADON;
//     }

//     // 2. Dispara a conversão
//     ADC1->CR2 |= ADC_CR2_SWSTART;
    
//     // 3. Loop com contador de segurança (Anti-travamento)
//     uint32_t timeout = 1000;
//     while (!(ADC1->SR & ADC_SR_EOC)) {
//         timeout--;
//         if (timeout == 0) {
//             // Se o hardware do clone travar, saímos do loop para não matar o PWM
//             return 0.0f; 
//         }
//     }
    
//     // 4. Lê o valor e limpa a flag
//     uint32_t raw_value = ADC1->DR;

//     return ((float)raw_value * 3.3f) / 4095.0f;
// }

// Altere o tipo de retorno de float para uint32_t
uint32_t Read_ADC_Raw(void) {
    // Liga o ADC se estiver desligado
    if (!(ADC1->CR2 & ADC_CR2_ADON)) {
        ADC1->CR2 |= ADC_CR2_ADON;
    }

    // Dispara a conversão
    ADC1->CR2 |= ADC_CR2_SWSTART;
    
    // Timeout ultra-curto apenas para o teste
    uint32_t timeout = 500;
    while (!(ADC1->SR & ADC_SR_EOC)) {
        timeout--;
        if (timeout == 0) {
            return 0; // Se falhar, retorna zero imediatamente
        }
    }
    
    // Retorna o valor bruto (0 a 4095) sem fazer nenhuma conta de float!
    return ADC1->DR;
}