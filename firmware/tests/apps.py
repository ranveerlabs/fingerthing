from pathlib import Path
import subprocess
import sys

src = Path('.build/pico-fido2/pico-keys-sdk/src/rescue.c').read_text()
assert 'register_app(rescue_select, rescue_aid)' not in src
symbols = subprocess.check_output(['arm-none-eabi-nm', sys.argv[1]], text=True)
names = {line.split()[-1] for line in symbols.splitlines() if line.split()}
assert {'fido_ctor', 'u2f_ctor'} <= names
assert not any(name.startswith('rescue_') for name in names)
assert not {'cmd_secure', 'cmd_reboot_bootsel', 'otp_enable_secure_boot'} & names
print('apps: FIDO constructors remain, rescue app and boot-fuse writer absent from image')
