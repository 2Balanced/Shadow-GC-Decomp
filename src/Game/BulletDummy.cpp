/* A bullet type without callbacks (file and record names inferred): the bullet table (0x8051C400)
 * uses this record for the slots of bullet types that create nothing. Only its reach is set, by
 * the static initializer. */
#include "Game/WeaponBullet.h"
#include "size_assert.h"

using Weapon::ShotInfo;

/* A bullet type's record (struct name unknown; see BulletLaser.cpp). */
struct UnkBulletType_80546018 {
    /* 0x00 */ void (*initialize)(void);
    /* 0x04 */ void (*finalize)(void);
    /* 0x08 */ void (*create)(const ShotInfo &info);
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ float m_reach;
};
SIZE_ASSERT(UnkBulletType_80546018, 0x14);

/* A distance (name inferred). As a call it is not a constant expression, so the record's value is
 * stored by the static initializer, as in retail. */
inline float Plain(float value) {
    return value;
}

UnkBulletType_80546018 lbl_8054DB70 = {0, 0, 0, 0, Plain(600.0f)};
