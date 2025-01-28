/* Código interrupções no microcontrolador RP2040 e explorar as
funcionalidades da placa de desenvolvimento BitDogLab.
Uso de interrupções
Fenômeno do bouncing
Controle de leds
*/

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define LED_R_PIN 11
#define LED_G_PIN 12
#define LED_B_PIN 13
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6
#define TEMPO 100

// Mapas dos números (0-9) na matriz 5x5
bool numbers[10][NUM_PIXELS] = {
    {1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1}, // 0

    {0, 1, 1, 1, 0,
     0, 0, 1, 0, 0,
     0, 0, 1, 0, 1,
     0, 1, 1, 0, 0,
     0, 0, 1, 0, 0}, // 1

    {1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1}, // 2

    {1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     0, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1}, // 3

    {1, 0, 0, 0, 0,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 0, 0, 0, 1}, // 4

    {1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1}, // 5

    {1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 0,
     1, 1, 1, 1, 1}, // 6

    {0, 0, 0, 0, 1,
     0, 1, 0, 0, 0,
     0, 1, 1, 1, 0,
     0, 0, 0, 1, 0,
     1, 1, 1, 1, 1}, // 7

    {1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1}, // 8

    {1, 1, 1, 1, 1,
     0, 0, 0, 0, 1,
     1, 1, 1, 1, 1,
     1, 0, 0, 0, 1,
     1, 1, 1, 1, 1} // 9
};

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void display_number(int number)
{
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (numbers[number][i])
        {
            put_pixel(urgb_u32(50, 0, 0)); // vermelho para os números
        }
        else
        {
            put_pixel(0); // Apaga o LED
        }
    }
}

void blink_led_rgb()
{
    gpio_put(LED_R_PIN, 1);
    sleep_ms(100);
    gpio_put(LED_R_PIN, 0);
    sleep_ms(100);
}

int main()
{
    stdio_init_all();

    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    int current_number = 0;

    while (1)
    {
        // Piscar o LED RGB vermelho
        blink_led_rgb();

        // Verifica se o botão A foi pressionado
        if (!gpio_get(BUTTON_A_PIN))
        {
            current_number = (current_number + 1) % 10;
            display_number(current_number);
            sleep_ms(200); // Debounce
        }

        // Verifica se o botão B foi pressionado
        if (!gpio_get(BUTTON_B_PIN))
        {
            current_number = (current_number - 1 + 10) % 10;
            display_number(current_number);
            sleep_ms(200); // Debounce
        }
    }

    return 0;
}
