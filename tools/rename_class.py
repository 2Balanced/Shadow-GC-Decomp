#!/usr/bin/env python3
"""Rename a class project-wide: src/, include/ and config/<VERSION>/symbols.txt.

Usage: python tools/rename_class.py OldName NewName [--dry-run] [--version GUPE8P]

- In src/ and include/ (.c/.cpp/.h/.hpp/.inc), every whole-word occurrence of OldName becomes
  NewName. Files are rewritten byte-for-byte otherwise, so CRLF and LF files keep their endings.
- In symbols.txt, every symbol name that embeds the class is rewritten as CodeWarrior mangles it:
  the class's own length prefix (<len(Old)>Old -> <len(New)>New) and the length prefix of every
  enclosing template-id (e.g. 39shared_ptr<25Old> -> 33shared_ptr<19New>) are recomputed. A
  plain-text OldName in the rest of a line (comments) is replaced too.

Old and New must be plain (unqualified) identifiers. After running it, reset and re-pair as
docs/matching-gadgets.md describes ("When a name has to change"); the symbols.txt rewrite here
already gives the shared instances their new mangled names, so usually only a rebuild is needed.
"""

import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC_EXTS = {".c", ".cpp", ".cp", ".h", ".hpp", ".inc"}
IDENT = re.compile(r"[A-Za-z0-9_]")


def _template_span_ok(s, start, n):
    """True when s[start:start+n] is `ident<...>` with the brackets closing exactly at its end."""
    end = start + n
    if end > len(s):
        return False
    i = start
    while i < end and IDENT.match(s[i]) and s[i] != "<":
        i += 1
    if i == start or i >= end or s[i] != "<" or s[end - 1] != ">":
        return False
    depth = 0
    for j in range(i, end):
        c = s[j]
        if c == "<":
            depth += 1
        elif c == ">":
            depth -= 1
            if depth == 0 and j != end - 1:
                return False
        if depth < 0:
            return False
    return depth == 0


def rename_mangled(name, old, new):
    """Rewrite one mangled name. Returns the new name (unchanged when Old is absent)."""
    tok_old = f"{len(old)}{old}"
    if tok_old not in name:
        return name
    delta = len(new) - len(old)
    # Positions of the class token itself (the digits plus the name).
    hits = []
    for m in re.finditer(re.escape(tok_old), name):
        # The length prefix fixes where the name ends, so what follows does not matter (it is
        # usually more of the signature). The prefix must start a token, though: not be the tail
        # of a longer number such as 13 in "113...", except after a Q<n> qualifier count.
        s = m.start()
        if s > 0 and name[s - 1].isdigit() and not (s > 1 and name[s - 2] == "Q"):
            continue
        hits.append(m.start())
    if not hits:
        return name
    edits = []  # (start, end, replacement) on the original string
    for h in hits:
        edits.append((h, h + len(tok_old), f"{len(new)}{new}"))
    # Enclosing template-id length prefixes.
    for m in re.finditer(r"\d+", name):
        run = m.group(0)
        best = None
        for k in range(len(run)):  # try every suffix: "Q25boost" -> 25 or 5
            n = int(run[k:])
            if n == 0 or run[k] == "0":
                continue
            body = m.end()
            if not _template_span_ok(name, body, n):
                continue
            inside = [h for h in hits if body <= h and h + len(tok_old) <= body + n]
            if inside:
                best = (m.start() + k, m.end(), n + delta * len(inside))
                break  # longest valid interpretation wins
        if best:
            edits.append((best[0], best[1], str(best[2])))
    out = name
    for s, e, r in sorted(edits, reverse=True):
        out = out[:s] + r + out[e:]
    return out


def rewrite_symbols(path, old, new, dry):
    data = path.read_bytes()
    lines = data.split(b"\n")
    word = re.compile(rb"\b" + re.escape(old.encode()) + rb"\b")
    changed = 0
    for i, raw in enumerate(lines):
        if old.encode() not in raw:
            continue
        text = raw.decode("latin-1")
        sym, sep, rest = text.partition(" = ")
        new_sym = rename_mangled(sym, old, new) if sep else sym
        new_rest = word.sub(new.encode(), rest.encode("latin-1")).decode("latin-1")
        if not sep:
            new_sym = word.sub(new.encode(), sym.encode("latin-1")).decode("latin-1")
        new_text = new_sym + sep + new_rest
        if new_text != text:
            print(f"{path.relative_to(ROOT)}: {sym} -> {new_sym}")
            lines[i] = new_text.encode("latin-1")
            changed += 1
    if changed and not dry:
        path.write_bytes(b"\n".join(lines))
    return changed


def rewrite_sources(old, new, dry):
    word = re.compile(rb"\b" + re.escape(old.encode()) + rb"\b")
    files = 0
    for top in ("src", "include"):
        for p in sorted((ROOT / top).rglob("*")):
            if not p.is_file() or p.suffix.lower() not in SRC_EXTS:
                continue
            data = p.read_bytes()
            if old.encode() not in data:
                continue
            new_data, n = word.subn(new.encode(), data)
            if n:
                files += 1
                print(f"{p.relative_to(ROOT)}: {n} occurrence(s)")
                if not dry:
                    p.write_bytes(new_data)
    return files


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("old")
    ap.add_argument("new")
    ap.add_argument("--version", default="GUPE8P")
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()
    for n in (args.old, args.new):
        if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", n):
            sys.exit(f"not a plain identifier: {n}")
    syms = ROOT / "config" / args.version / "symbols.txt"
    text = syms.read_text(encoding="latin-1")
    if re.search(r"\b" + re.escape(args.new) + r"\b", text) and args.old != args.new:
        print(f"note: {args.new} already appears in symbols.txt", file=sys.stderr)
    f = rewrite_sources(args.old, args.new, args.dry_run)
    s = rewrite_symbols(syms, args.old, args.new, args.dry_run)
    print(f"{f} source file(s), {s} symbols.txt line(s){' (dry run)' if args.dry_run else ''}")


if __name__ == "__main__":
    main()
