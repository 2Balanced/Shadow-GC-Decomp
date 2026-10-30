#ifndef GAME_GUNBIGFOOTAI_H
#define GAME_GUNBIGFOOTAI_H

/* GunBigfoot (namespace and class names from RTTI): the G.U.N. Bigfoot walker. Its AI is
 * GunBigfootAI (Game/GunBigfootAI.cpp); its body is GunBigfoot::GunBigfoot (Game/GunBigfoot.h).
 * Member names are inferred. */

#include "Game/EnemyAI.h"
#include "Game/GunBigfoot.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace GunBigfoot {

/* GunBigfootAI (class name from RTTI; vtable 0x8052F70C; constructor 0x80185C84): the walker's
 * AI. It waits crouching, hovering (swaying by the body's float width) or standing until the
 * target comes into its area, then attacks it from the ground or, hovering, from the air. */
class GunBigfootAI : public EnemyBaseAI {
  public:
    GunBigfootAI();
    virtual ~GunBigfootAI();  /* 0x80185C28 */
    virtual void vfAI_0x38(); /* 0x801859FC */
    virtual void vfAI_0x3C(); /* 0x8018586C */
    virtual void vfAI_0x48(); /* 0x801856B8 */

    /* 0x74 */ GunBigfoot *m_body;
    /* 0x78 */ u8 m_rising; /* the hover sway moves up (as FlyerCommonAI's) */
    /* 0x7C */ float m_wave; /* the hover sway's phase */
};

} // namespace GunBigfoot

#endif
