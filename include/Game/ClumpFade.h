#ifndef GAME_CLUMPFADE_H
#define GAME_CLUMPFADE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* A colour as one object (four floats, red to alpha; inferred). Copies of it are word copies,
 * as retail's (fn_80027D5C). */
struct UnkFadeColor_80027D5C {
    float m_rgba[4];
};

/* A clump's colour fade (struct name unknown; as in Game/BlackDoomCtrlUnit.cpp and
 * Game/BossRender.cpp, which declare it locally). Its constructor is the out-of-line C function
 * fn_80027D5C (Game/EffectObjectUSA.cpp, where retail has it), which other units call by that
 * name; fn_80038198 applies the fade to a clump. Member names are inferred. */
struct UnkClumpFade_80027D5C;
extern "C" UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
struct UnkClumpFade_80027D5C {
    UnkClumpFade_80027D5C() {
        fn_80027D5C(this);
    }

    /* 0x00 */ float m_color[4]; /* red, green, blue, alpha (inferred) */
    /* 0x10 */ float field_0x10[2];
    /* 0x18 */ float field_0x18[3];
    /* 0x24 */ u8 m_mode;
    /* 0x28 */ float field_0x28;
};
SIZE_ASSERT(UnkClumpFade_80027D5C, 0x2C);

#endif
