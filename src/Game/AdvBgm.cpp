/* The front-end's music player (the game keeps one, reached through fn_801E5CA8), and a holder of
 * the story check the front-end builds from the save data while its menus are open (reached
 * through fn_801E5CF8). Each menu track alternates between two files where it has two. The file
 * name, the class names and every member name are inferred; the functions other units call keep
 * their fn_ names. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/SavedataAnalyzer.h"
#include <dolphin/types.h>
#include "size_assert.h"

using SavedataAnalyzer::StoryCheck;

extern "C" {
UnkSaveData_800BB74C *fn_800BB74C(void);

/* The sound manager: stopping the music, and playing a music file (behaviour inferred). */
void *fn_80053164(void);
void fn_801D522C(void *sound);
void fn_801D524C(void *sound, const char *file);
}

/* The music player (class name inferred). */
class UnkBgmPlayer_801E5CA8 {
  public:
    UnkBgmPlayer_801E5CA8();

    /* 0x0 */ s32 m_track; /* the track playing, 0 for none */
};
SIZE_ASSERT(UnkBgmPlayer_801E5CA8, 0x4);

/* The holder of the story check (class name inferred). */
class UnkHolder_801E5CF8 {
  public:
    UnkHolder_801E5CF8();

    /* 0x0 */ StoryCheck *m_object;
};
SIZE_ASSERT(UnkHolder_801E5CF8, 0x4);

/* A track's music files, and the one to play next (struct name inferred). */
struct UnkBgmTrack {
    /* 0x0 */ const char *m_files[2];
    /* 0x8 */ s32 m_next;
};

static UnkBgmTrack s_tracks[6] = {
    {{0, 0}, 0},
    {{"sng_sys01.adx", 0}, 0},
    {{"sng_sys02.adx", 0}, 0},
    {{"sng_sys03.adx", 0}, 0},
    {{"sng_sys04.adx", 0}, 0},
    {{"sng_sys05.adx", 0}, 0},
};

UnkBgmPlayer_801E5CA8::UnkBgmPlayer_801E5CA8() {
    m_track = 0;
}

/* Plays track (0 stops the music); a track that is already playing goes on. */
extern "C" void fn_802C839C(UnkBgmPlayer_801E5CA8 *player, int track) {
    if (track == 0) {
        fn_801D522C(fn_80053164());
        player->m_track = 0;
    } else if (player->m_track != track) {
        const char *file;
        UnkBgmTrack *entry = &s_tracks[track];
        file = entry->m_files[entry->m_next];
        if (file != 0) {
            fn_801D524C(fn_80053164(), file);
        } else {
            entry->m_next = 0;
            fn_801D524C(fn_80053164(), entry->m_files[entry->m_next]);
        }
        entry->m_next++;
        if (entry->m_next >= 2) {
            entry->m_next = 0;
        }
        player->m_track = track;
    }
}

UnkHolder_801E5CF8::UnkHolder_801E5CF8() {
    m_object = 0;
}

/* Builds the story check from the save data and works it out. */
extern "C" void fn_802C8338(UnkHolder_801E5CF8 *holder) {
    StoryCheck *object = (StoryCheck *)operator new(sizeof(StoryCheck));
    if (object != 0) {
        object = fn_801AC1B4(object, fn_800BB74C());
    }
    holder->m_object = object;
    fn_801ABDB8(holder->m_object);
}

/* Destroys the story check. */
extern "C" void fn_802C8308(UnkHolder_801E5CF8 *holder) {
    if (holder->m_object != 0) {
        fn_800BBC04(holder->m_object, 1);
    }
}
