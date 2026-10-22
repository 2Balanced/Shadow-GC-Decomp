/* TPjsEventActionManager (class name from RTTI): the task that plays an event (a cutscene). It
 * finds the event's scene files event\eventNNNN_sceneX.one, keeps the scene playing and the next
 * one loaded in two slots, starts the event's music once the first scene is ready, and ends the
 * event when the last scene is over or the player skips it. The event's script,
 * event/eventNNNN.scr, is a list of timed cues (hints, sounds, voices, fades, far clip changes)
 * that the scene playing runs through fn_80173CD4. File name inferred from the class. Names marked
 * "(name inferred)" are not original; the functions other units call keep their fn_ names.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/FogTable.h"
#include "Game/PjsEventAction.h"
#include "Game/Task.h"
#include <cstdio>
#include <dolphin/types.h>
#include "size_assert.h"

/* An unused table at the start of the unit's .rodata (meaning unknown). */
extern const float lbl_804C3C90[19] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                       1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

/* Included after the table: the RTTI names its casts emit follow it in .rodata. */
#include "Game/TNumEdit.h"

/* The MSL variable argument list (layout of the PowerPC EABI va_list). */
typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} va_list[1];
#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_end(ap) ((void)0)

extern "C" int vsprintf(char *s, const char *format, va_list arg);

/* The game state (struct name unknown; see Game/CityBombSmall.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x10];
    /* 0x010 */ s32 field_0x10; /* set to 2 while an event plays (meaning unknown) */
    /* 0x014 */ u8 field_0x14[0x34 - 0x14];
    /* 0x034 */ s32 field_0x34; /* 0x3C while an event plays, 3 after (meaning unknown) */
    /* 0x038 */ u8 field_0x38[0x5C - 0x38];
    /* 0x05C */ s32 m_language; /* nonzero: the English voices (name inferred) */
    /* 0x060 */ u8 field_0x60[0xD1 - 0x60];
    /* 0x0D1 */ u8 m_canSkipEvents; /* (name inferred) */
    /* 0x0D2 */ u8 field_0xD2[0x124 - 0xD2];
    /* 0x124 */ u8 m_keepEventMusic; /* the event's music goes on after it (name inferred) */
    /* 0x125 */ u8 field_0x125[0x150 - 0x125];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The pad state fn_80051734 returns (struct name unknown; see Game/StageSequenceMovieTest.cpp). */
struct UnkPad_80051734 {
    /* 0x00 */ u8 field_0x0[0xD0];
    /* 0xD0 */ u32 m_pressed; /* 0x400: the skip button (meaning inferred) */
};

/* The object the first entry of lbl_8051CE88 points to (class name unknown; see
 * Game/WarpHole.cpp). */
struct UnkBlockManage_8051CE88 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 field_0x40;
    /* 0x044 */ u8 field_0x44[0x978 - 0x44];
    /* 0x978 */ u8 field_0x978;
};

extern UnkBlockManage_8051CE88 *lbl_8051CE88[4];

/* The camera of the screen (class unknown; see Game/TFogSet.cpp). */
struct UnkCamera_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x84];
    /* 0x84 */ float m_farClip; /* (name inferred) */
};

/* What fn_8000DCC0 returns (see Game/ScreenViewport.cpp), reduced to the camera. */
struct UnkScreenHolder_8000DCC0 {
    /* 0x0 */ UnkCamera_8000DCC0 *m_camera;
};

/* The file system fn_800371D8 returns (class name unknown; see Game/GadgetStageData.cpp): the
 * function at 0x4 tells whether a file exists. */
struct UnkFileSystem_800371D8 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ bool (*exists)(const char *path, UnkFileSystem_800371D8 *fileSystem);
};

/* The file system (behaviour inferred). */
extern "C" UnkFileSystem_800371D8 *fn_800371D8(void);

/* Whether file path exists (name inferred; see Game/GadgetStageData.cpp). */
inline bool FileExists(const char *path) {
    UnkFileSystem_800371D8 *fileSystem = fn_800371D8();
    return fileSystem->exists(path, fileSystem);
}

/* The hint manager (class name unknown; see HintRing.cpp). Only the member used here is laid
 * out. */
class UnkHintManager_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ s32 field_0x24; /* nonzero while a hint is shown (meaning inferred) */
};

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

