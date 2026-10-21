#!/usr/bin/env python3
"""Rename an identifier project-wide: src/, include/ (not include/boost) and symbols.txt.

Usage: python tools/rename_ident.py OldName NewName [--kind class|name] [--dry-run]

Source files are tokenised just enough to leave string and character literals alone (game strings
must not change), so only code and comments are rewritten. Bytes are otherwise kept, so CRLF and
LF files keep their endings.

symbols.txt:
- --kind class (pass it for classes, structs and typedefs): every mangled name embedding the type
  is rewritten with tools/rename_class.py's length-prefix logic.
- --kind name (the default): functions, methods and static data members. CodeWarrior puts the unqualified name
  first (Name__<scope><signature>), and thunks prefix it with @offset@; those leading names are
  rewritten, as are unmangled names (extern "C" functions, file-scope statics) equal to OldName.
Macros, locals and non-static members never reach symbols.txt; --kind name is harmless for them.
"""

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
from rename_class import rename_mangled  # noqa: E402

SRC_EXTS = {".c", ".cpp", ".cp", ".h", ".hpp", ".inc"}


def source_files():
    for top in ("src", "include"):
        for p in sorted((ROOT / top).rglob("*")):
            if not p.is_file():
                continue
            rel = p.relative_to(ROOT).as_posix()
            if rel.startswith("include/boost/"):
                continue
            if p.suffix.lower() in SRC_EXTS or (p.suffix == "" and "/Include/" in rel):
                yield p


def replace_outside_literals(text, word):
    """Replace word (a compiled regex) everywhere except inside "..." and '...' literals."""
    out = []
    i = 0
    n = len(text)
    count = 0
    seg = 0
    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j
            continue
        if text.startswith("/*", i):
            j = text.find("*/", i + 2)
            i = n if j < 0 else j + 2
            continue
        if c == '"' or c == "'":
            chunk, k = word.subn(NEW, text[seg:i])
            out.append(chunk)
            count += k
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1])
            i = j + 1
            seg = i
            continue
        i += 1
    chunk, k = word.subn(NEW, text[seg:])
    out.append(chunk)
    return "".join(out), count + k


def main():
    global NEW
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("old")
    ap.add_argument("new")
    ap.add_argument("--kind", choices=("class", "name"), default="name")
    ap.add_argument("--version", default="GUPE8P")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()
    for n in (args.old, args.new):
        if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", n):
            sys.exit(f"not a plain identifier: {n}")
    NEW = args.new
    word = re.compile(r"(?<![A-Za-z0-9_])" + re.escape(args.old) + r"(?![A-Za-z0-9_])")

    files = 0
    for p in source_files():
        raw = p.read_bytes()
        if args.old.encode() not in raw:
            continue
        text = raw.decode("latin-1")
        new_text, k = replace_outside_literals(text, word)
        if k:
            files += 1
            if not args.dry_run:
                p.write_bytes(new_text.encode("latin-1"))

    syms = ROOT / "config" / args.version / "symbols.txt"
    lines = syms.read_bytes().decode("latin-1").split("\n")
    lead = re.compile(r"(^|@)" + re.escape(args.old) + r"(?=__)")
    changed = 0
    for i, line in enumerate(lines):
        if args.old not in line:
            continue
        sym, sep, rest = line.partition(" = ")
        if not sep:
            continue
        if args.kind == "class":
            new_sym = rename_mangled(sym, args.old, args.new)
        else:
            new_sym = args.new if sym == args.old else lead.sub(lambda m: m.group(1) + args.new, sym)
        if new_sym != sym:
            lines[i] = new_sym + sep + rest
            changed += 1
    if changed and not args.dry_run:
        syms.write_bytes("\n".join(lines).encode("latin-1"))
    print(f"{args.old} -> {args.new}: {files} source file(s), {changed} symbols.txt line(s)"
          f"{' (dry run)' if args.dry_run else ''}")


NEW = ""

if __name__ == "__main__":
    main()
