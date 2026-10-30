/* Player::PlayerShadow (class name from RTTI): Shadow, the player character. fn_800AD298 creates
 * him for the stage set-up (Game/StageInitalizeTask.cpp) and registers him with the characters.
 * His setup (slot 0x30) builds his tasks below a task of his own: the controller, the executer,
 * the motion with the motion files of his kind, the attack, the weapon slot, the effect and the
 * sound, then his aim and his control and behavior factories; the kinds 0x10 to 0x13 start with
 * an embedded rifle or bazooka (Game/EmbeddedWeapon.cpp). His files are loaded with him and
 * unloaded with him (fn_800AD3A8, fn_800AD358). File name from the class. Function and member
 * names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define CHARACTER_FILES_GET_HANDLE_EXTERN
#define PLAYER_OBJECT_MEMBERS
#define PLAYER_PICKUP_SLOT
#define PLAYER_PLAYER_IMPLEMENTATION
#define PLAYER_WEAPON_BASE_GIVE_WEAPON_INLINE
#include "Game/CharacterFiles.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "Game/PlayerFactory.h"
#include "Game/PlayerShadowMotionFiles.h"
#include "Game/PlayerShadowWeapon.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponCannonBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;

struct UnkCharaList_80074194;
struct UnkNpcRecord_8006F8D0;


namespace Player {

/* Controller, Attack and Effect (class names from RTTI): tasks of a player character (as in
 * Game/BossSonic.cpp). Only their constructors are called here. */
class Controller : public Task {
  public:
    Controller(Task *parent, const shared_ptr<PlayerBase> &player);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};
SIZE_ASSERT(Controller, 0x30);

class Attack : public Task {
  public:
    Attack(Task *parent, const shared_ptr<PlayerBase> &player);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};
SIZE_ASSERT(Attack, 0x30);

class Effect : public Task {
  public:
    Effect(Task *parent, const shared_ptr<PlayerBase> &player);

    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
};
SIZE_ASSERT(Effect, 0x30);

} // namespace Player

/* What a player keeps at 0x248 (class unknown; as in Game/BossSonic.cpp). */
struct UnkPlayerPart_80095BD4 {
    UnkPlayerPart_80095BD4(const shared_ptr<PlayerBase> &player);

    /* 0x0 */ void *m_object;
};
SIZE_ASSERT(UnkPlayerPart_80095BD4, 0x4);

/* The embedded weapons' factories (Game/EmbeddedWeapon.cpp). */
namespace EmbeddedGun {
shared_ptr<Weapon::WeaponGunBase> Create(const weak_ptr<CharInfo> &owner,
                                         const weak_ptr<PlayerBase> &player);
}
namespace EmbeddedBazooka {
shared_ptr<Weapon::WeaponCannonBase> Create(const weak_ptr<CharInfo> &owner,
                                            const weak_ptr<PlayerBase> &player);
}

/* The game state (struct unknown): the frame time a character follows is at 0x18 (inferred). */
extern u8 lbl_8057D6A0[];
/* The characters of player 1 and player 2 (as Game/AudioListener.cpp reads them). */
extern s32 lbl_805EE588;
extern s32 lbl_805E4968;


