#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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

static volatile float dbg_y;
static volatile float dbg_e;
static volatile float dbg_u;
static volatile float dbg_sp;

static float y_adc = 0.0f;

static uint32_t contador_sp = 0;

void de_controle_callback(void *arg)
{

    // Média de 8 amostras
    // for(int i = 0; i < 16; i++)
    // {
    //     y_adc += ADC_GetValue();
    // }

    // y_adc /= 16.0f;
    gpio_set_level(PINO_FLAG_FREQ, 1);

    y_adc = ADC_GetValue();
    // Troca SP a cada ~500 ms
    contador_sp++;

    if(contador_sp >= 50)   // 120 * 4.2 ms ≈ 100 ms
    {
        contador_sp = 0;

        if(cntr.setpoint < 1.25f){
            cntr.setpoint = 1.5f;
            // Definir pino nível lógico alto 
            gpio_set_level(PINO_TRIGGER, 1);
        } else {
            cntr.setpoint = 1.0f;
            // Definir pino nível lógico baixo
            gpio_set_level(PINO_TRIGGER, 0);
        }
    }

    float u = Controller_Compute(&cntr, y_adc);

    float duty = (cntr.setpoint/ 3.3f) * 100.0f;

    if(duty < 0.0f)
        duty = 0.0f;

    if(duty > 100.0f)
        duty = 100.0f;

    Set_PWM_Duty(duty);

    dbg_sp = cntr.setpoint;
    dbg_y  = y_adc;
    dbg_e  = cntr.erro[0];
    dbg_u  = u;
    gpio_set_level(PINO_FLAG_FREQ, 0);
}

void app_main(void)
{
    gpio_reset_pin(PINO_TRIGGER);
    gpio_set_direction(PINO_TRIGGER, GPIO_MODE_OUTPUT);
    gpio_set_level(PINO_TRIGGER, 0);

    gpio_reset_pin(PINO_FLAG_FREQ);
    gpio_set_direction(PINO_FLAG_FREQ, GPIO_MODE_OUTPUT);
    gpio_set_level(PINO_FLAG_FREQ, 0);

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

    esp_timer_start_periodic(periodic_timer, 4200);

    while(1)
    {
        printf(
            "SP=%.1f y=%.3f e=%.3f u=%.3f\n",
            dbg_sp,
            dbg_y,
            dbg_e,
            dbg_u
        );

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}