/* The frame effects (class name unknown; see Game/ClumpFrameEffects.cpp). */
class UnkFrameEffects_803284B4 {
  public:
    void Start();
    void End();
    void Clear();
};

/* A scene of the event (class name from RTTI; Game/PjsEventActionScene.cpp). Only the members
 * used here are laid out. */
class TPjsEventActionScene : public TPjsEventActionData, public Task {
  public:
    TPjsEventActionScene(Task *parent, s32 sceneNo);

    /* 0x28 */ s32 m_sceneNo;
    /* 0x2C */ float m_startTime;
    /* 0x30 */ float m_length;
    /* 0x34 */ float m_time;
    /* 0x38 */ u8 field_0x38[0x60 - 0x38];
    /* 0x60 */ s32 m_state; /* 7: ready, 8: failed */
    /* 0x64 */ u32 field_0x64;
    /* 0x68 */ u8 m_play;
    /* 0x69 */ u8 field_0x69[0x1C60 - 0x69];
};
SIZE_ASSERT(TPjsEventActionScene, 0x1C60);

/* One cue of an event's script. */
struct UnkEventCue_80173CD4 {
    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ s32 m_frame;
    /* 0x08 */ s32 m_hint;
    /* 0x0C */ s32 m_sound;
    /* 0x10 */ s32 m_voice;
    /* 0x14 */ char m_soundName[0x20];
    /* 0x34 */ s32 m_sceneArg;
    /* 0x38 */ s32 m_sceneFlag;
    /* 0x3C */ s32 m_sceneArg2;
    /* 0x40 */ s32 m_fade;
    /* 0x44 */ s32 m_farClip;
    /* 0x48 */ u8 field_0x48[0x80 - 0x48];
};
SIZE_ASSERT(UnkEventCue_80173CD4, 0x80);

/* An event's script file. */
struct UnkEventScriptData_80173CD4 {
    /* 0x000 */ s32 field_0x0;
    /* 0x004 */ s32 m_count;
    /* 0x008 */ u8 field_0x8[0x180];
    /* 0x188 */ UnkEventCue_80173CD4 m_cues[1];
};

extern "C" {
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
/* RwCameraSetFarClipPlane (by its arguments). */
void fn_8048560C(UnkCamera_8000DCC0 *camera, float farClip);
}

/* The event's script (class name unknown): a function-local static object behind fn_80170D8C. */
class UnkEventScript_80170D8C {
  public:
    bool Load();
    bool Unload();
    int ShowHint(int id);

    static void OnLoaded(FogRequest *request);

    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ s32 m_state; /* 1 loading, 2 loaded */
    /* 0x08 */ UnkEventScriptData_80173CD4 *m_data;
    /* 0x0C */ float m_savedFarClip;
    /* 0x10 */ char m_fileName[0x24];
};

extern "C" {
/* The fader: whether a fade is running, and the fades (behaviour inferred; see
 * Game/AdvMemoryCheck.cpp). */
int fn_80044E10(void);
void *fn_80044254(void);
void fn_800447B8(void *fader, int arg1, float time);
void fn_800446D0(void *fader, int arg1, int arg2, float time);
/* The fade locks (Game/PjsEventActionFade.cpp). */
void fn_80328618(void);
void fn_80328640(void);
void fn_80328DDC(int type);
UnkPad_80051734 *fn_80051734(void);

/* The sound manager: music, voices and sounds (behaviour inferred). */
void *fn_80053164(void);
void fn_801D524C(void *manager, const char *name);
void fn_801D50F4(void *manager, const char *name);
void fn_801D522C(void *manager);
void fn_801D50D4(void *manager);
void fn_801D4C18(void *manager);
void fn_801D4C38(void *manager, int volume);
void fn_801D51A0(void *manager, int id, int arg2, int arg3);
void fn_801D560C(void *manager, int id, int arg2, int arg3);
void fn_801D5138(void *manager, const char *name);
int fn_801D7B0C(void);

/* The event scene's statics and helpers (Game/PjsEventActionScene.cpp). */
UnkFrameEffects_803284B4 *fn_8016F990(void);
void *fn_801702E4(void);
UnkEventScript_80170D8C *fn_80170D8C(void);
void fn_80172EE8(TPjsEventActionManager *manager);
void fn_801712F4(TPjsEventActionScene *scene, int arg1);
bool fn_80171124(TPjsEventActionScene *scene);
void fn_80170C58(TPjsEventActionScene *scene, int arg1, bool arg2, int arg3);
void fn_802DFBCC(void *arg0);
void fn_802DFC78(void *arg0);

/* A load request to the file system, and the release of loaded data (see Game/FogTable.cpp). */
bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, FogRequest *request, int arg2);
void fn_80049B34(void *data);
/* Byte-swaps a value of size bytes (behaviour inferred). */
void fn_80043D0C(void *value, int size);

