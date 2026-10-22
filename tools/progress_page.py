#!/usr/bin/env python3
"""Write docs/progress.html, a self-contained progress page, from objdiff's report.json.

Run automatically by `ninja` (the progress step in configure.py), or by hand:
  python tools/progress_page.py [--report build/GUPE8P/report.json]

Each run also appends a snapshot to docs/progress/history.jsonl when the numbers changed, so the
page can chart progress over time. Open docs/progress.html in any browser; it needs no server.

Merge safety (see CONTRIBUTING.md): history.jsonl holds one snapshot per line and is merged with
git's union driver, so snapshots pushed at nearly the same time are both kept. The other generated
files record a fingerprint of the sources they were built from (docs/progress/source.txt);
tools/check_progress.py fails when it no longer matches, which means they must be regenerated.
"""

import argparse
import hashlib
import datetime
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Approximate code regions of main.dol (see README "Approximate .text layout"). Addresses are the
# region starts; each region ends where the next begins.
REGIONS = [
    (0x80003100, "Startup & low-level init"),
    (0x80008D40, "Game code"),
    (0x8036FC94, "Dolphin SDK"),
    (0x8039A000, "Runtime, MSL & MetroTRK"),
    (0x803B6700, "CRI ADX/Sofdec"),
    (0x80408000, "Sonic Team engine & effects"),
    (0x80440000, "RenderWare"),
    (0x804A9C00, None),
]


def region_of(addr):
    for (start, name), (end, _) in zip(REGIONS, REGIONS[1:]):
        if start <= addr < end:
            return name
    return None


def git(*args):
    try:
        return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True).stdout.strip()
    except OSError:
        return ""


def summarize(report):
    m = report["measures"]
    units = report["units"]
    with_source = [u for u in units if not u.get("metadata", {}).get("auto_generated")]
    regions = {name: {"total": 0, "matched": 0, "linked": 0} for _, name in REGIONS if name}
    for u in units:
        complete = bool(u.get("metadata", {}).get("complete"))
        for f in u.get("functions", []):
            addr = int(f.get("metadata", {}).get("virtual_address", 0) or 0)
            name = region_of(addr)
            if not name:
                continue
            size = int(f.get("size", 0))
            r = regions[name]
            r["total"] += size
            if f.get("fuzzy_match_percent") == 100.0:
                r["matched"] += size
                if complete:
                    r["linked"] += size
    unit_rows = []
    for u in sorted(with_source, key=lambda u: u["name"]):
        um = u["measures"]
        unit_rows.append({
            "name": u["name"].split("/", 1)[-1],
            "linked": bool(u.get("metadata", {}).get("complete")),
            "functions": um.get("total_functions", 0),
            "matched_functions": um.get("matched_functions", 0),
            "code": round(um.get("matched_code_percent", 0.0), 2),
            "data": round(um.get("matched_data_percent", 0.0), 2),
            "code_bytes": int(um.get("total_code", 0)),
        })
    return {
        "total_code": int(m["total_code"]),
        "matched_code": int(m["matched_code"]),
        "linked_code": int(m["complete_code"]),
        "total_data": int(m["total_data"]),
        "matched_data": int(m["matched_data"]),
        "linked_data": int(m["complete_data"]),
        "total_functions": m["total_functions"],
        "matched_functions": m["matched_functions"],
        "units_with_source": len(with_source),
        "linked_units": sum(1 for r in unit_rows if r["linked"]),
        "regions": [{"name": k, **v} for k, v in regions.items()],
        "units": unit_rows,
    }


SNAPSHOT_KEYS = ["matched_code", "linked_code", "total_code", "matched_data", "linked_data",
                 "total_data", "matched_functions", "total_functions", "linked_units",
                 "units_with_source"]


# The inputs that decide the report: a change to any of them makes the generated files stale.
FINGERPRINT_PATHS = ["src", "include", "config", "configure.py"]


def source_fingerprint():
    """SHA-1 over the report's inputs (path and content, line endings normalised), so the same tree
    gives the same value on any checkout."""
    files = git("ls-files", "--cached", "--others", "--exclude-standard", "--", *FINGERPRINT_PATHS)
    digest = hashlib.sha1()
    for name in sorted(set(files.splitlines())):
        path = ROOT / name
        if not path.is_file():
            continue
        digest.update(name.encode("utf-8") + b"\0")
        digest.update(path.read_bytes().replace(b"\r\n", b"\n") + b"\0")
    return digest.hexdigest()


