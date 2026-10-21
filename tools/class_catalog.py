#!/usr/bin/env python3
"""Catalog the C++ classes in the retail DOL from their vtables and RTTI.

MWCC vtables are laid out as [RTTI record pointer][this adjustment][virtual functions...], and an
RTTI record (in .sdata/.data) is [class name string][base list or 0]. Every pointer in .data/.sdata
that leads to such a record and is followed by a zero word and code pointers is taken as a vtable.

Usage:
  python tools/class_catalog.py                    # one line per vtable, sorted by address
  python tools/class_catalog.py --units            # classes grouped by configured unit (or "-")
  python tools/class_catalog.py --csv out.csv      # full table: vtable, class, bases, methods
Classes in unconfigured code are the candidates for new units: the lowest and highest method
addresses bracket where the class's code lives.
"""

import argparse
import csv
import re
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TEXT = (0x80003100, 0x804A9C00)


class Dol:
    def __init__(self, path):
        self.data = path.read_bytes()
        offs = struct.unpack(">18I", self.data[0:0x48])
        addrs = struct.unpack(">18I", self.data[0x48:0x90])
        sizes = struct.unpack(">18I", self.data[0x90:0xD8])
        self.secs = [(a, s, o, i) for i, (o, a, s) in enumerate(zip(offs, addrs, sizes)) if s]

    def read(self, addr, n):
        for a, s, o, _ in self.secs:
            if a <= addr and addr + n <= a + s:
                return self.data[o + addr - a: o + addr - a + n]
        return None

    def u32(self, addr):
        b = self.read(addr, 4)
        return struct.unpack(">I", b)[0] if b else None

    def cstr(self, addr, limit=400):
        b = self.read(addr, 1)
        if b is None:
            return None
        out = bytearray()
        while len(out) < limit:
            c = self.read(addr + len(out), 1)
            if not c or c == b"\0":
                break
            out += c
        try:
            s = out.decode("ascii")
        except UnicodeDecodeError:
            return None
        return s if s and all(32 <= ord(ch) < 127 for ch in s) else None

    def data_ranges(self):
        return [(a, s) for a, s, o, i in self.secs if i >= 7]


def rtti_name(dol, rec):
    name_ptr = dol.u32(rec)
    if name_ptr is None or not (0x80000000 <= name_ptr < 0x81800000):
        return None
    name = dol.cstr(name_ptr)
    if not name or not re.match(r"^[A-Za-z_@][\w:<>,*& @()\[\].-]*$", name):
        return None
    return name


def bases(dol, rec):
    lst = dol.u32(rec + 4)
    out = []
    if lst and 0x80000000 <= lst < 0x81800000:
        a = lst
        for _ in range(16):
            r = dol.u32(a)
            if not r:
                break
            n = rtti_name(dol, r)
            if not n:
                break
            out.append(n)
            a += 8
    return out


def scan(dol):
    found = []
    for start, size in dol.data_ranges():
        a = start
        while a + 12 <= start + size:
            rec = dol.u32(a)
            if rec and 0x80000000 <= rec < 0x81800000 and dol.u32(a + 4) == 0:
                name = rtti_name(dol, rec)
                if name:
                    methods = []
                    m = a + 8
                    while True:
                        f = dol.u32(m)
                        if f is None or not (TEXT[0] <= f < TEXT[1]):
                            break
                        methods.append(f)
                        m += 4
                    if methods:
                        found.append((a, name, bases(dol, rec), methods))
                        a = m
                        continue
            a += 4
    return found


def units(version):
    spans = []
    cur = None
    for line in (ROOT / "config" / version / "splits.txt").read_text().splitlines():
        if line and not line[0].isspace() and line.endswith(":"):
            cur = line[:-1]
            continue
        m = re.match(r"\s+\.text\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", line)
        if m and cur:
            spans.append((int(m.group(1), 16), int(m.group(2), 16), cur))
    return spans


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--version", default="GUPE8P")
    ap.add_argument("--units", action="store_true")
    ap.add_argument("--csv")
    args = ap.parse_args()
    dol = Dol(ROOT / "orig" / args.version / "sys" / "main.dol")
    vts = scan(dol)
    spans = units(args.version)

    def unit_of(addr):
        for s, e, u in spans:
            if s <= addr < e:
                return u
        return "-"

    if args.csv:
        with open(args.csv, "w", newline="") as f:
            w = csv.writer(f)
            w.writerow(["vtable", "class", "bases", "lowest_method", "highest_method", "methods", "unit"])
            for a, n, b, ms in vts:
                own = [x for x in ms if x > 0x80008D40]
                w.writerow([f"{a:08X}", n, "|".join(b), f"{min(ms):08X}", f"{max(ms):08X}",
                            " ".join(f"{x:08X}" for x in ms), unit_of(max(ms))])
        print(f"wrote {len(vts)} vtables to {args.csv}")
        return
    if args.units:
        by = {}
        for a, n, b, ms in vts:
            by.setdefault(unit_of(ms[0]), []).append(n)
        for u in sorted(by):
            print(f"{u}: {', '.join(sorted(set(by[u])))}")
        return
    for a, n, b, ms in vts:
        print(f"{a:08X} {n:60} methods {min(ms):08X}-{max(ms):08X} ({len(ms)}) bases={','.join(b)} unit={unit_of(ms[0])}")


if __name__ == "__main__":
    main()