/* The task registry (behaviour inferred). */
void *fn_80014B90(void);
void fn_8025D144(void *registry, Task *task);
void fn_8025D1C4(void *registry, Task *task, int arg2, const char *name);

void fn_80015344(int arg0);
void fn_800C9B84(void);
void fn_800CB4A8(void);
void fn_800CB4D8(void);
void *fn_80074194(void);
void fn_800A8D48(void *arg0);

/* The hint display (see Game/EnemyHint.cpp). */
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);

/* The functions this unit defines that other units call. */
void fn_80174250(int level, const char *format, ...);
UnkEventScript_80170D8C *fn_80174228(UnkEventScript_80170D8C *script);
UnkEventScript_80170D8C *fn_801741E0(UnkEventScript_80170D8C *script, s16 flags);
void fn_80173ED0(UnkEventScriptData_80173CD4 *data);
bool fn_80173CD4(UnkEventScript_80170D8C *script, float time, float prevTime);
void fn_80173834(int arg0, float time);
void fn_80173878(int arg0, int arg1, float time);
void fn_80172F48(TPjsEventActionManager *manager, bool play);
}

/* The event's statics (Game/PjsEventActionScene.cpp; names inferred). */
extern s32 lbl_805EF2A0;                     /* the event */
extern s32 lbl_805EF2A4;                     /* the scene playing */
extern float lbl_805EF2A8;                   /* the music's time */
extern TPjsEventActionScene *lbl_805EF2AC[2]; /* the two scene slots */
extern u8 lbl_805EF2B5;                      /* the game state's word at 0x10 is saved */
extern s32 lbl_805EF2B8;
extern u8 lbl_805EF2BC; /* the event's number is below 950 */
extern u8 lbl_805EF948;
/* The event music's file name. */
extern char lbl_80580240[0x20];

/* The scene playing (name inferred). */
inline TPjsEventActionScene *GetScene(TPjsEventActionManager *manager) {
    TPjsEventActionScene *scene = lbl_805EF2AC[manager->m_sceneIndex];
    return scene ? scene : 0;
}

/* Prints a debug message (does nothing in this build). */
extern "C" void fn_80174250(int level, const char *format, ...) {
    char buffer[0x100];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
}

/* Whether event is one of the short events below 1000 (name inferred). */
bool IsShortEvent(int event) {
    return event < 1000;
}

/* The script's constructor. */
extern "C" UnkEventScript_80170D8C *fn_80174228(UnkEventScript_80170D8C *script) {
    script->m_data = 0;
    return script;
}

/* The script's destructor. */
extern "C" UnkEventScript_80170D8C *fn_801741E0(UnkEventScript_80170D8C *script, s16 flags) {
    if (script != 0) {
        script->Unload();
        if (flags > 0) {
            operator delete(script);
        }
    }
    return script;
}

/* Starts loading the event's script; the far clip plane goes out to 30000 meanwhile. */
bool UnkEventScript_80170D8C::Load() {
    sprintf(m_fileName, "event/event%04d.scr", lbl_805EF2A0);
    FogRequest request;
    request.m_path = m_fileName;
    request.m_data = 0;
    request.m_size = 0;
    request.m_user = this;
    request.m_callback = OnLoaded;
    FogTable::Get()->m_disabled = 1;
    m_savedFarClip = fn_8000DCC0()->m_camera->m_farClip;
    fn_8048560C(fn_8000DCC0()->m_camera, 30000.0f);
    if (fn_80045044(fn_800371D8(), &request, 1)) {
        m_state = 1;
        return true;
    } else {
        m_state = 0;
        return false;
    }
}

