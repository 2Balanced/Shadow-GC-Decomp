/* WeaponLockonBase (class name from RTTI: Weapon::WeaponLockonBase): a weapon that locks on to
 * targets before it fires. Its search (class name unknown) runs a cone search of the target
 * manager's targets from the muzzle each frame, holds the targets found as LockonTargets (class name
 * from RTTI) with a mark each (Weapon::LockonMark, Game/WeaponLockonMark.cpp), and fires at them one
 * by one through the weapon's slot 0x34. Other units call the functions without class names by
 * their addresses. File name from the class name; the other names are inferred.
 *
 * NonMatching: every function's code matches, but not the unit's function order. fn_8006127C
 * (clear a flag bit), which about 70 other units call by its address name, sits in retail among the
 * template functions the unit emits at its end, so it was a member of a class template there; a
 * C-linkage definition is emitted with the unit's own functions instead. The order of the
 * out-of-line copies of inline functions is not arranged either. */
#define WEAPON_LOCKON_TARGET_MEMBERS
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_ERASE_ONE
#define MSL_VECTOR_INDEX
#define MSL_ALGORITHM_COPY
#define BOOST_SP_RESET_POINTER
#include <algorithm>
#include <vector>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/DebugDisp.h"
#include "Game/GadgetUtil.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/TargetSearchList.h"
#include "Game/Task.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponLockonBase.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::LockonTarget;
using Weapon::WeaponLockonBase;

/* Target (class name from RTTI): what the lock-on finds. Only what is used here is declared: the
 * character at 0x28 and the flags at 0x30 (bit 0: it can be targeted; see Game/BAGunShip.cpp). */
class Target {
  public:
    bool IsEnabled() {
        return m_flags & 1;
    }

    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x30 */ u32 m_flags;
};

namespace boost {
/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line (boost 1.32's
 * definition). */
template <> inline void weak_ptr<Target>::reset() {
    this_type().swap(*this);
}
} // namespace boost

extern "C" {
/* A 32-bit flag word (see Game/BKTankController.h): construct cleared, set bit, test bit. */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Whether a target is alive and can be targeted (behaviour inferred). */
bool fn_801E7094(const boost::weak_ptr<Target> *target);
/* Whether task and all its parents are still running (see Game/WeaponDarkHammer.cpp). */
bool fn_8004EAC0(Task *task);
/* The translation of a matrix, a point taken through a matrix, and a normalised vector. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CEE8(const Vec3f *vector);
/* Copies a weapon's position and rotation out (Game/WeaponBase.cpp). */
void fn_8005CF90(const Weapon::WeaponBase *weapon, Vec3f *position, Vec3f *rotation);
/* Registers a static object's destructor (Runtime). */
void __register_global_object(void *object, void *destructor, void *link);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The target manager, once built. */
extern UnkTargetManager_80060058 *lbl_805EE200;

/* The target manager (a 0x14-byte object; class name unknown). */
class UnkTargetManager_80060058 {
  public:
    /* 0x801E7C58 and 0x801E7C08. */
    UnkTargetManager_80060058();
    ~UnkTargetManager_80060058();

    /* 0x00 */ u8 field_0x0[0x14];
};

/* The target manager object: a function-local static, built on first use. */
inline UnkTargetManager_80060058 &TargetManagerObject() {
    static UnkTargetManager_80060058 s_manager;
    return s_manager;
}

/* The target manager (other units call it by its address). It calls TargetManagerObject, so it
 * stays out of line. */
extern "C" inline UnkTargetManager_80060058 *fn_80060058(void) {
    if (lbl_805EE200 == 0) {
        lbl_805EE200 = &TargetManagerObject();
    }
    return lbl_805EE200;
}

namespace Weapon {

/* LockonMark (class name from RTTI; Game/WeaponLockonMark.cpp): the mark a LockonTarget shows on
 * its target. It clears the slot it is given when it dies. */
class LockonMark : public Task {
  public:
    LockonMark(LockonMark **slot, LockonTarget *target);

    /* Forgets the slot (0x80061D04), and moves the mark (0x80061D10). */
    void fn_80061D04();
    void fn_80061D10(const Vec3f *position);
    /* Ends the task. */
    void Kill() {
        m_flags |= 1;
    }

    /* 0x28 */ void *m_impl;
    /* 0x2C */ LockonMark **m_slot;
};

inline void LockonTarget::ResetTarget() {
    m_target.reset();
}

inline void LockonTarget::ReleaseMark() {
    if (m_mark != 0) {
        m_mark->fn_80061D04();
        m_mark->Kill();
        m_mark = 0;
    }
}

inline void LockonTarget::CreateMark() {
    new LockonMark(&m_mark, this);
}

inline void LockonTarget::UpdateMark(const Vec3f &position) {
    if (m_mark != 0) {
        if (!fn_8004EAC0(m_mark)) {
            m_mark = 0;
        } else {
            m_mark->fn_80061D10(&position);
        }
    }
}

} // namespace Weapon

/* A flag word (see Game/BKTankController.h; struct name unknown). */
struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit);
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* Clears bit (other units call it by its address). */
extern "C" void fn_8006127C(u32 *flags, int bit);