extern "C" {
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* Task makers of Shadow (behaviour inferred from their callers): the layer task, the watch, the
 * aim (Player::Shadow::PlayerTarget), the executer, the motion and the sound
 * (Game/PlayerShadowSound.cpp). */
Task *fn_800B8360(Task *parent, const shared_ptr<Player::Player> &player);
Task *fn_800B8310(Task *parent, const weak_ptr<PlayerBase> &player);
Player::PlayerTargetBase *fn_800B66A0(const shared_ptr<PlayerBase> &player);
void fn_80094088(Task *parent, const shared_ptr<PlayerBase> &player);
void fn_800AF1D4(Task *parent, const shared_ptr<PlayerBase> &player);
void fn_800B10CC(Task *parent, const shared_ptr<PlayerBase> &player,
                 const UnkMotionFiles_800ADE78 *files);
Task *fn_802DB814(Task *parent, const shared_ptr<PlayerBase> &player);
/* Shadow's factories (Game/PlayerShadowBehavior.cpp). */
shared_ptr<Player::Control::Factory> fn_800AEB10(const shared_ptr<PlayerBase> &player);
shared_ptr<Player::Behavior::Factory> fn_800AE330(const shared_ptr<PlayerBase> &player);
/* Hands the controller its control factory and the executer its behavior factory (as in
 * Game/BossSonic.cpp), and switches the control. */
void fn_8009EBC8(Task *controller, const shared_ptr<Player::Control::Factory> &factory);
void fn_800A8078(Player::Behavior::UnkBehaviorExecuter_800A7F14 *behaviors,
                 const shared_ptr<Player::Behavior::Factory> &factory);
void fn_8009EBA4(Task *controller, const Player::Control::InitializeArgs *args);
/* The character's option in slot index (Game/PlayerOption.cpp), and a node of the motion
 * (Game/PlayerMotion.cpp). */
weak_ptr<CharInfo> fn_800AADA4(Player::UnkCharaSlots_800AADA4 *slots, int index);
void *fn_800AA760(Player::PlayerMotion *motion, int index);
/* Sets a bit of the player's state bits (see Game/PlayerBarrier.cpp). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80079334(PlayerBase *player, int arg);
/* The character's file (behaviour inferred). */
s32 fn_80078C4C(PlayerBase *player);
/* The name of a character kind (Game/SetCharacter.cpp). */
const char *fn_802285D4(int kind);
/* The characters and their registration (Game/PlayerManageUnit.cpp). */
UnkCharaList_80074194 *fn_80074194(void);
void fn_800A8F08(UnkCharaList_80074194 *list, const shared_ptr<Player::Player> &player);
UnkNpcRecord_8006F8D0 *fn_8006F8D0(PlayerBase *player);
void fn_800A871C(UnkNpcRecord_8006F8D0 *record, int arg);
/* The CSD resource (Game/VehicleArrow.cpp). */
void fn_8033A938(void);
void fn_8033A8F0(void);
}

/* The slot 0x0C of a character's vtable with the argument it takes here (a view; CharInfo declares
 * it without arguments; see Game/PlayerOption.cpp). */
class UnkCharaSlots_800AD298 {
  public:
    /* 0x00 */ u8 field_0x0[0x38];
    virtual ~UnkCharaSlots_800AD298();
    virtual void vf_0x0C(int index);
};

/* The range set at 0x274 (name inferred). As a call it is not a constant expression, so the
 * static initializer stores it, as in retail (as in Game/BossSonic.cpp). */
inline float Value(float value) {
    return value;
}

static const float s_value274 = Value(5.0f);

namespace Player {

class PlayerShadow : public Player {
  public:
    PlayerShadow(int kind);
    virtual ~PlayerShadow();
    virtual void vf_0x30();

    static shared_ptr<Player> Create(int kind);
};
SIZE_ASSERT(PlayerShadow, 0x29C);

shared_ptr<Player> PlayerShadow::Create(int kind) {
    shared_ptr<Player> shadow = shared_ptr<Player>(new PlayerShadow(kind));
    shadow->m_self = shadow;
    shadow->vf_0x30();
    return shadow;
}

} // namespace Player

extern "C" void fn_800AD3A8(int kind);
extern "C" void fn_800AD358(int kind);

namespace Player {

PlayerShadow::PlayerShadow(int kind) : Player(kind, fn_802285D4(kind)) {
    fn_800AD3A8(kind);
    field_0x274 = s_value274;
    *reinterpret_cast<void **>(field_0x60) = &lbl_8057D6A0[0x18];
    fn_8033A938();
}

PlayerShadow::~PlayerShadow() {
    fn_8033A8F0();
    fn_800AD358(m_kind);
}

} // namespace Player

