#!/usr/bin/env python3
"""Rank non-matching units by how close they are to becoming `Matching`.

`build/<config>/ldscript.lcf` places no unit at a fixed address: MWLD lays the
inputs out in `configure.py` order, section by section, so a flipped unit only
has to keep the relinked `main.dol` byte-identical. Its own section sizes may
therefore differ from the retail split's - the drift is absorbed by the
Matching units around it, which is how `Game/CameraShake.cpp` (.text 2640 B
against a 1728 B split) links. What still decides the flip is the byte-for-byte
check, so this tool reports two things per unit:

  1. whether every function and data symbol is matched (`--flip` lists those
     units, and they are the only ones worth trying), and
  2. the signed size drift against the unit's `splits.txt` ranges, which says
     how much the link would move if the unit were flipped.

This reads nothing but build output, so it stays clean-room.

Usage:
    python tools/unitstatus.py [Game/Foo ...]     # only units whose name contains one of these
    python tools/unitstatus.py --flip             # fully matched units, smallest link drift first
    python tools/unitstatus.py [-n 20] [-c] [-a] [-u UNIT] [--sort work|gap|code]
"""

import argparse
import json
import os
import re
import subprocess
import sys

CONFIG = "GUPE8P"
DOL = "sys/main.dol"
OBJDUMP = os.path.join("build", "binutils", "powerpc-eabi-objdump.exe")
SECTIONS = (".text", ".ctors", ".rodata", ".data", ".bss", ".sdata", ".sdata2", ".sbss")

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def source_map():
    """`src/`-relative decomp path -> objdiff unit name, from objdiff.json."""
    with open(os.path.join(root, "objdiff.json"), encoding="utf-8") as handle:
        objdiff = json.load(handle)
    mapping = {}
    for unit in objdiff["units"]:
        source = (unit.get("metadata") or {}).get("source_path", "")
        source = re.sub(r"^src/", "", source.replace("\\", "/"))
        if source:
            mapping[source] = unit["name"]
    return mapping


def configure_units(config, nonmatching_only):
    path = os.path.join(root, "configure.py")
    entries = []
    pattern = re.compile(
        r"^\s*(?:Object|Assembly|TextCode|RawData|BinData|SubTextCode)\s*\(\s*"
        r"(Matching|NonMatching|AlmostMatching|Ignoring)\s*,\s*[\"']([^\"']+)[\"']"
    )
    with open(path, encoding="utf-8") as handle:
        for number, line in enumerate(handle, 1):
            if line.lstrip().startswith("#"):
                continue
            match = pattern.match(line)
            if match:
                kind, path_or_name = match.groups()
                if kind not in ("Matching", "NonMatching"):
                    continue
                entries.append((kind, path_or_name, number))
    # configure.py lists decomp files relative to `src/`; objdiff names the unit
    # after that path, e.g. `src/Game/Foo.cpp` is unit `main/Game/Foo`.
    by_source = source_map()
    units = []
    for kind, path_or_name, number in entries:
        normalised = path_or_name.replace("\\", "/")
        if normalised.startswith("asm/"):
            continue
        if nonmatching_only and kind != "NonMatching":
            continue
        unit = by_source.get(normalised)
        if unit is None:
            unit = "main/" + re.sub(r"\.(cpp|c|s)$", "", normalised)
        units.append(unit)
    return units


def retail_ranges(config, by_source):
    """Per-unit section ranges from splits.txt (headers are src-relative paths)."""
    path = os.path.join(root, "config", config, "splits.txt")
    units, current = {}, None
    with open(path, encoding="utf-8") as handle:
        for line in handle:
            line = line.rstrip("\n")
            if not line.strip() or line.lstrip().startswith("#"):
                continue
            if not line.startswith((" ", "\t")):
                name = line.split()[0].rstrip(":")
                current = by_source.get(name, name)
                units.setdefault(current, {})
                continue
            fields = line.split()
            if current is None or len(fields) < 3 or not fields[1].startswith("start:"):
                continue
            start = int(fields[1][len("start:"):], 16)
            if fields[2].startswith("end:"):
                end = int(fields[2][len("end:"):], 16)
            elif fields[2].startswith("size:"):
                end = start + int(fields[2][len("size:"):], 16)
            else:
                continue
            units[current][fields[0]] = (start, end)
    return units


