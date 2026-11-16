/* The stage callbacks of two stages (StageTable.cpp): Practice (0) and canyon2 (0x192). Each stage
 * has four: load, two that do nothing, and unload. Practice loads two character files and the
 * boss's effect files; canyon2's do nothing. No RTTI, string or data names this unit and neither
 * neighbour (Game/TFogSet.cpp, Game/Mission.cpp) refers to it, so it is a unit of its own, named
 * after its first function like Game/UnkStageCallbacks_80286D4C.cpp. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). Names are the fn_ names
 * StageTable.cpp refers to. */
#include "Game/CharacterFiles.h"
#include <dolphin/types.h>

extern "C" {
/* Loads and unloads the boss's effect files (see Game/DiablonEffect.cpp). */
void fn_801D2DE8(int arg0);
void fn_801D2D14(int arg0);
}

/* canyon2. */
extern "C" bool fn_8016AA88(void) {
    return true;
}

extern "C" bool fn_8016AA80(void) {
    return true;
}

extern "C" bool fn_8016AA78(void) {
    return true;
}

extern "C" bool fn_8016AA70(void) {
    return true;
}

/* Practice. */
extern "C" bool fn_8016AA30(void) {
    fn_80078C88()->Load(0xE);
    fn_801D2DE8(2);
    fn_80078C88()->Load(0xD);
    return true;
}

extern "C" bool fn_8016AA28(void) {
    return true;
}

extern "C" bool fn_8016AA20(void) {
    return true;
}

extern "C" bool fn_8016A9E0(void) {
    fn_80078C88()->Unload(0xE);
    fn_801D2D14(2);
    fn_80078C88()->Unload(0xD);
    return true;
}
