/* Copyright 2023 Willerson Stockler <https://github.com/willersonsp>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "config_common.h"
#include "config_led.h"

/* USB Device descriptor parameter */
#define VENDOR_ID       0x80E7
#define PRODUCT_ID      0x5001
#define DEVICE_VER      0x0001
#define MANUFACTURER    Sharkoon
#define PRODUCT         SKILLER MECH SGK3

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* Forcing to use NKRO instead 6KRO */
#define FORCE_NKRO

/* Change larger keys per scan for elite gaming - disabled QMK deprecated */
//#define QMK_KEYS_PER_SCAN 12

