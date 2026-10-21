/* SetCharacter (class @unnamed@SetCharacter_cpp@::SetCharacterTask from RTTI, which also gives the
 * file name): the SET_CHARACTOR gadget. Once the game reaches state 3 it places the character
 * chosen by its "Characotr" (sic) parameter at the owner's position and rotation, unless that
 * character is the player's own one in state 2. The character is looked up, or created when it
 * does not exist yet; an NPC gets the "ContactRange(m)" parameter (times 10) and a ShowCommand.
 * The gadget then finishes. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Gadget.h"
#include "Game/GadgetClassDesc.h"
#include "Game/Mission.h"
#include "Game/Npc.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The cursor handed to a parameter's label callback (layout from fn_800CBC20 and fn_801199EC;
 * names inferred). */
class UnkLabelQuery_800CBC20 {
  public:
    /* 0x0 */ const GadgetParamDesc *params;
    /* 0x4 */ s32 index; /* the parameter being shown */
    /* 0x8 */ s32 value; /* its current value */
};

class UnkPlayerManager_8006D6A8;
class UnkCharManager_8006F754;

/* The object fn_80074470 returns (class unknown); its word at 0x8 is the game's state (name
 * inferred). */
struct UnkGameMode_80074470 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ s32 m_state;
};

/* The game state block in .bss (layout of the byte this unit reads only). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xB1];
    /* 0xB1 */ u8 field_0xB1; /* meaning unknown */
    /* 0xB2 */ u8 field_0xB2[0x150 - 0xB2];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
/* The name of a character id, and the player manager index of a character id (behaviour
 * inferred). */
const char *fn_802285D4(int character);
int fn_802285A0(int character);
/* Returns the player manager and its entry of an index (behaviour inferred). */
UnkPlayerManager_8006D6A8 *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(UnkPlayerManager_8006D6A8 *manager, int index);
/* Returns the character manager and the character of an id in it; fn_80076218 creates the
 * character of an id (behaviour inferred). */
UnkCharManager_8006F754 *fn_8006F754(void);
boost::weak_ptr<CharInfo> fn_8007488C(UnkCharManager_8006F754 *manager, int character);
boost::weak_ptr<CharInfo> fn_80076218(int character);
/* Places the character at position and rotation (behaviour inferred). */
void fn_800B8000(const boost::weak_ptr<CharInfo> &character, const Vec3f *position,
                 const Vec3f *rotation);
/* NPC helpers (behaviour inferred): fn_80073708 tests bit 0 of the NPC's flags, fn_80073770 and
 * fn_80073838 set two of its ranges. */
bool fn_80073708(Player::Npc::Npc *npc);
void fn_80073770(Player::Npc::Npc *npc, float range);
void fn_80073838(Player::Npc::Npc *npc, float range);
/* Returns the game mode (behaviour inferred). */
UnkGameMode_80074470 *fn_80074470(void);
/* Marks the owner done (behaviour inferred). */
void fn_800C9AD4(UnkPlacement_800C9CD0 *owner);

extern UnkGameState_8057D6A0 lbl_8057D6A0;
}

/* The two parameters the owner supplies (names from the parameter table). */
struct SetCharacterParams {
    /* 0x0 */ s32 character;
    /* 0x4 */ float contactRange; /* metres */
};
SIZE_ASSERT(SetCharacterParams, 0x8);

namespace {

class SetCharacterTask : public Gadget {
  public:
    SetCharacterTask(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {}
    virtual ~SetCharacterTask();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Finds or creates the owner's character and places it (name inferred). */
    void SpawnCharacter(UnkPlacement_800C9CD0 *owner);
    /* Places the character at the owner's position and sets it up when it is an NPC (name
     * inferred). */
    void PlaceCharacter(boost::weak_ptr<CharInfo> &character, UnkPlacement_800C9CD0 *owner);
};
SIZE_ASSERT(SetCharacterTask, 0x30);

SetCharacterTask::~SetCharacterTask() {
    if (m_owner != 0) {
        fn_800C9AD4(m_owner);
    }
}

void SetCharacterTask::Behave(float dt) {
    if (fn_80074470()->m_state == 3) {
        SpawnCharacter(m_owner);
        m_flags |= 1;
    }
}

void SetCharacterTask::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {}

void SetCharacterTask::SpawnCharacter(UnkPlacement_800C9CD0 *owner) {
    SetCharacterParams params;

    fn_800C9DBC(owner, &params, sizeof(params));
    int character = params.character;
    int index = fn_802285A0(character);

    {
        boost::weak_ptr<Mission::Mission> player = fn_800740E4(fn_8006D6A8(), index);
        if (!player.expired() && player.lock()->m_state == 2) {
            return;
        }
    }

    boost::weak_ptr<CharInfo> charInfo = fn_8007488C(fn_8006F754(), character);
    if (charInfo.expired()) {
        charInfo = fn_80076218(character);
    }
    if (!charInfo.expired()) {
        PlaceCharacter(charInfo, owner);
    }
}

void SetCharacterTask::PlaceCharacter(boost::weak_ptr<CharInfo> &character,
                                      UnkPlacement_800C9CD0 *owner) {
    boost::shared_ptr<Player::Npc::Npc> npc =
        boost::dynamic_pointer_cast<Player::Npc::Npc>(character.lock());
    if (npc.px == 0) {
        return;
    }
    if (!fn_80073708(npc.px)) {
        SetCharacterParams params;
        fn_800C9DBC(owner, &params, sizeof(params));
        Vec3f position;
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        fn_800C9F14(owner, &position);
        fn_800C9ED4(owner, &rotation);
        fn_800B8000(character, &position, &rotation);
        fn_80073770(npc.px, 10.0f * params.contactRange);
        if (lbl_8057D6A0.field_0xB1) {
            fn_80073838(npc.px, 10.0f * params.contactRange);
        }
        npc->PushCommand(new Player::Npc::ShowCommand(0, 4));
    }
}

} // namespace

extern "C" void fn_80119A88(void) {}

extern "C" void fn_80119A84(void) {}

namespace {

void SetCharacterTask::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetCharacterTask(fn_801EB304(fn_8001C4F8(), 11), owner);
}

} // namespace

/* The label of the "Characotr" parameter: the character's name (behaviour inferred). */
extern "C" const char *fn_801199EC(UnkLabelQuery_800CBC20 *cursor) {
    if (cursor->index == 0) {
        return fn_802285D4(cursor->value);
    }
    return 0;
}

static GadgetParamDesc lbl_80526FA0[] = {
    {1, "Characotr", 1, 1, 0x13, 0.0f, 0.0f, 0.0f, fn_801199EC, 0},
    {4, "ContactRange(m)", 0, 0, 0, 100.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80527018 = {
    "SET_CHARACTOR", fn_80119A88, fn_80119A84, SetCharacterTask::Instantiate, 0, 0, 0x190, 1, 0, 10,
    lbl_80526FA0,
};
