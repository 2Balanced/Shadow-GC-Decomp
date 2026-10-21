/* EnemyTarget (class name from the RTTI of its shared_ptr control block): up to two Targets that
 * follow model parts of an enemy, so other objects (the lock-on, weapons) can aim at them. The
 * enemy passes a table of (part, flag) pairs; a negative part leaves the slot empty. The targets
 * take a type flag from the enemy's kind. File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at (see Spring.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* Whether the target is active, and switching it (names inferred). */
    bool IsActive() const {
        return m_flags & 1;
    }
    void Activate() {
        m_flags |= 1;
    }
    void Deactivate() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags; /* bit 0: active (inferred) */
};

/* The character info an enemy hands out through fn_8017D014 (class name unknown; it converts to
 * CharInfo without an offset). */
class UnkEnemyInfo_8017D014 : public CharInfo {};

/* The enemy object's kind source at 0x0 (class name unknown). */
class UnkEnemyKind_80319B8C {
  public:
    /* 0 or 1 or 2 (meaning unknown; selects the target flag). */
    virtual int GetKind();
};

/* The enemy's model at 0x8 (class name unknown); its vtable is at 0xC. */
struct UnkEnemyModelBase_80319A3C {
    /* 0x0 */ u8 field_0x0[0xC];
};

class UnkEnemyModel_80319A3C : public UnkEnemyModelBase_80319A3C {
  public:
    virtual void vf08();
    virtual void vf0C();
    virtual void vf10();
    virtual void vf14();
    virtual void vf18();
    virtual void vf1C();
    virtual void vf20();
    /* The matrix of model part index, or null (name inferred). */
    virtual Mtx34f *GetPartMatrix(int index);
};

/* The enemy that owns the targets (class name unknown; only the members used here). */
struct UnkEnemy_8017D014 {
    /* 0x0 */ UnkEnemyKind_80319B8C *m_kind;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ UnkEnemyModel_80319A3C *m_model;
};

/* One target slot of the table the enemy passes (names inferred). */
struct EnemyTargetEntry {
    /* 0x0 */ s32 part; /* negative: no target */
    /* 0x4 */ s32 flags;
};

extern "C" {
/* The enemy's character info (behaviour inferred). */
boost::weak_ptr<UnkEnemyInfo_8017D014> fn_8017D014(UnkEnemy_8017D014 *enemy);
/* Creates a Target for owner (behaviour inferred; the flags are unknown). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
/* The translation of matrix (behaviour inferred). */
void fn_8040FB08(Vec3f *translation, const Mtx34f *matrix);
}

class EnemyTarget {
  public:
    EnemyTarget(UnkEnemy_8017D014 *enemy, const EnemyTargetEntry *entries);
    ~EnemyTarget();

    /* Creates the targets of enemy (name inferred). */
    static boost::shared_ptr<EnemyTarget> Instantiate(UnkEnemy_8017D014 *enemy,
                                                 const EnemyTargetEntry *entries);

    /* Moves the active targets to their model parts (name inferred). */
    void Tick();
    /* Activates and deactivates every target (names inferred). */
    void TurnOn();
    void TurnOff();
    /* Target index, or none (name inferred). */
    boost::shared_ptr<Target> GetTarget(int index);

    /* 0x00 */ UnkEnemy_8017D014 *m_enemy;
    /* 0x04 */ const EnemyTargetEntry *m_entries;
    /* 0x08 */ boost::shared_ptr<Target> m_targets[2];
    /* 0x18 */ s32 m_count;
};
SIZE_ASSERT(EnemyTarget, 0x1C);

/* Deletes the targets (name inferred; see checked_delete below). */
inline void DeleteEnemyTarget(EnemyTarget *target) {
    delete target;
}

/* Retail keeps boost::checked_delete<EnemyTarget> out of line (0x80319F30); see Rocket.cpp and
 * docs/research-notes.md. */
namespace boost {
template<> inline void checked_delete<EnemyTarget>(EnemyTarget *x) {
    DeleteEnemyTarget(x);
}
} // namespace boost

boost::shared_ptr<EnemyTarget> EnemyTarget::Instantiate(UnkEnemy_8017D014 *enemy,
                                                   const EnemyTargetEntry *entries) {
    boost::shared_ptr<EnemyTarget> target =
        boost::shared_ptr<EnemyTarget>(new EnemyTarget(enemy, entries));
    return target;
}

EnemyTarget::EnemyTarget(UnkEnemy_8017D014 *enemy, const EnemyTargetEntry *entries)
    : m_enemy(enemy), m_entries(entries) {
    m_count = 0;
    s32 kindFlag = 0;
    switch (m_enemy->m_kind->GetKind()) {
    case 0:
        kindFlag |= 0x10000;
        break;
    case 2:
        kindFlag |= 0x20000;
        break;
    case 1:
        kindFlag |= 0x40000;
        break;
    }
    for (int i = 0; i < 2; i++) {
        if (m_entries[i].part >= 0) {
            m_targets[i] = fn_801E7128(boost::weak_ptr<CharInfo>(fn_8017D014(m_enemy)),
                                       kindFlag | m_entries[i].flags);
            m_targets[i]->Activate();
            m_count++;
        }
    }
}

EnemyTarget::~EnemyTarget() {
    for (int i = m_count - 1; i >= 0; i--) {
        m_targets[i].reset();
    }
}

void EnemyTarget::Tick() {
    for (int i = 0; i < m_count; i++) {
        boost::shared_ptr<Target> target = m_targets[i];
        if (target.get() != 0 && target->IsActive()) {
            Mtx34f *matrix = m_enemy->m_model->GetPartMatrix(m_entries[i].part);
            if (matrix != 0) {
                Target *t = target.operator->();
                Vec3f position;
                fn_8040FB08(&position, matrix);
                t->fn_800CCE38(&position);
            }
        }
    }
}

void EnemyTarget::TurnOn() {
    for (int i = 0; i < m_count; i++) {
        if (m_targets[i].get() != 0) {
            m_targets[i]->Activate();
        }
    }
}

void EnemyTarget::TurnOff() {
    for (int i = 0; i < m_count; i++) {
        if (m_targets[i].get() != 0) {
            m_targets[i]->Deactivate();
        }
    }
}

boost::shared_ptr<Target> EnemyTarget::GetTarget(int index) {
    if (index >= 0 && index < m_count) {
        return m_targets[index];
    }
    return boost::shared_ptr<Target>();
}
