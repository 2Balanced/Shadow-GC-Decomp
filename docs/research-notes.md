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
  `unitstatus.py --flip` lists the units whose every function and data symbol is matched, which are
  the only units that can become `Matching` (see "Which units can be flipped today"), and it prints
  each unit's signed section-size drift against its `splits.txt` ranges.
- `tools/flipcheck.py`: relinks once per candidate unit and keeps only the ones whose `main.dol`
  hashes to retail's, so a flip is decided by the byte check rather than by inference (it compares
  SHA-1 itself, because `tools/doldiff.py` only prints and always exits 0).
- `tools/flipsym.py`: our object versus the retail split object of a whole unit, symbol by symbol,
  for the units that fail that check.
- `tools/dolwhy.py`: flips one unit, relinks, and explains the difference in `main.dol` word by
  word - differing ranges first, then branch targets resolved through `symbols.txt`, which is how a
  moved function is told apart from shifted code.
- `tools/textorder.py`: walks a unit's own `.text` in MWCC's emission order against retail's
  addresses and prints the functions that would not land where retail had them.
- `tools/fnstatus.py`: one row per symbol below 100% (address, size, match %), units ranked by the
  bytes still unmatched, so the cheapest unit to finish is at the top.
- `tools/fnside.py`: our object versus the retail split object of one symbol, instruction by
  instruction (`-c N` context, `-a` whole function). Both objects are relocatable and use the same
  symbol names, so the diff is symbol-relative and needs no addresses.
- `tools/uprobe.py` + `probes/<unit>.py`: replaces one function body with a spelling from a probe
  table, rebuilds only that unit's object and scores the instruction rows left against retail. The
  source is restored unless `--keep NAME` names the winner; `-p TABLE` runs a later round against
  the same unit. Tables also serve as the record of what was tried.
- `tools/symfill.py`: annotates the data symbols our object defines inside a unit's split that
  retail left unnamed in `config/GUPE8P/symbols.txt` (small-data accesses carry no relocation, so
  `pair_symbols.py` cannot pair them).
- `tools/ghidra/ImportDtkSymbols.java`: applies the non-placeholder names in `symbols.txt` to a
  Ghidra program, in headless or GUI mode.

## Which units can be flipped today

A unit's `Object(...)` status follows its match percentages, and the link only has to keep
`main.dol` byte-identical: `build/GUPE8P/ldscript.lcf` has a single `MEMORY` origin and lays the
inputs out in `configure.py` order, section by section, with no per-unit addresses, so
`config/GUPE8P/splits.txt` ranges constrain the split objects and the report, not the link. Section
sizes therefore do not have to equal the retail split's - mwldeppc merges the same-named sections of
the objects, keeps one copy of duplicated template instantiations and drops code nothing references
(which is what the `FORCEACTIVE` list in the script exists to prevent). `Game/AdvSynopsis.cpp` is the
surprising case: its object has 3060 bytes of `.text` the retail split object does not have
(template instantiations of `Chao::CSD::RCPtr` and `boost` that retail's build put in neighbouring
units), and the relink is still byte-identical.

So the work list is not "units whose sizes already line up" but **units whose every function and
data symbol is matched**: `python tools/unitstatus.py --flip` prints them, ten of the 190
non-matching units when this was written, and `python tools/flipcheck.py <unit>.cpp ...` decides
each one by relinking. Only `Game/AdvSynopsis.cpp` passed, and it is now `Matching`.

For the failing units the difference is **placement inside the unit**, not how much code we have.
Each of our objects carries a single `.text` section (MWCC keeps the whole translation unit in one
section and puts a function's address in its symbol value; there are no per-function
`.text.<symbol>` sections), so MWLD can only lay a unit out where the compiler put things, and a
flip works when MWCC's emission order for the unit puts every function back at retail's address.
Two shapes show up once `tools/flipcheck.py` names the differing addresses:

