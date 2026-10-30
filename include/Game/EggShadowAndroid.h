#ifndef GAME_EGGSHADOWANDROID_H
#define GAME_EGGSHADOWANDROID_H

/* EggShadowAndroid (namespace and class names from RTTI): the Shadow android, EGG_SHADOWANDROID
 * in the stage editor (Game/EggShadowAndroid.cpp). Its body is EggShadowAndroid::EggShadowAndroid,
 * an EnemyWalkerCommon (Game/Enemy.h) that stands or appears at an offset (AppearType), jumps,
 * homes in and fires lock-on missiles, and its AI EggShadowAndroid::EggShadowAndroidAI
 * (Game/EggShadowAndroidAI.cpp); EggShadowAndroid::EggShadowAndroidTask is
 * EnemyTemplate<EggShadowAndroid, EggShadowAndroidAI>. Member names are inferred unless they repeat
 * a parameter name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;
namespace Effect {
namespace System {
class EffectElement;
}
} // namespace Effect
namespace Weapon {
class LockonTarget;
}

extern "C" {
/* A 32-bit flag word (see Game/EggLastMech.h): construct it cleared, set, test and clear a bit. */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
}

namespace EggShadowAndroid {

class Belt;

/* The android's parameters (struct name unknown; its constructor is inline). Names repeat the
 * stage editor's. */
struct EggShadowAndroidParam {
    EggShadowAndroidParam()
        : MoveRange(200.0f), SearchRange(0.0f), SearchAngle(0.0f), SearchWidth(100.0f),
          SearchHeight(100.0f), SearchHeightOffset(0.0f), MoveSpeedRatio(1.0f), AppearType(0),
          OffsetPos_X(0.0f), OffsetPos_Y(0.0f), OffsetPos_Z(0.0f) {}

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 AppearType; /* 0 STAND, 1 OFFSET */
    /* 0x20 */ float OffsetPos_X;
    /* 0x24 */ float OffsetPos_Y;
    /* 0x28 */ float OffsetPos_Z;
};
SIZE_ASSERT(EggShadowAndroidParam, 0x2C);

/* A flag word (struct name unknown). */
struct EggShadowAndroidFlags {
    EggShadowAndroidFlags() {
        fn_80014E04(&m_bits);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }

    /* 0x0 */ u32 m_bits;
};

/* EggShadowAndroid::EggShadowAndroid (class name from RTTI; vtable 0x805536A4): the android's
 * body. The overrides are declared in the order of the slots they add. */
class EggShadowAndroid : public EnemyWalkerCommon {
  public:
    EggShadowAndroid();
    virtual ~EggShadowAndroid();
    virtual void vfDisp_0x10(float dt);
    virtual void vfStat_0x38();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual int vfStat_0xB8();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
    virtual void vfStat_0x1C();
    virtual void vfDisp_0x40(EnemyMotionEventArg &arg);
    virtual bool vfDisp_0x44(int *command);
    virtual float vfPara_0x6C();
    virtual int vfPara_0x48();
    virtual int vfPara_0x4C();
    virtual int vfPara_0x50();
    virtual int vfPara_0x54();
    virtual int vfPara_0x58();
    virtual int vfPara_0x5C();
    virtual int vfPara_0x60();
    virtual float vfPara_0x68();
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);

    /* Called by the AI (names inferred). */
    void Fall();
    u8 WasHit();
    void StopJetEffect();
    void StartJetEffect();
    void StopBoostEffect();
    void StartBoostEffect();
    void DeleteBelt();
    void CreateBelt();
    void ClearLockon();
    /* Called by the motion events (names inferred). */
    void FireMissile();
    void Lockon();

    /* 0x270 */ EggShadowAndroidParam m_params;
    /* 0x29C */ u32 m_tube;          /* the launcher the next missile leaves from (0 to 7) */
    /* 0x2A0 */ s32 m_attackWait;    /* the attack decisions left until the next attack */
    /* 0x2A4 */ Belt *m_belt;
    /* 0x2A8 */ u8 m_hit;            /* hit by an attack with flag 8 */
    /* 0x2AC */ EggShadowAndroidFlags m_moveFlags;
    /* 0x2B0 */ EggShadowAndroidFlags m_guardFlags;
    /* 0x2B4 */ boost::weak_ptr<Effect::System::EffectElement> m_jetEffect;
    /* 0x2BC */ boost::weak_ptr<Effect::System::EffectElement> m_boostEffect;
    /* 0x2C4 */ Mtx34f m_jetMatrix;
    /* 0x2F4 */ Mtx34f m_boostMatrix;
    /* 0x324 */ u8 m_noGround;       /* in one of the motions 5 to 9 (meaning inferred) */
    /* 0x328 */ boost::shared_ptr<Weapon::LockonTarget> m_lockon;
    /* 0x330: EnemyReferer (virtual base; offset from the RTTI's base table) */
};
SIZE_ASSERT(EggShadowAndroid, 0x338);

/* EggShadowAndroid::EggShadowAndroidAI (class name from RTTI; vtable 0x80553C3C; constructor
 * 0x8029F218, destructor 0x8029F1A8; Game/EggShadowAndroidAI.cpp): the android's AI. */
class EggShadowAndroidAI : public EnemyBaseAI {
  public:
    EggShadowAndroidAI();
    virtual ~EggShadowAndroidAI();
    /* Defined under its symbol in Game/EggShadowAndroidAI.cpp: it takes the message (see
     * EnemyBaseAI::vfAI_0x28). */
    virtual void vfAI_0x28();
    virtual void vfAI_0x38();
    virtual void vfAI_0x3C();
    virtual void vfAI_0x48();

    /* 0x74 */ EggShadowAndroid *m_body;
    /* 0x78 */ boost::weak_ptr<CharInfo> m_attacker;
};

} // namespace EggShadowAndroid

#endif
