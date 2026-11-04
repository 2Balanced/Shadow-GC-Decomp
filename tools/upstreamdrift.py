#!/usr/bin/env python3
"""What has dtk-template changed since we vendored it?

    python tools/upstreamdrift.py                     # everything pending
    python tools/upstreamdrift.py --apply             # say what to check out
    python tools/upstreamdrift.py --vendor-ref vendor/template

The project is a squashed history with no common ancestor with the template, so a real merge is
impossible; keeping current is a vendor checkout, file by file. That only works if you know which
files are still the vendor's copy, which is exactly what this reports:

  clean      byte-identical to the version we vendored, and the template has since changed it
             -> take it:  git checkout <tip> -- <path>
  diverged   we edited it afterwards, so upstream changes have to be merged by hand
  ours       only we touch it (project files like config/ and src/)

After a sync, move the marker so the next run starts from the new state:

  git tag -f vendor/template <tip>          # pushed, so everyone shares the marker

The marker is a tag rather than a branch so it does not clutter the branch list.
"""

import argparse
import subprocess
import sys

# Project-owned trees: the template's copies of these are placeholders for a different game.
PROJECT_OWNED = ("config/", "src/", "docs/", "probes/", "include/")


def git(*args, check=True):
    r = subprocess.run(
        ["git", *args], capture_output=True, text=True, encoding="utf-8", errors="replace"
    )
    if check and r.returncode != 0:
        sys.exit(f"git {' '.join(args)}: {r.stderr.strip()}")
    return r.stdout.strip()


def resolve(ref):
    out = git("rev-parse", "--verify", ref, check=False)
    if not out:
        sys.exit(
            f"'{ref}' is not a commit. Fetch the template and its marker tag:\n"
            f"  git remote add template https://github.com/encounter/dtk-template\n"
            f"  git fetch template --tags"
        )
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--vendor-ref", default="vendor/template", help="where we are up to with the template")
    ap.add_argument("--upstream-ref", default="template/main", help="the template's current tip")
    ap.add_argument("--apply", action="store_true", help="print the checkout command for clean files")
    args = ap.parse_args()

    base = resolve(args.vendor_ref)
    tip = resolve(args.upstream_ref)
    if base == tip:
        print(f"Up to date with the template at {git('log', '--oneline', '-1', tip)}.")
        return 0

    commits = git("log", "--oneline", f"{base}..{tip}").splitlines()
    print(f"{len(commits)} template commits since {git('log', '--oneline', '-1', base)}\n")
    for c in commits:
        print(f"  {c}")

    # Every path the template has changed since our marker.
    changed = [
        p
        for p in git("diff", "--name-only", base, tip).splitlines()
        if not p.startswith(PROJECT_OWNED)
    ]
    print()
    clean, diverged = [], []
    for path in changed:
        ours_vs_base = git("diff", "--name-only", base, "HEAD", "--", path)
        in_tree = git("ls-files", "--", path)
        if in_tree and not ours_vs_base:
            clean.append(path)
        elif in_tree:
            diverged.append(path)
        else:
            print(f"  new from template: {path}")

    for path in clean:
        print(f"  clean    {path}")
    for path in diverged:
        print(f"  diverged {path}   (we edited it; merge by hand)")

    if args.apply and clean:
        print()
        print("git checkout " + tip + " -- " + " ".join(clean))
    elif clean:
        print("\nTake the clean ones with --apply, then: git tag -f vendor/template " + tip[:9])
    return 0 if not changed else 1


if __name__ == "__main__":
    sys.exit(main())
