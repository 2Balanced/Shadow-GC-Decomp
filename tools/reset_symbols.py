#!/usr/bin/env python3
"""Reset symbols.txt names back to placeholders so tools/pair_symbols.py can pair them again.

Use it after a type or header change alters mangled names that are already in symbols.txt (for
example when two units named the same class differently).

Usage:
  python tools/reset_symbols.py --match UnkCollider_800C7A84 --match UnkRumble_8034DE74
  python tools/reset_symbols.py --stale Game/FallRoad      # names in the unit's ranges that its
                                                         # compiled object no longer defines
Functions become fn_<ADDR> and data lbl_<ADDR>; the attributes after "//" are kept.
Then rebuild and run `python tools/pair_symbols.py <unit> --apply` for each affected unit.
"""

import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
from cctest import READELF  # noqa: E402

SYMLINE = re.compile(r"^(\S+) = (\S+):0x([0-9A-Fa-f]+); // (.*)$")
PLACEHOLDER = re.compile(r"^(fn|lbl|dtor|ctor|jumptable|gap)_[0-9A-Fa-f]+$|^@\d+$")


def unit_ranges(version, unit):
    out, cur = [], None
    for line in (ROOT / "config" / version / "splits.txt").read_text().splitlines():
        if line and not line[0].isspace() and line.endswith(":"):
            cur = line[:-1]
            continue
        m = re.match(r"\s+(\S+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", line)
        if m and cur and cur.rsplit(".", 1)[0] == unit:
            out.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16)))
    return out


def object_names(version, unit):
    obj = ROOT / "build" / version / "src" / f"{unit}.o"
    text = subprocess.run([str(READELF), "-sW", str(obj)], capture_output=True, text=True).stdout
    names = set()
    for line in text.splitlines():
        parts = line.split()
        if len(parts) >= 8 and parts[0].rstrip(":").isdigit():
            names.add(parts[7])
    return names


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--version", default="GUPE8P")
    ap.add_argument("--match", action="append", default=[], help="substring of names to reset")
    ap.add_argument("--stale", action="append", default=[], help="unit whose stale names to reset")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    stale = []
    for unit in args.stale:
        stale.append((unit_ranges(args.version, unit), object_names(args.version, unit)))

    path = ROOT / "config" / args.version / "symbols.txt"
    lines = path.read_text().splitlines(True)
    count = 0
    for i, line in enumerate(lines):
        m = SYMLINE.match(line.rstrip("\n"))
        if not m or PLACEHOLDER.match(m.group(1)):
            continue
        name, sec, addr, attrs = m.group(1), m.group(2), int(m.group(3), 16), m.group(4)
        hit = any(s in name for s in args.match)
        for ranges, names in stale:
            if any(sec == s and lo <= addr < hi for s, lo, hi in ranges) and name not in names:
                hit = True
        if not hit:
            continue
        new = ("fn_" if "type:function" in attrs else "lbl_") + f"{addr:08X}"
        print(f"{name[:110]} -> {new}")
        lines[i] = f"{new} = {sec}:0x{addr:08X}; // {attrs}\n"
        count += 1
    if not args.dry_run:
        path.write_text("".join(lines))
    print(f"{count} name(s) reset")


if __name__ == "__main__":
    main()
