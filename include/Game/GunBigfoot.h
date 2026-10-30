#ifndef GAME_GUNBIGFOOT_H
#define GAME_GUNBIGFOOT_H

/* GunBigfoot (namespace and class names from RTTI): the G.U.N. Bigfoot walker, GUN_BIGFOOT in the
 * stage editor (Game/GunBigfoot.cpp). Its body is GunBigfoot::GunBigfoot, an EnemyWalkerCommon
 * (Game/Enemy.h) that carries a vulcan or a missile launcher (WeaponType) and may hover, and its
 * AI GunBigfoot::GunBigfootAI (Game/GunBigfootAI.h); GunBigfoot::GunBigfootTask is
 * EnemyTemplate<GunBigfoot, GunBigfootAI>. Member names are inferred unless they repeat a
 * parameter name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/GadgetSound.h"
#include "Game/MathTypes.h"
#include "Game/Fn8040CopyVector3.h"
#include <dolphin/types.h>
#include "size_assert.h"

class EnemyWeapon;

namespace GunBigfoot {

/* The walker's parameters (struct name unknown; its constructor is inline). Names repeat the
 * stage editor's. While the walker hovers (AppearType HOVERING or ZUTTO_HOVERING) the editor shows
 * OffsetPos_Y as MoveWidth, how far it sways (GunBigfootAI). */
struct GunBigfootParam {
    GunBigfootParam()
        : MoveRange(200.0f), SearchRange(0.0f), SearchAngle(0.0f), SearchWidth(100.0f),
          SearchHeight(100.0f), SearchHeightOffset(0.0f), MoveSpeedRatio(1.0f), AppearType(0),
          WeaponType(0), OffsetPos_Y(0.0f) {}

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 AppearType; /* 0 WAIT_CROUCH, 1 HOVERING (until the target comes, then it
                                * lands), 2 DROP, 3 ACT_ATTACKING, 4 ZUTTO_HOVERING */
    /* 0x20 */ s32 WeaponType; /* 0 VULCAN, 1 MISSILE */
    /* 0x24 */ float OffsetPos_Y;
};
SIZE_ASSERT(GunBigfootParam, 0x28);

/* GunBigfoot::GunBigfoot (class name from RTTI; vtable 0x8054F368): the walker's body. */
class GunBigfoot : public EnemyWalkerCommon {
  public:
    GunBigfoot();
    virtual ~GunBigfoot();
    virtual void vfDisp_0x10(float dt);
    virtual void vfStat_0x38();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual int vfStat_0xB8();
    virtual bool vfStat_0x54();
    virtual bool vfStat_0x68();
    virtual bool vfStat_0xC8();
    virtual void vfStat_0xA0();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
    virtual void vfDisp_0x40(EnemyMotionEventArg &arg);
    virtual bool vfDisp_0x44(int *command);
    virtual float vfPara_0x6C();
    virtual int vfPara_0x48();
    virtual int vfPara_0x4C();
    virtual int vfPara_0x50();
    virtual int vfPara_0x54();
    /* The motion of an attack from the ground (7). */
    virtual int vfPara_0x58();
    virtual int vfPara_0x5C();
    virtual int vfPara_0x60();
    virtual float vfPara_0x68();
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);

    /* Fires the weapon: one vulcan shot from the muzzle, or a homing missile from the next of the
     * eight launcher tubes (names inferred). */
    void Fire();
    void FireVulcan();
    void FireMissile();
    /* Goes back to the first tube of the launcher's side (0x802875C8; name inferred). */
    void ResetTube();

    /* Sets the point the walker aims at (weak copy first at 0x80184C20). */
    void SetAimPoint(const Vec3f &point) {
        AssignVector(&m_aimPoint, &point);
    }
    /* The point the walker aims at (weak copy 0x802883DC; name inferred). */
    Fn8040CopyVector3 GetAimPoint() {
        return *reinterpret_cast<const Fn8040CopyVector3 *>(&m_aimPoint);
    }

    /* The model's update callback: turns the upper body toward the aim point (behaviour
     * inferred). */
    static void OnModelUpdate(void *arg);

    /* 0x270 */ GunBigfootParam m_params;
    /* 0x298 */ EnemyWeapon *m_weapon; /* never set by the walker; deleted with it */
    /* 0x29C */ u32 m_tube;            /* the launcher tube the next missile leaves from */
    /* 0x2A0 */ u8 m_hovering;
    /* 0x2A1 */ u8 m_jetOn;            /* the jet sound plays this frame (motion command 26) */
    /* 0x2A2 */ u8 m_aiming;           /* the AI is aiming at m_aimPoint */
    /* 0x2A4 */ Vec3f m_aimAngles;     /* the upper body's turn */
    /* 0x2B0 */ Vec3f m_aimPoint;
    /* 0x2BC */ UnkSoundHandle_801D3DC8 m_jetSound;
    /* 0x2D8: EnemyReferer (virtual base; offset from the RTTI's base table) */
};
SIZE_ASSERT(GunBigfoot, 0x2E0);

} // namespace GunBigfoot

#endif
