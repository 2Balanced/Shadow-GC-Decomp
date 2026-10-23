#ifndef GAME_GUNROBOT_H
#define GAME_GUNROBOT_H

/* GunRobot (class name from RTTI): the G.U.N. Robot, GUN_ROBOT in the stage editor
 * (Game/GunRobot.cpp). Its body is GunRobot, an EnemyWalker (Game/EnemyWalker.h), and its AI
 * GunRobotAI; GunRobotTask is EnemyTemplate<GunRobot, GunRobotAI> (Game/Enemy.h). Member names
 * are inferred unless they repeat a parameter name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/EnemyWalker.h"
#include "Game/EnemyWalkerAI.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Effect {
namespace System {
class EffectElement;
}
} // namespace Effect

class EnemyWeapon;

/* The Robot's parameters (struct name unknown; its constructor is inline). Names repeat the stage
 * editor's. With WaitActMoveType 3 (RANDOM), Pos1_ActionTime is the random move range. */
struct GunRobotParam {
    GunRobotParam()
        : MoveRange(200.0f), SearchRange(0.0f), SearchAngle(0.0f), SearchWidth(100.0f),
          SearchHeight(100.0f), SearchHeightOffset(0.0f), MoveSpeedRatio(1.0f), AppearType(0),
          WeaponType(0), BodyType(0), OffsetPos_X(0.0f), OffsetPos_Y(0.0f), OffsetPos_Z(0.0f),
          WaitActMoveType(0), Pos0_ActionTime(3.0f), Pos0_ActionType(0), Pos0_MoveSpeedRatio(1.0f),
          Pos1_X(0.0f), Pos1_Z(0.0f), Pos1_ActionTime(3.0f), Pos1_ActionType(0),
          Pos1_MoveSpeedRatio(1.0f), Pos2_X(0.0f), Pos2_Z(0.0f), Pos2_ActionTime(3.0f),
          Pos2_ActionType(0), Pos2_MoveSpeedRatio(1.0f) {}

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 AppearType;
    /* 0x20 */ s32 WeaponType;
    /* 0x24 */ s32 BodyType;
    /* 0x28 */ float OffsetPos_X;
    /* 0x2C */ float OffsetPos_Y;
    /* 0x30 */ float OffsetPos_Z;
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
SIZE_ASSERT(GunRobotParam, 0x6C);

/* GunRobot (class name from RTTI; vtable 0x8053801C): the Robot's body. */
class GunRobot : public EnemyWalker {
  public:
    GunRobot();
    virtual ~GunRobot();
    virtual void vfDisp_0x10(float dt);
    virtual float vf_0x238(int index);
    virtual int vf_0x23C(int index);
    virtual float vf_0x240(int index);
    virtual Fn8040CopyVector3 vf_0x244(int index);
    virtual float vf_0x248();
    virtual int vf_0x24C(int index);
    virtual int vf_0x250();
    virtual int vf_0x254();
    virtual int vfStat_0xB8();
    virtual bool vfStat_0xC8();
    virtual void vfStat_0xA0();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
    virtual void vfDisp_0x4C();
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
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfStat_0x38();

    /* Whether the Robot faces its target's way (behaviour inferred). */
    bool fn_8019531C();
    /* The AppearType (called by GunRobotAI; name inferred). */
    int GetAppearType();
    /* The WeaponType (name inferred). */
    int GetWeaponType() {
        return m_params.WeaponType;
    }

    /* The model's update callback (behaviour inferred). */
    static void OnModelUpdate(void *arg);

    /* 0x270 */ GunRobotParam m_params;
    /* 0x2DC */ EnemyWeapon *m_weapon;
    /* 0x2E0 */ float field_0x2E0;
    /* 0x2E4 */ u8 field_0x2E4;
    /* 0x2E5 */ u8 field_0x2E5;
    /* 0x2E8 */ Vec2f m_aim;
    /* 0x2F0 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
};

/* GunRobotAI (class name from RTTI; vtable 0x8052FC24; constructor 0x80186648): the Robot's AI. */
class GunRobotAI : public EnemyWalkerAI {
  public:
    GunRobotAI();
    virtual ~GunRobotAI() {}  /* weak, first at 0x801865F0 */
    virtual void vfAI_0x38(); /* 0x80186498 */
    virtual void vfAI_0x3C(); /* 0x801863D8 */
    virtual void vfAI_0x44(); /* 0x80186318 */
    virtual void vfAI_0x48(); /* 0x80186234 */
    virtual void vfAI_0x58(); /* 0x801861D4 */

    /* 0x7C */ GunRobot *m_body;
};

/* GunRobotTask (class name from RTTI; vtable 0x805379A0): the task that runs a Robot. */
class GunRobotTask : public EnemyTemplate<GunRobot, GunRobotAI> {
  public:
    GunRobotTask(Task *parent, UnkPlacement_800C9CD0 *placement);
    virtual ~GunRobotTask();

    /* The stage editor's hooks (names inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *placement);
    static void LoadFiles();
    static void DropFiles();

    static const char *s_taskName;
};
SIZE_ASSERT(GunRobotTask, 0x3B0);

#endif
