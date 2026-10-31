#!/usr/bin/env python3
"""Lay out a unit's data sections the way retail's split does, to scale.

A unit only flips to `Matching` when the relinked `main.dol` stays byte-identical,
and the units that are 100% matched on code usually fail on their data: retail's
object puts a few more (or fewer) bytes in `.rodata`, `.bss`, `.sdata`, `.sbss` or
`.sdata2` than ours, and every byte of difference moves the following units' data,
which changes addresses in code that references it.  objdiff hides this behind a
name comparison, where retail's records show up as anonymous `gap_*` rows.

This tool reads both objects from the build tree and prints, per section, retail's
items and ours in offset order: offsets line up in columns, so a hole in one side
is the padding to add (or drop), and the record before it says where.  Retail's
holes are named from `config/GUPE8P/symbols.txt` when the split has a `gap_*`
record there.  Reads build output and config only, so it stays clean-room.

Usage:
    python tools/dataorder.py Game/ResourceManagerTask
    python tools/dataorder.py Game/ResourceManagerTask --section .sdata
"""

import argparse
import os
import re
import subprocess

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BINUTILS = os.path.join(root, "build", "binutils")
OBJDUMP = os.path.join(BINUTILS, "powerpc-eabi-objdump.exe")
SPLITS = os.path.join(root, "config", "GUPE8P", "splits.txt")
SYMBOLS = os.path.join(root, "config", "GUPE8P", "symbols.txt")

SECTIONS = (".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2")

SECTION_ROW = re.compile(r"^\s*\d+\s+(\.\w+)\s+([0-9a-f]{8})")
SYMBOL_ROW = re.compile(
    r"^([0-9a-f]{8}) (l|g|w)\s+([FO])\s+(\.\w+)\s+([0-9a-f]{8})(?: (\S+))?$")
GAP_RECORD = re.compile(r"^(gap_\w+) = (\.\w+):0x([0-9a-fA-F]+); // .*size:0x([0-9a-f]+)")


def object_path(kind, unit):
    """`build/GUPE8P/<kind>/<unit>.o`, trying the prefixes the registry mixes."""
    base = os.path.join(root, "build", "GUPE8P", kind)
    for prefix in ("", "main/", "Game/"):
        name = unit[len(prefix):] if unit.startswith(prefix + "/") else unit
        path = os.path.join(base, name.replace("/", os.sep) + ".o")
        if os.path.exists(path):
            return path
    return None


def retail_starts(unit):
    """Section start addresses of the unit from splits.txt."""
    starts = {}
    wanted = False
    with open(SPLITS, encoding="utf-8") as handle:
        for line in handle:
            header = re.match(r"^(\S+?)(?:\.cpp|\.c)?:$", line.strip())
            if header:
                wanted = header.group(1) == unit or header.group(1).endswith("/" + unit.rsplit("/", 1)[-1])
                continue
            row = re.match(r"^\s*(\.\w+)\s+start:0x([0-9a-fA-F]+)", line)
            if wanted and row:
                starts[row.group(1)] = int(row.group(2), 16)
    return starts


def layout(path):
    """Per section: (size, [(offset, size, linkage, name)]) taken from the object."""
    headers = subprocess.run([OBJDUMP, "-h", path], capture_output=True,
                             text=True, errors="replace").stdout
    sizes = {}
    for line in headers.splitlines():
        row = SECTION_ROW.match(line)
        if row and row.group(1) in SECTIONS:
            sizes[row.group(1)] = int(row.group(2), 16)
    table = subprocess.run([OBJDUMP, "-t", path], capture_output=True,
                           text=True, errors="replace").stdout
    items = {}
    for line in table.splitlines():
        row = SYMBOL_ROW.match(line)
        if not row or row.group(4) not in SECTIONS:
            continue
        offset, linkage, kind, section, size, name = (
            int(row.group(1), 16), row.group(2), row.group(3), row.group(4),
            int(row.group(5), 16), row.group(6) or "")
        if kind != "F":
            items.setdefault(section, []).append((offset, size, linkage, name))
    for section in items:
        items[section].sort()
    return sizes, items


def with_holes(items, size, start, named):
    """Insert the space the object's own symbols do not cover."""
    rows = []
    offset = 0
    for item in items:
        if item[0] > offset:
            address = start + offset if start else 0
            rows.append((offset, item[0] - offset, "-",
                         named.get(address, "(hole)")))
        rows.append(item)
        offset = max(offset, item[0] + item[1])
    if size > offset:
        address = start + offset if start else 0
        rows.append((offset, size - offset, "-", named.get(address, "(tail)")))
    return rows


def named_holes(unit):
    """`gap_*` records of symbols.txt keyed by absolute address."""
    named = {}
    with open(SYMBOLS, encoding="utf-8") as handle:
        for line in handle:
            record = GAP_RECORD.match(line.strip())
            if record:
                named[int(record.group(3), 16)] = record.group(1)
    return named


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit", help="unit as in configure.py, e.g. Game/ResourceManagerTask")
    ap.add_argument("--section", action="append", help="only this section")
    args = ap.parse_args()

    retail_obj = object_path("obj", args.unit)
    our_obj = object_path("src", args.unit)
    if retail_obj is None or our_obj is None:
        print(f"no {args.unit}.o in build/GUPE8P/obj and build/GUPE8P/src")
        return
    starts = retail_starts(args.unit)
    named = named_holes(args.unit)
    retail_sizes, retail_items = layout(retail_obj)
    our_sizes, our_items = layout(our_obj)

    for section in args.section or SECTIONS:
        retail_size = retail_sizes.get(section, 0)
        our_size = our_sizes.get(section, 0)
        if not retail_size and not our_size:
            continue
        drift = our_size - retail_size
        print(f"=== {args.unit} {section}: retail {retail_size} B, ours {our_size} B "
              f"({drift:+d} B){'  <- must be closed' if drift else ''}")
        if not drift and section not in (args.section or ()):
            # Same size: only worth walking when the offsets differ.
            pass
        start = starts.get(section)
        retail = with_holes(retail_items.get(section, []), retail_size, start, named)
        ours = with_holes(our_items.get(section, []), our_size, start, named)
        rows = max(len(retail), len(ours))
        for index in range(rows):
            left = retail[index] if index < len(retail) else None
            right = ours[index] if index < len(ours) else None

            def cell(row):
                if row is None:
                    return " " * 44
                offset, size, linkage, name = row
                return f"+{offset:#06x} {size:#05x} {linkage} {name[:26]:26}"

            mark = "=" if (left and right and left[1] == right[1]
                           and left[0] == right[0]) else "!"
            print(f" {mark} {cell(left)} | {cell(right)}")


main()
