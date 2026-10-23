#ifndef GAME_BKWINGLARGE_H
#define GAME_BKWINGLARGE_H

/* BkWingLarge (class name from RTTI): the large Black Arms wing, BK_WINGLARGE in the stage editor
 * (Game/BkWingLarge.cpp). Its body is BkWingLargeBase, a FlyerCommonBase (Game/EnemyFlyer.h), and
 * its AI BkWingLargeAI; EnemyTemplate<BkWingLargeBase, BkWingLargeAI> (Game/Enemy.h) puts them
 * together. It fires BkWingLargeAirCutter, a FlyingObject (Game/FlyingObject.h). Member names are
 * inferred unless they repeat a parameter name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/EnemyFlyer.h"
#include "Game/FlyingObject.h"
#include "Game/GadgetSound.h"
#include "Game/Task.h"
#include "Effect/System/EffectElement.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The large wing's parameters (struct name unknown). */
struct BkWingLargeParam : public FlyerParam {
    BkWingLargeParam() {
        Reset();
        Type = 0;
    }

    /* 0x38 */ u32 Type; /* bits 0-3 and 4-7 select variants (meanings unknown) */
};
SIZE_ASSERT(BkWingLargeParam, 0x3C);

/* BkWingLargeBase (class name from RTTI; vtable 0x80533530): the large wing's body. The
 * declaration order of the overrides sets the order of the slots appended to the vtable. */
class BkWingLargeBase : public FlyerCommonBase {
  public:
    BkWingLargeBase();
    virtual ~BkWingLargeBase();
    /* Inline, first emitted with the AI unit (0x801815A8, 0x8018141C). */
    virtual bool vf_0x238() {
        return m_params.AppearType == 1;
    }
    virtual bool vf_0x23C() {
        return m_params.ActionType == 1;
    }
    /* Set the status's motion from vfPara_0x58 and vfPara_0x48 (behaviour inferred). */
    virtual void vf_0x244();
    virtual void vf_0x248();
    virtual int vfStat_0xB8();
    virtual int vfStat_0xC4();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x10(float dt);
    virtual void vfDisp_0x50();
    virtual int vfPara_0x48();
    virtual int vfPara_0x4C();
    virtual int vfPara_0x50();
    virtual int vfPara_0x54();
    virtual int vfPara_0x58();
    virtual int vfPara_0x5C();
    virtual int vfPara_0x60();
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    virtual bool vfDisp_0x44(int *command);
    /* Inline; its weak copy is first emitted with Game/BkWingLarge.cpp (0x8018FBD8). */
    virtual float vfPara_0x68() {
        if (m_enemy->m_status->vfStat_0x58() && (m_params.Type & 0xF)) {
            return 10.0f;
        }
        return 0.0f;
    }

    /* 0x310 */ BkWingLargeParam m_params;
    /* 0x34C */ UnkSoundHandle_801D3DC8 m_sound;
};

/* BkWingLargeAI (class name from RTTI; vtable 0x8052E1A4; constructor 0x801816EC): the large
 * wing's AI. */
class BkWingLargeAI : public FlyerCommonAI {
  public:
    BkWingLargeAI();
    virtual ~BkWingLargeAI() {} /* weak, first at 0x80180A34 */
#ifdef ENEMY_AI_MESSAGE_ARG
    virtual void vfAI_0x28(UnkEnemyMessage_8019DC9C *message); /* 0x801815BC */
#else
    virtual void vfAI_0x28();   /* 0x801815BC */
#endif
    virtual void vfAI_0x3C();   /* 0x80181430 */
    virtual void vfAI_0x44();   /* 0x801813C8 */
    virtual void vfAI_0x58();   /* 0x801812CC */

    /* Spawns object kind 8 or 9 (by bits 4-7 of Type) where the wing is, as the DeadToAirWing state
     * ends (0x801811C4; name and behaviour inferred). */
    void SpawnAirWing();

    /* 0x8C */ BkWingLargeBase *m_body;
};

/* BkWingLarge (class name from RTTI; vtable 0x80532CBC). */
class BkWingLarge : public EnemyTemplate<BkWingLargeBase, BkWingLargeAI> {
  public:
    BkWingLarge(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~BkWingLarge();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(BkWingLarge, 0x430);

/* BkWingLargeAirCutter (class name from RTTI; vtable 0x805337E4): the air cutter the large wing
 * fires, a FlyingObject run by its own Task. */
class BkWingLargeAirCutter : public FlyingObject, public Task {
  public:
    BkWingLargeAirCutter(const FlyingObjectMotion *motion, const AttackObjectParam *param);
    virtual ~BkWingLargeAirCutter();
    virtual bool vf_0x20();
    virtual void Tick(float dt);

    /* Fires one from position with rotation (name inferred). */
    static BkWingLargeAirCutter *Create(const Vec3f *position, const Vec3f *rotation);

    /* 0xD0 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0xD8 */ Vec3f m_effectRotation;
    /* 0xE4 */ float m_soundTimer;
};
SIZE_ASSERT(BkWingLargeAirCutter, 0xE8);

#endif
