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

**Corrected in wave 41: the shared conversion slot below is a source shape, not a compiler
floor.** GC/2.7 shares the slot, and numbers the float registers as retail does, when the
interpolation keeps the difference to the next table entry in a variable of its own reduced with
`-=` (see "Source levers measured in wave 41"). Every `HitCharacter` named in this section now
matches under GC/2.7, and the units that were held back by it link. The text below is kept as the
record of what was tried; the other differences it lists (tail calls, the `.sdata2` order,
`checked_delete`, `ScatteringRing`) are not affected by the correction.

Functions that convert between float and int show a code-generation detail that no compiler in
the decomp.dev package reproduces together with the rest of the game. Examples are the
speed-table interpolation in `HitCharacter` of JumpBoard (0x8010F028) and of LongSpring
(0x80110320).

- **What the original does.** It reuses one 8-byte stack slot for every conversion temporary: the
  `fctiwz`/`stfd` slot also serves the `0x43300000` int→float sequence.
- **GC 1.3.2 through 2.7.** Every one of these compilers gives each conversion its own slot for the
  one-expression interpolation (`speed += (table[level + 1] - speed) * (x - level);`). The
  stack frame grows by 0x10 and the float register numbering changes. No `-opt` sub-option
  (`lifetimes`, `cse`, `prop`, `deadstore`, ...) changes this; the `-=` spelling of wave 41 does.
- **GC 3.0a3/3.0a5.** These reuse the slot like the original. However, they also tail-call
  (`b fn_8006B50C` where the original keeps a frame) and inline differently. Boost 1.32 also takes
  its newer `enable_if` path under them. Together that breaks 18 of 22 other JumpBoard functions.
- **What was ruled out.** Union-based conversion helpers compiled with 2.7 produce
  `fsub`/`frsp`, which the original does not have.

So the original was taken to be a CodeWarrior build between 2.7 (build 108, 2.4.7) and 3.0a3
(build 51213, Dec 2005). That fits the game's Oct 10 2005 build date, but no such build is
public. (Wave 41: the slot is a source shape, and these functions match under GC/2.7; what is left
of the argument rests on the other differences below.)

Further evidence:

- **3.0a3 always tail-calls.** A one-call hook compiles to `b` under every flag tried
  (`-O2` … `-O4,p`, `-opt noschedule/nopeephole`, `-ipa off/file`). The original hooks keep a frame,
  so those units were not built with 3.0a3. GC/2.7 rejects `-ipa`.
- **GC/2.7 shares the slot for some shapes only.** Every shape tried before wave 41 kept separate
  slots: a named int variable, an int temporary passed straight to a call (the shape of
  `fn_80036464`), an explicit cast, and brace-less `if` bodies. The difference reduced with `-=`
  shares it (wave 41).
- **The DOL mixes both behaviours.** A scan of the dtk assembly found 56 game functions that hold
  both conversion kinds. 23 of them share the slot (e.g. `fn_80036464`, `fn_800A4944`, JumpBoard,
  LongSpring and DashRing `HitCharacter`, Effect code at 0x804230xx). The other 33 keep them
  separate, as GC/2.7 does.
  - This was read as translation units built with at least two code generators, or with a
    2.7-like build whose allocator changed. Wave 41 shows one compiler doing both, depending on
    how the expression is written.
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
- `tools/pprobe.py`: `uprobe.py`'s tables compiled side by side from patched copies of the unit (the
  real source is untouched unless `--keep`); it adds whole-file replacements (`REPL`), header
  replacements (`HDR`) and renamed symbols (`SCORE` pairs) to the table format.
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
| `Game/PadButtons` | `fn_80356190` | 5 (33) | 16 | counter and pad parameter trade saved slots |
| `Game/PadButtons` | `fn_80356260` | 15 (88) | 28 | saved-register permutation of the loop locals |

`Game/TGiNumberDisplay` is the counter-example worth remembering: two spelling changes (the
bottom edge added at the store, and `-total / 2.0f` instead of `-total * 0.5f`) took `fn_800BC7BC`
from 17 mismatched rows to 0 of its 220 instructions, so always probe before writing a floor.

