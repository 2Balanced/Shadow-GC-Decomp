# Matching a gadget unit

Gadgets are stage objects. Each type is one C++ unit with a `GadgetClassDesc` record in `.data`.
`Game/SetParticle.cpp` (small) and `Game/Elevator.cpp` (callbacks, motion, commands) are complete
examples. The steps below are the
recipe they followed.

## 1. Find the unit's extent

- **`.text` start: the `Instantiate` function.** Its address is the `spawn` field (+0x0C) of the type
  record. Deferred inlining emits functions in reverse definition order, so `Instantiate` (defined last)
  usually comes first. The order after it is: hook B, hook A, the helpers, `ReloadPlacement`,
  `Behave`, the destructor, then the constructor.
  - Some units start earlier. When the vtable's methods lie below `Instantiate`, extend the start down
    to cover them.
- **`.text` end: the next unit's first function.** Watch for a following unit whose label callback
  (`GadgetParamDesc::labelFn`) sits just before its own `Instantiate`. For example, SET_VEHICLE's
  `fn_8011E0A0` precedes its `Instantiate` at 0x8011E120.
- **`.data`:** the unit's parameter table(s), which come *before* the type record, then the type
  record, the RTTI base lists (0xC and 0x14 bytes), then the vtable (0x20 bytes for a plain
  `Gadget` subclass).
- **`.rodata`:** the class-name string (`s_taskName`), then the upper-case type name.
- **`.sdata`:** `s_taskName`, then the RTTI records (`__RTTI__4Task`, `__RTTI__6Gadget`,
  `__RTTI__<Class>`).
- **`.sdata2`:** strings of 8 bytes or fewer (parameter names, the RTTI names "Task"/"Gadget") and
  float constants.

Useful views:

- `build/GUPE8P/asm/*.s` holds the dtk disassembly.
- A small script over the DOL that prints each word with its `symbols.txt` name shows tables,
  vtables and RTTI clearly.
- The class name comes from the RTTI string that `s_taskName` points to.

## 2. Write the source

- Copy the shape of `src/Game/SetParticle.cpp`:
  - a class deriving from `Gadget`, with `SIZE_ASSERT` (`Instantiate` passes the size to `__nw__FUl`);
  - `s_taskName`;
  - definitions in reverse address order;
  - the hooks as `extern "C" void fn_XXXXXXXX(void)`;
  - the parameter table, then the `GadgetClassDesc` record (keep its `lbl_` name: a central table
    references it).
- Register the unit in `configure.py`: `Object(NonMatching, "Game/<Name>.cpp", cflags=cflags_game_cpp)`.
- Add the split to `config/GUPE8P/splits.txt` in address order.
- Declare unknown callees `extern "C"` with their `fn_` names in the unit's own `.cpp`. Prefer that
  over editing shared headers.

### Inlining rules (`-inline noauto,level=1,deferred`)

- Only functions marked inline (or defined in the class) are candidates.
- A helper whose body calls other inline functions is **not** inlined. It is emitted as a weak
  copy. Code that repeats in several places in the target was therefore written out in each place.
- A smart pointer's `operator->` is called out of line when its result feeds an inline call, for
  example `effect->Retire()` where `Retire` is inline. When it is used directly, it is inlined.
- boost smart-pointer members (`lock`, `expired`, `reset`, copy constructor and assignment,
  destructors) are calls to weak template instances. The target has them as `fn_`/`dtor_`
  symbols near their first user.

### boost::function and boost::bind callbacks

There are 329 bound callback types across 249 classes, and many gadgets register one.

The game used stock boost 1.32. The real headers are vendored in `include/boost/` under the Boost
Software License, alongside the reconstructed `shared_ptr.hpp`/`weak_ptr.hpp`. Write the callback
the way the original did:

```cpp
#include <boost/bind.hpp>
#include <boost/function.hpp>
fn_800D0B64(m_model->field_0x4, boost::bind(&JumpBoard::OnColliDetect, this, _1));
```

`Game/JumpBoard.cpp` shows the full pattern. The bound type's RTTI name in `.rodata` names the
template arguments: `boost::_bi::bind_t<void, boost::_mfi::mf1<void, Class, Args &>, ...>`.

Each unit carries its own instances, and all of them match as compiled:
- functor manager `manage`/`manager`, `invoke`
- `bind_t`/`list2` constructors and `operator()`, `unwrap`, `list1::operator[]`
- the `function1` converting constructor and both `assign_to` overloads
- `allocator` `allocate`/`construct`, `addressof`, `boost::bind`

