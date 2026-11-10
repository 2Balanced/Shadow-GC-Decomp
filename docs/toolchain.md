# Toolchain, tools and references

Background for contributors. The README covers building.

## Toolchain facts


| Item | Finding | Evidence |
| ---- | ------- | -------- |
| Game build date | Oct 10 2005 | string in `.rodata` |
| Linker | CodeWarrior for GameCube 2.7 | `__init_cpp_exceptions` 0x3C / `__fini_cpp_exceptions` 0x34, no `GetR2`, `.ctors$10`/`.dtors$15` layout; 3.0a3 (build 51213 = 2005‑12‑13) postdates the game |
| Game code flags | `-O4,s -use_lmw_stmw on` | `main()` @ 0x80050FD4 matches byte-for-byte with these flags on GC 1.3.2–2.7, not with `-O4,p` or 3.0a3 |
| Dolphin SDK | 20Apr2004 + Patch2 (OS/GX "Nov 10 2004", rev 0x2301) | `<< Dolphin SDK - ... >>` banners; prebuilt, so compile with GC/1.2.5n `-O4,p` |
| MetroTRK | "MetroTRK for GAMECUBE v2.6" | string |
| CRI | ADXT/GC 9.44, MWSFD 3.50, SFD/MPV 1.958 (Feb 28 2005, "MW2407 GC20Apr2004Patch2") | version banners |
| RenderWare | 3.7.0.2 | `RwEngineGetVersion` @ 0x80485D6C returns `0x37002` |
| C++ | boost `shared_ptr`/`function`/`bind`, RTTI, exceptions (extab present), namespace `SonicteamUSA::` | RTTI strings |

Approximate `.text` layout: game code 0x80008D40–0x8036FC94 · Dolphin SDK
0x8036FC94–~0x8039A000 · MetroTRK · Runtime/MSL ~0x803A0000–0x803B3000 ·
more SDK · CRI ADX/Sofdec ~0x803B6700–0x80408000 · Sonic Team engine/effect
library · RenderWare ~0x8043AA7C–0x804A9C00.

RenderWare's `.text` is fully matched and linked. `Game/RwText01.s` and `Game/RwText02.s`
preserve the retail code as Gecko assembly. In `RwText02.s`, 210 branches with `+`/`-`
prediction hints use exact `.long` words because GNU `as -mgekko` otherwise drops the hint bits.

Linker note: the GC 2.7 linker deadstrips the compiled runtime's
`__init_cpp_exceptions_reference` / `__fini_cpp_exceptions_reference`, so they
are listed under `force_active` in `config/GUPE8P/config.yml`.

## Tools

