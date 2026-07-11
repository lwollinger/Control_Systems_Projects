#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_pm.h"
#include "esp_private/esp_clk.h"
#include "soc/rtc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "driver/gpio.h"

#include "Controller_CFG.h"
#include "ADC.h"
#include "PWM.h"

#define PINO_TRIGGER GPIO_NUM_21
#define PINO_FLAG_FREQ GPIO_NUM_22

Controller_t cntr;


static volatile float dbg_e;
static volatile float dbg_u;
static volatile float dbg_sp;

static float y_adc = 0.0f;

// Contador para mudar o setpoint
static uint32_t contador_sp = 0;

void de_controle_callback(void *arg)
{
    // Liga o pino de flag (medir o tempo de execução no osciloscópio)
    gpio_set_level(PINO_FLAG_FREQ, 1);
    // 1. Leitura do sensor pelo ADC usando DMA
    uint16_t raw = ADC_GetRaw();
    gpio_set_level(PINO_FLAG_FREQ, 0);

    y_adc = raw * (3.3f / 4095.0f);

    // 2. Alternância do Setpoint
    // Como o timer roda a cada T = 100us, 5000 iterações = 500ms (0.5 segundos)
    contador_sp++;
    if(contador_sp >= 5000)   
    {
        contador_sp = 0;

        if(cntr.setpoint < 1.25f){
            cntr.setpoint = 1.5f;
            gpio_set_level(PINO_TRIGGER, 1);
        } else {
            cntr.setpoint = 1.0f;
            gpio_set_level(PINO_TRIGGER, 0);
        }
    }

    // 3. Execução do cálculo do controlador (Espaço de Estados)
    float u = Controller_Compute(&cntr, y_adc);

    // 5. Atuação no PWM
    //Set_PWM_Duty((u / 3.3f) * 100.0f);
    Set_PWM_Duty(50.0f);
    // Desliga o pino de flag
    //gpio_set_level(PINO_FLAG_FREQ, 0);
}

void app_main(void)
{

    // =========================================================================
    // CONFIGURAÇÃO PARA FORÇAR O PROCESSADOR EM MÁXIMA PERFORMANCE (240 MHz)
    // =========================================================================
    #if CONFIG_IDF_TARGET_ESP32
    rtc_cpu_freq_config_t clock_config;
    rtc_clk_cpu_freq_get_config(&clock_config);
    if (clock_config.freq_mhz != 240) {
        rtc_clk_cpu_freq_set_config_fast(&clock_config);
    }
    #endif
    // =========================================================================

    gpio_reset_pin(PINO_TRIGGER);
    gpio_set_direction(PINO_TRIGGER, GPIO_MODE_OUTPUT);
    gpio_set_level(PINO_TRIGGER, 0);

    gpio_reset_pin(PINO_FLAG_FREQ);
    gpio_set_direction(PINO_FLAG_FREQ, GPIO_MODE_OUTPUT);
    gpio_set_level(PINO_FLAG_FREQ, 0);

    // Inicializa a estrutura com os novos vetores K e Ke
    Controller_Init(&cntr);
    printf("Controller OK\n");

    ADC_Init();
    printf("ADC OK\n");

    PWM_Init();
    printf("PWM OK\n");

    const esp_timer_create_args_t periodic_timer_args =
    {
        .callback = &de_controle_callback,
        .name = "control_loop"
    };

    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);

    // microssegundos para 100 microssegundos (0.0001s)
    esp_timer_start_periodic(periodic_timer, 100);

    while(1)
    {
        // Printf agora mostrará os dados reais calculados na interrupção
        // printf(
        //     "SP=%.1f y=%.3f e=%.3f u=%.3f\n",
        //     dbg_sp,
        //     dbg_y,
        //     dbg_e,
        //     dbg_u
        // );

        printf("y = %.3f V\n", y_adc);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}