Two measured callback levers now account for several exact matches. If retail preserves an input
pointer in `r3`, a source declaration that returns that pointer can be required: the `void` form
lets GC/2.7 reuse `r3` for a temporary and changes the whole allocation. This matched helpers in
`Game/RwCpp07`, `RwCpp09`, `RwCpp17`, `RwCpp18`, `RwCpp19`, `RwCpp25` and `RwCpp46`. Likewise, a
wrapper that drops an existing argument or an indirect callee's result often shifts every temporary
one register down. Forward the argument and result explicitly, using cross-unit declarations where
available; `Game/TMessageDisp.cpp`'s declaration of `fn_80473CE0` identified its `path` argument and
`RtCharset *` result. Check callers after these corrections: the right signature should not add
instructions at the call site.

`#pragma scheduling off` is useful but not universal. It made `Game/RwCpp16`'s `fn_804533E0` and
`Game/EffectScreenBlurRW3`'s `GetParam` instruction-identical, while regressing the prologue and
call setup of `Game/RwCpp16`'s `fn_804535C8` and the axis rotation in
`Game/UnkAxisRotation_80049778`. Keep it scoped to one function and measure each use.

## Source levers measured in wave 39

Each of these took a function from a few rows to zero, and most of them finished a unit. The probe
tables in `probes/` keep the shapes that were tried; `python tools/pprobe.py -u Game/<Unit>` runs a
table's variants side by side (it also takes whole-file and header replacements, see its
docstring).

- **Saved registers go to the locals in declaration order, then to the parameters.** The first
  local declared gets the highest free register, the next one the next, and `this` and the other
  parameters come after them. `Game/PlayerPath.cpp` (`ChooseBranch`: the branch pointer declared
  before the result), `Game/TTextureChange.cpp` (`GetFrame`: the frame table declared after the
  counter; the same holds for the scratch registers r5 to r8) and `Game/CameraMotion.cpp` match
  through it. A parameter that retail keeps *between* two locals is a local too: a typed pointer
  made from a `void *` parameter is one (`fn_8000F9E8`), a copy of a parameter of the same type is
  merged back into the parameter and does not count.
- **`const` on the pointee decides whether loads may move above stores.** Through a pointer to
  const GC/2.7 moves a load above an unrelated store (the prologue's `stw r0`/`stmw`, a member
  store); through a pointer to non-const it leaves the load where it is written. The const-ness is
  read from the *parameter's declaration*: no cast or local in the body hides it. Retail has it both
  ways, so each case names the true signature: `const` added for `fn_800A9E1C`
  (`Game/PlayerMotion.cpp`), `fn_80173CD4` (`Game/PjsEventActionManager.cpp`) and `fn_8000F9E8`;
  `const` removed for `Chao::CSD::CProject::iterator::operator*` and for the description parameter
  of the `EffectGlobalParam`, `EffectLensFlare`, `EffectArranger` and `EffectShimmer` constructors
  (a constructor's `this` is never const, so the member stores and the description loads only stay
  in source order when the description is non-const too).
- **A value read through an inline function is a compiler temporary, and it is coloured apart from
  a named local.** `fn_8016F4E8` (`Game/SaveRecord.cpp`) needed the stage id from an inline getter
  to get r30, `GoalAction::SaveResult` the table entry from an inline lookup to get r5.
- **An inline call among a call's arguments is evaluated first.** `fn_80193790`
  (`Game/FlyerCommonBase.cpp`) forms its third argument before its second because the third is the
  inline `Enemy::GetHomeRotation()`. The same shows from the other side in
  `WeaponStickBase::fn_80067918`: the flags' address is formed after the bit is loaded because the
  test stands in an inline member that calls the out-of-line bit test directly.
- **A float constant in a product or sum is loaded first unless it has a name.**
  `float speed = -50.0f; z += dt * speed;` loads `dt` first, `z += dt * -50.0f` loads the constant
  first whatever the operand order (`MoveSlide` in `Game/VehicleGun.cpp`, `MakeSearch` in
  `Game/PlayerPath.cpp`).
