#include <stdio.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "Controller_CFG.h"
#include "ADC.h"
#include "PWM.h"
#include "Timer.h"

//void SystemClock_Config(void);

Controller_t cntr;
float y = 0.0f; // Variável global para armazenar a saída medida da planta (y[n])
float u = 0.0f; // Variável global para armazenar a saída do controlador (u[n])
float duty = 0.0f; // Variável global para armazenar o duty cycle calculado

int main(void) {
    HAL_Init();
    //SystemClock_Config();
    Controller_Init(&cntr); 
    ADC_Init();
    PWM_Init();
    Timer_Init();

    while (1) {

    }
}

// Leitura do ADC e acionamento do controle a cada 4.4ms
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        y = Read_ADC(); 
        
        // Nota: para usar 'cntr' aqui, ela teria que ser global ou static
        u = Controller_Compute(&cntr, y); 
        
        // 3. ATUAR
        duty = (u / 3.3f) * 100.0f; 
        Set_PWM_Duty(duty);
    }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  // Configura o multiplicador de clock (PLL) para chegar a 72MHz
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    while(1); // Erro na configuração
  }

  // Configura os barramentos (HCLK, PCLK1, PCLK2)
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    while(1); // Erro na configuração
  }
}