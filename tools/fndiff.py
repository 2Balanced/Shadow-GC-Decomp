#!/usr/bin/env python3
"""Instruction-level diff of one function: retail's split object against ours.

`fnstatus.py` says which function is not 100% yet; this shows why.  objdiff's
percent hides whether the gap is one swapped instruction, a different register
choice or a whole block that MWCC ordered differently, and that distinction
decides whether the function can be matched at all.

Both sides are disassembled from the build tree with binutils'
`--disassemble=`, which is the only form that works here: our objects' sections
all have VMA 0 and `main.dol` is not an ELF.  The address column is dropped and
branches are rewritten as the distance they jump, so the diff reports code
differences rather than where the unit happens to sit, and `-dr` supplies the
symbol behind every branch, `lis`/`ori` pair and load: a call that goes somewhere
else than retail's, or an `lis` that points at another global, shows up as text.
Reads build output only, so it stays clean-room.

Usage:
    python tools/fndiff.py Game/UnkImmediateDraw_800396D0 fn_80039920
    python tools/fndiff.py Game/GoalAction SaveResult__Q211StageAction10GoalActionFv --context 3
"""

import argparse
import difflib
import os
import re
import subprocess
import sys

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJDUMP = os.path.join(root, "build", "binutils", "powerpc-eabi-objdump.exe")

INSTRUCTION = re.compile(r"^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2}[ \t]+)+)(.*)$")
RELOCATION = re.compile(r"^\s*[0-9a-f]+:\s+(R_\w+)\s+(.*)$")


def object_path(kind, unit):
    """`build/GUPE8P/<kind>/<unit>.o`, trying the prefixes the registry mixes."""
    base = os.path.join(root, "build", "GUPE8P", kind)
    for prefix in ("", "main/", "Game/"):
        name = unit[len(prefix):] if unit.startswith(prefix + "/") else unit
        path = os.path.join(base, name.replace("/", os.sep) + ".o")
        if os.path.exists(path):
            return path
    return None


def disassemble(unit, symbol):
    path = object_path("obj", unit)
    ours = object_path("src", unit)
    if path is None or ours is None:
        sys.exit(f"no {unit}.o in build/GUPE8P/obj and build/GUPE8P/src")
    out = {}
    for label, obj in (("retail", path), ("ours", ours)):
        # -dr also prints the relocation of every instruction, which is where the
        # real target of a branch or the global of an lis/ori pair comes from.
        text = subprocess.run(
            [OBJDUMP, "-dr", f"--disassemble={symbol}", obj],
            capture_output=True, text=True, errors="replace").stdout
        rows = []
        for line in text.splitlines():
            match = INSTRUCTION.match(line)
            if match:
                words = " ".join(match.group(2).split())
                body = match.group(3).strip()
                # objdump's own `<address>' is the position in this object and says
                # nothing; a branch becomes the distance it jumps, and the
                # relocation line below names the symbol a call or lis refers to.
                address = int(match.group(1), 16)
                branch = re.match(r"^(b|ba|b[cl][ael]*|bl)\s+([0-9a-f]+)$",
                                  body.split(" <")[0].strip())
                if branch:
                    target = int(branch.group(2), 16)
                    body = f"{branch.group(1)} +{target - address:#x}"
                else:
                    body = body.split(" <")[0].strip()
                rows.append([words, body])
                continue
            reloc = RELOCATION.match(line)
            if reloc and rows:
                target = reloc.group(2).strip()
                rows[-1][1] = f"{rows[-1][1]} -> {target}"
        out[label] = [f"{words:<12} {body}" for words, body in rows]
    return out["retail"], out["ours"]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit", help="unit as in configure.py, e.g. Game/GoalAction")
    ap.add_argument("symbol", help="function to compare")
    ap.add_argument("--context", type=int, default=2, help="diff context lines")
    ap.add_argument("--side", action="store_true", help="list both disassemblies")
    args = ap.parse_args()

    retail, ours = disassemble(args.unit, args.symbol)
    print(f"=== {args.symbol} ({args.unit}): retail {len(retail)} instructions, "
          f"ours {len(ours)}")
    if not retail:
        print("retail's object has no such symbol; check the name in symbols.txt")
        return
    if args.side:
        for label, rows in (("retail", retail), ("ours", ours)):
            print(f"--- {label}")
            for row in rows:
                print(f"  {row}")
        return
    diff = [line for line in difflib.unified_diff(retail, ours, fromfile="retail",
                                                  tofile="ours", n=args.context,
                                                  lineterm="")
            if not line.startswith(("---", "+++"))]
    if not diff:
        print("identical instructions")
        return
    hunks = sum(1 for line in diff if line.startswith("@@"))
    removed = sum(1 for line in diff if line.startswith("-"))
    added = sum(1 for line in diff if line.startswith("+"))
    for line in diff:
        print(line)
    print(f"--- {hunks} differing run(s): {removed} retail instruction(s), {added} ours")


main()
