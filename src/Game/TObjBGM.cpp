/* TObjBGM and TObjSERIF (class names from RTTI): the tasks that stream music and voices through
 * the CRI ADX player (ADXT handles; the ADX library is at 0x803B5000..0x803C7000). Two TObjBGM
 * (the song and the jingle) and two TObjSERIF (voice lines) are created by fn_801D80AC; the
 * functions after them are the music and voice controls the sound manager forwards to
 * (Game/AudioManagerTask.cpp): play, stop, pause, fade and cross-fade. Behaviours and names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first).
 *
 * NonMatching: fn_801D78C4 (96%), fn_801D7A50 (99%) and fn_801D7CB8 (99%) differ only in which
 * registers hold the loop counter and the player pointers; every other function and all data
 * match. */
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 m_language; /* 0 picks the Japanese voices (meaning inferred) */
    /* 0x044 */ u8 field_0x44[0x5C - 0x44];
    /* 0x05C */ s32 m_voiceSet; /* the voice archive loaded (meaning inferred) */
    /* 0x060 */ u8 field_0x60[0x78 - 0x60];
    /* 0x078 */ s32 field_0x78; /* 1 lowers the music (meaning inferred) */
    /* 0x07C */ u8 field_0x7C[0x150 - 0x7C];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* The CRI ADX player (names unknown; behaviours inferred from their use here). */
void fn_803C1174(void *player, int pause);
void fn_803C1F40(void *player);
int fn_803C1EE8(void *player);
void fn_803C156C(void *player, int volume);
int fn_803C1518(void *player);
int fn_803C18DC(void *player);
int fn_803C1868(void *player);
void fn_803C12D0(void *player);
void *fn_803C2CE4(int channels, void *work, int size);
void fn_803C2860(void *player);
void fn_803C2D44(void *player, const char *file);
void fn_803C2DF0(void *player, int partition, int file);
int fn_803C0ED4(void *player);
void fn_803C0A90(int mono);
void fn_803C46C0(void);
void fn_803C4700(int arg);
void fn_803C40C8(int arg);
void fn_803B6288(void);
void fn_803B61F0(void);
void fn_803C6AF0(int partition, const char *file, int arg2, void *info);
int fn_803C6720(int partition);
/* The ADX seamless-loop player (names unknown). */
int fn_803BD464(void *loop);
void fn_803BD5D0(void *loop, void *entry);
void fn_803BD4C0(void *loop, int arg);
void fn_803BD520(void *loop);
void fn_803BD408(void *loop);
/* Memory, the task tree and whether a task is alive (behaviours inferred). */
void *fn_80049B90(int size, int align);
void fn_80049B34(void *block);
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
u8 fn_8004EAC0(Task *task);
/* Defined below. */
void fn_801D76A0(void);
void fn_801D7B28(void);
}

/* The seamless-loop player a song can use (class name unknown). */
class UnkBgmLoop_801D6914 {
  public:
    void Update();
    void Start();

    /* Queues entry next (name inferred). */
    void SetNext(s8 next) {
        if (next >= 0 && next <= 8) {
            if (m_current < 0) {
                m_current = next;
            }
            m_next = next;
        }
    }

    /* 0x00 */ s8 m_current;
    /* 0x01 */ s8 m_next;
    /* 0x04 */ int m_time;
    /* 0x08 */ void *m_loop;
    /* 0x0C */ u8 m_entries[9][0x20];
};

class TObjBGM : public Task {
  public:
    TObjBGM(Task *parent, s8 index, void *work);
    virtual ~TObjBGM();
    virtual void Tick(float dt);

    void Stop();
    bool SetName(const char *name);
    void Play();
    void FadeOut();
    void Reset();

    /* Fades the song out from wherever it is (name inferred). */
    void FadeOutNow() {
        switch (m_state) {
        case 1:
        case 2:
        case 3:
        case 5:
            fn_803C156C(m_player, -999);
            m_volume = -999;
            m_state = 6;
            break;
        }
    }

    static const char *s_taskName;

