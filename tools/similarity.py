#!/usr/bin/env python3
"""Measure how closely our sources follow a reference project, so code is never a copy of it.

  python tools/similarity.py --ref ../tp [--ref ../pikmin2 ...] [path ...]

For each of our .c/.cp/.cpp/.h files (default: src/ and include/, without include/boost), every
reference file with the same name is compared and the closest one reported:

- share: how much of our code appears in the reference, in order;
- run:   the longest stretch of consecutive identical lines.

Comments, blank lines, lone braces and all whitespace are ignored, so reformatting a file does not
lower its score. The bodies of `asm` functions are skipped: their instructions are fixed by the
binary, so every decompilation shares them.

A file is flagged when share >= --max-share (default 60%) or run >= --max-run (default 8 lines).
Rework a flagged file from the disassembly in your own words (structure, names, helpers) until it
passes. Exit status is 1 when anything is flagged. See CONTRIBUTING.md, "References".

Clone the references yourself (they are not part of this repository). Never point this at, or
otherwise open, another decompilation of this game.
"""

import argparse
import difflib
import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXTENSIONS = (".c", ".cp", ".cpp", ".h", ".hpp")
SKIP_PARTS = ("include/boost",)


def code_lines(path):
    """The lines that carry code, whitespace removed, without comments or asm function bodies."""
    text = path.read_text(encoding="utf-8", errors="replace")
    text = re.sub(r"//[^\n]*|/\*.*?\*/", "", text, flags=re.S)
    lines = []
    asm_depth = None  # brace depth inside an asm function body, None outside one
    pending_asm = False
    for raw in text.splitlines():
        line = re.sub(r"\s+", "", raw)
        if asm_depth is None and re.match(r"(static|inline|extern|\s)*asm\b", raw.strip()) \
                and "(" in raw:
            pending_asm = True
        if pending_asm or asm_depth is not None:
            if asm_depth is None and "{" in line:
                asm_depth = 0
                pending_asm = False
            if asm_depth is not None:
                asm_depth += line.count("{") - line.count("}")
                if asm_depth <= 0:
                    asm_depth = None
                continue
        if line in ("", "{", "}", "};"):
            continue
        lines.append(line)
    return lines


def compare(ours, theirs):
    matcher = difflib.SequenceMatcher(None, ours, theirs, autojunk=False)
    blocks = matcher.get_matching_blocks()
    shared = sum(block.size for block in blocks)
    return 100.0 * shared / max(len(ours), 1), max(block.size for block in blocks)


def our_files(paths):
    for start in paths:
        start = Path(start)
        found = [start] if start.is_file() else sorted(start.rglob("*"))
        for path in found:
            relative = path.resolve().relative_to(ROOT).as_posix()
            if path.suffix in EXTENSIONS and not any(part in relative for part in SKIP_PARTS):
                yield path


def reference_index(refs):
    index = {}
    for ref in refs:
        for folder, _, names in os.walk(ref):
            for name in names:
                if name.endswith(EXTENSIONS):
                    index.setdefault(name.lower(), []).append(Path(folder) / name)
    return index


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--ref", action="append", required=True,
                        help="a local checkout of a reference project (repeatable)")
    parser.add_argument("--max-share", type=float, default=60.0)
    parser.add_argument("--max-run", type=int, default=8)
    parser.add_argument("--all", action="store_true", help="list files that pass too")
    parser.add_argument("paths", nargs="*", default=[str(ROOT / "src"), str(ROOT / "include")])
    args = parser.parse_args()

    index = reference_index(args.ref)
    flagged = 0
    for path in our_files(args.paths):
        candidates = index.get(path.name.lower())
        if not candidates:
            continue
        ours = code_lines(path)
        if not ours:
            continue
        share, run, closest = max(compare(ours, code_lines(c)) + (c,) for c in candidates)
        too_close = share >= args.max_share or run >= args.max_run
        flagged += too_close
        if too_close or args.all:
            print(f"{'TOO CLOSE' if too_close else 'ok':9} {share:5.1f}%  run {run:3}  "
                  f"{len(ours):4} lines  {path.resolve().relative_to(ROOT).as_posix()}  ~ {closest}")
    print(f"{flagged} file(s) too close to a reference.")
    return 1 if flagged else 0


if __name__ == "__main__":
    sys.exit(main())
