#ifndef GAME_BKCHAOS_H
#define GAME_BKCHAOS_H

/* BkChaos (namespace and class names from RTTI): the Black Arms chaos, BK_CHAOS in the stage editor
 * (Game/BkChaos.cpp). Its body is BkChaos::BkChaos, an EnemyFlyerCommon (Game/EnemyFlyer.h) made of
 * a number of small chaos (BkChaos::BkChibiChaos, ChibiNum of them) that fly about its home and
 * gather into it (ShapeType CHIBI) or start gathered (ShapeType OYA); its AI is BkChaos::BkChaosAI
 * (Game/BkChaosAI.cpp). Member names are inferred unless they repeat a parameter name of the stage
 * editor. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/EnemyAttack.h"
#include "Game/EnemyFlyer.h"
#include "Game/GadgetSound.h"
#include "Game/UnkZeroVector3_80147830.h"
#include "Effect/System/EffectElement.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace BkChaos {

class BkChibiChaos;

/* The chaos's parameters (struct name unknown; its constructor is inline). Names repeat the stage
 * editor's. */
struct BkChaosParam {
    BkChaosParam()
        : MoveRange(200.0f), SearchRange(0.0f), SearchAngle(0.0f), SearchWidth(100.0f),
          SearchHeight(100.0f), SearchHeightOffset(0.0f), MaxLife(1.0f), ShapeType(0), ChibiNum(2),
          MoveHeight(100.0f), MoveHeightOffset(0.0f), CombineStartTime(1.0f), CombineTime(1.0f) {}

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MaxLife;
    /* 0x1C */ s32 ShapeType; /* 0 CHIBI (the chibis start apart), 1 OYA (gathered) */
    /* 0x20 */ s32 ChibiNum;
    /* 0x24 */ float MoveHeight;
    /* 0x28 */ float MoveHeightOffset;
    /* 0x2C */ float CombineStartTime;
    /* 0x30 */ float CombineTime;
};
SIZE_ASSERT(BkChaosParam, 0x34);

#ifdef BK_CHAOS_UNIT
/* The chibis (Game/BkChaos.cpp declares the list). */
class ChibiList;
#define BK_CHAOS_CHIBI_LIST ChibiList
#endif

/* BkChaos::BkChaos (class name from RTTI; vtable 0x80556F84): the chaos's body. The declaration
 * order of the overrides sets the order of the slots appended to the vtable. */
class BkChaos : public EnemyFlyerCommon {
  public:
    BkChaos();
    virtual ~BkChaos();
    virtual void vfStat_0x10();
    virtual void vfStat_0x38();
    virtual void vfStat_0x70(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual int vfStat_0xB8();
#ifdef ENEMY_STAT_SLOT_ARGS
    virtual void vfStat_0x98(const boost::shared_ptr<CharInfo> &attacker);
#else
    virtual void vfStat_0x98();
#endif
    virtual void vfDisp_0x64();
    virtual void vfDisp_0x08();
    virtual void vfDisp_0x10(float dt);
    virtual void vfDisp_0x40(EnemyMotionEventArg &arg);
    virtual bool vfDisp_0x44(int *command);
    virtual void vfDisp_0x4C();
    virtual float vfPara_0x6C();
    virtual int vfPara_0x48();
    virtual int vfPara_0x4C();
    virtual int vfPara_0x54();
    virtual int vfPara_0x58();
    virtual int vfPara_0x5C();
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);

    /* Whether the target is close enough for the rolling attack (0x802AFC0C). */
    bool IsTargetNear();
    /* Calls the chibis into the body (0x802AFA44). */
    void Gather();
    /* Lets go of the chibis that are in the body (0x802AF8E0). */
    void ReleaseChibis();
    /* A chibi was hit or taken by attacker: the body loses a chibi's share of its life
     * (0x802AFD80, 0x802AFD14). */
    void TakeHit(boost::shared_ptr<CharInfo> attacker);
    float GetChibiLife();
    /* Where the chibis gather: the core node of the model (0x802AF888). */
    Fn8040CopyVector3 GetCorePosition();
    /* Grows or shrinks the body toward the chibis in it (0x802AF7D4). */
    void UpdateSize();

    /* 0x268 */ float m_minY; /* the height range the chibis fly in */
    /* 0x26C */ float m_maxY;
    /* 0x270 */ BkChaosParam m_params;
    /* 0x2A4 */ float m_size; /* the chibis in the body, by five */
    /* 0x2A8 */ EnemyAttack m_attack;
#ifdef BK_CHAOS_CHIBI_LIST
    /* 0x2C0 */ BK_CHAOS_CHIBI_LIST m_chibis;
#else
    /* 0x2C0 */ u8 m_chibis[0xC];
#endif
    /* 0x2CC */ s32 m_combined; /* the chibis in the body */
    /* 0x2D0 */ s32 m_hits; /* the chibis hit */
    /* 0x2D4 */ float m_combineTimer;
    /* 0x2D8 */ s32 m_state; /* 0 waiting to gather, 1 apart, 2 gathered */
    /* 0x2DC */ UnkZeroVector3_80147830 m_tilt[2]; /* of the two stretching nodes */
    /* 0x2F4 */ float m_tiltRate[2];
    /* 0x2FC */ s32 field_0x2FC;
    /* 0x300 */ u8 field_0x300;
    /* 0x304 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x30C */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x328 */ UnkSoundHandle_801D3DC8 m_rollSound;
    /* 0x344: EnemyReferer (virtual base; offset from the RTTI's base table) */
};

/* BkChaos::BkChaosAI (class name from RTTI; vtable 0x805574FC; constructor 0x802B3D38,
 * destructor 0x802B3CDC; Game/BkChaosAI.cpp): the chaos's AI. */
class BkChaosAI : public EnemyBaseAI {
  public:
    BkChaosAI();
    virtual ~BkChaosAI();
#ifdef ENEMY_AI_MESSAGE_ARG
    virtual void vfAI_0x28(UnkEnemyMessage_8019DC9C *message);
#else
    virtual void vfAI_0x28();
#endif
    virtual void vfAI_0x38();
    virtual void vfAI_0x3C();
    virtual void vfAI_0x48();
    virtual void vfAI_0x44();
    virtual void vfAI_0x58();

    /* 0x74 */ BkChaos *m_body;
};

} // namespace BkChaos

#endif
