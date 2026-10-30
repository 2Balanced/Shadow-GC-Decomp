/* The collision queries (class name unknown; fn_80020D40 returns the object, which has no data):
 * a sphere moved through the stage (fn_80021934), a sphere at rest (fn_80021CA4), the first and all
 * hits of a line (fn_80021E24, fn_80021FE4). Each tests the RenderWare worlds of the stage's blocks
 * (through RpCollisionWorldForAllIntersections and, for a moving sphere, its own walk of the world
 * sectors and their collision trees) and then the characters' colliders (through the collider
 * manager, fn_80021B08). A hit becomes a CollisionResult (class name from the RTTI of its
 * shared_ptr control block) that holds a RangeTriangle for a world triangle and the contact
 * (UnkColliHit_8002686C); the hits are collected in a CollisionResultList. The unit also has the
 * pools the triangles, contacts and lists come from, and removes the contacts with a triangle's
 * edge or corner that lie on a neighbouring triangle's face (fn_8001E9F8). File name from the
 * object's placeholder; other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). Most functions keep their C names:
 * other units call them by those names. The const structs in .rodata (@N) are the constant parts
 * of the filter queries' initializers.
 *
 * NonMatching (48 of 53 functions match):
 * - fn_8001EE0C and fn_800201D4: the code is the game's, but GC/2.7 gives the variables other
 *   callee-saved registers (fn_8001EE0C: the game puts the function's own locals in r31 down and
 *   the arguments below them; fn_800201D4: the game has the shape in r31 and the triangle in r30).
 * - fn_8001FD6C and fn_80020088: the game copies the loop counter from the zero register (one
 *   instruction).
 * - fn_80021E24: the game has the line's parameter above the blocks' handle on the stack.
 * - Data: the function-local statics are written out here (their constructors and destructors have C names); the game's are real
 *   ones, which puts the pools' storage after AttackIgnorePoolObject's in .bss, and without
 *   that object's destructor record (this unit sees no destructor for it). */
#define ATTACK_IGNORE_LIST_MEMBERS
#include "Game/AttackIgnoreList.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/DebugDisp.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include "Game/RangeTriangle.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"
#include <new>

using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

namespace std {
/* MSL's std::fabs for floats, which the game calls out of line. */
float fabs(float x);
} // namespace std

/* ---- RenderWare's collision types (rwcore/rpworld/rpcollis) ---- */

struct RwV3d {
    float x;
    float y;
    float z;
};

struct RwBBox {
    /* 0x00 */ RwV3d sup;
    /* 0x0C */ RwV3d inf;
};

struct RwSphere {
    /* 0x00 */ RwV3d center;
    /* 0x0C */ float radius;
};

struct RwLine {
    /* 0x00 */ RwV3d start;
    /* 0x0C */ RwV3d end;
};

struct RpIntersection {
    union {
        RwLine line;
        RwSphere sphere;
        RwBBox box;
    } t;
    /* 0x18 */ s32 type;
};

struct RpCollisionTriangle {
    /* 0x00 */ RwV3d normal;
    /* 0x0C */ RwV3d point;
    /* 0x18 */ s32 index;
    /* 0x1C */ RwV3d *vertices[3];
};
SIZE_ASSERT(RpCollisionTriangle, 0x28);

/* A world triangle: its vertices' indices and its material's. */
struct RpTriangle {
    /* 0x0 */ u16 vertIndex[3];
    /* 0x6 */ s16 matIndex;
};

/* A node of a collision tree: a leaf (count triangles from index) or, when count is 0xFF, a split
 * (index is the split's). */
struct RpCollTreeNode {
    /* 0x0 */ u8 type;
    /* 0x1 */ u8 count;
    /* 0x2 */ u16 index;
};

struct RpCollTreeSplit {
    /* 0x0 */ RpCollTreeNode left;
    /* 0x4 */ float leftValue;
    /* 0x8 */ RpCollTreeNode right;
    /* 0xC */ float rightValue;
};

struct RpCollTree {
    /* 0x00 */ u32 flags;
    /* 0x04 */ RwBBox bbox;
    /* 0x1C */ u16 numTriangles;
    /* 0x1E */ u16 numSplits;
    /* 0x20 */ RpCollTreeSplit *splits;
    /* 0x24 */ u16 *map;
};

/* A sector of a world: a plane sector (type 0, 4 or 8: the axis) or, below 0, a world sector. */
struct RpSector {
    /* 0x00 */ s32 type;
};

struct RpPlaneSector : public RpSector {
    /* 0x04 */ float value;
    /* 0x08 */ RpSector *leftSubTree;
    /* 0x0C */ RpSector *rightSubTree;
    /* 0x10 */ float leftValue;
    /* 0x14 */ float rightValue;
};

struct RpWorldSector : public RpSector {
    /* 0x04 */ RpTriangle *triangles;
    /* 0x08 */ RwV3d *vertices;
};

struct RpWorld {
    /* 0x00 */ u8 field_0x0[0x1C];
    /* 0x1C */ RpSector *rootSector;
};

/* A material list (only the materials). */
struct RpMaterialList {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ void **materials;
};

typedef RpCollisionTriangle *(*RpIntersectionCallBackWorldTriangle)(RpIntersection *intersection,
                                                                     RpWorldSector *sector,
                                                                     RpCollisionTriangle *triangle,
                                                                     float distance, void *data);

/* The collision plugin's offset in a world sector. */
extern s32 lbl_805F1420;

extern "C" {
RpWorld *fn_80442AA4(RpWorld *world, RpIntersection *intersection,
                     RpIntersectionCallBackWorldTriangle callBack, void *data);
/* Whether a world sector's world has materials, and the world's material list. */
s32 fn_80452C04(RpWorldSector *sector);
RpMaterialList *fn_80452C4C(RpWorldSector *sector, int arg1);
/* 1 / sqrt(x). */
float fn_80482394(float x);
}

/* ---- The collision shapes (Game/RangeBase.h, Game/RangeTriangle.h) ---- */

using SonicteamUSA::System::Range::RangeBase;
using SonicteamUSA::System::Range::RangeTriangle;

extern "C" {
/* RangeTriangle's constructors (from a world triangle and its material; the pool's default) and
 * destructor, by their names: their addresses are taken. */
RangeTriangle *__ct__Q412SonicteamUSA6System5Range13RangeTriangleFPC24UnkColliPolygon_80036038Ul(
    void *triangle, const RpCollisionTriangle *from, void *material);
RangeTriangle *__ct__Q412SonicteamUSA6System5Range13RangeTriangleFv(void *triangle);
void *__dt__Q412SonicteamUSA6System5Range13RangeTriangleFv(void *triangle, short flags);
}

/* ---- Contacts and results ---- */

/* A contact as the results hold it (UnkColliHit_8002686C): pooled, with the contact of
 * Game/RangeBase.h. */
struct UnkColliHit {
    /* 0x00 */ UnkColliHit *m_prev;
    /* 0x04 */ UnkColliHit *m_next;
    /* 0x08 */ u8 m_unpooled;
    /* 0x0C */ UnkColliContact_8001FF00 m_contact;
    /* 0x44 */ float field_0x44;
};
SIZE_ASSERT(UnkColliHit, 0x48);

namespace SonicteamUSA {
namespace System {
namespace Collision {
/* CollisionResult (class name from the RTTI of its shared_ptr control block): a shape hit and its
 * contact. Allocated by fn_80026518, deleted by fn_800265A4. */
class CollisionResult {
  public:
    /* fn_80026518 (a later unit). */
    static void *operator new(size_t size);

    CollisionResult(RangeBase *shape, UnkColliHit *hit, s32 arg);

    /* 0x00 */ boost::weak_ptr<CharaColliBase> m_owner;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ RangeBase *m_shape;
    /* 0x10 */ CollisionResult *m_next;
    /* 0x14 */ UnkColliHit *m_hit;
    /* 0x18 */ u8 field_0x18[0x24 - 0x18];
};

/* CollisionResultList (class name from the RTTI of its shared_ptr control block): the results of
 * a query. Its members are in a later unit (constructor 0x800270CC). */
class CollisionResultList {
  public:
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CollisionResult *m_first;
    /* 0x0C */ CollisionResult *m_end;
    /* 0x10 */ CollisionResultList *m_prev;
    /* 0x14 */ CollisionResultList *m_next;
    /* 0x18 */ bool m_allocated;
};
SIZE_ASSERT(CollisionResultList, 0x1C);
} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResult;
using SonicteamUSA::System::Collision::CollisionResultList;

extern "C" void *fn_80026518(u32 size);

inline void *CollisionResult::operator new(size_t size) {
    return fn_80026518(size);
}

/* An iterator over a list's results (constructed out of line by fn_8001EDFC). */
struct UnkResultIter;
extern "C" UnkResultIter *fn_8001EDFC(UnkResultIter *it, CollisionResult *node);

struct UnkResultIter {
    UnkResultIter() : m_node(0) {}