def load_history(path):
    """The snapshots in history.jsonl, oldest first. A union merge can leave the two sides' new
    lines in either order, or the same line twice; both are tidied here."""
    snapshots = {}
    if path.exists():
        for line in path.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if not line.startswith("{"):
                continue
            entry = json.loads(line)
            snapshots[json.dumps(entry, sort_keys=True)] = entry
    return sorted(snapshots.values(), key=lambda e: (e.get("date", ""), e.get("commit", "")))


def migrate_history(path):
    """Converts the old single-array history.json into history.jsonl."""
    old = path.with_suffix(".json")
    if old.exists() and not path.exists():
        entries = json.loads(old.read_text(encoding="utf-8"))
        path.write_text("".join(json.dumps(e) + "\n" for e in entries), encoding="utf-8",
                        newline="\n")
        old.unlink()


def update_history(path, summary, commit):
    migrate_history(path)
    history = load_history(path)
    snap = {k: summary[k] for k in SNAPSHOT_KEYS}
    if not history or {k: history[-1].get(k) for k in SNAPSHOT_KEYS} != snap:
        history.append({"date": datetime.datetime.now().isoformat(timespec="minutes"),
                        "commit": commit, **snap})
    # Always rewrite in order: it also drops the duplicate lines a union merge may leave.
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("".join(json.dumps(e) + "\n" for e in history), encoding="utf-8",
                    newline="\n")
    return history


def write_progress_page(report_path, out_path, history_path, verified=None):
    report = json.loads(Path(report_path).read_text(encoding="utf-8"))
    summary = summarize(report)
    commit = git("rev-parse", "--short", "HEAD") or "unknown"
    if git("status", "--porcelain", "--untracked-files=no"):
        commit += "+changes"
    history = update_history(Path(history_path), summary, commit)
    data = {
        **summary,
        "history": history,
        "commit": commit,
        "generated": datetime.datetime.now().strftime("%Y-%m-%d %H:%M"),
        "verified": verified,
    }
    payload = json.dumps(data).replace("</", "<\\/")
    page = TEMPLATE.replace("/*DATA*/", payload)
    Path(out_path).write_text(page, encoding="utf-8", newline="\n")
    write_badge(Path(history_path).parent / "badge.json", summary)
    write_card(Path(history_path).parent / "progress.svg", summary, verified)
    (Path(history_path).parent / "source.txt").write_text(source_fingerprint() + "\n",
                                                          encoding="utf-8", newline="\n")


CARD_STYLE = """
.bg { fill: #fcfcfb; stroke: #e1e0d9; }
.title { fill: #0b0b0b; font: 600 18px system-ui, -apple-system, "Segoe UI", sans-serif; }
.sub, .label, .nums, .key { font: 13px system-ui, -apple-system, "Segoe UI", sans-serif; }
.sub, .key { fill: #6f6d68; }
.label { fill: #52514e; font-weight: 600; }
.nums { fill: #52514e; font-variant-numeric: tabular-nums; }
.track { fill: #e1e0d9; }
.linked { fill: #2a78d6; }
.matched { fill: #74a9f0; }
@media (prefers-color-scheme: dark) {
  .bg { fill: #1a1a19; stroke: #2c2c2a; }
  .title { fill: #f0efec; }
  .sub, .key { fill: #9a988f; }
  .label, .nums { fill: #c3c2b7; }
  .track { fill: #2c2c2a; }
  .linked { fill: #4a94ec; }
  .matched { fill: #2260b0; }
}
"""


