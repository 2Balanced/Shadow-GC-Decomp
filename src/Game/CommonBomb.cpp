/* @unnamed@CommonBomb_cpp@::Bomb (class name from RTTI): the blast of a bomb, a task named
 * "CommonBomb" that grows a sphere attack collider to its type's radius over its type's time and
 * then ends. Gadgets start one through fn_801DD040 and fn_801DD0BC. The unit begins with the list of
 * characters by kind that fn_800DDF20 returns (class name unknown), which the bomb gadgets search
 * for their targets. File name from the RTTI. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <string.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/AttackIgnoreList.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Task.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

extern "C" {
/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* Starts effect id at position, facing direction (see Game/BirdBase.cpp). */
boost::weak_ptr<EffectElement> fn_801D2AFC(int arg0, const Vec3f *position, const Vec3f *direction,
                                           int id, int arg4);
/* The collider manager, a sphere collider and its attack collision (see Game/BulletCannonBomb.cpp,
 * which explains the order of the arguments). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(float radius, void *manager, const Vec3f *position, u32 flags,
                                              int kind, int arg4, boost::weak_ptr<CharInfo> owner,
                                              const Vec3f *offset);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Sets up an attack collision (see Game/BulletCannonBomb.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int kind, u32 flags, float power, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
}

/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line (boost 1.32's
 * definition; see Game/PlayerBehaviorPickupWeapon.cpp). */
