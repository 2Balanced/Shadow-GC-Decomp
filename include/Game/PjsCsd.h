#ifndef GAME_PJSCSD_H
#define GAME_PJSCSD_H

#include <dolphin/types.h>
#include "size_assert.h"

/* TPjsCsd (class name from RTTI; vtable 0x8051CF00): a CSD project a task draws, loaded from a
 * file's data. Its constructor is at 0x8003FFE4 and its destructor at 0x8003FF84; the other
 * functions keep their fn_ names (they take the project by reference: a derived task passes
 * itself without a null test). The vtable pointer follows the first member, because the first
 * virtual function is declared after it. Member names are inferred. */
#ifdef TPJS_CSD_IMPL
/* (Where TPJS_CSD_IMPL is defined, Game/TPjsCsd.cpp, the unit with the class's code, the members
 * have their real types.) A queued motion request, allocated on the game heap: the motion's index,
 * or -1 and its name, and whether it loops. */
struct TPjsCsdRequest {
    /* 0x00 */ TPjsCsdRequest *m_next;
    /* 0x04 */ s32 m_index;
    /* 0x08 */ char m_motion[0x20];
    /* 0x28 */ bool m_loop;
};
#endif

class TPjsCsd {
  public:
    TPjsCsd();

#ifdef TPJS_CSD_IMPL
    /* 0x00 */ TPjsCsdRequest *m_requests;
#else
    /* 0x00 */ u32 field_0x0;
#endif

    virtual ~TPjsCsd();

#ifdef TPJS_CSD_IMPL
    /* 0x08 */ Chao::CSD::CProject *m_project;
    /* 0x0C */ Chao::CSD::RCPtr<Chao::CSD::CTexList> m_texList;
#else
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
#endif
    /* 0x10 */ char m_sceneName[0x50 - 0x10]; /* the scene to play */
    /* 0x50 */ u8 m_playing; /* set by fn_8003FE2C while the scene runs (meaning inferred) */
    /* 0x51 */ u8 field_0x51[0x54 - 0x51];
};
SIZE_ASSERT(TPjsCsd, 0x54);

#ifndef TPJS_CSD_IMPL
/* (Game/TPjsCsd.cpp declares them itself with its own parameter types.) */
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

#endif