Shared instances live in the unit that used them first. Rename them at their retail addresses:
- `make_any_pointer` at 0x8001C564
- `has_empty_target(...)` at 0x8001C574
- `list1<Args&>::operator[](arg<1>)`
- the `function1<...>` destructor

### Linking rule for weak instances

The linker keeps a weak copy from your object unless the target has a global symbol with the
same mangled name.
- **Rename every template instance your code calls directly** (smart-pointer constructors,
  destructors and `operator=`, `lock`, the boost instances above) at its retail address.
- Helpers those call (`shared_count`, `weak_count`, `sp_counted_base`) are dead-stripped
  automatically.
- There is no code merging: each `shared_ptr<T>` has its own copy of identical code.
- Member templates such as `weak_ptr::operator=<Y>(shared_ptr<Y>)` and
  `weak_ptr<T>::weak_ptr<Y>(shared_ptr<Y>)` are emitted as local copies inside each unit.

### Shared types: one name per class

A weak template instance has exactly one retail address, so every unit must spell the class
inside it the same way. Two units that each invent a name for the same class
(`shared_ptr<UnkCollider_...>` in one, `shared_ptr<CharaColliBase>` in the other) can't both link.

- **Reuse the shared headers before declaring a class locally:**
  - `Game/CharaColli.h`: `SonicteamUSA::System::CharaColli::CharaColliBase`, the collider behind
    `fn_800C7A84`, `fn_800C7D04` and `fn_80161614` (GadgetModelInfo's part collider).
  - `Game/Vibration.h`: `Vibration::RequestHandle`, the handle the rumble functions return
    (`fn_8034DD24`, `fn_8034DE74`, `fn_80169BB0`).
  - `Game/GadgetParticle.h`: `GadgetParticle::GadgetParticleCtrl` and `UnkParticleAttach_80169D00`,
    the parameter passed with `fn_80169D00` to `fn_8015FA78`.
  - `Game/CharaColliAttack.h`: `CharaColliAttack`, the attack collider `fn_80161428` returns,
    `AttackCallbackParam`, and the callback member type `UnkAttackCallback_80054E28` (assign it
    with `collider->m_attackCallback = AttackCallback(boost::bind(...))`).
  - `Game/GadgetSound.h`: `UnkSoundHandle_801D3DC8`, the sound `fn_801D3DC8` starts.
  - `Game/CharCommandUnit.h`: `CharCommandUnit`, the command receiver at
    `GadgetCharInfo::m_commandUnit`, and `fn_800729B0`, which assigns its callback.
  - `Game/TNumEdit.h` and `Game/CharaColliBaseCharInfo.h`: never-called `dynamic_cast`s to
    `TNumEditHex/Int/Float` and `CharaColliBaseCharInfo` (real RTTI names). They reproduce the
    orphan RTTI base lists that start the `.data` of about 70 units. Include order sets the order
    of the lists; see "Units whose `.data` is kept whole" below.
  - `Game/GadgetDebris.h`: `UnkDebrisParam_8015F0B8`, the debris spawn set up at 0x8015F0B8 and
    launched by `fn_8015D93C`.
  - `Game/GadgetCharCommands.h`, `Game/LineTarget.h`: more command classes, and `LineTarget`.
  - `Game/GadgetCharCollision.h`: `GadgetCharCollision`, the contact handle `fn_8015B8C4` returns
    (`shared_ptr<GadgetCharCollision>`; real name from its control block's RTTI). Units that only
    need its layout include it; `tools/rename_class.py Old New` renames a placeholder class
    project-wide, mangled `symbols.txt` names included.
  - `Game/GadgetCollision.h`, `Game/GadgetUtil.h`, `Game/CharCommand.h`: hit lists, collision
    callbacks, common helpers and character commands.
- **A new class that reaches a shared instance goes in a header under `include/Game/`**, named from
  RTTI when `tools/class_catalog.py` finds one (search the `sp_counted_base_impl<...>` records too).
- **Before inventing a placeholder, check `symbols.txt` at the instance's address.** If it already
  has a mangled name, use the class it names.
- **When a name has to change**, reset the affected entries with
  `python tools/reset_symbols.py --match OldName` (or `--stale Game/<Unit>` for names a unit no
  longer defines). Rebuild, then rerun `tools/pair_symbols.py --apply` for each unit that uses it.

### Units whose `.data` is kept whole

When a unit's code reaches `.data` through one base register (its constructor loads the address of
the first object and adds offsets), the linker keeps that unit's whole `.data`, including objects
that are normally dead-stripped. About 71 units show this. Their `.data` then:

- **starts with RTTI base lists** for `TNumEditHex/Int/Float` and `CharaColliBaseCharInfo`.
  Include `Game/TNumEdit.h` and `Game/CharaColliBaseCharInfo.h`, ordered to match retail. The
  derived classes' own RTTI records are stripped, so only the lists remain.
- **ends with zero-filled copies** of the weak `std::exception`, `boost::bad_weak_ptr` and
  `sp_counted_base` vtables. The real `weak_ptr::lock()` and `bad_weak_ptr` in `boost/` produce
  them. They come out zeroed because another unit's copies are the ones linked, and they need global
  names in `symbols.txt` (`__vt__Q25boost12bad_weak_ptr`, `__vt__Q35boost6detail15sp_counted_base`).

The pool base symbol at the start of such a section should be the compiler's `@N`. Never accept
`...data.0` as a name; `pair_symbols.py` now refuses it.

### Other conventions seen so far

- The out-of-line vector assignment is `fn_800091D0(dest, src)`. Pass a return value straight in
  through a const-reference inline wrapper, as `AssignVector` in JumpBoard.cpp does. A named
  temporary adds a copy.
- `.sbss`/`.sdata` globals are reached r13-relative only if declared as small objects (`u32`),
  not as arrays of unknown size.
- `CharInfo`'s vtable slot 0x10 is `PushCommand(CharCommand *)`. Command classes take their names
  from RTTI (`SetVelocityCommand`, `Player::LaunchCommand`, ...) and allocate through
  `CharCommand::operator new` (0x801D9C84). Give each class its real size: `new` passes `sizeof`.

- Function-local statics with class initializers compile to a 1-byte guard in `.sbss` plus
  storage in `.bss`. Each element is copy-constructed through `fn_800091EC` from a temporary.
  - Emulate them with a `static s8` guard, a `static float [N][3]` array, and **named** local
    temporaries passed to `fn_800091EC`. See `LongSpring.cpp`.
  - Don't use an inline wrapper here: an argument of an inline call gets its constructor called
    out of line.
- Integer constants loaded from `.sdata2` (about 35 sites): declare them at namespace scope
  (`const u32 kFlag = 0x100;`). GC/2.7 doesn't fold those and emits them at first use, which gives
  retail's placement; a function-local `static const` gets folded instead.
- A local `shared_ptr` whose pointer retail reloads from the stack after each call (about 25 sites)
  matches when each call is its own statement (`u32 a = p->Get(0); p->Set(a);`), not nested.
- A float product of a member load and an inline call result ignores source operand order under
  GC/2.7: both `(x - (float)fn_803B1618(x)) * freq` and `freq * (x - (float)fn_803B1618(x))` emit
  `fmuls fD, f_freq, f_sub`, while retail has the operands the other way. Naming the subtraction
  (`float phase = x - (float)fn_803B1618(x);`) makes `fmuls` follow the source. See
  `CameraShake.cpp`, `fn_8000EFC0`, which needed this for one instruction.
- A negated half-width computed from a variable: write `-total / 2.0f`, not `-total * 0.5f`. GC/2.7
  folds a divide by a power of two into the same reciprocal multiply retail uses but keeps `fneg`
  on `total` itself, which is what retail has; the multiply form negates the pool constant instead
  and loads it separately. In `TGiNumberDisplay.cpp`, `fn_800BC7BC` this was the last two rows.
- Where a quad's bottom edge is computed decides the register chain. Spelling the store's fourth
  operand as `y + height` (added at the store) instead of folding the height into `y` first is what
  retail does; the folded form re-uses one register for both and costs 15 rows in `fn_800BC7BC`.
  Both spellings are the same value, so the choice is invisible in the C but not in the schedule.
- Saved-register slots follow live-range order, so a parameter that the prologue can consume
  immediately never occupies one. `PadButtons.cpp`'s `fn_803560C8` matches exactly because its pad
  argument is folded into `mulli r29,r4,40` in the prologue, which leaves the counter in r27, the
  mask in r28, the pad offset in r29 and the row offset in r30; `fn_80356190` has to keep the
  argument live to become `r4` at a call, and GC/2.7 then hands it r27 and pushes the counter to
  r29 - the reverse of retail. 16 spellings of the mask, counter and pad declarations all stay at
  that 5-row floor (see `probes/Game_PadButtons.py`).
- A static callback (`HitCharacter`) defined *before* the constructor is emitted, with its float
  constants, where retail has it.
- Stack order follows declaration order: earlier locals get higher addresses. If two locals'
  slots are swapped, move a declaration up, as with LongSpring's `velocity`.
- Classes with a virtual base (the enemy framework, `Game/Enemy.h`):
  - An RTTI base list is the class's bases in reverse pre-order (each base before its own bases,
    in declaration order). Retail lists `EnemyReferer` last for the interface classes, so they
    name the virtual base first (`: public virtual EnemyReferer, public EnemyStatFunc`). This
    doesn't change the layout.
  - Virtual-base thunks (`@272@...`) of one offset come out in reverse order of the base class's
    member declarations, then its bases' declarations. A new virtual declared before the
    overrides (`~EnemyDispCommon`) gets its thunk last; new virtuals keep their relative order,
    so the slot layout stays the same. See `BkSoldier.cpp`.
