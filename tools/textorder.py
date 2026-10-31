#!/usr/bin/env python3
"""Report whether a unit's own `.text` would land in retail's order if flipped.

A fully matched unit only becomes `Matching` when the relinked `main.dol` is byte-identical,
and for most of the ones that fail the reason is placement, not code: MWCC puts every function
of a unit in one `.text` section in the order it emits them, and MWLD keeps that order, so a
flip only works when our definition order puts each function back at retail's address.
`flipsym.py` compares names and cannot see that.

This tool reads the function symbols of `build/GUPE8P/src/<unit>.o`, whose offsets inside its
single `.text` section are MWCC's emission order, and adds the unit's retail `.text` start from
`config/GUPE8P/splits.txt`.  Each function is then checked against `config/GUPE8P/symbols.txt`:
a line says where our function lands, which function retail really had there, and where
retail has our function.  Silence means the order is already retail's; `--all` prints the
whole walk.

Ranges the retail split could not name show as `(unnamed)`, so a run of those is the split's
gap, not necessarily a mismatch.  Reads build output and config only, so it stays clean-room.

Usage:
    python tools/textorder.py Game/EnemyMotion
    python tools/textorder.py Game/EnemyMotion --all
"""

import argparse
import re
import subprocess

OBJDUMP = "build/binutils/powerpc-eabi-objdump.exe"
CONFIG = "config/GUPE8P"


def retail_functions(lo, hi):
    """Retail's named `.text` functions inside [lo, hi) as (address, size, name)."""
    out = []
    for line in open(f"{CONFIG}/symbols.txt", encoding="utf-8"):
        m = re.search(
            r"=\s*\.text:0x([0-9A-Fa-f]{6,8});\s*//\s*type:function\s+size:0x([0-9a-f]+)", line
        )
        if not m:
            continue
        address, size = int(m.group(1), 16), int(m.group(2), 16)
        if lo <= address < hi:
            out.append((address, size, line.split("=")[0].strip()))
    out.sort()
    return out


def retail_at(known, addr):
    """Retail's function covering addr, or None in a range the split left unnamed."""
    for address, size, name in known:
        if address <= addr < address + size:
            return name
    return None


def our_text_order(unit):
    """Our object's `.text` functions in address order as (value, name, size)."""
    out = subprocess.run(
        [OBJDUMP, "-t", f"build/GUPE8P/src/{unit}.o"], capture_output=True, text=True
    ).stdout
    order = []
    for line in out.splitlines():
        parts = re.split(r"\s+", line.strip())
        if len(parts) >= 6 and parts[2] == "F" and parts[3] == ".text":
            order.append((int(parts[0], 16), " ".join(parts[5:]), int(parts[4], 16)))
    order.sort()
    return order


def retail_text_range(unit):
    wanted = f"{unit}.c:" if unit.endswith(".c") else f"{unit}.cpp:"
    inside = False
    for line in open(f"{CONFIG}/splits.txt", encoding="utf-8"):
        if line.endswith(":\n"):
            inside = line.strip() == wanted
        elif inside and ".text" in line:
            m = re.search(r"start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)", line)
            return int(m.group(1), 16), int(m.group(2), 16)
    return None, None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("units", nargs="+", help="unit as named in splits.txt, e.g. Game/EnemyMotion")
    ap.add_argument("--all", action="store_true", help="print every function, not only the misplaced ones")
    args = ap.parse_args()

    for unit in args.units:
        start, stop = retail_text_range(unit)
        if start is None:
            print(f"=== {unit}: no .text range in splits.txt")
            continue
        order = our_text_order(unit)
        total = sum(size for _value, _name, size in order)
        known = retail_functions(start, max(stop, start + total))
        by_name = {name: address for address, _size, name in known}
        print(f"=== {unit}: retail .text {start:#x}..{stop:#x} ({stop - start:#x} B), our "
              f"object lays out {len(order)} functions over {total:#x} B")
        misplaced = 0
        for value, name, size in order:
            addr = start + value
            want = by_name.get(name)
            here = retail_at(known, addr)
            if want == addr:
                status = "ok"
            else:
                misplaced += 1
                status = f"want {want:#x}" if want else "not in symbols.txt"
                status += f", retail has {here or '(unnamed)'} at {addr:#x}"
            if args.all or status != "ok":
                print(f"  {addr:#x} +{size:#05x} {name[:52]:52} {status}")
        print(f"  {len(order) - misplaced} of {len(order)} functions land where retail had them")


main()
