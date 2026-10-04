# fingerthing build journal

## session 001 - 2026-09-27

Started the repository with an AGPL-3.0-only license and folders for hardware,
firmware, the case and docs, following the structure of MorphCPU.

The goal is a fingerprint device that can be used as a passkey across laptops
and operating systems. The sensor, controller, connection and credential storage
are still undecided. There is no prototype or compatibility testing yet.

The next step is to work out the passkey requirements and compare parts before
starting the schematic. Fingerprint enrollment, matching, credential storage
and recovery need a design before firmware work starts.

Session time was not recorded.

## session 002 - 2026-09-27

Compared a plug-in security key with a separate fingerprint button. The proposed
first version uses a USB cable so the sensor can sit beside the keyboard. It
would still behave as a security key through CTAP2 over USB HID.

Added [design notes](hardware/DESIGN.md) with the authentication boundary, parts
to evaluate and prototype checks. Reviewed the FIDO CTAP and W3C WebAuthn
specifications. A touch establishes presence, while fingerprint matching or a
PIN provides user verification. Browser passkeys and OS login need separate
compatibility work.

No components were selected or tested. Session time was not recorded.

## session 003 - 2026-09-27

Narrowed the first target to 2FA on my npm account. Checked npm's documentation:
security keys are registered on the website through WebAuthn and can also be
used for CLI authentication. Updated the design with npm registration, fresh
sign-in and the `npm login` browser flow as the first acceptance checks.

Discoverable credentials are still useful for broader passkey support, but
npm's exact WebAuthn requirements have not been observed. No account settings
were changed and no hardware was tested. Session time was not recorded.

## session 004 - 2026-09-30

Switched the build plan to Pico 2 and librekeys/pico-fido2. R503 is the sensor
candidate. Removed the SEN0348 code and enclosure from the active tree, cut the
readmes, and added a FreeBSD USB permission rule. No hardware tests yet.

The upstream startup code writes key material into OTP and locks those pages.
A first flash is not reversible just by replacing the firmware. Secure boot is
a separate step.

## session 005 - 2026-09-30

Built librekeys/pico-fido2 for Pico 2 with SDK 2.1.1. Upstream's button check can
return success before configuration, so this build requires a fresh press and
release. Timeout, cancellation, pre-held button and timer-wrap tests pass.

The UF2 is 873472 bytes. Picotool identifies Pico 2, RP2350 ARM Secure, firmware
7.4 and a verified image hash. Secure boot is not enabled. Nothing flashed yet.

Pinned Mbed TLS to the upstream build's 3.6.5 revision and removed CMake's Git
configuration and dependency-replacement steps.

## session 006 - 2026-10-02

Added R503 packet handling and a fingerprint presence gate. Slot 0 only, fresh
touch, one match attempt per request. Button fallback needs that request's PIN.
Malformed replies, sensor errors, held inputs and cancellation fail closed.
Host tests cover timer wrap and replies arriving after the request timeout.

Registration could skip presence without a PIN. Patched registration and
assertion to check each request, and removed cached verification from registration.
Fingerprint matching does not set UV. Disabled unused OTP, OATH and debug output.
Both Pico 2 builds pass. Picotool verifies the sensor image hash.
Enrollment and hardware tests are still pending. No custom PCB yet.

## session 007 - 2026-10-04

Added a separate USB serial enrollment image. Explicit confirmation, two fresh
captures, slot 0 only. Capture or merge failure does not request a template write.
The signing image has no enrollment command. Enrollment is for development,
before storing credentials.

Shared the UART transport and removed the unused delete command. Host tests cover
RX floods, malformed lengths, late replies, cancellation and timer wrap. Enrollment
and signing builds pass. Nothing tested on hardware yet.
