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
    /* The timer of the attack stretch: the float at 0x2F0 when vf_0x238 and vf_0x23C hold, else 0
     * (weak copy first at 0x80183A5C; name inferred). */
    float GetAttackTime() {
        if (vf_0x238() && vf_0x23C()) {
            return field_0x2F0;
        }
        return 0.0f;
    }
    /* Sets both floats at 0x2F8 (name inferred; the AI states set them to 1 as the flyer starts
     * along its path). */
    void SetMoveRate(float rate) {
        field_0x2F8 = rate;
        field_0x2FC = rate;
    }

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

/* The flyers' states (class names from RTTI): following the path and floating in place. Behind
 * their own macro for the reason given at ENEMY_BASE_AI_STATES (Game/EnemyAI.h): only the units
 * that use them define it. Their code is in FlyerCommonAI's unit (0x801838FC..), their destructors
 * are inline (weak copies first emitted with Game/BkWingLargeAI.cpp). */
#ifdef FLYER_COMMON_AI_STATES
#include "Game/EnemyAIState.h"

class FlyerCommonAI;

class EnemyAIState_FlyerCommonAI_MoveOnPath : public TEnemyAIState {
  public:
    EnemyAIState_FlyerCommonAI_MoveOnPath(); /* 0x801840A0 */
    virtual ~EnemyAIState_FlyerCommonAI_MoveOnPath() {}
    virtual void SetOwner(EnemyBaseAI *ai); /* 0x80184054 */
    virtual void Enter();                   /* 0x80183AD8 */
    virtual void Update();                  /* 0x80183910 */
    virtual void Exit();                    /* 0x801838FC */

    /* 0x4 */ FlyerCommonAI *m_ai;
};

class EnemyAIState_FlyerCommonAI_WaitFloating : public TEnemyAIState {
  public:
    EnemyAIState_FlyerCommonAI_WaitFloating(); /* 0x80184110 */
    virtual ~EnemyAIState_FlyerCommonAI_WaitFloating() {}
    virtual void SetOwner(EnemyBaseAI *ai); /* 0x801840C4 */
    virtual void Enter();                   /* 0x80183EBC */
    virtual void Update();                  /* 0x80183B7C */
    virtual void Exit();                    /* 0x80183B78 */

    /* 0x4 */ FlyerCommonAI *m_ai;
};
#endif

/* FlyerCommonAI (class name from RTTI; vtable 0x8052EECC): the flyers' AI. */
class FlyerCommonAI : public EnemyBaseAI {
  public:
    FlyerCommonAI();
    virtual ~FlyerCommonAI() {} /* weak, first at 0x80180A8C */
    virtual void vfAI_0x58();   /* 0x80183F64 */

#ifdef FLYER_COMMON_AI_STATES
    /* The states' objects (names inferred), declared last state first (see EnemyWalkerAI). */
    static TEnemyAIState *GetMoveOnPathState() {
        static EnemyAIState_FlyerCommonAI_MoveOnPath s_state;
        return &s_state;
    }
    static TEnemyAIState *GetWaitFloatingState() {
        static EnemyAIState_FlyerCommonAI_WaitFloating s_state;
        return &s_state;
    }
#endif

    /* 0x74 */ FlyerCommonBase *m_flyer; /* the enemy's body (name inferred) */
    /* 0x78 */ u8 m_rising;  /* the float's direction (names inferred) */
    /* 0x7C */ float m_wave;
    /* 0x80 */ Vec3f field_0x80;
};

#endif