| Tool | Where | Use |
| ---- | ----- | --- |
| objdiff GUI | `.tools/objdiff-windows-x86_64.exe` | Open the repo as the project directory; per-function diffs, auto-rebuild |
| m2c | `.tools/venv/Scripts/m2c.exe` | First-draft C from `build/GUPE8P/asm/*.s` |
| `tools/cctest.py` | repo | Sweep compiler versions/flags for one function against the retail DOL |
| `tools/doldiff.py` | repo | When the SHA-1 check fails, list the differing addresses |
| `tools/pair_symbols.py` | repo | Pair a unit's object with the retail DOL and propose `symbols.txt` names (code and data references, `--apply`) |
| `tools/reset_symbols.py`, `tools/rename_symbols.py` | repo | Reset names to placeholders / apply `0xADDR=Name` renames |
| `tools/rename_class.py`, `tools/rename_ident.py` | repo | Rename a class or any other identifier in sources and `symbols.txt` (mangled names included) |
| `tools/unitstatus.py`, `tools/fndiff.py` | repo | Per-unit and per-function match status; side-by-side function diff |
| `tools/fnstatus.py` | repo | Every symbol below 100% match, one row each, units ranked by how little is left |
| `tools/flipcheck.py` | repo | Flip one unit to `Matching` in `configure.py`, relink, and keep it only if `main.dol`'s SHA-1 equals retail's |
| `tools/flipsym.py` | repo | Our object versus the retail split object of a whole unit: missing, extra and size-mismatched symbols. `--words .data,.sdata` dumps a data section word by word with the relocation that fills it resolved; `--refs` compares the order in which the two objects first reference each `__vt__`/`__RTTI__` record |
| `tools/uprobe.py` | repo | Compile one function with several source spellings from a table in `probes/<unit>.py`, and report which spellings change the code |
| `tools/notesaudit.py` | repo | Which `NonMatching` units have no rationale note yet (`--notes`), and which notes cite a script that is not in git (`--cites`) |
| `tools/upstreamdrift.py` | repo | What `dtk-template` has changed since the last vendor sync, split into files that can be checked out as-is and files we have edited (`--apply` prints the checkout) |
| `tools/textorder.py`, `tools/fnorder.py`, `tools/dataorder.py` | repo | What breaks a flip: which functions of a unit would not land at retail's addresses, in retail's order and in ours (`fnorder.py` reads the function symbol values of our single `.text`), and the same section-by-section for one data section |
| `tools/dolsections.py` | repo | Retail's DOL section table against the sections of the built DOL (`--built-only` for ours), to tell a `.text` drift from a linkinfo-table difference |
| `tools/class_catalog.py` | repo | List the game's C++ classes from vtables and RTTI |
| `tools/similarity.py` | repo | How closely our files follow a reference project's same-named files (see CONTRIBUTING.md, References) |
| `tools/objcompare.py`, `tools/cc_game.sh` | repo | Compile one game unit directly; compare two objects section by section |
| `tools/ghidra/ImportDtkSymbols.java` | repo | Push names from `symbols.txt` into a Ghidra program |
| REA (morluto/rea) | `.tools/rea.cmd` | Evidence-tracked Ghidra analysis. Needs Ghidra 12.1.x; with the GameCubeLoader installed for 12.1.2 Ghidra can import the DOL (headless: pass `-loader-autoloadMaps false`). Also useful on the Sonic Heroes PC exe |
| Ghidra + a GameCube loader extension | local install | Browse the DOL; the loader's Gekko/Broadway language must be built for the installed Ghidra version |

Example compiler sweep:

```sh
python tools/cctest.py path/to/func.c main 0x80050FD4 --size 0x170 \
    --versions 1.3.2 2.6 2.7 3.0a3 "--flags=-O4,s -use_lmw_stmw on|-O4,p" --show
```

## Reference material

Licensed and reusable:

- [zeldaret/tp](https://github.com/zeldaret/tp) (CC0): MSL/Runtime, MetroTRK v2.6, Dolphin SDK rev 2
- [ShulkMaster/mk-deception](https://github.com/ShulkMaster/mk-deception) (CC0): GC/2.7 runtime, TRK v2.6, CRI ADX 9.28 / Sofdec 3.31 with original CRI paths
- [projectPiki/pikmin2](https://github.com/projectPiki/pikmin2) (CC0): GC/2.6 runtime and TRK
- [Jovinull/sonicheroes](https://github.com/Jovinull/sonicheroes) (MIT): same engine lineage, about 30% matched

They are read to understand library code, not copied; see CONTRIBUTING.md, References.

Where a reference was followed:

| Our files | Reference | Status |
| --------- | --------- | ------ |
| `src/dolphin/` (PPCArch, db, OSAlarm, OSAlloc, OSArena, OSAudioSystem, OSCache, OSInterrupt, OSSync, OSTime, `__ppc_eabi_init`), `src/Runtime.PPCEABI.H/` (`__mem`, `__va_arg`, `global_destructor_chain`, `CPlusLibPPC`, `ptmf`, `runtime`), `include/dolphin/` | zeldaret/tp `libs/dolphin`, `libs/PowerPC_EABI_Support` (CC0); pull request #1 db, OSAlarm, OSAlloc, OSArena, OSAudioSystem, OSCache, OSInterrupt, OSTime, `__ppc_eabi_init` and CPlusLibPPC were rewritten in this project's own words from the disassembly; every file now passes `tools/similarity.py` against tp, mk-deception and pikmin2 |

This project is independent of other decompilations of this game and does not use their
code.

Leaked RenderWare/CRI SDK source is never a source for this project.

Caution on existing symbol maps: the community/Dolphin `GUPE8P.map` and the
older Ghidra databases apply many names by signature matching from unrelated games
(`cPlayer`, `cFielder`, `FormationOffensive` from *Mario Strikers*, Wii `nw4hbm`,
JSystem `J3D*`). Treat those names as unverified. Names backed by RTTI strings,
hand analysis, or library banners are reliable.
