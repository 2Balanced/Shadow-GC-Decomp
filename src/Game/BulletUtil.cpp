/* The bullets' shared code (no class of its own; file name inferred): the starting state of a shot,
 * the attack collision a bullet carries, the collision tests of a bullet's move (with its attack
 * collision, or for a player, or the laser's, which can stop at what it meets), and the line test's
 * filter. This unit is the first to emit CharaColliAttack's inline constructor and destructor, the
 * functions of its ignore list and pool, and boost's control block of a shared_ptr to it. Other
 * units call most of the functions here by their addresses. Names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * Not yet matching: the three contact loops (fn_8005A9D0, fn_8005AFE4 and fn_8005B2F0) have
 * retail's instructions and stack layout, but GC/2.7 gives their callee-saved registers out in a
 * different order (the contact's object, the node and the contact list trade registers; about
 * thirty source shapes tried, among them every declaration order of those variables). Everything
 * else, including the data, matches, and linking this unit changes only those three functions. */
#define ATTACK_IGNORE_LIST_MEMBERS
#define CHARA_COLLI_ATTACK_MEMBERS
#include "Game/AttackIgnoreList.h"

#include <boost/function.hpp>

#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;
using Weapon::ShotInfo;
using Weapon::Bullet::BulletPosition;
using Weapon::Bullet::HitResult;

/* A line between two points for the collision test (see Game/BlackBullCtrlUnit.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A collider the line test meets (struct name unknown): the collider, its kind flags, and the data
 * the test was given. */
struct UnkLineObject_8005B5A0 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_flags;
    /* 0x10 */ void *m_data;
};

/* What the line test's filter is given: the player whose colliders a bullet passes through, -1 for
 * none (struct name unknown). */
struct UnkLineFilter_8005B5A0 {
    UnkLineFilter_8005B5A0() : m_player(-1) {}

    s32 m_player;
};

/* A node of a contact list: the collider at 0x0 and the hit's object at 0x14 (see
 * Game/BulletHoming.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x08 */ u8 field_0x8[0x14 - 0x8];
    /* 0x14 */ void *m_object;
};

struct UnkColliContacts_8001EE04;

/* An iterator over a contact list (see Game/BombServer.cpp). */
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

extern "C" {
/* Sets up an attack collision: its kind, flags and power, and its callbacks
 * (Game/BulletCannonBomb.cpp). The callbacks are passed by value. */
void fn_80054DD0(CharaColliAttack *attack, int kind, u32 flags, float power, AttackCallback onHit,
                 AttackCallback onEnd);
/* The collision world, a line between two points and the contacts of what a line meets (see
 * Game/BulletHoming.cpp). */
void *fn_80021B08(void);
void *fn_80020D40(void);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
UnkColliContacts_8001EE04 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int kind,
                                       bool (*filter)(const UnkLineObject_8005B5A0 *object), void *data);
void fn_80026FB4(UnkColliContacts_8001EE04 *contacts, int arg1);
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
u32 fn_80026588(UnkColliContactNode_800587A0 *node);
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
bool fn_800C3ED4(void *manager, UnkColliContactNode_800587A0 *node);
void fn_800C40C4(void *manager, boost::weak_ptr<CharaColliAttack> attack, UnkColliContactNode_800587A0 *node,
                 HitResult *result);
/* The collider of a contact, and the character a collider is attached to (Game/BulletHoming.cpp). */
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CDA8(const Vec3f &vector, float scale); /* vector * scale */
Vec3f fn_8040CEE8(const Vec3f &vector);              /* normalised */
float fn_8040CF90(const Vec3f &a, const Vec3f &b);   /* a . b */
Vec3f fn_800091A4(const Vec3f &vector, float scale); /* vector * scale */
void fn_8040CE04(Vec3f *vector, const Vec3f &value); /* vector -= value */
/* A direction turned around, and into the angles of a rotation (see Game/BulletLaser.cpp). */
Vec3f fn_80016284(const Vec3f &direction);
Vec2f fn_8040F760(const Vec3f &direction);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A rotation made of the angles about x and y that a direction gives (struct name unknown; see
 * Game/BulletCannon.cpp). */
