/* SonicteamUSA::System::CharaColli::CharaColliGeneralBone (class name from RTTI): a
 * CharaColliGeneralCharInfo (Game/CharaColliGeneralCharInfo.cpp) whose shapes follow the bones of a
 * motion: the constructor looks up each shape's bone by the shape's id, and slot 0x90 hands the
 * bones' matrices to the shapes. Its bounding radius is measured from the collider's position to
 * each shape's. It uses CharaColliGeneral's list functions, which Game/CharaColliGeneral.cpp emits.
 * Member and function names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CharaColliGeneral.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::CharaColliGeneralBone;
using SonicteamUSA::System::CharaColli::CharaColliGeneralCharInfo;
using SonicteamUSA::System::CharaColli::ColliShapeList;

extern "C" {
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *block);
/* The matrix of a motion's bone. */
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
}

CharaColliGeneralBone::CharaColliGeneralBone(boost::weak_ptr<CharInfo> info,
                                             const UnkColliShapeDesc_80037668 *shapes, _stMtnPlayWork *motion,
                                             u16 type, u16 typeArg)
    : CharaColliGeneralCharInfo(info, shapes, type, typeArg) {
    m_bones = static_cast<Mtx34f **>(fn_80049B90(m_count * 4, 2));
    Mtx34f **bone = m_bones;
    ColliShapeList::iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++, bone++) {
        *bone = fn_80415E70(motion, (*it)->m_id);
    }
    vf_0x90();
}

CharaColliGeneralBone::~CharaColliGeneralBone() {
    if (m_bones != 0) {
        fn_80049B34(m_bones);
    }
}

void CharaColliGeneralBone::vf_0x90() {
    Mtx34f **bone = m_bones;
    ColliShapeList::iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++, bone++) {
        if (*bone != 0) {
            Mtx34f *matrix = *bone;
            (*it)->SetMatrix(matrix);
        }
    }
    m_changed = 1;
}

void CharaColliGeneralBone::vf_0x7C() {
    vf_0x88();
}

void CharaColliGeneralBone::vf_0x88() {
    CharaColliGeneralCharInfo::vf_0x88();
    vf_0x8C();
    vf_0x90();
    vf_0x84();
}

void CharaColliGeneralBone::vf_0x84() {
    ColliShapeList::iterator it = m_shapes.begin();
    SonicteamUSA::System::Range::RangeBase *first = *it;
    Vec3f offset;
    fn_8040CD74(&offset, &first->m_position, vf_0x14());
    float radius = fn_8040CF50(&offset) + first->GetMargin();
    for (; it != m_shapes.end(); it++) {
        SonicteamUSA::System::Range::RangeBase *shape = *it;
        if (shape->m_enabled) {
            Vec3f shapeOffset;
            fn_8040CD74(&shapeOffset, &shape->m_position, vf_0x14());
            float reach = fn_8040CF50(&shapeOffset) + shape->GetMargin();
            if (radius < reach) {
                radius = reach;
            }
        }
    }
    m_radius = radius;
}