    /* 0x28 */ s8 m_index;
    /* 0x29 */ s8 m_state; /* 1 starting, 2 playing, 3/5 fading in, 4/6 fading out */
    /* 0x2A */ s8 m_type;  /* 1 a song, 2 uses the loop player (meanings inferred) */
    /* 0x2B */ u8 m_paused;
    /* 0x2C */ s8 m_delay;
    /* 0x30 */ int field_0x30;
    /* 0x34 */ int m_targetVolume;
    /* 0x38 */ int m_volume;
    /* 0x3C */ int m_status;
    /* 0x40 */ int m_loopTime;
    /* 0x44 */ void *m_player;
    /* 0x48 */ void *m_work;
    /* 0x4C */ UnkBgmLoop_801D6914 *m_loop;
    /* 0x50 */ char m_file[0x20];
    /* 0x70 */ char m_nextFile[0x20];
};
SIZE_ASSERT(TObjBGM, 0x90);

class TObjSERIF : public Task {
  public:
    TObjSERIF(Task *parent, s8 index, void *work, int arg4);
    virtual ~TObjSERIF();
    virtual void Tick(float dt);

    void Reset();

    /* Stops the voice (name inferred). */
    void Stop() {
        fn_803C1F40(m_player);
        fn_803C1174(m_player, 0);
        m_voice = -1;
        m_owner = -1;
        m_priority = -1;
        m_state = 0;
        m_paused &= ~1;
    }

    static const char *s_taskName;

    /* 0x28 */ s8 m_index;
    /* 0x29 */ s8 m_state; /* 1 playing */
    /* 0x2A */ s8 m_priority;
    /* 0x2B */ u8 m_paused;
    /* 0x2C */ int m_voice;
    /* 0x30 */ int m_owner;
    /* 0x34 */ int m_status;
    /* 0x38 */ int m_volume;
    /* 0x3C */ void *m_player;
    /* 0x40 */ void *m_work;
    /* 0x44 */ int field_0x44;
};
SIZE_ASSERT(TObjSERIF, 0x48);

/* A song's loop point (struct name unknown). */
struct UnkBgmLoopPoint {
    /* 0x0 */ const char *m_file;
    /* 0x4 */ s16 m_seconds;
    /* 0x6 */ s16 m_frames;
};

/* The songs that loop before their end, ended by a negative entry (name inferred). */
static UnkBgmLoopPoint s_loopPoints[] = {
    {"sng_speedup.adx", 12, 90},
    {"sng_invncbl.adx", 20, 0},
    {"", -1, -1},
};

/* The songs, the jingles and their work areas (names inferred). */
static TObjBGM *s_bgm[2] = {0, 0};
static void *s_bgmWork[2] = {0, 0};
static TObjSERIF *s_serif[2] = {0, 0};
static void *s_serifWork[2] = {0, 0};
/* The voice archive loaded, 6 for none (name inferred). */
static int s_voiceSet = 6;
static const char *s_voiceFileJ = "PRS_VOICE_J.afs";
static const char *s_voiceFileE = "PRS_VOICE_E.afs";

const char *TObjBGM::s_taskName = "TObjBGM";
const char *TObjSERIF::s_taskName = "TObjSERIF";

/* Whether the music is cross-faded between the two songs, and the one playing (names
 * inferred). */
static s8 s_crossFade;
static s8 s_current;

/* The voice archive's partition (name inferred). */
static u8 s_partition[0x2420];

/* Initialises the ADX library and allocates the players' work areas (name inferred). */
extern "C" void fn_801D83AC(void) {
    fn_803C4700(0);
    fn_803C40C8(0);
    fn_803B6288();
    for (int i = 0; i < 2; i++) {
        s_bgmWork[i] = fn_80049B90(0x389E4, 2);
    }
    for (int i = 0; i < 2; i++) {
        s_serifWork[i] = fn_80049B90(0x17124, 2);
    }
    s_voiceSet = 6;
}

/* Frees the work areas and shuts the ADX library down (name inferred). */
extern "C" void fn_801D830C(void) {
    for (u8 i = 0; i < 2; i++) {
        if (s_bgmWork[i] != 0) {
            fn_80049B34(s_bgmWork[i]);
            s_bgmWork[i] = 0;
        }
    }
    for (u8 i = 0; i < 2; i++) {
        if (s_serifWork[i] != 0) {
            fn_80049B34(s_serifWork[i]);
            s_serifWork[i] = 0;
        }
    }
    s_voiceSet = 6;
    fn_803B61F0();
}