def write_card(path, summary, verified=None):
    """An SVG progress card for the README, which cannot embed the HTML page itself."""
    width, bar_x, bar_w = 760, 112, 360

    def pct(part, total):
        return 100.0 * part / max(1, total)

    def bar(y, linked, matched):
        # Linked is drawn from the left; matched-but-not-linked follows after a 2px gap.
        lw = bar_w * linked / 100.0
        mw = bar_w * max(0.0, matched - linked) / 100.0
        parts = [f'<rect class="track" x="{bar_x}" y="{y}" width="{bar_w}" height="12" rx="4"/>']
        if lw > 0:
            parts.append(f'<rect class="linked" x="{bar_x}" y="{y}" width="{lw:.1f}" height="12" rx="4"/>')
        if mw > 2:
            parts.append(f'<rect class="matched" x="{bar_x + lw + 2:.1f}" y="{y}" width="{mw - 2:.1f}" '
                         f'height="12" rx="4"/>')
        return "".join(parts)

    code = (pct(summary["linked_code"], summary["total_code"]),
            pct(summary["matched_code"], summary["total_code"]))
    data = (pct(summary["linked_data"], summary["total_data"]),
            pct(summary["matched_data"], summary["total_data"]))
    funcs = pct(summary["matched_functions"], summary["total_functions"])
    status = f"{summary['linked_units']} of {summary['units_with_source']} units linked"
    if verified:
        status += " · rebuilt main.dol matches retail"
    rows = [
        ("Code", code, f"{code[1]:.2f}% matched · {code[0]:.2f}% linked"),
        ("Data", data, f"{data[1]:.2f}% matched · {data[0]:.2f}% linked"),
        ("Functions", (funcs, funcs),
         f"{summary['matched_functions']:,} of {summary['total_functions']:,} matched"),
    ]
    body = []
    for i, (label, (linked, matched), nums) in enumerate(rows):
        y = 104 + 34 * i
        body.append(f'<text class="label" x="24" y="{y + 11}">{label}</text>')
        body.append(bar(y, linked, matched))
        body.append(f'<text class="nums" x="{bar_x + bar_w + 16}" y="{y + 11}">{nums}</text>')
    svg = f"""<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="216" viewBox="0 0 {width} 216" role="img" aria-label="Decompilation progress: {code[1]:.2f}% of code matched, {code[0]:.2f}% linked">
<style>{CARD_STYLE}</style>
<rect class="bg" x="0.5" y="0.5" width="{width - 1}" height="215" rx="8"/>
<text class="title" x="24" y="36">Shadow the Hedgehog (GUPE8P)</text>
<text class="sub" x="24" y="58">{status}</text>
<rect class="linked" x="24" y="72" width="10" height="10" rx="2"/><text class="key" x="40" y="81">Linked into the DOL</text>
<rect class="matched" x="184" y="72" width="10" height="10" rx="2"/><text class="key" x="200" y="81">Matched, not yet linked</text>
{''.join(body)}
</svg>
"""
    path.write_text(svg, encoding="utf-8", newline="\n")


def write_badge(path, summary):
    """A shields.io endpoint badge (https://shields.io/badges/endpoint-badge) for the README."""
    pct = 100.0 * summary["matched_code"] / max(1, summary["total_code"])
    badge = {"schemaVersion": 1, "label": "matched", "message": f"{pct:.2f}%", "color": "blue"}
    path.write_text(json.dumps(badge) + "\n", encoding="utf-8", newline="\n")


TEMPLATE = r"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Decomp Progress</title>
<style>
:root {
  color-scheme: light;
  --page: #f9f9f7; --surface: #fcfcfb; --ink: #0b0b0b; --ink-2: #52514e; --muted: #6f6d68;
  --grid: #e1e0d9; --axis: #c3c2b7; --ring: rgba(11,11,11,0.10);
  --linked: #2a78d6; --matched: #86b6ef; --series-1: #2a78d6; --series-2: #eb6834;
  --good: #006300;
}
@media (prefers-color-scheme: dark) {
  :root:where(:not([data-theme="light"])) {
    color-scheme: dark;
    --page: #0d0d0d; --surface: #1a1a19; --ink: #f0efec; --ink-2: #c3c2b7; --muted: #9a988f;
    --grid: #2c2c2a; --axis: #383835; --ring: rgba(255,255,255,0.10);
    --linked: #3987e5; --matched: #1c5cab; --series-1: #3987e5; --series-2: #d95926;
    --good: #0ca30c;
  }
}
:root[data-theme="dark"] {
  color-scheme: dark;
  --page: #0d0d0d; --surface: #1a1a19; --ink: #f0efec; --ink-2: #c3c2b7; --muted: #9a988f;
  --grid: #2c2c2a; --axis: #383835; --ring: rgba(255,255,255,0.10);
  --linked: #3987e5; --matched: #1c5cab; --series-1: #3987e5; --series-2: #d95926;
  --good: #0ca30c;
}
* { box-sizing: border-box; }
body { margin: 0; background: var(--page); color: var(--ink);
  font: 15px/1.5 system-ui, -apple-system, "Segoe UI", sans-serif; }
