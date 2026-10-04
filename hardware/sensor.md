# sensor

The board follows GROW's R503 manual, 2019.6 version 1.1: 3.3 V power and UART,
28 mm outside diameter, M25 thread, 23.5 mm inner diameter and 19 mm height.
The supplier's module must match this variant before ordering or printing a case.

| Sensor pin | Function | J2 connection |
| --- | --- | --- |
| 1 | 3.3 V power | SENSOR_3V3 |
| 2 | Ground | GND |
| 3 | TXD | GP5, MCU RX |
| 4 | RXD | GP4, MCU TX |
| 5 | Finger detection | GP7 |
| 6 | Touch supply | SENSOR_3V3 |

U5 switches both supplies with GP8. R15 keeps the switch off at reset.
R16 pulls sensor RX up to the switched rail. Touch wake is unused.
The `pcb` signing and `enroll-pcb` development images control GP8. UART pins
return to inputs without pulls before cutting sensor power. This needs a
physical backfeed and power-cycle check.

UART defaults to 57600 baud, 8N1. J2 uses JST SH at 1.0 mm pitch. The manual calls
the sensor connector MX1.0-6P. Use a pin-for-pin adapter cable, with each plug
matched to its socket. Verify pin 1 and continuity before applying power.

[manufacturer manual](https://download.mikroe.com/documents/datasheets/R503_datasheet.pdf)