namespace boost {
template <> inline void weak_ptr<CharInfo>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* fn_8040CD74 called as returning a - b by value (see Game/AttackObject.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The list of characters by kind (class name unknown): a pool of 256 entries, eight lists of them
 * by the kind in the character's word at 0x5C, and room for the result of a search. */
struct UnkCharEntry_801DD004 {
    UnkCharEntry_801DD004() : m_next(0), m_prev(0) {}

    bool IsFree() const;
    void Clear() {
        m_info.reset();
    }

    /* 0x0 */ boost::weak_ptr<CharInfo> m_info;
    /* 0x8 */ UnkCharEntry_801DD004 *m_next;
    /* 0xC */ UnkCharEntry_801DD004 *m_prev; /* in a list head: the last entry */
};
SIZE_ASSERT(UnkCharEntry_801DD004, 0x10);

/* A free entry and its index (struct name unknown). */
struct UnkCharSlot_801DCC28 {
    UnkCharEntry_801DD004 *entry;
    int index;
};

class UnkCharManager_800DDF20 {
  public:
    UnkCharManager_800DDF20();
    ~UnkCharManager_800DDF20();

    UnkCharSlot_801DCC28 FindFree(UnkCharEntry_801DD004 *entries, int count);

    /* 0x0000 */ UnkCharEntry_801DD004 m_entries[256];
    /* 0x1000 */ UnkCharEntry_801DD004 m_lists[8];
    /* 0x1080 */ UnkCharEntry_801DD004 m_found[256];
};
SIZE_ASSERT(UnkCharManager_800DDF20, 0x2080);

/* Returns x unchanged (name inferred). Being initialised through a function call, the radii below
 * are set up at startup (see Game/BulletCannonBomb.cpp). */
inline float Plain(float x) {
    return x;
}

namespace {

/* What each blast type does (struct name unknown). */
struct BombParam {
    /* 0x0 */ float radius;
    /* 0x4 */ float duration;
    /* 0x8 */ s32 effect;
};

BombParam s_params[] = {
    {Plain(25.0f), 0.5f, 15},
    {Plain(50.0f), 0.5f, 25},
    {Plain(100.0f), 0.5f, 0},
    {Plain(200.0f), 0.5f, 0x315},
};

class Bomb : public Task {
  public:
    Bomb(Task *parent, const boost::weak_ptr<CharInfo> &owner, const Vec3f *position, int type, u32 ignore,
         float power);
    virtual ~Bomb();
    virtual void Tick(float dt);

    void SetupAttack(const Vec3f *position, boost::weak_ptr<CharInfo> owner, float power);
    void ReleaseCollider();
    void UpdateCollider();
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    /* 0x28 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x30 */ boost::weak_ptr<CharaColliAttack> m_attack;
    /* 0x38 */ Vec3f m_position;
    /* 0x44 */ float m_time;
    /* 0x48 */ s32 m_type;
    /* 0x4C */ u32 m_ignore; /* bit 0: players, bit 1: kind 3, bit 2: kind 4 (meanings inferred) */
};
SIZE_ASSERT(Bomb, 0x50);

Bomb::Bomb(Task *parent, const boost::weak_ptr<CharInfo> &owner, const Vec3f *position, int type, u32 ignore,
           float power)
    : Task(parent) {
    fn_800091EC(&m_position, position);
    m_type = type;
    m_ignore = ignore;
    m_name = "CommonBomb";
    m_time = 0.0f;
    Vec3f direction(0.0f, 0.0f, 0.0f);
    fn_801D2AFC(0, &m_position, &direction, s_params[m_type].effect, 0);
    SetupAttack(&m_position, owner, power);
}

Bomb::~Bomb() {
    ReleaseCollider();
}

void Bomb::Tick(float dt) {
    m_time += dt;
    if (m_time >= s_params[m_type].duration) {
        m_flags |= 1;
    }
    UpdateCollider();
}

void Bomb::SetupAttack(const Vec3f *position, boost::weak_ptr<CharInfo> owner, float power) {
    void *manager = fn_80021B08();
    if (m_collider.get() != 0) {
        ReleaseCollider();
    }
    m_collider = fn_800C7EA8(0.0f, manager, &lbl_805E07A0, 0x100000, 15, 0, owner, &lbl_805E07A0);
    m_attack = fn_800C4374(manager, m_collider);
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack.get() != 0) {
        fn_80054DD0(attack.get(), 4, 0x200, power, boost::bind(&Bomb::OnHit, this, _1),
                    boost::bind(&Bomb::OnAttackEnd, this, _1));
    }
    /* Slot 0x3C, which takes the position here. */
    m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(const_cast<Vec3f *>(position)));
    UpdateCollider();
}

void Bomb::ReleaseCollider() {
    if (m_collider.get() != 0) {
        m_collider.reset();
    }
}

void Bomb::UpdateCollider() {
    if (m_collider.get() != 0) {
        m_collider->SetSize(m_time * s_params[m_type].radius / s_params[m_type].duration, 0.0f, 0.0f, -2);
    }
}

void Bomb::OnHit(AttackCallbackParam &param) {
    if (fn_80053538(reinterpret_cast<const boost::weak_ptr<CharaColliAttack> *>(param.field_0x8))) {
        return;
    }
    boost::shared_ptr<CharaColliAttack> attack;
    attack = reinterpret_cast<const boost::weak_ptr<CharaColliAttack> *>(param.field_0x8)->lock();
    if (attack.get() != 0 && !attack->m_owner.expired()) {
        boost::shared_ptr<CharInfo> info = attack->m_owner.lock();
        if (info.get() != 0) {
            fn_800091D0(reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]),
                        &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&info->m_position, &m_position));
            *reinterpret_cast<float *>(&param.field_0x8[0x4C - 0x8]) = 100.0f;
            u32 ignore = m_ignore;
            s32 kind = info->field_0x5C;
            if ((ignore & 4) && kind == 4) {
                return;
            }
            if ((ignore & 2) && kind == 3) {
                return;
            }
            if ((ignore & 1) && (kind == 1 || kind == 6)) {
                return;
            }
            param.field_0x8[0x30 - 0x8] = 1;
        }
    }
}

void Bomb::OnAttackEnd(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> attack;
    switch (param.m_result) {
    case 1:
        attack = m_attack.lock();
        if (attack.get() != 0) {
            fn_800532F8(attack.get(),
                        *reinterpret_cast<const boost::weak_ptr<CharaColliAttack> *>(param.field_0x8));
        }
        break;
    }
}

} // namespace

/* Starts a blast of type at position for position's character (names inferred); power is passed to
 * the attack collision. */
extern "C" void fn_801DD0BC(const Vec3f *position, int type, float power, u32 ignore) {
    new Bomb(fn_801EB304(fn_8001C4F8(), 11), boost::weak_ptr<CharInfo>(), position, type, ignore, power);
}

