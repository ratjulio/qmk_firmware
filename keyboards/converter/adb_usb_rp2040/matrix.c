/*
 * ADB to USB Keyboard Converter - RP2040 implementation
 *
 * Based on the original AVR implementation by TMK, adapted for RP2040
 */

#include <stdint.h>
#include <stdbool.h>
#include "quantum.h"
#include "adb.h"
#include "matrix.h"
#include "wait.h"
#include "print.h"
#include "debug.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// ADB power control
#ifndef ADB_POWER_PORT
#define ADB_POWER_PORT GP1
#endif

static bool matrix_is_modified = false;
static uint16_t matrix_last_modified = 0;

// matrix state buffer(1:on, 0:off)
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t matrix_debounced[MATRIX_ROWS];
static uint8_t led_states = 0;

static void matrix_power_up(void);
static void init_led(void);
static void led_set_adb(uint8_t bits);

/* ADB Keyboard matrix signals */
#define CAPS_LOCK   0x02
#define NUM_LOCK    0x01
#define SCROLL_LOCK 0x04

matrix_row_t matrix_get_row(uint8_t row) {
    return matrix_debounced[row];
}

void matrix_print(void) {
    print("\nr/c 01234567\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        print_hex8(row); print(": ");
        print_bin_reverse8(matrix_get_row(row));
        print("\n");
    }
}

void matrix_init(void) {
    debug_enable = true;
    dprintf("matrix_init()\n");

    // Initialize ADB power control pin
    if (ADB_POWER_PORT) {
        gpio_init(ADB_POWER_PORT);
        gpio_set_dir(ADB_POWER_PORT, GPIO_OUT);
    }

    matrix_power_up();
    
    // Wait for ADB keyboard to power up
    wait_ms(1000);

    // Initialize ADB host
    adb_host_init();

    // Initialize matrix state
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        matrix_debounced[i] = 0;
    }

    // Initialize LED status
    init_led();
}

static void init_led(void) {
    // Initialize keyboard led status
    led_set_adb(0);
}

// Converts key matrix from ADB scan codes to QMK matrix format
uint8_t matrix_scan(void) {
    uint16_t codes[MATRIX_ROWS];
    uint8_t key_count;

    // Empty buffer for scan result
    memset(codes, 0, sizeof(codes));

    // Get ADB keyboard data
    uint8_t buf[8];
    uint8_t bufsize = 8;
    
    if (!adb_host_kbd_recv(buf, &bufsize)) {
        return 0;
    }

    // Skip if no data received
    if (!bufsize) {
        return 0;
    }

    // Convert ADB data to matrix
    uint8_t adb_pos = 0;
    key_count = 0;
    
    // Parse ADB keyboard data to get scan codes
    for (uint8_t i = 0; i < bufsize; i++) {
        uint8_t key = buf[i];
        if (key == 0xff) {
            continue;  // Empty byte
        }
        
        uint8_t key_code = key & 0x7f;
        bool key_pressed = !(key & 0x80);  // ADB release uses high bit
        
        // Skip special keys
        if (key_code >= 0x7f) {
            continue;
        }
        
        // Convert ADB key code to matrix position
        // ADB scan code: row:4, col:3 (7bit)
        uint8_t row = (key_code >> 3) & 0x0F;
        uint8_t col = key_code & 0x07;
        
        // Update matrix state
        if (key_pressed) {
            matrix[row] |= (1 << col);
        } else {
            matrix[row] &= ~(1 << col);
        }
        
        key_count++;
    }

    // Debouncing: update only when state changed
    bool matrix_has_changed = false;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        if (matrix_debounced[i] != matrix[i]) {
            matrix_debounced[i] = matrix[i];
            matrix_has_changed = true;
        }
    }

    if (matrix_has_changed) {
        matrix_is_modified = true;
        matrix_last_modified = timer_read();
    }

    return key_count;
}

bool matrix_is_modified_debounce(uint16_t debounce) {
    bool is_modified = matrix_is_modified;

    // Reset modified flag when enough time has passed
    if (matrix_is_modified && timer_elapsed(matrix_last_modified) > debounce) {
        matrix_is_modified = false;
    }

    return is_modified;
}

bool matrix_is_on(uint8_t row, uint8_t col) {
    return (matrix_debounced[row] & (1 << col));
}

void matrix_power_up(void) {
    if (ADB_POWER_PORT) {
        gpio_put(ADB_POWER_PORT, 1);
    }
}

/* LED control for ADB keyboard */
void led_set(uint8_t usb_led) {
    led_states = 0;
    
    if (usb_led & (1<<USB_LED_CAPS_LOCK)) {
        led_states |= CAPS_LOCK;
    }
    if (usb_led & (1<<USB_LED_NUM_LOCK)) {
        led_states |= NUM_LOCK;
    }
    if (usb_led & (1<<USB_LED_SCROLL_LOCK)) {
        led_states |= SCROLL_LOCK;
    }
    
    led_set_adb(led_states);
}

static void led_set_adb(uint8_t bits) {
    adb_host_kbd_led(bits);
}