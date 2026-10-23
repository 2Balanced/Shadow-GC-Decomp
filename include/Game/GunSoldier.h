#ifndef GAME_GUNSOLDIER_H
#define GAME_GUNSOLDIER_H

/* GunSoldier (namespace and class names from RTTI): the G.U.N. soldier, GUN_SOLDIER in the stage
 * editor (Game/GunSoldier.cpp). Its body is GunSoldierBase, a SoldierCommonBase (Game/Enemy.h), and
 * its AI GunSoldierAI; GunSoldier::GunSoldier is EnemyTemplate<GunSoldierBase, GunSoldierAI>, the
 * task that runs it. STemporaryData is what the soldier leaves in its placement record across
 * respawns. Member names are inferred unless they repeat a parameter name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/EnemyHint.h"
#include "Game/SetAdapterState.h"
#include "Game/SoldierParam.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class EnemyShield;
class EnemyWeapon;

namespace GunSoldier {

class GunSoldierBase;

/* GunSoldierAI (class name from RTTI; vtable 0x8053008C; constructor 0x80188334): the soldier's AI.
 * Its code is with the other AI states (around 0x80187EFC). */
class GunSoldierAI : public SoldierCommonAI {
  public:
    GunSoldierAI();
    virtual ~GunSoldierAI() {} /* weak, first at 0x80186738 */
    virtual void vfAI_0x38(); /* 0x80188218 */
    virtual void vfAI_0x3C(); /* 0x8018802C */
    virtual void vfAI_0x44(); /* 0x80187EFC */
    virtual void vfAI_0x58(); /* 0x80188134 */
    virtual void vfAI_0x60(); /* 0x80187F34 */

    /* Goes over to the attack that suits the soldier's weapon (0x80187DEC; name inferred). */
    void StartAttack();

    /* 0x7C */ GunSoldierBase *m_body;
};

/* STemporaryData (class name from RTTI; vtable 0x80539810): the state a soldier keeps in its
 * placement record, so that a respawned soldier resumes it. */
class STemporaryData : public SetAdapterState {
  public:
    STemporaryData() {
        m_state = 0;
        field_0x8 = 0;
    }

    /* 0x4 */ s32 m_state; /* 0 normal, 1 down (HINSHI), 2 after its radio contact */
    /* 0x8 */ s32 field_0x8;
};
SIZE_ASSERT(STemporaryData, 0xC);

/* GunSoldierBase (class name from RTTI; vtable 0x80539BEC): the soldier's body. */
class GunSoldierBase : public SoldierCommonBase {
  public:
    GunSoldierBase();
    virtual ~GunSoldierBase();
    virtual int vf_0x238(int index);
    virtual int vf_0x23C();
    virtual void vfDisp_0x10(float dt);
    virtual void vfStat_0x08();
    virtual void vfStat_0x10();
    virtual void vfStat_0x0C();
    virtual void vfStat_0x38();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual int vfStat_0xB8();
    virtual void vfStat_0xA0();
    virtual void vfStat_0xA4();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
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
    virtual int vfPara_0x64();
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    /* Stores the soldier's state in its placement record's STemporaryData (behaviour inferred). */
    virtual void vf_0x29C() = 0;
    /* Marks the placement record (flag 0x8; behaviour inferred). */
    virtual void vf_0x2A0() = 0;

    /* Called by the soldier's AI states (meanings unknown). */
    void fn_80197280();
    void fn_801972A8();
    /* The shield's group (fn_801972D4; name inferred). */
    boost::weak_ptr<EnemyShield> CreateShield();

    /* 0x2E8 */ SoldierParam m_params;
    /* 0x344 */ s32 field_0x344;
    /* 0x348 */ boost::weak_ptr<EnemyShield> m_shield;
    /* 0x350 */ EnemyWeapon *m_weapon;
    /* 0x354 */ u8 m_hintShown;   /* a hint for the target in range was shown */
    /* 0x355 */ u8 m_hintApproach; /* a hint for the approaching player was shown */
    /* 0x356 */ u8 m_contacted;   /* the radio contact is over */
    /* 0x357 */ u8 field_0x357;
    /* 0x358 */ u8 field_0x358;
    /* 0x35C */ EnemyHint m_hint;
};

/* GunSoldier::GunSoldier (class name from RTTI; vtable 0x80538D5C): the task that runs a soldier. */
class GunSoldier : public EnemyTemplate<GunSoldierBase, GunSoldierAI> {
  public:
    GunSoldier(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~GunSoldier();
    virtual void vf_0x29C();
    virtual void vf_0x2A0();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(GunSoldier, 0x418);

} // namespace GunSoldier

#endif
