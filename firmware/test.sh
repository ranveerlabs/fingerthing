#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=0}"
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT
check() {
    local name=$1
    shift
    "${CC:-cc}" -std=c11 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined \
        -Ifirmware/tests/include -Ifirmware/src "$@" -o "$build/$name"
    "$build/$name"
}
check r503 firmware/src/r503.c firmware/tests/r503.c
check finger firmware/src/finger.c firmware/src/bus.c firmware/tests/finger.c
check enroll firmware/src/enroll.c firmware/tests/enroll.c
check uart firmware/src/r503.c firmware/src/uart.c firmware/tests/uart.c
check uart-power -DFINGERTHING_SENSOR_POWER_PIN=8 firmware/src/r503.c firmware/src/uart.c firmware/tests/uart.c
check port firmware/src/port.c firmware/tests/port.c