main { max-width: 1040px; margin: 0 auto; padding: 32px 16px 64px; }
h1 { font-size: 24px; margin: 0 0 4px; }
h2 { font-size: 17px; margin: 40px 0 12px; }
.sub { color: var(--ink-2); margin: 0; }
.sub code { font-size: 13px; }
.verified { color: var(--good); font-weight: 600; }
.card { background: var(--surface); border: 1px solid var(--ring); border-radius: 12px; padding: 20px; }
.tiles { display: grid; grid-template-columns: repeat(auto-fit, minmax(170px, 1fr)); gap: 12px; margin-top: 24px; }
.tile .label { color: var(--ink-2); font-size: 13px; }
.tile .value { font-size: 30px; font-weight: 650; line-height: 1.2; margin-top: 4px; }
.tile .detail { color: var(--muted); font-size: 13px; }
.legend { display: flex; flex-wrap: wrap; gap: 16px; color: var(--ink-2); font-size: 13px; margin: 0 0 12px; }
.key { display: inline-flex; align-items: center; gap: 6px; }
.sw { width: 12px; height: 12px; border-radius: 3px; display: inline-block; }
.sw.track { background: var(--grid); }
.bar-row { display: grid; grid-template-columns: minmax(150px, 220px) 1fr minmax(120px, auto); gap: 12px;
  align-items: center; padding: 8px 0; }
.bar-row + .bar-row { border-top: 1px solid var(--grid); }
.bar-name { font-size: 14px; }
.bar-nums { font-size: 13px; color: var(--ink-2); text-align: right; font-variant-numeric: tabular-nums; }
.bar { position: relative; height: 12px; border-radius: 4px; background: var(--grid); overflow: hidden; display: flex; gap: 2px; }
.bar .seg { height: 100%; border-radius: 4px; min-width: 0; }
.bar .seg.linked { background: var(--linked); }
.bar .seg.matched { background: var(--matched); }
.big .bar { height: 20px; }
.note { color: var(--muted); font-size: 13px; margin-top: 10px; }
.chart-wrap { position: relative; }
svg { display: block; width: 100%; height: auto; overflow: visible; }
.tip { position: absolute; pointer-events: none; background: var(--surface); border: 1px solid var(--ring);
  border-radius: 8px; padding: 8px 10px; font-size: 13px; box-shadow: 0 4px 16px rgba(0,0,0,0.12);
  white-space: nowrap; display: none; }
.tip .t { color: var(--ink-2); margin-bottom: 2px; }
table { width: 100%; border-collapse: collapse; font-size: 14px; }
th, td { text-align: left; padding: 7px 10px; border-bottom: 1px solid var(--grid); }
th { color: var(--ink-2); font-weight: 600; font-size: 13px; cursor: pointer; user-select: none; white-space: nowrap; }
th[aria-sort="ascending"]::after { content: " \2191"; }
th[aria-sort="descending"]::after { content: " \2193"; }
td.num, th.num { text-align: right; font-variant-numeric: tabular-nums; }
.status { display: inline-flex; align-items: center; gap: 6px; font-size: 13px; }
.dot { width: 8px; height: 8px; border-radius: 50%; }
.dot.linked { background: var(--linked); }
.dot.wip { background: var(--matched); }
.table-scroll { overflow-x: auto; }
details { margin-top: 12px; color: var(--ink-2); font-size: 13px; }
summary { cursor: pointer; }
.filters { display: flex; gap: 8px; margin-bottom: 10px; flex-wrap: wrap; }
.filters button { font: inherit; font-size: 13px; padding: 4px 12px; border-radius: 999px; border: 1px solid var(--ring);
  background: var(--surface); color: var(--ink); cursor: pointer; }