- Functions that convert between float and int (a `fctiwz` plus a `0x43300000` sequence) may stay
  at about 99.8%. The original compiler shares one stack slot for these; see
  `docs/research-notes.md`. Leave them NonMatching with a comment and move on.

## 3. Pair the symbols

**Use `python tools/pair_symbols.py Game/<Name>`.** It pairs the unit's functions with the retail
ones by name, then by identical code, then by unique size. It then walks every relocation and
branch of each pair and proposes names for retail placeholders: functions, shared template
instances, vtables, RTTI, strings and statics, with object-local ones marked `scope:local`. It
also follows the pointers inside data it has placed (RTTI records to their name strings, vtables,
type records), so rerunning it after a header change renumbers the compiler's `@N` locals. Review
the output, then rerun it with `--apply`. Rename by hand only what it can't pair.

Manual route:
- List what your object defines and calls:
  `build/binutils/powerpc-eabi-nm.exe -n -S build/GUPE8P/src/Game/<Name>.o`.
- Rename the target symbols to your mangled names:
  `python tools/rename_symbols.py 0xADDR=MangledName ...`.
  - This includes the boost instances the unit calls (for example
    `lock__Q25boost41weak_ptr<Q36Effect6System13EffectElement>CFv`). The link depends on it: an
    unpaired weak copy would be kept and shift the DOL.
