#!/usr/bin/env python3
"""Side-by-side instruction diff for one symbol: our object vs the retail split.

Both objects are relocatable and carry the same symbol name, so the comparison
is symbol-relative and free of the padding noise a whole-section diff hits.

    ours:    build/<cfg>/src/<Unit>.o
    retail:  build/<cfg>/obj/<Unit>.o

Branch targets and relocation annotations are stripped, because those differ for
reasons that have nothing to do with the source (section addresses).

Usage: python tools/fnside.py <unit> <symbol> [-c CTX] [--config GUPE8P] [-a]
"""

import argparse
import difflib
import os
import re
import subprocess
import sys

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJDUMP = os.path.join(root, "build", "binutils", "powerpc-eabi-objdump.exe")


def instructions(obj, symbol):
    """[(offset, hexbytes, normalized text)] for one symbol of an object file."""
    out = subprocess.run([OBJDUMP, "-d", f"--disassemble={symbol}", obj],
                         capture_output=True, text=True).stdout
    ins = []
    for line in out.splitlines():
        fields = line.split("\t")
        if len(fields) < 3:
            continue
        off_field, hex_field, text = fields[0], fields[1], "\t".join(fields[2:])
        off_field = off_field.strip().rstrip(":")
        if not re.fullmatch(r"[0-9a-f]+", off_field or ""):
            continue
        hex_field = hex_field.strip()
        if not re.fullmatch(r"(?:[0-9a-f]{2} ?){4}", hex_field.replace(" ", "")):
            continue
        text = re.sub(r"<[^>]*>", "", text).strip()
        # drop bare branch/call targets left over by the annotation strip
        text = re.sub(r"^(bl|b|ba|b\w*)\s+[0-9a-fx+-]+\s*$", r"\1", text)
        ins.append((int(off_field, 16), hex_field.replace(" ", ""), text))
    if not ins:
        return ins
    base = ins[0][0]
    return [(off - base, hx, tx) for off, hx, tx in ins]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit")
    ap.add_argument("symbol")
    ap.add_argument("-c", "--ctx", type=int, default=3)
    ap.add_argument("--config", default="GUPE8P")
    ap.add_argument("-a", "--all", action="store_true", help="print every row")
    args = ap.parse_args()

    cfg = args.config
    ours = os.path.join(root, "build", cfg, "src", args.unit + ".o")
    retail = os.path.join(root, "build", cfg, "obj", args.unit + ".o")
    for path in (ours, retail):
        if not os.path.exists(path):
            sys.exit(f"missing object: {path}")

    a = instructions(retail, args.symbol)   # retail first: it is the reference
    b = instructions(ours, args.symbol)
    if not a or not b:
        sys.exit(f"no disassembly: retail {len(a)} ours {len(b)} "
                 f"(wrong symbol name or the unit has no such function)")
    fa = [i[2] for i in a]
    fb = [i[2] for i in b]
    sm = difflib.SequenceMatcher(None, fa, fb, autojunk=False)
    ops = [o for o in sm.get_opcodes()]
    nbad = sum(max(j - i, k - l) for tag, i, j, k, l in ops if tag != "equal")
    print(f"{args.symbol}: retail {len(a)} insns @{a[0][0]:#x}..{a[-1][0]:#x}, "
          f"ours {len(b)} insns, {nbad} differing rows")

    bad = set()
    for tag, i, j, k, l in ops:
        if tag != "equal":
            bad.update(range(i, j))
    if args.all:
        print(f"{'':2}{'offset':>7}  {'retail bytes':<10}{'retail':<40}ours")
        for n in range(max(len(a), len(b))):
            mark = "!" if n in bad else " "
            ra = f"{a[n][1]:<10}{fa[n]}" if n < len(a) else ""
            rb = fb[n] if n < len(b) else ""
            off = a[n][0] if n < len(a) else b[n][0]
            print(f"{mark} +0x{off:04x}  {ra:<51}{rb}")
    else:
        for tag, i, j, k, l in ops:
            if tag == "equal":
                continue
            print(f"\n--- {tag} retail +0x{a[i][0] if i < len(a) else '?':x} "
                  f"[{i}:{j}] ours [{k}:{l}]")
            lo = max(0, i - args.ctx)
            hi = min(len(a), j + args.ctx)
            for n in range(lo, hi):
                mark = "!" if n in bad else " "
                print(f" {mark} +0x{a[n][0]:04x}  {a[n][1]:<10}{fa[n]:<40}"
                      f"{fb[n] if n < len(fb) else ''}")
    print(f"\nreg: retail regs vs ours, differing rows = {nbad}")


if __name__ == "__main__":
    main()
