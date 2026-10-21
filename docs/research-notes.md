# Research notes and local tooling

Findings gathered while setting up this checkout. They supplement the build flag
comments in `configure.py`.

## Retail DOL provenance

`orig/GUPE8P/sys/main.dol` (SHA-1 `118ef49ebd45b371b6f514e0e1b6d1ba7cf12904`) can also be
recovered from a Ghidra program imported with a GameCube loader extension (language
`PowerPC:BE:32:Gekko_Broadway`). The loader keeps no original file bytes, so the DOL is rebuilt
from the 2 text and 8 data blocks placed back to back after a 0x100 header, with the entry point
at 0x80003154 and BSS from 0x8056ED40 to 0x805FB564. Check the SHA-1 afterwards.

## Evidence behind the toolchain choices

- **CodeWarrior 3.0 is ruled out.** The game's build-date string is `Oct 10 2005`. The CodeWarrior
  3.0a3 compiler build number 51213 is a date, 2005‑12‑13, which postdates the game.
- **Game code uses `-O4,s -use_lmw_stmw on`.** `main` at 0x80050FD4, the RenderWare skeleton's
  `main`, matches byte for byte with these flags under GC 1.3.2, 2.0, 2.6 and 2.7. It does not
  match with `-O4,p` or with 3.0a3. The rect fields must be assigned one statement each
  (`r.x = 0; r.y = 0;`), and `r.w`/`r.h` are read back from `RsGlobal`.
- **RenderWare is 3.7.0.2.** `RwEngineGetVersion` at 0x80485D6C returns `0x37002`. RenderWare
  source that circulates online is leaked material; never use it as a reference.

## Open question: the game compiler is slightly newer than GC/2.7 build 108

Functions that convert between float and int show a code-generation detail that no compiler in
the decomp.dev package reproduces together with the rest of the game. Examples are the
speed-table interpolation in `HitCharacter` of JumpBoard (0x8010F028) and of LongSpring
(0x80110320).

- **What the original does.** It reuses one 8-byte stack slot for every conversion temporary: the
  `fctiwz`/`stfd` slot also serves the `0x43300000` int→float sequence.
- **GC 1.3.2 through 2.7.** Every one of these compilers gives each conversion its own slot. The
  stack frame grows by 0x10 and the float register numbering changes. No `-opt` sub-option
  (`lifetimes`, `cse`, `prop`, `deadstore`, ...) changes this.
- **GC 3.0a3/3.0a5.** These reuse the slot like the original. However, they also tail-call
  (`b fn_8006B50C` where the original keeps a frame) and inline differently. Boost 1.32 also takes
  its newer `enable_if` path under them. Together that breaks 18 of 22 other JumpBoard functions.
- **What was ruled out.** Union-based conversion helpers compiled with 2.7 produce
  `fsub`/`frsp`, which the original does not have.

So the original is probably a CodeWarrior build between 2.7 (build 108, 2.4.7) and 3.0a3
(build 51213, Dec 2005). That fits the game's Oct 10 2005 build date, but no such build is
public. Until one turns up, these functions stay at roughly 99.8% under GC/2.7. Everything else in
those units matches.

Further evidence:

- **3.0a3 always tail-calls.** A one-call hook compiles to `b` under every flag tried
  (`-O2` … `-O4,p`, `-opt noschedule/nopeephole`, `-ipa off/file`). The original hooks keep a frame,
  so those units were not built with 3.0a3. GC/2.7 rejects `-ipa`.
- **GC/2.7 never shares the slot.** Every shape tried keeps separate slots: a named int variable,
  an int temporary passed straight to a call (the shape of `fn_80036464`), an explicit cast, and
  brace-less `if` bodies.
- **The DOL mixes both behaviours.** A scan of the dtk assembly found 56 game functions that hold
  both conversion kinds. 23 of them share the slot (e.g. `fn_80036464`, `fn_800A4944`, JumpBoard,
  LongSpring and DashRing `HitCharacter`, Effect code at 0x804230xx). The other 33 keep them
  separate, as GC/2.7 does.
  - So translation units were built with at least two code generators, or with a 2.7-like build
    whose allocator changed.
  - Reimplementations of MWCC (rayanht/mwcc, zcanann/mwcc-rs) might eventually allow a patched
    build.

A second, independent difference points the same way: **the order of float constants in
`.sdata2` under deferred inlining.**

- Units whose constructor inlines small helpers can match every function under GC/2.7 and still
  fail to link as written, because their float constants come out in a different order.
- The original pools them in **source definition order**: the constants of the helpers the
  constructor inlines come first, followed by those of the functions defined after it. Functions
  are still emitted in reverse definition order.
- GC/2.7 build 108 pools them in reverse definition order, the same order it emits functions in.
- Dropping `deferred` and writing the source in address order does not help. Every function still
  matches, but the constant order stays wrong and the strings move behind the floats.
