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
    python tools/flipsym.py --words .data,.sdata Game/ThrowBomb
    python tools/flipsym.py --refs Game/ThrowBomb

`--words SECTION ...` prints the section word by word, with the relocation that fills each
word resolved to its symbol, retail next to ours. Use it when `tools/dolwhy.py` points at a
handful of data words: it shows which record each word belongs to, and which record the
linker was expecting. Rows that line up differently here are not always DOL differences -
the records only our object has are folded away by the link - so confirm with
`tools/dolwhy.py`. `--refs` prints, for both objects, the offset of the first relocation
that names each `__vt__`/`__RTTI__` record - the order CodeWarrior emits those records in,
which is what decides their addresses.
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


def section_words(path, section):
    """`objdump -h -s -j SEC -r`: the words of a section and the relocation covering each."""
    run = subprocess.run([OBJDUMP, "-h", "-s", "-j", section, "-r", path],
                         capture_output=True, text=True)
    out = run.stdout + run.stderr
    words, relocs = {}, {}
    in_content, in_reloc = False, False
    for line in out.splitlines():
        if "RELOCATION RECORDS FOR" in line:
            in_reloc = ("[" + section + "]") in line
            in_content = False
            continue
        if line.strip().lower().startswith("contents of section"):
            in_content, in_reloc = True, False
            continue
        if in_content:
            m = re.match(r"^\s*([0-9a-f]{4,})\s+((?:[0-9a-f]{8}\s+)+)", line)
            if m:
                offset = int(m.group(1), 16)
                for i, word in enumerate(m.group(2).split()):
                    words[offset + 4 * i] = int(word, 16)
                continue
        if in_reloc:
            m = re.match(r"^([0-9a-f]{4,})\s+(R_PPC\S*)\s*(.*)$", line.strip())
            if m:
                relocs[int(m.group(1), 16)] = (m.group(2), m.group(3).strip())
    return words, relocs


def compare_words(unit, sections_):
    ours, retail = f"build/GUPE8P/src/{unit}.o", f"build/GUPE8P/obj/{unit}.o"
    for section in sections_:
        print(f"=== {unit} {section}")
        left, right = section_words(retail, section), section_words(ours, section)
        for offset in sorted(set(left[0]) | set(right[0])):
            same = left[0].get(offset) == right[0].get(offset) and \
                left[1].get(offset) == right[1].get(offset)

            def fmt(side):
                word = side[0].get(offset, 0)
                reloc = side[1].get(offset)
                return "%08x %-9s %s" % (
                    word, reloc[0][5:] if reloc else "", reloc[1][:56] if reloc else "")

            print("%s +%04x  R %-70s | O %s" %
                  ("  " if same else "!!", offset, fmt(left), fmt(right)))


def first_refs(path, patterns):
    """Where each symbol is first referenced: CodeWarrior's emission order.

    The offsets objdump prints are section-relative, so the section is kept with each row.
    """
    out = subprocess.run([OBJDUMP, "-dr", path], capture_output=True, text=True).stdout
    seen = {}
    section = None
    for line in out.splitlines():
        header = re.match(r"^Disassembly of section (\S+):", line)
        if header:
            section = header.group(1)
            continue
        header = re.search(r"RELOCATION RECORDS FOR \[(\S+)\]", line)
        if header:
            section = header.group(1)
            continue
        m = re.match(r"^\s*([0-9a-f]{1,8}):\s+(R_PPC\S*)\s*(.*)$", line)
        if not m:
            continue
        name = m.group(3).strip()
        if any(pattern in name for pattern in patterns) and name not in seen:
            seen[name] = (section, int(m.group(1), 16))
    return seen


def compare_refs(unit, patterns):
    ours, retail = f"build/GUPE8P/src/{unit}.o", f"build/GUPE8P/obj/{unit}.o"
    left, right = first_refs(retail, patterns), first_refs(ours, patterns)
    print(f"=== {unit}: first reference to each record, in retail's order within each section")
    for name in sorted(set(left) | set(right),
                       key=lambda n: ((left.get(n) or right.get(n))[0],
                                      (left.get(n) or right.get(n))[1])):
        section, _ = left.get(name) or right.get(name)
        mark = "  " if name in left and name in right else "!!"
        print("%s  %-8s retail %-9s ours %-9s %s" % (
            mark, section,
            hex(left[name][1]) if name in left else "-",
            hex(right[name][1]) if name in right else "-", name[:70]))


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
    parser.add_argument("--words", metavar="SECTIONS",
                        help="comma-separated data sections to dump word by word, e.g. .data,.sdata")
    parser.add_argument("--refs", action="store_true",
                        help="compare the order in which vtable and RTTI records are first referenced")
    parser.add_argument("--pattern", action="append",
                        help="name substring for --refs (default: __vt__, __RTTI__)")
    args = parser.parse_args()
    for unit in args.units:
        if args.words:
            compare_words(unit, [s.strip() for s in args.words.split(",") if s.strip()])
        if args.refs:
            compare_refs(unit, args.pattern or ["__vt__", "__RTTI__"])
        if not args.words and not args.refs:
            report(unit, linked_names())


if __name__ == "__main__":
    main()