/* Runs the ADX server (name inferred). */
extern "C" void fn_801D82EC(void) {
    fn_803C46C0();
}

/* Loads the voice archive and creates the players (name inferred). */
extern "C" void fn_801D80AC(void) {
    s_crossFade = 0;
    s_current = 0;
    if (s_voiceSet != lbl_8057D6A0.m_voiceSet) {
        if (lbl_8057D6A0.m_language == 0) {
            fn_803C6AF0(0, s_voiceFileJ, 0, s_partition);
        } else {
            fn_803C6AF0(0, s_voiceFileE, 0, s_partition);
        }
        while (true) {
            int status = fn_803C6720(0);
            if (status == 3) {
                s_voiceSet = lbl_8057D6A0.m_voiceSet;
                break;
            }
            if (status == 4) {
                s_voiceSet = 6;
                break;
            }
        }
    }
    for (s8 i = 0; i < 2; i++) {
        if (s_bgm[i] != 0) {
            delete s_bgm[i];
            s_bgm[i] = 0;
        }
        if (s_bgmWork[i] != 0) {
            s_bgm[i] = new TObjBGM(fn_801EB304(fn_8001C4F8(), 1), i, s_bgmWork[i]);
            if (!fn_8004EAC0(s_bgm[i])) {
                s_bgm[i] = 0;
                fn_80049B34(s_bgmWork[i]);
                s_bgmWork[i] = 0;
            }
        }
    }
    for (s8 i = 0; i < 2; i++) {
        if (s_serif[i] != 0) {
            delete s_serif[i];
            s_serif[i] = 0;
        }
        if (s_serifWork[i] != 0) {
            s_serif[i] = new TObjSERIF(fn_801EB304(fn_8001C4F8(), 1), i, s_serifWork[i], 0);
            if (s_serif[i] == 0 || !fn_8004EAC0(s_serif[i])) {
                s_serif[i] = 0;
                fn_80049B34(s_serifWork[i]);
                s_serifWork[i] = 0;
            }
        }
    }
}

/* Deletes the players (name inferred). */
extern "C" void fn_801D7FF0(void) {
    for (u8 i = 0; i < 2; i++) {
        if (s_bgm[i] != 0) {
            delete s_bgm[i];
            s_bgm[i] = 0;
        }
    }
    for (u8 i = 0; i < 2; i++) {
        if (s_serif[i] != 0) {
            delete s_serif[i];
            s_serif[i] = 0;
        }
    }
}

/* Resets the players (name inferred). */
extern "C" void fn_801D7F74(void) {
    for (int i = 0; i < 2; i++) {
        if (s_bgm[i] != 0) {
            s_bgm[i]->Reset();
        }
    }
    for (int i = 0; i < 2; i++) {
        if (s_serif[i] != 0) {
            s_serif[i]->Reset();
        }
    }
}

/* Plays song name (name inferred). */
extern "C" void fn_801D7F2C(const char *name) {
    TObjBGM *bgm = s_bgm[0];
    if (bgm != 0) {
        bgm->SetName(name);
        bgm->Play();
    }
}

/* The song playing, or 0 (name inferred). */
extern "C" const char *fn_801D7ED8(void) {
    TObjBGM *bgm = s_bgm[0];
    if (bgm != 0) {
        if (strlen(bgm->m_file) == 0) {
            return 0;
        }
        return bgm->m_file;
    }
    return 0;
}

/* Stops the song (name inferred). */
extern "C" void fn_801D7EAC(void) {
    if (s_bgm[0] != 0) {
        s_bgm[0]->Stop();
    }
}

/* Plays jingle name over the song (name inferred). */
extern "C" void fn_801D7E10(const char *name) {
    TObjBGM *bgm = s_bgm[0];
    TObjBGM *jingle = s_bgm[1];
    if (jingle != 0) {
        if (s_crossFade == 1) {
            if (bgm != 0) {
                bgm->Stop();
            }
            jingle->Stop();
            jingle->SetName(name);
            jingle->Play();
        } else {
            jingle->SetName(name);
            jingle->Play();
            if (bgm != 0) {
                bgm->FadeOut();
            }
        }
    }
}

