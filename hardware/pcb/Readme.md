# pcb

KiCad 10. 34 x 48 mm, four-layer draft based on Raspberry Pi's RP2350A R4/S1
reference. USB-C, R503 header, GP6 presence button and rear SWD connection.
Flash, crystal and core regulator routing retained from the reference.

Power, USB CC, sensor, button, LED and SWD connections are routed. ERC passes and the PCB
matches the schematic. DRC reports eight unconnected USB data items and no other violations.
Stackup is preliminary,
USB impedance is not verified, and the R503 cable fit is not confirmed. Do not order this.

J1 is JAE DX07S016JA1R1500. The GCT part's locator-hole clearance was too small
for the board rules. Its replacement uses KiCad's JAE footprint, with the two
shell reinforcement pads grounded and the edge silk markers removed.

Reference symbols and footprints are local. New parts use KiCad's standard
libraries. `bash hardware/check.sh` runs ERC and DRC, including unfinished routing.

[reference](https://datasheets.raspberrypi.com/rp2350/Minimal-KiCAD.zip) · [license](LICENSE.raspberrypi)

[USB footprint](https://gitlab.com/kicad/libraries/kicad-footprints/-/blob/master/Connector_USB.pretty/USB_C_Receptacle_JAE_DX07S016JA1R1500.kicad_mod) · [license](LICENSE.kicad)