- For 100% data, rename the target's local objects to the compiler's local names:
  - string literals and constants (`@38`, `@98`, ...);
  - RTTI lists (`@106`, `@103`);
  - `__RTTI__…` records;
  - mark them `scope:local`, and fix sizes that include padding (vtable 0x20, RTTI 0x8).

## 4. Verify

```sh
python configure.py && ninja                  # must end with: build/GUPE8P/main.dol: OK
python tools/unitstatus.py Game/<Name>        # per-function and per-section match %
python tools/fnstatus.py                      # every symbol below 100%, cheapest unit first
python tools/fnside.py Game/<Name> <symbol>   # our object vs the retail split, instruction by instruction
```

Section sizes are *not* the gate, and it is worth checking a fully matched unit before assuming one:
`build/GUPE8P/ldscript.lcf` puts every input in one `GROUP` in `configure.py` order with no per-unit
origin, so mwldeppc lays the sections out by itself and folds weak/linkonce duplicates -
`Game/AdvSynopsis.cpp` carries 3060 bytes more `.text` than its retail split and still links
byte-identically. What decides a flip is only the byte-for-byte check, so try it with
`python tools/flipcheck.py Game/<Name>.cpp`, and read `python tools/flipsym.py Game/<Name>` when it
fails: the difference is usually extra symbols our object defines that the retail split has in a
neighbouring unit, or data records retail has and our source does not emit.
`python tools/unitstatus.py --flip` lists the units with every symbol matched, which are the only
ones worth testing.

When one or two symbols are left, `python tools/uprobe.py -u Game/<Name>` compiles and scores whole
tables of source spellings against the retail object (probe tables in `probes/`, `--keep NAME`
leaves the winner in the source). Some residuals are GC/2.7's list scheduler or register chooser
rather than the C, and no spelling fixes them; the floors found so far are documented in the source
comments and in `docs/research-notes.md` (`MemoryCardCrypt` 18 rows, `EnemyArea` 1 and 4 rows,
`PadButtons` 5 and 15 rows). Leave such units `NonMatching` with a comment naming the floor.

When every function and section is 100%, change the unit to `Object(Matching, ...)`. Run `ninja`
again. `main.dol: OK` with the unit linked is the finish line. If the hash fails, run
`python tools/doldiff.py` to see which addresses moved.
