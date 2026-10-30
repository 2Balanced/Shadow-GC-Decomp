#ifndef GAME_EGGPIERROT_H
#define GAME_EGGPIERROT_H

/* EggPierrot (namespace and class names from RTTI): the Egg Pierrot, a clown robot that balances on
 * a ball and throws bombs, EGG_PIERROT in the stage editor (Game/EggPierrot.cpp). Its body is
 * EggPierrot::EggPierrot, an EnemyWalkerCommon (Game/Enemy.h) riding a PierrotBall and juggling the
 * bombs of a BakudanManager, and its AI EggPierrot::EggPierrotAI (Game/EggPierrotAI.cpp);
 * EggPierrot::EggPierrotTask is EnemyTemplate<EggPierrot, EggPierrotAI>. Member names are inferred
 * unless they repeat a parameter name of the stage editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace EggPierrot {

class PierrotBall;
class BakudanManager;

/* The parameters (struct name unknown; its constructor is inline). Names repeat the stage
 * editor's; the editor calls the one at 0x20 "NOT_USE". */
struct EggPierrotParam {
    EggPierrotParam()
        : MoveRange(200.0f), SearchRange(0.0f), SearchAngle(0.0f), SearchWidth(100.0f),
          SearchHeight(100.0f), SearchHeightOffset(0.0f), MoveSpeedRatio(1.0f), AppearType(0),
          NOT_USE(0), BakudanTime(5.0f), OffsetPos_X(0.0f), OffsetPos_Y(0.0f), OffsetPos_Z(0.0f) {}

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
    /* 0x1C */ s32 AppearType; /* 0 WANDER, 1 OFFSET */
    /* 0x20 */ s32 NOT_USE;
    /* 0x24 */ float BakudanTime; /* the fuse of a thrown bomb */
    /* 0x28 */ float OffsetPos_X;
    /* 0x2C */ float OffsetPos_Y;
    /* 0x30 */ float OffsetPos_Z;
};
SIZE_ASSERT(EggPierrotParam, 0x34);

/* EggPierrot::EggPierrot (class name from RTTI; vtable 0x80551890; constructor 0x8028AC50,
 * destructor 0x8028AB24): the clown's body. */
class EggPierrot : public EnemyWalkerCommon {
  public:
    EggPierrot();
    virtual ~EggPierrot();
    virtual void vfDisp_0x10(float dt);
    virtual void vfStat_0x38();
    virtual void vfStat_0x10();
    virtual void vfStat_0x3C(bool value);
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual int vfStat_0xB8();
    virtual void vfStat_0xA0();
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
    /* Takes the frame time, which Game/Enemy.h does not declare: defined under its symbol. */
    virtual void vfDisp_0x0C();
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
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);

    /* Whether the ball is worn down to two hits or fewer (0x80289CA4), and the bombs' juggling
     * restarted and stopped (0x80289CD4, 0x80289D08; names inferred). */
    bool IsBallWeak();
    void StartBakudan();
    void StopBakudan();

    /* 0x270 */ EggPierrotParam m_params;
    /* 0x2A4 */ u8 m_targetInArea; /* the AI's target is in the enemy's area */
    /* 0x2A5 */ u8 m_throwReady;   /* set by the AI: the throw motion event throws a bomb */
    /* 0x2A8 */ boost::shared_ptr<BakudanManager> m_bakudan;
    /* 0x2B0 */ boost::shared_ptr<PierrotBall> m_ball;
    /* 0x2B8 */ Vec3f field_0x2B8;
    /* 0x2C4 */ Vec3f m_home;
    /* 0x2D0 */ Vec3f field_0x2D0;
    /* 0x2DC */ float field_0x2DC;
    /* 0x2E0: EnemyReferer (virtual base) */
};

/* EggPierrot::EggPierrotAI (class name from RTTI; vtable 0x805522CC; constructor 0x8028FF34,
 * destructor 0x8028FED8): the clown's AI. */
class EggPierrotAI : public EnemyBaseAI {
  public:
    EggPierrotAI();
    virtual ~EggPierrotAI();
    virtual void vfAI_0x2C();
    virtual void vfAI_0x38();
    virtual void vfAI_0x3C();
    virtual void vfAI_0x48();

    /* 0x74 */ EggPierrot *m_body;
};

} // namespace EggPierrot

#endif
