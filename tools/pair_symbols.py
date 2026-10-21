#!/usr/bin/env python3
"""Propose symbols.txt renames for a unit by pairing its compiled object with the retail DOL.

Usage:
  python tools/pair_symbols.py Game/LongSpring            # print proposals
  python tools/pair_symbols.py Game/LongSpring --apply    # write them to symbols.txt

How it works:
1. The unit's .text range comes from splits.txt and its object from build/<VERSION>/src/<unit>.o.
2. Each retail function in the range is paired with an object function: by identical name first,
   then by identical size and identical code (relocated fields masked), then by identical size
   when exactly one candidate of that size is left.
3. For every paired function, each relocation in the object names a symbol; the retail
   instruction at the same offset gives the address it refers to (branches, @ha/@l pairs,
   @sda21). Where the retail symbol at that address still has a placeholder name (fn_, lbl_,
   dtor_, @...), the object's name is proposed for it.
Only placeholder names are ever replaced, and an object name is only proposed for one address.
Local data names (@N) are proposed with scope:local so objdiff pairs the data as well.
"""

import argparse
import re
import struct
import subprocess
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
from cctest import OBJDUMP, READELF, read_dol  # noqa: E402

SDA_BASE = 0x805EB440
SDA2_BASE = 0x805F96E0
PLACEHOLDER = re.compile(r"^(fn|lbl|dtor|ctor|jumptable|gap)_[0-9A-Fa-f]+$|^@\d+$")
# Function-local statics carry the compiler's number (s_low$713), which shifts with header changes.
LOCAL_STATIC = re.compile(r"^(.+)\$\d+$")


def renameable(cur, new):
    """True when cur may be replaced by new: a placeholder, or the same local static renumbered."""
    if PLACEHOLDER.match(cur):
        return True
    a, b = LOCAL_STATIC.match(cur), LOCAL_STATIC.match(new)
    return bool(a and b and a.group(1) == b.group(1))
SYMLINE = re.compile(r"^(\S+) = (\S+):0x([0-9A-Fa-f]+); // (.*)$")


def unit_text_range(version, unit):
    cur = None
    for line in (ROOT / "config" / version / "splits.txt").read_text().splitlines():
        if line and not line[0].isspace() and line.endswith(":"):
            cur = line[:-1]
            continue
        m = re.match(r"\s+(\S+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", line)
        if m and cur in (unit + ".cpp", unit + ".c", unit + ".cp") and m.group(1) == ".text":
            return int(m.group(2), 16), int(m.group(3), 16)
    sys.exit(f"no .text split for {unit}")


def unit_ranges(version, unit):
    """All of the unit's section ranges from splits.txt: {section: (start, end)}."""
    cur, out = None, {}
    for line in (ROOT / "config" / version / "splits.txt").read_text().splitlines():
        if line and not line[0].isspace() and line.endswith(":"):
            cur = line[:-1]
            continue
        m = re.match(r"\s+(\S+)\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)", line)
        if m and cur in (unit + ".cpp", unit + ".c", unit + ".cp"):
            out[m.group(1)] = (int(m.group(2), 16), int(m.group(3), 16))
    return out


def object_path(version, unit):
    for ext in (".o",):
        p = ROOT / "build" / version / "src" / (unit + ext)
        if p.exists():
            return p
    sys.exit(f"object for {unit} not built (run ninja)")


def load_symbols(version):
    syms = []
    for line in (ROOT / "config" / version / "symbols.txt").read_text().splitlines():
        m = SYMLINE.match(line)
        if m:
            syms.append((m.group(1), m.group(2), int(m.group(3), 16), m.group(4)))
    return syms


def elf_sections(obj):
    out = subprocess.run([str(READELF), "-SW", str(obj)], capture_output=True, text=True).stdout
    secs = {}
    for line in out.splitlines():
        m = re.match(r"\s*\[\s*(\d+)\]\s+(\S+)\s+\S+\s+\S+\s+([0-9a-f]+)\s+([0-9a-f]+)", line)
        if m:
            secs[int(m.group(1))] = (m.group(2), int(m.group(3), 16), int(m.group(4), 16))
    return secs


