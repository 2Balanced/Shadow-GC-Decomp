#!/usr/bin/env python3
"""Compare two PowerPC ELF relocatable objects for exact equality.

Checks, per allocated section (matched by name): the section bytes, every relocation
(offset, type, target symbol name, addend) and every symbol defined in it (name, value, size,
type, binding). Undefined symbols are compared as a set of names. Local symbol names such as
the compiler's @N are compared literally.

Usage:
    python tools/objcompare.py ours.o reference.o            # one pair
    python tools/objcompare.py --dir ours_dir ref_dir [units] # Unit.o in both directories

Exit status is 0 when every pair is identical.
"""

import argparse
import os
import struct
import sys

SHT_SYMTAB = 2
SHT_RELA = 4
SHT_NOBITS = 8

RELOC_NAMES = {
    0: "NONE", 1: "ADDR32", 2: "ADDR24", 3: "ADDR16", 4: "ADDR16_LO", 5: "ADDR16_HI",
    6: "ADDR16_HA", 7: "ADDR14", 10: "REL24", 11: "REL14", 26: "REL32", 109: "EMB_SDA21",
}
STT = {0: "NOTYPE", 1: "OBJECT", 2: "FUNC", 3: "SECTION", 4: "FILE"}
STB = {0: "LOCAL", 1: "GLOBAL", 2: "WEAK"}


