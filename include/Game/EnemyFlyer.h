#ifndef GAME_ENEMYFLYER_H
#define GAME_ENEMYFLYER_H

/* The flying enemies' common parts (class names from RTTI): EnemyMoveFlyer moves an enemy in the
 * air, EnemyFlyerCommon puts the parts of a flying enemy together (as EnemyWalkerCommon does for
 * the walkers, see Game/Enemy.h), FlyerCommonBase is the body of the Black Arms wings and the Gun
 * Beetle, and FlyerCommonAI their AI. Slot names follow Game/Enemy.h; FlyerCommonBase's new slots
 * are named after their offset in the primary vtable, as SoldierCommonBase's are. The inline code
 * is first emitted with Game/BkWingLarge.cpp unless an address says otherwise. */

#include "Game/Enemy.h"
#include "Game/EnemyAI.h"
#include "Game/PathCtrl.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The search parameters every enemy has (struct name unknown; names repeat the stage editor's). */
struct EnemySearchParam {
    /* The stage editor's defaults (name inferred). */
    void Reset() {
        MoveRange = 200.0f;
        SearchRange = 0.0f;
        SearchAngle = 0.0f;
        SearchWidth = 100.0f;
        SearchHeight = 100.0f;
        SearchHeightOffset = 0.0f;
        MoveSpeedRatio = 1.0f;
    }

    /* 0x00 */ float MoveRange;
    /* 0x04 */ float SearchRange;
    /* 0x08 */ float SearchAngle;
    /* 0x0C */ float SearchWidth;
    /* 0x10 */ float SearchHeight;
    /* 0x14 */ float SearchHeightOffset;
    /* 0x18 */ float MoveSpeedRatio;
};

/* The parameters every flyer has (struct name unknown; names repeat the stage editor's, the
 * unnamed one, the path, is shown without a label). FlyerCommonBase reads them (0x80193790). */
struct FlyerParam : public EnemySearchParam {
    /* The stage editor's defaults (name inferred; weak copy 0x801907A8, also used by BkWingSmall
     * and GunBeetle). */
    void Reset() {
        EnemySearchParam::Reset();
        AppearType = 0;
        ActionType = 0;
        m_path = 0;
        FloatWidth = 0.0f;
        AttackStart = 0.4f;
        AttackEnd = 0.6f;
        PathMirror = 0;
    }

    /* 0x1C */ s32 AppearType;
    /* 0x20 */ s32 ActionType;
    /* 0x24 */ s32 m_path;
    /* 0x28 */ float FloatWidth;
    /* 0x2C */ float AttackStart;
    /* 0x30 */ float AttackEnd;
    /* 0x34 */ s32 PathMirror;
};
SIZE_ASSERT(FlyerParam, 0x38);

/* EnemyMoveFlyer (class name from RTTI; vtable 0x80532FC0): movement in the air. */
class EnemyMoveFlyer : public EnemyMoveCommon, public virtual EnemyReferer {
  public:
    EnemyMoveFlyer() {
        field_0x8[0x5C - 0x8] = 0;
    }

    virtual int vfMove_0x20() {
        return 1;
    }
};

/* EnemyFlyerCommon (class name from RTTI; vtable 0x80532FF8): an enemy that flies. */
class EnemyFlyerCommon : public TEnemyCommonBase,
                         public EnemyDispCommon,
                         public EnemyMoveFlyer,
                         public TEnemySetBase {
  public:
    EnemyFlyerCommon(int type) : TEnemyCommonBase(type) {
        m_enemy = this;
        m_disp = this;
        m_move = this;
        m_set = this;
    }
    virtual ~EnemyFlyerCommon() {}
};

/* The path a flyer follows (class name unknown; constructor 0x801A6844 and destructor 0x801A67F0
 * are in the Enemy unit). */
class UnkEnemyPath_801A6844 {
  public:
    UnkEnemyPath_801A6844();
    ~UnkEnemyPath_801A6844();

    /* 0x00 */ SonicteamUSA::System::PathCtrl m_path;
    /* 0x58 */ float field_0x58;
    /* 0x5C */ u8 field_0x5C[0x70 - 0x5C];
};

/* FlyerCommonBase (class name from RTTI; vtable 0x80535B0C, with its unit around 0x801938F8): the
 * body of a flying enemy. type is the enemy's character type. */
class FlyerCommonBase : public EnemyFlyerCommon {
  public:
    FlyerCommonBase(int type) : EnemyFlyerCommon(type) {
        field_0x30C = 0;
        field_0x2D8 = 0;
        field_0x2DC = 0.0f;
        field_0x2E0 = 0.0f;
        field_0x2E4 = 0.0f;
        field_0x2E8 = -1.0f;
        field_0x2EC = -1.0f;
        field_0x2F0 = 0.0f;
        field_0x2F8 = 0.0f;
        field_0x2FC = 0.0f;
    }
    virtual ~FlyerCommonBase() {}
    virtual void vfStat_0x38(); /* 0x801938F8 */
    /* New slots (meanings unknown). Inline in the original (weak copies first emitted around
     * 0x80183A58), but only declared here: an inline vf_0x240 adds its constant 20 to the .sdata2
     * of every unit that includes this header. The linker drops it again, but objdiff counts it. */
    virtual bool vf_0x238();  /* 0x80183ACC */
    virtual bool vf_0x23C();  /* 0x80183AC4 */
    virtual float vf_0x240(); /* 0x80183EB4 */
    virtual void vf_0x244();  /* 0x80183AD4 */
    virtual void vf_0x248();  /* 0x80183A58 */

    /* Game/FlyerCommonBase.cpp (names inferred): the point of the path at the end of the attack
     * stretch, one step along the path (state 0) or straight on past its end (state 1), and whether
     * the flyer is inside its attack stretch. The AI states call them. */
    void GetAttackEndPoint(Vec3f *position); /* 0x80193434 */
    void MoveAlongPath(float dt);            /* 0x80193460 */
    bool IsInAttackRange();                  /* 0x80193720 */

    /* 0x268 */ UnkEnemyPath_801A6844 m_path;
    /* 0x2D8 */ s32 field_0x2D8;
    /* 0x2DC */ float field_0x2DC;
    /* 0x2E0 */ float field_0x2E0;
    /* 0x2E4 */ float field_0x2E4;
    /* 0x2E8 */ float field_0x2E8;
    /* 0x2EC */ float field_0x2EC;
    /* 0x2F0 */ float field_0x2F0;
    /* 0x2F4 */ u32 field_0x2F4;
    /* 0x2F8 */ float field_0x2F8;
    /* 0x2FC */ float field_0x2FC;
    /* 0x300 */ u8 field_0x300[0x30C - 0x300];
    /* 0x30C */ u8 field_0x30C;
};

/* FlyerCommonAI (class name from RTTI; vtable 0x8052EECC): the flyers' AI. */
class FlyerCommonAI : public EnemyBaseAI {
  public:
    FlyerCommonAI();
    virtual ~FlyerCommonAI() {} /* weak, first at 0x80180A8C */
    virtual void vfAI_0x58();   /* 0x80183F64 */

    /* 0x74 */ u8 field_0x74[0x8C - 0x74];
};

#endif
