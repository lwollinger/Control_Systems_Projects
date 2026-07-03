#include "ADC.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

// CONFIGURAÇÕES DO ADC
// ADC1_CHANNEL_0 corresponde ao pino GPIO36 (Pino VP) no ESP32 clássico.
// Altere o canal conforme o pino físico que você deseja usar.
#define ADC_CHANNEL         ADC_CHANNEL_0 
#define ADC_UNIT            ADC_UNIT_1

// Atenuação define a faixa de tensão de entrada. 
// ADC_ATTEN_DB_11 ou DB_12 permite ler a faixa completa de 0V até ~3.3V
#define ADC_ATTEN           ADC_ATTEN_DB_11 

static adc_oneshot_unit_handle_t adc_handle;

void ADC_Init(void) {
    // 1. Configuração da Unidade ADC
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // 2. Configuração do Canal do ADC
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, // Resolução padrão de 12 bits (0 a 4095)
        .atten = ADC_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, ADC_CHANNEL, &config));
}

float ADC_GetValue(void)
{
    int raw;

    if(adc_oneshot_read(adc_handle, ADC_CHANNEL, &raw) == ESP_OK)
    {
        return (3.3f * raw) / 4095.0f;
    }

    return 0.0f;
}