- Workaround: an unused out-of-line function that inlines the helpers in the needed order makes
  the compiler pool the constants in retail order; the linker drops it. `Game/ElecSmallCube.cpp`
  and `Game/ScatteringWeapon.cpp` use this.

A third difference shows up in `Game/EnergyCore.cpp`:

- **Out-of-line `boost::checked_delete<T>` with the destructor in its normal place.** Retail keeps
  `checked_delete<EnergyCore>` out of line (0x80101368) and emits `~EnergyCore` where its definition
  is (0x80101A10).
- GC/2.7 keeps `checked_delete` out of line only when `~EnergyCore` is inline, and then emits the
  destructor right after it, moving 0x6C bytes. A non-inline destructor, out-of-class `inline`,
  class order, a class `operator delete` and `#pragma dont_inline` don't change that.
- About 110 out-of-line `checked_delete` copies in the DOL suggest this is how the game's compiler
  behaves generally, not a quirk of one unit.
- Workaround (source level): keep the destructor non-inline, add an inline helper that does the
  `delete`, and give `boost::checked_delete<T>` an explicit inline specialization that calls the
  helper. GC/2.7 then keeps `checked_delete` out of line with retail's code and leaves the
  destructor at its definition position. `Game/Rocket.cpp`, `Game/EnergyCore.cpp` and
  `Game/SetWeapon.cpp` (`Arms`) link this way. It reproduces retail's output but says nothing
  about how the original source was written or how the game's compiler behaved.

A fourth difference shows up in `Game/ScatteringRing.cpp` (`ScatteringRing::Instantiate`):

- Retail loads `request.count` after storing the launch direction to the stack. GC/1.3.2 through
  2.7 schedule that load above the stores; 3.0a3 and 3.0a5 keep it after them, as retail does.
  About 25 source shapes were tried without changing it.

These effects fit one later 2.7-era compiler build. If someone in the decomp community has a
CodeWarrior for GameCube build between 2.4.7 build 108 and 3.0a3, test it here first. Look for a
"2.7 patch", or early 3.0 alphas (a1/a2), with JumpBoard `HitCharacter` and ElecSmallCube. It would
probably let many otherwise-finished units link.

## Symbol map caveat

The community `GUPE8P.map` and older Ghidra databases name about 13k functions by signature
matching against unrelated games. Examples are `cPlayer::PostPhysicsUpdate`,
`cFielder::UpdatePlay` and `FormationOffensive` (from *Mario Strikers*), `nw4hbm::*`,
`_J3DGraphBase::*`, `_TRK_Hollywood_Revolution::*` and `_os::OSGetStackPointer` on tiny getters.
These names are wrong for this game. Names backed by RTTI strings (`Player::Behavior::*`,
`Weapon::*`, `SonicteamUSA::*`) or by hand analysis are reliable.

## Local tools (`.tools/`, not tracked)

| Tool | Command | Notes |
| ---- | ------- | ----- |
| objdiff GUI | `.tools/objdiff-windows-x86_64.exe` | Set the project directory to the repo root |
| m2c | `.tools/venv/Scripts/m2c.exe build/GUPE8P/asm/<unit>.s -f fn_XXXXXXXX` | First-draft C |
| REA | `.tools/rea.cmd analyze <exe> --provider ghidra` | Ghidra-backed analysis of the Sonic Heroes PC exe (x86 PE) for cross-reference; REA cannot open DOLs |

REA needs Node 24 (it rejects Node 25) and a JDK 21 in `JAVA_HOME`, and it refuses Ghidra install
paths containing `&`; a wrapper script can pin those and pass the install's 8.3 short path.

## Repository tools added here

- `tools/cctest.py`: compiles one function under several compiler versions and flag sets, then
  compares it with the retail DOL with relocations masked.
  Example: `python tools/cctest.py f.c fn 0x80050FD4 --versions 2.6 2.7 "--flags=-O4,s|-O4,p" --show`.
- `tools/doldiff.py`: run after a SHA-1 failure; it lists the differing address ranges in the built
  DOL.
- `tools/pair_symbols.py`: pairs a unit's compiled functions with the retail ones and proposes
  names for the placeholders they reference (`--apply` writes them).
- `tools/reset_symbols.py`: turns names back into placeholders (`--match <substring>`, or
  `--stale Game/<Unit>`) so `pair_symbols.py` can pair them again after a type changes name.
- `tools/rename_symbols.py`: applies `0xADDR=Name` renames to `symbols.txt`.
- `tools/class_catalog.py`: lists the game's C++ classes from their vtables and RTTI records.
- `tools/unitstatus.py`, `tools/fndiff.py`: per-unit and per-function match status from objdiff.
- `tools/ghidra/ImportDtkSymbols.java`: applies the non-placeholder names in `symbols.txt` to a
  Ghidra program, in headless or GUI mode.
