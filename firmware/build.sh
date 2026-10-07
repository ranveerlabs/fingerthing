#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.."
root=$PWD
mode=${1:-button}
pcb=OFF
power_pin=
case "$mode" in
    button) sensor=OFF; dir=pico2 ;;
    sensor) sensor=ON; dir=sensor ;;
    pcb) sensor=ON; pcb=ON; dir=pcb ;;
    enroll) dir=enroll ;;
    enroll-pcb) dir=enroll-pcb; power_pin=8 ;;
    power) dir=power ;;
    *) echo "Usage: $0 [button|sensor|pcb|enroll|enroll-pcb|power]" >&2; exit 1 ;;
esac
key=${SIGNING_KEY:-}
if [[ -n $key ]]; then
    [[ $key == /* && -f $key && -r $key ]] || {
        echo "SIGNING_KEY must be an absolute path to a readable PEM file" >&2
        exit 1
    }
    dir+=-signed
fi
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

get .build/pico-sdk-2.3.1 https://github.com/raspberrypi/pico-sdk.git \
    079c6f39023649b154152db30f1d781e884879bc
pico_sdk="$root/.build/pico-sdk-2.3.1"
picotool=(
    -DPICOTOOL_FORCE_FETCH_FROM_GIT=ON
    -DPICOTOOL_GIT_BRANCH=2041936441b48a3cc53ae3da9e805229fe8f4e18
    -DPICOTOOL_FETCH_FROM_GIT_PATH="$root/.build/picotool-2.3.1"
)
if [[ $mode == power ]]; then
    cmake --fresh -S firmware/power -B ".build/$dir" "${picotool[@]}" \
        -DPICO_SDK_PATH="$pico_sdk" -DPICO_BOARD=pico2 -DSECURE_BOOT_PKEY="$key"
    cmake --build ".build/$dir" --parallel "${BUILD_JOBS:-4}"
    exit
fi

if [[ $mode == enroll || $mode == enroll-pcb ]]; then
    cmake --fresh -S firmware/enroll -B ".build/$dir" "${picotool[@]}" \
        -DPICO_SDK_PATH="$pico_sdk" -DPICO_BOARD=pico2 \
        -DSECURE_BOOT_PKEY="$key" \
        -DFINGERTHING_SENSOR_POWER_PIN="$power_pin"
    cmake --build ".build/$dir" --parallel "${BUILD_JOBS:-4}"
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

cmake --fresh -S .build/pico-fido2 -B ".build/$dir" "${picotool[@]}" \
    -DPICO_SDK_PATH="$pico_sdk" -DPICO_BOARD=pico2 \
    -DFINGERTHING_DIR="$root/firmware" -DFINGERTHING_SENSOR="$sensor" \
    -DFINGERTHING_PCB="$pcb" \
    -DSECURE_BOOT_PKEY="$key" \
    -DENABLE_OATH_APP=OFF -DENABLE_OTP_APP=OFF -DDEBUG_APDU=0 \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build ".build/$dir" --parallel "${BUILD_JOBS:-4}"
python3 firmware/tests/button.py
python3 firmware/tests/gpio.py
python3 firmware/tests/presence.py
python3 firmware/tests/flash.py
python3 firmware/tests/pin.py
python3 firmware/tests/apps.py ".build/$dir/pico_fido2.elf"
bash firmware/test.sh
