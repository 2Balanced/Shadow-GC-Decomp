#!/usr/bin/env python3
"""Check that the committed progress tracker matches the sources it claims to describe.

  python tools/check_progress.py

Fails (exit 1) when:
- docs/progress/source.txt does not match the fingerprint of the current src/, include/, config/
  and configure.py: the progress page, card and badge were built from other sources (for example a
  merge brought in someone else's units) and must be regenerated with a build;
- docs/progress/history.jsonl has a line that is not a JSON object (a bad hand edit or merge).

It needs no game files, so GitHub Actions runs it on every push (.github/workflows/progress.yml).
To regenerate: build with `ninja` (it rewrites docs/progress* itself), then commit docs/progress*.
"""

import json
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from progress_page import ROOT, source_fingerprint  # noqa: E402


def main():
    problems = []
    recorded_path = ROOT / "docs" / "progress" / "source.txt"
    recorded = recorded_path.read_text(encoding="utf-8").strip() if recorded_path.exists() else ""
    current = source_fingerprint()
    if recorded != current:
        problems.append(
            "docs/progress/* were generated from different sources "
            f"(recorded {recorded or 'nothing'}, current {current}).\n"
            "  Rebuild with `ninja` so the tracker includes everyone's work, then commit docs/progress*.")
    history = ROOT / "docs" / "progress" / "history.jsonl"
    for number, line in enumerate(history.read_text(encoding="utf-8").splitlines(), 1):
        if not line.strip():
            continue
        try:
            if not isinstance(json.loads(line), dict):
                raise ValueError
        except ValueError:
            problems.append(f"docs/progress/history.jsonl line {number} is not a JSON object: {line[:60]}")
    if problems:
        print("Progress tracker check FAILED:")
        for problem in problems:
            print(" -", problem)
        return 1
    print("Progress tracker check passed: docs/progress/* match the current sources.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
