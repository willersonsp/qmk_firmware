# Sharkoon SKILLER MECH SGK3

![SKILLER MECH SGK3](https://i.imgur.com/V9qxnfd.png)

A full-size 104-key(ansi), 105-key(iso, deutsch), 107-key(abnt2) keyboard made by Sharkoon, which controlled by an SN32F248F chipset. The keyboard features per-key RGB.

* Keyboard Maintainers: [HorrorTroll](https://github.com/HorrorTroll) / [willersonsp] (https://github.com/willersonsp)
* Hardware Supported: SN32F248F
* Hardware Availability: [Amazon](https://www.amazon.co.uk/Sharkoon-SKILLER-SGK3-Mechanical-Keyboard/dp/B077C1BH61)

## ANSI support:

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb sharkoon/sgk3 -km ansi

## ABNT2 support:

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb sharkoon/sgk3 -km abnt2

## ISO support:

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb sharkoon/sgk3 -km iso_default

## ISO-DE support:

Make example for this keyboard (after setting up your build environment):

    qmk compile -kb sharkoon/sgk3 -km iso_deutsch

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).

## Requirement:
Make sure you use arm-none-eabi-gcc 10.3+ toolchain, if you use 9.xx the keyboard may crash.

## Bootloader
* **Reset by wire**: Using a wire, 1 head to GND hole and other one to Boot pin on MCU
* **Keycode in layout**: Press the key mapped to RESET if it is available
