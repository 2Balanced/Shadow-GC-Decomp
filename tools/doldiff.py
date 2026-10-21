#!/usr/bin/env python3
"""Compare two DOLs section by section and report differing address ranges.

Usage: python tools/doldiff.py [built.dol] [orig.dol]
Defaults to build/GUPE8P/main.dol vs orig/GUPE8P/sys/main.dol.
Useful when `ninja` reports a SHA-1 mismatch: it names the first differing
addresses so they can be looked up in symbols.txt.
"""

import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def sections(data: bytes):
    offs = struct.unpack(">18I", data[0:0x48])
    addrs = struct.unpack(">18I", data[0x48:0x90])
    sizes = struct.unpack(">18I", data[0x90:0xD8])
    out = []
    for i, (o, a, s) in enumerate(zip(offs, addrs, sizes)):
        if s:
            kind = f"text{i}" if i < 7 else f"data{i - 7}"
            out.append((kind, a, data[o : o + s]))
    return out


def main():
    built = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "build/GUPE8P/main.dol"
    orig = Path(sys.argv[2]) if len(sys.argv) > 2 else ROOT / "orig/GUPE8P/sys/main.dol"
    b, o = built.read_bytes(), orig.read_bytes()
    if b[:0x100] != o[:0x100]:
        print("header differs")
    bs = {a: (k, d) for k, a, d in sections(b)}
    for kind, addr, od in sections(o):
        if addr not in bs:
            print(f"{kind} @ {addr:#x}: missing in built DOL")
            continue
        bd = bs[addr][1]
        if len(bd) != len(od):
            print(f"{kind} @ {addr:#x}: size {len(bd):#x} vs orig {len(od):#x}")
        ranges, start = [], None
        for i in range(0, min(len(bd), len(od)), 4):
            diff = bd[i : i + 4] != od[i : i + 4]
            if diff and start is None:
                start = i
            elif not diff and start is not None:
                ranges.append((start, i))
                start = None
        if start is not None:
            ranges.append((start, min(len(bd), len(od))))
        for s, e in ranges[:20]:
            print(f"{kind}: {addr + s:#010x}..{addr + e:#010x}  built={bd[s:s+8].hex()} orig={od[s:s+8].hex()}")
        if len(ranges) > 20:
            print(f"{kind}: ... {len(ranges) - 20} more ranges")


if __name__ == "__main__":
    main()
