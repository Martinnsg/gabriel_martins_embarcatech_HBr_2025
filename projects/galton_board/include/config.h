#ifndef CONFIG_H
#define CONFIG_H

// Configurações de hardware
#define I2C_SDA 14
#define I2C_SCL 15
#define OLED_ADDR 0x3C
#define I2C_CLOCK_SPEED 400000  // 400kHz

// Configurações da Galton Board
#define NUM_ROWS 8
#define NUM_COLS 9
#define PIN_RADIUS 2
#define BALL_RADIUS 3
#define MAX_BALLS 10
#define BALL_SPEED 1

// Posicionamento
#define BOARD_X_OFFSET 20
#define BOARD_Y_OFFSET 10
#define PIN_SPACING_X 12
#define PIN_SPACING_Y 12

#endif // CONFIG_H