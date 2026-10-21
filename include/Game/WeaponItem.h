#ifndef GAME_WEAPONITEM_H
#define GAME_WEAPONITEM_H

#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>

namespace Weapon {

struct UnkWeaponEffectSet_80063428;

/* Item (class name from RTTI; vtable at 0x8051EC20): a weapon lying in the stage, which a character
 * picks up. fn_8005B9D4 creates one for a weapon type. Only what gadget code uses is named (names
 * inferred from that use); the size is unknown. */
class Item {
  public:
    /* Builds the item's model with effects (0x8005F380). */
    Item(const UnkWeaponEffectSet_80063428 *effects);
    virtual ~Item();
    virtual void vf_0x0C(int arg);
    /* Per-frame work (slot 0x10). */
    virtual void Tick(float dt);

    /* The weapon type (name inferred). */
    s32 TypeId() const {
        return m_type;
    }

    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ s32 m_type;
    /* 0x0C */ s32 m_ammo; /* passed on with the type when the item is picked up (inferred) */
};

} // namespace Weapon

extern "C" {
/* Creates the item of weapon type type (arg1's meaning unknown); empty when the type has none
 * (behaviour inferred). */
boost::shared_ptr<Weapon::Item> fn_8005B9D4(int type, int arg1);
}

#endif
