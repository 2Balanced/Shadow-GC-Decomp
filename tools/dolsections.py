#!/usr/bin/env python3
"""Compare the section tables of two Game Cube DOL images.

The DOL header is sixteen (offset, address, size, alignment) triples, eight
`.text`-like sections followed by eight data-like ones, so two images that differ
only in where the sections sit can be told apart in twelve lines.  `doldiff.py`
reports differing *addresses*; this reports the sizes that caused them, which is
the number a unit's flip actually has to match.

Usage:
    python tools/dolsections.py build/flipped_main.dol
    python tools/dolsections.py build/flipped_main.dol --full
"""

import argparse
import os
import struct

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RETAIL = os.path.join(root, "orig", "GUPE8P", "sys", "main.dol")

HEADER_SIZE = 0x100
COUNT = 0x10


def sections(path):
    with open(path, "rb") as handle:
        header = handle.read(HEADER_SIZE)
    rows = []
    for index in range(COUNT):
        offset, address, size, _align = struct.unpack_from(">IIII", header, index * 0x10)
        if size == 0:
            continue
        kind = "text" if index < 8 else "data"
        rows.append((index, kind, address, size, offset))
    rows.sort(key=lambda row: row[2])
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dol", help="the image to compare against retail's")
    ap.add_argument("--full", action="store_true", help="list both columns in full")
    args = ap.parse_args()

    retail = sections(RETAIL)
    other = sections(args.dol)
    print(f"=== retail {RETAIL.rsplit(os.sep, 1)[-1]} vs {args.dol.rsplit(os.sep, 1)[-1]}")
    print(f"{'idx':>3} {'kind':<5} {'retail addr':>12} {'size':>9} | "
          f"{'other addr':>12} {'size':>9}  drift")
    left = {row[0]: row for row in retail}
    right = {row[0]: row for row in other}
    total_drift = 0
    for index in sorted(set(left) | set(right)):
        a = left.get(index)
        b = right.get(index)
        if a is None or b is None:
            print(f"{index:>3} {('text' if index < 8 else 'data'):<5} "
                  f"{'present' if a else '-':>12} {'only in one image'}")
            continue
        drift = b[3] - a[3]
        total_drift += drift
        if not drift and not args.full and a[2] == b[2]:
            continue
        print(f"{index:>3} {a[1]:<5} {a[2]:>#14x} {a[3]:>9} | {b[2]:>#14x} {b[3]:>9} "
              f"{drift:+d}")
    print(f"--- total size drift {total_drift:+d} bytes")


main()
