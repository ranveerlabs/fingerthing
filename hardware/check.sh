#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
mkdir -p .build/pcb
kicad-cli sch erc --format json --exit-code-violations \
    -o .build/pcb/erc.json hardware/pcb/fingerthing.kicad_sch
kicad-cli pcb drc --format json --schematic-parity --refill-zones \
    --severity-all --severity-exclusions --exit-code-violations \
    -o .build/pcb/drc.json hardware/pcb/fingerthing.kicad_pcb