/* Releases the script and restores the far clip plane. */
bool UnkEventScript_80170D8C::Unload() {
    m_state = 0;
    FogTable::Get()->m_disabled = 0;
    float farClip = m_savedFarClip;
    UnkCamera_8000DCC0 *camera = fn_8000DCC0()->m_camera;
    fn_8048560C(camera, farClip);
    if (m_data != 0) {
        fn_80049B34(m_data);
        m_data = 0;
        return true;
    }
    return false;
}

/* Called by the file system when the script is loaded: swaps it to the game's byte order. */
void UnkEventScript_80170D8C::OnLoaded(FogRequest *request) {
    UnkEventScript_80170D8C *script = (UnkEventScript_80170D8C *)request->m_user;
    script->m_state = 2;
    script->m_data = (UnkEventScriptData_80173CD4 *)request->m_data;
    UnkEventCue_80173CD4 *cue;
    int i;
    UnkEventScriptData_80173CD4 *data = script->m_data;
    if (data != 0) {
        fn_80043D0C(&data->field_0x0, 4);
        fn_80043D0C(&data->m_count, 4);
        fn_80043D0C(&data->field_0x8, 0x180);
        cue = data->m_cues;
        for (i = 0; i < data->m_count; i++) {
            fn_80043D0C(&cue->field_0x0, 4);
            fn_80043D0C(&cue->m_frame, 4);
            fn_80043D0C(&cue->m_hint, 4);
            fn_80043D0C(&cue->m_sound, 4);
            fn_80043D0C(&cue->m_voice, 4);
            fn_80043D0C(&cue->m_sceneArg, 4);
            fn_80043D0C(&cue->m_sceneFlag, 4);
            fn_80043D0C(&cue->m_sceneArg2, 4);
            fn_80043D0C(&cue->m_fade, 4);
            cue++;
        }
    }
}

/* Swaps a script to the game's byte order. */
extern "C" void fn_80173ED0(UnkEventScriptData_80173CD4 *data) {
    if (data != 0) {
        fn_80043D0C(&data->field_0x0, 4);
        fn_80043D0C(&data->m_count, 4);
        fn_80043D0C(&data->field_0x8, 0x180);
        int i;
        UnkEventCue_80173CD4 *cue = data->m_cues;
        for (i = 0; i < data->m_count; i++) {
            fn_80043D0C(&cue->field_0x0, 4);
            fn_80043D0C(&cue->m_frame, 4);
            fn_80043D0C(&cue->m_hint, 4);
            fn_80043D0C(&cue->m_sound, 4);
            fn_80043D0C(&cue->m_voice, 4);
            fn_80043D0C(&cue->m_sceneArg, 4);
            fn_80043D0C(&cue->m_sceneFlag, 4);
            fn_80043D0C(&cue->m_sceneArg2, 4);
            fn_80043D0C(&cue->m_fade, 4);
            cue++;
        }
    }
}

/* Runs the first cue whose frame lies in (prevTime, time]; false while the script is not loaded.
 * NonMatching (2 instructions): retail loads script->m_state before the stmw of the prologue,
 * GC/2.7 after it; about 20 source shapes left the order unchanged. */
/* Plays sound id, if any (name inferred). */
inline void PlaySound(int id) {
    if (id >= 0) {
        fn_801D51A0(fn_80053164(), id, -1, -1);
    }
}

/* Plays voice id, if any (name inferred). */
inline void PlayVoice(int id) {
    if (id >= 0) {
        fn_801D560C(fn_80053164(), (u16)id, 0, 0);
    }
}

extern "C" bool fn_80173CD4(UnkEventScript_80170D8C *script, float time, float prevTime) {
    if (script->m_state != 2) {
        return false;
    }
    UnkEventCue_80173CD4 *cues = script->m_data->m_cues;
    for (int i = 0; i < script->m_data->m_count; i++) {
        if ((float)cues[i].m_frame > prevTime && (float)cues[i].m_frame <= time) {
            if (cues[i].m_hint >= 0) {
                script->ShowHint(cues[i].m_hint);
            }
            PlaySound(cues[i].m_sound);
            PlayVoice(cues[i].m_voice);
            if (cues[i].m_soundName[0] != 0) {
                fn_801D5138(fn_80053164(), cues[i].m_soundName);
            }
            if (cues[i].m_sceneArg > 0) {
                if (fn_801723E0()->GetTask() != 0) {
                    TPjsEventActionScene *scene = GetScene((TPjsEventActionManager *)fn_801723E0()->GetTask());
                    if (scene != 0) {
                        fn_80170C58(scene, cues[i].m_sceneArg, cues[i].m_sceneFlag != 0, cues[i].m_sceneArg2);
                    }
                }
            }
            if (cues[i].m_fade > 0) {
                fn_80328DDC(cues[i].m_fade);
            }
            int farClip = cues[i].m_farClip;
            if (farClip > 1) {
                fn_8048560C(fn_8000DCC0()->m_camera, (float)farClip);
            }
            return true;
        }
    }
    return true;
}

