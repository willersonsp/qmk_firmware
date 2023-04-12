/*
Copyright 2011 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Ported to QMK by Stephen Peery <https://github.com/smp4488/>
*/

#include <stdint.h>
#include <stdbool.h>
#include <SN32F240.h>
#include "ch.h"
#include "hal.h"
#include "CT16.h"
#include "CT32.h"

#include "color.h"
#include "wait.h"
#include "util.h"
#include "matrix.h"
#include "debounce.h"
#include "quantum.h"

#if defined(OPTICAL_MATRIX)
#ifndef PRESSED_KEY_PIN_STATE
#    define PRESSED_KEY_PIN_STATE 1
#endif
#ifndef MATRIX_KEY_SAMPLE_DELAY
#    define MATRIX_KEY_SAMPLE_DELAY 2000
#endif
#endif

#ifndef PRESSED_KEY_PIN_STATE
#    define PRESSED_KEY_PIN_STATE 0
#endif

#ifndef MATRIX_KEY_SAMPLE_DELAY
#    define MATRIX_KEY_SAMPLE_DELAY 100
#endif

#if defined(MATRIX_KEY_SAMPLE_DELAY)
void sample_delay(void){
    //should give 2000/48000000Mhz = 42us delay
    //we want 42 micro thus to get value
    for (int i = 0; i < MATRIX_KEY_SAMPLE_DELAY; ++i) {
        __asm__ volatile("" ::: "memory");
    }
    //wait_us(73); //does not work
}
#endif

static const pin_t row_pins[MATRIX_ROWS] = MATRIX_ROW_PINS;
static const pin_t col_pins[MATRIX_COLS] = MATRIX_COL_PINS;
static const pin_t led_row_pins[LED_MATRIX_ROWS_HW] = LED_MATRIX_ROW_PINS;
static const pin_t led_col_pins[LED_MATRIX_COLS] = LED_MATRIX_COL_PINS;
static uint16_t row_ofsts[LED_MATRIX_ROWS];
static uint8_t col_offset[21] = {0};

static uint32_t pwm_msk_ct16 = 0;
static uint32_t pwm_msk_ct32 = 0;

matrix_row_t raw_matrix[MATRIX_ROWS]; //raw values
matrix_row_t last_matrix[MATRIX_ROWS] = {0};  // raw values
matrix_row_t matrix[MATRIX_ROWS]; //debounced values

static bool matrix_changed = false;
static uint8_t current_row = 0;
extern volatile LED_TYPE led_state[LED_MATRIX_ROWS * LED_MATRIX_COLS];

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }

__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }

__attribute__((weak)) void matrix_init_user(void) {}

__attribute__((weak)) void matrix_scan_user(void) {}

inline matrix_row_t matrix_get_row(uint8_t row) { return matrix[row]; }

void matrix_print(void) {}

static void init_pins(void) {
#if(DIODE_DIRECTION == ROW2COL)
    //  Unselect ROWs
    for (uint8_t x = 0; x < MATRIX_ROWS; x++) {
        setPinInputHigh(row_pins[x]);
        writePinHigh(row_pins[x]);
    }
#elif(DIODE_DIRECTION == COL2ROW)
    //  Unselect ROWs
    for (uint8_t x = 0; x < MATRIX_ROWS; x++) {
        setPinOutput(row_pins[x]);
        writePinHigh(row_pins[x]);
    }
#else
#error DIODE_DIRECTION must be one of COL2ROW or ROW2COL!
#endif

    // Unselect COLs
    for (uint8_t x = 0; x < MATRIX_COLS; x++) {
        setPinOutput(col_pins[x]);
        writePinHigh(col_pins[x]);
    }

   for (uint8_t x = 0; x < LED_MATRIX_ROWS_HW; x++) {
        setPinOutput(led_row_pins[x]);
        writePinHigh(led_row_pins[x]);
   }
}

