#include "pico/stdlib.h"
#include "include/display.h"

int main() {
    stdio_init_all();

    Display display;
    display_init(&display);

    // Mensagens para exibir
    const char *text[] = {
        "  Bem-vindos!   ",
        "  Embarcatech   "
    };

    // Desenha as strings
    int y = 0;
    for (uint i = 0; i < sizeof(text)/sizeof(text[0]); i++) {
        display_draw_string(&display, 5, y, text[i]);
        y += 8;
    }

    // Atualiza o display
    display_update(&display);

    while(true) {
        sleep_ms(1000);
    }

    return 0;
}