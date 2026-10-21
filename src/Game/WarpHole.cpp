/* WarpHole::SetWarpHole (class name from RTTI): the WARP_HOLE gadget, a hole with an effect that a
 * player uses (UseCommand) to warp to the other WARP_HOLE placed with the same id. The warp itself
 * is run by a WarpHole::ManagerTask (class name from RTTI): it takes the player's control
 * (StartExternalControlCommand), fades out, moves the player to the destination, fades back in and
 * returns control. A table of the ids in use keeps a hole from being used while its warp runs. File
 * name inferred from the namespace. Method names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <string.h>

#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharCommands.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The handle fn_8015B8C4 wraps a collider in (class name unknown; not accessed here). */
class GadgetCharCollision;

/* A 32-bit flag word whose constructor clears it (fn_80014E04): fn_80014E1C sets bit, fn_8006127C
 * clears bit and fn_80014E34 tests bit (behaviour inferred from their bodies; name inferred). */
extern "C" {
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
}

struct UnkFlags_80014E04 {
    UnkFlags_80014E04() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

/* The game state block in .bss (layout as in GoalRing; only the address of the member at 0xA8 is
 * used here). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xA8];
    /* 0xA8 */ u8 field_0xA8[0x150 - 0xA8];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

/* The object the first entry of lbl_8051CE88 points to (class name unknown; meanings of the
 * members unknown). */
struct UnkBlockManage_8051CE88 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 field_0x40;
    /* 0x044 */ u8 field_0x44[0x978 - 0x44];
    /* 0x978 */ u8 field_0x978;
};

/* The fader (class name unknown): field_0x14 is 1 once a fade is over (meaning inferred). */
struct UnkFader_80044254 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ s32 field_0x14;
};

/* The objects returned by fn_800BADE8 and fn_80074470 (classes unknown). */
struct UnkObject_800BADE8 {
    /* Sets the word at 0x30 (meaning unknown). */
    void SetField30(s32 value) {
        field_0x30 = value;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ s32 field_0x30;
};

struct UnkObject_80074470 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ s32 field_0x8;
};

extern "C" {
/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a sphere collider for charInfo (behaviour inferred; the meanings of the arguments are
 * unknown). */
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, const Vec3f *offset,
                                              float radius);
/* Wraps collider in a new handle, and advances the handle by dt (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Creates the command receiver of a character info unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Starts effect id at position, and moves an effect (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(const Vec3f *position, int id);
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
/* Marks the gadget finished (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
/* Copies the owner's byte at +0x22 into *value (the warp id), the owner's type, the owner list and
 * the first owner with the given id in it (behaviour inferred). */
void fn_800C9E50(UnkPlacement_800C9CD0 *owner, int *value);
u32 fn_800C9D24(UnkPlacement_800C9CD0 *owner);
void *fn_800C9B84(void);
UnkPlacement_800C9CD0 *fn_800CB560(void *list, int id);
/* Moves character to position, facing along rotation (behaviour inferred). */
void fn_800B8000(const boost::weak_ptr<CharInfo> &character, const Vec3f *position,
                 const Vec3f *rotation);
/* boost::dynamic_pointer_cast<Player::Player>(character), a weak instance of the player code (the
 * trimmed boost header leaves out the casts). */
boost::shared_ptr<Player::Player> fn_8009DC10(const boost::shared_ptr<CharInfo> &character);
/* A test on the warped character (behaviour unknown). */
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> &character);
/* The fader, and its fade out and fade in (behaviour and argument meanings inferred). */
UnkFader_80044254 *fn_80044254(void);
void fn_800446D0(UnkFader_80044254 *fader, int arg1, int arg2, float time);
void fn_800447B8(UnkFader_80044254 *fader, int arg1, float time);
/* Unknown queries and switches around the warp (behaviour unknown). */
void *fn_80117514(void);
bool fn_802E5810(void *object);
UnkObject_800BADE8 *fn_800BADE8(void);
UnkObject_80074470 *fn_80074470(void);
void fn_801E2F28(UnkObject_800BADE8 *object);
void fn_801E2EDC(UnkObject_800BADE8 *object);
void fn_80203970(void *object, int arg);
void fn_8003A52C(UnkBlockManage_8051CE88 *object);
}