/* Shows hint message id. */
int UnkEventScript_80170D8C::ShowHint(int id) {
    if (GetHintManager()->field_0x24 != 0) {
        fn_801FAAA4(GetHintManager());
        fn_801FAF00(GetHintManager(), id, 0, 1, 0);
        fn_801FAAB4(GetHintManager());
    }
    return 0;
}

const char *TPjsEventActionManager::s_taskName = "TPjsEventActionManager";

TPjsEventActionManager::TPjsEventActionManager(Task *parent, int eventId) : Task(parent), m_skip(0), m_endCount(0), m_ended(0) {
    m_name = s_taskName;
    fn_800C9B84();
    fn_800CB4D8();
    fn_80172EE8(this);
    lbl_805EF2A0 = eventId;
    FindScenes();
    fn_80170D8C()->Load();
    fn_8016F990()->End();
    fn_80328640();
    m_savedMode = lbl_8057D6A0.field_0x10;
    lbl_805EF2B5 = 1;
    lbl_8057D6A0.field_0x10 = 2;
    lbl_8057D6A0.field_0x34 = 0x3C;
    fn_8025D1C4(fn_80014B90(), this, 1, "EventManager");
    lbl_805EF2BC = lbl_805EF2A0 < 950;
    fn_800A8D48(fn_80074194());
    if (eventId == 0x200B) {
        lbl_8057D6A0.m_keepEventMusic = 0;
    }
    fn_801D4C38(fn_80053164(), -20);
    lbl_805EF948 = 1;
    switch (eventId) {
    case 0x1B59:
    case 0x1B63:
    case 0x1C21:
    case 0x1C2B:
    case 0x1E83:
        fn_80015344(1);
        break;
    }
}

TPjsEventActionManager::~TPjsEventActionManager() {
    fn_80170D8C()->Unload();
    fn_8016F990()->Start();
    fn_801723E0()->m_task = 0;
    Release();
    if (!m_ended || !lbl_8057D6A0.m_keepEventMusic) {
        fn_801D522C(fn_80053164());
        fn_801D50D4(fn_80053164());
    }
    fn_800C9B84();
    fn_800CB4A8();
    if (lbl_805EF2B5) {
        lbl_805EF2B5 = 0;
        lbl_8057D6A0.field_0x10 = m_savedMode;
    }
    lbl_8057D6A0.field_0x34 = 3;
    lbl_805EF2B8 = -1;
    fn_8025D144(fn_80014B90(), this);
    fn_801D4C18(fn_80053164());
    lbl_805EF948 = 0;
    fn_80015344(0);
    if (lbl_8051CE88[0] != 0) {
        lbl_8051CE88[0]->field_0x40 = 2;
    }
}

/* NonMatching (register allocation only): retail keeps this in r29, the scene number in r30 and
 * the format string in r31; GC/2.7 assigns them the other way round. Writing the letter as
 * (char)i + 'A' gives retail's registers but sign-extends before the add; about 25 shapes tried. */
void TPjsEventActionManager::FindScenes() {
    char fileName[0x40];
    m_sceneIndex = 0;
    m_state = 0;
    m_time = 0.0f;
    fn_802DFC78(fn_801702E4());
    m_sceneCount = 0;
    m_nextScene = 0;
    for (int i = 0; i < 12; i++) {
        sprintf(fileName, "event\\event%04d_scene%c.one", lbl_805EF2A0, (char)('A' + i));
        if (FileExists(fileName)) {
            m_scenes[m_sceneCount] = i;
            m_sceneCount++;
        }
    }
    if (m_sceneCount <= 0) {
        m_state = 8;
    }
}

