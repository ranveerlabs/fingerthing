#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
root=$PWD
mode=${1:-button}
case "$mode" in
    button) sensor=OFF; dir=pico2 ;;
    sensor) sensor=ON; dir=sensor ;;
    enroll) dir=enroll ;;
    *) echo "Usage: $0 [button|sensor|enroll]" >&2; exit 1 ;;
esac
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

get .build/pico-sdk https://github.com/raspberrypi/pico-sdk.git \
    bddd20f928ce76142793bef434d4f75f4af6e433
if [[ $mode == enroll ]]; then
    cmake -S firmware/enroll -B .build/enroll \
        -DPICO_SDK_PATH="$root/.build/pico-sdk" -DPICO_BOARD=pico2
    cmake --build .build/enroll --parallel "${BUILD_JOBS:-4}"
    bash firmware/test.sh
    exit
fi
get .build/pico-fido2 https://github.com/librekeys/pico-fido2.git \
    391252e5e84b475add357ea0d3e5c79b7e42ace8

sdk="$root/.build/pico-fido2/pico-keys-sdk"
rev=e185d7fd85499c8ce5ca2a54f5cf8fe7dbe3f8df
if [[ $(git -C "$sdk/mbedtls" rev-parse HEAD) != "$rev" ]]; then
    if ! git -C "$sdk/mbedtls" cat-file -e "$rev^{commit}" 2>/dev/null; then
        git -C "$sdk/mbedtls" fetch --depth 1 origin "$rev"
    fi
    git -C "$sdk/mbedtls" checkout --detach "$rev"
fi
for part in sdk fido root; do
    case "$part" in
        sdk) src=$sdk ;;
        fido) src="$root/.build/pico-fido2/pico-fido" ;;
        root) src="$root/.build/pico-fido2" ;;
    esac
    for patch in firmware/patches/"$part"/*.patch; do
        if git -C "$src" apply --check "$root/$patch" 2>/dev/null; then
            git -C "$src" apply "$root/$patch"
        else
            git -C "$src" apply --reverse --check "$root/$patch"
        fi
    done
done

cmake -S .build/pico-fido2 -B ".build/$dir" \
    -DPICO_SDK_PATH="$root/.build/pico-sdk" -DPICO_BOARD=pico2 \
    -DFINGERTHING_DIR="$root/firmware" -DFINGERTHING_SENSOR="$sensor" \
    -DENABLE_OATH_APP=OFF -DENABLE_OTP_APP=OFF -DDEBUG_APDU=0 \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build ".build/$dir" --parallel "${BUILD_JOBS:-4}"
python3 firmware/tests/button.py
python3 firmware/tests/presence.py
bash firmware/test.sh
