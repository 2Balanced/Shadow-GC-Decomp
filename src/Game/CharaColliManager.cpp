/* The collider manager (class name unknown; the singleton fn_80021B08 returns, which
 * Game/CharaColliManageTask.cpp ticks). It keeps one list of weak pointers to colliders per kind and
 * a matrix of the kinds that touch each other. Each frame it tests every pair of touching
 * colliders, keeps their contacts in each collider's result list (CharaColliBase's 0x1C bytes at
 * 0xC) and runs the attack callbacks of their attack collisions; it also makes the colliders and
 * answers sphere and line queries. The functions other units call keep their address names. File
 * name, member and function names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * NonMatching: every section matches and the unit links byte for byte except fn_800C4DE0,
 * fn_800C5478, fn_800C5938, fn_800C5D74 and fn_800C6C80, which differ only in the choice of
 * callee-saved registers (same instructions and stack layout). Two unused functions
 * (DrawColliders, PoolConstants) place the list's iterator functions and order the .sdata2
 * constants as retail has them; the linker strips them. */
#define CHARACOLLI_BASE_MEMBERS
#include "Game/AttackIgnoreList.h"
#include "Game/BitTable.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharaColliBody.h"
#include "Game/CharaColliGeneral.h"
#include "Game/DebugDisp.h"
#include "Game/DebugDraw.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;
using SonicteamUSA::System::CharaColli::CharaColliBody;
using SonicteamUSA::System::CharaColli::CharaColliBodyCharInfo;
using SonicteamUSA::System::CharaColli::CharaColliGeneral;
using SonicteamUSA::System::CharaColli::CharaColliGeneralBone;
using SonicteamUSA::System::CharaColli::CharaColliGeneralCharInfo;

namespace SonicteamUSA {
namespace System {
namespace Collision {
class CollisionResult;
class CollisionResultList;
} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResult;
using SonicteamUSA::System::Collision::CollisionResultList;

/* A position in a result list (layout from fn_8001EDFC, its out-of-line constructor). */
struct UnkResultIter_8001EDFC {
    UnkResultIter_8001EDFC() : m_node(0) {}
    UnkResultIter_8001EDFC(CollisionResult *node) : m_node(node) {}

    CollisionResult *m_node;
};

/* The game's stage state (see Game/StageManager.cpp); byte 0xC8 stops the attack callbacks
 * (meaning inferred). */
struct UnkStage_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xC8];
    /* 0x0C8 */ u8 field_0xC8;
};
extern UnkStage_8057D6A0 lbl_8057D6A0;

extern "C" {
/* The collider manager (an inline singleton of an earlier unit). */
class UnkColliderManager_80021B08;
UnkColliderManager_80021B08 *fn_80021B08(void);

/* The result lists: allocation and constructor, destructor (arg 1 frees), first and end node,
 * erasing the node at it, inserting before it, appending, and the sorted inserts (behaviour
 * inferred). */
void *fn_8001FADC(u32 size);
CollisionResultList *fn_800270CC(CollisionResultList *list);
void fn_80026FB4(CollisionResultList *list, int arg1);
CollisionResult *fn_8001EE04(CollisionResultList *list);
CollisionResult *fn_8001EDEC(CollisionResultList *list);
UnkResultIter_8001EDFC fn_8001EDFC(CollisionResult *node);
UnkResultIter_8001EDFC fn_80027374(UnkResultIter_8001EDFC *it, int);
void fn_80026AAC(CollisionResultList *list, UnkResultIter_8001EDFC *it);
void fn_80026B20(CollisionResultList *list, UnkResultIter_8001EDFC *it, CollisionResult *result);
void fn_80026B64(CollisionResultList *list, CollisionResult *result);
void fn_80026BB4(CollisionResultList *list, CollisionResult *result);
void fn_80026BF4(CollisionResultList *list, CollisionResult *result);
void fn_80026D8C(CollisionResultList *list, CollisionResult *result);

/* A result's allocation; a contact's hit as a collider hands it out, with the length of the move
 * (fn_800267B0); the hit's object, its position and normal. */
void *fn_80026518(u32 size);
UnkColliHit_8002686C *fn_800267B0(UnkColliHit_8002686C *hit, SonicteamUSA::System::Range::RangeBase *shape,
                                  const UnkColliContact_8001FF00 *contact, float move);
s32 fn_80099080(void *object);
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);

/* The collider of a result, and the character of a collider (inline functions of the game, first
 * emitted by earlier units). */
boost::weak_ptr<CharaColliBase> fn_800587A0(CollisionResult *result);
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);

/* |a - b|^2 */
float fn_8040D180(const Vec3f *a, const Vec3f *b);
}

/* A zero rotation (read-only data). */
extern const Vec3f lbl_805E07B8;

/* The empty character info and attack collision (Game/CharaColliBaseCharInfo.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;
extern boost::shared_ptr<CharaColliAttack> lbl_805EE6A0;

/* Inline functions of the game that Game/BulletUtil.cpp emits first: the attack collision of a
 * collider attached to a character, and whether an attack collision ignores another. */
boost::weak_ptr<CharaColliAttack> AttackOf(CharaColliBaseCharInfo *collider);
bool IsIgnoring(CharaColliAttack *attack, boost::weak_ptr<CharaColliAttack> other);

namespace boost {
/* Out of line in the game (boost 1.32's definition). */
template <> inline void weak_ptr<CharaColliBase>::reset() {
    this_type().swap(*this);
}
} // namespace boost

namespace SonicteamUSA {
namespace System {
namespace Collision {

/* CollisionResult (class name from the RTTI of its shared_ptr control block): a collider a query
 * or a contact found, the shapes on either side and the hit (0x24 bytes; names inferred). */
class CollisionResult {
  public:
    static void *operator new(u32 size) {
        return fn_80026518(size);
    }

    CollisionResult(boost::weak_ptr<CharaColliBase> collider, SonicteamUSA::System::Range::RangeBase *shape,
                    UnkColliHit_8002686C *hit, SonicteamUSA::System::Range::RangeBase *ownShape)
        : m_collider(collider), m_ownShape(ownShape), m_shape(shape), m_hit(hit) {
        if (!collider.expired()) {
            m_raw = collider.lock().get();
        } else {
            m_raw = 0;
        }
    }

