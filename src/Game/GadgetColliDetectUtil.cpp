/* Helpers gadget units share that walk the characters of a collision event (ColliDetectArgs):
 * each calls a callback for, or tests for, the characters passing one of CharInfo's virtual tests
 * or matching given words. The file name is invented (the unit has no data to name it); it ends
 * where Game/GadgetCharCollision.cpp starts, which emits its own copy of the local weak_ptr
 * conversion this unit uses. Names of the functions are placeholders; their behaviour is
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/GadgetCollision.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A node of a collider's contact list (layout unused here; see BlockChange.cpp). */
struct UnkColliContactNode_800587A0;

/* What the character walks call for each character (see ElecBar.cpp). */
typedef void (*UnkHitCallback)(boost::shared_ptr<CharInfo> hit, void *user);

/* CharInfo's slot 0x28 as the character walks below use it: a test (CharInfo.h declares the slot
 * void, as its overrides in other units do). Same layout as CharInfo up to that slot. */
class UnkCharInfoTest_0x28 {
  public:
    /* 0x00 */ u8 field_0x0[0x38];

    /* 0x38: vtable pointer */
    virtual ~UnkCharInfoTest_0x28();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual void vf_0x20();
    virtual void vf_0x24();
    virtual bool vf_0x28();
};

/* The word at 0x50 of a character (as GetKind in BombServer.cpp; meaning unknown). */
inline s32 GetKind(CharInfo *info) {
    return *(s32 *)&info->field_0x3C[0x50 - 0x3C];
}

extern "C" {
/* The contact list: its first and end nodes, the step to the next node and the collider of a
 * node (see BlockChange.cpp); the collider manager and the character of a collider. */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
void fn_80027398(UnkColliContactNode_800587A0 **it);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
void *fn_80021B08(void);
boost::weak_ptr<CharInfo> fn_800C4568(void *manager, const boost::weak_ptr<CharaColliBase> &collider);
}

/* Calls callback with every character of kind in the collision event. */
extern "C" void fn_8015B70C(ColliDetectArgs &args, UnkHitCallback callback, void *user, int kind) {
    UnkColliContacts_8001EE04 *contacts = args.m_contacts;
    UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
        collider = fn_800587A0(it);
        if (boost::shared_ptr<CharaColliBase> colliderRef = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), colliderRef);
            if (boost::shared_ptr<CharInfo> hit = owner.lock()) {
                if (hit->field_0x5C == kind && callback != 0) {
                    callback(hit, user);
                }
            }
        }
    }
}

/* True when a character of kind and id is in the collision event. */
extern "C" bool fn_8015B544(ColliDetectArgs &args, int kind, int id) {
    UnkColliContacts_8001EE04 *contacts = args.m_contacts;
    UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
        collider = fn_800587A0(it);
        if (boost::shared_ptr<CharaColliBase> colliderRef = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), colliderRef);
            if (boost::shared_ptr<CharInfo> hit = owner.lock()) {
                if (kind == hit->field_0x5C && id == GetKind(hit.get())) {
                    return true;
                }
            }
        }
    }
    return false;
}

/* Calls callback with every character in the collision event that passes its slot 0x2C test. */
extern "C" void fn_8015B384(ColliDetectArgs &args, UnkHitCallback callback, void *user) {
    UnkColliContacts_8001EE04 *contacts = args.m_contacts;
    UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
        collider = fn_800587A0(it);
        if (boost::shared_ptr<CharaColliBase> colliderRef = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), colliderRef);
            if (boost::shared_ptr<CharInfo> hit = owner.lock()) {
                if (hit->vf_0x2C() && callback != 0) {
                    callback(hit, user);
                }
            }
        }
    }
}

/* True when a character in the collision event passes its slot 0x18 test. */
extern "C" bool fn_8015B1C4(ColliDetectArgs &args) {
    UnkColliContacts_8001EE04 *contacts = args.m_contacts;
    UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
        collider = fn_800587A0(it);
        if (boost::shared_ptr<CharaColliBase> colliderRef = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), colliderRef);
            if (boost::shared_ptr<CharInfo> hit = owner.lock()) {
                if (hit->vf_0x18()) {
                    return true;
                }
            }
        }
    }
    return false;
}

/* Calls callback with every character in the collision event that passes its slot 0x18 test. */
extern "C" void fn_8015B004(ColliDetectArgs &args, UnkHitCallback callback, void *user) {
    UnkColliContacts_8001EE04 *contacts = args.m_contacts;
    UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
        collider = fn_800587A0(it);
        if (boost::shared_ptr<CharaColliBase> colliderRef = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), colliderRef);
            if (boost::shared_ptr<CharInfo> hit = owner.lock()) {
                if (hit->vf_0x18() && callback != 0) {
                    callback(hit, user);
                }
            }
        }
    }
}

/* Calls callback with every character in the collision event that passes its slot 0x24 test. */
extern "C" void fn_8015AE44(ColliDetectArgs &args, UnkHitCallback callback, void *user) {
    UnkColliContacts_8001EE04 *contacts = args.m_contacts;
    UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
        collider = fn_800587A0(it);
        if (boost::shared_ptr<CharaColliBase> colliderRef = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), colliderRef);
            if (boost::shared_ptr<CharInfo> hit = owner.lock()) {
                if (hit->vf_0x24() && callback != 0) {
                    callback(hit, user);
                }
            }
        }
    }
}

/* True when a character in the collision event passes its slot 0x28 test. */
extern "C" bool fn_8015AC84(ColliDetectArgs &args) {
    UnkColliContacts_8001EE04 *contacts = args.m_contacts;
    UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
        collider = fn_800587A0(it);
        if (boost::shared_ptr<CharaColliBase> colliderRef = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), colliderRef);
            if (boost::shared_ptr<CharInfo> hit = owner.lock()) {
                if (reinterpret_cast<UnkCharInfoTest_0x28 *>(hit.get())->vf_0x28()) {
                    return true;
                }
            }
        }
    }
    return false;
}

/* Calls callback with every character in the collision event that passes its slot 0x28 test. */
extern "C" void fn_8015AA84(ColliDetectArgs &args, UnkHitCallback callback, void *user) {
    UnkColliContacts_8001EE04 *contacts = args.m_contacts;
    UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
        collider = fn_800587A0(it);
        if (boost::shared_ptr<CharaColliBase> colliderRef = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), colliderRef);
            if (boost::shared_ptr<CharInfo> hit = owner.lock()) {
                if (reinterpret_cast<UnkCharInfoTest_0x28 *>(hit.get())->vf_0x28() && callback != 0) {
                    callback(hit, user);
                }
            }
        }
    }
}
