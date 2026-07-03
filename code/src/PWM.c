#include "stdint.h"
#include "PWM.h"
#include "driver/ledc.h" // Biblioteca nativa do ESP-IDF para PWM

// Definições de configuração do PWM - pino GPIO18, escolha o que preferir
#define PWM_TIMER          LEDC_TIMER_0
#define PWM_MODE           LEDC_LOW_SPEED_MODE
#define PWM_CHANNEL        LEDC_CHANNEL_0
#define PWM_OUTPUT_IO      18   // Escolha o pino GPIO que preferir aqui
#define PWM_RESOLUTION     LEDC_TIMER_10_BIT // Resolução de 10 bits (0 a 1023)
#define PWM_FREQUENCY      1000 // 1 kHz

void PWM_Init(void) {
    // 1. Configuração do Timer do PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = PWM_MODE,
        .timer_num        = PWM_TIMER,
        .duty_resolution  = PWM_RESOLUTION,
        .freq_hz          = PWM_FREQUENCY,  
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // 2. Configuração do Canal do PWM ligado ao pino GPIO
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = PWM_MODE,
        .channel        = PWM_CHANNEL,
        .timer_sel      = PWM_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PWM_OUTPUT_IO,
        .duty           = 0, // Inicializa em 0%
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

void Set_PWM_Duty(float duty_cycle) {
    // Saturação de segurança entre 0 e 100%
    if (duty_cycle < 0.0f) duty_cycle = 0.0f;
    if (duty_cycle > 100.0f) duty_cycle = 100.0f;

    // Como a resolução é de 10 bits (máximo 1023):
    // Mapeamos a porcentagem (0-100) para a escala de 0 a 1023
    uint32_t compare_value = (uint32_t)((duty_cycle / 100.0f) * 1023.0f);

    // Atualiza o valor do Duty Cycle no periférico
    ledc_set_duty(PWM_MODE, PWM_CHANNEL, compare_value);
    
    // Aplica a atualização de fato (o ESP32 precisa desse "commit")
    ledc_update_duty(PWM_MODE, PWM_CHANNEL);
}