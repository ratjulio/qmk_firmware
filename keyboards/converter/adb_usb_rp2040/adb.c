// ADB protocol implementation for RP2040
// Based on the original AVR implementation in the QMK repository

#include <string.h>

#include "quantum.h"
#include "adb.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/irq.h"

// ADB connection pin - configure to match your wiring
#ifndef ADB_DATA_PIN
#define ADB_DATA_PIN GP0
#endif

// Timer configuration
#define ADB_BIT_TIME      100  // microseconds for a bit cell
#define ADB_TIMEOUT       500  // microseconds for timeout
#define ADB_SETUP_TIME    15   // microseconds for setup

static inline void adb_io_reset(void);
static inline bool adb_in(void);
static inline void adb_out(bool val);
static inline void adb_high(void);
static inline void adb_low(void);
static bool adb_host_request(uint8_t addr, uint8_t cmd, uint8_t *buf, uint8_t *bufsize);

// ADB Line Control Functions

static inline void adb_io_reset(void) {
    // Configure as input with pull-up (idle state)
    gpio_init(ADB_DATA_PIN);
    gpio_set_dir(ADB_DATA_PIN, GPIO_IN);
    gpio_pull_up(ADB_DATA_PIN);
}

static inline bool adb_in(void) {
    return gpio_get(ADB_DATA_PIN);
}

static inline void adb_out(bool val) {
    if (val) {
        adb_high();
    } else {
        adb_low();
    }
}

static inline void adb_high(void) {
    // To set the line high, configure as input with pull-up (tri-state)
    gpio_set_dir(ADB_DATA_PIN, GPIO_IN);
    gpio_pull_up(ADB_DATA_PIN);
}

static inline void adb_low(void) {
    // To set the line low, configure as output and write 0
    gpio_set_dir(ADB_DATA_PIN, GPIO_OUT);
    gpio_put(ADB_DATA_PIN, 0);
}

// Time-critical delay function
static inline void adb_delay_us(uint32_t us) {
    sleep_us(us);
}

// ADB Protocol Implementation

// Send ADB command and wait for response
// buffer size should be 0-8
bool adb_host_kbd_recv(uint8_t *buf, uint8_t *bufsize) {
    return adb_host_request(ADB_ADDR_KEYBOARD, ADB_CMD_TALK, buf, bufsize);
}

bool adb_host_kbd_led(uint8_t led) {
    uint8_t buf[1];
    buf[0] = led;
    uint8_t bufsize = 1;
    return adb_host_request(ADB_ADDR_KEYBOARD, ADB_CMD_LISTEN, buf, &bufsize);
}

bool adb_host_mouse_recv(uint8_t *buf, uint8_t *bufsize) {
    return adb_host_request(ADB_ADDR_MOUSE, ADB_CMD_TALK, buf, bufsize);
}

// Send ADB command and receive the response
static bool adb_host_request(uint8_t addr, uint8_t cmd, uint8_t *buf, uint8_t *bufsize) {
    // Command: [Addr:2][Cmd:2][0:1][Register/Handler:3]
    uint8_t request = (addr << 4) | cmd;
    uint8_t count = 0;
    bool result;

    // Generate attention signal (800us low pulse)
    adb_low();
    adb_delay_us(800);
    adb_high();
    adb_delay_us(150);

    // Send command
    for (int i = 0; i < 8; i++) {
        bool bit = (request >> (7 - i)) & 1;
        if (bit) {
            // Send '1' bit (65us low, 35us high)
            adb_low();
            adb_delay_us(65);
            adb_high();
            adb_delay_us(35);
        } else {
            // Send '0' bit (35us low, 65us high)
            adb_low();
            adb_delay_us(35);
            adb_high();
            adb_delay_us(65);
        }
    }

    // Stop bit (65us low, 45us high)
    adb_low();
    adb_delay_us(65);
    adb_high();

    // Wait for device to respond
    uint32_t timeout_start = time_us_32();
    while (adb_in() == 1) {
        if (time_us_32() - timeout_start > ADB_TIMEOUT) {
            return false;  // timeout - no response
        }
    }

    // Read response
    result = true;
    *bufsize = 0;
    memset(buf, 0, 8);    // initialize with zero to get clear result

    // Try to receive 8 bytes with timeout
    for (int byte = 0; byte < 8; byte++) {
        uint8_t data = 0;
        for (int bit = 0; bit < 8; bit++) {
            // Wait for rising edge
            timeout_start = time_us_32();
            while (adb_in() == 0) {
                if (time_us_32() - timeout_start > ADB_TIMEOUT) {
                    if (byte > 0) {
                        *bufsize = byte;
                        return true;   // End of transmission
                    } else {
                        return false;  // Timeout without receiving data
                    }
                }
            }

            // Measure bit time
            uint32_t bit_start = time_us_32();
            
            // Wait for falling edge
            timeout_start = time_us_32();
            while (adb_in() == 1) {
                if (time_us_32() - timeout_start > ADB_TIMEOUT) {
                    if (byte > 0) {
                        *bufsize = byte;
                        return true;   // End of transmission
                    } else {
                        return false;  // Timeout without receiving data
                    }
                }
            }

            // Calculate bit value based on timing (short high = 1, long high = 0)
            uint32_t high_time = time_us_32() - bit_start;
            data <<= 1;
            if (high_time < 50) {
                data |= 1;
            }
        }
        buf[byte] = data;
        count++;

        // Check for stop bit (should be high for ~65us)
        timeout_start = time_us_32();
        while (adb_in() == 0) {
            if (time_us_32() - timeout_start > ADB_TIMEOUT) {
                *bufsize = count;
                return false;   // Missing stop bit
            }
        }
        
        // Check if device has more data (line should go low again within timeout)
        timeout_start = time_us_32();
        while (adb_in() == 1) {
            if (time_us_32() - timeout_start > ADB_TIMEOUT) {
                *bufsize = count;
                return true;    // No more data
            }
        }
    }

    *bufsize = count;
    return result;
}

// Initialize ADB host controller
void adb_host_init(void) {
    // Initialize ADB pin
    adb_io_reset();
    
    // Wait for ADB device to stabilize
    adb_delay_us(1000);
}