#ifndef GAME_BKNINJA_H
#define GAME_BKNINJA_H

/* BkNinja (namespace and class names from RTTI): the Black Arms ninja, BK_NINJA in the stage
 * editor (Game/BkNinja.cpp). Its body is BkNinja::BkNinja, an EnemyWalkerCommon (Game/Enemy.h)
 * that throws its weapon, vanishes and warps between two points (Pos0, Pos1) or rides an Air
 * Saucer (AppearType), and its AI BkNinja::BkNinjaAI (Game/BkNinjaAI.cpp); BkNinja::BkNinjaTask
 * is EnemyTemplate<BkNinja, BkNinjaAI>. Member names are inferred unless they repeat a parameter
 * name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class EnemyWeapon;

namespace BkNinja {


/* The ninja's parameters (struct name unknown; its constructor is inline). Names repeat the stage
 * editor's. */
struct BkNinjaParam {
    BkNinjaParam()
        : MoveRange(200.0f), SearchRange(0.0f), SearchAngle(0.0f), SearchWidth(100.0f),
          SearchHeight(100.0f), SearchHeightOffset(0.0f), MoveSpeedRatio(1.0f), AppearType(0),
          ShootCount(1), AttackInterval(1.0f), WaitInterval(1.0f), Pos0_X(0.0f), Pos0_Y(0.0f),
          Pos0_Z(0.0f), Pos0_AngY(0.0f), Pos0_WaitType(0), Pos0_DisappearTime(0.0f), Pos1_X(0.0f),
          Pos1_Y(0.0f), Pos1_Z(0.0f), Pos1_AngY(0.0f), Pos1_WaitType(0), Pos1_DisappearTime(0.0f) {}

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 AppearType; /* 0 to 3; 1 waits, 2 and 3 warp, 3 rides an Air Saucer (inferred) */
    /* 0x20 */ s32 ShootCount;
    /* 0x24 */ float AttackInterval;
    /* 0x28 */ float WaitInterval;
    /* 0x2C */ float Pos0_X;
    /* 0x30 */ float Pos0_Y;
    /* 0x34 */ float Pos0_Z;
    /* 0x38 */ float Pos0_AngY; /* degrees */
    /* 0x3C */ s32 Pos0_WaitType;
    /* 0x40 */ float Pos0_DisappearTime;
    /* 0x44 */ float Pos1_X;
    /* 0x48 */ float Pos1_Y;
    /* 0x4C */ float Pos1_Z;
    /* 0x50 */ float Pos1_AngY;
    /* 0x54 */ s32 Pos1_WaitType;
    /* 0x58 */ float Pos1_DisappearTime;
};
SIZE_ASSERT(BkNinjaParam, 0x5C);

/* BkNinja::BkNinja (class name from RTTI; vtable 0x8055566C): the ninja's body. */
class BkNinja : public EnemyWalkerCommon {
  public:
    BkNinja();
    virtual ~BkNinja();
    virtual void vfDisp_0x10(float dt);
    virtual void vfStat_0x38();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual int vfStat_0xB8();
    virtual bool vfStat_0x54();
    virtual bool vfStat_0x68();
    virtual float vfStat_0xCC();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
    virtual void vfDisp_0x40(EnemyMotionEventArg &arg);
    virtual bool vfDisp_0x44(int *command);
    virtual void vfDisp_0x50();
    virtual int vfDisp_0x14();
    virtual float vfPara_0x6C();
    virtual int vfPara_0x48();
    virtual int vfPara_0x4C();
    virtual int vfPara_0x50();
    virtual int vfPara_0x54();
    virtual int vfPara_0x58();
    virtual int vfPara_0x5C() const;
    virtual int vfPara_0x60() const;
    virtual float vfPara_0x68();
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    virtual void vfStat_0xC0();
    virtual void vfStat_0xA0();
    virtual void vfMove_0x0C(float dt);

    /* Names below are inferred. Whether the ninja is in its attack motion with the target in its area. */
    bool IsAttacking();
    /* Vanishes (fades out) and comes back (fades in). */
    void Disappear();
    void Appear();
    /* Gets off the Air Saucer it rides, and lets it go. */
    void GetOffVehicle();
    void ReleaseVehicle();
    /* The motion of the current point's WaitType. */
    int GetWaitMotion();
    /* A point on the floor about radius from center, inside the ninja's area if possible. */
    Fn8040CopyVector3 FindWarpPoint(const Vec3f *center, float radius);
    /* Switches between the two points, and the current point's data. The getters are const:
     * retail schedules their loads of the ninja's members as GC/2.7 does only for const
     * members. */
    void NextWarpPoint();
    float GetDisappearTime();
    float GetWarpAngle() const;
    float GetWarpAngle(int index) const;
    Vec3f GetWarpPosition() const;
    Vec3f GetWarpPosition(int index) const;

    /* The shots of an attack (const for the same reason). */
    int GetShootCount() const {
        return m_params.ShootCount;
    }
    /* The time between attacks while it waits (weak copy in Game/BkNinjaAI.cpp). */
    float GetWaitInterval() {
        return m_params.WaitInterval;
    }

    /* The model's update callback: turns the upper body toward the target. */
    static void OnModelUpdate(void *data);

    /* 0x270 */ BkNinjaParam m_params;
    /* 0x2CC */ EnemyWeapon *m_weapon;
    /* 0x2D0 */ s32 m_warpIndex; /* the point the ninja warps to: 0 or 1 */
    /* 0x2D4 */ u8 m_hidden;     /* while it warps, it can't be hit */
    /* 0x2D8 */ boost::weak_ptr<Vehicle::TVehicleBase> m_vehicle;
    /* 0x2E0 */ u8 m_vanishing;  /* fading out (else in) */
    /* 0x2E4 */ Mtx34f m_vanishMatrix; /* the model's stretch while it fades */
    /* 0x314 */ float m_vanish;  /* 0 shown, 1 gone */
    /* 0x318 */ float m_voiceTime;
    /* 0x31C */ Vec2f m_aim;     /* the upper body's pitch and yaw */
    /* 0x324: EnemyReferer (virtual base) */
};
SIZE_ASSERT(BkNinja, 0x32C);

/* BkNinja::BkNinjaAI (class name from RTTI; vtable 0x80555C04; constructor 0x802AF0C0,
 * destructor 0x802AF064; Game/BkNinjaAI.cpp): the ninja's AI. */
class BkNinjaAI : public EnemyBaseAI {
  public:
    BkNinjaAI();
    virtual ~BkNinjaAI();
    virtual void vfAI_0x38();
    virtual void vfAI_0x3C();
    virtual void vfAI_0x48();

    /* 0x74 */ BkNinja *m_body;
    /* 0x78 */ s32 m_shots; /* the shots of the current attack */
};

} // namespace BkNinja

#endif
