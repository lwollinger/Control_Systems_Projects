#ifndef CONTROLLER_CFG_H
#define CONTROLLER_CFG_H

 // Estrutura do Controlador Digital
typedef struct {
    float setpoint;
    
    // VETORES DE GANHOS DO CONTROLADOR
    float K[3];   // Vetor de ganhos do controlador
    float Ke[2];  // Vetor de ganhos do observador de ordem mínima
    float Ki;     // Ganho do integrador
    
    // MATRIZES DO OBSERVADOR
    float Achapeu[2][2]; // Matriz de dinâmica do observador (2x2)
    float Bchapeu[2];    // Vetor de entrada de y medido (2x1)
    float Fchapeu[2];    // Vetor de entrada da ação de controle u (2x1)
    
    // Estados internos (Método de Euler)
    float xi;
    float xi_ponto;
    float eta1;
    float eta2;
    float eta1_ponto;
    float eta2_ponto;

    // Variáveis de monitoramento/debug
    float erro;
    float u;
} Controller_t;

// Inicializa os coeficientes e zera os históricos
void Controller_Init(Controller_t *ctrl);

// Processa a equação de diferenças (deve rodar no período T)
float Controller_Compute(Controller_t *ctrl, float y_medido);

#endif // CONTROLLER_CFG_H