#!/usr/bin/env python3
"""Rename symbols in config/<VERSION>/symbols.txt by address.

Usage: python tools/rename_symbols.py 0x8011DC3C=Instantiate__11SetParticleFP21UnkPlacement_800C9CD0 ...
       python tools/rename_symbols.py --file renames.txt   (one ADDRESS=NAME per line)
Each address must name exactly one existing symbol; the attributes are kept. A rename to a name
that already exists elsewhere is refused unless the existing symbol is scope:local.
"""

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LINE = re.compile(r"^(\S+)( = \S+:0x)([0-9A-Fa-f]+)(;.*)$")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("renames", nargs="*")
    ap.add_argument("--file")
    ap.add_argument("--version", default="GUPE8P")
    args = ap.parse_args()
    pairs = list(args.renames)
    if args.file:
        pairs += [l.strip() for l in Path(args.file).read_text().splitlines() if l.strip() and not l.startswith("#")]
    wanted = {}
    for p in pairs:
        addr, name = p.split("=", 1)
        wanted[int(addr, 16)] = name.strip()

    path = ROOT / "config" / args.version / "symbols.txt"
    lines = path.read_text().splitlines()
    existing = {}
    for l in lines:
        m = LINE.match(l)
        if m:
            existing.setdefault(m.group(1), []).append((int(m.group(3), 16), "scope:local" in l))
    hits = {a: 0 for a in wanted}
    out = []
    for l in lines:
        m = LINE.match(l)
        if m and int(m.group(3), 16) in wanted:
            a = int(m.group(3), 16)
            new = wanted[a]
            for other_addr, is_local in existing.get(new, []):
                if other_addr != a and not is_local and "scope:local" not in l:
                    sys.exit(f"{new} already names {other_addr:#x}")
            hits[a] += 1
            l = new + m.group(2) + m.group(3) + m.group(4)
        out.append(l)
    bad = [f"{a:#x}" for a, n in hits.items() if n != 1]
    if bad:
        sys.exit(f"addresses without exactly one symbol: {bad}")
    path.write_text("\n".join(out) + "\n")
    print(f"renamed {len(wanted)} symbols")


if __name__ == "__main__":
    main()
