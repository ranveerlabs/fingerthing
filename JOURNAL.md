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

## session 008 - 2026-10-04

Started the KiCad project from Raspberry Pi's RP2350A R4/S1 reference. Included
local symbols, footprints and embedded models, with the original MIT license.
Fixed the regulator tab's ERC pin type and declared the two filtered supply nets.
Mounting-hole drills and positions are unchanged.

ERC and DRC pass with the project rules, including board/schematic parity.
USB-C, the sensor connector and the final layout are still pending. This is a
reference base, not an orderable fingerthing board.

## session 009 - 2026-10-04

Read immurok's hardware notes and board renders. Use sensor clearance and labeled
service pads as layout references. Keep USB FIDO2 for npm. Immurok uses Bluetooth
with companion apps and its USB-C connector only supplies charging power.

Added the custom board build with a separate presence button on GP6 and 20 ms
debounce. BOOTSEL remains the recovery button. Host tests cover held startup,
contact bounce and timer wrap. Custom board and Pico 2 sensor builds pass.
Picotool verifies the custom image hash. No hardware test yet.

## session 010 - 2026-10-04

Replaced the development headers and micro USB with USB-C, CC pull-downs, ESD
protection, a sensor header, GP6 button and status LED. The draft is 34 x 48 mm
with rounded corners and four copper layers. Sensor and SWD connections are
underneath. Kept the reference flash, crystal and core regulator routing.

ERC passes and board/schematic parity has no issues. DRC still reports 88
unconnected items and four USB connector hole-clearance violations. The stackup
is preliminary. Routing, USB impedance, sensor cable fit and physical tests remain.

## session 011 - 2026-10-04

Changed USB-C to JAE DX07S016JA1R1500. The GCT footprint matched its drawing but
left 0.1944 mm between its locator holes and ground pads. Kept the 0.25 mm hole
clearance rule. The local JAE footprint grounds both shell reinforcement pads.

Added ground planes, a 3.3 V plane and supply vias. Removed floating copper.
ERC and schematic parity pass. DRC has 24 unconnected items and no other
violations. USB, sensor, button, LED and SWD routing remain. No hardware test yet.

## session 012 - 2026-10-04

Routed the GP6 presence button, its pull-up and the GPIO25 status LED. Moved one
3.3 V via to leave room for the LED signal. ERC and schematic parity pass.
DRC has 19 unconnected items and no other violations. USB, sensor and SWD
routing remain. The board is still a draft.

## session 013 - 2026-10-04

Routed GP4, GP5 and GP7 to the sensor header and SWD to the rear connector.
The sensor signals use the power layer to pass the retained core regulator
routing. Added a supply bridge where the routes split the 3.3 V fill.

ERC and schematic parity pass. DRC has 14 unconnected USB items and no other
violations. USB routing and impedance, sensor cable fit and hardware tests remain.

## session 014 - 2026-10-04

Connected both USB-C power banks, the ESD supply and regulator input. Routed
CC1 and CC2 to their separate 5.1k pull-downs. Power tracks are 0.5 mm wide.

ERC and schematic parity pass. DRC has eight unconnected USB data items and
no other violations. The data pair and final impedance still need work.

## session 015 - 2026-10-04

Routed USB through the ESD protector and turned it toward the MCU. Joined both
connector orientations and shortened the protector's ground connection.

JLCPCB's calculator returned 0.2347 mm for a 90 ohm coplanar differential pair
on JLC04161H-7628, with 0.15 mm spacing and 0.25 mm ground clearance. Used
0.235 mm and updated the copper and dielectric thicknesses to that stackup.
The connector joins and ESD fan-out are not fully coupled.

ERC, DRC and board/schematic parity pass with zero unconnected items. No
hardware test yet. Final fabrication review, sensor cable fit and enclosure
work remain.

## session 016 - 2026-10-04

Added C20, a 100 nF VBUS bypass beside the ESD protector's supply via, following
ST's layout example. The capacitor sits underneath and has a nearby ground via.
ERC, DRC and schematic parity pass with zero unconnected items.

Reviewed GROW's 2019.6 R503 manual. The board's six-pin mapping matches its
3.3 V variant. Documented the 28 mm diameter, M25 thread and 19 mm height.
The manual names an MX1.0 sensor plug while J2 uses JST SH. The adapter cable
and supplied sensor still need physical verification.

## session 017 - 2026-10-04

Added a rounded two-piece enclosure for the 34 x 48 mm board and documented
R503. The body is 39.4 x 53.4 x 33 mm. It has a separate button plunger,
underside screws and nut pockets. BOOTSEL and SWD require opening the case.

OpenSCAD 2021.01 exports three closed, connected meshes. Checked the shell
joint, sensor opening, board guides and button at rest and 0.6 mm down.
Intersections are empty or shared mounting surfaces without solid overlap.
Rendered the exported meshes. The assembly uses approximate component blocks.

Sensor flange and nut dimensions, cable routing, USB plug clearance and button
release still need measurements and a print fit check. No hardware tests yet.
