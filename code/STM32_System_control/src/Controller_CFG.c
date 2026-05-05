#include "Controller_CFG.h"



void Controller_Init(Controller_t *ctrl) {
    // Coeficientes do Numerador (b)
    ctrl->K = 4.308500f;
    ctrl->b0 = 1.0f * ctrl->K;       // resultado = 4.308500f (ganho total do controlador)
    ctrl->b1 = -1.562f * ctrl->K;    // resultado = -6.733f (ganho total do controlador)
    ctrl->b2 = 0.6854f * ctrl->K;   // resultado = 2.951f (ganho total do controlador)

    // Coeficientes do Denominador (a)
    ctrl->a1 = -0.870289f;   
    ctrl->a2 = -0.129711f;   


    // Limpa os históricos
    for (int i = 0; i < 3; i++) {
        ctrl->erro[i] = 0.0f;
        ctrl->u[i] = 0.0f;
    }
    ctrl->setpoint = 1.0f; // Ponto de operação desejado 
}

float Controller_Compute(Controller_t *ctrl, float y_medido) {
    // Cálculo do erro atual: e[n] = r[n] - y[n]
    ctrl->erro[0] = ctrl->setpoint - y_medido; // y_medido é a saída atual da planta (medida pelo ADC)

    /*
     * Equação de Diferenças 
     * u[n] = b0*e[n] + b1*e[n-1] + b2*e[n-2] - a1*u[n-1] - a2*u[n-2]
     * u[0] é a saída atual (do controlador), u[1] é a saída atrasada z^-1, u[2] é a saída atrasada z^-2... 
     */
    ctrl->u[0] = (ctrl->b0 * ctrl->erro[0]) + (ctrl->b1 * ctrl->erro[1]) + (ctrl->b2 * ctrl->erro[2]) - (ctrl->a1 * ctrl->u[1]) - (ctrl->a2 * ctrl->u[2]);

    // Saturação (Garante que a saída não passe dos limites do hardware 0-3.3V)
    if (ctrl->u[0] > 3.3f) ctrl->u[0] = 3.3f;
    else if (ctrl->u[0] < 0.0f) ctrl->u[0] = 0.0f;

    // Atualização do Histórico (Shift de memória para o próximo passo T)
    ctrl->erro[2] = ctrl->erro[1];
    ctrl->erro[1] = ctrl->erro[0];
    
    ctrl->u[2] = ctrl->u[1];
    ctrl->u[1] = ctrl->u[0];

    return ctrl->u[0];
}