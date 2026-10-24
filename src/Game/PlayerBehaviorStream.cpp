/* Player::Behavior::Stream (class name from RTTI): behavior 0x18 of Shadow, being carried by a
 * stream (a wind or water current). The player plays the stream motion with state bit 0xF set and
 * steers a little along the input; StreamCommand (0x122) with m_on cleared lets it fall (behavior
 * 2). File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/CharCommand.h"
#include "Game/GadgetCharCommands.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8008F814 {
    /* 0x000 */ u8 field_0x0[0xB8];
    /* 0x0B8 */ Vec3f field_0xB8; /* the input's direction (as in Game/PlayerBehaviorEdgeHang.cpp) */
};

extern "C" {
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8008F814 *>(player)

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

class Stream : public Behavior {
  public:
    Stream(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Stream();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    static const char *s_name;
};
SIZE_ASSERT(Stream, 0x1C);

const char *Stream::s_name = "Stream";

Stream::Stream(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0x18) {}

Stream::~Stream() {}

bool Stream::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void Stream::Enter(int prevId, const InitializeArgs &args) {
    m_player.get()->m_motion->vf_0x10(0x27, 0.0f);
    fn_80076CA8(m_player.get()->m_stateBits, 0xF);
    m_player.get()->vf_0x48();
}

void Stream::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0xF);
}

void Stream::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x122:
        if (static_cast<StreamCommand *>(command)->m_on == 0) {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        }
        command->Flags() |= 2;
        break;
    }
}

void Stream::Update(float dt) {
    AddVector(m_player->field_0x24,
              fn_800091A4(&FieldsOf(m_player.operator->())->field_0xB8, 220.0f));
    fn_80079720(m_player.get());
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_8009728C(&m_player, 0);
}

Behavior *Stream::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Stream(player);
}

void Stream::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Stream by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0DF8 = {
    Player::Behavior::Stream::Create, Player::Behavior::Stream::Release, 0};