inline void UnkFlags_80014E04::Clear(int bit) {
    fn_8006127C(&m_bits, bit);
}

namespace Weapon {

/* What the search hands a weapon's own search function (struct name unknown): the weapon, its
 * targets, the search line and range. */
struct UnkLockonQuery_80060510 {
    UnkLockonQuery_80060510(WeaponLockonBase *weapon, class UnkLockonTargets_80060D3C *targets,
                            Sonicteam::System::Geometry::Line *line, float range)
        : m_weapon(weapon), m_targets(targets), m_line(line), m_range(range) {}

    /* 0x0 */ WeaponLockonBase *m_weapon;
    /* 0x4 */ class UnkLockonTargets_80060D3C *m_targets;
    /* 0x8 */ Sonicteam::System::Geometry::Line *m_line;
    /* 0xC */ float m_range;
};

/* A weapon's own search function (class name unknown): when set, it replaces the cone search. */
class UnkLockonFilter_80060510 {
  public:
    UnkLockonFilter_80060510() {}

    bool IsSet() {
        return HasFunction();
    }
    bool HasFunction() {
        return !m_function.empty();
    }
    bool Search(const UnkLockonQuery_80060510 &query) {
        if (m_function.empty()) {
            return false;
        }
        return m_function(query);
    }

    /* 0x0 */ boost::function1<bool, const UnkLockonQuery_80060510 &> m_function;
};

/* The targets held (class name unknown): their LockonTargets, and the weapon told of each new one. */
class UnkLockonTargets_80060D3C {
  public:
    UnkLockonTargets_80060D3C() {
        m_targets.reserve(8);
        m_weapon = 0;
    }

    int Count() {
        return m_targets.size();
    }
    void Clear() {
        m_targets.clear();
    }
    boost::shared_ptr<LockonTarget> At(int index) {
        return m_targets[index];
    }

    /* Moves the marks to position, drops the targets that can no longer be targeted, takes the
     * targets held out of list, and holds a new target. */
    void UpdateMarks(const Vec3f &position);
    void DropLost();
    void Exclude(boost::shared_ptr<TargetSearchList> list);
    void Hold(const boost::weak_ptr<Target> &target);

    /* 0x0 */ std::vector<boost::shared_ptr<LockonTarget> > m_targets;
    /* 0xC */ WeaponLockonBase *m_weapon;
};
SIZE_ASSERT(UnkLockonTargets_80060D3C, 0x10);

/* The search (class name unknown). Flag bit 0: locking on; bit 1: searching this frame; bit 2:
 * firing. */
class UnkLockonSearch_8006065C {
  public:
    UnkLockonSearch_8006065C(int count);

    void SetColor(const UnkColor_8040D2D8 *color);
    void SetLine(const Mtx34f *muzzle, float range);
    int Count();
    void Cancel();
    void Tick(float dt);
    void BeginFrame();
    void SetWeapon(WeaponLockonBase *weapon);
    /* Holds the target nearest the line in the cone. */
    void SearchTarget();

    void AddShots(int n) {
        m_shots += n;
        m_fired++;
    }
    int Shots() {
        return m_shots;
    }
    int MaxCount() {
        return m_count;
    }
    void StartLocking() {
        fn_80014E1C(&m_flags.m_bits, 0);
    }
    void StopLocking() {
        fn_8006127C(&m_flags.m_bits, 0);
    }
    void StartFiring() {
        fn_80014E1C(&m_flags.m_bits, 2);
    }

    /* 0x00 */ WeaponLockonBase *m_weapon;
    /* 0x04 */ UnkFlags_80014E04 m_flags;
    /* 0x08 */ Sonicteam::System::Geometry::Line m_line;
    /* 0x20 */ float m_range;
    /* 0x24 */ UnkColor_8040D2D8 m_color;
    /* 0x28 */ UnkLockonTargets_80060D3C m_targets;
    /* 0x38 */ UnkLockonFilter_80060510 m_filter;
    /* 0x44 */ s32 m_count;  /* the most targets held */
    /* 0x48 */ s32 m_shots;  /* the shots fired this frame */
    /* 0x4C */ s32 m_fired;  /* the targets fired at this round */
    /* 0x50 */ float m_reload;
    /* 0x54 */ float m_searchWait;
};
SIZE_ASSERT(UnkLockonSearch_8006065C, 0x58);

/* LockonTarget's functions. */

LockonTarget::LockonTarget() {
    m_mark = 0;
}

inline LockonTarget::~LockonTarget() {
    ResetTarget();
    ReleaseMark();
}

} // namespace Weapon

