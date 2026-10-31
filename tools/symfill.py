#!/usr/bin/env python3
"""Fill the gaps retail left in ``config/GUPE8P/symbols.txt``.

Every annotation here describes a *retail* address. objdiff symbolises both sides
of a diff from this file, so a variable our object defines inside a unit's split
but which retail never named shows up as a ``DIFF_REPLACE`` row on every
instruction that touches it: the disassembly text is identical, yet the strict
comparison is not, and the unit can never be ``Matching``. ``pair_symbols.py``
cannot help, because small-data accesses carry no relocation to pair - the
symbol has to be annotated.

This tool reads a unit's own compiled object, takes the data symbols the
compiler emitted for it (``@129``, ``init$130``, ``s_platform$128`` - MWCC
generates the same local names for the same source, so ours are the retail
names), and writes an annotation for each address retail left blank:

    python tools/symfill.py Game/csdRW --dry-run
    python tools/symfill.py Game/csdRW

Already-annotated addresses are never touched, and neither are symbols at
address 0, zero-sized symbols, section symbols, or anything outside the unit's
own split ranges - an address that belongs to another unit is that unit's
business.

Note that MWCC renumbers the ``@NNN``/``NN$`` suffixes whenever the source
changes, so the names this writes are only valid for the source as it stands;
re-run it after a decompile that moves variables around.
"""
from __future__ import annotations

import argparse
import re
import subprocess
import sys
from bisect import bisect_right
from collections import Counter
from pathlib import Path
from typing import Any

DATA_SECTIONS = (".rodata", ".data", ".sdata", ".sdata2", ".bss", ".sbss")
SECTION_ORDER = (
    ".text", ".extab", ".extabindex", ".ctors", ".dtors",
    ".rodata", ".data", ".sdata", ".sdata2", ".bss", ".sbss",
)
# Symbols MWCC emits that are not retail-visible objects.
SECTION_SYMBOL = re.compile(r"^[^\s]*\.(bss|data|sdata|sdata2|sbss|rodata|text)\.0$")


def parse_splits(path: Path) -> dict[str, dict[str, tuple[int, int]]]:
    """``Game/foo.cpp:`` followed by ``\\t.section start:0x.. end:0x..`` lines."""
    units: dict[str, dict[str, tuple[int, int]]] = {}
    current: dict[str, tuple[int, int]] | None = None
    for line in path.read_text(encoding="utf-8").splitlines():
        if not line.strip():
            continue
        if not line.startswith((" ", "\t")):
            name = line.strip().rstrip(":")
            if name.endswith(".cpp"):
                name = name[:-4]
            current = units.setdefault(name, {})
            continue
        match = re.match(r"(\.\w+)\s+start:(0x[0-9A-Fa-f]+)\s+end:(0x[0-9A-Fa-f]+)", line.strip())
        if match and current is not None:
            current[match.group(1)] = (int(match.group(2), 16), int(match.group(3), 16))
    return units


def parse_symbols(path: Path) -> tuple[dict[tuple[str, int], dict[str, str]], list[dict[str, Any]]]:
    """Returns a ``(section, address) -> entry`` map and the file's entries in order."""
    annotated: dict[tuple[str, int], dict[str, str]] = {}
    entries: list[dict[str, Any]] = []
    for index, line in enumerate(path.read_text(encoding="utf-8").splitlines()):
        if "=" not in line:
            continue
        left, _, right = line.partition("=")
        name = left.strip()
        match = re.match(r"(\.[A-Za-z0-9_]+):0x([0-9A-Fa-f]+)\s*(?:\[\d+\])?\s*;(.*)", right.strip())
        if not match:
            continue
        entry = {
            "index": index,
            "line": line,
            "name": name,
            "section": match.group(1),
            "address": int(match.group(2), 16),
            "raw_size": (re.search(r"size:(0x[0-9A-Fa-f]+)", match.group(3)) or [None, "0x0"])[1],
        }
        entries.append(entry)
        annotated[(entry["section"], entry["address"])] = entry
    return annotated, entries


def object_data_symbols(obj: Path, objdump: Path) -> list[dict[str, Any]]:
    output = subprocess.run([str(objdump), "-t", str(obj)], check=True, capture_output=True, text=True).stdout
    symbols: list[dict[str, Any]] = []
    for line in output.splitlines():
        fields = line.split()
        if len(fields) == 6:
            value, binding, _type, section, size, name = fields
        elif len(fields) == 5:
            value, binding, section, size, name = fields
            binding = binding.lstrip("l") or "l"
        else:
            continue
        if not section.startswith(".") or not re.fullmatch(r"[0-9A-Fa-f]{8}", size):
            continue
        if section not in DATA_SECTIONS:
            continue
        symbols.append(
            {
                "value": int(value, 16),
                "address": int(value, 16),
                "binding": binding,
                "section": section,
                "size": int(size, 16),
                "name": name,
            }
        )
    return symbols


def symbol_spans(entries: list[dict[str, Any]]) -> dict[str, list[tuple[int, int, str]]]:
    """``section`` -> sorted ``(start, end, name)`` for every annotated symbol with a size."""
    spans: dict[str, list[tuple[int, int, str]]] = {}
    for entry in entries:
        size = int(entry["raw_size"], 16)
        if size:
            spans.setdefault(entry["section"], []).append((entry["address"], entry["address"] + size, entry["name"]))
    for section in spans:
        spans[section].sort()
    return spans