void matrix_init(void) {
    // initialize key pins
    init_pins();

    // initialize matrix state: all keys off
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        raw_matrix[i] = 0;
        matrix[i]     = 0;
    }

    for (uint8_t i = 0; i < LED_MATRIX_ROWS; i++) {
        row_ofsts[i] = i * LED_MATRIX_COLS;
    }

    debounce_init(MATRIX_ROWS);

    matrix_init_quantum();

    // Enable Timer Clocks
    SN_SYS1->AHBCLKEN_b.CT16B0CLKEN = 1;
    SN_SYS1->AHBCLKEN_b.CT16B1CLKEN = 1;
    SN_SYS1->AHBCLKEN_b.CT16B2CLKEN = 1;
    SN_SYS1->AHBCLKEN_b.CT32B0CLKEN = 1;
    SN_SYS1->AHBCLKEN_b.CT32B1CLKEN = 1;
    SN_SYS1->AHBCLKEN_b.CT32B2CLKEN = 1;

    // PFPA Timer Mapping
    // CT16B0   PWM0    P0.0        Col 0
    //          PWM1    P0.1        Col 1
    //          PWM2    P1.12       Col 17
    // CT16B1   PWM0    P1.15       Col 20
    //          PWM1    P1.3        Col 13
    //          PWM2    P0.15       Col 9
    // CT16B2   PWM0    P0.2        Col 2
    //          PWM1    P1.4        Col 14
    //          PWM2    P1.0        Col 10
    // CT32B0   PWM0    P0.4        Col 4
    //          PWM1    P1.11       Col 16
    //          PWM2    P1.14       Col 19
    //          PWM3    P1.2        Col 12
    // CT32B1   PWM0    P1.13       Col 18
    //          PWM1    P1.10       Col 15
    //          PWM2    P0.5        Col 5
    //          PWM3    P0.7        Col 7
    // CT32B2   PWM0    P1.1        Col 11
    //          PWM1    P0.14       Col 8
    //          PWM2    P0.3        Col 3
    //          PWM3    P0.6        Col 6
	SN_PFPA->CT16B0 = 0x00000;
    SN_PFPA->CT16B1 = 0x02330;
    SN_PFPA->CT16B2 = 0x02010;
    SN_PFPA->CT32B0 = 0x00410;
	SN_PFPA->CT32B1 = 0x21300;
    SN_PFPA->CT32B2 = 0x00030;

    pwm_msk_ct16 = 0;
    pwm_msk_ct32 = 0;

    // Enable PWM function, IOs and select the PWM modes for the LED column pins
    for(uint8_t i = 0; i < LED_MATRIX_COLS; i++) {
        switch(led_col_pins[i]) {
            case A0:
                pwm_msk_ct16 |=
                    (mskCT16_PWM0EN_EN \
                    |mskCT16_PWM0IOEN_EN);
                col_offset[0] = i;
                break;
            case A1:
                pwm_msk_ct16 |=
                    (mskCT16_PWM1EN_EN \
                    |mskCT16_PWM1IOEN_EN);
                col_offset[1] = i;
                break;
            case A2:
                pwm_msk_ct16 |=
                    (mskCT16_PWM0EN_EN \
                    |mskCT16_PWM0IOEN_EN);
                col_offset[2] = i;
                break;
            case A3:
                pwm_msk_ct32 |=
                    (mskCT32_PWM2EN_EN \
                    |mskCT32_PWM2IOEN_EN);
                col_offset[3] = i;
                break;
            case A4:
                pwm_msk_ct32 |=
                    (mskCT32_PWM0EN_EN \
                    |mskCT32_PWM0IOEN_EN);
                col_offset[4] = i;
                break;
            case A5:
                pwm_msk_ct32 |=
                    (mskCT32_PWM2EN_EN \
                    |mskCT32_PWM2IOEN_EN);
                col_offset[5] = i;
                break;
            case A6:
                pwm_msk_ct32 |=
                    (mskCT32_PWM3EN_EN \
                    |mskCT32_PWM3IOEN_EN);
                col_offset[6] = i;
                break;
            case A7:
                pwm_msk_ct32 |=
                    (mskCT32_PWM3EN_EN \
                    |mskCT32_PWM3IOEN_EN);
                col_offset[7] = i;
                break;
            case A14:
                pwm_msk_ct32 |=
                    (mskCT32_PWM1EN_EN \
                    |mskCT32_PWM1IOEN_EN);
                col_offset[8] = i;
                break;
            case A15:
                pwm_msk_ct16 |=
                    (mskCT16_PWM2EN_EN \
                    |mskCT16_PWM2IOEN_EN);
                col_offset[9] = i;
                break;
            case B0:
                pwm_msk_ct16 |=
                    (mskCT16_PWM2EN_EN \
                    |mskCT16_PWM2IOEN_EN);
                col_offset[10] = i;
                break;
            case B1:
                pwm_msk_ct32 |=
                    (mskCT32_PWM0EN_EN \
                    |mskCT32_PWM0IOEN_EN);
                col_offset[11] = i;
                break;
            case B2:
                pwm_msk_ct32 |=
                    (mskCT32_PWM3EN_EN \
                    |mskCT32_PWM3IOEN_EN);
                col_offset[12] = i;
                break;
            case B3:
                pwm_msk_ct16 |=
                    (mskCT16_PWM1EN_EN \
                    |mskCT16_PWM1IOEN_EN);
                col_offset[13] = i;
                break;
            case B4:
                pwm_msk_ct16 |=
                    (mskCT16_PWM1EN_EN \
                    |mskCT16_PWM1IOEN_EN);
                col_offset[14] = i;
                break;
            case B10:
                pwm_msk_ct32 |=
                    (mskCT32_PWM1EN_EN \
                    |mskCT32_PWM1IOEN_EN);
                col_offset[15] = i;
                break;
            case B11:
                pwm_msk_ct32 |=
                    (mskCT32_PWM1EN_EN \
                    |mskCT32_PWM1IOEN_EN);
                col_offset[16] = i;
                break;
            case B12:
                pwm_msk_ct16 |=
                    (mskCT16_PWM2EN_EN \
                    |mskCT16_PWM2IOEN_EN);
                col_offset[17] = i;
                break;
            case B13:
                pwm_msk_ct32 |=
                    (mskCT32_PWM0EN_EN \
                    |mskCT32_PWM0IOEN_EN);
                col_offset[18] = i;
                break;
            case B14:
                pwm_msk_ct32 |=
                    (mskCT32_PWM2EN_EN \
                    |mskCT32_PWM2IOEN_EN);
                col_offset[19] = i;
                break;
            case B15:
                pwm_msk_ct16 |=
                    (mskCT16_PWM0EN_EN \
                    |mskCT16_PWM0IOEN_EN);
                col_offset[20] = i;
                break;
        }
    }

	// Enable PWWM & IO
	SN_CT16B0->PWMCTRL |= pwm_msk_ct16;
	SN_CT16B1->PWMCTRL |= pwm_msk_ct16;
	SN_CT16B2->PWMCTRL |= pwm_msk_ct16;
	SN_CT32B0->PWMCTRL |= pwm_msk_ct32;
	SN_CT32B1->PWMCTRL |= pwm_msk_ct32;
	SN_CT32B2->PWMCTRL |= pwm_msk_ct32;

    // Set match interrupts and TC reset for CT16B0, which is used to generate interrupt
    SN_CT16B0->MCTRL = (mskCT16_MR3IE_EN|mskCT16_MR3STOP_EN);

    // COL match register
    SN_CT16B0->MR3 = 0xFF;

	// Set prescale value
    if (SystemCoreClock > 6000000)
    {
        SN_CT16B0->PRE = 0x38;
        SN_CT16B1->PRE = 0x38;
        SN_CT16B2->PRE = 0x38;
        SN_CT32B0->PRE = 0x38;
        SN_CT32B1->PRE = 0x38;
        SN_CT32B2->PRE = 0x38;
    }
    else if (SystemCoreClock > 3000000)
    {
        SN_CT16B0->PRE = 0x36;
        SN_CT16B1->PRE = 0x36;
        SN_CT16B2->PRE = 0x36;
        SN_CT32B0->PRE = 0x36;
        SN_CT32B1->PRE = 0x36;
        SN_CT32B2->PRE = 0x36;
    }
    else
    {
        SN_CT16B0->PRE = 0x30;
        SN_CT16B1->PRE = 0x30;
        SN_CT16B2->PRE = 0x30;
        SN_CT32B0->PRE = 0x30;
        SN_CT32B1->PRE = 0x30;
        SN_CT32B2->PRE = 0x30;
    }

    //Set timers to the up-counting mode.
    SN_CT16B0->TMRCTRL = (mskCT16_CRST);
    SN_CT16B1->TMRCTRL = (mskCT16_CRST);
    SN_CT16B2->TMRCTRL = (mskCT16_CRST);
    SN_CT32B0->TMRCTRL = (mskCT32_CRST);
    SN_CT32B1->TMRCTRL = (mskCT32_CRST);
    SN_CT32B2->TMRCTRL = (mskCT32_CRST);

    // Wait until timer reset done.
    while( (SN_CT16B0->TMRCTRL & mskCT16_CRST)
        || (SN_CT16B1->TMRCTRL & mskCT16_CRST)
        || (SN_CT16B2->TMRCTRL & mskCT16_CRST)
        || (SN_CT32B0->TMRCTRL & mskCT32_CRST)
        || (SN_CT32B1->TMRCTRL & mskCT32_CRST)
        || (SN_CT32B2->TMRCTRL & mskCT32_CRST));

    // Let TC start counting.  Start CT16B0 last because it generates our interrupt
    SN_CT16B1->TMRCTRL |= mskCT16_CEN_EN;
    SN_CT16B2->TMRCTRL |= mskCT16_CEN_EN;
    SN_CT32B0->TMRCTRL |= mskCT32_CEN_EN;
    SN_CT32B1->TMRCTRL |= mskCT32_CEN_EN;
    SN_CT32B2->TMRCTRL |= mskCT32_CEN_EN;
    SN_CT16B0->TMRCTRL |= mskCT16_CEN_EN;

    NVIC_ClearPendingIRQ(CT16B0_IRQn);
    nvicEnableVector(CT16B0_IRQn, 0);
}

