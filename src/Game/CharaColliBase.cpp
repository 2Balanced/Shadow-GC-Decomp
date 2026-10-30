/* SonicteamUSA::System::CharaColli::CharaColliBase (class name from RTTI): the base class of the
 * character colliders (Game/CharaColli.h). This unit holds its constructor and destructor, which
 * emit its vtable and RTTI, the virtuals that pass a size, offset or extent on to one shape (the
 * first one for id -2, else the shape with that id), and fn_80037668, which makes the shape a
 * description asks for. Member names and meanings are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define CHARACOLLI_BASE_MEMBERS
#define CHARACOLLI_BASE_PURE
#define CHARACOLLI_EXTENT Fn8040CopyVector3
#define RANGE_UNIT
#include "Game/Fn8040CopyVector3.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliGeneral.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "Game/RangeBox.h"
#include "Game/RangeCapsule.h"
#include "Game/RangeCylinder.h"
#include "Game/RangeSphere.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::Range::RangeBase;
using SonicteamUSA::System::Range::RangeBox;
using SonicteamUSA::System::Range::RangeCapsule;
using SonicteamUSA::System::Range::RangeCylinder;
using SonicteamUSA::System::Range::RangeSphere;

namespace SonicteamUSA {
namespace System {
namespace Range {

/* RangePlane (class name from RTTI; Game/RangePlane.cpp): only its constructor and size are used
 * here. */
class RangePlane {
  public:
    RangePlane(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position, const Vec3f *rotation, s32 order,
               boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> owner);

    u8 field_0x0[0xB4];
};

} // namespace Range
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Range::RangePlane;

/* The collision result list the collider keeps at 0xC (Game/CharaColliManager.cpp). */
class CollisionResultList;

extern "C" {
CollisionResultList *fn_800270CC(CollisionResultList *list);
void fn_80026FB4(CollisionResultList *list, int arg1);
RangeBase *fn_80037668(CharaColliBase *owner, const UnkColliShapeDesc_80037668 *shape, const Vec3f *position,
                       const Vec3f *rotation, s32 arg);
}

/* The collider-less owner the shapes are made with (Game/RangeSphere.cpp). */
extern boost::shared_ptr<CharaColliBase> lbl_805EE0A0;

/* The id that names the first shape. */
#define FIRST_SHAPE 0xFFFFFFFEu

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

CharaColliBase::CharaColliBase(u16 type, u16 arg) : m_type(type), field_0x6(arg) {
    fn_800270CC(reinterpret_cast<CollisionResultList *>(field_0xC));
    m_moved = 1;
    m_changed = 1;
    field_0x2A = 1;
    field_0x2C = 0;
}

CharaColliBase::~CharaColliBase() {
    fn_80026FB4(reinterpret_cast<CollisionResultList *>(field_0xC), -1);
}

void CharaColliBase::TurnOn(u32 id) {
    if (field_0x2A == 0) {
        field_0x2B = 0;
        field_0x2A = 1;
        m_moved = 1;
    }
}

void CharaColliBase::TurnOff(u32 id) {
    field_0x2B = field_0x2A;
    field_0x2A = 0;
}

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

extern "C" RangeBase *fn_80037668(CharaColliBase *owner, const UnkColliShapeDesc_80037668 *shape,
                                  const Vec3f *position, const Vec3f *rotation, s32 arg) {
    switch (shape->m_kind) {
    case 0:
        return new RangeSphere(shape, position, boost::weak_ptr<CharaColliBase>(lbl_805EE0A0));
    case 1:
        return new RangeBox(shape, position, rotation, arg, boost::weak_ptr<CharaColliBase>(lbl_805EE0A0));
    case 2:
        return new RangeCapsule(shape, position, rotation, arg, boost::weak_ptr<CharaColliBase>(lbl_805EE0A0));
    case 3:
        return new RangeCylinder(shape, position, rotation, arg, boost::weak_ptr<CharaColliBase>(lbl_805EE0A0));
    case 4:
        return reinterpret_cast<RangeBase *>(
            new RangePlane(shape, position, rotation, arg, boost::weak_ptr<CharaColliBase>(lbl_805EE0A0)));
    }
    return 0;
}

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

s32 CharaColliBase::vf_0x1C(float *x, float *y, float *z, u32 id) {
    RangeBase *shape;
    if (id == FIRST_SHAPE) {
        shape = vf_0x28(0);
        shape->GetSize(x, y, z);
    } else {
        shape = vf_0x2C(id);
        if (shape != 0) {
            shape->GetSize(x, y, z);
        } else {
            return -1;
        }
    }
    return shape->m_priority;
}

void CharaColliBase::SetSize(float x, float y, float z, int id) {
    if (id == FIRST_SHAPE) {
        vf_0x28(0)->SetSize(x, y, z);
    } else {
        RangeBase *shape = vf_0x2C(id);
        if (shape != 0) {
            shape->SetSize(x, y, z);
        } else {
            return;
        }
    }
    vf_0x84();
    m_changed = 1;
}

Fn8040CopyVector3 CharaColliBase::vf_0x20(u32 id) {
    RangeBase *shape;
    if (id == FIRST_SHAPE) {
        shape = vf_0x28(0);
        return shape->GetExtent();
    }
    shape = vf_0x2C(id);
    if (shape != 0) {
        return shape->GetExtent();
    } else {
        return Fn8040CopyVector3(lbl_805E07A0);
    }
}

void CharaColliBase::SetOffset(const Vec3f *offset, int id) {
    if (id == FIRST_SHAPE) {
        vf_0x28(0)->SetOffset(offset);
    } else {
        RangeBase *shape = vf_0x2C(id);
        if (shape != 0) {
            shape->SetOffset(offset);
        } else {
            return;
        }
    }
    vf_0x84();
    m_changed = 1;
}

float CharaColliBase::vf_0x38() {
    return m_radius;
}

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA
