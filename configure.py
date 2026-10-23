#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GUPE8P",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.3"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# Game C++ flags (see docs/research-notes.md). main() at 0x80050FD4 matches only with -O4,s and
# stmw/lmw. The gadget code needs RTTI (its classes have RTTI records), read-only pooled strings, and
# deferred inlining one level deep: functions come out in reverse definition order, and an inline
# function whose body calls another inline function is called out of line.
cflags_game_cpp = [
    *cflags_base,
    "-O4,s",
    "-use_lmw_stmw on",
    "-str reuse,readonly",
    "-RTTI on",
    "-inline noauto,level=1,deferred",
    # Stand-in MSL headers (<cmath>, <memory>, <typeinfo>, ...) for the game code and boost.
    "-i src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include",
    "-i src/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include",
]

# The Chao CSD 2D library (src/Game/csd*.cpp; see Game/csdAccess.cpp): game flags without RTTI and
# small data, with automatic inlining one level deep and no deferred inlining, so functions come out
# in definition order.
cflags_csd = [
    *cflags_base,
    "-O4,s",
    "-use_lmw_stmw on",
    "-str reuse,readonly",
    "-inline auto,level=1",
    "-sdata 0",
    "-sdata2 0",
    "-i src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include",
    "-i src/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include",
]

# The Chao CSD library's RenderWare layer ("libcsd_rw" in its assert messages; Game/csdTexListRW.cpp
# and the units around it): the library's flags with deferred inlining, so functions come out in
# reverse definition order and are inlined into functions defined before them.
cflags_csd_rw = [
    *cflags_csd,
    "-inline deferred",
]

# REL flags
cflags_rel = [
    *cflags_base,
    "-sdata 0",
    "-sdata2 0",
]

