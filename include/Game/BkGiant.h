#ifndef GAME_BKGIANT_H
#define GAME_BKGIANT_H

/* BkGiant (namespace and class names from RTTI): the Black Arms giant, BK_GIANT in the stage
 * editor (Game/BkGiant.cpp). Its body is BkGiant::BkGiant, an EnemyWalkerCommon (Game/Enemy.h)
 * that carries a black sword, a dark hammer or a big barrel (WeaponType) and may guard against
 * hits from the front (Guard), and its AI BkGiant::BkGiantAI; BkGiant::BkGiantTask is
 * EnemyTemplate<BkGiant, BkGiantAI>. Member names are inferred unless they repeat a parameter
 * name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include <dolphin/types.h>
#include "size_assert.h"

class EnemyWeapon;

namespace BkGiant {

/* The giant's parameters (struct name unknown; its constructor is inline). Names repeat the stage
 * editor's; the editor's label of Guard is "Gurad : ON". */
struct BkGiantParam {
    BkGiantParam()
        : MoveRange(200.0f), SearchRange(0.0f), SearchAngle(0.0f), SearchWidth(100.0f),
          SearchHeight(100.0f), SearchHeightOffset(0.0f), MoveSpeedRatio(1.0f), AppearType(0),
          WeaponType(0), OffsetPos_Y(0.0f), Guard(1) {}

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 AppearType; /* 0 WAIT, 1 DROP */
    /* 0x20 */ s32 WeaponType; /* 0 BLACK_SWORD, 1 DARK_HAMMER, 2 BIG_BARREL */
    /* 0x24 */ float OffsetPos_Y;
    /* 0x28 */ s32 Guard;
};
SIZE_ASSERT(BkGiantParam, 0x2C);

/* BkGiant::BkGiant (class name from RTTI; vtable 0x805502F0): the giant's body. */
class BkGiant : public EnemyWalkerCommon {
  public:
    BkGiant();
    virtual ~BkGiant();
    virtual void vfDisp_0x10(float dt);
    virtual void vfStat_0x38();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual int vfStat_0xB8();
    virtual bool vfStat_0xC8();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
    virtual void vfDisp_0x40(EnemyMotionEventArg &arg);
    virtual bool vfDisp_0x44(int *command);
    virtual void vfDisp_0x4C();
    virtual void vfDisp_0x50();
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
    virtual void vfStat_0xA0();

    /* The WeaponType (name inferred). */
    int GetWeaponType() {
        return m_params.WeaponType;
    }

    /* 0x270 */ BkGiantParam m_params;
    /* 0x29C */ EnemyWeapon *m_weapon;
    /* 0x2A0 */ s32 m_hits;      /* hits taken within ten seconds of each other */
    /* 0x2A4 */ float m_hitTime; /* the time since the last hit */
    /* 0x2A8 */ float m_voiceTime; /* the time to the next growl */
    /* 0x2AC: EnemyReferer (virtual base; offset from the RTTI's base table) */
};
SIZE_ASSERT(BkGiant, 0x2B4);

/* BkGiant::BkGiantAI (class name from RTTI; vtable 0x80551EC4; constructor 0x8028EC8C,
 * destructor 0x8028EC30): the giant's AI. */
class BkGiantAI : public EnemyBaseAI {
  public:
    BkGiantAI();
    virtual ~BkGiantAI();
    virtual void vfAI_0x2C();
    virtual void vfAI_0x38();
    virtual void vfAI_0x3C();
    virtual void vfAI_0x48();
    virtual void vfAI_0x50();

    /* 0x74 */ u8 field_0x74[0x78 - 0x74];
};

} // namespace BkGiant

#endif