    /* 0x0 */ CollisionResult *m_node;
};

/* Room for an iterator that fn_8001EDFC constructs. */
struct UnkResultIterStorage {
    CollisionResult *m_node;
};

extern "C" {
/* The list's members (a later unit): constructor, deleting destructor, adding a result first, in
 * order of distance, last, after others; erasing at an iterator. */
CollisionResultList *fn_800270CC(void *list);
void fn_80026FB4(CollisionResultList *list, int flags);
void fn_80026BB4(CollisionResultList *list, CollisionResult *result);
void fn_80026BF4(CollisionResultList *list, CollisionResult *result);
void fn_80026D8C(CollisionResultList *list, CollisionResult *result);
void fn_80026EA0(CollisionResultList *list, CollisionResult *result);
void fn_80026AAC(CollisionResultList *list, UnkResultIter *it);
UnkResultIter fn_80027374(UnkResultIter *it, int);
/* The list pool's allocation (a later unit). */
void fn_800268B0(void *pool, CollisionResultList **list);
/* A result's deleting destructor. */
void fn_800265A4(CollisionResult *result, int flags);
/* A contact's constructor from a shape and contact, and from those and a squared length. */
UnkColliHit *fn_800267B0(UnkColliHit *hit, RangeBase *shape, const UnkColliContact_8001FF00 *contact,
                         float length2);
/* The contact pool's element destructor. */
void *fn_800266DC(void *hit, short flags);
/* Allocation from a heap. */
void *fn_80049B90(u32 size, int heap);
/* The list pool's constructor and destructor. */
void *fn_80026964(void *pool, int capacity);
void *fn_80026900(void *pool, short flags);
void *__register_global_object(void *object, void *destructor, void *record);
void *__nwa__FUl(u32 size);
void *__construct_new_array(void *block, void *ctor, void *dtor, u32 size, u32 count);
void __destroy_new_array(void *block, void *dtor);
void __dl__FPv(void *p);
}

/* ---- Vector helpers ---- */

extern "C" {
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040D180(const Vec3f *a, const Vec3f *b);
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
float fn_8000FEE4(float x);
}

namespace ret {
extern "C" Vec3f fn_8040CDA8(const Vec3f *v, float scale);
extern "C" Vec3f fn_8040CD74(const Vec3f *a, const Vec3f *b);
extern "C" Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
extern "C" Vec3f fn_8040CEE8(const Vec3f *v);
extern "C" Vec3f fn_80016284(const Vec3f *v);
extern "C" Vec3f fn_800169F8(const Vec3f *v);
extern "C" RwV3d fn_8001DFE8(const Vec3f *v);
} // namespace ret

/* A vector copied and assigned through the out-of-line copy constructor and assignment. */
struct ColliVector {
    ColliVector() {}
    ColliVector(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }
    ColliVector &operator=(const Vec3f &v) {
        fn_800091D0(reinterpret_cast<Vec3f *>(this), &v);
        return *this;
    }
    const Vec3f *Ptr() const {
        return reinterpret_cast<const Vec3f *>(this);
    }
    Vec3f *Ptr() {
        return reinterpret_cast<Vec3f *>(this);
    }

