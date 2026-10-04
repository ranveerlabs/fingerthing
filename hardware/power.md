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
pending approval. PCB request cleanup releases UART and cuts sensor power.
MCU sleep, transition timing and whole-board measurements remain.

The [power probe](../firmware/power/Readme.md) tests MCU clock reduction on a
Pico 2, then sensor switching with an external circuit. The draft PCB now
switches both R503 supplies with GP8. The `pcb` signing image powers the sensor
for each request and turns it off afterward.
Backfeed checks and physical power measurements remain.

Do not order the custom board until these changes and checks are complete.

## sensor switch

U5 is [TPS22919DCKR](https://www.ti.com/lit/ds/symlink/tps22919.pdf), SC-70-6.
Checked the datasheet's top-view pin drawing. The same connections apply to
an external prototype circuit on Pico 2.

| Switch pin | Connection |
| --- | --- |
| 1 IN | Pico 2 3V3, 1 uF ceramic to ground beside the switch |
| 2 GND | Common ground |
| 3 ON | GP8, 100k to ground |
| 4 NC | Unconnected |
| 5 QOD | Pin 6 OUT |
| 6 OUT | R503 pins 1 and 6, 100 nF ceramic to ground |

Power both sensor supplies through the switch. Touch wake is unused. Connect
sensor TX to GP5 and RX to GP4. Put 10k from sensor RX to the switched supply,
so RX stays high while the probe leaves GP4 as an input. GP7 may connect to
finger detection. Check cable pin order first.

The probe disables the switch at startup, USB suspend and disconnect. Measure
the switched rail voltage and VBUS draw with power off, then check inrush when
it turns on. Include the sensor's internal capacitance. The signing image waits
250 ms before enabling UART and releases both pins before power-off. Test cold
startup, rapid repeated requests and cancellation during startup on the exact
sensor variant. No physical power-cycle tests yet.
