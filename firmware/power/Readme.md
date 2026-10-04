# power probe

```sh
bash firmware/build.sh power
```

Flash `.build/power/power.uf2` on a dedicated Pico 2 before storing credentials.
It replaces the running firmware. It has USB serial, no FIDO code and no OTP
provisioning. Reflashing does not undo OTP locks from an earlier signing image.
Leave the sensor disconnected.

This image pins Pico SDK 2.3.1. Signing and enrollment still use 2.1.1.
On USB suspend it switches the system and peripheral clocks to the crystal
reference, turns off PLL_SYS and sleeps until an interrupt. USB clocks and the
timer remain enabled. Resume restores the system clock.

Open USB serial and send `?`. The reply reports the configured system frequency
in Hz, not a measurement. Trigger actual USB bus suspend, then resume and send
`?` again. Check that serial returns without a reset or re-enumeration.

Measure VBUS current with a low-burden probe that preserves USB data. Capture
suspend entry and resume. Check the 2.5 mA suspend limit and 7 ms transition
in [the power notes](../../hardware/power.md). A sleeping computer alone does
not prove its USB port suspended this device.

The SDK serial descriptor declares 250 mA. This is a budget, not measured draw.
The image builds with warnings treated as errors. No physical power or wake
test yet. It is not a fix for the signing firmware's power use.
