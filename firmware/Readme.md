# firmware

[librekeys/pico-fido2](https://github.com/librekeys/pico-fido2) on Pico 2.
Pinned sources. Git, CMake, Make, ARM GCC with Newlib, native C compiler, Python 3.

```sh
bash firmware/build.sh
bash firmware/build.sh sensor
bash firmware/build.sh pcb
bash firmware/build.sh enroll
```

UF2: `.build/pico2/pico_fido2.uf2` or `.build/sensor/pico_fido2.uf2`.
The custom board build is `.build/pcb/pico_fido2.uf2`. It uses an active-low
button on GP6 with a 20 ms debounce. BOOTSEL stays available for recovery.
BOOTSEL while plugging in, then copy the UF2. Use a dedicated development board:
upstream writes and locks OTP keys on first boot. Reflashing cannot undo this.

Button: fresh press and release. Sensor: fresh match in slot 0, one attempt per
request. Button fallback requires a verified PIN for that request. Both time out
after 30 seconds. Fingerprint approval never sets UV. OTP, OATH and debug output
are disabled. Secure boot is not enabled.

The signing builds declare 100 mA USB power. Actual draw and USB suspend are
unverified, see [power](../hardware/power.md).

The separate [power probe](power/Readme.md) builds with `bash firmware/build.sh power`.

Approval starts only while USB is configured and awake. Suspend, disconnect or
reconfiguration cancels pending button and fingerprint approval, including PIN
fallback. Resuming does not clear that cancellation. These callbacks do not
put the MCU or sensor into a low power state.

R503: 3.3V and GND, sensor RX to GP4, TX to GP5. UART1, 57600 baud.
Enrollment: flash `.build/enroll/enroll.uf2` with BOOTSEL, open its USB serial
port, type `e`, then `y`. Lift and scan the same finger twice. Esc cancels.
This development image replaces slot 0 and has a 60-second timeout. Flash the
sensor firmware afterward. Use it before storing credentials. Enrollment has no FIDO code. Signing firmware has no enrollment command.

UART matches can be spoofed. Enrollment has not been tested on hardware.

Builds and host tests pass. No hardware, npm or FreeBSD test yet.