extern UnkGameState_8057D6A0 lbl_8057D6A0;
extern UnkBlockManage_8051CE88 *lbl_8051CE88[4];

namespace WarpHole {

/* Which warp ids are in use (name inferred). */
class WarpTable {
  public:
    ~WarpTable() {}

    /* Clear the table (names inferred; the load and unload hooks call one each). */
    void Clear();
    void Reset();

    /* The table (name inferred). */
    /* The entry of id (name inferred). */
    s32 IsUsed(int id) {
        return m_used[id];
    }

    static WarpTable &Instance();
    static WarpTable *Get();

    /* 0x000 */ s32 m_used[0x100];
    /* 0x400 */ u32 field_0x400;
};
SIZE_ASSERT(WarpTable, 0x404);

/* The task that runs a warp (class name from RTTI). */
class ManagerTask : public Task {
  public:
    ManagerTask(Task *parent, const Vec3f &destination,
                const boost::shared_ptr<Player::Player> &player, int id);
    virtual ~ManagerTask();
    virtual void Tick(float dt);

    /* Instantiate a warp task under task layer 11 (names inferred). */
    static void Instantiate(const Vec3f &destination, const boost::shared_ptr<Player::Player> &player,
                       int id);
    static void Instantiate(const Vec3f &destination, const boost::shared_ptr<Player::Player> &player);

    static const char *s_taskName;

    /* 0x28 */ Vec3f m_destination;
    /* 0x34 */ boost::weak_ptr<Player::Player> m_player;
    /* 0x3C */ s32 m_id;
    /* 0x40 */ s32 m_counter;
    /* 0x44 */ s32 m_state; /* 0 start, 1 fading out, 2 moved, 3 fading in, 5 done (inferred) */
    /* 0x48 */ UnkFlags_80014E04 m_warpFlags; /* bit 0 id taken, bit 1 control given back (inferred) */
    /* 0x4C */ u32 pad_0x4C;
};
SIZE_ASSERT(ManagerTask, 0x50);

class SetWarpHole : public Gadget {
  public:
    SetWarpHole(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetWarpHole();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the hole under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Lets the effect wind down, and moves it to the hole (names inferred). */
    void StopEffect();
    void UpdateEffect();
    /* Copies the owner's position, rotation and id, and resets the state (name inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x40 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x48 */ s32 m_state; /* 0 new, 3 ready, 4 waiting for the id, 5 done (inferred) */
    /* 0x4C */ Vec3f field_0x4C;
    /* 0x58 */ boost::weak_ptr<CharInfo> field_0x58;
    /* 0x60 */ int m_id;
    /* 0x64 */ u32 pad_0x64;
};
SIZE_ASSERT(SetWarpHole, 0x68);

/* What the search for the destination fills in (name inferred). */
struct WarpSearch {
    WarpSearch() : m_owner(0), m_found(false) {}

