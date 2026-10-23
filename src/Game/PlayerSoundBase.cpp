/* Player::SoundBase (class name from RTTI; declared in Game/PlayerSoundBase.h): the task that plays
 * a character's voices and sounds from its sound table. The character keeps it at its 0x240 while
 * it exists. The functions the derived classes call keep their fn_ names (C functions taking the
 * task). File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/GadgetSound.h"
#include "Game/Player.h"
#include "Game/PlayerSoundBase.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* One sound of a character's table (struct name unknown; as in Game/PlayerNpcSound.cpp): the
 * sound plays when m_present is set; m_id (if 0 or more) is the sound, which m_variations (if
 * more than 0) varies with each play, and m_voice (if 0 or more) a voice played with it (member
 * names inferred). */
struct UnkSoundEntry_80305C50 {
    /* 0x00 */ u8 m_present;
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ s32 m_variations;
    /* 0x10 */ s32 m_id;
    /* 0x14 */ s32 m_voice;
};
SIZE_ASSERT(UnkSoundEntry_80305C50, 0x18);

/* A character's sound table (struct name unknown; as in Game/PlayerNpcSound.cpp): two lists of
 * entries. */
struct UnkSoundTable_80305BF0 {
    /* 0x0 */ const UnkSoundEntry_80305C50 *m_sounds;
    /* 0x4 */ const UnkSoundEntry_80305C50 *m_voices;
};

extern "C" {
/* The sound manager and its players (see Game/GadgetUtil.h; behaviour inferred). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
void fn_801D51A0(void *manager, int id, int arg2, int arg3);
UnkSoundHandle_801D3DC8 fn_801D3EF0(u16 soundId, int arg);

/* The players the derived classes call (see the file comment; names of the functions unknown). */
UnkSoundHandle_801D3DC8 fn_802DBC8C(Player::SoundBase *sound, const UnkSoundTable_80305BF0 *table,
                                    s32 index, s32 arg);
void fn_802DBD1C(Player::SoundBase *sound, const UnkSoundTable_80305BF0 *table, s32 index);
void fn_802DBD4C(Player::SoundBase *sound, const UnkSoundTable_80305BF0 *table, s32 index, s32 arg);
}

namespace Player {

SoundBase::SoundBase(Task *parent, const boost::shared_ptr<PlayerBase> &player)
    : Task(parent), m_player(player) {
    m_name = "Sound";
    m_player->m_sound = this;
    field_0x30 = 0;
}

SoundBase::~SoundBase() {
    m_player->m_sound = 0;
}

} // namespace Player

/* Plays sound index of table, offset by arg. */
extern "C" void fn_802DBD4C(Player::SoundBase *sound, const UnkSoundTable_80305BF0 *table,
                            s32 index, s32 arg) {
    sound->PlayEntry(&table->m_sounds[index], arg);
}

/* Plays voice index of table. */
extern "C" void fn_802DBD1C(Player::SoundBase *sound, const UnkSoundTable_80305BF0 *table,
                            s32 index) {
    sound->PlayEntry(&table->m_voices[index], 0);
}

/* Plays voice index of table on its own, and returns its handle (an empty one if the voice has
 * no sound). */
extern "C" UnkSoundHandle_801D3DC8 fn_802DBC8C(Player::SoundBase *sound,
                                               const UnkSoundTable_80305BF0 *table, s32 index,
                                               s32 arg) {
    const UnkSoundEntry_80305C50 *entry = &table->m_voices[index];
    if (entry->m_present && entry->m_id >= 0) {
        s32 variations = entry->m_variations;
        u16 id = entry->m_id;
        if (variations > 0) {
            id += sound->field_0x30++ & variations;
        }
        return fn_801D3EF0(id, arg);
    }
    return UnkSoundHandle_801D3DC8();
}

namespace Player {

void SoundBase::PlayEntry(const UnkSoundEntry_80305C50 *entry, int arg) {
    if (entry != 0 && entry->m_present) {
        if (entry->m_id >= 0) {
            u16 id = entry->m_id + arg;
            s32 variations = entry->m_variations;
            if (variations > 0) {
                id += field_0x30++ % variations;
            }

            fn_801D560C(fn_80053164(), id, 0, 0);
        }
        if (entry->m_voice >= 0) {
            fn_801D51A0(fn_80053164(), entry->m_voice, -1, -1);
        }
    }
}

} // namespace Player
