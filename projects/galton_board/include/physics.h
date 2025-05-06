#ifndef PHYSICS_H
#define PHYSICS_H

#include "config.h"
#include "display.h"

typedef struct {
    int x, y;       // Posição
    int speed;      // Velocidade
    bool active;    // Se está em movimento
    int col;        // Coluna final no histograma
} Ball;

typedef struct {
    Ball balls[MAX_BALLS];
    int bins[NUM_COLS];
    int total_balls;
    float bias;     // Viés (0.5 = balanceado)
} GaltonBoard;

// Inicialização
void physics_init(GaltonBoard *board, float bias);

// Adicionar nova bola
void physics_add_ball(GaltonBoard *board);

// Atualizar física
void physics_update(GaltonBoard *board);

// Verificar se todas as bolas terminaram
bool physics_is_complete(GaltonBoard *board);

#endif // PHYSICS_H