    /* 0x00 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x08 */ UnkPlacement_800C9CD0 *m_owner;
    /* 0x0C */ Vec3f m_position;
    /* 0x18 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x20 */ bool m_found;
};
SIZE_ASSERT(WarpSearch, 0x24);

/* The id of WARP_HOLE gadgets (name inferred). */
const u16 kWarpHoleType = 0x1F;

/* Finds the other warp hole with the search owner's id (name inferred). */
static void FindDestination(boost::shared_ptr<CharInfo> character, void *user) {
    WarpSearch *search = static_cast<WarpSearch *>(user);

    if (search != 0 && search->m_owner != 0) {
        int id;
        fn_800C9E50(search->m_owner, &id);
        for (UnkPlacement_800C9CD0 *owner = fn_800CB560(fn_800C9B84(), id); owner != 0;
             owner = *(UnkPlacement_800C9CD0 **)((u8 *)owner + 0x8)) {
            if (owner != search->m_owner && (u16)fn_800C9D24(owner) == kWarpHoleType) {
                fn_800C9F14(owner, &search->m_position);
                search->m_character = character;
                search->m_found = true;
                return;
            }
        }
    }
}

const char *SetWarpHole::s_taskName = "SetWarpHole";

SetWarpHole::SetWarpHole(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x1B, 4, "SetWarpHole");
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&SetWarpHole::OnCommand, this, _1)));
    LoadFromOwner(m_owner);
    m_effect = fn_8015D794(&m_chara->m_position, 0xE1);
    {
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C7DEC(fn_80021B08(), m_chara, 0, 10, 0, &lbl_805E07A0, 20.0f);
        m_handle = fn_8015B8C4(collider);
    }
    UpdateEffect();
}

SetWarpHole::~SetWarpHole() {
    StopEffect();
}

void SetWarpHole::Behave(float dt) {
    fn_8015BE9C(m_handle.get(), dt);

    switch (m_state) {
    case 0:
        m_state = 3;
        break;
    case 3:
        break;
    case 4:
        if (WarpTable::Get()->IsUsed(m_id) == 0) {
            m_state = 3;
        }
        break;
    case 5:
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void SetWarpHole::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateEffect();
}

void SetWarpHole::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E50(owner, &m_id);
    m_state = 0;
    fn_8040CE60(&field_0x4C);
}

void SetWarpHole::UpdateEffect() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        fn_80425E4C(effect.get(), &m_chara->m_position);
    }
}

void SetWarpHole::StopEffect() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        effect->Retire();
    }
}

void SetWarpHole::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0xD: /* UseCommand */
        if (m_state == 3) {
            if (boost::shared_ptr<CharInfo> user = static_cast<UseCommand &>(command).m_user.lock()) {
                WarpSearch search;
                search.m_chara = m_chara;
                search.m_owner = m_owner;
                FindDestination(user, &search);
                if (search.m_found) {
                    boost::shared_ptr<Player::Player> player = fn_8009DC10(user);
                    if (player.get() != 0) {
                        ManagerTask::Instantiate(search.m_position, player, m_id);
                    }
                }
            }
        }
        break;
    }
}

void WarpTable::Reset() {
    memset(this, 0, sizeof(m_used));
}

void WarpTable::Clear() {
    memset(this, 0, sizeof(m_used));
}

const char *ManagerTask::s_taskName = "ManagerTask";

ManagerTask::ManagerTask(Task *parent, const Vec3f &destination,
                         const boost::shared_ptr<Player::Player> &player, int id)
    : Task(parent) {
    m_name = s_taskName;
    AssignVector(m_destination, destination);
    m_player = player;
    m_id = id;
    m_state = 0;
    if (m_id >= 0) {
        WarpTable::Get()->m_used[m_id] = 1;
        fn_80014E1C(&m_warpFlags.m_bits, 0);
    }
    player->PushCommand(new ClrVelocityCommand(4));
    player->PushCommand(new Player::StartExternalControlCommand(0, 4));
    fn_80203970(&lbl_8057D6A0.field_0xA8, 1);
    UnkObject_800BADE8 *object = fn_800BADE8();
    fn_801E2F28(object);
    fn_801E2EDC(object);
    object->field_0x30 = 0;
    fn_8006127C(&m_warpFlags.m_bits, 1);
}

ManagerTask::~ManagerTask() {
    if (fn_80014E34(&m_warpFlags.m_bits, 0)) {
        WarpTable::Get()->m_used[m_id] = 0;
    }
    if (fn_80074470()->field_0x8 != 6 && fn_802E5810(fn_80117514())) {
        fn_80203970(&lbl_8057D6A0.field_0xA8, 0);
    }
    fn_800BADE8()->SetField30(1);
}