/* Stops the jingle (name inferred). */
extern "C" void fn_801D7DA0(void) {
    TObjBGM *jingle = s_bgm[1];
    TObjBGM *bgm = s_bgm[0];
    if (jingle != 0) {
        if (s_crossFade == 1) {
            jingle->Stop();
        } else {
            jingle->Stop();
            if (bgm != 0) {
                switch (bgm->m_state) {
                case 6:
                    bgm->m_state = 5;
                    break;
                }
            }
        }
    }
}

/* Pauses the music and the voices (name inferred). */
extern "C" void fn_801D7CB8(void) {
    TObjBGM *bgm;
    for (u8 i = 0; i < 2; i++) {
        bgm = s_bgm[i];
        if (bgm != 0) {
            switch (bgm->m_state) {
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                fn_803C1174(bgm->m_player, 1);
                bgm->m_paused |= 1;
                break;
            case 1:
                bgm->m_paused |= 1;
                break;
            }
        }
    }
    TObjSERIF *serif;
    for (u8 i = 0; i < 2; i++) {
        serif = s_serif[i];
        if (serif != 0) {
            switch (serif->m_state) {
            case 1:
                fn_803C1174(serif->m_player, 1);
                serif->m_paused |= 1;
                break;
            }
        }
    }
}

/* Resumes the music and the voices (name inferred). */
extern "C" void fn_801D7BE0(void) {
    TObjBGM *bgm;
    for (u8 i = 0; i < 2; i++) {
        bgm = s_bgm[i];
        if (bgm != 0) {
            switch (bgm->m_state) {
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                fn_803C1174(bgm->m_player, 0);
                break;
            }
            bgm->m_paused &= ~1;
        }
    }
    for (u8 i = 0; i < 2; i++) {
        TObjSERIF *serif = s_serif[i];
        if (serif != 0) {
            switch (serif->m_state) {
            case 1:
                fn_803C1174(serif->m_player, 0);
                serif->m_paused &= ~1;
                break;
            }
        }
    }
}

/* Fades the song back in after a jingle (name inferred). */
extern "C" void fn_801D7B28(void) {
    if (s_crossFade != 1) {
        if (s_bgm[0] != 0 && s_bgm[1] != 0) {
            switch (s_bgm[0]->m_state) {
            case 6:
                s_bgm[0]->m_state = 5;
                break;
            }
            switch (s_bgm[1]->m_state) {
            case 2:
            case 3:
            case 5:
            case 6:
                s_bgm[1]->m_state = 4;
                break;
            }
        }
    } else if (s_bgm[0] != 0 && s_bgm[1] != 0) {
        s_bgm[1]->Stop();
        fn_801D76A0();
    }
}

/* The song's status, or -1 (name inferred). */
extern "C" int fn_801D7B0C(void) {
    if (s_bgm[0] == 0) {
        return -1;
    }
    return s_bgm[0]->m_status;
}

/* Whether a voice of priority current gives way to one of priority incoming (name inferred). */
inline bool CanInterrupt(s8 current, s8 incoming) {
    if (current == 3) {
        return false;
    }
    if (current <= 0) {
        return true;
    }
    return current < incoming;
}

/* The voice player to use for owner's voice of priority, or -1 (name inferred). */
extern "C" int fn_801D78C4(int owner, s8 priority) {
    if (owner >= 0) {
        for (int i = 0; i < 2; i++) {
            TObjSERIF *serif = s_serif[i];
            if (serif != 0 && serif->m_owner == owner) {
                if (CanInterrupt(serif->m_priority, priority)) {
                    return i;
                }
                return -1;
            }
        }
    }
    int found = -1;
    for (int i = 0; i < 2; i++) {
        TObjSERIF *serif = s_serif[i];
        if (serif != 0) {
            if (serif->m_state != 1) {
                return i;
            }
            if (CanInterrupt(serif->m_priority, priority) == true) {
                if (found < 0) {
                    found = i;
                } else if (CanInterrupt(serif->m_priority, s_serif[found]->m_priority) == true) {
                    found = i;
                }
            }
        }
    }
    return found;
}