class Elf:
    def __init__(self, path):
        with open(path, "rb") as f:
            self.data = f.read()
        d = self.data
        if d[:4] != b"\x7fELF":
            raise ValueError(f"{path}: not an ELF file")
        (self.shoff,) = struct.unpack_from(">I", d, 0x20)
        self.shentsize, self.shnum, self.shstrndx = struct.unpack_from(">HHH", d, 0x2E)
        self.sections = []
        for i in range(self.shnum):
            fields = struct.unpack_from(">IIIIIIIIII", d, self.shoff + i * self.shentsize)
            self.sections.append(dict(zip(
                ("name_off", "type", "flags", "addr", "offset", "size", "link", "info",
                 "align", "entsize"), fields)))
        shstr = self.sections[self.shstrndx]
        for s in self.sections:
            s["name"] = self._str(shstr, s["name_off"])
        self.symbols = []
        for s in self.sections:
            if s["type"] == SHT_SYMTAB:
                strtab = self.sections[s["link"]]
                for j in range(s["size"] // 16):
                    name, value, size, info, other, shndx = struct.unpack_from(
                        ">IIIBBH", d, s["offset"] + j * 16)
                    self.symbols.append(dict(
                        name=self._str(strtab, name), value=value, size=size,
                        type=STT.get(info & 0xF, str(info & 0xF)),
                        bind=STB.get(info >> 4, str(info >> 4)), shndx=shndx))

    def _str(self, sec, off):
        start = sec["offset"] + off
        end = self.data.index(b"\0", start)
        return self.data[start:end].decode("latin-1")

    def section_bytes(self, s):
        if s["type"] == SHT_NOBITS:
            return None
        return self.data[s["offset"]:s["offset"] + s["size"]]

    def sym_name(self, idx):
        sym = self.symbols[idx]
        if sym["type"] == "SECTION":
            return "[section " + self.sections[sym["shndx"]]["name"] + "]"
        return sym["name"]

    def relocs(self, target_index):
        out = []
        for s in self.sections:
            if s["type"] == SHT_RELA and s["info"] == target_index:
                for j in range(s["size"] // 12):
                    off, info, addend = struct.unpack_from(">IIi", self.data, s["offset"] + j * 12)
                    out.append((off, RELOC_NAMES.get(info & 0xFF, str(info & 0xFF)),
                                self.sym_name(info >> 8), addend))
        return sorted(out)

    def content_sections(self):
        """Sections compared by content: everything except symbol/string/relocation tables."""
        return {s["name"]: (i, s) for i, s in enumerate(self.sections)
                if i != 0 and s["type"] not in (SHT_SYMTAB, SHT_RELA, 3)}


def symbol_at(elf, sec_index, off):
    best = None
    for sym in elf.symbols:
        if sym["shndx"] == sec_index and sym["type"] in ("FUNC", "OBJECT") \
                and sym["value"] <= off < sym["value"] + max(sym["size"], 1):
            best = sym["name"]
    return best or "?"


def compare(ours_path, ref_path, skip_comment=False):
    """Returns a list of difference descriptions (empty when identical)."""
    ours, ref = Elf(ours_path), Elf(ref_path)
    diffs = []
    oc, rc = ours.content_sections(), ref.content_sections()
    for name in sorted(set(oc) | set(rc)):
        if skip_comment and name == ".comment":
            continue
        if name not in oc or name not in rc:
            diffs.append(f"section {name}: only in {'ours' if name in oc else 'reference'}")
            continue
        (oi, os_), (ri, rs) = oc[name], rc[name]
        if os_["size"] != rs["size"]:
            diffs.append(f"section {name}: size 0x{os_['size']:X} vs reference 0x{rs['size']:X}")
        ob, rb = ours.section_bytes(os_), ref.section_bytes(rs)
        if ob is not None and rb is not None and ob != rb:
            n = min(len(ob), len(rb))
            first = next((k for k in range(n) if ob[k] != rb[k]), n)
            diffs.append(f"section {name}: bytes differ from 0x{first:X} "
                         f"(in {symbol_at(ref, ri, first)})")
        orl, rrl = ours.relocs(oi), ref.relocs(ri)
        if orl != rrl:
            oset, rset = set(orl), set(rrl)
            extra = sorted(oset - rset)
            missing = sorted(rset - oset)
            diffs.append(f"section {name}: relocations differ "
                         f"({len(extra)} only ours, {len(missing)} only reference)")
            for r in missing[:8]:
                diffs.append(f"    ref : 0x{r[0]:X} {r[1]} {r[2]} {r[3]:+d}")
            for r in extra[:8]:
                diffs.append(f"    ours: 0x{r[0]:X} {r[1]} {r[2]} {r[3]:+d}")

    def symset(elf):
        out = {}
        for sym in elf.symbols:
            if sym["type"] in ("SECTION", "FILE") or sym["name"] == "":
                continue
            if sym["shndx"] == 0:
                key = ("UND", sym["name"])
                out[key] = ("UND",)
            else:
                sec = elf.sections[sym["shndx"]]["name"] if sym["shndx"] < 0xFF00 else "ABS"
                key = (sec, sym["name"])
                out[key] = (sym["value"], sym["size"], sym["type"], sym["bind"])
        return out

    osy, rsy = symset(ours), symset(ref)
    sym_diffs = []
    for key in sorted(set(osy) | set(rsy)):
        a, b = osy.get(key), rsy.get(key)
        if a == b:
            continue
        if a is None:
            sym_diffs.append(f"    ref only : {key[0]} {key[1]} {b}")
        elif b is None:
            sym_diffs.append(f"    ours only: {key[0]} {key[1]} {a}")
        else:
            sym_diffs.append(f"    differs  : {key[0]} {key[1]} ours {a} ref {b}")
    if sym_diffs:
        diffs.append(f"symbols: {len(sym_diffs)} differences")
        diffs.extend(sym_diffs)
    return diffs


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("paths", nargs="*")
    p.add_argument("--dir", nargs=2, metavar=("OURS", "REF"))
    p.add_argument("-q", "--quiet", action="store_true")
    p.add_argument("-n", "--max-lines", type=int, default=40)
    p.add_argument("--skip-comment", action="store_true", help="ignore the .comment section")
    a = p.parse_args()
    pairs = []
    if a.dir:
        units = a.paths or sorted(f[:-2] for f in os.listdir(a.dir[1]) if f.endswith(".o"))
        for u in units:
            pairs.append((u, os.path.join(a.dir[0], u + ".o"), os.path.join(a.dir[1], u + ".o")))
    elif len(a.paths) == 2:
        pairs.append((os.path.basename(a.paths[0]), a.paths[0], a.paths[1]))
    else:
        p.error("give two objects or --dir OURS REF [units]")
    ok_all = True
    for unit, ours, ref in pairs:
        if not os.path.exists(ours):
            print(f"{unit}: MISSING ({ours})")
            ok_all = False
            continue
        diffs = compare(ours, ref, skip_comment=a.skip_comment)
        print(f"{unit}: {'DIFFERENT' if diffs else 'IDENTICAL'}")
        if not a.quiet:
            for line in diffs[:a.max_lines]:
                print("  " + line)
            if len(diffs) > a.max_lines:
                print(f"  ... {len(diffs) - a.max_lines} more")
        ok_all &= not diffs
    sys.exit(0 if ok_all else 1)


if __name__ == "__main__":
    main()
