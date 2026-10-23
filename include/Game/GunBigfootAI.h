#ifndef GAME_GUNBIGFOOTAI_H
#define GAME_GUNBIGFOOTAI_H

/* GunBigfoot (namespace and class names from RTTI): the G.U.N. Bigfoot walker. Its AI is
 * GunBigfootAI (Game/GunBigfootAI.cpp); its body is GunBigfoot::GunBigfoot, an EnemyWalkerCommon
 * (Game/Enemy.h) whose code is around 0x80286D4C (vtable 0x8054F368). The body's unit is not done
 * yet, so the body is declared here only as far as its AI uses it; it belongs in the body's own
 * header once that unit is matched. Member names are inferred. */

#include "Game/EnemyAI.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace GunBigfoot {

/* GunBigfoot::GunBigfoot (class name from RTTI): the walker's body. */
class GunBigfoot : public EnemyWalkerCommon {
  public:
    virtual ~GunBigfoot(); /* 0x80288004 */
    /* The body's own slots, 0x238 to 0x28C (0x80287D78 ...); only vf_0x278 is called from
     * outside so far, the others are placeholders whose signatures are not known. */
    virtual void vf_0x238();
    virtual void vf_0x23C();
    virtual void vf_0x240();
    virtual void vf_0x244();
    virtual void vf_0x248();
    virtual void vf_0x24C();
    virtual void vf_0x250();
    virtual void vf_0x254();
    virtual void vf_0x258();
    virtual void vf_0x25C();
    virtual void vf_0x260();
    virtual void vf_0x264();
    virtual void vf_0x268();
    virtual void vf_0x26C();
    virtual void vf_0x270();
    virtual void vf_0x274();
    /* The motion of an attack from the ground (0x8028742C: motion 7). */
    virtual int vf_0x278();

    /* Sets the point the walker aims at (weak copy first at 0x80184C20). */
    void SetAimPoint(const Vec3f &point) {
        AssignVector(&m_aimPoint, &point);
    }

    /* 0x270 */ u8 field_0x270[0x28C - 0x270];
    /* 0x28C */ s32 m_appearType; /* the AI's start state: 0 crouching, 1 hovering until the
                                   * target comes (then it lands), 2 walking home, 3 waiting to act,
                                   * 4 hovering */
    /* 0x290 */ u8 field_0x290[0x294 - 0x290];
    /* 0x294 */ float m_floatWidth; /* how far it sways while hovering */
    /* 0x298 */ u8 field_0x298[0x29C - 0x298];
    /* 0x29C */ u32 m_flags;        /* fn_802875C8 clears all but bit 2 */
    /* 0x2A0 */ u8 m_hovering;
    /* 0x2A1 */ u8 field_0x2A1;
    /* 0x2A2 */ u8 m_aiming;        /* the AI is aiming at m_aimPoint */
    /* 0x2A3 */ u8 field_0x2A3[0x2B0 - 0x2A3];
    /* 0x2B0 */ Vec3f m_aimPoint;
    /* 0x2BC */ u8 field_0x2BC[0x2D8 - 0x2BC];
    /* 0x2D8: EnemyReferer (virtual base; offset from the RTTI's base table) */
};
SIZE_ASSERT(GunBigfoot, 0x2E0);

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
