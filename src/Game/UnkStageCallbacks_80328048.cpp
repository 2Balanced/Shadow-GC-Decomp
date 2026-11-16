/* The stage callbacks of five stages (StageTable.cpp): EggmansBase (0x25A), DoomsBase1 (0x259),
 * Space (0x1F7), ARKPast2 (0x1F8) and PrisonIsland (0x12D). Each stage has four (load, two more,
 * unload), and all twenty do nothing. No RTTI, string or data names this unit and neither
 * neighbour (Game/DevilDoomCtrlUnitBit.cpp, Game/ClumpFrameEffects.cpp) refers to it, so it is a
 * unit of its own, named after its first function like Game/UnkStageCallbacks_80286D4C.cpp.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). Names are the fn_ names StageTable.cpp refers to. */
#include <dolphin/types.h>

/* EggmansBase. */
extern "C" bool fn_803280E0(void) {
    return true;
}

extern "C" bool fn_803280D8(void) {
    return true;
}

extern "C" bool fn_803280D0(void) {
    return true;
}

extern "C" bool fn_803280C8(void) {
    return true;
}

/* DoomsBase1. */
extern "C" bool fn_803280C0(void) {
    return true;
}

extern "C" bool fn_803280B8(void) {
    return true;
}

extern "C" bool fn_803280B0(void) {
    return true;
}

extern "C" bool fn_803280A8(void) {
    return true;
}

/* Space. */
extern "C" bool fn_803280A0(void) {
    return true;
}

extern "C" bool fn_80328098(void) {
    return true;
}

extern "C" bool fn_80328090(void) {
    return true;
}

extern "C" bool fn_80328088(void) {
    return true;
}

/* ARKPast2. */
extern "C" bool fn_80328080(void) {
    return true;
}

extern "C" bool fn_80328078(void) {
    return true;
}

extern "C" bool fn_80328070(void) {
    return true;
}

extern "C" bool fn_80328068(void) {
    return true;
}

/* PrisonIsland. */
extern "C" bool fn_80328060(void) {
    return true;
}

extern "C" bool fn_80328058(void) {
    return true;
}

extern "C" bool fn_80328050(void) {
    return true;
}

extern "C" bool fn_80328048(void) {
    return true;
}
