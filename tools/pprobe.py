#!/usr/bin/env python3
"""Parallel probe runner: tools/uprobe.py's tables, compiled side by side.

    python tools/pprobe.py -u Game/X [-p TABLE] [-n NAME ...] [-d] [--keep NAME] [--noxtra] [-j 8]

Each variant is compiled from a patched copy of the unit (build/pprobe/<name>/<Unit>.cpp), so the
variants of a table build at the same time and the real source is only written with --keep.
Tables live in probes/<unit>.py ('/' -> '_') or probes/<TABLE>.py and may define:

    SYMBOLS = {symbol: anchor}         as tools/uprobe.py: anchor = text that starts the function
    BODIES  = {symbol: {name: body}}   body = text between the function's braces
    XTRA    = {symbol: prefix}         extra declarations put before the anchor
    REPL    = {name: [(old, new), ...]}   whole-file replacements in the unit's source
    HDR     = {name: {"Game/X.h": [(old, new), ...]}}   replacements in headers under include/
                                       (a patched copy is put first on the include path)
    SCORE   = [symbol, ...]            what REPL/HDR variants are scored on; an entry may be a
                                       (retail symbol, our symbol) pair when a variant renames it

The score is the number of instruction rows that differ from the retail split object (0 == the
instructions match).  Relocation targets are not compared, so confirm a zero with ninja and, for a
unit with every function matched, by flipping it and running tools/doldiff.py: two calls in the
other order or a `beq` for a `bne` only show there.  -d prints the differing rows of each variant.
"""
import argparse
import difflib
import os
import re
import shlex
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(root, "tools"))
import fnside  # noqa: E402
import uprobe  # noqa: E402

CFG = "GUPE8P"
BS = chr(92)


def unit_cmd(unit):
    """The compiler command of the unit's object, read from build.ninja (its rule and cflags)."""
    txt = open(os.path.join(root, "build.ninja"), encoding="utf-8").read().replace("$\n", "")
    key = "build build" + BS + CFG + BS + "src" + BS + unit.replace("/", BS) + ".o:"
    if key not in txt:
        key = "build build/" + CFG + "/src/" + unit + ".o:"
    i = txt.index(key)
    blk = txt[i:txt.index("\nbuild ", i + 10)]
    rule = blk.split(":", 1)[1].split()[0]
    mwv = re.search(r"mw_version = (\S+)", blk).group(1)
    cflags = re.sub(r"\s+", " ", re.search(r"cflags = (.*)", blk).group(1)).replace("$:", ":")
    r = txt.index("rule " + rule + "\n")
    command = re.search(r"command = (.*)", txt[r:]).group(1).replace("$:", ":")
    command = command.replace("$mw_version", mwv)
    # what stands before $cflags: [cmd /c] [sjiswrap] mwcceppc
    head = [h for h in command.split("$cflags")[0].split() if h.lower() not in ("cmd", "/c")]
    return head + shlex.split(cflags, posix=True)


def compile_variant(cmd, unit, name, text, hdrs=None):
    d = os.path.join(root, "build", "pprobe", re.sub(r"\W", "_", name))
    os.makedirs(d, exist_ok=True)
    pre = []
    if hdrs:
        inc = os.path.join(d, "inc")
        for h, pairs in hdrs.items():
            ht = open(os.path.join(root, "include", h), encoding="utf-8").read()
            for old, new in pairs:
                if old not in ht:
                    return None, f"header anchor missing {old[:40]!r}"
                ht = ht.replace(old, new, 1)
            os.makedirs(os.path.dirname(os.path.join(inc, h)), exist_ok=True)
            open(os.path.join(inc, h), "w", encoding="utf-8").write(ht)
        pre = ["-i", inc]
    base = os.path.basename(unit)
    src = os.path.join(d, base + ".cpp")
    open(src, "w", encoding="utf-8").write(text)
    obj = os.path.join(d, base + ".o")
    if os.path.exists(obj):
        os.remove(obj)
    k = 2 if "sjiswrap" in cmd[0].lower() else 1
    p = subprocess.run(cmd[:k] + pre + cmd[k:] + ["-i", "src/" + os.path.dirname(unit),
                                                   "-c", src, "-o", obj],
                       capture_output=True, text=True, cwd=root)
    if p.returncode != 0 or not os.path.exists(obj):
        msg = [l for l in (p.stdout + p.stderr).splitlines() if l.strip()]
        return None, " | ".join(msg[-6:])[:400]
    return obj, ""


def diffrows(a, b):
    fa = [i[2] for i in a]
    fb = [i[2] for i in b]
    sm = difflib.SequenceMatcher(None, fa, fb, autojunk=False)
    ops = sm.get_opcodes()
    rows = sum(max(j - i, l - k) for t, i, j, k, l in ops if t != "equal")
    return rows, ops, fa, fb


