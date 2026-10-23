/* Player::Shadow::Sound (class name from RTTI): Shadow's voice and sound task, a Player::SoundBase
 * with Shadow's sound table. A few of its voices vary: voice 15 depends on the character's state
 * bit 0x24 and alternates with a second voice, voice 16 alternates between two voices and voice 17
 * depends on the stage; sound 1 has the stick weapon play its own swing sound and rumble. The unit
 * also holds the sound tables of the other characters (Player::Npc::Sound,
 * Game/PlayerNpcSound.cpp) and the functions that create their sound tasks; the characters are
 * named after the classes that call them. File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/GadgetSound.h"
#include "Game/Player.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Task.h"
#include "Game/Vibration.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponStickBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* One sound of a character's table (struct name unknown; see Game/PlayerSoundBase.cpp). */
struct UnkSoundEntry_80305C50 {
    /* 0x00 */ u8 m_present;
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ s32 m_variations;
    /* 0x10 */ s32 m_id;
    /* 0x14 */ s32 m_voice;
};
SIZE_ASSERT(UnkSoundEntry_80305C50, 0x18);

/* A character's sound table (struct name unknown; as in Game/PlayerNpcSound.cpp). */
struct UnkSoundTable_80305BF0 {
    /* 0x0 */ const UnkSoundEntry_80305C50 *m_sounds;
    /* 0x4 */ const UnkSoundEntry_80305C50 *m_voices;
};

/* The game state (struct name unknown; see Game/TGi.h): the word at 0xA8 is the stage's id
 * (meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xA8];
    /* 0xA8 */ s32 field_0xA8;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* Creates the sound task of a non-player character (Game/PlayerNpcSound.cpp). */
Task *fn_80305BF0(Task *parent, const boost::shared_ptr<Player::PlayerBase> &player,
                  const UnkSoundTable_80305BF0 *table);
/* SoundBase's players (Game/PlayerSoundBase.cpp). */
UnkSoundHandle_801D3DC8 fn_802DBC8C(Player::SoundBase *sound, const UnkSoundTable_80305BF0 *table,
                                    s32 index, s32 arg);