- **The unit's own functions come out in another order.** Flipping `Game/EnemyMotion.cpp` changes 172
  words, all of them inside its own `.text` range `0x801A4430..0x801A54E8`, and each one is a branch
  whose target moved: retail keeps `__cl__Q25boost78function1<v,R19EnemyMotionEventArg,...>` at
  `0x801A5020`, between `UnkTree::find` and the tree's allocators, while GC/2.7 emits it after the
  tree's constructor instances. The instruction streams are identical, so objdiff never sees it, and
  `tools/textorder.py` is what shows it: it walks our object's `.text` in MWCC's emission order
  against retail's addresses and reports 31 of the unit's 70 functions in place.
  Much of what `tools/flipsym.py` calls "extra" belongs to this shape: the names exist in retail too,
  just in other units (`clear__Q25boost78function1<v,R19EnemyMotionEventArg,...>` is at
  `0x801A1A30`, `__ct__Q25boost17bad_function_callFv` at `0x80009FF4`), so our copies fold against
  them and their size costs nothing in itself - but MWCC emits them at the *end* of our unit, and
  those 0x22c bytes push the unit's own last functions (`insert_node`, `construct`, `allocate`,
  `size`) past retail's `0x801A54E8`, which is what actually breaks the link. The same holds in
  reverse for the `gap_*` records `flipsym.py` lists as missing: they are retail's unnamed records,
  not evidence of source we dropped.
- **The unit really does have code retail's unit did not have.** `Game/ResourceManagerTask.cpp` used to
  add the 8-byte `ResourceType__Fv` and `TaskType__Fv` `typeid` helpers (they existed only to create
  the RTTI records retail keeps in `.sdata`); nothing else in retail has those names, so they could
  not fold, and the 16 bytes shifted everything behind them. Including `<map_impl>` after `Tick`
  instead writes those records and the two `insert_unique` flags as data, which took the unit to
  retail's 64 functions, retail's size for every one of them and retail's 0x1720 for `.text`.
- **Two shared functions are generated at a different point of the template-instance run.** With
  `tools/fnorder.py` on that unit, 62 of the 64 functions stand in retail's order and only
  `fn_80012E90` (retail index 37, ours 32) and `fn_80013398` (retail 56, ours 33) are out of place,
  so all of `.text` is right except for those two addresses and the branches that reach them. Retail
  emits them inside the run of `__tree<std::pair<int, Resource*>, ...>` instances - after `comp__` and
  after `erase__` - as if they were members generated at their use point. GC/2.7 does not let the
  source move them there: written as ordinary `extern "C"` functions the two come out as one adjacent
  pair wherever their definitions stand (at the head of the unit they land at 32/33, after `Tick` at
  the end of the instance run, at the end of the file first of all); written `inline` they stay a
  pair; written as friends of a scratch class template with the untouched out-of-line member of
  `Game/CameraFactory.cpp` - the shape that does put that file's `fn_8000CF60`/`fn_8000CF6C` group
  where retail has it - they separate, but to 24 and 66 of 68, with four functions too many. Their
  place follows the compiler's generation order, not the source's.

Retail's split objects carry no `WEAK` symbols at all while ours mark their duplicate template
instantiations `WEAK` (23 in `Game/csdRW`, 8 in `Game/ResourceManagerTask`); mwldeppc folds those
against the unit that names them, which is why their bytes cost nothing while their place costs
everything.

So the remaining candidates are an emission-order puzzle plus, in most of them, retail's unnamed data
tails. Measuring that order needs `tools/fnorder.py`, which reads the function symbol values of our
single `.text` and lines them up against retail's sorted range (mwldeppc lays a unit out in that
order and folds duplicates, which is why `tools/textorder.py`'s size-only view is not enough);
`tools/dataorder.py` does the same for one data section of one unit. The drift table, re-measured
with those tools:

