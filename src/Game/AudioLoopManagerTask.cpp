/* AudioLoopManagerTask (class name from RTTI): the task that keeps the looping sounds going. The
 * sounds objects start (fn_801D3C38, fn_801D3DC8, fn_801D3E58, fn_801D3EF0) go into a list, and
 * every frame the task replays each one at its source: a character, a position or nothing.
 * UnkSoundHandle_801D3DC8::fn_801D455C takes a sound out again. The list is the MSL <list>
 * (stand-in in MSL_C++). Behaviours are inferred; names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/GadgetSound.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <list>

extern "C" {
/* The sound manager and playing a sound at a source (behaviours inferred). */
void *fn_80053164(void);
void fn_801D53C4(void *sound, int id, void *source, void *owner, int arg4, int arg5, int arg6);
/* Clears a vector (see Game/GadgetUtil.h). */
void fn_8040CE60(Vec3f *vector);
}

/* The looping sounds (class name unknown). */
class UnkLoopSounds_801D3CC8 {
  public:
    typedef std::list<UnkSoundHandle_801D3DC8>::iterator iterator;

    UnkLoopSounds_801D3CC8();
    ~UnkLoopSounds_801D3CC8();
    /* Adds sound under a new serial number and returns the entry (name inferred). */
    UnkSoundHandle_801D3DC8 Start(const UnkSoundHandle_801D3DC8 &sound);
    /* Takes sound out (name inferred). */
    void Stop(UnkSoundHandle_801D3DC8 *sound);
    /* Replays every sound at its source (name inferred). */
    void Update(float dt);
    /* Drops every sound (name inferred; unused, so the linker drops it). */
    void Clear();

    static bool IsSame(const UnkSoundHandle_801D3DC8 &a, const UnkSoundHandle_801D3DC8 &b) {
        return a.m_id == b.m_id && a.field_0x4 == b.field_0x4 && a.field_0x6 == b.field_0x6;
    }

    /* 0x00 */ u16 m_nextId;
    /* 0x04 */ std::list<UnkSoundHandle_801D3DC8> m_sounds;
    /* 0x10 */ s32 m_count;
};
SIZE_ASSERT(UnkLoopSounds_801D3CC8, 0x14);

/* The pointer fn_801D3CC8 hands out (name inferred). */
extern UnkLoopSounds_801D3CC8 *lbl_805EE23C;

