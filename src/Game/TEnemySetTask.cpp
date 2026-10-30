/* TEnemySetTask and TEnemySetBase (class names from RTTI; Game/Enemy.h): the Task that runs an
 * enemy, and the enemy's placement part, which waits until the enemy's resources and stage block
 * are loaded, places the enemy from its placement record, makes it appear and then updates its
 * parts every frame. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
/* The fields of TEnemySetBase (Game/Enemy.h). */
#define ENEMY_SET_BASE_UNIT

#include "Game/Enemy.h"
#include "Game/Gadget.h"
#include "Game/MathTypes.h"
#include "Game/TBlockManage.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

extern "C" {
/* The resource loader and the state of a slot's resources, 3 once loaded (see
 * Game/EnemyParamCommon.cpp and Game/BkLarva.cpp). */
void *fn_800122D4(void);
s32 fn_80012408(void *loader, s32 slot);
/* Whether the stage is ready (Game/StageManager.h). */
u8 fn_80177CF8(void);
/* An angle brought into range (behaviour inferred). */
float fn_8040E99C(float angle);
/* The placement record calls (behaviours inferred; see Game/BkLarva.h and Game/ArkCannon.cpp):
 * register the task with it, drop the task, whether it went away, whether it is being removed,
 * hand it the character info, report the enemy taken or destroyed, and its parameter block. */
void fn_800C9CD0(UnkPlacement_800C9CD0 *owner, Task *task);
void fn_800C9BE8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
void fn_800C97F8(UnkPlacement_800C9CD0 *owner, const boost::shared_ptr<CharInfo> &charInfo);
void fn_800C9AD4(UnkPlacement_800C9CD0 *owner);
void fn_800C9B04(UnkPlacement_800C9CD0 *owner);
void *fn_800C9D30(UnkPlacement_800C9CD0 *owner);
/* Sets the enemy's area up from the placement's parameters (Game/EnemyArea.cpp). */
void fn_801A69FC(UnkEnemyArea_8018A374 *area, const Vec3f *position, const Vec3f *rotation,
                 const void *params);
/* Sets the enemy's fade (Game/Enemy.cpp; behaviour inferred). */
void fn_8019AAD4(Enemy *enemy, float value);
}

/* The interface slots that take the frame time, which Game/Enemy.h declares without it (see
 * Game/EnemyBaseAI.cpp): views of the interfaces, used for those calls only (not original). */
class EnemyStatFuncDt {
  public:
    virtual void vfStat_0x08();
    virtual void vfStat_0x0C();
    virtual void vfStat_0x10();
    virtual void vfStat_0x14(float dt);
    virtual void vfStat_0x18(float dt);
    virtual void vfStat_0x1C(float dt);
};

class EnemyDispFuncDt {
  public:
    /* 0x0 */ Vec3f field_0x0;

    virtual void vfDisp_0x08();
    virtual void vfDisp_0x0C(float dt);
};

class EnemyCtrlFuncDt {
  public:
    /* 0x00 */ Vec3f field_0x0;
    /* 0x0C */ Vec3f field_0xC;

    virtual void vfCtrl_0x08();
    virtual void vfCtrl_0x0C(float dt);
};

