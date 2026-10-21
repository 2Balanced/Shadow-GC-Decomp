/* Player::Npc::Sound (class name from RTTI): the voice and sound task of a non-player character, a
 * Player::SoundBase that plays only the entries its table marks as present. fn_80305BF0 creates
 * one (the NPC classes pass their tables). The file name, the function names and the table's layout
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/GadgetSound.h"
#include "Game/PlayerSoundBase.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* One sound of a character's table (layout inferred: only the first byte, set when the sound
 * exists, is read here). */
struct UnkSoundEntry_80305C50 {
    /* 0x00 */ u8 m_present;
    /* 0x01 */ u8 field_0x1[0x18 - 0x1];
};
SIZE_ASSERT(UnkSoundEntry_80305C50, 0x18);

/* A character's sound table (struct name unknown): two lists of entries. */
struct UnkSoundTable_80305BF0 {
    /* 0x0 */ UnkSoundEntry_80305C50 *m_sounds;
    /* 0x4 */ UnkSoundEntry_80305C50 *m_voices;
};

extern "C" {
/* SoundBase's players for an entry of table (behaviour inferred). */
UnkSoundHandle_801D3DC8 fn_802DBC8C(Player::SoundBase *sound, UnkSoundTable_80305BF0 *table,
                                    s32 index, s32 arg);
void fn_802DBD1C(Player::SoundBase *sound, UnkSoundTable_80305BF0 *table, s32 index);
void fn_802DBD4C(Player::SoundBase *sound, UnkSoundTable_80305BF0 *table, s32 index, s32 arg);
}

namespace Player {
namespace Npc {

class Sound : public SoundBase {
  public:
    Sound(Task *parent, const boost::shared_ptr<PlayerBase> &player, UnkSoundTable_80305BF0 *table)
        : SoundBase(parent, player), m_table(table) {}
    virtual ~Sound();
    /* Plays sound index of the first list, stops voice index, and plays voice index (names
     * inferred). */
    virtual void PlaySound(s32 index, s32 arg);
    virtual void StopVoice(s32 index);
    virtual UnkSoundHandle_801D3DC8 PlayVoice(s32 index, s32 arg);

    /* 0x38 */ UnkSoundTable_80305BF0 *m_table;
    /* 0x3C */ u32 pad_0x3C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(Sound, 0x40);

Sound::~Sound() {}

void Sound::PlaySound(s32 index, s32 arg) {
    if (m_table->m_sounds[index].m_present) {
        fn_802DBD4C(this, m_table, index, arg);
    }
}

void Sound::StopVoice(s32 index) {
    if (m_table->m_voices[index].m_present) {
        fn_802DBD1C(this, m_table, index);
    }
}

UnkSoundHandle_801D3DC8 Sound::PlayVoice(s32 index, s32 arg) {
    if (m_table->m_voices[index].m_present) {
        return fn_802DBC8C(this, m_table, index, arg);
    }
    return UnkSoundHandle_801D3DC8();
}

} // namespace Npc
} // namespace Player

/* Creates the sound task of a non-player character (name of the function unknown). */
extern "C" Player::Npc::Sound *fn_80305BF0(Task *parent,
                                           const boost::shared_ptr<Player::PlayerBase> &player,
                                           UnkSoundTable_80305BF0 *table) {
    return new Player::Npc::Sound(parent, player, table);
}
