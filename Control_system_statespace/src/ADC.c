#include "ADC.h"
#include "esp_adc/adc_continuous.h"
#include "esp_attr.h"

#define ADC_UNIT             ADC_UNIT_1
#define ADC_CHANNEL          ADC_CHANNEL_0
#define ADC_ATTEN            ADC_ATTEN_DB_12

// Aumentamos o frame para ler múltiplos dados de uma vez se necessário
#define ADC_CONV_FRAME_SIZE  (4 * SOC_ADC_DIGI_DATA_BYTES_PER_CONV) 

static adc_continuous_handle_t adc_handle = NULL;

void ADC_Init(void)
{
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = 1024,          
        .conv_frame_size = ADC_CONV_FRAME_SIZE, 
    };
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_config, &adc_handle));

    adc_continuous_config_t config = {
        .pattern_num = 1,
        .sample_freq_hz = 20000,             // Baixamos para 20 kHz (mínimo do hardware do ESP32)
        .conv_mode = ADC_CONV_SINGLE_UNIT_1, 
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
    };

    adc_digi_pattern_config_t adc_pattern = {
        .atten = ADC_ATTEN,
        .channel = ADC_CHANNEL,
        .unit = ADC_UNIT,
        .bit_width = SOC_ADC_DIGI_MAX_BITWIDTH, 
    };
    config.adc_pattern = &adc_pattern;

    ESP_ERROR_CHECK(adc_continuous_config(adc_handle, &config));
    ESP_ERROR_CHECK(adc_continuous_start(adc_handle));
}

// Agora sim: Rápido e imune a estouro de buffer
IRAM_ATTR uint16_t ADC_GetRaw(void)
{
    // Criamos um array para puxar várias amostras acumuladas de uma vez só
    adc_digi_output_data_t out_data[8];
    uint32_t ret_num = 0;
    
    // Puxa tudo o que estiver acumulado no DMA para esvaziar o buffer e evitar o erro de Overflow
    esp_err_t ret = adc_continuous_read(adc_handle, (uint8_t *)out_data, sizeof(out_data), &ret_num, 0);
    
    // Se a leitura foi um sucesso e vieram dados...
    if (ret == ESP_OK && ret_num > 0) {
        // Calculamos quantas amostras válidas vieram dentro do pacote
        uint32_t num_samples = ret_num / SOC_ADC_DIGI_DATA_BYTES_PER_CONV;
        // Pegamos estritamente a ÚLTIMA amostra (índice num_samples - 1), que é o dado mais atualizado do sensor
        return (uint16_t)out_data[num_samples - 1].type1.data;
    }
    
    // // Se der erro de estouro ou buffer vazio, limpa o driver rapidamente em background para o próximo ciclo
    // if (ret == ESP_ERR_INVALID_STATE) {
    //     adc_continuous_flush(adc_handle);
    // }
    
    return 0; 
}