struct UnkRotation {
    UnkRotation(const Vec2f &angles) : x(angles.x), y(angles.y), z(0.0f) {}

    float x;
    float y;
    float z;
};

/* metres in the game's units, ten to a metre (name inferred). */
inline float MetresToUnits(float metres) {
    return metres * 10.0f;
}

/* The same (other units call it by its address). It calls MetresToUnits, so it stays out of
 * line. */
extern "C" inline float fn_8005AE20(float metres) {
    return MetresToUnits(metres);
}

/* The attack collision of a collider attached to a character, at 0x38 (name inferred). */
inline boost::weak_ptr<CharaColliAttack> AttackOf(CharaColliBaseCharInfo *collider) {
    return *reinterpret_cast<boost::shared_ptr<CharaColliAttack> *>(reinterpret_cast<u8 *>(collider) + 0x38);
}

/* Whether attack ignores other (name inferred). */
inline bool IsIgnoring(CharaColliAttack *attack, boost::weak_ptr<CharaColliAttack> other) {
    if (fn_80053538(&other)) {
        return true;
    }
    for (UnkAttackIgnoreList_80053340::iterator it = attack->field_0x44.Head(); it != attack->field_0x44.End(); it++) {
        if (!fn_80053538(&*it)) {
            if ((*it).lock().get() == other.lock().get()) {
                return true;
            }
        }
    }
    return false;
}

/* The line test's filter: whether the test looks at object. It skips the kinds of colliders in
 * 0xC0018 and, given a player, that player's own colliders (name inferred). */
static bool LineFilter(const UnkLineObject_8005B5A0 *object) {
    UnkLineFilter_8005B5A0 *filter = static_cast<UnkLineFilter_8005B5A0 *>(object->m_data);
    if ((object->m_flags & 0xC0018) != 0) {
        return false;
    }
    if (object->m_collider != 0) {
        CharaColliBaseCharInfo *collider = dynamic_cast<CharaColliBaseCharInfo *>(object->m_collider);
        if (collider != 0) {
            boost::shared_ptr<CharInfo> owner = fn_800586EC(collider).lock();
            if (owner.get() != 0) {
                s32 player = filter->m_player;
                if (player != -1 && player == owner->field_0x54) {
                    return false;
                }
            }
        }
    }
    return true;
}

/* What a bullet fired with info hits on the line from its owner to where it starts, for the
 * player filter (other units call it by its address). */
extern "C" HitResult fn_8005B2F0(int kind, const Vec3f *from, const Vec3f *to, s32 player);

extern "C" HitResult fn_8005B490(const ShotInfo &info, int kind) {
    if (info.m_owner.expired()) {
        return HitResult(false);
    }
    boost::shared_ptr<CharInfo> owner = info.m_owner.lock();
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8((const Vec3f &)info.m_direction));
    /* The start, moved back to the owner's position across the direction. */
    float along = fn_8040CF90(((VectorSubtractFn)fn_8040CD74)((const Vec3f *)&info.m_position, &owner->m_position),
                              direction);
    Vec3f from;
    fn_800091EC(&from, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)((const Vec3f *)&info.m_position,
                                                                        &(const Vec3f &)fn_8040CDA8(direction, along)));
    return fn_8005B2F0(kind, &from, (const Vec3f *)&info.m_position, info.m_owner.lock()->field_0x54);
}

/* What a bullet moving from from to to hits first, for the player filter (other units call it by
 * its address). */
extern "C" HitResult fn_8005B2F0(int kind, const Vec3f *from, const Vec3f *to, s32 player) {
    UnkLine_80020A20 line;
    fn_80020A20(&line, from, to);
    UnkLineFilter_8005B5A0 filter;
    filter.m_player = player;
    UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter);
    if (contacts != 0) {
        HitResult result(false);
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts)); it.m_node != fn_8001EDEC(contacts);
             fn_80027374(&it, 0)) {
            UnkColliContactNode_800587A0 *node = it.m_node;
            void *object = node->m_object;
            AssignVector((Vec3f &)result.m_position, *fn_8001E9BC(object));
            AssignVector((Vec3f &)result.m_normal, *fn_800266D4(object));
            AssignVector((Vec3f &)result.field_0x18, ((VectorSubtractFn)fn_8040CD74)(to, from));
            result.field_0x2C = fn_80026588(node) & 0x0FFFFFFF;
            result.field_0x30 = fn_80026588(node) >> 28;
            if (fn_800C3ED4(fn_80021B08(), node)) {
                result.m_hit = 1;
                result.m_struck = 0;
            } else {
                result.m_hit = 1;
                result.m_struck = 1;
            }
            if (result.m_hit) {
                break;
            }
        }
        fn_80026FB4(contacts, 1);
        return result;
    }
    return HitResult(false);
}

