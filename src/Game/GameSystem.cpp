/* GameSystem (class name from RTTI; Game/GameSystem.h): the base of the game's top-level systems,
 * and the game's start-up and frame functions around them. The current system (lbl_805EF9A8) is
 * initialized on its first frame, ticked through the task tree every frame and deleted once it has
 * finished; the frame time comes from the console clock, held to whole frames of the video mode's
 * refresh rate. The start-up function creates the game's managers and tasks, reads the
 * development defaults (Game/TGameDefaultParam.cpp) and hooks a check of the disc's file list into
 * the file system. File name from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first).
 *
 * NonMatching: 16 of 17 functions match. fn_801E2878 (96%) stores the count before loading the
 * second loop's start, retail the other way round (one instruction pair). The function-local
 * statics come out in definition order, while retail has them in the order the start-up function
 * (fn_801E2234) first calls their functions (fn_801E2178, fn_801E2878, fn_801E2128, fn_801E265C,
 * then fn_801E1BD4), with the instance pointers and lbl_805EF9E8 after them; .sbss and .bss
 * differ by that order. */
#include "Game/GameSystem.h"
#include "Game/NowLoading.h"
#include "Game/Task.h"
#include "Game/Thread.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>

/* RenderWare's video mode description (RwVideoMode; see Game/DiscErrorHandler.cpp). */
struct UnkVideoMode_8032C190 {
    /* 0x00 */ s32 m_width;
    /* 0x04 */ s32 m_height;
    /* 0x08 */ s32 m_depth;
    /* 0x0C */ s32 m_flags;
    /* 0x10 */ s32 m_refRate;
    /* 0x14 */ s32 m_format;
};
SIZE_ASSERT(UnkVideoMode_8032C190, 0x18);

/* The game state (struct name unknown; see Game/TGi.h), as far as this unit uses it. */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ s32 m_frames;       /* frames to advance (meaning inferred) */
    /* 0x10 */ s32 m_minFrames;    /* frames to wait at least (meaning inferred) */
    /* 0x14 */ u8 m_noWait;        /* meaning inferred */
    /* 0x15 */ u8 field_0x15[0x34 - 0x15];
    /* 0x34 */ s32 m_maxFrames;    /* meaning inferred */
    /* 0x38 */ u8 field_0x38[0x44 - 0x38];
    /* 0x44 */ s32 m_width;
    /* 0x48 */ s32 m_height;
    /* 0x4C */ s32 m_depth;
    /* 0x50 */ s32 m_refRate;
    /* 0x54 */ u8 m_exclusive;     /* meanings of the flags inferred */
    /* 0x55 */ u8 field_0x55;
    /* 0x56 */ u8 m_interlaced;
    /* 0x57 */ u8 m_fieldInterlaced;
    /* 0x58 */ u8 field_0x58[0x82 - 0x58];
    /* 0x82 */ u8 m_running;       /* meaning inferred */
    /* 0x83 */ u8 field_0x83[0x150 - 0x83];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The file system (see Game/ActStageController.cpp): its second word is the check of whether a
 * file exists. */
class UnkFileSystem_800371D8 {
  public:
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ bool (*exists)(const char *path);
};

/* The development defaults (Game/TGameDefaultParam.cpp). */
class TGameDefaultParam {
  public:
    void Initialize();
    bool Load(const char *path);
};

/* The pads (struct name unknown; see Game/AdvBattleMode.cpp). */
struct UnkPads_80051734 {
    void Init();
    void Reset();
};

/* The sound manager (see Game/SoundManager.h). */
class UnkSound_80053164 {
  public:
    void Init();
    void LoadBanks();
};

class GamePeripheralTask {
  public:
    GamePeripheralTask(Task *parent);

    /* 0x00 */ u8 field_0x0[0x28];
};

class AudioManagerTask {
  public:
    AudioManagerTask(Task *parent);

    /* 0x00 */ u8 field_0x0[0x28];
};

class AudioLoopManagerTask {
  public:
    AudioLoopManagerTask(Task *parent);

