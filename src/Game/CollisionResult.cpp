/* SonicteamUSA::System::Collision::CollisionResult and CollisionResultList (class names from the
 * RTTI of their shared_ptr control blocks): what a collision query found, and the lists queries
 * collect them in. The unit holds the result's allocation (from a pool of 0x80 results, a
 * function-local static, or from the heap), its destructor and the contact's destructor, the
 * list's constructor, destructor and inserts (at either end, before a position, and sorted by
 * the contact's distance or the move's length), and the pools' constructors, destructors,
 * allocations and releases. Other units call these functions by their C names, so they are written
 * as C functions over the classes' layouts (names and meanings inferred). Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharaColli.h"
#include "Game/RangeBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include <new>

using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::Range::RangeBase;

/* A contact as a collider hands it out (UnkColliHit_8002686C, 0x48 bytes): the links of its pool,
 * whether it came from the heap, the contact, and the length of the move that found it. */
struct UnkColliHitFields_8002686C {
    /* 0x00 */ UnkColliHitFields_8002686C *m_prev;
    /* 0x04 */ UnkColliHitFields_8002686C *m_next;
    /* 0x08 */ u8 m_allocated;
    /* 0x0C */ UnkColliContact_8001FF00 m_contact;
    /* 0x44 */ float m_length;
};

namespace SonicteamUSA {
namespace System {
namespace Collision {

/* A result (0x24 bytes): the collider found, the shapes on either side and the contact; the links
 * of its list (or of its pool) and its flags (bit 0: from the heap, bit 1: a list's end). */
class CollisionResult {
  public:
    /* From the result pool, or from the heap (fn_80026518). */
    static void *operator new(size_t size);

    CollisionResult();

    bool IsEnd() const {
        return (m_flags >> 1) & 1;
    }

    /* 0x00 */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x08 */ RangeBase *m_ownShape;
    /* 0x0C */ RangeBase *m_shape;
    /* 0x10 */ CharaColliBase *m_raw;
    /* 0x14 */ UnkColliHitFields_8002686C *m_hit;
    /* 0x18 */ CollisionResult *m_prev;
    /* 0x1C */ CollisionResult *m_next;
    /* 0x20 */ u32 m_flags;
};

/* A list of results (0x1C bytes), ended by a result of its own; the links of its pool and whether
 * it came from the heap. */
class CollisionResultList {
  public:
    /* 0x00 */ void *m_pool;
    /* 0x04 */ u32 m_count;
    /* 0x08 */ CollisionResult *m_first;
    /* 0x0C */ CollisionResult *m_end;
    /* 0x10 */ CollisionResultList *m_prev;
    /* 0x14 */ CollisionResultList *m_next;
    /* 0x18 */ u8 m_allocated;
};

} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResult;
using SonicteamUSA::System::Collision::CollisionResultList;

namespace boost {
/* Out of line in the game (boost 1.32's definition). */
template <> inline void weak_ptr<CharaColliBase>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* A pool: its elements, the first free one, the number of elements and the number in use. */
template <class T> struct UnkColliPool_80026964 {
    /* 0x0 */ T *m_array;
    /* 0x4 */ T *m_free;
    /* 0x8 */ s32 m_capacity;
    /* 0xC */ s32 m_used;
};

typedef UnkColliPool_80026964<CollisionResult> ResultPool;
typedef UnkColliPool_80026964<CollisionResultList> ListPool;
typedef UnkColliPool_80026964<UnkColliHitFields_8002686C> HitPool;

/* A position in a list. */
struct UnkResultIter_8001EDFC {
    UnkResultIter_8001EDFC() : m_node(0) {}