/* What a bullet moving from from to to hits first, with its attack collision (other units call
 * it by its address). */
extern "C" HitResult fn_8005AFE4(int kind, const Vec3f *from, const Vec3f *to,
                                 const boost::shared_ptr<CharaColliAttack> *attack);

/* What a bullet fired with info hits on the line from its owner to where it starts, with its
 * attack collision (other units call it by its address). */
extern "C" HitResult fn_8005B1F8(const ShotInfo &info, int kind, boost::shared_ptr<CharaColliAttack> *attack) {
    if (info.m_owner.expired()) {
        return HitResult(false);
    }
    boost::shared_ptr<CharInfo> owner = info.m_owner.lock();
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8((const Vec3f &)info.m_direction));
    /* The start, moved back to the owner's position across the direction. */
    float along = fn_8040CF90(((VectorSubtractFn)fn_8040CD74)((const Vec3f *)&info.m_position, &owner->m_position),
                              direction);
    Vec3f from;
    fn_800091EC(&from, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)((const Vec3f *)&info.m_position,
                                                                        &(const Vec3f &)fn_8040CDA8(direction, along)));
    return fn_8005AFE4(kind, &from, (const Vec3f *)&info.m_position, attack);
}

extern "C" HitResult fn_8005AFE4(int kind, const Vec3f *from, const Vec3f *to,
                                 const boost::shared_ptr<CharaColliAttack> *attack) {
    UnkLine_80020A20 line;
    fn_80020A20(&line, from, to);
    UnkLineFilter_8005B5A0 filter;
    if (attack->get() != 0 && !(*attack)->m_owner.expired()) {
        filter.m_player = (*attack)->m_owner.lock()->field_0x54;
    }
    UnkColliContacts_8001EE04 *contacts = fn_80021FE4(fn_80020D40(), &line, 3, kind, LineFilter, &filter);
    if (contacts != 0) {
        HitResult result(false);
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts)); it.m_node != fn_8001EDEC(contacts); fn_80027374(&it, 0)) {
            UnkColliContactNode_800587A0 *node = it.m_node;
            void *object = node->m_object;
            AssignVector((Vec3f &)result.m_position, *fn_8001E9BC(object));
            AssignVector((Vec3f &)result.m_normal, *fn_800266D4(object));
            AssignVector((Vec3f &)result.field_0x18, ((VectorSubtractFn)fn_8040CD74)(to, from));
            result.field_0x2C = fn_80026588(node) & 0x0FFFFFFF;
            result.field_0x30 = fn_80026588(node) >> 28;
            void *manager = fn_80021B08();
            if (attack->get() != 0 && fn_800C3ED4(manager, node)) {
                fn_800C40C4(manager, boost::weak_ptr<CharaColliAttack>(*attack), node, &result);
                result.m_struck = 0;
            } else {
                result.m_hit = 1;
                result.m_struck = 1;
            }
            if (result.m_hit) {
                break;
            }
        }
        fn_80026FB4(contacts, 1);
        return result;
    }
    return HitResult(false);
}

/* What the laser moving from from to to hits first, with its attack collision: what its attack
 * ignores lets it pass, and with stop set it stops at what its attack meets (Game/BulletLaser.cpp
 * calls it by its address). A hit point is pulled back a tenth of a unit along the move. */