/* Plays voice for owner with priority; returns the player used, or -1 (name inferred). */
extern "C" int fn_801D7A50(int voice, int priority, int owner) {
    TObjSERIF *serif;
    int index = fn_801D78C4(owner, priority);
    if (index >= 0) {
        s_serif[index]->Stop();
        serif = s_serif[index];
        serif->m_voice = voice;
        serif->m_priority = priority;
        serif->m_owner = owner;
        fn_803C2DF0(serif->m_player, 0, serif->m_voice);
        serif->m_state = 1;
        return index;
    }
    return -1;
}

/* Stops owner's voice (name inferred). */
extern "C" void fn_801D7830(int voice) {
    TObjSERIF *serif;
    for (int i = 0; i < 2; i++) {
        serif = s_serif[i];
        if (serif != 0 && voice == serif->m_voice) {
            serif->Stop();
        }
    }
}

/* Stops the voices (name inferred). */
extern "C" void fn_801D77AC(void) {
    TObjSERIF *serif;
    for (int i = 0; i < 2; i++) {
        serif = s_serif[i];
        if (serif != 0) {
            serif->Stop();
        }
    }
}

/* Cross-fades to the next song (name inferred). */
extern "C" void fn_801D76A0(void) {
    if (s_crossFade == 1) {
        TObjBGM *next = s_bgm[s_current];
        TObjBGM *prev = s_bgm[~s_current & 1];
        if (next != 0) {
            next->Stop();
            next->SetName(strlen(next->m_nextFile) == 0 ? 0 : next->m_nextFile);
            next->Play();
        }
        if (prev != 0) {
            prev->Stop();
            prev->SetName(strlen(prev->m_nextFile) == 0 ? 0 : prev->m_nextFile);
            prev->Play();
            prev->FadeOutNow();
        }
    }
}

/* Switches the sound to mode, mono for 1 (name inferred). */
extern "C" void fn_801D763C(int mode) {
    TObjBGM *bgm = s_bgm[0];
    fn_801D7EAC();
    fn_801D7DA0();
    fn_801D77AC();
    switch (mode) {
    case 1:
        fn_803C0A90(1);
        break;
    default:
        fn_803C0A90(0);
        break;
    }
    if (bgm != 0) {
        fn_801D80AC();
    }
}

/* Plays song name as the jingle (name inferred). */
extern "C" void fn_801D75B0(const char *name) {
    TObjBGM *bgm = s_bgm[0];
    TObjBGM *jingle = s_bgm[1];
    if (jingle != 0) {
        if (s_crossFade == 1) {
            if (bgm != 0) {
                bgm->Stop();
            }
            jingle->Stop();
            jingle->SetName(name);
            jingle->Play();
        } else {
            jingle->SetName(name);
            jingle->Play();
        }
    }
}

/* Stops the jingle (name inferred). */
extern "C" void fn_801D756C(void) {
    if (s_bgm[1] != 0) {
        if (s_crossFade == 1) {
            s_bgm[1]->Stop();
        } else {
            s_bgm[1]->Stop();
        }
    }
}

/* Sets the music volume (name inferred). */
extern "C" void fn_801D7544(int volume) {
    if (s_bgm[0] != 0) {
        s_bgm[0]->m_targetVolume = volume;
    }
    if (s_bgm[1] != 0) {
        s_bgm[1]->m_targetVolume = volume;
    }
}

/* Restores the music volume (name inferred). */
extern "C" void fn_801D7500(void) {
    int volume = -100;
    if (lbl_8057D6A0.field_0x78 == 1) {
        volume = -150;
    }
    if (s_bgm[0] != 0) {
        s_bgm[0]->m_targetVolume = volume;
    }
    if (s_bgm[1] != 0) {
        s_bgm[1]->m_targetVolume = volume;
    }
}

