#include "Controller_CFG.h"
#include "stdio.h"


// Período de amostragem (0.0001s)
#define T_AMOSTRAGEM  0.0001f


void Controller_Init(Controller_t *ctrl) {
    ctrl->setpoint = 1.0f;
    
    // =======================================================
    // COPIE E COLE TODAS AS MATRIZES DO SEU MATLAB AQUI!
    // =======================================================
    // Ganhos do Controle
    ctrl->K[0]  = -38.6448f; // K(1)
    ctrl->K[1]  = 75.9372f;  // K(2)
    ctrl->K[2]  = 1.9273f;   // K(3)
    ctrl->Ke[0] = 1.2851f;   // Ke(1)
    ctrl->Ke[1] = 85.2124f;  // Ke(2)
    ctrl->Ki    = 4788.1f;   // Ki
    
    // Matriz Achapeu (Linha por linha)
    ctrl->Achapeu[0][0] = -218.05f;
    ctrl->Achapeu[0][1] = 21.6263f;
    ctrl->Achapeu[1][0] = -38733.0f;
    ctrl->Achapeu[1][1] = -666.67f;

    // Vetor Bchapeu
    ctrl->Bchapeu[0] = 1759.0f;
    ctrl->Bchapeu[1] = -67850.0f;

    // Vetor Fchapeu
    ctrl->Fchapeu[0] = 0.0f;
    ctrl->Fchapeu[1] = 666.6667f;

    // Inicializa todos os estados e derivadas em zero
    ctrl->xi = 0.0f;
    ctrl->xi_ponto = 0.0f;
    ctrl->eta1 = 0.0f;
    ctrl->eta2 = 0.0f;
    ctrl->eta1_ponto = 0.0f;
    ctrl->eta2_ponto = 0.0f;
    
    ctrl->erro = 0.0f;
    ctrl->u = 0.0f;
}


/*
float Controller_Compute(Controller_t *ctrl, float y_medido) {
    // 1. Integração numérica de Euler para o passo atual
    ctrl->xi   += T_AMOSTRAGEM * ctrl->xi_ponto;
    ctrl->eta1 += T_AMOSTRAGEM * ctrl->eta1_ponto;
    ctrl->eta2 += T_AMOSTRAGEM * ctrl->eta2_ponto;

    // 2. Reconstrução dos estados não medidos usando o vetor Ke da struct
    float xtil2 = ctrl->eta1 + ctrl->Ke[0] * y_medido;
    float xtil3 = ctrl->eta2 + ctrl->Ke[1] * y_medido;

    // 3. Lei de Controle por Espaço de Estados 
    ctrl->u = -ctrl->K[0]*y_medido - ctrl->K[1]*xtil2 - ctrl->K[2]*xtil3 + ctrl->Ki * ctrl->xi;
    //ctrl->u = ctrl->setpoint;

    // Saturação anti-windup de segurança (0V a 3.3V)
    if (ctrl->u > 3.3f)  ctrl->u = 3.3f;
    if (ctrl->u < 0.0f)  ctrl->u = 0.0f;

    // 4. Cálculo das derivadas que serão integradas no PRÓXIMO período
    ctrl->erro = ctrl->setpoint - y_medido;
    ctrl->xi_ponto = ctrl->erro;

    // eta_ponto = Achapeu * eta + Bchapeu * y + Fchapeu * u
    ctrl->eta1_ponto = (ctrl->Achapeu[0][0] * ctrl->eta1) + (ctrl->Achapeu[0][1] * ctrl->eta2) + 
                       (ctrl->Bchapeu[0] * y_medido) + (ctrl->Fchapeu[0] * ctrl->u);
                       
    ctrl->eta2_ponto = (ctrl->Achapeu[1][0] * ctrl->eta1) + (ctrl->Achapeu[1][1] * ctrl->eta2) + 
                       (ctrl->Bchapeu[1] * y_medido) + (ctrl->Fchapeu[1] * ctrl->u);

    return ctrl->u;
}
*/


float Controller_Compute(Controller_t *ctrl, float y_medido) {
    // 1. Integração numérica de Euler para o passo atual (com base no passo anterior)


    ctrl->xi   += T_AMOSTRAGEM * ctrl->xi_ponto;
    ctrl->eta1 += T_AMOSTRAGEM * ctrl->eta1_ponto;
    ctrl->eta2 += T_AMOSTRAGEM * ctrl->eta2_ponto;

    // 2. Reconstrução dos estados não medidos (xtil2 e xtil3)
    // Multiplicações da matriz Cchapeu e Dchapeu simplificadas algebricamente:
    float xtil2 = ctrl->eta1 + 1.2851f * y_medido;   // eta1 + Ke(1)*y
    float xtil3 = ctrl->eta2 + 85.2124f * y_medido;  // eta2 + Ke(2)*y

    // 3. Lei de Controle: u = -K1*y - K2*xtil2 - K3*xtil3 + Ki*xi
    // Nota: K(1) no MATLAB é negativo (-38.6448), logo: -(-38.6448)*y = +38.6448*y
    ctrl->u = (38.6448f * y_medido) - (75.9372f * xtil2) - (1.9273f * xtil3) + (4788.1f * ctrl->xi);

    // Saturação anti-windup (Proteção do Hardware entre 0V e 3.3V)
    if (ctrl->u > 3.3f)  ctrl->u = 3.3f;
    if (ctrl->u < 0.0f)  ctrl->u = 0.0f;

    // 4. Cálculo das derivadas (*_ponto) que serão integradas no PRÓXIMO passo
    ctrl->erro = ctrl->setpoint - y_medido;
    ctrl->xi_ponto = ctrl->erro;

    // Equações da dinâmica do Observador (Achapeu*eta + Bchapeu*y + Fchapeu*u)
    ctrl->eta1_ponto = (-218.05f * ctrl->eta1) + (21.6263f * ctrl->eta2) + (1759.0f * y_medido); // Fchapeu[0] é 0
    ctrl->eta2_ponto = (-38733.0f * ctrl->eta1) - (666.67f * ctrl->eta2) - (67850.0f * y_medido) + (666.6667f * ctrl->u);

    return ctrl->u;
}