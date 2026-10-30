/* SonicteamUSA::System::CharaColli::CharaColliBodyCharInfo (class name from RTTI): a
 * CharaColliBody (Game/CharaColliBody.cpp) that takes its position, rotation and word 0x30 from
 * its character (slot 0x88) instead of being moved; the setters do nothing. Member and function
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColliBody.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;
using SonicteamUSA::System::CharaColli::CharaColliBodyCharInfo;

extern "C" {
/* The character a collider follows (an inline function of the game, first emitted in
 * Game/BulletUtil.cpp). */
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
}

/* An object whose out-of-line constructor follows the collider's (class unknown; a function-local
 * static of the unit at 0x801E2128 is built with it). */
struct UnkObject_800C18AC {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ u8 field_0x8;
};

extern "C" UnkObject_800C18AC *fn_800C18AC(UnkObject_800C18AC *object) {
    object->field_0x4 = 0;
    object->field_0x8 = 0;
    return object;
}

inline bool operator!=(const Vec3f &a, const Vec3f &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

CharaColliBodyCharInfo::CharaColliBodyCharInfo(boost::weak_ptr<CharInfo> info, const UnkColliShapeDesc_80037668 *shape,
                                               u16 type, u16 typeArg)
    : CharaColliBody(shape, &info.lock()->m_position, &info.lock()->m_rotation, info.lock()->field_0x30, type,
                     typeArg, info) {}

void CharaColliBodyCharInfo::vf_0x88() {
    CharInfo *info;
    if (fn_800586EC(this).expired() || (info = fn_800586EC(this).lock().get()) == 0) {
        return;
    }
    if (info->m_position != m_position) {
        fn_800091D0(&m_position, &info->m_position);
        m_moved = 1;
    }
    if (info->m_rotation.x != m_rotation.x || info->m_rotation.y != m_rotation.y ||
        info->m_rotation.z != m_rotation.z) {
        m_rotation.x = info->m_rotation.x;
        m_rotation.y = info->m_rotation.y;
        m_rotation.z = info->m_rotation.z;
        m_moved = 1;
    }
    if (info->field_0x30 != field_0x58) {
        field_0x58 = info->field_0x30;
        m_moved = 1;
    }
}

void CharaColliBodyCharInfo::SetRotation(const Vec3f *rotation) {}

void CharaColliBodyCharInfo::SetCharInfo(CharInfo *info) {}

CharaColliBodyCharInfo::~CharaColliBodyCharInfo() {}