    /* 0x00 */ u8 field_0x0[0x28];
};

namespace SonicteamUSA {
namespace System {
class DebugDisp {
  public:
    DebugDisp();

    /* 0x00 */ u8 field_0x0[0x38];
};
} // namespace System
} // namespace SonicteamUSA

namespace Chao {
namespace CSD {
class CScene {
  public:
    static void SetFrameTime(float time);
};
} // namespace CSD
} // namespace Chao

/* The managers whose instances other units reach through these functions (see
 * Game/DeltaMorphAnimManagerTask.cpp, Game/GadgetModelInfo.cpp). */
void *GetDeltaMorphAnimManager();
void *GetUVAnimManager();

/* The effect systems (class names from RTTI): the platform-independent interface, its RenderWare
 * version, the game's version and the version the game uses. Only the game's version has bodies
 * (in another unit; names of the functions inferred). */
namespace Effect {
namespace System {
class EffectSystem {
  public:
    EffectSystem() {}
    virtual void *Initialize(void *heap, u32 size) = 0;
    virtual void *Finalize() = 0;
    virtual void *Allocate(u32 size) = 0;
    virtual void Free(void *memory) = 0;
    virtual void Fill(void *memory, int value, u32 size) = 0;
    virtual void Copy(void *to, const void *from, u32 size) = 0;
    virtual void Flush(void *memory) = 0;

    /* 0x4 */ u32 field_0x4;
};
} // namespace System

namespace RW3 {
class EffectSystemRW3 : public System::EffectSystem {
  public:
    EffectSystemRW3() {}
    virtual void *GetCamera() = 0;
};
} // namespace RW3
} // namespace Effect

namespace SonicteamUSA {
namespace System {
namespace Effect {
class EffectSystemUSA : public ::Effect::RW3::EffectSystemRW3 {
  public:
    EffectSystemUSA() {}
    virtual void *Initialize(void *heap, u32 size);
    virtual void *Finalize();
    virtual void *Allocate(u32 size);
    virtual void Free(void *memory);
    virtual void Fill(void *memory, int value, u32 size);
    virtual void Copy(void *to, const void *from, u32 size);
    virtual void Flush(void *memory);
    virtual void *GetCamera();