uint8_t matrix_scan(void) {
	matrix_changed = false;
    for (uint8_t current_col = 0; current_col < MATRIX_COLS; current_col++) {
        for (uint8_t row_index = 0; row_index < MATRIX_ROWS; row_index++) {
            // Determine if the matrix changed state
            if ((last_matrix[row_index] != raw_matrix[row_index])) {
                matrix_changed         = true;
                last_matrix[row_index] = raw_matrix[row_index];
            }
        }
    }

    debounce(raw_matrix, matrix, MATRIX_ROWS, matrix_changed);

    matrix_scan_quantum();

    return matrix_changed;
}

/**
 * @brief   CT16B0 interrupt handler.
 *
 * @isr
 */
OSAL_IRQ_HANDLER(SN32_CT16B0_HANDLER) {

	chSysDisable();

    OSAL_IRQ_PROLOGUE();

     // Disable PWM outputs on column pins
    SN_CT16B0->PWMCTRL = 0;
    SN_CT16B1->PWMCTRL = 0;
    SN_CT16B2->PWMCTRL = 0;
    SN_CT32B0->PWMCTRL = 0;
    SN_CT32B1->PWMCTRL = 0;
    SN_CT32B2->PWMCTRL = 0;

    SN_CT16B0->IC = mskCT16_MR3IC; // Clear match interrupt status

    // Turn the selected row off
    writePinLow(led_row_pins[current_row]);

    // Turn the next row on
    current_row = (current_row + 1) % LED_MATRIX_ROWS_HW;

#if(DIODE_DIRECTION == ROW2COL)
    if(current_row == 0) {
        // Read the key matrix
        for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++) {
            // Enable the column
            writePinLow(col_pins[col_index]);
            sample_delay();
            for (uint8_t row_index = 0; row_index < MATRIX_ROWS; row_index++) {
                // Check row pin state
                if (readPin(row_pins[row_index]) == PRESSED_KEY_PIN_STATE) {
                    // Pin LO, set col bit
                    raw_matrix[row_index] |= (MATRIX_ROW_SHIFTER << col_index);
                } else {
                    // Pin HI, clear col bit
                    raw_matrix[row_index] &= ~(MATRIX_ROW_SHIFTER << col_index);
                }
            }
            // Disable the column
            writePinHigh(col_pins[col_index]);
            //see https://github.com/SonixQMK/qmk_firmware/issues/157
            sample_delay();
        }
    }
