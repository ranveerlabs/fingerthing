# hardware

Pico 2 for bring-up. Get registration and sign-in working with pico-fido2 and the
button before adding a fingerprint sensor. npm is the first real-service test.

## sensor

Use the 3.3 V R503 variant in [the sensor notes](sensor.md). Match on the module,
send the result over UART. Confirm the supplier's variant and cable before ordering.

A fresh match can gate user presence. It must not set FIDO's UV flag by itself.
Keep the upstream PIN path. Fingerprint UV needs enrollment authorization,
retry limits, reset handling and CTAP integration. PIN fallback does not prevent
someone injecting a fake match on the UART wires.

## board

- RP2350A, 12 MHz crystal, QSPI flash. Start from Raspberry Pi's reference circuit.
- USB-C, separate 5.1k CC pull-downs, ESD protection, 90 ohm differential routing.
- 3.3 V supply plus the RP2350 core regulator circuit and reference decoupling.
- Sensor connector, BOOTSEL, status LED and SWD pads.
- Separate presence button on GP6, pulled up to 3.3 V through 10k.
- Four layers preferred. Assembly service for the QFN.

The [BOM](pcb/bom.csv) selects parts, but assembly stock is not confirmed.
AP2112K-3.3TRG1 supplies 3.3 V. [USB suspend](power.md) still needs firmware and
measurements. USB series termination follows the RP2350 reference. The core
supply needs more than an external 3.3 V LDO.

OTP is storage, not automatic key protection. Secure boot, debug restrictions,
key provisioning and silicon revision all matter. Upstream initializes and locks key storage on first boot. Use a dedicated
bring-up board. Secure-boot provisioning waits until updates and recovery work.

## references

- [immurok hardware](https://github.com/immurok/hardware), reference for sensor clearance,
  separate service controls and labeled pads. Its USB-C is charging-only.
- [RP2350 design files and hardware guide](https://pip.raspberrypi.com/categories/1214-rp2350)
- [RP2350 security features](https://pip-assets.raspberrypi.com/categories/1260-security/documents/RP-009377-WP-1-Understanding%20RP2350_s%20security%20features.pdf)
- [npm security-key setup](https://docs.npmjs.com/configuring-two-factor-authentication/)
