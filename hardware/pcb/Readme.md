# pcb

KiCad 10. 34 x 48 mm, four layers, rounded corners. RP2350A, USB-C, R503 header,
GP6 presence button and rear SWD connector. All nets are routed. ERC, DRC and
board/schematic parity pass with no exclusions.

USB targets 90 ohms on JLC04161H-7628. The coupled section uses 0.235 mm traces,
0.15 mm spacing and 0.25 mm clearance to the ground fill. JLCPCB's calculator
returned 0.2347 mm for those inputs on 2026-10-04.

Copper and dielectric thicknesses follow that stackup: 0.035 mm outer copper,
0.0152 mm inner copper, 0.2104 mm prepreg and a 1.065 mm core. Solder-mask and
material-property fields remain nominal. Connector joins and ESD fan-out are
not fully coupled. Final fabrication review and hardware testing remain.

J1 is JAE DX07S016JA1R1500. The local KiCad footprint grounds both shell
reinforcement pads and removes the edge silk markers. Flash, crystal and core
regulator routing come from Raspberry Pi's RP2350A R4/S1 reference. C20 adds a
100 nF VBUS bypass beside the ESD protector's supply via.

R503 cable fit and the enclosure are not confirmed. Do not order this yet.
`bash hardware/check.sh` runs ERC and DRC with KiCad's standard libraries installed.

[BOM](bom.csv): 42 fitted components. R1 is DNP. U2 is AP2112K-3.3TRG1,
with C1 and C5 beside its input and output. Suspend handling and current
measurements remain, see [power](../power.md).

[reference](https://datasheets.raspberrypi.com/rp2350/Minimal-KiCAD.zip) · [license](LICENSE.raspberrypi)

[USB footprint](https://gitlab.com/kicad/libraries/kicad-footprints/-/blob/master/Connector_USB.pretty/USB_C_Receptacle_JAE_DX07S016JA1R1500.kicad_mod) · [license](LICENSE.kicad)

[impedance calculator](https://jlcpcb.com/pcb-impedance-calculator) · [stackup](https://jlcpcb.com/impedance) · [ESD layout](https://www.st.com/resource/en/datasheet/usblc6-2.pdf)