#elif(DIODE_DIRECTION == COL2ROW)
    if(current_row == 0)
    {
        // Set all column pins input high
        for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++) {
            setPinInputHigh(col_pins[col_index]);
        }
        // Read the key matrix
        for (uint8_t row_index = 0; row_index < MATRIX_ROWS; row_index++) {
            // Enable the row
            writePinLow(row_pins[row_index]);
            sample_delay();
            for (uint8_t col_index = 0; col_index < MATRIX_COLS; col_index++) {
                // Check row pin state
                if (readPin(col_pins[col_index]) == PRESSED_KEY_PIN_STATE) {
                    // Pin LO, set col bit
                    raw_matrix[row_index] |= (MATRIX_ROW_SHIFTER << col_index);
                } else {
                    // Pin HI, clear col bit
                    raw_matrix[row_index] &= ~(MATRIX_ROW_SHIFTER << col_index);
                }
            }
            // Disable the row
            writePinHigh(row_pins[row_index]);
            //see https://github.com/SonixQMK/qmk_firmware/issues/157
            sample_delay();
        }
    }
#endif

    uint8_t row_idx = ( current_row / 3 );
    uint16_t row_ofst = row_ofsts[row_idx];

    //Set timers to the up-counting mode.
    SN_CT16B0->TMRCTRL = (mskCT16_CRST);
    SN_CT16B1->TMRCTRL = (mskCT16_CRST);
    SN_CT16B2->TMRCTRL = (mskCT16_CRST);
    SN_CT32B0->TMRCTRL = (mskCT32_CRST);
    SN_CT32B1->TMRCTRL = (mskCT32_CRST);
    SN_CT32B2->TMRCTRL = (mskCT32_CRST);

    // Wait until timer reset done.
    while( (SN_CT16B0->TMRCTRL & mskCT16_CRST)
        || (SN_CT16B1->TMRCTRL & mskCT16_CRST)
        || (SN_CT16B2->TMRCTRL & mskCT16_CRST)
        || (SN_CT32B0->TMRCTRL & mskCT32_CRST)
        || (SN_CT32B1->TMRCTRL & mskCT32_CRST)
        || (SN_CT32B2->TMRCTRL & mskCT32_CRST));

    if (current_row % 3 == 0)
    {
		SN_CT16B0->MR0 = led_state[row_ofst + col_offset[0] ].r;
		SN_CT16B0->MR1 = led_state[row_ofst + col_offset[1] ].r;
		SN_CT16B2->MR0 = led_state[row_ofst + col_offset[2] ].r;
		SN_CT32B2->MR2 = led_state[row_ofst + col_offset[3] ].r;
		SN_CT32B0->MR0 = led_state[row_ofst + col_offset[4] ].r;
		SN_CT32B1->MR2 = led_state[row_ofst + col_offset[5] ].r;
		SN_CT32B2->MR3 = led_state[row_ofst + col_offset[6] ].r;
		SN_CT32B1->MR3 = led_state[row_ofst + col_offset[7] ].r;
		SN_CT32B2->MR1 = led_state[row_ofst + col_offset[8] ].r;
		SN_CT16B1->MR2 = led_state[row_ofst + col_offset[9] ].r;
		SN_CT16B2->MR2 = led_state[row_ofst + col_offset[10]].r;
		SN_CT32B2->MR0 = led_state[row_ofst + col_offset[11]].r;
		SN_CT32B0->MR3 = led_state[row_ofst + col_offset[12]].r;
		SN_CT16B1->MR1 = led_state[row_ofst + col_offset[13]].r;
		SN_CT16B2->MR1 = led_state[row_ofst + col_offset[14]].r;
		SN_CT32B1->MR1 = led_state[row_ofst + col_offset[15]].r;
		SN_CT32B0->MR1 = led_state[row_ofst + col_offset[16]].r;
		SN_CT16B0->MR2 = led_state[row_ofst + col_offset[17]].r;
		SN_CT32B1->MR0 = led_state[row_ofst + col_offset[18]].r;
		SN_CT32B0->MR2 = led_state[row_ofst + col_offset[19]].r;
		SN_CT16B1->MR0 = led_state[row_ofst + col_offset[20]].r;
	}

	if (current_row % 3 == 1)
    {
		SN_CT16B0->MR0 = led_state[row_ofst + col_offset[0] ].g;
		SN_CT16B0->MR1 = led_state[row_ofst + col_offset[1] ].g;
		SN_CT16B2->MR0 = led_state[row_ofst + col_offset[2] ].g;
		SN_CT32B2->MR2 = led_state[row_ofst + col_offset[3] ].g;
		SN_CT32B0->MR0 = led_state[row_ofst + col_offset[4] ].g;
		SN_CT32B1->MR2 = led_state[row_ofst + col_offset[5] ].g;
		SN_CT32B2->MR3 = led_state[row_ofst + col_offset[6] ].g;
		SN_CT32B1->MR3 = led_state[row_ofst + col_offset[7] ].g;
		SN_CT32B2->MR1 = led_state[row_ofst + col_offset[8] ].g;
		SN_CT16B1->MR2 = led_state[row_ofst + col_offset[9] ].g;
		SN_CT16B2->MR2 = led_state[row_ofst + col_offset[10]].g;
		SN_CT32B2->MR0 = led_state[row_ofst + col_offset[11]].g;
		SN_CT32B0->MR3 = led_state[row_ofst + col_offset[12]].g;
		SN_CT16B1->MR1 = led_state[row_ofst + col_offset[13]].g;
		SN_CT16B2->MR1 = led_state[row_ofst + col_offset[14]].g;
		SN_CT32B1->MR1 = led_state[row_ofst + col_offset[15]].g;
		SN_CT32B0->MR1 = led_state[row_ofst + col_offset[16]].g;
		SN_CT16B0->MR2 = led_state[row_ofst + col_offset[17]].g;
		SN_CT32B1->MR0 = led_state[row_ofst + col_offset[18]].g;
		SN_CT32B0->MR2 = led_state[row_ofst + col_offset[19]].g;
		SN_CT16B1->MR0 = led_state[row_ofst + col_offset[20]].g;
	}

	if (current_row % 3 == 2)
    {
		SN_CT16B0->MR0 = led_state[row_ofst + col_offset[0] ].b;
		SN_CT16B0->MR1 = led_state[row_ofst + col_offset[1] ].b;
		SN_CT16B2->MR0 = led_state[row_ofst + col_offset[2] ].b;
		SN_CT32B2->MR2 = led_state[row_ofst + col_offset[3] ].b;
		SN_CT32B0->MR0 = led_state[row_ofst + col_offset[4] ].b;
		SN_CT32B1->MR2 = led_state[row_ofst + col_offset[5] ].b;
		SN_CT32B2->MR3 = led_state[row_ofst + col_offset[6] ].b;
		SN_CT32B1->MR3 = led_state[row_ofst + col_offset[7] ].b;
		SN_CT32B2->MR1 = led_state[row_ofst + col_offset[8] ].b;
		SN_CT16B1->MR2 = led_state[row_ofst + col_offset[9] ].b;
		SN_CT16B2->MR2 = led_state[row_ofst + col_offset[10]].b;
		SN_CT32B2->MR0 = led_state[row_ofst + col_offset[11]].b;
		SN_CT32B0->MR3 = led_state[row_ofst + col_offset[12]].b;
		SN_CT16B1->MR1 = led_state[row_ofst + col_offset[13]].b;
		SN_CT16B2->MR1 = led_state[row_ofst + col_offset[14]].b;
		SN_CT32B1->MR1 = led_state[row_ofst + col_offset[15]].b;
		SN_CT32B0->MR1 = led_state[row_ofst + col_offset[16]].b;
		SN_CT16B0->MR2 = led_state[row_ofst + col_offset[17]].b;
		SN_CT32B1->MR0 = led_state[row_ofst + col_offset[18]].b;
		SN_CT32B0->MR2 = led_state[row_ofst + col_offset[19]].b;
		SN_CT16B1->MR0 = led_state[row_ofst + col_offset[20]].b;
	}

    uint32_t new_pwm_ct16 = 0;
    uint32_t new_pwm_ct32 = 0;

    // Enable PWM on all timers
    if(SN_CT16B0->MR0 > 0)
    {
        new_pwm_ct16 |=
            (mskCT16_PWM0EN_EN \
            |mskCT16_PWM0IOEN_EN);
    }
    if(SN_CT16B0->MR1 > 0)
    {
        new_pwm_ct16 |=
            (mskCT16_PWM1EN_EN \
            |mskCT16_PWM1IOEN_EN);
    }
    if(SN_CT16B2->MR0 > 0)
    {
        new_pwm_ct16 |=
            (mskCT16_PWM0EN_EN \
            |mskCT16_PWM0IOEN_EN);
    }
    if(SN_CT32B2->MR2 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM2EN_EN \
            |mskCT32_PWM2IOEN_EN);
    }
    if(SN_CT32B0->MR0 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM0EN_EN \
            |mskCT32_PWM0IOEN_EN);
    }
    if(SN_CT32B1->MR2 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM2EN_EN \
            |mskCT32_PWM2IOEN_EN);
    }
    if(SN_CT32B2->MR3 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM3EN_EN \
            |mskCT32_PWM3IOEN_EN);
    }
    if(SN_CT32B1->MR3 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM3EN_EN \
            |mskCT32_PWM3IOEN_EN);
    }
    if(SN_CT32B2->MR1 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM1EN_EN \
            |mskCT32_PWM1IOEN_EN);
    }
    if(SN_CT16B1->MR2 > 0)
    {
        new_pwm_ct16 |=
            (mskCT16_PWM2EN_EN \
            |mskCT16_PWM2IOEN_EN);
    }
    if(SN_CT16B2->MR2 > 0)
    {
        new_pwm_ct16 |=
            (mskCT16_PWM2EN_EN \
            |mskCT16_PWM2IOEN_EN);
    }
    if(SN_CT32B2->MR0 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM0EN_EN \
            |mskCT32_PWM0IOEN_EN);
    }
    if(SN_CT32B0->MR3 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM3EN_EN \
            |mskCT32_PWM3IOEN_EN);
    }
    if(SN_CT16B1->MR1 > 0)
    {
        new_pwm_ct16 |=
            (mskCT16_PWM1EN_EN \
            |mskCT16_PWM1IOEN_EN);
    }
    if(SN_CT16B2->MR1 > 0)
    {
        new_pwm_ct16 |=
            (mskCT16_PWM1EN_EN \
            |mskCT16_PWM1IOEN_EN);
    }
    if(SN_CT32B1->MR1 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM1EN_EN \
            |mskCT32_PWM1IOEN_EN);
    }
    if(SN_CT32B0->MR1 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM1EN_EN \
            |mskCT32_PWM1IOEN_EN);
    }
    if(SN_CT16B0->MR2 > 0)
    {
        new_pwm_ct16 |=
            (mskCT16_PWM2EN_EN \
            |mskCT16_PWM2IOEN_EN);
    }
    if(SN_CT32B1->MR0 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM0EN_EN \
            |mskCT32_PWM0IOEN_EN);
    }
    if(SN_CT32B0->MR2 > 0)
    {
        new_pwm_ct32 |=
            (mskCT32_PWM2EN_EN \
            |mskCT32_PWM2IOEN_EN);
    }
    if(SN_CT16B1->MR0 > 0)
    {
        new_pwm_ct16 |=
            (mskCT16_PWM0EN_EN \
            |mskCT16_PWM0IOEN_EN);
    }

    SN_CT16B0->PWMCTRL = pwm_msk_ct16 & new_pwm_ct16;
    SN_CT16B1->PWMCTRL = pwm_msk_ct16 & new_pwm_ct16;
    SN_CT16B2->PWMCTRL = pwm_msk_ct16 & new_pwm_ct16;
    SN_CT32B0->PWMCTRL = pwm_msk_ct32 & new_pwm_ct32;
    SN_CT32B1->PWMCTRL = pwm_msk_ct32 & new_pwm_ct32;
    SN_CT32B2->PWMCTRL = pwm_msk_ct32 & new_pwm_ct32;

    // Set match interrupts and TC rest for CT16B0, which is used to generate interrupt
    SN_CT16B0->MCTRL = (mskCT16_MR3IE_EN|mskCT16_MR3STOP_EN);

    writePinHigh(led_row_pins[current_row]);

    // Let TC start counting.  Start CT16B0 last because it generates our interrupt
    SN_CT16B1->TMRCTRL |= mskCT16_CEN_EN;
    SN_CT16B2->TMRCTRL |= mskCT16_CEN_EN;
    SN_CT32B0->TMRCTRL |= mskCT32_CEN_EN;
    SN_CT32B1->TMRCTRL |= mskCT32_CEN_EN;
    SN_CT32B2->TMRCTRL |= mskCT32_CEN_EN;
    SN_CT16B0->TMRCTRL |= mskCT16_CEN_EN;

    chSysEnable();

    OSAL_IRQ_EPILOGUE();
}
