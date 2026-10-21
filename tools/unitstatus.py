#!/usr/bin/env python3
"""Print per-function and per-section match status for units in build/<VERSION>/report.json.

Usage: python tools/unitstatus.py Game/SetParticle [more units...] [--version GUPE8P]
Run `ninja build/GUPE8P/report.json` (or plain `ninja`) first so the report is current.
"""

import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("units", nargs="+")
    ap.add_argument("--version", default="GUPE8P")
    args = ap.parse_args()
    report = json.loads((ROOT / "build" / args.version / "report.json").read_text())
    by_name = {u["name"]: u for u in report["units"]}
    for name in args.units:
        unit = by_name.get(name) or by_name.get(f"main/{name}")
        if unit is None:
            matches = [n for n in by_name if name in n]
            print(f"{name}: not found" + (f" (did you mean {matches[:5]})" if matches else ""))
            continue
        m = unit.get("measures", {})
        print(f"== {unit['name']}: code {m.get('matched_code_percent', 0):.2f}% "
              f"data {m.get('matched_data_percent', 0):.2f}% "
              f"functions {m.get('matched_functions', 0)}/{m.get('total_functions', 0)}")
        for sec in unit.get("sections", []):
            print(f"   section {sec['name']:10} {sec.get('fuzzy_match_percent', 0):6.2f}%  size {sec.get('size')}")
        for fn in unit.get("functions", []):
            meta = fn.get("metadata", {})
            addr = meta.get("virtual_address")
            addr = f"{int(addr):08X}" if addr is not None else "????????"
            print(f"   {addr} {fn.get('fuzzy_match_percent', 0):6.2f}%  {fn['name']}")


if __name__ == "__main__":
    main()