def show(a, b, ops, fa, fb, limit=40):
    n = 0
    for t, i, j, k, l in ops:
        if t == "equal":
            continue
        for x in range(max(j - i, l - k)):
            ra = f"+{a[i + x][0]:04x} {fa[i + x]}" if i + x < j else ""
            rb = fb[k + x] if k + x < l else ""
            print(f"      {ra:<44}| {rb}")
            n += 1
            if n >= limit:
                print("      ...")
                return
        print("      --")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-u", "--unit", required=True)
    ap.add_argument("-p", "--probes", default="",
                    help="probe table name when it is not probes/<unit>.py")
    ap.add_argument("-n", "--names", nargs="*", default=[])
    ap.add_argument("-d", "--diff", action="store_true", help="print each variant's differing rows")
    ap.add_argument("--keep", default="", help="write this variant into the real source")
    ap.add_argument("--noxtra", action="store_true", help="leave the table's XTRA out")
    ap.add_argument("-j", type=int, default=8)
    ap.add_argument("--limit", type=int, default=40)
    args = ap.parse_args()

    table = uprobe.load_probes(args.unit, args.probes)
    uprobe.SYMBOLS = table.get("SYMBOLS", {})
    bodies = table.get("BODIES", {})
    xtra = {} if args.noxtra else table.get("XTRA", {})
    repl = table.get("REPL", {})
    hdr = table.get("HDR", {})
    score_syms = table.get("SCORE", list(uprobe.SYMBOLS))
    src = os.path.join(root, "src", args.unit + ".cpp")
    if not os.path.exists(src):  # C units (src/RenderWare/*.c): their cflags carry -lang=c
        src = os.path.join(root, "src", args.unit + ".c")
    original = open(src, encoding="utf-8").read()
    retail = os.path.join(root, "build", CFG, "obj", args.unit + ".o")
    cmd = unit_cmd(args.unit)

    jobs = []  # (name, symbols, text)
    for sym, variants in bodies.items():
        for name, body in variants.items():
            if args.names and name not in args.names:
                continue
            jobs.append((name, [sym], uprobe.patch(original, sym, body, xtra.get(sym, ""))))
    for name, pairs in repl.items():
        if args.names and name not in args.names:
            continue
        text = original
        ok = True
        for old, new in pairs:
            if old not in text:
                print(f"{name}: anchor missing: {old[:50]!r}")
                ok = False
                break
            text = text.replace(old, new, 1)
        if ok:
            jobs.append((name, score_syms, text))
    for name in hdr:
        if name not in repl and not any(name in v for v in bodies.values()):
            if args.names and name not in args.names:
                continue
            jobs.append((name, score_syms, original))

    if args.keep:
        for name, syms, text in jobs:
            if name == args.keep:
                open(src, "w", encoding="utf-8").write(text)
                print("kept", name, "(header changes of the variant are not applied)")
                return
        sys.exit("no such variant")

    cache = {}

    def retail_ins(sym):
        if sym not in cache:
            cache[sym] = fnside.instructions(retail, sym)
        return cache[sym]

    def run(job):
        name, syms, text = job
        obj, err = compile_variant(cmd, args.unit, name, text, hdr.get(name))
        if obj is None:
            return name, None, err, []
        total = 0
        detail = []
        for s in syms:
            s, ours = (s if isinstance(s, tuple) else (s, s))
            a = retail_ins(s)
            b = fnside.instructions(obj, ours)
            if not b:
                detail.append((s, None))
                total += 999
                continue
            rows, ops, fa, fb = diffrows(a, b)
            total += rows
            detail.append((s, (rows, a, b, ops, fa, fb)))
        return name, total, "", detail

    with ThreadPoolExecutor(args.j) as ex:
        results = list(ex.map(run, jobs))
    for name, total, err, detail in results:
        if total is None:
            print(f"{name:28} BUILD FAILED: {err}")
            continue
        if len(detail) > 1:
            extra = " ".join(f"{s[:20]}={d[0] if d else 'NA'}" for s, d in detail)
        elif detail and detail[0][1]:
            extra = f"(retail {len(detail[0][1][1])} ours {len(detail[0][1][2])})"
        else:
            extra = ""
        print(f"{name:28} rows={total:<4} {extra}" + ("   <== ZERO" if total == 0 else ""))
        if args.diff and total:
            for s, d in detail:
                if d and d[0]:
                    show(d[1], d[2], d[3], d[4], d[5], args.limit)
    print("best:", sorted((t, n) for n, t, e, d in results if t is not None)[:5])


if __name__ == "__main__":
    main()
