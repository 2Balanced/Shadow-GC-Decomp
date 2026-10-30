/* SonicteamUSA::System::CharaColli::CharaColliGeneralCharInfo (class name from RTTI): a
 * CharaColliGeneral (Game/CharaColliGeneral.cpp) that takes its position, rotation and word 0x30
 * from its character (slot 0x88), as CharaColliBodyCharInfo does for a single shape. Its empty
 * setters are inline; the unit of CharaColliGeneralBone emits them first. Member and function names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColliGeneral.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;
using SonicteamUSA::System::CharaColli::CharaColliGeneralCharInfo;

extern "C" {
/* The character a collider follows (an inline function of the game, first emitted in
 * Game/BulletUtil.cpp). */
boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider);
}

inline bool operator!=(const Vec3f &a, const Vec3f &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

CharaColliGeneralCharInfo::CharaColliGeneralCharInfo(boost::weak_ptr<CharInfo> info,
                                                     const UnkColliShapeDesc_80037668 *shapes, u16 type,
                                                     u16 typeArg)
    : CharaColliGeneral(shapes, &info.lock()->m_position, &info.lock()->m_rotation, info.lock()->field_0x30, type,
                        typeArg, info) {}

void CharaColliGeneralCharInfo::vf_0x88() {
    CharInfo *info;
    if (fn_800586EC(this).expired() || (info = fn_800586EC(this).lock().get()) == 0) {
        return;
    }
    if (info->m_position != reinterpret_cast<const Vec3f &>(m_position)) {
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