    float x;
    float y;
    float z;
};

inline bool operator==(const ColliVector &a, const ColliVector &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

/* ---- The query object and its statics ---- */

/* The query object (class name unknown). */
struct UnkCollisionQuery_80020D40 {
    ~UnkCollisionQuery_80020D40() {}
};

typedef UnkCollisionQuery_80020D40 CollisionQuery;

/* What the triangle callbacks pass on (statics of the unit). */
static CollisionResultList *lbl_805EE044;   /* the results */
static CollisionResultList *lbl_805EE048;   /* the results behind the sphere, if collected */
static void *lbl_805EE04C;                  /* the material of the nearest triangle */
static CharaColliBase::Filter lbl_805EE050;      /* the filter */
static u32 lbl_805EE054;                    /* the filter's argument */

/* The up vector (other units read it). */
Vec3f lbl_80570C00(0.0f, 1.0f, 0.0f);
/* The nearest triangle of a line. */
static RpCollisionTriangle lbl_80570C0C;

/* The collider manager (Game/CharaColliManage; constructor and destructor out of line). */
struct UnkColliderManager_80021B08;
extern "C" {
UnkColliderManager_80021B08 *__ct__27UnkColliderManager_80021B08Fv(void *manager);
void *__dt__27UnkColliderManager_80021B08Fv(void *manager, short flags);
}

/* The pools. */
template <class T> struct UnkPool {
    /* 0x0 */ T *m_array;
    /* 0x4 */ T *m_free;
    /* 0x8 */ s32 m_capacity;
    /* 0xC */ s32 m_used;
};

/* The function-local statics (written out; the constructors and destructors have C names). */
static s8 lbl_805EE058;
static s8 lbl_805EE059;
static s8 lbl_805EE05A;
static u32 lbl_805EE05C;
static s8 lbl_805EE060;
static s8 lbl_805EE061;
static s8 lbl_805EE062;
static u32 lbl_805EE064;

static u32 lbl_80570C34[3];
static u32 lbl_80570C40[3];
static u32 lbl_80570C4C[3];
static u32 lbl_80570C58[3];
static u32 lbl_80570C64[3];
static u32 lbl_80570C80[4];
static u32 lbl_80570C90[7];
static UnkPool<RangeTriangle> lbl_80570CAC;
static UnkPool<UnkColliHit> lbl_80570CBC;

/* The instances. */
UnkColliderManager_80021B08 *lbl_805EE068;
UnkAttackIgnorePool_800535A4 *lbl_805EE06C;
CollisionQuery *lbl_805EE070;
void *lbl_805EE074;
void *lbl_805EE078;
void *lbl_805EE07C;
UnkPool<RangeTriangle> *lbl_805EE080;
/* The contact pool's instance (Game/UnkPathManager_8001B494.cpp). */
extern UnkPool<UnkColliHit> *lbl_805EE034;

/* The attack collider the queries pass to the collider manager (Game/CharaColliBaseCharInfo.cpp). */
extern boost::shared_ptr<CharaColliAttack> lbl_805EE6A0;

extern "C" {
/* The collider manager's queries: a moving sphere, a sphere, a line and the first hit of a line. */
CollisionResultList *fn_800C4DE0(UnkColliderManager_80021B08 *manager, const void *sphere, const Vec3f *move,
                                 u32 mask, int arg4, CollisionResultList *list, CharaColliBase::Filter filter,
                                 u32 arg, boost::weak_ptr<CharaColliAttack> attack, u32 arg9);
CollisionResultList *fn_800C5478(UnkColliderManager_80021B08 *manager, const void *sphere, u32 mask, int arg3,
                                 CollisionResultList *list, CharaColliBase::Filter filter, u32 arg,
                                 boost::weak_ptr<CharaColliAttack> attack);
CollisionResult *fn_800C5938(UnkColliderManager_80021B08 *manager, const UnkDebugLine_80020A20 *line,
                             u32 mask, int arg3, CharaColliBase::Filter filter, u32 arg,
                             boost::weak_ptr<CharaColliAttack> attack);
CollisionResultList *fn_800C5D74(UnkColliderManager_80021B08 *manager, const UnkDebugLine_80020A20 *line,
                                 u32 mask, int arg3, CollisionResultList *list, CharaColliBase::Filter filter, u32 arg,
                                 boost::weak_ptr<CharaColliAttack> attack);
/* The lock the line queries take (constructor 0x800379E4). */
void fn_800379E4(void *lock);
void fn_800379DC(void *lock);
void fn_800379E0(void *lock);
/* The closest point of a segment to a point, and its distance. */
float fn_8003C63C(const Vec3f *point, const UnkDebugLine_80020A20 *segment, Vec3f *closest);
/* The tests of a sphere moving against a triangle. */
bool fn_800221A8(const Vec3f *center, Vec3f *move, RwV3d **vertices, Vec3f *point, float *distance, s32 *type,
                 s32 *index, s32 arg7, s32 arg8, bool arg9, float radius);
bool fn_80024AC8(const Vec3f *center, RwV3d **vertices, Vec3f *point, float *distance, s32 *type, s32 *index,
                 s32 arg6, s32 arg7, Vec3f *move, bool arg9, s32 arg10, float radius);
}

/* ---- The stage's blocks ---- */

/* A handle of the stage's blocks (struct name unknown). */
struct UnkHandle_80021C80 {
    UnkHandle_80021C80(const UnkHandle_80021C80 &other) : field_0x0(other.field_0x0), field_0x2(other.field_0x2) {}

    /* 0x0 */ u16 field_0x0;
    /* 0x2 */ s16 field_0x2;
};

/* A block (the world is at 0xB8). */
struct UnkBlock_8003A540 {
    /* 0x00 */ u8 field_0x0[0xB8];
    /* 0xB8 */ RpWorld *m_world;
};

struct UnkBlockSetBase_80045A98 {
    /* 0x00 */ u8 field_0x0[0x4];
};

/* The stage's blocks (class name unknown); the ids of the blocks shown are at 0x828, ended by -1. */
struct UnkBlockSetHead_80045A98 {
    /* 0x00 */ u8 field_0x0[0x28];
};

struct UnkBlockSet_80045A98 : public UnkBlockSetHead_80045A98, public UnkBlockSetBase_80045A98 {
    /* 0x2C */ u8 field_0x2C[0x828 - 0x2C];
    /* 0x828 */ s16 m_shown[1];
};

extern UnkHandle_80021C80 lbl_80571484[];

/* The handle as the blocks take it (a copy). */
struct UnkBlockSetId_80045A98 {
    UnkBlockSetId_80045A98(const UnkHandle_80021C80 &handle)
        : field_0x0(handle.field_0x0), field_0x2(handle.field_0x2) {}

    /* 0x0 */ u16 field_0x0;
    /* 0x2 */ s16 field_0x2;
};

extern "C" {
UnkBlockSetBase_80045A98 *fn_80045A98(const UnkBlockSetId_80045A98 &id);
bool IsRequested__12TBlockManageFii(UnkBlockSet_80045A98 *blocks, int id, int state);
UnkBlock_8003A540 *fn_8003A540(UnkBlockSet_80045A98 *blocks, int id);
}

/* ---- Definitions ---- */

struct UnkSphere_80021C04;

extern "C" void *fn_8001FFA4(u32 size);
extern "C" void *fn_8001FC88(u32 size);

/* new expressions of the classes whose constructors have C names: allocate, then construct when
 * the allocation succeeded (the arguments are read after the allocation). */
#define NEW_TRIANGLE(var, triangle, material)                                                                   \
    var = static_cast<RangeTriangle *>(fn_8001FFA4(0x6C));                                                     \
    if (var != 0) {                                                                                            \
        var = __ct__Q412SonicteamUSA6System5Range13RangeTriangleFPC24UnkColliPolygon_80036038Ul(var, triangle, material);                                                            \
    }

#define NEW_HIT(var, shape, contact)                                                                            \
    var = static_cast<UnkColliHit *>(fn_8001FC88(0x48));                                                       \
    if (var != 0) {                                                                                            \
        var = reinterpret_cast<UnkColliHit *>(                                                                 \
            fn_8002686C(reinterpret_cast<UnkColliHit_8002686C *>(var), shape, contact));                       \
    }

#define NEW_MOVING_HIT(var, shape, contact, length2)                                                            \
    {                                                                                                          \
        UnkColliHit *hit_ = static_cast<UnkColliHit *>(fn_8001FC88(0x48));                                     \
        if (hit_ != 0) {                                                                                       \
            hit_ = fn_800267B0(hit_, shape, contact, length2);                                                 \
        }                                                                                                      \
        var = hit_;                                                                                            \
    }

#define NEW_LIST(var)                                                                                           \
    {                                                                                                          \
        CollisionResultList *list_ = static_cast<CollisionResultList *>(fn_8001FADC(0x1C));                    \
        if (list_ != 0) {                                                                                      \
            list_ = fn_800270CC(list_);                                                                        \
        }                                                                                                      \
        var = list_;                                                                                           \
    }

extern "C" {
UnkResultIter *fn_8001EDFC(UnkResultIter *it, CollisionResult *node);
CollisionResult *fn_8001EDEC(CollisionResultList *list);
CollisionResult *fn_8001EE04(CollisionResultList *list);
s32 fn_8001EDF4(UnkColliHit *hit);
s32 fn_8001E9F0(UnkColliHit *hit);
float fn_8001E9E8(UnkColliContact_8001FF00 *contact);
const Vec3f *fn_8001E9C4(UnkColliHit *hit);
const Vec3f *fn_8001E9BC(UnkColliHit *hit);
CollisionResultList *fn_8001EE0C(CollisionQuery *query, RpWorld *world, const Vec3f *center, Vec3f *move,
                                 s32 arg4, s32 arg5, CharaColliBase::Filter filter, u32 arg,
                                 CollisionResultList *list, bool arg9, float radius);
void *fn_8001FADC(u32 size);
void *fn_8001FB68(void);
void *fn_8001FC88(u32 size);
UnkPool<UnkColliHit> *fn_8001FD38(void);
UnkPool<UnkColliHit> *fn_8001FD6C(void);
void *fn_8001FE54(UnkPool<UnkColliHit> *pool, short flags);
UnkColliHit *fn_8001FEB8(UnkColliHit *hit);
UnkColliContact_8001FF00 *fn_8001FF1C(UnkColliContact_8001FF00 *contact, const Vec3f *position, float distance,
                                      float depth, const Vec3f *normal, s32 type, s32 index, s32 arg7,
                                      const Vec3f *other);
void *fn_8001FFA4(u32 size);
UnkPool<RangeTriangle> *fn_80020054(void);
UnkPool<RangeTriangle> *fn_80020088(void);
void *fn_80020170(UnkPool<RangeTriangle> *pool, short flags);
void fn_800201D4(CollisionQuery *query, RpIntersection *intersection, RpWorldSector *sector,
                 RpCollisionTriangle *triangle, float distance);
float fn_80020A60(const Vec3f *point, RwV3d a, RwV3d b, Vec3f *closest, s32 indexA, s32 indexEdge, s32 indexB,
                  s32 *type, s32 *index);
RpCollisionTriangle *fn_80020C24(RpIntersection *intersection, RpWorldSector *sector,
                                 RpCollisionTriangle *triangle, float distance, void *data);
CollisionQuery *fn_80020D40(void);
CollisionQuery *fn_80020D74(void);
RpCollisionTriangle *fn_80020DC0(RpIntersection *intersection, RpWorldSector *sector,
                                 RpCollisionTriangle *triangle, float distance, void *data);
void fn_80020F48(const Vec3f *a, const Vec3f *b, const Vec3f *c, const Vec3f *d, float *s, float *t);
void fn_800211F0(const Vec3f *a, const Vec3f *b, const Vec3f *c, const Vec3f *d, float *s, float *t);
CollisionResultList *fn_80021480(CollisionQuery *query, RpWorld *world, const UnkSphere_80021C04 *sphere,
                                 Vec3f *move, s32 arg4, CharaColliBase::Filter filter, u32 arg,
                                 CollisionResultList *list, bool arg8);
CollisionResultList *fn_800215C0(CollisionQuery *query, RpWorld *world, const UnkSphere_80021C04 *sphere,
                                 CharaColliBase::Filter filter, u32 arg, CollisionResultList *list,
                                 CollisionResultList *behind);
CollisionResult *fn_80021678(CollisionQuery *query, RpWorld *world, const UnkDebugLine_80020A20 *line,
                             CharaColliBase::Filter filter, u32 arg, float *outDistance, float length);
void *fn_800218E4(void);
UnkColliderManager_80021B08 *fn_80021B08(void);
void *fn_80022150(CollisionQuery *query, short flags);
}

/* The sphere of the queries: a centre and a radius. */
struct UnkSphere_80021C04 {
    /* 0x00 */ RwV3d m_center;
    /* 0x0C */ float m_radius;
};

extern "C" {
UnkSphere_80021C04 *fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
void *__dt__18UnkHandle_80045A98Fv(void *object, short flags);
UnkHandle_80021C80 fn_80021C80(int index);
}


/* The query object's destructor. */
extern "C" void *fn_80022150(CollisionQuery *query, short flags) {
    if (query != 0 && flags > 0) {
        __dl__FPv(query);
    }
    return query;
}

/* All hits of a line (flags: bit 0, the stage; bit 1, the colliders). */
extern "C" CollisionResultList *fn_80021FE4(CollisionQuery *query, const UnkDebugLine_80020A20 *line, u32 flags,
                                            u32 mask, CharaColliBase::Filter filter, u32 arg) {
    CollisionResultList *list = 0;
    if (flags & 1) {
        UnkBlockSet_80045A98 *blocks = static_cast<UnkBlockSet_80045A98 *>(fn_80045A98(fn_80021C80(0)));
        if (blocks != 0) {
            s16 *ids = blocks->m_shown;
            s16 id;
            while ((id = *ids++) != -1) {
                UnkBlock_8003A540 *block = fn_8003A540(blocks, id);
                if (block != 0 && block->m_world != 0) {
                    CollisionResult *result = fn_80021678(query, block->m_world, line, filter, arg, 0, 0.0f);
                    if (result != 0) {
                        if (list == 0) {
                            NEW_LIST(list)
                            fn_80026BB4(list, result);
                        } else {
                            fn_80026D8C(list, result);
                        }
                    }
                }
            }
        }
    }
    if (flags & 2) {
        list = fn_800C5D74(fn_80021B08(), line, mask, 1, list, filter, arg, lbl_805EE6A0);
    }
    return list;
}

/* The first hit of a line. */
extern "C" CollisionResult *fn_80021E24(CollisionQuery *query, const UnkDebugLine_80020A20 *segment, u32 flags,
                                        u32 mask, CharaColliBase::Filter filter, u32 arg) {
    UnkDebugLine_80020A20 line(*segment);
    CollisionResult *nearest = 0;
    if (flags & 1) {
        UnkBlockSet_80045A98 *blocks = static_cast<UnkBlockSet_80045A98 *>(fn_80045A98(fn_80021C80(0)));
        if (blocks != 0) {
            float t;
            s16 *ids = blocks->m_shown;
            s16 id;
            while ((id = *ids++) != -1) {
                UnkBlock_8003A540 *block = fn_8003A540(blocks, id);
                if (block != 0 && block->m_world != 0) {
                    CollisionResult *result = fn_80021678(query, block->m_world, &line, filter, arg, &t, 0.0f);
                    if (result != 0) {
                        if (nearest != 0) {
                            fn_800265A4(nearest, 1);
                        }
                        float s = t;
                        nearest = result;
                        fn_800091D0(&line.m_end,
                                    &(const Vec3f &)ret::fn_8040CDD0(
                                        &line.m_start,
                                        &(const Vec3f &)ret::fn_8040CDA8(
                                            &(const Vec3f &)ret::fn_8040CD74(&line.m_end, &line.m_start), s)));
                    }
                }
            }
        }
    }
    if (flags & 2) {
        CollisionResult *result;
        if ((result = fn_800C5938(fn_80021B08(), &line, mask, 1, filter, arg, lbl_805EE6A0)) != 0) {
            if (nearest != 0) {
                fn_800265A4(nearest, 1);
            }
            nearest = result;
        }
    }
    return nearest;
}

UnkDebugLine_80020A20::UnkDebugLine_80020A20(const UnkDebugLine_80020A20 &line) {
    fn_800091EC(&m_start, &line.m_start);
    fn_800091EC(&m_end, &line.m_end);
}

/* A sphere against the stage and the colliders. */
extern "C" CollisionResultList *fn_80021CA4(CollisionQuery *query, const UnkSphere_80021C04 *sphere, u32 flags,
                                            u32 mask, CharaColliBase::Filter filter, u32 arg,
                                            CollisionResultList *behind) {
    CollisionResultList *list = 0;
    if (flags & 1) {
        UnkBlockSet_80045A98 *blocks = static_cast<UnkBlockSet_80045A98 *>(fn_80045A98(fn_80021C80(0)));
        if (blocks != 0) {
            for (int i = 0; i < 0xA8; i++) {
                if (IsRequested__12TBlockManageFii(blocks, i, 0)) {
                    UnkBlock_8003A540 *block = fn_8003A540(blocks, i);
                    if (block != 0 && block->m_world != 0) {
                        list = fn_800215C0(query, block->m_world, sphere, filter, arg, list, behind);
                    }
                }
            }
        }
    }
    if (flags & 2) {
        list = fn_800C5478(fn_80021B08(), sphere, mask, 1, list, filter, arg, lbl_805EE6A0);
    }
    return list;
}

/* An entry of the stage's handles. */
extern "C" UnkHandle_80021C80 fn_80021C80(int index) {
    return lbl_80571484[index];
}

/* The handle's destructor (UnkHandle_80045A98, Game/UnkHandleTable.h; its first copy; also the
 * element destructor of Game/TBlockManage.cpp's handle array). */
extern "C" void *__dt__18UnkHandle_80045A98Fv(void *object, short flags) {
    if (object != 0 && flags > 0) {
        __dl__FPv(object);
    }
    return object;
}

/* The sphere's constructor. */
extern "C" UnkSphere_80021C04 *fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius) {
    fn_800091EC(reinterpret_cast<Vec3f *>(&sphere->m_center), center);
    sphere->m_radius = radius;
    return sphere;
}

/* The collider manager (a function-local static). */
extern "C" UnkColliderManager_80021B08 *fn_80021B08(void) {
    if (lbl_805EE068 == 0) {
        if (!lbl_805EE059) {
            __register_global_object(__ct__27UnkColliderManager_80021B08Fv(lbl_80570C90), (void *)__dt__27UnkColliderManager_80021B08Fv, lbl_80570C40);
            lbl_805EE059 = 1;
        }
        lbl_805EE068 = reinterpret_cast<UnkColliderManager_80021B08 *>(lbl_80570C90);
    }
    return lbl_805EE068;
}

/* A sphere moving through the stage and the colliders. */
extern "C" CollisionResultList *fn_80021934(CollisionQuery *query, const UnkSphere_80021C04 *sphere, Vec3f *move,
                                            u32 flags, u32 mask, s32 arg5, CharaColliBase::Filter filter, u32 arg,
                                            bool arg8) {
    CollisionResultList *list = 0;
    if (flags & 1) {
        UnkBlockSet_80045A98 *blocks = static_cast<UnkBlockSet_80045A98 *>(fn_80045A98(fn_80021C80(0)));
        if (blocks != 0) {
            for (int i = 0; i < 0xA8; i++) {
                if (IsRequested__12TBlockManageFii(blocks, i, 0)) {
                    UnkBlock_8003A540 *block = fn_8003A540(blocks, i);
                    if (block != 0 && block->m_world != 0) {
                        list = fn_80021480(query, block->m_world, sphere, move, arg5, filter, arg, list, !arg8);
                    }
                }
            }
        }
    }
    if (flags & 2) {
        UnkSphere_80021C04 s;
        fn_80021C04(&s, reinterpret_cast<const Vec3f *>(&sphere->m_center), sphere->m_radius);
        if (arg8) {
            list = fn_800C4DE0(fn_80021B08(), &s, move, mask, 1, list, filter, arg, lbl_805EE6A0, arg5);
        } else {
            list = fn_800C4DE0(fn_80021B08(), &s, move, mask, 1, list, filter, arg, lbl_805EE6A0, 0);
        }
    }
    return list;
}

/* The lock (a function-local static). */
extern "C" void *fn_800218E4(void) {
    if (lbl_805EE07C == 0) {
        if (!lbl_805EE05A) {
            fn_800379E4(&lbl_805EE05C);
            lbl_805EE05A = 1;
        }
        lbl_805EE07C = &lbl_805EE05C;
    }
    return lbl_805EE07C;
}

/* The first hit of a line in a world. */
extern "C" CollisionResult *fn_80021678(CollisionQuery *query, RpWorld *world, const UnkDebugLine_80020A20 *line,
                                        CharaColliBase::Filter filter, u32 arg, float *outDistance, float length) {
    RpIntersection intersection;
    RwLine l;
    l.start = ret::fn_8001DFE8(&line->m_start);
    l.end = ret::fn_8001DFE8(&line->m_end);
    intersection.type = 1;
    intersection.t.line = l;
    float nearest = 10000000.0f;
    lbl_805EE050 = filter;
    lbl_805EE054 = arg;
    fn_800379DC(fn_800218E4());
    fn_80442AA4(world, &intersection, fn_80020DC0, &nearest);
    fn_800379E0(fn_800218E4());
    if (10000000.0f == nearest) {
        return 0;
    }
    Vec3f point;
    point.x = l.end.x - l.start.x;
    point.y = l.end.y - l.start.y;
    point.z = l.end.z - l.start.z;
    point.x *= nearest;
    point.y *= nearest;
    point.z *= nearest;
    point.x += l.start.x;
    point.y += l.start.y;
    point.z += l.start.z;
    if (0.0f == length) {
        length = fn_8040D1B4(&line->m_start, &line->m_end);
    }
    RangeTriangle *triangle;
    NEW_TRIANGLE(triangle, &lbl_80570C0C, lbl_805EE04C)
    UnkColliContact_8001FF00 contact;
    fn_8001FF1C(&contact, &(const Vec3f &)ret::fn_800169F8(&point), length * nearest, length * nearest,
                triangle->GetNormal(), 0, -1, 1,
                &(const Vec3f &)Vec3f(0.0f, 0.0f, 0.0f));
    UnkColliHit *hit;
    NEW_HIT(hit, triangle, &contact)
    CollisionResult *result = new CollisionResult(triangle, hit, 0);
    if (outDistance != 0) {
        *outDistance = nearest;
    }
    return result;
}

/* A sphere against a world. */
extern "C" CollisionResultList *fn_800215C0(CollisionQuery *query, RpWorld *world, const UnkSphere_80021C04 *sphere,
                                            CharaColliBase::Filter filter, u32 arg, CollisionResultList *list,
                                            CollisionResultList *behind) {
    RwSphere s;
    s.center = ret::fn_8001DFE8(reinterpret_cast<const Vec3f *>(&sphere->m_center));
    s.radius = sphere->m_radius;
    RpIntersection intersection;
    intersection.type = 3;
    intersection.t.sphere = s;
    lbl_805EE044 = list;
    lbl_805EE048 = behind;
    lbl_805EE050 = filter;
    lbl_805EE054 = arg;
    fn_80442AA4(world, &intersection, fn_80020C24, 0);
    return lbl_805EE044;
}

/* A sphere moving through a world. */
extern "C" CollisionResultList *fn_80021480(CollisionQuery *query, RpWorld *world, const UnkSphere_80021C04 *sphere,
                                            Vec3f *move, s32 arg4, CharaColliBase::Filter filter, u32 arg,
                                            CollisionResultList *list, bool arg8) {
    if (fn_8040CF2C(move) < 0.000000009999999f) {
        fn_800091D0(move, &(const Vec3f &)Vec3f(0.0f, 0.0f, 0.0f));
        return fn_800215C0(query, world, sphere, filter, arg, list, 0);
    }
    RwSphere s;
    s.center = ret::fn_8001DFE8(reinterpret_cast<const Vec3f *>(&sphere->m_center));
    s.radius = sphere->m_radius;
    Vec3f m = (const Vec3f &)ret::fn_8001DFE8(move);
    list = fn_8001EE0C(query, world, reinterpret_cast<const Vec3f *>(&s.center),
                                              &m, 0, arg4, filter, arg, list, arg8,
                                              s.radius);
    reinterpret_cast<ColliVector &>(*move) = ret::fn_800169F8(&m);
    return list;
}

/* The parameters of the point where a line meets a plane (solving the plane's two equations along
 * the axis given by the plane's normal). */
extern "C" void fn_800211F0(const Vec3f *a, const Vec3f *b, const Vec3f *c, const Vec3f *d, float *s, float *t) {
    float eps = 0.001f * (std::fabs(a->x) + std::fabs(a->y) + std::fabs(a->z));
    if (std::fabs(d->y) > 0.001f || (0.0f != d->y && 0.0f == d->x && 0.0f == d->z)) {
        *t = a->z * c->x - a->x * c->z;
        *t = *t / (a->z * b->x - a->x * b->z);
        if (std::fabs(a->x) > eps || 0.0f == a->z) {
            *s = (c->x - b->x * *t) / a->x;
        } else {
            *s = (c->z - b->z * *t) / a->z;
        }
    } else if (std::fabs(d->z) > 0.001f || (0.0f != d->z && 0.0f == d->x)) {
        *t = a->y * c->x - a->x * c->y;
        *t = *t / (a->y * b->x - a->x * b->y);
        if (std::fabs(a->y) > eps || 0.0f == a->x) {
            *s = (c->y - b->y * *t) / a->y;
        } else {
            *s = (c->x - b->x * *t) / a->x;
        }
    } else {
        *t = a->z * c->y - a->y * c->z;
        *t = *t / (a->z * b->y - a->y * b->z);
        if (std::fabs(a->z) > eps || 0.0f == a->y) {
            *s = (c->z - b->z * *t) / a->z;
        } else {
            *s = (c->y - b->y * *t) / a->y;
        }
    }
}

/* The same with a scaled tolerance. */
extern "C" void fn_80020F48(const Vec3f *a, const Vec3f *b, const Vec3f *c, const Vec3f *d, float *s, float *t) {
    float epsD = 0.001f * (std::fabs(d->x) + std::fabs(d->y) + std::fabs(d->z));
    float eps = 0.001f * (std::fabs(a->x) + std::fabs(a->y) + std::fabs(a->z));
    if (std::fabs(d->y) > epsD || (0.0f != d->y && 0.0f == d->x && 0.0f == d->z)) {
        *t = a->z * c->x - a->x * c->z;
        *t = *t / d->y;
        if (std::fabs(a->x) > eps || 0.0f == a->z) {
            *s = (c->x - b->x * *t) / a->x;
        } else {
            *s = (c->z - b->z * *t) / a->z;
        }
    } else if (std::fabs(d->z) > epsD || (0.0f != d->z && 0.0f == d->x)) {
        *t = a->y * c->x - a->x * c->y;
        *t = *t / -d->z;
        if (std::fabs(a->y) > eps || 0.0f == a->x) {
            *s = (c->y - b->y * *t) / a->y;
        } else {
            *s = (c->x - b->x * *t) / a->x;
        }
    } else {
        *t = a->z * c->y - a->y * c->z;
        *t = *t / -d->x;
        if (std::fabs(a->z) > eps || 0.0f == a->y) {
            *s = (c->z - b->z * *t) / a->z;
        } else {
            *s = (c->y - b->y * *t) / a->y;
        }
    }
}

/* The material of a world triangle. */
inline void *TriangleMaterial(RpWorldSector *sector, RpCollisionTriangle *triangle) {
    return fn_80452C04(sector) ? fn_80452C4C(sector, 0)->materials[triangle->index] : 0;
}

/* The nearest triangle of a line (a world callback). */
extern "C" RpCollisionTriangle *fn_80020DC0(RpIntersection *intersection, RpWorldSector *sector,
                                            RpCollisionTriangle *triangle, float distance, void *data) {
    float *nearest = static_cast<float *>(data);
    void *material = TriangleMaterial(sector, triangle);
    UnkColliQuery_800C0D20 query = {(u32)triangle->vertices, 0, 0, (u32)material, lbl_805EE054};
    if (lbl_805EE050 == 0 || lbl_805EE050(&query)) {
        if (distance < *nearest) {
            *nearest = distance;
            lbl_80570C0C = *triangle;
            if (fn_80452C04(sector)) {
                lbl_805EE04C = fn_80452C4C(sector, 0)->materials[triangle->index];
            } else {
                lbl_805EE04C = 0;
            }
        }
    }
    return triangle;
}

/* The query object (a function-local static). */
extern "C" CollisionQuery *fn_80020D74(void) {
    if (!lbl_805EE062) {
        __register_global_object(&lbl_805EE064, (void *)fn_80022150, lbl_80570C64);
        lbl_805EE062 = 1;
    }
    return reinterpret_cast<CollisionQuery *>(&lbl_805EE064);
}

extern "C" CollisionQuery *fn_80020D40(void) {
    if (lbl_805EE070 == 0) {
        lbl_805EE070 = fn_80020D74();
    }
    return lbl_805EE070;
}

/* A triangle a sphere touches (a world callback). */
extern "C" RpCollisionTriangle *fn_80020C24(RpIntersection *intersection, RpWorldSector *sector,
                                            RpCollisionTriangle *triangle, float distance, void *data) {
    void *material = TriangleMaterial(sector, triangle);
    UnkColliQuery_800C0D20 query = {(u32)triangle->vertices, 0, 0, (u32)material, lbl_805EE054};
    if (lbl_805EE050 == 0 || lbl_805EE050(&query)) {
        if (lbl_805EE048 == 0 && distance < 0.0f) {
            return triangle;
        }
        fn_800201D4(fn_80020D40(), intersection, sector, triangle, distance);
    }
    return triangle;
}

/* The point of segment ab nearest to point, and which part of the segment it lies on. */
extern "C" float fn_80020A60(const Vec3f *point, RwV3d a, RwV3d b, Vec3f *closest, s32 indexA, s32 indexEdge,
                             s32 indexB, s32 *type, s32 *index) {
    Vec3f ab(b.x - a.x, b.y - a.y, b.z - a.z);
    Vec3f ap(point->x - a.x, point->y - a.y, point->z - a.z);
    float t = fn_8040CF90(&ap, &ab);
    if (t > 0.0f) {
        float length2 = fn_8040CF90(&ab, &ab);
        if (t >= length2) {
            if (closest != 0) {
                reinterpret_cast<ColliVector &>(*closest) = ret::fn_800169F8(reinterpret_cast<Vec3f *>(&b));
            }
            *type = 2;
            *index = indexB;
            return fn_8040D180(point, &(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(&b)));
        }
        ColliVector c(ret::fn_8040CDD0(&(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(&a)),
                                       &(const Vec3f &)ret::fn_8040CDA8(&ab, t / length2)));
        if (closest != 0) {
            fn_800091D0(closest, c.Ptr());
        }
        *type = 1;
        *index = indexEdge;
        return fn_8040D180(point, c.Ptr());
    }
    if (closest != 0) {
        reinterpret_cast<ColliVector &>(*closest) = ret::fn_800169F8(reinterpret_cast<Vec3f *>(&a));
    }
    *type = 2;
    *index = indexA;
    return fn_8040CF2C(&ap);
}

/* A segment from start to end (other units call it by this name). */
extern "C" UnkDebugLine_80020A20 *fn_80020A20(UnkDebugLine_80020A20 *line, const Vec3f *start, const Vec3f *end) {
    fn_800091EC(&line->m_start, start);
    fn_800091EC(&line->m_end, end);
    return line;
}

/* Room for a segment that fn_80020A20 constructs. */
struct UnkLineStorage {
    UnkDebugLine_80020A20 *Line() {
        return reinterpret_cast<UnkDebugLine_80020A20 *>(this);
    }

