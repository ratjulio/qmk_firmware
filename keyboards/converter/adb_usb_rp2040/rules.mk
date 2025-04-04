# Bootloader selection
BOOTLOADER = rp2040

# Build Options
BOOTMAGIC_ENABLE = no       # Enable Bootmagic Lite
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = yes        # Commands for debug and configuration
NKRO_ENABLE = yes           # USB Nkey Rollover - if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
AUDIO_ENABLE = no           # Audio output

# RP2040 specific options
PICO_INTRINSICS_ENABLED = no  # Enable RP2040 intrinsics
CONVERT_TO = promicro        # This is for QMK to recognize the correct pinout

# ADB support
SRC += adb.c matrix.c

# Custom matrix handling
CUSTOM_MATRIX = yes

# Debug options
VERBOSE = yes