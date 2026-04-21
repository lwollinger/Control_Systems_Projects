#ifndef CONTROLLER_CFG_H
#define CONTROLLER_CFG_H

/**
 * Estrutura que armazena os coeficientes e o estado do controlador.
 * 
 *
 *          DIAGRAMA DE BLOCOS DO SISTEMA DE CONTROLE
 * -----------------------------------------------------------------------------------
 * |                                                                                 |
 * |    Ref (setpoint)       Erro          U (acao_controle)          Y (saida)      |
 * |    ---->(+)-----------[ Controlador ]--------[ Planta ]--------+----->          |
 * |         ^ -          (C)                  (G)            |                      |
 * |         |                                                |                      |
 * |         -------------------( Realimentacao )-------------+                      |
 * |                                                                                 |
 * -----------------------------------------------------------------------------------
 * Onde:
 * - Ref: Sinal de referência (Setpoint desejado pelo usuario).
 * - Erro: Diferenca entre a Referencia e a Saida medida (e[n] = Ref - Y).
 * - C (Controlador): Equacao de diferencas processada pelo STM32.
 * - u: Acao de controle enviada para a planta (via DAC ou PWM).
 * - G (Planta): Sistema analógico real (Hardware).
 * - y: Saida medida pelo sensor (via ADC do STM32).
 *
 * EQUACAO DE DIFERENCAS 
 * u[n] = b0*e[n] + b1*e[n-1] + b2*e[n-2] - a1*u[n-1] - a2*u[n-2]

 */

 // Estrutura do Controlador Digital
typedef struct {
    float K; // Ganho do controlador (opcional, pode ser incorporado nos coeficientes b)

    // Coeficientes do Numerador (Zeros/Ganho)
    float b0;
    float b1;
    float b2;

    // Coeficientes do Denominador (Polos)
    float a1;
    float a2;

    // Histórico de Erros: erro[0]=atual, [1]=atrasado z^-1, [2]=z^-2
    float erro[3];

    // Histórico de Saídas: u[0]=atual, [1]=z^-1, [2]=z^-2
    float u[3];

    float setpoint; // local de desejo em que o sistema deve operar.
} Controller_t;

// Inicializa os coeficientes e zera os históricos
void Controller_Init(Controller_t *ctrl);

// Processa a equação de diferenças (deve rodar no período T)
float Controller_Compute(Controller_t *ctrl, float y_medido);

#endif // CONTROLLER_CFG_H