/* SonicteamUSA::System::Range::RangePlane (class name and base from RTTI): a box (RangeBox) of shape
 * kind 4. It tests other shapes as the box does, through its own table of the box's tests (indexed
 * by the other shape's kind). Member names and meanings are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#define RANGE_UNIT
#include "Game/RangeBox.h"
#include "Game/RangeBase.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::Range::RangeBase;
using SonicteamUSA::System::Range::RangeBox;

namespace SonicteamUSA {
namespace System {
namespace Range {

class RangePlane : public RangeBox {
  public:
    RangePlane(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position, const Vec3f *rotation, s32 order,
               boost::weak_ptr<CharaColliBase> owner);
    virtual ~RangePlane();
    virtual bool vf_0x48(RangeBase *other);
};

} // namespace Range
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Range::RangePlane;

extern "C" {
/* The box's tests (Game/RangeBox.cpp) and the capsule's and cylinder's tests against a box
 * (Game/RangeCapsule.cpp, Game/RangeCylinder.cpp). */
bool fn_8002A67C(RangeBase *box, RangeBase *sphere);
bool fn_80029ED0(RangeBase *box, RangeBase *other);
bool fn_8002EC10(RangeBase *capsule, RangeBase *box);
bool fn_80031EEC(RangeBase *cylinder, RangeBase *box);
}

typedef bool (*HitFn)(RangeBase *self, RangeBase *other);

static bool HitSphere(RangeBase *self, RangeBase *other);
static bool HitBox(RangeBase *self, RangeBase *other);
static bool HitCapsule(RangeBase *self, RangeBase *other);
static bool HitCylinder(RangeBase *self, RangeBase *other);
static bool HitPlane(RangeBase *self, RangeBase *other);

/* The tests by the other shape's kind (sphere, box, capsule, cylinder, plane). */
static HitFn s_hitTable[] = {HitSphere, HitBox, HitCapsule, HitCylinder, HitPlane};

RangePlane::RangePlane(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position, const Vec3f *rotation,
                       s32 order, boost::weak_ptr<CharaColliBase> owner)
    : RangeBox(desc, position, rotation, order, owner) {
    m_priority = 4;
    Update(position, rotation, order);
    UpdateBounds();
}

RangePlane::~RangePlane() {}

bool RangePlane::vf_0x48(RangeBase *other) {
    return s_hitTable[other->m_priority](this, other);
}

static bool HitSphere(RangeBase *self, RangeBase *other) {
    return fn_8002A67C(self, other);
}

static bool HitBox(RangeBase *self, RangeBase *other) {
    return fn_80029ED0(self, other);
}

static bool HitCapsule(RangeBase *self, RangeBase *other) {
    return fn_8002EC10(other, self);
}

static bool HitCylinder(RangeBase *self, RangeBase *other) {
    return fn_80031EEC(other, self);
}

static bool HitPlane(RangeBase *self, RangeBase *other) {
    return fn_80029ED0(self, other);
}
