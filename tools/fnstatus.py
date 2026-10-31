#!/usr/bin/env python3
"""Per-symbol status for every non-matching unit, via the objdiff CLI.

One row per symbol that is not 100%: unit, address, size, match percent, name.
Units are ranked by the cost of finishing them (sum of unmatched bytes), so the
top of the list is the cheapest work that flips a whole unit.

Usage: python tools/fnstatus.py [-u UNIT] [-n N] [--json OUT]
"""

import argparse
import json
import os
import subprocess
import sys

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OBJDIFF = os.path.join(root, "build", "tools", "objdiff-cli.exe")
sys.path.insert(0, os.path.join(root, "tools"))
import unitstatus  # noqa: E402  (reused unit registry parsing)


def symbols_of(tmp):
    with open(tmp, encoding="utf-8") as handle:
        data = json.load(handle)
    rows = []
    for sym in data.get("left", {}).get("symbols", []):
        try:
            size = int(sym.get("size") or 0)
        except (TypeError, ValueError):
            size = 0
        rows.append({
            "name": sym.get("name", ""),
            "addr": sym.get("address", 0),
            "segment": sym.get("segment", ""),
            "size": size,
            "match": float(sym.get("match_percent") or 0.0),
        })
    return rows


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-u", metavar="UNIT", help="only units containing this text")
    parser.add_argument("-n", type=int, default=40, help="show at most N units")
    parser.add_argument("--config", default="GUPE8P")
    parser.add_argument("--json", default="", help="dump every non-100%% symbol here")
    parser.add_argument("--all", action="store_true", help="include Matching units")
    args = parser.parse_args()

    units = unitstatus.configure_units(args.config, nonmatching_only=not args.all)
    if args.u:
        units = [u for u in units if args.u in u]
    tmp = os.path.join(root, "build", "fnstatus_tmp.json")

    per_unit = {}
    for i, unit in enumerate(units, 1):
        subprocess.run([OBJDIFF, "diff", "-p", root, "-u", unit, "--format", "json",
                        "-o", tmp], capture_output=True, text=True)
        if not os.path.exists(tmp):
            print(f"{unit}: no objdiff output", file=sys.stderr)
            continue
        rows = symbols_of(tmp)
        bad = [r for r in rows if r["match"] < 100.0]
        cost = sum(r["size"] * (100.0 - r["match"]) / 100.0 for r in bad)
        per_unit[unit] = {"rows": rows, "bad": bad, "cost": cost}
        if i % 50 == 0:
            print(f"... {i}/{len(units)}", file=sys.stderr)
    os.path.exists(tmp) and os.remove(tmp)

    ranked = sorted(per_unit.items(), key=lambda kv: (kv[1]["cost"], len(kv[1]["bad"])))
    print(f"{'unit':40} {'cost':>7} {'bad':>4} {'syms':>5}  worst symbols")
    print("-" * 110)
    for unit, d in ranked[: args.n or None]:
        worst = sorted(d["bad"], key=lambda r: r["match"])[:3]
        names = ", ".join(f"{r['name'][:26]} {r['match']:.0f}%/{r['size']}B"
                          for r in worst)
        print(f"{unit:40} {d['cost']:7.0f} {len(d['bad']):4} {len(d['rows']):5}  {names}")

    if args.json:
        with open(args.json, "w", encoding="utf-8") as handle:
            json.dump({u: d["bad"] for u, d in per_unit.items()}, handle, indent=1)
        print(f"\nraw rows written to {args.json}")


if __name__ == "__main__":
    main()