    /* 0x00 */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x08 */ SonicteamUSA::System::Range::RangeBase *m_ownShape;
    /* 0x0C */ SonicteamUSA::System::Range::RangeBase *m_shape;
    /* 0x10 */ CharaColliBase *m_raw;
    /* 0x14 */ UnkColliHit_8002686C *m_hit;
    /* 0x18 */ u8 field_0x18[0x24 - 0x18];
};

/* CollisionResultList (class name from the RTTI of its shared_ptr control block). */
class CollisionResultList {
  public:
    /* 0x00 */ u8 field_0x0[0x1C];
};

} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

/* A vector the queries return: its copy goes through the out-of-line copy (fn_800091EC). */
struct UnkResultVector_800C6B84 {
    UnkResultVector_800C6B84() {}
    UnkResultVector_800C6B84(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    UnkResultVector_800C6B84(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }
    UnkResultVector_800C6B84 &operator=(const UnkResultVector_800C6B84 &v) {
        fn_800091D0(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
        return *this;
    }

    float x;
    float y;
    float z;
};

/* The results a collider keeps (CharaColliBase's bytes at 0xC). */
inline CollisionResultList *ResultsOf(CharaColliBase *collider) {
    return reinterpret_cast<CollisionResultList *>(collider->field_0xC);
}

inline bool operator==(const Vec3f &a, const Vec3f &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

/* A node of a collider list (struct name unknown; layout as Game/AttackIgnoreList.h's). */
struct UnkColliNode_800C4D5C {
    UnkColliNode_800C4D5C() : m_raw(0), m_prev(0), m_next(0), m_allocated(false) {}

    boost::weak_ptr<CharaColliBase> Get() const {
        return m_value;
    }

    /* 0x00 */ boost::weak_ptr<CharaColliBase> m_value;
    /* 0x08 */ CharaColliBase *m_raw;
    /* 0x0C */ UnkColliNode_800C4D5C *m_prev;
    /* 0x10 */ UnkColliNode_800C4D5C *m_next;
    /* 0x14 */ bool m_allocated;
};
SIZE_ASSERT(UnkColliNode_800C4D5C, 0x18);

/* Gives node back to the pool the lists share (AttackIgnorePool). */
inline void FreeNode(UnkAttackIgnorePool_800535A4 *pool, UnkColliNode_800C4D5C *node) {
    pool->m_free->m_prev = reinterpret_cast<UnkAttackIgnoreNode_80053644 *>(node);
    node->m_next = reinterpret_cast<UnkColliNode_800C4D5C *>(pool->m_free);
    node->m_prev = 0;
    pool->m_free = reinterpret_cast<UnkAttackIgnoreNode_80053644 *>(node);
    node->m_value.reset();
    pool->m_used--;
}

UnkAttackIgnorePool_800535A4 *AttackIgnorePool();

/* The colliders of one kind (class name unknown): a list of weak pointers whose nodes come from the
 * pool Game/AttackIgnoreList.h's lists use. */
struct UnkColliList_800C7768 {
    typedef UnkColliNode_800C4D5C Node;

    class iterator {
      public:
        iterator() : m_node(0) {}
        iterator(Node *node) : m_node(node) {}

        boost::weak_ptr<CharaColliBase> operator*() const {
            return m_node->Get();
        }
        iterator operator++(int) {
            iterator previous = *this;
            m_node = m_node ? m_node->m_next : 0;
            return previous;
        }
        bool operator!=(const Node *node) const {
            return m_node != node;
        }
        bool operator==(const Node *node) const {
            return m_node == node;
        }

        Node *m_node;
    };

    UnkColliList_800C7768() : m_pool(AttackIgnorePool()) {
        Node *node;
        NewNode(node);
        node->m_raw = 0;
        m_head = m_tail = node;
        m_size = 0;
    }
    ~UnkColliList_800C7768() {
        Clear();
        Node *node = m_head;
        if (!node->m_allocated) {
            FreeNode(m_pool, node);
        } else {
            delete node;
        }
    }

    void Clear() {
        Node *node = m_head;
        while (node != m_tail) {
            Node *next = node->m_next;
            if (!node->m_allocated) {
                FreeNode(m_pool, node);
            } else {
                delete node;
            }
            node = next;
        }
        m_tail->m_prev = 0;
        m_head = m_tail;
        m_size = 0;
    }

    Node *Head() const {
        return m_head;
    }
    Node *End() const {
        return m_tail;
    }

    void NewNode(Node *&result) {
        Node *node;
        UnkAttackIgnorePool_800535A4 *pool = m_pool;
        node = reinterpret_cast<Node *>(pool->m_free);
        if (pool->m_used == pool->m_capacity - 1) {
            node = 0;
        } else {
            node->m_next->m_prev = 0;
            pool->m_used++;
            pool->m_free = reinterpret_cast<UnkAttackIgnoreNode_80053644 *>(node->m_next);
            node->m_next = 0;
        }
        if (!node) {
            node = new Node;
            node->m_allocated = true;
        }
        result = node;
    }

    void PushFront(boost::weak_ptr<CharaColliBase> value) {
        Node *node;
        NewNode(node);
        node->m_prev = 0;
        node->m_value = value;
        if (value.lock()) {
            node->m_raw = value.lock().get();
        } else {
            node->m_raw = 0;
        }
        node->m_next = m_head;
        m_head->m_prev = node;
        m_head = node;
        m_size++;
    }

    void EraseNode(Node *node) {
        if (node) {
            if (node->m_prev) {
                node->m_prev->m_next = node->m_next;
            } else {
                m_head = node->m_next;
            }
            if (node->m_next) {
                node->m_next->m_prev = node->m_prev;
            } else {
                m_tail = node->m_prev;
            }
            if (!node->m_allocated) {
                FreeNode(m_pool, node);
            } else {
                delete node;
            }
            m_size--;
        }
    }
    void Erase(const iterator &it) {
        EraseNode(it.m_node);
    }

    /* 0x0 */ UnkAttackIgnorePool_800535A4 *m_pool;
    /* 0x4 */ s32 m_size;
    /* 0x8 */ Node *m_head;
    /* 0xC */ Node *m_tail;
};
SIZE_ASSERT(UnkColliList_800C7768, 0x10);

/* The collider manager (class name unknown; the singleton fn_80021B08 returns). */
class UnkColliderManager_80021B08 {
  public:
    typedef UnkColliList_800C7768 List;

    UnkColliderManager_80021B08();
    ~UnkColliderManager_80021B08();

    /* 0x00 */ List *m_lists;
    /* 0x04 */ s32 m_count;
    /* 0x08 */ SonicteamUSA::System::BitTable m_matrix; /* the kinds that touch each other */
};
SIZE_ASSERT(UnkColliderManager_80021B08, 0x1C);

typedef UnkColliderManager_80021B08 Manager;
typedef UnkColliList_800C7768 ColliList;

/* What an attack collision passes to its callbacks, as this unit builds it (see
 * Game/CharaColliAttack.h; names inferred). */
struct UnkAttackHit_800C6600 {
    UnkAttackHit_800C6600() {
        m_x = 0.0f;
        m_y = 0.0f;
        m_z = 0.0f;
        field_0x40 = 0.0f;
        field_0x44 = 0.0f;
        field_0x48 = 0.0f;
        field_0x4C = 0.0f;
        m_damage = 0.0f;
        m_result = 0;
    }

    AttackCallbackParam &Param() {
        return *reinterpret_cast<AttackCallbackParam *>(this);
    }

    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ boost::weak_ptr<CharaColliAttack> m_other;
    /* 0x10 */ SonicteamUSA::System::Range::RangeBase *m_attackerShape;
    /* 0x14 */ SonicteamUSA::System::Range::RangeBase *m_otherShape;
    /* 0x18 */ UnkResultVector_800C6B84 m_position;
    /* 0x24 */ UnkResultVector_800C6B84 m_normal;
    /* 0x30 */ u8 m_handled;
    /* 0x34 */ float m_x;
    /* 0x38 */ float m_y;
    /* 0x3C */ float m_z;
    /* 0x40 */ float field_0x40;
    /* 0x44 */ float field_0x44;
    /* 0x48 */ float field_0x48;
    /* 0x4C */ float field_0x4C;
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
    /* 0x58 */ u32 m_arg;
};

/* Bytes and words of an attack collision that Game/CharaColliAttack.h does not name. */
inline u8 &Byte(CharaColliAttack *attack, int offset) {
    return reinterpret_cast<u8 *>(attack)[offset];
}
inline s32 &Word(CharaColliAttack *attack, int offset) {
    return *reinterpret_cast<s32 *>(reinterpret_cast<u8 *>(attack) + offset);
}

extern "C" {
/* Whether a callback is empty (other units call it by its address). */
inline bool fn_800C402C(const UnkAttackCallback_80054E28 *callback) {
    return callback->empty();
}

/* Calls a callback when it is set (other units call it by its address). */
inline void fn_800C6538(const UnkAttackCallback_80054E28 *callback, AttackCallbackParam *param) {
    if (!callback->empty()) {
        (*callback)(*param);
    }
}
}


extern "C" {
void fn_800C6C80(Manager *manager);
void fn_800C6664(Manager *manager, CharaColliBase *collider, CharaColliBase *other, CollisionResult *result,
                 CollisionResult *otherResult, u32 arg);
CollisionResult *fn_800C6AA0(Manager *manager, CharaColliBase *collider, CharaColliBase *other);
UnkResultVector_800C6B84 fn_800C6B84(Manager *manager, CollisionResult *result);
UnkResultVector_800C6B84 fn_800C6BF8(Manager *manager, CollisionResult *result);
void fn_800C40C4(Manager *manager, boost::weak_ptr<CharaColliAttack> attack, CollisionResult *result, u32 arg);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(Manager *manager, boost::shared_ptr<CharaColliBase> collider);
void fn_800C6220(Manager *manager, boost::weak_ptr<CharaColliAttack> attack, CharaColliBase *victim,
                 CollisionResult *result, u32 arg);
void fn_800C4B8C(Manager *manager, boost::shared_ptr<CharaColliBase> collider);
boost::shared_ptr<CharaColliBase> fn_800C81F4(Manager *manager, boost::weak_ptr<CharInfo> info,
                                              const UnkColliShapeDesc_80037668 *shapes, int type, u16 typeArg);
boost::shared_ptr<CharaColliBase> fn_800C86A8(Manager *manager, const UnkColliShapeDesc_80037668 *shapes,
                                              const Vec3f *position, const Vec3f *rotation, s32 arg, int type,
                                              u16 typeArg, boost::weak_ptr<CharInfo> info);
}

UnkColliderManager_80021B08::UnkColliderManager_80021B08() : m_count(1), m_matrix(1, 1, true) {
    m_lists = new List[1];
    m_lists->Clear();
}

UnkColliderManager_80021B08::~UnkColliderManager_80021B08() {
}

extern "C" {

void fn_800C8B80(Manager *manager) {
    delete[] manager->m_lists;
    manager->m_matrix.~BitTable();
    manager->m_lists = 0;
    manager->m_count = 0;
}

boost::shared_ptr<CharaColliBase> fn_800C86A8(Manager *manager, const UnkColliShapeDesc_80037668 *shapes,
                                              const Vec3f *position, const Vec3f *rotation, s32 arg, int type,
                                              u16 typeArg, boost::weak_ptr<CharInfo> info) {
    if (shapes[1].field_0x4 == 0) {
        boost::shared_ptr<CharaColliBase> collider(
            new CharaColliBody(shapes, position, rotation, arg, type, typeArg, info));
        collider->vf_0x54(collider);
        fn_800C4B8C(manager, collider);
        return collider;
    } else {
        boost::shared_ptr<CharaColliBase> collider(
            new CharaColliGeneral(shapes, position, rotation, arg, type, typeArg, info));
        collider->vf_0x54(collider);
        fn_800C4B8C(manager, collider);
        return collider;
    }
}

boost::shared_ptr<CharaColliBase> fn_800C81F4(Manager *manager, boost::weak_ptr<CharInfo> info,
                                              const UnkColliShapeDesc_80037668 *shapes, int type, u16 typeArg) {
    if (shapes[1].field_0x4 == 0) {
        boost::shared_ptr<CharaColliBase> collider(new CharaColliBodyCharInfo(info, shapes, type, typeArg));
        collider->vf_0x54(collider);
        fn_800C4B8C(manager, collider);
        return collider;
    } else {
        boost::shared_ptr<CharaColliBase> collider(new CharaColliGeneralCharInfo(info, shapes, type, typeArg));
        collider->vf_0x54(collider);
        fn_800C4B8C(manager, collider);
        return collider;
    }
}

boost::shared_ptr<CharaColliBase> fn_800C7F7C(Manager *manager, const boost::weak_ptr<CharInfo> &info,
                                              const UnkColliShapeDesc_80037668 *shapes, _stMtnPlayWork *motion,
                                              int type, u16 typeArg) {
    boost::shared_ptr<CharaColliBase> collider(new CharaColliGeneralBone(info, shapes, motion, type, typeArg));
    collider->vf_0x54(collider);
    fn_800C4B8C(manager, collider);
    return collider;
}

boost::shared_ptr<CharaColliBase> fn_800C7EA8(Manager *manager, const Vec3f *position, u32 attributes,
                                              int type, u16 typeArg, const boost::weak_ptr<CharInfo> &info,
                                              const Vec3f *center, float radius) {
    UnkSphere_80035CEC sphere(*center, radius);
    UnkColliShapeDesc_80037668 shapes[2] = {{0, 0, 0, 0}, {0, 0, 0, 0}};
    shapes[0].field_0x4 = reinterpret_cast<u32>(&sphere);
    shapes[0].field_0x8 = attributes;
    shapes[0].field_0xC = typeArg;
    return fn_800C86A8(fn_80021B08(), shapes, position, &lbl_805E07B8, 0, type, typeArg, info);
}

boost::shared_ptr<CharaColliBase> fn_800C7DEC(Manager *manager, const boost::weak_ptr<CharInfo> &info,
                                              u32 attributes, int type, u16 typeArg, const Vec3f *center,
                                              float radius) {
    UnkSphere_80035CEC sphere(*center, radius);
    UnkColliShapeDesc_80037668 shapes[2] = {{0, 0, 0, 0}, {0, 0, 0, 0}};
    shapes[0].field_0x4 = reinterpret_cast<u32>(&sphere);
    shapes[0].field_0x8 = attributes;
    shapes[0].field_0xC = typeArg;
    return fn_800C81F4(fn_80021B08(), info, shapes, type, typeArg);
}

boost::shared_ptr<CharaColliBase> fn_800C7D04(Manager *manager, const Vec3f *position, const Vec3f *size,
                                              const Vec3f *rotation, s32 arg, u32 attributes, int type,
                                              u16 typeArg, const boost::weak_ptr<CharInfo> &info,
                                              const Vec3f *center, const Vec3f *boxRotation, int boxArg) {
    UnkBox_8002DBC8 box(*center, *size, *boxRotation, boxArg);
    UnkColliShapeDesc_80037668 shapes[2] = {{1, 0, 0, 0}, {0, 0, 0, 0}};
    shapes[0].field_0x4 = reinterpret_cast<u32>(&box);
    shapes[0].field_0x8 = attributes;
    shapes[0].field_0xC = typeArg;
    return fn_800C86A8(fn_80021B08(), shapes, position, rotation, arg, type, typeArg, info);
}

boost::shared_ptr<CharaColliBase> fn_800C7C34(Manager *manager, const boost::weak_ptr<CharInfo> &info,
                                              const Vec3f *size, u32 attributes, int type, u16 typeArg,
                                              const Vec3f *center, const Vec3f *rotation, int arg) {
    UnkBox_8002DBC8 box(*center, *size, *rotation, arg);
    UnkColliShapeDesc_80037668 shapes[2] = {{1, 0, 0, 0}, {0, 0, 0, 0}};
    shapes[0].field_0x4 = reinterpret_cast<u32>(&box);
    shapes[0].field_0x8 = attributes;
    shapes[0].field_0xC = typeArg;
    return fn_800C81F4(fn_80021B08(), info, shapes, type, typeArg);
}

boost::shared_ptr<CharaColliBase> fn_800C7B4C(Manager *manager, const Vec3f *position, const Vec3f *rotation,
                                              s32 arg, u32 attributes, int type, u16 typeArg,
                                              const boost::weak_ptr<CharInfo> &info, const Vec3f *center,
                                              const Vec3f *shapeRotation, int shapeArg, float height,
                                              float radius) {
    UnkCylinder_80034854 cylinder(*center, height, radius, *shapeRotation, shapeArg);
    UnkColliShapeDesc_80037668 shapes[2] = {{3, 0, 0, 0}, {0, 0, 0, 0}};
    shapes[0].field_0x4 = reinterpret_cast<u32>(&cylinder);
    shapes[0].field_0x8 = attributes;
    shapes[0].field_0xC = typeArg;
    return fn_800C86A8(fn_80021B08(), shapes, position, rotation, arg, type, typeArg, info);
}

boost::shared_ptr<CharaColliBase> fn_800C7A84(Manager *manager, const boost::weak_ptr<CharInfo> &info,
                                              u32 attributes, int type, u16 typeArg, const Vec3f *center,
                                              const Vec3f *rotation, float height, float radius, int arg) {
    UnkCylinder_80034854 cylinder(*center, height, radius, *rotation, arg);
    UnkColliShapeDesc_80037668 shapes[2] = {{3, 0, 0, 0}, {0, 0, 0, 0}};
    shapes[0].field_0x4 = reinterpret_cast<u32>(&cylinder);
    shapes[0].field_0x8 = attributes;
    shapes[0].field_0xC = typeArg;
    return fn_800C81F4(fn_80021B08(), info, shapes, type, typeArg);
}

boost::shared_ptr<CharaColliBase> fn_800C799C(Manager *manager, const Vec3f *position, const Vec3f *rotation,
                                              s32 arg, u32 attributes, int type, u16 typeArg,
                                              const boost::weak_ptr<CharInfo> &info, const Vec3f *center,
                                              const Vec3f *shapeRotation, int shapeArg, float halfHeight,
                                              float radius) {
    UnkCapsule_80030A5C capsule(*center, halfHeight, radius, *shapeRotation, shapeArg);
    UnkColliShapeDesc_80037668 shapes[2] = {{2, 0, 0, 0}, {0, 0, 0, 0}};
    shapes[0].field_0x4 = reinterpret_cast<u32>(&capsule);
    shapes[0].field_0x8 = attributes;
    shapes[0].field_0xC = typeArg;
    return fn_800C86A8(fn_80021B08(), shapes, position, rotation, arg, type, typeArg, info);
}

boost::shared_ptr<CharaColliBase> fn_800C78D4(Manager *manager, const boost::weak_ptr<CharInfo> &info,
                                              u32 attributes, int type, u16 typeArg, const Vec3f *center,
                                              float halfHeight, float radius, const Vec3f *rotation, int arg) {
    UnkCapsule_80030A5C capsule(*center, halfHeight, radius, *rotation, arg);
    UnkColliShapeDesc_80037668 shapes[2] = {{2, 0, 0, 0}, {0, 0, 0, 0}};
    shapes[0].field_0x4 = reinterpret_cast<u32>(&capsule);
    shapes[0].field_0x8 = attributes;
    shapes[0].field_0xC = typeArg;
    return fn_800C81F4(fn_80021B08(), info, shapes, type, typeArg);
}

/* Releases a collider. */
void fn_800C78B0(Manager *manager, boost::shared_ptr<CharaColliBase> *collider) {
    collider->reset();
}

/* Sets the number of collider kinds; the kind matrix starts empty. */
void fn_800C76C0(Manager *manager, u32 count) {
    if (manager->m_count != count) {
        manager->m_count = count;
        {
            SonicteamUSA::System::BitTable matrix(count, count, false);
            manager->m_matrix = matrix;
        }
        if (manager->m_lists) {
            delete[] manager->m_lists;
        }
        manager->m_lists = new ColliList[manager->m_count];
    }
}

/* Inserts result into list, which is sorted by collider; a result for the same collider is
 * replaced. */
void fn_800C74F8(Manager *manager, CollisionResultList *list, CollisionResult *result) {
    UnkResultIter_8001EDFC it(fn_8001EE04(list));
    UnkResultIter_8001EDFC next;
    boost::shared_ptr<CharaColliBase> collider = fn_800587A0(result).lock();
    if (collider.get() == 0) {
        return;
    }
    for (; it.m_node != fn_8001EDEC(list); it = next) {
        next = it;
        fn_80027374(&next, 0);
        boost::shared_ptr<CharaColliBase> other = fn_800587A0(it.m_node).lock();
        if (other.get() == 0) {
            fn_80026AAC(list, &it);
            continue;
        }
        if (other.get() > collider.get()) {
            fn_80026B20(list, &it, result);
            return;
        }
        if (other.get() == collider.get()) {
            fn_80026B20(list, &it, result);
            fn_80026AAC(list, &it);
            return;
        }
    }
    fn_80026B64(list, result);
}

/* Removes collider's result from list. */
void fn_800C73F0(Manager *manager, CollisionResultList *list, CharaColliBase *collider) {
    UnkResultIter_8001EDFC it(fn_8001EE04(list));
    UnkResultIter_8001EDFC next;
    for (; it.m_node != fn_8001EDEC(list); it = next) {
        next = it;
        fn_80027374(&next, 0);
        boost::shared_ptr<CharaColliBase> other = fn_800587A0(it.m_node).lock();
        if (other.get() == 0) {
            fn_80026AAC(list, &it);
            continue;
        }
        if (other.get() == collider) {
            fn_80026AAC(list, &it);
            break;
        }
    }
}

/* Tests every pair of colliders whose kinds touch, keeps their results and runs their attacks. */
void fn_800C6C80(Manager *manager) {
    ColliList::iterator it;
    ColliList::iterator it2;
    ColliList::iterator next;
    ColliList::iterator next2;
    UnkResultIter_8001EDFC rit;
    UnkResultIter_8001EDFC rnext;
    for (int i = 0; i < manager->m_count; i++) {
        if (manager->m_lists[i].m_size == 0) {
            continue;
        }
        it = manager->m_lists[i].Head();
        s32 kind = i;
        while (it != manager->m_lists[i].End()) {
            boost::weak_ptr<CharaColliBase> colliderRef = *it;
            bool moved = false;
            if (colliderRef.expired()) {
                next = it;
                next++;
                manager->m_lists[i].Erase(it);
                it = next;
                continue;
            }
            CharaColliBase *collider = colliderRef.lock().get();
            it++;
            if (!collider->field_0x2A && !collider->field_0x2B) {
                continue;
            }
            CollisionResultList *results = ResultsOf(collider);
            rit = fn_8001EDFC(fn_8001EE04(results));
            int j = i;
            if (it == manager->m_lists[i].End()) {
                j = i + 1;
            }
            for (; j < manager->m_count; j++) {
                if (manager->m_lists[j].m_size == 0 || !manager->m_matrix.Get(j, kind)) {
                    continue;
                }
                if (j == i) {
                    it2 = it;
                } else {
                    it2 = manager->m_lists[j].Head();
                }
                while (it2 != manager->m_lists[j].End()) {
                    boost::weak_ptr<CharaColliBase> otherRef = *it2;
                    if (otherRef.expired()) {
                        next2 = it2;
                        next2++;
                        manager->m_lists[j].Erase(it2);
                        it2 = next2;
                        continue;
                    }
                    CharaColliBase *other = otherRef.lock().get();
                    it2++;
                    if (!other->field_0x2A && !other->field_0x2B) {
                        continue;
                    }
                    if (!moved) {
                        collider->vf_0x7C();
                        moved = true;
                    }
                    other->vf_0x7C();
                    bool found = false;
                    for (rit = fn_8001EDFC(fn_8001EE04(results)); rit.m_node != fn_8001EDEC(results); rit = rnext) {
                        rnext = rit;
                        fn_80027374(&rnext, 0);
                        CollisionResult *node = rit.m_node;
                        boost::shared_ptr<CharaColliBase> known = fn_800587A0(node).lock();
                        if (known.get() == 0) {
                            fn_80026AAC(results, &rit);
                            continue;
                        }
                        if (known.get() == other) {
                            found = true;
                            if (!collider->m_changed && !other->m_changed && collider->field_0x2A &&
                                other->field_0x2A) {
                                fn_800C6664(manager, collider, other, node, 0, 0);
                            }
                            break;
                        }
                    }
                    CollisionResultList *otherResults = ResultsOf(other);
                    if ((!collider->field_0x2A && collider->field_0x2B) ||
                        (!other->field_0x2A && other->field_0x2B)) {
                        if (found) {
                            fn_80026AAC(results, &rit);
                            fn_800C73F0(manager, otherResults, collider);
                        }
                        continue;
                    }
                    if (!collider->m_changed && !other->m_changed) {
                        continue;
                    }
                    float reach = 0.0001f + (collider->vf_0x38() + other->vf_0x38());
                    if (!(fn_8040D180(collider->vf_0x14(), other->vf_0x14()) <= reach * reach)) {
                        if (found) {
                            fn_80026AAC(results, &rit);
                            fn_800C73F0(manager, otherResults, collider);
                        }
                        continue;
                    }
                    SonicteamUSA::System::Range::RangeBase *mine = 0;
                    SonicteamUSA::System::Range::RangeBase *theirs = 0;
                    UnkColliHit_8002686C *mineHit = 0;
                    UnkColliHit_8002686C *theirHit = 0;
                    if (collider->vf_0x80(other, &mine, &theirs, &mineHit, &theirHit)) {
                        CollisionResult *result = new CollisionResult(otherRef, theirs, mineHit, mine);
                        CollisionResult *otherResult = new CollisionResult(colliderRef, mine, theirHit, theirs);
                        if (found) {
                            fn_80026B20(results, &rit, result);
                            fn_80026AAC(results, &rit);
                        } else {
                            fn_800C74F8(manager, results, result);
                        }
                        fn_800C74F8(manager, otherResults, otherResult);
                        fn_800C6664(manager, collider, other, result, otherResult, 0);
                    } else if (found) {
                        fn_80026AAC(results, &rit);
                        fn_800C73F0(manager, otherResults, collider);
                    }
                }
            }
            collider->m_changed = 0;
            collider->field_0x2B = collider->field_0x2A;
        }
    }
}

/* The position of a result. */
UnkResultVector_800C6B84 fn_800C6BF8(Manager *manager, CollisionResult *result) {
    if (result == 0) {
        return lbl_805E07A0;
    }
    if (result->m_hit) {
        return *fn_8001E9BC(result->m_hit);
    }
    if (result->m_shape) {
        return result->m_shape->m_position;
    }
    return lbl_805E07A0;
}

/* The normal at a result (up when it has no hit). */
UnkResultVector_800C6B84 fn_800C6B84(Manager *manager, CollisionResult *result) {
    if (result == 0) {
        return lbl_805E07A0;
    }
    if (result->m_hit) {
        return *fn_800266D4(result->m_hit);
    }
    return UnkResultVector_800C6B84(0.0f, 1.0f, 0.0f);
}

/* collider's result for other, if it has one. */
CollisionResult *fn_800C6AA0(Manager *manager, CharaColliBase *collider, CharaColliBase *other) {
    CollisionResultList *results = ResultsOf(collider);
    UnkResultIter_8001EDFC it;
    for (it = fn_8001EDFC(fn_8001EE04(results)); it.m_node != fn_8001EDEC(results); fn_80027374(&it, 0)) {
        boost::shared_ptr<CharaColliBase> known(fn_800587A0(it.m_node));
        if (known.get() == 0) {
            continue;
        }
        if (known.get() == other) {
            return it.m_node;
        }
    }
    return 0;
}

/* Runs the attacks between two colliders in contact. */
void fn_800C6664(Manager *manager, CharaColliBase *collider, CharaColliBase *other, CollisionResult *result,
                 CollisionResult *otherResult, u32 arg) {
    if (lbl_8057D6A0.field_0xC8) {
        return;
    }
    CharaColliBaseCharInfo *info = dynamic_cast<CharaColliBaseCharInfo *>(collider);
    CharaColliBaseCharInfo *otherInfo = dynamic_cast<CharaColliBaseCharInfo *>(other);
    if (fn_80053538(&AttackOf(info)) || fn_80053538(&AttackOf(otherInfo))) {
        return;
    }
    boost::shared_ptr<CharaColliAttack> attack = AttackOf(info).lock();
    CharaColliAttack *attacker = attack.get();
    boost::shared_ptr<CharaColliAttack> otherAttack = AttackOf(otherInfo).lock();
    CharaColliAttack *defender = otherAttack.get();
    if (info && attacker && otherInfo && defender) {
        UnkAttackHit_800C6600 hit;
        if (Byte(attacker, 0x54) && Byte(defender, 0x55) &&
            !IsIgnoring(attacker, AttackOf(otherInfo)) && !fn_800C402C(&attacker->m_hitCallback) &&
            !fn_800C402C(&defender->m_attackCallback) &&
            Word(attacker, 0x38) > Word(defender, 0x3C)) {
            hit.m_damage = attacker->m_power;
            hit.m_attacker = attack;
            hit.m_other = otherAttack;
            hit.m_attackerShape = result->m_ownShape;
            if (otherResult == 0) {
                otherResult = fn_800C6AA0(manager, other, collider);
            }
            hit.m_otherShape = otherResult->m_ownShape;
            hit.m_position = fn_800C6BF8(manager, result);
            hit.m_normal = fn_800C6B84(manager, result);
            hit.m_arg = arg;
            hit.m_handled = 0;
            fn_800C6538(&attacker->m_hitCallback, &hit.Param());
            if (!hit.m_handled) {
                fn_800C6538(&defender->m_attackCallback, &hit.Param());
            }
            if (!hit.m_handled) {
                fn_800C6538(reinterpret_cast<UnkAttackCallback_80054E28 *>(attacker->field_0x28), &hit.Param());
            }
        }
        if (Byte(attacker, 0x55) && Byte(defender, 0x54) &&
            !IsIgnoring(defender, AttackOf(info)) && !fn_800C402C(&defender->m_hitCallback) &&
            !fn_800C402C(&attacker->m_attackCallback) &&
            Word(attacker, 0x3C) < Word(defender, 0x38)) {
            hit.m_damage = defender->m_power;
            hit.m_attacker = otherAttack;
            hit.m_other = attack;
            hit.m_attackerShape = result->m_ownShape;
            if (otherResult == 0) {
                otherResult = fn_800C6AA0(manager, other, collider);
            }
            hit.m_otherShape = otherResult->m_ownShape;
            hit.m_position = fn_800C6BF8(manager, otherResult);
            hit.m_normal = fn_800C6B84(manager, otherResult);
            hit.m_arg = arg;
            hit.m_handled = 0;
            fn_800C6538(&defender->m_hitCallback, &hit.Param());
            if (!hit.m_handled) {
                fn_800C6538(&attacker->m_attackCallback, &hit.Param());
            }
            if (!hit.m_handled) {
                fn_800C6538(reinterpret_cast<UnkAttackCallback_80054E28 *>(defender->field_0x28), &hit.Param());
            }
        }
    }
}

/* Runs attack against the collider a query found. */
void fn_800C6220(Manager *manager, boost::weak_ptr<CharaColliAttack> attack, CharaColliBase *victim,
                 CollisionResult *result, u32 arg) {
    if (lbl_8057D6A0.field_0xC8) {
        return;
    }
    if (fn_80053538(&attack)) {
        return;
    }
    CharaColliAttack *attacker = attack.lock().get();
    if (attacker == 0) {
        return;
    }
    CharaColliBaseCharInfo *info = dynamic_cast<CharaColliBaseCharInfo *>(victim);
    boost::shared_ptr<CharaColliAttack> otherAttack = AttackOf(info).lock();
    CharaColliAttack *defender = otherAttack.get();
    if (info && defender) {
        UnkAttackHit_800C6600 hit;
        if (Byte(attacker, 0x54) && Byte(defender, 0x55) && !IsIgnoring(attacker, AttackOf(info)) &&
            !fn_800C402C(&attacker->m_hitCallback) && !fn_800C402C(&defender->m_attackCallback) &&
            Word(attacker, 0x38) > Word(defender, 0x3C)) {
            hit.m_damage = attacker->m_power;
            hit.m_attacker = attack;
            hit.m_other = otherAttack;
            hit.m_otherShape = result->m_shape;
            hit.m_position = fn_800C6BF8(manager, result);
            hit.m_normal = fn_800C6B84(manager, result);
            hit.m_arg = arg;
            hit.m_handled = 0;
            fn_800C6538(&attacker->m_hitCallback, &hit.Param());
            if (!hit.m_handled) {
                fn_800C6538(&defender->m_attackCallback, &hit.Param());
            }
            if (!hit.m_handled) {
                fn_800C6538(reinterpret_cast<UnkAttackCallback_80054E28 *>(attacker->field_0x28), &hit.Param());
            }
        }
        if (Byte(attacker, 0x55) && Byte(defender, 0x54) && !IsIgnoring(defender, attack) &&
            !fn_800C402C(&defender->m_hitCallback) && !fn_800C402C(&attacker->m_attackCallback) &&
            Word(attacker, 0x3C) < Word(defender, 0x38)) {
            hit.m_damage = defender->m_power;
            hit.m_attacker = otherAttack;
            hit.m_other = attack;
            hit.m_attackerShape = result->m_shape;
            hit.m_handled = 0;
            hit.m_arg = arg;
            fn_800C6538(&defender->m_hitCallback, &hit.Param());
            if (!hit.m_handled) {
                fn_800C6538(&attacker->m_attackCallback, &hit.Param());
            }
            if (!hit.m_handled) {
                fn_800C6538(reinterpret_cast<UnkAttackCallback_80054E28 *>(defender->field_0x28), &hit.Param());
            }
        }
    }
}

/* Whether a line meets a collider. */
bool fn_800C6174(Manager *manager, const UnkDebugLine_80020A20 *line, const boost::weak_ptr<CharaColliBase> &colliderRef,
                 UnkColliContact_8001FF00 *contact) {
    CharaColliBase *collider;
    if (colliderRef.expired() || (collider = colliderRef.lock().get()) == 0) {
        return false;
    }
    collider->vf_0x7C();
    return collider->vf_0x60(line, contact, 0, 0) != 0;
}

/* The colliders of the kinds that touch kind which a line meets. */
CollisionResultList *fn_800C5D74(Manager *manager, const UnkDebugLine_80020A20 *line, u32 kind, bool collect,
                                 CollisionResultList *results, CharaColliBase::Filter filter, u32 filterArg,
                                 boost::weak_ptr<CharaColliAttack> attack) {
    for (int i = 0; i < manager->m_count; i++) {
        if (manager->m_lists[i].m_size == 0 || !manager->m_matrix.Get(kind, i)) {
            continue;
        }
        ColliList::iterator it;
        ColliList::iterator next;
        for (it = manager->m_lists[i].Head(); it != manager->m_lists[i].End();) {
            boost::weak_ptr<CharaColliBase> colliderRef = *it;
            if (colliderRef.expired()) {
                next = it;
                next++;
                manager->m_lists[i].Erase(it);
                it = next;
                continue;
            }
            CharaColliBase *collider = colliderRef.lock().get();
            it++;
            if (!collider->field_0x2A) {
                continue;
            }
            if (collect) {
                UnkColliContact_8001FF00 contact;
                fn_8001FF00(&contact);
                SonicteamUSA::System::Range::RangeBase *shape = collider->vf_0x60(line, &contact, filter, filterArg);
                if (shape) {
                    UnkColliHit_8002686C *hit = (UnkColliHit_8002686C *)fn_8001FC88(0x48);
                    if (hit) {
                        hit = fn_8002686C(hit, shape, &contact);
                    }
                    CollisionResult *result = new CollisionResult(colliderRef, shape, hit, 0);
                    if (results == 0) {
                        CollisionResultList *list = (CollisionResultList *)fn_8001FADC(0x1C);
                        if (list) {
                            list = fn_800270CC(list);
                        }
                        results = list;
                    }
                    if (!fn_80053538(&attack) && attack.lock().get()) {
                        fn_800C6220(manager, attack, collider, result, 0);
                    }
                    fn_80026D8C(results, result);
                }
            } else {
                SonicteamUSA::System::Range::RangeBase *shape = collider->vf_0x60(line, 0, 0, 0);
                if (shape) {
                    CollisionResult *result = new CollisionResult(colliderRef, shape, 0, 0);
                    if (results == 0) {
                        CollisionResultList *list = (CollisionResultList *)fn_8001FADC(0x1C);
                        if (list) {
                            list = fn_800270CC(list);
                        }
                        results = list;
                    }
                    if (!fn_80053538(&attack) && attack.lock().get()) {
                        fn_800C6220(manager, attack, collider, result, 0);
                    }
                    fn_80026BB4(results, result);
                }
            }
        }
    }
    return results;
}

/* The first collider of the kinds that touch kind which a line meets. */
CollisionResult *fn_800C5938(Manager *manager, const UnkDebugLine_80020A20 *line, int kind, bool collect,
                             CharaColliBase::Filter filter, u32 filterArg, boost::weak_ptr<CharaColliAttack> attack) {
    CollisionResult *result = 0;
    boost::shared_ptr<CharaColliBase> nearest;
    SonicteamUSA::System::Range::RangeBase *nearestShape = 0;
    UnkColliContact_8001FF00 nearestContact;
    fn_8001FF00(&nearestContact);
    UnkDebugLine_80020A20 segment(*line);
    for (int i = 0; i < manager->m_count; i++) {
        if (manager->m_lists[i].m_size == 0 || !manager->m_matrix.Get(kind, i)) {
            continue;
        }
        ColliList::iterator it;
        ColliList::iterator next;
        for (it = manager->m_lists[i].Head(); it != manager->m_lists[i].End();) {
            boost::weak_ptr<CharaColliBase> colliderRef = *it;
            if (colliderRef.expired()) {
                next = it;
                next++;
                manager->m_lists[i].Erase(it);
                it = next;
                continue;
            }
            CharaColliBase *collider = colliderRef.lock().get();
            it++;
            if (!collider->field_0x2A) {
                continue;
            }
            if (collect) {
                UnkColliContact_8001FF00 contact;
                fn_8001FF00(&contact);
                SonicteamUSA::System::Range::RangeBase *shape = collider->vf_0x60(&segment, &contact, filter, filterArg);
                if (shape) {
                    nearestShape = shape;
                    nearest = colliderRef.lock();
                    nearestContact = contact;
                    fn_800091D0(&segment.m_end, reinterpret_cast<const Vec3f *>(&contact.m_position));
                    if (segment.m_start == segment.m_end) {
                        break;
                    }
                }
            } else {
                SonicteamUSA::System::Range::RangeBase *shape = collider->vf_0x60(&segment, 0, 0, 0);
                if (shape) {
                    nearestShape = shape;
                    nearest = colliderRef.lock();
                }
            }
        }
    }
    if (nearestShape) {
        if (collect) {
            UnkColliHit_8002686C *hit = (UnkColliHit_8002686C *)fn_8001FC88(0x48);
            if (hit) {
                hit = fn_8002686C(hit, nearestShape, &nearestContact);
            }
            result = new CollisionResult(nearest, nearestShape, hit, 0);
        } else {
            result = new CollisionResult(nearest, nearestShape, 0, 0);
        }
        if (!fn_80053538(&attack) && attack.lock().get()) {
            fn_800C6220(manager, attack, nearest.get(), result, 0);
        }
    }
    return result;
}

/* Whether a sphere meets a collider. */
bool fn_800C5878(Manager *manager, const UnkSphere_8004E6CC *sphere, const boost::weak_ptr<CharaColliBase> &colliderRef,
                 UnkColliContact_8001FF00 *contact) {
    CharaColliBase *collider;
    if (colliderRef.expired() || (collider = colliderRef.lock().get()) == 0) {
        return false;
    }
    if (!collider->field_0x2A) {
        return false;
    }
    collider->vf_0x7C();
    return collider->vf_0x68(sphere, contact, 0, 0) != 0;
}

/* The colliders of the kinds that touch kind which a sphere meets. */
CollisionResultList *fn_800C5478(Manager *manager, const UnkSphere_8004E6CC *sphere, u32 kind, bool collect,
                                 CollisionResultList *results, CharaColliBase::Filter filter, u32 filterArg,
                                 boost::weak_ptr<CharaColliAttack> attack) {
    for (int i = 0; i < manager->m_count; i++) {
        if (manager->m_lists[i].m_size == 0 || !manager->m_matrix.Get(kind, i)) {
            continue;
        }
        ColliList::iterator it;
        ColliList::iterator next;
        for (it = manager->m_lists[i].Head(); it != manager->m_lists[i].End();) {
            boost::weak_ptr<CharaColliBase> colliderRef = *it;
            if (colliderRef.expired()) {
                next = it;
                next++;
                manager->m_lists[i].Erase(it);
                it = next;
                continue;
            }
            CharaColliBase *collider = colliderRef.lock().get();
            it++;
            if (!collider->field_0x2A) {
                continue;
            }
            if (collect) {
                UnkColliContact_8001FF00 contact;
                fn_8001FF00(&contact);
                SonicteamUSA::System::Range::RangeBase *shape = collider->vf_0x68(sphere, &contact, filter, filterArg);
                if (shape) {
                    UnkColliHit_8002686C *hit = (UnkColliHit_8002686C *)fn_8001FC88(0x48);
                    if (hit) {
                        hit = fn_8002686C(hit, shape, &contact);
                    }
                    CollisionResult *result = new CollisionResult(colliderRef, shape, hit, 0);
                    if (results == 0) {
                        CollisionResultList *list = (CollisionResultList *)fn_8001FADC(0x1C);
                        if (list) {
                            list = fn_800270CC(list);
                        }
                        results = list;
                    }
                    if (!fn_80053538(&attack) && attack.lock().get()) {
                        fn_800C6220(manager, attack, collider, result, 0);
                    }
                    fn_80026D8C(results, result);
                }
            } else {
                SonicteamUSA::System::Range::RangeBase *shape = collider->vf_0x68(sphere, 0, 0, 0);
                if (shape) {
                    CollisionResult *result = new CollisionResult(colliderRef, shape, 0, 0);
                    if (results == 0) {
                        CollisionResultList *list = (CollisionResultList *)fn_8001FADC(0x1C);
                        if (list) {
                            list = fn_800270CC(list);
                        }
                        results = list;
                    }
                    if (!fn_80053538(&attack) && attack.lock().get()) {
                        fn_800C6220(manager, attack, collider, result, 0);
                    }
                    fn_80026BB4(results, result);
                }
            }
        }
    }
    return results;
}

/* The colliders of the kinds that touch kind which a sphere meets as it moves by move; ignore lists
 * colliders to skip. */
CollisionResultList *fn_800C4DE0(Manager *manager, const UnkSphere_8004E6CC *sphere, const Vec3f *move, u32 kind,
                                 bool collect, CollisionResultList *results, CharaColliBase::Filter filter,
                                 u32 filterArg, boost::weak_ptr<CharaColliAttack> attack,
                                 CollisionResultList *ignore) {
    bool still = false;
    if (results && collect) {
        UnkColliHit_8002686C *first = fn_8001EE04(results)->m_hit;
        if (first && fn_80099080(first) == 1) {
            still = true;
        }
    }
    for (int i = 0; i < manager->m_count; i++) {
        if (manager->m_lists[i].m_size == 0 || !manager->m_matrix.Get(kind, i)) {
            continue;
        }
        ColliList::iterator it;
        ColliList::iterator next;
        for (it = manager->m_lists[i].Head(); it != manager->m_lists[i].End();) {
            boost::weak_ptr<CharaColliBase> colliderRef = *it;
            if (colliderRef.expired()) {
                next = it;
                next++;
                manager->m_lists[i].Erase(it);
                it = next;
                continue;
            }
            CharaColliBase *collider = colliderRef.lock().get();
            it++;
            if (!collider->field_0x2A) {
                continue;
            }
            if (ignore) {
                bool skip = false;
                UnkResultIter_8001EDFC rit;
                for (rit = fn_8001EDFC(fn_8001EE04(ignore)); rit.m_node != fn_8001EDEC(ignore); fn_80027374(&rit, 0)) {
                    if (!fn_800587A0(rit.m_node).expired()) {
                        boost::shared_ptr<CharaColliBase> known(fn_800587A0(rit.m_node));
                        if (known.get() == collider) {
                            skip = true;
                            break;
                        }
                    }
                }
                if (skip) {
                    continue;
                }
            }
            if (collect) {
                UnkColliContact_8001FF00 contact;
                fn_8001FF00(&contact);
                SonicteamUSA::System::Range::RangeBase *shape;
                if (still) {
                    shape = collider->vf_0x68(sphere, &contact, filter, filterArg);
                } else {
                    shape = collider->vf_0x6C(sphere, move, &contact, filter, filterArg);
                }
                if (shape) {
                    if (!still && fn_8040CF2C(move) == 0.0f) {
                        still = true;
                        if (results) {
                            fn_80026FB4(results, 1);
                            results = 0;
                        }
                    }
                    UnkColliHit_8002686C *hit = (UnkColliHit_8002686C *)fn_8001FC88(0x48);
                    if (hit) {
                        hit = fn_800267B0(hit, shape, &contact, fn_8040CF2C(move));
                    }
                    CollisionResult *result = new CollisionResult(colliderRef, shape, hit, 0);
                    if (results == 0) {
                        CollisionResultList *list = (CollisionResultList *)fn_8001FADC(0x1C);
                        if (list) {
                            list = fn_800270CC(list);
                        }
                        results = list;
                    }
                    if (!fn_80053538(&attack) && attack.lock().get()) {
                        fn_800C6220(manager, attack, collider, result, 0);
                    }
                    fn_80026BF4(results, result);
                }
            } else {
                SonicteamUSA::System::Range::RangeBase *shape;
                if (still) {
                    shape = collider->vf_0x68(sphere, 0, 0, 0);
                } else {
                    shape = collider->vf_0x6C(sphere, move, 0, 0, 0);
                }
                if (shape) {
                    if (!still && fn_8040CF2C(move) == 0.0f) {
                        still = true;
                        if (results) {
                            fn_80026FB4(results, 1);
                            results = 0;
                        }
                    }
                    CollisionResult *result = new CollisionResult(colliderRef, shape, 0, 0);
                    if (results == 0) {
                        CollisionResultList *list = (CollisionResultList *)fn_8001FADC(0x1C);
                        if (list) {
                            list = fn_800270CC(list);
                        }
                        results = list;
                    }
                    if (!fn_80053538(&attack) && attack.lock().get()) {
                        fn_800C6220(manager, attack, collider, result, 0);
                    }
                    fn_80026BB4(results, result);
                }
            }
        }
    }
    return results;
}

/* Adds a collider to the list of its kind. */
void fn_800C4B8C(Manager *manager, boost::shared_ptr<CharaColliBase> collider) {
    CharaColliBase *raw = collider.get();
    manager->m_lists[raw->m_type].PushFront(collider);
}

/* Takes collider out of the list of its kind and out of the results of the colliders that touch
 * it. */
void fn_800C46DC(Manager *manager, CharaColliBase *collider) {
    ColliList::iterator it;
    int type = collider->m_type;
    ColliList::iterator next;
    for (it = manager->m_lists[type].Head(); it != manager->m_lists[type].End(); it = next) {
        next = it;
        next++;
        if ((*it).expired()) {
            manager->m_lists[type].Erase(it);
        } else if ((*it).lock().get() == collider) {
            manager->m_lists[type].Erase(it);
        }
    }
    u16 kind = type;
    for (int i = 0; i < manager->m_count; i++) {
        if (manager->m_lists[i].m_size == 0 || !manager->m_matrix.Get(kind, i)) {
            continue;
        }
        UnkResultIter_8001EDFC rit;
        UnkResultIter_8001EDFC rnext;
        for (it = manager->m_lists[i].Head(); it != manager->m_lists[i].End(); it = next) {
            next = it;
            next++;
            if ((*it).expired()) {
                manager->m_lists[i].Erase(it);
                continue;
            }
            CharaColliBase *other = (*it).lock().get();
            if (other == 0) {
                continue;
            }
            if (other != 0) {
                CollisionResultList *results = ResultsOf(other);
                for (rit = fn_8001EDFC(fn_8001EE04(results)); rit.m_node != fn_8001EDEC(results); rit = rnext) {
                    rnext = rit;
                    fn_80027374(&rnext, 0);
                    if (fn_800587A0(rit.m_node).expired()) {
                        fn_80026AAC(results, &rit);
                    } else if (fn_800587A0(rit.m_node).lock().get() == collider) {
                        fn_80026AAC(results, &rit);
                    }
                }
            }
        }
    }
}

/* Draws every collider for debugging (name inferred). Not called anywhere: the linker strips it,
 * but retail emits the list's iterator functions here, ahead of fn_800C46DC. */
void DrawColliders(Manager *manager) {
    for (int i = 0; i < manager->m_count; i++) {
        ColliList::iterator it;
        for (it = manager->m_lists[i].Head(); it != manager->m_lists[i].End(); it++) {
            boost::shared_ptr<CharaColliBase> collider = (*it).lock();
            if (collider.get()) {
                collider->vf_0x74();
            }
        }
    }
}

/* The character of a collider. */
boost::weak_ptr<CharInfo> fn_800C4568(Manager *manager, const boost::weak_ptr<CharaColliBase> &colliderRef) {
    CharaColliBase *collider;
    if (colliderRef.expired() || (collider = colliderRef.lock().get()) == 0) {
        return lbl_805EE698;
    }
    CharaColliBaseCharInfo *info = dynamic_cast<CharaColliBaseCharInfo *>(collider);
    if (info) {
        return fn_800586EC(info);
    }
    return lbl_805EE698;
}

/* Gives a collider attached to a character a new attack collision. */
boost::weak_ptr<CharaColliAttack> fn_800C4374(Manager *manager, boost::shared_ptr<CharaColliBase> collider) {
    CharaColliBaseCharInfo *info = dynamic_cast<CharaColliBaseCharInfo *>(collider.get());
    if (info) {
        boost::shared_ptr<CharaColliAttack> attack(new CharaColliAttack);
        info->m_attack = attack;
        *reinterpret_cast<boost::weak_ptr<CharaColliBase> *>(attack.get()) = collider;
        attack->m_owner = info->m_info;
        return attack;
    }
    return lbl_805EE6A0;
}

/* The attack collision of a collider attached to a character. */
boost::weak_ptr<CharaColliAttack> fn_800C42D0(Manager *manager, boost::shared_ptr<CharaColliBase> collider) {
    CharaColliBaseCharInfo *info = dynamic_cast<CharaColliBaseCharInfo *>(collider.get());
    if (info) {
        return info->m_attack;
    }
    return lbl_805EE6A0;
}

/* Sets the attack callback of an attack collision. */
bool fn_800C423C(Manager *manager, boost::weak_ptr<CharaColliAttack> attackRef, u32 arg, const AttackCallback &callback) {
    boost::shared_ptr<CharaColliAttack> attack = attackRef.lock();
    CharaColliAttack *a = attack.get();
    if (a == 0) {
        return false;
    }
    a->field_0x55 = 1;
    *reinterpret_cast<u32 *>(a->field_0x3C) = arg;
    a->m_attackCallback = callback;
    return true;
}

/* Runs an attack collision against the colliders of results. */
void fn_800C4190(Manager *manager, boost::weak_ptr<CharaColliAttack> attack, CollisionResultList *results, u32 arg) {
    UnkResultIter_8001EDFC it;
    for (it = fn_8001EDFC(fn_8001EE04(results)); it.m_node != fn_8001EDEC(results); fn_80027374(&it, 0)) {
        fn_800C40C4(manager, attack, it.m_node, arg);
    }
}

/* Runs an attack collision against the collider of a result. */
void fn_800C40C4(Manager *manager, boost::weak_ptr<CharaColliAttack> attack, CollisionResult *result, u32 arg) {
    boost::shared_ptr<CharaColliBase> collider = fn_800587A0(result).lock();
    CharaColliBase *victim = collider.get();
    if (victim == 0) {
        return;
    }
    if (victim) {
        fn_800C6220(manager, attack, victim, result, arg);
    }
}

/* Whether the collider of a result has an attack collision that can be attacked. */
bool fn_800C3ED4(Manager *manager, CollisionResult *result) {
    boost::weak_ptr<CharaColliBase> colliderRef = fn_800587A0(result);
    if (!colliderRef.expired()) {
        boost::shared_ptr<CharaColliBase> collider(colliderRef);
        if (collider.get()) {
            boost::weak_ptr<CharaColliAttack> attackRef = fn_800C42D0(manager, collider);
            if (!fn_80053538(&attackRef)) {
                boost::shared_ptr<CharaColliAttack> attack(attackRef);
                if (attack.get()) {
                    return !fn_800C402C(&attack->m_attackCallback);
                }
            }
        }
    }
    return false;
}

} /* extern "C" */

/* Not called anywhere: the linker strips it. Retail pools the unit's float constants in the order
 * this function, the first one GC/2.7 generates, uses them (see docs/research-notes.md). */
void PoolConstants(float *values) {
    values[0] = 0.0001f;
    values[1] = 0.0f;
    values[2] = 1.0f;
}
