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
    hadc1.Init.ContinuousConvMode = DISABLE; 
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START; // Início via software (manual)
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        // Erro de inicialização
    }

    // Configuração do Canal 
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5; // Tempo de amostragem curto para controle
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        // Erro de configuração
    }

}

float Read_ADC(void) {
    uint32_t raw_value = 0;
    
    HAL_ADC_Start(&hadc1); 
    
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        raw_value = HAL_ADC_GetValue(&hadc1);
    }
    
    HAL_ADC_Stop(&hadc1); 

    // Converte os 12 bits (0-4095) para Volts (0-3.3V)
    return (raw_value * 3.3f) / 4095.0f;
}