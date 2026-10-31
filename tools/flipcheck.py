#!/usr/bin/env python3
"""Try flipping units to `Object(Matching, ...)` one at a time and keep only the ones that link.

A unit whose every symbol is matched still only becomes `Matching` when the relinked
`main.dol` is byte-identical, and that is decided by one relink, not by reading the
object. This tool does that relink for a list of units: for each one it edits
`configure.py`, reconfigures, relinks, and asks `tools/doldiff.py` whether the DOL is
still retail's. `configure.py` is restored between attempts, so the units are tested
independently, and it is restored and rebuilt at the end unless `--keep` names winners,
which are then left flipped and rebuilt once.

Usage:
    python tools/flipcheck.py Game/Foo.cpp Game/Bar.cpp
    python tools/flipcheck.py --keep Game/Foo.cpp Game/Baz.cpp
"""

import argparse
import subprocess
import sys

CONFIGURE = "configure.py"
DOL = "build/GUPE8P/main.dol"


def run(*cmd):
    return subprocess.run(cmd, capture_output=True, text=True, shell=False)


def set_flag(unit, flag):
    """Rewrite one `Object(...)` entry byte for byte, so the file's line endings stay put."""
    data = open(CONFIGURE, "rb").read()
    for current in (b"NonMatching", b"Matching"):
        pattern = b"Object(" + current + b', "' + unit.encode() + b'"'
        if data.count(pattern) == 1:
            if current == flag.encode():
                return True
            replacement = b"Object(" + flag.encode() + b', "' + unit.encode() + b'"'
            open(CONFIGURE, "wb").write(data.replace(pattern, replacement))
            return True
    return False


def links():
    if run("python", "configure.py").returncode != 0:
        return False, "configure failed"
    link = run("ninja", "-j", "8", DOL)
    diff = run("python", "tools/doldiff.py")
    if diff.returncode == 0:
        return True, ""
    first = [line for line in diff.stdout.splitlines() if ":" in line][:3]
    return False, "; ".join(first) or link.stdout.strip().splitlines()[-1:]


def main():
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument("units", nargs="+", help="source path as written in configure.py")
    parser.add_argument("--keep", action="store_true", help="leave the passing units flipped")
    args = parser.parse_args()

    winners = []
    for unit in args.units:
        run("git", "checkout", "--", CONFIGURE)
        if not set_flag(unit, "Matching"):
            print(f"{unit}: no unique Object() entry, skipped", flush=True)
            continue
        ok, why = links()
        print(f"{unit}: {'PASS' if ok else 'FAIL'} {why}", flush=True)
        if ok:
            winners.append(unit)

    run("git", "checkout", "--", CONFIGURE)
    if args.keep:
        for unit in winners:
            set_flag(unit, "Matching")
    run("python", "configure.py")
    run("ninja", "-j", "8", DOL)
    state = ", ".join(winners) if args.keep and winners else "none"
    print(f"configure.py left with {state} flipped; DOL: {run('python', 'tools/doldiff.py').stdout.strip()}")
    return 0 if winners else 1


if __name__ == "__main__":
    sys.exit(main())