    float f[6];
};

/* The nearer of the corners or edges two segments give. */
#define NEAREST_OF_TWO(A, B, I0, I1, I2, B2, J0, J1, J2)                                                       \
    {                                                                                                          \
        Vec3f other;                                                                                           \
        s32 otherType;                                                                                         \
        s32 otherIndex;                                                                                        \
        nearest = fn_80020A60((Vec3f *)&center, *triangle->vertices[A], *triangle->vertices[B], &other, I0, I1, I2,    \
                               &otherType, &otherIndex);                                                       \
        float d2 = fn_80020A60((Vec3f *)&center, *triangle->vertices[A], *triangle->vertices[B2],              \
                               (Vec3f *)&point, J0, J1, J2, &type, &index);                                    \
        if (nearest < d2) {                                                                                    \
            point = other;                                                                                     \
            nearest = fn_8000FEE4(nearest);                                                                  \
            type = otherType;                                                                                  \
            index = otherIndex;                                                                                \
        } else {                                                                                               \
            nearest = fn_8000FEE4(d2);                                                                        \
        }                                                                                                      \
    }

/* A sphere against a world triangle: the nearest point of the triangle to the sphere's centre. */
extern "C" void fn_800201D4(CollisionQuery *query, RpIntersection *intersection, RpWorldSector *sector,
                            RpCollisionTriangle *triangle, float distance) {
    RangeTriangle *shape;
    void *material = TriangleMaterial(sector, triangle);
    NEW_TRIANGLE(shape, triangle, material)
    s32 index = 0;
    Vec3f e1;
    Vec3f e2;
    Vec3f q;
    float depth = intersection->t.sphere.radius * distance;
    float px = triangle->normal.x * -depth;
    float py = triangle->normal.y * -depth;
    float pz = triangle->normal.z * -depth;
    px += intersection->t.sphere.center.x;
    py += intersection->t.sphere.center.y;
    pz += intersection->t.sphere.center.z;
    e1.x = triangle->vertices[1]->x - triangle->vertices[0]->x;
    e1.y = triangle->vertices[1]->y - triangle->vertices[0]->y;
    e1.z = triangle->vertices[1]->z - triangle->vertices[0]->z;
    e2.x = triangle->vertices[2]->x - triangle->vertices[0]->x;
    e2.y = triangle->vertices[2]->y - triangle->vertices[0]->y;
    e2.z = triangle->vertices[2]->z - triangle->vertices[0]->z;
    q.x = px - triangle->vertices[0]->x;
    q.y = py - triangle->vertices[0]->y;
    q.z = pz - triangle->vertices[0]->z;
    float u;
    float v;
    fn_800211F0(&e1, &e2, &q, reinterpret_cast<Vec3f *>(&triangle->normal), &u, &v);
    ColliVector center(ret::fn_800169F8(reinterpret_cast<Vec3f *>(&intersection->t.sphere.center)));
    ColliVector point;
    s32 type;
    float nearest;
    if (u < -0.0001f) {
        if (v < -0.0001f) {
            NEAREST_OF_TWO(0, 1, 0, 0, 1, 2, 0, 1, 2)
        } else if (u + v > 1.0001f) {
            NEAREST_OF_TWO(2, 0, 2, 1, 0, 1, 2, 2, 1)
        } else {
            UnkLineStorage line;
            fn_80020A20(line.Line(), &(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(triangle->vertices[0])),
                        &(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(triangle->vertices[2])));
            nearest = fn_8003C63C((Vec3f *)&center, line.Line(), (Vec3f *)&point);
            type = 1;
            index = 1;
        }
    } else if (v < -0.0001f) {
        if (u + v > 1.0001f) {
            NEAREST_OF_TWO(1, 0, 1, 0, 0, 2, 1, 2, 2)
        } else {
            UnkLineStorage line;
            fn_80020A20(line.Line(), &(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(triangle->vertices[0])),
                        &(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(triangle->vertices[1])));
            nearest = fn_8003C63C((Vec3f *)&center, line.Line(), (Vec3f *)&point);
            type = 1;
            index = 0;
        }
    } else if (u + v > 1.0001f) {
        UnkLineStorage line;
        fn_80020A20(line.Line(), &(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(triangle->vertices[1])),
                    &(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(triangle->vertices[2])));
        nearest = fn_8003C63C((Vec3f *)&center, line.Line(), (Vec3f *)&point);
        type = 1;
        index = 2;
    } else {
        nearest = std::fabs(depth);
        point = ret::fn_8040CD74((Vec3f *)&center, &(const Vec3f &)ret::fn_8040CDA8(
                                                   &(const Vec3f &)ret::fn_800169F8(
                                                       reinterpret_cast<Vec3f *>(&triangle->normal)),
                                                   depth));
        type = 0.0f == depth ? 4 : 0;
        index = 0;
    }
    CollisionResult *result;
    if (type == 1 || type == 2) {
        UnkColliContact_8001FF00 contact;
        fn_8001FF1C(&contact, (Vec3f *)&point, nearest, nearest - intersection->t.sphere.radius, &lbl_80570C00, type,
                    index, 1, shape->GetNormal());
        UnkColliHit *hit;
        NEW_HIT(hit, shape, &contact)
        result = new CollisionResult(shape, hit, 0);
    } else {
        Vec3f zero(0.0f, 0.0f, 0.0f);
        UnkColliContact_8001FF00 contact;
        fn_8001FF1C(&contact, (Vec3f *)&point, nearest, nearest - intersection->t.sphere.radius, shape->GetNormal(),
                    type, index, 1, &zero);
        UnkColliHit *hit;
        NEW_HIT(hit, shape, &contact)
        result = new CollisionResult(shape, hit, 0);
    }
    if (distance < 0.0f && lbl_805EE048 != 0) {
        if (type == 0) {
            UnkColliHit *hit = result->m_hit;
            hit->m_contact.m_distance = distance * intersection->t.sphere.radius;
            hit->m_contact.field_0x10 = (distance - 1.0f) * intersection->t.sphere.radius;
            fn_80026D8C(lbl_805EE048, result);
        } else if (result != 0) {
            fn_800265A4(result, 1);
        }
    } else if (lbl_805EE044 == 0) {
        NEW_LIST(lbl_805EE044)
        fn_80026BB4(lbl_805EE044, result);
    } else {
        fn_80026D8C(lbl_805EE044, result);
    }
}

/* The triangle pool's destructor. */
extern "C" void *fn_80020170(UnkPool<RangeTriangle> *pool, short flags) {
    if (pool != 0) {
        if (pool->m_array != 0) {
            __destroy_new_array(pool->m_array, (void *)__dt__Q412SonicteamUSA6System5Range13RangeTriangleFv);
            pool->m_array = 0;
        }
        if (flags > 0) {
            __dl__FPv(pool);
        }
    }
    return pool;
}

/* The triangle pool (a function-local static). */
extern "C" UnkPool<RangeTriangle> *fn_80020088(void) {
    if (!lbl_805EE060) {
        lbl_80570CAC.m_array = static_cast<RangeTriangle *>(
            __construct_new_array(__nwa__FUl(0xD90), (void *)__ct__Q412SonicteamUSA6System5Range13RangeTriangleFv, (void *)__dt__Q412SonicteamUSA6System5Range13RangeTriangleFv, 0x6C, 0x20));
        if (lbl_80570CAC.m_array == 0) {
            for (;;) {
            }
        }
        lbl_80570CAC.m_used = 0;
        lbl_80570CAC.m_capacity = 0x20;
        lbl_80570CAC.m_free = lbl_80570CAC.m_array;
        RangeTriangle *node = lbl_80570CAC.m_array;
        for (int i = 0; i < 0x20; i++, node++) {
            if (i == 0) {
                node->m_prev = 0;
            } else {
                node->m_prev = node - 1;
            }
            if (i == 0x1F) {
                node->m_next = 0;
            } else {
                node->m_next = node + 1;
            }
        }
        __register_global_object(&lbl_80570CAC, (void *)fn_80020170, lbl_80570C4C);
        lbl_805EE060 = 1;
    }
    return &lbl_80570CAC;
}

extern "C" UnkPool<RangeTriangle> *fn_80020054(void) {
    if (lbl_805EE080 == 0) {
        lbl_805EE080 = fn_80020088();
    }
    return lbl_805EE080;
}

/* Allocation from a pool, then from the heap. */
template <class T> inline void *PoolNew(UnkPool<T> *pool, u32 size) {
    T *node = pool->m_free;
    if (pool->m_used == pool->m_capacity - 1) {
        node = 0;
    } else {
        node->m_next->m_prev = 0;
        pool->m_used++;
        pool->m_free = node->m_next;
        node->m_next = 0;
    }
    if (node != 0) {
        node->m_unpooled = false;
    } else {
        node = static_cast<T *>(fn_80049B90(size, 2));
        if (node != 0) {
            node->m_unpooled = true;
        }
    }
    node->m_next = 0;
    node->m_prev = 0;
    return node;
}

/* A triangle's allocation. */
extern "C" void *fn_8001FFA4(u32 size) {
    return PoolNew(fn_80020054(), size);
}

/* A contact's constructor. */
extern "C" UnkColliContact_8001FF00 *fn_8001FF1C(UnkColliContact_8001FF00 *contact, const Vec3f *position,
                                                 float distance, float depth, const Vec3f *normal, s32 type, s32 index,
                                                 s32 arg7, const Vec3f *other) {
    fn_800091EC(reinterpret_cast<Vec3f *>(&contact->m_position), position);
    contact->m_distance = distance;
    contact->field_0x10 = depth;
    fn_800091EC(reinterpret_cast<Vec3f *>(&contact->m_normal), normal);
    fn_800091EC(reinterpret_cast<Vec3f *>(&contact->field_0x20), other);
    contact->field_0x2C = type;
    contact->field_0x30 = index;
    contact->field_0x34 = arg7;
    return contact;
}

/* A contact's default constructor (other units call it by this name). */
extern "C" void fn_8001FF00(UnkColliContact_8001FF00 *contact) {
    contact->field_0x2C = 0;
    contact->field_0x30 = -1;
    contact->field_0x34 = 1;
}

/* The pooled contact's constructor. */
extern "C" UnkColliHit *fn_8001FEB8(UnkColliHit *hit) {
    hit->m_prev = 0;
    hit->m_next = 0;
    fn_8001FF00(&hit->m_contact);
    hit->field_0x44 = 0.0f;
    return hit;
}

/* The contact pool's destructor. */
extern "C" void *fn_8001FE54(UnkPool<UnkColliHit> *pool, short flags) {
    if (pool != 0) {
        if (pool->m_array != 0) {
            __destroy_new_array(pool->m_array, (void *)fn_800266DC);
            pool->m_array = 0;
        }
        if (flags > 0) {
            __dl__FPv(pool);
        }
    }
    return pool;
}

/* The contact pool (a function-local static). */
extern "C" UnkPool<UnkColliHit> *fn_8001FD6C(void) {
    if (!lbl_805EE061) {
        lbl_80570CBC.m_array = static_cast<UnkColliHit *>(
            __construct_new_array(__nwa__FUl(0x910), (void *)fn_8001FEB8, (void *)fn_800266DC, 0x48, 0x20));
        if (lbl_80570CBC.m_array == 0) {
            for (;;) {
            }
        }
        lbl_80570CBC.m_used = 0;
        lbl_80570CBC.m_capacity = 0x20;
        lbl_80570CBC.m_free = lbl_80570CBC.m_array;
        UnkColliHit *node = lbl_80570CBC.m_array;
        for (int i = 0; i < 0x20; i++, node++) {
            if (i == 0) {
                node->m_prev = 0;
            } else {
                node->m_prev = node - 1;
            }
            if (i == 0x1F) {
                node->m_next = 0;
            } else {
                node->m_next = node + 1;
            }
        }
        __register_global_object(&lbl_80570CBC, (void *)fn_8001FE54, lbl_80570C58);
        lbl_805EE061 = 1;
    }
    return &lbl_80570CBC;
}

extern "C" UnkPool<UnkColliHit> *fn_8001FD38(void) {
    if (lbl_805EE034 == 0) {
        lbl_805EE034 = fn_8001FD6C();
    }
    return lbl_805EE034;
}

/* A contact's allocation (other units call it by this name). */
extern "C" void *fn_8001FC88(u32 size) {
    return PoolNew(fn_8001FD38(), size);
}

CollisionResult::CollisionResult(RangeBase *shape, UnkColliHit *hit, s32 arg)
    : field_0x8(arg), m_shape(shape), m_next(0), m_hit(hit) {}

/* The list pool (a function-local static). */
extern "C" void *fn_8001FB68(void) {
    if (lbl_805EE074 == 0) {
        if (!lbl_805EE058) {
            __register_global_object(fn_80026964(lbl_80570C80, 0x20), (void *)fn_80026900, lbl_80570C34);
            lbl_805EE058 = 1;
        }
        lbl_805EE074 = lbl_80570C80;
    }
    return lbl_805EE074;
}

/* A list's allocation. */
extern "C" void *fn_8001FADC(u32 size) {
    CollisionResultList *list = 0;
    fn_800268B0(fn_8001FB68(), &list);
    if (list != 0) {
        list->m_allocated = false;
    } else {
        list = static_cast<CollisionResultList *>(fn_80049B90(size, 2));
        if (list != 0) {
            list->m_allocated = true;
        }
    }
    list->m_next = 0;
    list->m_prev = 0;
    return list;
}

/* The bounding box of a sphere moving by move. */
inline void SweepBox(RwBBox *box, const Vec3f *center, const Vec3f *move, float radius) {
    if (move->x < 0.0f) {
        box->sup.x = center->x + radius;
        box->inf.x = move->x + (center->x - radius);
    } else {
        box->inf.x = center->x - radius;
        box->sup.x = move->x + (center->x + radius);
    }
    if (move->y < 0.0f) {
        box->sup.y = center->y + radius;
        box->inf.y = move->y + (center->y - radius);
    } else {
        box->inf.y = center->y - radius;
        box->sup.y = move->y + (center->y + radius);
    }
    if (move->z < 0.0f) {
        box->sup.z = center->z + radius;
        box->inf.z = move->z + (center->z - radius);
    } else {
        box->inf.z = center->z - radius;
        box->sup.z = move->z + (center->z + radius);
    }
}

/* A vector copied as words. */
struct UnkWords3 {
    u32 x;
    u32 y;
    u32 z;
};

/* The walk of a world's sectors: the box and a stack of sectors. */
struct UnkSectorWalk {
    /* 0x000 */ RwBBox box;
    /* 0x018 */ RpSector *stack[64];
    /* 0x118 */ u32 field_0x118;
    /* 0x11C */ RpSector **sp;
};

/* What the walk of a collision tree keeps for a node: the node and the sides of the box it lies
 * within. */
struct UnkCollTreeEntry {
    /* 0x0 */ RpCollTreeNode *node;
    /* 0x4 */ u32 flags;
};

/* The walk of a collision tree. */
struct UnkCollTreeWalk {
    /* 0x000 */ RpCollTree *tree;
    /* 0x004 */ RwBBox box;
    /* 0x01C */ RwBBox outer;
    /* 0x034 */ RpCollTreeNode root;
    /* 0x038 */ UnkCollTreeEntry stack[33];
    /* 0x140 */ UnkCollTreeEntry *top;
};

/* A sphere moving through a world: its walk of the world's sectors and of their collision trees,
 * testing each triangle (fn_800221A8 or, once the sphere has stopped, fn_80024AC8). */
extern "C" CollisionResultList *fn_8001EE0C(CollisionQuery *query, RpWorld *world, const Vec3f *center, Vec3f *move,
                                            s32 arg4, s32 arg5, CharaColliBase::Filter filter, u32 arg,
                                            CollisionResultList *list, bool arg9, float radius) {
    RwV3d m = *reinterpret_cast<RwV3d *>(move);
    bool stopped = false;
    RwBBox box;
    SweepBox(&box, center, move, radius);
    UnkSectorWalk walkSectors;
    reinterpret_cast<UnkWords3 &>(walkSectors.box.sup) = reinterpret_cast<const UnkWords3 &>(box.sup);
    reinterpret_cast<UnkWords3 &>(walkSectors.box.inf) = reinterpret_cast<const UnkWords3 &>(box.inf);
    float tolerance = 0.001f + radius;
    walkSectors.stack[0] = world->rootSector;
    walkSectors.sp = &walkSectors.stack[1];
    while (walkSectors.sp > walkSectors.stack) {
        walkSectors.sp--;
        while ((*walkSectors.sp)->type >= 0) {
            RpPlaneSector *plane = static_cast<RpPlaneSector *>(*walkSectors.sp);
            if (plane->rightValue > *(float *)((u8 *)&walkSectors.box.sup + plane->type)) {
                *walkSectors.sp = plane->leftSubTree;
            } else {
                *walkSectors.sp = plane->rightSubTree;
                if (plane->leftValue >= *(float *)((u8 *)&walkSectors.box.inf + plane->type)) {
                    walkSectors.sp++;
                    *walkSectors.sp = plane->leftSubTree;
                }
            }
        }
        RpWorldSector *sector = static_cast<RpWorldSector *>(*walkSectors.sp);
        RpCollTree *tree = *reinterpret_cast<RpCollTree **>(reinterpret_cast<u8 *>(sector) + lbl_805F1420);
        if (tree == 0) {
            continue;
        }
        RpTriangle *triangles = sector->triangles;
        RwV3d *vertices = sector->vertices;
        bool hasMaterials = fn_80452C04(sector) != 0;
        bool moved = false;
        UnkCollTreeWalk walk;
        if (tree->bbox.inf.x <= box.sup.x && tree->bbox.inf.y <= box.sup.y && tree->bbox.inf.z <= box.sup.z &&
            tree->bbox.sup.x >= box.inf.x && tree->bbox.sup.y >= box.inf.y && tree->bbox.sup.z >= box.inf.z) {
            walk.tree = tree;
            walk.root.index = 0;
            walk.root.count = 0xFF;
            if (tree->numSplits == 0) {
                walk.root.count = tree->numTriangles;
            }
            walk.box = box;
            walk.outer = box;
            walk.stack[0].node = &walk.root;
            walk.stack[0].flags = (tree->bbox.inf.y < box.inf.y ? 4 : 0) | (tree->bbox.inf.x < box.inf.x ? 1 : 0) |
                                  (tree->bbox.inf.z < box.inf.z ? 0x10 : 0) |
                                  (tree->bbox.sup.x > box.sup.x ? 2 : 0) |
                                  (tree->bbox.sup.y > box.sup.y ? 8 : 0) |
                                  (tree->bbox.sup.z > box.sup.z ? 0x20 : 0);
            walk.top = &walk.stack[1];
        } else {
            walk.top = &walk.stack[0];
        }
        while (walk.top > walk.stack) {
            walk.top--;
            bool found = false;
            u16 start;
            u16 count;
            for (;;) {
                UnkCollTreeEntry *entry = walk.top;
                RpCollTreeNode *node = entry->node;
                if (node->count != 0xFF) {
                    start = node->index;
                    count = node->count;
                    break;
                }
                if (entry->flags == 0) {
                    RpCollTreeSplit *split = &walk.tree->splits[node->index];
                    RpCollTreeSplit *first = split;
                    while (first->left.count == 0xFF) {
                        first = &walk.tree->splits[first->left.index];
                    }
                    RpCollTreeNode *last = &split->right;
                    while (last->count == 0xFF) {
                        last = &walk.tree->splits[last->index].right;
                    }
                    start = first->left.index;
                    count = last->index + last->count - first->left.index;
                    break;
                }
                RpCollTreeSplit *split = &walk.tree->splits[node->index];
                u32 axis = split->left.type & 0xC;
                if (split->rightValue > *(float *)((u8 *)&walk.box.sup + axis)) {
                    entry->node = &split->left;
                    continue;
                }
                entry->node = &split->right;
                if (split->leftValue >= *(float *)((u8 *)&walk.box.inf + axis)) {
                    walk.top++;
                    walk.top->node = &split->left;
                    walk.top->flags = walk.top[-1].flags;
                    if (split->leftValue <= *(float *)((u8 *)&walk.outer.sup + axis)) {
                        walk.top->flags &= ~(2 << (axis >> 1));
                    }
                    if (split->rightValue >= *(float *)((u8 *)&walk.outer.inf + axis)) {
                        walk.top[-1].flags &= ~(1 << (axis >> 1));
                    }
                }
            }
            int end = start + count;
            for (u16 i = start; i < end; i++) {
                u16 triangleIndex = tree->map != 0 ? tree->map[i] : i;
                RpTriangle *t = &triangles[triangleIndex];
                RwV3d *v[3];
                v[0] = &vertices[t->vertIndex[0]];
                v[1] = &vertices[t->vertIndex[1]];
                v[2] = &vertices[t->vertIndex[2]];
                Vec3f point;
                float distance;
                s32 type;
                s32 index;
                index = -1;
                void *material = hasMaterials ? fn_80452C4C(sector, 0)->materials[triangleIndex] : 0;
                UnkColliQuery_800C0D20 q = {(u32)v, 0, 0, (u32)material, arg};
                if (filter != 0 && !filter(&q)) {
                    continue;
                }
                bool hit;
                if (stopped) {
                    hit = fn_80024AC8(center, v, &point, &distance, &type, &index, arg4, arg5, (Vec3f *)&m, arg9, 0,
                                      radius);
                } else {
                    hit = fn_800221A8(center, move, v, &point, &distance, &type, &index, arg4, arg5, arg9, radius);
                }
                if (!hit) {
                    continue;
                }
                if (!stopped) {
                    found = true;
                    moved = true;
                }
                if (!stopped && move->x * move->x + move->y * move->y + move->z * move->z < 0.0000010000001f) {
                    if (list != 0) {
                        dynamic_cast<RangeTriangle *>(fn_8001EE04(list)->m_shape);
                    }
                    stopped = true;
                    if (list != 0) {
                        fn_80026FB4(list, 1);
                        list = 0;
                    }
                }
                RpCollisionTriangle ct;
                ct.point = *v[0];
                ct.index = triangleIndex;
                ct.vertices[0] = v[0];
                ct.vertices[1] = v[1];
                ct.vertices[2] = v[2];
                float ax = v[1]->x - v[0]->x;
                float bx = v[2]->x - v[0]->x;
                float ay = v[1]->y - v[0]->y;
                float by = v[2]->y - v[0]->y;
                float az = v[1]->z - v[0]->z;
                float bz = v[2]->z - v[0]->z;
                ct.normal.z = ax * by - ay * bx;
                ct.normal.x = ay * bz - az * by;
                ct.normal.y = az * bx - ax * bz;
                float scale = fn_80482394(ct.normal.x * ct.normal.x + ct.normal.y * ct.normal.y +
                                          ct.normal.z * ct.normal.z);
                ct.normal.x *= scale;
                ct.normal.y *= scale;
                ct.normal.z *= scale;
                s32 inside;
                if (radius - 0.001f <= distance && distance <= tolerance) {
                    inside = 0;
                } else {
                    inside = 1;
                }
                RangeTriangle *shape;
                NEW_TRIANGLE(shape, &ct, material)
                UnkColliHit *contactHit;
                if (type == 1 || type == 2) {
                    ColliVector at(ret::fn_800169F8(&point));
                    Vec3f to(center->x + move->x, center->y + move->y, center->z + move->z);
                    float dist = distance;
                    UnkColliContact_8001FF00 contact;
                    fn_8001FF1C(&contact, (Vec3f *)&at, dist, dist - radius,
                                &(const Vec3f &)ret::fn_8040CEE8(
                                    &(const Vec3f &)ret::fn_8040CD74(&to, (Vec3f *)&at)),
                                type, index, inside,
                                &(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(&ct.normal)));
                    NEW_MOVING_HIT(contactHit, shape, &contact,
                                   move->x * move->x + move->y * move->y + move->z * move->z)
                } else {
                    float dist = distance;
                    UnkColliContact_8001FF00 contact;
                    fn_8001FF1C(&contact, &(const Vec3f &)ret::fn_800169F8(&point), dist, dist - radius,
                                &(const Vec3f &)ret::fn_800169F8(reinterpret_cast<Vec3f *>(&ct.normal)), type, index,
                                inside, &(const Vec3f &)Vec3f(0.0f, 0.0f, 0.0f));
                    NEW_MOVING_HIT(contactHit, shape, &contact,
                                   move->x * move->x + move->y * move->y + move->z * move->z)
                }
                CollisionResult *result = new CollisionResult(shape, contactHit, 0);
                if (list == 0) {
                    NEW_LIST(list)
                    fn_80026BB4(list, result);
                } else if (stopped) {
                    fn_80026EA0(list, result);
                } else {
                    fn_80026BF4(list, result);
                }
            }
            if (found) {
                SweepBox(&box, center, move, radius);
            }
        }
        if (moved) {
            SweepBox(&walkSectors.box, center, move, radius);
        }
    }
    return list;
}

/* The list's first and end results, an iterator at a result, and a contact's edge or corner. */
extern "C" CollisionResult *fn_8001EE04(CollisionResultList *list) {
    return list->m_first;
}

extern "C" UnkResultIter *fn_8001EDFC(UnkResultIter *it, CollisionResult *node) {
    it->m_node = node;
    return it;
}

extern "C" s32 fn_8001EDF4(UnkColliHit *hit) {
    return hit->m_contact.field_0x30;
}

extern "C" CollisionResult *fn_8001EDEC(CollisionResultList *list) {
    return list->m_end;
}

/* Removes the contacts with a triangle's edge or corner that lie on another triangle of the list
 * (where the sphere touches the face of that triangle). */
extern "C" void fn_8001E9F8(CollisionResultList *list) {
    if (list != 0) {
        UnkResultIter other;
        UnkResultIter it;
        UnkResultIter current;
        UnkResultIterStorage first;
        fn_8001EDFC(reinterpret_cast<UnkResultIter *>(&first), fn_8001EE04(list));
        it = reinterpret_cast<UnkResultIter &>(first);
        while (it.m_node != fn_8001EDEC(list)) {
            current = it;
            fn_80027374(&it, 0);
            UnkColliHit *hit;
            RangeBase *shape;
            shape = current.m_node->m_shape;
            hit = current.m_node->m_hit;
            if (shape == 0 || shape->m_priority != -1) {
                continue;
            }
            s32 type = fn_8001E9F0(hit);
            if (type != 1 && type != 2) {
                continue;
            }
            for (other = it; other.m_node != fn_8001EDEC(list); fn_80027374(&other, 0)) {
                RangeBase *face = other.m_node->m_shape;
                UnkColliHit *faceHit = other.m_node->m_hit;
                if (face == 0 || face->m_priority != -1 || fn_8001E9F0(faceHit) != 0) {
                    continue;
                }
                s32 index = fn_8001EDF4(hit);
                if (type == 1) {
                    ColliVector a;
                    ColliVector b;
                    switch (index) {
                    case 0:
                        a = static_cast<RangeTriangle *>(shape)->GetVertex(0);
                        b = static_cast<RangeTriangle *>(shape)->GetVertex(1);
                        break;
                    case 1:
                        a = static_cast<RangeTriangle *>(shape)->GetVertex(0);
                        b = static_cast<RangeTriangle *>(shape)->GetVertex(2);
                        break;
                    case 2:
                        a = static_cast<RangeTriangle *>(shape)->GetVertex(1);
                        b = static_cast<RangeTriangle *>(shape)->GetVertex(2);
                        break;
                    }
                    for (int i = 0; i < 3; i++) {
                        ColliVector p;
                        ColliVector q;
                        p = static_cast<RangeTriangle *>(face)->GetVertex(i);
                        q = static_cast<RangeTriangle *>(face)->GetVertex((i + 1) % 3);
                        if ((a == p && b == q) || (a == q && b == p)) {
                            fn_80026AAC(list, &current);
                            goto next;
                        }
                    }
                } else {
                    ColliVector corner(static_cast<RangeTriangle *>(shape)->GetVertex(index));
                    for (int i = 0; i < 3; i++) {
                        ColliVector p;
                        p = static_cast<RangeTriangle *>(face)->GetVertex(i);
                        if (corner == p) {
                            fn_80026AAC(list, &current);
                            goto next;
                        }
                    }
                }
            }
        next:;
        }
    }
}

/* A pooled contact's type, its contact's depth, normal and position. */
extern "C" s32 fn_8001E9F0(UnkColliHit *hit) {
    return hit->m_contact.field_0x2C;
}

extern "C" float fn_8001E9E8(UnkColliContact_8001FF00 *contact) {
    return contact->field_0x10;
}

extern "C" const Vec3f *fn_8001E9C4(UnkColliHit *hit) {
    s32 type = hit->m_contact.field_0x2C;
    if (type == 1 || type == 2) {
        return reinterpret_cast<const Vec3f *>(&hit->m_contact.field_0x20);
    }
    return reinterpret_cast<const Vec3f *>(&hit->m_contact.m_normal);
}

extern "C" const Vec3f *fn_8001E9BC(UnkColliHit *hit) {
    return reinterpret_cast<const Vec3f *>(&hit->m_contact.m_position);
}

/* The push out of a contact for a sphere of the given radius at position. */
extern "C" Vec3f fn_8001E858(CollisionQuery *query, UnkColliHit *hit, const Vec3f *position, float radius) {
    UnkColliContact_8001FF00 *contact = &hit->m_contact;
    switch (fn_8001E9F0(hit)) {
    case 0:
    case 3:
    case 4:
        return ret::fn_8040CDA8(fn_8001E9C4(hit), -fn_8001E9E8(contact));
    case 1:
    case 2: {
        float t = fn_8040CF90(&(const Vec3f &)ret::fn_80016284(fn_8001E9C4(hit)),
                              &(const Vec3f &)ret::fn_8040CD74(fn_8001E9BC(hit), position));
        ColliVector foot(ret::fn_8040CDD0(fn_8001E9BC(hit), &(const Vec3f &)ret::fn_8040CDA8(fn_8001E9C4(hit), t)));
        float s = fn_8000FEE4(radius * radius - fn_8040D180(foot.Ptr(), position));
        return ret::fn_8040CDA8(fn_8001E9C4(hit), s - t);
    }
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
}
