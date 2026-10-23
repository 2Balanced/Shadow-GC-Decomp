#ifndef GAME_BKSOLDIER_H
#define GAME_BKSOLDIER_H

/* BkSoldier (class name from RTTI): the Black Arms soldier, BK_SOLDIER in the stage editor
 * (Game/BkSoldier.cpp). Its body is BkSoldierBase, a SoldierCommonBase, and its AI BkSoldierAI
 * (Game/EnemyAI.h); EnemyTemplate<BkSoldierBase, BkSoldierAI> (Game/Enemy.h) puts them together.
 * Member names are inferred unless they repeat a parameter name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include <dolphin/types.h>
#include "size_assert.h"

class EnemyShield;
class EnemyWeapon;

/* The parameters a soldier shares with the other soldiers (struct name unknown; its constructor
 * 0x8018ED00 is inline, also used by GunSoldier). Names repeat the stage editor's. */
struct SoldierParam {
    SoldierParam() {
        Reset();
    }

    /* The stage editor's defaults (name inferred). */
    void Reset() {
        MoveRange = 200.0f;
        SearchRange = 0.0f;
        SearchAngle = 0.0f;
        SearchWidth = 100.0f;
        SearchHeight = 100.0f;
        SearchHeightOffset = 0.0f;
        MoveSpeedRatio = 1.0f;
        HaveShield = 0;
        WeaponType = 0;
        AppearType = 0;
        Pos0_WaitType = 0;
        Pos0_WaitSec = 1.0f;
        Pos0_MoveSpeedRatio = 1.0f;
        Pos1_X = 0.0f;
        Pos1_Z = 0.0f;
        Pos1_WaitType = 0;
        Pos1_WaitSec = 0.0f;
        Pos1_MoveSpeedRatio = 1.0f;
        Pos2_X = 0.0f;
        Pos2_Z = 0.0f;
        Pos2_WaitType = 0;
        Pos2_WaitSec = 0.0f;
        Pos2_MoveSpeedRatio = 1.0f;
    }

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 HaveShield;
    /* 0x20 */ s32 WeaponType;
    /* 0x24 */ s32 AppearType;
    /* 0x28 */ s32 Pos0_WaitType;
    /* 0x2C */ float Pos0_WaitSec;
    /* 0x30 */ float Pos0_MoveSpeedRatio;
    /* 0x34 */ float Pos1_X;
    /* 0x38 */ float Pos1_Z;
    /* 0x3C */ s32 Pos1_WaitType;
    /* 0x40 */ float Pos1_WaitSec;
    /* 0x44 */ float Pos1_MoveSpeedRatio;
    /* 0x48 */ float Pos2_X;
    /* 0x4C */ float Pos2_Z;
    /* 0x50 */ s32 Pos2_WaitType;
    /* 0x54 */ float Pos2_WaitSec;
    /* 0x58 */ float Pos2_MoveSpeedRatio;
};
SIZE_ASSERT(SoldierParam, 0x5C);

/* The Black Arms soldier's parameters (struct name unknown). */
struct BkSoldierParam : public SoldierParam {
    BkSoldierParam() : RideAirSaucer(0) {}

    /* 0x5C */ s32 RideAirSaucer;
};
SIZE_ASSERT(BkSoldierParam, 0x60);

/* BkSoldierBase (class name from RTTI; vtable 0x80532190): the soldier's body. */
class BkSoldierBase : public SoldierCommonBase {
  public:
    BkSoldierBase();
    virtual ~BkSoldierBase();
    virtual int vf_0x238(int index);
    virtual int vf_0x23C();
    virtual int vfStat_0xB8();
    virtual void vfStat_0x38();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void vfStat_0xA0();
    virtual void vfStat_0xC0();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
    virtual void vfDisp_0x40(EnemyMotionEventArg &arg);
    virtual bool vfDisp_0x44(int *command);
    virtual int vfPara_0x48();
    virtual int vfPara_0x4C();
    virtual int vfPara_0x50();
    virtual int vfPara_0x54();
    virtual int vfPara_0x58();
    virtual int vfPara_0x5C();
    virtual int vfPara_0x60();
    virtual int vfPara_0x64();
    virtual void vfMove_0x0C(float dt);
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    virtual void vfDisp_0x50();
    virtual void vfDisp_0x10(float dt);

    /* Called by the soldier's AI states (behaviour inferred: they use the weapon). */
    void fn_8018D34C();
    void fn_8018D3B0();
    /* Gets off the Air Saucer it rides (names inferred). */
    void GetOffVehicle();
    void ReleaseVehicle();
    bool fn_8018D120();
    /* The shield's group (fn_8018DF90; name inferred). */
    boost::weak_ptr<EnemyShield> CreateShield();

    /* The model's update callback (behaviour inferred). */
    static void OnModelUpdate(void *arg);

    /* 0x2E8 */ BkSoldierParam m_params;
    /* 0x348 */ s32 field_0x348;
    /* 0x34C */ boost::weak_ptr<EnemyShield> m_shield;
    /* 0x354 */ EnemyWeapon *m_weapon;
    /* 0x358 */ u8 field_0x358;
    /* 0x359 */ u8 field_0x359;
    /* 0x35C */ float field_0x35C;
    /* 0x360 */ float field_0x360;
    /* 0x364 */ u8 field_0x364;
    /* 0x365 */ u8 field_0x365;
    /* 0x368 */ boost::weak_ptr<Vehicle::TVehicleBase> m_vehicle;
    /* 0x370 */ Vec3f field_0x370;
};

/* BkSoldier (class name from RTTI; vtable 0x80531364). */
class BkSoldier : public EnemyTemplate<BkSoldierBase, BkSoldierAI> {
  public:
    BkSoldier(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~BkSoldier();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(BkSoldier, 0x438);

#endif
