/* GunBattery and BKBattery (names from the records' name strings): two entries of a table of
 * 0x30-byte type records that a central table at 0x8051ED00.. lists next to weapon and object
 * types (fn_800536D8, fn_80055D8C, ...). This unit holds no class, only the two records and their
 * callbacks, which play a sound and start an effect at a position. File name and all other names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Starts effect id at position for owner (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E418(const Vec3f *position, int id,
                                                           boost::weak_ptr<CharInfo> owner);
}

/* An empty character (used as "no owner"; name inferred). */
extern boost::shared_ptr<CharInfo> lbl_805EF980;

/* A time in seconds (name inferred). As a call it is not a constant expression, so the records
 * are filled in by a static initializer, as in retail. */
inline float Seconds(float seconds) {
    return seconds;
}

/* The type record (layout from the other records in the central table; names inferred). */
struct UnkTypeRecord_80542080 {
    /* 0x00 */ const char *name;
    /* 0x04 */ void (*callback0)(const Vec3f *position);
    /* 0x08 */ void (*callback1)(const Vec3f *position);
    /* 0x0C */ void (*onEffect)(const Vec3f *position);
    /* 0x10 */ float field_0x10;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ u32 field_0x24;
    /* 0x28 */ float field_0x28;
    /* 0x2C */ float field_0x2C;
};
SIZE_ASSERT(UnkTypeRecord_80542080, 0x30);

static void GunBatteryEffect(const Vec3f *position) {
    fn_801D560C(fn_80053164(), 0x200B, 0, 0);
    fn_8005E418(position, 0x18C, lbl_805EF980);
}

static void BKBatteryEffect(const Vec3f *position) {
    fn_801D560C(fn_80053164(), 0x201B, 0, 0);
    fn_8005E418(position, 0x192, lbl_805EF980);
}

UnkTypeRecord_80542080 lbl_80542080 = {
    "GunBattery", 0, 0, GunBatteryEffect, Seconds(1.0f), Seconds(5.0f), 0, 1, 0, 0, 0.2f, 1.0f,
};

UnkTypeRecord_80542080 lbl_805420B0 = {
    "BKBattery", 0, 0, BKBatteryEffect, Seconds(1.0f), Seconds(5.0f), 0, 1, 0, 0, 0.2f, 1.0f,
};