- **A default assigned before a test, with the tested value in a local of its own**
  (`fn_80228348`, `Game/GrindGadget.cpp`) gives `li` default / load / compare / `li` override where a
  ternary or an if/else swaps the two registers.
- **An unused trailing parameter is visible at the call site.** `folderPause`'s constructor keeps
  the player in r5 across another load because it passes it to `InitScenes`, which never reads it.
- **Where a function is emitted tells what kind of function it is.** After each function GC/2.7
  emits the inline functions that function referenced first, last reference first; after the unit's
  last function come the out-of-line members of class template instances and the inline virtuals
  that a late vtable needs. `Game/PlayerBase.cpp` would not link until `CharInfo`'s slot 0x0C was an
  inline virtual taking its parameter and the player's state bits were an instance of a class
  template with a non-inline constructor (`Game/UnkWords_8007A4D4.h`, probably the library's bit
  set).
- **GC/2.7 calls the right operand of `|` first** (`PathImpl::Search`).
- **objdiff's 100% does not compare relocation targets or branch senses closely enough to trust.**
  `Game/EffectArranger.cpp` had a condition inverted (`beq` for `bne`), `Game/EffectShimmer.cpp` a
  shift of 10 for 22 and `Game/PlayerPath.cpp` two calls in the other order while their functions
  showed as matched. Flipping a unit whose functions all match and running `tools/doldiff.py` names
  the words that differ.

- **The stores of one statement run may be reordered, the registers are not.** GC/2.7 moved the
  store of the winner above the two counters' stores in `Result2PAction`'s constructor, but numbered
  the scratch registers in source order; only the source order `rounds++; wins[n]++; winner = n`
  gives retail's registers (24 orders probed, `probes/Game_Result2PAction.py`).
- **A member read through an inline function keeps the conversion of its return type.**
  `FindFreeChannel` (`Game/AudioManagerTask.cpp`) narrows the channel's priority again before the
  equality test (`clrlwi r0,r0,24`) and keeps the age in r0 only when both are read through inline
  getters; the back end's calls behind a null test load their arguments first only as inline
  functions. Its two loops also needed their locals declared in the order of retail's scratch
  registers with the assignments in another order.
- **An unset tail of a struct shows only in the frame size.** `Result2PAction`'s constructor had a
  frame 16 bytes short until its local load request got the sixth word the file system's request
  has (`Game/TBGRead.h`).
- **A copy-constructed local orders a call's arguments.** `Helicopter::CatchPlayer` sets the result
  pointer before the line's address only when the line is a local built with the struct's
  out-of-line copy constructor; a reference to the temporary or the nested call swaps them or moves
  the result's stack slot.
- **A `float` return type instead of `double` removes a rounding before the store.**
  `fn_8040F8FC` returns `float` for `Helicopter::UpdateTilt` (`stfs` before `frsp`).
- **A grouping decides the register of a sum's operand.** `next = motion + (base - 1); step++;`
  (`ShadowMotion::Step800B2EA4`) is the only one of some sixty forms that keeps the motion in r5
  and adds base to it in retail's operand order.
- **A class's out-of-line constructor and destructor are emitted where they are defined.**
  `Game/PlayerShadowMotion.cpp` has the IK slot's constructor and destructor, each followed by the
  inline functions it calls, between two ordinary functions; written in the class they come out
  behind the owner's constructor. An `inline` definition outside the class does not move them.
- **Function template instances at the end of a unit come out in the reverse of the order of the
  templates' first declarations**, behind the members of class template instances
  (`Game/EggPawn.cpp`: a forward declaration ahead of the header that declares the others).
- **A dead reference orders weak copies.** `if (false) { GetPosition(); }` ahead of the statement
  that calls `GetRotation()` first makes GC/2.7 emit the two weak functions in retail's order
  without code (`EggPawnBase::vfSet_0x1C`); `(void)&Enemy::GetPosition;` does the same but leaves
  a member pointer in `.data`.
