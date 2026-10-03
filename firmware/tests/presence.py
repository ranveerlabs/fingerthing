from pathlib import Path
import os
import subprocess
import tempfile

root = Path(__file__).resolve().parents[2]
src = root / '.build/pico-fido2/pico-fido/src/fido'
blocks = []
for name in ('cbor_make_credential.c', 'cbor_get_assertion.c'):
    text = (src / name).read_text()
    start = text.index('if (options.up == ptrue')
    end = text.index('{', start) + 1
    depth = 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    blocks.append(text[start:end])
code = '''
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#define FIDO2_AUT_FLAG_UP 1
#define FIDO2_AUT_FLAG_UV 4
#define CTAP2_ERR_OPERATION_DENIED 1
#define CBOR_ERROR(x) return -(x)
static int yes, no;
static int *ptrue = &yes;
static int calls;
static bool allow, pin;
static bool check_user_presence_with_pin(bool verified) {
    ++calls;
    pin = verified;
    return allow;
}
static void clearUserPresentFlag(void) {}
static void clearUserVerifiedFlag(void) {}
static void clearPinUvAuthTokenPermissionsExceptLbw(void) {}
'''
for i, block in enumerate(blocks):
    code += f'''static int check{i}(unsigned flags, int *up, bool present) {{
    struct {{ int *up; bool present; }} options = {{up, present}};
    (void)options;
    {block}
    return (int)flags;
}}
'''
code += '''
int main(void) {
    int (*checks[])(unsigned, int *, bool) = {check0, check1};
    for (int i = 0; i < 2; ++i) {
        for (unsigned flags = 0; flags < 8; ++flags) {
            allow = false;
            calls = 0;
            assert(checks[i](flags, NULL, false) == -1);
            assert(calls == 1 && pin == !!(flags & FIDO2_AUT_FLAG_UV));
            allow = true;
            calls = 0;
            assert(checks[i](flags, ptrue, true) == (int)(flags | FIDO2_AUT_FLAG_UP));
            assert(calls == 1 && pin == !!(flags & FIDO2_AUT_FLAG_UV));
        }
        calls = 0;
        assert(checks[i](0, &no, true) == 0 && calls == 0);
    }
    puts("presence: registration and assertion require approval before UP");
}
'''
with tempfile.TemporaryDirectory() as tmp:
    c = Path(tmp) / 'presence.c'
    exe = Path(tmp) / 'presence'
    c.write_text(code)
    subprocess.run([os.environ.get('CC', 'cc'), '-std=c11', '-Wall', '-Wextra',
                    '-Werror', '-fsanitize=undefined', str(c), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
