#!/usr/bin/env python3
"""Rename a class project-wide: src/, include/ and config/<VERSION>/symbols.txt.

Usage: python tools/rename_class.py OldName NewName [--dry-run] [--version GUPE8P]

- In src/ and include/ (.c/.cpp/.h/.hpp/.inc), every whole-word occurrence of OldName becomes
  NewName. Files are rewritten byte-for-byte otherwise, so CRLF and LF files keep their endings.
- In symbols.txt, every symbol name that embeds the class is rewritten as CodeWarrior mangles it:
  the class's own length prefix (<len(Old)>Old -> <len(New)>New) and the length prefix of every
  enclosing template-id (e.g. 39shared_ptr<25Old> -> 33shared_ptr<19New>) are recomputed. A
  plain-text OldName in the rest of a line (comments) is replaced too.

Old and New are identifiers, optionally namespace-qualified (Ns::Class). A qualified name mangles
as a Q-list (Q213GadgetUtility7Watcher); when the class sits inside an enclosing Q-list, the
list's count is recomputed. In sources a qualified New is written as is at every use; the class
declaration itself (`class Old`/`struct Old`) cannot be rewritten textually, so the tool leaves
it and lists it for a manual edit (wrap it in `namespace Ns { ... }`). After running it, reset and re-pair as
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


def mangle(name):
    """CodeWarrior's mangling of a class name: 7Watcher, or Q213GadgetUtility7Watcher."""
    parts = name.split("::")
    body = "".join(f"{len(p)}{p}" for p in parts)
    return body if len(parts) == 1 else f"Q{len(parts)}{body}"


def _components(name):
    """(count, component string without the Q prefix) of a mangled class name."""
    parts = name.split("::")
    return len(parts), "".join(f"{len(p)}{p}" for p in parts)


def _qlists(name):
    """Every Q-list in name: (start of 'Q', end of its count digit, count, [(tok_start, tok_end)])."""
    out = []
    for m in re.finditer(r"Q(\d)", name):
        n = int(m.group(1))
        pos = m.end()
        toks = []
        ok = True
        for _ in range(n):
            d = re.match(r"\d+", name[pos:])
            if not d:
                ok = False
                break
            ln = int(d.group(0))
            toks.append((pos, pos + len(d.group(0)) + ln))
            pos += len(d.group(0)) + ln
            if pos > len(name):
                ok = False
                break
        if ok and n >= 2:
            out.append((m.start(), m.end(), n, toks))
    return out


def rename_mangled(name, old, new):
    """Rewrite one mangled name. Returns the new name (unchanged when Old is absent)."""
    old_n, old_comp = _components(old)
    new_n, new_comp = _components(new)
    tok_old = mangle(old)
    tok_new = mangle(new)
    if old_comp not in name:
        return name
    # A class inside an enclosing Q-list (Q2 5Outer 3Old) is spelled by its components, and the
    # list's count changes with the number of components.
    if old_n == 1:
        for qs, qe, n, toks in _qlists(name):
            for ts, te in toks:
                if name[ts:te] == old_comp:
                    inner = name[:qs] + f"Q{n - 1 + new_n}" + name[qe:ts] + new_comp + name[te:]
                    # Re-run on the result for further hits and enclosing template lengths.
                    fixed = _fix_template_lengths(name, inner, ts, len(new_comp) - len(old_comp)
                                                  + len(str(n - 1 + new_n)) - len(str(n)))
                    return rename_mangled(fixed, old, new)
    if tok_old not in name:
        return name
    delta = len(tok_new) - len(tok_old)
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
        edits.append((h, h + len(tok_old), tok_new))
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


def _fix_template_lengths(orig, edited, at, delta):
    """Adjust the length prefix of every template-id in orig that encloses position at by delta."""
    if delta == 0:
        return edited
    edits = []
    for m in re.finditer(r"\d+", orig):
        run = m.group(0)
        for k in range(len(run)):
            n = int(run[k:])
            if n == 0 or run[k] == "0":
                continue
            if _template_span_ok(orig, m.end(), n) and m.end() <= at < m.end() + n:
                edits.append((m.start() + k, m.end(), str(n + delta)))
                break
    # Only prefixes before the edit point move nothing, so apply them to edited at the same offsets.
    out = edited
    for s0, e0, r in sorted(edits, reverse=True):
        out = out[:s0] + r + out[e0:]
    return out


def rewrite_symbols(path, old, new, dry):
    data = path.read_bytes()
    lines = data.split(b"\n")
    word = re.compile(rb"\b" + re.escape(old.encode()) + rb"\b")
    old_comp = _components(old)[1].encode()
    changed = 0
    for i, raw in enumerate(lines):
        if old.encode() not in raw and old_comp not in raw:
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
    decl = re.compile(rb"\b(?:class|struct)\s+" + re.escape(old.encode()) + rb"\b")
    qualified = "::" in new
    files = 0
    for top in ("src", "include"):
        for p in sorted((ROOT / top).rglob("*")):
            if not p.is_file() or p.suffix.lower() not in SRC_EXTS:
                continue
            data = p.read_bytes()
            if old.encode() not in data:
                continue
            if qualified:
                for m in decl.finditer(data):
                    line = data.count(b"\n", 0, m.start()) + 1
                    print(f"{p.relative_to(ROOT)}:{line}: declaration of {old} left for a manual edit "
                          f"(wrap it in its namespace)")
                keep = [(m.start(), m.end()) for m in decl.finditer(data)]
                pieces, last, n = [], 0, 0
                for m in word.finditer(data):
                    if any(a <= m.start() < b for a, b in keep):
                        continue
                    pieces.append(data[last:m.start()])
                    pieces.append(new.encode())
                    last = m.end()
                    n += 1
                pieces.append(data[last:])
                new_data = b"".join(pieces)
            else:
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
        if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*(::[A-Za-z_][A-Za-z0-9_]*)*", n):
            sys.exit(f"not an identifier: {n}")
    syms = ROOT / "config" / args.version / "symbols.txt"
    text = syms.read_text(encoding="latin-1")
    if re.search(r"\b" + re.escape(args.new) + r"\b", text) and args.old != args.new:
        print(f"note: {args.new} already appears in symbols.txt", file=sys.stderr)
    f = rewrite_sources(args.old, args.new, args.dry_run)
    s = rewrite_symbols(syms, args.old, args.new, args.dry_run)
    print(f"{f} source file(s), {s} symbols.txt line(s){' (dry run)' if args.dry_run else ''}")


if __name__ == "__main__":
    main()