void TPjsEventActionManager::Release() {
    fn_802DFBCC(fn_801702E4());
}

void TPjsEventActionManager::RequestSkip() {
    if (lbl_8057D6A0.m_canSkipEvents) {
        m_skip = 1;
    }
}

/* Fades in (behaviour inferred). */
extern "C" void fn_80173878(int arg0, int arg1, float time) {
    fn_800446D0(fn_80044254(), arg0, arg1, time);
}

/* Fades out (behaviour inferred). */
extern "C" void fn_80173834(int arg0, float time) {
    fn_800447B8(fn_80044254(), arg0, time);
}

void TPjsEventActionManager::Tick(float dt) {
    if (fn_80044E10() == 0 && (fn_80051734()->m_pressed & 0x400) == 0x400 && fn_801723E0()->m_firstTime == 0 &&
        lbl_8057D6A0.m_canSkipEvents) {
        m_skip = 1;
    }
    if (m_skip && m_state == 5) {
        if (IsShortEvent(lbl_805EF2A0)) {
            m_flags |= 1;
            return;
        }
        fn_80328618();
        fn_80173878(0, 1, 1.0f);
        if ((lbl_805EF2AC[(m_sceneIndex + 1) % 2] == 0 || lbl_805EF2AC[OtherIndex()]->m_state == 7) &&
            fn_80044E10() == 1) {
            m_flags |= 1;
            return;
        }
    }
    for (int i = 0; i < m_sceneCount; i++) {
        int index = m_sceneIndex;
        if (lbl_805EF2AC[index] == 0 || m_scenes[i] != lbl_805EF2AC[index]->m_sceneNo) {
            TPjsEventActionScene *other = lbl_805EF2AC[(index + 1) % 2];
            if (other != 0 && m_scenes[i] == other->m_sceneNo) {
                OtherIndex();
            }
        }
    }
    switch (m_state) {
    case 0:
        if (CreateScene(m_scenes[m_nextScene++], 0)) {
            m_state = 1;
            m_wait = 2;
            m_musicWait = 0;
        } else {
            m_state = 8;
        }
        break;
    case 1: {
        TPjsEventActionScene *scene = lbl_805EF2AC[m_sceneIndex];
        if (scene->m_state == 7) {
            scene->m_play = 0;
            m_wait--;
            if (m_wait == 0) {
                m_state = 2;
                lbl_805EF2A8 = 0.0f;
                m_wait = 2;
            }
        } else if (scene->m_state == 8) {
            scene->m_flags |= 1;
            lbl_805EF2AC[m_sceneIndex] = 0;
            m_state = 8;
        }
        break;
    }
    case 2: {
        UnkBlockManage_8051CE88 *blocks = lbl_8051CE88[0];
        lbl_805EF2AC[m_sceneIndex]->m_play = 1;
        if (m_wait == 0) {
            if (!(blocks->field_0x978 | (blocks->field_0x40 != 0))) {
                m_time += lbl_805EF2AC[m_sceneIndex]->m_length;
                lbl_805EF2A4 = lbl_805EF2AC[m_sceneIndex]->m_sceneNo;
                m_state = 3;
                bool battle;
                switch (lbl_805EF2A0) {
                case 0x3B6:
                case 0x3B7:
                case 0x3CA:
                case 0x3CC:
                case 0x3CE:
                case 0x3D0:
                case 0x3D2:
                    battle = true;
                    break;
                default:
                    battle = false;
                    break;
                }
                if (battle) {
                    if (m_battleMusic == 0) {
                        fn_801D524C(fn_80053164(), "sng_Battle1.adx");
                    } else {
                        fn_801D524C(fn_80053164(), "sng_Battle2.adx");
                    }
                } else {
                    if (lbl_8057D6A0.m_language == 0) {
                        sprintf(lbl_80580240, "sng_EV%04d_J.adx", lbl_805EF2A0);
                    } else {
                        sprintf(lbl_80580240, "sng_EV%04d_E.adx", lbl_805EF2A0);
                    }
                    if (lbl_805EF2A0 == 0x200B) {
                        fn_801D50F4(fn_80053164(), lbl_80580240);
                    } else {
                        fn_801D524C(fn_80053164(), lbl_80580240);
                    }
                }
            }
        } else {
            m_wait--;
        }
        break;
    }
    case 3: {
        int status = fn_801D7B0C();
        if ((status != 1 && status != 2) || ++m_musicWait > 100) {
            m_state = 4;
            fn_80173834(1, 0.0f);
            fn_801712F4(lbl_805EF2AC[m_sceneIndex], 1);
            lbl_805EF2AC[m_sceneIndex]->m_play = 0;
        }
        break;
    }
    case 4:
        if (m_nextScene < m_sceneCount) {
            CreateScene(m_scenes[m_nextScene++], -1);
        }
        m_state = 5;
        break;
    case 5:
        if (lbl_805EF2AC[m_sceneIndex] == 0) {
            m_state = 8;
            break;
        }
        if (fn_80171124(lbl_805EF2AC[m_sceneIndex])) {
            TPjsEventActionScene *other = lbl_805EF2AC[(m_sceneIndex + 1) % 2];
            if (other != 0 && other->m_state != 7) {
                break;
            }
            fn_8016F990()->Clear();
            lbl_805EF2AC[m_sceneIndex]->m_flags |= 1;
            lbl_805EF2AC[m_sceneIndex] = 0;
            m_endCount = 0;
            m_sceneIndex = (m_sceneIndex + 1) % 2;
            if (lbl_805EF2AC[m_sceneIndex] != 0) {
                lbl_805EF2AC[m_sceneIndex]->m_startTime = m_time;
                lbl_805EF2A4 = lbl_805EF2AC[m_sceneIndex]->m_sceneNo;
                fn_801712F4(lbl_805EF2AC[m_sceneIndex], 1);
                lbl_805EF2AC[m_sceneIndex]->m_play = 0;
                m_time = lbl_805EF2AC[m_sceneIndex]->m_length;
                if (lbl_805EF2AC[m_sceneIndex]->m_state == 7) {
                    m_state = 4;
                } else {
                    m_state = 7;
                }
            } else {
                m_state = 8;
            }
        }
        if (lbl_805EF2AC[(m_sceneIndex + 1) % 2] == 0) {
            TPjsEventActionScene *scene = lbl_805EF2AC[m_sceneIndex];
            if (scene != 0 && scene->m_length - scene->m_time - scene->m_startTime < 50.0f) {
                m_endCount++;
                if (m_endCount > 2) {
                    m_ended = 1;
                    if (lbl_8057D6A0.m_canSkipEvents) {
                        m_skip = 1;
                    }
                    switch (lbl_805EF2A0) {
                    case 0x10CD:
                    case 0x10D7:
                    case 0x1389:
                    case 0x14B5:
                    case 0x1519:
                    case 0x151A:
                    case 0x17D5:
                    case 0x17D7:
                    case 0x17D8:
                    case 0x1839:
                    case 0x1901:
                    case 0x1B63:
                    case 0x1EE7:
                    case 0x1F46:
                    case 0x1FA5:
                    case 0x1FA7:
                    case 0x1FA8:
                    case 0x1FAA:
                    case 0x200A:
                        lbl_8057D6A0.m_keepEventMusic = 1;
                        break;
                    }
                }
            }
        }
        break;
    case 7:
        if (lbl_805EF2AC[m_sceneIndex]->m_state == 7) {
            m_skip = 1;
            m_state = 4;
            fn_80173878(0, 1, 1.0f);
        }
        break;
    case 8:
        m_flags |= 1;
        break;
    }
}

bool TPjsEventActionManager::CreateScene(int sceneNo, int slot) {
    if (slot < 0) {
        slot = (m_sceneIndex + 1) % 2;
    }
    if (lbl_805EF2AC[slot] == 0) {
        lbl_805EF2AC[slot] = new TPjsEventActionScene(fn_801723E0()->m_task, sceneNo);
        return true;
    }
    return false;
}

int TPjsEventActionManager::OtherIndex() {
    return (m_sceneIndex + 1) % 2;
}

/* Starts or pauses the scene playing (behaviour inferred). */
extern "C" void fn_80172F48(TPjsEventActionManager *manager, bool play) {
    TPjsEventActionScene *scene = lbl_805EF2AC[manager->m_sceneIndex];
    if (scene != 0) {
        if (play) {
            scene->m_play = 1;
        } else {
            scene->m_play = 0;
        }
    }
}