.filters button[aria-pressed="true"] { background: var(--ink); color: var(--surface); }
@media (max-width: 560px) {
  .bar-row { grid-template-columns: 1fr; gap: 4px; }
  .bar-nums { text-align: left; }
}
</style>
</head>
<body>
<main>
  <h1>Shadow the Hedgehog decomp: progress</h1>
  <p class="sub" id="meta"></p>
  <section class="tiles" id="tiles"></section>

  <h2>Code in main.dol</h2>
  <div class="card big">
    <div class="legend">
      <span class="key"><span class="sw" style="background:var(--linked)"></span>Linked (in the rebuilt DOL)</span>
      <span class="key"><span class="sw" style="background:var(--matched)"></span>Matched, not yet linked</span>
      <span class="key"><span class="sw track"></span>Not matched</span>
    </div>
    <div id="overall"></div>
    <p class="note">Matched: functions whose compiled code is byte-identical to retail (objdiff). Linked: units
      whose objects replace the original assembly in the build, which still reproduces the retail SHA-1.</p>
  </div>

  <h2>By region</h2>
  <div class="card" id="regions"></div>
  <p class="note">Region boundaries are approximate (see the README's .text layout).</p>

  <h2>Over time</h2>
  <div class="card">
    <div class="legend" id="hist-legend"></div>
    <div class="chart-wrap" id="hist"><div class="tip" id="tip"></div></div>
    <details><summary>Show as table</summary><div class="table-scroll"><table id="hist-table"></table></div></details>
  </div>

  <h2>Units with source</h2>
  <div class="card">
    <div class="filters" role="group" aria-label="Filter units">
      <button data-f="all" aria-pressed="true">All</button>
      <button data-f="linked" aria-pressed="false">Linked</button>
      <button data-f="wip" aria-pressed="false">In progress</button>
    </div>
    <div class="table-scroll"><table id="units"></table></div>
  </div>
</main>
<script type="application/json" id="data">/*DATA*/</script>
<script>
const D = JSON.parse(document.getElementById("data").textContent);
const pct = (a, b) => b ? (100 * a / b) : 0;
const f1 = v => v.toFixed(v >= 10 ? 1 : 2) + "%";
const n = v => v.toLocaleString("en-US");
const kb = v => (v / 1024).toLocaleString("en-US", {maximumFractionDigits: 0}) + " KB";
const el = (tag, attrs = {}, html = "") => { const e = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) e.setAttribute(k, v); e.innerHTML = html; return e; };

document.getElementById("meta").innerHTML =
  `Commit <code>${D.commit}</code> · generated ${D.generated}` +
  (D.verified ? ` · <span class="verified">&#10003; rebuilt DOL matches retail SHA-1</span>` : "");

const tiles = [
  ["Code matched", f1(pct(D.matched_code, D.total_code)), `${kb(D.matched_code)} of ${kb(D.total_code)}`],
  ["Code linked", f1(pct(D.linked_code, D.total_code)), `${kb(D.linked_code)} in the rebuilt DOL`],
  ["Functions matched", n(D.matched_functions), `of ${n(D.total_functions)} (${f1(pct(D.matched_functions, D.total_functions))})`],
  ["Units linked", n(D.linked_units), `of ${n(D.units_with_source)} with source`],
  ["Data matched", f1(pct(D.matched_data, D.total_data)), `${kb(D.matched_data)} of ${kb(D.total_data)}`],
];
for (const [l, v, d] of tiles)
  document.getElementById("tiles").append(el("div", {class: "card tile"},
    `<div class="label">${l}</div><div class="value">${v}</div><div class="detail">${d}</div>`));

function barRow(name, linked, matched, total) {
  const lp = pct(linked, total), mp = pct(matched - linked, total);
  const row = el("div", {class: "bar-row"});
  row.append(el("div", {class: "bar-name"}, name));
  const bar = el("div", {class: "bar", role: "img",
    "aria-label": `${name}: ${f1(lp)} linked, ${f1(pct(matched, total))} matched`});
  if (lp > 0) bar.append(el("div", {class: "seg linked", style: `width:${lp}%`, title: `Linked: ${f1(lp)}`}));
  if (mp > 0) bar.append(el("div", {class: "seg matched", style: `width:${mp}%`, title: `Matched, not linked: ${f1(mp)}`}));
  row.append(bar);
  row.append(el("div", {class: "bar-nums"}, `${f1(pct(matched, total))} matched · ${f1(lp)} linked`));
  return row;
}
document.getElementById("overall").append(barRow("All code", D.linked_code, D.matched_code, D.total_code));
for (const r of D.regions)
  document.getElementById("regions").append(barRow(`${r.name} <span style="color:var(--muted)">(${kb(r.total)})</span>`,
    r.linked, r.matched, r.total));

