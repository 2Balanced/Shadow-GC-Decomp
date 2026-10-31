#!/usr/bin/env python3
"""Compile-and-score source shapes for the functions of one unit.

A probe replaces one function's whole body in the source, rebuilds only that
unit's object (~4 s), and counts the instruction rows that still differ from the
retail split object (0 == matched).  The original source is always restored
unless --keep names the winning variant.

    python tools/uprobe.py -u Game/EnemyArea                # every probe
    python tools/uprobe.py -u Game/EnemyArea -n NAME [...]  # selected probes
    python tools/uprobe.py -u Game/EnemyArea -p ROUND2      # probes/Game_EnemyArea_ROUND2.py
    python tools/uprobe.py -u Game/EnemyArea --list         # probe names
    python tools/uprobe.py -u Game/EnemyArea --keep NAME    # leave the variant applied

Probe tables live in probes/<unit>.py ('/' -> '_'), or probes/<name>.py with -p for a later round
against the same unit.  Each defines:

    SYMBOLS = {symbol: anchor}        anchor = text that starts the function
    BODIES  = {symbol: {name: body}}  body  = text between the braces
    XTRA    = {symbol: prefix}        optional extra declaration before the anchor
"""
import argparse
import difflib
import os
import subprocess
import sys

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(root, "tools"))
import fnside  # noqa: E402

CFG = "GUPE8P"


def load_probes(unit, table=""):
    """Probe table probes/<unit>.py, or probes/<table>.py when -p names another round."""
    name = (table or unit).replace("/", "_")
    path = os.path.join(root, "probes", name + ".py")
    if not os.path.exists(path):
        sys.exit(f"no probe table: {path}")
    namespace = {"__file__": path}
    exec(compile(open(path, encoding="utf-8").read(), path, "exec"), namespace)
    return namespace


def obj_paths(unit):
    sub = unit.replace("/", os.sep)
    return (os.path.join(root, "build", CFG, "src", sub + ".o"),
            os.path.join(root, "build", CFG, "obj", sub + ".o"),
            f"build/{CFG}/src/{unit}.o")


def patch(source, symbol, body, xtra=""):
    """Swap the body of the function that starts at the symbol's anchor."""
    anchor = SYMBOLS[symbol]
    start = source.index(anchor)
    brace = source.index("{", start)
    end = close_brace(source, brace) + 1
    return source[:start] + xtra + body_function(anchor, body) + source[end:]


def close_brace(source, brace):
    """Index of the '}' that matches the '{' at brace (comments and strings skipped)."""
    depth = 0
    i = brace
    while i < len(source):
        c = source[i]
        if c == "/" and source[i + 1:i + 2] == "*":
            i = source.index("*/", i) + 2
            continue
        if c == "/" and source[i + 1:i + 2] == "/":
            i = source.index("\n", i)
            continue
        if c in '"\'':
            quote = c
            i += 1
            while i < len(source) and source[i] != quote:
                i += 2 if source[i] == "\\" else 1
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    raise ValueError(f"unbalanced braces from offset {brace}")


def body_function(anchor, body):
    return anchor + "\n" + body + "}\n"


def score(retail_obj, our_obj, symbol):
    a = fnside.instructions(retail_obj, symbol)
    b = fnside.instructions(our_obj, symbol)
    if not a or not b:
        return None, len(a), len(b)
    fa = [i[2] for i in a]
    fb = [i[2] for i in b]
    sm = difflib.SequenceMatcher(None, fa, fb, autojunk=False)
    rows = sum(max(j - i, k - l) for t, i, j, k, l in sm.get_opcodes() if t != "equal")
    return rows, len(a), len(b)


def rebuild(objrel):
    return subprocess.run(["ninja", "-j", "4", objrel], capture_output=True, text=True,
                          cwd=root)


def main():
    global SYMBOLS
    ap = argparse.ArgumentParser()
    ap.add_argument("-u", "--unit", default="Game/EnemyArea")
    ap.add_argument("-p", "--probes", default="",
                    help="probe table name when it is not probes/<unit>.py")
    ap.add_argument("-n", "--names", nargs="*", default=[])
    ap.add_argument("-s", "--symbols", nargs="*", default=[])
    ap.add_argument("--keep", default="")
    ap.add_argument("--list", action="store_true")
    args = ap.parse_args()

    table = load_probes(args.unit, args.probes)
    SYMBOLS = table["SYMBOLS"]
    bodies = table["BODIES"]
    xtra = table.get("XTRA", {})
    if args.list:
        for sym in bodies:
            print(f"{sym}: {', '.join(bodies[sym])}")
        return

    src = os.path.join(root, "src", args.unit + ".cpp")
    our_obj, retail_obj, objrel = obj_paths(args.unit)
    original = open(src, encoding="utf-8").read()
    for sym, anchor in SYMBOLS.items():
        assert anchor in original, f"anchor not in {src}: {anchor}"

    want = [s for s in SYMBOLS if not args.symbols or s in args.symbols]
    baseline = {}
    for sym in want:
        n, la, lb = score(retail_obj, our_obj, sym)
        baseline[sym] = n
        print(f"baseline {sym}: rows={n} (retail {la} insns, ours {lb})")

    results = []
    try:
        for sym in want:
            for i, (name, body) in enumerate(bodies[sym].items(), 1):
                if args.names and name not in args.names:
                    continue
                text = patch(original, sym, body, xtra.get(sym, ""))
                open(src, "w", encoding="utf-8").write(text)
                proc = rebuild(objrel)
                if proc.returncode != 0:
                    msg = [l for l in (proc.stderr or proc.stdout).splitlines() if l.strip()]
                    print(f"{sym[:28]:28} {name:24} BUILD FAILED: {(msg[-1] if msg else '')[:80]}")
                    results.append((9999, sym, name))
                    continue
                n, la, lb = score(retail_obj, our_obj, sym)
                if n is None:
                    print(f"{sym[:28]:28} {name:24} no disassembly")
                    results.append((9998, sym, name))
                    continue
                results.append((n, sym, name))
                flag = "" if n <= baseline[sym] else " (worse)"
                print(f"{sym[:28]:28} {name:24} rows={n:<4} retail {la} ours {lb}{flag}",
                      flush=True)
                if n == 0:
                    print("ZERO DIFF")
                    args.keep = args.keep or name
                    break
    finally:
        keep_src = original
        if args.keep:
            for sym in want:
                if args.keep in bodies[sym]:
                    keep_src = patch(keep_src, sym, bodies[sym][args.keep], xtra.get(sym, ""))
                    print(f"\nleft variant '{args.keep}' in {sym}")
        open(src, "w", encoding="utf-8").write(keep_src)
        rebuild(objrel)

    print()
    for n, sym, name in sorted(results)[:20]:
        print(f"  {n:>5}  {sym[:34]:34} {name}")


if __name__ == "__main__":
    main()