class EnemySetFuncDt {
  public:
    virtual bool vfSet_0x08();
    virtual bool vfSet_0x0C();
    virtual void vfSet_0x10();
    virtual void vfSet_0x14();
    virtual void vfSet_0x18();
    virtual void vfSet_0x1C(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x20(UnkPlacement_800C9CD0 *placement);
    virtual void vfSet_0x24(float dt);
};

inline EnemyStatFuncDt *StatDt(Enemy *enemy) {
    return reinterpret_cast<EnemyStatFuncDt *>(enemy->m_status);
}

inline EnemyDispFuncDt *DispDt(Enemy *enemy) {
    return reinterpret_cast<EnemyDispFuncDt *>(enemy->m_disp);
}

inline EnemyCtrlFuncDt *CtrlDt(Enemy *enemy) {
    return reinterpret_cast<EnemyCtrlFuncDt *>(enemy->m_ctrl);
}

inline EnemySetFuncDt *SetDt(Enemy *enemy) {
    return reinterpret_cast<EnemySetFuncDt *>(enemy->m_set);
}

/* The placement record, as slot 0x2C hands it out. */
inline UnkPlacement_800C9CD0 *GetPlacement(TEnemySetBase *set) {
    return reinterpret_cast<UnkPlacement_800C9CD0 *>(set->vfSet_0x2C());
}

/* EnemySetFunc's defaults (Game/Enemy.h; inline, first emitted with Game/Enemy.cpp and the
 * enemy units). Visible here, its vtable and RTTI are emitted with this unit too, which puts its
 * class name ahead of TEnemySetBase's as in retail; the linker keeps the other copies. */
inline bool EnemySetFunc::vfSet_0x0C() {
    return 0;
}
inline void EnemySetFunc::vfSet_0x10() {}
inline void EnemySetFunc::vfSet_0x14() {}
inline void EnemySetFunc::vfSet_0x18() {}
inline void EnemySetFunc::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {}
inline void EnemySetFunc::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {}
inline void EnemySetFunc::vfSet_0x24() {}
inline bool EnemySetFunc::vfSet_0x28() {
    return 0;
}
inline int EnemySetFunc::vfSet_0x2C() {
    return 0;
}

/* The flag slots (inline; weak copies in Game/EggPawn.cpp). Inline here too, so that slot 0x14 is
 * the first one defined out of line and the vtable is emitted with this unit. */
inline bool TEnemySetBase::vfSet_0x08() {
    return field_0x14;
}

inline bool TEnemySetBase::vfSet_0x0C() {
    return field_0x15;
}

inline void TEnemySetBase::vfSet_0x10() {
    field_0x14 = true;
}

inline bool TEnemySetBase::vfSet_0x28() {
    return m_state >= 3;
}

TEnemySetBase::TEnemySetBase() : m_state(1), m_block() {
    field_0x14 = 0;
    field_0x15 = 0;
}

TEnemySetBase::~TEnemySetBase() {}

void TEnemySetBase::vfSet_0x1C(UnkPlacement_800C9CD0 *placement) {}

extern "C" void fn_801A724C(TEnemySetBase *set, UnkPlacement_800C9CD0 *placement);

void TEnemySetBase::vfSet_0x20(UnkPlacement_800C9CD0 *placement) {
    fn_801A724C(this, placement);
}

/* Sets the enemy's position and home rotation (names inferred). */
extern "C" void fn_801A735C(Enemy *enemy, const Vec3f *position) {
    fn_800091D0(&enemy->field_0x48, position);
}

extern "C" void fn_801A7340(Enemy *enemy, const Vec3f *rotation) {
    enemy->field_0x60 = *rotation;
}

/* Places the enemy from its placement record (name inferred). */
extern "C" void fn_801A724C(TEnemySetBase *set, UnkPlacement_800C9CD0 *placement) {
    Vec3f position;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    fn_800C9F14(placement, &position);
    fn_800C9ED4(placement, &rotation);
    rotation.x = fn_8040E99C(rotation.x);
    rotation.y = fn_8040E99C(rotation.y);
    rotation.z = fn_8040E99C(rotation.z);
    fn_801A735C(set->m_enemy, &position);
    fn_801A7340(set->m_enemy, &rotation);
    fn_800091D0(set->m_enemy->GetPosition(), &position);
    *set->m_enemy->GetRotation() = rotation;
    void *params = fn_800C9D30(placement);
    fn_801A69FC(&set->m_enemy->field_0xA8, &position, &rotation, params);
}

/* Slot 0x24, the per-frame update: it takes the frame time, which Game/Enemy.h does not declare,
 * so it is defined under its symbol (as in Game/EnemyBaseAI.cpp). */
extern "C" void vfSet_0x24__13TEnemySetBaseFv(TEnemySetBase *set, float dt) {
    switch (set->m_state) {
    case 1:
        if (fn_80012408(fn_800122D4(), set->m_enemy->m_motionBase) != 3 || !fn_80177CF8()) {
            break;
        }
        {
            Vec3f position;
            fn_800C9F14(GetPlacement(set), &position);
            set->m_block = fn_8003A154(&position);
        }
        set->m_state = 2;
    case 2: {
        bool ready = false;
        if (set->m_block.get() != 0) {
            if (!fn_80039EDC(boost::weak_ptr<TBlockManage::forBlockCheck>(set->m_block))) {
                ready = true;
            }
        }
        if (!ready) {
            break;
        }
        set->m_block.reset();
        set->m_enemy->m_set->vfSet_0x20(reinterpret_cast<UnkPlacement_800C9CD0 *>(set->vfSet_0x2C()));
        set->m_enemy->m_disp->vfDisp_0x08();
        set->m_enemy->m_status->vfStat_0x08();
        set->m_enemy->m_ctrl->vfCtrl_0x08();
        set->m_enemy->m_move->vfMove_0x08();
        set->m_state = 3;
    }
    case 3:
        set->m_enemy->m_status->vfStat_0x38();
        if (set->m_enemy->m_status->vfStat_0x34()) {
            set->m_enemy->field_0xF8 = *reinterpret_cast<u32 *>(&lbl_8057D6A0[0x90]);
            set->m_enemy->m_status->vfStat_0x0C();
            set->m_state = 4;
        }
        break;
    case 4:
        StatDt(set->m_enemy)->vfStat_0x10();
        StatDt(set->m_enemy)->vfStat_0x14(dt);
        CtrlDt(set->m_enemy)->vfCtrl_0x0C(dt);
        StatDt(set->m_enemy)->vfStat_0x18(dt);
        DispDt(set->m_enemy)->vfDisp_0x0C(dt);
        set->m_enemy->m_move->vfMove_0x0C(dt);
        set->m_enemy->m_disp->vfDisp_0x10(dt);
        StatDt(set->m_enemy)->vfStat_0x1C(dt);
        break;
    }
}

void TEnemySetBase::vfSet_0x14() {
    field_0x15 = 1;
    UnkPlacement_800C9CD0 *placement = GetPlacement(this);
    if (placement != 0) {
        fn_800C9B04(placement);
    }
}

void TEnemySetBase::vfSet_0x18() {
    field_0x15 = 1;
    UnkPlacement_800C9CD0 *placement = GetPlacement(this);
    if (placement != 0) {
        fn_800C9AD4(placement);
    }
}

TEnemySetTask::TEnemySetTask(Task *parent, UnkPlacement_800C9CD0 *placement) : Task(parent) {
    field_0x2C = 0;
    if (placement != 0) {
        field_0x2C = placement;
        fn_800C9CD0(field_0x2C, this);
        boost::shared_ptr<CharInfo> info = fn_8017D014(m_enemy).lock();
        fn_800C97F8(field_0x2C, boost::shared_ptr<CharInfo>(info));
    }
}

TEnemySetTask::~TEnemySetTask() {
    if (field_0x2C != 0 && !m_enemy->m_set->vfSet_0x0C()) {
        fn_800C9BE8(field_0x2C);
        field_0x2C = 0;
    }
}

void TEnemySetTask::Tick(float) {
    float dt = m_enemy->GetFrameTime();
    if (m_enemy->m_set->vfSet_0x08()) {
        m_flags |= 1;
        return;
    }
    float fade = m_enemy->field_0xEC;
    if (field_0x2C != 0) {
        if (fn_800C9A64(field_0x2C)) {
            m_flags |= 1;
            return;
        }
        if (fn_800C996C(field_0x2C)) {
            fade -= 2.0f * dt;
            if (fade < 0.0f) {
                m_flags |= 1;
                return;
            }
        } else if (m_enemy->m_status->vfStat_0x40()) {
            fade -= 2.0f * dt;
            if (fade < 0.0f) {
                fade = 0.0f;
            }
        } else {
            fade += 2.0f * dt;
            if (fade > 1.0f) {
                fade = 1.0f;
            }
        }
    }
    fn_8019AAD4(m_enemy, fade);
    SetDt(m_enemy)->vfSet_0x24(dt);
}