void ManagerTask::Tick(float dt) {
    if (!fn_802E5810(fn_80117514())) {
        fn_800BADE8()->SetField30(1);
    }

    switch (m_state) {
    case 0:
        fn_800446D0(fn_80044254(), 1, 1, 1.0f);
        m_state = 1;
        fn_801D560C(fn_80053164(), 0xE014, 0, 0);
        break;
    case 1:
        if (fn_80044254()->field_0x14 == 1) {
            boost::shared_ptr<Player::Player> player = m_player.lock();
            if (player.get() != 0) {
                fn_800B8000(boost::weak_ptr<CharInfo>(m_player), &m_destination, &player->m_rotation);
                UnkBlockManage_8051CE88 *manage = lbl_8051CE88[0];
                if (manage != 0) {
                    manage->field_0x40 = 2;
                }
            }
            m_state = 2;
            m_counter = 0;
        }
        break;
    case 2:
        if (m_counter == 1) {
            UnkBlockManage_8051CE88 *manage = lbl_8051CE88[0];
            if (manage != 0) {
                fn_8003A52C(manage);
                manage->field_0x40 = 2;
            }
            m_state = 3;
        }
        m_counter++;
        break;
    case 3: {
        boost::shared_ptr<Player::Player> player = m_player.lock();
        UnkBlockManage_8051CE88 *manage = lbl_8051CE88[0];
        if (manage != 0 && !(manage->field_0x978 | (manage->field_0x40 != 0))) {
            if (player.get() != 0) {
                player->PushCommand(new Player::EndExternalControlCommand(4));
            }
            fn_800447B8(fn_80044254(), 1, 0.0f);
            fn_801D560C(fn_80053164(), 0xE015, 0, 0);
            m_state = 5;
            fn_80014E1C(&m_warpFlags.m_bits, 1);
        } else if (player.get() != 0) {
            fn_800B8000(boost::weak_ptr<CharInfo>(m_player), &m_destination, &player->m_rotation);
        }
        break;
    }
    case 5:
        if (!fn_80014E34(&m_warpFlags.m_bits, 0)) {
            m_flags |= 1;
        }
        break;
    }

    if (fn_80014E34(&m_warpFlags.m_bits, 1) && fn_80014E34(&m_warpFlags.m_bits, 0) && !m_player.expired()) {
        if (fn_800B7AD0(m_player.lock())) {
            m_flags |= 1;
        }
    }
}

void ManagerTask::Instantiate(const Vec3f &destination, const boost::shared_ptr<Player::Player> &player,
                         int id) {
    new ManagerTask(fn_801EB304(fn_8001C4F8(), 11), destination, player, id);
}

void ManagerTask::Instantiate(const Vec3f &destination, const boost::shared_ptr<Player::Player> &player) {
    Instantiate(destination, player, -1);
}

} // namespace WarpHole

extern "C" void fn_80129594(void) {
    WarpHole::WarpTable::Get()->Reset();
}

namespace WarpHole {

WarpTable &WarpTable::Instance() {
    static WarpTable s_table;
    return s_table;
}

/* The table, cached (name inferred). */
static WarpTable *s_cachedTable;

WarpTable *WarpTable::Get() {
    if (s_cachedTable == 0) {
        s_cachedTable = &Instance();
    }
    return s_cachedTable;
}

} // namespace WarpHole

extern "C" void fn_801294AC(void) {
    WarpHole::WarpTable::Get()->Clear();
}

void WarpHole::SetWarpHole::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetWarpHole(fn_801EB304(fn_8001C4F8(), 11), owner);
}

GadgetClassDesc lbl_805280DC = {
    "WARP_HOLE", fn_80129594, fn_801294AC, WarpHole::SetWarpHole::Instantiate, 0, 0, 0x1F, 1, 0, 10, 0,
};
