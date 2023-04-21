# project specific files
SRC = ../../../drivers/led/sn32/matrix_sn32f240.c

# MCU name
MCU = SN32F248F

# Build Options
#   comment out to disable the options.
#
BOOTMAGIC_ENABLE = yes      # Enable Bootmagic Lite
MOUSEKEY_ENABLE = no        # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
COMMAND_ENABLE = no         # Commands for debug and configuration
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
RGBLIGHT_ENABLE = no        # Enable keyboard RGB underglow
AUDIO_ENABLE = no           # Audio output
NKRO_ENABLE = yes           # Enable N-Key Rollover
CONSOLE_ENABLE = no         # Console for debug

# Custom matrix handling
CUSTOM_MATRIX = yes

# Custom RGB matrix handling
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = SN32F24x

# OpenRGB support
OPENRGB_ENABLE = yes

#custom debounce
DEBOUNCE_TYPE = sym_eager_pk
