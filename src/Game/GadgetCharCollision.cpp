/* GadgetCharCollision (class name from RTTI; see Game/GadgetCharCollision.h): the contact handle
 * gadgets wrap their colliders in, with the collision event GadgetColliDetectArgs. File name
 * inferred from the class. Names of the free functions are placeholders; their behaviour is
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* boost::function1::operator() puts a bad_function_call on the stack: use the library's
 * runtime_error size (see <stdexcept>). */
#define MSL_RUNTIME_ERROR_STRING
/* The parts of Game/GadgetCharCollision.h only this unit sees. */
#define GADGETCHARCOLLISION_IMPL
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/GadgetCharCollision.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetCollision.h"
#include "boost/detail/sp_counted_base_impl.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A node of a collider's contact list (layout unused here; see BlockChange.cpp). */
struct UnkColliContactNode_800587A0;

/* The word at 0x50 of a character (as GetKind in BombServer.cpp; meaning unknown). */
inline s32 GetKind(CharInfo *info) {
    return *(s32 *)&info->field_0x3C[0x50 - 0x3C];
}

extern "C" {
/* The contact list: its first and end nodes, the step to the next node and the collider of a
 * node (see BlockChange.cpp); the collider manager, the character of a collider and the removal
 * of a collider (see BlockChange.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
void fn_80027398(UnkColliContactNode_800587A0 **it);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
void *fn_80021B08(void);
boost::weak_ptr<CharInfo> fn_800C4568(void *manager, const boost::weak_ptr<CharaColliBase> &collider);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
}

/* Deletes a handle (name inferred; see checked_delete below). */
inline void DeleteGadgetCharCollision(GadgetCharCollision *handle) {
    delete handle;
}

/* Retail keeps boost::checked_delete<GadgetCharCollision> out of line (0x8015BAD0) and
 * ~GadgetCharCollision at its own place; the specialization calling an inline helper gives that
 * (see docs/research-notes.md and EnergyCore.cpp). */
namespace boost {
template<> inline void checked_delete<GadgetCharCollision>(GadgetCharCollision *x) {
    DeleteGadgetCharCollision(x);
}
} // namespace boost

GadgetCharCollision::GadgetCharCollision(boost::shared_ptr<CharaColliBase> collider) {
    m_collider = collider;
    m_flags = 0;
    m_timer = 0.0f;
    m_ignore.clear();
}

GadgetCharCollision::~GadgetCharCollision() {
    m_ignore.clear();
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
}

GadgetColliDetectArgs::~GadgetColliDetectArgs() {}

/* The per-frame update: reports the collider's contacts to the callbacks unless the handle is
 * disabled or every touching character is ignored, and runs the timer. */
extern "C" void fn_8015BE9C(GadgetCharCollision *handle, float dt) {
    if (!(handle->m_flags & GadgetCharCollision::FLAG_TIMER) &&
        !(handle->m_flags & GadgetCharCollision::FLAG_DISABLED)) {
        GadgetColliDetectArgs args;
        UnkColliContacts_8001EE04 *contacts = reinterpret_cast<UnkColliContacts_8001EE04 *>(
            reinterpret_cast<u8 *>(handle->m_collider.get()) + 0xC);
        if (contacts != 0 && contacts->m_count > 0) {
            if (handle->m_ignore.size() != 0 && handle->IsAllIgnored(contacts)) {
                /* Only ignored characters touch the collider: nothing is reported. */
            } else {
                if (!(handle->m_flags & GadgetCharCollision::FLAG_TOUCHING)) {
                    /* Retail tests each callback through function1's operator!, out of line. */
                    if (!handle->m_onEnter.m_function == false) {
                        args.m_contacts = contacts;
                        handle->m_onEnter.CallIfSet(args);
                    }
                    handle->m_flags |= GadgetCharCollision::FLAG_TOUCHING;
                }
                if (!handle->m_onContact.m_function == false) {
                    args.m_contacts = contacts;
                    handle->m_onContact.CallIfSet(args);
                }
            }
        } else if (handle->m_flags & GadgetCharCollision::FLAG_TOUCHING) {
            if (!handle->m_onLeave.m_function == false) {
                args.m_contacts = 0;
                handle->m_onLeave.CallIfSet(args);
            }
            handle->m_flags &= ~GadgetCharCollision::FLAG_TOUCHING;
        }
    }
    if (handle->m_flags & GadgetCharCollision::FLAG_TIMER) {
        if (handle->m_timer > 0.0f) {
            handle->m_timer -= dt;
        } else {
            handle->m_flags &= ~GadgetCharCollision::FLAG_TIMER;
        }
    }
}

/* Re-registers the handle's collider. */
extern "C" void fn_8015BE64(GadgetCharCollision *handle) {
    if (handle->m_collider.get() != 0) {
        handle->m_collider->Reset();
    }
}

/* Ignores the characters whose words at 0x5C and 0x50 are kind and id. */
extern "C" void fn_8015BDC4(GadgetCharCollision *handle, int kind, int id) {
    std::pair<s32, s32> entry;
    entry.first = kind;
    entry.second = id;
    handle->m_ignore.push_back(entry);
}

/* NonMatching (99.67%): retail colours the two field comparisons' registers the other way round
 * (lwz r0 from the entry, r3 from the character, then cmpw r0,r3); about 35 source shapes tried
 * (operand order, casts, pair copies, inline helpers, enum/int/long types, index forms). */
bool GadgetCharCollision::IsAllIgnored(UnkColliContacts_8001EE04 *contacts) {
    UnkColliContactNode_800587A0 *it = fn_8001EE04(contacts);
    boost::weak_ptr<CharaColliBase> collider;
    boost::weak_ptr<CharInfo> owner;
    for (; it != fn_8001EDEC(contacts); fn_80027398(&it)) {
        collider = fn_800587A0(it);
        if (boost::shared_ptr<CharaColliBase> colliderRef = collider.lock()) {
            owner = fn_800C4568(fn_80021B08(), colliderRef);
            if (boost::shared_ptr<CharInfo> hit = owner.lock()) {
                bool found = false;
                for (std::pair<s32, s32> *entry = m_ignore.begin(); entry != m_ignore.end(); entry++) {
                    if (entry->first == hit->field_0x5C && entry->second == GetKind(hit.get())) {
                        found = true;
                    }
                }
                if (!found) {
                    return false;
                }
            }
        }
    }
    return true;
}

/* Wraps collider in a new handle. */
extern "C" boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider) {
    boost::shared_ptr<GadgetCharCollision> handle =
        boost::shared_ptr<GadgetCharCollision>(new GadgetCharCollision(collider));
    return handle;
}
