#!/usr/bin/env python3
"""Set unused padding bytes of a function's exception table in a compiled object, in place.

MWCC leaves the unused bytes of some exception actions uninitialized (they change from one
compile to the next), so an object whose retail copy has such bytes cannot be reproduced by the
compiler alone. `dtk extab clean --padding` (the `extab_padding` option of configure.py) fixes
them by rewriting the whole object, but the rewrite clears a flag in the object's `.comment`
section that MWCC sets on a section's data-pool base symbol (`...rodata.0`): without it the linker
no longer keeps that section whole and strips the strings the code reaches through the base
register. This tool changes nothing but the named bytes.

Usage: python tools/extabpad.py OBJECT STAMP FUNCTION:OFFSET=HEXBYTES ...
    FUNCTION  a function symbol of the object; OFFSET is relative to the start of its extab entry
    STAMP     written when done (the build's record that the object has been patched)
The object is only rewritten when a byte differs, so a second run leaves it untouched.
"""

import struct
import sys


def main():
    obj, stamp, specs = sys.argv[1], sys.argv[2], sys.argv[3:]
    with open(obj, "rb") as f:
        d = bytearray(f.read())
    (shoff,) = struct.unpack_from(">I", d, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from(">HHH", d, 0x2E)
    secs = [struct.unpack_from(">10I", d, shoff + i * shentsize) for i in range(shnum)]

    def cstr(sec, off):
        base = secs[sec][4] + off
        return d[base:d.index(0, base)].decode()

    names = [cstr(shstrndx, s[0]) for s in secs]
    symtab = next(i for i, s in enumerate(secs) if s[1] == 2)
    strtab = secs[symtab][6]
    syms = []
    for i in range(secs[symtab][5] // 16):
        name, value, size, info, other, shndx = struct.unpack_from(
            ">IIIBBH", d, secs[symtab][4] + i * 16)
        syms.append((cstr(strtab, name), value, shndx))
    extab = names.index("extab")
    index_rela = names.index(".relaextabindex")
    # extabindex entries are (function, size, extab entry): relocations at 12n and 12n + 8.
    relocs = {}
    for i in range(secs[index_rela][5] // 12):
        offset, info, addend = struct.unpack_from(">IIi", d, secs[index_rela][4] + i * 12)
        relocs[offset] = (syms[info >> 8], addend)
    entries = {}
    for offset, (sym, addend) in relocs.items():
        if offset % 12 == 0 and offset + 8 in relocs:
            tsym, taddend = relocs[offset + 8]
            if tsym[2] != extab:
                raise SystemExit(f"{obj}: extab entry of {sym[0]} is not in extab")
            entries[sym[0]] = tsym[1] + taddend
    changed = False
    for spec in specs:
        where, value = spec.split("=")
        func, off = where.rsplit(":", 1)
        if func not in entries:
            raise SystemExit(f"{obj}: no exception table entry for {func}")
        data = bytes.fromhex(value)
        pos = secs[extab][4] + entries[func] + int(off, 0)
        if pos + len(data) > secs[extab][4] + secs[extab][5]:
            raise SystemExit(f"{obj}: {spec} is outside extab")
        if d[pos:pos + len(data)] != data:
            d[pos:pos + len(data)] = data
            changed = True
    if changed:
        with open(obj, "wb") as f:
            f.write(d)
    with open(stamp, "w") as f:
        f.write("ok\n")


if __name__ == "__main__":
    main()