/* Whether the lock-on's target can still be targeted. */
extern "C" bool fn_80061110(LockonTarget *lockon) {
    return fn_801E7094(&lockon->m_target);
}

/* Moves the lock-on's mark to position. */
extern "C" void fn_800610B4(LockonTarget *lockon, const Vec3f *position) {
    lockon->UpdateMark(*position);
}

/* The lock-on's target. */
extern "C" const boost::weak_ptr<Target> *fn_80061050(LockonTarget *lockon) {
    return &lockon->m_target;
}

namespace Weapon {

void LockonTarget::fn_80060F70() {
    CreateMark();
}

} // namespace Weapon

/* Tells the lock-on's target that attacker is attacking it. */
extern "C" void fn_80060E58(LockonTarget *lockon, const boost::weak_ptr<CharInfo> &attacker) {
    if (!lockon->m_target.expired()) {
        boost::shared_ptr<CharInfo> chara = lockon->m_target.lock()->m_chara.lock();
        if (chara.get() != 0) {
            chara->PushCommand(new AnnounceAttackCommand(attacker, 3, 4));
        }
    }
}

namespace Weapon {

/* The targets' functions. */

void UnkLockonTargets_80060D3C::Hold(const boost::weak_ptr<Target> &target) {
    if (m_weapon != 0) {
        m_weapon->vf_0x30();
    }
    boost::shared_ptr<LockonTarget> lockon = boost::shared_ptr<LockonTarget>(new LockonTarget());
    lockon->SetTarget(target);
    lockon->CreateMark();
    m_targets.push_back(lockon);
}

void UnkLockonTargets_80060D3C::Exclude(boost::shared_ptr<TargetSearchList> list) {
    for (std::vector<boost::shared_ptr<LockonTarget> >::iterator it = m_targets.begin(); it != m_targets.end();
         ++it) {
        fn_801E7E48(list.get(), (*it)->m_target);
    }
}

void UnkLockonTargets_80060D3C::DropLost() {
    std::vector<boost::shared_ptr<LockonTarget> >::iterator it = m_targets.begin();
    while (it != m_targets.end()) {
        boost::shared_ptr<Target> target = (*it)->m_target.lock();
        if (target.get() == 0 || !target->IsEnabled()) {
            it = m_targets.erase(it);
        } else {
            ++it;
        }
    }
}

void UnkLockonTargets_80060D3C::UpdateMarks(const Vec3f &position) {
    for (std::vector<boost::shared_ptr<LockonTarget> >::iterator it = m_targets.begin(); it != m_targets.end();
         ++it) {
        (*it)->UpdateMark(position);
    }
}

/* The search's functions. */

UnkLockonSearch_8006065C::UnkLockonSearch_8006065C(int count) : m_count(count) {
    m_color = UnkColor_8040D2D8(200, 0xFF, 0, 0);
}

void UnkLockonSearch_8006065C::SetWeapon(WeaponLockonBase *weapon) {
    m_weapon = weapon;
    m_targets.m_weapon = m_weapon;
}

void UnkLockonSearch_8006065C::BeginFrame() {
    m_shots = 0;
    m_targets.DropLost();
}

void UnkLockonSearch_8006065C::Tick(float dt) {
    if (m_searchWait >= 0.0f) {
        m_searchWait -= dt;
    }
    if (m_flags.Test(0) && !m_flags.Test(2)) {
        m_flags.Set(1);
    } else {
        m_flags.Clear(1);
    }
    if (m_flags.Test(1)) {
        if (m_color.r != 0) {
            fn_802E0B58(&m_line, reinterpret_cast<u32 *>(&m_color));
        }
        if (Count() < m_count && m_searchWait <= 0.0f) {
            if (m_filter.IsSet()) {
                UnkLockonQuery_80060510 query(m_weapon, &m_targets, &m_line, m_range);
                if (m_filter.Search(query)) {
                    m_searchWait = 0.1f;
                }
            } else {
                SearchTarget();
            }
        }
    }
    if (m_reload > 0.0f) {
        m_reload -= dt;
        if (m_reload < 0.0f) {
            m_reload = 0.0f;
        }
    }
    if (Count() <= 0) {
        if (m_flags.Test(2) && m_fired == 0 && m_reload <= 0.0f) {
            m_reload = m_weapon->vf_0x34(boost::shared_ptr<LockonTarget>(), m_fired);
        } else {
            m_flags.Clear(2);
            m_fired = 0;
            m_reload = 0.0f;
            m_searchWait = 0.0f;
        }
    } else if (m_flags.Test(2)) {
        int index;
        while (m_reload <= 0.0f && (index = m_fired) < Count()) {
            boost::shared_ptr<LockonTarget> target = m_targets.At(index);
            m_reload = m_weapon->vf_0x34(target, m_fired);
        }
    }
    if (m_fired >= Count()) {
        m_flags.Clear(2);
        m_targets.Clear();
    }
    Vec3f position;
    fn_8005CF90(m_weapon, &position, 0);
    m_targets.UpdateMarks(position);
}

void UnkLockonSearch_8006065C::Cancel() {
    m_flags.Clear(0);
    m_targets.Clear();
}

int UnkLockonSearch_8006065C::Count() {
    return m_targets.Count();
}

void UnkLockonSearch_8006065C::SetLine(const Mtx34f *muzzle, float range) {
    fn_800091D0(&m_line.m_start, &(const Vec3f &)fn_8040FB08(muzzle));
    fn_800091D0(&m_line.m_end, &(const Vec3f &)fn_8040B67C(muzzle, &(const Vec3f &)Vec3f(0.0f, 0.0f, range)));
    m_range = range;
}

void UnkLockonSearch_8006065C::SetColor(const UnkColor_8040D2D8 *color) {
    m_color = *color;
}

void UnkLockonSearch_8006065C::SearchTarget() {
    boost::shared_ptr<TargetSearchList> list;
    list.reset(fn_801E7930(fn_80060058()));
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(
                                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_line.m_end, &m_line.m_start)));
    fn_801E80C0(list.get(), 0x1800);
    fn_801E7FB4(list.get(), &m_line.m_start, 0.93969f, &direction);
    fn_801E8218(list.get(), &m_line.m_start, m_range);
    m_targets.Exclude(list);
    boost::weak_ptr<Target> target = fn_801E848C(list.get(), &m_line.m_start);
    boost::shared_ptr<Target> locked = target.lock();
    if (locked.get() != 0) {
        m_targets.Hold(target);
        m_searchWait = 0.1f;
    }
}

