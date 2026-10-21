#ifndef GAME_GADGETCOLLISION_H
#define GAME_GADGETCOLLISION_H

#include <boost/function.hpp>

#include "Game/CharInfo.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* The contacts a collider keeps 0xC bytes into the object (layout from fn_8001EE04, which returns
 * the items, and from the gadgets that test the count; name inferred). */
struct UnkColliContacts_8001EE04 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ s32 m_count;
    /* 0x8 */ void *m_items;
};

/* The collision event a model reports (name from the bound callbacks' RTTI). Gadget code mostly
 * passes it through; a gadget can also build one over a collider's contacts to test them (layout
 * from those builds; its vtable at 0x80526C48 holds only the destructor at 0x80112C74). The vtable
 * pointer sits at 0x8, after the members declared before the first virtual function. */
struct ColliDetectArgs {
    /* 0x0 */ UnkColliContacts_8001EE04 *m_contacts;
    /* 0x4 */ s32 m_index;

    ColliDetectArgs() : m_contacts(0), m_index(0) {}
    virtual ~ColliDetectArgs() {}
};

/* The characters a gadget has already hit, behind a pointer to its implementation (names and
 * behaviour inferred from the callers). fn_801695B8 creates one; fn_8016952C releases a gadget's
 * list when the gadget is destroyed. */
class UnkHitList_801695B8 {
  public:
    /* True when target was already hit (inferred). */
    bool fn_801697CC(const boost::weak_ptr<CharInfo> &target);
    /* Per-frame update (inferred). */
    void fn_801697F0();
};

extern "C" {
boost::shared_ptr<UnkHitList_801695B8> fn_801695B8(void);
void fn_8016952C(boost::shared_ptr<UnkHitList_801695B8> *list);
/* Rumbles the controller of the player behind target (behaviour inferred from the vibration code
 * it calls); the second argument picks the pattern. */
boost::weak_ptr<Vibration::RequestHandle> fn_80169BB0(const boost::weak_ptr<CharInfo> &target, int arg1);
/* Calls callback with every character in the collision event (behaviour inferred). */
void fn_8015B004(ColliDetectArgs &args, void (*callback)(boost::shared_ptr<CharInfo> hit, void *user),
                 void *user);
/* Sets the model's collision callback (inferred). */
void fn_800D0B64(void *collision, const boost::function1<void, ColliDetectArgs &> &callback);
}

#endif
