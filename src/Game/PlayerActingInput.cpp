/* @unnamed@PlayerActingInput_cpp@::AccelerateInput (class name from RTTI; base Task; its name is
 * "AccelerateInput"): a task that pushes the player along a direction for a time, as if the pad
 * were held. It blocks the pad's input (IgnoreInputCommand) while it runs and sends an
 * AccelerateInputCommand each frame; it ends when its time runs out or the player lands or leaves
 * the ground (whichever it was not on when it began). Its maker and the function that stops a
 * running one keep their addresses as names: other units call them by those. File name from the
 * RTTI name; function and member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* Declares the player commands' Reject overrides, so IgnoreInputCommand's vtable and destructor stay in
 * Game/PlayerCommands.cpp. */
#define CHARCOMMAND_PLAYER_COMMANDS
#include <string>
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;

extern "C" {
/* A 32-bit flag word (see Game/BKTankController.h). */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
/* Whether a character stands on the ground (behaviour inferred). */
bool fn_800B7AD0(const shared_ptr<CharInfo> &chara);
}

namespace {

class AccelerateInput : public Task {
  public:
    AccelerateInput(Task *parent, const weak_ptr<PlayerBase> &player, const Vec3f &direction,
                    float time);
    virtual ~AccelerateInput();
    virtual void Tick(float dt);

    /* Gives the pad back to the player if it was taken. */
    void Release();

    /* 0x28 */ weak_ptr<PlayerBase> m_player;
    /* 0x30 */ Vec3f m_direction;
    /* 0x3C */ float m_time;
    /* 0x40 */ s32 m_state;
    /* 0x44 */ u32 field_0x44;
    /* 0x48 */ u32 m_bits; /* bit 0: began on the ground, bit 1: the pad is blocked */
    /* 0x4C */ u32 field_0x4C;
};
SIZE_ASSERT(AccelerateInput, 0x50);

AccelerateInput::AccelerateInput(Task *parent, const weak_ptr<PlayerBase> &player,
                                 const Vec3f &direction, float time)
    : Task(parent), m_player(player) {
    fn_800091EC(&m_direction, &direction);
    m_time = time;
    fn_80014E04(&m_bits);
    m_name = "AccelerateInput";
    shared_ptr<PlayerBase> chara = m_player.lock();
    if (chara) {
        chara->PushCommand(new Player::IgnoreInputCommand(2, 1));
        fn_80014E1C(&m_bits, 1);
    } else {
        m_flags |= 1;
    }
    m_state = 0;
}

AccelerateInput::~AccelerateInput() {
    Release();
}

void AccelerateInput::Tick(float dt) {
    bool done = false;
    if (m_player.expired()) {
        m_flags |= 1;
        return;
    }
    shared_ptr<PlayerBase> chara = m_player.lock();
    chara->PushCommand(new AccelerateInputCommand(m_direction, 4));
    switch (m_state) {
    case 0:
        m_state++;
        break;
    case 1:
        if (fn_800B7AD0(chara)) {
            fn_80014E1C(&m_bits, 0);
        }
        m_state++;
    default:
        if (fn_80014E34(&m_bits, 0)) {
            if (!fn_800B7AD0(chara)) {
                done = true;
            }
        } else if (fn_800B7AD0(chara)) {
            done = true;
        }
        break;
    }
    m_time -= **reinterpret_cast<float **>(&chara.get()->field_0x60);
    if (m_time <= 0.0f) {
        done = true;
    }
    if (done) {
        Release();
        m_flags |= 1;
    }
}

} // namespace

namespace Player {

IgnoreInputCommand::IgnoreInputCommand(int arg0, int arg1) : PlayerCommand(0x127, 0) {
    m_kind = arg0;
    m_mode = arg1;
}

} // namespace Player

namespace {

void AccelerateInput::Release() {
    if (fn_80014E34(&m_bits, 1)) {
        shared_ptr<PlayerBase> chara = m_player.lock();
        if (chara) {
            chara->PushCommand(new Player::IgnoreInputCommand(3, 1));
            fn_8006127C(&m_bits, 1);
        }
    }
}

} // namespace

/* Starts pushing player along direction for time (called by the player commands by its
 * address). */
extern "C" Task *fn_800773A4(Task *parent, const weak_ptr<PlayerBase> &player,
                             const Vec3f &direction, float time) {
    return new AccelerateInput(parent, player, direction, time);
}

/* Stops a running AccelerateInput (called by the player's controller by its address). */
extern "C" void fn_80077360(Task *task) {
    AccelerateInput *input = dynamic_cast<AccelerateInput *>(task);
    if (input) {
        input->m_flags |= 1;
    }
}
