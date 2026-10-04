# power

Signing firmware declares a 100 mA USB budget. The inherited descriptor declared
2 mA. Neither value is a measurement. Check draw before configuration, during
registration and signing, with the sensor lit and during suspend.

The current NCP1117 does not provide a suitable suspend current budget.
[onsemi's specification](https://www.onsemi.com/pdf/datasheet/ncp1117-d.pdf)
gives 6 mA typical quiescent current for the 3.3 V part at 15 V input.
USB supplies 5 V, so this is not a measured draw for this board.

[TinyUSB's suspend callback](https://github.com/hathach/tinyusb/blob/0.18.0/src/device/usbd.h)
requires less than 2.5 mA average bus draw within 7 ms. There is no suspend
callback in this firmware yet. Replace the regulator with a low idle current
part, implement suspend and resume handling, then measure the whole board.

Do not order the custom board until these changes and checks are complete.