    /* 0x8 */ void *m_heap;
    /* 0xC */ void *m_allocator;
};
} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

namespace Effect {
namespace PJS {
class EffectSystemPJS : public SonicteamUSA::System::Effect::EffectSystemUSA {
  public:
    EffectSystemPJS() {}
};
} // namespace PJS
} // namespace Effect

/* The disc's file list, sorted by name (a table of another unit), and the index into it by first
 * letter (name inferred). */
extern const char *lbl_8053E530[];
extern const s32 lbl_805F6498;
struct UnkFileTable_80583AF8 {
    /* 0x00 */ s32 m_first[26];
    /* 0x68 */ s32 m_count;
};
extern UnkFileTable_80583AF8 lbl_80583AF8;

/* The index of the file list by first letter (struct name unknown): its constructor fills
 * lbl_80583AF8 with the first entry of each letter and, after them, the count. */
struct UnkFileIndex_801E2878 {
    UnkFileIndex_801E2878() {
        memset(&lbl_80583AF8, 0xFF, sizeof(lbl_80583AF8));
        s32 count = lbl_805F6498;
        for (int i = count - 1; i >= 0; i--) {
            char c = lbl_8053E530[i][0];
            if (c >= 'A' && c <= 'Z') {
                lbl_80583AF8.m_first[c - 'A'] = i;
            }
        }
        lbl_80583AF8.m_count = count;
        for (int i = 25; i >= 0; i--) {
            if (lbl_80583AF8.m_first[i] < 0) {
                lbl_80583AF8.m_first[i] = lbl_80583AF8.m_first[i + 1];
            }
        }
    }
};

/* The manager of the systems (class unknown; see Game/WaitInitialize.cpp). */
class UnkSystemManager_801E2128 {
  public:
    /* 0x0 */ u8 field_0x0[0x8];
};

/* The object fn_800BADE8 returns (class unknown; see Game/TPauseManager.cpp). */
struct UnkObject_800BADE8 {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 field_0x30;
};

class TPauseManager;

/* An object of another unit (struct name unknown): a byte at 0x8 it clears (fn_801E29CC). */
struct UnkObject_801E29CC {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ u8 field_0x8;
};

/* The object fn_800122D4 returns (struct name unknown). */
struct UnkObject_800122D4 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ s32 field_0x14;
};

/* The object fn_80046178 returns (struct name unknown). */
struct UnkObject_80046178 {
    /* 0x00 */ u8 field_0x0[0x3C];
    /* 0x3C */ void *field_0x3C;
};

extern "C" {
/* The game state's helpers and the console (behaviour inferred). */
u64 fn_80051A94(void);
void fn_8040305C(void);
void fn_80203A10(void *object, s32 frames, float dt);
void fn_80336FF8(void *object, u8 value);
s32 fn_80485E70(void);
void fn_80485DE4(UnkVideoMode_8032C190 *mode, s32 index);
s32 fn_80305BE8(void);
void fn_8032BB78(bool noReset);
void fn_804157E8(float frames);

/* The task tree. */
Task *fn_8001C4F8(void);
Task *fn_801EB304(Task *root, int layer);
void fn_801EB370(Task *root, float dt);

void fn_801E3EC4(TPauseManager *manager);
void fn_801E3F24(void);
UnkObject_800BADE8 *fn_800BADE8(void);
void fn_801E2F28(UnkObject_800BADE8 *object);
void fn_801E2EDC(UnkObject_800BADE8 *object);
void fn_800436FC(void);
void fn_801E59B8(UnkSystemManager_801E2128 *manager);
void fn_801E5860(UnkSystemManager_801E2128 *manager);
void fn_801E590C(UnkSystemManager_801E2128 *manager, int value);
void fn_801E59C4(void);
UnkSystemManager_801E2128 *fn_801E2128(void);

UnkFileSystem_800371D8 *fn_800371D8(void);
void fn_80045220(UnkFileSystem_800371D8 *fileSystem, Task *task);
TGameDefaultParam *fn_800CBAA4(void);
void fn_800CBB10(UnkGameState_8057D6A0 *state);
UnkPads_80051734 *fn_80051734(void);
UnkSound_80053164 *fn_80053164(void);
void fn_801D654C(UnkSound_80053164 *sound);
void fn_801D656C(UnkSound_80053164 *sound);
void fn_801D658C(UnkSound_80053164 *sound);
void fn_801D6760(UnkSound_80053164 *sound);

void fn_804743AC(void);
void fn_8003723C(void);
void fn_80044148(void);
void fn_8004EA40(void);
void fn_8004EA88(u32 *size);
void fn_80045A00(void);
void *fn_80046178(void);
void fn_80117514(void);
void fn_802E59A0(void);
void fn_803563C0(void);
void fn_80177F38(void);
void fn_8034B4F4(void);
void fn_801D0AD8(void);
void fn_80043908(void *task, Task *parent);
void fn_8003727C(Task *task, int pal);
void fn_8003F8D4(void);
void fn_802BEBE0(s32 stage);
void fn_80170234(void);
UnkObject_800122D4 *fn_800122D4(void);
void fn_800123B8(void *task, Task *parent);
void fn_8003A840(void);
void fn_8004414C(Task *task, void *object);
void fn_801EB50C(void *task, Task *parent);
void fn_801DDEF0(void);
void fn_800AD3A8(int kinds);
void fn_801D2DE8(int arg0);
void fn_8042A5E0(Effect::System::EffectSystem *system);
void fn_800A31F0(void);
void **fn_8000DCC0(void);
void fn_80474258(void *object);
void fn_8038D030(void);
}

/* The movie's status (see Game/DiscErrorHandler.cpp; only tested here). */
extern void *lbl_805EE0B0;
extern u8 lbl_805EE1DC;
extern float lbl_805EE1A8;
extern Task *lbl_805EE190;
extern TPauseManager *lbl_805EF9F4;
/* The manager's instance pointer (a variable of another unit). */
extern UnkSystemManager_801E2128 *lbl_805EF338;

/* The current system and whether it was initialized (names inferred). */
GameSystem *lbl_805EF9A8;
u8 lbl_805EF9AC;
/* The time of the frame and the time since the last one, in seconds, and a fixed frame time that
 * overrides them when positive (names inferred). */
float lbl_805EF9B0;
float lbl_805EF9B4;
float lbl_805EF9B8;
/* Shared with the system manager's unit (name and meaning unknown). */
u32 lbl_805EF9E8[2];

/* Clears the byte at 0x8 of an object of another unit (name inferred). */
extern "C" void fn_801E29CC(UnkObject_801E29CC *object) {
    object->field_0x8 = 0;
}

GameSystem::GameSystem() {
    lbl_805EF9A8 = this;
    lbl_805EF9AC = 0;
    lbl_805EF9B8 = 0.0f;
}

GameSystem::~GameSystem() {
    lbl_805EF9A8 = 0;
    lbl_805EF9AC = 0;
}

/* The file index (name inferred). */
extern "C" UnkFileIndex_801E2878 *fn_801E2878(void) {
    static UnkFileIndex_801E2878 s_index;
    return &s_index;
}

extern "C" UnkFileIndex_801E2878 *fn_801E2844(void) {
    static UnkFileIndex_801E2878 *s_instance;
    if (s_instance == 0) {
        s_instance = fn_801E2878();
    }
    return s_instance;
}

/* Whether the disc's file list has path, in upper case with '/' separators (name inferred). */
extern "C" bool fn_801E2718(const char *path) {
    char name[0x80];
    for (int i = 0; i < 0x80; i++) {
        if (path[i] >= 'a' && path[i] <= 'z') {
            name[i] = path[i] - 0x20;
        } else if (path[i] == '\\') {
            name[i] = '/';
        } else {
            name[i] = path[i];
        }
        if (name[i] == '\0') {
            break;
        }
    }
    s32 first;
    s32 end;
    if (name[0] < 'A') {
        first = 0;
        end = lbl_80583AF8.m_first[1];
    } else {
        first = lbl_80583AF8.m_first[name[0] - 'A'];
        end = lbl_80583AF8.m_first[name[0] - 'A' + 1];
    }
    if (first == -1) {
        return false;
    }
    for (int i = first; i < end; i++) {
        if (strcmp(lbl_8053E530[i], name) == 0) {
            return true;
        }
    }
    return false;
}

/* The loading screen (name inferred). */
extern "C" NowLoadingScreen *fn_801E265C(void) {
    static NowLoadingScreen *s_instance;
    if (s_instance == 0) {
        static NowLoadingScreen s_screen;
        s_instance = &s_screen;
    }
    return s_instance;
}

/* Reads the video mode into the game state (name inferred). */
inline void ReadVideoMode() {
    UnkGameState_8057D6A0 *state = &lbl_8057D6A0;
    s32 index = fn_80485E70();
    if (index >= 0) {
        UnkVideoMode_8032C190 mode;
        fn_80485DE4(&mode, index);
        state->m_width = mode.m_width;
        state->m_height = mode.m_height;
        state->m_depth = mode.m_depth;
        state->m_refRate = mode.m_refRate;
        if (mode.m_refRate == 0) {
            if (fn_80305BE8() != 0) {
                state->m_refRate = 50;
            } else {
                state->m_refRate = 60;
            }
        }
        state->m_exclusive = mode.m_flags & 1;
        state->m_interlaced = (mode.m_flags & 0x300) != 0;
        state->m_fieldInterlaced = (mode.m_flags >> 2) & 1;
    }
}

/* Starts the game (name inferred). */
extern "C" bool fn_801E2234(void) {
    ReadVideoMode();
    fn_801E2178();
    u32 size = 0x70000;
    fn_8004EA88(&size);
    fn_80045A00();
    fn_8001C4F8();
    fn_80046178();
    fn_80117514();
    fn_802E59A0();
    Task *task = fn_801EB304(fn_8001C4F8(), 0);
    fn_80045220(fn_800371D8(), task);
    fn_800CBAA4()->Initialize();
    fn_800CBAA4()->Load("nukkoro.inf");
    fn_800CBAA4()->Load("nukkoro2.inf");
    fn_801E2844();
    UnkFileSystem_800371D8 *fileSystem = fn_800371D8();
    fileSystem->exists = fn_801E2718;
    fn_800CBB10(&lbl_8057D6A0);
    lbl_805EE190 = fn_801EB304(fn_8001C4F8(), 0);
    fn_80051734()->Init();
    new GamePeripheralTask(fn_801EB304(fn_8001C4F8(), 0));
    fn_803563C0();
    fn_80177F38();
    fn_8034B4F4();
    fn_801D0AD8();
    fn_80053164()->Init();
    new AudioManagerTask(fn_801EB304(fn_8001C4F8(), 1));
    new AudioLoopManagerTask(fn_801EB304(fn_8001C4F8(), 1));
    void *memory = operator new(0x38);
    if (memory != 0) {
        fn_80043908(memory, fn_801EB304(fn_8001C4F8(), 0));
    }
    fn_80053164()->LoadBanks();
    fn_801D656C(fn_80053164());
    if (lbl_8057D6A0.m_refRate == 50) {
        fn_8003727C(fn_801EB304(fn_8001C4F8(), 3), 1);
    } else {
        fn_8003727C(fn_801EB304(fn_8001C4F8(), 3), 0);
    }
    fn_801E590C(fn_801E2128(), 1);
    fn_8003F8D4();
    fn_801E265C()->Load();
    new NowLoading(fn_801EB304(fn_8001C4F8(), 0));
    fn_802BEBE0(-1);
    fn_801E3F24();
    fn_80170234();
    UnkObject_800122D4 *object122D4 = fn_800122D4();
    object122D4->field_0x14 = 0x17;
    memory = operator new(0x28);
    if (memory != 0) {
        fn_800123B8(memory, fn_801EB304(fn_8001C4F8(), 0));
    }
    new SonicteamUSA::System::DebugDisp();
    fn_8003A840();
    UnkObject_80046178 *object = static_cast<UnkObject_80046178 *>(fn_80046178());
    fn_8004414C(fn_801EB304(fn_8001C4F8(), 0), object->field_0x3C);
    GetUVAnimManager();
    memory = operator new(0x28);
    if (memory != 0) {
        fn_801EB50C(memory, fn_801EB304(fn_8001C4F8(), 0));
    }
    GetDeltaMorphAnimManager();
    fn_801DDEF0();
    fn_800AD3A8(14);
    fn_801D2DE8(0);
    fn_8042A5E0(new Effect::PJS::EffectSystemPJS());
    fn_800A31F0();
    fn_80474258(*fn_8000DCC0());
    fn_801E59C4();
    fn_8038D030();
    return true;
}

/* Stops the game's services (name inferred). */
extern "C" bool fn_801E21E4(void) {
    fn_804743AC();
    fn_8003723C();
    fn_80044148();
    fn_801D654C(fn_80053164());
    fn_801D658C(fn_80053164());
    fn_801D6760(fn_80053164());
    fn_80051734()->Reset();
    fn_8004EA40();
    return true;
}

extern "C" UnkThreadList_801E2178 *fn_801E2178(void) {
    static UnkThreadList_801E2178 *s_instance;
    if (s_instance == 0) {
        static UnkThreadList_801E2178 s_list;
        s_instance = &s_list;
    }
    return s_instance;
}

/* The system manager (name inferred). */
extern "C" UnkSystemManager_801E2128 *fn_801E2128(void) {
    if (lbl_805EF338 == 0) {
        static s8 s_constructed;
        static UnkSystemManager_801E2128 s_manager;
        if (!s_constructed) {
            fn_801E59B8(&s_manager);
            s_constructed = true;
        }
        lbl_805EF338 = &s_manager;
    }
    return lbl_805EF338;
}

extern "C" void fn_801E1BD4(void);

/* Runs one frame of the current system; returns whether it is still running (name inferred). */
extern "C" bool fn_801E1EB8(void) {
    UnkGameState_8057D6A0 *state = &lbl_8057D6A0;
    fn_8032BB78(state->m_running == 0);
    state->m_running = 1;
    if (!lbl_805EF9AC) {
        if (!lbl_805EF9A8->Initialize()) {
            return false;
        }
        fn_801EB370(fn_8001C4F8(), lbl_805EF9B0);
        lbl_805EF9AC = 1;
        return true;
    }
    ReadVideoMode();
    fn_801E2178()->Reap();
    fn_801E1BD4();
    lbl_805EE1A8 = lbl_805EF9B4;
    Chao::CSD::CScene::SetFrameTime(lbl_805EF9B0);
    s32 frames = lbl_8057D6A0.m_frames;
    if (frames > lbl_8057D6A0.m_maxFrames) {
        frames = lbl_8057D6A0.m_maxFrames;
    }
    fn_804157E8(60.0f / lbl_8057D6A0.m_refRate * frames);
    fn_801EB370(fn_8001C4F8(), lbl_805EF9B0);
    fn_801E3EC4(lbl_805EF9F4);
    if (lbl_805EF9A8->IsFinished()) {
        if (!lbl_805EF9A8->Finalize()) {
            for (;;) {
            }
        }
        delete lbl_805EF9A8;
        lbl_805EF9A8 = 0;
        fn_801EB370(fn_8001C4F8(), lbl_805EF9B0);
        fn_801EB370(fn_8001C4F8(), lbl_805EF9B0);
        fn_801EB370(fn_8001C4F8(), lbl_805EF9B0);
        fn_800436FC();
        fn_801E5860(fn_801E2128());
        UnkObject_800BADE8 *object = fn_800BADE8();
        fn_801E2F28(object);
        fn_801E2EDC(object);
        object->field_0x30 = 0;
        return true;
    }
    return true;
}

/* Measures the frame time (name inferred; Game/DiscErrorHandler.cpp calls it too). */
extern "C" void fn_801E1BD4(void) {
    static bool s_first = true;
    static u64 s_last;
    u64 now;
    s32 frame = 1000000.0f * (1.0f / lbl_8057D6A0.m_refRate);
    float minTime = 0.000001f * frame;
    float maxTime = minTime * lbl_8057D6A0.m_maxFrames;
    if (s_first) {
        s_last = fn_80051A94();
        s_first = false;
    }
    u64 start = (s_last + frame - 1) / frame * frame;
    now = fn_80051A94();
    u64 elapsed = (now + frame - 1) / frame * frame - start;
    if (lbl_805EE0B0 != 0) {
        fn_8040305C();
    }
    if (!lbl_8057D6A0.m_noWait) {
        while (elapsed < lbl_8057D6A0.m_minFrames * frame) {
            now = fn_80051A94();
            elapsed = (now + frame - 1) / frame * frame - start;
        }
    }
    lbl_805EF9B0 = 0.000001f * elapsed;
    lbl_805EF9B4 = 0.000001f * (now - s_last);
    s_last = now;
    if (lbl_805EF9B8 > 0.0f) {
        lbl_805EF9B0 = lbl_805EF9B8;
        lbl_805EF9B4 = lbl_805EF9B8;
    }
    if (lbl_805EF9B0 > maxTime) {
        lbl_805EF9B0 = maxTime;
    } else if (lbl_805EF9B0 < minTime) {
        lbl_805EF9B0 = minTime;
    }
    fn_80203A10(&lbl_8057D6A0.field_0x0[0xC], elapsed / frame, lbl_805EF9B0);
    fn_80336FF8(&lbl_8057D6A0.field_0x83[0x88 - 0x83], lbl_805EE1DC);
}