TObjBGM::TObjBGM(Task *parent, s8 index, void *work) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
    m_work = work;
    m_loop = 0;
    m_player = fn_803C2CE4(2, m_work, 0x389E4);
    if (m_player == 0) {
        m_flags |= 1;
        return;
    }
    m_index = index;
    Reset();
}

/* Deletes the loop player (name inferred). */
static void DeleteLoop(UnkBgmLoop_801D6914 *loop);

void TObjBGM::Reset() {
    Stop();
    if (m_loop != 0) {
        DeleteLoop(m_loop);
        m_loop = 0;
    }
    m_state = 0;
    m_type = 0;
    m_paused = 0;
    m_delay = 0;
    field_0x30 = 0;
    m_loopTime = 0;
    if (lbl_8057D6A0.field_0x78 == 1) {
        m_targetVolume = -150;
        m_volume = -150;
    } else {
        m_targetVolume = -100;
        m_volume = -100;
    }
    m_status = -1;
    memset(m_file, 0, 0x20);
    memset(m_nextFile, 0, 0x20);
    fn_803C156C(m_player, m_volume);
}

TObjBGM::~TObjBGM() {
    Stop();
    if (m_loop != 0) {
        DeleteLoop(m_loop);
    }
    if (m_player != 0) {
        fn_803C2860(m_player);
    }
    s_bgm[m_index] = 0;
}

/* The loop point of file, or 0 (name inferred). */
inline int FindLoopTime(const char *file) {
    for (UnkBgmLoopPoint *point = s_loopPoints; point->m_seconds >= 0; point++) {
        if (strcmp(point->m_file, file) == 0) {
            return point->m_frames + point->m_seconds * 100;
        }
    }
    return 0;
}

void TObjBGM::Play() {
    if (m_type == 2 || strlen(m_file) != 0) {
        switch (m_status) {
        case 0:
        case 5:
            break;
        default:
            Stop();
            break;
        }
        m_state = 1;
        m_delay = 2;
        m_loopTime = FindLoopTime(m_file);
        void *player = m_player;
        fn_803C156C(player, m_targetVolume);
        fn_803C1174(player, 1);
        if (m_type == 2 && m_loop != 0) {
            m_loop->Start();
        } else {
            fn_803C2D44(player, m_file);
        }
        m_volume = m_targetVolume;
    }
}

bool TObjBGM::SetName(const char *name) {
    if (name == 0) {
        return false;
    }
    memset(m_file, 0, 0x20);
    strcpy(m_file, name);
    return true;
}

void TObjBGM::Stop() {
    fn_803C1F40(m_player);
    fn_803C1174(m_player, 0);
    UnkBgmLoop_801D6914 *loop = m_loop;
    if (loop != 0) {
        fn_803BD408(loop->m_loop);
        loop->m_current = -1;
    }
    m_state = 0;
    m_paused &= ~1;
}

void TObjBGM::FadeOut() {
    switch (m_state) {
    case 1:
        FadeOutNow();
        break;
    case 2:
    case 3:
    case 5:
        m_state = 6;
        break;
    }
}

