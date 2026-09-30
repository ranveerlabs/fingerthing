#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
root=$PWD
mkdir -p .build

get() {
    local dir=$1 url=$2 rev=$3
    if [[ ! -d "$dir/.git" ]]; then
        git init "$dir"
        git -C "$dir" remote add origin "$url"
        git -C "$dir" fetch --depth 1 origin "$rev"
        git -C "$dir" checkout --detach FETCH_HEAD
    fi
    [[ $(git -C "$dir" rev-parse HEAD) == "$rev" ]] || {
        echo "Wrong revision: $dir" >&2
        exit 1
    }
    git -C "$dir" submodule update --init --recursive --depth 1
}

get .build/pico-fido2 https://github.com/librekeys/pico-fido2.git \
    391252e5e84b475add357ea0d3e5c79b7e42ace8
get .build/pico-sdk https://github.com/raspberrypi/pico-sdk.git \
    bddd20f928ce76142793bef434d4f75f4af6e433

sdk="$root/.build/pico-fido2/pico-keys-sdk"
rev=e185d7fd85499c8ce5ca2a54f5cf8fe7dbe3f8df
if [[ $(git -C "$sdk/mbedtls" rev-parse HEAD) != "$rev" ]]; then
    if ! git -C "$sdk/mbedtls" cat-file -e "$rev^{commit}" 2>/dev/null; then
        git -C "$sdk/mbedtls" fetch --depth 1 origin "$rev"
    fi
    git -C "$sdk/mbedtls" checkout --detach "$rev"
fi
for patch in firmware/patches/*.patch; do
    if git -C "$sdk" apply --check "$root/$patch" 2>/dev/null; then
        git -C "$sdk" apply "$root/$patch"
    else
        git -C "$sdk" apply --reverse --check "$root/$patch"
    fi
done

cmake -S .build/pico-fido2 -B .build/pico2 \
    -DPICO_SDK_PATH="$root/.build/pico-sdk" -DPICO_BOARD=pico2 \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build .build/pico2 --parallel "${BUILD_JOBS:-4}"
python3 firmware/tests/button.py