UnkMotionFiles_800ADE78::UnkMotionFiles_800ADE78() {}

UnkMotionFile_800ADE6C::UnkMotionFile_800ADE6C(s32 file, s32 index) {
    m_file = file;
    m_index = index;
}

namespace Player {

void PlayerShadow::vf_0x30() {
    shared_ptr<PlayerBase> self = m_self.lock();
    shared_ptr<Player> player = boost::dynamic_pointer_cast<Player>(self);
    m_layerTask = fn_800B8360(fn_801EB304(fn_8001C4F8(), 7), player);
    m_rootTask = new Task(fn_801EB304(fn_8001C4F8(), 13));
    m_rootTask->m_name = "Shadow";
    fn_800B8310(m_layerTask, m_self);
    PlayerBase::vf_0x30();
    m_target = fn_800B66A0(self);
    new Controller(m_layerTask, self);
    s32 file = fn_80078C4C(this);
    s32 common = fn_80078C88()->GetHandle(0xC);
    fn_80094088(m_rootTask, self);
    fn_800AF1D4(m_rootTask, self);
    UnkMotionFiles_800ADE78 files;
    files.m_common1 = UnkMotionFile_800ADE6C(common, 0x1B);
    files.m_common2 = UnkMotionFile_800ADE6C(common, 0x1A);
    switch (m_kind) {
    case 0xF:
        files.m_main = UnkMotionFile_800ADE6C(file, 2);
        files.m_set1[0] = UnkMotionFile_800ADE6C(file, 5);
        files.m_set1[1] = UnkMotionFile_800ADE6C(file, 6);
        files.m_set1[2] = UnkMotionFile_800ADE6C(file, 4);
        files.m_set1[3] = UnkMotionFile_800ADE6C(file, 7);
        files.m_set1[4] = UnkMotionFile_800ADE6C(file, 3);
        files.m_set2[1] = UnkMotionFile_800ADE6C(file, 8);
        files.m_set2[2] = UnkMotionFile_800ADE6C(file, 9);
        files.m_set2[3] = UnkMotionFile_800ADE6C(file, 0xA);
        files.m_set2[4] = UnkMotionFile_800ADE6C(file, 0xB);
        break;
    case 0x10:
        files.m_main = UnkMotionFile_800ADE6C(file, 4);
        files.m_set1[1] = UnkMotionFile_800ADE6C(file, 0xA);
        files.m_set3[0] = UnkMotionFile_800ADE6C(file, 6);
        files.m_set3[1] = UnkMotionFile_800ADE6C(file, 7);
        files.m_common2 = UnkMotionFile_800ADE6C(file, 2);
        break;
    case 0x11:
        files.m_main = UnkMotionFile_800ADE6C(file, 4);
        files.m_set1[1] = UnkMotionFile_800ADE6C(file, 0xA);
        files.m_set3[0] = UnkMotionFile_800ADE6C(file, 6);
        files.m_set3[1] = UnkMotionFile_800ADE6C(file, 7);
        files.m_common2 = UnkMotionFile_800ADE6C(file, 2);
        break;
    case 0x12:
        files.m_main = UnkMotionFile_800ADE6C(file, 4);
        files.m_set1[1] = UnkMotionFile_800ADE6C(file, 9);
        files.m_set3[0] = UnkMotionFile_800ADE6C(file, 6);
        files.m_set3[1] = UnkMotionFile_800ADE6C(file, 7);
        files.m_common2 = UnkMotionFile_800ADE6C(file, 2);
        break;
    case 0x13:
        files.m_main = UnkMotionFile_800ADE6C(file, 4);
        files.m_set1[1] = UnkMotionFile_800ADE6C(file, 9);
        files.m_set3[0] = UnkMotionFile_800ADE6C(file, 6);
        files.m_set3[1] = UnkMotionFile_800ADE6C(file, 7);
        files.m_common2 = UnkMotionFile_800ADE6C(file, 2);
        break;
    default:
        files.m_main = UnkMotionFile_800ADE6C(file, 9);
        files.m_set1[0] = UnkMotionFile_800ADE6C(file, 0x10);
        files.m_set1[1] = UnkMotionFile_800ADE6C(file, 0x11);
        files.m_set1[2] = UnkMotionFile_800ADE6C(file, 0xF);
        files.m_set1[3] = UnkMotionFile_800ADE6C(file, 0x12);
        files.m_set1[4] = UnkMotionFile_800ADE6C(file, 0xE);
        files.m_set3[0] = UnkMotionFile_800ADE6C(file, 0xA);
        files.m_set3[1] = UnkMotionFile_800ADE6C(file, 0xB);
        files.m_set2[1] = UnkMotionFile_800ADE6C(file, 0x13);
        files.m_set2[2] = UnkMotionFile_800ADE6C(file, 0x14);
        files.m_set2[3] = UnkMotionFile_800ADE6C(file, 0x15);
        files.m_set2[4] = UnkMotionFile_800ADE6C(file, 0x16);
        break;
    }
    fn_800B10CC(m_rootTask, self, &files);
    new Attack(m_rootTask, self);
    Shadow::ShadowWeapon::Create(m_rootTask, self);
    new Effect(m_rootTask, self);
    fn_802DB814(m_rootTask, self);
    *reinterpret_cast<UnkPlayerPart_80095BD4 **>(field_0x244 + (0x248 - 0x244)) =
        new UnkPlayerPart_80095BD4(self);
    fn_8009EBC8(m_task, fn_800AEB10(self));
    fn_800A8078(m_behaviors, fn_800AE330(self));
    fn_8009EBA4(m_task, &Control::InitializeArgs(0));
    weak_ptr<CharInfo> option = fn_800AADA4(m_charaSlots, 1);
    fn_800AA760(m_motion, 5);
    switch (m_kind) {
    case 0x10:
    case 0x11:
        m_weapon->GiveWeapon(EmbeddedGun::Create(option, m_self), 999);
        fn_80076CA8(m_stateBits, 0x33);
        fn_80079334(this, 1);
        break;
    case 0x12:
    case 0x13:
        m_weapon->GiveWeapon(EmbeddedBazooka::Create(option, m_self), 999);
        fn_80076CA8(m_stateBits, 0x33);
        fn_80079334(this, 1);
        break;
    }
}

} // namespace Player

