/* BlockChange (class name from RTTI): the BLOCK_CHANGE gadget, a task (not a Gadget) with a
 * sphere collider of the "Radius" parameter. When a player touches the collider, the four "BLOCK"
 * parameters name blocks of that player's block set that the gadget switches on (state 1) or off
 * (state 2) according to its "On/Off" parameter. It re-reads its parameters when the owner reports
 * a change and finishes when the owner goes away. File name from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* A node of a collider's contact list: the contact's collider comes first and the next node at
 * 0x1C (layout from fn_800587A0 and fn_80027374; name inferred). */
struct UnkColliContactNode_800587A0;

/* An iterator over a contact list (layout from fn_8001EDFC; name inferred). */
struct UnkColliContactIter_8001EDFC {
    /* Marks an iterator the out-of-line constructor fn_8001EDFC initializes. */
    struct Unset {};

    UnkColliContactIter_8001EDFC() : m_node(0) {}
    UnkColliContactIter_8001EDFC(Unset) {}

    UnkColliContactNode_800587A0 *m_node;
};

/* A player's set of switchable blocks (class name unknown). */
class UnkBlockSet_8003AAB0;

extern "C" {
/* The owner's state (behaviour inferred from their bodies): whether its parameters changed (the
 * first always answers false), the acknowledgement of a change, whether it was removed or is out
 * of range, and the registration and its release. */
bool fn_800C9D0C(UnkPlacement_800C9CD0 *owner);
bool fn_800C9D14(UnkPlacement_800C9CD0 *owner);
void fn_800C9CF8(UnkPlacement_800C9CD0 *owner);
bool fn_800C9A64(UnkPlacement_800C9CD0 *owner);
bool fn_800C996C(UnkPlacement_800C9CD0 *owner);
void fn_800C9CD0(UnkPlacement_800C9CD0 *owner, Task *task);
void fn_800C9BE8(UnkPlacement_800C9CD0 *owner);

/* The collider manager, a sphere collider at position, and the removal of a collider (behaviour
 * inferred; the meanings of the integer arguments are unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position,
                                              u32 flags, int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
/* The character info a collider belongs to (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800C4568(void *manager,
                                      const boost::weak_ptr<CharaColliBase> &collider);

/* The contact list: its first and end nodes, the iterator constructor and post-increment, and the
 * collider of a node (behaviour from their bodies). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
void fn_8001EDFC(UnkColliContactIter_8001EDFC *it, UnkColliContactNode_800587A0 *node);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node);

/* Sets block id of the block set to state (1 on, 2 off; behaviour inferred). */
void fn_8003AAB0(UnkBlockSet_8003AAB0 *blocks, int id, int state);
}

/* The empty character info (an .sbss shared_ptr; meaning inferred from fn_800C4568). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;
/* Each player's block set, by player index (meaning inferred). */
extern UnkBlockSet_8003AAB0 *lbl_8051CE88[4];

class BlockChange : public Task {
  public:
    BlockChange(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~BlockChange();
    virtual void Tick(float dt);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position and parameters (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
        fn_800C9F14(owner, &m_position);
        fn_800C9DBC(owner, &m_type, 0x1C);
    }

    /* Switches the blocks for every player touching the collider (name inferred). */
    void Behave(float dt);

    static const char *s_taskName;

    /* Member names are inferred; the parameters' from the parameter table. */
    /* 0x28 */ Vec3f m_position;
    /* 0x34 */ UnkPlacement_800C9CD0 *m_owner;
    /* 0x38 */ s32 m_type;
    /* 0x3C */ s32 m_on;
    /* 0x40 */ float m_radius;
    /* 0x44 */ s32 m_blocks[4];
    /* 0x54 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x5C */ u32 pad_0x5C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(BlockChange, 0x60);

const char *BlockChange::s_taskName = "BlockChange";

extern "C" void fn_800ED9E4(void) {}

extern "C" void fn_800ED9E0(void) {}

void BlockChange::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new BlockChange(fn_801EB304(fn_8001C4F8(), 11), owner);
}

BlockChange::BlockChange(Task *parent, UnkPlacement_800C9CD0 *owner) : Task(parent) {
    m_name = s_taskName;
    if (owner != 0) {
        m_owner = owner;
        fn_800C9CD0(m_owner, this);
    }
    LoadFromOwner(m_owner);
    m_collider = fn_800C7EA8(fn_80021B08(), &m_position, 0xC0100, 10, 0x35, lbl_805EE698,
                             &lbl_805E07A0, m_radius);
}

BlockChange::~BlockChange() {
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
    if (m_owner != 0) {
        fn_800C9BE8(m_owner);
        m_owner = 0;
    }
}

void BlockChange::Behave(float dt) {
    UnkColliContacts_8001EE04 *contacts =
        reinterpret_cast<UnkColliContacts_8001EE04 *>(reinterpret_cast<u8 *>(m_collider.get()) + 0xC);
    UnkColliContactIter_8001EDFC it;
    {
        UnkColliContactIter_8001EDFC first((UnkColliContactIter_8001EDFC::Unset()));
        fn_8001EDFC(&first, fn_8001EE04(contacts));
        it = first;
    }
    for (; it.m_node != fn_8001EDEC(contacts); fn_80027374(&it, 0)) {
        boost::weak_ptr<CharaColliBase> collider = fn_800587A0(it.m_node);
        if (boost::shared_ptr<CharaColliBase> hit = collider.lock()) {
            boost::weak_ptr<CharInfo> owner = fn_800C4568(fn_80021B08(), hit);
            if (boost::shared_ptr<CharInfo> target = owner.lock()) {
                if (target.get() != 0 && target->field_0x5C == 1) {
                    Player::Player *player = dynamic_cast<Player::Player *>(target.get());
                    if (player != 0 && player->field_0x54 >= 0) {
                        UnkBlockSet_8003AAB0 *blocks = lbl_8051CE88[player->field_0x54];
                        if (blocks != 0) {
                            for (int i = 0; i < 4; i++) {
                                if (m_blocks[i] >= 0) {
                                    if (m_on != 0) {
                                        fn_8003AAB0(blocks, m_blocks[i], 1);
                                    } else {
                                        fn_8003AAB0(blocks, m_blocks[i], 2);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void BlockChange::Tick(float dt) {
    if (m_owner != 0) {
        if (fn_800C9D0C(m_owner) && fn_800C9D14(m_owner)) {
            fn_800C9F14(m_owner, &m_position);
            LoadFromOwner(m_owner);
            m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&m_position));
            m_collider->SetSize(m_radius, 0.0f, 0.0f, -2);
            fn_800C9CF8(m_owner);
        }
        if (fn_800C9A64(m_owner)) {
            m_flags |= 1;
            return;
        }
        if (fn_800C996C(m_owner)) {
            m_flags |= 1;
            return;
        }
    }
    Behave(dt);
}

GadgetParamDesc lbl_80524A94[] = {
    {1, "Type", 0, 0, 0, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "On/Off", 0, 0, 1, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "Radius", 0, 0, 0, 10.0f, 1.0f, 10000.0f, fn_800CBC20, 0},
    {1, "BLOCK", -1, -1, 99, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "BLOCK", -1, -1, 99, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "BLOCK", -1, -1, 99, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {1, "BLOCK", -1, -1, 99, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80524BD4 = {
    "BlockChange", fn_800ED9E4, fn_800ED9E0, BlockChange::Instantiate, 0, 0, 0x59, 1, 0, 10,
    lbl_80524A94,
};
