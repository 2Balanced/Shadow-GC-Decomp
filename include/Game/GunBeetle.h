#ifndef GAME_GUNBEETLE_H
#define GAME_GUNBEETLE_H

/* GunBeetle (class name from RTTI): the G.U.N. Beetle, GUN_BEETLE in the stage editor
 * (Game/GunBeetle.cpp). Its body is GunBeetleBase, a FlyerCommonBase (Game/EnemyFlyer.h), and its
 * AI GunBeetleAI; EnemyTemplate<GunBeetleBase, GunBeetleAI> (Game/Enemy.h) puts them together.
 * Member names are inferred unless they repeat a parameter name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/EnemyFlyer.h"
#include "Game/GadgetSound.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Effect {
namespace System {
class EffectElement;
}
} // namespace Effect

class GunBeetleGun;

/* The Beetle's parameters (struct name unknown; its constructor 0x801949C0 is inline). */
struct GunBeetleParam : public FlyerParam {
    GunBeetleParam() {
        FlyerParam::Reset();
        BodyType = 0;
        WeaponType = 0;
        SparkDischarge = 2.0f;
        SparkWait = 2.0f;
    }

    /* 0x38 */ s32 BodyType;
    /* 0x3C */ s32 WeaponType;
    /* 0x40 */ float SparkDischarge;
    /* 0x44 */ float SparkWait;
};
SIZE_ASSERT(GunBeetleParam, 0x48);

/* GunBeetleBase (class name from RTTI; vtable 0x80536C54): the Beetle's body. It may carry a gun
 * (WeaponType) and, with ActionType 1, discharges sparks: it waits SparkWait seconds, then sparks
 * for SparkDischarge seconds. */
class GunBeetleBase : public FlyerCommonBase {
  public:
    GunBeetleBase();
    virtual ~GunBeetleBase();
    virtual void vfStat_0x38();
    /* Whether AppearType is 1 (weak, first emitted with GunBeetleAI at 0x80184620). */
    virtual bool vf_0x238();
    virtual bool vf_0x23C();
    virtual void vf_0x244();
    virtual void vf_0x248();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual int vfStat_0xB8();
    virtual void vfStat_0xA0();
    virtual float vfPara_0x6C();
    virtual int vfPara_0x48();
    virtual int vfPara_0x4C();
    virtual int vfPara_0x50();
    virtual int vfPara_0x54();
    virtual int vfPara_0x58();
    virtual int vfPara_0x5C();
    virtual int vfPara_0x60();
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
    virtual void vfDisp_0x10(float dt);

    /* Enemy::field_0xEC, read out of line (name inferred). */
    float GetFieldEC();

    /* 0x310 */ GunBeetleParam m_params;
    /* 0x358 */ GunBeetleGun *m_gun;
    /* 0x35C */ boost::weak_ptr<Effect::System::EffectElement> m_spark;
    /* 0x364 */ s32 m_sparkState;
    /* 0x368 */ float m_sparkTimer;
    /* 0x36C */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x388 */ UnkSoundHandle_801D3DC8 m_sparkSound;
};

/* GunBeetleAI (class name from RTTI; vtable 0x8052F35C): the Beetle's AI. */
class GunBeetleAI : public FlyerCommonAI {
  public:
    GunBeetleAI();             /* 0x80184634 */
    virtual ~GunBeetleAI() {}  /* weak, first at 0x80184134 */
    virtual void vfAI_0x3C();  /* 0x80184560 */
    virtual void vfAI_0x44();  /* 0x80184528 */

    /* 0x8C */ GunBeetleBase *m_body;
};

/* GunBeetle (class name from RTTI; vtable 0x8053664C). */
class GunBeetle : public EnemyTemplate<GunBeetleBase, GunBeetleAI> {
  public:
    GunBeetle(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~GunBeetle();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(GunBeetle, 0x470);

#endif
