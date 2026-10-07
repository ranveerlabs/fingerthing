# fingerthing

i wanted touchid but too broke for a maccc

USB fingerprint security key for passkeys and 2FA. RP2350, a 3.3 V R503 sensor
and [pico-fido2](https://github.com/librekeys/pico-fido2) firmware.

![fingerthing board front](docs/img/board.svg)

## why

I want fingerprint approval for my npm account and other FIDO2 sites, using
the same key across laptops. Linux, Windows, macOS and
[FreeBSD](host/freebsd/readme.md) are targets. sudo through PAM is another target.

## the key

The sensor matches on-chip and sends the result over UART. A fresh match
approves user presence. FIDO PIN handles verification and allows button
fallback. The fingerprint never sets UV, and UART matches can be spoofed.

Pico 2 comes first for bring-up. The custom board has USB-C, switched sensor
power, a separate presence button, BOOTSEL, a status LED and SWD pads.

![fingerthing case design](case/preview.png)

## build and test

The [firmware guide](firmware/Readme.md) lists the tools and image paths.
Use a dedicated development board. Signing firmware locks OTP storage on
first boot, and reflashing cannot undo that.

```sh
bash firmware/test.sh
bash firmware/build.sh button
bash firmware/build.sh sensor
bash firmware/build.sh pcb
```

[Parts](docs/BOM.md) and [bring-up steps](docs/bringup.md) cover wiring and
enrollment. [Hardware notes](hardware/DESIGN.md) cover the board and sensor.

## status

The 34 x 48 mm four-layer PCB is routed. ERC, DRC and schematic parity pass
with no violations or unconnected items. Firmware builds and host tests pass.
Nothing has been tested on hardware. npm, sudo and OS compatibility remain
unverified. USB power, secure boot and [case fit](case/ReadMe.md) still need
physical tests. The PCB and case are drafts.

Changes and test results are in the [journal](JOURNAL.md).

## license

[CERN-OHL-S-2.0](LICENSE). Firmware is [AGPL-3.0-only](firmware/LICENSE),
including the pico-fido2 patches. Third-party files retain their own licenses :p
