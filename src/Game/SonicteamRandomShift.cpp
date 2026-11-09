/* A small random number generator over four words of shift-register state (Game/GadgetCrash3D.cpp
 * draws from one): seeding, and the next number in [0, 1). Other units call them by their fn_
 * names, so they keep them; the struct, file and member names are invented. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

struct UnkRandom_8040CB2C {
    /* 0x0 */ u32 m_state[4];
};
SIZE_ASSERT(UnkRandom_8040CB2C, 0x10);

extern "C" {
void fn_8040CB2C(UnkRandom_8040CB2C *random, u32 seed);
float fn_8040CB58(UnkRandom_8040CB2C *random);
}

float fn_8040CB58(UnkRandom_8040CB2C *random) {
    u32 first = random->m_state[0];
    u32 next = random->m_state[1];
    next <<= 1;
    if (first & 0x80000000) {

        next++;
    }
    next <<= 1;
    if (first & 0x40000000) {
        next++;
    }
    random->m_state[3] <<= 1;
    if (random->m_state[2] & 0x80000000) {
        random->m_state[3]++;
    }
    next ^= random->m_state[3];
    random->m_state[3] = random->m_state[2];
    random->m_state[2] = random->m_state[1];
    random->m_state[1] = random->m_state[0];
    random->m_state[0] = next;
    return (1.0f / 2147483648.0f) * (next >> 1);
}

void fn_8040CB2C(UnkRandom_8040CB2C *random, u32 seed) {
    random->m_state[0] = seed;
    random->m_state[1] = seed ^ 0x3CC19185;
    random->m_state[2] = seed ^ 0x01C9C2D0;
    random->m_state[3] = seed ^ 0x2E43C13E;
}
