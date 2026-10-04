# pcb

KiCad 10. 34 x 48 mm, four-layer draft based on Raspberry Pi's RP2350A R4/S1
reference. USB-C, R503 header, GP6 presence button and rear SWD connection.
Flash, crystal and core regulator routing retained from the reference.

ERC passes and the PCB matches the schematic. Power and new connections still
need routing. USB-C footprint hole clearance is unresolved. Stackup is preliminary,
USB impedance is not verified, and the R503 cable fit is not confirmed. Do not order this.

Reference symbols and footprints are local. New parts use KiCad's standard
libraries. `bash hardware/check.sh` runs ERC and DRC, including unfinished routing.

[reference](https://datasheets.raspberrypi.com/rp2350/Minimal-KiCAD.zip) · [license](LICENSE.raspberrypi)
