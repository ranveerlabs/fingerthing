#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=0}"
build=$(mktemp -d)
trap 'rm -rf "$build"' EXIT
"${CC:-cc}" -std=c11 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined \
    -Ifirmware/src firmware/src/r503.c firmware/tests/r503.c -o "$build/r503"
"$build/r503"
"${CC:-cc}" -std=c11 -Wall -Wextra -Werror -pedantic -fsanitize=address,undefined \
    -Ifirmware/src firmware/src/finger.c firmware/tests/finger.c -o "$build/finger"
"$build/finger"
