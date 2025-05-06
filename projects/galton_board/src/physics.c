#include "include/physics.h"
#include <stdlib.h>
#include "pico/time.h"

void physics_init(GaltonBoard *board, float bias) {
    // Inicializa randomização
    srand(to_us_since_boot(get_absolute_time()));
    
    for (int i = 0; i < MAX_BALLS; i++) {
        board->balls[i].active = false;
    }
    
    for (int i = 0; i < NUM_COLS; i++) {
        board->bins[i] = 0;
    }
    
    board->total_balls = 0;
    board->bias = bias;
}

void physics_add_ball(GaltonBoard *board) {
    if (board->total_balls >= MAX_BALLS) return;
    
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!board->balls[i].active) {
            board->balls[i].x = BOARD_X_OFFSET + (NUM_COLS * PIN_SPACING_X) / 2;
            board->balls[i].y = BOARD_Y_OFFSET - BALL_RADIUS * 2;
            board->balls[i].speed = BALL_SPEED;
            board->balls[i].active = true;
            board->balls[i].col = NUM_COLS / 2;
            board->total_balls++;
            break;
        }
    }
}

void physics_update(GaltonBoard *board) {
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!board->balls[i].active) continue;
        
        Ball *ball = &board->balls[i];
        ball->y += ball->speed;
        
        // Colisão com pinos
        for (uint8_t row = 0; row < NUM_ROWS; row++) {
            uint8_t num_cols = NUM_COLS - row;
            for (uint8_t col = 0; col < num_cols; col++) {
                int pin_x = BOARD_X_OFFSET + col * PIN_SPACING_X;
                if (row % 2 == 1) pin_x += PIN_SPACING_X / 2;
                int pin_y = BOARD_Y_OFFSET + row * PIN_SPACING_Y;
                
                int dx = ball->x - pin_x;
                int dy = ball->y - pin_y;
                if (dx*dx + dy*dy <= (PIN_RADIUS + BALL_RADIUS)*(PIN_RADIUS + BALL_RADIUS)) {
                    float r = (float)rand() / (float)RAND_MAX;
                    if (r < board->bias) {
                        ball->x += 2;
                        ball->col++;
                    } else {
                        ball->x -= 2;
                        ball->col--;
                    }
                }
            }
        }
        
        // Verifica se chegou ao final
        if (ball->y >= BOARD_Y_OFFSET + NUM_ROWS * PIN_SPACING_Y + 10) {
            if (ball->col >= 0 && ball->col < NUM_COLS) {
                board->bins[ball->col]++;
            }
            ball->active = false;
        }
    }
}

bool physics_is_complete(GaltonBoard *board) {
    for (int i = 0; i < MAX_BALLS; i++) {
        if (board->balls[i].active) {
            return false;
        }
    }
    return true;
}