- **Duplicate strings and 8-byte padding inside a unit's sections mean several translation
  units.** `Game/Mission.cpp` held two copies of `"Mission::Mission"` and of
  `"Memory allocation failure"`, a second RTTI record of `Mission` and a zero word that no source
  produced: retail has four units there (`Game/CountMission.cpp`, `Game/Mission.cpp`,
  `Game/MissionTable.cpp`, `Game/MissionManagerTask.cpp`), each section padded to 8 bytes. Split
  that way, three of them match without the `typeid` helper functions the combined unit needed.
- **An inherited inline virtual is emitted behind the class's own virtuals.** The address two
  vtables shared (0x8017B5A0) was `EnemyMoveFunc::vfMove_0x24`, emitted in `Game/EggPawn.cpp` for
  `EnemyMoveCommon`'s vtable, which does not override it; a label alias in `symbols.txt` had hidden
  that until the unit was linked from source.

Residuals that no shape changed, each seen in more than one unit (so probably the later compiler
build described above):

- a loop's strength-reduced offset initialised by copying the zeroed counter (`li r29,0` ...
  `mr r31,r29`), where GC/2.7 emits a second `li`: `Game/ClumpUpdater.cpp` (`fn_80015570`),
  `Game/folderTargetWindow.cpp` (constructor), `Game/TTextureChange.cpp` (`Setup`). This one is
  a source shape after all, see "Levers measured in wave 40": `Game/DiablonExecuter.cpp` and the
  two pool getters of `Game/UnkCollisionQuery_80020D40.cpp` match with the loop in an inline
  function, so the three units left here are worth the same try;
- `this` in r31 and a pooled `.rodata` base in r30 (`Game/GiGameOver.cpp`,
  `Game/TGiCommandWindow.cpp`): a string that goes through a value of its own flips the pair but
  then takes a scratch register from a constant;
- `this` or a float parameter in the first saved register with declared locals after it
  (`EggShadowAndroid::vfDisp_0x64`, `fn_8029EB5C`);
- a switch whose arms were still separate blocks when it was lowered and were joined afterwards
  (`MemoryCard::PollOpen`), and two constant returns kept as branches (`fn_801E7094` in
  `Game/Target.cpp`).

## Source levers measured in wave 40

- **Two "floors" were the const lever.** `Game/EnemyArea.cpp`'s `Contains` and `fn_801A69A0` (26
  shapes each in wave 39, once listed above as prologue scheduling) match with the point and the
  character declared as pointers to const: the loads then move above the prologue's stores. The
  other way round, `EffectParticleRW3`'s and `EffectLensFlareRW3`'s constructors need a description
  that is *not* const in the constructor's own parameter (the callers cast), whatever an inline
  reader further down takes. Try both const-nesses of every pointer parameter before writing a
  scheduling floor.
- **An inline function for a doubly indirect value orders two scratch registers.** The tree
  rebalancing `fn_8000CD64` (`Game/CameraFactory.cpp`) keeps the parent in r4 and the grandparent
  in r5 only when the grandparent comes from an inline function of its own (`Grand(x)`), not from
  `Parent(Parent(x))` or from named locals (31 earlier shapes).
- **A callee that rounds its own result returns float.** `fn_80016260` ends in `frsp`; declared
  `float`, `Event::Tick` (`Game/EggLastMechAIState.cpp`) stores the angle unrounded (`stfs f1`) and
  rounds a second copy for the subtraction, as retail does. `Game/ArkCannon.cpp`'s `Aim` gets the
  same store with `fn_8040F8FC` as float but then differs in more float registers.
- **`#pragma pool_data off` around one function** gives a `lis`/`addi` pair per constant where the
  rest of the unit reaches its constants through one pool base (`EffectParticle3DRW3`'s
  constructor).
- **A const local pointer is not merged back into what it copies.** `Impl *const self = this;`
  with `s32 *const value = data;` gives `ProcessEvents` (`Game/UnkMotion_8022F058.cpp`) retail's
  saved registers in declaration order (this above the value); the plain copies are merged away.
  A const copy of an ordinary local is still merged (`fn_8009339C`, `Game/PlayerUtil.cpp`).