    /* 0x0 */ CollisionResult *m_node;
};

typedef UnkResultIter_8001EDFC ResultIter;

/* Room for a position that fn_8001EDFC constructs. */
struct UnkResultIterStorage_8001EDFC {
    CollisionResult *m_node;
};

extern "C" {
/* Heap allocation and release. */
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *memory);
void *__register_global_object(void *object, void *destructor, void *record);
void *__nwa__FUl(u32 size);
void *__construct_new_array(void *block, void *ctor, void *dtor, u32 size, u32 count);
void __destroy_new_array(void *block, void *dtor);
void __dl__FPv(void *p);

/* The list pool and the contact pool (Game/UnkCollisionQuery_80020D40.cpp). */
ListPool *fn_8001FB68(void);
HitPool *fn_8001FD38(void);
/* A list's first position, its end, and a position's constructor (the same unit). */
CollisionResult *fn_8001EE04(CollisionResultList *list);
CollisionResult *fn_8001EDEC(CollisionResultList *list);
ResultIter *fn_8001EDFC(ResultIter *it, CollisionResult *node);

void fn_800273B8(ListPool *pool, CollisionResultList *list);
void fn_80027398(ResultIter *it);
ResultIter fn_80027374(ResultIter *it, int);
CollisionResult *fn_8002735C(CollisionResultList *list);
ResultPool *fn_80027284(ResultPool *pool, int capacity);
ResultPool *fn_80027220(ResultPool *pool, short flags);
CollisionResult *fn_800271A8(ResultPool *pool);
void fn_80027170(ResultPool *pool, CollisionResult *result);
/* The result's constructor, by its link name (for the pool's array). */
CollisionResult *__ct__Q412SonicteamUSA6System9Collision15CollisionResultFv(CollisionResult *result);
CollisionResultList *fn_800270CC(CollisionResultList *list);
void fn_80027054(CollisionResultList *list);
CollisionResultList *fn_80026FB4(CollisionResultList *list, short flags);
void fn_80026AAC(CollisionResultList *list, const ResultIter &it);
ListPool *fn_80026964(ListPool *pool, int capacity);
ListPool *fn_80026900(ListPool *pool, short flags);
UnkColliHitFields_8002686C *fn_800267F8(UnkColliContact_8001FF00 *contact, const UnkColliContact_8001FF00 *from);
void fn_80026718(UnkColliHitFields_8002686C *hit);
UnkColliHitFields_8002686C *fn_800266DC(UnkColliHitFields_8002686C *hit, short flags);
CollisionResult *fn_800265A4(CollisionResult *result, short flags);
void *fn_80026518(u32 size);
ResultPool *fn_800264A8(void);
void fn_80026430(CollisionResult *result);
}

/* The result pool's instance (Game/UnkCollisionQuery_80020D40.cpp). */
extern ResultPool *lbl_805EE078;

inline void *CollisionResult::operator new(size_t size) {
    return fn_80026518(size);
}

/* Inserts result before the position it (nothing when it is at no result). */
inline void InsertBefore(CollisionResultList *list, const ResultIter *it, CollisionResult *result) {
    CollisionResult *node = it->m_node;
    if (node == 0) {
        return;
    }
    result->m_prev = node->m_prev;
    result->m_next = node;
    if (node->m_prev != 0) {
        node->m_prev->m_next = result;
    } else {
        list->m_first = result;
    }
    node->m_prev = result;
    list->m_count++;
}

/* Releases a list back to its pool. */
extern "C" void fn_800273B8(ListPool *pool, CollisionResultList *list) {
    pool->m_free->m_prev = list;
    list->m_next = pool->m_free;
    list->m_prev = 0;
    pool->m_free = list;
    pool->m_used--;
}

/* ++it */
extern "C" void fn_80027398(ResultIter *it) {
    it->m_node = it->m_node != 0 ? it->m_node->m_next : 0;
}

/* it++ */
extern "C" ResultIter fn_80027374(ResultIter *it, int) {
    ResultIter old = *it;
    it->m_node = old.m_node != 0 ? old.m_node->m_next : 0;
    return old;
}

/* The list's first result, or 0 when it is empty. */
extern "C" CollisionResult *fn_8002735C(CollisionResultList *list) {
    if (list->m_first->IsEnd()) {
        return 0;
    }
    return list->m_first;
}

/* The result pool's constructor. */
extern "C" ResultPool *fn_80027284(ResultPool *pool, int capacity) {
    if ((pool->m_array = static_cast<CollisionResult *>(__construct_new_array(
        __nwa__FUl(capacity * sizeof(CollisionResult) + 0x10), (void *)__ct__Q412SonicteamUSA6System9Collision15CollisionResultFv, (void *)fn_800265A4,
        sizeof(CollisionResult), capacity))) == 0) {
        for (;;) {
        }
    }
    pool->m_capacity = capacity;
    pool->m_used = 0;
    pool->m_free = pool->m_array;
    CollisionResult *node = pool->m_free;
    for (int i = 0; i < capacity; i++, node++) {
        node->m_collider.reset();
        if (i == 0) {
            node->m_prev = 0;
        } else {
            node->m_prev = node - 1;
        }
        if (i == capacity - 1) {
            node->m_next = 0;
        } else {
            node->m_next = node + 1;
        }
    }
    return pool;
}

/* The result pool's destructor. */
extern "C" ResultPool *fn_80027220(ResultPool *pool, short flags) {
    if (pool != 0) {
        if (pool->m_array != 0) {
            __destroy_new_array(pool->m_array, (void *)fn_800265A4);
            pool->m_array = 0;
        }
        if (flags > 0) {
            __dl__FPv(pool);
        }
    }
    return pool;
}

/* Takes a result from the pool (0 when it is used up). */
extern "C" CollisionResult *fn_800271A8(ResultPool *pool) {
    CollisionResult *result = pool->m_free;
    if (pool->m_used == pool->m_capacity - 1) {
        return 0;
    }
    result->m_collider.reset();
    result->m_next->m_prev = 0;
    pool->m_used++;
    pool->m_free = result->m_next;
    result->m_next = 0;
    return result;
}

/* Gives a result back to the pool. */
extern "C" void fn_80027170(ResultPool *pool, CollisionResult *result) {
    pool->m_free->m_prev = result;
    result->m_next = pool->m_free;
    result->m_prev = 0;
    result->m_flags &= ~2;
    pool->m_free = result;
    pool->m_used--;
}

CollisionResult::CollisionResult() : m_raw(0), m_hit(0) {}

/* The list's constructor: an empty list, its end result marked as such. */
extern "C" CollisionResultList *fn_800270CC(CollisionResultList *list) {
    list->m_pool = fn_800264A8();
    CollisionResult *end = new CollisionResult;
    end->m_flags |= 2;
    list->m_end = end;
    list->m_first = end;
    list->m_count = 0;
    return list;
}

/* The list's operator delete: back to the list pool, or to the heap. */
extern "C" void fn_80027054(CollisionResultList *list) {
    if (list == 0) {
        return;
    }
    if (list->m_prev != 0) {
        list->m_prev->m_next = list->m_next;
    }
    if (list->m_next != 0) {
        list->m_next->m_prev = list->m_prev;
    }
    if (!list->m_allocated) {
        fn_800273B8(fn_8001FB68(), list);
    } else {
        fn_80049B34(list);
    }
}

/* The list's destructor. */
extern "C" CollisionResultList *fn_80026FB4(CollisionResultList *list, short flags) {
    if (list != 0) {
        CollisionResult *result = list->m_first;
        while (result != list->m_end) {
            CollisionResult *next = result->m_next;
            fn_800265A4(result, 1);
            result = next;
        }
        if (list->m_end != 0) {
            list->m_end->m_prev = 0;
        }
        list->m_first = list->m_end;
        list->m_count = 0;
        fn_800265A4(list->m_first, 1);
        list->m_end = 0;
        list->m_first = 0;
        if (flags > 0) {
            fn_80027054(list);
        }
    }
    return list;
}

/* Inserts result before the first result whose move is not shorter. */
extern "C" void fn_80026EA0(CollisionResultList *list, CollisionResult *result) {
    ResultIter it;
    UnkResultIterStorage_8001EDFC first;
    fn_8001EDFC(reinterpret_cast<ResultIter *>(&first), fn_8001EE04(list));
    for (it = reinterpret_cast<ResultIter &>(first); it.m_node != fn_8001EDEC(list); fn_80027374(&it, 0)) {
        if (it.m_node->m_hit == 0 || result->m_hit->m_length <= it.m_node->m_hit->m_length) {
            InsertBefore(list, &it, result);
            return;
        }
    }
    InsertBefore(list, &it, result);
}

/* Inserts result before the first result whose contact is not nearer. */
extern "C" void fn_80026D8C(CollisionResultList *list, CollisionResult *result) {
    ResultIter it;
    UnkResultIterStorage_8001EDFC first;
    fn_8001EDFC(reinterpret_cast<ResultIter *>(&first), fn_8001EE04(list));
    for (it = reinterpret_cast<ResultIter &>(first); it.m_node != fn_8001EDEC(list); fn_80027374(&it, 0)) {
        if (it.m_node->m_hit == 0 ||
            result->m_hit->m_contact.m_distance <= it.m_node->m_hit->m_contact.m_distance) {
            InsertBefore(list, &it, result);
            return;
        }
    }
    InsertBefore(list, &it, result);
}

/* Inserts result before the first result whose move is not shorter, then removes the results
 * whose move is longer than result's by more than a margin. */
extern "C" void fn_80026BF4(CollisionResultList *list, CollisionResult *result) {
    ResultIter it;
    UnkResultIterStorage_8001EDFC at;
    UnkResultIterStorage_8001EDFC first;
    fn_8001EDFC(reinterpret_cast<ResultIter *>(&first), fn_8001EE04(list));
    for (it = reinterpret_cast<ResultIter &>(first); it.m_node != fn_8001EDEC(list); fn_80027374(&it, 0)) {
        if (result->m_hit->m_length <= it.m_node->m_hit->m_length) {
            InsertBefore(list, &it, result);
            break;
        }
    }
    if (it.m_node == fn_8001EDEC(list)) {
        InsertBefore(list, &it, result);
    } else {
        at.m_node = 0;
        while (it.m_node != fn_8001EDEC(list)) {
            at.m_node = it.m_node;
            fn_80027374(&it, 0);
            if (0.001f + result->m_hit->m_length < at.m_node->m_hit->m_length) {
                fn_80026AAC(list, reinterpret_cast<ResultIter &>(at));
            }
        }
    }
}

/* Inserts result at the front. */
extern "C" void fn_80026BB4(CollisionResultList *list, CollisionResult *result) {
    result->m_prev = 0;
    result->m_next = list->m_first;
    if (list->m_first->m_next != 0) {
        list->m_first->m_next->m_prev = result;
    }
    list->m_first->m_prev = result;
    list->m_first = result;
    list->m_count++;
}

/* Inserts result at the back (before the end result). */
extern "C" void fn_80026B64(CollisionResultList *list, CollisionResult *result) {
    result->m_prev = list->m_end->m_prev;
    result->m_next = list->m_end;
    if (list->m_end->m_prev != 0) {
        list->m_end->m_prev->m_next = result;
    }
    list->m_end->m_prev = result;
    if (list->m_count == 0) {
        list->m_first = result;
    }
    list->m_count++;
}

/* Inserts result before the position it. */
extern "C" void fn_80026B20(CollisionResultList *list, const ResultIter &it, CollisionResult *result) {
    CollisionResult *node = it.m_node;
    if (node == 0) {
        return;
    }
    result->m_prev = node->m_prev;
    result->m_next = it.m_node;
    if (node->m_prev != 0) {
        node->m_prev->m_next = result;
    } else {
        list->m_first = result;
    }
    node->m_prev = result;
    list->m_count++;
}

/* Removes and destroys the result at it. */
extern "C" void fn_80026AAC(CollisionResultList *list, const ResultIter &it) {
    CollisionResult *node = it.m_node;
    if (node != 0) {
        if (node->m_prev == 0) {
            list->m_first = node->m_next;
        }
        if (node->m_next == 0) {
            list->m_end = node->m_prev;
        }
        fn_800265A4(node, 1);
        list->m_count--;
    }
}

/* Destroys every result but the end one. */
extern "C" void fn_80026A40(CollisionResultList *list) {
    CollisionResult *result = list->m_first;
    while (result != list->m_end) {
        CollisionResult *next = result->m_next;
        fn_800265A4(result, 1);
        result = next;
    }
    if (list->m_end != 0) {
        list->m_end->m_prev = 0;
    }
    list->m_first = list->m_end;
    list->m_count = 0;
}

/* The list pool's constructor. */
extern "C" ListPool *fn_80026964(ListPool *pool, int capacity) {
    if ((pool->m_array = static_cast<CollisionResultList *>(__construct_new_array(
        __nwa__FUl(0x20 * sizeof(CollisionResultList) + 0x10), (void *)fn_800270CC, (void *)fn_80026FB4,
        sizeof(CollisionResultList), 0x20))) == 0) {
        for (;;) {
        }
    }
    pool->m_capacity = capacity;
    pool->m_used = 0;
    pool->m_free = pool->m_array;
    CollisionResultList *node = pool->m_free;
    for (int i = 0; i < capacity; i++, node++) {
        if (i == 0) {
            node->m_prev = 0;
        } else {
            node->m_prev = node - 1;
        }
        if (i == capacity - 1) {
            node->m_next = 0;
        } else {
            node->m_next = node + 1;
        }
        fn_800265A4(node->m_first, 1);
        node->m_end = 0;
        node->m_first = 0;
    }
    return pool;
}

/* The list pool's destructor. */
extern "C" ListPool *fn_80026900(ListPool *pool, short flags) {
    if (pool != 0) {
        if (pool->m_array != 0) {
            __destroy_new_array(pool->m_array, (void *)fn_80026FB4);
            pool->m_array = 0;
        }
        if (flags > 0) {
            __dl__FPv(pool);
        }
    }
    return pool;
}

/* Takes a list from the pool (0 when it is used up). */
extern "C" void fn_800268B0(ListPool *pool, CollisionResultList **list) {
    CollisionResultList *free = pool->m_free;
    if (pool->m_used == pool->m_capacity - 1) {
        *list = 0;
        return;
    }
    free->m_next->m_prev = 0;
    pool->m_used++;
    pool->m_free = free->m_next;
    free->m_next = 0;
    *list = free;
}

/* A contact from a shape's contact (the hit's fields are UnkColliHitFields_8002686C). */
extern "C" UnkColliHit_8002686C *fn_8002686C(UnkColliHit_8002686C *hit, RangeBase *shape,
                                            const UnkColliContact_8001FF00 *contact) {
    fn_800267F8(&reinterpret_cast<UnkColliHitFields_8002686C *>(hit)->m_contact, contact);
    reinterpret_cast<UnkColliHitFields_8002686C *>(hit)->m_length = contact->m_distance;
    return hit;
}

/* The contact's copy. */
extern "C" UnkColliHitFields_8002686C *fn_800267F8(UnkColliContact_8001FF00 *contact,
                                                  const UnkColliContact_8001FF00 *from) {
    fn_800091EC(reinterpret_cast<Vec3f *>(&contact->m_position), reinterpret_cast<const Vec3f *>(&from->m_position));
    contact->m_distance = from->m_distance;
    contact->field_0x10 = from->field_0x10;
    fn_800091EC(reinterpret_cast<Vec3f *>(&contact->m_normal), reinterpret_cast<const Vec3f *>(&from->m_normal));
    fn_800091EC(reinterpret_cast<Vec3f *>(&contact->field_0x20), reinterpret_cast<const Vec3f *>(&from->field_0x20));
    contact->field_0x2C = from->field_0x2C;
    contact->field_0x30 = from->field_0x30;
    contact->field_0x34 = from->field_0x34;
    return reinterpret_cast<UnkColliHitFields_8002686C *>(contact);
}

/* A contact from a shape's contact, with the length of the move that found it. */
extern "C" UnkColliHitFields_8002686C *fn_800267B0(UnkColliHitFields_8002686C *hit, RangeBase *shape,
                                                  const UnkColliContact_8001FF00 *contact, float length) {
    fn_800267F8(&hit->m_contact, contact);
    hit->m_length = length;
    return hit;
}

/* The contact's operator delete: back to the contact pool, or to the heap. */
extern "C" void fn_80026718(UnkColliHitFields_8002686C *hit) {
    if (hit == 0) {
        return;
    }
    if (hit->m_prev != 0) {
        hit->m_prev->m_next = hit->m_next;
    }
    if (hit->m_next != 0) {
        hit->m_next->m_prev = hit->m_prev;
    }
    if (!hit->m_allocated) {
        HitPool *pool = fn_8001FD38();
        pool->m_free->m_prev = hit;
        hit->m_next = pool->m_free;
        hit->m_prev = 0;
        pool->m_free = hit;
        pool->m_used--;
    } else {
        fn_80049B34(hit);
    }
}

/* The contact's destructor. */
extern "C" UnkColliHitFields_8002686C *fn_800266DC(UnkColliHitFields_8002686C *hit, short flags) {
    if (hit != 0 && flags > 0) {
        fn_80026718(hit);
    }
    return hit;
}

/* The contact's normal. */
extern "C" Vec3f *fn_800266D4(UnkColliHitFields_8002686C *hit) {
    return reinterpret_cast<Vec3f *>(&hit->m_contact.m_normal);
}

/* The result's destructor: it destroys the shape it made (a triangle of the stage, when no collider
 * was found) and its contact, unless it is a list's end. */
extern "C" CollisionResult *fn_800265A4(CollisionResult *result, short flags) {
    if (result != 0) {
        RangeBase *shape = result->m_shape;
        if (!(result->m_flags & 2) && result->m_raw == 0 && shape != 0 && shape->m_priority == -1) {
            delete shape;
        }
        if (!(result->m_flags & 2) && result->m_hit != 0) {
            fn_800266DC(result->m_hit, 1);
        }
        result->m_collider.reset();
        result->m_collider.~weak_ptr();
        if (flags > 0) {
            fn_80026430(result);
        }
    }
    return result;
}

/* The attributes of the shape hit (0 when there is none). */
extern "C" u32 fn_80026588(CollisionResult *result) {
    if (result->m_shape != 0) {
        return result->m_shape->m_attributes;
    }
    return 0;
}

/* The result's operator new: from the result pool, or from the heap. */
extern "C" void *fn_80026518(u32 size) {
    CollisionResult *result = fn_800271A8(fn_800264A8());
    if (result != 0) {
        result->m_flags = 0;
    } else {
        result = static_cast<CollisionResult *>(fn_80049B90(size, 2));
        if (result != 0) {
            result->m_flags = 1;
        }
    }
    result->m_next = 0;
    result->m_prev = 0;
    result->m_hit = 0;
    return result;
}

/* The result pool (a function-local static). */
static s8 lbl_805EE088;
static u32 lbl_80570CD0[3];
static ResultPool lbl_80570CDC;

extern "C" ResultPool *fn_800264A8(void) {
    if (lbl_805EE078 == 0) {
        if (!lbl_805EE088) {
            __register_global_object(fn_80027284(&lbl_80570CDC, 0x80), (void *)fn_80027220, lbl_80570CD0);
            lbl_805EE088 = 1;
        }
        lbl_805EE078 = &lbl_80570CDC;
    }
    return lbl_805EE078;
}

/* The result's operator delete: back to the result pool, or to the heap. */
extern "C" void fn_80026430(CollisionResult *result) {
    if (result == 0) {
        return;
    }
    if (result->m_prev != 0) {
        result->m_prev->m_next = result->m_next;
    }
    if (result->m_next != 0) {
        result->m_next->m_prev = result->m_prev;
    }
    if (!(result->m_flags & 1)) {
        fn_80027170(fn_800264A8(), result);
    } else {
        fn_80049B34(result);
    }
}