| Unit | `.text` drift | other section drift |
| ---- | ------------- | ------------------- |
| `Game/EnemyMotion` | +532 | `.data` +80, `.rodata` +106, `.sdata` +24, `.sbss` +9, `.sdata2` -4 |
| `Game/CameraManagerUnit` | +1780 | `.data` +68, `.rodata` +46, `.sdata` +16, `.sbss` -3, `.sdata2` -4 |
| `Game/ResourceManagerTask` | +0 | `.bss` -4, `.rodata` -2, `.sdata2` -3 |
| `Game/TextureUVAnimManagerTask` | +164 | `.data` -4, `.rodata` -5, `.sdata` -2, `.sdata2` -4 |
| `Game/CameraManager` | +944 | `.data` +36, `.rodata` +10, `.sbss` -3 |
| `Game/WeaponLockonBase` | +2668 | `.data` +128, `.rodata` +125, `.sdata` +32, `.sbss` +1 |
| `Game/ItemIconManagerTask` | +1164 | `.sbss` +25, `.data` +24, `.sdata` -4, `.rodata` -2 |
| `Game/AdvFolder` | +732 | `.data` +24, `.rodata` -10, `.sbss` -7, `.sdata2` -4 |
| `Game/csdRW` | +568 | `.data` +152, `.rodata` -6, `.bss` -7 |

`Game/EnemyMotion` and `Game/ResourceManagerTask` remain the two to attack. The first is pure
placement in a unit whose functions all match, so the question there is how to make GC/2.7 emit the
same set of inline definitions in that translation unit as retail's did (retail emits only
`__cl__Q25boost78function1...` inside the range and keeps `__dt__`, `clear__`, the `std::exception`
and `boost::bad_function_call` members in the units that name them at `0x801A19E8` and `0x80009FF4`).
The second is down to the two shared helpers above plus nine bytes of retail's data tails, measured by
`tools/dataorder.py`: 2 B after the last string pool entry of `.rodata`, 4 B after the manager object
in `.bss`, 3 B after the pool constant in `.sdata2`, and in `.sdata` the two `insert_unique` flags
where retail has them after the three RTTI records instead of before them. `Game/csdRW` is the third
candidate and a different shape: its `.data` is 152 bytes short of retail's because retail's copy
holds three Chao/CSD vtables (`__vt__CPlatformRW`, `__vt__CMemoryAllocRW`, `__vt__CTexListRW2`, with a
4-byte hole between the second and third) whose classes no header of the repo declares, and flipping
it shrinks the DOL's own header by 32 bytes - the first differing words are in `_rom_copy_info` and
`__init_registers`, so a flip there has to keep the section table exactly as it is.

## Code-generation floors found by probing

Spelled-out in `probes/`; each floor was confirmed by compiling many source spellings, not by
reading the disassembly once. In all of them our object has retail's instruction count, immediate
values and call targets, and only the scheduler order or the saved-register choice differs, which
GC/2.7 does not expose to the source.

| Unit | Symbol | Rows left (of) | Shapes tried | Residual |
| ---- | ------ | -------------- | ------------ | -------- |
| `Game/MemoryCardCrypt` | `Crypt__7CardDesFPCcPci` | 18 (218) | ~170 | MWCC list-scheduler order in the S-box gather |
| `Game/EnemyArea` | `Contains__21UnkEnemyArea_8018A374FPv` | 1 (33) | 26 | prologue scheduling |
| `Game/EnemyArea` | `fn_801A69A0` | 4 (23) | 26 | prologue scheduling |
| `Game/PadButtons` | `fn_80356190` | 5 (33) | 16 | counter and pad parameter trade saved slots |
| `Game/PadButtons` | `fn_80356260` | 15 (88) | 28 | saved-register permutation of the loop locals |

`Game/TGiNumberDisplay` is the counter-example worth remembering: two spelling changes (the
bottom edge added at the store, and `-total / 2.0f` instead of `-total * 0.5f`) took `fn_800BC7BC`
from 17 mismatched rows to 0 of its 220 instructions, so always probe before writing a floor.
