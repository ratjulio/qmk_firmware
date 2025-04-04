/* ADB protocol definitions for RP2040
 * Based on the original AVR implementation in the QMK repository
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/* ADB commands */
#define ADB_CMD_RESET      0
#define ADB_CMD_FLUSH      1
#define ADB_CMD_LISTEN     2
#define ADB_CMD_TALK       3

/* ADB default addresses */
#define ADB_ADDR_DONGLE    1
#define ADB_ADDR_KEYBOARD  2
#define ADB_ADDR_MOUSE     3
#define ADB_ADDR_TABLET    4
#define ADB_ADDR_MODEM     5
#define ADB_ADDR_RESERVED  6
#define ADB_ADDR_MISC      7

/* ADB keyboard registers */
#define ADB_REG_KBD_MAP    0
#define ADB_REG_KBD_LED    2

/* ADB LED bitmasks */
#define ADB_LED_CAPS       0x02
#define ADB_LED_NUM        0x01
#define ADB_LED_SCROLL     0x04

/* ADB Error codes */
#define ADB_ERR_TIMEOUT    0x1
#define ADB_ERR_COLLISION  0x2

/* Initialize ADB host controller */
void adb_host_init(void);

/* Send command to ADB keyboard */
bool adb_host_kbd_recv(uint8_t *buf, uint8_t *bufsize);

/* Set LEDs on ADB keyboard */
bool adb_host_kbd_led(uint8_t led);

/* Send command to ADB mouse */
bool adb_host_mouse_recv(uint8_t *buf, uint8_t *bufsize);