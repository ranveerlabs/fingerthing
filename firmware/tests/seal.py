from pathlib import Path
import re
import struct
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
tool = root / '.build/picotool-2.3.1/picotool/picotool'


def check(path, expected):
    info = subprocess.check_output([str(tool), 'info', str(path)], text=True)
    status = dict(re.findall(r'^\s*(hash|signature):\s*(\w+)\s*$', info, re.M))
    assert status == {'hash': expected, 'signature': expected}, info


with tempfile.TemporaryDirectory() as tmp:
    p = Path(tmp)
    key = p / 'test.pem'
    subprocess.run(['openssl', 'ecparam', '-name', 'secp256k1', '-genkey', '-out', str(key)], check=True)
    for mode in ('pico2', 'sensor', 'pcb'):
        src = root / f'.build/{mode}/pico_fido2.uf2'
        signed = p / 'signed.uf2'
        subprocess.run([str(tool), 'seal', '--sign', '--hash', str(src), str(signed), str(key)],
                       check=True, stdout=subprocess.DEVNULL)
        check(signed, 'verified')
        data = bytearray(signed.read_bytes())
        for offset in range(0, len(data), 512):
            if struct.unpack_from('<I', data, offset + 12)[0] == 0x10002000:
                data[offset + 32] ^= 1
                break
        else:
            raise RuntimeError(f'Missing application block: {mode}')
        changed = p / 'changed.uf2'
        changed.write_bytes(data)
        check(changed, 'incorrect')
print('seal: signing images verify, altered application bytes fail hash and signature checks')
