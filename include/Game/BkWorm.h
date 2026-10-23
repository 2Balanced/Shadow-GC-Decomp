#ifndef GAME_BKWORM_H
#define GAME_BKWORM_H

/* BkWorm (class name from RTTI): the Black Arms worm, BK_WORM in the stage editor
 * (Game/BkWorm.cpp). Its body is BkWormBase, an EnemyWalkerCommon (Game/Enemy.h), and its AI
 * BkWormAI; EnemyTemplate<BkWormBase, BkWormAI> puts them together. The worm comes out of the
 * ground and fires homing shots from the barrels of one of three weapons (WeaponType). Member
 * names are inferred unless they repeat a parameter name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Effect {
namespace System {
class EffectElement;
} // namespace System
} // namespace Effect

namespace Weapon {
class LockonTarget;
} // namespace Weapon

/* The worm's parameters (struct name unknown; its constructor is inline). Names repeat the stage
 * editor's. */
struct BkWormParam {
    BkWormParam() {
        MoveRange = 200.0f;
        SearchRange = 0.0f;
        SearchAngle = 0.0f;
        SearchWidth = 100.0f;
        SearchHeight = 100.0f;
        SearchHeightOffset = 0.0f;
        MoveSpeedRatio = 1.0f;
        WeaponType = 0;
        AttackTimes = 1;
        IntervalSec = 3.0f;
    }

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 WeaponType; /* 0 NORMAL, 1 WIDE, 2 BIG */
    /* 0x20 */ s32 AttackTimes;
    /* 0x24 */ float IntervalSec;
};
SIZE_ASSERT(BkWormParam, 0x28);

/* BkWormBase (class name from RTTI; vtable 0x805355E4): the worm's body. */
class BkWormBase : public EnemyWalkerCommon {
  public:
    BkWormBase();
    virtual ~BkWormBase();
    virtual void vfDisp_0x10(float dt);
    virtual int vfStat_0xB8();
    virtual void vfStat_0x38();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void vfStat_0xA0();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x4C();
    virtual bool vfDisp_0x44(int *command);
    virtual void vfDisp_0x5C();
    virtual void vfDisp_0x54();
    virtual int vfPara_0x48();
    virtual int vfPara_0x5C();

    /* The shots: creates a lock-on target per barrel, fires one shot from each barrel, and fires
     * one from matrix along target (names inferred). */
    void CreateLockons();
    void FireShots();
    void Shoot(Mtx34f *matrix, boost::shared_ptr<Weapon::LockonTarget> &target, int type);
    /* Stops effect index of m_effects, and starts it at position (names inferred). */
    void StopEffect(int index);
    void StartEffect(const Vec3f *position, int index);
    /* The time between appearances (IntervalSec; weak copy first at 0x801832F8; name inferred). */
    float GetInterval() {
        return m_params.IntervalSec;
    }

    /* 0x270 */ BkWormParam m_params;
    /* 0x298 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x2A0 */ boost::weak_ptr<Effect::System::EffectElement> m_effects[2];
    /* 0x2B0 */ float field_0x2B0;
    /* 0x2B4 */ boost::shared_ptr<Weapon::LockonTarget> m_lockons[6];
};

/* BkWormAI (class name from RTTI; vtable 0x8052EA6C; constructor 0x8018358C): the worm's AI. */
class BkWormAI : public EnemyBaseAI {
  public:
    BkWormAI();
    virtual ~BkWormAI() {}    /* weak, first at 0x8018252C */
    virtual void vfAI_0x38(); /* 0x80183530 */
    virtual void vfAI_0x3C(); /* 0x801834D8 */
    virtual void vfAI_0x58(); /* 0x80183474 */

    /* A point to move to: a random one around the worm while it is well inside its move range,
     * else one back toward home (0x80183300; name inferred). */
    void GetMovePoint(Vec3f *point);
#ifdef ENEMY_STAT_0x90_ARG
    /* Shows the worm above ground, or hides it (name inferred). */
    void SetAppeared(bool appeared) {
        m_enemy->m_status->vfStat_0x3C(!appeared);
        m_enemy->m_status->vfStat_0x7C(appeared, appeared);
        m_enemy->m_status->vfStat_0x90(appeared);
    }
#endif

    /* 0x74 */ BkWormBase *m_worm; /* the body (name inferred) */
    /* 0x78 */ s32 m_shots;        /* shots left before it moves (name inferred) */
    /* 0x7C */ u8 m_underground;   /* name inferred */
};

/* BkWorm (class name from RTTI; vtable 0x80534FE0). */
class BkWorm : public EnemyTemplate<BkWormBase, BkWormAI> {
  public:
    BkWorm(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~BkWorm();
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    virtual bool vfMove_0x10();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(BkWorm, 0x3A0);

#endif
