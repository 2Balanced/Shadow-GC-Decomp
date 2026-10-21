/* ContainerBehavior (class name from RTTI): drops a pickup object (a container or a bomb) onto the
 * ground below it. Each update it falls by its fall speed and probes the stage below it through
 * its model's collider; once it lands it plays a sound, and it keeps a 10x10x10 box collider whose
 * height covers the distance to the ground. It waits while the stage block it is in is not ready.
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/ContainerBehavior.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetModelInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/TBlockManage.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The handle fn_8015B8C4 wraps a collider in (class name unknown; only the collider is mapped, as
 * in Bomb.cpp). */
class GadgetCharCollision {
  public:
    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ boost::shared_ptr<CharaColliBase> m_collider;
};

/* The surface a ground probe hit (class name unknown; only the member used here is named). */
struct UnkSurface_80021E24 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ s32 m_id; /* -1 marks a special surface (meaning unknown) */
};

/* What the ground probe below the object hits (class name unknown; names inferred; the same
 * result as in SetWeapon.cpp). */
struct UnkCollision_80021E24 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ UnkSurface_80021E24 *m_surface;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ void *m_object;
};

extern "C" {
/* The collision world, a probe along segment returning the first object for which filter is
 * true (ignoring the collider ignore), the hit object's position, and releasing the hit
 * (behaviour inferred). */
void *fn_80020D40(void);
UnkCollision_80021E24 *fn_80021E24(void *world, void *segment, int arg2, int arg3,
                                   bool (*filter)(UnkCollision_80021E24 *hit),
                                   CharaColliBase *ignore);
const Vec3f *fn_8001E9BC(void *object);
void fn_800265A4(UnkCollision_80021E24 *hit, int arg1);
/* The collision of a model variant (behaviour inferred). */
boost::weak_ptr<CharaColliBase> fn_80161614(GadgetModelInfo *model, int index);
/* The collision manager, and a box collider at position (behaviour inferred; the meaning of the
 * integer arguments is unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase>
fn_800C7D04(void *manager, const Vec3f &position, const Vec3f &size,
            const Vec3f &rotation, int arg4, int arg5, int arg6, int arg7,
            const boost::weak_ptr<CharInfo> &owner, const Vec3f &offset,
            const Vec3f &arg10, int arg11);
/* A vector constant read by the collision setup (meaning unknown). */
extern Vec3f lbl_805E07B8;
/* Wraps collider in a new handle (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
/* Plays sound id at position (behaviour inferred; the other arguments are unknown). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const Vec3f *position, int arg3, int arg4);
}

/* The empty character info (an .sbss shared_ptr; meaning inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* The size of the box collider (name inferred). */
static Vec3f s_colliSize(10.0f, 10.0f, 10.0f);

ContainerBehavior::ContainerBehavior(GadgetCharInfo *charInfo,
                                     boost::shared_ptr<GadgetModelInfo> model) {
    AssignVector(m_motion.m_position, charInfo->m_position);
    m_motion.m_fallSpeed = -1.2f;
    m_motion.m_groundY = 0.0f;
    m_model = model;
    m_block = fn_8003A154(&m_motion.m_position);
    {
        Vec3f size;
        fn_800091EC(&size, &s_colliSize);
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C7D04(fn_80021B08(), m_motion.m_position, size, m_motion.m_rotation, 2, 0x80002,
                        10, 0, lbl_805EE698, lbl_805E07A0, lbl_805E07B8, 0);
        m_handle = fn_8015B8C4(collider);
    }
    m_falling = true;
    m_flags = 0;
}

ContainerBehavior::~ContainerBehavior() {}

/* Probes from start to end for the ground below the object (name inferred): 0 for nothing, 1 for
 * ground and 2 for (meaning unknown) ground, whose height goes to groundY. */
static int ProbeGround(ContainerBehavior *behavior, const Vec3f *start,
                       const Vec3f *end, float *groundY);

extern "C" void fn_800F9000(ContainerBehavior *behavior) {
    if (behavior->m_block.get() != 0 && fn_80039EDC(behavior->m_block)) {
        return;
    }

    if (behavior->m_falling) {
        behavior->m_motion.m_position.y += behavior->m_motion.m_fallSpeed;
        Vec3f start;
        fn_800091EC(&start, &behavior->m_motion.m_position);
        Vec3f end;
        fn_800091EC(&end, &start);
        start.y += 5.0f;
        end.y -= 100.0f;
        int ground = ProbeGround(behavior, &start, &end, &behavior->m_motion.m_groundY);
        if (ground == 1) {
            if (behavior->m_motion.m_position.y < behavior->m_motion.m_groundY) {
                behavior->m_motion.m_position.y = behavior->m_motion.m_groundY;
            }
        } else if (ground == 2) {
            if (behavior->m_motion.m_position.y - behavior->m_motion.m_groundY < 5.0f) {
                behavior->m_flags |= 1;
            }
            if (behavior->m_motion.m_position.y < behavior->m_motion.m_groundY) {
                behavior->m_motion.m_position.y = behavior->m_motion.m_groundY;
            }
            behavior->m_falling = false;
        }
    } else {
        behavior->m_motion.m_position.y += behavior->m_motion.m_fallSpeed;
        if (behavior->m_motion.m_position.y < behavior->m_motion.m_groundY) {
            behavior->m_motion.m_position.y = behavior->m_motion.m_groundY;
            if (!(behavior->m_flags & 1)) {
                behavior->m_flags |= 1;
                fn_801D54F4(fn_80053164(), 0x1003, &behavior->m_motion.m_position, 0, 1);
            }
        }
    }

    if (behavior->m_handle.get() != 0) {
        behavior->m_handle->m_collider->SetCharInfo((CharInfo *)&behavior->m_motion.m_position);
        behavior->m_handle->m_collider->SetRotation(&behavior->m_motion.m_rotation);
        float height = 0.5f * (behavior->m_motion.m_position.y - behavior->m_motion.m_groundY);
        if (height > 0.0f) {
            behavior->m_handle->m_collider->SetSize(10.0f, height, 10.0f, -2);
        }
        Vec3f offset(0.0f, -height, 0.0f);
        behavior->m_handle->m_collider->SetOffset(&offset, -2);
    }
}

extern "C" void fn_800F8FC4(ContainerBehavior *behavior, const Vec3f *position,
                            const Vec3f *rotation) {
    fn_800EF474(behavior, position, rotation);
    behavior->m_falling = true;
    behavior->m_flags = 0;
}

/* The ground probe's filter (name inferred; meaning of the fields unknown). */
static bool IsGround(UnkCollision_80021E24 *hit) {
    return hit->field_0x8 != hit->field_0x10;
}

/* The surface of a hit (name inferred). */
inline UnkSurface_80021E24 *GetSurface(UnkCollision_80021E24 *hit) {
    return hit->m_surface;
}

/* Whether the hit surface has id -1 (name and meaning inferred). */
inline bool IsSpecialSurface(UnkCollision_80021E24 *hit) {
    return GetSurface(hit)->m_id == -1;
}

static int ProbeGround(ContainerBehavior *behavior, const Vec3f *start,
                       const Vec3f *end, float *groundY) {
    u8 segment[0x18];
    Vec3f position;
    boost::shared_ptr<GadgetModelInfo> model = behavior->m_model.lock();

    if (model.get() != 0) {
        fn_80020A20(segment, start, end);
        void *object;
        UnkCollision_80021E24 *hit = fn_80021E24(fn_80020D40(), segment, 3, 8, IsGround,
                                                 fn_80161614(model.get(), 0).lock().get());
        if (hit != 0) {
            object = hit->m_object;
            if (object != 0) {
                bool special = IsSpecialSurface(hit);
                fn_800091EC(&position, fn_8001E9BC(object));
                fn_800265A4(hit, 1);
                *groundY = position.y;
                if (special) {
                    return 2;
                }
                return 1;
            }
        }
    }
    return 0;
}