class AudioLoopManagerTask : public Task {
  public:
    AudioLoopManagerTask(Task *parent);
    virtual ~AudioLoopManagerTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(AudioLoopManagerTask, 0x28);

extern "C" UnkLoopSounds_801D3CC8 *fn_801D3CC8(void);

void UnkSoundHandle_801D3DC8::fn_801D455C() {
    if (m_id != 0) {
        fn_801D3CC8()->Stop(this);
    }
    m_id = 0;
    field_0x4 = 0;
    field_0x6 = 0;
    field_0x8 = 0;
    field_0xA = 0;
    field_0xB = 0;
    field_0x18 = 0;
    fn_8040CE60(&m_position);
}

UnkLoopSounds_801D3CC8::UnkLoopSounds_801D3CC8() {
    m_nextId = 0;
    m_count = 0;
    m_sounds.clear();
}

UnkLoopSounds_801D3CC8::~UnkLoopSounds_801D3CC8() {
    m_count = 0;
}

void UnkLoopSounds_801D3CC8::Update(float dt) {
    for (iterator it = m_sounds.begin(); it != m_sounds.end(); ++it) {
        if (it->field_0xB & 1) {
            if (it->field_0xB & 4) {
                fn_801D53C4(fn_80053164(), it->field_0x4, (void *)it->field_0x18, (void *)it->m_id,
                            it->field_0x8, it->field_0xA, 2);
            } else {
                fn_801D53C4(fn_80053164(), it->field_0x4, &it->m_position, (void *)it->m_id,
                            it->field_0x8, it->field_0xA, 2);
            }
        } else {
            fn_801D53C4(fn_80053164(), it->field_0x4, 0, (void *)it->m_id, it->field_0x8,
                        it->field_0xA, 2);
        }
    }
}

UnkSoundHandle_801D3DC8 UnkLoopSounds_801D3CC8::Start(const UnkSoundHandle_801D3DC8 &sound) {
    UnkSoundHandle_801D3DC8 entry(sound);
    u16 serial = m_nextId;
    entry.field_0x6 = serial;
    m_nextId = serial + 1;
    m_sounds.push_back(entry);
    m_count++;
    return entry;
}

void UnkLoopSounds_801D3CC8::Stop(UnkSoundHandle_801D3DC8 *sound) {
    for (iterator it = m_sounds.begin(); it != m_sounds.end(); ++it) {
        if (IsSame(*it, *sound)) {
            m_sounds.erase(it);
            m_count--;
            break;
        }
    }
}

void UnkLoopSounds_801D3CC8::Clear() {
    m_sounds.clear();
    m_count = 0;
}

const char *AudioLoopManagerTask::s_taskName = "AudioLoopManagerTask";

AudioLoopManagerTask::AudioLoopManagerTask(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
}

AudioLoopManagerTask::~AudioLoopManagerTask() {}

void AudioLoopManagerTask::Tick(float dt) {
    fn_801D3CC8()->Update(dt);
}

/* Starts a sound with no source (name inferred). */
extern "C" UnkSoundHandle_801D3DC8 fn_801D3EF0(int id, void *owner) {
    UnkSoundHandle_801D3DC8 sound;
    if ((u16)id == 0xFFFF) {
        return sound;
    }
    sound.m_id = (u32)owner;
    sound.field_0x4 = id;
    sound.field_0x8 |= 2;
    return fn_801D3CC8()->Start(sound);
}

/* Starts a sound at position (name inferred). */
extern "C" UnkSoundHandle_801D3DC8 fn_801D3E58(int id, const Vec3f *position, void *owner) {
    UnkSoundHandle_801D3DC8 sound;
    if ((u16)id == 0xFFFF) {
        return sound;
    }
    sound.m_id = (u32)owner;
    sound.field_0x4 = id;
    fn_800091D0(&sound.m_position, position);
    sound.field_0x8 |= 3;
    sound.field_0xB |= 1;
    return fn_801D3CC8()->Start(sound);
}

/* Starts a sound at a character (name inferred). */
extern "C" UnkSoundHandle_801D3DC8 fn_801D3DC8(int id, GadgetCharInfo *character, void *owner) {
    UnkSoundHandle_801D3DC8 sound;
    if ((u16)id == 0xFFFF) {
        return sound;
    }
    sound.m_id = (u32)owner;
    sound.field_0x4 = id;
    sound.field_0x18 = (u32)character;
    sound.field_0x8 |= 3;
    sound.field_0xB |= 5;
    return fn_801D3CC8()->Start(sound);
}

UnkSoundHandle_801D3DC8::UnkSoundHandle_801D3DC8(const UnkSoundHandle_801D3DC8 &other)
    : m_id(other.m_id), field_0x4(other.field_0x4), field_0x6(other.field_0x6),
      field_0x8(other.field_0x8), field_0xA(other.field_0xA), field_0xB(other.field_0xB) {
    fn_800091EC(&m_position, &other.m_position);
    field_0x18 = other.field_0x18;
}

/* The looping sounds (name inferred). */
static UnkLoopSounds_801D3CC8 *GetLoopSounds() {
    static UnkLoopSounds_801D3CC8 s_sounds;
    return &s_sounds;
}

extern "C" UnkLoopSounds_801D3CC8 *fn_801D3CC8(void) {
    if (lbl_805EE23C == 0) {
        lbl_805EE23C = GetLoopSounds();
    }
    return lbl_805EE23C;
}

/* Starts a sound at a character, flagged 0x40 (name inferred). */
extern "C" UnkSoundHandle_801D3DC8 fn_801D3C38(int id, GadgetCharInfo *character, void *owner) {
    UnkSoundHandle_801D3DC8 sound;
    if ((u16)id == 0xFFFF) {
        return sound;
    }
    sound.m_id = (u32)owner;
    sound.field_0x4 = id;
    sound.field_0x18 = (u32)character;
    sound.field_0x8 |= 0x43;
    sound.field_0xB |= 5;
    return fn_801D3CC8()->Start(sound);
}
