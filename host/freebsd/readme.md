# FreeBSD

Target: npm 2FA in Firefox, using the same USB firmware. Not tested yet.

The upstream firmware uses USB ID `1d50:619b`. Check with `usbconfig` before
installing the rule. It grants the `u2f` group access to matching `uhid` and
`hidraw` devices.

As root, from the repo:

```sh
pkg install firefox libfido2 u2f-devd
pw groupmod u2f -m YOUR_USER
install -m 644 host/freebsd/fingerthing.conf /usr/local/etc/devd/fingerthing.conf
service devd restart
```

Replace `YOUR_USER`, log out and back in, then reconnect the key. Run
`fido2-token -L` as that user. Test enrollment, sign-in and reconnect in Firefox
before trying npm. Record the FreeBSD, Firefox and firmware versions.

If the key is missing from the list, inspect its device node and permissions.
Don't grant access to every USB device or run the browser as root.

Based on FreeBSD's [u2f-devd rules](https://github.com/freebsd/freebsd-ports/tree/main/security/u2f-devd).
[libfido2](https://github.com/Yubico/libfido2/blob/main/src/hid_freebsd.c)
prefers `hidraw`, then falls back to `uhid`. FreeBSD's
[HID bus](https://github.com/freebsd/freebsd-src/blob/main/sys/dev/hid/hidbus.c)
reports the vendor and product IDs used by this rule.
