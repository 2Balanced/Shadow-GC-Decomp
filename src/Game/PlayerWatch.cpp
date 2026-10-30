/* The player watch tasks of PlayerWatch.cpp (an anonymous namespace; class names from RTTI): a
 * WaitTask that ends when its player is gone or done (bits 0x46 and 0x47 of its state), and a
 * Watch that unregisters its player from the characters when it is deleted. fn_800B8310 and
 * fn_800B8360 create them (Game/PlayerShadow.cpp, Game/BossSonic.cpp). File name from the RTTI
 * names; other names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;

class UnkCharaList_80074194;

extern "C" {
/* A bit array: test bit. */
bool fn_8006CD78(const u32 *bits, int bit);
/* The task registry, and adding and removing a named task (see Game/NpcNameDisplay.cpp). */
void *fn_80014B90(void);
void fn_8025D144(void *registry, Task *task);
void fn_8025D1C4(void *registry, Task *task, int arg2, const char *name);
/* The characters, and removing a player from them (behaviour inferred). */
UnkCharaList_80074194 *fn_80074194(void);
void fn_800A8E5C(UnkCharaList_80074194 *list, const shared_ptr<Player::Player> *player);
}

namespace {

/* Ends when the player is gone, or when its state has bits 0x46 and 0x47. */
class WaitTask : public Task {
  public:
    WaitTask(Task *parent, const weak_ptr<PlayerBase> &player);
    virtual ~WaitTask();
    virtual void Tick(float dt);

    /* 0x28 */ weak_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(WaitTask, 0x30);

/* Holds the player; removes it from the characters when deleted. */
class Watch : public Task {
  public:
    Watch(Task *parent, const shared_ptr<Player::Player> &player);
    virtual ~Watch();

    /* 0x28 */ shared_ptr<Player::Player> m_player;
};
SIZE_ASSERT(Watch, 0x30);

Watch::Watch(Task *parent, const shared_ptr<Player::Player> &player)
    : Task(parent), m_player(player) {
    m_name = "Player::Watch";
}

Watch::~Watch() {
    if (m_player) {
        fn_800A8E5C(fn_80074194(), &m_player);
    }
}

WaitTask::WaitTask(Task *parent, const weak_ptr<PlayerBase> &player)
    : Task(parent), m_player(player) {
    m_name = "Wait";
    fn_8025D1C4(fn_80014B90(), this, 0, "Player");
}

WaitTask::~WaitTask() {
    fn_8025D144(fn_80014B90(), this);
}

void WaitTask::Tick(float dt) {
    shared_ptr<PlayerBase> player = m_player.lock();
    if (!player) {
        m_flags |= 1;
        return;
    }
    if (fn_8006CD78(player->m_stateBits, 0x46) && fn_8006CD78(player->m_stateBits, 0x47)) {
        m_flags |= 1;
    }
}

} // namespace

extern "C" Task *fn_800B8360(Task *parent, const shared_ptr<Player::Player> &player) {
    return new Watch(parent, player);
}

extern "C" Task *fn_800B8310(Task *parent, const weak_ptr<PlayerBase> &player) {
    return new WaitTask(parent, player);
}