config.linker_version = "GC/2.7"


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_base,
        "progress_category": "sdk",
        "objects": objects,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    {
        "lib": "Runtime.PPCEABI.H",
        "mw_version": "GC/2.7",
        "cflags": cflags_runtime,
        "progress_category": "sdk",  # str | List[str]
        "objects": [
            Object(Matching, "Runtime.PPCEABI.H/__mem.c"),
            Object(Matching, "Runtime.PPCEABI.H/__va_arg.c"),
            Object(Matching, "Runtime.PPCEABI.H/global_destructor_chain.c"),
            Object(Matching, "Runtime.PPCEABI.H/CPlusLibPPC.cp"),
            Object(Matching, "Runtime.PPCEABI.H/ptmf.c"),
            Object(Matching, "Runtime.PPCEABI.H/runtime.c"),
            Object(Matching, "Runtime.PPCEABI.H/__init_cpp_exceptions.cpp"),
        ],
    },
    DolphinLib(
        "base",
        [
            Object(Matching, "dolphin/base/PPCArch.c"),
        ],
    ),
    DolphinLib(
        "db",
        [
            Object(Matching, "dolphin/db/db.c"),
        ],
    ),
    DolphinLib(
        "os",
        [
            Object(Matching, "dolphin/os/OSAlarm.c"),
            Object(Matching, "dolphin/os/OSAlloc.c"),
            Object(Matching, "dolphin/os/OSArena.c"),
            Object(Matching, "dolphin/os/OSAudioSystem.c"),
            Object(Matching, "dolphin/os/OSCache.c"),
            Object(Matching, "dolphin/os/OSInterrupt.c"),
            Object(Matching, "dolphin/os/OSSync.c"),
            Object(Matching, "dolphin/os/OSTime.c"),
            Object(Matching, "dolphin/os/__ppc_eabi_init.cpp"),
        ],
    ),
    {
        # Gadget units. They have no splits yet, so none is linked.
        "lib": "Game",
        "mw_version": "GC/2.7",
        "cflags": cflags_game_cpp,
        "progress_category": "game",
        "objects": [
            Object(Matching, "Game/ActStageController.cpp"),
            Object(Matching, "Game/AdjustCosRot.cpp"),
            Object(Matching, "Game/AdvBase.cpp"),
            Object(Matching, "Game/AdvCsdCache.cpp"),
            Object(Matching, "Game/AdvCsdEffect.cpp"),
            Object(NonMatching, "Game/AdvFolder.cpp"),
            Object(Matching, "Game/AdvBattleMode.cpp"),
            Object(Matching, "Game/AdvBgm.cpp"),
            Object(Matching, "Game/AdvLogo.cpp"),
            Object(NonMatching, "Game/AdvMemoryCheck.cpp"),
            Object(Matching, "Game/AdvMessage.cpp"),
            Object(Matching, "Game/AdvMovie.cpp"),
            Object(Matching, "Game/AirBoltModel.cpp"),
            Object(NonMatching, "Game/AdvOption.cpp"),
            Object(Matching, "Game/AdvPadInput.cpp"),
            Object(Matching, "Game/AdvStoryLibrary.cpp"),
            Object(NonMatching, "Game/AdvSelectMode.cpp"),
            Object(Matching, "Game/AdvStaffRoll.cpp"),
            Object(NonMatching, "Game/AdvStaffRollFont.cpp"),
            Object(NonMatching, "Game/AdvStaffRollMovie.cpp"),
            Object(Matching, "Game/AdvStaffRollText.cpp"),
            Object(Matching, "Game/AdvStageTable.cpp"),
            Object(NonMatching, "Game/AdvSynopsis.cpp"),
            Object(Matching, "Game/AdvTitle.cpp"),
            Object(Matching, "Game/Advertise.cpp"),
            Object(NonMatching, "Game/ArkCannon.cpp"),
            Object(Matching, "Game/ArkLaser.cpp"),
            Object(NonMatching, "Game/AudioDataBank.cpp"),
            Object(Matching, "Game/AudioLoopManagerTask.cpp"),
            Object(NonMatching, "Game/AudioManagerTask.cpp"),
            Object(Matching, "Game/AttackObject.cpp"),
            Object(Matching, "Game/BAGunShip.cpp"),
            Object(Matching, "Game/BAMine.cpp"),
            Object(Matching, "Game/BAShield.cpp"),
            Object(Matching, "Game/BASwitch.cpp"),
            Object(Matching, "Game/BDHologram.cpp"),
            Object(Matching, "Game/BackDrop.cpp"),
            Object(NonMatching, "Game/BalloonGenerator.cpp"),
            Object(Matching, "Game/BaseDoor.cpp"),
            Object(Matching, "Game/BasicMessage.cpp"),
            Object(Matching, "Game/BDTrue.cpp"),
            Object(Matching, "Game/BirdBase.cpp"),
            Object(Matching, "Game/BirdModelBase.cpp"),
            Object(NonMatching, "Game/BkChaos.cpp"),
            Object(NonMatching, "Game/BkChaosAI.cpp"),
            Object(Matching, "Game/BkLarva.cpp"),
            Object(NonMatching, "Game/BkNinja.cpp"),
            Object(NonMatching, "Game/BkNinjaAI.cpp"),
            Object(Matching, "Game/BkSoldier.cpp"),
            Object(NonMatching, "Game/BkSoldierAI.cpp"),
            Object(Matching, "Game/BkWingLarge.cpp"),
            Object(NonMatching, "Game/BkWingLargeAI.cpp"),
            Object(Matching, "Game/BkWingSmall.cpp"),
            Object(Matching, "Game/BkWingSmallAI.cpp"),
            Object(NonMatching, "Game/BkWorm.cpp"),
            Object(NonMatching, "Game/BkWormAI.cpp"),
            Object(Matching, "Game/BlackBull0210.cpp"),
            Object(Matching, "Game/BlackBull0210CtrlUnit.cpp"),
            Object(NonMatching, "Game/BlackBull0210ModeCtrlUnit.cpp"),
            Object(Matching, "Game/BlackBull0412.cpp"),
            Object(Matching, "Game/BlackBull0412CtrlUnit.cpp"),
            Object(Matching, "Game/BlackBull0412ModeCtrlUnit.cpp"),
            Object(Matching, "Game/BlackBullCtrlUnit.cpp"),
            Object(Matching, "Game/BlackDoomCtrlUnit.cpp"),
            Object(Matching, "Game/BlackDoom.cpp"),
            Object(Matching, "Game/BlackDoomCtrlUnitAlterEgo.cpp"),
            Object(Matching, "Game/BlackDoomModeCtrlUnit.cpp"),
            Object(Matching, "Game/BKTank.cpp"),
            Object(Matching, "Game/BKTankColi.cpp"),
            Object(Matching, "Game/BKTankController.cpp"),
            Object(Matching, "Game/BlastBreak.cpp"),
            Object(Matching, "Game/BlockChange.cpp"),
            Object(Matching, "Game/Bomb.cpp"),
            Object(Matching, "Game/BombingWall.cpp"),
            Object(Matching, "Game/BombServer.cpp"),
            Object(Matching, "Game/BossCallbackContainer.cpp"),
            Object(Matching, "Game/BossCameraLamborghini.cpp"),
            Object(NonMatching, "Game/BossCharaColli.cpp"),
            Object(Matching, "Game/BossCtrl.cpp"),
            Object(NonMatching, "Game/BossCtrlUnit.cpp"),
            Object(Matching, "Game/BossDesire.cpp"),
            Object(Matching, "Game/BossDesireManager.cpp"),
            Object(Matching, "Game/BossForceClear.cpp"),
            Object(Matching, "Game/BossMotion.cpp"),
            Object(Matching, "Game/BossRender.cpp"),
            Object(Matching, "Game/BossRootTask.cpp"),
            Object(Matching, "Game/BossSonic.cpp"),
            Object(Matching, "Game/BossSonicControlAI.cpp"),
            Object(Matching, "Game/BossSonicControlRestart.cpp"),
            Object(Matching, "Game/BossSonicDesireAttack.cpp"),
            Object(NonMatching, "Game/BossSonicDesireCounterAttack.cpp"),
            Object(Matching, "Game/BossSonicDesireDodge.cpp"),
            Object(NonMatching, "Game/BossSonicDesireMove.cpp"),
            Object(Matching, "Game/BossSonicExecuter.cpp"),
            Object(Matching, "Game/BossSonicSonicTypeControl.cpp"),
            Object(Matching, "Game/BossSonicSubControlDodge.cpp"),
            Object(Matching, "Game/BossSonicSubControlJump.cpp"),
            Object(Matching, "Game/BossSonicTarget.cpp"),
            Object(Matching, "Game/BossSonicWork.cpp"),
            Object(Matching, "Game/BossTarget.cpp"),
            Object(Matching, "Game/BreakObj.cpp"),
            Object(Matching, "Game/BreakRoad.cpp"),
            Object(Matching, "Game/BreakWall.cpp"),
            Object(Matching, "Game/BulletChaosSpear.cpp"),
            Object(Matching, "Game/BulletDummy.cpp"),
            Object(Matching, "Game/BulletHitEffect.cpp"),
            Object(NonMatching, "Game/BulletLaser.cpp"),
            Object(NonMatching, "Game/BulletSatelliteLaser.cpp"),
            Object(Matching, "Game/Cage.cpp"),
            Object(NonMatching, "Game/CameraManagerUnit.cpp"),
            Object(Matching, "Game/CameraModeFixLookChar.cpp"),
            Object(NonMatching, "Game/CameraScreenSet.cpp"),
            Object(Matching, "Game/CandleStick.cpp"),
            Object(Matching, "Game/CaptureCage.cpp"),
            Object(NonMatching, "Game/Catapult.cpp"),
            Object(Matching, "Game/CatchStick.cpp"),
            Object(Matching, "Game/CenFile.cpp"),
            Object(Matching, "Game/CenFileNumber.cpp"),
            Object(Matching, "Game/ChaoHolder.cpp"),
            Object(Matching, "Game/CharacterFiles.cpp"),
            Object(NonMatching, "Game/CircusBall.cpp"),
            Object(Matching, "Game/CircusPole.cpp"),
            Object(Matching, "Game/CityBombBig.cpp"),
            Object(Matching, "Game/CityBombSmall.cpp"),
            Object(Matching, "Game/CityGadget.cpp"),
            Object(Matching, "Game/ClumpFrameEffects.cpp"),
            Object(NonMatching, "Game/Coaster.cpp"),
            Object(Matching, "Game/ContainerBehavior.cpp"),
            Object(Matching, "Game/ContainerBreak.cpp"),
            Object(Matching, "Game/ContainerContent.cpp"),
            Object(Matching, "Game/ContainerHard.cpp"),
            Object(Matching, "Game/ContainerNormal.cpp"),
            Object(Matching, "Game/ContainerSecret.cpp"),
            Object(Matching, "Game/ContainerUnbreak.cpp"),
            Object(Matching, "Game/ContainerWeapon.cpp"),
            Object(Matching, "Game/ContinueAction.cpp"),
            Object(Matching, "Game/CountMessage.cpp"),
            Object(Matching, "Game/Cream.cpp"),
            Object(NonMatching, "Game/csdAccess.cpp", cflags=cflags_csd),
            Object(Matching, "Game/csdBase.cpp", cflags=cflags_csd),
            Object(Matching, "Game/csdFileSystem.cpp", cflags=cflags_csd),
            Object(Matching, "Game/csdFileSystemRW.cpp", cflags=cflags_csd_rw),
            Object(NonMatching, "Game/csdLoader.cpp", cflags=cflags_csd),
            Object(Matching, "Game/csdMatrix.cpp", cflags=cflags_csd),
            Object(Matching, "Game/csdmMotionPalette.cpp", cflags=cflags_csd_rw),
            Object(Matching, "Game/csdmMotionPattern.cpp", cflags=cflags_csd_rw),
            Object(NonMatching, "Game/csdmNode.cpp", cflags=cflags_csd_rw),
            Object(NonMatching, "Game/csdMotionPalette.cpp", cflags=cflags_csd),
            Object(NonMatching, "Game/csdmScene.cpp", cflags=cflags_csd_rw),
            Object(Matching, "Game/csdPlatform.cpp", cflags=cflags_csd),
            Object(Matching, "Game/csdPlatformRW.cpp", cflags=cflags_csd_rw),
            Object(NonMatching, "Game/csdRW.cpp", cflags=cflags_csd_rw),
            Object(Matching, "Game/csdScreen.cpp", cflags=cflags_csd),
            Object(Matching, "Game/csdTexList.cpp", cflags=cflags_csd),
            Object(Matching, "Game/csdTexListRW.cpp", cflags=cflags_csd_rw),
            Object(Matching, "Game/CsdOneShot.cpp"),
            Object(Matching, "Game/DamageBlock.cpp"),
            Object(Matching, "Game/DeadAction.cpp"),
            Object(Matching, "Game/DarkSpinEntrance.cpp"),
            Object(NonMatching, "Game/DashPanel.cpp"),
            Object(NonMatching, "Game/DashRing.cpp"),
            Object(Matching, "Game/DebugDisp.cpp"),
            Object(Matching, "Game/DebugDispBox.cpp"),
            Object(Matching, "Game/DebugDispCapsule.cpp"),
            Object(Matching, "Game/DebugDispCylinder.cpp"),
            Object(Matching, "Game/DebugDispLine.cpp"),
            Object(Matching, "Game/DebugDispSphere.cpp"),
            Object(Matching, "Game/DebugMissionClearCollision.cpp"),
            Object(Matching, "Game/DefenseShield.cpp"),
            Object(Matching, "Game/DefenseSystem.cpp"),
            Object(Matching, "Game/DevilDoom.cpp"),
            Object(NonMatching, "Game/DevilDoomModeCtrlUnit.cpp"),
            Object(NonMatching, "Game/DevilDoomCtrlUnit.cpp"),
            Object(Matching, "Game/DevilDoomCtrlUnitBit.cpp"),
            Object(NonMatching, "Game/DevilDoomCtrlUnitPsychoObject.cpp"),
            Object(Matching, "Game/Diablon.cpp"),
            Object(Matching, "Game/DiablonBehaviorBase.cpp"),

            Object(Matching, "Game/DiablonBehaviorBeam.cpp"),
            Object(Matching, "Game/DiablonBehaviorEvent.cpp"),
            Object(Matching, "Game/DiablonBehaviorIdle.cpp"),
            Object(Matching, "Game/DiablonBehaviorBomb.cpp"),
            Object(Matching, "Game/DiablonBehaviorMove.cpp"),
            Object(Matching, "Game/DiablonBehaviorWipe.cpp"),
            Object(Matching, "Game/DiablonController.cpp"),
            Object(Matching, "Game/DiablonDesireBase.cpp"),
            Object(Matching, "Game/DiablonDesireBeam.cpp"),
            Object(Matching, "Game/DiablonDesireBomb.cpp"),
            Object(Matching, "Game/DiablonDesireEvent.cpp"),
            Object(Matching, "Game/DiablonDesireMove.cpp"),
            Object(Matching, "Game/DiablonDesireWipe.cpp"),
            Object(Matching, "Game/DiablonEffect.cpp"),
            Object(NonMatching, "Game/DiablonExecuter.cpp"),
            Object(Matching, "Game/DiablonParam.cpp"),
            Object(Matching, "Game/DiablonPartsBarrier.cpp"),
            Object(Matching, "Game/DiablonPartsBase.cpp"),
            Object(Matching, "Game/DiablonPartsBody.cpp"),
            Object(Matching, "Game/DiablonPartsBeam.cpp"),
            Object(Matching, "Game/DiablonPartsBomb.cpp"),
            Object(Matching, "Game/DiablonPartsCameraControl.cpp"),
            Object(Matching, "Game/DiablonPartsCharge.cpp"),
            Object(Matching, "Game/DiablonPartsChargeMeter.cpp"),
            Object(Matching, "Game/DiablonPartsHPGauge.cpp"),
            Object(Matching, "Game/DiablonPartsHead.cpp"),
            Object(Matching, "Game/DiablonPartsModelBase.cpp"),
            Object(Matching, "Game/DiablonPartsShoulder.cpp"),
            Object(Matching, "Game/DiablonSetData.cpp"),
            Object(Matching, "Game/DiscErrorHandler.cpp"),
            Object(Matching, "Game/Door.cpp"),
            Object(NonMatching, "Game/EffBelt.cpp"),
            Object(Matching, "Game/EffectGlobalParamUSA.cpp"),
            Object(Matching, "Game/EffectKage.cpp"),
            Object(Matching, "Game/EffectLauncherTable.cpp"),
            Object(Matching, "Game/EffectManagerPJS.cpp"),
            Object(Matching, "Game/EffectObjectPJS.cpp"),
            Object(Matching, "Game/EffectParticle3DPJS.cpp"),
            Object(Matching, "Game/EffectPJS.cpp"),
            Object(Matching, "Game/EffectTask.cpp"),
            Object(NonMatching, "Game/EffLocusMultiVtx.cpp"),
            Object(Matching, "Game/EggBalloon.cpp"),
            Object(Matching, "Game/EggBalloonColi.cpp"),
            Object(Matching, "Game/EggBalloonController.cpp"),
            Object(Matching, "Game/EggLastMech.cpp"),
            Object(NonMatching, "Game/EggLastMechAIState.cpp"),
            Object(NonMatching, "Game/EggLastMechArea.cpp"),
            Object(Matching, "Game/EggLastMechCamera.cpp"),
            Object(Matching, "Game/EggLastMechEffects.cpp"),
            Object(Matching, "Game/EggLastMechFeverRing.cpp"),
            Object(Matching, "Game/EggLastMechPart.cpp"),
            Object(NonMatching, "Game/EggLastMechSlot.cpp"),
            Object(Matching, "Game/EggLastMechSlotDisp2D.cpp"),
            Object(Matching, "Game/EggLastMechSlotSwitch.cpp"),
            Object(Matching, "Game/EggLastMechTask.cpp"),
            Object(NonMatching, "Game/EggMecaCtrlUnit.cpp"),
            Object(Matching, "Game/EggMeca310.cpp"),
            Object(Matching, "Game/EggMeca411.cpp"),
            Object(Matching, "Game/EggMeca511.cpp"),
            Object(NonMatching, "Game/EggMecaModeCtrlUnit.cpp"),
            Object(NonMatching, "Game/EggPawn.cpp"),
            Object(NonMatching, "Game/EggPawnAI.cpp"),
            Object(Matching, "Game/ElecAccessPanel.cpp"),
            Object(Matching, "Game/ElecBar.cpp"),
            Object(Matching, "Game/ElecBarrier.cpp"),
            Object(Matching, "Game/ElecBlock.cpp"),
            Object(Matching, "Game/ElecBomb.cpp"),
            Object(Matching, "Game/ElecCircuitColli.cpp"),
            Object(Matching, "Game/ElecCircuitEntrance.cpp"),
            Object(Matching, "Game/ElecCircuitExit.cpp"),
            Object(Matching, "Game/ElecCoreProgram.cpp"),
            Object(Matching, "Game/ElecCoreWall.cpp"),
            Object(NonMatching, "Game/ElecCristalWall.cpp"),
            Object(Matching, "Game/ElecCristalWallSwitch.cpp"),
            Object(Matching, "Game/ElecFan.cpp"),
            Object(Matching, "Game/ElecFireWall.cpp"),
            Object(Matching, "Game/ElecFootingRotate.cpp"),
            Object(Matching, "Game/ElecKyuu.cpp"),
            Object(Matching, "Game/ElecRollHexa.cpp"),
            Object(Matching, "Game/ElecSearchLight.cpp"),
            Object(NonMatching, "Game/ElecSecurityCode.cpp"),
            Object(Matching, "Game/ElecSecurityCodeGenerator.cpp"),
            Object(Matching, "Game/ElecSmallCube.cpp"),
            Object(Matching, "Game/Elevator.cpp"),
            Object(Matching, "Game/EmbeddedWeapon.cpp"),
            Object(Matching, "Game/EndAction.cpp"),
            Object(Matching, "Game/Enemy.cpp"),
            Object(NonMatching, "Game/EnemyAttack.cpp"),
            Object(Matching, "Game/EnemyHint.cpp"),
            Object(Matching, "Game/EnemyMission.cpp"),
            Object(NonMatching, "Game/EnemyRender.cpp"),
            Object(Matching, "Game/EnemyTarget.cpp"),
            Object(Matching, "Game/EnemyWalkerAI.cpp"),
            Object(Matching, "Game/EnergyCore.cpp"),
            Object(Matching, "Game/EscapePlane.cpp"),
            Object(Matching, "Game/EscapePlaneRail.cpp"),
            Object(Matching, "Game/EventAction.cpp"),
            Object(Matching, "Game/FallRoad.cpp"),
            Object(Matching, "Game/Fan.cpp"),
            Object(Matching, "Game/Fire.cpp"),
            Object(Matching, "Game/FireRing.cpp"),
            Object(Matching, "Game/FlashbackAction.cpp"),
            Object(Matching, "Game/FlyTypeBehaviorFactory.cpp"),
            Object(Matching, "Game/FlyTypeControlFactory.cpp"),
            Object(Matching, "Game/FlyerCommonAI.cpp"),
            Object(NonMatching, "Game/FlyerCommonBase.cpp"),
            Object(Matching, "Game/FogTable.cpp"),
            Object(Matching, "Game/Footing.cpp"),
            Object(Matching, "Game/FootingMovable.cpp"),
            Object(Matching, "Game/Fruit.cpp"),
            Object(Matching, "Game/GiantLantern.cpp"),
            Object(Matching, "Game/Gadget.cpp"),
            Object(Matching, "Game/GadgetBuilder.cpp"),
            Object(NonMatching, "Game/GadgetCharCollision.cpp"),
            Object(Matching, "Game/GadgetCharInfo.cpp"),
            Object(Matching, "Game/GadgetColliDetectUtil.cpp"),
            Object(Matching, "Game/GadgetCommand.cpp"),
            Object(Matching, "Game/GadgetCommonData.cpp"),
            Object(Matching, "Game/GadgetCrash3D.cpp"),
            Object(NonMatching, "Game/GadgetModelInfo.cpp"),
            Object(Matching, "Game/GadgetParticle.cpp"),
            Object(Matching, "Game/GadgetPickup.cpp"),
            Object(Matching, "Game/GadgetStageData.cpp"),
            Object(Matching, "Game/GadgetSwitch.cpp"),
            Object(Matching, "Game/GadgetUtility.cpp"),
            Object(NonMatching, "Game/GamePeripheralTask.cpp"),
            Object(NonMatching, "Game/GiMissionTarget.cpp"),
            Object(NonMatching, "Game/GiGameOver.cpp"),
            Object(NonMatching, "Game/GoalAction.cpp"),
            Object(Matching, "Game/GoalMission.cpp"),
            Object(Matching, "Game/GoalRing.cpp"),
            Object(Matching, "Game/Gong.cpp"),
            Object(Matching, "Game/GravityChangeCollision.cpp"),
            Object(Matching, "Game/GravityChangeSwitch.cpp"),
            Object(Matching, "Game/GravityChangeZone.cpp"),
            Object(Matching, "Game/GravityDirection.cpp"),
            Object(Matching, "Game/GroundEffectTracker.cpp"),
            Object(Matching, "Game/GunBattery.cpp"),
            Object(Matching, "Game/GunBeetle.cpp"),
            Object(Matching, "Game/GunBeetleAI.cpp"),
            Object(Matching, "Game/GunBigfootAI.cpp"),
            Object(Matching, "Game/GunBuilding.cpp"),
            Object(Matching, "Game/GunLift.cpp"),
            Object(Matching, "Game/GunRobot.cpp"),
            Object(Matching, "Game/GunSoldier.cpp"),
            Object(NonMatching, "Game/GunRobotAI.cpp"),
            Object(NonMatching, "Game/GunSoldierAI.cpp"),
            Object(Matching, "Game/HealingUnit.cpp"),
            Object(Matching, "Game/HeavyDog0410.cpp"),
            Object(Matching, "Game/HeavyDog0410CtrlUnit.cpp"),
            Object(Matching, "Game/HeavyDog0410ModeCtrlUnit.cpp"),
            Object(Matching, "Game/HeavyDog0510.cpp"),
            Object(Matching, "Game/HeavyDog0510CtrlUnit.cpp"),
            Object(NonMatching, "Game/HeavyDog0510ModeCtrlUnit.cpp"),
            Object(NonMatching, "Game/HeavyDogCtrlUnit.cpp"),
            Object(NonMatching, "Game/Helicopter.cpp"),
            Object(Matching, "Game/HintController.cpp"),
            Object(Matching, "Game/HintRing.cpp"),
            Object(Matching, "Game/InitAction.cpp"),
            Object(NonMatching, "Game/ItemIconManagerTask.cpp"),
            Object(NonMatching, "Game/JumpBoard.cpp"),
            Object(Matching, "Game/KeyCounter.cpp"),
            Object(NonMatching, "Game/LongSpring.cpp"),
            Object(Matching, "Game/Magma.cpp"),
            Object(Matching, "Game/ManagerImpGc.cpp"),
            Object(NonMatching, "Game/MemoryCardCrypt.cpp"),
            Object(NonMatching, "Game/MemoryCardCtrl.cpp"),
            Object(Matching, "Game/MaterialBlink.cpp"),
            Object(Matching, "Game/MessageTask.cpp"),
            Object(Matching, "Game/Meteor.cpp"),
            Object(Matching, "Game/MeteorsHolder.cpp"),
            Object(NonMatching, "Game/Mission.cpp"),
            Object(Matching, "Game/MissionClearCollision.cpp"),
            Object(Matching, "Game/MissionMessage.cpp"),
            Object(Matching, "Game/Monster.cpp"),
            Object(Matching, "Game/MonsterControl.cpp"),
            Object(NonMatching, "Game/NowLoading.cpp"),
            Object(Matching, "Game/NpcBeeFeather.cpp"),
            Object(Matching, "Game/NpcEggMonitorMotion.cpp"),
            Object(Matching, "Game/NpcHint.cpp"),
            Object(Matching, "Game/NpcMotion.cpp"),
            Object(Matching, "Game/NpcNameDisplay.cpp"),
            Object(Matching, "Game/NpcTailsTail.cpp"),
            Object(Matching, "Game/NumberDisplayHolder.cpp"),
            Object(Matching, "Game/PJSCameraTargetInfo.cpp"),
            Object(NonMatching, "Game/PadButtons.cpp"),
            Object(Matching, "Game/Parachute.cpp"),
            Object(Matching, "Game/PauseAction.cpp"),
            Object(Matching, "Game/PjsEventActionFade.cpp"),
            Object(NonMatching, "Game/PjsEventActionManager.cpp"),
            Object(NonMatching, "Game/PjsEventActionScene.cpp"),
            Object(Matching, "Game/PjsEventFolder.cpp"),
            Object(Matching, "Game/PlayAction.cpp"),
            Object(Matching, "Game/PlayerBarrier.cpp"),
            Object(Matching, "Game/PlayerBarrierHeatEffect.cpp"),
            Object(Matching, "Game/PlayerBarrierMagnetEffect.cpp"),
            Object(Matching, "Game/PlayerBehaviorFirstMeet.cpp"),
            Object(Matching, "Game/PlayerBehaviorSuper.cpp"),
            Object(Matching, "Game/PlayerNpcSound.cpp"),
            Object(Matching, "Game/PlayerNpcTarget.cpp"),
            Object(NonMatching, "Game/PlayerPostManager.cpp"),
            Object(Matching, "Game/PlayerShadowMotionSub.cpp"),
            Object(Matching, "Game/PlayerShadowSound.cpp"),
            Object(Matching, "Game/PlayerShadowSuper.cpp"),
            Object(NonMatching, "Game/PlayerShadowWeapon.cpp"),
            Object(Matching, "Game/PlayerShadowWeaponCannon.cpp"),
            Object(Matching, "Game/PlayerShadowWeaponGun.cpp"),
            Object(Matching, "Game/PlayerShadowWeaponLockon.cpp"),
            Object(Matching, "Game/PlayerShadowWeaponStick.cpp"),
            Object(Matching, "Game/PlayerShadowWeaponVacuum.cpp"),
            Object(Matching, "Game/PlayerSoundBase.cpp"),
            Object(Matching, "Game/PlayerTargetBase.cpp"),
            Object(Matching, "Game/PlayRecord.cpp"),
            Object(Matching, "Game/PoisonGas.cpp"),
            Object(NonMatching, "Game/ProgressiveModeCheck.cpp"),
            Object(Matching, "Game/RailSwitch.cpp"),
            Object(NonMatching, "Game/RegularRing.cpp"),
            Object(Matching, "Game/Researcher.cpp"),
            Object(Matching, "Game/RestartAction.cpp"),
            Object(NonMatching, "Game/Result2PAction.cpp"),
            Object(NonMatching, "Game/ResultTask.cpp"),
            Object(Matching, "Game/RingMission.cpp"),
            Object(Matching, "Game/Rocket.cpp"),
            Object(Matching, "Game/Sample.cpp"),
            Object(Matching, "Game/SaveAction.cpp"),
            Object(NonMatching, "Game/SaveCycle.cpp"),
            Object(Matching, "Game/SaveCycleCheck.cpp"),
            Object(Matching, "Game/SaveCycleCheckModule.cpp"),
            Object(Matching, "Game/SaveCycleLoad.cpp"),
            Object(NonMatching, "Game/SaveCycleMessage.cpp"),
            Object(Matching, "Game/SaveCycleSave.cpp"),
            Object(Matching, "Game/SavePoint.cpp"),
            Object(NonMatching, "Game/SavePointSelect.cpp"),
            Object(NonMatching, "Game/SaveRecord.cpp"),
            Object(NonMatching, "Game/ScatteringRing.cpp"),
            Object(Matching, "Game/ScatteringWeapon.cpp"),
            Object(Matching, "Game/ScoreData.cpp"),
            Object(Matching, "Game/ScreenViewport.cpp"),
            Object(Matching, "Game/SecretDisk.cpp"),
            Object(Matching, "Game/SecretDoor.cpp"),
            Object(Matching, "Game/SecretKey.cpp"),
            Object(Matching, "Game/SetCharacter.cpp"),
            Object(Matching, "Game/SetCollision.cpp"),
            Object(Matching, "Game/SetGenerator.cpp"),
            Object(Matching, "Game/SetHintCollision.cpp"),
            Object(Matching, "Game/SetItem.cpp"),
            Object(Matching, "Game/SetParticle.cpp"),
            Object(NonMatching, "Game/SetSeLoop.cpp"),
            Object(Matching, "Game/SetSeOneShot.cpp"),
            Object(Matching, "Game/SetVehicle.cpp"),
            Object(Matching, "Game/SetWeapon.cpp"),
            Object(Matching, "Game/ShowAction.cpp"),
            Object(Matching, "Game/ShootingGameManager.cpp"),
            Object(Matching, "Game/ShootingGameScore.cpp"),
            Object(Matching, "Game/SoldierCommonBase.cpp"),
            Object(Matching, "Game/SoldierCommonAI.cpp"),
            Object(Matching, "Game/SonicTypeBehaviorFactory.cpp"),
            Object(Matching, "Game/SonicTypeControlFactory.cpp"),
            Object(Matching, "Game/SpaceGadget.cpp"),
            Object(NonMatching, "Game/Spring.cpp"),
            Object(NonMatching, "Game/SpringPanel.cpp"),
            Object(Matching, "Game/StageActionManager.cpp"),
            Object(Matching, "Game/StageBase.cpp"),
            Object(Matching, "Game/StageInitalizeTask.cpp"),
            Object(NonMatching, "Game/StageManager.cpp"),
            Object(Matching, "Game/StageRecord.cpp"),
            Object(NonMatching, "Game/StageRouteDisplay.cpp"),
            Object(Matching, "Game/StageMovie.cpp"),
            Object(Matching, "Game/StageSequenceBase.cpp"),
            Object(Matching, "Game/StageSequenceManager.cpp"),
            Object(Matching, "Game/StageSequenceMovieTest.cpp"),
            Object(Matching, "Game/StageSequencePlay2POnce.cpp"),
            Object(Matching, "Game/StageSequencePlay2PThree.cpp"),
            Object(Matching, "Game/StageSequencePlayEvent.cpp"),
            Object(NonMatching, "Game/StageSequencePlayFlashback.cpp"),
            Object(Matching, "Game/StageSequencePlayHard.cpp"),
            Object(Matching, "Game/StageSequencePlayOnce.cpp"),
            Object(Matching, "Game/StageSequencePlayStory.cpp"),
            Object(Matching, "Game/StageStaffroll.cpp"),
            Object(Matching, "Game/StageTable.cpp"),
            Object(NonMatching, "Game/StageTitleTask.cpp"),
            Object(Matching, "Game/StageTimer.cpp"),
            Object(Matching, "Game/Stick.cpp"),
            Object(NonMatching, "Game/StretchGrass.cpp"),
            Object(Matching, "Game/SuctionPillar.cpp"),
            Object(Matching, "Game/SweepMove.cpp"),
            Object(Matching, "Game/Switch.cpp"),
            Object(Matching, "Game/SystemSettings.cpp"),
            Object(Matching, "Game/TestNakazawa.cpp"),
            Object(Matching, "Game/TextureRasterCache.cpp"),
            Object(Matching, "Game/TFogSet.cpp"),
            Object(NonMatching, "Game/TGiCommandWindow.cpp"),
            Object(Matching, "Game/TGiRingChallenge.cpp"),
            Object(Matching, "Game/TGiScoreDisplay.cpp"),
            Object(Matching, "Game/TargetSwitch.cpp"),
            Object(Matching, "Game/Thread.cpp"),
            Object(Matching, "Game/ThreatObjHolder.cpp"),
            Object(NonMatching, "Game/ThrowBomb.cpp"),
            Object(Matching, "Game/ThrowContainer.cpp"),
            Object(Matching, "Game/ThrowObject.cpp"),
            Object(Matching, "Game/ThrowVacuumBomb.cpp"),
            Object(Matching, "Game/TimerCountMission.cpp"),
            Object(Matching, "Game/TimerGoalMission.cpp"),
            Object(Matching, "Game/TimerMessage.cpp"),
            Object(Matching, "Game/TimerMission.cpp"),
            Object(Matching, "Game/TimeupAction.cpp"),
            Object(NonMatching, "Game/TObjBGM.cpp"),
            Object(Matching, "Game/TrigerCounter.cpp"),
            Object(Matching, "Game/TryAgainAction.cpp"),
            Object(Matching, "Game/TurnOver.cpp"),
            Object(Matching, "Game/UpDownReel.cpp"),
            Object(Matching, "Game/VacuumBomb.cpp"),
            Object(Matching, "Game/VacuumContainer.cpp"),
            Object(Matching, "Game/VacuumObject.cpp"),
            Object(Matching, "Game/VacuumRing.cpp"),
            Object(Matching, "Game/VehicleArrow.cpp"),
            Object(Matching, "Game/VehicleAirBolt.cpp"),
            Object(Matching, "Game/VehicleCharaColliMove.cpp"),
            Object(NonMatching, "Game/VehicleColliMove.cpp"),
            Object(Matching, "Game/VehicleEffect.cpp"),
            Object(NonMatching, "Game/VehicleGun.cpp"),
            Object(NonMatching, "Game/VehiclePathMove.cpp"),
            Object(Matching, "Game/Vine.cpp"),
            Object(NonMatching, "Game/VibrationManagerTask.cpp"),
            Object(Matching, "Game/VibrationManagerUnit.cpp"),
            Object(Matching, "Game/VibrationPattern.cpp"),
            Object(Matching, "Game/VibrationRequest.cpp"),
            Object(Matching, "Game/VibrationServer.cpp"),
            Object(Matching, "Game/VibrationServerGC.cpp"),
            Object(Matching, "Game/WaitInitialize.cpp"),
            Object(Matching, "Game/WarpHole.cpp"),
            Object(Matching, "Game/WatchCamera.cpp"),
            Object(Matching, "Game/WeaponAirBolt.cpp"),
            Object(Matching, "Game/WeaponChaosSpear.cpp"),
            Object(Matching, "Game/WeaponDarkHammer.cpp"),
            Object(Matching, "Game/WeaponHeavyShot.cpp"),
            Object(Matching, "Game/WeaponLaserRifle.cpp"),
            Object(Matching, "Game/WeaponReflector.cpp"),
            Object(Matching, "Game/WeaponSpecial.cpp"),
            Object(Matching, "Game/WeaponSpecialHealCannon.cpp"),
            Object(Matching, "Game/WeaponSpecialKatana.cpp"),
            Object(Matching, "Game/WeaponSpecialOmochaoGun.cpp"),
            Object(Matching, "Game/WeaponSpecialSatelliteLaser.cpp"),
            Object(Matching, "Game/WeaponSpecialShadowRifle.cpp"),
            Object(Matching, "Game/WeaponSpecialVacuumEgg.cpp"),
            Object(Matching, "Game/WeaponSplitter.cpp"),
            Object(Matching, "Game/WeaponTorch.cpp"),
            Object(Matching, "Game/WeaponVacuumBase.cpp"),
            Object(Matching, "Game/WeaponVacuumPod.cpp"),
            Object(Matching, "Game/WeaponWormShooter.cpp"),
            Object(Matching, "Game/Weight.cpp"),
        ],
    },
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information, then refresh docs/progress.html
    calculate_progress(config)
    from tools.progress_page import write_page_for_build

    write_page_for_build(config.version)
else:
    sys.exit("Unknown mode: " + args.mode)