void TObjBGM::Tick(float dt) {
    void *player = m_player;
    switch (m_state) {
    case 1:
        if (!(m_paused & 1) && m_delay-- <= 0) {
            fn_803C1174(player, 0);
            if (m_type == 2) {
                UnkBgmLoop_801D6914 *loop = m_loop;
                if (loop != 0) {
                    loop->m_time = fn_803C1868(loop->m_loop);
                }
            }
            m_state = 2;
        }
        m_status = fn_803C1EE8(m_player);
        if (m_status == 6) {
            Stop();
            fn_803C12D0(m_player);
        }
        break;
    case 2:
        if (m_paused & 1) {
            break;
        }
        if (m_loopTime > 0 && fn_803C18DC(player) >= m_loopTime && m_index == 1) {
            if (m_type == 1) {
                Stop();
                fn_801D76A0();
            } else {
                fn_801D7B28();
            }
        }
        if (m_type == 2 && m_loop != 0) {
            m_loop->Update();
        }
        if (m_status == 0 || m_status == 5) {
            m_state = 0;
            if (m_index == 1) {
                if (m_type == 1) {
                    fn_801D76A0();
                } else {
                    fn_801D7B28();
                }
            }
        }
        break;
    case 3: {
        if (m_paused & 1) {
            break;
        }
        int volume = fn_803C1518(player);
        if (volume + 8 <= m_targetVolume) {
            fn_803C156C(player, volume + 8);
            m_volume = volume;
        } else {
            fn_803C156C(player, m_targetVolume);
            m_volume = m_targetVolume;
            m_state = 2;
        }
        break;
    }
    case 5: {
        if (m_paused & 1) {
            break;
        }
        if (m_type == 2 && m_loop != 0) {
            m_loop->Update();
        }
        int volume = fn_803C1518(player);
        if (volume + 8 <= m_targetVolume) {
            fn_803C156C(player, volume + 16);
            m_volume = volume;
        } else {
            fn_803C156C(player, m_targetVolume);
            m_volume = m_targetVolume;
            m_state = 2;
        }
        break;
    }
    case 4: {
        if (m_paused & 1) {
            break;
        }
        if (m_type == 2 && m_loop != 0) {
            m_loop->Update();
        }
        int volume = fn_803C1518(m_player);
        if (volume + 8 > -999) {
            fn_803C156C(player, volume - 8);
            m_volume = volume;
        } else {
            fn_803C156C(player, -999);
            m_volume = -999;
            Stop();
        }
        break;
    }
    case 6: {
        if (m_paused & 1) {
            break;
        }
        if (m_type == 2 && m_loop != 0) {
            m_loop->Update();
        }
        if (m_type == 1 && !(m_paused & 1) && m_delay >= 0 && m_delay-- <= 0 && m_delay <= 0) {
            fn_803C1174(player, 0);
            m_delay = -1;
        }
        int volume = fn_803C1518(m_player);
        if (volume + 8 > -999) {
            fn_803C156C(player, volume - 5);
            m_volume = volume;
        } else {
            m_volume = -999;
        }
        break;
    }
    case 0:
        break;
    }
    m_status = fn_803C1EE8(m_player);
    field_0x30 = 0;
}

TObjSERIF::TObjSERIF(Task *parent, s8 index, void *work, int arg4) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
    m_work = work;
    field_0x44 = arg4;
    m_player = fn_803C2CE4(1, m_work, 0x17124);
    if (m_player == 0) {
        m_flags |= 1;
        return;
    }
    m_index = index;
    Reset();
}

void TObjSERIF::Reset() {
    Stop();
    m_state = 0;
    m_priority = -1;
    m_voice = -1;
    m_owner = -1;
    m_status = -1;
    m_volume = -40;
    m_paused = 0;
    fn_803C156C(m_player, m_volume);
}

TObjSERIF::~TObjSERIF() {
    Stop();
    if (m_player != 0) {
        fn_803C2860(m_player);
    }
    s_serif[m_index] = 0;
}

void TObjSERIF::Tick(float dt) {
    switch (m_state) {
    case 0:
        m_status = fn_803C1EE8(m_player);
        break;
    case 1:
        m_status = fn_803C1EE8(m_player);
        if (m_status == 0 || m_status == 5) {
            m_voice = -1;
            m_owner = -1;
            m_priority = -1;
            m_state = 0;
        }
        break;
    }
}

static void DeleteLoop(UnkBgmLoop_801D6914 *loop) {
    if (loop != 0) {
        delete loop;
    }
}

void UnkBgmLoop_801D6914::Start() {
    SetNext(m_next);
    if (m_current >= 0) {
        fn_803BD5D0(m_loop, m_entries[m_current]);
        fn_803BD4C0(m_loop, 0);
        fn_803BD520(m_loop);
        m_time = fn_803C1868(m_loop);
    }
}

void UnkBgmLoop_801D6914::Update() {
    if (fn_803BD464(m_loop) == 0) {
        int start = fn_803C0ED4(m_loop);
        m_time = fn_803C1868(m_loop);
        if (m_time - start < 32000) {
            if (m_current != m_next) {
                m_current = m_next;
            }
            if (m_current >= 0) {
                fn_803BD5D0(m_loop, m_entries[m_current]);
            }
        }
    }
}