void fn_802DBD1C(Player::SoundBase *sound, const UnkSoundTable_80305BF0 *table, s32 index);
void fn_802DBD4C(Player::SoundBase *sound, const UnkSoundTable_80305BF0 *table, s32 index, s32 arg);
/* The sound manager and its players (see Game/GadgetUtil.h; behaviour inferred). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
void fn_801D51A0(void *manager, int id, int arg2, int arg3);
/* Tests bit of a bit array (see Game/Player.h). */
bool fn_8006CD78(u32 *bits, int bit);
/* Starts a rumble pattern for player (see Game/DiablonPartsBody.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);
}

/* The characters' sound tables (names inferred). */
static const UnkSoundEntry_80305C50 s_vectorSounds[11] = {
    {1, 1, 0, 0, 0x402C, -1},
    {0},
    {1, 1, 0, 2, 0x402E, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_vectorVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x402A, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x402B, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x402D, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x4030, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_amySounds[11] = {
    {1, 1, 0, 0, 0x4017, -1},
    {0},
    {1, 1, 0, 2, 0x4019, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_amyVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x4015, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x4016, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x4018, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x401B, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_beeSounds[11] = {
    {1, 1, 0, 0, 0x4033, -1},
    {0},
    {1, 1, 0, 2, 0x4035, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_beeVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x4031, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x4032, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x4034, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x4037, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_doomsEyeSounds[11] = {
    {1, 1, 0, 0, 0x5002, -1},
    {0},
    {1, 1, 0, 0, 0x5004, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_doomsEyeVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x5000, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x5001, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x5003, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x5006, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_eggMonitorSounds[11] = {
    {1, 1, 0, 0, 0x5009, -1},
    {0},
    {1, 1, 0, 2, 0x500B, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_eggMonitorVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x5007, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x5008, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x500A, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x500D, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_espioSounds[11] = {
    {1, 1, 0, 0, 0x403A, -1},
    {0},
    {1, 1, 0, 2, 0x403C, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_espioVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x4038, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x4039, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x403B, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x403E, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_knucklesSounds[11] = {
    {1, 1, 0, 0, 0x4010, -1},
    {0},
    {1, 1, 0, 2, 0x4012, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_knucklesVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x400E, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x400F, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x4011, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x4014, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_mariaSounds[11] = {
    {1, 1, 0, 0, 0x4041, -1},
    {0},
    {1, 1, 0, 2, 0x4043, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_mariaVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x403F, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x4040, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x4042, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x4045, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_omegaSounds[11] = {
    {1, 1, 0, 0, 0x4025, -1},
    {0},
    {1, 1, 0, 2, 0x4027, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_omegaVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x4023, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x4024, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x4026, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x4029, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_rougeSounds[11] = {
    {1, 1, 0, 0, 0x401E, -1},
    {0},
    {1, 1, 0, 2, 0x4020, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_rougeVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x401C, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x401D, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x401F, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x4022, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_sonicSounds[11] = {
    {1, 1, 0, 0, 0x4002, -1},
    {0},
    {1, 1, 0, 2, 0x4004, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_sonicVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x4000, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x4001, -1},
    {1, 1, 0, 0, 0x3005, -1},
    {0},
    {1, 1, 0, 0, 0x3002, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x4003, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x4006, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_tailsSounds[11] = {
    {1, 1, 0, 0, 0x4009, -1},
    {0},
    {1, 1, 0, 2, 0x400B, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_tailsVoices[32] = {
    {0},
    {1, 1, 0, 0, 0x4007, -1},
    {0}, {0},
    {1, 1, 0, 0, 0x4008, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {0}, {0},
    {1, 1, 0, 0, 0x400A, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0}, {0},
    {1, 1, 0, 0, 0x400D, -1},
    {0}, {0}, {0}, {0}, {0}, {0}, {0},
};
static const UnkSoundEntry_80305C50 s_shadowSounds[11] = {
    {1, 1, 0, 0, 0x300A, -1},
    {1, 1, 0, 0, 0x2005, -1},
    {1, 1, 0, 2, 0x301F, -1},
    {1, 1, 0, 0, 0x3021, -1},
    {1, 1, 0, 0, 0x3020, -1},
    {1, 1, 0, 0, 0x3022, -1},
    {1, 1, 0, 0, 0x301F, -1},
    {1, 1, 0, 0, 0x3014, -1},
    {1, 1, 0, 0, 0x3021, -1},
    {1, 1, 0, 0, 0x3022, -1},
    {1, 1, 0, 0, 0x3026, -1},
};
static const UnkSoundEntry_80305C50 s_shadowVoices[32] = {
    {1, 1, 1, 0, 0xE005, -1},
    {1, 1, 0, 0, 0x3003, -1},
    {1, 1, 0, 0, 0x3004, -1},
    {1, 1, 0, 0, 0x3003, -1},
    {1, 1, 0, 0, 0x3005, -1},
    {1, 1, 0, 0, 0x3005, -1},
    {1, 0, 0, 0, 0x3012, -1},
    {1, 1, 0, 0, 0x3002, -1},
    {1, 1, 0, 0, 0x3001, -1},
    {1, 1, 0, 0, 0x300D, -1},
    {1, 1, 0, 0, 0x7000, -1},
    {1, 0, 0, 0, 0x7001, -1},
    {1, 0, 0, 0, 0x300E, -1},
    {1, 1, 0, 0, 0x3005, 13},
    {1, 0, 0, 0, 0x3010, 13},
    {1, 1, 0, 0, 0x3023, -1},
    {1, 1, 0, 0, -1, -1},
    {1, 1, 0, 0, -1, -1},
    {1, 1, 0, 0, 0x3006, -1},
    {1, 1, 0, 0, 0x3009, -1},
    {1, 1, 0, 0, 0x3006, -1},
    {1, 1, 0, 0, 0x301E, -1},
    {1, 1, 0, 0, -1, -1},
    {1, 1, 0, 0, 0x3015, -1},
    {1, 1, 0, 0, -1, 11},
    {1, 1, 1, 0, 0x3016, 9},
    {1, 1, 1, 0, 0x3016, 10},
    {1, 1, 1, 0, 0xE005, -1},
    {1, 1, 0, 0, 0x3019, -1},
    {1, 1, 0, 0, 0x1010, -1},
    {1, 0, 0, 0, 0x1011, -1},
    {1, 1, 0, 0, 0x1012, -1},
};
static const UnkSoundTable_80305BF0 s_vectorTable = {s_vectorSounds, s_vectorVoices};
static const UnkSoundTable_80305BF0 s_amyTable = {s_amySounds, s_amyVoices};
static const UnkSoundTable_80305BF0 s_beeTable = {s_beeSounds, s_beeVoices};
static const UnkSoundTable_80305BF0 s_doomsEyeTable = {s_doomsEyeSounds, s_doomsEyeVoices};
static const UnkSoundTable_80305BF0 s_eggMonitorTable = {s_eggMonitorSounds, s_eggMonitorVoices};
static const UnkSoundTable_80305BF0 s_espioTable = {s_espioSounds, s_espioVoices};
static const UnkSoundTable_80305BF0 s_knucklesTable = {s_knucklesSounds, s_knucklesVoices};
static const UnkSoundTable_80305BF0 s_mariaTable = {s_mariaSounds, s_mariaVoices};
static const UnkSoundTable_80305BF0 s_omegaTable = {s_omegaSounds, s_omegaVoices};
static const UnkSoundTable_80305BF0 s_rougeTable = {s_rougeSounds, s_rougeVoices};
static const UnkSoundTable_80305BF0 s_sonicTable = {s_sonicSounds, s_sonicVoices};
static const UnkSoundTable_80305BF0 s_tailsTable = {s_tailsSounds, s_tailsVoices};
static const UnkSoundTable_80305BF0 s_shadowTable = {s_shadowSounds, s_shadowVoices};

namespace Player {
namespace Shadow {

class Sound : public SoundBase {
  public:
    Sound(Task *parent, const boost::shared_ptr<PlayerBase> &player)
        : SoundBase(parent, player), m_count15(0), m_count16(0) {}
    virtual ~Sound();
    virtual void PlaySound(s32 index, s32 arg);
    virtual void StopVoice(s32 index);
    virtual UnkSoundHandle_801D3DC8 PlayVoice(s32 index, s32 arg);

    /* Count the plays of voices 15 and 16, which alternate (names inferred). */
    /* 0x38 */ s32 m_count15;
    /* 0x3C */ s32 m_count16;
};
SIZE_ASSERT(Sound, 0x40);

Sound::~Sound() {}

void Sound::PlaySound(s32 index, s32 arg) {
    if (index == 1) {
        boost::shared_ptr<Weapon::WeaponBase> weapon = m_player->m_weapon->GetWeapon();
        boost::shared_ptr<Weapon::WeaponStickBase> stick =
            boost::dynamic_pointer_cast<Weapon::WeaponStickBase>(weapon);
        if (stick.get() != 0) {
            stick->vf_0x38(arg);
            switch (stick->TypeId()) {
            case 0x20:
                fn_8034DF48(11, m_player->field_0x54);
                break;
            case 0x39:
            case 0x3A:
                fn_8034DF48(0, m_player->field_0x54);
                break;
            }
        }
    } else {
        fn_802DBD4C(this, &s_shadowTable, index, arg);
    }
}

void Sound::StopVoice(s32 index) {
    switch (index) {
    case 15:
        if (fn_8006CD78(m_player->m_stateBits, 0x24)) {
            fn_801D560C(fn_80053164(), 0x8131, 0, 0);
            if ((++m_count15 & 1) == 0) {
                fn_801D51A0(fn_80053164(), 0x1150, -1, -1);
            }
        } else if ((++m_count15 & 1) == 0) {
            fn_802DBD1C(this, &s_shadowTable, index);
        } else {
            fn_801D51A0(fn_80053164(), 0x1150, -1, -1);
        }
        break;
    case 16:
        if ((++m_count16 & 1) == 0) {
            fn_801D51A0(fn_80053164(), 1, -1, -1);
        } else {
            fn_801D51A0(fn_80053164(), 2, -1, -1);
        }
        break;
    case 17:
        if (lbl_8057D6A0.field_0xA8 == 0x2C6) {
            fn_801D51A0(fn_80053164(), 0x11, -1, -1);
        } else {
            fn_801D51A0(fn_80053164(), 3, -1, -1);
        }
        break;
    default:
        fn_802DBD1C(this, &s_shadowTable, index);
        break;
    }
}

UnkSoundHandle_801D3DC8 Sound::PlayVoice(s32 index, s32 arg) {
    return fn_802DBC8C(this, &s_shadowTable, index, arg);
}

} // namespace Shadow
} // namespace Player

/* Creates Shadow's sound task (name of the function unknown). */
extern "C" Task *fn_802DB814(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return new Player::Shadow::Sound(parent, player);
}

/* Create the sound tasks of the other characters (names of the functions unknown). */
extern "C" Task *fn_802DB7F0(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_tailsTable);
}

extern "C" Task *fn_802DB7CC(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_sonicTable);
}

extern "C" Task *fn_802DB7A8(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_rougeTable);
}

extern "C" Task *fn_802DB784(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_omegaTable);
}

extern "C" Task *fn_802DB760(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_mariaTable);
}

extern "C" Task *fn_802DB73C(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_knucklesTable);
}

extern "C" Task *fn_802DB718(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_espioTable);
}

extern "C" Task *fn_802DB6F4(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_eggMonitorTable);
}

extern "C" Task *fn_802DB6D0(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_doomsEyeTable);
}

extern "C" Task *fn_802DB6AC(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_beeTable);
}

extern "C" Task *fn_802DB688(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_amyTable);
}

extern "C" Task *fn_802DB664(Task *parent,
                              const boost::shared_ptr<Player::PlayerBase> &player) {
    return fn_80305BF0(parent, player, &s_vectorTable);
}
