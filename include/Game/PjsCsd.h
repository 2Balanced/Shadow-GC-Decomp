#ifndef GAME_PJSCSD_H
#define GAME_PJSCSD_H

#include <dolphin/types.h>
#include "size_assert.h"

/* TPjsCsd (class name from RTTI; vtable 0x8051CF00): a CSD project a task draws, loaded from a
 * file's data. Its constructor is at 0x8003FFE4 and its destructor at 0x8003FF84; the other
 * functions keep their fn_ names (they take the project by reference: a derived task passes
 * itself without a null test). The vtable pointer follows the first member, because the first
 * virtual function is declared after it. Member names are inferred. */
class TPjsCsd {
  public:
    TPjsCsd();

    /* 0x00 */ u32 field_0x0;

    virtual ~TPjsCsd();

    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ char m_sceneName[0x50 - 0x10]; /* the scene to play */
    /* 0x50 */ u8 m_playing; /* set by fn_8003FE2C while the scene runs (meaning inferred) */
    /* 0x51 */ u8 field_0x51[0x54 - 0x51];
};
SIZE_ASSERT(TPjsCsd, 0x54);

extern "C" {
/* Loads the project from data; false when it could not (behaviour inferred). */
bool fn_8003E750(TPjsCsd &csd, void *data, u32 size);
/* Starts the scene in m_sceneName (behaviour inferred; the arguments' meanings are unknown). */
void fn_8003E414(TPjsCsd &csd, int arg1, int arg2);
void fn_8003FB10(TPjsCsd &csd);
/* Draws and advances the project (behaviour inferred). */
void fn_8003FDF4(TPjsCsd &csd);
void fn_8003FE2C(TPjsCsd &csd);
}

#endif