- **Function-local statics show in `.sbss`.** A flip of `Game/EggLastMechAIState.cpp` failed on
  r13 displacements only: a guarded static of a class type is laid out guard first and numbered
  two apart (`s_state$417`, `init$419`), a guarded `float` value first and numbered one apart
  (`s_time$719`, `init$720`), and a 4-byte hole behind an instance means the class has a member
  nothing reads. `tools/dataorder.py --section .sbss` shows all three.
- **An exception record out of step with the code.** In `Game/EffectLensFlareRW3.cpp` and
  `Game/EffectManagerRW3.cpp` retail has one function's code last in the unit and its `extab`
  record right behind the record of the first function that calls it. GC/2.7 puts both where the
  function is defined (out of line) or both behind the caller (inline); no shape tried splits
  them, and it is all that keeps the two units from linking.

## Levers measured in wave 40

Several residuals that the sections above put down to the later compiler build were source shapes.
Try these before recording a floor:

- **Prologue and store scheduling is the `const` lever, in both directions.** A load that retail
  has *behind* the prologue's `stw r31` or behind member stores means the object is reached through
  a pointer or reference to non-const; a load retail has *above* them means const.
  `EnemyStatusCommon`'s slot 0x98 (attacker as `boost::shared_ptr<CharInfo> &`, which flipped the
  unit), `EffectObject`'s constructor (description as a pointer to non-const: the four colour
  loads stay behind the stores) and `fn_8003FAC4` of `Game/TPjsCsd.cpp` (`const TPjsCsd &`) were
  all recorded as compiler differences. `ScatteringRing::Instantiate` (the "fourth difference"
  above), `Game/EnemyArea.cpp` and `Game/EnemyParamCommon.cpp` describe the same symptom and have
  not been retried with it.
- **`mr` from the zeroed counter means the loop was in an inline function.** Written in place,
  GC/2.7 starts the loop's second induction variable with its own `li`; inlined from a function
  (`LinkPool` in `Game/UnkCollisionQuery_80020D40.cpp`, `SetUpBehaviors`/`TearDownBehaviors` in
  `Game/DiablonExecuter.cpp`) it copies the zero that is already in a register. That flipped
  `Game/DiablonExecuter.cpp`.
- **A member read into a local decides the operand registers of a comparison.**
  `int area = m_area; if (area != m_baseArea) { m_area = m_baseArea; ...` loads `m_area` into r3
  and `m_baseArea` into r0 (`UnkModel_801A3E9C::Part::SetAlpha`); the two members compared
  directly, or two locals, swap them.
- **`new T(...)` and operator new plus the constructor called by its symbol colour differently.**
  The compiler's temporary for a `new` expression took r29 in `StageManager::Start`; a local that
  receives `operator new` and then the constructor's result took retail's r28.
- **`x += a + b` and `x = x + a + b` add in different orders** (`EffectObject`'s constructor:
  the first adds the constant last, as retail does).
- **Members of class template instances are emitted template by template, in the order the class
  templates were first declared, and inside one template instance by instance.** With one class
  holding all of a vector's out-of-line members, a unit with two vector types gets all the members
  of one vector and then the other's. Retail's `Game/StageCameraPostProcess.cpp` has `erase`,
  `insert_n` and `reserve` of both vectors first and `destroy_all`, the destructor and `data` of
  both after them: two class templates, the growing members in the class between the vector and
  its storage. `MSL_VECTOR_MIDDLE_MEMBERS` in `<vector>` does that (the middle class is forward
  declared ahead of the storage class, which is what puts its members first) and gives that unit
  retail's function order. `Game/ItemIconManagerTask.cpp` records the same symptom.