/* The file index of a character kind, asked of the file manager (name inferred; the game calls
 * fn_80078C88 for it although the index does not depend on the manager). */
inline s32 FileIndex(CharacterFileManager *manager, int kind) {
    return fn_8027A994(kind);
}

extern "C" {

/* Loads Shadow's files: the common one and his kind's (names inferred). */
void fn_800AD3A8(int kind) {
    fn_80078C88()->Load(0xC);
    fn_80078C88()->Load(FileIndex(fn_80078C88(), kind));
}

void fn_800AD358(int kind) {
    fn_80078C88()->Unload(0xC);
    fn_80078C88()->Unload(FileIndex(fn_80078C88(), kind));
}

/* Creates Shadow of a kind as player index and registers him (names inferred). */
void fn_800AD298(int kind, int index) {
    shared_ptr<Player::Player> player = Player::PlayerShadow::Create(kind);
    reinterpret_cast<UnkCharaSlots_800AD298 *>(player.get())->vf_0x0C(index);
    fn_800A871C(fn_8006F8D0(player.get()), index);
    fn_800A8F08(fn_80074194(), player);
    switch (index) {
    case 0:
        lbl_805EE588 = player->field_0x298;
        break;
    case 1:
        lbl_805E4968 = player->field_0x298;
        break;
    }
}

} // extern "C"
