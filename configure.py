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
            Object(Matching, "Runtime.PPCEABI.H/__init_cpp_exceptions.cpp"),
        ],
    },
    {
        # Gadget units. They have no splits yet, so none is linked.
        "lib": "Game",
        "mw_version": "GC/2.7",
        "cflags": cflags_game_cpp,
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "Game/AdvMemoryCheck.cpp"),
            Object(NonMatching, "Game/ArkCannon.cpp"),
            Object(Matching, "Game/ArkLaser.cpp"),
            Object(Matching, "Game/BAGunShip.cpp"),
            Object(Matching, "Game/BAMine.cpp"),
            Object(Matching, "Game/BAShield.cpp"),
            Object(Matching, "Game/BASwitch.cpp"),
            Object(Matching, "Game/BDHologram.cpp"),
            Object(Matching, "Game/BackDrop.cpp"),
            Object(NonMatching, "Game/BalloonGenerator.cpp"),
            Object(Matching, "Game/BDTrue.cpp"),
            Object(Matching, "Game/BlackBull0412CtrlUnit.cpp"),
            Object(Matching, "Game/BKTank.cpp"),
            Object(Matching, "Game/BKTankColi.cpp"),
            Object(Matching, "Game/BKTankController.cpp"),
            Object(Matching, "Game/BlastBreak.cpp"),
            Object(Matching, "Game/BlockChange.cpp"),
            Object(Matching, "Game/Bomb.cpp"),
            Object(Matching, "Game/BombingWall.cpp"),
            Object(Matching, "Game/BombServer.cpp"),
            Object(Matching, "Game/BossCameraLamborghini.cpp"),
            Object(Matching, "Game/BossDesire.cpp"),
            Object(Matching, "Game/BreakObj.cpp"),
            Object(Matching, "Game/BreakRoad.cpp"),
            Object(Matching, "Game/BreakWall.cpp"),
            Object(NonMatching, "Game/BulletLaser.cpp"),
            Object(Matching, "Game/Cage.cpp"),
            Object(Matching, "Game/CameraModeFixLookChar.cpp"),
            Object(Matching, "Game/CandleStick.cpp"),
            Object(Matching, "Game/CaptureCage.cpp"),
            Object(NonMatching, "Game/Catapult.cpp"),
            Object(Matching, "Game/CatchStick.cpp"),
            Object(Matching, "Game/CenFile.cpp"),
            Object(Matching, "Game/CenFileNumber.cpp"),
            Object(Matching, "Game/ChaoHolder.cpp"),
            Object(NonMatching, "Game/CircusBall.cpp"),
            Object(Matching, "Game/CircusPole.cpp"),
            Object(Matching, "Game/CityBombBig.cpp"),
            Object(Matching, "Game/CityBombSmall.cpp"),
            Object(Matching, "Game/CityGadget.cpp"),
            Object(Matching, "Game/ClumpFrameEffects.cpp"),
            Object(NonMatching, "Game/Coaster.cpp"),
            Object(Matching, "Game/ContainerBehavior.cpp"),
            Object(Matching, "Game/ContainerHard.cpp"),
            Object(Matching, "Game/ContainerNormal.cpp"),
            Object(Matching, "Game/ContainerUnbreak.cpp"),
            Object(Matching, "Game/ContainerWeapon.cpp"),
            Object(Matching, "Game/Cream.cpp"),
            Object(Matching, "Game/DamageBlock.cpp"),
            Object(Matching, "Game/DarkSpinEntrance.cpp"),
            Object(NonMatching, "Game/DashPanel.cpp"),
            Object(NonMatching, "Game/DashRing.cpp"),
            Object(Matching, "Game/DefenseShield.cpp"),
            Object(Matching, "Game/DefenseSystem.cpp"),
            Object(Matching, "Game/DevilDoom.cpp"),
            Object(NonMatching, "Game/DevilDoomModeCtrlUnit.cpp"),
            Object(NonMatching, "Game/DevilDoomCtrlUnit.cpp"),
            Object(Matching, "Game/DevilDoomCtrlUnitBit.cpp"),
            Object(NonMatching, "Game/DevilDoomCtrlUnitPsychoObject.cpp"),
            Object(Matching, "Game/DiablonBehaviorMove.cpp"),
            Object(Matching, "Game/DiablonDesireMove.cpp"),
            Object(Matching, "Game/DiablonPartsBeam.cpp"),
            Object(Matching, "Game/DiablonPartsBomb.cpp"),
            Object(Matching, "Game/DiablonPartsCameraControl.cpp"),
            Object(Matching, "Game/DiscErrorHandler.cpp"),
            Object(Matching, "Game/Door.cpp"),
            Object(Matching, "Game/EffectGlobalParamUSA.cpp"),
            Object(Matching, "Game/EffectPJS.cpp"),
            Object(Matching, "Game/EggBalloon.cpp"),
            Object(Matching, "Game/EggBalloonColi.cpp"),
            Object(Matching, "Game/EggBalloonController.cpp"),
            Object(Matching, "Game/EggLastMech.cpp"),
            Object(NonMatching, "Game/EggLastMechAIState.cpp"),
            Object(NonMatching, "Game/EggLastMechArea.cpp"),
            Object(Matching, "Game/EggLastMechCamera.cpp"),
            Object(Matching, "Game/EggLastMechPart.cpp"),
            Object(NonMatching, "Game/EggLastMechSlot.cpp"),
            Object(Matching, "Game/EggLastMechSlotDisp2D.cpp"),
            Object(Matching, "Game/EggLastMechSlotSwitch.cpp"),
            Object(Matching, "Game/EggLastMechTask.cpp"),
            Object(Matching, "Game/ElecBar.cpp"),
            Object(Matching, "Game/ElecBarrier.cpp"),
            Object(Matching, "Game/ElecBlock.cpp"),
            Object(Matching, "Game/ElecBomb.cpp"),
            Object(Matching, "Game/ElecCircuitColli.cpp"),
            Object(Matching, "Game/ElecCircuitEntrance.cpp"),
            Object(Matching, "Game/ElecCoreProgram.cpp"),
            Object(Matching, "Game/ElecCoreWall.cpp"),
            Object(NonMatching, "Game/ElecCristalWall.cpp"),
            Object(Matching, "Game/ElecCristalWallSwitch.cpp"),
            Object(Matching, "Game/ElecFan.cpp"),
            Object(Matching, "Game/ElecFireWall.cpp"),
            Object(Matching, "Game/ElecKyuu.cpp"),
            Object(Matching, "Game/ElecRollHexa.cpp"),
            Object(Matching, "Game/ElecSearchLight.cpp"),
            Object(NonMatching, "Game/ElecSecurityCode.cpp"),
            Object(Matching, "Game/ElecSecurityCodeGenerator.cpp"),
            Object(Matching, "Game/ElecSmallCube.cpp"),
            Object(Matching, "Game/Elevator.cpp"),
            Object(Matching, "Game/EnemyHint.cpp"),
            Object(Matching, "Game/EnemyTarget.cpp"),
            Object(Matching, "Game/EnergyCore.cpp"),
            Object(Matching, "Game/FallRoad.cpp"),
            Object(Matching, "Game/Fan.cpp"),
            Object(Matching, "Game/Fire.cpp"),
            Object(Matching, "Game/FireRing.cpp"),
            Object(Matching, "Game/FogTable.cpp"),
            Object(Matching, "Game/Footing.cpp"),
            Object(Matching, "Game/FootingMovable.cpp"),
            Object(Matching, "Game/Fruit.cpp"),
            Object(Matching, "Game/GiantLantern.cpp"),
            Object(Matching, "Game/Gadget.cpp"),
            Object(NonMatching, "Game/GadgetCharCollision.cpp"),
            Object(Matching, "Game/GadgetCharInfo.cpp"),
            Object(Matching, "Game/GadgetColliDetectUtil.cpp"),
            Object(Matching, "Game/GadgetCommand.cpp"),
            Object(Matching, "Game/GadgetCommonData.cpp"),
            Object(Matching, "Game/GadgetCrash3D.cpp"),
            Object(Matching, "Game/GadgetParticle.cpp"),
            Object(Matching, "Game/GadgetPickup.cpp"),
            Object(NonMatching, "Game/GadgetStageData.cpp"),
            Object(Matching, "Game/GadgetSwitch.cpp"),
            Object(Matching, "Game/GadgetUtility.cpp"),
            Object(Matching, "Game/GoalRing.cpp"),
            Object(Matching, "Game/Gong.cpp"),
            Object(Matching, "Game/GunBattery.cpp"),
            Object(Matching, "Game/GunBuilding.cpp"),
            Object(Matching, "Game/GunLift.cpp"),
            Object(Matching, "Game/HealingUnit.cpp"),
            Object(Matching, "Game/HeavyDog0410CtrlUnit.cpp"),
            Object(Matching, "Game/HeavyDog0510CtrlUnit.cpp"),
            Object(NonMatching, "Game/Helicopter.cpp"),
            Object(Matching, "Game/HintRing.cpp"),
            Object(NonMatching, "Game/JumpBoard.cpp"),
            Object(NonMatching, "Game/LongSpring.cpp"),
            Object(Matching, "Game/Meteor.cpp"),
            Object(Matching, "Game/MeteorsHolder.cpp"),
            Object(Matching, "Game/MissionClearCollision.cpp"),
            Object(Matching, "Game/Monster.cpp"),
            Object(Matching, "Game/MonsterControl.cpp"),
            Object(Matching, "Game/NpcBeeFeather.cpp"),
            Object(Matching, "Game/NpcTailsTail.cpp"),
            Object(Matching, "Game/PjsEventActionFade.cpp"),
            Object(Matching, "Game/PlayerNpcSound.cpp"),
            Object(NonMatching, "Game/PlayerShadowWeapon.cpp"),
            Object(Matching, "Game/PlayerShadowWeaponCannon.cpp"),
            Object(NonMatching, "Game/PlayerShadowWeaponGun.cpp"),
            Object(Matching, "Game/PlayerShadowWeaponLockon.cpp"),
            Object(Matching, "Game/PlayerShadowWeaponStick.cpp"),
            Object(Matching, "Game/PlayerShadowWeaponVacuum.cpp"),
            Object(Matching, "Game/PoisonGas.cpp"),
            Object(NonMatching, "Game/ProgressiveModeCheck.cpp"),
            Object(NonMatching, "Game/RegularRing.cpp"),
            Object(Matching, "Game/Researcher.cpp"),
            Object(Matching, "Game/Rocket.cpp"),
            Object(Matching, "Game/SavePoint.cpp"),
            Object(NonMatching, "Game/SavePointSelect.cpp"),
            Object(NonMatching, "Game/ScatteringRing.cpp"),
            Object(Matching, "Game/ScatteringWeapon.cpp"),
            Object(Matching, "Game/ScreenViewport.cpp"),
            Object(Matching, "Game/SecretDisk.cpp"),
            Object(Matching, "Game/SecretDoor.cpp"),
            Object(Matching, "Game/SecretKey.cpp"),
            Object(Matching, "Game/SetCharacter.cpp"),
            Object(Matching, "Game/SetCollision.cpp"),
            Object(Matching, "Game/SetItem.cpp"),
            Object(Matching, "Game/SetParticle.cpp"),
            Object(Matching, "Game/SetVehicle.cpp"),
            Object(Matching, "Game/SetWeapon.cpp"),
            Object(Matching, "Game/ShootingGameManager.cpp"),
            Object(Matching, "Game/ShootingGameScore.cpp"),
            Object(Matching, "Game/SpaceGadget.cpp"),
            Object(NonMatching, "Game/Spring.cpp"),
            Object(NonMatching, "Game/SpringPanel.cpp"),
            Object(Matching, "Game/StageSequenceMovieTest.cpp"),
            Object(Matching, "Game/Stick.cpp"),
            Object(NonMatching, "Game/StretchGrass.cpp"),
            Object(Matching, "Game/SuctionPillar.cpp"),
            Object(Matching, "Game/Switch.cpp"),
            Object(Matching, "Game/SystemSettings.cpp"),
            Object(Matching, "Game/TFogSet.cpp"),
            Object(Matching, "Game/TargetSwitch.cpp"),
            Object(Matching, "Game/ThreatObjHolder.cpp"),
            Object(Matching, "Game/TrigerCounter.cpp"),
            Object(Matching, "Game/TurnOver.cpp"),
            Object(Matching, "Game/UpDownReel.cpp"),
            Object(Matching, "Game/VacuumObject.cpp"),
            Object(Matching, "Game/VacuumRing.cpp"),
            Object(NonMatching, "Game/VehicleColliMove.cpp"),
            Object(NonMatching, "Game/VehiclePathMove.cpp"),
            Object(Matching, "Game/Vine.cpp"),
            Object(Matching, "Game/WarpHole.cpp"),
            Object(Matching, "Game/WatchCamera.cpp"),
            Object(Matching, "Game/WeaponLaserRifle.cpp"),
            Object(Matching, "Game/WeaponReflector.cpp"),
            Object(Matching, "Game/WeaponSpecial.cpp"),
            Object(Matching, "Game/WeaponSpecialHealCannon.cpp"),
            Object(Matching, "Game/WeaponSpecialKatana.cpp"),
            Object(Matching, "Game/WeaponSplitter.cpp"),
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