extern "C" HitResult fn_8005A9D0(int kind, const Vec3f *from, const Vec3f *to,
                                 const boost::shared_ptr<CharaColliAttack> *attack, bool stop) {
    UnkLine_80020A20 line;
    fn_80020A20(&line, from, to);
    UnkLineFilter_8005B5A0 filter;
    if (attack->get() != 0 && !(*attack)->m_owner.expired()) {
        filter.m_player = (*attack)->m_owner.lock()->field_0x54;
    }
    void *world = fn_80020D40();
    void *manager = fn_80021B08();
    UnkColliContacts_8001EE04 *contacts = fn_80021FE4(world, &line, 3, kind, LineFilter, &filter);
    if (contacts != 0) {
        HitResult result(false);
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts)); it.m_node != fn_8001EDEC(contacts);
             fn_80027374(&it, 0)) {
            UnkColliContactNode_800587A0 *node = it.m_node;
            void *object = node->m_object;
            boost::shared_ptr<CharInfo> hit;
            result.m_struck = 1;
            boost::shared_ptr<CharaColliBase> collider = fn_800587A0(node).lock();
            if (collider.get() != 0) {
                boost::shared_ptr<CharaColliBaseCharInfo> charCollider =
                    boost::dynamic_pointer_cast<CharaColliBaseCharInfo>(collider);
                boost::weak_ptr<CharaColliAttack> other = AttackOf(charCollider.get());
                hit = fn_800586EC(charCollider.get()).lock();
                if (attack->get() != 0 && !fn_80053538(&other) && IsIgnoring(attack->get(), other)) {
                    continue;
                }
                if (fn_800C3ED4(manager, node)) {
                    result.m_struck = 0;
                }
            }
            result.m_hit = 0;
            AssignVector((Vec3f &)result.m_position, *fn_8001E9BC(object));
            AssignVector((Vec3f &)result.m_normal, *fn_800266D4(object));
            AssignVector((Vec3f &)result.field_0x18, ((VectorSubtractFn)fn_8040CD74)(to, from));
            result.field_0x2C = fn_80026588(node) & 0x0FFFFFFF;
            result.field_0x30 = fn_80026588(node) >> 28;
            result.m_target = hit;
            if (result.m_struck) {
                result.m_hit = 1;
            } else if (attack->get() != 0) {
                fn_800C40C4(manager, boost::weak_ptr<CharaColliAttack>(*attack), node, &result);
                if (stop) {
                    result.m_hit = 1;
                }
            }
            if (result.m_hit) {
                if (collider.get() != 0) {
                    float back = fn_8005AE20(0.01f);
                    fn_8040CE04((Vec3f *)&result.m_position,
                                fn_800091A4(fn_8040CEE8((const Vec3f &)result.field_0x18), back));
                }
                break;
            }
        }
        fn_80026FB4(contacts, 1);
        return result;
    }
    return HitResult(false);
}

/* Creates the attack collision of a bullet: of kind 5, flags 0x40, the given damage and
 * callbacks, for owner (side is not used; other units call it by its address). */
extern "C" boost::shared_ptr<CharaColliAttack> fn_8005A300(int side, boost::weak_ptr<CharInfo> owner, float damage,
                                                           const AttackCallback &onHit,
                                                           const AttackCallback &onEnd) {
    boost::shared_ptr<CharaColliAttack> attack = boost::shared_ptr<CharaColliAttack>(new CharaColliAttack());
    if (attack.get() != 0) {
        fn_80054DD0(attack.get(), 5, 0x40, damage, onHit, onEnd);
        attack->m_owner = owner;
    }
    return attack;
}

/* The starting state of a bullet fired with info at speed: it adds the part of the aim along the
 * direction to the speed (other units call it by its address). */
extern "C" BulletPosition fn_8005A228(const ShotInfo &info, float speed) {
    float dot = fn_8040CF90((const Vec3f &)info.m_aim, fn_8040CEE8((const Vec3f &)info.m_direction));
    float along = 0.0f;
    if (dot > along) {
        along = dot;
    }
    return BulletPosition((const Vec3f &)info.m_position,
                          fn_8040CDA8(fn_8040CEE8((const Vec3f &)info.m_direction), speed + along),
                          (const Vec3f &)UnkRotation(fn_8040F760(fn_80016284((const Vec3f &)info.m_direction))));
}
