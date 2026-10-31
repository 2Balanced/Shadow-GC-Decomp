#!/usr/bin/env python3
"""Function emission order of a unit's object, retail's split next to ours.

MWCC writes one `.text` per object and lays the functions out in the order it
finishes them, so the symbol values inside the object *are* the emission order.
`textorder.py` compares those against retail's addresses in the DOL; here the two
object files are lined up directly, which is what you want when moving a
definition in the source to change its position.

Usage:
    python tools/fnorder.py Game/ResourceManagerTask
    python tools/fnorder.py Game/ResourceManagerTask --diff
"""

import argparse
import os
import re
import subprocess

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
READELF = os.path.join(root, "build", "binutils", "powerpc-eabi-readelf.exe")

SECTION_ROW = re.compile(r"^\s*\[\s*(\d+)\]\s+(\S+)")
SYMBOL_ROW = re.compile(r"^\s*\d+:\s+([0-9a-f]{8})\s+(\d+)\s+FUNC\s+(\w+)\s+\w+\s+(\d+)\s+(\S+)")


def object_path(kind, unit):
    base = os.path.join(root, "build", "GUPE8P", kind)
    for prefix in ("", "main/", "Game/"):
        name = unit[len(prefix):] if unit.startswith(prefix + "/") else unit
        path = os.path.join(base, name.replace("/", os.sep) + ".o")
        if os.path.exists(path):
            return path
    return None


def order(path):
    """The .text function symbols by value: readelf keeps long names on one line."""
    out = subprocess.run([READELF, "-SW", path], capture_output=True,
                         text=True, errors="replace").stdout
    text_index = {}
    for line in out.splitlines():
        m = SECTION_ROW.match(line)
        if m:
            text_index[m.group(2)] = m.group(1)
    index = text_index.get(".text")
    if index is None:
        return []
    out = subprocess.run([READELF, "-sW", path], capture_output=True,
                         text=True, errors="replace").stdout
    rows = []
    for line in out.splitlines():
        row = SYMBOL_ROW.match(line)
        if not row or row.group(4) != index:
            continue
        bind = {"GLOBAL": "g", "WEAK": "w", "LOCAL": "l"}.get(row.group(3), "?")
        rows.append((int(row.group(1), 16), bind, int(row.group(2)), row.group(5)))
    rows.sort()
    return rows


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit")
    ap.add_argument("--diff", action="store_true", help="only rows that differ")
    args = ap.parse_args()

    retail = order(object_path("obj", args.unit))
    ours = order(object_path("src", args.unit))
    if not retail or not ours:
        print(f"{args.unit}: no .text symbols in one of the objects")
        return
    print(f"=== {args.unit}: retail {len(retail)} functions +0x0..+{retail[-1][0] + retail[-1][2]:#x}, "
          f"ours {len(ours)} +0x0..+{ours[-1][0] + ours[-1][2]:#x}")

    ours_index = {row[3]: index for index, row in enumerate(ours)}
    print(f"{'':>1} {'#':>3} {'retail':<50} | {'ours':>4}")
    for index, (_, linkage, size, name) in enumerate(retail):
        position = ours_index.get(name)
        mark = " " if position == index else "!"
        where = f"{position}" if position is not None else "-"
        if args.diff and position == index:
            continue
        print(f"{mark}{index:>3} {linkage} +{size:#07x} {name[:46]:<46} | {where:>4}")
    for name, position in sorted(ours_index.items(), key=lambda p: p[1]):
        if name not in {row[3] for row in retail}:
            print(f"?  -- {'':<49} | {position:>4} {name[:46]}")


main()
