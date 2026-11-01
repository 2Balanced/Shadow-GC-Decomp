#!/usr/bin/env python3
"""Audit the rationale notes in unit sources.

Every `NonMatching` unit should say, in the comment above its includes, why it does not
match yet and which source spellings were tried: that note is what stops the next session
from re-running a sweep that already failed. `--notes` lists the ones with no such note.

A note that cites a script under `build/` is a dead end for anyone else, because `build/`
is generated and gitignored. `--cites` lists the tracked sources that point into it, so the
sweep can be re-run with a tracked tool (`tools/uprobe.py`, `tools/flipsym.py`) or the path
dropped from the comment.

Usage:
    python tools/notesaudit.py --notes
    python tools/notesaudit.py --cites
"""

import argparse
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# How a head comment says "this does not match yet, and here is what was tried".
RATIONALE = re.compile(
    r"NonMatching|Not linking yet|does not link|not link in place|"
    r"not reproduced by|tried|shapes?", re.IGNORECASE)

BUILD_CITE = re.compile(r"build/[A-Za-z0-9_./-]+\.(?:py|sh|bat)")


def configured_units():
    """Source paths in configure.py, in the order the linker lays them out."""
    path = os.path.join(ROOT, "configure.py")
    with open(path, encoding="utf-8") as handle:
        return handle.read()


def source_of(unit):
    for candidate in ("src", ""):
        path = os.path.join(ROOT, candidate, unit.replace("/", os.sep))
        if os.path.exists(path):
            return path
    return None


def audit_notes(all_units):
    config = configured_units()
    missing, noted = [], []
    for line in config.splitlines():
        if "NonMatching" not in line:
            continue
        match = re.search(r"['\"]([A-Za-z0-9_./-]+\.(?:cpp|c|s))['\"]", line)
        if not match:
            continue
        path = source_of(match.group(1))
        if path is None:
            continue
        with open(path, encoding="utf-8", errors="replace") as handle:
            text = handle.read()
        head = text.split("\n#define")[0].split("\n#include")[0]
        (noted if RATIONALE.search(head) else missing).append(match.group(1))
    print("NonMatching sources: %d, with note: %d, missing note: %d"
          % (len(noted) + len(missing), len(noted), len(missing)))
    for unit in sorted(missing):
        print("MISSING  %s" % unit)
    if all_units:
        for unit in sorted(noted):
            print("noted    %s" % unit)


def audit_cites():
    listed = subprocess.run(["git", "grep", "-n", "--", "build/"],
                            cwd=ROOT, capture_output=True, text=True)
    rows = [line for line in listed.stdout.splitlines() if BUILD_CITE.search(line)]
    print("tracked sources citing gitignored build/ scripts: %d" % len(rows))
    for line in rows:
        print("  %s" % line[:160])
    return 1 if rows else 0


def main():
    parser = argparse.ArgumentParser(add_help=True)
    parser.add_argument("--notes", action="store_true",
                        help="list NonMatching units whose head comment has no rationale note")
    parser.add_argument("--all", action="store_true",
                        help="with --notes, also list the units that already carry one")
    parser.add_argument("--cites", action="store_true",
                        help="list tracked sources that cite a script under build/")
    args = parser.parse_args()
    failed = 0
    if args.notes:
        audit_notes(args.all)
    if args.cites:
        failed = audit_cites()
    if not (args.notes or args.cites):
        parser.print_help()
    return failed


if __name__ == "__main__":
    sys.exit(main())
