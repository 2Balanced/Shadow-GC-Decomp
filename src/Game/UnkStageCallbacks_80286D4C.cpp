/* The stage callbacks of three stages (StageTable.cpp): BossLast (0x2C6), eWorld (0x193) and
 * Circuit (0xC8). Each stage has four: load (loads the stage's character file and its effects),
 * two that do nothing, and unload. No RTTI or string names this unit; the file is named after its
 * first function. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). Names are the fn_ names StageTable.cpp refers to. */
#include "Game/CharacterFiles.h"
#include <dolphin/types.h>

extern "C" {
/* Loads and unloads the boss's effect files (see Game/DiablonEffect.cpp). */
void fn_801D2DE8(int arg0);
void fn_801D2D14(int arg0);
/* Sets up and drops the ground effects of a stage kind (see Game/GroundEffectTracker.cpp). */
void fn_800A2730(int index, s32 start, s32 effect, s32 arg3, s32 sound);
void fn_800A26F0(int index);
}

/* Circuit. */
extern "C" bool fn_80286E90(void) {
    fn_80078C88()->Load(0xD);
    fn_800A2730(0, -1, 0x1A6, -1, 0x7010);
    return true;
}

extern "C" bool fn_80286E88(void) {
    return true;
}

extern "C" bool fn_80286E80(void) {
    return true;
}

extern "C" bool fn_80286E4C(void) {
    fn_80078C88()->Unload(0xD);
    fn_800A26F0(0);
    return true;
}

/* eWorld. */
extern "C" bool fn_80286E08(void) {
    fn_80078C88()->Load(0xD);
    fn_800A2730(0, -1, 0x1A6, -1, 0x7010);
    return true;
}

extern "C" bool fn_80286E00(void) {
    return true;
}

extern "C" bool fn_80286DF8(void) {
    return true;
}

extern "C" bool fn_80286DC4(void) {
    fn_80078C88()->Unload(0xD);
    fn_800A26F0(0);
    return true;
}

/* BossLast. */
extern "C" bool fn_80286D90(void) {
    fn_80078C88()->Load(0xE);
    fn_801D2DE8(2);
    return true;
}

extern "C" bool fn_80286D88(void) {
    return true;
}

extern "C" bool fn_80286D80(void) {
    return true;
}

extern "C" bool fn_80286D4C(void) {
    fn_80078C88()->Unload(0xE);
    fn_801D2D14(2);
    return true;
}