- **Exception tables are written in code order, always.** `Game/EffectObject.cpp` has every
  function matching and retail's code order within reach (the two operator deletes defined out of
  line ahead of `~EffectCommandExec` under `#pragma auto_inline on`, so that the deleting
  destructors still inline them), but retail's tables are in another order than its code
  (`Update`, `~EffectCommandExec`, `ParseCommands`, ...), so the unit differs in `extab` and
  `extabindex` either way. `Game/EffectManager.cpp` shows the same.
- **`fnside.py` rows that differ only in a branch's printed target are not differences**: the
  target is printed as an offset in the object, and it moves when the function order differs.

## Source levers measured in wave 41

- **The shared float/int conversion slot is a source shape (A).** The speed-table interpolation
  written in one expression, `speed += (table[level + 1] - speed) * (x - level);`, gives the
  float-to-int conversion of the level and the int-to-float conversion back their own stack slots
  (the frame grows by 0x10 and the float registers renumber). Written
  `float delta = table[level + 1]; delta -= speed; speed += delta * (x - level);`, GC/2.7 shares
  one slot and numbers the float registers as retail does (the level's value f2, the difference
  f3). This was the "compiler version" blocker of the open question above. It flipped
  `Game/LongSpring`, `CircusBall`, `DashPanel` and `TGiHdGage`, and then `Game/DashRing`,
  `JumpBoard` (both of its interpolations), `Spring`, `SpringPanel`, `Catapult` and `StretchGrass`.
  Expect `@N` renumbering in the unit after the change (`tools/pair_symbols.py --apply`).
