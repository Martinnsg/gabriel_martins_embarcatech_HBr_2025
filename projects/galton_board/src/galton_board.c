#include "pico/stdlib.h"
#include "include/display.h"
<<<<<<< HEAD
#include "include/physics.h"

void draw_board(Display *disp) {
    display_clear(disp);
    
    // Desenha os pinos
    for (uint8_t row = 0; row < NUM_ROWS; row++) {
        uint8_t num_cols = NUM_COLS - row;
        for (uint8_t col = 0; col < num_cols; col++) {
            int x = BOARD_X_OFFSET + col * PIN_SPACING_X;
            if (row % 2 == 1) x += PIN_SPACING_X / 2;
            int y = BOARD_Y_OFFSET + row * PIN_SPACING_Y;
            display_draw_circle(disp, x, y, PIN_RADIUS, true);
        }
    }
}

void draw_histogram(Display *disp, int *bins, int total_balls) {
    if (total_balls == 0) return;
    
    int max_height = 20;
    int bin_width = 8;
    int start_x = BOARD_X_OFFSET + 10;
    int start_y = SSD1306_HEIGHT - 5;
    
    // Desenha eixos
    display_draw_line(disp, start_x, start_y, start_x + NUM_COLS * bin_width, start_y, true);
    display_draw_line(disp, start_x, start_y, start_x, start_y - max_height, true);
    
    // Desenha barras
    for (int i = 0; i < NUM_COLS; i++) {
        int height = (bins[i] * max_height) / total_balls;
        if (height > max_height) height = max_height;
        
        int x1 = start_x + i * bin_width;
        int x2 = x1 + bin_width - 2;
        int y1 = start_y - height;
        
        display_draw_line(disp, x1, y1, x2, y1, true);
        display_draw_line(disp, x1, y1, x1, start_y, true);
        display_draw_line(disp, x2, y1, x2, start_y, true);
    }
    
    // Mostra contador
    char text[16];
    snprintf(text, sizeof(text), "Bolas: %d", total_balls);
    display_draw_string(disp, SSD1306_WIDTH - 60, SSD1306_HEIGHT - 8, text);
}

int main() {
    stdio_init_all();
    Display display;
    display_init(&display);
    
    GaltonBoard board;
    physics_init(&board, 0.5); // Bias de 0.5 = balanceado
    
    uint32_t last_ball_time = 0;
    bool experiment_complete = false;
    
    while (true) {
        uint32_t now = to_ms_since_boot(get_absolute_time());
        
        // Adiciona nova bola a cada segundo
        if (!experiment_complete && now - last_ball_time > 1000 && board.total_balls < 20) {
            physics_add_ball(&board);
            last_ball_time = now;
        }
        
        // Atualiza física
        physics_update(&board);
        experiment_complete = physics_is_complete(&board);
        
        // Desenha cena
        draw_board(&display);
        
        // Desenha bolas
        for (int i = 0; i < MAX_BALLS; i++) {
            if (board.balls[i].active) {
                display_draw_circle(&display, board.balls[i].x, board.balls[i].y, BALL_RADIUS, true);
            }
        }
        
        // Desenha histograma
        draw_histogram(&display, board.bins, board.total_balls);
        
        display_update(&display);
        sleep_ms(16); // ~60 FPS
    }
    
=======
#include "include/galton.h"
#include "include/ball.h"
#include "include/display.h"

int main() {
    stdio_init_all();

    Display *display;
    display_init(display);

    while (true) {
        display_draw_pins(display, MAX_BINS);
    }

>>>>>>> 4416be1 (connect to wifi ok)
    return 0;
}