def elf_functions(obj):
    out = subprocess.run([str(READELF), "-sW", str(obj)], capture_output=True, text=True).stdout
    funcs = []
    secs = elf_sections(obj)
    data = obj.read_bytes()
    for line in out.splitlines():
        p = line.split()
        if len(p) >= 8 and p[3] == "FUNC" and p[6].isdigit():
            sec = secs[int(p[6])]
            if sec[0] != ".text":
                continue
            value, size = int(p[1], 16), int(p[2])
            funcs.append((p[7], value, size, data[sec[1] + value: sec[1] + value + size]))
    return funcs


def local_symbol_names(obj):
    out = subprocess.run([str(READELF), "-sW", str(obj)], capture_output=True, text=True).stdout
    return {p[7] for p in (l.split() for l in out.splitlines()) if len(p) >= 8 and p[4] == "LOCAL"}


def section_symbols(obj):
    """Map (section name, offset) to the object's symbol name there (functions and objects)."""
    secs = elf_sections(obj)
    out = subprocess.run([str(READELF), "-sW", str(obj)], capture_output=True, text=True).stdout
    table = {}
    for line in out.splitlines():
        p = line.split()
        if len(p) >= 8 and p[3] in ("FUNC", "OBJECT") and p[6].isdigit() and int(p[6]) in secs:
            table.setdefault((secs[int(p[6])][0], int(p[1], 16)), p[7])
    return table


def data_objects(obj):
    """The object's data symbols: (name, section, offset, size)."""
    secs = elf_sections(obj)
    out = subprocess.run([str(READELF), "-sW", str(obj)], capture_output=True, text=True).stdout
    res = []
    for line in out.splitlines():
        p = line.split()
        if len(p) >= 8 and p[3] == "OBJECT" and p[6].isdigit() and int(p[6]) in secs:
            res.append((p[7], secs[int(p[6])][0], int(p[1], 16), int(p[2])))
    return res


def text_relocs(obj, section=".text"):
    out = subprocess.run([str(OBJDUMP), "-r", "-j", section, str(obj)], capture_output=True, text=True).stdout
    local = section_symbols(obj)
    rel = []
    for line in out.splitlines():
        p = line.split()
        if len(p) >= 3 and p[1].startswith("R_PPC"):
            try:
                off = int(p[0], 16)
            except ValueError:
                continue
            target = p[2]
            addend = 0
            m = re.match(r"(.+?)([+-]0x[0-9a-f]+)$", target)
            if m:
                target, addend = m.group(1), int(m.group(2), 16)
            anchor = re.match(r"^\.\.(\.\w+)\.\d+$", target)  # MWCC section anchors: ...data.0
            if anchor:
                target = anchor.group(1)
            if target.startswith(".") and (target, addend) in local:
                target, addend = local[target, addend], 0
            rel.append((off, p[1], target, addend))
    return rel


def masked(word, relocated):
    op = word >> 26
    if op == 18:
        return word & 0xFC000003
    if op == 16:
        return word & 0xFFFF0003
    if relocated:
        return word & 0xFFE00000
    return word


def same_code(ours, target, reloc_offsets):
    if len(ours) != len(target):
        return False
    for i in range(0, len(ours), 4):
        a = struct.unpack(">I", ours[i:i + 4])[0]
        b = struct.unpack(">I", target[i:i + 4])[0]
        r = i in reloc_offsets
        if masked(a, r) != masked(b, r):
            return False
    return True


