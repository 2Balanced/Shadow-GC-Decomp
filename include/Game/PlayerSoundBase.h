#ifndef GAME_PLAYERSOUNDBASE_H
#define GAME_PLAYERSOUNDBASE_H

#include "Game/GadgetSound.h"
#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* One sound of a character's sound table (struct name unknown; see Game/PlayerSoundBase.cpp). */
struct UnkSoundEntry_80305C50;

namespace Player {

/* SoundBase (class name from RTTI; vtable at 0x80558D6C): the task that plays a character's
 * voices and sounds. The constructor (0x802DBDF0) keeps the player and registers itself at the
 * player's 0x240; the destructor (0x802DBD7C) clears that. Player::Shadow::Sound
 * (Game/PlayerShadowSound.cpp) and Player::Npc::Sound (Game/PlayerNpcSound.cpp) derive from it
 * (Game/PlayerSoundBase.cpp). Member names are inferred. */
class SoundBase : public Task {
  public:
    SoundBase(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~SoundBase();
    /* Plays sound index of the character's table, plays voice index, and plays voice index
     * returning its handle (names as in Game/PlayerNpcSound.cpp; meanings inferred). */
    virtual void PlaySound(s32 index, s32 arg) = 0;
    virtual void StopVoice(s32 index) = 0;
    virtual UnkSoundHandle_801D3DC8 PlayVoice(s32 index, s32 arg) = 0;

    /* Plays entry, offset by arg (0x802DBBE0; name inferred). */
    void PlayEntry(const UnkSoundEntry_80305C50 *entry, int arg);

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x30 */ s32 field_0x30; /* counts the plays of entries with variations */
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(SoundBase, 0x38);

} // namespace Player

#endif
