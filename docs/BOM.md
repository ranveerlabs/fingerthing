# parts

| part | quantity | status |
|---|---:|---|
| [Pico 2, Adafruit #6006](https://www.adafruit.com/product/6006) | 1 | development board, solder two 20-pin headers |
| [USB A to micro-B data cable, #592](https://www.adafruit.com/product/592) | 1 | for Pico 2 |
| [3.3 V R503, #4651](https://www.adafruit.com/product/4651) | 1 | UART, supplied sensor cable |
| [830-point breadboard, #239](https://www.adafruit.com/product/239) | 1 | prototype wiring |
| [Male/male jumpers, #759](https://www.adafruit.com/product/759) | 1 pack | breadboard connections |

These are bring-up parts. Adafruit's Pico 2 listing still specifies A2 silicon.
Use it for development only. Require A4 for the final board and verify the chip
marking before provisioning. [A4 fixes GPIO leakage and several security defects](https://www.raspberrypi.com/news/rp2350-a4-rp2354-and-a-new-hacking-challenge/).
The signing build still uses SDK 2.1.1. A4 toolchain support and secure boot need work.

The sensor cable needs soldered 2.54 mm header pins for the breadboard. Check
pin numbers with the [sensor notes](../hardware/sensor.md), not wire colours.
Its plug is not verified as compatible with the PCB's JST SH socket.

The final board uses USB-C. Nothing ordered yet. A soldering iron, solder,
wire stripper and multimeter are needed for this prototype.

The [board BOM](../hardware/pcb/bom.csv) comes from the schematic. It lists
47 fitted components with manufacturer part numbers. R1 is DNP and the four
mounting holes are excluded. Assembly stock has not been confirmed.

The board uses AP2112K-3.3TRG1 for 3.3 V. USB suspend and current measurements
still need work. Sensor and enclosure fit remain unverified. Do not use this
BOM to order a board yet.
