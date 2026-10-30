/* SetControl: puts the set objects to sleep and wakes them up again, all of them or those of one
 * group, or those of a group near a point (SetControl::WakeupByRangeEventArgs, class name from
 * RTTI). Each call walks the set objects with fn_800CB47C and changes their flags. The file is
 * named after the namespace. The functions keep their fn_ names (other units call them so);
 * their meanings and the member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/MathTypes.h"
#include "Game/SetAdapterEventArgs.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A set object's placement (struct name unknown; see Game/SetGenerator.cpp). */
struct UnkPlacement_800C9CD0;

namespace SetControl {

/* The objects of a group within a distance of a point (member names inferred). */
class WakeupByRangeEventArgs : public SetAdapterEventArgs {
  public:
    WakeupByRangeEventArgs() {}
    virtual ~WakeupByRangeEventArgs() {}

    /* 0x08 */ Vec3f m_position;
    /* 0x14 */ float m_rangeSquared;
};
SIZE_ASSERT(WakeupByRangeEventArgs, 0x18);

} // namespace SetControl

extern "C" {
void *fn_800C9B84(void);
void fn_800CB47C(void *manager, void *(*callback)(UnkPlacement_800C9CD0 *owner, void *arg), void *arg);
void fn_800C9EC4(UnkPlacement_800C9CD0 *owner, u32 *flags);
void fn_800C9E9C(UnkPlacement_800C9CD0 *owner, u32 mask);
void fn_800C9EB0(UnkPlacement_800C9CD0 *owner, u32 mask);
void fn_800C9F14(UnkPlacement_800C9CD0 *owner, Vec3f *position);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* vector -= value */
}

/* Wakes up an object unless it is disabled (name inferred). */
static void *fn_801ED720(UnkPlacement_800C9CD0 *owner, void *arg) {
    u32 flags = 0;
    fn_800C9EC4(owner, &flags);
    if (!(flags & 1)) {
        fn_800C9EB0(owner, 0x800000);
    }
    fn_800C9E9C(owner, 0x20000);
    return owner;
}

/* Wakes up an object within the range (name inferred). */
static void *fn_801ED660(UnkPlacement_800C9CD0 *owner, void *arg) {
    SetControl::WakeupByRangeEventArgs *args = (SetControl::WakeupByRangeEventArgs *)arg;
    u32 flags = 0;
    Vec3f offset;
    fn_800C9F14(owner, &offset);
    fn_8040CE04(&offset, &args->m_position);
    offset.y = 0.0f;
    if (fn_8040CF2C(&offset) > args->m_rangeSquared || fn_800C996C(owner)) {
        return owner;
    }
    fn_800C9EC4(owner, &flags);
    if (flags & 2) {
        return owner;
    }
    if (!(flags & 1)) {
        fn_800C9EB0(owner, 0x800000);
    }
    fn_800C9E9C(owner, 0x20000);
    return owner;
}

/* Puts an object to sleep (name inferred). */
static void *fn_801ED600(UnkPlacement_800C9CD0 *owner, void *arg) {
    u32 flags = 0;
    fn_800C9EC4(owner, &flags);
    if (flags & 2) {
        fn_800C9EB0(owner, 4);
    }
    fn_800C9EB0(owner, 0x20000);
    return owner;
}

/* Wakes up the objects of a group (name inferred). */
extern "C" void fn_801ED5A8(u8 group) {
    SetAdapterEventArgs args;
    args.field_0x0 = group;
    fn_800CB47C(fn_800C9B84(), fn_801ED720, &args);
}

/* Wakes up the objects of a group within a distance of a point (name inferred). */
extern "C" void fn_801ED4B0(u8 group, const Vec3f *position, float rangeSquared) {
    SetControl::WakeupByRangeEventArgs args;
    args.field_0x0 = group;
    fn_800091D0(&args.m_position, position);
    args.m_rangeSquared = rangeSquared;
    fn_800CB47C(fn_800C9B84(), fn_801ED660, &args);
}

/* Puts the objects of a group to sleep (name inferred). */
extern "C" void fn_801ED410(u8 group) {
    SetAdapterEventArgs args;
    args.field_0x0 = group;
    fn_800CB47C(fn_800C9B84(), fn_801ED600, &args);
}
