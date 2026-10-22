/* BossCharaColli (class name from RTTI): the task that keeps a boss's collision. BossCtrl creates it
 * with the boss's other tasks (0x8017A108). Its state is a separately allocated object (class name
 * unknown) that builds the colliders of the boss's collision table into a PooledList (the list
 * BossVarParam::m_colliders points to), switches their attack colliders on and off with the boss,
 * and pushes the boss out of the world's collision around its focus point. Member and function
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define BOSSVARPARAM_CHARA_SHARED_PTR
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/MathTypes.h"
#include "Game/PooledList.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The game state (struct name unknown; see Game/BossMotion.cpp): the frame time at 0x20. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x20];
    /* 0x020 */ float m_frameTime;
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0x150 - 0x28];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A sphere for the world collision query (layout as in Game/MeteorsHolder.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

/* The hits of a world collision query and the nodes of their list (names unknown; layout as far
 * as used: each node holds the object hit at 0x14). */
struct UnkHits_80021CA4;
struct UnkHitNode_8001EE04 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};
struct UnkHitIter_8001EDFC {
    UnkHitIter_8001EDFC() : m_node(0) {}

    UnkHitNode_8001EE04 *m_node;
};

extern "C" {
/* The resource loader (see Game/BossRender.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int arg2);

/* Colliders (see Game/DevilDoomCtrlUnit.cpp and Game/EggLastMech.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              const void *part, void *work, int arg4, int arg5);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(void *manager, boost::shared_ptr<CharaColliBase> collider);

/* The world collision (see Game/VehicleColliMove.cpp; behaviour inferred). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
UnkHits_80021CA4 *fn_80021CA4(void *world, const UnkSphere_80021C04 *sphere, int arg2, int arg3,
                              void *filter, void *self, void *list);
void fn_80026FB4(UnkHits_80021CA4 *hits, int arg1);
UnkHitNode_8001EE04 *fn_8001EE04(UnkHits_80021CA4 *hits);
UnkHitNode_8001EE04 *fn_8001EDEC(UnkHits_80021CA4 *hits);
UnkHitIter_8001EDFC fn_8001EDFC(UnkHitNode_8001EE04 *node);
UnkHitIter_8001EDFC fn_80027374(UnkHitIter_8001EDFC *it, int);
/* The push-out direction of a hit object and its depth (behaviour inferred). */
const Vec3f *fn_8001E9C4(void *object);
float fn_8001E9E8(void *object);

/* Vectors and matrices. */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *offset); /* vector += offset */
}

/* The out-of-line operator-> of BossVarParam::m_chara (0x801792D0). The other boss units, which
 * see m_chara as a plain pointer, call it by this name (Game/BlackBullCtrlUnit.cpp,
 * Game/DevilDoomModeCtrlUnit.cpp, Game/HeavyDog0410CtrlUnit.cpp), so it is written as a C
 * function here instead of boost::shared_ptr<BossCharInfo>::operator->, whose code it repeats. */
extern "C" inline BossCharInfo *fn_801792D0(const boost::shared_ptr<BossCharInfo> *chara) {
    return chara->get();
}

inline const Vec3f &Ref(const Vec3f &v) {
    return v;
}

/* Moves position by offset (an inline function: retail calls the shared_ptr's operator-> out of
 * line before it). */
inline void MoveBy(Vec3f &position, const Vec3f &offset) {
    fn_8040CFF8(&position, &offset);
}

class BossCharaColli : public Task {
  public:
    class Impl;

    BossCharaColli(Task *parent, BossVarParam *param);
    virtual ~BossCharaColli();
    virtual void Tick(float dt);

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the allocation is 0x30 bytes */
};

class BossCharaColli::Impl {
  public:
    Impl(BossVarParam *param);
    ~Impl();
    void Update(float dt);

    /* 0x00 */ PooledList<CharaColliBase> m_list;
    /* 0x10 */ BossVarParam *m_param;
    /* 0x14 */ u8 m_stopped; /* BossVarParam::m_stopped as last applied to the colliders */
};
SIZE_ASSERT(BossCharaColli::Impl, 0x18);
SIZE_ASSERT(BossCharaColli, 0x30);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(BossCharaColli::Impl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<Impl> out of line (0x80178EDC) and ~Impl at its own place
 * (0x80179364). GC/2.7 inlines checked_delete when the destructor is not inline; this
 * specialization calls an inline helper instead, which gives retail's code and order (as in
 * Game/Rocket.cpp; see docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<BossCharaColli::Impl>(BossCharaColli::Impl *x) {
    DeleteImpl(x);
}
} // namespace boost

/* NonMatching (99.87%): in the first fn_800C7F7C call retail keeps m_param in r3 and sets up the
 * weak_ptr argument's address before the return slot's; GC/2.7 does the reverse and uses r5
 * (4 instructions). The second call matches. About 20 source shapes were tried. */