// History: matched and linked code share one axis (both are % of code).
(function history() {
  const H = D.history, host = document.getElementById("hist"), tip = document.getElementById("tip");
  const series = [
    {key: "matched", label: "Code matched", color: "var(--series-1)", v: h => pct(h.matched_code, h.total_code)},
    {key: "linked", label: "Code linked", color: "var(--series-2)", v: h => pct(h.linked_code, h.total_code)},
  ];
  document.getElementById("hist-legend").innerHTML = series.map(s =>
    `<span class="key"><span class="sw" style="background:${s.color}"></span>${s.label}</span>`).join("");
  const t = document.getElementById("hist-table");
  t.innerHTML = `<tr><th>Date</th><th>Commit</th><th class="num">Matched</th><th class="num">Linked</th><th class="num">Functions</th><th class="num">Units linked</th></tr>` +
    H.map(h => `<tr><td>${h.date.replace("T", " ")}</td><td><code>${h.commit}</code></td><td class="num">${f1(series[0].v(h))}</td><td class="num">${f1(series[1].v(h))}</td><td class="num">${n(h.matched_functions)}</td><td class="num">${n(h.linked_units)}</td></tr>`).join("");
  if (H.length < 2) {
    host.prepend(el("p", {class: "note"}, "The chart appears once there are two snapshots; one is added each time a build changes the numbers."));
    return;
  }
  const W = 960, Ht = 280, m = {l: 44, r: 96, t: 12, b: 28};
  const iw = W - m.l - m.r, ih = Ht - m.t - m.b;
  const maxV = Math.max(...H.flatMap(h => series.map(s => s.v(h))));
  const step = maxV > 20 ? 10 : maxV > 10 ? 5 : maxV > 4 ? 2 : 1;
  const top = Math.max(step, Math.ceil(maxV / step) * step);
  const x = i => m.l + (H.length === 1 ? iw / 2 : i * iw / (H.length - 1));
  const y = v => m.t + ih - v / top * ih;
  let s = `<svg viewBox="0 0 ${W} ${Ht}" role="img" aria-label="Code matched and linked over time">`;
  for (let v = 0; v <= top + 1e-9; v += step)
    s += `<line x1="${m.l}" x2="${W - m.r}" y1="${y(v)}" y2="${y(v)}" stroke="var(--grid)" stroke-width="1"/>` +
         `<text x="${m.l - 8}" y="${y(v) + 4}" text-anchor="end" font-size="12" fill="var(--muted)">${v}%</text>`;
  const sameDay = H[0].date.slice(0, 10) === H[H.length - 1].date.slice(0, 10);
  const lab = h => sameDay ? h.date.replace("T", " ") : h.date.slice(0, 10);
  const firstLabel = lab(H[0]), lastLabel = lab(H[H.length - 1]);
  s += `<text x="${m.l}" y="${Ht - 6}" font-size="12" fill="var(--muted)">${firstLabel}</text>`;
  s += `<text x="${W - m.r}" y="${Ht - 6}" font-size="12" fill="var(--muted)" text-anchor="end">${lastLabel}</text>`;
  const ends = [];
  for (const se of series) {
    const pts = H.map((h, i) => `${x(i).toFixed(1)},${y(se.v(h)).toFixed(1)}`).join(" ");
    s += `<polyline points="${pts}" fill="none" stroke="${se.color}" stroke-width="2" stroke-linejoin="round" stroke-linecap="round"/>`;
    ends.push({se, y: y(se.v(H[H.length - 1]))});
  }
  ends.sort((a, b) => a.y - b.y);
  for (let i = 1; i < ends.length; i++) if (ends[i].y - ends[i - 1].y < 16) ends[i].y = ends[i - 1].y + 16;
  for (const e of ends)
    s += `<text x="${W - m.r + 8}" y="${e.y + 4}" font-size="12" fill="var(--ink-2)">${e.se.label}</text>`;
  s += `<line id="cross" x1="0" x2="0" y1="${m.t}" y2="${m.t + ih}" stroke="var(--axis)" stroke-width="1" visibility="hidden"/>`;
  for (const se of series) s += `<circle class="hov" data-k="${se.key}" r="4" fill="${se.color}" stroke="var(--surface)" stroke-width="2" visibility="hidden"/>`;
  s += `<rect x="${m.l}" y="${m.t}" width="${iw}" height="${ih}" fill="transparent" id="hit"/></svg>`;
  host.insertAdjacentHTML("beforeend", s);
  const svg = host.querySelector("svg"), cross = svg.querySelector("#cross");
  svg.querySelector("#hit").addEventListener("pointermove", ev => {
    const pt = svg.createSVGPoint(); pt.x = ev.clientX; pt.y = ev.clientY;
    const p = pt.matrixTransform(svg.getScreenCTM().inverse());
    const i = Math.max(0, Math.min(H.length - 1, Math.round((p.x - m.l) / iw * (H.length - 1))));
    const h = H[i];
    cross.setAttribute("x1", x(i)); cross.setAttribute("x2", x(i)); cross.setAttribute("visibility", "visible");
    svg.querySelectorAll(".hov").forEach(c => { const se = series.find(s => s.key === c.dataset.k);
      c.setAttribute("cx", x(i)); c.setAttribute("cy", y(se.v(h))); c.setAttribute("visibility", "visible"); });
    tip.innerHTML = `<div class="t">${h.date.replace("T", " ")} · <code>${h.commit}</code></div>` +
      series.map(se => `<div><span class="sw" style="background:${se.color};width:8px;height:8px"></span> ${se.label}: <b>${f1(se.v(h))}</b></div>`).join("") +
      `<div>${n(h.matched_functions)} functions · ${n(h.linked_units)} units linked</div>`;
    tip.style.display = "block";
    const r = host.getBoundingClientRect(), sx = r.width / W;
    let left = x(i) * sx + 12; if (left + tip.offsetWidth > r.width) left = x(i) * sx - tip.offsetWidth - 12;
    tip.style.left = left + "px"; tip.style.top = "8px";
  });
  svg.querySelector("#hit").addEventListener("pointerleave", () => {
    tip.style.display = "none"; cross.setAttribute("visibility", "hidden");
    svg.querySelectorAll(".hov").forEach(c => c.setAttribute("visibility", "hidden"));
  });
})();