def our_sizes(config, units):
    build = os.path.join(root, "build", config, "src")
    result = {}
    for unit in units:
        name = re.sub(r"^main/", "", unit).replace("/", os.sep)
        candidate = os.path.join(build, name + ".o")
        if not os.path.exists(candidate):
            continue
        try:
            listing = subprocess.run(
                [os.path.join(root, OBJDUMP), "-h", candidate],
                capture_output=True, text=True, check=True
            ).stdout
        except subprocess.CalledProcessError:
            continue
        sizes = {}
        for line in listing.splitlines():
            parts = line.split()
            # objdump -h: `Idx Name Size VMA LMA File-off Algn`
            if len(parts) == 7 and parts[0].isdigit():
                sizes[parts[1]] = int(parts[2], 16)
        result[unit] = sizes
    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("units", nargs="*", help="only units whose name contains one of these")
    parser.add_argument("-n", type=int, default=0, help="show at most N units")
    parser.add_argument("-c", "--closest", action="store_true", help="only units with zero size gaps")
    parser.add_argument(
        "--flip",
        action="store_true",
        help="only units whose every symbol is matched (flip candidates), sorted by link drift",
    )
    parser.add_argument("-a", "--all-units", action="store_true", help="include Matching units")
    parser.add_argument("-u", metavar="UNIT", help="only this unit")
    parser.add_argument("--sort", choices=("work", "gap", "code"), default="work")
    parser.add_argument("--config", default=CONFIG)
    args = parser.parse_args()

    config = args.config
    wanted = ([args.u] if args.u else []) + list(args.units)
    units = configure_units(config, nonmatching_only=not args.all_units)
    if wanted and not any(any(name in unit for name in wanted) for unit in units):
        # A unit named on the command line may already be Matching, and those are
        # left out of the list above; look in the full list so the filter still hits.
        units = configure_units(config, nonmatching_only=False)
    splits = retail_ranges(config, source_map())
    sizes = our_sizes(config, units)
    report_path = os.path.join(root, "build", config, "report.json")
    with open(report_path, encoding="utf-8") as handle:
        report = json.load(handle)
    measures = {u["name"]: u["measures"] for u in report["units"]}

    rows = []
    for unit in units:
        ranges = splits.get(unit, {})
        ours = sizes.get(unit, {})
        gaps = []
        gap_bytes = 0
        drift = {}
        for section in sorted(set(ranges) | set(ours)):
            if section.startswith(".comment") or section.startswith(".note"):
                continue  # compiler version strings, never part of the game
            retail = ranges[section][1] - ranges[section][0] if section in ranges else 0
            gap = ours.get(section, 0) - retail
            drift[section] = gap
            if gap:
                gaps.append((section, retail, ours.get(section, 0), gap))
            gap_bytes += abs(gap)
        empty = not ours or all(size == 0 for size in ours.values())
        m = measures.get(unit, {})

        def number(key):
            value = m.get(key, 0)
            return int(value or 0)

        def percent(key):
            value = m.get(key, 0.0)
            return float(value) if isinstance(value, str) else (value or 0.0)

        total_code = number("total_code")
        matched_code = number("matched_code")
        rows.append(
            {
                "unit": unit,
                "empty": empty,
                "gap_bytes": gap_bytes,
                "gaps": gaps,
                "drift": drift,
                "all_matched": (
                    number("total_functions") > 0
                    and number("matched_functions") == number("total_functions")
                    and number("matched_data") == number("total_data")
                    and percent("matched_code_percent") >= 100.0
                ),
                "total_code": total_code,
                "matched_code": matched_code,
                "code_percent": percent("matched_code_percent"),
                "fuzzy": percent("fuzzy_match_percent"),
                "fn_matched": number("matched_functions"),
                "fn_total": number("total_functions"),
                "data_matched": number("matched_data"),
                "total_data": number("total_data"),
            }
        )

    if args.sort == "work":
        rows.sort(key=lambda r: (r["gap_bytes"] > 0, -r["code_percent"], r["total_code"]))
    elif args.sort == "gap":
        rows.sort(key=lambda r: (r["gap_bytes"], -r["total_code"]))
    else:
        rows.sort(key=lambda r: (-r["total_code"],))

    exact = sum(1 for r in rows if r["gap_bytes"] == 0)
    matched = sum(1 for r in rows if r["all_matched"])
    scope = "units" if args.all_units else "non-matching units"
    print(
        f"{len(rows)} {scope}, {exact} whose section sizes already equal retail's, "
        f"{matched} with every symbol matched (flip candidates)\n"
    )
    shown = rows
    if args.flip:
        shown = [r for r in rows if r["all_matched"]]
        shown.sort(key=lambda r: (sum(abs(g) for g in r["drift"].values()), -r["total_code"]))
    if args.closest:
        shown = [r for r in shown if r["gap_bytes"] == 0]
    if wanted:
        shown = [r for r in shown if any(name in r["unit"] for name in wanted)]
    header = (
        f"{'unit':36} {'code':>6} {'code%':>6} {'fn':>8} {'fuzzy':>6} {'data':>10}  size gaps"
    )
    print(header)
    print("-" * len(header))
    for row in shown[: args.n or None]:
        flag = " NO-OBJECT" if row["empty"] else ""
        gaps = " ".join(f"{s}:{g:+d}" for s, _, _, g in row["gaps"]) or "none"
        print(
            f"{row['unit']:36} {row['total_code']:6} {row['code_percent']:6.1f} "
            f"{row['fn_matched']:3}/{row['fn_total']:<4} {row['fuzzy']:6.2f} "
            f"{row['data_matched']:4}/{row['total_data']:<5} {gaps}{flag}"
        )
        if wanted and row["gaps"]:
            for section, retail, ours_size, gap in row["gaps"]:
                print(f"    {section:9} retail {retail:6} ours {ours_size:6} {gap:+}")


if __name__ == "__main__":
    main()