BossCharaColli::Impl::Impl(BossVarParam *param) : m_param(param), m_stopped(true) {
    if (m_param->m_charaPtr && m_param->m_motion) {
        boost::shared_ptr<CharaColliBase> collider;
        void *manager = fn_80021B08();
        UnkBossColli *colli = m_param->m_def->m_collis;
        if (colli) {
            while (colli->m_shape || colli->field_0x4 != -1) {
                if (colli->m_shape) {
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr),
                                           colli->m_shape, m_param->m_motion, colli->field_0x8, 0);
                }
                if (colli->field_0x4 != -1) {
                    void *part = fn_8001277C(fn_800122D4(), 0x16, colli->field_0x4);
                    collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_param->m_charaPtr), part,
                                           m_param->m_motion, colli->field_0x8, 0);
                }
                if (collider) {
                    collider->SetField2C(1);
                    m_list.PushBack(collider);
                    boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, collider);
                    boost::shared_ptr<CharaColliAttack> collision(attack.lock());
                    if (collision) {
                        collision.px->m_power = colli->field_0xC;
                        collision.px->m_kind = colli->field_0x10;
                        collision.px->m_flags = colli->field_0x14;
                        collision.px->field_0x54 = colli->field_0x18;
                        *reinterpret_cast<u32 *>(collision.px->field_0x3C) = colli->field_0x1C;
                        collision.px->field_0x55 = colli->field_0x20;
                    }
                }
                colli++;
            }
        }
    }
    param->m_colliders = &m_list;
}

BossCharaColli::Impl::~Impl() {
    if (m_list.Size() > 0) {
        m_list.Clear();
    }
    m_param->m_colliders = 0;
}

void BossCharaColli::Impl::Update(float dt) {
    PooledList<CharaColliBase>::iterator it;
    UnkBossColli *colli = m_param->m_def->m_collis;
    void *manager = fn_80021B08();
    for (it = PooledList<CharaColliBase>::iterator(m_list.Head()); it != m_list.End(); it++) {
        if ((*it).get() != 0) {
            if (m_stopped != m_param->m_stopped) {
                boost::weak_ptr<CharaColliAttack> attack = fn_800C42D0(manager, *it);
                boost::shared_ptr<CharaColliAttack> collision(attack.lock());
                if (collision) {
                    if (m_param->m_stopped) {
                        collision.px->field_0x55 = 0;
                    } else {
                        collision.px->field_0x55 = colli->field_0x20;
                    }
                    if (m_param->m_active) {
                        collision.px->field_0x54 = 1;
                    } else {
                        collision.px->field_0x54 = 0;
                    }
                }
            }
            colli++;
        }
    }
    m_stopped = m_param->m_stopped;
    UnkBossFocus *focus = m_param->m_def->m_focus;
    if (focus) {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_param->m_charaPtr->m_position);
        fn_8040B80C(&matrix, m_param->m_charaPtr->m_rotation.z);
        fn_8040B848(&matrix, m_param->m_charaPtr->m_rotation.y);
        fn_8040B884(&matrix, m_param->m_charaPtr->m_rotation.x);
        Vec3f center;
        fn_800091EC(&center, &Ref(fn_8040B67C(&matrix, focus->m_point)));
        UnkSphere_80021C04 sphere;
        fn_80021C04(&sphere, &center, focus->m_radius);
        void *object;
        UnkHits_80021CA4 *hits = fn_80021CA4(fn_80020D40(), &sphere, 1, 4, 0, 0, 0);
        if (hits) {
            Vec3f push;
            fn_800091EC(&push, &lbl_805E07A0);
            UnkHitIter_8001EDFC hit;
            for (hit = fn_8001EDFC(fn_8001EE04(hits)); hit.m_node != fn_8001EDEC(hits);
                 fn_80027374(&hit, 0)) {
                object = hit.m_node->m_object;
                if (object) {
                    Vec3f direction;
                    fn_800091EC(&direction, fn_8001E9C4(object));
                    fn_8040CE38(&direction, -fn_8001E9E8((u8 *)object + 0xC));
                    fn_8040CFF8(&push, &direction);
                }
            }
            push.x *= m_param->field_0x58.x;
            push.y *= m_param->field_0x58.y;
            push.z *= m_param->field_0x58.z;
            MoveBy(fn_801792D0(&m_param->m_charaPtr)->m_position, push);
            fn_80026FB4(hits, 1);
        }
    }
}

BossCharaColli::BossCharaColli(Task *parent, BossVarParam *param) : Task(parent), m_impl(new Impl(param)) {
    m_name = "CharaColli";
}

BossCharaColli::~BossCharaColli() {}

void BossCharaColli::Tick(float dt) {
    m_impl->Update(lbl_8057D6A0.m_frameTime);
}