// Units table: sortable, filterable.
(function units() {
  const cols = [
    {k: "name", l: "Unit"}, {k: "linked", l: "Status"},
    {k: "matched_functions", l: "Functions", num: true}, {k: "code", l: "Code", num: true}, {k: "data", l: "Data", num: true},
  ];
  let sort = {k: "name", dir: 1}, filter = "all";
  const t = document.getElementById("units");
  function render() {
    const rows = D.units.filter(u => filter === "all" || (filter === "linked") === u.linked)
      .sort((a, b) => (a[sort.k] > b[sort.k] ? 1 : a[sort.k] < b[sort.k] ? -1 : 0) * sort.dir);
    t.innerHTML = "<tr>" + cols.map(c => `<th class="${c.num ? "num" : ""}" data-k="${c.k}" aria-sort="${sort.k === c.k ? (sort.dir > 0 ? "ascending" : "descending") : "none"}">${c.l}</th>`).join("") + "</tr>" +
      rows.map(u => `<tr><td>${u.name}</td><td><span class="status"><span class="dot ${u.linked ? "linked" : "wip"}"></span>${u.linked ? "Linked" : "In progress"}</span></td>` +
        `<td class="num">${u.matched_functions} / ${u.functions}</td><td class="num">${f1(u.code)}</td><td class="num">${f1(u.data)}</td></tr>`).join("");
    t.querySelectorAll("th").forEach(th => th.addEventListener("click", () => {
      sort = {k: th.dataset.k, dir: sort.k === th.dataset.k ? -sort.dir : 1}; render(); }));
  }
  document.querySelectorAll(".filters button").forEach(b => b.addEventListener("click", () => {
    filter = b.dataset.f;
    document.querySelectorAll(".filters button").forEach(x => x.setAttribute("aria-pressed", x === b ? "true" : "false"));
    render();
  }));
  render();
})();
</script>
</body>
</html>
"""


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--version", default="GUPE8P")
    ap.add_argument("--report")
    ap.add_argument("--out", default=str(ROOT / "docs" / "progress.html"))
    ap.add_argument("--history", default=str(ROOT / "docs" / "progress" / "history.jsonl"))
    args = ap.parse_args()
    report = args.report or str(ROOT / "build" / args.version / "report.json")
    write_page_for_build(args.version, report, args.out, args.history)
    print(f"wrote {args.out}")


def write_page_for_build(version, report=None, out=None, history=None):
    """Entry point for the build: the badge shows only when the hash check ran on this DOL."""
    build = ROOT / "build" / version
    report = report or str(build / "report.json")
    ok, dol = build / "ok", build / "main.dol"
    verified = ok.exists() and dol.exists() and ok.stat().st_mtime >= dol.stat().st_mtime
    write_progress_page(report, out or str(ROOT / "docs" / "progress.html"),
                        history or str(ROOT / "docs" / "progress" / "history.jsonl"), verified)


if __name__ == "__main__":
    main()
