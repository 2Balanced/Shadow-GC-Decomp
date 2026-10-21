#!/usr/bin/env python3
"""Compiler/flag sweep for a single function.

Compiles a C/C++ file with one or more mwcceppc versions and flag sets, then
compares the named function's instructions against the original DOL.
Relocated fields (branch targets, @ha/@l/@sda21 immediates) are masked so
only real codegen differences are reported.

Example:
  python tools/cctest.py test.c main 0x80050FD4 --versions 2.6 2.7 \
      "--flags=-O4,p|-O4,s" "--base=-nodefaults -proc gekko -align powerpc"
"""

import argparse
import itertools
import shlex
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
DOL = ROOT / "orig" / "GUPE8P" / "sys" / "main.dol"
OBJDUMP = BUILD / "binutils" / "powerpc-eabi-objdump.exe"
READELF = BUILD / "binutils" / "powerpc-eabi-readelf.exe"

DEFAULT_BASE = (
    "-nodefaults -proc gekko -align powerpc -enum int -fp hardware "
    "-inline auto -nosyspath -fp_contract on -str reuse -maxerrors 1"
)


def read_dol(addr: int, size: int) -> bytes:
    data = DOL.read_bytes()
    offs = struct.unpack(">18I", data[0:0x48])
    addrs = struct.unpack(">18I", data[0x48:0x90])
    sizes = struct.unpack(">18I", data[0x90:0xD8])
    for o, a, s in zip(offs, addrs, sizes):
        if a <= addr < a + s:
            start = o + (addr - a)
            return data[start : start + size]
    raise ValueError(f"address {addr:#x} not in DOL")


def elf_function(obj: Path, name: str):
    """Return (bytes, set of reloc offsets relative to function) for symbol name."""
    import re

    syms = subprocess.run(
        [str(READELF), "-sW", str(obj)], capture_output=True, text=True
    ).stdout
    func = None
    for line in syms.splitlines():
        parts = line.split()
        if len(parts) >= 8 and parts[7] == name and parts[3] == "FUNC":
            func = (int(parts[1], 16), int(parts[2]), parts[6])
    if func is None:
        raise SystemExit(f"{name} not found in {obj}")
    value, size, shndx = func
    sec = subprocess.run(
        [str(READELF), "-SW", str(obj)], capture_output=True, text=True
    ).stdout
    secname = None
    for line in sec.splitlines():
        m = re.match(r"\s*\[\s*(\d+)\]\s+(\S+)\s+\S+\s+\S+\s+([0-9a-f]+)\s+([0-9a-f]+)", line)
        if m and m.group(1) == shndx:
            secname, off = m.group(2), int(m.group(3), 16)
    raw = obj.read_bytes()[off + value : off + value + size]
    rel = subprocess.run(
        [str(OBJDUMP), "-r", "-j", secname, str(obj)], capture_output=True, text=True
    ).stdout
    relocs = set()
    for line in rel.splitlines():
        parts = line.split()
        if len(parts) >= 3 and parts[1].startswith("R_PPC"):
            try:
                o = int(parts[0], 16)
            except ValueError:
                continue
            if value <= o < value + size:
                relocs.add((o - value) & ~3)
    return raw, relocs


def mask(word: int) -> int:
    op = word >> 26
    if op in (18,):  # b/bl
        return word & 0xFC000003
    if op == 16:  # bc
        return word & 0xFFFF0003
    return word & 0xFFE00000  # rA and immediate of relocated D-form insns (sda21 fills rA at link)


def compare(target: bytes, ours: bytes, relocs):
    diffs = []
    n = max(len(target), len(ours)) // 4
    for i in range(n):
        t = struct.unpack(">I", target[i * 4 : i * 4 + 4])[0] if i * 4 < len(target) else None
        o = struct.unpack(">I", ours[i * 4 : i * 4 + 4])[0] if i * 4 < len(ours) else None
        if t is None or o is None:
            diffs.append(i)
            continue
        if i * 4 in relocs or (t >> 26) in (16, 18):
            if mask(t) != mask(o):
                diffs.append(i)
        elif t != o:
            diffs.append(i)
    return diffs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("source")
    ap.add_argument("function")
    ap.add_argument("address", type=lambda s: int(s, 0))
    ap.add_argument("--size", type=lambda s: int(s, 0))
    ap.add_argument("--versions", nargs="+", default=["2.7"])
    ap.add_argument("--flags", default="-O4,p", help="flag sets separated by '|' (use --flags=...)")
    ap.add_argument("--base", default=DEFAULT_BASE)
    ap.add_argument("--show", action="store_true", help="print disassembly of best result")
    args = ap.parse_args()

    results = []
    with tempfile.TemporaryDirectory() as td:
        for ver, flags in itertools.product(args.versions, args.flags.split("|")):
            obj = Path(td) / "out.o"
            cc = BUILD / "compilers" / "GC" / ver / "mwcceppc.exe"
            cmd = [str(cc)] + shlex.split(args.base) + shlex.split(flags) + ["-c", args.source, "-o", str(obj)]
            p = subprocess.run(cmd, capture_output=True, text=True)
            if p.returncode != 0:
                print(f"[{ver} {flags}] compile error:\n{p.stdout}{p.stderr}")
                continue
            ours, relocs = elf_function(obj, args.function)
            size = args.size or len(ours)
            target = read_dol(args.address, size)
            d = compare(target, ours, relocs)
            status = "MATCH" if not d and len(ours) == len(target) else f"{len(d)} diffs"
            print(f"[{ver:6} {flags:30}] ours={len(ours)//4:4} target={len(target)//4:4} {status}")
            results.append((len(d), ver, flags))
            if args.show and d:
                show_diff(target, ours, set(d), Path(td))
    if not results:
        sys.exit(1)


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


def show_diff(target: bytes, ours: bytes, diffs, tmp: Path):
    t, o = disasm(target, tmp), disasm(ours, tmp)
    for i in range(max(len(t), len(o))):
        mark = "!!" if i in diffs else "  "
        tl = t[i] if i < len(t) else ""
        ol = o[i] if i < len(o) else ""
        print(f"{mark} {i*4:4x}  {tl:40} | {ol}")


if __name__ == "__main__":
    main()
