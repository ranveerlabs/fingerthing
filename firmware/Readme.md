# firmware

Base: [librekeys/pico-fido2](https://github.com/librekeys/pico-fido2), pinned to
`391252e5e84b475add357ea0d3e5c79b7e42ace8`. Pico 2 button first, sensor next.

The Pico 2 build is being checked. No device has been flashed.

Upstream writes and locks key material in OTP on first boot. Flashing it is not
a fully reversible trial. Secure-boot provisioning is a separate step.