/* WeaponLockonBase's functions. */

WeaponLockonBase::WeaponLockonBase(const boost::weak_ptr<CharInfo> &owner, int count, int type)
    : WeaponBase(owner, type, 3), m_search(new UnkLockonSearch_8006065C(count)) {
    m_search->SetWeapon(this);
}

WeaponLockonBase::~WeaponLockonBase() {}

void WeaponLockonBase::fn_8005FAF0() {
    m_search->BeginFrame();
}

void WeaponLockonBase::fn_8005FACC(float dt) {
    m_search->Tick(dt);
}

} // namespace Weapon

extern "C" {

/* Fires at the targets held: their marks start the round, and the time until the next one. */
float fn_8005FA98(WeaponLockonBase *weapon) {
    weapon->m_search->StartFiring();
    return 0.25f;
}

/* Starts and stops locking on, and cancels it. */
void fn_8005FA68(WeaponLockonBase *weapon) {
    weapon->m_search->StartLocking();
}

void fn_8005FA38(WeaponLockonBase *weapon) {
    weapon->m_search->StopLocking();
}

void fn_8005FA14(WeaponLockonBase *weapon) {
    weapon->m_search->Cancel();
}

} // extern "C"

namespace Weapon {

int WeaponLockonBase::fn_8005F9EC() {
    return m_search->MaxCount();
}

int WeaponLockonBase::fn_8005F9C8() {
    return m_search->Count();
}

} // namespace Weapon

extern "C" {

/* The shots fired this frame, and whether the weapon is searching. */
int fn_8005F9A0(WeaponLockonBase *weapon) {
    return weapon->m_search->Shots();
}

bool fn_8005F8C8(WeaponLockonBase *weapon) {
    return fn_80014E34(&weapon->m_search->m_flags.m_bits, 1);
}

} // extern "C"

namespace Weapon {

void WeaponLockonBase::fn_8005F878(int n) {
    m_search->AddShots(n);
}

void WeaponLockonBase::fn_8005F854(const Mtx34f *muzzle, float range) {
    m_search->SetLine(muzzle, range);
}

} // namespace Weapon

/* Sets the colour of the target marks' line. */
extern "C" void fn_8005F830(WeaponLockonBase *weapon, const UnkColor_8040D2D8 *color) {
    weapon->m_search->SetColor(color);
}

extern "C" void fn_8006127C(u32 *flags, int bit) {
    *flags &= ~(1 << bit);
}
