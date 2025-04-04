/* Copyright 2011 Jun Wako <wakojun@gmail.com>
 * RP2040 Port Copyright 2023
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

/* RP2040 specific definitions */
#define VENDOR_ID       0xFEED
#define PRODUCT_ID      0xADB0
#define DEVICE_VER      0x0101
#define MANUFACTURER    "QMK"
#define PRODUCT         "ADB-to-USB Keyboard Converter (RP2040)"

/* key matrix size */
#define MATRIX_ROWS 16
#define MATRIX_COLS 8

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* ADB port setting */
#define ADB_DATA_PIN GP0

/* Set 0 if no ADB power support */
#define ADB_POWER_PORT GP1

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

/* RP2040 has a different reset than AVR */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED GP25
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U

/* Double tap reset on the boot pin allows entering bootloader */
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET 
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U

/* USB Device descriptor parameter */
#define USB_MAX_POWER_CONSUMPTION 500

/* 
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT