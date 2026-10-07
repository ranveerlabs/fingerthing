from pathlib import Path
import hashlib
import json
import re
import subprocess
import sys

root = Path(__file__).resolve().parents[2]
tool = root / '.build/picotool-2.3.1/picotool/picotool'
key, image = map(Path, sys.argv[1:])
pub = subprocess.check_output(['openssl', 'ec', '-in', str(key), '-pubout',
                               '-conv_form', 'uncompressed', '-outform', 'DER'])[-64:]
assert len(pub) == 64
info = subprocess.check_output([str(tool), 'info', '-a', str(image)], text=True)
status = re.findall(r'^\s*(hash|signature):\s*(\w+)\s*$', info, re.M)
assert {name for name, value in status} == {'hash', 'signature'}
assert all(value == 'verified' for name, value in status), info
keys = re.findall(r'^\s*public key:\s*([A-Fa-f0-9]+)', info, re.M)
assert keys and all(bytes.fromhex(value) == pub for value in keys)
plan = json.loads(image.with_suffix('.otp.json').read_text())
assert bytes(plan['bootkey0']) == hashlib.sha256(pub).digest()
assert plan['boot_flags1'] == {'key_valid': 1}
assert plan['crit1'] == {'secure_boot_enable': 1}
assert set(plan) == {'bootkey0', 'boot_flags1', 'crit1'}
print('signed: image verifies with the supplied key, OTP plan matches and contains only boot fields')
