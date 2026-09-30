# firmware

Pico 2 build of [librekeys/pico-fido2](https://github.com/librekeys/pico-fido2).
Source revisions are pinned in `build.sh`. Needs Git, CMake 3.19+, Make, ARM GCC
with Newlib, a native C compiler and Python 3.

```sh
bash firmware/build.sh
```

Output: `.build/pico2/pico_fido2.uf2`. Hold BOOTSEL while plugging in a dedicated
Pico 2, then copy the UF2 onto its USB drive. Upstream writes and locks key
material in OTP on first boot. This cannot be undone by reflashing.

The button patch requires a fresh press and release within 30 seconds. It removes
upstream's unconfigured-device bypass. Tests cover timeout, cancellation,
pre-held buttons and timer wrap. The dependency patch stops CMake from changing
Git settings or replacing its own crypto sources during configuration.

Built for Pico 2 and checked with picotool. No hardware or npm test yet.
Secure boot is not enabled. Fingerprint matching is not connected yet.
