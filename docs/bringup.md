# bring-up

Start with the [parts list](BOM.md) and a dedicated development board. Nothing
has been tested on hardware. The signing firmware locks OTP storage on first
boot, and reflashing cannot undo that.

1. Solder the Pico 2 headers and the supplied sensor cable's breadboard pins.
   Leave the sensor disconnected. Build and flash the
   [power probe](../firmware/power/Readme.md). Check USB serial, suspend and resume.
2. Build the button image with `bash firmware/build.sh button`. Hold BOOTSEL
   while plugging in, then copy `.build/pico2/pico_fido2.uf2`. Check that
   `fido2-token -L` finds it. Use BOOTSEL for a fresh press and release when
   requested. Test registration, sign-in and reconnect with a disposable
   WebAuthn credential before adding the sensor.
3. Unplug USB. Connect the 3.3 V R503 using the table below. Verify cable
   continuity and pin order. Leave its finger-detection wire insulated.
4. Build and flash `enroll`, open USB serial, then type `e` and `y`. Lift and
   scan the same finger twice. Continue only after it reports `saved`.
5. Build and flash `sensor`. Test a matching finger, a different finger,
   cancellation, disconnect during a request and a fresh request after reconnect.
   Test PIN fallback with BOOTSEL too. A match supplies presence, never UV.
6. Once those checks pass, follow [npm's security-key setup](https://docs.npmjs.com/configuring-two-factor-authentication/).
   Name it `fingerthing`, retain another working key and save recovery codes
   separately. Test a fresh sign-in after unplugging and reconnecting it.

| R503 pin | Pico 2 connection | Physical pin |
| --- | --- | --- |
| 1, power | 3V3(OUT) | 36 |
| 2, ground | GND | 8 |
| 3, TXD | GP5, UART1 RX | 7 |
| 4, RXD | GP4, UART1 TX | 6 |
| 5, finger detection | Unconnected | |
| 6, touch supply | 3V3(OUT) | 36 |

Use the [Pico 2 pinout](https://datasheets.raspberrypi.com/pico/Pico-2-Pinout.pdf)
and [sensor pinout](../hardware/sensor.md). This wiring uses constant sensor
power. It does not use the PCB's GP8 switch or the `pcb` firmware profile.

Record the board stepping, sensor variant, firmware commit, OS and browser
versions with each result. Repeat the host checks on FreeBSD using its
[access rule](../host/freebsd/readme.md). Custom-board assembly, switched-power
tests, secure boot and case fit remain separate hardware gates.
