#!/usr/bin/env python3
"""Explain why flipping one unit to `Matching` changes `main.dol`, word by word.

`tools/flipcheck.py` says whether a flip links and prints the first few differing
addresses; this tool flips the same way (and then restores `configure.py`) and reports
every difference: first as contiguous ranges, which shows whether the unit was shifted or
only rearranged, then word by word, decoding branches and resolving both targets through
`config/GUPE8P/symbols.txt`.  A differing pair whose decoded targets are two copies of the
same function means the unit's own code is fine and MWCC emitted it in another order.

`--keep-dol` also leaves the failing DOL at `build/flipped_main.dol` for `doldiff.py`.

Usage:
    python tools/dolwhy.py Game/EnemyMotion.cpp
"""

import argparse
import bisect
import re
import shutil
import struct
import sys

from flipcheck import DOL, RETAIL, links, run, set_flag, sha1  # noqa: E402

CONFIGURE = "configure.py"
SYMBOLS = "config/GUPE8P/symbols.txt"


def symbols():
    rows = []
    for line in open(SYMBOLS, encoding="utf-8"):
        m = re.search(
            r"=\s*\.([a-z0-9]+):0x([0-9A-Fa-f]{6,8});\s*//\s*type:\S+\s+size:0x([0-9a-f]+)",
            line,
        )
        if m:
            rows.append(
                (int(m.group(2), 16), int(m.group(3), 16), m.group(1), line.split("=")[0].strip())
            )
    rows.sort()
    return rows


ROWS = symbols()
STARTS = [row[0] for row in ROWS]


def name(addr):
    """Retail's symbol at addr, naming the gap after a symbol when there is none."""
    i = bisect.bisect_right(STARTS, addr) - 1
    while i + 1 < len(ROWS) and ROWS[i + 1][0] <= addr:
        i += 1
    if i < 0:
        return "?"
    start, size, section, symbol = ROWS[i]
    if addr < start + size:
        return f"{symbol}+{addr - start:#x} ({section})"
    return f"after {symbol}+{size:#x} ({section})"


def decode(word, addr):
    """Decode a PowerPC `b`/`bl` as (text, target), or (None, None)."""
    if word & 0xFC000003 in (0x48000000, 0x48000001):
        delta = word & 0x03FFFFFC
        if delta & 0x02000000:
            delta -= 0x04000000
        link = "bl" if word & 1 else "b"
        return f"{link} {addr + delta:#x}", addr + delta
    return None, None


def dol_sections(data):
    offs = struct.unpack(">18I", data[0:0x48])
    addrs = struct.unpack(">18I", data[0x48:0x90])
    sizes = struct.unpack(">18I", data[0x90:0xD8])
    return {a: data[o : o + s] for o, a, s in zip(offs, addrs, sizes) if s}


def runs(built, orig):
    """Contiguous differing (start, end) address ranges, word aligned."""
    out = []
    for addr, od in sorted(orig.items()):
        bd = built.get(addr, b"")
        start = None
        for i in range(0, min(len(bd), len(od)), 4):
            if bd[i : i + 4] != od[i : i + 4]:
                if start is None:
                    start = i
            elif start is not None:
                out.append((addr + start, addr + i))
                start = None
        if start is not None:
            out.append((addr + start, addr + min(len(bd), len(od))))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit", help="source path as written in configure.py, e.g. Game/Foo.cpp")
    ap.add_argument("--keep-dol", action="store_true", help="save the failing DOL as build/flipped_main.dol")
    ap.add_argument("--words", type=int, default=15, help="how many differing words to spell out (default 15)")
    args = ap.parse_args()

    run("git", "checkout", "--", CONFIGURE)
    if not set_flag(args.unit, "Matching"):
        print(f"{args.unit}: configure.py has no unique Object() entry for it")
        return
    ok, note = links()
    print(f"{args.unit}: {'PASS' if ok else 'FAIL'} {note}")
    if ok:
        run("git", "checkout", "--", CONFIGURE)
        run("python", CONFIGURE)
        run("ninja", "-j", "8", DOL)
        return

    built = dol_sections(open(DOL, "rb").read())
    orig = dol_sections(open(RETAIL, "rb").read())
    if args.keep_dol:
        shutil.copyfile(DOL, "build/flipped_main.dol")
        print("saved the failing DOL at build/flipped_main.dol")
    every = runs(built, orig)
    print(f"{sum(end - start for start, end in every)} bytes in {len(every)} differing ranges")
    for start, end in every[: args.words]:
        print(f"  {start:#x}..{end:#x} ({end - start} B) at {name(start)}")
    if len(every) > args.words:
        print(f"  ... {len(every) - args.words} more ranges")

    shown = 0
    for start, end in every:
        for addr in range(start, end, 4):
            section = next((a for a in orig if a <= addr < a + len(orig[a])), None)
            offset = addr - section
            bw = int.from_bytes(built[section][offset : offset + 4], "big")
            ow = int.from_bytes(orig[section][offset : offset + 4], "big")
            btext, btarget = decode(bw, addr)
            otext, otarget = decode(ow, addr)
            if btext and otext:
                print(f"  {addr:#x} built {btext} -> {name(btarget)}")
                print(f"  {' ' * len(hex(addr))} orig  {otext} -> {name(otarget)}")
            else:
                print(f"  {addr:#x} built={bw:08x} orig={ow:08x} {name(addr)}")
            shown += 1
            if shown >= args.words * 4:
                print("  ...")
                break
        if shown >= args.words * 4:
            break

    run("git", "checkout", "--", CONFIGURE)
    run("python", CONFIGURE)
    run("ninja", "-j", "8", DOL)
    print("configure.py restored, DOL matches retail:", sha1(DOL) == sha1(RETAIL))


main()