- **Conversion slots are handed out in argument-evaluation order (B).** Right to left, except that a
  first argument computed in place goes ahead of the second; masking that value into a named
  local first restores the strict order (`Game/EffectObjectUSA.cpp`: blue, green, red, then alpha
  in the highest slot). In `Game/EffectParticleUSA.cpp` the same colour block had retail's slots but
  two scratch registers swapped (the alpha byte and the clump's offset, r7/r8) with the alpha
  local as `u32`, `unsigned long`, `int` or `const`, declared anywhere, with copy propagation on or
  off; **the local's type decides it**: as `u8` (or `u16`) holding the masked byte it takes r7.
- **A struct returned by value and then copied into a local costs a copy and frame space.**
  `Mtx34f inverse = f(&matrix); g(&inverse, ...)` copies the call's return slot into the local
  (a 48-byte word loop); `g(&(const Mtx34f &)f(&matrix), ...)` passes the return slot on, as retail
  does. That copy was the 0x30-byte frame difference of `EffectLocusRW3::BuildVertices`, which its
  header had put down to the conversion slot; once the frame is right its twelve conversions take
  retail's slots.
- **Variables that the cases of a switch share are declared together at the top.**
  `Effect::System::EffectRay::Generate` and `EffectParticle::Generate` got retail's saved
  registers once the shape pointer (and EffectRay's description) were declared ahead of the count,
  and retail's float registers once the spread, angle and size floats were declared at the top of
  the function (ahead of EffectParticle's speed) and assigned in the cases; declared in each case,
  every case numbers them differently. Three smaller shapes in the same functions:
  `angle -= spread / 2.0f` (the `* 0.5f` form swaps the `fnmsubs` operands); the height's `0.0f`
  assigned between the sine and the cosine calls (it lands in the angle's register there); and the
  radius pick as a conditional expression (`float r = c ? a : a * Random();`), which loads the
  radius straight into the result's register where an if/else copies it with `fmr`. Their
  conversions share slots as retail's without lever A.
- **The constructor lever (wave 40) again.** `EffectRay`'s constructor keeps every description load
  behind the store before it only with the description parameter non-const (its caller casts; the
  mangled name in `symbols.txt` follows the declaration).
- **An inline destructor whose code sits in the middle of a unit was defined out of line.**
  `EffectParticleUSA`'s destructor is between its `Update` and its constructor in retail; written
  in the class, GC/2.7 emits it after the unit's last function (a late vtable's inline virtual);
  defined out of line between the two, it lands where retail has it and the unit links. The
  out-of-line declaration stands behind `EFFECT_PARTICLE_USA_IMPLEMENTATION` in
  `Game/EffectManagerPJS.h`: taking an inline function out of a header that many units include
  renumbers the compiler's `@N` locals of all of them (by two here).
- **An unused function that orders constants must not instantiate anything.** `Game/StretchGrass`'s
  `LaunchDirectionOutOfLine` (the unused-function trick for `.sdata2` order) built its vector as a
  temporary, `*d = Vec3f(0.0f, 0.0f, 1.0f)`; the temporary calls the weak `__ct__5Vec3fFfff`, which
  the linker then placed in this unit (first in link order) ahead of the copy
  `Game/ElecSecurityCode.cpp` places at 0x80157A5C, 0x10 bytes too many. A local
  (`Vec3f v(0.0f, 0.0f, 1.0f); *d = v;`) is built inline and pools the same constants.
- **Exception records out of code order, two more units.** `Game/EffectRay.cpp` and
  `Game/EffectParticle.cpp` now match every function but have retail's executor `operator delete`
  code last in the unit with its `extab` record right behind `Generate`'s, the shape of
  `Game/EffectObject.cpp`; they stay NonMatching.
- **`#pragma opt_propagation off` (the wave 41 lever) did not help here.** Around
  `EffectParticle3DUSA::Update` it took the function from 5 rows to 19 (the inline setter's
  parameter copies keep float registers of their own), with or without named copies of the alpha,
  the counter, the particle, the model or the scale; around `EffectLocusRW3::BuildVertices` it
  changed nothing.

## The C++ runtime library (Runtime.PPCEABI.H)

`NMWException.cp`, `MWRTTI.cp` and `Gecko_ExceptionPPC.cp` match under GC/2.7 with the runtime flags
plus `-Cpp_exceptions on -RTTI on -inline deferred -str nopool` (`cflags_runtime_cpp`): their
functions stand in reverse definition order, and every one with a frame has an `extab` entry.

- **Dead-stripped functions still shape the data.** `MWRTTI.cp` needs the unreferenced `__get_typeid`
  (and its `std::bad_typeid`) for `std::exception`'s type name to precede `std::bad_cast`'s in
  `.rodata`; the linker drops the function, its strings and its vtable.
- **A struct that stays in memory.** `ExPPC_FindExceptionRecord` reads its `FragmentInfo` local back
  from the stack after an inlined call filled it in. GC/2.7 keeps it there only when the caller
  reaches it through a pointer variable (`FragmentInfo *frag = &fragment;`); used by name, the
  fields are promoted to registers and the frame disappears.
- **Register order follows declaration order, loads follow assignment order.** The two array
  cases of `ExPPC_UnwindStack` match with `long size, n; char *ptr;` declared without initializers
  and assigned in the order retail loads them; initialized declarations in any order do not.
- **`#pragma exception_terminate`** in a function body gives the `0x8E` terminate action retail has
  in `ExPPC_UnwindStack` and in the inlined part of `ExPPC_ThrowHandler`; `#pragma exception_magic`
  gives `__unexpected` access to the current catch block (`__exception_magic`).
- **`extab_padding` and pooled data do not mix.** The catch-all action of `__unexpected` has two
  unused bytes that MWCC leaves uninitialized (they differ on every compile; retail has `0x0255`).
  `dtk extab clean` (the `extab_padding` option) fixes such bytes but rewrites the object, and the
  rewrite clears the flag byte (`0x10`) MWCC sets in `.comment` for a section's data-pool base
  symbol (`...rodata.0`). Without the flag mwldeppc no longer keeps that section whole, and strips
  the strings the code reaches through the base register (0x60 bytes of `.rodata` here).
  `tools/extabpad.py` sets the bytes in place instead, as a `post-compile` step in `configure.py`.
- **Assembly functions switch the peephole pass off for the rest of a file** (GC/1.2.5n): with
  `__init_hardware` and `__flush_cache` at the top of `dolphin/os/__ppc_eabi_init.cpp`, and
  `__start`/`__init_registers` ahead of `__init_data` in `dolphin/os/__start.c`, the C functions
  after them come out with retail's unscheduled prologues and redundant branches, with no pragma.
