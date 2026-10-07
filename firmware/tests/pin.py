import os
from pathlib import Path
import subprocess
import tempfile

src = Path('.build/pico-fido2/pico-fido/src/fido/cbor_client_pin.c').read_text()


def block(text, start):
    end = text.index('{', start)
    depth = 1
    while depth:
        end += 1
        depth += (text[end] == '{') - (text[end] == '}')
    return text[start:end + 1]


ecdh = block(src, src.index('int ecdh('))
guards = []
for cmd in (3, 4):
    branch = block(src, src.index(f'else if (subcommand == 0x{cmd})'))
    start = branch.index('if (pinUvAuthParam.len !=')
    assert start < branch.index('mbedtls_mpi_read_binary(') < branch.index('verify(')
    guards.append(f'static int check{cmd}(uint64_t pinUvAuthProtocol, size_t len) {{\n'
                  'struct { size_t len; } pinUvAuthParam = {len};\n'
                  + block(branch, start) + '\nreturn 0;\n}\n')

prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define CTAP2_ERR_PIN_AUTH_INVALID 0x33
#define CBOR_ERROR(err) return err
typedef int mbedtls_mpi;
typedef int mbedtls_ecp_point;
static struct { struct { struct { int grp, d; } mbed_ecdh; } ctx; } hkey;
static int shared_error, kdf_error, calls, freed;
static int random_gen;
static void mbedtls_mpi_init(mbedtls_mpi *z) { *z = 0; }
static void mbedtls_mpi_free(mbedtls_mpi *z) { (void)z; ++freed; }
static int mbedtls_ecdh_compute_shared(int *grp, mbedtls_mpi *z,
        const mbedtls_ecp_point *q, int *d, int rng, void *ctx) {
    assert(grp == &hkey.ctx.mbed_ecdh.grp && d == &hkey.ctx.mbed_ecdh.d);
    assert(q && rng == random_gen && !ctx);
    *z = 42;
    return shared_error;
}
static int kdf(uint8_t protocol, mbedtls_mpi *z, uint8_t *out) {
    assert((protocol == 1 || protocol == 2) && *z == 42);
    ++calls;
    if (!kdf_error) memset(out, protocol, 64);
    return kdf_error;
}
'''
tests = r'''
int main(void) {
    for (int protocol = 1; protocol <= 2; ++protocol) {
        for (int len = 0; len <= 96; ++len) {
            int expected = len == (protocol == 1 ? 16 : 32) ? 0 : CTAP2_ERR_PIN_AUTH_INVALID;
            assert(check3(protocol, len) == expected);
            assert(check4(protocol, len) == expected);
        }
        for (int kind = 0; kind < 3; ++kind) {
            uint8_t out[64];
            mbedtls_ecp_point q = 0;
            memset(out, 0xa5, sizeof(out));
            shared_error = kind == 1 ? -17 : 0;
            kdf_error = kind == 2 ? -23 : 0;
            calls = freed = 0;
            assert(ecdh(protocol, &q, out) == (shared_error ? shared_error : kdf_error));
            assert(calls == (kind != 1) && freed == 1);
            for (size_t i = 0; i < sizeof(out); ++i)
                assert(out[i] == (kind == 0 ? protocol : 0xa5));
        }
    }
    puts("pin: tag lengths and ECDH error propagation passed for protocols 1 and 2");
}
'''
with tempfile.TemporaryDirectory() as tmp:
    path = Path(tmp)
    (path / 'pin.c').write_text(prefix + ecdh + '\n' + ''.join(guards) + tests)
    subprocess.run([os.environ.get('CC', 'cc'), '-std=c11', '-Wall', '-Wextra', '-Werror',
                    '-fsanitize=address,undefined', str(path / 'pin.c'), '-o', str(path / 'pin')], check=True)
    subprocess.run([str(path / 'pin')], check=True,
                   env={**os.environ, 'ASAN_OPTIONS': os.environ.get('ASAN_OPTIONS', 'detect_leaks=0')})
