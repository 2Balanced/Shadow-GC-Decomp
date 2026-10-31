#!/usr/bin/env python3
"""Explain why a fully matched unit still relinks differently from retail.

`tools/unitstatus.py --flip` lists the units whose every function and data symbol is
matched; those are the only ones that can become `Matching`, and `tools/flipcheck.py`
shows which of them really relink byte-identically. This tool answers the next
question for the ones that do not: what is different between our object and the
retail split object, symbol by symbol?

It prints the section-size drift, the symbols only our object has (with their linkage
and whether the name already exists in `main.elf`, which is how mwldeppc can fold or
strip a copy away), and the symbols only the retail object has - usually the unnamed
`gap_*` records the retail split could not name, which are bytes our source does not
produce yet.

It reads build output only, so it stays clean-room.

Usage:
    python tools/flipsym.py Game/ResourceManagerTask Game/csdRW
"""

import argparse
import re
import subprocess

OBJDUMP = "build/binutils/powerpc-eabi-objdump.exe"
NM = "build/binutils/powerpc-eabi-nm.exe"
DATA_SECTIONS = (".ctors", ".rodata", ".data", ".bss", ".sdata", ".sbss", ".sdata2", ".sbss2")


def sections(path):
    out = subprocess.run([OBJDUMP, "-h", path], capture_output=True, text=True).stdout
    table = {}
    for line in out.splitlines():
        parts = re.split(r"\s+", line.strip())
        if len(parts) == 7 and parts[0].isdigit() and parts[1].startswith("."):
            table[parts[1]] = int(parts[2], 16)
    return table


def symbols(path):
    """`objdump -t` rows: value, linkage, F/O, section, size, name."""
    out = subprocess.run([OBJDUMP, "-t", path], capture_output=True, text=True).stdout
    rows = {}
    for line in out.splitlines():
        parts = re.split(r"\s+", line.strip())
        if len(parts) >= 6 and re.fullmatch(r"[0-9a-f]{8}", parts[0]) and parts[2] in ("F", "O"):
            linkage, section, size, name = parts[1], parts[3], parts[4], " ".join(parts[5:])
            if section == ".text" or section.startswith(".text") or section in DATA_SECTIONS:
                rows[name] = (section, linkage, int(size, 16))
    return rows


def linked_names():
    out = subprocess.run([NM, "--defined-only", "build/GUPE8P/main.elf"], capture_output=True, text=True).stdout
    names = set()
    for line in out.splitlines():
        parts = line.split(" ", 2)
        if len(parts) == 3:
            names.add(parts[2].strip())
    return names


def report(unit, linked):
    ours, retail = f"build/GUPE8P/src/{unit}.o", f"build/GUPE8P/obj/{unit}.o"
    our_sections, retail_sections = sections(ours), sections(retail)
    our_symbols, retail_symbols = symbols(ours), symbols(retail)

    print(f"=== {unit}: {len(our_symbols)} symbols, retail {len(retail_symbols)}")
    for section in sorted(set(our_sections) | set(retail_sections)):
        drift = our_sections.get(section, 0) - retail_sections.get(section, 0)
        if drift and not section.startswith(".comment") and not section.startswith(".note"):
            print(f"    {section:8} ours {our_sections.get(section, 0):6} "
                  f"retail {retail_sections.get(section, 0):6} drift {drift:+}")

    grouped = {}
    for name, (section, linkage, size) in our_symbols.items():
        if name in retail_symbols:
            continue
        state = "in main.elf" if name in linked else "stripped"
        key = (section, {"l": "local", "w": "weak", "g": "global"}.get(linkage, linkage), state)
        count, total = grouped.get(key, (0, 0))
        grouped[key] = (count + 1, total + size)
    for (section, linkage, state), (count, total) in sorted(grouped.items()):
        print(f"    extra   {section:8} {linkage:6} {state:11} x{count:3} {total:6} bytes")

    for name, (section, linkage, size) in sorted(retail_symbols.items()):
        if name not in our_symbols:
            print(f"    missing {section:8} {size:5} {name[:74]}")

    resized = [
        (name, our_symbols[name][2] - retail_symbols[name][2])
        for name in our_symbols
        if name in retail_symbols and our_symbols[name][2] != retail_symbols[name][2]
    ]
    for name, drift in sorted(resized, key=lambda item: -abs(item[1]))[:8]:
        print(f"    size differs {drift:+} {name[:70]}")


def main():
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument("units", nargs="+", help="unit name as in splits.txt, e.g. Game/csdRW")
    args = parser.parse_args()
    linked = linked_names()
    for unit in args.units:
        report(unit, linked)


if __name__ == "__main__":
    main()
