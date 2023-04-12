#include "rgb_matrix.h"
#include "sn32f24x.h"

#if !defined(RGB_MATRIX_HUE_STEP)
#    define RGB_MATRIX_HUE_STEP 8
#endif

#if !defined(RGB_MATRIX_SAT_STEP)
#    define RGB_MATRIX_SAT_STEP 16
#endif

#if !defined(RGB_MATRIX_VAL_STEP)
#    define RGB_MATRIX_VAL_STEP 16
#endif

#if !defined(RGB_MATRIX_SPD_STEP)
#    define RGB_MATRIX_SPD_STEP 16
#endif

/*
    COLS key / led
    PWM PWM00A - PWM21A (PWM15A unused)
    2ty transistors PNP driven high
    base      - GPIO
    collector - LED Col pins
    emitter   - VDD

    VDD     GPIO
    (E)     (B)
     |  PNP  |
     |_______|
         |
         |
        (C)
        LED

    ROWS RGB
    PWM PWM22A - PWM21B (PWM10B unused)
    C 0-15
    j3y transistors NPN driven low
    base      - GPIO
    collector - LED RGB row pins
    emitter   - GND

        LED
        (C)
         |
         |
      _______
     |  NPN  |
     |       |
    (B)     (E)
    GPIO    GND
*/

LED_TYPE led_state[LED_MATRIX_ROWS * LED_MATRIX_COLS];
uint8_t led_pos[DRIVER_LED_TOTAL];

void SN32F24x_init(void) {
    unsigned int i = 0;
    for (unsigned int y = 0; y < LED_MATRIX_ROWS; y++) {
        for (unsigned int x = 0; x < LED_MATRIX_COLS; x++) {
            if (g_led_config.matrix_co[y][x] != NO_LED) {
                led_pos[g_led_config.matrix_co[y][x]] = i;
            }
            i++;
        }
    }
}

static void flush(void) {}

void SN32F24x_set_color(int index, uint8_t r, uint8_t g, uint8_t b) {
    int corrected_index = led_pos[index];
    led_state[corrected_index].r = r;
    led_state[corrected_index].g = g;
    led_state[corrected_index].b = b;
}

void SN32F24x_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int i=0; i<DRIVER_LED_TOTAL; i++)
        SN32F24x_set_color(i, r, g, b);
}

const rgb_matrix_driver_t rgb_matrix_driver = {
    .init          = SN32F24x_init,
    .flush         = flush,
    .set_color     = SN32F24x_set_color,
    .set_color_all = SN32F24x_set_color_all,
};