/* Starts a blast of type at position for owner (name inferred). */
extern "C" void fn_801DD040(const boost::weak_ptr<CharInfo> &owner, const Vec3f *position, int type, u32 ignore,
                            float power) {
    new Bomb(fn_801EB304(fn_8001C4F8(), 11), owner, position, type, ignore, power);
}

UnkCharManager_800DDF20::UnkCharManager_800DDF20() {
    memset(m_entries, 0, sizeof(m_entries));
    memset(m_lists, 0, sizeof(m_lists));
    for (int i = 0; i < 8; i++) {
        m_lists[i].m_prev = &m_lists[i];
    }
}

UnkCharManager_800DDF20::~UnkCharManager_800DDF20() {}

/* Adds a character to the list of its kind (name inferred); false when the pool is full. The
 * pointer is taken by value (other units declare a const reference, which passes the same way). */
extern "C" bool fn_801DCD8C(UnkCharManager_800DDF20 *manager, boost::shared_ptr<CharInfo> info) {
    UnkCharSlot_801DCC28 slot = manager->FindFree(manager->m_entries, 256);
    if (slot.entry != 0) {
        slot.entry->m_info = info;
        *reinterpret_cast<s32 *>(info->field_0x58) = slot.index; /* the character's entry */
        UnkCharEntry_801DD004 *head = &manager->m_lists[info->field_0x5C];
        UnkCharEntry_801DD004 *last = head->m_prev;
        last->m_next = slot.entry;
        slot.entry->m_prev = last;
        slot.entry->m_next = 0;
        head->m_prev = slot.entry;
        return true;
    }
    return false;
}

/* Takes a character out of its list (name inferred). */
extern "C" void fn_801DCCF4(UnkCharManager_800DDF20 *manager, boost::shared_ptr<CharInfo> info) {
    UnkCharEntry_801DD004 *entry = &manager->m_entries[*reinterpret_cast<s32 *>(info->field_0x58)];
    if (entry->m_next != 0) {
        entry->m_prev->m_next = entry->m_next;
        entry->m_next->m_prev = entry->m_prev;
    } else {
        entry->m_prev->m_next = 0;
        UnkCharEntry_801DD004 *head = &manager->m_lists[info->field_0x5C];
        head->m_prev = entry->m_prev;
    }
    entry->Clear();
}

/* Forgets the result of the last search (name inferred). */
extern "C" void fn_801DCCA8(UnkCharManager_800DDF20 *manager) {
    for (int i = 0; i < 256; i++) {
        manager->m_found[i].m_info.reset();
    }
}

UnkCharSlot_801DCC28 UnkCharManager_800DDF20::FindFree(UnkCharEntry_801DD004 *entries, int count) {
    UnkCharSlot_801DCC28 slot = {0, 0};
    for (int i = 0; i < count; i++, entries++) {
        if (entries->IsFree()) {
            slot.entry = entries;
            slot.index = i;
            return slot;
        }
    }
    return slot;
}

/* The characters of kind (the word at 0x50), linked through m_next; 0 if there are none (name
 * inferred). */
extern "C" UnkCharEntry_801DD004 *fn_801DCB14(UnkCharManager_800DDF20 *manager, int kind) {
    UnkCharEntry_801DD004 *entry = manager->m_entries;
    int count = 0;
    for (int i = 0; i < 256; i++, entry++) {
        if (!entry->IsFree()) {
            boost::shared_ptr<CharInfo> info = entry->m_info.lock();
            if (kind == info->GetKind()) {
                if (count > 0) {
                    manager->m_found[count].m_info = entry->m_info;
                    manager->m_found[count].m_prev = &manager->m_found[count - 1];
                    manager->m_found[count].m_next = 0;
                    manager->m_found[count - 1].m_next = &manager->m_found[count];
                } else {
                    manager->m_found[count].m_info = entry->m_info;
                    manager->m_found[count].m_prev = 0;
                    manager->m_found[count].m_next = 0;
                }
                count++;
            }
        }
    }
    return count > 0 ? manager->m_found : 0;
}

/* Whether the entry holds no character (name inferred). */
bool UnkCharEntry_801DD004::IsFree() const {
    return m_info.expired();
}
