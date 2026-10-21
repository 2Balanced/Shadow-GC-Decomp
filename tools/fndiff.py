#!/usr/bin/env python3
"""Side-by-side diff of one compiled function against the retail DOL.

Usage:
  python tools/fndiff.py build/GUPE8P/src/Game/JumpBoard.o HitCharacter 0x8010F028
The symbol may be a substring of the object's (mangled) name when it is unique. The target size
comes from symbols.txt (any symbol at the address) unless --size is given. Instructions whose only
difference is a relocated field (branch target, @ha/@l/@sda21 immediate) are not flagged.
"""

import argparse
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from cctest import OBJDUMP, READELF, compare, read_dol  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent


def find_symbol(obj: Path, wanted: str):
    out = subprocess.run([str(READELF), "-sW", str(obj)], capture_output=True, text=True).stdout
    cands = []
    for line in out.splitlines():
        parts = line.split()
        if len(parts) >= 8 and parts[3] == "FUNC" and (parts[7] == wanted or wanted in parts[7]):
            cands.append(parts[7])
    exact = [c for c in cands if c == wanted]
    if exact:
        return exact[0]
    if len(cands) != 1:
        sys.exit(f"symbol {wanted!r} matches {len(cands)} functions: {cands[:10]}")
    return cands[0]


def target_size(addr: int, version: str):
    for line in (ROOT / "config" / version / "symbols.txt").read_text().splitlines():
        m = re.match(r"\S+ = \.text:0x([0-9A-Fa-f]+); // .*size:0x([0-9A-Fa-f]+)", line)
        if m and int(m.group(1), 16) == addr:
            return int(m.group(2), 16)
    sys.exit(f"no symbol with a size at {addr:#x}")


def disasm(code: bytes, tmp: Path):
    f = tmp / "code.bin"
    f.write_bytes(code)
    out = subprocess.run(
        [str(OBJDUMP), "-D", "-b", "binary", "-m", "powerpc:750", "-EB", str(f)],
        capture_output=True, text=True,
    ).stdout
    lines = []
    for line in out.splitlines():
        parts = line.split("\t")
        if len(parts) >= 3 and parts[0].strip().endswith(":"):
            lines.append(" ".join(p.strip() for p in parts[2:]))
    return lines


def main():
    import tempfile

    from cctest import elf_function

    ap = argparse.ArgumentParser()
    ap.add_argument("object")
    ap.add_argument("symbol")
    ap.add_argument("address", type=lambda s: int(s, 0))
    ap.add_argument("--size", type=lambda s: int(s, 0))
    ap.add_argument("--version", default="GUPE8P")
    ap.add_argument("--all", action="store_true", help="print matching lines too")
    args = ap.parse_args()

    obj = Path(args.object)
    name = find_symbol(obj, args.symbol)
    ours, relocs = elf_function(obj, name)
    size = args.size or target_size(args.address, args.version)
    target = read_dol(args.address, size)
    diffs = set(compare(target, ours, relocs))
    print(f"{name}\n  ours {len(ours):#x}  target {size:#x}  differing instructions: {len(diffs)}")
    with tempfile.TemporaryDirectory() as td:
        t, o = disasm(target, Path(td)), disasm(ours, Path(td))
    for i in range(max(len(t), len(o))):
        mark = "!!" if i in diffs else "  "
        if mark == "  " and not args.all:
            continue
        tl = t[i] if i < len(t) else ""
        ol = o[i] if i < len(o) else ""
        print(f"{mark} {i * 4:4x}  {tl:42} | {ol}")


if __name__ == "__main__":
    main()