def s16(v):
    return v - 0x10000 if v & 0x8000 else v


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("unit")
    ap.add_argument("--version", default="GUPE8P")
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()

    start, end = unit_text_range(args.version, args.unit)
    own_ranges = unit_ranges(args.version, args.unit)
    obj = object_path(args.version, args.unit)
    syms = load_symbols(args.version)
    by_addr = defaultdict(list)
    for name, sec, addr, attrs in syms:
        by_addr[addr].append((name, sec, attrs))
    existing_names = {s[0] for s in syms}

    funcs = elf_functions(obj)
    obj_locals = local_symbol_names(obj)
    relocs = text_relocs(obj)
    rel_by_func = defaultdict(list)
    for off, typ, tgt, add in relocs:
        for name, value, size, _ in funcs:
            if value <= off < value + size:
                rel_by_func[name].append((off - value, typ, tgt, add))
                break

    targets = []
    for name, sec, addr, attrs in syms:
        if sec == ".text" and start <= addr < end and "type:function" in attrs:
            m = re.search(r"size:0x([0-9A-Fa-f]+)", attrs)
            targets.append((addr, name, int(m.group(1), 16)))
    targets.sort()

    pairs = {}  # target addr -> object function name
    used = set()
    objs = {f[0]: f for f in funcs}
    for addr, name, size in targets:
        if name in objs:
            pairs[addr] = name
            used.add(name)
    for addr, name, size in targets:
        if addr in pairs:
            continue
        code = read_dol(addr, size)
        cands = [f for f in funcs if f[0] not in used and f[2] == size
                 and same_code(f[3], code, {o & ~3 for o, *_ in rel_by_func[f[0]]})]
        if len(cands) == 1:
            pairs[addr] = cands[0][0]
            used.add(cands[0][0])
    for addr, name, size in targets:
        if addr in pairs:
            continue
        cands = [f for f in funcs if f[0] not in used and f[2] == size]
        if len(cands) == 1:
            pairs[addr] = cands[0][0]
            used.add(cands[0][0])

    proposals = {}  # addr -> (new name, local)
    confirmed = {}  # addr -> name established by a code or data reference (outranks layout)

    def propose(addr, new, kind):
        if new.startswith("."):  # a section symbol, not a name
            return
        cur = by_addr.get(addr)
        if not cur:
            return
        if kind == "layout" and (addr in confirmed or new in confirmed.values()):
            return
        if kind != "layout":
            confirmed.setdefault(addr, new)
        names = [c[0] for c in cur]
        if new in names:
            return
        if not all(renameable(n, new) for n in names):
            return
        local = new.startswith("@") or "$" in new or "@unnamed@" in new or new in obj_locals
        if local and not any(lo <= addr < hi for lo, hi in own_ranges.values()):
            # A unit's local symbols live in its own sections; a local name for an address outside
            # them is a mis-pairing (e.g. a constant another unit owns).
            return
        if not local and new in existing_names:
            return
        prev = proposals.get(addr)
        if prev and prev[0] != new:
            print(f"# conflict at {addr:#x}: {prev[0]} vs {new}", file=sys.stderr)
            return
        proposals[addr] = (new, local, kind)

    for addr, oname in sorted(pairs.items()):
        tname = [n for n, s, a in by_addr[addr] if s == ".text"][0]
        if tname != oname:
            propose(addr, oname, "function")
        size = objs[oname][2]
        code = read_dol(addr, size)
        ours = objs[oname][3]
        obj_value = objs[oname][1]
        reloc_offsets = {o & ~3 for o, *_ in rel_by_func[oname]}
        # Calls to functions local to the object's .text have no relocation: decode both branches.
        for ins_off in range(0, min(len(ours), len(code)), 4):
            if ins_off in reloc_offsets:
                continue
            ow = struct.unpack(">I", ours[ins_off:ins_off + 4])[0]
            tw = struct.unpack(">I", code[ins_off:ins_off + 4])[0]
            if ow >> 26 == 18 and tw >> 26 == 18 and (ow & 1) and (tw & 1):
                def disp(w):
                    li = w & 0x03FFFFFC
                    return li - 0x04000000 if li & 0x02000000 else li
                callee = obj_value + ins_off + disp(ow)
                local = [f[0] for f in funcs if f[1] == callee]
                if local:
                    propose(addr + ins_off + disp(tw), local[0], "local call")
        ha = {}
        for off, typ, tgt, add in rel_by_func[oname]:
            ins_off = off & ~3
            if ins_off + 4 > len(code):
                continue
            word = struct.unpack(">I", code[ins_off:ins_off + 4])[0]
            if typ == "R_PPC_REL24":
                li = word & 0x03FFFFFC
                if li & 0x02000000:
                    li -= 0x04000000
                dest = addr + ins_off + li
                propose(dest - add, tgt, "call")
            elif typ == "R_PPC_ADDR16_HA":
                ha[tgt, add] = (word & 0xFFFF) << 16
            elif typ == "R_PPC_ADDR16_LO" and (tgt, add) in ha:
                dest = ha[tgt, add] + s16(word & 0xFFFF)
                propose(dest - add, tgt, "data")
            elif typ == "R_PPC_EMB_SDA21":
                ra = (word >> 16) & 0x1F
                base = SDA_BASE if ra == 13 else SDA2_BASE if ra == 2 else 0
                if base:
                    propose(base + s16(word & 0xFFFF) - add, tgt, "sda")

    # Data pass: follow the 32-bit pointers inside data objects whose retail address is known (named
    # the same in the unit's ranges, or just proposed), e.g. RTTI records to their name strings,
    # vtables to their functions, type records to their parameter tables.
    ranges = unit_ranges(args.version, args.unit)
    addr_of = {}
    for name, sec, addr, attrs in syms:
        if sec in ranges and ranges[sec][0] <= addr < ranges[sec][1]:
            addr_of.setdefault(name, addr)
    for addr, (new, _, _) in proposals.items():
        addr_of.setdefault(new, addr)
    dobjs = data_objects(obj)
    # Layout pass: when an object section has exactly the size of the unit's split range, its
    # objects sit at the same offsets as in retail, so every object pairs by offset (this reaches
    # data no relocation points at, such as tables addressed through a pooled base).
    sec_sizes = {name: size for name, _, size in elf_sections(obj).values()}
    for name, sec, value, size in dobjs:
        span = ranges[sec][1] - ranges[sec][0] if sec in ranges else -1
        if sec in ranges and sec in sec_sizes and 0 <= span - sec_sizes[sec] < 0x20:
            addr = ranges[sec][0] + value
            if not name.startswith("."):
                propose(addr, name, "layout")
                addr_of.setdefault(name, addr)
    drel = {}
    for sec in {d[1] for d in dobjs}:
        drel[sec] = [r for r in text_relocs(obj, sec) if r[1] == "R_PPC_ADDR32"]
    changed = True
    while changed:
        changed = False
        for name, sec, value, size in dobjs:
            base = addr_of.get(name)
            if base is None:
                continue
            for off, typ, tgt, add in drel.get(sec, []):
                if not value <= off < value + max(size, 1):
                    continue
                try:
                    word = read_dol(base + off - value, 4)
                except ValueError:  # .bss and other sections without file data
                    continue
                if len(word) < 4:
                    continue
                dest = struct.unpack(">I", word)[0] - add
                propose(dest, tgt, "data ref")
                if tgt not in addr_of:
                    addr_of[tgt] = dest
                    changed = True

    unpaired = [hex(a) for a, n, s in targets if a not in pairs]
    print(f"# {args.unit}: paired {len(pairs)}/{len(targets)} functions; unpaired: {unpaired}")
    for addr, (new, local, kind) in sorted(proposals.items()):
        print(f"0x{addr:08X}={new}{'  [local]' if local else ''}  # {kind}, was {by_addr[addr][0][0]}")

    if args.apply and proposals:
        path = ROOT / "config" / args.version / "symbols.txt"
        lines = path.read_text().splitlines()
        out = []
        for line in lines:
            m = SYMLINE.match(line)
            if m and int(m.group(3), 16) in proposals and renameable(m.group(1), proposals[int(m.group(3), 16)][0]):
                new, local, _ = proposals[int(m.group(3), 16)]
                attrs = m.group(4)
                if local and "scope:local" not in attrs:
                    attrs = re.sub(r"scope:\w+", "scope:local", attrs) if "scope:" in attrs else attrs + " scope:local"
                line = f"{new} = {m.group(2)}:0x{m.group(3)}; // {attrs}"
            out.append(line)
        path.write_text("\n".join(out) + "\n", newline="\n")
        print(f"# applied {len(proposals)} renames")


if __name__ == "__main__":
    main()
