#ifndef GAME_EGGPAWN_H
#define GAME_EGGPAWN_H

/* EggPawn (class name from RTTI): the Egg Pawn, EGG_PAWN in the stage editor (Game/EggPawn.cpp).
 * Its body is EggPawnBase, an EnemyWalker (Game/EnemyWalker.h), and its AI EggPawnAI, an
 * EnemyWalkerAI (Game/EggPawnAI.cpp); EnemyTemplate<EggPawnBase, EggPawnAI> (Game/Enemy.h) puts
 * them together. Member names are inferred unless they repeat a parameter name of the stage
 * editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/EnemyWalker.h"
#include "Game/EnemyWalkerAI.h"
#include <dolphin/types.h>
#include "size_assert.h"

class EnemyShield;
class EnemyWeapon;
class EggPawnBase;

/* The Egg Pawn's parameters (struct name unknown). Names repeat the stage editor's. */
struct EggPawnParam {
    EggPawnParam()
        : MoveRange(200.0f), SearchRange(0.0f), SearchAngle(0.0f), SearchWidth(100.0f),
          SearchHeight(100.0f), SearchHeightOffset(0.0f), MoveSpeedRatio(1.0f), HaveShield(0),
          WeaponType(0), AppearType(0), OffsetPos_X(0.0f), OffsetPos_Z(0.0f), OffsetPos_Y(0.0f),
          WaitActMoveType(0), Pos0_ActionTime(3.0f), Pos0_ActionType(0), Pos0_MoveSpeedRatio(1.0f),
          Pos1_X(0.0f), Pos1_Z(0.0f), Pos1_ActionTime(3.0f), Pos1_ActionType(0),
          Pos1_MoveSpeedRatio(1.0f), Pos2_X(0.0f), Pos2_Z(0.0f), Pos2_ActionTime(3.0f),
          Pos2_ActionType(0), Pos2_MoveSpeedRatio(1.0f) {}

    int GetWeaponType() {
        return WeaponType;
    }

    /* The values of point index (Game/EnemyWalker.h). */
    float GetActionTime(int index) const {
        return EnemyWalkerPoints<EggPawnParam>::GetActionTime(*this, index);
    }
    int GetActionType(int index) const {
        return EnemyWalkerPoints<EggPawnParam>::GetActionType(*this, index);
    }
    float GetMoveSpeedRatio(int index) const {
        return EnemyWalkerPoints<EggPawnParam>::GetMoveSpeedRatio(*this, index);
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
    /* 0x28 */ float OffsetPos_X;
    /* 0x2C */ float OffsetPos_Z;
    /* 0x30 */ float OffsetPos_Y;
    /* 0x34 */ s32 WaitActMoveType;
    /* 0x38 */ float Pos0_ActionTime;
    /* 0x3C */ s32 Pos0_ActionType;
    /* 0x40 */ float Pos0_MoveSpeedRatio;
    /* 0x44 */ float Pos1_X;
    /* 0x48 */ float Pos1_Z;
    /* 0x4C */ float Pos1_ActionTime;
    /* 0x50 */ s32 Pos1_ActionType;
    /* 0x54 */ float Pos1_MoveSpeedRatio;
    /* 0x58 */ float Pos2_X;
    /* 0x5C */ float Pos2_Z;
    /* 0x60 */ float Pos2_ActionTime;
    /* 0x64 */ s32 Pos2_ActionType;
    /* 0x68 */ float Pos2_MoveSpeedRatio;
};
SIZE_ASSERT(EggPawnParam, 0x6C);

/* EggPawnBase (class name from RTTI; vtable 0x8052D034): the Egg Pawn's body. The overrides of
 * the parts' virtuals are declared in the order of the slots they add. */
class EggPawnBase : public EnemyWalker {
  public:
    EggPawnBase();
    virtual ~EggPawnBase();
    /* The EnemyWalker slots (Game/EnemyWalker.h). */
    virtual float vf_0x238(int index);
    virtual int vf_0x23C(int index);
    virtual float vf_0x240(int index);
    virtual Fn8040CopyVector3 vf_0x244(int index);
    virtual float vf_0x248();
    virtual int vf_0x24C(int index);
    virtual int vf_0x250();
    virtual int vf_0x254();
    virtual int vfStat_0xB8();
    virtual void vfDisp_0x10(float dt);
    virtual void vfStat_0xA0();
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfStat_0x38();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual int vfPara_0x48();
    virtual int vfPara_0x4C();
    virtual int vfPara_0x50();
    virtual int vfPara_0x54();
    virtual int vfPara_0x58();
    virtual int vfPara_0x5C();
    virtual int vfPara_0x64();
    virtual int vfPara_0x60();
    virtual float vfPara_0x20();
    virtual void vfDisp_0x08();
    virtual void vfDisp_0x64();
    virtual bool vfDisp_0x44(int *command);
    virtual void vfDisp_0x40(EnemyMotionEventArg &arg);

    /* Called by EggPawnAI (names inferred). */
    bool HasShield();
    int GetWeaponType();
    int GetAppearType();
    /* The shield's group (name inferred). */
    boost::weak_ptr<EnemyShield> CreateShield();

    /* 0x270 */ u8 field_0x270; /* has a shield (inferred) */
    /* 0x274 */ EggPawnParam m_params;
    /* 0x2E0 */ EnemyWeapon *m_weapon;
    /* 0x2E4 */ boost::weak_ptr<EnemyShield> m_shield;
    /* 0x2EC */ float field_0x2EC;
    /* 0x2F0 */ u8 field_0x2F0;
    /* 0x2F1 */ u8 field_0x2F1;
};

/* EggPawnAI (class name from RTTI; vtable 0x8052D6F4; Game/EggPawnAI.cpp): the Egg Pawn's AI. Its
 * attack states are local to its unit. */
class EggPawnAI : public EnemyWalkerAI {
  public:
    EggPawnAI();
    virtual ~EggPawnAI();
    virtual void vfAI_0x38();
    virtual void vfAI_0x3C();
    virtual void vfAI_0x48();

    /* 0x7C */ EggPawnBase *m_body;
};

/* EggPawn (class name from RTTI; vtable 0x8052C9B8). */
class EggPawn : public EnemyTemplate<EggPawnBase, EggPawnAI> {
  public:
    EggPawn(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~EggPawn();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(EggPawn, 0x3B0);

#endif