def gather(
    splits: dict[str, dict[str, tuple[int, int]]],
    annotated: dict[tuple[str, int], dict[str, str]],
    project_root: Path,
    unit: str,
    spans: dict[str, list[tuple[int, int, str]]] | None = None,
) -> dict[str, Any]:
    """Our object's data symbols for ``unit``, split into missing and already-named."""
    ranges = {s: r for s, r in splits.get(unit, {}).items() if s in DATA_SECTIONS}
    obj = project_root / "build/GUPE8P/src" / f"{unit}.o"
    objdump = project_root / "build/binutils/powerpc-eabi-objdump.exe"
    missing: list[dict[str, Any]] = []
    covered: list[dict[str, Any]] = []
    skipped: list[dict[str, Any]] = []
    seen: set[tuple[str, int]] = set()
    for symbol in object_data_symbols(obj, objdump):
        span = ranges.get(symbol["section"])
        if span is None:
            skipped.append({**symbol, "why": "section not in this unit's split"})
            continue
        # The compiler's object holds section-relative offsets; the split tells us
        # where that section lands in the retail image.
        symbol = {**symbol, "address": span[0] + symbol["value"]}
        if not (span[0] <= symbol["address"] < span[1]):
            skipped.append({**symbol, "why": "past the end of the split"})
            continue
        if spans:
            owner = next(
                (
                    (start, end, name)
                    for start, end, name in spans.get(symbol["section"], ())
                    if start <= symbol["address"] < end and name != symbol["name"]
                ),
                None,
            )
            if owner:
                skipped.append({**symbol, "why": f"inside {owner[2]} (0x{owner[0]:X}-0x{owner[1]:X})"})
                continue
        if symbol["name"] == "." or SECTION_SYMBOL.match(symbol["name"]):
            skipped.append({**symbol, "why": "section symbol"})
            continue
        if symbol["size"] == 0 or symbol["address"] == 0:
            skipped.append({**symbol, "why": "zero-sized / absolute"})
            continue
        key = (symbol["section"], symbol["address"])
        if key in annotated:
            covered.append({**symbol, "retail_name": annotated[key].get("name", "")})
            continue
        if key in seen:
            skipped.append({**symbol, "why": "duplicate address"})
            continue
        seen.add(key)
        missing.append(symbol)
    missing.sort(key=lambda s: (s["section"], s["address"], s["name"]))
    return {
        "unit": unit,
        "ranges": ranges,
        "missing": missing,
        "covered": covered,
        "skipped": skipped,
    }


def new_symbol_line(symbol: dict[str, Any]) -> str:
    scope = {"l": "local", "w": "weak", "g": "global"}.get(symbol["binding"], "local")
    return f"{symbol['name']} = {symbol['section']}:0x{symbol['address']:08X}; // type:object size:{hex(symbol['size'])} scope:{scope}"


def apply_annotations(
    path: Path, entries: list[dict[str, Any]], proposals: list[dict[str, Any]]
) -> int:
    """Insert the proposals into ``path``, keeping the file sorted by address."""
    lines = path.read_text(encoding="utf-8").splitlines()
    ordered = sorted(entries, key=lambda e: e["index"])
    addresses = [e["address"] for e in ordered]
    after: dict[int, list[str]] = {}
    for proposal in proposals:
        position = bisect_right(addresses, proposal["address"]) - 1
        after.setdefault(ordered[position]["index"] if position >= 0 else -1, []).append(proposal["line"])
    out: list[str] = []
    for index, line in enumerate(lines):
        for line_to_add in after.get(index, ()):
            out.append(line_to_add)
        out.append(line)
    for line_to_add in after.get(-1, ()):
        out.insert(0, line_to_add)
    path.write_text("\n".join(out) + "\n", encoding="utf-8")
    return len(proposals)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("unit", nargs="+", help="unit path, e.g. Game/csdRW")
    parser.add_argument("--dry-run", action="store_true", help="show the annotations instead of writing them")
    parser.add_argument("-v", "--verbose", action="store_true", help="also list the symbols that were skipped")
    args = parser.parse_args()

    project_root = Path(__file__).resolve().parent.parent
    splits = parse_splits(project_root / "config/GUPE8P/splits.txt")
    annotated, entries = parse_symbols(project_root / "config/GUPE8P/symbols.txt")
    spans = symbol_spans(entries)

    proposals: list[dict[str, Any]] = []
    for unit in args.unit:
        data = gather(splits, annotated, project_root, unit, spans)
        if not data["missing"]:
            print(f"{unit}: nothing to add")
        else:
            lines = [new_symbol_line(s) for s in data["missing"]]
            counts = Counter(s["section"] for s in data["missing"])
            order = [s for s in SECTION_ORDER if s in counts]
            print(f"{unit}: {len(lines)} missing " + ", ".join(f"{s} {counts[s]}" for s in order))
            for symbol, line in zip(data["missing"], lines):
                print(f"  {line}")
                proposals.append({"address": symbol["address"], "line": line, "unit": unit})
            annotated.update({(s["section"], s["address"]): {"name": s["name"]} for s in data["missing"]})
            for symbol in data["missing"]:
                touched = spans.setdefault(symbol["section"], [])
                touched.append((symbol["address"], symbol["address"] + symbol["size"], symbol["name"]))
                touched.sort()
        if args.verbose and data["skipped"]:
            print(f"  skipped: {len(data['skipped'])}")
            for s in data["skipped"]:
                print(f"    {s['section']:8} 0x{s['address']:08X} {s['name']} ({s['why']})")

    if not proposals:
        return 0
    if args.dry_run:
        print(f"\n[dry-run] {len(proposals)} annotations not written")
        return 0
    written = apply_annotations(project_root / "config/GUPE8P/symbols.txt", entries, proposals)
    print(f"\nwrote {written} annotations; run configure.py + ninja to re-measure")
    return 0


if __name__ == "__main__":
    sys.exit(main())
