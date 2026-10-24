/* Item (class name from RTTI: Weapon::Item): a weapon lying in the stage, shown by a weapon model
 * without an owner, and the functions that make it blink, place it, and work out where it lies when
 * dropped (other units call them by their addresses, so they keep those names). Item's slot 0x0C is
 * in Game/WeaponTable.cpp. File name from the class name; method names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/WeaponItem.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/WeaponBase.h"
#include "boost/weak_ptr.hpp"

using Weapon::Item;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponModel_80063428;

extern "C" {
/* Matrix and vector helpers (behaviour inferred; see Game/ArkCannon.cpp): a matrix turned by Euler
 * angles in an axis order, the inverse of a matrix, the product of two, a matrix multiplied in
 * place, a vector taken through a matrix, the Euler angles of a forward and an up direction, and the
 * translation of a matrix. */
Mtx34f fn_8040FFBC(const Mtx34f *matrix, const Vec3f *angles, int order);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up);
Vec3f fn_8040FB08(const Mtx34f *matrix);
}

/* The item's model is the word at 0x4, which Game/WeaponItem.h declares as a plain word; the code
 * below casts it at each use (an inline accessor would let the compiler keep the pointer in a
 * register across calls, which retail does not). */
typedef UnkWeaponModel_80063428 Model;

namespace Weapon {

Item::Item(const UnkWeaponEffectSet_80063428 *effects) {
    field_0x4 = reinterpret_cast<u32>(new Model(boost::weak_ptr<CharInfo>(), effects));
    m_ammo = -1;
}

Item::Item(void *resource, const UnkWeaponEffectSet_80063428 *effects) {
    field_0x4 = reinterpret_cast<u32>(new Model(resource, effects));
    m_ammo = -1;
}

Item::~Item() {
    delete reinterpret_cast<Model *>(field_0x4);
}

} // namespace Weapon

extern "C" {

/* Places the item. */
void fn_8005F274(Item *item, const Vec3f *position, const Vec3f *rotation) {
    if (reinterpret_cast<Model *>(item->field_0x4) != 0) {
        reinterpret_cast<Model *>(item->field_0x4)->fn_800632E8(position, rotation);
        reinterpret_cast<Model *>(item->field_0x4)->fn_800632A0(0.0f);
    }
}

/* Where an item dropped at position with rotation lies: placed by the line from its model's node 0
 * to its node 1 (meaning inferred). */
void fn_8005F0F4(Item *item, Vec3f *outPosition, Vec3f *outRotation, const Vec3f *position,
                 const Vec3f *rotation) {
    if (reinterpret_cast<Model *>(item->field_0x4) != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        fn_8040B034(&translation, position);
        fn_8040C148(&matrix, &translation);
        fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FFBC(&matrix, rotation, 2));
        /* The matrix taken on by the line from node 0 to node 1. */
        fn_8040BAC4(&matrix,
                    &(const Mtx34f &)fn_8001E044(
                        reinterpret_cast<Model *>(item->field_0x4)->fn_80063234(1),
                        &(const Mtx34f &)fn_8040BEF8(reinterpret_cast<Model *>(item->field_0x4)->fn_80063234(0))));
        Vec3f forward;
        fn_800091D0(&forward,
                    &(const Vec3f &)fn_8040B618(&matrix, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f)));
        Vec3f up;
        fn_800091D0(&up, &(const Vec3f &)fn_8040B618(&matrix, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f)));
        *outRotation = fn_8040F364(&forward, &up);
        fn_800091D0(outPosition, &(const Vec3f &)fn_8040FB08(&matrix));
    } else {
        fn_800091D0(outPosition, position);
        *outRotation = *rotation;
    }
}

} // extern "C"

namespace Weapon {

void Item::Tick(float dt) {
    if (reinterpret_cast<Model *>(field_0x4) != 0) {
        reinterpret_cast<Model *>(field_0x4)->fn_800632A0(dt);
    }
}

} // namespace Weapon

/* Makes the item blink. */
extern "C" void fn_8005F09C(Item *item) {
    if (reinterpret_cast<Model *>(item->field_0x4) != 0) {
        reinterpret_cast<Model *>(item->field_0x4)->fn_800631D0();
    }
}
