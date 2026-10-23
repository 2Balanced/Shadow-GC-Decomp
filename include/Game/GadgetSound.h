#ifndef GAME_GADGETSOUND_H
#define GAME_GADGETSOUND_H

#include "Game/MathTypes.h"
#include "dolphin/types.h"
#include "size_assert.h"

class GadgetCharInfo;

/* A sound playing at an object (class name unknown; layout from its constructor and assignment).
 * fn_801D3DC8 starts one, fn_800CCE14 moves it and fn_801D455C stops it and clears the handle. Its
 * destructor does nothing; owners stop it explicitly. */
class UnkSoundHandle_801D3DC8 {
  public:
    UnkSoundHandle_801D3DC8();
    /* Defined in Game/AudioLoopManagerTask.cpp (0x801D3D54). */
    UnkSoundHandle_801D3DC8(const UnkSoundHandle_801D3DC8 &other);
    UnkSoundHandle_801D3DC8 &operator=(const UnkSoundHandle_801D3DC8 &other);
    /* Stops the sound and clears the handle (behaviour inferred). */
    void fn_801D455C();

    /* 0x00 */ u32 m_id; /* zero when no sound is playing */
    /* 0x04 */ u16 field_0x4;
    /* 0x06 */ u16 field_0x6;
    /* 0x08 */ u16 field_0x8;
    /* 0x0A */ u8 field_0xA;
    /* 0x0B */ u8 field_0xB;
    /* 0x0C */ Vec3f m_position;
    /* 0x18 */ u32 field_0x18;
};
SIZE_ASSERT(UnkSoundHandle_801D3DC8, 0x1C);

extern "C" {
/* Starts sound soundId at charInfo's position for owner (behaviour inferred). */
UnkSoundHandle_801D3DC8 fn_801D3DC8(int soundId, GadgetCharInfo *charInfo, void *owner);
/* Moves the sound to position (behaviour inferred). */
void fn_800CCE14(UnkSoundHandle_801D3DC8 *sound, const Vec3f *position);
}

#endif
