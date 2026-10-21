/* GadgetCommand::GadgetCommand (class name from RTTI): the base of the commands gadgets send
 * (types 0x301..0x304: MoveStart, MoveEnd, AttackStart, AttackEnd). The unit also holds helpers
 * gadget units share (contact tests, the attacker of an attack, a line probe). File name inferred
 * from the class. Names of the free functions are placeholders; their behaviour is
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetUtil.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A node of a collider's contact list (layout unused here; see BlockChange.cpp). */
struct UnkColliContactNode_800587A0;

/* An iterator over a contact list (as in BlockChange.cpp; name inferred). */
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* What fn_80021E24 returns (layout from its use here). */
struct UnkLineHit_80021E24 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_hit;
};

class UnkLineManager_80020D40;

extern "C" {
/* The contact list: its first and end nodes, the iterator's post-increment, and the collider of a
 * node (see BlockChange.cpp); the collider manager and the character of a collider. */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);
void *fn_80021B08(void);
boost::weak_ptr<CharInfo> fn_800C4568(void *manager, const boost::weak_ptr<CharaColliBase> &collider);
/* The line probe: its manager, the query, the hit point and the release of the result. */
UnkLineManager_80020D40 *fn_80020D40(void);
UnkLineHit_80021E24 *fn_80021E24(UnkLineManager_80020D40 *manager, void *segment, int arg2, int arg3,
                                 int arg4, int arg5);
const Vec3f *fn_8001E9BC(void *hit);
void fn_800265A4(UnkLineHit_80021E24 *result, int arg1);
}

namespace GadgetCommand {

class GadgetCommand : public CharCommand {
  public:
    virtual ~GadgetCommand();
    virtual const char *GetName();
};
SIZE_ASSERT(GadgetCommand, 0x14);

} // namespace GadgetCommand

/* The names of the gadget command types. */
struct GadgetCommandName {
    s32 type;
    const char *name;
};

static const GadgetCommandName lbl_804C2634[4] = {
    {0x301, "MoveStart"},
    {0x302, "MoveEnd"},
    {0x303, "AttackStart"},
    {0x304, "AttackEnd"},
};

extern "C" boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param);

/* The kind of a character (as in BombServer.cpp; meaning unknown). */
inline s32 GetKind(CharInfo *info) {
    return *(s32 *)&info->field_0x3C[0x50 - 0x3C];
}

namespace GadgetCommand {

const char *GadgetCommand::GetName() {
    u16 type = *reinterpret_cast<u16 *>(field_0x4);
    for (int i = 0; i < 4; i++) {
        if (type == lbl_804C2634[i].type) {
            return lbl_804C2634[i].name;
        }
    }
    return "Unknown GadgetCommand ?";
}

GadgetCommand::~GadgetCommand() {}

} // namespace GadgetCommand

/* Probes the line from start to end; on a hit stores the hit point in point (when given). */
extern "C" bool fn_8015D61C(Vec3f *point, const Vec3f *start, const Vec3f *end) {
    u8 segment[0x18];
    bool found = false;
    fn_80020A20(segment, start, end);
    UnkLineHit_80021E24 *result = fn_80021E24(fn_80020D40(), segment, 1, 8, 0, 0);
    if (result != 0) {
        if (result->m_hit != 0) {
            if (point != 0) {
                fn_800091D0(point, fn_8001E9BC(result->m_hit));
            }
            found = true;
        }
        fn_800265A4(result, 1);
    }
    return found;
}

/* The character behind an attack. */
extern "C" boost::shared_ptr<CharInfo> fn_8015D570(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker;
    boost::shared_ptr<CharaColliAttack> collider = param.m_attacker.lock();
    if (collider.get() != 0) {
        attacker = collider->m_owner.lock();
    }
    return attacker;
}

/* True when the attacker's word at 0x50 is 1, 2 or 3. */
extern "C" bool fn_8015D4E4(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = fn_8015D570(param);
    if (attacker.get() != 0) {
        if (GetKind(attacker.get()) == 1 || GetKind(attacker.get()) == 2 ||
            GetKind(attacker.get()) == 3) {
            return true;
        }
    }
    return false;
}

/* True when a character in contacts passes its slot 0x18 test. */
extern "C" bool fn_8015D3E8(UnkColliContacts_8001EE04 *contacts) {
    if (contacts != 0) {
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts)); it.m_node != fn_8001EDEC(contacts);
             fn_80027374(&it, 0)) {
            boost::shared_ptr<CharInfo> hit = fn_800C4568(fn_80021B08(), fn_800587A0(it.m_node)).lock();
            if (hit.get() != 0 && hit->vf_0x18()) {
                return true;
            }
        }
    }
    return false;
}

/* True when a character in contacts passes its slot 0x24 test. */
extern "C" bool fn_8015D2EC(UnkColliContacts_8001EE04 *contacts) {
    if (contacts != 0) {
        for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts)); it.m_node != fn_8001EDEC(contacts);
             fn_80027374(&it, 0)) {
            boost::shared_ptr<CharInfo> hit = fn_800C4568(fn_80021B08(), fn_800587A0(it.m_node)).lock();
            if (hit.get() != 0 && hit->vf_0x24()) {
                return true;
            }
        }
    }
    return false;
}
