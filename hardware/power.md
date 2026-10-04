# power

Signing firmware declares a 100 mA USB budget. The inherited descriptor declared
2 mA. Neither value is a measurement. Check draw before configuration, during
registration and signing, with the sensor lit and during suspend.

U2 is AP2112K-3.3TRG1. Its [datasheet](https://www.diodes.com/assets/Datasheets/AP2112.pdf)
specifies 55 uA typical and 80 uA maximum idle current at 4.3 V input with
no load. It replaces the NCP1117. These figures do not include the rest of the board.

Enable is tied to VBUS. Pin 4 is unconnected. C1 and C5 are 10 uF ceramic
capacitors beside the input and output. Check inrush and VBUS dip at plug-in,
as well as steady current and regulator temperature in the closed case.

[TinyUSB's suspend callback](https://github.com/hathach/tinyusb/blob/0.18.0/src/device/usbd.h)
requires less than 2.5 mA average bus draw within 7 ms. The callback cancels
pending approval but does not reduce draw. MCU and sensor power management,
resume handling and whole-board measurements remain.

The [power probe](../firmware/power/Readme.md) tests MCU clock reduction on a
Pico 2 with the sensor disconnected. The board currently connects R503 main
power directly to 3.3 V. Its separate touch supply does not switch that main
power off. A sensor power switch and backfeed checks are still needed.

Do not order the custom board until these changes and checks are complete.
